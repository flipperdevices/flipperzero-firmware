/* wolfmath.h
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

/*
DESCRIPTION
This library provides big integer math functions.

*/
#ifndef __WOLFMATH_H__
#define __WOLFMATH_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_PUBLIC_MP
    #define MP_API   WOLFSSL_API
#else
    #define MP_API   WOLFSSL_LOCAL
#endif


#if defined(USE_FAST_MATH)
    #include <wolfssl/wolfcrypt/tfm.h>
#elif defined(USE_INTEGER_HEAP_MATH)
    #include <wolfssl/wolfcrypt/integer.h>
#else
    #include <wolfssl/wolfcrypt/sp_int.h>
#endif

#if !defined(NO_BIG_INT) || defined(WOLFSSL_SP_MATH)
    #include <wolfssl/wolfcrypt/random.h>
#endif

#ifndef MIN
   #define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
   #define MAX(x,y) ((x)>(y)?(x):(y))
#endif

/* timing resistance array */
#if !defined(WC_NO_CACHE_RESISTANT) && \
    ((defined(HAVE_ECC) && defined(ECC_TIMING_RESISTANT)) || \
     (defined(USE_FAST_MATH) && defined(TFM_TIMING_RESISTANT)))

    extern const wc_ptr_t wc_off_on_addr[2];
#endif

#if !defined(NO_BIG_INT) || defined(WOLFSSL_SP_MATH)
/* common math functions */
MP_API int get_digit_count(const mp_int* a);
MP_API mp_digit get_digit(const mp_int* a, int n);
MP_API int get_rand_digit(WC_RNG* rng, mp_digit* d);
WOLFSSL_LOCAL void mp_reverse(unsigned char *s, int len);

WOLFSSL_API int mp_cond_copy(mp_int* a, int copy, mp_int* b);
WOLFSSL_API int mp_rand(mp_int* a, int digits, WC_RNG* rng);
#endif

#define WC_TYPE_HEX_STR 1
#define WC_TYPE_UNSIGNED_BIN 2
#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    #define WC_TYPE_BLACK_KEY 3
#endif

#if defined(HAVE_ECC) || defined(WOLFSSL_EXPORT_INT)
WOLFSSL_API int wc_export_int(mp_int* mp, byte* buf, word32* len,
    word32 keySz, int encType);
#endif

#ifdef HAVE_WOLF_BIGINT
    #if !defined(WOLF_BIGINT_DEFINED)
        /* raw big integer */
        typedef struct WC_BIGINT {
            byte*   buf;
            word32  len;
            void*   heap;
        } WC_BIGINT;
        #define WOLF_BIGINT_DEFINED
    #endif

    WOLFSSL_LOCAL void wc_bigint_init(WC_BIGINT* a);
    WOLFSSL_LOCAL int wc_bigint_alloc(WC_BIGINT* a, word32 sz);
    WOLFSSL_LOCAL int wc_bigint_from_unsigned_bin(WC_BIGINT* a, const byte* in, word32 inlen);
    WOLFSSL_LOCAL int wc_bigint_to_unsigned_bin(WC_BIGINT* a, byte* out, word32* outlen);
    WOLFSSL_LOCAL void wc_bigint_zero(WC_BIGINT* a);
    WOLFSSL_LOCAL void wc_bigint_free(WC_BIGINT* a);

    WOLFSSL_LOCAL int wc_mp_to_bigint(mp_int* src, WC_BIGINT* dst);
    WOLFSSL_LOCAL int wc_mp_to_bigint_sz(mp_int* src, WC_BIGINT* dst, word32 sz);
    WOLFSSL_LOCAL int wc_bigint_to_mp(WC_BIGINT* src, mp_int* dst);
#endif /* HAVE_WOLF_BIGINT */


#ifdef HAVE_WC_INTROSPECTION
    WOLFSSL_API const char *wc_GetMathInfo(void);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* __WOLFMATH_H__ */
