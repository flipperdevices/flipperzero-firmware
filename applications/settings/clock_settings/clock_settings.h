#pragma once

#include "scenes/clock_settings_scene.h"

#include <furi_hal_clock.h>
#include <furi_hal_pwm.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include "views/clock_settings_module.h"

typedef struct ClockSettings ClockSettings;

struct ClockSettings {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    ClockSettingsModule* pwm_view;
};

typedef enum {
    ClockSettingsViewPwm,
} ClockSettingsView;

typedef enum {
    ClockSettingsCustomEventNone,
} ClockSettingsCustomEvent;
