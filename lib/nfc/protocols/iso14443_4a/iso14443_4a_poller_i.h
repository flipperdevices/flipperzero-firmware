#pragma once

#include <lib/nfc/protocols/nfca/nfca_poller_i.h>

#include "iso14443_4a_poller.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    Iso14443_4aAtsRequest ats_request;
    Iso14443_4aAtsResponse ats_response;
} Iso14443_4aPollerProtocolData;

struct Iso14443_4aPoller {
    NfcaPoller* iso14443_3a_poller;
    Iso14443_4aPollerState state;
    Iso14443_4aPollerSessionState session_state;
    Iso14443_4aData* data;
    Iso14443_4aPollerProtocolData protocol_data;
    Iso14443_4aPollerCallback callback;
    Iso14443_4aError error;
    void* context;
};

Iso14443_4aError iso14443_4a_poller_halt(Iso14443_4aPoller* instance);

Iso14443_4aError iso14443_4a_poller_async_read_ats(Iso14443_4aPoller* instance);

#ifdef __cplusplus
}
#endif
