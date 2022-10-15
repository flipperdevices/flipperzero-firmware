#pragma once

#include <gui/view.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>

typedef struct PomodoroTimer PomodoroTimer;

typedef struct PomodoroTimerModel PomodoroTimerModel;

void pomodoro_timer_process(PomodoroTimer* pomodoro_timer, InputEvent* event);

void pomodoro_draw_callback(Canvas* canvas, void* context, int max_seconds);
