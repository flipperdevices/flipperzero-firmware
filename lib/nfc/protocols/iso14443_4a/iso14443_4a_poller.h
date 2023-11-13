#pragma once

#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

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

Iso14443_4aError iso14443_4a_poller_send_block(
    Iso14443_4aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer);

Iso14443_4aError iso14443_4a_poller_halt(Iso14443_4aPoller* instance);

Iso14443_4aError
    iso14443_4a_poller_read_ats(Iso14443_4aPoller* instance, Iso14443_4aAtsData* data);

#ifdef __cplusplus
}
#endif
