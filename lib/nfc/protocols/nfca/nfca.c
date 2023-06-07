#include "nfca.h"

#include <furi.h>

#define NFCA_CRC_INIT (0x6363)

uint16_t nfca_get_crc(uint8_t* buff, uint16_t len) {
    furi_assert(buff);
    furi_assert(len);

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

void nfca_append_crc_buff(BitBuffer* buffer) {
    furi_assert(buffer);

    uint8_t* data = bit_buffer_get_data(buffer);
    size_t bytes = bit_buffer_get_size_bytes(buffer);

    uint16_t crc = nfca_get_crc(data, bytes);
    uint8_t crc_bytes[2] = {(uint8_t)crc, (uint8_t)(crc >> 8)};
    bit_buffer_append_bytes(buffer, crc_bytes, sizeof(crc_bytes));
}

bool nfca_check_and_trim_crc_buff(BitBuffer* buffer, const BitBuffer* src) {
    furi_assert(buffer);

    bool crc_ok = false;
    do {
        uint8_t* data = bit_buffer_get_data(src);
        size_t bytes = bit_buffer_get_size_bytes(src);
        if(bytes < 3) break;

        uint16_t crc_calc = nfca_get_crc(data, bytes - 2);
        uint8_t crc_start = bit_buffer_get_byte(src, bytes - 2);
        uint8_t crc_end = bit_buffer_get_byte(src, bytes - 1);
        uint16_t crc_received = (crc_end << 8) | crc_start;
        crc_ok = (crc_calc == crc_received);
        if(crc_ok) {
            bit_buffer_copy_left(buffer, src, bytes - 2);
        } else {
            bit_buffer_copy(buffer, src);
        }
    } while(false);

    return crc_ok;
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
