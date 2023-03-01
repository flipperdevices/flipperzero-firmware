/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 * Copyright (c)      2015 Jochen Hoenicke
 * Copyright (c)      2016 Alex Beregszaszi
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

#include "bignum.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memzero.h"
#include "script.h"

/*
 This library implements 256-bit numbers arithmetic.

 An unsigned 256-bit number is represented by a bignum256 structure, that is an
 array of nine 32-bit values called limbs. Limbs are digits of the number in
 the base 2**29 representation in the little endian order. This means that
   bignum256 x;
 represents the value
   sum([x[i] * 2**(29*i) for i in range(9)).

 A limb of a bignum256 is *normalized* iff it's less than 2**29.
 A bignum256 is *normalized* iff every its limb is normalized.
 A number is *fully reduced modulo p* iff it is less than p.
 A number is *partly reduced modulo p* iff is is less than 2*p.
 The number p is usually a prime number such that 2^256 - 2^224 <= p <= 2^256.

 All functions except bn_fast_mod expect that all their bignum256 inputs are
 normalized. (The function bn_fast_mod allows the input number to have the
 most significant limb unnormalized). All bignum256 outputs of all functions
 are guaranteed to be  normalized.

 A number can be partly reduced with bn_fast_mod, a partly reduced number can
 be fully reduced with bn_mod.

 A function has *constant control flow with regard to its argument* iff the
 order in which instructions of the function are executed doesn't depend on the
 value of the argument.
 A function has *constant memory access flow with regard to its argument* iff
 the memory addresses that are acessed and the order in which they are accessed
 don't depend on the value of the argument.
 A function *has contant control (memory access) flow* iff it has constant
 control (memory access) flow with regard to all its arguments.

 The following function has contant control flow with regard to its arugment
 n, however is doesn't have constant memory access flow with regard to it:
 void (int n, int *a) }
   a[0] = 0;
   a[n] = 0; // memory address reveals the value of n
 }

 Unless stated otherwise all functions are supposed to have both constant
 control flow and constant memory access flow.
 */

#define BN_MAX_DECIMAL_DIGITS \
  79  // floor(log(2**(LIMBS * BITS_PER_LIMB), 10)) + 1

// out_number = (bignum256) in_number
// Assumes in_number is a raw bigendian 256-bit number
// Guarantees out_number is normalized
void bn_read_be(const uint8_t *in_number, bignum256 *out_number) {
  uint32_t temp = 0;

  for (int i = 0; i < BN_LIMBS - 1; i++) {
    uint32_t limb = read_be(in_number + (BN_LIMBS - 2 - i) * 4);

    temp |= limb << (BN_EXTRA_BITS * i);
    out_number->val[i] = temp & BN_LIMB_MASK;

    temp = limb >> (32 - BN_EXTRA_BITS * (i + 1));
  }

  out_number->val[BN_LIMBS - 1] = temp;
}

// out_number = (256BE) in_number
// Assumes in_number < 2**256
// Guarantess out_number is a raw bigendian 256-bit number
void bn_write_be(const bignum256 *in_number, uint8_t *out_number) {
  uint32_t temp = in_number->val[BN_LIMBS - 1];
  for (int i = BN_LIMBS - 2; i >= 0; i--) {
    uint32_t limb = in_number->val[i];

    temp = (temp << (BN_BITS_PER_LIMB - BN_EXTRA_BITS * i)) |
           (limb >> (BN_EXTRA_BITS * i));
    write_be(out_number + (BN_LIMBS - 2 - i) * 4, temp);

    temp = limb;
  }
}

// out_number = (bignum256) in_number
// Assumes in_number is a raw little endian 256-bit number
// Guarantees out_number is normalized
void bn_read_le(const uint8_t *in_number, bignum256 *out_number) {
  uint32_t temp = 0;
  for (int i = 0; i < BN_LIMBS - 1; i++) {
    uint32_t limb = read_le(in_number + i * 4);

    temp |= limb << (BN_EXTRA_BITS * i);
    out_number->val[i] = temp & BN_LIMB_MASK;
    temp = limb >> (32 - BN_EXTRA_BITS * (i + 1));
  }

  out_number->val[BN_LIMBS - 1] = temp;
}

// out_number = (256LE) in_number
// Assumes in_number < 2**256
// Guarantess out_number is a raw little endian 256-bit number
void bn_write_le(const bignum256 *in_number, uint8_t *out_number) {
  uint32_t temp = in_number->val[BN_LIMBS - 1];

  for (int i = BN_LIMBS - 2; i >= 0; i--) {
    uint32_t limb = in_number->val[i];
    temp = (temp << (BN_BITS_PER_LIMB - BN_EXTRA_BITS * i)) |
           (limb >> (BN_EXTRA_BITS * i));
    write_le(out_number + i * 4, temp);
    temp = limb;
  }
}

// out_number = (bignum256) in_number
// Guarantees out_number is normalized
void bn_read_uint32(uint32_t in_number, bignum256 *out_number) {
  out_number->val[0] = in_number & BN_LIMB_MASK;
  out_number->val[1] = in_number >> BN_BITS_PER_LIMB;
  for (uint32_t i = 2; i < BN_LIMBS; i++) out_number->val[i] = 0;
}

// out_number = (bignum256) in_number
// Guarantees out_number is normalized
void bn_read_uint64(uint64_t in_number, bignum256 *out_number) {
  out_number->val[0] = in_number & BN_LIMB_MASK;
  out_number->val[1] = (in_number >>= BN_BITS_PER_LIMB) & BN_LIMB_MASK;
  out_number->val[2] = in_number >> BN_BITS_PER_LIMB;
  for (uint32_t i = 3; i < BN_LIMBS; i++) out_number->val[i] = 0;
}

// Returns the bitsize of x
// Assumes x is normalized
// The function doesn't have neither constant control flow nor constant memory
//   access flow
int bn_bitcount(const bignum256 *x) {
  for (int i = BN_LIMBS - 1; i >= 0; i--) {
    uint32_t limb = x->val[i];
    if (limb != 0) {
      // __builtin_clz returns the number of leading zero bits starting at the
      // most significant bit position
      return i * BN_BITS_PER_LIMB + (32 - __builtin_clz(limb));
    }
  }
  return 0;
}

// Returns the number of decimal digits of x; if x is 0, returns 1
// Assumes x is normalized
// The function doesn't have neither constant control flow nor constant memory
//   access flow
unsigned int bn_digitcount(const bignum256 *x) {
  bignum256 val = {0};
  bn_copy(x, &val);

  unsigned int digits = 1;
  for (unsigned int i = 0; i < BN_MAX_DECIMAL_DIGITS; i += 3) {
    uint32_t limb = 0;

    bn_divmod1000(&val, &limb);

    if (limb >= 100) {
      digits = i + 3;
    } else if (limb >= 10) {
      digits = i + 2;
    } else if (limb >= 1) {
      digits = i + 1;
    }
  }

  memzero(&val, sizeof(val));

  return digits;
}

// x = 0
// Guarantees x is normalized
void bn_zero(bignum256 *x) {
  for (int i = 0; i < BN_LIMBS; i++) {
    x->val[i] = 0;
  }
}

// x = 1
// Guarantees x is normalized
void bn_one(bignum256 *x) {
  x->val[0] = 1;
  for (int i = 1; i < BN_LIMBS; i++) {
    x->val[i] = 0;
  }
}

// Returns x == 0
// Assumes x is normalized
int bn_is_zero(const bignum256 *x) {
  uint32_t result = 0;
  for (int i = 0; i < BN_LIMBS; i++) {
    result |= x->val[i];
  }
  return !result;
}

// Returns x == 1
// Assumes x is normalized
int bn_is_one(const bignum256 *x) {
  uint32_t result = x->val[0] ^ 1;
  for (int i = 1; i < BN_LIMBS; i++) {
    result |= x->val[i];
  }
  return !result;
}

// Returns x < y
// Assumes x, y are normalized
int bn_is_less(const bignum256 *x, const bignum256 *y) {
  uint32_t res1 = 0;
  uint32_t res2 = 0;
  for (int i = BN_LIMBS - 1; i >= 0; i--) {
    res1 = (res1 << 1) | (x->val[i] < y->val[i]);
    res2 = (res2 << 1) | (x->val[i] > y->val[i]);
  }
  return res1 > res2;
}

// Returns x == y
// Assumes x, y are normalized
int bn_is_equal(const bignum256 *x, const bignum256 *y) {
  uint32_t result = 0;
  for (int i = 0; i < BN_LIMBS; i++) {
    result |= x->val[i] ^ y->val[i];
  }
  return !result;
}

// res = cond if truecase else falsecase
// Assumes cond is either 0 or 1
// Works properly even if &res == &truecase or &res == &falsecase or
//   &truecase == &falsecase or &res == &truecase == &falsecase
void bn_cmov(bignum256 *res, volatile uint32_t cond, const bignum256 *truecase,
             const bignum256 *falsecase) {
  // Intentional use of bitwise OR operator to ensure constant-time
  assert((int)(cond == 1) | (int)(cond == 0));

  uint32_t tmask = -cond;   // tmask = 0xFFFFFFFF if cond else 0x00000000
  uint32_t fmask = ~tmask;  // fmask = 0x00000000 if cond else 0xFFFFFFFF

  for (int i = 0; i < BN_LIMBS; i++) {
    res->val[i] = (truecase->val[i] & tmask) | (falsecase->val[i] & fmask);
  }
}

