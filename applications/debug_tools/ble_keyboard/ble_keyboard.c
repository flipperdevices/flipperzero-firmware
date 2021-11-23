#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} BleKeyboardEvent;

static void ble_keyboard_render_callback(Canvas* canvas, void* ctx) {
    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "BLE keypad demo");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 63, "Hold [back] to exit");
}

static void ble_keyboard_input_callback(InputEvent* input_event, void* ctx) {
    osMessageQueueId_t event_queue = ctx;

    BleKeyboardEvent event;
    event.type = EventTypeInput;
    event.input = *input_event;
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

int32_t ble_keyboard_app(void* p) {
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(BleKeyboardEvent), NULL);
    furi_check(event_queue);
    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, ble_keyboard_render_callback, NULL);
    view_port_input_callback_set(view_port, ble_keyboard_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    furi_hal_bt_hid_init();

    BleKeyboardEvent event;
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, osWaitForever);

        if(event_status == osOK) {
            if(event.type == EventTypeInput) {
                if(event.input.type == InputTypeLong && event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.key == InputKeyBack) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_ESC);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_ESC);
                    }
                }

                if(event.input.key == InputKeyOk) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_ENTER);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_ENTER);
                    }
                }

                if(event.input.key == InputKeyRight) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_RIGHT_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_RIGHT_ARROW);
                    }
                }

                if(event.input.key == InputKeyLeft) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_LEFT_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_LEFT_ARROW);
                    }
                }

                if(event.input.key == InputKeyDown) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_DOWN_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_DOWN_ARROW);
                    }
                }

                if(event.input.key == InputKeyUp) {
                    if(event.input.type == InputTypePress) {
                        furi_hal_bt_hid_kb_press(KEY_UP_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        furi_hal_bt_hid_kb_release(KEY_UP_ARROW);
                    }
                }
            }
        }
        view_port_update(view_port);
    }

    // remove & free all stuff created by app
    furi_hal_bt_hid_deinit();
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);

    return 0;
}
