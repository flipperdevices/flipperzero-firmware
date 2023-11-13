#pragma once

#include "slix.h"

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SlixPoller SlixPoller;

typedef enum {
    SlixPollerEventTypeError,
    SlixPollerEventTypeReady,
} SlixPollerEventType;

typedef struct {
    SlixError error;
} SlixPollerEventData;

typedef struct {
    SlixPollerEventType type;
    SlixPollerEventData* data;
} SlixPollerEvent;

SlixError slix_poller_send_frame(
    SlixPoller* instance,
    const BitBuffer* tx_data,
    BitBuffer* rx_data,
    uint32_t fwt);

SlixError slix_poller_get_nxp_system_info(SlixPoller* instance, SlixSystemInfo* data);

SlixError slix_poller_read_signature(SlixPoller* instance, SlixSignature* data);

#ifdef __cplusplus
}
#endif
