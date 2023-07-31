#include "iso15693_3.h"

#include <nfc/nfc_common.h>

#define ISO15693_3_PROTOCOL_NAME "ISO15693-3"
#define ISO15693_3_PROTOCOL_NAME_LEGACY "ISO15693"
#define ISO15693_3_DEVICE_NAME "ISO15693-3 (Unknown)"

#define ISO15693_3_DSFID_KEY "DSFID"
#define ISO15693_3_AFI_KEY "AFI"
#define ISO15693_3_IC_REF_KEY "IC Reference"
#define ISO15693_3_BLOCK_COUNT_KEY "Block Count"
#define ISO15693_3_BLOCK_SIZE_KEY "Block Size"
#define ISO15693_3_DATA_CONTENT_KEY "Data Content"
#define ISO15693_3_SECURITY_STATUS_KEY "Security Status"

const NfcDeviceBase nfc_device_iso15693_3 = {
    .protocol_name = ISO15693_3_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)iso15693_3_alloc,
    .free = (NfcDeviceFree)iso15693_3_free,
    .reset = (NfcDeviceReset)iso15693_3_reset,
    .copy = (NfcDeviceCopy)iso15693_3_copy,
    .verify = (NfcDeviceVerify)iso15693_3_verify,
    .load = (NfcDeviceLoad)iso15693_3_load,
    .save = (NfcDeviceSave)iso15693_3_save,
    .is_equal = (NfcDeviceEqual)iso15693_3_is_equal,
    .get_name = (NfcDeviceGetName)iso15693_3_get_device_name,
    .get_uid = (NfcDeviceGetUid)iso15693_3_get_uid,
    .set_uid = (NfcDeviceSetUid)iso15693_3_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)iso15693_3_get_base_data,
};

Iso15693_3Data* iso15693_3_alloc() {
    Iso15693_3Data* data = malloc(sizeof(Iso15693_3Data));

    data->block_data = simple_array_alloc(&simple_array_config_uint8_t);
    data->security_status = simple_array_alloc(&simple_array_config_uint8_t);

    return data;
}

void iso15693_3_free(Iso15693_3Data* data) {
    furi_assert(data);

    simple_array_free(data->block_data);
    simple_array_free(data->security_status);
    free(data);
}

void iso15693_3_reset(Iso15693_3Data* data) {
    furi_assert(data);

    memset(data->uid, 0, ISO15693_3_UID_SIZE);
    memset(&data->system_info, 0, sizeof(Iso15693_3SystemInfo));

    simple_array_reset(data->block_data);
    simple_array_reset(data->security_status);
}

void iso15693_3_copy(Iso15693_3Data* data, const Iso15693_3Data* other) {
    furi_assert(data);
    furi_assert(other);

    memcpy(data->uid, other->uid, ISO15693_3_UID_SIZE);
    data->system_info = other->system_info;

    simple_array_copy(data->block_data, other->block_data);
    simple_array_copy(data->security_status, other->security_status);
}

bool iso15693_3_verify(Iso15693_3Data* data, const FuriString* device_type) {
    UNUSED(data);
    return furi_string_equal(device_type, ISO15693_3_PROTOCOL_NAME_LEGACY);
}

bool iso15693_3_load(Iso15693_3Data* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);
    UNUSED(version);

    bool loaded = false;

    do {
        if(flipper_format_key_exist(ff, ISO15693_3_DSFID_KEY)) {
            if(!flipper_format_read_hex(ff, ISO15693_3_DSFID_KEY, &data->system_info.dsfid, 1))
                break;
            data->system_info.flags |= ISO15693_3_SYSINFO_FLAG_DSFID;
        }

        if(flipper_format_key_exist(ff, ISO15693_3_AFI_KEY)) {
            if(!flipper_format_read_hex(ff, ISO15693_3_AFI_KEY, &data->system_info.afi, 1)) break;
            data->system_info.flags |= ISO15693_3_SYSINFO_FLAG_AFI;
        }

        if(flipper_format_key_exist(ff, ISO15693_3_IC_REF_KEY)) {
            if(!flipper_format_read_hex(ff, ISO15693_3_IC_REF_KEY, &data->system_info.ic_ref, 1))
                break;
            data->system_info.flags |= ISO15693_3_SYSINFO_FLAG_IC_REF;
        }

        if(flipper_format_key_exist(ff, ISO15693_3_BLOCK_COUNT_KEY) &&
           flipper_format_key_exist(ff, ISO15693_3_BLOCK_SIZE_KEY)) {
            uint32_t block_count;
            if(!flipper_format_read_uint32(ff, ISO15693_3_BLOCK_COUNT_KEY, &block_count, 1)) break;

            data->system_info.block_count = block_count;
            data->system_info.flags |= ISO15693_3_SYSINFO_FLAG_MEMORY;

            if(!flipper_format_read_hex(
                   ff, ISO15693_3_BLOCK_SIZE_KEY, &(data->system_info.block_size), 1))
                break;

            simple_array_init(
                data->block_data, data->system_info.block_size * data->system_info.block_count);

            if(!flipper_format_read_hex(
                   ff,
                   ISO15693_3_DATA_CONTENT_KEY,
                   simple_array_get_data(data->block_data),
                   simple_array_get_count(data->block_data)))
                break;

            if(flipper_format_key_exist(ff, ISO15693_3_SECURITY_STATUS_KEY)) {
                simple_array_init(data->security_status, data->system_info.block_count + 1);
                if(!flipper_format_read_hex(
                       ff,
                       ISO15693_3_SECURITY_STATUS_KEY,
                       simple_array_get_data(data->security_status),
                       simple_array_get_count(data->security_status)))
                    break;
            }
        }

        loaded = true;
    } while(false);

    return loaded;
}

