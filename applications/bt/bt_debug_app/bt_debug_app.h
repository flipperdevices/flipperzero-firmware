#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include "views/bt_carrier_test.h"
#include "views/bt_packet_test.h"
#include "../bt_settings.h"

typedef struct {
    BtSettings settings;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Popup* popup;
    BtCarrierTest* bt_carrier_test;
    BtPacketTest* bt_packet_test;
} BtDebugApp;

typedef enum {
    BtDebugAppViewSubmenu,
    BtDebugAppViewPopup,
    BtDebugAppViewCarrierTest,
    BtDebugAppViewPacketTest,
} BtDebugAppView;
