#include "usb_ccb_help.h"
#include <furi.h>
#include <furi_hal_usb_hid.h>
#include <gui/elements.h>

struct UsbCcbHelp {
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
} UsbCcbHelpModel;

static void usb_ccb_help_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 8, "Exit to menu: hold [back].");
    canvas_draw_str(canvas, 0, 18, "Adjust delay between keys:");
    canvas_draw_str(canvas, 0, 26, "push [up] or [down].");
    canvas_draw_str(canvas, 0, 36, "Cycle between keys:");
    canvas_draw_str(canvas, 0, 44, "push/hold [left] or [right].");
    canvas_draw_str(canvas, 0, 54, "Toggle sending keys: push [ok].");
    canvas_draw_str(canvas, 0, 62, "Send one key: double click [ok].");
}

static bool usb_ccb_help_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        furi_hal_hid_kb_release_all();
    }

    return consumed;
}

UsbCcbHelp* usb_ccb_help_alloc() {
    UsbCcbHelp* usb_ccb_help = malloc(sizeof(UsbCcbHelp));
    usb_ccb_help->view = view_alloc();
    view_set_context(usb_ccb_help->view, usb_ccb_help);
    view_allocate_model(usb_ccb_help->view, ViewModelTypeLocking, sizeof(UsbCcbHelpModel));
    view_set_draw_callback(usb_ccb_help->view, usb_ccb_help_draw_callback);
    view_set_input_callback(usb_ccb_help->view, usb_ccb_help_input_callback);

    return usb_ccb_help;
}

void usb_ccb_help_free(UsbCcbHelp* usb_ccb_help) {
    furi_assert(usb_ccb_help);
    view_free(usb_ccb_help->view);
    free(usb_ccb_help);
}

View* usb_ccb_help_get_view(UsbCcbHelp* usb_ccb_help) {
    furi_assert(usb_ccb_help);
    return usb_ccb_help->view;
}

void usb_ccb_help_set_connected_status(UsbCcbHelp* usb_ccb_help, bool connected) {
    furi_assert(usb_ccb_help);
    with_view_model(
        usb_ccb_help->view, UsbCcbHelpModel * model, { model->connected = connected; }, true);
}
