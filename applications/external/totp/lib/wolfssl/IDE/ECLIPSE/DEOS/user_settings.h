/* user_setting.h
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

#ifndef DEOS_USER_SETTINGS_H_
#define DEOS_USER_SETTINGS_H_

#ifdef __cplusplus
    extern "C" {
#endif

#if 0
    /* RTEMS */
    #define WOLFSSL_DEOS_RTEMS

    #include <stdio.h>
    #include <string.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <pthread.h>
    #include <stdint.h>
    #include <stddef.h>  /* for size_t */
    #include <stdlib.h> /* for malloc/free */

    #if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        #define BIG_ENDIAN_ORDER
    #else
        #undef  BIG_ENDIAN_ORDER
        #define LITTLE_ENDIAN_ORDER
    #endif
#else
    /* DEOS Native */
    #define WOLFSSL_DEOS

    #include <deos.h>
    #include <timeout.h>
    #include <socketapi.h>
    #include <lwip-socket.h>
    #include <mem.h>
    #include <string.h>
    #include <stdlib.h> /* for rand_r: pseudo-random number generator */
    #include <stdio.h>  /* for snprintf */
#endif

/* Porting */

/* adjust CURRENT_UNIX_TIMESTAMP to seconds since Jan 01 1970. (UTC)
You can get the current time from https://www.unixtimestamp.com/ */
#define CURRENT_UNIX_TIMESTAMP 1663020069

#define BENCH_EMBEDDED /* use kB instead of mB for embedded benchmarking */
#define WOLFSSL_IGNORE_FILE_WARN /* ignore warning for include of files not required */

/* Math */
#define USE_FAST_MATH
#define FP_MAX_BITS (4096*2)
#define SIZEOF_LONG_LONG 8
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* Wolf Single Precision Math */
#undef WOLFSSL_SP
#if 1
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
    #define WOLFSSL_SP_4096 /* Enable RSA/RH 4096-bit support */
    #define WOLFSSL_SP_384 /* Enable ECC 384-bit SECP384R1 support */

    //#define WOLFSSL_SP_CACHE_RESISTANT
    #define WOLFSSL_SP_MATH     /* only SP math - disables integer.c/tfm.c */
    //#define WOLFSSL_SP_MATH_ALL /* use SP math for all key sizes and curves */

    #define WOLFSSL_SP_NO_MALLOC
    //#define WOLFSSL_SP_DIV_32 /* do not use 64-bit divides */

    /* use smaller version of code */
    #define WOLFSSL_SP_SMALL
#endif

/* Algorithms */
#undef NO_RSA
#undef NO_DH
#define WOLFSSL_DH_CONST
#define HAVE_ECC
#define ECC_USER_CURVES
#define HAVE_ECC384

#if 1
    #define HAVE_CURVE25519
    #define HAVE_ED25519
    #define CURVED25519_SMALL
#endif

#undef NO_SHA
#undef NO_MD5
#undef NO_SHA256
#define HAVE_AESGCM
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512

/* TLS */
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_EXTENDED_MASTER
#define HAVE_ENCRYPT_THEN_MAC

//#define WOLFSSL_DTLS
//#define WOLFSSL_DTLS13
//#define WOLFSSL_NO_TLS12

#if 0
    /* TLS 1.3 */
    #define WOLFSSL_TLS13
    #define WC_RSA_PSS
    #define HAVE_HKDF
    #define HAVE_FFDHE_2048
    #define HAVE_FFDHE_3072
    #define HAVE_FFDHE_4096
#endif

/* wolfSentry */
#if 0
    #define WOLFSSL_WOLFSENTRY_HOOKS
    #define HAVE_EX_DATA
    #define HAVE_EX_DATA_CLEANUP_HOOKS
#endif

/* compatibility layer */
#if 0
    #define OPENSSL_EXTRA
#endif

/* Random */
#ifdef WOLFSSL_DEOS_RTEMS
    extern int rtems_wolf_seed(unsigned char* output, unsigned int sz);
    #define CUSTOM_RAND_GENERATE_SEED rtems_wolf_seed
#endif

/* custom heap handling */
#ifdef WOLFSSL_DEOS
    #define WOLFSSL_NO_MALLOC
    #define XMALLOC_OVERRIDE

    /* prototypes for user heap override functions */
    extern void *malloc_deos(size_t size);
    extern void  free_deos(void *ptr);
    extern void *realloc_deos(void *ptr, size_t size);

    #define XMALLOC(n, h, t)     malloc_deos(n)
    #define XFREE(p, h, t)       free_deos(p)
    #define XREALLOC(p, n, h, t) realloc_deos(p, n)
#endif

#if 0
    #define DEBUG_WOLFSSL
#endif

/* You can select none or all of the following tests
 * using #define instead of #undef.
 * By default, all four tests run*/
#define NO_CRYPT_TEST
#define NO_CRYPT_BENCHMARK
#undef NO_WOLFSSL_CLIENT
#undef NO_WOLFSSL_SERVER

/* file system has not been ported since it is a separate product */
#define NO_FILESYSTEM
#define NO_WOLFSSL_DIR
#define NO_WRITEV
#define NO_WRITE_TEMP_FILES

/* disable old protocols and algorithms */
#define NO_OLD_TLS
#define NO_PSK
#define NO_DSA
#define NO_RC4
#define NO_MD4

#define NO_PWDBASED

/* prevents from including multiple definition of main() */
#define NO_MAIN_DRIVER
#define NO_TESTSUITE_MAIN_DRIVER


#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif
