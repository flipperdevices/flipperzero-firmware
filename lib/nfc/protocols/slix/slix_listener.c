#include "slix_listener_i.h"

#include <furi.h>
#include <nfc/protocols/nfc_listener_base.h>

#define TAG "SlixListener"

#define SLIX_LISTENER_BUF_SIZE (64U)

static SlixListener* slix_listener_alloc(Iso15693_3Listener* iso15693_3_listener, SlixData* data) {
    furi_assert(iso15693_3_listener);

    SlixListener* instance = malloc(sizeof(SlixListener));
    instance->iso15693_3_listener = iso15693_3_listener;
    instance->data = data;

    instance->tx_buffer = bit_buffer_alloc(SLIX_LISTENER_BUF_SIZE);

    instance->slix_event.data = &instance->slix_event_data;
    instance->generic_event.protocol = NfcProtocolSlix;
    instance->generic_event.instance = instance;
    instance->generic_event.event_data = &instance->slix_event;

    slix_listener_init_iso15693_3_extensions(instance);

    instance->history.base.protocol = NfcProtocolSlix;
    instance->history.base.data_block_size = sizeof(SlixListenerHistoryData);
    instance->history.data = &instance->history_data;
    return instance;
}

static void slix_listener_free(SlixListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    free(instance);
}

static void slix_listener_set_callback(
    SlixListener* instance,
    NfcGenericCallback callback,
    NfcGenericLogHistoryCallback log_callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
    instance->log_callback = log_callback;
}

static const SlixData* slix_listener_get_data(SlixListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

static NfcCommand slix_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso15693_3);
    furi_assert(event.event_data);

    SlixListener* instance = context;
    Iso15693_3ListenerEvent* iso15693_3_event = event.event_data;
    BitBuffer* rx_buffer = iso15693_3_event->data->buffer;
    NfcCommand command = NfcCommandContinue;

    if(iso15693_3_event->type == Iso15693_3ListenerEventTypeCustomCommand) {
        const SlixError error = slix_listener_process_request(instance, rx_buffer);
        instance->history_data.error = error;
        if(error == SlixErrorWrongPassword) {
            command = NfcCommandSleep;
        }
    }

    instance->history_data.command = command;
    instance->history_data.event = iso15693_3_event->type;
    instance->history_data.session_state = instance->session_state;
    instance->history.base.modified = true;
    return command;
}

void slix_log_history(NfcLogger* logger, void* context) {
    SlixListener* instance = context;
    nfc_logger_append_history(logger, &instance->history);

    if(instance->log_callback) {
        instance->log_callback(logger, instance->context);
    }
}

const NfcListenerBase nfc_listener_slix = {
    .alloc = (NfcListenerAlloc)slix_listener_alloc,
    .free = (NfcListenerFree)slix_listener_free,
    .set_callback = (NfcListenerSetCallback)slix_listener_set_callback,
    .get_data = (NfcListenerGetData)slix_listener_get_data,
    .run = (NfcListenerRun)slix_listener_run,
    .log_history = (NfcListenerLogHistory)slix_log_history,
};
