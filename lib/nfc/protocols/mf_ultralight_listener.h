#pragma once

#include "mf_ultralight.h"
#include "nfca_listener.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfUltralightListener MfUltralightListener;

typedef enum {
    MfUltralightListenerEventTypeAuth,
} MfUltralightListenerEventType;

typedef struct {
    union {
        MfUltralightAuthPassword password;
    };
} MfUltralightListenerEventData;

typedef struct {
    MfUltralightListenerEventType type;
    MfUltralightListenerEventData data;
} MfUltralightListenerEvent;

typedef void (*MfUltralightListenerEventcallback)(MfUltralightListenerEvent event, void* context);

MfUltralightListener* mf_ultralight_listener_alloc(NfcaListener* nfca_listener);

void mf_ultralight_listener_free(MfUltralightListener* instance);

MfUltralightError mf_ultralight_listener_start(
    MfUltralightListener* instance,
    MfUltralightData* data,
    MfUltralightListenerEventcallback callback,
    void* context);

MfUltralightError mf_ultralight_listener_reset(MfUltralightListener* instance);

#ifdef __cplusplus
}
#endif
