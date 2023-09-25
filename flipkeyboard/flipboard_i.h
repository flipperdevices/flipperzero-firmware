#pragma once

#include "flipboard.h"

#include <gui/view_dispatcher.h>
#include "../common/app_menu.h"
#include "../common/flipboard_model.h"
#include "../common/key_config.h"
#include "../common/leds.h"
#include "../common/keyboard.h"

struct Flipboard {
    ViewDispatcher* view_dispatcher;
    AppMenu* app_menu;
    KeyConfig* key_config;

    View* view_primary;

    FlipboardModel* model;
};

typedef enum {
    FlipboardViewAppMenuId, // The first entry should be the app menu
    FlipboardViewConfigureId,
    FlipboardViewConfigureSubviewId,
    FlipboardViewConfigureTextInputId,

    FlipboardViewAboutId,
    FlipboardViewPrimaryId,
} FlipboardViewId;

#define TAG "FlipboardKeyboard"
#define FLIPBOARD_APP_NAME "keyboard"
#define FLIPBOARD_PRIMARY_ITEM_NAME "Flipboard Keyboard"