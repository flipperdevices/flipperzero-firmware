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
    PicopassPollerEventTypeRequestKey,
    PicopassPollerEventTypeRequestWriteBlock,
    PicopassPollerEventTypeRequestWriteKey,
    PicopassPollerEventTypeSuccess,
    PicopassPollerEventTypeFail,
    PicopassPollerEventTypeAuthFail,
} PicopassPollerEventType;

typedef enum {
    PicopassPollerModeRead,
    PicopassPollerModeWrite,
    PicopassPollerModeWriteKey,
} PicopassPollerMode;

typedef struct {
    PicopassPollerMode mode;
} PicopassPollerEventDataRequestMode;

typedef struct {
    uint8_t key[PICOPASS_KEY_LEN];
    bool is_key_provided;
    bool is_elite_key;
} PicopassPollerEventDataRequestKey;

typedef struct {
    bool perform_write;
    uint8_t block_num;
    const PicopassBlock* block;
} PicopassPollerEventDataRequestWriteBlock;

typedef struct {
    const PicopassDeviceData* data;
    uint8_t key[PICOPASS_KEY_LEN];
    bool is_elite_key;
} PicopassPollerEventDataRequestWriteKey;

typedef union {
    PicopassPollerEventDataRequestMode req_mode;
    PicopassPollerEventDataRequestKey req_key;
    PicopassPollerEventDataRequestWriteBlock req_write;
    PicopassPollerEventDataRequestWriteKey req_write_key;
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

const PicopassDeviceData* picopass_poller_get_data(PicopassPoller* instance);

#ifdef __cplusplus
}
#endif
