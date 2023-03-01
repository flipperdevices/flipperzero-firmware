/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 * Copyright (c)      2015 Jochen Hoenicke
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

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "address.h"
#include "base58.h"
#include "bignum.h"
#include "ecdsa.h"
#include "hmac.h"
#include "memzero.h"
#include "rand.h"
#include "rfc6979.h"
#include "secp256k1.h"

// Set cp2 = cp1
void point_copy(const curve_point *cp1, curve_point *cp2) { *cp2 = *cp1; }

// cp2 = cp1 + cp2
void point_add(const ecdsa_curve *curve, const curve_point *cp1,
               curve_point *cp2) {
  bignum256 lambda = {0}, inv = {0}, xr = {0}, yr = {0};

  if (point_is_infinity(cp1)) {
    return;
  }
  if (point_is_infinity(cp2)) {
    point_copy(cp1, cp2);
    return;
  }
  if (point_is_equal(cp1, cp2)) {
    point_double(curve, cp2);
    return;
  }
  if (point_is_negative_of(cp1, cp2)) {
    point_set_infinity(cp2);
    return;
  }

  // lambda = (y2 - y1) / (x2 - x1)
  bn_subtractmod(&(cp2->x), &(cp1->x), &inv, &curve->prime);
  bn_inverse(&inv, &curve->prime);
  bn_subtractmod(&(cp2->y), &(cp1->y), &lambda, &curve->prime);
  bn_multiply(&inv, &lambda, &curve->prime);

  // xr = lambda^2 - x1 - x2
  xr = lambda;
  bn_multiply(&xr, &xr, &curve->prime);
  yr = cp1->x;
  bn_addmod(&yr, &(cp2->x), &curve->prime);
  bn_subtractmod(&xr, &yr, &xr, &curve->prime);
  bn_fast_mod(&xr, &curve->prime);
  bn_mod(&xr, &curve->prime);

  // yr = lambda (x1 - xr) - y1
  bn_subtractmod(&(cp1->x), &xr, &yr, &curve->prime);
  bn_multiply(&lambda, &yr, &curve->prime);
  bn_subtractmod(&yr, &(cp1->y), &yr, &curve->prime);
  bn_fast_mod(&yr, &curve->prime);
  bn_mod(&yr, &curve->prime);

  cp2->x = xr;
  cp2->y = yr;
}

// cp = cp + cp
void point_double(const ecdsa_curve *curve, curve_point *cp) {
  bignum256 lambda = {0}, xr = {0}, yr = {0};

  if (point_is_infinity(cp)) {
    return;
  }
  if (bn_is_zero(&(cp->y))) {
    point_set_infinity(cp);
    return;
  }

  // lambda = (3 x^2 + a) / (2 y)
  lambda = cp->y;
  bn_mult_k(&lambda, 2, &curve->prime);
  bn_fast_mod(&lambda, &curve->prime);
  bn_mod(&lambda, &curve->prime);
  bn_inverse(&lambda, &curve->prime);

  xr = cp->x;
  bn_multiply(&xr, &xr, &curve->prime);
  bn_mult_k(&xr, 3, &curve->prime);
  bn_subi(&xr, -curve->a, &curve->prime);
  bn_multiply(&xr, &lambda, &curve->prime);

  // xr = lambda^2 - 2*x
  xr = lambda;
  bn_multiply(&xr, &xr, &curve->prime);
  yr = cp->x;
  bn_lshift(&yr);
  bn_subtractmod(&xr, &yr, &xr, &curve->prime);
  bn_fast_mod(&xr, &curve->prime);
  bn_mod(&xr, &curve->prime);

  // yr = lambda (x - xr) - y
  bn_subtractmod(&(cp->x), &xr, &yr, &curve->prime);
  bn_multiply(&lambda, &yr, &curve->prime);
  bn_subtractmod(&yr, &(cp->y), &yr, &curve->prime);
  bn_fast_mod(&yr, &curve->prime);
  bn_mod(&yr, &curve->prime);

  cp->x = xr;
  cp->y = yr;
}

// set point to internal representation of point at infinity
void point_set_infinity(curve_point *p) {
  bn_zero(&(p->x));
  bn_zero(&(p->y));
}

// return true iff p represent point at infinity
// both coords are zero in internal representation
int point_is_infinity(const curve_point *p) {
  return bn_is_zero(&(p->x)) && bn_is_zero(&(p->y));
}

// return true iff both points are equal
int point_is_equal(const curve_point *p, const curve_point *q) {
  return bn_is_equal(&(p->x), &(q->x)) && bn_is_equal(&(p->y), &(q->y));
}

// returns true iff p == -q
// expects p and q be valid points on curve other than point at infinity
int point_is_negative_of(const curve_point *p, const curve_point *q) {
  // if P == (x, y), then -P would be (x, -y) on this curve
  if (!bn_is_equal(&(p->x), &(q->x))) {
    return 0;
  }

  // we shouldn't hit this for a valid point
  if (bn_is_zero(&(p->y))) {
    return 0;
  }

  return !bn_is_equal(&(p->y), &(q->y));
}

typedef struct jacobian_curve_point {
  bignum256 x, y, z;
} jacobian_curve_point;

// generate random K for signing/side-channel noise
static void generate_k_random(bignum256 *k, const bignum256 *prime) {
  do {
    int i = 0;
    for (i = 0; i < 8; i++) {
      k->val[i] = random32() & ((1u << BN_BITS_PER_LIMB) - 1);
    }
    k->val[8] = random32() & ((1u << BN_BITS_LAST_LIMB) - 1);
    // check that k is in range and not zero.
  } while (bn_is_zero(k) || !bn_is_less(k, prime));
}

