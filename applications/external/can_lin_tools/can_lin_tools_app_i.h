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

typedef struct CanLinToolsApp CanLinToolsApp;

struct CanLinToolsApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    Submenu* submenu;
    Widget* widget;
    CanLinToolsCanHacker2* can_lin_tools_can_hacker2;
};

void can_lin_tools_app_can_hacker2_start(CanLinToolsApp* app);
void can_lin_tools_app_can_hacker2_stop(CanLinToolsApp* app);
bool can_lin_tools_app_can_hacker2_is_nfc(CanLinToolsApp* app);
bool can_lin_tools_app_can_hacker2_is_rfid(CanLinToolsApp* app, uint32_t* frequency);