#pragma once

#include <furi-hal-i2c-types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FuriHalI2cBus furi_hal_i2c_bus_power;

extern FuriHalI2cBus furi_hal_i2c_bus_external;

extern FuriHalI2cBusHandle furi_hal_i2c_handle_power;

#ifdef __cplusplus
}
#endif