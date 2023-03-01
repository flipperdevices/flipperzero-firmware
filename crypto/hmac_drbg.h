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

#ifndef __HMAC_DRBG_H__
#define __HMAC_DRBG_H__

#include <sha2.h>
#include <stdint.h>

// HMAC based Deterministic Random Bit Generator with SHA-256

typedef struct _HMAC_DRBG_CTX {
  uint32_t odig[SHA256_DIGEST_LENGTH / sizeof(uint32_t)];
  uint32_t idig[SHA256_DIGEST_LENGTH / sizeof(uint32_t)];
  uint32_t v[SHA256_BLOCK_LENGTH / sizeof(uint32_t)];
} HMAC_DRBG_CTX;

void hmac_drbg_init(HMAC_DRBG_CTX *ctx, const uint8_t *buf, size_t len,
                    const uint8_t *nonce, size_t nonce_len);
void hmac_drbg_reseed(HMAC_DRBG_CTX *ctx, const uint8_t *buf, size_t len,
                      const uint8_t *addin, size_t addin_len);
void hmac_drbg_generate(HMAC_DRBG_CTX *ctx, uint8_t *buf, size_t len);

#endif
