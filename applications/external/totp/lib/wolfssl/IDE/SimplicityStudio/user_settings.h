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

/* Template based on examples/config/user_settings_template.h, but modified to
 * include `WOLFSSL_SILABS_SE_ACCEL` and tune for ARM Cortex M. */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Silicon Labs ERF32 Hardware Acceleration */
#define WOLFSSL_SILABS_SE_ACCEL

/* ------------------------------------------------------------------------- */
/* Platform */
/* ------------------------------------------------------------------------- */
#define WOLFSSL_GENERAL_ALIGNMENT 4
#define SIZEOF_LONG_LONG 8
#define HAVE_STRINGS_H

/* Use FreeRTOS */
#if 1
    #define FREERTOS
#else
    /* disable mutex locking */
    #define SINGLE_THREADED
#endif

/* reduce stack use. For variables over 100 bytes allocate from heap */
#define WOLFSSL_SMALL_STACK

/* Disable the built-in socket support and use the IO callbacks.
 * Set IO callbacks with wolfSSL_CTX_SetIORecv/wolfSSL_CTX_SetIOSend
 */
#define WOLFSSL_USER_IO

/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
/* Math Choices: SP (preferred), TFM or Normal (heap) */
#if 1
    /* Wolf Single Precision Math */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
    //#define WOLFSSL_SP_4096 /* Enable RSA/RH 4096-bit support */
    //#define WOLFSSL_SP_384 /* Enable ECC 384-bit SECP384R1 support */

    //#define WOLFSSL_SP_CACHE_RESISTANT
    //#define WOLFSSL_SP_MATH     /* only SP math - disables integer.c/tfm.c */
    #define WOLFSSL_SP_MATH_ALL /* use SP math for all key sizes and curves */

    //#define WOLFSSL_SP_NO_MALLOC
    //#define WOLFSSL_SP_DIV_32 /* do not use 64-bit divides */

    /* use smaller version of code */
    #define WOLFSSL_SP_SMALL

    /* SP Assembly Speedups - specific to chip type */
    #define WOLFSSL_SP_ASM
    #define WOLFSSL_SP_ARM_CORTEX_M_ASM
#elif 1
    /* Fast Math (tfm.c) (stack based and timing resistant) */
    #define USE_FAST_MATH
    #define TFM_TIMING_RESISTANT
#else
    /* Normal (integer.c) (heap based, not timing resistant) - not recommended*/
    #define USE_INTEGER_HEAP_MATH
#endif


/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
/* RSA */
#undef NO_RSA
#if 1
    #ifdef USE_FAST_MATH
        /* Maximum math bits (Max RSA key bits * 2) */
        #define FP_MAX_BITS 4096
    #endif

    /* half as much memory but twice as slow */
    //#define RSA_LOW_MEM

    /* Enables blinding mode, to prevent timing attacks */
    #define WC_RSA_BLINDING

    /* RSA PSS Support */
    #define WC_RSA_PSS
#else
    #define NO_RSA
#endif

/* DH */
#undef  NO_DH
#if 1
    /* Use table for DH instead of -lm (math) lib dependency */
    #if 1
        #define WOLFSSL_DH_CONST
        #define HAVE_FFDHE_2048
        //#define HAVE_FFDHE_4096
        //#define HAVE_FFDHE_6144
        //#define HAVE_FFDHE_8192
    #endif
#else
    #define NO_DH
#endif

/* ECC */
#undef HAVE_ECC
#if 1
    #define HAVE_ECC

    /* Manually define enabled curves */
    #define ECC_USER_CURVES

    #ifdef ECC_USER_CURVES
        /* Manual Curve Selection */
        //#define HAVE_ECC192
        //#define HAVE_ECC224
        #undef NO_ECC256
        //#define HAVE_ECC384
        //#define HAVE_ECC521
    #endif

    /* Fixed point cache (speeds repeated operations against same private key) */
    //#define FP_ECC
    #ifdef FP_ECC
        /* Bits / Entries */
        #define FP_ENTRIES  2
        #define FP_LUT      4
    #endif

    /* Optional ECC calculation method */
    /* Note: doubles heap usage, but slightly faster */
    #define ECC_SHAMIR

    /* Reduces heap usage, but slower */
    #define ECC_TIMING_RESISTANT

    /* Compressed ECC Key Support */
    //#define HAVE_COMP_KEY

    /* Use alternate ECC size for ECC math */
    #ifdef USE_FAST_MATH
        /* MAX ECC BITS = ROUND8(MAX ECC) * 2 */
        #if defined(NO_RSA) && defined(NO_DH)
            /* Custom fastmath size if not using RSA/DH */
            #define FP_MAX_BITS     (256 * 2)
        #else
            /* use heap allocation for ECC points */
            #define ALT_ECC_SIZE

            /* wolfSSL will compute the FP_MAX_BITS_ECC, but it can be overridden */
            //#define FP_MAX_BITS_ECC (256 * 2)
        #endif

        /* Speedups specific to curve */
        #ifndef NO_ECC256
            #define TFM_ECC256
        #endif
    #endif
