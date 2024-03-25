#include "iso14443_3a.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#include "../nfc_cli_protocol_support_common.h"

static NfcCliPollerError nfc_cli_iso14443_3a_process_error(Iso14443_3aError error) {
    NfcCliPollerError ret = NfcCliPollerErrorNone;

    switch(error) {
    case Iso14443_3aErrorNone:
        ret = NfcCliPollerErrorNone;
        break;
    case Iso14443_3aErrorTimeout:
    case Iso14443_3aErrorNotPresent:
        ret = NfcCliPollerErrorTimeout;
        break;
    default:
        ret = NfcCliPollerErrorInternal;
        break;
    }

    return ret;
}

static void iso14443_3a_request_handler(NfcCliProtocolRequest* request) {
    furi_assert(request);

    Iso14443_3aPoller* poller = request->data.poller;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    if(request->type == NfcCliProtocolRequestTypeActivate) {
        Iso14443_3aData iso3_data = {};
        Iso14443_3aError error = iso14443_3a_poller_activate(poller, &iso3_data);
        request->data.error = nfc_cli_iso14443_3a_process_error(error);
        if(request->data.error == NfcCliPollerErrorNone) {
            FuriString* activation_info = request->data.activation_info;
            furi_string_printf(activation_info, "UID:");
            for(size_t i = 0; i < iso3_data.uid_len; i++) {
                furi_string_cat_printf(activation_info, " %02X", iso3_data.uid[i]);
            }
            furi_string_cat_printf(
                activation_info,
                " ATQA: %02X%02X SAK: %02X",
                iso3_data.atqa[0],
                iso3_data.atqa[1],
                iso3_data.sak);
        }
    } else if(request->type == NfcCliProtocolRequestTypeFrameExchange) {
        const NfcCliPollCmdData* tx_data = request->data.frame_exchange.tx_data;
        if(tx_data->append_crc) {
            error = iso14443_3a_poller_send_standard_frame(
                poller, tx_data->tx_data, request->data.frame_exchange.rx_data, tx_data->timeout);
        } else {
            error = iso14443_3a_poller_txrx(
                poller, tx_data->tx_data, request->data.frame_exchange.rx_data, tx_data->timeout);
        }

        if(error == Iso14443_3aErrorWrongCrc) {
            request->data.error = NfcCliPollerErrorNone;
        } else {
            request->data.error = nfc_cli_iso14443_3a_process_error(error);
        }
    }
}

static void iso14443_3a_cmd_handler_poll(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_poll_handler(
        NfcProtocolIso14443_3a, nfc_cli, args, iso14443_3a_request_handler);
}

static void iso14443_3a_cmd_handler_start_poller(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_start_poller_handler(
        NfcProtocolIso14443_3a, nfc_cli, args, iso14443_3a_request_handler);
}

const NfcCliProtocolSupportBase nfc_cli_protocol_support_base_iso14443_3a = {
    .protocol = NfcProtocolIso14443_3a,
    .cmd_name = "iso14443-3a",
    .cmd_handler =
        {
            iso14443_3a_cmd_handler_poll,
            iso14443_3a_cmd_handler_start_poller,
        },
};
