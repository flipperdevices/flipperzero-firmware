#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcPoller NfcPoller;

typedef enum {
    NfcPollerEventNfcaDetected,
    NfcPollerEventNfcbDetected,
    NfcPollerEventNfcfDetected,
    NfcPollerEventNfcvDetected,
} NfcPollerEvent;

typedef void (*NfcPollerEventCallback)(NfcPollerEvent event, void* context);

NfcPoller* nfc_poller_alloc();

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context);

#ifdef __cplusplus
}
#endif
