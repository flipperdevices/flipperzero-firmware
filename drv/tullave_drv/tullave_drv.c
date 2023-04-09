#include "tullave_drv.h"

#include <lib/toolbox/hex.h>
#include <furi.h>
#include <furi_hal_nfc.h>

#define LOG_TAG "TuLlaveCONfcDrv"
#define TULLAVE_NFC_TIMEOUT 300
// The size of initial response is fixed, we can use it to check if card is responding correctly.
#define TULLAVE_AUTH_RESP_SIZE 53
#define TULLAVE_DETECTED_RESP_FIRST_BYTE 0x6F
#define TULLAVE_DETECTED_RESP_SECOND_BYTE 0x31
#define TULLAVE_DETECTED_RESP_BUFFER_SIZE 2
#define TULLAVE_CARD_NUM_BYTES 16
#define TULLAVE_NUM_CARD_OFFSET 8

void uint8_to_hex_chars(const uint8_t* src, uint8_t* target, int length) {
    const char chars[] = "0123456789ABCDEF";
    while(--length >= 0) {
        target[length] = chars[(src[length >> 1] >> ((1 - (length & 1)) << 2)) & 0xF];
    }
}

void tullave_drv_exit_sleep() {
    furi_hal_nfc_exit_sleep();
}
void tullave_drv_start_sleep() {
    furi_hal_nfc_start_sleep();
}

size_t tullave_drv_send_apdu_command(
    const uint8_t* req_buffer,
    size_t req_size,
    uint8_t** resp_buffer) {
    furi_assert(req_buffer);

    size_t resp_buffer_size = 0;

    FuriHalNfcTxRxContext tx_rx = {};
    FuriHalNfcDevData dev_data = {};

    uint8_t* req_buffer_hex_debug = malloc(req_size * 2);
    uint8_t* resp_buffer_hex_debug = NULL;

    do {
        FURI_LOG_D(LOG_TAG, "Send APDU command called");

        if(!furi_hal_nfc_detect(&dev_data, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_D(LOG_TAG, "TuLlave card was not detected");
            break;
        }
        FURI_LOG_D(LOG_TAG, "NFC-A Card detected");

        uint8_to_hex_chars(req_buffer, req_buffer_hex_debug, req_size * 2);
        req_buffer_hex_debug[req_size * 2] = 0x00; // Null Delimited string
        FURI_LOG_D(LOG_TAG, "APDU Command: %s", req_buffer_hex_debug);

        memcpy(tx_rx.tx_data, req_buffer, req_size);
        tx_rx.tx_bits = req_size * 8;
        tx_rx.tx_rx_type = FuriHalNfcTxRxTypeDefault;

        if(!furi_hal_nfc_tx_rx(&tx_rx, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_E(LOG_TAG, "Failed to tx_rx");
            break;
        }

        resp_buffer_size = tx_rx.rx_bits / 8;
        *resp_buffer = malloc(resp_buffer_size);
        resp_buffer_hex_debug = malloc(resp_buffer_size * 2);

        uint8_to_hex_chars(tx_rx.rx_data, resp_buffer_hex_debug, resp_buffer_size * 2);
        resp_buffer_hex_debug[resp_buffer_size * 2] = 0x00; // Null Delimited string
        FURI_LOG_D(LOG_TAG, "APDU Response: %s", resp_buffer_hex_debug);

        memcpy(*resp_buffer, tx_rx.rx_data, resp_buffer_size);

        free(resp_buffer_hex_debug);
        free(req_buffer_hex_debug);
        resp_buffer_hex_debug = NULL;
        req_buffer_hex_debug = NULL;

    } while(false);
    return resp_buffer_size;
}

bool tullave_drv_req_card_info(TuLlaveInfo* card_info) {
    //Debug command: nfc adpu 00a4040007d4100000030001
    const uint8_t auth_req[] = {
        0x00, 0xa4, 0x04, 0x00, 0x07, 0xd4, 0x10, 0x00, 0x00, 0x03, 0x00, 0x01};

    //Debug command: nfc adpu 904c000004. Send the first command before this.
    //const uint8_t bal_req[] = {0x90, 0x4c, 0x00, 0x00, 0x04};
    //UNUSED(bal_req);
    size_t req_size = sizeof(auth_req) / sizeof(uint8_t);

    uint8_t* response = NULL;
    uint8_t resp_size = 0;
    bool success = false;
    UNUSED(card_info);

    do {
        FURI_LOG_D(LOG_TAG, "Requesting TuLlave Card Info");

        resp_size = tullave_drv_send_apdu_command(auth_req, req_size, &response);

        FURI_LOG_D(LOG_TAG, "Response Size was: %i", resp_size);
        if(resp_size <= 0) {
            FURI_LOG_D(LOG_TAG, "The card does not respond correctly. Invalid response size.");
            break;
        }
        FURI_LOG_D(LOG_TAG, "TuLlave was authenticated");

        card_info->card_number = malloc(TULLAVE_CARD_NUM_BYTES);
        uint8_to_hex_chars(
            &response[TULLAVE_NUM_CARD_OFFSET], card_info->card_number, TULLAVE_CARD_NUM_BYTES);
        card_info->card_number[TULLAVE_CARD_NUM_BYTES] = 0x00; // Null-terminated string
        FURI_LOG_D(LOG_TAG, "Card Number: %s", card_info->card_number);

        //Check the balance
        /*
        req_size = sizeof(bal_req) / sizeof(uint8_t);
        tx_rx.tx_bits = req_size;
        memcpy(tx_rx.tx_data, bal_req, req_size);

        tullave_drv_send_apdu_command(&tx_rx);

        FURI_LOG_D(LOG_TAG, "Card Balance: %i", card_info->balance);
        */
        free(response);
    } while(false);

    return success;
}
