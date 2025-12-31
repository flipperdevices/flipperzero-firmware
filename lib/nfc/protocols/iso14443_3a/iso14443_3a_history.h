#pragma once

#include "iso14443_3a_poller_i.h"
#include "iso14443_3a_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    Iso14443_3aPollerEventType poller_event;
    Iso14443_3aListenerEventType listener_event;
} NfcHistoryEventType_Iso14443_3a;

typedef union {
    Iso14443_3aPollerState poller_state;
} NfcHistoryState_Iso14443_3a;

#ifdef __cplusplus
}
#endif
