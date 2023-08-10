/* x509_vfy.h
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

/* x509_vfy.h for openSSL */

#ifndef WOLFSSL_x509_vfy_H_
#define WOLFSSL_x509_vfy_H_

#include <wolfssl/openssl/compat_types.h>
#include <wolfssl/openssl/x509v3.h>

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(WOLFSSL_QT) || defined(OPENSSL_ALL)
    WOLFSSL_API int wolfSSL_X509_STORE_CTX_set_purpose(WOLFSSL_X509_STORE_CTX *ctx, int purpose);
    WOLFSSL_API void wolfSSL_X509_STORE_CTX_set_flags(WOLFSSL_X509_STORE_CTX *ctx,
        unsigned long flags);
#endif

#define X509_STORE_CTX_set_purpose  wolfSSL_X509_STORE_CTX_set_purpose
#define X509_STORE_CTX_set_flags    wolfSSL_X509_STORE_CTX_set_flags

#ifdef  __cplusplus
} /* extern "C" */
#endif

#endif /* WOLFSSL_x509_vfy_H_ */
