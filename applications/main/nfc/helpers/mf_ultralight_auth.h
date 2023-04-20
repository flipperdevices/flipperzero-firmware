#pragma once

#include <lib/nfc/protocols/mf_ultralight.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfUltralightAuthTypeNone,
    MfUltralightAuthTypeReader,
    MfUltralightAuthTypeManual,
    MfUltralightAuthTypeXiaomii,
    MfUltralightAuthTypeAmiibo,
} MfUltralightAuthType;

typedef struct {
    MfUltralightAuthType type;
    MfUltralightAuthPassword password;
    MfUltralightAuthPack pack;
} MfUltralightAuth;

MfUltralightAuth* mf_ultralight_auth_alloc();

void mf_ultralight_auth_free(MfUltralightAuth* instance);

void mf_ultralight_auth_reset(MfUltralightAuth* instance);

#ifdef __cplusplus
}
#endif
