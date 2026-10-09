#include "nfc_cli_apdu_innovatron.h"

#include <nfc/protocols/innovatron/innovatron.h>
#include <nfc/protocols/innovatron/innovatron_poller.h>

#define TAG "InnovatronAPDU"

static NfcCliApduError nfc_cli_apdu_innovatron_process_error(InnovatronError error) {
    switch(error) {
    case InnovatronErrorNone:
        return NfcCliApduErrorNone;
    case InnovatronErrorTimeout:
        return NfcCliApduErrorTimeout;
    case InnovatronErrorWrongCrc:
        return NfcCliApduErrorWrongCrc;
    case InnovatronErrorNotPresent:
        return NfcCliApduErrorNotPresent;
    default:
        return NfcCliApduErrorProtocol;
    }
}

NfcCommand
    nfc_cli_apdu_innovatron_handler(NfcGenericEvent event, NfcCliApduRequestResponse* instance) {
    InnovatronPollerEvent* innovatron_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    if(innovatron_event->type == InnovatronPollerEventTypeReady) {
        InnovatronError err =
            innovatron_poller_com_r(event.instance, instance->tx_buffer, instance->rx_buffer);
        instance->result = nfc_cli_apdu_innovatron_process_error(err);
        if(err != InnovatronErrorNone) command = NfcCommandStop;
    } else if(innovatron_event->type == InnovatronPollerEventTypeError) {
        instance->result = nfc_cli_apdu_innovatron_process_error(innovatron_event->data->error);
        command = NfcCommandStop;
    }

    return command;
}
