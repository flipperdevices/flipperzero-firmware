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

#ifdef __cplusplus
}
#endif
