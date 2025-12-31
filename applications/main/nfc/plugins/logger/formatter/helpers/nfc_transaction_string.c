#include "nfc_transaction_string.h"

NfcTransactionString* nfc_transaction_string_alloc() {
    NfcTransactionString* instance = malloc(sizeof(NfcTransactionString));
    FuriString** array = (FuriString**)instance;
    for(size_t i = 0; i < (sizeof(NfcTransactionString) / sizeof(FuriString*)); i++) {
        array[i] = furi_string_alloc();
    }
    return instance;
}

void nfc_transaction_string_reset(NfcTransactionString* instance) {
    furi_assert(instance);

    FuriString** array = (FuriString**)instance;
    for(size_t i = 0; i < (sizeof(NfcTransactionString) / sizeof(FuriString*)); i++) {
        furi_string_reset(array[i]);
    }
}

void nfc_transaction_string_free(NfcTransactionString* instance) {
    furi_assert(instance);

    FuriString** array = (FuriString**)instance;
    for(size_t i = 0; i < (sizeof(NfcTransactionString) / sizeof(FuriString*)); i++) {
        furi_string_free(array[i]);
    }
    free(instance);
}
