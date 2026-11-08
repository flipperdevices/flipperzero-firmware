/**
 * @file power_settings.h
 * @brief Power service settings.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Default battery charge level (in percent) below which the user is alerted */
#define POWER_SETTINGS_LOW_BATTERY_THRESHOLD_DEFAULT   (10U)
/** Default additional charge drop (in percent) after which the alert is repeated */
#define POWER_SETTINGS_LOW_BATTERY_REPEAT_STEP_DEFAULT (5U)

/**
 * @brief Power service settings storage type.
 */
typedef struct {
    /** Battery charge level in percent at which the low battery alert is triggered, 0 - disabled */
    uint8_t low_battery_threshold;
    /** Show low battery alert screen in addition to LED, sound and vibration */
    bool low_battery_show_popup;
    /** Repeat the alert each time the charge drops by this many percent more, 0 - alert once */
    uint8_t low_battery_repeat_step;
} PowerSettings;

/**
 * @brief Fill power settings with default values.
 *
 * @param[out] settings pointer to a PowerSettings instance to reset.
 */
void power_settings_reset(PowerSettings* settings);

/**
 * @brief Load power settings from file, use defaults on failure.
 *
 * @param[out] settings pointer to a PowerSettings instance to load settings into.
 */
void power_settings_load(PowerSettings* settings);

/**
 * @brief Save power settings to file.
 *
 * @param[in] settings pointer to a PowerSettings instance to save settings from.
 */
void power_settings_save(const PowerSettings* settings);

#ifdef __cplusplus
}
#endif
