/* user_settings.h
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


#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#undef  SINGLE_THREADED
#define SINGLE_THREADED

#undef WOLFSSL_STATIC_MEMORY
#define WOLFSSL_STATIC_MEMORY

#undef WOLFSSL_NO_MALLOC
#define WOLFSSL_NO_MALLOC

#undef  NO_FILESYSTEM
#define NO_FILESYSTEM

/* WolfCrypt Only (no TLS) */
#define WOLFCRYPT_ONLY

/* 16 bit */
#define WC_16BIT_CPU

/* Endianness - defaults to little endian */
#ifdef __BIG_ENDIAN__
    #define BIG_ENDIAN_ORDER
#endif

/* Expose the math mp_ API's */
#define WOLFSSL_PUBLIC_MP

/* Use single precision math only */
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_NO_MALLOC
#define WOLFSSL_SP_SMALL
#define WOLFSSL_SP_NO_DYN_STACK
#define WOLFSSL_HAVE_SP_ECC
#define SP_WORD_SIZE 32
#define FP_MAX_BITS (2*256)

#define USE_SLOW_SHA
#define USE_SLOW_SHA256
#define USE_SLOW_SHA512

/* Enable Timing Resistance */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT

/* Enable ECC */
#define HAVE_ECC
#define ECC_USER_CURVES /* Only 256-Bit Curves */
//#define ECC_SHAMIR

/* Optional Feature Disables */
#define NO_SIG_WRAPPER
//#define NO_ECC_KEY_EXPORT
//#define NO_ECC_DHE
//#define NO_ECC_SIGN
//#define NO_ECC_VERIFY

/* Disable Algorithms */
#define NO_AES
#define NO_AES_CBC
#define NO_DES3
#define NO_DSA
#define NO_RSA
#define NO_DH
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_SHA
#define NO_PWDBASED
#define NO_PKCS12
#define NO_PKCS8

#define HAVE_CHACHA
#define HAVE_POLY1305
#define SIZEOF_LONG_LONG 8

/* Disable Features */
//#define NO_ASN
//#define NO_CERTS
//#define NO_WOLFSSL_MEMORY
#define WOLFSSL_NO_PEM
//#define NO_CODING
#define NO_PSK
#ifndef DEBUG_WOLFSSL
    #define DEBUG_WOLFSSL
    #define NO_ERROR_STRINGS
#endif

#define CUSTOM_RAND_TYPE      unsigned int
extern unsigned int msp430_rnd(void);
#undef  CUSTOM_RAND_GENERATE
#define CUSTOM_RAND_GENERATE  msp430_rnd

extern unsigned int msp430_time(long *x);
#undef XTIME
#define XTIME msp430_time

#undef  NO_CRYPT_TEST
#define NO_CRYPT_TEST

#undef  NO_CRYPT_BENCHMARK
#define NO_CRYPT_BENCHMARK

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
