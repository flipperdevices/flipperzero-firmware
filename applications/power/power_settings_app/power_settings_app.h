#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>

#include "scenes/power_settings_scene.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
} PowerSettingsApp;

typedef enum {
    PowerSettingsAppViewSubmenu,
    PowerSettingsAppViewDialog,
} PowerSettingsAppView;
