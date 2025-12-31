#pragma once

#include "../nfc_formatter_context.h"

#include <furi.h>
#include <nfc/helpers/logger/nfc_trace_data_type.h>
#include <nfc/helpers/logger/transaction/nfc_transaction.h>

#ifdef __cplusplus
extern "C" {
#endif

void nfc_transaction_format(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    FuriString* output);
#ifdef __cplusplus
}
#endif
