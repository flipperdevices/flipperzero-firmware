#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <nfc/protocols/nfc_poller_base.h>

typedef struct NfcPollerManager NfcPollerManager;

NfcPollerManager* nfc_poller_manager_alloc(Nfc* nfc);

void nfc_poller_manager_free(NfcPollerManager* instance);

void nfc_poller_manager_start(
    NfcPollerManager* instance,
    NfcProtocolType protocol,
    NfcPollerCallback callback,
    void* context);

void nfc_poller_manager_stop(NfcPollerManager* instance);

#ifdef __cplusplus
}
#endif
