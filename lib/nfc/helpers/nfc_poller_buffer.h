#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t* tx_data;
    uint16_t tx_data_size;
    uint16_t tx_bits;
    uint8_t* rx_data;
    uint16_t rx_data_size;
    uint16_t rx_bits;
} NfcPollerBuffer;

NfcPollerBuffer* nfc_poller_buffer_alloc(uint16_t tx_bytes_max, uint16_t rx_bytes_max);

void nfc_poller_buffer_free(NfcPollerBuffer* instance);

void nfc_poller_buffer_reset(NfcPollerBuffer* instance);

#ifdef __cplusplus
}
#endif
