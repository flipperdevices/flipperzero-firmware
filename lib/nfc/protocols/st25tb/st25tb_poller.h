#pragma once

#include "st25tb.h"
#include <lib/nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct St25tbPoller St25tbPoller;

typedef enum {
    St25tbPollerEventTypeError,
    St25tbPollerEventTypeReady,
    St25tbPollerEventTypeReadSuccessful,
} St25tbPollerEventType;

typedef struct {
    St25tbError error;
} St25tbPollerEventData;

typedef struct {
    St25tbPollerEventType type;
    St25tbPollerEventData* data;
} St25tbPollerEvent;

St25tbError st25tb_poller_send_frame(
    St25tbPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

St25tbError st25tb_poller_initiate(St25tbPoller* instance, uint8_t* chip_id_ptr);

St25tbError st25tb_poller_select(St25tbPoller* instance, uint8_t* chip_id_ptr);

St25tbError st25tb_poller_read(St25tbPoller* instance, St25tbData* data);

St25tbError st25tb_poller_get_uid(St25tbPoller* instance, uint8_t* uid);

St25tbError
    st25tb_poller_read_block(St25tbPoller* instance, uint32_t* block, uint8_t block_number);

St25tbError
    st25tb_poller_write_block(St25tbPoller* instance, uint32_t block, uint8_t block_number);

St25tbError st25tb_poller_halt(St25tbPoller* instance);

#ifdef __cplusplus
}
#endif
