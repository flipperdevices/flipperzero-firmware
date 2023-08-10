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

#define NO_MAIN_DRIVER
#define BENCH_EMBEDDED
#define SIZEOF_LONG_LONG 8
#define NO_WRITEV
#define NO_DEV_RANDOM
#define WOLFSSL_IGNORE_FILE_WARN

#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

#define WOLFSSL_USER_CURRTIME /* for benchmark */
#define WOLFSSL_CURRTIME_OSTICK /* use OS tick for current_time */
#define WOLFSSL_GMTIME
#define NO_MULTIBYTE_PRINT

// <<< Use Configuration Wizard in Context Menu >>>


//  <h>Common options
//        <o> MPU<0=>Undefined<1=>STM32F2xx<2=>STM32F4xx<3=>STM32F7xx
#define MDK_CONF_MPU 3
#if     MDK_CONF_MPU == 0

#elif   MDK_CONF_MPU == 1
#define WOLFSSL_STM32_CUBEMX
#define STM32F2xx
#elif MDK_CONF_MPU == 2
#define WOLFSSL_STM32_CUBEMX
#define STM32F4xx
#elif MDK_CONF_MPU == 3
#define WOLFSSL_STM32_CUBEMX
#define STM32F7xx
#endif

//        <o> Thread/RTOS<0=>Single Threaded <1=>FreeRTOS <3=>SafeRTOS <4=>Windows
//         <5=>PThread <6=>ThreadX
//         <7=>Micrium <8=>EBSnet <9=>MQX
//         <10=>T-RTOS <11=>uITRON4 <12=>uTKERNEL2
//         <13=>Frosted <14=>CMSIS RTOS <15=>CMSIS RTOSv2 <16=>Others
#define MDK_CONF_THREAD 15
#if MDK_CONF_THREAD== 0
#define SINGLE_THREADED
#elif MDK_CONF_THREAD == 1
#define FREERTOS
#elif MDK_CONF_THREAD == 3
#define WOLFSSL_SAFERTOS
#elif MDK_CONF_THREAD == 4
#define USE_WINDOWS_API
#elif MDK_CONF_THREAD == 5
#define WOLFSSL_PTHREADS
#elif MDK_CONF_THREAD == 6
#define THREADX
#define NETX
#elif MDK_CONF_THREAD == 7
#define MICRIUM
#elif MDK_CONF_THREAD == 8
#define EBSNET
#elif MDK_CONF_THREAD == 9
#define FREESCALE_MQX
#define FREESCALE_KSDK_MQX
#elif MDK_CONF_THREAD == 10
#define WOLFSSL_TIRTOS
#elif MDK_CONF_THREAD == 11
#define WOLFSSL_uITRON4
#elif MDK_CONF_THREAD == 12
#define WOLFSSL_uTKERNEL2
#elif MDK_CONF_THREAD == 13
#define WOLFSSL_FROSTED
#elif MDK_CONF_THREAD == 14
#define WOLFSSL_CMSIS_RTOS
#elif MDK_CONF_THREAD == 15
#define WOLFSSL_CMSIS_RTOSv2
#elif MDK_CONF_THREAD == 16
#define SINGLE_THREADED
#endif


//      <e>File System
#define MDK_CONF_FILESYSTEM 0
#if MDK_CONF_FILESYSTEM == 0
#define NO_FILESYSTEM
#else
#define WOLFSSL_KEIL_FS
#define NO_WOLFSSL_DIR
#endif
//  </e>

//   <o> Network<0=>None <1=>RLnet <2=>User I/O
#ifndef MDK_CONF_NETWORK
#define MDK_CONF_NETWORK 1
#endif
#if   MDK_CONF_NETWORK == 0
#elif MDK_CONF_NETWORK == 1
#define WOLFSSL_KEIL_TCP_NET
#elif MDK_CONF_NETWORK == 2
#define WOLFSSL_USER_IO
#endif

//  <h>Debug options

