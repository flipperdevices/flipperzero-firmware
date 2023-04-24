#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "protocols/nfca_poller.h"
#include "protocols/nfcb_poller.h"
#include "protocols/mf_ultralight.h"

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

typedef void (*NfcPollerEventCallback)(NfcPollerEvent event, void* context);

NfcPoller* nfc_poller_alloc(NfcPollerCollection* pollers);

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context);

void nfc_poller_stop(NfcPoller* instance);

#ifdef __cplusplus
}
#endif
