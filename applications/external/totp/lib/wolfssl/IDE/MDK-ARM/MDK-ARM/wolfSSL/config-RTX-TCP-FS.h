/* config-RTX-TCP-FS.h
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




/**** wolfSSL for MDK-RTX-TCP-FS  Configuration ****/

#define __CORTEX_M3__
#define WOLFSSL_MDK_ARM
#define NO_WRITEV
#define NO_WOLFSSL_DIR
#define NO_MAIN_DRIVER

#define WOLFSSL_DER_LOAD
#define HAVE_NULL_CIPHER

#define HAVE_KEIL_RTX
#define WOLFSSL_KEIL_TCP_NET


// <<< Use Configuration Wizard in Context Menu >>>
// <h> Build Target: KEIL-RTX-TCP-FS
//  <h> RTOS, File System and TCP-net
//  </h>
//      <e>Command Shell
#define MDK_CONF_SHELL 1
#if MDK_CONF_SHELL  == 1
#define WOLFSSL_MDK_SHELL
#endif
//  </e>
//  <h>wolfSSL Apps
//  <h>Crypt/Cipher
//        <o>Cert Storage <0=> SD Card <1=> Mem Buff (1024bytes) <2=> Mem Buff (2048bytes)
#define MDK_CONF_CERT_BUFF 0
#if MDK_CONF_CERT_BUFF== 1
#define USE_CERT_BUFFERS_1024
#elif MDK_CONF_CERT_BUFF == 2
#define USE_CERT_BUFFERS_2048
#endif

//      <e>Crypt/Cipher Test Suite
#define MDK_CONF_CTaoCryptTest 1
#if MDK_CONF_CTaoCryptTest  == 0
#define NO_CRYPT_TEST
#endif
//  </e>
//      <e>Crypt/Cipher Benchmark
#define MDK_CONF_CTaoCryptBenchmark 1
#if MDK_CONF_CTaoCryptBenchmark == 0
#define NO_CRYPT_BENCHMARK
#define BENCH_EMBEDDED
#endif
//  </e>
//   </h>
//   <h>SSL/TLS Server/Client
//      <e>echoServer
#define MDK_CONF_echoServer 1
#if MDK_CONF_echoServer == 0
#define NO_ECHOSERVER
#endif
//  </e>
//      <e>echoClient
#define MDK_CONF_echoClient 1
#if MDK_CONF_echoClient == 0
#define NO_ECHOCLIENT
#endif
//  </e>
//      <e>SimpleServer
#define MDK_CONF_simpleServer 1
#if MDK_CONF_simpleServer == 0
#define NO_SIMPLE_SERVER
#endif
//  </e>
//      <e>SimpleCliet
#define MDK_CONF_simpleClient 1
#if MDK_CONF_simpleClient == 0
#define NO_SIMPLE_CLIENT
#endif
//  </e>
// </h>
// </h>
//  <h>STM32 Hardware Crypt
//      <e>STM32F2 Hardware RNG
#define MDK_CONF_STM32F2_RNG 0
#if MDK_CONF_STM32F2_RNG == 1
#define STM32F2_RNG
#else
#define NO_DEV_RANDOM
#endif
//  </e>
//      <e>STM32F2 Hardware Crypt
#define MDK_CONF_STM32F2_CRYPTO 0
#if MDK_CONF_STM32F2_CRYPTO == 1
#define STM32F2_CRYPTO
#endif
//  </e>

// </h>

//  <h>wolfSSL Library
//     <h>SSL (Included by default)
//     </h>

//      <e>TLS
#define MDK_CONF_TLS 1
#if MDK_CONF_TLS == 0
#define NO_TLS
#endif
//  </e>

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
#endif
//  </e>
//      <e>CRL
#define MDK_CONF_DER_LOAD 0
#if MDK_CONF_DER_LOAD == 1
#define WOLFSSL_DER_LOAD
#endif
//  </e>
//      <e>OpenSSL Extra
#define MDK_CONF_OPENSSL_EXTRA 0
#if MDK_CONF_OPENSSL_EXTRA == 1
#define OPENSSL_EXTRA
#endif
//  </e>
//      <h>CRL Monitor, OCSP (not supported with KEIL)
//     </h>

// </h>

//  <h>wolfCrypt Library

