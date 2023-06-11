#include "mf_desfire.h"

#include <furi.h>

#define MF_DESFIRE_PROTOCOL_NAME "Mifare DESfire"

const NfcProtocolBase nfc_protocol_mf_desfire = {
    .alloc = (NfcProtocolAlloc)mf_desfire_alloc,
    .free = (NfcProtocolFree)mf_desfire_free,
    .reset = (NfcProtocolReset)mf_desfire_reset,
    .copy = (NfcProtocolCopy)mf_desfire_copy,
    .verify = (NfcProtocolVerify)mf_desfire_verify,
    .load = (NfcProtocolLoad)mf_desfire_load,
    .save = (NfcProtocolSave)mf_desfire_save,
    .is_equal = (NfcProtocolEqual)mf_desfire_is_equal,
    .get_name = (NfcProtocolGetName)mf_desfire_get_name,
    .get_uid = (NfcProtocolGetUid)mf_desfire_get_uid,
};

MfDesfireData* mf_desfire_alloc() {
    MfDesfireData* data = malloc(sizeof(MfDesfireData));
    data->iso14443_4a_data = iso14443_4a_alloc();
    return data;
}

void mf_desfire_free(MfDesfireData* data) {
    furi_assert(data);
    mf_desfire_reset(data);
    iso14443_4a_free(data->iso14443_4a_data);
    free(data);
}

void mf_desfire_reset(MfDesfireData* data) {
    furi_assert(data);
    iso14443_4a_reset(data->iso14443_4a_data);
    // TODO: implementation
}

void mf_desfire_copy(MfDesfireData* data, const MfDesfireData* other) {
    furi_assert(data);
    furi_assert(other);

    mf_desfire_reset(data);

    iso14443_4a_copy(data->iso14443_4a_data, other->iso14443_4a_data);

    data->version = other->version;
    data->free_memory = other->free_memory;

    // TODO: Implementation
}

bool mf_desfire_verify(MfDesfireData* data, const FuriString* device_type) {
    UNUSED(data);
    return furi_string_equal_str(device_type, "Mifare Desfire");
}

bool mf_desfire_load(MfDesfireData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: Implementation
    return false;
}

bool mf_desfire_save(const MfDesfireData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: Implementation
    return false;
}

bool mf_desfire_is_equal(const MfDesfireData* data, const MfDesfireData* other) {
    furi_assert(data);
    furi_assert(other);

    // TODO: Complete equality method
    return iso14443_4a_is_equal(data->iso14443_4a_data, other->iso14443_4a_data);
}

const char* mf_desfire_get_name(const MfDesfireData* data, NfcProtocolNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);
    return MF_DESFIRE_PROTOCOL_NAME;
}

const uint8_t* mf_desfire_get_uid(const MfDesfireData* data, size_t* uid_len) {
    furi_assert(data);

    return iso14443_4a_get_uid(data->iso14443_4a_data, uid_len);
}

bool mf_desfire_detect_protocol(NfcaData* nfca_data) {
    furi_assert(nfca_data);

    bool mfu_detected = false;
    return mfu_detected;
}
