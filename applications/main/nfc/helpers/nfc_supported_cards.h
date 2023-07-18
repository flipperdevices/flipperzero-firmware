#pragma once

#include <core/string.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

bool nfc_supported_cards_read(NfcDevice* device, Nfc* nfc);

bool nfc_supported_cards_parse(const NfcDevice* device, FuriString* parsed_data);

#ifdef __cplusplus
}
#endif
