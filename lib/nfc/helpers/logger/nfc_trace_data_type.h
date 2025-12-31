
#pragma once

#include <furi.h>
#include <nfc_mode.h>
#include <nfc/protocols/nfc_protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NfcProtocol protocol;
    NfcMode mode;
    size_t transactions_count;
} FURI_PACKED NfcTrace;

#ifdef __cplusplus
}
#endif
