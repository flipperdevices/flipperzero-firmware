#pragma once

#include "nfca_common.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaPoller NfcaPoller;

NfcaPoller* nfca_poller_alloc();

void nfca_poller_free(NfcaPoller* instance);

NfcaError nfca_poller_check_presence(NfcaPoller* instance);

NfcaError nfca_poller_activate(NfcaPoller* instance, NfcaData* nfca_data);

NfcaError nfca_poller_halt(NfcaPoller* instance);

NfcaError nfca_poller_txrx(
    NfcaPoller* instance,
    uint8_t* tx_buff,
    uint16_t tx_bits,
    uint8_t* rx_buff,
    uint16_t rx_buff_size,
    uint16_t* rx_bits,
    uint32_t fwt);

NfcaError nfca_poller_send_standart_frame(
    NfcaPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt);

#ifdef __cplusplus
}
#endif
