#pragma once

#include <nfc/protocols/nfc_generic_event.h>

#include "slix_listener.h"
#include "slix_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t random;
    bool is_password_valid[SlixPasswordTypeCount];
} SlixListenerSessionState;

struct SlixListener {
    Iso15693_3Listener* iso15693_3_listener;
    SlixData* data;
    SlixListenerSessionState session_state;

    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    SlixListenerEvent slix_event;
    SlixListenerEventData slix_event_data;
    NfcGenericCallback callback;
    void* context;
};

SlixError slix_listener_set_password(
    SlixListener* instance,
    SlixPasswordType password_type,
    SlixPasswordValue password);

SlixError slix_listener_process_request(SlixListener* instance, const BitBuffer* rx_buffer);

#ifdef __cplusplus
}
#endif
