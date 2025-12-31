#pragma once

#include <furi.h>
#include <power/power_service/power.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>

#include "views/battery_info.h"
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>

#include <power/power_settings.h>
#include "scenes/power_settings_scene.h"

typedef struct {
    Power* power;
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    BatteryInfo* batery_info;
    Submenu* submenu;
    DialogEx* dialog;
    PowerInfo info;
<<<<<<< HEAD
    VariableItemList* variable_item_list;
    uint32_t shutdown_idle_delay_ms;
    FuriPubSub* settings_events;
=======
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner
} PowerSettingsApp;

typedef enum {
    PowerSettingsAppViewBatteryInfo,
    PowerSettingsAppViewSubmenu,
    PowerSettingsAppViewDialog,
    PowerSettingsAppViewVariableItemList
} PowerSettingsAppView;
<<<<<<< HEAD

typedef enum { RebootTypeDFU, RebootTypeNormal } RebootType;
=======
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner
