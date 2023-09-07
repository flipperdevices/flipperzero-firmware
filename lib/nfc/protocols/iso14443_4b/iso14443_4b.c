#include "iso14443_4b_i.h"
#include "iso14443_4b_device_defs.h"

#include <furi.h>

#define ISO14443_4B_PROTOCOL_NAME "ISO14443-4B"
#define ISO14443_4B_DEVICE_NAME "ISO14443-4B (Unknown)"
#define ISO14443_4B_ATS_BIT (1U << 5)

#define ISO14443_4B_ATS_KEY "ATS"

const NfcDeviceBase nfc_device_iso14443_4b = {
    .protocol_name = ISO14443_4B_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)iso14443_4b_alloc,
    .free = (NfcDeviceFree)iso14443_4b_free,
    .reset = (NfcDeviceReset)iso14443_4b_reset,
    .copy = (NfcDeviceCopy)iso14443_4b_copy,
    .verify = (NfcDeviceVerify)iso14443_4b_verify,
    .load = (NfcDeviceLoad)iso14443_4b_load,
    .save = (NfcDeviceSave)iso14443_4b_save,
    .is_equal = (NfcDeviceEqual)iso14443_4b_is_equal,
    .get_name = (NfcDeviceGetName)iso14443_4b_get_device_name,
    .get_uid = (NfcDeviceGetUid)iso14443_4b_get_uid,
    .set_uid = (NfcDeviceSetUid)iso14443_4b_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)iso14443_4b_get_base_data,
};

Iso14443_4bData* iso14443_4b_alloc() {
    Iso14443_4bData* data = malloc(sizeof(Iso14443_4bData));

    data->iso14443_3b_data = iso14443_3b_alloc();
    data->ats_data = simple_array_alloc(&simple_array_config_uint8_t);

    return data;
}

void iso14443_4b_free(Iso14443_4bData* data) {
    furi_assert(data);

    simple_array_free(data->ats_data);
    iso14443_3b_free(data->iso14443_3b_data);

    free(data);
}

void iso14443_4b_reset(Iso14443_4bData* data) {
    furi_assert(data);

    iso14443_3b_reset(data->iso14443_3b_data);
    simple_array_reset(data->ats_data);
}

void iso14443_4b_copy(Iso14443_4bData* data, const Iso14443_4bData* other) {
    furi_assert(data);
    furi_assert(other);

    iso14443_3b_copy(data->iso14443_3b_data, other->iso14443_3b_data);
    simple_array_copy(data->ats_data, other->ats_data);
}

bool iso14443_4b_verify(Iso14443_4bData* data, const FuriString* device_type) {
    UNUSED(data);
    UNUSED(device_type);

    // Empty, unified file format only
    return false;
}

bool iso14443_4b_load(Iso14443_4bData* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);

    bool parsed = false;

    do {
        if(!iso14443_3b_load(data->iso14443_3b_data, ff, version)) break;
        if(flipper_format_key_exist(ff, ISO14443_4B_ATS_KEY)) {
            uint32_t ats_size = 0;
            if(!flipper_format_get_value_count(ff, ISO14443_4B_ATS_KEY, &ats_size)) break;

            if(ats_size < sizeof(Iso14443_4bAtsData)) break;
            simple_array_init(data->ats_data, ats_size);

            if(!flipper_format_read_hex(
                   ff, ISO14443_4B_ATS_KEY, simple_array_get_data(data->ats_data), ats_size))
                break;
        } else {
            iso14443_4b_ats_fill_default(data->ats_data);
        }
        parsed = true;
    } while(false);

    return parsed;
}

bool iso14443_4b_save(const Iso14443_4bData* data, FlipperFormat* ff) {
    furi_assert(data);

    bool saved = false;

    do {
        if(!iso14443_3b_save(data->iso14443_3b_data, ff)) break;
        if(!flipper_format_write_comment_cstr(ff, ISO14443_4B_PROTOCOL_NAME " specific data"))
            break;
        if(!flipper_format_write_hex(
               ff,
               ISO14443_4B_ATS_KEY,
               simple_array_cget_data(data->ats_data),
               simple_array_get_count(data->ats_data)))
            break;
        saved = true;
    } while(false);

    return saved;
}

bool iso14443_4b_is_equal(const Iso14443_4bData* data, const Iso14443_4bData* other) {
    return iso14443_3b_is_equal(data->iso14443_3b_data, other->iso14443_3b_data);
}

const char* iso14443_4b_get_device_name(const Iso14443_4bData* data, NfcDeviceNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);
    return ISO14443_4B_DEVICE_NAME;
}

const uint8_t* iso14443_4b_get_uid(const Iso14443_4bData* data, size_t* uid_len) {
    return iso14443_3b_get_uid(data->iso14443_3b_data, uid_len);
}

bool iso14443_4b_set_uid(Iso14443_4bData* data, const uint8_t* uid, size_t uid_len) {
    furi_assert(data);

    return iso14443_3b_set_uid(data->iso14443_3b_data, uid, uid_len);
}

Iso14443_3bData* iso14443_4b_get_base_data(const Iso14443_4bData* data) {
    furi_assert(data);

    return data->iso14443_3b_data;
}
