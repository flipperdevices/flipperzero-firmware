#include "set_name.h"
#include <stdio.h>
#include <stdint.h>
#include <furi_hal_rtc.h>

void set_name(char* name, uint8_t max_name_size, char* app_name) {
    FuriHalRtcDateTime dateTime;
    furi_hal_rtc_get_datetime(&dateTime);
    snprintf(
        name,
        max_name_size,
        "%s-%.4d_%.2d_%.2d-%.2d_%.2d",
        app_name,
        dateTime.year,
        dateTime.month,
        dateTime.day,
        dateTime.hour,
        dateTime.minute);
}
