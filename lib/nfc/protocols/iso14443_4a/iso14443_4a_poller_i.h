#pragma once

#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller_i.h>

#include "iso14443_4a_poller.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_4A_POLLER_ATS_FWT_FC (12000)

typedef enum {
    Iso14443_4aPollerStateIdle,
    Iso14443_4aPollerStateReadAts,
    Iso14443_4aPollerStateError,
    Iso14443_4aPollerStateReady,

    Iso14443_4aPollerStateNum,
} Iso14443_4aPollerState;

typedef enum {
    Iso14443_4aPollerSessionStateIdle,
    Iso14443_4aPollerSessionStateActive,
    Iso14443_4aPollerSessionStateStopRequest,
} Iso14443_4aPollerSessionState;

typedef struct {
    uint32_t block_number;
} Iso14443_4aPollerProtocolState;

struct Iso14443_4aPoller {
    Iso14443_3aPoller* iso14443_3a_poller;
    Iso14443_4aPollerState poller_state;
    Iso14443_4aPollerSessionState session_state;
    Iso14443_4aPollerProtocolState protocol_state;
    Iso14443_4aError error;
    Iso14443_4aData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    Iso14443_4aPollerEventData iso14443_4a_event_data;
    Iso14443_4aPollerEvent iso14443_4a_event;
    NfcGenericEvent general_event;
    NfcGenericCallback callback;
    void* context;
};

Iso14443_4aError iso14443_4a_poller_process_error(Iso14443_3aError error);

const Iso14443_4aData* iso14443_4a_poller_get_data(Iso14443_4aPoller* instance);

Iso14443_4aError iso14443_4a_poller_halt(Iso14443_4aPoller* instance);

Iso14443_4aError
    iso14443_4a_poller_async_read_ats(Iso14443_4aPoller* instance, Iso14443_4aAtsData* data);

Iso14443_4aError iso14443_4a_poller_send_block(
    Iso14443_4aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

#ifdef __cplusplus
}
#endif