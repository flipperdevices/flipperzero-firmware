
#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H


/* Default build with fast math */


/* Slower build but uses less memory */
//#define BUILD_B

/* Performant build but uses more memory */
//#define BUILD_C



/* Used for getting random value for seeding RNG */
#define WOLFSSL_NETBURNER
#define WOLFSSL_MCF5441X

/* environment settings */
#define NO_WRITEV
#define WOLFSSL_NO_SOCK
#define NO_WOLFSSL_DIR

/* with USE_FAST_MATH smallstack is used to fit in the default stack size */
#define WOLFSSL_SMALL_STACK


/* enable features off by default */
#define WOLFSSL_SHA512
// OPENSSL_EXTRA uses a lot more memory but is needed in order to enable
// compatibility layer API
#define OPENSSL_EXTRA

// additional RSA padding schemes
#define WC_RSA_NO_PADDING
#define WC_RSA_PSS

// uncomment and add wolfSSL_Debugging_ON() to app for debug messages
//#define DEBUG_WOLFSSL


/* disable features that are on by default */
#define WOLFCRYPT_ONLY
#define NO_FILESYSTEM
#define SINGLE_THREADED

#define NO_ASN_TIME
#define NO_PWDBASED
#define NO_RC4
#define NO_DSA
#define NO_DES3
#define NO_DH
#define NO_MD4


#define USE_FAST_MATH
#ifdef BUILD_B
    #define RSA_LOW_MEM
    #define USE_SLOW_SHA
    #define USE_SLOW_SHA256
    #define NO_ERROR_STRINGS
    #define USE_FAST_MATH
#endif

#ifdef BUILD_C
    #define WOLFSSL_HAVE_SP_RSA
    #define SP_WORD_SIZE 32
#endif

/* hardening against side channel attacks */
#if defined(USE_FAST_MATH)
    #define TFM_TIMING_RESISTANT
    #ifdef HAVE_ECC
        #define ECC_TIMING_RESISTANT
    #endif
#endif /* USE_FAST_MATH */
#ifndef NO_RSA
    /* this slows down RSA operations but increases side channel resistance */
    #define WC_RSA_BLINDING
#endif

#endif /* USER_SETTINGS_H */

