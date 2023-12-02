#pragma once

#include <gui/view.h>
#include "../pomodoro_timer.h"

PomodoroTimer* pomodoro_52_alloc();

void pomodoro_52_free(PomodoroTimer* pomodoro_52);

View* pomodoro_52_get_view(PomodoroTimer* pomodoro_52);
