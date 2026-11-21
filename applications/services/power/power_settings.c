#include "power_settings.h"
#include "power_settings_filename.h"

#include <furi.h>
#include <storage/storage.h>
#include <toolbox/saved_struct.h>

#define TAG "PowerSettings"

#define POWER_SETTINGS_PATH    INT_PATH(POWER_SETTINGS_FILE_NAME)
#define POWER_SETTINGS_VERSION (0)
#define POWER_SETTINGS_MAGIC   (0x21)

void power_settings_reset(PowerSettings* settings) {
    furi_assert(settings);

    *settings = (PowerSettings){
        .low_battery_threshold = POWER_SETTINGS_LOW_BATTERY_THRESHOLD_DEFAULT,
        .low_battery_show_popup = true,
        .low_battery_repeat_step = POWER_SETTINGS_LOW_BATTERY_REPEAT_STEP_DEFAULT,
    };
}

void power_settings_load(PowerSettings* settings) {
    furi_assert(settings);

    const bool success = saved_struct_load(
        POWER_SETTINGS_PATH,
        settings,
        sizeof(PowerSettings),
        POWER_SETTINGS_MAGIC,
        POWER_SETTINGS_VERSION);

    if(!success || settings->low_battery_threshold > 100 ||
       settings->low_battery_repeat_step > 100) {
        FURI_LOG_W(TAG, "Failed to load file, using defaults");
        power_settings_reset(settings);
    }
}

void power_settings_save(const PowerSettings* settings) {
    furi_assert(settings);

    const bool success = saved_struct_save(
        POWER_SETTINGS_PATH,
        settings,
        sizeof(PowerSettings),
        POWER_SETTINGS_MAGIC,
        POWER_SETTINGS_VERSION);

    if(!success) {
        FURI_LOG_E(TAG, "Failed to save file");
    }
}
