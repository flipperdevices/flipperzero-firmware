#include "mf_classic_poller_i.h"

#include <furi.h>

#define MF_CLASSIC_MAX_BUFF_SIZE (64)

MfClassicPoller* mf_classic_poller_alloc(NfcaPoller* nfca_poller) {
    furi_assert(nfca_poller);

    MfClassicPoller* instance = malloc(sizeof(MfClassicPoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_classic_poller_free(MfClassicPoller* instance) {
    furi_assert(instance);

    free(instance);
}

MfClassicError mf_classic_poller_start(
    MfClassicPoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(instance->nfca_poller);
    furi_assert(instance->session_state == MfClassicPollerSessionStateIdle);

    instance->data = malloc(sizeof(MfClassicData));
    instance->buffer = nfc_poller_buffer_alloc(MF_CLASSIC_MAX_BUFF_SIZE, MF_CLASSIC_MAX_BUFF_SIZE);

    instance->session_state = MfClassicPollerSessionStateActive;
    nfca_poller_start(instance->nfca_poller, callback, context);

    return MfClassicErrorNone;
}

MfClassicError mf_classic_poller_read(
    MfClassicPoller* instance,
    MfClassicPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return MfClassicErrorNone;
}

MfClassicError mf_classic_poller_get_data(MfClassicPoller* instance, MfClassicData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfClassicErrorNone;
}

MfClassicError mf_classic_poller_reset(MfClassicPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->buffer);
    furi_assert(instance->nfca_poller);

    nfc_poller_buffer_free(instance->buffer);
    instance->callback = NULL;
    instance->context = NULL;

    return MfClassicErrorNone;
}

MfClassicError mf_classic_poller_stop(MfClassicPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfca_poller);

    instance->session_state = MfClassicPollerSessionStateStopRequest;
    nfca_poller_stop(instance->nfca_poller);
    instance->session_state = MfClassicPollerSessionStateIdle;
    free(instance->data);

    return mf_classic_poller_reset(instance);
}
