#include "iso14443_4a_i.h"

#include <furi.h>

#define ISO14443_4A_PROTOCOL_NAME "ISO14443-4A"
#define ISO14443_4A_DEVICE_NAME "Unknown ISO14443-4A Tag"
#define ISO14443_4A_ATS_BIT (1 << 5)

const NfcProtocolBase nfc_protocol_iso14443_4a = {
    .protocol_name = ISO14443_4A_PROTOCOL_NAME,
    .alloc = (NfcProtocolAlloc)iso14443_4a_alloc,
    .free = (NfcProtocolFree)iso14443_4a_free,
    .reset = (NfcProtocolReset)iso14443_4a_reset,
    .copy = (NfcProtocolCopy)iso14443_4a_copy,
    .verify = (NfcProtocolVerify)iso14443_4a_verify,
    .load = (NfcProtocolLoad)iso14443_4a_load,
    .save = (NfcProtocolSave)iso14443_4a_save,
    .is_equal = (NfcProtocolEqual)iso14443_4a_is_equal,
    .get_device_name = (NfcProtocolGetDeviceName)iso14443_4a_get_device_name,
    .get_uid = (NfcProtocolGetUid)iso14443_4a_get_uid,
};

Iso14443_4aData* iso14443_4a_alloc() {
    Iso14443_4aData* data = malloc(sizeof(Iso14443_4aData));
    data->iso14443_3a_data = nfca_alloc();

    return data;
}

void iso14443_4a_free(Iso14443_4aData* data) {
    furi_assert(data);

    nfca_free(data->iso14443_3a_data);
    free(data);
}

void iso14443_4a_reset(Iso14443_4aData* data) {
    furi_assert(data);

    nfca_reset(data->iso14443_3a_data);
}

void iso14443_4a_copy(Iso14443_4aData* data, const Iso14443_4aData* other) {
    furi_assert(data);
    furi_assert(other);

    nfca_copy(data->iso14443_3a_data, other->iso14443_3a_data);
    data->ats_data = other->ats_data;
}

bool iso14443_4a_verify(Iso14443_4aData* data, const FuriString* device_type) {
    UNUSED(data);
    UNUSED(device_type);

    // TODO: implementation
    return false;
}

bool iso14443_4a_load(Iso14443_4aData* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);
    // TODO: handle additional fields
    return nfca_load(data->iso14443_3a_data, ff, version);
}

bool iso14443_4a_save(const Iso14443_4aData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: implementation
    return false;
}

bool iso14443_4a_is_equal(const Iso14443_4aData* data, const Iso14443_4aData* other) {
    return nfca_is_equal(data->iso14443_3a_data, other->iso14443_3a_data);
}

const char*
    iso14443_4a_get_device_name(const Iso14443_4aData* data, NfcProtocolNameType name_type) {
    UNUSED(data);
    UNUSED(name_type);
    return ISO14443_4A_DEVICE_NAME;
}

const uint8_t* iso14443_4a_get_uid(const Iso14443_4aData* data, size_t* uid_len) {
    return nfca_get_uid(data->iso14443_3a_data, uid_len);
}

bool iso14443_4a_is_ats_supported(const Iso14443_4aData* data) {
    const NfcaData* iso14443_3a_data = data->iso14443_3a_data;
    return iso14443_3a_data->sak & ISO14443_4A_ATS_BIT;
}
