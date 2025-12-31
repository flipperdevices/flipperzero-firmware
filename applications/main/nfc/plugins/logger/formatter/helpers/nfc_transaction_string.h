#pragma once

#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FuriString* id;
    FuriString* type;
    FuriString* time;
    FuriString* src;
    FuriString* payload;
    FuriString* crc_status;
    FuriString* annotation;
} NfcTransactionString;

NfcTransactionString* nfc_transaction_string_alloc(void);
void nfc_transaction_string_reset(NfcTransactionString* instance);
void nfc_transaction_string_free(NfcTransactionString* instance);
#ifdef __cplusplus
}
#endif
