[#ftl]
/**
  ******************************************************************************
  * File Name          : ${name}
  * Description        : This file provides code for the configuration
  *                      of the ${name} instances.
  ******************************************************************************
[@common.optinclude name=mxTmpFolder+"/license.tmp"/][#--include License text --]
  ******************************************************************************
  */
[#assign s = name]
[#assign toto = s?replace(".","_")]
[#assign toto = toto?replace("/","")]
[#assign toto = toto?replace("-","_")]
[#assign inclusion_protection = toto?upper_case]
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __${inclusion_protection}__
#define __${inclusion_protection}__

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
[#if includes??]
[#list includes as include]
#include "${include}"
[/#list]
[/#if]

[#-- SWIPdatas is a list of SWIPconfigModel --]
[#list SWIPdatas as SWIP]
[#-- Global variables --]
[#if SWIP.variables??]
	[#list SWIP.variables as variable]
extern ${variable.value} ${variable.name};
	[/#list]
[/#if]

[#-- Global variables --]

[#assign instName = SWIP.ipName]
[#assign fileName = SWIP.fileName]
[#assign version = SWIP.version]

/**
	MiddleWare name : ${instName}
	MiddleWare fileName : ${fileName}
	MiddleWare version : ${version}
*/
[#if SWIP.defines??]
	[#list SWIP.defines as definition]
/*---------- [#if definition.comments??]${definition.comments}[/#if] -----------*/
#define ${definition.name} #t#t ${definition.value}
[#if definition.description??]${definition.description} [/#if]
	[/#list]
[/#if]



[/#list]

/* ------------------------------------------------------------------------- */
/* Hardware platform */
/* ------------------------------------------------------------------------- */
/* Setup default (No crypto hardware acceleration or TLS UART test).
 * Use undef in platform section to enable it.
 */
#define NO_STM32_HASH
#define NO_STM32_CRYPTO
#define NO_TLS_UART_TEST

#if defined(STM32WB55xx)
    #define WOLFSSL_STM32WB
    #define WOLFSSL_STM32_PKA
    #undef  NO_STM32_CRYPTO
    #define HAL_CONSOLE_UART huart1
#elif defined(STM32F407xx)
    #define WOLFSSL_STM32F4
    #define HAL_CONSOLE_UART huart2
#elif defined(STM32F437xx)
    #define WOLFSSL_STM32F4
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define STM32_HAL_V2
    #define HAL_CONSOLE_UART huart4
#elif defined(STM32F777xx)
    #define WOLFSSL_STM32F7
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define STM32_HAL_V2
    #define HAL_CONSOLE_UART huart2
#elif defined(STM32F756xx)
    #define WOLFSSL_STM32F7
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define STM32_HAL_V2
    #define HAL_CONSOLE_UART huart3
#elif defined(STM32H753xx)
    #define WOLFSSL_STM32H7
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define HAL_CONSOLE_UART huart3
#elif defined(STM32H723xx)
    #define WOLFSSL_STM32H7
    #define HAL_CONSOLE_UART huart3
#elif defined(STM32L4A6xx)
    #define WOLFSSL_STM32L4
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define HAL_CONSOLE_UART hlpuart1
#elif defined(STM32L475xx)
    #define WOLFSSL_STM32L4
    #define HAL_CONSOLE_UART huart1
#elif defined(STM32L562xx)
    #define WOLFSSL_STM32L5
    #define WOLFSSL_STM32_PKA
    #undef  NO_STM32_HASH
    #undef  NO_STM32_CRYPTO
    #define HAL_CONSOLE_UART huart1
#elif defined(STM32L552xx)
    #define WOLFSSL_STM32L5
    #undef  NO_STM32_HASH
    #define HAL_CONSOLE_UART hlpuart1
#elif defined(STM32F207xx)
    #define WOLFSSL_STM32F2
    #define HAL_CONSOLE_UART huart3
#elif defined(STM32F217xx)
    #define WOLFSSL_STM32F2
    #define HAL_CONSOLE_UART huart2
#elif defined(STM32F107xC)
    #define WOLFSSL_STM32F1
    #define HAL_CONSOLE_UART huart4
    #define NO_STM32_RNG
#elif defined(STM32F401xE)
    #define WOLFSSL_STM32F4
    #define HAL_CONSOLE_UART huart2
    #define NO_STM32_RNG
    #define WOLFSSL_GENSEED_FORTEST /* no HW RNG is available use test seed */
#elif defined(STM32G071xx)
    #define WOLFSSL_STM32G0
    #define HAL_CONSOLE_UART huart2
    #define NO_STM32_RNG
    #define WOLFSSL_GENSEED_FORTEST /* no HW RNG is available use test seed */
#elif defined(STM32U575xx) || defined(STM32U585xx)
    #define HAL_CONSOLE_UART huart1
    #define WOLFSSL_STM32U5
    #define STM32_HAL_V2
    #ifdef STM32U585xx
        #undef  NO_STM32_HASH
        #undef  NO_STM32_CRYPTO
        #define WOLFSSL_STM32_PKA
    #endif
#elif defined(STM32H563xx)
    #define WOLFSSL_STM32H5
    #define HAL_CONSOLE_UART huart3
    #define STM32_HAL_V2
    #undef  NO_STM32_HASH

#else
    #warning Please define a hardware platform!
    /* This means there is not a pre-defined platform for your board/CPU */
    /* You need to define a CPU type, HW crypto and debug UART */
    /* CPU Type: WOLFSSL_STM32F1, WOLFSSL_STM32F2, WOLFSSL_STM32F4,
        WOLFSSL_STM32F7, WOLFSSL_STM32H7, WOLFSSL_STM32L4, WOLFSSL_STM32L5,
        WOLFSSL_STM32G0, WOLFSSL_STM32WB and WOLFSSL_STM32U5 */
    #define WOLFSSL_STM32F4

    /* Debug UART used for printf */
    /* The UART interface number varies for each board/CPU */
    /* Typically this is the UART attached to the ST-Link USB CDC UART port */
    #define HAL_CONSOLE_UART huart4

    /* Hardware Crypto - uncomment as available on hardware */
    //#define WOLFSSL_STM32_PKA
    //#define NO_STM32_RNG
    //#undef  NO_STM32_HASH
    //#undef  NO_STM32_CRYPTO
    //#define WOLFSSL_GENSEED_FORTEST /* if no HW RNG is available use test seed */
    //#define STM32_HAL_V2
#endif


/* ------------------------------------------------------------------------- */
/* Platform */
/* ------------------------------------------------------------------------- */
#define SIZEOF_LONG_LONG 8
#define WOLFSSL_GENERAL_ALIGNMENT 4
#define WOLFSSL_STM32_CUBEMX
#define WOLFSSL_SMALL_STACK
#define WOLFSSL_USER_IO
#define WOLFSSL_NO_SOCK
#define WOLFSSL_IGNORE_FILE_WARN


/* ------------------------------------------------------------------------- */
/* Operating System */
/* ------------------------------------------------------------------------- */
#if defined(WOLF_CONF_RTOS) && WOLF_CONF_RTOS == 2
    #define FREERTOS
#else
    #define SINGLE_THREADED
#endif


/* ------------------------------------------------------------------------- */
/* Math Configuration */
/* ------------------------------------------------------------------------- */
/* 1=Fast (stack)
 * 2=Normal (heap)
 * 3=Single Precision C (only common curves/key sizes)
 * 4=Single Precision ASM Cortex-M3+
 * 5=Single Precision ASM Cortex-M0 (Generic Thumb)
 * 6=Single Precision C all small
 * 7=Single Precision C all big
 */
#if defined(WOLF_CONF_MATH) && WOLF_CONF_MATH == 1
    /* fast (stack) math - tfm.c */
    #define USE_FAST_MATH
    #define TFM_TIMING_RESISTANT

    /* Optimizations (TFM_ARM, TFM_ASM or none) */
    //#define TFM_NO_ASM
    //#define TFM_ASM
#elif defined(WOLF_CONF_MATH) && WOLF_CONF_MATH == 2
    /* heap math - integer.c */
    #define USE_INTEGER_HEAP_MATH
#elif defined(WOLF_CONF_MATH) && (WOLF_CONF_MATH >= 3)
    /* single precision only */
    #define WOLFSSL_SP
    #if WOLF_CONF_MATH != 7
        #define WOLFSSL_SP_SMALL      /* use smaller version of code */
    #endif
    #if defined(WOLF_CONF_RSA) && WOLF_CONF_RSA == 1
        #define WOLFSSL_HAVE_SP_RSA
    #endif
    #if defined(WOLF_CONF_DH) && WOLF_CONF_DH == 1
        #define WOLFSSL_HAVE_SP_DH
    #endif
    #if defined(WOLF_CONF_ECC) && WOLF_CONF_ECC == 1
        #define WOLFSSL_HAVE_SP_ECC
    #endif
    #if WOLF_CONF_MATH == 6 || WOLF_CONF_MATH == 7
        #define WOLFSSL_SP_MATH    /* disable non-standard curves / key sizes */
    #endif
    #define SP_WORD_SIZE 32

    /* Enable to put all math on stack (no heap) */
    //#define WOLFSSL_SP_NO_MALLOC
    /* Enable for SP cache resistance (not usually enabled for embedded micros) */
    //#define WOLFSSL_SP_CACHE_RESISTANT

    #if WOLF_CONF_MATH == 4 || WOLF_CONF_MATH == 5
        #define WOLFSSL_SP_ASM /* required if using the ASM versions */
        #if WOLF_CONF_MATH == 4
            /* ARM Cortex-M3+ */
            #define WOLFSSL_SP_ARM_CORTEX_M_ASM
        #endif
        #if WOLF_CONF_MATH == 5
            /* Generic ARM Thumb (Cortex-M0) Assembly */
            #define WOLFSSL_SP_ARM_THUMB_ASM
        #endif
    #endif
#endif


/* ------------------------------------------------------------------------- */
/* Enable Features */
/* ------------------------------------------------------------------------- */
/* Required for TLS */
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_ENCRYPT_THEN_MAC
#define HAVE_EXTENDED_MASTER

#if defined(WOLF_CONF_TLS13) && WOLF_CONF_TLS13 == 1
    #define WOLFSSL_TLS13
    #define HAVE_HKDF
#endif
#if defined(WOLF_CONF_DTLS) && WOLF_CONF_DTLS == 1
    #define WOLFSSL_DTLS
#endif
#if defined(WOLF_CONF_PSK) && WOLF_CONF_PSK == 0
    #define NO_PSK
#endif
#if defined(WOLF_CONF_PWDBASED) && WOLF_CONF_PWDBASED == 0
    #define NO_PWDBASED
#endif
#if defined(WOLF_CONF_KEEP_PEER_CERT) && WOLF_CONF_KEEP_PEER_CERT == 1
    #define KEEP_PEER_CERT
#endif
#if defined(WOLF_CONF_BASE64_ENCODE) && WOLF_CONF_BASE64_ENCODE == 1
    #define WOLFSSL_BASE64_ENCODE
#endif
#if defined(WOLF_CONF_OPENSSL_EXTRA) && WOLF_CONF_OPENSSL_EXTRA >= 1
    #define OPENSSL_EXTRA
    #if !defined(INT_MAX)
        #include <limits.h>
    #endif
#endif
#if defined(WOLF_CONF_OPENSSL_EXTRA) && WOLF_CONF_OPENSSL_EXTRA >= 2
    #define OPENSSL_ALL
#endif

/* TLS Session Cache */
#if 0
    #define SMALL_SESSION_CACHE
#else
    #define NO_SESSION_CACHE
#endif

/* Post Quantum
 * Note: PQM4 is compatible with STM32. The project can be found at:
 * https://github.com/mupq/pqm4
 */
#if defined(WOLF_CONF_PQM4) && WOLF_CONF_PQM4 == 1
    #define HAVE_PQM4
#endif


/* ------------------------------------------------------------------------- */
/* Crypto */
/* ------------------------------------------------------------------------- */
/* RSA */
#undef NO_RSA
#if defined(WOLF_CONF_RSA) && WOLF_CONF_RSA == 1
    #ifdef USE_FAST_MATH
        /* Maximum math bits (Max RSA key bits * 2) */
        #undef  FP_MAX_BITS
        #define FP_MAX_BITS     4096
    #endif

    /* half as much memory but twice as slow */
    #undef  RSA_LOW_MEM
    //#define RSA_LOW_MEM

    /* Enables blinding mode, to prevent timing attacks */
    #undef  WC_RSA_BLINDING
    #define WC_RSA_BLINDING

    /* RSA PSS Support (required for TLS v1.3) */
    #ifdef WOLFSSL_TLS13
        #define WC_RSA_PSS
    #endif
#else
    #define NO_RSA
#endif

/* ECC */
#undef HAVE_ECC
#if defined(WOLF_CONF_ECC) && WOLF_CONF_ECC == 1
    #define HAVE_ECC

    /* Manually define enabled curves */
    #define ECC_USER_CURVES

    //#define HAVE_ECC192
    //#define HAVE_ECC224
    #undef NO_ECC256
    //#define HAVE_ECC384
    //#define HAVE_ECC521

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
    #define ECC_TIMING_RESISTANT

    /* Compressed ECC key support */
    //#define HAVE_COMP_KEY

    #ifdef USE_FAST_MATH
        #ifdef NO_RSA
            /* Custom fastmath size if not using RSA */
            /* MAX = ROUND32(ECC BITS) * 2 */
            #define FP_MAX_BITS     (256 * 2)
        #else
            #define ALT_ECC_SIZE
        #endif

        /* Enable TFM optimizations for ECC */
        //#define TFM_ECC192
        //#define TFM_ECC224
        //#define TFM_ECC256
        //#define TFM_ECC384
        //#define TFM_ECC521
    #endif
#endif

/* DH */
#undef NO_DH
#if defined(WOLF_CONF_DH) && WOLF_CONF_DH == 1
    #define HAVE_DH /* freeRTOS settings.h requires this */
    #define HAVE_FFDHE_2048
    #define HAVE_DH_DEFAULT_PARAMS
#else
    #define NO_DH
#endif

/* AES */
#if defined(WOLF_CONF_AESGCM) && WOLF_CONF_AESGCM == 1
    #define HAVE_AESGCM
    /* GCM Method: GCM_SMALL, GCM_WORD32, GCM_TABLE or GCM_TABLE_4BIT */
    /* GCM_TABLE is about 4K larger and 3x faster for GHASH */
    #define GCM_SMALL
    #define HAVE_AES_DECRYPT
#endif

#if defined(WOLF_CONF_AESCBC) && WOLF_CONF_AESCBC == 1
    #define HAVE_AES_CBC
    #define HAVE_AES_DECRYPT
#endif

/* Other possible AES modes */
//#define WOLFSSL_AES_COUNTER
//#define HAVE_AESCCM
//#define WOLFSSL_AES_XTS
//#define WOLFSSL_AES_DIRECT
//#define HAVE_AES_ECB
//#define HAVE_AES_KEYWRAP
//#define AES_MAX_KEY_SIZE 256

/* ChaCha20 / Poly1305 */
#undef HAVE_CHACHA
#undef HAVE_POLY1305
#if defined(WOLF_CONF_CHAPOLY) && WOLF_CONF_CHAPOLY == 1
    #define HAVE_CHACHA
    #define HAVE_POLY1305

    /* Needed for Poly1305 */
    #undef  HAVE_ONE_TIME_AUTH
    #define HAVE_ONE_TIME_AUTH
#endif

/* Ed25519 / Curve25519 */
#undef HAVE_CURVE25519
#undef HAVE_ED25519
#if defined(WOLF_CONF_EDCURVE25519) && WOLF_CONF_EDCURVE25519 == 1
    #define HAVE_CURVE25519
    #define HAVE_ED25519

    /* Optionally use small math (less flash usage, but much slower) */
    #define CURVED25519_SMALL
#endif


/* ------------------------------------------------------------------------- */
/* Hashing */
/* ------------------------------------------------------------------------- */
/* Sha1 */
#undef NO_SHA
#if defined(WOLF_CONF_SHA1) && WOLF_CONF_SHA1 == 1
    /* 1k smaller, but 25% slower */
    //#define USE_SLOW_SHA
#else
    #define NO_SHA
#endif

/* Sha2-256 */
#undef NO_SHA256
#if defined(WOLF_CONF_SHA2_256) && WOLF_CONF_SHA2_256 == 1
    /* not unrolled - ~2k smaller and ~25% slower */
    //#define USE_SLOW_SHA256

    //#define WOLFSSL_SHAKE256

    /* Sha2-224 */
    #if defined(WOLF_CONF_SHA2_224) && WOLF_CONF_SHA2_224 == 1
        #define WOLFSSL_SHA224
    #endif
#else
    #define NO_SHA256
#endif

/* Sha2-512 */
#undef WOLFSSL_SHA512
#if defined(WOLF_CONF_SHA2_512) && WOLF_CONF_SHA2_512 == 1
    /* over twice as small, but 50% slower */
    //#define USE_SLOW_SHA512

    #define WOLFSSL_SHA512
    #define HAVE_SHA512 /* freeRTOS settings.h requires this */
#endif

/* Sha2-384 */
#undef WOLFSSL_SHA384
#if defined(WOLF_CONF_SHA2_384) && WOLF_CONF_SHA2_384 == 1
    #define WOLFSSL_SHA384
#endif

/* Sha3 */
#undef WOLFSSL_SHA3
#if defined(WOLF_CONF_SHA3) && WOLF_CONF_SHA3 == 1
	#define WOLFSSL_SHA3
#endif

/* MD5 */
#if defined(WOLF_CONF_MD5) && WOLF_CONF_MD5 == 1
	/* enabled */
#else
    #define NO_MD5
#endif


/* ------------------------------------------------------------------------- */
/* Benchmark / Test */
/* ------------------------------------------------------------------------- */
/* Use reduced benchmark / test sizes */
#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_256


/* ------------------------------------------------------------------------- */
/* Debugging */
/* ------------------------------------------------------------------------- */
#if defined(WOLF_CONF_DEBUG) && WOLF_CONF_DEBUG == 1
    #define DEBUG_WOLFSSL

    /* Use this to measure / print heap usage */
    #if 0
        #define USE_WOLFSSL_MEMORY
        #define WOLFSSL_TRACK_MEMORY
  		  #define WOLFSSL_DEBUG_MEMORY
	  	  #define WOLFSSL_DEBUG_MEMORY_PRINT
    #endif
#else
    //#define NO_WOLFSSL_MEMORY
    //#define NO_ERROR_STRINGS
#endif


/* ------------------------------------------------------------------------- */
/* Port */
/* ------------------------------------------------------------------------- */

/* Override Current Time */
/* Allows custom "custom_time()" function to be used for benchmark */
#define WOLFSSL_USER_CURRTIME


/* ------------------------------------------------------------------------- */
/* RNG */
/* ------------------------------------------------------------------------- */
#define NO_OLD_RNGNAME /* conflicts with STM RNG macro */
#if !defined(WOLF_CONF_RNG) || WOLF_CONF_RNG == 1
    /* default is enabled */
    #define HAVE_HASHDRBG
#else /* WOLF_CONF_RNG == 0 */
    #define WC_NO_HASHDRBG
    #define WC_NO_RNG
#endif


/* ------------------------------------------------------------------------- */
/* Disable Features */
/* ------------------------------------------------------------------------- */
#if defined(WOLF_CONF_TLS12) && WOLF_CONF_TLS12 == 0
    #define WOLFSSL_NO_TLS12
#endif
#if defined(WOLF_CONF_WOLFCRYPT_ONLY) && WOLF_CONF_WOLFCRYPT_ONLY == 1
    #define WOLFCRYPT_ONLY
#endif
//#define NO_WOLFSSL_SERVER
//#define NO_WOLFSSL_CLIENT

#if defined(WOLF_CONF_TEST) && WOLF_CONF_TEST == 0
    #define NO_CRYPT_TEST
    #define NO_CRYPT_BENCHMARK
#endif

#define NO_FILESYSTEM
#define NO_WRITEV
#define NO_MAIN_DRIVER
#define NO_DEV_RANDOM
#define NO_OLD_TLS
#define WOLFSSL_NO_CLIENT_AUTH /* disable client auth for Ed25519/Ed448 */

#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_DES3

/* In-lining of misc.c functions */
/* If defined, must include wolfcrypt/src/misc.c in build */
/* Slower, but about 1k smaller */
//#define NO_INLINE

/* Base16 / Base64 encoding */
//#define NO_CODING

/* bypass certificate date checking, due to lack of properly configured RTC source */
#ifndef HAL_RTC_MODULE_ENABLED
    #define NO_ASN_TIME
#endif


#ifdef __cplusplus
}
#endif
#endif /* ${inclusion_protection}_H */

/**
  * @}
  */

/*****END OF FILE****/
