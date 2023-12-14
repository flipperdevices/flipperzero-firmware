#pragma once

#include <core/string.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcSupCards NfcSupCards;

NfcSupCards* nfc_sup_cards_alloc();

void nfc_sup_cards_free(NfcSupCards* instance);

void nfc_sup_cards_load_cache(NfcSupCards* instance);

bool nfc_sup_cards_read(NfcSupCards* instance, NfcDevice* device, Nfc* nfc);

bool nfc_sup_cards_parse(NfcSupCards* instance, NfcDevice* device, FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
