#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef struct {
    FuriMutex* mutex;
    int mode;
} PluginState;

void vibro_test_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Vibro Modes");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "UP: Pulsed");
    canvas_draw_str(canvas, 2, 34, "LEFT: strong / RIGHT: Soft");
    canvas_draw_str(canvas, 2, 46, "DOWN: Pleasure combo");
    canvas_draw_str(canvas, 2, 58, "OK: Pause");
}

void vibro_test_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void delay(int milliseconds) {
    furi_thread_flags_wait(0, FuriFlagWaitAny, milliseconds);
}

int32_t orgasmotron_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!plugin_state->mutex) {
        FURI_LOG_E("Orgasmatron", "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, vibro_test_draw_callback, NULL);
    view_port_input_callback_set(view_port, vibro_test_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    InputEvent event;
    bool processing = true;
    size_t i = 0;
    while(processing) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 50);
        furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);
        if(event_status == FuriStatusOk) {
            if(event.key == InputKeyBack && event.type == InputTypeShort) {
                //Exit Application
                plugin_state->mode = 0;
                processing = false;
            }
            if(event.key == InputKeyOk &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 0;
            }
            if(event.key == InputKeyLeft &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                notification_message(notification, &sequence_set_green_255);
                plugin_state->mode = 1;
            }
            if(event.key == InputKeyRight &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                notification_message(notification, &sequence_set_green_255);
                plugin_state->mode = 3;
            }
            if(event.key == InputKeyUp &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                notification_message(notification, &sequence_set_green_255);
                plugin_state->mode = 2;
            }
            if(event.key == InputKeyDown &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                notification_message(notification, &sequence_set_green_255);
                plugin_state->mode = 4;
            }
            i = 0;
        }

        if(plugin_state->mode == 0) {
            //Stop Vibration
            if(i == 0) {
                notification_message(notification, &sequence_reset_vibro);
                notification_message(notification, &sequence_reset_green);
                i++;
            }
        } else if(plugin_state->mode == 1) {
            //Full power
            if(i == 0) {
                notification_message(notification, &sequence_set_vibro_on);
                i++;
            }
        } else if(plugin_state->mode == 2) {
            //Pulsed Vibration
            i++;
            if(i == 1) {
                notification_message(notification, &sequence_set_vibro_on);
            }
            if(i == 3) {
                notification_message(notification, &sequence_reset_vibro);
            }
            if(i == 4) {
                i = 0;
            }
        } else if(plugin_state->mode == 3) {
            //Soft power
            i++;
            if(i == 1) {
                notification_message(notification, &sequence_set_vibro_on);
            }
            if(i == 2) {
                notification_message(notification, &sequence_reset_vibro);
                i = 0;
            }
        } else if(plugin_state->mode == 4) {
            //Special Sequence
            i++;
            if(i < 23) {
                if(i % 2) {
                    notification_message(notification, &sequence_set_vibro_on);
                } else {
                    notification_message(notification, &sequence_reset_vibro);
                }
            } else if(i < 40) {
                if(i == 24 || i == 33) {
                    notification_message(notification, &sequence_set_vibro_on);
                } else if(i == 32) {
                    notification_message(notification, &sequence_reset_vibro);
                }
            } else if(i == 41) {
                notification_message(notification, &sequence_reset_vibro);
                i = 0;
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

    return 0;
}