void curve_to_jacobian(const curve_point *p, jacobian_curve_point *jp,
                       const bignum256 *prime) {
  // randomize z coordinate
  generate_k_random(&jp->z, prime);

  jp->x = jp->z;
  bn_multiply(&jp->z, &jp->x, prime);
  // x = z^2
  jp->y = jp->x;
  bn_multiply(&jp->z, &jp->y, prime);
  // y = z^3

  bn_multiply(&p->x, &jp->x, prime);
  bn_multiply(&p->y, &jp->y, prime);
}

void jacobian_to_curve(const jacobian_curve_point *jp, curve_point *p,
                       const bignum256 *prime) {
  p->y = jp->z;
  bn_inverse(&p->y, prime);
  // p->y = z^-1
  p->x = p->y;
  bn_multiply(&p->x, &p->x, prime);
  // p->x = z^-2
  bn_multiply(&p->x, &p->y, prime);
  // p->y = z^-3
  bn_multiply(&jp->x, &p->x, prime);
  // p->x = jp->x * z^-2
  bn_multiply(&jp->y, &p->y, prime);
  // p->y = jp->y * z^-3
  bn_mod(&p->x, prime);
  bn_mod(&p->y, prime);
}

void point_jacobian_add(const curve_point *p1, jacobian_curve_point *p2,
                        const ecdsa_curve *curve) {
  bignum256 r = {0}, h = {0}, r2 = {0};
  bignum256 hcby = {0}, hsqx = {0};
  bignum256 xz = {0}, yz = {0}, az = {0};
  int is_doubling = 0;
  const bignum256 *prime = &curve->prime;
  int a = curve->a;

  assert(-3 <= a && a <= 0);

  /* First we bring p1 to the same denominator:
   * x1' := x1 * z2^2
   * y1' := y1 * z2^3
   */
  /*
   * lambda  = ((y1' - y2)/z2^3) / ((x1' - x2)/z2^2)
   *         = (y1' - y2) / (x1' - x2) z2
   * x3/z3^2 = lambda^2 - (x1' + x2)/z2^2
   * y3/z3^3 = 1/2 lambda * (2x3/z3^2 - (x1' + x2)/z2^2) + (y1'+y2)/z2^3
   *
   * For the special case x1=x2, y1=y2 (doubling) we have
   * lambda = 3/2 ((x2/z2^2)^2 + a) / (y2/z2^3)
   *        = 3/2 (x2^2 + a*z2^4) / y2*z2)
   *
   * to get rid of fraction we write lambda as
   * lambda = r / (h*z2)
   * with  r = is_doubling ? 3/2 x2^2 + az2^4 : (y1 - y2)
   *       h = is_doubling ?      y1+y2       : (x1 - x2)
   *
   * With z3 = h*z2  (the denominator of lambda)
   * we get x3 = lambda^2*z3^2 - (x1' + x2)/z2^2*z3^2
   *           = r^2 - h^2 * (x1' + x2)
   *    and y3 = 1/2 r * (2x3 - h^2*(x1' + x2)) + h^3*(y1' + y2)
   */

  /* h = x1 - x2
   * r = y1 - y2
   * x3 = r^2 - h^3 - 2*h^2*x2
   * y3 = r*(h^2*x2 - x3) - h^3*y2
   * z3 = h*z2
   */

  xz = p2->z;
  bn_multiply(&xz, &xz, prime);  // xz = z2^2
  yz = p2->z;
  bn_multiply(&xz, &yz, prime);  // yz = z2^3

  if (a != 0) {
    az = xz;
    bn_multiply(&az, &az, prime);  // az = z2^4
    bn_mult_k(&az, -a, prime);     // az = -az2^4
  }

  bn_multiply(&p1->x, &xz, prime);  // xz = x1' = x1*z2^2;
  h = xz;
  bn_subtractmod(&h, &p2->x, &h, prime);
  bn_fast_mod(&h, prime);
  // h = x1' - x2;

  bn_add(&xz, &p2->x);
  // xz = x1' + x2

  // check for h == 0 % prime.  Note that h never normalizes to
  // zero, since h = x1' + 2*prime - x2 > 0 and a positive
  // multiple of prime is always normalized to prime by
  // bn_fast_mod.
  is_doubling = bn_is_equal(&h, prime);

  bn_multiply(&p1->y, &yz, prime);  // yz = y1' = y1*z2^3;
  bn_subtractmod(&yz, &p2->y, &r, prime);
  // r = y1' - y2;

  bn_add(&yz, &p2->y);
  // yz = y1' + y2

  r2 = p2->x;
  bn_multiply(&r2, &r2, prime);
  bn_mult_k(&r2, 3, prime);

  if (a != 0) {
    // subtract -a z2^4, i.e, add a z2^4
    bn_subtractmod(&r2, &az, &r2, prime);
  }
  bn_cmov(&r, is_doubling, &r2, &r);
  bn_cmov(&h, is_doubling, &yz, &h);

  // hsqx = h^2
  hsqx = h;
  bn_multiply(&hsqx, &hsqx, prime);

  // hcby = h^3
  hcby = h;
  bn_multiply(&hsqx, &hcby, prime);

  // hsqx = h^2 * (x1 + x2)
  bn_multiply(&xz, &hsqx, prime);

  // hcby = h^3 * (y1 + y2)
  bn_multiply(&yz, &hcby, prime);

  // z3 = h*z2
  bn_multiply(&h, &p2->z, prime);

  // x3 = r^2 - h^2 (x1 + x2)
  p2->x = r;
  bn_multiply(&p2->x, &p2->x, prime);
  bn_subtractmod(&p2->x, &hsqx, &p2->x, prime);
  bn_fast_mod(&p2->x, prime);

  // y3 = 1/2 (r*(h^2 (x1 + x2) - 2x3) - h^3 (y1 + y2))
  bn_subtractmod(&hsqx, &p2->x, &p2->y, prime);
  bn_subtractmod(&p2->y, &p2->x, &p2->y, prime);
  bn_multiply(&r, &p2->y, prime);
  bn_subtractmod(&p2->y, &hcby, &p2->y, prime);
  bn_mult_half(&p2->y, prime);
  bn_fast_mod(&p2->y, prime);
}

