#include "tullave_apdu.h"

#define LOG_TAG "TuLlaveAPDU"
#define TULLAVE_NFC_DELAY_START 10
#define TULLAVE_NFC_TIMEOUT 300
#define TULLAVE_CARD_NUM_LEN 16
#define TULLAVE_CARD_BALANCE_FIRST_PART_LEN 4
#define TULLAVE_CARD_BALANCE_SECOND_PART_LEN 2
#define TULLAVE_CARD_BALANCE_LEN 6
#define TULLAVE_NUM_CARD_OFFSET 8

void uint8_to_hex_chars(const uint8_t* src, uint8_t* target, int length) {
    const char chars[] = "0123456789ABCDEF";
    while(--length >= 0)
        target[length] = chars[(src[length >> 1] >> ((1 - (length & 1)) << 2)) & 0xF];
}

void uint8_to_furi_string(const uint8_t* src, FuriString* target, size_t length) {
    for(size_t i = 0; i < length; i++) {
        furi_string_cat_printf(target, "%c", src[i]);
    }
}

uint64_t uint8_to_integer_big_endian(const uint8_t* src, size_t length) {
    uint64_t result = 0;
    for(size_t i = 0; i < length; i++) {
        result |= ((uint64_t)src[i]) << (8 * (length - i - 1));
    }
    return result;
}

size_t
    tullave_apdu_send_command(const uint8_t* req_buffer, size_t req_size, uint8_t** resp_buffer) {
    furi_assert(req_buffer);

    size_t resp_buffer_size = 0;

    FuriHalNfcTxRxContext tx_rx = {};

    uint8_t* req_buffer_hex_debug = malloc(req_size * 2);
    uint8_t* resp_buffer_hex_debug = NULL;

    do {
        FURI_LOG_D(LOG_TAG, "Send APDU command called");

        uint8_to_hex_chars(req_buffer, req_buffer_hex_debug, req_size * 2);
        req_buffer_hex_debug[req_size * 2] = 0x00; // Null Delimited string
        FURI_LOG_D(LOG_TAG, "Command: %s", req_buffer_hex_debug);

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
        FURI_LOG_D(LOG_TAG, "Response: %s", resp_buffer_hex_debug);

        memcpy(*resp_buffer, tx_rx.rx_data, resp_buffer_size);

        free(resp_buffer_hex_debug);
        free(req_buffer_hex_debug);
        resp_buffer_hex_debug = NULL;
        req_buffer_hex_debug = NULL;

    } while(false);
    return resp_buffer_size;
}

bool tullave_apdu_read(TuLlaveInfo* card_info) {
    uint8_t* response = NULL;
    uint8_t c_info[TULLAVE_CARD_NUM_LEN] = {0X00};
    uint8_t c_bal[TULLAVE_CARD_BALANCE_LEN] = {0x00};

    uint8_t resp_size = 0;
    bool success = false;
    FuriHalNfcDevData dev_data = {};

    do {
        FURI_LOG_D(LOG_TAG, "Requesting TuLlave Card Info");

        if(!furi_hal_nfc_detect(&dev_data, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_D(LOG_TAG, "Tag was not detected");
            break;
        }
        FURI_LOG_D(LOG_TAG, "Tag detected");

        resp_size = tullave_apdu_send_command(auth_req, sizeof(auth_req), &response);

        FURI_LOG_D(LOG_TAG, "Response Size was: %i", resp_size);
        if(resp_size <= 0) {
            FURI_LOG_D(LOG_TAG, "The card does not respond correctly. Invalid response size.");
            break;
        }
        FURI_LOG_D(LOG_TAG, "TuLlave was authenticated");

        uint8_to_hex_chars(&response[TULLAVE_NUM_CARD_OFFSET], c_info, TULLAVE_CARD_NUM_LEN);
        c_info[TULLAVE_CARD_NUM_LEN] = 0x00;

        //Set first the card number to empty string.
        furi_string_set_str(card_info->card_number, "");
        uint8_to_furi_string(c_info, card_info->card_number, sizeof(c_info));
        FURI_LOG_D(LOG_TAG, "Card Number: %s", furi_string_get_cstr(card_info->card_number));

        free(response);
        response = NULL;

        //Check the balance
        tullave_apdu_send_command(bal_req, sizeof(bal_req), &response);

        memcpy(c_bal, &response[5], 2);
        memcpy(&c_bal[2], response, 4);

        card_info->balance = (uint8_to_integer_big_endian(c_bal, TULLAVE_CARD_BALANCE_LEN) / 100);

        FURI_LOG_D(LOG_TAG, "Card Balance: %.2f", card_info->balance);

        free(response);

        success = true;

        furi_hal_nfc_sleep();
        furi_delay_ms(100);
    } while(false);

    return success;
}
