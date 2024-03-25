#include "iso14443_3b.h"
#include <nfc/protocols/iso14443_3b/iso14443_3b_poller.h>

#include "../nfc_cli_protocol_support_common.h"

static Iso14443_3bError nfc_cli_iso14443_3b_process_nfc_error(NfcError error) {
    Iso14443_3bError ret = Iso14443_3bErrorNone;

    switch(error) {
    case NfcErrorNone:
        ret = Iso14443_3bErrorNone;
        break;
    case NfcErrorTimeout:
        ret = Iso14443_3bErrorTimeout;
        break;
    default:
        ret = Iso14443_3bErrorColResFailed;
        break;
    }

    return ret;
}

static NfcCliPollerError nfc_cli_iso14443_3b_process_error(Iso14443_3bError error) {
    NfcCliPollerError ret = NfcCliPollerErrorNone;

    switch(error) {
    case Iso14443_3bErrorNone:
        ret = NfcCliPollerErrorNone;
        break;
    case Iso14443_3bErrorTimeout:
    case Iso14443_3bErrorNotPresent:
        ret = NfcCliPollerErrorTimeout;
        break;
    default:
        ret = NfcCliPollerErrorInternal;
        break;
    }

    return ret;
}

static void iso14443_3b_request_handler(NfcCliProtocolRequest* request) {
    furi_assert(request);

    Iso14443_3bPoller* poller = request->data.poller;
    Iso14443_3bError error = Iso14443_3bErrorNone;

    if(request->type == NfcCliProtocolRequestTypeActivate) {
        Iso14443_3bData* iso3b_data = iso14443_3b_alloc();
        Iso14443_3bError error = iso14443_3b_poller_activate(poller, iso3b_data);
        request->data.error = nfc_cli_iso14443_3b_process_error(error);
        if(request->data.error == NfcCliPollerErrorNone) {
            FuriString* activation_info = request->data.activation_info;
            furi_string_printf(activation_info, "UID:");
            size_t uid_len = 0;
            const uint8_t* uid = iso14443_3b_get_uid(iso3b_data, &uid_len);
            for(size_t i = 0; i < uid_len; i++) {
                furi_string_cat_printf(activation_info, " %02X", uid[i]);
            }
        }
        iso14443_3b_free(iso3b_data);
    } else if(request->type == NfcCliProtocolRequestTypeFrameExchange) {
        const NfcCliPollCmdData* tx_data = request->data.frame_exchange.tx_data;
        if(tx_data->append_crc) {
            error = iso14443_3b_poller_send_frame(
                poller, tx_data->tx_data, request->data.frame_exchange.rx_data);
        } else {
            NfcError nfc_error = nfc_poller_trx(
                request->data.nfc,
                tx_data->tx_data,
                request->data.frame_exchange.rx_data,
                tx_data->timeout);
            error = nfc_cli_iso14443_3b_process_nfc_error(nfc_error);
        }

        if(error == Iso14443_3bErrorWrongCrc) {
            request->data.error = NfcCliPollerErrorNone;
        } else {
            request->data.error = nfc_cli_iso14443_3b_process_error(error);
        }
    }
}

static void iso14443_3b_cmd_handler_poll(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_poll_handler(
        NfcProtocolIso14443_3b, nfc_cli, args, iso14443_3b_request_handler);
}

static void iso14443_3b_cmd_handler_start_poller(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_start_poller_handler(
        NfcProtocolIso14443_3b, nfc_cli, args, iso14443_3b_request_handler);
}

const NfcCliProtocolSupportBase nfc_cli_protocol_support_base_iso14443_3b = {
    .protocol = NfcProtocolIso14443_3b,
    .cmd_name = "iso14443-3b",
    .cmd_handler =
        {
            iso14443_3b_cmd_handler_poll,
            iso14443_3b_cmd_handler_start_poller,
        },
};
