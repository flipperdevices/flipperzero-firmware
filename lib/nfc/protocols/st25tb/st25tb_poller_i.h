#pragma once

#include "st25tb_poller.h"
#include "st25tb_poller_history_data.h"

#include <nfc/nfc_poller.h>
#include <helpers/logger/nfc_logger_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ST25TB_POLLER_MAX_BUFFER_SIZE (16U)

typedef struct {
    uint8_t current_block;
} St25tbPollerReadContext;

typedef struct {
    uint8_t block_number;
    uint32_t block_data;
} St25tbPollerWriteContext;

typedef union {
    St25tbPollerReadContext read;
    St25tbPollerWriteContext write;
} St25tbPollerContext;

struct St25tbPoller {
    Nfc* nfc;
    St25tbPollerState state;
    St25tbData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    St25tbPollerContext poller_ctx;

    NfcGenericEvent general_event;
    St25tbPollerEvent st25tb_event;
    St25tbPollerEventData st25tb_event_data;
    NfcGenericCallback callback;
    NfcGenericLogHistoryCallback log_callback;
    NfcHistoryItem history;
    St25tbPollerHistoryData history_data;
    void* context;
};

const St25tbData* st25tb_poller_get_data(St25tbPoller* instance);

#ifdef __cplusplus
}
#endif
