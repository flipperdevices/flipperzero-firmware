#include "iso14443_crc.h"

#include <core/check.h>

#define ISO14443_3A_CRC_INIT (0x6363U)
#define ISO14443_3B_CRC_INIT (0xFFFFU)

uint16_t iso14443_crc_calculate(Iso14443CrcType type, const uint8_t* data, size_t data_size) {
    furi_assert(data);

    uint16_t crc;

    if(type == Iso14443CrcTypeA) {
        crc = ISO14443_3A_CRC_INIT;
    } else if(type == Iso14443CrcTypeB) {
        crc = ISO14443_3B_CRC_INIT;
    } else {
        furi_crash("Wrong ISO14443 CRC type");
    }

    for(size_t i = 0; i < data_size; i++) {
        uint8_t byte = data[i];
        byte ^= (uint8_t)(crc & 0xff);
        byte ^= byte << 4;
        crc = (crc >> 8) ^ (((uint16_t)byte) << 8) ^ (((uint16_t)byte) << 3) ^ (byte >> 4);
    }

    return type == Iso14443CrcTypeA ? crc : ~crc;
}
