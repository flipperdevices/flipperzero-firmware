#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>

#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync_api.h>

#define TAG "Plaintain"

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

typedef struct {
    const MfClassicKeyPair* keys;
    uint32_t data_sector;
} PlaintainCardConfig;

static const MfClassicKeyPair plaintain_1k_keys[] = {
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xe56ac127dd45, .b = 0x19fc84a3784b},
    {.a = 0x77dabc9825e1, .b = 0x9764fec3154a},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0x26973ea74321, .b = 0xd27058c6e2c7},
    {.a = 0xeb0a8ff88ade, .b = 0x578a9ada41e3},
    {.a = 0xea0fd73cb149, .b = 0x29c35fa068fb},
    {.a = 0xc76bf71a2509, .b = 0x9ba241db3f56},
    {.a = 0xacffffffffff, .b = 0x71f3a315ad26},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff},
};

static const MfClassicKeyPair plaintain_4k_keys[] = {
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xe56ac127dd45, .b = 0x19fc84a3784b}, {.a = 0x77dabc9825e1, .b = 0x9764fec3154a},
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0x26973ea74321, .b = 0xd27058c6e2c7}, {.a = 0xeb0a8ff88ade, .b = 0x578a9ada41e3},
    {.a = 0xea0fd73cb149, .b = 0x29c35fa068fb}, {.a = 0xc76bf71a2509, .b = 0x9ba241db3f56},
    {.a = 0xacffffffffff, .b = 0x71f3a315ad26}, {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, {.a = 0xffffffffffff, .b = 0xffffffffffff},
    {.a = 0x72f96bdd3714, .b = 0x462225cd34cf}, {.a = 0x044ce1872bc3, .b = 0x8c90c70cff4a},
    {.a = 0xbc2d1791dec1, .b = 0xca96a487de0b}, {.a = 0x8791b2ccb5c4, .b = 0xc956c3b80da3},
    {.a = 0x8e26e45e7d65, .b = 0x8e65b3af7d22}, {.a = 0x0f318130ed18, .b = 0x0c420a20e056},
    {.a = 0x045ceca15535, .b = 0x31bec3d9e510}, {.a = 0x9d993c5d4ef4, .b = 0x86120e488abf},
    {.a = 0xc65d4eaa645b, .b = 0xb69d40d1a439}, {.a = 0x3a8a139c20b4, .b = 0x8818a9c5d406},
    {.a = 0xbaff3053b496, .b = 0x4b7cb25354d3}, {.a = 0x7413b599c4ea, .b = 0xb0a2AAF3A1BA},
    {.a = 0x0ce7cd2cc72b, .b = 0xfa1fbb3f0f1f}, {.a = 0x0be5fac8b06a, .b = 0x6f95887a4fd3},
    {.a = 0x0eb23cc8110b, .b = 0x04dc35277635}, {.a = 0xbc4580b7f20b, .b = 0xd0a4131fb290},
    {.a = 0x7a396f0d633d, .b = 0xad2bdc097023}, {.a = 0xa3faa6daff67, .b = 0x7600e889adf9},
    {.a = 0xfd8705e721b0, .b = 0x296fc317a513}, {.a = 0x22052b480d11, .b = 0xe19504c39461},
    {.a = 0xa7141147d430, .b = 0xff16014fefc7}, {.a = 0x8a8d88151a00, .b = 0x038b5f9b5a2a},
    {.a = 0xb27addfb64b0, .b = 0x152fd0c420a7}, {.a = 0x7259fa0197c6, .b = 0x5583698df085},
};

static bool plaintain_get_card_config(PlaintainCardConfig* config, MfClassicType type) {
    bool success = true;

    if(type == MfClassicType1k) {
        config->data_sector = 8;
        config->keys = plaintain_1k_keys;
    } else if(type == MfClassicType4k) {
        config->data_sector = 8;
        config->keys = plaintain_4k_keys;
    } else {
        success = false;
    }

    return success;
}

static bool plaintain_verify_type(Nfc* nfc, MfClassicType type) {
    bool verified = false;

    do {
        PlaintainCardConfig cfg;
        if(!plaintain_get_card_config(&cfg, type)) break;

        const uint8_t block_num = mf_classic_get_first_block_num_of_sector(cfg.data_sector);
        FURI_LOG_D(TAG, "Verifying sector %lu", cfg.data_sector);

        MfClassicKey key = {0};
        nfc_util_num2bytes(cfg.keys[cfg.data_sector].a, COUNT_OF(key.data), key.data);

        MfClassicAuthContext auth_context;
        MfClassicError error =
            mf_classic_poller_auth(nfc, block_num, &key, MfClassicKeyTypeA, &auth_context);
        if(error != MfClassicErrorNone) {
            FURI_LOG_D(TAG, "Failed to read block %u: %d", block_num, error);
            break;
        }

        verified = true;
    } while(false);

    return verified;
}

static bool plaintain_verify(Nfc* nfc) {
    return plaintain_verify_type(nfc, MfClassicType1k) ||
           plaintain_verify_type(nfc, MfClassicType4k);
}

static bool plaintain_read(Nfc* nfc, NfcDevice* device) {
    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, data);

    do {
        if(!mf_classic_detect_protocol(data->iso14443_3a_data, &data->type)) break;

        PlaintainCardConfig cfg;
        if(!plaintain_get_card_config(&cfg, data->type)) break;

        MfClassicKey key = {0};
        nfc_util_num2bytes(cfg.keys[cfg.data_sector].a, COUNT_OF(key.data), key.data);

        const uint8_t block_num_start = mf_classic_get_first_block_num_of_sector(cfg.data_sector);
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

            mf_classic_set_block_read(data, block_num, &block);
        }

        if(block_num != block_num_end) break;

        mf_classic_set_key_found(
            data, cfg.data_sector, MfClassicKeyTypeA, cfg.keys[cfg.data_sector].a);
        nfc_device_set_data(device, NfcProtocolMfClassic, data);

        is_read = true;
    } while(false);

    mf_classic_free(data);

    return is_read;
}

static bool plaintain_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;

    do {
        // Verify card type
        PlaintainCardConfig cfg;
        if(!plaintain_get_card_config(&cfg, data->type)) break;

        // Verify key
        const MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, cfg.data_sector);

        const uint64_t key = nfc_util_bytes2num(sec_tr->key_a.data, COUNT_OF(sec_tr->key_a.data));
        if(key != cfg.keys[cfg.data_sector].a) break;

        // Parse data
        const uint8_t start_block_num = mf_classic_get_first_block_num_of_sector(cfg.data_sector);

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

        furi_string_printf(
            parsed_data, "\e#Plaintain\nNum: %lu\nBalance: %u RUR", number, balance);
        parsed = true;
    } while(false);

    return parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin plaintain_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = plaintain_verify,
    .read = plaintain_read,
    .parse = plaintain_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor plaintain_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &plaintain_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* plaintain_plugin_ep() {
    return &plaintain_plugin_descriptor;
}
