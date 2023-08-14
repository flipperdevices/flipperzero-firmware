#include "iso15693_3_listener_i.h"

#include <furi.h>

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_listener_base.h>
#include <nfc/helpers/iso13239_crc.h>

#define TAG "Iso15693_3Listener"

#define ISO15693_3_LISTENER_BUFFER_SIZE (64U)

Iso15693_3Listener* iso15693_3_listener_alloc(Nfc* nfc, const Iso15693_3Data* data) {
    furi_assert(nfc);

    Iso15693_3Listener* instance = malloc(sizeof(Iso15693_3Listener));
    instance->nfc = nfc;
    instance->data = iso15693_3_alloc();
    iso15693_3_copy(instance->data, data);
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
    iso15693_3_free(instance->data);

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

    if(nfc_event->type == NfcEventTypeFieldOff) {
        if(instance->callback) {
            instance->iso15693_3_event.type = Iso15693_3ListenerEventTypeFieldOff;
            command = instance->callback(instance->generic_event, instance->context);
        }
        FURI_LOG_D(TAG, "Field OFF");
        iso15693_3_listener_sleep(instance);
    } else if(nfc_event->type == NfcEventTypeRxEnd) {
        if(iso13239_crc_check(Iso13239CrcTypeDefault, nfc_event->data.buffer)) {
            iso13239_crc_trim(nfc_event->data.buffer);
            iso15693_3_listener_process_request(instance, nfc_event->data.buffer);
        } else {
            FURI_LOG_D(TAG, "Wrong CRC");
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
