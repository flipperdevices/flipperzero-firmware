#pragma once

#include <stm32wbxx_ll_i2c.h>

/** FuriHal i2c bus */
struct FuriHalI2cBus {
    I2C_TypeDef* i2c;
    FuriHalI2cBusHandle* current_handle;
    FuriHalI2cBusEventCallback callback;
};

