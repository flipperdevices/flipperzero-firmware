#include "innovatron.h"

#include <furi.h>
#include <nfc/nfc_common.h>

#define INNOVATRON_PROTOCOL_NAME "Innovatron"

#define INNOVATRON_VERLOG_KEY  "VerLog"
#define INNOVATRON_CONFIG_KEY  "Config"
#define INNOVATRON_ATR_LEN_KEY "ATR Length"
#define INNOVATRON_ATR_KEY     "ATR"

const NfcDeviceBase nfc_device_innovatron = {
    .protocol_name = INNOVATRON_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)innovatron_alloc,
    .free = (NfcDeviceFree)innovatron_free,
    .reset = (NfcDeviceReset)innovatron_reset,
    .copy = (NfcDeviceCopy)innovatron_copy,
    .verify = (NfcDeviceVerify)innovatron_verify,
    .load = (NfcDeviceLoad)innovatron_load,
    .save = (NfcDeviceSave)innovatron_save,
    .is_equal = (NfcDeviceEqual)innovatron_is_equal,
    .get_name = (NfcDeviceGetName)innovatron_get_device_name,
    .get_uid = (NfcDeviceGetUid)innovatron_get_uid,
    .set_uid = (NfcDeviceSetUid)innovatron_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)innovatron_get_base_data,
};

InnovatronData* innovatron_alloc(void) {
    return malloc(sizeof(InnovatronData));
}

void innovatron_free(InnovatronData* data) {
    furi_check(data);

    free(data);
}

void innovatron_reset(InnovatronData* data) {
    furi_check(data);

    memset(data, 0, sizeof(InnovatronData));
}

void innovatron_copy(InnovatronData* data, const InnovatronData* other) {
    furi_check(data);
    furi_check(other);

    *data = *other;
}

bool innovatron_verify(InnovatronData* data, const FuriString* device_type) {
    UNUSED(data);
    furi_check(device_type);

    return furi_string_equal_str(device_type, INNOVATRON_PROTOCOL_NAME);
}

bool innovatron_load(InnovatronData* data, FlipperFormat* ff, uint32_t version) {
    furi_check(data);
    furi_check(ff);

    bool loaded = false;
    uint32_t atr_len = 0;

    do {
        if(version < NFC_UNIFIED_FORMAT_VERSION) break;
        data->vt_addr = INNOVATRON_VT_ADDR_DEFAULT;
        if(!flipper_format_read_hex(ff, INNOVATRON_VERLOG_KEY, &data->verlog, 1)) break;
        if(!flipper_format_read_hex(ff, INNOVATRON_CONFIG_KEY, &data->config, 1)) break;
        if(!flipper_format_read_uint32(ff, INNOVATRON_ATR_LEN_KEY, &atr_len, 1)) break;
        if(atr_len > INNOVATRON_ATR_MAX_SIZE) break;

        memset(data->atr, 0, sizeof(data->atr));
        data->atr_len = atr_len;
        if(data->atr_len &&
           !flipper_format_read_hex(ff, INNOVATRON_ATR_KEY, data->atr, data->atr_len))
            break;

        loaded = true;
    } while(false);

    return loaded;
}

bool innovatron_save(const InnovatronData* data, FlipperFormat* ff) {
    furi_check(data);
    furi_check(ff);

    bool saved = false;
    const uint32_t atr_len = data->atr_len;

    do {
        if(!flipper_format_write_comment_cstr(ff, INNOVATRON_PROTOCOL_NAME " specific data"))
            break;
        if(!flipper_format_write_hex(ff, INNOVATRON_VERLOG_KEY, &data->verlog, 1)) break;
        if(!flipper_format_write_hex(ff, INNOVATRON_CONFIG_KEY, &data->config, 1)) break;
        if(!flipper_format_write_uint32(ff, INNOVATRON_ATR_LEN_KEY, &atr_len, 1)) break;
        if(data->atr_len &&
           !flipper_format_write_hex(ff, INNOVATRON_ATR_KEY, data->atr, data->atr_len))
            break;

        saved = true;
    } while(false);

    return saved;
}

bool innovatron_is_equal(const InnovatronData* data, const InnovatronData* other) {
    furi_check(data);
    furi_check(other);

    return memcmp(data, other, sizeof(InnovatronData)) == 0;
}

const char* innovatron_get_device_name(const InnovatronData* data, NfcDeviceNameType name_type) {
    UNUSED(data);

    if(name_type == NfcDeviceNameTypeFull) {
        return "B Prime Innovatron";
    }

    return "bi";
}

const uint8_t* innovatron_get_uid(const InnovatronData* data, size_t* uid_len) {
    furi_check(data);

    if(uid_len) {
        *uid_len = INNOVATRON_DIV_SIZE;
    }

    return data->div;
}

bool innovatron_set_uid(InnovatronData* data, const uint8_t* uid, size_t uid_len) {
    furi_check(data);
    furi_check(uid);

    const bool uid_valid = uid_len == INNOVATRON_DIV_SIZE;

    if(uid_valid) {
        memcpy(data->div, uid, uid_len);
    }

    return uid_valid;
}

InnovatronData* innovatron_get_base_data(const InnovatronData* data) {
    UNUSED(data);
    furi_crash("No base data");
}
