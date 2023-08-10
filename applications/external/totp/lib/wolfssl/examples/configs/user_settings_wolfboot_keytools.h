/* user_settings_wolfboot_keytools.h
 *
 * wolfCrypt build settings for wolfBoot keygen and signing tool
 * Enabled via WOLFSSL_USER_SETTINGS.
 *
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

#ifndef H_USER_SETTINGS_
#define H_USER_SETTINGS_

/* #include <stdint.h> */ /* DG: Removed, not needed for testing */

/* System */
#define WOLFSSL_GENERAL_ALIGNMENT 4
#define SINGLE_THREADED
#define WOLFCRYPT_ONLY
#define SIZEOF_LONG_LONG 8

/* Math */
#define USE_FAST_MATH
#define FP_MAX_BITS (4096 * 2)
#define TFM_TIMING_RESISTANT

/* ECC */
#define HAVE_ECC
#define WOLFSSL_HAVE_SP_ECC
#define ECC_TIMING_RESISTANT
#define HAVE_ECC256
#define HAVE_ECC384
#define HAVE_ECC521

/* ED25519 */
#define HAVE_ED25519
/* DG: Added, since --enable-usersettings expects small version to be used */
#define ED25519_SMALL

/* ED448 */
#define HAVE_ED448
#define WOLFSSL_SHAKE256

/* RSA */
#define HAVE_RSA
#define WOLFSSL_HAVE_SP_RSA
#define WC_RSA_BLINDING
#define WOLFSSL_KEY_GEN

/* Hashing */
#define WOLFSSL_SHA512 /* Required for ED25519 */
#define WOLFSSL_SHA384
#define WOLFSSL_SHA3
#undef  NO_SHA256

/* Chacha stream cipher */
#define HAVE_CHACHA

/* AES */
#define WOLFSSL_AES_COUNTER
#define WOLFSSL_AES_DIRECT

/* Disables */
#define NO_CMAC
#define NO_HMAC
#define NO_RC4
#define NO_SHA
#define NO_DH
#define NO_DSA
#define NO_MD4
#define NO_RABBIT
#define NO_MD5
#define NO_SIG_WRAPPER
#define NO_CERT
#define NO_SESSION_CACHE
#define NO_HC128
#define NO_DES3
#define NO_PWDBASED
#define NO_WRITEV
#define NO_FILESYSTEM
#define NO_OLD_RNGNAME
#define NO_WOLFSSL_DIR
#define WOLFSSL_NO_SOCK
#define WOLFSSL_IGNORE_FILE_WARN
#define NO_ERROR_STRINGS

#define BENCH_EMBEDDED

/* DG: Removed since we need it here for testing */
/* #define NO_MAIN_DRIVER */
/* #define NO_CRYPT_TEST */
/* #define NO_CRYPT_BENCHMARK */

#endif /* !H_USER_SETTINGS_ */
