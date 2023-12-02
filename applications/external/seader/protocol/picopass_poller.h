#pragma once

#include <nfc/nfc.h>
#include "picopass_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PicopassPollerEventTypeRequestMode,
    PicopassPollerEventTypeCardDetected,
    PicopassPollerEventTypeCardLost,
    PicopassPollerEventTypeSuccess,
    PicopassPollerEventTypeFail,
} PicopassPollerEventType;

typedef enum {
    PicopassPollerModeRead,
} PicopassPollerMode;

typedef struct {
    PicopassPollerMode mode;
} PicopassPollerEventDataRequestMode;

typedef union {
    PicopassPollerEventDataRequestMode req_mode;
} PicopassPollerEventData;

typedef struct {
    PicopassPollerEventType type;
    PicopassPollerEventData* data;
} PicopassPollerEvent;

typedef NfcCommand (*PicopassPollerCallback)(PicopassPollerEvent event, void* context);

typedef struct PicopassPoller PicopassPoller;

PicopassPoller* picopass_poller_alloc(Nfc* nfc);

void picopass_poller_free(PicopassPoller* instance);

void picopass_poller_start(
    PicopassPoller* instance,
    PicopassPollerCallback callback,
    void* context);

void picopass_poller_stop(PicopassPoller* instance);
uint8_t* picopass_poller_get_csn(PicopassPoller* instance);

PicopassError picopass_poller_send_frame(
    PicopassPoller* instance,
    BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt_fc);

#ifdef __cplusplus
}
#endif
