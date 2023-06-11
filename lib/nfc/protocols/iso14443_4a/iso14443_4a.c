#include "iso14443_4a.h"

#include <furi.h>

#define ISO14443_4A_PROTOCOL_NAME "Unknown ISO14443-4A Tag"

const NfcProtocolBase nfc_protocol_iso14443_4a = {
    .alloc = (NfcProtocolAlloc)iso14443_4a_alloc,
    .free = (NfcProtocolFree)iso14443_4a_free,
    .reset = (NfcProtocolReset)iso14443_4a_reset,
    .copy = (NfcProtocolCopy)iso14443_4a_copy,
    .verify = (NfcProtocolVerify)iso14443_4a_verify,
    .load = (NfcProtocolLoad)iso14443_4a_load,
    .save = (NfcProtocolSave)iso14443_4a_save,
    .is_equal = (NfcProtocolEqual)iso14443_4a_is_equal,
    .get_name = (NfcProtocolGetName)iso14443_4a_get_name,
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
}

bool iso14443_4a_verify(Iso14443_4aData* data, const FuriString* device_type) {
    UNUSED(data);
    UNUSED(device_type);

    // TODO: implementation
    return false;
}

bool iso14443_4a_load(Iso14443_4aData* data, FlipperFormat* ff, uint32_t version) {
    UNUSED(data);
    UNUSED(ff);
    UNUSED(version);

    // TODO: implementation
    return false;
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

const char* iso14443_4a_get_name(const Iso14443_4aData* data) {
    UNUSED(data);
    return ISO14443_4A_PROTOCOL_NAME;
}

const uint8_t* iso14443_4a_get_uid(const Iso14443_4aData* data, size_t* uid_len) {
    return nfca_get_uid(data->iso14443_3a_data, uid_len);
}
