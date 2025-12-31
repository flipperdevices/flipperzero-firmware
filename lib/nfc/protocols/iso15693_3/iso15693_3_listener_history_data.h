#pragma once

#include "iso15693_3.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso15693_3ListenerEventTypeFieldOff,
    Iso15693_3ListenerEventTypeCustomCommand,
    Iso15693_3ListenerEventTypeSingleEof,
} Iso15693_3ListenerEventType;

typedef enum {
    Iso15693_3ListenerStateReady,
    Iso15693_3ListenerStateSelected,
    Iso15693_3ListenerStateQuiet,
} Iso15693_3ListenerState;

typedef struct {
    NfcEventType event;
    Iso15693_3ListenerState state;
    Iso15693_3Error error;
    NfcCommand command;
} FURI_PACKED Iso15693_3ListenerHistoryData;

#ifdef __cplusplus
}
#endif
