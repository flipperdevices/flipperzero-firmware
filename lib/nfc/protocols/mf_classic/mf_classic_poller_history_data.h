#pragma once
#include "mf_classic_poller.h"
#include <lib/nfc/helpers/crypto1.h>
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    MfClassicPollerReadContext read_ctx;
} MfClassicPollerModeContextHistoryData;

typedef struct {
    Crypto1 crypto;
    Iso14443_3aPollerEventType event;

    MfClassicPollerState state;
    bool crc_ok;
    bool encrypted;
    NfcCommand command;
    //MfClassicError error;
    MfClassicPollerMode mode;

    MfClassicPollerModeContextHistoryData mode_ctx;
} MfClassicPollerHistoryData;

#ifdef __cplusplus
}
#endif