void point_jacobian_double(jacobian_curve_point *p, const ecdsa_curve *curve) {
  bignum256 az4 = {0}, m = {0}, msq = {0}, ysq = {0}, xysq = {0};
  const bignum256 *prime = &curve->prime;

  assert(-3 <= curve->a && curve->a <= 0);
  /* usual algorithm:
   *
   * lambda  = (3((x/z^2)^2 + a) / 2y/z^3) = (3x^2 + az^4)/2yz
   * x3/z3^2 = lambda^2 - 2x/z^2
   * y3/z3^3 = lambda * (x/z^2 - x3/z3^2) - y/z^3
   *
   * to get rid of fraction we set
   *  m = (3 x^2 + az^4) / 2
   * Hence,
   *  lambda = m / yz = m / z3
   *
   * With z3 = yz  (the denominator of lambda)
   * we get x3 = lambda^2*z3^2 - 2*x/z^2*z3^2
   *           = m^2 - 2*xy^2
   *    and y3 = (lambda * (x/z^2 - x3/z3^2) - y/z^3) * z3^3
   *           = m * (xy^2 - x3) - y^4
   */

  /* m = (3*x^2 + a z^4) / 2
   * x3 = m^2 - 2*xy^2
   * y3 = m*(xy^2 - x3) - 8y^4
   * z3 = y*z
   */

  m = p->x;
  bn_multiply(&m, &m, prime);
  bn_mult_k(&m, 3, prime);

  az4 = p->z;
  bn_multiply(&az4, &az4, prime);
  bn_multiply(&az4, &az4, prime);
  bn_mult_k(&az4, -curve->a, prime);
  bn_subtractmod(&m, &az4, &m, prime);
  bn_mult_half(&m, prime);

  // msq = m^2
  msq = m;
  bn_multiply(&msq, &msq, prime);
  // ysq = y^2
  ysq = p->y;
  bn_multiply(&ysq, &ysq, prime);
  // xysq = xy^2
  xysq = p->x;
  bn_multiply(&ysq, &xysq, prime);

  // z3 = yz
  bn_multiply(&p->y, &p->z, prime);

  // x3 = m^2 - 2*xy^2
  p->x = xysq;
  bn_lshift(&p->x);
  bn_fast_mod(&p->x, prime);
  bn_subtractmod(&msq, &p->x, &p->x, prime);
  bn_fast_mod(&p->x, prime);

  // y3 = m*(xy^2 - x3) - y^4
  bn_subtractmod(&xysq, &p->x, &p->y, prime);
  bn_multiply(&m, &p->y, prime);
  bn_multiply(&ysq, &ysq, prime);
  bn_subtractmod(&p->y, &ysq, &p->y, prime);
  bn_fast_mod(&p->y, prime);
}

// res = k * p
// returns 0 on success
int point_multiply(const ecdsa_curve *curve, const bignum256 *k,
                   const curve_point *p, curve_point *res) {
  // this algorithm is loosely based on
  //  Katsuyuki Okeya and Tsuyoshi Takagi, The Width-w NAF Method Provides
  //  Small Memory and Fast Elliptic Scalar Multiplications Secure against
  //  Side Channel Attacks.
  if (!bn_is_less(k, &curve->order)) {
    return 1;
  }

  int i = 0, j = 0;
  static CONFIDENTIAL bignum256 a;
  uint32_t *aptr = NULL;
  uint32_t abits = 0;
  int ashift = 0;
  uint32_t is_even = (k->val[0] & 1) - 1;
  uint32_t bits = {0}, sign = {0}, nsign = {0};
  static CONFIDENTIAL jacobian_curve_point jres;
  curve_point pmult[8] = {0};
  const bignum256 *prime = &curve->prime;

  // is_even = 0xffffffff if k is even, 0 otherwise.

  // add 2^256.
  // make number odd: subtract curve->order if even
  uint32_t tmp = 1;
  uint32_t is_non_zero = 0;
  for (j = 0; j < 8; j++) {
    is_non_zero |= k->val[j];
    tmp += (BN_BASE - 1) + k->val[j] - (curve->order.val[j] & is_even);
    a.val[j] = tmp & (BN_BASE - 1);
    tmp >>= BN_BITS_PER_LIMB;
  }
  is_non_zero |= k->val[j];
  a.val[j] = tmp + 0xffffff + k->val[j] - (curve->order.val[j] & is_even);
  assert((a.val[0] & 1) != 0);

  // special case 0*p:  just return zero. We don't care about constant time.
  if (!is_non_zero) {
    point_set_infinity(res);
    return 1;
  }

  // Now a = k + 2^256 (mod curve->order) and a is odd.
  //
  // The idea is to bring the new a into the form.
  // sum_{i=0..64} a[i] 16^i,  where |a[i]| < 16 and a[i] is odd.
  // a[0] is odd, since a is odd.  If a[i] would be even, we can
  // add 1 to it and subtract 16 from a[i-1].  Afterwards,
  // a[64] = 1, which is the 2^256 that we added before.
  //
  // Since k = a - 2^256 (mod curve->order), we can compute
  //   k*p = sum_{i=0..63} a[i] 16^i * p
  //
  // We compute |a[i]| * p in advance for all possible
  // values of |a[i]| * p.  pmult[i] = (2*i+1) * p
  // We compute p, 3*p, ..., 15*p and store it in the table pmult.
  // store p^2 temporarily in pmult[7]
  pmult[7] = *p;
  point_double(curve, &pmult[7]);
  // compute 3*p, etc by repeatedly adding p^2.
  pmult[0] = *p;
  for (i = 1; i < 8; i++) {
    pmult[i] = pmult[7];
    point_add(curve, &pmult[i - 1], &pmult[i]);
  }

  // now compute  res = sum_{i=0..63} a[i] * 16^i * p step by step,
  // starting with i = 63.
  // initialize jres = |a[63]| * p.
  // Note that a[i] = a>>(4*i) & 0xf if (a&0x10) != 0
  // and - (16 - (a>>(4*i) & 0xf)) otherwise.   We can compute this as
  //   ((a ^ (((a >> 4) & 1) - 1)) & 0xf) >> 1
  // since a is odd.
  aptr = &a.val[8];
  abits = *aptr;
  ashift = 256 - (BN_BITS_PER_LIMB * 8) - 4;
  bits = abits >> ashift;
  sign = (bits >> 4) - 1;
  bits ^= sign;
  bits &= 15;
  curve_to_jacobian(&pmult[bits >> 1], &jres, prime);
  for (i = 62; i >= 0; i--) {
    // sign = sign(a[i+1])  (0xffffffff for negative, 0 for positive)
    // invariant jres = (-1)^sign sum_{j=i+1..63} (a[j] * 16^{j-i-1} * p)
    // abits >> (ashift - 4) = lowbits(a >> (i*4))

    point_jacobian_double(&jres, curve);
    point_jacobian_double(&jres, curve);
    point_jacobian_double(&jres, curve);
    point_jacobian_double(&jres, curve);

    // get lowest 5 bits of a >> (i*4).
    ashift -= 4;
    if (ashift < 0) {
      // the condition only depends on the iteration number and
      // leaks no private information to a side-channel.
      bits = abits << (-ashift);
      abits = *(--aptr);
      ashift += BN_BITS_PER_LIMB;
      bits |= abits >> ashift;
    } else {
      bits = abits >> ashift;
    }
    bits &= 31;
    nsign = (bits >> 4) - 1;
    bits ^= nsign;
    bits &= 15;

    // negate last result to make signs of this round and the
    // last round equal.
    bn_cnegate((sign ^ nsign) & 1, &jres.z, prime);

    // add odd factor
    point_jacobian_add(&pmult[bits >> 1], &jres, curve);
    sign = nsign;
  }
  bn_cnegate(sign & 1, &jres.z, prime);
  jacobian_to_curve(&jres, res, prime);
  memzero(&a, sizeof(a));
  memzero(&jres, sizeof(jres));

  return 0;
}

