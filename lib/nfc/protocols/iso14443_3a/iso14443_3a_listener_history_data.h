#pragma once

#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_3aListenerStateIdle,
    Iso14443_3aListenerStateActive,
} Iso14443_3aListenerState;

typedef struct {
    NfcEventType event;
    Iso14443_3aListenerState state;
    NfcCommand command;
} FURI_PACKED Iso14443_3aListenerHistoryData;

#ifdef __cplusplus
}
#endif
