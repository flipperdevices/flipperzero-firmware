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

/* Example 'user_settings.h' for IoT-Safe demo */

#ifndef IOTSAFE_EXAMPLE_USER_SETTINGS_H
#define IOTSAFE_EXAMPLE_USER_SETTINGS_H
#include <stdint.h>

/* Uncomment next line to enable 2-bytes ID demo */
/* #define TWO_BYTES_ID_DEMO */


/* IOT-Safe slot configurations for this example:
 *   - TWO_BYTES_ID_DEMO: two-bytes ID sim, with hardcoded CA
 *   - Default: one-byte ID sim, with hardcoded server certificate
 */

#ifdef TWO_BYTES_ID_DEMO
    #define IOTSAFE_ID_SIZE 2
    #define CRT_CLIENT_FILE_ID  0x3430     /* pre-provisioned */
    #define CRT_SERVER_FILE_ID  0x3330
    #define PRIVKEY_ID          0x3230     /* pre-provisioned */
    #define ECDH_KEYPAIR_ID     0x3330
    #define PEER_PUBKEY_ID      0x3730
    #define PEER_CERT_ID        0x3430

    /* In this version of the demo, the server certificate is
     * stored in a buffer, while the CA is read from a file slot in IoT-SAFE
     */
    #define SOFT_SERVER_CERT
#else
    #define IOTSAFE_ID_SIZE     1
    #define CRT_CLIENT_FILE_ID  0x03     /* pre-provisioned */
    #define CRT_SERVER_FILE_ID  0x04
    #define PRIVKEY_ID          0x02 /* pre-provisioned */
    #define ECDH_KEYPAIR_ID     0x03
    #define PEER_PUBKEY_ID      0x04
    #define PEER_CERT_ID        0x05

    /* In this version of the demo, the server certificate is
     * read from a file slot in IoT-SAFE, while the CA is stored in buffer in memory
     */
    #define SOFT_SERVER_CA
#endif




/* Platform */
#define WOLFSSL_IOTSAFE
#define WOLFSSL_SMALL_STACK
#define WOLFSSL_GENERAL_ALIGNMENT 4
#define SINGLE_THREADED
#define WOLFSSL_USER_IO


/* Debugging */
#define WOLFSSL_LOG_PRINTF

/* Change to "if 1" to enable debug */
#if 0
    #define DEBUG_WOLFSSL
    #define WOLFSSL_DEBUG_TLS
    #define DEBUG_IOTSAFE
#endif

/* Features */
#define HAVE_PK_CALLBACKS /* Connect IoT-safe with PK_CALLBACKS */
#define SMALL_SESSION_CACHE
#define USE_CERT_BUFFERS_256

/* RNG */
#define HAVE_IOTSAFE_HWRNG
#define HAVE_HASHDRBG
#define NO_OLD_RNGNAME

//#define USE_GENSEED_FORTEST

/* Time porting */
#define TIME_OVERRIDES
extern volatile unsigned long jiffies;
static inline long XTIME(long *x) { return jiffies;}
#define WOLFSSL_USER_CURRTIME
#define NO_ASN_TIME

/* Math */
#define TFM_TIMING_RESISTANT
#define TFM_ARM
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_MATH_ALL
#define WOLFSSL_SP_SMALL
#define WOLFSSL_HAVE_SP_DH
#define WOLFSSL_HAVE_SP_ECC
#define WOLFSSL_HAVE_SP_RSA
#define SP_WORD_SIZE 32

/* ECC */
#define HAVE_ECC
#define ECC_ALT_SIZE
#define ECC_TIMING_RESISTANT

/* RSA */
#define RSA_LOW_MEM
#define WC_RSA_BLINDING
#define WC_RSA_PSS

/* DH - on by default */
#define WOLFSSL_DH_CONST
#define HAVE_FFDHE_2048

/* AES */
#define HAVE_AES_DECRYPT
#define HAVE_AESGCM
#define GCM_SMALL
#define HAVE_AESCCM
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_DIRECT

/* Hashing */
#define HAVE_SHA384
#define HAVE_SHA512
#define HAVE_HKDF

/* TLS */
#if 0
    /* TLS v1.3 only */
    #define WOLFSSL_TLS13
    #define WOLFSSL_NO_TLS12
#else
    /* TLS v1.2 only */
#endif
#define NO_OLD_TLS
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES

/* Disable Features */
#define NO_WRITEV
#define NO_FILESYSTEM
#define NO_MAIN_DRIVER
//#define NO_ERROR_STRINGS

/* Disable Algorithms */
#define NO_DES3
#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_SHA
#define NO_PKCS12

/* helpers */
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#define ntohl(x) __builtin_bswap32(x)
#define htonl(x) __builtin_bswap32(x)

#endif /* !IOTSAFE_EXAMPLE_USER_SETTINGS_H */
