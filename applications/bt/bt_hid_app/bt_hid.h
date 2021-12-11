#pragma once

#include <furi.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>
#include "views/bt_hid_keynote.h"
#include "views/bt_hid_media.h"

typedef struct {
    Bt* bt;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    BtHidKeynote* bt_hid_keynote;
    BtHidMedia* bt_hid_media;
} BtHid;

typedef enum {
    BtHidViewSubmenu,
    BtHidViewKeynote,
    BtHidViewMedia,
} BtHidView;
