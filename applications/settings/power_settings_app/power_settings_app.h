#pragma once

#include <furi.h>
#include <power/power_service/power.h>
#include <power/power_service/power_settings_api_i.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>

#include "views/battery_info.h"
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>

#include "scenes/power_settings_scene.h"

#include <settings_helpers/submenu_based.h>

typedef struct {
    Power* power;
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    BatteryInfo* batery_info;
    Submenu* submenu;
    DialogEx* dialog;
    VariableItemList* var_item_list;
    PowerInfo info;
    PowerSettings settings;
    SubmenuSettingsHelper* settings_helper;
} PowerSettingsApp;

typedef enum {
    PowerSettingsAppViewBatteryInfo,
    PowerSettingsAppViewSubmenu,
    PowerSettingsAppViewDialog,
    PowerSettingsAppViewVarItemList,
} PowerSettingsAppView;

typedef enum {
    RebootTypeDFU,
    RebootTypeNormal
} RebootType;
