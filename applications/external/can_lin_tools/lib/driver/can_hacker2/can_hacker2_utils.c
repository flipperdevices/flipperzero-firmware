#include "can_hacker2_utils.h"

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
