#pragma once

#include <furi.h>

#include <dialogs/dialogs.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/submenu.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#include <storage/storage.h>

#include "mfc_editor_app.h"
#include "mfc_editor_icons.h"
#include "scenes/mfc_editor_scene.h"

#define TAG "MFCEditor"

#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

enum MfcEditorCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    MfcEditorCustomEventReserved = 100,

    MfcEditorCustomEventViewExit,
};

struct MfcEditorApp {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;

    Submenu* submenu;
    Popup* popup;
    DialogEx* dialog_ex;

    MfClassicData* mf_classic_data;

    FuriString* file_path;

    uint8_t current_sector;
    uint8_t current_block;

    // DialogEx doesn't copy the strings given to it, so we need these
    FuriString* data_view_header;
    FuriString* data_view_text;
};

typedef struct {
    uint8_t bits : 3;
    uint8_t check_bits : 3;
} MfcEditorAccessBits;

typedef enum {
    MfcEditorAppViewSubmenu,
    MfcEditorAppViewPopup,
    MfcEditorAppViewDialogEx,
} MfcEditorAppView;

typedef enum {
    // Generic
    MfcEditorPromptResponseSuccess,
    MfcEditorPromptResponseFailure,

    MfcEditorPromptResponseNotMfClassic,

    // Backed out of a prompt
    MfcEditorPromptResponseExitedFile,
    MfcEditorPromptResponseExitedShadow,
} MfcEditorPromptResponse;

typedef enum {
    MfcEditorBlockViewNormal,

    // Special options - Sector 0 only
    MfcEditorBlockViewUID,
    MfcEditorBlockViewBCC,
    MfcEditorBlockViewManufacturerBytes,

    // Special options - All sectors
    MfcEditorBlockViewKeyA,
    MfcEditorBlockViewKeyB,
    MfcEditorBlockViewAccessBits,
    MfcEditorBlockViewUserByte,
} MfcEditorBlockView;

// Main loading methods

MfcEditorPromptResponse mfc_editor_prompt_load_file(MfcEditorApp* instance);

MfcEditorPromptResponse mfc_editor_load_file(MfcEditorApp* instance, FuriString* file_path);

// Helper methods

uint8_t mfc_editor_calculate_uid_bcc(uint8_t* uid, uint8_t uid_len);

MfcEditorAccessBits mfc_editor_get_block_access_bits(const MfClassicData* data, uint8_t block_num);
