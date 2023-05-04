#pragma once

#include "nfca.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaListener NfcaListener;

typedef enum {
    NfcaListenerEventConfigRequest,
    NfcaListenerEventTypeAbort,
    NfcaListenerEventTypeFieldOn,
    NfcaListenerEventTypeFieldOff,
    NfcaListenerEventTypeHalted,
    NfcaListenerEventTypeReceivedStandartFrame,
    NfcaListenerEventTypeReceivedData,
    NfcaListenerEventTypeReset,
} NfcaListenerEventType;

typedef struct {
    uint8_t* rx_data;
    uint16_t rx_bits;
} NfcaListenerEventData;

typedef struct {
    NfcaListenerEventType type;
    NfcaListenerEventData data;
} NfcaListenerEvent;

typedef enum {
    NfcaListenerCommandContinue = NfcCommandContinue,
    NfcaListenerCommandReset = NfcCommandReset,
} NfcaListenerCommand;

typedef NfcaListenerCommand (*NfcaListenerEventCallback)(NfcaListenerEvent event, void* context);

NfcaListener* nfca_listener_alloc(Nfc* nfc);

void nfca_listener_free(NfcaListener* instance);

NfcaError nfca_listener_start(
    NfcaListener* instance,
    NfcaData* data,
    NfcaListenerEventCallback callback,
    void* context);

NfcaError nfca_listener_get_data(NfcaListener* instance, NfcaData* data);

NfcaError nfca_listener_stop(NfcaListener* instance);

// Called from NfcWorker thread

NfcaError nfca_listener_sleep(NfcaListener* instance);

NfcaError nfca_listener_tx(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits);

NfcaError
    nfca_listener_send_standart_frame(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits);

#ifdef __cplusplus
}
#endif
