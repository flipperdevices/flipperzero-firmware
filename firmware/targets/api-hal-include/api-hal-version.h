#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

const uint8_t api_hal_version_get_hw_version();

const uint8_t api_hal_version_get_hw_target();

const uint8_t api_hal_version_get_hw_board();

const uint8_t api_hal_version_get_hw_connect();

const uint32_t api_hal_version_get_hw_timestamp();

#ifdef __cplusplus
}
#endif
