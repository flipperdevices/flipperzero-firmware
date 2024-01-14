#pragma once

#include <furi.h>

#include <dialogs/dialogs.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/submenu.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#include <storage/storage.h>

#include "mfc_editor_app.h"
#include "mfc_editor_icons.h"
#include "scenes/mfc_editor_scene.h"

#define TAG "MFCEditor"

#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

struct MfcEditorApp {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;

    Submenu* submenu;

    NfcDevice* nfc_device;

    FuriString* file_path;
};

typedef enum {
    MfcEditorAppViewSubmenu,
} MfcEditorAppView;

bool mfc_editor_prompt_load_file(MfcEditorApp* instance);

bool mfc_editor_load_file(MfcEditorApp* instance, FuriString* file_path, bool show_dialog);
