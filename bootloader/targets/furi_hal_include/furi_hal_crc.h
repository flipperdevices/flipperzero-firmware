#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// Enable CRC module clock + configure for CRC32 calculation
void furi_hal_crc_init();

// Disable CRC module clock
void furi_hal_crc_deinit();

// Reset current calculation state
void furi_hal_crc_reset();

// Process data block. Does not reset current state,
// allowing to process arbitrary data lengths
uint32_t furi_hal_crc_feed(void* data, uint16_t length);


#ifdef __cplusplus
}
#endif