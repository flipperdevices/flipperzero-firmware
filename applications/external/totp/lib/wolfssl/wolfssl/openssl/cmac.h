/* cmac.h
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

#ifndef WOLFSSL_CMAC_H_
#define WOLFSSL_CMAC_H_

#include <wolfssl/wolfcrypt/cmac.h>
#include <wolfssl/openssl/compat_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WOLFSSL_CMAC_CTX {
    void* internal; /* internal Cmac object */
    WOLFSSL_EVP_CIPHER_CTX* cctx;
} WOLFSSL_CMAC_CTX;

typedef WOLFSSL_CMAC_CTX CMAC_CTX;

WOLFSSL_API WOLFSSL_CMAC_CTX* wolfSSL_CMAC_CTX_new(void);
WOLFSSL_API void wolfSSL_CMAC_CTX_free(WOLFSSL_CMAC_CTX *ctx);
WOLFSSL_API WOLFSSL_EVP_CIPHER_CTX* wolfSSL_CMAC_CTX_get0_cipher_ctx(
    WOLFSSL_CMAC_CTX* ctx);
WOLFSSL_API int wolfSSL_CMAC_Init(
    WOLFSSL_CMAC_CTX* ctx, const void *key, size_t keyLen,
    const WOLFSSL_EVP_CIPHER* cipher, WOLFSSL_ENGINE* engine);
WOLFSSL_API int wolfSSL_CMAC_Update(
    WOLFSSL_CMAC_CTX* ctx, const void* data, size_t len);
WOLFSSL_API int wolfSSL_CMAC_Final(
    WOLFSSL_CMAC_CTX* ctx, unsigned char* out, size_t* len);

#define CMAC_CTX_new              wolfSSL_CMAC_CTX_new
#define CMAC_CTX_free             wolfSSL_CMAC_CTX_free
#define CMAC_CTX_get0_cipher_ctx  wolfSSL_CMAC_CTX_get0_cipher_ctx
#define CMAC_Init                 wolfSSL_CMAC_Init
#define CMAC_Update               wolfSSL_CMAC_Update
#define CMAC_Final                wolfSSL_CMAC_Final

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* WOLFSSL_CMAC_H_ */