// x = -x % prime if cond else x,
// Explicitly x = (3 * prime - x if x > prime else 2 * prime - x) if cond else
//   else (x if x > prime else x + prime)
// Assumes x is normalized and partly reduced
// Assumes cond is either 1 or 0
// Guarantees x is normalized
// Assumes prime is normalized and
//   0 < prime < 2**260 == 2**(BITS_PER_LIMB * LIMBS - 1)
void bn_cnegate(volatile uint32_t cond, bignum256 *x, const bignum256 *prime) {
  // Intentional use of bitwise OR operator to ensure constant time
  assert((int)(cond == 1) | (int)(cond == 0));

  uint32_t tmask = -cond;   // tmask = 0xFFFFFFFF if cond else 0x00000000
  uint32_t fmask = ~tmask;  // fmask = 0x00000000 if cond else 0xFFFFFFFF

  bn_mod(x, prime);
  // x < prime

  uint32_t acc1 = 1;
  uint32_t acc2 = 0;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc1 += (BN_BASE - 1) + 2 * prime->val[i] - x->val[i];
    // acc1 neither overflows 32 bits nor underflows 0
    // Proof:
    //   acc1 + (BASE - 1) + 2 * prime[i] - x[i]
    //     >= (BASE - 1) - x >= (2**BITS_PER_LIMB - 1) - (2**BITS_PER_LIMB - 1)
    //     == 0
    //   acc1 + (BASE - 1) + 2 * prime[i] - x[i]
    //     <= acc1 + (BASE - 1) + 2 * prime[i]
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + 2 * (2**BITS_PER_LIMB - 1) +
    //       (2**BITS_PER_LIMB - 1)
    //     == 7 + 3 * 2**29 < 2**32

    acc2 += prime->val[i] + x->val[i];
    // acc2 doesn't overflow 32 bits
    // Proof:
    //   acc2 + prime[i] + x[i]
    //     <= 2**(32 - BITS_PER_LIMB) - 1 + 2 * (2**BITS_PER_LIMB - 1)
    //     == 2**(32 - BITS_PER_LIMB) + 2**(BITS_PER_LIMB + 1) - 2
    //     == 2**30 + 5 < 2**32

    // x = acc1 & LIMB_MASK if cond else acc2 & LIMB_MASK
    x->val[i] = ((acc1 & tmask) | (acc2 & fmask)) & BN_LIMB_MASK;

    acc1 >>= BN_BITS_PER_LIMB;
    // acc1 <= 7 == 2**(32 - BITS_PER_LIMB) - 1
    // acc1 == 2**(BITS_PER_LIMB * (i + 1)) + 2 * prime[:i + 1] - x[:i + 1]
    //   >> BITS_PER_LIMB * (i + 1)

    acc2 >>= BN_BITS_PER_LIMB;
    // acc2 <= 7 == 2**(32 - BITS_PER_LIMB) - 1
    // acc2 == prime[:i + 1] + x[:i + 1] >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc1 == 1); // assert prime <= 2**260
  // assert(acc2 == 0);

  // clang-format off
  // acc1 == 1
  // Proof:
  //   acc1 == 2**(BITS_PER_LIMB * LIMBS) + 2 * prime[:LIMBS] - x[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + 2 * prime - x >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) + 2 * prime >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) + 2 * (2**(BITS_PER_LIMB * LIMBS - 1) - 1) >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) + 2**(BITS_PER_LIMB * LIMBS) - 2 >> BITS_PER_LIMB * LIMBS
  //     == 1

  //   acc1 == 2**(BITS_PER_LIMB * LIMBS) + 2 * prime[:LIMBS] - x[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + 2 * prime - x >> BITS_PER_LIMB * LIMBS
  //     >= 2**(BITS_PER_LIMB * LIMBS) + 0 >> BITS_PER_LIMB * LIMBS
  //     == 1

  // acc2 == 0
  // Proof:
  //   acc2 == prime[:LIMBS] + x[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == prime + x >> BITS_PER_LIMB * LIMBS
  //     <= 2 * prime - 1 >> BITS_PER_LIMB * LIMBS
  //     <= 2 * (2**(BITS_PER_LIMB * LIMBS - 1) - 1) - 1 >> 261
  //     == 2**(BITS_PER_LIMB * LIMBS) - 3 >> BITS_PER_LIMB * LIMBS
  //     == 0
  // clang-format on
}

// x <<= 1
// Assumes x is normalized, x < 2**260 == 2**(LIMBS*BITS_PER_LIMB - 1)
// Guarantees x is normalized
void bn_lshift(bignum256 *x) {
  for (int i = BN_LIMBS - 1; i > 0; i--) {
    x->val[i] = ((x->val[i] << 1) & BN_LIMB_MASK) |
                (x->val[i - 1] >> (BN_BITS_PER_LIMB - 1));
  }
  x->val[0] = (x->val[0] << 1) & BN_LIMB_MASK;
}

// x >>= 1, i.e. x = floor(x/2)
// Assumes x is normalized
// Guarantees x is normalized
// If x is partly reduced (fully reduced) modulo prime,
//   guarantess x will be partly reduced (fully reduced) modulo prime
void bn_rshift(bignum256 *x) {
  for (int i = 0; i < BN_LIMBS - 1; i++) {
    x->val[i] =
        (x->val[i] >> 1) | ((x->val[i + 1] & 1) << (BN_BITS_PER_LIMB - 1));
  }
  x->val[BN_LIMBS - 1] >>= 1;
}

// Sets i-th least significant bit (counting from zero)
// Assumes x is normalized and 0 <= i < 261 == LIMBS*BITS_PER_LIMB
// Guarantees x is normalized
// The function has constant control flow but not constant memory access flow
//   with regard to i
void bn_setbit(bignum256 *x, uint16_t i) {
  assert(i < BN_LIMBS * BN_BITS_PER_LIMB);
  x->val[i / BN_BITS_PER_LIMB] |= (1u << (i % BN_BITS_PER_LIMB));
}

// clears i-th least significant bit (counting from zero)
// Assumes x is normalized and 0 <= i < 261 == LIMBS*BITS_PER_LIMB
// Guarantees x is normalized
// The function has constant control flow but not constant memory access flow
//   with regard to i
void bn_clearbit(bignum256 *x, uint16_t i) {
  assert(i < BN_LIMBS * BN_BITS_PER_LIMB);
  x->val[i / BN_BITS_PER_LIMB] &= ~(1u << (i % BN_BITS_PER_LIMB));
}

// returns i-th least significant bit (counting from zero)
// Assumes x is normalized and 0 <= i < 261 == LIMBS*BITS_PER_LIMB
// The function has constant control flow but not constant memory access flow
//   with regard to i
uint32_t bn_testbit(const bignum256 *x, uint16_t i) {
  assert(i < BN_LIMBS * BN_BITS_PER_LIMB);
  return (x->val[i / BN_BITS_PER_LIMB] >> (i % BN_BITS_PER_LIMB)) & 1;
}

// res = x ^ y
// Assumes x, y are normalized
// Guarantees res is normalized
// Works properly even if &res == &x or &res == &y or &res == &x == &y
void bn_xor(bignum256 *res, const bignum256 *x, const bignum256 *y) {
  for (int i = 0; i < BN_LIMBS; i++) {
    res->val[i] = x->val[i] ^ y->val[i];
  }
}

// x = x / 2 % prime
// Explicitly x = x / 2 if is_even(x) else (x + prime) / 2
// Assumes x is normalized, x + prime < 261 == LIMBS * BITS_PER_LIMB
// Guarantees x is normalized
// If x is partly reduced (fully reduced) modulo prime,
//   guarantess x will be partly reduced (fully reduced) modulo prime
// Assumes prime is an odd number and normalized
void bn_mult_half(bignum256 *x, const bignum256 *prime) {
  // x = x / 2 if is_even(x) else (x + prime) / 2

  uint32_t x_is_odd_mask =
      -(x->val[0] & 1);  // x_is_odd_mask = 0xFFFFFFFF if is_odd(x) else 0

  uint32_t acc = (x->val[0] + (prime->val[0] & x_is_odd_mask)) >> 1;
  // acc < 2**BITS_PER_LIMB
  // Proof:
  //   acc == x[0] + prime[0] & x_is_odd_mask >> 1
  //     <= (2**(BITS_PER_LIMB) - 1) + (2**(BITS_PER_LIMB) - 1) >> 1
  //     == 2**(BITS_PER_LIMB + 1) - 2 >> 1
  //     <  2**(BITS_PER_LIMB)

  for (int i = 0; i < BN_LIMBS - 1; i++) {
    uint32_t temp = (x->val[i + 1] + (prime->val[i + 1] & x_is_odd_mask));
    // temp < 2**(BITS_PER_LIMB + 1)
    // Proof:
    //   temp == x[i + 1] + val[i + 1] & x_is_odd_mask
    //     <= (2**(BITS_PER_LIMB) - 1) + (2**(BITS_PER_LIMB) - 1)
    //     <  2**(BITS_PER_LIMB + 1)

    acc += (temp & 1) << (BN_BITS_PER_LIMB - 1);
    // acc doesn't overflow 32 bits
    // Proof:
    //   acc + (temp & 1 << BITS_PER_LIMB - 1)
    //     <= 2**(BITS_PER_LIMB + 1) + 2**(BITS_PER_LIMB - 1)
    //     <= 2**30 + 2**28 < 2**32

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    acc += temp >> 1;
    // acc < 2**(BITS_PER_LIMB + 1)
    // Proof:
    //   acc + (temp >> 1)
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + (2**(BITS_PER_LIMB + 1) - 1 >> 1)
    //     == 7 + 2**(BITS_PER_LIMB) - 1 < 2**(BITS_PER_LIMB + 1)

    // acc == x[:i+2]+(prime[:i+2] & x_is_odd_mask) >> BITS_PER_LIMB * (i+1)
  }
  x->val[BN_LIMBS - 1] = acc;

  // assert(acc >> BITS_PER_LIMB == 0);
  // acc >> BITS_PER_LIMB == 0
  // Proof:
  //   acc
  //     == x[:LIMBS] + (prime[:LIMBS] & x_is_odd_mask) >> BITS_PER_LIMB*LIMBS
  //     == x + (prime & x_is_odd_mask) >> BITS_PER_LIMB * LIMBS
  //     <= x + prime >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) - 1 >> BITS_PER_LIMB * LIMBS
  //     == 0
}

