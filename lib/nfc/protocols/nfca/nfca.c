#include "nfca.h"

#include <furi.h>

#define NFCA_CRC_INIT (0x6363)

uint16_t nfca_get_crc(uint8_t* buff, uint16_t len) {
    furi_assert(buff);

    uint16_t crc = NFCA_CRC_INIT;
    uint8_t byte = 0;

    for(uint8_t i = 0; i < len; i++) {
        byte = buff[i];
        byte ^= (uint8_t)(crc & 0xff);
        byte ^= byte << 4;
        crc = (crc >> 8) ^ (((uint16_t)byte) << 8) ^ (((uint16_t)byte) << 3) ^
              (((uint16_t)byte) >> 4);
    }

    return crc;
}

void nfca_append_crc(uint8_t* buff, uint16_t len) {
    furi_assert(buff);

    uint16_t crc = nfca_get_crc(buff, len);
    buff[len] = (uint8_t)crc;
    buff[len + 1] = (uint8_t)(crc >> 8);
}

bool nfca_check_crc(uint8_t* buff, uint16_t len) {
    bool crc_ok = false;

    if(len > 2) {
        uint16_t crc_calc = nfca_get_crc(buff, len - 2);
        uint16_t crc_received = (buff[len - 1] << 8) | buff[len - 2];
        crc_ok = (crc_calc == crc_received);
    }

    return crc_ok;
}
