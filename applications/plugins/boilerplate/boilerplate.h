#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include "scenes/boilerplate_scene.h"
#include "views/boilerplate_scene_1.h"
#include "views/boilerplate_scene_2.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    BoilerplateScene1* boilerplate_scene_1;
    BoilerplateScene2* boilerplate_scene_2;
    Submenu* boilerplate_settings;
    int haptic;
    int speaker;
    int led;
    //BoilerplateSettings* boilerplate_settings;
} Boilerplate;

typedef enum {
    BoilerplateViewIdMenu,
    BoilerplateViewIdScene1,
    BoilerplateViewIdScene2,
    BoilerplateViewIdSettings,
} BoilerplateViewId;

typedef enum {
    BoilerplateHapticOff,
    BoilerplateHapticOn,
} BoilerplateHapticState;

typedef enum {
    BoilerplateSpeakerOff,
    BoilerplateSpeakerOn,
} BoilerplateSpeakerState;

typedef enum {
    BoilerplateLedOff,
    BoilerplateLedOn,
} BoilerplateLedState;
