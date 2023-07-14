#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>

#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync_api.h>

#define TAG "Troika"

#define TROIKA_DATA_SECTOR (8U)

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

static const MfClassicKeyPair troika_keys[] = {
    {.a = 0xa0a1a2a3a4a5, .b = 0xfbf225dc5d58},
    {.a = 0xa82607b01c0d, .b = 0x2910989b6880},
    {.a = 0x2aa05ed1856f, .b = 0xeaac88e5dc99},
    {.a = 0x2aa05ed1856f, .b = 0xeaac88e5dc99},
    {.a = 0x73068f118c13, .b = 0x2b7f3253fac5},
    {.a = 0xfbc2793d540b, .b = 0xd3a297dc2698},
    {.a = 0x2aa05ed1856f, .b = 0xeaac88e5dc99},
    {.a = 0xae3d65a3dad4, .b = 0x0f1c63013dba},
    {.a = 0xa73f5dc1d333, .b = 0xe35173494a81},
    {.a = 0x69a32f1c2f19, .b = 0x6b8bd9860763},
    {.a = 0x9becdf3d9273, .b = 0xf8493407799d},
    {.a = 0x08b386463229, .b = 0x5efbaecef46b},
    {.a = 0xcd4c61c26e3d, .b = 0x31c7610de3b0},
    {.a = 0xa82607b01c0d, .b = 0x2910989b6880},
    {.a = 0x0e8f64340ba4, .b = 0x4acec1205d75},
    {.a = 0x2aa05ed1856f, .b = 0xeaac88e5dc99},
};

static bool troika_read(Nfc* nfc, NfcDevice* device) {
    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* mf_classic_data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, mf_classic_data);

    do {
        if(!mf_classic_detect_protocol(mf_classic_data->iso14443_3a_data, &mf_classic_data->type))
            break;
        if(mf_classic_data->type != MfClassicType1k) break;

        MfClassicKey key = {0};
        nfc_util_num2bytes(troika_keys[TROIKA_DATA_SECTOR].a, COUNT_OF(key.data), key.data);

        const uint8_t block_num_start =
            mf_classic_get_first_block_num_of_sector(TROIKA_DATA_SECTOR);
        const uint8_t block_num_end = block_num_start + 2;

        uint8_t block_num;
        for(block_num = block_num_start; block_num < block_num_end; ++block_num) {
            MfClassicBlock block;
            MfClassicError error;

            error = mf_classic_poller_read_block(nfc, block_num, &key, MfClassicKeyTypeA, &block);

            if(error != MfClassicErrorNone) {
                FURI_LOG_D(TAG, "Failed to read block %u: %d", block_num, error);
                break;
            }

            mf_classic_set_block_read(mf_classic_data, block_num, &block);
        }

        if(block_num != block_num_end) break;

        mf_classic_set_key_found(
            mf_classic_data,
            TROIKA_DATA_SECTOR,
            MfClassicKeyTypeA,
            troika_keys[TROIKA_DATA_SECTOR].a);
        nfc_device_set_data(device, NfcProtocolMfClassic, mf_classic_data);

        is_read = true;
    } while(false);

    mf_classic_free(mf_classic_data);

    return is_read;
}

static bool troika_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;

    do {
        // Verify card type
        if(data->type != MfClassicType1k) break;

        // Verify key
        const MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, TROIKA_DATA_SECTOR);

        const uint64_t key = nfc_util_bytes2num(sec_tr->key_a.data, COUNT_OF(sec_tr->key_a.data));
        if(key != troika_keys[TROIKA_DATA_SECTOR].a) break;

        // Parse data
        const uint8_t start_block_num =
            mf_classic_get_first_block_num_of_sector(TROIKA_DATA_SECTOR);

        const uint8_t* temp_ptr = &data->block[start_block_num + 1].data[5];
        uint16_t balance = ((temp_ptr[0] << 8) | temp_ptr[1]) / 25;
        temp_ptr = &data->block[start_block_num].data[2];

        uint32_t number = 0;
        for(size_t i = 1; i < 5; i++) {
            number <<= 8;
            number |= temp_ptr[i];
        }
        number >>= 4;
        number |= (temp_ptr[0] & 0xf) << 28;

        furi_string_printf(parsed_data, "\e#Troika\nNum: %lu\nBalance: %u RUR", number, balance);
        parsed = true;
    } while(false);

    return parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin troika_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = NULL,
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
