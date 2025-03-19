#include "nfc_cli_raw_felica.h"

#include <nfc/helpers/felica_crc.h>
#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/felica/felica_poller_i.h>

#define TAG "FELICA"

static void felica_format_array(
    const uint8_t* data,
    size_t data_size,
    const char* header,
    FuriString* output) {
    furi_string_cat_printf(output, "%s", header);
    for(size_t i = 0; i < data_size; i++)
        furi_string_cat_printf(output, "%02X ", data[i]);
}

static inline void felica_format_activation_data(const FelicaData* data, FuriString* output) {
    felica_format_array(data->idm.data, FELICA_IDM_SIZE, "IDm: ", output);
    felica_format_array(data->pmm.data, FELICA_PMM_SIZE, " PMm: ", output);
}

NfcCommand nfc_cli_raw_felica_handler(
    NfcGenericInstance* poller,
    const NfcCliRawRequest* request,
    NfcCliRawResponse* const response) {
    FelicaPoller* felica_poller = poller;

    if(request->select) {
        FelicaData felica_data = {};
        FURI_LOG_D(TAG, "Activating...");

        FelicaError error = felica_poller_activate(felica_poller, &felica_data);
        if(error == FelicaErrorNone) {
            FURI_LOG_D(TAG, "Activate OK");
            felica_format_activation_data(&felica_data, response->activation_string);
        }
    }

    if(bit_buffer_get_size_bytes(request->tx_buffer) > 0) {
        FURI_LOG_D(TAG, "Tx");
        if(request->append_crc) {
            FURI_LOG_D(TAG, "Tx CRC");
            felica_crc_append(request->tx_buffer);
        }

        bit_buffer_reset(response->rx_buffer);
        NfcError error = nfc_poller_trx(
            felica_poller->nfc, request->tx_buffer, response->rx_buffer, FELICA_FDT_POLL_FC);

        if(error == NfcErrorNone) {
            FURI_LOG_D(TAG, "Tx OK");
        } else {
            FURI_LOG_D(TAG, "Tx Error");
        }
    }
    return request->keep_field ? NfcCommandContinue : NfcCommandStop;
}
