#pragma once

#include <nfc.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_4aListenerStateIdle,
    Iso14443_4aListenerStateActive,
} Iso14443_4aListenerState;

typedef struct {
    Iso14443_3aListenerEventType event;
    Iso14443_4aListenerState state;
    NfcCommand command;
} FURI_PACKED Iso14443_4aListenerHistoryData;

#ifdef __cplusplus
}
#endif
