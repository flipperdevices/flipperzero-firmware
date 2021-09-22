#pragma once

#include "power.h"

#include <stdint.h>

#include <gui/view_dispatcher.h>
#include <gui/modules/dialog.h>
#include <menu/menu.h>
#include <menu/menu_item.h>
#include <cli/cli.h>
#include <bt/bt_service/bt.h>

typedef enum {
    PowerStateNotCharging,
    PowerStateCharging,
    PowerStateCharged,
} PowerState;

struct Power {
    ViewDispatcher* view_dispatcher;
    View* info_view;
    View* off_view;
    View* disconnect_view;

    ViewPort* battery_view_port;

    Dialog* dialog;

    ValueMutex* menu_vm;
    Cli* cli;
    Bt* bt;
    MenuItem* menu;

    PowerState state;
    PowerInfo info;
    osMutexId_t info_mtx;
};