//              <e>Debug Message
#define MDK_CONF_DebugMessage 0
#if MDK_CONF_DebugMessage == 1
#define DEBUG_WOLFSSL
#endif
//         </e>
//              <e>Check malloc
#define MDK_CONF_CheckMalloc 1
#if MDK_CONF_CheckMalloc == 1
#define WOLFSSL_MALLOC_CHECK
#define USE_WOLFSSL_MEMORY
#endif
//         </e>
//      <e>ErrNo.h
#define MDK_CONF_ErrNo 1
#if MDK_CONF_ErrNo == 1
#define HAVE_ERRNO
#endif
//  </e>
//      <e>Error Strings
#define MDK_CONF_ErrorStrings 1
#if MDK_CONF_ErrorStrings == 0
#define NO_ERROR_STRINGS
#endif
//  </e>

//</h>
//</h>

// <h> wolfCrypt Configuration

//  <h>Hash/Crypt Algorithm

//      <e>MD2
#define MDK_CONF_MD2 0
#if MDK_CONF_MD2 == 1
#define WOLFSSL_MD2
#endif
//  </e>
//      <e>MD4
#define MDK_CONF_MD4 0
#if MDK_CONF_MD4 == 0
#define NO_MD4
#endif
//  </e>
//      <e>MD5
#define MDK_CONF_MD5 1
#if MDK_CONF_MD5 == 0
#define NO_MD5
#endif
//  </e>
//      <e>SHA
#define MDK_CONF_SHA  1
#if MDK_CONF_SHA == 0
#define NO_SHA
#endif
//  </e>
//      <e>SHA-256
#define MDK_CONF_SHA256 1
#if MDK_CONF_SHA256 == 0
#define NO_SHA256
#endif
//  </e>
//      <e>SHA-384
#define MDK_CONF_SHA384 1
#if MDK_CONF_SHA384 == 1
#define WOLFSSL_SHA384
#endif
//  </e>
//      <e>SHA-512
#define MDK_CONF_SHA512     1
#if MDK_CONF_SHA512     == 1
#define WOLFSSL_SHA512
#endif
//  </e>
//      <e>Hash DRBG
#define MDK_CONF_HASHDRBG 1
#if MDK_CONF_HASHDRBG == 1
#define HAVE_HASHDRBG
#endif
//  </e>
//      <e>RIPEMD
#define MDK_CONF_RIPEMD 0
#if MDK_CONF_RIPEMD == 1
#define WOLFSSL_RIPEMD
#endif
//  </e>
//      <e>BLAKE2
#define MDK_CONF_BLAKE2 0
#if MDK_CONF_BLAKE2 == 1
#define HAVE_BLAKE2
#endif
//  </e>
//      <e>HMAC
#define MDK_CONF_HMAC 1
#if MDK_CONF_HMAC == 0
#define NO_HMAC
#endif
//  </e>
//      <e>HMAC KDF
#define MDK_CONF_HKDF 1
#if MDK_CONF_HKDF == 1
#define HAVE_HKDF
#endif
//  </e>

//      <e>AES CCM
#define MDK_CONF_AESCCM 1
#if MDK_CONF_AESCCM == 1
#define HAVE_AESCCM
#endif
//  </e>
//      <e>AES GCM
#define MDK_CONF_AESGCM 1
#if MDK_CONF_AESGCM == 1
#define HAVE_AESGCM
#endif
//  </e>

//      <e>RC4
#define MDK_CONF_RC4 0
#if MDK_CONF_RC4 == 0
#define NO_RC4
#endif
//  </e>

//      <e>CHACHA
#define MDK_CONF_CHACHA 1
#if MDK_CONF_CHACHA == 1
#define HAVE_CHACHA
#endif
//  </e>

//      <e>POLY1305
#define MDK_CONF_POLY1305 1
#if MDK_CONF_POLY1305 == 1
#define HAVE_POLY1305
#define HAVE_ONE_TIME_AUTH
#endif
//  </e>

//      <e>DES3
#define MDK_CONF_DES3 1
#if MDK_CONF_DES3 == 0
#define NO_DES3
#endif
//  </e>

//      <e>AES
#define MDK_CONF_AES 1
#if MDK_CONF_AES == 0
#define NO_AES
#endif
//  </e>

//      <e>CAMELLIA
#define MDK_CONF_CAMELLIA 0
#if MDK_CONF_CAMELLIA == 1
#define HAVE_CAMELLIA
#endif
//  </e>

//      <e>DH
#define MDK_CONF_DH 1
#if MDK_CONF_DH == 0
#define NO_DH
#endif
//  </e>

