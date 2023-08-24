#include "name_generator.h"

#include <stdio.h>
#include <stdint.h>
#include <furi_hal_rtc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <furi.h>

void set_random_name(char* name, uint8_t max_name_size) {
    const char* prefix[] = {
        "ancient",     "hollow",     "strange",    "disappeared", "unknown",
        "unthinkable", "unnameable", "nameless",   "my",          "concealed",
        "forgotten",   "hidden",     "mysterious", "obscure",     "random",
        "remote",      "uncharted",  "undefined",  "untraveled",  "untold",
    };

    const char* suffix[] = {
        "door",
        "entrance",
        "doorway",
        "entry",
        "portal",
        "entree",
        "opening",
        "crack",
        "access",
        "corridor",
        "passage",
        "port",
    };
    uint8_t prefix_i = rand() % COUNT_OF(prefix);
    uint8_t suffix_i = rand() % COUNT_OF(suffix);

    snprintf(name, max_name_size, "%s_%s", prefix[prefix_i], suffix[suffix_i]);
    // Set first symbol to upper case
    name[0] = name[0] - 0x20;
}

const char* convert_app_extension_to_name(char* app_name) {
    struct NameConversion {
        const char* original;
        const char* converted;
    };

    struct NameConversion conversions[] = {
        {".ibtn", "iBtn"}, {".nfc", "NFC"}, {".rfid", "RFID"}, {".sub", "SubGhz"}};

    const int numConversions =
        sizeof(conversions) / sizeof(conversions[0]); // gets the number of potential conversions
    const char* converted_name = app_name;

    for(int i = 0; i < numConversions; ++i) {
        if(!strcmp(app_name, conversions[i].original)) {
            converted_name = conversions[i].converted;
            break;
        }
    }

    return converted_name;
}

void name_generator_set(char* name, uint8_t max_name_size, char* app_name) {
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDetailedFilename)) {
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
    } else {
        set_random_name(name, max_name_size);
    }
}
