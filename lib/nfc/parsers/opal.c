#include "nfc_supported_card.h"
#include <nfc_worker_i.h>
#include "furi_hal.h"
#include <time.h>
#include <locale/locale.h>

static const char* opal_modes_of_transport[] = {
    "Rail",
    "Ferry or light rail",
    "Bus",
};

static const char* opal_usage_types[] = {
    "None (new, unused card)",
    "New journey",
    "Transfer (from same mode)",
    "Transfer (from different mode)",
    "New journey (Manly Ferry)",
    "Transfer (from other ferry to Manly Ferry)",
    "Transfer (from other mode to Manly Ferry)",
    "Journey completed (distance fare)",
    "Journey completed (flat-rate fare)",
    "Journey completed (failure to tap off)",
    "Journey completed (failure to tap on)",
    "Tap on reversal",
    "Tap on rejected",
};

bool opal_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    UNUSED(tx_rx);
    MifareDesfireData* data = &nfc_worker->dev_data->mf_df_data;
    if(mf_df_find_application(data, 0x314553)) {
        FURI_LOG_I("opal", "Card verified");
        return true;
    }
    return false;
}

bool opal_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    UNUSED(tx_rx);
    FURI_LOG_I("opal", "Card read");
    return true;
}

bool opal_parser_parse(NfcDeviceData* dev_data) {
    UNUSED(dev_data);
    // Reference: https://github.com/metrodroid/metrodroid/wiki/Opal
    if(dev_data->protocol != NfcDeviceProtocolMifareDesfire) return false;
    MifareDesfireApplication* app = mf_df_find_application(&dev_data->mf_df_data, 0x314553);
    if(!app) return false;

    FuriString* tmp = furi_string_alloc();

    bool found_card = false;
    uint8_t* data;
    for(MifareDesfireFile* file = app->file_head; file; file = file->next) {
        data = file->contents;
        if(file->id == 7 && data) {
            found_card = true;
            break;
        }
    }
    if(!found_card || !data) return false;

    uint32_t serial_number = ((uint32_t)data[3] << 24) | ((uint32_t)data[2] << 16) |
                             ((uint32_t)data[1] << 8) | (uint32_t)data[0];
    uint8_t serial_number_check_digit = data[4] & 0x0F;
    bool card_blocked = data[4] & 0x10;
    uint16_t transaction_sequence_number = ((uint16_t)(data[6] & 0x1F) << 11) |
                                           ((uint16_t)data[5] << 3) | ((uint16_t)data[4] >> 5);
    int32_t balance_in_cents = ((uint16_t)(data[9] & 0x03) << 19) | ((int32_t)data[8] << 11) |
                               ((int32_t)data[7] << 3) | ((int32_t)data[6] >> 5);
    uint16_t last_tap_date_since_epoch = ((uint16_t)(data[11] & 0x01) << 14) |
                                         ((uint16_t)data[10] << 6) | ((uint16_t)data[9] >> 2);
    uint16_t last_tap_time_minutes = ((uint16_t)(data[12] & 0x0F) << 7) |
                                     ((uint16_t)data[11] >> 1);
    uint8_t last_tap_mode_of_transport = (data[12] >> 4) & 0x07;
    uint8_t last_tap_usage_type = (data[13] & 0x07) << 1 | (data[12] >> 7);
    bool autotopup_enabled = (data[13] >> 3) & 0x01;
    uint8_t weekly_paid_journey_count = data[13] >> 4;

    // Ignore CRC verification
    // uint16_t crc = ((uint16_t)data[15] << 8) | ((uint16_t)data[14]);

    furi_string_set(dev_data->parsed_data, "\e#Opal\n");
    // Stylise card number according to the physical card
    char serial_string[11];
    snprintf(serial_string, 11, "%09lu", serial_number);
    furi_string_cat_printf(
        dev_data->parsed_data,
        "3085 22%c%c %c%c%c%c %c%c%c%u\n",
        serial_string[0],
        serial_string[1],
        serial_string[2],
        serial_string[3],
        serial_string[4],
        serial_string[5],
        serial_string[6],
        serial_string[7],
        serial_string[8],
        serial_number_check_digit);

    if(card_blocked) {
        furi_string_cat(dev_data->parsed_data, "Card blocked\n");
    }

    furi_string_cat_printf(
        dev_data->parsed_data,
        "Balance: $%ld.%02d",
        balance_in_cents / 100,
        abs(balance_in_cents) % 100);
    if(autotopup_enabled) {
        furi_string_cat(dev_data->parsed_data, ", auto top-up");
    }
    furi_string_cat(dev_data->parsed_data, "\n");

    furi_string_cat_printf(
        dev_data->parsed_data,
        "Rides: %u weekly, %u total\n",
        weekly_paid_journey_count,
        transaction_sequence_number);

    furi_string_cat(dev_data->parsed_data, "Last trip: ");
    if(last_tap_mode_of_transport < 3) {
        furi_string_cat_printf(
            dev_data->parsed_data, "%s, ", opal_modes_of_transport[last_tap_mode_of_transport]);
    }

    if(last_tap_usage_type < 13) {
        furi_string_cat_printf(
            dev_data->parsed_data, "%s ", opal_usage_types[last_tap_usage_type]);
    }

    // Calculate timestamp since 1 January 1980 epoch
    // Stored as Australia/Sydney local time
    struct tm last_tap = {0};
    last_tap.tm_year = 80;
    last_tap.tm_mon = 0;
    last_tap.tm_mday = last_tap_date_since_epoch + 1;
    last_tap.tm_min = last_tap_time_minutes;
    mktime(&last_tap);

    // Convert last_tap to furi format and print
    FuriHalRtcDateTime last_tap_furi = {
        .year = last_tap.tm_year + 1900,
        .month = last_tap.tm_mon + 1,
        .day = last_tap.tm_mday,
        .hour = last_tap.tm_hour,
        .minute = last_tap.tm_min,
    };
    furi_string_cat(dev_data->parsed_data, "at ");
    locale_format_date(tmp, &last_tap_furi, locale_get_date_format(), "/");
    furi_string_cat(dev_data->parsed_data, tmp);
    furi_string_cat(dev_data->parsed_data, " ");
    locale_format_time(tmp, &last_tap_furi, locale_get_time_format(), false);
    furi_string_cat(dev_data->parsed_data, tmp);
    furi_string_cat(dev_data->parsed_data, "\n");

    furi_string_free(tmp);
    return true;
}
