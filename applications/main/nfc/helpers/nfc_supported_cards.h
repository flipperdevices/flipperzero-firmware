#pragma once

#include <lib/nfc/nfc_device_data.h>
#include <furi/core/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcSupportedCards NfcSupportedCards;

NfcSupportedCards* nfc_supported_cards_alloc();

void nfc_supported_cards_free(NfcSupportedCards* instance);

bool nfc_supported_cards_read(
    NfcSupportedCards* instance,
    NfcDevProtocol protocol,
    void* poller,
    void* data);

bool nfc_supported_cards_parse(
    NfcSupportedCards* instance,
    NfcDevProtocol protocol,
    void* data,
    FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
