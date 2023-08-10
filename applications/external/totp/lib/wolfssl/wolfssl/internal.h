/* internal.h
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



#ifndef WOLFSSL_INT_H
#define WOLFSSL_INT_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/ssl.h>
#ifdef HAVE_CRL
    #include <wolfssl/crl.h>
#endif
#include <wolfssl/wolfcrypt/random.h>
#ifndef NO_DES3
    #include <wolfssl/wolfcrypt/des3.h>
#endif
#ifdef HAVE_CHACHA
    #include <wolfssl/wolfcrypt/chacha.h>
#endif
#ifndef NO_ASN
    #include <wolfssl/wolfcrypt/asn.h>
    #include <wolfssl/wolfcrypt/pkcs12.h>
#endif
#ifndef NO_MD5
    #include <wolfssl/wolfcrypt/md5.h>
#endif
#ifndef NO_SHA
    #include <wolfssl/wolfcrypt/sha.h>
#endif
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif
#ifdef HAVE_POLY1305
    #include <wolfssl/wolfcrypt/poly1305.h>
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && defined(OPENSSL_EXTRA)
    #include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#endif
#ifdef HAVE_ARIA
    #include <wolfssl/wolfcrypt/port/aria/aria-crypt.h>
#endif
#ifdef HAVE_CAMELLIA
    #include <wolfssl/wolfcrypt/camellia.h>
#endif
#ifdef WOLFSSL_SM4
    #include <wolfssl/wolfcrypt/sm4.h>
#endif
#include <wolfssl/wolfcrypt/logging.h>
#ifndef NO_HMAC
    #include <wolfssl/wolfcrypt/hmac.h>
#endif
#ifndef NO_RC4
    #include <wolfssl/wolfcrypt/arc4.h>
#endif
#ifndef NO_SHA256
    #include <wolfssl/wolfcrypt/sha256.h>
#endif
#if defined(WOLFSSL_SHA384)
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef HAVE_OCSP
    #include <wolfssl/ocsp.h>
#endif
#ifdef WOLFSSL_QUIC
    #include <wolfssl/quic.h>
#endif
#ifdef WOLFSSL_SHA384
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef WOLFSSL_SHA512
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef WOLFSSL_SM3
    #include <wolfssl/wolfcrypt/sm3.h>
#endif
#ifdef HAVE_AESGCM
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef WOLFSSL_RIPEMD
    #include <wolfssl/wolfcrypt/ripemd.h>
#endif
#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifdef WOLFSSL_SM2
    #include <wolfssl/wolfcrypt/sm2.h>
#endif
#ifndef NO_DH
    #include <wolfssl/wolfcrypt/dh.h>
#endif
#ifdef HAVE_ED25519
    #include <wolfssl/wolfcrypt/ed25519.h>
#endif
#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif
#ifdef HAVE_ED448
    #include <wolfssl/wolfcrypt/ed448.h>
#endif
#ifdef HAVE_CURVE448
    #include <wolfssl/wolfcrypt/curve448.h>
#endif
#ifdef HAVE_PQC
    #include <wolfssl/wolfcrypt/falcon.h>
    #include <wolfssl/wolfcrypt/dilithium.h>
#endif
#ifdef HAVE_HKDF
    #include <wolfssl/wolfcrypt/kdf.h>
#endif
#ifndef WOLFSSL_NO_DEF_TICKET_ENC_CB
    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && \
        !defined(WOLFSSL_TICKET_ENC_AES128_GCM) && \
        !defined(WOLFSSL_TICKET_ENC_AES256_GCM)
        #include <wolfssl/wolfcrypt/chacha20_poly1305.h>
    #else
        #include <wolfssl/wolfcrypt/aes.h>
    #endif
#endif

#include <wolfssl/wolfcrypt/wc_encrypt.h>
#include <wolfssl/wolfcrypt/hash.h>

#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    #include <wolfssl/callbacks.h>
#endif
#ifdef WOLFSSL_CALLBACKS
    #include <signal.h>
#endif

#ifdef USE_WINDOWS_API
    #ifdef WOLFSSL_GAME_BUILD
        #include "system/xtl.h"
    #else
        #if defined(_WIN32_WCE) || defined(WIN32_LEAN_AND_MEAN)
            /* On WinCE winsock2.h must be included before windows.h */
            #include <winsock2.h>
        #endif
        #include <windows.h>
    #endif
#elif defined(THREADX)
    #ifndef SINGLE_THREADED
        #include "tx_api.h"
    #endif

#elif defined(WOLFSSL_DEOS)
    /* do nothing, just don't pick Unix */
#elif defined(MICRIUM)
    /* do nothing, just don't pick Unix */
#elif defined(FREERTOS) || defined(FREERTOS_TCP) || defined(WOLFSSL_SAFERTOS)
    /* do nothing */
#elif defined(RTTHREAD)
    /* do nothing */
#elif defined(EBSNET)
    /* do nothing */
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    /* do nothing */
#elif defined(FREESCALE_FREE_RTOS)
    #include "fsl_os_abstraction.h"
#elif defined(WOLFSSL_uITRON4)
        /* do nothing */
#elif defined(WOLFSSL_uTKERNEL2)
        /* do nothing */
#elif defined(WOLFSSL_CMSIS_RTOS)
    #include "cmsis_os.h"
#elif defined(WOLFSSL_CMSIS_RTOSv2)
    #include "cmsis_os2.h"
#elif defined(WOLFSSL_MDK_ARM)
    #if defined(WOLFSSL_MDK5)
        #include "cmsis_os.h"
    #else
        #include <rtl.h>
    #endif
#elif defined(MBED)
#elif defined(WOLFSSL_TIRTOS)
    /* do nothing */
#elif defined(INTIME_RTOS)
    #include <rt.h>
#elif defined(WOLFSSL_NUCLEUS_1_2)
    /* do nothing */
#elif defined(WOLFSSL_APACHE_MYNEWT)
    #if !defined(WOLFSSL_LWIP)
        void mynewt_ctx_clear(void *ctx);
        void* mynewt_ctx_new();
    #endif
#elif defined(WOLFSSL_ZEPHYR)
    #ifndef SINGLE_THREADED
        #include <zephyr/kernel.h>
    #endif
#elif defined(WOLFSSL_TELIT_M2MB)
    /* do nothing */
#elif defined(WOLFSSL_EMBOS)
    /* do nothing */
#else
    #ifndef SINGLE_THREADED
        #if defined(WOLFSSL_LINUXKM)
            /* setup is in linuxkm/linuxkm_wc_port.h */
        #elif defined(WOLFSSL_USER_MUTEX)
            /* do nothing */
        #else
            #define WOLFSSL_PTHREADS
            #include <pthread.h>
        #endif
    #endif
    #if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM)
        #ifdef FUSION_RTOS
           #include <fclunistd.h>
        #else
            #include <unistd.h>      /* for close of BIO */
        #endif
    #endif
#endif

#if !defined(CHAR_BIT) || (defined(OPENSSL_EXTRA) && !defined(INT_MAX))
    /* Needed for DTLS without big math and INT_MAX */
    #include <limits.h>
#endif


#ifdef HAVE_LIBZ
    #include "zlib.h"
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef OPENSSL_EXTRA
    #ifdef WOLFCRYPT_HAVE_SRP
        #include <wolfssl/wolfcrypt/srp.h>
    #endif
#endif

#ifdef _MSC_VER
    /* 4996 warning to use MS extensions e.g., strcpy_s instead of strncpy */
    #pragma warning(disable: 4996)
#endif

#ifdef NO_SHA
    #define WC_SHA_DIGEST_SIZE 20
#endif

#ifdef NO_SHA256
    #define WC_SHA256_DIGEST_SIZE 32
#endif

#ifdef NO_MD5
    #define WC_MD5_DIGEST_SIZE 16
#endif

#ifdef WOLFSSL_IOTSAFE
    #include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>
#endif

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
#endif

#include <wolfssl/wolfcrypt/hpke.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Define or comment out the cipher suites you'd like to be compiled in
   make sure to use at least one BUILD_SSL_xxx or BUILD_TLS_xxx is defined

   When adding cipher suites, add name to cipher_names, idx to cipher_name_idx

   Now that there is a maximum strength crypto build, the following BUILD_XXX
   flags need to be divided into two groups selected by WOLFSSL_MAX_STRENGTH.
   Those that do not use Perfect Forward Security and do not use AEAD ciphers
   need to be switched off. Allowed suites use (EC)DHE, AES-GCM|CCM, or
   CHACHA-POLY.
*/

/* Check that if WOLFSSL_MAX_STRENGTH is set that all the required options are
 * not turned off. */
#if defined(WOLFSSL_MAX_STRENGTH) && \
    ((!defined(HAVE_ECC) && (defined(NO_DH) || defined(NO_RSA))) || \
     (!defined(HAVE_AESGCM) && !defined(HAVE_AESCCM) && \
      (!defined(HAVE_POLY1305) || !defined(HAVE_CHACHA))) || \
     (defined(NO_SHA256) && !defined(WOLFSSL_SHA384)) || \
     !defined(NO_OLD_TLS))

    #error "You are trying to build max strength with requirements disabled."
#endif

#ifndef WOLFSSL_NO_TLS12

#ifndef WOLFSSL_MAX_STRENGTH

#ifdef WOLFSSL_AEAD_ONLY
    /* AES CBC ciphers are not allowed in AEAD only mode */
    #undef HAVE_AES_CBC
#endif

/* When adding new ciphersuites, make sure that they have appropriate
 * guards for WOLFSSL_HARDEN_TLS. */
#if defined(WOLFSSL_HARDEN_TLS) && \
    !defined(WOLFSSL_HARDEN_TLS_ALLOW_ALL_CIPHERSUITES)
/* Use a separate define (undef'ed later) to simplify macro logic. */
#define WSSL_HARDEN_TLS WOLFSSL_HARDEN_TLS
#define NO_TLS_DH
#endif

#ifndef WOLFSSL_AEAD_ONLY
    #if !defined(NO_RSA) && !defined(NO_RC4) && !defined(WSSL_HARDEN_TLS)
        /* MUST NOT negotiate RC4 cipher suites
         * https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
        #if defined(WOLFSSL_STATIC_RSA)
            #if !defined(NO_SHA)
                #define BUILD_SSL_RSA_WITH_RC4_128_SHA
            #endif
            #if !defined(NO_MD5)
                #define BUILD_SSL_RSA_WITH_RC4_128_MD5
            #endif
        #endif
    #endif

    #if !defined(NO_RSA) && !defined(NO_DES3)
        #if !defined(NO_SHA)
            #if defined(WOLFSSL_STATIC_RSA)
                #define BUILD_SSL_RSA_WITH_3DES_EDE_CBC_SHA
            #endif
        #endif
    #endif
#endif /* !WOLFSSL_AEAD_ONLY */

    #if !defined(NO_RSA) && !defined(NO_AES) && !defined(NO_TLS)
        #if !defined(NO_SHA) && defined(HAVE_AES_CBC)
            #if defined(WOLFSSL_STATIC_RSA)
                #ifdef WOLFSSL_AES_128
                    #define BUILD_TLS_RSA_WITH_AES_128_CBC_SHA
                #endif
                #ifdef WOLFSSL_AES_256
                    #define BUILD_TLS_RSA_WITH_AES_256_CBC_SHA
                #endif
            #endif
        #endif
        #if defined(WOLFSSL_STATIC_RSA)
            #if !defined (NO_SHA256) && defined(HAVE_AES_CBC)
                #ifdef WOLFSSL_AES_128
                    #define BUILD_TLS_RSA_WITH_AES_128_CBC_SHA256
                #endif
                #ifdef WOLFSSL_AES_256
                    #define BUILD_TLS_RSA_WITH_AES_256_CBC_SHA256
                #endif
            #endif
            #if defined (HAVE_AESGCM)
                #ifdef WOLFSSL_AES_128
                    #define BUILD_TLS_RSA_WITH_AES_128_GCM_SHA256
                #endif
                #if defined (WOLFSSL_SHA384) && defined(WOLFSSL_AES_256)
                    #define BUILD_TLS_RSA_WITH_AES_256_GCM_SHA384
                #endif
            #endif
            #if defined (HAVE_AESCCM)
                #ifdef WOLFSSL_AES_128
                    #define BUILD_TLS_RSA_WITH_AES_128_CCM_8
                #endif
                #ifdef WOLFSSL_AES_256
                    #define BUILD_TLS_RSA_WITH_AES_256_CCM_8
                #endif
            #endif
        #endif
    #endif

    #if defined(HAVE_CAMELLIA) && !defined(NO_TLS) && !defined(NO_CAMELLIA_CBC)
        #ifndef NO_RSA
          #if defined(WOLFSSL_STATIC_RSA)
            #if !defined(NO_SHA)
                #define BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
                #define BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
            #endif
            #ifndef NO_SHA256
                #define BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
                #define BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
            #endif
          #endif
            #if !defined(NO_DH) && !defined(NO_TLS_DH)
              /* SHOULD NOT negotiate cipher suites based on ephemeral
               * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
               * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
              #if !defined(NO_SHA)
                #define BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
                #define BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
              #endif
                #ifndef NO_SHA256
                    #define BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
                    #define BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
                #endif
            #endif
        #endif
    #endif

#if defined(WOLFSSL_STATIC_PSK)
    #if !defined(NO_PSK) && !defined(NO_AES) && !defined(NO_TLS)
        #if !defined(NO_SHA)
            #ifdef WOLFSSL_AES_128
                #define BUILD_TLS_PSK_WITH_AES_128_CBC_SHA
            #endif
            #ifdef WOLFSSL_AES_256
                #define BUILD_TLS_PSK_WITH_AES_256_CBC_SHA
            #endif
        #endif
        #ifndef NO_SHA256
            #ifdef WOLFSSL_AES_128
                #ifdef HAVE_AES_CBC
                    #define BUILD_TLS_PSK_WITH_AES_128_CBC_SHA256
                #endif
                #ifdef HAVE_AESGCM
                    #define BUILD_TLS_PSK_WITH_AES_128_GCM_SHA256
                #endif
            #endif /* WOLFSSL_AES_128 */
            #ifdef HAVE_AESCCM
                #ifdef WOLFSSL_AES_128
                    #define BUILD_TLS_PSK_WITH_AES_128_CCM_8
                    #define BUILD_TLS_PSK_WITH_AES_128_CCM
                #endif
                #ifdef WOLFSSL_AES_256
                    #define BUILD_TLS_PSK_WITH_AES_256_CCM_8
                    #define BUILD_TLS_PSK_WITH_AES_256_CCM
                #endif
            #endif
        #endif
        #if defined(WOLFSSL_SHA384) && defined(WOLFSSL_AES_256)
            #ifdef HAVE_AES_CBC
                #define BUILD_TLS_PSK_WITH_AES_256_CBC_SHA384
            #endif
            #ifdef HAVE_AESGCM
                #define BUILD_TLS_PSK_WITH_AES_256_GCM_SHA384
            #endif
        #endif
    #endif
#endif

    #if !defined(NO_TLS) && defined(HAVE_NULL_CIPHER)
        #if !defined(NO_RSA)
            #if defined(WOLFSSL_STATIC_RSA)
                #ifndef NO_MD5
                    #define BUILD_TLS_RSA_WITH_NULL_MD5
                #endif
                #if !defined(NO_SHA)
                    #define BUILD_TLS_RSA_WITH_NULL_SHA
                #endif
                #ifndef NO_SHA256
                    #define BUILD_TLS_RSA_WITH_NULL_SHA256
                #endif
            #endif
        #endif
        #if !defined(NO_PSK) && defined(WOLFSSL_STATIC_PSK)
            #if !defined(NO_SHA)
                #define BUILD_TLS_PSK_WITH_NULL_SHA
            #endif
            #ifndef NO_SHA256
                #define BUILD_TLS_PSK_WITH_NULL_SHA256
            #endif
            #ifdef WOLFSSL_SHA384
                #define BUILD_TLS_PSK_WITH_NULL_SHA384
            #endif
        #endif
    #endif

    #if !defined(NO_DH) && !defined(NO_AES) && !defined(NO_TLS) && \
        !defined(NO_RSA) && !defined(NO_TLS_DH)
        /* SHOULD NOT negotiate cipher suites based on ephemeral
         * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
         * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */

        #if !defined(NO_SHA)
            #if defined(WOLFSSL_AES_128) && defined(HAVE_AES_CBC)
                #define BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
            #endif
            #if defined(WOLFSSL_AES_256) && defined(HAVE_AES_CBC)
                #define BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
            #endif
            #if !defined(NO_DES3)
                #define BUILD_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
            #endif
        #endif
        #if !defined(NO_SHA256) && defined(HAVE_AES_CBC)
            #ifdef WOLFSSL_AES_128
                #define BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
            #endif
            #ifdef WOLFSSL_AES_256
                #define BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
            #endif
        #endif
    #endif

    #if defined(HAVE_ANON) && !defined(NO_TLS) && !defined(NO_DH) && \
        !defined(NO_AES) && !defined(NO_SHA) && defined(WOLFSSL_AES_128)
        #ifdef HAVE_AES_CBC
            #define BUILD_TLS_DH_anon_WITH_AES_128_CBC_SHA
        #endif

        #if defined(WOLFSSL_SHA384) && defined(HAVE_AESGCM)
            #define BUILD_TLS_DH_anon_WITH_AES_256_GCM_SHA384
        #endif
    #endif

    #if !defined(NO_DH) && !defined(NO_PSK) && !defined(NO_TLS) && \
        !defined(NO_TLS_DH)
        /* SHOULD NOT negotiate cipher suites based on ephemeral
         * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
         * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
        #ifndef NO_SHA256
            #if !defined(NO_AES) && defined(WOLFSSL_AES_128) && \
                                                           defined(HAVE_AES_CBC)
                #define BUILD_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
            #endif
            #ifdef HAVE_NULL_CIPHER
                #define BUILD_TLS_DHE_PSK_WITH_NULL_SHA256
            #endif
        #endif
        #ifdef WOLFSSL_SHA384
            #if !defined(NO_AES) && defined(WOLFSSL_AES_256) && \
                                                           defined(HAVE_AES_CBC)
                #define BUILD_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
            #endif
            #ifdef HAVE_NULL_CIPHER
                #define BUILD_TLS_DHE_PSK_WITH_NULL_SHA384
            #endif
        #endif
    #endif

    #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                                     defined(HAVE_CURVE448)) && !defined(NO_TLS)
        #if !defined(NO_AES)
            #if !defined(NO_SHA) && defined(HAVE_AES_CBC)
                #if !defined(NO_RSA)
                    #ifdef WOLFSSL_AES_128
                        #define BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
                    #endif
                    #ifdef WOLFSSL_AES_256
                        #define BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
                    #endif
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #ifdef WOLFSSL_AES_128
                            #define BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
                        #endif
                        #ifdef WOLFSSL_AES_256
                            #define BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
                        #endif
                    #endif
                #endif

                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #ifdef WOLFSSL_AES_128
                        #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
                    #endif
                    #ifdef WOLFSSL_AES_256
                        #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
                    #endif
                #endif

                #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                    #ifdef WOLFSSL_AES_128
                        #define BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
                    #endif
                    #ifdef WOLFSSL_AES_256
                        #define BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
                    #endif
                #endif
            #endif /* NO_SHA */
            #if !defined(NO_SHA256) && defined(WOLFSSL_AES_128) && \
                                                           defined(HAVE_AES_CBC)
                #if !defined(NO_RSA)
                    #define BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #define BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
                    #endif
                #endif
                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
                #endif
                #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                    #define BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
                #endif
            #endif

            #if defined(WOLFSSL_SHA384) && defined(WOLFSSL_AES_256) && \
                                                           defined(HAVE_AES_CBC)
                #if !defined(NO_RSA)
                    #define BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #define BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
                    #endif
                #endif
                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
                #endif
                #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                    #define BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
                #endif
            #endif

            #if defined (HAVE_AESGCM)
                #if !defined(NO_RSA)
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #ifdef WOLFSSL_AES_128
                            #define BUILD_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
                        #endif
                    #endif
                    #if defined(WOLFSSL_SHA384)
                        #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                            #ifdef WOLFSSL_AES_256
                                #define BUILD_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
                            #endif
                        #endif
                    #endif
                #endif

                #if defined(WOLFSSL_STATIC_DH) && defined(WOLFSSL_AES_128) && \
                                                               defined(HAVE_ECC)
                    #define BUILD_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
                #endif

                #if defined(WOLFSSL_SHA384)
                    #if defined(WOLFSSL_STATIC_DH) && \
                                   defined(WOLFSSL_AES_256) && defined(HAVE_ECC)
                        #define BUILD_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
                    #endif
                #endif
            #endif
        #endif /* NO_AES */
        #ifdef HAVE_ARIA
            #define BUILD_TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256
            #define BUILD_TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384
        #endif /* HAVE_ARIA */
        #if !defined(NO_RC4) && !defined(WSSL_HARDEN_TLS)
            /* MUST NOT negotiate RC4 cipher suites
             * https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
            #if !defined(NO_SHA)
                #if !defined(NO_RSA)
                    #ifndef WOLFSSL_AEAD_ONLY
                        #define BUILD_TLS_ECDHE_RSA_WITH_RC4_128_SHA
                    #endif
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #define BUILD_TLS_ECDH_RSA_WITH_RC4_128_SHA
                    #endif
                #endif

                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #ifndef WOLFSSL_AEAD_ONLY
                        #define BUILD_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA
                    #endif
                #endif
                #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                    #define BUILD_TLS_ECDH_ECDSA_WITH_RC4_128_SHA
                #endif
            #endif
        #endif
        #if !defined(NO_DES3) && !(defined(WSSL_HARDEN_TLS) && \
                                           WSSL_HARDEN_TLS > 112)
            /* 3DES offers only 112 bits of security.
             * Using guidance from section 5.6.1
             * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-57pt1r5.pdf */
            #ifndef NO_SHA
                #if !defined(NO_RSA)
                    #define BUILD_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
                    #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                        #define BUILD_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
                    #endif
                #endif

                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #define BUILD_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
                #endif
                #if defined(WOLFSSL_STATIC_DH) && defined(HAVE_ECC)
                    #define BUILD_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
                #endif
            #endif /* NO_SHA */
        #endif
        #if defined(HAVE_NULL_CIPHER)
            #if !defined(NO_SHA)
                #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                    #define BUILD_TLS_ECDHE_ECDSA_WITH_NULL_SHA
                #endif
            #endif
            #if !defined(NO_PSK) && !defined(NO_SHA256)
                #define BUILD_TLS_ECDHE_PSK_WITH_NULL_SHA256
            #endif
        #endif
        #if !defined(NO_PSK) && !defined(NO_SHA256) && !defined(NO_AES) && \
            defined(WOLFSSL_AES_128) && defined(HAVE_AES_CBC)
            #define BUILD_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
        #endif
        #if !defined(NO_PSK) && !defined(NO_SHA256) && !defined(NO_AES) && \
            defined(WOLFSSL_AES_128) && defined(HAVE_AESGCM)
            #define BUILD_TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256
        #endif
    #endif
    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && !defined(NO_SHA256)
        #if !defined(NO_OLD_POLY1305)
        #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
            #define BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256
        #endif
        #if !defined(NO_RSA) && defined(HAVE_ECC)
            #define BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
        #endif
        #if !defined(NO_DH) && !defined(NO_RSA) && !defined(NO_TLS_DH)
            /* SHOULD NOT negotiate cipher suites based on ephemeral
             * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
             * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
            #define BUILD_TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
        #endif
        #endif /* NO_OLD_POLY1305 */
        #if !defined(NO_PSK)
            #define BUILD_TLS_PSK_WITH_CHACHA20_POLY1305_SHA256
            #if defined(HAVE_ECC) || defined(HAVE_ED25519) || \
                                                             defined(HAVE_ED448)
                #define BUILD_TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256
            #endif
            #if !defined(NO_DH) && !defined(NO_TLS_DH)
                /* SHOULD NOT negotiate cipher suites based on ephemeral
                 * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
                 * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
                #define BUILD_TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256
            #endif
        #endif /* !NO_PSK */
    #endif

#endif /* !WOLFSSL_MAX_STRENGTH */

#if !defined(NO_DH) && !defined(NO_AES) && !defined(NO_TLS) && \
    !defined(NO_RSA) && defined(HAVE_AESGCM) && !defined(NO_TLS_DH)
    /* SHOULD NOT negotiate cipher suites based on ephemeral
     * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
     * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */

    #if !defined(NO_SHA256) && defined(WOLFSSL_AES_128)
        #define BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
    #endif

    #if defined(WOLFSSL_SHA384) && defined(WOLFSSL_AES_256)
        #define BUILD_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
    #endif
#endif

#if !defined(NO_DH) && !defined(NO_PSK) && !defined(NO_TLS) && \
    !defined(NO_TLS_DH)
    /* SHOULD NOT negotiate cipher suites based on ephemeral
     * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
     * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
    #ifndef NO_SHA256
        #if defined(HAVE_AESGCM) && defined(WOLFSSL_AES_128)
            #define BUILD_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
        #endif
        #ifdef HAVE_AESCCM
            #ifdef WOLFSSL_AES_128
                #define BUILD_TLS_DHE_PSK_WITH_AES_128_CCM
            #endif
            #ifdef WOLFSSL_AES_256
                #define BUILD_TLS_DHE_PSK_WITH_AES_256_CCM
            #endif
        #endif
    #endif
    #if defined(WOLFSSL_SHA384) && defined(HAVE_AESGCM) && \
        defined(WOLFSSL_AES_256)
        #define BUILD_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
    #endif
#endif

#if (defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)) \
                                         && !defined(NO_TLS) && !defined(NO_AES)
    #ifdef HAVE_AESGCM
        #if !defined(NO_SHA256) && defined(WOLFSSL_AES_128)
            #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
            #endif
            #ifndef NO_RSA
                #define BUILD_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
            #endif
        #endif
        #if defined(WOLFSSL_SHA384) && defined(WOLFSSL_AES_256)
            #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
                #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
            #endif
            #ifndef NO_RSA
                #define BUILD_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
            #endif
        #endif
    #endif
    #if defined(HAVE_AESCCM) && !defined(NO_SHA256)
        #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
            #ifdef WOLFSSL_AES_128
                #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM
                #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
            #endif
            #ifdef WOLFSSL_AES_256
                #define BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8
            #endif
        #endif
    #endif
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && !defined(NO_SHA256)
    #if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
        #if defined(HAVE_ECC) || \
                        (defined(HAVE_CURVE25519) && defined(HAVE_ED25519)) || \
                        (defined(HAVE_CURVE448) && defined(HAVE_ED448))
            #define BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256
        #endif
        #ifndef NO_RSA
            #define BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
        #endif
    #endif
    #if !defined(NO_DH) && !defined(NO_RSA) && !defined(NO_TLS_DH)
        /* SHOULD NOT negotiate cipher suites based on ephemeral
         * finite-field Diffie-Hellman key agreement (i.e., "TLS_DHE_*"
         * suites). https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
        #define BUILD_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    #endif
#endif

    #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
        #ifdef WOLFSSL_SM4_CBC
            #define BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3
        #endif
        #ifdef WOLFSSL_SM4_GCM
            #define BUILD_TLS_ECDHE_ECDSA_WITH_SM4_GCM_SM3
        #endif
        #ifdef WOLFSSL_SM4_CCM
            #define BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CCM_SM3
        #endif
    #endif
#endif

#if defined(WOLFSSL_TLS13)
    #ifdef HAVE_AESGCM
        #if !defined(NO_SHA256) && defined(WOLFSSL_AES_128)
            #define BUILD_TLS_AES_128_GCM_SHA256
        #endif
        #if defined(WOLFSSL_SHA384) && defined(WOLFSSL_AES_256)
            #define BUILD_TLS_AES_256_GCM_SHA384
        #endif
    #endif

    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        #ifndef NO_SHA256
            #define BUILD_TLS_CHACHA20_POLY1305_SHA256
        #endif
    #endif

    #ifdef HAVE_AESCCM
        #if !defined(NO_SHA256) && defined(WOLFSSL_AES_128)
            #define BUILD_TLS_AES_128_CCM_SHA256
            #define BUILD_TLS_AES_128_CCM_8_SHA256
        #endif
    #endif
    #ifdef HAVE_NULL_CIPHER
        #ifndef NO_SHA256
            #define BUILD_TLS_SHA256_SHA256
        #endif
        #ifdef WOLFSSL_SHA384
            #define BUILD_TLS_SHA384_SHA384
        #endif
    #endif

    #ifdef WOLFSSL_SM3
        #ifdef WOLFSSL_SM4_GCM
            #define BUILD_TLS_SM4_GCM_SM3
        #endif

        #ifdef WOLFSSL_SM4_CCM
            #define BUILD_TLS_SM4_CCM_SM3
        #endif
    #endif
#endif

#if !defined(WOLFCRYPT_ONLY) && defined(NO_PSK) && \
    (defined(NO_DH) || !defined(HAVE_ANON)) && \
    defined(NO_RSA) && !defined(HAVE_ECC) && \
    !defined(HAVE_ED25519) && !defined(HAVE_ED448)
   #error "No cipher suites available with this build"
#endif

#ifdef WOLFSSL_MULTICAST
    #if defined(HAVE_NULL_CIPHER) && !defined(NO_SHA256)
        #define BUILD_WDM_WITH_NULL_SHA256
    #endif
#endif

#if defined(BUILD_SSL_RSA_WITH_RC4_128_SHA) || \
    defined(BUILD_SSL_RSA_WITH_RC4_128_MD5)
    #define BUILD_ARC4
#endif

#if defined(BUILD_SSL_RSA_WITH_3DES_EDE_CBC_SHA)
    #define BUILD_DES3
#endif

#if defined(BUILD_TLS_RSA_WITH_AES_128_CBC_SHA) || \
    defined(BUILD_TLS_RSA_WITH_AES_256_CBC_SHA) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256) || \
    defined(BUILD_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256)
    #undef  BUILD_AES
    #define BUILD_AES
#endif

#if defined(BUILD_TLS_RSA_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_PSK_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_RSA_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_PSK_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384) || \
    defined(BUILD_TLS_AES_128_GCM_SHA256) || \
    defined(BUILD_TLS_AES_256_GCM_SHA384)
    #define BUILD_AESGCM
#else
    /* No AES-GCM cipher suites available with build */
    #define NO_AESGCM_AEAD
#endif

#if defined(BUILD_TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384)
    #define BUILD_ARIA
#endif

#if defined(BUILD_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256) || \
    defined(BUILD_TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256) || \
    defined(BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256) || \
    defined(BUILD_TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_PSK_WITH_CHACHA20_POLY1305_SHA256) || \
    defined(BUILD_TLS_CHACHA20_POLY1305_SHA256)
    /* Have an available ChaCha Poly cipher suite */
#else
    /* No ChaCha Poly cipher suites available with build */
    #define NO_CHAPOL_AEAD
#endif

#ifdef NO_DES3
    #define DES_BLOCK_SIZE 8
#else
    #undef  BUILD_DES3
    #define BUILD_DES3
#endif

#if defined(NO_AES) || !defined(HAVE_AES_DECRYPT)
    #define AES_BLOCK_SIZE 16
    #undef  BUILD_AES
#else
    #undef  BUILD_AES
    #define BUILD_AES
#endif

#if !defined(NO_RC4) && !defined(WSSL_HARDEN_TLS)
    /* MUST NOT negotiate RC4 cipher suites
     * https://www.rfc-editor.org/rfc/rfc9325#section-4.1 */
    #undef  BUILD_ARC4
    #define BUILD_ARC4
#endif

#ifdef HAVE_CHACHA
    #define CHACHA20_BLOCK_SIZE 16
#endif

#if defined(WOLFSSL_MAX_STRENGTH) || \
    (defined(HAVE_AESGCM) && !defined(NO_AESGCM_AEAD)) || \
     defined(HAVE_AESCCM) || \
     defined(HAVE_ARIA) || \
    (defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && \
     !defined(NO_CHAPOL_AEAD)) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM) || \
    (defined(WOLFSSL_TLS13) && defined(HAVE_NULL_CIPHER))

    #define HAVE_AEAD
#endif

#if defined(WOLFSSL_MAX_STRENGTH) || \
    defined(HAVE_ECC) || !defined(NO_DH)

    #define HAVE_PFS
#endif

#ifdef WSSL_HARDEN_TLS
    #ifdef HAVE_NULL_CIPHER
        #error "NULL ciphers not allowed https://www.rfc-editor.org/rfc/rfc9325#section-4.1"
    #endif
    #ifdef WOLFSSL_STATIC_RSA
        #error "Static RSA ciphers not allowed https://www.rfc-editor.org/rfc/rfc9325#section-4.1"
    #endif
    #ifdef WOLFSSL_STATIC_DH
        #error "Static DH ciphers not allowed https://www.rfc-editor.org/rfc/rfc9325#section-4.1"
    #endif
    #ifdef HAVE_ANON
        #error "At least the server side has to be authenticated"
    #endif
#endif

#undef WSSL_HARDEN_TLS

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EXTRA) || defined(HAVE_LIGHTY)
#define SSL_CA_NAMES(ssl) ((ssl)->client_ca_names != NULL ? (ssl)->client_ca_names : \
        (ssl)->ctx->client_ca_names)
#else
#define WOLFSSL_NO_CA_NAMES
#endif

/* actual cipher values, 2nd byte */
enum {
    TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA = 0x16,
    TLS_DHE_RSA_WITH_AES_256_CBC_SHA  = 0x39,
    TLS_DHE_RSA_WITH_AES_128_CBC_SHA  = 0x33,
    TLS_DH_anon_WITH_AES_128_CBC_SHA  = 0x34,
    TLS_RSA_WITH_AES_256_CBC_SHA      = 0x35,
    TLS_RSA_WITH_AES_128_CBC_SHA      = 0x2F,
    TLS_RSA_WITH_NULL_MD5             = 0x01,
    TLS_RSA_WITH_NULL_SHA             = 0x02,
    TLS_PSK_WITH_AES_256_CBC_SHA      = 0x8d,
    TLS_PSK_WITH_AES_128_CBC_SHA256   = 0xae,
    TLS_PSK_WITH_AES_256_CBC_SHA384   = 0xaf,
    TLS_PSK_WITH_AES_128_CBC_SHA      = 0x8c,
    TLS_PSK_WITH_NULL_SHA256          = 0xb0,
    TLS_PSK_WITH_NULL_SHA384          = 0xb1,
    TLS_PSK_WITH_NULL_SHA             = 0x2c,
    SSL_RSA_WITH_RC4_128_SHA          = 0x05,
    SSL_RSA_WITH_RC4_128_MD5          = 0x04,
    SSL_RSA_WITH_3DES_EDE_CBC_SHA     = 0x0A,

    /* ECC suites, first byte is 0xC0 (ECC_BYTE) */
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA    = 0x14,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA    = 0x13,
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA  = 0x0A,
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA  = 0x09,
    TLS_ECDHE_RSA_WITH_RC4_128_SHA        = 0x11,
    TLS_ECDHE_ECDSA_WITH_RC4_128_SHA      = 0x07,
    TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA   = 0x12,
    TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA = 0x08,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256   = 0x27,
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = 0x23,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384   = 0x28,
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 = 0x24,
    TLS_ECDHE_ECDSA_WITH_NULL_SHA           = 0x06,
    TLS_ECDHE_PSK_WITH_NULL_SHA256          = 0x3a,
    TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256   = 0x37,

    /* static ECDH, first byte is 0xC0 (ECC_BYTE) */
    TLS_ECDH_RSA_WITH_AES_256_CBC_SHA    = 0x0F,
    TLS_ECDH_RSA_WITH_AES_128_CBC_SHA    = 0x0E,
    TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA  = 0x05,
    TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA  = 0x04,
    TLS_ECDH_RSA_WITH_RC4_128_SHA        = 0x0C,
    TLS_ECDH_ECDSA_WITH_RC4_128_SHA      = 0x02,
    TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA   = 0x0D,
    TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA = 0x03,
    TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256   = 0x29,
    TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 = 0x25,
    TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384   = 0x2A,
    TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 = 0x26,

    WDM_WITH_NULL_SHA256          = 0xFE, /* wolfSSL DTLS Multicast */

    /* SHA256 */
    TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 = 0x6b,
    TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 = 0x67,
    TLS_RSA_WITH_AES_256_CBC_SHA256     = 0x3d,
    TLS_RSA_WITH_AES_128_CBC_SHA256     = 0x3c,
    TLS_RSA_WITH_NULL_SHA256            = 0x3b,
    TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 = 0xb2,
    TLS_DHE_PSK_WITH_NULL_SHA256        = 0xb4,

    /* SHA384 */
    TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 = 0xb3,
    TLS_DHE_PSK_WITH_NULL_SHA384        = 0xb5,