// x = x * k % prime
// Assumes x is normalized, 0 <= k <= 8 = 2**(32 - BITS_PER_LIMB)
// Assumes prime is normalized and 2^256 - 2^224 <= prime <= 2^256
// Guarantees x is normalized and partly reduced modulo prime
void bn_mult_k(bignum256 *x, uint8_t k, const bignum256 *prime) {
  assert(k <= 8);

  for (int i = 0; i < BN_LIMBS; i++) {
    x->val[i] = k * x->val[i];
    // x[i] doesn't overflow 32 bits
    // k * x[i] <= 2**(32 - BITS_PER_LIMB) * (2**BITS_PER_LIMB - 1)
    //   < 2**(32 - BITS_PER_LIMB) * 2**BITS_PER_LIMB == 2**32
  }

  bn_fast_mod(x, prime);
}

// Reduces partly reduced x modulo prime
// Explicitly x = x if x < prime else x - prime
// Assumes x is partly reduced modulo prime
// Guarantees x is fully reduced modulo prime
// Assumes prime is nonzero and normalized
void bn_mod(bignum256 *x, const bignum256 *prime) {
  uint32_t x_less_prime = bn_is_less(x, prime);

  bignum256 temp = {0};
  bn_subtract(x, prime, &temp);
  bn_cmov(x, x_less_prime, x, &temp);

  memzero(&temp, sizeof(temp));
}

