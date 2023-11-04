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
#include "helpers/color_guess_custom_event.h"
#include "scenes/color_guess_scene.h"
#include "views/color_guess_color_set.h"
#include "views/color_guess_play.h"
#include "views/color_guess_startscreen.h"
#include "helpers/color_guess_storage.h"

#define TAG "Color_Guess"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewPort* view_port;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    VariableItemList* variable_item_list;
    SceneManager* scene_manager;
    ColorGuessColorSet* color_guess_color_set;
    ColorGuessPlay* color_guess_play;
    ColorGuessStartscreen* color_guess_startscreen;
    Submenu* color_guess_settings;
    bool error;
    uint32_t haptic;
    //uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
} ColorGuess;

typedef enum {
    ColorGuessViewIdStartscreen,
    ColorGuessViewIdMenu,
    ColorGuessViewIdPlay,
    ColorGuessViewIdColorSet,
    ColorGuessViewIdSettings,
} ColorGuessViewId;

typedef enum {
    ColorGuessHapticOff,
    ColorGuessHapticOn,
} ColorGuessHapticState;

typedef enum {
    ColorGuessSpeakerOff,
    ColorGuessSpeakerOn,
} ColorGuessSpeakerState;

typedef enum {
    ColorGuessLedOff,
    ColorGuessLedOn,
} ColorGuessLedState;