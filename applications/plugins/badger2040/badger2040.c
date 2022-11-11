#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "badger2040.h"

#define ADDRESS_QUESTION_X 5
#define ADDRESS_QUESTION_Y 10
#define DATA_X 5
#define DATA_Y 20
#define ERROR_X 5
#define ERROR_Y 55

#define DEBUG true

#define ERROR

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef enum {
    BadgerGetAddress,
    BadgerSend
} STATE;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    STATE state;
    bool get_failed;
    uint8_t address;
    uint8_t data;
    bool send_failed;
} PluginState;

static void badger2040_state_init(PluginState* const plugin_state) {
    plugin_state->state = BadgerGetAddress;
    plugin_state->get_failed = false;
    plugin_state->address = FIRST_NON_RESERVED_I2C_ADDRESS;
    plugin_state->data = 0;
    plugin_state->send_failed = false;
}

static void render_callback(Canvas* const canvas, void* ctx) {
    PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) {
        return;
    }

    canvas_set_color(canvas, ColorBlack);

    char str[32];

    if (plugin_state->state == BadgerGetAddress) {
        sprintf(str, "Selected Address: %d", plugin_state->address);
        canvas_draw_str(canvas, ADDRESS_QUESTION_X, ADDRESS_QUESTION_Y, str);

        if (plugin_state->get_failed) {
            strcpy(str, "Error! Device not found!");
            canvas_draw_str(canvas, ERROR_X, ERROR_Y, str);
        }
    } else if (plugin_state->state == BadgerSend) {
        sprintf(str, "Sending to Address: %d", plugin_state->address);
        canvas_draw_str(canvas, ADDRESS_QUESTION_X, ADDRESS_QUESTION_Y, str);

        sprintf(str, "Data: %d", plugin_state->data);
        canvas_draw_str(canvas, DATA_X, DATA_Y, str);

        if (plugin_state->send_failed) {
            strcpy(str, "Error! Unable to send data!");
            canvas_draw_str(canvas, ERROR_X, ERROR_Y, str);
        }
    }

    release_mutex((ValueMutex*)ctx, plugin_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

bool device_exists(int address) {
    if (DEBUG) return true;

    uint32_t response_timeout_ticks = furi_ms_to_ticks(5.f);

    return furi_hal_i2c_is_device_ready(
               &furi_hal_i2c_handle_external,
               address << 1,
               response_timeout_ticks);
}

bool sendData(uint8_t address, uint8_t data) {
    if (DEBUG) return true;

    uint32_t response_timeout_ticks = furi_ms_to_ticks(5.f);

    return furi_hal_i2c_tx(&furi_hal_i2c_handle_external, address, &data, 1, response_timeout_ticks);
}

int32_t badger2040_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));
    badger2040_state_init(plugin_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("badger2040", "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if((event.input.type == InputTypePress) || (event.input.type == InputTypeRepeat)) {

                    // Reset failed status to clear display after key press
                    if (plugin_state->state == BadgerGetAddress) {
                        plugin_state->get_failed = false;
                    } else if (plugin_state->state == BadgerSend) {
                        plugin_state->send_failed = false;
                    }

                    switch(event.input.key) {
                    case InputKeyUp:
                        if (plugin_state->state == BadgerGetAddress) {
                            uint8_t prevAddr = plugin_state->address;

                            if (prevAddr == HIGHEST_I2C_ADDRESS) {
                                plugin_state->address = FIRST_NON_RESERVED_I2C_ADDRESS;
                            } else {
                                plugin_state->address = prevAddr + 1;
                            }
                        } else if (plugin_state->state == BadgerSend) {
                            uint8_t prevData = plugin_state->data;

                            if (prevData == MAX_DATA) {
                                plugin_state->data = MIN_DATA;
                            } else {
                                plugin_state->data = prevData + 1;
                            }
                        }
                        break;
                    case InputKeyDown:
                        if (plugin_state->state == BadgerGetAddress) {
                            uint8_t prevAddr = plugin_state->address;

                            if (prevAddr == FIRST_NON_RESERVED_I2C_ADDRESS) {
                                plugin_state->address = HIGHEST_I2C_ADDRESS;
                            } else {
                                plugin_state->address = prevAddr - 1;
                            }
                        } else if (plugin_state->state == BadgerSend) {
                            uint8_t prevData = plugin_state->data;

                            if (prevData == MIN_DATA) {
                                plugin_state->data = MAX_DATA;
                            } else {
                                plugin_state->data = prevData - 1;
                            }
                        }
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyOk:
                        if (plugin_state->state == BadgerGetAddress) {
                            if (device_exists(plugin_state->address)) {
                                if (plugin_state->state == BadgerGetAddress) {
                                    plugin_state->state = BadgerSend;
                                }
                            } else {
                                plugin_state->get_failed = true;
                            }
                        } else if (plugin_state->state == BadgerSend) {
                            if (!sendData) {
                                plugin_state->send_failed = true;
                            }
                        }
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D("badger2040", "osMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    furi_message_queue_free(event_queue);

    return 0;
}