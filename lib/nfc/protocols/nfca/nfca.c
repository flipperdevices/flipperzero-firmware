#include "nfca.h"

#include <furi.h>

#define NFCA_CRC_INIT (0x6363)

static uint16_t nfca_get_crc(const uint8_t* buff, uint16_t len) {
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

void nfca_append_crc(BitBuffer* buffer) {
    furi_assert(buffer);

    const uint8_t* data = bit_buffer_get_data(buffer);
    size_t bytes = bit_buffer_get_size_bytes(buffer);

    uint16_t crc = nfca_get_crc(data, bytes);
    uint8_t crc_bytes[2] = {(uint8_t)crc, (uint8_t)(crc >> 8)};
    bit_buffer_append_bytes(buffer, crc_bytes, sizeof(crc_bytes));
}

bool nfca_check_crc(const BitBuffer* buf) {
    furi_assert(buf);

    bool crc_ok = false;
    do {
        const uint8_t* data = bit_buffer_get_data(buf);
        size_t bytes = bit_buffer_get_size_bytes(buf);
        if(bytes < 3) break;

        uint16_t crc_calc = nfca_get_crc(data, bytes - 2);
        uint8_t crc_start = bit_buffer_get_byte(buf, bytes - 2);
        uint8_t crc_end = bit_buffer_get_byte(buf, bytes - 1);
        uint16_t crc_received = (crc_end << 8) | crc_start;
        crc_ok = (crc_calc == crc_received);
    } while(false);

    return crc_ok;
}

void nfca_trim_crc(BitBuffer* buf) {
    furi_assert(buf);

    size_t bytes = bit_buffer_get_size_bytes(buf);
    furi_assert(bytes > 2);

    bit_buffer_set_size_bytes(buf, bytes - 2);
}

uint32_t nfca_get_cuid(NfcaData* nfca_data) {
    furi_assert(nfca_data);

    uint32_t cuid = 0;
    uint8_t* cuid_start = nfca_data->uid;
    if(nfca_data->uid_len == 7) {
        cuid_start = &nfca_data->uid[3];
    }
    cuid = (cuid_start[0] << 24) | (cuid_start[1] << 16) | (cuid_start[2] << 8) | (cuid_start[3]);

    return cuid;
}
