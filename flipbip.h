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
#include "scenes/flipbip_scene.h"
#include "views/flipbip_startscreen.h"
#include "views/flipbip_scene_1.h"

#define FLIPBIP_VERSION "v0.0.4"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    FlipBipStartscreen* flipbip_startscreen;
    FlipBipScene1* flipbip_scene_1;
    int haptic;
    int led;
    int bip39_strength;
    int bip44_coin;
    int overwrite_saved_seed;
} FlipBip;

typedef enum {
    FlipBipViewIdStartscreen,
    FlipBipViewIdMenu,
    FlipBipViewIdScene1,
    // FlipBipViewIdScene2,
    FlipBipViewIdSettings,
} FlipBipViewId;

typedef enum {
    FlipBipHapticOff,
    FlipBipHapticOn,
} FlipBipHapticState;

typedef enum {
    FlipBipLedOff,
    FlipBipLedOn,
} FlipBipLedState;

typedef enum {
    FlipBipStrength128,
    FlipBipStrength192,
    FlipBipStrength256,
} FlipBipStrengthState;

typedef enum {
    FlipBipCoinBTC0,
    FlipBipCoinETH60,
} FlipBipCoinState;
