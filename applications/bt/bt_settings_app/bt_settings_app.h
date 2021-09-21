#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <gui/modules/variable-item-list.h>
#include <gui/modules/dialog_ex.h>

#include "../bt_settings.h"
#include "scenes/bt_settings_scene.h"

typedef struct {
    BtSettings settings;
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
    DialogEx* dialog;
} BtSettingsApp;

typedef enum {
    BtSettingsAppViewVarItemList,
    BtSettingsAppViewDialog,
} BtSettingsAppView;
