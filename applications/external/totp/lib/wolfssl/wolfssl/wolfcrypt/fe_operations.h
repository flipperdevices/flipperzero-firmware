/* fe_operations.h
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


#ifndef WOLF_CRYPT_FE_OPERATIONS_H
#define WOLF_CRYPT_FE_OPERATIONS_H

#include <wolfssl/wolfcrypt/settings.h>

#if defined(HAVE_CURVE25519) || defined(HAVE_ED25519)

#include <wolfssl/wolfcrypt/types.h>

#if defined(USE_INTEL_SPEEDUP) && !defined(NO_CURVED25519_X64)
    #define CURVED25519_X64
#elif defined(HAVE___UINT128_T) && !defined(NO_CURVED25519_128BIT)
    #define CURVED25519_128BIT
#endif

#if defined(CURVED25519_X64)
    #define CURVED25519_ASM_64BIT
    #define CURVED25519_ASM
#endif
#if defined(WOLFSSL_ARMASM)
    #ifdef __aarch64__
        #define CURVED25519_ASM_64BIT
    #else
        #define CURVED25519_ASM_32BIT
    #endif
    #define CURVED25519_ASM
#endif

/*
fe means field element.
Here the field is \Z/(2^255-19).
An element t, entries t[0]...t[9], represents the integer
t[0]+2^26 t[1]+2^51 t[2]+2^77 t[3]+2^102 t[4]+...+2^230 t[9].
Bounds on each t[i] vary depending on context.
*/

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(CURVE25519_SMALL) || defined(ED25519_SMALL)
    #define F25519_SIZE 32

    WOLFSSL_LOCAL void lm_copy(byte*, const byte*);
    WOLFSSL_LOCAL void lm_add(byte*, const byte*, const byte*);
    WOLFSSL_LOCAL void lm_sub(byte*, const byte*, const byte*);
    WOLFSSL_LOCAL void lm_neg(byte*,const byte*);
    WOLFSSL_LOCAL void lm_invert(byte*, const byte*);
    WOLFSSL_LOCAL void lm_mul(byte*,const byte*,const byte*);
#endif


#if !defined(FREESCALE_LTC_ECC)
WOLFSSL_LOCAL void fe_init(void);

WOLFSSL_LOCAL int  curve25519(byte * q, const byte * n, const byte * p);
#endif

/* default to be faster but take more memory */
#if !defined(CURVE25519_SMALL) || !defined(ED25519_SMALL)

#ifdef CURVED25519_ASM_64BIT
    typedef sword64  fe[4];
#elif defined(CURVED25519_ASM_32BIT)
    typedef sword32  fe[8];
#elif defined(CURVED25519_128BIT)
    typedef sword64  fe[5];
#else
    typedef sword32  fe[10];
#endif

WOLFSSL_LOCAL void fe_copy(fe h,const fe f);
WOLFSSL_LOCAL void fe_add(fe h,const fe f,const fe g);
WOLFSSL_LOCAL void fe_neg(fe h,const fe f);
WOLFSSL_LOCAL void fe_sub(fe h,const fe f,const fe g);
WOLFSSL_LOCAL void fe_invert(fe out,const fe z);
WOLFSSL_LOCAL void fe_mul(fe h,const fe f,const fe g);


/* Based On Daniel J Bernstein's curve25519 and ed25519 Public Domain ref10
   work. */

WOLFSSL_LOCAL void fe_0(fe h);
WOLFSSL_LOCAL void fe_1(fe h);
WOLFSSL_LOCAL int  fe_isnonzero(const fe f);
WOLFSSL_LOCAL int  fe_isnegative(const fe f);
WOLFSSL_LOCAL void fe_tobytes(unsigned char *s,const fe h);
WOLFSSL_LOCAL void fe_sq(fe h,const fe f);
WOLFSSL_LOCAL void fe_sq2(fe h,const fe f);
WOLFSSL_LOCAL void fe_frombytes(fe h,const unsigned char *s);
WOLFSSL_LOCAL void fe_cswap(fe f, fe g, int b);
WOLFSSL_LOCAL void fe_mul121666(fe h,fe f);
WOLFSSL_LOCAL void fe_cmov(fe f, const fe g, int b);
WOLFSSL_LOCAL void fe_pow22523(fe out,const fe z);

/* 64 type needed for SHA512 */
WOLFSSL_LOCAL word64 load_3(const unsigned char *in);
WOLFSSL_LOCAL word64 load_4(const unsigned char *in);

