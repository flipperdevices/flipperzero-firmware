#include "nfc_supported_card_plugin.h"
#include <flipper_application/flipper_application.h>
#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>

#define TAG "Microel"
#define KEY_LENGTH 6
#define UID_LENGTH 4

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

static MfClassicKeyPair microel_1k_keys[] = {
    {.a = 0x000000000000, .b = 0x000000000000}, // 000
    {.a = 0x000000000000, .b = 0x000000000000}, // 001
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 002
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 003
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 004
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 005
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 006
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 007
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 008
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 009
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 010
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 011
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 012
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 013
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 014
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 015
};

const uint8_t verify_sector = 1;

void calcolaSommaHex(const uint8_t* uid, size_t uidSize, uint8_t sommaHex[]) {
    const uint8_t xorKey[] = {0x01, 0x92, 0xA7, 0x75, 0x2B, 0xF9};
    int somma = 0;

    for(size_t i = 0; i < uidSize; i++) {
        somma += uid[i];
    }

    int sommaDueNumeri = somma % 256;

    for(size_t i = 0; i < sizeof(xorKey); i++) {
        sommaHex[i] = sommaDueNumeri ^ xorKey[i];
    }
}

void generateKeyA(const uint8_t* uid, uint8_t uidSize, uint8_t keyA[]) {
    uint8_t sommaHex[6];
    calcolaSommaHex(uid, uidSize, sommaHex);
    uint8_t primoCarattere = (sommaHex[0] >> 4) & 0xF;

    if(primoCarattere == 0x2 || primoCarattere == 0x3 || primoCarattere == 0xA ||
       primoCarattere == 0xB) {
        // XOR WITH 0x40
        for(size_t i = 0; i < sizeof(sommaHex); i++) {
            keyA[i] = 0x40 ^ sommaHex[i];
        }
    } else if(
        primoCarattere == 0x6 || primoCarattere == 0x7 || primoCarattere == 0xE ||
        primoCarattere == 0xF) {
        // XOR WITH 0xC0
        for(size_t i = 0; i < sizeof(sommaHex); i++) {
            keyA[i] = 0xC0 ^ sommaHex[i];
        }
    }
}

void generateKeyB(uint8_t keyA[], size_t keyASize, uint8_t keyB[]) {
    for(size_t i = 0; i < keyASize; i++) {
        keyB[i] = 0xFF ^ keyA[i];
    }
}

/*static bool microel_verify(Nfc* nfc) {
    furi_assert(nfc);

    bool verified = false;

    do {
        uint8_t block_num = mf_classic_get_first_block_num_of_sector(verify_sector);
        FURI_LOG_D(TAG, "Verifying sector %u", verify_sector);

        uint8_t uid[UID_LENGTH] = {0xd4, 0x23, 0xb7, 0x34};

        size_t uid_len;
        const uint8_t* uidn = mf_classic_get_uid(data, &uid_len);
        FURI_LOG_D(TAG, "Microel provadiocane: UID identified: %02X%02X%02X%02X", uidn[0], uidn[1], uidn[2], uidn[3]);
        if(uid_len != UID_LENGTH) break;
        size_t uid_len = 0;
        const uint8_t* originalUid = mf_classic_get_uid(data, &uid_len);
        uint8_t uid[UID_LENGTH];  // Sostituisci UID_LENGTH con la lunghezza effettiva dell'UID
        memcpy(uid, originalUid, UID_LENGTH);
        FURI_LOG_D(TAG, "UID: %02X %02X %02X %02X", uid[0],uid[1],uid[2],uid[3]);
        FURI_LOG_D(TAG, "UID GET: %02X %02X %02X %02X", originalUid[0],originalUid[1],originalUid[2],originalUid[3]);
        //memcpy(uid, data->iso14443_3a_data->uid, UID_LENGTH);

        // Generate key from uid
        uint8_t keyA[KEY_LENGTH];
        generateKeyA(uid, UID_LENGTH, keyA);

        MfClassicKey key = {};
        memcpy(key.data, keyA, KEY_LENGTH);

        MfClassicAuthContext auth_ctx = {};
        MfClassicError error =
            mf_classic_poller_sync_auth(nfc, block_num, &key, MfClassicKeyTypeA, &auth_ctx);
        if(error != MfClassicErrorNone) {
            verified = false;
            FURI_LOG_D(TAG, "Failed to read block %u: %d", block_num, error);
            break;
        }

        verified = true;
    } while(false);

    return verified;
}*/

