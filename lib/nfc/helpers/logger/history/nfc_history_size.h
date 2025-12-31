#pragma once

#include "nfc_history_data_type.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t nfc_history_get_size_bytes(NfcProtocol protocol, NfcMode mode, uint8_t max_chain_count);

#ifdef __cplusplus
}
#endif
