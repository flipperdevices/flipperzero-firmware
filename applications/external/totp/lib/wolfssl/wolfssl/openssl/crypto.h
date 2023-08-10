/* crypto.h
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

/* crypto.h for openSSL */

#ifndef WOLFSSL_CRYPTO_H_
#define WOLFSSL_CRYPTO_H_

#include <wolfssl/openssl/compat_types.h>

typedef struct WOLFSSL_INIT_SETTINGS {
    char* appname;
} WOLFSSL_INIT_SETTINGS;
typedef WOLFSSL_INIT_SETTINGS OPENSSL_INIT_SETTINGS;

typedef struct WOLFSSL_CRYPTO_THREADID {
    int dummy;
} WOLFSSL_CRYPTO_THREADID;
typedef struct crypto_threadid_st   CRYPTO_THREADID;

typedef struct CRYPTO_EX_DATA            CRYPTO_EX_DATA;

#ifdef HAVE_EX_DATA
typedef WOLFSSL_CRYPTO_EX_new CRYPTO_new_func;
typedef WOLFSSL_CRYPTO_EX_dup CRYPTO_dup_func;
typedef WOLFSSL_CRYPTO_EX_free CRYPTO_free_func;
#endif

#include <wolfssl/openssl/opensslv.h>
#include <wolfssl/openssl/conf.h>

#ifdef WOLFSSL_PREFIX
#include "prefix_crypto.h"
#endif