// Auxiliary function for bn_multiply
// res = k * x
// Assumes k and x are normalized
// Guarantees res is normalized 18 digit little endian number in base 2**29
void bn_multiply_long(const bignum256 *k, const bignum256 *x,
                      uint32_t res[2 * BN_LIMBS]) {
  // Uses long multiplication in base 2**29, see
  // https://en.wikipedia.org/wiki/Multiplication_algorithm#Long_multiplication

  uint64_t acc = 0;

  // compute lower half
  for (int i = 0; i < BN_LIMBS; i++) {
    for (int j = 0; j <= i; j++) {
      acc += k->val[j] * (uint64_t)x->val[i - j];
      // acc doesn't overflow 64 bits
      // Proof:
      //   acc <= acc + sum([k[j] * x[i-j] for j in range(i)])
      //     <= (2**(64 - BITS_PER_LIMB) - 1) +
      //       LIMBS * (2**BITS_PER_LIMB - 1) * (2**BITS_PER_LIMB - 1)
      //     == (2**35 - 1) + 9 * (2**29 - 1) * (2**29 - 1)
      //     <= 2**35 + 9 * 2**58 < 2**64
    }

    res[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 2**35 - 1 == 2**(64 - BITS_PER_LIMB) - 1
  }

  // compute upper half
  for (int i = BN_LIMBS; i < 2 * BN_LIMBS - 1; i++) {
    for (int j = i - BN_LIMBS + 1; j < BN_LIMBS; j++) {
      acc += k->val[j] * (uint64_t)x->val[i - j];
      // acc doesn't overflow 64 bits
      // Proof:
      //   acc <= acc + sum([k[j] * x[i-j] for j in range(i)])
      //     <= (2**(64 - BITS_PER_LIMB) - 1)
      //       LIMBS * (2**BITS_PER_LIMB - 1) * (2**BITS_PER_LIMB - 1)
      //     == (2**35 - 1) + 9 * (2**29 - 1) * (2**29 - 1)
      //     <= 2**35 + 9 * 2**58 < 2**64
    }

    res[i] = acc & (BN_BASE - 1);
    acc >>= BN_BITS_PER_LIMB;
    // acc < 2**35 == 2**(64 - BITS_PER_LIMB)
  }

  res[2 * BN_LIMBS - 1] = acc;
}

// Auxiliary function for bn_multiply
// Assumes 0 <= d <= 8 == LIMBS - 1
// Assumes res is normalized and res < 2**(256 + 29*d + 31)
// Guarantess res in normalized and res < 2 * prime * 2**(29*d)
// Assumes prime is normalized, 2**256 - 2**224 <= prime <= 2**256
void bn_multiply_reduce_step(uint32_t res[2 * BN_LIMBS], const bignum256 *prime,
                             uint32_t d) {
  // clang-format off
  // Computes res = res - (res // 2**(256 + BITS_PER_LIMB * d)) * prime * 2**(BITS_PER_LIMB * d)

  // res - (res // 2**(256 + BITS_PER_LIMB * d)) * prime * 2**(BITS_PER_LIMB * d) < 2 * prime * 2**(BITS_PER_LIMB * d)
  // Proof:
  //   res - res // (2**(256 + BITS_PER_LIMB * d)) * 2**(BITS_PER_LIMB * d) * prime
  //     == res - res // (2**(256 + BITS_PER_LIMB * d)) * 2**(BITS_PER_LIMB * d) * (2**256 - (2**256 - prime))
  //     == res - res // (2**(256 + BITS_PER_LIMB * d)) * 2**(BITS_PER_LIMB * d) * 2**256 + res // (2**(256 + BITS_PER_LIMB * d)) * 2**(BITS_PER_LIMB * d) * (2**256 - prime)
  //     == (res % 2**(256 + BITS_PER_LIMB * d)) + res // (2**256 + BITS_PER_LIMB * d) * 2**(BITS_PER_LIMB * d) * (2**256 - prime)
  //     <= (2**(256 + 29*d + 31) % 2**(256 + 29*d)) + (2**(256 + 29*d + 31) - 1) / (2**256 + 29*d) * 2**(29*d) * (2**256 - prime)
  //     <= 2**(256 + 29*d) + 2**(256 + 29*d + 31) / (2**256 + 29*d) * 2**(29*d) * (2**256 - prime)
  //     == 2**(256 + 29*d) + 2**31 * 2**(29*d) * (2**256 - prime)
  //     == 2**(29*d) * (2**256 + 2**31 * (2*256 - prime))
  //     <= 2**(29*d) * (2**256 + 2**31 * 2*224)
  //     <= 2**(29*d) * (2**256 + 2**255)
  //     <= 2**(29*d) * 2 * (2**256 - 2**224)
  //     <= 2 * prime * 2**(29*d)
  // clang-format on

  uint32_t coef =
      (res[d + BN_LIMBS - 1] >> (256 - (BN_LIMBS - 1) * BN_BITS_PER_LIMB)) +
      (res[d + BN_LIMBS] << ((BN_LIMBS * BN_BITS_PER_LIMB) - 256));

  // coef == res // 2**(256 + BITS_PER_LIMB * d)

  // coef <  2**31
  // Proof:
  //   coef == res // 2**(256 + BITS_PER_LIMB * d)
  //     <  2**(256 + 29 * d + 31) // 2**(256 + 29 * d)
  //     == 2**31

  const int shift = 31;
  uint64_t acc = 1ull << shift;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc += (((uint64_t)(BN_BASE - 1)) << shift) + res[d + i] -
           prime->val[i] * (uint64_t)coef;
    // acc neither overflow 64 bits nor underflow zero
    // Proof:
    //   acc + ((BASE - 1) << shift) + res[d + i] - prime[i] * coef
    //     >= ((BASE - 1) << shift) - prime[i] * coef
    //     == 2**shift * (2**BITS_PER_LIMB - 1) - (2**BITS_PER_LIMB - 1) *
    //       (2**31 - 1)
    //     == (2**shift - 2**31 + 1) * (2**BITS_PER_LIMB - 1)
    //     == (2**31 - 2**31 + 1) * (2**29 - 1)
    //     == 2**29 - 1 > 0
    //   acc + ((BASE - 1) << shift) + res[d + i] - prime[i] * coef
    //     <= acc + ((BASE - 1) << shift) + res[d+i]
    //     <= (2**(64 - BITS_PER_LIMB) - 1) + 2**shift * (2**BITS_PER_LIMB - 1)
    //       + (2*BITS_PER_LIMB - 1)
    //     == (2**(64 - BITS_PER_LIMB) - 1) + (2**shift + 1) *
    //       (2**BITS_PER_LIMB - 1)
    //     == (2**35 - 1) + (2**31 + 1) * (2**29 - 1)
    //     <= 2**35 + 2**60 + 2**29 < 2**64

    res[d + i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 2**(64 - BITS_PER_LIMB) - 1 == 2**35 - 1

    // acc == (1 << BITS_PER_LIMB * (i + 1) + shift) + res[d : d + i + 1]
    //   - coef * prime[:i + 1] >> BITS_PER_LIMB * (i + 1)
  }

  // acc += (((uint64_t)(BASE - 1)) << shift) + res[d + LIMBS];
  // acc >>= BITS_PER_LIMB;
  // assert(acc <= 1ul << shift);

  // clang-format off
  // acc == 1 << shift
  // Proof:
  //   acc
  //     == (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + res[d : d + LIMBS + 1] - coef * prime[:LIMBS] >> BITS_PER_LIMB * (LIMBS + 1)
  //     == (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + res[d : d + LIMBS + 1] - coef * prime >> BITS_PER_LIMB * (LIMBS + 1)

  //     == (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + (res[d : d + LIMBS + 1] - coef * prime) >> BITS_PER_LIMB * (LIMBS + 1)
  //     <= (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + (res[:d] + BASE**d * res[d : d + LIMBS + 1] - BASE**d * coef * prime)//BASE**d >> BITS_PER_LIMB * (LIMBS + 1)
  //     <= (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + (res - BASE**d * coef * prime) // BASE**d >> BITS_PER_LIMB * (LIMBS + 1)
  //     == (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + (2 * prime * BASE**d) // BASE**d >> BITS_PER_LIMB * (LIMBS + 1)
  //     <= (1 << 321) + 2 * 2**256 >> 290
  //     == 1 << 31 == 1 << shift

  //     == (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + res[d : d + LIMBS + 1] - coef * prime[:LIMBS + 1] >> BITS_PER_LIMB * (LIMBS + 1)
  //     >= (1 << BITS_PER_LIMB * (LIMBS + 1) + shift) + 0 >> BITS_PER_LIMB * (LIMBS + 1)
  //     == 1 << shift
  // clang-format on

  res[d + BN_LIMBS] = 0;
}

// Auxiliary function for bn_multiply
// Partly reduces res and stores both in x and res
// Assumes res in normalized and res < 2**519
// Guarantees x is normalized and partly reduced modulo prime
// Assumes prime is normalized, 2**256 - 2**224 <= prime <= 2**256
void bn_multiply_reduce(bignum256 *x, uint32_t res[2 * BN_LIMBS],
                        const bignum256 *prime) {
  for (int i = BN_LIMBS - 1; i >= 0; i--) {
    // res < 2**(256 + 29*i + 31)
    // Proof:
    //   if i == LIMBS - 1:
    //     res < 2**519
    //       == 2**(256 + 29 * 8 + 31)
    //       == 2**(256 + 29 * (LIMBS - 1) + 31)
    //   else:
    //     res < 2 * prime * 2**(29 * (i + 1))
    //       <= 2**256 * 2**(29*i + 29) < 2**(256 + 29*i + 31)
    bn_multiply_reduce_step(res, prime, i);
  }

  for (int i = 0; i < BN_LIMBS; i++) {
    x->val[i] = res[i];
  }
}

// x = k * x % prime
// Assumes k, x are normalized, k * x < 2**519
// Guarantees x is normalized and partly reduced modulo prime
// Assumes prime is normalized, 2**256 - 2**224 <= prime <= 2**256
void bn_multiply(const bignum256 *k, bignum256 *x, const bignum256 *prime) {
  uint32_t res[2 * BN_LIMBS] = {0};

  bn_multiply_long(k, x, res);
  bn_multiply_reduce(x, res, prime);

  memzero(res, sizeof(res));
}

// Partly reduces x modulo prime
// Assumes limbs of x except the last (the most significant) one are normalized
// Assumes prime is normalized and 2^256 - 2^224 <= prime <= 2^256
// Guarantees x is normalized and partly reduced modulo prime
void bn_fast_mod(bignum256 *x, const bignum256 *prime) {
  // Computes x = x - (x // 2**256) * prime

  // x < 2**((LIMBS - 1) * BITS_PER_LIMB + 32) == 2**264

  // x - (x // 2**256) * prime < 2 * prime
  // Proof:
  //   x - (x // 2**256) * prime
  //     == x - (x // 2**256) * (2**256 - (2**256 - prime))
  //     == x - ((x // 2**256) * 2**256) + (x // 2**256) * (2**256 - prime)
  //     == (x % prime) + (x // 2**256) * (2**256 - prime)
  //     <= prime - 1 + (2**264 // 2**256) * (2**256 - prime)
  //     <= 2**256 + 2**8 * 2**224 == 2**256 + 2**232
  //     <  2 * (2**256 - 2**224)
  //     <= 2 * prime

  // x - (x // 2**256 - 1) * prime < 2 * prime
  // Proof:
  //   x - (x // 2**256) * prime + prime
  //     == x - (x // 2**256) * (2**256 - (2**256 - prime)) + prime
  //     == x - ((x//2**256) * 2**256) + (x//2**256) * (2**256 - prime) + prime
  //     == (x % prime) + (x // 2**256) * (2**256 - prime) + prime
  //     <= 2 * prime - 1 + (2**264 // 2**256) * (2**256 - prime)
  //     <= 2 * prime + 2**8 * 2**224 == 2**256 + 2**232 + 2**256 - 2**224
  //     <  2 * (2**256 - 2**224)
  //     <= 2 * prime

  uint32_t coef =
      x->val[BN_LIMBS - 1] >> (256 - ((BN_LIMBS - 1) * BN_BITS_PER_LIMB));

  // clang-format off
  // coef == x // 2**256
  // 0 <= coef < 2**((LIMBS - 1) * BITS_PER_LIMB + 32 - 256) == 256
  // Proof:
  //*  Let x[[a : b] be the number consisting of a-th to (b-1)-th bit of the number x.
  //   x[LIMBS - 1] >> (256 - ((LIMBS - 1) * BITS_PER_LIMB))
  //     == x[[(LIMBS - 1) * BITS_PER_LIMB : (LIMBS - 1) * BITS_PER_LIMB + 32]] >> (256 - ((LIMBS - 1) * BITS_PER_LIMB))
  //     == x[[256 - ((LIMBS - 1) * BITS_PER_LIMB) + (LIMBS - 1) * BITS_PER_LIMB : (LIMBS - 1) * BITS_PER_LIMB + 32]]
  //     == x[[256 : (LIMBS - 1) * BITS_PER_LIMB + 32]]
  //     == x[[256 : 264]] == x // 2**256
  // clang-format on

  const int shift = 8;
  uint64_t acc = 1ull << shift;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc += (((uint64_t)(BN_BASE - 1)) << shift) + x->val[i] -
           prime->val[i] * (uint64_t)coef;
    // acc neither overflows 64 bits nor underflows 0
    // Proof:
    //   acc + (BASE - 1 << shift) + x[i] - prime[i] * coef
    //     >= (BASE - 1 << shift) - prime[i] * coef
    //     >= 2**shift * (2**BITS_PER_LIMB - 1) - (2**BITS_PER_LIMB - 1) * 255
    //     == (2**shift - 255) * (2**BITS_PER_LIMB - 1)
    //     == (2**8 - 255) * (2**29 - 1) == 2**29 - 1 >= 0
    //   acc + (BASE - 1 << shift) + x[i] - prime[i] * coef
    //     <= acc + ((BASE - 1) << shift) + x[i]
    //     <= (2**(64 - BITS_PER_LIMB) - 1) + 2**shift * (2**BITS_PER_LIMB - 1)
    //       + (2**32 - 1)
    //     == (2**35 - 1) + 2**8 * (2**29 - 1) + 2**32
    //     <  2**35 + 2**37 + 2**32 < 2**64

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 2**(64 - BITS_PER_LIMB) - 1 == 2**35 - 1

    // acc == (1 << BITS_PER_LIMB * (i + 1) + shift) + x[:i + 1]
    //   - coef * prime[:i + 1] >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc == 1 << shift);

  // clang-format off
  // acc == 1 << shift
  // Proof:
  //   acc
  //     == (1 << BITS_PER_LIMB * LIMBS + shift) + x[:LIMBS] - coef * prime[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == (1 << BITS_PER_LIMB * LIMBS + shift) + (x - coef * prime) >> BITS_PER_LIMB * LIMBS
  //     <= (1 << BITS_PER_LIMB * LIMBS + shift) + (2 * prime) >> BITS_PER_LIMB * LIMBS
  //     <= (1 << BITS_PER_LIMB * LIMBS + shift) + 2 * 2**256 >> BITS_PER_LIMB * LIMBS
  //     <= 2**269 + 2**257 >> 2**261
  //     <= 1 << 8 == 1 << shift

  //   acc
  //     == (1 << BITS_PER_LIMB * LIMBS + shift) + x[:LIMBS] - coef * prime[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     >= (1 << BITS_PER_LIMB * LIMBS + shift) + 0 >> BITS_PER_LIMB * LIMBS
  //     == (1 << BITS_PER_LIMB * LIMBS + shift) + 0 >> BITS_PER_LIMB * LIMBS
  //     <= 1 << 8 == 1 << shift
  // clang-format on
}

// res = x**e % prime
// Assumes both x and e are normalized, x < 2**259
// Guarantees res is normalized and partly reduced modulo prime
// Works properly even if &x == &res
// Assumes prime is normalized, 2**256 - 2**224 <= prime <= 2**256
// The function doesn't have neither constant control flow nor constant memory
//  access flow with regard to e
void bn_power_mod(const bignum256 *x, const bignum256 *e,
                  const bignum256 *prime, bignum256 *res) {
  // Uses iterative right-to-left exponentiation by squaring, see
  // https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method

  bignum256 acc = {0};
  bn_copy(x, &acc);

  bn_one(res);
  for (int i = 0; i < BN_LIMBS; i++) {
    uint32_t limb = e->val[i];

    for (int j = 0; j < BN_BITS_PER_LIMB; j++) {
      // Break if the following bits of the last limb are zero
      if (i == BN_LIMBS - 1 && limb == 0) break;

      if (limb & 1)
        // acc * res < 2**519
        // Proof:
        //   acc * res <= max(2**259 - 1, 2 * prime) * (2 * prime)
        //     == max(2**259 - 1, 2**257) * 2**257 < 2**259 * 2**257
        //     == 2**516 < 2**519
        bn_multiply(&acc, res, prime);

      limb >>= 1;
      // acc * acc < 2**519
      // Proof:
      //   acc * acc <= max(2**259 - 1, 2 * prime)**2
      //     <= (2**259)**2 == 2**518 < 2**519
      bn_multiply(&acc, &acc, prime);
    }
    // acc == x**(e[:i + 1]) % prime
  }

  memzero(&acc, sizeof(acc));
}

// x = sqrt(x) % prime
// Explicitly x = x**((prime+1)/4) % prime
// The other root is -sqrt(x)
// Assumes x is normalized, x < 2**259 and quadratic residuum mod prime
// Assumes prime is a prime number, prime % 4 == 3, it is normalized and
//   2**256 - 2**224 <= prime <= 2**256
// Guarantees x is normalized and fully reduced modulo prime
// The function doesn't have neither constant control flow nor constant memory
//  access flow with regard to prime
void bn_sqrt(bignum256 *x, const bignum256 *prime) {
  // Uses the Lagrange formula for the primes of the special form, see
  // http://en.wikipedia.org/wiki/Quadratic_residue#Prime_or_prime_power_modulus
  // If prime % 4 == 3, then sqrt(x) % prime == x**((prime+1)//4) % prime

  assert(prime->val[BN_LIMBS - 1] % 4 == 3);

  // e = (prime + 1) // 4
  bignum256 e = {0};
  bn_copy(prime, &e);
  bn_addi(&e, 1);
  bn_rshift(&e);
  bn_rshift(&e);

  bn_power_mod(x, &e, prime, x);
  bn_mod(x, prime);

  memzero(&e, sizeof(e));
}

// a = 1/a % 2**n
// Assumes a is odd, 1 <= n <= 32
// The function doesn't have neither constant control flow nor constant memory
//   access flow with regard to n
uint32_t inverse_mod_power_two(uint32_t a, uint32_t n) {
  // Uses "Explicit Quadratic Modular inverse modulo 2" from section 3.3 of "On
  // Newton-Raphson iteration for multiplicative inverses modulo prime powers"
  // by Jean-Guillaume Dumas, see
  // https://arxiv.org/pdf/1209.6626.pdf

  // 1/a % 2**n
  //   = (2-a) * product([1 + (a-1)**(2**i) for i in range(1, floor(log2(n)))])

  uint32_t acc = 2 - a;
  uint32_t f = a - 1;

  // mask = (1 << n) - 1
  uint32_t mask = n == 32 ? 0xFFFFFFFF : (1u << n) - 1;

  for (uint32_t i = 1; i < n; i <<= 1) {
    f = (f * f) & mask;
    acc = (acc * (1 + f)) & mask;
  }

  return acc;
}

// x = (x / 2**BITS_PER_LIMB) % prime
// Assumes both x and prime are normalized
// Assumes prime is an odd number and normalized
// Guarantees x is normalized
// If x is partly reduced (fully reduced) modulo prime,
//   guarantess x will be partly reduced (fully reduced) modulo prime
void bn_divide_base(bignum256 *x, const bignum256 *prime) {
  // Uses an explicit formula for the modular inverse of power of two
  // (x / 2**n) % prime == (x + ((-x / prime) % 2**n) * prime) // 2**n
  // Proof:
  //   (x + ((-x / prime) % 2**n) * prime) % 2**n
  //     == (x - x / prime * prime) % 2**n
  //     == 0
  //   (x + ((-1 / prime) % 2**n) * prime) % prime
  //     == x
  //   if x < prime:
  //     (x + ((-x / prime) % 2**n) * prime) // 2**n
  //       <= ((prime - 1) + (2**n - 1) * prime) / 2**n
  //       == (2**n * prime - 1) / 2**n == prime - 1 / 2**n < prime
  //   if x < 2 * prime:
  //     (x + ((-x / prime) % 2**n) * prime) // 2**n
  //       <= ((2 * prime - 1) + (2**n - 1) * prime) / 2**n
  //       == (2**n * prime + prime - 1) / 2**n
  //       == prime + (prime - 1) / 2**n < 2 * prime

  // m = (-x / prime) % 2**BITS_PER_LIMB
  uint32_t m = (x->val[0] * (BN_BASE - inverse_mod_power_two(
                                           prime->val[0], BN_BITS_PER_LIMB))) &
               BN_LIMB_MASK;
  // m < 2**BITS_PER_LIMB

  uint64_t acc = x->val[0] + (uint64_t)m * prime->val[0];
  acc >>= BN_BITS_PER_LIMB;

  for (int i = 1; i < BN_LIMBS; i++) {
    acc = acc + x->val[i] + (uint64_t)m * prime->val[i];
    // acc does not overflow 64 bits
    // acc == acc + x + m * prime
    //    <= 2**(64 - BITS_PER_LIMB) + 2**(BITS_PER_LIMB)
    //      2**(BITS_PER_LIMB) * 2**(BITS_PER_LIMB)
    //    <= 2**(2 * BITS_PER_LIMB) + 2**(64 - BITS_PER_LIMB) +
    //      2**(BITS_PER_LIMB)
    //    <= 2**58 + 2**35 + 2**29 < 2**64

    x->val[i - 1] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc < 2**35 == 2**(64 - BITS_PER_LIMB)

    // acc == x[:i + 1] + m * prime[:i + 1] >> BITS_PER_LIMB * (i + 1)
  }

  x->val[BN_LIMBS - 1] = acc;

  assert(acc >> BN_BITS_PER_LIMB == 0);

  // clang-format off
  // acc >> BITS_PER_LIMB == 0
  // Proof:
  //   acc >> BITS_PER_LIMB
  //     == (x[:LIMB] + m * prime[:LIMB] >> BITS_PER_LIMB * LIMBS) >> BITS_PER_LIMB * (LIMBS + 1)
  //     == x + m * prime >> BITS_PER_LIMB * (LIMBS + 1)
  //     <= (2**(BITS_PER_LIMB * LIMBS) - 1) + (2**BITS_PER_LIMB - 1) * (2**(BITS_PER_LIMB * LIMBS) - 1) >> BITS_PER_LIMB * (LIMBS + 1)
  //     == 2**(BITS_PER_LIMB * LIMBS) - 1 + 2**(BITS_PER_LIMB * (LIMBS + 1)) - 2**(BITS_PER_LIMB * LIMBS) - 2**BITS_PER_LIMB + 1 >> BITS_PER_LIMB * (LIMBS + 1)
  //     == 2**(BITS_PER_LIMB * (LIMBS + 1)) - 2**BITS_PER_LIMB >> BITS_PER_LIMB * (LIMBS + 1)
  //     == 0
  // clang-format on
}

#if !USE_INVERSE_FAST
// x = 1/x % prime if x != 0 else 0
// Assumes x is normalized
// Assumes prime is a prime number
// Guarantees x is normalized and fully reduced modulo prime
// Assumes prime is normalized, 2**256 - 2**224 <= prime <= 2**256
// The function doesn't have neither constant control flow nor constant memory
//   access flow with regard to prime
static void bn_inverse_slow(bignum256 *x, const bignum256 *prime) {
  // Uses formula 1/x % prime == x**(prime - 2) % prime
  // See https://en.wikipedia.org/wiki/Fermat%27s_little_theorem

  bn_fast_mod(x, prime);

  // e = prime - 2
  bignum256 e = {0};
  bn_read_uint32(2, &e);
  bn_subtract(prime, &e, &e);

  bn_power_mod(x, &e, prime, x);
  bn_mod(x, prime);

  memzero(&e, sizeof(e));
}
#endif

#if false
// x = 1/x % prime if x != 0 else 0
// Assumes x is is_normalized
// Assumes GCD(x, prime) = 1
// Guarantees x is normalized and fully reduced modulo prime
// Assumes prime is odd, normalized, 2**256 - 2**224 <= prime <= 2**256
// The function doesn't have neither constant control flow nor constant memory
//   access flow with regard to prime and x
static void bn_inverse_fast(bignum256 *x, const bignum256 *prime) {
  // "The Almost Montgomery Inverse" from the section 3 of "Constant Time
  // Modular Inversion" by Joppe W. Bos
  // See http://www.joppebos.com/files/CTInversion.pdf

  /*
    u = prime
    v = x & prime
    s = 1
    r = 0

    k = 0
    while v != 1:
      k += 1
      if is_even(u):
        u = u // 2
        s = 2 * s
      elif is_even(v):
        v = v // 2
        r = 2 * r
      elif v < u:
        u = (u - v) // 2
        r = r + s
        s = 2 * s
      else:
        v = (v - u) // 2
        s = r + s
        r = 2 * r

    s = (s / 2**k) % prime
    return s
  */

  if (bn_is_zero(x)) return;

  bn_fast_mod(x, prime);
  bn_mod(x, prime);

  bignum256 u = {0}, v = {0}, r = {0}, s = {0};
  bn_copy(prime, &u);
  bn_copy(x, &v);
  bn_one(&s);
  bn_zero(&r);

  int k = 0;
  while (!bn_is_one(&v)) {
    if ((u.val[0] & 1) == 0) {
      bn_rshift(&u);
      bn_lshift(&s);
    } else if ((v.val[0] & 1) == 0) {
      bn_rshift(&v);
      bn_lshift(&r);
    } else if (bn_is_less(&v, &u)) {
      bn_subtract(&u, &v, &u);
      bn_rshift(&u);
      bn_add(&r, &s);
      bn_lshift(&s);
    } else {
      bn_subtract(&v, &u, &v);
      bn_rshift(&v);
      bn_add(&s, &r);
      bn_lshift(&r);
    }
    k += 1;
    assert(!bn_is_zero(&v)); // assert GCD(x, prime) == 1
  }

  // s = s / 2**(k // BITS_PER_LIMB * BITS_PER_LIMB)
  for (int i = 0; i < k / BITS_PER_LIMB; i++) {
    bn_divide_base(&s, prime);
  }

  // s = s / 2**(k % BITS_PER_LIMB)
  for (int i = 0; i < k % BN_BITS_PER_LIMB; i++) {
    bn_mult_half(&s, prime);
  }

  bn_copy(&s, x);

  memzero(&u, sizeof(u));
  memzero(&v, sizeof(v));
  memzero(&r, sizeof(r));
  memzero(&s, sizeof(s));
}
#endif

#if USE_INVERSE_FAST
// x = 1/x % prime if x != 0 else 0
// Assumes x is is_normalized
// Assumes GCD(x, prime) = 1
// Guarantees x is normalized and fully reduced modulo prime
// Assumes prime is odd, normalized, 2**256 - 2**224 <= prime <= 2**256
// The function has constant control flow but not constant memory access flow
//   with regard to prime and x
static void bn_inverse_fast(bignum256 *x, const bignum256 *prime) {
  // Custom constant time version of "The Almost Montgomery Inverse" from the
  // section 3 of "Constant Time Modular Inversion" by Joppe W. Bos
  // See http://www.joppebos.com/files/CTInversion.pdf

  /*
    u = prime
    v = x % prime
    s = 1
    r = 0

    k = 0
    while v != 1:
      k += 1
      if is_even(u): # b1
        u = u // 2
        s = 2 * s
      elif is_even(v): # b2
        v = v // 2
        r = 2 * r
      elif v < u: # b3
        u = (u - v) // 2
        r = r + s
        s = 2 * s
      else: # b4
        v = (v - u) // 2
        s = r + s
        r = 2 * r

    s = (s / 2**k) % prime
    return s
  */

  bn_fast_mod(x, prime);
  bn_mod(x, prime);

  bignum256 u = {0}, v = {0}, r = {0}, s = {0};
  bn_copy(prime, &u);
  bn_copy(x, &v);
  bn_one(&s);
  bn_zero(&r);

  bignum256 zero = {0};
  bn_zero(&zero);

  int k = 0;

  int finished = 0, u_even = 0, v_even = 0, v_less_u = 0, b1 = 0, b2 = 0,
      b3 = 0, b4 = 0;
  finished = 0;

  for (int i = 0; i < 2 * BN_LIMBS * BN_BITS_PER_LIMB; i++) {
    finished = finished | -bn_is_one(&v);
    u_even = -bn_is_even(&u);
    v_even = -bn_is_even(&v);
    v_less_u = -bn_is_less(&v, &u);

    b1 = ~finished & u_even;
    b2 = ~finished & ~b1 & v_even;
    b3 = ~finished & ~b1 & ~b2 & v_less_u;
    b4 = ~finished & ~b1 & ~b2 & ~b3;

// The ternary operator for pointers with constant control flow
// BN_INVERSE_FAST_TERNARY(c, t, f) = t if c else f
// Very nasty hack, sorry for that
#define BN_INVERSE_FAST_TERNARY(c, t, f) \
  ((void *)(((c) & (uintptr_t)(t)) | (~(c) & (uintptr_t)(f))))

    bn_subtract(BN_INVERSE_FAST_TERNARY(b3, &u, &v),
                BN_INVERSE_FAST_TERNARY(
                    b3 | b4, BN_INVERSE_FAST_TERNARY(b3, &v, &u), &zero),
                BN_INVERSE_FAST_TERNARY(b3, &u, &v));

    bn_add(BN_INVERSE_FAST_TERNARY(b3, &r, &s),
           BN_INVERSE_FAST_TERNARY(b3 | b4, BN_INVERSE_FAST_TERNARY(b3, &s, &r),
                                   &zero));
    bn_rshift(BN_INVERSE_FAST_TERNARY(b1 | b3, &u, &v));
    bn_lshift(BN_INVERSE_FAST_TERNARY(b1 | b3, &s, &r));

    k = k - ~finished;
  }

  // s = s / 2**(k // BITS_PER_LIMB * BITS_PER_LIMB)
  for (int i = 0; i < 2 * BN_LIMBS; i++) {
    // s = s / 2**BITS_PER_LIMB % prime if i < k // BITS_PER_LIMB else s
    bn_copy(&s, &r);
    bn_divide_base(&r, prime);
    bn_cmov(&s, i < k / BN_BITS_PER_LIMB, &r, &s);
  }

  // s = s / 2**(k % BITS_PER_LIMB)
  for (int i = 0; i < BN_BITS_PER_LIMB; i++) {
    // s = s / 2 % prime if i < k % BITS_PER_LIMB else s
    bn_copy(&s, &r);
    bn_mult_half(&r, prime);
    bn_cmov(&s, i < k % BN_BITS_PER_LIMB, &r, &s);
  }

  bn_cmov(x, bn_is_zero(x), x, &s);

  memzero(&u, sizeof(u));
  memzero(&v, sizeof(v));
  memzero(&r, sizeof(s));
  memzero(&s, sizeof(s));
}
#endif

#if false
// x = 1/x % prime if x != 0 else 0
// Assumes x is is_normalized
// Assumes GCD(x, prime) = 1
// Guarantees x is normalized and fully reduced modulo prime
// Assumes prime is odd, normalized, 2**256 - 2**224 <= prime <= 2**256
static void bn_inverse_fast(bignum256 *x, const bignum256 *prime) {
  // Custom constant time version of "The Almost Montgomery Inverse" from the
  // section 3 of "Constant Time Modular Inversion" by Joppe W. Bos
  // See http://www.joppebos.com/files/CTInversion.pdf

  /*
    u = prime
    v = x % prime
    s = 1
    r = 0

    k = 0
    while v != 1:
      k += 1
      if is_even(u): # b1
        u = u // 2
        s = 2 * s
      elif is_even(v): # b2
        v = v // 2
        r = 2 * r
      elif v < u: # b3
        u = (u - v) // 2
        r = r + s
        s = 2 * s
      else: # b4
        v = (v - u) // 2
        s = r + s
        r = 2 * r

    s = (s / 2**k) % prime
    return s
  */

  bn_fast_mod(x, prime);
  bn_mod(x, prime);

  bignum256 u = {0}, v = {0}, r = {0}, s = {0};
  bn_copy(prime, &u);
  bn_copy(x, &v);
  bn_one(&s);
  bn_zero(&r);

  bignum256 zero = {0};
  bn_zero(&zero);

  int k = 0;

  uint32_t finished = 0, u_even = 0, v_even = 0, v_less_u = 0, b1 = 0, b2 = 0,
      b3 = 0, b4 = 0;
  finished = 0;

  bignum256 u_half = {0}, v_half = {0}, u_minus_v_half = {0}, v_minus_u_half = {0}, r_plus_s = {0}, r_twice = {0}, s_twice = {0};
  for (int i = 0; i < 2 * BN_LIMBS * BN_BITS_PER_LIMB; i++) {
    finished = finished | bn_is_one(&v);
    u_even = bn_is_even(&u);
    v_even = bn_is_even(&v);
    v_less_u = bn_is_less(&v, &u);

    b1 = (finished ^ 1) & u_even;
    b2 = (finished ^ 1) & (b1 ^ 1) & v_even;
    b3 = (finished ^ 1) & (b1 ^ 1) & (b2 ^ 1) & v_less_u;
    b4 = (finished ^ 1) & (b1 ^ 1) & (b2 ^ 1) & (b3 ^ 1);

    // u_half = u // 2
    bn_copy(&u, &u_half);
    bn_rshift(&u_half);

    // v_half = v // 2
    bn_copy(&v, &v_half);
    bn_rshift(&v_half);

    // u_minus_v_half  = (u - v) // 2
    bn_subtract(&u, &v, &u_minus_v_half);
    bn_rshift(&u_minus_v_half);

    // v_minus_u_half  = (v - u) // 2
    bn_subtract(&v, &u, &v_minus_u_half);
    bn_rshift(&v_minus_u_half);

    // r_plus_s = r + s
    bn_copy(&r, &r_plus_s);
    bn_add(&r_plus_s, &s);

    // r_twice = 2 * r
    bn_copy(&r, &r_twice);
    bn_lshift(&r_twice);

    // s_twice = 2 * s
    bn_copy(&s, &s_twice);
    bn_lshift(&s_twice);

    bn_cmov(&u, b1, &u_half, &u);
    bn_cmov(&u, b3, &u_minus_v_half, &u);

    bn_cmov(&v, b2, &v_half, &v);
    bn_cmov(&v, b4, &v_minus_u_half, &v);

    bn_cmov(&r, b2 | b4, &r_twice, &r);
    bn_cmov(&r, b3, &r_plus_s, &r);

    bn_cmov(&s, b1 | b3, &s_twice, &s);
    bn_cmov(&s, b4, &r_plus_s, &s);

    k = k + (finished ^ 1);
  }

  // s = s / 2**(k // BITS_PER_LIMB * BITS_PER_LIMB)
  for (int i = 0; i < 2 * BN_LIMBS; i++) {
    // s = s / 2**BITS_PER_LIMB % prime if i < k // BITS_PER_LIMB else s
    bn_copy(&s, &r);
    bn_divide_base(&r, prime);
    bn_cmov(&s, i < k / BITS_PER_LIMB, &r, &s);
  }

  // s = s / 2**(k % BITS_PER_LIMB)
  for (int i = 0; i < BN_BITS_PER_LIMB; i++) {
    // s = s / 2 % prime if i < k % BITS_PER_LIMB else s
    bn_copy(&s, &r);
    bn_mult_half(&r, prime);
    bn_cmov(&s, i < k % BN_BITS_PER_LIMB, &r, &s);
  }

  bn_cmov(x, bn_is_zero(x), x, &s);

  memzero(&u, sizeof(u));
  memzero(&v, sizeof(v));
  memzero(&r, sizeof(r));
  memzero(&s, sizeof(s));
  memzero(&u_half, sizeof(u_half));
  memzero(&v_half, sizeof(v_half));
  memzero(&u_minus_v_half, sizeof(u_minus_v_half));
  memzero(&v_minus_u_half, sizeof(v_minus_u_half));
  memzero(&r_twice, sizeof(r_twice));
  memzero(&s_twice, sizeof(s_twice));
  memzero(&r_plus_s, sizeof(r_plus_s));
}
#endif

// Normalizes x
// Assumes x < 2**261 == 2**(LIMBS * BITS_PER_LIMB)
// Guarantees x is normalized
void bn_normalize(bignum256 *x) {
  uint32_t acc = 0;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc += x->val[i];
    // acc doesn't overflow 32 bits
    // Proof:
    //   acc + x[i]
    //      <= (2**(32 - BITS_PER_LIMB) - 1) + (2**BITS_PER_LIMB - 1)
    //      == 7 + 2**29 - 1 < 2**32

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= (BN_BITS_PER_LIMB);
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1
  }
}

