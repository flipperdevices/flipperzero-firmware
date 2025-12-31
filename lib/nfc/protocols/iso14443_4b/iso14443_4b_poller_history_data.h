#pragma once

#include <nfc/protocols/iso14443_3b/iso14443_3b_poller.h>
#include "iso14443_4b.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_4bPollerStateIdle,
    Iso14443_4bPollerStateError,
    Iso14443_4bPollerStateReady,

    Iso14443_4bPollerStateNum,
} Iso14443_4bPollerState;

typedef enum {
    Iso14443_4bPollerSessionStateIdle,
    Iso14443_4bPollerSessionStateActive,
    Iso14443_4bPollerSessionStateStopRequest,
} Iso14443_4bPollerSessionState;

typedef struct {
    Iso14443_3bPollerEventType event;
    Iso14443_4bPollerState state;
    Iso14443_4bPollerSessionState session_state;
    Iso14443_4bError error;
    NfcCommand command;
} FURI_PACKED Iso14443_4bPollerHistoryData;

#ifdef __cplusplus
}
#endif
