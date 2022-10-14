#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include "views/pomodoro_25.h"

typedef struct {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
    Pomodoro25* pomodoro_25;
    uint32_t view_id;
} Pomodoro;

typedef enum {
    PomodoroViewSubmenu,
    PomodoroView25,
    PomodoroViewExitConfirm,
} PomodoroView;