bool iso15693_3_save(const Iso15693_3Data* data, FlipperFormat* ff) {
    furi_assert(data);

    bool saved = false;

    do {
        if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
            if(!flipper_format_write_comment_cstr(ff, "Data Storage Format Identifier")) break;
            if(!flipper_format_write_hex(ff, ISO15693_3_DSFID_KEY, &data->system_info.dsfid, 1))
                break;
        }

        if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_AFI) {
            if(!flipper_format_write_comment_cstr(ff, "Application Family Identifier")) break;
            if(!flipper_format_write_hex(ff, ISO15693_3_AFI_KEY, &data->system_info.afi, 1)) break;
        }

        if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
            if(!flipper_format_write_comment_cstr(ff, "IC Reference")) break;
            if(!flipper_format_write_hex(ff, ISO15693_3_IC_REF_KEY, &data->system_info.ic_ref, 1))
                break;
        }

        if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
            const uint32_t block_count = data->system_info.block_count;
            if(!flipper_format_write_comment_cstr(ff, "Number of memory blocks, 1 to 256")) break;
            if(!flipper_format_write_uint32(ff, ISO15693_3_BLOCK_COUNT_KEY, &block_count, 1))
                break;

            if(!flipper_format_write_comment_cstr(ff, "Size of a single memory block, usually 4"))
                break;
            if(!flipper_format_write_hex(
                   ff, ISO15693_3_BLOCK_SIZE_KEY, &data->system_info.block_size, 1))
                break;

            if(!flipper_format_write_hex(
                   ff,
                   ISO15693_3_DATA_CONTENT_KEY,
                   simple_array_cget_data(data->block_data),
                   simple_array_get_count(data->block_data)))
                break;

            if(!flipper_format_write_comment_cstr(
                   ff, "First byte: DSFID (0x01) / AFI (0x02) lock info, others: block lock info"))
                break;
            if(!flipper_format_write_hex(
                   ff,
                   ISO15693_3_SECURITY_STATUS_KEY,
                   simple_array_cget_data(data->security_status),
                   simple_array_get_count(data->security_status)))
                break;
        }
        saved = true;
    } while(false);

    return saved;
}

bool iso15693_3_is_equal(const Iso15693_3Data* data, const Iso15693_3Data* other) {
    furi_assert(data);
    furi_assert(other);

    return memcmp(data, other, sizeof(Iso15693_3Data)) == 0;
}

const char* iso15693_3_get_device_name(const Iso15693_3Data* data, NfcDeviceNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);

    return ISO15693_3_DEVICE_NAME;
}

const uint8_t* iso15693_3_get_uid(const Iso15693_3Data* data, size_t* uid_len) {
    furi_assert(data);

    if(uid_len) *uid_len = ISO15693_3_UID_SIZE;
    return data->uid;
}

bool iso15693_3_set_uid(Iso15693_3Data* data, const uint8_t* uid, size_t uid_len) {
    furi_assert(data);
    furi_assert(uid);

    bool uid_valid = uid_len == ISO15693_3_UID_SIZE;

    if(uid_valid) {
        memcpy(data->uid, uid, uid_len);
    }

    return uid_valid;
}

const Iso15693_3Data* iso15693_3_get_base_data(const Iso15693_3Data* data) {
    UNUSED(data);
    furi_crash("No base data");
}
