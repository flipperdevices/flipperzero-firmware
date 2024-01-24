#pragma once

#include <furi.h>

#include <dialogs/dialogs.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/byte_input.h>
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
    MfcEditorCustomEventSave,
};

typedef struct {
    uint8_t bits : 3;
    uint8_t check_bits : 3;
} MfcEditorAccessBits;

struct MfcEditorApp {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;

    Submenu* submenu;
    Popup* popup;
    DialogEx* dialog_ex;
    ByteInput* byte_input;

    MfClassicData* mf_classic_data;

    FuriString* file_path;

    bool is_unsaved_changes;

    uint8_t current_sector;
    uint8_t current_block;

    // DialogEx doesn't copy the strings given to it, so we need these
    FuriString* data_view_header;
    FuriString* data_view_text;

    uint8_t* edit_buffer;
    MfcEditorAccessBits access_bits_edit;
};

typedef enum {
    MfcEditorAppViewSubmenu,
    MfcEditorAppViewPopup,
    MfcEditorAppViewDialogEx,
    MfcEditorAppViewByteInput,
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
    MfcEditorSaveResponseSave,
    MfcEditorSaveResponseDiscard,
    MfcEditorSaveResponseCancel,
} MfcEditorSaveResponse;

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

bool mfc_editor_save_file(MfcEditorApp* instance);

// Warning dialogs

bool mfc_editor_warn_risky_operation(MfcEditorApp* instance);

MfcEditorSaveResponse mfc_editor_warn_unsaved_changes(MfcEditorApp* instance);

// Helper methods

uint8_t mfc_editor_calculate_uid_bcc(uint8_t* uid, uint8_t uid_len);

MfcEditorAccessBits mfc_editor_get_block_access_bits(const MfClassicData* data, uint8_t block_num);

void mfc_editor_set_block_access_bits(
    MfClassicData* data,
    uint8_t block_num,
    const MfcEditorAccessBits* access_bits);

void mfc_editor_furi_string_render_bytes(FuriString* string, const uint8_t* data, uint8_t length);

// Strings

extern const char* access_data_block_labels[8];

extern const char* access_sector_trailer_labels[8];
