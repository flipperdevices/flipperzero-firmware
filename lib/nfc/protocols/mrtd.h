#pragma once

#include <furi_hal_nfc.h>

#include "mrtd_helpers.h"

typedef struct {
    FuriHalNfcTxRxContext* tx_rx;
    uint16_t file_offset;
    uint8_t* kmrz;
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    uint64_t ssc_long;
} MrtdApplication;

typedef struct {
    MrtdAuthData auth;
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
bool mrtd_select(MrtdApplication* app, EFFile file);
bool mrtd_select_efcardaccess(MrtdApplication* mrtd_app);
bool mrtd_select_efdir(MrtdApplication* mrtd_app);
void mrtd_test(MrtdApplication* app);

/** Select the LDS1 eMRTD application
 * @note Can be used to detect presence of Passport/ID-card
 *
 * @param emv_app   MrtdApplication instance
 * 
 * @return true on success
 */
bool mrtd_select_lds1(MrtdApplication* mrtd_app);
