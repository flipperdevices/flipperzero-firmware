#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "color_guess_icons.h"
#include "digits.h"

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
    bool error;
} ColorGuess;

typedef enum {
    ColorGuessViewIdMenu,
    ColorGuessViewIdPlay,
    ColorGuessViewIdColorSet,
} ColorGuessViewId;