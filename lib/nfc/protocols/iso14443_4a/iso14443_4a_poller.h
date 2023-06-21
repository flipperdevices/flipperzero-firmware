#pragma once

#include <lib/nfc/protocols/nfca/nfca_poller.h>

#include "iso14443_4a.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_4aPoller Iso14443_4aPoller;

typedef enum {
    Iso14443_4aPollerEventTypeError,
    Iso14443_4aPollerEventTypeReady,
} Iso14443_4aPollerEventType;

typedef struct {
    Iso14443_4aError error;
} Iso14443_4aPollerEventData;

typedef struct {
    Iso14443_4aPollerEventType type;
    Iso14443_4aPollerEventData* data;
} Iso14443_4aPollerEvent;

const Iso14443_4aData* iso14443_4a_poller_get_data(Iso14443_4aPoller* instance);

#ifdef __cplusplus
}
#endif
