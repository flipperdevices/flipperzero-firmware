#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <api-hal-spi.h>

#ifdef __cplusplus
extern "C" {
#endif

void cc1101_init(const ApiHalSpiDevice* device);

void cc1101_reset(const ApiHalSpiDevice* device);

void cc1101_sleep(const ApiHalSpiDevice* device);

#ifdef __cplusplus
}
#endif