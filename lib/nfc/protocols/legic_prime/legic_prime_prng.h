/**
 * @file legic_prime_prng.h
 * @brief LEGIC Prime obfuscation PRNG (LFSR pair).
 *
 * Ported from the Proxmark3 project (common/legic_prng.c) under GPLv3.
 * See: https://github.com/RfidResearchGroup/proxmark3
 *
 * Both reader and tag run the same LFSR state machine in lock-step; the
 * keystream is XORed onto every transmitted and received bit. The IV is sent
 * in the clear (with a pre-init that produces an all-zero keystream).
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void legic_prime_prng_init(uint8_t iv);

void legic_prime_prng_forward(int count);

uint8_t legic_prime_prng_get_bit(void);

#ifdef __cplusplus
}
#endif
