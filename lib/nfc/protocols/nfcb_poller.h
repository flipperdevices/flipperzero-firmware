#pragma once

#include <lib/nfc/nfc.h>
#include "nfcb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcbPoller NfcbPoller;

typedef enum {
    NfcbPollerEventTypeError,
    NfcbPollerEventDetected,
    NfcbPollerEventTypeReady,
} NfcbPollerEventType;

typedef struct {
    NfcbError error;
} NfcbPollerEventData;

typedef struct {
    NfcbPollerEventType type;
    NfcbPollerEventData data;
} NfcbPollerEvent;

typedef enum {
    NfcbPollerCommandContinue = NfcCommandContinue,
    NfcbPollerCommandReset = NfcCommandReset,
    NfcbPollerCommandStop = NfcCommandStop,
} NfcbPollerCommand;

typedef NfcbPollerCommand (*NfcbPollerEventCallback)(NfcbPollerEvent* event, void* context);

NfcbPoller* nfcb_poller_alloc(Nfc* nfc);

void nfcb_poller_free(NfcbPoller* instance);

NfcbError nfcb_poller_start(NfcbPoller* instance, NfcbPollerEventCallback callback, void* context);

NfcbError nfcb_poller_get_data(NfcbPoller* instance, NfcbData* data);

NfcbError nfcb_poller_reset(NfcbPoller* instance);

NfcbError nfcb_poller_config(NfcbPoller* instance);

NfcbError nfcb_poller_activate(NfcbPoller* instance, NfcbData* data);

#ifdef __cplusplus
}
#endif