#if USE_PRECOMPUTED_CP

// res = k * G
// k must be a normalized number with 0 <= k < curve->order
// returns 0 on success
int scalar_multiply(const ecdsa_curve *curve, const bignum256 *k,
                    curve_point *res) {
  if (!bn_is_less(k, &curve->order)) {
    return 1;
  }

  int i = {0}, j = {0};
  static CONFIDENTIAL bignum256 a;
  uint32_t is_even = (k->val[0] & 1) - 1;
  uint32_t lowbits = 0;
  static CONFIDENTIAL jacobian_curve_point jres;
  const bignum256 *prime = &curve->prime;

  // is_even = 0xffffffff if k is even, 0 otherwise.

  // add 2^256.
  // make number odd: subtract curve->order if even
  uint32_t tmp = 1;
  uint32_t is_non_zero = 0;
  for (j = 0; j < 8; j++) {
    is_non_zero |= k->val[j];
    tmp += (BN_BASE - 1) + k->val[j] - (curve->order.val[j] & is_even);
    a.val[j] = tmp & (BN_BASE - 1);
    tmp >>= BN_BITS_PER_LIMB;
  }
  is_non_zero |= k->val[j];
  a.val[j] = tmp + 0xffffff + k->val[j] - (curve->order.val[j] & is_even);
  assert((a.val[0] & 1) != 0);

  // special case 0*G:  just return zero. We don't care about constant time.
  if (!is_non_zero) {
    point_set_infinity(res);
    return 0;
  }

  // Now a = k + 2^256 (mod curve->order) and a is odd.
  //
  // The idea is to bring the new a into the form.
  // sum_{i=0..64} a[i] 16^i,  where |a[i]| < 16 and a[i] is odd.
  // a[0] is odd, since a is odd.  If a[i] would be even, we can
  // add 1 to it and subtract 16 from a[i-1].  Afterwards,
  // a[64] = 1, which is the 2^256 that we added before.
  //
  // Since k = a - 2^256 (mod curve->order), we can compute
  //   k*G = sum_{i=0..63} a[i] 16^i * G
  //
  // We have a big table curve->cp that stores all possible
  // values of |a[i]| 16^i * G.
  // curve->cp[i][j] = (2*j+1) * 16^i * G

  // now compute  res = sum_{i=0..63} a[i] * 16^i * G step by step.
  // initial res = |a[0]| * G.  Note that a[0] = a & 0xf if (a&0x10) != 0
  // and - (16 - (a & 0xf)) otherwise.   We can compute this as
  //   ((a ^ (((a >> 4) & 1) - 1)) & 0xf) >> 1
  // since a is odd.
  lowbits = a.val[0] & ((1 << 5) - 1);
  lowbits ^= (lowbits >> 4) - 1;
  lowbits &= 15;
  curve_to_jacobian(&curve->cp[0][lowbits >> 1], &jres, prime);
  for (i = 1; i < 64; i++) {
    // invariant res = sign(a[i-1]) sum_{j=0..i-1} (a[j] * 16^j * G)

    // shift a by 4 places.
    for (j = 0; j < 8; j++) {
      a.val[j] =
          (a.val[j] >> 4) | ((a.val[j + 1] & 0xf) << (BN_BITS_PER_LIMB - 4));
    }
    a.val[j] >>= 4;
    // a = old(a)>>(4*i)
    // a is even iff sign(a[i-1]) = -1

    lowbits = a.val[0] & ((1 << 5) - 1);
    lowbits ^= (lowbits >> 4) - 1;
    lowbits &= 15;
    // negate last result to make signs of this round and the
    // last round equal.
    bn_cnegate(~lowbits & 1, &jres.y, prime);

    // add odd factor
    point_jacobian_add(&curve->cp[i][lowbits >> 1], &jres, curve);
  }
  bn_cnegate(~(a.val[0] >> 4) & 1, &jres.y, prime);
  jacobian_to_curve(&jres, res, prime);
  memzero(&a, sizeof(a));
  memzero(&jres, sizeof(jres));

  return 0;
}

