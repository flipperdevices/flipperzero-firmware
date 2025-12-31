#pragma once

#include "nfc_history.h"

#include <stream/file_stream.h>

#ifdef __cplusplus
extern "C" {
#endif

NfcHistory* nfc_history_alloc(uint8_t history_size_bytes, uint8_t max_chain_count);
void nfc_history_free(NfcHistory* instance);

void nfc_history_append(NfcHistory* instance, const NfcHistoryItem* item);

bool nfc_history_load(Stream* stream, NfcHistory** instance_ptr);
bool nfc_history_save(Stream* stream, const NfcHistory* instance);

#ifdef __cplusplus
}
#endif
