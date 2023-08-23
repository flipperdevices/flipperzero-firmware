#include "carbus_analyzer_utils.h"

const char hex_asc_upper[] = "0123456789ABCDEF";

inline void put_hex_byte(uint8_t* buf, uint8_t byte) {
    buf[0] = hex_asc_upper_hi(byte);
    buf[1] = hex_asc_upper_lo(byte);
}

inline void _put_id(uint8_t* buf, int end_offset, uint32_t id) {
    /* build 3 (SFF) or 8 (EFF) digit CAN identifier */
    while(end_offset >= 0) {
        buf[end_offset--] = hex_asc_upper[id & 0xF];
        id >>= 4;
    }
}

unsigned char hexCharToByte(char hex)
{
    unsigned char result = 0;
    if (hex >= 0x30 && hex <= 0x39) {
        result = hex - 0x30;
    } else if (hex >= 0x41 && hex <= 0x46) {
        result = hex - 0x41 + 0x0A;
    } else if (hex >= 0x61 && hex <= 0x66) {
        result = hex - 0x61 + 0x0A;
    }
    return result;
}

uint8_t ascii2byte (uint8_t *val) {
    uint8_t temp = *val;
    if (temp > 0x60) temp -= 0x27;                // convert chars a-f
    else if (temp > 0x40) temp -= 0x07;           // convert chars A-F
    temp -= 0x30;                                 // convert chars 0-9
    return temp & 0x0F;
}

uint8_t nibble2ascii(uint8_t nibble) {
    uint8_t tmp = nibble & 0x0f;
    return tmp < 10 ? tmp + 48 : tmp + 55;
}
