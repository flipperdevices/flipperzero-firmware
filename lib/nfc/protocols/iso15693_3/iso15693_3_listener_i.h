#pragma once

#include <nfc/protocols/nfc_generic_event.h>

#include "iso15693_3_listener.h"

#include "iso15693_3_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso15693_3ListenerStateIdle,
    Iso15693_3ListenerStateReady,
    Iso15693_3ListenerStateSelected,
    Iso15693_3ListenerStateQuiet,
} Iso15693_3ListenerState;

typedef struct {
    bool wait_for_eof;
    bool no_reply;
} Iso15693_3ListenerSessionState;

struct Iso15693_3Listener {
    Nfc* nfc;
    Iso15693_3Data* data;
    Iso15693_3ListenerState state;
    Iso15693_3ListenerSessionState session_state;

    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    Iso15693_3ListenerEvent iso15693_3_event;
    Iso15693_3ListenerEventData iso15693_3_event_data;
    NfcGenericCallback callback;
    void* context;
};

Iso15693_3Error iso15693_3_listener_ready(Iso15693_3Listener* instance);

Iso15693_3Error iso15693_3_listener_sleep(Iso15693_3Listener* instance);

Iso15693_3Error
    iso15693_3_listener_send_frame(Iso15693_3Listener* instance, const BitBuffer* tx_buffer);

Iso15693_3Error
    iso15693_3_listener_process_request(Iso15693_3Listener* instance, const BitBuffer* rx_buffer);

#ifdef __cplusplus
}
#endif
