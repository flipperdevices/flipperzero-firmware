#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <nfc/protocols/nfc_poller_common.h>

typedef struct NfcPoller NfcPoller;

NfcPoller* nfc_poller_alloc(Nfc* nfc, NfcProtocolType protocol);

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcPollerCallback callback, void* context);

void nfc_poller_stop(NfcPoller* instance);

bool nfc_poller_detect(NfcPoller* instance);

const NfcProtocolData* nfc_poller_get_data(NfcPoller* instance);

#ifdef __cplusplus
}
#endif
