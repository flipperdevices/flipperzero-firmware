#pragma once

#include "helpers/scope_types.h"
#include "scenes/scope_scene.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

typedef struct ScopeApp ScopeApp;

struct ScopeApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    VariableItemList* variable_item_list;
    Submenu* submenu;
    Widget* widget;
    double time;
};

typedef struct {
    double time;
    char* str;
} timeperiod;

static const timeperiod time_list[] = {
    {1.0, "1s"},
    {0.1, "0.1s"},
    {1e-3, "1ms"},
    {0.1e-3, "0.1ms"},
    {1e-6, "1us"},
    {0.5e-6, "0.5us"}};
