#pragma once

#include "mf_desfire_poller.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MF_DESFIRE_POLLER_STANDARD_FWT_FC (60000)

typedef enum {
    MfDesfirePollerStateIdle,
    MfDesfirePollerStateReadVersion,
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
    Iso14443_4aPoller* iso14443_4a_poller;
    MfDesfirePollerSessionState session_state;
    MfDesfirePollerState state;
    NfcPollerBuffer* buffer;
    MfDesfireData* data;
    MfDesfirePollerCallback callback;
    MfDesfireError error;
    void* context;
};

MfDesfireError mf_desfire_process_error(Iso14443_4aError error);

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data);

#ifdef __cplusplus
}
#endif
