/*
 * clipper.c - Parser for Clipper cards (San Francisco, California).
 *
 * Based on research, some of which dates to 2007!
 *
 * Copyright 2024 Jeremy Cooper <jeremy.gthb@baymoo.org>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nfc_supported_card_plugin.h"

#include <flipper_application/flipper_application.h>
#include <lib/nfc/protocols/mf_desfire/mf_desfire.h>
#include <lib/nfc/helpers/nfc_util.h>
#include <applications/services/locale/locale.h>
#include <furi_hal_rtc.h>
#include <inttypes.h>

//
// Table of application ids observed in the wild, and their sources.
//
static const struct {
    const MfDesfireApplicationId app;
    const char* type;
} clipper_types[] = {
    // Application advertised on classic, plastic cards.
    {.app = {.data = {0x90, 0x11, 0xf2}}, .type = "Card"},
    // Application advertised on a mobile device.
    {.app = {.data = {0x91, 0x11, 0xf2}}, .type = "Mobile Device"},
};
static const size_t kNumCardTypes = sizeof(clipper_types) / sizeof(clipper_types[0]);

static const MfDesfireFileId clipper_ecash_file_id = 2;
static const MfDesfireFileId clipper_identity_file_id = 8;

static void epoch_1900_datetime_to_furi(uint32_t seconds, FuriHalRtcDateTime* out);

// Unmarshal a 32-bit integer, big endian, unsigned
static inline uint32_t get_u32be(const uint8_t* field) {
    return nfc_util_bytes2num(field, 4);
}

// Unmarshal a 16-bit integer, big endian, unsigned
static uint16_t get_u16be(const uint8_t* field) {
    return nfc_util_bytes2num(field, 2);
}

// Unmarshal a 16-bit integer, big endian, signed, two's-complement
static int16_t get_i16be(const uint8_t* field) {
    uint16_t raw = get_u16be(field);
    if(raw > 0x7fff)
        return -((uint32_t)0x10000 - raw);
    else
        return raw;
}

static bool clipper_parse(const NfcDevice* device, FuriString* parsed_data) {
    furi_assert(device);
    furi_assert(parsed_data);

    bool parsed = false;

    do {
        const MfDesfireData* data = nfc_device_get_data(device, NfcProtocolMfDesfire);

        const MfDesfireApplication* app = NULL;
        const char* device_description = NULL;

        for(size_t i = 0; i < kNumCardTypes; i++) {
            app = mf_desfire_get_application(data, &clipper_types[i].app);
            device_description = clipper_types[i].type;
            if(app != NULL) break;
        }

        // If no matching application was found, abort this parser.
        if(app == NULL) break;

        // Identity file (8)
        //
        // Byte view
        //
        //       0    1    2    3    4    5    6    7    8
        //       +----+----.----.----.----+----.----.----+
        // 0x00  | uk | card_id           | unknown      |
        //       +----+----.----.----.----+----.----.----+
        // 0x08  | unknown                               |
        //       +----.----.----.----.----.----.----.----+
        // 0x10    ...
        //
        //
        // Field          Datatype   Description
        // -----          --------   -----------
        // uk             ?8??       Unknown, 8-bit byte
        // card_id        U32BE      Card identifier
        //
        const MfDesfireFileSettings* id_file_settings =
            mf_desfire_get_file_settings(app, &clipper_identity_file_id);

        if(id_file_settings == NULL || id_file_settings->type != MfDesfireFileTypeStandard ||
           id_file_settings->data.size < 5)
            break;

        const MfDesfireFileData* id_file_data =
            mf_desfire_get_file_data(app, &clipper_identity_file_id);
        if(id_file_data == NULL) break;

        const uint8_t* id_contents = simple_array_cget_data(id_file_data->data);

        uint32_t card_id = nfc_util_bytes2num(&id_contents[1], 4);

        // ECash file (2)
        //
        // Byte view
        //
        //       0    1    2    3    4    5    6    7    8
        //       +----.----+----.----+----.----.----.----+
        // 0x00  |  unk00  | counter | timestamp_1900    |
        //       +----.----+----.----+----.----.----.----+
        // 0x08  | term_id |     unk01                   |
        //       +----.----+----.----+----.----.----.----+
        // 0x10  | txn_id  | balance |      unknown      |
        //       +----.----+----.----+----.----.----.----+
        // 0x18  |               unknown                 |
        //       +---------------------------------------+
        //
        // Field          Datatype Description
        // -----          -------- -----------
        // unk00          U8[2]     Unknown bytes
        // counter        U16BE     Unknown, appears to be a counter
        // timestamp_1900 U32BE     Timestamp of last transaction, in seconds
        //                          since 1900-01-01 GMT.
        // unk01          U8[6]     Unknown bytes
        // txn_id         U16BE     Id of last transaction.
        // balance        S16BE     Card cash balance, in cents.
        //                          Cards can obtain negative balances in this
        //                          system, so balances are signed integers.
        //                          Maximum card balance is therefore
        //                          $327.67.
        // unk02          U8[12]    Unknown bytes.
        //
        const MfDesfireFileSettings* cash_file_settings =
            mf_desfire_get_file_settings(app, &clipper_ecash_file_id);

        if(cash_file_settings == NULL || cash_file_settings->type != MfDesfireFileTypeBackup ||
           cash_file_settings->data.size < 32)
            break;

        const MfDesfireFileData* ec_file_data =
            mf_desfire_get_file_data(app, &clipper_ecash_file_id);
        if(ec_file_data == NULL) break;

        const uint8_t* ec_contents = simple_array_cget_data(ec_file_data->data);

        uint16_t counter = get_u16be(&ec_contents[2]);
        uint32_t timestamp_1900 = get_u32be(&ec_contents[4]);
        uint16_t terminal_id = get_u16be(&ec_contents[8]);
        uint16_t txn_id = get_u16be(&ec_contents[0x10]);
        int16_t balance_in_cents = get_i16be(&ec_contents[0x12]);

        int16_t balance_usd = balance_in_cents / 100;
        uint16_t balance_cents;

        if(balance_in_cents >= 0)
            balance_cents = balance_in_cents % 100;
        else
            balance_cents = (balance_in_cents * -1) % 100;

        FuriHalRtcDateTime timestamp;
        epoch_1900_datetime_to_furi(timestamp_1900, &timestamp);

        FuriString* date_str = furi_string_alloc();
        locale_format_date(date_str, &timestamp, locale_get_date_format(), "-");

        FuriString* time_str = furi_string_alloc();
        locale_format_time(time_str, &timestamp, locale_get_time_format(), true);

        furi_string_cat_printf(
            parsed_data,
            "\e#Clipper\n"
            "Serial: %" PRIu32 "\n"
            "Balance: $%d.%02u\n"
            "Type: %s\n"
            "\e#Last Update\n"
            "Timestamp: %s %s (UTC)\n"
            "Terminal: 0x%04x\n"
            "Transaction Id: %u\n"
            "Counter: %u\n",
            card_id,
            balance_usd,
            balance_cents,
            device_description,
            furi_string_get_cstr(date_str),
            furi_string_get_cstr(time_str),
            terminal_id,
            txn_id,
            counter);

        furi_string_free(date_str);
        furi_string_free(time_str);

        parsed = true;
    } while(false);

    return parsed;
}

// Convert a "1900"-based timestamp to Furi time, assuming a UTC/GMT timezone.
static void epoch_1900_datetime_to_furi(uint32_t seconds, FuriHalRtcDateTime* out) {
    uint16_t year, month, day, hour, minute, second;

    // Calculate absolute number of days elapsed since the 1900 epoch
    // and save the residual for the time within the day.
    uint32_t absolute_days = seconds / 86400;
    uint32_t seconds_within_day = seconds % 86400;

    // Calculate day of the week.
    // January 1, 1900 was a Monday ("day of week" = 1)
    uint8_t dow = (absolute_days + 1) % 7;

    //
    // Compute the date by simply marching through time in as large chunks
    // as possible.
    //

    for(year = 1900;; year++) {
        uint16_t year_days = furi_hal_rtc_get_days_per_year(year);
        if(absolute_days >= year_days)
            absolute_days -= year_days;
        else
            break;
    }

    bool is_leap = furi_hal_rtc_is_leap_year(year);

    for(month = 1;; month++) {
        uint8_t days_in_month = furi_hal_rtc_get_days_per_month(is_leap, month);
        if(absolute_days >= days_in_month)
            absolute_days -= days_in_month;
        else
            break;
    }

    day = absolute_days + 1;
    hour = seconds_within_day / 3600;
    uint16_t sub_hour = seconds_within_day % 3600;
    minute = sub_hour / 60;
    second = sub_hour % 60;

    out->year = year;
    out->month = month;
    out->day = day;
    out->hour = hour;
    out->minute = minute;
    out->second = second;
    out->weekday = dow;
}

/* Actual implementation of app<>plugin interface */
static const NfcSupportedCardsPlugin clipper_plugin = {
    .protocol = NfcProtocolMfDesfire,
    .verify = NULL,
    .read = NULL,
    .parse = clipper_parse,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor clipper_plugin_descriptor = {
    .appid = NFC_SUPPORTED_CARD_PLUGIN_APP_ID,
    .ep_api_version = NFC_SUPPORTED_CARD_PLUGIN_API_VERSION,
    .entry_point = &clipper_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* clipper_plugin_ep() {
    return &clipper_plugin_descriptor;
}
