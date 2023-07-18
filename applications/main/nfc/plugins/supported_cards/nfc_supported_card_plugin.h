#pragma once

#include <furi/core/string.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#define NFC_SUPPORTED_CARD_PLUGIN_APP_ID "NfcSupportedCardPlugin"
#define NFC_SUPPORTED_CARD_PLUGIN_API_VERSION 1

typedef bool (*NfcSupportedCardPluginVerify)(Nfc* nfc);

typedef bool (*NfcSupportedCardPluginRead)(Nfc* nfc, NfcDevice* device);

typedef bool (*NfcSupportedCardPluginParse)(const NfcDevice* device, FuriString* parsed_data);

typedef struct {
    NfcProtocol protocol;
    NfcSupportedCardPluginVerify verify;
    NfcSupportedCardPluginRead read;
    NfcSupportedCardPluginParse parse;
} NfcSupportedCardsPlugin;
