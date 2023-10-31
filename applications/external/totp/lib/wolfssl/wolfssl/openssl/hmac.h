/* hmac.h
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



/*  hmac.h defines mini hmac openssl compatibility layer
 *
 */


#ifndef WOLFSSL_HMAC_H_
#define WOLFSSL_HMAC_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_PREFIX
#include "prefix_hmac.h"
#endif

#include <wolfssl/openssl/compat_types.h>
#include <wolfssl/openssl/opensslv.h>

#ifdef __cplusplus
    extern "C" {
#endif


WOLFSSL_API unsigned char* wolfSSL_HMAC(const WOLFSSL_EVP_MD* evp_md,
                               const void* key, int key_len,
                               const unsigned char* d, int n, unsigned char* md,
                               unsigned int* md_len);

WOLFSSL_API WOLFSSL_HMAC_CTX* wolfSSL_HMAC_CTX_new(void);
WOLFSSL_API int wolfSSL_HMAC_CTX_Init(WOLFSSL_HMAC_CTX* ctx);
WOLFSSL_API int wolfSSL_HMAC_CTX_copy(WOLFSSL_HMAC_CTX* des,
                                       WOLFSSL_HMAC_CTX* src);
WOLFSSL_LOCAL int wolfSSL_HmacCopy(Hmac* des, Hmac* src);
WOLFSSL_API int wolfSSL_HMAC_Init(WOLFSSL_HMAC_CTX* ctx, const void* key,
                                 int keylen, const WOLFSSL_EVP_MD* type);
WOLFSSL_API int wolfSSL_HMAC_Init_ex(WOLFSSL_HMAC_CTX* ctx, const void* key,
                             int keylen, const WOLFSSL_EVP_MD* type, WOLFSSL_ENGINE* e);
WOLFSSL_API int wolfSSL_HMAC_Update(WOLFSSL_HMAC_CTX* ctx,
                                   const unsigned char* data, int len);
WOLFSSL_API int wolfSSL_HMAC_Final(WOLFSSL_HMAC_CTX* ctx, unsigned char* hash,
                                  unsigned int* len);
WOLFSSL_API int wolfSSL_HMAC_cleanup(WOLFSSL_HMAC_CTX* ctx);
WOLFSSL_API void wolfSSL_HMAC_CTX_cleanup(WOLFSSL_HMAC_CTX* ctx);
WOLFSSL_API void wolfSSL_HMAC_CTX_free(WOLFSSL_HMAC_CTX* ctx);
WOLFSSL_API size_t wolfSSL_HMAC_size(const WOLFSSL_HMAC_CTX *ctx);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_HMAC_CTX_get_md(const WOLFSSL_HMAC_CTX *ctx);

typedef struct WOLFSSL_HMAC_CTX HMAC_CTX;

#define HMAC(a,b,c,d,e,f,g) wolfSSL_HMAC((a),(b),(c),(d),(e),(f),(g))

#define HMAC_CTX_new wolfSSL_HMAC_CTX_new
#define HMAC_CTX_init wolfSSL_HMAC_CTX_Init
#define HMAC_CTX_copy wolfSSL_HMAC_CTX_copy
#define HMAC_CTX_free wolfSSL_HMAC_CTX_free
#define HMAC_CTX_cleanup wolfSSL_HMAC_CTX_cleanup
#define HMAC_CTX_reset wolfSSL_HMAC_cleanup
#define HMAC_Init_ex  wolfSSL_HMAC_Init_ex
#define HMAC_Init     wolfSSL_HMAC_Init
#define HMAC_Update   wolfSSL_HMAC_Update
#define HMAC_Final    wolfSSL_HMAC_Final
#define HMAC_cleanup  wolfSSL_HMAC_cleanup
#define HMAC_size     wolfSSL_HMAC_size
#define HMAC_CTX_get_md wolfSSL_HMAC_CTX_get_md


#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* WOLFSSL_HMAC_H_ */
