#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_api_accumulator_set(uint32_t value);

uint32_t app_api_accumulator_get();

void app_api_accumulator_add(uint32_t value);

void app_api_accumulator_sub(uint32_t value);

void app_api_accumulator_mul(uint32_t value);

#ifdef __cplusplus
}
#endif
