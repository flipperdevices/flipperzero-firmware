#include "nfc_cli_raw_iso15693_3.h"

#include <nfc/helpers/iso13239_crc.h>
#include <nfc/protocols/iso15693_3/iso15693_3.h>
#include <nfc/protocols/iso15693_3/iso15693_3_poller_i.h>

#define TAG "ISO15"

static void iso15693_3_format_activation_data(const uint8_t* data, FuriString* output) {
    furi_string_printf(output, "UID:");
    for(size_t i = 0; i < ISO15693_3_UID_SIZE; i++) {
        furi_string_cat_printf(output, " %02X", data[i]);
    }
}

NfcCommand nfc_cli_raw_iso15693_3_handler(
    NfcGenericInstance* poller,
    const NfcCliRawRequest* request,
    NfcCliRawResponse* const response) {
    Iso15693_3Poller* iso15_poller = poller;

    if(request->select) {
        FURI_LOG_D(TAG, "Activating...");
        uint8_t uid[ISO15693_3_UID_SIZE] = {0};
        Iso15693_3Error error = iso15693_3_poller_inventory(iso15_poller, uid);
        if(error == Iso15693_3ErrorNone) {
            FURI_LOG_D(TAG, "Activate OK");
            iso15693_3_format_activation_data(uid, response->activation_string);
        }
    }

    uint32_t timeout = ISO15693_3_FDT_POLL_FC;
    //if(request->timeout > 0)
    //{ timeout = request->timeout; }

    if(bit_buffer_get_size_bytes(request->tx_buffer) > 0) {
        FURI_LOG_D(TAG, "Tx");
        if(request->append_crc) {
            FURI_LOG_D(TAG, "Tx CRC");
            iso13239_crc_append(Iso13239CrcTypeDefault, request->tx_buffer);
        }

        bit_buffer_reset(response->rx_buffer);
        NfcError error =
            nfc_poller_trx(iso15_poller->nfc, request->tx_buffer, response->rx_buffer, timeout);

        if(error == NfcErrorNone) {
            FURI_LOG_D(TAG, "Tx OK");
        } else {
            FURI_LOG_D(TAG, "Tx Error");
        }
    }
    return request->keep_field ? NfcCommandContinue : NfcCommandStop;
}