    /* AES-GCM */
    TLS_RSA_WITH_AES_128_GCM_SHA256          = 0x9c,
    TLS_RSA_WITH_AES_256_GCM_SHA384          = 0x9d,
    TLS_DHE_RSA_WITH_AES_128_GCM_SHA256      = 0x9e,
    TLS_DHE_RSA_WITH_AES_256_GCM_SHA384      = 0x9f,
    TLS_DH_anon_WITH_AES_256_GCM_SHA384      = 0xa7,
    TLS_PSK_WITH_AES_128_GCM_SHA256          = 0xa8,
    TLS_PSK_WITH_AES_256_GCM_SHA384          = 0xa9,
    TLS_DHE_PSK_WITH_AES_128_GCM_SHA256      = 0xaa,
    TLS_DHE_PSK_WITH_AES_256_GCM_SHA384      = 0xab,

    /* ECC AES-GCM, first byte is 0xC0 (ECC_BYTE) */
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256  = 0x2b,
    TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384  = 0x2c,
    TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256   = 0x2d,
    TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384   = 0x2e,
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256    = 0x2f,
    TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384    = 0x30,
    TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256     = 0x31,
    TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384     = 0x32,

    /* AES-CCM, first byte is 0xC0 but isn't ECC,
     * also, in some of the other AES-CCM suites
     * there will be second byte number conflicts
     * with non-ECC AES-GCM */
    TLS_RSA_WITH_AES_128_CCM_8         = 0xa0,
    TLS_RSA_WITH_AES_256_CCM_8         = 0xa1,
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM   = 0xac,
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 = 0xae,
    TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 = 0xaf,
    TLS_PSK_WITH_AES_128_CCM           = 0xa4,
    TLS_PSK_WITH_AES_256_CCM           = 0xa5,
    TLS_PSK_WITH_AES_128_CCM_8         = 0xa8,
    TLS_PSK_WITH_AES_256_CCM_8         = 0xa9,
    TLS_DHE_PSK_WITH_AES_128_CCM       = 0xa6,
    TLS_DHE_PSK_WITH_AES_256_CCM       = 0xa7,

    /* Camellia */
    TLS_RSA_WITH_CAMELLIA_128_CBC_SHA        = 0x41,
    TLS_RSA_WITH_CAMELLIA_256_CBC_SHA        = 0x84,
    TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256     = 0xba,
    TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256     = 0xc0,
    TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA    = 0x45,
    TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA    = 0x88,
    TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 = 0xbe,
    TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 = 0xc4,

    /* chacha20-poly1305 suites first byte is 0xCC (CHACHA_BYTE) */
    TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256   = 0xa8,
    TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = 0xa9,
    TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256     = 0xaa,
    TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256   = 0xac,
    TLS_PSK_WITH_CHACHA20_POLY1305_SHA256         = 0xab,
    TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256     = 0xad,

    /* chacha20-poly1305 earlier version of nonce and padding (CHACHA_BYTE) */
    TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256   = 0x13,
    TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256 = 0x14,
    TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256     = 0x15,

    /* ECDHE_PSK RFC8442, first byte is 0xD0 (EDHE_PSK_BYTE) */
    TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256    = 0x01,

    /* TLS v1.3 cipher suites */
    TLS_AES_128_GCM_SHA256       = 0x01,
    TLS_AES_256_GCM_SHA384       = 0x02,
    TLS_CHACHA20_POLY1305_SHA256 = 0x03,
    TLS_AES_128_CCM_SHA256       = 0x04,
    TLS_AES_128_CCM_8_SHA256     = 0x05,

    /* TLS v1.3 Integrity only cipher suites - 0xC0 (ECC) first byte */
    TLS_SHA256_SHA256            = 0xB4,
    TLS_SHA384_SHA384            = 0xB5,

    /* ARIA-GCM, first byte is 0xC0 (ECC_BYTE)
    * See: https://www.rfc-editor.org/rfc/rfc6209.html#section-5
    */
    TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256   = 0x5c,
    TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384   = 0x5d,

    /* TLS v1.3 SM cipher suites - 0x00 (CIPHER_BYTE) is first byte */
    TLS_SM4_GCM_SM3              = 0xC6,
    TLS_SM4_CCM_SM3              = 0xC7,

    /* TLS v1.2 SM cipher suites - 0xE0 (SM_BYTE) is first byte */
    TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3 = 0x11,
    TLS_ECDHE_ECDSA_WITH_SM4_GCM_SM3 = 0x51,
    TLS_ECDHE_ECDSA_WITH_SM4_CCM_SM3 = 0x52,

    /* Fallback SCSV (Signaling Cipher Suite Value) */
    TLS_FALLBACK_SCSV                        = 0x56,
    /* Renegotiation Indication Extension Special Suite */
    TLS_EMPTY_RENEGOTIATION_INFO_SCSV        = 0xff
};


#ifndef WOLFSSL_SESSION_TIMEOUT
    #define WOLFSSL_SESSION_TIMEOUT 500
    /* default session resumption cache timeout in seconds */
#endif


#ifndef WOLFSSL_DTLS_WINDOW_WORDS
    #define WOLFSSL_DTLS_WINDOW_WORDS 2
#endif /* WOLFSSL_DTLS_WINDOW_WORDS */
#define DTLS_WORD_BITS (sizeof(word32) * CHAR_BIT)
#define DTLS_SEQ_BITS  (WOLFSSL_DTLS_WINDOW_WORDS * DTLS_WORD_BITS)
#define DTLS_SEQ_SZ    (sizeof(word32) * WOLFSSL_DTLS_WINDOW_WORDS)

#ifndef WOLFSSL_MULTICAST
    #define WOLFSSL_DTLS_PEERSEQ_SZ 1
#else
    #ifndef WOLFSSL_MULTICAST_PEERS
        /* max allowed multicast group peers */
        #define WOLFSSL_MULTICAST_PEERS 100
    #endif
    #define WOLFSSL_DTLS_PEERSEQ_SZ WOLFSSL_MULTICAST_PEERS
#endif /* WOLFSSL_MULTICAST */

#ifndef WOLFSSL_MAX_MTU
    /* 1500 - 100 bytes to account for UDP and IP headers */
    #define WOLFSSL_MAX_MTU 1400
#endif /* WOLFSSL_MAX_MTU */

#ifndef WOLFSSL_DTLS_MTU_ADDITIONAL_READ_BUFFER
    #define WOLFSSL_DTLS_MTU_ADDITIONAL_READ_BUFFER 500
#endif /* WOLFSSL_DTLS_MTU_ADDITIONAL_READ_BUFFER */

#ifndef WOLFSSL_DTLS_FRAG_POOL_SZ
    #define WOLFSSL_DTLS_FRAG_POOL_SZ 10
#endif

/* set minimum DH key size allowed */
#ifndef WOLFSSL_MIN_DHKEY_BITS
    #if defined(WOLFSSL_HARDEN_TLS) && !defined(WOLFSSL_HARDEN_TLS_NO_PKEY_CHECK)
        /* Using guidance from section 5.6.1
         * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-57pt1r5.pdf */
        #if WOLFSSL_HARDEN_TLS >= 128
            #define WOLFSSL_MIN_DHKEY_BITS 3072
        #elif WOLFSSL_HARDEN_TLS >= 112
            #define WOLFSSL_MIN_DHKEY_BITS 2048
        #endif
    #elif defined(WOLFSSL_MAX_STRENGTH)
        #define WOLFSSL_MIN_DHKEY_BITS 2048
    #else
        #define WOLFSSL_MIN_DHKEY_BITS 1024
    #endif
#endif
#if defined(WOLFSSL_HARDEN_TLS) && WOLFSSL_MIN_DHKEY_BITS < 2048 && \
    !defined(WOLFSSL_HARDEN_TLS_NO_PKEY_CHECK)
    /* Implementations MUST NOT negotiate cipher suites offering less than
     * 112 bits of security.
     * https://www.rfc-editor.org/rfc/rfc9325#section-4.1
     * Using guidance from section 5.6.1
     * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-57pt1r5.pdf */
    #error "For 112 bits of security DH needs at least 2048 bit keys"
#endif
#if (WOLFSSL_MIN_DHKEY_BITS % 8)
    #error DH minimum bit size must be multiple of 8
#endif
#if (WOLFSSL_MIN_DHKEY_BITS > 16000)
    #error DH minimum bit size must not be greater than 16000
#endif
#define MIN_DHKEY_SZ (WOLFSSL_MIN_DHKEY_BITS / 8)
/* set maximum DH key size allowed */
#ifndef WOLFSSL_MAX_DHKEY_BITS
    #if (defined(USE_FAST_MATH) && defined(FP_MAX_BITS) && FP_MAX_BITS >= 16384)
        #define WOLFSSL_MAX_DHKEY_BITS  (FP_MAX_BITS / 2)
    #elif (defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)) && \
           defined(SP_INT_BITS)
        /* SP implementation supports numbers of SP_INT_BITS bits. */
        #define WOLFSSL_MAX_DHKEY_BITS  (((SP_INT_BITS + 7) / 8) * 8)
    #else
        #define WOLFSSL_MAX_DHKEY_BITS  4096
    #endif
#endif
#if (WOLFSSL_MAX_DHKEY_BITS % 8)
    #error DH maximum bit size must be multiple of 8
#endif
#if (WOLFSSL_MAX_DHKEY_BITS > 16384)
    #error DH maximum bit size must not be greater than 16384
#endif
#define MAX_DHKEY_SZ (WOLFSSL_MAX_DHKEY_BITS / 8)

#ifndef NO_DH
#if WOLFSSL_MAX_DHKEY_BITS < WOLFSSL_MIN_DHKEY_BITS
#error "WOLFSSL_MAX_DHKEY_BITS has to be greater than WOLFSSL_MIN_DHKEY_BITS"
#endif
#endif /* NO_DH */

#ifndef MAX_PSK_ID_LEN
    /* max psk identity/hint supported */
    #if defined(WOLFSSL_TLS13)
        /* OpenSSL has a 1472 byte session ticket */
        #define MAX_PSK_ID_LEN 1536
    #else
        #define MAX_PSK_ID_LEN 128
    #endif
#endif

#ifndef MAX_EARLY_DATA_SZ
    /* maximum early data size */
    #define MAX_EARLY_DATA_SZ  4096
#endif

#ifndef NO_RSA
    #ifndef WOLFSSL_MAX_RSA_BITS
        #ifdef USE_FAST_MATH
            /* FP implementation support numbers up to FP_MAX_BITS / 2 bits. */
            #define WOLFSSL_MAX_RSA_BITS    (FP_MAX_BITS / 2)
        #elif defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)
            /* SP implementation supports numbers of SP_INT_BITS bits. */
            #define WOLFSSL_MAX_RSA_BITS    (((SP_INT_BITS + 7) / 8) * 8)
        #else
            /* Integer maths is dynamic but we only go up to 4096 bits. */
            #define WOLFSSL_MAX_RSA_BITS 4096
        #endif
    #endif
    #if (WOLFSSL_MAX_RSA_BITS % 8)
        #error RSA maximum bit size must be multiple of 8
    #endif
#endif


#if !defined(NO_RSA) || !defined(NO_DH) || defined(HAVE_ECC)
    /* MySQL wants to be able to use 8192-bit numbers. */
    #if defined(USE_FAST_MATH) && defined(FP_MAX_BITS)
        /* Use the FP size up to 8192-bit and down to a min of 1024-bit. */
        #if FP_MAX_BITS >= 16384
            #define ENCRYPT_BASE_BITS  8192
        #elif defined(HAVE_ECC)
            #if FP_MAX_BITS > 2224
                #define ENCRYPT_BASE_BITS  (FP_MAX_BITS / 2)
            #else
                /* 521-bit ASN.1 signature - 3 + 2 * (2 + 66) bytes. */
                #define ENCRYPT_BASE_BITS  1112
            #endif
        #else
            #if FP_MAX_BITS > 2048
                #define ENCRYPT_BASE_BITS  (FP_MAX_BITS / 2)
            #else
                #define ENCRYPT_BASE_BITS  1024
            #endif
        #endif

        /* Check MySQL size requirements met. */
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && ENCRYPT_BASE_BITS < 8192
            #error "MySQL needs FP_MAX_BITS at least at 16384"
        #endif

        #if !defined(NO_RSA) && defined(WOLFSSL_MAX_RSA_BITS) && \
            WOLFSSL_MAX_RSA_BITS > ENCRYPT_BASE_BITS
            #error "FP_MAX_BITS too small for WOLFSSL_MAX_RSA_BITS"
        #endif
    #elif defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_MATH)
        /* Use the SP size up to 8192-bit and down to a min of 1024-bit. */
        #if SP_INT_BITS >= 8192
            #define ENCRYPT_BASE_BITS  8192
        #elif defined(HAVE_ECC)
            #if SP_INT_BITS > 1112
                #define ENCRYPT_BASE_BITS  SP_INT_BITS
            #else
                /* 521-bit ASN.1 signature - 3 + 2 * (2 + 66) bytes. */
                #define ENCRYPT_BASE_BITS  1112
            #endif
        #else
            #if SP_INT_BITS > 1024
                #define ENCRYPT_BASE_BITS  SP_INT_BITS
            #else
                #define ENCRYPT_BASE_BITS  1024
            #endif
        #endif

        /* Check MySQL size requirements met. */
        #if defined(WOLFSSL_MYSQL_COMPATIBLE) && ENCRYPT_BASE_BITS < 8192
            #error "MySQL needs SP_INT_BITS at least at 8192"
        #endif

        #if !defined(NO_RSA) && defined(WOLFSSL_MAX_RSA_BITS) && \
            WOLFSSL_MAX_RSA_BITS > SP_INT_BITS
            #error "SP_INT_BITS too small for WOLFSSL_MAX_RSA_BITS"
        #endif
    #else
        /* Integer/heap maths - support 4096-bit. */
        #define ENCRYPT_BASE_BITS  4096
    #endif
#elif defined(HAVE_CURVE448)
    #define ENCRYPT_BASE_BITS    (456 * 2)
#elif defined(HAVE_CURVE25519)
    #define ENCRYPT_BASE_BITS    (256 * 2)
#else
    /* No secret from public key operation but PSK key plus length used. */
    #define ENCRYPT_BASE_BITS  ((MAX_PSK_ID_LEN + 2) * 8)
#endif

#ifdef WOLFSSL_DTLS_CID
#ifndef DTLS_CID_MAX_SIZE
/* DTLSv1.3 parsing code copies the record header in a static buffer to decrypt
 * the record. Increasing the CID max size does increase also this buffer,
 * impacting on per-session runtime memory footprint. */
#define DTLS_CID_MAX_SIZE 2
#endif
#else
#undef DTLS_CID_MAX_SIZE
#define DTLS_CID_MAX_SIZE 0
#endif /* WOLFSSL_DTLS_CID */

#if DTLS_CID_MAX_SIZE > 255
#error "Max size for DTLS CID is 255 bytes"
#endif

#ifndef MAX_TICKET_AGE_DIFF
/* maximum ticket age difference in seconds, 10 seconds */
#define MAX_TICKET_AGE_DIFF     10
#endif
#ifndef TLS13_MAX_TICKET_AGE
/* max ticket age in seconds, 7 days */
#define TLS13_MAX_TICKET_AGE    (7*24*60*60)
#endif


/* Limit is 2^24.5
 * https://www.rfc-editor.org/rfc/rfc8446#section-5.5
 * Without the fraction is 23726566 (0x016A09E6) */
#define AEAD_AES_LIMIT                           w64From32(0x016A, 0x09E6)
/* Limit is 2^23
 * https://www.rfc-editor.org/rfc/rfc9147.html#name-integrity-limits */
#define DTLS_AEAD_AES_CCM_LIMIT                  w64From32(0, 1 << 22)

/* Limit is 2^36
 * https://www.rfc-editor.org/rfc/rfc9147.html#name-aead-limits */
#define DTLS_AEAD_AES_GCM_CHACHA_FAIL_LIMIT      w64From32(1 << 3, 0)
#define DTLS_AEAD_AES_GCM_CHACHA_FAIL_KU_LIMIT   w64From32(1 << 2, 0)
/* Limit is 2^7
 * https://www.rfc-editor.org/rfc/rfc9147.html#name-limits-for-aead_aes_128_ccm */
#define DTLS_AEAD_AES_CCM_8_FAIL_LIMIT           w64From32(0, 1 << 6)
#define DTLS_AEAD_AES_CCM_8_FAIL_KU_LIMIT        w64From32(0, 1 << 5)
/* Limit is 2^23.5.
 * https://www.rfc-editor.org/rfc/rfc9147.html#name-integrity-limits
 * Without the fraction is 11863283 (0x00B504F3)
 * Half of this value is    5931641 (0x005A8279) */
#define DTLS_AEAD_AES_CCM_FAIL_LIMIT             w64From32(0x00B5, 0x04F3)
#define DTLS_AEAD_AES_CCM_FAIL_KU_LIMIT          w64From32(0x005A, 0x8279)

/* Limit is (2^22 - 1) full messages [2^36 - 31 octets]
 * https://www.rfc-editor.org/rfc/rfc8998.html#name-aead_sm4_gcm
 */
#define AEAD_SM4_GCM_LIMIT                       w64From32(0, (1 << 22) - 1)
/* Limit is (2^10 - 1) full messages [2^24 - 1 octets]
 * https://www.rfc-editor.org/rfc/rfc8998.html#name-aead_sm4_ccm
 */
#define AEAD_SM4_CCM_LIMIT                       w64From32(0, (1 << 10) - 1)

#if defined(WOLFSSL_TLS13) || !defined(NO_PSK)

#define TLS13_TICKET_NONCE_MAX_SZ 255

#if (defined(HAVE_FIPS) &&                                                     \
    !(defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3))) &&                    \
    defined(TLS13_TICKET_NONCE_STATIC_SZ)
#error "TLS13_TICKET_NONCE_STATIC_SZ is not supported in this FIPS version"
#endif

#ifndef TLS13_TICKET_NONCE_STATIC_SZ
#define TLS13_TICKET_NONCE_STATIC_SZ 8
#endif

#if TLS13_TICKET_NONCE_STATIC_SZ > TLS13_TICKET_NONCE_MAX_SZ
#error "Max size for ticket nonce is 255 bytes"
#endif

#endif /* WOLFSSL_TLS13 || !NO_PSK */

#ifdef WOLFSSL_TLS13
/* The length of the certificate verification label - client and server. */
#define CERT_VFY_LABEL_SZ    34
/* The number of prefix bytes for signature data. */
#define SIGNING_DATA_PREFIX_SZ     64
/* Maximum length of the signature data. */
#define MAX_SIG_DATA_SZ            (SIGNING_DATA_PREFIX_SZ + \
                                    CERT_VFY_LABEL_SZ      + \
                                    WC_MAX_DIGEST_SIZE)
#endif /* WOLFSSL_TLS13 */

enum Misc {
    CIPHER_BYTE    = 0x00,         /* Default ciphers */
    ECC_BYTE       = 0xC0,         /* ECC first cipher suite byte */
    CHACHA_BYTE    = 0xCC,         /* ChaCha first cipher suite */
    TLS13_BYTE     = 0x13,         /* TLS v1.3 first byte of cipher suite */
    ECDHE_PSK_BYTE = 0xD0,         /* RFC 8442 */
    SM_BYTE        = 0xE0,         /* SM first byte - private range */

    SEND_CERT       = 1,
    SEND_BLANK_CERT = 2,

    DTLS_MAJOR      = 0xfe,     /* DTLS major version number */
    DTLS_MINOR      = 0xff,     /* DTLS minor version number */
    DTLS_BOGUS_MINOR = 0xfe,    /* DTLS 0xfe was skipped, see RFC6347 Sec. 1 */
    DTLSv1_2_MINOR  = 0xfd,     /* DTLS minor version number */
    DTLSv1_3_MINOR  = 0xfc,     /* DTLS minor version number */
    SSLv3_MAJOR     = 3,        /* SSLv3 and TLSv1+  major version number */
    SSLv3_MINOR     = 0,        /* TLSv1   minor version number */
    TLSv1_MINOR     = 1,        /* TLSv1   minor version number */
    TLSv1_1_MINOR   = 2,        /* TLSv1_1 minor version number */
    TLSv1_2_MINOR   = 3,        /* TLSv1_2 minor version number */
    TLSv1_3_MINOR   = 4,        /* TLSv1_3 minor version number */
    TLS_DRAFT_MAJOR = 0x7f,     /* Draft TLS major version number */
    OLD_HELLO_ID    = 0x01,     /* SSLv2 Client Hello Indicator */
    INVALID_BYTE    = 0xff,     /* Used to initialize cipher specs values */
    NO_COMPRESSION  =  0,
    ZLIB_COMPRESSION = 221,     /* wolfSSL zlib compression */
    HELLO_EXT_SIG_ALGO = 13,    /* ID for the sig_algo hello extension */
    HELLO_EXT_EXTMS = 0x0017,   /* ID for the extended master secret ext */
    SECRET_LEN      = WOLFSSL_MAX_MASTER_KEY_LENGTH,
                                /* pre RSA and all master */
#if !defined(WOLFSSL_TLS13) || defined(WOLFSSL_32BIT_MILLI_TIME)
    TIMESTAMP_LEN   = 4,        /* timestamp size in ticket */
#else
    TIMESTAMP_LEN   = 8,        /* timestamp size in ticket */
#endif
#ifdef WOLFSSL_TLS13
    AGEADD_LEN      = 4,        /* ageAdd size in ticket */
    NAMEDGROUP_LEN  = 2,        /* namedGroup size in ticket */
#ifdef WOLFSSL_EARLY_DATA
    MAXEARLYDATASZ_LEN = 4,     /* maxEarlyDataSz size in ticket */
#endif
#endif
#ifdef HAVE_PQC
    ENCRYPT_LEN     = 4600,     /* allow 4600 byte buffer for dilithium. */
#else
#ifndef NO_PSK
    ENCRYPT_LEN     = (ENCRYPT_BASE_BITS / 8) + MAX_PSK_ID_LEN + 2,
#else
    ENCRYPT_LEN     = (ENCRYPT_BASE_BITS / 8),
#endif
#endif
    SIZEOF_SENDER   =  4,       /* clnt or srvr           */
    FINISHED_SZ     = 36,       /* WC_MD5_DIGEST_SIZE + WC_SHA_DIGEST_SIZE */
    MAX_RECORD_SIZE = 16384,    /* 2^14, max size by standard */
    MAX_PLAINTEXT_SZ   = (1 << 14),        /* Max plaintext sz   */
    MAX_TLS_CIPHER_SZ  = (1 << 14) + 2048, /* Max TLS encrypted data sz */
#ifdef WOLFSSL_TLS13
    MAX_TLS13_PLAIN_SZ = (1 << 14) + 1,    /* Max unencrypted data sz */
    MAX_TLS13_ENC_SZ   = (1 << 14) + 256,  /* Max encrypted data sz   */
#endif
    MAX_MSG_EXTRA   = 38 + WC_MAX_DIGEST_SIZE,
                                /* max added to msg, mac + pad  from */
                                /* RECORD_HEADER_SZ + BLOCK_SZ (pad) + Max
                                   digest sz + BLOC_SZ (iv) + pad byte (1) */
    MAX_COMP_EXTRA  = 1024,     /* max compression extra */
    MAX_MTU         = WOLFSSL_MAX_MTU,     /* max expected MTU */
    MAX_UDP_SIZE    = 8192 - 100, /* was MAX_MTU - 100 */
    MAX_DH_SZ       = (MAX_DHKEY_SZ * 3) + 12, /* DH_P, DH_G and DH_Pub */
                                /* 4096 p, pub, g + 2 byte size for each */
    MAX_STR_VERSION = 8,        /* string rep of protocol version */

    PAD_MD5        = 48,       /* pad length for finished */
    PAD_SHA        = 40,       /* pad length for finished */
    MAX_PAD_SIZE   = 256,      /* maximum length of padding */

    LENGTH_SZ      =  2,       /* length field for HMAC, data only */
    VERSION_SZ     =  2,       /* length of proctocol version */
    SEQ_SZ         =  8,       /* 64 bit sequence number  */
    ALERT_SIZE     =  2,       /* level + description     */
    VERIFY_HEADER  =  2,       /* always use 2 bytes      */
    EXTS_SZ        =  2,       /* always use 2 bytes      */
    EXT_ID_SZ      =  2,       /* always use 2 bytes      */
    MAX_DH_SIZE    = MAX_DHKEY_SZ+1,
                               /* Max size plus possible leading 0 */
    MIN_FFHDE_GROUP = 0x100,   /* Named group minimum for FFDHE parameters  */
    MAX_FFHDE_GROUP = 0x1FF,   /* Named group maximum for FFDHE parameters  */
    SESSION_HINT_SZ = 4,       /* session timeout hint */
    SESSION_ADD_SZ = 4,        /* session age add */
    TICKET_NONCE_LEN_SZ = 1,   /* Ticket nonce length size */
    DEF_TICKET_NONCE_SZ = 1,   /* Default ticket nonce size */
#if defined(WOLFSSL_TLS13) || !defined(NO_PSK)
    MAX_TICKET_NONCE_STATIC_SZ = TLS13_TICKET_NONCE_STATIC_SZ,
                               /* maximum ticket nonce static size */
#endif /* WOLFSSL_TLS13 || !NO_PSK */
    MAX_LIFETIME   = 604800,   /* maximum ticket lifetime */

    RAN_LEN      = 32,         /* random length           */
    SEED_LEN     = RAN_LEN * 2, /* tls prf seed length    */
    ID_LEN       = 32,         /* session id length       */
    COOKIE_SECRET_SZ = 14,     /* dtls cookie secret size */
    MAX_COOKIE_LEN = 32,       /* max dtls cookie size    */
    COOKIE_SZ    = 20,         /* use a 20 byte cookie    */
    SUITE_LEN    =  2,         /* cipher suite sz length  */
    ENUM_LEN     =  1,         /* always a byte           */
    OPAQUE8_LEN  =  1,         /* 1 byte                  */
    OPAQUE16_LEN =  2,         /* 2 bytes                 */
    OPAQUE24_LEN =  3,         /* 3 bytes                 */
    OPAQUE32_LEN =  4,         /* 4 bytes                 */
    OPAQUE64_LEN =  8,         /* 8 bytes                 */
    COMP_LEN     =  1,         /* compression length      */
    CURVE_LEN    =  2,         /* ecc named curve length  */
    KE_GROUP_LEN =  2,         /* key exchange group length */
#if defined(NO_SHA) && !defined(NO_SHA256)
    SERVER_ID_LEN = WC_SHA256_DIGEST_SIZE,
#else
    SERVER_ID_LEN = WC_SHA_DIGEST_SIZE,
#endif

    HANDSHAKE_HEADER_SZ   = 4,  /* type + length(3)        */
    RECORD_HEADER_SZ      = 5,  /* type + version + len(2) */
    CERT_HEADER_SZ        = 3,  /* always 3 bytes          */
    REQ_HEADER_SZ         = 2,  /* cert request header sz  */
    HINT_LEN_SZ           = 2,  /* length of hint size field */
    TRUNCATED_HMAC_SZ     = 10, /* length of hmac w/ truncated hmac extension */
    HELLO_EXT_SZ          = 4,  /* base length of a hello extension */
    HELLO_EXT_TYPE_SZ     = 2,  /* length of a hello extension type */
    HELLO_EXT_SZ_SZ       = 2,  /* length of a hello extension size */
    HELLO_EXT_SIGALGO_SZ  = 2,  /* length of number of items in sigalgo list */

    DTLS_HANDSHAKE_HEADER_SZ = 12, /* normal + seq(2) + offset(3) + length(3) */
    DTLS_RECORD_HEADER_SZ    = 13, /* normal + epoch(2) + seq_num(6) */
    DTLS_UNIFIED_HEADER_MIN_SZ = 2,
    /* flags + seq_number(2) + length(2) + CID */
    DTLS_RECVD_RL_HEADER_MAX_SZ = 5 + DTLS_CID_MAX_SIZE,
    DTLS_RECORD_HEADER_MAX_SZ = 13,
    DTLS_HANDSHAKE_EXTRA     = 8,  /* diff from normal */
    DTLS_RECORD_EXTRA        = 8,  /* diff from normal */
    DTLS_HANDSHAKE_SEQ_SZ    = 2,  /* handshake header sequence number */
    DTLS_HANDSHAKE_FRAG_SZ   = 3,  /* fragment offset and length are 24 bit */
    DTLS_POOL_SZ             = 20, /* allowed number of list items in TX and
                                    * RX pool */
    DTLS_FRAG_POOL_SZ        = WOLFSSL_DTLS_FRAG_POOL_SZ,
                                   /* allowed number of fragments per msg */
    DTLS_EXPORT_PRO          = 165,/* wolfSSL protocol for serialized session */
    DTLS_EXPORT_STATE_PRO    = 166,/* wolfSSL protocol for serialized state */
    TLS_EXPORT_PRO           = 167,/* wolfSSL protocol for serialized TLS */
    DTLS_EXPORT_OPT_SZ       = 61, /* amount of bytes used from Options */
    TLS_EXPORT_OPT_SZ        = 65, /* amount of bytes used from Options */
    DTLS_EXPORT_OPT_SZ_3     = 60, /* amount of bytes used from Options */
    DTLS_EXPORT_KEY_SZ       = 325 + (DTLS_SEQ_SZ * 2),
                                   /* max amount of bytes used from Keys */
    DTLS_EXPORT_MIN_KEY_SZ   = 85 + (DTLS_SEQ_SZ * 2),
                                   /* min amount of bytes used from Keys */
    WOLFSSL_EXPORT_TLS       = 1,
    WOLFSSL_EXPORT_DTLS      = 0,
#ifndef WOLFSSL_EXPORT_SPC_SZ
    WOLFSSL_EXPORT_SPC_SZ    = 16, /* amount of bytes used from CipherSpecs */
#endif
    WOLFSSL_EXPORT_LEN       = 2,  /* 2 bytes for length and protocol */
    WOLFSSL_EXPORT_VERSION   = 4,  /* wolfSSL version for serialized session */

    /* older export versions supported */
    WOLFSSL_EXPORT_VERSION_3 = 3,  /* wolfSSL version before TLS 1.3 addition */

    MAX_EXPORT_IP            = 46, /* max ip size IPv4 mapped IPv6 */
    DTLS_MTU_ADDITIONAL_READ_BUFFER = WOLFSSL_DTLS_MTU_ADDITIONAL_READ_BUFFER,
                                   /* Additional bytes to read so that
                                    * we can work with a peer that has
                                    * a slightly different MTU than us. */
    MAX_EXPORT_BUFFER        = 514, /* max size of buffer for exporting */
    MAX_EXPORT_STATE_BUFFER  = (DTLS_EXPORT_MIN_KEY_SZ) + (3 * WOLFSSL_EXPORT_LEN),
                                    /* max size of buffer for exporting state */
    FINISHED_LABEL_SZ   = 15,  /* TLS finished label size */
    TLS_FINISHED_SZ     = 12,  /* TLS has a shorter size  */
    TLS_FINISHED_SZ_MAX = WC_MAX_DIGEST_SIZE,
                            /* longest message digest size is SHA512, 64 */
    EXT_MASTER_LABEL_SZ = 22,  /* TLS extended master secret label sz */
    MASTER_LABEL_SZ     = 13,  /* TLS master secret label sz */
    KEY_LABEL_SZ        = 13,  /* TLS key block expansion sz */
    PROTOCOL_LABEL_SZ   = 9,   /* Length of the protocol label */
    MAX_LABEL_SZ        = 34,  /* Maximum length of a label */
    MAX_REQUEST_SZ      = 256, /* Maximum cert req len (no auth yet */
    SESSION_FLUSH_COUNT = 256, /* Flush session cache unless user turns off */
    TLS_MAX_PAD_SZ      = 255, /* Max padding in TLS */

#if defined(HAVE_NULL_CIPHER) && defined(WOLFSSL_TLS13)
    #if defined(WOLFSSL_SHA384) && WC_MAX_SYM_KEY_SIZE < 48
        MAX_SYM_KEY_SIZE    = WC_SHA384_DIGEST_SIZE,
    #elif !defined(NO_SHA256) && WC_MAX_SYM_KEY_SIZE < 32
        MAX_SYM_KEY_SIZE    = WC_SHA256_DIGEST_SIZE,
    #else
        MAX_SYM_KEY_SIZE    = WC_MAX_SYM_KEY_SIZE,
    #endif
#else
    MAX_SYM_KEY_SIZE    = WC_MAX_SYM_KEY_SIZE,
#endif

#if defined(HAVE_SELFTEST) && \
    (!defined(HAVE_SELFTEST_VERSION) || (HAVE_SELFTEST_VERSION < 2))
    #ifndef WOLFSSL_AES_KEY_SIZE_ENUM
    #define WOLFSSL_AES_KEY_SIZE_ENUM
    AES_IV_SIZE         = 16,
    AES_128_KEY_SIZE    = 16,
    AES_192_KEY_SIZE    = 24,
    AES_256_KEY_SIZE    = 32,
    #endif
#endif

    MAX_IV_SZ           = AES_BLOCK_SIZE,

    AEAD_SEQ_OFFSET     = 4,   /* Auth Data: Sequence number */
    AEAD_TYPE_OFFSET    = 8,   /* Auth Data: Type            */
    AEAD_VMAJ_OFFSET    = 9,   /* Auth Data: Major Version   */
    AEAD_VMIN_OFFSET    = 10,  /* Auth Data: Minor Version   */
    AEAD_LEN_OFFSET     = 11,  /* Auth Data: Length          */
    AEAD_AUTH_DATA_SZ   = 13,  /* Size of the data to authenticate */
    AEAD_NONCE_SZ       = 12,
    AESGCM_IMP_IV_SZ    = 4,   /* Size of GCM/CCM AEAD implicit IV */
    AESGCM_EXP_IV_SZ    = 8,   /* Size of GCM/CCM AEAD explicit IV */
    AESGCM_NONCE_SZ     = AESGCM_EXP_IV_SZ + AESGCM_IMP_IV_SZ,
    GCM_IMP_IV_SZ       = 4,   /* Size of GCM/CCM AEAD implicit IV */
    GCM_EXP_IV_SZ       = 8,   /* Size of GCM/CCM AEAD explicit IV */
    GCM_NONCE_SZ        = GCM_EXP_IV_SZ + GCM_IMP_IV_SZ,

    CHACHA20_IMP_IV_SZ  = 12,  /* Size of ChaCha20 AEAD implicit IV */
    CHACHA20_NONCE_SZ   = 12,  /* Size of ChacCha20 nonce           */
    CHACHA20_OLD_OFFSET = 4,   /* Offset for seq # in old poly1305  */

    /* For any new implicit/explicit IV size adjust AEAD_MAX_***_SZ */

    AES_GCM_AUTH_SZ     = 16, /* AES-GCM Auth Tag length    */
    AES_CCM_16_AUTH_SZ  = 16, /* AES-CCM-16 Auth Tag length */
    AES_CCM_8_AUTH_SZ   = 8,  /* AES-CCM-8 Auth Tag Length  */
    AESCCM_NONCE_SZ     = 12,

    SM4_GCM_AUTH_SZ     = 16, /* SM4-GCM Auth Tag length    */
    SM4_GCM_NONCE_SZ    = 12, /* SM4 GCM Nonce length       */
    SM4_CCM_AUTH_SZ     = 16, /* SM4-CCM Auth Tag length    */
    SM4_CCM_NONCE_SZ    = 12, /* SM4 CCM Nonce length       */

    CAMELLIA_128_KEY_SIZE = 16, /* for 128 bit */
    CAMELLIA_192_KEY_SIZE = 24, /* for 192 bit */
    CAMELLIA_256_KEY_SIZE = 32, /* for 256 bit */
    CAMELLIA_IV_SIZE      = 16, /* always block size */

    CHACHA20_256_KEY_SIZE = 32,  /* for 256 bit             */
    CHACHA20_128_KEY_SIZE = 16,  /* for 128 bit             */
    CHACHA20_IV_SIZE      = 12,  /* 96 bits for iv          */

    POLY1305_AUTH_SZ    = 16,  /* 128 bits                */

    HMAC_NONCE_SZ       = 12,  /* Size of HMAC nonce */

    EVP_SALT_SIZE       =  8,  /* evp salt size 64 bits   */

#ifndef ECDHE_SIZE /* allow this to be overridden at compile-time */
    ECDHE_SIZE          = 32,  /* ECDHE server size defaults to 256 bit */
#endif
    MAX_EXPORT_ECC_SZ   = 256, /* Export ANS X9.62 max future size */
    MAX_CURVE_NAME_SZ   = 16,  /* Maximum size of curve name string */

