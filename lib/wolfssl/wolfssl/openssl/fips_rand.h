/* fips_rand.h
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

/* fips_rand.h for openSSL compatibility */

#ifndef WOLFSSL_OPENSSL_FIPS_RAND_H_
#define WOLFSSL_OPENSSL_FIPS_RAND_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h>

#if !defined(WC_NO_RNG) && defined(HAVE_HASHDRBG)

struct WOLFSSL_DRBG_CTX;

typedef size_t (*drbg_entropy_get)(struct WOLFSSL_DRBG_CTX* ctx, unsigned char** pout,
                                  int entropy, size_t min_len, size_t max_len);
typedef void   (*drbg_entropy_clean)(struct WOLFSSL_DRBG_CTX* ctx, unsigned char* out,
                                    size_t olen);
typedef size_t (*drbg_nonce_get)(struct WOLFSSL_DRBG_CTX* ctx, unsigned char** pout,
                                int entropy, size_t min_len, size_t max_len);
typedef void   (*drbg_nonce_clean)(struct WOLFSSL_DRBG_CTX* ctx, unsigned char* out,
                                  size_t olen);

typedef struct WOLFSSL_DRBG_CTX {
    WC_RNG* rng;
    drbg_entropy_get entropy_get;
    drbg_entropy_clean entropy_clean;
    size_t entropy_blocklen;
    drbg_nonce_get none_get;
    drbg_nonce_clean nonce_clean;

    int type;
    int status;
    int xflags;
    void* app_data;
} WOLFSSL_DRBG_CTX;

#define DRBG_FLAG_CTR_USE_DF 0x1
#define DRBG_FLAG_TEST       0x2

#define DRBG_FLAG_NOERR      0x1
#define DRBG_CUSTOM_RESEED   0x2

#define DRBG_STATUS_UNINITIALISED 0
#define DRBG_STATUS_READY         1
#define DRBG_STATUS_RESEED        2
#define DRBG_STATUS_ERROR         3

WOLFSSL_API WOLFSSL_DRBG_CTX* wolfSSL_FIPS_drbg_new(int type,
    unsigned int flags);

WOLFSSL_API int wolfSSL_FIPS_drbg_init(WOLFSSL_DRBG_CTX *ctx,
    int type, unsigned int flags);

WOLFSSL_API int  wolfSSL_FIPS_drbg_instantiate(WOLFSSL_DRBG_CTX* ctx,
    const unsigned char* pers, size_t perslen);

WOLFSSL_API int  wolfSSL_FIPS_drbg_set_callbacks(WOLFSSL_DRBG_CTX* ctx,
    drbg_entropy_get entropy_get, drbg_entropy_clean entropy_clean,
    size_t entropy_blocklen,
    drbg_nonce_get none_get, drbg_nonce_clean nonce_clean);

WOLFSSL_API void wolfSSL_FIPS_rand_add(const void* buf, int num,
    double entropy);
WOLFSSL_API int  wolfSSL_FIPS_drbg_reseed(WOLFSSL_DRBG_CTX* ctx,
    const unsigned char* adin, size_t adinlen);

WOLFSSL_API int  wolfSSL_FIPS_drbg_generate(WOLFSSL_DRBG_CTX* ctx,
    unsigned char* out, size_t outlen, int prediction_resistance,
    const unsigned char* adin, size_t adinlen);

WOLFSSL_API int  wolfSSL_FIPS_drbg_uninstantiate(WOLFSSL_DRBG_CTX *ctx);

WOLFSSL_API void wolfSSL_FIPS_drbg_free(WOLFSSL_DRBG_CTX *ctx);

WOLFSSL_API WOLFSSL_DRBG_CTX* wolfSSL_FIPS_get_default_drbg(void);

WOLFSSL_API void wolfSSL_FIPS_get_timevec(unsigned char* buf,
    unsigned long* pctr);

WOLFSSL_API void* wolfSSL_FIPS_drbg_get_app_data(WOLFSSL_DRBG_CTX *ctx);

WOLFSSL_API void  wolfSSL_FIPS_drbg_set_app_data(WOLFSSL_DRBG_CTX *ctx,
    void *app_data);


/* compatibility mapping */
typedef WOLFSSL_DRBG_CTX DRBG_CTX;

#define FIPS_drbg_init           wolfSSL_FIPS_drbg_init
#define FIPS_drbg_new            wolfSSL_FIPS_drbg_new
#define FIPS_drbg_instantiate    wolfSSL_FIPS_drbg_instantiate
#define FIPS_drbg_set_callbacks  wolfSSL_FIPS_drbg_set_callbacks
#define FIPS_rand_add            wolfSSL_FIPS_rand_add
#define FIPS_drbg_reseed         wolfSSL_FIPS_drbg_reseed
#define FIPS_drbg_generate       wolfSSL_FIPS_drbg_generate
#define FIPS_drbg_uninstantiate  wolfSSL_FIPS_drbg_uninstantiate
#define FIPS_drbg_free           wolfSSL_FIPS_drbg_free
#define FIPS_get_default_drbg    wolfSSL_FIPS_get_default_drbg
#define FIPS_get_timevec         wolfSSL_FIPS_get_timevec
#define FIPS_drbg_get_app_data   wolfSSL_FIPS_drbg_get_app_data
#define FIPS_drbg_set_app_data   wolfSSL_FIPS_drbg_set_app_data

#endif /* !WC_NO_RNG && HAVE_HASHDRBG */

#endif /* WOLFSSL_OPENSSL_FIPS_RAND_H_ */
