#ifndef _WIN_USER_SETTINGS_H_
#define _WIN_USER_SETTINGS_H_

/* For FIPS Ready, uncomment the following: */
/* #define WOLFSSL_FIPS_READY */
#ifdef WOLFSSL_FIPS_READY
    #undef HAVE_FIPS_VERSION
    #define HAVE_FIPS_VERSION 3
#endif


/* Verify this is Windows */
#ifndef _WIN32
#error This user_settings.h header is only designed for Windows
#endif

/* Configurations */
#define WOLFCRYPT_ONLY

#undef USE_FAST_MATH
#if 1
    #define USE_FAST_MATH

    #undef  TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT

    /* Optimizations */
    //#define TFM_ARM
#endif

/* Wolf Single Precision Math */
#undef WOLFSSL_SP
#if 0
    #define WOLFSSL_SP
    //#define WOLFSSL_SP_SMALL      /* use smaller version of code */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC
    //#define WOLFSSL_SP_CACHE_RESISTANT
    //#define WOLFSSL_SP_MATH     /* only SP math - eliminates fast math code */

    /* SP Assembly Speedups */
    //#define WOLFSSL_SP_ASM      /* required if using the ASM versions */
    //#define WOLFSSL_SP_ARM32_ASM
    //#define WOLFSSL_SP_ARM64_ASM
    //#define WOLFSSL_SP_ARM_THUMB_ASM
    //#define WOLFSSL_SP_ARM_CORTEX_M_ASM
#endif

/* ------------------------------------------------------------------------- */
/* FIPS - Requires eval or license from wolfSSL */
/* ------------------------------------------------------------------------- */
#undef  HAVE_FIPS
#if 1
    #define HAVE_FIPS

    #undef  HAVE_FIPS_VERSION
    #define HAVE_FIPS_VERSION 2

    #ifdef SINGLE_THREADED
        #undef  NO_THREAD_LS
        #define NO_THREAD_LS
    #else
        #ifndef USE_WINDOWS_API
            #define USE_WINDOWS_API
        #endif
    #endif

    #undef NO_ATTRIBUTE_CONSTRUCTOR
    //#define NO_ATTRIBUTE_CONSTRUCTOR

#endif


/* FIPS */
//#define OPENSSL_EXTRA
//#define HAVE_THREAD_LS
#define WOLFSSL_KEY_GEN
#define HAVE_AESGCM
#define HAVE_HASHDRBG
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define NO_PSK
#define NO_RC4
//#define NO_DSA
#define NO_MD4

