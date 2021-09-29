#pragma once

#include "scenes/gpio_scene.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification-messages.h>

#include <gui/modules/variable-item-list.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;

    VariableItemList* var_item_list;
} GpioApp;

typedef enum {
    GpioAppViewVarItemList,
} GpioAppView;
