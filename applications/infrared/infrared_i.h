#pragma once

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/button_panel.h>

#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include <notification/notification_messages.h>

#include <infrared_worker.h>

#include "infrared.h"

#include "scenes/infrared_scene.h"
#include "views/infrared_progress_view.h"

#define INFRARED_FILE_NAME_SIZE 100
#define INFRARED_TEXT_STORE_NUM 2
#define INFRARED_TEXT_STORE_SIZE 128

#define INFRARED_APP_FOLDER "/any/infrared"
#define INFRARED_APP_EXTENSION ".ir"
#define INFRARED_APP_FILE_TYPE "Flipper iButton key"

struct Infrared {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;
    NotificationApp* notifications;
    InfraredWorker* worker;

    Submenu* submenu;
    TextInput* text_input;
    DialogEx* dialog_ex;
    ButtonMenu* button_menu;
    Popup* popup;

    ViewStack* view_stack;

    ButtonPanel* button_panel;
    Loading* loading;
    InfraredProgressView* progress;

    string_t file_path;
    char text_store[INFRARED_TEXT_STORE_NUM][INFRARED_TEXT_STORE_SIZE + 1];
};

typedef enum {
    InfraredViewSubmenu,
    InfraredViewTextInput,
    InfraredViewDialogEx,
    InfraredViewButtonMenu,
    InfraredViewPopup,
    InfraredViewStack,
} InfraredView;

typedef enum {
    iButtonNotificationMessageSuccess,
    iButtonNotificationMessageGreenOn,
    iButtonNotificationMessageGreenOff,
    iButtonNotificationMessageBlinkRead,
    iButtonNotificationMessageBlinkSend,
} InfraredNotificationMessage;

bool infrared_remote_select_file(Infrared* infrared);
void infrared_text_store_set(Infrared* infrared, uint32_t bank, const char* text, ...);
void infrared_text_store_clear(Infrared* infrared, uint32_t bank);
void infrared_play_notification_message(Infrared* infrared, uint32_t message);
