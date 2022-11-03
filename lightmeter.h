#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "gui/views/main_view.h"

#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include "gui/scenes/config/lightmeter_scene.h"
#include <notification/notification_messages.h>

#include "lightmeter_config.h"

#define LM_VERSION_APP "0.2"
#define LM_DEVELOPED "Oleksii Kutuzov"
#define LM_GITHUB "https://github.com/oleksiikutuzov/flipperzero-lightmeter"

typedef struct {
    int iso;
    int nd;
    int aperture;
} LightMeterConfig;

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    MainView* main_view;
    VariableItemList* var_item_list;
    LightMeterConfig* config;
    NotificationApp* notifications;
    Widget* widget;
} LightMeterApp;

typedef enum {
    LightMeterAppViewMainView,
    LightMeterAppViewConfigView,
    LightMeterAppViewVarItemList,
    LightMeterAppViewAbout,
    LightMeterAppViewHelp,
} LightMeterAppView;

typedef enum {
    LightMeterAppCustomEventConfig,
    LightMeterAppCustomEventHelp,
    LightMeterAppCustomEventAbout,
} LightMeterAppCustomEvent;

void lightmeter_app_set_config(LightMeterApp* app, LightMeterConfig* config);

int send_command(uint8_t command);
