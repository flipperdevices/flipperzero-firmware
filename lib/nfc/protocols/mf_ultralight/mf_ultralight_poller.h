#pragma once

#include "mf_ultralight.h"
#include <lib/nfc/protocols/nfca/nfca_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfUltralightPoller MfUltralightPoller;

typedef enum {
    MfUltralightPollerEventTypeAuthRequest,
    MfUltralightPollerEventTypeAuthSuccess,
    MfUltralightPollerEventTypeAuthFailed,
    MfUltralightPollerEventTypeReadSuccess,
    MfUltralightPollerEventTypeReadFailed,
} MfUltralightPollerEventType;

typedef struct {
    MfUltralightAuthPassword password;
    MfUltralightAuthPack pack;
    bool auth_success;
    bool skip_auth;
} MfUltralightPollerAuthContext;

typedef struct {
    union {
        MfUltralightPollerAuthContext auth_context;
        MfUltralightError error;
    };
} MfUltralightPollerEventData;

typedef struct {
    MfUltralightPollerEventType type;
    MfUltralightPollerEventData* data;
} MfUltralightPollerEvent;

const MfUltralightData* mf_ultralight_poller_get_data(MfUltralightPoller* instance);

#ifdef __cplusplus
}
#endif
