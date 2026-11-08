#include "nfc_cli_dump_felica_seac.h"

#include <nfc/protocols/felica_seac/felica_seac_poller.h>

NfcCommand nfc_cli_dump_poller_callback_felica_seac(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolFelicaSeac);

    NfcCliDumpContext* instance = context;
    const FelicaSeacPollerEvent* felica_seac_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    if(felica_seac_event->type == FelicaSeacPollerEventTypeReady) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolFelicaSeac, nfc_poller_get_data(instance->poller));
        instance->result = NfcCliDumpErrorNone;
        command = NfcCommandStop;
    } else if(felica_seac_event->type == FelicaSeacPollerEventTypeError) {
        instance->result = felica_seac_event->data->error == FelicaSeacErrorTimeout ?
                               NfcCliDumpErrorTimeout :
                               NfcCliDumpErrorFailedToRead;
        command = NfcCommandStop;
    }

    if(command == NfcCommandStop) {
        furi_semaphore_release(instance->sem_done);
    }

    return command;
}
