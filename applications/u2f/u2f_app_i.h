#pragma once

#include "u2f_app.h"
#include "scenes/u2f_scene.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification-messages.h>
#include <gui/modules/variable-item-list.h>
#include "views/u2f_view.h"
#include "u2f_hid.h"

struct U2FApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    U2FHid* u2f_hid;
    U2F* u2f_view;
};

typedef enum {
    U2FAppViewMain,
} U2FAppView;
