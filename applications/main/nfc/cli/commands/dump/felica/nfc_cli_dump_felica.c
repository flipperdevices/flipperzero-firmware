#include "nfc_cli_dump_felica.h"
#include <nfc/protocols/felica/felica_poller.h>

NfcCommand nfc_cli_dump_poller_callback_felica(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolFelica);

    NfcCliDumpContext* instance = context;
    const FelicaPollerEvent* felica_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    if(felica_event->type == FelicaPollerEventTypeReady) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolFelica, nfc_poller_get_data(instance->poller));
        //view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        command = NfcCommandStop;
    } else if(
        felica_event->type == FelicaPollerEventTypeError ||
        felica_event->type == FelicaPollerEventTypeIncomplete) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolFelica, nfc_poller_get_data(instance->poller));
        // view_dispatcher_send_custom_event(
        //     instance->view_dispatcher, NfcCustomEventPollerIncomplete);
        command = NfcCommandStop;
    } else if(felica_event->type == FelicaPollerEventTypeRequestAuthContext) {
        FelicaAuthenticationContext* ctx = felica_event->data->auth_context;
        ctx->skip_auth = true; //instance->felica_auth->skip_auth;
        //memcpy(ctx->card_key.data, instance->felica_auth->card_key.data, FELICA_DATA_BLOCK_SIZE);
    }

    if(command == NfcCommandStop) {
        furi_semaphore_release(instance->sem_done);
    }

    return command;
}
