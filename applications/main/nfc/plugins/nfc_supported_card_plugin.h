#pragma once

#include <lib/nfc/nfc_device_data.h>
#include <furi/core/string.h>

#define NFC_SUPPORTED_CARD_PLUGIN_APP_ID "NfcSupportedCardPlugin"
#define NFC_SUPPORTED_CARD_PLUGIN_API_VERSION 1

typedef bool (*NfcSupportedCardPluginVerify)(void* poller);

typedef bool (*NfcSupportedCardPluginRead)(void* poller, void* data);

typedef bool (*NfcSupportedCardPluginParse)(void* data, FuriString* parsed_data);

typedef struct {
    NfcDevProtocol protocol;
    NfcSupportedCardPluginVerify verify;
    NfcSupportedCardPluginRead read;
    NfcSupportedCardPluginParse parse;
} NfcSupportedCardsPlugin;
