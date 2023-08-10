/* user_settings_fipsv5.h
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

/* should be renamed to user_settings.h for customer use
 * generated from configure options:
 * ./fips-check.sh linuxv5-dev keep
 * XXX-fips-test\wolfssl\options.h
 * ./configure --enable-fips=v5-dev
 *
 * Cleaned up by David Garske
 */


#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* FIPS Version 5.3 */
#define HAVE_FIPS
#define HAVE_FIPS_VERSION 5
#define HAVE_FIPS_VERSION_MINOR 3

#define HAVE_HASHDRBG /* NIST Certified DRBG - SHA256 based */
#define HAVE_THREAD_LS

/* Math */
#define USE_FAST_MATH
#define FP_MAX_BITS 16384
#define SP_INT_BITS 8192
#define WOLFSSL_PUBLIC_MP /* expose the mp_ math API's */

/* Timing Resistance */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* FIPS Features */
#define WC_RNG_SEED_CB
#define WOLFSSL_VALIDATE_ECC_IMPORT
#define WOLFSSL_VALIDATE_ECC_KEYGEN
#define WOLFSSL_VALIDATE_FFC_IMPORT

/* TLS Features */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define HAVE_ENCRYPT_THEN_MAC
#define HAVE_SUPPORTED_CURVES
#define HAVE_EXTENDED_MASTER
#define HAVE_ONE_TIME_AUTH /* TLS extension used with Poly1305 */

/* TLS Resumption */
#define HAVE_SECURE_RENEGOTIATION
#define HAVE_SERVER_RENEGOTIATION_INFO
#define HAVE_SESSION_TICKET

/* Other Features */
#define WOLFSSL_USE_ALIGN
#define WOLFSSL_BASE64_ENCODE
#if 0 /* optionally enable CRL (Certificate Revocation List) */
    #define HAVE_CRL
#endif
#define HAVE_EXT_CACHE
#define WOLFSSL_VERIFY_CB_ALL_CERTS
#define WOLFSSL_ALWAYS_VERIFY_CB
#define WOLFSSL_DH_EXTRA /* DHE ASN.1 key import/export support */

#ifndef WOLFSSL_WOLFSSH /* ifndef check required due to configure already generating flag */
#define WOLFSSL_WOLFSSH
#endif

/* Compatibility Layer */
#define OPENSSL_EXTRA
#if 0 /* if full suite of compatibility API's are needed */
    #define OPENSSL_ALL
#endif

/* DH */
#undef  NO_DH
#define HAVE_FFDHE_Q
#define HAVE_FFDHE_2048
#define HAVE_FFDHE_3072
#define HAVE_FFDHE_4096
#define HAVE_FFDHE_6144
#define HAVE_FFDHE_8192
#define HAVE_DH_DEFAULT_PARAMS
#define HAVE_PUBLIC_FFDHE

/* ECC */
#define HAVE_ECC
#define TFM_ECC256
#define ECC_SHAMIR
#define HAVE_ECC_CDH
#define ECC_USER_CURVES
#define HAVE_ECC192
#define HAVE_ECC224
#define HAVE_ECC256
#define HAVE_ECC384
#define HAVE_ECC521
#define WOLFSSL_ECDSA_SET_K

/* RSA */
#undef  NO_RSA
#define WC_RSA_PSS
#define WOLFSSL_KEY_GEN
#define WC_RSA_NO_PADDING

/* AES */
#undef  NO_AES
#define WOLFSSL_AES_COUNTER
#define HAVE_AESCCM
#define HAVE_AES_ECB
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_DIRECT
#define WOLFSSL_AES_OFB
#define HAVE_AESGCM
#define GCM_TABLE_4BIT
#define WOLFSSL_CMAC

/* ChaCha/Poly */
#define HAVE_CHACHA
#define HAVE_POLY1305

/* Hashing */
#undef  NO_SHA
#undef  NO_SHA256
#define WOLFSSL_SHA224
#define WOLFSSL_SHA512
#define WOLFSSL_SHA384
#define WOLFSSL_NO_SHAKE256
#define WOLFSSL_NOSHA512_224
#define WOLFSSL_NOSHA512_256
#define WOLFSSL_SHA3
#define HAVE_HKDF

/* Disabled Features */
#define NO_OLD_TLS
#define NO_PSK

/* Disabled Algorithms */
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_DES3
#define NO_DSA
#define NO_RABBIT
#define NO_HC128
#define WOLFSSL_NO_SHAKE256

/* Debugging */
#if 0
    #define DEBUG_WOLFSSL
#else
    /* #define NO_ERROR_STRINGS */
#endif

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */
