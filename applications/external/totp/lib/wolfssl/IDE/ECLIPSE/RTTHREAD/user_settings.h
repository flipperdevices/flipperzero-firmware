/* user_setting.h
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

#ifndef WOLFSSL_USER_SETTINGS_H_
#define WOLFSSL_USER_SETTINGS_H_

#ifdef __cplusplus
    extern "C" {
#endif

#define RTTHREAD

/* You can select one or all of the following tests */
#define WOLFSSL_WOLFCRYPT_TEST
#define WOLFSSL_BENCHMARK_TEST
#define WOLFSSL_CLIENT_TEST
#define WOLFSSL_SERVER_TEST
#define USE_TEST_GENSEED
#define NO_DEV_RANDOM
#define HAVE_PKCS7
#define HAVE_AES_KEYWRAP
#define HAVE_X963_KDF
#define WOLFSSL_AES_DIRECT
/* adjust CURRENT_UNIX_TS to seconds since Jan 01 1970. (UTC)
You can get the current time from https://www.unixtimestamp.com/
*/
#define CURRENT_UNIX_TS 1542605837UL

/* When using Windows simulator, you must define USE_WINDOWS_API for test.h to build */
#ifdef _WIN32
#define USE_WINDOWS_API
#endif

#define NO_FILESYSTEM
#define SIZEOF_LONG_LONG 8

/* prevents from including multiple definition of main() */
#define NO_MAIN_DRIVER
#define NO_TESTSUITE_MAIN_DRIVER

/* includes certificate test buffers via header files */
#define USE_CERT_BUFFERS_2048
/*use kB instead of mB for embedded benchmarking*/
#define BENCH_EMBEDDED

#define NO_WRITE_TEMP_FILES

#define XSNPRINTF snprintf
#define NO_WRITEV

#define HAVE_AESGCM
#define WOLFSSL_SHA512
#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#define HAVE_ED25519
#define ED25519_SMALL

#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif
