#pragma once

#include <furi_hal_nfc.h>

#include "mrtd_helpers.h"

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
    bool auth_success; //TODO: register (and display) method used BAC/PACE

    struct {
        EF_DIR_contents EF_DIR;
        EF_COM_contents EF_COM;
        EF_DG1_contents DG1;
    } files;
} MrtdData;

//TODO: description
MrtdApplication* mrtd_alloc_init(FuriHalNfcTxRxContext* tx_rx);
bool mrtd_select_app(MrtdApplication* app, AIDValue aid);
bool mrtd_select_file(MrtdApplication* app, EFFile file);
void mrtd_test(MrtdApplication* app, MrtdData* mrtd_data);
bool mrtd_bac(MrtdApplication* app, MrtdAuthData* auth);
