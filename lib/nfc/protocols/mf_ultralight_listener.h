#pragma once

#include "mf_ultralight.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfUltralightListener MfUltralightListener;

MfUltralightListener* mf_ultralight_listener_alloc(MfUltralightData* data);

void mf_ultralight_listener_free(MfUltralightListener* instance);

#ifdef __cplusplus
}
#endif
