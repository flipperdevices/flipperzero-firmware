#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include "ledpulser_icons.h"

const int color_green = 1;
const int color_blue = 2;
const int color_red = 3;
const int color_yellow = 4;
const int color_white = 5;

typedef struct {
    int mode;
    int color;
    int intensity;
    int direction;
    FuriMutex* mutex;
} PluginState;

int intensity = 0;

void led_test_draw_callback(Canvas* canvas, void* ctx) {
    //UNUSED(ctx);
    furi_assert(ctx);
    PluginState* plugin_state = ctx;
    furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 0, 0, &I_flipper_cross);
    canvas_draw_str(canvas, 79, 35, "Red");
    canvas_draw_str(canvas, 101, 35, "Green");
    canvas_draw_str(canvas, 78, 12, "Blue");
    canvas_draw_str(canvas, 49, 35, "White");
    canvas_draw_str(canvas, 73, 57, "Yellow");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "LED Pulser");

    canvas_set_font(canvas, FontSecondary);
    if(plugin_state->mode == color_yellow) {
        canvas_draw_box(canvas, 71, 48, 31, 11);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 73, 57, "Yellow");
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == color_red) {
        canvas_draw_box(canvas, 77, 26, 20, 11);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 79, 35, "Red");
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == color_blue) {
        canvas_draw_box(canvas, 76, 3, 22, 11);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 78, 12, "Blue");
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == color_green) {
        canvas_draw_box(canvas, 100, 26, 27, 11);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 101, 35, "Green");
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == color_white) {
        canvas_draw_box(canvas, 47, 26, 27, 11);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 49, 35, "White");
        canvas_invert_color(canvas);
    }
    furi_mutex_release(plugin_state->mutex);
}

void led_test_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void led_set_light(
    NotificationApp* notification,
    NotificationMessage notification_led_message,
    int intensity) {
    notification_led_message.data.led.value = intensity;
    const NotificationSequence notification_sequence = {
        &notification_led_message,
        &message_do_not_reset,
        NULL,
    };
    notification_message(notification, &notification_sequence);
}

void delay(int milliseconds) {
    furi_thread_flags_wait(0, FuriFlagWaitAny, milliseconds);
}

int32_t ledpulser_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));
    NotificationMessage* dyn_notification_message = malloc(sizeof(NotificationMessage));
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!plugin_state->mutex) {
        FURI_LOG_E("LED Pulser", "cannot create mutex\r\n");
        free(plugin_state);
        free(dyn_notification_message);
        return 255;
    }
    plugin_state->direction = 0;
    plugin_state->intensity = 0;

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, led_test_draw_callback, plugin_state);
    view_port_input_callback_set(view_port, led_test_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    InputEvent event;
    bool processing = true;
    notification_message(notification, &sequence_display_backlight_on);
    while(processing) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);
        NotificationMessage notification_led_message;
        NotificationMessage notification_led_message_1;
        NotificationMessage notification_led_message_2;
        NotificationMessage notification_led_message_3;
        if(event_status == FuriStatusOk) {
            if(event.key == InputKeyBack && event.type == InputTypeShort) {
                //Exit Application
                notification_message(notification, &sequence_reset_green);
                plugin_state->mode = 0;
                processing = false;
                //break;
            }
            if(event.key == InputKeyOk && event.type == InputTypePress) {
                if(plugin_state->mode == color_red) {
                    plugin_state->mode = 0;
                } else {
                    plugin_state->mode = color_red;
                    notification_led_message.type = NotificationMessageTypeLedRed;
                }
            }
            if(event.key == InputKeyLeft && event.type == InputTypePress) {
                if(plugin_state->mode == color_white) {
                    plugin_state->mode = 0;
                } else {
                    plugin_state->mode = color_white;
                }
            }
            if(event.key == InputKeyRight && event.type == InputTypePress) {
                if(plugin_state->mode == color_green) {
                    plugin_state->mode = 0;
                } else {
                    plugin_state->mode = color_green;
                    notification_led_message.type = NotificationMessageTypeLedGreen;
                }
            }
            if(event.key == InputKeyUp && event.type == InputTypePress) {
                if(plugin_state->mode == color_blue) {
                    plugin_state->mode = 0;
                } else {
                    plugin_state->mode = color_blue;
                    notification_led_message.type = NotificationMessageTypeLedBlue;
                }
            }
            if(event.key == InputKeyDown && event.type == InputTypePress) {
                if(plugin_state->mode == color_yellow) {
                    plugin_state->mode = 0;
                } else {
                    plugin_state->mode = color_yellow;
                }
            }
        }

        if(plugin_state->mode == 0) {
            //Off
            notification_message(notification, &sequence_reset_red);
            notification_message(notification, &sequence_reset_green);
            notification_message(notification, &sequence_reset_blue);
        } else if(
            plugin_state->mode == color_green || plugin_state->mode == color_blue ||
            plugin_state->mode == color_red) {
            while(true) {
                notification_led_message.data.led.value = plugin_state->intensity;
                const NotificationSequence notification_sequence = {
                    &notification_led_message,
                    &message_do_not_reset,
                    NULL,
                };
                notification_message(notification, &notification_sequence);
                delay(2);
                if(plugin_state->direction == 0) {
                    plugin_state->intensity++;
                    if(plugin_state->intensity >= 255) {
                        plugin_state->direction = 1;
                        break;
                    }
                } else {
                    plugin_state->intensity--;
                    if(plugin_state->intensity <= 0) {
                        plugin_state->direction = 0;
                        break;
                    }
                }
            }
        } else if(plugin_state->mode == color_white || plugin_state->mode == color_yellow) {
            notification_led_message_1.type = NotificationMessageTypeLedRed;
            notification_led_message_2.type = NotificationMessageTypeLedGreen;
            notification_led_message_3.type = NotificationMessageTypeLedBlue;

            while(true) {
                notification_led_message_1.data.led.value = plugin_state->intensity;
                notification_led_message_2.data.led.value = plugin_state->intensity;
                if(plugin_state->mode == color_white) {
                    notification_led_message_3.data.led.value = plugin_state->intensity;
                } else {
                    notification_led_message_3.data.led.value = 0;
                }
                const NotificationSequence notification_sequence = {
                    &notification_led_message_1,
                    &notification_led_message_2,
                    &notification_led_message_3,
                    &message_do_not_reset,
                    NULL,
                };
                notification_message(notification, &notification_sequence);
                delay(2);
                if(plugin_state->direction == 0) {
                    plugin_state->intensity++;
                    if(plugin_state->intensity >= 255) {
                        plugin_state->direction = 1;
                        break;
                    }
                } else {
                    plugin_state->intensity--;
                    if(plugin_state->intensity <= 0) {
                        plugin_state->direction = 0;
                        break;
                    }
                }
            }
        }
        furi_mutex_release(plugin_state->mutex);
    }
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_mutex_free(plugin_state->mutex);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(plugin_state);
    return 0;
}