#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>
#include "views/bt_hid_keyboard.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    BtHidKeyboard* bt_hid_keyboard;
} BtHid;

typedef enum {
    BtHidViewSubmenu,
    BtHidViewKeyboard,
    BtHidViewMediaController,
} BtHidView;
