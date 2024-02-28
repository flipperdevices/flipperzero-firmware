#include "felica.h"
#include <nfc/protocols/felica/felica_poller.h>

#include "../nfc_cli_protocol_support_common.h"

static FelicaError nfc_cli_iso14443_3b_process_nfc_error(NfcError error) {
    FelicaError ret = FelicaErrorNone;

    switch(error) {
    case NfcErrorNone:
        ret = FelicaErrorNone;
        break;
    case NfcErrorTimeout:
        ret = FelicaErrorTimeout;
        break;
    default:
        ret = FelicaErrorProtocol;
        break;
    }

    return ret;
}

static NfcCliPollerError nfc_cli_felica_process_error(FelicaError error) {
    NfcCliPollerError ret = NfcCliPollerErrorNone;

    switch(error) {
    case FelicaErrorNone:
        ret = NfcCliPollerErrorNone;
        break;
    case FelicaErrorTimeout:
    case FelicaErrorNotPresent:
        ret = NfcCliPollerErrorTimeout;
        break;
    default:
        ret = NfcCliPollerErrorInternal;
        break;
    }

    return ret;
}

static void felica_request_handler(NfcCliProtocolRequest* request) {
    furi_assert(request);

    FelicaPoller* poller = request->data.poller;
    FelicaError error = FelicaErrorNone;

    if(request->type == NfcCliProtocolRequestTypeActivate) {
        FelicaData felica_data = {};
        FelicaError error = felica_poller_activate(poller, &felica_data);
        request->data.error = nfc_cli_felica_process_error(error);
        if(request->data.error == NfcCliPollerErrorNone) {
            FuriString* activation_info = request->data.activation_info;
            furi_string_printf(activation_info, "IDm:");
            for(size_t i = 0; i < FELICA_IDM_SIZE; i++) {
                furi_string_cat_printf(activation_info, " %02X", felica_data.idm.data[i]);
            }
        }
    } else if(request->type == NfcCliProtocolRequestTypeFrameExchange) {
        const NfcCliPollCmdData* tx_data = request->data.frame_exchange.tx_data;
        if(tx_data->append_crc) {
            error = felica_poller_frame_exchange(
                poller, tx_data->tx_data, request->data.frame_exchange.rx_data, tx_data->timeout);
        } else {
            NfcError nfc_error = nfc_poller_trx(
                request->data.nfc,
                tx_data->tx_data,
                request->data.frame_exchange.rx_data,
                tx_data->timeout);
            error = nfc_cli_iso14443_3b_process_nfc_error(nfc_error);
        }

        if(error == FelicaErrorWrongCrc) {
            request->data.error = NfcCliPollerErrorNone;
        } else {
            request->data.error = nfc_cli_felica_process_error(error);
        }
    }
}

static void felica_cmd_handler_poll(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_poll_handler(
        NfcProtocolFelica, nfc_cli, args, felica_request_handler);
}

static void felica_cmd_handler_start_poller(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_start_poller_handler(
        NfcProtocolFelica, nfc_cli, args, felica_request_handler);
}

const NfcCliProtocolSupportBase nfc_cli_protocol_support_base_felica = {
    .protocol = NfcProtocolFelica,
    .cmd_name = "felica",
    .cmd_handler =
        {
            felica_cmd_handler_poll,
            felica_cmd_handler_start_poller,
        },
};
