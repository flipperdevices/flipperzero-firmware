#pragma once

#include "flipboard.h"

#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include "app_menu.h"
#include "flipboard_model.h"
#include "key_config.h"
#include "leds.h"
#include "keyboard.h"

struct Flipboard {
    ViewDispatcher* view_dispatcher;

    AppMenu* app_menu;

    KeyConfig* key_config;
    View* view_primary;
    Widget* widget_about;

    FlipboardModel* model;
};

typedef enum {
    FlipboardViewAppMenuId, // The first entry must be the app menu
    FlipboardViewConfigureId,
    FlipboardViewConfigureSubviewId,
    FlipboardViewConfigureTextInputId,
    FlipboardViewConfigureKeyboardInputId,

    FlipboardViewAboutId,
    FlipboardViewPrimaryId,
} FlipboardViewId;
