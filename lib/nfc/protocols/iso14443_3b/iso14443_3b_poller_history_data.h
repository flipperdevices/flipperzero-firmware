#pragma once

#include "iso14443_3b.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_3bPollerStateIdle,
    Iso14443_3bPollerStateColResInProgress,
    Iso14443_3bPollerStateColResFailed,
    Iso14443_3bPollerStateActivationInProgress,
    Iso14443_3bPollerStateActivationFailed,
    Iso14443_3bPollerStateActivated,
} Iso14443_3bPollerState;

typedef struct {
    NfcEventType event;
    Iso14443_3bPollerState state;
    Iso14443_3bError error;
    NfcCommand command;
} FURI_PACKED Iso14443_3bPollerHistoryData;

#ifdef __cplusplus
}
#endif
