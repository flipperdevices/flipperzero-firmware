#pragma once

#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <gui/view.h>

#include <gui/modules/submenu.h>

#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include <notification/notification_messages.h>

#include "infrared.h"
#include "scenes/infrared_scene.h"

struct Infrared {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;
    NotificationApp* notifications;

    Submenu* submenu;
};

typedef enum {
    InfraredViewSubmenu,
} InfraredView;
