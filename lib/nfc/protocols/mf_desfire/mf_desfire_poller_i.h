#pragma once

#include "mf_desfire_poller.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>

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
    NfcaPoller* nfca_poller;
    MfDesfirePollerSessionState session_state;
    MfDesfirePollerState state;
    NfcPollerBuffer* buffer;
    MfDesfireData* data;
    MfDesfirePollerCallback callback;
    MfDesfireError error;
    void* context;
};

MfDesfireError mf_desfire_process_error(NfcaError error);

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data);

#ifdef __cplusplus
}
#endif
