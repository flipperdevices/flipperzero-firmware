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

Iso14443_3aError iso14443_3a_poller_txrx(
    Iso14443_3aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

Iso14443_3aError iso14443_3a_poller_send_standard_frame(
    Iso14443_3aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

Iso14443_3aError iso14443_3a_poller_txrx_custom_parity(
    Iso14443_3aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

Iso14443_3aError iso14443_3a_poller_check_presence(Iso14443_3aPoller* instance);

Iso14443_3aError
    iso14443_3a_poller_activate(Iso14443_3aPoller* instance, Iso14443_3aData* iso14443_3a_data);

Iso14443_3aError iso14443_3a_poller_halt(Iso14443_3aPoller* instance);

#ifdef __cplusplus
}
#endif
