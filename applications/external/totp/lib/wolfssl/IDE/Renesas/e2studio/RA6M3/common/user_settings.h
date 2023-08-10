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
#ifndef USER_SETTINGS_H_
#define USER_SETTINGS_H_


/* Temporary defines. Not suitable for production. */
#define WOLFSSL_GENSEED_FORTEST /* Warning: define your own seed gen */
/* End temporary defines */

/* TLS 1.3 */
#define WOLFSSL_TLS13
#define HAVE_HKDF
#define WC_RSA_PSS
#if defined(WOLFSSL_TLS13)
    #include <sys/time.h>
#endif
/* Operating Environment and Threading */
#define FREERTOS
#define FREERTOS_TCP
#define NO_DEV_RANDOM
#define NO_WRITEV
#define NO_MAIN_DRIVER
#define BENCH_EMBEDDED

/* Filesystem and IO */
#define NO_WOLFSSL_DIR
#define WOLFSSL_NO_CURRDIR
#define NO_FILESYSTEM

/* Cryptography Enable Options */
#define HAVE_CHACHA
#define HAVE_POLY1305
#define HAVE_ECC
#define HAVE_RSA
#define HAVE_SHA256
#define HAVE_SUPPORTED_CURVES
#define HAVE_TLS_EXTENSIONS
#define HAVE_TRUNCATED_HMAC
#define HAVE_EXTENDED_MASTER
#define HAVE_ALPN
#define HAVE_SNI
#define HAVE_OCSP
#define HAVE_AESGCM
#define HAVE_ONE_TIME_AUTH

/* Non-Fast Math may call realloc. This project has no realloc support */
#define USE_FAST_MATH
#define ALT_ECC_SIZE
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING
#define WOLFSSL_SMALL_STACK
#define WOLFSSL_DH_CONST

/* Cryptography Disable options */
#define NO_PWDBASED
#define NO_DSA
#define NO_DES3
#define NO_RC4
#define NO_MD4

void wolfssl_thread_entry(void *pvParameters);
extern void initialise_monitor_handles(void);
int strncasecmp(const char *s1, const char * s2, unsigned int sz);

#endif /* USER_SETTINGS_H_ */
