#pragma once

#include "scenes/scope_types.h"
#include "scenes/scope_scene.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

typedef struct ScopeApp ScopeApp;

typedef struct {
    double time;
    char* str;
} timeperiod;

static const timeperiod time_list[] =
    {{1.0, "1s"}, {0.1, "0.1s"}, {1e-3, "1ms"}, {0.1e-3, "0.1ms"}, {1e-6, "1us"}};

enum measureenum { m_time, m_voltage, m_capture };

typedef struct {
    enum measureenum type;
    char* str;
} measurement;

static const measurement measurement_list[] = {
    {m_time, "Time"},
    {m_voltage, "Voltage"},
    {m_capture, "Capture"}};

struct ScopeApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    VariableItemList* variable_item_list;
    Submenu* submenu;
    Widget* widget;
    double time;
    enum measureenum measurement;
};
