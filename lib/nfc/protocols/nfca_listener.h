#pragma once

#include "nfca_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaListener NfcaListener;

NfcaListener* nfca_listener_alloc(NfcaData* data);

void nfca_listener_free(NfcaListener* instance);

NfcaError nfca_listener_rx(
    NfcaListener* instance,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t timeout_ms);

NfcaError nfca_listener_tx(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits);

#ifdef __cplusplus
}
#endif
