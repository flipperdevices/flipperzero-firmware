#ifndef _EMBOS_USER_SETTINGS_H_
#define _EMBOS_USER_SETTINGS_H_

/* ------------------------------------------------------------------------- */
/* Platform */
/* ------------------------------------------------------------------------- */
#undef NO_INLINE
#define NO_INLINE

#undef  SINGLE_THREADED
#define SINGLE_THREADED

#undef  WOLFSSL_SMALL_STACK
#define WOLFSSL_SMALL_STACK


/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
#undef NO_64BIT
#define NO_64BIT

#ifndef SIZEOF_LONG
#define SIZEOF_LONG 4
#endif
#ifndef SIZEOF_LONG_LONG
#define SIZEOF_LONG_LONG 8
#endif

#undef  USE_FAST_MATH
#define USE_FAST_MATH

#ifdef USE_FAST_MATH
    #undef  TFM_TIMING_RESISTANT
//    #define TFM_TIMING_RESISTANT

    /* Optimizations (TFM_ARM, TFM_ASM or none) */
    #undef  TFM_ARM
//    #define TFM_ARM

    #undef  TFM_ASM
    //#define TFM_ASM
#endif

/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
#undef WOLFCRYPT_ONLY
#define WOLFCRYPT_ONLY

/* ECC */
#if 1
    #undef  HAVE_ECC
    #define HAVE_ECC

    /* Manually define enabled curves */
    #undef  ECC_USER_CURVES
    #define ECC_USER_CURVES

    #define HAVE_ECC192
    #define HAVE_ECC224
    #undef NO_ECC256
    #define HAVE_ECC384
    #define HAVE_ECC521

    /* Fixed point cache (speeds repeated operations against same private key) */
    #undef  FP_ECC
    //#define FP_ECC
    #ifdef FP_ECC
        /* Bits / Entries */
        #undef  FP_ENTRIES
        #define FP_ENTRIES  2
        #undef  FP_LUT
        #define FP_LUT      4
    #endif

    /* Optional ECC calculation method */
    /* Note: doubles heap usage, but slightly faster */
    #undef  ECC_SHAMIR
    #define ECC_SHAMIR

    /* Reduces heap usage, but slower */
    #undef  ECC_TIMING_RESISTANT
    #define ECC_TIMING_RESISTANT

    #ifdef USE_FAST_MATH
        #undef  ALT_ECC_SIZE
        #define ALT_ECC_SIZE

        /* wolfSSL will compute the FP_MAX_BITS_ECC, but it can be overridden */
        /* MAX ECC BITS = ROUND8(MAX ECC) * 2 */
        //#undef  FP_MAX_BITS_ECC
        //#define FP_MAX_BITS_ECC (528 * 2)

        /* Enable TFM optimizations for ECC */
        #define TFM_ECC192
        #define TFM_ECC224
        #define TFM_ECC256
        #define TFM_ECC384
        #define TFM_ECC521
    #endif
#endif

/* RSA */
#undef NO_RSA
#if 1
    #ifdef USE_FAST_MATH
        /* Maximum math bits (Max RSA key bits * 2) */
        #undef  FP_MAX_BITS
        #define FP_MAX_BITS     4096
    #endif

    /* half as much memory but twice as slow */
    #undef  RSA_LOW_MEM
    //#define RSA_LOW_MEM
#else
    #define NO_RSA
#endif

/* AES */
#undef NO_AES
#if 1
    #undef  HAVE_AESGCM
    #define HAVE_AESGCM

    /* GCM Method: GCM_SMALL, GCM_WORD32 or GCM_TABLE */
    #undef  GCM_SMALL
    #define GCM_SMALL
#else
    #define NO_AES
#endif

/* ChaCha20 / Poly1305 */
#undef HAVE_CHACHA
#undef HAVE_POLY1305
#if 1
    #define HAVE_CHACHA
    #define HAVE_POLY1305

    /* Needed for Poly1305 */
    #undef  HAVE_ONE_TIME_AUTH
    #define HAVE_ONE_TIME_AUTH
#endif


/* ------------------------------------------------------------------------- */
/* Hashing */
/* ------------------------------------------------------------------------- */
/* Sha */
#undef NO_SHA
#if 1
#else
    #define NO_SHA
#endif

/* Sha256 */
#undef NO_SHA256
#if 1
    /* over twice as small, but 50% slower */
    //#define USE_SLOW_SHA2
#else
    #define NO_SHA256
#endif

/* Sha512 */
#undef WOLFSSL_SHA512
#if 1
    #define WOLFSSL_SHA512

    /* Sha384 */
    #undef  WOLFSSL_SHA384
    #if 1
        #define WOLFSSL_SHA384
    #endif
#endif

/* MD5 */
#undef  NO_MD5
#if 1
    #define NO_MD5
#endif

/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
/* Use reduced benchmark / test sizes */
#undef  BENCH_EMBEDDED
#define BENCH_EMBEDDED

#undef  USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_2048

#endif /* _EMBOS_USER_SETTINGS_H_ */
