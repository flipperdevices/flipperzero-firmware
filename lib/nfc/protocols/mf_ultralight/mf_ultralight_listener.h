#pragma once

#include "mf_ultralight.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

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
    MfUltralightListenerEventData* data;
} MfUltralightListenerEvent;

typedef enum {
    MfUltralightListenerCommandContinue = Iso14443_3aListenerCommandContinue,
    MfUltralightListenerCommandReset = Iso14443_3aListenerCommandReset,
} MfUltralightListenerCommand;

typedef MfUltralightListenerCommand (
    *MfUltralightListenerEventCallback)(MfUltralightListenerEvent event, void* context);

MfUltralightListener* mf_ultralight_listener_alloc(Iso14443_3aListener* iso14443_3a_listener);

void mf_ultralight_listener_free(MfUltralightListener* instance);

MfUltralightError mf_ultralight_listener_start(
    MfUltralightListener* instance,
    const MfUltralightData* data,
    MfUltralightListenerEventCallback callback,
    void* context);

MfUltralightError
    mf_ultralight_listener_get_data(MfUltralightListener* instance, MfUltralightData* data);

MfUltralightError mf_ultralight_listener_stop(MfUltralightListener* instance);

#ifdef __cplusplus
}
#endif
