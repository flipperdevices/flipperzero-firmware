#pragma once

#include "mass_storage_app.h"
#include "scenes/mass_storage_scene.h"
#include "helpers/mass_storage_usb.h"

#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <storage/storage.h>
#include "views/mass_storage_view.h"

#define MASS_STORAGE_APP_PATH_FOLDER STORAGE_APP_DATA_PATH_PREFIX
#define MASS_STORAGE_FILE_NAME_LEN 40

typedef enum {
    SizeUnitBytes,
    SizeUnitKb,
    SizeUnitMb,
    SizeUnitGb,
    SizeUnitCount,
} SizeUnit;

struct MassStorageApp {
    Gui* gui;
    Storage* fs_api;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    MassStorage* mass_storage_view;
    VariableItemList* var_item_list;
    Submenu* submenu;
    TextInput* text_input;
    Popup* popup;

    uint32_t create_image_size;
    SizeUnit create_size_unit;
    char create_name[MASS_STORAGE_FILE_NAME_LEN];

    FuriString* file_path;
    File* file;

    FuriMutex* usb_mutex;
    MassStorageUsb* usb;
};

typedef enum {
    MassStorageAppViewError,
    MassStorageAppViewWork,
    MassStorageAppViewVarItemList,
    MassStorageAppViewSubmenu,
    MassStorageAppViewTextInput,
    MassStorageAppViewPopup,
} MassStorageAppView;
