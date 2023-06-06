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

typedef enum {
    MfDesfirePollerCommandContinue = NfcaPollerCommandContinue,
    MfDesfirePollerCommandReset = NfcaPollerCommandReset,
    MfDesfirePollerCommandStop = NfcaPollerCommandStop,
} MfDesfirePollerCommand;

typedef MfDesfirePollerCommand (
    *MfDesfirePollerCallback)(MfDesfirePollerEvent event, void* context);

MfDesfirePoller* mf_desfire_poller_alloc(Iso14443_4aPoller* iso14443_4a_poller);

void mf_desfire_poller_free(MfDesfirePoller* instance);

MfDesfireError mf_desfire_poller_start(
    MfDesfirePoller* instance,
    Iso14443_4aPollerCallback callback,
    void* context);

MfDesfireError mf_desfire_poller_read(
    MfDesfirePoller* instance,
    MfDesfirePollerCallback callback,
    void* context);

MfDesfireError mf_desfire_poller_get_data(MfDesfirePoller* instance, MfDesfireData* data);

MfDesfireError mf_desfire_poller_stop(MfDesfirePoller* instance);

#ifdef __cplusplus
}
#endif
