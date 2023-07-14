#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>

// #include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync_api.h>

#define TAG "Troika"

// TODO: Use a proper data structure ?
typedef struct {
    uint8_t sector;
    uint64_t key_a;
    uint64_t key_b;
} MfClassicKeySet;

static const MfClassicKeySet troika_keys[] = {
    {.sector = 0, .key_a = 0xa0a1a2a3a4a5, .key_b = 0xfbf225dc5d58},
    {.sector = 1, .key_a = 0xa82607b01c0d, .key_b = 0x2910989b6880},
    {.sector = 2, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
    {.sector = 3, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
    {.sector = 4, .key_a = 0x73068f118c13, .key_b = 0x2b7f3253fac5},
    {.sector = 5, .key_a = 0xfbc2793d540b, .key_b = 0xd3a297dc2698},
    {.sector = 6, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
    {.sector = 7, .key_a = 0xae3d65a3dad4, .key_b = 0x0f1c63013dba},
    {.sector = 8, .key_a = 0xa73f5dc1d333, .key_b = 0xe35173494a81},
    {.sector = 9, .key_a = 0x69a32f1c2f19, .key_b = 0x6b8bd9860763},
    {.sector = 10, .key_a = 0x9becdf3d9273, .key_b = 0xf8493407799d},
    {.sector = 11, .key_a = 0x08b386463229, .key_b = 0x5efbaecef46b},
    {.sector = 12, .key_a = 0xcd4c61c26e3d, .key_b = 0x31c7610de3b0},
    {.sector = 13, .key_a = 0xa82607b01c0d, .key_b = 0x2910989b6880},
    {.sector = 14, .key_a = 0x0e8f64340ba4, .key_b = 0x4acec1205d75},
    {.sector = 15, .key_a = 0x2aa05ed1856f, .key_b = 0xeaac88e5dc99},
};

static bool troika_verify(void* poller) {
    furi_assert(poller);

    bool verified = false;
    // MfClassicPoller* mfc_poller = poller;
    // uint8_t sector = 11;
    // uint8_t block = mf_classic_get_sector_trailer_num_by_sector(sector);
    // MfClassicKey key = {.data = {0x08, 0xb3, 0x86, 0x46, 0x32, 0x29}};
    // MfClassicAuthContext auth_context = {};

    // FURI_LOG_D("Troika", "Verifying sector %d", sector);
    // if(mf_classic_poller_auth(mfc_poller, block, &key, MfClassicKeyTypeA, &auth_context) ==
    //    MfClassicErrorNone) {
    //     FURI_LOG_D(TAG, "Sector %d verified", sector);
    //     verified = true;
    // }

    return verified;
}

static bool troika_read(void* poller, void* data) {
    furi_assert(poller);
    furi_assert(data);

    return true;
}

static uint64_t nfc_util_bytes2num(const uint8_t* src, uint8_t len) {
    furi_assert(src);
    furi_assert(len <= 8);

    uint64_t res = 0;
    while(len--) {
        res = (res << 8) | (*src);
        src++;
    }
    return res;
}

static bool troika_parse(const MfClassicData* data, FuriString* parsed_data) {
    furi_assert(data);
    bool troika_parsed = false;

    do {
        // Verify key
        const MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, 8);

        // TODO: Add nfc_util to API?
        const uint64_t key = nfc_util_bytes2num(sec_tr->key_a.data, 6);
        if(key != troika_keys[8].key_a) break;

        // Verify card type
        if(data->type != MfClassicType1k) break;

        // Parse data
        const uint8_t* temp_ptr = &data->block[8 * 4 + 1].data[5];
        uint16_t balance = ((temp_ptr[0] << 8) | temp_ptr[1]) / 25;
        temp_ptr = &data->block[8 * 4].data[2];
        uint32_t number = 0;
        for(size_t i = 1; i < 5; i++) {
            number <<= 8;
            number |= temp_ptr[i];
        }
        number >>= 4;
        number |= (temp_ptr[0] & 0xf) << 28;

        furi_string_printf(parsed_data, "\e#Troika\nNum: %lu\nBalance: %u RUR.", number, balance);
        troika_parsed = true;
    } while(false);

    return troika_parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin troika_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = troika_verify,
    .read = troika_read,
    .parse = (NfcSupportedCardPluginParse)troika_parse,
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
