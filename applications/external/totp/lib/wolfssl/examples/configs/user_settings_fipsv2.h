/* user_settings_fipsv2.h
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
 * ./fips-check.sh linuxv2 keep
 * XXX-fips-test\wolfssl\options.h
 *
 * Cleaned up by David Garske
 */


#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* FIPS Version 3 (3389 Certificate) */
#define HAVE_FIPS
#define HAVE_FIPS_VERSION 2

#define HAVE_HASHDRBG /* NIST Certified DRBG - SHA256 based */
#define HAVE_THREAD_LS

/* Math */
#define USE_FAST_MATH

/* Timing Resistance */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* TLS Features */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_EXTENDED_MASTER
#define HAVE_ENCRYPT_THEN_MAC

/* DH */
#undef  NO_DH
#define HAVE_FFDHE_2048
#define HAVE_FFDHE_Q
#define WOLFSSL_VALIDATE_ECC_IMPORT
#define WOLFSSL_VALIDATE_FFC_IMPORT
#define HAVE_DH_DEFAULT_PARAMS

/* ECC */
#define HAVE_ECC
#define TFM_ECC256
#define ECC_SHAMIR
#define HAVE_ECC_CDH

/* RSA */
#undef  NO_RSA
#define WC_RSA_PSS
#define WOLFSSL_KEY_GEN
#define WC_RSA_NO_PADDING

/* AES */
#define WOLFSSL_AES_DIRECT
#define HAVE_AES_ECB
#define HAVE_AESGCM
#define GCM_TABLE_4BIT
#define HAVE_AESCCM
#define WOLFSSL_AES_COUNTER

/* Hashing */
#undef  NO_SHA
#undef  NO_SHA256
#define WOLFSSL_SHA224
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define WOLFSSL_SHA3
#define HAVE_HKDF

/* Other */
#define WOLFSSL_CMAC
#define WOLFSSL_BASE64_ENCODE

/* Disabled Algorithms */
#define NO_DSA
#define NO_MD4
#define NO_PSK
#define NO_PWDBASED
#define NO_RC4
#define WOLFSSL_NO_SHAKE256
#define NO_RABBIT
#define NO_HC128

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_OPTIONS_H */
