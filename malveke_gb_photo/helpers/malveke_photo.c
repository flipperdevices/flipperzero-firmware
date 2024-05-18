#include "malveke_photo.h"

void get_timefilename(FuriString* name, int index) {
    DateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);
    furi_string_printf(
        name,
        "%s/%.4d%.2d%.2d-%.2d%.2d%.2d-%d.bmp",
        MALVEKE_APP_FOLDER_PHOTOS,
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second,
        index);
}