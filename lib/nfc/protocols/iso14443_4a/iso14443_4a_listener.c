#include "iso14443_4a_listener_i.h"

#include <furi.h>
#include <nfc/protocols/nfc_listener_base.h>

#define TAG "Iso14443_4aListener"

#define ISO14443_4A_LISTENER_MAX_BUFFER_SIZE (512U)

static Iso14443_4aListener* iso14443_4a_listener_alloc(
    Iso14443_3aListener* iso14443_3a_listener,
    const Iso14443_4aData* data) {
    furi_assert(iso14443_3a_listener);

    Iso14443_4aListener* instance = malloc(sizeof(Iso14443_4aListener));
    instance->iso14443_3a_listener = iso14443_3a_listener;
    instance->data = iso14443_4a_alloc();
    iso14443_4a_copy(instance->data, data);

    instance->tx_buffer = bit_buffer_alloc(ISO14443_4A_LISTENER_MAX_BUFFER_SIZE);

    instance->iso14443_4a_event.data = &instance->iso14443_4a_event_data;
    instance->generic_event.protocol = NfcProtocolIso14443_4a;
    instance->generic_event.instance = instance;
    instance->generic_event.data = &instance->iso14443_4a_event;

    return instance;
}

static void iso14443_4a_listener_free(Iso14443_4aListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    iso14443_4a_free(instance->data);
    free(instance);
}

static void iso14443_4a_listener_set_callback(
    Iso14443_4aListener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

static const Iso14443_4aData* iso14443_4a_listener_get_data(Iso14443_4aListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

static NfcCommand iso14443_4a_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.data);

    Iso14443_4aListener* instance = context;
    Iso14443_3aListenerEvent* iso14443_3a_event = event.data;
    BitBuffer* rx_buffer = iso14443_3a_event->data->buffer;
    NfcCommand command = NfcCommandContinue;

    if(iso14443_3a_event->type == Iso14443_3aListenerEventTypeReceivedStandardFrame) {
        UNUSED(instance);
        FURI_LOG_D(
            TAG,
            "Standard frame recieived with length of %zu",
            bit_buffer_get_size_bytes(rx_buffer));
    }

    return command;
}

const NfcListenerBase nfc_listener_iso14443_4a = {
    .alloc = (NfcListenerAlloc)iso14443_4a_listener_alloc,
    .free = (NfcListenerFree)iso14443_4a_listener_free,
    .set_callback = (NfcListenerSetCallback)iso14443_4a_listener_set_callback,
    .get_data = (NfcListenerGetData)iso14443_4a_listener_get_data,
    .run = (NfcListenerRun)iso14443_4a_listener_run,
};
