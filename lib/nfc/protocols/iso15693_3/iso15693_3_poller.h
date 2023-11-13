#pragma once

#include "iso15693_3.h"

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso15693_3Poller Iso15693_3Poller;

typedef enum {
    Iso15693_3PollerEventTypeError,
    Iso15693_3PollerEventTypeReady,
} Iso15693_3PollerEventType;

typedef struct {
    Iso15693_3Error error;
} Iso15693_3PollerEventData;

typedef struct {
    Iso15693_3PollerEventType type;
    Iso15693_3PollerEventData* data;
} Iso15693_3PollerEvent;

Iso15693_3Error iso15693_3_poller_send_frame(
    Iso15693_3Poller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

Iso15693_3Error iso15693_3_poller_activate(Iso15693_3Poller* instance, Iso15693_3Data* data);

Iso15693_3Error iso15693_3_poller_inventory(Iso15693_3Poller* instance, uint8_t* uid);

Iso15693_3Error
    iso15693_3_poller_get_system_info(Iso15693_3Poller* instance, Iso15693_3SystemInfo* data);

Iso15693_3Error iso15693_3_poller_read_block(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint8_t block_number,
    uint8_t block_size);

Iso15693_3Error iso15693_3_poller_read_blocks(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint16_t block_count,
    uint8_t block_size);

Iso15693_3Error iso15693_3_poller_get_blocks_security(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint16_t block_count);

#ifdef __cplusplus
}
#endif
