#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO14443_CRC_SIZE sizeof(uint16_t)

typedef enum {
    Iso14443CrcTypeA,
    Iso14443CrcTypeB,
} Iso14443CrcType;

uint16_t iso14443_crc_calculate(Iso14443CrcType type, const uint8_t* data, size_t data_size);

#ifdef __cplusplus
}
#endif
