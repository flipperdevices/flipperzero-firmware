#pragma once

#include "mf_classic_poller.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>
#include <nfc/helpers/nfc_poller_buffer.h>
#include <lib/nfc/protocols/nfc_util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfClassicPollerSessionStateIdle,
    MfClassicPollerSessionStateActive,
    MfClassicPollerSessionStateStopRequest,
} MfClassicPollerSessionState;

typedef enum {
    MfClassicAuthStateIdle,
    MfClassicAuthStateWaitNt,
    MfClassicAuthStateWaitAt,
    MfClassicAuthStateSuccess,
    MfClassicAuthStateFail,
} MfClassicAuthState;

typedef enum {
    MfClassicPollerStateIdle,    
} MfClassicPollerState;

struct MfClassicPoller {
    NfcaPoller* nfca_poller;
    MfClassicPollerSessionState session_state;
    MfClassicAuthState auth_state;
    NfcPollerBuffer* buffer;
    MfClassicData* data;
    MfClassicPollerCallback callback;
    void* context;
};

#ifdef __cplusplus
}
#endif
