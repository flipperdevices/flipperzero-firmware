#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>

#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>

#define TAG "Skylanders"

static const uint64_t skylanders_key = 0x4b0b20107ccb;

bool skylanders_verify(Nfc* nfc) {
    bool verified = false;

    do {
        const uint8_t verify_sector = 0;
        uint8_t block_num = mf_classic_get_first_block_num_of_sector(verify_sector);
        FURI_LOG_D(TAG, "Verifying sector %u", verify_sector);

        MfClassicKey key = {};
        nfc_util_num2bytes(skylanders_key, COUNT_OF(key.data), key.data);

        MfClassicAuthContext auth_ctx = {};
        MfClassicError error =
            mf_classic_poller_sync_auth(nfc, block_num, &key, MfClassicKeyTypeA, &auth_ctx);

        if(error != MfClassicErrorNone) {
            FURI_LOG_D(TAG, "Failed to read block %u: %d", block_num, error);
            break;
        }

        verified = true;
    } while(false);

    return verified;
}

static bool skylanders_read(Nfc* nfc, NfcDevice* device) {
    furi_assert(nfc);
    furi_assert(device);

    bool is_read = false;

    MfClassicData* data = mf_classic_alloc();
    nfc_device_copy_data(device, NfcProtocolMfClassic, data);

    do {
        MfClassicType type = MfClassicType1k;
        MfClassicError error = mf_classic_poller_sync_detect_type(nfc, &type);
        if(error != MfClassicErrorNone) break;

        data->type = type;
        MfClassicDeviceKeys keys = {};
        for(size_t i = 0; i < mf_classic_get_total_sectors_num(data->type); i++) {
            nfc_util_num2bytes(skylanders_key, sizeof(MfClassicKey), keys.key_a[i].data);
            FURI_BIT_SET(keys.key_a_mask, i);
            nfc_util_num2bytes(skylanders_key, sizeof(MfClassicKey), keys.key_b[i].data);
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

static uint8_t fill_name(const uint16_t id, FuriString* name) {
    switch(id) {
    case 0x00EB:
        furi_string_cat_printf(name, "Imaginite Mystery Chest");
        break;
    default:
        furi_string_cat_printf(name, "Unknown");
        break;
    }

    return true;
}

static bool skylanders_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);

    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    bool parsed = false;
    FuriString* name = furi_string_alloc();

    do {
        // verify key
        const uint8_t verify_sector = 0;
        MfClassicSectorTrailer* sec_tr =
            mf_classic_get_sector_trailer_by_sector(data, verify_sector);
        uint64_t key = nfc_util_bytes2num(sec_tr->key_a.data, 6);
        if(key != skylanders_key) break;

        const uint16_t id = (uint16_t)*data->block[1].data;
        if(id == 0) break;

        bool success = fill_name(id, name);
        if(!success) break;

        furi_string_printf(parsed_data, "\e#Skylanders\n%s", furi_string_get_cstr(name));

        parsed = true;

    } while(false);

    furi_string_free(name);

    return parsed;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin skylanders_plugin = {
    .protocol = NfcProtocolMfClassic,
    .verify = skylanders_verify,
    .read = skylanders_read,
    .parse = skylanders_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor skylanders_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &skylanders_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* skylanders_plugin_ep() {
    return &skylanders_plugin_descriptor;
}
