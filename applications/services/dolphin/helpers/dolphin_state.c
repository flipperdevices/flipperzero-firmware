#include "dolphin_state.h"
#include "dolphin/helpers/dolphin_deed.h"
#include "dolphin_state_filename.h"

#include <stdint.h>
#include <storage/storage.h>
#include <furi.h>
#include <furi_hal.h>
#include <math.h>
#include <toolbox/saved_struct.h>

#define TAG "DolphinState"

#define DOLPHIN_STATE_PATH INT_PATH(DOLPHIN_STATE_FILE_NAME)
#define DOLPHIN_STATE_HEADER_MAGIC 0xD0
#define DOLPHIN_STATE_HEADER_VERSION 0x01

/*
The way the math works:

Current xp (icounter) is subtracted from the threshold (xp requirement for next level up)

The array values in level_array result in the following XP requirements per level below:

500,  750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750,
3000, 3250, 3500, 3750, 4000, 4250, 4500, 4750, 5000, 6000,
7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000

To change the level requirement, add the last level's value with the desired next level's value.
Ex: Level 1 is 0 - 500. Level 2 needs to be Level 1 + next desired XP value.

Level 2 is calculated:
Previous Level's Required XP: 500
+
Desired Level XP: 750
=
Value in level_array: 1250
*/

uint32_t level_array[29] = {500,    1250,   2250,   3500,   5000,  6750,  8750,  11000,
                            13500,  16250,  19250,  22500,  26000, 29750, 33750, 38000,
                            42500,  47250,  52250,  58250,  65250, 73250, 82250, 92250,
                            103250, 115250, 128250, 142250, 157250};

/*
This calculates the size of an array. This is good as it's used for dynamic for loops below. Therefore, you can just add more values to level_array for more levels.
*/
#define NUM(a) (sizeof(a) / sizeof(*a))

#define BUTTHURT_MAX 14
#define BUTTHURT_MIN 0

DolphinState* dolphin_state_alloc() {
    return malloc(sizeof(DolphinState));
}

int dolphin_state_max_level() {
    return NUM(level_array) + 1;
}

void dolphin_state_free(DolphinState* dolphin_state) {
    free(dolphin_state);
}

bool dolphin_state_save(DolphinState* dolphin_state) {
    if(!dolphin_state->dirty) {
        return true;
    }

    bool result = saved_struct_save(
        DOLPHIN_STATE_PATH,
        &dolphin_state->data,
        sizeof(DolphinStoreData),
        DOLPHIN_STATE_HEADER_MAGIC,
        DOLPHIN_STATE_HEADER_VERSION);

    if(result) {
        FURI_LOG_I(TAG, "State saved");
        dolphin_state->dirty = false;
    } else {
        FURI_LOG_E(TAG, "Failed to save state");
    }

    return result;
}

bool dolphin_state_load(DolphinState* dolphin_state) {
    bool success = saved_struct_load(
        DOLPHIN_STATE_PATH,
        &dolphin_state->data,
        sizeof(DolphinStoreData),
        DOLPHIN_STATE_HEADER_MAGIC,
        DOLPHIN_STATE_HEADER_VERSION);

    if(success) {
        if((dolphin_state->data.butthurt > BUTTHURT_MAX) ||
           (dolphin_state->data.butthurt < BUTTHURT_MIN)) {
            success = false;
        }
    }

    if(!success) {
        FURI_LOG_W(TAG, "Reset dolphin-state");
        memset(dolphin_state, 0, sizeof(*dolphin_state));
        dolphin_state->dirty = true;
    }

    return success;
}

uint64_t dolphin_state_timestamp() {
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    return furi_hal_rtc_datetime_to_timestamp(&datetime);
}

bool dolphin_state_is_levelup(uint32_t icounter) {
    for(int i = 0; i < NUM(level_array); i++) {
        if((icounter == level_array[i])) {
            return (icounter == level_array[i]);
        }
    }
    return NUM(level_array) + 1;
}

uint8_t dolphin_get_level(uint32_t icounter) {
    for(int i = 0; i < NUM(level_array); i++) {
        if(icounter <= level_array[i]) {
            return i + 1;
        }
    }
    return NUM(level_array) + 1;
}

uint32_t dolphin_state_xp_above_last_levelup(uint32_t icounter) {
    uint32_t threshold = 0;
    for(int i = 0; i < NUM(level_array); i++) {
        if(icounter <= level_array[0]) {
            threshold = 0;
            break;
        } else if(icounter <= level_array[i]) {
            threshold = level_array[i - 1];
            break;
        } else {
            threshold = level_array[NUM(level_array) - 1];
        }
    }
    return icounter - threshold;
}

