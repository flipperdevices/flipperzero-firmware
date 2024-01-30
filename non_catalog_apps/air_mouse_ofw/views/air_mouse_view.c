#include "air_mouse_view.h"
#include <gui/elements.h>
#include "../imu_mouse.h"
#include "air_mouse_ofw_icons.h"

struct AirMouseView {
    View* view;
    void* imu_device;
    ImuThread* imu;
    const ImuHidApi* hid_api;
    AirMouseViewExit exit_callback;
    void* context;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool ok_pressed;
    bool connected;
    bool show_ble_icon;
} AirMouseModel;

static void air_mouse_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    AirMouseModel* model = context;

    canvas_set_color(canvas, ColorBlack);

    if(model->show_ble_icon) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_draw_icon(canvas, 78, 8, &I_Circles_47x47);

    if(model->ok_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 95, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 97, 27, &I_Left_mouse_icon_9x9);
    canvas_set_color(canvas, ColorBlack);

    if(model->up_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 95, 9, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 97, 11, &I_Right_mouse_icon_9x9);
    canvas_set_color(canvas, ColorBlack);

    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 79, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 81, 27, &I_Scroll_icon_9x9);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 17, 12, "Air Mouse");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 56, "Press Back to exit");
}

static bool air_mouse_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    AirMouseView* air_mouse = context;
    bool consumed = false;

    if(event->key == InputKeyOk) {
        if((event->type == InputTypePress) || (event->type == InputTypeRelease)) {
            bool state = (event->type == InputTypePress);
            imu_mouse_key_press(air_mouse->imu, ImuMouseKeyLeft, state);
            with_view_model(
                air_mouse->view, AirMouseModel * model, { model->ok_pressed = state; }, true);
        }
        consumed = true;
    } else if(event->key == InputKeyUp) {
        if((event->type == InputTypePress) || (event->type == InputTypeRelease)) {
            bool state = (event->type == InputTypePress);
            imu_mouse_key_press(air_mouse->imu, ImuMouseKeyRight, state);
            with_view_model(
                air_mouse->view, AirMouseModel * model, { model->up_pressed = state; }, true);
        }
        consumed = true;
    } else if(event->key == InputKeyLeft) {
        if((event->type == InputTypePress) || (event->type == InputTypeRelease)) {
            bool state = (event->type == InputTypePress);
            imu_mouse_scroll_mode(air_mouse->imu, state);
            with_view_model(
                air_mouse->view, AirMouseModel * model, { model->left_pressed = state; }, true);
        }
        consumed = true;
    }

    return consumed;
}

static void air_mouse_view_enter(void* context) {
    furi_assert(context);
    AirMouseView* air_mouse = context;
    furi_assert(air_mouse->imu == NULL);
    air_mouse->imu = imu_start(air_mouse->imu_device, air_mouse->hid_api);
}

static void air_mouse_view_exit(void* context) {
    furi_assert(context);
    AirMouseView* air_mouse = context;
    imu_stop(air_mouse->imu);
    air_mouse->imu = NULL;
    if(air_mouse->exit_callback) {
        air_mouse->exit_callback(air_mouse->context);
    }
}

AirMouseView* air_mouse_view_alloc(AirMouseViewExit exit_callback, void* context) {
    AirMouseView* air_mouse = malloc(sizeof(AirMouseView));
    air_mouse->view = view_alloc();
    air_mouse->exit_callback = exit_callback;
    air_mouse->context = context;
    view_set_context(air_mouse->view, air_mouse);
    view_allocate_model(air_mouse->view, ViewModelTypeLocking, sizeof(AirMouseModel));
    view_set_draw_callback(air_mouse->view, air_mouse_view_draw_callback);
    view_set_input_callback(air_mouse->view, air_mouse_view_input_callback);
    view_set_enter_callback(air_mouse->view, air_mouse_view_enter);
    view_set_exit_callback(air_mouse->view, air_mouse_view_exit);

    with_view_model(
        air_mouse->view, AirMouseModel * model, { model->connected = true; }, true);

    return air_mouse;
}

void air_mouse_view_free(AirMouseView* air_mouse) {
    furi_assert(air_mouse);
    view_free(air_mouse->view);
    free(air_mouse);
}

View* air_mouse_view_get_view(AirMouseView* air_mouse) {
    furi_assert(air_mouse);
    return air_mouse->view;
}

void air_mouse_view_set_device(AirMouseView* air_mouse, void* imu_device) {
    furi_assert(air_mouse);
    air_mouse->imu_device = imu_device;
}

void air_mouse_view_set_hid_api(
    AirMouseView* air_mouse,
    const ImuHidApi* hid,
    bool is_ble_interface) {
    furi_assert(air_mouse);
    air_mouse->hid_api = hid;
    with_view_model(
        air_mouse->view,
        AirMouseModel * model,
        { model->show_ble_icon = is_ble_interface; },
        false);
}

void air_mouse_view_set_connected_status(AirMouseView* air_mouse, bool connected) {
    furi_assert(air_mouse);
    with_view_model(
        air_mouse->view, AirMouseModel * model, { model->connected = connected; }, true);
}