#else

int scalar_multiply(const ecdsa_curve *curve, const bignum256 *k,
                    curve_point *res) {
  return point_multiply(curve, k, &curve->G, res);
}

#endif

int ecdh_multiply(const ecdsa_curve *curve, const uint8_t *priv_key,
                  const uint8_t *pub_key, uint8_t *session_key) {
  curve_point point = {0};
  if (!ecdsa_read_pubkey(curve, pub_key, &point)) {
    return 1;
  }

  bignum256 k = {0};
  bn_read_be(priv_key, &k);
  if (bn_is_zero(&k) || !bn_is_less(&k, &curve->order)) {
    // Invalid private key.
    return 2;
  }

  point_multiply(curve, &k, &point, &point);
  memzero(&k, sizeof(k));

  session_key[0] = 0x04;
  bn_write_be(&point.x, session_key + 1);
  bn_write_be(&point.y, session_key + 33);
  memzero(&point, sizeof(point));

  return 0;
}

// msg is a data to be signed
// msg_len is the message length
int ecdsa_sign(const ecdsa_curve *curve, HasherType hasher_sign,
               const uint8_t *priv_key, const uint8_t *msg, uint32_t msg_len,
               uint8_t *sig, uint8_t *pby,
               int (*is_canonical)(uint8_t by, uint8_t sig[64])) {
  uint8_t hash[32] = {0};
  hasher_Raw(hasher_sign, msg, msg_len, hash);
  int res = ecdsa_sign_digest(curve, priv_key, hash, sig, pby, is_canonical);
  memzero(hash, sizeof(hash));
  return res;
}

// uses secp256k1 curve
// priv_key is a 32 byte big endian stored number
// sig is 64 bytes long array for the signature
// digest is 32 bytes of digest
// is_canonical is an optional function that checks if the signature
// conforms to additional coin-specific rules.
int ecdsa_sign_digest(const ecdsa_curve *curve, const uint8_t *priv_key,
                      const uint8_t *digest, uint8_t *sig, uint8_t *pby,
                      int (*is_canonical)(uint8_t by, uint8_t sig[64])) {
  int i = 0;
  curve_point R = {0};
  bignum256 k = {0}, z = {0}, randk = {0};
  bignum256 *s = &R.y;
  uint8_t by;  // signature recovery byte

#if USE_RFC6979
  rfc6979_state rng = {0};
  init_rfc6979(priv_key, digest, curve, &rng);
#endif

  bn_read_be(digest, &z);
  if (bn_is_zero(&z)) {
    // The probability of the digest being all-zero by chance is infinitesimal,
    // so this is most likely an indication of a bug. Furthermore, the signature
    // has no value, because in this case it can be easily forged for any public
    // key, see ecdsa_verify_digest().
    return 1;
  }

  for (i = 0; i < 10000; i++) {
#if USE_RFC6979
    // generate K deterministically
    generate_k_rfc6979(&k, &rng);
    // if k is too big or too small, we don't like it
    if (bn_is_zero(&k) || !bn_is_less(&k, &curve->order)) {
      continue;
    }
#else
    // generate random number k
    generate_k_random(&k, &curve->order);
#endif

    // compute k*G
    scalar_multiply(curve, &k, &R);
    by = R.y.val[0] & 1;
    // r = (rx mod n)
    if (!bn_is_less(&R.x, &curve->order)) {
      bn_subtract(&R.x, &curve->order, &R.x);
      by |= 2;
    }
    // if r is zero, we retry
    if (bn_is_zero(&R.x)) {
      continue;
    }

    bn_read_be(priv_key, s);
    if (bn_is_zero(s) || !bn_is_less(s, &curve->order)) {
      // Invalid private key.
      return 2;
    }

    // randomize operations to counter side-channel attacks
    generate_k_random(&randk, &curve->order);
    bn_multiply(&randk, &k, &curve->order);  // k*rand
    bn_inverse(&k, &curve->order);           // (k*rand)^-1
    bn_multiply(&R.x, s, &curve->order);     // R.x*priv
    bn_add(s, &z);                           // R.x*priv + z
    bn_multiply(&k, s, &curve->order);       // (k*rand)^-1 (R.x*priv + z)
    bn_multiply(&randk, s, &curve->order);   // k^-1 (R.x*priv + z)
    bn_mod(s, &curve->order);
    // if s is zero, we retry
    if (bn_is_zero(s)) {
      continue;
    }

    // if S > order/2 => S = -S
    if (bn_is_less(&curve->order_half, s)) {
      bn_subtract(&curve->order, s, s);
      by ^= 1;
    }
    // we are done, R.x and s is the result signature
    bn_write_be(&R.x, sig);
    bn_write_be(s, sig + 32);

    // check if the signature is acceptable or retry
    if (is_canonical && !is_canonical(by, sig)) {
      continue;
    }

    if (pby) {
      *pby = by;
    }

    memzero(&k, sizeof(k));
    memzero(&randk, sizeof(randk));
#if USE_RFC6979
    memzero(&rng, sizeof(rng));
#endif
    return 0;
  }

  // Too many retries without a valid signature
  // -> fail with an error
  memzero(&k, sizeof(k));
  memzero(&randk, sizeof(randk));
#if USE_RFC6979
  memzero(&rng, sizeof(rng));
#endif
  return -1;
}

