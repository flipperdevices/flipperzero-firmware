#pragma once

#include <nfc/protocols/nfc_generic_event.h>

#include "iso14443_4a_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso14443_4aListenerStateIdle,
    Iso14443_4aListenerStateActive,
} Iso14443_4aListenerState;

struct Iso14443_4aListener {
    Iso14443_3aListener* iso14443_3a_listener;
    Iso14443_4aData* data;
    Iso14443_4aListenerState state;

    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    Iso14443_4aListenerEvent iso14443_4a_event;
    Iso14443_4aListenerEventData iso14443_4a_event_data;
    NfcGenericCallback callback;
    void* context;
};

Iso14443_4aError iso14443_4a_listener_send_block(
    Iso14443_4aListener* instance,
    const BitBuffer* tx_buffer);

#ifdef __cplusplus
}
#endif
