// Copyright (c) 2014-2018, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote
// developers

#include "base58.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "../base58.h"
#include "../byte_order.h"
#include "int-util.h"
#include "sha2.h"

const size_t alphabet_size = 58;  // sizeof(b58digits_ordered) - 1;
const size_t full_encoded_block_size = 11;
const size_t encoded_block_sizes[] = {
    0, 2, 3, 5, 6, 7, 9, 10, full_encoded_block_size};
const size_t full_block_size =
    sizeof(encoded_block_sizes) / sizeof(encoded_block_sizes[0]) - 1;
const size_t addr_checksum_size = 4;
const size_t max_bin_data_size = 72;
const int decoded_block_sizes[] = {0, -1, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8};
#define reverse_alphabet(letter) ((int8_t)b58digits_map[(int)letter])

uint64_t uint_8be_to_64(const uint8_t *data, size_t size) {
  assert(1 <= size && size <= sizeof(uint64_t));

  uint64_t res = 0;
  switch (9 - size) {
    case 1:
      res |= *data++; /* FALLTHRU */
    case 2:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 3:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 4:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 5:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 6:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 7:
      res <<= 8;
      res |= *data++; /* FALLTHRU */
    case 8:
      res <<= 8;
      res |= *data;
      break;
    default:
      assert(false);
  }

  return res;
}

void uint_64_to_8be(uint64_t num, size_t size, uint8_t *data) {
  assert(1 <= size && size <= sizeof(uint64_t));

#if BYTE_ORDER == LITTLE_ENDIAN
  uint64_t num_be = SWAP64(num);
#else
  uint64_t num_be = num;
#endif
  memcpy(data, (uint8_t *)(&num_be) + sizeof(uint64_t) - size, size);
}

void encode_block(const char *block, size_t size, char *res) {
  assert(1 <= size && size <= full_block_size);

  uint64_t num = uint_8be_to_64((uint8_t *)(block), size);
  int i = ((int)(encoded_block_sizes[size])) - 1;
  while (0 <= i) {
    uint64_t remainder = num % alphabet_size;
    num /= alphabet_size;
    res[i] = b58digits_ordered[remainder];
    --i;
  }
}

bool decode_block(const char *block, size_t size, char *res) {
  assert(1 <= size && size <= full_encoded_block_size);

  int res_size = decoded_block_sizes[size];
  if (res_size <= 0) {
    return false;  // Invalid block size
  }

  uint64_t res_num = 0;
  uint64_t order = 1;
  for (size_t i = size - 1; i < size; --i) {
    if (block[i] & 0x80) {
      return false;  // Invalid symbol
    }
    int digit = reverse_alphabet(block[i]);
    if (digit < 0) {
      return false;  // Invalid symbol
    }

    uint64_t product_hi = 0;
    uint64_t tmp = res_num + mul128(order, (uint64_t)digit, &product_hi);
    if (tmp < res_num || 0 != product_hi) {
      return false;  // Overflow
    }

    res_num = tmp;
    // The original code comment for the order multiplication says
    // "Never overflows, 58^10 < 2^64"
    // This is incorrect since it overflows on the 11th iteration
    // However, there is no negative impact since the result is unused
    order *= alphabet_size;
  }

  if ((size_t)res_size < full_block_size &&
      (UINT64_C(1) << (8 * res_size)) <= res_num)
    return false;  // Overflow

  uint_64_to_8be(res_num, res_size, (uint8_t *)(res));

  return true;
}

bool xmr_base58_encode(char *b58, size_t *b58sz, const void *data,
                       size_t binsz) {
  if (binsz == 0) {
    if (b58sz) {
      *b58sz = 0;
    }
    return true;
  }

  const char *data_bin = data;
  size_t full_block_count = binsz / full_block_size;
  size_t last_block_size = binsz % full_block_size;
  size_t res_size = full_block_count * full_encoded_block_size +
                    encoded_block_sizes[last_block_size];

  if (b58sz) {
    if (res_size > *b58sz) {
      return false;
    }
    *b58sz = res_size;
  }

  for (size_t i = 0; i < full_block_count; ++i) {
    encode_block(data_bin + i * full_block_size, full_block_size,
                 b58 + i * full_encoded_block_size);
  }

  if (0 < last_block_size) {
    encode_block(data_bin + full_block_count * full_block_size, last_block_size,
                 b58 + full_block_count * full_encoded_block_size);
  }

  return true;
}

bool xmr_base58_decode(const char *b58, size_t b58sz, void *data,
                       size_t *binsz) {
  if (b58sz == 0) {
    *binsz = 0;
    return true;
  }

  size_t full_block_count = b58sz / full_encoded_block_size;
  size_t last_block_size = b58sz % full_encoded_block_size;
  int last_block_decoded_size = decoded_block_sizes[last_block_size];
  if (last_block_decoded_size < 0) {
    *binsz = 0;
    return false;  // Invalid enc length
  }

  size_t data_size =
      full_block_count * full_block_size + last_block_decoded_size;
  if (*binsz < data_size) {
    *binsz = 0;
    return false;
  }

  char *data_bin = data;
  for (size_t i = 0; i < full_block_count; ++i) {
    if (!decode_block(b58 + i * full_encoded_block_size,
                      full_encoded_block_size,
                      data_bin + i * full_block_size)) {
      *binsz = 0;
      return false;
    }
  }

  if (0 < last_block_size) {
    if (!decode_block(b58 + full_block_count * full_encoded_block_size,
                      last_block_size,
                      data_bin + full_block_count * full_block_size)) {
      *binsz = 0;
      return false;
    }
  }

  *binsz = data_size;
  return true;
}

int xmr_base58_addr_encode_check(uint64_t tag, const uint8_t *data,
                                 size_t binsz, char *b58, size_t b58sz) {
  if (binsz > max_bin_data_size || tag > 127) {  // tag varint
    return false;
  }

  size_t b58size = b58sz;
  uint8_t buf[(binsz + 1) + HASHER_DIGEST_LENGTH];
  memset(buf, 0, sizeof(buf));
  uint8_t *hash = buf + binsz + 1;
  buf[0] = (uint8_t)tag;
  memcpy(buf + 1, data, binsz);
  hasher_Raw(HASHER_SHA3K, buf, binsz + 1, hash);

  bool r =
      xmr_base58_encode(b58, &b58size, buf, binsz + 1 + addr_checksum_size);
  return (int)(!r ? 0 : b58size);
}

int xmr_base58_addr_decode_check(const char *addr, size_t sz, uint64_t *tag,
                                 void *data, size_t datalen) {
  size_t buflen = 1 + max_bin_data_size + addr_checksum_size;
  uint8_t buf[buflen];
  memset(buf, 0, sizeof(buf));
  uint8_t hash[HASHER_DIGEST_LENGTH] = {0};

  if (!xmr_base58_decode(addr, sz, buf, &buflen)) {
    return 0;
  }

  if (buflen <= addr_checksum_size + 1) {
    return 0;
  }

  size_t res_size = buflen - addr_checksum_size - 1;
  if (datalen < res_size) {
    return 0;
  }

  hasher_Raw(HASHER_SHA3K, buf, buflen - addr_checksum_size, hash);
  if (memcmp(hash, buf + buflen - addr_checksum_size, addr_checksum_size) !=
      0) {
    return 0;
  }

  *tag = buf[0];
  if (*tag > 127) {
    return false;  // varint
  }

  memcpy(data, buf + 1, res_size);
  return (int)res_size;
}
