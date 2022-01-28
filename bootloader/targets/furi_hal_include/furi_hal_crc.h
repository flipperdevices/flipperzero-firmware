#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_hal_crc_init();

void furi_hal_crc_deinit();

void furi_hal_crc_reset();

void furi_hal_crc_feed(void* data, uint16_t length);

uint32_t furi_hal_crc_get();

#ifdef __cplusplus
}
#endif