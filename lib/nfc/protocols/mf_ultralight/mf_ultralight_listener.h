#pragma once

#include "mf_ultralight.h"
#include <lib/nfc/protocols/nfca/nfca_listener.h>

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
    MfUltralightListenerCommandContinue = NfcaListenerCommandContinue,
    MfUltralightListenerCommandReset = NfcaListenerCommandReset,
} MfUltralightListenerCommand;

typedef MfUltralightListenerCommand (
    *MfUltralightListenerEventCallback)(MfUltralightListenerEvent event, void* context);

MfUltralightListener* mf_ultralight_listener_alloc(NfcaListener* nfca_listener);

void mf_ultralight_listener_free(MfUltralightListener* instance);

MfUltralightError mf_ultralight_listener_start(
    MfUltralightListener* instance,
    MfUltralightData* data,
    MfUltralightListenerEventCallback callback,
    void* context);

MfUltralightError mf_ultralight_listener_stop(MfUltralightListener* instance);

#ifdef __cplusplus
}
#endif
