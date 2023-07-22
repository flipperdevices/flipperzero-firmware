#pragma once

#include "mass_storage_app.h"
#include "scenes/mass_storage_scene.h"
#include "helpers/mass_storage_usb.h"

#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include "views/mass_storage_view.h"

#define MASS_STORAGE_APP_PATH_FOLDER STORAGE_APP_DATA_PATH_PREFIX
#define MASS_STORAGE_APP_EXTENSION ".img"
#define MASS_STORAGE_FILE_NAME_LEN 40

struct MassStorageApp {
    Gui* gui;
    Storage* fs_api;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;

    FuriString* file_path;
    File* file;
    MassStorage* mass_storage_view;

    FuriMutex* usb_mutex;
    MassStorageUsb* usb;
};

typedef enum {
    MassStorageAppViewError,
    MassStorageAppViewWork,
} MassStorageAppView;
