#include "nfc_supported_card_plugin.h"
#include <flipper_application/flipper_application.h>
#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>
#include <stdint.h>

#define TAG "MiZIP"
#define KEY_LENGTH 6
#define UID_LENGTH 4

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

static MfClassicKeyPair mizip_1k_keys[] = {
    {.a = 0xa0a1a2a3a4a5, .b = 0xb4c132439eef}, // 000
    {.a = 0x000000000000, .b = 0x000000000000}, // 001
    {.a = 0x000000000000, .b = 0x000000000000}, // 002
    {.a = 0x000000000000, .b = 0x000000000000}, // 003
    {.a = 0x000000000000, .b = 0x000000000000}, // 004
};

const uint8_t verify_sector = 0;

//KDF
void mizip_generate_key(uint8_t* uid, uint8_t keyA[4][KEY_LENGTH], uint8_t keyB[4][KEY_LENGTH]) {
    // Static XOR table for key generation
    static const uint8_t xor_table_keyA[4][6] = {
        {0x09, 0x12, 0x5A, 0x25, 0x89, 0xE5},
        {0xAB, 0x75, 0xC9, 0x37, 0x92, 0x2F},
        {0xE2, 0x72, 0x41, 0xAF, 0x2C, 0x09},
        {0x31, 0x7A, 0xB7, 0x2F, 0x44, 0x90}};

    static const uint8_t xor_table_keyB[4][6] = {
        {0xF1, 0x2C, 0x84, 0x53, 0xD8, 0x21},
        {0x73, 0xE7, 0x99, 0xFE, 0x32, 0x41},
        {0xAA, 0x4D, 0x13, 0x76, 0x56, 0xAE},
        {0xB0, 0x13, 0x27, 0x27, 0x2D, 0xFD}};

    // Permutation table for rearranging elements in uid
    static const uint8_t xorOrderA[6] = {0, 1, 2, 3, 0, 1};
    static const uint8_t xorOrderB[6] = {2, 3, 0, 1, 2, 3};

    // Generate key based on uid and XOR table
    for(uint8_t j = 1; j < 5; j++) {
        for(uint8_t i = 0; i < 6; i++) {
            keyA[j][i] = uid[xorOrderA[i]] ^ xor_table_keyA[j - 1][i];
            keyB[j][i] = uid[xorOrderB[i]] ^ xor_table_keyB[j - 1][i];
        }
    }
}

//Fix get uid
static bool mizip_verify(Nfc* nfc) {
    bool verified = false;

    do {
        const uint8_t block_num = mf_classic_get_first_block_num_of_sector(verify_sector);
        FURI_LOG_D(TAG, "Verifying sector %i", verify_sector);

        MfClassicKey key = {0};
        nfc_util_num2bytes(mizip_1k_keys[verify_sector].b, COUNT_OF(key.data), key.data);

        MfClassicAuthContext auth_context;
        MfClassicError error =
            mf_classic_poller_sync_auth(nfc, block_num, &key, MfClassicKeyTypeB, &auth_context);
        if(error != MfClassicErrorNone) {
            FURI_LOG_D(
                TAG, "Failed to read block %u: %d, this is not a MiZIP card", block_num, error);
            break;
        }
        FURI_LOG_D(TAG, "Found a MiZIP Card");
        verified = true;
    } while(false);

    return verified;
}

static bool mizip_read(Nfc* nfc, NfcDevice* device) {
    FURI_LOG_D(TAG, "Entering MiZIP KDF");
    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, data);

    do {
        MfClassicType type = MfClassicTypeMini;
        MfClassicError error = mf_classic_poller_sync_detect_type(nfc, &type);
        if(error != MfClassicErrorNone) break;
        data->type = MfClassicTypeMini;

        uint8_t uid[UID_LENGTH];
        memcpy(uid, data->iso14443_3a_data->uid, UID_LENGTH);

        uint8_t keyA[4][KEY_LENGTH];
        uint8_t keyB[4][KEY_LENGTH];
        mizip_generate_key(uid, keyA, keyB);

        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            if(mizip_1k_keys[i].a == 0x000000000000 && mizip_1k_keys[i].b == 0x000000000000) {
                mizip_1k_keys[i].a = nfc_util_bytes2num(keyA[i], KEY_LENGTH);
                mizip_1k_keys[i].b = nfc_util_bytes2num(keyB[i], KEY_LENGTH);
            }
        }

        MfClassicDeviceKeys keys = {};
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            nfc_util_num2bytes(mizip_1k_keys[i].a, sizeof(MfClassicKey), keys.key_a[i].data);
            FURI_BIT_SET(keys.key_a_mask, i);
            nfc_util_num2bytes(mizip_1k_keys[i].b, sizeof(MfClassicKey), keys.key_b[i].data);
            FURI_BIT_SET(keys.key_b_mask, i);
        }

        error = mf_classic_poller_sync_read(nfc, &keys, data);
        if(error != MfClassicErrorNone) {
            FURI_LOG_W(TAG, "Failed to read data");
            break;
        }

        nfc_device_set_data(device, NfcProtocolMfClassic, data);

        is_read = true;
    } while(false);

    mf_classic_free(data);

    return is_read;
}

static bool mizip_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;

    do {
        // Verify key
        MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, verify_sector);
        uint64_t key = nfc_util_bytes2num(sec_tr->key_b.data, 6);
        if(key != mizip_1k_keys[verify_sector].b) return false;

        //Get UID
        uint8_t uid[UID_LENGTH];
        memcpy(uid, data->iso14443_3a_data->uid, UID_LENGTH);

        //Get credit
        uint8_t credit_pointer = 0x08;
        uint8_t previus_credit_pointer = 0x09;
        if(data->block[10].data[0] == 0x55) {
            credit_pointer = 0x09;
            previus_credit_pointer = 0x08;
        }
        uint16_t balance = (data->block[credit_pointer].data[2] << 8) |
                           (data->block[credit_pointer].data[1]);
        uint16_t previus_balance = (data->block[previus_credit_pointer].data[2] << 8) |
                                   (data->block[previus_credit_pointer].data[1]);

        //parse data
        furi_string_cat_printf(parsed_data, "\e#MiZIP Card\n");
        furi_string_cat_printf(parsed_data, "UID:");
        for(size_t i = 0; i < UID_LENGTH; i++) {
            furi_string_cat_printf(parsed_data, " %02X", uid[i]);
        }
        furi_string_cat_printf(
            parsed_data, "\nCurrent Credit: %d.%02d E \n", balance / 100, balance % 100);
        furi_string_cat_printf(
            parsed_data,
            "Previus Credit: %d.%02d E \n",
            previus_balance / 100,
            previus_balance % 100);

        parsed = true;
    } while(false);

    return parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin mizip_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = mizip_verify,
    .read = mizip_read,
    .parse = mizip_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor mizip_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &mizip_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* mizip_plugin_ep() {
    return &mizip_plugin_descriptor;
}