#pragma once

#include "iso14443_3b.h"
#include <lib/nfc/nfc.h>

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_3bPoller Iso14443_3bPoller;

typedef enum {
    Iso14443_3bPollerEventTypeError,
    Iso14443_3bPollerEventTypeReady,
} Iso14443_3bPollerEventType;

typedef struct {
    Iso14443_3bError error;
} Iso14443_3bPollerEventData;

typedef struct {
    Iso14443_3bPollerEventType type;
    Iso14443_3bPollerEventData* data;
} Iso14443_3bPollerEvent;

Iso14443_3bError iso14443_3b_poller_send_frame(
    Iso14443_3bPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer);

Iso14443_3bError iso14443_3b_poller_activate(Iso14443_3bPoller* instance, Iso14443_3bData* data);

Iso14443_3bError iso14443_3b_poller_halt(Iso14443_3bPoller* instance);

#ifdef __cplusplus
}
#endif