    NEW_SA_MAJOR        = 8,   /* Most significant byte used with new sig algos */
    ED25519_SA_MAJOR    = 8,   /* Most significant byte for ED25519 */
    ED25519_SA_MINOR    = 7,   /* Least significant byte for ED25519 */
    ED448_SA_MAJOR      = 8,   /* Most significant byte for ED448 */
    ED448_SA_MINOR      = 8,   /* Least significant byte for ED448 */
    SM2_SA_MAJOR        = 7,   /* Most significant byte for SM2 with SM3 */
    SM2_SA_MINOR        = 8,   /* Least significant byte for SM2 with SM3 */

    PQC_SA_MAJOR        = 0xFE,/* Most significant byte used with PQC sig algs */

    /* These values for falcon and dilithium match what OQS has defined in their OpenSSL fork. */
    FALCON_LEVEL1_SA_MAJOR = 0xFE,
    FALCON_LEVEL1_SA_MINOR = 0x0B,
    FALCON_LEVEL5_SA_MAJOR = 0xFE,
    FALCON_LEVEL5_SA_MINOR = 0x0E,

    DILITHIUM_LEVEL2_SA_MAJOR = 0xFE,
    DILITHIUM_LEVEL2_SA_MINOR = 0xA0,
    DILITHIUM_LEVEL3_SA_MAJOR = 0xFE,
    DILITHIUM_LEVEL3_SA_MINOR = 0xA3,
    DILITHIUM_LEVEL5_SA_MAJOR = 0xFE,
    DILITHIUM_LEVEL5_SA_MINOR = 0xA5,

    MIN_RSA_SHA512_PSS_BITS = 512 * 2 + 8 * 8, /* Min key size */
    MIN_RSA_SHA384_PSS_BITS = 384 * 2 + 8 * 8, /* Min key size */

#if defined(HAVE_PQC)
    MAX_CERT_VERIFY_SZ = 6000,            /* For Dilithium */
#elif !defined(NO_RSA) && defined(WOLFSSL_MAX_RSA_BITS)
    MAX_CERT_VERIFY_SZ = WOLFSSL_MAX_RSA_BITS / 8, /* max RSA bytes */
#elif defined(HAVE_ECC)
    MAX_CERT_VERIFY_SZ = ECC_MAX_SIG_SIZE, /* max ECC  */
#elif defined(HAVE_ED448)
    MAX_CERT_VERIFY_SZ = ED448_SIG_SIZE,   /* max Ed448  */
#elif defined(HAVE_ED25519)
    MAX_CERT_VERIFY_SZ = ED25519_SIG_SIZE, /* max Ed25519  */
#else
    MAX_CERT_VERIFY_SZ = 1024, /* max default  */
#endif
    CLIENT_HELLO_FIRST =  35,  /* Protocol + RAN_LEN + sizeof(id_len) */
    MAX_SUITE_NAME     =  48,  /* maximum length of cipher suite string */

    DTLS_TIMEOUT_INIT       =  1, /* default timeout init for DTLS receive  */
    DTLS_TIMEOUT_MAX        = 64, /* default max timeout for DTLS receive */
    DTLS_TIMEOUT_MULTIPLIER =  2, /* default timeout multiplier for DTLS recv */

    NULL_TERM_LEN        =   1,  /* length of null '\0' termination character */
    MAX_PSK_KEY_LEN      =  64,  /* max psk key supported */
    MIN_PSK_ID_LEN       =   6,  /* min length of identities */
    MIN_PSK_BINDERS_LEN  =  33,  /* min length of binders */

#ifndef MAX_WOLFSSL_FILE_SIZE
    MAX_WOLFSSL_FILE_SIZE = 1024UL * 1024UL * 4,  /* 4 mb file size alloc limit */
#endif

    CERT_MIN_SIZE      =  256, /* min PEM cert size with header/footer */

    NO_SNIFF           =   0,  /* not sniffing */
    SNIFF              =   1,  /* currently sniffing */

    HASH_SIG_SIZE      =   2,  /* default SHA1 RSA */

    NO_COPY            =   0,  /* should we copy static buffer for write */
    COPY               =   1,  /* should we copy static buffer for write */

    INVALID_PEER_ID    = 0xFFFF, /* Initialize value for peer ID. */

    PREV_ORDER         = -1,   /* Sequence number is in previous epoch. */
    PEER_ORDER         = 1,    /* Peer sequence number for verify. */
    CUR_ORDER          = 0,    /* Current sequence number. */
    WRITE_PROTO        = 1,    /* writing a protocol message */
    READ_PROTO         = 0     /* reading a protocol message */
};

#define WOLFSSL_NAMED_GROUP_IS_FFHDE(group) \
    (MIN_FFHDE_GROUP <= (group) && (group) <= MAX_FFHDE_GROUP)
#ifdef HAVE_PQC
#define WOLFSSL_NAMED_GROUP_IS_PQC(group) \
    (WOLFSSL_PQC_MIN <= (group) && (group) <= WOLFSSL_PQC_MAX)
#else
#define WOLFSSL_NAMED_GROUP_IS_PQC(group)    ((void)(group), 0)
#endif /* HAVE_PQC */

/* minimum Downgrade Minor version */
#ifndef WOLFSSL_MIN_DOWNGRADE
    #ifndef NO_OLD_TLS
        #define WOLFSSL_MIN_DOWNGRADE TLSv1_MINOR
    #else
        #define WOLFSSL_MIN_DOWNGRADE TLSv1_2_MINOR
    #endif
#endif

/* minimum DTLS Downgrade Minor version */
#ifndef WOLFSSL_MIN_DTLS_DOWNGRADE
#define WOLFSSL_MIN_DTLS_DOWNGRADE DTLS_MINOR;
#endif

/* Set max implicit IV size for AEAD cipher suites */
#define AEAD_MAX_IMP_SZ 12

/* Set max explicit IV size for AEAD cipher suites */
#define AEAD_MAX_EXP_SZ 8


#ifndef WOLFSSL_MAX_SUITE_SZ
    #define WOLFSSL_MAX_SUITE_SZ 300
    /* 150 suites for now! */
#endif

/* number of items in the signature algo list */
#ifndef WOLFSSL_MAX_SIGALGO
#ifdef HAVE_PQC
    /* If we are building with post-quantum algorithms, we likely want to
     * inter-op with OQS's OpenSSL and they send a lot more sigalgs.
     */
    #define WOLFSSL_MAX_SIGALGO 128
#else
    #define WOLFSSL_MAX_SIGALGO 38
#endif
#endif


/* set minimum ECC key size allowed */
#ifndef WOLFSSL_MIN_ECC_BITS
    #ifdef WOLFSSL_MAX_STRENGTH
        #define WOLFSSL_MIN_ECC_BITS  256
    #else
        #define WOLFSSL_MIN_ECC_BITS 224
    #endif
#endif /* WOLFSSL_MIN_ECC_BITS */
#if (WOLFSSL_MIN_ECC_BITS % 8)
    /* Some ECC keys are not divisible by 8 such as prime239v1 or sect131r1.
       In these cases round down to the nearest value divisible by 8. The
       restriction of being divisible by 8 is in place to match wc_ecc_size
       function from wolfSSL.
     */
    #error ECC minimum bit size must be a multiple of 8
#endif
#define MIN_ECCKEY_SZ (WOLFSSL_MIN_ECC_BITS / 8)

#ifdef HAVE_PQC
#ifndef MIN_FALCONKEY_SZ
    #define MIN_FALCONKEY_SZ    897
#endif
#ifndef MIN_DILITHIUMKEY_SZ
    #define MIN_DILITHIUMKEY_SZ    1312
#endif
#endif

/* set minimum RSA key size allowed */
#ifndef WOLFSSL_MIN_RSA_BITS
    #if defined(WOLFSSL_HARDEN_TLS) && !defined(WOLFSSL_HARDEN_TLS_NO_PKEY_CHECK)
        /* Using guidance from section 5.6.1
         * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-57pt1r5.pdf */
        #if WOLFSSL_HARDEN_TLS >= 128
            #define WOLFSSL_MIN_RSA_BITS 3072
        #elif WOLFSSL_HARDEN_TLS >= 112
            #define WOLFSSL_MIN_RSA_BITS 2048
        #endif
    #elif defined(WOLFSSL_MAX_STRENGTH)
        #define WOLFSSL_MIN_RSA_BITS 2048
    #else
        #define WOLFSSL_MIN_RSA_BITS 1024
    #endif
#endif /* WOLFSSL_MIN_RSA_BITS */
#if defined(WOLFSSL_HARDEN_TLS) && WOLFSSL_MIN_RSA_BITS < 2048 && \
    !defined(WOLFSSL_HARDEN_TLS_NO_PKEY_CHECK)
    /* Implementations MUST NOT negotiate cipher suites offering less than
     * 112 bits of security.
     * https://www.rfc-editor.org/rfc/rfc9325#section-4.1
     * Using guidance from section 5.6.1
     * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-57pt1r5.pdf */
    #error "For 112 bits of security RSA needs at least 2048 bit keys"
#endif
#if (WOLFSSL_MIN_RSA_BITS % 8)
    /* This is to account for the example case of a min size of 2050 bits but
       still allows 2049 bit key. So we need the measurement to be in bytes. */
    #error RSA minimum bit size must be a multiple of 8
#endif
#define MIN_RSAKEY_SZ (WOLFSSL_MIN_RSA_BITS / 8)

#ifdef SESSION_INDEX
/* Shift values for making a session index */
#define SESSIDX_ROW_SHIFT 4
#define SESSIDX_IDX_MASK  0x0F
#endif

#ifndef MAX_X509_SIZE
    #if defined(HAVE_PQC)
        #define MAX_X509_SIZE   (8*1024) /* max static x509 buffer size; dilithium is big */
    #elif defined(WOLFSSL_HAPROXY)
        #define MAX_X509_SIZE   3072 /* max static x509 buffer size */
    #else
        #define MAX_X509_SIZE   2048 /* max static x509 buffer size */
    #endif
#endif

/* max cert chain peer depth */
#ifndef MAX_CHAIN_DEPTH
    #define MAX_CHAIN_DEPTH 9
#endif

/* max size of a certificate message payload */
/* assumes MAX_CHAIN_DEPTH number of certificates at 2kb per certificate */
#ifndef MAX_CERTIFICATE_SZ
    #define MAX_CERTIFICATE_SZ \
                (CERT_HEADER_SZ + \
                (MAX_X509_SIZE + CERT_HEADER_SZ) * MAX_CHAIN_DEPTH)
#endif

/* max size of a handshake message, currently set to the certificate */
#ifndef MAX_HANDSHAKE_SZ
    #define MAX_HANDSHAKE_SZ MAX_CERTIFICATE_SZ
#endif

#ifndef PREALLOC_SESSION_TICKET_LEN
    #define PREALLOC_SESSION_TICKET_LEN 512
#endif

#ifndef PREALLOC_SESSION_TICKET_NONCE_LEN
    #define PREALLOC_SESSION_TICKET_NONCE_LEN 32
#endif

#ifndef SESSION_TICKET_HINT_DEFAULT
    #define SESSION_TICKET_HINT_DEFAULT 300
#endif

#if !defined(WOLFSSL_NO_DEF_TICKET_ENC_CB) && !defined(NO_WOLFSSL_SERVER)
    /* Check chosen encryption is available. */
    #if !(defined(HAVE_CHACHA) && defined(HAVE_POLY1305)) && \
        defined(WOLFSSL_TICKET_ENC_CHACHA20_POLY1305)
        #error "ChaCha20-Poly1305 not available for default ticket encryption"
    #endif
    #if !defined(HAVE_AESGCM) && (defined(WOLFSSL_TICKET_ENC_AES128_GCM) || \
        defined(WOLFSSL_TICKET_ENC_AES256_GCM))
        #error "AES-GCM not available for default ticket encryption"
    #endif

    #ifndef WOLFSSL_TICKET_KEY_LIFETIME
        /* Default lifetime is 1 hour from issue of first ticket with key. */
        #define WOLFSSL_TICKET_KEY_LIFETIME       (60 * 60)
    #endif
    #if WOLFSSL_TICKET_KEY_LIFETIME <= SESSION_TICKET_HINT_DEFAULT
        #error "Ticket Key lifetime must be longer than ticket life hint."
    #endif
#endif

#define MAX_ENCRYPT_SZ ENCRYPT_LEN

/* A static check to assert a relation between x and y */
#define WOLFSSL_ASSERT_TEST(x, y, op) do {         \
    typedef char _args_test_[(x) op (y) ? 1 : -1]; \
    (void)sizeof(_args_test_);                     \
} while(0)

#define WOLFSSL_ASSERT_EQ(x, y) WOLFSSL_ASSERT_TEST(x, y, ==)

#define WOLFSSL_ASSERT_SIZEOF_TEST(x, y, op) \
    WOLFSSL_ASSERT_TEST(sizeof((x)), sizeof((y)), op)

#define WOLFSSL_ASSERT_SIZEOF_GE(x, y) WOLFSSL_ASSERT_SIZEOF_TEST(x, y, >=)

/* states. Adding state before HANDSHAKE_DONE will break session importing */
enum states {
    NULL_STATE = 0,

    SERVER_HELLOVERIFYREQUEST_COMPLETE,
    SERVER_HELLO_RETRY_REQUEST_COMPLETE,
    SERVER_HELLO_COMPLETE,
    SERVER_ENCRYPTED_EXTENSIONS_COMPLETE,
    SERVER_CERT_COMPLETE,
    SERVER_CERT_VERIFY_COMPLETE,
    SERVER_KEYEXCHANGE_COMPLETE,
    SERVER_HELLODONE_COMPLETE,
    SERVER_CHANGECIPHERSPEC_COMPLETE,
    SERVER_FINISHED_COMPLETE,

    CLIENT_HELLO_RETRY,
    CLIENT_HELLO_COMPLETE,
    CLIENT_KEYEXCHANGE_COMPLETE,
    CLIENT_CHANGECIPHERSPEC_COMPLETE,
    CLIENT_FINISHED_COMPLETE,

    HANDSHAKE_DONE,

#ifdef WOLFSSL_DTLS13
    SERVER_FINISHED_ACKED,
#endif /* WOLFSSL_DTLS13 */

};

/* SSL Version */
typedef struct ProtocolVersion {
    byte major;
    byte minor;
} WOLFSSL_PACK ProtocolVersion;


WOLFSSL_LOCAL ProtocolVersion MakeSSLv3(void);
WOLFSSL_LOCAL ProtocolVersion MakeTLSv1(void);
WOLFSSL_LOCAL ProtocolVersion MakeTLSv1_1(void);
WOLFSSL_LOCAL ProtocolVersion MakeTLSv1_2(void);
WOLFSSL_LOCAL ProtocolVersion MakeTLSv1_3(void);

#ifdef WOLFSSL_DTLS
    WOLFSSL_LOCAL ProtocolVersion MakeDTLSv1(void);
    WOLFSSL_LOCAL ProtocolVersion MakeDTLSv1_2(void);

#ifdef WOLFSSL_DTLS13
    WOLFSSL_LOCAL ProtocolVersion MakeDTLSv1_3(void);
#endif /* WOLFSSL_DTLS13 */

#endif
#ifdef WOLFSSL_SESSION_EXPORT
WOLFSSL_LOCAL int wolfSSL_session_export_internal(WOLFSSL* ssl, byte* buf,
        word32* sz, int type);
WOLFSSL_LOCAL int wolfSSL_session_import_internal(WOLFSSL* ssl, const byte* buf,
        word32 sz, int type);
#ifdef WOLFSSL_DTLS
    WOLFSSL_LOCAL int wolfSSL_dtls_export_state_internal(WOLFSSL* ssl,
                                                          byte* buf, word32 sz);
    WOLFSSL_LOCAL int wolfSSL_dtls_import_state_internal(WOLFSSL* ssl,
                                                    const byte* buf, word32 sz);
    WOLFSSL_LOCAL int wolfSSL_send_session(WOLFSSL* ssl);
#endif
#endif

struct WOLFSSL_BY_DIR_HASH {
    unsigned long hash_value;
    int last_suffix;
};

struct WOLFSSL_BY_DIR_entry {
    char*   dir_name;
    int     dir_type;
    WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH) *hashes;
};

struct WOLFSSL_BY_DIR {
    WOLF_STACK_OF(WOLFSSL_BY_DIR_entry) *dir_entry;
    wolfSSL_Mutex    lock; /* dir list lock */
};

/* wolfSSL method type */
struct WOLFSSL_METHOD {
    ProtocolVersion version;
    byte            side;         /* connection side, server or client */
    byte            downgrade;    /* whether to downgrade version, default no */
};

/* wolfSSL buffer type - internal uses "buffer" type */
typedef WOLFSSL_BUFFER_INFO buffer;

typedef struct Suites Suites;

/* Declare opaque struct for API to use */
#ifndef WOLFSSL_CLIENT_SESSION_DEFINED
    typedef struct ClientSession ClientSession;
    #define WOLFSSL_CLIENT_SESSION_DEFINED
#endif

/* defaults to client */
WOLFSSL_LOCAL void InitSSL_Method(WOLFSSL_METHOD* method, ProtocolVersion pv);

WOLFSSL_LOCAL void InitSSL_CTX_Suites(WOLFSSL_CTX* ctx);
WOLFSSL_LOCAL int InitSSL_Suites(WOLFSSL* ssl);
WOLFSSL_LOCAL int InitSSL_Side(WOLFSSL* ssl, word16 side);

/* for sniffer */
WOLFSSL_LOCAL int DoFinished(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                            word32 size, word32 totalSz, int sniff);
#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL int DoTls13Finished(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                           word32 size, word32 totalSz, int sniff);
#endif
WOLFSSL_LOCAL int DoApplicationData(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                                    int sniff);
/* TLS v1.3 needs these */
WOLFSSL_LOCAL int  HandleTlsResumption(WOLFSSL* ssl, Suites* clSuites);
#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL byte SuiteMac(const byte* suite);
#endif
WOLFSSL_LOCAL int  DoClientHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                             word32 helloSz);
#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL int DoTls13ClientHello(WOLFSSL* ssl, const byte* input,
                                     word32* inOutIdx, word32 helloSz);
#endif
WOLFSSL_LOCAL int  DoServerHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                      word32 helloSz);
WOLFSSL_LOCAL int  CompleteServerHello(WOLFSSL *ssl);
WOLFSSL_LOCAL int  CheckVersion(WOLFSSL *ssl, ProtocolVersion pv);
WOLFSSL_LOCAL int  PickHashSigAlgo(WOLFSSL* ssl, const byte* hashSigAlgo,
                                   word32 hashSigAlgoSz);
#if defined(WOLF_PRIVATE_KEY_ID) && !defined(NO_CHECK_PRIVATE_KEY)
WOLFSSL_LOCAL int  CreateDevPrivateKey(void** pkey, byte* data, word32 length,
                                       int hsType, int label, int id,
                                       void* heap, int devId);
#endif
WOLFSSL_LOCAL int  DecodePrivateKey(WOLFSSL *ssl, word16* length);
#ifdef WOLF_PRIVATE_KEY_ID
WOLFSSL_LOCAL int GetPrivateKeySigSize(WOLFSSL* ssl);
#ifndef NO_ASN
    WOLFSSL_LOCAL int  InitSigPkCb(WOLFSSL* ssl, SignatureCtx* sigCtx);
#endif
#endif
WOLFSSL_LOCAL int CreateSigData(WOLFSSL* ssl, byte* sigData, word16* sigDataSz,
                                int check);
WOLFSSL_LOCAL int CreateRSAEncodedSig(byte* sig, byte* sigData, int sigDataSz,
                                      int sigAlgo, int hashAlgo);
#ifdef WOLFSSL_ASYNC_IO
WOLFSSL_LOCAL void FreeAsyncCtx(WOLFSSL* ssl, byte freeAsync);
#endif
WOLFSSL_LOCAL void FreeKeyExchange(WOLFSSL* ssl);
WOLFSSL_LOCAL void FreeSuites(WOLFSSL* ssl);
WOLFSSL_LOCAL int  ProcessPeerCerts(WOLFSSL* ssl, byte* input, word32* inOutIdx, word32 totalSz);
WOLFSSL_LOCAL int  MatchDomainName(const char* pattern, int len, const char* str);
#ifndef NO_CERTS
WOLFSSL_LOCAL int  CheckForAltNames(DecodedCert* dCert, const char* domain, int* checkCN);
WOLFSSL_LOCAL int  CheckIPAddr(DecodedCert* dCert, const char* ipasc);
WOLFSSL_LOCAL void CopyDecodedName(WOLFSSL_X509_NAME* name, DecodedCert* dCert, int nameType);
#endif
WOLFSSL_LOCAL int  SetupTicket(WOLFSSL* ssl);
WOLFSSL_LOCAL int  CreateTicket(WOLFSSL* ssl);
WOLFSSL_LOCAL int  HashRaw(WOLFSSL* ssl, const byte* output, int sz);
WOLFSSL_LOCAL int  HashOutput(WOLFSSL* ssl, const byte* output, int sz,
                              int ivSz);
WOLFSSL_LOCAL int  HashInput(WOLFSSL* ssl, const byte* input, int sz);

#ifdef HAVE_SNI
#ifndef NO_WOLFSSL_SERVER
WOLFSSL_LOCAL int SNI_Callback(WOLFSSL* ssl);
#endif
#endif

#ifdef HAVE_ALPN
WOLFSSL_LOCAL int ALPN_Select(WOLFSSL* ssl);
#endif

WOLFSSL_LOCAL int ChachaAEADEncrypt(WOLFSSL* ssl, byte* out, const byte* input,
                              word16 sz); /* needed by sniffer */

#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL int  DecryptTls13(WOLFSSL* ssl, byte* output, const byte* input,
                                word16 sz, const byte* aad, word16 aadSz);
WOLFSSL_LOCAL int  DoTls13HandShakeMsgType(WOLFSSL* ssl, byte* input,
                                           word32* inOutIdx, byte type,
                                           word32 size, word32 totalSz);
WOLFSSL_LOCAL int  DoTls13HandShakeMsg(WOLFSSL* ssl, byte* input,
                                       word32* inOutIdx, word32 totalSz);
WOLFSSL_LOCAL int DoTls13ServerHello(WOLFSSL* ssl, const byte* input,
                                     word32* inOutIdx, word32 helloSz,
                                     byte* extMsgType);
WOLFSSL_LOCAL int RestartHandshakeHash(WOLFSSL* ssl);

WOLFSSL_LOCAL int Tls13DeriveKey(WOLFSSL *ssl, byte *output, int outputLen,
    const byte *secret, const byte *label, word32 labelLen, int hashAlgo,
    int includeMsgs, int side);
#endif
int TimingPadVerify(WOLFSSL* ssl, const byte* input, int padLen, int macSz,
                    int pLen, int content);


enum {
    FORCED_FREE = 1,
    NO_FORCED_FREE = 0
};


/* only use compression extra if using compression */
#ifdef HAVE_LIBZ
    #define COMP_EXTRA MAX_COMP_EXTRA
#else
    #define COMP_EXTRA 0
#endif

/* only the sniffer needs space in the buffer for extra MTU record(s) */
#ifdef WOLFSSL_SNIFFER
    #define MTU_EXTRA MAX_MTU * 3
#else
    #define MTU_EXTRA 0
#endif


/* embedded callbacks require large static buffers, make sure on */
#ifdef WOLFSSL_CALLBACKS
    #undef  LARGE_STATIC_BUFFERS
    #define LARGE_STATIC_BUFFERS
#endif


/* determine maximum record size */
#ifdef RECORD_SIZE
    /* user supplied value */
    #if RECORD_SIZE < 128 || RECORD_SIZE > MAX_RECORD_SIZE
        #error Invalid record size
    #endif
#else
    /* give user option to use 16K static buffers */
    #if defined(LARGE_STATIC_BUFFERS)
        #define RECORD_SIZE     MAX_RECORD_SIZE
    #else
        #ifdef WOLFSSL_DTLS
            #define RECORD_SIZE MAX_MTU
        #else
            #define RECORD_SIZE 128
        #endif
    #endif
#endif


/* user option to turn off 16K output option */
/* if using small static buffers (default) and SSL_write tries to write data
   larger than the record we have, dynamically get it, unless user says only
   write in static buffer chunks  */
#ifndef STATIC_CHUNKS_ONLY
    #define OUTPUT_RECORD_SIZE MAX_RECORD_SIZE
#else
    #define OUTPUT_RECORD_SIZE RECORD_SIZE
#endif

/* wolfSSL input buffer

   RFC 2246:

   length
       The length (in bytes) of the following TLSPlaintext.fragment.
       The length should not exceed 2^14.
*/
#ifdef STATIC_BUFFER_LEN
    /* user supplied option */
    #if STATIC_BUFFER_LEN < 5 || STATIC_BUFFER_LEN > (RECORD_HEADER_SZ + \
                          RECORD_SIZE + COMP_EXTRA + MTU_EXTRA + MAX_MSG_EXTRA))
        #error Invalid static buffer length
    #endif
#elif defined(LARGE_STATIC_BUFFERS)
    #define STATIC_BUFFER_LEN (RECORD_HEADER_SZ + RECORD_SIZE + COMP_EXTRA + \
             MTU_EXTRA + MAX_MSG_EXTRA)
#else
    /* don't fragment memory from the record header */
    #define STATIC_BUFFER_LEN RECORD_HEADER_SZ
#endif

typedef struct {
    ALIGN16 byte staticBuffer[STATIC_BUFFER_LEN];
    byte*  buffer;       /* place holder for static or dynamic buffer */
    word32 length;       /* total buffer length used */
    word32 idx;          /* idx to part of length already consumed */
    word32 bufferSize;   /* current buffer size */
    byte   dynamicFlag;  /* dynamic memory currently in use */
    byte   offset;       /* alignment offset attempt */
} bufferStatic;

/* Cipher Suites holder */
struct Suites {
    word16 suiteSz;                 /* suite length in bytes        */
    word16 hashSigAlgoSz;           /* SigAlgo extension length in bytes */
    byte   suites[WOLFSSL_MAX_SUITE_SZ];
    byte   hashSigAlgo[WOLFSSL_MAX_SIGALGO]; /* sig/algo to offer */
    byte   setSuites;               /* user set suites from default */
};

typedef struct CipherSuite {
    byte   cipherSuite0;
    byte   cipherSuite;
    word32 ecdhCurveOID;
    struct KeyShareEntry* clientKSE;
#if defined(WOLFSSL_TLS13) && defined(HAVE_SUPPORTED_CURVES)
    int    doHelloRetry;
#endif
} CipherSuite;

WOLFSSL_LOCAL void InitSuitesHashSigAlgo(Suites* suites, int haveECDSAsig,
                                         int haveRSAsig, int haveFalconSig,
                                         int haveDilithiumSig, int haveAnon,
                                         int tls1_2, int keySz);
WOLFSSL_LOCAL void InitSuitesHashSigAlgo_ex(byte* hashSigAlgo, int haveECDSAsig,
                                            int haveRSAsig, int haveFalconSig,
                                            int haveDilithiumSig, int haveAnon,
                                            int tls1_2, int keySz, word16* len);
WOLFSSL_LOCAL void InitSuitesHashSigAlgo_ex2(byte* hashSigAlgo, int have,
                                             int tls1_2, int keySz,
                                             word16* len);
WOLFSSL_LOCAL int AllocateCtxSuites(WOLFSSL_CTX* ctx);
WOLFSSL_LOCAL int AllocateSuites(WOLFSSL* ssl);
WOLFSSL_LOCAL void InitSuites(Suites* suites, ProtocolVersion pv, int keySz,
                              word16 haveRSA, word16 havePSK, word16 haveDH,
                              word16 haveECDSAsig, word16 haveECC,
                              word16 haveStaticRSA, word16 haveStaticECC,
                              word16 haveFalconSig, word16 haveDilithiumSig,
                              word16 haveAnon, word16 haveNull, int side);

typedef struct TLSX TLSX;
WOLFSSL_LOCAL int MatchSuite_ex(const WOLFSSL* ssl, Suites* peerSuites,
                                CipherSuite* cs, TLSX* extensions);
WOLFSSL_LOCAL int  MatchSuite(WOLFSSL* ssl, Suites* peerSuites);
WOLFSSL_LOCAL int  SetCipherList(WOLFSSL_CTX* ctx, Suites* suites,
                                 const char* list);
WOLFSSL_LOCAL int  SetCipherListFromBytes(WOLFSSL_CTX* ctx, Suites* suites,
                                          const byte* list, const int listSz);
WOLFSSL_LOCAL int  SetSuitesHashSigAlgo(Suites* suites, const char* list);

#ifndef PSK_TYPES_DEFINED
    typedef unsigned int (*wc_psk_client_callback)(WOLFSSL*, const char*, char*,
                          unsigned int, unsigned char*, unsigned int);
    typedef unsigned int (*wc_psk_server_callback)(WOLFSSL*, const char*,
                          unsigned char*, unsigned int);
#ifdef WOLFSSL_TLS13
    typedef unsigned int (*wc_psk_client_cs_callback)(WOLFSSL*, const char*,
                          char*, unsigned int, unsigned char*, unsigned int,
                          const char* cipherName);
    typedef unsigned int (*wc_psk_client_tls13_callback)(WOLFSSL*, const char*,
                          char*, unsigned int, unsigned char*, unsigned int,
                          const char** cipherName);
    typedef unsigned int (*wc_psk_server_tls13_callback)(WOLFSSL*, const char*,
                          unsigned char*, unsigned int,
                          const char** cipherName);
#endif
#endif /* PSK_TYPES_DEFINED */
#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_SESSION_EXPORT) && \
   !defined(WOLFSSL_DTLS_EXPORT_TYPES)
    typedef int (*wc_dtls_export)(WOLFSSL* ssl,

#define WOLFSSL_DTLS_EXPORT_TYPES
#endif /* WOLFSSL_DTLS_EXPORT_TYPES */


#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT)
#define MAX_DESCRIPTION_SZ 255
#endif
struct WOLFSSL_CIPHER {
    byte cipherSuite0;
    byte cipherSuite;
    const WOLFSSL* ssl;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT)
    char description[MAX_DESCRIPTION_SZ];
    unsigned long offset;
    unsigned int in_stack; /* TRUE if added to stack in wolfSSL_get_ciphers_compat */
    int bits;
#endif
};


#ifdef NO_ASN
    /* no_asn won't have */
    typedef struct CertStatus CertStatus;
#endif

#ifndef HAVE_OCSP
    typedef struct WOLFSSL_OCSP WOLFSSL_OCSP;
#endif

/* wolfSSL OCSP controller */
#ifdef HAVE_OCSP
struct WOLFSSL_OCSP {
    WOLFSSL_CERT_MANAGER* cm;            /* pointer back to cert manager */
    OcspEntry*            ocspList;      /* OCSP response list */
    wolfSSL_Mutex         ocspLock;      /* OCSP list lock */
    int                   error;
#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    int(*statusCb)(WOLFSSL*, void*);
#endif
};
#endif

#ifndef MAX_DATE_SIZE
#define MAX_DATE_SIZE 32
#endif

typedef struct CRL_Entry CRL_Entry;

#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
    #define CRL_DIGEST_SIZE WC_SM3_DIGEST_SIZE
#elif defined(NO_SHA)
    #define CRL_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#else
    #define CRL_DIGEST_SIZE WC_SHA_DIGEST_SIZE
#endif

#ifdef NO_ASN
    typedef struct RevokedCert RevokedCert;
#endif
#ifdef CRL_STATIC_REVOKED_LIST
    #ifndef CRL_MAX_REVOKED_CERTS
        #define CRL_MAX_REVOKED_CERTS 4
    #elif CRL_MAX_REVOKED_CERTS > 22000
        #error CRL_MAX_REVOKED_CERTS too big, max is 22000
    #endif
#endif
/* Complete CRL */
struct CRL_Entry {
    CRL_Entry* next;                      /* next entry */
    byte    issuerHash[CRL_DIGEST_SIZE];  /* issuer hash                 */
    /* byte    crlHash[CRL_DIGEST_SIZE];      raw crl data hash           */
    /* restore the hash here if needed for optimized comparisons */
    byte    lastDate[MAX_DATE_SIZE]; /* last date updated  */
    byte    nextDate[MAX_DATE_SIZE]; /* next update date   */
    byte    lastDateFormat;          /* last date format */
    byte    nextDateFormat;          /* next date format */
#if defined(OPENSSL_EXTRA)
    WOLFSSL_ASN1_TIME lastDateAsn1;  /* last date updated  */
    WOLFSSL_ASN1_TIME nextDateAsn1;  /* next update date   */
#endif
#ifdef CRL_STATIC_REVOKED_LIST
    RevokedCert certs[CRL_MAX_REVOKED_CERTS];
#else
    RevokedCert* certs;             /* revoked cert list  */
#endif
    int     totalCerts;             /* number on list     */
    int     version;                /* version of certificate */
    int     verified;
    byte*   toBeSigned;
    word32  tbsSz;
    byte*   signature;
    word32  signatureSz;
    word32  signatureOID;
#if !defined(NO_SKID) && !defined(NO_ASN)
    byte    extAuthKeyIdSet;
    byte    extAuthKeyId[KEYID_SIZE];
#endif
    int                   crlNumber;  /* CRL number extension */
#if defined(OPENSSL_EXTRA)
    WOLFSSL_X509_NAME*    issuer;     /* X509_NAME type issuer */
#endif
};


typedef struct CRL_Monitor CRL_Monitor;

/* CRL directory monitor */
struct CRL_Monitor {
    char* path;      /* full dir path, if valid pointer we're using */
    int   type;      /* PEM or ASN1 type */
};


#if defined(HAVE_CRL) && defined(NO_FILESYSTEM)
    #undef HAVE_CRL_MONITOR
#endif

/* PEM and DER possible */
#define WOLFSSL_CRL_MONITORS_LEN (2)

#if defined(__MACH__) || defined(__FreeBSD__) || defined(__linux__)
typedef int    wolfSSL_CRL_mfd_t; /* monitor fd, -1 if no init yet */
/* mfd for bsd is kqueue fd, eventfd for linux */
#define WOLFSSL_CRL_MFD_INIT_VAL (-1)
#elif defined(_MSC_VER)
typedef HANDLE wolfSSL_CRL_mfd_t; /* monitor fd, INVALID_HANDLE_VALUE if
                                   * no init yet */
#define WOLFSSL_CRL_MFD_INIT_VAL (INVALID_HANDLE_VALUE)
#endif

/* wolfSSL CRL controller */
struct WOLFSSL_CRL {
    WOLFSSL_CERT_MANAGER* cm;            /* pointer back to cert manager */
    CRL_Entry*            currentEntry;  /* Current CRL entry being processed */
    CRL_Entry*            crlList;       /* our CRL list */
#ifdef HAVE_CRL_IO
    CbCrlIO               crlIOCb;
#endif
    wolfSSL_Mutex         crlLock;       /* CRL list lock */
    CRL_Monitor           monitors[WOLFSSL_CRL_MONITORS_LEN];
#ifdef HAVE_CRL_MONITOR
    COND_TYPE             cond;          /* condition to signal setup */
    THREAD_TYPE           tid;           /* monitoring thread */
    wolfSSL_CRL_mfd_t     mfd;
    int                   setup;         /* thread is setup predicate */
#endif
    void*                 heap;          /* heap hint for dynamic memory */
};


#ifdef NO_ASN
    typedef struct Signer Signer;
#ifdef WOLFSSL_TRUST_PEER_CERT
    typedef struct TrustedPeerCert TrustedPeerCert;
#endif
#endif


#ifndef CA_TABLE_SIZE
    #define CA_TABLE_SIZE 11
#endif
#ifdef WOLFSSL_TRUST_PEER_CERT
    #define TP_TABLE_SIZE 11
#endif

/* wolfSSL Certificate Manager */
struct WOLFSSL_CERT_MANAGER {
    Signer*         caTable[CA_TABLE_SIZE]; /* the CA signer table */
    void*           heap;                /* heap helper */
#ifdef WOLFSSL_TRUST_PEER_CERT
    TrustedPeerCert* tpTable[TP_TABLE_SIZE]; /* table of trusted peer certs */
    wolfSSL_Mutex   tpLock;                  /* trusted peer list lock */
#endif
    WOLFSSL_CRL*    crl;                 /* CRL checker */
    WOLFSSL_OCSP*   ocsp;                /* OCSP checker */
#if !defined(NO_WOLFSSL_SERVER) && (defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
                               ||  defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2))
    WOLFSSL_OCSP*   ocsp_stapling;       /* OCSP checker for OCSP stapling */
#endif
    char*           ocspOverrideURL;     /* use this responder */
    void*           ocspIOCtx;           /* I/O callback CTX */
#ifndef NO_WOLFSSL_CM_VERIFY
    VerifyCallback  verifyCallback;      /* Verify callback */
