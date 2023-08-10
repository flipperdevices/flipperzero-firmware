/* bn.h
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

/* bn.h for openssl */

/*!
    \file wolfssl/openssl/bn.h
    \brief bn.h for openssl
*/


#ifndef WOLFSSL_BN_H_
#define WOLFSSL_BN_H_

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/wolfmath.h>

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct WOLFSSL_BIGNUM {
    int neg;        /* openssh deference */
    void *internal; /* our big num */
    mp_int mpi;
} WOLFSSL_BIGNUM;

#define WOLFSSL_BN_ULONG unsigned long
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
#define BN_ULONG         WOLFSSL_BN_ULONG
#endif

#ifndef WOLFSSL_MAX_BN_BITS
    #if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)
        /* SP implementation supports numbers of SP_INT_BITS bits. */
        #define WOLFSSL_MAX_BN_BITS     SP_INT_BITS
    #elif defined(USE_FAST_MATH)
        /* FP implementation support numbers up to FP_MAX_BITS / 2 bits. */
        #define WOLFSSL_MAX_BN_BITS     (FP_MAX_BITS / 2)
    #else
        #ifdef WOLFSSL_MYSQL_COMPATIBLE
            /* Integer maths is dynamic but we only go up to 8192 bits. */
            #define WOLFSSL_MAX_BN_BITS 8192
        #else
            /* Integer maths is dynamic but we only go up to 4096 bits. */
            #define WOLFSSL_MAX_BN_BITS 4096
        #endif
    #endif
#endif

#define WOLFSSL_BN_RAND_TOP_ANY     (-1)
#define WOLFSSL_BN_RAND_TOP_ONE     0
#define WOLFSSL_BN_RAND_TOP_TWO     1

#define WOLFSSL_BN_RAND_BOTTOM_ANY  0
#define WOLFSSL_BN_RAND_BOTTOM_ODD  1

#define WOLFSSL_BN_MAX_VAL          ((BN_ULONG)-1)

typedef struct WOLFSSL_BN_CTX   WOLFSSL_BN_CTX;
typedef struct WOLFSSL_BN_GENCB WOLFSSL_BN_GENCB;

WOLFSSL_API WOLFSSL_BN_CTX* wolfSSL_BN_CTX_new(void);
WOLFSSL_API void           wolfSSL_BN_CTX_init(WOLFSSL_BN_CTX* ctx);
WOLFSSL_API void           wolfSSL_BN_CTX_free(WOLFSSL_BN_CTX* ctx);

WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_new(void);
#if !defined(USE_INTEGER_HEAP_MATH) && !defined(HAVE_WOLF_BIGINT)
WOLFSSL_API void           wolfSSL_BN_init(WOLFSSL_BIGNUM* bn);
#endif
WOLFSSL_API void           wolfSSL_BN_free(WOLFSSL_BIGNUM* bn);
WOLFSSL_API void           wolfSSL_BN_clear_free(WOLFSSL_BIGNUM* bn);
WOLFSSL_API void           wolfSSL_BN_clear(WOLFSSL_BIGNUM* bn);


WOLFSSL_API int wolfSSL_BN_sub(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
                             const WOLFSSL_BIGNUM* b);
WOLFSSL_API int wolfSSL_BN_mul(WOLFSSL_BIGNUM *r, WOLFSSL_BIGNUM *a,
                               WOLFSSL_BIGNUM *b, WOLFSSL_BN_CTX *ctx);
WOLFSSL_API int wolfSSL_BN_div(WOLFSSL_BIGNUM* dv, WOLFSSL_BIGNUM* rem,
                   const WOLFSSL_BIGNUM* a, const WOLFSSL_BIGNUM* d,
                   WOLFSSL_BN_CTX* ctx);
#if defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA)
WOLFSSL_API int wolfSSL_BN_gcd(WOLFSSL_BIGNUM* r, WOLFSSL_BIGNUM* a,
                               WOLFSSL_BIGNUM* b, WOLFSSL_BN_CTX* ctx);
#endif
WOLFSSL_API int wolfSSL_BN_mod(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
                  const WOLFSSL_BIGNUM* b, const WOLFSSL_BN_CTX* c);
WOLFSSL_API int wolfSSL_BN_mod_exp(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
      const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx);
WOLFSSL_API int wolfSSL_BN_mod_mul(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
        const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx);
WOLFSSL_API const WOLFSSL_BIGNUM* wolfSSL_BN_value_one(void);


