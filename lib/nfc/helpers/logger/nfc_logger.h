#pragma once

#include <furi.h>
#include <furi_hal_nfc.h>
#include <stream/file_stream.h>

#include <nfc/protocols/nfc_protocol.h>
#include "transaction/nfc_transaction.h"
#include "history/nfc_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcLogger NfcLogger;

void nfc_logger_config(NfcLogger* instance, bool enabled, const char* log_folder_path);
bool nfc_logger_raw_log_file_present(NfcLogger* instance, FuriString* output);
#ifdef __cplusplus
}
#endif
