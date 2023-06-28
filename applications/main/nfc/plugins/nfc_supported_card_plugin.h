#pragma once

#include <furi/core/string.h>
#include <nfc/protocols/nfc_protocol.h>

#define NFC_SUPPORTED_CARD_PLUGIN_APP_ID "NfcSupportedCardPlugin"
#define NFC_SUPPORTED_CARD_PLUGIN_API_VERSION 1

typedef bool (*NfcSupportedCardPluginVerify)(void* poller);

typedef bool (*NfcSupportedCardPluginRead)(void* poller, void* data);

typedef bool (*NfcSupportedCardPluginParse)(void* data, FuriString* parsed_data);

typedef struct {
    NfcProtocol protocol;
    NfcSupportedCardPluginVerify verify;
    NfcSupportedCardPluginRead read;
    NfcSupportedCardPluginParse parse;
} NfcSupportedCardsPlugin;
