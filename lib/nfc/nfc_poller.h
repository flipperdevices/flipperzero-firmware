#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <nfc/protocols/nfc_generic_event.h>

typedef struct NfcPoller NfcPoller;

NfcPoller* nfc_poller_alloc(Nfc* nfc, NfcProtocol protocol);

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcGenericCallback callback, void* context);

void nfc_poller_stop(NfcPoller* instance);

bool nfc_poller_detect(NfcPoller* instance);

const NfcDeviceData* nfc_poller_get_data(NfcPoller* instance);

#ifdef __cplusplus
}
#endif
