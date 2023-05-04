#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lib/nfc/protocols/nfca/nfca_poller.h>
#include <lib/nfc/protocols/nfcb/nfcb_poller.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

typedef struct NfcPoller NfcPoller;

typedef struct {
    Nfc* nfc;
    NfcaPoller* nfca_poller;
    NfcbPoller* nfcb_poller;
} NfcPollerCollection;

typedef enum {
    NfcPollerEventNfcaDetected,
    NfcPollerEventNfcbDetected,
    NfcPollerEventNfcfDetected,
    NfcPollerEventNfcvDetected,
    NfcPollerEventMfUltralightDetected,
    NfcPollerEventMfClassicDetected,
} NfcPollerEvent;

typedef enum {
    NfcPollerCommandContinue = NfcCommandContinue,
    NfcPollerCommandReset = NfcCommandReset,
    NfcPollerCommandStop = NfcCommandStop,
} NfcPollerCommand;

typedef NfcPollerCommand (*NfcPollerEventCallback)(NfcPollerEvent event, void* context);

NfcPoller* nfc_poller_alloc(NfcPollerCollection* pollers);

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context);

void nfc_poller_stop(NfcPoller* instance);

#ifdef __cplusplus
}
#endif
