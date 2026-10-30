#include "felica_seac.h"

#include <flipper_format.h>
#include <furi.h>

#include <nfc/nfc_common.h>

#define FELICA_SEAC_PROTOCOL_NAME "FeliCa SEAC"
#define FELICA_SEAC_CONTEXT_KEY   "Context"

static FelicaSeacData* felica_seac_alloc(void) {
    return malloc(sizeof(FelicaSeacData));
}

static void felica_seac_free(FelicaSeacData* data) {
    furi_check(data);

    free(data);
}

static void felica_seac_reset(FelicaSeacData* data) {
    furi_check(data);

    memset(data, 0, sizeof(FelicaSeacData));
}

static void felica_seac_copy(FelicaSeacData* data, const FelicaSeacData* other) {
    furi_check(data);
    furi_check(other);

    *data = *other;
}

static bool felica_seac_verify(FelicaSeacData* data, const FuriString* device_type) {
    UNUSED(data);
    UNUSED(device_type);

    return false;
}

static bool felica_seac_load(FelicaSeacData* data, FlipperFormat* ff, uint32_t version) {
    furi_check(data);
    furi_check(ff);

    if(version < NFC_UNIFIED_FORMAT_VERSION) return false;

    uint32_t context_size;
    if(!flipper_format_get_value_count(ff, FELICA_SEAC_CONTEXT_KEY, &context_size) ||
       context_size != FELICA_SEAC_CONTEXT_SIZE) {
        return false;
    }

    return flipper_format_read_hex(
        ff, FELICA_SEAC_CONTEXT_KEY, data->context, FELICA_SEAC_CONTEXT_SIZE);
}

static bool felica_seac_save(const FelicaSeacData* data, FlipperFormat* ff) {
    furi_check(data);
    furi_check(ff);

    return flipper_format_write_comment_cstr(ff, FELICA_SEAC_PROTOCOL_NAME " specific data") &&
           flipper_format_write_hex(
               ff, FELICA_SEAC_CONTEXT_KEY, data->context, FELICA_SEAC_CONTEXT_SIZE);
}

static bool felica_seac_is_equal(const FelicaSeacData* data, const FelicaSeacData* other) {
    furi_check(data);
    furi_check(other);

    return memcmp(data, other, sizeof(FelicaSeacData)) == 0;
}

static const char*
    felica_seac_get_device_name(const FelicaSeacData* data, NfcDeviceNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);

    return FELICA_SEAC_PROTOCOL_NAME;
}

static const uint8_t* felica_seac_get_uid(const FelicaSeacData* data, size_t* uid_len) {
    furi_check(data);

    if(uid_len) {
        *uid_len = FELICA_SEAC_IDM_SIZE;
    }

    return data->idm;
}

static bool felica_seac_set_uid(FelicaSeacData* data, const uint8_t* uid, size_t uid_len) {
    furi_check(data);
    furi_check(uid);

    const bool uid_valid = uid_len == FELICA_SEAC_IDM_SIZE;
    if(uid_valid) {
        memcpy(data->idm, uid, uid_len);
    }

    return uid_valid;
}

static FelicaSeacData* felica_seac_get_base_data(const FelicaSeacData* data) {
    UNUSED(data);
    furi_crash("No base data");
}

const NfcDeviceBase nfc_device_felica_seac = {
    .protocol_name = FELICA_SEAC_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)felica_seac_alloc,
    .free = (NfcDeviceFree)felica_seac_free,
    .reset = (NfcDeviceReset)felica_seac_reset,
    .copy = (NfcDeviceCopy)felica_seac_copy,
    .verify = (NfcDeviceVerify)felica_seac_verify,
    .load = (NfcDeviceLoad)felica_seac_load,
    .save = (NfcDeviceSave)felica_seac_save,
    .is_equal = (NfcDeviceEqual)felica_seac_is_equal,
    .get_name = (NfcDeviceGetName)felica_seac_get_device_name,
    .get_uid = (NfcDeviceGetUid)felica_seac_get_uid,
    .set_uid = (NfcDeviceSetUid)felica_seac_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)felica_seac_get_base_data,
};
