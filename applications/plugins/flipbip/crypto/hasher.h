/**
 * Copyright (c) 2017 Saleem Rashid
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

#ifndef __HASHER_H__
#define __HASHER_H__

#include <stddef.h>
#include <stdint.h>

#include "blake256.h"
#include "blake2b.h"
#include "groestl.h"
#include "sha2.h"
#include "sha3.h"

#define HASHER_DIGEST_LENGTH 32

typedef enum {
  HASHER_SHA2,
  HASHER_SHA2D,
  HASHER_SHA2_RIPEMD,
  HASHER_SHA2_TAPSIGHASH,

  HASHER_SHA3,
#if USE_KECCAK
  HASHER_SHA3K,
#endif

  HASHER_BLAKE,
  HASHER_BLAKED,
  HASHER_BLAKE_RIPEMD,

  HASHER_GROESTLD_TRUNC, /* Double Groestl512 hasher truncated to 256 bits */

  HASHER_BLAKE2B,
  HASHER_BLAKE2B_PERSONAL,
} HasherType;

typedef struct {
  HasherType type;

  union {
    SHA256_CTX sha2;         // for HASHER_SHA2{,D}
    SHA3_CTX sha3;           // for HASHER_SHA3{,K}
    BLAKE256_CTX blake;      // for HASHER_BLAKE{,D}
    GROESTL512_CTX groestl;  // for HASHER_GROESTLD_TRUNC
    BLAKE2B_CTX blake2b;     // for HASHER_BLAKE2B{,_PERSONAL}
  } ctx;

  const void *param;
  uint32_t param_size;
} Hasher;

void hasher_InitParam(Hasher *hasher, HasherType type, const void *param,
                      uint32_t param_size);
void hasher_Init(Hasher *hasher, HasherType type);
void hasher_Reset(Hasher *hasher);
void hasher_Update(Hasher *hasher, const uint8_t *data, size_t length);
void hasher_Final(Hasher *hasher, uint8_t hash[HASHER_DIGEST_LENGTH]);

void hasher_Raw(HasherType type, const uint8_t *data, size_t length,
                uint8_t hash[HASHER_DIGEST_LENGTH]);

#endif
