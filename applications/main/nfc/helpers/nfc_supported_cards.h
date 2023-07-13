#pragma once

#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcSupportedCards NfcSupportedCards;

NfcSupportedCards* nfc_supported_cards_alloc();

void nfc_supported_cards_free(NfcSupportedCards* instance);

// TODO: Implement read
// bool nfc_supported_cards_read(
//     NfcSupportedCards* instance,
//     NfcProtocol protocol,
//     void* poller,
//     void* data);

bool nfc_supported_cards_parse(
    NfcSupportedCards* instance,
    const NfcDevice* device,
    FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