#ifdef __cplusplus
    extern "C" {
#endif

WOLFSSL_API const char*   wolfSSLeay_version(int type);
WOLFSSL_API unsigned long wolfSSLeay(void);
WOLFSSL_API unsigned long wolfSSL_OpenSSL_version_num(void);

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
WOLFSSL_API void wolfSSL_OPENSSL_free(void* p);
#endif

#ifdef OPENSSL_EXTRA
WOLFSSL_API void *wolfSSL_OPENSSL_malloc(size_t a);
WOLFSSL_API int wolfSSL_OPENSSL_hexchar2int(unsigned char c);
WOLFSSL_API unsigned char *wolfSSL_OPENSSL_hexstr2buf(const char *str, long *len);

WOLFSSL_API int wolfSSL_OPENSSL_init_crypto(word64 opts, const OPENSSL_INIT_SETTINGS *settings);
#endif

/* class index for wolfSSL_CRYPTO_get_ex_new_index */
#define CRYPTO_EX_INDEX_SSL             WOLF_CRYPTO_EX_INDEX_SSL
#define CRYPTO_EX_INDEX_SSL_CTX         WOLF_CRYPTO_EX_INDEX_SSL_CTX
#define CRYPTO_EX_INDEX_SSL_SESSION     WOLF_CRYPTO_EX_INDEX_SSL_SESSION
#define CRYPTO_EX_INDEX_X509            WOLF_CRYPTO_EX_INDEX_X509
#define CRYPTO_EX_INDEX_X509_STORE      WOLF_CRYPTO_EX_INDEX_X509_STORE
#define CRYPTO_EX_INDEX_X509_STORE_CTX  WOLF_CRYPTO_EX_INDEX_X509_STORE_CTX
#define CRYPTO_EX_INDEX_DH              WOLF_CRYPTO_EX_INDEX_DH
#define CRYPTO_EX_INDEX_DSA             WOLF_CRYPTO_EX_INDEX_DSA
#define CRYPTO_EX_INDEX_EC_KEY          WOLF_CRYPTO_EX_INDEX_EC_KEY
#define CRYPTO_EX_INDEX_RSA             WOLF_CRYPTO_EX_INDEX_RSA
#define CRYPTO_EX_INDEX_ENGINE          WOLF_CRYPTO_EX_INDEX_ENGINE
#define CRYPTO_EX_INDEX_UI              WOLF_CRYPTO_EX_INDEX_UI
#define CRYPTO_EX_INDEX_BIO             WOLF_CRYPTO_EX_INDEX_BIO
#define CRYPTO_EX_INDEX_APP             WOLF_CRYPTO_EX_INDEX_APP
#define CRYPTO_EX_INDEX_UI_METHOD       WOLF_CRYPTO_EX_INDEX_UI_METHOD
#define CRYPTO_EX_INDEX_DRBG            WOLF_CRYPTO_EX_INDEX_DRBG
#define CRYPTO_EX_INDEX__COUNT          WOLF_CRYPTO_EX_INDEX__COUNT

#define crypto_threadid_st          WOLFSSL_CRYPTO_THREADID
#define CRYPTO_THREADID             WOLFSSL_CRYPTO_THREADID

#define SSLeay_version wolfSSLeay_version
#define SSLeay wolfSSLeay
#define OpenSSL_version_num wolfSSL_OpenSSL_version_num

#if defined(WOLFSSL_QT) || defined(WOLFSSL_HITCH)
    #define SSLEAY_VERSION 0x10001000L
#else
    #define SSLEAY_VERSION 0x0090600fL
#endif
#define SSLEAY_VERSION_NUMBER SSLEAY_VERSION
#define CRYPTO_lock wc_LockMutex_ex

/* this function was used to set the default malloc, free, and realloc */
#define CRYPTO_malloc_init() 0 /* CRYPTO_malloc_init is not needed */

#define OPENSSL_free wolfSSL_OPENSSL_free
#define OPENSSL_malloc wolfSSL_OPENSSL_malloc
#define OPENSSL_hexchar2int wolfSSL_OPENSSL_hexchar2int
#define OPENSSL_hexstr2buf wolfSSL_OPENSSL_hexstr2buf

#define OPENSSL_INIT_ENGINE_ALL_BUILTIN 0x00000001L
#define OPENSSL_INIT_ADD_ALL_CIPHERS    0x00000004L
#define OPENSSL_INIT_ADD_ALL_DIGESTS    0x00000008L
#define OPENSSL_INIT_LOAD_CONFIG        0x00000040L

#define OPENSSL_init_crypto wolfSSL_OPENSSL_init_crypto

#ifdef WOLFSSL_OPENVPN
# define OPENSSL_assert(e) \
    if (!(e)) { \
        fprintf(stderr, "%s:%d wolfSSL internal error: assertion failed: " #e, \
                __FILE__, __LINE__); \
        raise(SIGABRT); \
        _exit(3); \
    }
#endif

#if defined(OPENSSL_ALL) || defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA) || defined(HAVE_EX_DATA)
#define CRYPTO_set_mem_ex_functions      wolfSSL_CRYPTO_set_mem_ex_functions
#define FIPS_mode                        wolfSSL_FIPS_mode
#define FIPS_mode_set                    wolfSSL_FIPS_mode_set
#define CRYPTO_THREADID_set_callback wolfSSL_THREADID_set_callback
#define CRYPTO_THREADID_set_numeric wolfSSL_THREADID_set_numeric
#define CRYPTO_THREADID_current      wolfSSL_THREADID_current
#define CRYPTO_THREADID_hash         wolfSSL_THREADID_hash

#define CRYPTO_r_lock wc_LockMutex_ex
#define CRYPTO_unlock wc_LockMutex_ex

#define CRYPTO_THREAD_lock wc_LockMutex
#define CRYPTO_THREAD_r_lock wc_LockMutex
#define CRYPTO_THREAD_unlock wc_UnLockMutex

#define CRYPTO_THREAD_lock_new wc_InitAndAllocMutex
#define CRYPTO_THREAD_read_lock wc_LockMutex
#define CRYPTO_THREAD_write_lock wc_LockMutex
#define CRYPTO_THREAD_lock_free wc_FreeMutex

#define CRYPTO_get_ex_data wolfSSL_CRYPTO_get_ex_data
#define CRYPTO_set_ex_data wolfSSL_CRYPTO_set_ex_data

#endif /* OPENSSL_ALL || HAVE_STUNNEL || WOLFSSL_NGINX || WOLFSSL_HAPROXY || HAVE_EX_DATA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* header */
