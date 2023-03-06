/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 * Copyright (c) 2016 Alex Beregszaszi
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

#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "options.h"

#define BN_LIMBS 9
#define BN_BITS_PER_LIMB 29
#define BN_BASE (1u << BN_BITS_PER_LIMB)
#define BN_LIMB_MASK ((1u << BN_BITS_PER_LIMB) - 1)
#define BN_EXTRA_BITS (32 - BN_BITS_PER_LIMB)
#define BN_BITS_LAST_LIMB (256 - (BN_LIMBS - 1) * BN_BITS_PER_LIMB)

// Represents the number sum([val[i] * 2**(29*i) for i in range(9))
typedef struct {
  uint32_t val[BN_LIMBS];
} bignum256;

static inline uint32_t read_be(const uint8_t *data) {
  return (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) |
         (((uint32_t)data[2]) << 8) | (((uint32_t)data[3]));
}

static inline void write_be(uint8_t *data, uint32_t x) {
  data[0] = x >> 24;
  data[1] = x >> 16;
  data[2] = x >> 8;
  data[3] = x;
}

static inline uint32_t read_le(const uint8_t *data) {
  return (((uint32_t)data[3]) << 24) | (((uint32_t)data[2]) << 16) |
         (((uint32_t)data[1]) << 8) | (((uint32_t)data[0]));
}

static inline void write_le(uint8_t *data, uint32_t x) {
  data[3] = x >> 24;
  data[2] = x >> 16;
  data[1] = x >> 8;
  data[0] = x;
}

void bn_read_be(const uint8_t *in_number, bignum256 *out_number);
void bn_write_be(const bignum256 *in_number, uint8_t *out_number);
void bn_read_le(const uint8_t *in_number, bignum256 *out_number);
void bn_write_le(const bignum256 *in_number, uint8_t *out_number);
void bn_read_uint32(uint32_t in_number, bignum256 *out_number);
void bn_read_uint64(uint64_t in_number, bignum256 *out_number);
int bn_bitcount(const bignum256 *x);
unsigned int bn_digitcount(const bignum256 *x);
void bn_zero(bignum256 *x);
void bn_one(bignum256 *x);
int bn_is_zero(const bignum256 *x);
int bn_is_one(const bignum256 *x);
int bn_is_less(const bignum256 *x, const bignum256 *y);
int bn_is_equal(const bignum256 *x, const bignum256 *y);
void bn_cmov(bignum256 *res, volatile uint32_t cond, const bignum256 *truecase,
             const bignum256 *falsecase);
void bn_cnegate(volatile uint32_t cond, bignum256 *x, const bignum256 *prime);
void bn_lshift(bignum256 *x);
void bn_rshift(bignum256 *x);
void bn_setbit(bignum256 *x, uint16_t i);
void bn_clearbit(bignum256 *x, uint16_t i);
uint32_t bn_testbit(const bignum256 *x, uint16_t i);
void bn_xor(bignum256 *res, const bignum256 *x, const bignum256 *y);
void bn_mult_half(bignum256 *x, const bignum256 *prime);
void bn_mult_k(bignum256 *x, uint8_t k, const bignum256 *prime);
void bn_mod(bignum256 *x, const bignum256 *prime);
void bn_multiply(const bignum256 *k, bignum256 *x, const bignum256 *prime);
void bn_fast_mod(bignum256 *x, const bignum256 *prime);
void bn_power_mod(const bignum256 *x, const bignum256 *e,
                  const bignum256 *prime, bignum256 *res);
void bn_sqrt(bignum256 *x, const bignum256 *prime);
uint32_t inverse_mod_power_two(uint32_t a, uint32_t n);
void bn_divide_base(bignum256 *x, const bignum256 *prime);
void bn_normalize(bignum256 *x);
void bn_add(bignum256 *x, const bignum256 *y);
void bn_addmod(bignum256 *x, const bignum256 *y, const bignum256 *prime);
void bn_addi(bignum256 *x, uint32_t y);
void bn_subi(bignum256 *x, uint32_t y, const bignum256 *prime);
void bn_subtractmod(const bignum256 *x, const bignum256 *y, bignum256 *res,
                    const bignum256 *prime);
void bn_subtract(const bignum256 *x, const bignum256 *y, bignum256 *res);
void bn_long_division(bignum256 *x, uint32_t d, bignum256 *q, uint32_t *r);
void bn_divmod58(bignum256 *x, uint32_t *r);
void bn_divmod1000(bignum256 *x, uint32_t *r);
void bn_inverse(bignum256 *x, const bignum256 *prime);
size_t bn_format(const bignum256 *amount, const char *prefix,
                 const char *suffix, unsigned int decimals, int exponent,
                 bool trailing, char thousands, char *output,
                 size_t output_length);

// Returns (uint32_t) in_number
// Assumes in_number < 2**32
// Assumes in_number is normalized
static inline uint32_t bn_write_uint32(const bignum256 *in_number) {
  return in_number->val[0] | (in_number->val[1] << BN_BITS_PER_LIMB);
}

// Returns (uint64_t) in_number
// Assumes in_number < 2**64
// Assumes in_number is normalized
static inline uint64_t bn_write_uint64(const bignum256 *in_number) {
  uint64_t acc;
  acc = in_number->val[2];
  acc <<= BN_BITS_PER_LIMB;
  acc |= in_number->val[1];
  acc <<= BN_BITS_PER_LIMB;
  acc |= in_number->val[0];
  return acc;
}

// y = x
static inline void bn_copy(const bignum256 *x, bignum256 *y) { *y = *x; }

// Returns x % 2 == 0
static inline int bn_is_even(const bignum256 *x) {
  return (x->val[0] & 1) == 0;
}

// Returns x % 2 == 0
static inline int bn_is_odd(const bignum256 *x) { return (x->val[0] & 1) == 1; }

static inline size_t bn_format_uint64(uint64_t amount, const char *prefix,
                                      const char *suffix, unsigned int decimals,
                                      int exponent, bool trailing,
                                      char thousands, char *output,
                                      size_t output_length) {
  bignum256 bn_amount;
  bn_read_uint64(amount, &bn_amount);

  return bn_format(&bn_amount, prefix, suffix, decimals, exponent, trailing,
                   thousands, output, output_length);
}

static inline size_t bn_format_amount(uint64_t amount, const char *prefix,
                                      const char *suffix, unsigned int decimals,
                                      char *output, size_t output_length) {
  return bn_format_uint64(amount, prefix, suffix, decimals, 0, false, ',',
                          output, output_length);
}

#if USE_BN_PRINT
void bn_print(const bignum256 *x);
void bn_print_raw(const bignum256 *x);
#endif

#endif
