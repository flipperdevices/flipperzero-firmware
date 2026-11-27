/*
 * Two-LFSR muxed keystream used by LEGIC Prime to obfuscate frames, ported
 * from Proxmark3 common/legic_prng.c.  The step counter behind
 * legic_prng_count() is left out: nothing here reads it.
 *
 * Copyright (C) Proxmark3 contributors. See AUTHORS.md for details
 * (https://github.com/RfidResearchGroup/proxmark3).
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "legic_prime_prng.h"

static struct {
    uint8_t a; // 7-bit LFSR
    uint8_t b; // 8-bit LFSR
} lfsr;

void legic_prime_prng_init(uint8_t iv) {
    lfsr.a = iv;
    // As in Proxmark3, iv == 0 gives an all-zero keystream.
    lfsr.b = 0;
    if(iv) lfsr.b = (iv << 1) | 1;
}

void legic_prime_prng_forward(int count) {
    while(count-- > 0) {
        lfsr.a = (lfsr.a >> 1 | (lfsr.a ^ lfsr.a >> 6) << 6) & 0x7F;
        lfsr.b = lfsr.b >> 1 | (lfsr.b ^ lfsr.b >> 2 ^ lfsr.b >> 3 ^ lfsr.b >> 7) << 7;
    }
}

uint8_t legic_prime_prng_get_bit(void) {
    uint8_t idx = 7 - ((lfsr.a & 4) | (lfsr.a >> 2 & 2) | (lfsr.a >> 4 & 1));
    return lfsr.b >> idx & 1;
}
