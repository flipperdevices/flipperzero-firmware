#include "tullave_drv.h"

#include <furi.h>
#include <furi_hal_nfc.h>

#define LOG_TAG "TuLlaveCONfcDrv"
#define TULLAVE_NFC_TIMEOUT 300
#define TULLAVE_DETECTED_RESP_FIRS_BYTE 0x6F
#define TULLAVE_DETECTED_RESP_SECOND_BYTE 0x31

#define TULLAVE_DETECTED_RESP_BUFFER_SIZE 2

void uint8_to_hex_chars(const uint8_t* src, uint8_t* target, int length) {
    const char chars[] = "0123456789ABCDEF";
    while(--length >= 0)
        target[length] = chars[(src[length >> 1] >> ((1 - (length & 1)) << 2)) & 0xF];
}

void tullave_drv_send_apdu_command(
    uint8_t* req_buffer,
    size_t req_size,
    uint8_t* resp_buffer,
    size_t resp_size) {
    furi_assert(req_buffer);
    furi_assert(resp_buffer);

    if(furi_hal_nfc_is_busy()) {
        FURI_LOG_E(LOG_TAG, "NFC is busy");
        return;
    }

    FuriHalNfcTxRxContext tx_rx = {};
    FuriHalNfcDevData dev_data = {};

    uint8_t* req_buffer_hex_debug = malloc(req_size * 2);
    uint8_t* resp_buffer_hex_debug = malloc(resp_size * 2);

    furi_hal_nfc_exit_sleep();

    do {
        FURI_LOG_D(LOG_TAG, "Send APDU command called");

        if(!furi_hal_nfc_detect(&dev_data, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_D(LOG_TAG, "TuLlave card was not detected");
            break;
        }
        FURI_LOG_D(LOG_TAG, "NFC-A Card detected");

        uint8_to_hex_chars(req_buffer, req_buffer_hex_debug, req_size * 2);
        req_buffer_hex_debug[req_size * 2] = 0x00; // Null Delimited string
        FURI_LOG_D(LOG_TAG, "Sending TuLlave APDU command: %s", req_buffer_hex_debug);

        memcpy(tx_rx.tx_data, req_buffer, req_size);
        tx_rx.tx_bits = req_size * 8;
        tx_rx.tx_rx_type = FuriHalNfcTxRxTypeDefault;

        if(!furi_hal_nfc_tx_rx(&tx_rx, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_E(LOG_TAG, "Failed to tx_rx");
            break;
        }

        uint8_to_hex_chars(tx_rx.rx_data, resp_buffer_hex_debug, resp_size * 2);
        resp_buffer_hex_debug[req_size * 2] = 0x00; // Null Delimited string
        FURI_LOG_D(LOG_TAG, "Response to Detect APDU: %s", resp_buffer_hex_debug);

        memcpy(resp_buffer, tx_rx.rx_data, resp_size);

        free(resp_buffer_hex_debug);
        free(req_buffer_hex_debug);
        resp_buffer_hex_debug = NULL;
        req_buffer_hex_debug = NULL;

    } while(false);

    // APDU command response is stateless, therefore ever close the nfc.
    furi_hal_nfc_sleep();
}

bool tullave_drv_detect_tullave_card() {
    //You can Debug this through console, using this command: nfc adpu 00a4040007d4100000030001
    size_t detect_apdu_cc_size = 12;

    uint8_t detect_apdu_cc[] = {
        0x00, 0xa4, 0x04, 0x00, 0x07, 0xd4, 0x10, 0x00, 0x00, 0x03, 0x00, 0x01};

    uint8_t resp_apdu_command[TULLAVE_DETECTED_RESP_BUFFER_SIZE] = {0x00};

    bool detected = false;

    FURI_LOG_D(LOG_TAG, "Trying to detect TuLlave Card");

    tullave_drv_send_apdu_command(
        detect_apdu_cc, detect_apdu_cc_size, resp_apdu_command, TULLAVE_DETECTED_RESP_BUFFER_SIZE);

    if(resp_apdu_command != NULL) {
        detected = resp_apdu_command[0] == TULLAVE_DETECTED_RESP_FIRS_BYTE;
        detected &= resp_apdu_command[1] == TULLAVE_DETECTED_RESP_SECOND_BYTE;
    }

    FURI_LOG_I(LOG_TAG, "TuLlave card Detected Result: %i", detected);
    return detected;
}