//      <e>RSA
#define MDK_CONF_RSA 1
#if MDK_CONF_RSA == 1
/* #define RSA_LOW_MEM */
#else
#define NO_RSA
#endif
//  </e>


//      <e>DSA
#define MDK_CONF_DSA 0
#if MDK_CONF_DSA == 0
#define NO_DSA
#endif
//  </e>

//      <e>SRP
#define MDK_CONF_SRP 1
#if MDK_CONF_SRP == 1
#define WOLFCRYPT_HAVE_SRP
#endif
//  </e>

//      <e>PWDBASED
#define MDK_CONF_PWDBASED 1
#if MDK_CONF_PWDBASED == 0
#define NO_PWDBASED
#endif
//  </e>

//      <e>ECC
#define MDK_CONF_ECC 1
#if MDK_CONF_ECC == 1
#define HAVE_ECC
#endif
//  </e>

//      <e>CURVE25519
#define MDK_CONF_CURVE25519 1
#if MDK_CONF_CURVE25519 == 1
#define HAVE_CURVE25519
#define CURVED25519_SMALL
#endif
//  </e>
//      <e>CURVE25519 SMALL
#define MDK_CONF_CURVE25519_SMALL 0
#if MDK_CONF_CURVE25519_SMALL == 1
#define CURVED25519_SMALL
#endif
//  </e>
//      <e>ED25519
#define MDK_CONF_ED25519 1
#if MDK_CONF_ED25519 == 1
#define HAVE_ED25519
#endif
//  </e>
//      <e>ED25519 SMALL
#define MDK_CONF_ED25519_SMALL 0
#if MDK_CONF_ED25519_SMALL == 1
#define ED25519_SMALL
#endif
//  </e>
//      <e>PKCS7
#define MDK_CONF_PKCS7 0
#if MDK_CONF_PKCS7 == 1
#define HAVE_PKCS7
#endif
//  </e>
//  </h>

//      <e>Random Seed, for TEST Only
#define MDK_CONF_RNDSEED 1
#if MDK_CONF_RNDSEED == 1
#define WOLFSSL_GENSEED_FORTEST
#endif
//  </e>

//  <h>Hardware Crypt (See document for usage)
//      <e>Hardware RNG
#define MDK_CONF_STM32F2_RNG 0
#if MDK_CONF_STM32F2_RNG == 1
#define WOLFSSL_STM32_CUBEMX
#define STM32_RNG
#define WC_ASYNC_DEV_SIZE 320+24
#define STM32_HAL_TIMEOUT 0xFF

#if defined(STM32F2xx)
#define WOLFSSL_STM32F2
#elif defined(STM32F4xx)
#define WOLFSSL_STM32F4
#elif defined(STM32F7xx)
#define WOLFSSL_STM32F7
#endif

#endif
//  </e>
//      <e>Hardware Crypt
#define MDK_CONF_STM32F2_CRYPTO 0
#if MDK_CONF_STM32F2_CRYPTO == 1
#define WOLFSSL_STM32_CUBEMX
#define STM32_CRYPTO
#define WC_ASYNC_DEV_SIZE 320+24
#define STM32_HAL_TIMEOUT 0xFF

#if defined(STM32F2xx)
#define WOLFSSL_STM32F2
#elif defined(STM32F4xx)
#define WOLFSSL_STM32F4
#elif defined(STM32F7xx)
#define WOLFSSL_STM32F7
#endif

#endif
//  </e>
//      <e>Hardware Hash
#define MDK_CONF_STM32F2_HASH 0
#if MDK_CONF_STM32F2_HASH == 1
#define WOLFSSL_STM32_CUBEMX
#define STM32_HASH
#define WC_ASYNC_DEV_SIZE 320+24
#define STM32_HAL_TIMEOUT 0xFF

#if defined(STM32F2xx)
#define WOLFSSL_STM32F2
#elif defined(STM32F4xx)
#define WOLFSSL_STM32F4
#elif defined(STM32F7xx)
#define WOLFSSL_STM32F7
#endif

#endif
//  </e>
// </h>

