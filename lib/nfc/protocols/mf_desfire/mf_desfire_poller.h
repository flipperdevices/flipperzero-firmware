#pragma once

#include "mf_desfire.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfDesfirePoller MfDesfirePoller;

typedef enum {
    MfDesfirePollerEventTypeReadSuccess,
    MfDesfirePollerEventTypeReadFailed,
} MfDesfirePollerEventType;

typedef struct {
    union {
        MfDesfireError error;
    };
} MfDesfirePollerEventData;

typedef struct {
    MfDesfirePollerEventType type;
    MfDesfirePollerEventData* data;
} MfDesfirePollerEvent;

const MfDesfireData* mf_desfire_poller_get_data(MfDesfirePoller* instance);

#ifdef __cplusplus
}
#endif
