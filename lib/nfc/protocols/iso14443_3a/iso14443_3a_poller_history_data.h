#pragma once

#include "iso14443_3a.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_3aPollerStateIdle,
    Iso14443_3aPollerStateColResInProgress,
    Iso14443_3aPollerStateColResFailed,
    Iso14443_3aPollerStateActivated,
} Iso14443_3aPollerState;

typedef struct {
    NfcEventType event;
    Iso14443_3aPollerState state;
    Iso14443_3aError error;
    NfcCommand command;
} FURI_PACKED Iso14443_3aPollerHistoryData;

#ifdef __cplusplus
}
#endif
