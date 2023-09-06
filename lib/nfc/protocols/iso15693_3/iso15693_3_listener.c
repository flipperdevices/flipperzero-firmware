#include "iso15693_3_listener_i.h"

#include <furi.h>

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_listener_base.h>
#include <nfc/helpers/iso13239_crc.h>

#define TAG "Iso15693_3Listener"

#define ISO15693_3_LISTENER_BUFFER_SIZE (64U)

Iso15693_3Listener* iso15693_3_listener_alloc(Nfc* nfc, Iso15693_3Data* data) {
    furi_assert(nfc);

    Iso15693_3Listener* instance = malloc(sizeof(Iso15693_3Listener));
    instance->nfc = nfc;
    instance->data = data;

    instance->tx_buffer = bit_buffer_alloc(ISO15693_3_LISTENER_BUFFER_SIZE);

    instance->iso15693_3_event.data = &instance->iso15693_3_event_data;
    instance->generic_event.protocol = NfcProtocolIso15693_3;
    instance->generic_event.instance = instance;
    instance->generic_event.data = &instance->iso15693_3_event;

    nfc_set_fdt_listen_fc(instance->nfc, ISO15693_3_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeIso15693Listener);

    return instance;
}

void iso15693_3_listener_free(Iso15693_3Listener* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->tx_buffer);

    free(instance);
}

void iso15693_3_listener_set_callback(
    Iso15693_3Listener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

const Iso15693_3Data* iso15693_3_listener_get_data(Iso15693_3Listener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

NfcCommand iso15693_3_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.data);

    Iso15693_3Listener* instance = context;
    NfcEvent* nfc_event = event.data;
    NfcCommand command = NfcCommandContinue;

    if(nfc_event->type == NfcEventTypeFieldOn) {
        iso15693_3_listener_ready(instance);
    } else if(nfc_event->type == NfcEventTypeFieldOff) {
        if(instance->callback) {
            instance->iso15693_3_event.type = Iso15693_3ListenerEventTypeFieldOff;
            command = instance->callback(instance->generic_event, instance->context);
        }
        iso15693_3_listener_sleep(instance);
    } else if(nfc_event->type == NfcEventTypeRxEnd) {
        BitBuffer* request_buf = nfc_event->data.buffer;
        if(iso13239_crc_check(Iso13239CrcTypeDefault, request_buf)) {
            iso13239_crc_trim(request_buf);
            const Iso15693_3Error error =
                iso15693_3_listener_process_request(instance, request_buf);
            if(error == Iso15693_3ErrorNotSupported) {
                instance->iso15693_3_event.type = Iso15693_3ListenerEventTypeCustomCommand;
                command = instance->callback(instance->generic_event, instance->context);
            }
        } else if(bit_buffer_get_size(request_buf) == 0) {
            // Special case: Single EOF
            if(instance->session_state.wait_for_eof) {
                iso15693_3_listener_send_frame(instance, instance->tx_buffer);
                instance->session_state.wait_for_eof = false;
            } else if(instance->callback) {
                instance->iso15693_3_event.type = Iso15693_3ListenerEventTypeSingleEof;
                command = instance->callback(instance->generic_event, instance->context);
            }
        } else {
            FURI_LOG_D(
                TAG, "Wrong CRC, buffer size: %zu", bit_buffer_get_size(nfc_event->data.buffer));
        }
    }

    return command;
}

const NfcListenerBase nfc_listener_iso15693_3 = {
    .alloc = (NfcListenerAlloc)iso15693_3_listener_alloc,
    .free = (NfcListenerFree)iso15693_3_listener_free,
    .set_callback = (NfcListenerSetCallback)iso15693_3_listener_set_callback,
    .get_data = (NfcListenerGetData)iso15693_3_listener_get_data,
    .run = (NfcListenerRun)iso15693_3_listener_run,
};