// returns 0 on success
int ecdsa_get_public_key33(const ecdsa_curve *curve, const uint8_t *priv_key,
                           uint8_t *pub_key) {
  curve_point R = {0};
  bignum256 k = {0};

  bn_read_be(priv_key, &k);
  if (bn_is_zero(&k) || !bn_is_less(&k, &curve->order)) {
    // Invalid private key.
    memzero(pub_key, 33);
    return -1;
  }

  // compute k*G
  if (scalar_multiply(curve, &k, &R) != 0) {
    memzero(&k, sizeof(k));
    return 1;
  }
  pub_key[0] = 0x02 | (R.y.val[0] & 0x01);
  bn_write_be(&R.x, pub_key + 1);
  memzero(&R, sizeof(R));
  memzero(&k, sizeof(k));
  return 0;
}

// returns 0 on success
int ecdsa_get_public_key65(const ecdsa_curve *curve, const uint8_t *priv_key,
                           uint8_t *pub_key) {
  curve_point R = {0};
  bignum256 k = {0};

  bn_read_be(priv_key, &k);
  if (bn_is_zero(&k) || !bn_is_less(&k, &curve->order)) {
    // Invalid private key.
    memzero(pub_key, 65);
    return -1;
  }

  // compute k*G
  if (scalar_multiply(curve, &k, &R) != 0) {
    memzero(&k, sizeof(k));
    return 1;
  }
  pub_key[0] = 0x04;
  bn_write_be(&R.x, pub_key + 1);
  bn_write_be(&R.y, pub_key + 33);
  memzero(&R, sizeof(R));
  memzero(&k, sizeof(k));
  return 0;
}

int ecdsa_uncompress_pubkey(const ecdsa_curve *curve, const uint8_t *pub_key,
                            uint8_t *uncompressed) {
  curve_point pub = {0};

  if (!ecdsa_read_pubkey(curve, pub_key, &pub)) {
    return 0;
  }

  uncompressed[0] = 4;
  bn_write_be(&pub.x, uncompressed + 1);
  bn_write_be(&pub.y, uncompressed + 33);

  return 1;
}

void ecdsa_get_pubkeyhash(const uint8_t *pub_key, HasherType hasher_pubkey,
                          uint8_t *pubkeyhash) {
  uint8_t h[HASHER_DIGEST_LENGTH] = {0};
  if (pub_key[0] == 0x04) {  // uncompressed format
    hasher_Raw(hasher_pubkey, pub_key, 65, h);
  } else if (pub_key[0] == 0x00) {  // point at infinity
    hasher_Raw(hasher_pubkey, pub_key, 1, h);
  } else {  // expecting compressed format
    hasher_Raw(hasher_pubkey, pub_key, 33, h);
  }
  memcpy(pubkeyhash, h, 20);
  memzero(h, sizeof(h));
}

void ecdsa_get_address_raw(const uint8_t *pub_key, uint32_t version,
                           HasherType hasher_pubkey, uint8_t *addr_raw) {
  size_t prefix_len = address_prefix_bytes_len(version);
  address_write_prefix_bytes(version, addr_raw);
  ecdsa_get_pubkeyhash(pub_key, hasher_pubkey, addr_raw + prefix_len);
}

void ecdsa_get_address(const uint8_t *pub_key, uint32_t version,
                       HasherType hasher_pubkey, HasherType hasher_base58,
                       char *addr, int addrsize) {
  uint8_t raw[MAX_ADDR_RAW_SIZE] = {0};
  size_t prefix_len = address_prefix_bytes_len(version);
  ecdsa_get_address_raw(pub_key, version, hasher_pubkey, raw);
  base58_encode_check(raw, 20 + prefix_len, hasher_base58, addr, addrsize);
  // not as important to clear this one, but we might as well
  memzero(raw, sizeof(raw));
}

void ecdsa_get_address_segwit_p2sh_raw(const uint8_t *pub_key, uint32_t version,
                                       HasherType hasher_pubkey,
                                       uint8_t *addr_raw) {
  uint8_t buf[32 + 2] = {0};
  buf[0] = 0;   // version byte
  buf[1] = 20;  // push 20 bytes
  ecdsa_get_pubkeyhash(pub_key, hasher_pubkey, buf + 2);
  size_t prefix_len = address_prefix_bytes_len(version);
  address_write_prefix_bytes(version, addr_raw);
  hasher_Raw(hasher_pubkey, buf, 22, addr_raw + prefix_len);
}

void ecdsa_get_address_segwit_p2sh(const uint8_t *pub_key, uint32_t version,
                                   HasherType hasher_pubkey,
                                   HasherType hasher_base58, char *addr,
                                   int addrsize) {
  uint8_t raw[MAX_ADDR_RAW_SIZE] = {0};
  size_t prefix_len = address_prefix_bytes_len(version);
  ecdsa_get_address_segwit_p2sh_raw(pub_key, version, hasher_pubkey, raw);
  base58_encode_check(raw, prefix_len + 20, hasher_base58, addr, addrsize);
  memzero(raw, sizeof(raw));
}

void ecdsa_get_wif(const uint8_t *priv_key, uint32_t version,
                   HasherType hasher_base58, char *wif, int wifsize) {
  uint8_t wif_raw[MAX_WIF_RAW_SIZE] = {0};
  size_t prefix_len = address_prefix_bytes_len(version);
  address_write_prefix_bytes(version, wif_raw);
  memcpy(wif_raw + prefix_len, priv_key, 32);
  wif_raw[prefix_len + 32] = 0x01;
  base58_encode_check(wif_raw, prefix_len + 32 + 1, hasher_base58, wif,
                      wifsize);
  // private keys running around our stack can cause trouble
  memzero(wif_raw, sizeof(wif_raw));
}

