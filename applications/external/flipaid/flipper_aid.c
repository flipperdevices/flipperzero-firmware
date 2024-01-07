#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

//CPR pause/play variable
short cprProgress = 0;

/**
 * Enumeration establishes difference between ticks and input events in CPR
*/

typedef enum {
    CPREventTypeTick,
    CPREventTypeInput,
} CPREventType;

/**
 * Structure for CPR events concerning ticks and input events
*/
typedef struct {
    CPREventType type;
    InputEvent input;
} CPREvent;

void cpr_update(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    CPREvent CPRevent = {.type = CPREventTypeTick};
    furi_message_queue_put(event_queue, &CPRevent, 0);
}

void flipper_aid_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Flipper Aid CPR timer");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "Press OK to start CPR");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 34, "Press return to exit the app");
    //pause button
    if(cprProgress == 0) {
        canvas_draw_line(canvas, 60, 45, 60, 57);
        canvas_draw_line(canvas, 61, 46, 61, 56);
        canvas_draw_line(canvas, 62, 47, 62, 55);
        canvas_draw_line(canvas, 63, 48, 63, 54);
        canvas_draw_line(canvas, 64, 49, 64, 53);
        canvas_draw_line(canvas, 65, 50, 65, 52);
        canvas_draw_dot(canvas, 66, 51);
    } else {
        canvas_draw_box(canvas, 60, 45, 3, 12);
        canvas_draw_box(canvas, 66, 45, 3, 12);
    }
}

void flipper_aid_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    CPREvent event = {.type = CPREventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t flipper_aid_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(CPREvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, flipper_aid_draw_callback, NULL);
    view_port_input_callback_set(view_port, flipper_aid_input_callback, event_queue);
    FuriTimer* timer = furi_timer_alloc(cpr_update, FuriTimerTypePeriodic, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    CPREvent event;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
            break;
        } else if(event.input.type == InputTypeShort && event.input.key == InputKeyOk) {
            if(furi_hal_speaker_acquire(1000)) {
                uint32_t freq = round(furi_kernel_get_tick_frequency() * 0.5);
                furi_timer_start(timer, freq);
                cprProgress = 1; //start CPR
                while(1) {
                    furi_check(
                        furi_message_queue_get(event_queue, &event, FuriWaitForever) ==
                        FuriStatusOk);
                    if(event.type == CPREventTypeInput && event.input.type == InputTypeShort &&
                       (event.input.key == InputKeyBack || event.input.key == InputKeyOk)) {
                        cprProgress = 0; //pause CPR
                        break;
                    } else if(event.type == CPREventTypeTick) {
                        furi_hal_speaker_start(440.0f, 1.0f);
                        furi_delay_ms(50);
                        furi_hal_speaker_stop();
                    }
                }
                furi_hal_speaker_release();
                furi_timer_stop(timer);
            }
        }
    }
    furi_timer_free(timer);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    return 0;
}