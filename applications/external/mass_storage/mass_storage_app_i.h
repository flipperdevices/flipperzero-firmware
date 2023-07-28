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
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/loading.h>
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
    DialogsApp* dialogs;
    TextInput* text_input;
    VariableItemList* variable_item_list;
    Loading* loading;

    FuriString* file_path;
    File* file;
    MassStorage* mass_storage_view;

    FuriMutex* usb_mutex;
    MassStorageUsb* usb;

    char new_file_name[MASS_STORAGE_FILE_NAME_LEN + 1];
    uint32_t new_file_size;
};

typedef enum {
    MassStorageAppViewStart,
    MassStorageAppViewTextInput,
    MassStorageAppViewWork,
    MassStorageAppViewLoading,
} MassStorageAppView;

enum MassStorageCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    MassStorageCustomEventReserved = 100,

    MassStorageCustomEventFileSelect,
    MassStorageCustomEventNewImage,
    MassStorageCustomEventNameInput,
};

void mass_storage_app_show_loading_popup(MassStorageApp* app, bool show);
