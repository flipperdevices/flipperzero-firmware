#include "tullave_nfc_drv.h"
#include <furi_hal_nfc.h>

#define LOG_TAG "TuLlaveCONfcDrv"
#define TULLAVE_NFC_TIMEOUT 300
#define TULLAVE_START_RESP_FIRS_BYTE 0x6F
#define TULLAVE_START_RESP_SECOND_BYTE 0x31

void end_tullave_nfc() {
    furi_hal_nfc_ll_txrx_off();
    furi_hal_nfc_sleep();
}

bool detect_tullave_card() {
    bool tullave_activated = false;
    FuriHalNfcTxRxContext tx_rx = {};
    FuriHalNfcDevData dev_data = {};

    //You can Debug with nfc adpu 00a4040007d4100000030001
    const uint8_t req_buffer[] = {
        0x00, 0xa4, 0x04, 0x00, 0x07, 0xd4, 0x10, 0x00, 0x00, 0x03, 0x00, 0x01};
    uint8_t* resp_buffer = NULL;

    size_t req_size = sizeof(req_buffer) / sizeof(uint8_t);
    size_t resp_size = 0;

    FURI_LOG_D(LOG_TAG, "Initializing Nfc Poller");
    do {
        if(!furi_hal_nfc_detect(&dev_data, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_D(LOG_TAG, "TuLlave Card was not detected");
            break;
        }

        memcpy(tx_rx.tx_data, req_buffer, req_size);
        tx_rx.tx_bits = req_size * 8;
        tx_rx.tx_rx_type = FuriHalNfcTxRxTypeDefault;

        FURI_LOG_D(LOG_TAG, "Sending TuLlave Detect APDU");
        if(!furi_hal_nfc_tx_rx(&tx_rx, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_E(LOG_TAG, "Failed to tx_rx");
            break;
        }

        resp_size = (tx_rx.rx_bits / 8) * 2;
        resp_buffer = malloc(resp_size);
        uint8_to_hex_chars(tx_rx.rx_data, resp_buffer, resp_size);
        resp_buffer[resp_size] = 0x00; // Null Delimited string

        FURI_LOG_D(LOG_TAG, "Response to Detect APDU: %s", resp_buffer);

        tullave_activated = tx_rx.rx_data[0] == TULLAVE_START_RESP_FIRS_BYTE;
        tullave_activated &= tx_rx.rx_data[1] == TULLAVE_START_RESP_SECOND_BYTE;

        free(resp_buffer);
        resp_buffer = NULL;
    } while(false);

    if(!tullave_activated) {
        end_tullave_nfc();
    }
    return tullave_activated;
}