#include "emrtd.h"

#include <core/common_defines.h>

#define TAG "EMRTD"

static bool id_decode_response(uint8_t* buff, uint16_t len) {
    if(len < 2) {
        return false;
    }
    bool success = false;
    if(buff[0] == 0x69 && buff[1] == 0x82) { // Security status not satisfied
        success = true;
        // Not sure about this but in theory this should only be the
        // answer if the LDS1 app is indeed found
    }
    if(buff[0] == 0x90 && buff[1] == 0x00) { // Normal Processing
        success = true;
        // App is found
    }
    return success;
}

bool id_select_app(FuriHalNfcTxRxContext* tx_rx, EmrtdApplication* app) {
    bool select_app_success = false;
    const uint8_t id_select_header[] = {
        0x00, // CLA
        0xA4, // SELECT application
        0x04, // P1
        0x0C // P2
    }; // Information taken from Doc 9303
    uint16_t size = sizeof(id_select_header);

    // Copy header
    memcpy(tx_rx->tx_data, id_select_header, size);
    // Copy AID
    // tx_rx->tx_data[size++] = app->aid_len;
    memcpy(&tx_rx->tx_data[size], app->aid, app->aid_len);
    size += app->aid_len;
    tx_rx->tx_data[size++] = 0x00;
    tx_rx->tx_bits = size * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    FURI_LOG_D(TAG, "Select the LDS1 application");
    if(furi_hal_nfc_tx_rx(tx_rx, 800)) {
        if(id_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8)) {
            select_app_success = true;
        } else {
            FURI_LOG_E(TAG, "Failed to find LDS1 app");
        }
    } else {
        FURI_LOG_E(TAG, "Failed to start application");
    }

    return select_app_success;
}

bool read_id_card(FuriHalNfcTxRxContext* tx_rx, EmrtdApplication* emrtd_app) {
    furi_assert(tx_rx);
    furi_assert(emrtd_app);
    bool lds1_present = false;
    do {
        if(!id_select_app(tx_rx, emrtd_app)) break;
        lds1_present = true;
    } while(false);

    return lds1_present;
}
