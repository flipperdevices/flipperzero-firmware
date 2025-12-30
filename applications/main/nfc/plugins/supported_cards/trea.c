#include "nfc_supported_card_plugin.h"
#include <flipper_application.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>
#include <bit_lib.h>

#define TAG "TREA"
#define KEY_LENGTH 6
#define UID_LENGTH 4

typedef struct {
    uint64_t a;
    uint64_t b;
} MfClassicKeyPair;

static MfClassicKeyPair trea_1k_keys[] = {
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 000
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 001
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 002
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 003
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 004
    {.a = 0xffffffffffff, .b = 0xffffffffffff}, // 005
    {.a = 0x000000000000, .b = 0x000000000000}, // 006 - Blocco da calcolare
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

const uint8_t verify_sector = 6;

static const char* trea_identify_service_type(const MfClassicData* data) {
    
    uint8_t service_indicator = data->block[24].data[7];
    
    switch(service_indicator) {
        case 0x04:
            return "Autolavaggio";
        // Altri tipi da aggiungere con nuovi dump:
        // case 0xXX: return "Distributore";
        // case 0xYY: return "Parcheggio";
        default:
            return "Generic Service";
    }
}

uint8_t trea_calculateByte(
    const uint8_t* uid,
    const uint8_t* codiceGestore,
    size_t base,
    uint8_t offset) {
    uint8_t uidPart = uid[base];
    uint8_t codiceGestorePart = codiceGestore[0];

    return (uidPart - ((0xFF ^ codiceGestorePart) + 0x09) + offset) & 0xFF;
}

void trea_calculateKeys(
    const uint8_t* uid,
    const uint8_t* codiceGestore,
    uint8_t* chiaveA,
    uint8_t* chiaveB) {
    
    // calcolo della chiave A
    chiaveA[0] = codiceGestore[0];
    chiaveA[1] = (codiceGestore[0] + 0x02) & 0xFF;
    chiaveA[2] = trea_calculateByte(uid, codiceGestore, 0, 0);
    chiaveA[3] = trea_calculateByte(uid, codiceGestore, 1, 0x02);
    chiaveA[4] = trea_calculateByte(uid, codiceGestore, 2, 0x04);
    chiaveA[5] = trea_calculateByte(uid, codiceGestore, 3, 0x06);

    // calcolo della chiave B
    chiaveB[0] = chiaveA[2];
    chiaveB[1] = chiaveA[3];
    chiaveB[2] = chiaveA[4];
    chiaveB[3] = chiaveA[5];
    chiaveB[4] = chiaveA[0];
    chiaveB[5] = chiaveA[1];
}

static bool trea_read(Nfc* nfc, NfcDevice* device) {
    FURI_LOG_D(TAG, "Entering TREA KDF");

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

        uint8_t keyF[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        MfClassicKey key_default = {0};
        memcpy(key_default.data, keyF, sizeof(keyF));
        
        bool __attribute__((unused)) default_auth_works = false;
        const uint8_t test_block = mf_classic_get_first_block_num_of_sector(1);
        MfClassicAuthContext auth_context;
        error = mf_classic_poller_sync_auth(nfc, test_block, &key_default, MfClassicKeyTypeA, &auth_context);
        if(error == MfClassicErrorNone) {
            default_auth_works = true;
        }

        bool keys_found = false;
        uint8_t chiaveA[6], chiaveB[6];
        
        for(int vendor = 0x00; vendor <= 0xFF && !keys_found; vendor++) {
            uint8_t codiceGestore[1] = {vendor};
            trea_calculateKeys(uid, codiceGestore, chiaveA, chiaveB);

            MfClassicKey key_calc = {0};
            memcpy(key_calc.data, chiaveA, sizeof(chiaveA));
            
            const uint8_t block_num = mf_classic_get_first_block_num_of_sector(verify_sector);
            error = mf_classic_poller_sync_auth(nfc, block_num, &key_calc, MfClassicKeyTypeA, &auth_context);
            
            if(error == MfClassicErrorNone) {
                FURI_LOG_D(TAG, "TREA keys found with vendor code: %02X", vendor);
                keys_found = true;
                
                trea_1k_keys[verify_sector].a = bit_lib_bytes_to_num_be(chiaveA, KEY_LENGTH);
                trea_1k_keys[verify_sector].b = bit_lib_bytes_to_num_be(chiaveB, KEY_LENGTH);
                break;
            }
        }

        if(!keys_found) break;

        MfClassicDeviceKeys keys = {};
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            bit_lib_num_to_bytes_be(
                trea_1k_keys[i].a, sizeof(MfClassicKey), keys.key_a[i].data);
            FURI_BIT_SET(keys.key_a_mask, i);
            bit_lib_num_to_bytes_be(
                trea_1k_keys[i].b, sizeof(MfClassicKey), keys.key_b[i].data);
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

static bool trea_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);
    bool parsed = false;

    do {
        size_t uid_len;
        const uint8_t* uid = mf_classic_get_uid(data, &uid_len);
        if(uid_len != UID_LENGTH) break;

        // verifica treA
        bool is_trea = false;
        uint8_t vendor_code = 0;
        uint8_t chiaveA[6], chiaveB[6];
        
        for(int vendor = 0x00; vendor <= 0xFF; vendor++) {
            uint8_t codiceGestore[1] = {vendor};
            trea_calculateKeys(uid, codiceGestore, chiaveA, chiaveB);

            MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, verify_sector);
            uint64_t stored_key = bit_lib_bytes_to_num_be(sec_tr->key_a.data, 6);
            uint64_t calculated_key = bit_lib_bytes_to_num_be(chiaveA, KEY_LENGTH);
            
            if(stored_key == calculated_key) {
                is_trea = true;
                vendor_code = vendor;
                break;
            }
        }

        if(!is_trea) break;

        // UID, ATQA, SAK
        furi_string_cat_printf(parsed_data, "\e#TREA/Washtec Card\n");
        furi_string_cat_printf(parsed_data, "(Mifare Classic 1k)\n");
        furi_string_cat_printf(parsed_data, "====================\n");

        // UID
        furi_string_cat_printf(parsed_data, "UID:");
        for(size_t i = 0; i < UID_LENGTH; i++) {
            furi_string_cat_printf(parsed_data, " %02X", uid[i]);
        }
        furi_string_cat_printf(parsed_data, "\n");

        uint8_t atqa_lsb = data->block[0].data[6];
        uint8_t atqa_msb = data->block[0].data[7];
        uint8_t sak = data->block[0].data[5];
        furi_string_cat_printf(parsed_data, "ATQA: %02X %02X ~ SAK: %02X\n", atqa_msb, atqa_lsb, sak);
        
        furi_string_cat_printf(parsed_data, "--------------------\n");

        // credito attuale
        const uint8_t* block_24_data = data->block[24].data;
        uint16_t balance_from_block_24 = (block_24_data[0] << 8) | block_24_data[1];

        furi_string_cat_printf(parsed_data, "-> Credit Available: %d.00\n", balance_from_block_24);
        furi_string_cat_printf(parsed_data, "--------------------\n");

        // vendor code
        furi_string_cat_printf(parsed_data, "Vendor Code: 0x%02X\n", vendor_code);

        // ttipo di servizio
        const char* service_type = trea_identify_service_type(data);
        furi_string_cat_printf(parsed_data, "Service Type: %s\n", service_type);

        furi_string_cat_printf(parsed_data, "--------------------\n");

        // chiavi A e B generate
        furi_string_cat_printf(parsed_data, "Key A: ");
        for(size_t i = 0; i < KEY_LENGTH; i++) {
            furi_string_cat_printf(parsed_data, "%02X", chiaveA[i]);
        }
        furi_string_cat_printf(parsed_data, "\n");

        furi_string_cat_printf(parsed_data, "Key B: ");
        for(size_t i = 0; i < KEY_LENGTH; i++) {
            furi_string_cat_printf(parsed_data, "%02X", chiaveB[i]);
        }
        furi_string_cat_printf(parsed_data, "\n");

        parsed = true;
    } while(false);

    return parsed;
}

static const NfcSupportedCardsPlugin trea_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = NULL,
    .read = trea_read,
    .parse = trea_parse,
};

static const FlipperAppPluginDescriptor trea_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &trea_plugin,
};

const FlipperAppPluginDescriptor* trea_plugin_ep(void) {
    return &trea_plugin_descriptor;
}
