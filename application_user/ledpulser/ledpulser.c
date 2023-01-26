#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

const int color_green = 1;
const int color_blue = 2;
const int color_red = 3;
const int color_white = 4;

const NotificationSequence sequence_solid_white = {
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_do_not_reset,
    NULL,
};

const NotificationMessage message_red_128 = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = 128,
};

const NotificationSequence sequence_half_red = {
    &message_red_128,
    &message_do_not_reset,
    NULL,
};

typedef struct {
    int mode;
    int color;
    int intensity;
    int direction;
} PluginState;

int intensity = 0;

/*
const NotificationMessage message_red_dyn = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = intensity,
};

const NotificationSequence sequence_red_dyn = {
    &message_red_dyn,
    &message_do_not_reset,
    NULL,
};*/

/*
NotificationSequence* generate_led_notification(int color, int intensity) {
    //enum setColor = NotificationMessageTypeLedRed;
    NotificationSequence response = {
        {
            .type = &NotificationMessageTypeLedRed,
            .data.led.value = intensity,
        },
        NULL
    };
    return response;
}*/



void led_test_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "LED Pulser");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "LEFT: Green / RIGHT: Red");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 34, "UP: Blue / Down: White");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 58, "OK: Off");
}

void led_test_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void led_set_light(NotificationApp* notification, NotificationMessage notification_led_message, int intensity) {
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
    //NotificationSequence* dyn_notification_sequence = malloc(sizeof(sequence_half_red));
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("LED Pulser", "cannot create mutex\r\n");
        free(plugin_state);
        free(dyn_notification_message);
        //free(dyn_notification_sequence);
        return 255;
    }
    plugin_state->direction = 0;
    plugin_state->intensity = 0;

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, led_test_draw_callback, NULL);
    view_port_input_callback_set(view_port, led_test_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    InputEvent event;
    //int mode = 0;
    bool processing = true;
    //while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
    while (processing) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);
        NotificationMessage notification_led_message;
        NotificationMessage notification_led_message_1;
        NotificationMessage notification_led_message_2;
        NotificationMessage notification_led_message_3;
        if (event_status == FuriStatusOk) {
            if(event.key == InputKeyBack && event.type == InputTypeShort) {
                //Exit Application
                notification_message(notification, &sequence_reset_green);
                plugin_state->mode = 0;
                processing = false;
                //break;
            }
            if(event.key == InputKeyOk && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 0;
            }
            if(event.key == InputKeyLeft && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = color_red;
                notification_led_message.type = NotificationMessageTypeLedRed;
            }
            if(event.key == InputKeyRight && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = color_green;
                notification_led_message.type = NotificationMessageTypeLedGreen;
            }
            if(event.key == InputKeyUp && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = color_blue;
                notification_led_message.type = NotificationMessageTypeLedBlue;
            }
            if(event.key == InputKeyDown && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = color_white;
            }
        }
        
        if (plugin_state->mode == 0) {
            //Off
            notification_message(notification, &sequence_reset_red);
            notification_message(notification, &sequence_reset_green);
            notification_message(notification, &sequence_reset_blue);
        } else if (plugin_state->mode == color_green || plugin_state->mode == color_blue || plugin_state->mode == color_red) {
            while (true) {
                notification_led_message.data.led.value = plugin_state->intensity;
                const NotificationSequence notification_sequence = {
                    &notification_led_message,
                    &message_do_not_reset,
                    NULL,
                };
                notification_message(notification, &notification_sequence);
                delay(2);
                if (plugin_state->direction == 0) {
                    plugin_state->intensity++;
                    if (plugin_state->intensity >= 255) {
                        plugin_state->direction = 1;
                        break;
                    }
                } else {
                    plugin_state->intensity--;
                    if (plugin_state->intensity <= 0) {
                        plugin_state->direction = 0;
                        break;
                    }
                }
            }
        } else if (plugin_state->mode == color_white) {
            notification_led_message_1.type = NotificationMessageTypeLedRed;
            notification_led_message_2.type = NotificationMessageTypeLedGreen;
            notification_led_message_3.type = NotificationMessageTypeLedBlue;
            
            //White
            while (true) {
                notification_led_message_1.data.led.value = plugin_state->intensity;
                notification_led_message_2.data.led.value = plugin_state->intensity;
                notification_led_message_3.data.led.value = plugin_state->intensity;
                const NotificationSequence notification_sequence = {
                    &notification_led_message_1,
                    &notification_led_message_2,
                    &notification_led_message_3,
                    &message_do_not_reset,
                    NULL,
                };
                notification_message(notification, &notification_sequence);
                delay(2);
                if (plugin_state->direction == 0) {
                    plugin_state->intensity++;
                    if (plugin_state->intensity >= 255) {
                        plugin_state->direction = 1;
                        break;
                    }
                } else {
                    plugin_state->intensity--;
                    if (plugin_state->intensity <= 0) {
                        plugin_state->direction = 0;
                        break;
                    }
                }
            }
        }
        release_mutex(&state_mutex, plugin_state);
    }
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    delete_mutex(&state_mutex);

    return 0;
}