#if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    /* ------------------------------------------------------------------------- */
    /* Crypto */
    /* ------------------------------------------------------------------------- */
    #define USE_CERT_BUFFERS_2048
    #define USE_CERT_BUFFERS_256
    /* RSA */
    #undef NO_RSA
    #if 1
        #ifdef USE_FAST_MATH
            /* Maximum math bits (Max RSA key bits * 2) */
            #undef  FP_MAX_BITS
            #define FP_MAX_BITS     8192
        #endif

        /* half as much memory but twice as slow */
        #undef  RSA_LOW_MEM
        //#define RSA_LOW_MEM

        /* Enables blinding mode, to prevent timing attacks */
        #if 0
            #undef  WC_RSA_BLINDING
            #define WC_RSA_BLINDING
        #else
            #undef  WC_NO_HARDEN
            #define WC_NO_HARDEN
        #endif

        /* RSA PSS Support */
        #if 1
            #define WC_RSA_PSS
        #endif

        #if 1
            #define WC_RSA_NO_PADDING
        #endif
    #else
        #define NO_RSA
    #endif
    /* ECC */
    #undef HAVE_ECC
    #if 1
        #define HAVE_ECC

        /* Manually define enabled curves */
        #undef  ECC_USER_CURVES
        //#define ECC_USER_CURVES

        #ifdef ECC_USER_CURVES
            /* Manual Curve Selection */
            //#define HAVE_ECC192
            //#define HAVE_ECC224
            #undef NO_ECC256
            //#define HAVE_ECC384
            //#define HAVE_ECC521
        #endif

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

        #ifdef HAVE_FIPS
            #undef  HAVE_ECC_CDH
            #define HAVE_ECC_CDH /* Enable cofactor support */

            #undef NO_STRICT_ECDSA_LEN
            #define NO_STRICT_ECDSA_LEN /* Do not force fixed len w/ FIPS */

            #undef  WOLFSSL_VALIDATE_ECC_IMPORT
            #define WOLFSSL_VALIDATE_ECC_IMPORT /* Validate import */
        #endif

        /* Compressed Key Support */
        #undef  HAVE_COMP_KEY
        //#define HAVE_COMP_KEY

        /* Use alternate ECC size for ECC math */
        #ifdef USE_FAST_MATH
            /* MAX ECC BITS = ROUND8(MAX ECC) * 2 */
            #ifdef NO_RSA
                /* Custom fastmath size if not using RSA */
                #undef  FP_MAX_BITS
                #define FP_MAX_BITS     (256 * 2)
            #else
                #undef  ALT_ECC_SIZE
                #define ALT_ECC_SIZE
                /* wolfSSL will compute the FP_MAX_BITS_ECC, but it can be overridden */
                //#undef  FP_MAX_BITS_ECC
                //#define FP_MAX_BITS_ECC (256 * 2)
            #endif

            /* Speedups specific to curve */
            #ifndef NO_ECC256
                #undef  TFM_ECC256
                #define TFM_ECC256
            #endif
        #endif
    #endif

    /* AES */
    #undef NO_AES
    #if 1
        #undef  HAVE_AES_CBC
        #define HAVE_AES_CBC

        #undef  HAVE_AESGCM
        #define HAVE_AESGCM

        /* GCM Method: GCM_SMALL, GCM_WORD32 or GCM_TABLE */
        //#define GCM_SMALL
        //#define GCM_WORD32
        #define GCM_TABLE

        #undef  WOLFSSL_AES_DIRECT
        #define WOLFSSL_AES_DIRECT

        #undef  HAVE_AES_ECB
        #define HAVE_AES_ECB

        #undef  WOLFSSL_AES_COUNTER
        #define WOLFSSL_AES_COUNTER

        #undef  HAVE_AESCCM
        #define HAVE_AESCCM

    #else
        #define NO_AES
    #endif


    /* DES3 */
    #undef NO_DES3
    #if 1
    #else
        #define NO_DES3
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
        #if 1
            #define WOLFSSL_SHA224
        #endif
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

        /* over twice as small, but 50% slower */
        //#define USE_SLOW_SHA512
    #endif

    /* Sha3 */
    #undef WOLFSSL_SHA3
    #if 1
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
    #if 1
        #define WOLFSSL_CMAC
    #endif

    /* DH */
    #undef  NO_DH
    #if 1
        /* Use table for DH instead of -lm (math) lib dependency */
        #if 0
            #define WOLFSSL_DH_CONST
            #define HAVE_FFDHE_2048
            #define HAVE_FFDHE_4096
            //#define HAVE_FFDHE_6144
            //#define HAVE_FFDHE_8192
        #endif

        #ifdef HAVE_FIPS
            #define WOLFSSL_VALIDATE_FFC_IMPORT
            #define HAVE_FFDHE_Q
        #endif
    #else
        #define NO_DH
    #endif
#endif /* FIPS v2 */

//#define DEBUG_WOLFSSL
#define NO_MAIN_DRIVER

/* wolfEngine settings */
#define WOLFSSL_PUBLIC_MP
#define NO_WOLFSSL_DIR

//#define WOLFENGINE_DEBUG
/* TODO: Add WE_HAVE_* settings here */

#endif /* _WIN_USER_SETTINGS_H_ */
