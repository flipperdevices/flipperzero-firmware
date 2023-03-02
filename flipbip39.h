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
#include "scenes/flipbip39_scene.h"
#include "views/flipbip39_startscreen.h"
#include "views/flipbip39_scene_1.h"
#include "views/flipbip39_scene_2.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    FlipBip39Startscreen* flipbip39_startscreen;
    FlipBip39Scene1* flipbip39_scene_1;
    FlipBip39Scene2* flipbip39_scene_2;
    int haptic; 
    int speaker;
    int led;
    int bip39_strength;
} FlipBip39;

typedef enum {
    FlipBip39ViewIdStartscreen,
    FlipBip39ViewIdMenu,
    FlipBip39ViewIdScene1,
    FlipBip39ViewIdScene2,
    FlipBip39ViewIdSettings,
} FlipBip39ViewId;

typedef enum {
    FlipBip39HapticOff,
    FlipBip39HapticOn,
} FlipBip39HapticState;

typedef enum {
    FlipBip39SpeakerOff,
    FlipBip39SpeakerOn,
} FlipBip39SpeakerState;

typedef enum {
    FlipBip39LedOff,
    FlipBip39LedOn,
} FlipBip39LedState;

typedef enum {
    FlipBip39Strength128,
    FlipBip39Strength192,
    FlipBip39Strength256,
} FlipBip39StrengthState;
