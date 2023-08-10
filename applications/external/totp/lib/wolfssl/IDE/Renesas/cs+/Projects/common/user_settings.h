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
#define NO_DEV_RANDOM
#define USE_CERT_BUFFERS_2048
#define SIZEOF_LONG_LONG 8
#define NO_WOLFSSL_DIR 
#define WOLFSSL_NO_CURRDIR
#define WOLFSSL_LOG_PRINTF
#define NO_WOLFSSL_STUB
#define NO_DYNAMIC_ARRAY       /* for compilers not allowed dynamic size array */ 
#define WOLFSSL_SMALL_STACK
#define WOLFSSL_DH_CONST

#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES

#define WOLFSSL_USER_IO

//#define WOLFSSL_USER_KEYFILES /* To be defined key and cert files in user_settings.h  */
//#define WOLFSSL_NO_ABORT      /* No abort during the test except exception occurred */
//#define DEBUG_WOLFSSL

#define OPENSSL_EXTRA

#define USER_TIME
#define XTIME time
#define USE_WOLF_SUSECONDS_T
#define USE_WOLF_TIMEVAL_T

#define WOLFSSL_USER_CURRTIME /* for benchmark */

#define WOLFSSL_GENSEED_FORTEST /* Wardning: define your own seed gen */

#define SINGLE_THREADED  /* or define RTOS  option */
/*#define WOLFSSL_CMSIS_RTOS */

/* #define NO_DH */
#define NO_RC4
#define HAVE_AESGCM
#define WOLFSSL_SHA512
#define WOLFSSL_SHA384
#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#define HAVE_ED25519
#define NO_OLD_SHA256_NAMES
#define HAVE_CRL
#define HAVE_OCSP
#define HAVE_CERTIFICATE_STATUS_REQUEST

#define NO_FILESYSTEM


