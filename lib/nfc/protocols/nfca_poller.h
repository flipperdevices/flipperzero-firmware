#pragma once

#include "nfca.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaPoller NfcaPoller;

typedef enum {
    NfcaPollerEventTypeError,
    NfcaPollerEventDetected,
    NfcaPollerEventTypeActivated,
    NfcaPollerEventTypeReady,
} NfcaPollerEventType;

typedef struct {
    NfcaError error;
} NfcaPollerEventData;

typedef struct {
    NfcaPollerEventType type;
    NfcaPollerEventData data;
} NfcaPollerEvent;

typedef void (*NfcaPollerEventCallback)(NfcaPollerEvent event, void* context);

NfcaPoller* nfca_poller_alloc();

void nfca_poller_free(NfcaPoller* instance);

NfcaError
    nfca_poller_start(NfcaPoller* instance, NfcaPollerEventCallback callback, void* context);

NfcaError nfca_poller_get_data(NfcaPoller* instance, NfcaData* data);

NfcaError nfca_poller_stop(NfcaPoller* instance);

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

// Syncronous API
NfcaError nfca_poller_activate_sync(NfcaPoller* instance, NfcaData* nfca_data);

NfcaError nfca_poller_check_presence_sync(NfcaPoller* instance);

#ifdef __cplusplus
}
#endif
