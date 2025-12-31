#pragma once

#include <furi_hal_nfc.h>
#include <stream/file_stream.h>
#include "../history/nfc_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcTransactionTypeEmpty,
    NfcTransactionTypeFlagsOnly,
    NfcTransactionTypeRequest,
    NfcTransactionTypeResponse,
    NfcTransactionTypeRequestResponse,
} NfcTransactionType;

typedef struct NfcTransaction NfcTransaction;

bool nfc_transaction_read(Stream* stream, NfcTransaction** transaction_ptr);
void nfc_transaction_free(NfcTransaction* instance);

#ifdef __cplusplus
}
#endif
