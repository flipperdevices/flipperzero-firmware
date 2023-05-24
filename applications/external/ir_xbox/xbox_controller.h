#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <infrared_worker.h>
#include <infrared_transmit.h>
#include <gui/modules/dialog_ex.h>

#include "views/xbox_controller_view.h"
#include "xc_icons.h"

// this should be used as global state
// we can store different things here
typedef struct {
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
    XboxControllerView* xbox_controller_view;
    uint32_t view_id;
} XboxController;

typedef enum {
    UsbHidViewSubmenu,
    UsbHidViewXboxController,
    UsbHidViewExitConfirm,
} UsbHidView;
