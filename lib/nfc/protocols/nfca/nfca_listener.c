#include "nfca_listener.h"

#include <furi.h>
#include <lib/nfc/nfc.h>

#define TAG "NfcaListener"

#define NFCA_LISTENER_MAX_BUFFER_SIZE (256)

typedef enum {
    NfcaListenerStateIdle,
    NfcaListenerStateActive,
} NfcaListenerState;

struct NfcaListener {
    Nfc* nfc;
    NfcaData* data;
    NfcaListenerState state;
    NfcaListenerEventCallback callback;

    BitBuffer* tx_buffer;
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

static void nfca_listener_config(NfcaListener* instance) {
    furi_assert(instance);

    instance->tx_buffer = bit_buffer_alloc(NFCA_LISTENER_MAX_BUFFER_SIZE);

    nfc_set_fdt_listen_fc(instance->nfc, NFCA_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeNfcaListener);
    nfc_listener_set_col_res_data(
        instance->nfc,
        instance->data->uid,
        instance->data->uid_len,
        instance->data->atqa,
        instance->data->sak);
}

static void nfca_listener_reset(NfcaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    instance->tx_buffer = NULL;
}

static bool nfca_listener_halt_received(BitBuffer* buf) {
    bool halt_cmd_received = false;

    do {
        if(bit_buffer_get_size_bytes(buf) != 4) break;
        if(!nfca_check_crc(buf)) break;
        if(bit_buffer_get_byte(buf, 0) != 0x50) break;
        if(bit_buffer_get_byte(buf, 1) != 0x00) break;
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

    if(event_type == NfcEventTypeConfigureRequest) {
        nfca_listener_config(instance);
        if(instance->callback) {
            nfca_listener_event.type = NfcaListenerEventConfigRequest;
            instance->callback(nfca_listener_event, instance->context);
        }
    } else if(event_type == NfcEventTypeListenerActivated) {
        instance->state = NfcaListenerStateActive;
    } else if((event_type == NfcEventTypeRxEnd) && (instance->state == NfcaListenerStateActive)) {
        if(nfca_listener_halt_received(event.data.buffer)) {
            // TODO rework with commands
            nfca_listener_sleep(instance);
            instance->state = NfcaListenerStateIdle;
            if(instance->callback) {
                nfca_listener_event.type = NfcaListenerEventTypeHalted;
                instance->callback(nfca_listener_event, instance->context);
            }
        } else if(instance->callback) {
            if(nfca_check_crc(event.data.buffer)) {
                nfca_listener_event.type = NfcaListenerEventTypeReceivedStandartFrame;
                size_t bytes = bit_buffer_get_size_bytes(event.data.buffer);
                bit_buffer_set_size_bytes(event.data.buffer, bytes - 2);
            } else {
                nfca_listener_event.type = NfcaListenerEventTypeReceivedData;
            }
            nfca_listener_event.data.buffer = event.data.buffer;
            if(instance->callback) {
                instance->callback(nfca_listener_event, instance->context);
            }
        }
    } else if(event_type == NfcEventTypeReset) {
        nfca_listener_reset(instance);
        if(instance->callback) {
            nfca_listener_event.type = NfcaListenerEventTypeReset;
            instance->callback(nfca_listener_event, instance->context);
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
    const NfcaData* data,
    NfcaListenerEventCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;

    instance->data = nfca_alloc();
    nfca_copy(instance->data, data);

    nfc_start_listener(instance->nfc, nfca_listener_event_handler, instance);

    return NfcaErrorNone;
}

NfcaError nfca_listener_stop(NfcaListener* instance) {
    furi_assert(instance);

    nfc_listener_abort(instance->nfc);
    nfca_free(instance->data);

    instance->callback = NULL;
    instance->context = NULL;
    instance->state = NfcaListenerStateIdle;

    return NfcaErrorNone;
}

NfcaError nfca_listener_get_data(NfcaListener* instance, NfcaData* data) {
    furi_assert(instance);

    nfca_copy(data, instance->data);

    return NfcaErrorNone;
}

NfcaError nfca_listener_sleep(NfcaListener* instance) {
    furi_assert(instance);

    NfcError error = nfc_listener_sleep(instance->nfc);
    instance->state = NfcaListenerStateIdle;

    return nfca_listener_process_nfc_error(error);
}

NfcaError nfca_listener_tx(NfcaListener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    NfcaError ret = NfcaErrorNone;
    NfcError error = nfc_listener_tx(instance->nfc, tx_buffer);
    if(error != NfcErrorNone) {
        FURI_LOG_W(TAG, "Tx error: %d", error);
        ret = nfca_listener_process_nfc_error(error);
    }

    return ret;
}

NfcaError nfca_listener_send_standart_frame(NfcaListener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(instance->tx_buffer);

    NfcaError ret = NfcaErrorNone;
    do {
        bit_buffer_copy(instance->tx_buffer, tx_buffer);
        nfca_append_crc(instance->tx_buffer);

        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        if(error != NfcErrorNone) {
            FURI_LOG_W(TAG, "Tx error: %d", error);
            ret = nfca_listener_process_nfc_error(error);
            break;
        }
    } while(false);

    return ret;
}
