#include "nfc_cli_raw_iso14443_3a.h"

#include <nfc/helpers/iso14443_crc.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#define TAG "ISO14A"

NfcCommand nfc_cli_raw_iso14443_3a_handler(
    NfcGenericInstance* poller,
    const NfcCliRawRequest* request,
    NfcCliRawResponse* const response) {
    if(request->select) {
        Iso14443_3aData iso3_data = {};
        FURI_LOG_D(TAG, "Activating...");

        Iso14443_3aError error = iso14443_3a_poller_activate(poller, &iso3_data);
        if(error == Iso14443_3aErrorNone) {
            FURI_LOG_D(TAG, "Activate OK");

            furi_string_printf(response->activation_string, "UID:");
            for(size_t i = 0; i < iso3_data.uid_len; i++) {
                furi_string_cat_printf(response->activation_string, " %02X", iso3_data.uid[i]);
            }
            furi_string_cat_printf(
                response->activation_string,
                " ATQA: %02X%02X SAK: %02X",
                iso3_data.atqa[0],
                iso3_data.atqa[1],
                iso3_data.sak);
        }
    }

    if(bit_buffer_get_size_bytes(request->tx_buffer) > 0) {
        FURI_LOG_D(TAG, "Tx");
        if(request->append_crc) {
            FURI_LOG_D(TAG, "Tx CRC");
            iso14443_crc_append(Iso14443CrcTypeA, request->tx_buffer);
        }

        bit_buffer_reset(response->rx_buffer);

        Iso14443_3aError error = iso14443_3a_poller_txrx(
            poller, request->tx_buffer, response->rx_buffer, ISO14443_3A_FDT_LISTEN_FC);

        if(error == Iso14443_3aErrorNone) {
            FURI_LOG_D(TAG, "Tx OK");
        } else {
            FURI_LOG_D(TAG, "Tx Error");
        }
    }
    return request->keep_field ? NfcCommandContinue : NfcCommandStop;
}
