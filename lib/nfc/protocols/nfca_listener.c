#include "nfca_listener.h"

#include <furi.h>
#include <lib/nfc/nfc.h>

#define TAG "NfcaListener"

typedef enum {
    NfcaListenerStateIdle,
    NfcaListenerStateColResInProgress,
    NfcaListenerStateActive,
} NfcaListenerState;

struct NfcaListener {
    Nfc* nfc;
    NfcaData data;
    NfcaListenerState state;
};

static NfcaError nfca_listener_process_nfc_error(NfcError error) {
    NfcaError ret = NfcaErrorNone;

    if(error == NfcErrorNone) {
        ret = NfcaErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = NfcaErrorTimeout;
    } else {
        ret = NfcaErrorFieldOff;
    }

    return ret;
}

NfcaListener* nfca_listener_alloc(NfcaData* data) {
    NfcaListener* instance = malloc(sizeof(NfcaListener));
    instance->data = *data;
    instance->nfc = nfc_alloc();
    nfc_set_fdt_listen_fc(instance->nfc, NFCA_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeNfcaListener);
    nfc_listener_set_col_res_data(
        instance->nfc,
        instance->data.uid,
        instance->data.uid_len,
        instance->data.atqa,
        instance->data.sak);

    return instance;
}

void nfca_listener_free(NfcaListener* instance) {
    furi_assert(instance);
    nfc_free(instance->nfc);
    free(instance);
}

NfcaError nfca_listener_rx(
    NfcaListener* instance,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t timeout_ms) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    furi_assert(rx_data);
    furi_assert(rx_bits);

    NfcaError ret = NfcaErrorNone;
    NfcError error = nfc_listener_rx(instance->nfc, rx_data, rx_data_size, rx_bits, timeout_ms);
    if(error != NfcErrorNone) {
        FURI_LOG_W(TAG, "Rx error: %d", error);
        ret = nfca_listener_process_nfc_error(error);
    }
    return ret;
}

NfcaError nfca_listener_tx(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits) {
    furi_assert(instance);
    furi_assert(tx_data);

    NfcaError ret = NfcaErrorNone;
    NfcError error = nfc_listener_tx(instance->nfc, tx_data, tx_bits);
    if(error != NfcErrorNone) {
        FURI_LOG_W(TAG, "Tx error: %d", error);
        ret = nfca_listener_process_nfc_error(error);
    }
    return ret;
}
