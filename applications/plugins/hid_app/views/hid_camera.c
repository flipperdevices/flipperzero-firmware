#include "hid_camera.h"
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidCamera"

struct HidCamera {
    View* view;
    Hid* hid;
};

typedef struct {
    bool ok_pressed;
    bool connected;
    HidTransport transport;
} HidCameraModel;

static void hid_camera_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidCameraModel* model = context;

    // Header
    if(model->transport == HidTransportBle) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Camera");
    canvas_set_font(canvas, FontSecondary);

    // Ok
    canvas_draw_icon(canvas, 63, 25, &I_Space_65x18);
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 66, 27, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 74, 29, &I_Ok_btn_9x9);
    elements_multiline_text_aligned(canvas, 91, 36, AlignLeft, AlignBottom, "Photo");
    canvas_set_color(canvas, ColorBlack);

    // Back
    canvas_draw_icon(canvas, 74, 49, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 91, 57, AlignLeft, AlignBottom, "Quit");
}

static void hid_camera_process(HidCamera* hid_camera, InputEvent* event) {
    with_view_model(
        hid_camera->view,
        HidCameraModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    hid_hal_consumer_key_press(hid_camera->hid, HID_CONSUMER_VOLUME_INCREMENT);
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyOk) {
                    model->ok_pressed = false;
                    hid_hal_consumer_key_release(hid_camera->hid, HID_CONSUMER_VOLUME_INCREMENT);
                }
            }
        },
        true);
}

static bool hid_camera_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidCamera* hid_camera = context;
    bool consumed = false;

    hid_camera_process(hid_camera, event);

    return consumed;
}

HidCamera* hid_camera_alloc(Hid* hid) {
    HidCamera* hid_camera = malloc(sizeof(HidCamera));
    hid_camera->view = view_alloc();
    hid_camera->hid = hid;
    view_set_context(hid_camera->view, hid_camera);
    view_allocate_model(hid_camera->view, ViewModelTypeLocking, sizeof(HidCameraModel));
    view_set_draw_callback(hid_camera->view, hid_camera_draw_callback);
    view_set_input_callback(hid_camera->view, hid_camera_input_callback);

    with_view_model(
        hid_camera->view, HidCameraModel * model, { model->transport = hid->transport; }, true);

    return hid_camera;
}

void hid_camera_free(HidCamera* hid_camera) {
    furi_assert(hid_camera);
    view_free(hid_camera->view);
    free(hid_camera);
}

View* hid_camera_get_view(HidCamera* hid_camera) {
    furi_assert(hid_camera);
    return hid_camera->view;
}

void hid_camera_set_connected_status(HidCamera* hid_camera, bool connected) {
    furi_assert(hid_camera);
    with_view_model(
        hid_camera->view, HidCameraModel * model, { model->connected = connected; }, true);
}
