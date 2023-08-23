#pragma once

#include "helpers/can_lin_tools_types.h"
#include "helpers/can_lin_tools_event.h"

#include "scenes/can_lin_tools_scene.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include "views/can_lin_tools_view_can_hacker2.h"
#include "views/can_lin_tools_view_lin_hacker.h"
#include "views/can_lin_tools_view_carbus_analyzer.h"

typedef struct CanLinToolsApp CanLinToolsApp;

struct CanLinToolsApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    Submenu* submenu;
    Widget* widget;
    CanLinToolsCanHacker2View* can_lin_tools_can_hacker2_view;
    CanLinToolsLinHackerView* can_lin_tools_lin_hacker_view;
    CanLinToolsCarBusAnalyzerView* can_lin_tools_carbus_analyzer_view;
};
