#ifndef _WIN_USER_SETTINGS_H_
#define _WIN_USER_SETTINGS_H_

/* Set the following to 1 for WCv5.0-RC12 build. */
#if 0
#undef HAVE_FIPS
#define HAVE_FIPS
#undef HAVE_FIPS_VERSION
#define HAVE_FIPS_VERSION 5
#undef HAVE_FIPS_VERSION_MINOR
#define HAVE_FIPS_VERSION_MINOR 2
#endif

/* For FIPS Ready, uncomment the following: */
/* #define WOLFSSL_FIPS_READY */
#ifdef WOLFSSL_FIPS_READY
    #undef HAVE_FIPS
    #define HAVE_FIPS
    #undef HAVE_FIPS_VERSION
    #define HAVE_FIPS_VERSION 5
    #undef HAVE_FIPS_VERSION_MINOR
    #define HAVE_FIPS_VERSION_MINOR 3
#endif


/* Verify this is Windows */
#ifndef _WIN32
#error This user_settings.h header is only designed for Windows
#endif

/* Configurations */
#if defined(HAVE_FIPS)
    /* FIPS */
    #define OPENSSL_EXTRA
    #define HAVE_THREAD_LS
    #define WOLFSSL_KEY_GEN
    #define HAVE_AESGCM
    #define HAVE_HASHDRBG
    #define WOLFSSL_SHA384
    #define WOLFSSL_SHA512
    #define NO_PSK
    #define NO_RC4
    #define NO_DSA
    #define NO_MD4

    #if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
        #define WOLFSSL_SHA224
        #define WOLFSSL_SHA3
        #define WC_RSA_PSS
        #define WC_RSA_NO_PADDING
        #define HAVE_ECC
        #define HAVE_ECC384
        #define HAVE_ECC521
        #define HAVE_SUPPORTED_CURVES
        #define HAVE_TLS_EXTENSIONS
        #define ECC_SHAMIR
        #define HAVE_ECC_CDH
        #define ECC_TIMING_RESISTANT
        #define TFM_TIMING_RESISTANT
        #define WOLFSSL_AES_COUNTER
        #define WOLFSSL_AES_DIRECT
        #define HAVE_AES_ECB
        #define HAVE_AESCCM
        #define WOLFSSL_CMAC
        #define HAVE_HKDF
        #define WOLFSSL_VALIDATE_ECC_IMPORT
        #define WOLFSSL_VALIDATE_FFC_IMPORT
        #define HAVE_FFDHE_Q
        #define HAVE_PUBLIC_FFDHE
        #define WOLFSSL_AESNI
        #define HAVE_INTEL_RDSEED
        #define FORCE_FAILURE_RDSEED
    #endif /* FIPS v2 */
    #if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 5)
        #undef WOLFSSL_AESNI /* Comment out if using PAA */
        #undef HAVE_INTEL_RDSEED
        #undef FORCE_FAILURE_RDSEED
        #undef HAVE_PUBLIC_FFDHE

        #define NO_DES
        #define NO_DES3
        #define NO_MD5
        #define NO_OLD_TLS

        #define WOLFSSL_TLS13
        #define HAVE_TLS_EXTENSIONS
        #define HAVE_SUPPORTED_CURVES
        #define GCM_TABLE_4BIT
        #define WOLFSSL_NO_SHAKE256
        #define WOLFSSL_VALIDATE_ECC_KEYGEN
        #define WOLFSSL_ECDSA_SET_K
        #define WOLFSSL_WOLFSSH
        #define WOLFSSL_PUBLIC_MP
        #define WC_RNG_SEED_CB
        #define TFM_ECC256
        #define ECC_USER_CURVES
        #define HAVE_ECC192
        #define HAVE_ECC224
        #define HAVE_ECC256
        #define HAVE_ECC384
        #define HAVE_ECC521
        #define HAVE_FFDHE_2048
        #define HAVE_FFDHE_3072
        #define HAVE_FFDHE_4096
        #define HAVE_FFDHE_6144
        #define HAVE_FFDHE_8192
        #define WOLFSSL_AES_OFB
        #define FP_MAX_BITS 16384
    #endif /* FIPS v5 */
#else
    /* Enables blinding mode, to prevent timing attacks */
    #define WC_RSA_BLINDING

    #if defined(WOLFSSL_LIB)
        /* The lib */
        #define OPENSSL_EXTRA
        #define WOLFSSL_RIPEMD
        #define NO_PSK
        #define HAVE_EXTENDED_MASTER
        #define WOLFSSL_SNIFFER
        #define HAVE_SECURE_RENEGOTIATION

        #define HAVE_AESGCM
        #define WOLFSSL_SHA384
        #define WOLFSSL_SHA512

        #define HAVE_SUPPORTED_CURVES
        #define HAVE_TLS_EXTENSIONS

        #define HAVE_ECC
        #define ECC_SHAMIR
        #define ECC_TIMING_RESISTANT
    #else
        /* The servers and clients */
        #define OPENSSL_EXTRA
        #define NO_PSK
    #endif
#endif /* HAVE_FIPS */

#endif /* _WIN_USER_SETTINGS_H_ */
