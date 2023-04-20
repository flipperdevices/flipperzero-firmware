#include "mf_ultralight_auth.h"

#include <furi.h>

MfUltralightAuth* mf_ultralight_auth_alloc() {
    MfUltralightAuth* instance = malloc(sizeof(MfUltralightAuth));

    return instance;
}

void mf_ultralight_auth_free(MfUltralightAuth* instance) {
    furi_assert(instance);

    free(instance);
}

void mf_ultralight_auth_reset(MfUltralightAuth* instance) {
    furi_assert(instance);

    instance->type = MfUltralightAuthTypeNone;
    memset(&instance->password, 0, sizeof(MfUltralightAuthPassword));
    memset(&instance->pack, 0, sizeof(MfUltralightAuthPack));
}