#endif
    CallbackCACache caCacheCallback;       /* CA cache addition callback */
    CbMissingCRL    cbMissingCRL;          /* notify thru cb of missing crl */
    CbOCSPIO        ocspIOCb;              /* I/O callback for OCSP lookup */
    CbOCSPRespFree  ocspRespFreeCb;        /* Frees OCSP Response from IO Cb */
    wolfSSL_Mutex   caLock;                /* CA list lock */
    byte            crlEnabled:1;          /* is CRL on ? */
    byte            crlCheckAll:1;         /* always leaf, but all ? */
    byte            ocspEnabled:1;         /* is OCSP on ? */
    byte            ocspCheckAll:1;        /* always leaf, but all ? */
    byte            ocspSendNonce:1;       /* send the OCSP nonce ? */
    byte            ocspUseOverrideURL:1;  /* ignore cert responder, override */
    byte            ocspStaplingEnabled:1; /* is OCSP Stapling on ? */
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
||  defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    byte            ocspMustStaple:1;      /* server must respond with staple */
#endif

#ifndef NO_RSA
    short           minRsaKeySz;         /* minimum allowed RSA key size */
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    short           minEccKeySz;         /* minimum allowed ECC key size */
#endif
#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
    WOLFSSL_X509_STORE  *x509_store_p;  /* a pointer back to CTX x509 store  */
                                        /* CTX has ownership and free this   */
                                        /* with CTX free.                    */
#endif
    wolfSSL_Ref     ref;
#ifdef HAVE_PQC
    short           minFalconKeySz;      /* minimum allowed Falcon key size */
    short           minDilithiumKeySz;   /* minimum allowed Dilithium key size */
#endif

};

WOLFSSL_LOCAL int CM_SaveCertCache(WOLFSSL_CERT_MANAGER* cm,
                                   const char* fname);
WOLFSSL_LOCAL int CM_RestoreCertCache(WOLFSSL_CERT_MANAGER* cm,
                                      const char* fname);
WOLFSSL_LOCAL int CM_MemSaveCertCache(WOLFSSL_CERT_MANAGER* cm, void* mem,
                                      int sz, int* used);
WOLFSSL_LOCAL int CM_MemRestoreCertCache(WOLFSSL_CERT_MANAGER* cm,
                                         const void* mem, int sz);
WOLFSSL_LOCAL int CM_GetCertCacheMemSize(WOLFSSL_CERT_MANAGER* cm);
WOLFSSL_LOCAL int CM_VerifyBuffer_ex(WOLFSSL_CERT_MANAGER* cm, const byte* buff,
                                     long sz, int format, int prev_err);


#ifndef NO_CERTS
#if !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH)
typedef struct ProcPeerCertArgs {
    buffer*      certs;
#ifdef WOLFSSL_TLS13
    buffer*      exts; /* extensions */
#endif
    DecodedCert* dCert;
    word32 idx;
    word32 begin;
    int    totalCerts; /* number of certs in certs buffer */
    int    count;
    int    certIdx;
    int    lastErr;
#ifdef WOLFSSL_TLS13
    byte   ctxSz;
#endif
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    char   untrustedDepth;
#endif
    word16 fatal:1;
    word16 verifyErr:1;
    word16 dCertInit:1;
#ifdef WOLFSSL_TRUST_PEER_CERT
    word16 haveTrustPeer:1; /* was cert verified by loaded trusted peer cert */
#endif
} ProcPeerCertArgs;
WOLFSSL_LOCAL int DoVerifyCallback(WOLFSSL_CERT_MANAGER* cm, WOLFSSL* ssl,
        int ret, ProcPeerCertArgs* args);
#endif /* !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH) */
#endif /* !defined NO_CERTS */

/* wolfSSL Sock Addr */
struct WOLFSSL_SOCKADDR {
    unsigned int sz; /* sockaddr size */
    unsigned int bufSz; /* size of allocated buffer */
    void*        sa; /* pointer to the sockaddr_in or sockaddr_in6 */
};

typedef struct WOLFSSL_DTLS_CTX {
    WOLFSSL_SOCKADDR peer;
    int rfd;
    int wfd;
    byte userSet:1;
    byte connected:1; /* When set indicates rfd and wfd sockets are
                       * connected (connect() and bind() both called).
                       * This means that sendto and recvfrom do not need to
                       * specify and store the peer address. */
} WOLFSSL_DTLS_CTX;


typedef struct WOLFSSL_DTLS_PEERSEQ {
    word32 window[WOLFSSL_DTLS_WINDOW_WORDS];
                        /* Sliding window for current epoch    */
    word16 nextEpoch;   /* Expected epoch in next record       */
    word16 nextSeq_hi;  /* Expected sequence in next record    */
    word32 nextSeq_lo;

    word32 prevWindow[WOLFSSL_DTLS_WINDOW_WORDS];
                        /* Sliding window for old epoch        */
    word32 prevSeq_lo;
    word16 prevSeq_hi;  /* Next sequence in allowed old epoch  */

#ifdef WOLFSSL_MULTICAST
    word16 peerId;
    word32 highwaterMark;
#endif
} WOLFSSL_DTLS_PEERSEQ;


#define MAX_WRITE_IV_SZ 16 /* max size of client/server write_IV */

/* keys and secrets
 * keep as a constant size (no additional ifdefs) for session export */
typedef struct Keys {
#if !defined(WOLFSSL_AEAD_ONLY) || defined(WOLFSSL_TLS13)
    byte client_write_MAC_secret[WC_MAX_DIGEST_SIZE];   /* max sizes */
    byte server_write_MAC_secret[WC_MAX_DIGEST_SIZE];
#endif
    byte client_write_key[MAX_SYM_KEY_SIZE];         /* max sizes */
    byte server_write_key[MAX_SYM_KEY_SIZE];
    byte client_write_IV[MAX_WRITE_IV_SZ];               /* max sizes */
    byte server_write_IV[MAX_WRITE_IV_SZ];
#if defined(HAVE_AEAD) || defined(WOLFSSL_SESSION_EXPORT)
    byte aead_exp_IV[AEAD_MAX_EXP_SZ];
    byte aead_enc_imp_IV[AEAD_MAX_IMP_SZ];
    byte aead_dec_imp_IV[AEAD_MAX_IMP_SZ];
#endif

#ifdef WOLFSSL_DTLS13
    byte client_sn_key[MAX_SYM_KEY_SIZE];
    byte server_sn_key[MAX_SYM_KEY_SIZE];
#endif /* WOLFSSL_DTLS13 */

    word32 peer_sequence_number_hi;
    word32 peer_sequence_number_lo;
    word32 sequence_number_hi;
    word32 sequence_number_lo;

#ifdef WOLFSSL_DTLS
    word16 curEpoch;    /* Received epoch in current record    */
    word16 curSeq_hi;   /* Received sequence in current record */
    word32 curSeq_lo;

#ifdef WOLFSSL_DTLS13
    w64wrapper curEpoch64;    /* Received epoch in current record    */
    w64wrapper curSeq;
#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_MULTICAST
    byte   curPeerId;   /* Received peer group ID in current record */
#endif
    WOLFSSL_DTLS_PEERSEQ peerSeq[WOLFSSL_DTLS_PEERSEQ_SZ];

    word16 dtls_peer_handshake_number;
    word16 dtls_expected_peer_handshake_number;

    word16 dtls_epoch;                          /* Current epoch    */
    word16 dtls_sequence_number_hi;             /* Current epoch */
    word32 dtls_sequence_number_lo;
    word16 dtls_prev_sequence_number_hi;        /* Previous epoch */
    word32 dtls_prev_sequence_number_lo;
    word16 dtls_handshake_number;               /* Current tx handshake seq */
#endif

    word32 encryptSz;             /* last size of encrypted data   */
    word32 padSz;                 /* how much to advance after decrypt part */
    byte   encryptionOn;          /* true after change cipher spec */
    byte   decryptedCur;          /* only decrypt current record once */
#ifdef WOLFSSL_TLS13
    byte   updateResponseReq:1;   /* KeyUpdate response from peer required. */
    byte   keyUpdateRespond:1;    /* KeyUpdate is to be responded to. */
#endif
#ifdef WOLFSSL_RENESAS_TSIP_TLS

    tsip_hmac_sha_key_index_t tsip_client_write_MAC_secret;
    tsip_hmac_sha_key_index_t tsip_server_write_MAC_secret;

#endif
#ifdef WOLFSSL_RENESAS_SCEPROTECT

    sce_hmac_sha_wrapped_key_t sce_client_write_MAC_secret;
    sce_hmac_sha_wrapped_key_t sce_server_write_MAC_secret;
#endif
} Keys;

/* Forward declare opaque pointer to make available for func def */
typedef struct Options Options;


/** TLS Extensions - RFC 6066 */
#ifdef HAVE_TLS_EXTENSIONS

typedef enum {
#ifdef HAVE_SNI
    TLSX_SERVER_NAME                = 0x0000, /* a.k.a. SNI  */
#endif
    TLSX_MAX_FRAGMENT_LENGTH        = 0x0001,
    TLSX_TRUSTED_CA_KEYS            = 0x0003,
    TLSX_TRUNCATED_HMAC             = 0x0004,
    TLSX_STATUS_REQUEST             = 0x0005, /* a.k.a. OCSP stapling   */
    TLSX_SUPPORTED_GROUPS           = 0x000a, /* a.k.a. Supported Curves */
    TLSX_EC_POINT_FORMATS           = 0x000b,
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
    TLSX_SIGNATURE_ALGORITHMS       = 0x000d, /* HELLO_EXT_SIG_ALGO */
#endif
#ifdef WOLFSSL_SRTP
    TLSX_USE_SRTP                   = 0x000e, /* 14 */
#endif
    TLSX_APPLICATION_LAYER_PROTOCOL = 0x0010, /* a.k.a. ALPN */
    TLSX_STATUS_REQUEST_V2          = 0x0011, /* a.k.a. OCSP stapling v2 */
#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
    TLSX_ENCRYPT_THEN_MAC           = 0x0016, /* RFC 7366 */
#endif
    TLSX_EXTENDED_MASTER_SECRET     = 0x0017, /* HELLO_EXT_EXTMS */
    TLSX_SESSION_TICKET             = 0x0023,
#ifdef WOLFSSL_TLS13
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    TLSX_PRE_SHARED_KEY             = 0x0029,
    #endif
    #ifdef WOLFSSL_EARLY_DATA
    TLSX_EARLY_DATA                 = 0x002a,
    #endif
    TLSX_SUPPORTED_VERSIONS         = 0x002b,
    #ifdef WOLFSSL_SEND_HRR_COOKIE
    TLSX_COOKIE                     = 0x002c,
    #endif
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    TLSX_PSK_KEY_EXCHANGE_MODES     = 0x002d,
    #endif
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
    TLSX_CERTIFICATE_AUTHORITIES    = 0x002f,
    #endif
    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    TLSX_POST_HANDSHAKE_AUTH        = 0x0031,
    #endif
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
    TLSX_SIGNATURE_ALGORITHMS_CERT  = 0x0032,
    #endif
    TLSX_KEY_SHARE                  = 0x0033,
    #if defined(WOLFSSL_DTLS_CID)
    TLSX_CONNECTION_ID              = 0x0036,
    #endif /* defined(WOLFSSL_DTLS_CID) */
    #ifdef WOLFSSL_QUIC
    TLSX_KEY_QUIC_TP_PARAMS         = 0x0039, /* RFC 9001, ch. 8.2 */
    #endif
#endif
    TLSX_RENEGOTIATION_INFO         = 0xff01,
#ifdef WOLFSSL_QUIC
    TLSX_KEY_QUIC_TP_PARAMS_DRAFT   = 0xffa5, /* from draft-ietf-quic-tls-27 */
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    TLSX_ECH                        = 0xfe0d, /* from draft-ietf-tls-esni-13 */
#endif
} TLSX_Type;

#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)

typedef enum {
    ECH_TYPE_OUTER = 0,
    ECH_TYPE_INNER = 1
} EchType;

typedef enum {
    ECH_WRITE_GREASE,
    ECH_WRITE_REAL,
    ECH_WRITE_RETRY_CONFIGS,
    ECH_WRITE_NONE,
    ECH_PARSED_INTERNAL,
} EchState;

typedef struct EchCipherSuite {
    word16 kdfId;
    word16 aeadId;
} EchCipherSuite;

typedef struct WOLFSSL_EchConfig {
    byte* raw;
    char* publicName;
    void* receiverPrivkey;
    struct WOLFSSL_EchConfig* next;
    EchCipherSuite* cipherSuites;
    word32 rawLen;
    word16 kemId;
    byte configId;
    byte numCipherSuites;
    byte receiverPubkey[HPKE_Npk_MAX];
} WOLFSSL_EchConfig;

typedef struct WOLFSSL_ECH {
    Hpke* hpke;
    const byte* aad;
    void* ephemeralKey;
    WOLFSSL_EchConfig* echConfig;
    byte* innerClientHello;
    byte* outerClientPayload;
    EchCipherSuite cipherSuite;
    word16 aadLen;
    word16 paddingLen;
    word16 innerClientHelloLen;
    word16 kemId;
    word16 encLen;
    EchState state;
    byte type;
    byte configId;
    byte enc[HPKE_Npk_MAX];
} WOLFSSL_ECH;

WOLFSSL_LOCAL int EchConfigGetSupportedCipherSuite(WOLFSSL_EchConfig* config);

WOLFSSL_LOCAL int TLSX_FinalizeEch(WOLFSSL_ECH* ech, byte* aad, word32 aadLen);

WOLFSSL_LOCAL int GetEchConfig(WOLFSSL_EchConfig* config, byte* output,
    word32* outputLen);

WOLFSSL_LOCAL int GetEchConfigsEx(WOLFSSL_EchConfig* configs,
    byte* output, word32* outputLen);
#endif

struct TLSX {
    TLSX_Type    type; /* Extension Type  */
    void*        data; /* Extension Data  */
    word32       val;  /* Extension Value */
    byte         resp; /* IsResponse Flag */
    struct TLSX* next; /* List Behavior   */
};

WOLFSSL_LOCAL TLSX* TLSX_Find(TLSX* list, TLSX_Type type);
WOLFSSL_LOCAL void  TLSX_Remove(TLSX** list, TLSX_Type type, void* heap);
WOLFSSL_LOCAL void  TLSX_FreeAll(TLSX* list, void* heap);
WOLFSSL_LOCAL int   TLSX_SupportExtensions(WOLFSSL* ssl);
WOLFSSL_LOCAL int   TLSX_PopulateExtensions(WOLFSSL* ssl, byte isRequest);

#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_CLIENT)
WOLFSSL_LOCAL int   TLSX_GetRequestSize(WOLFSSL* ssl, byte msgType,
                                         word16* pLength);
WOLFSSL_LOCAL int   TLSX_WriteRequest(WOLFSSL* ssl, byte* output,
                                       byte msgType, word16* pOffset);
#endif

#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_SERVER)
/* TLS 1.3 Certificate messages have extensions. */
WOLFSSL_LOCAL int   TLSX_GetResponseSize(WOLFSSL* ssl, byte msgType,
                                          word16* pLength);
WOLFSSL_LOCAL int   TLSX_WriteResponse(WOLFSSL *ssl, byte* output, byte msgType,
                                        word16* pOffset);
#endif

WOLFSSL_LOCAL int   TLSX_ParseVersion(WOLFSSL* ssl, const byte* input,
                                      word16 length, byte msgType, int* found);
WOLFSSL_LOCAL int TLSX_SupportedVersions_Parse(const WOLFSSL* ssl,
        const byte* input, word16 length, byte msgType, ProtocolVersion* pv,
        Options* opts, TLSX** exts);
WOLFSSL_LOCAL int   TLSX_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                               byte msgType, Suites *suites);
WOLFSSL_LOCAL int TLSX_Push(TLSX** list, TLSX_Type type,
                            const void* data, void* heap);
WOLFSSL_LOCAL int TLSX_Append(TLSX** list, TLSX_Type type,
                            const void* data, void* heap);

#elif defined(HAVE_SNI)                           \
   || defined(HAVE_MAX_FRAGMENT)                  \
   || defined(HAVE_TRUSTED_CA)                    \
   || defined(HAVE_TRUNCATED_HMAC)                \
   || defined(HAVE_CERTIFICATE_STATUS_REQUEST)    \
   || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2) \
   || defined(HAVE_SUPPORTED_CURVES)              \
   || defined(HAVE_ALPN)                          \
   || defined(HAVE_SESSION_TICKET)                \
   || defined(HAVE_SECURE_RENEGOTIATION)          \
   || defined(HAVE_SERVER_RENEGOTIATION_INFO)

#error Using TLS extensions requires HAVE_TLS_EXTENSIONS to be defined.

#endif /* HAVE_TLS_EXTENSIONS */

/** Server Name Indication - RFC 6066 (session 3) */
#ifdef HAVE_SNI

typedef struct SNI {
    byte                       type;    /* SNI Type         */
    union { char* host_name; } data;    /* SNI Data         */
    struct SNI*                next;    /* List Behavior    */
    byte                       status;  /* Matching result  */
#ifndef NO_WOLFSSL_SERVER
    byte                       options; /* Behavior options */
#endif
} SNI;

WOLFSSL_LOCAL int TLSX_UseSNI(TLSX** extensions, byte type, const void* data,
                                                       word16 size, void* heap);
WOLFSSL_LOCAL byte TLSX_SNI_Status(TLSX* extensions, byte type);
WOLFSSL_LOCAL word16 TLSX_SNI_GetRequest(TLSX* extensions, byte type,
                                                                   void** data);

#ifndef NO_WOLFSSL_SERVER
WOLFSSL_LOCAL void   TLSX_SNI_SetOptions(TLSX* extensions, byte type,
                                                                  byte options);
WOLFSSL_LOCAL int    TLSX_SNI_GetFromBuffer(const byte* clientHello,
                         word32 helloSz, byte type, byte* sni, word32* inOutSz);
#endif

#endif /* HAVE_SNI */

/* Trusted CA Key Indication - RFC 6066 (section 6) */
#ifdef HAVE_TRUSTED_CA

typedef struct TCA {
    byte                       type;    /* TCA Type            */
    byte*                      id;      /* TCA identifier      */
    word16                     idSz;    /* TCA identifier size */
    struct TCA*                next;    /* List Behavior       */
} TCA;

WOLFSSL_LOCAL int TLSX_UseTrustedCA(TLSX** extensions, byte type,
                    const byte* id, word16 idSz, void* heap);

#endif /* HAVE_TRUSTED_CA */

/* Application-Layer Protocol Negotiation - RFC 7301 */
#ifdef HAVE_ALPN
typedef struct ALPN {
    char*        protocol_name; /* ALPN protocol name */
    struct ALPN* next;          /* List Behavior      */
    byte         options;       /* Behavior options */
    byte         negotiated;    /* ALPN protocol negotiated or not */
} ALPN;

WOLFSSL_LOCAL int TLSX_ALPN_GetRequest(TLSX* extensions,
                                       void** data, word16 *dataSz);

WOLFSSL_LOCAL int TLSX_UseALPN(TLSX** extensions, const void* data,
                               word16 size, byte options, void* heap);

WOLFSSL_LOCAL int TLSX_ALPN_SetOptions(TLSX** extensions, byte option);

#endif /* HAVE_ALPN */

/** Maximum Fragment Length Negotiation - RFC 6066 (session 4) */
#ifdef HAVE_MAX_FRAGMENT

WOLFSSL_LOCAL int TLSX_UseMaxFragment(TLSX** extensions, byte mfl, void* heap);

#endif /* HAVE_MAX_FRAGMENT */

/** Truncated HMAC - RFC 6066 (session 7) */
#ifdef HAVE_TRUNCATED_HMAC

WOLFSSL_LOCAL int TLSX_UseTruncatedHMAC(TLSX** extensions, void* heap);

#endif /* HAVE_TRUNCATED_HMAC */

/** Certificate Status Request - RFC 6066 (session 8) */
#ifdef HAVE_CERTIFICATE_STATUS_REQUEST

typedef struct {
    byte status_type;
    byte options;
    WOLFSSL* ssl;
    union {
        OcspRequest ocsp;
    } request;
#ifdef WOLFSSL_TLS13
    buffer response;
#endif
} CertificateStatusRequest;

WOLFSSL_LOCAL int   TLSX_UseCertificateStatusRequest(TLSX** extensions,
           byte status_type, byte options, WOLFSSL* ssl, void* heap, int devId);
#ifndef NO_CERTS
WOLFSSL_LOCAL int   TLSX_CSR_InitRequest(TLSX* extensions, DecodedCert* cert,
                                                                    void* heap);
#endif
WOLFSSL_LOCAL void* TLSX_CSR_GetRequest(TLSX* extensions);
WOLFSSL_LOCAL int   TLSX_CSR_ForceRequest(WOLFSSL* ssl);

#endif

/** Certificate Status Request v2 - RFC 6961 */
#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2

typedef struct CSRIv2 {
    byte status_type;
    byte options;
    word16 requests;
    union {
        OcspRequest ocsp[1 + MAX_CHAIN_DEPTH];
    } request;
    struct CSRIv2* next;
} CertificateStatusRequestItemV2;

WOLFSSL_LOCAL int   TLSX_UseCertificateStatusRequestV2(TLSX** extensions,
                         byte status_type, byte options, void* heap, int devId);
#ifndef NO_CERTS
WOLFSSL_LOCAL int   TLSX_CSR2_InitRequests(TLSX* extensions, DecodedCert* cert,
                                                       byte isPeer, void* heap);
#endif
WOLFSSL_LOCAL void* TLSX_CSR2_GetRequest(TLSX* extensions, byte status_type,
                                                                    byte idx);
WOLFSSL_LOCAL int   TLSX_CSR2_ForceRequest(WOLFSSL* ssl);

#endif

#if defined(WOLFSSL_PUBLIC_ASN) && defined(HAVE_PK_CALLBACKS)
/* Internal callback guarded by WOLFSSL_PUBLIC_ASN because of DecodedCert. */
typedef int (*CallbackProcessPeerCert)(WOLFSSL* ssl, DecodedCert* p_cert);
WOLFSSL_API void wolfSSL_CTX_SetProcessPeerCertCb(WOLFSSL_CTX* ctx,
       CallbackProcessPeerCert cb);
#endif /* DecodedCert && HAVE_PK_CALLBACKS */

#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
typedef struct SignatureAlgorithms {
    /* Not const since it is modified in TLSX_SignatureAlgorithms_MapPss */
    WOLFSSL*    ssl;
    word16      hashSigAlgoSz; /* SigAlgo extension length in bytes */
    /* Ignore "nonstandard extension used : zero-sized array in struct/union"
     * MSVC warning */
    #ifdef _MSC_VER
    #pragma warning(disable: 4200)
    #endif
    byte        hashSigAlgo[]; /* sig/algo to offer */
} SignatureAlgorithms;

WOLFSSL_LOCAL SignatureAlgorithms* TLSX_SignatureAlgorithms_New(
        WOLFSSL* ssl, word16 hashSigAlgoSz, void* heap);
WOLFSSL_LOCAL void TLSX_SignatureAlgorithms_FreeAll(SignatureAlgorithms* sa,
                                                    void* heap);
#endif

/** Supported Elliptic Curves - RFC 4492 (session 4) */
#ifdef HAVE_SUPPORTED_CURVES

typedef struct SupportedCurve {
    word16 name;                 /* Curve Names */
    struct SupportedCurve* next; /* List Behavior */
} SupportedCurve;

typedef struct PointFormat {
    byte format;                /* PointFormat */
    struct PointFormat* next;   /* List Behavior */
} PointFormat;

WOLFSSL_LOCAL int TLSX_SupportedCurve_Copy(TLSX* src, TLSX** dst, void* heap);
WOLFSSL_LOCAL int TLSX_UseSupportedCurve(TLSX** extensions, word16 name,
                                                                    void* heap);

WOLFSSL_LOCAL int TLSX_UsePointFormat(TLSX** extensions, byte point,
                                                                    void* heap);

#ifndef NO_WOLFSSL_SERVER
WOLFSSL_LOCAL int TLSX_ValidateSupportedCurves(const WOLFSSL* ssl, byte first,
                                               byte second, word32* ecdhCurveOID);
WOLFSSL_LOCAL int TLSX_SupportedCurve_CheckPriority(WOLFSSL* ssl);
WOLFSSL_LOCAL int TLSX_SupportedFFDHE_Set(WOLFSSL* ssl);
#endif
WOLFSSL_LOCAL int TLSX_SupportedCurve_Preferred(WOLFSSL* ssl,
                                                            int checkSupported);
WOLFSSL_LOCAL int TLSX_SupportedCurve_Parse(const WOLFSSL* ssl,
        const byte* input, word16 length, byte isRequest, TLSX** extensions);

#endif /* HAVE_SUPPORTED_CURVES */

/** Renegotiation Indication - RFC 5746 */
#if defined(HAVE_SECURE_RENEGOTIATION) \
 || defined(HAVE_SERVER_RENEGOTIATION_INFO)

enum key_cache_state {
    SCR_CACHE_NULL   = 0,       /* empty / begin state */
    SCR_CACHE_NEEDED,           /* need to cache keys */
    SCR_CACHE_COPY,             /* we have a cached copy */
    SCR_CACHE_PARTIAL,          /* partial restore to real keys */
    SCR_CACHE_COMPLETE          /* complete restore to real keys */
};

/* Additional Connection State according to rfc5746 section 3.1 */
typedef struct SecureRenegotiation {
   byte                 enabled;  /* secure_renegotiation flag in rfc */
   byte                 verifySet;
   byte                 startScr; /* server requested client to start scr */
   enum key_cache_state cache_status;  /* track key cache state */
   byte                 client_verify_data[TLS_FINISHED_SZ];  /* cached */
   byte                 server_verify_data[TLS_FINISHED_SZ];  /* cached */
   byte                 subject_hash_set; /* if peer cert hash is set */
   byte                 subject_hash[KEYID_SIZE];  /* peer cert hash */
   Keys                 tmp_keys;  /* can't overwrite real keys yet */
} SecureRenegotiation;

WOLFSSL_LOCAL int TLSX_UseSecureRenegotiation(TLSX** extensions, void* heap);

#ifdef HAVE_SERVER_RENEGOTIATION_INFO
WOLFSSL_LOCAL int TLSX_AddEmptyRenegotiationInfo(TLSX** extensions, void* heap);
#endif

#endif /* HAVE_SECURE_RENEGOTIATION */

#ifdef HAVE_SESSION_TICKET
/* Our ticket format. All members need to be a byte or array of byte to
 * avoid alignment issues */
typedef struct InternalTicket {
    ProtocolVersion pv;                    /* version when ticket created */
    byte            suite[SUITE_LEN];      /* cipher suite when created */
    byte            msecret[SECRET_LEN];   /* master secret */
    byte            timestamp[TIMESTAMP_LEN];          /* born on */
    byte            haveEMS;               /* have extended master secret */
#ifdef WOLFSSL_TLS13
    byte            ageAdd[AGEADD_LEN];    /* Obfuscation of age */
    byte            namedGroup[NAMEDGROUP_LEN]; /* Named group used */
    byte            ticketNonceLen;
    byte            ticketNonce[MAX_TICKET_NONCE_STATIC_SZ];
#ifdef WOLFSSL_EARLY_DATA
    byte            maxEarlyDataSz[MAXEARLYDATASZ_LEN]; /* Max size of
                                                         * early data */
#endif
#endif
#ifdef WOLFSSL_TICKET_HAVE_ID
    byte            id[ID_LEN];
#endif
#ifdef OPENSSL_EXTRA
    byte            sessionCtxSz;          /* sessionCtx length        */
    byte            sessionCtx[ID_LEN];    /* app specific context id */
#endif /* OPENSSL_EXTRA */
} InternalTicket;

#ifndef WOLFSSL_TICKET_EXTRA_PADDING_SZ
#define WOLFSSL_TICKET_EXTRA_PADDING_SZ 32
#endif

#define WOLFSSL_TICKET_ENC_SZ \
    (sizeof(InternalTicket) + WOLFSSL_TICKET_EXTRA_PADDING_SZ)

/* RFC 5077 defines this for session tickets. All members need to be a byte or
 * array of byte to avoid alignment issues */
typedef struct ExternalTicket {
    byte key_name[WOLFSSL_TICKET_NAME_SZ];  /* key context name - 16 */
    byte iv[WOLFSSL_TICKET_IV_SZ];          /* this ticket's iv - 16 */
    byte enc_len[OPAQUE16_LEN];             /* encrypted length - 2 */
    byte enc_ticket[WOLFSSL_TICKET_ENC_SZ];
                                            /* encrypted internal ticket */
    byte mac[WOLFSSL_TICKET_MAC_SZ];        /* total mac - 32 */
} ExternalTicket;

/* Cast to int to reduce amount of casts in code */
#define SESSION_TICKET_LEN ((int)sizeof(ExternalTicket))
#define WOLFSSL_TICKET_FIXED_SZ (SESSION_TICKET_LEN - WOLFSSL_TICKET_ENC_SZ)

typedef struct SessionTicket {
    word32 lifetime;
#ifdef WOLFSSL_TLS13
    word64 seen;
    word32 ageAdd;
#endif
    byte*  data;
    word16 size;
} SessionTicket;

#if !defined(WOLFSSL_NO_DEF_TICKET_ENC_CB) && !defined(NO_WOLFSSL_SERVER)

/* Data passed to default SessionTicket enc/dec callback. */
typedef struct TicketEncCbCtx {
    /* Name for this context. */
    byte name[WOLFSSL_TICKET_NAME_SZ];
    /* Current keys - current and next. */
    byte key[2][WOLFSSL_TICKET_KEY_SZ];
    /* Expirary date of keys. */
    word32 expirary[2];
    /* Random number generator to use for generating name, keys and IV. */
    WC_RNG rng;
#ifndef SINGLE_THREADED
    /* Mutex for access to changing keys. */
    wolfSSL_Mutex mutex;
#endif
    /* Pointer back to SSL_CTX. */
    WOLFSSL_CTX* ctx;
} TicketEncCbCtx;

#endif /* !WOLFSSL_NO_DEF_TICKET_ENC_CB && !NO_WOLFSSL_SERVER */

WOLFSSL_LOCAL int  TLSX_UseSessionTicket(TLSX** extensions,
                                             SessionTicket* ticket, void* heap);
WOLFSSL_LOCAL SessionTicket* TLSX_SessionTicket_Create(word32 lifetime,
                                           byte* data, word16 size, void* heap);
WOLFSSL_LOCAL void TLSX_SessionTicket_Free(SessionTicket* ticket, void* heap);

#endif /* HAVE_SESSION_TICKET */

#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
int TLSX_EncryptThenMac_Respond(WOLFSSL* ssl);
#endif

#ifdef WOLFSSL_TLS13
/* Cookie extension information - cookie data. */
typedef struct Cookie {
    word16 len;
    /* Ignore "nonstandard extension used : zero-sized array in struct/union"
     * MSVC warning */
    #ifdef _MSC_VER
    #pragma warning(disable: 4200)
    #endif
    byte   data[];
} Cookie;

WOLFSSL_LOCAL int TLSX_Cookie_Use(const WOLFSSL* ssl, const byte* data,
        word16 len, byte* mac, byte macSz, int resp, TLSX** exts);
WOLFSSL_LOCAL int TlsCheckCookie(const WOLFSSL* ssl, const byte* cookie,
                                 word16 cookieSz);


/* Key Share - TLS v1.3 Specification */

/* The KeyShare extension information - entry in a linked list. */
typedef struct KeyShareEntry {
    word16                group;     /* NamedGroup                        */
    byte*                 ke;        /* Key exchange data                 */
    word32                keLen;     /* Key exchange data length          */
    void*                 key;       /* Key struct                        */
    word32                keyLen;    /* Key size (bytes)                  */
    byte*                 pubKey;    /* Public key                        */
    word32                pubKeyLen; /* Public key length                 */
#if !defined(NO_DH) || defined(HAVE_PQC)
    byte*                 privKey;   /* Private key - DH and PQ KEMs only */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    int                   lastRet;
#endif
    struct KeyShareEntry* next;      /* List pointer             */
} KeyShareEntry;

WOLFSSL_LOCAL int TLSX_KeyShare_Use(const WOLFSSL* ssl, word16 group,
        word16 len, byte* data, KeyShareEntry **kse, TLSX** extensions);
WOLFSSL_LOCAL int TLSX_KeyShare_Empty(WOLFSSL* ssl);
WOLFSSL_LOCAL int TLSX_KeyShare_SetSupported(const WOLFSSL* ssl,
        TLSX** extensions);
WOLFSSL_LOCAL int TLSX_KeyShare_GenKey(WOLFSSL *ssl, KeyShareEntry *kse);
WOLFSSL_LOCAL int TLSX_KeyShare_Choose(const WOLFSSL *ssl, TLSX* extensions,
        byte cipherSuite0, byte cipherSuite, KeyShareEntry** kse,
        byte* searched);
WOLFSSL_LOCAL int TLSX_KeyShare_Setup(WOLFSSL *ssl, KeyShareEntry* clientKSE);
WOLFSSL_LOCAL int TLSX_KeyShare_Establish(WOLFSSL* ssl, int* doHelloRetry);
WOLFSSL_LOCAL int TLSX_KeyShare_DeriveSecret(WOLFSSL* sclientKSEclientKSEsl);
WOLFSSL_LOCAL int TLSX_KeyShare_Parse(WOLFSSL* ssl, const byte* input,
        word16 length, byte msgType);
WOLFSSL_LOCAL int TLSX_KeyShare_Parse_ClientHello(const WOLFSSL* ssl,
        const byte* input, word16 length, TLSX** extensions);


#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)

enum PskDecryptReturn {
    PSK_DECRYPT_NONE = 0,
    PSK_DECRYPT_OK,
    PSK_DECRYPT_CREATE,
    PSK_DECRYPT_FAIL,
};

#ifdef HAVE_SESSION_TICKET
typedef struct psk_sess_free_cb_ctx {
    word32 row;
#ifdef HAVE_EXT_CACHE
    int extCache;
    int freeSess;
#endif
} psk_sess_free_cb_ctx;
typedef void (psk_sess_free_cb)(const WOLFSSL* ssl, const WOLFSSL_SESSION* sess,
        psk_sess_free_cb_ctx* freeCtx);
#endif

/* The PreSharedKey extension information - entry in a linked list. */
typedef struct PreSharedKey {
    word16               identityLen;             /* Length of identity */
    byte*                identity;                /* PSK identity       */
    word32               ticketAge;               /* Age of the ticket  */
    byte                 cipherSuite0;            /* Cipher Suite       */
    byte                 cipherSuite;             /* Cipher Suite       */
    word32               binderLen;               /* Length of HMAC     */
    byte                 binder[WC_MAX_DIGEST_SIZE]; /* HMAC of handshake */
    byte                 hmac;                    /* HMAC algorithm     */
#ifdef HAVE_SESSION_TICKET
    InternalTicket*      it;                      /* ptr to ticket      */
    const WOLFSSL_SESSION* sess; /* ptr to session either from external cache or
                                  * into SessionCache. Work around so that we
                                  * don't call into the cache more than once */
    psk_sess_free_cb* sess_free_cb;               /* callback to free sess */
    psk_sess_free_cb_ctx sess_free_cb_ctx;        /* info for sess_free_cb */
#endif
    byte                 resumption:1;            /* Resumption PSK     */
    byte                 chosen:1;                /* Server's choice    */
    byte                 decryptRet:3;            /* Ticket decrypt return */
    struct PreSharedKey* next;                    /* List pointer       */
} PreSharedKey;

WOLFSSL_LOCAL int TLSX_PreSharedKey_WriteBinders(PreSharedKey* list,
                                                 byte* output, byte msgType,
                                                 word16* pSz);
WOLFSSL_LOCAL int TLSX_PreSharedKey_GetSizeBinders(PreSharedKey* list,
                                                   byte msgType, word16* pSz);
WOLFSSL_LOCAL int TLSX_PreSharedKey_Use(TLSX** extensions, const byte* identity,
                                        word16 len, word32 age, byte hmac,
                                        byte cipherSuite0, byte cipherSuite,
                                        byte resumption,
                                        PreSharedKey **preSharedKey,
                                        void* heap);
WOLFSSL_LOCAL int TLSX_PreSharedKey_Parse_ClientHello(TLSX** extensions,
                                  const byte* input, word16 length, void* heap);

/* The possible Pre-Shared Key key exchange modes. */
enum PskKeyExchangeMode {
    PSK_KE,
    PSK_DHE_KE
};

/* User can define this. */
#ifndef WOLFSSL_DEF_PSK_CIPHER
#define WOLFSSL_DEF_PSK_CIPHER    TLS_AES_128_GCM_SHA256
#endif

WOLFSSL_LOCAL int TLSX_PskKeyModes_Use(WOLFSSL* ssl, byte modes);
WOLFSSL_LOCAL int TLSX_PskKeyModes_Parse_Modes(const byte* input, word16 length,
                                              byte msgType, byte* modes);

#ifdef WOLFSSL_EARLY_DATA
WOLFSSL_LOCAL int TLSX_EarlyData_Use(WOLFSSL* ssl, word32 max, int is_response);
#endif
#endif /* HAVE_SESSION_TICKET || !NO_PSK */


