#pragma once

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcSupportedCards NfcSupportedCards;

NfcSupportedCards* nfc_supported_cards_alloc();

void nfc_supported_cards_free(NfcSupportedCards* instance);

bool nfc_supported_cards_read(NfcSupportedCards* instance, NfcDevice* device, Nfc* nfc);

bool nfc_supported_cards_parse(
    NfcSupportedCards* instance,
    const NfcDevice* device,
    FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
