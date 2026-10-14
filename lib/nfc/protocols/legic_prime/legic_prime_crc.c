/*
 * LEGIC Prime CRC implementations.
 *
 * Logic copied from Proxmark3 (common/crc.c crc_update + crc_update2 +
 * armsrc/legicrf.c:313 init values). Stripped to the two specific CRCs we
 * need and inlined for clarity — we don't need the full generic CRC engine.
 */
#include "legic_prime_crc.h"

static uint8_t reflect8(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint8_t legic_prime_crc4(uint32_t cmd, uint8_t cmd_sz, uint8_t value) {
    // Equivalent to Proxmark crc_init(&legic_crc, 4, 0x19>>1, 0x05, 0)
    // followed by crc_update over (value << cmd_sz) | cmd, 8 + cmd_sz bits.
    // crc_update is the LSB-first form: shift state right, XOR poly if (oldstate^data)&1.
    const uint32_t poly = 0x19u >> 1; // 0x0C
    uint32_t state = 0x05u;
    uint32_t data = ((uint32_t)value << cmd_sz) | (cmd & ((1u << cmd_sz) - 1));
    int data_width = 8 + cmd_sz;
    for(int i = 0; i < data_width; i++) {
        uint32_t oldstate = state;
        state >>= 1;
        if((oldstate ^ data) & 1u) {
            state ^= poly;
        }
        data >>= 1;
    }
    return state & 0x0F;
}

uint8_t legic_prime_crc8(const uint8_t* buf, size_t size) {
    // Equivalent to Proxmark CRC8Legic: poly=0x63, init=0x55, refin=true,
    // refout=true, followed by reflect8() on the result. The two reflections
    // on output cancel, so we compute the MSB-first form on reflected input
    // bytes and don't reflect the output.
    const uint8_t poly = 0x63u;
    uint8_t state = reflect8(0x55u);
    for(size_t i = 0; i < size; i++) {
        state ^= reflect8(buf[i]);
        for(int bit = 0; bit < 8; bit++) {
            if(state & 0x80u) {
                state = (uint8_t)((state << 1) ^ poly);
            } else {
                state = (uint8_t)(state << 1);
            }
        }
    }
    return state;
}