//       <h>MD5, SHA, SHA-256, AES, RC4, ASN, RSA
//        </h>
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
//      <e>SHA-384
//          <i>This has to be with SHA512
#define MDK_CONF_SHA384 0
#if MDK_CONF_SHA384 == 1
#define WOLFSSL_SHA384
#endif
//  </e>
//      <e>SHA-512
#define MDK_CONF_SHA512     0
#if MDK_CONF_SHA512     == 1
#define WOLFSSL_SHA512
#endif
//  </e>
//      <e>RIPEMD
#define MDK_CONF_RIPEMD 0
#if MDK_CONF_RIPEMD == 1
#define WOLFSSL_RIPEMD
#endif
//  </e>
//      <e>HMAC
#define MDK_CONF_HMAC 1
#if MDK_CONF_HMAC == 0
#define NO_HMAC
#endif
//  </e>
//      <e>AEAD
#define MDK_CONF_AEAD 0
#if MDK_CONF_AEAD == 1
#define HAVE_AEAD
#endif
//  </e>
//      <e>DES3
#define MDK_CONF_DES3 0
#if MDK_CONF_DES3 == 0
#define NO_DES3
#endif
//  </e>
//      <e>CAMELLIA
#define MDK_CONF_CAMELLIA 0
#if MDK_CONF_CAMELLIA == 1
#define HAVE_CAMELLIA
#endif
//  </e>

//      <e>DH
//              <i>need this for WOLFSSL_SERVER, OPENSSL_EXTRA
#define MDK_CONF_DH 1
#if MDK_CONF_DH == 0
#define NO_DH
#endif
//  </e>
//      <e>DSA
#define MDK_CONF_DSA 0
#if MDK_CONF_DSA == 0
#define NO_DSA
#endif
//  </e>
//      <e>PWDBASED
#define MDK_CONF_PWDBASED 1
#if MDK_CONF_PWDBASED == 0
#define NO_PWDBASED
#endif
//  </e>

//      <e>ECC
#define MDK_CONF_ECC 0
#if MDK_CONF_ECC == 1
#define HAVE_ECC
#endif
//  </e>
//      <e>PSK
#define MDK_CONF_PSK 1
#if MDK_CONF_PSK == 0
#define NO_PSK
#endif
//  </e>
//      <e>AESCCM (Turn off Hardware Crypt)
#define MDK_CONF_AESCCM 0
#if MDK_CONF_AESCCM == 1
#define HAVE_AESCCM
#endif
//  </e>
//      <e>AESGCM (Turn off Hardware Crypt)
#define MDK_CONF_AESGCM 0
#if MDK_CONF_AESGCM == 1
#define HAVE_AESGCM
#define BUILD_AESGCM
#endif
//  </e>
//  </h>

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
    //#define WOLFSSL_SP_NO_MALLOC
#else
    #define WOLFSSL_SMALL_STACK
#endif
//      </e>
//  </h>

//  <h>Others

//      <e>Inline
#define MDK_CONF_INLINE 0
#if MDK_CONF_INLINE == 0
#define NO_INLINE
#endif
//  </e>
//      <h>Debug
//              <e>Debug Message
#define MDK_CONF_DEBUG_MSG 0
#if MDK_CONF_DEBUG_MSG == 1
#define DEBUG_WOLFSSL
#endif
//         </e>
//              <e>Check malloc
#define MDK_CONF_CHECK_MALLOC 1
#if MDK_CONF_CHECK_MALLOC == 1
#define WOLFSSL_MALLOC_CHECK
#endif
//         </e>


//  </h>
//      <e>ErrNo.h
#define MDK_CONF_ERR_NO 0
#if MDK_CONF_ERR_NO == 1
#define HAVE_ERRNO
#endif
//  </e>
//      <e>zlib (need "zlib.h")
#define MDK_CONF_LIBZ 0
#if MDK_CONF_LIBZ == 1
#define HAVE_LIBZ
#endif
//  </e>
//      <e>CAVIUM (need CAVIUM headers)
#define MDK_CONF_CAVIUM 0
#if MDK_CONF_CAVIUM == 1
#define HAVE_CAVIUM
#endif
//  </e>

//      <e>Error Strings
#define MDK_CONF_ErrorStrings 0
#if MDK_CONF_ErrorStrings == 0
#define NO_ERROR_STRINGS
#endif
//  </e>
//  </h>

//</h>
// <<< end of configuration section >>>

