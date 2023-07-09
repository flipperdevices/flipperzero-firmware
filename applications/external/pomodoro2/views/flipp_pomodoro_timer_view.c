#include "flipp_pomodoro_timer_view.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view.h>
#include "../helpers/debug.h"
#include "../flipp_pomodoro_app.h"
#include "../modules/flipp_pomodoro.h"

// Auto-compiled icons
#include "flipp_pomodoro_icons.h"

enum {
    ViewInputConsumed = true,
    ViewInputNotConusmed = false,
};

struct FlippPomodoroTimerView {
    View* view;
    FlippPomodoroTimerViewInputCb right_cb;
    void* right_cb_ctx;
};

typedef struct {
    IconAnimation* icon;
    FlippPomodoroState* state;
} FlippPomodoroTimerViewModel;

static const Icon* stage_background_image[] = {
    [FlippPomodoroStageFocus] = &A_flipp_pomodoro_focus_64,
    [FlippPomodoroStageRest] = &A_flipp_pomodoro_rest_64,
    [FlippPomodoroStageLongBreak] = &A_flipp_pomodoro_rest_64,
};

static void
    flipp_pomodoro_view_timer_draw_countdown(Canvas* canvas, TimeDifference remaining_time) {
    canvas_set_font(canvas, FontBigNumbers);
    const uint8_t right_border_margin = 1;

    const uint8_t countdown_box_height = canvas_height(canvas) * 0.4;
    const uint8_t countdown_box_width = canvas_width(canvas) * 0.5;
    const uint8_t countdown_box_x =
        canvas_width(canvas) - countdown_box_width - right_border_margin;
    const uint8_t countdown_box_y = 15;

    elements_bold_rounded_frame(
        canvas, countdown_box_x, countdown_box_y, countdown_box_width, countdown_box_height);

    FuriString* timer_string = furi_string_alloc();
    furi_string_printf(timer_string, "%02u:%02u", remaining_time.minutes, remaining_time.seconds);
    const char* remaining_stage_time_string = furi_string_get_cstr(timer_string);
    canvas_draw_str_aligned(
        canvas,
        countdown_box_x + (countdown_box_width / 2),
        countdown_box_y + (countdown_box_height / 2),
        AlignCenter,
        AlignCenter,
        remaining_stage_time_string);

    furi_string_free(timer_string);
}

static void draw_str_with_drop_shadow(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    const char* str) {
    canvas_set_color(canvas, ColorWhite);
    for(int x_off = -2; x_off <= 2; x_off++) {
        for(int y_off = -2; y_off <= 2; y_off++) {
            canvas_draw_str_aligned(canvas, x + x_off, y + y_off, horizontal, vertical, str);
        }
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, x, y, horizontal, vertical, str);
}

static void
    flipp_pomodoro_view_timer_draw_current_stage_label(Canvas* canvas, FlippPomodoroState* state) {
    canvas_set_font(canvas, FontPrimary);
    draw_str_with_drop_shadow(
        canvas,
        canvas_width(canvas),
        0,
        AlignRight,
        AlignTop,
        flipp_pomodoro__current_stage_label(state));
}

static void flipp_pomodoro_view_timer_draw_callback(Canvas* canvas, void* _model) {
    if(!_model) {
        return;
    }

    FlippPomodoroTimerViewModel* model = _model;

    canvas_clear(canvas);
    if(model->icon) {
        canvas_draw_icon_animation(canvas, 0, 0, model->icon);
    }

    flipp_pomodoro_view_timer_draw_countdown(
        canvas, flipp_pomodoro__stage_remaining_duration(model->state));

    flipp_pomodoro_view_timer_draw_current_stage_label(canvas, model->state);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontSecondary);
    elements_button_right(canvas, flipp_pomodoro__next_stage_label(model->state));
}

bool flipp_pomodoro_view_timer_input_callback(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    furi_assert(event);
    FlippPomodoroTimerView* timer = ctx;

    const bool should_trigger_right_event_cb = (event->type == InputTypePress) &&
                                               (event->key == InputKeyRight) &&
                                               (timer->right_cb != NULL);

    if(should_trigger_right_event_cb) {
        furi_assert(timer->right_cb);
        furi_assert(timer->right_cb_ctx);
        timer->right_cb(timer->right_cb_ctx);
        return ViewInputConsumed;
    }

    return ViewInputNotConusmed;
}

View* flipp_pomodoro_view_timer_get_view(FlippPomodoroTimerView* timer) {
    furi_assert(timer);
    return timer->view;
}

void flipp_pomodoro_view_timer_assign_animation(View* view) {
    with_view_model(
        view,
        FlippPomodoroTimerViewModel * model,
        {
            furi_assert(model->state);
            if(model->icon) {
                icon_animation_free(model->icon);
            }
            model->icon = icon_animation_alloc(
                stage_background_image[flipp_pomodoro__get_stage(model->state)]);
            view_tie_icon_animation(view, model->icon);
            icon_animation_start(model->icon);
        },
        true);
}

FlippPomodoroTimerView* flipp_pomodoro_view_timer_alloc() {
    FlippPomodoroTimerView* timer = malloc(sizeof(FlippPomodoroTimerView));
    timer->view = view_alloc();

    view_allocate_model(
        flipp_pomodoro_view_timer_get_view(timer),
        ViewModelTypeLockFree,
        sizeof(FlippPomodoroTimerViewModel));
    view_set_context(flipp_pomodoro_view_timer_get_view(timer), timer);
    view_set_draw_callback(timer->view, flipp_pomodoro_view_timer_draw_callback);
    view_set_input_callback(timer->view, flipp_pomodoro_view_timer_input_callback);

    return timer;
}

void flipp_pomodoro_view_timer_set_on_right_cb(
    FlippPomodoroTimerView* timer,
    FlippPomodoroTimerViewInputCb right_cb,
    void* right_cb_ctx) {
    furi_assert(right_cb);
    furi_assert(right_cb_ctx);
    timer->right_cb = right_cb;
    timer->right_cb_ctx = right_cb_ctx;
}

void flipp_pomodoro_view_timer_set_state(View* view, FlippPomodoroState* state) {
    furi_assert(view);
    furi_assert(state);
    with_view_model(
        view, FlippPomodoroTimerViewModel * model, { model->state = state; }, false);
    flipp_pomodoro_view_timer_assign_animation(view);
}

void flipp_pomodoro_view_timer_free(FlippPomodoroTimerView* timer) {
    furi_assert(timer);
    with_view_model(
        timer->view,
        FlippPomodoroTimerViewModel * model,
        { icon_animation_free(model->icon); },
        false);
    view_free(timer->view);

    free(timer);
}