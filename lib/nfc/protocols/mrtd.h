#pragma once

#include <furi_hal_nfc.h>

#include "mrtd_helpers.h"

#define MAX_EFDIR_APPS 4

typedef uint8_t AIDValue[7];

struct AIDSet {
    AIDValue eMRTDApplication;
    AIDValue TravelRecords;
    AIDValue VisaRecords;
    AIDValue AdditionalBiometrics;
};

extern struct AIDSet AID;

typedef struct {
    AIDValue applications[MAX_EFDIR_APPS];
    uint8_t applications_count;
} EF_DIR_contents;

typedef struct {
    FuriHalNfcTxRxContext* tx_rx;
    uint16_t file_offset;
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    uint64_t ssc_long;

    struct {
        EF_DIR_contents EF_DIR;
    } files;
} MrtdApplication;

typedef struct {
    MrtdAuthData auth;
    bool auth_success;
} MrtdData;

typedef struct {
    const uint8_t short_id;
    const uint16_t file_id;
} EFFile;

struct EFFormat {
    // Under Master File (MF)
    EFFile ATR;
    EFFile DIR;
    EFFile CardAccess;
    EFFile CardSecurity;

    // Under LDS1 eMRTD Application
    EFFile COM;
    EFFile SOD;
    EFFile DG1;
    EFFile DG2;
    EFFile DG3;
    EFFile DG4;
    EFFile DG5;
    EFFile DG6;
    EFFile DG7;
    EFFile DG8;
    EFFile DG9;
    EFFile DG10;
    EFFile DG11;
    EFFile DG12;
    EFFile DG13;
    EFFile DG14;
    EFFile DG15;
    EFFile DG16;
};

extern struct EFFormat EF;

//TODO: description
MrtdApplication* mrtd_alloc_init(FuriHalNfcTxRxContext* tx_rx);
bool mrtd_select_app(MrtdApplication* app, AIDValue aid);
bool mrtd_select_file(MrtdApplication* app, EFFile file);
void mrtd_test(MrtdApplication* app, MrtdData* mrtd_data);
bool mrtd_bac(MrtdApplication* app, MrtdAuthData* auth);
