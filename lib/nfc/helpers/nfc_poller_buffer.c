#include "nfc_poller_buffer.h"

#include <furi.h>

NfcPollerBuffer* nfc_poller_buffer_alloc(uint16_t tx_bytes_max, uint16_t rx_bytes_max) {
    NfcPollerBuffer* instance = malloc(sizeof(NfcPollerBuffer));
    instance->tx_data = malloc(tx_bytes_max);
    instance->tx_data_size = tx_bytes_max;
    instance->rx_data = malloc(rx_bytes_max);
    instance->rx_data_size = rx_bytes_max;

    return instance;
}

void nfc_poller_buffer_free(NfcPollerBuffer* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_data);
    furi_assert(instance->rx_data);
    
    free(instance->tx_data);
    free(instance->rx_data);
    free(instance);
}

void nfc_poller_buffer_reset(NfcPollerBuffer* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_data);
    furi_assert(instance->rx_data);
    
    memset(instance->tx_data, 0, instance->tx_data_size);
    memset(instance->rx_data, 0, instance->rx_data_size);
    instance->tx_bits = 0;
    instance->rx_bits = 0;
}
