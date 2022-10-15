#include "pomodoro_timer.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>

struct PomodoroTimer {
    View* view;
};

struct {
    bool ok_pressed;
    bool reset_pressed;
    bool back_pressed;
    bool connected;
    bool timer_running;
    uint32_t timer_start_timestamp;
    uint32_t timer_stopped_seconds;
    uint32_t time_passed;
} PomodoroTimerModel;

void pomodoro_timer_process(PomodoroTimer* pomodoro_timer, InputEvent* event) {
    with_view_model(
        pomodoro_timer->view,
        PomodoroTimerModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                } else if(event->key == InputKeyLeft) {
                    model->reset_pressed = true;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyOk) {
                    model->ok_pressed = false;

                    // START/STOP TIMER
                    FuriHalRtcDateTime curr_dt;
                    furi_hal_rtc_get_datetime(&curr_dt);
                    uint32_t current_timestamp = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

                    // STARTED -> PAUSED
                    if(model->timer_running) {
                        // Update stopped seconds
                        model->timer_stopped_seconds =
                            current_timestamp - model->timer_start_timestamp;
                    } else if(!model->time_passed) {
                        // INITIAL -> STARTED
                        model->timer_start_timestamp = current_timestamp;
                    } else {
                        // PAUSED -> STARTED
                        model->timer_start_timestamp =
                            current_timestamp - model->timer_stopped_seconds;
                    }
                    model->timer_running = !model->timer_running;
                } else if(event->key == InputKeyLeft) {
                    if(!model->timer_running) {
                        furi_record_close(RECORD_NOTIFICATION);
                        model->timer_stopped_seconds = 0;
                        model->timer_start_timestamp = 0;
                        model->time_passed = 0;
                        model->timer_running = false;
                    }
                    model->reset_pressed = false;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            }
        },
        true);
}

void pomodoro_draw_callback(Canvas* canvas, void* context, int max_seconds) {
    furi_assert(context);
    PomodoroTimerModel* model = context;
    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t current_timestamp = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

    // Header
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 3, 3, AlignLeft, AlignTop, "Pomodoro");

    canvas_draw_icon(canvas, 68, 2, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 127, 3, AlignRight, AlignTop, "Hold to exit");

    // Ok
    canvas_draw_icon(canvas, 63, 25, &I_Space_65x18); // button
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 66, 27, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if(model->timer_running) {
        model->time_passed = current_timestamp - model->timer_start_timestamp;
        elements_multiline_text_aligned(canvas, 83, 36, AlignLeft, AlignBottom, "Pause");
        canvas_draw_icon(canvas, 67, 27, &I_Ok_btn_pressed_13x13); // OK icon
    } else {
        if(model->time_passed) {
            elements_multiline_text_aligned(canvas, 83, 36, AlignLeft, AlignBottom, "Continue");
        } else {
            elements_multiline_text_aligned(canvas, 83, 36, AlignLeft, AlignBottom, "Start");
        }
        canvas_draw_icon(canvas, 70, 29, &I_Ok_btn_9x9); // OK icon
    }
    canvas_set_color(canvas, ColorBlack);

    // Reset
    if(!model->timer_running && model->time_passed) {
        canvas_draw_icon(canvas, 63, 46, &I_Space_65x18);
        if(model->reset_pressed) {
            elements_slightly_rounded_box(canvas, 66, 48, 60, 13);
            canvas_set_color(canvas, ColorWhite);
        }
        canvas_draw_icon(canvas, 72, 50, &I_ButtonLeft_4x7);
        elements_multiline_text_aligned(canvas, 83, 57, AlignLeft, AlignBottom, "Reset");
        canvas_set_color(canvas, ColorBlack);
    }

    char buffer[64];

    // Time passed
    int total_time_left = (max_seconds - (uint32_t)model->time_passed);
    int minutes_left = total_time_left / 60;
    int seconds_left = total_time_left % 60;
    canvas_set_font(canvas, FontBigNumbers);

    snprintf(buffer, sizeof(buffer), "%d:%02d", minutes_left, seconds_left);
    canvas_draw_str(canvas, 0, 40, buffer);

    if(total_time_left < 1) {
        notification_message(furi_record_open(RECORD_NOTIFICATION), &sequence_success);
    }
    if(total_time_left < 0) {
        model->timer_running = false;
        model->time_passed = 0;
    }

    canvas_set_font(canvas, FontSecondary);
    snprintf(
        buffer,
        sizeof(buffer),
        "%02ld:%02ld:%02ld",
        ((uint32_t)current_timestamp % (60 * 60 * 24)) / (60 * 60),
        ((uint32_t)current_timestamp % (60 * 60)) / 60,
        (uint32_t)current_timestamp % 60);
    canvas_draw_str(canvas, 0, 60, buffer);
}
