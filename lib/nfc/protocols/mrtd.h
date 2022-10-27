#pragma once

#include <furi_hal_nfc.h>

#include "mrtd_helpers.h"

#define MRTD_APP_FOLDER NFC_APP_FOLDER "/mrtd"
#define MRTD_APP_EXTENSION ".mrtd"

typedef struct {
    FuriHalNfcTxRxContext* tx_rx;
    uint16_t file_offset;
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    uint64_t ssc_long; // TODO: rename without _long

    bool secure_messaging;
} MrtdApplication;

typedef struct {
    MrtdAuthData auth;
    bool auth_success;
    MrtdAuthMethod auth_method_used;

    struct {
        EF_DIR_contents EF_DIR;
        EF_COM_contents EF_COM;
        EF_DG1_contents DG1;
    } files;
} MrtdData;

//TODO: description
MrtdApplication* mrtd_alloc_init(FuriHalNfcTxRxContext* tx_rx);
void mrtd_test(MrtdApplication* app, MrtdData* mrtd_data); //TODO: remove
bool mrtd_select_app(MrtdApplication* app, AIDValue aid);
bool mrtd_authenticate(MrtdApplication* app, MrtdData* mrtd_data);
bool mrtd_read_parse_file(MrtdApplication* app, MrtdData* mrtd_data, EFFile file);

bool mrtd_auth_params_save(Storage* storage, DialogsApp* dialogs, MrtdAuthData* auth_data, const char* file_name);
bool mrtd_auth_params_save_file(Storage* storage, DialogsApp* dialogs, MrtdAuthData* auth_data, const char* file_name, const char* folder, const char* extension);

bool mrtd_auth_params_load(Storage* storage, DialogsApp* dialogs, MrtdAuthData* auth_data, const char* file_path, bool show_dialog);
