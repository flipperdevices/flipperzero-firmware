#pragma once

#include <stdint.h>

typedef struct Power Power;

typedef enum {
    PowerBootModeNormal,
    PowerBootModeDfu,
} PowerBootMode;

typedef struct {
    float current_charger;
    float current_gauge;

    float voltage_charger;
    float voltage_gauge;
    float voltage_vbus;

    uint32_t capacity_remaining;
    uint32_t capacity_full;

    float temperature_charger;
    float temperature_gauge;

    uint8_t charge;
    uint8_t health;
} PowerInfo;

/** Power off device
 * @param power - Power instance
 */
void power_off(Power* power);

/** Reboot device
 * @param power - Power instance
 * @param mode - PowerBootMode
 */
void power_reboot(Power* power, PowerBootMode mode);

/** Get power info
 * @param power - Power instance
 * @param info - PowerInfo instance
 */
void power_get_info(Power* power, PowerInfo* info);