/* The types of keys to derive for. */
enum DeriveKeyType {
    no_key,
    early_data_key,
    handshake_key,
    traffic_key,
    update_traffic_key
};

WOLFSSL_LOCAL int DeriveEarlySecret(WOLFSSL* ssl);
WOLFSSL_LOCAL int DeriveHandshakeSecret(WOLFSSL* ssl);
WOLFSSL_LOCAL int DeriveTls13Keys(WOLFSSL* ssl, int secret, int side, int store);
WOLFSSL_LOCAL int DeriveMasterSecret(WOLFSSL* ssl);
WOLFSSL_LOCAL int DeriveResumptionPSK(WOLFSSL* ssl, byte* nonce, byte nonceLen, byte* secret);
WOLFSSL_LOCAL int DeriveResumptionSecret(WOLFSSL* ssl, byte* key);

WOLFSSL_LOCAL int Tls13_Exporter(WOLFSSL* ssl, unsigned char *out, size_t outLen,
        const char *label, size_t labelLen,
        const unsigned char *context, size_t contextLen);

/* The key update request values for KeyUpdate message. */
enum KeyUpdateRequest {
    update_not_requested,
    update_requested
};
#endif /* WOLFSSL_TLS13 */

#ifdef WOLFSSL_DTLS_CID
WOLFSSL_LOCAL void TLSX_ConnectionID_Free(byte* ext, void* heap);
WOLFSSL_LOCAL word16 TLSX_ConnectionID_Write(byte* ext, byte* output);
WOLFSSL_LOCAL word16 TLSX_ConnectionID_GetSize(byte* ext);
WOLFSSL_LOCAL int TLSX_ConnectionID_Use(WOLFSSL* ssl);
WOLFSSL_LOCAL int TLSX_ConnectionID_Parse(WOLFSSL* ssl, const byte* input,
    word16 length, byte isRequest);
WOLFSSL_LOCAL void DtlsCIDOnExtensionsParsed(WOLFSSL* ssl);
WOLFSSL_LOCAL byte DtlsCIDCheck(WOLFSSL* ssl, const byte* input,
    word16 inputSize);
#endif /* WOLFSSL_DTLS_CID */

#ifdef OPENSSL_EXTRA
enum SetCBIO {
    WOLFSSL_CBIO_NONE = 0,
    WOLFSSL_CBIO_RECV = 0x1,
    WOLFSSL_CBIO_SEND = 0x2,
};
#endif

#ifdef WOLFSSL_STATIC_EPHEMERAL
/* contains static ephemeral keys */
typedef struct {
#ifndef NO_DH
    DerBuffer* dhKey;
#endif
#ifdef HAVE_ECC
    DerBuffer* ecKey;
#endif
#ifdef HAVE_CURVE25519
    DerBuffer* x25519Key;
#endif
#ifdef HAVE_CURVE448
    DerBuffer* x448Key;
#endif
} StaticKeyExchangeInfo_t;
#endif /* WOLFSSL_STATIC_EPHEMERAL */


/* wolfSSL context type */
struct WOLFSSL_CTX {
    WOLFSSL_METHOD* method;
#ifdef SINGLE_THREADED
    WC_RNG*         rng;          /* to be shared with WOLFSSL w/o locking */
#endif
    wolfSSL_Ref     ref;
    int         err;              /* error code in case of mutex not created */
#ifndef NO_DH
    buffer      serverDH_P;
    buffer      serverDH_G;
#endif
#ifndef NO_CERTS
    DerBuffer*  certificate;
    DerBuffer*  certChain;
                 /* chain after self, in DER, with leading size for each cert */
    #ifndef WOLFSSL_NO_CA_NAMES
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* client_ca_names;
    #endif
    #ifdef OPENSSL_EXTRA
    WOLF_STACK_OF(WOLFSSL_X509)* x509Chain;
    client_cert_cb CBClientCert;  /* client certificate callback */
    CertSetupCallback  certSetupCb;
    void*              certSetupCbArg;
    #endif
#ifdef WOLFSSL_TLS13
    int         certChainCnt;
#endif
    DerBuffer*  privateKey;
    byte        privateKeyType:6;
    byte        privateKeyId:1;
    byte        privateKeyLabel:1;
    int         privateKeySz;
    int         privateKeyDevId;
#ifdef OPENSSL_ALL
    WOLFSSL_EVP_PKEY* privateKeyPKey;
#endif
    WOLFSSL_CERT_MANAGER* cm;      /* our cert manager, ctx owns SSL will use */
#endif
#ifdef KEEP_OUR_CERT
    WOLFSSL_X509*    ourCert;     /* keep alive a X509 struct of cert */
    int              ownOurCert;  /* Dispose of certificate if we own */
#endif
    Suites*     suites;           /* make dynamic, user may not need/set */
    void*       heap;             /* for user memory overrides */
    byte        verifyDepth;
    byte        verifyPeer:1;
    byte        verifyNone:1;
    byte        failNoCert:1;
    byte        failNoCertxPSK:1; /* fail if no cert with the exception of PSK*/
    byte        sessionCacheOff:1;
    byte        sessionCacheFlushOff:1;
#ifdef HAVE_EXT_CACHE
    byte        internalCacheOff:1;
    byte        internalCacheLookupOff:1;
#endif
    byte        sendVerify:2;     /* for client side (can not be single bit) */
    byte        haveRSA:1;        /* RSA available */
    byte        haveECC:1;        /* ECC available */
    byte        haveDH:1;         /* server DH parms set by user */
    byte        haveECDSAsig:1;   /* server cert signed w/ ECDSA */
    byte        haveFalconSig:1;  /* server cert signed w/ Falcon */
    byte        haveDilithiumSig:1;/* server cert signed w/ Dilithium */
    byte        haveStaticECC:1;  /* static server ECC private key */
    byte        partialWrite:1;   /* only one msg per write call */
    byte        autoRetry:1;      /* retry read/write on a WANT_{READ|WRITE} */
    byte        quietShutdown:1;  /* don't send close notify */
    byte        groupMessages:1;  /* group handshake messages before sending */
    byte        minDowngrade;     /* minimum downgrade version */
    byte        haveEMS:1;        /* have extended master secret extension */
    byte        useClientOrder:1; /* Use client's cipher preference order */
#if defined(HAVE_SESSION_TICKET)
    byte        noTicketTls12:1;  /* TLS 1.2 server won't send ticket */
#endif
#ifdef WOLFSSL_TLS13
    #if defined(HAVE_SESSION_TICKET) && !defined(NO_WOLFSSL_SERVER)
    unsigned int maxTicketTls13;  /* maximum number of tickets to send */
    #endif
    byte        noTicketTls13:1;  /* TLS 1.3 Server won't create new Ticket */
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    byte        noPskDheKe:1;     /* Don't use (EC)DHE with PSK */
#ifdef HAVE_SUPPORTED_CURVES
    byte        onlyPskDheKe:1;   /* Only use (EC)DHE with PSK */
#endif
#endif
#endif /* WOLFSSL_TLS13 */
    byte        mutualAuth:1;     /* Mutual authentication required */
#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
    byte        postHandshakeAuth:1;  /* Post-handshake auth supported. */
    byte        verifyPostHandshake:1; /* Only send client cert req post
                                        * handshake, not also during */
#endif
#ifndef NO_DH
    #if !defined(WOLFSSL_OLD_PRIME_CHECK) && !defined(HAVE_FIPS) && \
        !defined(HAVE_SELFTEST)
    byte        dhKeyTested:1;   /* Set when key has been tested. */
    #endif
#endif
#if defined(HAVE_SECURE_RENEGOTIATION) || defined(HAVE_SERVER_RENEGOTIATION_INFO)
    byte        useSecureReneg:1; /* when set will set WOLFSSL objects generated to enable */
#endif
#ifdef HAVE_ENCRYPT_THEN_MAC
    byte        disallowEncThenMac:1;  /* Don't do Encrypt-Then-MAC */
#endif
#ifdef WOLFSSL_STATIC_MEMORY
    byte        onHeapHint:1; /* whether the ctx/method is put on heap hint */
#endif
#if defined(WOLFSSL_STATIC_EPHEMERAL) && !defined(SINGLE_THREADED)
    byte        staticKELockInit:1;
#endif
#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_SCTP)
    byte        dtlsSctp:1;         /* DTLS-over-SCTP mode */
#endif
    word16      minProto:1; /* sets min to min available */
    word16      maxProto:1; /* sets max to max available */

#ifdef WOLFSSL_SRTP
    word16      dtlsSrtpProfiles;  /* DTLS-with-SRTP mode
                                    * (list of selected profiles - up to 16) */
#endif
#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_MULTICAST)
    byte        haveMcast;        /* multicast requested */
    byte        mcastID;          /* multicast group ID */
#endif
#if defined(WOLFSSL_DTLS) && \
    (defined(WOLFSSL_SCTP) || defined(WOLFSSL_DTLS_MTU))
    word16      dtlsMtuSz;        /* DTLS MTU size */
#endif
#ifndef NO_DH
    word16      minDhKeySz;       /* minimum DH key size */
    word16      maxDhKeySz;       /* maximum DH key size */
#endif
#ifndef NO_RSA
    short       minRsaKeySz;      /* minimum RSA key size */
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    short       minEccKeySz;      /* minimum ECC key size */
#endif
#ifdef HAVE_PQC
    short       minFalconKeySz;   /* minimum Falcon key size */
    short       minDilithiumKeySz;/* minimum Dilithium key size */
#endif
    unsigned long     mask;             /* store SSL_OP_ flags */
#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
    word32            disabledCurves;   /* curves disabled by user */
#endif
#ifdef OPENSSL_EXTRA
    byte              sessionCtx[ID_LEN]; /* app session context ID */
    const unsigned char *alpn_cli_protos;/* ALPN client protocol list */
    unsigned int         alpn_cli_protos_len;
    byte              sessionCtxSz;
    byte              cbioFlag;  /* WOLFSSL_CBIO_RECV/SEND: CBIORecv/Send is set */
    CallbackInfoState* CBIS;      /* used to get info about SSL state */
    WOLFSSL_X509_VERIFY_PARAM* param;    /* verification parameters*/
#endif
#ifdef WOLFSSL_WOLFSENTRY_HOOKS
    NetworkFilterCallback_t AcceptFilter;
    void *AcceptFilter_arg;
    NetworkFilterCallback_t ConnectFilter;
    void *ConnectFilter_arg;
#endif /* WOLFSSL_WOLFSENTRY_HOOKS */
    CallbackIORecv CBIORecv;
    CallbackIOSend CBIOSend;
#ifdef WOLFSSL_DTLS
    CallbackGenCookie CBIOCookie;       /* gen cookie callback */
#endif /* WOLFSSL_DTLS */
#ifdef WOLFSSL_SESSION_EXPORT
#ifdef WOLFSSL_DTLS
    wc_dtls_export  dtls_export;        /* export function for DTLS session */
#endif
    CallbackGetPeer CBGetPeer;
    CallbackSetPeer CBSetPeer;
#endif
    VerifyCallback  verifyCallback;     /* cert verification callback */
    void*           verifyCbCtx;        /* cert verify callback user ctx*/
#ifdef OPENSSL_ALL
    CertVerifyCallback verifyCertCb;
    void*              verifyCertCbArg;
#endif /* OPENSSL_ALL */
#ifdef OPENSSL_EXTRA
    SSL_Msg_Cb      protoMsgCb;         /* inspect protocol message callback */
    void*           protoMsgCtx;        /* user set context with msg callback */
#endif
    word32          timeout;            /* session timeout */
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_CURVE25519) || \
    defined(HAVE_ED448)
    word32          ecdhCurveOID;       /* curve Ecc_Sum */
#endif
#ifdef HAVE_ECC
    word16          eccTempKeySz;       /* in octets 20 - 66 */
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    word32          pkCurveOID;         /* curve Ecc_Sum */
#endif
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    byte        havePSK;                /* psk key set by user */
    wc_psk_client_callback client_psk_cb;  /* client callback */
    wc_psk_server_callback server_psk_cb;  /* server callback */
#ifdef WOLFSSL_TLS13
    wc_psk_client_cs_callback    client_psk_cs_cb;     /* client callback */
    wc_psk_client_tls13_callback client_psk_tls13_cb;  /* client callback */
    wc_psk_server_tls13_callback server_psk_tls13_cb;  /* server callback */
#endif
    void*       psk_ctx;
    char        server_hint[MAX_PSK_ID_LEN + NULL_TERM_LEN];
#endif /* HAVE_SESSION_TICKET || !NO_PSK */
#ifdef WOLFSSL_TLS13
    word16          group[WOLFSSL_MAX_GROUP_COUNT];
    byte            numGroups;
#endif
#ifdef WOLFSSL_EARLY_DATA
    word32          maxEarlyDataSz;
#endif
#ifdef HAVE_ANON
    byte        haveAnon;               /* User wants to allow Anon suites */
#endif /* HAVE_ANON */
#ifdef WOLFSSL_ENCRYPTED_KEYS
    wc_pem_password_cb* passwd_cb;
    void*               passwd_userdata;
#endif
#ifdef WOLFSSL_LOCAL_X509_STORE
    WOLFSSL_X509_STORE x509_store; /* points to ctx->cm */
    WOLFSSL_X509_STORE* x509_store_pt; /* take ownership of external store */
#endif
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || defined(WOLFSSL_WPAS_SMALL)
    byte            readAhead;
    void*           userPRFArg; /* passed to prf callback */
#endif
#ifdef HAVE_EX_DATA
    WOLFSSL_CRYPTO_EX_DATA ex_data;
#endif
#if defined(HAVE_ALPN) && (defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(HAVE_LIGHTY) || defined(WOLFSSL_QUIC))
    CallbackALPNSelect alpnSelect;
    void*              alpnSelectArg;
#endif
#ifdef HAVE_SNI
    CallbackSniRecv sniRecvCb;
    void*           sniRecvCbArg;
#endif
#if defined(WOLFSSL_MULTICAST) && defined(WOLFSSL_DTLS)
    CallbackMcastHighwater mcastHwCb; /* Sequence number highwater callback */
    word32      mcastFirstSeq;    /* first trigger level */
    word32      mcastSecondSeq;   /* second trigger level */
    word32      mcastMaxSeq;      /* max level */
#endif
#ifdef HAVE_OCSP
    WOLFSSL_OCSP      ocsp;
#endif
    int             devId;              /* async device id to use */
#ifdef HAVE_TLS_EXTENSIONS
    TLSX* extensions;                  /* RFC 6066 TLS Extensions data */
    #ifndef NO_WOLFSSL_SERVER
        #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
         || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
            OcspRequest* certOcspRequest;
        #endif
        #if defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
            OcspRequest* chainOcspRequest[MAX_CHAIN_DEPTH];
        #endif
    #endif
    #if defined(HAVE_SESSION_TICKET) && !defined(NO_WOLFSSL_SERVER)
        SessionTicketEncCb ticketEncCb;   /* enc/dec session ticket Cb */
        void*              ticketEncCtx;  /* session encrypt context */
        #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) \
          || defined(OPENSSL_EXTRA) || defined(HAVE_LIGHTY)
        ticketCompatCb     ticketEncWrapCb; /* callback for OpenSSL ticket key callback */
        #endif
        int                ticketHint;    /* ticket hint in seconds */
        #ifndef WOLFSSL_NO_DEF_TICKET_ENC_CB
            TicketEncCbCtx ticketKeyCtx;
        #endif
    #endif
    #endif
    #ifdef HAVE_SUPPORTED_CURVES
        byte userCurves;                  /* indicates user called wolfSSL_CTX_UseSupportedCurve */
    #endif
#ifdef ATOMIC_USER
    CallbackMacEncrypt    MacEncryptCb;    /* Atomic User Mac/Encrypt Cb */
    CallbackDecryptVerify DecryptVerifyCb; /* Atomic User Decrypt/Verify Cb */
    #ifdef HAVE_ENCRYPT_THEN_MAC
        CallbackEncryptMac    EncryptMacCb;    /* Atomic User Mac/Enc Cb */
        CallbackVerifyDecrypt VerifyDecryptCb; /* Atomic User Dec/Verify Cb */
    #endif
#endif
#ifdef HAVE_PK_CALLBACKS
    #ifdef HAVE_ECC
        CallbackEccKeyGen EccKeyGenCb;  /* User EccKeyGen Callback Handler */
        CallbackEccSign   EccSignCb;    /* User EccSign   Callback handler */
        void*             EccSignCtx;   /* Ecc Sign       Callback Context */
        CallbackEccVerify EccVerifyCb;  /* User EccVerify Callback handler */
        CallbackEccSharedSecret EccSharedSecretCb; /* User EccVerify Callback handler */
    #endif /* HAVE_ECC */
    #ifdef HAVE_HKDF
        CallbackHKDFExtract HkdfExtractCb; /* User hkdf Extract Callback handler */
    #endif
    #ifdef HAVE_ED25519
        /* User Ed25519Sign   Callback handler */
        CallbackEd25519Sign   Ed25519SignCb;
        /* User Ed25519Verify Callback handler */
        CallbackEd25519Verify Ed25519VerifyCb;
    #endif
    #ifdef HAVE_CURVE25519
        /* User X25519 KeyGen Callback Handler */
        CallbackX25519KeyGen X25519KeyGenCb;
        /* User X25519 SharedSecret Callback handler */
        CallbackX25519SharedSecret X25519SharedSecretCb;
    #endif
    #ifdef HAVE_ED448
        /* User Ed448Sign   Callback handler */
        CallbackEd448Sign   Ed448SignCb;
        /* User Ed448Verify Callback handler */
        CallbackEd448Verify Ed448VerifyCb;
    #endif
    #ifdef HAVE_CURVE448
        /* User X448 KeyGen Callback Handler */
        CallbackX448KeyGen X448KeyGenCb;
        /* User X448 SharedSecret Callback handler */
        CallbackX448SharedSecret X448SharedSecretCb;
    #endif
    #ifndef NO_DH
        /* User DH KeyGen Callback handler*/
        CallbackDhGenerateKeyPair DhGenerateKeyPairCb;
        /* User DH Agree Callback handler */
        CallbackDhAgree DhAgreeCb;
    #endif
    #ifndef NO_RSA
        /* User RsaSign Callback handler (priv key) */
        CallbackRsaSign   RsaSignCb;
        /* User RsaVerify Callback handler (pub key) */
        CallbackRsaVerify RsaVerifyCb;
        /* User VerifyRsaSign Callback handler (priv key) */
        CallbackRsaVerify RsaSignCheckCb;
        #ifdef WC_RSA_PSS
            /* User RsaSign (priv key) */
            CallbackRsaPssSign   RsaPssSignCb;
            /* User RsaVerify (pub key) */
            CallbackRsaPssVerify RsaPssVerifyCb;
            /* User VerifyRsaSign (priv key) */
            CallbackRsaPssVerify RsaPssSignCheckCb;
        #endif
        CallbackRsaEnc    RsaEncCb;     /* User Rsa Public Encrypt  handler */
        CallbackRsaDec    RsaDecCb;     /* User Rsa Private Decrypt handler */
    #endif /* NO_RSA */

    /* User generate pre-master handler */
    CallbackGenPreMaster        GenPreMasterCb;
    /* User generate master secret handler */
    CallbackGenMasterSecret     GenMasterCb;
    /* User generate session key handler */
    CallbackGenSessionKey       GenSessionKeyCb;
    /* User setting encrypt keys handler */
    CallbackEncryptKeys         EncryptKeysCb;
    /* User Tls finished handler */
    CallbackTlsFinished         TlsFinishedCb;
#if !defined(WOLFSSL_NO_TLS12) && !defined(WOLFSSL_AEAD_ONLY)
    /* User Verify mac handler */
    CallbackVerifyMac           VerifyMacCb;
#endif
#if defined(WOLFSSL_PUBLIC_ASN)
    /* User handler to process a certificate */
    CallbackProcessPeerCert ProcessPeerCertCb;
#endif
    /* User handler to process the server's key exchange public key */
    CallbackProcessServerSigKex ProcessServerSigKexCb;
    /* User handler to process the TLS record */
    CallbackPerformTlsRecordProcessing PerformTlsRecordProcessingCb;
    /* User handler to do HKDF expansions */
    CallbackHKDFExpandLabel HKDFExpandLabelCb;

#endif /* HAVE_PK_CALLBACKS */
#ifdef HAVE_WOLF_EVENT
    WOLF_EVENT_QUEUE event_queue;
#endif /* HAVE_WOLF_EVENT */
#ifdef HAVE_EXT_CACHE
    WOLFSSL_SESSION*(*get_sess_cb)(WOLFSSL*, const unsigned char*, int, int*);
    int (*new_sess_cb)(WOLFSSL*, WOLFSSL_SESSION*);
#endif
#if defined(HAVE_EXT_CACHE) || defined(HAVE_EX_DATA)
    Rem_Sess_Cb rem_sess_cb;
#endif
#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) && !defined(NO_SHA256)
    Srp*  srp;  /* TLS Secure Remote Password Protocol*/
    byte* srp_password;
#endif
#if defined(OPENSSL_EXTRA) && defined(HAVE_SECRET_CALLBACK)
    wolfSSL_CTX_keylog_cb_func keyLogCb;
#endif /* OPENSSL_EXTRA && HAVE_SECRET_CALLBACK */
#ifdef WOLFSSL_STATIC_EPHEMERAL
    StaticKeyExchangeInfo_t staticKE;
    #ifndef SINGLE_THREADED
    wolfSSL_Mutex staticKELock;
    #endif
#endif
#ifdef WOLFSSL_QUIC
    struct {
        const WOLFSSL_QUIC_METHOD *method;
    } quic;
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    WOLFSSL_EchConfig* echConfigs;
#endif
};

WOLFSSL_LOCAL
int InitSSL_Ctx(WOLFSSL_CTX* ctx, WOLFSSL_METHOD* method, void* heap);
WOLFSSL_LOCAL
void FreeSSL_Ctx(WOLFSSL_CTX* ctx);
WOLFSSL_LOCAL
void SSL_CtxResourceFree(WOLFSSL_CTX* ctx);

#ifdef HAVE_EX_DATA_CLEANUP_HOOKS
    #ifndef HAVE_EX_DATA
        #error "HAVE_EX_DATA_CLEANUP_HOOKS requires HAVE_EX_DATA to be defined"
    #endif
void wolfSSL_CRYPTO_cleanup_ex_data(WOLFSSL_CRYPTO_EX_DATA* ex_data);
#endif

WOLFSSL_LOCAL
int DeriveTlsKeys(WOLFSSL* ssl);
WOLFSSL_LOCAL
int ProcessOldClientHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                          word32 inSz, word16 sz);

#ifndef NO_CERTS
    WOLFSSL_LOCAL
    int AddCA(WOLFSSL_CERT_MANAGER* cm, DerBuffer** pDer, int type, int verify);
    WOLFSSL_LOCAL
    int AlreadySigner(WOLFSSL_CERT_MANAGER* cm, byte* hash);
#ifdef WOLFSSL_TRUST_PEER_CERT
    WOLFSSL_LOCAL
    int AddTrustedPeer(WOLFSSL_CERT_MANAGER* cm, DerBuffer** pDer, int verify);
    WOLFSSL_LOCAL
    int AlreadyTrustedPeer(WOLFSSL_CERT_MANAGER* cm, DecodedCert* cert);
#endif
#endif

/* All cipher suite related info
 * Keep as a constant size (no ifdefs) for session export */
typedef struct CipherSpecs {
    word16 key_size;
    word16 iv_size;
    word16 block_size;
    word16 aead_mac_size;
    byte bulk_cipher_algorithm;
    byte cipher_type;               /* block, stream, or aead */
    byte mac_algorithm;
    byte kea;                       /* key exchange algo */
    byte sig_algo;
    byte hash_size;
    byte pad_size;
    byte static_ecdh;
} CipherSpecs;


void InitCipherSpecs(CipherSpecs* cs);


/* Supported Key Exchange Protocols */
enum KeyExchangeAlgorithm {
    no_kea,
    rsa_kea,
    diffie_hellman_kea,
    fortezza_kea,
    psk_kea,
    dhe_psk_kea,
    ecdhe_psk_kea,
    ecc_diffie_hellman_kea,
    ecc_static_diffie_hellman_kea       /* for verify suite only */
};

/* Used with InitSuitesHashSigAlgo_ex2 */
#define SIG_ECDSA       0x01
#define SIG_RSA         0x02
#define SIG_SM2         0x04
#define SIG_FALCON      0x08
#define SIG_DILITHIUM   0x10
#define SIG_ANON        0x20

/* Supported Authentication Schemes */
enum SignatureAlgorithm {
    anonymous_sa_algo            = 0,
    rsa_sa_algo                  = 1,
    dsa_sa_algo                  = 2,
    ecc_dsa_sa_algo              = 3,
    rsa_pss_sa_algo              = 8,
    ed25519_sa_algo              = 9,
    rsa_pss_pss_algo             = 10,
    ed448_sa_algo                = 11,
    falcon_level1_sa_algo        = 12,
    falcon_level5_sa_algo        = 13,
    dilithium_level2_sa_algo     = 14,
    dilithium_level3_sa_algo     = 15,
    dilithium_level5_sa_algo     = 16,
    sm2_sa_algo                  = 17,
    invalid_sa_algo              = 255
};

#define PSS_RSAE_TO_PSS_PSS(macAlgo) \
    ((macAlgo) + (pss_sha256 - sha256_mac))

#define PSS_PSS_HASH_TO_MAC(macAlgo) \
    ((macAlgo) - (pss_sha256 - sha256_mac))

enum SigAlgRsaPss {
    pss_sha256  = 0x09,
    pss_sha384  = 0x0a,
    pss_sha512  = 0x0b,
};

#ifdef WOLFSSL_SM2
    /* Default SM2 signature ID. */
    #define TLS12_SM2_SIG_ID        ((byte*)"1234567812345678")
    /* Length of default SM2 signature ID. */
    #define TLS12_SM2_SIG_ID_SZ     16

    /* https://www.rfc-editor.org/rfc/rfc8998.html#name-sm2-signature-scheme */
    /* ID to use when signing/verifying TLS v1.3 data. */
    #define TLS13_SM2_SIG_ID        ((byte*)"TLSv1.3+GM+Cipher+Suite")
    /* Length of ID to use when signing/verifying TLS v1.3 data. */
    #define TLS13_SM2_SIG_ID_SZ     23
#endif

/* Supported ECC Curve Types */
enum EccCurves {
    named_curve = 3
};


/* Valid client certificate request types from page 27 */
enum ClientCertificateType {
    rsa_sign            = 1,
    dss_sign            = 2,
    rsa_fixed_dh        = 3,
    dss_fixed_dh        = 4,
    rsa_ephemeral_dh    = 5,
    dss_ephemeral_dh    = 6,
    fortezza_kea_cert   = 20,
    ecdsa_sign          = 64,
    rsa_fixed_ecdh      = 65,
    ecdsa_fixed_ecdh    = 66,
    falcon_sign         = 67,
    dilithium_sign      = 68,
};


#ifndef WOLFSSL_AEAD_ONLY
enum CipherType { stream, block, aead };
#else
enum CipherType { aead };
#endif


#if defined(BUILD_AES) || defined(BUILD_AESGCM) || defined(HAVE_ARIA) || \
        (defined(HAVE_CHACHA) && defined(HAVE_POLY1305)) || defined(WOLFSSL_TLS13)
    #define CIPHER_NONCE
#endif

#if defined(WOLFSSL_DTLS) && defined(HAVE_SECURE_RENEGOTIATION)
enum CipherSrc {
    KEYS_NOT_SET = 0,
    KEYS,     /* keys from ssl->keys are loaded */
    SCR       /* keys from ssl->secure_renegotiation->tmp_keys are loaded */
};
#endif

#ifdef WOLFSSL_CIPHER_TEXT_CHECK
    #ifndef WOLFSSL_CIPHER_CHECK_SZ
        /* 64-bits to confirm encrypt operation worked */
        #define WOLFSSL_CIPHER_CHECK_SZ 8
    #endif
#endif

/* cipher for now */
typedef struct Ciphers {
#ifdef BUILD_ARC4
    Arc4*   arc4;
#endif
#ifdef BUILD_DES3
    Des3*   des3;
#endif
#if defined(BUILD_AES) || defined(BUILD_AESGCM)
    Aes*    aes;
#endif
#if (defined(BUILD_AESGCM) || defined(HAVE_AESCCM)) && !defined(WOLFSSL_NO_TLS12)
    byte* additional;
#endif
#ifdef HAVE_ARIA
    wc_Aria* aria;
#endif
#ifdef CIPHER_NONCE
    byte* nonce;
#endif
#ifdef HAVE_CAMELLIA
    Camellia* cam;
#endif
#ifdef HAVE_CHACHA
    ChaCha*   chacha;
#endif
#ifdef WOLFSSL_SM4
    wc_Sm4*   sm4;
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_NULL_CIPHER) && !defined(NO_HMAC)
    Hmac* hmac;
#endif
#ifdef WOLFSSL_CIPHER_TEXT_CHECK
    word32 sanityCheck[WOLFSSL_CIPHER_CHECK_SZ/sizeof(word32)];
#endif
    byte    state;
    byte    setup;       /* have we set it up flag for detection */
#if defined(WOLFSSL_DTLS) && defined(HAVE_SECURE_RENEGOTIATION)
    enum CipherSrc src;  /* DTLS uses this to determine which keys
                          * are currently loaded */
#endif
} Ciphers;

#ifdef WOLFSSL_DTLS13
typedef struct RecordNumberCiphers {
#if defined(BUILD_AES) || defined(BUILD_AESGCM)
        Aes *aes;
#endif /*  BUILD_AES || BUILD_AESGCM */
#ifdef HAVE_CHACHA
        ChaCha *chacha;
#endif
} RecordNumberCiphers;
#endif /* WOLFSSL_DTLS13 */

#ifdef HAVE_ONE_TIME_AUTH
/* Ciphers for one time authentication such as poly1305 */
typedef struct OneTimeAuth {
#ifdef HAVE_POLY1305
    Poly1305* poly1305;
#endif
    byte    setup;      /* flag for if a cipher has been set */

} OneTimeAuth;
#endif


WOLFSSL_LOCAL void InitCiphers(WOLFSSL* ssl);
WOLFSSL_LOCAL void FreeCiphers(WOLFSSL* ssl);


/* hashes type */
typedef struct Hashes {
    #if !defined(NO_MD5) && !defined(NO_OLD_TLS)
        byte md5[WC_MD5_DIGEST_SIZE];
    #endif
    #if !defined(NO_SHA)
        byte sha[WC_SHA_DIGEST_SIZE];
    #endif
    #ifndef NO_SHA256
        byte sha256[WC_SHA256_DIGEST_SIZE];
    #endif
    #ifdef WOLFSSL_SHA384
        byte sha384[WC_SHA384_DIGEST_SIZE];
    #endif
    #ifdef WOLFSSL_SHA512
        byte sha512[WC_SHA512_DIGEST_SIZE];
    #endif
    #ifdef WOLFSSL_SM3
        byte sm3[WC_SM3_DIGEST_SIZE];
    #endif
} Hashes;

WOLFSSL_LOCAL int BuildCertHashes(const WOLFSSL* ssl, Hashes* hashes);

#ifdef WOLFSSL_TLS13
typedef union Digest {
#ifndef NO_WOLFSSL_SHA256
    wc_Sha256 sha256;
#endif
#ifdef WOLFSSL_SHA384
    wc_Sha384 sha384;
#endif
#ifdef WOLFSSL_SHA512
    wc_Sha512 sha512;
#endif
#ifdef WOLFSSL_SM3
    wc_Sm3    sm3;
#endif
} Digest;
#endif

/* Static x509 buffer */
typedef struct x509_buffer {
    int  length;                  /* actual size */
    byte buffer[MAX_X509_SIZE];   /* max static cert size */
} x509_buffer;


/* wolfSSL X509_CHAIN, for no dynamic memory SESSION_CACHE */
struct WOLFSSL_X509_CHAIN {
    int         count;                    /* total number in chain */
    x509_buffer certs[MAX_CHAIN_DEPTH];   /* only allow max depth 4 for now */
};

typedef enum WOLFSSL_SESSION_TYPE {
    WOLFSSL_SESSION_TYPE_UNKNOWN,
    WOLFSSL_SESSION_TYPE_SSL,    /* in ssl->session */
    WOLFSSL_SESSION_TYPE_CACHE,  /* pointer to internal cache */
    WOLFSSL_SESSION_TYPE_HEAP    /* allocated from heap SESSION_new */
} WOLFSSL_SESSION_TYPE;

#ifdef WOLFSSL_QUIC
typedef struct QuicRecord QuicRecord;
typedef struct QuicRecord {
    struct QuicRecord *next;
    uint8_t *data;
    word32 capacity;
    word32 len;
    word32 start;
    word32 end;
    WOLFSSL_ENCRYPTION_LEVEL level;
    word32 rec_hdr_remain;
} QuicEncData;

typedef struct QuicTransportParam QuicTransportParam;
struct QuicTransportParam {
    const uint8_t *data;
    word16 len;
};

WOLFSSL_LOCAL const QuicTransportParam *QuicTransportParam_new(const uint8_t *data, size_t len, void *heap);
WOLFSSL_LOCAL const QuicTransportParam *QuicTransportParam_dup(const QuicTransportParam *tp, void *heap);
WOLFSSL_LOCAL void QuicTransportParam_free(const QuicTransportParam *tp, void *heap);
WOLFSSL_LOCAL int TLSX_QuicTP_Use(WOLFSSL* ssl, TLSX_Type ext_type, int is_response);
WOLFSSL_LOCAL int wolfSSL_quic_add_transport_extensions(WOLFSSL *ssl, int msg_type);

#define QTP_FREE     QuicTransportParam_free

#endif /* WOLFSSL_QUIC */

/** Session Ticket - RFC 5077 (session 3.2) */
#if defined(WOLFSSL_TLS13) && defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
/* Ticket nonce - for deriving PSK.
   Length allowed to be: 1..255. Only support
 * TLS13_TICKET_NONCE_STATIC_SZ length bytes.
 */
typedef struct TicketNonce {
    byte len;
#if defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                    \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
    byte *data;
    byte dataStatic[MAX_TICKET_NONCE_STATIC_SZ];
#else
    byte data[MAX_TICKET_NONCE_STATIC_SZ];
#endif /* WOLFSSL_TICKET_NONCE_MALLOC  && FIPS_VERSION_GE(5,3) */
} TicketNonce;

#endif

/* wolfSSL session type */
struct WOLFSSL_SESSION {
    /* WARNING Do not add fields here. They will be ignored in
     *         wolfSSL_DupSession. */
    WOLFSSL_SESSION_TYPE type;
#ifndef NO_SESSION_CACHE
    int                cacheRow;          /* row in session cache     */
#endif
    wolfSSL_Ref        ref;
    byte               altSessionID[ID_LEN];
    byte               haveAltSessionID:1;
#ifdef HAVE_EX_DATA
    byte               ownExData:1;
#endif
#if defined(HAVE_EXT_CACHE) || defined(HAVE_EX_DATA)
    Rem_Sess_Cb        rem_sess_cb;
#endif
    void*              heap;
    /* WARNING The above fields (up to and including the heap) are not copied
     *         in wolfSSL_DupSession. Place new fields after the heap
     *         member */

    byte               side;              /* Either WOLFSSL_CLIENT_END or
                                                    WOLFSSL_SERVER_END */

    word32             bornOn;            /* create time in seconds   */
    word32             timeout;           /* timeout in seconds       */

    byte               sessionID[ID_LEN]; /* id for protocol or bogus
                                           * ID for TLS 1.3           */
    byte               sessionIDSz;

    byte               masterSecret[SECRET_LEN]; /* stored secret     */
    word16             haveEMS;           /* ext master secret flag   */
#if defined(SESSION_CERTS) && defined(OPENSSL_EXTRA)
    WOLFSSL_X509*      peer;              /* peer cert */
#endif
#if defined(SESSION_CERTS) || (defined(WOLFSSL_TLS13) && \
                               defined(HAVE_SESSION_TICKET))
    ProtocolVersion    version;           /* which version was used   */
#endif
#if defined(SESSION_CERTS) || !defined(NO_RESUME_SUITE_CHECK) || \
                        (defined(WOLFSSL_TLS13) && defined(HAVE_SESSION_TICKET))
    byte               cipherSuite0;      /* first byte, normally 0   */
    byte               cipherSuite;       /* 2nd byte, actual suite   */
#endif
#ifndef NO_CLIENT_CACHE
    word16             idLen;             /* serverID length          */
    byte               serverID[SERVER_ID_LEN]; /* for easier client lookup */
