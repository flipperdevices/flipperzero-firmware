/*
 * Ported verbatim from Proxmark3 common/legic_prng.c (GPLv3).
 * Two-LFSR muxed keystream used by LEGIC Prime to obfuscate frames.
 */
#include "legic_prime_prng.h"

static struct {
    uint8_t a; // 7-bit LFSR
    uint8_t b; // 8-bit LFSR
    uint32_t c; // step counter
} lfsr;

void legic_prime_prng_init(uint8_t iv) {
    lfsr.a = iv;
    // Special case: iv==0 yields all-zero keystream (used for IV TX itself).
    lfsr.b = 0;
    lfsr.c = 0;
    if(iv) lfsr.b = (iv << 1) | 1;
}

void legic_prime_prng_forward(int count) {
    if(count == 0) return;

    lfsr.c += count;
    while(count--) {
        lfsr.a = (lfsr.a >> 1 | (lfsr.a ^ lfsr.a >> 6) << 6) & 0x7F;
        lfsr.b = lfsr.b >> 1 | (lfsr.b ^ lfsr.b >> 2 ^ lfsr.b >> 3 ^ lfsr.b >> 7) << 7;
    }
}

uint8_t legic_prime_prng_get_bit(void) {
    uint8_t idx = 7 - ((lfsr.a & 4) | (lfsr.a >> 2 & 2) | (lfsr.a >> 4 & 1));
    return lfsr.b >> idx & 1;
}
