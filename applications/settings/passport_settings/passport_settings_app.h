#pragma once

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <assets_icons.h>

#include "passport_settings.h"

typedef enum {
    PassportSettingsAppViewMenu,
    PassportSettingsAppViewVarItemList,
} PassportSettingsAppView;

typedef struct {
    PassportSettings settings;

    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* variable_item_list;
    Submenu* submenu;
} PassportSettingsApp;
