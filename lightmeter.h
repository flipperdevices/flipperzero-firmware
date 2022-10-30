#pragma once

#include <furi.h>
// #include <power/power_service/power.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "views/main_view.h"
// #include "views/module_view.h"
// #include "views/reboot_view.h"
// #include "views/reset_view.h"
// #include <gui/modules/submenu.h>
// #include <gui/modules/dialog_ex.h>

#include "scenes/lightmeter_scene.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    MainView* main_view;
} LightMeterApp;

typedef enum {
    VirtualButtonAppViewMainView,
} LightMeterAppView;
