#include "nfc_cli_dump_legic_prime.h"
#include <nfc/protocols/legic_prime/legic_prime_poller.h>

NfcCommand nfc_cli_dump_poller_callback_legic_prime(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolLegicPrime);

    NfcCliDumpContext* instance = context;
    const LegicPrimePollerEvent* legic_event = event.event_data;

    NfcCommand command = NfcCommandContinue;

    if(legic_event->type == LegicPrimePollerEventTypeRequestMode) {
        legic_event->data->mode_request.mode = LegicPrimePollerModeRead;
    } else if(legic_event->type == LegicPrimePollerEventTypeSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolLegicPrime, nfc_poller_get_data(instance->poller));
        instance->result = NfcCliDumpErrorNone;
        command = NfcCommandStop;
    } else if(legic_event->type == LegicPrimePollerEventTypeFailure) {
        instance->result = legic_event->data->result.error == LegicPrimeErrorNotPresent ?
                               NfcCliDumpErrorNotPresent :
                               NfcCliDumpErrorFailedToRead;
        command = NfcCommandStop;
    }

    if(command == NfcCommandStop) {
        furi_semaphore_release(instance->sem_done);
    }

    return command;
}
