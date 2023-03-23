#include "mf_ultralight_listener.h"
#include "nfca_listener.h"

#include <furi.h>

struct MfUltralightListener {
    NfcaListener* nfca_listener;
};

MfUltralightListener* mf_ultralight_listener_alloc(MfUltralightData* data) {
    furi_assert(data);

    MfUltralightListener* instance = malloc(sizeof(MfUltralightData));
    instance->nfca_listener = nfca_listener_alloc(&data->nfca_data);

    return instance;
}

void mf_ultralight_listener_free(MfUltralightListener* instance) {
    furi_assert(instance);

    nfca_listener_free(instance->nfca_listener);
    free(instance);
}