int ecdsa_address_decode(const char *addr, uint32_t version,
                         HasherType hasher_base58, uint8_t *out) {
  if (!addr) return 0;
  int prefix_len = address_prefix_bytes_len(version);
  return base58_decode_check(addr, hasher_base58, out, 20 + prefix_len) ==
             20 + prefix_len &&
         address_check_prefix(out, version);
}

void compress_coords(const curve_point *cp, uint8_t *compressed) {
  compressed[0] = bn_is_odd(&cp->y) ? 0x03 : 0x02;
  bn_write_be(&cp->x, compressed + 1);
}

void uncompress_coords(const ecdsa_curve *curve, uint8_t odd,
                       const bignum256 *x, bignum256 *y) {
  // y^2 = x^3 + a*x + b
  memcpy(y, x, sizeof(bignum256));       // y is x
  bn_multiply(x, y, &curve->prime);      // y is x^2
  bn_subi(y, -curve->a, &curve->prime);  // y is x^2 + a
  bn_multiply(x, y, &curve->prime);      // y is x^3 + ax
  bn_add(y, &curve->b);                  // y is x^3 + ax + b
  bn_sqrt(y, &curve->prime);             // y = sqrt(y)
  if ((odd & 0x01) != (y->val[0] & 1)) {
    bn_subtract(&curve->prime, y, y);  // y = -y
  }
}

int ecdsa_read_pubkey(const ecdsa_curve *curve, const uint8_t *pub_key,
                      curve_point *pub) {
  if (!curve) {
    curve = &secp256k1;
  }
  if (pub_key[0] == 0x04) {
    bn_read_be(pub_key + 1, &(pub->x));
    bn_read_be(pub_key + 33, &(pub->y));
    return ecdsa_validate_pubkey(curve, pub);
  }
  if (pub_key[0] == 0x02 || pub_key[0] == 0x03) {  // compute missing y coords
    bn_read_be(pub_key + 1, &(pub->x));
    uncompress_coords(curve, pub_key[0], &(pub->x), &(pub->y));
    return ecdsa_validate_pubkey(curve, pub);
  }
  // error
  return 0;
}

// Verifies that:
//   - pub is not the point at infinity.
//   - pub->x and pub->y are in range [0,p-1].
//   - pub is on the curve.
// We assume that all curves using this code have cofactor 1, so there is no
// need to verify that pub is a scalar multiple of G.
int ecdsa_validate_pubkey(const ecdsa_curve *curve, const curve_point *pub) {
  bignum256 y_2 = {0}, x3_ax_b = {0};

  if (point_is_infinity(pub)) {
    return 0;
  }

  if (!bn_is_less(&(pub->x), &curve->prime) ||
      !bn_is_less(&(pub->y), &curve->prime)) {
    return 0;
  }

  memcpy(&y_2, &(pub->y), sizeof(bignum256));
  memcpy(&x3_ax_b, &(pub->x), sizeof(bignum256));

  // y^2
  bn_multiply(&(pub->y), &y_2, &curve->prime);
  bn_mod(&y_2, &curve->prime);

  // x^3 + ax + b
  bn_multiply(&(pub->x), &x3_ax_b, &curve->prime);  // x^2
  bn_subi(&x3_ax_b, -curve->a, &curve->prime);      // x^2 + a
  bn_multiply(&(pub->x), &x3_ax_b, &curve->prime);  // x^3 + ax
  bn_addmod(&x3_ax_b, &curve->b, &curve->prime);    // x^3 + ax + b
  bn_mod(&x3_ax_b, &curve->prime);

  if (!bn_is_equal(&x3_ax_b, &y_2)) {
    return 0;
  }

  return 1;
}

// uses secp256k1 curve
// pub_key - 65 bytes uncompressed key
// signature - 64 bytes signature
// msg is a data that was signed
// msg_len is the message length

int ecdsa_verify(const ecdsa_curve *curve, HasherType hasher_sign,
                 const uint8_t *pub_key, const uint8_t *sig, const uint8_t *msg,
                 uint32_t msg_len) {
  uint8_t hash[32] = {0};
  hasher_Raw(hasher_sign, msg, msg_len, hash);
  int res = ecdsa_verify_digest(curve, pub_key, sig, hash);
  memzero(hash, sizeof(hash));
  return res;
}

// Compute public key from signature and recovery id.
// returns 0 if the key is successfully recovered
int ecdsa_recover_pub_from_sig(const ecdsa_curve *curve, uint8_t *pub_key,
                               const uint8_t *sig, const uint8_t *digest,
                               int recid) {
  bignum256 r = {0}, s = {0}, e = {0};
  curve_point cp = {0}, cp2 = {0};

  // read r and s
  bn_read_be(sig, &r);
  bn_read_be(sig + 32, &s);
  if (!bn_is_less(&r, &curve->order) || bn_is_zero(&r)) {
    return 1;
  }
  if (!bn_is_less(&s, &curve->order) || bn_is_zero(&s)) {
    return 1;
  }
  // cp = R = k * G (k is secret nonce when signing)
  memcpy(&cp.x, &r, sizeof(bignum256));
  if (recid & 2) {
    bn_add(&cp.x, &curve->order);
    if (!bn_is_less(&cp.x, &curve->prime)) {
      return 1;
    }
  }
  // compute y from x
  uncompress_coords(curve, recid & 1, &cp.x, &cp.y);
  if (!ecdsa_validate_pubkey(curve, &cp)) {
    return 1;
  }
  // e = -digest
  bn_read_be(digest, &e);
  bn_mod(&e, &curve->order);
  bn_subtract(&curve->order, &e, &e);
  // r = r^-1
  bn_inverse(&r, &curve->order);
  // e = -digest * r^-1
  bn_multiply(&r, &e, &curve->order);
  bn_mod(&e, &curve->order);
  // s = s * r^-1
  bn_multiply(&r, &s, &curve->order);
  bn_mod(&s, &curve->order);
  // cp = s * r^-1 * k * G
  point_multiply(curve, &s, &cp, &cp);
  // cp2 = -digest * r^-1 * G
  scalar_multiply(curve, &e, &cp2);
  // cp = (s * r^-1 * k - digest * r^-1) * G = Pub
  point_add(curve, &cp2, &cp);
  // The point at infinity is not considered to be a valid public key.
  if (point_is_infinity(&cp)) {
    return 1;
  }
  pub_key[0] = 0x04;
  bn_write_be(&cp.x, pub_key + 1);
  bn_write_be(&cp.y, pub_key + 33);
  return 0;
}

