#pragma once

#include "helpers/avr_isp_types.h"

#include "scenes/avr_isp_scene.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
//#include <gui/modules/variable_item_list.h>

#include "views/avr_isp_view_programmer.h"

typedef struct AvrIspApp AvrIspApp;

struct AvrIspApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    //VariableItemList* variable_item_list;
    Submenu* submenu;
    Widget* widget;
    AvrIspProgrammerView* avr_asp_programmer_view;
};
