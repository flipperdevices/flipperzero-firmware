#pragma once

#include "nfca.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaListener NfcaListener;

typedef enum {
    NfcaListenerEventTypeAbort,
    NfcaListenerEventTypeFieldOn,
    NfcaListenerEventTypeFieldOff,
    NfcaListenerEventTypeHalted,
    NfcaListenerEventTypeReceivedStandartFrame,
    NfcaListenerEventTypeReceivedData,
} NfcaListenerEventType;

typedef struct {
    uint8_t* rx_data;
    uint16_t rx_bits;
} NfcaListenerEventData;

typedef struct {
    NfcaListenerEventType type;
    NfcaListenerEventData data;
} NfcaListenerEvent;

typedef void (*NfcaListenerCallback)(NfcaListenerEvent event, void* context);

NfcaListener* nfca_listener_alloc(NfcaData* data);

void nfca_listener_free(NfcaListener* instance);

NfcaError nfca_listener_set_callback(
    NfcaListener* instance,
    NfcaListenerCallback callback,
    void* context);

NfcaError nfca_listener_sleep(NfcaListener* instance);

NfcaError nfca_listener_rx(
    NfcaListener* instance,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t timeout_ms);

NfcaError nfca_listener_tx(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits);

NfcaError
    nfca_listener_send_standart_frame(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits);

#ifdef __cplusplus
}
#endif