// returns 0 if verification succeeded
int ecdsa_verify_digest(const ecdsa_curve *curve, const uint8_t *pub_key,
                        const uint8_t *sig, const uint8_t *digest) {
  curve_point pub = {0}, res = {0};
  bignum256 r = {0}, s = {0}, z = {0};
  int result = 0;

  if (!ecdsa_read_pubkey(curve, pub_key, &pub)) {
    result = 1;
  }

  if (result == 0) {
    bn_read_be(sig, &r);
    bn_read_be(sig + 32, &s);
    bn_read_be(digest, &z);
    if (bn_is_zero(&r) || bn_is_zero(&s) || (!bn_is_less(&r, &curve->order)) ||
        (!bn_is_less(&s, &curve->order))) {
      result = 2;
    }
    if (bn_is_zero(&z)) {
      // The digest was all-zero. The probability of this happening by chance is
      // infinitesimal, but it could be induced by a fault injection. In this
      // case the signature (r,s) can be forged by taking r := (t * Q).x mod n
      // and s := r * t^-1 mod n for any t in [1, n-1]. We fail verification,
      // because there is no guarantee that the signature was created by the
      // owner of the private key.
      result = 3;
    }
  }

  if (result == 0) {
    bn_inverse(&s, &curve->order);       // s = s^-1
    bn_multiply(&s, &z, &curve->order);  // z = z * s  [u1 = z * s^-1 mod n]
    bn_mod(&z, &curve->order);
  }

  if (result == 0) {
    bn_multiply(&r, &s, &curve->order);  // s = r * s  [u2 = r * s^-1 mod n]
    bn_mod(&s, &curve->order);
    scalar_multiply(curve, &z, &res);       // res = z * G    [= u1 * G]
    point_multiply(curve, &s, &pub, &pub);  // pub = s * pub  [= u2 * Q]
    point_add(curve, &pub, &res);  // res = pub + res  [R = u1 * G + u2 * Q]
    if (point_is_infinity(&res)) {
      // R == Infinity
      result = 4;
    }
  }

  if (result == 0) {
    bn_mod(&(res.x), &curve->order);
    if (!bn_is_equal(&res.x, &r)) {
      // R.x != r
      // signature does not match
      result = 5;
    }
  }

  memzero(&pub, sizeof(pub));
  memzero(&res, sizeof(res));
  memzero(&r, sizeof(r));
  memzero(&s, sizeof(s));
  memzero(&z, sizeof(z));

  // all OK
  return result;
}

int ecdsa_sig_to_der(const uint8_t *sig, uint8_t *der) {
  int i = 0;
  uint8_t *p = der, *len = NULL, *len1 = NULL, *len2 = NULL;
  *p = 0x30;
  p++;  // sequence
  *p = 0x00;
  len = p;
  p++;  // len(sequence)

  *p = 0x02;
  p++;  // integer
  *p = 0x00;
  len1 = p;
  p++;  // len(integer)

  // process R
  i = 0;
  while (i < 31 && sig[i] == 0) {
    i++;
  }                      // skip leading zeroes
  if (sig[i] >= 0x80) {  // put zero in output if MSB set
    *p = 0x00;
    p++;
    *len1 = *len1 + 1;
  }
  while (i < 32) {  // copy bytes to output
    *p = sig[i];
    p++;
    *len1 = *len1 + 1;
    i++;
  }

  *p = 0x02;
  p++;  // integer
  *p = 0x00;
  len2 = p;
  p++;  // len(integer)

  // process S
  i = 32;
  while (i < 63 && sig[i] == 0) {
    i++;
  }                      // skip leading zeroes
  if (sig[i] >= 0x80) {  // put zero in output if MSB set
    *p = 0x00;
    p++;
    *len2 = *len2 + 1;
  }
  while (i < 64) {  // copy bytes to output
    *p = sig[i];
    p++;
    *len2 = *len2 + 1;
    i++;
  }

  *len = *len1 + *len2 + 4;
  return *len + 2;
}

// Parse a DER-encoded signature. We don't check whether the encoded integers
// satisfy DER requirements regarding leading zeros.
int ecdsa_sig_from_der(const uint8_t *der, size_t der_len, uint8_t sig[64]) {
  memzero(sig, 64);

  // Check sequence header.
  if (der_len < 2 || der_len > 72 || der[0] != 0x30 || der[1] != der_len - 2) {
    return 1;
  }

  // Read two DER-encoded integers.
  size_t pos = 2;
  for (int i = 0; i < 2; ++i) {
    // Check integer header.
    if (der_len < pos + 2 || der[pos] != 0x02) {
      return 1;
    }

    // Locate the integer.
    size_t int_len = der[pos + 1];
    pos += 2;
    if (pos + int_len > der_len) {
      return 1;
    }

    // Skip a possible leading zero.
    if (int_len != 0 && der[pos] == 0) {
      int_len--;
      pos++;
    }

    // Copy the integer to the output, making sure it fits.
    if (int_len > 32) {
      return 1;
    }
    memcpy(sig + 32 * (i + 1) - int_len, der + pos, int_len);

    // Move on to the next one.
    pos += int_len;
  }

  // Check that there are no trailing elements in the sequence.
  if (pos != der_len) {
    return 1;
  }

  return 0;
}
