/* Configuration */
#define IPHONE  /* Needed for Xcode */
#define HAVE_HASHDRBG
#define HAVE_AESGCM
#define WOLFSSL_SHA512
#define WOLFSSL_SHA384

#ifdef HAVE_FIPS
    #define NO_MD4
    #define NO_DSA
    #define NO_PWDBASED
#else
    /* disable "main" entry */
    #undef NO_MAIN_DRIVER
    #define NO_MAIN_DRIVER

    /* 128-bit type */
    #define HAVE___UINT128_T

    /* SP Math */
    #define WOLFSSL_SP_MATH
    #define HAVE_ECC

    /* ECC speedups */
    #define ECC_SHAMIR
    #define TFM_ECC256

    /* timing resistance */
    #if 1
        #define WC_RSA_BLINDING
        #define TFM_TIMING_RESISTANT
        #define ECC_TIMING_RESISTANT
    #else
        #define WC_NO_HARDEN
    #endif

    /* single precision math */
    #if 1
        #define WOLFSSL_HAVE_SP_RSA
        #define WOLFSSL_HAVE_SP_DH
        #define WOLFSSL_HAVE_SP_ECC
    #endif

    /* ARMv8 - iPhone 8/8Plus and iPhone X */
    #ifdef __ARM_FEATURE_CRYPTO
        #define WOLFSSL_ARMASM
        #define WOLFSSL_SP_ARM64_ASM
    #endif

    /* newer algorithms */
    #define WOLFSSL_SHA3

    #define HAVE_POLY1305
    #define HAVE_CHACHA

    #define HAVE_CURVE25519
    #ifndef WOLFSSL_ARMASM
        #define HAVE_ED25519
    #endif

    /* TLS extensions */
    #define HAVE_ONE_TIME_AUTH
    #define HAVE_TLS_EXTENSIONS
    #define HAVE_SUPPORTED_CURVES
    #define HAVE_EXTENDED_MASTER

    /* off by default */
    #define NO_RC4
    #define NO_MD4
    #define NO_DSA
    #define NO_PSK
    #define NO_PWDBASED

    /* test certificate buffers */
    #define USE_CERT_BUFFERS_2048
    #define USE_CERT_BUFFERS_256
    #define NO_WRITE_TEMP_FILES

    #define WOLFSSL_DTLS

    //#define DEBUG_WOLFSSL

#endif
