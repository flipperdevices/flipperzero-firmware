#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>

#include <lib/nfc/protocols/mf_classic/mf_classic.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

#define TAG "Troika"

// static const MfClassicAuthContext troika_keys[] = {
//     {.sector = 0, .key_a = 0xa0a1a2a3a4a5, .key_b = 0xfbf225dc5d58},
//     {.sector = 1, .key_a = 0xa82607b01c0d, .key_b = 0x2910989b6880},
//     {.sector = 2, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
//     {.sector = 3, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
//     {.sector = 4, .key_a = 0x73068f118c13, .key_b = 0x2b7f3253fac5},
//     {.sector = 5, .key_a = 0xfbc2793d540b, .key_b = 0xd3a297dc2698},
//     {.sector = 6, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
//     {.sector = 7, .key_a = 0xae3d65a3dad4, .key_b = 0x0f1c63013dba},
//     {.sector = 8, .key_a = 0xa73f5dc1d333, .key_b = 0xe35173494a81},
//     {.sector = 9, .key_a = 0x69a32f1c2f19, .key_b = 0x6b8bd9860763},
//     {.sector = 10, .key_a = 0x9becdf3d9273, .key_b = 0xf8493407799d},
//     {.sector = 11, .key_a = 0x08b386463229, .key_b = 0x5efbaecef46b},
//     {.sector = 12, .key_a = 0xcd4c61c26e3d, .key_b = 0x31c7610de3b0},
//     {.sector = 13, .key_a = 0xa82607b01c0d, .key_b = 0x2910989b6880},
//     {.sector = 14, .key_a = 0x0e8f64340ba4, .key_b = 0x4acec1205d75},
//     {.sector = 15, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
// };

bool troika_verify(void* poller) {
    furi_assert(poller);

    bool verified = false;
    MfClassicPoller* mfc_poller = poller;
    uint8_t sector = 11;
    uint8_t block = mf_classic_get_sector_trailer_num_by_sector(sector);
    MfClassicKey key = {.data = {0x08, 0xb3, 0x86, 0x46, 0x32, 0x29}};
    MfClassicAuthContext auth_context = {};

    FURI_LOG_D("Troika", "Verifying sector %d", sector);
    if(mf_classic_poller_auth(mfc_poller, block, &key, MfClassicKeyTypeA, &auth_context) ==
       MfClassicErrorNone) {
        FURI_LOG_D(TAG, "Sector %d verified", sector);
        verified = true;
    }

    return verified;
}

bool troika_read(void* poller, void* data) {
    furi_assert(poller);
    furi_assert(data);

    return true;
}

bool troika_parse(void* data, FuriString* parsed_data) {
    furi_assert(data);

    furi_string_printf(parsed_data, "%s", "Hello Troika");

    return true;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin troika_plugin = {
    .protocol = NfcDevProtocolMfClassic,
    .verify = troika_verify,
    .read = troika_read,
    .parse = troika_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor troika_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &troika_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* troika_plugin_ep() {
    return &troika_plugin_descriptor;
}
