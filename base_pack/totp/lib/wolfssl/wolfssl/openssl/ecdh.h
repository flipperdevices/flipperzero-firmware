/* ecdh.h
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

/* ecdh.h for openssl */

#ifndef WOLFSSL_ECDH_H_
#define WOLFSSL_ECDH_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/openssl/bn.h>

#ifdef __cplusplus
extern "C" {
#endif


WOLFSSL_API int wolfSSL_ECDH_compute_key(void *out, size_t outlen,
                                         const WOLFSSL_EC_POINT *pub_key,
                                         WOLFSSL_EC_KEY *ecdh,
                                         void *(*KDF) (const void *in,
                                                       size_t inlen,
                                                       void *out,
                                                       size_t *outlen));

#define ECDH_compute_key wolfSSL_ECDH_compute_key

#ifdef __cplusplus
}  /* extern C */
#endif

#endif /* header */
