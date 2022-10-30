#pragma once

#include <furi.h>
// #include <power/power_service/power.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "gui/views/main_view.h"

#include <gui/modules/submenu.h>
// #include <gui/modules/dialog_ex.h>

#include "gui/scenes/config/lightmeter_scene.h"

// #include "lightmeterI2C.h"

typedef struct {
    int iso;
    int nd;
} LightMeterAppConfig;

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    MainView* main_view;
    Submenu* submenu;
    LightMeterAppConfig* config;
} LightMeterApp;

typedef enum {
    LightMeterAppViewMainView,
    LightMeterAppViewConfigView,
    LightMeterAppViewSubmenu,
} LightMeterAppView;

typedef enum {
    LightMeterAppCustomEventConfig,
    LightMeterAppCustomEventHelp,
    LightMeterAppCustomEventAbout,
} LightMeterAppCustomEvent;
