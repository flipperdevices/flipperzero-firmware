#pragma once

#include "mf_desfire_poller.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDesfirePollerStateIdle,
    MfDesfirePollerStateReadFailed,
    MfDesfirePollerStateReadSuccess,

    MfDesfirePollerStateNum,
} MfDesfirePollerState;

typedef enum {
    MfDesfirePollerSessionStateIdle,
    MfDesfirePollerSessionStateActive,
    MfDesfirePollerSessionStateStopRequest,
} MfDesfirePollerSessionState;

struct MfDesfirePoller {
    NfcaPoller* nfca_poller;
    MfDesfirePollerSessionState session_state;
    MfDesfirePollerState state;
    NfcPollerBuffer* buffer;
    MfDesfireData* data;
    MfDesfirePollerCallback callback;
    MfDesfireError error;
    void* context;
};

#ifdef __cplusplus
}
#endif
