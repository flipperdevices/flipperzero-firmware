#pragma once

#include "mf_plus.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfPlusPollerStateIdle,
    MfPlusPollerStateReadVersion,
    MfPlusPollerStateParseVersion,
    MfPlusPollerStateParseIso4,
    MfPlusPollerStateReadFailed,
    MfPlusPollerStateReadSuccess,

    MfPlusPollerStateNum,
} MfPlusPollerState;

typedef struct {
    Iso14443_4aPollerEventType event;
    MfPlusPollerState state;
    MfPlusError error;
    NfcCommand command;
} FURI_PACKED MfPlusPollerHistoryData;

#ifdef __cplusplus
}
#endif
