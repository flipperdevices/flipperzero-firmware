#pragma once

#include <furi_hal_nfc.h>

typedef struct {
    uint8_t* kmrz;
    uint8_t ksenc[16];
    uint8_t ksmac[16];
    uint64_t ssc_long;
} MrtdApplication;

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
} MrtdDate;

// NULL terminated document ID
#define MRTD_DOCNR_MAX_LENGTH 21

typedef enum {
    MrtdAuthMethodBac,
    MrtdAuthMethodPace,
} MrtdAuthMethod;

typedef struct {
    MrtdAuthMethod method;

    // BAC input fields
    MrtdDate birth_date;
    MrtdDate expiry_date;
    char doc_number[MRTD_DOCNR_MAX_LENGTH];

    //TODO: PACE
} MrtdAuthData;

typedef struct {
    MrtdAuthData auth;
} MrtdData;

/** Select the LDS1 eMRTD application
 * @note Can be used to detect presence of Passport/ID-card
 *
 * @param tx_rx     FuriHalNfcTxRxContext instance
 * @param emv_app   MrtdApplication instance
 * 
 * @return true on success
 */
bool mrtd_select_lds1(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app);
