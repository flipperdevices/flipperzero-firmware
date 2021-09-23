#pragma once

#include "power.h"

#include <stdint.h>

#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <notification/notification-messages.h>
#include <bt/bt_service/bt.h>

typedef enum {
    PowerStateNotCharging,
    PowerStateCharging,
    PowerStateCharged,
} PowerState;

struct Power {
    ViewDispatcher* view_dispatcher;
    View* off_view;

    ViewPort* battery_view_port;
    Bt* bt;
    Gui* gui;
    NotificationApp* notification;

    PowerState state;
    PowerInfo info;
    osMutexId_t info_mtx;
};
