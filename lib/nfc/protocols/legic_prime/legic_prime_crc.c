/*
 * LEGIC Prime CRCs.  The CRC-4 is derived from Proxmark3: crc_update() in
 * common/crc.c, with the parameters of armsrc/legicrf.c calc_crc4().
 *
 * Copyright (C) Proxmark3 contributors. See AUTHORS.md for details
 * (https://github.com/RfidResearchGroup/proxmark3).
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "legic_prime_crc.h"

#include <lib/bit_lib/bit_lib.h>

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
    // Proxmark3 CRC8Legic() reflects the init along with the input (0x55 ->
    // 0xAA) and reflects the result twice, which cancels out.
    return bit_lib_crc8(buf, size, 0x63, 0xAA, true, false, 0);
}
