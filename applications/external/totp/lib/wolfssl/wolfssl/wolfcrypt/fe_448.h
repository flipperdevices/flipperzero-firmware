/* fe448_448.h
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


#ifndef WOLF_CRYPT_FE_448_H
#define WOLF_CRYPT_FE_448_H

#include <wolfssl/wolfcrypt/settings.h>

#if defined(HAVE_CURVE448) || defined(HAVE_ED448)

#include <wolfssl/wolfcrypt/types.h>

#if defined(HAVE___UINT128_T) && !defined(NO_CURVED448_128BIT)
    #define CURVED448_128BIT
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* default to be faster but take more memory */
#if !defined(CURVE448_SMALL) && !defined(ED448_SMALL)

#if defined(CURVED448_128BIT)
    typedef sword64 fe448;
    #ifndef WOLFSSL_UINT128_T_DEFINED
        #ifdef __SIZEOF_INT128__
            typedef __uint128_t uint128_t;
            typedef __int128_t int128_t;
            typedef __uint128_t word128;
            typedef __int128_t sword128;
        #else
            typedef unsigned long uint128_t __attribute__ ((mode(TI)));
            typedef long int128_t __attribute__ ((mode(TI)));
            typedef uint128_t word128;
            typedef int128_t sword128;
        #endif
        #define WOLFSSL_UINT128_T_DEFINED
    #endif
#else
    typedef sword32 fe448;
#endif

WOLFSSL_LOCAL void fe448_init(void);
WOLFSSL_LOCAL int  curve448(byte* r, const byte* n, const byte* a);

#if !defined(CURVED448_128BIT)
WOLFSSL_LOCAL void fe448_reduce(fe448*);
#else
#define fe448_reduce(a) WC_DO_NOTHING
#endif
WOLFSSL_LOCAL void fe448_neg(fe448* r, const fe448* a);
WOLFSSL_LOCAL void fe448_add(fe448* r, const fe448* a, const fe448* b);
WOLFSSL_LOCAL void fe448_sub(fe448* r, const fe448* a, const fe448* b);
WOLFSSL_LOCAL void fe448_mul(fe448* r, const fe448* a, const fe448* b);
WOLFSSL_LOCAL void fe448_sqr(fe448* r, const fe448* a);
WOLFSSL_LOCAL void fe448_mul39081(fe448* r, const fe448* a);
WOLFSSL_LOCAL void fe448_invert(fe448* r, const fe448* a);

WOLFSSL_LOCAL void fe448_0(fe448* a);
WOLFSSL_LOCAL void fe448_1(fe448* a);
WOLFSSL_LOCAL void fe448_copy(fe448* d, const fe448* a);
WOLFSSL_LOCAL int  fe448_isnonzero(const fe448* a);
WOLFSSL_LOCAL int  fe448_isnegative(const fe448* a);

WOLFSSL_LOCAL void fe448_from_bytes(fe448* r, const unsigned char* b);
WOLFSSL_LOCAL void fe448_to_bytes(unsigned char* b, const fe448* a);

WOLFSSL_LOCAL void fe448_cmov(fe448* a, const fe448* b, int c);
WOLFSSL_LOCAL void fe448_pow_2_446_222_1(fe448* r, const fe448* a);

#else

WOLFSSL_LOCAL void fe448_init(void);
WOLFSSL_LOCAL int  curve448(byte* r, const byte* n, const byte* a);

#define fe448_reduce(a) WC_DO_NOTHING

WOLFSSL_LOCAL void fe448_neg(word8* r, const word8* a);
WOLFSSL_LOCAL void fe448_add(word8* r, const word8* a, const word8* b);
WOLFSSL_LOCAL void fe448_sub(word8* r, const word8* a, const word8* b);
WOLFSSL_LOCAL void fe448_mul(word8* r, const word8* a, const word8* b);
WOLFSSL_LOCAL void fe448_sqr(word8* r, const word8* a);
WOLFSSL_LOCAL void fe448_mul39081(word8* r, const word8* a);
WOLFSSL_LOCAL void fe448_invert(word8* r, const word8* a);

WOLFSSL_LOCAL void fe448_norm(byte *a);
WOLFSSL_LOCAL void fe448_copy(word8* d, const word8* a);
WOLFSSL_LOCAL int  fe448_isnonzero(const word8* a);

WOLFSSL_LOCAL void fe448_cmov(word8* a, const word8* b, int c);
WOLFSSL_LOCAL void fe448_pow_2_446_222_1(word8* r, const word8* a);

#endif /* !CURVE448_SMALL || !ED448_SMALL */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_CURVE448 || HAVE_ED448 */

#endif /* WOLF_CRYPT_FE_448_H */
