#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

uint8_t state = 0;
int time = 42;
FuriTimer* timer;
FuriTimer* timer_stop;

static void beep() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_success);
    furi_record_close(RECORD_NOTIFICATION);
}

static void tick(void* context) {
    UNUSED(context);
    time++;
}
static void stop(void* context) {
    UNUSED(context);
    furi_timer_stop(timer_stop);
    furi_timer_start(timer, 1);
    beep();
    state = 2;
}

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    if(state == 0) {
        canvas_draw_str(canvas, 16, 36, "Press OK to start!");
    } else if(state == 1) {
        canvas_draw_str(canvas, 44, 36, "Wait...");
    } else if(state == 2) {
        canvas_draw_str(canvas, 35, 36, "Press OK!");
    } else if(state == 3) {
        char* time_str = "??????";
        snprintf(time_str, 6, "%d", time);
        canvas_draw_str(canvas, 56, 36, time_str);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 40, 24, "Time (ms):");
        canvas_draw_str(canvas, 18, 48, "Press OK to play again");
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t reaction_main(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    //TImer
    timer = furi_timer_alloc(tick, FuriTimerTypePeriodic, NULL);
    timer_stop = furi_timer_alloc(stop, FuriTimerTypeOnce, NULL);

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypePress && event.key == InputKeyBack) running = false;

            if(state == 0 && event.type == InputTypePress && event.key == InputKeyOk) {
                state = 1;
                time = 0;
                view_port_update(view_port);
                int t = rand() % 6000 + 1000;
                furi_delay_ms(t);
                furi_timer_start(timer_stop, t);
                continue;
            }
            if(state == 2 && event.type == InputTypePress && event.key == InputKeyOk) {
                state = 3;
                furi_timer_stop(timer);
                continue;
            }
            if(state == 3 && event.type == InputTypePress && event.key == InputKeyOk) {
                state = 0;
                continue;
            }
        }
        view_port_update(view_port);
    }

    furi_timer_stop(timer);
    furi_timer_free(timer);
    furi_timer_stop(timer_stop);
    furi_timer_free(timer_stop);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