static bool microel_read(Nfc* nfc, NfcDevice* device) {
    FURI_LOG_D(TAG, "Entering Microel KDF");

    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, data);

    do {
        MfClassicType type = MfClassicType1k;
        MfClassicError error = mf_classic_poller_sync_detect_type(nfc, &type);
        if(error != MfClassicErrorNone) break;

        size_t uid_len;
        const uint8_t* uid = mf_classic_get_uid(data, &uid_len);
        FURI_LOG_D(TAG, "UID identified: %02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);
        if(uid_len != UID_LENGTH) break;

        uint8_t keyA[KEY_LENGTH];
        uint8_t keyB[KEY_LENGTH];
        generateKeyA(uid, UID_LENGTH, keyA);
        generateKeyB(keyA, KEY_LENGTH, keyB);

        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            if(microel_1k_keys[i].a == 0x000000000000) {
                microel_1k_keys[i].a = nfc_util_bytes2num(keyA, KEY_LENGTH);
            }
            if(microel_1k_keys[i].b == 0x000000000000) {
                microel_1k_keys[i].b = nfc_util_bytes2num(keyB, KEY_LENGTH);
            }
        }

        MfClassicDeviceKeys keys = {};
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            nfc_util_num2bytes(microel_1k_keys[i].a, sizeof(MfClassicKey), keys.key_a[i].data);
            FURI_BIT_SET(keys.key_a_mask, i);
            nfc_util_num2bytes(microel_1k_keys[i].b, sizeof(MfClassicKey), keys.key_b[i].data);
            FURI_BIT_SET(keys.key_b_mask, i);
        }

        error = mf_classic_poller_sync_read(nfc, &keys, data);
        if(error == MfClassicErrorNotPresent) {
            FURI_LOG_W(TAG, "Failed to read data");
            break;
        }

        nfc_device_set_data(device, NfcProtocolMfClassic, data);

        is_read = (error == MfClassicErrorNone);
    } while(false);

    mf_classic_free(data);

    return is_read;
}

static bool microel_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;

    do {
        //Get UID
        size_t uid_len;
        const uint8_t* uid = mf_classic_get_uid(data, &uid_len);
        if(uid_len != UID_LENGTH) break;

        // Generate key from uid
        uint8_t keyA[KEY_LENGTH];
        generateKeyA(uid, UID_LENGTH, keyA);

        // Verify key
        MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, verify_sector);
        uint64_t key = nfc_util_bytes2num(sec_tr->key_a.data, 6);
        uint64_t key_for_check_from_array = nfc_util_bytes2num(keyA, KEY_LENGTH);
        if(key != key_for_check_from_array) break;

        //Get credit in block number 8
        const uint8_t* temp_ptr = data->block[4].data;
        uint16_t balance = (temp_ptr[6] << 8) | (temp_ptr[5]);
        uint16_t previus_balance = (data->block[5].data[6] << 8) | (data->block[5].data[5]);
        furi_string_cat_printf(parsed_data, "\e#Microel Card\n");
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
static const NfcSupportedCardsPlugin microel_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify =
        NULL, // the verification I need is based on verifying the keys generated via uid and try to authenticate not like on mizip that there is default b0
    .read = microel_read,
    .parse = microel_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor microel_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &microel_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* microel_plugin_ep() {
    return &microel_plugin_descriptor;
}