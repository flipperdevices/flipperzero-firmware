#include "tullave_apdu.h"

#define LOG_TAG "TuLlaveAPDU"
#define TULLAVE_NFC_DELAY_START 10
#define TULLAVE_NFC_TIMEOUT 300
#define TULLAVE_CARD_NUM_LEN 16
#define TULLAVE_CARD_BALANCE_FIRST_PART_LEN 4
#define TULLAVE_CARD_BALANCE_SECOND_PART_LEN 2
#define TULLAVE_CARD_BALANCE_LEN 6
#define TULLAVE_NUM_CARD_OFFSET 8

uint64_t uint8_to_integer_big_endian(const uint8_t* src, size_t length) {
    uint64_t result = 0;
    for(size_t i = 0; i < length; i++) {
        result |= ((uint64_t)src[i]) << (8 * (length - i - 1));
    }
    return result;
}

void uint8_to_hex_chars(const uint8_t* src, FuriString* target, int length) {
    const char chars[] = "0123456789ABCDEF";
    furi_string_reserve(target, length);
    while(--length >= 0) {
        furi_string_set_char(
            target, length, chars[(src[length >> 1] >> ((1 - (length & 1)) << 2)) & 0xF]);
    }
}

void
    tullave_apdu_send_command(const uint8_t* req_buffer, size_t req_size, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(req_buffer);

    FuriString* req_buffer_hex_debug = furi_string_alloc();
    FuriString* resp_buffer_hex_debug = NULL;

    do {
        FURI_LOG_D(LOG_TAG, "Send APDU command called");

        uint8_to_hex_chars(req_buffer, req_buffer_hex_debug, req_size * 2);
        FURI_LOG_D(LOG_TAG, "Command: %s", furi_string_get_cstr(req_buffer_hex_debug));

        memcpy(tx_rx->tx_data, req_buffer, req_size);
        tx_rx->tx_bits = req_size * 8;
        tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

        if(!furi_hal_nfc_tx_rx(tx_rx, TULLAVE_NFC_TIMEOUT)) {
            FURI_LOG_E(LOG_TAG, "Failed to tx_rx");
            break;
        }

        resp_buffer_hex_debug = furi_string_alloc();

        uint8_to_hex_chars(tx_rx->rx_data, resp_buffer_hex_debug, (tx_rx->rx_bits / 8) * 2);
        FURI_LOG_D(LOG_TAG, "Response: %s", furi_string_get_cstr(resp_buffer_hex_debug));

        furi_string_free(req_buffer_hex_debug);
        furi_string_free(resp_buffer_hex_debug);
    } while(false);
}

bool tullave_detect_card() {
    FuriHalNfcDevData dev_data = {};

    if(!furi_hal_nfc_detect(&dev_data, TULLAVE_NFC_TIMEOUT)) {
        FURI_LOG_D(LOG_TAG, "Tag was not detected");
        return false;
    }
    if(dev_data.type != FuriHalNfcTypeA && dev_data.type != FuriHalNfcTypeB) {
        FURI_LOG_D(LOG_TAG, "Tag should be Nfc-A or Nfc-B");
        return false;
    }
    return true;
}

bool tullave_select_app(TuLlaveInfo* card_info, FuriHalNfcTxRxContext* tx_rx) {
    
    //Debug command: nfc adpu 00a4040007d4100000030001
    const uint8_t select_req[] = {
        0x00, 0xa4, // SELECT the app
        0x04, 0x00, // P1:By name, P2:empty 
        0x07, // Lc: Data length 
        0xd4, 0x10, 0x00, 0x00, 0x03, 0x00, 0x01 // Application Id
    };

    tullave_apdu_send_command(select_req, sizeof(select_req), tx_rx);
    if(tx_rx->rx_bits == 0) {
        FURI_LOG_D(LOG_TAG, "The card does not respond correctly. Invalid response size.");
        return false;
    }
    FURI_LOG_D(LOG_TAG, "TuLlave was authenticated");

    //First set the card number to empty string.
    furi_string_set_str(card_info->card_number, "");
    uint8_to_hex_chars(
        &tx_rx->rx_data[TULLAVE_NUM_CARD_OFFSET], card_info->card_number, TULLAVE_CARD_NUM_LEN);

    FURI_LOG_D(LOG_TAG, "Card Number: %s", furi_string_get_cstr(card_info->card_number));

    return true;
}

void tullave_read_balance(TuLlaveInfo* card_info, FuriHalNfcTxRxContext* tx_rx) {
    uint8_t c_bal[TULLAVE_CARD_BALANCE_LEN] = {0x00};

    //Debug command: nfc adpu 904c000004. Send first select_req.
    const uint8_t bal_req[] = {
        0x90, 0x4c, 0x00, 0x00, 0x04 // Propietary command to get the balance.
    };

    tullave_apdu_send_command(bal_req, sizeof(bal_req), tx_rx);
    memcpy(c_bal, &tx_rx->rx_data[5], 2);
    memcpy(&c_bal[2], tx_rx->rx_data, 4);
    card_info->balance = uint8_to_integer_big_endian(c_bal, TULLAVE_CARD_BALANCE_LEN);

    FURI_LOG_D(
        LOG_TAG, "Card Balance: %lld.%02lld", card_info->balance, (card_info->balance % 100));
}

bool tullave_read_info(TuLlaveInfo* card_info) {
    FuriHalNfcTxRxContext tx_rx = {};
    bool success = false;
    do {
        FURI_LOG_D(LOG_TAG, "Requesting TuLlave Card Info");

        if(!tullave_detect_card()) {
            break;
        }

        if(!tullave_select_app(card_info, &tx_rx)) {
            break;
        }
        tullave_read_balance(card_info, &tx_rx);

        success = true;
        furi_hal_nfc_sleep();
        furi_delay_ms(100);
    } while(false);

    return success;
}
