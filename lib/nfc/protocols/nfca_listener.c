#include "nfca_listener.h"

#include <furi.h>
#include <lib/nfc/nfc.h>

#define TAG "NfcaListener"

typedef enum {
    NfcaListenerStateIdle,
    NfcaListenerStateActive,
} NfcaListenerState;

struct NfcaListener {
    Nfc* nfc;
    NfcaData data;
    NfcaListenerState state;
    NfcaListenerCallback callback;
    void* context;
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

static bool nfca_listener_halt_received(uint8_t* rx_data, uint16_t rx_bits) {
    bool halt_cmd_received = false;

    do {
        if(rx_bits != 4 * 8) break;
        uint16_t rx_bytes = rx_bits / 8;
        if(!nfca_check_crc(rx_data, rx_bytes)) break;
        if(!((rx_data[0] == 0x50) && (rx_data[1] == 0x00))) break;
        halt_cmd_received = true;
    } while(false);

    return halt_cmd_received;
}

static void nfca_listener_event_handler(NfcEvent event, void* context) {
    furi_assert(context);

    NfcaListener* instance = context;
    NfcEventType event_type = event.type;
    NfcaListenerEvent nfca_listener_event = {};
    if(event_type == NfcEventTypeListenerActivated) {
        instance->state = NfcaListenerStateActive;
    } else if(event_type == NfcEventTypeRxEnd) {
        if(nfca_listener_halt_received(event.data.rx_data, event.data.rx_bits)) {
            nfca_listener_sleep(instance);
            instance->state = NfcaListenerStateIdle;
            if(instance->callback) {
                nfca_listener_event.type = NfcaListenerEventTypeHalted;
                instance->callback(nfca_listener_event, instance->context);
            }
        } else if(instance->callback) {
            nfca_listener_event.data.rx_data = event.data.rx_data;
            if(nfca_check_crc(event.data.rx_data, event.data.rx_bits / 8)) {
                nfca_listener_event.type = NfcaListenerEventTypeReceivedStandartFrame;
                nfca_listener_event.data.rx_bits = event.data.rx_bits - 16;
            } else {
                nfca_listener_event.type = NfcaListenerEventTypeReceivedData;
                nfca_listener_event.data.rx_bits = event.data.rx_bits;
            }
            instance->callback(nfca_listener_event, instance->context);
        }
    }
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
    nfc_listener_start(instance->nfc, nfca_listener_event_handler, instance);

    return instance;
}

void nfca_listener_free(NfcaListener* instance) {
    furi_assert(instance);
    nfc_free(instance->nfc);
    free(instance);
}

NfcaError nfca_listener_set_callback(
    NfcaListener* instance,
    NfcaListenerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return NfcaErrorNone;
}

NfcaError nfca_listener_sleep(NfcaListener* instance) {
    furi_assert(instance);

    NfcError error = nfc_listener_sleep(instance->nfc);
    instance->state = NfcaListenerStateIdle;

    return nfca_listener_process_nfc_error(error);
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

NfcaError
    nfca_listener_send_standart_frame(NfcaListener* instance, uint8_t* tx_data, uint16_t tx_bits) {
    furi_assert(instance);
    furi_assert(tx_data);

    NfcaError ret = NfcaErrorNone;
    // TODO another buffer ...
    uint8_t tx_buff[64];
    uint16_t tx_bytes = tx_bits / 8;

    do {
        if(tx_bytes > sizeof(tx_buff) - 2) {
            ret = NfcaErrorBufferOverflow;
            break;
        }
        memcpy(tx_buff, tx_data, tx_bytes);
        nfca_append_crc(tx_buff, tx_bytes);

        NfcError error = nfc_listener_tx(instance->nfc, tx_buff, tx_bits + 16);
        if(error != NfcErrorNone) {
            FURI_LOG_W(TAG, "Tx error: %d", error);
            ret = nfca_listener_process_nfc_error(error);
            break;
        }
    } while(false);

    return ret;
}