uint32_t dolphin_state_xp_to_levelup(uint32_t icounter) {
    uint32_t threshold = 0;
    for(int i = 0; i < NUM(level_array); i++) {
        if(icounter <= level_array[i]) {
            threshold = level_array[i];
            break;
        } else {
            threshold = (uint32_t)-1;
        }
    }
    return threshold - icounter;
}

void dolphin_state_on_deed(DolphinState* dolphin_state, DolphinDeed deed) {
    // Special case for testing
    if(deed > DolphinDeedMAX) {
        if(deed == DolphinDeedTestLeft) {
            dolphin_state->data.butthurt =
                CLAMP(dolphin_state->data.butthurt + 1, BUTTHURT_MAX, BUTTHURT_MIN);
            if(dolphin_state->data.icounter > 0) dolphin_state->data.icounter--;
            dolphin_state->data.timestamp = dolphin_state_timestamp();
            dolphin_state->dirty = true;
        } else if(deed == DolphinDeedTestRight) {
            dolphin_state->data.butthurt = BUTTHURT_MIN;
            if(dolphin_state->data.icounter < UINT32_MAX) dolphin_state->data.icounter++;
            dolphin_state->data.timestamp = dolphin_state_timestamp();
            dolphin_state->dirty = true;
        }
        return;
    }

    DolphinApp app = dolphin_deed_get_app(deed);
    int8_t weight_limit =
        dolphin_deed_get_app_limit(app) - dolphin_state->data.icounter_daily_limit[app];
    uint8_t deed_weight = CLAMP(dolphin_deed_get_weight(deed), weight_limit, 0);

    uint32_t xp_to_levelup = dolphin_state_xp_to_levelup(dolphin_state->data.icounter);
    if(xp_to_levelup) {
        deed_weight = MIN(xp_to_levelup, deed_weight);
        dolphin_state->data.icounter += deed_weight;
        dolphin_state->data.icounter_daily_limit[app] += deed_weight;
    }

    /* decrease butthurt:
     * 0 deeds accumulating --> 0 butthurt
     * +1....+15 deeds accumulating --> -1 butthurt
     * +16...+30 deeds accumulating --> -1 butthurt
     * +31...+45 deeds accumulating --> -1 butthurt
     * +46...... deeds accumulating --> -1 butthurt
     * -4 butthurt per day is maximum
     * */
    uint8_t butthurt_icounter_level_old = dolphin_state->data.butthurt_daily_limit / 15 +
                                          !!(dolphin_state->data.butthurt_daily_limit % 15);
    dolphin_state->data.butthurt_daily_limit =
        CLAMP(dolphin_state->data.butthurt_daily_limit + deed_weight, 46, 0);
    uint8_t butthurt_icounter_level_new = dolphin_state->data.butthurt_daily_limit / 15 +
                                          !!(dolphin_state->data.butthurt_daily_limit % 15);
    int32_t new_butthurt = ((int32_t)dolphin_state->data.butthurt) -
                           (butthurt_icounter_level_old != butthurt_icounter_level_new);
    new_butthurt = CLAMP(new_butthurt, BUTTHURT_MAX, BUTTHURT_MIN);
    if(new_butthurt >= 7) new_butthurt = BUTTHURT_MIN; // FLIPPER STAYS HAPPY
    dolphin_state->data.butthurt = new_butthurt;
    dolphin_state->data.timestamp = dolphin_state_timestamp();
    dolphin_state->dirty = true;

    FURI_LOG_D(
        TAG,
        "icounter %lu, butthurt %ld",
        dolphin_state->data.icounter,
        dolphin_state->data.butthurt);
}

void dolphin_state_butthurted(DolphinState* dolphin_state) {
    if(dolphin_state->data.butthurt < BUTTHURT_MAX) {
        dolphin_state->data.butthurt++;
        dolphin_state->data.timestamp = dolphin_state_timestamp();
        dolphin_state->dirty = true;
    }
}

void dolphin_state_increase_level(DolphinState* dolphin_state) {
    furi_assert(dolphin_state_is_levelup(dolphin_state->data.icounter));
    ++dolphin_state->data.icounter;
    dolphin_state->dirty = true;
}

void dolphin_state_clear_limits(DolphinState* dolphin_state) {
    furi_assert(dolphin_state);

    for(int i = 0; i < DolphinAppMAX; ++i) {
        dolphin_state->data.icounter_daily_limit[i] = 0;
    }
    dolphin_state->data.butthurt_daily_limit = 0;
    dolphin_state->dirty = true;
}