#pragma once
#include "mf_classic_listener.h"
#include <lib/nfc/helpers/crypto1.h>
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Iso14443_3aListenerEventType event;
    MfClassicListenerCommand mfc_command;
    MfClassicListenerState state;
    MfClassicListenerCommState comms_state;
    bool crc_ok;
    NfcCommand command;
    uint32_t cuid;
    MfClassicKey key;
    MfClassicAuthContext auth_context;
    Crypto1 crypto;
} FURI_PACKED MfClassicListenerHistoryData;

#ifdef __cplusplus
}
#endif
