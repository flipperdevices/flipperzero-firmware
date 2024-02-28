#include "iso15693_3.h"
#include <nfc/protocols/iso15693_3/iso15693_3_poller.h>

#include "../nfc_cli_protocol_support_common.h"

static Iso15693_3Error nfc_cli_iso15693_3_process_nfc_error(NfcError error) {
    Iso15693_3Error ret = Iso15693_3ErrorNone;

    switch(error) {
    case NfcErrorNone:
        ret = Iso15693_3ErrorNone;
        break;
    case NfcErrorTimeout:
        ret = Iso15693_3ErrorTimeout;
        break;
    default:
        ret = Iso15693_3ErrorInternal;
        break;
    }

    return ret;
}

static NfcCliPollerError nfc_cli_iso15693_3_process_error(Iso15693_3Error error) {
    NfcCliPollerError ret = NfcCliPollerErrorNone;

    switch(error) {
    case Iso15693_3ErrorNone:
        ret = NfcCliPollerErrorNone;
        break;
    case Iso15693_3ErrorTimeout:
    case Iso15693_3ErrorNotPresent:
        ret = NfcCliPollerErrorTimeout;
        break;
    default:
        ret = NfcCliPollerErrorInternal;
        break;
    }

    return ret;
}

static void iso15693_3_request_handler(NfcCliProtocolRequest* request) {
    furi_assert(request);

    Iso15693_3Poller* poller = request->data.poller;
    Iso15693_3Error error = Iso15693_3ErrorNone;

    if(request->type == NfcCliProtocolRequestTypeActivate) {
        uint8_t uid[ISO15693_3_UID_SIZE] = {};
        Iso15693_3Error error = iso15693_3_poller_inventory(poller, uid);
        request->data.error = nfc_cli_iso15693_3_process_error(error);
        if(request->data.error == NfcCliPollerErrorNone) {
            FuriString* activation_info = request->data.activation_info;
            furi_string_printf(activation_info, "UID:");
            for(size_t i = 0; i < ISO15693_3_UID_SIZE; i++) {
                furi_string_cat_printf(activation_info, " %02X", uid[i]);
            }
        }
    } else if(request->type == NfcCliProtocolRequestTypeFrameExchange) {
        const NfcCliPollCmdData* tx_data = request->data.frame_exchange.tx_data;
        if(tx_data->append_crc) {
            error = iso15693_3_poller_send_frame(
                poller, tx_data->tx_data, request->data.frame_exchange.rx_data, tx_data->timeout);
        } else {
            NfcError nfc_error = nfc_poller_trx(
                request->data.nfc,
                tx_data->tx_data,
                request->data.frame_exchange.rx_data,
                tx_data->timeout);
            error = nfc_cli_iso15693_3_process_nfc_error(nfc_error);
        }

        if(error == Iso15693_3ErrorWrongCrc) {
            request->data.error = NfcCliPollerErrorNone;
        } else {
            request->data.error = nfc_cli_iso15693_3_process_error(error);
        }
    }
}

static void iso15693_3_cmd_handler_poll(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_poll_handler(
        NfcProtocolIso15693_3, nfc_cli, args, iso15693_3_request_handler);
}

static void iso15693_3_cmd_handler_start_poller(NfcCli* nfc_cli, FuriString* args) {
    nfc_cli_protocol_support_common_start_poller_handler(
        NfcProtocolIso15693_3, nfc_cli, args, iso15693_3_request_handler);
}

const NfcCliProtocolSupportBase nfc_cli_protocol_support_base_iso15693_3 = {
    .protocol = NfcProtocolIso15693_3,
    .cmd_name = "iso15693-3",
    .cmd_handler =
        {
            iso15693_3_cmd_handler_poll,
            iso15693_3_cmd_handler_start_poller,
        },
};
