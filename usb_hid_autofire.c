#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "version.h"

// Uncomment to be able to make a screenshot
//#define USB_HID_AUTOFIRE_SCREENSHOT

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} UsbMouseEvent;

bool btn_left_autofire = false;

static void usb_hid_autofire_render_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "USB HID Autofire");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 90, 10, "v");
    canvas_draw_str(canvas, 96, 10, VERSION);
    canvas_draw_str(canvas, 0, 22, "Press [ok] for auto left clicking");
    canvas_draw_str(canvas, 0, 34, btn_left_autofire ? "<active>" : "<inactive>");
    canvas_draw_str(canvas, 0, 63, "Press [back] to exit");
}

static void usb_hid_autofire_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    UsbMouseEvent event;
    event.type = EventTypeInput;
    event.input = *input_event;
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

//void wait(int ticks) {
//    for (int i = 0; i < ticks; i++) {}
//}

int32_t usb_hid_autofire_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(UsbMouseEvent));
    furi_check(event_queue);
    ViewPort* view_port = view_port_alloc();

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
#ifndef USB_HID_AUTOFIRE_SCREENSHOT
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
#endif

    view_port_draw_callback_set(view_port, usb_hid_autofire_render_callback, NULL);
    view_port_input_callback_set(view_port, usb_hid_autofire_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    UsbMouseEvent event;
    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 50);

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.key == InputKeyOk && event.input.type == InputTypeRelease) {
                    btn_left_autofire = !btn_left_autofire;
                }
            }
        }

        if(btn_left_autofire) {
            furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
//            wait(100);
            furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
//            wait(100);
        }

        view_port_update(view_port);
    }

    furi_hal_usb_set_config(usb_mode_prev, NULL);

    // remove & free all stuff created by app
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    return 0;
}
