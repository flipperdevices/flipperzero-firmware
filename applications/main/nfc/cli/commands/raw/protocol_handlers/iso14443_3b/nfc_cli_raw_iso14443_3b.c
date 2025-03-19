#include "nfc_cli_raw_iso14443_3b.h"

#include <nfc/helpers/iso14443_crc.h>
#include <nfc/protocols/iso14443_3b/iso14443_3b_i.h>
#include <nfc/protocols/iso14443_3b/iso14443_3b_poller_i.h>

#define TAG "ISO14B"

static void iso14443_3b_format_activation_data(const Iso14443_3bData* data, FuriString* output) {
    furi_string_printf(output, "UID:");
    for(size_t i = 0; i < ISO14443_3B_UID_SIZE; i++) {
        furi_string_cat_printf(output, " %02X", data->uid[i]);
    }

    const Iso14443_3bProtocolInfo* info = &data->protocol_info;
    furi_string_cat_printf(
        output,
        " BitRate: %d, Protocol: %d, Max Frame Size: %d, Fo: %d, Adc: %d, Fwi: %d",
        info->bit_rate_capability,
        info->protocol_type,
        info->max_frame_size,
        info->fo,
        info->adc,
        info->fwi);
}

NfcCommand nfc_cli_raw_iso14443_3b_handler(
    NfcGenericInstance* poller,
    const NfcCliRawRequest* request,
    NfcCliRawResponse* const response) {
    Iso14443_3bData iso3b_data = {0};
    Iso14443_3bPoller* iso14b_poller = poller;

    bool activated = false;
    if(request->select) {
        FURI_LOG_D(TAG, "Activating...");
        Iso14443_3bError error = iso14443_3b_poller_activate(iso14b_poller, &iso3b_data);
        if(error == Iso14443_3bErrorNone) {
            FURI_LOG_D(TAG, "Activate OK");
            activated = true;
            iso14443_3b_format_activation_data(&iso3b_data, response->activation_string);
        }
    }

    uint32_t timeout = ISO14443_3B_FDT_POLL_FC;
    //if(request->timeout > 0)
    //{ timeout = request->timeout; }
    //else
    if(activated) {
        timeout = iso14443_3b_get_fwt_fc_max(&iso3b_data);
    }

    if(bit_buffer_get_size_bytes(request->tx_buffer) > 0) {
        FURI_LOG_D(TAG, "Tx");
        if(request->append_crc) {
            FURI_LOG_D(TAG, "Tx CRC");
            iso14443_crc_append(Iso14443CrcTypeB, request->tx_buffer);
        }

        bit_buffer_reset(response->rx_buffer);
        NfcError error =
            nfc_poller_trx(iso14b_poller->nfc, request->tx_buffer, response->rx_buffer, timeout);

        if(error == NfcErrorNone) {
            FURI_LOG_D(TAG, "Tx OK");
        } else {
            FURI_LOG_D(TAG, "Tx Error");
        }
    }
    return request->keep_field ? NfcCommandContinue : NfcCommandStop;
}
