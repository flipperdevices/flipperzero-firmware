#pragma once

#include <nfc/protocols/nfc_generic_event.h>

#include "iso15693_3_listener.h"

#include "iso15693_3_i.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Iso15693_3ListenerStateReady,
    Iso15693_3ListenerStateSelected,
    Iso15693_3ListenerStateQuiet,
} Iso15693_3ListenerState;

typedef struct {
    bool selected;
    bool addressed;
    bool wait_for_eof;
} Iso15693_3ListenerSessionState;

typedef Iso15693_3Error (*Iso15693_3ListenerOverrideHandler)(void* context, va_list args);

typedef struct {
    Iso15693_3ListenerOverrideHandler mandatory[ISO15693_3_MANDATORY_COUNT];
    Iso15693_3ListenerOverrideHandler optional[ISO15693_3_OPTIONAL_COUNT];
} Iso15693_3ListenerOverrideTable;

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

    const Iso15693_3ListenerOverrideTable* override_table;
    void* override_context;
};

Iso15693_3Error iso15693_3_listener_set_override_table(
    Iso15693_3Listener* instance,
    const Iso15693_3ListenerOverrideTable* table,
    void* context);

Iso15693_3Error iso15693_3_listener_ready(Iso15693_3Listener* instance);

Iso15693_3Error
    iso15693_3_listener_send_frame(Iso15693_3Listener* instance, const BitBuffer* tx_buffer);

Iso15693_3Error
    iso15693_3_listener_process_request(Iso15693_3Listener* instance, const BitBuffer* rx_buffer);

Iso15693_3Error iso15693_3_listener_process_single_eof(Iso15693_3Listener* instance);

Iso15693_3Error iso15693_3_listener_process_uid_mismatch(
    Iso15693_3Listener* instance,
    const BitBuffer* rx_buffer);

#ifdef __cplusplus
}
#endif
