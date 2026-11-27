#pragma once

typedef struct PowerBatteryLow PowerBatteryLow;

typedef void (*PowerBatteryLowCallback)(void* context);

#include <gui/view.h>

PowerBatteryLow* power_battery_low_alloc(void);

void power_battery_low_free(PowerBatteryLow* power_battery_low);

View* power_battery_low_get_view(PowerBatteryLow* power_battery_low);

void power_battery_low_set_callback(
    PowerBatteryLow* power_battery_low,
    PowerBatteryLowCallback callback,
    void* context);

void power_battery_low_set_charge(PowerBatteryLow* power_battery_low, uint8_t charge);
