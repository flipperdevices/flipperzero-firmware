#pragma once

#include "ibutton.h"

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
// #include <cli/cli.h>
#include <notification/notification_messages.h>

#include <one_wire/ibutton/ibutton_worker.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include <gui/modules/submenu.h>
// #include <gui/modules/popup.h>
// #include <gui/modules/text_input.h>
// #include <gui/modules/byte_input.h>
// #include <gui/modules/text_box.h>
// #include <gui/modules/widget.h>

#include "scenes/ibutton_scene.h"

#define IBUTTON_FILE_NAME_SIZE 100
#define IBUTTON_TEXT_STORE_SIZE 128

struct iButton {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;
    NotificationApp* notifications;

    iButtonWorker* key_worker;
    iButtonKey* key;

    char file_name[IBUTTON_FILE_NAME_SIZE];
//     char text_store[IBUTTON_TEXT_STORE_SIZE + 1];
//     string_t text_box_store;

    // Common Views
    Submenu* submenu;
};

typedef enum {
    iButtonViewSubmenu,
} iButtonView;

iButton* ibutton_alloc();

bool ibutton_file_select(iButton* ibutton);
bool ibutton_load_key_data(iButton* ibutton, string_t key_path);