WOLFSSL_API int wolfSSL_BN_num_bytes(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_num_bits(const WOLFSSL_BIGNUM* bn);

WOLFSSL_API void wolfSSL_BN_zero(WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_one(WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_is_zero(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_is_one(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_is_odd(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_is_negative(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API int wolfSSL_BN_is_word(const WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w);

WOLFSSL_API int wolfSSL_BN_cmp(const WOLFSSL_BIGNUM* a, const WOLFSSL_BIGNUM* b);

WOLFSSL_API int wolfSSL_BN_bn2bin(const WOLFSSL_BIGNUM* bn, unsigned char* r);
WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_bin2bn(const unsigned char* str, int len,
                                              WOLFSSL_BIGNUM* ret);

WOLFSSL_API int wolfSSL_mask_bits(WOLFSSL_BIGNUM* bn, int n);

WOLFSSL_API int wolfSSL_BN_pseudo_rand(WOLFSSL_BIGNUM* bn, int bits, int top,
                                       int bottom);
WOLFSSL_API int wolfSSL_BN_rand_range(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *range);
WOLFSSL_API int wolfSSL_BN_rand(WOLFSSL_BIGNUM* bn, int bits, int top, int bottom);
WOLFSSL_API int wolfSSL_BN_is_bit_set(const WOLFSSL_BIGNUM* bn, int n);
WOLFSSL_API int wolfSSL_BN_hex2bn(WOLFSSL_BIGNUM** bn, const char* str);

WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_dup(const WOLFSSL_BIGNUM* bn);
WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_copy(WOLFSSL_BIGNUM* r,
                                            const WOLFSSL_BIGNUM* bn);

WOLFSSL_API int   wolfSSL_BN_dec2bn(WOLFSSL_BIGNUM** bn, const char* str);
WOLFSSL_API char* wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM* bn);

WOLFSSL_API int wolfSSL_BN_lshift(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* bn,
                                  int n);
WOLFSSL_API int wolfSSL_BN_add_word(WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w);
WOLFSSL_API int wolfSSL_BN_sub_word(WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w);
WOLFSSL_API int wolfSSL_BN_set_bit(WOLFSSL_BIGNUM* bn, int n);
WOLFSSL_API int wolfSSL_BN_clear_bit(WOLFSSL_BIGNUM* bn, int n);
WOLFSSL_API int wolfSSL_BN_set_word(WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w);
WOLFSSL_API WOLFSSL_BN_ULONG wolfSSL_BN_get_word(const WOLFSSL_BIGNUM* bn);

WOLFSSL_API int wolfSSL_BN_add(WOLFSSL_BIGNUM* r, WOLFSSL_BIGNUM* a,
                               WOLFSSL_BIGNUM* b);
WOLFSSL_API int wolfSSL_BN_mod_add(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
                                   const WOLFSSL_BIGNUM *b, const WOLFSSL_BIGNUM *m,
                                   WOLFSSL_BN_CTX *ctx);
WOLFSSL_API char *wolfSSL_BN_bn2hex(const WOLFSSL_BIGNUM* bn);
#if defined(WOLFSSL_KEY_GEN) && (!defined(NO_RSA) || !defined(NO_DH) || !defined(NO_DSA))
WOLFSSL_API int wolfSSL_BN_generate_prime_ex(
    WOLFSSL_BIGNUM* prime, int bits, int safe, const WOLFSSL_BIGNUM* add,
    const WOLFSSL_BIGNUM* rem, WOLFSSL_BN_GENCB* cb);
WOLFSSL_API int wolfSSL_BN_is_prime_ex(const WOLFSSL_BIGNUM *bn, int nbchecks,
                           WOLFSSL_BN_CTX *ctx, WOLFSSL_BN_GENCB *cb);
WOLFSSL_API WOLFSSL_BN_ULONG wolfSSL_BN_mod_word(const WOLFSSL_BIGNUM *bn,
                                                 WOLFSSL_BN_ULONG w);
#endif
#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
    WOLFSSL_API int wolfSSL_BN_print_fp(XFILE fp, const WOLFSSL_BIGNUM *bn);
#endif
WOLFSSL_API int wolfSSL_BN_rshift(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *bn,
                                  int n);
WOLFSSL_API WOLFSSL_BIGNUM *wolfSSL_BN_CTX_get(WOLFSSL_BN_CTX *ctx);
WOLFSSL_API void wolfSSL_BN_CTX_start(WOLFSSL_BN_CTX *ctx);
WOLFSSL_API WOLFSSL_BIGNUM *wolfSSL_BN_mod_inverse(
    WOLFSSL_BIGNUM *r,
    WOLFSSL_BIGNUM *a,
    const WOLFSSL_BIGNUM *n,
    WOLFSSL_BN_CTX *ctx);


#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

#define BN_RAND_TOP_ANY     WOLFSSL_BN_RAND_TOP_ANY
#define BN_RAND_TOP_ONE     WOLFSSL_BN_RAND_TOP_ONE
#define BN_RAND_TOP_TWO     WOLFSSL_BN_RAND_TOP_TWO

#define BN_RAND_BOTTOM_ANY  WOLFSSL_BN_RAND_BOTTOM_ANY
#define BN_RAND_BOTTOM_ODD  WOLFSSL_BN_RAND_BOTTOM_ODD

typedef WOLFSSL_BIGNUM   BIGNUM;
typedef WOLFSSL_BN_CTX   BN_CTX;
typedef WOLFSSL_BN_GENCB BN_GENCB;

#define BN_CTX_new        wolfSSL_BN_CTX_new
#define BN_CTX_init       wolfSSL_BN_CTX_init
#define BN_CTX_free       wolfSSL_BN_CTX_free

#define BN_new        wolfSSL_BN_new
#if !defined(USE_INTEGER_HEAP_MATH) && !defined(HAVE_WOLF_BIGINT)
#define BN_init       wolfSSL_BN_init
#endif
#define BN_free       wolfSSL_BN_free
#define BN_clear_free wolfSSL_BN_clear_free
#define BN_clear      wolfSSL_BN_clear

#define BN_num_bytes wolfSSL_BN_num_bytes
#define BN_num_bits  wolfSSL_BN_num_bits

#define BN_zero        wolfSSL_BN_zero
#define BN_one         wolfSSL_BN_one
#define BN_is_zero     wolfSSL_BN_is_zero
#define BN_is_one      wolfSSL_BN_is_one
#define BN_is_odd      wolfSSL_BN_is_odd
#define BN_is_negative wolfSSL_BN_is_negative
#define BN_is_word     wolfSSL_BN_is_word

#define BN_cmp    wolfSSL_BN_cmp

#define BN_bn2bin  wolfSSL_BN_bn2bin
#define BN_bin2bn  wolfSSL_BN_bin2bn

#define BN_mod       wolfSSL_BN_mod
#define BN_mod_exp   wolfSSL_BN_mod_exp
#define BN_mod_mul   wolfSSL_BN_mod_mul
#define BN_sub       wolfSSL_BN_sub
#define BN_mul       wolfSSL_BN_mul
#define BN_div       wolfSSL_BN_div
#define BN_gcd       wolfSSL_BN_gcd
#define BN_value_one wolfSSL_BN_value_one

#define BN_mask_bits wolfSSL_mask_bits

#define BN_pseudo_rand wolfSSL_BN_pseudo_rand
#define BN_rand        wolfSSL_BN_rand
#define BN_rand_range  wolfSSL_BN_rand_range
#define BN_is_bit_set  wolfSSL_BN_is_bit_set
#define BN_hex2bn      wolfSSL_BN_hex2bn

#define BN_dup  wolfSSL_BN_dup
#define BN_copy wolfSSL_BN_copy

#define BN_get_word wolfSSL_BN_get_word
#define BN_set_word wolfSSL_BN_set_word

#define BN_dec2bn wolfSSL_BN_dec2bn
#define BN_bn2dec wolfSSL_BN_bn2dec
#define BN_bn2hex wolfSSL_BN_bn2hex

#define BN_lshift wolfSSL_BN_lshift
#define BN_add_word wolfSSL_BN_add_word
#define BN_sub_word wolfSSL_BN_sub_word
#define BN_add wolfSSL_BN_add
#define BN_mod_add wolfSSL_BN_mod_add
#define BN_set_word wolfSSL_BN_set_word
#define BN_set_bit wolfSSL_BN_set_bit
#define BN_clear_bit wolfSSL_BN_clear_bit

#define BN_generate_prime_ex wolfSSL_BN_generate_prime_ex
#define BN_is_prime_ex wolfSSL_BN_is_prime_ex
#define BN_print_fp wolfSSL_BN_print_fp
#define BN_rshift wolfSSL_BN_rshift
#define BN_rshift1(r, a) wolfSSL_BN_rshift((r), (a), 1)
#define BN_mod_word wolfSSL_BN_mod_word

#define BN_CTX_get wolfSSL_BN_CTX_get
#define BN_CTX_start wolfSSL_BN_CTX_start

#define BN_mod_inverse wolfSSL_BN_mod_inverse

#define BN_set_flags(x1, x2) WC_DO_NOTHING

#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
#define BN_get_rfc2409_prime_768   wolfSSL_DH_768_prime
#define BN_get_rfc2409_prime_1024  wolfSSL_DH_1024_prime
#define BN_get_rfc3526_prime_1536  wolfSSL_DH_1536_prime
#define BN_get_rfc3526_prime_2048  wolfSSL_DH_2048_prime
#define BN_get_rfc3526_prime_3072  wolfSSL_DH_3072_prime
#define BN_get_rfc3526_prime_4096  wolfSSL_DH_4096_prime
#define BN_get_rfc3526_prime_6144  wolfSSL_DH_6144_prime
#define BN_get_rfc3526_prime_8192  wolfSSL_DH_8192_prime
#endif

#define BN_prime_checks 0

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */


#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFSSL__H_ */
