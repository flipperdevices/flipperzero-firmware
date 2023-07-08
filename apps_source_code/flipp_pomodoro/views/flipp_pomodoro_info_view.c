
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view.h>
#include "flipp_pomodoro_info_view.h"
// Auto-compiled icons
#include "flipp_pomodoro_icons.h"

enum {
    ViewInputConsumed = true,
    ViewInputNotConusmed = false,
};

struct FlippPomodoroInfoView {
    View* view;
    FlippPomodoroInfoViewUserActionCb resume_timer_cb;
    void* user_action_cb_ctx;
};

typedef struct {
    uint8_t pomodoros_completed;
    FlippPomodoroInfoViewMode mode;
} FlippPomodoroInfoViewModel;

static void
    flipp_pomodoro_info_view_draw_statistics(Canvas* canvas, FlippPomodoroInfoViewModel* model) {
    FuriString* stats_string = furi_string_alloc();

    furi_string_printf(
        stats_string,
        "So Long,\nand Thanks for All the Focus...\nand for completing\n%i pomodoro(s)",
        model->pomodoros_completed);
    const char* stats_string_formatted = furi_string_get_cstr(stats_string);

    elements_text_box(
        canvas,
        0,
        0,
        canvas_width(canvas),
        canvas_height(canvas) - 10,
        AlignCenter,
        AlignCenter,
        stats_string_formatted,
        true);

    furi_string_free(stats_string);

    elements_button_left(canvas, "Guide");
}

static void
    flipp_pomodoro_info_view_draw_about(Canvas* canvas, FlippPomodoroInfoViewModel* model) {
    UNUSED(model);
    canvas_draw_icon(canvas, 0, 0, &I_flipp_pomodoro_learn_50x128);
    elements_button_left(canvas, "Stats");
}

static void flipp_pomodoro_info_view_draw_callback(Canvas* canvas, void* _model) {
    if(!_model) {
        return;
    };

    FlippPomodoroInfoViewModel* model = _model;

    canvas_clear(canvas);

    if(model->mode == FlippPomodoroInfoViewModeStats) {
        flipp_pomodoro_info_view_draw_statistics(canvas, model);
    } else {
        flipp_pomodoro_info_view_draw_about(canvas, model);
    }

    elements_button_right(canvas, "Resume");
}

void flipp_pomodoro_info_view_set_mode(View* view, FlippPomodoroInfoViewMode desired_mode) {
    with_view_model(
        view, FlippPomodoroInfoViewModel * model, { model->mode = desired_mode; }, false);
}

void flipp_pomodoro_info_view_toggle_mode(FlippPomodoroInfoView* info_view) {
    with_view_model(
        flipp_pomodoro_info_view_get_view(info_view),
        FlippPomodoroInfoViewModel * model,
        {
            flipp_pomodoro_info_view_set_mode(
                flipp_pomodoro_info_view_get_view(info_view),
                (model->mode == FlippPomodoroInfoViewModeStats) ? FlippPomodoroInfoViewModeAbout :
                                                                  FlippPomodoroInfoViewModeStats);
        },
        true);
}

bool flipp_pomodoro_info_view_input_callback(InputEvent* event, void* ctx) {
    FlippPomodoroInfoView* info_view = ctx;

    if(event->type == InputTypePress) {
        if(event->key == InputKeyRight && info_view->resume_timer_cb != NULL) {
            info_view->resume_timer_cb(info_view->user_action_cb_ctx);
            return ViewInputConsumed;
        } else if(event->key == InputKeyLeft) {
            flipp_pomodoro_info_view_toggle_mode(info_view);
            return ViewInputConsumed;
        }
    }

    return ViewInputNotConusmed;
}

FlippPomodoroInfoView* flipp_pomodoro_info_view_alloc() {
    FlippPomodoroInfoView* info_view = malloc(sizeof(FlippPomodoroInfoView));
    info_view->view = view_alloc();

    view_allocate_model(
        flipp_pomodoro_info_view_get_view(info_view),
        ViewModelTypeLockFree,
        sizeof(FlippPomodoroInfoViewModel));
    view_set_context(flipp_pomodoro_info_view_get_view(info_view), info_view);
    view_set_draw_callback(
        flipp_pomodoro_info_view_get_view(info_view), flipp_pomodoro_info_view_draw_callback);
    view_set_input_callback(
        flipp_pomodoro_info_view_get_view(info_view), flipp_pomodoro_info_view_input_callback);

    return info_view;
}

View* flipp_pomodoro_info_view_get_view(FlippPomodoroInfoView* info_view) {
    return info_view->view;
}

void flipp_pomodoro_info_view_free(FlippPomodoroInfoView* info_view) {
    furi_assert(info_view);
    view_free(info_view->view);
    free(info_view);
}

void flipp_pomodoro_info_view_set_pomodoros_completed(View* view, uint8_t pomodoros_completed) {
    with_view_model(
        view,
        FlippPomodoroInfoViewModel * model,
        { model->pomodoros_completed = pomodoros_completed; },
        false);
}

void flipp_pomodoro_info_view_set_resume_timer_cb(
    FlippPomodoroInfoView* info_view,
    FlippPomodoroInfoViewUserActionCb user_action_cb,
    void* user_action_cb_ctx) {
    info_view->resume_timer_cb = user_action_cb;
    info_view->user_action_cb_ctx = user_action_cb_ctx;
}
