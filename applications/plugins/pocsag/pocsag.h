#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/submenu.h>
#include "core/pocsag_tx_rx_worker.h"


typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;

    uint32_t view_id;
    PocsagTxRxWorker* subghz_tx_rx;
} PocsagApp;


enum {
    PocsagViewSubmenu,
};