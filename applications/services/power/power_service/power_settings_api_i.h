#pragma once

#include "power.h"
#include "../power_settings.h"

/** Get current power service settings
 *
 * @param power     Power instance
 * @param settings  PowerSettings instance to copy settings into
 */
void power_get_settings(Power* power, PowerSettings* settings);

/** Apply and save power service settings
 *
 * @param power     Power instance
 * @param settings  PowerSettings instance to copy settings from
 */
void power_set_settings(Power* power, const PowerSettings* settings);
