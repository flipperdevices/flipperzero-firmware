#pragma once

#include <furi.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>
#include "views/bt_hid_keyboard.h"
#include "views/bt_hid_media.h"

typedef struct {
    Bt* bt;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    BtHidKeyboard* bt_hid_keyboard;
    BtHidMedia* bt_hid_media;
} BtHid;

typedef enum {
    BtHidViewSubmenu,
    BtHidViewKeyboard,
    BtHidViewMedia,
} BtHidView;
