#pragma once

#include "st25tb_poller.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ST25TB_POLLER_MAX_BUFFER_SIZE (16U)

typedef enum {
    St25tbPollerStateIdle,
    St25tbPollerStateSelected,
    St25tbPollerStateRead,

    St25tbPollerStateNum,
} St25tbPollerState;

struct St25tbPoller {
    Nfc* nfc;
    St25tbPollerState state;
    St25tbData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent general_event;
    St25tbPollerEvent st25tb_event;
    St25tbPollerEventData st25tb_event_data;
    NfcGenericCallback callback;
    void* context;
};

const St25tbData* st25tb_poller_get_data(St25tbPoller* instance);

#ifdef __cplusplus
}
#endif
