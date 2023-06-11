#pragma once

#include <furi/core/string.h>
#include <nfc/protocols/nfc_protocol_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcSupportedCards NfcSupportedCards;

NfcSupportedCards* nfc_supported_cards_alloc();

void nfc_supported_cards_free(NfcSupportedCards* instance);

//FIXME: Make it compile
// bool nfc_supported_cards_read(
//     NfcSupportedCards* instance,
//     NfcProtocolType protocol,
//     void* poller,
//     void* data);
//
// bool nfc_supported_cards_parse(
//     NfcSupportedCards* instance,
//     NfcProtocolType protocol,
//     void* data,
//     FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
