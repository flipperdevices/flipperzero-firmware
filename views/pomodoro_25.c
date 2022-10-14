#include "pomodoro_25.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>

struct Pomodoro25 {
    View* view;
};

typedef struct {
    bool ok_pressed;
    bool reset_pressed;
    bool back_pressed;
    bool connected;
    bool timer_running;
    uint32_t timer_start_timestamp;
    uint32_t timer_stopped_seconds;
    uint32_t time_passed;
} Pomodoro25Model;

int max_seconds_25 = 25 * 60;

static void pomodoro_25_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    Pomodoro25Model* model = context;
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

    // Timer start time
    // snprintf(buffer, sizeof(buffer), "%ld", (uint32_t)model->timer_start_timestamp);
    // elements_multiline_text_aligned(canvas, 3, 20, AlignLeft, AlignBottom, buffer);

    // Timer stopped
    // snprintf(buffer, sizeof(buffer), "%ld", (uint32_t)model->timer_stopped_seconds);
    // elements_multiline_text_aligned(canvas, 3, 30, AlignLeft, AlignBottom, buffer);

    // Time passed
    int total_time_left = (max_seconds_25 - (uint32_t)model->time_passed);
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

static void pomodoro_25_process(Pomodoro25* pomodoro_25, InputEvent* event) {
    with_view_model(
        pomodoro_25->view,
        Pomodoro25Model * model,
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
                    furi_record_close(RECORD_NOTIFICATION);
                    model->timer_stopped_seconds = 0;
                    model->timer_start_timestamp = 0;
                    model->time_passed = 0;
                    model->reset_pressed = false;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                }
            }
        },
        true);
}

static bool pomodoro_25_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    Pomodoro25* pomodoro_25 = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
    } else {
        pomodoro_25_process(pomodoro_25, event);
        consumed = true;
    }

    return consumed;
}

Pomodoro25* pomodoro_25_alloc() {
    Pomodoro25* pomodoro_25 = malloc(sizeof(Pomodoro25));
    pomodoro_25->view = view_alloc();
    view_set_context(pomodoro_25->view, pomodoro_25);
    view_allocate_model(pomodoro_25->view, ViewModelTypeLocking, sizeof(Pomodoro25Model));
    view_set_draw_callback(pomodoro_25->view, pomodoro_25_draw_callback);
    view_set_input_callback(pomodoro_25->view, pomodoro_25_input_callback);

    return pomodoro_25;
}

void pomodoro_25_free(Pomodoro25* pomodoro_25) {
    furi_assert(pomodoro_25);
    view_free(pomodoro_25->view);
    free(pomodoro_25);
}

View* pomodoro_25_get_view(Pomodoro25* pomodoro_25) {
    furi_assert(pomodoro_25);
    return pomodoro_25->view;
}