#endif


/* AES */
#undef NO_AES
#if 1
    #define HAVE_AES_CBC

    /* GCM Method: GCM_TABLE_4BIT, GCM_SMALL, GCM_WORD32 or GCM_TABLE */
    #define HAVE_AESGCM
    #define GCM_SMALL

    //#define WOLFSSL_AES_DIRECT
    //#define HAVE_AES_ECB
    //#define WOLFSSL_AES_COUNTER
    //#define HAVE_AESCCM
#else
    #define NO_AES
#endif


/* DES3 */
#undef NO_DES3
#if 0
#else
    #define NO_DES3
#endif

/* ChaCha20 / Poly1305 */
#undef HAVE_CHACHA
#undef HAVE_POLY1305
#if 1
    #define HAVE_CHACHA
    #define HAVE_POLY1305

    /* Needed for Poly1305 */
    #define HAVE_ONE_TIME_AUTH
#endif

/* Ed25519 / Curve25519 */
#undef HAVE_CURVE25519
#undef HAVE_ED25519
#if 0
    #define HAVE_CURVE25519
    #define HAVE_ED25519 /* ED25519 Requires SHA512 */

    /* Optionally use small math (less flash usage, but much slower) */
    #if 1
        #define CURVED25519_SMALL
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Hashing */
/* ------------------------------------------------------------------------- */
/* Sha */
#undef NO_SHA
#if 1
    /* 1k smaller, but 25% slower */
    //#define USE_SLOW_SHA
#else
    #define NO_SHA
#endif

/* Sha256 */
#undef NO_SHA256
#if 1
    /* not unrolled - ~2k smaller and ~25% slower */
    //#define USE_SLOW_SHA256

    /* Sha224 */
    #if 0
        #define WOLFSSL_SHA224
    #endif
#else
    #define NO_SHA256
#endif

/* Sha512 */
#undef WOLFSSL_SHA512
#if 0
    #define WOLFSSL_SHA512

    /* Sha384 */
    #undef  WOLFSSL_SHA384
    #if 0
        #define WOLFSSL_SHA384
    #endif

    /* over twice as small, but 50% slower */
    //#define USE_SLOW_SHA512
#endif

/* Sha3 */
#undef WOLFSSL_SHA3
#if 0
    #define WOLFSSL_SHA3
#endif

/* MD5 */
#undef  NO_MD5
#if 0

#else
    #define NO_MD5
#endif

/* HKDF */
#undef HAVE_HKDF
#if 1
    #define HAVE_HKDF
#endif

/* CMAC */
#undef WOLFSSL_CMAC
#if 0
    #define WOLFSSL_CMAC
#endif


/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
/* Use reduced benchmark / test sizes */
#define BENCH_EMBEDDED

/* Use test buffers from array (not filesystem) */
#define USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_2048

/* ------------------------------------------------------------------------- */
/* Debugging */
/* ------------------------------------------------------------------------- */

#undef DEBUG_WOLFSSL
#undef NO_ERROR_STRINGS
#if 0
    #define DEBUG_WOLFSSL
#else
    #if 0
        #define NO_ERROR_STRINGS
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Memory */
/* ------------------------------------------------------------------------- */

/* Override Memory API's */
#if 0
    #define XMALLOC_OVERRIDE

    /* prototypes for user heap override functions */
    /* Note: Realloc only required for normal math */
    #include <stddef.h>  /* for size_t */
    extern void *myMalloc(size_t n, void* heap, int type);
    extern void myFree(void *p, void* heap, int type);
    extern void *myRealloc(void *p, size_t n, void* heap, int type);

    #define XMALLOC(n, h, t)     myMalloc(n, h, t)
    #define XFREE(p, h, t)       myFree(p, h, t)
    #define XREALLOC(p, n, h, t) myRealloc(p, n, h, t)
#endif

#if 0
    /* Static memory requires fast math */
    #define WOLFSSL_STATIC_MEMORY

    /* Disable fallback malloc/free */
    #define WOLFSSL_NO_MALLOC
    #if 1
        #define WOLFSSL_MALLOC_CHECK /* trap malloc failure */
    #endif
#endif

/* Memory callbacks */
#if 0
    #undef  USE_WOLFSSL_MEMORY
    #define USE_WOLFSSL_MEMORY

    /* Use this to measure / print heap usage */
    #if 0
        #define WOLFSSL_TRACK_MEMORY
        #define WOLFSSL_DEBUG_MEMORY
    #endif
