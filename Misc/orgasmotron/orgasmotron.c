#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <dolphin/dolphin.h>

typedef struct {
    int mode;
} PluginState;

void vibro_test_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Vibro Modes");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "LEFT: strong / RIGHT: Soft");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 34, "UP: Pulsed");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 46, "DOWN Pleasure combo");
    canvas_set_font(canvas, FontSecondary);
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
    FuriMutex* state_mutex;
    state_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    if(!state_mutex) {
        FURI_LOG_E("Orgasmatron", "cannot create mutex");
        return 0;
    }

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, vibro_test_draw_callback, NULL);
    view_port_input_callback_set(view_port, vibro_test_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_display_backlight_on);
    DOLPHIN_DEED(DolphinDeedPluginStart);

    InputEvent event;
    bool processing = true;
    while (processing) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(state_mutex, FuriWaitForever);
        if (event_status == FuriStatusOk) {
            if(event.key == InputKeyBack && event.type == InputTypeShort) {
                //Exit Application
                notification_message(notification, &sequence_reset_vibro);
                notification_message(notification, &sequence_reset_green);
                plugin_state->mode = 0;
                processing = false;
                //break;
            }
            if(event.key == InputKeyOk && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 0;
            }
            if(event.key == InputKeyLeft && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 1;
            }
            if(event.key == InputKeyRight && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 3;
            }
            if(event.key == InputKeyUp && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 2;
            }
            if(event.key == InputKeyDown && (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 4;
            }
        }
        
        if (plugin_state->mode == 0) {
            //Stop Vibration
            notification_message(notification, &sequence_reset_vibro);
            notification_message(notification, &sequence_reset_green);
        } else if (plugin_state->mode == 1) {
            //Full power
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_green_255);
        } else if (plugin_state->mode == 2) {
            //Pulsed Vibration
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_green_255);
            delay(100);
            notification_message(notification, &sequence_reset_vibro);
        } else if (plugin_state->mode == 3) {
            //Soft power
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_green_255);
            delay(50);
            notification_message(notification, &sequence_reset_vibro);
        } else if (plugin_state->mode == 4) {
            //Special Sequence
            for (int i = 0;i < 15;i++) {
                notification_message(notification, &sequence_set_vibro_on);
                notification_message(notification, &sequence_set_green_255);
                delay(50);
                notification_message(notification, &sequence_reset_vibro);
                delay(50);
            }
            for (int i = 0;i < 2;i++) {
                notification_message(notification, &sequence_set_vibro_on);
                notification_message(notification, &sequence_set_green_255);
                delay(400);
                notification_message(notification, &sequence_reset_vibro);
                delay(50);
            }
        }
        furi_mutex_release(state_mutex);
    }
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_mutex_free(state_mutex);

    return 0;
}