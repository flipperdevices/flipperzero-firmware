#include "nfc_cli_dump_ask_ctx.h"
#include <nfc/protocols/ask_ctx/ask_ctx_poller.h>

NfcCommand nfc_cli_dump_poller_callback_ask_ctx(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolAskCtx);

    NfcCliDumpContext* instance = context;
    const AskCtxPollerEvent* ask_ctx_event = event.event_data;

    NfcCommand command = NfcCommandContinue;

    if(ask_ctx_event->type == AskCtxPollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolAskCtx, nfc_poller_get_data(instance->poller));
        instance->result = NfcCliDumpErrorNone;
        command = NfcCommandStop;
    } else if(ask_ctx_event->type == AskCtxPollerEventTypeReadFailed) {
        instance->result = NfcCliDumpErrorFailedToRead;
        command = NfcCommandStop;
    }

    if(command == NfcCommandStop) {
        furi_semaphore_release(instance->sem_done);
    }

    return command;
}
