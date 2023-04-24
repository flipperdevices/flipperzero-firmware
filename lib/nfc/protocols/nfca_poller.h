#pragma once

#include "nfca.h"
#include "nfc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaPoller NfcaPoller;

typedef enum {
    NfcaPollerEventTypeError,
    NfcaPollerEventTypeReady,
} NfcaPollerEventType;

typedef struct {
    NfcaError error;
} NfcaPollerEventData;

typedef struct {
    NfcaPollerEventType type;
    NfcaPollerEventData data;
} NfcaPollerEvent;

typedef enum {
    NfcaPollerCommandContinue = NfcCommandContinue,
    NfcaPollerCommandReset = NfcCommandReset,
    NfcaPollerCommandStop = NfcCommandStop,
} NfcaPollerCommand;

typedef NfcaPollerCommand (*NfcaPollerEventCallback)(NfcaPollerEvent event, void* context);

NfcaPoller* nfca_poller_alloc(Nfc* nfc);

void nfca_poller_free(NfcaPoller* instance);

NfcaError nfca_poller_start(NfcaPoller* instance, NfcaPollerEventCallback callback, void* context);

NfcaError nfca_poller_stop(NfcaPoller* instance);

NfcaError nfca_poller_get_data(NfcaPoller* instance, NfcaData* data);

// Private API

NfcaError nfca_poller_config(NfcaPoller* instance);

NfcaError nfca_poller_reset(NfcaPoller* instance);

// Syncronous API
NfcaError nfca_poller_activate(NfcaPoller* instance, NfcaData* nfca_data);

// Called from NfcWorker thread

NfcaError nfca_poller_check_presence(NfcaPoller* instance);

NfcaError nfca_poller_async_activate(NfcaPoller* instance, NfcaData* nfca_data);

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
