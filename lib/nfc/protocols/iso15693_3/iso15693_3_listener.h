#pragma once

#include <nfc/nfc_listener.h>

#include "iso15693_3_listener_history_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso15693_3Listener Iso15693_3Listener;

typedef struct {
    BitBuffer* buffer;
} Iso15693_3ListenerEventData;

typedef struct {
    Iso15693_3ListenerEventType type;
    Iso15693_3ListenerEventData* data;
} Iso15693_3ListenerEvent;

#ifdef __cplusplus
}
#endif
