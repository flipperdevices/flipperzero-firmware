#pragma once

#include "mf_classic_listener.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>
#include <nfc/protocols/nfc_generic_event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfClassicListenerStateIdle,
    MfClassicListenerStateAuthComplete,
} MfClassicListenerState;

struct MfClassicListener {
    Iso14443_3aListener* iso14443_3a_listener;
    MfClassicListenerState state;

    MfClassicData* data;
    BitBuffer* tx_plain_buffer;
    BitBuffer* tx_encrypted_buffer;
    BitBuffer* rx_plain_buffer;

    NfcGenericEvent generic_event;
    MfClassicListenerEvent mfu_event;
    MfClassicListenerEventData mfu_event_data;
    NfcGenericCallback callback;
    void* context;
};

#ifdef __cplusplus
}
#endif
