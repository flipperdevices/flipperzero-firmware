#pragma once

#include <furi.h>
#include <furi_hal_random.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>

#include <storage/storage.h>

#include <lib/toolbox/path.h>

#include <lib/nfc/nfc_device.h>

#include "nfc_crocodile_icons.h"
#include "nfc_crocodile_worker.h"
#include "scenes/nfc_crocodile_scene.h"

typedef enum {
    NfcCrocodileViewMenu,
    NfcCrocodileViewPopup,
    NfcCrocodileViewVariableItemList,
    NfcCrocodileViewDialog
} NfcCrocodileView;

typedef struct NfcCrocodile {
    // GUI
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Popup* popup;
    VariableItemList* variable_item_list;
    DialogEx* dialog;
    //DialogsApp* dialogs;

    // Storage
    Storage* storage;

    // Internal
    NfcCrocodileWorker* nfc_crocodile_worker;
    NfcCrocodileStorageType storage_type;
    char* card_content;
} NfcCrocodile;