#endif
#ifdef OPENSSL_EXTRA
    byte               sessionCtxSz;      /* sessionCtx length        */
    byte               sessionCtx[ID_LEN]; /* app specific context id */
#endif /* OPENSSL_EXTRA */
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    byte               peerVerifyRet;     /* cert verify error */
#endif
#ifdef WOLFSSL_TLS13
    word16             namedGroup;
#endif
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
#ifdef WOLFSSL_TLS13
#ifdef WOLFSSL_32BIT_MILLI_TIME
    word32             ticketSeen;        /* Time ticket seen (ms) */
#else
    sword64            ticketSeen;        /* Time ticket seen (ms) */
#endif
    word32             ticketAdd;         /* Added by client */
    TicketNonce        ticketNonce;       /* Nonce used to derive PSK */
#endif
#ifdef WOLFSSL_EARLY_DATA
    word32             maxEarlyDataSz;
#endif
#endif
#ifdef HAVE_SESSION_TICKET
    byte               staticTicket[SESSION_TICKET_LEN];
    byte*              ticket;
    word16             ticketLen;
    word16             ticketLenAlloc;    /* is dynamic */
#endif

#ifdef SESSION_CERTS
    WOLFSSL_X509_CHAIN chain;             /* peer cert chain, static  */
    #ifdef WOLFSSL_ALT_CERT_CHAINS
    WOLFSSL_X509_CHAIN altChain;          /* peer alt cert chain, static */
    #endif
#endif
#ifdef HAVE_EX_DATA
    WOLFSSL_CRYPTO_EX_DATA ex_data;
#endif
    byte               isSetup:1;
};

#if defined(WOLFSSL_TLS13) && defined(HAVE_SESSION_TICKET) &&                  \
        defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
WOLFSSL_LOCAL int SessionTicketNoncePopulate(WOLFSSL_SESSION *session,
    const byte* nonce, byte len);
#endif /* WOLFSSL_TLS13 &&  */

WOLFSSL_LOCAL int wolfSSL_RAND_Init(void);

WOLFSSL_LOCAL WOLFSSL_SESSION* wolfSSL_NewSession(void* heap);
WOLFSSL_LOCAL WOLFSSL_SESSION* wolfSSL_GetSession(
    WOLFSSL* ssl, byte* masterSecret, byte restoreSessionCerts);
WOLFSSL_LOCAL void SetupSession(WOLFSSL* ssl);
WOLFSSL_LOCAL void AddSession(WOLFSSL* ssl);
/* use wolfSSL_API visibility to be able to test in tests/api.c */
WOLFSSL_API int AddSessionToCache(WOLFSSL_CTX* ctx,
    WOLFSSL_SESSION* addSession, const byte* id, byte idSz, int* sessionIndex,
    int side, word16 useTicket, ClientSession** clientCacheEntry);
#ifndef NO_CLIENT_CACHE
WOLFSSL_LOCAL ClientSession* AddSessionToClientCache(int side, int row, int idx,
                      byte* serverID, word16 idLen, const byte* sessionID,
                      word16 useTicket);
#endif
WOLFSSL_LOCAL
WOLFSSL_SESSION* ClientSessionToSession(const WOLFSSL_SESSION* session);
WOLFSSL_LOCAL void TlsSessionCacheUnlockRow(word32 row);
WOLFSSL_LOCAL int TlsSessionCacheGetAndRdLock(const byte *id,
    const WOLFSSL_SESSION **sess, word32 *lockedRow, byte side);
WOLFSSL_LOCAL int TlsSessionCacheGetAndWrLock(const byte *id,
    WOLFSSL_SESSION **sess, word32 *lockedRow, byte side);
WOLFSSL_LOCAL void EvictSessionFromCache(WOLFSSL_SESSION* session);
/* WOLFSSL_API to test it in tests/api.c */
WOLFSSL_API int wolfSSL_GetSessionFromCache(WOLFSSL* ssl, WOLFSSL_SESSION* output);
WOLFSSL_LOCAL int wolfSSL_SetSession(WOLFSSL* ssl, WOLFSSL_SESSION* session);
WOLFSSL_LOCAL void wolfSSL_FreeSession(WOLFSSL_CTX* ctx,
        WOLFSSL_SESSION* session);
WOLFSSL_LOCAL int wolfSSL_DupSession(const WOLFSSL_SESSION* input,
        WOLFSSL_SESSION* output, int avoidSysCalls);


typedef int (*hmacfp) (WOLFSSL*, byte*, const byte*, word32, int, int, int, int);

#ifndef NO_CLIENT_CACHE
    WOLFSSL_LOCAL WOLFSSL_SESSION* wolfSSL_GetSessionClient(
        WOLFSSL* ssl, const byte* id, int len);
#endif

/* client connect state for nonblocking restart */
enum ConnectState {
    CONNECT_BEGIN = 0,
    CLIENT_HELLO_SENT,
    HELLO_AGAIN,               /* HELLO_AGAIN s for DTLS case */
    HELLO_AGAIN_REPLY,
    FIRST_REPLY_DONE,
    FIRST_REPLY_FIRST,
    FIRST_REPLY_SECOND,
    FIRST_REPLY_THIRD,
    FIRST_REPLY_FOURTH,
    FINISHED_DONE,
    SECOND_REPLY_DONE,

#ifdef WOLFSSL_DTLS13
    WAIT_FINISHED_ACK
#endif /* WOLFSSL_DTLS13 */

};


/* server accept state for nonblocking restart */
enum AcceptState {
    ACCEPT_BEGIN = 0,
    ACCEPT_BEGIN_RENEG,
    ACCEPT_CLIENT_HELLO_DONE,
    ACCEPT_HELLO_RETRY_REQUEST_DONE,
    ACCEPT_FIRST_REPLY_DONE,
    SERVER_HELLO_SENT,
    CERT_SENT,
    CERT_VERIFY_SENT,
    CERT_STATUS_SENT,
    KEY_EXCHANGE_SENT,
    CERT_REQ_SENT,
    SERVER_HELLO_DONE,
    ACCEPT_SECOND_REPLY_DONE,
    TICKET_SENT,
    CHANGE_CIPHER_SENT,
    ACCEPT_FINISHED_DONE,
    ACCEPT_THIRD_REPLY_DONE
};

/* TLS 1.3 server accept state for nonblocking restart */
enum AcceptStateTls13 {
    TLS13_ACCEPT_BEGIN = 0,
    TLS13_ACCEPT_BEGIN_RENEG,
    TLS13_ACCEPT_CLIENT_HELLO_DONE,
    TLS13_ACCEPT_HELLO_RETRY_REQUEST_DONE,
    TLS13_ACCEPT_FIRST_REPLY_DONE,
    TLS13_ACCEPT_SECOND_REPLY_DONE,
    TLS13_SERVER_HELLO_SENT,
    TLS13_ACCEPT_THIRD_REPLY_DONE,
    TLS13_SERVER_EXTENSIONS_SENT,
    TLS13_CERT_REQ_SENT,
    TLS13_CERT_SENT,
    TLS13_CERT_VERIFY_SENT,
    TLS13_ACCEPT_FINISHED_SENT,
    TLS13_PRE_TICKET_SENT,
    TLS13_ACCEPT_FINISHED_DONE,
    TLS13_TICKET_SENT
};

/* buffers for struct WOLFSSL */
typedef struct Buffers {
    bufferStatic    inputBuffer;
    bufferStatic    outputBuffer;
    buffer          domainName;            /* for client check */
    buffer          clearOutputBuffer;
    buffer          sig;                   /* signature data */
    buffer          digest;                /* digest data */
    int             prevSent;              /* previous plain text bytes sent
                                              when got WANT_WRITE            */
    int             plainSz;               /* plain text bytes in buffer to send
                                              when got WANT_WRITE            */
    byte            weOwnCert;             /* SSL own cert flag */
    byte            weOwnCertChain;        /* SSL own cert chain flag */
    byte            weOwnKey;              /* SSL own key  flag */
    byte            weOwnDH;               /* SSL own dh (p,g)  flag */
#ifndef NO_DH
    buffer          serverDH_P;            /* WOLFSSL_CTX owns, unless we own */
    buffer          serverDH_G;            /* WOLFSSL_CTX owns, unless we own */
    buffer          serverDH_Pub;
    buffer          serverDH_Priv;
    DhKey*          serverDH_Key;
#endif
#ifndef NO_CERTS
    DerBuffer*      certificate;           /* WOLFSSL_CTX owns, unless we own */
    DerBuffer*      key;                   /* WOLFSSL_CTX owns, unless we own */
    byte            keyType:6;             /* Type of key: RSA, ECC, Ed25519 */
    byte            keyId:1;               /* Key data is an id not data */
    byte            keyLabel:1;            /* Key data is a label not data */
    int             keySz;                 /* Size of RSA key */
    int             keyDevId;              /* Device Id for key */
    DerBuffer*      certChain;             /* WOLFSSL_CTX owns, unless we own */
                 /* chain after self, in DER, with leading size for each cert */
#ifdef WOLFSSL_TLS13
    int             certChainCnt;
    DerBuffer*      certExts;
#endif
#endif
#ifdef WOLFSSL_SEND_HRR_COOKIE
    buffer          tls13CookieSecret;     /* HRR cookie secret */
#endif
#ifdef WOLFSSL_DTLS
    WOLFSSL_DTLS_CTX dtlsCtx;              /* DTLS connection context */
    #ifndef NO_WOLFSSL_SERVER
        buffer       dtlsCookieSecret;     /* DTLS cookie secret */
    #endif /* NO_WOLFSSL_SERVER */
#endif
#ifdef HAVE_PK_CALLBACKS
    #ifdef HAVE_ECC
        buffer peerEccDsaKey;              /* we own for Ecc Verify Callbacks */
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        buffer peerEd25519Key;             /* for Ed25519 Verify Callbacks */
    #endif /* HAVE_ED25519 */
    #ifdef HAVE_ED448
        buffer peerEd448Key;             /* for Ed448 Verify Callbacks */
    #endif /* HAVE_ED448 */
    #ifndef NO_RSA
        buffer peerRsaKey;                 /* we own for Rsa Verify Callbacks */
    #endif /* NO_RSA */
#endif /* HAVE_PK_CALLBACKS */
} Buffers;

/* sub-states for send/do key share (key exchange) */
enum asyncState {
    TLS_ASYNC_BEGIN = 0,
    TLS_ASYNC_BUILD,
    TLS_ASYNC_DO,
    TLS_ASYNC_VERIFY,
    TLS_ASYNC_FINALIZE,
    TLS_ASYNC_END
};

/* sub-states for build message */
enum buildMsgState {
    BUILD_MSG_BEGIN = 0,
    BUILD_MSG_SIZE,
    BUILD_MSG_HASH,
    BUILD_MSG_VERIFY_MAC,
    BUILD_MSG_ENCRYPT,
    BUILD_MSG_ENCRYPTED_VERIFY_MAC,
};

/* sub-states for cipher operations */
enum cipherState {
    CIPHER_STATE_BEGIN = 0,
    CIPHER_STATE_DO,
    CIPHER_STATE_END,
};

struct Options {
#ifndef NO_PSK
    wc_psk_client_callback client_psk_cb;
    wc_psk_server_callback server_psk_cb;
#ifdef OPENSSL_EXTRA
    wc_psk_use_session_cb_func session_psk_cb;
#endif
#ifdef WOLFSSL_TLS13
    wc_psk_client_cs_callback    client_psk_cs_cb;     /* client callback */
    wc_psk_client_tls13_callback client_psk_tls13_cb;  /* client callback */
    wc_psk_server_tls13_callback server_psk_tls13_cb;  /* server callback */
#endif
    void*             psk_ctx;
#endif /* NO_PSK */
    unsigned long     mask; /* store SSL_OP_ flags */
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || defined(WOLFSSL_WPAS_SMALL)
    word16            minProto:1; /* sets min to min available */
    word16            maxProto:1; /* sets max to max available */
#endif
#if defined(HAVE_SESSION_TICKET) && defined(WOLFSSL_TLS13)
    unsigned int      maxTicketTls13;  /* maximum number of tickets to send */
    unsigned int      ticketsSent;     /* keep track of the total sent */
#endif

    /* on/off or small bit flags, optimize layout */
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    word16            havePSK:1;            /* psk key set by user */
#endif /* HAVE_SESSION_TICKET || !NO_PSK */
    word16            sendVerify:2;     /* false = 0, true = 1, sendBlank = 2 */
    word16            sessionCacheOff:1;
    word16            sessionCacheFlushOff:1;
#ifdef HAVE_EXT_CACHE
    word16            internalCacheOff:1;
    word16            internalCacheLookupOff:1;
#endif
    word16            side:2;             /* client, server or neither end */
    word16            verifyPeer:1;
    word16            verifyNone:1;
    word16            failNoCert:1;
    word16            failNoCertxPSK:1;   /* fail for no cert except with PSK */
    word16            downgrade:1;        /* allow downgrade of versions */
    word16            resuming:1;
#ifdef HAVE_SECURE_RENEGOTIATION
    word16            resumed:1;          /* resuming may be reset on SCR */
#endif
    word16            isPSK:1;
    word16            haveSessionId:1;    /* server may not send */
    word16            tls:1;              /* using TLS ? */
    word16            tls1_1:1;           /* using TLSv1.1+ ? */
    word16            tls1_3:1;           /* using TLSv1.3+ ? */
    word16            dtls:1;             /* using datagrams ? */
    word16            dtlsStateful:1;     /* allow stateful processing ? */
    word16            connReset:1;        /* has the peer reset */
    word16            isClosed:1;         /* if we consider conn closed */
    word16            closeNotify:1;      /* we've received a close notify */
    word16            sentNotify:1;       /* we've sent a close notify */
    word16            shutdownDone:1;     /* we've completed a shutdown */
    word16            usingCompression:1; /* are we using compression */
    word16            haveRSA:1;          /* RSA available */
    word16            haveECC:1;          /* ECC available */
    word16            haveDH:1;           /* server DH parms set by user */
    word16            haveECDSAsig:1;     /* server ECDSA signed cert */
    word16            haveStaticECC:1;    /* static server ECC private key */
    word16            haveFalconSig:1;    /* server Falcon signed cert */
    word16            haveDilithiumSig:1; /* server Dilithium signed cert */
    word16            havePeerCert:1;     /* do we have peer's cert */
    word16            havePeerVerify:1;   /* and peer's cert verify */
    word16            usingPSK_cipher:1;  /* are using psk as cipher */
    word16            usingAnon_cipher:1; /* are we using an anon cipher */
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    word16            noPskDheKe:1;       /* Don't use (EC)DHE with PSK */
#ifdef HAVE_SUPPORTED_CURVES
    word16            onlyPskDheKe:1;     /* Only use (EC)DHE with PSK */
#endif
#endif
    word16            partialWrite:1;     /* only one msg per write call */
    word16            quietShutdown:1;    /* don't send close notify */
    word16            certOnly:1;         /* stop once we get cert */
    word16            groupMessages:1;    /* group handshake messages */
    word16            saveArrays:1;       /* save array Memory for user get keys
                                           or psk */
    word16            weOwnRng:1;         /* will be true unless CTX owns */
    word16            dontFreeDigest:1;   /* when true, we used SetDigest */
    word16            haveEMS:1;          /* using extended master secret */
#ifdef HAVE_POLY1305
    word16            oldPoly:1;        /* set when to use old rfc way of poly*/
#endif
    word16            haveAnon:1;       /* User wants to allow Anon suites */
#ifdef HAVE_SESSION_TICKET
    word16            createTicket:1;     /* Server to create new Ticket */
    word16            useTicket:1;        /* Use Ticket not session cache */
    word16            rejectTicket:1;     /* Callback rejected ticket */
    word16            noTicketTls12:1;    /* TLS 1.2 server won't send ticket */
#ifdef WOLFSSL_TLS13
    word16            noTicketTls13:1;    /* Server won't create new Ticket */
#endif
#endif
#ifdef WOLFSSL_DTLS
#ifdef HAVE_SECURE_RENEGOTIATION
    word16            dtlsDoSCR:1;        /* Enough packets were dropped. We
                                           * need to re-key. */
#endif
    word16            dtlsUseNonblock:1;  /* are we using nonblocking socket */
    word16            dtlsHsRetain:1;     /* DTLS retaining HS data */
    word16            haveMcast:1;        /* using multicast ? */
#ifdef WOLFSSL_SCTP
    word16            dtlsSctp:1;         /* DTLS-over-SCTP mode */
#endif
#endif /* WOLFSSL_DTLS */
#if defined(HAVE_TLS_EXTENSIONS) && defined(HAVE_SUPPORTED_CURVES)
    word16            userCurves:1;       /* indicates user called wolfSSL_UseSupportedCurve */
#endif
    word16            keepResources:1;    /* Keep resources after handshake */
    word16            useClientOrder:1;   /* Use client's cipher order */
    word16            mutualAuth:1;       /* Mutual authentication is required */
    word16            peerAuthGood:1;     /* Any required peer auth done */
#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
    word16            pskNegotiated:1;    /* Session Ticket/PSK negotiated. */
#endif
#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
    word16            postHandshakeAuth:1;/* Client send post_handshake_auth
                                           * extension */
    word16            verifyPostHandshake:1; /* Only send client cert req post
                                              * handshake, not also during */
#endif
#if defined(WOLFSSL_TLS13) && !defined(NO_WOLFSSL_SERVER)
    word16            sendCookie:1;       /* Server creates a Cookie in HRR */
#endif
#ifdef WOLFSSL_ALT_CERT_CHAINS
    word16            usingAltCertChain:1;/* Alternate cert chain was used */
#endif
#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_TLS13_MIDDLEBOX_COMPAT)
    word16            sentChangeCipher:1; /* Change Cipher Spec sent */
#endif
#if !defined(WOLFSSL_NO_CLIENT_AUTH) && \
               ((defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)) || \
                (defined(HAVE_ED25519) && !defined(NO_ED25519_CLIENT_AUTH)) || \
                (defined(HAVE_ED448) && !defined(NO_ED448_CLIENT_AUTH)))
    word16            cacheMessages:1;    /* Cache messages for sign/verify */
#endif
#ifndef NO_DH
    #if !defined(WOLFSSL_OLD_PRIME_CHECK) && \
        !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        word16        dhDoKeyTest:1;      /* Need to do the DH Key prime test */
        word16        dhKeyTested:1;      /* Set when key has been tested. */
    #endif
#endif
#ifdef HAVE_ENCRYPT_THEN_MAC
    word16            disallowEncThenMac:1;   /* Don't do Encrypt-Then-MAC */
    word16            encThenMac:1;           /* Doing Encrypt-Then-MAC */
    word16            startedETMRead:1;       /* Doing Encrypt-Then-MAC read */
    word16            startedETMWrite:1;      /* Doing Encrypt-Then-MAC write */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    word16            buildArgsSet:1;         /* buildArgs are set and need to
                                               * be free'd */
#endif
    word16            buildingMsg:1;      /* If set then we need to re-enter the
                                           * handshake logic. */
#ifdef WOLFSSL_DTLS13
    word16            dtls13SendMoreAcks:1;  /* Send more acks during the
                                              * handshake process */
#endif
#ifdef WOLFSSL_TLS13
    word16            tls13MiddleBoxCompat:1; /* TLSv1.3 middlebox compatibility */
#endif
#ifdef WOLFSSL_DTLS_CID
    word16            useDtlsCID:1;
#endif /* WOLFSSL_DTLS_CID */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    word16            useEch:1;
#endif
#ifdef WOLFSSL_SEND_HRR_COOKIE
    word16            cookieGood:1;
#endif

    /* need full byte values for this section */
    byte            processReply;           /* nonblocking resume */
    byte            cipherSuite0;           /* first byte, normally 0 */
    byte            cipherSuite;            /* second byte, actual suite */
    byte            hashAlgo;               /* selected hash algorithm */
    byte            sigAlgo;                /* selected sig algorithm */
    byte            serverState;
    byte            clientState;
    byte            handShakeState;
    byte            handShakeDone;      /* at least one handshake complete */
    byte            minDowngrade;       /* minimum downgrade version */
    byte            connectState;       /* nonblocking resume */
    byte            acceptState;        /* nonblocking resume */
    byte            asyncState;         /* sub-state for enum asyncState */
    byte            buildMsgState;      /* sub-state for enum buildMsgState */
    byte            alertCount;         /* detect warning dos attempt */
#ifdef WOLFSSL_MULTICAST
    word16          mcastID;            /* Multicast group ID */
#endif
#ifndef NO_DH
    word16          minDhKeySz;         /* minimum DH key size */
    word16          maxDhKeySz;         /* minimum DH key size */
    word16          dhKeySz;            /* actual DH key size */
#endif
#ifndef NO_RSA
    short           minRsaKeySz;      /* minimum RSA key size */
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    short           minEccKeySz;      /* minimum ECC key size */
#endif
#if defined(HAVE_PQC)
    short           minFalconKeySz;   /* minimum Falcon key size */
    short           minDilithiumKeySz;/* minimum Dilithium key size */
#endif
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    byte            verifyDepth;      /* maximum verification depth */
#endif
#ifdef WOLFSSL_EARLY_DATA
    word16          pskIdIndex;
    word32          maxEarlyDataSz;
#endif
#ifdef WOLFSSL_TLS13
    byte            oldMinor;          /* client preferred version < TLS 1.3 */
#endif
};

typedef struct Arrays {
    byte*           pendingMsg;         /* defrag buffer */
    byte*           preMasterSecret;
    word32          preMasterSz;        /* differs for DH, actual size */
    word32          pendingMsgSz;       /* defrag buffer size */
    word32          pendingMsgOffset;   /* current offset into defrag buffer */
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    word32          psk_keySz;          /* actual size */
    char            client_identity[MAX_PSK_ID_LEN + NULL_TERM_LEN];
    char            server_hint[MAX_PSK_ID_LEN + NULL_TERM_LEN];
    byte            psk_key[MAX_PSK_KEY_LEN];
#endif
    byte            clientRandom[RAN_LEN];
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    byte            clientRandomInner[RAN_LEN];
#endif
    byte            serverRandom[RAN_LEN];
    byte            sessionID[ID_LEN];
    byte            sessionIDSz;
#ifdef WOLFSSL_TLS13
    byte            secret[SECRET_LEN];
#endif
#ifdef HAVE_KEYING_MATERIAL
    byte            exporterSecret[WC_MAX_DIGEST_SIZE];
#endif
    byte            masterSecret[SECRET_LEN];
#if defined(WOLFSSL_RENESAS_TSIP_TLS) && \
   !defined(NO_WOLFSSL_RENESAS_TSIP_TLS_SESSION)
    byte            tsip_masterSecret[TSIP_TLS_MASTERSECRET_SIZE];
#endif
#if defined(WOLFSSL_RENESAS_SCEPROTECT)
    byte            sce_masterSecret[SCE_TLS_MASTERSECRET_SIZE];
#endif
#ifdef WOLFSSL_DTLS
    byte            cookie[MAX_COOKIE_LEN];
    byte            cookieSz;
#endif
    byte            pendingMsgType;    /* defrag buffer message type */
} Arrays;

#ifndef ASN_NAME_MAX
    #ifndef NO_ASN
        /* use value from asn.h */
        #define ASN_NAME_MAX WC_ASN_NAME_MAX
    #else
        /* calculate for WOLFSSL_X509 */
        #if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
            defined(WOLFSSL_CERT_EXT)
            #define ASN_NAME_MAX 330
        #else
            #define ASN_NAME_MAX 256
        #endif
    #endif
#endif

#ifndef MAX_DATE_SZ
#define MAX_DATE_SZ 32
#endif

typedef enum {
    STACK_TYPE_X509               = 0,
    STACK_TYPE_GEN_NAME           = 1,
    STACK_TYPE_BIO                = 2,
    STACK_TYPE_OBJ                = 3,
    STACK_TYPE_STRING             = 4,
    STACK_TYPE_CIPHER             = 5,
    STACK_TYPE_ACCESS_DESCRIPTION = 6,
    STACK_TYPE_X509_EXT           = 7,
    STACK_TYPE_NULL               = 8,
    STACK_TYPE_X509_NAME          = 9,
    STACK_TYPE_CONF_VALUE         = 10,
    STACK_TYPE_X509_INFO          = 11,
    STACK_TYPE_BY_DIR_entry       = 12,
    STACK_TYPE_BY_DIR_hash        = 13,
    STACK_TYPE_X509_OBJ           = 14,
    STACK_TYPE_DIST_POINT         = 15,
    STACK_TYPE_X509_CRL           = 16,
    STACK_TYPE_X509_NAME_ENTRY    = 17,
    STACK_TYPE_X509_REQ_ATTR      = 18,
} WOLF_STACK_TYPE;

struct WOLFSSL_STACK {
    unsigned long num; /* number of nodes in stack
                        * (safety measure for freeing and shortcut for count) */
    #if defined(OPENSSL_ALL)
    wolf_sk_hash_cb hash_fn;
    unsigned long hash;
    #endif

    union {
        WOLFSSL_X509*          x509;
        WOLFSSL_X509_NAME*     name;
        WOLFSSL_X509_NAME_ENTRY* name_entry;
        WOLFSSL_X509_INFO*     info;
        WOLFSSL_BIO*           bio;
        WOLFSSL_ASN1_OBJECT*   obj;
        WOLFSSL_CIPHER         cipher;
        WOLFSSL_ACCESS_DESCRIPTION* access;
        WOLFSSL_X509_EXTENSION* ext;
#ifdef OPENSSL_EXTRA
        WOLFSSL_CONF_VALUE*    conf;
#endif
        void*                  generic;
        char*                  string;
        WOLFSSL_GENERAL_NAME*  gn;
        WOLFSSL_BY_DIR_entry*  dir_entry;
        WOLFSSL_BY_DIR_HASH*   dir_hash;
        WOLFSSL_X509_OBJECT*   x509_obj;
        WOLFSSL_DIST_POINT*    dp;
        WOLFSSL_X509_CRL*      crl;
    } data;
    void* heap; /* memory heap hint */
    WOLFSSL_STACK* next;
    WOLF_STACK_TYPE type;     /* Identifies type of stack. */
};

struct WOLFSSL_X509_NAME {
    char  *name;
    int   dynamicName;
    int   sz;
    char  staticName[ASN_NAME_MAX];
#if (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)) && \
    !defined(NO_ASN)
    DecodedName fullName;
    int   entrySz; /* number of entries */
    WOLFSSL_X509_NAME_ENTRY entry[MAX_NAME_ENTRIES]; /* all entries i.e. CN */
    WOLFSSL_X509*           x509;   /* x509 that struct belongs to */
#endif /* OPENSSL_EXTRA */
#ifndef WOLFSSL_NO_CA_NAMES
    byte  raw[ASN_NAME_MAX];
    int   rawLen;

    WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* entries;
#endif
    void* heap;
};

#ifndef EXTERNAL_SERIAL_SIZE
    #define EXTERNAL_SERIAL_SIZE 32
#endif

#ifdef NO_ASN
    typedef struct DNS_entry DNS_entry;
#endif

struct WOLFSSL_X509 {
    int              version;
    int              serialSz;
#ifdef WOLFSSL_SEP
    int              deviceTypeSz;
    int              hwTypeSz;
    byte             deviceType[EXTERNAL_SERIAL_SIZE];
    byte             hwType[EXTERNAL_SERIAL_SIZE];
    int              hwSerialNumSz;
    byte             hwSerialNum[EXTERNAL_SERIAL_SIZE];
#endif /* WOLFSSL_SEP */
#if (defined(WOLFSSL_SEP) || defined(WOLFSSL_QT) || defined(OPENSSL_ALL) || \
    defined (OPENSSL_EXTRA)) && \
    (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
    byte             certPolicySet;
    byte             certPolicyCrit;
#endif /* (WOLFSSL_SEP || WOLFSSL_QT) && (OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL) */
#if defined(WOLFSSL_QT) || defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA)
    WOLFSSL_STACK* ext_sk; /* Store X509_EXTENSIONS from wolfSSL_X509_get_ext */
    WOLFSSL_STACK* ext_sk_full; /* Store X509_EXTENSIONS from wolfSSL_X509_get0_extensions */
    WOLFSSL_STACK* ext_d2i;/* Store d2i extensions from wolfSSL_X509_get_ext_d2i */
#endif /* WOLFSSL_QT || OPENSSL_ALL */
#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
    WOLFSSL_ASN1_INTEGER* serialNumber; /* Stores SN from wolfSSL_X509_get_serialNumber */
#endif
    WOLFSSL_ASN1_TIME notBefore;
    WOLFSSL_ASN1_TIME notAfter;
    buffer           sig;
    int              sigOID;
    DNS_entry*       altNames;                       /* alt names list */
    buffer           pubKey;
    int              pubKeyOID;
    DNS_entry*       altNamesNext;                   /* hint for retrieval */
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448) || \
    defined(HAVE_PQC)
    word32       pkCurveOID;
#endif /* HAVE_ECC || HAVE_PQC */
#ifndef NO_CERTS
    DerBuffer*   derCert;                            /* may need  */
#endif
    void*            heap;                           /* heap hint */
    byte             dynamicMemory;                  /* dynamic memory flag */
    byte             isCa:1;
#ifdef WOLFSSL_CERT_EXT
    char             certPolicies[MAX_CERTPOL_NB][MAX_CERTPOL_SZ];
    int              certPoliciesNb;
#endif /* WOLFSSL_CERT_EXT */
#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA)
    wolfSSL_Ref      ref;
#endif
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
#ifdef HAVE_EX_DATA
    WOLFSSL_CRYPTO_EX_DATA ex_data;
#endif
    byte*            authKeyId; /* Points into authKeyIdSrc */
    byte*            authKeyIdSrc;
    byte*            subjKeyId;
    byte*            extKeyUsageSrc;
#ifdef OPENSSL_ALL
    byte*            subjAltNameSrc;
#endif
    byte*            rawCRLInfo;
    byte*            CRLInfo;
    byte*            authInfo;
#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || defined(WOLFSSL_QT)
    byte*            authInfoCaIssuer;
    int              authInfoCaIssuerSz;
#endif
    word32           pathLength;
    word16           keyUsage;
    int              rawCRLInfoSz;
    int              CRLInfoSz;
    int              authInfoSz;
    word32           authKeyIdSz;
    word32           authKeyIdSrcSz;
    word32           subjKeyIdSz;
    byte             extKeyUsage;
    word32           extKeyUsageSz;
    word32           extKeyUsageCount;
#ifndef IGNORE_NETSCAPE_CERT_TYPE
    byte             nsCertType;
#endif
#ifdef OPENSSL_ALL
    word32           subjAltNameSz;
#endif

    byte             CRLdistSet:1;
    byte             CRLdistCrit:1;
    byte             authInfoSet:1;
    byte             authInfoCrit:1;
    byte             keyUsageSet:1;
    byte             keyUsageCrit:1;
    byte             extKeyUsageCrit:1;
    byte             subjKeyIdSet:1;

    byte             subjKeyIdCrit:1;
    byte             basicConstSet:1;
    byte             basicConstCrit:1;
    byte             basicConstPlSet:1;
    byte             subjAltNameSet:1;
    byte             subjAltNameCrit:1;
    byte             authKeyIdSet:1;
    byte             authKeyIdCrit:1;
    byte             issuerSet:1;
#ifdef WOLFSSL_CUSTOM_OID
    CertExtension    custom_exts[NUM_CUSTOM_EXT];
    int              customExtCount;
#endif /* WOLFSSL_CUSTOM_OID */
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */
#ifdef WOLFSSL_CERT_REQ
    byte             isCSR:1;
#endif
    byte             serial[EXTERNAL_SERIAL_SIZE];
    char             subjectCN[ASN_NAME_MAX];        /* common name short cut */
#if defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_GEN)
#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA)
    /* stack of CSR attributes */
    WOLF_STACK_OF(WOLFSSL_X509_ATRIBUTE)* reqAttributes;
#endif
    #if defined(WOLFSSL_CERT_REQ)
    char             challengePw[CTC_NAME_SIZE]; /* for REQ certs */
    char             contentType[CTC_NAME_SIZE];
    #endif
#endif /* WOLFSSL_CERT_REQ || WOLFSSL_CERT_GEN */
    WOLFSSL_X509_NAME issuer;
    WOLFSSL_X509_NAME subject;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_WPAS)
    WOLFSSL_X509_ALGOR algor;
    WOLFSSL_X509_PUBKEY key;
#endif
#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_ALL) || defined(KEEP_OUR_CERT) || \
    defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)
    byte            notBeforeData[CTC_DATE_SIZE];
    byte            notAfterData[CTC_DATE_SIZE];
#endif
};


/* record layer header for PlainText, Compressed, and CipherText */
typedef struct RecordLayerHeader {
    byte            type;
    byte            pvMajor;
    byte            pvMinor;
    byte            length[2];
} RecordLayerHeader;


/* record layer header for DTLS PlainText, Compressed, and CipherText */
typedef struct DtlsRecordLayerHeader {
    byte            type;
    byte            pvMajor;
    byte            pvMinor;
    byte            sequence_number[8];   /* per record */
    byte            length[2];
} DtlsRecordLayerHeader;

typedef struct DtlsFragBucket {
    /* m stands for meta */
    union {
        struct {
            struct DtlsFragBucket* next;
            word32 offset;
            word32 sz;
        } m;
        /* Make sure we have at least DTLS_HANDSHAKE_HEADER_SZ bytes before the
         * buf so that we can reconstruct the header in the allocated
         * DtlsFragBucket buffer. */
        byte padding[DTLS_HANDSHAKE_HEADER_SZ];
    } m;
/* Ignore "nonstandard extension used : zero-sized array in struct/union"
 * MSVC warning */
#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif
    byte buf[];
} DtlsFragBucket;

typedef struct DtlsMsg {
    struct DtlsMsg* next;
    byte*           raw;
    byte*           fullMsg;   /* for TX fullMsg == raw. For RX this points to
                                * the start of the message after headers. */
    DtlsFragBucket* fragBucketList;
    word32          bytesReceived;
    word16          epoch;     /* Epoch that this message belongs to */
    word32          seq;       /* Handshake sequence number    */
    word32          sz;        /* Length of whole message      */
    byte            type;
    byte            fragBucketListCount;
    byte            ready:1;
} DtlsMsg;


#ifdef HAVE_NETX

    /* NETX I/O Callback default */
    typedef struct NetX_Ctx {
        NX_TCP_SOCKET* nxSocket;    /* send/recv socket handle */
        NX_PACKET*     nxPacket;    /* incoming packet handle for short reads */
        ULONG          nxOffset;    /* offset already read from nxPacket */
        ULONG          nxWait;      /* wait option flag */
    } NetX_Ctx;

#endif

/* Handshake messages received from peer (plus change cipher */
typedef struct MsgsReceived {
    word16 got_hello_request:1;
    word16 got_client_hello:2;
    word16 got_server_hello:1;
    word16 got_hello_verify_request:1;
    word16 got_session_ticket:1;
    word16 got_end_of_early_data:1;
    word16 got_hello_retry_request:1;
    word16 got_encrypted_extensions:1;
    word16 got_certificate:1;
    word16 got_certificate_status:1;
    word16 got_server_key_exchange:1;
    word16 got_certificate_request:1;
    word16 got_server_hello_done:1;
    word16 got_certificate_verify:1;
    word16 got_client_key_exchange:1;
    word16 got_finished:1;
    word16 got_key_update:1;
    word16 got_change_cipher:1;
} MsgsReceived;


/* Handshake hashes */
typedef struct HS_Hashes {
    Hashes          verifyHashes;
    Hashes          certHashes;         /* for cert verify */
#ifndef NO_SHA
    wc_Sha          hashSha;            /* sha hash of handshake msgs */
#endif
#if !defined(NO_MD5) && !defined(NO_OLD_TLS)
    wc_Md5          hashMd5;            /* md5 hash of handshake msgs */
#endif
#ifndef NO_SHA256
    wc_Sha256       hashSha256;         /* sha256 hash of handshake msgs */
#endif
#ifdef WOLFSSL_SHA384
    wc_Sha384       hashSha384;         /* sha384 hash of handshake msgs */
#endif
#ifdef WOLFSSL_SHA512
    wc_Sha512       hashSha512;         /* sha512 hash of handshake msgs */
#endif
#ifdef WOLFSSL_SM3
    wc_Sm3          hashSm3;            /* sm3 hash of handshake msgs */
#endif
#if (defined(HAVE_ED25519) || defined(HAVE_ED448) || \
     (defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3))) && \
    !defined(WOLFSSL_NO_CLIENT_AUTH)
    byte*           messages;           /* handshake messages */
    int             length;             /* length of handshake messages' data */
    int             prevLen;            /* length of messages but last */
#endif
} HS_Hashes;


#ifndef WOLFSSL_NO_TLS12
/* Persistable BuildMessage arguments */
typedef struct BuildMsgArgs {
    word32 digestSz;
    word32 sz;
    word32 pad;
    word32 idx;
    word32 headerSz;
    word16 size;
    word32 ivSz;      /* TLSv1.1  IV */
    byte*  iv;
    ALIGN16 byte staticIvBuffer[MAX_IV_SZ];
} BuildMsgArgs;
#endif

