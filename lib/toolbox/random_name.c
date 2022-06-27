#include "random_name.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <furi.h>

void set_random_name(char* name, uint8_t max_name_size) {
    static bool rand_generator_inited = false;

    if(!rand_generator_inited) {
        srand(DWT->CYCCNT);
        rand_generator_inited = true;
    }
    const char* prefix[] = {
        "super",
        "big",
        "little",
        "liquid",
        "unknown",
        "thin",
        "thick",
        "great",
        "my",
    };

    const char* suffix[] = {
        "maslina",
        "sus",
        "anomalija",
        "artefact",
        "monolit",
        "burer",
        "sidorovich",
        "habar",
    };
    // sus is not (sus)pect - this is about super sus
    uint8_t prefix_i = rand() % COUNT_OF(prefix);
    uint8_t suffix_i = rand() % COUNT_OF(suffix);

    sniprintf(name, max_name_size, "%s_%s", prefix[prefix_i], suffix[suffix_i]);
    // Set first symbol to upper case
    name[0] = name[0] - 0x20;
}
/* 
void set_random_name(char* name, uint8_t max_name_size) {
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    char strings[1][25];
    sprintf(strings[0], "%s%.4d%.2d%.2d%.2d%.2d", "s"
        , datetime.year, datetime.month, datetime.day
        , datetime.hour, datetime.minute
    );
    sniprintf(name, max_name_size, "%s", strings[0]);
    // Set first symbol to upper case
    name[0] = name[0] - 0x20;
}
*/