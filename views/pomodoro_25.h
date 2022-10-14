#pragma once

#include <gui/view.h>

typedef struct Pomodoro25 Pomodoro25;

Pomodoro25* pomodoro_25_alloc();

void pomodoro_25_free(Pomodoro25* pomodoro_25);

View* pomodoro_25_get_view(Pomodoro25* pomodoro_25);
