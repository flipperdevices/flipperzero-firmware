#pragma once

#include <nfc/protocols/nfc_generic_event.h>
#include <nfc/protocols/nfc_device_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcPoller NfcPoller;

NfcPoller* nfc_poller_alloc(Nfc* nfc, NfcProtocol protocol);

void nfc_poller_free(NfcPoller* instance);

void nfc_poller_start(NfcPoller* instance, NfcGenericCallback callback, void* context);

void nfc_poller_stop(NfcPoller* instance);

bool nfc_poller_detect(NfcPoller* instance);

NfcProtocol nfc_poller_get_protocol(const NfcPoller* instance);

const NfcDeviceData* nfc_poller_get_data(const NfcPoller* instance);

#ifdef __cplusplus
}
#endif
