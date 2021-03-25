#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ApiHalSubGhzPathIsolation,
    ApiHalSubGhzPath1,
    ApiHalSubGhzPath2,
    ApiHalSubGhzPath3,
} ApiHalSubGhzPath;

void api_hal_subghz_init();

void api_hal_subghz_sleep();

void api_hal_subghz_set_frequency(uint32_t value);

void api_hal_subghz_set_path(ApiHalSubGhzPath path);

#ifdef __cplusplus
}
#endif
