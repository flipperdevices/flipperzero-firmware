#pragma once

#include "nfca.h"
#include <lib/nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaPoller NfcaPoller;

typedef enum {
    NfcaPollerEventTypeError,
    NfcaPollerEventTypeReady,
} NfcaPollerEventType;

typedef struct {
    NfcaError error;
} NfcaPollerEventData;

typedef struct {
    NfcaPollerEventType type;
    NfcaPollerEventData data;
} NfcaPollerEvent;

typedef enum {
    NfcaPollerCommandContinue = NfcCommandContinue,
    NfcaPollerCommandReset = NfcCommandReset,
    NfcaPollerCommandStop = NfcCommandStop,
} NfcaPollerCommand;

typedef NfcaPollerCommand (*NfcaPollerEventCallback)(NfcaPollerEvent event, void* context);

NfcaPoller* nfca_poller_alloc(Nfc* nfc);

void nfca_poller_free(NfcaPoller* instance);

NfcaError nfca_poller_start(NfcaPoller* instance, NfcaPollerEventCallback callback, void* context);

NfcaError nfca_poller_stop(NfcaPoller* instance);

NfcaError nfca_poller_get_data(NfcaPoller* instance, NfcaData* data);

// Sync call
NfcaError nfca_poller_read(NfcaPoller* instance, NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
