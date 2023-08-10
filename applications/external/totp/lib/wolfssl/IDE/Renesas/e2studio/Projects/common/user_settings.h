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
#define NO_WRITEV
#define WOLFSSL_USER_IO
#define NO_DEV_RANDOM
#define USE_CERT_BUFFERS_2048
#define WOLFSSL_USER_CURRTIME
#define SIZEOF_LONG_LONG 8
#define NO_WOLFSSL_DIR 
#define WOLFSSL_NO_CURRDIR
#define NO_FILESYSTEM
#define WOLFSSL_LOG_PRINTF
#define WOLFSSL_HAVE_MIN
#define WOLFSSL_HAVE_MAX
#if !defined(min)
    #define min(data1, data2)                _builtin_min(data1, data2)
#endif

#define WOLFSSL_SMALL_STACK
#define WOLFSSL_DH_CONST
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES

#define USER_TIME
#define XTIME time
#define USE_WOLF_SUSECONDS_T
#define USE_WOLF_TIMEVAL_T

#define WOLFSSL_USER_CURRTIME /* for benchmark */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

#define SINGLE_THREADED  /* or define RTOS  option */
#ifndef SINGLE_THREADED
    /* expects use FRERRTOS */
    #define FREERTOS
#endif

/* #define NO_DH */
#define HAVE_AESGCM
#define WOLFSSL_SHA512
#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#define HAVE_ED25519

/* #define NO_WOLFSSL_STUB */
/* #define DEBUG_WOLFSSL */
/* TSIP TLS specific definitions */
#if !defined(WOLFSSL_RENESAS_TSIP)
    #define OPENSSL_EXTRA
    #define WOLFSSL_GENSEED_FORTEST /* Warning: define your own seed gen */
#else
#if defined(WOLFSSL_RENESAS_RX65N)
    /* In the case of Static RSA and AES-CBC use */
    #define HAVE_AES_CBC
    /* TSIP ver 1.0.6 or lower supports only static RSA as Public-key cryptography. */
    #define WOLFSSL_STATIC_RSA
    /* for disabling TSIP CRYPT and TSIP TLS */
    /* #define NO_RENESAS_TSIP_CRYPT */
    /* for disabling only TSIP TLS-linked Common key encryption method */
    /* #define NO_WOLFSSL_RENESAS_TSIP_TLS_SESSION */
#endif
#endif
