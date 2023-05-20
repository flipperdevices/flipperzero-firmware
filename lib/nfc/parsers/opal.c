/*
 * opal.c - Parser for Opal card (Sydney, Australia).
 *
 * Copyright 2023 Michael Farrell <micolous+git@gmail.com>
 *
 * This will only read "standard" MIFARE DESFire-based Opal cards. Free travel
 * cards (including School Opal cards, veteran, vision-impaired persons and
 * TfNSW employees' cards) and single-trip tickets are MIFARE Ultralight C
 * cards and not supported.
 *
 * Reference: https://github.com/metrodroid/metrodroid/wiki/Opal
 *
 * Note: The card values are all little-endian (like Flipper), but the above
 * reference was originally written based on Java APIs, which are big-endian.
 * This implementation presumes a little-endian system.
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
#include "nfc_supported_card.h"
#include "opal.h"

#include <applications/services/locale/locale.h>
#include <gui/modules/widget.h>
#include <nfc_worker_i.h>

#include <furi_hal.h>

static const uint8_t opal_aid[3] = {0x31, 0x45, 0x53};
static const char* opal_modes[5] =
    {"Rail / Metro", "Ferry / Light Rail", "Bus", "Unknown mode", "Manly Ferry"};
static const char* opal_usages[14] = {
    "New / Unused",
    "Tap on: new journey",
    "Tap on: transfer from same mode",
    "Tap on: transfer from other mode",
    "", // Manly Ferry: new journey
    "", // Manly Ferry: transfer from ferry
    "", // Manly Ferry: transfer from other
    "Tap off: distance fare",
    "Tap off: flat fare",
    "Automated tap off: failed to tap off",
    "Tap off: end of trip without start",
    "Tap off: reversal",
    "Tap on: rejected",
    "Unknown usage",
};

// Converts an Opal timestamp to FuriHalRtcDateTime.
//
// Opal measures days since 1980-01-01 and minutes since midnight, and presumes
// all days are 1440 minutes.
void opal_date_time_to_furi(uint16_t days, uint16_t minutes, FuriHalRtcDateTime* out) {
    if(!out) return;
    uint16_t diy;
    out->year = 1980;
    out->month = 0;
    // 1980-01-01 is a Tuesday
    out->weekday = ((days + 1) % 7) + 1;
    out->hour = minutes / 60;
    out->minute = minutes % 60;
    out->second = 0;

    // What year is it?
    for(;;) {
        diy = (FURI_HAL_RTC_IS_LEAP_YEAR(out->year) ? 366 : 365);
        if(days < diy) break;
        days -= diy;
        out->year++;
    }

    // 1-index the day of the year
    days++;
    // What month is it?
    uint8_t is_leap = diy - 365;

    for(;;) {
        uint8_t dim = furi_hal_rtc_days_per_month[is_leap][out->month];
        if(days <= dim) break;
        days -= dim;
        out->month++;
    }

    out->month++;
    out->day = days;
}

bool opal_parser_parse(NfcDeviceData* dev_data) {
    if(dev_data->protocol != NfcDeviceProtocolMifareDesfire) {
        return false;
    }

    MifareDesfireApplication* app = mf_df_get_application(&dev_data->mf_df_data, &opal_aid);
    if(app == NULL) {
        return false;
    }
    MifareDesfireFile* f = mf_df_get_file(app, 0x07);
    if(f == NULL || f->type != MifareDesfireFileTypeStandard || f->settings.data.size != 16 ||
       !f->contents) {
        return false;
    }

    uint32_t serial = *(uint32_t*)(f->contents); // bit 96..127
    uint8_t serial2 = serial / 10000000;
    uint16_t serial3 = (serial / 1000) % 10000;
    uint16_t serial4 = (serial % 1000);

    uint8_t check_digit = f->contents[4] & 0x0f; // bit 92..95
    if(check_digit > 9) {
        return false;
    }
    bool blocked = (f->contents[4] >> 4) & 0x1; // bit 91
    uint16_t txn_number = (*(uint32_t*)(f->contents + 4)) >> 5; // bit 75..90

    int32_t balance = ((*(uint32_t*)(f->contents + 6)) >> 5) & 0x1fffff; // bit 54..74
    char* sign = "";
    if((balance & 0x100000) > 0) {
        // Negative balance. Make this a positive value again and record the
        // sign separately, because then we can handle balances of -99..-1
        // cents, as the "dollars" division below would result in a positive
        // zero value.
        balance = abs((int32_t)(balance | 0xfff00000));
        sign = "-";
    }
    uint8_t cents = balance % 100;
    int32_t dollars = balance / 100;

    uint16_t days = ((*(uint32_t*)(f->contents + 9)) >> 2) & 0x7fff; // bit 39..53
    uint16_t minutes = ((*(uint32_t*)(f->contents + 11)) >> 1) & 0x7ff; // bit 28..38
    FuriHalRtcDateTime timestamp;
    opal_date_time_to_furi(days, minutes, &timestamp);

    uint8_t mode = ((*(uint16_t*)(f->contents + 12)) >> 4) & 0x7; // bit 25..27
    if(mode >= 3) {
        // 3..7 are "reserved", but we use 4 to indicate the Manly Ferry.
        mode = 3;
    }

    uint8_t usage = ((*(uint16_t*)(f->contents + 12)) >> 7) & 0xf; // bit 21..24
    if(usage >= 4 && usage <= 6) {
        // Usages 4..6 associated with the Manly Ferry, which correspond to
        // usages 1..3 for other modes.
        usage -= 3;
        mode = 4;
    }

    bool auto_topup = (f->contents[13] >> 3) & 0x1; // bit 20
    uint8_t weekly_journeys = f->contents[13] >> 4; // bit 16..19

    const char* mode_str = (mode <= 4 ? opal_modes[mode] : opal_modes[3]);
    const char* usage_str = (usage <= 12 ? opal_usages[usage] : opal_usages[13]);

    furi_string_printf(
        dev_data->parsed_data,
        "\e#Opal: $%s%ld.%02hu\n3085 22%02hhu %04hu %03hu%01hhu\n%s, %s\n",
        sign,
        dollars,
        cents,
        serial2,
        serial3,
        serial4,
        check_digit,
        mode_str,
        usage_str);
    FuriString* timestamp_str = furi_string_alloc();
    locale_format_date(timestamp_str, &timestamp, locale_get_date_format(), "-");
    furi_string_cat(dev_data->parsed_data, timestamp_str);
    furi_string_cat_str(dev_data->parsed_data, " at ");

    locale_format_time(timestamp_str, &timestamp, locale_get_time_format(), false);
    furi_string_cat(dev_data->parsed_data, timestamp_str);

    furi_string_free(timestamp_str);
    furi_string_cat_printf(
        dev_data->parsed_data, "\nWeekly journeys: %hhu, Txn #%hu\n", weekly_journeys, txn_number);

    if(auto_topup) {
        furi_string_cat_str(dev_data->parsed_data, "Auto-topup enabled\n");
    }
    if(blocked) {
        furi_string_cat_str(dev_data->parsed_data, "Card blocked\n");
    }
    return true;
}
