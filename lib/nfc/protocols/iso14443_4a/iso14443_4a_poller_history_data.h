#pragma once

#include "iso14443_4a.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_4aPollerStateIdle,
    Iso14443_4aPollerStateReadAts,
    Iso14443_4aPollerStateError,
    Iso14443_4aPollerStateReady,

    Iso14443_4aPollerStateNum,
} Iso14443_4aPollerState;

typedef enum {
    Iso14443_4aPollerSessionStateIdle,
    Iso14443_4aPollerSessionStateActive,
    Iso14443_4aPollerSessionStateStopRequest,
} Iso14443_4aPollerSessionState;

typedef struct {
    Iso14443_3aPollerEventType event;
    Iso14443_4aPollerState state;
    Iso14443_4aPollerSessionState session_state;
    Iso14443_4aError error;
    NfcCommand command;
} FURI_PACKED Iso14443_4aPollerHistoryData;

#ifdef __cplusplus
}
#endif
