#include "nfc_cli_dump_slix.h"
#include <nfc/protocols/slix/slix_poller.h>

NfcCommand nfc_cli_dump_poller_callback_slix(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolSlix);

    NfcCliDumpContext* instance = context;
    const SlixPollerEvent* slix_event = event.event_data;

    if(slix_event->type == SlixPollerEventTypeReady) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolSlix, nfc_poller_get_data(instance->poller));
        furi_semaphore_release(instance->sem_done);
        return NfcCommandStop;
    }

    return NfcCommandContinue;
}
