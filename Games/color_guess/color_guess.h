#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
//#include "color_guess_icons.h"
//#include "digits.h"

//#include <furi.h>
//#include <gui/gui.h>
//#include <input/input.h>
//#include <lib/toolbox/path.h>
//#include <lib/subghz/protocols/protocol_items.h>
//#include "scenes/color_guess_scene.h"
#include <gui/scene_manager.h>
//#include <color_guess/scenes/color_guess_scene.h>
#include "scenes/color_guess_scene.h"
//#include "color_guess_icons.h"
#include "views/color_guess_color_set.h"
#include "views/color_guess_play.h"

//#include <stdlib.h>
//#include <notification/notification_messages.h>
//#include "color_guess_icons.h"
//#include "digits.h"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    int scene;
    int cursorpos;
    int red;
    int digit[6];
    int green;
    int blue;
} PluginState;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    FuriMessageQueue* event_queue;
    PluginState* plugin_state;
    ViewPort* view_port;
    ValueMutex state_mutex;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    ColorGuessColorSet* color_guess_color_set;
    ColorGuessPlay* color_guess_play;
    bool error;
} ColorGuess;

typedef enum {
    ColorGuessViewIdMenu,
    ColorGuessViewIdPlay,
    ColorGuessViewIdColorSet,
} ColorGuessViewId;