#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

MfDesfirePoller* mf_desfire_poller_alloc(NfcaPoller* nfca_poller) {
    MfDesfirePoller* instance = malloc(sizeof(MfDesfirePoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_desfire_poller_free(MfDesfirePoller* instance) {
    furi_assert(instance);
    free(instance);
}

MfDesfireError mf_desfire_poller_read(
    MfDesfirePoller* instance,
    MfDesfirePollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(context);

    return MfDesfireErrorNone;
}

MfDesfireError
    mf_desfire_poller_get_data(MfDesfirePoller* instance, MfDesfireData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfDesfireErrorNone;
}

MfDesfireError mf_desfire_poller_stop(MfDesfirePoller* instance) {
    furi_assert(instance);

    return MfDesfireErrorNone;
}
