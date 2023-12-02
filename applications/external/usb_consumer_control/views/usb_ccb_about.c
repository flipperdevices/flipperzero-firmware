#include "usb_ccb_about.h"
#include <furi.h>
#include <furi_hal_usb_hid.h>
#include <gui/elements.h>

struct UsbCcbAbout {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool back_pressed;
    bool connected;
} UsbCcbAboutModel;

static void usb_ccb_about_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 7, "A tool to send USB CCBs,");
    canvas_draw_str(canvas, 0, 16, "to launch & control apps.");
    canvas_draw_str(canvas, 0, 28, "For security research.");
    canvas_draw_str(canvas, 0, 37, "Only use with permission.");
    canvas_draw_str(canvas, 0, 49, "More info at: github.com");
    canvas_draw_str(canvas, 0, 58, "/piraija/usb-hid-and-run");
}

static bool usb_ccb_about_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        furi_hal_hid_kb_release_all();
    }

    return consumed;
}

UsbCcbAbout* usb_ccb_about_alloc() {
    UsbCcbAbout* usb_ccb_about = malloc(sizeof(UsbCcbAbout));
    usb_ccb_about->view = view_alloc();
    view_set_context(usb_ccb_about->view, usb_ccb_about);
    view_allocate_model(usb_ccb_about->view, ViewModelTypeLocking, sizeof(UsbCcbAboutModel));
    view_set_draw_callback(usb_ccb_about->view, usb_ccb_about_draw_callback);
    view_set_input_callback(usb_ccb_about->view, usb_ccb_about_input_callback);

    return usb_ccb_about;
}

void usb_ccb_about_free(UsbCcbAbout* usb_ccb_about) {
    furi_assert(usb_ccb_about);
    view_free(usb_ccb_about->view);
    free(usb_ccb_about);
}

View* usb_ccb_about_get_view(UsbCcbAbout* usb_ccb_about) {
    furi_assert(usb_ccb_about);
    return usb_ccb_about->view;
}

void usb_ccb_about_set_connected_status(UsbCcbAbout* usb_ccb_about, bool connected) {
    furi_assert(usb_ccb_about);
    with_view_model(
        usb_ccb_about->view, UsbCcbAboutModel * model, { model->connected = connected; }, true);
}
