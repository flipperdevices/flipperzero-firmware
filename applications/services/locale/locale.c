#include "locale.h"

#define TAG "LocaleSrv"

LocaleMeasurementUnit locale_get_measurement_unit(void) {
    return furi_hal_rtc_get_locale_units();
}

void locale_set_measurement_unit(LocaleMeasurementUnit format) {
    furi_hal_rtc_set_locale_units(format);
}

LocaleTimeFormat locale_get_time_format(void) {
    return furi_hal_rtc_get_locale_timeformat();
}

void locale_set_time_format(LocaleTimeFormat format) {
    furi_hal_rtc_set_locale_timeformat(format);
}

LocaleDateFormat locale_get_date_format(void) {
    return furi_hal_rtc_get_locale_dateformat();
}

void locale_set_date_format(LocaleDateFormat format) {
    furi_hal_rtc_set_locale_dateformat(format);
}

float locale_fahrenheit_to_celsius(float temp_f) {
    return (temp_f - 32.f) / 1.8f;
}

float locale_celsius_to_fahrenheit(float temp_c) {
    return (temp_c * 1.8f + 32.f);
}

void locale_format_time(
    FuriString* out_str,
    FuriHalRtcDateTime* datetime,
    LocaleTimeFormat format,
    bool show_seconds) {
    furi_assert(out_str);
    furi_assert(datetime);

    uint8_t hours = datetime->hour;
    uint8_t am_pm = 0;
    if(format == LocaleTimeFormat12h) {
        if(hours > 12) {
            hours -= 12;
            am_pm = 2;
        } else {
            am_pm = 1;
        }
    }

    if(show_seconds) {
        furi_string_printf(out_str, "%02u:%02u:%02u", hours, datetime->minute, datetime->second);
    } else {
        furi_string_printf(out_str, "%02u:%02u", hours, datetime->minute);
    }

    if(am_pm > 0) {
        furi_string_cat_printf(out_str, " %s", (am_pm == 1) ? ("AM") : ("PM"));
    }
}

void locale_format_date(
    FuriString* out_str,
    FuriHalRtcDateTime* datetime,
    LocaleDateFormat format,
    char* separator) {
    furi_assert(out_str);
    furi_assert(datetime);
    furi_assert(separator);

    if(format == LocaleDateFormatDMY) {
        furi_string_printf(
            out_str,
            "%02u%s%02u%s%04u",
            datetime->day,
            separator,
            datetime->month,
            separator,
            datetime->year);
    } else if(format == LocaleDateFormatMDY) {
        furi_string_printf(
            out_str,
            "%02u%s%02u%s%04u",
            datetime->month,
            separator,
            datetime->day,
            separator,
            datetime->year);
    } else {
        furi_string_printf(
            out_str,
            "%04u%s%02u%s%02u",
            datetime->year,
            separator,
            datetime->month,
            separator,
            datetime->day);
    }
}

int32_t locale_on_system_start(void* p) {
    UNUSED(p);
    return 0;
}
