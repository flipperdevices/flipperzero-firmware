#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "hid_service.h"

#define BLE_HID_INFO_BASE_USB_SPECIFICATION (0x0101)
#define BLE_HID_INFO_COUNTRY_CODE (0x00)
#define BLE_HID_INFO_FLAG_REMOTE_WAKE_MSK (0x01)
#define BLE_HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK (0x02)

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} BleKeyboardEvent;

typedef struct {
    uint16_t bcd_hid; // Number of base USB HID specification
    uint8_t country_code;
    uint8_t flags;
} BleHidInfo;

static uint8_t report_map_data[] = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x06,       // Usage (Keyboard)
    0xA1, 0x01,       // Collection (Application)
    0x05, 0x07,       // Usage Page (Key Codes)
    0x19, 0xe0,       // Usage Minimum (224)
    0x29, 0xe7,       // Usage Maximum (231)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x01,       // Logical Maximum (1)
    0x75, 0x01,       // Report Size (1)
    0x95, 0x08,       // Report Count (8)
    0x81, 0x02,       // Input (Data, Variable, Absolute)

    0x95, 0x01,       // Report Count (1)
    0x75, 0x08,       // Report Size (8)
    0x81, 0x01,       // Input (Constant) reserved byte(1)

    0x95, 0x05,       // Report Count (5)
    0x75, 0x01,       // Report Size (1)
    0x05, 0x08,       // Usage Page (Page# for LEDs)
    0x19, 0x01,       // Usage Minimum (1)
    0x29, 0x05,       // Usage Maximum (5)
    0x91, 0x02,       // Output (Data, Variable, Absolute), Led report
    0x95, 0x01,       // Report Count (1)
    0x75, 0x03,       // Report Size (3)
    0x91, 0x01,       // Output (Data, Variable, Absolute), Led report padding

    0x95, 0x06,       // Report Count (6)
    0x75, 0x08,       // Report Size (8)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x65,       // Logical Maximum (101)
    0x05, 0x07,       // Usage Page (Key codes)
    0x19, 0x00,       // Usage Minimum (0)
    0x29, 0x65,       // Usage Maximum (101)
    0x81, 0x00,       // Input (Data, Array) Key array(6 bytes)

    0x09, 0x05,       // Usage (Vendor Defined)
    0x15, 0x00,       // Logical Minimum (0)
    0x26, 0xFF, 0x00, // Logical Maximum (255)
    0x75, 0x08,       // Report Size (8 bit)
    0x95, 0x02,       // Report Count (2)
    0xB1, 0x02,       // Feature (Data, Variable, Absolute)

    0xC0              // End Collection (Application)
};

#define BLE_KEYBOARD_MAX_KEYS (6)

typedef struct {
    uint8_t mods;
    uint8_t reserved;
    uint8_t key[BLE_KEYBOARD_MAX_KEYS];
} BleKeyboard;

static BleKeyboard* ble_keyboard = NULL;

bool ble_keyboard_press(uint16_t button) {
    furi_assert(ble_keyboard);
    for (uint8_t i = 0; i < BLE_KEYBOARD_MAX_KEYS; i++) {
        if (ble_keyboard->key[i] == 0) {
            ble_keyboard->key[i] = button & 0xFF;
            break;
        }
    }
    ble_keyboard->mods |= (button >> 8);
    return hid_svc_update_input_report((uint8_t*)ble_keyboard, sizeof(BleKeyboard));
}

bool ble_keyboard_release(uint16_t button) {
    furi_assert(ble_keyboard);
    for (uint8_t i = 0; i < BLE_KEYBOARD_MAX_KEYS; i++) {
        if (ble_keyboard->key[i] == (button & 0xFF)) {
            ble_keyboard->key[i] = 0;
            break;
        }
    }
    ble_keyboard->mods &= ~(button >> 8);
    return hid_svc_update_input_report((uint8_t*)ble_keyboard, sizeof(BleKeyboard));
}

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

    // Setup BLE HID characteristics
    hid_svc_update_report_map(report_map_data, sizeof(report_map_data));
    BleHidInfo hid_info = {
        .bcd_hid = BLE_HID_INFO_BASE_USB_SPECIFICATION,
        .country_code = BLE_HID_INFO_COUNTRY_CODE,
        .flags = BLE_HID_INFO_FLAG_REMOTE_WAKE_MSK | BLE_HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK,
    };
    uint8_t hid_info_val[4] = {};
    hid_info_val[0] = hid_info.bcd_hid & 0x00ff;
    hid_info_val[1] = (hid_info.bcd_hid & 0xff00) >> 8;
    hid_info_val[2] = hid_info.country_code;
    hid_info_val[3] = hid_info.flags;
    hid_svc_update_info(hid_info_val, sizeof(hid_info_val));

    BleKeyboardEvent event;
    ble_keyboard = furi_alloc(sizeof(BleKeyboard));
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, osWaitForever);

        if(event_status == osOK) {
            if(event.type == EventTypeInput) {
                if(event.input.type == InputTypeLong && event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.key == InputKeyBack) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_ESC);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_ESC);
                    }
                }

                if(event.input.key == InputKeyOk) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_ENTER);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_ENTER);
                    }
                }

                if(event.input.key == InputKeyRight) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_RIGHT_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_RIGHT_ARROW);
                    }
                }

                if(event.input.key == InputKeyLeft) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_LEFT_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_LEFT_ARROW);
                    }
                }

                if(event.input.key == InputKeyDown) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_DOWN_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_DOWN_ARROW);
                    }
                }

                if(event.input.key == InputKeyUp) {
                    if(event.input.type == InputTypePress) {
                        ble_keyboard_press(KEY_UP_ARROW);
                    } else if(event.input.type == InputTypeRelease) {
                        ble_keyboard_release(KEY_UP_ARROW);
                    }
                }
            }
        }
        view_port_update(view_port);
    }

    // remove & free all stuff created by app
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);
    free(ble_keyboard);
    ble_keyboard = NULL;

    return 0;
}