#else
    #ifndef WOLFSSL_STATIC_MEMORY
        #define NO_WOLFSSL_MEMORY
        /* Otherwise we will use stdlib malloc, free and realloc */
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Port */
/* ------------------------------------------------------------------------- */

/* Override Current Time */
#if 0
    /* Allows custom "custom_time()" function to be used for benchmark */
    #define WOLFSSL_USER_CURRTIME
    #define WOLFSSL_GMTIME
    #define USER_TICKS
    extern unsigned long my_time(unsigned long* timer);
    #define XTIME my_time
#endif


/* ------------------------------------------------------------------------- */
/* RNG */
/* ------------------------------------------------------------------------- */

/* Choose RNG method */
#if 1
    /* Custom Seed Source */
    #if 0
        /* Size of returned HW RNG value */
        #define CUSTOM_RAND_TYPE      unsigned int
        extern unsigned int my_rng_seed_gen(void);
        #undef  CUSTOM_RAND_GENERATE
        #define CUSTOM_RAND_GENERATE  my_rng_seed_gen
    #endif

    /* Use built-in P-RNG (SHA256 based) with HW RNG */
    /* P-RNG + HW RNG (P-RNG is ~8K) */
    #undef  HAVE_HASHDRBG
    #define HAVE_HASHDRBG
#else
    #undef  WC_NO_HASHDRBG
    #define WC_NO_HASHDRBG

    /* Bypass P-RNG and use only HW RNG */
    extern int my_rng_gen_block(unsigned char* output, unsigned int sz);
    #undef  CUSTOM_RAND_GENERATE_BLOCK
    #define CUSTOM_RAND_GENERATE_BLOCK  my_rng_gen_block
#endif


/* ------------------------------------------------------------------------- */
/* Custom Standard Lib */
/* ------------------------------------------------------------------------- */
/* Allows override of all standard library functions */
#undef STRING_USER
#if 0
    #define STRING_USER

    #include <string.h>

    #define USE_WOLF_STRSEP
    #define XSTRSEP(s1,d)     wc_strsep((s1),(d))

    #define USE_WOLF_STRTOK
    #define XSTRTOK(s1,d,ptr) wc_strtok((s1),(d),(ptr))

    #define XSTRNSTR(s1,s2,n) mystrnstr((s1),(s2),(n))

    #define XMEMCPY(d,s,l)    memcpy((d),(s),(l))
    #define XMEMSET(b,c,l)    memset((b),(c),(l))
    #define XMEMCMP(s1,s2,n)  memcmp((s1),(s2),(n))
    #define XMEMMOVE(d,s,l)   memmove((d),(s),(l))

    #define XSTRLEN(s1)       strlen((s1))
    #define XSTRNCPY(s1,s2,n) strncpy((s1),(s2),(n))
    #define XSTRSTR(s1,s2)    strstr((s1),(s2))

    #define XSTRNCMP(s1,s2,n)     strncmp((s1),(s2),(n))
    #define XSTRNCAT(s1,s2,n)     strncat((s1),(s2),(n))
    #define XSTRNCASECMP(s1,s2,n) strncasecmp((s1),(s2),(n))

    #define XSNPRINTF snprintf
#endif



/* ------------------------------------------------------------------------- */
/* Enable Features */
/* ------------------------------------------------------------------------- */

#define WOLFSSL_TLS13
#define WOLFSSL_OLD_PRIME_CHECK /* Use faster DH prime checking */
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define WOLFSSL_BASE64_ENCODE
#define WOLFSSL_PUB_PEM_TO_DER

//#define WOLFSSL_KEY_GEN /* For RSA Key gen only */
//#define KEEP_PEER_CERT
//#define HAVE_COMP_KEY

/* TLS Session Cache */
#if 0
    #define SMALL_SESSION_CACHE
#else
    #define NO_SESSION_CACHE
#endif


/* ------------------------------------------------------------------------- */
/* Disable Features */
/* ------------------------------------------------------------------------- */
//#define NO_WOLFSSL_SERVER
//#define NO_WOLFSSL_CLIENT
//#define NO_CRYPT_TEST
//#define NO_CRYPT_BENCHMARK
//#define WOLFCRYPT_ONLY

/* do not warm when file is included to be built and not required to be */
#define WOLFSSL_IGNORE_FILE_WARN

/* In-lining of misc.c functions */
/* If defined, must include wolfcrypt/src/misc.c in build */
/* Slower, but about 1k smaller */
//#define NO_INLINE

#define NO_FILESYSTEM
#define NO_WRITEV
#define NO_MAIN_DRIVER
#define NO_DEV_RANDOM

#define NO_OLD_TLS
#define NO_PSK

#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_PWDBASED
//#define NO_CODING
//#define NO_ASN_TIME
//#define NO_CERTS
//#define NO_SIG_WRAPPER

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
