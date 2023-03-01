/**
 * Copyright (c) 2019 Andrew R. Kozlik
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

#include "hmac_drbg.h"
#include <string.h>
#include "memzero.h"
#include "sha2.h"

static void update_k(HMAC_DRBG_CTX *ctx, uint8_t domain, const uint8_t *data1,
                     size_t len1, const uint8_t *data2, size_t len2) {
  // Computes K = HMAC(K, V || domain || data1 || data 2).

  // First hash operation of HMAC.
  uint32_t h[SHA256_BLOCK_LENGTH / sizeof(uint32_t)] = {0};
  if (len1 + len2 == 0) {
    ctx->v[8] = 0x00800000;
    ctx->v[15] = (SHA256_BLOCK_LENGTH + SHA256_DIGEST_LENGTH + 1) * 8;
    sha256_Transform(ctx->idig, ctx->v, h);
    ctx->v[8] = 0x80000000;
    ctx->v[15] = (SHA256_BLOCK_LENGTH + SHA256_DIGEST_LENGTH) * 8;
  } else {
    SHA256_CTX sha_ctx = {0};
    memcpy(sha_ctx.state, ctx->idig, SHA256_DIGEST_LENGTH);
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH / sizeof(uint32_t); i++) {
#if BYTE_ORDER == LITTLE_ENDIAN
      REVERSE32(ctx->v[i], sha_ctx.buffer[i]);
#else
      sha_ctx.buffer[i] = ctx->v[i];
#endif
    }
    ((uint8_t *)sha_ctx.buffer)[SHA256_DIGEST_LENGTH] = domain;
    sha_ctx.bitcount = (SHA256_BLOCK_LENGTH + SHA256_DIGEST_LENGTH + 1) * 8;
    sha256_Update(&sha_ctx, data1, len1);
    sha256_Update(&sha_ctx, data2, len2);
    sha256_Final(&sha_ctx, (uint8_t *)h);
#if BYTE_ORDER == LITTLE_ENDIAN
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH / sizeof(uint32_t); i++)
      REVERSE32(h[i], h[i]);
#endif
  }

  // Second hash operation of HMAC.
  h[8] = 0x80000000;
  h[15] = (SHA256_BLOCK_LENGTH + SHA256_DIGEST_LENGTH) * 8;
  sha256_Transform(ctx->odig, h, h);

  // Precompute the inner digest and outer digest of K.
  h[8] = 0;
  h[15] = 0;
  for (size_t i = 0; i < SHA256_BLOCK_LENGTH / sizeof(uint32_t); i++) {
    h[i] ^= 0x36363636;
  }
  sha256_Transform(sha256_initial_hash_value, h, ctx->idig);

  for (size_t i = 0; i < SHA256_BLOCK_LENGTH / sizeof(uint32_t); i++) {
    h[i] = h[i] ^ 0x36363636 ^ 0x5c5c5c5c;
  }
  sha256_Transform(sha256_initial_hash_value, h, ctx->odig);
  memzero(h, sizeof(h));
}

static void update_v(HMAC_DRBG_CTX *ctx) {
  sha256_Transform(ctx->idig, ctx->v, ctx->v);
  sha256_Transform(ctx->odig, ctx->v, ctx->v);
}

void hmac_drbg_init(HMAC_DRBG_CTX *ctx, const uint8_t *entropy,
                    size_t entropy_len, const uint8_t *nonce,
                    size_t nonce_len) {
  uint32_t h[SHA256_BLOCK_LENGTH / sizeof(uint32_t)] = {0};

  // Precompute the inner digest and outer digest of K = 0x00 ... 0x00.
  memset(h, 0x36, sizeof(h));
  sha256_Transform(sha256_initial_hash_value, h, ctx->idig);
  memset(h, 0x5c, sizeof(h));
  sha256_Transform(sha256_initial_hash_value, h, ctx->odig);

  // Let V = 0x01 ... 0x01.
  memset(ctx->v, 1, SHA256_DIGEST_LENGTH);
  for (size_t i = 9; i < 15; i++) ctx->v[i] = 0;
  ctx->v[8] = 0x80000000;
  ctx->v[15] = (SHA256_BLOCK_LENGTH + SHA256_DIGEST_LENGTH) * 8;

  hmac_drbg_reseed(ctx, entropy, entropy_len, nonce, nonce_len);

  memzero(h, sizeof(h));
}

void hmac_drbg_reseed(HMAC_DRBG_CTX *ctx, const uint8_t *entropy, size_t len,
                      const uint8_t *addin, size_t addin_len) {
  update_k(ctx, 0, entropy, len, addin, addin_len);
  update_v(ctx);
  if (len == 0) return;
  update_k(ctx, 1, entropy, len, addin, addin_len);
  update_v(ctx);
}

void hmac_drbg_generate(HMAC_DRBG_CTX *ctx, uint8_t *buf, size_t len) {
  size_t i = 0;
  while (i < len) {
    update_v(ctx);
    for (size_t j = 0; j < 8 && i < len; j++) {
      uint32_t r = ctx->v[j];
      for (int k = 24; k >= 0 && i < len; k -= 8) {
        buf[i++] = (r >> k) & 0xFF;
      }
    }
  }
  update_k(ctx, 0, NULL, 0, NULL, 0);
  update_v(ctx);
}
