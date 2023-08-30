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

typedef Iso15693_3Error (*Iso15693_3RequestHandler)(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags);

typedef struct {
    union {
        struct {
            Iso15693_3RequestHandler inventory;
            Iso15693_3RequestHandler stay_quiet;
        };
        Iso15693_3RequestHandler mandatory[ISO15693_3_MANDATORY_COUNT];
    };
    union {
        struct {
            Iso15693_3RequestHandler read_block;
            Iso15693_3RequestHandler write_block;
            Iso15693_3RequestHandler lock_block;
            Iso15693_3RequestHandler read_multi;
            Iso15693_3RequestHandler write_multi;
            Iso15693_3RequestHandler select;
            Iso15693_3RequestHandler reset_to_ready;
            Iso15693_3RequestHandler write_afi;
            Iso15693_3RequestHandler lock_afi;
            Iso15693_3RequestHandler write_dsfid;
            Iso15693_3RequestHandler lock_dsfid;
            Iso15693_3RequestHandler get_system;
            Iso15693_3RequestHandler get_multi_blocks_security;
        };
        Iso15693_3RequestHandler optional[ISO15693_3_OPTIONAL_COUNT];
    };
} Iso15693_3ListenerHandlerTable;

// Default ISO15693-3 handler table
extern const Iso15693_3ListenerHandlerTable iso15693_3_handler_table;

struct Iso15693_3Listener {
    Nfc* nfc;
    Iso15693_3Data* data;
    Iso15693_3ListenerState state;
    Iso15693_3ListenerSessionState session_state;
    const Iso15693_3ListenerHandlerTable* handler_table;
    BitBuffer* tx_buffer;

    NfcGenericEvent generic_event;
    Iso15693_3ListenerEvent iso15693_3_event;
    Iso15693_3ListenerEventData iso15693_3_event_data;
    NfcGenericCallback callback;
    void* context;
};

Iso15693_3Error iso15693_3_listener_set_handler_table(
    Iso15693_3Listener* instance,
    const Iso15693_3ListenerHandlerTable* table);

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
