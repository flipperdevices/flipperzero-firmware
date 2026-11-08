#include "nfc_cli_dump_innovatron.h"

#include <nfc/protocols/innovatron/innovatron_poller.h>

NfcCommand nfc_cli_dump_poller_callback_innovatron(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolInnovatron);

    NfcCliDumpContext* instance = context;
    const InnovatronPollerEvent* innovatron_event = event.event_data;

    NfcCommand command = NfcCommandContinue;

    if(innovatron_event->type == InnovatronPollerEventTypeReady) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolInnovatron, nfc_poller_get_data(instance->poller));
        instance->result = NfcCliDumpErrorNone;
        command = NfcCommandStop;
    } else if(innovatron_event->type == InnovatronPollerEventTypeError) {
        instance->result = NfcCliDumpErrorFailedToRead;
        command = NfcCommandStop;
    }

    if(command == NfcCommandStop) {
        furi_semaphore_release(instance->sem_done);
    }

    return command;
}
