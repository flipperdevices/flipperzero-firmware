#include "mf_desfire.h"

#include <furi.h>

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

    // TODO: Implementation
}

bool mf_desfire_detect_protocol(NfcaData* nfca_data) {
    furi_assert(nfca_data);

    bool mfu_detected = false;
    return mfu_detected;
}
