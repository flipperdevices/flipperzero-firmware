#pragma once

#include "gen1a_poller.h"
#include <nfc/protocols/nfc_generic_event.h>
#include <nfc/nfc_device.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GEN1A_POLLER_MAX_BUFFER_SIZE (64U)
#define GEN1A_POLLER_MAX_FWT (60000U)

typedef enum {
    Gen1aPollerErrorNone,
    Gen1aPollerErrorTimeout,
    Gen1aPollerErrorNotPresent,
    Gen1aPollerErrorProtocol,
} Gen1aPollerError;

typedef enum {
    Gen1aPollerStateIdle,
    Gen1aPollerStateRequestMode,
    Gen1aPollerStateWipe,
    Gen1aPollerStateWriteDataRequest,
    Gen1aPollerStateWrite,
    Gen1aPollerStateSuccess,
    Gen1aPollerStateFail,

    Gen1aPollerStateNum,
} Gen1aPollerState;

typedef enum {
    Gen1aPollerSessionStateIdle,
    Gen1aPollerSessionStateStarted,
    Gen1aPollerSessionStateStopRequest,
} Gen1aPollerSessionState;

struct Gen1aPoller {
    Nfc* nfc;
    Gen1aPollerState state;
    Gen1aPollerSessionState session_state;

    uint16_t current_block;
    NfcDevice* mfc_device;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    Gen1aPollerEvent gen1a_event;
    Gen1aPollerEventData gen1a_event_data;

    Gen1aPollerCallback callback;
    void* context;
};

Gen1aPollerError gen1a_poller_wupa(Gen1aPoller* instance);

Gen1aPollerError gen1a_poller_data_access(Gen1aPoller* instance);

Gen1aPollerError
    gen1a_poller_write_block(Gen1aPoller* instance, uint8_t block_num, const MfClassicBlock* block);

#ifdef __cplusplus
}
#endif
