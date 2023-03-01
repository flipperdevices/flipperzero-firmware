/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "chacha_drbg.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "chacha20poly1305/ecrypt-portable.h"
#include "memzero.h"
#include "sha2.h"

#define CHACHA_DRBG_KEY_LENGTH 32
#define CHACHA_DRBG_COUNTER_LENGTH 8
#define CHACHA_DRBG_IV_LENGTH 8
#define CHACHA_DRBG_SEED_LENGTH \
  (CHACHA_DRBG_KEY_LENGTH + CHACHA_DRBG_COUNTER_LENGTH + CHACHA_DRBG_IV_LENGTH)

#define MAX(a, b) (a) > (b) ? (a) : (b)

static void derivation_function(const uint8_t *input1, size_t input1_length,
                                const uint8_t *input2, size_t input2_length,
                                uint8_t *output, size_t output_length) {
  // Implementation of Hash_df from NIST SP 800-90A
  uint32_t block_count = (output_length - 1) / SHA256_DIGEST_LENGTH + 1;
  size_t partial_block_length = output_length % SHA256_DIGEST_LENGTH;
  assert(block_count <= 255);

  uint32_t output_length_bits = output_length * 8;
#if BYTE_ORDER == LITTLE_ENDIAN
  REVERSE32(output_length_bits, output_length_bits);
#endif

  SHA256_CTX ctx = {0};

  for (uint8_t counter = 1; counter <= block_count; counter++) {
    sha256_Init(&ctx);
    sha256_Update(&ctx, &counter, sizeof(counter));
    sha256_Update(&ctx, (uint8_t *)&output_length_bits,
                  sizeof(output_length_bits));
    sha256_Update(&ctx, input1, input1_length);
    sha256_Update(&ctx, input2, input2_length);

    if (counter != block_count || partial_block_length == 0) {
      sha256_Final(&ctx, output);
      output += SHA256_DIGEST_LENGTH;
    } else {  // last block is partial
      uint8_t digest[SHA256_DIGEST_LENGTH] = {0};
      sha256_Final(&ctx, digest);
      memcpy(output, digest, partial_block_length);
      memzero(digest, sizeof(digest));
    }
  }

  memzero(&ctx, sizeof(ctx));
}

void chacha_drbg_init(CHACHA_DRBG_CTX *ctx, const uint8_t *entropy,
                      size_t entropy_length, const uint8_t *nonce,
                      size_t nonce_length) {
  uint8_t buffer[MAX(CHACHA_DRBG_KEY_LENGTH, CHACHA_DRBG_IV_LENGTH)] = {0};
  ECRYPT_keysetup(&ctx->chacha_ctx, buffer, CHACHA_DRBG_KEY_LENGTH * 8,
                  CHACHA_DRBG_IV_LENGTH * 8);
  ECRYPT_ivsetup(&ctx->chacha_ctx, buffer);

  chacha_drbg_reseed(ctx, entropy, entropy_length, nonce, nonce_length);
}

static void chacha_drbg_update(CHACHA_DRBG_CTX *ctx,
                               const uint8_t data[CHACHA_DRBG_SEED_LENGTH]) {
  uint8_t seed[CHACHA_DRBG_SEED_LENGTH] = {0};

  if (data)
    ECRYPT_encrypt_bytes(&ctx->chacha_ctx, data, seed, CHACHA_DRBG_SEED_LENGTH);
  else
    ECRYPT_keystream_bytes(&ctx->chacha_ctx, seed, CHACHA_DRBG_SEED_LENGTH);

  ECRYPT_keysetup(&ctx->chacha_ctx, seed, CHACHA_DRBG_KEY_LENGTH * 8,
                  CHACHA_DRBG_IV_LENGTH * 8);

  ECRYPT_ivsetup(&ctx->chacha_ctx,
                 seed + CHACHA_DRBG_KEY_LENGTH + CHACHA_DRBG_COUNTER_LENGTH);

  ECRYPT_ctrsetup(&ctx->chacha_ctx, seed + CHACHA_DRBG_KEY_LENGTH);

  memzero(seed, sizeof(seed));
}

void chacha_drbg_generate(CHACHA_DRBG_CTX *ctx, uint8_t *output,
                          size_t output_length) {
  assert(output_length < 65536);
  assert(ctx->reseed_counter + 1 != 0);

  ECRYPT_keystream_bytes(&ctx->chacha_ctx, output, output_length);
  chacha_drbg_update(ctx, NULL);
  ctx->reseed_counter++;
}

void chacha_drbg_reseed(CHACHA_DRBG_CTX *ctx, const uint8_t *entropy,
                        size_t entropy_length, const uint8_t *additional_input,
                        size_t additional_input_length) {
  uint8_t seed[CHACHA_DRBG_SEED_LENGTH] = {0};
  derivation_function(entropy, entropy_length, additional_input,
                      additional_input_length, seed, sizeof(seed));
  chacha_drbg_update(ctx, seed);
  memzero(seed, sizeof(seed));

  ctx->reseed_counter = 1;
}