#ifdef WOLFSSL_ASYNC_IO
    #define MAX_ASYNC_ARGS 18
    typedef void (*FreeArgsCb)(struct WOLFSSL* ssl, void* pArgs);

    struct WOLFSSL_ASYNC {
#if defined(WOLFSSL_ASYNC_CRYPT) && !defined(WOLFSSL_NO_TLS12)
        BuildMsgArgs  buildArgs; /* holder for current BuildMessage args */
#endif
        FreeArgsCb    freeArgs; /* function pointer to cleanup args */
        word32        args[MAX_ASYNC_ARGS]; /* holder for current args */
    };
#endif

#ifdef HAVE_WRITE_DUP

    #define WRITE_DUP_SIDE 1
    #define READ_DUP_SIDE 2

    typedef struct WriteDup {
        wolfSSL_Mutex   dupMutex;       /* reference count mutex */
        int             dupCount;       /* reference count */
        int             dupErr;         /* under dupMutex, pass to other side */
    } WriteDup;

    WOLFSSL_LOCAL void FreeWriteDup(WOLFSSL* ssl);
    WOLFSSL_LOCAL int  NotifyWriteSide(WOLFSSL* ssl, int err);
#endif /* HAVE_WRITE_DUP */

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
typedef struct CertReqCtx CertReqCtx;

struct CertReqCtx {
    CertReqCtx* next;
    byte        len;
    byte        ctx;
};
#endif

#ifdef WOLFSSL_EARLY_DATA
typedef enum EarlyDataState {
    no_early_data,
    early_data_ext,
    expecting_early_data,
    process_early_data,
    done_early_data
} EarlyDataState;
#endif

#ifdef WOLFSSL_DTLS13

/* size of the mask used to encrypt/decrypt Record Number  */
#define DTLS13_RN_MASK_SIZE 16

typedef struct Dtls13UnifiedHdrInfo {
    word16 recordLength;
    byte seqLo;
    byte seqHi;
    byte seqHiPresent:1;
    byte epochBits;
} Dtls13UnifiedHdrInfo;

enum  {
    DTLS13_EPOCH_EARLYDATA = 1,
    DTLS13_EPOCH_HANDSHAKE = 2,
    DTLS13_EPOCH_TRAFFIC0 = 3
};

typedef struct Dtls13Epoch {
    w64wrapper epochNumber;

    w64wrapper nextSeqNumber;
    w64wrapper nextPeerSeqNumber;

#ifndef WOLFSSL_TLS13_IGNORE_AEAD_LIMITS
    w64wrapper dropCount; /* Amount of records that failed decryption */
#endif

    word32 window[WOLFSSL_DTLS_WINDOW_WORDS];

    /* key material for the epoch */
    byte client_write_key[MAX_SYM_KEY_SIZE];
    byte server_write_key[MAX_SYM_KEY_SIZE];
    byte client_write_IV[MAX_WRITE_IV_SZ];
    byte server_write_IV[MAX_WRITE_IV_SZ];

    byte aead_exp_IV[AEAD_MAX_EXP_SZ];
    byte aead_enc_imp_IV[AEAD_MAX_IMP_SZ];
    byte aead_dec_imp_IV[AEAD_MAX_IMP_SZ];

    byte client_sn_key[MAX_SYM_KEY_SIZE];
    byte server_sn_key[MAX_SYM_KEY_SIZE];

    byte isValid;
    byte side;
} Dtls13Epoch;

#ifndef DTLS13_EPOCH_SIZE
#define DTLS13_EPOCH_SIZE 4
#endif

#ifndef DTLS13_RETRANS_RN_SIZE
#define DTLS13_RETRANS_RN_SIZE 3
#endif

enum Dtls13RtxFsmState {
    DTLS13_RTX_FSM_PREPARING = 0,
    DTLS13_RTX_FSM_SENDING,
    DTLS13_RTX_FSM_WAITING,
    DTLS13_RTX_FSM_FINISHED
};

typedef struct Dtls13RtxRecord {
    struct Dtls13RtxRecord *next;
    word16 length;
    byte *data;
    w64wrapper epoch;
    w64wrapper seq[DTLS13_RETRANS_RN_SIZE];
    byte rnIdx;
    byte handshakeType;
} Dtls13RtxRecord;

typedef struct Dtls13RecordNumber {
    struct Dtls13RecordNumber *next;
    w64wrapper epoch;
    w64wrapper seq;
} Dtls13RecordNumber;

typedef struct Dtls13Rtx {
    enum Dtls13RtxFsmState state;
    Dtls13RtxRecord *rtxRecords;
    Dtls13RtxRecord **rtxRecordTailPtr;
    Dtls13RecordNumber *seenRecords;
    word32 lastRtx;
    byte triggeredRtxs;
    byte sendAcks:1;
    byte retransmit:1;
} Dtls13Rtx;

#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_DTLS_CID
typedef struct CIDInfo CIDInfo;
#endif /* WOLFSSL_DTLS_CID */

/* The idea is to reuse the context suites object whenever possible to save
 * space. */
#define WOLFSSL_SUITES(ssl) \
    ((const Suites*) ((ssl)->suites != NULL ? \
        (ssl)->suites : \
        (ssl)->ctx->suites))

/* wolfSSL ssl type */
struct WOLFSSL {
    WOLFSSL_CTX*    ctx;
#if defined(WOLFSSL_HAPROXY)
    WOLFSSL_CTX*    initial_ctx; /* preserve session key materials */
#endif
    Suites*         suites; /* Only need during handshake. Can be NULL when
                             * re-using the context's object. When WOLFSSL
                             * object needs separate instance of suites use
                             * AllocateSuites(). */
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    WOLF_STACK_OF(WOLFSSL_CIPHER)* suitesStack; /* stack of available cipher
                                                 * suites */
#endif
    Arrays*         arrays;
#ifdef WOLFSSL_TLS13
    byte            clientSecret[SECRET_LEN];
    byte            serverSecret[SECRET_LEN];
#endif
    HS_Hashes*      hsHashes;
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    HS_Hashes*      hsHashesEch;
#endif
    void*           IOCB_ReadCtx;
    void*           IOCB_WriteCtx;
    WC_RNG*         rng;
    void*           verifyCbCtx;        /* cert verify callback user ctx*/
    VerifyCallback  verifyCallback;     /* cert verification callback */
    void*           heap;               /* for user overrides */
#ifdef HAVE_WRITE_DUP
    WriteDup*       dupWrite;           /* valid pointer indicates ON */
             /* side that decrements dupCount to zero frees overall structure */
    byte            dupSide;            /* write side or read side */
#endif
#ifdef OPENSSL_EXTRA
    byte              cbioFlag;         /* WOLFSSL_CBIO_RECV/SEND:
                                         * CBIORecv/Send is set */
#endif
#ifdef WOLFSSL_WOLFSENTRY_HOOKS
    NetworkFilterCallback_t AcceptFilter;
    void *AcceptFilter_arg;
    NetworkFilterCallback_t ConnectFilter;
    void *ConnectFilter_arg;
#endif /* WOLFSSL_WOLFSENTRY_HOOKS */
    CallbackIORecv  CBIORecv;
    CallbackIOSend  CBIOSend;
#ifdef WOLFSSL_STATIC_MEMORY
    WOLFSSL_HEAP_HINT heap_hint;
#endif
#if defined(WOLFSSL_DTLS) && !defined(NO_WOLFSSL_SERVER)
    ClientHelloGoodCb chGoodCb;        /*  notify user we parsed a verified
                                        *  ClientHello */
    void*             chGoodCtx;       /*  user ClientHello cb context  */
#endif
#ifndef NO_HANDSHAKE_DONE_CB
    HandShakeDoneCb hsDoneCb;          /*  notify user handshake done */
    void*           hsDoneCtx;         /*  user handshake cb context  */
#endif
#ifdef WOLFSSL_ASYNC_IO
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev;
#endif
    /* Message building context should be stored here for functions that expect
     * to encounter encryption blocking or fragment the message. */
    struct WOLFSSL_ASYNC* async;
#endif
    void*           hsKey;              /* Handshake key (RsaKey or ecc_key)
                                         * allocated from heap */
    word32          hsType;             /* Type of Handshake key (hsKey) */
    WOLFSSL_CIPHER  cipher;
#ifndef WOLFSSL_AEAD_ONLY
    hmacfp          hmac;
#endif
    Ciphers         encrypt;
    Ciphers         decrypt;
    Buffers         buffers;
    WOLFSSL_SESSION* session;
#ifndef NO_CLIENT_CACHE
    ClientSession*  clientSession;
#endif
    WOLFSSL_ALERT_HISTORY alert_history;
    WOLFSSL_ALERT   pendingAlert;
    int             error;
    int             rfd;                /* read  file descriptor */
    int             wfd;                /* write file descriptor */
    int             rflags;             /* user read  flags */
    int             wflags;             /* user write flags */
    word32          timeout;            /* session timeout */
    word32          fragOffset;         /* fragment offset */
    word16          curSize;
    byte            verifyDepth;
    RecordLayerHeader curRL;
    MsgsReceived    msgsReceived;       /* peer messages received */
    ProtocolVersion version;            /* negotiated version */
    ProtocolVersion chVersion;          /* client hello version */
    CipherSpecs     specs;
    Keys            keys;
    Options         options;
#ifdef OPENSSL_EXTRA
    CallbackInfoState* CBIS;             /* used to get info about SSL state */
    int              cbmode;             /* read or write on info callback */
    int              cbtype;             /* event type in info callback */
    WOLFSSL_BIO*     biord;              /* socket bio read  to free/close */
    WOLFSSL_BIO*     biowr;              /* socket bio write to free/close */
    byte             sessionCtx[ID_LEN]; /* app session context ID */
    WOLFSSL_X509_VERIFY_PARAM* param;    /* verification parameters*/
#endif
#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
    word32            disabledCurves;   /* curves disabled by user */
#endif
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    unsigned long    peerVerifyRet;
#endif
#ifdef OPENSSL_EXTRA
    byte             readAhead;
    byte             sessionCtxSz;       /* size of sessionCtx stored */
#ifdef HAVE_PK_CALLBACKS
    void*            loggingCtx;         /* logging callback argument */
#endif
#endif /* OPENSSL_EXTRA */
#ifndef NO_RSA
    RsaKey*         peerRsaKey;
#if defined(WOLFSSL_RENESAS_TSIP_TLS) || defined(WOLFSSL_RENESAS_SCEPROTECT)
    void*           RenesasUserCtx;
    byte*           peerSceTsipEncRsaKeyIndex;
#endif
    byte            peerRsaKeyPresent;
#endif
#if defined(WOLFSSL_TLS13) || defined(HAVE_FFDHE)
    word16          namedGroup;
#endif
#ifdef WOLFSSL_TLS13
    word16          group[WOLFSSL_MAX_GROUP_COUNT];
    byte            numGroups;
#endif
    word16          pssAlgo;
#ifdef WOLFSSL_TLS13
    word16          certHashSigAlgoSz;  /* SigAlgoCert ext length in bytes */
    byte            certHashSigAlgo[WOLFSSL_MAX_SIGALGO]; /* cert sig/algo to
                                                           * offer */
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    int             eccVerifyRes;
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_CURVE25519) || \
    defined(HAVE_ED448) || defined(HAVE_CURVE448)
    word32          ecdhCurveOID;            /* curve Ecc_Sum     */
    ecc_key*        eccTempKey;              /* private ECDHE key */
    byte            eccTempKeyPresent;       /* also holds type */
    byte            peerEccKeyPresent;
#endif
#ifdef HAVE_ECC
    ecc_key*        peerEccKey;              /* peer's  ECDHE key */
    ecc_key*        peerEccDsaKey;           /* peer's  ECDSA key */
    word16          eccTempKeySz;            /* in octets 20 - 66 */
    byte            peerEccDsaKeyPresent;
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || \
    defined(HAVE_CURVE448) || defined(HAVE_ED448)
    word32          pkCurveOID;              /* curve Ecc_Sum     */
#endif
#ifdef HAVE_ED25519
    ed25519_key*    peerEd25519Key;
    byte            peerEd25519KeyPresent;
#endif
#ifdef HAVE_CURVE25519
    curve25519_key* peerX25519Key;
    byte            peerX25519KeyPresent;
#endif
#ifdef HAVE_ED448
    ed448_key*      peerEd448Key;
    byte            peerEd448KeyPresent;
#endif
#ifdef HAVE_CURVE448
    curve448_key*   peerX448Key;
    byte            peerX448KeyPresent;
#endif
#ifdef HAVE_PQC
    falcon_key*     peerFalconKey;
    byte            peerFalconKeyPresent;
    dilithium_key*  peerDilithiumKey;
    byte            peerDilithiumKeyPresent;
#endif
#ifdef HAVE_LIBZ
    z_stream        c_stream;           /* compression   stream */
    z_stream        d_stream;           /* decompression stream */
    byte            didStreamInit;      /* for stream init and end */
#endif
#ifdef WOLFSSL_DTLS
    int             dtls_timeout_init;  /* starting timeout value */
    int             dtls_timeout_max;   /* maximum timeout value */
    int             dtls_timeout;       /* current timeout value, changes */
#ifndef NO_ASN_TIME
    word32          dtls_start_timeout;
#endif /* !NO_ASN_TIME */
    word32          dtls_tx_msg_list_sz;
    word32          dtls_rx_msg_list_sz;
    DtlsMsg*        dtls_tx_msg_list;
    DtlsMsg*        dtls_tx_msg;
    DtlsMsg*        dtls_rx_msg_list;
    void*           IOCB_CookieCtx;     /* gen cookie ctx */
    word32          dtls_expected_rx;
#ifdef WOLFSSL_SESSION_EXPORT
    wc_dtls_export  dtls_export;        /* export function for session */
#endif
#if defined(WOLFSSL_SCTP) || defined(WOLFSSL_DTLS_MTU)
    word16          dtlsMtuSz;
#endif /* WOLFSSL_SCTP || WOLFSSL_DTLS_MTU */
#ifdef WOLFSSL_MULTICAST
    void*           mcastHwCbCtx;       /* Multicast highwater callback ctx */
#endif /* WOLFSSL_MULTICAST */
#ifdef WOLFSSL_DTLS_DROP_STATS
    word32 macDropCount;
    word32 replayDropCount;
#endif /* WOLFSSL_DTLS_DROP_STATS */
#ifdef WOLFSSL_SRTP
    word16         dtlsSrtpProfiles;   /* DTLS-with-SRTP profiles list
                                        * (selected profiles - up to 16) */
    word16         dtlsSrtpId;         /* DTLS-with-SRTP profile ID selected */
#endif
#ifdef WOLFSSL_DTLS13
    RecordNumberCiphers dtlsRecordNumberEncrypt;
    RecordNumberCiphers dtlsRecordNumberDecrypt;
    Dtls13Epoch dtls13Epochs[DTLS13_EPOCH_SIZE];
    Dtls13Epoch *dtls13EncryptEpoch;
    Dtls13Epoch *dtls13DecryptEpoch;
    w64wrapper dtls13Epoch;
    w64wrapper dtls13PeerEpoch;
    w64wrapper dtls13InvalidateBefore;
    byte dtls13CurRL[DTLS_RECVD_RL_HEADER_MAX_SZ];
    word16 dtls13CurRlLength;

    /* used to store the message if it needs to be fragmented */
    buffer dtls13FragmentsBuffer;
    byte dtls13SendingFragments:1;
    byte dtls13SendingAckOrRtx:1;
    byte dtls13FastTimeout:1;
    byte dtls13WaitKeyUpdateAck:1;
    byte dtls13DoKeyUpdate:1;
    word32 dtls13MessageLength;
    word32 dtls13FragOffset;
    byte dtls13FragHandshakeType;
    Dtls13Rtx dtls13Rtx;
    byte *dtls13ClientHello;
    word16 dtls13ClientHelloSz;

#endif /* WOLFSSL_DTLS13 */
#ifdef WOLFSSL_DTLS_CID
    CIDInfo *dtlsCidInfo;
#endif /* WOLFSSL_DTLS_CID */

#endif /* WOLFSSL_DTLS */
#ifdef WOLFSSL_CALLBACKS
    TimeoutInfo     timeoutInfo;        /* info saved during handshake */
    HandShakeInfo   handShakeInfo;      /* info saved during handshake */
#endif
#ifdef OPENSSL_EXTRA
    SSL_Msg_Cb      protoMsgCb;         /* inspect protocol message callback */
    void*           protoMsgCtx;        /* user set context with msg callback */
#endif
#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    byte            hsInfoOn;           /* track handshake info        */
    byte            toInfoOn;           /* track timeout   info        */
#endif
#ifdef HAVE_FUZZER
    CallbackFuzzer  fuzzerCb;           /* for testing with using fuzzer */
    void*           fuzzerCtx;          /* user defined pointer */
#endif
#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
    CertReqCtx*     certReqCtx;
#endif
#ifdef WOLFSSL_LOCAL_X509_STORE
    WOLFSSL_X509_STORE* x509_store_pt; /* take ownership of external store */
#endif
#ifdef KEEP_PEER_CERT
    /* TODO put this on the heap so we can properly use the
     * reference counter and not have to duplicate it. */
    WOLFSSL_X509     peerCert;           /* X509 peer cert */
#endif
#ifdef KEEP_OUR_CERT
    WOLFSSL_X509*    ourCert;            /* keep alive a X509 struct of cert.
                                            points to ctx if not owned (owned
                                            flag found in buffers.weOwnCert) */
#endif
    byte             keepCert;           /* keep certificate after handshake */
#ifdef HAVE_EX_DATA
    WOLFSSL_CRYPTO_EX_DATA ex_data; /* external data, for Fortress */
#endif
    int              devId;             /* async device id to use */
#ifdef HAVE_ONE_TIME_AUTH
    OneTimeAuth     auth;
#endif
#ifdef HAVE_TLS_EXTENSIONS
    TLSX* extensions;                  /* RFC 6066 TLS Extensions data */
    #ifdef HAVE_MAX_FRAGMENT
        word16 max_fragment;
    #endif
    #ifdef HAVE_TRUNCATED_HMAC
        byte truncated_hmac;
    #endif
    #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
        byte status_request;
    #endif
    #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
        byte status_request_v2;
    #endif
    #if defined(HAVE_SECURE_RENEGOTIATION) \
        || defined(HAVE_SERVER_RENEGOTIATION_INFO)
        int                  secure_rene_count;    /* how many times */
        SecureRenegotiation* secure_renegotiation; /* valid pointer indicates */
    #endif                                         /* user turned on */
    #ifdef HAVE_ALPN
        byte *alpn_peer_requested; /* the ALPN bytes requested by peer, sequence
                                    * of length byte + chars */
        word16 alpn_peer_requested_length; /* number of bytes total */
        #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)  || \
            defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_QUIC)
            CallbackALPNSelect alpnSelect;
            void*              alpnSelectArg;
        #endif
    #endif                         /* of accepted protocols */
    #if !defined(NO_WOLFSSL_CLIENT) && defined(HAVE_SESSION_TICKET)
        CallbackSessionTicket session_ticket_cb;
        void*                 session_ticket_ctx;
        byte                  expect_session_ticket;
    #endif
#endif /* HAVE_TLS_EXTENSIONS */
#ifdef HAVE_OCSP
        void*       ocspIOCtx;
        byte ocspProducedDate[MAX_DATE_SZ];
        int ocspProducedDateFormat;
    #ifdef OPENSSL_EXTRA
        byte*       ocspResp;
        int         ocspRespSz;
        #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
            char*   url;
        #endif
    #endif
#endif
#ifdef HAVE_NETX
    NetX_Ctx        nxCtx;             /* NetX IO Context */
#endif
#if defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP)
    void*           mnCtx;             /* mynewt mn_socket IO Context */
#endif /* defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP) */
#ifdef WOLFSSL_GNRC
    struct gnrc_wolfssl_ctx *gnrcCtx;  /* Riot-OS GNRC UDP/IP context */
#endif
#ifdef SESSION_INDEX
    int sessionIndex;                  /* Session's location in the cache. */
#endif
#ifdef ATOMIC_USER
    void*    MacEncryptCtx;    /* Atomic User Mac/Encrypt Callback Context */
    void*    DecryptVerifyCtx; /* Atomic User Decrypt/Verify Callback Context */
    #ifdef HAVE_ENCRYPT_THEN_MAC
        void*    EncryptMacCtx;    /* Atomic User Encrypt/Mac Callback Ctx */
        void*    VerifyDecryptCtx; /* Atomic User Verify/Decrypt Callback Ctx */
    #endif
#endif
#ifdef HAVE_PK_CALLBACKS
    #ifdef HAVE_ECC
        void* EccKeyGenCtx;          /* EccKeyGen  Callback Context */
        void* EccSignCtx;            /* Ecc Sign   Callback Context */
        void* EccVerifyCtx;          /* Ecc Verify Callback Context */
        void* EccSharedSecretCtx;    /* Ecc Pms    Callback Context */
    #endif /* HAVE_ECC */
    #ifdef HAVE_HKDF
        void* HkdfExtractCtx;       /* Hkdf extract callback context */
    #endif
    #ifdef HAVE_ED25519
        void* Ed25519SignCtx;        /* ED25519 Sign   Callback Context */
        void* Ed25519VerifyCtx;      /* ED25519 Verify Callback Context */
    #endif
    #ifdef HAVE_CURVE25519
        void* X25519KeyGenCtx;       /* X25519 KeyGen Callback Context */
        void* X25519SharedSecretCtx; /* X25519 Pms    Callback Context */
    #endif
    #ifdef HAVE_ED448
        void* Ed448SignCtx;          /* ED448 Sign   Callback Context */
        void* Ed448VerifyCtx;        /* ED448 Verify Callback Context */
    #endif
    #ifdef HAVE_CURVE448
        void* X448KeyGenCtx;         /* X448 KeyGen Callback Context */
        void* X448SharedSecretCtx;   /* X448 Pms    Callback Context */
    #endif
    #ifndef NO_DH
        void* DhAgreeCtx; /* DH Pms Callback Context */
    #endif /* !NO_DH */
    #ifndef NO_RSA
        void* RsaSignCtx;     /* Rsa Sign   Callback Context */
        void* RsaVerifyCtx;   /* Rsa Verify Callback Context */
        #ifdef WC_RSA_PSS
            void* RsaPssSignCtx;     /* Rsa PSS Sign   Callback Context */
            void* RsaPssVerifyCtx;   /* Rsa PSS Verify Callback Context */
        #endif
        void* RsaEncCtx;      /* Rsa Public  Encrypt   Callback Context */
        void* RsaDecCtx;      /* Rsa Private Decrypt   Callback Context */
    #endif /* NO_RSA */
    void* GenPreMasterCtx;   /* Generate Premaster Callback Context */
    void* GenMasterCtx;      /* Generate Master Callback Context */
    void* GenSessionKeyCtx;  /* Generate Session Key Callback Context */
    void* EncryptKeysCtx;    /* Set Encrypt keys Callback Context */
    void* TlsFinishedCtx;    /* Generate Tls Finished Callback Context */
    void* VerifyMacCtx;      /* Verify mac Callback Context */
#endif /* HAVE_PK_CALLBACKS */
#ifdef HAVE_SECRET_CALLBACK
        SessionSecretCb sessionSecretCb;
        void*           sessionSecretCtx;
    #ifdef WOLFSSL_TLS13
        Tls13SecretCb   tls13SecretCb;
        void*           tls13SecretCtx;
    #endif
    #ifdef OPENSSL_EXTRA
        SessionSecretCb keyLogCb;
    #ifdef WOLFSSL_TLS13
        Tls13SecretCb   tls13KeyLogCb;
    #endif
    #endif
#endif /* HAVE_SECRET_CALLBACK */
#ifdef WOLFSSL_JNI
        void* jObjectRef;     /* reference to WolfSSLSession in JNI wrapper */
#endif /* WOLFSSL_JNI */
#ifdef WOLFSSL_EARLY_DATA
    EarlyDataState earlyData;
    word32 earlyDataSz;
    byte earlyDataStatus;
#endif
#ifdef OPENSSL_ALL
    long verifyCallbackResult;
#endif
#if defined(OPENSSL_EXTRA)
    WOLFSSL_STACK* supportedCiphers; /* Used in wolfSSL_get_ciphers_compat */
    WOLFSSL_STACK* peerCertChain;    /* Used in wolfSSL_get_peer_cert_chain */
#ifdef KEEP_OUR_CERT
    WOLFSSL_STACK* ourCertChain;    /* Used in wolfSSL_add1_chain_cert */
#endif
#endif
#ifdef WOLFSSL_STATIC_EPHEMERAL
    StaticKeyExchangeInfo_t staticKE;
#endif
#ifdef WOLFSSL_MAXQ10XX_TLS
    maxq_ssl_t maxq_ctx;
#endif
#ifdef WOLFSSL_HAVE_TLS_UNIQUE
    /* Added in libest port: allow applications to get the 'tls-unique' Channel
     * Binding Type (https://tools.ietf.org/html/rfc5929#section-3). This is
     * used in the EST protocol to bind an enrollment to a TLS session through
     * 'proof-of-possession' (https://tools.ietf.org/html/rfc7030#section-3.4
     * and https://tools.ietf.org/html/rfc7030#section-3.5). */
    byte clientFinished[TLS_FINISHED_SZ_MAX];
    byte serverFinished[TLS_FINISHED_SZ_MAX];
    byte clientFinished_len;
    byte serverFinished_len;
#endif
#ifndef WOLFSSL_NO_CA_NAMES
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* client_ca_names;
#endif
#if defined(WOLFSSL_IOTSAFE) && defined(HAVE_PK_CALLBACKS)
    IOTSAFE iotsafe;
#endif
#ifdef WOLFSSL_LWIP_NATIVE
    WOLFSSL_LWIP_NATIVE_STATE      lwipCtx; /* LwIP native socket IO Context */
#endif
#ifdef WOLFSSL_QUIC
    struct {
        const WOLFSSL_QUIC_METHOD* method;
        WOLFSSL_ENCRYPTION_LEVEL enc_level_read;
        WOLFSSL_ENCRYPTION_LEVEL enc_level_read_next;
        WOLFSSL_ENCRYPTION_LEVEL enc_level_latest_recvd;
        WOLFSSL_ENCRYPTION_LEVEL enc_level_write;
        WOLFSSL_ENCRYPTION_LEVEL enc_level_write_next;
        int transport_version;
        const QuicTransportParam* transport_local;
        const QuicTransportParam* transport_peer;
        const QuicTransportParam* transport_peer_draft;
        QuicRecord* input_head;          /* we own, data for handshake */
        QuicRecord* input_tail;          /* points to last element for append */
        QuicRecord* scratch;             /* we own, record construction */
        enum wolfssl_encryption_level_t output_rec_level;
                                         /* encryption level of current output record */
        word32 output_rec_remain;        /* how many bytes of output TLS record
                                          * content have not been handled yet by quic */
    } quic;
#endif /* WOLFSSL_QUIC */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    WOLFSSL_EchConfig* echConfigs;
#endif
};

/*
 * wolfSSL_PEM_read_bio_X509 pushes an ASN_NO_PEM_HEADER error
 * to the error queue on file end. This should not be left
 * for the caller to find so we clear the last error.
 */
#if defined(OPENSSL_EXTRA) && defined(WOLFSSL_HAVE_ERROR_QUEUE)
#define CLEAR_ASN_NO_PEM_HEADER_ERROR(err)                  \
    (err) = wolfSSL_ERR_peek_last_error();                  \
    if (ERR_GET_LIB(err) == ERR_LIB_PEM &&                  \
            ERR_GET_REASON(err) == PEM_R_NO_START_LINE) {   \
        wc_RemoveErrorNode(-1);                             \
    }
#else
#define CLEAR_ASN_NO_PEM_HEADER_ERROR(err) (void)(err);
#endif

/*
 * The SSL object may have its own certificate store. The below macros simplify
 * logic for choosing which WOLFSSL_CERT_MANAGER and WOLFSSL_X509_STORE to use.
 * Always use SSL specific objects when available and revert to CTX otherwise.
 */
#ifdef WOLFSSL_LOCAL_X509_STORE
#define SSL_CM(ssl) ((ssl)->x509_store_pt ? (ssl)->x509_store_pt->cm : \
                     ((ssl)->ctx->x509_store_pt ? (ssl)->ctx->x509_store_pt->cm : \
                                            (ssl)->ctx->cm))
#define SSL_STORE(ssl) ((ssl)->x509_store_pt ? (ssl)->x509_store_pt : \
                  ((ssl)->ctx->x509_store_pt ? (ssl)->ctx->x509_store_pt : \
                                            &(ssl)->ctx->x509_store))
#define CTX_STORE(ctx) ((ctx)->x509_store_pt ? (ctx)->x509_store_pt : \
                                            &(ctx)->x509_store)
#else
#define SSL_CM(ssl) (ssl)->ctx->cm
#endif
/* Issue warning when we are modifying the overall context CM */
#define SSL_CM_WARNING(ssl) \
    do {                                                             \
        if (SSL_CM( (ssl) ) == (ssl)->ctx->cm) {                     \
            WOLFSSL_MSG("Modifying SSL_CTX CM not SSL specific CM"); \
        }                                                            \
    } while (0)

WOLFSSL_LOCAL int  SetSSL_CTX(WOLFSSL* ssl, WOLFSSL_CTX* ctx, int writeDup);
WOLFSSL_LOCAL int  InitSSL(WOLFSSL* ssl, WOLFSSL_CTX* ctx, int writeDup);
WOLFSSL_LOCAL int  ReinitSSL(WOLFSSL* ssl, WOLFSSL_CTX* ctx, int writeDup);
WOLFSSL_LOCAL void FreeSSL(WOLFSSL* ssl, void* heap);
WOLFSSL_API   void SSL_ResourceFree(WOLFSSL* ssl);   /* Micrium uses */


#ifndef NO_CERTS

    WOLFSSL_LOCAL int ProcessBuffer(WOLFSSL_CTX* ctx, const unsigned char* buff,
                                    long sz, int format, int type, WOLFSSL* ssl,
                                    long* used, int userChain, int verify);
    WOLFSSL_LOCAL int ProcessFile(WOLFSSL_CTX* ctx, const char* fname, int format,
                                 int type, WOLFSSL* ssl, int userChain,
                                WOLFSSL_CRL* crl, int verify);

    WOLFSSL_LOCAL int CheckHostName(DecodedCert* dCert, const char *domainName,
                                    size_t domainNameLen);
#endif


#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    WOLFSSL_LOCAL void InitHandShakeInfo(HandShakeInfo* info, WOLFSSL* ssl);
    WOLFSSL_LOCAL void FinishHandShakeInfo(HandShakeInfo* info);
    WOLFSSL_LOCAL void AddPacketName(WOLFSSL* ssl, const char* name);

    WOLFSSL_LOCAL void InitTimeoutInfo(TimeoutInfo* info);
    WOLFSSL_LOCAL void FreeTimeoutInfo(TimeoutInfo* info, void* heap);
    WOLFSSL_LOCAL int AddPacketInfo(WOLFSSL* ssl, const char* name, int type,
                             const byte* data, int sz, int written, int lateRL,
                             void* heap);
    WOLFSSL_LOCAL void AddLateName(const char* name, TimeoutInfo* info);
    WOLFSSL_LOCAL void AddLateRecordHeader(const RecordLayerHeader* rl,
                                           TimeoutInfo* info);
#endif


/* Record Layer Header identifier from page 12 */
enum ContentType {
    no_type            = 0,
    change_cipher_spec = 20,
    alert              = 21,
    handshake          = 22,
    application_data   = 23,
#ifdef WOLFSSL_DTLS13
    ack                = 26,
#endif /* WOLFSSL_DTLS13 */
};


/* handshake header, same for each message type, pgs 20/21 */
typedef struct HandShakeHeader {
    byte            type;
    word24          length;
} HandShakeHeader;


/* DTLS handshake header, same for each message type */
typedef struct DtlsHandShakeHeader {
    byte            type;
    word24          length;
    byte            message_seq[2];    /* start at 0, retransmit gets same # */
    word24          fragment_offset;   /* bytes in previous fragments */
    word24          fragment_length;   /* length of this fragment */
} DtlsHandShakeHeader;


enum HandShakeType {
    hello_request        =   0,
    client_hello         =   1,
    server_hello         =   2,
    hello_verify_request =   3,    /* DTLS addition */
    session_ticket       =   4,
    end_of_early_data    =   5,
    hello_retry_request  =   6,
    encrypted_extensions =   8,
    certificate          =  11,
    server_key_exchange  =  12,
    certificate_request  =  13,
    server_hello_done    =  14,
    certificate_verify   =  15,
    client_key_exchange  =  16,
    finished             =  20,
    certificate_status   =  22,
    key_update           =  24,
    change_cipher_hs     =  55,    /* simulate unique handshake type for sanity
                                      checks.  record layer change_cipher
                                      conflicts with handshake finished */
    message_hash         = 254,    /* synthetic message type for TLS v1.3 */
    no_shake             = 255     /* used to initialize the DtlsMsg record */
};

enum ProvisionSide {
    PROVISION_CLIENT = 1,
    PROVISION_SERVER = 2,
    PROVISION_CLIENT_SERVER = 3
};


static const byte kTlsClientStr[SIZEOF_SENDER+1] = { 0x43, 0x4C, 0x4E, 0x54, 0x00 }; /* CLNT */
static const byte kTlsServerStr[SIZEOF_SENDER+1] = { 0x53, 0x52, 0x56, 0x52, 0x00 }; /* SRVR */

static const byte kTlsClientFinStr[FINISHED_LABEL_SZ + 1] = "client finished";
static const byte kTlsServerFinStr[FINISHED_LABEL_SZ + 1] = "server finished";

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
typedef struct {
    int name_len;
    const char *name;
    int nid;
} WOLF_EC_NIST_NAME;
extern const WOLF_EC_NIST_NAME kNistCurves[];
/* This is the longest and shortest curve name in the kNistCurves list. Note we
 * also have quantum-safe group names as well. */
#define kNistCurves_MIN_NAME_LEN 5
#ifdef HAVE_PQC
#define kNistCurves_MAX_NAME_LEN 32
#else
#define kNistCurves_MAX_NAME_LEN 7
#endif
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

/* internal functions */
WOLFSSL_LOCAL int SendChangeCipher(WOLFSSL* ssl);
WOLFSSL_LOCAL int SendTicket(WOLFSSL* ssl);
#ifdef HAVE_SESSION_TICKET
WOLFSSL_LOCAL int DoDecryptTicket(const WOLFSSL* ssl, const byte* input,
        word32 len, InternalTicket **it);
/* Return 0 when check successful. <0 on failure. */
WOLFSSL_LOCAL void DoClientTicketFinalize(WOLFSSL* ssl, InternalTicket* it,
                                          const WOLFSSL_SESSION* sess);

#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL int DoClientTicketCheck(const WOLFSSL* ssl,
        const PreSharedKey* psk, sword64 timeout, const byte* suite);
WOLFSSL_LOCAL void CleanupClientTickets(PreSharedKey* psk);
WOLFSSL_LOCAL int DoClientTicket_ex(const WOLFSSL* ssl, PreSharedKey* psk,
                                    int retainSess);
#endif

WOLFSSL_LOCAL int DoClientTicket(WOLFSSL* ssl, const byte* input, word32 len);
#endif /* HAVE_SESSION_TICKET */
WOLFSSL_LOCAL int SendData(WOLFSSL* ssl, const void* data, int sz);
#ifdef WOLFSSL_TLS13
WOLFSSL_LOCAL int SendTls13ServerHello(WOLFSSL* ssl, byte extMsgType);
#endif
WOLFSSL_LOCAL int SendCertificate(WOLFSSL* ssl);
WOLFSSL_LOCAL int SendCertificateRequest(WOLFSSL* ssl);
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
WOLFSSL_LOCAL int CreateOcspResponse(WOLFSSL* ssl, OcspRequest** ocspRequest,
                       buffer* response);
#endif
#if defined(HAVE_SECURE_RENEGOTIATION) && \
    !defined(NO_WOLFSSL_SERVER)
WOLFSSL_LOCAL int SendHelloRequest(WOLFSSL* ssl);
#endif
WOLFSSL_LOCAL int SendCertificateStatus(WOLFSSL* ssl);
WOLFSSL_LOCAL int SendServerKeyExchange(WOLFSSL* ssl);
WOLFSSL_LOCAL int SendBuffered(WOLFSSL* ssl);
WOLFSSL_LOCAL int ReceiveData(WOLFSSL* ssl, byte* output, int sz, int peek);
WOLFSSL_LOCAL int SendFinished(WOLFSSL* ssl);
WOLFSSL_LOCAL int RetrySendAlert(WOLFSSL* ssl);
WOLFSSL_LOCAL int SendAlert(WOLFSSL* ssl, int severity, int type);
WOLFSSL_LOCAL int SendFatalAlertOnly(WOLFSSL *ssl, int error);
WOLFSSL_LOCAL int ProcessReply(WOLFSSL* ssl);
WOLFSSL_LOCAL int ProcessReplyEx(WOLFSSL* ssl, int allowSocketErr);

