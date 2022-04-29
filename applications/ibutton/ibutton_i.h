#pragma once

#include "ibutton.h"

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
// #include <cli/cli.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
// #include <gui/modules/dialog_ex.h>
// #include <gui/modules/popup.h>
// #include <gui/modules/text_input.h>
// #include <gui/modules/byte_input.h>
// #include <gui/modules/text_box.h>
// #include <gui/modules/widget.h>

#include "scenes/ibutton_scene.h"

struct iButton {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;

    // Common Views
    Submenu* submenu;
};

typedef enum {
    iButtonViewSubmenu,
} iButtonView;

iButton* ibutton_alloc();
