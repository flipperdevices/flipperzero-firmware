/**
 * Copyright (c) 2013-2021 SatoshiLabs
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __CARDANO_H__
#define __CARDANO_H__

#include <stdbool.h>
#include <stdint.h>
#include "bip32.h"
#include "options.h"

#if USE_CARDANO

#define CARDANO_SECRET_LENGTH 96
#define CARDANO_ICARUS_PBKDF2_ROUNDS 4096

extern const curve_info ed25519_cardano_info;

int hdnode_private_ckd_cardano(HDNode *inout, uint32_t i);

int secret_from_entropy_cardano_icarus(
    const uint8_t *pass, int pass_len, const uint8_t *entropy, int entropy_len,
    uint8_t secret_out[CARDANO_SECRET_LENGTH],
    void (*progress_callback)(uint32_t current, uint32_t total));
int secret_from_seed_cardano_ledger(const uint8_t *seed, int seed_len,
                                    uint8_t secret_out[CARDANO_SECRET_LENGTH]);
int secret_from_seed_cardano_slip23(const uint8_t *seed, int seed_len,
                                    uint8_t secret_out[CARDANO_SECRET_LENGTH]);

int hdnode_from_secret_cardano(const uint8_t secret[CARDANO_SECRET_LENGTH],
                               HDNode *out);

#endif  // USE_CARDANO

#endif  // __CARDANO_H__
