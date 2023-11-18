#include "../pomodoro_timer.h"
#include "pomodoro_52.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>

static void pomodoro_52_draw_callback(Canvas* canvas, void* context) {
    int max_seconds = 60 * 52;
    int max_seconds_rest = 60 * 17;
    pomodoro_draw_callback(canvas, context, max_seconds, max_seconds_rest);
}

static bool pomodoro_52_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    PomodoroTimer* pomodoro_52 = context;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        return false;
    } else {
        pomodoro_timer_process(pomodoro_52, event);
        return true;
    }
}

PomodoroTimer* pomodoro_52_alloc() {
    PomodoroTimer* pomodoro_52 = malloc(sizeof(PomodoroTimer));
    pomodoro_52->view = view_alloc();
    view_set_context(pomodoro_52->view, pomodoro_52);
    view_allocate_model(pomodoro_52->view, ViewModelTypeLocking, sizeof(PomodoroTimerModel));
    view_set_draw_callback(pomodoro_52->view, pomodoro_52_draw_callback);
    view_set_input_callback(pomodoro_52->view, pomodoro_52_input_callback);

    return pomodoro_52;
}

void pomodoro_52_free(PomodoroTimer* pomodoro_52) {
    furi_assert(pomodoro_52);
    view_free(pomodoro_52->view);
    free(pomodoro_52);
}

View* pomodoro_52_get_view(PomodoroTimer* pomodoro_52) {
    furi_assert(pomodoro_52);
    return pomodoro_52->view;
}
