#pragma once

#include "iso14443_3a.h"
#include <lib/nfc/nfc.h>

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_3aPoller Iso14443_3aPoller;

typedef enum {
    Iso14443_3aPollerEventTypeError,
    Iso14443_3aPollerEventTypeReady,
} Iso14443_3aPollerEventType;

typedef struct {
    Iso14443_3aError error;
} Iso14443_3aPollerEventData;

typedef struct {
    Iso14443_3aPollerEventType type;
    Iso14443_3aPollerEventData* data;
} Iso14443_3aPollerEvent;

#ifdef __cplusplus
}
#endif
