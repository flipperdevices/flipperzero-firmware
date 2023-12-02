#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include "orgasmotron_icons.h"

typedef struct {
    int mode;
    FuriMutex* mutex;
} PluginState;

void vibro_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    const PluginState* plugin_state = ctx;
    furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);
    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, &I_ui_128x64);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 56, 37, &I_left_4x7);
    canvas_draw_str(canvas, 61, 44, "Strong");
    canvas_draw_str(canvas, 97, 44, "Soft");
    canvas_draw_icon(canvas, 117, 37, &I_right_4x7);
    canvas_draw_str(canvas, 76, 33, "Pulsed");
    canvas_draw_icon(canvas, 85, 21, &I_up_7x4);
    canvas_draw_str(canvas, 76, 56, "Combo");
    canvas_draw_icon(canvas, 85, 57, &I_down_7x4);
    canvas_draw_str(canvas, 38, 39, "Off");
    canvas_draw_icon(canvas, 38, 24, &I_center_7x7);
    furi_mutex_release(plugin_state->mutex);

    if(plugin_state->mode == 0) {
        canvas_draw_box(canvas, 36, 23, 15, 18);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 38, 39, "Off");
        canvas_draw_icon(canvas, 38, 24, &I_center_7x7);
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == 1) {
        canvas_draw_box(canvas, 55, 36, 36, 10);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 61, 44, "Strong");
        canvas_draw_icon(canvas, 56, 37, &I_left_4x7);
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == 2) {
        canvas_draw_box(canvas, 74, 20, 30, 16);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 76, 33, "Pulsed");
        canvas_draw_icon(canvas, 85, 21, &I_up_7x4);
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == 3) {
        canvas_draw_box(canvas, 95, 36, 28, 10);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 97, 44, "Soft");
        canvas_draw_icon(canvas, 117, 37, &I_right_4x7);
        canvas_invert_color(canvas);
    } else if(plugin_state->mode == 4) {
        canvas_draw_box(canvas, 74, 47, 32, 16);
        canvas_invert_color(canvas);
        canvas_draw_str(canvas, 76, 56, "Combo");
        canvas_draw_icon(canvas, 85, 57, &I_down_7x4);
        canvas_invert_color(canvas);
    }
}

void vibro_input_callback(InputEvent* input_event, void* ctx) {
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
    view_port_draw_callback_set(view_port, vibro_draw_callback, plugin_state);
    view_port_input_callback_set(view_port, vibro_input_callback, event_queue);

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
        if(event_status == FuriStatusOk) {
            if(event.key == InputKeyBack && event.type == InputTypeShort) {
                //Exit Application
                notification_message(notification, &sequence_reset_vibro);
                notification_message(notification, &sequence_reset_green);
                plugin_state->mode = 0;
                processing = false;
                //break;
            }
            if(event.key == InputKeyOk &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 0;
            }
            if(event.key == InputKeyLeft &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 1;
            }
            if(event.key == InputKeyRight &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 3;
            }
            if(event.key == InputKeyUp &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 2;
            }
            if(event.key == InputKeyDown &&
               (event.type == InputTypePress || event.type == InputTypeRelease)) {
                plugin_state->mode = 4;
            }
        }

        if(plugin_state->mode == 0) {
            //Stop Vibration
            notification_message(notification, &sequence_reset_vibro);
            notification_message(notification, &sequence_reset_green);
        } else if(plugin_state->mode == 1) {
            //Full power
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_green_255);
        } else if(plugin_state->mode == 2) {
            //Pulsed Vibration
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_red_255);
            delay(100);
            notification_message(notification, &sequence_reset_vibro);
        } else if(plugin_state->mode == 3) {
            //Soft power
            notification_message(notification, &sequence_set_vibro_on);
            notification_message(notification, &sequence_set_blue_255);
            delay(50);
            notification_message(notification, &sequence_reset_vibro);
        } else if(plugin_state->mode == 4) {
            //Special Sequence
            notification_message(notification, &sequence_solid_yellow);
            for(int i = 0; i < 5; i++) {
                notification_message(notification, &sequence_set_vibro_on);
                delay(50);
                notification_message(notification, &sequence_reset_vibro);
                delay(50);
            }
            for(int i = 0; i < 2; i++) {
                notification_message(notification, &sequence_set_vibro_on);
                delay(400);
                notification_message(notification, &sequence_reset_vibro);
                delay(50);
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