WOLFSSL_LOCAL const char* AlertTypeToString(int type);

WOLFSSL_LOCAL int SetCipherSpecs(WOLFSSL* ssl);
WOLFSSL_LOCAL int GetCipherSpec(word16 side, byte cipherSuite0,
        byte cipherSuite, CipherSpecs* specs, Options* opts);
WOLFSSL_LOCAL int MakeMasterSecret(WOLFSSL* ssl);

WOLFSSL_LOCAL int DeriveKeys(WOLFSSL* ssl);
WOLFSSL_LOCAL int StoreKeys(WOLFSSL* ssl, const byte* keyData, int side);

WOLFSSL_LOCAL int IsTLS(const WOLFSSL* ssl);
WOLFSSL_LOCAL int IsAtLeastTLSv1_2(const WOLFSSL* ssl);
WOLFSSL_LOCAL int IsAtLeastTLSv1_3(ProtocolVersion pv);
WOLFSSL_LOCAL int IsEncryptionOn(WOLFSSL* ssl, int isSend);
WOLFSSL_LOCAL int TLSv1_3_Capable(WOLFSSL* ssl);

WOLFSSL_LOCAL void FreeHandshakeResources(WOLFSSL* ssl);
WOLFSSL_LOCAL void ShrinkInputBuffer(WOLFSSL* ssl, int forcedFree);
WOLFSSL_LOCAL void ShrinkOutputBuffer(WOLFSSL* ssl);
WOLFSSL_LOCAL byte* GetOutputBuffer(WOLFSSL* ssl);

WOLFSSL_LOCAL int VerifyClientSuite(word16 havePSK, byte cipherSuite0,
                                    byte cipherSuite);

WOLFSSL_LOCAL int SetTicket(WOLFSSL* ssl, const byte* ticket, word32 length);
WOLFSSL_LOCAL int wolfSSL_GetMaxFragSize(WOLFSSL* ssl, int maxFragment);

#if defined(WOLFSSL_IOTSAFE) && defined(HAVE_PK_CALLBACKS)
WOLFSSL_LOCAL IOTSAFE *wolfSSL_get_iotsafe_ctx(WOLFSSL *ssl);
WOLFSSL_LOCAL int wolfSSL_set_iotsafe_ctx(WOLFSSL *ssl, IOTSAFE *iotsafe);
#endif

#if (defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)) && defined(HAVE_ECC)
WOLFSSL_LOCAL int SetECKeyInternal(WOLFSSL_EC_KEY* eckey);
WOLFSSL_LOCAL int SetECKeyExternal(WOLFSSL_EC_KEY* eckey);
#endif

#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
WOLFSSL_LOCAL int wolfSSL_curve_is_disabled(const WOLFSSL* ssl,
                                            word16 named_curve);
#else
#define wolfSSL_curve_is_disabled(ssl, c)   ((void)(ssl), (void)(c), 0)
#endif

WOLFSSL_LOCAL WC_RNG* WOLFSSL_RSA_GetRNG(WOLFSSL_RSA *rsa, WC_RNG **tmpRNG,
                                         int *initTmpRng);

#ifndef NO_CERTS
    #ifndef NO_RSA
        #ifdef WC_RSA_PSS
            WOLFSSL_LOCAL int CheckRsaPssPadding(const byte* plain, word32 plainSz,
                byte* out, word32 sigSz, enum wc_HashType hashType);
            WOLFSSL_LOCAL int ConvertHashPss(int hashAlgo,
                enum wc_HashType* hashType, int* mgf);
        #endif
        WOLFSSL_LOCAL int VerifyRsaSign(WOLFSSL* ssl, byte* verifySig,
            word32 sigSz, const byte* plain, word32 plainSz, int sigAlgo,
            int hashAlgo, RsaKey* key, DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int RsaSign(WOLFSSL* ssl, const byte* in, word32 inSz,
            byte* out, word32* outSz, int sigAlgo, int hashAlgo, RsaKey* key,
            DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int RsaVerify(WOLFSSL* ssl, byte* in, word32 inSz,
            byte** out, int sigAlgo, int hashAlgo, RsaKey* key,
            buffer* keyBufInfo);
        WOLFSSL_LOCAL int RsaDec(WOLFSSL* ssl, byte* in, word32 inSz, byte** out,
            word32* outSz, RsaKey* key, DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int RsaEnc(WOLFSSL* ssl, const byte* in, word32 inSz, byte* out,
            word32* outSz, RsaKey* key, buffer* keyBufInfo);
    #endif /* !NO_RSA */

    #ifdef HAVE_ECC
        WOLFSSL_LOCAL int EccSign(WOLFSSL* ssl, const byte* in, word32 inSz,
            byte* out, word32* outSz, ecc_key* key, DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int EccVerify(WOLFSSL* ssl, const byte* in, word32 inSz,
            const byte* out, word32 outSz, ecc_key* key, buffer* keyBufInfo);
        WOLFSSL_LOCAL int EccSharedSecret(WOLFSSL* ssl, ecc_key* priv_key,
            ecc_key* pub_key, byte* pubKeyDer, word32* pubKeySz, byte* out,
            word32* outlen, int side);
    #endif /* HAVE_ECC */
    #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
        WOLFSSL_LOCAL int Sm2wSm3Sign(WOLFSSL* ssl, const byte* id, word32 idSz,
            const byte* in, word32 inSz, byte* out, word32* outSz, ecc_key* key,
            DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int Sm2wSm3Verify(WOLFSSL* ssl, const byte* id,
            word32 idSz, const byte* in, word32 inSz, const byte* out,
            word32 outSz, ecc_key* key, buffer* keyBufInfo);
    #endif /* WOLFSSL_SM2 && WOLFSSL_SM3 */
    #ifdef HAVE_ED25519
        WOLFSSL_LOCAL int Ed25519CheckPubKey(WOLFSSL* ssl);
        WOLFSSL_LOCAL int Ed25519Sign(WOLFSSL* ssl, const byte* in, word32 inSz,
            byte* out, word32* outSz, ed25519_key* key, DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int Ed25519Verify(WOLFSSL* ssl, const byte* in,
            word32 inSz, const byte* msg, word32 msgSz, ed25519_key* key,
            buffer* keyBufInfo);
    #endif /* HAVE_ED25519 */
    #ifdef HAVE_ED448
        WOLFSSL_LOCAL int Ed448CheckPubKey(WOLFSSL* ssl);
        WOLFSSL_LOCAL int Ed448Sign(WOLFSSL* ssl, const byte* in, word32 inSz,
            byte* out, word32* outSz, ed448_key* key, DerBuffer* keyBufInfo);
        WOLFSSL_LOCAL int Ed448Verify(WOLFSSL* ssl, const byte* in,
            word32 inSz, const byte* msg, word32 msgSz, ed448_key* key,
            buffer* keyBufInfo);
    #endif /* HAVE_ED448 */


    #ifdef WOLFSSL_TRUST_PEER_CERT

        /* options for searching hash table for a matching trusted peer cert */
        #define WC_MATCH_SKID 0
        #define WC_MATCH_NAME 1

        WOLFSSL_LOCAL TrustedPeerCert* GetTrustedPeer(void* vp, DecodedCert* cert);
        WOLFSSL_LOCAL int MatchTrustedPeer(TrustedPeerCert* tp,
                                                             DecodedCert* cert);
    #endif

    #ifndef GetCA
        WOLFSSL_LOCAL Signer* GetCA(void* vp, byte* hash);
    #endif
    #ifdef WOLFSSL_AKID_NAME
        WOLFSSL_LOCAL Signer* GetCAByAKID(void* vp, const byte* issuer,
                word32 issuerSz, const byte* serial, word32 serialSz);
    #endif
    #if !defined(NO_SKID) && !defined(GetCAByName)
        WOLFSSL_LOCAL Signer* GetCAByName(void* cm, byte* hash);
    #endif
#endif /* !NO_CERTS */
WOLFSSL_LOCAL int  BuildTlsHandshakeHash(WOLFSSL* ssl, byte* hash,
                                   word32* hashLen);
WOLFSSL_LOCAL int  BuildTlsFinished(WOLFSSL* ssl, Hashes* hashes,
                                   const byte* sender);
WOLFSSL_LOCAL void FreeArrays(WOLFSSL* ssl, int keep);
WOLFSSL_LOCAL  int CheckAvailableSize(WOLFSSL *ssl, int size);
WOLFSSL_LOCAL  int GrowInputBuffer(WOLFSSL* ssl, int size, int usedLength);
#if !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH)
WOLFSSL_LOCAL void DoCertFatalAlert(WOLFSSL* ssl, int ret);
#endif
#ifndef NO_TLS
    WOLFSSL_LOCAL int  MakeTlsMasterSecret(WOLFSSL* ssl);
#ifndef WOLFSSL_AEAD_ONLY
    WOLFSSL_LOCAL int  TLS_hmac(WOLFSSL* ssl, byte* digest, const byte* in,
                                word32 sz, int padSz, int content, int verify, int epochOrder);
#endif
#endif

WOLFSSL_LOCAL int cipherExtraData(WOLFSSL* ssl);

#ifndef NO_WOLFSSL_CLIENT
    WOLFSSL_LOCAL int SendClientHello(WOLFSSL* ssl);
    WOLFSSL_LOCAL int DoHelloVerifyRequest(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
        word32 size);
    #ifdef WOLFSSL_TLS13
    WOLFSSL_LOCAL int SendTls13ClientHello(WOLFSSL* ssl);
    #endif
    WOLFSSL_LOCAL int SendClientKeyExchange(WOLFSSL* ssl);
    WOLFSSL_LOCAL int SendCertificateVerify(WOLFSSL* ssl);
#endif /* NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
    WOLFSSL_LOCAL int SendServerHello(WOLFSSL* ssl);
    WOLFSSL_LOCAL int SendServerHelloDone(WOLFSSL* ssl);
#endif /* NO_WOLFSSL_SERVER */

#ifdef WOLFSSL_DTLS
    WOLFSSL_LOCAL DtlsMsg* DtlsMsgNew(word32 sz, byte tx, void* heap);
    WOLFSSL_LOCAL void DtlsMsgDelete(DtlsMsg* item, void* heap);
    /* Use WOLFSSL_API to enable src/api.c testing */
    WOLFSSL_API void DtlsMsgListDelete(DtlsMsg* head, void* heap);
    WOLFSSL_LOCAL void DtlsTxMsgListClean(WOLFSSL* ssl);
    WOLFSSL_LOCAL int  DtlsMsgSet(DtlsMsg* msg, word32 seq, word16 epoch,
                                  const byte* data, byte type,
                                  word32 fragOffset, word32 fragSz, void* heap,
                                  word32 totalLen);
    /* Use WOLFSSL_API to enable src/api.c testing */
    WOLFSSL_API DtlsMsg* DtlsMsgFind(DtlsMsg* head, word16 epoch, word32 seq);

    /* Use WOLFSSL_API to enable src/api.c testing */
    WOLFSSL_API void DtlsMsgStore(WOLFSSL* ssl, word16 epoch, word32 seq,
                                    const byte* data, word32 dataSz, byte type,
                                    word32 fragOffset, word32 fragSz,
                                    void* heap);
    WOLFSSL_LOCAL DtlsMsg* DtlsMsgInsert(DtlsMsg* head, DtlsMsg* item);

    WOLFSSL_LOCAL int  DtlsMsgPoolSave(WOLFSSL* ssl, const byte* data,
                                       word32 dataSz, enum HandShakeType type);
    WOLFSSL_LOCAL int  DtlsMsgPoolTimeout(WOLFSSL* ssl);
    WOLFSSL_LOCAL int  VerifyForDtlsMsgPoolSend(WOLFSSL* ssl, byte type,
                                                word32 fragOffset);
    WOLFSSL_LOCAL int  VerifyForTxDtlsMsgDelete(WOLFSSL* ssl, DtlsMsg* item);
    WOLFSSL_LOCAL void DtlsMsgPoolReset(WOLFSSL* ssl);
    WOLFSSL_LOCAL int  DtlsMsgPoolSend(WOLFSSL* ssl, int sendOnlyFirstPacket);
    WOLFSSL_LOCAL void DtlsMsgDestroyFragBucket(DtlsFragBucket* fragBucket, void* heap);
    WOLFSSL_LOCAL int GetDtlsHandShakeHeader(WOLFSSL *ssl, const byte *input,
        word32 *inOutIdx, byte *type, word32 *size, word32 *fragOffset,
        word32 *fragSz, word32 totalSz);
    WOLFSSL_LOCAL int DtlsMsgDrain(WOLFSSL *ssl);
    WOLFSSL_LOCAL int SendHelloVerifyRequest(WOLFSSL* ssl,
        const byte* cookie, byte cookieSz);

#if !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_LOCAL int DoClientHelloStateless(WOLFSSL* ssl,
            const byte* input, word32* inOutIdx, word32 helloSz);
#endif /* !defined(NO_WOLFSSL_SERVER) */
#endif /* WOLFSSL_DTLS */

#if defined(HAVE_SECURE_RENEGOTIATION) && defined(WOLFSSL_DTLS)
    WOLFSSL_LOCAL int DtlsSCRKeysSet(WOLFSSL* ssl);
    WOLFSSL_LOCAL int IsDtlsMsgSCRKeys(WOLFSSL* ssl);
    WOLFSSL_LOCAL int DtlsUseSCRKeys(WOLFSSL* ssl);
    WOLFSSL_LOCAL int DtlsCheckOrder(WOLFSSL* ssl, int order);
#endif
    WOLFSSL_LOCAL int IsSCR(WOLFSSL* ssl);
    WOLFSSL_LOCAL int IsDtlsNotSctpMode(WOLFSSL* ssl);
    WOLFSSL_LOCAL int IsDtlsNotSrtpMode(WOLFSSL* ssl);

    WOLFSSL_LOCAL void WriteSEQ(WOLFSSL* ssl, int verifyOrder, byte* out);

#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
#ifdef WOLFSSL_32BIT_MILLI_TIME
    WOLFSSL_LOCAL word32 TimeNowInMilliseconds(void);
#else
    WOLFSSL_LOCAL sword64 TimeNowInMilliseconds(void);
#endif

#endif
WOLFSSL_LOCAL word32  LowResTimer(void);

WOLFSSL_LOCAL int FindSuiteSSL(const WOLFSSL* ssl, byte* suite);

#ifndef NO_CERTS
    WOLFSSL_LOCAL void InitX509Name(WOLFSSL_X509_NAME* name, int dynamicFlag,
                                    void* heap);
    WOLFSSL_LOCAL void FreeX509Name(WOLFSSL_X509_NAME* name);
    WOLFSSL_LOCAL void InitX509(WOLFSSL_X509* x509, int dynamicFlag,
                                void* heap);
    WOLFSSL_LOCAL void FreeX509(WOLFSSL_X509* x509);
    WOLFSSL_LOCAL int  CopyDecodedToX509(WOLFSSL_X509* x509,
                                         DecodedCert* dCert);
#endif

#ifndef MAX_CIPHER_NAME
#define MAX_CIPHER_NAME 50
#endif

#ifdef WOLFSSL_NAMES_STATIC
typedef char cipher_name[MAX_CIPHER_NAME];
#else
typedef const char* cipher_name;
#endif

typedef struct CipherSuiteInfo {
    cipher_name name;
#ifndef NO_ERROR_STRINGS
    cipher_name name_iana;
#endif
    byte cipherSuite0;
    byte cipherSuite;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT) || \
    defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_NGINX)
    byte minor;
    byte major;
#endif
    byte flags;
} CipherSuiteInfo;

WOLFSSL_LOCAL const CipherSuiteInfo* GetCipherNames(void);
WOLFSSL_LOCAL int GetCipherNamesSize(void);
WOLFSSL_LOCAL const char* GetCipherNameInternal(byte cipherSuite0, byte cipherSuite);
#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT)
/* used in wolfSSL_sk_CIPHER_description */
#define MAX_SEGMENTS    5
#define MAX_SEGMENT_SZ 20
WOLFSSL_LOCAL int wolfSSL_sk_CIPHER_description(WOLFSSL_CIPHER* cipher);
WOLFSSL_LOCAL const char* GetCipherSegment(const WOLFSSL_CIPHER* cipher,
                                           char n[][MAX_SEGMENT_SZ]);
WOLFSSL_LOCAL const char* GetCipherProtocol(byte minor);
WOLFSSL_LOCAL const char* GetCipherKeaStr(char n[][MAX_SEGMENT_SZ]);
WOLFSSL_LOCAL const char* GetCipherAuthStr(char n[][MAX_SEGMENT_SZ]);
WOLFSSL_LOCAL const char* GetCipherEncStr(char n[][MAX_SEGMENT_SZ]);
WOLFSSL_LOCAL const char* GetCipherMacStr(char n[][MAX_SEGMENT_SZ]);
WOLFSSL_LOCAL int SetCipherBits(const char* enc);
WOLFSSL_LOCAL int IsCipherAEAD(char n[][MAX_SEGMENT_SZ]);
#endif
WOLFSSL_LOCAL const char* GetCipherNameIana(byte cipherSuite0, byte cipherSuite);
WOLFSSL_LOCAL const char* wolfSSL_get_cipher_name_internal(WOLFSSL* ssl);
WOLFSSL_LOCAL const char* wolfSSL_get_cipher_name_iana(WOLFSSL* ssl);
WOLFSSL_LOCAL int GetCipherSuiteFromName(const char* name, byte* cipherSuite0,
                                         byte* cipherSuite, int* flags);


enum encrypt_side {
    ENCRYPT_SIDE_ONLY = 1,
    DECRYPT_SIDE_ONLY,
    ENCRYPT_AND_DECRYPT_SIDE
};

WOLFSSL_LOCAL int SetKeysSide(WOLFSSL* ssl, enum encrypt_side side);

/* Set*Internal and Set*External functions */
WOLFSSL_LOCAL int SetDsaInternal(WOLFSSL_DSA* dsa);
WOLFSSL_LOCAL int SetDsaExternal(WOLFSSL_DSA* dsa);
#ifndef HAVE_USER_RSA
WOLFSSL_LOCAL int SetRsaExternal(WOLFSSL_RSA* rsa);
WOLFSSL_LOCAL int SetRsaInternal(WOLFSSL_RSA* rsa);
#endif

typedef enum elem_set {
    ELEMENT_P   = 0x01,
    ELEMENT_Q   = 0x02,
    ELEMENT_G   = 0x04,
    ELEMENT_PUB = 0x08,
    ELEMENT_PRV = 0x10,
} Element_Set;
WOLFSSL_LOCAL int SetDhExternal_ex(WOLFSSL_DH *dh, int elm );
WOLFSSL_LOCAL int SetDhInternal(WOLFSSL_DH* dh);
WOLFSSL_LOCAL int SetDhExternal(WOLFSSL_DH *dh);

#if !defined(NO_DH) && (!defined(NO_CERTS) || !defined(NO_PSK))
    WOLFSSL_LOCAL int DhGenKeyPair(WOLFSSL* ssl, DhKey* dhKey,
        byte* priv, word32* privSz,
        byte* pub, word32* pubSz);
    WOLFSSL_LOCAL int DhAgree(WOLFSSL* ssl, DhKey* dhKey,
        const byte* priv, word32 privSz,
        const byte* otherPub, word32 otherPubSz,
        byte* agree, word32* agreeSz,
        const byte* prime, word32 primeSz);
#endif /* !NO_DH */

#ifdef HAVE_ECC
    WOLFSSL_LOCAL int EccMakeKey(WOLFSSL* ssl, ecc_key* key, ecc_key* peer);
    WOLFSSL_LOCAL word16 GetCurveByOID(int oidSum);
#endif

WOLFSSL_LOCAL int InitHandshakeHashes(WOLFSSL* ssl);
WOLFSSL_LOCAL void FreeHandshakeHashes(WOLFSSL* ssl);
WOLFSSL_LOCAL int InitHandshakeHashesAndCopy(WOLFSSL* ssl, HS_Hashes* source,
    HS_Hashes** destination);


#ifndef WOLFSSL_NO_TLS12
WOLFSSL_LOCAL void FreeBuildMsgArgs(WOLFSSL* ssl, BuildMsgArgs* args);
#endif
WOLFSSL_LOCAL int BuildMessage(WOLFSSL* ssl, byte* output, int outSz,
                        const byte* input, int inSz, int type, int hashOutput,
                        int sizeOnly, int asyncOkay, int epochOrder);

#ifdef WOLFSSL_TLS13
/* Use WOLFSSL_API to use this function in tests/api.c */
WOLFSSL_API int BuildTls13Message(WOLFSSL* ssl, byte* output, int outSz, const byte* input,
               int inSz, int type, int hashOutput, int sizeOnly, int asyncOkay);
WOLFSSL_LOCAL int Tls13UpdateKeys(WOLFSSL* ssl);
#endif

WOLFSSL_LOCAL int AllocKey(WOLFSSL* ssl, int type, void** pKey);
WOLFSSL_LOCAL void FreeKey(WOLFSSL* ssl, int type, void** pKey);

#ifdef WOLFSSL_ASYNC_CRYPT
    WOLFSSL_LOCAL int wolfSSL_AsyncInit(WOLFSSL* ssl, WC_ASYNC_DEV* asyncDev, word32 flags);
    WOLFSSL_LOCAL int wolfSSL_AsyncPop(WOLFSSL* ssl, byte* state);
    WOLFSSL_LOCAL int wolfSSL_AsyncPush(WOLFSSL* ssl, WC_ASYNC_DEV* asyncDev);
#endif

#if defined(OPENSSL_ALL) && defined(WOLFSSL_CERT_GEN) && \
    (defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_EXT)) && \
    !defined(NO_FILESYSTEM) && !defined(NO_WOLFSSL_DIR)
WOLFSSL_LOCAL int LoadCertByIssuer(WOLFSSL_X509_STORE* store,
                                           X509_NAME* issuer, int Type);
#endif
#if defined(OPENSSL_ALL) && !defined(NO_FILESYSTEM) && !defined(NO_WOLFSSL_DIR)
WOLFSSL_LOCAL WOLFSSL_BY_DIR_HASH* wolfSSL_BY_DIR_HASH_new(void);
WOLFSSL_LOCAL void wolfSSL_BY_DIR_HASH_free(WOLFSSL_BY_DIR_HASH* dir_hash);
WOLFSSL_LOCAL WOLFSSL_STACK* wolfSSL_sk_BY_DIR_HASH_new_null(void);
WOLFSSL_LOCAL int wolfSSL_sk_BY_DIR_HASH_find(
   WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH)* sk, const WOLFSSL_BY_DIR_HASH* toFind);
WOLFSSL_LOCAL int wolfSSL_sk_BY_DIR_HASH_num(const WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH) *sk);
WOLFSSL_LOCAL WOLFSSL_BY_DIR_HASH* wolfSSL_sk_BY_DIR_HASH_value(
                        const WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH) *sk, int i);
WOLFSSL_LOCAL WOLFSSL_BY_DIR_HASH* wolfSSL_sk_BY_DIR_HASH_pop(
                                WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH)* sk);
WOLFSSL_LOCAL void wolfSSL_sk_BY_DIR_HASH_pop_free(WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH)* sk,
    void (*f) (WOLFSSL_BY_DIR_HASH*));
WOLFSSL_LOCAL void wolfSSL_sk_BY_DIR_HASH_free(WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH) *sk);
WOLFSSL_LOCAL int wolfSSL_sk_BY_DIR_HASH_push(WOLF_STACK_OF(WOLFSSL_BY_DIR_HASH)* sk,
                                               WOLFSSL_BY_DIR_HASH* in);
/* WOLFSSL_BY_DIR_entry stuff */
WOLFSSL_LOCAL WOLFSSL_BY_DIR_entry* wolfSSL_BY_DIR_entry_new(void);
WOLFSSL_LOCAL void wolfSSL_BY_DIR_entry_free(WOLFSSL_BY_DIR_entry* entry);
WOLFSSL_LOCAL WOLFSSL_STACK* wolfSSL_sk_BY_DIR_entry_new_null(void);
WOLFSSL_LOCAL int wolfSSL_sk_BY_DIR_entry_num(const WOLF_STACK_OF(WOLFSSL_BY_DIR_entry) *sk);
WOLFSSL_LOCAL WOLFSSL_BY_DIR_entry* wolfSSL_sk_BY_DIR_entry_value(
                        const WOLF_STACK_OF(WOLFSSL_BY_DIR_entry) *sk, int i);
WOLFSSL_LOCAL WOLFSSL_BY_DIR_entry* wolfSSL_sk_BY_DIR_entry_pop(
                                WOLF_STACK_OF(WOLFSSL_BY_DIR_entry)* sk);
WOLFSSL_LOCAL void wolfSSL_sk_BY_DIR_entry_pop_free(WOLF_STACK_OF(wolfSSL_BY_DIR_entry)* sk,
    void (*f) (WOLFSSL_BY_DIR_entry*));
WOLFSSL_LOCAL void wolfSSL_sk_BY_DIR_entry_free(WOLF_STACK_OF(wolfSSL_BY_DIR_entry) *sk);
WOLFSSL_LOCAL int wolfSSL_sk_BY_DIR_entry_push(WOLF_STACK_OF(wolfSSL_BY_DIR_entry)* sk,
                                               WOLFSSL_BY_DIR_entry* in);
#endif /* OPENSSL_ALL && !NO_FILESYSTEM && !NO_WOLFSSL_DIR */
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
WOLFSSL_LOCAL int oid2nid(word32 oid, int grp);
WOLFSSL_LOCAL word32 nid2oid(int nid, int grp);
#endif

#ifdef WOLFSSL_DTLS
WOLFSSL_API int wolfSSL_DtlsUpdateWindow(word16 cur_hi, word32 cur_lo,
        word16* next_hi, word32* next_lo, word32 *window);
WOLFSSL_LOCAL void DtlsResetState(WOLFSSL *ssl);
WOLFSSL_LOCAL int DtlsIgnoreError(int err);
WOLFSSL_LOCAL void DtlsSetSeqNumForReply(WOLFSSL* ssl);
#endif

#ifdef WOLFSSL_DTLS13

/* Use WOLFSSL_API to use this function in tests/api.c */
WOLFSSL_API struct Dtls13Epoch* Dtls13GetEpoch(WOLFSSL* ssl,
    w64wrapper epochNumber);
WOLFSSL_LOCAL void Dtls13SetOlderEpochSide(WOLFSSL* ssl, w64wrapper epochNumber,
    int side);
WOLFSSL_LOCAL int Dtls13NewEpoch(WOLFSSL* ssl, w64wrapper epochNumber,
    int side);
WOLFSSL_LOCAL int Dtls13SetEpochKeys(WOLFSSL* ssl, w64wrapper epochNumber,
    enum encrypt_side side);
WOLFSSL_LOCAL int Dtls13GetSeq(WOLFSSL* ssl, int order, word32* seq,
    byte increment);
WOLFSSL_LOCAL int Dtls13DoScheduledWork(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13DeriveSnKeys(WOLFSSL* ssl, int provision);
WOLFSSL_LOCAL int Dtls13SetRecordNumberKeys(WOLFSSL* ssl,
    enum encrypt_side side);

WOLFSSL_LOCAL int Dtls13AddHeaders(byte* output, word32 length,
    enum HandShakeType hs_type, WOLFSSL* ssl);
WOLFSSL_LOCAL word16 Dtls13GetHeadersLength(WOLFSSL *ssl,
    enum HandShakeType type);
WOLFSSL_LOCAL word16 Dtls13GetRlHeaderLength(WOLFSSL *ssl, byte is_encrypted);
WOLFSSL_LOCAL int Dtls13RlAddCiphertextHeader(WOLFSSL* ssl, byte* out,
    word16 length);
WOLFSSL_LOCAL int Dtls13RlAddPlaintextHeader(WOLFSSL* ssl, byte* out,
    enum ContentType content_type, word16 length);
WOLFSSL_LOCAL int Dtls13EncryptRecordNumber(WOLFSSL* ssl, byte* hdr,
    word16 recordLength);
WOLFSSL_LOCAL int Dtls13IsUnifiedHeader(byte header_flags);
WOLFSSL_LOCAL int Dtls13GetUnifiedHeaderSize(WOLFSSL* ssl, const byte input,
    word16* size);
WOLFSSL_LOCAL int Dtls13ParseUnifiedRecordLayer(WOLFSSL* ssl, const byte* input,
    word16 input_size, Dtls13UnifiedHdrInfo* hdrInfo);
WOLFSSL_LOCAL int Dtls13HandshakeSend(WOLFSSL* ssl, byte* output,
    word16 output_size, word16 length, enum HandShakeType handshake_type,
    int hash_output);
WOLFSSL_LOCAL int Dtls13RecordRecvd(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13HandshakeRecv(WOLFSSL* ssl, byte* input,
    word32* inOutIdx, word32 totalSz);
WOLFSSL_LOCAL int Dtls13HandshakeAddHeader(WOLFSSL* ssl, byte* output,
    enum HandShakeType msg_type, word32 length);
#define EE_MASK (0x3)
WOLFSSL_LOCAL int Dtls13FragmentsContinue(WOLFSSL* ssl);
WOLFSSL_LOCAL int DoDtls13Ack(WOLFSSL* ssl, const byte* input, word32 inputSize,
    word32* processedSize);
WOLFSSL_LOCAL int Dtls13ReconstructEpochNumber(WOLFSSL* ssl, byte epochBits,
    w64wrapper* epoch);
WOLFSSL_LOCAL int Dtls13ReconstructSeqNumber(WOLFSSL* ssl,
    Dtls13UnifiedHdrInfo* hdrInfo, w64wrapper* out);
WOLFSSL_LOCAL int SendDtls13Ack(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13RtxProcessingCertificate(WOLFSSL* ssl, byte* input,
    word32 inputSize);
WOLFSSL_LOCAL int Dtls13HashHandshake(WOLFSSL* ssl, const byte* input,
    word16 length);
WOLFSSL_LOCAL int Dtls13HashClientHello(const WOLFSSL* ssl, byte* hash,
        int* hashSz, const byte* body, word32 length, CipherSpecs* specs);
WOLFSSL_LOCAL void Dtls13FreeFsmResources(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13RtxTimeout(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13ProcessBufferedMessages(WOLFSSL* ssl);
WOLFSSL_LOCAL int Dtls13CheckAEADFailLimit(WOLFSSL* ssl);
#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_STATIC_EPHEMERAL
WOLFSSL_LOCAL int wolfSSL_StaticEphemeralKeyLoad(WOLFSSL* ssl, int keyAlgo, void* keyPtr);
#endif

#ifndef NO_CERTS
#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_EXTRA_X509_SMALL)
WOLFSSL_LOCAL int wolfSSL_ASN1_STRING_canon(WOLFSSL_ASN1_STRING* asn_out,
    const WOLFSSL_ASN1_STRING* asn_in);
#endif
#ifdef OPENSSL_EXTRA
WOLFSSL_LOCAL int GetX509Error(int e);
#endif
#endif

#if defined(HAVE_EX_DATA) && \
    (defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA) || \
    defined(HAVE_LIGHTY)) || defined(HAVE_EX_DATA) || \
    defined(WOLFSSL_WPAS_SMALL)
typedef struct CRYPTO_EX_cb_ctx {
    long ctx_l;
    void *ctx_ptr;
    WOLFSSL_CRYPTO_EX_new* new_func;
    WOLFSSL_CRYPTO_EX_free* free_func;
    WOLFSSL_CRYPTO_EX_dup* dup_func;
    struct CRYPTO_EX_cb_ctx* next;
} CRYPTO_EX_cb_ctx;
/* use wolfSSL_API visibility to be able to clear in tests/api.c */
WOLFSSL_API extern CRYPTO_EX_cb_ctx* crypto_ex_cb_ctx_session;
WOLFSSL_API void crypto_ex_cb_free(CRYPTO_EX_cb_ctx* cb_ctx);
WOLFSSL_LOCAL void crypto_ex_cb_setup_new_data(void *new_obj,
        CRYPTO_EX_cb_ctx* cb_ctx, WOLFSSL_CRYPTO_EX_DATA* ex_data);
WOLFSSL_LOCAL void crypto_ex_cb_free_data(void *obj, CRYPTO_EX_cb_ctx* cb_ctx,
        WOLFSSL_CRYPTO_EX_DATA* ex_data);
WOLFSSL_LOCAL int crypto_ex_cb_dup_data(const WOLFSSL_CRYPTO_EX_DATA *in,
        WOLFSSL_CRYPTO_EX_DATA *out, CRYPTO_EX_cb_ctx* cb_ctx);
WOLFSSL_LOCAL int wolfssl_get_ex_new_index(int class_index, long ctx_l,
        void* ctx_ptr, WOLFSSL_CRYPTO_EX_new* new_func,
        WOLFSSL_CRYPTO_EX_dup* dup_func, WOLFSSL_CRYPTO_EX_free* free_func);
#endif

WOLFSSL_LOCAL WC_RNG* wolfssl_get_global_rng(void);
WOLFSSL_LOCAL WC_RNG* wolfssl_make_global_rng(void);

#if !defined(WOLFCRYPT_ONLY) && defined(OPENSSL_EXTRA)
#if defined(WOLFSSL_KEY_GEN) && defined(WOLFSSL_PEM_TO_DER)
WOLFSSL_LOCAL int EncryptDerKey(byte *der, int *derSz, const EVP_CIPHER* cipher,
    unsigned char* passwd, int passwdSz, byte **cipherInfo, int maxDerSz);
#endif
#endif

#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
WOLFSSL_LOCAL int wolfSSL_RSA_To_Der(WOLFSSL_RSA* rsa, byte** outBuf,
    int publicKey, void* heap);
#endif

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) \
    || defined(OPENSSL_EXTRA) || defined(HAVE_LIGHTY) || defined(HAVE_SECRET_CALLBACK)
WOLFSSL_LOCAL int wolfSSL_SSL_do_handshake_internal(WOLFSSL *s);
#endif

#ifdef WOLFSSL_QUIC
#define WOLFSSL_IS_QUIC(s)  (((s) != NULL) && ((s)->quic.method != NULL))
WOLFSSL_LOCAL int wolfSSL_quic_receive(WOLFSSL* ssl, byte* buf, word32 sz);
WOLFSSL_LOCAL int wolfSSL_quic_send(WOLFSSL* ssl);
WOLFSSL_LOCAL void wolfSSL_quic_clear(WOLFSSL* ssl);
WOLFSSL_LOCAL void wolfSSL_quic_free(WOLFSSL* ssl);
WOLFSSL_LOCAL int wolfSSL_quic_forward_secrets(WOLFSSL *ssl,
                                               int ktype, int side);
WOLFSSL_LOCAL int wolfSSL_quic_keys_active(WOLFSSL* ssl, enum encrypt_side side);

#else
#define WOLFSSL_IS_QUIC(s) 0
#endif /* WOLFSSL_QUIC (else) */

#if defined(SHOW_SECRETS) && defined(WOLFSSL_SSLKEYLOGFILE)
WOLFSSL_LOCAL int tls13ShowSecrets(WOLFSSL* ssl, int id, const unsigned char* secret,
    int secretSz, void* ctx);
#endif

/* Optional Pre-Master-Secret logging for Wireshark */
#if !defined(NO_FILESYSTEM) && defined(WOLFSSL_SSLKEYLOGFILE)
#ifndef WOLFSSL_SSLKEYLOGFILE_OUTPUT
    #define WOLFSSL_SSLKEYLOGFILE_OUTPUT "sslkeylog.log"
#endif
#endif

#if defined(WOLFSSL_TLS13) && !defined(NO_PSK)
WOLFSSL_LOCAL int FindPskSuite(const WOLFSSL* ssl, PreSharedKey* psk,
        byte* psk_key, word32* psk_keySz, const byte* suite, int* found,
        byte* foundSuite);
#endif

WOLFSSL_LOCAL int wolfSSL_GetHmacType_ex(CipherSpecs* specs);

#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
WOLFSSL_LOCAL int CreateCookieExt(const WOLFSSL* ssl, byte* hash,
                                  word16 hashSz, TLSX** exts,
                                  byte cipherSuite0, byte cipherSuite);
#endif

WOLFSSL_LOCAL int TranslateErrorToAlert(int err);

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
void* wolfssl_sk_pop_type(WOLFSSL_STACK* sk, WOLF_STACK_TYPE type);
WOLFSSL_STACK* wolfssl_sk_new_type(WOLF_STACK_TYPE type);
#endif

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* wolfSSL_INT_H */
