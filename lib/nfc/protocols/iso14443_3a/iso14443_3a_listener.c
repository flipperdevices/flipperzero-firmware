#include "iso14443_3a_listener.h"

#include <furi.h>
#include <lib/nfc/nfc.h>

#define TAG "Iso14443_3aListener"

#define ISO14443_3A_LISTENER_MAX_BUFFER_SIZE (256)

typedef enum {
    Iso14443_3aListenerStateIdle,
    Iso14443_3aListenerStateActive,
} Iso14443_3aListenerState;

struct Iso14443_3aListener {
    Nfc* nfc;
    Iso14443_3aData* data;
    Iso14443_3aListenerState state;
    Iso14443_3aListenerEventCallback callback;

    BitBuffer* tx_buffer;
    void* context;
};

static Iso14443_3aError iso14443_3a_listener_process_nfc_error(NfcError error) {
    Iso14443_3aError ret = Iso14443_3aErrorNone;

    if(error == NfcErrorNone) {
        ret = Iso14443_3aErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = Iso14443_3aErrorTimeout;
    } else {
        ret = Iso14443_3aErrorFieldOff;
    }

    return ret;
}

static void iso14443_3a_listener_config(Iso14443_3aListener* instance) {
    furi_assert(instance);

    instance->tx_buffer = bit_buffer_alloc(ISO14443_3A_LISTENER_MAX_BUFFER_SIZE);

    nfc_set_fdt_listen_fc(instance->nfc, ISO14443_3A_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeIso14443_3aListener);
    nfc_listener_set_col_res_data(
        instance->nfc,
        instance->data->uid,
        instance->data->uid_len,
        instance->data->atqa,
        instance->data->sak);
}

static void iso14443_3a_listener_reset(Iso14443_3aListener* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    instance->tx_buffer = NULL;
}

static bool iso14443_3a_listener_halt_received(BitBuffer* buf) {
    bool halt_cmd_received = false;

    do {
        if(bit_buffer_get_size_bytes(buf) != 4) break;
        if(!iso14443_3a_check_crc(buf)) break;
        if(bit_buffer_get_byte(buf, 0) != 0x50) break;
        if(bit_buffer_get_byte(buf, 1) != 0x00) break;
        halt_cmd_received = true;
    } while(false);

    return halt_cmd_received;
}

static NfcCommand iso14443_3a_listener_event_handler(NfcEvent event, void* context) {
    furi_assert(context);

    Iso14443_3aListener* instance = context;
    NfcEventType event_type = event.type;
    Iso14443_3aListenerEvent iso14443_3a_listener_event = {};
    NfcCommand command = NfcCommandContinue;

    if(event_type == NfcEventTypeConfigureRequest) {
        iso14443_3a_listener_config(instance);
        if(instance->callback) {
            iso14443_3a_listener_event.type = Iso14443_3aListenerEventConfigRequest;
            instance->callback(iso14443_3a_listener_event, instance->context);
        }
    } else if(event_type == NfcEventTypeListenerActivated) {
        instance->state = Iso14443_3aListenerStateActive;
    } else if(
        (event_type == NfcEventTypeRxEnd) && (instance->state == Iso14443_3aListenerStateActive)) {
        if(iso14443_3a_listener_halt_received(event.data.buffer)) {
            // TODO rework with commands
            iso14443_3a_listener_sleep(instance);
            instance->state = Iso14443_3aListenerStateIdle;
            if(instance->callback) {
                iso14443_3a_listener_event.type = Iso14443_3aListenerEventTypeHalted;
                instance->callback(iso14443_3a_listener_event, instance->context);
            }
        } else if(instance->callback) {
            if(iso14443_3a_check_crc(event.data.buffer)) {
                iso14443_3a_listener_event.type =
                    Iso14443_3aListenerEventTypeReceivedStandartFrame;
                size_t bytes = bit_buffer_get_size_bytes(event.data.buffer);
                bit_buffer_set_size_bytes(event.data.buffer, bytes - 2);
            } else {
                iso14443_3a_listener_event.type = Iso14443_3aListenerEventTypeReceivedData;
            }
            iso14443_3a_listener_event.data.buffer = event.data.buffer;
            if(instance->callback) {
                instance->callback(iso14443_3a_listener_event, instance->context);
            }
        }
    } else if(event_type == NfcEventTypeReset) {
        iso14443_3a_listener_reset(instance);
        if(instance->callback) {
            iso14443_3a_listener_event.type = Iso14443_3aListenerEventTypeReset;
            instance->callback(iso14443_3a_listener_event, instance->context);
        }
    }

    return command;
}

Iso14443_3aListener* iso14443_3a_listener_alloc(Nfc* nfc) {
    furi_assert(nfc);

    Iso14443_3aListener* instance = malloc(sizeof(Iso14443_3aListener));
    instance->nfc = nfc;

    return instance;
}

void iso14443_3a_listener_free(Iso14443_3aListener* instance) {
    furi_assert(instance);
    free(instance);
}

Iso14443_3aError iso14443_3a_listener_start(
    Iso14443_3aListener* instance,
    const Iso14443_3aData* data,
    Iso14443_3aListenerEventCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;

    instance->data = iso14443_3a_alloc();
    iso14443_3a_copy(instance->data, data);

    nfc_start_listener(instance->nfc, iso14443_3a_listener_event_handler, instance);

    return Iso14443_3aErrorNone;
}

Iso14443_3aError iso14443_3a_listener_stop(Iso14443_3aListener* instance) {
    furi_assert(instance);

    nfc_listener_abort(instance->nfc);
    iso14443_3a_free(instance->data);

    instance->callback = NULL;
    instance->context = NULL;
    instance->state = Iso14443_3aListenerStateIdle;

    return Iso14443_3aErrorNone;
}

Iso14443_3aError
    iso14443_3a_listener_get_data(Iso14443_3aListener* instance, Iso14443_3aData* data) {
    furi_assert(instance);

    iso14443_3a_copy(data, instance->data);

    return Iso14443_3aErrorNone;
}

Iso14443_3aError iso14443_3a_listener_sleep(Iso14443_3aListener* instance) {
    furi_assert(instance);

    NfcError error = nfc_listener_sleep(instance->nfc);
    instance->state = Iso14443_3aListenerStateIdle;

    return iso14443_3a_listener_process_nfc_error(error);
}

Iso14443_3aError
    iso14443_3a_listener_tx(Iso14443_3aListener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    Iso14443_3aError ret = Iso14443_3aErrorNone;
    NfcError error = nfc_listener_tx(instance->nfc, tx_buffer);
    if(error != NfcErrorNone) {
        FURI_LOG_W(TAG, "Tx error: %d", error);
        ret = iso14443_3a_listener_process_nfc_error(error);
    }

    return ret;
}

Iso14443_3aError iso14443_3a_listener_send_standart_frame(
    Iso14443_3aListener* instance,
    const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(instance->tx_buffer);

    Iso14443_3aError ret = Iso14443_3aErrorNone;
    do {
        bit_buffer_copy(instance->tx_buffer, tx_buffer);
        iso14443_3a_append_crc(instance->tx_buffer);

        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        if(error != NfcErrorNone) {
            FURI_LOG_W(TAG, "Tx error: %d", error);
            ret = iso14443_3a_listener_process_nfc_error(error);
            break;
        }
    } while(false);

    return ret;
}
