/**
 * @file legic_prime_prng.h
 * @brief LEGIC Prime obfuscation PRNG (LFSR pair).
 *
 * Ported from Proxmark3 common/legic_prng.c.
 * Copyright (C) Proxmark3 contributors. See AUTHORS.md for details
 * (https://github.com/RfidResearchGroup/proxmark3).
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Both reader and tag run the same LFSR state machine in lock-step; the
 * keystream is XORed onto every transmitted and received bit.  The IV itself
 * is sent in the clear and seeds the keystream: legic_prime_prng_init(iv).
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
