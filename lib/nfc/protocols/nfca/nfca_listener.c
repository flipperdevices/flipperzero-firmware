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
    NfcaData* data;
    NfcaListenerState state;
    NfcaListenerEventCallback callback;
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

static NfcCommand nfca_listener_event_handler(NfcEvent event, void* context) {
    furi_assert(context);

    NfcaListener* instance = context;
    NfcEventType event_type = event.type;
    NfcaListenerEvent nfca_listener_event = {};
    NfcCommand command = NfcCommandContinue;

    if(event_type == NfcEventTypeListenerActivated) {
        instance->state = NfcaListenerStateActive;
    } else if((event_type == NfcEventTypeRxEnd) && (instance->state == NfcaListenerStateActive)) {
        if(nfca_listener_halt_received(event.data.rx_data, event.data.rx_bits)) {
            // TODO rework with commands
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
            if(instance->callback) {
                instance->callback(nfca_listener_event, instance->context);
            }
        }
    }

    return command;
}

NfcaListener* nfca_listener_alloc(Nfc* nfc) {
    furi_assert(nfc);

    NfcaListener* instance = malloc(sizeof(NfcaListener));
    instance->nfc = nfc;

    return instance;
}

void nfca_listener_free(NfcaListener* instance) {
    furi_assert(instance);
    free(instance);
}

NfcaError nfca_listener_start(
    NfcaListener* instance,
    NfcaData* data,
    NfcaListenerEventCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;

    instance->data = malloc(sizeof(NfcaData));
    *instance->data = *data;

    nfc_set_fdt_listen_fc(instance->nfc, NFCA_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeNfcaListener);
    nfc_listener_set_col_res_data(
        instance->nfc,
        instance->data->uid,
        instance->data->uid_len,
        instance->data->atqa,
        instance->data->sak);
    nfc_start_worker(instance->nfc, nfca_listener_event_handler, instance);

    return NfcaErrorNone;
}

NfcaError nfca_listener_reset(NfcaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    furi_assert(instance->data);

    nfc_listener_abort(instance->nfc);
    free(instance->data);
    instance->callback = NULL;
    instance->context = NULL;
    instance->state = NfcaListenerStateIdle;

    return NfcaErrorNone;
}

NfcaError nfca_listener_get_data(NfcaListener* instance, NfcaData* data) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(instance->data);

    *data = *instance->data;

    return NfcaErrorNone;
}

NfcaError nfca_listener_sleep(NfcaListener* instance) {
    furi_assert(instance);

    NfcError error = nfc_listener_sleep(instance->nfc);
    instance->state = NfcaListenerStateIdle;

    return nfca_listener_process_nfc_error(error);
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