//  <h>Cert/Key Storage
//        <o>Cert Storage <0=> SD Card <1=> Mem Buff (1024bytes) <2=> Mem Buff (2048bytes)
#define MDK_CONF_CERT_BUFF 0
#if MDK_CONF_CERT_BUFF== 1
#define USE_CERT_BUFFERS_1024
#elif MDK_CONF_CERT_BUFF == 2
#define USE_CERT_BUFFERS_2048
#endif
//</h>
//  <h>Cert/Key Generation
//      <e>CertGen
#define MDK_CONF_CERT_GEN 0
#if MDK_CONF_CERT_GEN == 1
#define WOLFSSL_CERT_GEN
#endif
//  </e>
//      <e>KeyGen
#define MDK_CONF_KEY_GEN 0
#if MDK_CONF_KEY_GEN == 1
#define WOLFSSL_KEY_GEN
#define WOLFSSL_OLD_PRIME_CHECK /* use older prime check (faster) */
#endif
//  </e>
// </h>

// <h>Math / Memory
//      <o> Math Library
//        <0=>SP Math All (sp_int.c)
//        <1=>Fast Math (tfm.c)
//        <2=>Heap Math (integer.c)
//        <3=>SP Math (RSA/DH 2048/3072/4096 and ECC 256/384/521 only)
//        <4=>SP Math +ASM (faster)
#define MDK_CONF_MATH 0
#if MDK_CONF_MATH == 0
    #define WOLFSSL_SP_MATH_ALL /* use SP math for all key sizes and curves */
#elif MDK_CONF_MATH == 1
    #define USE_FAST_MATH
#elif MDK_CONF_MATH == 2
    #define USE_INTEGER_HEAP_MATH
#elif MDK_CONF_MATH == 3 || MDK_CONF_MATH == 4
    #define WOLFSSL_SP_MATH     /* only SP math - disables integer.c/tfm.c */
    #define WOLFSSL_HAVE_SP_RSA
    #define WOLFSSL_HAVE_SP_DH
    #define WOLFSSL_HAVE_SP_ECC

    //#define WOLFSSL_SP_NO_2048
    //#define WOLFSSL_SP_NO_3072
    #define WOLFSSL_SP_4096
    //#define WOLFSSL_SP_NO_256
    //#define WOLFSSL_SP_384
    //#define WOLFSSL_SP_521

    #define WOLFSSL_SP_SMALL /* use smaller version of code */
    //#define WOLFSSL_SP_NO_MALLOC /* do not use heap */
    //#define WOLFSSL_SP_CACHE_RESISTANT
    //#define WOLFSSL_SP_DIV_32 /* do not use 64-bit divides */

    #if MDK_CONF_MATH == 4
        /* SP Assembly Speedups - specific to chip type */
        #define WOLFSSL_SP_ASM

        //#define WOLFSSL_SP_ARM32_ASM
        //#define WOLFSSL_SP_ARM64_ASM
        //#define WOLFSSL_SP_ARM_THUMB_ASM
        //#define WOLFSSL_SP_ARM_CORTEX_M_ASM
    #endif
#endif

//      <e>Small Stack
#define MDK_CONF_SmallStack 1
#if MDK_CONF_SmallStack == 0
    #define NO_WOLFSSL_SMALL_STACK
#else
    #define WOLFSSL_SMALL_STACK
#endif
//      </e>
//  </h>
//  </h>

/**** wolfSSL Configuration ****/

// <h> wolfSSL Configuration

//      <e>TLS 1.3
#define MDK_CONF_TLS 1
#if MDK_CONF_TLS == 1
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define WC_RSA_PSS
#define HAVE_HKDF
#define HAVE_FFDHE_2048
//#define HAVE_FFDHE_3072
#endif
//  </e>

//      <e>Include Old TLS
#define MDK_CONF_NO_OLDTLS 0
#if MDK_CONF_NO_OLDTLS == 0
#define NO_OLD_TLS
#endif
//  </e>
//      <e>CRL
#define MDK_CONF_CRL 0
#if MDK_CONF_CRL == 1
#define HAVE_CRL
#define WOLFSSL_DER_LOAD
#endif
//  </e>
//      <e>OCSP
#define MDK_CONF_OCSP 0
#if MDK_CONF_OCSP == 1
#define HAVE_OCSP
#endif
//  </e>
//      <e>OpenSSL Extra
#define MDK_CONF_OPENSSL_EXTRA 0
#if MDK_CONF_OPENSSL_EXTRA == 1
#define OPENSSL_EXTRA
#endif
//  </e>

//</h>
