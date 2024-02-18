#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include "pomodoro_timer.h"
#include "views/pomodoro_10.h"
#include "views/pomodoro_25.h"
#include "views/pomodoro_50.h"
#include "views/pomodoro_52.h"

typedef struct {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
    PomodoroTimer* pomodoro_10;
    PomodoroTimer* pomodoro_25;
    PomodoroTimer* pomodoro_50;
    PomodoroTimer* pomodoro_52;
    uint32_t view_id;
} Pomodoro;

typedef enum {
    PomodoroViewSubmenu,
    PomodoroView10,
    PomodoroView25,
    PomodoroView50,
    PomodoroView52,
    PomodoroViewExitConfirm,
} PomodoroView;
