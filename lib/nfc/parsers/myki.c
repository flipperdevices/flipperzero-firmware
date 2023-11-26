#include "nfc_supported_card.h"

#include <nfc_worker_i.h>

#include "furi_hal.h"

static const uint8_t myki_aid[3] = {0x00, 0x11, 0xf2};

bool myki_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    UNUSED(tx_rx);
    MifareDesfireData* data = &nfc_worker->dev_data->mf_df_data;

    MifareDesfireApplication* app = mf_df_get_application(data, &myki_aid);
    if(app == NULL) {
        return false;
    } else {
        FURI_LOG_I("Myki", "Card Verified.");
        return true;
    }
}

bool myki_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    UNUSED(tx_rx);
    FURI_LOG_I("Myki", "Card Read");
    return true;
}

uint8_t myki_calculate_luhn(uint64_t number) {
    // https://en.wikipedia.org/wiki/Luhn_algorithm
    // Drop existing check digit to form payload
    uint64_t payload = number / 10;
    int sum = 0;
    int position = 0;

    while(payload > 0) {
        int digit = payload % 10;
        if(position % 2 == 0) {
            digit *= 2;
        }
        if(digit > 9) {
            digit = (digit / 10) + (digit % 10);
        }
        sum += digit;
        payload /= 10;
        position++;
    }

    return (10 - (sum % 10)) % 10;
}

bool myki_parser_parse(NfcDeviceData* dev_data) {
    // Reference: https://github.com/metrodroid/metrodroid/wiki/Myki
    if(dev_data->protocol != NfcDeviceProtocolMifareDesfire) return false;
    MifareDesfireApplication* app = mf_df_get_application(&dev_data->mf_df_data, &myki_aid);
    if(!app) return false;

    uint64_t card_number = 0;
    bool found_card = false;
    for(MifareDesfireFile* file = app->file_head; file; file = file->next) {
        uint8_t* data = file->contents;
        if(file->id == 15 && data) {
            uint64_t top = ((uint64_t)data[3] << 24) | ((uint64_t)data[2] << 16) |
                           ((uint64_t)data[1] << 8) | (uint64_t)data[0];
            uint64_t bottom = ((uint64_t)data[7] << 24) | ((uint64_t)data[6] << 16) |
                              ((uint64_t)data[5] << 8) | (uint64_t)data[4];

            // All myki card numbers are prefixed with "308425"
            if(top != 308425) return false;
            // Card numbers are always 15 digits in length
            if(bottom < 10000000 || bottom >= 100000000) return false;

            card_number = (top * 1000000000) + (bottom * 10);
            // Stored card number doesn't include check digit
            card_number = card_number + myki_calculate_luhn(card_number);
            found_card = true;
            break;
        }
    }
    if(!found_card) return false;

    // Stylise card number according to the physical card
    char card_string[20];
    snprintf(card_string, 20, "%llu", card_number);
    furi_string_printf(
        dev_data->parsed_data,
        "\e#Myki (PTV)\nCard: %c %c%c%c%c%c %c%c%c%c %c%c%c%c %c\n",
        card_string[0],
        card_string[1],
        card_string[2],
        card_string[3],
        card_string[4],
        card_string[5],
        card_string[6],
        card_string[7],
        card_string[8],
        card_string[9],
        card_string[10],
        card_string[11],
        card_string[12],
        card_string[13],
        card_string[14]);

    return true;
}