#ifdef CURVED25519_ASM
WOLFSSL_LOCAL void fe_ge_to_p2(fe rx, fe ry, fe rz, const fe px, const fe py,
                               const fe pz, const fe pt);
WOLFSSL_LOCAL void fe_ge_to_p3(fe rx, fe ry, fe rz, fe rt, const fe px,
                               const fe py, const fe pz, const fe pt);
WOLFSSL_LOCAL void fe_ge_dbl(fe rx, fe ry, fe rz, fe rt, const fe px,
                             const fe py, const fe pz);
WOLFSSL_LOCAL void fe_ge_madd(fe rx, fe ry, fe rz, fe rt, const fe px,
                              const fe py, const fe pz, const fe pt,
                              const fe qxy2d, const fe qyplusx,
                              const fe qyminusx);
WOLFSSL_LOCAL void fe_ge_msub(fe rx, fe ry, fe rz, fe rt, const fe px,
                              const fe py, const fe pz, const fe pt,
                              const fe qxy2d, const fe qyplusx,
                              const fe qyminusx);
WOLFSSL_LOCAL void fe_ge_add(fe rx, fe ry, fe rz, fe rt, const fe px,
                             const fe py, const fe pz, const fe pt, const fe qz,
                             const fe qt2d, const fe qyplusx,
                             const fe qyminusx);
WOLFSSL_LOCAL void fe_ge_sub(fe rx, fe ry, fe rz, fe rt, const fe px,
                             const fe py, const fe pz, const fe pt, const fe qz,
                             const fe qt2d, const fe qyplusx,
                             const fe qyminusx);
WOLFSSL_LOCAL void fe_cmov_table(fe* r, fe* base, signed char b);
#endif /* CURVED25519_ASM */
#endif /* !CURVE25519_SMALL || !ED25519_SMALL */

/* Use less memory and only 32bit types or less, but is slower
   Based on Daniel Beer's public domain work. */
#if defined(CURVE25519_SMALL) || defined(ED25519_SMALL)
static const byte c25519_base_x[F25519_SIZE] = {9};
static const byte f25519_zero[F25519_SIZE]   = {0};
static const byte f25519_one[F25519_SIZE]    = {1};
static const byte fprime_zero[F25519_SIZE]   = {0};
static const byte fprime_one[F25519_SIZE]    = {1};

WOLFSSL_LOCAL void fe_load(byte *x, word32 c);
WOLFSSL_LOCAL void fe_normalize(byte *x);
WOLFSSL_LOCAL void fe_inv__distinct(byte *r, const byte *x);

/* Conditional copy. If condition == 0, then zero is copied to dst. If
 * condition == 1, then one is copied to dst. Any other value results in
 * undefined behavior.
 */
WOLFSSL_LOCAL void fe_select(byte *dst, const byte *zero, const byte *one,
                   byte condition);

/* Multiply a point by a small constant. The two pointers are not
 * required to be distinct.
 *
 * The constant must be less than 2^24.
 */
WOLFSSL_LOCAL void fe_mul_c(byte *r, const byte *a, word32 b);
WOLFSSL_LOCAL void fe_mul__distinct(byte *r, const byte *a, const byte *b);

/* Compute one of the square roots of the field element, if the element
 * is square. The other square is -r.
 *
 * If the input is not square, the returned value is a valid field
 * element, but not the correct answer. If you don't already know that
 * your element is square, you should square the return value and test.
 */
WOLFSSL_LOCAL void fe_sqrt(byte *r, const byte *x);

/* Conditional copy. If condition == 0, then zero is copied to dst. If
 * condition == 1, then one is copied to dst. Any other value results in
 * undefined behavior.
 */
WOLFSSL_LOCAL void fprime_select(byte *dst, const byte *zero, const byte *one,
                                         byte condition);
WOLFSSL_LOCAL void fprime_add(byte *r, const byte *a, const byte *modulus);
WOLFSSL_LOCAL void fprime_sub(byte *r, const byte *a, const byte *modulus);
WOLFSSL_LOCAL void fprime_mul(byte *r, const byte *a, const byte *b,
                                      const byte *modulus);
WOLFSSL_LOCAL void fprime_copy(byte *x, const byte *a);

#endif /* CURVE25519_SMALL || ED25519_SMALL */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_CURVE25519 || HAVE_ED25519 */

#endif /* WOLF_CRYPT_FE_OPERATIONS_H */
