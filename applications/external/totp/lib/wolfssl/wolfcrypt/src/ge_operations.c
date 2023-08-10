/* ge_operations.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


 /* Based On Daniel J Bernstein's ed25519 Public Domain ref10 work. */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ED25519
#ifndef ED25519_SMALL /* run when not defined to use small memory math */

#include <wolfssl/wolfcrypt/ge_operations.h>
#include <wolfssl/wolfcrypt/ed25519.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(CURVED25519_X64)
    #define CURVED25519_ASM_64BIT
    #define CURVED25519_ASM
#endif
#if defined(WOLFSSL_ARMASM)
    #if defined(__aarch64__)
        #define CURVED25519_ASM_64BIT
    #else
        #define CURVED25519_ASM_32BIT
    #endif
    #define CURVED25519_ASM
#endif


static void ge_p2_0(ge_p2 *h);
#ifndef CURVED25519_ASM
static void ge_precomp_0(ge_precomp *h);
static void ge_p3_to_p2(ge_p2 *r,const ge_p3 *p);
#endif
static WC_INLINE void ge_p3_to_cached(ge_cached *r,const ge_p3 *p);
static void ge_p1p1_to_p2(ge_p2 *r,const ge_p1p1 *p);
static WC_INLINE void ge_p1p1_to_p3(ge_p3 *r,const ge_p1p1 *p);
static WC_INLINE void ge_p2_dbl(ge_p1p1 *r,const ge_p2 *p);
static void ge_p3_dbl(ge_p1p1 *r,const ge_p3 *p);

static WC_INLINE void ge_madd(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q);
static WC_INLINE void ge_msub(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q);
static WC_INLINE void ge_add(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q);
static WC_INLINE void ge_sub(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q);

/*
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see ge_operations.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -121665/121666.

Representations:
  ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
  ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
  ge_precomp (Duif): (y+x,y-x,2dxy)
*/

#if !defined(HAVE___UINT128_T) || defined(NO_CURVED25519_128BIT)
#define MASK_21     0x1fffff
#define ORDER_0     0x15d3ed
#define ORDER_1     0x18d2e7
#define ORDER_2     0x160498
#define ORDER_3     0xf39ac
#define ORDER_4     0x1dea2f
#define ORDER_5     0xa6f7c

#ifdef CURVED25519_ASM_32BIT
word64 load_3(const unsigned char *in)
{
  word64 result;
  result = (word64) in[0];
  result |= ((word64) in[1]) << 8;
  result |= ((word64) in[2]) << 16;
  return result;
}


word64 load_4(const unsigned char *in)
{
  word64 result;
  result = (word64) in[0];
  result |= ((word64) in[1]) << 8;
  result |= ((word64) in[2]) << 16;
  result |= ((word64) in[3]) << 24;
  return result;
}
#endif

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/
void sc_reduce(byte* s)
{
    sword64 t[24];
    sword64 carry;

    t[ 0] = MASK_21 & (load_3(s +  0) >> 0);
    t[ 1] = MASK_21 & (load_4(s +  2) >> 5);
    t[ 2] = MASK_21 & (load_3(s +  5) >> 2);
    t[ 3] = MASK_21 & (load_4(s +  7) >> 7);
    t[ 4] = MASK_21 & (load_4(s + 10) >> 4);
    t[ 5] = MASK_21 & (load_3(s + 13) >> 1);
    t[ 6] = MASK_21 & (load_4(s + 15) >> 6);
    t[ 7] = MASK_21 & (load_3(s + 18) >> 3);
    t[ 8] = MASK_21 & (load_3(s + 21) >> 0);
    t[ 9] = MASK_21 & (load_4(s + 23) >> 5);
    t[10] = MASK_21 & (load_3(s + 26) >> 2);
    t[11] = MASK_21 & (load_4(s + 28) >> 7);
    t[12] = MASK_21 & (load_4(s + 31) >> 4);
    t[13] = MASK_21 & (load_3(s + 34) >> 1);
    t[14] = MASK_21 & (load_4(s + 36) >> 6);
    t[15] = MASK_21 & (load_3(s + 39) >> 3);
    t[16] = MASK_21 & (load_3(s + 42) >> 0);
    t[17] = MASK_21 & (load_4(s + 44) >> 5);
    t[18] = MASK_21 & (load_3(s + 47) >> 2);
    t[19] = MASK_21 & (load_4(s + 49) >> 7);
    t[20] = MASK_21 & (load_4(s + 52) >> 4);
    t[21] = MASK_21 & (load_3(s + 55) >> 1);
    t[22] = MASK_21 & (load_4(s + 57) >> 6);
    t[23] =           (load_4(s + 60) >> 3);

    t[11] -= t[23] * ORDER_0;
    t[12] -= t[23] * ORDER_1;
    t[13] -= t[23] * ORDER_2;
    t[14] -= t[23] * ORDER_3;
    t[15] -= t[23] * ORDER_4;
    t[16] -= t[23] * ORDER_5;

    t[10] -= t[22] * ORDER_0;
    t[11] -= t[22] * ORDER_1;
    t[12] -= t[22] * ORDER_2;
    t[13] -= t[22] * ORDER_3;
    t[14] -= t[22] * ORDER_4;
    t[15] -= t[22] * ORDER_5;

    t[ 9] -= t[21] * ORDER_0;
    t[10] -= t[21] * ORDER_1;
    t[11] -= t[21] * ORDER_2;
    t[12] -= t[21] * ORDER_3;
    t[13] -= t[21] * ORDER_4;
    t[14] -= t[21] * ORDER_5;

    t[ 8] -= t[20] * ORDER_0;
    t[ 9] -= t[20] * ORDER_1;
    t[10] -= t[20] * ORDER_2;
    t[11] -= t[20] * ORDER_3;
    t[12] -= t[20] * ORDER_4;
    t[13] -= t[20] * ORDER_5;

    t[ 7] -= t[19] * ORDER_0;
    t[ 8] -= t[19] * ORDER_1;
    t[ 9] -= t[19] * ORDER_2;
    t[10] -= t[19] * ORDER_3;
    t[11] -= t[19] * ORDER_4;
    t[12] -= t[19] * ORDER_5;

    t[ 6] -= t[18] * ORDER_0;
    t[ 7] -= t[18] * ORDER_1;
    t[ 8] -= t[18] * ORDER_2;
    t[ 9] -= t[18] * ORDER_3;
    t[10] -= t[18] * ORDER_4;
    t[11] -= t[18] * ORDER_5;

    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;

    t[ 5] -= t[17] * ORDER_0;
    t[ 6] -= t[17] * ORDER_1;
    t[ 7] -= t[17] * ORDER_2;
    t[ 8] -= t[17] * ORDER_3;
    t[ 9] -= t[17] * ORDER_4;
    t[10] -= t[17] * ORDER_5;

    t[ 4] -= t[16] * ORDER_0;
    t[ 5] -= t[16] * ORDER_1;
    t[ 6] -= t[16] * ORDER_2;
    t[ 7] -= t[16] * ORDER_3;
    t[ 8] -= t[16] * ORDER_4;
    t[ 9] -= t[16] * ORDER_5;

    t[ 3] -= t[15] * ORDER_0;
    t[ 4] -= t[15] * ORDER_1;
    t[ 5] -= t[15] * ORDER_2;
    t[ 6] -= t[15] * ORDER_3;
    t[ 7] -= t[15] * ORDER_4;
    t[ 8] -= t[15] * ORDER_5;

    t[ 2] -= t[14] * ORDER_0;
    t[ 3] -= t[14] * ORDER_1;
    t[ 4] -= t[14] * ORDER_2;
    t[ 5] -= t[14] * ORDER_3;
    t[ 6] -= t[14] * ORDER_4;
    t[ 7] -= t[14] * ORDER_5;

    t[ 1] -= t[13] * ORDER_0;
    t[ 2] -= t[13] * ORDER_1;
    t[ 3] -= t[13] * ORDER_2;
    t[ 4] -= t[13] * ORDER_3;
    t[ 5] -= t[13] * ORDER_4;
    t[ 6] -= t[13] * ORDER_5;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;
    t[12]  = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)((t[ 0] >> 16) | (t[ 1] <<  5));
    s[ 3] = (byte)(t[ 1] >>  3);
    s[ 4] = (byte)(t[ 1] >> 11);
    s[ 5] = (byte)((t[ 1] >> 19) | (t[ 2] <<  2));
    s[ 6] = (byte)(t[ 2] >>  6);
    s[ 7] = (byte)((t[ 2] >> 14) | (t[ 3] <<  7));
    s[ 8] = (byte)(t[ 3] >>  1);
    s[ 9] = (byte)(t[ 3] >>  9);
    s[10] = (byte)((t[ 3] >> 17) | (t[ 4] <<  4));
    s[11] = (byte)(t[ 4] >>  4);
    s[12] = (byte)(t[ 4] >> 12);
    s[13] = (byte)((t[ 4] >> 20) | (t[ 5] <<  1));
    s[14] = (byte)(t[ 5] >>  7);
    s[15] = (byte)((t[ 5] >> 15) | (t[ 6] <<  6));
    s[16] = (byte)(t[ 6] >>  2);
    s[17] = (byte)(t[ 6] >> 10);
    s[18] = (byte)((t[ 6] >> 18) | (t[ 7] <<  3));
    s[19] = (byte)(t[ 7] >>  5);
    s[20] = (byte)(t[ 7] >> 13);
    s[21] = (byte)(t[ 8] >>  0);
    s[22] = (byte)(t[ 8] >>  8);
    s[23] = (byte)((t[ 8] >> 16) | (t[ 9] <<  5));
    s[24] = (byte)(t[ 9] >>  3);
    s[25] = (byte)(t[ 9] >> 11);
    s[26] = (byte)((t[ 9] >> 19) | (t[10] <<  2));
    s[27] = (byte)(t[10] >>  6);
    s[28] = (byte)((t[10] >> 14) | (t[11] <<  7));
    s[29] = (byte)(t[11] >>  1);
    s[30] = (byte)(t[11] >>  9);
    s[31] = (byte)(t[11] >> 17);
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/
void sc_muladd(byte* s, const byte* a, const byte* b, const byte* c)
{
    word32 ad[12], bd[12], cd[12];
    sword64 t[24];
    sword64 carry;

    ad[ 0] = MASK_21 & (load_3(a +  0) >> 0);
    ad[ 1] = MASK_21 & (load_4(a +  2) >> 5);
    ad[ 2] = MASK_21 & (load_3(a +  5) >> 2);
    ad[ 3] = MASK_21 & (load_4(a +  7) >> 7);
    ad[ 4] = MASK_21 & (load_4(a + 10) >> 4);
    ad[ 5] = MASK_21 & (load_3(a + 13) >> 1);
    ad[ 6] = MASK_21 & (load_4(a + 15) >> 6);
    ad[ 7] = MASK_21 & (load_3(a + 18) >> 3);
    ad[ 8] = MASK_21 & (load_3(a + 21) >> 0);
    ad[ 9] = MASK_21 & (load_4(a + 23) >> 5);
    ad[10] = MASK_21 & (load_3(a + 26) >> 2);
    ad[11] = (word32)(load_4(a + 28) >> 7);
    bd[ 0] = MASK_21 & (load_3(b +  0) >> 0);
    bd[ 1] = MASK_21 & (load_4(b +  2) >> 5);
    bd[ 2] = MASK_21 & (load_3(b +  5) >> 2);
    bd[ 3] = MASK_21 & (load_4(b +  7) >> 7);
    bd[ 4] = MASK_21 & (load_4(b + 10) >> 4);
    bd[ 5] = MASK_21 & (load_3(b + 13) >> 1);
    bd[ 6] = MASK_21 & (load_4(b + 15) >> 6);
    bd[ 7] = MASK_21 & (load_3(b + 18) >> 3);
    bd[ 8] = MASK_21 & (load_3(b + 21) >> 0);
    bd[ 9] = MASK_21 & (load_4(b + 23) >> 5);
    bd[10] = MASK_21 & (load_3(b + 26) >> 2);
    bd[11] = (word32)(load_4(b + 28) >> 7);
    cd[ 0] = MASK_21 & (load_3(c +  0) >> 0);
    cd[ 1] = MASK_21 & (load_4(c +  2) >> 5);
    cd[ 2] = MASK_21 & (load_3(c +  5) >> 2);
    cd[ 3] = MASK_21 & (load_4(c +  7) >> 7);
    cd[ 4] = MASK_21 & (load_4(c + 10) >> 4);
    cd[ 5] = MASK_21 & (load_3(c + 13) >> 1);
    cd[ 6] = MASK_21 & (load_4(c + 15) >> 6);
    cd[ 7] = MASK_21 & (load_3(c + 18) >> 3);
    cd[ 8] = MASK_21 & (load_3(c + 21) >> 0);
    cd[ 9] = MASK_21 & (load_4(c + 23) >> 5);
    cd[10] = MASK_21 & (load_3(c + 26) >> 2);
    cd[11] = (word32)(load_4(c + 28) >> 7);

    t[ 0] = cd[ 0] + (sword64)ad[ 0] * bd[ 0];
    t[ 1] = cd[ 1] + (sword64)ad[ 0] * bd[ 1] + (sword64)ad[ 1] * bd[ 0];
    t[ 2] = cd[ 2] + (sword64)ad[ 0] * bd[ 2] + (sword64)ad[ 1] * bd[ 1] +
                     (sword64)ad[ 2] * bd[ 0];
    t[ 3] = cd[ 3] + (sword64)ad[ 0] * bd[ 3] + (sword64)ad[ 1] * bd[ 2] +
                     (sword64)ad[ 2] * bd[ 1] + (sword64)ad[ 3] * bd[ 0];
    t[ 4] = cd[ 4] + (sword64)ad[ 0] * bd[ 4] + (sword64)ad[ 1] * bd[ 3] +
                     (sword64)ad[ 2] * bd[ 2] + (sword64)ad[ 3] * bd[ 1] +
                     (sword64)ad[ 4] * bd[ 0];
    t[ 5] = cd[ 5] + (sword64)ad[ 0] * bd[ 5] + (sword64)ad[ 1] * bd[ 4] +
                     (sword64)ad[ 2] * bd[ 3] + (sword64)ad[ 3] * bd[ 2] +
                     (sword64)ad[ 4] * bd[ 1] + (sword64)ad[ 5] * bd[ 0];
    t[ 6] = cd[ 6] + (sword64)ad[ 0] * bd[ 6] + (sword64)ad[ 1] * bd[ 5] +
                     (sword64)ad[ 2] * bd[ 4] + (sword64)ad[ 3] * bd[ 3] +
                     (sword64)ad[ 4] * bd[ 2] + (sword64)ad[ 5] * bd[ 1] +
                     (sword64)ad[ 6] * bd[ 0];
    t[ 7] = cd[ 7] + (sword64)ad[ 0] * bd[ 7] + (sword64)ad[ 1] * bd[ 6] +
                     (sword64)ad[ 2] * bd[ 5] + (sword64)ad[ 3] * bd[ 4] +
                     (sword64)ad[ 4] * bd[ 3] + (sword64)ad[ 5] * bd[ 2] +
                     (sword64)ad[ 6] * bd[ 1] + (sword64)ad[ 7] * bd[ 0];
    t[ 8] = cd[ 8] + (sword64)ad[ 0] * bd[ 8] + (sword64)ad[ 1] * bd[ 7] +
                     (sword64)ad[ 2] * bd[ 6] + (sword64)ad[ 3] * bd[ 5] +
                     (sword64)ad[ 4] * bd[ 4] + (sword64)ad[ 5] * bd[ 3] +
                     (sword64)ad[ 6] * bd[ 2] + (sword64)ad[ 7] * bd[ 1] +
                     (sword64)ad[ 8] * bd[ 0];
    t[ 9] = cd[ 9] + (sword64)ad[ 0] * bd[ 9] + (sword64)ad[ 1] * bd[ 8] +
                     (sword64)ad[ 2] * bd[ 7] + (sword64)ad[ 3] * bd[ 6] +
                     (sword64)ad[ 4] * bd[ 5] + (sword64)ad[ 5] * bd[ 4] +
                     (sword64)ad[ 6] * bd[ 3] + (sword64)ad[ 7] * bd[ 2] +
                     (sword64)ad[ 8] * bd[ 1] + (sword64)ad[ 9] * bd[ 0];
    t[10] = cd[10] + (sword64)ad[ 0] * bd[10] + (sword64)ad[ 1] * bd[ 9] +
                     (sword64)ad[ 2] * bd[ 8] + (sword64)ad[ 3] * bd[ 7] +
                     (sword64)ad[ 4] * bd[ 6] + (sword64)ad[ 5] * bd[ 5] +
                     (sword64)ad[ 6] * bd[ 4] + (sword64)ad[ 7] * bd[ 3] +
                     (sword64)ad[ 8] * bd[ 2] + (sword64)ad[ 9] * bd[ 1] +
                     (sword64)ad[10] * bd[ 0];
    t[11] = cd[11] + (sword64)ad[ 0] * bd[11] + (sword64)ad[ 1] * bd[10] +
                     (sword64)ad[ 2] * bd[ 9] + (sword64)ad[ 3] * bd[ 8] +
                     (sword64)ad[ 4] * bd[ 7] + (sword64)ad[ 5] * bd[ 6] +
                     (sword64)ad[ 6] * bd[ 5] + (sword64)ad[ 7] * bd[ 4] +
                     (sword64)ad[ 8] * bd[ 3] + (sword64)ad[ 9] * bd[ 2] +
                     (sword64)ad[10] * bd[ 1] + (sword64)ad[11] * bd[ 0];
    t[12] =          (sword64)ad[ 1] * bd[11] + (sword64)ad[ 2] * bd[10] +
                     (sword64)ad[ 3] * bd[ 9] + (sword64)ad[ 4] * bd[ 8] +
                     (sword64)ad[ 5] * bd[ 7] + (sword64)ad[ 6] * bd[ 6] +
                     (sword64)ad[ 7] * bd[ 5] + (sword64)ad[ 8] * bd[ 4] +
                     (sword64)ad[ 9] * bd[ 3] + (sword64)ad[10] * bd[ 2] +
                     (sword64)ad[11] * bd[ 1];
    t[13] =          (sword64)ad[ 2] * bd[11] + (sword64)ad[ 3] * bd[10] +
                     (sword64)ad[ 4] * bd[ 9] + (sword64)ad[ 5] * bd[ 8] +
                     (sword64)ad[ 6] * bd[ 7] + (sword64)ad[ 7] * bd[ 6] +
                     (sword64)ad[ 8] * bd[ 5] + (sword64)ad[ 9] * bd[ 4] +
                     (sword64)ad[10] * bd[ 3] + (sword64)ad[11] * bd[ 2];
    t[14] =          (sword64)ad[ 3] * bd[11] + (sword64)ad[ 4] * bd[10] +
                     (sword64)ad[ 5] * bd[ 9] + (sword64)ad[ 6] * bd[ 8] +
                     (sword64)ad[ 7] * bd[ 7] + (sword64)ad[ 8] * bd[ 6] +
                     (sword64)ad[ 9] * bd[ 5] + (sword64)ad[10] * bd[ 4] +
                     (sword64)ad[11] * bd[ 3];
    t[15] =          (sword64)ad[ 4] * bd[11] + (sword64)ad[ 5] * bd[10] +
                     (sword64)ad[ 6] * bd[ 9] + (sword64)ad[ 7] * bd[ 8] +
                     (sword64)ad[ 8] * bd[ 7] + (sword64)ad[ 9] * bd[ 6] +
                     (sword64)ad[10] * bd[ 5] + (sword64)ad[11] * bd[ 4];
    t[16] =          (sword64)ad[ 5] * bd[11] + (sword64)ad[ 6] * bd[10] +
                     (sword64)ad[ 7] * bd[ 9] + (sword64)ad[ 8] * bd[ 8] +
                     (sword64)ad[ 9] * bd[ 7] + (sword64)ad[10] * bd[ 6] +
                     (sword64)ad[11] * bd[ 5];
    t[17] =          (sword64)ad[ 6] * bd[11] + (sword64)ad[ 7] * bd[10] +
                     (sword64)ad[ 8] * bd[ 9] + (sword64)ad[ 9] * bd[ 8] +
                     (sword64)ad[10] * bd[ 7] + (sword64)ad[11] * bd[ 6];
    t[18] =          (sword64)ad[ 7] * bd[11] + (sword64)ad[ 8] * bd[10] +
                     (sword64)ad[ 9] * bd[ 9] + (sword64)ad[10] * bd[ 8] +
                     (sword64)ad[11] * bd[ 7];
    t[19] =          (sword64)ad[ 8] * bd[11] + (sword64)ad[ 9] * bd[10] +
                     (sword64)ad[10] * bd[ 9] + (sword64)ad[11] * bd[ 8];
    t[20] =          (sword64)ad[ 9] * bd[11] + (sword64)ad[10] * bd[10] +
                     (sword64)ad[11] * bd[ 9];
    t[21] =          (sword64)ad[10] * bd[11] + (sword64)ad[11] * bd[10];
    t[22] =          (sword64)ad[11] * bd[11];
    t[23] = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[18] >> 21; t[19] += carry; t[18] &= MASK_21;
    carry = t[20] >> 21; t[21] += carry; t[20] &= MASK_21;
    carry = t[22] >> 21; t[23] += carry; t[22] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;
    carry = t[17] >> 21; t[18] += carry; t[17] &= MASK_21;
    carry = t[19] >> 21; t[20] += carry; t[19] &= MASK_21;
    carry = t[21] >> 21; t[22] += carry; t[21] &= MASK_21;

    t[11] -= t[23] * ORDER_0;
    t[12] -= t[23] * ORDER_1;
    t[13] -= t[23] * ORDER_2;
    t[14] -= t[23] * ORDER_3;
    t[15] -= t[23] * ORDER_4;
    t[16] -= t[23] * ORDER_5;

    t[10] -= t[22] * ORDER_0;
    t[11] -= t[22] * ORDER_1;
    t[12] -= t[22] * ORDER_2;
    t[13] -= t[22] * ORDER_3;
    t[14] -= t[22] * ORDER_4;
    t[15] -= t[22] * ORDER_5;

    t[ 9] -= t[21] * ORDER_0;
    t[10] -= t[21] * ORDER_1;
    t[11] -= t[21] * ORDER_2;
    t[12] -= t[21] * ORDER_3;
    t[13] -= t[21] * ORDER_4;
    t[14] -= t[21] * ORDER_5;

    t[ 8] -= t[20] * ORDER_0;
    t[ 9] -= t[20] * ORDER_1;
    t[10] -= t[20] * ORDER_2;
    t[11] -= t[20] * ORDER_3;
    t[12] -= t[20] * ORDER_4;
    t[13] -= t[20] * ORDER_5;

    t[ 7] -= t[19] * ORDER_0;
    t[ 8] -= t[19] * ORDER_1;
    t[ 9] -= t[19] * ORDER_2;
    t[10] -= t[19] * ORDER_3;
    t[11] -= t[19] * ORDER_4;
    t[12] -= t[19] * ORDER_5;

    t[ 6] -= t[18] * ORDER_0;
    t[ 7] -= t[18] * ORDER_1;
    t[ 8] -= t[18] * ORDER_2;
    t[ 9] -= t[18] * ORDER_3;
    t[10] -= t[18] * ORDER_4;
    t[11] -= t[18] * ORDER_5;

    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;

    t[ 5] -= t[17] * ORDER_0;
    t[ 6] -= t[17] * ORDER_1;
    t[ 7] -= t[17] * ORDER_2;
    t[ 8] -= t[17] * ORDER_3;
    t[ 9] -= t[17] * ORDER_4;
    t[10] -= t[17] * ORDER_5;

    t[ 4] -= t[16] * ORDER_0;
    t[ 5] -= t[16] * ORDER_1;
    t[ 6] -= t[16] * ORDER_2;
    t[ 7] -= t[16] * ORDER_3;
    t[ 8] -= t[16] * ORDER_4;
    t[ 9] -= t[16] * ORDER_5;

    t[ 3] -= t[15] * ORDER_0;
    t[ 4] -= t[15] * ORDER_1;
    t[ 5] -= t[15] * ORDER_2;
    t[ 6] -= t[15] * ORDER_3;
    t[ 7] -= t[15] * ORDER_4;
    t[ 8] -= t[15] * ORDER_5;

    t[ 2] -= t[14] * ORDER_0;
    t[ 3] -= t[14] * ORDER_1;
    t[ 4] -= t[14] * ORDER_2;
    t[ 5] -= t[14] * ORDER_3;
    t[ 6] -= t[14] * ORDER_4;
    t[ 7] -= t[14] * ORDER_5;

    t[ 1] -= t[13] * ORDER_0;
    t[ 2] -= t[13] * ORDER_1;
    t[ 3] -= t[13] * ORDER_2;
    t[ 4] -= t[13] * ORDER_3;
    t[ 5] -= t[13] * ORDER_4;
    t[ 6] -= t[13] * ORDER_5;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;
    t[12]  = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)((t[ 0] >> 16) | (t[ 1] <<  5));
    s[ 3] = (byte)(t[ 1] >>  3);
    s[ 4] = (byte)(t[ 1] >> 11);
    s[ 5] = (byte)((t[ 1] >> 19) | (t[ 2] <<  2));
    s[ 6] = (byte)(t[ 2] >>  6);
    s[ 7] = (byte)((t[ 2] >> 14) | (t[ 3] <<  7));
    s[ 8] = (byte)(t[ 3] >>  1);
    s[ 9] = (byte)(t[ 3] >>  9);
    s[10] = (byte)((t[ 3] >> 17) | (t[ 4] <<  4));
    s[11] = (byte)(t[ 4] >>  4);
    s[12] = (byte)(t[ 4] >> 12);
    s[13] = (byte)((t[ 4] >> 20) | (t[ 5] <<  1));
    s[14] = (byte)(t[ 5] >>  7);
    s[15] = (byte)((t[ 5] >> 15) | (t[ 6] <<  6));
    s[16] = (byte)(t[ 6] >>  2);
    s[17] = (byte)(t[ 6] >> 10);
    s[18] = (byte)((t[ 6] >> 18) | (t[ 7] <<  3));
    s[19] = (byte)(t[ 7] >>  5);
    s[20] = (byte)(t[ 7] >> 13);
    s[21] = (byte)(t[ 8] >>  0);
    s[22] = (byte)(t[ 8] >>  8);
    s[23] = (byte)((t[ 8] >> 16) | (t[ 9] <<  5));
    s[24] = (byte)(t[ 9] >>  3);
    s[25] = (byte)(t[ 9] >> 11);
    s[26] = (byte)((t[ 9] >> 19) | (t[10] <<  2));
    s[27] = (byte)(t[10] >>  6);
    s[28] = (byte)((t[10] >> 14) | (t[11] <<  7));
    s[29] = (byte)(t[11] >>  1);
    s[30] = (byte)(t[11] >>  9);
    s[31] = (byte)(t[11] >> 17);
}
#else
static word64 load_6(const byte* a)
{
    word64 n;
    n = ((word64)a[0] <<  0) |
        ((word64)a[1] <<  8) |
        ((word64)a[2] << 16) |
        ((word64)a[3] << 24) |
        ((word64)a[4] << 32) |
        ((word64)a[5] << 40);
    return n;
}

static word64 load_7(const byte* a)
{
    word64 n;
    n = ((word64)a[0] <<  0) |
        ((word64)a[1] <<  8) |
        ((word64)a[2] << 16) |
        ((word64)a[3] << 24) |
        ((word64)a[4] << 32) |
        ((word64)a[5] << 40) |
        ((word64)a[6] << 48);
    return n;
}

#define MASK_42     0x3ffffffffffl
#define ORDER_0     0x31a5cf5d3edl
#define ORDER_1     0x1e735960498l
#define ORDER_2     0x14def9dea2fl

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/
void sc_reduce(byte* s)
{
    __int128_t t[12];
    __int128_t carry;

    t[ 0] = MASK_42 & (load_6(s +  0) >> 0);
    t[ 1] = MASK_42 & (load_6(s +  5) >> 2);
    t[ 2] = MASK_42 & (load_6(s + 10) >> 4);
    t[ 3] = MASK_42 & (load_6(s + 15) >> 6);
    t[ 4] = MASK_42 & (load_6(s + 21) >> 0);
    t[ 5] = MASK_42 & (load_6(s + 26) >> 2);
    t[ 6] = MASK_42 & (load_6(s + 31) >> 4);
    t[ 7] = MASK_42 & (load_6(s + 36) >> 6);
    t[ 8] = MASK_42 & (load_6(s + 42) >> 0);
    t[ 9] = MASK_42 & (load_6(s + 47) >> 2);
    t[10] = MASK_42 & (load_6(s + 52) >> 4);
    t[11] =           (load_7(s + 57) >> 6);

    t[ 5] -= t[11] * ORDER_0;
    t[ 6] -= t[11] * ORDER_1;
    t[ 7] -= t[11] * ORDER_2;

    t[ 4] -= t[10] * ORDER_0;
    t[ 5] -= t[10] * ORDER_1;
    t[ 6] -= t[10] * ORDER_2;

    t[ 3] -= t[ 9] * ORDER_0;
    t[ 4] -= t[ 9] * ORDER_1;
    t[ 5] -= t[ 9] * ORDER_2;

    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;

    t[ 2] -= t[ 8] * ORDER_0;
    t[ 3] -= t[ 8] * ORDER_1;
    t[ 4] -= t[ 8] * ORDER_2;

    t[ 1] -= t[ 7] * ORDER_0;
    t[ 2] -= t[ 7] * ORDER_1;
    t[ 3] -= t[ 7] * ORDER_2;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;
    t[ 6]  = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)(t[ 0] >> 16);
    s[ 3] = (byte)(t[ 0] >> 24);
    s[ 4] = (byte)(t[ 0] >> 32);
    s[ 5] = (byte)(t[ 0] >> 40) | (byte)(t[ 1] <<  2);
    s[ 6] = (byte)(t[ 1] >>  6);
    s[ 7] = (byte)(t[ 1] >> 14);
    s[ 8] = (byte)(t[ 1] >> 22);
    s[ 9] = (byte)(t[ 1] >> 30);
    s[10] = (byte)(t[ 1] >> 38) | (byte)(t[ 2] <<  4);
    s[11] = (byte)(t[ 2] >>  4);
    s[12] = (byte)(t[ 2] >> 12);
    s[13] = (byte)(t[ 2] >> 20);
    s[14] = (byte)(t[ 2] >> 28);
    s[15] = (byte)(t[ 2] >> 36) | (byte)(t[ 3] <<  6);
    s[16] = (byte)(t[ 3] >>  2);
    s[17] = (byte)(t[ 3] >> 10);
    s[18] = (byte)(t[ 3] >> 18);
    s[19] = (byte)(t[ 3] >> 26);
    s[20] = (byte)(t[ 3] >> 34);
    s[21] = (byte)(t[ 4] >>  0);
    s[22] = (byte)(t[ 4] >>  8);
    s[23] = (byte)(t[ 4] >> 16);
    s[24] = (byte)(t[ 4] >> 24);
    s[25] = (byte)(t[ 4] >> 32);
    s[26] = (byte)(t[ 4] >> 40) | (byte)(t[ 5] <<  2);
    s[27] = (byte)(t[ 5] >>  6);
    s[28] = (byte)(t[ 5] >> 14);
    s[29] = (byte)(t[ 5] >> 22);
    s[30] = (byte)(t[ 5] >> 30);
    s[31] = (byte)(t[ 5] >> 38);
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/
void sc_muladd(byte* s, const byte* a, const byte* b, const byte* c)
{
    word64 ad[6], bd[6], cd[6];
    __int128_t t[12];
    __int128_t carry;

    ad[ 0] = MASK_42 & (load_6(a +  0) >> 0);
    ad[ 1] = MASK_42 & (load_6(a +  5) >> 2);
    ad[ 2] = MASK_42 & (load_6(a + 10) >> 4);
    ad[ 3] = MASK_42 & (load_6(a + 15) >> 6);
    ad[ 4] = MASK_42 & (load_6(a + 21) >> 0);
    ad[ 5] =           (load_6(a + 26) >> 2);
    bd[ 0] = MASK_42 & (load_6(b +  0) >> 0);
    bd[ 1] = MASK_42 & (load_6(b +  5) >> 2);
    bd[ 2] = MASK_42 & (load_6(b + 10) >> 4);
    bd[ 3] = MASK_42 & (load_6(b + 15) >> 6);
    bd[ 4] = MASK_42 & (load_6(b + 21) >> 0);
    bd[ 5] =           (load_6(b + 26) >> 2);
    cd[ 0] = MASK_42 & (load_6(c +  0) >> 0);
    cd[ 1] = MASK_42 & (load_6(c +  5) >> 2);
    cd[ 2] = MASK_42 & (load_6(c + 10) >> 4);
    cd[ 3] = MASK_42 & (load_6(c + 15) >> 6);
    cd[ 4] = MASK_42 & (load_6(c + 21) >> 0);
    cd[ 5] =           (load_6(c + 26) >> 2);

    t[ 0] = cd[ 0] + (__int128_t)ad[ 0] * bd[ 0];
    t[ 1] = cd[ 1] + (__int128_t)ad[ 0] * bd[ 1] + (__int128_t)ad[ 1] * bd[ 0];
    t[ 2] = cd[ 2] + (__int128_t)ad[ 0] * bd[ 2] + (__int128_t)ad[ 1] * bd[ 1] +
                     (__int128_t)ad[ 2] * bd[ 0];
    t[ 3] = cd[ 3] + (__int128_t)ad[ 0] * bd[ 3] + (__int128_t)ad[ 1] * bd[ 2] +
                     (__int128_t)ad[ 2] * bd[ 1] + (__int128_t)ad[ 3] * bd[ 0];
    t[ 4] = cd[ 4] + (__int128_t)ad[ 0] * bd[ 4] + (__int128_t)ad[ 1] * bd[ 3] +
                     (__int128_t)ad[ 2] * bd[ 2] + (__int128_t)ad[ 3] * bd[ 1] +
                     (__int128_t)ad[ 4] * bd[ 0];
    t[ 5] = cd[ 5] + (__int128_t)ad[ 0] * bd[ 5] + (__int128_t)ad[ 1] * bd[ 4] +
                     (__int128_t)ad[ 2] * bd[ 3] + (__int128_t)ad[ 3] * bd[ 2] +
                     (__int128_t)ad[ 4] * bd[ 1] + (__int128_t)ad[ 5] * bd[ 0];
    t[ 6] =          (__int128_t)ad[ 1] * bd[ 5] + (__int128_t)ad[ 2] * bd[ 4] +
                     (__int128_t)ad[ 3] * bd[ 3] + (__int128_t)ad[ 4] * bd[ 2] +
                     (__int128_t)ad[ 5] * bd[ 1];
    t[ 7] =          (__int128_t)ad[ 2] * bd[ 5] + (__int128_t)ad[ 3] * bd[ 4] +
                     (__int128_t)ad[ 4] * bd[ 3] + (__int128_t)ad[ 5] * bd[ 2];
    t[ 8] =          (__int128_t)ad[ 3] * bd[ 5] + (__int128_t)ad[ 4] * bd[ 4] +
                     (__int128_t)ad[ 5] * bd[ 3];
    t[ 9] =          (__int128_t)ad[ 4] * bd[ 5] + (__int128_t)ad[ 5] * bd[ 4];
    t[10] =          (__int128_t)ad[ 5] * bd[ 5];
    t[11] = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;
    carry = t[ 8] >> 42; t[ 9] += carry; t[ 8] &= MASK_42;
    carry = t[10] >> 42; t[11] += carry; t[10] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 9] >> 42; t[10] += carry; t[ 9] &= MASK_42;

    t[ 5] -= t[11] * ORDER_0;
    t[ 6] -= t[11] * ORDER_1;
    t[ 7] -= t[11] * ORDER_2;

    t[ 4] -= t[10] * ORDER_0;
    t[ 5] -= t[10] * ORDER_1;
    t[ 6] -= t[10] * ORDER_2;

    t[ 3] -= t[ 9] * ORDER_0;
    t[ 4] -= t[ 9] * ORDER_1;
    t[ 5] -= t[ 9] * ORDER_2;

    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;

    t[ 2] -= t[ 8] * ORDER_0;
    t[ 3] -= t[ 8] * ORDER_1;
    t[ 4] -= t[ 8] * ORDER_2;

    t[ 1] -= t[ 7] * ORDER_0;
    t[ 2] -= t[ 7] * ORDER_1;
    t[ 3] -= t[ 7] * ORDER_2;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;
    t[ 6]  = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)(t[ 0] >> 16);
    s[ 3] = (byte)(t[ 0] >> 24);
    s[ 4] = (byte)(t[ 0] >> 32);
    s[ 5] = (byte)(t[ 0] >> 40) | (byte)(t[ 1] <<  2);
    s[ 6] = (byte)(t[ 1] >>  6);
    s[ 7] = (byte)(t[ 1] >> 14);
    s[ 8] = (byte)(t[ 1] >> 22);
    s[ 9] = (byte)(t[ 1] >> 30);
    s[10] = (byte)(t[ 1] >> 38) | (byte)(t[ 2] <<  4);
    s[11] = (byte)(t[ 2] >>  4);
    s[12] = (byte)(t[ 2] >> 12);
    s[13] = (byte)(t[ 2] >> 20);
    s[14] = (byte)(t[ 2] >> 28);
    s[15] = (byte)(t[ 2] >> 36) | (byte)(t[ 3] <<  6);
    s[16] = (byte)(t[ 3] >>  2);
    s[17] = (byte)(t[ 3] >> 10);
    s[18] = (byte)(t[ 3] >> 18);
    s[19] = (byte)(t[ 3] >> 26);
    s[20] = (byte)(t[ 3] >> 34);
    s[21] = (byte)(t[ 4] >>  0);
    s[22] = (byte)(t[ 4] >>  8);
    s[23] = (byte)(t[ 4] >> 16);
    s[24] = (byte)(t[ 4] >> 24);
    s[25] = (byte)(t[ 4] >> 32);
    s[26] = (byte)(t[ 4] >> 40) | (byte)(t[ 5] <<  2);
    s[27] = (byte)(t[ 5] >>  6);
    s[28] = (byte)(t[ 5] >> 14);
    s[29] = (byte)(t[ 5] >> 22);
    s[30] = (byte)(t[ 5] >> 30);
    s[31] = (byte)(t[ 5] >> 38);
}
#endif /* !HAVE___UINT128_T || NO_CURVED25519_128BIT */

int ge_compress_key(byte* out, const byte* xIn, const byte* yIn, word32 keySz)
{
    ge     x,y,z;
    ge_p3  g;
    byte   bArray[ED25519_KEY_SIZE];
    word32 i;

    fe_0(x);
    fe_0(y);
    fe_1(z);
    fe_frombytes(x, xIn);
    fe_frombytes(y, yIn);

    fe_copy(g.X, x);
    fe_copy(g.Y, y);
    fe_copy(g.Z, z);

    ge_p3_tobytes(bArray, &g);

    for (i = 0; i < keySz; i++) {
        out[keySz - 1 - i] = bArray[i];
    }

    return 0;
}


/*
r = p + q
*/
static WC_INLINE void ge_add(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
{
#ifndef CURVED25519_ASM
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->YplusX);
    fe_mul(r->Y,r->Y,q->YminusX);
    fe_mul(r->T,q->T2d,p->T);
    fe_mul(r->X,p->Z,q->Z);
    fe_add(t0,r->X,r->X);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_add(r->Z,t0,r->T);
    fe_sub(r->T,t0,r->T);
#else
    fe_ge_add(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->Z, q->T2d,
              q->YplusX, q->YminusX);
#endif
}


#ifndef CURVED25519_ASM
/* ge_scalar mult base */
static unsigned char equal(unsigned char b,unsigned char c)
{
  unsigned char x = b ^ c; /* 0: yes; 1..255: no */
  word32 y = x; /* 0: yes; 1..255: no */
  y -= 1; /* 4294967295: yes; 0..254: no */
  y >>= 31; /* 1: yes; 0: no */
  return (unsigned char)y;
}


static unsigned char negative(signed char b)
{
  return ((unsigned char)b) >> 7;
}


static WC_INLINE void cmov(ge_precomp *t,const ge_precomp *u,unsigned char b,
                        unsigned char n)
{
  b = equal(b,n);
  fe_cmov(t->yplusx,u->yplusx,b);
  fe_cmov(t->yminusx,u->yminusx,b);
  fe_cmov(t->xy2d,u->xy2d,b);
}
#endif

#ifdef CURVED25519_ASM_64BIT
static const ge_precomp base[64][8] = {
{
    {
        { 0x2fbc93c6f58c3b85, -0x306cd2390473f1e7, 0x270b4898643d42c2, 0x07cf9d3a33d4ba65 },
        { -0x62efc6fa28bf6ec2, -0x02c660fa2ebf414d, -0x5a3e7bcb977075f7, 0x44fd2f9298f81267 },
        { -0x2442ea98b49044a7, 0x41e13f00eea2a5ea, -0x322b62e336a83906, 0x4f0ebe1faf16ecca }
    },
    {
        { -0x6ddb18036cc38e29, -0x60b9626985f00a4b, 0x5aa69a65e1d60702, 0x590c063fa87d2e2e },
        { -0x75665a9fbd4b2a58, -0x70d47ef3b19f530a, -0x1f61dc944e91c856, 0x6bb595a669c92555 },
        { 0x6e347eaadad36802, -0x450ca66c7c11b7fb, 0x3bcabe10e6076826, 0x49314f0a165ed1b8 }
    },
    {
        { -0x50da4f57b31168d0, 0x025a8430e8864b8a, -0x3ee4affd60fe98ce, 0x7a164e1b9a80f8f4 },
        { 0x56611fe8a4fcd265, 0x3bd353fde5c1ba7d, -0x7ece0ce5deb42943, 0x2ab91587555bda62 },
        { -0x640dee0b0e98b7cc, -0x47b194e809d2076b, -0x282190f8a48dd5b2, 0x549a04b963bb2a21 }
    },
    {
        { 0x287351b98efc099f, 0x6765c6f47dfd2538, -0x35cb72c204f56d9b, 0x680e910321e58727 },
        { -0x6a01faf5fa97e741, 0x327e89715660faa9, -0x3c171c32f95faf8d, 0x27933f4c7445a49a },
        { -0x40e1ba131aebd950, -0x1cd439c29245f06c, -0x1bd68b2a7307ad40, 0x44f079b1b0e64c18 }
    },
    {
        { -0x5ded43bbf75a44cd, -0x72afb73c38a112fe, -0x22e414f3a54013bc, 0x2945ccf146e206eb },
        { 0x7f9182c3a447d6ba, -0x2affeb2eb4d8d649, -0x1cc30ee3479b5f79, 0x154a7e73eb1b55f3 },
        { -0x37cd5e86182ffc4d, 0x5f729d0a00124d7e, 0x62c1d4a10e6d8ff3, 0x68b8ac5938b27a98 }
    },
    {
        { 0x3a0ceeeb77157131, -0x64d8ea76ff375078, -0x7f9a499725a658ca, 0x51e57bb6a2cc38bd },
        { 0x499806b67b7d8ca4, 0x575be28427d22739, -0x44f7a318dfbaac47, 0x38b64c41ae417884 },
        { -0x7062526e97621c5c, 0x175f2428f8fb9137, 0x050ab5329fcfb988, 0x7865dfa21354c09f }
    },
    {
        { 0x6b1a5cd0944ea3bf, 0x7470353ab39dc0d2, 0x71b2528228542e49, 0x461bea69283c927e },
        { -0x4590d36555cdde4f, 0x6ca021533bba23a7, -0x621589b06de6d3c6, 0x1d6edd5d2e5317e0 },
        { 0x217a8aacab0fda36, -0x5ad739abc2cab638, 0x37d05b8b13ab7568, 0x233cef623a2cbc37 }
    },
    {
        { 0x59b7596604dd3e8f, 0x6cb30377e288702c, -0x4ecc6399a1263cdd, 0x0915e76061bce52f },
        { -0x1d58a2120c6dcb27, -0x69c2897f1e4aa707, 0x2c2741ac6e3c23fb, 0x3a9024a1320e01c3 },
        { -0x208217ca57cb5c82, -0x741e63259767a816, 0x2c1185367167b326, 0x589eb3d9dbefd5c2 }
    },
},
{
    {
        { 0x322d04a52d9021f6, -0x463e60cc8a394064, 0x587a3a4342d20b09, 0x143b1cf8aa64fe61 },
        { 0x7ec851ca553e2df3, -0x58ed7b3459b7874d, -0x194a1be6cd772e19, 0x4cf210ec5a9a8883 },
        { -0x6079838269753555, 0x5f54258e27092729, -0x2f582cb415e7f68b, 0x21b546a3374126e1 }
    },
    {
        { 0x490a7a45d185218f, -0x65eac887b9fb6ccb, 0x0060ea09cc31e1f6, 0x7e041577f86ee965 },
        { -0x56b007a75d777cbd, -0x31f12ba9acec12c4, -0x0aa3c2304a40cb06, 0x0a653ca5c9eab371 },
        { 0x66b2a496ce5b67f3, -0x00ab6d2742a9686a, 0x503cec294a592cd0, 0x566943650813acb2 }
    },
    {
        { 0x5672f9eb1dabb69d, -0x458f4aca5017ac04, 0x47ac0f752796d66d, 0x32a5351794117275 },
        { -0x47e724f3d99df868, 0x5d5c31d9606e354a, 0x0982fa4f00a8cdc7, 0x17e12bcd4653e2d4 },
        { -0x2c59bb59209b7bc9, 0x703b6559880fbfdd, -0x347adabf52c5e55b, 0x0900b3f78e4c6468 }
    },
    {
        { -0x12d7f04137e952cf, 0x52d9595bd8e6efe3, 0x0fe71772f6c623f5, 0x4314030b051e293c },
        { 0x0a851b9f679d651b, -0x1ef7349efcccbd0e, -0x29fe0a801774cf5d, 0x371f3acaed2dd714 },
        { -0x2a9fffa1040f4353, -0x7148f0d12e78f3a2, 0x201f9033d084e6a0, 0x4c3a5ae1ce7b6670 }
    },
    {
        { -0x45078a1b36c25f23, -0x46cd7d588e46d6b3, -0x7f29c0480b393ba0, 0x6de9c73dea66c181 },
        { 0x4138a434dcb8fa95, -0x78f3098293697bf5, -0x21c77a8bd68417d4, 0x7c814db27262a55a },
        { 0x478904d5a04df8f2, -0x050451b54efebd2d, -0x0937539caaa2f668, 0x5aac4a412f90b104 }
    },
    {
        { 0x603a0d0abd7f5134, -0x7f7636cd1e2c51ba, -0x20da6ec67867429d, 0x1c145cd274ba0235 },
        { -0x39b0cd94c536d6f8, 0x5551b282e663e1e0, 0x476b35f54a1a4b83, 0x1b9da3fe189f68c2 },
        { 0x32e8386475f3d743, 0x365b8baf6ae5d9ef, -0x7dadc749c7a497e2, 0x234929c1167d65e1 }
    },
    {
        { 0x48145cc21d099fcf, 0x4535c192cc28d7e5, -0x7f183e1ab7db81ff, 0x4a5f28743b2973ee },
        { -0x67b213545f885218, 0x383f77ad19eb389d, -0x38139481d6ab286c, 0x59c77b3aeb7c3a7a },
        { -0x2c5228dadda3309e, -0x6ee5cc7e4dead3a3, -0x274c6052a4f70783, 0x6f05606b4799fe3b }
    },
    {
        { 0x5b433149f91b6483, -0x524a239aa5d3409e, -0x78057bed9cd7d84d, 0x60895e91ab49f8d8 },
        { -0x6001616de884569e, -0x675118e2f21a351f, 0x3ff4ae942d831044, 0x714de12e58533ac8 },
        { -0x16130d12f30793e8, -0x4b92f9edf8ca202c, -0x43625f67fb469419, 0x73e2e62fd96dc26b }
    },
},
{
    {
        { 0x2eccdd0e632f9c1d, 0x51d0b69676893115, 0x52dfb76ba8637a58, 0x6dd37d49a00eef39 },
        { -0x12a49cabb655aea2, -0x579a3b60f4397dc6, -0x7af3e016a4bd2e3c, 0x30d76d6f03d315b9 },
        { 0x6c4444172106e4c7, -0x04ac297f6d728097, -0x4b8c615b96b2c0da, 0x10c697112e864bb0 }
    },
    {
        { 0x0ca62aa08358c805, 0x6a3d4ae37a204247, 0x7464d3a63b11eddc, 0x03bf9baf550806ef },
        { 0x6493c4277dbe5fde, 0x265d4fad19ad7ea2, 0x0e00dfc846304590, 0x25e61cabed66fe09 },
        { 0x3f13e128cc586604, 0x6f5873ecb459747e, -0x5f49c21233ed970b, 0x566d78634586e22c }
    },
    {
        { -0x5efabd7a39a5d030, 0x6c64112af31667c3, 0x680ae240731aee58, 0x14fba5f34793b22a },
        { 0x1637a49f9cc10834, -0x4371a92a57643baf, 0x1cb5ec0f7f7fd2db, 0x33975bca5ecc35d9 },
        { 0x3cd746166985f7d4, 0x593e5e84c9c80057, 0x2fc3f2b67b61131e, 0x14829cea83fc526c }
    },
    {
        { 0x21e70b2f4e71ecb8, -0x19a92246bf5b881d, -0x409aa93131e2b080, 0x05fc3bc4535d7b7e },
        { -0x00bc847b68226a3e, 0x6c744e30aa4eb5a7, -0x61f3a29ec37a1775, 0x2fd9c71e5f758173 },
        { 0x24b8b3ae52afdedd, 0x3495638ced3b30cf, 0x33a4bc83a9be8195, 0x373767475c651f04 }
    },
    {
        { 0x634095cb14246590, -0x10edebbfe93eaacb, -0x61c7ebf376ef43a0, 0x6bf5905730907c8c },
        { 0x2fba99fd40d1add9, -0x4cf8e990690b2fd9, 0x4363f05215f03bae, 0x1fbea56c3b18f999 },
        { 0x0fa778f1e1415b8a, 0x06409ff7bac3a77e, 0x6f52d7b89aa29a50, 0x02521cf67a635a56 }
    },
    {
        { -0x4eeb98df88d0a11c, -0x17076b4e69f86532, 0x4af8224d00ac824a, 0x001753d9f7cd6cc4 },
        { 0x513fee0b0a9d5294, -0x706718a3f020a59a, -0x2b9e7977401ef832, 0x3fa00a7e71382ced },
        { 0x3c69232d963ddb34, 0x1dde87dab4973858, -0x55282e065f6e0d7b, 0x12b5fe2fa048edb6 }
    },
    {
        { -0x20d483d95290e16e, 0x4b66d323504b8913, -0x73bf623f8ae3743d, 0x6f7e93c20796c7b8 },
        { 0x71f0fbc496fce34d, 0x73b9826badf35bed, -0x2dfb8d9e00d73a9f, 0x749b76f96fb1206f },
        { 0x1f5af604aea6ae05, -0x3edcae0e411b6367, 0x61a808b5eeff6b66, 0x0fcec10f01e02151 }
    },
    {
        { 0x3df2d29dc4244e45, 0x2b020e7493d8de0a, 0x6cc8067e820c214d, 0x413779166feab90a },
        { 0x644d58a649fe1e44, 0x21fcaea231ad777e, 0x02441c5a887fd0d2, 0x4901aa7183c511f3 },
        { 0x08b1b7548c1af8f0, -0x31f08583db9d664c, -0x089f4f06e1f926c7, 0x41bb887b726d1213 }
    },
},
{
    {
        { -0x68267f1f55c6082e, 0x35d0384252c6b51c, 0x7d43f49307cd55aa, 0x56bd36cfb78ac362 },
        { -0x6d987f93a983b628, 0x066d04ccca791e6a, -0x5960a9ba1c33c6b5, 0x5c95b686a0788cd2 },
        { 0x2ac519c10d14a954, -0x150b8b4b6b4a0570, -0x19507c7d560785a6, 0x0dea6db1879be094 }
    },
    {
        { 0x15baeb74d6a8797a, 0x7ef55cf1fac41732, 0x29001f5a3c8b05c5, 0x0ad7cc8752eaccfb },
        { -0x559940ab8cbb1a55, -0x25eda77770e4bcf7, 0x5e87d2b3fd564b2f, 0x5b2c78885483b1dd },
        { 0x52151362793408cf, -0x14f0e8fce669c26c, -0x57cc4d0577c26b9a, 0x093a7fa775003c78 }
    },
    {
        { -0x47169fbb9f56ed7a, 0x7f3fd8047778d3de, 0x67d01e31bf8a5e2d, 0x7b038a06c27b653e },
        { -0x1aef8219c5e92842, -0x5c880023650ccd31, 0x70d5bf18440b677f, 0x6a252b19a4a31403 },
        { -0x6126e62a2c966f0d, 0x5213aebbdb4eb9f2, -0x38f715fab3466ecb, 0x58ded57f72260e56 }
    },
    {
        { -0x2592acd9a4f02b75, -0x769f7dce6c405678, -0x287536cd9e2a81d8, 0x79f2942d3a5c8143 },
        { 0x78e79dade9413d77, -0x0da8062a68d61983, 0x59db910ee37aa7e6, 0x6aa11b5bbb9e039c },
        { -0x6825d0da49377217, 0x251ba7eaacf20169, 0x09b44f87ef4eb4e4, 0x7d90ab1bbc6a7da5 }
    },
    {
        { 0x1a07a3f496b3c397, 0x11ceaa188f4e2532, 0x7d9498d5a7751bf0, 0x19ed161f508dd8a0 },
        { -0x6533597c58fe9402, -0x6fafa0b20d3af493, 0x6b610d5fcce435aa, 0x19a10d446198ff96 },
        { 0x560a2cd687dce6ca, 0x7f3568c48664cf4d, -0x78be16addd7fc5c8, 0x483bdab1595653fc }
    },
    {
        { -0x2930b2f54b257f0a, -0x7db7c1ba07cf8020, 0x05005269ae6f9da4, 0x1c7052909cf7877a },
        { -0x0587f0eb78cb05b7, 0x106f0b70360534e0, 0x2210776fe3e307bd, 0x3286c109dde6a0fe },
        { 0x32ee7de2874e98d4, 0x14c362e9b97e0c60, 0x5781dcde6a60a38a, 0x217dd5eaaa7aa840 }
    },
    {
        { -0x7420e0464173f138, 0x00bae7f8e30a0282, 0x4963991dad6c4f6c, 0x07058a6e5df6f60a },
        { -0x62483b2fdb71e150, -0x1f89681eb28b40ae, 0x1e6a9b173c562354, 0x7fa7c21f795a4965 },
        { -0x1614fd3b24ce0981, -0x12da0276ef4304d5, 0x46c8131f5c5cddb4, 0x33b21c13a0cb9bce }
    },
    {
        { -0x6550464fa11c73a5, -0x4062d2b1f8e5ec39, -0x7111919216ccd6f6, 0x1c3bab17ae109717 },
        { 0x360692f8087d8e31, -0x0b2339c82d8e9c09, 0x25a4e62065ea5963, 0x659bf72e5ac160d9 },
        { 0x1c9ab216c7cab7b0, 0x7d65d37407bbc3cc, 0x52744750504a58d5, 0x09f2606b131a2990 }
    },
},
{
    {
        { 0x7e234c597c6691ae, 0x64889d3d0a85b4c8, -0x251d36f3cab50519, 0x0a871e070c6a9e1d },
        { 0x40e87d44744346be, 0x1d48dad415b52b25, 0x7c3a8a18a13b603e, 0x4eb728c12fcdbdf7 },
        { 0x3301b5994bbc8989, 0x736bae3a5bdd4260, 0x0d61ade219d59e3c, 0x3ee7300f2685d464 }
    },
    {
        { 0x43fa7947841e7518, -0x1a3905a69c63b929, -0x5ef9a1e21cfad48c, 0x7d47c6a2cfb89030 },
        { -0x0a2daa1b61822949, -0x7fe9eea39ef4e154, 0x3c99975d92e187ca, 0x13815762979125c2 },
        { 0x3fdad0148ef0d6e0, -0x62c18b656eab90c4, 0x71ec621026bb8157, 0x148cf58d34c9ec80 }
    },
    {
        { -0x1da8d082651b8a93, 0x56c345bb88f3487f, -0x602ef492969f5773, 0x278febad4eaea1b9 },
        { 0x46a492f67934f027, 0x469984bef6840aa9, 0x5ca1bc2a89611854, 0x3ff2fa1ebd5dbbd4 },
        { -0x4e5597e0736cc69a, -0x73de6b63dfd6f368, 0x39115291219d3c52, 0x4104dd02fe9c677b }
    },
    {
        { -0x7edeb1f924f69548, 0x21a8b6c90ce44f35, 0x6524c12a409e2af5, 0x0165b5a48efca481 },
        { 0x72b2bf5e1124422a, -0x5e05f3cc675cc54b, -0x6b349efe05ad499a, 0x2c863b00afaf53d5 },
        { -0x0e6f5b8b5f7b958a, 0x12eff984cd2f7cc0, 0x695e290658aa2b8f, 0x591b67d9bffec8b8 }
    },
    {
        { -0x66464c8e60e74aa3, -0x1b9a1a055e739be2, 0x61081136c29f05ed, 0x489b4f867030128b },
        { 0x312f0d1c80b49bfa, 0x5979515eabf3ec8a, 0x727033c09ef01c88, 0x3de02ec7ca8f7bcb },
        { -0x2dcdefd2c5146d11, -0x1e9dac4b9ee9579f, 0x3d7eabe7190baa24, 0x49f5fbba496cbebf }
    },
    {
        { 0x155d628c1e9c572e, -0x75b279533a77b8bf, -0x6e5cad09aea89c15, 0x06a1a6c28867515b },
        { 0x30949a108a5bcfd4, -0x23bf228f439b8c15, -0x6d3d6b3ecf83f2e4, 0x5604a86dcbfa6e74 },
        { 0x7288d1d47c1764b6, 0x72541140e0418b51, -0x60fce59fe753092f, 0x20989e89fe2742c6 }
    },
    {
        { 0x1674278b85eaec2e, 0x5621dc077acb2bdf, 0x640a4c1661cbf45a, 0x730b9950f70595d3 },
        { 0x499777fd3a2dcc7f, 0x32857c2ca54fd892, -0x5d86279b2df81c60, 0x0403ed1d0ca67e29 },
        { -0x36b4d2ca78b13aae, -0x3a19373067db9073, -0x0834b905e93fca32, 0x5bd7454308303dcc }
    },
    {
        { -0x7a3b6cdeea1886d6, -0x39b3765d42322237, -0x62e1c257525c289e, 0x5bb7db123067f82c },
        { 0x7f9ad19528b24cc2, 0x7f6b54656335c181, 0x66b8b66e4fc07236, 0x133a78007380ad83 },
        { 0x0961f467c6ca62be, 0x04ec21d6211952ee, 0x182360779bd54770, 0x740dca6d58f0e0d2 }
    },
},
{
    {
        { 0x3906c72aed261ae5, -0x65497026771eff09, -0x0a16fa650cc9fe69, 0x0e53dc78bf2b6d47 },
        { 0x50b70bf5d3f0af0b, 0x4feaf48ae32e71f7, 0x60e84ed3a55bbd34, 0x00ed489b3f50d1ed },
        { -0x46f7d640868e7886, 0x5e4444636d17e631, 0x4d05c52e18276893, 0x27632d9a5a4a4af5 }
    },
    {
        { -0x567d7a2e78150025, -0x5a4b0444272f579c, -0x49a70d80fdd99c09, 0x3bbc2b22d99ce282 },
        { -0x2ee00faeab4d9f32, -0x27923c718d06ad90, 0x601fcd0d267cc138, 0x2b67916429e90ccd },
        { -0x46e836ada7c3f5a8, 0x653ff9b80fe4c6f3, -0x64f258284320c3f4, 0x43a0eeb6ab54d60e }
    },
    {
        { 0x3ac6322357875fe8, -0x262b0b130a043471, -0x72117b6cc7d449e0, 0x50c5eaa14c799fdc },
        { 0x396966a46d4a5487, -0x07ee5e7553d44c46, 0x66e4685b5628b26b, 0x70a477029d929b92 },
        { -0x22f12374290d04c4, 0x54c63aa79cc7b7a0, -0x51f4fcd4d37260e6, 0x6f9ce107602967fb }
    },
    {
        { 0x139693063520e0b5, 0x437fcf7c88ea03fe, -0x082b3bf42c36a644, 0x699154d1f893ded9 },
        { -0x52efab4e321e3dd6, -0x3b5716fdb714cd21, 0x5f3e7b33accdc0ea, 0x72364713fc79963e },
        { 0x315d5c75b4b27526, -0x33347bd2fdc9255b, 0x22f0c8a3345fee8e, 0x73975a617d39dbed }
    },
    {
        { 0x6f37f392f4433e46, 0x0e19b9a11f566b18, 0x220fb78a1fd1d662, 0x362a4258a381c94d },
        { -0x1bfdb2069c8a25f0, 0x78d3251a1830c870, -0x6fd4e6b79a7326e4, 0x7e18b10b29b7438a },
        { -0x6f8e26ecd49414d1, 0x0f26e9ad28418247, -0x1546e13642136da3, 0x4be65bc8f48af2de }
    },
    {
        { 0x1d50fba257c26234, 0x7bd4823adeb0678b, -0x3d4f239159ac750b, 0x5665eec6351da73e },
        { 0x78487feba36e7028, 0x5f3f13001dd8ce34, -0x6cb04ed2b4cf3b77, 0x056c244d397f0a2b },
        { -0x24c11ff6bc404df0, 0x4972018720800ac2, 0x26ab5d6173bd8667, 0x20b209c2ab204938 }
    },
    {
        { 0x1fcca94516bd3289, 0x448d65aa41420428, 0x59c3b7b216a55d62, 0x49992cc64e612cd8 },
        { 0x549e342ac07fb34b, 0x02d8220821373d93, -0x43d9d28f532e0a99, 0x7a92c9fdfbcac784 },
        { 0x65bd1bea70f801de, 0x1befb7c0fe49e28a, -0x579cf9324e4d51b6, 0x3b7ac0cd265c2a09 }
    },
    {
        { -0x0f2ab1b0dd12c659, -0x5d5516e1a9f7eaf6, -0x0bde4d161225178b, 0x31bc531d6b7de992 },
        { -0x7dd411bc73fe4314, 0x530cb525c0fbc73b, 0x48519034c1953fe9, 0x265cc261e09a0f5b },
        { -0x20c2ecb2567f068f, 0x7a4fb8d1221a22a7, 0x3df7d42035aad6d8, 0x2a14edcc6a1a125e }
    },
},
{
    {
        { 0x231a8c570478433c, -0x484ad8f13d7ebc63, -0x245566151c26f861, 0x2c03f5256c2b03d9 },
        { -0x20b711f8ad3031b2, -0x3c00050cf913f749, 0x05710b2ab95459c4, 0x161d25fa963ea38d },
        { 0x790f18757b53a47d, 0x307b0130cf0c5879, 0x31903d77257ef7f9, 0x699468bdbd96bbaf }
    },
    {
        { -0x2722c2199556e6b8, 0x485064c22fc0d2cc, -0x64b7db99cb0215d1, 0x293e1c4e6c4a2e3a },
        { -0x42e0d0b90b250131, 0x7cef0114a47fd6f7, -0x2ce00225b5b84c81, 0x525219a473905785 },
        { 0x376e134b925112e1, 0x703778b5dca15da0, -0x4fba7650b9e3ceef, 0x5b605c447f032823 }
    },
    {
        { 0x3be9fec6f0e7f04c, -0x7995a8618a1cb69e, 0x5542ef161e1de61a, 0x2f12fef4cc5abdd5 },
        { -0x469a7fa6df3b8377, -0x180feff36dc47034, 0x0001256502e2ef77, 0x24a76dcea8aeb3ee },
        { 0x0a4522b2dfc0c740, 0x10d06e7f40c9a407, -0x3930ebbe87300998, 0x5e607b2518a43790 }
    },
    {
        { -0x5fd3bce35a6930ec, -0x1c3bd2bf512c1c00, -0x2dbad97fd1f0d925, 0x201f33139e457068 },
        { 0x58b31d8f6cdf1818, 0x35cfa74fc36258a2, -0x1e4c00b09919e292, 0x5067acab6ccdd5f7 },
        { -0x02ad8094f7fc62af, 0x18b14964017c0006, -0x2addf14fd1da5b58, 0x397cba8862460375 }
    },
    {
        { 0x7815c3fbc81379e7, -0x599e6bdf221ed50f, -0x00563f077a57022b, 0x771b4022c1e1c252 },
        { 0x30c13093f05959b2, -0x1dc55e721656868a, 0x222fd491721d5e26, 0x2339d320766e6c3a },
        { -0x27822679aec5d059, -0x0a53648e062b30f8, -0x2f943ce4e15d7c4d, 0x331a189219971a76 }
    },
    {
        { 0x26512f3a9d7572af, 0x5bcbe28868074a9e, -0x7b123e3eee7f083c, 0x1ac9619ff649a67b },
        { -0x0ae990ba04b07f3a, -0x63c938219e388a31, -0x1c2b17e46fbe26e4, 0x31167c6b83bdfe21 },
        { -0x0dd4c7bdadb4ef98, 0x5068343bee9ce987, -0x03628e7bb59daf38, 0x612436341f08b111 }
    },
    {
        { -0x749cb61ce5d2d9c8, -0x622048ff642c02cb, 0x7f8bf1b8a3a06ba4, 0x1522aa3178d90445 },
        { -0x2662be2478b17673, 0x09fea5f16c07dc20, 0x793d2c67d00f9bbc, 0x46ebe2309e5eff40 },
        { 0x2c382f5369614938, -0x2501bf6548d292f0, -0x1737cc6e49b90dd9, 0x45fe70f50524306c }
    },
    {
        { 0x62f24920c8951491, 0x05f007c83f630ca2, 0x6fbb45d2f5c9d4b8, 0x16619f6db57a2245 },
        { -0x25b78a5969f3f474, 0x5b68d076ef0e2f20, 0x07fb51cf3d0b8fd4, 0x428d1623a0e392d4 },
        { 0x084f4a4401a308fd, -0x57dde63c895a3554, -0x214721b9bc2e4383, 0x1d81592d60bd38c6 }
    },
},
{
    {
        { 0x3a4a369a2f89c8a1, 0x63137a1d7c8de80d, -0x4353ff7587125feb, 0x2cb8b3a5b483b03f },
        { -0x27cc284113d5b3c8, 0x2c9162830acc20ed, -0x16c5b8556d208a7f, 0x702d67a3333c4a81 },
        { 0x36e417cbcb1b90a1, 0x33b3ddaa7f11794e, 0x3f510808885bc607, 0x24141dc0e6a8020d }
    },
    {
        { -0x6e6da233427cea83, 0x3ca1205322cc8094, 0x28e57f183f90d6e4, 0x1a4714cede2e767b },
        { 0x59f73c773fefee9d, -0x4c0e10763e306763, -0x1ca204bd1fd1aba1, 0x5766120b47a1b47c },
        { -0x24df45f047494801, -0x48cd3c4988aee05f, -0x56d4ae3f660fd277, 0x4f3875ad489ca5f1 }
    },
    {
        { 0x79ed13f6ee73eec0, -0x5a39ad9296eef44f, -0x1b76d73c79fc79f4, 0x722a1446fd7059f5 },
        { -0x380389d0b6cd54de, 0x7ac0edf72f4c3c1b, 0x5f6b55aa9aa895e8, 0x3680274dad0a0081 },
        { -0x2f6a6016573077e7, -0x2f566aaf7b8a5664, 0x6eac173320b09cc5, 0x628ecf04331b1095 }
    },
    {
        { -0x64be5307a38b330f, -0x498cce7ef7d9adaf, -0x6636d512ee524eb9, 0x7a47d70d34ecb40f },
        { -0x67434ee7562f2244, -0x11bb61cbf74b7fd5, -0x78f76dd947594efc, 0x685f349a45c7915d },
        { 0x60a0c4cbcc43a4f5, 0x775c66ca3677bea9, -0x5e855e8ad0070a13, 0x11ded9020e01fdc0 }
    },
    {
        { 0x471f95b03bea93b7, 0x0552d7d43313abd3, -0x426c8f1d1e81c085, 0x7b120f1db20e5bec },
        { -0x76f187f6351018fc, -0x78d7d6921cf17394, 0x4c5cd2a392aeb1c9, 0x194263d15771531f },
        { 0x17d2fb3d86502d7a, -0x4a9b27bbaf596cae, 0x7da962c8a60ed75d, 0x00d0f85b318736aa }
    },
    {
        { -0x598ac3e10289de3f, 0x69c0b4a7445671f5, -0x68e0ad8bfa4dc3ef, 0x387bc74851a8c7cd },
        { -0x6874ebd188837b03, -0x0bfd9bb8fa573f9e, -0x59852ae41819ed39, 0x2f7b459698dd6a33 },
        { -0x7e76b4b2b5ad5658, -0x5226c1ed09477cd1, 0x184d8548b61bd638, 0x3f1c62dbd6c9f6cd }
    },
    {
        { 0x3fad3e40148f693d, 0x052656e194eb9a72, 0x2f4dcbfd184f4e2f, 0x406f8db1c482e18b },
        { 0x2e8f1f0091910c1f, -0x5b20b01f400d1ed4, 0x60c6560aee927438, 0x6338283facefc8fa },
        { -0x619cf2d380e6e11c, 0x4fbf8301bc3ff670, 0x787d8e4e7afb73c4, 0x50d83d5be8f58fa5 }
    },
    {
        { -0x3f53306f4b2c4993, -0x58fa621a9e8cd1a0, 0x033d1f7870c6b0ba, 0x584161cd26d946e4 },
        { -0x7a97c6e93ee5e769, 0x2d69a4efe506d008, 0x39af1378f664bd01, 0x65942131361517c6 },
        { -0x440d4e5f8d2d835e, -0x40c6c3a6042138fc, -0x167244311d9d47e2, 0x02eebd0b3029b589 }
    },
},
{
    {
        { -0x789a4960847a3a18, 0x6ff0678bd168bab2, 0x3a70e77c1d330f9b, 0x3a5f6d51b0af8e7c },
        { 0x61368756a60dac5f, 0x17e02f6aebabdc57, 0x7f193f2d4cce0f7d, 0x20234a7789ecdcf0 },
        { 0x76d20db67178b252, 0x071c34f9d51ed160, -0x09d5b5df4c1bee90, 0x7cd682353cffe366 }
    },
    {
        { -0x599a329f97530b0d, 0x42d92d183cd7e3d3, 0x5759389d336025d9, 0x3ef0253b2b2cd8ff },
        { 0x0be1a45bd887fab6, 0x2a846a32ba403b6e, -0x266defed1691a000, 0x2838c8863bdc0943 },
        { -0x2e944f30b5b9afd0, -0x05b694beea3a8855, -0x7d3051750b54be63, 0x21dcb8a606a82812 }
    },
    {
        { -0x6572ff054188ce46, -0x7dfc9f819d61e777, -0x4d33fdc8bc0c2681, 0x5d840dbf6c6f678b },
        { 0x5c6004468c9d9fc8, 0x2540096ed42aa3cb, 0x125b4d4c12ee2f9c, 0x0bc3d08194a31dab },
        { 0x706e380d309fe18b, 0x6eb02da6b9e165c7, 0x57bbba997dae20ab, 0x3a4276232ac196dd }
    },
    {
        { 0x3bf8c172db447ecb, 0x5fcfc41fc6282dbd, -0x7f53003f8a55ea02, 0x0770c9e824e1a9f9 },
        { 0x4b42432c8a7084fa, -0x7675e61c20461abb, -0x4160ffde63a71ba3, 0x1ff177cea16debd1 },
        { -0x309e2665ba4a4a03, -0x79f67b16e4c586dc, -0x18cff6e6cfc1c177, 0x39f264fd41500b1e }
    },
    {
        { -0x2e64b55401f6841f, -0x5b92031e201fe6d7, -0x3c36f76bd3590e01, 0x65c621272c35f14e },
        { -0x5852cbe824181d64, -0x426bc895d463ec64, -0x5f16e4716ca68457, 0x1712d73468889840 },
        { -0x18d4760731ce6c23, 0x4d103356a125c0bb, 0x0419a93d2e1cfe83, 0x22f9800ab19ce272 }
    },
    {
        { 0x42029fdd9a6efdac, -0x46ed3141cb5ab6bf, 0x640f64b987bdf37b, 0x4171a4d38598cab4 },
        { 0x605a368a3e9ef8cb, -0x1c163fdd5aafb8eb, 0x553d48b05f24248f, 0x13f416cd647626e5 },
        { -0x05d8a7556636b374, 0x23006f6fb000b807, -0x042d6e225225ac6e, 0x508214fa574bd1ab }
    },
    {
        { 0x461a15bb53d003d6, -0x4defd777430c369b, 0x27c576756c683a5a, 0x3a7758a4c86cb447 },
        { -0x3dfd96eac12901b5, -0x59a598c6aee2883c, -0x3421d9b9d3eb506c, 0x22f960ec6faba74b },
        { 0x548111f693ae5076, 0x1dae21df1dfd54a6, 0x12248c90f3115e65, 0x5d9fd15f8de7f494 }
    },
    {
        { 0x3f244d2aeed7521e, -0x71c56fd7bcd169eb, -0x1e9b4588d163e92c, 0x3bc187fa47eb98d8 },
        { 0x031408d36d63727f, 0x6a379aefd7c7b533, -0x561e703a33511db5, 0x332f35914f8fbed3 },
        { 0x6d470115ea86c20c, -0x6675483493b92edb, -0x2887cd4ac599fe78, 0x450d81ce906fba03 }
    },
},
{
    {
        { 0x23264d66b2cae0b5, 0x7dbaed33ebca6576, 0x030ebed6f0d24ac8, 0x2a887f78f7635510 },
        { -0x0751b2d527bac6fe, 0x7018058ee8db2d1d, -0x554c66a0382d3ee2, 0x53b16d2324ccca79 },
        { 0x2a23b9e75c012d4f, 0x0c974651cae1f2ea, 0x2fb63273675d70ca, 0x0ba7250b864403f5 }
    },
    {
        { -0x229ca76c79079264, 0x61699176e13a85a4, 0x2e5111954eaa7d57, 0x32c21b57fb60bdfb },
        { -0x44f2e702fd639bdf, -0x43d2ebde76d670fe, -0x7cb8071974daf16a, 0x7b9f2fe8032d71c9 },
        { -0x2787dc32ce61f880, -0x103b303e76888a3b, 0x4854fb129a0ab3f7, 0x12c49d417238c371 }
    },
    {
        { 0x09b3a01783799542, 0x626dd08faad5ee3f, -0x45ff4311148feb61, 0x1421b246a0a444c9 },
        { 0x0950b533ffe83769, 0x21861c1d8e1d6bd1, -0x0fdd27c7ecfd1af0, 0x2509200c6391cab4 },
        { 0x4aa43a8e8c24a7c7, 0x04c1f540d8f05ef5, -0x5245a1f3f4c14624, 0x2ab5504448a49ce3 }
    },
    {
        { -0x23f8539ce3a2c506, 0x58615171f9df8c6c, 0x72a079d89d73e2b0, 0x7301f4ceb4eae15d },
        { 0x2ed227266f0f5dec, -0x67db11bea12af7dc, -0x7f8413836b972beb, 0x7093bae1b521e23f },
        { 0x6409e759d6722c41, -0x598b1e308d408d65, -0x43f5db14c3de1a97, 0x390167d24ebacb23 }
    },
    {
        { -0x2844fab45d0dedf5, -0x1d4631514efa7649, 0x3fe8bac8f3c0edbe, 0x4cbd40767112cb69 },
        { 0x27f58e3bba353f1c, 0x4c47764dbf6a4361, -0x50443b1a91a9d9b0, 0x07db2ee6aae1a45d },
        { 0x0b603cc029c58176, 0x5988e3825cb15d61, 0x2bb61413dcf0ad8d, 0x7b8eec6c74183287 }
    },
    {
        { 0x32fee570fc386b73, -0x2574febe25c57339, -0x68a002f537697ca7, 0x6ee809a1b132a855 },
        { -0x1b35bf87d32d8350, -0x25063cdc04169843, -0x4d642cb5752be162, 0x72810497626ede4d },
        { -0x6bbb44ce030279c6, 0x2fe3690a3e4e48c5, -0x23d637982f7705db, 0x13bd1e38d173292e }
    },
    {
        { 0x223fb5cf1dfac521, 0x325c25316f554450, 0x030b98d7659177ac, 0x1ed018b64f88a4bd },
        { -0x2cd4b327969eb64b, -0x1aa6c8287e275549, 0x0bcb2127ae122b94, 0x41e86fcfb14099b0 },
        { 0x3630dfa1b802a6b0, -0x77f078b8bd52c42b, 0x0af90d6ceec5a4d4, 0x746a247a37cdc5d9 }
    },
    {
        { 0x6eccd85278d941ed, 0x2254ae83d22f7843, -0x3add2fd184403249, 0x681e3351bff0e4e2 },
        { -0x2ace4742d484650a, 0x5005093537fc5b51, 0x232fcf25c593546d, 0x20a365142bb40f49 },
        { -0x749b4a627cfcb0bb, 0x2f8b71f21fa20efb, 0x69249495ba6550e4, 0x539ef98e45d5472b }
    },
},
{
    {
        { -0x2f8b2769e3518bc1, -0x0792e70a11e39c13, -0x68423aa4180b12d7, 0x4cbad279663ab108 },
        { 0x6e7bb6a1a6205275, -0x55b0de28bec3717d, 0x6f56d155e88f5cb2, 0x2de25d4ba6345be1 },
        { -0x7f2e6fdb5f28e033, -0x3ada3df504d77508, -0x4e5c68b4a0c59be7, 0x7d7fbcefe2007233 }
    },
    {
        { -0x3283a23a0c3d6f6c, -0x387e5d65d56efa55, -0x7f39e2c9bde3cfa8, 0x4f9cd196dcd8d4d7 },
        { -0x0510e195d994d7ff, -0x7993973b2a8c60ea, -0x0975d043e4fc89d4, 0x5975435e87b75a8d },
        { 0x199297d86a7b3768, -0x2f2fa7dbe52e859d, -0x45fd6352a3e3f3e9, 0x7ccdd084387a0307 }
    },
    {
        { -0x64f37be7989f336d, -0x3251ff85e54cd567, -0x577213799df425e8, 0x3593ca848190ca44 },
        { -0x2359bdd392d9fbe9, -0x51eac2af6b7dbf43, -0x563f3e4b04973989, 0x428bd0ed61d0cf53 },
        { -0x6dece765a17b6559, -0x2b273cca9a270533, -0x73adaba4ac02442f, 0x27398308da2d63e6 }
    },
    {
        { -0x465ef1b3f58fdbad, 0x0fa25866d57d1bde, -0x0046264a32d82509, 0x572c2945492c33fd },
        { 0x42c38d28435ed413, -0x42af0c9fcd873337, -0x44f854e58625fc11, 0x269597aebe8c3355 },
        { -0x388038ba2932cf42, -0x1b20172c1c455105, -0x5dd377cf55a225f4, 0x7f985498c05bca80 }
    },
    {
        { -0x2ca9eaadf0409c9d, 0x08045a45cf4dfba6, -0x113db04378c05f3e, 0x30f2653cd69b12e7 },
        { 0x3849ce889f0be117, -0x7ffa52e484ab5d78, 0x3da3c39f23fc921c, 0x76c2ec470a31f304 },
        { -0x75f736c7553ef37b, 0x46179b60db276bcb, -0x56df3fe1f1905390, 0x2f1273f1596473da }
    },
    {
        { 0x30488bd755a70bc0, 0x06d6b5a4f1d442e7, -0x152e596143a69e9e, 0x38ac1997edc5f784 },
        { 0x4739fc7c8ae01e11, -0x02ad8b6fb5955461, 0x41d98a8287728f2e, 0x5d9e572ad85b69f2 },
        { 0x0666b517a751b13b, 0x747d06867e9b858c, -0x53533feebab221b7, 0x22dfcd9cbfe9e69c }
    },
    {
        { 0x56ec59b4103be0a1, 0x2ee3baecd259f969, 0x797cb29413f5cd32, 0x0fe9877824cde472 },
        { -0x72242d1f3cf2f327, -0x527199a05344bccd, -0x7094da73cdd569e1, 0x6b2916c05448c1c7 },
        { 0x7edb34d10aba913b, 0x4ea3cd822e6dac0e, 0x66083dff6578f815, 0x4c303f307ff00a17 }
    },
    {
        { 0x29fc03580dd94500, -0x132d855b9044136d, 0x130a155fc2e2a7f8, 0x416b151ab706a1d5 },
        { -0x2cf5c429e84d737b, -0x3a2c8848c688c416, -0x39391873e195a341, 0x0d61b8f78b2ab7c4 },
        { 0x56a8d7efe9c136b0, -0x42f81a32a71bb4e0, -0x5019d025e4a81f55, 0x191a2af74277e8d2 }
    },
},
{
    {
        { 0x09d4b60b2fe09a14, -0x3c7b0f50244e8b82, 0x58e2ea8978b5fd6e, 0x519ef577b5e09b0a },
        { -0x2aaff6a45490b67b, 0x04f4cd5b4fbfaf1a, -0x6271d12ed5f38ac0, 0x2bc24e04b2212286 },
        { 0x1863d7d91124cca9, 0x7ac08145b88a708e, 0x2bcd7309857031f5, 0x62337a6e8ab8fae5 }
    },
    {
        { -0x2e54cdb1e4c5ed8d, 0x18947cf181055340, 0x3b5d9567a98c196e, 0x7fa00425802e1e68 },
        { 0x4bcef17f06ffca16, -0x21f91e2496d51e96, 0x0753702d614f42b0, 0x5f6041b45b9212d0 },
        { 0x7d531574028c2705, -0x7fce829624f28a02, 0x30fface8ef8c8ddd, 0x7e9de97bb6c3e998 }
    },
    {
        { -0x0ffb419d5db2bf23, -0x45f9a66efbad2be1, -0x7e3ba11e9d5bbdcc, 0x4cb829d8a22266ef },
        { 0x1558967b9e6585a3, -0x6836631f6716746e, 0x10af149b6eb3adad, 0x42181fe8f4d38cfa },
        { 0x1dbcaa8407b86681, 0x081f001e8b26753b, 0x3cd7ce6a84048e81, 0x78af11633f25f22c }
    },
    {
        { 0x3241c00e7d65318c, -0x19411a232f179219, 0x118b2dc2fbc08c26, 0x680d04a7fc603dc3 },
        { -0x7be9142bf4af4544, 0x1508722628208bee, -0x5ceb7050463e3c93, 0x0d07daacd32d7d5d },
        { -0x063dbeb596a55c15, -0x255bd3b3fa5970df, 0x7c6c23987f93963e, 0x210e8cd30c3954e3 }
    },
    {
        { 0x2b50f16137fe6c26, -0x1efd4327a91bfb28, 0x12b0f1414c561f6b, 0x51b17bc8d028ec91 },
        { -0x53bdfe0def58e3fa, 0x6a65e0aef3bfb021, -0x43bd3ca3c6c9cd09, 0x56ea8db1865f0742 },
        { -0x000a04b430acaee7, -0x0b67628620eef760, -0x4203159a65c45cdb, 0x18a11f1174d1a6f2 }
    },
    {
        { -0x0429c3252d85a0d4, -0x0ff03b43755ef929, 0x53fb5c1a8e64a430, 0x04eaabe50c1a2e85 },
        { 0x407375ab3f6bba29, -0x613c492766e1b7d2, -0x6637f17d1aa06d17, 0x307c13b6fb0c0ae1 },
        { 0x24751021cb8ab5e7, -0x03dcbbb6a3afef15, 0x5f1e717b4e5610a1, 0x44da5f18c2710cd5 }
    },
    {
        { -0x6ea9019476271534, -0x19486bae1dced95f, -0x428b9c26c6bb14b2, 0x726373f6767203ae },
        { 0x033cc55ff1b82eb5, -0x4ea51c92bee351ae, -0x45bf49e67004532d, 0x768edce1532e861f },
        { -0x1cfa358d14810976, 0x662cf31f70eadb23, 0x18f026fdb4c45b68, 0x513b5384b5d2ecbd }
    },
    {
        { 0x5e2702878af34ceb, -0x6ff4fbf646b92952, 0x6512ebf7dabd8512, 0x61d9b76988258f81 },
        { 0x46d46280c729989e, 0x4b93fbd05368a5dd, 0x63df3f81d1765a89, 0x34cebd64b9a0a223 },
        { -0x593a58ecb64826b5, -0x5c0c2ea7dc146bba, 0x0416fbd277484834, 0x69d45e6f2c70812f }
    },
},
{
    {
        { -0x6019d4bcb0b9f105, -0x212cfc2b59c9f82a, -0x0faddef1485f25dc, 0x237e7dbe00545b93 },
        { -0x31e908b43ac3ebcf, 0x2b9725ce2072edde, -0x47463c904a4dc119, 0x7e2e0e450b5cc908 },
        { 0x013575ed6701b430, 0x231094e69f0bfd10, 0x75320f1583e47f22, 0x71afa699b11155e3 }
    },
    {
        { -0x15bdc3e3b8c4af2a, 0x51e87a1f3b38ef10, -0x647b40a04d36416b, 0x00731fbc78f89a1c },
        { 0x65ce6f9b3953b61d, -0x39a7c615505ebe1a, 0x0f435ffda9f759fe, 0x021142e9c2b1c28e },
        { -0x1bcf38e7b707e780, -0x4069f3dda1313ee7, -0x49251f7c9445ea1d, 0x4c4d6f3347e15808 }
    },
    {
        { 0x2f0cddfc988f1970, 0x6b916227b0b9f51b, 0x6ec7b6c4779176be, 0x38bf9500a88f9fa8 },
        { 0x18f7eccfc17d1fc9, 0x6c75f5a651403c14, -0x24218ed40811f321, 0x193fddaaa7e47a22 },
        { 0x1fd2c93c37e8876f, -0x5d09e1a5e72eb9d4, 0x5080f58239241276, 0x6a6fb99ebf0d4969 }
    },
    {
        { -0x114edd4a491bdc3a, -0x6c628fef0d790072, -0x6f56d57ce230a274, 0x136fda9f42c5eb10 },
        { 0x6a46c1bb560855eb, 0x2416bb38f893f09d, -0x28e2eec8708e533f, 0x75f76914a31896ea },
        { -0x06b3204e5cfa422f, 0x0f364b9d9ff82c08, 0x2a87d8a5c3bb588a, 0x022183510be8dcba }
    },
    {
        { -0x62a58efebccf8581, -0x4f9c21613b825ba1, 0x22bbfe52be927ad3, 0x1387c441fd40426c },
        { 0x4af766385ead2d14, -0x5f71277f3583a7d0, 0x0d13a6e610211e3d, 0x6a071ce17b806c03 },
        { -0x4a2c3c2e78687508, 0x722b5a3d7f0e4413, 0x0d7b4848bb477ca0, 0x3171b26aaf1edc92 }
    },
    {
        { -0x59f248274d75b82f, -0x5940eb29e88f5b0f, -0x2b5e076cac2242a8, 0x6c514a63344243e9 },
        { -0x56d0ce6f68a9b358, -0x008447b3dd8a1ee7, 0x4f55fe37a4875150, 0x221fd4873cf0835a },
        { 0x2322204f3a156341, -0x048c1f1645f5fcd3, -0x031f22b3bef0fcf2, 0x48daa596fb924aaa }
    },
    {
        { 0x14f61d5dc84c9793, -0x66be061c10be7dfa, -0x320a4770cb9d8854, 0x58c837fa0e8a79a9 },
        { 0x6eca8e665ca59cc7, -0x57b8dab4d1c75360, 0x31afc708d21e17ce, 0x676dd6fccad84af7 },
        { 0x0cf9688596fc9058, 0x1ddcbbf37b56a01b, -0x233d1882b6ca2996, 0x1c4f73f2c6a57f0a }
    },
    {
        { -0x4c918f910383cb7c, 0x73dfc9b4c3c1cf61, -0x14e2863687e3381b, 0x70459adb7daf675c },
        { 0x0e7a4fbd305fa0bb, -0x7d62b31fab399c53, -0x0bde3c7cd01cc7b8, 0x795ac80d1bf64c42 },
        { 0x1b91db4991b42bb3, 0x572696234b02dcca, -0x6020611ae0738724, 0x5fe162848ce21fd3 }
    },
},
{
    {
        { 0x315c29c795115389, -0x281f1af879d08b32, 0x0c4a762185927432, 0x72de6c984a25a1e4 },
        { -0x1d86f551b2f883bf, -0x746c7d8f248b965d, 0x6eb632dc8abd16a2, 0x720814ecaa064b72 },
        { -0x51654aac40955cf0, 0x050a50a9806d6e1b, -0x6d448bfc5200aec7, 0x0394d27645be618b }
    },
    {
        { -0x0ac69bda4dcaba5c, 0x15a7a27e98fbb296, -0x5493ad439c90227a, 0x79d995a8419334ee },
        { 0x4d572251857eedf4, -0x1c8db1221e616c3b, -0x758ebdf1f4868fcb, 0x3b3c833687abe743 },
        { -0x32757159ee6a228b, -0x5afb2757e22657d1, 0x540dca81a35879b6, 0x60dd16a379c86a8a }
    },
    {
        { 0x3501d6f8153e47b8, -0x485698abeb5d09f4, 0x112ee8b6455d9523, 0x4e62a3c18112ea8a },
        { 0x35a2c8487381e559, 0x596ffea6d78082cb, -0x34688e14245849ad, 0x5a08b5019b4da685 },
        { -0x372b53fbae95487a, 0x595af3215295b23d, -0x29122dcb24fdcf3f, 0x0929efe8825b41cc }
    },
    {
        { -0x74ce8d4852a99ae3, 0x01581b7a3fabd717, 0x2dc94df6424df6e4, 0x30376e5d2c29284f },
        { 0x5f0601d1cbd0f2d3, 0x736e412f6132bb7f, -0x7c9fbbcddc722179, 0x1e3a5272f5c0753c },
        { -0x2d6e72587ea65a64, 0x6bdc1cd93f0713f3, 0x565f7a934acd6590, 0x53daacec4cb4c128 }
    },
    {
        { -0x667ad43c7ad30250, 0x2cc12e9559d6ed0b, 0x70f9e2bf9b5ac27b, 0x4f3b8c117959ae99 },
        { 0x4ca73bd79cc8a7d6, 0x4d4a738f47e9a9b2, -0x0b340ed6bd0a0200, 0x01a13ff9bdbf0752 },
        { 0x55b6c9c82ff26412, 0x1ac4a8c91fb667a8, -0x2ad840301488740e, 0x303337da7012a3be }
    },
    {
        { -0x6892c334052d022f, -0x34777c68c859bf58, 0x2ff00c1d6734cb25, 0x269ff4dc789c2d2b },
        { -0x6aabdddd73e36284, 0x01fac1371a9b340f, 0x7e8d9177925b48d7, 0x53f8ad5661b3e31b },
        { 0x0c003fbdc08d678d, 0x4d982fa37ead2b17, -0x3f8194324d1a7d0f, 0x296c7291df412a44 }
    },
    {
        { -0x204dcdfa25474a62, 0x465aeaa0c8092250, -0x2ecc3ee7658da2e8, 0x2327370261f117d1 },
        { 0x7903de2b33daf397, -0x2f00f9e63659db4d, -0x75e2dad4aaa4c1e8, 0x2b6d581c52e0b7c0 },
        { 0x3d0543d3623e7986, 0x679414c2c278a354, -0x51bc0f338d9e690a, 0x7836c41f8245eaba }
    },
    {
        { -0x359ae17b7fee6c84, -0x394f3b91910be5d8, -0x48fde458a0c072ae, 0x119dff99ead7b9fd },
        { -0x185dab24b616a57f, 0x5192d5d008b0ad73, 0x4d20e5b1d00afc07, 0x5d55f8012cf25f38 },
        { 0x43eadfcbf4b31d4d, -0x39afc08beeeb776e, -0x0111973af9f2c4e9, 0x329293b3dd4a0ac8 }
    },
},
{
    {
        { 0x2879852d5d7cb208, -0x4721228f97820d19, -0x23f40054de97876f, 0x2b44c043677daa35 },
        { 0x4e59214fe194961a, 0x49be7dc70d71cd4f, -0x6cff302dc4af0dd3, 0x4789d446fc917232 },
        { 0x1a1c87ab074eb78e, -0x05392e7166250b99, 0x3eacbbcd484f9067, 0x60c52eef2bb9a4e4 }
    },
    {
        { 0x702bc5c27cae6d11, 0x44c7699b54a48cab, -0x1043bfa945b6d14e, 0x70d77248d9b6676d },
        { 0x0b5d89bc3bfd8bf1, -0x4f946dc8360caae6, 0x0e4c16b0d53028f5, 0x10bc9c312ccfcaab },
        { -0x557517b4c13d5fa5, -0x6796610b12e87e20, 0x794513e4708e85d1, 0x63755bd3a976f413 }
    },
    {
        { 0x3dc7101897f1acb7, 0x5dda7d5ec165bbd8, 0x508e5b9c0fa1020f, 0x2763751737c52a56 },
        { -0x4aa05fc1d52ef7ad, 0x356f75909ee63569, -0x60060e0241964770, 0x0d8cc1c48bc16f84 },
        { 0x029402d36eb419a9, -0x0f4bb181884b9f5b, -0x30579dcf2bc3b6aa, 0x70c2dd8a7ad166e7 }
    },
    {
        { -0x6e2b6982471281ed, 0x74252f0ad776817a, -0x1bf67d1ff27ada9c, 0x32b8613816a53ce5 },
        { 0x656194509f6fec0e, -0x11d18156b939ae73, -0x68cc3e0c981f64a4, 0x2e0fac6363948495 },
        { 0x79e7f7bee448cd64, 0x6ac83a67087886d0, -0x07602b265f1b24d2, 0x4179215c735a4f41 }
    },
    {
        { -0x1b51cc46d79432cc, -0x48108149aa622924, 0x278b141fb3d38e1f, 0x31fa85662241c286 },
        { -0x738f6b18282312d6, -0x6804753cb82c6390, -0x1ec41fcc56f926fe, 0x700344a30cd99d76 },
        { -0x507d93bdd1c9dd0c, -0x3edfd67867ccafd3, -0x643e481ed4c76edd, 0x24bb2312a9952489 }
    },
    {
        { 0x41f80c2af5f85c6b, 0x687284c304fa6794, -0x76ba20665c45e453, 0x0d1d2af9ffeb5d16 },
        { -0x4e5712e8cd21983d, 0x3cb49418461b4948, -0x7142bcbc8930432e, 0x0fee3e871e188008 },
        { -0x5625755ecd9de121, 0x30b822a159226579, 0x4004197ba79ac193, 0x16acd79718531d76 }
    },
    {
        { -0x36a6393a87784953, -0x6b1e6152a06f0146, 0x16e24e62a342f504, 0x164ed34b18161700 },
        { 0x72df72af2d9b1d3d, 0x63462a36a432245a, 0x3ecea07916b39637, 0x123e0ef6b9302309 },
        { 0x487ed94c192fe69a, 0x61ae2cea3a911513, -0x7884092c465b21d9, 0x78da0fc61073f3eb }
    },
    {
        { -0x5d607f0e97f3c56c, 0x71f77e151ae9e7e6, 0x1100f15848017973, 0x054aa4b316b38ddd },
        { 0x5bf15d28e52bc66a, 0x2c47e31870f01a8e, 0x2419afbc06c28bdd, 0x2d25deeb256b173a },
        { -0x2037b972e6d98348, 0x0b28789c66e54daf, 0x2aeb1d2a666eec17, 0x134610a6ab7da760 }
    },
},
{
    {
        { -0x26ebcf1f23fd73c4, 0x0eb955a85217c771, 0x4b09e1ed2c99a1fa, 0x42881af2bd6a743c },
        { -0x350aa13d83a64dc1, -0x665112c1eab2fb0e, 0x68441d72e14141f4, 0x140345133932a0a2 },
        { 0x7bfec69aab5cad3d, -0x3dc1732cb34d3053, 0x685dd14bfb37d6a2, 0x0ad6d64415677a18 }
    },
    {
        { 0x7914892847927e9f, 0x33dad6ef370aa877, 0x1f8f24fa11122703, 0x5265ac2f2adf9592 },
        { 0x781a439e417becb5, 0x4ac5938cd10e0266, 0x5da385110692ac24, 0x11b065a2ade31233 },
        { 0x405fdd309afcb346, -0x268dc2bbd719c0ac, -0x6b3fe20fa09a5552, 0x43e4dc3ae14c0809 }
    },
    {
        { -0x1590853c523d395d, -0x2f16d709168e836c, -0x1d2c861529ba150b, 0x46dd8785c51ffbbe },
        { -0x43ed380e56c75ae9, 0x473028ab3180b2e1, 0x3f78571efbcd254a, 0x74e534426ff6f90f },
        { 0x709801be375c8898, 0x4b06dab5e3fd8348, 0x75880ced27230714, 0x2b09468fdd2f4c42 }
    },
    {
        { 0x5b97946582ffa02a, -0x25f695ae01570ab7, -0x5f9caec8a0885065, 0x1bcfde61201d1e76 },
        { -0x6838b61148fe346a, -0x7c0bc72b495c963d, 0x62962b8b9a402cd9, 0x6976c7509888df7b },
        { 0x4a4a5490246a59a2, -0x29c1422117802270, -0x26bc8398f2dc8e06, 0x69e87308d30f8ed6 }
    },
    {
        { 0x0f80bf028bc80303, 0x6aae16b37a18cefb, -0x22b815b828d3295d, 0x61943588f4ed39aa },
        { 0x435a8bb15656beb0, -0x07053645b0b2a436, -0x464d873beab73f8b, 0x3eb0ef76e892b622 },
        { -0x2d91a3c16efc607b, -0x3f161882090cc557, -0x176973aa8ff9956d, 0x3c34d1881faaaddd }
    },
    {
        { -0x42a4f470d0001f27, 0x6aa254103ed24fb9, 0x2ac7d7bcb26821c4, 0x605b394b60dca36a },
        { 0x3f9d2b5ea09f9ec0, 0x1dab3b6fb623a890, -0x5f645c158d26d93c, 0x374193513fd8b36d },
        { -0x4b17a91ba562e12e, -0x1017b7899368565e, -0x4efb309be1a11183, 0x2f50b81c88a71c8f }
    },
    {
        { 0x2b552ca0a7da522a, 0x3230b336449b0250, -0x0d3b3a435b466047, 0x7b2c674958074a22 },
        { 0x31723c61fc6811bb, -0x634bafb79dee7ff1, 0x768933d347995753, 0x3491a53502752fcd },
        { -0x2aae9a77c12d7321, 0x12d84fd2d362de39, 0x0a874ad3e3378e4f, 0x000d2b1f7c763e74 }
    },
    {
        { -0x69db8873c16b5755, 0x0ad6f3cee9a78bec, -0x6b75387ef28bc3b1, 0x76627935aaecfccc },
        { 0x3d420811d06d4a67, -0x4103fb7a6f1f001d, -0x078f394842b78422, 0x6e2a7316319afa28 },
        { 0x56a8ac24d6d59a9f, -0x37248ac1cf690ffa, 0x477f41e68f4c5299, 0x588d851cf6c86114 }
    },
},
{
    {
        { -0x32d59a18882e0aeb, 0x548991878faa60f1, -0x4e48c4432543f91b, 0x654878cba97cc9fb },
        { 0x51138ec78df6b0fe, 0x5397da89e575f51b, 0x09207a1d717af1b9, 0x2102fdba2b20d650 },
        { -0x69611bfafaa3195f, 0x36bca7681251ad29, 0x3a1af517aa7da415, 0x0ad725db29ecb2ba }
    },
    {
        { -0x013843f364fa907b, 0x537d5268e7f5ffd7, 0x77afc6624312aefa, 0x4f675f5302399fd9 },
        { -0x23bd984e7cb1dba9, -0x498abb4a8f31e43b, 0x1af07a0bf7d15ed7, 0x4aefcffb71a03650 },
        { -0x3cd2c9c9fbeae8e2, -0x32d410ee7667b7c5, -0x78f591522f6baef0, 0x0bccbb72a2a86561 }
    },
    {
        { 0x186d5e4c50fe1296, -0x1fc6847d01176082, 0x3bc7f6c5507031b0, 0x6678fd69108f37c2 },
        { 0x185e962feab1a9c8, -0x791819ca9aeb8233, -0x4f6d1fce44a4920e, 0x4024f0ab59d6b73e },
        { 0x1586fa31636863c2, 0x07f68c48572d33f2, 0x4f73cc9f789eaefc, 0x2d42e2108ead4701 }
    },
    {
        { 0x21717b0d0f537593, -0x6eb196f4ece1f9b4, 0x1bb687ae752ae09f, 0x420bf3a79b423c6e },
        { -0x680aecea6b202d65, 0x6155985d313f4c6a, -0x145ec0f8f7baaff0, 0x676b2608b8d2d322 },
        { -0x7ec7459ae3a4d4b9, -0x798e4913cee4e480, 0x7bff0cb1bc3135b0, 0x745d2ffa9c0cf1e0 }
    },
    {
        { 0x6036df5721d34e6a, -0x4e2477d866844c30, -0x2c3df63c378a9506, 0x06e15be54c1dc839 },
        { -0x40ada5e1d4363743, -0x15a4d9f7d9b8627f, -0x2aee38f120feaa25, 0x1ae23ceb960cf5d0 },
        { 0x5b725d871932994a, 0x32351cb5ceb1dab0, 0x7dc41549dab7ca05, 0x58ded861278ec1f7 }
    },
    {
        { 0x2dfb5ba8b6c2c9a8, 0x48eeef8ef52c598c, 0x33809107f12d1573, 0x08ba696b531d5bd8 },
        { -0x27e8c86c0d993aa4, -0x3736893a33bab1b7, 0x5ce382f8bc26c3a8, 0x2ff39de85485f6f9 },
        { 0x77ed3eeec3efc57a, 0x04e05517d4ff4811, -0x15c285c00e598e35, 0x120633b4947cfe54 }
    },
    {
        { -0x7d42ceb8b6edeff6, -0x21dc8492819041fa, -0x1ee189e6ee15863a, 0x07433be3cb393bde },
        { 0x0b94987891610042, 0x4ee7b13cecebfae8, 0x70be739594f0a4c0, 0x35d30a99b4d59185 },
        { -0x0086bb3fa316680c, 0x575d3de4b05c51a3, 0x583381fd5a76847c, 0x2d873ede7af6da9f }
    },
    {
        { -0x559dfd1eb1a2067f, -0x5df2a6e8afea1e0b, 0x18a275d3bae21d6c, 0x0543618a01600253 },
        { 0x157a316443373409, -0x054748110b557e27, -0x4f6c01190a59b7fa, 0x2e773654707fa7b6 },
        { 0x0deabdf4974c23c1, -0x5590f5da6231b96d, 0x04202cb8a29aba2c, 0x4b1443362d07960d }
    },
},
{
    {
        { 0x299b1c3f57c5715e, -0x69346d6194979270, 0x3004806447235ab3, 0x2c435c24a44d9fe1 },
        { 0x47b837f753242cec, 0x256dc48cc04212f2, -0x1ddd04041e26d73b, 0x48ea295bad8a2c07 },
        { 0x0607c97c80f8833f, 0x0e851578ca25ec5b, 0x54f7450b161ebb6f, 0x7bcb4792a0def80e }
    },
    {
        { 0x1cecd0a0045224c2, 0x757f1b1b69e53952, 0x775b7a925289f681, 0x1b6cc62016736148 },
        { -0x7b781c2fd438c9a7, 0x4baf8445059979df, -0x2e8368a523529041, 0x57369f0bdefc96b6 },
        { -0x0e5666fe8a9c7968, 0x353dd1beeeaa60d3, -0x7b6b8eccb3645b78, 0x63fa6e6843ade311 }
    },
    {
        { 0x2195becdd24b5eb7, 0x5e41f18cc0cd44f9, -0x20d7f8bbbe356122, 0x07073b98f35b7d67 },
        { -0x2ea3dfac9a683e98, -0x608c8bff672d7877, 0x18aee7f13257ba1f, 0x3418bfda07346f14 },
        { -0x2fc39893b31acf2c, 0x0b64c0473b5df9f4, 0x065cef8b19b3a31e, 0x3084d661533102c9 }
    },
    {
        { -0x6593178989fcde03, 0x7fe2b5109eb63ad8, 0x00e7d4ae8ac80592, 0x73d86b7abb6f723a },
        { -0x1e094861407b9653, 0x15801004e2663135, -0x65b67ccf508be7e5, 0x3ba2504f049b673c },
        { 0x0b52b5606dba5ab6, -0x56ecb0f0444e1255, 0x30a9520d9b04a635, 0x6813b8f37973e5db }
    },
    {
        { -0x0e6b35a90cea81d7, 0x136d35705ef528a5, -0x22b3108874fa6644, 0x7d5472af24f833ed },
        { -0x67ab4fabccbed83f, 0x105d047882fbff25, -0x24b60806bbe790b1, 0x1768e838bed0b900 },
        { -0x2f1078b250cc25b9, 0x00d3be5db6e339f9, 0x3f2a8a2f9c9ceece, 0x5d1aeb792352435a }
    },
    {
        { 0x12c7bfaeb61ba775, -0x47b19de01d9c4003, 0x0b47a5c35c840dcf, 0x7e83be0bccaf8634 },
        { -0x0a61944ce6329c36, 0x670c159221d06839, -0x4f92a9a4deaf354a, 0x20fb199d104f12a3 },
        { 0x61943dee6d99c120, -0x79efe0d1b9f46020, 0x6bb2f1518ee8598d, 0x76b76289fcc475cc }
    },
    {
        { 0x4245f1a1522ec0b3, 0x558785b22a75656d, 0x1d485a2548a1b3c0, 0x60959eccd58fe09f },
        { 0x791b4cc1756286fa, -0x24312ce828b5ea84, 0x7e732421ea72bde6, 0x01fe18491131c8e9 },
        { 0x3ebfeb7ba8ed7a09, 0x49fdc2bbe502789c, 0x44ebce5d3c119428, 0x35e1eb55be947f4a }
    },
    {
        { 0x14fd6dfa726ccc74, 0x3b084cfe2f53b965, -0x0cc51b0aad5d374c, 0x59aab07a0d40166a },
        { -0x242518fe3a8c722d, -0x063909ca4d90e412, 0x61e96a8042f15ef4, 0x3aa1d11faf60a4d8 },
        { 0x77bcec4c925eac25, 0x1848718460137738, 0x5b374337fea9f451, 0x1865e78ec8e6aa46 }
    },
},
{
    {
        { -0x6983ab16e3ad6335, 0x30f6269264c635fb, 0x2747aff478121965, 0x17038418eaf66f5c },
        { -0x333b48384991e086, 0x44157e25f50c2f7e, 0x3ef06dfc713eaf1c, 0x582f446752da63f7 },
        { -0x39ce842cdfcdb31c, -0x57efbd175bb7743c, -0x4de10e74b1a5ec9c, 0x0c2a1c4bcda28dc9 }
    },
    {
        { -0x123b7eb7964296bb, 0x0d6d907dbe1c8d22, -0x39c42ded2aa33a55, 0x5a6a9b30a314dc83 },
        { -0x2db2382f90e0fbb9, -0x4dd961c124783fa7, -0x2ea4fd8d044d2d71, 0x7c558bd1c6f64877 },
        { -0x2f13eadb2c69b9c3, 0x12bb628ac35a24f0, -0x5af3c586e343a05c, 0x0404a5ca0afbafc3 }
    },
    {
        { 0x62bc9e1b2a416fd1, -0x4a3908d71cafa675, 0x04343fd83d5d6967, 0x39527516e7f8ee98 },
        { -0x73e0bff8f558bc2a, -0x33452f34a4d9a118, 0x574b046b668fd2de, 0x46395bfdcadd9633 },
        { 0x117fdb2d1a5d9a9c, -0x6388ba432effa3d6, -0x102b410eab2a9016, 0x76579a29e822d016 }
    },
    {
        { 0x333cb51352b434f2, -0x27cdd7b66c217f1f, -0x4aaed7788af2ca32, 0x02c514bb2a2777c1 },
        { 0x45b68e7e49c02a17, 0x23cd51a2bca9a37f, 0x3ed65f11ec224c1b, 0x43a384dc9e05bdb1 },
        { 0x684bd5da8bf1b645, -0x04742c81094ab4ad, 0x313916d7a9b0d253, 0x1160920961548059 }
    },
    {
        { 0x7a385616369b4dcd, 0x75c02ca7655c3563, 0x7dc21bf9d4f18021, 0x2f637d7491e6e042 },
        { -0x4bb2e996d6253056, -0x25ad60b37beca671, -0x16109c35bac2aaa7, 0x351e125bc5698e0b },
        { -0x2b4b64b9e5098442, -0x29fcfc853754769f, 0x71dee19ff9a699fb, 0x7f182d06e7ce2a9a }
    },
    {
        { 0x09454b728e217522, -0x55a7170b2b7b4728, -0x2ca7dab280b96fc4, 0x44acc043241c5217 },
        { 0x7a7c8e64ab0168ec, -0x34a5b5aaea123abd, 0x095519d347cd0eda, 0x67d4ac8c343e93b0 },
        { 0x1c7d6bbb4f7a5777, -0x74ca012b6e7cec1f, 0x4adca1c6c96b4684, 0x556d1c8312ad71bd }
    },
    {
        { -0x7e0f98a94ee417df, 0x0faff82310a3f3dd, -0x074d2faa9566b9a3, 0x097abe38cc8c7f05 },
        { 0x17ef40e30c8d3982, 0x31f7073e15a3fa34, 0x4f21f3cb0773646e, 0x746c6c6d1d824eff },
        { 0x0c49c9877ea52da4, 0x4c4369559bdc1d43, 0x022c3809f7ccebd2, 0x577e14a34bee84bd }
    },
    {
        { -0x6b01314142b228d5, -0x0b95b025f9f0ddef, 0x124a5977c0c8d1ff, 0x705304b8fb009295 },
        { -0x0f1d97539e58c4f6, -0x0d0505efc86e5a0b, -0x3e1ec17d9492ff17, 0x60fa7ee96fd78f42 },
        { -0x49c2e2cab2d6913a, -0x0c3cfac1a052ce28, 0x670b958cb4bd42ec, 0x21398e0ca16353fd }
    },
},
{
    {
        { -0x793a03e979e48166, -0x095ccfb895d83baf, 0x01667267a1e93597, 0x05ffb9cd6082dfeb },
        { 0x216ab2ca8da7d2ef, 0x366ad9dd99f42827, -0x519b46ffb022c38b, 0x403a395b53909e62 },
        { -0x59e805600ac09ec7, 0x60f2b5e513e66cb6, -0x285741104cbb755c, 0x7a2932856f5ea192 }
    },
    {
        { -0x4763bbb7869c6cfe, 0x4ae4f19350c67f2c, -0x0f4ca25737e5063a, 0x39d0003546871017 },
        { 0x0b39d761b02de888, 0x5f550e7ed2414e1f, -0x59405ba7dd1e56c0, 0x050a2f7dfd447b99 },
        { 0x437c3b33a650db77, 0x6bafe81dbac52bb2, -0x0166bfd2d2482ce8, 0x2b5b7eec372ba6ce }
    },
    {
        { -0x596bbfb29ec5370c, 0x500c3c2bfa97e72c, -0x78befb2de0313df0, 0x1b205fb38604a8ee },
        { -0x4c43b4427c0af111, 0x508f0c998c927866, 0x43e76587c8b7e66e, 0x0f7655a3a47f98d9 },
        { 0x55ecad37d24b133c, 0x441e147d6038c90b, 0x656683a1d62c6fee, 0x0157d5dc87e0ecae }
    },
    {
        { -0x6ad9aaeb28e14adc, -0x19fc277ea20eba6d, 0x147cdf410d4de6b7, 0x5293b1730437c850 },
        { -0x0d5850aefcab3ec3, -0x285f4eba55c8d4a0, 0x2869b96a05a3d470, 0x6528e42d82460173 },
        { 0x23d0e0814bccf226, -0x6d38ba327e69046d, -0x749e8693a6abe1a5, 0x40a44df0c021f978 }
    },
    {
        { -0x793691aeb43a2f6b, -0x0df2bf6703597fb6, 0x27363d89c826ea5d, 0x39ca36565719cacf },
        { -0x25579676b0df1596, -0x15eb5c2eb39df9e8, 0x6001fccb090bf8be, 0x35f4e822947e9cf0 },
        { -0x68af90d0907848a4, -0x39db515ffcb51f90, 0x1ec856e3aad34dd6, 0x055b0be0e440e58f }
    },
    {
        { 0x4d12a04b6ea33da2, 0x57cf4c15e36126dd, -0x6f13698a11bb2699, 0x64ca348d2a985aac },
        { 0x6469a17d89735d12, -0x2490d82a199d460f, -0x60345cd795c6a97f, 0x363b8004d269af25 },
        { -0x66a771e61b3b6ed3, -0x1033c4b1e35a3195, 0x4522ea60fa5b98d5, 0x7064bbab1de4a819 }
    },
    {
        { -0x5d6f3f9ebdabded7, -0x0d1d3d514172a470, -0x30dba724895401e5, 0x02157ade83d626bf },
        { -0x46e61eaea588f9bf, -0x565d1d38b1807fc7, 0x7527250b3df23109, 0x756a7330ac27b78b },
        { 0x3e46972a1b9a038b, 0x2e4ee66a7ee03fb4, -0x7e5db78891244b36, 0x1a944ee88ecd0563 }
    },
    {
        { -0x44bf57a6e7dc9d2a, -0x4660aa8875b2e545, -0x72e74bd88a7aa60a, 0x26c20fe74d26235a },
        { -0x2a56e2eeaefc6c8e, 0x2ed377b799ca26de, -0x5e8dfd5302c99495, 0x0730291bd6901995 },
        { 0x648d1d9fe9cc22f5, 0x66bc561928dd577c, 0x47d3ed21652439d1, 0x49d271acedaf8b49 }
    },
},
{
    {
        { 0x2798aaf9b4b75601, 0x5eac72135c8dad72, -0x2d31559e9e485fdd, 0x1bbfb284e98f7d4e },
        { -0x760afa75c7d4cc0d, 0x5ae2ba0bad48c0b4, -0x706c4afc5ac24c92, 0x5aa3ed9d95a232e6 },
        { 0x656777e9c7d96561, -0x34d4edab8d387fca, 0x65053299d9506eee, 0x4a07e14e5e8957cc }
    },
    {
        { 0x240b58cdc477a49b, -0x02c725219bb80fe9, 0x19928d32a7c86aad, 0x50af7aed84afa081 },
        { 0x4ee412cb980df999, -0x5cea2890c391388f, -0x445a12216da38803, 0x3f0bac391d313402 },
        { 0x6e4fde0115f65be5, 0x29982621216109b2, 0x780205810badd6d9, 0x1921a316baebd006 }
    },
    {
        { -0x28a55265260c3e75, 0x566a0eef60b1c19c, 0x3e9a0bac255c0ed9, 0x7b049deca062c7f5 },
        { -0x76bdd08120478f04, 0x2c296beb4f76b3bd, 0x0738f1d436c24df7, 0x6458df41e273aeb0 },
        { -0x23341c85cabbbb7d, 0x758879330fedbe93, 0x786004c312c5dd87, 0x6093dccbc2950e64 }
    },
    {
        { 0x6bdeeebe6084034b, 0x3199c2b6780fb854, -0x68cc895449d2f96b, 0x6e3180c98b647d90 },
        { 0x1ff39a8585e0706d, 0x36d0a5d8b3e73933, 0x43b9f2e1718f453b, 0x57d1ea084827a97c },
        { -0x118549185ed74f8f, -0x5b3ea6926c577456, -0x084b217d4dde9ed0, 0x363e999ddd97bd18 }
    },
    {
        { 0x2f1848dce24baec6, 0x769b7255babcaf60, -0x6f34c391c31016cf, 0x231f979bc6f9b355 },
        { -0x6957bc3eca11e03c, -0x68914caaf71b3731, -0x4bd097fe4a732cd0, 0x48ee9b78693a052b },
        { 0x5c31de4bcc2af3c6, -0x4fb44fcf01df72e1, -0x48728ff63eb04b9a, 0x079bfa9b08792413 }
    },
    {
        { -0x0c36127f5d2abdbb, 0x0aa08b7877f63952, -0x2892539c2ef7ab8b, 0x1ef4fb159470636b },
        { -0x1c6fc5ae25cff20c, -0x7bc69bdcc256a550, -0x12c30ed2f4ca9b80, 0x038c77f684817194 },
        { -0x7ab1a119a4e98414, 0x59590a4296d0cdc2, 0x72b2df3498102199, 0x575ee92a4a0bff56 }
    },
    {
        { 0x5d46bc450aa4d801, -0x3c50edd85acc4628, 0x389e3b262b8906c2, 0x200a1e7e382f581b },
        { -0x2b3f7f6f75e7d031, 0x30e170c299489dbd, 0x05babd5752f733de, 0x43d4e7112cd3fd00 },
        { 0x518db967eaf93ac5, 0x71bc989b056652c0, -0x01d47a26a98e680b, 0x050eca52651e4e38 }
    },
    {
        { -0x6853c6899f199716, -0x64e64401eac54b69, 0x4cb179b534eca79f, 0x6151c09fa131ae57 },
        { -0x3cbce521bac0f364, -0x160afba1008fc465, -0x03268536127b84c3, 0x4b0ee6c21c58f4c6 },
        { 0x3af55c0dfdf05d96, -0x22d9d11fd54b1186, 0x11b2bb8712171709, 0x1fef24fa800f030b }
    },
},
{
    {
        { -0x006e59956fe99de0, -0x0ddaad51a40e1ff7, 0x7dff85d87f90df7c, 0x4f620ffe0c736fb9 },
        { -0x4b69edc5949399f7, -0x58af017a7f54a6c8, -0x0b8e40c6483d85a1, 0x507903ce77ac193c },
        { 0x62f90d65dfde3e34, -0x30d73a6d4605a053, -0x6637910639e9baf0, 0x25d448044a256c84 }
    },
    {
        { 0x2c7c4415c9022b55, 0x56a0d241812eb1fe, -0x0fd15e362849a1f3, 0x4180512fd5323b26 },
        { -0x4297dcf138164e91, 0x0eb1b9c1c1c5795d, 0x7943c8c495b6b1ff, 0x2f9faf620bbacf5e },
        { -0x5b00c19675b75a25, -0x4595c7f9426abfc5, -0x60831e50b82a49a3, 0x15e087e55939d2fb }
    },
    {
        { -0x776be7910469c0c8, 0x48a00e80dc639bd5, -0x5b17f6d41693e367, 0x5a097d54ca573661 },
        { 0x12207543745c1496, -0x2500c30225c79ef4, -0x1b1868d8d38e3cb1, 0x39c07b1934bdede9 },
        { 0x2d45892b17c9e755, -0x2fcc028d76cf7208, 0x6c2fe9d9525b8bd9, 0x2edbecf1c11cc079 }
    },
    {
        { -0x11f0f0222f785da1, -0x638aceaaa3c1cb12, 0x660c572e8fab3ab5, 0x0854fc44544cd3b2 },
        { 0x1616a4e3c715a0d2, 0x53623cb0f8341d4d, -0x6910acd638176635, 0x3d4e8dbba668baa6 },
        { 0x61eba0c555edad19, 0x24b533fef0a83de6, 0x3b77042883baa5f8, 0x678f82b898a47e8d }
    },
    {
        { 0x1e09d94057775696, -0x112ed9a3c326ae25, -0x056253d4df431e91, 0x0f7f76e0e8d089f4 },
        { -0x4eb6e2f4296ff3ac, 0x3539722c9d132636, 0x4db928920b362bc9, 0x4d7cd1fea68b69df },
        { 0x36d9ebc5d485b00c, -0x5da69b6d1b524c9b, -0x3e9a6b7f3dee6333, 0x45306349186e0d5f }
    },
    {
        { -0x695beb13d4f8db6f, 0x1bb2218127a7b65b, 0x6d2849596e8a4af0, 0x65f3b08ccd27765f },
        { -0x6b222f3e593200e3, 0x55f6f115e84213ae, 0x6c935f85992fcf6a, 0x067ee0f54a37f16f },
        { -0x134d6000e667fe09, -0x62c9e2e05d5f08d1, 0x25f11d2375fd2f49, 0x124cefe80fe10fe2 }
    },
    {
        { 0x1518e85b31b16489, -0x70552348248ef405, 0x39b0bdf4a14ae239, 0x05f4cbea503d20c1 },
        { 0x4c126cf9d18df255, -0x3e2b8e16eb859c4a, 0x2c6d3c73f3c93b5f, 0x6be3a6a2e3ff86a2 },
        { -0x31fbf1613fbeba44, -0x38e00b1df7097cb4, -0x42ab91725477b85d, 0x64666aa0a4d2aba5 }
    },
    {
        { -0x4f3ac408ccc816b4, 0x7cb5697e11e14f15, 0x4b84abac1930c750, 0x28dd4abfe0640468 },
        { 0x6841435a7c06d912, -0x35edc3de44c07cf5, -0x2b4c84d84e341d88, 0x1d753b84c76f5046 },
        { 0x7dc0b64c44cb9f44, 0x18a3e1ace3925dbf, 0x7a3034862d0457c4, 0x4c498bf78a0c892e }
    },
},
{
    {
        { 0x22d2aff530976b86, -0x726f47f93d2db9fc, -0x235e7693b21a451b, 0x28005fe6c8340c17 },
        { 0x37d653fb1aa73196, 0x0f9495303fd76418, -0x52dff4f604c5e84e, 0x544d49292fc8613e },
        { 0x6aefba9f34528688, 0x5c1bff9425107da1, -0x08a444329926b4ca, 0x72e472930f316dfa }
    },
    {
        { 0x07f3f635d32a7627, 0x7aaa4d865f6566f0, 0x3c85e79728d04450, 0x1fee7f000fe06438 },
        { 0x2695208c9781084f, -0x4eafd5f4dcbaf11f, -0x02625159fc1021fe, 0x5a9d2e8c2733a34c },
        { 0x765305da03dbf7e5, -0x5b250db6ebcb3243, 0x7b4ad5cdd24a88ec, 0x00f94051ee040543 }
    },
    {
        { -0x28106c44f85068ad, 0x583ed0cf3db766a7, -0x3196674091f4e13b, 0x47b7ffd25dd40452 },
        { -0x72ca94dc3c2ccf4e, -0x0de374644fb8e4fa, -0x4c93ce9391bd47c4, 0x07d79c7e8beab10d },
        { -0x7804046343f722ee, -0x75f994c51e113d65, 0x0d57242bdb1fc1bf, 0x1c3520a35ea64bb6 }
    },
    {
        { -0x325790bfde943fa7, 0x1fbb231d12bcd87e, -0x4b6a9561e838f670, 0x38750c3b66d12e55 },
        { -0x7f2dac5943345cb6, 0x3e61c3a13838219b, -0x6f3c49fe677d1c6a, 0x1c3d05775d0ee66f },
        { 0x692ef1409422e51a, -0x343f38c3d4a2098f, 0x21014fe7744ce029, 0x0621e2c7d330487c }
    },
    {
        { -0x4851e8694f240f0d, 0x54dfafb9e17ce196, 0x25923071e9aaa3b4, 0x5d8e589ca1002e9d },
        { -0x50679f337da67c73, -0x6f15b73e39606524, 0x6526483765581e30, 0x0007d6097bd3a5bc },
        { -0x3f40e26af7bd56b5, -0x4d2c3c9ca770d1c2, 0x0a961438bb51e2ef, 0x1583d7783c1cbf86 }
    },
    {
        { -0x6ffcb8fb3362d739, 0x1d1b679ef72cc58f, 0x16e12b5fbe5b8726, 0x4958064e83c5580a },
        { -0x13115d10a25d851f, 0x597c3a1455670174, -0x3659d5ed99f6e986, 0x252a5f2e81ed8f70 },
        { 0x0d2894265066e80d, -0x033c087acf837395, 0x1b53da780c1112fd, 0x079c170bd843b388 }
    },
    {
        { -0x322932af3f2a2faa, -0x6508979244fca8c5, 0x3ca6723ff3c3ef48, 0x6768c0d7317b8acc },
        { 0x0506ece464fa6fff, -0x411cbce19dfa1add, 0x3579422451b8ea42, 0x6dec05e34ac9fb00 },
        { -0x6b49da1a0eaa3e4d, 0x417bf3a7997b7b91, -0x3dd342239294da00, 0x51445e14ddcd52f4 }
    },
    {
        { -0x76ceb854d4415bab, -0x73ac5db06df86ed7, 0x4b49f948be30f7a7, 0x12e990086e4fd43d },
        { 0x57502b4b3b144951, -0x71980094bbb4434d, -0x474296d8e99c7a25, 0x13186f31e39295c8 },
        { -0x0ef3694c802044d2, -0x60656ca1ede31507, -0x20eec93bc5a467c1, 0x77b2e3f05d3e99af }
    },
},
{
    {
        { -0x6acd0b7033a32d65, 0x2ba851bea3ce3671, 0x32dacaa051122941, 0x478d99d9350004f2 },
        { -0x02f28a78630ed9a9, -0x17d0106b1ac5f1d7, -0x33cb580fa444b419, 0x0b251172a50c38a2 },
        { 0x1d5ad94890bb02c0, 0x50e208b10ec25115, -0x5d95dd76b10de8fe, 0x4dc923343b524805 }
    },
    {
        { 0x3ad3e3ebf36c4975, -0x28a2da5ac879dedb, -0x178c6bc25fda5aea, 0x6bbc7cb4c411c847 },
        { -0x1c7d73bff07f794a, 0x3f77e6f7979f0dc8, 0x7ef6de304df42cb4, 0x5265797cb6abd784 },
        { 0x3c6f9cd1d4a50d56, -0x49dbbf8839015482, 0x6ff9bf483580972e, 0x00375883b332acfb }
    },
    {
        { -0x3674137a938a3664, -0x1bbe7b3fff1cc30c, 0x0a676b9bba907634, 0x669e2cb571f379d7 },
        { 0x0001b2cd28cb0940, 0x63fb51a06f1c24c9, -0x4a52796e232a35cf, 0x67238dbd8c450660 },
        { -0x34ee948c5b642cf8, 0x025aad6b2392729e, -0x4b86c105c0aa264f, 0x72a1056140678bb9 }
    },
    {
        { 0x0d8d2909e2e505b6, -0x673587543fd6edd0, 0x77ef5569a9b12327, 0x7c77897b81439b47 },
        { -0x5d497ed4e336db63, 0x62866eee21211f58, 0x2cb5c5b85df10ece, 0x03a6b259e263ae00 },
        { -0x0e3e4a1d21cce34b, 0x5a9f5d8e15fca420, -0x605bc70e8426cd4f, 0x2a381bf01c6146e7 }
    },
    {
        { -0x083f41cd4acbe991, 0x27e6ca6419cf70d4, -0x6cb2082856a858a7, 0x5701461dabdec2aa },
        { -0x536467863037ee3f, -0x7482d67ec8a91a99, 0x50da4e607c70edfc, 0x5dbca62f884400b6 },
        { 0x2c6747402c915c25, 0x1bdcd1a80b0d340a, 0x5e5601bd07b43f5f, 0x2555b4e05539a242 }
    },
    {
        { 0x78409b1d87e463d4, -0x52b256a532049c63, -0x13d788c8aada6464, 0x69c806e9c31230ab },
        { 0x6fc09f5266ddd216, -0x231a9f58371c8fb8, -0x139a6c625d209d03, 0x7a869ae7e52ed192 },
        { 0x7b48f57414bb3f22, 0x68c7cee4aedccc88, -0x12d06c9e86127f42, 0x25d70b885f77bc4b }
    },
    {
        { -0x67ba62d644e51b2c, 0x56b9c4c739f954ec, -0x7cd8bc093d64b4c2, 0x21ea8e2798b6878a },
        { 0x4151c3d9762bf4de, 0x083f435f2745d82b, 0x29775a2e0d23ddd5, 0x138e3a6269a5db24 },
        { -0x78410b4b95a58464, -0x2dd662e4a03e2f9e, -0x7dbf67e722cde9b8, 0x5c5abeb1e5a2e03d }
    },
    {
        { 0x02cde6de1306a233, 0x7b5a52a2116f8ec7, -0x1e397e0b3ee9c4a5, 0x241d350660d32643 },
        { 0x14722af4b73c2ddb, -0x43b8f3a0a5faf9f3, 0x00943eac2581b02e, 0x0e434b3b1f499c8f },
        { 0x6be4404d0ebc52c7, -0x51b9dcc44e586e0b, 0x2aec170ed25db42b, 0x1d8dfd966645d694 }
    },
},
{
    {
        { -0x2a679c63ed224f5c, -0x5a2e60cf3fdb7995, -0x2e83d0fca7031ba0, 0x07a195152e095e8a },
        { 0x296fa9c59c2ec4de, -0x43749e40b07b0c35, 0x1c7706d917a8f908, 0x63b795fc7ad3255d },
        { -0x57c970fdc761a038, -0x6fbcc4fd30721bc5, -0x505e02a23abed9bd, 0x3e8fe83d032f0137 }
    },
    {
        { 0x08704c8de8efd13c, -0x203ae571cc1fc8cf, -0x5a62a25aed9f321d, 0x22d60899a6258c86 },
        { 0x2f8b15b90570a294, -0x6b0dbd8f98f7bab7, -0x21e3a51e9e44027c, 0x75ba3b797fac4007 },
        { 0x6239dbc070cdd196, 0x60fe8a8b6c7d8a9a, -0x4c77b84314bfeda0, 0x0904d07b87779e5e }
    },
    {
        { -0x0bcdd299b706bf47, 0x06952f0cbd2d0c39, 0x167697ada081f931, 0x6240aacebaf72a6c },
        { -0x4b31e02b22456e64, -0x30ce24c138b37256, 0x2c63cc63ad86cc51, 0x43e2143fbc1dde07 },
        { -0x07cb8b63a45d6a60, -0x296b83a435c82da6, 0x66f13ba7e7c9316a, 0x56bdaf238db40cac }
    },
    {
        { 0x1310d36cc19d3bb2, 0x062a6bb7622386b9, 0x7c9b8591d7a14f5c, 0x03aa31507e1e5754 },
        { 0x362ab9e3f53533eb, 0x338568d56eb93d40, -0x61f1ebade2a5aa8e, 0x1d24a86d83741318 },
        { -0x0b1389b7002b31e1, -0x1fba150fab5373e4, -0x772dda7de2f6ca84, 0x43b261dc9aeb4859 }
    },
    {
        { 0x19513d8b6c951364, -0x6b018ed9fff40b85, 0x028d10ddd54f9567, 0x02b4d5e242940964 },
        { -0x1aa4e1e677448645, -0x5f612f823e85ca63, -0x4fd3d11d9fc215cd, 0x326055cf5b276bc2 },
        { -0x4b5eaa34d72e720e, -0x1533b9b9e7931af8, -0x3b630b6c937dbc77, 0x27a6c809ae5d3410 }
    },
    {
        { -0x32d3d8f53bc296ac, -0x22b5c1a89599354e, 0x79fa592469d7036c, 0x221503603d8c2599 },
        { -0x74591432e0f24e78, 0x37d3d73a675a5be8, -0x0dd1205cea0aa7a6, 0x2cb67174ff60a17e },
        { 0x59eecdf9390be1d0, -0x56bddfbb8d731c0f, -0x7d76e399856b0f0c, 0x7b1df4b73890f436 }
    },
    {
        { 0x5f2e221807f8f58c, -0x1caaa3602b6bf62c, -0x4d555772e04959d0, 0x68698245d352e03d },
        { -0x1b6d0d1f4c4d5ddc, 0x7c6c9e062b551160, 0x15eb8fe20d7f7b0e, 0x61fcef2658fc5992 },
        { -0x244ea27ad5e7e786, -0x0c1b552c79225329, 0x44bae2810ff6c482, 0x46cf4c473daf01cf }
    },
    {
        { 0x213c6ea7f1498140, 0x7c1e7ef8392b4854, 0x2488c38c5629ceba, 0x1065aae50d8cc5bb },
        { 0x426525ed9ec4e5f9, 0x0e5eda0116903303, 0x72b1a7f2cbe5cadc, 0x29387bcd14eb5f40 },
        { 0x1c2c4525df200d57, 0x5c3b2dd6bfca674a, 0x0a07e7b1e1834030, 0x69a198e64f1ce716 }
    },
},
{
    {
        { 0x7b26e56b9e2d4734, -0x3b38ecd47e39e98b, -0x10a36ada13632181, 0x39c80b16e71743ad },
        { 0x7afcd613efa9d697, 0x0cc45aa41c067959, -0x5a901efb3e05256a, 0x3a73b70472e40365 },
        { 0x0f196e0d1b826c68, -0x08e00f1db69f1c25, 0x6113167023b7436c, 0x0cf0ea5877da7282 }
    },
    {
        { -0x1ccd312bc4596ba6, -0x21f4ec9e177e3fa3, 0x1ad40f095e67ed3b, 0x5da8acdab8c63d5d },
        { 0x196c80a4ddd4ccbd, 0x22e6f55d95f2dd9d, -0x38a1cc38bf2938e5, 0x7bb51279cb3c042f },
        { -0x3b4999b5c58fea61, 0x76194f0f0a904e14, -0x5a9eb3c65bf693ed, 0x6cd0ff50979feced }
    },
    {
        { 0x7fecfabdb04ba18e, -0x2f038403c4224309, -0x5be2b791fa85ece4, 0x641a4391f2223a61 },
        { -0x3f1f981870bbd754, 0x14835ab0a61135e3, -0x0de2eb0cc7f9d6cb, 0x6390a4c8df04849c },
        { -0x3a3946a559f95725, -0x6eb480614f97da0f, 0x2a731f6b44fc9eff, 0x30ddf38562705cfc }
    },
    {
        { 0x33bef2bd68bcd52c, -0x39b6244f96b7d10e, -0x4a4911f3be34e512, 0x5c294d270212a7e5 },
        { 0x4e3dcbdad1bff7f9, -0x36ee717ddf9ba8e9, -0x45333143f0e762aa, 0x1b4822e9d4467668 },
        { -0x54c9f580daa9c87f, 0x2512228a480f7958, -0x38a2fad89eeb4b1d, 0x222d9625d976fe2a }
    },
    {
        { 0x0f94be7e0a344f85, -0x14d05573780dd3c8, -0x631e18a1b11e90f1, 0x43e64e5418a08dea },
        { 0x1c717f85b372ace1, -0x7e6cf196b9c740e8, 0x239cad056bc08b58, 0x0b34271c87f8fff4 },
        { -0x7eaa1dade5ca319d, -0x41eff2b206edfd72, -0x4007f4075a822314, 0x57342dc96d6bc6e4 }
    },
    {
        { -0x0c3c4348e18f840a, 0x351d9b8c7291a762, 0x00502e6edad69a33, 0x522f521f1ec8807f },
        { -0x10110f9a3731a668, -0x40fd6aef4a34155e, -0x739b5ef9df483ba8, 0x35134fb231c24855 },
        { 0x272c1f46f9a3902b, -0x36e45c48669a8434, -0x519eb4cfb075e3f2, 0x7afcaad70b99017b }
    },
    {
        { -0x577ebe13107bd495, 0x55e7b14797abe6c5, -0x738b7068fc87b002, 0x5b50a1f7afcd00b7 },
        { -0x3da212ab5b4741bf, -0x6fd2ec1ee44f1d23, 0x41f43233cde82ab2, 0x1085faa5c3aae7cb },
        { -0x647bf0990ec9eceb, 0x18462242701003e9, 0x65ed45fae4a25080, 0x0a2862393fda7320 }
    },
    {
        { -0x69f18c84913462e9, -0x050db6b72983151f, 0x37e7a9b4d55e1b89, 0x5cb7173cb46c59eb },
        { 0x46ab13c8347cbc9d, 0x3849e8d499c12383, 0x4cea314087d64ac9, 0x1f354134b1a29ee7 },
        { 0x4a89e68b82b7abf0, -0x0be326d864594847, 0x16e6c210e18d876f, 0x7cacdb0f7f1b09c6 }
    },
},
{
    {
        { -0x1efebbcb233a3513, 0x47ed5d963c84fb33, 0x70019576ed86a0e7, 0x25b2697bd267f9e4 },
        { -0x6f9d4d1f26e58744, 0x47c9889cc8509667, -0x620ab599bfaf8f48, 0x7369e6a92493a1bf },
        { -0x6298c004ec67979c, 0x3ca5fbd9415dc7b8, -0x1fb133c420d8c4a2, 0x1420683db54e4cd2 }
    },
    {
        { 0x34eebb6fc1cc5ad0, 0x6a1b0ce99646ac8b, -0x2c4f25b6599421ad, 0x31e83b4161d081c1 },
        { -0x4b8742e1db622e69, 0x620c35005e58c102, -0x04fd2cd0334553a4, 0x60b63bebf508a72d },
        { -0x681738ed61f9d4b1, 0x49e48f4f29320ad8, 0x5bece14b6f18683f, 0x55cf1eb62d550317 }
    },
    {
        { 0x3076b5e37df58c52, -0x28c54622186633ca, -0x427ce31cb6ec11e0, 0x1a56fbaa62ba0133 },
        { 0x5879101065c23d58, -0x7462f792af6b7e64, -0x1dbfd056ed3aa059, 0x669a6564570891d4 },
        { -0x6bc194afa3623614, 0x302557bba77c371a, -0x678c51a9becb89af, 0x13c4836799c58a5c }
    },
    {
        { -0x3b230495a2742f80, -0x21143b13a8e5b7be, -0x2b4d177c471aac9b, 0x50bdc87dc8e5b827 },
        { 0x423a5d465ab3e1b9, -0x03ec3e78380ec09f, 0x19f83664ecb5b9b6, 0x66f80c93a637b607 },
        { 0x606d37836edfe111, 0x32353e15f011abd9, 0x64b03ac325b73b96, 0x1dd56444725fd5ae }
    },
    {
        { -0x3d6819fff7453766, 0x7d4cea11eae1c3e0, -0x0c1c741e60186884, 0x3a3a450f63a305cd },
        { -0x705b8007cc9ded83, -0x4360953b8e3283eb, 0x6e71454349220c8b, 0x0e645912219f732e },
        { 0x078f2f31d8394627, 0x389d3183de94a510, -0x2e1c9392e8669080, 0x318c8d9393a9a87b }
    },
    {
        { 0x5d669e29ab1dd398, -0x036de9a7cbd261c5, 0x55851dfdf35973cd, 0x509a41c325950af6 },
        { -0x0d8ba2fcd50001e7, 0x0c9f3c497f24db66, -0x43672c1c457a6711, 0x224c7c679a1d5314 },
        { -0x423f91235906da17, 0x793ef3f4641b1f33, -0x7d13ed7f627cc177, 0x05bff02328a11389 }
    },
    {
        { 0x6881a0dd0dc512e4, 0x4fe70dc844a5fafe, 0x1f748e6b8f4a5240, 0x576277cdee01a3ea },
        { 0x3632137023cae00b, 0x544acf0ad1accf59, -0x698befb62de5e378, 0x780b8cc3fa2a44a7 },
        { 0x1ef38abc234f305f, -0x65a88042ebfa21f8, 0x5e82a51434e62a0d, 0x5ff418726271b7a1 }
    },
    {
        { -0x1a24b817ec496ac0, -0x0ca2d5c4bcd9ef1f, -0x53e0d916c787ed8a, 0x29d4db8ca0a0cb69 },
        { 0x398e080c1789db9d, -0x589fdfda0c18870b, -0x056776b3f942fca3, 0x106a03dc25a966be },
        { -0x2652f550ccccac30, 0x38669da5acd309e5, 0x3c57658ac888f7f0, 0x4ab38a51052cbefa }
    },
},
{
    {
        { -0x09701d177f621fac, -0x1c43f695637d452f, 0x076353d40aadbf45, 0x7b9b1fb5dea1959e },
        { -0x20253411bcdb3f17, 0x054442883f955bb7, -0x2108555715ce9f61, 0x68aee70642287cff },
        { -0x0fe3370e8b8e33f4, -0x6adbd1c8a86f7d45, 0x27776093d3e46b5f, 0x2d13d55a28bd85fb }
    },
    {
        { -0x40fe6331851185ae, -0x57212d491bab152d, 0x3c619f0b87a8bb19, 0x3619b5d7560916d8 },
        { -0x053a2df9a4ca4726, -0x572575657a9db449, -0x332d356ec2de32f1, 0x6b8341ee8bf90d58 },
        { 0x3579f26b0282c4b2, 0x64d592f24fafefae, -0x48321284d7373840, 0x6a927b6b7173a8d7 }
    },
    {
        { -0x728fbf79c1317715, -0x0f1cf8567f113f74, -0x53ddaf9ef2877026, 0x056d92a43a0d478d },
        { 0x1f6db24f986e4656, 0x1021c02ed1e9105b, -0x0700c000d33f5c8b, 0x1d2a6bf8c6c82592 },
        { 0x1b05a196fc3da5a1, 0x77d7a8c243b59ed0, 0x06da3d6297d17918, 0x66fbb494f12353f7 }
    },
    {
        { -0x2928f6690edcf62a, -0x2404dc7a163c2ac7, 0x46d602b0f7552411, 0x270a0b0557843e0c },
        { 0x751a50b9d85c0fb8, -0x2e5023da7430f685, 0x2f16a6a38309a969, 0x14ddff9ee5b00659 },
        { 0x61ff0640a7862bcc, -0x7e353f65a0ee5402, -0x6fb87cfbaa2ed545, 0x19a4bde1945ae873 }
    },
    {
        { 0x40c709dec076c49f, 0x657bfaf27f3e53f6, 0x40662331eca042c4, 0x14b375487eb4df04 },
        { -0x6460d90adf59dff6, 0x64804443cf13eaf8, -0x759c98c079ce122d, 0x72bbbce11ed39dc1 },
        { -0x517ac36b549923b9, -0x149dcbc12089d292, -0x0f71f1e7904d082f, 0x4f0b1c02700ab37a }
    },
    {
        { 0x79fd21ccc1b2e23f, 0x4ae7c281453df52a, -0x37e8d1362eaeb795, 0x68abe9443e0a7534 },
        { -0x1e8f987827e6ae06, -0x5ef5d3714d6f3885, -0x18c7d05fc129988d, 0x0a4d84710bcc4b54 },
        { -0x25ed393bf87ce235, 0x0da230d74d5c510d, 0x4ab1531e6bd404e1, 0x4106b166bcf440ef }
    },
    {
        { -0x5b7a332ac61b130e, 0x5aa3f3ad0555bab5, 0x145e3439937df82d, 0x1238b51e1214283f },
        { 0x02e57a421cd23668, 0x4ad9fb5d0eaef6fd, -0x6ab198d84edbbb80, 0x7f792f9d2699f331 },
        { 0x0b886b925fd4d924, 0x60906f7a3626a80d, -0x132c984b467542ee, 0x2876beb1def344cf }
    },
    {
        { -0x2a6b4cccc5757a08, 0x4ea37689e78d7d58, 0x73bf9f455e8e351f, 0x5507d7d2bc41ebb4 },
        { -0x237b16ca9cebb96f, 0x632fe8a0d61f23f4, 0x4caa800612a9a8d5, 0x48f9dbfa0e9918d3 },
        { 0x1ceb2903299572fc, 0x7c8ccaa29502d0ee, -0x6e405bcbee331985, 0x5784481964a831e7 }
    },
},
{
    {
        { -0x29302e10a0223f64, -0x17d4c10208a8a232, 0x25d56b5d201634c2, 0x3041c6bb04ed2b9b },
        { -0x2583d4da98972a6d, -0x673e3fa8bbdd35ed, -0x0e57f42a35f531e3, 0x29cdd1adc088a690 },
        { 0x0ff2f2f9d956e148, -0x5218688a60ca94d2, 0x1a4698bb5f6c025c, 0x104bbd6814049a7b }
    },
    {
        { -0x56a265a029800e9d, -0x16d41962b338a97f, -0x4807fdb321df0da9, 0x204f2a20fb072df5 },
        { 0x51f0fd3168f1ed67, 0x2c811dcdd86f3bc2, 0x44dc5c4304d2f2de, 0x5be8cc57092a7149 },
        { -0x37ebc4c2cf144f87, 0x7589155abd652e30, 0x653c3c318f6d5c31, 0x2570fb17c279161f }
    },
    {
        { 0x192ea9550bb8245a, -0x37190457706faf2f, 0x7986ea2d88a4c935, 0x241c5f91de018668 },
        { 0x3efa367f2cb61575, -0x0a069089e329fd94, -0x1738ebd59a4ada9e, 0x3dcb65ea53030acd },
        { 0x28d8172940de6caa, -0x7040d30fdd268cc6, 0x16d7fcdd235b01d1, 0x08420edd5fcdf0e5 }
    },
    {
        { 0x0358c34e04f410ce, -0x49eca4a5d891f97b, 0x5d9670c7ebb91521, 0x04d654f321db889c },
        { -0x3200df547c9d05b6, 0x57e118d4e21a3e6e, -0x1ce869e803c619d5, 0x0d9a53efbc1769fd },
        { 0x5e7dc116ddbdb5d5, 0x2954deb68da5dd2d, 0x1cb608173334a292, 0x4a7a4f2618991ad7 }
    },
    {
        { 0x24c3b291af372a4b, -0x6c257d8f8e7eb80e, -0x227b7a9b7976610e, 0x4a96314223e0ee33 },
        { -0x0b58e7fda04ea06b, 0x3df65f346b5c1b8f, -0x32030f7aff1feeee, 0x11b50c4cddd31848 },
        { -0x5917d8bbf75b002a, 0x738e177e9c1576d9, 0x773348b63d02b3f2, 0x4f4bce4dce6bcc51 }
    },
    {
        { 0x30e2616ec49d0b6f, -0x1ba98e703513dce9, 0x48eb409bf26b4fa6, 0x3042cee561595f37 },
        { -0x58e031a51ddbda7c, 0x26ea725692f58a9e, -0x2de5f628e315c30c, 0x73fcdd14b71c01e6 },
        { 0x427e7079449bac41, -0x7aa51c92431dcef6, 0x4cae76215f841a7c, 0x389e740c9a9ce1d6 }
    },
    {
        { -0x36428709a8f153d8, -0x1aa4f4cdd86e631f, 0x65fc3eaba19b91ed, 0x25c425e5d6263690 },
        { 0x64fcb3ae34dcb9ce, -0x68affcdc1cb72f53, 0x45b3f07d62c6381b, 0x61545379465a6788 },
        { 0x3f3e06a6f1d7de6e, 0x3ef976278e062308, -0x73eb09d9b1759389, 0x6539a08915484759 }
    },
    {
        { -0x223b242beb44b5e7, 0x19b2bc3c98424f8e, 0x48a89fd736ca7169, 0x0f65320ef019bd90 },
        { -0x162de08b3c2d088d, -0x3eafabbeda3b97bb, 0x624e5ce8f9b99e33, 0x11c5e4aac5cd186c },
        { -0x2b792e4e35021f3a, 0x4f3fe6e3163b5181, 0x59a8af0dfaf2939a, 0x4cabc7bdec33072a }
    },
},
{
    {
        { -0x083f5e63e5ab5fbc, 0x4a1c5e2477bd9fbb, -0x591c35eea50dd68e, 0x1819bb953f2e9e0d },
        { 0x16faa8fb532f7428, -0x242bd15fb95b1d8e, 0x5337653b8b9ea480, 0x4065947223973f03 },
        { 0x498fbb795e042e84, 0x7d0dd89a7698b714, -0x7404f45bd8019d6b, 0x36ba82e721200524 }
    },
    {
        { -0x372962f5a8d8b12b, 0x45ba803260804b17, -0x20c325efddaa2054, 0x77d221232709b339 },
        { -0x29f13448bdba13bf, -0x02641761cbcb78ea, -0x36dbf5011bdd7b22, 0x4472f648d0531db4 },
        { 0x498a6d7064ad94d8, -0x5a4a37026509dd9d, -0x735712faba3ebe0c, 0x2c63bec3662d358c }
    },
    {
        { -0x65ae74c57a790741, -0x6118e509344e6910, -0x55f9da195dc7a30e, 0x1deb2176ddd7c8d1 },
        { 0x7fe60d8bea787955, -0x4623ee814a0bfe49, -0x6e383f65e6caa332, 0x22692ef59442bedf },
        { -0x7a9c2e65df993094, 0x401bfd8c4dcc7cd7, -0x2689594132f2709e, 0x67cfd773a278b05e }
    },
    {
        { 0x2d5fa9855a4e586a, 0x65f8f7a449beab7e, -0x55f8b2220de2cc2d, 0x185cba721bcb9dee },
        { -0x7213ce0510c11b8b, -0x6624007561dd026e, 0x512d11594e26cab1, 0x0cde561eec4310b9 },
        { -0x6c79625c0b1c34bf, -0x40fc6d0abf086882, 0x026204fcd0463b83, 0x3ec91a769eec6eed }
    },
    {
        { 0x0fad2fb7b0a3402f, 0x46615ecbfb69f4a8, -0x08ba43373a07155a, 0x7a5fa8794a94e896 },
        { 0x1e9df75bf78166ad, 0x4dfda838eb0cd7af, -0x45ffd1273e150678, 0x13fedb3e11f33cfc },
        { 0x52958faa13cd67a1, -0x69a11f7e74244ae9, 0x16e58daa2e8845b3, 0x357d397d5499da8f }
    },
    {
        { 0x481dacb4194bfbf8, 0x4d77e3f1bae58299, 0x1ef4612e7d1372a0, 0x3a8d867e70ff69e1 },
        { 0x1ebfa05fb0bace6c, -0x36cb9df3e35065e2, -0x3388e33be27d49e6, 0x2d94a16aa5f74fec },
        { 0x6f58cd5d55aff958, -0x45c155a38aa988df, 0x75c123999165227d, 0x69be1343c2f2b35e }
    },
    {
        { -0x7d44425397b4721d, -0x5d0b382fc035f8e8, 0x337f92fbe096aaa8, 0x200d4d8c63587376 },
        { 0x0e091d5ee197c92a, 0x4f51019f2945119f, 0x143679b9f034e99c, 0x7d88112e4d24c696 },
        { 0x208aed4b4893b32b, 0x3efbf23ebe59b964, -0x289d214f245a1af9, 0x69607bd681bd9d94 }
    },
    {
        { 0x3b7f3bd49323a902, 0x7c21b5566b2c6e53, -0x1a45700ac587ad59, 0x28bc77a5838ece00 },
        { -0x0941fdef9721e31f, -0x172ae718f12343e1, -0x1c10022fe4aafa5b, 0x35f63353d3ec3fd0 },
        { 0x63ba78a8e25d8036, 0x63651e0094333490, 0x48d82f20288ce532, 0x3a31abfa36b57524 }
    },
},
{
    {
        { -0x3f708770c0872d77, -0x01cf58d35ebfb261, -0x0d887403309a3363, 0x7ee498165acb2021 },
        { 0x239e9624089c0a2e, -0x38b73b3fc501b8c8, 0x17dbed2a764fa12a, 0x639b93f0321c8582 },
        { 0x7bd508e39111a1c3, 0x2b2b90d480907489, -0x182d513d518d02e7, 0x0edf493c85b602a6 }
    },
    {
        { 0x6767c4d284764113, -0x5f6fbfc0080a07cb, 0x1c8fcffacae6bede, 0x04c00c54d1dfa369 },
        { -0x51337ea7a664a598, -0x15a8b0f014521df2, 0x4fe41d7422b67f07, 0x403b92e3019d4fb4 },
        { 0x4dc22f818b465cf8, 0x71a0f35a1480eff8, -0x51174052fb3829a9, 0x355bb12ab26176f4 }
    },
    {
        { -0x5cfe2538a5738ce8, -0x126ffc624c3155ef, 0x6f077cbf3bae3f2d, 0x7518eaf8e052ad8e },
        { -0x58e19b338b6c440c, -0x1a427b26135c4f3d, 0x0a6bc50cfa05e785, 0x0f9b8132182ec312 },
        { -0x5b77a63be48093ce, 0x0f2d60bcf4383298, 0x1815a929c9b1d1d9, 0x47c3871bbb1755c4 }
    },
    {
        { -0x0419a2af37af9950, 0x62ecc4b0b3a299b0, -0x1ac8ab15bbe51720, 0x08fea02ce8d48d5f },
        { 0x5144539771ec4f48, -0x07fa4e823673a292, -0x089d3ee5b83c3995, 0x00b89b85764699dc },
        { -0x7db2228997211530, -0x379bbadfb497a2dd, -0x4aeb3032a276299b, 0x473829a74f75d537 }
    },
    {
        { 0x23d9533aad3902c9, 0x64c2ddceef03588f, 0x15257390cfe12fb4, 0x6c668b4d44e4d390 },
        { -0x7d2d258ab9863be8, -0x19c428274d9e7210, 0x355eef24ac47eb0a, 0x2078684c4833c6b4 },
        { 0x3b48cf217a78820c, -0x0895f54d7ed8c169, -0x56939a5873711285, 0x7411a6054f8a433f }
    },
    {
        { 0x579ae53d18b175b4, 0x68713159f392a102, -0x7baa1345e110ca0b, 0x1ec9a872458c398f },
        { 0x4d659d32b99dc86d, 0x044cdc75603af115, -0x4cb38ed3233d1b78, 0x7c136574fb8134ff },
        { -0x47195b2bff5daf65, -0x647e28fdf4377d4c, 0x57e7cc9bf1957561, 0x3add88a5c7cd6460 }
    },
    {
        { -0x7a3d672ba6c6cfba, -0x7081ca67a009a614, 0x1d2ca22af2f66e3a, 0x61ba1131a406a720 },
        { -0x5476a88f49ca230e, 0x02dfef6cf66c1fbc, -0x7aacfd9741492e79, 0x249929fccc879e74 },
        { -0x5c2f5f0ee96a6fd7, 0x023b6b6cba7ebd89, 0x7bf15a3e26783307, 0x5620310cbbd8ece7 }
    },
    {
        { 0x6646b5f477e285d6, 0x40e8ff676c8f6193, -0x59138cee544a6b23, 0x7ec846f3658cec4d },
        { 0x528993434934d643, -0x462407f95aeddd0b, -0x709278703c0be3de, 0x37676a2a4d9d9730 },
        { -0x64a170c0e25dd139, 0x130f1d776c01cd13, 0x214c8fcfa2989fb8, 0x6daaf723399b9dd5 }
    },
},
{
    {
        { -0x7e514422d32ecf90, -0x69d1bcda07a5f162, -0x216c6e5535200135, 0x53177fda52c230e6 },
        { 0x591e4a5610628564, 0x2a4bb87ca8b4df34, -0x21d5da8d185c71bd, 0x3cbdabd9fee5046e },
        { -0x584368f9af462187, 0x3d12a7fbc301b59b, 0x02652e68d36ae38c, 0x79d739835a6199dc }
    },
    {
        { 0x21c9d9920d591737, -0x6415be2d164b932a, -0x1df17bdff2764036, 0x79d99f946eae5ff8 },
        { -0x26cab209bece3e43, 0x758094a186ec5822, 0x4464ee12e459f3c2, 0x6c11fce4cb133282 },
        { -0x0e84b7ca9798cdfb, 0x387deae83caad96c, 0x61b471fd56ffe386, 0x31741195b745a599 }
    },
    {
        { 0x17f8ba683b02a047, 0x50212096feefb6c8, 0x70139be21556cbe2, 0x203e44a11d98915b },
        { -0x172efe6f4885c9f5, -0x66467cdf666a18fe, -0x42b0200705fdb856, 0x2772e344e0d36a87 },
        { -0x2979c145c8461c61, 0x105bc169723b5a23, 0x104f6459a65c0762, 0x567951295b4d38d4 }
    },
    {
        { 0x07242eb30d4b497f, 0x1ef96306b9bccc87, 0x37950934d8116f45, 0x05468d6201405b04 },
        { 0x535fd60613037524, -0x1def52094f043d96, -0x5372f564dc166f52, 0x47204d08d72fdbf9 },
        { 0x00f565a9f93267de, -0x313028723f2a7176, -0x5dea1d230ce71d72, 0x4599ee919b633352 }
    },
    {
        { -0x538b929479e51a87, 0x31ab0650f6aea9dc, 0x241d661140256d4c, 0x2f485e853d21a5de },
        { -0x2c3ddf358f1f1895, -0x4ed415a71560cf6c, 0x294ddec8c3271282, 0x0c3539e1a1d1d028 },
        { 0x329744839c0833f3, 0x6fe6257fd2abc484, 0x5327d1814b358817, 0x65712585893fe9bc }
    },
    {
        { -0x7e3d60e428f711c1, -0x2234a5fa519bf830, -0x68513e282d5c1459, 0x1590521a91d50831 },
        { -0x63efd048cd59ee9f, -0x1b71ef22cb2adf58, 0x365c63546f9a9176, 0x32f6fe4c046f6006 },
        { 0x40a3a11ec7910acc, -0x6fec20070e92d852, 0x1a9720d8abb195d4, 0x1bb9fe452ea98463 }
    },
    {
        { -0x30a1936a33c98b84, 0x294201536b0bc30d, 0x453ac67cee797af0, 0x5eae6ab32a8bb3c9 },
        { -0x162e26af4c2ab062, 0x2d5f9cbee00d33c1, 0x51c2c656a04fc6ac, 0x65c091ee3c1cbcc9 },
        { 0x7083661114f118ea, 0x2b37b87b94349cad, 0x7273f51cb4e99f40, 0x78a2a95823d75698 }
    },
    {
        { -0x4b0dc3bda107cdf9, -0x54076b2c3656cb4b, -0x2f8f73ecc6027809, 0x1876789117166130 },
        { -0x5d4f8d16a373d532, 0x69cffc96651e9c4b, 0x44328ef842e7b42b, 0x5dd996c122aadeb3 },
        { -0x6da4a10f98f3af84, -0x7e6437bd46c3cc41, 0x10792e9a70dd003f, 0x59ad4b7a6e28dc74 }
    },
},
{
    {
        { 0x583b04bfacad8ea2, 0x29b743e8148be884, 0x2b1e583b0810c5db, 0x2b5449e58eb3bbaa },
        { 0x5f3a7562eb3dbe47, -0x0815c7ab71425f48, 0x00c3e53145747299, 0x1304e9e71627d551 },
        { 0x789814d26adc9cfe, 0x3c1bab3f8b48dd0b, -0x25f01e00068639f6, 0x4468de2d7c2dd693 }
    },
    {
        { 0x4b9ad8c6f86307ce, 0x21113531435d0c28, -0x2b57993a9a8588d4, 0x5da6427e63247352 },
        { 0x51bb355e9419469e, 0x33e6dc4c23ddc754, -0x6c5a4929bb80669e, 0x6cce7c6ffb44bd63 },
        { 0x1a94c688deac22ca, -0x46f991084451e008, -0x775273c772a6a7f1, 0x58f29abfe79f2ca8 }
    },
    {
        { 0x4b5a64bf710ecdf6, -0x4eb31ac7b9d3d6c4, 0x3643d056d50b3ab9, 0x6af93724185b4870 },
        { -0x16f130547218c198, 0x54036f9f377e76a5, -0x0fb6a4f441fea67e, 0x577629c4a7f41e36 },
        { 0x3220024509c6a888, -0x2d1fc9ecb4aa768d, -0x7c1dc9dcc3ccd761, 0x701f25bb0caec18f }
    },
    {
        { -0x62e7092683413eed, -0x7bb5f9198b40241c, 0x20f5b522ac4e60d6, 0x720a5bc050955e51 },
        { -0x3c574f071b9e9313, -0x08ff99f161da5783, 0x61e3061ff4bca59c, 0x2e0c92bfbdc40be9 },
        { 0x0c3f09439b805a35, -0x17b174c89dbd5404, 0x691417f35c229346, 0x0e9b9cbb144ef0ec }
    },
    {
        { -0x7211642aa24e4112, -0x363c54c8f58dc047, 0x44a8f1bf1c68d791, 0x366d44191cfd3cde },
        { -0x04452b7004a8df53, -0x117e6e942406f2f2, -0x2b7ecead9caabc41, 0x221104eb3f337bd8 },
        { -0x61c3e8bc0d4373ec, 0x2eda26fcb5856c3b, -0x3347d0f197580469, 0x4167a4e6bc593244 }
    },
    {
        { -0x3d41d99a07317012, -0x169800eb177f29d4, -0x0ed19181d0c9b112, 0x34b33370cb7ed2f6 },
        { 0x643b9d2876f62700, 0x5d1d9d400e7668eb, 0x1b4b430321fc0684, 0x7938bb7e2255246a },
        { -0x323a6e11797e2934, -0x31fdef63127a58ad, -0x128b7a3ea77f777d, 0x1176fc6e2dfe65e4 }
    },
    {
        { -0x246f1d76b688f148, -0x670433d5530bbf5d, 0x21354ffeded7879b, 0x1f6a3e54f26906b6 },
        { -0x4b50932fa4639e65, 0x2ddfc9f4b2a58480, 0x3d4fa502ebe94dc4, 0x08fc3a4c677d5f34 },
        { 0x60a4c199d30734ea, 0x40c085b631165cd6, -0x1dccc1dc08a67d6b, 0x4f2fad0116b900d1 }
    },
    {
        { -0x69d326e248c449c8, -0x19fa885503ed63f8, 0x6f619b39f3b61689, 0x3451995f2944ee81 },
        { 0x44beb24194ae4e54, 0x5f541c511857ef6c, -0x59e194d2c972fb68, 0x445484a4972ef7ab },
        { -0x6ead032f60158284, 0x4a816c94b0935cf6, 0x258e9aaa47285c40, 0x10b89ca6042893b7 }
    },
},
{
    {
        { -0x29832129862cb560, -0x33f4613f33b24c61, -0x5aca5ba91ca2e6f1, 0x2e05d9eaf61f6fef },
        { -0x64d5bd91c49b9fdb, 0x32127190385ce4cf, -0x5da3003d229215bb, 0x06409010bea8de75 },
        { -0x3bb86fe529e414a7, 0x661f19bce5dc880a, 0x24685482b7ca6827, 0x293c778cefe07f26 }
    },
    {
        { 0x16c795d6a11ff200, -0x348f2f1d4ea7ea37, -0x760d6cdf64ac6a4b, 0x50b8c2d031e47b4f },
        { -0x797f618ff8f96f6a, -0x5528a4ea1b1afe77, 0x07f35715a21a0147, 0x0487f3f112815d5e },
        { 0x48350c08068a4962, 0x6ffdd05351092c9a, 0x17af4f4aaf6fc8dd, 0x4b0553b53cdba58b }
    },
    {
        { -0x40fadee4d83ead2c, 0x5ec26849bd1af639, 0x5e0b2caa8e6fab98, 0x054c8bdd50bd0840 },
        { -0x639a0341e4cd0087, -0x148a1560fc4af065, -0x0312d59393f819fa, 0x35106cd551717908 },
        { 0x38a0b12f1dcf073d, 0x4b60a8a3b7f6a276, -0x012a53da2cbfb066, 0x72e82d5e5505c229 }
    },
    {
        { 0x00d9cdfd69771d02, 0x410276cd6cfbf17e, 0x4c45306c1cb12ec7, 0x2857bf1627500861 },
        { 0x6b0b697ff0d844c8, -0x44ed07a3268634b7, -0x2d5abe393e25f0e1, 0x7b7c242958ce7211 },
        { -0x60de6fc0fefe9762, -0x2886202c4079effb, -0x5edd11a0c214f0e5, 0x510df84b485a00d4 }
    },
    {
        { 0x24b3c887c70ac15e, -0x4f0c5aa8047e48ce, -0x64d321d01a8733e5, 0x4cf7ed0703b54f8e },
        { -0x5abecc446d885e06, 0x74ec3b6263991237, 0x1a3c54dc35d2f15a, 0x2d347144e482ba3a },
        { 0x6bd47c6598fbee0f, -0x61b8cc1d54aa41d3, 0x1093f624127610c5, 0x4e05e26ad0a1eaa4 }
    },
    {
        { 0x1833c773e18fe6c0, -0x1c3b8ee52c378d9b, 0x3bfd3c4f0116b283, 0x1955875eb4cd4db8 },
        { -0x2564949db4ace0e0, 0x429a760e77509abb, -0x24160add17dc3480, 0x618f1856880c8f82 },
        { 0x6da6de8f0e399799, 0x7ad61aa440fda178, -0x4cd327efa1ca9c23, 0x15f6beae2ae340ae }
    },
    {
        { -0x4565f0846dba1deb, -0x0c979ed22673f245, 0x2e84e4cbf220b020, 0x6ba92fe962d90eda },
        { -0x79d434f3ce13c59e, -0x7ef1d4baeec70c3e, 0x788ec4b839dac2a4, 0x28f76867ae2a9281 },
        { 0x3e4df9655884e2aa, -0x429d0424242b9a5b, -0x28a69355f2161adc, 0x6e8042ccb2b1b3d7 }
    },
    {
        { 0x1530653616521f7e, 0x660d06b896203dba, 0x2d3989bc545f0879, 0x4b5303af78ebd7b0 },
        { -0x0ef2c3d631d73592, -0x452cbabf0349f6c3, -0x18bd91285d15d2c1, 0x08af9d4e4ff298b9 },
        { 0x72f8a6c3bebcbde8, 0x4f0fca4adc3a8e89, 0x6fa9d4e8c7bfdf7a, 0x0dcf2d679b624eb7 }
    },
},
{
    {
        { 0x753941be5a45f06e, -0x2f8351129263a09b, 0x11776b9c72ff51b6, 0x17d2d1d9ef0d4da9 },
        { 0x3d5947499718289c, 0x12ebf8c524533f26, 0x0262bfcb14c3ef15, 0x20b878d577b7518e },
        { 0x27f2af18073f3e6a, -0x02c01ae628adef97, 0x22e3b72c3ca60022, 0x72214f63cc65c6a7 }
    },
    {
        { 0x1d9db7b9f43b29c9, -0x29fa7db5b0ae708b, -0x0d3f8d42ced0623c, 0x1f24ac855a1545b0 },
        { -0x4b1c80bfacf8596d, -0x5458eb28d0cc986b, -0x29042f588c89ef67, 0x5fdf48c58171cbc9 },
        { 0x24d608328e9505aa, 0x4748c1d10c1420ee, -0x38001ba3f904da5e, 0x00ba739e2ae395e6 }
    },
    {
        { -0x51bbd90a157744da, 0x360679d984973bfb, 0x5c9f030c26694e50, 0x72297de7d518d226 },
        { 0x592e98de5c8790d6, -0x1a40482cba3d5d21, 0x115a3b60f9b49922, 0x03283a3e67ad78f3 },
        { 0x48241dc7be0cb939, 0x32f19b4d8b633080, -0x2c2036f2fdd76cf8, 0x05e1296846271945 }
    },
    {
        { -0x52404437dbd3bab0, -0x4337f3132fcf7e27, -0x7bca99590a37206e, 0x78cf25d38258ce4c },
        { -0x457d114cd263b6a6, -0x311037030ed44684, -0x4fd254516c4a2e20, 0x39c00c9c13698d9b },
        { 0x15ae6b8e31489d68, -0x557ae35463d40f79, -0x3658a5680fb105fb, 0x006b52076b3ff832 }
    },
    {
        { -0x0a3481e94631f7d3, 0x3407f14c417abc29, -0x2b4c9431d40b5855, 0x7de2e9561a9f75ce },
        { 0x29e0cfe19d95781c, -0x497e20e7699cef1e, 0x57df39d370516b39, 0x4d57e3443bc76122 },
        { -0x218f2b0b495aa135, 0x4801527f5d85db99, -0x24363bbf2c11657f, 0x6b2a90af1a6029ed }
    },
    {
        { 0x77ebf3245bb2d80a, -0x27cfe4b8d046f865, -0x39b8190db3118ccd, 0x465812c8276c2109 },
        { 0x6923f4fc9ae61e97, 0x5735281de03f5fd1, -0x589b51bc19122ed3, 0x5fd8f4e9d12d3e4a },
        { 0x4d43beb22a1062d9, 0x7065fb753831dc16, 0x180d4a7bde2968d7, 0x05b32c2b1cb16790 }
    },
    {
        { -0x08035bd3852a7e6b, 0x3214286e4333f3cc, -0x493d62f2cbf46863, 0x31771a48567307e1 },
        { -0x373fa1332db25703, -0x5e30e553fa20107d, -0x2441100d8206329f, 0x3b5556a37b471e99 },
        { 0x32b0c524e14dd482, -0x124caeabe5d45b4a, -0x5c2e9fb7d7d4a50d, 0x4fc079d27a7336eb }
    },
    {
        { -0x23cb74bbf3793af3, 0x1337cbc9cc94e651, 0x6422f74d643e3cb9, 0x241170c2bae3cd08 },
        { 0x51c938b089bf2f7f, 0x2497bd6502dfe9a7, -0x00003f63877f1bad, 0x124567cecaf98e92 },
        { 0x3ff9ab860ac473b4, -0x0f6ee211feec1bcb, 0x4ae75060ebc6c4af, 0x3f8612966c87000d }
    },
},
{
    {
        { 0x529fdffe638c7bf3, -0x20d4619fc774b66b, -0x1fd84cb0e452fdb7, 0x7bc92fc9b9fa74ed },
        { 0x0c9c5303f7957be4, -0x5c3ce5df1f7a3ebb, -0x4f8de28e2f7affb0, 0x0aba390eab0bf2da },
        { -0x606810d17fe52607, -0x7c9682ab865025c6, -0x16f94c0042a694b0, 0x02672b37dd3fb8e0 }
    },
    {
        { -0x116458d6c673580b, -0x146359da85b7b625, 0x29eb29ce7ec544e1, 0x232ca21ef736e2c8 },
        { 0x48b2ca8b260885e4, -0x5bd794137d4cb3e4, -0x6c81e5d9e80a708c, 0x741d1fcbab2ca2a5 },
        { -0x409ebdc2dac034e9, 0x08803ceafa39eb14, -0x0e79fd2067ae3851, 0x0400f3a049e3414b }
    },
    {
        { 0x2efba412a06e7b06, 0x146785452c8d2560, -0x2068ec1429856e39, 0x32830ac7157eadf3 },
        { -0x5431fb89459e3aa5, 0x36a3d6d7c4d39716, 0x6eb259d5e8d82d09, 0x0c9176e984d756fb },
        { 0x0e782a7ab73769e8, 0x04a05d7875b18e2c, 0x29525226ebcceae1, 0x0d794f8383eba820 }
    },
    {
        { 0x7be44ce7a7a2e1ac, 0x411fd93efad1b8b7, 0x1734a1d70d5f7c9b, 0x0d6592233127db16 },
        { -0x00ca0a3461eae90c, -0x117fa4309b7551bb, -0x0f28c3d446c5610d, 0x097b0bf22092a6c2 },
        { -0x3b7454eade5628cd, -0x593d151529e544db, 0x625c6c1cc6cb4305, 0x7fc90fea93eb3a67 }
    },
    {
        { -0x3ad8214a63834dc3, -0x6aac6e96acd7bfb2, -0x29bc6d7e8330d386, 0x6ce97dabf7d8fa11 },
        { 0x0408f1fe1f5c5926, 0x1a8f2f5e3b258bf4, 0x40a951a2fdc71669, 0x6598ee93c98b577e },
        { 0x25b5a8e50ef7c48f, -0x149fcbee90d31ace, -0x3a18ae8c1ac21ac9, 0x73119fa08c12bb03 }
    },
    {
        { 0x7845b94d21f4774d, -0x409d0e93876848d9, 0x671857c03c56522b, 0x3cd6a85295621212 },
        { -0x12cfed6bac0e5b35, -0x4319de36370ac879, -0x0534d4ecc7411847, 0x3025798a9ea8428c },
        { 0x3fecde923aeca999, -0x4255a4ff9d173ed1, 0x67b99dfc96988ade, 0x3f52c02852661036 }
    },
    {
        { -0x6da74066113be93a, -0x5375afe8562d098f, 0x629549ab16dea4ab, 0x05d0e85c99091569 },
        { -0x00155b71d5ecae3a, 0x28624754fa7f53d7, 0x0b5ba9e57582ddf1, 0x60c0104ba696ac59 },
        { 0x051de020de9cbe97, -0x05f803a94af4308c, 0x378cec9f0f11df65, 0x36853c69ab96de4d }
    },
    {
        { 0x4433c0b0fac5e7be, 0x724bae854c08dcbe, -0x0e0db33bb9687065, 0x4a0aff6d62825fc8 },
        { 0x36d9b8de78f39b2d, 0x7f42ed71a847b9ec, 0x241cd1d679bd3fde, 0x6a704fec92fbce6b },
        { -0x16e804619ef6acff, -0x3efd206bfd5f6d08, -0x40f61d0a0599e6f5, 0x681109bee0dcfe37 }
    },
},
{
    {
        { -0x63e70305c9fb72ed, 0x29159db373899ddd, -0x2360caf4606d2f56, 0x26f57eee878a19d4 },
        { 0x559a0cc9782a0dde, 0x551dcdb2ea718385, 0x7f62865b31ef238c, 0x504aa7767973613d },
        { 0x0cab2cd55687efb1, 0x5180d162247af17b, -0x7a3ea5cbb0a5db99, 0x4041943d9dba3069 }
    },
    {
        { 0x4b217743a26caadd, 0x47a6b424648ab7ce, -0x34e2b085fc04361d, 0x12d931429800d019 },
        { -0x3c3f1145bc14336a, -0x728b6363d9156351, -0x26056a11e388333a, 0x1420a1d97684340f },
        { 0x00c67799d337594f, 0x5e3c5140b23aa47b, 0x44182854e35ff395, 0x1b4f92314359a012 }
    },
    {
        { 0x33cf3030a49866b1, 0x251f73d2215f4859, -0x547d55bfae210b0a, 0x5ff191d56f9a23f6 },
        { 0x3e5c109d89150951, 0x39cefa912de9696a, 0x20eae43f975f3020, 0x239b572a7f132dae },
        { -0x7e612bcc53d26f98, 0x2883ab795fc98523, -0x10ba8d7faa6c14c3, 0x020c526a758f36cb }
    },
    {
        { -0x16ce10a60fbd3377, 0x2c589c9d8e124bb6, -0x52371e755138a669, 0x452cfe0a5602c50c },
        { 0x779834f89ed8dbbc, -0x370d550623835b94, -0x56adb3235c1e4f8c, 0x02aacc4615313877 },
        { -0x795f085f9b878821, -0x443b9bd8f19f8361, -0x54e815da0e04ee37, 0x4cfb7d7b304b877b }
    },
    {
        { -0x1d79663d687610ee, 0x2b6ecd71df57190d, -0x3cbc37a813368f30, 0x5b1d4cbc434d3ac5 },
        { 0x72b43d6cb89b75fe, 0x54c694d99c6adc80, -0x473c55c8c11cb361, 0x14b4622b39075364 },
        { -0x4904d9ea33f560da, 0x3a4f0e2bb88dcce5, 0x1301498b3369a705, 0x2f98f71258592dd1 }
    },
    {
        { 0x2e12ae444f54a701, -0x0301c10f56342822, -0x314076f28a7ca220, 0x1d8062e9e7614554 },
        { 0x0c94a74cb50f9e56, 0x5b1ff4a98e8e1320, -0x65d533de7dcff099, 0x3a6ae249d806aaf9 },
        { 0x657ada85a9907c5a, 0x1a0ea8b591b90f62, -0x72f1e20420cb4b17, 0x298b8ce8aef25ff3 }
    },
    {
        { -0x7c858d15f5de9a22, 0x3fab07b40bcf79f6, 0x521636c77738ae70, 0x6ba6271803a7d7dc },
        { 0x2a927953eff70cb2, 0x4b89c92a79157076, -0x6be7ba85cf583096, 0x34b8a8404d5ce485 },
        { -0x3d91134a7c96cccb, -0x2a57ec209c4a0103, -0x5d6c55655b4dda8d, 0x71d62bdd465e1c6a }
    },
    {
        { -0x32d24a254e08a10b, -0x28806a30e94f9a0b, 0x14571fea3f49f085, 0x1c333621262b2b3d },
        { 0x6533cc28d378df80, -0x0924bc86f5f05b4c, -0x1c9ba00608fe25a6, 0x74d5f317f3172ba4 },
        { -0x57901aab9826357f, 0x398b7c752b298c37, -0x2592f76d1c539dc5, 0x4aebcc4547e9d98c }
    },
},
{
    {
        { 0x0de9b204a059a445, -0x1ea34b55b4e852f1, -0x1e4413ade0863aa9, 0x2633f1b9d071081b },
        { 0x53175a7205d21a77, -0x4f3fbbdd2c46cb2c, -0x52260db422a21524, 0x074f46e69f10ff8c },
        { -0x3e04be88fe7466f0, -0x5915df2393f01ec0, -0x299e0c18bcab3901, 0x5ecb72e6f1a3407a }
    },
    {
        { -0x01151ef917179669, -0x679ccc80672f6c7d, -0x6b8fb7f155f91411, 0x038b6898d4c5c2d0 },
        { -0x5aea5ce4dda604b2, 0x0960f3972bcac52f, -0x124ad01372cbab35, 0x382e2720c476c019 },
        { -0x0c6e3ae27531af5a, 0x3142d0b9ae2d2948, -0x24b2a5e580db3580, 0x21aeba8b59250ea8 }
    },
    {
        { 0x53853600f0087f23, 0x4c461879da7d5784, 0x6af303deb41f6860, 0x0a3c16c5c27c18ed },
        { 0x24f13b34cf405530, 0x3c44ea4a43088af7, 0x5dd5c5170006a482, 0x118eb8f8890b086d },
        { 0x17e49c17cc947f3d, -0x33391259553e2d85, -0x209f6d314f0f71aa, 0x4909b3e22c67c36b }
    },
    {
        { 0x59a16676706ff64e, 0x10b953dd0d86a53d, 0x5848e1e6ce5c0b96, 0x2d8b78e712780c68 },
        { -0x63637a159c01d177, -0x41e4506ef16bed14, -0x7084557579040185, 0x0fb17f9fef968b6c },
        { 0x79d5c62eafc3902b, 0x773a215289e80728, -0x3c7519bf1efedf47, 0x09ae23717b2b1a6d }
    },
    {
        { 0x10ab8fa1ad32b1d0, -0x165312e41d8874dc, -0x577a943fc8c216f1, 0x66f35ddddda53996 },
        { -0x4495e6d5b1b2f7c4, 0x34ace0630029e192, -0x67dba5a655054515, 0x6d9c8a9ada97faac },
        { -0x2d826504db668cdd, 0x1bb7e07ef6f01d2e, 0x2ba7472df52ecc7f, 0x03019b4f646f9dc8 }
    },
    {
        { -0x50f64deb194c2395, 0x3f7573b5ad7d2f65, -0x2fe62677eff5dc50, 0x392b63a58b5c35f7 },
        { 0x04a186b5565345cd, -0x111899ef433bee96, 0x689c73b478fb2a45, 0x387dcbff65697512 },
        { 0x4093addc9c07c205, -0x3a9a41ea0acd3c82, 0x63dbecfd1583402a, 0x61722b4aef2e032e }
    },
    {
        { -0x294f85aa7e34f1c4, 0x290ff006d9444969, 0x08680b6a16dcda1f, 0x5568d2b75a06de59 },
        { 0x0012aafeecbd47af, 0x55a266fb1cd46309, -0x0dfc1497f69838d4, 0x39633944ca3c1429 },
        { -0x72f34773e4c8301f, 0x05b6a5a3053818f3, -0x0d1643fb487826a7, 0x6beba1249add7f64 }
    },
    {
        { 0x5c3cecb943f5a53b, -0x633659e2f93f720e, -0x30459c657a76abb9, 0x5a845ae80df09fd5 },
        { 0x1d06005ca5b1b143, 0x6d4c6bb87fd1cda2, 0x6ef5967653fcffe7, 0x097c29e8c1ce1ea5 },
        { 0x4ce97dbe5deb94ca, 0x38d0a4388c709c48, -0x3bc1312b5e962f69, 0x0a1249fff7e587c3 }
    },
},
{
    {
        { 0x0b408d9e7354b610, -0x7f94cdaca457a492, -0x2419c5fcb5a75df9, 0x173bd9ddc9a1df2c },
        { 0x12f0071b276d01c9, -0x1847453a793b7390, 0x5308129b71d6fba9, 0x5d88fbf95a3db792 },
        { 0x2b500f1efe5872df, 0x58d6582ed43918c1, -0x1912d8713698c520, 0x06e1cd13b19ea319 }
    },
    {
        { 0x472baf629e5b0353, 0x3baa0b90278d0447, 0x0c785f469643bf27, 0x7f3a6a1a8d837b13 },
        { 0x40d0ad516f166f23, 0x118e32931fab6abe, 0x3fe35e14a04d088e, 0x3080603526e16266 },
        { -0x0819bbc6a2c27ff5, -0x6a572aaa36fe120a, 0x68cd7830592c6339, 0x30d0fded2e51307e }
    },
    {
        { -0x634b68e19747b8b0, -0x5f6a8dd6999b4431, 0x5c8de72672fa412b, 0x4615084351c589d9 },
        { -0x1fa6b2e50dedcc4d, 0x1bdbe78ef0cc4d9c, 0x6965187f8f499a77, 0x0a9214202c099868 },
        { -0x436fe63f51465fd2, 0x55c7110d16034cae, 0x0e6df501659932ec, 0x3bca0d2895ca5dfe }
    },
    {
        { -0x639771496133fe41, -0x0f437c5259bb7691, -0x35d26aa0a085601e, 0x4ea8b4038df28241 },
        { 0x40f031bc3c5d62a4, 0x19fc8b3ecff07a60, -0x67e7c25decf04abb, 0x5631deddae8f13cd },
        { 0x2aed460af1cad202, 0x46305305a48cee83, -0x6ede88bab60ee5a1, 0x24ce0930542ca463 }
    },
    {
        { 0x3fcfa155fdf30b85, -0x2d08e971c9c8d15c, -0x4d1f9b219b6d07bc, 0x549928a7324f4280 },
        { 0x1fe890f5fd06c106, -0x4a3b97caa277ef0e, -0x7d87f701917350c2, 0x41d4e3c28a06d74b },
        { -0x0d91cd589c11e5d2, -0x516e1b482da00216, -0x43c42cc42e80b297, 0x491b66dec0dcff6a }
    },
    {
        { 0x75f04a8ed0da64a1, -0x12ddd350981dd7b5, -0x7dcb5c86e084845c, 0x4cf6b8b0b7018b67 },
        { -0x670a4ec23815cd59, -0x1c2a073381e92468, -0x53f540ad340726b9, 0x08f338d0c85ee4ac },
        { -0x3c7c57de66e58c43, -0x54d843fe20cdf386, -0x3ec2cce47b888f9d, 0x530d4a82eb078a99 }
    },
    {
        { 0x6d6973456c9abf9e, 0x257fb2fc4900a880, 0x2bacf412c8cfb850, 0x0db3e7e00cbfbd5b },
        { 0x004c3630e1f94825, 0x7e2d78268cab535a, -0x38b7dcdc337b0075, 0x65ea753f101770b9 },
        { 0x3d66fc3ee2096363, -0x7e29d3809e4a3495, 0x0fbe044213443b1a, 0x02a4ec1921e1a1db }
    },
    {
        { -0x0a379e9d0e3086a1, 0x118c861926ee57f2, 0x172124851c063578, 0x36d12b5dec067fcf },
        { 0x5ce6259a3b24b8a2, -0x47a88533ba505f48, -0x33341917745f8fc9, 0x3d143c51127809bf },
        { 0x126d279179154557, -0x2a1b70a30387c5f6, 0x36bdb6e8df179bac, 0x2ef517885ba82859 }
    },
},
{
    {
        { 0x1ea436837c6da1e9, -0x063e7650e0464242, 0x303001fcce5dd155, 0x28a7c99ebc57be52 },
        { -0x7742bc732ee1f2b6, 0x30cb610d43ccf308, -0x1f65f1c86e6c8434, 0x4559135b25b1720c },
        { -0x47026c66172e6163, -0x6f7e6e3469dbdc01, -0x4d46b728b838bd5d, 0x37f33226d7fb44c4 }
    },
    {
        { 0x33912553c821b11d, 0x66ed42c241e301df, 0x066fcc11104222fd, 0x307a3b41c192168f },
        { 0x0dae8767b55f6e08, 0x4a43b3b35b203a02, -0x1c8da5917f507387, 0x0f7a7fd1705fa7a3 },
        { -0x7114a2f8914aa320, 0x2fc536bfaa0d925a, -0x417e7cf023493918, 0x556c7045827baf52 }
    },
    {
        { -0x46b46ffdd40bbbfa, -0x542bdc81006f4acc, 0x7600a960faf86d3a, 0x2f45abdac2322ee3 },
        { -0x71d4ae8cfd162749, -0x1c1add96db78eb18, -0x42b04288b3569f4b, 0x6f4b4199c5ecada9 },
        { 0x61af4912c8ef8a6a, -0x1a705b01bc0491a2, -0x4a5033a2902bd831, 0x6a5393281e1e11eb }
    },
    {
        { 0x0fff04fe149443cf, 0x53cac6d9865cddd7, 0x31385b03531ed1b7, 0x5846a27cacd1039d },
        { -0x0c25aec65a2e1177, -0x7ebaba83006c9678, 0x3f622fed00e188c4, 0x0f513815db8b5a3d },
        { 0x4ff5cdac1eb08717, 0x67e8b29590f2e9bc, 0x44093b5e237afa99, 0x0d414bed8708b8b2 }
    },
    {
        { -0x7e77956dd6b53618, 0x23162b45d55547be, -0x6b3043bbfc8ea67d, 0x50eb8fdb134bc401 },
        { -0x30497d9a02f18a0a, -0x1ba4c1d7446f18f9, 0x7242a8de9ff92c7a, 0x685b3201933202dd },
        { -0x3f48c139294ccf33, -0x7b1bb7f8ecd0500f, 0x732b7352c4a5dee1, 0x5d7c7cf1aa7cd2d2 }
    },
    {
        { 0x33d1013e9b73a562, -0x6da310a8b713d91f, -0x580319eb22b97fa8, 0x78b0fad41e9aa438 },
        { -0x50c4b94085b5505e, -0x4878fa13b2bf2bef, 0x114f0c6aca7c15e3, 0x3f364faaa9489d4d },
        { -0x40a95bce12fa4b78, -0x5acc199363b6a382, -0x179ad450780c9ae6, 0x0241800059d66c33 }
    },
    {
        { 0x28350c7dcf38ea01, 0x7c6cdbc0b2917ab6, -0x531830417a8f7d09, 0x4d2845aba2d9a1e0 },
        { -0x314f88015c85a41c, -0x249bd0fd1a5a1149, -0x3d192f3ab8ed8f48, 0x4771b65538e4529c },
        { -0x44ac801fbb8f8f22, -0x3458bbbc922aa821, -0x2c4a5cb8c9ff2435, 0x4aeabbe6f9ffd7f8 }
    },
    {
        { 0x6a2134bcc4a9c8f2, -0x040702e37531d1c9, 0x000ae3049911a0ba, 0x046e3a616bc89b9e },
        { 0x4630119e40d8f78c, -0x5fe5643ac38ef1ef, 0x486d2b258910dd79, 0x1e6c47b3db0324e5 },
        { 0x14e65442f03906be, 0x4a019d54e362be2a, 0x68ccdfec8dc230c7, 0x7cfb7e3faf6b861c }
    },
},
{
    {
        { -0x69114004cfa4d0af, -0x2c06c752776a6948, -0x0f0ad238b92a22db, 0x57968290bb3a0095 },
        { 0x4637974e8c58aedc, -0x4610dd04540fbe5c, -0x1e7a26a9167f8e76, 0x2f1b78fab143a8a6 },
        { -0x08e547bcf5df1eff, -0x0c6c9a72db0f13b9, -0x308af657911d112f, 0x7dc43e35dc2aa3e1 }
    },
    {
        { 0x5a782a5c273e9718, 0x3576c6995e4efd94, 0x0f2ed8051f237d3e, 0x044fb81d82d50a99 },
        { -0x7a69999a7782263d, -0x36f064ceb44facab, -0x391f720710df864f, 0x7ef72016758cc12f },
        { -0x3e20e73a56f81c27, 0x57b3371dce4c6359, -0x358fbacb4dfe44b7, 0x7f79823f9c30dd2e }
    },
    {
        { 0x6a9c1ff068f587ba, 0x0827894e0050c8de, 0x3cbf99557ded5be7, 0x64a9b0431c06d6f0 },
        { -0x7ccb2dc65c4aec18, -0x3ec98f2b46e05728, 0x12b54136f590bd33, 0x0a4e0373d784d9b4 },
        { 0x2eb3d6a15b7d2919, -0x4f4b095f2ac57dcb, 0x7156ce4389a45d47, 0x071a7d0ace18346c }
    },
    {
        { -0x33f3caaddf1ebbcf, 0x0d65950709b15141, -0x650a9de4df62a0ca, 0x7c69bcf7617755d3 },
        { -0x2cf8d255377845f5, 0x01262905bfa562ee, -0x30abcffd3f108975, 0x2c3bcc7146ea7e9c },
        { 0x07f0d7eb04e8295f, 0x10db18252f50f37d, -0x16ae565ce8e86729, 0x6f5a9a7322aca51d }
    },
    {
        { -0x18d62b145c26bb42, -0x7261f6bf7f875062, 0x4525567a47869c03, 0x02ab9680ee8d3b24 },
        { -0x745efff3d0be393b, -0x3b60863ef3010465, 0x4efa47703cc51c9f, 0x494e21a2e147afca },
        { -0x105b757a221af266, 0x219a224e0fb9a249, -0x05f6e0e226e10927, 0x6b5d76cbea46bb34 }
    },
    {
        { -0x1f06bee8e187dade, -0x0e19518bfc96c92d, 0x408b3ea2d0fcc746, 0x16fb869c03dd313e },
        { -0x77a8aa9313f3266c, 0x6472dc6f5cd01dba, -0x50fe96eb70bd4b89, 0x0ae333f685277354 },
        { 0x288e199733b60962, 0x24fc72b4d8abe133, 0x4811f7ed0991d03e, 0x3f81e38b8f70d075 }
    },
    {
        { 0x0adb7f355f17c824, 0x74b923c3d74299a4, -0x2a83c17434071509, 0x0ad3e2d34cdedc3d },
        { 0x7f910fcc7ed9affe, 0x545cb8a12465874b, -0x57c6812db4f3b8fc, 0x50510fc104f50993 },
        { 0x6f0c0fc5336e249d, 0x745ede19c331cfd9, -0x0d2902fff61101e4, 0x127c158bf0fa1ebe }
    },
    {
        { -0x215d703b51ae468c, 0x1d9973d3744dfe96, 0x6240680b873848a8, 0x4ed82479d167df95 },
        { -0x09e683bdd167865e, -0x5bb5222bad35c9b9, -0x64bec03eb4b15335, 0x354ef87d07ef4f68 },
        { -0x011c4add9f3a268b, 0x50352efceb41b0b8, -0x77f753cf56099ac4, 0x302d92d20539236d }
    },
},
{
    {
        { -0x6a847474f20ac3d0, 0x2a1c770a8e60f098, -0x4438598fcba86922, 0x22a48f9a90c99bc9 },
        { 0x4c59023fcb3efb7c, 0x6c2fcb99c63c2a94, -0x45be6f1d3c381f7c, 0x0e545daea51874d9 },
        { 0x6b7dc0dc8d3fac58, 0x5497cd6ce6e42bfd, 0x542f7d1bf400d305, 0x4159f47f048d9136 }
    },
    {
        { 0x748515a8bbd24839, 0x77128347afb02b55, 0x50ba2ac649a2a17f, 0x060525513ad730f1 },
        { 0x20ad660839e31e32, -0x07e1e42a7bfa41b0, -0x07f9bfa90b254397, 0x14d23dd4ce71b975 },
        { -0x0dc671f6755d807e, 0x6d7982bb89a1b024, -0x0596bf7bdeb22db4, 0x71ab966fa32301c3 }
    },
    {
        { -0x4ef775f8fd7f66ab, 0x43b273ea0b43c391, -0x3564985101f97913, 0x605eecbf8335f4ed },
        { 0x2dcbd8e34ded02fc, 0x1151f3ec596f22aa, -0x435daabcb1fcd726, 0x35768fbe92411b22 },
        { -0x7cdff59a93cbfbcf, -0x60328e98711a63d1, 0x75d4613f71300f8a, 0x7a912faf60f542f9 }
    },
    {
        { 0x253f4f8dfa2d5597, 0x25e49c405477130c, 0x00c052e5996b1102, 0x33cb966e33bb6c4a },
        { -0x4dfba7a1a123e5bd, -0x60f1e911a76838c4, 0x5b82c0ae4e70483c, 0x624a170e2bddf9be },
        { 0x597028047f116909, -0x7d753be3e1a9bb99, 0x70417dbde6217387, 0x721627aefbac4384 }
    },
    {
        { -0x02cf6843bef4d0de, -0x0e5fa2584a3057bc, 0x61289a1def57ca74, 0x245ea199bb821902 },
        { -0x682fc43c78c9522b, 0x2f1422afc532b130, 0x3aa68a057101bbc4, 0x4c946cf7e74f9fa7 },
        { -0x51235996872b8808, 0x1898ba3c29117fe1, -0x308c067c8df342a8, 0x67da12e6b8b56351 }
    },
    {
        { 0x2b7ef3d38ec8308c, -0x7d7028138e146b55, -0x7f83c4c93af9d543, 0x0cb64cb831a94141 },
        { 0x7067e187b4bd6e07, 0x6e8f0203c7d1fe74, -0x6c3955d0c737a5d0, 0x76297d1f3d75a78a },
        { 0x3030fc33534c6378, -0x469ca3a31abe179f, 0x15d9a9bed9b2c728, 0x49233ea3f3775dcb }
    },
    {
        { 0x7b3985fe1c9f249b, 0x4fd6b2d5a1233293, -0x314cba6be520b29e, 0x6987ff6f542de50c },
        { 0x629398fa8dbffc3a, -0x1ed01ad22ab24bab, -0x0c41ee20250dad6b, 0x628b140dce5e7b51 },
        { 0x47e241428f83753c, 0x6317bebc866af997, -0x2544a4bcc2e567d7, 0x074d8d245287fb2d }
    },
    {
        { 0x481875c6c0e31488, 0x219429b2e22034b4, 0x7223c98a31283b65, 0x3420d60b342277f9 },
        { -0x7cc82632bbf403cf, 0x729d2ca1af318fd7, -0x5fbf5b5b88d3df90, 0x46002ef03a7349be },
        { -0x055dc52150019a09, 0x78261ed45be0764c, 0x441c0a1e2f164403, 0x5aea8e567a87d395 }
    },
},
{
    {
        { 0x2dbc6fb6e4e0f177, 0x04e1bf29a4bd6a93, 0x5e1966d4787af6e8, 0x0edc5f5eb426d060 },
        { 0x7813c1a2bca4283d, -0x129d0f6e5e79c227, -0x513843473d97057a, 0x10e5d3b76f1cae4c },
        { 0x5453bfd653da8e67, -0x1623e113db5609bf, -0x4078d9c4fca875dd, 0x45b46c51361cba72 }
    },
    {
        { -0x3162b22275801c1c, -0x54ec9ba9899df1d0, 0x4b594f7bb30e9958, 0x5c1c0aef321229df },
        { -0x56bfd540ceb0805f, -0x1da80e2371730bb0, 0x1dbbd54b23a8be84, 0x2177bfa36dcb713b },
        { 0x37081bbcfa79db8f, 0x6048811ec25f59b3, 0x087a76659c832487, 0x4ae619387d8ab5bb }
    },
    {
        { 0x61117e44985bfb83, -0x031fb9d58e69ceca, -0x7c53cbb72bda6fb5, 0x75685abe5ba43d64 },
        { -0x72240955acbb5cd2, 0x7d88eab4b41b4078, 0x5eb0eb974a130d60, 0x1a00d91b17bf3e03 },
        { 0x6e960933eb61f2b2, 0x543d0fa8c9ff4952, -0x208d8aef85099a97, 0x135529b623b0e6aa }
    },
    {
        { -0x0a38e9431dd17c02, -0x4bd414e617f67a3f, -0x136259c8ebdab552, 0x5972ea051590a613 },
        { 0x18f0dbd7add1d518, -0x68608777303ee0ef, -0x78cd1e0f8eeb8a65, 0x79b5b81a65ca3a01 },
        { 0x0fd4ac20dc8f7811, -0x65652d6b53b2b058, -0x3fe4d29b4cc9fbcc, 0x4f7e9c95905f3bdb }
    },
    {
        { 0x71c8443d355299fe, -0x7432c4e324141529, -0x7f6db6610e5b6b9a, 0x1942eec4a144adc8 },
        { 0x62674bbc5781302e, -0x27adf0c6765223f1, -0x73d66651ac04263a, 0x31993ad92e638e4c },
        { 0x7dac5319ae234992, 0x2c1b3d910cea3e92, 0x553ce494253c1122, 0x2a0a65314ef9ca75 }
    },
    {
        { -0x30c9e532c3e386c6, 0x2f9ebcac5a35bc3b, 0x60e860e9a8cda6ab, 0x055dc39b6dea1a13 },
        { 0x2db7937ff7f927c2, -0x248be0f9e82f59cb, 0x5982f3a21155af76, 0x4cf6e218647c2ded },
        { -0x4ee6dd833d72a44a, 0x07e24ebc774dffab, -0x57c387311b5cd377, 0x121a307710aa24b6 }
    },
    {
        { -0x29a68ec1388b7c37, -0x77401f8847d46951, 0x289e28231097bcd3, 0x527bb94a6ced3a9b },
        { -0x1b24a2a160fcb569, -0x1eac03f6cfcb43d3, 0x460546919551d3b1, 0x333fc76c7a40e52d },
        { 0x563d992a995b482e, 0x3405d07c6e383801, 0x485035de2f64d8e5, 0x6b89069b20a7a9f7 }
    },
    {
        { 0x4082fa8cb5c7db77, 0x068686f8c734c155, 0x29e6c8d9f6e7a57e, 0x0473d308a7639bcf },
        { -0x7ed55fbe9d8fddf3, -0x66a5760506dba4b2, -0x00523b31af8d10fb, 0x23bc2103aa73eb73 },
        { -0x351186d9fca761fb, 0x2b4b421246dcc492, 0x02a1ef74e601a94f, 0x102f73bfde04341a }
    },
},
{
    {
        { 0x358ecba293a36247, -0x5070679d4d97029b, 0x412f7e9968a01c89, 0x5786f312cd754524 },
        { -0x4a5d2af3813df2c2, -0x39b422915f368d9d, 0x56e89052c1ff734d, 0x4929c6f72b2ffaba },
        { 0x337788ffca14032c, -0x0c6defd7bb80e11d, -0x74ebf8e0dce43353, 0x4c817b4bf2344783 }
    },
    {
        { 0x413ba057a40b4484, -0x45b3d1e5b0a095bd, 0x614ba0a5aee1d61c, 0x78a1531a8b05dc53 },
        { 0x0ff853852871b96e, -0x1ec160549f3c0e45, -0x1102a6acdacbbbfe, 0x0a37c37075b7744b },
        { 0x6cbdf1703ad0562b, -0x7130b7cf36dade5d, -0x25142cfc027bdb19, 0x72ad82a42e5ec56f }
    },
    {
        { -0x3c976c6e98fdb43d, -0x71962e92b6afd026, -0x030d13c31ba0b4d7, 0x065f669ea3b4cbc4 },
        { 0x3f9e8e35bafb65f6, 0x39d69ec8f27293a1, 0x6cb8cd958cf6a3d0, 0x1734778173adae6d },
        { -0x75ff5138aacd24b3, -0x47965b1bbc1ce44f, 0x4a0f8552d3a7f515, 0x19adeb7c303d7c08 }
    },
    {
        { -0x62fa4582bc3ce86c, 0x2470c8ff93322526, -0x7cdc2137e9e68bc8, 0x2852709881569b53 },
        { -0x38df349eac15265d, 0x55b2c97f512b636e, -0x4e1ca4a02bfd6f4f, 0x2fd9ccf13b530ee2 },
        { 0x07bd475b47f796b8, -0x2d384fecabd370ac, 0x2dbd23f43b24f87e, 0x6551afd77b0901d6 }
    },
    {
        { 0x68a24ce3a1d5c9ac, -0x44885cc2ef009b9f, 0x0f86ce4425d3166e, 0x56507c0950b9623b },
        { 0x4546baaf54aac27f, -0x090990134d5ba5d8, 0x582d1b5b562bcfe8, 0x44b123f3920f785f },
        { 0x1206f0b7d1713e63, 0x353fe3d915bafc74, 0x194ceb970ad9d94d, 0x62fadd7cf9d03ad3 }
    },
    {
        { 0x3cd7bc61e7ce4594, -0x3294ca564822d982, -0x5f7f5437bc9910d9, 0x6ec7c46f59c79711 },
        { -0x394a6984aa675f8c, 0x5efe91ce8e493e25, -0x2b48d3bab6d7f778, 0x20ef1149a26740c2 },
        { 0x2f07ad636f09a8a2, -0x79681931dbdfa183, -0x3f5103fa11ca5ec7, 0x15e80958b5f9d897 }
    },
    {
        { 0x4dd1ed355bb061c4, 0x42dc0cef941c0700, 0x61305dc1fd86340e, 0x56b2cc930e55a443 },
        { 0x25a5ef7d0c3e235b, 0x6c39c17fbe134ee7, -0x388b1ecbd23a3cd9, 0x021354b892021f39 },
        { 0x1df79da6a6bfc5a2, 0x02f3a2749fde4369, -0x4cdc260d325c6f59, 0x7be0847b8774d363 }
    },
    {
        { 0x1466f5af5307fa11, -0x7e8033821293f50e, 0x0a6de44ec3a4a3fb, 0x74071475bc927d0b },
        { -0x736633a574c0aa3d, 0x0611d7253fded2a0, -0x12d66a00c948f5ca, 0x1f699a54d78a2619 },
        { -0x188d6d0c8c181576, 0x296537d2cb045a31, 0x1bd0653ed3274fde, 0x2f9a2c4476bd2966 }
    },
},
{
    {
        { -0x5d4b251f4aaee366, 0x7ac860292bffff06, -0x67e0c8a20aafbdcc, 0x3f6bd725da4ea12d },
        { -0x14e7465480a8ba3a, 0x023a8aee5787c690, -0x48d8ed25d2085057, 0x36597d25ea5c013d },
        { 0x734d8d7b106058ac, -0x26bfa86190396fa1, 0x6466f8f99202932d, 0x7b7ecc19da60d6d0 }
    },
    {
        { 0x6dae4a51a77cfa9b, -0x7dd9c9ab185c79b0, 0x09bbffcd8f2d82db, 0x03bedc661bf5caba },
        { 0x78c2373c695c690d, -0x22dad199f9bd6f92, -0x6ae2bbbbb51ed42e, 0x4235ad7601743956 },
        { 0x6258cb0d078975f5, 0x492942549189f298, -0x5f354bdc1d1c911c, 0x0e7ce2b0cdf066a1 }
    },
    {
        { -0x0159012026b48f07, -0x0ecf3fae3e0345d3, 0x4882d47e7f2fab89, 0x615256138aeceeb5 },
        { -0x3b6b9bc53b737a5d, -0x02c9e20bc39ec653, 0x09db17dd3ae94d48, 0x666e0a5d8fb4674a },
        { 0x2abbf64e4870cb0d, -0x329a430f55ba7495, -0x6541b1458a1767a3, 0x7f0bc810d514dee4 }
    },
    {
        { -0x7c5362528c8dec60, -0x60090745d108d168, 0x311e2edd43ec6957, 0x1d3a907ddec5ab75 },
        { -0x46ff945bd90bec91, -0x7298c961a81fcfcb, -0x34372026b0b9c3d8, 0x0d1f8dbcf8eedbf5 },
        { -0x45e96ccec12f7e24, 0x29329fad851b3480, 0x0128013c030321cb, 0x00011b44a31bfde3 }
    },
    {
        { 0x16561f696a0aa75c, -0x3e408da3a7ad4296, 0x11a8dd7f9a7966ad, 0x63d988a2d2851026 },
        { 0x3fdfa06c3fc66c0c, 0x5d40e38e4dd60dd2, 0x7ae38b38268e4d71, 0x3ac48d916e8357e1 },
        { 0x00120753afbd232e, -0x16d431470227097d, -0x07e9964c7b18d46f, 0x33fad52b2368a066 }
    },
    {
        { -0x72d3372f3bdd3018, 0x072b4f7b05a13acb, -0x5c01491913095a91, 0x3cc355ccb90a71e2 },
        { 0x540649c6c5e41e16, 0x0af86430333f7735, -0x4d53032d0cfa18ba, 0x16c0f429a256dca7 },
        { -0x16496bbc6fc16ecf, -0x475b6b3485a9c832, -0x37832e5b45456dbc, 0x631eaf426bae7568 }
    },
    {
        { 0x47d975b9a3700de8, 0x7280c5fbe2f80552, 0x53658f2732e45de1, 0x431f2c7f665f80b5 },
        { -0x4c16fbef25990161, -0x7a22b4ad93e91a5a, -0x43c2689ee106407d, 0x5599648b1ea919b5 },
        { -0x29fd9cbb7a7084e7, 0x14ab352fa1ea514a, -0x76ffbbe5df6f5629, 0x7b04715f91253b26 }
    },
    {
        { -0x4c893d7f3b19453a, -0x68f12c2292e264f5, -0x4f656aa7baf406bc, 0x48d0acfa57cde223 },
        { -0x7c1242d7530951bd, -0x79ca837482a3854c, -0x3fbfb8964814d3bc, 0x59b37bf5c2f6583f },
        { -0x49f0d91b8254198f, -0x0e2e5e689dd0c5c9, 0x4208ce7ee9960394, 0x16234191336d3bdb }
    },
},
{
    {
        { -0x7ad22e02c2a87442, 0x2b65ce72c3286108, 0x658c07f4eace2273, 0x0933f804ec38ab40 },
        { -0x0e651538cc59c511, 0x2c7fba5d4442454e, 0x5da87aa04795e441, 0x413051e1a4e0b0f5 },
        { -0x5854968672b69b8a, -0x7ede5521034a5438, -0x5a23ed1084ac6b8e, 0x07fd47065e45351a }
    },
    {
        { 0x304211559ae8e7c3, -0x0d7e4dd66bb77d5b, -0x75ec53d1c87daf1c, 0x014afa0954ba48f4 },
        { -0x37a7c3c2da72d433, 0x17029a4daf60b73f, -0x05f03629be95c87f, 0x1c1e5fba38b3fb23 },
        { -0x34ce68ffe44c9994, 0x330060524bffecb9, 0x293711991a88233c, 0x291884363d4ed364 }
    },
    {
        { -0x0462c83c43e54915, 0x02be14534d57a240, -0x0b28cbea075a1e0a, 0x5964f4300ccc8188 },
        { 0x033c6805dc4babfa, 0x2c15bf5e5596ecc1, 0x1bc70624b59b1d3b, 0x3ede9850a19f0ec5 },
        { -0x1bb5dcead2f69800, 0x5c08c55970866996, -0x20d249f5b9500492, 0x579155c1f856fd89 }
    },
    {
        { -0x4a0e949cf7e8185a, -0x7f7396dcc3caefda, 0x324a983b54cef201, 0x53c092084a485345 },
        { -0x69cdb122ed1f3611, 0x468b878df2420297, 0x199a3776a4f573be, 0x1e7fbcf18e91e92a },
        { -0x2d2beb7e0e345041, 0x231d2db6716174e5, 0x0b7d7656e2a55c98, 0x3e955cd82aa495f6 }
    },
    {
        { -0x54c60c109e44c5c1, -0x714bff9ad146e6c2, -0x4a219133c73ee08c, 0x654d7e9626f3c49f },
        { -0x1b70aca1c12eabcd, -0x2f8a96d5f28d8f5d, 0x40fbd21daade6387, 0x14264887cf4495f5 },
        { -0x1a9b3022a382d315, -0x7d11502128c83347, 0x6107db62d1f9b0ab, 0x0b6baac3b4358dbb }
    },
    {
        { 0x204abad63700a93b, -0x41ffdc2c25886c8d, -0x27a0fcb99cc548f7, 0x00496dc490820412 },
        { 0x7ae62bcb8622fe98, 0x47762256ceb891af, 0x1a5a92bcf2e406b4, 0x7d29401784e41501 },
        { 0x1c74b88dc27e6360, 0x074854268d14850c, -0x5eba0484c1f234d0, 0x10843f1b43803b23 }
    },
    {
        { -0x2a9098d21cdb9765, -0x2e2575124c6b567f, -0x2284a7016e973013, 0x7ce246cd4d56c1e8 },
        { -0x3a06fbaac89d8923, -0x31a6ea72289ba327, -0x6d09a2aee2c994c7, 0x11574b6e526996c4 },
        { -0x470bcf71807f41ad, 0x5f3cb8cb34a9d397, 0x18a961bd33cc2b2c, 0x710045fb3a9af671 }
    },
    {
        { -0x5fc0379dfa629662, 0x2370cfa19a619e69, -0x3b01c4edd07dc215, 0x1d1b056fa7f0844e },
        { 0x73f93d36101b95eb, -0x0510cc86b090bb7a, 0x5651735f8f15e562, 0x7fa3f19058b40da1 },
        { 0x1bc64631e56bf61f, -0x2c8654ef91ac7d5d, 0x4d58c57e0540168d, 0x566256628442d8e4 }
    },
},
{
    {
        { -0x22b66329e00c79c0, 0x29cd9bc3063625a0, 0x51e2d8023dd73dc3, 0x4a25707a203b9231 },
        { -0x461b662109d9800a, 0x7772ca7b742c0843, 0x23a0153fe9a4f2b1, 0x2cdfdfecd5d05006 },
        { 0x2ab7668a53f6ed6a, 0x304242581dd170a1, 0x4000144c3ae20161, 0x5721896d248e49fc }
    },
    {
        { 0x285d5091a1d0da4e, 0x4baa6fa7b5fe3e08, 0x63e5177ce19393b3, 0x03c935afc4b030fd },
        { 0x0b6e5517fd181bae, -0x6fdd9d60d4469c4c, 0x5509bce932064625, 0x578edd74f63c13da },
        { -0x668d8939b6d4f3c3, 0x47ccc2c4dfe205fc, -0x232d647b229dc5c4, 0x3ec2ab590288c7a2 }
    },
    {
        { -0x58dec5f651cd2e35, 0x0f2b87df40f5c2d5, 0x0baea4c6e81eab29, 0x0e1bf66c6adbac5e },
        { -0x5e5f2d841b278447, -0x5674b2149ec6e513, -0x665f222f8c34647d, 0x2dd5c25a200fcace },
        { -0x1d542a1686d37782, 0x1a020018cb926d5d, -0x404596324551a0e2, 0x730548b35ae88f5f }
    },
    {
        { -0x7fa4f6b45e291ccc, -0x40c10e88f6cac0e7, 0x423f06cb0622702b, 0x585a2277d87845dd },
        { -0x3bcaae5c34574712, 0x65a26f1db2115f16, 0x760f4f52ab8c3850, 0x3043443b411db8ca },
        { -0x5e75a07dcc2b769e, 0x6698c4b5ec78257f, -0x5871905ac8c1be01, 0x7656278950ef981f }
    },
    {
        { -0x1e8f8c5c15793063, 0x3a8cfbb707155fdc, 0x4853e7fc31838a8e, 0x28bbf484b613f616 },
        { 0x38c3cf59d51fc8c0, -0x64122d02faf9490e, 0x26bf109fab570e8f, 0x3f4160a8c1b846a6 },
        { -0x0d9ed0a390ec9384, -0x50152ef80922ee42, 0x527e9ad213de6f33, 0x1e79cb358188f75d }
    },
    {
        { 0x77e953d8f5e08181, -0x7b5af3bbd6622127, -0x2393d2f379bada1b, 0x478ab52d39d1f2f4 },
        { 0x013436c3eef7e3f1, -0x7d7495800161ef08, 0x7ff908e5bcf9defc, 0x65d7951b3a3b3831 },
        { 0x66a6a4d39252d159, -0x1a221e4378e537f9, -0x47d394bf593e3691, 0x16d87a411a212214 }
    },
    {
        { -0x045b2a1d2ab1fa7d, -0x1de05028d1426606, 0x497ac2736ee9778f, 0x1f990b577a5a6dde },
        { -0x4c4281a5bdf99deb, -0x78641c32f3a5db3f, 0x57c05db1d6f994b7, 0x28f87c8165f38ca6 },
        { -0x5ccbb152e417082a, 0x7d1e50ebacea798f, 0x77c6569e520de052, 0x45882fe1534d6d3e }
    },
    {
        { -0x275366d66bc3901c, -0x4a060e9e5c7c6d5e, 0x2699db13bec89af3, 0x7dcf843ce405f074 },
        { 0x6669345d757983d6, 0x62b6ed1117aa11a6, 0x7ddd1857985e128f, 0x688fe5b8f626f6dd },
        { 0x6c90d6484a4732c0, -0x2adebc0235a9cd67, -0x4c41d73c6ea2391f, 0x6739687e7327191b }
    },
},
{
    {
        { -0x731a552f363468e1, 0x1156aaa99fd54a29, 0x41f7247015af9b78, 0x1fe8cca8420f49aa },
        { -0x609a3a15dff7eb31, -0x7bfac91e965ce8c0, -0x74f12ec6da374b53, 0x0080dbafe936361d },
        { 0x72a1848f3c0cc82a, 0x38c560c2877c9e54, 0x5004e228ce554140, 0x042418a103429d71 }
    },
    {
        { 0x58e84c6f20816247, -0x724d4d491c90286d, -0x688e7da9e2b7b27b, 0x0822024f8632abd7 },
        { -0x766215ae540c00a1, -0x646c5798d03d2746, 0x2c38cb97be6ebd5c, 0x114d578497263b5d },
        { -0x4cfe448394e4135d, 0x55393f6dc6eb1375, -0x6ef2d7ef68491b15, 0x1ad4548d9d479ea3 }
    },
    {
        { -0x5f901992f016012d, -0x578cc5bfe3a786f7, 0x30d14d800df98953, 0x41ce5876c7b30258 },
        { -0x32a5825fc765b703, -0x4c705b556587c8e2, -0x392689e4d3247194, 0x35cf51dbc97e1443 },
        { 0x59ac3bc5d670c022, -0x151983ef64ee6bfa, -0x6867420f4c87d026, 0x651e3201fd074092 }
    },
    {
        { -0x5a845b5fe1035162, 0x769f4beedc308a94, -0x2e0ef114c9fc34d2, 0x4099ce5e7e441278 },
        { -0x29c27b7c10cf3a31, 0x4cd4b4962361cc0c, -0x116f1aff5b7bd954, 0x0af51d7d18c14eeb },
        { 0x1ac98e4f8a5121e9, 0x7dae9544dbfa2fe0, -0x7cdf55f229bcf207, 0x667282652c4a2fb5 }
    },
    {
        { -0x5257491fd6b924dd, 0x1c0ce51a7b253ab7, -0x7bb737a59922b7a5, 0x7f1fc025d0675adf },
        { -0x78b9de0b27943655, -0x4ab38441a9019016, 0x077a24257fadc22c, 0x1ab53be419b90d39 },
        { -0x2711e4e7ce615956, 0x004d88083a21f0da, 0x3bd6aa1d883a4f4b, 0x4db9a3a6dfd9fd14 }
    },
    {
        { -0x26a4ff4434488398, -0x22437b956e0e87b7, 0x7cf700aebe28d9b3, 0x5ce1285c85d31f3e },
        { -0x73184dc44663f8ab, 0x35c5d6edc4f50f7a, 0x7e1e2ed2ed9b50c3, 0x36305f16e8934da1 },
        { 0x31b6972d98b0bde8, 0x7d920706aca6de5b, -0x198cef076f759a61, 0x50fac2a6efdf0235 }
    },
    {
        { 0x295b1c86f6f449bc, 0x51b2e84a1f0ab4dd, -0x3ffe34cf5571aae3, 0x6a28d35944f43662 },
        { -0x0c2c560ca477f0a6, -0x1213faf324fc183e, -0x576967e0060f4e5e, 0x49a4ae2bac5e34a4 },
        { 0x28bb12ee04a740e0, 0x14313bbd9bce8174, 0x72f5b5e4e8c10c40, 0x7cbfb19936adcd5b }
    },
    {
        { -0x7186c58533c91920, -0x0605485c82a79113, 0x3a4f9692bae1f4e4, 0x1c14b03eff5f447e },
        { -0x5cee223d947686d3, 0x1b30b4c6da512664, 0x0ca77b4ccf150859, 0x1de443df1b009408 },
        { 0x19647bd114a85291, 0x57b76cb21034d3af, 0x6329db440f9d6dfa, 0x5ef43e586a571493 }
    },
},
{
    {
        { -0x5992336237f3e540, -0x685fa30be4c75bca, -0x58140c416a24283a, 0x7da0b8f68d7e7dab },
        { -0x1087dfebc7a98a5a, -0x5d9b60cf55025618, 0x4cd1eb505cdfa8cb, 0x46115aba1d4dc0b3 },
        { -0x2bf0e6ac3c4a258a, 0x1dac6f7321119e9b, 0x03cc6021feb25960, 0x5a5f887e83674b4b }
    },
    {
        { -0x6169d72c5f59bc47, -0x4a3c34ff193cdf9c, -0x64acfd7683d213ce, 0x43e37ae2d5d1c70c },
        { -0x709cfe308f5ec2ef, -0x303147eacaf22f3c, -0x08fd682b5b435b82, 0x3669b656e44d1434 },
        { 0x387e3f06eda6e133, 0x67301d5199a13ac0, -0x42a52707c9d9c7ef, 0x6a21e6cd4fd5e9be }
    },
    {
        { -0x10bed6ed99664d1d, 0x71d30847708d1301, 0x325432d01182b0bd, 0x45371b07001e8b36 },
        { -0x0e39e8f5cfb919a1, 0x58712a2a00d23524, 0x69dbbd3c8c82b755, 0x586bf9f1a195ff57 },
        { -0x5924f772a10786f5, 0x5278f0dc610937e5, -0x53fcb62d9e5e9148, 0x0eafb03790e52179 }
    },
    {
        { 0x5140805e0f75ae1d, -0x13fd041cd99d33d0, 0x2cebdf1eea92396d, 0x44ae3344c5435bb3 },
        { -0x69faaa3ec8b7fbd1, 0x219a41e6820baa11, 0x1c81f73873486d0c, 0x309acc675a02c661 },
        { -0x630d7646445abc12, -0x0c89f162a5368ebe, 0x1d82e5c64f9360aa, 0x62d5221b7f94678f }
    },
    {
        { 0x7585d4263af77a3c, -0x205184ee0116ebb3, -0x5af98f7fa608e6c3, 0x14f29a5383922037 },
        { 0x524c299c18d0936d, -0x37944a9375f3e5f4, -0x5c8afad124b579cf, 0x5c0efde4bc754562 },
        { -0x208e8123da4d280b, 0x21f970db99b53040, -0x256dcb483c12b39e, 0x5e72365c7bee093e }
    },
    {
        { 0x7d9339062f08b33e, 0x5b9659e5df9f32be, -0x5300c252e0614203, 0x70b20555cb7349b7 },
        { 0x575bfc074571217f, 0x3779675d0694d95b, -0x65f5c8440be6e1cd, 0x77f1104c47b4eabc },
        { -0x41aeec3aaaeed3b4, 0x6688423a9a881fcd, 0x446677855e503b47, 0x0e34398f4a06404a }
    },
    {
        { 0x18930b093e4b1928, 0x7de3e10e73f3f640, -0x0bcde8258cc6a291, 0x6f8aded6ca379c3e },
        { -0x4982dd26c1314218, 0x09b3e84127822f07, 0x743fa61fb05b6d8d, 0x5e5405368a362372 },
        { -0x1cbfedc202484d66, 0x487b97e1a21ab291, -0x066982fd02196b62, 0x780de72ec8d3de97 }
    },
    {
        { 0x671feaf300f42772, -0x708d14d5d573be56, 0x29a17fd797373292, 0x1defc6ad32b587a6 },
        { 0x0ae28545089ae7bc, 0x388ddecf1c7f4d06, 0x38ac15510a4811b8, 0x0eb28bf671928ce4 },
        { -0x50a441e510ae6a59, 0x148c1277917b15ed, 0x2991f7fb7ae5da2e, 0x467d201bf8dd2867 }
    },
},
{
    {
        { 0x745f9d56296bc318, -0x66ca7f2b27ead19b, -0x4f1a4ec0a7c61632, 0x51fc2b28d43921c0 },
        { 0x7906ee72f7bd2e6b, 0x05d270d6109abf4e, -0x72a301ba46be575c, 0x44c218671c974287 },
        { 0x1b8fd11795e2a98c, 0x1c4e5ee12b6b6291, 0x5b30e7107424b572, 0x6e6b9de84c4f4ac6 }
    },
    {
        { 0x6b7c5f10f80cb088, 0x736b54dc56e42151, -0x3d49df5a3910663c, 0x5f4c802cc3a06f42 },
        { -0x200da031b4e21eaf, -0x27be3f381ee3bfdb, 0x2554b3c854749c87, 0x2d292459908e0df9 },
        { -0x649a370e82f8ad26, -0x77e31cc738811800, -0x3c4aeb0fa49d061d, 0x66ed5dd5bec10d48 }
    },
    {
        { -0x0f520c363435fb83, -0x7e3c4d340baad095, -0x3025eed2bb8ca06d, 0x1f23a0c77e20048c },
        { 0x7d38a1c20bb2089d, -0x7f7ccb1e69332bee, -0x3b58f47393682ced, 0x2eacf8bc03007f20 },
        { -0x0dcab9841a43ea90, 0x03d2d9020dbab38c, 0x27529aa2fcf9e09e, 0x0840bef29d34bc50 }
    },
    {
        { -0x32ab1f9480c81b15, -0x733ea0780a169336, -0x47db744f2ca68232, 0x246affa06074400c },
        { 0x796dfb35dc10b287, 0x27176bcd5c7ff29d, 0x7f3d43e8c7b24905, 0x0304f5a191c54276 },
        { 0x37d88e68fbe45321, -0x79f68ab73f28afce, 0x4e9b13ef894a0d35, 0x25a83cac5753d325 }
    },
    {
        { -0x60f099d6c6ad491e, 0x33db5e0e0934267b, -0x00badad429f60124, 0x06be10f5c506e0c9 },
        { 0x10222f48eed8165e, 0x623fc1234b8bcf3a, 0x1e145c09c221e8f0, 0x7ccfa59fca782630 },
        { 0x1a9615a9b62a345f, 0x22050c564a52fecc, -0x585d877ad743f202, 0x5e82770a1a1ee71d }
    },
    {
        { -0x17fd17f5bdcc638c, 0x34175166a7fffae5, 0x34865d1f1c408cae, 0x2cca982c605bc5ee },
        { 0x35425183ad896a5c, -0x1798c5041872ad0a, 0x2c66f25f92a35f64, 0x09d04f3b3b86b102 },
        { -0x02d2a2cae6824192, 0x207c2eea8be4ffa3, 0x2613d8db325ae918, 0x7a325d1727741d3e }
    },
    {
        { -0x132d82fe81d5f896, -0x28779760e9c9b6a2, 0x52a61af0919233e5, 0x2a479df17bb1ae64 },
        { -0x2fc946442e92021e, -0x5dfaa8a83b6857d7, -0x71933699580ed999, 0x4d3b1a791239c180 },
        { -0x61a11171cc24d8f0, 0x189854ded6c43ca5, -0x5be3dd3a6d8e7ec8, 0x27ad5538a43a5e9b }
    },
    {
        { -0x34a5829c71b8f884, -0x7248ac9edf5e3fa7, 0x549e1e4d8bedfdcc, 0x080153b7503b179d },
        { 0x2746dd4b15350d61, -0x2fc03437116ade49, -0x1791c9a5ec798d36, 0x510e987f7e7d89e2 },
        { -0x2259626cf5c12c1d, 0x3d386ef1cd60a722, -0x37e852a74255b11a, 0x23be8d554fe7372a }
    },
},
{
    {
        { -0x43e10b42a9851857, 0x3f624cb2d64498bd, -0x1bef9b2dd3e0b138, 0x2ef9c5a5ba384001 },
        { -0x6a016e658b10b053, 0x3a827becf6a308a2, -0x69b1fe2cf65b84ff, 0x71c43c4f5ba3c797 },
        { -0x4902920905618b33, -0x0e7d87431b50d986, -0x7daa4c2f0e1066f2, 0x5a758ca390c5f293 }
    },
    {
        { -0x731f6e74e29e236c, -0x7212c9b9657ecf9a, -0x2b1957d65017552d, 0x0a738027f639d43f },
        { -0x5d48d8ef26b9db6b, 0x3aa8c6d2d57d5003, -0x1c2bff405f4b7836, 0x2dbae244b3eb72ec },
        { -0x67f0b5d0a8001e34, 0x00670d0de1839843, 0x105c3f4a49fb15fd, 0x2698ca635126a69c }
    },
    {
        { 0x2e3d702f5e3dd90e, -0x61c0f6e71b2dac7a, 0x5e773ef6024da96a, 0x3c004b0c4afa3332 },
        { -0x189ace77cd4f4588, 0x381831f7925cff8b, 0x08a81b91a0291fcc, 0x1fb43dcc49caeb07 },
        { -0x6556b953f90b47d5, 0x1ca284a5a806c4f3, 0x3ed3265fc6cd4787, 0x6b43fd01cd1fd217 }
    },
    {
        { -0x4a38bda7c189f10d, 0x75dc52b9ee0ab990, -0x40ebd83df8d46dc1, 0x73420b2d6ff0d9f0 },
        { -0x3858a2b4b9683abc, 0x15fdf848df0fffbf, 0x2868b9ebaa46785a, 0x5a68d7105b52f714 },
        { -0x50d30934617ae1fa, -0x70a6c6ec39ddc73c, -0x2575476966040c8d, 0x3db5632fea34bc9e }
    },
    {
        { 0x2e4990b1829825d5, -0x12151478c165766f, -0x110fc2c6b38fb508, 0x59197ea495df2b0e },
        { -0x0b9111d408a22628, 0x0d17b1f6396759a5, 0x1bf2d131499e7273, 0x04321adf49d75f13 },
        { 0x04e16019e4e55aae, -0x1884bc8581d06d17, -0x3831d23e90ea655c, 0x45eafdc1f4d70cc0 }
    },
    {
        { -0x49f1b9db30334e13, 0x59dbc292bd5c0395, 0x31a09d1ddc0481c9, 0x3f73ceea5d56d940 },
        { 0x698401858045d72b, 0x4c22faa2cf2f0651, -0x6be5c99a94ddd23a, 0x5a5eebc80362dade },
        { -0x4858402ef5b1723a, -0x41a8ff81bb364cc7, 0x60c1207f1557aefa, 0x26058891266218db }
    },
    {
        { 0x4c818e3cc676e542, 0x5e422c9303ceccad, -0x13f833354bed60f8, 0x0dedfa10b24443b8 },
        { 0x59f704a68360ff04, -0x3c26c021899e190c, -0x7ce4d58ced78caaf, 0x54ad0c2e4e615d57 },
        { -0x11c4982a47d4add6, 0x36f163469fa5c1eb, -0x5a4b2d0d913e602d, 0x62ecb2baa77a9408 }
    },
    {
        { -0x6df8d7c95049d78c, 0x5fcd5e8579e104a5, 0x5aad01adc630a14a, 0x61913d5075663f98 },
        { -0x1a1286ad9eead4c3, 0x4962357d0eddd7d1, 0x7482c8d0b96b4c71, 0x2e59f919a966d8be },
        { 0x0dc62d361a3231da, -0x05b8a7cd6bdffd90, 0x02d801513f9594ce, 0x3ddbc2a131c05d5c }
    },
},
{
    {
        { -0x048ca53dffb5ca2f, 0x31de0f433a6607c3, 0x7b8591bfc528d599, 0x55be9a25f5bb050c },
        { 0x3f50a50a4ffb81ef, -0x4e1fcaf6c40bdf41, -0x645571e33955d330, 0x32239861fa237a40 },
        { 0x0d005acd33db3dbf, 0x0111b37c80ac35e2, 0x4892d66c6f88ebeb, 0x770eadb16508fbcd }
    },
    {
        { -0x0e2c497e5faf8e47, 0x2207659a3592ff3a, 0x5f0169297881e40e, 0x16bedd0e86ba374e },
        { -0x7bae061fa1b17623, -0x3f9cfd004386c6c9, 0x5d22749556a6495c, 0x09a6755ca05603fb },
        { 0x5ecccc4f2c2737b5, 0x43b79e0c2dccb703, 0x33e008bc4ec43df3, 0x06c1b840f07566c0 }
    },
    {
        { 0x69ee9e7f9b02805c, -0x34007d75ab82e9c0, 0x3d93a869b2430968, 0x46b7b8cd3fe26972 },
        { 0x7688a5c6a388f877, 0x02a96c14deb2b6ac, 0x64c9f3431b8c2af8, 0x3628435554a1eed6 },
        { -0x167edf7901811420, 0x4cba6be72f515437, 0x1d04168b516efae9, 0x5ea1391043982cb9 }
    },
    {
        { 0x6f2b3be4d5d3b002, -0x5013cc2695f63780, 0x035f73a4a8bcc4cc, 0x22c5b9284662198b },
        { 0x49125c9cf4702ee1, 0x4520b71f8b25b32d, 0x33193026501fef7e, 0x656d8997c8d2eb2b },
        { -0x34a73701bcc276c7, -0x765f34d1957281b0, 0x79ca955309fbbe5a, 0x0c626616cd7fc106 }
    },
    {
        { -0x70203c86040bab4f, 0x45a5a970f1a4b771, -0x536de108452ca6eb, 0x42d088dca81c2192 },
        { 0x1ffeb80a4879b61f, 0x6396726e4ada21ed, 0x33c7b093368025ba, 0x471aa0c6f3c31788 },
        { -0x7025f0c85fe9ae67, 0x0adadb77c8a0e343, 0x20fbfdfcc875e820, 0x1cf2bea80c2206e7 }
    },
    {
        { -0x67d291e5fd3fbed1, -0x6f05b37c24a71702, 0x01c2f5bcdcb18bc0, 0x686e0c90216abc66 },
        { -0x3d220e214c9dfd54, -0x6d5a01f62d1d855b, 0x7d1648f6fc09f1d3, 0x74c2cc0513bc4959 },
        { 0x1fadbadba54395a7, -0x4be5fd5f51f25996, -0x40e60a67445c83f9, 0x6a12b8acde48430d }
    },
    {
        { 0x793bdd801aaeeb5f, 0x00a2a0aac1518871, -0x175c8c5ce0dec94c, 0x48aab888fc91ef19 },
        { -0x072515e0c62b6a27, 0x592c190e525f1dfc, -0x247342fb3666e2e5, 0x11f7fda3d88f0cb7 },
        { 0x041f7e925830f40e, 0x002d6ca979661c06, -0x79236006d4fb95d2, 0x760360928b0493d1 }
    },
    {
        { -0x4bcef71a96a5f4fb, 0x6cb00ee8ad37a38b, 0x5edad6eea3537381, 0x3f2602d4b6dc3224 },
        { 0x21bb41c6120cf9c6, -0x154d55ed21325a65, -0x3e58d2fdf55b74cc, 0x215d4d27e87d3b68 },
        { -0x374db849a4350e64, 0x49779dc3b1b2c652, -0x765e7f442a131d1e, 0x13f098a3cec8e039 }
    },
},
{
    {
        { -0x0c55a85dd86944ec, -0x77c5454864f825df, -0x1ab41de7ce5fc6e4, 0x5ee7fb38d83205f9 },
        { -0x6523f00631a13ab5, 0x039c2a6b8c2f130d, 0x028007c7f0f89515, 0x78968314ac04b36b },
        { 0x538dfdcb41446a8e, -0x5a530256bcb6c807, 0x46af908d263c8c78, 0x61d0633c9bca0d09 }
    },
    {
        { -0x525cd74307038c21, -0x117b96a2590fc804, 0x637fb4db38c2a909, 0x5b23ac2df8067bdc },
        { 0x63744935ffdb2566, -0x3a42947687f49745, 0x6f1b3280553eec03, 0x6e965fd847aed7f5 },
        { -0x652d46ac117fad85, -0x1770e65505219273, 0x0e711704150e82cf, 0x79b9bbb9dd95dedc }
    },
    {
        { -0x2e66825171608c8c, -0x5fcd5d073044f7ea, -0x329345ed92bba0f6, 0x1ba811460accb834 },
        { -0x144caabf95ced93e, -0x2d9c7c5797373c6d, 0x6c0c6429e5b97a82, 0x5065f158c9fd2147 },
        { 0x708169fb0c429954, -0x1eb9ff5328913099, 0x2eaab98a70e645ba, 0x3981f39e58a4faf2 }
    },
    {
        { -0x37ba205a92199022, -0x1ead5affd3bfb7c6, -0x162d1e9c384b09ce, 0x30f4452edcbc1b65 },
        { 0x18fb8a7559230a93, 0x1d168f6960e6f45d, 0x3a85a94514a93cb5, 0x38dc083705acd0fd },
        { -0x7a92d87d3a8a68c0, -0x05ecba9606634134, -0x77bb038c3f15b18f, 0x632d9a1a593f2469 }
    },
    {
        { -0x40f602ee12f37b59, 0x63f071810d9f693a, 0x21908c2d57cf8779, 0x3a5a7df28af64ba2 },
        { -0x094494ea47f8345a, 0x1823c7dfbc54f0d7, -0x44e268fc91d698f5, 0x0b24f48847ed4a57 },
        { -0x23252b41aee41539, -0x5bac7f8a12d9330e, -0x1e630060ffa0659b, 0x34fcf74475481f63 }
    },
    {
        { -0x5a44e25487305568, 0x5ceda267190b72f2, -0x6cf636eef56d9f72, 0x0119a3042fb374b0 },
        { -0x3e681fb387689836, -0x478eb234c726b983, 0x55de888283f95fa8, 0x3d3bdc164dfa63f7 },
        { 0x67a2d89ce8c2177d, 0x669da5f66895d0c1, -0x0a9a671a4d7d5d50, 0x56c088f1ede20a73 }
    },
    {
        { 0x581b5fac24f38f02, -0x56f41601451cf343, -0x65de96fd75306d10, 0x038b7ea48359038f },
        { 0x336d3d1110a86e17, -0x280c77cdf48a4d06, -0x06eacc89daf8d678, 0x09674c6b99108b87 },
        { -0x60b107de66ce9008, 0x2f49d282eaa78d4f, 0x0971a5ab5aef3174, 0x6e5e31025969eb65 }
    },
    {
        { 0x3304fb0e63066222, -0x04caf976785345c1, -0x42e6db8873ef9e5d, 0x3058ad43d1838620 },
        { -0x4e939d0a781a6c05, 0x4999eddeca5d3e71, -0x4b6e3e1feb33c193, 0x08f5114789a8dba8 },
        { 0x323c0ffde57663d0, 0x05c3df38a22ea610, -0x423875425366b066, 0x26549fa4efe3dc99 }
    },
},
{
    {
        { 0x04dbbc17f75396b9, 0x69e6a2d7d2f86746, -0x39bf62660ac1543a, 0x606175f6332e25d2 },
        { 0x738b38d787ce8f89, -0x49d9a71dbe865773, 0x30738c9cf151316d, 0x49128c7f727275c9 },
        { 0x4021370ef540e7dd, 0x0910d6f5a1f1d0a5, 0x4634aacd5b06b807, 0x6a39e6356944f235 }
    },
    {
        { 0x1da1965774049e9d, -0x0432915e6701cad5, -0x4e3432af33adc95a, 0x1f5ec83d3f9846e2 },
        { -0x6932a9bf206f0c19, 0x6c3a760edbfa25ea, 0x24f3ef0959e33cc4, 0x42889e7e530d2e58 },
        { -0x7104dc3ccd73348b, -0x50bd5df822789117, 0x20fbdadc5dfae796, 0x241e246b06bf9f51 }
    },
    {
        { 0x7eaafc9a6280bbb8, 0x22a70f12f403d809, 0x31ce40bb1bfc8d20, 0x2bc65635e8bd53ee },
        { 0x29e68e57ad6e98f6, 0x4c9260c80b462065, 0x3f00862ea51ebb4b, 0x5bc2c77fb38d9097 },
        { -0x172a23605694526d, -0x1a704e8221e6b824, 0x681532ea65185fa3, 0x1fdd6c3b034a7830 }
    },
    {
        { -0x63ec595ad2270857, 0x2dbb1f8c3efdcabf, -0x69e1cdbfa1f7084b, 0x48c8a121bbe6c9e5 },
        { 0x0a64e28c55dc18fe, -0x1c206166cc661423, 0x79ac432370e2e652, 0x35ff7fc33ae4cc0e },
        { -0x03bea583a69b9bbb, -0x2ddb4d283ed749eb, 0x6035c9c905fbb912, 0x42d7a91274429fab }
    },
    {
        { -0x565b76b86cc25a44, 0x4a58920ec2e979ec, -0x69277fffec1a53b4, 0x453692d74b48b147 },
        { 0x4e6213e3eaf72ed3, 0x6794981a43acd4e7, -0x00ab8321914af735, 0x6fed19dd10fcb532 },
        { -0x2288a26657aa6391, -0x0bd5debf20ffc1dc, 0x5223e229da928a66, 0x063f46ba6d38f22c }
    },
    {
        { 0x39843cb737346921, -0x58b804f8c7376bb9, -0x34727fce5dbacf82, 0x67810f8e6d82f068 },
        { -0x2d2dbd76a0ac996c, -0x35cc5d3abd6c64d4, -0x67905259382246a4, 0x5a152c042f712d5d },
        { 0x3eeb8fbcd2287db4, 0x72c7d3a301a03e93, 0x5473e88cbd98265a, 0x7324aa515921b403 }
    },
    {
        { -0x52dc092517dcab35, 0x6962502ab6571a6d, -0x649ae9c91c71c82f, 0x5cac5005d1a3312f },
        { -0x7a86bd0b93c34172, -0x5e2c9b4eb8cf3fba, 0x1c8ed914d23c41bf, 0x0838e161eef6d5d2 },
        { -0x733eab33161c66fc, 0x5b3a040b84de6846, -0x3b2759e34e41a292, 0x40fb897bd8861f02 }
    },
    {
        { -0x1a8127b8a54ef89f, 0x71435e206fd13746, 0x342f824ecd025632, 0x4b16281ea8791e7b },
        { -0x7b3a556f9d21c85f, 0x421da5000d1d96e1, 0x788286306a9242d9, 0x3c5e464a690d10da },
        { -0x2e3efe2af47ecc7f, -0x2119f0ee891197d8, 0x0cb68893383f6409, 0x6183c565f6ff484a }
    },
},
{
    {
        { -0x24b97ab650c09992, -0x288030fb0eb5f15b, 0x3df23ff7a4ba0c47, 0x3a10dfe132ce3c85 },
        { 0x741d5a461e6bf9d6, 0x2305b3fc7777a581, -0x2baa8b5d9b8b2c27, 0x1926e1dc6401e0ff },
        { -0x1f80b17515e83160, 0x2fd515463a1fc1fd, 0x175322fd31f2c0f1, 0x1fa1d01d861e5d15 }
    },
    {
        { 0x38dcac00d1df94ab, 0x2e712bddd1080de9, 0x7f13e93efdd5e262, 0x73fced18ee9a01e5 },
        { -0x337faa6b82a667ce, 0x1e4656da37f15520, -0x6609088bb1fa6ce0, 0x773563bc6a75cf33 },
        { 0x06b1e90863139cb3, -0x5b6c25983a5fc133, -0x72883137529c76ce, 0x1f426b701b864f44 }
    },
    {
        { -0x0e81ca376e5edaae, -0x48947eaca8a1638a, -0x057cbf90f2648dc2, 0x0b76bb1b3fa7e438 },
        { -0x1036d9b3be6ee3ff, -0x0e5c4847e85dd3db, 0x5875da6bf30f1447, 0x4e1af5271d31b090 },
        { 0x08b8c1f97f92939b, -0x41988e342bbb5492, 0x22e5646399bb8017, 0x7b6dd61eb772a955 }
    },
    {
        { 0x5730abf9ab01d2c7, 0x16fb76dc40143b18, -0x7993419a5f344d7f, 0x53fa9b659bff6afe },
        { -0x48523e17af0cc26e, 0x7998fa4f608cd5cf, -0x5269d2427203a425, 0x703e9bceaf1d2f4f },
        { 0x6c14c8e994885455, -0x7bc5a2999a512b1b, 0x181bb73ebcd65af1, 0x398d93e5c4c61f50 }
    },
    {
        { -0x3c78839f2d181c0e, 0x3b34aaa030828bb1, 0x283e26e7739ef138, 0x699c9c9002c30577 },
        { 0x1c4bd16733e248f3, -0x4261ed78ea40f5a1, -0x2bc0730f5ef4fc8a, 0x53b09b5ddf191b13 },
        { -0x0cf958dca6b90e34, -0x6de8e74a331a2683, 0x28cdd24781b4e975, 0x51caf30c6fcdd907 }
    },
    {
        { 0x737af99a18ac54c7, -0x6fcc87233ae34cf1, 0x2b89bc334ce10cc7, 0x12ae29c189f8e99a },
        { -0x59f458bd898b1ff6, 0x630e8570a17a7bf3, 0x3758563dcf3324cc, 0x5504aa292383fdaa },
        { -0x56613f34e0f2fe31, 0x0dd1efcc3a34f7ae, 0x55ca7521d09c4e22, 0x5fd14fe958eba5ea }
    },
    {
        { 0x3c42fe5ebf93cb8e, -0x412057aec92ba9a1, -0x1f0f7a6177bddf18, 0x7dd73f960725d128 },
        { -0x4a23d220d7ba54d4, 0x069491b10a7fe993, 0x4daaf3d64002e346, 0x093ff26e586474d1 },
        { -0x4ef2db0197fa67d7, 0x75730672dbaf23e5, 0x1367253ab457ac29, 0x2f59bcbc86b470a4 }
    },
    {
        { 0x7041d560b691c301, -0x7adfe4c0522818e2, 0x16c2e16311335585, 0x2aa55e3d010828b1 },
        { -0x7c7b82bd66e8eca1, -0x52e46ee0a982fc29, 0x7e7748d9be77aad1, 0x5458b42e2e51af4a },
        { -0x12ae6d19f3f8bbb1, 0x42c54e2d74421d10, 0x352b4c82fdb5c864, 0x13e9004a8a768664 }
    },
},
{
    {
        { 0x1e6284c5806b467c, -0x3a09668418a29f85, -0x749826a74c872d9e, 0x3d88d66a81cd8b70 },
        { -0x344a4aaa93fcd401, -0x208e6e48d6d685c6, -0x3e008cd952127e45, 0x71ade8bb68be03f5 },
        { -0x7489856cdfb12877, 0x762fcacb9fa0ae2a, 0x771febcc6dce4887, 0x343062158ff05fb3 }
    },
    {
        { -0x031de6f8d584ce4c, 0x4d7adc75aa578016, 0x0ec276a687479324, 0x6d6d9d5d1fda4beb },
        { -0x1fa25e581e0a40b7, 0x26457d6dd4736092, 0x77dcb07773cc32f6, 0x0a5d94969cdd5fcd },
        { 0x22b1a58ae9b08183, -0x026a2f8e3ea3c775, -0x567edc897af5fae9, 0x33384cbabb7f335e }
    },
    {
        { 0x33bc627a26218b8d, -0x157f4de03857f39f, -0x6ba74ed4e8c1611a, 0x076247be0e2f3059 },
        { 0x3c6fa2680ca2c7b5, 0x1b5082046fb64fda, -0x14accb63abce2922, 0x5278b38f6b879c89 },
        { 0x52e105f61416375a, -0x136850c97a54145c, 0x26e6b50623a67c36, 0x5cf0e856f3d4fb01 }
    },
    {
        { -0x415131cec24cbd58, -0x345c9ca47bd24812, -0x177399df7e80ec11, 0x1b9438aa4e76d5c6 },
        { -0x0936978ce517354c, 0x5e20741ecb4f92c5, 0x2da53be58ccdbc3e, 0x2dddfea269970df7 },
        { -0x75af8881e990fce6, 0x067b39f10fb7a328, 0x1925c9a6010fbd76, 0x6df9b575cc740905 }
    },
    {
        { -0x13203ca4b73521bf, 0x6a88471fb2328270, 0x740a4a2440a01b6a, 0x471e5796003b5f29 },
        { 0x42c1192927f6bdcf, -0x706e6e85bfc29e36, -0x23e3a5997461e09f, 0x1596047804ec0f8d },
        { -0x2569444c5312c854, 0x7a2423b5e9208cea, 0x24cc5c3038aebae2, 0x50c356afdc5dae2f }
    },
    {
        { -0x30126320e4ce469c, -0x0b79567a735ae50d, 0x14897265ea8c1f84, 0x784a53dd932acc00 },
        { 0x09dcbf4341c30318, -0x1145f9ee7ce7e232, -0x3e863f3123e1d65f, 0x1dbf7b89073f35b0 },
        { 0x2d99f9df14fc4920, 0x76ccb60cc4499fe5, -0x5becd3441a30fffd, 0x3f93d82354f000ea }
    },
    {
        { -0x1553ed2e861eb688, -0x006dc00c441400a2, 0x4af663e40663ce27, 0x0fd381a811a5f5ff },
        { -0x7e7c189761fb317b, 0x678fb71e04465341, -0x526dfa7099771254, 0x5da350d3532b099a },
        { -0x0da953135bc920ac, 0x108b6168ae69d6e8, 0x20d986cb6b5d036c, 0x655957b9fee2af50 }
    },
    {
        { -0x423ebf642ffd2f54, 0x66660245b5ccd9a6, -0x7dce823b05217a14, 0x02fe934b6ad7df0d },
        { -0x51574f8056fdfcf1, -0x077389950b9c2ebd, 0x15b083663c787a60, 0x08eab1148267a4a8 },
        { -0x10a30eff3048158c, 0x22897633a1cb42ac, -0x2b31f3ab310d7a1e, 0x30408c048a146a55 }
    },
},
{
    {
        { -0x44d1ff36e6c47881, -0x131c576f1f23af95, -0x130c483fc9219b61, 0x5f46040898de9e1a },
        { 0x739d8845832fcedb, -0x05c729365194079d, 0x32bc0dcab74ffef7, 0x73937e8814bce45e },
        { -0x46fc8ee9d6840b73, -0x562ec4dd2b0f97cc, -0x1e68eaa8b969423a, 0x2cf8a4e891d5e835 }
    },
    {
        { 0x2cb5487e17d06ba2, 0x24d2381c3950196b, -0x289a637e7a6875d0, 0x7a6f7f2891d6a4f6 },
        { 0x6d93fd8707110f67, -0x22b3f62c83c74ab7, 0x7cb16a4cc2736a86, 0x2049bd6e58252a09 },
        { 0x7d09fd8d6a9aef49, -0x0f119f41a4c246f5, 0x4c21b52c519ebfd4, 0x6011aadfc545941d }
    },
    {
        { 0x63ded0c802cbf890, -0x042f6735f2009556, 0x624d0afdb9b6ed99, 0x69ce18b779340b1e },
        { 0x5f67926dcf95f83c, 0x7c7e856171289071, -0x295e180c667085a5, 0x6fc5cc1b0b62f9e0 },
        { -0x2e10aad74d678635, -0x22e551c32b816f6e, 0x127e0442189f2352, 0x15596b3ae57101f1 }
    },
    {
        { 0x09ff31167e5124ca, 0x0be4158bd9c745df, 0x292b7d227ef556e5, 0x3aa4e241afb6d138 },
        { 0x462739d23f9179a2, -0x007cedce68292231, 0x1307deb553f2148a, 0x0d2237687b5f4dda },
        { 0x2cc138bf2a3305f5, 0x48583f8fa2e926c3, 0x083ab1a25549d2eb, 0x32fcaa6e4687a36c }
    },
    {
        { 0x3207a4732787ccdf, 0x17e31908f213e3f8, -0x2a4d132809f269b2, 0x746f6336c2600be9 },
        { 0x7bc56e8dc57d9af5, 0x3e0bd2ed9df0bdf2, -0x553feb21dd101b5d, 0x4627e9cefebd6a5c },
        { 0x3f4af345ab6c971c, -0x1d77148d66bc8ce1, 0x33596a8a0344186d, 0x7b4917007ed66293 }
    },
    {
        { 0x54341b28dd53a2dd, -0x55e86fa420bd03c1, 0x0ff592d94dd2f8f4, 0x1d03620fe08cd37d },
        { 0x2d85fb5cab84b064, 0x497810d289f3bc14, 0x476adc447b15ce0c, 0x122ba376f844fd7b },
        { -0x3dfdcd325d4b1aac, -0x612f02bdeea2e781, 0x2eabb4be7dd479d9, 0x02c70bf52b68ec4c }
    },
    {
        { -0x531acd40ba728d1f, 0x5be768e07cb73cb5, 0x56cf7d94ee8bbde7, 0x6b0697e3feb43a03 },
        { -0x5d7813b4a2f4d045, 0x415c5790074882ca, -0x1fbb59e13e2f7ea4, 0x26334f0a409ef5e0 },
        { -0x49370fb5209d5c40, 0x3ef000ef076da45d, -0x636346a7b60f2d57, 0x1cc37f43441b2fae }
    },
    {
        { -0x2899a90e36315147, 0x1c5b15f818e5656a, 0x26e72832844c2334, 0x3a346f772f196838 },
        { 0x508f565a5cc7324f, -0x2f9e3b3f1af956de, -0x04e75424a3ba53e7, 0x6c6809c10380314a },
        { -0x2d2aaeed1d259538, -0x1642fcce4e17ae13, -0x69f8b92271398d9e, 0x05911b9f6ef7c5d0 }
    },
},
{
    {
        { 0x01c18980c5fe9f94, -0x329a98968e902a38, -0x7e9fba3c2e6a5f7a, 0x6e2b7f3266cc7982 },
        { -0x162328a949c800d3, -0x13b3cb7036780f3c, -0x312a6d7a0c043849, 0x3305354793e1ea87 },
        { -0x337fdb97083ca971, -0x6216457de668b34d, -0x5448dd634a47eca0, 0x44e2017a6fbeba62 }
    },
    {
        { -0x7807d30c49359133, 0x580f893e18f4a0c2, 0x058930072604e557, 0x6cab6ac256d19c1d },
        { -0x3b3d58bcab25488c, -0x71a2b3c3b150fce6, -0x4893dc2dbd7c70e9, 0x749a098f68dce4ea },
        { -0x23201f5fd33e21a0, 0x032665ff51c5575b, 0x2c0c32f1073abeeb, 0x6a882014cd7b8606 }
    },
    {
        { -0x2eee2e8350b01492, 0x050bba42b33aa4a3, 0x17514c3ceeb46c30, 0x54bedb8b1bc27d75 },
        { -0x5ad56d015b8b804b, -0x23ed5bb6e05a5477, -0x27d256b447b85b32, 0x4d77edce9512cc4e },
        { 0x77c8e14577e2189c, -0x5c1b909500663bbb, 0x3144dfc86d335343, 0x3a96559e7c4216a9 }
    },
    {
        { 0x4493896880baaa52, 0x4c98afc4f285940e, -0x10b558645babb74a, 0x5278c510a57aae7f },
        { 0x12550d37f42ad2ee, -0x74871ffb675e040b, 0x5d53078233894cb2, 0x02c84e4e3e498d0c },
        { -0x5ab22f8bd6b3f46c, -0x0aa2b94720e7004a, -0x0f90133a72517c9a, 0x588657668190d165 }
    },
    {
        { -0x40a7cb0fc21da33d, -0x47783751297eab6a, 0x5105221a9481e892, 0x6760ed19f7723f93 },
        { -0x2b88edcee5108ee9, 0x50343101229e92c7, 0x7a95e1849d159b97, 0x2449959b8b5d29c9 },
        { 0x669ba3b7ac35e160, 0x2eccf73fba842056, 0x1aec1f17c0804f07, 0x0d96bc031856f4e7 }
    },
    {
        { -0x4e2acb4f338afa1f, 0x32cd003416c35288, -0x34c95a7ff89d3d63, 0x5bfe69b9237a0bf8 },
        { 0x3318be7775c52d82, 0x4cb764b554d0aab9, -0x5430c2d83388c26f, 0x3bf4d1848123288a },
        { 0x183eab7e78a151ab, -0x44166f3666f6c89d, -0x008e8291b5381ccb, 0x4c5cddb325f39f88 }
    },
    {
        { 0x57750967e7a9f902, 0x2c37fdfc4f5b467e, -0x4d9e99c5ce8845ba, 0x3a375e78dc2d532b },
        { -0x3f0948b29e6f5915, 0x20ea81a42db8f4e4, -0x5742908268cea8a0, 0x33b1d60262ac7c21 },
        { -0x7ebe18d0d2b22216, -0x191501679d39f838, 0x23c28458573cafd0, 0x46b9476f4ff97346 }
    },
    {
        { 0x1215505c0d58359f, 0x2a2013c7fc28c46b, 0x24a0a1af89ea664e, 0x4400b638a1130e1f },
        { 0x0c1ffea44f901e5c, 0x2b0b6fb72184b782, -0x1a78006efeeb2478, 0x37130f364785a142 },
        { 0x3a01b76496ed19c3, 0x31e00ab0ed327230, 0x520a885783ca15b1, 0x06aab9875accbec7 }
    },
},
{
    {
        { 0x5349acf3512eeaef, 0x20c141d31cc1cb49, 0x24180c07a99a688d, 0x555ef9d1c64b2d17 },
        { -0x3ecc667c0a20f145, -0x3f0c8a70aed3b354, 0x2cf1130a0bb398e1, 0x6b3cecf9aa270c62 },
        { 0x36a770ba3b73bd08, 0x624aef08a3afbf0c, 0x5737ff98b40946f2, 0x675f4de13381749d }
    },
    {
        { -0x5ed00926c4254ce3, 0x0725d80f9d652dfe, 0x019c4ff39abe9487, 0x60f450b882cd3c43 },
        { 0x0e2c52036b1782fc, 0x64816c816cad83b4, -0x2f234226969bf8c2, 0x13d99df70164c520 },
        { 0x014b5ec321e5c0ca, 0x4fcb69c9d719bfa2, 0x4e5f1c18750023a0, 0x1c06de9e55edac80 }
    },
    {
        { -0x002ad4bf00929656, 0x34530b18dc4049bb, 0x5e4a5c2fa34d9897, 0x78096f8e7d32ba2d },
        { -0x66f085295cc13b1e, 0x6608f938be2ee08e, -0x635ebc3a9cd7baeb, 0x4cf38a1fec2db60d },
        { -0x5f55559af205a319, -0x063b61d5b74ab874, 0x4f09cc7d7003725b, 0x373cad3a26091abe }
    },
    {
        { -0x0e41570476224453, 0x3bcb2cbc61aeaecb, -0x70a75844e0647263, 0x21547eda5112a686 },
        { -0x4d6b9cb27d360a84, 0x1fcbfde124934536, -0x6163b24cbe7324a6, 0x0040f3d9454419fc },
        { -0x210216c602a6792d, -0x0bd8d376aef5c7f4, -0x48d45bf844cee647, 0x63550a334a254df4 }
    },
    {
        { -0x6445a7ba8dab84b7, -0x0cfa39051d3bf720, 0x60e8fa69c734f18d, 0x39a92bafaa7d767a },
        { 0x6507d6edb569cf37, 0x178429b00ca52ee1, -0x1583ff6f149429a3, 0x3eea62c7daf78f51 },
        { -0x62db38ec196cd8b2, 0x5f63857768dbd375, 0x70525560eb8ab39a, 0x68436a0665c9c4cd }
    },
    {
        { 0x1e56d317e820107c, -0x3ad997bb7bf5169b, -0x3e1f5e39cdf00386, 0x5373669c91611472 },
        { -0x43fdca17dfd0c0d9, -0x38a3ff1d9b068a50, -0x6e5b162a5c73dbea, 0x17b6e7f68ab789f9 },
        { 0x5d2814ab9a0e5257, -0x6f70df7b36354c04, -0x50350a77a4d2e136, 0x1cb4b5a678f87d11 }
    },
    {
        { 0x6b74aa62a2a007e7, -0x0cee1f4f0f8e384f, 0x5707e438000be223, 0x2dc0fd2d82ef6eac },
        { -0x499b3f94c6b50394, 0x0c88de2498da5fb1, 0x4f8d03164bcad834, 0x330bca78de7434a2 },
        { -0x67d1007beee68bb2, -0x0696a169d4f8b8dc, -0x3a753eb04036ac05, 0x3c31be1b369f1cf5 }
    },
    {
        { -0x3e97436c0634bd8e, -0x51478ee038312468, 0x7f0e52aa34ac8d7a, 0x41cec1097e7d55bb },
        { -0x4f0b79b2f76b7512, 0x07dc19ee91ba1c6f, 0x7975cdaea6aca158, 0x330b61134262d4bb },
        { -0x0869e6285d927f76, -0x44e02b61e261ea93, 0x73d7c36cdba1df27, 0x26b44cd91f28777d }
    },
},
{
    {
        { -0x50bb7bd24fd7a0c9, -0x78ace76fb8103721, -0x6a8b1f6e07df6866, 0x0e378d6069615579 },
        { 0x300a9035393aa6d8, 0x2b501131a12bb1cd, 0x7b1ff677f093c222, 0x4309c1f8cab82bad },
        { -0x26056e8e7cf8a5ab, 0x4bdb5ad26b009fdc, 0x7829ad2cd63def0e, 0x078fc54975fd3877 }
    },
    {
        { -0x1dffb4a447cc5676, 0x44775dec2d4c3330, 0x3aa244067eace913, 0x272630e3d58e00a9 },
        { -0x782042ebd77870d3, 0x134636dd1e9421a1, 0x4f17c951257341a3, 0x5df98d4bad296cb8 },
        { -0x0c98702f1336f4ac, -0x0ffeba64edfbca67, 0x26725fbc3758b89b, 0x4325e4aa73a719ae }
    },
    {
        { -0x12db9d6530960a63, 0x2a4a1ccedd5abbf4, 0x3535ca1f56b2d67b, 0x5d8c68d043b1b42d },
        { 0x657dc6ef433c3493, 0x65375e9f80dbf8c3, 0x47fd2d465b372dae, 0x4966ab79796e7947 },
        { -0x11ccd2b21c4bd4f6, -0x27b1a5d4e95b9fe4, 0x78243877078ba3e4, 0x77ed1eb4184ee437 }
    },
    {
        { 0x185d43f89e92ed1a, -0x4fb5e11501b8e63a, 0x499fbe88a6f03f4f, 0x5d8b0d2f3c859bdd },
        { -0x402b1ec0dfe7c660, -0x5110001dc1c20e9f, -0x49a4fb0f94a2e01d, 0x52e085fb2b62fbc0 },
        { 0x124079eaa54cf2ba, -0x28db9a14ffe4d919, 0x6843bcfdc97af7fd, 0x0524b42b55eacd02 }
    },
    {
        { -0x43e72352647d6154, 0x23ae7d28b5f579d0, -0x3cb9edd596c7bdcd, 0x1a6110b2e7d4ac89 },
        { -0x02f2a2411babb850, 0x6cec351a092005ee, -0x665b87bba98a8635, 0x59d242a216e7fa45 },
        { 0x4f833f6ae66997ac, 0x6849762a361839a4, 0x6985dec1970ab525, 0x53045e89dcb1f546 }
    },
    {
        { -0x7b25c32172ba01ee, -0x42bd3de71bbb1d2e, -0x57ae6987e081ca68, 0x7642c93f5616e2b2 },
        { -0x34744cb928acac25, -0x03034db451aee1de, -0x345b72bf2af51911, 0x26e3bae5f4f7cb5d },
        { 0x2323daa74595f8e4, -0x219773747a85414c, 0x3fc48e961c59326e, 0x0b2e73ca15c9b8ba }
    },
    {
        { 0x0e3fbfaf79c03a55, 0x3077af054cbb5acf, -0x2a3aadba24c21c61, 0x015e68c1476a4af7 },
        { -0x2944bbd73e80afda, -0x614d8ddc04a56359, -0x1c845afce6e639bc, 0x21ce380db59a6602 },
        { -0x3e2ad7addff995c8, -0x6a9fc1adca8f510d, -0x7cd9a658dd9475b3, 0x5dd689091f8eedc9 }
    },
    {
        { 0x1d022591a5313084, -0x35d2b55129d8f78e, -0x795ed47ad0f402e0, 0x56e6c439ad7da748 },
        { -0x34537b21402c37aa, 0x1624c348b35ff244, -0x48077235a26352f9, 0x3b0e574da2c2ebe8 },
        { -0x38fb00b6bd42451a, 0x5e21ade2b2de1f79, -0x16a24c0ca9ad0528, 0x0822b5378f08ebc1 }
    },
},
{
    {
        { -0x1e480d6c9d8cfc7d, 0x4b5279ffebca8a2c, -0x25038875402becec, 0x7deb10149c72610f },
        { 0x51f048478f387475, -0x4da2430b634134c4, -0x6554edbb2660dfab, 0x2c709e6c1c10a5d6 },
        { -0x349d509578991186, 0x66cbec045553cd0e, 0x588001380f0be4b5, 0x08e68e9ff62ce2ea }
    },
    {
        { 0x2f2d09d50ab8f2f9, -0x5346de723aa6dc21, 0x4a8f342673766cb9, 0x4cb13bd738f719f5 },
        { 0x34ad500a4bc130ad, -0x72c724b6c2f42b64, -0x5da3c267aff57642, 0x2f1f3f87eeba3b09 },
        { -0x087b738a1aea49b6, -0x5a6afe4524b56fc8, -0x3df2cec0c08ae4b0, 0x19a1e353c0ae2ee8 }
    },
    {
        { -0x4bde8d322a694243, -0x6c1fbabc671103c0, -0x604eacb84bbef64b, 0x736bd3990266ae34 },
        { 0x7d1c7560bafa05c3, -0x4c1e5f5f391aa19f, -0x1cad68e73f299b8d, 0x41546b11c20c3486 },
        { -0x7aacd2af6ccb4c4c, 0x46fd114b60816573, -0x33a0a0cfbda37c8b, 0x412295a2b87fab5c }
    },
    {
        { 0x2e655261e293eac6, -0x7ba56dfcdecc5325, 0x460975cb7900996b, 0x0760bb8d195add80 },
        { 0x19c99b88f57ed6e9, 0x5393cb266df8c825, 0x5cee3213b30ad273, 0x14e153ebb52d2e34 },
        { 0x413e1a17cde6818a, 0x57156da9ed69a084, 0x2cbf268f46caccb1, 0x6b34be9bc33ac5f2 }
    },
    {
        { 0x11fc69656571f2d3, -0x393617baacf18c86, -0x1cc5185d2b01afcb, 0x01b9c7b62e6dd30b },
        { -0x0c20d09bc5873f4e, 0x4c3e971ef22e027c, -0x1382e3a1b63e4a5d, 0x2012c18f0922dd2d },
        { -0x77f4aa1aa53762d7, 0x1483241f45a0a763, 0x3d36efdfc2e76c1f, 0x08af5b784e4bade8 }
    },
    {
        { -0x1d8ceb2d7633d3b5, 0x4be4bd11a287178d, 0x18d528d6fa3364ce, 0x6423c1d5afd9826e },
        { 0x283499dc881f2533, -0x62fada25886cdc4a, -0x7685220498cbbe0c, 0x32b79d71163a168d },
        { -0x337a072612034c96, 0x22bcc28f3746e5f9, -0x1b621cc7061a2c33, 0x480a5efbc13e2dcc }
    },
    {
        { -0x499eb31bbd31dde1, 0x6e199dcc4c053928, 0x663fb4a4dc1cbe03, 0x24b31d47691c8e06 },
        { 0x0b51e70b01622071, 0x06b505cf8b1dafc5, 0x2c6bb061ef5aabcd, 0x47aa27600cb7bf31 },
        { 0x2a541eedc015f8c3, 0x11a4fe7e7c693f7c, -0x0f5099ecb15d872a, 0x545b585d14dda094 }
    },
    {
        { 0x6204e4d0e3b321e1, 0x3baa637a28ff1e95, 0x0b0ccffd5b99bd9e, 0x4d22dc3e64c8d071 },
        { 0x67bf275ea0d43a0f, -0x521971cbf7641142, 0x4289134cd479e72e, 0x0f62f9c332ba5454 },
        { -0x034b9a7629c4a0c7, 0x5cae6a3f57cbcf61, -0x01453d2d6ac505fb, 0x1c0fa01a36371436 }
    },
},
{
    {
        { -0x3ee11a17ab3ac052, 0x6a0b06c12b4f3ff4, 0x33540f80e0b67a72, 0x15f18fc3cd07e3ef },
        { -0x18ab8bb64383296e, 0x0f9abeaae6f73ddf, 0x4af01ca700837e29, 0x63ab1b5d3f1bc183 },
        { 0x32750763b028f48c, 0x06020740556a065f, -0x2ac427ed3cb6a4a8, 0x08706c9b865f508d }
    },
    {
        { -0x3366e4bec74bedba, 0x243b9c526f9ac26b, -0x4610b6b248345443, 0x5fba433dd082ed00 },
        { -0x0c835d54c2cbc201, 0x1a8c6a2d80abc617, -0x71b61fca2b330036, 0x48b46beebaa1d1b9 },
        { -0x63b61caa366be530, -0x468cb5218bb6707c, 0x41c3fed066663e5c, 0x0ecfedf8e8e710b3 }
    },
    {
        { 0x744f7463e9403762, -0x0865721172033637, 0x163a649655e4cde3, 0x3b61788db284f435 },
        { 0x76430f9f9cd470d9, -0x49d533645bd09ff8, 0x1898297c59adad5e, 0x7789dd2db78c5080 },
        { -0x4dddd7e6f291094e, -0x56b5994db931b406, 0x46c1a77a4f0b6cc7, 0x4236ccffeb7338cf }
    },
    {
        { 0x3bd82dbfda777df6, 0x71b177cc0b98369e, 0x1d0e8463850c3699, 0x5a71945b48e2d1f1 },
        { -0x7b68bfb2f2aa1d8c, 0x6c6663d9c4ad2b53, -0x13d04f265256a8cc, 0x2617e120cdb8f73c },
        { 0x6f203dd5405b4b42, 0x327ec60410b24509, -0x63cb8dcf53d577ba, 0x77de29fc11ffeb6a }
    },
    {
        { -0x7ca1ec7013312d36, -0x736150ec1569c466, -0x36a0403f4de9f15a, 0x575e66f3ad877892 },
        { -0x4f53a8367c488758, 0x53cdcca9d7fe912c, 0x61c2b854ff1f59dc, 0x3a1a2cf0f0de7dac },
        { -0x667fc5d8377034c6, 0x345a6789275ec0b0, 0x459789d0ff6c2be5, 0x62f882651e70a8b2 }
    },
    {
        { 0x6d822986698a19e0, -0x2367de1e8b28758f, 0x41a85f31f6cb1f47, 0x352721c2bcda9c51 },
        { 0x085ae2c759ff1be4, 0x149145c93b0e40b7, -0x3b981805800d8c87, 0x4eeecf0ad5c73a95 },
        { 0x48329952213fc985, 0x1087cf0d368a1746, -0x71ad9e4e993ea55b, 0x2d5b2d842ed24c21 }
    },
    {
        { 0x5eb7d13d196ac533, 0x377234ecdb80be2b, -0x1ebb3003830a51dc, 0x5226bcf9c441acec },
        { 0x02cfebd9ebd3ded1, -0x2ba4de88c6fde68c, 0x7576f813fe30a1b7, 0x5691b6f9a34ef6c2 },
        { 0x79ee6c7223e5b547, 0x6f5f50768330d679, -0x128c1e1692752317, 0x27c3da1e1d8ccc03 }
    },
    {
        { 0x28302e71630ef9f6, -0x3d2b5dfcd49b3120, 0x090820304b6292be, 0x5fca747aa82adf18 },
        { 0x7eb9efb23fe24c74, 0x3e50f49f1651be01, 0x3ea732dc21858dea, 0x17377bd75bb810f9 },
        { 0x232a03c35c258ea5, -0x790dc5d39434f30f, 0x3dad8d0d2e442166, 0x04a8933cab76862b }
    },
},
{
    {
        { 0x69082b0e8c936a50, -0x06365fca3e253a4a, 0x6fb73e54c4dfb634, 0x4005419b1d2bc140 },
        { -0x2d39fb49dd6bc201, -0x43734131bb304c60, 0x5d254ff397808678, 0x0fa3614f3b1ca6bf },
        { -0x5ffc014246417d10, 0x2089c1af3a44ac90, -0x07b6606ee6ab0572, 0x1fba218aef40ab42 }
    },
    {
        { 0x4f3e57043e7b0194, -0x57e2c111f7255081, -0x37c639546623210f, 0x6c535d13ff7761d5 },
        { -0x54ab6bb705370ac2, -0x7e0917658459c8bf, 0x74fd6c7d6c2b5e01, 0x392e3acaa8c86e42 },
        { 0x4cbd34e93e8a35af, 0x2e0781445887e816, 0x19319c76f29ab0ab, 0x25e17fe4d50ac13b }
    },
    {
        { -0x6ea0800a890ede59, -0x3cb5cdd8d032781d, -0x3345d021b2e41ada, 0x6bba828f8969899b },
        { 0x0a289bd71e04f676, 0x208e1c52d6420f95, 0x5186d8b034691fab, 0x255751442a9fb351 },
        { -0x1d2e43996f01c6ff, 0x4cb54a18a0997ad5, -0x68e296eb507b9f2c, 0x559d504f7f6b7be4 }
    },
    {
        { -0x63b76e18092d9903, 0x0744a19b0307781b, -0x77c770e29f9e1dc5, 0x123ea6a3354bd50e },
        { -0x588c7c874c14ab2b, 0x1d69d366a5553c7c, 0x0a26cf62f92800ba, 0x01ab12d5807e3217 },
        { 0x118d189041e32d96, -0x46121c3d27cea7b8, 0x1eab4271d83245d9, 0x4a3961e2c918a154 }
    },
    {
        { 0x0327d644f3233f1e, 0x499a260e34fcf016, -0x7c4a58e90d254687, 0x68aceead9bd4111f },
        { 0x71dc3be0f8e6bba0, -0x293107cb81001cf6, -0x566dbda01ec5b896, 0x2cd6bce3fb1db763 },
        { 0x38b4c90ef3d7c210, 0x308e6e24b7ad040c, 0x3860d9f1b7e73e23, 0x595760d5b508f597 }
    },
    {
        { -0x77d5341402fdd870, -0x7650ccfa3beea8a0, 0x65f492e37d3473f4, 0x2cb2c5df54515a2b },
        { 0x6129bfe104aa6397, -0x7069fff75b580335, 0x3f8bc0897d909458, 0x709fa43edcb291a9 },
        { -0x14f5a2739c02d536, -0x2dd43e99d196b101, 0x2723f36ef8cbb03a, 0x70f029ecf0c8131f }
    },
    {
        { 0x2a6aafaa5e10b0b9, 0x78f0a370ef041aa9, 0x773efb77aa3ad61f, 0x44eca5a2a74bd9e1 },
        { 0x461307b32eed3e33, -0x51fbd0cc5baa7e19, -0x36bbb62ce6a0fc9a, 0x0b7d5d8a6c314858 },
        { 0x25d448327b95d543, 0x70d38300a3340f1d, -0x21e3ace39f1e3ad5, 0x272224512c7de9e4 }
    },
    {
        { -0x40844475bd568a04, -0x73a3c68869525ca8, -0x1d803890321255b8, 0x19735fd7f6bc20a6 },
        { 0x1abc92af49c5342e, -0x001127ee4d190530, -0x105d73720337b1d7, 0x11b5df18a44cc543 },
        { -0x1c546f2fbd37bd9a, -0x147b71f080e6ab82, 0x2503a1d065a497b9, 0x0fef911191df895f }
    },
},
{
    {
        { 0x6ab5dcb85b1c16b7, -0x6b3f0317c384d85b, -0x5b4ee3e58caae842, 0x499238d0ba0eafaa },
        { -0x4eaf835e54e39147, -0x42bb70c1e949784d, 0x3455fb7f2c7a91ab, 0x7579229e2f2adec1 },
        { -0x130b91ad854574a9, 0x15a08c478bd1647b, 0x7af1c6a65f706fef, 0x6345fa78f03a30d5 }
    },
    {
        { -0x6c2c341642270f5c, -0x24ead3e402e88cfe, 0x7dbddc6d7f17a875, 0x3e1a71cc8f426efe },
        { -0x20fd06a0efea185f, 0x790ec41da9b40263, 0x4d3a0ea133ea1107, 0x54f70be7e33af8c9 },
        { -0x37c35c1c6f45429e, -0x7f121c98fd6e37cd, -0x377fc7332c86ff3c, 0x2c5fc0231ec31fa1 }
    },
    {
        { -0x3bdd1b2efdba919b, -0x78beb53e352b846f, 0x1592e2bba2b6ffdd, 0x75d9d2bff5c2100f },
        { -0x01456ee8e8fc74b1, -0x1aedc8de3621107f, 0x1c97e4e75d0d8834, 0x68afae7a23dc3bc6 },
        { 0x5bd9b4763626e81c, -0x766996c9435fd123, 0x0a41193d61f077b3, 0x3097a24200ce5471 }
    },
    {
        { -0x5e9d18db996a3b7a, 0x131d633435a89607, 0x30521561a0d12a37, 0x56704bada6afb363 },
        { 0x57427734c7f8b84c, -0x0ebe5ec1fe4d8f17, 0x02d1adfeb4e564a6, 0x4bb23d92ce83bd48 },
        { -0x5093b558ad06ed47, 0x5e665f6cd86770c8, 0x4c35ac83a3c8cd58, 0x2b7a29c010a58a7e }
    },
    {
        { 0x33810a23bf00086e, -0x50316da118c90084, 0x3d60e670e24922d4, 0x11ce9e714f96061b },
        { -0x3bff80882f3e313d, -0x72efdf49453b6d08, 0x32ec29d57e69daaf, 0x599408759d95fce0 },
        { 0x219ef713d815bac1, -0x0ebeb9a2b7a41da4, 0x6d5447cc4e513c51, 0x174926be5ef44393 }
    },
    {
        { 0x3ef5d41593ea022e, 0x5cbcc1a20ed0eed6, -0x702db130f8c7d374, 0x6fa42ead06d8e1ad },
        { -0x4a214d0603a42a45, -0x6d2558d51e27ef1f, -0x503b302348d5e3a7, 0x497d78813fc22a24 },
        { -0x1d897db5e08cc8e1, 0x7f7cf01c4f5b6736, 0x7e201fe304fa46e7, 0x785a36a357808c96 }
    },
    {
        { 0x070442985d517bc3, 0x6acd56c7ae653678, 0x00a27983985a7763, 0x5167effae512662b },
        { -0x7da042029cfeb2d5, -0x37adc9639358a875, 0x5b2fcd285c0b5df0, 0x12ab214c58048c8f },
        { -0x42b1561ef0ac3b4a, 0x1673dc5f8ac91a14, -0x5707e5b1d533e546, 0x33a92a7924332a25 }
    },
    {
        { 0x7ba95ba0218f2ada, -0x300bdd78ccf04636, -0x2525b692a93926f9, 0x5380c296f4beee54 },
        { -0x622e0b66d86693fe, 0x0cb3b058e04d1752, 0x1f7e88967fd02c3e, 0x2f964268cb8b3eb1 },
        { -0x62b0d8fb997672f6, 0x3d0987990aff3f7a, -0x2f610c9d982545bb, 0x7761455e7b1c669c }
    },
},
};
#elif defined(CURVED25519_ASM_32BIT)
static const ge_precomp base[64][8] = {
{
    {
        { -0x0a73c47b, 0x2fbc93c6, -0x0473f1e7, -0x306cd23a, 0x643d42c2, 0x270b4898, 0x33d4ba65, 0x07cf9d3a },
        { -0x28bf6ec2, -0x62efc6fb, -0x2ebf414d, -0x02c660fb, 0x688f8a09, -0x5a3e7bcc, -0x6707ed99, 0x44fd2f92 },
        { 0x4b6fbb59, -0x2442ea99, -0x115d5a16, 0x41e13f00, -0x36a83906, -0x322b62e4, -0x50e91336, 0x4f0ebe1f }
    },
    {
        { -0x6cc38e29, -0x6ddb1804, 0x7a0ff5b5, -0x60b9626a, -0x1e29f8fe, 0x5aa69a65, -0x5782d1d2, 0x590c063f },
        { 0x42b4d5a8, -0x75665aa0, 0x4e60acf6, -0x70d47ef4, -0x4e91c856, -0x1f61dc95, 0x69c92555, 0x6bb595a6 },
        { -0x252c97fe, 0x6e347eaa, -0x7c11b7fb, -0x450ca66d, -0x19f897da, 0x3bcabe10, 0x165ed1b8, 0x49314f0a }
    },
    {
        { 0x4cee9730, -0x50da4f58, -0x1779b476, 0x025a8430, -0x60fe98ce, -0x3ee4affe, -0x657f070c, 0x7a164e1b },
        { -0x5b032d9b, 0x56611fe8, -0x1a3e4583, 0x3bd353fd, 0x214bd6bd, -0x7ece0ce6, 0x555bda62, 0x2ab91587 },
        { -0x0e98b7cc, -0x640dee0c, -0x09d2076b, -0x47b194e9, 0x5b722a4e, -0x282190f9, 0x63bb2a21, 0x549a04b9 }
    },
    {
        { -0x7103f661, 0x287351b9, 0x7dfd2538, 0x6765c6f4, -0x04f56d9b, -0x35cb72c3, 0x21e58727, 0x680e9103 },
        { 0x056818bf, -0x6a01faf6, 0x5660faa9, 0x327e8971, 0x06a05073, -0x3c171c33, 0x7445a49a, 0x27933f4c },
        { -0x1aebd950, -0x40e1ba14, 0x6dba0f94, -0x1cd439c3, -0x7307ad40, -0x1bd68b2b, -0x4f19b3e8, 0x44f079b1 }
    },
    {
        { 0x08a5bb33, -0x5ded43bc, -0x38a112fe, -0x72afb73d, 0x5abfec44, -0x22e414f4, 0x46e206eb, 0x2945ccf1 },
        { -0x5bb82946, 0x7f9182c3, 0x4b2729b7, -0x2affeb2f, -0x479b5f79, -0x1cc30ee4, -0x14e4aa0d, 0x154a7e73 },
        { -0x182ffc4d, -0x37cd5e87, 0x00124d7e, 0x5f729d0a, 0x0e6d8ff3, 0x62c1d4a1, 0x38b27a98, 0x68b8ac59 }
    },
    {
        { 0x77157131, 0x3a0ceeeb, 0x00c8af88, -0x64d8ea77, -0x25a658ca, -0x7f9a4998, -0x5d33c743, 0x51e57bb6 },
        { 0x7b7d8ca4, 0x499806b6, 0x27d22739, 0x575be284, 0x204553b9, -0x44f7a319, -0x51be877c, 0x38b64c41 },
        { 0x689de3a4, -0x7062526f, -0x07046ec9, 0x175f2428, -0x60304678, 0x050ab532, 0x1354c09f, 0x7865dfa2 }
    },
    {
        { -0x6bb15c41, 0x6b1a5cd0, -0x4c623f2e, 0x7470353a, 0x28542e49, 0x71b25282, 0x283c927e, 0x461bea69 },
        { -0x55cdde4f, -0x4590d366, 0x3bba23a7, 0x6ca02153, -0x6de6d3c6, -0x621589b1, 0x2e5317e0, 0x1d6edd5d },
        { -0x54f025ca, 0x217a8aac, 0x3d3549c8, -0x5ad739ac, 0x13ab7568, 0x37d05b8b, 0x3a2cbc37, 0x233cef62 }
    },
    {
        { 0x04dd3e8f, 0x59b75966, -0x1d778fd4, 0x6cb30377, 0x5ed9c323, -0x4ecc639a, 0x61bce52f, 0x0915e760 },
        { -0x0c6dcb27, -0x1d58a213, -0x1e4aa707, -0x69c28980, 0x6e3c23fb, 0x2c2741ac, 0x320e01c3, 0x3a9024a1 },
        { -0x57cb5c82, -0x208217cb, 0x689857ea, -0x741e6326, 0x7167b326, 0x2c118536, -0x24102a3e, 0x589eb3d9 }
    },
},
{
    {
        { 0x2d9021f6, 0x322d04a5, 0x75c6bf9c, -0x463e60cd, 0x42d20b09, 0x587a3a43, -0x559b019f, 0x143b1cf8 },
        { 0x553e2df3, 0x7ec851ca, -0x59b7874d, -0x58ed7b35, 0x3288d1e7, -0x194a1be7, 0x5a9a8883, 0x4cf210ec },
        { -0x69753555, -0x60798383, 0x27092729, 0x5f54258e, -0x15e7f68b, -0x2f582cb5, 0x374126e1, 0x21b546a3 }
    },
    {
        { -0x2e7ade71, 0x490a7a45, 0x46049335, -0x65eac888, -0x33ce1e0a, 0x0060ea09, -0x0791169b, 0x7e041577 },
        { -0x5d777cbd, -0x56b007a8, 0x5313ed3c, -0x31f12baa, -0x4a40cb06, -0x0aa3c231, -0x36154c8f, 0x0a653ca5 },
        { -0x31a4980d, 0x66b2a496, -0x42a9686a, -0x00ab6d28, 0x4a592cd0, 0x503cec29, 0x0813acb2, 0x56694365 }
    },
    {
        { 0x1dabb69d, 0x5672f9eb, -0x5017ac04, -0x458f4acb, 0x2796d66d, 0x47ac0f75, -0x6bee8d8b, 0x32a53517 },
        { 0x26620798, -0x47e724f4, 0x606e354a, 0x5d5c31d9, 0x00a8cdc7, 0x0982fa4f, 0x4653e2d4, 0x17e12bcd },
        { -0x209b7bc9, -0x2c59bb5a, -0x77f04023, 0x703b6559, -0x52c5e55b, -0x347adac0, -0x71b39b98, 0x0900b3f7 }
    },
    {
        { -0x37e952cf, -0x12d7f042, -0x2719101d, 0x52d9595b, -0x0939dc0b, 0x0fe71772, 0x051e293c, 0x4314030b },
        { 0x679d651b, 0x0a851b9f, 0x033342f2, -0x1ef7349f, -0x1774cf5d, -0x29fe0a81, -0x12d228ec, 0x371f3aca },
        { -0x040f4353, -0x2a9fffa2, -0x2e78f3a2, -0x7148f0d2, -0x2f7b1960, 0x201f9033, -0x31849990, 0x4c3a5ae1 }
    },
    {
        { -0x36c25f23, -0x45078a1c, 0x71b9294d, -0x46cd7d59, -0x0b393ba0, -0x7f29c049, -0x15993e7f, 0x6de9c73d },
        { -0x2347056b, 0x4138a434, 0x6c96840b, -0x78f30983, 0x297be82c, -0x21c77a8c, 0x7262a55a, 0x7c814db2 },
        { -0x5fb2070e, 0x478904d5, -0x4efebd2d, -0x050451b6, 0x555d0998, -0x0937539d, 0x2f90b104, 0x5aac4a41 }
    },
    {
        { -0x4280aecc, 0x603a0d0a, -0x1e2c51ba, -0x7f7636ce, -0x7867429d, -0x20da6ec7, 0x74ba0235, 0x1c145cd2 },
        { 0x3ac92908, -0x39b0cd95, -0x199c1e20, 0x5551b282, 0x4a1a4b83, 0x476b35f5, 0x189f68c2, 0x1b9da3fe },
        { 0x75f3d743, 0x32e83864, 0x6ae5d9ef, 0x365b8baf, 0x385b681e, -0x7dadc74a, 0x167d65e1, 0x234929c1 }
    },
    {
        { 0x1d099fcf, 0x48145cc2, -0x33d7281b, 0x4535c192, 0x48247e01, -0x7f183e1b, 0x3b2973ee, 0x4a5f2874 },
        { -0x5f885218, -0x67b21355, 0x19eb389d, 0x383f77ad, 0x2954d794, -0x38139482, -0x1483c586, 0x59c77b3a },
        { 0x225ccf62, -0x2c5228db, -0x4dead3a3, -0x6ee5cc7f, 0x5b08f87d, -0x274c6053, 0x4799fe3b, 0x6f05606b }
    },
    {
        { -0x06e49b7d, 0x5b433149, 0x5a2cbf62, -0x524a239b, 0x632827b3, -0x78057bee, -0x54b60728, 0x60895e91 },
        { 0x177ba962, -0x6001616e, 0x0de5cae1, -0x675118e3, 0x2d831044, 0x3ff4ae94, 0x58533ac8, 0x714de12e },
        { 0x0cf86c18, -0x16130d13, 0x0735dfd4, -0x4b92f9ee, 0x04b96be7, -0x43625f68, -0x26923d95, 0x73e2e62f }
    },
},
{
    {
        { 0x632f9c1d, 0x2eccdd0e, 0x76893115, 0x51d0b696, -0x579c85a8, 0x52dfb76b, -0x5ff110c7, 0x6dd37d49 },
        { 0x49aa515e, -0x12a49cac, 0x0bc6823a, -0x579a3b61, 0x5b42d1c4, -0x7af3e017, 0x03d315b9, 0x30d76d6f },
        { 0x2106e4c7, 0x6c444417, -0x6d728097, -0x04ac2980, 0x694d3f26, -0x4b8c615c, 0x2e864bb0, 0x10c69711 }
    },
    {
        { -0x7ca737fb, 0x0ca62aa0, 0x7a204247, 0x6a3d4ae3, 0x3b11eddc, 0x7464d3a6, 0x550806ef, 0x03bf9baf },
        { 0x7dbe5fde, 0x6493c427, 0x19ad7ea2, 0x265d4fad, 0x46304590, 0x0e00dfc8, -0x129901f7, 0x25e61cab },
        { -0x33a799fc, 0x3f13e128, -0x4ba68b82, 0x6f5873ec, -0x33ed970b, -0x5f49c213, 0x4586e22c, 0x566d7863 }
    },
    {
        { -0x39a5d030, -0x5efabd7b, -0x0ce9983d, 0x6c64112a, 0x731aee58, 0x680ae240, 0x4793b22a, 0x14fba5f3 },
        { -0x633ef7cc, 0x1637a49f, -0x57643baf, -0x4371a92b, 0x7f7fd2db, 0x1cb5ec0f, 0x5ecc35d9, 0x33975bca },
        { 0x6985f7d4, 0x3cd74616, -0x3637ffa9, 0x593e5e84, 0x7b61131e, 0x2fc3f2b6, -0x7c03ad94, 0x14829cea }
    },
    {
        { 0x4e71ecb8, 0x21e70b2f, 0x40a477e3, -0x19a92247, -0x31e2b080, -0x409aa932, 0x535d7b7e, 0x05fc3bc4 },
        { -0x68226a3e, -0x00bc847c, -0x55b14a59, 0x6c744e30, 0x3c85e88b, -0x61f3a29f, 0x5f758173, 0x2fd9c71e },
        { 0x52afdedd, 0x24b8b3ae, -0x12c4cf31, 0x3495638c, -0x56417e6b, 0x33a4bc83, 0x5c651f04, 0x37376747 }
    },
    {
        { 0x14246590, 0x634095cb, 0x16c15535, -0x10edebc0, -0x76ef43a0, -0x61c7ebf4, 0x30907c8c, 0x6bf59057 },
        { 0x40d1add9, 0x2fba99fd, -0x690b2fd9, -0x4cf8e991, 0x15f03bae, 0x4363f052, 0x3b18f999, 0x1fbea56c },
        { -0x1ebea476, 0x0fa778f1, -0x453c5882, 0x06409ff7, -0x655d65b0, 0x6f52d7b8, 0x7a635a56, 0x02521cf6 }
    },
    {
        { 0x772f5ee4, -0x4eeb98e0, -0x69f86532, -0x17076b4f, 0x00ac824a, 0x4af8224d, -0x0832933c, 0x001753d9 },
        { 0x0a9d5294, 0x513fee0b, 0x0fdf5a66, -0x706718a4, -0x401ef832, -0x2b9e7978, 0x71382ced, 0x3fa00a7e },
        { -0x69c224cc, 0x3c69232d, -0x4b68c7a8, 0x1dde87da, -0x5f6e0d7b, -0x55282e07, -0x5fb7124a, 0x12b5fe2f }
    },
    {
        { -0x5290e16e, -0x20d483da, 0x504b8913, 0x4b66d323, 0x751c8bc3, -0x73bf6240, 0x0796c7b8, 0x6f7e93c2 },
        { -0x69031cb3, 0x71f0fbc4, -0x520ca413, 0x73b9826b, -0x00d73a9f, -0x2dfb8d9f, 0x6fb1206f, 0x749b76f9 },
        { -0x515951fb, 0x1f5af604, -0x411b6367, -0x3edcae0f, -0x1100949a, 0x61a808b5, 0x01e02151, 0x0fcec10f }
    },
    {
        { -0x3bdbb1bb, 0x3df2d29d, -0x6c2721f6, 0x2b020e74, -0x7df3deb3, 0x6cc8067e, 0x6feab90a, 0x41377916 },
        { 0x49fe1e44, 0x644d58a6, 0x31ad777e, 0x21fcaea2, -0x77802f2e, 0x02441c5a, -0x7c3aee0d, 0x4901aa71 },
        { -0x73e50710, 0x08b1b754, 0x246299b4, -0x31f08584, 0x1e06d939, -0x089f4f07, 0x726d1213, 0x41bb887b }
    },
},
{
    {
        { -0x55c6082e, -0x68267f20, 0x52c6b51c, 0x35d03842, 0x07cd55aa, 0x7d43f493, -0x48753c9e, 0x56bd36cf },
        { 0x567c49d8, -0x6d987f94, -0x3586e196, 0x066d04cc, -0x1c33c6b5, -0x5960a9bb, -0x5f87732e, 0x5c95b686 },
        { 0x0d14a954, 0x2ac519c1, -0x6b4a0570, -0x150b8b4c, -0x560785a6, -0x19507c7e, -0x78641f6c, 0x0dea6db1 }
    },
    {
        { -0x29578686, 0x15baeb74, -0x053be8ce, 0x7ef55cf1, 0x3c8b05c5, 0x29001f5a, 0x52eaccfb, 0x0ad7cc87 },
        { 0x7344e5ab, -0x559940ac, -0x70e4bcf7, -0x25eda778, -0x02a9b4d1, 0x5e87d2b3, 0x5483b1dd, 0x5b2c7888 },
        { 0x793408cf, 0x52151362, 0x19963d94, -0x14f0e8fd, -0x77c26b9a, -0x57cc4d06, 0x75003c78, 0x093a7fa7 }
    },
    {
        { 0x60a91286, -0x47169fbc, 0x7778d3de, 0x7f3fd804, -0x4075a1d3, 0x67d01e31, -0x3d849ac2, 0x7b038a06 },
        { 0x3a16d7be, -0x1aef821a, -0x650ccd31, -0x5c880024, 0x440b677f, 0x70d5bf18, -0x5b5cebfd, 0x6a252b19 },
        { -0x2c966f0d, -0x6126e62b, -0x24b1460e, 0x5213aebb, 0x4cb99135, -0x38f715fb, 0x72260e56, 0x58ded57f }
    },
    {
        { 0x5b0fd48b, -0x2592acda, -0x6c405678, -0x769f7dcf, 0x61d57e28, -0x287536ce, 0x3a5c8143, 0x79f2942d },
        { -0x16bec289, 0x78e79dad, -0x68d61983, -0x0da8062b, -0x1c85581a, 0x59db910e, -0x4461fc64, 0x6aa11b5b },
        { -0x49377217, -0x6825d0db, -0x530dfe97, 0x251ba7ea, -0x10b14b1c, 0x09b44f87, -0x4395825b, 0x7d90ab1b }
    },
    {
        { -0x694c3c69, 0x1a07a3f4, -0x70b1dace, 0x11ceaa18, -0x588ae410, 0x7d9498d5, 0x508dd8a0, 0x19ed161f },
        { -0x58fe9402, -0x6533597d, -0x0d3af493, -0x6fafa0b3, -0x331bca56, 0x6b610d5f, 0x6198ff96, 0x19a10d44 },
        { -0x78231936, 0x560a2cd6, -0x799b30b3, 0x7f3568c4, 0x22803a38, -0x78be16ae, 0x595653fc, 0x483bdab1 }
    },
    {
        { -0x4b257f0a, -0x2930b2f6, -0x07cf8020, -0x7db7c1bb, -0x5190625c, 0x05005269, -0x63087886, 0x1c705290 },
        { -0x78cb05b7, -0x0587f0ec, 0x360534e0, 0x106f0b70, -0x1c1cf843, 0x2210776f, -0x22195f02, 0x3286c109 },
        { -0x78b1672c, 0x32ee7de2, -0x4681f3a0, 0x14c362e9, 0x6a60a38a, 0x5781dcde, -0x558557c0, 0x217dd5ea }
    },
    {
        { -0x4173f138, -0x7420e047, -0x1cf5fd7e, 0x00bae7f8, -0x5293b094, 0x4963991d, 0x5df6f60a, 0x07058a6e },
        { 0x248e1eb0, -0x62483b30, 0x4d74bf52, -0x1f89681f, 0x3c562354, 0x1e6a9b17, 0x795a4965, 0x7fa7c21f },
        { -0x24ce0981, -0x1614fd3c, 0x10bcfb2b, -0x12da0277, 0x5c5cddb4, 0x46c8131f, -0x5f346432, 0x33b21c13 }
    },
    {
        { 0x5ee38c5b, -0x65504650, 0x071a13c7, -0x4062d2b2, -0x16ccd6f6, -0x71119193, -0x51ef68e9, 0x1c3bab17 },
        { 0x087d8e31, 0x360692f8, -0x2d8e9c09, -0x0b2339c9, 0x65ea5963, 0x25a4e620, 0x5ac160d9, 0x659bf72e },
        { -0x38354850, 0x1c9ab216, 0x07bbc3cc, 0x7d65d374, 0x504a58d5, 0x52744750, 0x131a2990, 0x09f2606b }
    },
},
{
    {
        { 0x7c6691ae, 0x7e234c59, 0x0a85b4c8, 0x64889d3d, 0x354afae7, -0x251d36f4, 0x0c6a9e1d, 0x0a871e07 },
        { 0x744346be, 0x40e87d44, 0x15b52b25, 0x1d48dad4, -0x5ec49fc2, 0x7c3a8a18, 0x2fcdbdf7, 0x4eb728c1 },
        { 0x4bbc8989, 0x3301b599, 0x5bdd4260, 0x736bae3a, 0x19d59e3c, 0x0d61ade2, 0x2685d464, 0x3ee7300f }
    },
    {
        { -0x7be18ae8, 0x43fa7947, 0x639c46d7, -0x1a3905a7, -0x1cfad48c, -0x5ef9a1e3, -0x30476fd0, 0x7d47c6a2 },
        { -0x61822949, -0x0a2daa1c, 0x610b1eac, -0x7fe9eea4, -0x6d1e7836, 0x3c99975d, -0x686eda3e, 0x13815762 },
        { -0x710f2920, 0x3fdad014, -0x6eab90c4, -0x62c18b66, 0x26bb8157, 0x71ec6210, 0x34c9ec80, 0x148cf58d }
    },
    {
        { -0x651b8a93, -0x1da8d083, -0x770cb781, 0x56c345bb, 0x6960a88d, -0x602ef493, 0x4eaea1b9, 0x278febad },
        { 0x7934f027, 0x46a492f6, -0x097bf557, 0x469984be, -0x769ee7ac, 0x5ca1bc2a, -0x42a2442c, 0x3ff2fa1e },
        { -0x736cc69a, -0x4e5597e1, 0x20290c98, -0x73de6b64, 0x219d3c52, 0x39115291, -0x01639885, 0x4104dd02 }
    },
    {
        { -0x24f69548, -0x7edeb1fa, 0x0ce44f35, 0x21a8b6c9, 0x409e2af5, 0x6524c12a, -0x71035b7f, 0x0165b5a4 },
        { 0x1124422a, 0x72b2bf5e, -0x675cc54b, -0x5e05f3cd, -0x05ad499a, -0x6b349eff, -0x5050ac2b, 0x2c863b00 },
        { -0x5f7b958a, -0x0e6f5b8c, -0x32d08340, 0x12eff984, 0x58aa2b8f, 0x695e2906, -0x40013748, 0x591b67d9 }
    },
    {
        { -0x60e74aa3, -0x66464c8f, -0x5e739be2, -0x1b9a1a06, -0x3d60fa13, 0x61081136, 0x7030128b, 0x489b4f86 },
        { -0x7f4b6406, 0x312f0d1c, -0x540c1376, 0x5979515e, -0x610fe378, 0x727033c0, -0x35708435, 0x3de02ec7 },
        { 0x3aeb92ef, -0x2dcdefd3, 0x6116a861, -0x1e9dac4c, 0x190baa24, 0x3d7eabe7, 0x496cbebf, 0x49f5fbba }
    },
    {
        { 0x1e9c572e, 0x155d628c, -0x3a77b8bf, -0x75b27954, 0x515763eb, -0x6e5cad0a, -0x7798aea5, 0x06a1a6c2 },
        { -0x75a4302c, 0x30949a10, -0x439b8c15, -0x23bf2290, 0x307c0d1c, -0x6d3d6b3f, -0x3405918c, 0x5604a86d },
        { 0x7c1764b6, 0x7288d1d4, -0x1fbe74af, 0x72541140, 0x18acf6d1, -0x60fce5a0, -0x01d8bd3a, 0x20989e89 }
    },
    {
        { -0x7a1513d2, 0x1674278b, 0x7acb2bdf, 0x5621dc07, 0x61cbf45a, 0x640a4c16, -0x08fa6a2d, 0x730b9950 },
        { 0x3a2dcc7f, 0x499777fd, -0x5ab0276e, 0x32857c2c, -0x2df81c60, -0x5d86279c, 0x0ca67e29, 0x0403ed1d },
        { -0x78b13aae, -0x36b4d2cb, -0x67db9073, -0x3a193731, 0x16c035ce, -0x0834b906, 0x08303dcc, 0x5bd74543 }
    },
    {
        { 0x15e7792a, -0x7a3b6cdf, -0x42322237, -0x39b3765e, -0x525c289e, -0x62e1c258, 0x3067f82c, 0x5bb7db12 },
        { 0x28b24cc2, 0x7f9ad195, 0x6335c181, 0x7f6b5465, 0x4fc07236, 0x66b8b66e, 0x7380ad83, 0x133a7800 },
        { -0x39359d42, 0x0961f467, 0x211952ee, 0x04ec21d6, -0x642ab890, 0x18236077, 0x58f0e0d2, 0x740dca6d }
    },
},
{
    {
        { -0x12d9e51b, 0x3906c72a, -0x771eff09, -0x65497027, -0x0cc9fe69, -0x0a16fa66, -0x40d492b9, 0x0e53dc78 },
        { -0x2c0f50f5, 0x50b70bf5, -0x1cd18e09, 0x4feaf48a, -0x5aa442cc, 0x60e84ed3, 0x3f50d1ed, 0x00ed489b },
        { 0x7971877a, -0x46f7d641, 0x6d17e631, 0x5e444463, 0x18276893, 0x4d05c52e, 0x5a4a4af5, 0x27632d9a }
    },
    {
        { -0x78150025, -0x567d7a2f, -0x272f579c, -0x5a4b0445, 0x022663f7, -0x49a70d81, -0x26631d7e, 0x3bbc2b22 },
        { 0x54b260ce, -0x2ee00faf, 0x72f95270, -0x27923c72, 0x267cc138, 0x601fcd0d, 0x29e90ccd, 0x2b679164 },
        { 0x583c0a58, -0x46e836ae, 0x0fe4c6f3, 0x653ff9b8, -0x4320c3f4, -0x64f25829, -0x54ab29f2, 0x43a0eeb6 }
    },
    {
        { 0x57875fe8, 0x3ac63223, -0x0a043471, -0x262b0b14, 0x382bb620, -0x72117b6d, 0x4c799fdc, 0x50c5eaa1 },
        { 0x6d4a5487, 0x396966a4, -0x53d44c46, -0x07ee5e76, 0x5628b26b, 0x66e4685b, -0x626d646e, 0x70a47702 },
        { -0x290d04c4, -0x22f12375, -0x63384860, 0x54c63aa7, 0x2c8d9f1a, -0x51f4fcd5, 0x602967fb, 0x6f9ce107 }
    },
    {
        { 0x3520e0b5, 0x13969306, -0x7715fc02, 0x437fcf7c, -0x2c36a644, -0x082b3bf5, -0x076c2127, 0x699154d1 },
        { -0x321e3dd6, -0x52efab4f, 0x48eb32df, -0x3b5716fe, -0x53323f16, 0x5f3e7b33, -0x038669c2, 0x72364713 },
        { -0x4b4d8ada, 0x315d5c75, 0x0236daa5, -0x33347bd3, 0x345fee8e, 0x22f0c8a3, 0x7d39dbed, 0x73975a61 }
    },
    {
        { -0x0bbcc1ba, 0x6f37f392, 0x1f566b18, 0x0e19b9a1, 0x1fd1d662, 0x220fb78a, -0x5c7e36b3, 0x362a4258 },
        { 0x6375da10, -0x1bfdb207, 0x1830c870, 0x78d3251a, 0x658cd91c, -0x6fd4e6b8, 0x29b7438a, 0x7e18b10b },
        { 0x2b6beb2f, -0x6f8e26ed, 0x28418247, 0x0f26e9ad, -0x42136da3, -0x1546e137, -0x0b750d22, 0x4be65bc8 }
    },
    {
        { 0x57c26234, 0x1d50fba2, -0x214f9875, 0x7bd4823a, -0x59ac750b, -0x3d4f2392, 0x351da73e, 0x5665eec6 },
        { -0x5c918fd8, 0x78487feb, 0x1dd8ce34, 0x5f3f1300, 0x4b30c489, -0x6cb04ed3, 0x397f0a2b, 0x056c244d },
        { 0x43bfb210, -0x24c11ff7, 0x20800ac2, 0x49720187, 0x73bd8667, 0x26ab5d61, -0x54dfb6c8, 0x20b209c2 }
    },
    {
        { 0x16bd3289, 0x1fcca945, 0x41420428, 0x448d65aa, 0x16a55d62, 0x59c3b7b2, 0x4e612cd8, 0x49992cc6 },
        { -0x3f804cb5, 0x549e342a, 0x21373d93, 0x02d82208, -0x532e0a99, -0x43d9d290, -0x0435387c, 0x7a92c9fd },
        { 0x70f801de, 0x65bd1bea, -0x01b61d76, 0x1befb7c0, -0x4e4d51b6, -0x579cf933, 0x265c2a09, 0x3b7ac0cd }
    },
    {
        { 0x22ed39a7, -0x0f2ab1b1, 0x5608150a, -0x5d5516e2, -0x1225178b, -0x0bde4d17, 0x6b7de992, 0x31bc531d },
        { -0x73fe4314, -0x7dd411bd, -0x3f0438c5, 0x530cb525, -0x3e6ac017, 0x48519034, -0x1f65f0a5, 0x265cc261 },
        { -0x567f068f, -0x20c2ecb3, 0x221a22a7, 0x7a4fb8d1, 0x35aad6d8, 0x3df7d420, 0x6a1a125e, 0x2a14edcc }
    },
},
{
    {
        { 0x0478433c, 0x231a8c57, -0x3d7ebc63, -0x484ad8f2, -0x1c26f861, -0x24556616, 0x6c2b03d9, 0x2c03f525 },
        { 0x52cfce4e, -0x20b711f9, 0x06ec08b7, -0x3c00050d, -0x46aba63c, 0x05710b2a, -0x69c15c73, 0x161d25fa },
        { 0x7b53a47d, 0x790f1875, -0x30f3a787, 0x307b0130, 0x257ef7f9, 0x31903d77, -0x42694451, 0x699468bd }
    },
    {
        { 0x6aa91948, -0x2722c21a, 0x2fc0d2cc, 0x485064c2, 0x34fdea2f, -0x64b7db9a, 0x6c4a2e3a, 0x293e1c4e },
        { -0x0b250131, -0x42e0d0ba, -0x5b802909, 0x7cef0114, 0x4a47b37f, -0x2ce00226, 0x73905785, 0x525219a4 },
        { -0x6daeed1f, 0x376e134b, -0x235ea260, 0x703778b5, 0x461c3111, -0x4fba7651, 0x7f032823, 0x5b605c44 }
    },
    {
        { -0x0f180fb4, 0x3be9fec6, 0x75e34962, -0x7995a862, 0x1e1de61a, 0x5542ef16, -0x33a5422b, 0x2f12fef4 },
        { 0x20c47c89, -0x469a7fa7, -0x6dc47034, -0x180feff4, 0x02e2ef77, 0x00012565, -0x57514c12, 0x24a76dce },
        { -0x203f38c0, 0x0a4522b2, 0x40c9a407, 0x10d06e7f, 0x78cff668, -0x3930ebbf, 0x18a43790, 0x5e607b25 }
    },
    {
        { -0x5a6930ec, -0x5fd3bce4, -0x512c1c00, -0x1c3bd2c0, 0x2e0f26db, -0x2dbad980, -0x61ba8f98, 0x201f3313 },
        { 0x6cdf1818, 0x58b31d8f, -0x3c9da75e, 0x35cfa74f, 0x66e61d6e, -0x1e4c00b1, 0x6ccdd5f7, 0x5067acab },
        { 0x08039d51, -0x02ad8095, 0x017c0006, 0x18b14964, 0x2e25a4a8, -0x2addf150, 0x62460375, 0x397cba88 }
    },
    {
        { -0x37ec8619, 0x7815c3fb, -0x221ed50f, -0x599e6be0, -0x7a57022b, -0x00563f08, -0x3e1e3dae, 0x771b4022 },
        { -0x0fa6a64e, 0x30c13093, -0x1656868a, -0x1dc55e73, 0x721d5e26, 0x222fd491, 0x766e6c3a, 0x2339d320 },
        { 0x513a2fa7, -0x2782267a, -0x062b30f8, -0x0a53648f, 0x1ea283b3, -0x2f943ce5, 0x19971a76, 0x331a1892 }
    },
    {
        { -0x628a8d51, 0x26512f3a, 0x68074a9e, 0x5bcbe288, 0x1180f7c4, -0x7b123e3f, -0x09b65985, 0x1ac9619f },
        { -0x04b07f3a, -0x0ae990bb, 0x61c775cf, -0x63c93822, -0x6fbe26e4, -0x1c2b17e5, -0x7c4201df, 0x31167c6b },
        { 0x524b1068, -0x0dd4c7be, -0x11631679, 0x5068343b, 0x4a6250c8, -0x03628e7c, 0x1f08b111, 0x61243634 }
    },
    {
        { 0x1a2d2638, -0x749cb61d, -0x642c02cb, -0x62204900, -0x5c5f945c, 0x7f8bf1b8, 0x78d90445, 0x1522aa31 },
        { -0x78b17673, -0x2662be25, 0x6c07dc20, 0x09fea5f1, -0x2ff06444, 0x793d2c67, -0x61a100c0, 0x46ebe230 },
        { 0x69614938, 0x2c382f53, -0x48d292f0, -0x2501bf66, -0x49b90dd9, -0x1737cc6f, 0x0524306c, 0x45fe70f5 }
    },
    {
        { -0x376aeb6f, 0x62f24920, 0x3f630ca2, 0x05f007c8, -0x0a362b48, 0x6fbb45d2, -0x4a85ddbb, 0x16619f6d },
        { -0x69f3f474, -0x25b78a5a, -0x10f1d0e0, 0x5b68d076, 0x3d0b8fd4, 0x07fb51cf, -0x5f1c6d2c, 0x428d1623 },
        { 0x01a308fd, 0x084f4a44, 0x76a5caac, -0x57dde63d, 0x43d1bc7d, -0x214721ba, 0x60bd38c6, 0x1d81592d }
    },
},
{
    {
        { 0x2f89c8a1, 0x3a4a369a, 0x7c8de80d, 0x63137a1d, 0x78eda015, -0x4353ff76, -0x4b7c4fc1, 0x2cb8b3a5 },
        { -0x13d5b3c8, -0x27cc2842, 0x0acc20ed, 0x2c916283, -0x6d208a7f, -0x16c5b856, 0x333c4a81, 0x702d67a3 },
        { -0x34e46f5f, 0x36e417cb, 0x7f11794e, 0x33b3ddaa, -0x77a439f9, 0x3f510808, -0x1957fdf3, 0x24141dc0 }
    },
    {
        { -0x427cea83, -0x6e6da234, 0x22cc8094, 0x3ca12053, 0x3f90d6e4, 0x28e57f18, -0x21d18985, 0x1a4714ce },
        { 0x3fefee9d, 0x59f73c77, -0x3e306763, -0x4c0e1077, -0x1fd1aba1, -0x1ca204be, 0x47a1b47c, 0x5766120b },
        { -0x47494801, -0x24df45f1, 0x77511fa1, -0x48cd3c4a, -0x660fd277, -0x56d4ae40, 0x489ca5f1, 0x4f3875ad }
    },
    {
        { -0x118c1140, 0x79ed13f6, 0x69110bb1, -0x5a39ad93, -0x79fc79f4, -0x1b76d73d, -0x028fa60b, 0x722a1446 },
        { 0x4932ab22, -0x380389d1, 0x2f4c3c1b, 0x7ac0edf7, -0x65576a18, 0x5f6b55aa, -0x52f5ff7f, 0x3680274d },
        { -0x573077e7, -0x2f6a6017, -0x7b8a5664, -0x2f566ab0, 0x20b09cc5, 0x6eac1733, 0x331b1095, 0x628ecf04 }
    },
    {
        { 0x5c74ccf1, -0x64be5308, 0x08265251, -0x498cce7f, 0x11adb147, -0x6636d513, 0x34ecb40f, 0x7a47d70d },
        { -0x562f2244, -0x67434ee8, 0x08b4802b, -0x11bb61cc, -0x47594efc, -0x78f76dda, 0x45c7915d, 0x685f349a },
        { -0x33bc5b0b, 0x60a0c4cb, 0x3677bea9, 0x775c66ca, 0x2ff8f5ed, -0x5e855e8b, 0x0e01fdc0, 0x11ded902 }
    },
    {
        { 0x3bea93b7, 0x471f95b0, 0x3313abd3, 0x0552d7d4, -0x1e81c085, -0x426c8f1e, -0x4df1a414, 0x7b120f1d },
        { -0x351018fc, -0x76f187f7, -0x1cf17394, -0x78d7d693, -0x6d514e37, 0x4c5cd2a3, 0x5771531f, 0x194263d1 },
        { -0x79afd286, 0x17d2fb3d, 0x50a69352, -0x4a9b27bc, -0x59f128a3, 0x7da962c8, 0x318736aa, 0x00d0f85b }
    },
    {
        { -0x0289de3f, -0x598ac3e2, 0x445671f5, 0x69c0b4a7, 0x05b23c11, -0x68e0ad8c, 0x51a8c7cd, 0x387bc748 },
        { 0x777c84fd, -0x6874ebd2, 0x05a8c062, -0x0bfd9bb9, -0x1819ed39, -0x59852ae5, -0x672295cd, 0x2f7b4596 },
        { 0x4a52a9a8, -0x7e76b4b3, -0x09477cd1, -0x5226c1ee, -0x49e429c8, 0x184d8548, -0x29360933, 0x3f1c62db }
    },
    {
        { 0x148f693d, 0x3fad3e40, -0x6b14658e, 0x052656e1, 0x184f4e2f, 0x2f4dcbfd, -0x3b7d1e75, 0x406f8db1 },
        { -0x6e6ef3e1, 0x2e8f1f00, -0x400d1ed4, -0x5b20b020, -0x116d8bc8, 0x60c6560a, -0x53103706, 0x6338283f },
        { 0x7f191ee4, -0x619cf2d4, -0x43c00990, 0x4fbf8301, 0x7afb73c4, 0x787d8e4e, -0x170a705b, 0x50d83d5b }
    },
    {
        { -0x4b2c4993, -0x3f533070, 0x61732e60, -0x58fa621b, 0x70c6b0ba, 0x033d1f78, 0x26d946e4, 0x584161cd },
        { -0x3ee5e769, -0x7a97c6ea, -0x1af92ff8, 0x2d69a4ef, -0x099b42ff, 0x39af1378, 0x361517c6, 0x65942131 },
        { 0x72d27ca2, -0x440d4e60, -0x042138fc, -0x40c6c3a7, -0x1d9d47e2, -0x16724432, 0x3029b589, 0x02eebd0b }
    },
},
{
    {
        { 0x7b85c5e8, -0x789a4961, -0x2e97454e, 0x6ff0678b, 0x1d330f9b, 0x3a70e77c, -0x4f507184, 0x3a5f6d51 },
        { -0x59f253a1, 0x61368756, -0x145423a9, 0x17e02f6a, 0x4cce0f7d, 0x7f193f2d, -0x76132310, 0x20234a77 },
        { 0x7178b252, 0x76d20db6, -0x2ae12ea0, 0x071c34f9, -0x4c1bee90, -0x09d5b5e0, 0x3cffe366, 0x7cd68235 }
    },
    {
        { 0x68acf4f3, -0x599a32a0, 0x3cd7e3d3, 0x42d92d18, 0x336025d9, 0x5759389d, 0x2b2cd8ff, 0x3ef0253b },
        { -0x2778054a, 0x0be1a45b, -0x45bfc492, 0x2a846a32, -0x1691a000, -0x266defee, 0x3bdc0943, 0x2838c886 },
        { 0x4a465030, -0x2e944f31, 0x15c577ab, -0x05b694bf, -0x0b54be63, -0x7d305176, 0x06a82812, 0x21dcb8a6 }
    },
    {
        { -0x4188ce46, -0x6572ff06, 0x629e1889, -0x7dfc9f82, 0x43f3d97f, -0x4d33fdc9, 0x6c6f678b, 0x5d840dbf },
        { -0x73626038, 0x5c600446, -0x2bd55c35, 0x2540096e, 0x12ee2f9c, 0x125b4d4c, -0x6b5ce255, 0x0bc3d081 },
        { 0x309fe18b, 0x706e380d, -0x461e9a39, 0x6eb02da6, 0x7dae20ab, 0x57bbba99, 0x2ac196dd, 0x3a427623 }
    },
    {
        { -0x24bb8135, 0x3bf8c172, -0x39d7d243, 0x5fcfc41f, 0x75aa15fe, -0x7f530040, 0x24e1a9f9, 0x0770c9e8 },
        { -0x758f7b06, 0x4b42432c, -0x20461abb, -0x7675e61d, -0x63a71ba3, -0x4160ffdf, -0x5e92142f, 0x1ff177ce },
        { 0x45b5b5fd, -0x309e2666, 0x1b3a7924, -0x79f67b17, 0x303e3e89, -0x18cff6e7, 0x41500b1e, 0x39f264fd }
    },
    {
        { -0x01f6841f, -0x2e64b555, -0x201fe6d7, -0x5b92031f, 0x2ca6f1ff, -0x3c36f76c, 0x2c35f14e, 0x65c62127 },
        { -0x24181d64, -0x5852cbe9, 0x2b9c139c, -0x426bc896, -0x6ca68457, -0x5f16e472, 0x68889840, 0x1712d734 },
        { -0x31ce6c23, -0x18d47608, -0x5eda3f45, 0x4d103356, 0x2e1cfe83, 0x0419a93d, -0x4e631d8e, 0x22f9800a }
    },
    {
        { -0x65910254, 0x42029fdd, 0x34a54941, -0x46ed3142, -0x78420c85, 0x640f64b9, -0x7a67354c, 0x4171a4d3 },
        { 0x3e9ef8cb, 0x605a368a, -0x5aafb8eb, -0x1c163fde, 0x5f24248f, 0x553d48b0, 0x647626e5, 0x13f416cd },
        { -0x6636b374, -0x05d8a756, -0x4fff47f9, 0x23006f6f, -0x5225ac6e, -0x042d6e23, 0x574bd1ab, 0x508214fa }
    },
    {
        { 0x53d003d6, 0x461a15bb, -0x430c369b, -0x4defd778, 0x6c683a5a, 0x27c57675, -0x37934bb9, 0x3a7758a4 },
        { 0x3ed6fe4b, -0x3dfd96eb, 0x511d77c4, -0x59a598c7, 0x2c14af94, -0x3421d9ba, 0x6faba74b, 0x22f960ec },
        { -0x6c51af8a, 0x548111f6, 0x1dfd54a6, 0x1dae21df, -0x0ceea19b, 0x12248c90, -0x72180b6c, 0x5d9fd15f }
    },
    {
        { -0x1128ade2, 0x3f244d2a, 0x432e9615, -0x71c56fd8, 0x2e9c16d4, -0x1e9b4589, 0x47eb98d8, 0x3bc187fa },
        { 0x6d63727f, 0x031408d3, -0x28384acd, 0x6a379aef, -0x33511db5, -0x561e703b, 0x4f8fbed3, 0x332f3591 },
        { -0x15793df4, 0x6d470115, 0x6c46d125, -0x66754835, 0x3a660188, -0x2887cd4b, -0x6f9045fd, 0x450d81ce }
    },
},
{
    {
        { -0x4d351f4b, 0x23264d66, -0x14359a8a, 0x7dbaed33, -0x0f2db538, 0x030ebed6, -0x089caaf0, 0x2a887f78 },
        { -0x27bac6fe, -0x0751b2d6, -0x1724d2e3, 0x7018058e, -0x382d3ee2, -0x554c66a1, 0x24ccca79, 0x53b16d23 },
        { 0x5c012d4f, 0x2a23b9e7, -0x351e0d16, 0x0c974651, 0x675d70ca, 0x2fb63273, -0x79bbfc0b, 0x0ba7250b }
    },
    {
        { -0x79079264, -0x229ca76d, -0x1ec57a5c, 0x61699176, 0x4eaa7d57, 0x2e511195, -0x049f4205, 0x32c21b57 },
        { 0x029c6421, -0x44f2e703, -0x76d670fe, -0x43d2ebdf, -0x74daf16a, -0x7cb8071a, 0x032d71c9, 0x7b9f2fe8 },
        { 0x319e0780, -0x2787dc33, -0x76888a3b, -0x103b303f, -0x65f54c09, 0x4854fb12, 0x7238c371, 0x12c49d41 }
    },
    {
        { -0x7c866abe, 0x09b3a017, -0x552a11c1, 0x626dd08f, -0x148feb61, -0x45ff4312, -0x5f5bbb37, 0x1421b246 },
        { -0x0017c897, 0x0950b533, -0x71e2942f, 0x21861c1d, 0x1302e510, -0x0fdd27c8, 0x6391cab4, 0x2509200c },
        { -0x73db5839, 0x4aa43a8e, -0x270fa10b, 0x04c1f540, 0x0b3eb9dc, -0x5245a1f4, 0x48a49ce3, 0x2ab55044 }
    },
    {
        { 0x1c5d3afa, -0x23f8539d, -0x06207394, 0x58615171, -0x628c1d50, 0x72a079d8, -0x4b151ea3, 0x7301f4ce },
        { 0x6f0f5dec, 0x2ed22726, 0x5ed50824, -0x67db11bf, -0x6b972beb, -0x7f841384, -0x4ade1dc1, 0x7093bae1 },
        { -0x298dd3bf, 0x6409e759, 0x72bf729b, -0x598b1e31, 0x3c21e569, -0x43f5db15, 0x4ebacb23, 0x390167d2 }
    },
    {
        { -0x5d0dedf5, -0x2844fab5, -0x4efa7649, -0x1d463152, -0x0c3f1242, 0x3fe8bac8, 0x7112cb69, 0x4cbd4076 },
        { -0x45cac0e4, 0x27f58e3b, -0x4095bc9f, 0x4c47764d, 0x6e562650, -0x50443b1b, -0x551e5ba3, 0x07db2ee6 },
        { 0x29c58176, 0x0b603cc0, 0x5cb15d61, 0x5988e382, -0x230f5273, 0x2bb61413, 0x74183287, 0x7b8eec6c }
    },
    {
        { -0x03c7948d, 0x32fee570, -0x25c57339, -0x2574febf, -0x37697ca7, -0x68a002f6, -0x4ecd57ab, 0x6ee809a1 },
        { 0x2cd27cb0, -0x1b35bf88, -0x04169843, -0x25063cdd, -0x752be162, -0x4d642cb6, 0x626ede4d, 0x72810497 },
        { -0x030279c6, -0x6bbb44cf, 0x3e4e48c5, 0x2fe3690a, -0x2f7705db, -0x23d63799, -0x2e8cd6d2, 0x13bd1e38 }
    },
    {
        { 0x1dfac521, 0x223fb5cf, 0x6f554450, 0x325c2531, 0x659177ac, 0x030b98d7, 0x4f88a4bd, 0x1ed018b6 },
        { 0x696149b5, -0x2cd4b328, -0x7e275549, -0x1aa6c829, -0x51edd46c, 0x0bcb2127, -0x4ebf6650, 0x41e86fcf },
        { -0x47fd5950, 0x3630dfa1, 0x42ad3bd5, -0x77f078b9, -0x113a5b2c, 0x0af90d6c, 0x37cdc5d9, 0x746a247a }
    },
    {
        { 0x78d941ed, 0x6eccd852, -0x2dd087bd, 0x2254ae83, 0x7bbfcdb7, -0x3add2fd2, -0x400f1b1e, 0x681e3351 },
        { 0x2b7b9af6, -0x2ace4743, 0x37fc5b51, 0x50050935, -0x3a6cab93, 0x232fcf25, 0x2bb40f49, 0x20a36514 },
        { -0x7cfcb0bb, -0x749b4a63, 0x1fa20efb, 0x2f8b71f2, -0x459aaf1c, 0x69249495, 0x45d5472b, 0x539ef98e }
    },
},
{
    {
        { 0x1cae743f, -0x2f8b276a, -0x11e39c13, -0x0792e70b, -0x180b12d7, -0x68423aa5, 0x663ab108, 0x4cbad279 },
        { -0x59dfad8b, 0x6e7bb6a1, 0x413c8e83, -0x55b0de29, -0x1770a34e, 0x6f56d155, -0x59cba41f, 0x2de25d4b },
        { -0x5f28e033, -0x7f2e6fdc, -0x04d77508, -0x3ada3df6, 0x5f3a6419, -0x4e5c68b5, -0x1dff8dcd, 0x7d7fbcef }
    },
    {
        { -0x0c3d6f6c, -0x3283a23b, 0x2a9105ab, -0x387e5d66, 0x421c3058, -0x7f39e2ca, -0x23272b29, 0x4f9cd196 },
        { 0x266b2801, -0x0510e196, -0x2a8c60ea, -0x7993973c, 0x1b03762c, -0x0975d044, -0x7848a573, 0x5975435e },
        { 0x6a7b3768, 0x199297d8, 0x1ad17a63, -0x2f2fa7dc, 0x5c1c0c17, -0x45fd6353, 0x387a0307, 0x7ccdd084 }
    },
    {
        { 0x6760cc93, -0x64f37be8, 0x1ab32a99, -0x3251ff86, 0x620bda18, -0x5772137a, -0x7e6f35bc, 0x3593ca84 },
        { 0x6d260417, -0x2359bdd4, -0x6b7dbf43, -0x51eac2b0, -0x04973989, -0x563f3e4c, 0x61d0cf53, 0x428bd0ed },
        { 0x5e849aa7, -0x6dece766, 0x65d8facd, -0x2b273ccb, 0x53fdbbd1, -0x73adaba5, -0x25d29c1a, 0x27398308 }
    },
    {
        { 0x0a702453, -0x465ef1b4, -0x2a82e422, 0x0fa25866, -0x32d82509, -0x0046264b, 0x492c33fd, 0x572c2945 },
        { 0x435ed413, 0x42c38d28, 0x3278ccc9, -0x42af0ca0, 0x79da03ef, -0x44f854e6, -0x4173ccab, 0x269597ae },
        { -0x2932cf42, -0x388038bb, -0x1c455105, -0x1b20172d, -0x55a225f4, -0x5dd377d0, -0x3fa43580, 0x7f985498 }
    },
    {
        { 0x0fbf6363, -0x2ca9eaae, -0x30b2045a, 0x08045a45, -0x78c05f3e, -0x113db044, -0x2964ed19, 0x30f2653c },
        { -0x60f41ee9, 0x3849ce88, 0x7b54a288, -0x7ffa52e5, 0x23fc921c, 0x3da3c39f, 0x0a31f304, 0x76c2ec47 },
        { -0x553ef37b, -0x75f736c8, -0x24d89435, 0x46179b60, 0x0e6fac70, -0x56df3fe2, 0x596473da, 0x2f1273f1 }
    },
    {
        { 0x55a70bc0, 0x30488bd7, -0x0e2bbd19, 0x06d6b5a4, -0x43a69e9e, -0x152e5962, -0x123a087c, 0x38ac1997 },
        { -0x751fe1ef, 0x4739fc7c, 0x4a6aab9f, -0x02ad8b70, -0x788d70d2, 0x41d98a82, -0x27a4960e, 0x5d9e572a },
        { -0x58ae4ec5, 0x0666b517, 0x7e9b858c, 0x747d0686, 0x454dde49, -0x53533fef, -0x40161964, 0x22dfcd9c }
    },
    {
        { 0x103be0a1, 0x56ec59b4, -0x2da60697, 0x2ee3baec, 0x13f5cd32, 0x797cb294, 0x24cde472, 0x0fe98778 },
        { -0x3cf2f327, -0x72242d20, -0x5344bccd, -0x527199a1, 0x322a961f, -0x7094da74, 0x5448c1c7, 0x6b2916c0 },
        { 0x0aba913b, 0x7edb34d1, 0x2e6dac0e, 0x4ea3cd82, 0x6578f815, 0x66083dff, 0x7ff00a17, 0x4c303f30 }
    },
    {
        { 0x0dd94500, 0x29fc0358, 0x6fbbec93, -0x132d855c, -0x3d1d5808, 0x130a155f, -0x48f95e2b, 0x416b151a },
        { 0x17b28c85, -0x2cf5c42a, 0x39773bea, -0x3a2c8849, 0x1e6a5cbf, -0x39391874, -0x74d5483c, 0x0d61b8f7 },
        { -0x163ec950, 0x56a8d7ef, 0x58e44b20, -0x42f81a33, 0x1b57e0ab, -0x5019d026, 0x4277e8d2, 0x191a2af7 }
    },
},
{
    {
        { 0x2fe09a14, 0x09d4b60b, -0x244e8b82, -0x3c7b0f51, 0x78b5fd6e, 0x58e2ea89, -0x4a1f64f6, 0x519ef577 },
        { -0x5490b67b, -0x2aaff6a5, 0x4fbfaf1a, 0x04f4cd5b, 0x2a0c7540, -0x6271d12f, -0x4ddedd7a, 0x2bc24e04 },
        { 0x1124cca9, 0x1863d7d9, -0x47758f72, 0x7ac08145, -0x7a8fce0b, 0x2bcd7309, -0x7547051b, 0x62337a6e }
    },
    {
        { 0x1b3a1273, -0x2e54cdb2, -0x7efaacc0, 0x18947cf1, -0x5673e692, 0x3b5d9567, -0x7fd1e198, 0x7fa00425 },
        { 0x06ffca16, 0x4bcef17f, 0x692ae16a, -0x21f91e25, 0x614f42b0, 0x0753702d, 0x5b9212d0, 0x5f6041b4 },
        { 0x028c2705, 0x7d531574, -0x24f28a02, -0x7fce8297, -0x10737223, 0x30fface8, -0x493c1668, 0x7e9de97b }
    },
    {
        { -0x5db2bf23, -0x0ffb419e, 0x0452d41f, -0x45f9a66f, 0x62a44234, -0x7e3ba11f, -0x5ddd9911, 0x4cb829d8 },
        { -0x619a7a5d, 0x1558967b, -0x6716746e, -0x68366320, 0x6eb3adad, 0x10af149b, -0x0b2c7306, 0x42181fe8 },
        { 0x07b86681, 0x1dbcaa84, -0x74d98ac5, 0x081f001e, -0x7bfb717f, 0x3cd7ce6a, 0x3f25f22c, 0x78af1163 }
    },
    {
        { 0x7d65318c, 0x3241c00e, -0x2f179219, -0x19411a24, -0x043f73da, 0x118b2dc2, -0x039fc23d, 0x680d04a7 },
        { 0x0b50babc, -0x7be9142c, 0x28208bee, 0x15087226, -0x463e3c93, -0x5ceb7051, -0x2cd282a3, 0x0d07daac },
        { 0x695aa3eb, -0x063dbeb6, 0x05a68f21, -0x255bd3b4, 0x7f93963e, 0x7c6c2398, 0x0c3954e3, 0x210e8cd3 }
    },
    {
        { 0x37fe6c26, 0x2b50f161, 0x56e404d8, -0x1efd4328, 0x4c561f6b, 0x12b0f141, -0x2fd7136f, 0x51b17bc8 },
        { 0x10a71c06, -0x53bdfe0e, -0x0c404fdf, 0x6a65e0ae, 0x393632f7, -0x43bd3ca4, -0x79a0f8be, 0x56ea8db1 },
        { -0x30acaee7, -0x000a04b5, -0x20eef760, -0x0b676287, -0x65c45cdb, -0x4203159b, 0x74d1a6f2, 0x18a11f11 }
    },
    {
        { -0x2d85a0d4, -0x0429c326, -0x755ef929, -0x0ff03b44, -0x719b5bd0, 0x53fb5c1a, 0x0c1a2e85, 0x04eaabe5 },
        { 0x3f6bba29, 0x407375ab, -0x66e1b7d2, -0x613c4928, -0x1aa06d17, -0x6637f17e, -0x04f3f51f, 0x307c13b6 },
        { -0x34754a19, 0x24751021, 0x5c5010eb, -0x03dcbbb7, 0x4e5610a1, 0x5f1e717b, -0x3d8ef32b, 0x44da5f18 }
    },
    {
        { -0x76271534, -0x6ea90195, -0x1dced95f, -0x19486baf, 0x3944eb4e, -0x428b9c27, 0x767203ae, 0x726373f6 },
        { -0x0e47d14b, 0x033cc55f, 0x411cae52, -0x4ea51c93, -0x7004532d, -0x45bf49e7, 0x532e861f, 0x768edce1 },
        { -0x14810976, -0x1cfa358e, 0x70eadb23, 0x662cf31f, -0x4b3ba498, 0x18f026fd, -0x4a2d1343, 0x513b5384 }
    },
    {
        { -0x750cb315, 0x5e270287, -0x46b92952, -0x6ff4fbf7, -0x25427aee, 0x6512ebf7, -0x77da707f, 0x61d9b769 },
        { -0x38d66762, 0x46d46280, 0x5368a5dd, 0x4b93fbd0, -0x2e89a577, 0x63df3f81, -0x465f5ddd, 0x34cebd64 },
        { 0x49b7d94b, -0x593a58ed, 0x23eb9446, -0x5c0c2ea8, 0x77484834, 0x0416fbd2, 0x2c70812f, 0x69d45e6f }
    },
},
{
    {
        { 0x4f460efb, -0x6019d4bd, -0x59c9f82a, -0x212cfc2c, -0x485f25dc, -0x0faddef2, 0x00545b93, 0x237e7dbe },
        { -0x3ac3ebcf, -0x31e908b5, 0x2072edde, 0x2b9725ce, -0x4a4dc119, -0x47463c91, 0x0b5cc908, 0x7e2e0e45 },
        { 0x6701b430, 0x013575ed, -0x60f402f0, 0x231094e6, -0x7c1b80de, 0x75320f15, -0x4eeeaa1d, 0x71afa699 }
    },
    {
        { 0x473b50d6, -0x15bdc3e4, 0x3b38ef10, 0x51e87a1f, -0x4d36416b, -0x647b40a1, 0x78f89a1c, 0x00731fbc },
        { 0x3953b61d, 0x65ce6f9b, -0x505ebe1a, -0x39a7c616, -0x5608a602, 0x0f435ffd, -0x3d4e3d72, 0x021142e9 },
        { 0x48f81880, -0x1bcf38e8, 0x5ecec119, -0x4069f3de, 0x6bba15e3, -0x49251f7d, 0x47e15808, 0x4c4d6f33 }
    },
    {
        { -0x6770e690, 0x2f0cddfc, -0x4f460ae5, 0x6b916227, 0x779176be, 0x6ec7b6c4, -0x57706058, 0x38bf9500 },
        { -0x3e82e037, 0x18f7eccf, 0x51403c14, 0x6c75f5a6, -0x0811f321, -0x24218ed5, -0x581b85de, 0x193fddaa },
        { 0x37e8876f, 0x1fd2c93c, 0x18d1462c, -0x5d09e1a6, 0x39241276, 0x5080f582, -0x40f2b697, 0x6a6fb99e }
    },
    {
        { -0x491bdc3a, -0x114edd4b, -0x0d790072, -0x6c628ff0, 0x1dcf5d8c, -0x6f56d57d, 0x42c5eb10, 0x136fda9f },
        { 0x560855eb, 0x6a46c1bb, -0x076c0f63, 0x2416bb38, -0x708e533f, -0x28e2eec9, -0x5ce76916, 0x75f76914 },
        { -0x5cfa422f, -0x06b3204f, -0x6007d3f8, 0x0f364b9d, -0x3c44a776, 0x2a87d8a5, 0x0be8dcba, 0x02218351 }
    },
    {
        { 0x43307a7f, -0x62a58eff, -0x3b825ba1, -0x4f9c2162, -0x416d852d, 0x22bbfe52, -0x02bfbd94, 0x1387c441 },
        { 0x5ead2d14, 0x4af76638, -0x3583a7d0, -0x5f712780, 0x10211e3d, 0x0d13a6e6, 0x7b806c03, 0x6a071ce1 },
        { -0x78687508, -0x4a2c3c2f, 0x7f0e4413, 0x722b5a3d, -0x44b88360, 0x0d7b4848, -0x50e1236e, 0x3171b26a }
    },
    {
        { -0x4d75b82f, -0x59f24828, 0x1770a4f1, -0x5940eb2a, 0x53ddbd58, -0x2b5e076d, 0x344243e9, 0x6c514a63 },
        { -0x68a9b358, -0x56d0ce70, 0x2275e119, -0x008447b4, -0x5b78aeb0, 0x4f55fe37, 0x3cf0835a, 0x221fd487 },
        { 0x3a156341, 0x2322204f, -0x45f5fcd3, -0x048c1f17, 0x410f030e, -0x031f22b4, -0x046db556, 0x48daa596 }
    },
    {
        { -0x37b3686d, 0x14f61d5d, -0x10be7dfa, -0x66be061d, 0x346277ac, -0x320a4771, 0x0e8a79a9, 0x58c837fa },
        { 0x5ca59cc7, 0x6eca8e66, 0x2e38aca0, -0x57b8dab5, -0x2de1e832, 0x31afc708, -0x3527b509, 0x676dd6fc },
        { -0x69036fa8, 0x0cf96885, 0x7b56a01b, 0x1ddcbbf3, 0x4935d66a, -0x233d1883, -0x395a80f6, 0x1c4f73f2 }
    },
    {
        { -0x0383cb7c, -0x4c918f92, -0x3c3e309f, 0x73dfc9b4, 0x781cc7e5, -0x14e28637, 0x7daf675c, 0x70459adb },
        { 0x305fa0bb, 0x0e7a4fbd, 0x54c663ad, -0x7d62b320, 0x2fe33848, -0x0bde3c7d, 0x1bf64c42, 0x795ac80d },
        { -0x6e4bd44d, 0x1b91db49, 0x4b02dcca, 0x57269623, 0x1f8c78dc, -0x6020611b, -0x731de02d, 0x5fe16284 }
    },
},
{
    {
        { -0x6aeeac77, 0x315c29c7, -0x79d08b32, -0x281f1af9, -0x7a6d8bce, 0x0c4a7621, 0x4a25a1e4, 0x72de6c98 },
        { 0x4d077c41, -0x1d86f552, -0x248b965d, -0x746c7d90, -0x7542e95e, 0x6eb632dc, -0x55f9b48e, 0x720814ec },
        { -0x40955cf0, -0x51654aad, -0x7f9291e5, 0x050a50a9, -0x5200aec7, -0x6d448bfd, 0x45be618b, 0x0394d276 }
    },
    {
        { -0x4dcaba5c, -0x0ac69bdb, -0x67044d6a, 0x15a7a27e, 0x636fdd86, -0x5493ad44, 0x419334ee, 0x79d995a8 },
        { -0x7a81120c, 0x4d572251, -0x1e616c3b, -0x1c8db123, 0x0b797035, -0x758ebdf2, -0x785418bd, 0x3b3c8336 },
        { 0x1195dd75, -0x3275715a, 0x1dd9a82f, -0x5afb2758, -0x5ca7864a, 0x540dca81, 0x79c86a8a, 0x60dd16a3 }
    },
    {
        { 0x153e47b8, 0x3501d6f8, 0x14a2f60c, -0x485698ac, 0x455d9523, 0x112ee8b6, -0x7eed1576, 0x4e62a3c1 },
        { 0x7381e559, 0x35a2c848, -0x287f7d35, 0x596ffea6, -0x245849ad, -0x34688e15, -0x64b2597b, 0x5a08b501 },
        { 0x516ab786, -0x372b53fc, 0x5295b23d, 0x595af321, -0x24fdcf3f, -0x29122dcc, -0x7da4be34, 0x0929efe8 }
    },
    {
        { -0x52a99ae3, -0x74ce8d49, 0x3fabd717, 0x01581b7a, 0x424df6e4, 0x2dc94df6, 0x2c29284f, 0x30376e5d },
        { -0x342f0d2d, 0x5f0601d1, 0x6132bb7f, 0x736e412f, 0x238dde87, -0x7c9fbbce, -0x0a3f8ac4, 0x1e3a5272 },
        { -0x7ea65a64, -0x2d6e7259, 0x3f0713f3, 0x6bdc1cd9, 0x4acd6590, 0x565f7a93, 0x4cb4c128, 0x53daacec }
    },
    {
        { -0x7ad30250, -0x667ad43d, 0x59d6ed0b, 0x2cc12e95, -0x64a53d85, 0x70f9e2bf, 0x7959ae99, 0x4f3b8c11 },
        { -0x6337582a, 0x4ca73bd7, 0x47e9a9b2, 0x4d4a738f, 0x42f5fe00, -0x0b340ed7, -0x4240f8ae, 0x01a13ff9 },
        { 0x2ff26412, 0x55b6c9c8, 0x1fb667a8, 0x1ac4a8c9, -0x1488740e, -0x2ad84031, 0x7012a3be, 0x303337da }
    },
    {
        { -0x052d022f, -0x6892c335, 0x37a640a8, -0x34777c69, 0x6734cb25, 0x2ff00c1d, 0x789c2d2b, 0x269ff4dc },
        { -0x73e36284, -0x6aabddde, 0x1a9b340f, 0x01fac137, -0x6da4b729, 0x7e8d9177, 0x61b3e31b, 0x53f8ad56 },
        { -0x3f729873, 0x0c003fbd, 0x7ead2b17, 0x4d982fa3, -0x4d1a7d0f, -0x3f819433, -0x20bed5bc, 0x296c7291 }
    },
    {
        { -0x25474a62, -0x204dcdfb, -0x37f6ddb0, 0x465aeaa0, -0x658da2e8, -0x2ecc3ee8, 0x61f117d1, 0x23273702 },
        { 0x33daf397, 0x7903de2b, -0x3659db4d, -0x2f00f9e7, 0x555b3e18, -0x75e2dad5, 0x52e0b7c0, 0x2b6d581c },
        { 0x623e7986, 0x3d0543d3, -0x3d875cac, 0x679414c2, 0x726196f6, -0x51bc0f34, -0x7dba1546, 0x7836c41f }
    },
    {
        { -0x7fee6c84, -0x359ae17c, 0x6ef41a28, -0x394f3b92, 0x5f3f8d52, -0x48fde459, -0x15284603, 0x119dff99 },
        { 0x49e95a81, -0x185dab25, 0x08b0ad73, 0x5192d5d0, -0x2ff503f9, 0x4d20e5b1, 0x2cf25f38, 0x5d55f801 },
        { -0x0b4ce2b3, 0x43eadfcb, 0x11148892, -0x39afc08c, 0x060d3b17, -0x0111973b, -0x22b5f538, 0x329293b3 }
    },
},
{
    {
        { 0x5d7cb208, 0x2879852d, 0x687df2e7, -0x47212290, 0x21687891, -0x23f40055, 0x677daa35, 0x2b44c043 },
        { -0x1e6b69e6, 0x4e59214f, 0x0d71cd4f, 0x49be7dc7, 0x3b50f22d, -0x6cff302e, -0x036e8dce, 0x4789d446 },
        { 0x074eb78e, 0x1a1c87ab, -0x66250b99, -0x05392e72, 0x484f9067, 0x3eacbbcd, 0x2bb9a4e4, 0x60c52eef }
    },
    {
        { 0x7cae6d11, 0x702bc5c2, 0x54a48cab, 0x44c7699b, -0x45b6d14e, -0x1043bfaa, -0x26499893, 0x70d77248 },
        { 0x3bfd8bf1, 0x0b5d89bc, -0x360caae6, -0x4f946dc9, -0x2acfd70b, 0x0e4c16b0, 0x2ccfcaab, 0x10bc9c31 },
        { 0x3ec2a05b, -0x557517b5, -0x12e87e20, -0x6796610c, 0x708e85d1, 0x794513e4, -0x56890bed, 0x63755bd3 }
    },
    {
        { -0x680e5349, 0x3dc71018, -0x3e9a4428, 0x5dda7d5e, 0x0fa1020f, 0x508e5b9c, 0x37c52a56, 0x27637517 },
        { 0x2ad10853, -0x4aa05fc2, -0x6119ca97, 0x356f7590, -0x41964770, -0x60060e03, -0x743e907c, 0x0d8cc1c4 },
        { 0x6eb419a9, 0x029402d3, 0x77b460a5, -0x0f4bb182, -0x2bc3b6aa, -0x30579dd0, 0x7ad166e7, 0x70c2dd8a }
    },
    {
        { -0x471281ed, -0x6e2b6983, -0x28897e86, 0x74252f0a, 0x0d852564, -0x1bf67d20, 0x16a53ce5, 0x32b86138 },
        { -0x609013f2, 0x65619450, 0x46c6518d, -0x11d18157, 0x67e09b5c, -0x68cc3e0d, 0x63948495, 0x2e0fac63 },
        { -0x1bb7329c, 0x79e7f7be, 0x087886d0, 0x6ac83a67, -0x5f1b24d2, -0x07602b27, 0x735a4f41, 0x4179215c }
    },
    {
        { 0x286bcd34, -0x1b51cc47, 0x559dd6dc, -0x4810814a, -0x4c2c71e1, 0x278b141f, 0x2241c286, 0x31fa8566 },
        { -0x282312d6, -0x738f6b19, 0x47d39c70, -0x6804753d, -0x56f926fe, -0x1ec41fcd, 0x0cd99d76, 0x700344a3 },
        { 0x2e3622f4, -0x507d93be, -0x67ccafd3, -0x3edfd679, 0x2b389123, -0x643e481f, -0x566adb77, 0x24bb2312 }
    },
    {
        { -0x0a07a395, 0x41f80c2a, 0x04fa6794, 0x687284c3, -0x5c45e453, -0x76ba2067, -0x0014a2ea, 0x0d1d2af9 },
        { 0x32de67c3, -0x4e5712e9, 0x461b4948, 0x3cb49418, 0x76cfbcd2, -0x7142bcbd, 0x1e188008, 0x0fee3e87 },
        { 0x32621edf, -0x5625755f, 0x59226579, 0x30b822a1, -0x58653e6d, 0x4004197b, 0x18531d76, 0x16acd797 }
    },
    {
        { 0x7887b6ad, -0x36a6393b, 0x5f90feba, -0x6b1e6153, -0x5cbd0afc, 0x16e24e62, 0x18161700, 0x164ed34b },
        { 0x2d9b1d3d, 0x72df72af, -0x5bcddba6, 0x63462a36, 0x16b39637, 0x3ecea079, -0x46cfdcf7, 0x123e0ef6 },
        { 0x192fe69a, 0x487ed94c, 0x3a911513, 0x61ae2cea, -0x465b21d9, -0x7884092d, 0x1073f3eb, 0x78da0fc6 }
    },
    {
        { 0x680c3a94, -0x5d607f0f, 0x1ae9e7e6, 0x71f77e15, 0x48017973, 0x1100f158, 0x16b38ddd, 0x054aa4b3 },
        { -0x1ad43996, 0x5bf15d28, 0x70f01a8e, 0x2c47e318, 0x06c28bdd, 0x2419afbc, 0x256b173a, 0x2d25deeb },
        { 0x19267cb8, -0x2037b973, 0x66e54daf, 0x0b28789c, 0x666eec17, 0x2aeb1d2a, -0x548258a0, 0x134610a6 }
    },
},
{
    {
        { -0x23fd73c4, -0x26ebcf20, 0x5217c771, 0x0eb955a8, 0x2c99a1fa, 0x4b09e1ed, -0x42958bc4, 0x42881af2 },
        { 0x7c59b23f, -0x350aa13e, 0x154d04f2, -0x665112c2, -0x1ebebe0c, 0x68441d72, 0x3932a0a2, 0x14034513 },
        { -0x54a352c3, 0x7bfec69a, 0x4cb2cfad, -0x3dc1732d, -0x04c8295e, 0x685dd14b, 0x15677a18, 0x0ad6d644 }
    },
    {
        { 0x47927e9f, 0x79148928, 0x370aa877, 0x33dad6ef, 0x11122703, 0x1f8f24fa, 0x2adf9592, 0x5265ac2f },
        { 0x417becb5, 0x781a439e, -0x2ef1fd9a, 0x4ac5938c, 0x0692ac24, 0x5da38511, -0x521cedcd, 0x11b065a2 },
        { -0x65034cba, 0x405fdd30, 0x28e63f54, -0x268dc2bc, 0x5f65aaae, -0x6b3fe210, -0x1eb3f7f7, 0x43e4dc3a }
    },
    {
        { -0x523d395d, -0x1590853d, -0x168e836c, -0x2f16d70a, -0x29ba150b, -0x1d2c8616, -0x3ae00442, 0x46dd8785 },
        { -0x56c75ae9, -0x43ed380f, 0x3180b2e1, 0x473028ab, -0x0432dab6, 0x3f78571e, 0x6ff6f90f, 0x74e53442 },
        { 0x375c8898, 0x709801be, -0x1c027cb8, 0x4b06dab5, 0x27230714, 0x75880ced, -0x22d0b3be, 0x2b09468f }
    },
    {
        { -0x7d005fd6, 0x5b979465, -0x01570ab7, -0x25f695af, 0x5f77af9b, -0x5f9caec9, 0x201d1e76, 0x1bcfde61 },
        { -0x48fe346a, -0x6838b612, -0x495c963d, -0x7c0bc72c, -0x65bfd327, 0x62962b8b, -0x67772085, 0x6976c750 },
        { 0x246a59a2, 0x4a4a5490, -0x17802270, -0x29c14222, 0x0d2371fa, -0x26bc8399, -0x2cf0712a, 0x69e87308 }
    },
    {
        { -0x7437fcfd, 0x0f80bf02, 0x7a18cefb, 0x6aae16b3, -0x28d3295d, -0x22b815b9, -0x0b12c656, 0x61943588 },
        { 0x5656beb0, 0x435a8bb1, 0x4f4d5bca, -0x07053646, 0x1548c075, -0x464d873c, -0x176d49de, 0x3eb0ef76 },
        { -0x6efc607b, -0x2d91a3c2, -0x090cc557, -0x3f161883, 0x70066a93, -0x176973ab, 0x1faaaddd, 0x3c34d188 }
    },
    {
        { 0x2fffe0d9, -0x42a4f471, 0x3ed24fb9, 0x6aa25410, -0x4d97de3c, 0x2ac7d7bc, 0x60dca36a, 0x605b394b },
        { -0x5f606140, 0x3f9d2b5e, -0x49dc5770, 0x1dab3b6f, 0x72d926c4, -0x5f645c16, 0x3fd8b36d, 0x37419351 },
        { 0x5a9d1ed2, -0x4b17a91c, 0x6c97a9a2, -0x1017b78a, 0x1e5eee7d, -0x4efb309c, -0x7758e371, 0x2f50b81c }
    },
    {
        { -0x5825add6, 0x2b552ca0, 0x449b0250, 0x3230b336, -0x5b466047, -0x0d3b3a44, 0x58074a22, 0x7b2c6749 },
        { -0x0397ee45, 0x31723c61, 0x6211800f, -0x634bafb8, 0x47995753, 0x768933d3, 0x02752fcd, 0x3491a535 },
        { 0x3ed28cdf, -0x2aae9a78, -0x2c9d21c7, 0x12d84fd2, -0x1cc871b1, 0x0a874ad3, 0x7c763e74, 0x000d2b1f }
    },
    {
        { 0x3e94a8ab, -0x69db8874, -0x16587414, 0x0ad6f3ce, 0x0d743c4f, -0x6b75387f, -0x55130334, 0x76627935 },
        { -0x2f92b599, 0x3d420811, -0x6f1f001d, -0x4103fb7b, -0x42b78422, -0x078f3949, 0x319afa28, 0x6e2a7316 },
        { -0x292a6561, 0x56a8ac24, 0x3096f006, -0x37248ac2, -0x70b3ad67, 0x477f41e6, -0x09379eec, 0x588d851c }
    },
},
{
    {
        { 0x77d1f515, -0x32d59a19, -0x70559f0f, 0x54899187, -0x2543f91b, -0x4e48c444, -0x56833605, 0x654878cb },
        { -0x72094f02, 0x51138ec7, -0x1a8a0ae5, 0x5397da89, 0x717af1b9, 0x09207a1d, 0x2b20d650, 0x2102fdba },
        { 0x055ce6a1, -0x69611bfb, 0x1251ad29, 0x36bca768, -0x55825beb, 0x3a1af517, 0x29ecb2ba, 0x0ad725db }
    },
    {
        { -0x64fa907b, -0x013843f4, -0x180a0029, 0x537d5268, 0x4312aefa, 0x77afc662, 0x02399fd9, 0x4f675f53 },
        { -0x7cb1dba9, -0x23bd984f, 0x70ce1bc5, -0x498abb4b, -0x082ea129, 0x1af07a0b, 0x71a03650, 0x4aefcffb },
        { 0x0415171e, -0x3cd2c9ca, -0x7667b7c5, -0x32d410ef, -0x2f6baef0, -0x78f59153, -0x5d579a9f, 0x0bccbb72 }
    },
    {
        { 0x50fe1296, 0x186d5e4c, -0x01176082, -0x1fc6847e, 0x507031b0, 0x3bc7f6c5, 0x108f37c2, 0x6678fd69 },
        { -0x154e5638, 0x185e962f, 0x65147dcd, -0x791819cb, -0x44a4920e, -0x4f6d1fcf, 0x59d6b73e, 0x4024f0ab },
        { 0x636863c2, 0x1586fa31, 0x572d33f2, 0x07f68c48, 0x789eaefc, 0x4f73cc9f, -0x7152b8ff, 0x2d42e210 }
    },
    {
        { 0x0f537593, 0x21717b0d, 0x131e064c, -0x6eb196f5, 0x752ae09f, 0x1bb687ae, -0x64bdc392, 0x420bf3a7 },
        { -0x6b202d65, -0x680aeceb, 0x313f4c6a, 0x6155985d, 0x08455010, -0x145ec0f9, -0x472d2cde, 0x676b2608 },
        { 0x1c5b2b47, -0x7ec7459b, 0x311b1b80, -0x798e4914, -0x43ceca50, 0x7bff0cb1, -0x63f30e20, 0x745d2ffa }
    },
    {
        { 0x21d34e6a, 0x6036df57, -0x66844c30, -0x4e2477d9, -0x378a9506, -0x2c3df63d, 0x4c1dc839, 0x06e15be5 },
        { 0x2bc9c8bd, -0x40ada5e2, 0x26479d81, -0x15a4d9f8, -0x20feaa25, -0x2aee38f2, -0x69f30a30, 0x1ae23ceb },
        { 0x1932994a, 0x5b725d87, -0x314e2550, 0x32351cb5, -0x254835fb, 0x7dc41549, 0x278ec1f7, 0x58ded861 }
    },
    {
        { -0x493d3658, 0x2dfb5ba8, -0x0ad3a674, 0x48eeef8e, -0x0ed2ea8d, 0x33809107, 0x531d5bd8, 0x08ba696b },
        { -0x0d993aa4, -0x27e8c86d, -0x33bab1b7, -0x3736893b, -0x43d93c58, 0x5ce382f8, 0x5485f6f9, 0x2ff39de8 },
        { -0x3c103a86, 0x77ed3eee, -0x2b00b7ef, 0x04e05517, -0x0e598e35, -0x15c285c1, -0x6b8301ac, 0x120633b4 }
    },
    {
        { 0x4912100a, -0x7d42ceb9, 0x7e6fbe06, -0x21dc8493, 0x11ea79c6, -0x1ee189e7, -0x34c6c422, 0x07433be3 },
        { -0x6e9effbe, 0x0b949878, -0x13140518, 0x4ee7b13c, -0x6b0f5b40, 0x70be7395, -0x4b2a6e7b, 0x35d30a99 },
        { 0x5ce997f4, -0x0086bb40, -0x4fa3ae5d, 0x575d3de4, 0x5a76847c, 0x583381fd, 0x7af6da9f, 0x2d873ede }
    },
    {
        { 0x4e5df981, -0x559dfd1f, 0x5015e1f5, -0x5df2a6e9, -0x451de294, 0x18a275d3, 0x01600253, 0x0543618a },
        { 0x43373409, 0x157a3164, -0x0b557e27, -0x05474812, -0x0a59b7fa, -0x4f6c011a, 0x707fa7b6, 0x2e773654 },
        { -0x68b3dc3f, 0x0deabdf4, -0x6231b96d, -0x5590f5db, -0x5d6545d4, 0x04202cb8, 0x2d07960d, 0x4b144336 }
    },
},
{
    {
        { 0x57c5715e, 0x299b1c3f, 0x6b686d90, -0x69346d62, 0x47235ab3, 0x30048064, -0x5bb2601f, 0x2c435c24 },
        { 0x53242cec, 0x47b837f7, -0x3fbded0e, 0x256dc48c, -0x1e26d73b, -0x1ddd0405, -0x5275d3f9, 0x48ea295b },
        { -0x7f077cc1, 0x0607c97c, -0x35da13a5, 0x0e851578, 0x161ebb6f, 0x54f7450b, -0x5f2107f2, 0x7bcb4792 }
    },
    {
        { 0x045224c2, 0x1cecd0a0, 0x69e53952, 0x757f1b1b, 0x5289f681, 0x775b7a92, 0x16736148, 0x1b6cc620 },
        { 0x2bc73659, -0x7b781c30, 0x059979df, 0x4baf8445, -0x23529041, -0x2e8368a6, -0x2103694a, 0x57369f0b },
        { 0x75638698, -0x0e5666ff, -0x11559f2d, 0x353dd1be, 0x4c9ba488, -0x7b6b8ecd, 0x43ade311, 0x63fa6e68 }
    },
    {
        { -0x2db4a149, 0x2195becd, -0x3f32bb07, 0x5e41f18c, 0x41ca9ede, -0x20d7f8bc, -0x0ca48299, 0x07073b98 },
        { 0x6597c168, -0x2ea3dfad, -0x672d7877, -0x608c8c00, 0x3257ba1f, 0x18aee7f1, 0x07346f14, 0x3418bfda },
        { 0x4ce530d4, -0x2fc39894, 0x3b5df9f4, 0x0b64c047, 0x19b3a31e, 0x065cef8b, 0x533102c9, 0x3084d661 }
    },
    {
        { 0x760321fd, -0x6593178a, -0x6149c528, 0x7fe2b510, -0x7537fa6e, 0x00e7d4ae, -0x44908dc6, 0x73d86b7a },
        { -0x407b9653, -0x1e094862, -0x1d99cecb, 0x15801004, -0x508be7e5, -0x65b67cd0, 0x049b673c, 0x3ba2504f },
        { 0x6dba5ab6, 0x0b52b560, -0x444e1255, -0x56ecb0f1, -0x64fb59cb, 0x30a9520d, 0x7973e5db, 0x6813b8f3 }
    },
    {
        { -0x0cea81d7, -0x0e6b35aa, 0x5ef528a5, 0x136d3570, -0x74fa6644, -0x22b31089, 0x24f833ed, 0x7d5472af },
        { 0x334127c1, -0x67ab4fac, -0x7d0400db, 0x105d0478, 0x44186f4f, -0x24b60807, -0x412f4700, 0x1768e838 },
        { -0x50cc25b9, -0x2f1078b3, -0x491cc607, 0x00d3be5d, -0x63631132, 0x3f2a8a2f, 0x2352435a, 0x5d1aeb79 }
    },
    {
        { -0x49e4588b, 0x12c7bfae, -0x1d9c4003, -0x47b19de1, 0x5c840dcf, 0x0b47a5c3, -0x335079cc, 0x7e83be0b },
        { 0x19cd63ca, -0x0a61944d, 0x21d06839, 0x670c1592, 0x2150cab6, -0x4f92a9a5, 0x104f12a3, 0x20fb199d },
        { 0x6d99c120, 0x61943dee, 0x460b9fe0, -0x79efe0d2, -0x7117a673, 0x6bb2f151, -0x033b8a34, 0x76b76289 }
    },
    {
        { 0x522ec0b3, 0x4245f1a1, 0x2a75656d, 0x558785b2, 0x48a1b3c0, 0x1d485a25, -0x2a701f61, 0x60959ecc },
        { 0x756286fa, 0x791b4cc1, -0x28b5ea84, -0x24312ce9, -0x158d421a, 0x7e732421, 0x1131c8e9, 0x01fe1849 },
        { -0x571285f7, 0x3ebfeb7b, -0x1afd8764, 0x49fdc2bb, 0x3c119428, 0x44ebce5d, -0x416b80b6, 0x35e1eb55 }
    },
    {
        { 0x726ccc74, 0x14fd6dfa, 0x2f53b965, 0x3b084cfe, 0x52a2c8b4, -0x0cc51b0b, 0x0d40166a, 0x59aab07a },
        { -0x3a8c722d, -0x242518ff, -0x4d90e412, -0x063909cb, 0x42f15ef4, 0x61e96a80, -0x509f5b28, 0x3aa1d11f },
        { -0x6da153db, 0x77bcec4c, 0x60137738, 0x18487184, -0x01560baf, 0x5b374337, -0x371955ba, 0x1865e78e }
    },
},
{
    {
        { 0x1c529ccb, -0x6983ab17, 0x64c635fb, 0x30f62692, 0x78121965, 0x2747aff4, -0x150990a4, 0x17038418 },
        { -0x4991e086, -0x333b4839, -0x0af3d082, 0x44157e25, 0x713eaf1c, 0x3ef06dfc, 0x52da63f7, 0x582f4467 },
        { 0x20324ce4, -0x39ce842d, -0x5bb7743c, -0x57efbd18, 0x4e5a1364, -0x4de10e75, -0x325d7237, 0x0c2a1c4b }
    },
    {
        { 0x69bd6945, -0x123b7eb8, -0x41e372de, 0x0d6d907d, -0x2aa33a55, -0x39c42dee, -0x5ceb237d, 0x5a6a9b30 },
        { 0x6f1f0447, -0x2db23830, -0x24783fa7, -0x4dd961c2, -0x044d2d71, -0x2ea4fd8e, -0x3909b789, 0x7c558bd1 },
        { -0x2c69b9c3, -0x2f13eadc, -0x3ca5db10, 0x12bb628a, 0x1cbc5fa4, -0x5af3c587, 0x0afbafc3, 0x0404a5ca }
    },
    {
        { 0x2a416fd1, 0x62bc9e1b, -0x1cafa675, -0x4a3908d8, 0x3d5d6967, 0x04343fd8, -0x18071168, 0x39527516 },
        { 0x0aa743d6, -0x73e0bff9, 0x5b265ee8, -0x33452f35, 0x668fd2de, 0x574b046b, -0x352269cd, 0x46395bfd },
        { 0x1a5d9a9c, 0x117fdb2d, -0x2effa3d6, -0x6388ba44, 0x54d56fea, -0x102b410f, -0x17dd2fea, 0x76579a29 }
    },
    {
        { 0x52b434f2, 0x333cb513, -0x6c217f1f, -0x27cdd7b7, 0x750d35ce, -0x4aaed779, 0x2a2777c1, 0x02c514bb },
        { 0x49c02a17, 0x45b68e7e, -0x43565c81, 0x23cd51a2, -0x13ddb3e5, 0x3ed65f11, -0x61fa424f, 0x43a384dc },
        { -0x740e49bb, 0x684bd5da, -0x094ab4ad, -0x04742c82, -0x564f2dad, 0x313916d7, 0x61548059, 0x11609209 }
    },
    {
        { 0x369b4dcd, 0x7a385616, 0x655c3563, 0x75c02ca7, -0x2b0e7fdf, 0x7dc21bf9, -0x6e191fbe, 0x2f637d74 },
        { 0x29dacfaa, -0x4bb2e997, -0x7beca671, -0x25ad60b4, 0x453d5559, -0x16109c36, -0x3a9671f5, 0x351e125b },
        { 0x1af67bbe, -0x2b4b64ba, -0x3754769f, -0x29fcfc86, -0x06596605, 0x71dee19f, -0x1831d566, 0x7f182d06 }
    },
    {
        { -0x71de8ade, 0x09454b72, -0x2b7b4728, -0x55a7170c, 0x7f46903c, -0x2ca7dab3, 0x241c5217, 0x44acc043 },
        { -0x54fe9714, 0x7a7c8e64, 0x15edc543, -0x34a5b5ab, 0x47cd0eda, 0x095519d3, 0x343e93b0, 0x67d4ac8c },
        { 0x4f7a5777, 0x1c7d6bbb, -0x6e7cec1f, -0x74ca012c, -0x3694b97c, 0x4adca1c6, 0x12ad71bd, 0x556d1c83 }
    },
    {
        { -0x4ee417df, -0x7e0f98aa, 0x10a3f3dd, 0x0faff823, 0x6a99465d, -0x074d2fab, -0x337380fb, 0x097abe38 },
        { 0x0c8d3982, 0x17ef40e3, 0x15a3fa34, 0x31f7073e, 0x0773646e, 0x4f21f3cb, 0x1d824eff, 0x746c6c6d },
        { 0x7ea52da4, 0x0c49c987, -0x6423e2bd, 0x4c436955, -0x0833142e, 0x022c3809, 0x4bee84bd, 0x577e14a3 }
    },
    {
        { -0x42b228d5, -0x6b013142, 0x060f2211, -0x0b95b026, -0x3f372e01, 0x124a5977, -0x04ff6d6b, 0x705304b8 },
        { 0x61a73b0a, -0x0f1d9754, 0x3791a5f5, -0x0d0505f0, 0x6b6d00e9, -0x3e1ec17e, 0x6fd78f42, 0x60fa7ee9 },
        { 0x4d296ec6, -0x49c2e2cb, 0x5fad31d8, -0x0c3cfac2, -0x4b42bd14, 0x670b958c, -0x5e9cac03, 0x21398e0c }
    },
},
{
    {
        { -0x79e48166, -0x793a03ea, 0x6a27c451, -0x095ccfb9, -0x5e16ca69, 0x01667267, 0x6082dfeb, 0x05ffb9cd },
        { -0x72582d11, 0x216ab2ca, -0x660bd7d9, 0x366ad9dd, 0x4fdd3c75, -0x519b4700, 0x53909e62, 0x403a395b },
        { -0x0ac09ec7, -0x59e80561, 0x13e66cb6, 0x60f2b5e5, -0x4cbb755c, -0x28574111, 0x6f5ea192, 0x7a293285 }
    },
    {
        { 0x79639302, -0x4763bbb8, 0x50c67f2c, 0x4ae4f193, -0x37e5063a, -0x0f4ca258, 0x46871017, 0x39d00035 },
        { -0x4fd21778, 0x0b39d761, -0x2dbeb1e1, 0x5f550e7e, 0x22e1a940, -0x59405ba8, -0x02bb8467, 0x050a2f7d },
        { -0x59af2489, 0x437c3b33, -0x453ad44e, 0x6bafe81d, 0x2db7d318, -0x0166bfd3, 0x372ba6ce, 0x2b5b7eec }
    },
    {
        { 0x613ac8f4, -0x596bbfb3, -0x056818d4, 0x500c3c2b, 0x1fcec210, -0x78befb2e, -0x79fb5712, 0x1b205fb3 },
        { -0x7c0af111, -0x4c43b443, -0x736d879a, 0x508f0c99, -0x37481992, 0x43e76587, -0x5b806727, 0x0f7655a3 },
        { -0x2db4ecc4, 0x55ecad37, 0x6038c90b, 0x441e147d, -0x29d39012, 0x656683a1, -0x781f1352, 0x0157d5dc }
    },
    {
        { -0x28e14adc, -0x6ad9aaec, 0x5df14593, -0x19fc277f, 0x0d4de6b7, 0x147cdf41, 0x0437c850, 0x5293b173 },
        { 0x0354c13d, -0x0d5850af, -0x55c8d4a0, -0x285f4ebb, 0x05a3d470, 0x2869b96a, -0x7db9fe8d, 0x6528e42d },
        { 0x4bccf226, 0x23d0e081, -0x7e69046d, -0x6d38ba33, 0x59541e5b, -0x749e8694, -0x3fde0688, 0x40a44df0 }
    },
    {
        { 0x4bc5d095, -0x793691af, -0x03597fb6, -0x0df2bf68, -0x37d915a3, 0x27363d89, 0x5719cacf, 0x39ca3656 },
        { 0x4f20ea6a, -0x25579677, 0x4c620618, -0x15eb5c2f, 0x090bf8be, 0x6001fccb, -0x6b816310, 0x35f4e822 },
        { 0x6f87b75c, -0x68af90d1, 0x034ae070, -0x39db5160, -0x552cb22a, 0x1ec856e3, -0x1bbf1a71, 0x055b0be0 }
    },
    {
        { 0x6ea33da2, 0x4d12a04b, -0x1c9ed923, 0x57cf4c15, -0x11bb2699, -0x6f13698b, 0x2a985aac, 0x64ca348d },
        { -0x768ca2ee, 0x6469a17d, -0x199d460f, -0x2490d82b, 0x6a395681, -0x60345cd8, -0x2d9650db, 0x363b8004 },
        { -0x1b3b6ed3, -0x66a771e7, 0x1ca5ce6b, -0x1033c4b2, -0x05a4672b, 0x4522ea60, 0x1de4a819, 0x7064bbab }
    },
    {
        { 0x42542129, -0x5d6f3f9f, -0x4172a470, -0x0d1d3d52, 0x76abfe1b, -0x30dba725, -0x7c29d941, 0x02157ade },
        { 0x5a770641, -0x46e61eaf, 0x4e7f8039, -0x565d1d39, 0x3df23109, 0x7527250b, -0x53d84875, 0x756a7330 },
        { 0x1b9a038b, 0x3e46972a, 0x7ee03fb4, 0x2e4ee66a, 0x6edbb4ca, -0x7e5db789, -0x7132fa9d, 0x1a944ee8 }
    },
    {
        { 0x182362d6, -0x44bf57a7, -0x75b2e545, -0x4660aa89, 0x758559f6, -0x72e74bd9, 0x4d26235a, 0x26c20fe7 },
        { 0x51039372, -0x2a56e2ef, -0x6635d922, 0x2ed377b7, -0x02c99495, -0x5e8dfd54, -0x296fe66b, 0x0730291b },
        { -0x1633dd0b, 0x648d1d9f, 0x28dd577c, 0x66bc5619, 0x652439d1, 0x47d3ed21, -0x125074b7, 0x49d271ac }
    },
},
{
    {
        { -0x4b48a9ff, 0x2798aaf9, 0x5c8dad72, 0x5eac7213, 0x61b7a023, -0x2d31559f, -0x167082b2, 0x1bbfb284 },
        { 0x382b33f3, -0x760afa76, -0x52b73f4c, 0x5ae2ba0b, -0x5ac24c92, -0x706c4afd, -0x6a5dcd1a, 0x5aa3ed9d },
        { -0x38269a9f, 0x656777e9, 0x72c78036, -0x34d4edac, -0x26af9112, 0x65053299, 0x5e8957cc, 0x4a07e14e }
    },
    {
        { -0x3b885b65, 0x240b58cd, 0x6447f017, -0x02c72522, -0x58379553, 0x19928d32, -0x7b505f7f, 0x50af7aed },
        { -0x67f20667, 0x4ee412cb, 0x3c6ec771, -0x5cea2891, -0x6da38803, -0x445a1222, 0x1d313402, 0x3f0bac39 },
        { 0x15f65be5, 0x6e4fde01, 0x216109b2, 0x29982621, 0x0badd6d9, 0x78020581, -0x45142ffa, 0x1921a316 }
    },
    {
        { -0x260c3e75, -0x28a55266, 0x60b1c19c, 0x566a0eef, 0x255c0ed9, 0x3e9a0bac, -0x5f9d380b, 0x7b049dec },
        { -0x20478f04, -0x76bdd082, 0x4f76b3bd, 0x2c296beb, 0x36c24df7, 0x0738f1d4, -0x1d8c5150, 0x6458df41 },
        { 0x35444483, -0x23341c86, 0x0fedbe93, 0x75887933, 0x12c5dd87, 0x786004c3, -0x3d6af19c, 0x6093dccb }
    },
    {
        { 0x6084034b, 0x6bdeeebe, 0x780fb854, 0x3199c2b6, -0x49d2f96b, -0x68cc8955, -0x749b8270, 0x6e3180c9 },
        { -0x7a1f8f93, 0x1ff39a85, -0x4c18c6cd, 0x36d0a5d8, 0x718f453b, 0x43b9f2e1, 0x4827a97c, 0x57d1ea08 },
        { -0x5ed74f8f, -0x11854919, -0x6c577456, -0x5b3ea693, -0x4dde9ed0, -0x084b217e, -0x226842e8, 0x363e999d }
    },
    {
        { -0x1db4513a, 0x2f1848dc, -0x454350a0, 0x769b7255, 0x3cefe931, -0x6f34c392, -0x39064cab, 0x231f979b },
        { 0x35ee1fc4, -0x6957bc3f, 0x08e4c8cf, -0x68914cab, -0x4a732cd0, -0x4bd097ff, 0x693a052b, 0x48ee9b78 },
        { -0x33d50c3a, 0x5c31de4b, -0x01df72e1, -0x4fb44fd0, -0x3eb04b9a, -0x48728ff7, 0x08792413, 0x079bfa9b }
    },
    {
        { -0x5d2abdbb, -0x0c361280, 0x77f63952, 0x0aa08b78, -0x2ef7ab8b, -0x2892539d, -0x6b8f9c95, 0x1ef4fb15 },
        { -0x25cff20c, -0x1c6fc5af, 0x3da95ab0, -0x7bc69bdd, 0x0b356480, -0x12c30ed3, -0x7b7e8e6c, 0x038c77f6 },
        { 0x5b167bec, -0x7ab1a11a, -0x692f323e, 0x59590a42, -0x67efde67, 0x72b2df34, 0x4a0bff56, 0x575ee92a }
    },
    {
        { 0x0aa4d801, 0x5d46bc45, -0x5acc4628, -0x3c50edd9, 0x2b8906c2, 0x389e3b26, 0x382f581b, 0x200a1e7e },
        { -0x75e7d031, -0x2b3f7f70, -0x66b76243, 0x30e170c2, 0x52f733de, 0x05babd57, 0x2cd3fd00, 0x43d4e711 },
        { -0x1506c53b, 0x518db967, 0x056652c0, 0x71bc989b, 0x567197f5, -0x01d47a27, 0x651e4e38, 0x050eca52 }
    },
    {
        { 0x60e668ea, -0x6853c68a, 0x153ab497, -0x64e64402, 0x34eca79f, 0x4cb179b5, -0x5ece51a9, 0x6151c09f },
        { 0x453f0c9c, -0x3cbce522, -0x008fc465, -0x160afba2, -0x127b84c3, -0x03268537, 0x1c58f4c6, 0x4b0ee6c2 },
        { -0x020fa26a, 0x3af55c0d, 0x2ab4ee7a, -0x22d9d120, 0x12171709, 0x11b2bb87, -0x7ff0fcf5, 0x1fef24fa }
    },
},
{
    {
        { -0x6fe99de0, -0x006e5996, 0x5bf1e009, -0x0ddaad52, 0x7f90df7c, 0x7dff85d8, 0x0c736fb9, 0x4f620ffe },
        { 0x6b6c6609, -0x4b69edc6, -0x7f54a6c8, -0x58af017b, -0x483d85a1, -0x0b8e40c7, 0x77ac193c, 0x507903ce },
        { -0x2021c1cc, 0x62f90d65, -0x4605a053, -0x30d73a6e, -0x39e9baf0, -0x66379107, 0x4a256c84, 0x25d44804 }
    },
    {
        { -0x36fdd4ab, 0x2c7c4415, -0x7ed14e02, 0x56a0d241, -0x2849a1f3, -0x0fd15e37, -0x2acdc4da, 0x4180512f },
        { -0x38164e91, -0x4297dcf2, -0x3e3a86a3, 0x0eb1b9c1, -0x6a494e01, 0x7943c8c4, 0x0bbacf5e, 0x2f9faf62 },
        { -0x75b75a25, -0x5b00c197, -0x426abfc5, -0x4595c7fa, 0x47d5b65d, -0x60831e51, 0x5939d2fb, 0x15e087e5 }
    },
    {
        { -0x0469c0c8, -0x776be792, -0x239c642b, 0x48a00e80, -0x1693e367, -0x5b17f6d5, -0x35a8c99f, 0x5a097d54 },
        { 0x745c1496, 0x12207543, -0x25c79ef4, -0x2500c303, 0x2c71c34f, -0x1b1868d9, 0x34bdede9, 0x39c07b19 },
        { 0x17c9e755, 0x2d45892b, -0x76cf7208, -0x2fcc028e, 0x525b8bd9, 0x6c2fe9d9, -0x3ee33f87, 0x2edbecf1 }
    },
    {
        { -0x2f785da1, -0x11f0f023, 0x5c3e34ee, -0x638aceab, -0x7054c54b, 0x660c572e, 0x544cd3b2, 0x0854fc44 },
        { -0x38ea5f2e, 0x1616a4e3, -0x07cbe2b3, 0x53623cb0, -0x38176635, -0x6910acd7, -0x5997455a, 0x3d4e8dbb },
        { 0x55edad19, 0x61eba0c5, -0x0f57c21a, 0x24b533fe, -0x7c455a08, 0x3b770428, -0x675b8173, 0x678f82b8 }
    },
    {
        { 0x57775696, 0x1e09d940, 0x3cd951db, -0x112ed9a4, 0x20bce16f, -0x056253d5, -0x172f760c, 0x0f7f76e0 },
        { -0x296ff3ac, -0x4eb6e2f5, -0x62ecd9ca, 0x3539722c, 0x0b362bc9, 0x4db92892, -0x59749621, 0x4d7cd1fe },
        { -0x2b7a4ff4, 0x36d9ebc5, -0x1b524c9b, -0x5da69b6e, -0x3dee6333, -0x3e9a6b80, 0x186e0d5f, 0x45306349 }
    },
    {
        { 0x2b072491, -0x695beb14, 0x27a7b65b, 0x1bb22181, 0x6e8a4af0, 0x6d284959, -0x32d889a1, 0x65f3b08c },
        { -0x593200e3, -0x6b222f3f, -0x17bdec52, 0x55f6f115, -0x66d03096, 0x6c935f85, 0x4a37f16f, 0x067ee0f5 },
        { 0x199801f7, -0x134d6001, -0x5d5f08d1, -0x62c9e2e1, 0x75fd2f49, 0x25f11d23, 0x0fe10fe2, 0x124cefe8 }
    },
    {
        { 0x31b16489, 0x1518e85b, -0x248ef405, -0x70552349, -0x5eb51dc7, 0x39b0bdf4, 0x503d20c1, 0x05f4cbea },
        { -0x2e720dab, 0x4c126cf9, 0x147a63b6, -0x3e2b8e17, -0x0c36c4a1, 0x2c6d3c73, -0x1c00795e, 0x6be3a6a2 },
        { -0x3fbeba44, -0x31fbf162, 0x08f6834c, -0x38e00b1e, -0x5477b85d, -0x42ab9173, -0x5b2d545b, 0x64666aa0 }
    },
    {
        { 0x3337e94c, -0x4f3ac409, 0x11e14f15, 0x7cb5697e, 0x1930c750, 0x4b84abac, -0x1f9bfb98, 0x28dd4abf },
        { 0x7c06d912, 0x6841435a, -0x44c07cf5, -0x35edc3df, -0x4e341d88, -0x2b4c84d9, -0x3890afba, 0x1d753b84 },
        { 0x44cb9f44, 0x7dc0b64c, -0x1c6da241, 0x18a3e1ac, 0x2d0457c4, 0x7a303486, -0x75f376d2, 0x4c498bf7 }
    },
},
{
    {
        { 0x30976b86, 0x22d2aff5, -0x3d2db9fc, -0x726f47fa, 0x4de5bae5, -0x235e7694, -0x37cbf3e9, 0x28005fe6 },
        { 0x1aa73196, 0x37d653fb, 0x3fd76418, 0x0f949530, -0x04c5e84e, -0x52dff4f7, 0x2fc8613e, 0x544d4929 },
        { 0x34528688, 0x6aefba9f, 0x25107da1, 0x5c1bff94, 0x66d94b36, -0x08a44433, 0x0f316dfa, 0x72e47293 }
    },
    {
        { -0x2cd589d9, 0x07f3f635, 0x5f6566f0, 0x7aaa4d86, 0x28d04450, 0x3c85e797, 0x0fe06438, 0x1fee7f00 },
        { -0x687ef7b1, 0x2695208c, 0x23450ee1, -0x4eafd5f5, 0x03efde02, -0x0262515a, 0x2733a34c, 0x5a9d2e8c },
        { 0x03dbf7e5, 0x765305da, 0x1434cdbd, -0x5b250db7, -0x2db57714, 0x7b4ad5cd, -0x11fbfabd, 0x00f94051 }
    },
    {
        { 0x07af9753, -0x28106c45, 0x3db766a7, 0x583ed0cf, 0x6e0b1ec5, -0x31966741, 0x5dd40452, 0x47b7ffd2 },
        { -0x3c2ccf4e, -0x72ca94dd, -0x4fb8e4fa, -0x0de37465, 0x6e42b83c, -0x4c93ce94, -0x74154ef3, 0x07d79c7e },
        { -0x43f722ee, -0x78040464, -0x1e113d65, -0x75f994c6, -0x24e03e41, 0x0d57242b, 0x5ea64bb6, 0x1c3520a3 }
    },
    {
        { 0x216bc059, -0x325790c0, 0x12bcd87e, 0x1fbb231d, 0x17c70990, -0x4b6a9562, 0x66d12e55, 0x38750c3b },
        { -0x43345cb6, -0x7f2dac5a, 0x3838219b, 0x3e61c3a1, -0x677d1c6a, -0x6f3c49ff, 0x5d0ee66f, 0x1c3d0577 },
        { -0x6bdd1ae6, 0x692ef140, 0x2b5df671, -0x343f38c4, 0x744ce029, 0x21014fe7, -0x2ccfb784, 0x0621e2c7 }
    },
    {
        { -0x4f240f0d, -0x4851e86a, -0x1e831e6a, 0x54dfafb9, -0x16555c4c, 0x25923071, -0x5effd163, 0x5d8e589c },
        { -0x7da67c73, -0x50679f34, -0x39606524, -0x6f15b73f, 0x65581e30, 0x65264837, 0x7bd3a5bc, 0x0007d609 },
        { 0x0842a94b, -0x3f40e26b, 0x588f2e3e, -0x4d2c3c9d, -0x44ae1d11, 0x0a961438, 0x3c1cbf86, 0x1583d778 }
    },
    {
        { -0x3362d739, -0x6ffcb8fc, -0x08d33a71, 0x1d1b679e, -0x41a478da, 0x16e12b5f, -0x7c3aa7f6, 0x4958064e },
        { 0x5da27ae1, -0x13115d11, 0x55670174, 0x597c3a14, 0x6609167a, -0x3659d5ee, -0x7e127090, 0x252a5f2e },
        { 0x5066e80d, 0x0d289426, 0x307c8c6b, -0x033c087b, 0x0c1112fd, 0x1b53da78, -0x27bc4c78, 0x079c170b }
    },
    {
        { -0x3f2a2faa, -0x322932b0, -0x44fca8c5, -0x65089793, -0x0c3c10b8, 0x3ca6723f, 0x317b8acc, 0x6768c0d7 },
        { 0x64fa6fff, 0x0506ece4, 0x6205e523, -0x411cbce2, 0x51b8ea42, 0x35794224, 0x4ac9fb00, 0x6dec05e3 },
        { -0x0eaa3e4d, -0x6b49da1b, -0x6684846f, 0x417bf3a7, 0x6d6b2600, -0x3dd34224, -0x2232ad0c, 0x51445e14 }
    },
    {
        { 0x2bbea455, -0x76ceb855, -0x6df86ed7, -0x73ac5db1, -0x41cf0859, 0x4b49f948, 0x6e4fd43d, 0x12e99008 },
        { 0x3b144951, 0x57502b4b, 0x444bbcb3, -0x71980095, 0x166385db, -0x474296d9, -0x1c6d6a38, 0x13186f31 },
        { 0x7fdfbb2e, -0x0ef3694d, 0x121ceaf9, -0x60656ca2, 0x3a5b983f, -0x20eec93c, 0x5d3e99af, 0x77b2e3f0 }
    },
},
{
    {
        { -0x33a32d65, -0x6acd0b71, -0x5c31c98f, 0x2ba851be, 0x51122941, 0x32dacaa0, 0x350004f2, 0x478d99d9 },
        { -0x630ed9a9, -0x02f28a79, -0x1ac5f1d7, -0x17d0106c, 0x5bbb4be7, -0x33cb5810, -0x5af3c75e, 0x0b251172 },
        { -0x6f44fd40, 0x1d5ad948, 0x0ec25115, 0x50e208b1, 0x4ef21702, -0x5d95dd77, 0x3b524805, 0x4dc92334 }
    },
    {
        { -0x0c93b68b, 0x3ad3e3eb, 0x37862125, -0x28a2da5b, -0x5fda5aea, -0x178c6bc3, -0x3bee37b9, 0x6bbc7cb4 },
        { 0x0f8086b6, -0x1c7d73c0, -0x6860f238, 0x3f77e6f7, 0x4df42cb4, 0x7ef6de30, -0x4954287c, 0x5265797c },
        { -0x2b5af2aa, 0x3c6f9cd1, -0x39015482, -0x49dbbf89, 0x3580972e, 0x6ff9bf48, -0x4ccd5305, 0x00375883 }
    },
    {
        { 0x6c75c99c, -0x3674137b, 0x00e33cf4, -0x1bbe7b40, -0x456f89cc, 0x0a676b9b, 0x71f379d7, 0x669e2cb5 },
        { 0x28cb0940, 0x0001b2cd, 0x6f1c24c9, 0x63fb51a0, -0x232a35cf, -0x4a52796f, -0x73baf9a0, 0x67238dbd },
        { -0x5b642cf8, -0x34ee948d, 0x2392729e, 0x025aad6b, 0x3f55d9b1, -0x4b86c106, 0x40678bb9, 0x72a10561 }
    },
    {
        { -0x1d1afa4a, 0x0d8d2909, -0x3fd6edd0, -0x67358755, -0x564edcd9, 0x77ef5569, -0x7ebc64b9, 0x7c77897b },
        { 0x1cc9249d, -0x5d497ed5, 0x21211f58, 0x62866eee, 0x5df10ece, 0x2cb5c5b8, -0x1d9c5200, 0x03a6b259 },
        { -0x21cce34b, -0x0e3e4a1e, 0x15fca420, 0x5a9f5d8e, 0x7bd932b1, -0x605bc70f, 0x1c6146e7, 0x2a381bf0 }
    },
    {
        { -0x4acbe991, -0x083f41ce, 0x19cf70d4, 0x27e6ca64, -0x56a858a7, -0x6cb20829, -0x54213d56, 0x5701461d },
        { -0x3037ee3f, -0x53646787, 0x3756e567, -0x7482d67f, 0x7c70edfc, 0x50da4e60, -0x77bbff4a, 0x5dbca62f },
        { 0x2c915c25, 0x2c674740, 0x0b0d340a, 0x1bdcd1a8, 0x07b43f5f, 0x5e5601bd, 0x5539a242, 0x2555b4e0 }
    },
    {
        { -0x781b9c2c, 0x78409b1d, -0x32049c63, -0x52b256a6, 0x55259b9c, -0x13d788c9, -0x3cedcf55, 0x69c806e9 },
        { 0x66ddd216, 0x6fc09f52, -0x371c8fb8, -0x231a9f59, -0x5d209d03, -0x139a6c63, -0x1ad12e6e, 0x7a869ae7 },
        { 0x14bb3f22, 0x7b48f574, -0x51233378, 0x68c7cee4, 0x79ed80be, -0x12d06c9f, 0x5f77bc4b, 0x25d70b88 }
    },
    {
        { -0x44e51b2c, -0x67ba62d7, 0x39f954ec, 0x56b9c4c7, -0x3d64b4c2, -0x7cd8bc0a, -0x67497876, 0x21ea8e27 },
        { 0x762bf4de, 0x4151c3d9, 0x2745d82b, 0x083f435f, 0x0d23ddd5, 0x29775a2e, 0x69a5db24, 0x138e3a62 },
        { 0x6a5a7b9c, -0x78410b4c, 0x5fc1d062, -0x2dd662e5, -0x22cde9b8, -0x7dbf67e8, -0x1a5d1fc3, 0x5c5abeb1 }
    },
    {
        { 0x1306a233, 0x02cde6de, 0x116f8ec7, 0x7b5a52a2, -0x3ee9c4a5, -0x1e397e0c, 0x60d32643, 0x241d3506 },
        { -0x48c3d225, 0x14722af4, 0x5a05060d, -0x43b8f3a1, 0x2581b02e, 0x00943eac, 0x1f499c8f, 0x0e434b3b },
        { 0x0ebc52c7, 0x6be4404d, -0x4e586e0b, -0x51b9dcc5, -0x2da24bd5, 0x2aec170e, 0x6645d694, 0x1d8dfd96 }
    },
},
{
    {
        { 0x12ddb0a4, -0x2a679c64, -0x3fdb7995, -0x5a2e60d0, 0x58fce460, -0x2e83d0fd, 0x2e095e8a, 0x07a19515 },
        { -0x63d13b22, 0x296fa9c5, 0x4f84f3cb, -0x43749e41, 0x17a8f908, 0x1c7706d9, 0x7ad3255d, 0x63b795fc },
        { 0x389e5fc8, -0x57c970fe, -0x30721bc5, -0x6fbcc4fe, -0x3abed9bd, -0x505e02a3, 0x032f0137, 0x3e8fe83d }
    },
    {
        { -0x17102ec4, 0x08704c8d, 0x33e03731, -0x203ae572, 0x1260cde3, -0x5a62a25b, -0x59da737a, 0x22d60899 },
        { 0x0570a294, 0x2f8b15b9, 0x67084549, -0x6b0dbd90, 0x61bbfd84, -0x21e3a51f, 0x7fac4007, 0x75ba3b79 },
        { 0x70cdd196, 0x6239dbc0, 0x6c7d8a9a, 0x60fe8a8b, -0x14bfeda0, -0x4c77b844, -0x788861a2, 0x0904d07b }
    },
    {
        { 0x48f940b9, -0x0bcdd29a, -0x42d2f3c7, 0x06952f0c, -0x5f7e06cf, 0x167697ad, -0x4508d594, 0x6240aace },
        { -0x22456e64, -0x4b31e02c, -0x38b37256, -0x30ce24c2, -0x527933af, 0x2c63cc63, -0x43e221f9, 0x43e2143f },
        { 0x5ba295a0, -0x07cb8b64, -0x35c82da6, -0x296b83a5, -0x1836ce96, 0x66f13ba7, -0x724bf354, 0x56bdaf23 }
    },
    {
        { -0x3e62c44e, 0x1310d36c, 0x622386b9, 0x062a6bb7, -0x285eb0a4, 0x7c9b8591, 0x7e1e5754, 0x03aa3150 },
        { -0x0acacc15, 0x362ab9e3, 0x6eb93d40, 0x338568d5, 0x1d5a5572, -0x61f1ebae, -0x7c8bece8, 0x1d24a86d },
        { -0x002b31e1, -0x0b1389b8, 0x54ac8c1c, -0x1fba1510, 0x1d09357c, -0x772dda7e, -0x6514b7a7, 0x43b261dc }
    },
    {
        { 0x6c951364, 0x19513d8b, 0x000bf47b, -0x6b018eda, -0x2ab06a99, 0x028d10dd, 0x42940964, 0x02b4d5e2 },
        { -0x77448645, -0x1aa4e1e7, -0x3e85ca63, -0x5f612f83, 0x603dea33, -0x4fd3d11e, 0x5b276bc2, 0x326055cf },
        { 0x28d18df2, -0x4b5eaa35, 0x186ce508, -0x1533b9ba, 0x6c824389, -0x3b630b6d, -0x51a2cbf0, 0x27a6c809 }
    },
    {
        { -0x3bc296ac, -0x32d3d8f6, 0x6a66cab2, -0x22b5c1a9, 0x69d7036c, 0x79fa5924, 0x3d8c2599, 0x22150360 },
        { 0x1f0db188, -0x74591433, 0x675a5be8, 0x37d3d73a, 0x15f5585a, -0x0dd1205d, -0x009f5e82, 0x2cb67174 },
        { 0x390be1d0, 0x59eecdf9, 0x728ce3f1, -0x56bddfbc, 0x7a94f0f4, -0x7d76e39a, 0x3890f436, 0x7b1df4b7 }
    },
    {
        { 0x07f8f58c, 0x5f2e2218, -0x2b6bf62c, -0x1caaa361, 0x1fb6a630, -0x4d555773, -0x2cad1fc3, 0x68698245 },
        { -0x4c4d5ddc, -0x1b6d0d20, 0x2b551160, 0x7c6c9e06, 0x0d7f7b0e, 0x15eb8fe2, 0x58fc5992, 0x61fcef26 },
        { 0x2a18187a, -0x244ea27b, -0x79225329, -0x0c1b552d, 0x0ff6c482, 0x44bae281, 0x3daf01cf, 0x46cf4c47 }
    },
    {
        { -0x0eb67ec0, 0x213c6ea7, 0x392b4854, 0x7c1e7ef8, 0x5629ceba, 0x2488c38c, 0x0d8cc5bb, 0x1065aae5 },
        { -0x613b1a07, 0x426525ed, 0x16903303, 0x0e5eda01, -0x341a3524, 0x72b1a7f2, 0x14eb5f40, 0x29387bcd },
        { -0x20dff2a9, 0x1c2c4525, -0x403598b6, 0x5c3b2dd6, -0x1e7cbfd0, 0x0a07e7b1, 0x4f1ce716, 0x69a198e6 }
    },
},
{
    {
        { -0x61d2b8cc, 0x7b26e56b, -0x7e39e98b, -0x3b38ecd5, -0x13632181, -0x10a36adb, -0x18e8bc53, 0x39c80b16 },
        { -0x10562969, 0x7afcd613, 0x1c067959, 0x0cc45aa4, -0x3e05256a, -0x5a901efc, 0x72e40365, 0x3a73b704 },
        { 0x1b826c68, 0x0f196e0d, 0x4960e3db, -0x08e00f1e, 0x23b7436c, 0x61131670, 0x77da7282, 0x0cf0ea58 }
    },
    {
        { 0x3ba6945a, -0x1ccd312c, -0x177e3fa3, -0x21f4ec9f, 0x5e67ed3b, 0x1ad40f09, -0x4739c2a3, 0x5da8acda },
        { -0x222b3343, 0x196c80a4, -0x6a0d2263, 0x22e6f55d, 0x40d6c71b, -0x38a1cc39, -0x34c3fbd1, 0x7bb51279 },
        { 0x3a70159f, -0x3b4999b6, 0x0a904e14, 0x76194f0f, -0x5bf693ed, -0x5a9eb3c7, -0x68601313, 0x6cd0ff50 }
    },
    {
        { -0x4fb45e72, 0x7fecfabd, 0x3bddbcf7, -0x2f038404, 0x057a131c, -0x5be2b792, -0x0dddc59f, 0x641a4391 },
        { -0x70bbd754, -0x3f1f9819, -0x59eeca1d, 0x14835ab0, 0x38062935, -0x0de2eb0d, -0x20fb7b64, 0x6390a4c8 },
        { -0x59f95725, -0x3a3946a6, -0x4f97da0f, -0x6eb48062, 0x44fc9eff, 0x2a731f6b, 0x62705cfc, 0x30ddf385 }
    },
    {
        { 0x68bcd52c, 0x33bef2bd, 0x69482ef2, -0x39b62450, 0x41cb1aee, -0x4a4911f4, 0x0212a7e5, 0x5c294d27 },
        { -0x2e400807, 0x4e3dcbda, 0x20645717, -0x36ee717e, 0x0f189d56, -0x45333144, -0x2bb98998, 0x1b4822e9 },
        { 0x25563781, -0x54c9f581, 0x480f7958, 0x2512228a, 0x6114b4e3, -0x38a2fad9, -0x268901d6, 0x222d9625 }
    },
    {
        { 0x0a344f85, 0x0f94be7e, -0x780dd3c8, -0x14d05574, 0x4ee16f0f, -0x631e18a2, 0x18a08dea, 0x43e64e54 },
        { -0x4c8d531f, 0x1c717f85, 0x4638bf18, -0x7e6cf197, 0x6bc08b58, 0x239cad05, -0x7807000c, 0x0b34271c },
        { 0x1a35ce63, -0x7eaa1dae, -0x06edfd72, -0x41eff2b3, -0x5a822314, -0x4007f408, 0x6d6bc6e4, 0x57342dc9 }
    },
    {
        { 0x1e707bf6, -0x0c3c4349, 0x7291a762, 0x351d9b8c, -0x252965cd, 0x00502e6e, 0x1ec8807f, 0x522f521f },
        { -0x3731a668, -0x10110f9b, -0x4a34155e, -0x40fd6af0, 0x20b7c458, -0x739b5efa, 0x31c24855, 0x35134fb2 },
        { -0x065c6fd5, 0x272c1f46, -0x669a8434, -0x36e45c49, 0x4f8a1c0e, -0x519eb4d0, 0x0b99017b, 0x7afcaad7 }
    },
    {
        { -0x107bd495, -0x577ebe14, -0x6854193b, 0x55e7b147, 0x03784ffe, -0x738b7069, -0x5032ff49, 0x5b50a1f7 },
        { -0x5b4741bf, -0x3da212ac, 0x1bb0e2dd, -0x6fd2ec1f, -0x3217d54e, 0x41f43233, -0x3c551835, 0x1085faa5 },
        { -0x0ec9eceb, -0x647bf09a, 0x701003e9, 0x18462242, -0x1b5daf80, 0x65ed45fa, 0x3fda7320, 0x0a286239 }
    },
    {
        { 0x6ecb9d17, -0x69f18c85, -0x2983151f, -0x050db6b8, -0x2aa1e477, 0x37e7a9b4, -0x4b93a615, 0x5cb7173c },
        { 0x347cbc9d, 0x46ab13c8, -0x663edc7d, 0x3849e8d4, -0x7829b537, 0x4cea3140, -0x4e5d6119, 0x1f354134 },
        { -0x7d485410, 0x4a89e68b, -0x64594847, -0x0be326d9, -0x1e727891, 0x16e6c210, 0x7f1b09c6, 0x7cacdb0f }
    },
},
{
    {
        { -0x233a3513, -0x1efebbcc, 0x3c84fb33, 0x47ed5d96, -0x12795f19, 0x70019576, -0x2d98061c, 0x25b2697b },
        { -0x26e58744, -0x6f9d4d20, -0x37af6999, 0x47c9889c, 0x405070b8, -0x620ab59a, 0x2493a1bf, 0x7369e6a9 },
        { 0x13986864, -0x6298c005, 0x415dc7b8, 0x3ca5fbd9, -0x20d8c4a2, -0x1fb133c5, -0x4ab1b32e, 0x1420683d }
    },
    {
        { -0x3e33a530, 0x34eebb6f, -0x69b95375, 0x6a1b0ce9, -0x599421ad, -0x2c4f25b7, 0x61d081c1, 0x31e83b41 },
        { 0x249dd197, -0x4b8742e2, 0x5e58c102, 0x620c3500, -0x334553a4, -0x04fd2cd1, -0x0af758d3, 0x60b63beb },
        { -0x61f9d4b1, -0x681738ee, 0x29320ad8, 0x49e48f4f, 0x6f18683f, 0x5bece14b, 0x2d550317, 0x55cf1eb6 }
    },
    {
        { 0x7df58c52, 0x3076b5e3, -0x186633ca, -0x28c54623, 0x4913ee20, -0x427ce31d, 0x62ba0133, 0x1a56fbaa },
        { 0x65c23d58, 0x58791010, 0x5094819c, -0x7462f793, 0x12c55fa7, -0x1dbfd057, 0x570891d4, 0x669a6564 },
        { 0x5c9dc9ec, -0x6bc194b0, -0x5883c8e6, 0x302557bb, 0x41347651, -0x678c51aa, -0x663a75a4, 0x13c48367 }
    },
    {
        { 0x5d8bd080, -0x3b230496, 0x571a4842, -0x21143b14, -0x471aac9b, -0x2b4d177d, -0x371a47d9, 0x50bdc87d },
        { 0x5ab3e1b9, 0x423a5d46, -0x380ec09f, -0x03ec3e79, -0x134a464a, 0x19f83664, -0x59c849f9, 0x66f80c93 },
        { 0x6edfe111, 0x606d3783, -0x0fee5427, 0x32353e15, 0x25b73b96, 0x64b03ac3, 0x725fd5ae, 0x1dd56444 }
    },
    {
        { 0x08bac89a, -0x3d681a00, -0x151e3c20, 0x7d4cea11, -0x60186884, -0x0c1c741f, 0x63a305cd, 0x3a3a450f },
        { 0x3362127d, -0x705b8008, 0x71cd7c15, -0x4360953c, 0x49220c8b, 0x6e714543, 0x219f732e, 0x0e645912 },
        { -0x27c6b9d9, 0x078f2f31, -0x216b5af0, 0x389d3183, 0x17996f80, -0x2e1c9393, -0x6c565785, 0x318c8d93 }
    },
    {
        { -0x54e22c68, 0x5d669e29, 0x342d9e3b, -0x036de9a8, -0x0ca68c33, 0x55851dfd, 0x25950af6, 0x509a41c3 },
        { 0x2afffe19, -0x0d8ba2fd, 0x7f24db66, 0x0c9f3c49, -0x457a6711, -0x43672c1d, -0x65e2acec, 0x224c7c67 },
        { -0x5906da17, -0x423f9124, 0x641b1f33, 0x793ef3f4, -0x627cc177, -0x7d13ed80, 0x28a11389, 0x05bff023 }
    },
    {
        { 0x0dc512e4, 0x6881a0dd, 0x44a5fafe, 0x4fe70dc8, -0x70b5adc0, 0x1f748e6b, -0x11fe5c16, 0x576277cd },
        { 0x23cae00b, 0x36321370, -0x2e5330a7, 0x544acf0a, -0x2de5e378, -0x698befb7, -0x05d5bb59, 0x780b8cc3 },
        { 0x234f305f, 0x1ef38abc, 0x1405de08, -0x65a88043, 0x34e62a0d, 0x5e82a514, 0x6271b7a1, 0x5ff41872 }
    },
    {
        { 0x13b69540, -0x1a24b818, 0x432610e1, -0x0ca2d5c5, 0x38781276, -0x53e0d917, -0x5f5f3497, 0x29d4db8c },
        { 0x1789db9d, 0x398e080c, -0x0c18870b, -0x589fdfdb, 0x06bd035d, -0x056776b4, 0x25a966be, 0x106a03dc },
        { 0x333353d0, -0x2652f551, -0x532cf61b, 0x38669da5, -0x37770810, 0x3c57658a, 0x052cbefa, 0x4ab38a51 }
    },
},
{
    {
        { -0x7f621fac, -0x09701d18, -0x637d452f, -0x1c43f696, 0x0aadbf45, 0x076353d4, -0x215e6a62, 0x7b9b1fb5 },
        { 0x4324c0e9, -0x20253412, 0x3f955bb7, 0x05444288, -0x15ce9f61, -0x21085558, 0x42287cff, 0x68aee706 },
        { 0x7471cc0c, -0x0fe3370f, 0x579082bb, -0x6adbd1c9, -0x2c1b94a1, 0x27776093, 0x28bd85fb, 0x2d13d55a }
    },
    {
        { 0x7aee7a52, -0x40fe6332, -0x1bab152d, -0x57212d4a, -0x785744e7, 0x3c619f0b, 0x560916d8, 0x3619b5d7 },
        { 0x5b35b8da, -0x053a2dfa, -0x7a9db449, -0x57257566, 0x3d21cd0f, -0x332d356f, -0x7406f2a8, 0x6b8341ee },
        { 0x0282c4b2, 0x3579f26b, 0x4fafefae, 0x64d592f2, 0x28c8c7c0, -0x48321285, 0x7173a8d7, 0x6a927b6b }
    },
    {
        { 0x3ece88eb, -0x728fbf7a, -0x7f113f74, -0x0f1cf857, 0x0d788fda, -0x53ddaf9f, 0x3a0d478d, 0x056d92a4 },
        { -0x6791b9aa, 0x1f6db24f, -0x2e16efa5, 0x1021c02e, 0x2cc0a375, -0x0700c001, -0x3937da6e, 0x1d2a6bf8 },
        { -0x03c25a5f, 0x1b05a196, 0x43b59ed0, 0x77d7a8c2, -0x682e86e8, 0x06da3d62, -0x0edcac09, 0x66fbb494 }
    },
    {
        { -0x0edcf62a, -0x2928f66a, -0x163c2ac7, -0x2404dc7b, -0x08aadbef, 0x46d602b0, 0x57843e0c, 0x270a0b05 },
        { -0x27a3f048, 0x751a50b9, -0x7430f685, -0x2e5023db, -0x7cf65697, 0x2f16a6a3, -0x1a4ff9a7, 0x14ddff9e },
        { -0x5879d434, 0x61ff0640, 0x5f11abfe, -0x7e353f66, 0x55d12abb, -0x6fb87cfc, -0x6ba5178d, 0x19a4bde1 }
    },
    {
        { -0x3f893b61, 0x40c709de, 0x7f3e53f6, 0x657bfaf2, -0x135fbd3c, 0x40662331, 0x7eb4df04, 0x14b37548 },
        { 0x20a6200a, -0x6460d90b, -0x30ec1508, 0x64804443, -0x79ce122d, -0x759c98c1, 0x1ed39dc1, 0x72bbbce1 },
        { -0x549923b9, -0x517ac36c, -0x2089d292, -0x149dcbc2, 0x6fb2f7d1, -0x0f71f1e8, 0x700ab37a, 0x4f0b1c02 }
    },
    {
        { -0x3e4d1dc1, 0x79fd21cc, 0x453df52a, 0x4ae7c281, -0x2eaeb795, -0x37e8d137, 0x3e0a7534, 0x68abe944 },
        { -0x27e6ae06, -0x1e8f9879, -0x4d6f3885, -0x5ef5d372, 0x3ed66773, -0x18c7d060, 0x0bcc4b54, 0x0a4d8471 },
        { 0x07831dcb, -0x25ed393c, 0x4d5c510d, 0x0da230d7, 0x6bd404e1, 0x4ab1531e, -0x430bbf11, 0x4106b166 }
    },
    {
        { 0x39e4ecf2, -0x5b7a332b, 0x0555bab5, 0x5aa3f3ad, -0x6c8207d3, 0x145e3439, 0x1214283f, 0x1238b51e },
        { 0x1cd23668, 0x02e57a42, 0x0eaef6fd, 0x4ad9fb5d, -0x4edbbb80, -0x6ab198d9, 0x2699f331, 0x7f792f9d },
        { 0x5fd4d924, 0x0b886b92, 0x3626a80d, 0x60906f7a, -0x467542ee, -0x132c984c, -0x210cbb31, 0x2876beb1 }
    },
    {
        { 0x3a8a85f8, -0x2a6b4ccd, -0x187282a8, 0x4ea37689, 0x5e8e351f, 0x73bf9f45, -0x43be144c, 0x5507d7d2 },
        { 0x63144691, -0x237b16cb, -0x29e0dc0c, 0x632fe8a0, 0x12a9a8d5, 0x4caa8006, 0x0e9918d3, 0x48f9dbfa },
        { 0x299572fc, 0x1ceb2903, -0x6afd2f12, 0x7c8ccaa2, 0x11cce67b, -0x6e405bcc, 0x64a831e7, 0x57844819 }
    },
},
{
    {
        { 0x5fddc09c, -0x29302e11, -0x08a8a232, -0x17d4c103, 0x201634c2, 0x25d56b5d, 0x04ed2b9b, 0x3041c6bb },
        { 0x6768d593, -0x2583d4db, 0x4422ca13, -0x673e3fa9, -0x35f531e3, -0x0e57f42b, -0x3f775970, 0x29cdd1ad },
        { -0x26a91eb8, 0x0ff2f2f9, -0x60ca94d2, -0x5218688b, 0x5f6c025c, 0x1a4698bb, 0x14049a7b, 0x104bbd68 }
    },
    {
        { -0x29800e9d, -0x56a265a1, 0x4cc75681, -0x16d41963, -0x21df0da9, -0x4807fdb4, -0x04f8d20b, 0x204f2a20 },
        { 0x68f1ed67, 0x51f0fd31, -0x2790c43e, 0x2c811dcd, 0x04d2f2de, 0x44dc5c43, 0x092a7149, 0x5be8cc57 },
        { 0x30ebb079, -0x37ebc4c3, -0x429ad1d0, 0x7589155a, -0x7092a3cf, 0x653c3c31, -0x3d86e9e1, 0x2570fb17 }
    },
    {
        { 0x0bb8245a, 0x192ea955, -0x706faf2f, -0x37190458, -0x775b36cb, 0x7986ea2d, -0x21fe7998, 0x241c5f91 },
        { 0x2cb61575, 0x3efa367f, 0x1cd6026c, -0x0a06908a, 0x65b52562, -0x1738ebd6, 0x53030acd, 0x3dcb65ea },
        { 0x40de6caa, 0x28d81729, 0x22d9733a, -0x7040d310, 0x235b01d1, 0x16d7fcdd, 0x5fcdf0e5, 0x08420edd }
    },
    {
        { 0x04f410ce, 0x0358c34e, 0x276e0685, -0x49eca4a6, -0x1446eadf, 0x5d9670c7, 0x21db889c, 0x04d654f3 },
        { -0x7c9d05b6, -0x3200df55, -0x1de5c192, 0x57e118d4, -0x03c619d5, -0x1ce869e9, -0x43e89603, 0x0d9a53ef },
        { -0x22424a2b, 0x5e7dc116, -0x725a22d3, 0x2954deb6, 0x3334a292, 0x1cb60817, 0x18991ad7, 0x4a7a4f26 }
    },
    {
        { -0x50c8d5b5, 0x24c3b291, 0x718147f2, -0x6c257d90, -0x7976610e, -0x227b7a9c, 0x23e0ee33, 0x4a963142 },
        { 0x5fb15f95, -0x0b58e7fe, 0x6b5c1b8f, 0x3df65f34, 0x00e01112, -0x32030f7b, -0x222ce7b8, 0x11b50c4c },
        { 0x08a4ffd6, -0x5917d8bc, -0x63ea8927, 0x738e177e, 0x3d02b3f2, 0x773348b6, -0x319433af, 0x4f4bce4d }
    },
    {
        { -0x3b62f491, 0x30e2616e, -0x3513dce9, -0x1ba98e71, -0x0d94b05a, 0x48eb409b, 0x61595f37, 0x3042cee5 },
        { -0x1ddbda7c, -0x58e031a6, -0x6d0a7562, 0x26ea7256, 0x1cea3cf4, -0x2de5f629, -0x48e3fe1a, 0x73fcdd14 },
        { 0x449bac41, 0x427e7079, -0x431dcef6, -0x7aa51c93, 0x5f841a7c, 0x4cae7621, -0x65631e2a, 0x389e740c }
    },
    {
        { 0x570eac28, -0x3642870a, 0x27919ce1, -0x1aa4f4ce, -0x5e646e13, 0x65fc3eab, -0x29d9c970, 0x25c425e5 },
        { 0x34dcb9ce, 0x64fcb3ae, -0x1cb72f53, -0x68affcdd, 0x62c6381b, 0x45b3f07d, 0x465a6788, 0x61545379 },
        { -0x0e282192, 0x3f3e06a6, -0x71f9dcf8, 0x3ef97627, 0x4e8a6c77, -0x73eb09da, 0x15484759, 0x6539a089 }
    },
    {
        { 0x14bb4a19, -0x223b242c, -0x67bdb072, 0x19b2bc3c, 0x36ca7169, 0x48a89fd7, -0x0fe64270, 0x0f65320e },
        { -0x3c2d088d, -0x162de08c, 0x25c46845, -0x3eafabbf, -0x064661cd, 0x624e5ce8, -0x3a32e794, 0x11c5e4aa },
        { -0x35021f3a, -0x2b792e4f, 0x163b5181, 0x4f3fe6e3, -0x050d6c66, 0x59a8af0d, -0x13ccf8d6, 0x4cabc7bd }
    },
},
{
    {
        { 0x1a54a044, -0x083f5e64, 0x77bd9fbb, 0x4a1c5e24, 0x5af22972, -0x591c35ef, 0x3f2e9e0d, 0x1819bb95 },
        { 0x532f7428, 0x16faa8fb, 0x46a4e272, -0x242bd160, -0x74615b80, 0x5337653b, 0x23973f03, 0x40659472 },
        { 0x5e042e84, 0x498fbb79, 0x7698b714, 0x7d0dd89a, 0x27fe6295, -0x7404f45c, 0x21200524, 0x36ba82e7 }
    },
    {
        { 0x57274ed5, -0x372962f6, 0x60804b17, 0x45ba8032, 0x2255dfac, -0x20c325f0, 0x2709b339, 0x77d22123 },
        { 0x4245ec41, -0x29f13449, 0x34348716, -0x02641762, -0x1bdd7b22, -0x36dbf502, -0x2face24c, 0x4472f648 },
        { 0x64ad94d8, 0x498a6d70, -0x6509dd9d, -0x5a4a3703, 0x45c141f4, -0x735712fb, 0x662d358c, 0x2c63bec3 }
    },
    {
        { -0x7a790741, -0x65ae74c6, -0x344e6910, -0x6118e50a, -0x5dc7a30e, -0x55f9da1a, -0x2228372f, 0x1deb2176 },
        { -0x158786ab, 0x7fe60d8b, -0x4a0bfe49, -0x4623ee82, 0x19355cce, -0x6e383f66, -0x6bbd4121, 0x22692ef5 },
        { 0x2066cf6c, -0x7a9c2e66, 0x4dcc7cd7, 0x401bfd8c, -0x32f2709e, -0x26895942, -0x5d874fa2, 0x67cfd773 }
    },
    {
        { 0x5a4e586a, 0x2d5fa985, 0x49beab7e, 0x65f8f7a4, -0x0de2cc2d, -0x55f8b223, 0x1bcb9dee, 0x185cba72 },
        { -0x10c11b8b, -0x7213ce06, -0x61dd026e, -0x66240076, 0x4e26cab1, 0x512d1159, -0x13bcef47, 0x0cde561e },
        { -0x0b1c34bf, -0x6c79625d, 0x40f7977e, -0x40fc6d0b, -0x2fb9c47d, 0x026204fc, -0x61139113, 0x3ec91a76 }
    },
    {
        { -0x4f5cbfd1, 0x0fad2fb7, -0x04960b58, 0x46615ecb, -0x3a07155a, -0x08ba4338, 0x4a94e896, 0x7a5fa879 },
        { -0x087e9953, 0x1e9df75b, -0x14f32851, 0x4dfda838, -0x3e150678, -0x45ffd128, 0x11f33cfc, 0x13fedb3e },
        { 0x13cd67a1, 0x52958faa, -0x74244ae9, -0x69a11f7f, 0x2e8845b3, 0x16e58daa, 0x5499da8f, 0x357d397d }
    },
    {
        { 0x194bfbf8, 0x481dacb4, -0x451a7d67, 0x4d77e3f1, 0x7d1372a0, 0x1ef4612e, 0x70ff69e1, 0x3a8d867e },
        { -0x4f453194, 0x1ebfa05f, 0x1caf9a1e, -0x36cb9df4, 0x1d82b61a, -0x3388e33c, -0x5a08b014, 0x2d94a16a },
        { 0x55aff958, 0x6f58cd5d, 0x75567721, -0x45c155a4, -0x6e9add83, 0x75c12399, -0x3d0d4ca2, 0x69be1343 }
    },
    {
        { 0x684b8de3, -0x7d444254, 0x3fca0718, -0x5d0b3830, -0x1f695558, 0x337f92fb, 0x63587376, 0x200d4d8c },
        { -0x1e6836d6, 0x0e091d5e, 0x2945119f, 0x4f51019f, -0x0fcb1664, 0x143679b9, 0x4d24c696, 0x7d88112e },
        { 0x4893b32b, 0x208aed4b, -0x41a6469c, 0x3efbf23e, -0x245a1af9, -0x289d2150, -0x7e42626c, 0x69607bd6 }
    },
    {
        { -0x6cdc56fe, 0x3b7f3bd4, 0x6b2c6e53, 0x7c21b556, 0x3a7852a7, -0x1a45700b, -0x7c713200, 0x28bc77a5 },
        { 0x68de1ce1, -0x0941fdf0, 0x0edcbc1f, -0x172ae719, 0x1b5505a5, -0x1c100230, -0x2c13c030, 0x35f63353 },
        { -0x1da27fca, 0x63ba78a8, -0x6bcccb70, 0x63651e00, 0x288ce532, 0x48d82f20, 0x36b57524, 0x3a31abfa }
    },
},
{
    {
        { 0x3f78d289, -0x3f708771, -0x5ebfb261, -0x01cf58d4, -0x309a3363, -0x0d887404, 0x5acb2021, 0x7ee49816 },
        { 0x089c0a2e, 0x239e9624, 0x3afe4738, -0x38b73b40, 0x764fa12a, 0x17dbed2a, 0x321c8582, 0x639b93f0 },
        { -0x6eee5e3d, 0x7bd508e3, -0x7f6f8b77, 0x2b2b90d4, -0x518d02e7, -0x182d513e, -0x7a49fd5a, 0x0edf493c }
    },
    {
        { -0x7b89beed, 0x6767c4d2, -0x080a07cb, -0x5f6fbfc1, -0x35194122, 0x1c8fcffa, -0x2e205c97, 0x04c00c54 },
        { 0x599b5a68, -0x51337ea8, -0x14521df2, -0x15a8b0f1, 0x22b67f07, 0x4fe41d74, 0x019d4fb4, 0x403b92e3 },
        { -0x74b9a308, 0x4dc22f81, 0x1480eff8, 0x71a0f35a, 0x04c7d657, -0x51174053, -0x4d9e890c, 0x355bb12a }
    },
    {
        { 0x5a8c7318, -0x5cfe2539, -0x4c3155ef, -0x126ffc63, 0x3bae3f2d, 0x6f077cbf, -0x1fad5272, 0x7518eaf8 },
        { 0x7493bbf4, -0x58e19b34, -0x135c4f3d, -0x1a427b27, -0x05fa187b, 0x0a6bc50c, 0x182ec312, 0x0f9b8132 },
        { 0x1b7f6c32, -0x5b77a63c, -0x0bc7cd68, 0x0f2d60bc, -0x364e2e27, 0x1815a929, -0x44e8aa3c, 0x47c3871b }
    },
    {
        { -0x37af9950, -0x0419a2b0, -0x4c5d6650, 0x62ecc4b0, 0x441ae8e0, -0x1ac8ab16, -0x172b72a1, 0x08fea02c },
        { 0x71ec4f48, 0x51445397, -0x3673a292, -0x07fa4e83, 0x47c3c66b, -0x089d3ee6, 0x764699dc, 0x00b89b85 },
        { 0x68deead0, -0x7db2228a, 0x4b685d23, -0x379bbae0, 0x5d89d665, -0x4aeb3033, 0x4f75d537, 0x473829a7 }
    },
    {
        { -0x52c6fd37, 0x23d9533a, -0x10fca771, 0x64c2ddce, -0x301ed04c, 0x15257390, 0x44e4d390, 0x6c668b4d },
        { 0x4679c418, -0x7d2d258b, -0x4d9e7210, -0x19c42828, -0x53b814f6, 0x355eef24, 0x4833c6b4, 0x2078684c },
        { 0x7a78820c, 0x3b48cf21, -0x7ed8c169, -0x0895f54e, -0x73711285, -0x56939a59, 0x4f8a433f, 0x7411a605 }
    },
    {
        { 0x18b175b4, 0x579ae53d, -0x0c6d5efe, 0x68713159, 0x1eef35f5, -0x7baa1346, 0x458c398f, 0x1ec9a872 },
        { -0x46623793, 0x4d659d32, 0x603af115, 0x044cdc75, -0x233d1b78, -0x4cb38ed4, -0x047ecb01, 0x7c136574 },
        { 0x00a2509b, -0x47195b2c, 0x0bc882b4, -0x647e28fe, -0x0e6a8a9f, 0x57e7cc9b, -0x38329ba0, 0x3add88a5 }
    },
    {
        { 0x59393046, -0x7a3d672c, 0x5ff659ec, -0x7081ca68, -0x0d0991c6, 0x1d2ca22a, -0x5bf958e0, 0x61ba1131 },
        { -0x49ca230e, -0x5476a890, -0x0993e044, 0x02dfef6c, -0x41492e79, -0x7aacfd98, -0x3378618c, 0x249929fc },
        { 0x16959029, -0x5c2f5f0f, -0x45814277, 0x023b6b6c, 0x26783307, 0x7bf15a3e, -0x44271319, 0x5620310c }
    },
    {
        { 0x77e285d6, 0x6646b5f4, 0x6c8f6193, 0x40e8ff67, -0x544a6b23, -0x59138cef, 0x658cec4d, 0x7ec846f3 },
        { 0x4934d643, 0x52899343, -0x5aeddd0b, -0x462407fa, -0x3c0be3de, -0x70927871, 0x4d9d9730, 0x37676a2a },
        { 0x1da22ec7, -0x64a170c1, 0x6c01cd13, 0x130f1d77, -0x5d676048, 0x214c8fcf, 0x399b9dd5, 0x6daaf723 }
    },
},
{
    {
        { 0x2cd13070, -0x7e514423, -0x07a5f162, -0x69d1bcdb, -0x35200135, -0x216c6e56, 0x52c230e6, 0x53177fda },
        { 0x10628564, 0x591e4a56, -0x574b20cc, 0x2a4bb87c, -0x185c71bd, -0x21d5da8e, -0x011afb92, 0x3cbdabd9 },
        { 0x50b9de79, -0x584368fa, -0x3cfe4a65, 0x3d12a7fb, -0x2c951c74, 0x02652e68, 0x5a6199dc, 0x79d73983 }
    },
    {
        { 0x0d591737, 0x21c9d992, -0x164b932a, -0x6415be2e, 0x0d89bfca, -0x1df17be0, 0x6eae5ff8, 0x79d99f94 },
        { 0x4131c1bd, -0x26cab20a, -0x7913a7de, 0x758094a1, -0x1ba60c3e, 0x4464ee12, -0x34eccd7e, 0x6c11fce4 },
        { 0x68673205, -0x0e84b7cb, 0x3caad96c, 0x387deae8, 0x56ffe386, 0x61b471fd, -0x48ba5a67, 0x31741195 }
    },
    {
        { 0x3b02a047, 0x17f8ba68, -0x01104938, 0x50212096, 0x1556cbe2, 0x70139be2, 0x1d98915b, 0x203e44a1 },
        { -0x4885c9f5, -0x172efe70, -0x666a18fe, -0x66467ce0, -0x05fdb856, -0x42b02008, -0x1f2c9579, 0x2772e344 },
        { 0x37b9e39f, -0x2979c146, 0x723b5a23, 0x105bc169, -0x59a3f89e, 0x104f6459, 0x5b4d38d4, 0x56795129 }
    },
    {
        { 0x0d4b497f, 0x07242eb3, -0x46433379, 0x1ef96306, -0x27ee90bb, 0x37950934, 0x01405b04, 0x05468d62 },
        { 0x13037524, 0x535fd606, -0x4f043d96, -0x1def520a, 0x23e990ae, -0x5372f565, -0x28d02407, 0x47204d08 },
        { -0x06cd9822, 0x00f565a9, -0x3f2a7176, -0x31302873, -0x0ce71d72, -0x5dea1d24, -0x649cccae, 0x4599ee91 }
    },
    {
        { -0x79e51a87, -0x538b9295, -0x09515624, 0x31ab0650, 0x40256d4c, 0x241d6611, 0x3d21a5de, 0x2f485e85 },
        { 0x70e0e76b, -0x2c3ddf36, -0x1560cf6c, -0x4ed415a8, -0x3cd8ed7e, 0x294ddec8, -0x5e2e2fd8, 0x0c3539e1 },
        { -0x63f7cc0d, 0x32974483, -0x2d543b7c, 0x6fe6257f, 0x4b358817, 0x5327d181, -0x76c01644, 0x65712585 }
    },
    {
        { -0x28f711c1, -0x7e3d60e5, -0x519bf830, -0x2234a5fb, -0x2d5c1459, -0x68513e29, -0x6e2af7cf, 0x1590521a },
        { 0x32a61161, -0x63efd049, 0x34d520a8, -0x1b71ef23, 0x6f9a9176, 0x365c6354, 0x046f6006, 0x32f6fe4c },
        { -0x386ef534, 0x40a3a11e, -0x0e92d852, -0x6fec2008, -0x544e6a2c, 0x1a9720d8, 0x2ea98463, 0x1bb9fe45 }
    },
    {
        { -0x33c98b84, -0x30a1936b, 0x6b0bc30d, 0x29420153, -0x11868510, 0x453ac67c, 0x2a8bb3c9, 0x5eae6ab3 },
        { -0x4c2ab062, -0x162e26b0, -0x1ff2cc3f, 0x2d5f9cbe, -0x5fb03954, 0x51c2c656, 0x3c1cbcc9, 0x65c091ee },
        { 0x14f118ea, 0x70836611, -0x6bcb6353, 0x2b37b87b, -0x4b1660c0, 0x7273f51c, 0x23d75698, 0x78a2a958 }
    },
    {
        { 0x5ef83207, -0x4b0dc3be, -0x3656cb4b, -0x54076b2d, 0x39fd87f7, -0x2f8f73ed, 0x17166130, 0x18767891 },
        { 0x5c8c2ace, -0x5d4f8d17, 0x651e9c4b, 0x69cffc96, 0x42e7b42b, 0x44328ef8, 0x22aadeb3, 0x5dd996c1 },
        { 0x670c507c, -0x6da4a110, -0x46c3cc41, -0x7e6437be, 0x70dd003f, 0x10792e9a, 0x6e28dc74, 0x59ad4b7a }
    },
},
{
    {
        { -0x5352715e, 0x583b04bf, 0x148be884, 0x29b743e8, 0x0810c5db, 0x2b1e583b, -0x714c4456, 0x2b5449e5 },
        { -0x14c241b9, 0x5f3a7562, -0x71425f48, -0x0815c7ac, 0x45747299, 0x00c3e531, 0x1627d551, 0x1304e9e7 },
        { 0x6adc9cfe, 0x789814d2, -0x74b722f5, 0x3c1bab3f, -0x068639f6, -0x25f01e01, 0x7c2dd693, 0x4468de2d }
    },
    {
        { -0x079cf832, 0x4b9ad8c6, 0x435d0c28, 0x21113531, 0x657a772c, -0x2b57993b, 0x63247352, 0x5da6427e },
        { -0x6be6b962, 0x51bb355e, 0x23ddc754, 0x33e6dc4c, 0x447f9962, -0x6c5a492a, -0x04bb429d, 0x6cce7c6f },
        { -0x2153dd36, 0x1a94c688, -0x4451e008, -0x46f99109, -0x72a6a7f1, -0x775273c8, -0x1860d358, 0x58f29abf }
    },
    {
        { 0x710ecdf6, 0x4b5a64bf, 0x462c293c, -0x4eb31ac8, -0x2af4c547, 0x3643d056, 0x185b4870, 0x6af93724 },
        { -0x7218c198, -0x16f13055, 0x377e76a5, 0x54036f9f, -0x41fea67e, -0x0fb6a4f5, -0x580be1ca, 0x577629c4 },
        { 0x09c6a888, 0x32200245, 0x4b558973, -0x2d1fc9ed, 0x3c33289f, -0x7c1dc9dd, 0x0caec18f, 0x701f25bb }
    },
    {
        { 0x7cbec113, -0x62e70927, 0x74bfdbe4, -0x7bb5f91a, -0x53b19f2a, 0x20f5b522, 0x50955e51, 0x720a5bc0 },
        { -0x1b9e9313, -0x3c574f08, -0x61da5783, -0x08ff99f2, -0x0b435a64, 0x61e3061f, -0x423bf417, 0x2e0c92bf },
        { -0x647fa5cb, 0x0c3f0943, 0x6242abfc, -0x17b174c9, 0x5c229346, 0x691417f3, 0x144ef0ec, 0x0e9b9cbb }
    },
    {
        { 0x5db1beee, -0x7211642b, 0x0a723fb9, -0x363c54c9, 0x1c68d791, 0x44a8f1bf, 0x1cfd3cde, 0x366d4419 },
        { -0x04a8df53, -0x04452b71, -0x2406f2f2, -0x117e6e95, 0x635543bf, -0x2b7eceae, 0x3f337bd8, 0x221104eb },
        { -0x0d4373ec, -0x61c3e8bd, -0x4a7a93c5, 0x2eda26fc, 0x68a7fb97, -0x3347d0f2, -0x43a6cdbc, 0x4167a4e6 }
    },
    {
        { -0x07317012, -0x3d41d99b, -0x177f29d4, -0x169800ec, 0x2f364eee, -0x0ed19182, -0x34812d0a, 0x34b33370 },
        { 0x76f62700, 0x643b9d28, 0x0e7668eb, 0x5d1d9d40, 0x21fc0684, 0x1b4b4303, 0x2255246a, 0x7938bb7e },
        { -0x797e2934, -0x323a6e12, -0x127a58ad, -0x31fdef64, 0x58808883, -0x128b7a3f, 0x2dfe65e4, 0x1176fc6e }
    },
    {
        { 0x49770eb8, -0x246f1d77, -0x530bbf5d, -0x670433d6, -0x21287865, 0x21354ffe, -0x0d96f94a, 0x1f6a3e54 },
        { 0x5b9c619b, -0x4b509330, -0x4d5a7b80, 0x2ddfc9f4, -0x1416b23c, 0x3d4fa502, 0x677d5f34, 0x08fc3a4c },
        { -0x2cf8cb16, 0x60a4c199, 0x31165cd6, 0x40c085b6, -0x08a67d6b, -0x1dccc1dd, 0x16b900d1, 0x4f2fad01 }
    },
    {
        { -0x48c449c8, -0x69d326e3, -0x03ed63f8, -0x19fa8856, -0x0c49e977, 0x6f619b39, 0x2944ee81, 0x3451995f },
        { -0x6b51b1ac, 0x44beb241, 0x1857ef6c, 0x5f541c51, 0x368d0498, -0x59e194d3, -0x68d10855, 0x445484a4 },
        { -0x60158284, -0x6ead0330, -0x4f6ca30a, 0x4a816c94, 0x47285c40, 0x258e9aaa, 0x042893b7, 0x10b89ca6 }
    },
},
{
    {
        { 0x79d34aa0, -0x2983212a, -0x33b24c61, -0x33f46140, -0x1ca2e6f1, -0x5aca5baa, -0x09e09011, 0x2e05d9ea },
        { 0x3b646025, -0x64d5bd92, 0x385ce4cf, 0x32127190, -0x229215bb, -0x5da3003e, -0x4157218b, 0x06409010 },
        { -0x29e414a7, -0x3bb86fe6, -0x1a2377f6, 0x661f19bc, -0x483597d9, 0x24685482, -0x101f80da, 0x293c778c }
    },
    {
        { -0x5ee00e00, 0x16c795d6, -0x4ea7ea37, -0x348f2f1e, -0x64ac6a4b, -0x760d6ce0, 0x31e47b4f, 0x50b8c2d0 },
        { 0x07069096, -0x797f6190, -0x1b1afe77, -0x5528a4eb, -0x5de5feb9, 0x07f35715, 0x12815d5e, 0x0487f3f1 },
        { 0x068a4962, 0x48350c08, 0x51092c9a, 0x6ffdd053, -0x50903723, 0x17af4f4a, 0x3cdba58b, 0x4b0553b5 }
    },
    {
        { 0x27c152d4, -0x40fadee5, -0x42e509c7, 0x5ec26849, -0x71905468, 0x5e0b2caa, 0x50bd0840, 0x054c8bdd },
        { 0x1b32ff79, -0x639a0342, 0x03b50f9b, -0x148a1561, 0x6c07e606, -0x0312d594, 0x51717908, 0x35106cd5 },
        { 0x1dcf073d, 0x38a0b12f, -0x48095d8a, 0x4b60a8a3, -0x2cbfb066, -0x012a53db, 0x5505c229, 0x72e82d5e }
    },
    {
        { 0x69771d02, 0x00d9cdfd, 0x6cfbf17e, 0x410276cd, 0x1cb12ec7, 0x4c45306c, 0x27500861, 0x2857bf16 },
        { -0x0f27bb38, 0x6b0b697f, -0x268634b7, -0x44ed07a4, -0x3e25f0e1, -0x2d5abe3a, 0x58ce7211, 0x7b7c2429 },
        { 0x0101689e, -0x60de6fc1, -0x4079effb, -0x2886202d, 0x3deb0f1b, -0x5edd11a1, 0x485a00d4, 0x510df84b }
    },
    {
        { -0x38f53ea2, 0x24b3c887, -0x047e48ce, -0x4f0c5aa9, -0x1a8733e5, -0x64d321d1, 0x03b54f8e, 0x4cf7ed07 },
        { -0x6d885e06, -0x5abecc45, 0x63991237, 0x74ec3b62, 0x35d2f15a, 0x1a3c54dc, -0x1b7d45c6, 0x2d347144 },
        { -0x670411f1, 0x6bd47c65, -0x54aa41d3, -0x61b8cc1e, 0x127610c5, 0x1093f624, -0x2f5e155c, 0x4e05e26a }
    },
    {
        { -0x1e701940, 0x1833c773, -0x2c378d9b, -0x1c3b8ee6, 0x0116b283, 0x3bfd3c4f, -0x4b32b248, 0x1955875e },
        { 0x4b531f20, -0x2564949e, 0x77509abb, 0x429a760e, -0x17dc3480, -0x24160ade, -0x77f3707e, 0x618f1856 },
        { 0x0e399799, 0x6da6de8f, 0x40fda178, 0x7ad61aa4, 0x5e3563dd, -0x4cd327f0, 0x2ae340ae, 0x15f6beae }
    },
    {
        { -0x6dba1deb, -0x4565f085, -0x2673f245, -0x0c979ed3, -0x0ddf4fe0, 0x2e84e4cb, 0x62d90eda, 0x6ba92fe9 },
        { 0x31ec3a62, -0x79d434f4, 0x1138f3c2, -0x7ef1d4bb, 0x39dac2a4, 0x788ec4b8, -0x51d56d7f, 0x28f76867 },
        { 0x5884e2aa, 0x3e4df965, -0x242b9a5b, -0x429d0425, 0x0de9e524, -0x28a69356, -0x4d4e4c29, 0x6e8042cc }
    },
    {
        { 0x16521f7e, 0x15306536, -0x69dfc246, 0x660d06b8, 0x545f0879, 0x2d3989bc, 0x78ebd7b0, 0x4b5303af },
        { -0x31d73592, -0x0ef2c3d7, -0x0349f6c3, -0x452cbac0, -0x5d15d2c1, -0x18bd9129, 0x4ff298b9, 0x08af9d4e },
        { -0x41434218, 0x72f8a6c3, -0x23c57177, 0x4f0fca4a, -0x38402086, 0x6fa9d4e8, -0x649db149, 0x0dcf2d67 }
    },
},
{
    {
        { 0x5a45f06e, 0x753941be, 0x6d9c5f65, -0x2f835113, 0x72ff51b6, 0x11776b9c, -0x10f2b257, 0x17d2d1d9 },
        { -0x68e7d764, 0x3d594749, 0x24533f26, 0x12ebf8c5, 0x14c3ef15, 0x0262bfcb, 0x77b7518e, 0x20b878d5 },
        { 0x073f3e6a, 0x27f2af18, -0x28adef97, -0x02c01ae7, 0x3ca60022, 0x22e3b72c, -0x339a3959, 0x72214f63 }
    },
    {
        { -0x0bc4d637, 0x1d9db7b9, 0x4f518f75, -0x29fa7db6, 0x312f9dc4, -0x0d3f8d43, 0x5a1545b0, 0x1f24ac85 },
        { 0x5307a693, -0x4b1c80c0, 0x2f336795, -0x5458eb29, 0x73761099, -0x29042f59, -0x7e8e3437, 0x5fdf48c5 },
        { -0x716afa56, 0x24d60832, 0x0c1420ee, 0x4748c1d1, 0x06fb25a2, -0x38001ba4, 0x2ae395e6, 0x00ba739e }
    },
    {
        { -0x157744da, -0x51bbd90b, -0x7b68c405, 0x360679d9, 0x26694e50, 0x5c9f030c, -0x2ae72dda, 0x72297de7 },
        { 0x5c8790d6, 0x592e98de, 0x45c2a2df, -0x1a40482d, -0x064b66de, 0x115a3b60, 0x67ad78f3, 0x03283a3e },
        { -0x41f346c7, 0x48241dc7, -0x749ccf80, 0x32f19b4d, 0x02289308, -0x2c2036f3, 0x46271945, 0x05e12968 }
    },
    {
        { 0x242c4550, -0x52404438, -0x2fcf7e27, -0x4337f314, -0x0a37206e, -0x7bca995a, -0x7da731b4, 0x78cf25d3 },
        { 0x2d9c495a, -0x457d114d, -0x0ed44684, -0x31103704, -0x6c4a2e20, -0x4fd25452, 0x13698d9b, 0x39c00c9c },
        { 0x31489d68, 0x15ae6b8e, -0x63d40f79, -0x557ae355, -0x0fb105fb, -0x3658a569, 0x6b3ff832, 0x006b5207 }
    },
    {
        { -0x4631f7d3, -0x0a3481ea, 0x417abc29, 0x3407f14c, 0x2bf4a7ab, -0x2b4c9432, 0x1a9f75ce, 0x7de2e956 },
        { -0x626a87e4, 0x29e0cfe1, -0x699cef1e, -0x497e20e8, 0x70516b39, 0x57df39d3, 0x3bc76122, 0x4d57e344 },
        { -0x495aa135, -0x218f2b0c, 0x5d85db99, 0x4801527f, -0x2c11657f, -0x24363bc0, 0x1a6029ed, 0x6b2a90af }
    },
    {
        { 0x5bb2d80a, 0x77ebf324, 0x2fb9079b, -0x27cfe4b9, 0x4cee7333, -0x39b8190e, 0x276c2109, 0x465812c8 },
        { -0x6519e169, 0x6923f4fc, -0x1fc0a02f, 0x5735281d, -0x19122ed3, -0x589b51bd, -0x2ed2c1b6, 0x5fd8f4e9 },
        { 0x2a1062d9, 0x4d43beb2, 0x3831dc16, 0x7065fb75, -0x21d69729, 0x180d4a7b, 0x1cb16790, 0x05b32c2b }
    },
    {
        { 0x7ad58195, -0x08035bd4, 0x4333f3cc, 0x3214286e, 0x340b979d, -0x493d62f3, 0x567307e1, 0x31771a48 },
        { -0x2db25703, -0x373fa134, 0x05dfef83, -0x5e30e554, 0x7df9cd61, -0x2441100e, 0x7b471e99, 0x3b5556a3 },
        { -0x1eb22b7e, 0x32b0c524, 0x1a2ba4b6, -0x124caeac, 0x282b5af3, -0x5c2e9fb8, 0x7a7336eb, 0x4fc079d2 }
    },
    {
        { 0x0c86c50d, -0x23cb74bc, -0x336b19af, 0x1337cbc9, 0x643e3cb9, 0x6422f74d, -0x451c32f8, 0x241170c2 },
        { -0x7640d081, 0x51c938b0, 0x02dfe9a7, 0x2497bd65, 0x7880e453, -0x00003f64, -0x3506716e, 0x124567ce },
        { 0x0ac473b4, 0x3ff9ab86, 0x0113e435, -0x0f6ee212, -0x14393b51, 0x4ae75060, 0x6c87000d, 0x3f861296 }
    },
},
{
    {
        { 0x638c7bf3, 0x529fdffe, 0x388b4995, -0x20d461a0, 0x1bad0249, -0x1fd84cb1, -0x46058b13, 0x7bc92fc9 },
        { -0x086a841c, 0x0c9c5303, -0x1f7a3ebb, -0x5c3ce5e0, -0x2f7affb0, -0x4f8de28f, -0x54f40d26, 0x0aba390e },
        { -0x7fe52607, -0x606810d2, 0x79afda3a, -0x7c9682ac, -0x42a694b0, -0x16f94c01, -0x22c04720, 0x02672b37 }
    },
    {
        { 0x398ca7f5, -0x116458d7, 0x7a4849db, -0x146359db, 0x7ec544e1, 0x29eb29ce, -0x08c91d38, 0x232ca21e },
        { 0x260885e4, 0x48b2ca8b, -0x7d4cb3e4, -0x5bd79414, 0x17f58f74, -0x6c81e5da, -0x54d35d5b, 0x741d1fcb },
        { 0x253fcb17, -0x409ebdc3, -0x05c614ec, 0x08803cea, -0x67ae3851, -0x0e79fd21, 0x49e3414b, 0x0400f3a0 }
    },
    {
        { -0x5f9184fa, 0x2efba412, 0x2c8d2560, 0x14678545, -0x29856e39, -0x2068ec15, 0x157eadf3, 0x32830ac7 },
        { -0x459e3aa5, -0x5431fb8a, -0x3b2c68ea, 0x36a3d6d7, -0x1727d2f7, 0x6eb259d5, -0x7b28a905, 0x0c9176e9 },
        { -0x48c89618, 0x0e782a7a, 0x75b18e2c, 0x04a05d78, -0x1433151f, 0x29525226, -0x7c1457e0, 0x0d794f83 }
    },
    {
        { -0x585d1e54, 0x7be44ce7, -0x052e4749, 0x411fd93e, 0x0d5f7c9b, 0x1734a1d7, 0x3127db16, 0x0d659223 },
        { -0x61eae90c, -0x00ca0a35, 0x648aae45, -0x117fa431, -0x46c5610d, -0x0f28c3d5, 0x2092a6c2, 0x097b0bf2 },
        { 0x21a9d733, -0x3b7454eb, -0x29e544db, -0x593d1516, -0x3934bcfb, 0x625c6c1c, -0x6c14c599, 0x7fc90fea }
    },
    {
        { -0x63834dc3, -0x3ad8214b, 0x5328404e, -0x6aac6e97, 0x7ccf2c7a, -0x29bc6d7f, -0x082705ef, 0x6ce97dab },
        { 0x1f5c5926, 0x0408f1fe, 0x3b258bf4, 0x1a8f2f5e, -0x0238e997, 0x40a951a2, -0x3674a882, 0x6598ee93 },
        { 0x0ef7c48f, 0x25b5a8e5, 0x6f2ce532, -0x149fcbef, -0x1ac21ac9, -0x3a18ae8d, -0x73ed44fd, 0x73119fa0 }
    },
    {
        { 0x21f4774d, 0x7845b94d, 0x7897b727, -0x409d0e94, 0x3c56522b, 0x671857c0, -0x6a9dedee, 0x3cd6a852 },
        { 0x53f1a4cb, -0x12cfed6c, -0x370ac879, -0x4319de37, 0x38bee7b9, -0x0534d4ed, -0x6157bd74, 0x3025798a },
        { 0x3aeca999, 0x3fecde92, 0x62e8c12f, -0x4255a500, -0x69677522, 0x67b99dfc, 0x52661036, 0x3f52c028 }
    },
    {
        { -0x113be93a, -0x6da74067, -0x562d098f, -0x5375afe9, 0x16dea4ab, 0x629549ab, -0x66f6ea97, 0x05d0e85c },
        { 0x2a1351c6, -0x00155b72, -0x0580ac29, 0x28624754, 0x7582ddf1, 0x0b5ba9e5, -0x596953a7, 0x60c0104b },
        { -0x21634169, 0x051de020, -0x4af4308c, -0x05f803aa, 0x0f11df65, 0x378cec9f, -0x546921b3, 0x36853c69 }
    },
    {
        { -0x053a1842, 0x4433c0b0, 0x4c08dcbe, 0x724bae85, 0x46978f9b, -0x0e0db33c, 0x62825fc8, 0x4a0aff6d },
        { 0x78f39b2d, 0x36d9b8de, -0x57b84614, 0x7f42ed71, 0x79bd3fde, 0x241cd1d6, -0x6d043195, 0x6a704fec },
        { 0x61095301, -0x16e80462, 0x02a092f8, -0x3efd206c, -0x0599e6f5, -0x40f61d0b, -0x1f2301c9, 0x681109be }
    },
},
{
    {
        { 0x36048d13, -0x63e70306, 0x73899ddd, 0x29159db3, -0x606d2f56, -0x2360caf5, -0x7875e62c, 0x26f57eee },
        { 0x782a0dde, 0x559a0cc9, -0x158e7c7b, 0x551dcdb2, 0x31ef238c, 0x7f62865b, 0x7973613d, 0x504aa776 },
        { 0x5687efb1, 0x0cab2cd5, 0x247af17b, 0x5180d162, 0x4f5a2467, -0x7a3ea5cc, -0x6245cf97, 0x4041943d }
    },
    {
        { -0x5d935523, 0x4b217743, 0x648ab7ce, 0x47a6b424, 0x03fbc9e3, -0x34e2b086, -0x67ff2fe7, 0x12d93142 },
        { 0x43ebcc96, -0x3c3f1146, 0x26ea9caf, -0x728b6364, 0x1c77ccc6, -0x26056a12, 0x7684340f, 0x1420a1d9 },
        { -0x2cc8a6b1, 0x00c67799, -0x4dc55b85, 0x5e3c5140, -0x1ca00c6b, 0x44182854, 0x4359a012, 0x1b4f9231 }
    },
    {
        { -0x5b67994f, 0x33cf3030, 0x215f4859, 0x251f73d2, 0x51def4f6, -0x547d55c0, 0x6f9a23f6, 0x5ff191d5 },
        { -0x76eaf6af, 0x3e5c109d, 0x2de9696a, 0x39cefa91, -0x68a0cfe0, 0x20eae43f, 0x7f132dae, 0x239b572a },
        { -0x53d26f98, -0x7e612bcd, 0x5fc98523, 0x2883ab79, 0x5593eb3d, -0x10ba8d80, 0x758f36cb, 0x020c526a }
    },
    {
        { -0x0fbd3377, -0x16ce10a7, -0x71edb44a, 0x2c589c9d, -0x5138a669, -0x52371e76, 0x5602c50c, 0x452cfe0a },
        { -0x61272444, 0x779834f8, -0x23835b94, -0x370d5507, -0x5c1e4f8c, -0x56adb324, 0x15313877, 0x02aacc46 },
        { 0x647877df, -0x795f0860, 0x0e607c9f, -0x443b9bd9, -0x0e04ee37, -0x54e815db, 0x304b877b, 0x4cfb7d7b }
    },
    {
        { -0x687610ee, -0x1d79663e, -0x20a8e6f3, 0x2b6ecd71, -0x13368f30, -0x3cbc37a9, 0x434d3ac5, 0x5b1d4cbc },
        { -0x47648a02, 0x72b43d6c, -0x63952380, 0x54c694d9, 0x3ee34c9f, -0x473c55c9, 0x39075364, 0x14b4622b },
        { -0x33f560da, -0x4904d9eb, -0x4772331b, 0x3a4f0e2b, 0x3369a705, 0x1301498b, 0x58592dd1, 0x2f98f712 }
    },
    {
        { 0x4f54a701, 0x2e12ae44, -0x56342822, -0x0301c110, 0x75835de0, -0x314076f3, -0x189ebaac, 0x1d8062e9 },
        { -0x4af061aa, 0x0c94a74c, -0x7171ece0, 0x5b1ff4a9, -0x7dcff099, -0x65d533df, -0x27f95507, 0x3a6ae249 },
        { -0x566f83a6, 0x657ada85, -0x6e46f09e, 0x1a0ea8b5, -0x20cb4b17, -0x72f1e205, -0x510da00d, 0x298b8ce8 }
    },
    {
        { 0x0a2165de, -0x7c858d16, 0x0bcf79f6, 0x3fab07b4, 0x7738ae70, 0x521636c7, 0x03a7d7dc, 0x6ba62718 },
        { -0x1008f34e, 0x2a927953, 0x79157076, 0x4b89c92a, 0x30a7cf6a, -0x6be7ba86, 0x4d5ce485, 0x34b8a840 },
        { -0x7c96cccb, -0x3d91134b, 0x63b5fefd, -0x2a57ec21, -0x5b4dda8d, -0x5d6c5566, 0x465e1c6a, 0x71d62bdd }
    },
    {
        { -0x4e08a10b, -0x32d24a26, 0x16b065f5, -0x28806a31, 0x3f49f085, 0x14571fea, 0x262b2b3d, 0x1c333621 },
        { -0x2c872080, 0x6533cc28, 0x0a0fa4b4, -0x0924bc87, -0x08fe25a6, -0x1c9ba007, -0x0ce8d45c, 0x74d5f317 },
        { 0x67d9ca81, -0x57901aac, 0x2b298c37, 0x398b7c75, -0x1c539dc5, -0x2592f76e, 0x47e9d98c, 0x4aebcc45 }
    },
},
{
    {
        { -0x5fa65bbb, 0x0de9b204, 0x4b17ad0f, -0x1ea34b56, 0x1f79c557, -0x1e4413ae, -0x2f8ef7e5, 0x2633f1b9 },
        { 0x05d21a77, 0x53175a72, -0x2c46cb2c, -0x4f3fbbde, -0x22a21524, -0x52260db5, -0x60ef0074, 0x074f46e6 },
        { 0x018b9910, -0x3e04be89, 0x6c0fe140, -0x5915df24, 0x4354c6ff, -0x299e0c19, -0x0e5cbf86, 0x5ecb72e6 }
    },
    {
        { -0x17179669, -0x01151efa, -0x672f6c7d, -0x679ccc81, -0x55f91411, -0x6b8fb7f2, -0x2b3a3d30, 0x038b6898 },
        { 0x2259fb4e, -0x5aea5ce5, 0x2bcac52f, 0x0960f397, -0x72cbab35, -0x124ad014, -0x3b893fe7, 0x382e2720 },
        { -0x7531af5a, -0x0c6e3ae3, -0x51d2d6b8, 0x3142d0b9, 0x7f24ca80, -0x24b2a5e6, 0x59250ea8, 0x21aeba8b }
    },
    {
        { -0x0ff780dd, 0x53853600, -0x2582a87c, 0x4c461879, -0x4be097a0, 0x6af303de, -0x3d83e713, 0x0a3c16c5 },
        { -0x30bfaad0, 0x24f13b34, 0x43088af7, 0x3c44ea4a, 0x0006a482, 0x5dd5c517, -0x76f4f793, 0x118eb8f8 },
        { -0x336b80c3, 0x17e49c17, -0x553e2d85, -0x3339125a, -0x4f0f71aa, -0x209f6d32, 0x2c67c36b, 0x4909b3e2 }
    },
    {
        { 0x706ff64e, 0x59a16676, 0x0d86a53d, 0x10b953dd, -0x31a3f46a, 0x5848e1e6, 0x12780c68, 0x2d8b78e7 },
        { 0x63fe2e89, -0x63637a16, 0x0e9412ec, -0x41e4506f, -0x79040185, -0x70845576, -0x10697494, 0x0fb17f9f },
        { -0x503c6fd5, 0x79d5c62e, -0x7617f8d8, 0x773a2152, -0x1efedf47, -0x3c7519c0, 0x7b2b1a6d, 0x09ae2371 }
    },
    {
        { -0x52cd4e30, 0x10ab8fa1, -0x1d8874dc, -0x165312e5, 0x373de90f, -0x577a9440, -0x225ac66a, 0x66f35ddd },
        { 0x4e4d083c, -0x4495e6d6, 0x0029e192, 0x34ace063, -0x55054515, -0x67dba5a7, -0x25680554, 0x6d9c8a9a },
        { 0x24997323, -0x2d826505, -0x090fe2d2, 0x1bb7e07e, -0x0ad13381, 0x2ba7472d, 0x646f9dc8, 0x03019b4f }
    },
    {
        { -0x194c2395, -0x50f64dec, -0x5282d09b, 0x3f7573b5, 0x100a23b0, -0x2fe62678, -0x74a3ca09, 0x392b63a5 },
        { 0x565345cd, 0x04a186b5, -0x433bee96, -0x111899f0, 0x78fb2a45, 0x689c73b4, 0x65697512, 0x387dcbff },
        { -0x63f83dfb, 0x4093addc, -0x0acd3c82, -0x3a9a41eb, 0x1583402a, 0x63dbecfd, -0x10d1fcd2, 0x61722b4a }
    },
    {
        { -0x7e34f1c4, -0x294f85ab, -0x26bbb697, 0x290ff006, 0x16dcda1f, 0x08680b6a, 0x5a06de59, 0x5568d2b7 },
        { -0x1342b851, 0x0012aafe, 0x1cd46309, 0x55a266fb, 0x0967c72c, -0x0dfc1498, -0x35c3ebd7, 0x39633944 },
        { 0x1b37cfe1, -0x72f34774, 0x053818f3, 0x05b6a5a3, -0x487826a7, -0x0d1643fc, -0x6522809c, 0x6beba124 }
    },
    {
        { 0x43f5a53b, 0x5c3cecb9, 0x06c08df2, -0x633659e3, -0x7a76abb9, -0x30459c66, 0x0df09fd5, 0x5a845ae8 },
        { -0x5a4e4ebd, 0x1d06005c, 0x7fd1cda2, 0x6d4c6bb8, 0x53fcffe7, 0x6ef59676, -0x3e31e15b, 0x097c29e8 },
        { 0x5deb94ca, 0x4ce97dbe, -0x738f63b8, 0x38d0a438, -0x5e962f69, -0x3bc1312c, -0x081a783d, 0x0a1249ff }
    },
},
{
    {
        { 0x7354b610, 0x0b408d9e, 0x5ba85b6e, -0x7f94cdad, 0x4a58a207, -0x2419c5fd, -0x365e20d4, 0x173bd9dd },
        { 0x276d01c9, 0x12f0071b, -0x793b7390, -0x1847453b, 0x71d6fba9, 0x5308129b, 0x5a3db792, 0x5d88fbf9 },
        { -0x01a78d21, 0x2b500f1e, -0x2bc6e73f, 0x58d6582e, -0x3698c520, -0x1912d872, -0x4e615ce7, 0x06e1cd13 }
    },
    {
        { -0x61a4fcad, 0x472baf62, 0x278d0447, 0x3baa0b90, -0x69bc40d9, 0x0c785f46, -0x727c84ed, 0x7f3a6a1a },
        { 0x6f166f23, 0x40d0ad51, 0x1fab6abe, 0x118e3293, -0x5fb2f772, 0x3fe35e14, 0x26e16266, 0x30806035 },
        { 0x5d3d800b, -0x0819bbc7, -0x36fe120a, -0x6a572aab, 0x592c6339, 0x68cd7830, 0x2e51307e, 0x30d0fded }
    },
    {
        { 0x68b84750, -0x634b68e2, 0x6664bbcf, -0x5f6a8dd7, 0x72fa412b, 0x5c8de726, 0x51c589d9, 0x46150843 },
        { -0x0dedcc4d, -0x1fa6b2e6, -0x0f33b264, 0x1bdbe78e, -0x70b66589, 0x6965187f, 0x2c099868, 0x0a921420 },
        { -0x51465fd2, -0x436fe640, 0x16034cae, 0x55c7110d, 0x659932ec, 0x0e6df501, -0x6a35a202, 0x3bca0d28 }
    },
    {
        { -0x6133fe41, -0x6397714a, -0x59bb7691, -0x0f437c53, 0x5f7a9fe2, -0x35d26aa1, -0x720d7dbf, 0x4ea8b403 },
        { 0x3c5d62a4, 0x40f031bc, -0x300f85a0, 0x19fc8b3e, 0x130fb545, -0x67e7c25e, -0x5170ec33, 0x5631dedd },
        { -0x0e352dfe, 0x2aed460a, -0x5b73117d, 0x46305305, 0x49f11a5f, -0x6ede88bb, 0x542ca463, 0x24ce0930 }
    },
    {
        { -0x020cf47b, 0x3fcfa155, 0x36372ea4, -0x2d08e972, 0x6492f844, -0x4d1f9b22, 0x324f4280, 0x549928a7 },
        { -0x02f93efa, 0x1fe890f5, 0x5d8810f2, -0x4a3b97cb, 0x6e8caf3e, -0x7d87f702, -0x75f928b5, 0x41d4e3c2 },
        { 0x63ee1a2e, -0x0d91cd59, -0x2da00216, -0x516e1b49, -0x2e80b297, -0x43c42cc5, -0x3f230096, 0x491b66de }
    },
    {
        { -0x2f259b5f, 0x75f04a8e, 0x67e2284b, -0x12ddd351, 0x1f7b7ba4, -0x7dcb5c87, -0x48fe7499, 0x4cf6b8b0 },
        { -0x3815cd59, -0x670a4ec3, 0x7e16db98, -0x1c2a0734, -0x340726b9, -0x53f540ae, -0x37a11b54, 0x08f338d0 },
        { -0x66e58c43, -0x3c7c57df, -0x20cdf386, -0x54d843ff, -0x7b888f9d, -0x3ec2cce5, -0x14f87567, 0x530d4a82 }
    },
    {
        { 0x6c9abf9e, 0x6d697345, 0x4900a880, 0x257fb2fc, -0x373047b0, 0x2bacf412, 0x0cbfbd5b, 0x0db3e7e0 },
        { -0x1e06b7db, 0x004c3630, -0x7354aca6, 0x7e2d7826, -0x337b0075, -0x38b7dcdd, 0x101770b9, 0x65ea753f },
        { -0x1df69c9d, 0x3d66fc3e, 0x61b5cb6b, -0x7e29d381, 0x13443b1a, 0x0fbe0442, 0x21e1a1db, 0x02a4ec19 }
    },
    {
        { -0x0e3086a1, -0x0a379e9e, 0x26ee57f2, 0x118c8619, 0x1c063578, 0x17212485, -0x13f98031, 0x36d12b5d },
        { 0x3b24b8a2, 0x5ce6259a, 0x45afa0b8, -0x47a88534, -0x745f8fc9, -0x33341918, 0x127809bf, 0x3d143c51 },
        { 0x79154557, 0x126d2791, -0x0387c5f6, -0x2a1b70a4, -0x20e86454, 0x36bdb6e8, 0x5ba82859, 0x2ef51788 }
    },
},
{
    {
        { 0x7c6da1e9, 0x1ea43683, 0x1fb9bdbe, -0x063e7651, -0x31a22eab, 0x303001fc, -0x43a841ae, 0x28a7c99e },
        { -0x2ee1f2b6, -0x7742bc74, 0x43ccf308, 0x30cb610d, -0x6e6c8434, -0x1f65f1c9, 0x25b1720c, 0x4559135b },
        { -0x172e6163, -0x47026c67, -0x69dbdc01, -0x6f7e6e35, 0x47c742a3, -0x4d46b729, -0x2804bb3c, 0x37f33226 }
    },
    {
        { -0x37de4ee3, 0x33912553, 0x41e301df, 0x66ed42c2, 0x104222fd, 0x066fcc11, -0x3e6de971, 0x307a3b41 },
        { -0x4aa091f8, 0x0dae8767, 0x5b203a02, 0x4a43b3b3, -0x7f507387, -0x1c8da592, 0x705fa7a3, 0x0f7a7fd1 },
        { 0x6eb55ce0, -0x7114a2f9, -0x55f26da6, 0x2fc536bf, -0x23493918, -0x417e7cf1, -0x7d8450ae, 0x556c7045 }
    },
    {
        { 0x2bf44406, -0x46b46ffe, -0x006f4acc, -0x542bdc82, -0x050792c6, 0x7600a960, -0x3dcdd11d, 0x2f45abda },
        { 0x02e9d8b7, -0x71d4ae8d, 0x248714e8, -0x1c1add97, 0x4ca960b5, -0x42b04289, -0x3a135257, 0x6f4b4199 },
        { -0x37107596, 0x61af4912, 0x43fb6e5e, -0x1a705b02, 0x6fd427cf, -0x4a5033a3, 0x1e1e11eb, 0x6a539328 }
    },
    {
        { 0x149443cf, 0x0fff04fe, -0x79a32229, 0x53cac6d9, 0x531ed1b7, 0x31385b03, -0x532efc63, 0x5846a27c },
        { -0x5a2e1177, -0x0c25aec7, -0x006c9678, -0x7ebaba84, 0x00e188c4, 0x3f622fed, -0x2474a5c3, 0x0f513815 },
        { 0x1eb08717, 0x4ff5cdac, -0x6f0d1644, 0x67e8b295, 0x237afa99, 0x44093b5e, -0x78f7474e, 0x0d414bed }
    },
    {
        { 0x294ac9e8, -0x7e77956e, -0x2aaab842, 0x23162b45, 0x03715983, -0x6b3043bc, 0x134bc401, 0x50eb8fdb },
        { -0x02f18a0a, -0x30497d9b, -0x446f18f9, -0x1ba4c1d8, -0x6006d386, 0x7242a8de, -0x6ccdfd23, 0x685b3201 },
        { -0x294ccf33, -0x3f48c13a, 0x132faff1, -0x7b1bb7f9, -0x3b5a211f, 0x732b7352, -0x55832d2e, 0x5d7c7cf1 }
    },
    {
        { -0x648c5a9e, 0x33d1013e, 0x48ec26e1, -0x6da310a9, -0x22b97fa8, -0x580319ec, 0x1e9aa438, 0x78b0fad4 },
        { 0x7a4aafa2, -0x50c4b941, 0x4d40d411, -0x4878fa14, -0x3583ea1d, 0x114f0c6a, -0x56b762b3, 0x3f364faa },
        { -0x12fa4b78, -0x40a95bcf, -0x63b6a382, -0x5acc1994, -0x780c9ae6, -0x179ad451, 0x59d66c33, 0x02418000 }
    },
    {
        { -0x30c715ff, 0x28350c7d, -0x4d6e854a, 0x7c6cdbc0, -0x7a8f7d09, -0x53183042, -0x5d265e20, 0x4d2845ab },
        { -0x5c85a41c, -0x314f8802, -0x1a5a1149, -0x249bd0fe, 0x471270b8, -0x3d192f3b, 0x38e4529c, 0x4771b655 },
        { 0x447070de, -0x44ac8020, 0x6dd557df, -0x3458bbbd, 0x3600dbcb, -0x2c4a5cb9, -0x06002808, 0x4aeabbe6 }
    },
    {
        { -0x3b56370e, 0x6a2134bc, -0x7531d1c9, -0x040702e4, -0x66ee5f46, 0x000ae304, 0x6bc89b9e, 0x046e3a61 },
        { 0x40d8f78c, 0x4630119e, 0x3c710e11, -0x5fe5643b, -0x76ef2287, 0x486d2b25, -0x24fcdb1b, 0x1e6c47b3 },
        { -0x0fc6f942, 0x14e65442, -0x1c9d41d6, 0x4a019d54, -0x723dcf39, 0x68ccdfec, -0x509479e4, 0x7cfb7e3f }
    },
},
{
    {
        { 0x305b2f51, -0x69114005, -0x776a6948, -0x2c06c753, 0x46d5dd25, -0x0f0ad239, -0x44c5ff6b, 0x57968290 },
        { -0x73a75124, 0x4637974e, -0x540fbe5c, -0x4610dd05, -0x167f8e76, -0x1e7a26aa, -0x4ebc575a, 0x2f1b78fa },
        { 0x0a20e101, -0x08e547bd, 0x24f0ec47, -0x0c6c9a73, 0x6ee2eed1, -0x308af658, -0x23d55c1f, 0x7dc43e35 }
    },
    {
        { 0x273e9718, 0x5a782a5c, 0x5e4efd94, 0x3576c699, 0x1f237d3e, 0x0f2ed805, -0x7d2af567, 0x044fb81d },
        { -0x7782263d, -0x7a69999b, 0x4bb05355, -0x36f064cf, -0x10df864f, -0x391f7208, 0x758cc12f, 0x7ef72016 },
        { -0x56f81c27, -0x3e20e73b, -0x31b39ca7, 0x57b3371d, -0x4dfe44b7, -0x358fbacc, -0x63cf22d2, 0x7f79823f }
    },
    {
        { 0x68f587ba, 0x6a9c1ff0, 0x0050c8de, 0x0827894e, 0x7ded5be7, 0x3cbf9955, 0x1c06d6f0, 0x64a9b043 },
        { -0x5c4aec18, -0x7ccb2dc7, -0x46e05728, -0x3ec98f2c, -0x0a6f42cd, 0x12b54136, -0x287b264c, 0x0a4e0373 },
        { 0x5b7d2919, 0x2eb3d6a1, -0x2ac57dcb, -0x4f4b0960, -0x765ba2b9, 0x7156ce43, -0x31e7cb94, 0x071a7d0a }
    },
    {
        { 0x20e14431, -0x33f3caae, 0x09b15141, 0x0d659507, 0x209d5f36, -0x650a9de5, 0x617755d3, 0x7c69bcf7 },
        { -0x377845f5, -0x2cf8d256, -0x405a9d12, 0x01262905, -0x3f108975, -0x30abcffe, 0x46ea7e9c, 0x2c3bcc71 },
        { 0x04e8295f, 0x07f0d7eb, 0x2f50f37d, 0x10db1825, 0x171798d7, -0x16ae565d, 0x22aca51d, 0x6f5a9a73 }
    },
    {
        { -0x5c26bb42, -0x18d62b15, -0x7f875062, -0x7261f6c0, 0x47869c03, 0x4525567a, -0x1172c4dc, 0x02ab9680 },
        { 0x2f41c6c5, -0x745efff4, 0x0cfefb9b, -0x3b60863f, 0x3cc51c9f, 0x4efa4770, -0x1eb85036, 0x494e21a2 },
        { -0x221af266, -0x105b757b, 0x0fb9a249, 0x219a224e, -0x26e10927, -0x05f6e0e3, -0x15b944cc, 0x6b5d76cb }
    },
    {
        { 0x1e782522, -0x1f06bee9, 0x036936d3, -0x0e19518c, -0x2f0338ba, 0x408b3ea2, 0x03dd313e, 0x16fb869c },
        { -0x13f3266c, -0x77a8aa94, 0x5cd01dba, 0x6472dc6f, -0x70bd4b89, -0x50fe96ec, -0x7ad88cac, 0x0ae333f6 },
        { 0x33b60962, 0x288e1997, -0x27541ecd, 0x24fc72b4, 0x0991d03e, 0x4811f7ed, -0x708f2f8b, 0x3f81e38b }
    },
    {
        { 0x5f17c824, 0x0adb7f35, -0x28bd665c, 0x74b923c3, -0x34071509, -0x2a83c175, 0x4cdedc3d, 0x0ad3e2d3 },
        { 0x7ed9affe, 0x7f910fcc, 0x2465874b, 0x545cb8a1, 0x4b0c4704, -0x57c6812e, 0x04f50993, 0x50510fc1 },
        { 0x336e249d, 0x6f0c0fc5, -0x3cce3027, 0x745ede19, 0x09eefe1c, -0x0d290300, -0x0f05e142, 0x127c158b }
    },
    {
        { -0x51ae468c, -0x215d703c, 0x744dfe96, 0x1d9973d3, -0x78c7b758, 0x6240680b, -0x2e98206b, 0x4ed82479 },
        { 0x2e9879a2, -0x09e683be, 0x52ca3647, -0x5bb5222c, 0x4b4eaccb, -0x64bec03f, 0x07ef4f68, 0x354ef87d },
        { 0x60c5d975, -0x011c4ade, -0x14be4f48, 0x50352efc, -0x56099ac4, -0x77f753d0, 0x0539236d, 0x302d92d2 }
    },
},
{
    {
        { 0x0df53c30, -0x6a847475, -0x719f0f68, 0x2a1c770a, 0x345796de, -0x44385990, -0x6f366437, 0x22a48f9a },
        { -0x34c10484, 0x4c59023f, -0x39c3d56c, 0x6c2fcb99, -0x3c381f7c, -0x45be6f1e, -0x5ae78b27, 0x0e545dae },
        { -0x72c053a8, 0x6b7dc0dc, -0x191bd403, 0x5497cd6c, -0x0bff2cfb, 0x542f7d1b, 0x048d9136, 0x4159f47f }
    },
    {
        { -0x442db7c7, 0x748515a8, -0x504fd4ab, 0x77128347, 0x49a2a17f, 0x50ba2ac6, 0x3ad730f1, 0x06052551 },
        { 0x39e31e32, 0x20ad6608, -0x7bfa41b0, -0x07e1e42b, -0x0b254397, -0x07f9bfaa, -0x318e468b, 0x14d23dd4 },
        { -0x755d807e, -0x0dc671f7, -0x765e4fdc, 0x6d7982bb, 0x214dd24c, -0x0596bf7c, -0x5cdcfe3d, 0x71ab966f }
    },
    {
        { 0x02809955, -0x4ef775f9, 0x0b43c391, 0x43b273ea, -0x01f97913, -0x35649852, -0x7cca0b13, 0x605eecbf },
        { 0x4ded02fc, 0x2dcbd8e3, 0x596f22aa, 0x1151f3ec, 0x4e0328da, -0x435daabd, -0x6dbee4de, 0x35768fbe },
        { 0x6c340431, -0x7cdff59b, -0x711a63d1, -0x60328e99, 0x71300f8a, 0x75d4613f, 0x60f542f9, 0x7a912faf }
    },
    {
        { -0x05d2aa69, 0x253f4f8d, 0x5477130c, 0x25e49c40, -0x6694eefe, 0x00c052e5, 0x33bb6c4a, 0x33cb966e },
        { 0x5edc1a43, -0x4dfba7a2, 0x5897c73c, -0x60f1e912, 0x4e70483c, 0x5b82c0ae, 0x2bddf9be, 0x624a170e },
        { 0x7f116909, 0x59702804, 0x1e564467, -0x7d753be4, -0x19de8c79, 0x70417dbd, -0x0453bc7c, 0x721627ae }
    },
    {
        { 0x410b2f22, -0x02cf6844, -0x4a3057bc, -0x0e5fa259, -0x10a8358c, 0x61289a1d, -0x447de6fe, 0x245ea199 },
        { -0x78c9522b, -0x682fc43d, -0x3acd4ed0, 0x2f1422af, 0x7101bbc4, 0x3aa68a05, -0x18b06059, 0x4c946cf7 },
        { 0x78d477f8, -0x51235997, 0x29117fe1, 0x1898ba3c, 0x720cbd58, -0x308c067d, -0x474a9caf, 0x67da12e6 }
    },
    {
        { -0x7137cf74, 0x2b7ef3d3, 0x71eb94ab, -0x7d702814, -0x3af9d543, -0x7f83c4ca, 0x31a94141, 0x0cb64cb8 },
        { -0x4b4291f9, 0x7067e187, -0x382e018c, 0x6e8f0203, 0x38c85a30, -0x6c3955d1, 0x3d75a78a, 0x76297d1f },
        { 0x534c6378, 0x3030fc33, -0x1abe179f, -0x469ca3a4, -0x264d38d8, 0x15d9a9be, -0x0c88a235, 0x49233ea3 }
    },
    {
        { 0x1c9f249b, 0x7b3985fe, -0x5edccd6d, 0x4fd6b2d5, 0x1adf4d62, -0x314cba6c, 0x542de50c, 0x6987ff6f },
        { -0x724003c6, 0x629398fa, -0x2ab24bab, -0x1ed01ad3, -0x250dad6b, -0x0c41ee21, -0x31a184af, 0x628b140d },
        { -0x707c8ac4, 0x47e24142, -0x79950669, 0x6317bebc, 0x3d1a9829, -0x2544a4bd, 0x5287fb2d, 0x074d8d24 }
    },
    {
        { -0x3f1ceb78, 0x481875c6, -0x1ddfcb4c, 0x219429b2, 0x31283b65, 0x7223c98a, 0x342277f9, 0x3420d60b },
        { 0x440bfc31, -0x7cc82633, -0x50ce7029, 0x729d2ca1, 0x772c2070, -0x5fbf5b5c, 0x3a7349be, 0x46002ef0 },
        { -0x50019a09, -0x055dc522, 0x5be0764c, 0x78261ed4, 0x2f164403, 0x441c0a1e, 0x7a87d395, 0x5aea8e56 }
    },
},
{
    {
        { -0x1b1f0e89, 0x2dbc6fb6, -0x5b42956d, 0x04e1bf29, 0x787af6e8, 0x5e1966d4, -0x4bd92fa0, 0x0edc5f5e },
        { -0x435bd7c3, 0x7813c1a2, -0x5e79c227, -0x129d0f6f, -0x3d97057a, -0x51384348, 0x6f1cae4c, 0x10e5d3b7 },
        { 0x53da8e67, 0x5453bfd6, 0x24a9f641, -0x1623e114, 0x03578a23, -0x4078d9c5, 0x361cba72, 0x45b46c51 }
    },
    {
        { -0x75801c1c, -0x3162b223, 0x76620e30, -0x54ec9baa, -0x4cf166a8, 0x4b594f7b, 0x321229df, 0x5c1c0aef },
        { 0x314f7fa1, -0x56bfd541, -0x71730bb0, -0x1da80e24, 0x23a8be84, 0x1dbbd54b, 0x6dcb713b, 0x2177bfa3 },
        { -0x05862471, 0x37081bbc, -0x3da0a64d, 0x6048811e, -0x637cdb79, 0x087a7665, 0x7d8ab5bb, 0x4ae61938 }
    },
    {
        { -0x67a4047d, 0x61117e44, 0x71963136, -0x031fb9d6, -0x2bda6fb5, -0x7c53cbb8, 0x5ba43d64, 0x75685abe },
        { 0x5344a32e, -0x72240956, -0x4be4bf88, 0x7d88eab4, 0x4a130d60, 0x5eb0eb97, 0x17bf3e03, 0x1a00d91b },
        { -0x149e0d4e, 0x6e960933, -0x3600b6ae, 0x543d0fa8, 0x7af66569, -0x208d8af0, 0x23b0e6aa, 0x135529b6 }
    },
    {
        { -0x1dd17c02, -0x0a38e944, -0x17f67a3f, -0x4bd414e7, 0x14254aae, -0x136259c9, 0x1590a613, 0x5972ea05 },
        { -0x522e2ae8, 0x18f0dbd7, -0x303ee0ef, -0x68608778, 0x7114759b, -0x78cd1e10, 0x65ca3a01, 0x79b5b81a },
        { -0x237087ef, 0x0fd4ac20, -0x53b2b058, -0x65652d6c, -0x4cc9fbcc, -0x3fe4d29c, -0x6fa0c425, 0x4f7e9c95 }
    },
    {
        { 0x355299fe, 0x71c8443d, -0x24141529, -0x7432c4e4, -0x0e5b6b9a, -0x7f6db662, -0x5ebb5238, 0x1942eec4 },
        { 0x5781302e, 0x62674bbc, -0x765223f1, -0x27adf0c7, 0x53fbd9c6, -0x73d66652, 0x2e638e4c, 0x31993ad9 },
        { -0x51dcb66e, 0x7dac5319, 0x0cea3e92, 0x2c1b3d91, 0x253c1122, 0x553ce494, 0x4ef9ca75, 0x2a0a6531 }
    },
    {
        { 0x3c1c793a, -0x30c9e533, 0x5a35bc3b, 0x2f9ebcac, -0x57325955, 0x60e860e9, 0x6dea1a13, 0x055dc39b },
        { -0x0806d83e, 0x2db7937f, 0x17d0a635, -0x248be0fa, 0x1155af76, 0x5982f3a2, 0x647c2ded, 0x4cf6e218 },
        { -0x3d72a44a, -0x4ee6dd84, 0x774dffab, 0x07e24ebc, -0x1b5cd377, -0x57c38732, 0x10aa24b6, 0x121a3077 }
    },
    {
        { -0x388b7c37, -0x29a68ec2, -0x47d46951, -0x77401f89, 0x1097bcd3, 0x289e2823, 0x6ced3a9b, 0x527bb94a },
        { -0x60fcb569, -0x1b24a2a2, 0x3034bc2d, -0x1eac03f7, -0x6aae2c4f, 0x46054691, 0x7a40e52d, 0x333fc76c },
        { -0x66a4b7d2, 0x563d992a, 0x6e383801, 0x3405d07c, 0x2f64d8e5, 0x485035de, 0x20a7a9f7, 0x6b89069b }
    },
    {
        { -0x4a382489, 0x4082fa8c, -0x38cb3eab, 0x068686f8, -0x09185a82, 0x29e6c8d9, -0x589c6431, 0x0473d308 },
        { 0x6270220d, -0x7ed55fbf, -0x06dba4b2, -0x66a57606, 0x5072ef05, -0x00523b32, -0x558c148d, 0x23bc2103 },
        { 0x03589e05, -0x351186da, 0x46dcc492, 0x2b4b4212, -0x19fe56b1, 0x02a1ef74, -0x21fbcbe6, 0x102f73bf }
    },
},
{
    {
        { -0x6c5c9db9, 0x358ecba2, -0x4d97029b, -0x5070679e, 0x68a01c89, 0x412f7e99, -0x328abadc, 0x5786f312 },
        { 0x7ec20d3e, -0x4a5d2af4, -0x5f368d9d, -0x39b42292, -0x3e008cb3, 0x56e89052, 0x2b2ffaba, 0x4929c6f7 },
        { -0x35ebfcd4, 0x337788ff, 0x447f1ee3, -0x0c6defd8, 0x231bccad, -0x74ebf8e1, -0x0dcbb87d, 0x4c817b4b }
    },
    {
        { -0x5bf4bb7c, 0x413ba057, 0x4f5f6a43, -0x45b3d1e6, -0x511e29e4, 0x614ba0a5, -0x74fa23ad, 0x78a1531a },
        { 0x2871b96e, 0x0ff85385, 0x60c3f1bb, -0x1ec16055, 0x25344402, -0x1102a6ad, 0x75b7744b, 0x0a37c370 },
        { 0x3ad0562b, 0x6cbdf170, -0x36dade5d, -0x7130b7d0, -0x027bdb19, -0x25142cfd, 0x2e5ec56f, 0x72ad82a4 }
    },
    {
        { 0x67024bc3, -0x3c976c6f, 0x49502fda, -0x71962e93, -0x1ba0b4d7, -0x030d13c4, -0x5c4b343c, 0x065f669e },
        { -0x45049a0a, 0x3f9e8e35, -0x0d8d6c5f, 0x39d69ec8, -0x73095c30, 0x6cb8cd95, 0x73adae6d, 0x17347781 },
        { 0x5532db4d, -0x75ff5139, 0x43e31bb1, -0x47965b1c, -0x2c580aeb, 0x4a0f8552, 0x303d7c08, 0x19adeb7c }
    },
    {
        { 0x43c31794, -0x62fa4583, -0x6ccddada, 0x2470c8ff, 0x16197438, -0x7cdc2138, -0x7ea964ad, 0x28527098 },
        { 0x53ead9a3, -0x38df349f, 0x512b636e, 0x55b2c97f, -0x2bfd6f4f, -0x4e1ca4a1, 0x3b530ee2, 0x2fd9ccf1 },
        { 0x47f796b8, 0x07bd475b, 0x542c8f54, -0x2d384fed, 0x3b24f87e, 0x2dbd23f4, 0x7b0901d6, 0x6551afd7 }
    },
    {
        { -0x5e2a3654, 0x68a24ce3, 0x10ff6461, -0x44885cc3, 0x25d3166e, 0x0f86ce44, 0x50b9623b, 0x56507c09 },
        { 0x54aac27f, 0x4546baaf, -0x4d5ba5d8, -0x09099014, 0x562bcfe8, 0x582d1b5b, -0x6df087a1, 0x44b123f3 },
        { -0x2e8ec19d, 0x1206f0b7, 0x15bafc74, 0x353fe3d9, 0x0ad9d94d, 0x194ceb97, -0x062fc52d, 0x62fadd7c }
    },
    {
        { -0x1831ba6c, 0x3cd7bc61, -0x4822d982, -0x3294ca57, 0x4366ef27, -0x5f7f5438, 0x59c79711, 0x6ec7c46f },
        { 0x5598a074, -0x394a6985, -0x71b6c1db, 0x5efe91ce, 0x49280888, -0x2b48d3bb, -0x5d98bf3e, 0x20ef1149 },
        { 0x6f09a8a2, 0x2f07ad63, 0x24205e7d, -0x79681932, -0x11ca5ec7, -0x3f5103fb, -0x4a062769, 0x15e80958 }
    },
    {
        { 0x5bb061c4, 0x4dd1ed35, -0x6be3f900, 0x42dc0cef, -0x0279cbf2, 0x61305dc1, 0x0e55a443, 0x56b2cc93 },
        { 0x0c3e235b, 0x25a5ef7d, -0x41ecb119, 0x6c39c17f, 0x2dc5c327, -0x388b1ecc, -0x6dfde0c7, 0x021354b8 },
        { -0x59403a5e, 0x1df79da6, -0x6021bc97, 0x02f3a274, -0x325c6f59, -0x4cdc260e, -0x788b2c9d, 0x7be0847b }
    },
    {
        { 0x5307fa11, 0x1466f5af, -0x1293f50e, -0x7e803383, -0x3c5b5c05, 0x0a6de44e, -0x436d82f5, 0x74071475 },
        { -0x74c0aa3d, -0x736633a6, 0x3fded2a0, 0x0611d725, 0x36b70a36, -0x12d66a01, -0x2875d9e7, 0x1f699a54 },
        { 0x73e7ea8a, -0x188d6d0d, -0x34fba5cf, 0x296537d2, -0x2cd8b022, 0x1bd0653e, 0x76bd2966, 0x2f9a2c44 }
    },
},
{
    {
        { -0x4aaee366, -0x5d4b2520, 0x2bffff06, 0x7ac86029, -0x0aafbdcc, -0x67e0c8a3, -0x25b15ed3, 0x3f6bd725 },
        { 0x7f5745c6, -0x14e74655, 0x5787c690, 0x023a8aee, 0x2df7afa9, -0x48d8ed26, -0x15a3fec3, 0x36597d25 },
        { 0x106058ac, 0x734d8d7b, 0x6fc6905f, -0x26bfa862, -0x6dfd6cd3, 0x6466f8f9, -0x259f2930, 0x7b7ecc19 }
    },
    {
        { -0x58830565, 0x6dae4a51, -0x185c79b0, -0x7dd9c9ac, -0x70d27d25, 0x09bbffcd, 0x1bf5caba, 0x03bedc66 },
        { 0x695c690d, 0x78c2373c, 0x0642906e, -0x22dad19a, 0x4ae12bd2, -0x6ae2bbbc, 0x01743956, 0x4235ad76 },
        { 0x078975f5, 0x6258cb0d, -0x6e760d68, 0x49294254, -0x1d1c911c, -0x5f354bdd, -0x320f995f, 0x0e7ce2b0 }
    },
    {
        { -0x26b48f07, -0x01590121, -0x3e0345d3, -0x0ecf3faf, 0x7f2fab89, 0x4882d47e, -0x7513114b, 0x61525613 },
        { -0x3b737a5d, -0x3b6b9bc6, 0x3c6139ad, -0x02c9e20c, 0x3ae94d48, 0x09db17dd, -0x704b98b6, 0x666e0a5d },
        { 0x4870cb0d, 0x2abbf64e, -0x55ba7495, -0x329a4310, 0x75e8985d, -0x6541b146, -0x2aeb211c, 0x7f0bc810 }
    },
    {
        { 0x737213a0, -0x7c536253, 0x2ef72e98, -0x60090746, 0x43ec6957, 0x311e2edd, -0x213a548b, 0x1d3a907d },
        { 0x26f4136f, -0x46ff945c, 0x57e03035, -0x7298c962, 0x4f463c28, -0x34372027, -0x0711240b, 0x0d1f8dbc },
        { 0x3ed081dc, -0x45e96ccf, -0x7ae4cb80, 0x29329fad, 0x030321cb, 0x0128013c, -0x5ce4021d, 0x00011b44 }
    },
    {
        { 0x6a0aa75c, 0x16561f69, 0x5852bd6a, -0x3e408da4, -0x65869953, 0x11a8dd7f, -0x2d7aefda, 0x63d988a2 },
        { 0x3fc66c0c, 0x3fdfa06c, 0x4dd60dd2, 0x5d40e38e, 0x268e4d71, 0x7ae38b38, 0x6e8357e1, 0x3ac48d91 },
        { -0x5042dcd2, 0x00120753, -0x0227097d, -0x16d43148, -0x7b18d46f, -0x07e9964d, 0x2368a066, 0x33fad52b }
    },
    {
        { -0x3bdd3018, -0x72d33730, 0x05a13acb, 0x072b4f7b, -0x13095a91, -0x5c01491a, -0x46f58e1e, 0x3cc355cc },
        { -0x3a1be1ea, 0x540649c6, 0x333f7735, 0x0af86430, -0x0cfa18ba, -0x4d53032e, -0x5da92359, 0x16c0f429 },
        { -0x6fc16ecf, -0x16496bbd, 0x7a5637ce, -0x475b6b35, -0x45456dbc, -0x37832e5c, 0x6bae7568, 0x631eaf42 }
    },
    {
        { -0x5c8ff218, 0x47d975b9, -0x1d07faae, 0x7280c5fb, 0x32e45de1, 0x53658f27, 0x665f80b5, 0x431f2c7f },
        { -0x25990161, -0x4c16fbf0, 0x6c16e5a6, -0x7a22b4ae, 0x1ef9bf83, -0x43c2689f, 0x1ea919b5, 0x5599648b },
        { -0x7a7084e7, -0x29fd9cbc, -0x5e15aeb6, 0x14ab352f, 0x2090a9d7, -0x76ffbbe6, -0x6edac4da, 0x7b04715f }
    },
    {
        { -0x3b19453a, -0x4c893d80, 0x6d1d9b0b, -0x68f12c23, 0x450bf944, -0x4f656aa8, 0x57cde223, 0x48d0acfa },
        { -0x530951bd, -0x7c1242d8, 0x7d5c7ab4, -0x79ca8375, -0x4814d3bc, -0x3fbfb897, -0x3d09a7c1, 0x59b37bf5 },
        { 0x7dabe671, -0x49f0d91c, 0x622f3a37, -0x0e2e5e69, -0x1669fc6c, 0x4208ce7e, 0x336d3bdb, 0x16234191 }
    },
},
{
    {
        { 0x3d578bbe, -0x7ad22e03, -0x3cd79ef8, 0x2b65ce72, -0x1531dd8d, 0x658c07f4, -0x13c754c0, 0x0933f804 },
        { 0x33a63aef, -0x0e651539, 0x4442454e, 0x2c7fba5d, 0x4795e441, 0x5da87aa0, -0x5b1f4f0b, 0x413051e1 },
        { -0x72b69b8a, -0x58549687, -0x034a5438, -0x7ede5522, 0x7b539472, -0x5a23ed11, 0x5e45351a, 0x07fd4706 }
    },
    {
        { -0x6517183d, 0x30421155, -0x6bb77d5b, -0x0d7e4dd7, 0x378250e4, -0x75ec53d2, 0x54ba48f4, 0x014afa09 },
        { 0x258d2bcd, -0x37a7c3c3, -0x509f48c1, 0x17029a4d, 0x416a3781, -0x05f0362a, 0x38b3fb23, 0x1c1e5fba },
        { 0x1bb3666c, -0x34ce6900, 0x4bffecb9, 0x33006052, 0x1a88233c, 0x29371199, 0x3d4ed364, 0x29188436 }
    },
    {
        { -0x43e54915, -0x0462c83d, 0x4d57a240, 0x02be1453, -0x075a1e0a, -0x0b28cbeb, 0x0ccc8188, 0x5964f430 },
        { -0x23b45406, 0x033c6805, 0x5596ecc1, 0x2c15bf5e, -0x4a64e2c5, 0x1bc70624, -0x5e60f13b, 0x3ede9850 },
        { 0x2d096800, -0x1bb5dceb, 0x70866996, 0x5c08c559, 0x46affb6e, -0x20d249f6, -0x07a90277, 0x579155c1 }
    },
    {
        { 0x0817e7a6, -0x4a0e949d, 0x3c351026, -0x7f7396dd, 0x54cef201, 0x324a983b, 0x4a485345, 0x53c09208 },
        { 0x12e0c9ef, -0x69cdb123, -0x0dbdfd69, 0x468b878d, -0x5b0a8c42, 0x199a3776, -0x716e16d6, 0x1e7fbcf1 },
        { -0x0e345041, -0x2d2beb7f, 0x716174e5, 0x231d2db6, -0x1d5aa368, 0x0b7d7656, 0x2aa495f6, 0x3e955cd8 }
    },
    {
        { 0x61bb3a3f, -0x54c60c11, 0x2eb9193e, -0x714bff9b, 0x38c11f74, -0x4a219134, 0x26f3c49f, 0x654d7e96 },
        { 0x3ed15433, -0x1b70aca2, 0x0d7270a3, -0x2f8a96d6, -0x55219c79, 0x40fbd21d, -0x30bb6a0b, 0x14264887 },
        { 0x5c7d2ceb, -0x1a9b3023, -0x28c83347, -0x7d115022, -0x2e064f55, 0x6107db62, -0x4bca7245, 0x0b6baac3 }
    },
    {
        { 0x3700a93b, 0x204abad6, -0x25886c8d, -0x41ffdc2d, 0x633ab709, -0x27a0fcba, -0x6f7dfbee, 0x00496dc4 },
        { -0x79dd0168, 0x7ae62bcb, -0x31476e51, 0x47762256, -0x0d1bf94c, 0x1a5a92bc, -0x7b1beaff, 0x7d294017 },
        { -0x3d819ca0, 0x1c74b88d, -0x72eb7af4, 0x07485426, 0x3e0dcb30, -0x5eba0485, 0x43803b23, 0x10843f1b }
    },
    {
        { -0x1cdb9765, -0x2a9098d3, -0x4c6b567f, -0x2e257513, -0x6e973013, -0x2284a702, 0x4d56c1e8, 0x7ce246cd },
        { 0x376276dd, -0x3a06fbab, -0x289ba327, -0x31a6ea73, 0x1d366b39, -0x6d09a2af, 0x526996c4, 0x11574b6e },
        { 0x7f80be53, -0x470bcf72, 0x34a9d397, 0x5f3cb8cb, 0x33cc2b2c, 0x18a961bd, 0x3a9af671, 0x710045fb }
    },
    {
        { 0x059d699e, -0x5fc0379e, -0x659e6197, 0x2370cfa1, 0x2f823deb, -0x3b01c4ee, -0x580f7bb2, 0x1d1b056f },
        { 0x101b95eb, 0x73f93d36, 0x4f6f4486, -0x0510cc87, -0x70ea1a9e, 0x5651735f, 0x58b40da1, 0x7fa3f190 },
        { -0x1a9409e1, 0x1bc64631, 0x6e5382a3, -0x2c8654f0, 0x0540168d, 0x4d58c57e, -0x7bbd271c, 0x56625662 }
    },
},
{
    {
        { 0x1ff38640, -0x22b6632a, 0x063625a0, 0x29cd9bc3, 0x3dd73dc3, 0x51e2d802, 0x203b9231, 0x4a25707a },
        { -0x09d9800a, -0x461b6622, 0x742c0843, 0x7772ca7b, -0x165b0d4f, 0x23a0153f, -0x2a2faffa, 0x2cdfdfec },
        { 0x53f6ed6a, 0x2ab7668a, 0x1dd170a1, 0x30424258, 0x3ae20161, 0x4000144c, 0x248e49fc, 0x5721896d }
    },
    {
        { -0x5e2f25b2, 0x285d5091, -0x4a01c1f8, 0x4baa6fa7, -0x1e6c6c4d, 0x63e5177c, -0x3b4fcf03, 0x03c935af },
        { -0x02e7e452, 0x0b6e5517, 0x2bb963b4, -0x6fdd9d61, 0x32064625, 0x5509bce9, -0x09c3ec26, 0x578edd74 },
        { 0x492b0c3d, -0x668d893a, -0x201dfa04, 0x47ccc2c4, -0x229dc5c4, -0x232d647c, 0x0288c7a2, 0x3ec2ab59 }
    },
    {
        { -0x51cd2e35, -0x58dec5f7, 0x40f5c2d5, 0x0f2b87df, -0x17e154d7, 0x0baea4c6, 0x6adbac5e, 0x0e1bf66c },
        { -0x1b278447, -0x5e5f2d85, 0x61391aed, -0x5674b215, 0x73cb9b83, -0x665f2230, 0x200fcace, 0x2dd5c25a },
        { 0x792c887e, -0x1d542a17, -0x346d92a3, 0x1a020018, -0x4551a0e2, -0x40459633, 0x5ae88f5f, 0x730548b3 }
    },
    {
        { -0x5e291ccc, -0x7fa4f6b5, 0x09353f19, -0x40c10e89, 0x0622702b, 0x423f06cb, -0x2787ba23, 0x585a2277 },
        { -0x34574712, -0x3bcaae5d, -0x4deea0ea, 0x65a26f1d, -0x5473c7b0, 0x760f4f52, 0x411db8ca, 0x3043443b },
        { 0x33d48962, -0x5e75a07e, -0x1387da81, 0x6698c4b5, 0x373e41ff, -0x5871905b, 0x50ef981f, 0x76562789 }
    },
    {
        { -0x15793063, -0x1e8f8c5d, 0x07155fdc, 0x3a8cfbb7, 0x31838a8e, 0x4853e7fc, -0x49ec09ea, 0x28bbf484 },
        { -0x2ae03740, 0x38c3cf59, 0x0506b6f2, -0x64122d03, -0x54a8f171, 0x26bf109f, -0x3e47b95a, 0x3f4160a8 },
        { 0x6f136c7c, -0x0d9ed0a4, -0x0922ee42, -0x50152ef9, 0x13de6f33, 0x527e9ad2, -0x7e7708a3, 0x1e79cb35 }
    },
    {
        { -0x0a1f7e7f, 0x77e953d8, 0x299dded9, -0x7b5af3bc, -0x79bada1b, -0x2393d2f4, 0x39d1f2f4, 0x478ab52d },
        { -0x11081c0f, 0x013436c3, -0x0161ef08, -0x7d749581, -0x43062104, 0x7ff908e5, 0x3a3b3831, 0x65d7951b },
        { -0x6dad2ea7, 0x66a6a4d3, -0x78e537f9, -0x1a221e44, -0x593e3691, -0x47d394c0, 0x1a212214, 0x16d87a41 }
    },
    {
        { -0x2ab1fa7d, -0x045b2a1e, 0x2ebd99fa, -0x1de05029, 0x6ee9778f, 0x497ac273, 0x7a5a6dde, 0x1f990b57 },
        { 0x42066215, -0x4c4281a6, 0x0c5a24c1, -0x78641c33, -0x29066b49, 0x57c05db1, 0x65f38ca6, 0x28f87c81 },
        { 0x1be8f7d6, -0x5ccbb153, -0x53158671, 0x7d1e50eb, 0x520de052, 0x77c6569e, 0x534d6d3e, 0x45882fe1 }
    },
    {
        { -0x6bc3901c, -0x275366d7, -0x5c7c6d5e, -0x4a060e9f, -0x4137650d, 0x2699db13, -0x1bfa0f8c, 0x7dcf843c },
        { 0x757983d6, 0x6669345d, 0x17aa11a6, 0x62b6ed11, -0x67a1ed71, 0x7ddd1857, -0x09d90923, 0x688fe5b8 },
        { 0x4a4732c0, 0x6c90d648, -0x35a9cd67, -0x2adebc03, -0x6ea2391f, -0x4c41d73d, 0x7327191b, 0x6739687e }
    },
},
{
    {
        { -0x363468e1, -0x731a5530, -0x602ab5d7, 0x1156aaa9, 0x15af9b78, 0x41f72470, 0x420f49aa, 0x1fe8cca8 },
        { 0x200814cf, -0x609a3a16, 0x69a31740, -0x7bfac91f, 0x25c8b4ad, -0x74f12ec7, -0x16c9c9e3, 0x0080dbaf },
        { 0x3c0cc82a, 0x72a1848f, -0x788361ac, 0x38c560c2, -0x31aabec0, 0x5004e228, 0x03429d71, 0x042418a1 }
    },
    {
        { 0x20816247, 0x58e84c6f, -0x1c90286d, -0x724d4d4a, 0x1d484d85, -0x688e7daa, -0x79cd5429, 0x0822024f },
        { -0x540c00a1, -0x766215af, 0x2fc2d8ba, -0x646c5799, -0x419142a4, 0x2c38cb97, -0x68d9c4a3, 0x114d5784 },
        { 0x6b1beca3, -0x4cfe4484, -0x3914ec8b, 0x55393f6d, -0x68491b15, -0x6ef2d7f0, -0x62b8615d, 0x1ad4548d }
    },
    {
        { 0x0fe9fed3, -0x5f901993, 0x1c587909, -0x578cc5c0, 0x0df98953, 0x30d14d80, -0x384cfda8, 0x41ce5876 },
        { 0x389a48fd, -0x32a58260, -0x6587c8e2, -0x4c705b56, 0x2cdb8e6c, -0x392689e5, -0x3681ebbd, 0x35cf51db },
        { -0x298f3fde, 0x59ac3bc5, -0x64ee6bfa, -0x151983f0, -0x4c87d026, -0x68674210, -0x02f8bf6e, 0x651e3201 }
    },
    {
        { 0x1efcae9e, -0x5a845b60, -0x23cf756c, 0x769f4bee, 0x3603cb2e, -0x2e0ef115, 0x7e441278, 0x4099ce5e },
        { -0x10cf3a31, -0x29c27b7d, 0x2361cc0c, 0x4cd4b496, -0x5b7bd954, -0x116f1b00, 0x18c14eeb, 0x0af51d7d },
        { -0x75aede17, 0x1ac98e4f, -0x2405d020, 0x7dae9544, -0x29bcf207, -0x7cdf55f3, 0x2c4a2fb5, 0x66728265 }
    },
    {
        { 0x2946db23, -0x52574920, 0x7b253ab7, 0x1c0ce51a, 0x66dd485b, -0x7bb737a6, -0x2f98a521, 0x7f1fc025 },
        { -0x27943655, -0x78b9de0c, 0x56fe6fea, -0x4ab38442, 0x7fadc22c, 0x077a2425, 0x19b90d39, 0x1ab53be4 },
        { 0x319ea6aa, -0x2711e4e8, 0x3a21f0da, 0x004d8808, -0x77c5b0b5, 0x3bd6aa1d, -0x202602ec, 0x4db9a3a6 }
    },
    {
        { -0x34488398, -0x26a4ff45, -0x6e0e87b7, -0x22437b96, -0x41d7264d, 0x7cf700ae, -0x7a2ce0c2, 0x5ce1285c },
        { -0x4663f8ab, -0x73184dc5, -0x3b0af086, 0x35c5d6ed, -0x1264af3d, 0x7e1e2ed2, -0x176cb25f, 0x36305f16 },
        { -0x674f4218, 0x31b6972d, -0x535921a5, 0x7d920706, -0x6f759a61, -0x198cef08, -0x1020fdcb, 0x50fac2a6 }
    },
    {
        { -0x090bb644, 0x295b1c86, 0x1f0ab4dd, 0x51b2e84a, -0x5571aae3, -0x3ffe34d0, 0x44f43662, 0x6a28d359 },
        { 0x5b880f5a, -0x0c2c560d, -0x24fc183e, -0x1213faf4, -0x060f4e5e, -0x576967e1, -0x53a1cb5c, 0x49a4ae2b },
        { 0x04a740e0, 0x28bb12ee, -0x64317e8c, 0x14313bbd, -0x173ef3c0, 0x72f5b5e4, 0x36adcd5b, 0x7cbfb199 }
    },
    {
        { -0x33c91920, -0x7186c586, 0x7d586eed, -0x0605485d, -0x451e0b1c, 0x3a4f9692, -0x00a0bb82, 0x1c14b03e },
        { 0x6b89792d, -0x5cee223e, -0x25aed99c, 0x1b30b4c6, -0x30eaf7a7, 0x0ca77b4c, 0x1b009408, 0x1de443df },
        { 0x14a85291, 0x19647bd1, 0x1034d3af, 0x57b76cb2, 0x0f9d6dfa, 0x6329db44, 0x6a571493, 0x5ef43e58 }
    },
},
{
    {
        { -0x37f3e540, -0x59923363, 0x1b38a436, -0x685fa30c, -0x6a24283a, -0x58140c42, -0x72818255, 0x7da0b8f6 },
        { 0x385675a6, -0x1087dfec, -0x55025618, -0x5d9b60d0, 0x5cdfa8cb, 0x4cd1eb50, 0x1d4dc0b3, 0x46115aba },
        { -0x3c4a258a, -0x2bf0e6ad, 0x21119e9b, 0x1dac6f73, -0x014da6a0, 0x03cc6021, -0x7c98b4b5, 0x5a5f887e }
    },
    {
        { -0x5f59bc47, -0x6169d72d, -0x193cdf9c, -0x4a3c3500, 0x7c2dec32, -0x64acfd77, -0x2a2e38f4, 0x43e37ae2 },
        { 0x70a13d11, -0x709cfe31, 0x350dd0c4, -0x303147eb, -0x5b435b82, -0x08fd682c, -0x1bb2ebcc, 0x3669b656 },
        { -0x12591ecd, 0x387e3f06, -0x665ec540, 0x67301d51, 0x36263811, -0x42a52708, 0x4fd5e9be, 0x6a21e6cd }
    },
    {
        { 0x6699b2e3, -0x10bed6ee, 0x708d1301, 0x71d30847, 0x1182b0bd, 0x325432d0, 0x001e8b36, 0x45371b07 },
        { 0x3046e65f, -0x0e39e8f6, 0x00d23524, 0x58712a2a, -0x737d48ab, 0x69dbbd3c, -0x5e6a00a9, 0x586bf9f1 },
        { 0x5ef8790b, -0x5924f773, 0x610937e5, 0x5278f0dc, 0x61a16eb8, -0x53fcb62e, -0x6f1ade87, 0x0eafb037 }
    },
    {
        { 0x0f75ae1d, 0x5140805e, 0x2662cc30, -0x13fd041d, -0x156dc693, 0x2cebdf1e, -0x3abca44d, 0x44ae3344 },
        { 0x3748042f, -0x69faaa3f, -0x7df455ef, 0x219a41e6, 0x73486d0c, 0x1c81f738, 0x5a02c661, 0x309acc67 },
        { -0x445abc12, -0x630d7647, 0x5ac97142, -0x0c89f163, 0x4f9360aa, 0x1d82e5c6, 0x7f94678f, 0x62d5221b }
    },
    {
        { 0x3af77a3c, 0x7585d426, -0x0116ebb3, -0x205184ef, 0x59f7193d, -0x5af98f80, -0x7c6ddfc9, 0x14f29a53 },
        { 0x18d0936d, 0x524c299c, -0x75f3e5f4, -0x37944a94, -0x24b579cf, -0x5c8afad2, -0x438aba9e, 0x5c0efde4 },
        { 0x25b2d7f5, -0x208e8124, -0x664acfc0, 0x21f970db, -0x3c12b39e, -0x256dcb49, 0x7bee093e, 0x5e72365c }
    },
    {
        { 0x2f08b33e, 0x7d933906, -0x2060cd42, 0x5b9659e5, 0x1f9ebdfd, -0x5300c253, -0x348cb649, 0x70b20555 },
        { 0x4571217f, 0x575bfc07, 0x0694d95b, 0x3779675d, -0x0be6e1cd, -0x65f5c845, 0x47b4eabc, 0x77f1104c },
        { 0x55112c4c, -0x41aeec3b, -0x6577e033, 0x6688423a, 0x5e503b47, 0x44667785, 0x4a06404a, 0x0e34398f }
    },
    {
        { 0x3e4b1928, 0x18930b09, 0x73f3f640, 0x7de3e10e, 0x73395d6f, -0x0bcde826, -0x35c863c2, 0x6f8aded6 },
        { 0x3ecebde8, -0x4982dd27, 0x27822f07, 0x09b3e841, -0x4fa49273, 0x743fa61f, -0x75c9dc8e, 0x5e540536 },
        { -0x02484d66, -0x1cbfedc3, -0x5de54d6f, 0x487b97e1, -0x02196b62, -0x066982fe, -0x372c2169, 0x780de72e }
    },
    {
        { 0x00f42772, 0x671feaf3, 0x2a8c41aa, -0x708d14d6, -0x68c8cd6e, 0x29a17fd7, 0x32b587a6, 0x1defc6ad },
        { 0x089ae7bc, 0x0ae28545, 0x1c7f4d06, 0x388ddecf, 0x0a4811b8, 0x38ac1551, 0x71928ce4, 0x0eb28bf6 },
        { -0x10ae6a59, -0x50a441e6, -0x6e84ea13, 0x148c1277, 0x7ae5da2e, 0x2991f7fb, -0x0722d799, 0x467d201b }
    },
},
{
    {
        { 0x296bc318, 0x745f9d56, -0x27ead19b, -0x66ca7f2c, 0x5839e9ce, -0x4f1a4ec1, -0x2bc6de40, 0x51fc2b28 },
        { -0x0842d195, 0x7906ee72, 0x109abf4e, 0x05d270d6, -0x46be575c, -0x72a301bb, 0x1c974287, 0x44c21867 },
        { -0x6a1d5674, 0x1b8fd117, 0x2b6b6291, 0x1c4e5ee1, 0x7424b572, 0x5b30e710, 0x4c4f4ac6, 0x6e6b9de8 }
    },
    {
        { -0x07f34f78, 0x6b7c5f10, 0x56e42151, 0x736b54dc, -0x3910663c, -0x3d49df5b, -0x3c5f90be, 0x5f4c802c },
        { 0x4b1de151, -0x200da032, -0x1ee3bfdb, -0x27be3f39, 0x54749c87, 0x2554b3c8, -0x6f71f207, 0x2d292459 },
        { 0x7d0752da, -0x649a370f, -0x38811800, -0x77e31cc8, 0x5b62f9e3, -0x3c4aeb10, -0x413ef2b8, 0x66ed5dd5 }
    },
    {
        { -0x3435fb83, -0x0f520c37, -0x0baad095, -0x7e3c4d35, 0x44735f93, -0x3025eed3, 0x7e20048c, 0x1f23a0c7 },
        { 0x0bb2089d, 0x7d38a1c2, -0x69332bee, -0x7f7ccb1f, 0x6c97d313, -0x3b58f474, 0x03007f20, 0x2eacf8bc },
        { -0x1a43ea90, -0x0dcab985, 0x0dbab38c, 0x03d2d902, -0x03061f62, 0x27529aa2, -0x62cb43b0, 0x0840bef2 }
    },
    {
        { 0x7f37e4eb, -0x32ab1f95, -0x0a169336, -0x733ea079, -0x2ca68232, -0x47db7450, 0x6074400c, 0x246affa0 },
        { -0x23ef4d79, 0x796dfb35, 0x5c7ff29d, 0x27176bcd, -0x384db6fb, 0x7f3d43e8, -0x6e3abd8a, 0x0304f5a1 },
        { -0x041bacdf, 0x37d88e68, -0x3f28afce, -0x79f68ab8, -0x76b5f2cb, 0x4e9b13ef, 0x5753d325, 0x25a83cac }
    },
    {
        { 0x3952b6e2, -0x60f099d7, 0x0934267b, 0x33db5e0e, -0x29f60124, -0x00badad5, -0x3af91f37, 0x06be10f5 },
        { -0x1127e9a2, 0x10222f48, 0x4b8bcf3a, 0x623fc123, -0x3dde1710, 0x1e145c09, -0x3587d9d0, 0x7ccfa59f },
        { -0x49d5cba1, 0x1a9615a9, 0x4a52fecc, 0x22050c56, 0x28bc0dfe, -0x585d877b, 0x1a1ee71d, 0x5e82770a }
    },
    {
        { 0x42339c74, -0x17fd17f6, -0x5800051b, 0x34175166, 0x1c408cae, 0x34865d1f, 0x605bc5ee, 0x2cca982c },
        { -0x527695a4, 0x35425183, -0x1872ad0a, -0x1798c505, -0x6d5ca09c, 0x2c66f25f, 0x3b86b102, 0x09d04f3b },
        { 0x197dbe6e, -0x02d2a2cb, -0x741b005d, 0x207c2eea, 0x325ae918, 0x2613d8db, 0x27741d3e, 0x7a325d17 }
    },
    {
        { 0x7e2a076a, -0x132d82ff, 0x1636495e, -0x28779761, -0x6e6dcc1b, 0x52a61af0, 0x7bb1ae64, 0x2a479df1 },
        { -0x2e92021e, -0x2fc94645, -0x3b6857d7, -0x5dfaa8a9, -0x580ed999, -0x7193369a, 0x1239c180, 0x4d3b1a79 },
        { 0x33db2710, -0x61a11172, -0x293bc35b, 0x189854de, -0x6d8e7ec8, -0x5be3dd3b, -0x5bc5a165, 0x27ad5538 }
    },
    {
        { -0x71b8f884, -0x34a5829d, 0x20a1c059, -0x7248ac9f, -0x74120234, 0x549e1e4d, 0x503b179d, 0x080153b7 },
        { 0x15350d61, 0x2746dd4b, -0x116ade49, -0x2fc03438, 0x138672ca, -0x1791c9a6, 0x7e7d89e2, 0x510e987f },
        { 0x0a3ed3e3, -0x2259626d, -0x329f58de, 0x3d386ef1, -0x4255b11a, -0x37e852a8, 0x4fe7372a, 0x23be8d55 }
    },
},
{
    {
        { 0x567ae7a9, -0x43e10b43, -0x29bb6743, 0x3f624cb2, 0x2c1f4ec8, -0x1bef9b2e, -0x45c7bfff, 0x2ef9c5a5 },
        { 0x74ef4fad, -0x6a016e66, -0x095cf75e, 0x3a827bec, 0x09a47b01, -0x69b1fe2d, 0x5ba3c797, 0x71c43c4f },
        { -0x05618b33, -0x4902920a, -0x1b50d986, -0x0e7d8744, -0x0e1066f2, -0x7daa4c30, -0x6f3a0d6d, 0x5a758ca3 }
    },
    {
        { 0x1d61dc94, -0x731f6e75, -0x657ecf9a, -0x7212c9ba, -0x5017552d, -0x2b1957d7, -0x09c62bc1, 0x0a738027 },
        { -0x26b9db6b, -0x5d48d8f0, -0x2a82affd, 0x3aa8c6d2, -0x5f4b7836, -0x1c2bff41, -0x4c148d14, 0x2dbae244 },
        { 0x57ffe1cc, -0x67f0b5d1, -0x1e7c67bd, 0x00670d0d, 0x49fb15fd, 0x105c3f4a, 0x5126a69c, 0x2698ca63 }
    },
    {
        { 0x5e3dd90e, 0x2e3d702f, -0x1b2dac7a, -0x61c0f6e8, 0x024da96a, 0x5e773ef6, 0x4afa3332, 0x3c004b0c },
        { 0x32b0ba78, -0x189ace78, -0x6da30075, 0x381831f7, -0x5fd6e034, 0x08a81b91, 0x49caeb07, 0x1fb43dcc },
        { 0x06f4b82b, -0x6556b954, -0x57f93b0d, 0x1ca284a5, -0x3932b879, 0x3ed3265f, -0x32e02de9, 0x6b43fd01 }
    },
    {
        { 0x3e760ef3, -0x4a38bda8, -0x11f54670, 0x75dc52b9, 0x072b923f, -0x40ebd83e, 0x6ff0d9f0, 0x73420b2d },
        { 0x4697c544, -0x3858a2b5, -0x20f00041, 0x15fdf848, -0x55b987a6, 0x2868b9eb, 0x5b52f714, 0x5a68d710 },
        { -0x617ae1fa, -0x50d30935, -0x39ddc73c, -0x70a6c6ed, -0x66040c8d, -0x2575476a, -0x15cb4362, 0x3db5632f }
    },
    {
        { -0x7d67da2b, 0x2e4990b1, 0x3e9a8991, -0x12151479, 0x4c704af8, -0x110fc2c7, -0x6a20d4f2, 0x59197ea4 },
        { -0x08a22628, -0x0b9111d5, 0x396759a5, 0x0d17b1f6, 0x499e7273, 0x1bf2d131, 0x49d75f13, 0x04321adf },
        { -0x1b1aa552, 0x04e16019, 0x7e2f92e9, -0x1884bc86, 0x6f159aa4, -0x3831d23f, -0x0b28f340, 0x45eafdc1 }
    },
    {
        { -0x30334e13, -0x49f1b9dc, -0x42a3fc6b, 0x59dbc292, -0x23fb7e37, 0x31a09d1d, 0x5d56d940, 0x3f73ceea },
        { -0x7fba28d5, 0x69840185, -0x30d0f9af, 0x4c22faa2, 0x6b222dc6, -0x6be5c99b, 0x0362dade, 0x5a5eebc8 },
        { 0x0a4e8dc6, -0x4858402f, 0x44c9b339, -0x41a8ff82, 0x1557aefa, 0x60c1207f, 0x266218db, 0x26058891 }
    },
    {
        { -0x39891abe, 0x4c818e3c, 0x03ceccad, 0x5e422c93, -0x4bed60f8, -0x13f83336, -0x4dbbbc48, 0x0dedfa10 },
        { -0x7c9f00fc, 0x59f704a6, 0x7661e6f4, -0x3c26c022, 0x12873551, -0x7ce4d58d, 0x4e615d57, 0x54ad0c2e },
        { -0x47d4add6, -0x11c4982b, -0x605a3e15, 0x36f16346, 0x6ec19fd3, -0x5a4b2d0e, -0x58856bf8, 0x62ecb2ba }
    },
    {
        { -0x5049d78c, -0x6df8d7ca, 0x79e104a5, 0x5fcd5e85, -0x39cf5eb6, 0x5aad01ad, 0x75663f98, 0x61913d50 },
        { 0x61152b3d, -0x1a1286ae, 0x0eddd7d1, 0x4962357d, -0x4694b38f, 0x7482c8d0, -0x56992742, 0x2e59f919 },
        { 0x1a3231da, 0x0dc62d36, -0x6bdffd90, -0x05b8a7ce, 0x3f9594ce, 0x02d80151, 0x31c05d5c, 0x3ddbc2a1 }
    },
},
{
    {
        { 0x004a35d1, -0x048ca53e, 0x3a6607c3, 0x31de0f43, -0x3ad72a67, 0x7b8591bf, -0x0a44faf4, 0x55be9a25 },
        { 0x4ffb81ef, 0x3f50a50a, 0x3bf420bf, -0x4e1fcaf7, -0x3955d330, -0x645571e4, -0x05dc85c0, 0x32239861 },
        { 0x33db3dbf, 0x0d005acd, -0x7f53ca1e, 0x0111b37c, 0x6f88ebeb, 0x4892d66c, 0x6508fbcd, 0x770eadb1 }
    },
    {
        { -0x5faf8e47, -0x0e2c497f, 0x3592ff3a, 0x2207659a, 0x7881e40e, 0x5f016929, -0x7945c8b2, 0x16bedd0e },
        { 0x5e4e89dd, -0x7bae0620, -0x4386c6c9, -0x3f9cfd01, 0x56a6495c, 0x5d227495, -0x5fa9fc05, 0x09a6755c },
        { 0x2c2737b5, 0x5ecccc4f, 0x2dccb703, 0x43b79e0c, 0x4ec43df3, 0x33e008bc, -0x0f8a9940, 0x06c1b840 }
    },
    {
        { -0x64fd7fa4, 0x69ee9e7f, 0x547d1640, -0x34007d76, -0x4dbcf698, 0x3d93a869, 0x3fe26972, 0x46b7b8cd },
        { -0x5c770789, 0x7688a5c6, -0x214d4954, 0x02a96c14, 0x1b8c2af8, 0x64c9f343, 0x54a1eed6, 0x36284355 },
        { -0x01811420, -0x167edf7a, 0x2f515437, 0x4cba6be7, 0x516efae9, 0x1d04168b, 0x43982cb9, 0x5ea13910 }
    },
    {
        { -0x2a2c4ffe, 0x6f2b3be4, 0x6a09c880, -0x5013cc27, -0x57433b34, 0x035f73a4, 0x4662198b, 0x22c5b928 },
        { -0x0b8fd11f, 0x49125c9c, -0x74da4cd3, 0x4520b71f, 0x501fef7e, 0x33193026, -0x372d14d5, 0x656d8997 },
        { 0x433d8939, -0x34a73702, 0x6a8d7e50, -0x765f34d2, 0x09fbbe5a, 0x79ca9553, -0x32803efa, 0x0c626616 }
    },
    {
        { -0x040bab4f, -0x70203c87, -0x0e5b488f, 0x45a5a970, -0x452ca6eb, -0x536de109, -0x57e3de6e, 0x42d088dc },
        { 0x4879b61f, 0x1ffeb80a, 0x4ada21ed, 0x6396726e, 0x368025ba, 0x33c7b093, -0x0c3ce878, 0x471aa0c6 },
        { -0x5fe9ae67, -0x7025f0c9, -0x375f1cbd, 0x0adadb77, -0x378a17e0, 0x20fbfdfc, 0x0c2206e7, 0x1cf2bea8 }
    },
    {
        { 0x02c0412f, -0x67d291e6, -0x24a71702, -0x6f05b37d, -0x234e7440, 0x01c2f5bc, 0x216abc66, 0x686e0c90 },
        { -0x4c9dfd54, -0x3d220e22, -0x2d1d855b, -0x6d5a01f7, -0x03f60e2d, 0x7d1648f6, 0x13bc4959, 0x74c2cc05 },
        { -0x5abc6a59, 0x1fadbadb, -0x51f25996, -0x4be5fd60, -0x445c83f9, -0x40e60a68, -0x21b7bcf3, 0x6a12b8ac }
    },
    {
        { 0x1aaeeb5f, 0x793bdd80, -0x3eae778f, 0x00a2a0aa, 0x1f2136b4, -0x175c8c5d, -0x036e10e7, 0x48aab888 },
        { 0x39d495d9, -0x072515e1, 0x525f1dfc, 0x592c190e, -0x3666e2e5, -0x247342fc, -0x2770f349, 0x11f7fda3 },
        { 0x5830f40e, 0x041f7e92, 0x79661c06, 0x002d6ca9, 0x2b046a2e, -0x79236007, -0x74fb6c2f, 0x76036092 }
    },
    {
        { 0x695a0b05, -0x4bcef71b, -0x52c85c75, 0x6cb00ee8, -0x5cac8c7f, 0x5edad6ee, -0x4923cddc, 0x3f2602d4 },
        { 0x120cf9c6, 0x21bb41c6, -0x21325a65, -0x154d55ee, 0x0aa48b34, -0x3e58d2fe, -0x1782c498, 0x215d4d27 },
        { 0x5bcaf19c, -0x374db84a, -0x4e4d39ae, 0x49779dc3, -0x2a131d1e, -0x765e7f45, -0x31371fc7, 0x13f098a3 }
    },
},
{
    {
        { 0x2796bb14, -0x0c55a85e, -0x64f825df, -0x77c54549, 0x31a0391c, -0x1ab41de8, -0x27cdfa07, 0x5ee7fb38 },
        { -0x31a13ab5, -0x6523f007, -0x73d0ecf3, 0x039c2a6b, -0x0f076aeb, 0x028007c7, -0x53fb4c95, 0x78968314 },
        { 0x41446a8e, 0x538dfdcb, 0x434937f9, -0x5a530257, 0x263c8c78, 0x46af908d, -0x6435f2f7, 0x61d0633c }
    },
    {
        { -0x07038c21, -0x525cd744, -0x590fc804, -0x117b96a3, 0x38c2a909, 0x637fb4db, -0x07f98424, 0x5b23ac2d },
        { -0x0024da9a, 0x63744935, 0x780b68bb, -0x3a429477, 0x553eec03, 0x6f1b3280, 0x47aed7f5, 0x6e965fd8 },
        { -0x117fad85, -0x652d46ad, -0x05219273, -0x1770e656, 0x150e82cf, 0x0e711704, -0x226a2124, 0x79b9bbb9 }
    },
    {
        { -0x71608c8c, -0x2e668252, -0x3044f7ea, -0x5fcd5d08, 0x6d445f0a, -0x329345ee, 0x0accb834, 0x1ba81146 },
        { 0x6a3126c2, -0x144caac0, 0x68c8c393, -0x2d9c7c58, -0x1a46857e, 0x6c0c6429, -0x3602deb9, 0x5065f158 },
        { 0x0c429954, 0x708169fb, -0x28913099, -0x1eb9ff54, 0x70e645ba, 0x2eaab98a, 0x58a4faf2, 0x3981f39e }
    },
    {
        { 0x6de66fde, -0x37ba205b, 0x2c40483a, -0x1ead5b00, -0x384b09ce, -0x162d1e9d, -0x2343e49b, 0x30f4452e },
        { 0x59230a93, 0x18fb8a75, 0x60e6f45d, 0x1d168f69, 0x14a93cb5, 0x3a85a945, 0x05acd0fd, 0x38dc0837 },
        { -0x3a8a68c0, -0x7a92d87e, -0x06634134, -0x05ecba97, -0x3f15b18f, -0x77bb038d, 0x593f2469, 0x632d9a1a }
    },
    {
        { -0x12f37b59, -0x40f602ef, 0x0d9f693a, 0x63f07181, 0x57cf8779, 0x21908c2d, -0x7509b45e, 0x3a5a7df2 },
        { -0x47f8345a, -0x094494eb, -0x43ab0f29, 0x1823c7df, 0x6e29670b, -0x44e268fd, 0x47ed4a57, 0x0b24f488 },
        { 0x511beac7, -0x23252b42, -0x12d9330e, -0x5bac7f8b, 0x005f9a65, -0x1e630061, 0x75481f63, 0x34fcf744 }
    },
    {
        { 0x78cfaa98, -0x5a44e255, 0x190b72f2, 0x5ceda267, 0x0a92608e, -0x6cf636ef, 0x2fb374b0, 0x0119a304 },
        { 0x789767ca, -0x3e681fb4, 0x38d9467d, -0x478eb235, -0x7c06a058, 0x55de8882, 0x4dfa63f7, 0x3d3bdc16 },
        { -0x173de883, 0x67a2d89c, 0x6895d0c1, 0x669da5f6, -0x4d7d5d50, -0x0a9a671b, -0x121df58d, 0x56c088f1 }
    },
    {
        { 0x24f38f02, 0x581b5fac, -0x451cf343, -0x56f41602, -0x75306d10, -0x65de96fe, -0x7ca6fc71, 0x038b7ea4 },
        { 0x10a86e17, 0x336d3d11, 0x0b75b2fa, -0x280c77ce, 0x25072988, -0x06eacc8a, -0x66ef7479, 0x09674c6b },
        { -0x66ce9008, -0x60b107df, -0x155872b1, 0x2f49d282, 0x5aef3174, 0x0971a5ab, 0x5969eb65, 0x6e5e3102 }
    },
    {
        { 0x63066222, 0x3304fb0e, -0x785345c1, -0x04caf977, -0x73ef9e5d, -0x42e6db89, -0x2e7c79e0, 0x3058ad43 },
        { -0x781a6c05, -0x4e939d0b, -0x35a2c18f, 0x4999edde, 0x14cc3e6d, -0x4b6e3e20, -0x76572458, 0x08f51147 },
        { -0x1a899c30, 0x323c0ffd, -0x5dd159f0, 0x05c3df38, -0x5366b066, -0x42387543, -0x101c2367, 0x26549fa4 }
    },
},
{
    {
        { -0x08ac6947, 0x04dbbc17, -0x2d0798ba, 0x69e6a2d7, -0x0ac1543a, -0x39bf6267, 0x332e25d2, 0x606175f6 },
        { -0x78317077, 0x738b38d7, 0x4179a88d, -0x49d9a71e, -0x0eaece93, 0x30738c9c, 0x727275c9, 0x49128c7f },
        { -0x0abf1823, 0x4021370e, -0x5e0e2f5b, 0x0910d6f5, 0x5b06b807, 0x4634aacd, 0x6944f235, 0x6a39e635 }
    },
    {
        { 0x74049e9d, 0x1da19657, -0x6701cad5, -0x0432915f, -0x33adc95a, -0x4e3432b0, 0x3f9846e2, 0x1f5ec83d },
        { -0x206f0c19, -0x6932a9c0, -0x2405da16, 0x6c3a760e, 0x59e33cc4, 0x24f3ef09, 0x530d2e58, 0x42889e7e },
        { 0x328ccb75, -0x7104dc3d, -0x22789117, -0x50bd5df9, 0x5dfae796, 0x20fbdadc, 0x06bf9f51, 0x241e246b }
    },
    {
        { 0x6280bbb8, 0x7eaafc9a, -0x0bfc27f7, 0x22a70f12, 0x1bfc8d20, 0x31ce40bb, -0x1742ac12, 0x2bc65635 },
        { -0x5291670a, 0x29e68e57, 0x0b462065, 0x4c9260c8, -0x5ae144b5, 0x3f00862e, -0x4c726f69, 0x5bc2c77f },
        { -0x5694526d, -0x172a2361, -0x21e6b824, -0x1a704e83, 0x65185fa3, 0x681532ea, 0x034a7830, 0x1fdd6c3b }
    },
    {
        { 0x2dd8f7a9, -0x63ec595b, 0x3efdcabf, 0x2dbb1f8c, 0x5e08f7b5, -0x69e1cdc0, -0x4419361b, 0x48c8a121 },
        { 0x55dc18fe, 0x0a64e28c, 0x3399ebdd, -0x1c206167, 0x70e2e652, 0x79ac4323, 0x3ae4cc0e, 0x35ff7fc3 },
        { 0x59646445, -0x03bea584, -0x3ed749eb, -0x2ddb4d29, 0x05fbb912, 0x6035c9c9, 0x74429fab, 0x42d7a912 }
    },
    {
        { -0x6cc25a44, -0x565b76b9, -0x3d168614, 0x4a58920e, 0x13e5ac4c, -0x69278000, 0x4b48b147, 0x453692d7 },
        { -0x1508d12d, 0x4e6213e3, 0x43acd4e7, 0x6794981a, 0x6eb508cb, -0x00ab8322, 0x10fcb532, 0x6fed19dd },
        { -0x57aa6391, -0x2288a267, -0x20ffc1dc, -0x0bd5dec0, -0x256d759a, 0x5223e229, 0x6d38f22c, 0x063f46ba }
    },
    {
        { 0x37346921, 0x39843cb7, 0x38c89447, -0x58b804f9, -0x5dbacf82, -0x34727fcf, 0x6d82f068, 0x67810f8e },
        { 0x5f536694, -0x2d2dbd77, 0x42939b2c, -0x35cc5d3b, -0x382246a4, -0x6790525a, 0x2f712d5d, 0x5a152c04 },
        { -0x2dd7824c, 0x3eeb8fbc, 0x01a03e93, 0x72c7d3a3, -0x4267d9a6, 0x5473e88c, 0x5921b403, 0x7324aa51 }
    },
    {
        { -0x17dcab35, -0x52dc0926, -0x49a8e593, 0x6962502a, -0x1c71c82f, -0x649ae9ca, -0x2e5cced1, 0x5cac5005 },
        { 0x6c3cbe8e, -0x7a86bd0c, 0x4730c046, -0x5e2c9b4f, -0x2dc3be41, 0x1c8ed914, -0x11092a2e, 0x0838e161 },
        { -0x161c66fc, -0x733eab34, -0x7b2197ba, 0x5b3a040b, -0x4e41a292, -0x3b2759e4, -0x2779e0fe, 0x40fb897b }
    },
    {
        { 0x5ab10761, -0x1a8127b9, 0x6fd13746, 0x71435e20, -0x32fda9ce, 0x342f824e, -0x5786e185, 0x4b16281e },
        { 0x62de37a1, -0x7b3a5570, 0x0d1d96e1, 0x421da500, 0x6a9242d9, 0x78828630, 0x690d10da, 0x3c5e464a },
        { 0x0b813381, -0x2e3efe2b, 0x76ee6828, -0x2119f0ef, 0x383f6409, 0x0cb68893, -0x0900b7b6, 0x6183c565 }
    },
},
{
    {
        { -0x50c09992, -0x24b97ab7, -0x0eb5f15b, -0x288030fc, -0x5b45f3b9, 0x3df23ff7, 0x32ce3c85, 0x3a10dfe1 },
        { 0x1e6bf9d6, 0x741d5a46, 0x7777a581, 0x2305b3fc, 0x6474d3d9, -0x2baa8b5e, 0x6401e0ff, 0x1926e1dc },
        { -0x15e83160, -0x1f80b176, 0x3a1fc1fd, 0x2fd51546, 0x31f2c0f1, 0x175322fd, -0x79e1a2eb, 0x1fa1d01d }
    },
    {
        { -0x2e206b55, 0x38dcac00, -0x2ef7f217, 0x2e712bdd, -0x022a1d9e, 0x7f13e93e, -0x1165fe1b, 0x73fced18 },
        { 0x7d599832, -0x337faa6c, 0x37f15520, 0x1e4656da, 0x4e059320, -0x6609088c, 0x6a75cf33, 0x773563bc },
        { 0x63139cb3, 0x06b1e908, -0x3a5fc133, -0x5b6c2599, -0x529c76ce, -0x72883138, 0x1b864f44, 0x1f426b70 }
    },
    {
        { -0x6e5edaae, -0x0e81ca38, 0x575e9c76, -0x48947ead, 0x0d9b723e, -0x057cbf91, 0x3fa7e438, 0x0b76bb1b },
        { 0x41911c01, -0x1036d9b4, 0x17a22c25, -0x0e5c4848, -0x0cf0ebb9, 0x5875da6b, 0x1d31b090, 0x4e1af527 },
        { 0x7f92939b, 0x08b8c1f9, -0x2bbb5492, -0x41988e35, -0x66447fe9, 0x22e56463, -0x488d56ab, 0x7b6dd61e }
    },
    {
        { -0x54fe2d39, 0x5730abf9, 0x40143b18, 0x16fb76dc, -0x5f344d7f, -0x7993419b, -0x64009502, 0x53fa9b65 },
        { 0x50f33d92, -0x48523e18, 0x608cd5cf, 0x7998fa4f, -0x7203a425, -0x5269d243, -0x50e2d0b1, 0x703e9bce },
        { -0x6b77abab, 0x6c14c8e9, 0x65aed4e5, -0x7bc5a29a, -0x4329a50f, 0x181bb73e, -0x3b39e0b0, 0x398d93e5 }
    },
    {
        { -0x2d181c0e, -0x3c7883a0, 0x30828bb1, 0x3b34aaa0, 0x739ef138, 0x283e26e7, 0x02c30577, 0x699c9c90 },
        { 0x33e248f3, 0x1c4bd167, 0x15bf0a5f, -0x4261ed79, -0x5ef4fc8a, -0x2bc07310, -0x20e6e4ed, 0x53b09b5d },
        { 0x5946f1cc, -0x0cf958dd, -0x331a2683, -0x6de8e74b, -0x7e4b168b, 0x28cdd247, 0x6fcdd907, 0x51caf30c }
    },
    {
        { 0x18ac54c7, 0x737af99a, -0x3ae34cf1, -0x6fcc8724, 0x4ce10cc7, 0x2b89bc33, -0x76071666, 0x12ae29c1 },
        { 0x7674e00a, -0x59f458be, -0x5e85840d, 0x630e8570, -0x30ccdb34, 0x3758563d, 0x2383fdaa, 0x5504aa29 },
        { 0x1f0d01cf, -0x56613f35, 0x3a34f7ae, 0x0dd1efcc, -0x2f63b1de, 0x55ca7521, 0x58eba5ea, 0x5fd14fe9 }
    },
    {
        { -0x406c3472, 0x3c42fe5e, 0x36d4565f, -0x412057af, -0x77bddf18, -0x1f0f7a62, 0x0725d128, 0x7dd73f96 },
        { 0x2845ab2c, -0x4a23d221, 0x0a7fe993, 0x069491b1, 0x4002e346, 0x4daaf3d6, 0x586474d1, 0x093ff26e },
        { 0x68059829, -0x4ef2db02, -0x2450dc1b, 0x75730672, -0x4ba853d7, 0x1367253a, -0x794b8f5c, 0x2f59bcbc }
    },
    {
        { -0x496e3cff, 0x7041d560, -0x522818e2, -0x7adfe4c1, 0x11335585, 0x16c2e163, 0x010828b1, 0x2aa55e3d },
        { -0x66e8eca1, -0x7c7b82be, 0x567d03d7, -0x52e46ee1, -0x4188552f, 0x7e7748d9, 0x2e51af4a, 0x5458b42e },
        { 0x0c07444f, -0x12ae6d1a, 0x74421d10, 0x42c54e2d, -0x024a379c, 0x352b4c82, -0x7589799c, 0x13e9004a }
    },
},
{
    {
        { -0x7f94b984, 0x1e6284c5, -0x18a29f85, -0x3a096685, -0x4c872d9e, -0x749826a8, -0x7e327490, 0x3d88d66a },
        { 0x6c032bff, -0x344a4aab, 0x29297a3a, -0x208e6e49, -0x52127e45, -0x3e008cda, 0x68be03f5, 0x71ade8bb },
        { 0x204ed789, -0x7489856d, -0x605f51d6, 0x762fcacb, 0x6dce4887, 0x771febcc, -0x700fa04d, 0x34306215 }
    },
    {
        { 0x2a7b31b4, -0x031de6f9, -0x55a87fea, 0x4d7adc75, -0x78b86cdc, 0x0ec276a6, 0x1fda4beb, 0x6d6d9d5d },
        { -0x1e0a40b7, -0x1fa25e59, -0x2b8c9f6e, 0x26457d6d, 0x73cc32f6, 0x77dcb077, -0x6322a033, 0x0a5d9496 },
        { -0x164f7e7d, 0x22b1a58a, -0x3ea3c775, -0x026a2f8f, -0x7af5fae9, -0x567edc8a, -0x4480cca2, 0x33384cba }
    },
    {
        { 0x26218b8d, 0x33bc627a, -0x3857f39f, -0x157f4de1, 0x173e9ee6, -0x6ba74ed5, 0x0e2f3059, 0x076247be },
        { 0x0ca2c7b5, 0x3c6fa268, 0x6fb64fda, 0x1b508204, 0x5431d6de, -0x14accb64, 0x6b879c89, 0x5278b38f },
        { 0x1416375a, 0x52e105f6, -0x7a54145c, -0x136850ca, 0x23a67c36, 0x26e6b506, -0x0c2b04ff, 0x5cf0e856 }
    },
    {
        { 0x3db342a8, -0x415131cf, -0x7bd24812, -0x345c9ca5, -0x7e80ec11, -0x177399e0, 0x4e76d5c6, 0x1b9438aa },
        { 0x1ae8cab4, -0x0936978d, -0x34b06d3b, 0x5e20741e, -0x733243c2, 0x2da53be5, 0x69970df7, 0x2dddfea2 },
        { 0x166f031a, -0x75af8882, 0x0fb7a328, 0x067b39f1, 0x010fbd76, 0x1925c9a6, -0x338bf6fb, 0x6df9b575 }
    },
    {
        { 0x48cade41, -0x13203ca5, -0x4dcd7d90, 0x6a88471f, 0x40a01b6a, 0x740a4a24, 0x003b5f29, 0x471e5796 },
        { 0x27f6bdcf, 0x42c11929, 0x403d61ca, -0x706e6e86, -0x7461e09f, -0x23e3a59a, 0x04ec0f8d, 0x15960478 },
        { -0x5312c854, -0x2569444d, -0x16df7316, 0x7a2423b5, 0x38aebae2, 0x24cc5c30, -0x23a251d1, 0x50c356af }
    },
    {
        { 0x1b31b964, -0x30126321, -0x735ae50d, -0x0b79567b, -0x1573e07c, 0x14897265, -0x6cd53400, 0x784a53dd },
        { 0x41c30318, 0x09dcbf43, -0x7ce7e232, -0x1145f9ef, -0x23e1d65f, -0x3e863f32, 0x073f35b0, 0x1dbf7b89 },
        { 0x14fc4920, 0x2d99f9df, -0x3bb6601b, 0x76ccb60c, -0x1a30fffd, -0x5becd345, 0x54f000ea, 0x3f93d823 }
    },
    {
        { 0x79e14978, -0x1553ed2f, -0x441400a2, -0x006dc00d, 0x0663ce27, 0x4af663e4, 0x11a5f5ff, 0x0fd381a8 },
        { -0x61fb317b, -0x7e7c1898, 0x04465341, 0x678fb71e, 0x6688edac, -0x526dfa71, 0x532b099a, 0x5da350d3 },
        { -0x5bc920ac, -0x0da95314, -0x51962918, 0x108b6168, 0x6b5d036c, 0x20d986cb, -0x011d50b0, 0x655957b9 }
    },
    {
        { -0x2ffd2f54, -0x423ebf65, -0x4a33265a, 0x66660245, -0x05217a14, -0x7dce823c, 0x6ad7df0d, 0x02fe934b },
        { -0x56fdfcf1, -0x51574f81, -0x0b9c2ebd, -0x07738996, 0x3c787a60, 0x15b08366, -0x7d985b58, 0x08eab114 },
        { -0x3048158c, -0x10a30f00, -0x5e34bd54, 0x22897633, -0x310d7a1e, -0x2b31f3ac, -0x75eb95ab, 0x30408c04 }
    },
},
{
    {
        { 0x193b877f, -0x44d1ff37, -0x1f23af95, -0x131c5770, 0x36de649f, -0x130c4840, -0x672161e6, 0x5f460408 },
        { -0x7cd03125, 0x739d8845, -0x5194079d, -0x05c72937, -0x48b00109, 0x32bc0dca, 0x14bce45e, 0x73937e88 },
        { 0x297bf48d, -0x46fc8eea, -0x2b0f97cc, -0x562ec4de, 0x4696bdc6, -0x1e68eaa9, -0x6e2a17cb, 0x2cf8a4e8 }
    },
    {
        { 0x17d06ba2, 0x2cb5487e, 0x3950196b, 0x24d2381c, -0x7a6875d0, -0x289a637f, -0x6e295b0a, 0x7a6f7f28 },
        { 0x07110f67, 0x6d93fd87, 0x7c38b549, -0x22b3f62d, -0x3d8c957a, 0x7cb16a4c, 0x58252a09, 0x2049bd6e },
        { 0x6a9aef49, 0x7d09fd8d, 0x5b3db90b, -0x0f119f42, 0x519ebfd4, 0x4c21b52c, -0x3aba6be3, 0x6011aadf }
    },
    {
        { 0x02cbf890, 0x63ded0c8, 0x0dff6aaa, -0x042f6736, -0x46491267, 0x624d0afd, 0x79340b1e, 0x69ce18b7 },
        { -0x306a07c4, 0x5f67926d, 0x71289071, 0x7c7e8561, -0x667085a5, -0x295e180d, 0x0b62f9e0, 0x6fc5cc1b },
        { -0x4d678635, -0x2e10aad8, -0x2b816f6e, -0x22e551c4, 0x189f2352, 0x127e0442, -0x1a8efe0f, 0x15596b3a }
    },
    {
        { 0x7e5124ca, 0x09ff3116, -0x2638ba21, 0x0be4158b, 0x7ef556e5, 0x292b7d22, -0x50492ec8, 0x3aa4e241 },
        { 0x3f9179a2, 0x462739d2, -0x68292231, -0x007cedcf, 0x53f2148a, 0x1307deb5, 0x7b5f4dda, 0x0d223768 },
        { 0x2a3305f5, 0x2cc138bf, -0x5d16d93d, 0x48583f8f, 0x5549d2eb, 0x083ab1a2, 0x4687a36c, 0x32fcaa6e }
    },
    {
        { 0x2787ccdf, 0x3207a473, -0x0dec1c08, 0x17e31908, -0x09f269b2, -0x2a4d1329, -0x3d9ff417, 0x746f6336 },
        { -0x3a82650b, 0x7bc56e8d, -0x620f420e, 0x3e0bd2ed, 0x22efe4a3, -0x553feb22, -0x014295a4, 0x4627e9ce },
        { -0x549368e4, 0x3f4af345, -0x66bc8ce1, -0x1d77148e, 0x0344186d, 0x33596a8a, 0x7ed66293, 0x7b491700 }
    },
    {
        { -0x22ac5d23, 0x54341b28, -0x20bd03c1, -0x55e86fa5, 0x4dd2f8f4, 0x0ff592d9, -0x1f732c83, 0x1d03620f },
        { -0x547b4f9c, 0x2d85fb5c, -0x760c43ec, 0x497810d2, 0x7b15ce0c, 0x476adc44, -0x07bb0285, 0x122ba376 },
        { -0x5d4b1aac, -0x3dfdcd33, 0x115d187f, -0x612f02be, 0x7dd479d9, 0x2eabb4be, 0x2b68ec4c, 0x02c70bf5 }
    },
    {
        { 0x458d72e1, -0x531acd41, 0x7cb73cb5, 0x5be768e0, -0x11744219, 0x56cf7d94, -0x014bc5fd, 0x6b0697e3 },
        { 0x5d0b2fbb, -0x5d7813b5, 0x074882ca, 0x415c5790, -0x3e2f7ea4, -0x1fbb59e2, 0x409ef5e0, 0x26334f0a },
        { -0x209d5c40, -0x49370fb6, 0x076da45d, 0x3ef000ef, 0x49f0d2a9, -0x636346a8, 0x441b2fae, 0x1cc37f43 }
    },
    {
        { -0x36315147, -0x2899a90f, 0x18e5656a, 0x1c5b15f8, -0x7bb3dccc, 0x26e72832, 0x2f196838, 0x3a346f77 },
        { 0x5cc7324f, 0x508f565a, -0x1af956de, -0x2f9e3b40, 0x5c45ac19, -0x04e75425, 0x0380314a, 0x6c6809c1 },
        { -0x1d259538, -0x2d2aaeee, -0x4e17ae13, -0x1642fccf, -0x71398d9e, -0x69f8b923, 0x6ef7c5d0, 0x05911b9f }
    },
},
{
    {
        { -0x3a01606c, 0x01c18980, 0x716fd5c8, -0x329a9897, -0x2e6a5f7a, -0x7e9fba3d, 0x66cc7982, 0x6e2b7f32 },
        { -0x49c800d3, -0x162328aa, -0x36780f3c, -0x13b3cb71, -0x0c043849, -0x312a6d7b, -0x6c1e1579, 0x33053547 },
        { -0x083ca971, -0x337fdb98, 0x19974cb3, -0x6216457e, -0x4a47eca0, -0x5448dd64, 0x6fbeba62, 0x44e2017a }
    },
    {
        { -0x49359133, -0x7807d30d, 0x18f4a0c2, 0x580f893e, 0x2604e557, 0x05893007, 0x56d19c1d, 0x6cab6ac2 },
        { 0x54dab774, -0x3b3d58bd, 0x4eaf031a, -0x71a2b3c4, 0x42838f17, -0x4893dc2e, 0x68dce4ea, 0x749a098f },
        { 0x2cc1de60, -0x23201f60, 0x51c5575b, 0x032665ff, 0x073abeeb, 0x2c0c32f1, -0x328479fa, 0x6a882014 }
    },
    {
        { -0x50b01492, -0x2eee2e84, -0x4cc55b5d, 0x050bba42, -0x114b93d0, 0x17514c3c, 0x1bc27d75, 0x54bedb8b },
        { -0x5b8b804b, -0x5ad56d02, 0x1fa5ab89, -0x23ed5bb7, -0x47b85b32, -0x27d256b5, -0x6aed33b2, 0x4d77edce },
        { 0x77e2189c, 0x77c8e145, -0x00663bbb, -0x5c1b9096, 0x6d335343, 0x3144dfc8, 0x7c4216a9, 0x3a96559e }
    },
    {
        { -0x7f4555ae, 0x44938968, -0x0d7a6bf2, 0x4c98afc4, -0x5babb74a, -0x10b55865, -0x5a855181, 0x5278c510 },
        { -0x0bd52d12, 0x12550d37, -0x675e040b, -0x74871ffc, 0x33894cb2, 0x5d530782, 0x3e498d0c, 0x02c84e4e },
        { 0x294c0b94, -0x5ab22f8c, -0x20e7004a, -0x0aa2b948, -0x72517c9a, -0x0f90133b, -0x7e6f2e9b, 0x58865766 }
    },
    {
        { 0x3de25cc3, -0x40a7cb10, -0x297eab6a, -0x47783752, -0x6b7e176e, 0x5105221a, -0x088dc06d, 0x6760ed19 },
        { 0x1aef7117, -0x2b88edcf, 0x229e92c7, 0x50343101, -0x62ea6469, 0x7a95e184, -0x74a2d637, 0x2449959b },
        { -0x53ca1ea0, 0x669ba3b7, -0x457bdfaa, 0x2eccf73f, -0x3f7fb0f9, 0x1aec1f17, 0x1856f4e7, 0x0d96bc03 }
    },
    {
        { -0x338afa1f, -0x4e2acb50, 0x16c35288, 0x32cd0034, 0x0762c29d, -0x34c95a80, 0x237a0bf8, 0x5bfe69b9 },
        { 0x75c52d82, 0x3318be77, 0x54d0aab9, 0x4cb764b5, -0x3388c26f, -0x5430c2d9, -0x7edcd776, 0x3bf4d184 },
        { 0x78a151ab, 0x183eab7e, -0x66f6c89d, -0x44166f37, 0x4ac7e335, -0x008e8292, 0x25f39f88, 0x4c5cddb3 }
    },
    {
        { -0x185606fe, 0x57750967, 0x4f5b467e, 0x2c37fdfc, 0x3177ba46, -0x4d9e99c6, -0x23d2acd5, 0x3a375e78 },
        { 0x6190a6eb, -0x3f0948b3, 0x2db8f4e4, 0x20ea81a4, -0x68cea8a0, -0x57429083, 0x62ac7c21, 0x33b1d602 },
        { 0x2d4dddea, -0x7ebe18d1, 0x62c607c8, -0x19150168, 0x573cafd0, 0x23c28458, 0x4ff97346, 0x46b9476f }
    },
    {
        { 0x0d58359f, 0x1215505c, -0x03d73b95, 0x2a2013c7, -0x761599b2, 0x24a0a1af, -0x5eecf1e1, 0x4400b638 },
        { 0x4f901e5c, 0x0c1ffea4, 0x2184b782, 0x2b0b6fb7, 0x0114db88, -0x1a78006f, 0x4785a142, 0x37130f36 },
        { -0x6912e63d, 0x3a01b764, -0x12cd8dd0, 0x31e00ab0, -0x7c35ea4f, 0x520a8857, 0x5accbec7, 0x06aab987 }
    },
},
{
    {
        { 0x512eeaef, 0x5349acf3, 0x1cc1cb49, 0x20c141d3, -0x56659773, 0x24180c07, -0x39b4d2e9, 0x555ef9d1 },
        { -0x0a20f145, -0x3ecc667d, 0x512c4cac, -0x3f0c8a71, 0x0bb398e1, 0x2cf1130a, -0x55d8f39e, 0x6b3cecf9 },
        { 0x3b73bd08, 0x36a770ba, -0x5c5040f4, 0x624aef08, -0x4bf6b90e, 0x5737ff98, 0x3381749d, 0x675f4de1 }
    },
    {
        { 0x3bdab31d, -0x5ed00927, -0x629ad202, 0x0725d80f, -0x65416b79, 0x019c4ff3, -0x7d32c3bd, 0x60f450b8 },
        { 0x6b1782fc, 0x0e2c5203, 0x6cad83b4, 0x64816c81, 0x6964073e, -0x2f234227, 0x0164c520, 0x13d99df7 },
        { 0x21e5c0ca, 0x014b5ec3, -0x28e6405e, 0x4fcb69c9, 0x750023a0, 0x4e5f1c18, 0x55edac80, 0x1c06de9e }
    },
    {
        { -0x00929656, -0x002ad4c0, -0x23bfb645, 0x34530b18, -0x5cb26769, 0x5e4a5c2f, 0x7d32ba2d, 0x78096f8e },
        { -0x5cc13b1e, -0x66f0852a, -0x41d11f72, 0x6608f938, 0x63284515, -0x635ebc3b, -0x13d249f3, 0x4cf38a1f },
        { 0x0dfa5ce7, -0x5f55559b, 0x48b5478c, -0x063b61d6, 0x7003725b, 0x4f09cc7d, 0x26091abe, 0x373cad3a }
    },
    {
        { -0x76224453, -0x0e415705, 0x61aeaecb, 0x3bcb2cbc, 0x1f9b8d9d, -0x70a75845, 0x5112a686, 0x21547eda },
        { -0x7d360a84, -0x4d6b9cb3, 0x24934536, 0x1fcbfde1, 0x418cdb5a, -0x6163b24d, 0x454419fc, 0x0040f3d9 },
        { -0x02a6792d, -0x210216c7, 0x510a380c, -0x0bd8d377, -0x44cee647, -0x48d45bf9, 0x4a254df4, 0x63550a33 }
    },
    {
        { 0x72547b49, -0x6445a7bb, -0x1d3bf720, -0x0cfa3906, -0x38cb0e73, 0x60e8fa69, -0x55828986, 0x39a92baf },
        { -0x4a9630c9, 0x6507d6ed, 0x0ca52ee1, 0x178429b0, -0x149429a3, -0x1583ff70, -0x250870af, 0x3eea62c7 },
        { -0x196cd8b2, -0x62db38ed, 0x68dbd375, 0x5f638577, -0x14754c66, 0x70525560, 0x65c9c4cd, 0x68436a06 }
    },
    {
        { -0x17dfef84, 0x1e56d317, -0x7bf5169b, -0x3ad997bc, 0x320ffc7a, -0x3e1f5e3a, -0x6e9eeb8e, 0x5373669c },
        { 0x202f3f27, -0x43fdca18, 0x64f975b0, -0x38a3ff1e, -0x5c73dbea, -0x6e5b162b, -0x75487607, 0x17b6e7f6 },
        { -0x65f1ada9, 0x5d2814ab, -0x36354c04, -0x6f70df7c, 0x5b2d1eca, -0x50350a78, 0x78f87d11, 0x1cb4b5a6 }
    },
    {
        { -0x5d5ff819, 0x6b74aa62, -0x0f8e384f, -0x0cee1f50, 0x000be223, 0x5707e438, -0x7d109154, 0x2dc0fd2d },
        { 0x394afc6c, -0x499b3f95, -0x6725a04f, 0x0c88de24, 0x4bcad834, 0x4f8d0316, -0x218bcb5e, 0x330bca78 },
        { 0x1119744e, -0x67d1007c, 0x2b074724, -0x0696a16a, -0x4036ac05, -0x3a753eb1, 0x369f1cf5, 0x3c31be1b }
    },
    {
        { -0x0634bd8e, -0x3e97436d, -0x38312468, -0x51478ee1, 0x34ac8d7a, 0x7f0e52aa, 0x7e7d55bb, 0x41cec109 },
        { 0x08948aee, -0x4f0b79b3, -0x6e45e391, 0x07dc19ee, -0x59535ea8, 0x7975cdae, 0x4262d4bb, 0x330b6113 },
        { -0x5d927f76, -0x0869e629, 0x1d9e156d, -0x44e02b62, -0x245e20d9, 0x73d7c36c, 0x1f28777d, 0x26b44cd9 }
    },
},
{
    {
        { -0x4fd7a0c9, -0x50bb7bd3, 0x47efc8df, -0x78ace770, -0x07df6866, -0x6a8b1f6f, 0x69615579, 0x0e378d60 },
        { 0x393aa6d8, 0x300a9035, -0x5ed44e33, 0x2b501131, -0x0f6c3dde, 0x7b1ff677, -0x3547d453, 0x4309c1f8 },
        { -0x7cf8a5ab, -0x26056e8f, 0x6b009fdc, 0x4bdb5ad2, -0x29c210f2, 0x7829ad2c, 0x75fd3877, 0x078fc549 }
    },
    {
        { -0x47cc5676, -0x1dffb4a5, 0x2d4c3330, 0x44775dec, 0x7eace913, 0x3aa24406, -0x2a71ff57, 0x272630e3 },
        { 0x28878f2d, -0x782042ec, 0x1e9421a1, 0x134636dd, 0x257341a3, 0x4f17c951, -0x52d69348, 0x5df98d4b },
        { -0x1336f4ac, -0x0c987030, 0x12043599, -0x0ffeba65, 0x3758b89b, 0x26725fbc, 0x73a719ae, 0x4325e4aa }
    },
    {
        { -0x30960a63, -0x12db9d66, -0x22a5440c, 0x2a4a1cce, 0x56b2d67b, 0x3535ca1f, 0x43b1b42d, 0x5d8c68d0 },
        { 0x433c3493, 0x657dc6ef, -0x7f24073d, 0x65375e9f, 0x5b372dae, 0x47fd2d46, 0x796e7947, 0x4966ab79 },
        { -0x1c4bd4f6, -0x11ccd2b3, 0x16a4601c, -0x27b1a5d5, 0x078ba3e4, 0x78243877, 0x184ee437, 0x77ed1eb4 }
    },
    {
        { -0x616d12e6, 0x185d43f8, -0x01b8e63a, -0x4fb5e116, -0x590fc0b1, 0x499fbe88, 0x3c859bdd, 0x5d8b0d2f },
        { 0x201839a0, -0x402b1ec1, 0x3e3df161, -0x5110001e, 0x6b5d1fe3, -0x49a4fb10, 0x2b62fbc0, 0x52e085fb },
        { -0x5ab30d46, 0x124079ea, 0x001b26e7, -0x28db9a15, -0x36850803, 0x6843bcfd, 0x55eacd02, 0x0524b42b }
    },
    {
        { -0x647d6154, -0x43e72353, -0x4a0a8630, 0x23ae7d28, 0x69384233, -0x3cb9edd6, -0x182b5377, 0x1a6110b2 },
        { -0x1babb850, -0x02f2a242, 0x092005ee, 0x6cec351a, 0x567579cb, -0x665b87bc, 0x16e7fa45, 0x59d242a2 },
        { -0x19966854, 0x4f833f6a, 0x361839a4, 0x6849762a, -0x68f54adb, 0x6985dec1, -0x234e0aba, 0x53045e89 }
    },
    {
        { -0x72ba01ee, -0x7b25c322, -0x1bbb1d2e, -0x42bd3de8, 0x1f7e3598, -0x57ae6988, 0x5616e2b2, 0x7642c93f },
        { -0x28acac25, -0x34744cba, -0x51aee1de, -0x03034db5, -0x2af51911, -0x345b72c0, -0x0b0834a3, 0x26e3bae5 },
        { 0x4595f8e4, 0x2323daa7, -0x7a85414c, -0x21977375, 0x1c59326e, 0x3fc48e96, 0x15c9b8ba, 0x0b2e73ca }
    },
    {
        { 0x79c03a55, 0x0e3fbfaf, 0x4cbb5acf, 0x3077af05, -0x24c21c61, -0x2a3aadbb, 0x476a4af7, 0x015e68c1 },
        { -0x3e80afda, -0x2944bbd8, -0x04a56359, -0x614d8ddd, 0x1919c644, -0x1c845afd, -0x4a6599fe, 0x21ce380d },
        { 0x20066a38, -0x3e2ad7ae, 0x3570aef3, -0x6a9fc1ae, 0x226b8a4d, -0x7cd9a659, 0x1f8eedc9, 0x5dd68909 }
    },
    {
        { -0x5acecf7c, 0x1d022591, -0x29d8f78e, -0x35d2b552, 0x2f0bfd20, -0x795ed47b, -0x528258b8, 0x56e6c439 },
        { -0x402c37aa, -0x34537b22, -0x4ca00dbc, 0x1624c348, 0x5d9cad07, -0x48077236, -0x5d3d1418, 0x3b0e574d },
        { 0x42bdbae6, -0x38fb00b7, -0x4d21e087, 0x5e21ade2, 0x5652fad8, -0x16a24c0d, -0x70f7143f, 0x0822b537 }
    },
},
{
    {
        { 0x62730383, -0x1e480d6d, -0x143575d4, 0x4b5279ff, -0x402becec, -0x25038876, -0x638d9ef1, 0x7deb1014 },
        { -0x70c78b8b, 0x51f04847, -0x634134c4, -0x4da2430c, -0x2660dfab, -0x6554edbc, 0x1c10a5d6, 0x2c709e6c },
        { -0x78991186, -0x349d5096, 0x5553cd0e, 0x66cbec04, 0x0f0be4b5, 0x58800138, -0x09d31d16, 0x08e68e9f }
    },
    {
        { 0x0ab8f2f9, 0x2f2d09d5, -0x3aa6dc21, -0x5346de73, 0x73766cb9, 0x4a8f3426, 0x38f719f5, 0x4cb13bd7 },
        { 0x4bc130ad, 0x34ad500a, 0x3d0bd49c, -0x72c724b7, 0x500a89be, -0x5da3c268, -0x1145c4f7, 0x2f1f3f87 },
        { -0x1aea49b6, -0x087b738b, -0x24b56fc8, -0x5a6afe46, 0x3f751b50, -0x3df2cec1, -0x3f51d118, 0x19a1e353 }
    },
    {
        { -0x2a694243, -0x4bde8d33, -0x671103c0, -0x6c1fbabd, -0x4bbef64b, -0x604eacb9, 0x0266ae34, 0x736bd399 },
        { -0x4505fa3d, 0x7d1c7560, -0x391aa19f, -0x4c1e5f60, -0x3f299b8d, -0x1cad68e8, -0x3df3cb7a, 0x41546b11 },
        { -0x6ccb4c4c, -0x7aacd2b0, 0x60816573, 0x46fd114b, 0x425c8375, -0x33a0a0d0, -0x478054a4, 0x412295a2 }
    },
    {
        { -0x1d6c153a, 0x2e655261, 0x2133acdb, -0x7ba56dfd, 0x7900996b, 0x460975cb, 0x195add80, 0x0760bb8d },
        { -0x0a812917, 0x19c99b88, 0x6df8c825, 0x5393cb26, -0x4cf52d8d, 0x5cee3213, -0x4ad2d1cc, 0x14e153eb },
        { -0x32197e76, 0x413e1a17, -0x12965f7c, 0x57156da9, 0x46caccb1, 0x2cbf268f, -0x3cc53a0e, 0x6b34be9b }
    },
    {
        { 0x6571f2d3, 0x11fc6965, 0x530e737a, -0x393617bb, -0x2b01afcb, -0x1cc5185e, 0x2e6dd30b, 0x01b9c7b6 },
        { 0x3a78c0b2, -0x0c20d09c, -0x0dd1fd84, 0x4c3e971e, 0x49c1b5a3, -0x1382e3a2, 0x0922dd2d, 0x2012c18f },
        { 0x5ac89d29, -0x77f4aa1b, 0x45a0a763, 0x1483241f, -0x3d1893e1, 0x3d36efdf, 0x4e4bade8, 0x08af5b78 }
    },
    {
        { -0x7633d3b5, -0x1d8ceb2e, -0x5d78e873, 0x4be4bd11, -0x05cc9b32, 0x18d528d6, -0x50267d92, 0x6423c1d5 },
        { -0x77e0dacd, 0x283499dc, 0x779323b6, -0x62fada26, 0x673441f4, -0x76852205, 0x163a168d, 0x32b79d71 },
        { -0x12034c96, -0x337a0727, 0x3746e5f9, 0x22bcc28f, -0x061a2c33, -0x1b621cc8, -0x3ec1d234, 0x480a5efb }
    },
    {
        { 0x42ce221f, -0x499eb31c, 0x4c053928, 0x6e199dcc, -0x23e341fd, 0x663fb4a4, 0x691c8e06, 0x24b31d47 },
        { 0x01622071, 0x0b51e70b, -0x74e2503b, 0x06b505cf, -0x10a55433, 0x2c6bb061, 0x0cb7bf31, 0x47aa2760 },
        { -0x3fea073d, 0x2a541eed, 0x7c693f7c, 0x11a4fe7e, 0x4ea278d6, -0x0f5099ed, 0x14dda094, 0x545b585d }
    },
    {
        { -0x1c4cde1f, 0x6204e4d0, 0x28ff1e95, 0x3baa637a, 0x5b99bd9e, 0x0b0ccffd, 0x64c8d071, 0x4d22dc3e },
        { -0x5f2bc5f1, 0x67bf275e, 0x089beebe, -0x521971cc, -0x2b8618d2, 0x4289134c, 0x32ba5454, 0x0f62f9c3 },
        { -0x29c4a0c7, -0x034b9a77, 0x57cbcf61, 0x5cae6a3f, -0x6ac505fb, -0x01453d2e, 0x36371436, 0x1c0fa01a }
    },
},
{
    {
        { 0x54c53fae, -0x3ee11a18, 0x2b4f3ff4, 0x6a0b06c1, -0x1f49858e, 0x33540f80, -0x32f81c11, 0x15f18fc3 },
        { -0x4383296e, -0x18ab8bb7, -0x1908c221, 0x0f9abeaa, 0x00837e29, 0x4af01ca7, 0x3f1bc183, 0x63ab1b5d },
        { -0x4fd70b74, 0x32750763, 0x556a065f, 0x06020740, -0x3cb6a4a8, -0x2ac427ee, -0x79a0af73, 0x08706c9b }
    },
    {
        { 0x38b41246, -0x3366e4bf, 0x6f9ac26b, 0x243b9c52, -0x48345443, -0x4610b6b3, -0x2f7d1300, 0x5fba433d },
        { 0x3d343dff, -0x0c835d55, -0x7f5439e9, 0x1a8c6a2d, -0x2b330036, -0x71b61fcb, -0x455e2e47, 0x48b46bee },
        { -0x366be530, -0x63b61cab, 0x74498f84, -0x468cb522, 0x66663e5c, 0x41c3fed0, -0x1718ef4d, 0x0ecfedf8 }
    },
    {
        { -0x16bfc89e, 0x744f7463, -0x72033637, -0x08657212, 0x55e4cde3, 0x163a6496, -0x4d7b0bcb, 0x3b61788d },
        { -0x632b8f27, 0x76430f9f, -0x5bd09ff8, -0x49d53365, 0x59adad5e, 0x1898297c, -0x4873af80, 0x7789dd2d },
        { 0x0d6ef6b2, -0x4dddd7e7, 0x46ce4bfa, -0x56b5994e, 0x4f0b6cc7, 0x46c1a77a, -0x148cc731, 0x4236ccff }
    },
    {
        { -0x2588820a, 0x3bd82dbf, 0x0b98369e, 0x71b177cc, -0x7af3c967, 0x1d0e8463, 0x48e2d1f1, 0x5a71945b },
        { 0x0d55e274, -0x7b68bfb3, -0x3b52d4ad, 0x6c6663d9, -0x5256a8cc, -0x13d04f27, -0x324708c4, 0x2617e120 },
        { 0x405b4b42, 0x6f203dd5, 0x10b24509, 0x327ec604, -0x53d577ba, -0x63cb8dd0, 0x11ffeb6a, 0x77de29fc }
    },
    {
        { -0x13312d36, -0x7ca1ec71, -0x1569c466, -0x736150ed, -0x4de9f15a, -0x36a04040, -0x5278876e, 0x575e66f3 },
        { -0x7c488758, -0x4f53a837, -0x28016ed4, 0x53cdcca9, -0x00e0a624, 0x61c2b854, -0x0f218254, 0x3a1a2cf0 },
        { -0x377034c6, -0x667fc5d9, 0x275ec0b0, 0x345a6789, -0x0093d41b, 0x459789d0, 0x1e70a8b2, 0x62f88265 }
    },
    {
        { 0x698a19e0, 0x6d822986, 0x74d78a71, -0x2367de1f, -0x0934e0b9, 0x41a85f31, -0x432563af, 0x352721c2 },
        { 0x59ff1be4, 0x085ae2c7, 0x3b0e40b7, 0x149145c9, 0x7ff27379, -0x3b981806, -0x2a38c56b, 0x4eeecf0a },
        { 0x213fc985, 0x48329952, 0x368a1746, 0x1087cf0d, 0x66c15aa5, -0x71ad9e4f, 0x2ed24c21, 0x2d5b2d84 }
    },
    {
        { 0x196ac533, 0x5eb7d13d, -0x247f41d5, 0x377234ec, 0x7cf5ae24, -0x1ebb3004, -0x3bbe5314, 0x5226bcf9 },
        { -0x142c212f, 0x02cfebd9, 0x39021974, -0x2ba4de89, -0x01cf5e49, 0x7576f813, -0x5cb1093e, 0x5691b6f9 },
        { 0x23e5b547, 0x79ee6c72, -0x7ccf2987, 0x6f5f5076, 0x6d8adce9, -0x128c1e17, 0x1d8ccc03, 0x27c3da1e }
    },
    {
        { 0x630ef9f6, 0x28302e71, 0x2b64cee0, -0x3d2b5dfd, 0x4b6292be, 0x09082030, -0x57d520e8, 0x5fca747a },
        { 0x3fe24c74, 0x7eb9efb2, 0x1651be01, 0x3e50f49f, 0x21858dea, 0x3ea732dc, 0x5bb810f9, 0x17377bd7 },
        { 0x5c258ea5, 0x232a03c3, 0x6bcb0cf1, -0x790dc5d4, 0x2e442166, 0x3dad8d0d, -0x548979d5, 0x04a8933c }
    },
},
{
    {
        { -0x736c95b0, 0x69082b0e, -0x3e253a4a, -0x06365fcb, -0x3b2049cc, 0x6fb73e54, 0x1d2bc140, 0x4005419b },
        { 0x22943dff, -0x2d39fb4a, 0x44cfb3a0, -0x43734132, -0x687f7988, 0x5d254ff3, 0x3b1ca6bf, 0x0fa3614f },
        { -0x46417d10, -0x5ffc0143, 0x3a44ac90, 0x2089c1af, 0x1954fa8e, -0x07b6606f, -0x10bf54be, 0x1fba218a }
    },
    {
        { 0x3e7b0194, 0x4f3e5704, 0x08daaf7f, -0x57e2c112, -0x6623210f, -0x37c63955, -0x00889e2b, 0x6c535d13 },
        { -0x05370ac2, -0x54ab6bb8, 0x7ba63741, -0x7e091766, 0x6c2b5e01, 0x74fd6c7d, -0x573791be, 0x392e3aca },
        { 0x3e8a35af, 0x4cbd34e9, 0x5887e816, 0x2e078144, -0x0d654f55, 0x19319c76, -0x2af53ec5, 0x25e17fe4 }
    },
    {
        { 0x76f121a7, -0x6ea0800b, 0x2fcd87e3, -0x3cb5cdd9, 0x4d1be526, -0x3345d022, -0x76967665, 0x6bba828f },
        { 0x1e04f676, 0x0a289bd7, -0x29bdf06b, 0x208e1c52, 0x34691fab, 0x5186d8b0, 0x2a9fb351, 0x25575144 },
        { -0x6f01c6ff, -0x1d2e439a, -0x5f66852b, 0x4cb54a18, -0x507b9f2c, -0x68e296ec, 0x7f6b7be4, 0x559d504f }
    },
    {
        { -0x092d9903, -0x63b76e19, 0x0307781b, 0x0744a19b, 0x6061e23b, -0x77c770e3, 0x354bd50e, 0x123ea6a3 },
        { -0x4c14ab2b, -0x588c7c88, -0x5aaac384, 0x1d69d366, -0x06d7ff46, 0x0a26cf62, -0x7f81cde9, 0x01ab12d5 },
        { 0x41e32d96, 0x118d1890, -0x27cea7b8, -0x46121c3e, -0x27cdba27, 0x1eab4271, -0x36e75eac, 0x4a3961e2 }
    },
    {
        { -0x0cdcc0e2, 0x0327d644, 0x34fcf016, 0x499a260e, -0x0d254687, -0x7c4a58ea, -0x642beee1, 0x68aceead },
        { -0x07194460, 0x71dc3be0, 0x7effe30a, -0x293107cc, -0x1ec5b896, -0x566dbda1, -0x04e2489d, 0x2cd6bce3 },
        { -0x0c283df0, 0x38b4c90e, -0x4852fbf4, 0x308e6e24, -0x4818c1dd, 0x3860d9f1, -0x4af70a69, 0x595760d5 }
    },
    {
        { -0x02fdd870, -0x77d53415, -0x3beea8a0, -0x7650ccfb, 0x7d3473f4, 0x65f492e3, 0x54515a2b, 0x2cb2c5df },
        { 0x04aa6397, 0x6129bfe1, -0x5b580335, -0x7069fff8, 0x7d909458, 0x3f8bc089, -0x234d6e57, 0x709fa43e },
        { 0x63fd2aca, -0x14f5a274, 0x2e694eff, -0x2dd43e9a, -0x07344fc6, 0x2723f36e, -0x0f37ece1, 0x70f029ec }
    },
    {
        { 0x5e10b0b9, 0x2a6aafaa, -0x10fbe557, 0x78f0a370, -0x55c529e1, 0x773efb77, -0x58b4261f, 0x44eca5a2 },
        { 0x2eed3e33, 0x461307b3, -0x5baa7e19, -0x51fbd0cd, 0x195f0366, -0x36bbb62d, 0x6c314858, 0x0b7d5d8a },
        { 0x7b95d543, 0x25d44832, -0x5ccbf0e3, 0x70d38300, 0x60e1c52b, -0x21e3ace4, 0x2c7de9e4, 0x27222451 }
    },
    {
        { 0x42a975fc, -0x40844476, -0x69525ca8, -0x73a3c689, -0x321255b8, -0x1d803891, -0x0943df5a, 0x19735fd7 },
        { 0x49c5342e, 0x1abc92af, -0x4d190530, -0x001127ef, -0x0337b1d7, -0x105d7373, -0x5bb33abd, 0x11b5df18 },
        { 0x42c84266, -0x1c546f30, 0x7f19547e, -0x147b71f1, 0x65a497b9, 0x2503a1d0, -0x6e2076a1, 0x0fef9111 }
    },
},
{
    {
        { 0x5b1c16b7, 0x6ab5dcb8, 0x3c7b27a5, -0x6b3f0318, 0x735517be, -0x5b4ee3e6, -0x45f15056, 0x499238d0 },
        { -0x54e39147, -0x4eaf835f, 0x16b687b3, -0x42bb70c2, 0x2c7a91ab, 0x3455fb7f, 0x2f2adec1, 0x7579229e },
        { 0x7aba8b57, -0x130b91ae, -0x742e9b85, 0x15a08c47, 0x5f706fef, 0x7af1c6a6, -0x0fc5cf2b, 0x6345fa78 }
    },
    {
        { -0x42270f5c, -0x6c2c3417, -0x02e88cfe, -0x24ead3e5, 0x7f17a875, 0x7dbddc6d, -0x70bd9102, 0x3e1a71cc },
        { 0x1015e7a1, -0x20fd06a1, -0x564bfd9d, 0x790ec41d, 0x33ea1107, 0x4d3a0ea1, -0x1cc50737, 0x54f70be7 },
        { -0x6f45429e, -0x37c35c1d, 0x0291c833, -0x7f121c99, -0x2c86ff3c, -0x377fc734, 0x1ec31fa1, 0x2c5fc023 }
    },
    {
        { 0x02456e65, -0x3bdd1b2f, -0x352b846f, -0x78beb53f, -0x5d490023, 0x1592e2bb, -0x0a3deff1, 0x75d9d2bf },
        { 0x17038b4f, -0x01456ee9, -0x3621107f, -0x1aedc8df, 0x5d0d8834, 0x1c97e4e7, 0x23dc3bc6, 0x68afae7a },
        { 0x3626e81c, 0x5bd9b476, -0x435fd123, -0x766996ca, 0x61f077b3, 0x0a41193d, 0x00ce5471, 0x3097a242 }
    },
    {
        { 0x6695c486, -0x5e9d18dc, 0x35a89607, 0x131d6334, -0x5f2ed5c9, 0x30521561, -0x59504c9d, 0x56704bad },
        { -0x380747b4, 0x57427734, 0x01b270e9, -0x0ebe5ec2, -0x4b1a9b5a, 0x02d1adfe, -0x317c42b8, 0x4bb23d92 },
        { 0x52f912b9, -0x5093b559, -0x27988f38, 0x5e665f6c, -0x5c3732a8, 0x4c35ac83, 0x10a58a7e, 0x2b7a29c0 }
    },
    {
        { -0x40fff792, 0x33810a23, -0x18c90084, -0x50316da2, -0x1db6dd2c, 0x3d60e670, 0x4f96061b, 0x11ce9e71 },
        { -0x2f3e313d, -0x3bff8089, -0x453b6d08, -0x72efdf4a, 0x7e69daaf, 0x32ec29d5, -0x626a0320, 0x59940875 },
        { -0x27ea453f, 0x219ef713, 0x485be25c, -0x0ebeb9a3, 0x4e513c51, 0x6d5447cc, 0x5ef44393, 0x174926be }
    },
    {
        { -0x6c15fdd2, 0x3ef5d415, 0x0ed0eed6, 0x5cbcc1a2, 0x07382c8c, -0x702db131, 0x06d8e1ad, 0x6fa42ead },
        { -0x03a42a45, -0x4a214d07, -0x1e27ef1f, -0x6d2558d6, -0x48d5e3a7, -0x503b3024, 0x3fc22a24, 0x497d7881 },
        { 0x1f73371f, -0x1d897db6, 0x4f5b6736, 0x7f7cf01c, 0x04fa46e7, 0x7e201fe3, 0x57808c96, 0x785a36a3 }
    },
    {
        { 0x5d517bc3, 0x07044298, -0x519ac988, 0x6acd56c7, -0x67a5889d, 0x00a27983, -0x1aed99d5, 0x5167effa },
        { 0x63014d2b, -0x7da04203, 0x6ca7578b, -0x37adc964, 0x5c0b5df0, 0x5b2fcd28, 0x58048c8f, 0x12ab214c },
        { 0x0f53c4b6, -0x42b1561f, -0x7536e5ec, 0x1673dc5f, 0x2acc1aba, -0x5707e5b2, 0x24332a25, 0x33a92a79 }
    },
    {
        { 0x218f2ada, 0x7ba95ba0, 0x330fb9ca, -0x300bdd79, 0x56c6d907, -0x2525b693, -0x0b4111ac, 0x5380c296 },
        { 0x27996c02, -0x622e0b67, -0x1fb2e8ae, 0x0cb3b058, 0x7fd02c3e, 0x1f7e8896, -0x3474c14f, 0x2f964268 },
        { 0x66898d0a, -0x62b0d8fc, 0x0aff3f7a, 0x3d098799, 0x67daba45, -0x2f610c9e, 0x7b1c669c, 0x7761455e }
    },
},
};
#elif defined(CURVED25519_128BIT)
static const ge_precomp base[32][8] = {
{
    {
        { 0x493c6f58c3b85, 0x0df7181c325f7, 0x0f50b0b3e4cb7, 0x5329385a44c32, 0x07cf9d3a33d4b },
        { 0x03905d740913e, 0x0ba2817d673a2, 0x23e2827f4e67c, 0x133d2e0c21a34, 0x44fd2f9298f81 },
        { 0x11205877aaa68, 0x479955893d579, 0x50d66309b67a0, 0x2d42d0dbee5ee, 0x6f117b689f0c6 },
    },
    {
        { 0x4e7fc933c71d7, 0x2cf41feb6b244, 0x7581c0a7d1a76, 0x7172d534d32f0, 0x590c063fa87d2 },
        { 0x1a56042b4d5a8, 0x189cc159ed153, 0x5b8deaa3cae04, 0x2aaf04f11b5d8, 0x6bb595a669c92 },
        { 0x2a8b3a59b7a5f, 0x3abb359ef087f, 0x4f5a8c4db05af, 0x5b9a807d04205, 0x701af5b13ea50 },
    },
    {
        { 0x5b0a84cee9730, 0x61d10c97155e4, 0x4059cc8096a10, 0x47a608da8014f, 0x7a164e1b9a80f },
        { 0x11fe8a4fcd265, 0x7bcb8374faacc, 0x52f5af4ef4d4f, 0x5314098f98d10, 0x2ab91587555bd },
        { 0x6933f0dd0d889, 0x44386bb4c4295, 0x3cb6d3162508c, 0x26368b872a2c6, 0x5a2826af12b9b },
    },
    {
        { 0x351b98efc099f, 0x68fbfa4a7050e, 0x42a49959d971b, 0x393e51a469efd, 0x680e910321e58 },
        { 0x6050a056818bf, 0x62acc1f5532bf, 0x28141ccc9fa25, 0x24d61f471e683, 0x27933f4c7445a },
        { 0x3fbe9c476ff09, 0x0af6b982e4b42, 0x0ad1251ba78e5, 0x715aeedee7c88, 0x7f9d0cbf63553 },
    },
    {
        { 0x2bc4408a5bb33, 0x078ebdda05442, 0x2ffb112354123, 0x375ee8df5862d, 0x2945ccf146e20 },
        { 0x182c3a447d6ba, 0x22964e536eff2, 0x192821f540053, 0x2f9f19e788e5c, 0x154a7e73eb1b5 },
        { 0x3dbf1812a8285, 0x0fa17ba3f9797, 0x6f69cb49c3820, 0x34d5a0db3858d, 0x43aabe696b3bb },
    },
    {
        { 0x4eeeb77157131, 0x1201915f10741, 0x1669cda6c9c56, 0x45ec032db346d, 0x51e57bb6a2cc3 },
        { 0x006b67b7d8ca4, 0x084fa44e72933, 0x1154ee55d6f8a, 0x4425d842e7390, 0x38b64c41ae417 },
        { 0x4326702ea4b71, 0x06834376030b5, 0x0ef0512f9c380, 0x0f1a9f2512584, 0x10b8e91a9f0d6 },
    },
    {
        { 0x25cd0944ea3bf, 0x75673b81a4d63, 0x150b925d1c0d4, 0x13f38d9294114, 0x461bea69283c9 },
        { 0x72c9aaa3221b1, 0x267774474f74d, 0x064b0e9b28085, 0x3f04ef53b27c9, 0x1d6edd5d2e531 },
        { 0x36dc801b8b3a2, 0x0e0a7d4935e30, 0x1deb7cecc0d7d, 0x053a94e20dd2c, 0x7a9fbb1c6a0f9 },
    },
    {
        { 0x7596604dd3e8f, 0x6fc510e058b36, 0x3670c8db2cc0d, 0x297d899ce332f, 0x0915e76061bce },
        { 0x75dedf39234d9, 0x01c36ab1f3c54, 0x0f08fee58f5da, 0x0e19613a0d637, 0x3a9024a1320e0 },
        { 0x1f5d9c9a2911a, 0x7117994fafcf8, 0x2d8a8cae28dc5, 0x74ab1b2090c87, 0x26907c5c2ecc4 },
    },
},
{
    {
        { 0x4dd0e632f9c1d, 0x2ced12622a5d9, 0x18de9614742da, 0x79ca96fdbb5d4, 0x6dd37d49a00ee },
        { 0x3635449aa515e, 0x3e178d0475dab, 0x50b4712a19712, 0x2dcc2860ff4ad, 0x30d76d6f03d31 },
        { 0x444172106e4c7, 0x01251afed2d88, 0x534fc9bed4f5a, 0x5d85a39cf5234, 0x10c697112e864 },
    },
    {
        { 0x62aa08358c805, 0x46f440848e194, 0x447b771a8f52b, 0x377ba3269d31d, 0x03bf9baf55080 },
        { 0x3c4277dbe5fde, 0x5a335afd44c92, 0x0c1164099753e, 0x70487006fe423, 0x25e61cabed66f },
        { 0x3e128cc586604, 0x5968b2e8fc7e2, 0x049a3d5bd61cf, 0x116505b1ef6e6, 0x566d78634586e },
    },
    {
        { 0x54285c65a2fd0, 0x55e62ccf87420, 0x46bb961b19044, 0x1153405712039, 0x14fba5f34793b },
        { 0x7a49f9cc10834, 0x2b513788a22c6, 0x5ff4b6ef2395b, 0x2ec8e5af607bf, 0x33975bca5ecc3 },
        { 0x746166985f7d4, 0x09939000ae79a, 0x5844c7964f97a, 0x13617e1f95b3d, 0x14829cea83fc5 },
    },
    {
        { 0x70b2f4e71ecb8, 0x728148efc643c, 0x0753e03995b76, 0x5bf5fb2ab6767, 0x05fc3bc4535d7 },
        { 0x37b8497dd95c2, 0x61549d6b4ffe8, 0x217a22db1d138, 0x0b9cf062eb09e, 0x2fd9c71e5f758 },
        { 0x0b3ae52afdedd, 0x19da76619e497, 0x6fa0654d2558e, 0x78219d25e41d4, 0x373767475c651 },
    },
    {
        { 0x095cb14246590, 0x002d82aa6ac68, 0x442f183bc4851, 0x6464f1c0a0644, 0x6bf5905730907 },
        { 0x299fd40d1add9, 0x5f2de9a04e5f7, 0x7c0eebacc1c59, 0x4cca1b1f8290a, 0x1fbea56c3b18f },
        { 0x778f1e1415b8a, 0x6f75874efc1f4, 0x28a694019027f, 0x52b37a96bdc4d, 0x02521cf67a635 },
    },
    {
        { 0x46720772f5ee4, 0x632c0f359d622, 0x2b2092ba3e252, 0x662257c112680, 0x001753d9f7cd6 },
        { 0x7ee0b0a9d5294, 0x381fbeb4cca27, 0x7841f3a3e639d, 0x676ea30c3445f, 0x3fa00a7e71382 },
        { 0x1232d963ddb34, 0x35692e70b078d, 0x247ca14777a1f, 0x6db556be8fcd0, 0x12b5fe2fa048e },
    },
    {
        { 0x37c26ad6f1e92, 0x46a0971227be5, 0x4722f0d2d9b4c, 0x3dc46204ee03a, 0x6f7e93c20796c },
        { 0x0fbc496fce34d, 0x575be6b7dae3e, 0x4a31585cee609, 0x037e9023930ff, 0x749b76f96fb12 },
        { 0x2f604aea6ae05, 0x637dc939323eb, 0x3fdad9b048d47, 0x0a8b0d4045af7, 0x0fcec10f01e02 },
    },
    {
        { 0x2d29dc4244e45, 0x6927b1bc147be, 0x0308534ac0839, 0x4853664033f41, 0x413779166feab },
        { 0x558a649fe1e44, 0x44635aeefcc89, 0x1ff434887f2ba, 0x0f981220e2d44, 0x4901aa7183c51 },
        { 0x1b7548c1af8f0, 0x7848c53368116, 0x01b64e7383de9, 0x109fbb0587c8f, 0x41bb887b726d1 },
    },
},
{
    {
        { 0x34c597c6691ae, 0x7a150b6990fc4, 0x52beb9d922274, 0x70eed7164861a, 0x0a871e070c6a9 },
        { 0x07d44744346be, 0x282b6a564a81d, 0x4ed80f875236b, 0x6fbbe1d450c50, 0x4eb728c12fcdb },
        { 0x1b5994bbc8989, 0x74b7ba84c0660, 0x75678f1cdaeb8, 0x23206b0d6f10c, 0x3ee7300f2685d },
    },
    {
        { 0x27947841e7518, 0x32c7388dae87f, 0x414add3971be9, 0x01850832f0ef1, 0x7d47c6a2cfb89 },
        { 0x255e49e7dd6b7, 0x38c2163d59eba, 0x3861f2a005845, 0x2e11e4ccbaec9, 0x1381576297912 },
        { 0x2d0148ef0d6e0, 0x3522a8de787fb, 0x2ee055e74f9d2, 0x64038f6310813, 0x148cf58d34c9e },
    },
    {
        { 0x72f7d9ae4756d, 0x7711e690ffc4a, 0x582a2355b0d16, 0x0dccfe885b6b4, 0x278febad4eaea },
        { 0x492f67934f027, 0x7ded0815528d4, 0x58461511a6612, 0x5ea2e50de1544, 0x3ff2fa1ebd5db },
        { 0x2681f8c933966, 0x3840521931635, 0x674f14a308652, 0x3bd9c88a94890, 0x4104dd02fe9c6 },
    },
    {
        { 0x14e06db096ab8, 0x1219c89e6b024, 0x278abd486a2db, 0x240b292609520, 0x0165b5a48efca },
        { 0x2bf5e1124422a, 0x673146756ae56, 0x14ad99a87e830, 0x1eaca65b080fd, 0x2c863b00afaf5 },
        { 0x0a474a0846a76, 0x099a5ef981e32, 0x2a8ae3c4bbfe6, 0x45c34af14832c, 0x591b67d9bffec },
    },
    {
        { 0x1b3719f18b55d, 0x754318c83d337, 0x27c17b7919797, 0x145b084089b61, 0x489b4f8670301 },
        { 0x70d1c80b49bfa, 0x3d57e7d914625, 0x3c0722165e545, 0x5e5b93819e04f, 0x3de02ec7ca8f7 },
        { 0x2102d3aeb92ef, 0x68c22d50c3a46, 0x42ea89385894e, 0x75f9ebf55f38c, 0x49f5fbba496cb },
    },
    {
        { 0x5628c1e9c572e, 0x598b108e822ab, 0x55d8fae29361a, 0x0adc8d1a97b28, 0x06a1a6c288675 },
        { 0x49a108a5bcfd4, 0x6178c8e7d6612, 0x1f03473710375, 0x73a49614a6098, 0x5604a86dcbfa6 },
        { 0x0d1d47c1764b6, 0x01c08316a2e51, 0x2b3db45c95045, 0x1634f818d300c, 0x20989e89fe274 },
    },
    {
        { 0x4278b85eaec2e, 0x0ef59657be2ce, 0x72fd169588770, 0x2e9b205260b30, 0x730b9950f7059 },
        { 0x777fd3a2dcc7f, 0x594a9fb124932, 0x01f8e80ca15f0, 0x714d13cec3269, 0x0403ed1d0ca67 },
        { 0x32d35874ec552, 0x1f3048df1b929, 0x300d73b179b23, 0x6e67be5a37d0b, 0x5bd7454308303 },
    },
    {
        { 0x4932115e7792a, 0x457b9bbb930b8, 0x68f5d8b193226, 0x4164e8f1ed456, 0x5bb7db123067f },
        { 0x2d19528b24cc2, 0x4ac66b8302ff3, 0x701c8d9fdad51, 0x6c1b35c5b3727, 0x133a78007380a },
        { 0x1f467c6ca62be, 0x2c4232a5dc12c, 0x7551dc013b087, 0x0690c11b03bcd, 0x740dca6d58f0e },
    },
},
{
    {
        { 0x28c570478433c, 0x1d8502873a463, 0x7641e7eded49c, 0x1ecedd54cf571, 0x2c03f5256c2b0 },
        { 0x0ee0752cfce4e, 0x660dd8116fbe9, 0x55167130fffeb, 0x1c682b885955c, 0x161d25fa963ea },
        { 0x718757b53a47d, 0x619e18b0f2f21, 0x5fbdfe4c1ec04, 0x5d798c81ebb92, 0x699468bdbd96b },
    },
    {
        { 0x53de66aa91948, 0x045f81a599b1b, 0x3f7a8bd214193, 0x71d4da412331a, 0x293e1c4e6c4a2 },
        { 0x72f46f4dafecf, 0x2948ffadef7a3, 0x11ecdfdf3bc04, 0x3c2e98ffeed25, 0x525219a473905 },
        { 0x6134b925112e1, 0x6bb942bb406ed, 0x070c445c0dde2, 0x411d822c4d7a3, 0x5b605c447f032 },
    },
    {
        { 0x1fec6f0e7f04c, 0x3cebc692c477d, 0x077986a19a95e, 0x6eaaaa1778b0f, 0x2f12fef4cc5ab },
        { 0x5805920c47c89, 0x1924771f9972c, 0x38bbddf9fc040, 0x1f7000092b281, 0x24a76dcea8aeb },
        { 0x522b2dfc0c740, 0x7e8193480e148, 0x33fd9a04341b9, 0x3c863678a20bc, 0x5e607b2518a43 },
    },
    {
        { 0x4431ca596cf14, 0x015da7c801405, 0x03c9b6f8f10b5, 0x0346922934017, 0x201f33139e457 },
        { 0x31d8f6cdf1818, 0x1f86c4b144b16, 0x39875b8d73e9d, 0x2fbf0d9ffa7b3, 0x5067acab6ccdd },
        { 0x27f6b08039d51, 0x4802f8000dfaa, 0x09692a062c525, 0x1baea91075817, 0x397cba8862460 },
    },
    {
        { 0x5c3fbc81379e7, 0x41bbc255e2f02, 0x6a3f756998650, 0x1297fd4e07c42, 0x771b4022c1e1c },
        { 0x13093f05959b2, 0x1bd352f2ec618, 0x075789b88ea86, 0x61d1117ea48b9, 0x2339d320766e6 },
        { 0x5d986513a2fa7, 0x63f3a99e11b0f, 0x28a0ecfd6b26d, 0x53b6835e18d8f, 0x331a189219971 },
    },
    {
        { 0x12f3a9d7572af, 0x10d00e953c4ca, 0x603df116f2f8a, 0x33dc276e0e088, 0x1ac9619ff649a },
        { 0x66f45fb4f80c6, 0x3cc38eeb9fea2, 0x107647270db1f, 0x710f1ea740dc8, 0x31167c6b83bdf },
        { 0x33842524b1068, 0x77dd39d30fe45, 0x189432141a0d0, 0x088fe4eb8c225, 0x612436341f08b },
    },
    {
        { 0x349e31a2d2638, 0x0137a7fa6b16c, 0x681ae92777edc, 0x222bfc5f8dc51, 0x1522aa3178d90 },
        { 0x541db874e898d, 0x62d80fb841b33, 0x03e6ef027fa97, 0x7a03c9e9633e8, 0x46ebe2309e5ef },
        { 0x02f5369614938, 0x356e5ada20587, 0x11bc89f6bf902, 0x036746419c8db, 0x45fe70f505243 },
    },
    {
        { 0x24920c8951491, 0x107ec61944c5e, 0x72752e017c01f, 0x122b7dda2e97a, 0x16619f6db57a2 },
        { 0x075a6960c0b8c, 0x6dde1c5e41b49, 0x42e3f516da341, 0x16a03fda8e79e, 0x428d1623a0e39 },
        { 0x74a4401a308fd, 0x06ed4b9558109, 0x746f1f6a08867, 0x4636f5c6f2321, 0x1d81592d60bd3 },
    },
},
{
    {
        { 0x5b69f7b85c5e8, 0x17a2d175650ec, 0x4cc3e6dbfc19e, 0x73e1d3873be0e, 0x3a5f6d51b0af8 },
        { 0x68756a60dac5f, 0x55d757b8aec26, 0x3383df45f80bd, 0x6783f8c9f96a6, 0x20234a7789ecd },
        { 0x20db67178b252, 0x73aa3da2c0eda, 0x79045c01c70d3, 0x1b37b15251059, 0x7cd682353cffe },
    },
    {
        { 0x5cd6068acf4f3, 0x3079afc7a74cc, 0x58097650b64b4, 0x47fabac9c4e99, 0x3ef0253b2b2cd },
        { 0x1a45bd887fab6, 0x65748076dc17c, 0x5b98000aa11a8, 0x4a1ecc9080974, 0x2838c8863bdc0 },
        { 0x3b0cf4a465030, 0x022b8aef57a2d, 0x2ad0677e925ad, 0x4094167d7457a, 0x21dcb8a606a82 },
    },
    {
        { 0x500fabe7731ba, 0x7cc53c3113351, 0x7cf65fe080d81, 0x3c5d966011ba1, 0x5d840dbf6c6f6 },
        { 0x004468c9d9fc8, 0x5da8554796b8c, 0x3b8be70950025, 0x6d5892da6a609, 0x0bc3d08194a31 },
        { 0x6380d309fe18b, 0x4d73c2cb8ee0d, 0x6b882adbac0b6, 0x36eabdddd4cbe, 0x3a4276232ac19 },
    },
    {
        { 0x0c172db447ecb, 0x3f8c505b7a77f, 0x6a857f97f3f10, 0x4fcc0567fe03a, 0x0770c9e824e1a },
        { 0x2432c8a7084fa, 0x47bf73ca8a968, 0x1639176262867, 0x5e8df4f8010ce, 0x1ff177cea16de },
        { 0x1d99a45b5b5fd, 0x523674f2499ec, 0x0f8fa26182613, 0x58f7398048c98, 0x39f264fd41500 },
    },
    {
        { 0x34aabfe097be1, 0x43bfc03253a33, 0x29bc7fe91b7f3, 0x0a761e4844a16, 0x65c621272c35f },
        { 0x53417dbe7e29c, 0x54573827394f5, 0x565eea6f650dd, 0x42050748dc749, 0x1712d73468889 },
        { 0x389f8ce3193dd, 0x2d424b8177ce5, 0x073fa0d3440cd, 0x139020cd49e97, 0x22f9800ab19ce },
    },
    {
        { 0x29fdd9a6efdac, 0x7c694a9282840, 0x6f7cdeee44b3a, 0x55a3207b25cc3, 0x4171a4d38598c },
        { 0x2368a3e9ef8cb, 0x454aa08e2ac0b, 0x490923f8fa700, 0x372aa9ea4582f, 0x13f416cd64762 },
        { 0x758aa99c94c8c, 0x5f6001700ff44, 0x7694e488c01bd, 0x0d5fde948eed6, 0x508214fa574bd },
    },
    {
        { 0x215bb53d003d6, 0x1179e792ca8c3, 0x1a0e96ac840a2, 0x22393e2bb3ab6, 0x3a7758a4c86cb },
        { 0x269153ed6fe4b, 0x72a23aef89840, 0x052be5299699c, 0x3a5e5ef132316, 0x22f960ec6faba },
        { 0x111f693ae5076, 0x3e3bfaa94ca90, 0x445799476b887, 0x24a0912464879, 0x5d9fd15f8de7f },
    },
    {
        { 0x44d2aeed7521e, 0x50865d2c2a7e4, 0x2705b5238ea40, 0x46c70b25d3b97, 0x3bc187fa47eb9 },
        { 0x408d36d63727f, 0x5faf8f6a66062, 0x2bb892da8de6b, 0x769d4f0c7e2e6, 0x332f35914f8fb },
        { 0x70115ea86c20c, 0x16d88da24ada8, 0x1980622662adf, 0x501ebbc195a9d, 0x450d81ce906fb },
    },
},
{
    {
        { 0x4d8961cae743f, 0x6bdc38c7dba0e, 0x7d3b4a7e1b463, 0x0844bdee2adf3, 0x4cbad279663ab },
        { 0x3b6a1a6205275, 0x2e82791d06dcf, 0x23d72caa93c87, 0x5f0b7ab68aaf4, 0x2de25d4ba6345 },
        { 0x19024a0d71fcd, 0x15f65115f101a, 0x4e99067149708, 0x119d8d1cba5af, 0x7d7fbcefe2007 },
    },
    {
        { 0x45dc5f3c29094, 0x3455220b579af, 0x070c1631e068a, 0x26bc0630e9b21, 0x4f9cd196dcd8d },
        { 0x71e6a266b2801, 0x09aae73e2df5d, 0x40dd8b219b1a3, 0x546fb4517de0d, 0x5975435e87b75 },
        { 0x297d86a7b3768, 0x4835a2f4c6332, 0x070305f434160, 0x183dd014e56ae, 0x7ccdd084387a0 },
    },
    {
        { 0x484186760cc93, 0x7435665533361, 0x02f686336b801, 0x5225446f64331, 0x3593ca848190c },
        { 0x6422c6d260417, 0x212904817bb94, 0x5a319deb854f5, 0x7a9d4e060da7d, 0x428bd0ed61d0c },
        { 0x3189a5e849aa7, 0x6acbb1f59b242, 0x7f6ef4753630c, 0x1f346292a2da9, 0x27398308da2d6 },
    },
    {
        { 0x10e4c0a702453, 0x4daafa37bd734, 0x49f6bdc3e8961, 0x1feffdcecdae6, 0x572c2945492c3 },
        { 0x38d28435ed413, 0x4064f19992858, 0x7680fbef543cd, 0x1aadd83d58d3c, 0x269597aebe8c3 },
        { 0x7c745d6cd30be, 0x27c7755df78ef, 0x1776833937fa3, 0x5405116441855, 0x7f985498c05bc },
    },
    {
        { 0x615520fbf6363, 0x0b9e9bf74da6a, 0x4fe8308201169, 0x173f76127de43, 0x30f2653cd69b1 },
        { 0x1ce889f0be117, 0x36f6a94510709, 0x7f248720016b4, 0x1821ed1e1cf91, 0x76c2ec470a31f },
        { 0x0c938aac10c85, 0x41b64ed797141, 0x1beb1c1185e6d, 0x1ed5490600f07, 0x2f1273f159647 },
    },
    {
        { 0x08bd755a70bc0, 0x49e3a885ce609, 0x16585881b5ad6, 0x3c27568d34f5e, 0x38ac1997edc5f },
        { 0x1fc7c8ae01e11, 0x2094d5573e8e7, 0x5ca3cbbf549d2, 0x4f920ecc54143, 0x5d9e572ad85b6 },
        { 0x6b517a751b13b, 0x0cfd370b180cc, 0x5377925d1f41a, 0x34e56566008a2, 0x22dfcd9cbfe9e },
    },
    {
        { 0x459b4103be0a1, 0x59a4b3f2d2add, 0x7d734c8bb8eeb, 0x2393cbe594a09, 0x0fe9877824cde },
        { 0x3d2e0c30d0cd9, 0x3f597686671bb, 0x0aa587eb63999, 0x0e3c7b592c619, 0x6b2916c05448c },
        { 0x334d10aba913b, 0x045cdb581cfdb, 0x5e3e0553a8f36, 0x50bb3041effb2, 0x4c303f307ff00 },
    },
    {
        { 0x403580dd94500, 0x48df77d92653f, 0x38a9fe3b349ea, 0x0ea89850aafe1, 0x416b151ab706a },
        { 0x23bd617b28c85, 0x6e72ee77d5a61, 0x1a972ff174dde, 0x3e2636373c60f, 0x0d61b8f78b2ab },
        { 0x0d7efe9c136b0, 0x1ab1c89640ad5, 0x55f82aef41f97, 0x46957f317ed0d, 0x191a2af74277e },
    },
},
{
    {
        { 0x62b434f460efb, 0x294c6c0fad3fc, 0x68368937b4c0f, 0x5c9f82910875b, 0x237e7dbe00545 },
        { 0x6f74bc53c1431, 0x1c40e5dbbd9c2, 0x6c8fb9cae5c97, 0x4845c5ce1b7da, 0x7e2e0e450b5cc },
        { 0x575ed6701b430, 0x4d3e17fa20026, 0x791fc888c4253, 0x2f1ba99078ac1, 0x71afa699b1115 },
    },
    {
        { 0x23c1c473b50d6, 0x3e7671de21d48, 0x326fa5547a1e8, 0x50e4dc25fafd9, 0x00731fbc78f89 },
        { 0x66f9b3953b61d, 0x555f4283cccb9, 0x7dd67fb1960e7, 0x14707a1affed4, 0x021142e9c2b1c },
        { 0x0c71848f81880, 0x44bd9d8233c86, 0x6e8578efe5830, 0x4045b6d7041b5, 0x4c4d6f3347e15 },
    },
    {
        { 0x4ddfc988f1970, 0x4f6173ea365e1, 0x645daf9ae4588, 0x7d43763db623b, 0x38bf9500a88f9 },
        { 0x7eccfc17d1fc9, 0x4ca280782831e, 0x7b8337db1d7d6, 0x5116def3895fb, 0x193fddaaa7e47 },
        { 0x2c93c37e8876f, 0x3431a28c583fa, 0x49049da8bd879, 0x4b4a8407ac11c, 0x6a6fb99ebf0d4 },
    },
    {
        { 0x122b5b6e423c6, 0x21e50dff1ddd6, 0x73d76324e75c0, 0x588485495418e, 0x136fda9f42c5e },
        { 0x6c1bb560855eb, 0x71f127e13ad48, 0x5c6b304905aec, 0x3756b8e889bc7, 0x75f76914a3189 },
        { 0x4dfb1a305bdd1, 0x3b3ff05811f29, 0x6ed62283cd92e, 0x65d1543ec52e1, 0x022183510be8d },
    },
    {
        { 0x2710143307a7f, 0x3d88fb48bf3ab, 0x249eb4ec18f7a, 0x136115dff295f, 0x1387c441fd404 },
        { 0x766385ead2d14, 0x0194f8b06095e, 0x08478f6823b62, 0x6018689d37308, 0x6a071ce17b806 },
        { 0x3c3d187978af8, 0x7afe1c88276ba, 0x51df281c8ad68, 0x64906bda4245d, 0x3171b26aaf1ed },
    },
    {
        { 0x5b7d8b28a47d1, 0x2c2ee149e34c1, 0x776f5629afc53, 0x1f4ea50fc49a9, 0x6c514a6334424 },
        { 0x7319097564ca8, 0x1844ebc233525, 0x21d4543fdeee1, 0x1ad27aaff1bd2, 0x221fd4873cf08 },
        { 0x2204f3a156341, 0x537414065a464, 0x43c0c3bedcf83, 0x5557e706ea620, 0x48daa596fb924 },
    },
    {
        { 0x61d5dc84c9793, 0x47de83040c29e, 0x189deb26507e7, 0x4d4e6fadc479a, 0x58c837fa0e8a7 },
        { 0x28e665ca59cc7, 0x165c715940dd9, 0x0785f3aa11c95, 0x57b98d7e38469, 0x676dd6fccad84 },
        { 0x1688596fc9058, 0x66f6ad403619f, 0x4d759a87772ef, 0x7856e6173bea4, 0x1c4f73f2c6a57 },
    },
    {
        { 0x6706efc7c3484, 0x6987839ec366d, 0x0731f95cf7f26, 0x3ae758ebce4bc, 0x70459adb7daf6 },
        { 0x24fbd305fa0bb, 0x40a98cc75a1cf, 0x78ce1220a7533, 0x6217a10e1c197, 0x795ac80d1bf64 },
        { 0x1db4991b42bb3, 0x469605b994372, 0x631e3715c9a58, 0x7e9cfefcf728f, 0x5fe162848ce21 },
    },
},
{
    {
        { 0x1852d5d7cb208, 0x60d0fbe5ce50f, 0x5a1e246e37b75, 0x51aee05ffd590, 0x2b44c043677da },
        { 0x1214fe194961a, 0x0e1ae39a9e9cb, 0x543c8b526f9f7, 0x119498067e91d, 0x4789d446fc917 },
        { 0x487ab074eb78e, 0x1d33b5e8ce343, 0x13e419feb1b46, 0x2721f565de6a4, 0x60c52eef2bb9a },
    },
    {
        { 0x3c5c27cae6d11, 0x36a9491956e05, 0x124bac9131da6, 0x3b6f7de202b5d, 0x70d77248d9b66 },
        { 0x589bc3bfd8bf1, 0x6f93e6aa3416b, 0x4c0a3d6c1ae48, 0x55587260b586a, 0x10bc9c312ccfc },
        { 0x2e84b3ec2a05b, 0x69da2f03c1551, 0x23a174661a67b, 0x209bca289f238, 0x63755bd3a976f },
    },
    {
        { 0x7101897f1acb7, 0x3d82cb77b07b8, 0x684083d7769f5, 0x52b28472dce07, 0x2763751737c52 },
        { 0x7a03e2ad10853, 0x213dcc6ad36ab, 0x1a6e240d5bdd6, 0x7c24ffcf8fedf, 0x0d8cc1c48bc16 },
        { 0x402d36eb419a9, 0x7cef68c14a052, 0x0f1255bc2d139, 0x373e7d431186a, 0x70c2dd8a7ad16 },
    },
    {
        { 0x4967db8ed7e13, 0x15aeed02f523a, 0x6149591d094bc, 0x672f204c17006, 0x32b8613816a53 },
        { 0x194509f6fec0e, 0x528d8ca31acac, 0x7826d73b8b9fa, 0x24acb99e0f9b3, 0x2e0fac6363948 },
        { 0x7f7bee448cd64, 0x4e10f10da0f3c, 0x3936cb9ab20e9, 0x7a0fc4fea6cd0, 0x4179215c735a4 },
    },
    {
        { 0x633b9286bcd34, 0x6cab3badb9c95, 0x74e387edfbdfa, 0x14313c58a0fd9, 0x31fa85662241c },
        { 0x094e7d7dced2a, 0x068fa738e118e, 0x41b640a5fee2b, 0x6bb709df019d4, 0x700344a30cd99 },
        { 0x26c422e3622f4, 0x0f3066a05b5f0, 0x4e2448f0480a6, 0x244cde0dbf095, 0x24bb2312a9952 },
    },
    {
        { 0x00c2af5f85c6b, 0x0609f4cf2883f, 0x6e86eb5a1ca13, 0x68b44a2efccd1, 0x0d1d2af9ffeb5 },
        { 0x0ed1732de67c3, 0x308c369291635, 0x33ef348f2d250, 0x004475ea1a1bb, 0x0fee3e871e188 },
        { 0x28aa132621edf, 0x42b244caf353b, 0x66b064cc2e08a, 0x6bb20020cbdd3, 0x16acd79718531 },
    },
    {
        { 0x1c6c57887b6ad, 0x5abf21fd7592b, 0x50bd41253867a, 0x3800b71273151, 0x164ed34b18161 },
        { 0x772af2d9b1d3d, 0x6d486448b4e5b, 0x2ce58dd8d18a8, 0x1849f67503c8b, 0x123e0ef6b9302 },
        { 0x6d94c192fe69a, 0x5475222a2690f, 0x693789d86b8b3, 0x1f5c3bdfb69dc, 0x78da0fc61073f },
    },
    {
        { 0x780f1680c3a94, 0x2a35d3cfcd453, 0x005e5cdc7ddf8, 0x6ee888078ac24, 0x054aa4b316b38 },
        { 0x15d28e52bc66a, 0x30e1e0351cb7e, 0x30a2f74b11f8c, 0x39d120cd7de03, 0x2d25deeb256b1 },
        { 0x0468d19267cb8, 0x38cdca9b5fbf9, 0x1bbb05c2ca1e2, 0x3b015758e9533, 0x134610a6ab7da },
    },
},
{
    {
        { 0x265e777d1f515, 0x0f1f54c1e39a5, 0x2f01b95522646, 0x4fdd8db9dde6d, 0x654878cba97cc },
        { 0x38ec78df6b0fe, 0x13caebea36a22, 0x5ebc6e54e5f6a, 0x32804903d0eb8, 0x2102fdba2b20d },
        { 0x6e405055ce6a1, 0x5024a35a532d3, 0x1f69054daf29d, 0x15d1d0d7a8bd5, 0x0ad725db29ecb },
    },
    {
        { 0x7bc0c9b056f85, 0x51cfebffaffd8, 0x44abbe94df549, 0x7ecbbd7e33121, 0x4f675f5302399 },
        { 0x267b1834e2457, 0x6ae19c378bb88, 0x7457b5ed9d512, 0x3280d783d05fb, 0x4aefcffb71a03 },
        { 0x536360415171e, 0x2313309077865, 0x251444334afbc, 0x2b0c3853756e8, 0x0bccbb72a2a86 },
    },
    {
        { 0x55e4c50fe1296, 0x05fdd13efc30d, 0x1c0c6c380e5ee, 0x3e11de3fb62a8, 0x6678fd69108f3 },
        { 0x6962feab1a9c8, 0x6aca28fb9a30b, 0x56db7ca1b9f98, 0x39f58497018dd, 0x4024f0ab59d6b },
        { 0x6fa31636863c2, 0x10ae5a67e42b0, 0x27abbf01fda31, 0x380a7b9e64fbc, 0x2d42e2108ead4 },
    },
    {
        { 0x17b0d0f537593, 0x16263c0c9842e, 0x4ab827e4539a4, 0x6370ddb43d73a, 0x420bf3a79b423 },
        { 0x5131594dfd29b, 0x3a627e98d52fe, 0x1154041855661, 0x19175d09f8384, 0x676b2608b8d2d },
        { 0x0ba651c5b2b47, 0x5862363701027, 0x0c4d6c219c6db, 0x0f03dff8658de, 0x745d2ffa9c0cf },
    },
    {
        { 0x6df5721d34e6a, 0x4f32f767a0c06, 0x1d5abeac76e20, 0x41ce9e104e1e4, 0x06e15be54c1dc },
        { 0x25a1e2bc9c8bd, 0x104c8f3b037ea, 0x405576fa96c98, 0x2e86a88e3876f, 0x1ae23ceb960cf },
        { 0x25d871932994a, 0x6b9d63b560b6e, 0x2df2814c8d472, 0x0fbbee20aa4ed, 0x58ded861278ec },
    },
    {
        { 0x35ba8b6c2c9a8, 0x1dea58b3185bf, 0x4b455cd23bbbe, 0x5ec19c04883f8, 0x08ba696b531d5 },
        { 0x73793f266c55c, 0x0b988a9c93b02, 0x09b0ea32325db, 0x37cae71c17c5e, 0x2ff39de85485f },
        { 0x53eeec3efc57a, 0x2fa9fe9022efd, 0x699c72c138154, 0x72a751ebd1ff8, 0x120633b4947cf },
    },
    {
        { 0x531474912100a, 0x5afcdf7c0d057, 0x7a9e71b788ded, 0x5ef708f3b0c88, 0x07433be3cb393 },
        { 0x4987891610042, 0x79d9d7f5d0172, 0x3c293013b9ec4, 0x0c2b85f39caca, 0x35d30a99b4d59 },
        { 0x144c05ce997f4, 0x4960b8a347fef, 0x1da11f15d74f7, 0x54fac19c0fead, 0x2d873ede7af6d },
    },
    {
        { 0x202e14e5df981, 0x2ea02bc3eb54c, 0x38875b2883564, 0x1298c513ae9dd, 0x0543618a01600 },
        { 0x2316443373409, 0x5de95503b22af, 0x699201beae2df, 0x3db5849ff737a, 0x2e773654707fa },
        { 0x2bdf4974c23c1, 0x4b3b9c8d261bd, 0x26ae8b2a9bc28, 0x3068210165c51, 0x4b1443362d079 },
    },
},
{
    {
        { 0x454e91c529ccb, 0x24c98c6bf72cf, 0x0486594c3d89a, 0x7ae13a3d7fa3c, 0x17038418eaf66 },
        { 0x4b7c7b66e1f7a, 0x4bea185efd998, 0x4fabc711055f8, 0x1fb9f7836fe38, 0x582f446752da6 },
        { 0x17bd320324ce4, 0x51489117898c6, 0x1684d92a0410b, 0x6e4d90f78c5a7, 0x0c2a1c4bcda28 },
    },
    {
        { 0x4814869bd6945, 0x7b7c391a45db8, 0x57316ac35b641, 0x641e31de9096a, 0x5a6a9b30a314d },
        { 0x5c7d06f1f0447, 0x7db70f80b3a49, 0x6cb4a3ec89a78, 0x43be8ad81397d, 0x7c558bd1c6f64 },
        { 0x41524d396463d, 0x1586b449e1a1d, 0x2f17e904aed8a, 0x7e1d2861d3c8e, 0x0404a5ca0afba },
    },
    {
        { 0x49e1b2a416fd1, 0x51c6a0b316c57, 0x575a59ed71bdc, 0x74c021a1fec1e, 0x39527516e7f8e },
        { 0x740070aa743d6, 0x16b64cbdd1183, 0x23f4b7b32eb43, 0x319aba58235b3, 0x46395bfdcadd9 },
        { 0x7db2d1a5d9a9c, 0x79a200b85422f, 0x355bfaa71dd16, 0x00b77ea5f78aa, 0x76579a29e822d },
    },
    {
        { 0x4b51352b434f2, 0x1327bd01c2667, 0x434d73b60c8a1, 0x3e0daa89443ba, 0x02c514bb2a277 },
        { 0x68e7e49c02a17, 0x45795346fe8b6, 0x089306c8f3546, 0x6d89f6b2f88f6, 0x43a384dc9e05b },
        { 0x3d5da8bf1b645, 0x7ded6a96a6d09, 0x6c3494fee2f4d, 0x02c989c8b6bd4, 0x1160920961548 },
    },
    {
        { 0x05616369b4dcd, 0x4ecab86ac6f47, 0x3c60085d700b2, 0x0213ee10dfcea, 0x2f637d7491e6e },
        { 0x5166929dacfaa, 0x190826b31f689, 0x4f55567694a7d, 0x705f4f7b1e522, 0x351e125bc5698 },
        { 0x49b461af67bbe, 0x75915712c3a96, 0x69a67ef580c0d, 0x54d38ef70cffc, 0x7f182d06e7ce2 },
    },
    {
        { 0x54b728e217522, 0x69a90971b0128, 0x51a40f2a963a3, 0x10be9ac12a6bf, 0x44acc043241c5 },
        { 0x48e64ab0168ec, 0x2a2bdb8a86f4f, 0x7343b6b2d6929, 0x1d804aa8ce9a3, 0x67d4ac8c343e9 },
        { 0x56bbb4f7a5777, 0x29230627c238f, 0x5ad1a122cd7fb, 0x0dea56e50e364, 0x556d1c8312ad7 },
    },
    {
        { 0x06756b11be821, 0x462147e7bb03e, 0x26519743ebfe0, 0x782fc59682ab5, 0x097abe38cc8c7 },
        { 0x740e30c8d3982, 0x7c2b47f4682fd, 0x5cd91b8c7dc1c, 0x77fa790f9e583, 0x746c6c6d1d824 },
        { 0x1c9877ea52da4, 0x2b37b83a86189, 0x733af49310da5, 0x25e81161c04fb, 0x577e14a34bee8 },
    },
    {
        { 0x6cebebd4dd72b, 0x340c1e442329f, 0x32347ffd1a93f, 0x14a89252cbbe0, 0x705304b8fb009 },
        { 0x268ac61a73b0a, 0x206f234bebe1c, 0x5b403a7cbebe8, 0x7a160f09f4135, 0x60fa7ee96fd78 },
        { 0x51d354d296ec6, 0x7cbf5a63b16c7, 0x2f50bb3cf0c14, 0x1feb385cac65a, 0x21398e0ca1635 },
    },
},
{
    {
        { 0x0aaf9b4b75601, 0x26b91b5ae44f3, 0x6de808d7ab1c8, 0x6a769675530b0, 0x1bbfb284e98f7 },
        { 0x5058a382b33f3, 0x175a91816913e, 0x4f6cdb96b8ae8, 0x17347c9da81d2, 0x5aa3ed9d95a23 },
        { 0x777e9c7d96561, 0x28e58f006ccac, 0x541bbbb2cac49, 0x3e63282994cec, 0x4a07e14e5e895 },
    },
    {
        { 0x358cdc477a49b, 0x3cc88fe02e481, 0x721aab7f4e36b, 0x0408cc9469953, 0x50af7aed84afa },
        { 0x412cb980df999, 0x5e78dd8ee29dc, 0x171dff68c575d, 0x2015dd2f6ef49, 0x3f0bac391d313 },
        { 0x7de0115f65be5, 0x4242c21364dc9, 0x6b75b64a66098, 0x0033c0102c085, 0x1921a316baebd },
    },
    {
        { 0x2ad9ad9f3c18b, 0x5ec1638339aeb, 0x5703b6559a83b, 0x3fa9f4d05d612, 0x7b049deca062c },
        { 0x22f7edfb870fc, 0x569eed677b128, 0x30937dcb0a5af, 0x758039c78ea1b, 0x6458df41e273a },
        { 0x3e37a35444483, 0x661fdb7d27b99, 0x317761dd621e4, 0x7323c30026189, 0x6093dccbc2950 },
    },
    {
        { 0x6eebe6084034b, 0x6cf01f70a8d7b, 0x0b41a54c6670a, 0x6c84b99bb55db, 0x6e3180c98b647 },
        { 0x39a8585e0706d, 0x3167ce72663fe, 0x63d14ecdb4297, 0x4be21dcf970b8, 0x57d1ea084827a },
        { 0x2b6e7a128b071, 0x5b27511755dcf, 0x08584c2930565, 0x68c7bda6f4159, 0x363e999ddd97b },
    },
    {
        { 0x048dce24baec6, 0x2b75795ec05e3, 0x3bfa4c5da6dc9, 0x1aac8659e371e, 0x231f979bc6f9b },
        { 0x043c135ee1fc4, 0x2a11c9919f2d5, 0x6334cc25dbacd, 0x295da17b400da, 0x48ee9b78693a0 },
        { 0x1de4bcc2af3c6, 0x61fc411a3eb86, 0x53ed19ac12ec0, 0x209dbc6b804e0, 0x079bfa9b08792 },
    },
    {
        { 0x1ed80a2d54245, 0x70efec72a5e79, 0x42151d42a822d, 0x1b5ebb6d631e8, 0x1ef4fb1594706 },
        { 0x03a51da300df4, 0x467b52b561c72, 0x4d5920210e590, 0x0ca769e789685, 0x038c77f684817 },
        { 0x65ee65b167bec, 0x052da19b850a9, 0x0408665656429, 0x7ab39596f9a4c, 0x575ee92a4a0bf },
    },
    {
        { 0x6bc450aa4d801, 0x4f4a6773b0ba8, 0x6241b0b0ebc48, 0x40d9c4f1d9315, 0x200a1e7e382f5 },
        { 0x080908a182fcf, 0x0532913b7ba98, 0x3dccf78c385c3, 0x68002dd5eaba9, 0x43d4e7112cd3f },
        { 0x5b967eaf93ac5, 0x360acca580a31, 0x1c65fd5c6f262, 0x71c7f15c2ecab, 0x050eca52651e4 },
    },
    {
        { 0x4397660e668ea, 0x7c2a75692f2f5, 0x3b29e7e6c66ef, 0x72ba658bcda9a, 0x6151c09fa131a },
        { 0x31ade453f0c9c, 0x3dfee07737868, 0x611ecf7a7d411, 0x2637e6cbd64f6, 0x4b0ee6c21c58f },
        { 0x55c0dfdf05d96, 0x405569dcf475e, 0x05c5c277498bb, 0x18588d95dc389, 0x1fef24fa800f0 },
    },
},
{
    {
        { 0x2aff530976b86, 0x0d85a48c0845a, 0x796eb963642e0, 0x60bee50c4b626, 0x28005fe6c8340 },
        { 0x653fb1aa73196, 0x607faec8306fa, 0x4e85ec83e5254, 0x09f56900584fd, 0x544d49292fc86 },
        { 0x7ba9f34528688, 0x284a20fb42d5d, 0x3652cd9706ffe, 0x6fd7baddde6b3, 0x72e472930f316 },
    },
    {
        { 0x3f635d32a7627, 0x0cbecacde00fe, 0x3411141eaa936, 0x21c1e42f3cb94, 0x1fee7f000fe06 },
        { 0x5208c9781084f, 0x16468a1dc24d2, 0x7bf780ac540a8, 0x1a67eced75301, 0x5a9d2e8c2733a },
        { 0x305da03dbf7e5, 0x1228699b7aeca, 0x12a23b2936bc9, 0x2a1bda56ae6e9, 0x00f94051ee040 },
    },
    {
        { 0x793bb07af9753, 0x1e7b6ecd4fafd, 0x02c7b1560fb43, 0x2296734cc5fb7, 0x47b7ffd25dd40 },
        { 0x56b23c3d330b2, 0x37608e360d1a6, 0x10ae0f3c8722e, 0x086d9b618b637, 0x07d79c7e8beab },
        { 0x3fb9cbc08dd12, 0x75c3dd85370ff, 0x47f06fe2819ac, 0x5db06ab9215ed, 0x1c3520a35ea64 },
    },
    {
        { 0x06f40216bc059, 0x3a2579b0fd9b5, 0x71c26407eec8c, 0x72ada4ab54f0b, 0x38750c3b66d12 },
        { 0x253a6bccba34a, 0x427070433701a, 0x20b8e58f9870e, 0x337c861db00cc, 0x1c3d05775d0ee },
        { 0x6f1409422e51a, 0x7856bbece2d25, 0x13380a72f031c, 0x43e1080a7f3ba, 0x0621e2c7d3304 },
    },
    {
        { 0x61796b0dbf0f3, 0x73c2f9c32d6f5, 0x6aa8ed1537ebe, 0x74e92c91838f4, 0x5d8e589ca1002 },
        { 0x060cc8259838d, 0x038d3f35b95f3, 0x56078c243a923, 0x2de3293241bb2, 0x0007d6097bd3a },
        { 0x71d950842a94b, 0x46b11e5c7d817, 0x5478bbecb4f0d, 0x7c3054b0a1c5d, 0x1583d7783c1cb },
    },
    {
        { 0x34704cc9d28c7, 0x3dee598b1f200, 0x16e1c98746d9e, 0x4050b7095afdf, 0x4958064e83c55 },
        { 0x6a2ef5da27ae1, 0x28aace02e9d9d, 0x02459e965f0e8, 0x7b864d3150933, 0x252a5f2e81ed8 },
        { 0x094265066e80d, 0x0a60f918d61a5, 0x0444bf7f30fde, 0x1c40da9ed3c06, 0x079c170bd843b },
    },
    {
        { 0x6cd50c0d5d056, 0x5b7606ae779ba, 0x70fbd226bdda1, 0x5661e53391ff9, 0x6768c0d7317b8 },
        { 0x6ece464fa6fff, 0x3cc40bca460a0, 0x6e3a90afb8d0c, 0x5801abca11228, 0x6dec05e34ac9f },
        { 0x625e5f155c1b3, 0x4f32f6f723296, 0x5ac980105efce, 0x17a61165eee36, 0x51445e14ddcd5 },
    },
    {
        { 0x147ab2bbea455, 0x1f240f2253126, 0x0c3de9e314e89, 0x21ea5a4fca45f, 0x12e990086e4fd },
        { 0x02b4b3b144951, 0x5688977966aea, 0x18e176e399ffd, 0x2e45c5eb4938b, 0x13186f31e3929 },
        { 0x496b37fdfbb2e, 0x3c2439d5f3e21, 0x16e60fe7e6a4d, 0x4d7ef889b621d, 0x77b2e3f05d3e9 },
    },
},
{
    {
        { 0x0639c12ddb0a4, 0x6180490cd7ab3, 0x3f3918297467c, 0x74568be1781ac, 0x07a195152e095 },
        { 0x7a9c59c2ec4de, 0x7e9f09e79652d, 0x6a3e422f22d86, 0x2ae8e3b836c8b, 0x63b795fc7ad32 },
        { 0x68f02389e5fc8, 0x059f1bc877506, 0x504990e410cec, 0x09bd7d0feaee2, 0x3e8fe83d032f0 },
    },
    {
        { 0x04c8de8efd13c, 0x1c67c06e6210e, 0x183378f7f146a, 0x64352ceaed289, 0x22d60899a6258 },
        { 0x315b90570a294, 0x60ce108a925f1, 0x6eff61253c909, 0x003ef0e2d70b0, 0x75ba3b797fac4 },
        { 0x1dbc070cdd196, 0x16d8fb1534c47, 0x500498183fa2a, 0x72f59c423de75, 0x0904d07b87779 },
    },
    {
        { 0x22d6648f940b9, 0x197a5a1873e86, 0x207e4c41a54bc, 0x5360b3b4bd6d0, 0x6240aacebaf72 },
        { 0x61fd4ddba919c, 0x7d8e991b55699, 0x61b31473cc76c, 0x7039631e631d6, 0x43e2143fbc1dd },
        { 0x4749c5ba295a0, 0x37946fa4b5f06, 0x724c5ab5a51f1, 0x65633789dd3f3, 0x56bdaf238db40 },
    },
    {
        { 0x0d36cc19d3bb2, 0x6ec4470d72262, 0x6853d7018a9ae, 0x3aa3e4dc2c8eb, 0x03aa31507e1e5 },
        { 0x2b9e3f53533eb, 0x2add727a806c5, 0x56955c8ce15a3, 0x18c4f070a290e, 0x1d24a86d83741 },
        { 0x47648ffd4ce1f, 0x60a9591839e9d, 0x424d5f38117ab, 0x42cc46912c10e, 0x43b261dc9aeb4 },
    },
    {
        { 0x13d8b6c951364, 0x4c0017e8f632a, 0x53e559e53f9c4, 0x4b20146886eea, 0x02b4d5e242940 },
        { 0x31e1988bb79bb, 0x7b82f46b3bcab, 0x0f7a8ce827b41, 0x5e15816177130, 0x326055cf5b276 },
        { 0x155cb28d18df2, 0x0c30d9ca11694, 0x2090e27ab3119, 0x208624e7a49b6, 0x27a6c809ae5d3 },
    },
    {
        { 0x4270ac43d6954, 0x2ed4cd95659a5, 0x75c0db37528f9, 0x2ccbcfd2c9234, 0x221503603d8c2 },
        { 0x6ebcd1f0db188, 0x74ceb4b7d1174, 0x7d56168df4f5c, 0x0bf79176fd18a, 0x2cb67174ff60a },
        { 0x6cdf9390be1d0, 0x08e519c7e2b3d, 0x253c3d2a50881, 0x21b41448e333d, 0x7b1df4b73890f },
    },
    {
        { 0x6221807f8f58c, 0x3fa92813a8be5, 0x6da98c38d5572, 0x01ed95554468f, 0x68698245d352e },
        { 0x2f2e0b3b2a224, 0x0c56aa22c1c92, 0x5fdec39f1b278, 0x4c90af5c7f106, 0x61fcef2658fc5 },
        { 0x15d852a18187a, 0x270dbb59afb76, 0x7db120bcf92ab, 0x0e7a25d714087, 0x46cf4c473daf0 },
    },
    {
        { 0x46ea7f1498140, 0x70725690a8427, 0x0a73ae9f079fb, 0x2dd924461c62b, 0x1065aae50d8cc },
        { 0x525ed9ec4e5f9, 0x022d20660684c, 0x7972b70397b68, 0x7a03958d3f965, 0x29387bcd14eb5 },
        { 0x44525df200d57, 0x2d7f94ce94385, 0x60d00c170ecb7, 0x38b0503f3d8f0, 0x69a198e64f1ce },
    },
},
{
    {
        { 0x14434dcc5caed, 0x2c7909f667c20, 0x61a839d1fb576, 0x4f23800cabb76, 0x25b2697bd267f },
        { 0x2b2e0d91a78bc, 0x3990a12ccf20c, 0x141c2e11f2622, 0x0dfcefaa53320, 0x7369e6a92493a },
        { 0x73ffb13986864, 0x3282bb8f713ac, 0x49ced78f297ef, 0x6697027661def, 0x1420683db54e4 },
    },
    {
        { 0x6bb6fc1cc5ad0, 0x532c8d591669d, 0x1af794da86c33, 0x0e0e9d86d24d3, 0x31e83b4161d08 },
        { 0x0bd1e249dd197, 0x00bcb1820568f, 0x2eab1718830d4, 0x396fd816997e6, 0x60b63bebf508a },
        { 0x0c7129e062b4f, 0x1e526415b12fd, 0x461a0fd27923d, 0x18badf670a5b7, 0x55cf1eb62d550 },
    },
    {
        { 0x6b5e37df58c52, 0x3bcf33986c60e, 0x44fb8835ceae7, 0x099dec18e71a4, 0x1a56fbaa62ba0 },
        { 0x1101065c23d58, 0x5aa1290338b0f, 0x3157e9e2e7421, 0x0ea712017d489, 0x669a656457089 },
        { 0x66b505c9dc9ec, 0x774ef86e35287, 0x4d1d944c0955e, 0x52e4c39d72b20, 0x13c4836799c58 },
    },
    {
        { 0x4fb6a5d8bd080, 0x58ae34908589b, 0x3954d977baf13, 0x413ea597441dc, 0x50bdc87dc8e5b },
        { 0x25d465ab3e1b9, 0x0f8fe27ec2847, 0x2d6e6dbf04f06, 0x3038cfc1b3276, 0x66f80c93a637b },
        { 0x537836edfe111, 0x2be02357b2c0d, 0x6dcee58c8d4f8, 0x2d732581d6192, 0x1dd56444725fd },
    },
    {
        { 0x7e60008bac89a, 0x23d5c387c1852, 0x79e5df1f533a8, 0x2e6f9f1c5f0cf, 0x3a3a450f63a30 },
        { 0x47ff83362127d, 0x08e39af82b1f4, 0x488322ef27dab, 0x1973738a2a1a4, 0x0e645912219f7 },
        { 0x72f31d8394627, 0x07bd294a200f1, 0x665be00e274c6, 0x43de8f1b6368b, 0x318c8d9393a9a },
    },
    {
        { 0x69e29ab1dd398, 0x30685b3c76bac, 0x565cf37f24859, 0x57b2ac28efef9, 0x509a41c325950 },
        { 0x45d032afffe19, 0x12fe49b6cde4e, 0x21663bc327cf1, 0x18a5e4c69f1dd, 0x224c7c679a1d5 },
        { 0x06edca6f925e9, 0x68c8363e677b8, 0x60cfa25e4fbcf, 0x1c4c17609404e, 0x05bff02328a11 },
    },
    {
        { 0x1a0dd0dc512e4, 0x10894bf5fcd10, 0x52949013f9c37, 0x1f50fba4735c7, 0x576277cdee01a },
        { 0x2137023cae00b, 0x15a3599eb26c6, 0x0687221512b3c, 0x253cb3a0824e9, 0x780b8cc3fa2a4 },
        { 0x38abc234f305f, 0x7a280bbc103de, 0x398a836695dfe, 0x3d0af41528a1a, 0x5ff418726271b },
    },
    {
        { 0x347e813b69540, 0x76864c21c3cbb, 0x1e049dbcd74a8, 0x5b4d60f93749c, 0x29d4db8ca0a0c },
        { 0x6080c1789db9d, 0x4be7cef1ea731, 0x2f40d769d8080, 0x35f7d4c44a603, 0x106a03dc25a96 },
        { 0x50aaf333353d0, 0x4b59a613cbb35, 0x223dfc0e19a76, 0x77d1e2bb2c564, 0x4ab38a51052cb },
    },
},
{
    {
        { 0x7d1ef5fddc09c, 0x7beeaebb9dad9, 0x058d30ba0acfb, 0x5cd92eab5ae90, 0x3041c6bb04ed2 },
        { 0x42b256768d593, 0x2e88459427b4f, 0x02b3876630701, 0x34878d405eae5, 0x29cdd1adc088a },
        { 0x2f2f9d956e148, 0x6b3e6ad65c1fe, 0x5b00972b79e5d, 0x53d8d234c5daf, 0x104bbd6814049 },
    },
    {
        { 0x59a5fd67ff163, 0x3a998ead0352b, 0x083c95fa4af9a, 0x6fadbfc01266f, 0x204f2a20fb072 },
        { 0x0fd3168f1ed67, 0x1bb0de7784a3e, 0x34bcb78b20477, 0x0a4a26e2e2182, 0x5be8cc57092a7 },
        { 0x43b3d30ebb079, 0x357aca5c61902, 0x5b570c5d62455, 0x30fb29e1e18c7, 0x2570fb17c2791 },
    },
    {
        { 0x6a9550bb8245a, 0x511f20a1a2325, 0x29324d7239bee, 0x3343cc37516c4, 0x241c5f91de018 },
        { 0x2367f2cb61575, 0x6c39ac04d87df, 0x6d4958bd7e5bd, 0x566f4638a1532, 0x3dcb65ea53030 },
        { 0x0172940de6caa, 0x6045b2e67451b, 0x56c07463efcb3, 0x0728b6bfe6e91, 0x08420edd5fcdf },
    },
    {
        { 0x0c34e04f410ce, 0x344edc0d0a06b, 0x6e45486d84d6d, 0x44e2ecb3863f5, 0x04d654f321db8 },
        { 0x720ab8362fa4a, 0x29c4347cdd9bf, 0x0e798ad5f8463, 0x4fef18bcb0bfe, 0x0d9a53efbc176 },
        { 0x5c116ddbdb5d5, 0x6d1b4bba5abcf, 0x4d28a48a5537a, 0x56b8e5b040b99, 0x4a7a4f2618991 },
    },
    {
        { 0x3b291af372a4b, 0x60e3028fe4498, 0x2267bca4f6a09, 0x719eec242b243, 0x4a96314223e0e },
        { 0x718025fb15f95, 0x68d6b8371fe94, 0x3804448f7d97c, 0x42466fe784280, 0x11b50c4cddd31 },
        { 0x0274408a4ffd6, 0x7d382aedb34dd, 0x40acfc9ce385d, 0x628bb99a45b1e, 0x4f4bce4dce6bc },
    },
    {
        { 0x2616ec49d0b6f, 0x1f95d8462e61c, 0x1ad3e9b9159c6, 0x79ba475a04df9, 0x3042cee561595 },
        { 0x7ce5ae2242584, 0x2d25eb153d4e3, 0x3a8f3d09ba9c9, 0x0f3690d04eb8e, 0x73fcdd14b71c0 },
        { 0x67079449bac41, 0x5b79c4621484f, 0x61069f2156b8d, 0x0eb26573b10af, 0x389e740c9a9ce },
    },
    {
        { 0x578f6570eac28, 0x644f2339c3937, 0x66e47b7956c2c, 0x34832fe1f55d0, 0x25c425e5d6263 },
        { 0x4b3ae34dcb9ce, 0x47c691a15ac9f, 0x318e06e5d400c, 0x3c422d9f83eb1, 0x61545379465a6 },
        { 0x606a6f1d7de6e, 0x4f1c0c46107e7, 0x229b1dcfbe5d8, 0x3acc60a7b1327, 0x6539a08915484 },
    },
    {
        { 0x4dbd414bb4a19, 0x7930849f1dbb8, 0x329c5a466caf0, 0x6c824544feb9b, 0x0f65320ef019b },
        { 0x21f74c3d2f773, 0x024b88d08bd3a, 0x6e678cf054151, 0x43631272e747c, 0x11c5e4aac5cd1 },
        { 0x6d1b1cafde0c6, 0x462c76a303a90, 0x3ca4e693cff9b, 0x3952cd45786fd, 0x4cabc7bdec330 },
    },
},
{
    {
        { 0x7788f3f78d289, 0x5942809b3f811, 0x5973277f8c29c, 0x010f93bc5fe67, 0x7ee498165acb2 },
        { 0x69624089c0a2e, 0x0075fc8e70473, 0x13e84ab1d2313, 0x2c10bedf6953b, 0x639b93f0321c8 },
        { 0x508e39111a1c3, 0x290120e912f7a, 0x1cbf464acae43, 0x15373e9576157, 0x0edf493c85b60 },
    },
    {
        { 0x7c4d284764113, 0x7fefebf06acec, 0x39afb7a824100, 0x1b48e47e7fd65, 0x04c00c54d1dfa },
        { 0x48158599b5a68, 0x1fd75bc41d5d9, 0x2d9fc1fa95d3c, 0x7da27f20eba11, 0x403b92e3019d4 },
        { 0x22f818b465cf8, 0x342901dff09b8, 0x31f595dc683cd, 0x37a57745fd682, 0x355bb12ab2617 },
    },
    {
        { 0x1dac75a8c7318, 0x3b679d5423460, 0x6b8fcb7b6400e, 0x6c73783be5f9d, 0x7518eaf8e052a },
        { 0x664cc7493bbf4, 0x33d94761874e3, 0x0179e1796f613, 0x1890535e2867d, 0x0f9b8132182ec },
        { 0x059c41b7f6c32, 0x79e8706531491, 0x6c747643cb582, 0x2e20c0ad494e4, 0x47c3871bbb175 },
    },
    {
        { 0x65d50c85066b0, 0x6167453361f7c, 0x06ba3818bb312, 0x6aff29baa7522, 0x08fea02ce8d48 },
        { 0x4539771ec4f48, 0x7b9318badca28, 0x70f19afe016c5, 0x4ee7bb1608d23, 0x00b89b8576469 },
        { 0x5dd7668deead0, 0x4096d0ba47049, 0x6275997219114, 0x29bda8a67e6ae, 0x473829a74f75d },
    },
    {
        { 0x1533aad3902c9, 0x1dde06b11e47b, 0x784bed1930b77, 0x1c80a92b9c867, 0x6c668b4d44e4d },
        { 0x2da754679c418, 0x3164c31be105a, 0x11fac2b98ef5f, 0x35a1aaf779256, 0x2078684c4833c },
        { 0x0cf217a78820c, 0x65024e7d2e769, 0x23bb5efdda82a, 0x19fd4b632d3c6, 0x7411a6054f8a4 },
    },
    {
        { 0x2e53d18b175b4, 0x33e7254204af3, 0x3bcd7d5a1c4c5, 0x4c7c22af65d0f, 0x1ec9a872458c3 },
        { 0x59d32b99dc86d, 0x6ac075e22a9ac, 0x30b9220113371, 0x27fd9a638966e, 0x7c136574fb813 },
        { 0x6a4d400a2509b, 0x041791056971c, 0x655d5866e075c, 0x2302bf3e64df8, 0x3add88a5c7cd6 },
    },
    {
        { 0x298d459393046, 0x30bfecb3d90b8, 0x3d9b8ea3df8d6, 0x3900e96511579, 0x61ba1131a406a },
        { 0x15770b635dcf2, 0x59ecd83f79571, 0x2db461c0b7fbd, 0x73a42a981345f, 0x249929fccc879 },
        { 0x0a0f116959029, 0x5974fd7b1347a, 0x1e0cc1c08edad, 0x673bdf8ad1f13, 0x5620310cbbd8e },
    },
    {
        { 0x6b5f477e285d6, 0x4ed91ec326cc8, 0x6d6537503a3fd, 0x626d3763988d5, 0x7ec846f3658ce },
        { 0x193434934d643, 0x0d4a2445eaa51, 0x7d0708ae76fe0, 0x39847b6c3c7e1, 0x37676a2a4d9d9 },
        { 0x68f3f1da22ec7, 0x6ed8039a2736b, 0x2627ee04c3c75, 0x6ea90a647e7d1, 0x6daaf723399b9 },
    },
},
{
    {
        { 0x304bfacad8ea2, 0x502917d108b07, 0x043176ca6dd0f, 0x5d5158f2c1d84, 0x2b5449e58eb3b },
        { 0x27562eb3dbe47, 0x291d7b4170be7, 0x5d1ca67dfa8e1, 0x2a88061f298a2, 0x1304e9e71627d },
        { 0x014d26adc9cfe, 0x7f1691ba16f13, 0x5e71828f06eac, 0x349ed07f0fffc, 0x4468de2d7c2dd },
    },
    {
        { 0x2d8c6f86307ce, 0x6286ba1850973, 0x5e9dcb08444d4, 0x1a96a543362b2, 0x5da6427e63247 },
        { 0x3355e9419469e, 0x1847bb8ea8a37, 0x1fe6588cf9b71, 0x6b1c9d2db6b22, 0x6cce7c6ffb44b },
        { 0x4c688deac22ca, 0x6f775c3ff0352, 0x565603ee419bb, 0x6544456c61c46, 0x58f29abfe79f2 },
    },
    {
        { 0x264bf710ecdf6, 0x708c58527896b, 0x42ceae6c53394, 0x4381b21e82b6a, 0x6af93724185b4 },
        { 0x6cfab8de73e68, 0x3e6efced4bd21, 0x0056609500dbe, 0x71b7824ad85df, 0x577629c4a7f41 },
        { 0x0024509c6a888, 0x2696ab12e6644, 0x0cca27f4b80d8, 0x0c7c1f11b119e, 0x701f25bb0caec },
    },
    {
        { 0x0f6d97cbec113, 0x4ce97fb7c93a3, 0x139835a11281b, 0x728907ada9156, 0x720a5bc050955 },
        { 0x0b0f8e4616ced, 0x1d3c4b50fb875, 0x2f29673dc0198, 0x5f4b0f1830ffa, 0x2e0c92bfbdc40 },
        { 0x709439b805a35, 0x6ec48557f8187, 0x08a4d1ba13a2c, 0x076348a0bf9ae, 0x0e9b9cbb144ef },
    },
    {
        { 0x69bd55db1beee, 0x6e14e47f731bd, 0x1a35e47270eac, 0x66f225478df8e, 0x366d44191cfd3 },
        { 0x2d48ffb5720ad, 0x57b7f21a1df77, 0x5550effba0645, 0x5ec6a4098a931, 0x221104eb3f337 },
        { 0x41743f2bc8c14, 0x796b0ad8773c7, 0x29fee5cbb689b, 0x122665c178734, 0x4167a4e6bc593 },
    },
    {
        { 0x62665f8ce8fee, 0x29d101ac59857, 0x4d93bbba59ffc, 0x17b7897373f17, 0x34b33370cb7ed },
        { 0x39d2876f62700, 0x001cecd1d6c87, 0x7f01a11747675, 0x2350da5a18190, 0x7938bb7e22552 },
        { 0x591ee8681d6cc, 0x39db0b4ea79b8, 0x202220f380842, 0x2f276ba42e0ac, 0x1176fc6e2dfe6 },
    },
    {
        { 0x0e28949770eb8, 0x5559e88147b72, 0x35e1e6e63ef30, 0x35b109aa7ff6f, 0x1f6a3e54f2690 },
        { 0x76cd05b9c619b, 0x69654b0901695, 0x7a53710b77f27, 0x79a1ea7d28175, 0x08fc3a4c677d5 },
        { 0x4c199d30734ea, 0x6c622cb9acc14, 0x5660a55030216, 0x068f1199f11fb, 0x4f2fad0116b90 },
    },
    {
        { 0x4d91db73bb638, 0x55f82538112c5, 0x6d85a279815de, 0x740b7b0cd9cf9, 0x3451995f2944e },
        { 0x6b24194ae4e54, 0x2230afded8897, 0x23412617d5071, 0x3d5d30f35969b, 0x445484a4972ef },
        { 0x2fcd09fea7d7c, 0x296126b9ed22a, 0x4a171012a05b2, 0x1db92c74d5523, 0x10b89ca604289 },
    },
},
{
    {
        { 0x141be5a45f06e, 0x5adb38becaea7, 0x3fd46db41f2bb, 0x6d488bbb5ce39, 0x17d2d1d9ef0d4 },
        { 0x147499718289c, 0x0a48a67e4c7ab, 0x30fbc544bafe3, 0x0c701315fe58a, 0x20b878d577b75 },
        { 0x2af18073f3e6a, 0x33aea420d24fe, 0x298008bf4ff94, 0x3539171db961e, 0x72214f63cc65c },
    },
    {
        { 0x5b7b9f43b29c9, 0x149ea31eea3b3, 0x4be7713581609, 0x2d87960395e98, 0x1f24ac855a154 },
        { 0x37f405307a693, 0x2e5e66cf2b69c, 0x5d84266ae9c53, 0x5e4eb7de853b9, 0x5fdf48c58171c },
        { 0x608328e9505aa, 0x22182841dc49a, 0x3ec96891d2307, 0x2f363fff22e03, 0x00ba739e2ae39 },
    },
    {
        { 0x426f5ea88bb26, 0x33092e77f75c8, 0x1a53940d819e7, 0x1132e4f818613, 0x72297de7d518d },
        { 0x698de5c8790d6, 0x268b8545beb25, 0x6d2648b96fedf, 0x47988ad1db07c, 0x03283a3e67ad7 },
        { 0x41dc7be0cb939, 0x1b16c66100904, 0x0a24c20cbc66d, 0x4a2e9efe48681, 0x05e1296846271 },
    },
    {
        { 0x7bbc8242c4550, 0x59a06103b35b7, 0x7237e4af32033, 0x726421ab3537a, 0x78cf25d38258c },
        { 0x2eeb32d9c495a, 0x79e25772f9750, 0x6d747833bbf23, 0x6cdd816d5d749, 0x39c00c9c13698 },
        { 0x66b8e31489d68, 0x573857e10e2b5, 0x13be816aa1472, 0x41964d3ad4bf8, 0x006b52076b3ff },
    },
    {
        { 0x37e16b9ce082d, 0x1882f57853eb9, 0x7d29eacd01fc5, 0x2e76a59b5e715, 0x7de2e9561a9f7 },
        { 0x0cfe19d95781c, 0x312cc621c453c, 0x145ace6da077c, 0x0912bef9ce9b8, 0x4d57e3443bc76 },
        { 0x0d4f4b6a55ecb, 0x7ebb0bb733bce, 0x7ba6a05200549, 0x4f6ede4e22069, 0x6b2a90af1a602 },
    },
    {
        { 0x3f3245bb2d80a, 0x0e5f720f36efd, 0x3b9cccf60c06d, 0x084e323f37926, 0x465812c8276c2 },
        { 0x3f4fc9ae61e97, 0x3bc07ebfa2d24, 0x3b744b55cd4a0, 0x72553b25721f3, 0x5fd8f4e9d12d3 },
        { 0x3beb22a1062d9, 0x6a7063b82c9a8, 0x0a5a35dc197ed, 0x3c80c06a53def, 0x05b32c2b1cb16 },
    },
    {
        { 0x4a42c7ad58195, 0x5c8667e799eff, 0x02e5e74c850a1, 0x3f0db614e869a, 0x31771a4856730 },
        { 0x05eccd24da8fd, 0x580bbfdf07918, 0x7e73586873c6a, 0x74ceddf77f93e, 0x3b5556a37b471 },
        { 0x0c524e14dd482, 0x283457496c656, 0x0ad6bcfb6cd45, 0x375d1e8b02414, 0x4fc079d27a733 },
    },
    {
        { 0x48b440c86c50d, 0x139929cca3b86, 0x0f8f2e44cdf2f, 0x68432117ba6b2, 0x241170c2bae3c },
        { 0x138b089bf2f7f, 0x4a05bfd34ea39, 0x203914c925ef5, 0x7497fffe04e3c, 0x124567cecaf98 },
        { 0x1ab860ac473b4, 0x5c0227c86a7ff, 0x71b12bfc24477, 0x006a573a83075, 0x3f8612966c870 },
    },
},
{
    {
        { 0x0fcfa36048d13, 0x66e7133bbb383, 0x64b42a8a45676, 0x4ea6e4f9a85cf, 0x26f57eee878a1 },
        { 0x20cc9782a0dde, 0x65d4e3070aab3, 0x7bc8e31547736, 0x09ebfb1432d98, 0x504aa77679736 },
        { 0x32cd55687efb1, 0x4448f5e2f6195, 0x568919d460345, 0x034c2e0ad1a27, 0x4041943d9dba3 },
    },
    {
        { 0x17743a26caadd, 0x48c9156f9c964, 0x7ef278d1e9ad0, 0x00ce58ea7bd01, 0x12d931429800d },
        { 0x0eeba43ebcc96, 0x384dd5395f878, 0x1df331a35d272, 0x207ecfd4af70e, 0x1420a1d976843 },
        { 0x67799d337594f, 0x01647548f6018, 0x57fce5578f145, 0x009220c142a71, 0x1b4f92314359a },
    },
    {
        { 0x73030a49866b1, 0x2442be90b2679, 0x77bd3d8947dcf, 0x1fb55c1552028, 0x5ff191d56f9a2 },
        { 0x4109d89150951, 0x225bd2d2d47cb, 0x57cc080e73bea, 0x6d71075721fcb, 0x239b572a7f132 },
        { 0x6d433ac2d9068, 0x72bf930a47033, 0x64facf4a20ead, 0x365f7a2b9402a, 0x020c526a758f3 },
    },
    {
        { 0x1ef59f042cc89, 0x3b1c24976dd26, 0x31d665cb16272, 0x28656e470c557, 0x452cfe0a5602c },
        { 0x034f89ed8dbbc, 0x73b8f948d8ef3, 0x786c1d323caab, 0x43bd4a9266e51, 0x02aacc4615313 },
        { 0x0f7a0647877df, 0x4e1cc0f93f0d4, 0x7ec4726ef1190, 0x3bdd58bf512f8, 0x4cfb7d7b304b8 },
    },
    {
        { 0x699c29789ef12, 0x63beae321bc50, 0x325c340adbb35, 0x562e1a1e42bf6, 0x5b1d4cbc434d3 },
        { 0x43d6cb89b75fe, 0x3338d5b900e56, 0x38d327d531a53, 0x1b25c61d51b9f, 0x14b4622b39075 },
        { 0x32615cc0a9f26, 0x57711b99cb6df, 0x5a69c14e93c38, 0x6e88980a4c599, 0x2f98f71258592 },
    },
    {
        { 0x2ae444f54a701, 0x615397afbc5c2, 0x60d7783f3f8fb, 0x2aa675fc486ba, 0x1d8062e9e7614 },
        { 0x4a74cb50f9e56, 0x531d1c2640192, 0x0c03d9d6c7fd2, 0x57ccd156610c1, 0x3a6ae249d806a },
        { 0x2da85a9907c5a, 0x6b23721ec4caf, 0x4d2d3a4683aa2, 0x7f9c6870efdef, 0x298b8ce8aef25 },
    },
    {
        { 0x272ea0a2165de, 0x68179ef3ed06f, 0x4e2b9c0feac1e, 0x3ee290b1b63bb, 0x6ba6271803a7d },
        { 0x27953eff70cb2, 0x54f22ae0ec552, 0x29f3da92e2724, 0x242ca0c22bd18, 0x34b8a8404d5ce },
        { 0x6ecb583693335, 0x3ec76bfdfb84d, 0x2c895cf56a04f, 0x6355149d54d52, 0x71d62bdd465e1 },
    },
    {
        { 0x5b5dab1f75ef5, 0x1e2d60cbeb9a5, 0x527c2175dfe57, 0x59e8a2b8ff51f, 0x1c333621262b2 },
        { 0x3cc28d378df80, 0x72141f4968ca6, 0x407696bdb6d0d, 0x5d271b22ffcfb, 0x74d5f317f3172 },
        { 0x7e55467d9ca81, 0x6a5653186f50d, 0x6b188ece62df1, 0x4c66d36844971, 0x4aebcc4547e9d },
    },
},
{
    {
        { 0x08d9e7354b610, 0x26b750b6dc168, 0x162881e01acc9, 0x7966df31d01a5, 0x173bd9ddc9a1d },
        { 0x0071b276d01c9, 0x0b0d8918e025e, 0x75beea79ee2eb, 0x3c92984094db8, 0x5d88fbf95a3db },
        { 0x00f1efe5872df, 0x5da872318256a, 0x59ceb81635960, 0x18cf37693c764, 0x06e1cd13b19ea },
    },
    {
        { 0x3af629e5b0353, 0x204f1a088e8e5, 0x10efc9ceea82e, 0x589863c2fa34b, 0x7f3a6a1a8d837 },
        { 0x0ad516f166f23, 0x263f56d57c81a, 0x13422384638ca, 0x1331ff1af0a50, 0x3080603526e16 },
        { 0x644395d3d800b, 0x2b9203dbedefc, 0x4b18ce656a355, 0x03f3466bc182c, 0x30d0fded2e513 },
    },
    {
        { 0x4971e68b84750, 0x52ccc9779f396, 0x3e904ae8255c8, 0x4ecae46f39339, 0x4615084351c58 },
        { 0x14d1af21233b3, 0x1de1989b39c0b, 0x52669dc6f6f9e, 0x43434b28c3fc7, 0x0a9214202c099 },
        { 0x019c0aeb9a02e, 0x1a2c06995d792, 0x664cbb1571c44, 0x6ff0736fa80b2, 0x3bca0d2895ca5 },
    },
    {
        { 0x08eb69ecc01bf, 0x5b4c8912df38d, 0x5ea7f8bc2f20e, 0x120e516caafaf, 0x4ea8b4038df28 },
        { 0x031bc3c5d62a4, 0x7d9fe0f4c081e, 0x43ed51467f22c, 0x1e6cc0c1ed109, 0x5631deddae8f1 },
        { 0x5460af1cad202, 0x0b4919dd0655d, 0x7c4697d18c14c, 0x231c890bba2a4, 0x24ce0930542ca },
    },
    {
        { 0x7a155fdf30b85, 0x1c6c6e5d487f9, 0x24be1134bdc5a, 0x1405970326f32, 0x549928a7324f4 },
        { 0x090f5fd06c106, 0x6abb1021e43fd, 0x232bcfad711a0, 0x3a5c13c047f37, 0x41d4e3c28a06d },
        { 0x632a763ee1a2e, 0x6fa4bffbd5e4d, 0x5fd35a6ba4792, 0x7b55e1de99de8, 0x491b66dec0dcf },
    },
    {
        { 0x04a8ed0da64a1, 0x5ecfc45096ebe, 0x5edee93b488b2, 0x5b3c11a51bc8f, 0x4cf6b8b0b7018 },
        { 0x5b13dc7ea32a7, 0x18fc2db73131e, 0x7e3651f8f57e3, 0x25656055fa965, 0x08f338d0c85ee },
        { 0x3a821991a73bd, 0x03be6418f5870, 0x1ddc18eac9ef0, 0x54ce09e998dc2, 0x530d4a82eb078 },
    },
    {
        { 0x173456c9abf9e, 0x7892015100dad, 0x33ee14095fecb, 0x6ad95d67a0964, 0x0db3e7e00cbfb },
        { 0x43630e1f94825, 0x4d1956a6b4009, 0x213fe2df8b5e0, 0x05ce3a41191e6, 0x65ea753f10177 },
        { 0x6fc3ee2096363, 0x7ec36b96d67ac, 0x510ec6a0758b1, 0x0ed87df022109, 0x02a4ec1921e1a },
    },
    {
        { 0x06162f1cf795f, 0x324ddcafe5eb9, 0x018d5e0463218, 0x7e78b9092428e, 0x36d12b5dec067 },
        { 0x6259a3b24b8a2, 0x188b5f4170b9c, 0x681c0dee15deb, 0x4dfe665f37445, 0x3d143c5112780 },
        { 0x5279179154557, 0x39f8f0741424d, 0x45e6eb357923d, 0x42c9b5edb746f, 0x2ef517885ba82 },
    },
},
{
    {
        { 0x6bffb305b2f51, 0x5b112b2d712dd, 0x35774974fe4e2, 0x04af87a96e3a3, 0x57968290bb3a0 },
        { 0x7974e8c58aedc, 0x7757e083488c6, 0x601c62ae7bc8b, 0x45370c2ecab74, 0x2f1b78fab143a },
        { 0x2b8430a20e101, 0x1a49e1d88fee3, 0x38bbb47ce4d96, 0x1f0e7ba84d437, 0x7dc43e35dc2aa },
    },
    {
        { 0x02a5c273e9718, 0x32bc9dfb28b4f, 0x48df4f8d5db1a, 0x54c87976c028f, 0x044fb81d82d50 },
        { 0x66665887dd9c3, 0x629760a6ab0b2, 0x481e6c7243e6c, 0x097e37046fc77, 0x7ef72016758cc },
        { 0x718c5a907e3d9, 0x3b9c98c6b383b, 0x006ed255eccdc, 0x6976538229a59, 0x7f79823f9c30d },
    },
    {
        { 0x41ff068f587ba, 0x1c00a191bcd53, 0x7b56f9c209e25, 0x3781e5fccaabe, 0x64a9b0431c06d },
        { 0x4d239a3b513e8, 0x29723f51b1066, 0x642f4cf04d9c3, 0x4da095aa09b7a, 0x0a4e0373d784d },
        { 0x3d6a15b7d2919, 0x41aa75046a5d6, 0x691751ec2d3da, 0x23638ab6721c4, 0x071a7d0ace183 },
    },
    {
        { 0x4355220e14431, 0x0e1362a283981, 0x2757cd8359654, 0x2e9cd7ab10d90, 0x7c69bcf761775 },
        { 0x72daac887ba0b, 0x0b7f4ac5dda60, 0x3bdda2c0498a4, 0x74e67aa180160, 0x2c3bcc7146ea7 },
        { 0x0d7eb04e8295f, 0x4a5ea1e6fa0fe, 0x45e635c436c60, 0x28ef4a8d4d18b, 0x6f5a9a7322aca },
    },
    {
        { 0x1d4eba3d944be, 0x0100f15f3dce5, 0x61a700e367825, 0x5922292ab3d23, 0x02ab9680ee8d3 },
        { 0x1000c2f41c6c5, 0x0219fdf737174, 0x314727f127de7, 0x7e5277d23b81e, 0x494e21a2e147a },
        { 0x48a85dde50d9a, 0x1c1f734493df4, 0x47bdb64866889, 0x59a7d048f8eec, 0x6b5d76cbea46b },
    },
    {
        { 0x141171e782522, 0x6806d26da7c1f, 0x3f31d1bc79ab9, 0x09f20459f5168, 0x16fb869c03dd3 },
        { 0x7556cec0cd994, 0x5eb9a03b7510a, 0x50ad1dd91cb71, 0x1aa5780b48a47, 0x0ae333f685277 },
        { 0x6199733b60962, 0x69b157c266511, 0x64740f893f1ca, 0x03aa408fbf684, 0x3f81e38b8f70d },
    },
    {
        { 0x37f355f17c824, 0x07ae85334815b, 0x7e3abddd2e48f, 0x61eeabe1f45e5, 0x0ad3e2d34cded },
        { 0x10fcc7ed9affe, 0x4248cb0e96ff2, 0x4311c115172e2, 0x4c9d41cbf6925, 0x50510fc104f50 },
        { 0x40fc5336e249d, 0x3386639fb2de1, 0x7bbf871d17b78, 0x75f796b7e8004, 0x127c158bf0fa1 },
    },
    {
        { 0x28fc4ae51b974, 0x26e89bfd2dbd4, 0x4e122a07665cf, 0x7cab1203405c3, 0x4ed82479d167d },
        { 0x17c422e9879a2, 0x28a5946c8fec3, 0x53ab32e912b77, 0x7b44da09fe0a5, 0x354ef87d07ef4 },
        { 0x3b52260c5d975, 0x79d6836171fdc, 0x7d994f140d4bb, 0x1b6c404561854, 0x302d92d205392 },
    },
},
{
    {
        { 0x46fb6e4e0f177, 0x53497ad5265b7, 0x1ebdba01386fc, 0x0302f0cb36a3c, 0x0edc5f5eb426d },
        { 0x3c1a2bca4283d, 0x23430c7bb2f02, 0x1a3ea1bb58bc2, 0x7265763de5c61, 0x10e5d3b76f1ca },
        { 0x3bfd653da8e67, 0x584953ec82a8a, 0x55e288fa7707b, 0x5395fc3931d81, 0x45b46c51361cb },
    },
    {
        { 0x54ddd8a7fe3e4, 0x2cecc41c619d3, 0x43a6562ac4d91, 0x4efa5aca7bdd9, 0x5c1c0aef32122 },
        { 0x02abf314f7fa1, 0x391d19e8a1528, 0x6a2fa13895fc7, 0x09d8eddeaa591, 0x2177bfa36dcb7 },
        { 0x01bbcfa79db8f, 0x3d84beb3666e1, 0x20c921d812204, 0x2dd843d3b32ce, 0x4ae619387d8ab },
    },
    {
        { 0x17e44985bfb83, 0x54e32c626cc22, 0x096412ff38118, 0x6b241d61a246a, 0x75685abe5ba43 },
        { 0x3f6aa5344a32e, 0x69683680f11bb, 0x04c3581f623aa, 0x701af5875cba5, 0x1a00d91b17bf3 },
        { 0x60933eb61f2b2, 0x5193fe92a4dd2, 0x3d995a550f43e, 0x3556fb93a883d, 0x135529b623b0e },
    },
    {
        { 0x716bce22e83fe, 0x33d0130b83eb8, 0x0952abad0afac, 0x309f64ed31b8a, 0x5972ea051590a },
        { 0x0dbd7add1d518, 0x119f823e2231e, 0x451d66e5e7de2, 0x500c39970f838, 0x79b5b81a65ca3 },
        { 0x4ac20dc8f7811, 0x29589a9f501fa, 0x4d810d26a6b4a, 0x5ede00d96b259, 0x4f7e9c95905f3 },
    },
    {
        { 0x0443d355299fe, 0x39b7d7d5aee39, 0x692519a2f34ec, 0x6e4404924cf78, 0x1942eec4a144a },
        { 0x74bbc5781302e, 0x73135bb81ec4c, 0x7ef671b61483c, 0x7264614ccd729, 0x31993ad92e638 },
        { 0x45319ae234992, 0x2219d47d24fb5, 0x4f04488b06cf6, 0x53aaa9e724a12, 0x2a0a65314ef9c },
    },
    {
        { 0x61acd3c1c793a, 0x58b46b78779e6, 0x3369aacbe7af2, 0x509b0743074d4, 0x055dc39b6dea1 },
        { 0x7937ff7f927c2, 0x0c2fa14c6a5b6, 0x556bddb6dd07c, 0x6f6acc179d108, 0x4cf6e218647c2 },
        { 0x1227cc28d5bb6, 0x78ee9bff57623, 0x28cb2241f893a, 0x25b541e3c6772, 0x121a307710aa2 },
    },
    {
        { 0x1713ec77483c9, 0x6f70572d5facb, 0x25ef34e22ff81, 0x54d944f141188, 0x527bb94a6ced3 },
        { 0x35d5e9f034a97, 0x126069785bc9b, 0x5474ec7854ff0, 0x296a302a348ca, 0x333fc76c7a40e },
        { 0x5992a995b482e, 0x78dc707002ac7, 0x5936394d01741, 0x4fba4281aef17, 0x6b89069b20a7a },
    },
    {
        { 0x2fa8cb5c7db77, 0x718e6982aa810, 0x39e95f81a1a1b, 0x5e794f3646cfb, 0x0473d308a7639 },
        { 0x2a0416270220d, 0x75f248b69d025, 0x1cbbc16656a27, 0x5b9ffd6e26728, 0x23bc2103aa73e },
        { 0x6792603589e05, 0x248db9892595d, 0x006a53cad2d08, 0x20d0150f7ba73, 0x102f73bfde043 },
    },
},
{
    {
        { 0x4dae0b5511c9a, 0x5257fffe0d456, 0x54108d1eb2180, 0x096cc0f9baefa, 0x3f6bd725da4ea },
        { 0x0b9ab7f5745c6, 0x5caf0f8d21d63, 0x7debea408ea2b, 0x09edb93896d16, 0x36597d25ea5c0 },
        { 0x58d7b106058ac, 0x3cdf8d20bee69, 0x00a4cb765015e, 0x36832337c7cc9, 0x7b7ecc19da60d },
    },
    {
        { 0x64a51a77cfa9b, 0x29cf470ca0db5, 0x4b60b6e0898d9, 0x55d04ddffe6c7, 0x03bedc661bf5c },
        { 0x2373c695c690d, 0x4c0c8520dcf18, 0x384af4b7494b9, 0x4ab4a8ea22225, 0x4235ad7601743 },
        { 0x0cb0d078975f5, 0x292313e530c4b, 0x38dbb9124a509, 0x350d0655a11f1, 0x0e7ce2b0cdf06 },
    },
    {
        { 0x6fedfd94b70f9, 0x2383f9745bfd4, 0x4beae27c4c301, 0x75aa4416a3f3f, 0x615256138aece },
        { 0x4643ac48c85a3, 0x6878c2735b892, 0x3a53523f4d877, 0x3a504ed8bee9d, 0x666e0a5d8fb46 },
        { 0x3f64e4870cb0d, 0x61548b16d6557, 0x7a261773596f3, 0x7724d5f275d3a, 0x7f0bc810d514d },
    },
    {
        { 0x49dad737213a0, 0x745dee5d31075, 0x7b1a55e7fdbe2, 0x5ba988f176ea1, 0x1d3a907ddec5a },
        { 0x06ba426f4136f, 0x3cafc0606b720, 0x518f0a2359cda, 0x5fae5e46feca7, 0x0d1f8dbcf8eed },
        { 0x693313ed081dc, 0x5b0a366901742, 0x40c872ca4ca7e, 0x6f18094009e01, 0x00011b44a31bf },
    },
    {
        { 0x61f696a0aa75c, 0x38b0a57ad42ca, 0x1e59ab706fdc9, 0x01308d46ebfcd, 0x63d988a2d2851 },
        { 0x7a06c3fc66c0c, 0x1c9bac1ba47fb, 0x23935c575038e, 0x3f0bd71c59c13, 0x3ac48d916e835 },
        { 0x20753afbd232e, 0x71fbb1ed06002, 0x39cae47a4af3a, 0x0337c0b34d9c2, 0x33fad52b2368a },
    },
    {
        { 0x4c8d0c422cfe8, 0x760b4275971a5, 0x3da95bc1cad3d, 0x0f151ff5b7376, 0x3cc355ccb90a7 },
        { 0x649c6c5e41e16, 0x60667eee6aa80, 0x4179d182be190, 0x653d9567e6979, 0x16c0f429a256d },
        { 0x69443903e9131, 0x16f4ac6f9dd36, 0x2ea4912e29253, 0x2b4643e68d25d, 0x631eaf426bae7 },
    },
    {
        { 0x175b9a3700de8, 0x77c5f00aa48fb, 0x3917785ca0317, 0x05aa9b2c79399, 0x431f2c7f665f8 },
        { 0x10410da66fe9f, 0x24d82dcb4d67d, 0x3e6fe0e17752d, 0x4dade1ecbb08f, 0x5599648b1ea91 },
        { 0x26344858f7b19, 0x5f43d4a295ac0, 0x242a75c52acd4, 0x5934480220d10, 0x7b04715f91253 },
    },
    {
        { 0x6c280c4e6bac6, 0x3ada3b361766e, 0x42fe5125c3b4f, 0x111d84d4aac22, 0x48d0acfa57cde },
        { 0x5bd28acf6ae43, 0x16fab8f56907d, 0x7acb11218d5f2, 0x41fe02023b4db, 0x59b37bf5c2f65 },
        { 0x726e47dabe671, 0x2ec45e746f6c1, 0x6580e53c74686, 0x5eda104673f74, 0x16234191336d3 },
    },
},
{
    {
        { 0x19cd61ff38640, 0x060c6c4b41ba9, 0x75cf70ca7366f, 0x118a8f16c011e, 0x4a25707a203b9 },
        { 0x499def6267ff6, 0x76e858108773c, 0x693cac5ddcb29, 0x00311d00a9ff4, 0x2cdfdfecd5d05 },
        { 0x7668a53f6ed6a, 0x303ba2e142556, 0x3880584c10909, 0x4fe20000a261d, 0x5721896d248e4 },
    },
    {
        { 0x55091a1d0da4e, 0x4f6bfc7c1050b, 0x64e4ecd2ea9be, 0x07eb1f28bbe70, 0x03c935afc4b03 },
        { 0x65517fd181bae, 0x3e5772c76816d, 0x019189640898a, 0x1ed2a84de7499, 0x578edd74f63c1 },
        { 0x276c6492b0c3d, 0x09bfc40bf932e, 0x588e8f11f330b, 0x3d16e694dc26e, 0x3ec2ab590288c },
    },
    {
        { 0x13a09ae32d1cb, 0x3e81eb85ab4e4, 0x07aaca43cae1f, 0x62f05d7526374, 0x0e1bf66c6adba },
        { 0x0d27be4d87bb9, 0x56c27235db434, 0x72e6e0ea62d37, 0x5674cd06ee839, 0x2dd5c25a200fc },
        { 0x3d5e9792c887e, 0x319724dabbc55, 0x2b97c78680800, 0x7afdfdd34e6dd, 0x730548b35ae88 },
    },
    {
        { 0x3094ba1d6e334, 0x6e126a7e3300b, 0x089c0aefcfbc5, 0x2eea11f836583, 0x585a2277d8784 },
        { 0x551a3cba8b8ee, 0x3b6422be2d886, 0x630e1419689bc, 0x4653b07a7a955, 0x3043443b411db },
        { 0x25f8233d48962, 0x6bd8f04aff431, 0x4f907fd9a6312, 0x40fd3c737d29b, 0x7656278950ef9 },
    },
    {
        { 0x073a3ea86cf9d, 0x6e0e2abfb9c2e, 0x60e2a38ea33ee, 0x30b2429f3fe18, 0x28bbf484b613f },
        { 0x3cf59d51fc8c0, 0x7a0a0d6de4718, 0x55c3a3e6fb74b, 0x353135f884fd5, 0x3f4160a8c1b84 },
        { 0x12f5c6f136c7c, 0x0fedba237de4c, 0x779bccebfab44, 0x3aea93f4d6909, 0x1e79cb358188f },
    },
    {
        { 0x153d8f5e08181, 0x08533bbdb2efd, 0x1149796129431, 0x17a6e36168643, 0x478ab52d39d1f },
        { 0x436c3eef7e3f1, 0x7ffd3c21f0026, 0x3e77bf20a2da9, 0x418bffc8472de, 0x65d7951b3a3b3 },
        { 0x6a4d39252d159, 0x790e35900ecd4, 0x30725bf977786, 0x10a5c1635a053, 0x16d87a411a212 },
    },
    {
        { 0x4d5e2d54e0583, 0x2e5d7b33f5f74, 0x3a5de3f887ebf, 0x6ef24bd6139b7, 0x1f990b577a5a6 },
        { 0x57e5a42066215, 0x1a18b44983677, 0x3e652de1e6f8f, 0x6532be02ed8eb, 0x28f87c8165f38 },
        { 0x44ead1be8f7d6, 0x5759d4f31f466, 0x0378149f47943, 0x69f3be32b4f29, 0x45882fe1534d6 },
    },
    {
        { 0x49929943c6fe4, 0x4347072545b15, 0x3226bced7e7c5, 0x03a134ced89df, 0x7dcf843ce405f },
        { 0x1345d757983d6, 0x222f54234cccd, 0x1784a3d8adbb4, 0x36ebeee8c2bcc, 0x688fe5b8f626f },
        { 0x0d6484a4732c0, 0x7b94ac6532d92, 0x5771b8754850f, 0x48dd9df1461c8, 0x6739687e73271 },
    },
},
{
    {
        { 0x5cc9dc80c1ac0, 0x683671486d4cd, 0x76f5f1a5e8173, 0x6d5d3f5f9df4a, 0x7da0b8f68d7e7 },
        { 0x02014385675a6, 0x6155fb53d1def, 0x37ea32e89927c, 0x059a668f5a82e, 0x46115aba1d4dc },
        { 0x71953c3b5da76, 0x6642233d37a81, 0x2c9658076b1bd, 0x5a581e63010ff, 0x5a5f887e83674 },
    },
    {
        { 0x628d3a0a643b9, 0x01cd8640c93d2, 0x0b7b0cad70f2c, 0x3864da98144be, 0x43e37ae2d5d1c },
        { 0x301cf70a13d11, 0x2a6a1ba1891ec, 0x2f291fb3f3ae0, 0x21a7b814bea52, 0x3669b656e44d1 },
        { 0x63f06eda6e133, 0x233342758070f, 0x098e0459cc075, 0x4df5ead6c7c1b, 0x6a21e6cd4fd5e },
    },
    {
        { 0x129126699b2e3, 0x0ee11a2603de8, 0x60ac2f5c74c21, 0x59b192a196808, 0x45371b07001e8 },
        { 0x6170a3046e65f, 0x5401a46a49e38, 0x20add5561c4a8, 0x7abb4edde9e46, 0x586bf9f1a195f },
        { 0x3088d5ef8790b, 0x38c2126fcb4db, 0x685bae149e3c3, 0x0bcd601a4e930, 0x0eafb03790e52 },
    },
    {
        { 0x0805e0f75ae1d, 0x464cc59860a28, 0x248e5b7b00bef, 0x5d99675ef8f75, 0x44ae3344c5435 },
        { 0x555c13748042f, 0x4d041754232c0, 0x521b430866907, 0x3308e40fb9c39, 0x309acc675a02c },
        { 0x289b9bba543ee, 0x3ab592e28539e, 0x64d82abcdd83a, 0x3c78ec172e327, 0x62d5221b7f946 },
    },
    {
        { 0x5d4263af77a3c, 0x23fdd2289aeb0, 0x7dc64f77eb9ec, 0x01bd28338402c, 0x14f29a5383922 },
        { 0x4299c18d0936d, 0x5914183418a49, 0x52a18c721aed5, 0x2b151ba82976d, 0x5c0efde4bc754 },
        { 0x17edc25b2d7f5, 0x37336a6081bee, 0x7b5318887e5c3, 0x49f6d491a5be1, 0x5e72365c7bee0 },
    },
    {
        { 0x339062f08b33e, 0x4bbf3e657cfb2, 0x67af7f56e5967, 0x4dbd67f9ed68f, 0x70b20555cb734 },
        { 0x3fc074571217f, 0x3a0d29b2b6aeb, 0x06478ccdde59d, 0x55e4d051bddfa, 0x77f1104c47b4e },
        { 0x113c555112c4c, 0x7535103f9b7ca, 0x140ed1d9a2108, 0x02522333bc2af, 0x0e34398f4a064 },
    },
    {
        { 0x30b093e4b1928, 0x1ce7e7ec80312, 0x4e575bdf78f84, 0x61f7a190bed39, 0x6f8aded6ca379 },
        { 0x522d93ecebde8, 0x024f045e0f6cf, 0x16db63426cfa1, 0x1b93a1fd30fd8, 0x5e5405368a362 },
        { 0x0123dfdb7b29a, 0x4344356523c68, 0x79a527921ee5f, 0x74bfccb3e817e, 0x780de72ec8d3d },
    },
    {
        { 0x7eaf300f42772, 0x5455188354ce3, 0x4dcca4a3dcbac, 0x3d314d0bfebcb, 0x1defc6ad32b58 },
        { 0x28545089ae7bc, 0x1e38fe9a0c15c, 0x12046e0e2377b, 0x6721c560aa885, 0x0eb28bf671928 },
        { 0x3be1aef5195a7, 0x6f22f62bdb5eb, 0x39768b8523049, 0x43394c8fbfdbd, 0x467d201bf8dd2 },
    },
},
{
    {
        { 0x6f4bd567ae7a9, 0x65ac89317b783, 0x07d3b20fd8932, 0x000f208326916, 0x2ef9c5a5ba384 },
        { 0x6919a74ef4fad, 0x59ed4611452bf, 0x691ec04ea09ef, 0x3cbcb2700e984, 0x71c43c4f5ba3c },
        { 0x56df6fa9e74cd, 0x79c95e4cf56df, 0x7be643bc609e2, 0x149c12ad9e878, 0x5a758ca390c5f },
    },
    {
        { 0x0918b1d61dc94, 0x0d350260cd19c, 0x7a2ab4e37b4d9, 0x21fea735414d7, 0x0a738027f639d },
        { 0x72710d9462495, 0x25aafaa007456, 0x2d21f28eaa31b, 0x17671ea005fd0, 0x2dbae244b3eb7 },
        { 0x74a2f57ffe1cc, 0x1bc3073087301, 0x7ec57f4019c34, 0x34e082e1fa524, 0x2698ca635126a },
    },
    {
        { 0x5702f5e3dd90e, 0x31c9a4a70c5c7, 0x136a5aa78fc24, 0x1992f3b9f7b01, 0x3c004b0c4afa3 },
        { 0x5318832b0ba78, 0x6f24b9ff17cec, 0x0a47f30e060c7, 0x58384540dc8d0, 0x1fb43dcc49cae },
        { 0x146ac06f4b82b, 0x4b500d89e7355, 0x3351e1c728a12, 0x10b9f69932fe3, 0x6b43fd01cd1fd },
    },
    {
        { 0x742583e760ef3, 0x73dc1573216b8, 0x4ae48fdd7714a, 0x4f85f8a13e103, 0x73420b2d6ff0d },
        { 0x75d4b4697c544, 0x11be1fff7f8f4, 0x119e16857f7e1, 0x38a14345cf5d5, 0x5a68d7105b52f },
        { 0x4f6cb9e851e06, 0x278c4471895e5, 0x7efcdce3d64e4, 0x64f6d455c4b4c, 0x3db5632fea34b },
    },
    {
        { 0x190b1829825d5, 0x0e7d3513225c9, 0x1c12be3b7abae, 0x58777781e9ca6, 0x59197ea495df2 },
        { 0x6ee2bf75dd9d8, 0x6c72ceb34be8d, 0x679c9cc345ec7, 0x7898df96898a4, 0x04321adf49d75 },
        { 0x16019e4e55aae, 0x74fc5f25d209c, 0x4566a939ded0d, 0x66063e716e0b7, 0x45eafdc1f4d70 },
    },
    {
        { 0x64624cfccb1ed, 0x257ab8072b6c1, 0x0120725676f0a, 0x4a018d04e8eee, 0x3f73ceea5d56d },
        { 0x401858045d72b, 0x459e5e0ca2d30, 0x488b719308bea, 0x56f4a0d1b32b5, 0x5a5eebc80362d },
        { 0x7bfd10a4e8dc6, 0x7c899366736f4, 0x55ebbeaf95c01, 0x46db060903f8a, 0x2605889126621 },
    },
    {
        { 0x18e3cc676e542, 0x26079d995a990, 0x04a7c217908b2, 0x1dc7603e6655a, 0x0dedfa10b2444 },
        { 0x704a68360ff04, 0x3cecc3cde8b3e, 0x21cd5470f64ff, 0x6abc18d953989, 0x54ad0c2e4e615 },
        { 0x367d5b82b522a, 0x0d3f4b83d7dc7, 0x3067f4cdbc58d, 0x20452da697937, 0x62ecb2baa77a9 },
    },
    {
        { 0x72836afb62874, 0x0af3c2094b240, 0x0c285297f357a, 0x7cc2d5680d6e3, 0x61913d5075663 },
        { 0x5795261152b3d, 0x7a1dbbafa3cbd, 0x5ad31c52588d5, 0x45f3a4164685c, 0x2e59f919a966d },
        { 0x62d361a3231da, 0x65284004e01b8, 0x656533be91d60, 0x6ae016c00a89f, 0x3ddbc2a131c05 },
    },
},
{
    {
        { 0x257a22796bb14, 0x6f360fb443e75, 0x680e47220eaea, 0x2fcf2a5f10c18, 0x5ee7fb38d8320 },
        { 0x40ff9ce5ec54b, 0x57185e261b35b, 0x3e254540e70a9, 0x1b5814003e3f8, 0x78968314ac04b },
        { 0x5fdcb41446a8e, 0x5286926ff2a71, 0x0f231e296b3f6, 0x684a357c84693, 0x61d0633c9bca0 },
    },
    {
        { 0x328bcf8fc73df, 0x3b4de06ff95b4, 0x30aa427ba11a5, 0x5ee31bfda6d9c, 0x5b23ac2df8067 },
        { 0x44935ffdb2566, 0x12f016d176c6e, 0x4fbb00f16f5ae, 0x3fab78d99402a, 0x6e965fd847aed },
        { 0x2b953ee80527b, 0x55f5bcdb1b35a, 0x43a0b3fa23c66, 0x76e07388b820a, 0x79b9bbb9dd95d },
    },
    {
        { 0x17dae8e9f7374, 0x719f76102da33, 0x5117c2a80ca8b, 0x41a66b65d0936, 0x1ba811460accb },
        { 0x355406a3126c2, 0x50d1918727d76, 0x6e5ea0b498e0e, 0x0a3b6063214f2, 0x5065f158c9fd2 },
        { 0x169fb0c429954, 0x59aedd9ecee10, 0x39916eb851802, 0x57917555cc538, 0x3981f39e58a4f },
    },
    {
        { 0x5dfa56de66fde, 0x0058809075908, 0x6d3d8cb854a94, 0x5b2f4e970b1e3, 0x30f4452edcbc1 },
        { 0x38a7559230a93, 0x52c1cde8ba31f, 0x2a4f2d4745a3d, 0x07e9d42d4a28a, 0x38dc083705acd },
        { 0x52782c5759740, 0x53f3397d990ad, 0x3a939c7e84d15, 0x234c4227e39e0, 0x632d9a1a593f2 },
    },
    {
        { 0x1fd11ed0c84a7, 0x021b3ed2757e1, 0x73e1de58fc1c6, 0x5d110c84616ab, 0x3a5a7df28af64 },
        { 0x36b15b807cba6, 0x3f78a9e1afed7, 0x0a59c2c608f1f, 0x52bdd8ecb81b7, 0x0b24f48847ed4 },
        { 0x2d4be511beac7, 0x6bda4d99e5b9b, 0x17e6996914e01, 0x7b1f0ce7fcf80, 0x34fcf74475481 },
    },
    {
        { 0x31dab78cfaa98, 0x4e3216e5e54b7, 0x249823973b689, 0x2584984e48885, 0x0119a3042fb37 },
        { 0x7e04c789767ca, 0x1671b28cfb832, 0x7e57ea2e1c537, 0x1fbaaef444141, 0x3d3bdc164dfa6 },
        { 0x2d89ce8c2177d, 0x6cd12ba182cf4, 0x20a8ac19a7697, 0x539fab2cc72d9, 0x56c088f1ede20 },
    },
    {
        { 0x35fac24f38f02, 0x7d75c6197ab03, 0x33e4bc2a42fa7, 0x1c7cd10b48145, 0x038b7ea483590 },
        { 0x53d1110a86e17, 0x6416eb65f466d, 0x41ca6235fce20, 0x5c3fc8a99bb12, 0x09674c6b99108 },
        { 0x6f82199316ff8, 0x05d54f1a9f3e9, 0x3bcc5d0bd274a, 0x5b284b8d2d5ad, 0x6e5e31025969e },
    },
    {
        { 0x4fb0e63066222, 0x130f59747e660, 0x041868fecd41a, 0x3105e8c923bc6, 0x3058ad43d1838 },
        { 0x462f587e593fb, 0x3d94ba7ce362d, 0x330f9b52667b7, 0x5d45a48e0f00a, 0x08f5114789a8d },
        { 0x40ffde57663d0, 0x71445d4c20647, 0x2653e68170f7c, 0x64cdee3c55ed6, 0x26549fa4efe3d },
    },
},
{
    {
        { 0x68549af3f666e, 0x09e2941d4bb68, 0x2e8311f5dff3c, 0x6429ef91ffbd2, 0x3a10dfe132ce3 },
        { 0x55a461e6bf9d6, 0x78eeef4b02e83, 0x1d34f648c16cf, 0x07fea2aba5132, 0x1926e1dc6401e },
        { 0x74e8aea17cea0, 0x0c743f83fbc0f, 0x7cb03c4bf5455, 0x68a8ba9917e98, 0x1fa1d01d861e5 },
    },
    {
        { 0x4ac00d1df94ab, 0x3ba2101bd271b, 0x7578988b9c4af, 0x0f2bf89f49f7e, 0x73fced18ee9a0 },
        { 0x055947d599832, 0x346fe2aa41990, 0x0164c8079195b, 0x799ccfb7bba27, 0x773563bc6a75c },
        { 0x1e90863139cb3, 0x4f8b407d9a0d6, 0x58e24ca924f69, 0x7a246bbe76456, 0x1f426b701b864 },
    },
    {
        { 0x635c891a12552, 0x26aebd38ede2f, 0x66dc8faddae05, 0x21c7d41a03786, 0x0b76bb1b3fa7e },
        { 0x1264c41911c01, 0x702f44584bdf9, 0x43c511fc68ede, 0x0482c3aed35f9, 0x4e1af5271d31b },
        { 0x0c1f97f92939b, 0x17a88956dc117, 0x6ee005ef99dc7, 0x4aa9172b231cc, 0x7b6dd61eb772a },
    },
    {
        { 0x0abf9ab01d2c7, 0x3880287630ae6, 0x32eca045beddb, 0x57f43365f32d0, 0x53fa9b659bff6 },
        { 0x5c1e850f33d92, 0x1ec119ab9f6f5, 0x7f16f6de663e9, 0x7a7d6cb16dec6, 0x703e9bceaf1d2 },
        { 0x4c8e994885455, 0x4ccb5da9cad82, 0x3596bc610e975, 0x7a80c0ddb9f5e, 0x398d93e5c4c61 },
    },
    {
        { 0x77c60d2e7e3f2, 0x4061051763870, 0x67bc4e0ecd2aa, 0x2bb941f1373b9, 0x699c9c9002c30 },
        { 0x3d16733e248f3, 0x0e2b7e14be389, 0x42c0ddaf6784a, 0x589ea1fc67850, 0x53b09b5ddf191 },
        { 0x6a7235946f1cc, 0x6b99cbb2fbe60, 0x6d3a5d6485c62, 0x4839466e923c0, 0x51caf30c6fcdd },
    },
    {
        { 0x2f99a18ac54c7, 0x398a39661ee6f, 0x384331e40cde3, 0x4cd15c4de19a6, 0x12ae29c189f8e },
        { 0x3a7427674e00a, 0x6142f4f7e74c1, 0x4cc93318c3a15, 0x6d51bac2b1ee7, 0x5504aa292383f },
        { 0x6c0cb1f0d01cf, 0x187469ef5d533, 0x27138883747bf, 0x2f52ae53a90e8, 0x5fd14fe958eba },
    },
    {
        { 0x2fe5ebf93cb8e, 0x226da8acbe788, 0x10883a2fb7ea1, 0x094707842cf44, 0x7dd73f960725d },
        { 0x42ddf2845ab2c, 0x6214ffd3276bb, 0x00b8d181a5246, 0x268a6d579eb20, 0x093ff26e58647 },
        { 0x524fe68059829, 0x65b75e47cb621, 0x15eb0a5d5cc19, 0x05209b3929d5a, 0x2f59bcbc86b47 },
    },
    {
        { 0x1d560b691c301, 0x7f5bafce3ce08, 0x4cd561614806c, 0x4588b6170b188, 0x2aa55e3d01082 },
        { 0x47d429917135f, 0x3eacfa07af070, 0x1deab46b46e44, 0x7a53f3ba46cdf, 0x5458b42e2e51a },
        { 0x192e60c07444f, 0x5ae8843a21daa, 0x6d721910b1538, 0x3321a95a6417e, 0x13e9004a8a768 },
    },
},
{
    {
        { 0x600c9193b877f, 0x21c1b8a0d7765, 0x379927fb38ea2, 0x70d7679dbe01b, 0x5f46040898de9 },
        { 0x58845832fcedb, 0x135cd7f0c6e73, 0x53ffbdfe8e35b, 0x22f195e06e55b, 0x73937e8814bce },
        { 0x37116297bf48d, 0x45a9e0d069720, 0x25af71aa744ec, 0x41af0cb8aaba3, 0x2cf8a4e891d5e },
    },
    {
        { 0x5487e17d06ba2, 0x3872a032d6596, 0x65e28c09348e0, 0x27b6bb2ce40c2, 0x7a6f7f2891d6a },
        { 0x3fd8707110f67, 0x26f8716a92db2, 0x1cdaa1b753027, 0x504be58b52661, 0x2049bd6e58252 },
        { 0x1fd8d6a9aef49, 0x7cb67b7216fa1, 0x67aff53c3b982, 0x20ea610da9628, 0x6011aadfc5459 },
    },
    {
        { 0x6d0c802cbf890, 0x141bfed554c7b, 0x6dbb667ef4263, 0x58f3126857edc, 0x69ce18b779340 },
        { 0x7926dcf95f83c, 0x42e25120e2bec, 0x63de96df1fa15, 0x4f06b50f3f9cc, 0x6fc5cc1b0b62f },
        { 0x75528b29879cb, 0x79a8fd2125a3d, 0x27c8d4b746ab8, 0x0f8893f02210c, 0x15596b3ae5710 },
    },
    {
        { 0x731167e5124ca, 0x17b38e8bbe13f, 0x3d55b942f9056, 0x09c1495be913f, 0x3aa4e241afb6d },
        { 0x739d23f9179a2, 0x632fadbb9e8c4, 0x7c8522bfe0c48, 0x6ed0983ef5aa9, 0x0d2237687b5f4 },
        { 0x138bf2a3305f5, 0x1f45d24d86598, 0x5274bad2160fe, 0x1b6041d58d12a, 0x32fcaa6e4687a },
    },
    {
        { 0x7a4732787ccdf, 0x11e427c7f0640, 0x03659385f8c64, 0x5f4ead9766bfb, 0x746f6336c2600 },
        { 0x56e8dc57d9af5, 0x5b3be17be4f78, 0x3bf928cf82f4b, 0x52e55600a6f11, 0x4627e9cefebd6 },
        { 0x2f345ab6c971c, 0x653286e63e7e9, 0x51061b78a23ad, 0x14999acb54501, 0x7b4917007ed66 },
    },
    {
        { 0x41b28dd53a2dd, 0x37be85f87ea86, 0x74be3d2a85e41, 0x1be87fac96ca6, 0x1d03620fe08cd },
        { 0x5fb5cab84b064, 0x2513e778285b0, 0x457383125e043, 0x6bda3b56e223d, 0x122ba376f844f },
        { 0x232cda2b4e554, 0x0422ba30ff840, 0x751e7667b43f5, 0x6261755da5f3e, 0x02c70bf52b68e },
    },
    {
        { 0x532bf458d72e1, 0x40f96e796b59c, 0x22ef79d6f9da3, 0x501ab67beca77, 0x6b0697e3feb43 },
        { 0x7ec4b5d0b2fbb, 0x200e910595450, 0x742057105715e, 0x2f07022530f60, 0x26334f0a409ef },
        { 0x0f04adf62a3c0, 0x5e0edb48bb6d9, 0x7c34aa4fbc003, 0x7d74e4e5cac24, 0x1cc37f43441b2 },
    },
    {
        { 0x656f1c9ceaeb9, 0x7031cacad5aec, 0x1308cd0716c57, 0x41c1373941942, 0x3a346f772f196 },
        { 0x7565a5cc7324f, 0x01ca0d5244a11, 0x116b067418713, 0x0a57d8c55edae, 0x6c6809c103803 },
        { 0x55112e2da6ac8, 0x6363d0a3dba5a, 0x319c98ba6f40c, 0x2e84b03a36ec7, 0x05911b9f6ef7c },
    },
},
{
    {
        { 0x1acf3512eeaef, 0x2639839692a69, 0x669a234830507, 0x68b920c0603d4, 0x555ef9d1c64b2 },
        { 0x39983f5df0ebb, 0x1ea2589959826, 0x6ce638703cdd6, 0x6311678898505, 0x6b3cecf9aa270 },
        { 0x770ba3b73bd08, 0x11475f7e186d4, 0x0251bc9892bbc, 0x24eab9bffcc5a, 0x675f4de133817 },
    },
    {
        { 0x7f6d93bdab31d, 0x1f3aca5bfd425, 0x2fa521c1c9760, 0x62180ce27f9cd, 0x60f450b882cd3 },
        { 0x452036b1782fc, 0x02d95b07681c5, 0x5901cf99205b2, 0x290686e5eecb4, 0x13d99df70164c },
        { 0x35ec321e5c0ca, 0x13ae337f44029, 0x4008e813f2da7, 0x640272f8e0c3a, 0x1c06de9e55eda },
    },
    {
        { 0x52b40ff6d69aa, 0x31b8809377ffa, 0x536625cd14c2c, 0x516af252e17d1, 0x78096f8e7d32b },
        { 0x77ad6a33ec4e2, 0x717c5dc11d321, 0x4a114559823e4, 0x306ce50a1e2b1, 0x4cf38a1fec2db },
        { 0x2aa650dfa5ce7, 0x54916a8f19415, 0x00dc96fe71278, 0x55f2784e63eb8, 0x373cad3a26091 },
    },
    {
        { 0x6a8fb89ddbbad, 0x78c35d5d97e37, 0x66e3674ef2cb2, 0x34347ac53dd8f, 0x21547eda5112a },
        { 0x4634d82c9f57c, 0x4249268a6d652, 0x6336d687f2ff7, 0x4fe4f4e26d9a0, 0x0040f3d945441 },
        { 0x5e939fd5986d3, 0x12a2147019bdf, 0x4c466e7d09cb2, 0x6fa5b95d203dd, 0x63550a334a254 },
    },
    {
        { 0x2584572547b49, 0x75c58811c1377, 0x4d3c637cc171b, 0x33d30747d34e3, 0x39a92bafaa7d7 },
        { 0x7d6edb569cf37, 0x60194a5dc2ca0, 0x5af59745e10a6, 0x7a8f53e004875, 0x3eea62c7daf78 },
        { 0x4c713e693274e, 0x6ed1b7a6eb3a4, 0x62ace697d8e15, 0x266b8292ab075, 0x68436a0665c9c },
    },
    {
        { 0x6d317e820107c, 0x090815d2ca3ca, 0x03ff1eb1499a1, 0x23960f050e319, 0x5373669c91611 },
        { 0x235e8202f3f27, 0x44c9f2eb61780, 0x630905b1d7003, 0x4fcc8d274ead1, 0x17b6e7f68ab78 },
        { 0x014ab9a0e5257, 0x09939567f8ba5, 0x4b47b2a423c82, 0x688d7e57ac42d, 0x1cb4b5a678f87 },
    },
    {
        { 0x4aa62a2a007e7, 0x61e0e38f62d6e, 0x02f888fcc4782, 0x7562b83f21c00, 0x2dc0fd2d82ef6 },
        { 0x4c06b394afc6c, 0x4931b4bf636cc, 0x72b60d0322378, 0x25127c6818b25, 0x330bca78de743 },
        { 0x6ff841119744e, 0x2c560e8e49305, 0x7254fefe5a57a, 0x67ae2c560a7df, 0x3c31be1b369f1 },
    },
    {
        { 0x0bc93f9cb4272, 0x3f8f9db73182d, 0x2b235eabae1c4, 0x2ddbf8729551a, 0x41cec1097e7d5 },
        { 0x4864d08948aee, 0x5d237438df61e, 0x2b285601f7067, 0x25dbcbae6d753, 0x330b61134262d },
        { 0x619d7a26d808a, 0x3c3b3c2adbef2, 0x6877c9eec7f52, 0x3beb9ebe1b66d, 0x26b44cd91f287 },
    },
},
{
    {
        { 0x7f29362730383, 0x7fd7951459c36, 0x7504c512d49e7, 0x087ed7e3bc55f, 0x7deb10149c726 },
        { 0x048478f387475, 0x69397d9678a3e, 0x67c8156c976f3, 0x2eb4d5589226c, 0x2c709e6c1c10a },
        { 0x2af6a8766ee7a, 0x08aaa79a1d96c, 0x42f92d59b2fb0, 0x1752c40009c07, 0x08e68e9ff62ce },
    },
    {
        { 0x509d50ab8f2f9, 0x1b8ab247be5e5, 0x5d9b2e6b2e486, 0x4faa5479a1339, 0x4cb13bd738f71 },
        { 0x5500a4bc130ad, 0x127a17a938695, 0x02a26fa34e36d, 0x584d12e1ecc28, 0x2f1f3f87eeba3 },
        { 0x48c75e515b64a, 0x75b6952071ef0, 0x5d46d42965406, 0x7746106989f9f, 0x19a1e353c0ae2 },
    },
    {
        { 0x172cdd596bdbd, 0x0731ddf881684, 0x10426d64f8115, 0x71a4fd8a9a3da, 0x736bd3990266a },
        { 0x47560bafa05c3, 0x418dcabcc2fa3, 0x35991cecf8682, 0x24371a94b8c60, 0x41546b11c20c3 },
        { 0x32d509334b3b4, 0x16c102cae70aa, 0x1720dd51bf445, 0x5ae662faf9821, 0x412295a2b87fa },
    },
    {
        { 0x55261e293eac6, 0x06426759b65cc, 0x40265ae116a48, 0x6c02304bae5bc, 0x0760bb8d195ad },
        { 0x19b88f57ed6e9, 0x4cdbf1904a339, 0x42b49cd4e4f2c, 0x71a2e771909d9, 0x14e153ebb52d2 },
        { 0x61a17cde6818a, 0x53dad34108827, 0x32b32c55c55b6, 0x2f9165f9347a3, 0x6b34be9bc33ac },
    },
    {
        { 0x469656571f2d3, 0x0aa61ce6f423f, 0x3f940d71b27a1, 0x185f19d73d16a, 0x01b9c7b62e6dd },
        { 0x72f643a78c0b2, 0x3de45c04f9e7b, 0x706d68d30fa5c, 0x696f63e8e2f24, 0x2012c18f0922d },
        { 0x355e55ac89d29, 0x3e8b414ec7101, 0x39db07c520c90, 0x6f41e9b77efe1, 0x08af5b784e4ba },
    },
    {
        { 0x314d289cc2c4b, 0x23450e2f1bc4e, 0x0cd93392f92f4, 0x1370c6a946b7d, 0x6423c1d5afd98 },
        { 0x499dc881f2533, 0x34ef26476c506, 0x4d107d2741497, 0x346c4bd6efdb3, 0x32b79d71163a1 },
        { 0x5f8d9edfcb36a, 0x1e6e8dcbf3990, 0x7974f348af30a, 0x6e6724ef19c7c, 0x480a5efbc13e2 },
    },
    {
        { 0x14ce442ce221f, 0x18980a72516cc, 0x072f80db86677, 0x703331fda526e, 0x24b31d47691c8 },
        { 0x1e70b01622071, 0x1f163b5f8a16a, 0x56aaf341ad417, 0x7989635d830f7, 0x47aa27600cb7b },
        { 0x41eedc015f8c3, 0x7cf8d27ef854a, 0x289e3584693f9, 0x04a7857b309a7, 0x545b585d14dda },
    },
    {
        { 0x4e4d0e3b321e1, 0x7451fe3d2ac40, 0x666f678eea98d, 0x038858667fead, 0x4d22dc3e64c8d },
        { 0x7275ea0d43a0f, 0x681137dd7ccf7, 0x1e79cbab79a38, 0x22a214489a66a, 0x0f62f9c332ba5 },
        { 0x46589d63b5f39, 0x7eaf979ec3f96, 0x4ebe81572b9a8, 0x21b7f5d61694a, 0x1c0fa01a36371 },
    },
},
{
    {
        { 0x02b0e8c936a50, 0x6b83b58b6cd21, 0x37ed8d3e72680, 0x0a037db9f2a62, 0x4005419b1d2bc },
        { 0x604b622943dff, 0x1c899f6741a58, 0x60219e2f232fb, 0x35fae92a7f9cb, 0x0fa3614f3b1ca },
        { 0x3febdb9be82f0, 0x5e74895921400, 0x553ea38822706, 0x5a17c24cfc88c, 0x1fba218aef40a },
    },
    {
        { 0x657043e7b0194, 0x5c11b55efe9e7, 0x7737bc6a074fb, 0x0eae41ce355cc, 0x6c535d13ff776 },
        { 0x49448fac8f53e, 0x34f74c6e8356a, 0x0ad780607dba2, 0x7213a7eb63eb6, 0x392e3acaa8c86 },
        { 0x534e93e8a35af, 0x08b10fd02c997, 0x26ac2acb81e05, 0x09d8c98ce3b79, 0x25e17fe4d50ac },
    },
    {
        { 0x77ff576f121a7, 0x4e5f9b0fc722b, 0x46f949b0d28c8, 0x4cde65d17ef26, 0x6bba828f89698 },
        { 0x09bd71e04f676, 0x25ac841f2a145, 0x1a47eac823871, 0x1a8a8c36c581a, 0x255751442a9fb },
        { 0x1bc6690fe3901, 0x314132f5abc5a, 0x611835132d528, 0x5f24b8eb48a57, 0x559d504f7f6b7 },
    },
    {
        { 0x091e7f6d266fd, 0x36060ef037389, 0x18788ec1d1286, 0x287441c478eb0, 0x123ea6a3354bd },
        { 0x38378b3eb54d5, 0x4d4aaa78f94ee, 0x4a002e875a74d, 0x10b851367b17c, 0x01ab12d5807e3 },
        { 0x5189041e32d96, 0x05b062b090231, 0x0c91766e7b78f, 0x0aa0f55a138ec, 0x4a3961e2c918a },
    },
    {
        { 0x7d644f3233f1e, 0x1c69f9e02c064, 0x36ae5e5266898, 0x08fc1dad38b79, 0x68aceead9bd41 },
        { 0x43be0f8e6bba0, 0x68fdffc614e3b, 0x4e91dab5b3be0, 0x3b1d4c9212ff0, 0x2cd6bce3fb1db },
        { 0x4c90ef3d7c210, 0x496f5a0818716, 0x79cf88cc239b8, 0x2cb9c306cf8db, 0x595760d5b508f },
    },
    {
        { 0x2cbebfd022790, 0x0b8822aec1105, 0x4d1cfd226bccc, 0x515b2fa4971be, 0x2cb2c5df54515 },
        { 0x1bfe104aa6397, 0x11494ff996c25, 0x64251623e5800, 0x0d49fc5e044be, 0x709fa43edcb29 },
        { 0x25d8c63fd2aca, 0x4c5cd29dffd61, 0x32ec0eb48af05, 0x18f9391f9b77c, 0x70f029ecf0c81 },
    },
    {
        { 0x2afaa5e10b0b9, 0x61de08355254d, 0x0eb587de3c28d, 0x4f0bb9f7dbbd5, 0x44eca5a2a74bd },
        { 0x307b32eed3e33, 0x6748ab03ce8c2, 0x57c0d9ab810bc, 0x42c64a224e98c, 0x0b7d5d8a6c314 },
        { 0x448327b95d543, 0x0146681e3a4ba, 0x38714adc34e0c, 0x4f26f0e298e30, 0x272224512c7de },
    },
    {
        { 0x3bb8a42a975fc, 0x6f2d5b46b17ef, 0x7b6a9223170e5, 0x053713fe3b7e6, 0x19735fd7f6bc2 },
        { 0x492af49c5342e, 0x2365cdf5a0357, 0x32138a7ffbb60, 0x2a1f7d14646fe, 0x11b5df18a44cc },
        { 0x390d042c84266, 0x1efe32a8fdc75, 0x6925ee7ae1238, 0x4af9281d0e832, 0x0fef911191df8 },
    },
},
};
#else
/* base[i][j] = (j+1)*256^i*B */
static const ge_precomp base[32][8] = {
{
 {
  { 25967493,-14356035,29566456,3660896,-12694345,4014787,27544626,-11754271,-6079156,2047605 },
  { -12545711,934262,-2722910,3049990,-727428,9406986,12720692,5043384,19500929,-15469378 },
  { -8738181,4489570,9688441,-14785194,10184609,-12363380,29287919,11864899,-24514362,-4438546 },
 },
 {
  { -12815894,-12976347,-21581243,11784320,-25355658,-2750717,-11717903,-3814571,-358445,-10211303 },
  { -21703237,6903825,27185491,6451973,-29577724,-9554005,-15616551,11189268,-26829678,-5319081 },
  { 26966642,11152617,32442495,15396054,14353839,-12752335,-3128826,-9541118,-15472047,-4166697 },
 },
 {
  { 15636291,-9688557,24204773,-7912398,616977,-16685262,27787600,-14772189,28944400,-1550024 },
  { 16568933,4717097,-11556148,-1102322,15682896,-11807043,16354577,-11775962,7689662,11199574 },
  { 30464156,-5976125,-11779434,-15670865,23220365,15915852,7512774,10017326,-17749093,-9920357 },
 },
 {
  { -17036878,13921892,10945806,-6033431,27105052,-16084379,-28926210,15006023,3284568,-6276540 },
  { 23599295,-8306047,-11193664,-7687416,13236774,10506355,7464579,9656445,13059162,10374397 },
  { 7798556,16710257,3033922,2874086,28997861,2835604,32406664,-3839045,-641708,-101325 },
 },
 {
  { 10861363,11473154,27284546,1981175,-30064349,12577861,32867885,14515107,-15438304,10819380 },
  { 4708026,6336745,20377586,9066809,-11272109,6594696,-25653668,12483688,-12668491,5581306 },
  { 19563160,16186464,-29386857,4097519,10237984,-4348115,28542350,13850243,-23678021,-15815942 },
 },
 {
  { -15371964,-12862754,32573250,4720197,-26436522,5875511,-19188627,-15224819,-9818940,-12085777 },
  { -8549212,109983,15149363,2178705,22900618,4543417,3044240,-15689887,1762328,14866737 },
  { -18199695,-15951423,-10473290,1707278,-17185920,3916101,-28236412,3959421,27914454,4383652 },
 },
 {
  { 5153746,9909285,1723747,-2777874,30523605,5516873,19480852,5230134,-23952439,-15175766 },
  { -30269007,-3463509,7665486,10083793,28475525,1649722,20654025,16520125,30598449,7715701 },
  { 28881845,14381568,9657904,3680757,-20181635,7843316,-31400660,1370708,29794553,-1409300 },
 },
 {
  { 14499471,-2729599,-33191113,-4254652,28494862,14271267,30290735,10876454,-33154098,2381726 },
  { -7195431,-2655363,-14730155,462251,-27724326,3941372,-6236617,3696005,-32300832,15351955 },
  { 27431194,8222322,16448760,-3907995,-18707002,11938355,-32961401,-2970515,29551813,10109425 },
 },
},
{
 {
  { -13657040,-13155431,-31283750,11777098,21447386,6519384,-2378284,-1627556,10092783,-4764171 },
  { 27939166,14210322,4677035,16277044,-22964462,-12398139,-32508754,12005538,-17810127,12803510 },
  { 17228999,-15661624,-1233527,300140,-1224870,-11714777,30364213,-9038194,18016357,4397660 },
 },
 {
  { -10958843,-7690207,4776341,-14954238,27850028,-15602212,-26619106,14544525,-17477504,982639 },
  { 29253598,15796703,-2863982,-9908884,10057023,3163536,7332899,-4120128,-21047696,9934963 },
  { 5793303,16271923,-24131614,-10116404,29188560,1206517,-14747930,4559895,-30123922,-10897950 },
 },
 {
  { -27643952,-11493006,16282657,-11036493,28414021,-15012264,24191034,4541697,-13338309,5500568 },
  { 12650548,-1497113,9052871,11355358,-17680037,-8400164,-17430592,12264343,10874051,13524335 },
  { 25556948,-3045990,714651,2510400,23394682,-10415330,33119038,5080568,-22528059,5376628 },
 },
 {
  { -26088264,-4011052,-17013699,-3537628,-6726793,1920897,-22321305,-9447443,4535768,1569007 },
  { -2255422,14606630,-21692440,-8039818,28430649,8775819,-30494562,3044290,31848280,12543772 },
  { -22028579,2943893,-31857513,6777306,13784462,-4292203,-27377195,-2062731,7718482,14474653 },
 },
 {
  { 2385315,2454213,-22631320,46603,-4437935,-15680415,656965,-7236665,24316168,-5253567 },
  { 13741529,10911568,-33233417,-8603737,-20177830,-1033297,33040651,-13424532,-20729456,8321686 },
  { 21060490,-2212744,15712757,-4336099,1639040,10656336,23845965,-11874838,-9984458,608372 },
 },
 {
  { -13672732,-15087586,-10889693,-7557059,-6036909,11305547,1123968,-6780577,27229399,23887 },
  { -23244140,-294205,-11744728,14712571,-29465699,-2029617,12797024,-6440308,-1633405,16678954 },
  { -29500620,4770662,-16054387,14001338,7830047,9564805,-1508144,-4795045,-17169265,4904953 },
 },
 {
  { 24059557,14617003,19037157,-15039908,19766093,-14906429,5169211,16191880,2128236,-4326833 },
  { -16981152,4124966,-8540610,-10653797,30336522,-14105247,-29806336,916033,-6882542,-2986532 },
  { -22630907,12419372,-7134229,-7473371,-16478904,16739175,285431,2763829,15736322,4143876 },
 },
 {
  { 2379352,11839345,-4110402,-5988665,11274298,794957,212801,-14594663,23527084,-16458268 },
  { 33431127,-11130478,-17838966,-15626900,8909499,8376530,-32625340,4087881,-15188911,-14416214 },
  { 1767683,7197987,-13205226,-2022635,-13091350,448826,5799055,4357868,-4774191,-16323038 },
 },
},
{
 {
  { 6721966,13833823,-23523388,-1551314,26354293,-11863321,23365147,-3949732,7390890,2759800 },
  { 4409041,2052381,23373853,10530217,7676779,-12885954,21302353,-4264057,1244380,-12919645 },
  { -4421239,7169619,4982368,-2957590,30256825,-2777540,14086413,9208236,15886429,16489664 },
 },
 {
  { 1996075,10375649,14346367,13311202,-6874135,-16438411,-13693198,398369,-30606455,-712933 },
  { -25307465,9795880,-2777414,14878809,-33531835,14780363,13348553,12076947,-30836462,5113182 },
  { -17770784,11797796,31950843,13929123,-25888302,12288344,-30341101,-7336386,13847711,5387222 },
 },
 {
  { -18582163,-3416217,17824843,-2340966,22744343,-10442611,8763061,3617786,-19600662,10370991 },
  { 20246567,-14369378,22358229,-543712,18507283,-10413996,14554437,-8746092,32232924,16763880 },
  { 9648505,10094563,26416693,14745928,-30374318,-6472621,11094161,15689506,3140038,-16510092 },
 },
 {
  { -16160072,5472695,31895588,4744994,8823515,10365685,-27224800,9448613,-28774454,366295 },
  { 19153450,11523972,-11096490,-6503142,-24647631,5420647,28344573,8041113,719605,11671788 },
  { 8678025,2694440,-6808014,2517372,4964326,11152271,-15432916,-15266516,27000813,-10195553 },
 },
 {
  { -15157904,7134312,8639287,-2814877,-7235688,10421742,564065,5336097,6750977,-14521026 },
  { 11836410,-3979488,26297894,16080799,23455045,15735944,1695823,-8819122,8169720,16220347 },
  { -18115838,8653647,17578566,-6092619,-8025777,-16012763,-11144307,-2627664,-5990708,-14166033 },
 },
 {
  { -23308498,-10968312,15213228,-10081214,-30853605,-11050004,27884329,2847284,2655861,1738395 },
  { -27537433,-14253021,-25336301,-8002780,-9370762,8129821,21651608,-3239336,-19087449,-11005278 },
  { 1533110,3437855,23735889,459276,29970501,11335377,26030092,5821408,10478196,8544890 },
 },
 {
  { 32173121,-16129311,24896207,3921497,22579056,-3410854,19270449,12217473,17789017,-3395995 },
  { -30552961,-2228401,-15578829,-10147201,13243889,517024,15479401,-3853233,30460520,1052596 },
  { -11614875,13323618,32618793,8175907,-15230173,12596687,27491595,-4612359,3179268,-9478891 },
 },
 {
  { 31947069,-14366651,-4640583,-15339921,-15125977,-6039709,-14756777,-16411740,19072640,-9511060 },
  { 11685058,11822410,3158003,-13952594,33402194,-4165066,5977896,-5215017,473099,5040608 },
  { -20290863,8198642,-27410132,11602123,1290375,-2799760,28326862,1721092,-19558642,-3131606 },
 },
},
{
 {
  { 7881532,10687937,7578723,7738378,-18951012,-2553952,21820786,8076149,-27868496,11538389 },
  { -19935666,3899861,18283497,-6801568,-15728660,-11249211,8754525,7446702,-5676054,5797016 },
  { -11295600,-3793569,-15782110,-7964573,12708869,-8456199,2014099,-9050574,-2369172,-5877341 },
 },
 {
  { -22472376,-11568741,-27682020,1146375,18956691,16640559,1192730,-3714199,15123619,10811505 },
  { 14352098,-3419715,-18942044,10822655,32750596,4699007,-70363,15776356,-28886779,-11974553 },
  { -28241164,-8072475,-4978962,-5315317,29416931,1847569,-20654173,-16484855,4714547,-9600655 },
 },
 {
  { 15200332,8368572,19679101,15970074,-31872674,1959451,24611599,-4543832,-11745876,12340220 },
  { 12876937,-10480056,33134381,6590940,-6307776,14872440,9613953,8241152,15370987,9608631 },
  { -4143277,-12014408,8446281,-391603,4407738,13629032,-7724868,15866074,-28210621,-8814099 },
 },
 {
  { 26660628,-15677655,8393734,358047,-7401291,992988,-23904233,858697,20571223,8420556 },
  { 14620715,13067227,-15447274,8264467,14106269,15080814,33531827,12516406,-21574435,-12476749 },
  { 236881,10476226,57258,-14677024,6472998,2466984,17258519,7256740,8791136,15069930 },
 },
 {
  { 1276410,-9371918,22949635,-16322807,-23493039,-5702186,14711875,4874229,-30663140,-2331391 },
  { 5855666,4990204,-13711848,7294284,-7804282,1924647,-1423175,-7912378,-33069337,9234253 },
  { 20590503,-9018988,31529744,-7352666,-2706834,10650548,31559055,-11609587,18979186,13396066 },
 },
 {
  { 24474287,4968103,22267082,4407354,24063882,-8325180,-18816887,13594782,33514650,7021958 },
  { -11566906,-6565505,-21365085,15928892,-26158305,4315421,-25948728,-3916677,-21480480,12868082 },
  { -28635013,13504661,19988037,-2132761,21078225,6443208,-21446107,2244500,-12455797,-8089383 },
 },
 {
  { -30595528,13793479,-5852820,319136,-25723172,-6263899,33086546,8957937,-15233648,5540521 },
  { -11630176,-11503902,-8119500,-7643073,2620056,1022908,-23710744,-1568984,-16128528,-14962807 },
  { 23152971,775386,27395463,14006635,-9701118,4649512,1689819,892185,-11513277,-15205948 },
 },
 {
  { 9770129,9586738,26496094,4324120,1556511,-3550024,27453819,4763127,-19179614,5867134 },
  { -32765025,1927590,31726409,-4753295,23962434,-16019500,27846559,5931263,-29749703,-16108455 },
  { 27461885,-2977536,22380810,1815854,-23033753,-3031938,7283490,-15148073,-19526700,7734629 },
 },
},
{
 {
  { -8010264,-9590817,-11120403,6196038,29344158,-13430885,7585295,-3176626,18549497,15302069 },
  { -32658337,-6171222,-7672793,-11051681,6258878,13504381,10458790,-6418461,-8872242,8424746 },
  { 24687205,8613276,-30667046,-3233545,1863892,-1830544,19206234,7134917,-11284482,-828919 },
 },
 {
  { 11334899,-9218022,8025293,12707519,17523892,-10476071,10243738,-14685461,-5066034,16498837 },
  { 8911542,6887158,-9584260,-6958590,11145641,-9543680,17303925,-14124238,6536641,10543906 },
  { -28946384,15479763,-17466835,568876,-1497683,11223454,-2669190,-16625574,-27235709,8876771 },
 },
 {
  { -25742899,-12566864,-15649966,-846607,-33026686,-796288,-33481822,15824474,-604426,-9039817 },
  { 10330056,70051,7957388,-9002667,9764902,15609756,27698697,-4890037,1657394,3084098 },
  { 10477963,-7470260,12119566,-13250805,29016247,-5365589,31280319,14396151,-30233575,15272409 },
 },
 {
  { -12288309,3169463,28813183,16658753,25116432,-5630466,-25173957,-12636138,-25014757,1950504 },
  { -26180358,9489187,11053416,-14746161,-31053720,5825630,-8384306,-8767532,15341279,8373727 },
  { 28685821,7759505,-14378516,-12002860,-31971820,4079242,298136,-10232602,-2878207,15190420 },
 },
 {
  { -32932876,13806336,-14337485,-15794431,-24004620,10940928,8669718,2742393,-26033313,-6875003 },
  { -1580388,-11729417,-25979658,-11445023,-17411874,-10912854,9291594,-16247779,-12154742,6048605 },
  { -30305315,14843444,1539301,11864366,20201677,1900163,13934231,5128323,11213262,9168384 },
 },
 {
  { -26280513,11007847,19408960,-940758,-18592965,-4328580,-5088060,-11105150,20470157,-16398701 },
  { -23136053,9282192,14855179,-15390078,-7362815,-14408560,-22783952,14461608,14042978,5230683 },
  { 29969567,-2741594,-16711867,-8552442,9175486,-2468974,21556951,3506042,-5933891,-12449708 },
 },
 {
  { -3144746,8744661,19704003,4581278,-20430686,6830683,-21284170,8971513,-28539189,15326563 },
  { -19464629,10110288,-17262528,-3503892,-23500387,1355669,-15523050,15300988,-20514118,9168260 },
  { -5353335,4488613,-23803248,16314347,7780487,-15638939,-28948358,9601605,33087103,-9011387 },
 },
 {
  { -19443170,-15512900,-20797467,-12445323,-29824447,10229461,-27444329,-15000531,-5996870,15664672 },
  { 23294591,-16632613,-22650781,-8470978,27844204,11461195,13099750,-2460356,18151676,13417686 },
  { -24722913,-4176517,-31150679,5988919,-26858785,6685065,1661597,-12551441,15271676,-15452665 },
 },
},
{
 {
  { 11433042,-13228665,8239631,-5279517,-1985436,-725718,-18698764,2167544,-6921301,-13440182 },
  { -31436171,15575146,30436815,12192228,-22463353,9395379,-9917708,-8638997,12215110,12028277 },
  { 14098400,6555944,23007258,5757252,-15427832,-12950502,30123440,4617780,-16900089,-655628 },
 },
 {
  { -4026201,-15240835,11893168,13718664,-14809462,1847385,-15819999,10154009,23973261,-12684474 },
  { -26531820,-3695990,-1908898,2534301,-31870557,-16550355,18341390,-11419951,32013174,-10103539 },
  { -25479301,10876443,-11771086,-14625140,-12369567,1838104,21911214,6354752,4425632,-837822 },
 },
 {
  { -10433389,-14612966,22229858,-3091047,-13191166,776729,-17415375,-12020462,4725005,14044970 },
  { 19268650,-7304421,1555349,8692754,-21474059,-9910664,6347390,-1411784,-19522291,-16109756 },
  { -24864089,12986008,-10898878,-5558584,-11312371,-148526,19541418,8180106,9282262,10282508 },
 },
 {
  { -26205082,4428547,-8661196,-13194263,4098402,-14165257,15522535,8372215,5542595,-10702683 },
  { -10562541,14895633,26814552,-16673850,-17480754,-2489360,-2781891,6993761,-18093885,10114655 },
  { -20107055,-929418,31422704,10427861,-7110749,6150669,-29091755,-11529146,25953725,-106158 },
 },
 {
  { -4234397,-8039292,-9119125,3046000,2101609,-12607294,19390020,6094296,-3315279,12831125 },
  { -15998678,7578152,5310217,14408357,-33548620,-224739,31575954,6326196,7381791,-2421839 },
  { -20902779,3296811,24736065,-16328389,18374254,7318640,6295303,8082724,-15362489,12339664 },
 },
 {
  { 27724736,2291157,6088201,-14184798,1792727,5857634,13848414,15768922,25091167,14856294 },
  { -18866652,8331043,24373479,8541013,-701998,-9269457,12927300,-12695493,-22182473,-9012899 },
  { -11423429,-5421590,11632845,3405020,30536730,-11674039,-27260765,13866390,30146206,9142070 },
 },
 {
  { 3924129,-15307516,-13817122,-10054960,12291820,-668366,-27702774,9326384,-8237858,4171294 },
  { -15921940,16037937,6713787,16606682,-21612135,2790944,26396185,3731949,345228,-5462949 },
  { -21327538,13448259,25284571,1143661,20614966,-8849387,2031539,-12391231,-16253183,-13582083 },
 },
 {
  { 31016211,-16722429,26371392,-14451233,-5027349,14854137,17477601,3842657,28012650,-16405420 },
  { -5075835,9368966,-8562079,-4600902,-15249953,6970560,-9189873,16292057,-8867157,3507940 },
  { 29439664,3537914,23333589,6997794,-17555561,-11018068,-15209202,-15051267,-9164929,6580396 },
 },
},
{
 {
  { -12185861,-7679788,16438269,10826160,-8696817,-6235611,17860444,-9273846,-2095802,9304567 },
  { 20714564,-4336911,29088195,7406487,11426967,-5095705,14792667,-14608617,5289421,-477127 },
  { -16665533,-10650790,-6160345,-13305760,9192020,-1802462,17271490,12349094,26939669,-3752294 },
 },
 {
  { -12889898,9373458,31595848,16374215,21471720,13221525,-27283495,-12348559,-3698806,117887 },
  { 22263325,-6560050,3984570,-11174646,-15114008,-566785,28311253,5358056,-23319780,541964 },
  { 16259219,3261970,2309254,-15534474,-16885711,-4581916,24134070,-16705829,-13337066,-13552195 },
 },
 {
  { 9378160,-13140186,-22845982,-12745264,28198281,-7244098,-2399684,-717351,690426,14876244 },
  { 24977353,-314384,-8223969,-13465086,28432343,-1176353,-13068804,-12297348,-22380984,6618999 },
  { -1538174,11685646,12944378,13682314,-24389511,-14413193,8044829,-13817328,32239829,-5652762 },
 },
 {
  { -18603066,4762990,-926250,8885304,-28412480,-3187315,9781647,-10350059,32779359,5095274 },
  { -33008130,-5214506,-32264887,-3685216,9460461,-9327423,-24601656,14506724,21639561,-2630236 },
  { -16400943,-13112215,25239338,15531969,3987758,-4499318,-1289502,-6863535,17874574,558605 },
 },
 {
  { -13600129,10240081,9171883,16131053,-20869254,9599700,33499487,5080151,2085892,5119761 },
  { -22205145,-2519528,-16381601,414691,-25019550,2170430,30634760,-8363614,-31999993,-5759884 },
  { -6845704,15791202,8550074,-1312654,29928809,-12092256,27534430,-7192145,-22351378,12961482 },
 },
 {
  { -24492060,-9570771,10368194,11582341,-23397293,-2245287,16533930,8206996,-30194652,-5159638 },
  { -11121496,-3382234,2307366,6362031,-135455,8868177,-16835630,7031275,7589640,8945490 },
  { -32152748,8917967,6661220,-11677616,-1192060,-15793393,7251489,-11182180,24099109,-14456170 },
 },
 {
  { 5019558,-7907470,4244127,-14714356,-26933272,6453165,-19118182,-13289025,-6231896,-10280736 },
  { 10853594,10721687,26480089,5861829,-22995819,1972175,-1866647,-10557898,-3363451,-6441124 },
  { -17002408,5906790,221599,-6563147,7828208,-13248918,24362661,-2008168,-13866408,7421392 },
 },
 {
  { 8139927,-6546497,32257646,-5890546,30375719,1886181,-21175108,15441252,28826358,-4123029 },
  { 6267086,9695052,7709135,-16603597,-32869068,-1886135,14795160,-7840124,13746021,-1742048 },
  { 28584902,7787108,-6732942,-15050729,22846041,-7571236,-3181936,-363524,4771362,-8419958 },
 },
},
{
 {
  { 24949256,6376279,-27466481,-8174608,-18646154,-9930606,33543569,-12141695,3569627,11342593 },
  { 26514989,4740088,27912651,3697550,19331575,-11472339,6809886,4608608,7325975,-14801071 },
  { -11618399,-14554430,-24321212,7655128,-1369274,5214312,-27400540,10258390,-17646694,-8186692 },
 },
 {
  { 11431204,15823007,26570245,14329124,18029990,4796082,-31446179,15580664,9280358,-3973687 },
  { -160783,-10326257,-22855316,-4304997,-20861367,-13621002,-32810901,-11181622,-15545091,4387441 },
  { -20799378,12194512,3937617,-5805892,-27154820,9340370,-24513992,8548137,20617071,-7482001 },
 },
 {
  { -938825,-3930586,-8714311,16124718,24603125,-6225393,-13775352,-11875822,24345683,10325460 },
  { -19855277,-1568885,-22202708,8714034,14007766,6928528,16318175,-1010689,4766743,3552007 },
  { -21751364,-16730916,1351763,-803421,-4009670,3950935,3217514,14481909,10988822,-3994762 },
 },
 {
  { 15564307,-14311570,3101243,5684148,30446780,-8051356,12677127,-6505343,-8295852,13296005 },
  { -9442290,6624296,-30298964,-11913677,-4670981,-2057379,31521204,9614054,-30000824,12074674 },
  { 4771191,-135239,14290749,-13089852,27992298,14998318,-1413936,-1556716,29832613,-16391035 },
 },
 {
  { 7064884,-7541174,-19161962,-5067537,-18891269,-2912736,25825242,5293297,-27122660,13101590 },
  { -2298563,2439670,-7466610,1719965,-27267541,-16328445,32512469,-5317593,-30356070,-4190957 },
  { -30006540,10162316,-33180176,3981723,-16482138,-13070044,14413974,9515896,19568978,9628812 },
 },
 {
  { 33053803,199357,15894591,1583059,27380243,-4580435,-17838894,-6106839,-6291786,3437740 },
  { -18978877,3884493,19469877,12726490,15913552,13614290,-22961733,70104,7463304,4176122 },
  { -27124001,10659917,11482427,-16070381,12771467,-6635117,-32719404,-5322751,24216882,5944158 },
 },
 {
  { 8894125,7450974,-2664149,-9765752,-28080517,-12389115,19345746,14680796,11632993,5847885 },
  { 26942781,-2315317,9129564,-4906607,26024105,11769399,-11518837,6367194,-9727230,4782140 },
  { 19916461,-4828410,-22910704,-11414391,25606324,-5972441,33253853,8220911,6358847,-1873857 },
 },
 {
  { 801428,-2081702,16569428,11065167,29875704,96627,7908388,-4480480,-13538503,1387155 },
  { 19646058,5720633,-11416706,12814209,11607948,12749789,14147075,15156355,-21866831,11835260 },
  { 19299512,1155910,28703737,14890794,2925026,7269399,26121523,15467869,-26560550,5052483 },
 },
},
{
 {
  { -3017432,10058206,1980837,3964243,22160966,12322533,-6431123,-12618185,12228557,-7003677 },
  { 32944382,14922211,-22844894,5188528,21913450,-8719943,4001465,13238564,-6114803,8653815 },
  { 22865569,-4652735,27603668,-12545395,14348958,8234005,24808405,5719875,28483275,2841751 },
 },
 {
  { -16420968,-1113305,-327719,-12107856,21886282,-15552774,-1887966,-315658,19932058,-12739203 },
  { -11656086,10087521,-8864888,-5536143,-19278573,-3055912,3999228,13239134,-4777469,-13910208 },
  { 1382174,-11694719,17266790,9194690,-13324356,9720081,20403944,11284705,-14013818,3093230 },
 },
 {
  { 16650921,-11037932,-1064178,1570629,-8329746,7352753,-302424,16271225,-24049421,-6691850 },
  { -21911077,-5927941,-4611316,-5560156,-31744103,-10785293,24123614,15193618,-21652117,-16739389 },
  { -9935934,-4289447,-25279823,4372842,2087473,10399484,31870908,14690798,17361620,11864968 },
 },
 {
  { -11307610,6210372,13206574,5806320,-29017692,-13967200,-12331205,-7486601,-25578460,-16240689 },
  { 14668462,-12270235,26039039,15305210,25515617,4542480,10453892,6577524,9145645,-6443880 },
  { 5974874,3053895,-9433049,-10385191,-31865124,3225009,-7972642,3936128,-5652273,-3050304 },
 },
 {
  { 30625386,-4729400,-25555961,-12792866,-20484575,7695099,17097188,-16303496,-27999779,1803632 },
  { -3553091,9865099,-5228566,4272701,-5673832,-16689700,14911344,12196514,-21405489,7047412 },
  { 20093277,9920966,-11138194,-5343857,13161587,12044805,-32856851,4124601,-32343828,-10257566 },
 },
 {
  { -20788824,14084654,-13531713,7842147,19119038,-13822605,4752377,-8714640,-21679658,2288038 },
  { -26819236,-3283715,29965059,3039786,-14473765,2540457,29457502,14625692,-24819617,12570232 },
  { -1063558,-11551823,16920318,12494842,1278292,-5869109,-21159943,-3498680,-11974704,4724943 },
 },
 {
  { 17960970,-11775534,-4140968,-9702530,-8876562,-1410617,-12907383,-8659932,-29576300,1903856 },
  { 23134274,-14279132,-10681997,-1611936,20684485,15770816,-12989750,3190296,26955097,14109738 },
  { 15308788,5320727,-30113809,-14318877,22902008,7767164,29425325,-11277562,31960942,11934971 },
 },
 {
  { -27395711,8435796,4109644,12222639,-24627868,14818669,20638173,4875028,10491392,1379718 },
  { -13159415,9197841,3875503,-8936108,-1383712,-5879801,33518459,16176658,21432314,12180697 },
  { -11787308,11500838,13787581,-13832590,-22430679,10140205,1465425,12689540,-10301319,-13872883 },
 },
},
{
 {
  { 5414091,-15386041,-21007664,9643570,12834970,1186149,-2622916,-1342231,26128231,6032912 },
  { -26337395,-13766162,32496025,-13653919,17847801,-12669156,3604025,8316894,-25875034,-10437358 },
  { 3296484,6223048,24680646,-12246460,-23052020,5903205,-8862297,-4639164,12376617,3188849 },
 },
 {
  { 29190488,-14659046,27549113,-1183516,3520066,-10697301,32049515,-7309113,-16109234,-9852307 },
  { -14744486,-9309156,735818,-598978,-20407687,-5057904,25246078,-15795669,18640741,-960977 },
  { -6928835,-16430795,10361374,5642961,4910474,12345252,-31638386,-494430,10530747,1053335 },
 },
 {
  { -29265967,-14186805,-13538216,-12117373,-19457059,-10655384,-31462369,-2948985,24018831,15026644 },
  { -22592535,-3145277,-2289276,5953843,-13440189,9425631,25310643,13003497,-2314791,-15145616 },
  { -27419985,-603321,-8043984,-1669117,-26092265,13987819,-27297622,187899,-23166419,-2531735 },
 },
 {
  { -21744398,-13810475,1844840,5021428,-10434399,-15911473,9716667,16266922,-5070217,726099 },
  { 29370922,-6053998,7334071,-15342259,9385287,2247707,-13661962,-4839461,30007388,-15823341 },
  { -936379,16086691,23751945,-543318,-1167538,-5189036,9137109,730663,9835848,4555336 },
 },
 {
  { -23376435,1410446,-22253753,-12899614,30867635,15826977,17693930,544696,-11985298,12422646 },
  { 31117226,-12215734,-13502838,6561947,-9876867,-12757670,-5118685,-4096706,29120153,13924425 },
  { -17400879,-14233209,19675799,-2734756,-11006962,-5858820,-9383939,-11317700,7240931,-237388 },
 },
 {
  { -31361739,-11346780,-15007447,-5856218,-22453340,-12152771,1222336,4389483,3293637,-15551743 },
  { -16684801,-14444245,11038544,11054958,-13801175,-3338533,-24319580,7733547,12796905,-6335822 },
  { -8759414,-10817836,-25418864,10783769,-30615557,-9746811,-28253339,3647836,3222231,-11160462 },
 },
 {
  { 18606113,1693100,-25448386,-15170272,4112353,10045021,23603893,-2048234,-7550776,2484985 },
  { 9255317,-3131197,-12156162,-1004256,13098013,-9214866,16377220,-2102812,-19802075,-3034702 },
  { -22729289,7496160,-5742199,11329249,19991973,-3347502,-31718148,9936966,-30097688,-10618797 },
 },
 {
  { 21878590,-5001297,4338336,13643897,-3036865,13160960,19708896,5415497,-7360503,-4109293 },
  { 27736861,10103576,12500508,8502413,-3413016,-9633558,10436918,-1550276,-23659143,-8132100 },
  { 19492550,-12104365,-29681976,-852630,-3208171,12403437,30066266,8367329,13243957,8709688 },
 },
},
{
 {
  { 12015105,2801261,28198131,10151021,24818120,-4743133,-11194191,-5645734,5150968,7274186 },
  { 2831366,-12492146,1478975,6122054,23825128,-12733586,31097299,6083058,31021603,-9793610 },
  { -2529932,-2229646,445613,10720828,-13849527,-11505937,-23507731,16354465,15067285,-14147707 },
 },
 {
  { 7840942,14037873,-33364863,15934016,-728213,-3642706,21403988,1057586,-19379462,-12403220 },
  { 915865,-16469274,15608285,-8789130,-24357026,6060030,-17371319,8410997,-7220461,16527025 },
  { 32922597,-556987,20336074,-16184568,10903705,-5384487,16957574,52992,23834301,6588044 },
 },
 {
  { 32752030,11232950,3381995,-8714866,22652988,-10744103,17159699,16689107,-20314580,-1305992 },
  { -4689649,9166776,-25710296,-10847306,11576752,12733943,7924251,-2752281,1976123,-7249027 },
  { 21251222,16309901,-2983015,-6783122,30810597,12967303,156041,-3371252,12331345,-8237197 },
 },
 {
  { 8651614,-4477032,-16085636,-4996994,13002507,2950805,29054427,-5106970,10008136,-4667901 },
  { 31486080,15114593,-14261250,12951354,14369431,-7387845,16347321,-13662089,8684155,-10532952 },
  { 19443825,11385320,24468943,-9659068,-23919258,2187569,-26263207,-6086921,31316348,14219878 },
 },
 {
  { -28594490,1193785,32245219,11392485,31092169,15722801,27146014,6992409,29126555,9207390 },
  { 32382935,1110093,18477781,11028262,-27411763,-7548111,-4980517,10843782,-7957600,-14435730 },
  { 2814918,7836403,27519878,-7868156,-20894015,-11553689,-21494559,8550130,28346258,1994730 },
 },
 {
  { -19578299,8085545,-14000519,-3948622,2785838,-16231307,-19516951,7174894,22628102,8115180 },
  { -30405132,955511,-11133838,-15078069,-32447087,-13278079,-25651578,3317160,-9943017,930272 },
  { -15303681,-6833769,28856490,1357446,23421993,1057177,24091212,-1388970,-22765376,-10650715 },
 },
 {
  { -22751231,-5303997,-12907607,-12768866,-15811511,-7797053,-14839018,-16554220,-1867018,8398970 },
  { -31969310,2106403,-4736360,1362501,12813763,16200670,22981545,-6291273,18009408,-15772772 },
  { -17220923,-9545221,-27784654,14166835,29815394,7444469,29551787,-3727419,19288549,1325865 },
 },
 {
  { 15100157,-15835752,-23923978,-1005098,-26450192,15509408,12376730,-3479146,33166107,-8042750 },
  { 20909231,13023121,-9209752,16251778,-5778415,-8094914,12412151,10018715,2213263,-13878373 },
  { 32529814,-11074689,30361439,-16689753,-9135940,1513226,22922121,6382134,-5766928,8371348 },
 },
},
{
 {
  { 9923462,11271500,12616794,3544722,-29998368,-1721626,12891687,-8193132,-26442943,10486144 },
  { -22597207,-7012665,8587003,-8257861,4084309,-12970062,361726,2610596,-23921530,-11455195 },
  { 5408411,-1136691,-4969122,10561668,24145918,14240566,31319731,-4235541,19985175,-3436086 },
 },
 {
  { -13994457,16616821,14549246,3341099,32155958,13648976,-17577068,8849297,65030,8370684 },
  { -8320926,-12049626,31204563,5839400,-20627288,-1057277,-19442942,6922164,12743482,-9800518 },
  { -2361371,12678785,28815050,4759974,-23893047,4884717,23783145,11038569,18800704,255233 },
 },
 {
  { -5269658,-1773886,13957886,7990715,23132995,728773,13393847,9066957,19258688,-14753793 },
  { -2936654,-10827535,-10432089,14516793,-3640786,4372541,-31934921,2209390,-1524053,2055794 },
  { 580882,16705327,5468415,-2683018,-30926419,-14696000,-7203346,-8994389,-30021019,7394435 },
 },
 {
  { 23838809,1822728,-15738443,15242727,8318092,-3733104,-21672180,-3492205,-4821741,14799921 },
  { 13345610,9759151,3371034,-16137791,16353039,8577942,31129804,13496856,-9056018,7402518 },
  { 2286874,-4435931,-20042458,-2008336,-13696227,5038122,11006906,-15760352,8205061,1607563 },
 },
 {
  { 14414086,-8002132,3331830,-3208217,22249151,-5594188,18364661,-2906958,30019587,-9029278 },
  { -27688051,1585953,-10775053,931069,-29120221,-11002319,-14410829,12029093,9944378,8024 },
  { 4368715,-3709630,29874200,-15022983,-20230386,-11410704,-16114594,-999085,-8142388,5640030 },
 },
 {
  { 10299610,13746483,11661824,16234854,7630238,5998374,9809887,-16694564,15219798,-14327783 },
  { 27425505,-5719081,3055006,10660664,23458024,595578,-15398605,-1173195,-18342183,9742717 },
  { 6744077,2427284,26042789,2720740,-847906,1118974,32324614,7406442,12420155,1994844 },
 },
 {
  { 14012521,-5024720,-18384453,-9578469,-26485342,-3936439,-13033478,-10909803,24319929,-6446333 },
  { 16412690,-4507367,10772641,15929391,-17068788,-4658621,10555945,-10484049,-30102368,-4739048 },
  { 22397382,-7767684,-9293161,-12792868,17166287,-9755136,-27333065,6199366,21880021,-12250760 },
 },
 {
  { -4283307,5368523,-31117018,8163389,-30323063,3209128,16557151,8890729,8840445,4957760 },
  { -15447727,709327,-6919446,-10870178,-29777922,6522332,-21720181,12130072,-14796503,5005757 },
  { -2114751,-14308128,23019042,15765735,-25269683,6002752,10183197,-13239326,-16395286,-2176112 },
 },
},
{
 {
  { -19025756,1632005,13466291,-7995100,-23640451,16573537,-32013908,-3057104,22208662,2000468 },
  { 3065073,-1412761,-25598674,-361432,-17683065,-5703415,-8164212,11248527,-3691214,-7414184 },
  { 10379208,-6045554,8877319,1473647,-29291284,-12507580,16690915,2553332,-3132688,16400289 },
 },
 {
  { 15716668,1254266,-18472690,7446274,-8448918,6344164,-22097271,-7285580,26894937,9132066 },
  { 24158887,12938817,11085297,-8177598,-28063478,-4457083,-30576463,64452,-6817084,-2692882 },
  { 13488534,7794716,22236231,5989356,25426474,-12578208,2350710,-3418511,-4688006,2364226 },
 },
 {
  { 16335052,9132434,25640582,6678888,1725628,8517937,-11807024,-11697457,15445875,-7798101 },
  { 29004207,-7867081,28661402,-640412,-12794003,-7943086,31863255,-4135540,-278050,-15759279 },
  { -6122061,-14866665,-28614905,14569919,-10857999,-3591829,10343412,-6976290,-29828287,-10815811 },
 },
 {
  { 27081650,3463984,14099042,-4517604,1616303,-6205604,29542636,15372179,17293797,960709 },
  { 20263915,11434237,-5765435,11236810,13505955,-10857102,-16111345,6493122,-19384511,7639714 },
  { -2830798,-14839232,25403038,-8215196,-8317012,-16173699,18006287,-16043750,29994677,-15808121 },
 },
 {
  { 9769828,5202651,-24157398,-13631392,-28051003,-11561624,-24613141,-13860782,-31184575,709464 },
  { 12286395,13076066,-21775189,-1176622,-25003198,4057652,-32018128,-8890874,16102007,13205847 },
  { 13733362,5599946,10557076,3195751,-5557991,8536970,-25540170,8525972,10151379,10394400 },
 },
 {
  { 4024660,-16137551,22436262,12276534,-9099015,-2686099,19698229,11743039,-33302334,8934414 },
  { -15879800,-4525240,-8580747,-2934061,14634845,-698278,-9449077,3137094,-11536886,11721158 },
  { 17555939,-5013938,8268606,2331751,-22738815,9761013,9319229,8835153,-9205489,-1280045 },
 },
 {
  { -461409,-7830014,20614118,16688288,-7514766,-4807119,22300304,505429,6108462,-6183415 },
  { -5070281,12367917,-30663534,3234473,32617080,-8422642,29880583,-13483331,-26898490,-7867459 },
  { -31975283,5726539,26934134,10237677,-3173717,-605053,24199304,3795095,7592688,-14992079 },
 },
 {
  { 21594432,-14964228,17466408,-4077222,32537084,2739898,6407723,12018833,-28256052,4298412 },
  { -20650503,-11961496,-27236275,570498,3767144,-1717540,13891942,-1569194,13717174,10805743 },
  { -14676630,-15644296,15287174,11927123,24177847,-8175568,-796431,14860609,-26938930,-5863836 },
 },
},
{
 {
  { 12962541,5311799,-10060768,11658280,18855286,-7954201,13286263,-12808704,-4381056,9882022 },
  { 18512079,11319350,-20123124,15090309,18818594,5271736,-22727904,3666879,-23967430,-3299429 },
  { -6789020,-3146043,16192429,13241070,15898607,-14206114,-10084880,-6661110,-2403099,5276065 },
 },
 {
  { 30169808,-5317648,26306206,-11750859,27814964,7069267,7152851,3684982,1449224,13082861 },
  { 10342826,3098505,2119311,193222,25702612,12233820,23697382,15056736,-21016438,-8202000 },
  { -33150110,3261608,22745853,7948688,19370557,-15177665,-26171976,6482814,-10300080,-11060101 },
 },
 {
  { 32869458,-5408545,25609743,15678670,-10687769,-15471071,26112421,2521008,-22664288,6904815 },
  { 29506923,4457497,3377935,-9796444,-30510046,12935080,1561737,3841096,-29003639,-6657642 },
  { 10340844,-6630377,-18656632,-2278430,12621151,-13339055,30878497,-11824370,-25584551,5181966 },
 },
 {
  { 25940115,-12658025,17324188,-10307374,-8671468,15029094,24396252,-16450922,-2322852,-12388574 },
  { -21765684,9916823,-1300409,4079498,-1028346,11909559,1782390,12641087,20603771,-6561742 },
  { -18882287,-11673380,24849422,11501709,13161720,-4768874,1925523,11914390,4662781,7820689 },
 },
 {
  { 12241050,-425982,8132691,9393934,32846760,-1599620,29749456,12172924,16136752,15264020 },
  { -10349955,-14680563,-8211979,2330220,-17662549,-14545780,10658213,6671822,19012087,3772772 },
  { 3753511,-3421066,10617074,2028709,14841030,-6721664,28718732,-15762884,20527771,12988982 },
 },
 {
  { -14822485,-5797269,-3707987,12689773,-898983,-10914866,-24183046,-10564943,3299665,-12424953 },
  { -16777703,-15253301,-9642417,4978983,3308785,8755439,6943197,6461331,-25583147,8991218 },
  { -17226263,1816362,-1673288,-6086439,31783888,-8175991,-32948145,7417950,-30242287,1507265 },
 },
 {
  { 29692663,6829891,-10498800,4334896,20945975,-11906496,-28887608,8209391,14606362,-10647073 },
  { -3481570,8707081,32188102,5672294,22096700,1711240,-33020695,9761487,4170404,-2085325 },
  { -11587470,14855945,-4127778,-1531857,-26649089,15084046,22186522,16002000,-14276837,-8400798 },
 },
 {
  { -4811456,13761029,-31703877,-2483919,-3312471,7869047,-7113572,-9620092,13240845,10965870 },
  { -7742563,-8256762,-14768334,-13656260,-23232383,12387166,4498947,14147411,29514390,4302863 },
  { -13413405,-12407859,20757302,-13801832,14785143,8976368,-5061276,-2144373,17846988,-13971927 },
 },
},
{
 {
  { -2244452,-754728,-4597030,-1066309,-6247172,1455299,-21647728,-9214789,-5222701,12650267 },
  { -9906797,-16070310,21134160,12198166,-27064575,708126,387813,13770293,-19134326,10958663 },
  { 22470984,12369526,23446014,-5441109,-21520802,-9698723,-11772496,-11574455,-25083830,4271862 },
 },
 {
  { -25169565,-10053642,-19909332,15361595,-5984358,2159192,75375,-4278529,-32526221,8469673 },
  { 15854970,4148314,-8893890,7259002,11666551,13824734,-30531198,2697372,24154791,-9460943 },
  { 15446137,-15806644,29759747,14019369,30811221,-9610191,-31582008,12840104,24913809,9815020 },
 },
 {
  { -4709286,-5614269,-31841498,-12288893,-14443537,10799414,-9103676,13438769,18735128,9466238 },
  { 11933045,9281483,5081055,-5183824,-2628162,-4905629,-7727821,-10896103,-22728655,16199064 },
  { 14576810,379472,-26786533,-8317236,-29426508,-10812974,-102766,1876699,30801119,2164795 },
 },
 {
  { 15995086,3199873,13672555,13712240,-19378835,-4647646,-13081610,-15496269,-13492807,1268052 },
  { -10290614,-3659039,-3286592,10948818,23037027,3794475,-3470338,-12600221,-17055369,3565904 },
  { 29210088,-9419337,-5919792,-4952785,10834811,-13327726,-16512102,-10820713,-27162222,-14030531 },
 },
 {
  { -13161890,15508588,16663704,-8156150,-28349942,9019123,-29183421,-3769423,2244111,-14001979 },
  { -5152875,-3800936,-9306475,-6071583,16243069,14684434,-25673088,-16180800,13491506,4641841 },
  { 10813417,643330,-19188515,-728916,30292062,-16600078,27548447,-7721242,14476989,-12767431 },
 },
 {
  { 10292079,9984945,6481436,8279905,-7251514,7032743,27282937,-1644259,-27912810,12651324 },
  { -31185513,-813383,22271204,11835308,10201545,15351028,17099662,3988035,21721536,-3148940 },
  { 10202177,-6545839,-31373232,-9574638,-32150642,-8119683,-12906320,3852694,13216206,14842320 },
 },
 {
  { -15815640,-10601066,-6538952,-7258995,-6984659,-6581778,-31500847,13765824,-27434397,9900184 },
  { 14465505,-13833331,-32133984,-14738873,-27443187,12990492,33046193,15796406,-7051866,-8040114 },
  { 30924417,-8279620,6359016,-12816335,16508377,9071735,-25488601,15413635,9524356,-7018878 },
 },
 {
  { 12274201,-13175547,32627641,-1785326,6736625,13267305,5237659,-5109483,15663516,4035784 },
  { -2951309,8903985,17349946,601635,-16432815,-4612556,-13732739,-15889334,-22258478,4659091 },
  { -16916263,-4952973,-30393711,-15158821,20774812,15897498,5736189,15026997,-2178256,-13455585 },
 },
},
{
 {
  { -8858980,-2219056,28571666,-10155518,-474467,-10105698,-3801496,278095,23440562,-290208 },
  { 10226241,-5928702,15139956,120818,-14867693,5218603,32937275,11551483,-16571960,-7442864 },
  { 17932739,-12437276,-24039557,10749060,11316803,7535897,22503767,5561594,-3646624,3898661 },
 },
 {
  { 7749907,-969567,-16339731,-16464,-25018111,15122143,-1573531,7152530,21831162,1245233 },
  { 26958459,-14658026,4314586,8346991,-5677764,11960072,-32589295,-620035,-30402091,-16716212 },
  { -12165896,9166947,33491384,13673479,29787085,13096535,6280834,14587357,-22338025,13987525 },
 },
 {
  { -24349909,7778775,21116000,15572597,-4833266,-5357778,-4300898,-5124639,-7469781,-2858068 },
  { 9681908,-6737123,-31951644,13591838,-6883821,386950,31622781,6439245,-14581012,4091397 },
  { -8426427,1470727,-28109679,-1596990,3978627,-5123623,-19622683,12092163,29077877,-14741988 },
 },
 {
  { 5269168,-6859726,-13230211,-8020715,25932563,1763552,-5606110,-5505881,-20017847,2357889 },
  { 32264008,-15407652,-5387735,-1160093,-2091322,-3946900,23104804,-12869908,5727338,189038 },
  { 14609123,-8954470,-6000566,-16622781,-14577387,-7743898,-26745169,10942115,-25888931,-14884697 },
 },
 {
  { 20513500,5557931,-15604613,7829531,26413943,-2019404,-21378968,7471781,13913677,-5137875 },
  { -25574376,11967826,29233242,12948236,-6754465,4713227,-8940970,14059180,12878652,8511905 },
  { -25656801,3393631,-2955415,-7075526,-2250709,9366908,-30223418,6812974,5568676,-3127656 },
 },
 {
  { 11630004,12144454,2116339,13606037,27378885,15676917,-17408753,-13504373,-14395196,8070818 },
  { 27117696,-10007378,-31282771,-5570088,1127282,12772488,-29845906,10483306,-11552749,-1028714 },
  { 10637467,-5688064,5674781,1072708,-26343588,-6982302,-1683975,9177853,-27493162,15431203 },
 },
 {
  { 20525145,10892566,-12742472,12779443,-29493034,16150075,-28240519,14943142,-15056790,-7935931 },
  { -30024462,5626926,-551567,-9981087,753598,11981191,25244767,-3239766,-3356550,9594024 },
  { -23752644,2636870,-5163910,-10103818,585134,7877383,11345683,-6492290,13352335,-10977084 },
 },
 {
  { -1931799,-5407458,3304649,-12884869,17015806,-4877091,-29783850,-7752482,-13215537,-319204 },
  { 20239939,6607058,6203985,3483793,-18386976,-779229,-20723742,15077870,-22750759,14523817 },
  { 27406042,-6041657,27423596,-4497394,4996214,10002360,-28842031,-4545494,-30172742,-4805667 },
 },
},
{
 {
  { 11374242,12660715,17861383,-12540833,10935568,1099227,-13886076,-9091740,-27727044,11358504 },
  { -12730809,10311867,1510375,10778093,-2119455,-9145702,32676003,11149336,-26123651,4985768 },
  { -19096303,341147,-6197485,-239033,15756973,-8796662,-983043,13794114,-19414307,-15621255 },
 },
 {
  { 6490081,11940286,25495923,-7726360,8668373,-8751316,3367603,6970005,-1691065,-9004790 },
  { 1656497,13457317,15370807,6364910,13605745,8362338,-19174622,-5475723,-16796596,-5031438 },
  { -22273315,-13524424,-64685,-4334223,-18605636,-10921968,-20571065,-7007978,-99853,-10237333 },
 },
 {
  { 17747465,10039260,19368299,-4050591,-20630635,-16041286,31992683,-15857976,-29260363,-5511971 },
  { 31932027,-4986141,-19612382,16366580,22023614,88450,11371999,-3744247,4882242,-10626905 },
  { 29796507,37186,19818052,10115756,-11829032,3352736,18551198,3272828,-5190932,-4162409 },
 },
 {
  { 12501286,4044383,-8612957,-13392385,-32430052,5136599,-19230378,-3529697,330070,-3659409 },
  { 6384877,2899513,17807477,7663917,-2358888,12363165,25366522,-8573892,-271295,12071499 },
  { -8365515,-4042521,25133448,-4517355,-6211027,2265927,-32769618,1936675,-5159697,3829363 },
 },
 {
  { 28425966,-5835433,-577090,-4697198,-14217555,6870930,7921550,-6567787,26333140,14267664 },
  { -11067219,11871231,27385719,-10559544,-4585914,-11189312,10004786,-8709488,-21761224,8930324 },
  { -21197785,-16396035,25654216,-1725397,12282012,11008919,1541940,4757911,-26491501,-16408940 },
 },
 {
  { 13537262,-7759490,-20604840,10961927,-5922820,-13218065,-13156584,6217254,-15943699,13814990 },
  { -17422573,15157790,18705543,29619,24409717,-260476,27361681,9257833,-1956526,-1776914 },
  { -25045300,-10191966,15366585,15166509,-13105086,8423556,-29171540,12361135,-18685978,4578290 },
 },
 {
  { 24579768,3711570,1342322,-11180126,-27005135,14124956,-22544529,14074919,21964432,8235257 },
  { -6528613,-2411497,9442966,-5925588,12025640,-1487420,-2981514,-1669206,13006806,2355433 },
  { -16304899,-13605259,-6632427,-5142349,16974359,-10911083,27202044,1719366,1141648,-12796236 },
 },
 {
  { -12863944,-13219986,-8318266,-11018091,-6810145,-4843894,13475066,-3133972,32674895,13715045 },
  { 11423335,-5468059,32344216,8962751,24989809,9241752,-13265253,16086212,-28740881,-15642093 },
  { -1409668,12530728,-6368726,10847387,19531186,-14132160,-11709148,7791794,-27245943,4383347 },
 },
},
{
 {
  { -28970898,5271447,-1266009,-9736989,-12455236,16732599,-4862407,-4906449,27193557,6245191 },
  { -15193956,5362278,-1783893,2695834,4960227,12840725,23061898,3260492,22510453,8577507 },
  { -12632451,11257346,-32692994,13548177,-721004,10879011,31168030,13952092,-29571492,-3635906 },
 },
 {
  { 3877321,-9572739,32416692,5405324,-11004407,-13656635,3759769,11935320,5611860,8164018 },
  { -16275802,14667797,15906460,12155291,-22111149,-9039718,32003002,-8832289,5773085,-8422109 },
  { -23788118,-8254300,1950875,8937633,18686727,16459170,-905725,12376320,31632953,190926 },
 },
 {
  { -24593607,-16138885,-8423991,13378746,14162407,6901328,-8288749,4508564,-25341555,-3627528 },
  { 8884438,-5884009,6023974,10104341,-6881569,-4941533,18722941,-14786005,-1672488,827625 },
  { -32720583,-16289296,-32503547,7101210,13354605,2659080,-1800575,-14108036,-24878478,1541286 },
 },
 {
  { 2901347,-1117687,3880376,-10059388,-17620940,-3612781,-21802117,-3567481,20456845,-1885033 },
  { 27019610,12299467,-13658288,-1603234,-12861660,-4861471,-19540150,-5016058,29439641,15138866 },
  { 21536104,-6626420,-32447818,-10690208,-22408077,5175814,-5420040,-16361163,7779328,109896 },
 },
 {
  { 30279744,14648750,-8044871,6425558,13639621,-743509,28698390,12180118,23177719,-554075 },
  { 26572847,3405927,-31701700,12890905,-19265668,5335866,-6493768,2378492,4439158,-13279347 },
  { -22716706,3489070,-9225266,-332753,18875722,-1140095,14819434,-12731527,-17717757,-5461437 },
 },
 {
  { -5056483,16566551,15953661,3767752,-10436499,15627060,-820954,2177225,8550082,-15114165 },
  { -18473302,16596775,-381660,15663611,22860960,15585581,-27844109,-3582739,-23260460,-8428588 },
  { -32480551,15707275,-8205912,-5652081,29464558,2713815,-22725137,15860482,-21902570,1494193 },
 },
 {
  { -19562091,-14087393,-25583872,-9299552,13127842,759709,21923482,16529112,8742704,12967017 },
  { -28464899,1553205,32536856,-10473729,-24691605,-406174,-8914625,-2933896,-29903758,15553883 },
  { 21877909,3230008,9881174,10539357,-4797115,2841332,11543572,14513274,19375923,-12647961 },
 },
 {
  { 8832269,-14495485,13253511,5137575,5037871,4078777,24880818,-6222716,2862653,9455043 },
  { 29306751,5123106,20245049,-14149889,9592566,8447059,-2077124,-2990080,15511449,4789663 },
  { -20679756,7004547,8824831,-9434977,-4045704,-3750736,-5754762,108893,23513200,16652362 },
 },
},
{
 {
  { -33256173,4144782,-4476029,-6579123,10770039,-7155542,-6650416,-12936300,-18319198,10212860 },
  { 2756081,8598110,7383731,-6859892,22312759,-1105012,21179801,2600940,-9988298,-12506466 },
  { -24645692,13317462,-30449259,-15653928,21365574,-10869657,11344424,864440,-2499677,-16710063 },
 },
 {
  { -26432803,6148329,-17184412,-14474154,18782929,-275997,-22561534,211300,2719757,4940997 },
  { -1323882,3911313,-6948744,14759765,-30027150,7851207,21690126,8518463,26699843,5276295 },
  { -13149873,-6429067,9396249,365013,24703301,-10488939,1321586,149635,-15452774,7159369 },
 },
 {
  { 9987780,-3404759,17507962,9505530,9731535,-2165514,22356009,8312176,22477218,-8403385 },
  { 18155857,-16504990,19744716,9006923,15154154,-10538976,24256460,-4864995,-22548173,9334109 },
  { 2986088,-4911893,10776628,-3473844,10620590,-7083203,-21413845,14253545,-22587149,536906 },
 },
 {
  { 4377756,8115836,24567078,15495314,11625074,13064599,7390551,10589625,10838060,-15420424 },
  { -19342404,867880,9277171,-3218459,-14431572,-1986443,19295826,-15796950,6378260,699185 },
  { 7895026,4057113,-7081772,-13077756,-17886831,-323126,-716039,15693155,-5045064,-13373962 },
 },
 {
  { -7737563,-5869402,-14566319,-7406919,11385654,13201616,31730678,-10962840,-3918636,-9669325 },
  { 10188286,-15770834,-7336361,13427543,22223443,14896287,30743455,7116568,-21786507,5427593 },
  { 696102,13206899,27047647,-10632082,15285305,-9853179,10798490,-4578720,19236243,12477404 },
 },
 {
  { -11229439,11243796,-17054270,-8040865,-788228,-8167967,-3897669,11180504,-23169516,7733644 },
  { 17800790,-14036179,-27000429,-11766671,23887827,3149671,23466177,-10538171,10322027,15313801 },
  { 26246234,11968874,32263343,-5468728,6830755,-13323031,-15794704,-101982,-24449242,10890804 },
 },
 {
  { -31365647,10271363,-12660625,-6267268,16690207,-13062544,-14982212,16484931,25180797,-5334884 },
  { -586574,10376444,-32586414,-11286356,19801893,10997610,2276632,9482883,316878,13820577 },
  { -9882808,-4510367,-2115506,16457136,-11100081,11674996,30756178,-7515054,30696930,-3712849 },
 },
 {
  { 32988917,-9603412,12499366,7910787,-10617257,-11931514,-7342816,-9985397,-32349517,7392473 },
  { -8855661,15927861,9866406,-3649411,-2396914,-16655781,-30409476,-9134995,25112947,-2926644 },
  { -2504044,-436966,25621774,-5678772,15085042,-5479877,-24884878,-13526194,5537438,-13914319 },
 },
},
{
 {
  { -11225584,2320285,-9584280,10149187,-33444663,5808648,-14876251,-1729667,31234590,6090599 },
  { -9633316,116426,26083934,2897444,-6364437,-2688086,609721,15878753,-6970405,-9034768 },
  { -27757857,247744,-15194774,-9002551,23288161,-10011936,-23869595,6503646,20650474,1804084 },
 },
 {
  { -27589786,15456424,8972517,8469608,15640622,4439847,3121995,-10329713,27842616,-202328 },
  { -15306973,2839644,22530074,10026331,4602058,5048462,28248656,5031932,-11375082,12714369 },
  { 20807691,-7270825,29286141,11421711,-27876523,-13868230,-21227475,1035546,-19733229,12796920 },
 },
 {
  { 12076899,-14301286,-8785001,-11848922,-25012791,16400684,-17591495,-12899438,3480665,-15182815 },
  { -32361549,5457597,28548107,7833186,7303070,-11953545,-24363064,-15921875,-33374054,2771025 },
  { -21389266,421932,26597266,6860826,22486084,-6737172,-17137485,-4210226,-24552282,15673397 },
 },
 {
  { -20184622,2338216,19788685,-9620956,-4001265,-8740893,-20271184,4733254,3727144,-12934448 },
  { 6120119,814863,-11794402,-622716,6812205,-15747771,2019594,7975683,31123697,-10958981 },
  { 30069250,-11435332,30434654,2958439,18399564,-976289,12296869,9204260,-16432438,9648165 },
 },
 {
  { 32705432,-1550977,30705658,7451065,-11805606,9631813,3305266,5248604,-26008332,-11377501 },
  { 17219865,2375039,-31570947,-5575615,-19459679,9219903,294711,15298639,2662509,-16297073 },
  { -1172927,-7558695,-4366770,-4287744,-21346413,-8434326,32087529,-1222777,32247248,-14389861 },
 },
 {
  { 14312628,1221556,17395390,-8700143,-4945741,-8684635,-28197744,-9637817,-16027623,-13378845 },
  { -1428825,-9678990,-9235681,6549687,-7383069,-468664,23046502,9803137,17597934,2346211 },
  { 18510800,15337574,26171504,981392,-22241552,7827556,-23491134,-11323352,3059833,-11782870 },
 },
 {
  { 10141598,6082907,17829293,-1947643,9830092,13613136,-25556636,-5544586,-33502212,3592096 },
  { 33114168,-15889352,-26525686,-13343397,33076705,8716171,1151462,1521897,-982665,-6837803 },
  { -32939165,-4255815,23947181,-324178,-33072974,-12305637,-16637686,3891704,26353178,693168 },
 },
 {
  { 30374239,1595580,-16884039,13186931,4600344,406904,9585294,-400668,31375464,14369965 },
  { -14370654,-7772529,1510301,6434173,-18784789,-6262728,32732230,-13108839,17901441,16011505 },
  { 18171223,-11934626,-12500402,15197122,-11038147,-15230035,-19172240,-16046376,8764035,12309598 },
 },
},
{
 {
  { 5975908,-5243188,-19459362,-9681747,-11541277,14015782,-23665757,1228319,17544096,-10593782 },
  { 5811932,-1715293,3442887,-2269310,-18367348,-8359541,-18044043,-15410127,-5565381,12348900 },
  { -31399660,11407555,25755363,6891399,-3256938,14872274,-24849353,8141295,-10632534,-585479 },
 },
 {
  { -12675304,694026,-5076145,13300344,14015258,-14451394,-9698672,-11329050,30944593,1130208 },
  { 8247766,-6710942,-26562381,-7709309,-14401939,-14648910,4652152,2488540,23550156,-271232 },
  { 17294316,-3788438,7026748,15626851,22990044,113481,2267737,-5908146,-408818,-137719 },
 },
 {
  { 16091085,-16253926,18599252,7340678,2137637,-1221657,-3364161,14550936,3260525,-7166271 },
  { -4910104,-13332887,18550887,10864893,-16459325,-7291596,-23028869,-13204905,-12748722,2701326 },
  { -8574695,16099415,4629974,-16340524,-20786213,-6005432,-10018363,9276971,11329923,1862132 },
 },
 {
  { 14763076,-15903608,-30918270,3689867,3511892,10313526,-21951088,12219231,-9037963,-940300 },
  { 8894987,-3446094,6150753,3013931,301220,15693451,-31981216,-2909717,-15438168,11595570 },
  { 15214962,3537601,-26238722,-14058872,4418657,-15230761,13947276,10730794,-13489462,-4363670 },
 },
 {
  { -2538306,7682793,32759013,263109,-29984731,-7955452,-22332124,-10188635,977108,699994 },
  { -12466472,4195084,-9211532,550904,-15565337,12917920,19118110,-439841,-30534533,-14337913 },
  { 31788461,-14507657,4799989,7372237,8808585,-14747943,9408237,-10051775,12493932,-5409317 },
 },
 {
  { -25680606,5260744,-19235809,-6284470,-3695942,16566087,27218280,2607121,29375955,6024730 },
  { 842132,-2794693,-4763381,-8722815,26332018,-12405641,11831880,6985184,-9940361,2854096 },
  { -4847262,-7969331,2516242,-5847713,9695691,-7221186,16512645,960770,12121869,16648078 },
 },
 {
  { -15218652,14667096,-13336229,2013717,30598287,-464137,-31504922,-7882064,20237806,2838411 },
  { -19288047,4453152,15298546,-16178388,22115043,-15972604,12544294,-13470457,1068881,-12499905 },
  { -9558883,-16518835,33238498,13506958,30505848,-1114596,-8486907,-2630053,12521378,4845654 },
 },
 {
  { -28198521,10744108,-2958380,10199664,7759311,-13088600,3409348,-873400,-6482306,-12885870 },
  { -23561822,6230156,-20382013,10655314,-24040585,-11621172,10477734,-1240216,-3113227,13974498 },
  { 12966261,15550616,-32038948,-1615346,21025980,-629444,5642325,7188737,18895762,12629579 },
 },
},
{
 {
  { 14741879,-14946887,22177208,-11721237,1279741,8058600,11758140,789443,32195181,3895677 },
  { 10758205,15755439,-4509950,9243698,-4879422,6879879,-2204575,-3566119,-8982069,4429647 },
  { -2453894,15725973,-20436342,-10410672,-5803908,-11040220,-7135870,-11642895,18047436,-15281743 },
 },
 {
  { -25173001,-11307165,29759956,11776784,-22262383,-15820455,10993114,-12850837,-17620701,-9408468 },
  { 21987233,700364,-24505048,14972008,-7774265,-5718395,32155026,2581431,-29958985,8773375 },
  { -25568350,454463,-13211935,16126715,25240068,8594567,20656846,12017935,-7874389,-13920155 },
 },
 {
  { 6028182,6263078,-31011806,-11301710,-818919,2461772,-31841174,-5468042,-1721788,-2776725 },
  { -12278994,16624277,987579,-5922598,32908203,1248608,7719845,-4166698,28408820,6816612 },
  { -10358094,-8237829,19549651,-12169222,22082623,16147817,20613181,13982702,-10339570,5067943 },
 },
 {
  { -30505967,-3821767,12074681,13582412,-19877972,2443951,-19719286,12746132,5331210,-10105944 },
  { 30528811,3601899,-1957090,4619785,-27361822,-15436388,24180793,-12570394,27679908,-1648928 },
  { 9402404,-13957065,32834043,10838634,-26580150,-13237195,26653274,-8685565,22611444,-12715406 },
 },
 {
  { 22190590,1118029,22736441,15130463,-30460692,-5991321,19189625,-4648942,4854859,6622139 },
  { -8310738,-2953450,-8262579,-3388049,-10401731,-271929,13424426,-3567227,26404409,13001963 },
  { -31241838,-15415700,-2994250,8939346,11562230,-12840670,-26064365,-11621720,-15405155,11020693 },
 },
 {
  { 1866042,-7949489,-7898649,-10301010,12483315,13477547,3175636,-12424163,28761762,1406734 },
  { -448555,-1777666,13018551,3194501,-9580420,-11161737,24760585,-4347088,25577411,-13378680 },
  { -24290378,4759345,-690653,-1852816,2066747,10693769,-29595790,9884936,-9368926,4745410 },
 },
 {
  { -9141284,6049714,-19531061,-4341411,-31260798,9944276,-15462008,-11311852,10931924,-11931931 },
  { -16561513,14112680,-8012645,4817318,-8040464,-11414606,-22853429,10856641,-20470770,13434654 },
  { 22759489,-10073434,-16766264,-1871422,13637442,-10168091,1765144,-12654326,28445307,-5364710 },
 },
 {
  { 29875063,12493613,2795536,-3786330,1710620,15181182,-10195717,-8788675,9074234,1167180 },
  { -26205683,11014233,-9842651,-2635485,-26908120,7532294,-18716888,-9535498,3843903,9367684 },
  { -10969595,-6403711,9591134,9582310,11349256,108879,16235123,8601684,-139197,4242895 },
 },
},
{
 {
  { 22092954,-13191123,-2042793,-11968512,32186753,-11517388,-6574341,2470660,-27417366,16625501 },
  { -11057722,3042016,13770083,-9257922,584236,-544855,-7770857,2602725,-27351616,14247413 },
  { 6314175,-10264892,-32772502,15957557,-10157730,168750,-8618807,14290061,27108877,-1180880 },
 },
 {
  { -8586597,-7170966,13241782,10960156,-32991015,-13794596,33547976,-11058889,-27148451,981874 },
  { 22833440,9293594,-32649448,-13618667,-9136966,14756819,-22928859,-13970780,-10479804,-16197962 },
  { -7768587,3326786,-28111797,10783824,19178761,14905060,22680049,13906969,-15933690,3797899 },
 },
 {
  { 21721356,-4212746,-12206123,9310182,-3882239,-13653110,23740224,-2709232,20491983,-8042152 },
  { 9209270,-15135055,-13256557,-6167798,-731016,15289673,25947805,15286587,30997318,-6703063 },
  { 7392032,16618386,23946583,-8039892,-13265164,-1533858,-14197445,-2321576,17649998,-250080 },
 },
 {
  { -9301088,-14193827,30609526,-3049543,-25175069,-1283752,-15241566,-9525724,-2233253,7662146 },
  { -17558673,1763594,-33114336,15908610,-30040870,-12174295,7335080,-8472199,-3174674,3440183 },
  { -19889700,-5977008,-24111293,-9688870,10799743,-16571957,40450,-4431835,4862400,1133 },
 },
 {
  { -32856209,-7873957,-5422389,14860950,-16319031,7956142,7258061,311861,-30594991,-7379421 },
  { -3773428,-1565936,28985340,7499440,24445838,9325937,29727763,16527196,18278453,15405622 },
  { -4381906,8508652,-19898366,-3674424,-5984453,15149970,-13313598,843523,-21875062,13626197 },
 },
 {
  { 2281448,-13487055,-10915418,-2609910,1879358,16164207,-10783882,3953792,13340839,15928663 },
  { 31727126,-7179855,-18437503,-8283652,2875793,-16390330,-25269894,-7014826,-23452306,5964753 },
  { 4100420,-5959452,-17179337,6017714,-18705837,12227141,-26684835,11344144,2538215,-7570755 },
 },
 {
  { -9433605,6123113,11159803,-2156608,30016280,14966241,-20474983,1485421,-629256,-15958862 },
  { -26804558,4260919,11851389,9658551,-32017107,16367492,-20205425,-13191288,11659922,-11115118 },
  { 26180396,10015009,-30844224,-8581293,5418197,9480663,2231568,-10170080,33100372,-1306171 },
 },
 {
  { 15121113,-5201871,-10389905,15427821,-27509937,-15992507,21670947,4486675,-5931810,-14466380 },
  { 16166486,-9483733,-11104130,6023908,-31926798,-1364923,2340060,-16254968,-10735770,-10039824 },
  { 28042865,-3557089,-12126526,12259706,-3717498,-6945899,6766453,-8689599,18036436,5803270 },
 },
},
{
 {
  { -817581,6763912,11803561,1585585,10958447,-2671165,23855391,4598332,-6159431,-14117438 },
  { -31031306,-14256194,17332029,-2383520,31312682,-5967183,696309,50292,-20095739,11763584 },
  { -594563,-2514283,-32234153,12643980,12650761,14811489,665117,-12613632,-19773211,-10713562 },
 },
 {
  { 30464590,-11262872,-4127476,-12734478,19835327,-7105613,-24396175,2075773,-17020157,992471 },
  { 18357185,-6994433,7766382,16342475,-29324918,411174,14578841,8080033,-11574335,-10601610 },
  { 19598397,10334610,12555054,2555664,18821899,-10339780,21873263,16014234,26224780,16452269 },
 },
 {
  { -30223925,5145196,5944548,16385966,3976735,2009897,-11377804,-7618186,-20533829,3698650 },
  { 14187449,3448569,-10636236,-10810935,-22663880,-3433596,7268410,-10890444,27394301,12015369 },
  { 19695761,16087646,28032085,12999827,6817792,11427614,20244189,-1312777,-13259127,-3402461 },
 },
 {
  { 30860103,12735208,-1888245,-4699734,-16974906,2256940,-8166013,12298312,-8550524,-10393462 },
  { -5719826,-11245325,-1910649,15569035,26642876,-7587760,-5789354,-15118654,-4976164,12651793 },
  { -2848395,9953421,11531313,-5282879,26895123,-12697089,-13118820,-16517902,9768698,-2533218 },
 },
 {
  { -24719459,1894651,-287698,-4704085,15348719,-8156530,32767513,12765450,4940095,10678226 },
  { 18860224,15980149,-18987240,-1562570,-26233012,-11071856,-7843882,13944024,-24372348,16582019 },
  { -15504260,4970268,-29893044,4175593,-20993212,-2199756,-11704054,15444560,-11003761,7989037 },
 },
 {
  { 31490452,5568061,-2412803,2182383,-32336847,4531686,-32078269,6200206,-19686113,-14800171 },
  { -17308668,-15879940,-31522777,-2831,-32887382,16375549,8680158,-16371713,28550068,-6857132 },
  { -28126887,-5688091,16837845,-1820458,-6850681,12700016,-30039981,4364038,1155602,5988841 },
 },
 {
  { 21890435,-13272907,-12624011,12154349,-7831873,15300496,23148983,-4470481,24618407,8283181 },
  { -33136107,-10512751,9975416,6841041,-31559793,16356536,3070187,-7025928,1466169,10740210 },
  { -1509399,-15488185,-13503385,-10655916,32799044,909394,-13938903,-5779719,-32164649,-15327040 },
 },
 {
  { 3960823,-14267803,-28026090,-15918051,-19404858,13146868,15567327,951507,-3260321,-573935 },
  { 24740841,5052253,-30094131,8961361,25877428,6165135,-24368180,14397372,-7380369,-6144105 },
  { -28888365,3510803,-28103278,-1158478,-11238128,-10631454,-15441463,-14453128,-1625486,-6494814 },
 },
},
{
 {
  { 793299,-9230478,8836302,-6235707,-27360908,-2369593,33152843,-4885251,-9906200,-621852 },
  { 5666233,525582,20782575,-8038419,-24538499,14657740,16099374,1468826,-6171428,-15186581 },
  { -4859255,-3779343,-2917758,-6748019,7778750,11688288,-30404353,-9871238,-1558923,-9863646 },
 },
 {
  { 10896332,-7719704,824275,472601,-19460308,3009587,25248958,14783338,-30581476,-15757844 },
  { 10566929,12612572,-31944212,11118703,-12633376,12362879,21752402,8822496,24003793,14264025 },
  { 27713862,-7355973,-11008240,9227530,27050101,2504721,23886875,-13117525,13958495,-5732453 },
 },
 {
  { -23481610,4867226,-27247128,3900521,29838369,-8212291,-31889399,-10041781,7340521,-15410068 },
  { 4646514,-8011124,-22766023,-11532654,23184553,8566613,31366726,-1381061,-15066784,-10375192 },
  { -17270517,12723032,-16993061,14878794,21619651,-6197576,27584817,3093888,-8843694,3849921 },
 },
 {
  { -9064912,2103172,25561640,-15125738,-5239824,9582958,32477045,-9017955,5002294,-15550259 },
  { -12057553,-11177906,21115585,-13365155,8808712,-12030708,16489530,13378448,-25845716,12741426 },
  { -5946367,10645103,-30911586,15390284,-3286982,-7118677,24306472,15852464,28834118,-7646072 },
 },
 {
  { -17335748,-9107057,-24531279,9434953,-8472084,-583362,-13090771,455841,20461858,5491305 },
  { 13669248,-16095482,-12481974,-10203039,-14569770,-11893198,-24995986,11293807,-28588204,-9421832 },
  { 28497928,6272777,-33022994,14470570,8906179,-1225630,18504674,-14165166,29867745,-8795943 },
 },
 {
  { -16207023,13517196,-27799630,-13697798,24009064,-6373891,-6367600,-13175392,22853429,-4012011 },
  { 24191378,16712145,-13931797,15217831,14542237,1646131,18603514,-11037887,12876623,-2112447 },
  { 17902668,4518229,-411702,-2829247,26878217,5258055,-12860753,608397,16031844,3723494 },
 },
 {
  { -28632773,12763728,-20446446,7577504,33001348,-13017745,17558842,-7872890,23896954,-4314245 },
  { -20005381,-12011952,31520464,605201,2543521,5991821,-2945064,7229064,-9919646,-8826859 },
  { 28816045,298879,-28165016,-15920938,19000928,-1665890,-12680833,-2949325,-18051778,-2082915 },
 },
 {
  { 16000882,-344896,3493092,-11447198,-29504595,-13159789,12577740,16041268,-19715240,7847707 },
  { 10151868,10572098,27312476,7922682,14825339,4723128,-32855931,-6519018,-10020567,3852848 },
  { -11430470,15697596,-21121557,-4420647,5386314,15063598,16514493,-15932110,29330899,-15076224 },
 },
},
{
 {
  { -25499735,-4378794,-15222908,-6901211,16615731,2051784,3303702,15490,-27548796,12314391 },
  { 15683520,-6003043,18109120,-9980648,15337968,-5997823,-16717435,15921866,16103996,-3731215 },
  { -23169824,-10781249,13588192,-1628807,-3798557,-1074929,-19273607,5402699,-29815713,-9841101 },
 },
 {
  { 23190676,2384583,-32714340,3462154,-29903655,-1529132,-11266856,8911517,-25205859,2739713 },
  { 21374101,-3554250,-33524649,9874411,15377179,11831242,-33529904,6134907,4931255,11987849 },
  { -7732,-2978858,-16223486,7277597,105524,-322051,-31480539,13861388,-30076310,10117930 },
 },
 {
  { -29501170,-10744872,-26163768,13051539,-25625564,5089643,-6325503,6704079,12890019,15728940 },
  { -21972360,-11771379,-951059,-4418840,14704840,2695116,903376,-10428139,12885167,8311031 },
  { -17516482,5352194,10384213,-13811658,7506451,13453191,26423267,4384730,1888765,-5435404 },
 },
 {
  { -25817338,-3107312,-13494599,-3182506,30896459,-13921729,-32251644,-12707869,-19464434,-3340243 },
  { -23607977,-2665774,-526091,4651136,5765089,4618330,6092245,14845197,17151279,-9854116 },
  { -24830458,-12733720,-15165978,10367250,-29530908,-265356,22825805,-7087279,-16866484,16176525 },
 },
 {
  { -23583256,6564961,20063689,3798228,-4740178,7359225,2006182,-10363426,-28746253,-10197509 },
  { -10626600,-4486402,-13320562,-5125317,3432136,-6393229,23632037,-1940610,32808310,1099883 },
  { 15030977,5768825,-27451236,-2887299,-6427378,-15361371,-15277896,-6809350,2051441,-15225865 },
 },
 {
  { -3362323,-7239372,7517890,9824992,23555850,295369,5148398,-14154188,-22686354,16633660 },
  { 4577086,-16752288,13249841,-15304328,19958763,-14537274,18559670,-10759549,8402478,-9864273 },
  { -28406330,-1051581,-26790155,-907698,-17212414,-11030789,9453451,-14980072,17983010,9967138 },
 },
 {
  { -25762494,6524722,26585488,9969270,24709298,1220360,-1677990,7806337,17507396,3651560 },
  { -10420457,-4118111,14584639,15971087,-15768321,8861010,26556809,-5574557,-18553322,-11357135 },
  { 2839101,14284142,4029895,3472686,14402957,12689363,-26642121,8459447,-5605463,-7621941 },
 },
 {
  { -4839289,-3535444,9744961,2871048,25113978,3187018,-25110813,-849066,17258084,-7977739 },
  { 18164541,-10595176,-17154882,-1542417,19237078,-9745295,23357533,-15217008,26908270,12150756 },
  { -30264870,-7647865,5112249,-7036672,-1499807,-6974257,43168,-5537701,-32302074,16215819 },
 },
},
{
 {
  { -6898905,9824394,-12304779,-4401089,-31397141,-6276835,32574489,12532905,-7503072,-8675347 },
  { -27343522,-16515468,-27151524,-10722951,946346,16291093,254968,7168080,21676107,-1943028 },
  { 21260961,-8424752,-16831886,-11920822,-23677961,3968121,-3651949,-6215466,-3556191,-7913075 },
 },
 {
  { 16544754,13250366,-16804428,15546242,-4583003,12757258,-2462308,-8680336,-18907032,-9662799 },
  { -2415239,-15577728,18312303,4964443,-15272530,-12653564,26820651,16690659,25459437,-4564609 },
  { -25144690,11425020,28423002,-11020557,-6144921,-15826224,9142795,-2391602,-6432418,-1644817 },
 },
 {
  { -23104652,6253476,16964147,-3768872,-25113972,-12296437,-27457225,-16344658,6335692,7249989 },
  { -30333227,13979675,7503222,-12368314,-11956721,-4621693,-30272269,2682242,25993170,-12478523 },
  { 4364628,5930691,32304656,-10044554,-8054781,15091131,22857016,-10598955,31820368,15075278 },
 },
 {
  { 31879134,-8918693,17258761,90626,-8041836,-4917709,24162788,-9650886,-17970238,12833045 },
  { 19073683,14851414,-24403169,-11860168,7625278,11091125,-19619190,2074449,-9413939,14905377 },
  { 24483667,-11935567,-2518866,-11547418,-1553130,15355506,-25282080,9253129,27628530,-7555480 },
 },
 {
  { 17597607,8340603,19355617,552187,26198470,-3176583,4593324,-9157582,-14110875,15297016 },
  { 510886,14337390,-31785257,16638632,6328095,2713355,-20217417,-11864220,8683221,2921426 },
  { 18606791,11874196,27155355,-5281482,-24031742,6265446,-25178240,-1278924,4674690,13890525 },
 },
 {
  { 13609624,13069022,-27372361,-13055908,24360586,9592974,14977157,9835105,4389687,288396 },
  { 9922506,-519394,13613107,5883594,-18758345,-434263,-12304062,8317628,23388070,16052080 },
  { 12720016,11937594,-31970060,-5028689,26900120,8561328,-20155687,-11632979,-14754271,-10812892 },
 },
 {
  { 15961858,14150409,26716931,-665832,-22794328,13603569,11829573,7467844,-28822128,929275 },
  { 11038231,-11582396,-27310482,-7316562,-10498527,-16307831,-23479533,-9371869,-21393143,2465074 },
  { 20017163,-4323226,27915242,1529148,12396362,15675764,13817261,-9658066,2463391,-4622140 },
 },
 {
  { -16358878,-12663911,-12065183,4996454,-1256422,1073572,9583558,12851107,4003896,12673717 },
  { -1731589,-15155870,-3262930,16143082,19294135,13385325,14741514,-9103726,7903886,2348101 },
  { 24536016,-16515207,12715592,-3862155,1511293,10047386,-3842346,-7129159,-28377538,10048127 },
 },
},
{
 {
  { -12622226,-6204820,30718825,2591312,-10617028,12192840,18873298,-7297090,-32297756,15221632 },
  { -26478122,-11103864,11546244,-1852483,9180880,7656409,-21343950,2095755,29769758,6593415 },
  { -31994208,-2907461,4176912,3264766,12538965,-868111,26312345,-6118678,30958054,8292160 },
 },
 {
  { 31429822,-13959116,29173532,15632448,12174511,-2760094,32808831,3977186,26143136,-3148876 },
  { 22648901,1402143,-22799984,13746059,7936347,365344,-8668633,-1674433,-3758243,-2304625 },
  { -15491917,8012313,-2514730,-12702462,-23965846,-10254029,-1612713,-1535569,-16664475,8194478 },
 },
 {
  { 27338066,-7507420,-7414224,10140405,-19026427,-6589889,27277191,8855376,28572286,3005164 },
  { 26287124,4821776,25476601,-4145903,-3764513,-15788984,-18008582,1182479,-26094821,-13079595 },
  { -7171154,3178080,23970071,6201893,-17195577,-4489192,-21876275,-13982627,32208683,-1198248 },
 },
 {
  { -16657702,2817643,-10286362,14811298,6024667,13349505,-27315504,-10497842,-27672585,-11539858 },
  { 15941029,-9405932,-21367050,8062055,31876073,-238629,-15278393,-1444429,15397331,-4130193 },
  { 8934485,-13485467,-23286397,-13423241,-32446090,14047986,31170398,-1441021,-27505566,15087184 },
 },
 {
  { -18357243,-2156491,24524913,-16677868,15520427,-6360776,-15502406,11461896,16788528,-5868942 },
  { -1947386,16013773,21750665,3714552,-17401782,-16055433,-3770287,-10323320,31322514,-11615635 },
  { 21426655,-5650218,-13648287,-5347537,-28812189,-4920970,-18275391,-14621414,13040862,-12112948 },
 },
 {
  { 11293895,12478086,-27136401,15083750,-29307421,14748872,14555558,-13417103,1613711,4896935 },
  { -25894883,15323294,-8489791,-8057900,25967126,-13425460,2825960,-4897045,-23971776,-11267415 },
  { -15924766,-5229880,-17443532,6410664,3622847,10243618,20615400,12405433,-23753030,-8436416 },
 },
 {
  { -7091295,12556208,-20191352,9025187,-17072479,4333801,4378436,2432030,23097949,-566018 },
  { 4565804,-16025654,20084412,-7842817,1724999,189254,24767264,10103221,-18512313,2424778 },
  { 366633,-11976806,8173090,-6890119,30788634,5745705,-7168678,1344109,-3642553,12412659 },
 },
 {
  { -24001791,7690286,14929416,-168257,-32210835,-13412986,24162697,-15326504,-3141501,11179385 },
  { 18289522,-14724954,8056945,16430056,-21729724,7842514,-6001441,-1486897,-18684645,-11443503 },
  { 476239,6601091,-6152790,-9723375,17503545,-4863900,27672959,13403813,11052904,5219329 },
 },
},
{
 {
  { 20678546,-8375738,-32671898,8849123,-5009758,14574752,31186971,-3973730,9014762,-8579056 },
  { -13644050,-10350239,-15962508,5075808,-1514661,-11534600,-33102500,9160280,8473550,-3256838 },
  { 24900749,14435722,17209120,-15292541,-22592275,9878983,-7689309,-16335821,-24568481,11788948 },
 },
 {
  { -3118155,-11395194,-13802089,14797441,9652448,-6845904,-20037437,10410733,-24568470,-1458691 },
  { -15659161,16736706,-22467150,10215878,-9097177,7563911,11871841,-12505194,-18513325,8464118 },
  { -23400612,8348507,-14585951,-861714,-3950205,-6373419,14325289,8628612,33313881,-8370517 },
 },
 {
  { -20186973,-4967935,22367356,5271547,-1097117,-4788838,-24805667,-10236854,-8940735,-5818269 },
  { -6948785,-1795212,-32625683,-16021179,32635414,-7374245,15989197,-12838188,28358192,-4253904 },
  { -23561781,-2799059,-32351682,-1661963,-9147719,10429267,-16637684,4072016,-5351664,5596589 },
 },
 {
  { -28236598,-3390048,12312896,6213178,3117142,16078565,29266239,2557221,1768301,15373193 },
  { -7243358,-3246960,-4593467,-7553353,-127927,-912245,-1090902,-4504991,-24660491,3442910 },
  { -30210571,5124043,14181784,8197961,18964734,-11939093,22597931,7176455,-18585478,13365930 },
 },
 {
  { -7877390,-1499958,8324673,4690079,6261860,890446,24538107,-8570186,-9689599,-3031667 },
  { 25008904,-10771599,-4305031,-9638010,16265036,15721635,683793,-11823784,15723479,-15163481 },
  { -9660625,12374379,-27006999,-7026148,-7724114,-12314514,11879682,5400171,519526,-1235876 },
 },
 {
  { 22258397,-16332233,-7869817,14613016,-22520255,-2950923,-20353881,7315967,16648397,7605640 },
  { -8081308,-8464597,-8223311,9719710,19259459,-15348212,23994942,-5281555,-9468848,4763278 },
  { -21699244,9220969,-15730624,1084137,-25476107,-2852390,31088447,-7764523,-11356529,728112 },
 },
 {
  { 26047220,-11751471,-6900323,-16521798,24092068,9158119,-4273545,-12555558,-29365436,-5498272 },
  { 17510331,-322857,5854289,8403524,17133918,-3112612,-28111007,12327945,10750447,10014012 },
  { -10312768,3936952,9156313,-8897683,16498692,-994647,-27481051,-666732,3424691,7540221 },
 },
 {
  { 30322361,-6964110,11361005,-4143317,7433304,4989748,-7071422,-16317219,-9244265,15258046 },
  { 13054562,-2779497,19155474,469045,-12482797,4566042,5631406,2711395,1062915,-5136345 },
  { -19240248,-11254599,-29509029,-7499965,-5835763,13005411,-6066489,12194497,32960380,1459310 },
 },
},
{
 {
  { 19852034,7027924,23669353,10020366,8586503,-6657907,394197,-6101885,18638003,-11174937 },
  { 31395534,15098109,26581030,8030562,-16527914,-5007134,9012486,-7584354,-6643087,-5442636 },
  { -9192165,-2347377,-1997099,4529534,25766844,607986,-13222,9677543,-32294889,-6456008 },
 },
 {
  { -2444496,-149937,29348902,8186665,1873760,12489863,-30934579,-7839692,-7852844,-8138429 },
  { -15236356,-15433509,7766470,746860,26346930,-10221762,-27333451,10754588,-9431476,5203576 },
  { 31834314,14135496,-770007,5159118,20917671,-16768096,-7467973,-7337524,31809243,7347066 },
 },
 {
  { -9606723,-11874240,20414459,13033986,13716524,-11691881,19797970,-12211255,15192876,-2087490 },
  { -12663563,-2181719,1168162,-3804809,26747877,-14138091,10609330,12694420,33473243,-13382104 },
  { 33184999,11180355,15832085,-11385430,-1633671,225884,15089336,-11023903,-6135662,14480053 },
 },
 {
  { 31308717,-5619998,31030840,-1897099,15674547,-6582883,5496208,13685227,27595050,8737275 },
  { -20318852,-15150239,10933843,-16178022,8335352,-7546022,-31008351,-12610604,26498114,66511 },
  { 22644454,-8761729,-16671776,4884562,-3105614,-13559366,30540766,-4286747,-13327787,-7515095 },
 },
 {
  { -28017847,9834845,18617207,-2681312,-3401956,-13307506,8205540,13585437,-17127465,15115439 },
  { 23711543,-672915,31206561,-8362711,6164647,-9709987,-33535882,-1426096,8236921,16492939 },
  { -23910559,-13515526,-26299483,-4503841,25005590,-7687270,19574902,10071562,6708380,-6222424 },
 },
 {
  { 2101391,-4930054,19702731,2367575,-15427167,1047675,5301017,9328700,29955601,-11678310 },
  { 3096359,9271816,-21620864,-15521844,-14847996,-7592937,-25892142,-12635595,-9917575,6216608 },
  { -32615849,338663,-25195611,2510422,-29213566,-13820213,24822830,-6146567,-26767480,7525079 },
 },
 {
  { -23066649,-13985623,16133487,-7896178,-3389565,778788,-910336,-2782495,-19386633,11994101 },
  { 21691500,-13624626,-641331,-14367021,3285881,-3483596,-25064666,9718258,-7477437,13381418 },
  { 18445390,-4202236,14979846,11622458,-1727110,-3582980,23111648,-6375247,28535282,15779576 },
 },
 {
  { 30098053,3089662,-9234387,16662135,-21306940,11308411,-14068454,12021730,9955285,-16303356 },
  { 9734894,-14576830,-7473633,-9138735,2060392,11313496,-18426029,9924399,20194861,13380996 },
  { -26378102,-7965207,-22167821,15789297,-18055342,-6168792,-1984914,15707771,26342023,10146099 },
 },
},
{
 {
  { -26016874,-219943,21339191,-41388,19745256,-2878700,-29637280,2227040,21612326,-545728 },
  { -13077387,1184228,23562814,-5970442,-20351244,-6348714,25764461,12243797,-20856566,11649658 },
  { -10031494,11262626,27384172,2271902,26947504,-15997771,39944,6114064,33514190,2333242 },
 },
 {
  { -21433588,-12421821,8119782,7219913,-21830522,-9016134,-6679750,-12670638,24350578,-13450001 },
  { -4116307,-11271533,-23886186,4843615,-30088339,690623,-31536088,-10406836,8317860,12352766 },
  { 18200138,-14475911,-33087759,-2696619,-23702521,-9102511,-23552096,-2287550,20712163,6719373 },
 },
 {
  { 26656208,6075253,-7858556,1886072,-28344043,4262326,11117530,-3763210,26224235,-3297458 },
  { -17168938,-14854097,-3395676,-16369877,-19954045,14050420,21728352,9493610,18620611,-16428628 },
  { -13323321,13325349,11432106,5964811,18609221,6062965,-5269471,-9725556,-30701573,-16479657 },
 },
 {
  { -23860538,-11233159,26961357,1640861,-32413112,-16737940,12248509,-5240639,13735342,1934062 },
  { 25089769,6742589,17081145,-13406266,21909293,-16067981,-15136294,-3765346,-21277997,5473616 },
  { 31883677,-7961101,1083432,-11572403,22828471,13290673,-7125085,12469656,29111212,-5451014 },
 },
 {
  { 24244947,-15050407,-26262976,2791540,-14997599,16666678,24367466,6388839,-10295587,452383 },
  { -25640782,-3417841,5217916,16224624,19987036,-4082269,-24236251,-5915248,15766062,8407814 },
  { -20406999,13990231,15495425,16395525,5377168,15166495,-8917023,-4388953,-8067909,2276718 },
 },
 {
  { 30157918,12924066,-17712050,9245753,19895028,3368142,-23827587,5096219,22740376,-7303417 },
  { 2041139,-14256350,7783687,13876377,-25946985,-13352459,24051124,13742383,-15637599,13295222 },
  { 33338237,-8505733,12532113,7977527,9106186,-1715251,-17720195,-4612972,-4451357,-14669444 },
 },
 {
  { -20045281,5454097,-14346548,6447146,28862071,1883651,-2469266,-4141880,7770569,9620597 },
  { 23208068,7979712,33071466,8149229,1758231,-10834995,30945528,-1694323,-33502340,-14767970 },
  { 1439958,-16270480,-1079989,-793782,4625402,10647766,-5043801,1220118,30494170,-11440799 },
 },
 {
  { -5037580,-13028295,-2970559,-3061767,15640974,-6701666,-26739026,926050,-1684339,-13333647 },
  { 13908495,-3549272,30919928,-6273825,-21521863,7989039,9021034,9078865,3353509,4033511 },
  { -29663431,-15113610,32259991,-344482,24295849,-12912123,23161163,8839127,27485041,7356032 },
 },
},
{
 {
  { 9661027,705443,11980065,-5370154,-1628543,14661173,-6346142,2625015,28431036,-16771834 },
  { -23839233,-8311415,-25945511,7480958,-17681669,-8354183,-22545972,14150565,15970762,4099461 },
  { 29262576,16756590,26350592,-8793563,8529671,-11208050,13617293,-9937143,11465739,8317062 },
 },
 {
  { -25493081,-6962928,32500200,-9419051,-23038724,-2302222,14898637,3848455,20969334,-5157516 },
  { -20384450,-14347713,-18336405,13884722,-33039454,2842114,-21610826,-3649888,11177095,14989547 },
  { -24496721,-11716016,16959896,2278463,12066309,10137771,13515641,2581286,-28487508,9930240 },
 },
 {
  { -17751622,-2097826,16544300,-13009300,-15914807,-14949081,18345767,-13403753,16291481,-5314038 },
  { -33229194,2553288,32678213,9875984,8534129,6889387,-9676774,6957617,4368891,9788741 },
  { 16660756,7281060,-10830758,12911820,20108584,-8101676,-21722536,-8613148,16250552,-11111103 },
 },
 {
  { -19765507,2390526,-16551031,14161980,1905286,6414907,4689584,10604807,-30190403,4782747 },
  { -1354539,14736941,-7367442,-13292886,7710542,-14155590,-9981571,4383045,22546403,437323 },
  { 31665577,-12180464,-16186830,1491339,-18368625,3294682,27343084,2786261,-30633590,-14097016 },
 },
 {
  { -14467279,-683715,-33374107,7448552,19294360,14334329,-19690631,2355319,-19284671,-6114373 },
  { 15121312,-15796162,6377020,-6031361,-10798111,-12957845,18952177,15496498,-29380133,11754228 },
  { -2637277,-13483075,8488727,-14303896,12728761,-1622493,7141596,11724556,22761615,-10134141 },
 },
 {
  { 16918416,11729663,-18083579,3022987,-31015732,-13339659,-28741185,-12227393,32851222,11717399 },
  { 11166634,7338049,-6722523,4531520,-29468672,-7302055,31474879,3483633,-1193175,-4030831 },
  { -185635,9921305,31456609,-13536438,-12013818,13348923,33142652,6546660,-19985279,-3948376 },
 },
 {
  { -32460596,11266712,-11197107,-7899103,31703694,3855903,-8537131,-12833048,-30772034,-15486313 },
  { -18006477,12709068,3991746,-6479188,-21491523,-10550425,-31135347,-16049879,10928917,3011958 },
  { -6957757,-15594337,31696059,334240,29576716,14796075,-30831056,-12805180,18008031,10258577 },
 },
 {
  { -22448644,15655569,7018479,-4410003,-30314266,-1201591,-1853465,1367120,25127874,6671743 },
  { 29701166,-14373934,-10878120,9279288,-17568,13127210,21382910,11042292,25838796,4642684 },
  { -20430234,14955537,-24126347,8124619,-5369288,-5990470,30468147,-13900640,18423289,4177476 },
 },
},
} ;
#endif


static void ge_select(ge_precomp *t,int pos,signed char b)
{
#ifndef CURVED25519_ASM
  ge_precomp minust;
  unsigned char bnegative = negative(b);
  unsigned char babs = (unsigned char)(b - (((-bnegative) & b) << 1));

  ge_precomp_0(t);
  cmov(t,&base[pos][0],babs,1);
  cmov(t,&base[pos][1],babs,2);
  cmov(t,&base[pos][2],babs,3);
  cmov(t,&base[pos][3],babs,4);
  cmov(t,&base[pos][4],babs,5);
  cmov(t,&base[pos][5],babs,6);
  cmov(t,&base[pos][6],babs,7);
  cmov(t,&base[pos][7],babs,8);
  fe_cswap(t->yminusx, t->yplusx, bnegative);
  fe_neg(minust.xy2d,t->xy2d);
  fe_cmov(t->xy2d,minust.xy2d,bnegative);
#else
  fe_cmov_table((fe*)t, (fe*)base[pos], b);
#endif
}

/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/
void ge_scalarmult_base(ge_p3 *h,const unsigned char *a)
{
  signed char e[64];
  signed char carry;
  ge_p1p1 r;
#ifndef CURVED25519_ASM
  ge_p2 s;
#endif
  ge_precomp t;
  int i;

  for (i = 0;i < 32;++i) {
    e[2 * i + 0] = (a[i] >> 0) & 15;
    e[2 * i + 1] = (a[i] >> 4) & 15;
  }
  /* each e[i] is between 0 and 15 */
  /* e[63] is between 0 and 7 */

  carry = 0;
  for (i = 0;i < 63;++i) {
    e[i] += carry;
    carry = e[i] + 8;
    carry >>= 4;
    e[i] -= (signed char)(carry << 4);
  }
  e[63] += carry;
  /* each e[i] is between -8 and 8 */

#ifndef CURVED25519_ASM
  ge_select(&t,0,e[1]);
  fe_sub(h->X, t.yplusx, t.yminusx);
  fe_add(h->Y, t.yplusx, t.yminusx);
  fe_0(h->Z);
  h->Z[0] = 4;
  fe_mul(h->T,h->X,h->Y);
  fe_add(h->X, h->X, h->X);
  fe_add(h->Y, h->Y, h->Y);

  for (i = 3;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }

  ge_p3_dbl(&r,h);  ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p3(h,&r);

  for (i = 0;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }
#else
  ge_select(&t, 0, e[0]);
  fe_sub(h->X, t.yplusx, t.yminusx);
  fe_add(h->Y, t.yplusx, t.yminusx);
  fe_0(h->Z);
  h->Z[0] = 2;
  fe_copy(h->T, t.xy2d);
  for (i = 1; i < 64; i++) {
    ge_select(&t, i, e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }
#endif
}


#define SLIDE_SIZE 256

/* ge double scalar mult */
static void slide(signed char *r,const unsigned char *a)
{
  int i;
  int b;
  int k;

  for (i = 0;i < SLIDE_SIZE;++i)
    r[i] = 1 & (a[i >> 3] >> (i & 7));

  for (i = 0;i < SLIDE_SIZE;++i)
    if (r[i]) {
      for (b = 1;b <= 6 && i + b < SLIDE_SIZE;++b) {
        if (r[i + b]) {
          if (r[i] + (r[i + b] << b) <= 15) {
            r[i] += (signed char)(r[i + b] << b); r[i + b] = 0;
          } else if (r[i] - (r[i + b] << b) >= -15) {
            r[i] -= (signed char)(r[i + b] << b);
            for (k = i + b;k < SLIDE_SIZE;++k) {
              if (!r[k]) {
                r[k] = 1;
                break;
              }
              r[k] = 0;
            }
          } else
            break;
        }
      }
    }
}

#ifdef CURVED25519_ASM_64BIT
static const ge_precomp Bi[8] = {
    {
        { 0x2fbc93c6f58c3b85, -0x306cd2390473f1e7, 0x270b4898643d42c2, 0x07cf9d3a33d4ba65,  },
        { -0x62efc6fa28bf6ec2, -0x02c660fa2ebf414d, -0x5a3e7bcb977075f7, 0x44fd2f9298f81267,  },
        { -0x5436edfa78855598, 0x26d9e823ccaac49e, 0x5a1b7dcbdd43598c, 0x6f117b689f0c65a8,  },
    },
    {
        { -0x50da4f57b31168d0, 0x025a8430e8864b8a, -0x3ee4affd60fe98ce, 0x7a164e1b9a80f8f4,  },
        { 0x56611fe8a4fcd265, 0x3bd353fde5c1ba7d, -0x7ece0ce5deb42943, 0x2ab91587555bda62,  },
        { 0x14ae933f0dd0d889, 0x589423221c35da62, -0x2e8f1aba730d24b4, 0x5a2826af12b9b4c6,  },
    },
    {
        { -0x5ded43bbf75a44cd, -0x72afb73c38a112fe, -0x22e414f3a54013bc, 0x2945ccf146e206eb,  },
        { 0x7f9182c3a447d6ba, -0x2affeb2eb4d8d649, -0x1cc30ee3479b5f79, 0x154a7e73eb1b55f3,  },
        { -0x4344240e7ed57d7b, 0x270e0807d0bdd1fc, -0x4be498f4e44258d3, 0x43aabe696b3bb69a,  },
    },
    {
        { 0x6b1a5cd0944ea3bf, 0x7470353ab39dc0d2, 0x71b2528228542e49, 0x461bea69283c927e,  },
        { -0x4590d36555cdde4f, 0x6ca021533bba23a7, -0x621589b06de6d3c6, 0x1d6edd5d2e5317e0,  },
        { -0x0e7c9237fe474c5e, -0x4cfca0b8fac15b66, 0x529c41ba5877adf3, 0x7a9fbb1c6a0f90a7,  },
    },
    {
        { -0x64d1987559579cd1, -0x59af6190ae43b93b, -0x314dcc3639790a4b, 0x34b9ed338add7f59,  },
        { -0x0c91de81fc627f9c, -0x675f7e490adfbe65, -0x693439f718a14fbc, 0x49c05a51fadc9c8f,  },
        { 0x06b4e8bf9045af1b, -0x1d007c1758e62dd1, -0x550903d66c2b30ea, 0x73c172021b008b06,  },
    },
    {
        { 0x2fbf00848a802ade, -0x1a260130fdcfd1d9, 0x113e847117703406, 0x4275aae2546d8faf,  },
        { 0x315f5b0249864348, 0x3ed6b36977088381, -0x5c5f8aaa9572146b, 0x18ab598029d5c77f,  },
        { -0x27d4d33a029f7617, 0x031eb4a13282e4a4, 0x44311199b51a8622, 0x3dc65522b53df948,  },
    },
    {
        { -0x408f3ddd5dff8093, -0x407b4c654a432125, 0x537a0e12fb07ba07, 0x234fd7eec346f241,  },
        { 0x506f013b327fbf93, -0x5103143664889095, -0x62ed4dcd5552a698, 0x0267882d176024a7,  },
        { 0x5360a119732ea378, 0x2437e6b1df8dd471, -0x5d10c8076e581acd, 0x497ba6fdaa097863,  },
    },
    {
        { 0x24cecc0313cfeaa0, -0x79b73d72e763db93, 0x2dbdbdfac1f2d4d0, 0x61e22917f12de72b,  },
        { 0x040bcd86468ccf0b, -0x2c7d645bd566ef2a, 0x7508300807b25192, 0x43b5cd4218d05ebf,  },
        { 0x5d9a762f9bd0b516, -0x14c750b1c8c02112, 0x032e5a7d93d64270, 0x511d61210ae4d842,  },
    },
};
#elif defined(CURVED25519_ASM_32BIT)
static const ge_precomp Bi[8] = {
    {
        { -0x0a73c47b, 0x2fbc93c6, -0x0473f1e7, -0x306cd23a, 0x643d42c2, 0x270b4898, 0x33d4ba65, 0x07cf9d3a,  },
        { -0x28bf6ec2, -0x62efc6fb, -0x2ebf414d, -0x02c660fb, 0x688f8a09, -0x5a3e7bcc, -0x6707ed99, 0x44fd2f92,  },
        { -0x78855598, -0x5436edfb, -0x33553b62, 0x26d9e823, -0x22bca674, 0x5a1b7dcb, -0x60f39a58, 0x6f117b68,  },
    },
    {
        { 0x4cee9730, -0x50da4f58, -0x1779b476, 0x025a8430, -0x60fe98ce, -0x3ee4affe, -0x657f070c, 0x7a164e1b,  },
        { -0x5b032d9b, 0x56611fe8, -0x1a3e4583, 0x3bd353fd, 0x214bd6bd, -0x7ece0ce6, 0x555bda62, 0x2ab91587,  },
        { 0x0dd0d889, 0x14ae933f, 0x1c35da62, 0x58942322, -0x730d24b4, -0x2e8f1abb, 0x12b9b4c6, 0x5a2826af,  },
    },
    {
        { 0x08a5bb33, -0x5ded43bc, -0x38a112fe, -0x72afb73d, 0x5abfec44, -0x22e414f4, 0x46e206eb, 0x2945ccf1,  },
        { -0x5bb82946, 0x7f9182c3, 0x4b2729b7, -0x2affeb2f, -0x479b5f79, -0x1cc30ee4, -0x14e4aa0d, 0x154a7e73,  },
        { -0x7ed57d7b, -0x4344240f, -0x2f422e04, 0x270e0807, 0x1bbda72d, -0x4be498f5, 0x6b3bb69a, 0x43aabe69,  },
    },
    {
        { -0x6bb15c41, 0x6b1a5cd0, -0x4c623f2e, 0x7470353a, 0x28542e49, 0x71b25282, 0x283c927e, 0x461bea69,  },
        { -0x55cdde4f, -0x4590d366, 0x3bba23a7, 0x6ca02153, -0x6de6d3c6, -0x621589b1, 0x2e5317e0, 0x1d6edd5d,  },
        { 0x01b8b3a2, -0x0e7c9238, 0x053ea49a, -0x4cfca0b9, 0x5877adf3, 0x529c41ba, 0x6a0f90a7, 0x7a9fbb1c,  },
    },
    {
        { -0x59579cd1, -0x64d19876, 0x51bc46c5, -0x59af6191, -0x39790a4b, -0x314dcc37, -0x752280a7, 0x34b9ed33,  },
        { 0x039d8064, -0x0c91de82, -0x0adfbe65, -0x675f7e4a, -0x18a14fbc, -0x693439f8, -0x05236371, 0x49c05a51,  },
        { -0x6fba50e5, 0x06b4e8bf, -0x58e62dd1, -0x1d007c18, -0x6c2b30ea, -0x550903d7, 0x1b008b06, 0x73c17202,  },
    },
    {
        { -0x757fd522, 0x2fbf0084, 0x02302e27, -0x1a260131, 0x17703406, 0x113e8471, 0x546d8faf, 0x4275aae2,  },
        { 0x49864348, 0x315f5b02, 0x77088381, 0x3ed6b369, 0x6a8deb95, -0x5c5f8aab, 0x29d5c77f, 0x18ab5980,  },
        { -0x029f7617, -0x27d4d33b, 0x3282e4a4, 0x031eb4a1, -0x4ae579de, 0x44311199, -0x4ac206b8, 0x3dc65522,  },
    },
    {
        { -0x5dff8093, -0x408f3dde, -0x4a432125, -0x407b4c66, -0x04f845f9, 0x537a0e12, -0x3cb90dbf, 0x234fd7ee,  },
        { 0x327fbf93, 0x506f013b, -0x64889095, -0x51031437, -0x5552a698, -0x62ed4dce, 0x176024a7, 0x0267882d,  },
        { 0x732ea378, 0x5360a119, -0x20722b8f, 0x2437e6b1, -0x6e581acd, -0x5d10c808, -0x55f6879d, 0x497ba6fd,  },
    },
    {
        { 0x13cfeaa0, 0x24cecc03, 0x189c246d, -0x79b73d73, -0x3e0d2b30, 0x2dbdbdfa, -0x0ed218d5, 0x61e22917,  },
        { 0x468ccf0b, 0x040bcd86, 0x2a9910d6, -0x2c7d645c, 0x07b25192, 0x75083008, 0x18d05ebf, 0x43b5cd42,  },
        { -0x642f4aea, 0x5d9a762f, 0x373fdeee, -0x14c750b2, -0x6c29bd90, 0x032e5a7d, 0x0ae4d842, 0x511d6121,  },
    },
};
#elif defined(CURVED25519_128BIT)
static const ge_precomp Bi[8] = {
    {
        { 0x493c6f58c3b85, 0x0df7181c325f7, 0x0f50b0b3e4cb7, 0x5329385a44c32, 0x07cf9d3a33d4b },
        { 0x03905d740913e, 0x0ba2817d673a2, 0x23e2827f4e67c, 0x133d2e0c21a34, 0x44fd2f9298f81 },
        { 0x11205877aaa68, 0x479955893d579, 0x50d66309b67a0, 0x2d42d0dbee5ee, 0x6f117b689f0c6 },
    },
    {
        { 0x5b0a84cee9730, 0x61d10c97155e4, 0x4059cc8096a10, 0x47a608da8014f, 0x7a164e1b9a80f },
        { 0x11fe8a4fcd265, 0x7bcb8374faacc, 0x52f5af4ef4d4f, 0x5314098f98d10, 0x2ab91587555bd },
        { 0x6933f0dd0d889, 0x44386bb4c4295, 0x3cb6d3162508c, 0x26368b872a2c6, 0x5a2826af12b9b },
    },
    {
        { 0x2bc4408a5bb33, 0x078ebdda05442, 0x2ffb112354123, 0x375ee8df5862d, 0x2945ccf146e20 },
        { 0x182c3a447d6ba, 0x22964e536eff2, 0x192821f540053, 0x2f9f19e788e5c, 0x154a7e73eb1b5 },
        { 0x3dbf1812a8285, 0x0fa17ba3f9797, 0x6f69cb49c3820, 0x34d5a0db3858d, 0x43aabe696b3bb },
    },
    {
        { 0x25cd0944ea3bf, 0x75673b81a4d63, 0x150b925d1c0d4, 0x13f38d9294114, 0x461bea69283c9 },
        { 0x72c9aaa3221b1, 0x267774474f74d, 0x064b0e9b28085, 0x3f04ef53b27c9, 0x1d6edd5d2e531 },
        { 0x36dc801b8b3a2, 0x0e0a7d4935e30, 0x1deb7cecc0d7d, 0x053a94e20dd2c, 0x7a9fbb1c6a0f9 },
    },
    {
        { 0x6678aa6a8632f, 0x5ea3788d8b365, 0x21bd6d6994279, 0x7ace75919e4e3, 0x34b9ed338add7 },
        { 0x6217e039d8064, 0x6dea408337e6d, 0x57ac112628206, 0x647cb65e30473, 0x49c05a51fadc9 },
        { 0x4e8bf9045af1b, 0x514e33a45e0d6, 0x7533c5b8bfe0f, 0x583557b7e14c9, 0x73c172021b008 },
    },
    {
        { 0x700848a802ade, 0x1e04605c4e5f7, 0x5c0d01b9767fb, 0x7d7889f42388b, 0x4275aae2546d8 },
        { 0x75b0249864348, 0x52ee11070262b, 0x237ae54fb5acd, 0x3bfd1d03aaab5, 0x18ab598029d5c },
        { 0x32cc5fd6089e9, 0x426505c949b05, 0x46a18880c7ad2, 0x4a4221888ccda, 0x3dc65522b53df },
    },
    {
        { 0x0c222a2007f6d, 0x356b79bdb77ee, 0x41ee81efe12ce, 0x120a9bd07097d, 0x234fd7eec346f },
        { 0x7013b327fbf93, 0x1336eeded6a0d, 0x2b565a2bbf3af, 0x253ce89591955, 0x0267882d17602 },
        { 0x0a119732ea378, 0x63bf1ba8e2a6c, 0x69f94cc90df9a, 0x431d1779bfc48, 0x497ba6fdaa097 },
    },
    {
        { 0x6cc0313cfeaa0, 0x1a313848da499, 0x7cb534219230a, 0x39596dedefd60, 0x61e22917f12de },
        { 0x3cd86468ccf0b, 0x48553221ac081, 0x6c9464b4e0a6e, 0x75fba84180403, 0x43b5cd4218d05 },
        { 0x2762f9bd0b516, 0x1c6e7fbddcbb3, 0x75909c3ace2bd, 0x42101972d3ec9, 0x511d61210ae4d },
    },
};
#else
static const ge_precomp Bi[8] = {
 {
  { 25967493,-14356035,29566456,3660896,-12694345,4014787,27544626,-11754271,-6079156,2047605 },
  { -12545711,934262,-2722910,3049990,-727428,9406986,12720692,5043384,19500929,-15469378 },
  { -8738181,4489570,9688441,-14785194,10184609,-12363380,29287919,11864899,-24514362,-4438546 },
 },
 {
  { 15636291,-9688557,24204773,-7912398,616977,-16685262,27787600,-14772189,28944400,-1550024 },
  { 16568933,4717097,-11556148,-1102322,15682896,-11807043,16354577,-11775962,7689662,11199574 },
  { 30464156,-5976125,-11779434,-15670865,23220365,15915852,7512774,10017326,-17749093,-9920357 },
 },
 {
  { 10861363,11473154,27284546,1981175,-30064349,12577861,32867885,14515107,-15438304,10819380 },
  { 4708026,6336745,20377586,9066809,-11272109,6594696,-25653668,12483688,-12668491,5581306 },
  { 19563160,16186464,-29386857,4097519,10237984,-4348115,28542350,13850243,-23678021,-15815942 },
 },
 {
  { 5153746,9909285,1723747,-2777874,30523605,5516873,19480852,5230134,-23952439,-15175766 },
  { -30269007,-3463509,7665486,10083793,28475525,1649722,20654025,16520125,30598449,7715701 },
  { 28881845,14381568,9657904,3680757,-20181635,7843316,-31400660,1370708,29794553,-1409300 },
 },
 {
  { -22518993,-6692182,14201702,-8745502,-23510406,8844726,18474211,-1361450,-13062696,13821877 },
  { -6455177,-7839871,3374702,-4740862,-27098617,-10571707,31655028,-7212327,18853322,-14220951 },
  { 4566830,-12963868,-28974889,-12240689,-7602672,-2830569,-8514358,-10431137,2207753,-3209784 },
 },
 {
  { -25154831,-4185821,29681144,7868801,-6854661,-9423865,-12437364,-663000,-31111463,-16132436 },
  { 25576264,-2703214,7349804,-11814844,16472782,9300885,3844789,15725684,171356,6466918 },
  { 23103977,13316479,9739013,-16149481,817875,-15038942,8965339,-14088058,-30714912,16193877 },
 },
 {
  { -33521811,3180713,-2394130,14003687,-16903474,-16270840,17238398,4729455,-18074513,9256800 },
  { -25182317,-4174131,32336398,5036987,-21236817,11360617,22616405,9761698,-19827198,630305 },
  { -13720693,2639453,-24237460,-7406481,9494427,-5774029,-6554551,-15960994,-2449256,-14291300 },
 },
 {
  { -3151181,-5046075,9282714,6866145,-31907062,-863023,-18940575,15033784,25105118,-7894876 },
  { -24326370,15950226,-31801215,-14592823,-11662737,-5090925,1573892,-2625887,2198790,-15804619 },
  { -3099351,10324967,-2241613,7453183,-5446979,-2735503,-13812022,-16236442,-32461234,-12290683 },
 },
} ;
#endif


/*
r = a * A + b * B
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/
int ge_double_scalarmult_vartime(ge_p2 *r, const unsigned char *a,
                                 const ge_p3 *A, const unsigned char *b)
{
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
  signed char *aslide = NULL;
  signed char *bslide = NULL;
  ge_cached *Ai = NULL; /* A,3A,5A,7A,9A,11A,13A,15A */

  ge_p1p1 *t = NULL;
  ge_p3 *u = NULL;
  ge_p3 *A2 = NULL;

  int ret;
#else
  signed char aslide[SLIDE_SIZE];
  signed char bslide[SLIDE_SIZE];
  ge_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */

  ge_p1p1 t[1];
  ge_p3 u[1];
  ge_p3 A2[1];
#endif
  int i;

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
  if (((aslide = (signed char *)XMALLOC(SLIDE_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL) ||
      ((bslide = (signed char *)XMALLOC(SLIDE_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL) ||
      ((Ai = (ge_cached *)XMALLOC(8 * sizeof(*Ai), NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL) ||
      ((t = (ge_p1p1 *)XMALLOC(sizeof(*t), NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL) ||
      ((u = (ge_p3 *)XMALLOC(sizeof(*u), NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL) ||
      ((A2 = (ge_p3 *)XMALLOC(sizeof(*A2), NULL, DYNAMIC_TYPE_TMP_BUFFER))== NULL))
  {
      ret = MEMORY_E;
      goto out;
  } else
      ret = 0;
#endif

  slide(aslide,a);
  slide(bslide,b);

  ge_p3_to_cached(&Ai[0],A);
  ge_p3_dbl(t,A); ge_p1p1_to_p3(A2,t);
  ge_add(t,A2,&Ai[0]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[1],u);
  ge_add(t,A2,&Ai[1]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[2],u);
  ge_add(t,A2,&Ai[2]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[3],u);
  ge_add(t,A2,&Ai[3]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[4],u);
  ge_add(t,A2,&Ai[4]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[5],u);
  ge_add(t,A2,&Ai[5]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[6],u);
  ge_add(t,A2,&Ai[6]); ge_p1p1_to_p3(u,t); ge_p3_to_cached(&Ai[7],u);

  ge_p2_0(r);

  for (i = 255;i >= 0;--i) {
    if (aslide[i] || bslide[i]) break;
  }

  for (;i >= 0;--i) {
    ge_p2_dbl(t,r);

    if (aslide[i] > 0) {
      ge_p1p1_to_p3(u,t);
      ge_add(t,u,&Ai[aslide[i]/2]);
    } else if (aslide[i] < 0) {
      ge_p1p1_to_p3(u,t);
      ge_sub(t,u,&Ai[(-aslide[i])/2]);
    }

    if (bslide[i] > 0) {
      ge_p1p1_to_p3(u,t);
      ge_madd(t,u,&Bi[bslide[i]/2]);
    } else if (bslide[i] < 0) {
      ge_p1p1_to_p3(u,t);
      ge_msub(t,u,&Bi[(-bslide[i])/2]);
    }

    ge_p1p1_to_p2(r,t);
  }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
  out:

  if (aslide != NULL)
      XFREE(aslide, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (bslide != NULL)
      XFREE(bslide, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (Ai != NULL)
      XFREE(Ai, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (t != NULL)
      XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (u != NULL)
      XFREE(u, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (A2 != NULL)
      XFREE(A2, NULL, DYNAMIC_TYPE_TMP_BUFFER);

  return ret;
#else
  return 0;
#endif
}

#ifdef CURVED25519_ASM_64BIT
static const ge d = {
    0x75eb4dca135978a3, 0x00700a4d4141d8ab, -0x7338bf8688861768, 0x52036cee2b6ffe73,
};
#elif defined(CURVED25519_ASM_32BIT)
static const ge d = {
    0x135978a3, 0x75eb4dca, 0x4141d8ab, 0x00700a4d, 0x7779e898, -0x7338bf87, 0x2b6ffe73, 0x52036cee,
};
#elif defined(CURVED25519_128BIT)
static const ge d = {
    0x34dca135978a3, 0x1a8283b156ebd, 0x5e7a26001c029, 0x739c663a03cbb,
    0x52036cee2b6ff
};
#else
static const ge d = {
-10913610,13857413,-15372611,6949391,114729,
-8787816,-6275908,-3247719,-18696448,-12055116
};
#endif


#ifdef CURVED25519_ASM_64BIT
static const ge sqrtm1 = {
    -0x3b11e4d8b5f15f50, 0x2f431806ad2fe478, 0x2b4d00993dfbd7a7, 0x2b8324804fc1df0b,
};
#elif defined(CURVED25519_ASM_32BIT)
static const ge sqrtm1 = {
    0x4a0ea0b0, -0x3b11e4d9, -0x52d01b88, 0x2f431806, 0x3dfbd7a7, 0x2b4d0099, 0x4fc1df0b, 0x2b832480,
};
#elif defined(CURVED25519_128BIT)
static const ge sqrtm1 = {
    0x61b274a0ea0b0, 0x0d5a5fc8f189d, 0x7ef5e9cbd0c60, 0x78595a6804c9e,
    0x2b8324804fc1d
};
#else
static const ge sqrtm1 = {
-32595792,-7943725,9377950,3500415,12389472,
-272473,-25146209,-2005654,326686,11406482
};
#endif


int ge_frombytes_negate_vartime(ge_p3 *h,const unsigned char *s)
{
  ge u;
  ge v;
  ge v3;
  ge vxx;
  ge check;

  fe_frombytes(h->Y,s);
  fe_1(h->Z);
  fe_sq(u,h->Y);
  fe_mul(v,u,d);
  fe_sub(u,u,h->Z);       /* u = y^2-1 */
  fe_add(v,v,h->Z);       /* v = dy^2+1 */


  fe_sq(v3,v);
  fe_mul(v3,v3,v);        /* v3 = v^3 */
  fe_sq(h->X,v3);
  fe_mul(h->X,h->X,v);
  fe_mul(h->X,h->X,u);    /* x = uv^7 */

  fe_pow22523(h->X,h->X); /* x = (uv^7)^((q-5)/8) */
  fe_mul(h->X,h->X,v3);
  fe_mul(h->X,h->X,u);    /* x = uv^3(uv^7)^((q-5)/8) */

  fe_sq(vxx,h->X);
  fe_mul(vxx,vxx,v);
  fe_sub(check,vxx,u);    /* vx^2-u */
  if (fe_isnonzero(check)) {
    fe_add(check,vxx,u);  /* vx^2+u */
    if (fe_isnonzero(check)) return -1;
    fe_mul(h->X,h->X,sqrtm1);
  }

  if (fe_isnegative(h->X) == (s[31] >> 7))
    fe_neg(h->X,h->X);

  fe_mul(h->T,h->X,h->Y);
  return 0;
}


/* ge madd */
/*
r = p + q
*/

static WC_INLINE void ge_madd(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
{
#ifndef CURVED25519_ASM
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->yplusx);
    fe_mul(r->Y,r->Y,q->yminusx);
    fe_mul(r->T,q->xy2d,p->T);
    fe_add(t0,p->Z,p->Z);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_add(r->Z,t0,r->T);
    fe_sub(r->T,t0,r->T);
#else
    fe_ge_madd(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->xy2d,
              q->yplusx, q->yminusx);
#endif
}


/* ge msub */

/*
r = p - q
*/

static WC_INLINE void ge_msub(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
{
#ifndef CURVED25519_ASM
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->yminusx);
    fe_mul(r->Y,r->Y,q->yplusx);
    fe_mul(r->T,q->xy2d,p->T);
    fe_add(t0,p->Z,p->Z);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_sub(r->Z,t0,r->T);
    fe_add(r->T,t0,r->T);
#else
    fe_ge_msub(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->xy2d,
              q->yplusx, q->yminusx);
#endif
}


/* ge p1p1 to p2 */
/*
r = p
*/

static void ge_p1p1_to_p2(ge_p2 *r,const ge_p1p1 *p)
{
#ifndef CURVED25519_ASM
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
#else
  fe_ge_to_p2(r->X, r->Y, r->Z, p->X, p->Y, p->Z, p->T);
#endif
}


/* ge p1p1 to p3 */

/*
r = p
*/

static WC_INLINE void ge_p1p1_to_p3(ge_p3 *r,const ge_p1p1 *p)
{
#ifndef CURVED25519_ASM
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
  fe_mul(r->T,p->X,p->Y);
#else
  fe_ge_to_p3(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T);
#endif
}


/* ge p2 0 */

static void ge_p2_0(ge_p2 *h)
{
  fe_0(h->X);
  fe_1(h->Y);
  fe_1(h->Z);
}


/* ge p2 dbl */

/*
r = 2 * p
*/

static WC_INLINE void ge_p2_dbl(ge_p1p1 *r,const ge_p2 *p)
{
#ifndef CURVED25519_ASM
    ge t0;
    fe_sq(r->X,p->X);
    fe_sq(r->Z,p->Y);
    fe_sq2(r->T,p->Z);
    fe_add(r->Y,p->X,p->Y);
    fe_sq(t0,r->Y);
    fe_add(r->Y,r->Z,r->X);
    fe_sub(r->Z,r->Z,r->X);
    fe_sub(r->X,t0,r->Y);
    fe_sub(r->T,r->T,r->Z);
#else
    fe_ge_dbl(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z);
#endif
}


/* ge p3 dble */

/*
r = 2 * p
*/

static void ge_p3_dbl(ge_p1p1 *r,const ge_p3 *p)
{
#ifndef CURVED25519_ASM
    ge_p2 q;
    ge_p3_to_p2(&q,p);
    ge_p2_dbl(r,&q);
#else
    fe_ge_dbl(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z);
#endif
}


/* ge p3 to cached */

/*
r = p
*/

#ifdef CURVED25519_ASM_64BIT
static const ge d2 = {
    -0x1429646bd94d0ea7, 0x00e0149a8283b156, 0x198e80f2eef3d130, 0x2406d9dc56dffce7,
};
#elif defined(CURVED25519_ASM_32BIT)
static const ge d2 = {
    0x26b2f159, -0x1429646c, -0x7d7c4eaa, 0x00e0149a, -0x110c2ed0, 0x198e80f2, 0x56dffce7, 0x2406d9dc,
};
#elif defined(CURVED25519_128BIT)
static const ge d2 = {
    0x69b9426b2f159, 0x35050762add7a, 0x3cf44c0038052, 0x6738cc7407977,
    0x2406d9dc56dff
};
#else
static const ge d2 = {
-21827239,-5839606,-30745221,13898782,229458,
15978800,-12551817,-6495438,29715968,9444199
} ;
#endif


static WC_INLINE void ge_p3_to_cached(ge_cached *r,const ge_p3 *p)
{
  fe_add(r->YplusX,p->Y,p->X);
  fe_sub(r->YminusX,p->Y,p->X);
  fe_copy(r->Z,p->Z);
  fe_mul(r->T2d,p->T,d2);
}


/* ge p3 to p2 */
/*
r = p
*/

#ifndef CURVED25519_ASM
static void ge_p3_to_p2(ge_p2 *r,const ge_p3 *p)
{
  fe_copy(r->X,p->X);
  fe_copy(r->Y,p->Y);
  fe_copy(r->Z,p->Z);
}
#endif


/* ge p3 tobytes */
void ge_p3_tobytes(unsigned char *s,const ge_p3 *h)
{
  ge recip;
  ge x;
  ge y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= (unsigned char)(fe_isnegative(x) << 7);
}


#ifndef CURVED25519_ASM
/* ge_precomp_0 */
static void ge_precomp_0(ge_precomp *h)
{
  fe_1(h->yplusx);
  fe_1(h->yminusx);
  fe_0(h->xy2d);
}
#endif


/* ge_sub */
/*
r = p - q
*/

static WC_INLINE void ge_sub(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
{
#ifndef CURVED25519_ASM
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->YminusX);
    fe_mul(r->Y,r->Y,q->YplusX);
    fe_mul(r->T,q->T2d,p->T);
    fe_mul(r->X,p->Z,q->Z);
    fe_add(t0,r->X,r->X);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_sub(r->Z,t0,r->T);
    fe_add(r->T,t0,r->T);
#else
    fe_ge_sub(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->Z, q->T2d,
              q->YplusX, q->YminusX);
#endif
}


/* ge tobytes */
void ge_tobytes(unsigned char *s,const ge_p2 *h)
{
  ge recip;
  ge x;
  ge y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= (unsigned char)(fe_isnegative(x) << 7);
}

#endif /* !ED25519_SMALL */
#endif /* HAVE_ED25519 */