// x = x + y
// Assumes x, y are normalized, x + y < 2**(LIMBS*BITS_PER_LIMB) == 2**261
// Guarantees x is normalized
// Works properly even if &x == &y
void bn_add(bignum256 *x, const bignum256 *y) {
  uint32_t acc = 0;
  for (int i = 0; i < BN_LIMBS; i++) {
    acc += x->val[i] + y->val[i];
    // acc doesn't overflow 32 bits
    // Proof:
    //   acc + x[i] + y[i]
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + 2 * (2**BITS_PER_LIMB - 1)
    //     == (2**(32 - BITS_PER_LIMB) - 1) + 2**(BITS_PER_LIMB + 1) - 2
    //     == 7 + 2**30 - 2 < 2**32

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1

    // acc == x[:i + 1] + y[:i + 1] >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc == 0); // assert x + y < 2**261
  // acc == 0
  // Proof:
  //   acc == x[:LIMBS] + y[:LIMBS] >> LIMBS * BITS_PER_LIMB
  //     == x + y >> LIMBS * BITS_PER_LIMB
  //     <= 2**(LIMBS * BITS_PER_LIMB) - 1 >> LIMBS * BITS_PER_LIMB == 0
}

// x = x + y % prime
// Assumes x, y are normalized
// Guarantees x is normalized and partly reduced modulo prime
// Assumes prime is normalized and 2^256 - 2^224 <= prime <= 2^256
void bn_addmod(bignum256 *x, const bignum256 *y, const bignum256 *prime) {
  for (int i = 0; i < BN_LIMBS; i++) {
    x->val[i] += y->val[i];
    // x[i] doesn't overflow 32 bits
    // Proof:
    //   x[i] + y[i]
    //     <= 2 * (2**BITS_PER_LIMB - 1)
    //     == 2**30 - 2 < 2**32
  }

  bn_fast_mod(x, prime);
}

// x = x + y
// Assumes x is normalized
// Assumes y <= 2**32 - 2**29 == 2**32 - 2**BITS_PER_LIMB and
//   x + y < 2**261 == 2**(LIMBS * BITS_PER_LIMB)
// Guarantees x is normalized
void bn_addi(bignum256 *x, uint32_t y) {
  // assert(y <= 3758096384); // assert y <= 2**32 - 2**29
  uint32_t acc = y;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc += x->val[i];
    // acc doesn't overflow 32 bits
    // Proof:
    //   if i == 0:
    //     acc + x[i] == y + x[0]
    //       <= (2**32 - 2**BITS_PER_LIMB) + (2**BITS_PER_LIMB - 1)
    //       == 2**32 - 1 < 2**32
    //   else:
    //     acc + x[i]
    //       <= (2**(32 - BITS_PER_LIMB) - 1) + (2**BITS_PER_LIMB - 1)
    //       == 7 + 2**29 - 1 < 2**32

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= (BN_BITS_PER_LIMB);
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1

    // acc == x[:i + 1] + y >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc == 0); // assert x + y < 2**261
  // acc == 0
  // Proof:
  //   acc == x[:LIMBS] + y << LIMBS * BITS_PER_LIMB
  //     == x + y << LIMBS * BITS_PER_LIMB
  //     <= 2**(LIMBS + BITS_PER_LIMB) - 1 << LIMBS * BITS_PER_LIMB
  //     == 0
}

// x = x - y % prime
// Explicitly x = x + prime - y
// Assumes x, y are normalized
// Assumes y < prime[0], x + prime - y < 2**261 == 2**(LIMBS * BITS_PER_LIMB)
// Guarantees x is normalized
// If x is fully reduced modulo prime,
//   guarantess x will be partly reduced modulo prime
// Assumes prime is nonzero and normalized
void bn_subi(bignum256 *x, uint32_t y, const bignum256 *prime) {
  assert(y < prime->val[0]);

  // x = x + prime - y

  uint32_t acc = -y;
  for (int i = 0; i < BN_LIMBS; i++) {
    acc += x->val[i] + prime->val[i];
    // acc neither overflows 32 bits nor underflows 0
    // Proof:
    //   acc + x[i] + prime[i]
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + 2 * (2**BITS_PER_LIMB - 1)
    //     <= 7 + 2**30 - 2 < 2**32
    //   acc + x[i] + prime[i]
    //     >= -y + prime[0] >= 0

    x->val[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1

    // acc == x[:i + 1] + prime[:i + 1] - y >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc == 0); // assert x + prime - y < 2**261
  // acc == 0
  // Proof:
  //   acc == x[:LIMBS] + prime[:LIMBS] - y >> BITS_PER_LIMB * LIMBS
  //     == x + prime - y >> BITS_PER_LIMB * LIMBS
  //     <= 2**(LIMBS * BITS_PER_LIMB) - 1 >> BITS_PER_LIMB * LIMBS == 0
}

// res = x - y % prime
// Explicitly res = x + (2 * prime - y)
// Assumes x, y are normalized, y is partly reduced
// Assumes x + 2 * prime - y < 2**261 == 2**(BITS_PER_LIMB * LIMBS)
// Guarantees res is normalized
// Assumes prime is nonzero and normalized
void bn_subtractmod(const bignum256 *x, const bignum256 *y, bignum256 *res,
                    const bignum256 *prime) {
  // res = x + (2 * prime - y)

  uint32_t acc = 1;

  for (int i = 0; i < BN_LIMBS; i++) {
    acc += (BN_BASE - 1) + x->val[i] + 2 * prime->val[i] - y->val[i];
    // acc neither overflows 32 bits nor underflows 0
    // Proof:
    //   acc + (BASE - 1) + x[i] + 2 * prime[i] - y[i]
    //     >= (BASE - 1) - y[i]
    //     == (2**BITS_PER_LIMB - 1) - (2**BITS_PER_LIMB - 1) == 0
    //   acc + (BASE - 1) + x[i] + 2 * prime[i] - y[i]
    //     <= acc + (BASE - 1) + x[i] + 2 * prime[i]
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + (2**BITS_PER_LIMB - 1) +
    //     (2**BITS_PER_LIMB - 1) + 2 * (2**BITS_PER_LIMB - 1)
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + 4 * (2**BITS_PER_LIMB - 1)
    //     == 7 + 4 * 2**29 - 4 == 2**31 + 3 < 2**32

    res->val[i] = acc & (BN_BASE - 1);
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1

    // acc == 2**(BITS_PER_LIMB * (i + 1)) + x[:i+1] - y[:i+1] + 2*prime[:i+1]
    //   >> BITS_PER_LIMB * (i+1)
  }

  // assert(acc == 1); // assert x + 2 * prime - y < 2**261

  // clang-format off
  // acc == 1
  // Proof:
  //   acc == 2**(BITS_PER_LIMB * LIMBS) + x[:LIMBS] - y[:LIMBS] + 2 * prime[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x - y + 2 * prime >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x + (2 * prime - y) >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) + 2**(BITS_PER_LIMB * LIMBS) - 1 >> BITS_PER_LIMB * LIMBS
  //     <= 2 * 2**(BITS_PER_LIMB * LIMBS) - 1 >> BITS_PER_LIMB * LIMBS
  //     == 1

  //   acc == 2**(BITS_PER_LIMB * LIMBS) + x[:LIMBS] - y[:LIMBS] + 2 * prime[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x - y + 2 * prime >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x + (2 * prime - y) >> BITS_PER_LIMB * LIMBS
  //     >= 2**(BITS_PER_LIMB * LIMBS) + 0 + 1 >> BITS_PER_LIMB * LIMBS
  //     == 1
  // clang-format on
}

// res = x - y
// Assumes x, y are normalized and x >= y
// Guarantees res is normalized
// Works properly even if &x == &y or &x == &res or &y == &res or
//   &x == &y == &res
void bn_subtract(const bignum256 *x, const bignum256 *y, bignum256 *res) {
  uint32_t acc = 1;
  for (int i = 0; i < BN_LIMBS; i++) {
    acc += (BN_BASE - 1) + x->val[i] - y->val[i];
    // acc neither overflows 32 bits nor underflows 0
    // Proof:
    //   acc + (BASE - 1) + x[i] - y[i]
    //     >= (BASE - 1) - y == (2**BITS_PER_LIMB - 1) - (2**BITS_PER_LIMB - 1)
    //     == 0
    //   acc + (BASE - 1) + x[i] - y[i]
    //     <= acc + (BASE - 1) + x[i]
    //     <= (2**(32 - BITS_PER_LIMB) - 1) + (2**BITS_PER_LIMB - 1) +
    //       (2**BITS_PER_LIMB - 1)
    //     == 7 + 2 * 2**29 < 2 **32

    res->val[i] = acc & BN_LIMB_MASK;
    acc >>= BN_BITS_PER_LIMB;
    // acc <= 7 == 2**(32 - BITS_PER_LIMB) - 1

    // acc == 2**(BITS_PER_LIMB * (i + 1)) + x[:i + 1] - y[:i + 1]
    //   >> BITS_PER_LIMB * (i + 1)
  }

  // assert(acc == 1); // assert x >= y

  // clang-format off
  // acc == 1
  // Proof:
  //   acc == 2**(BITS_PER_LIMB * LIMBS) + x[:LIMBS] - y[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x - y >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x >> BITS_PER_LIMB * LIMBS
  //     <= 2**(BITS_PER_LIMB * LIMBS) + 2**(BITS_PER_LIMB * LIMBS) - 1 >> BITS_PER_LIMB * LIMBS
  //     <= 2 * 2**(BITS_PER_LIMB * LIMBS) - 1 >> BITS_PER_LIMB * LIMBS
  //     == 1

  //   acc == 2**(BITS_PER_LIMB * LIMBS) + x[:LIMBS] - y[:LIMBS] >> BITS_PER_LIMB * LIMBS
  //     == 2**(BITS_PER_LIMB * LIMBS) + x - y >> BITS_PER_LIMB * LIMBS
  //     >= 2**(BITS_PER_LIMB * LIMBS) >> BITS_PER_LIMB * LIMBS
  //     == 1
}

// q = x // d, r = x % d
// Assumes x is normalized, 1 <= d <= 61304
// Guarantees q is normalized
void bn_long_division(bignum256 *x, uint32_t d, bignum256 *q, uint32_t *r) {
  assert(1 <= d && d < 61304);

  uint32_t acc = 0;

  *r = x->val[BN_LIMBS - 1] % d;
  q->val[BN_LIMBS - 1] = x->val[BN_LIMBS - 1] / d;

  for (int i = BN_LIMBS - 2; i >= 0; i--) {
    acc = *r * (BN_BASE % d) + x->val[i];
    // acc doesn't overflow 32 bits
    // Proof:
    //   r * (BASE % d) + x[i]
    //     <= (d - 1) * (d - 1) + (2**BITS_PER_LIMB - 1)
    //     == d**2 - 2*d + 2**BITS_PER_LIMB
    //     == 61304**2 - 2 * 61304 + 2**29
    //     == 3758057808 + 2**29  < 2**32

    q->val[i] = *r * (BN_BASE / d) + (acc / d);
    // q[i] doesn't overflow 32 bits
    // Proof:
    //   r * (BASE // d) + (acc // d)
    //     <= (d - 1) * (2**BITS_PER_LIMB / d) +
    //      ((d**2 - 2*d + 2**BITS_PER_LIMB) / d)
    //     <= (d - 1) * (2**BITS_PER_LIMB / d) + (d - 2 + 2**BITS_PER_LIMB / d)
    //     == (d - 1 + 1) * (2**BITS_PER_LIMB / d) + d - 2
    //     == 2**BITS_PER_LIMB + d - 2 <= 2**29 + 61304 < 2**32

    // q[i] == (r * BASE + x[i]) // d
    // Proof:
    //   q[i] == r * (BASE // d) + (acc // d)
    //     == r * (BASE // d) + (r * (BASE % d) + x[i]) // d
    //     == (r * d * (BASE // d) + r * (BASE % d) + x[i]) // d
    //     == (r * (d * (BASE // d) + (BASE % d)) + x[i]) // d
    //     == (r * BASE + x[i]) // d

    // q[i] < 2**BITS_PER_LIMB
    // Proof:
    //   q[i] == (r * BASE + x[i]) // d
    //     <= ((d - 1) * 2**BITS_PER_LIMB + (2**BITS_PER_LIMB - 1)) / d
    //     == (d * 2**BITS_PER_LIMB - 1) / d == 2**BITS_PER_LIMB - 1 / d
    //     <  2**BITS_PER_LIMB

    *r = acc % d;
    // r == (r * BASE + x[i]) % d
    // Proof:
    //   r == acc % d == (r * (BASE % d) + x[i]) % d
    //     == (r * BASE + x[i]) % d

    // x[:i] == q[:i] * d + r
  }
}

// x = x // 58, r = x % 58
// Assumes x is normalized
// Guarantees x is normalized
void bn_divmod58(bignum256 *x, uint32_t *r) { bn_long_division(x, 58, x, r); }

// x = x // 1000, r = x % 1000
// Assumes x is normalized
// Guarantees x is normalized
void bn_divmod1000(bignum256 *x, uint32_t *r) {
  bn_long_division(x, 1000, x, r);
}

// x = x // 10, r = x % 10
// Assumes x is normalized
// Guarantees x is normalized
void bn_divmod10(bignum256 *x, uint32_t *r) { bn_long_division(x, 10, x, r); }

// Formats amount
// Assumes amount is normalized
// Assumes prefix and suffix are null-terminated strings
// Assumes output is an array of length output_length
// The function doesn't have neither constant control flow nor constant memory
//   access flow with regard to any its argument
size_t bn_format(const bignum256 *amount, const char *prefix, const char *suffix, unsigned int decimals, int exponent, bool trailing, char thousands, char *output, size_t output_length) {

/*
  Python prototype of the function:

  def format(amount, prefix, suffix, decimals, exponent, trailing, thousands):
      if exponent >= 0:
          amount *= 10**exponent
      else:
          amount //= 10 ** (-exponent)

      d = pow(10, decimals)

      integer_part = amount // d
      integer_str = f"{integer_part:,}".replace(",", thousands or "")

      if decimals:
          decimal_part = amount % d
          decimal_str = f".{decimal_part:0{decimals}d}"
          if not trailing:
              decimal_str = decimal_str.rstrip("0").rstrip(".")
      else:
          decimal_str = ""

      return prefix + integer_str + decimal_str + suffix
*/

// Auxiliary macro for bn_format
// If enough space adds one character to output starting from the end
#define BN_FORMAT_ADD_OUTPUT_CHAR(c)                               \
  {                                                                \
    --position;                                                    \
    if (output <= position && position < output + output_length) { \
      *position = (c);                                             \
    } else {                                                       \
      memset(output, '\0', output_length);                         \
      return 0;                                                    \
    }                                                              \
  }

  bignum256 temp = {0};
  bn_copy(amount, &temp);
  uint32_t digit = 0;

  char *position = output + output_length;

  // Add string ending character
  BN_FORMAT_ADD_OUTPUT_CHAR('\0');

  // Add suffix
  size_t suffix_length = suffix ? strlen(suffix) : 0;
  for (int i = suffix_length - 1; i >= 0; --i)
    BN_FORMAT_ADD_OUTPUT_CHAR(suffix[i])

  // amount //= 10**exponent
  for (; exponent < 0; ++exponent) {
    // if temp == 0, there is no need to divide it by 10 anymore
    if (bn_is_zero(&temp)) {
      exponent = 0;
      break;
    }
    bn_divmod10(&temp, &digit);
  }

  // exponent >= 0 && decimals >= 0

  bool fractional_part = false;  // is fractional-part of amount present

  {  // Add fractional-part digits of amount
    // Add trailing zeroes
    unsigned int trailing_zeros = decimals < (unsigned int) exponent ? decimals : (unsigned int) exponent;
    // When casting a negative int to unsigned int, UINT_MAX is added to the int before
    // Since exponent >= 0, the value remains unchanged
    decimals -= trailing_zeros;
    exponent -= trailing_zeros;

    if (trailing && trailing_zeros) {
      fractional_part = true;
      for (; trailing_zeros > 0; --trailing_zeros)
          BN_FORMAT_ADD_OUTPUT_CHAR('0')
    }

    // exponent == 0 || decimals == 0

    // Add significant digits and leading zeroes
    for (; decimals > 0; --decimals) {
      bn_divmod10(&temp, &digit);

      if (fractional_part || digit || trailing) {
        fractional_part = true;
        BN_FORMAT_ADD_OUTPUT_CHAR('0' + digit)
      }
      else if (bn_is_zero(&temp)) {
        // We break since the remaining digits are zeroes and fractional_part == trailing == false
        decimals = 0;
        break;
      }
    }
    // decimals == 0
  }

  if (fractional_part) {
    BN_FORMAT_ADD_OUTPUT_CHAR('.')
  }

  {  // Add integer-part digits of amount
    // Add trailing zeroes
    int digits = 0;
    if (!bn_is_zero(&temp)) {
      for (; exponent > 0; --exponent) {
        ++digits;
        BN_FORMAT_ADD_OUTPUT_CHAR('0')
        if (thousands != 0 && digits % 3 == 0) {
          BN_FORMAT_ADD_OUTPUT_CHAR(thousands)
        }
      }
    }
    // decimals == 0 && exponent == 0

    // Add significant digits
    bool is_zero = false;
    do {
      ++digits;
      bn_divmod10(&temp, &digit);
      is_zero = bn_is_zero(&temp);
      BN_FORMAT_ADD_OUTPUT_CHAR('0' + digit)
      if (thousands != 0 && !is_zero && digits % 3 == 0) {
        BN_FORMAT_ADD_OUTPUT_CHAR(thousands)
      }
    } while (!is_zero);
  }

  // Add prefix
  size_t prefix_length = prefix ? strlen(prefix) : 0;
  for (int i = prefix_length - 1; i >= 0; --i)
    BN_FORMAT_ADD_OUTPUT_CHAR(prefix[i])

  // Move formatted amount to the start of output
  int length = output - position + output_length;
  memmove(output, position, length);
  return length - 1;
}

#if USE_BN_PRINT
// Prints x in hexadecimal
// Assumes x is normalized and x < 2**256
void bn_print(const bignum256 *x) {
  printf("%06x", x->val[8]);
  printf("%08x", ((x->val[7] << 3) | (x->val[6] >> 26)));
  printf("%07x", ((x->val[6] << 2) | (x->val[5] >> 27)) & 0x0FFFFFFF);
  printf("%07x", ((x->val[5] << 1) | (x->val[4] >> 28)) & 0x0FFFFFFF);
  printf("%07x", x->val[4] & 0x0FFFFFFF);
  printf("%08x", ((x->val[3] << 3) | (x->val[2] >> 26)));
  printf("%07x", ((x->val[2] << 2) | (x->val[1] >> 27)) & 0x0FFFFFFF);
  printf("%07x", ((x->val[1] << 1) | (x->val[0] >> 28)) & 0x0FFFFFFF);
  printf("%07x", x->val[0] & 0x0FFFFFFF);
}

// Prints comma separated list of limbs of x
void bn_print_raw(const bignum256 *x) {
  for (int i = 0; i < BN_LIMBS - 1; i++) {
    printf("0x%08x, ", x->val[i]);
  }
  printf("0x%08x", x->val[BN_LIMBS - 1]);
}
#endif

#if USE_INVERSE_FAST
void bn_inverse(bignum256 *x, const bignum256 *prime) {
  bn_inverse_fast(x, prime);
}
#else
void bn_inverse(bignum256 *x, const bignum256 *prime) {
  bn_inverse_slow(x, prime);
}
#endif
