#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lib/nfc/protocols/nfca/nfca_poller.h>
#include <lib/nfc/protocols/nfcb/nfcb_poller.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>
#include <lib/nfc/protocols/mf_desfire/mf_desfire_poller.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

typedef struct NfcPollerOld NfcPollerOld;

typedef struct {
    Nfc* nfc;
    NfcaPoller* nfca_poller;
    NfcbPoller* nfcb_poller;
    MfUltralightPoller* mfu_poller;
    MfClassicPoller* mfc_poller;
} NfcPollerOldCollection;

typedef enum {
    NfcPollerOldEventNfcaDetected,
    NfcPollerOldEventNfcbDetected,
    NfcPollerOldEventNfcfDetected,
    NfcPollerOldEventNfcvDetected,
    NfcPollerOldEventMfUltralightDetected,
    NfcPollerOldEventMfClassicDetected,
    NfcPollerOldEventMfDesfireDetected,
} NfcPollerOldEvent;

typedef enum {
    NfcPollerOldCommandContinue = NfcCommandContinue,
    NfcPollerOldCommandReset = NfcCommandReset,
    NfcPollerOldCommandStop = NfcCommandStop,
} NfcPollerOldCommand;

typedef NfcPollerOldCommand (*NfcPollerOldEventCallback)(NfcPollerOldEvent event, void* context);

NfcPollerOld* nfc_poller_alloc(NfcPollerOldCollection* pollers);

void nfc_poller_free(NfcPollerOld* instance);

void nfc_poller_start(NfcPollerOld* instance, NfcPollerOldEventCallback callback, void* context);

void nfc_poller_stop(NfcPollerOld* instance);

#ifdef __cplusplus
}
#endif
