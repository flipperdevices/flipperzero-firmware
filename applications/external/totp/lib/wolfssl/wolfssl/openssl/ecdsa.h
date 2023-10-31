/* ecdsa.h
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

/* ecdsa.h for openssl */

#ifndef WOLFSSL_ECDSA_H_
#define WOLFSSL_ECDSA_H_

#include <wolfssl/openssl/bn.h>
#include <wolfssl/openssl/ec.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef WOLFSSL_ECDSA_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_ECDSA_SIG      WOLFSSL_ECDSA_SIG;
#define WOLFSSL_ECDSA_TYPE_DEFINED
#endif

typedef WOLFSSL_ECDSA_SIG             ECDSA_SIG;

struct WOLFSSL_ECDSA_SIG {
    WOLFSSL_BIGNUM *r;
    WOLFSSL_BIGNUM *s;
};

WOLFSSL_API void wolfSSL_ECDSA_SIG_free(WOLFSSL_ECDSA_SIG *sig);
WOLFSSL_API WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_SIG_new(void);
WOLFSSL_API void wolfSSL_ECDSA_SIG_get0(const WOLFSSL_ECDSA_SIG* sig,
    const WOLFSSL_BIGNUM** r, const WOLFSSL_BIGNUM** s);
WOLFSSL_API int wolfSSL_ECDSA_SIG_set0(WOLFSSL_ECDSA_SIG* sig, WOLFSSL_BIGNUM* r,
    WOLFSSL_BIGNUM* s);
WOLFSSL_API WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_do_sign(const unsigned char *dgst,
                                                     int dgst_len,
                                                     WOLFSSL_EC_KEY *eckey);
WOLFSSL_API int wolfSSL_ECDSA_do_verify(const unsigned char *dgst,
                                        int dgst_len,
                                        const WOLFSSL_ECDSA_SIG *sig,
                                        WOLFSSL_EC_KEY *eckey);

WOLFSSL_API WOLFSSL_ECDSA_SIG *wolfSSL_d2i_ECDSA_SIG(WOLFSSL_ECDSA_SIG **sig,
                                                     const unsigned char **pp,
                                                     long len);
WOLFSSL_API int wolfSSL_i2d_ECDSA_SIG(const WOLFSSL_ECDSA_SIG *sig,
                                      unsigned char **pp);

#define ECDSA_SIG_free         wolfSSL_ECDSA_SIG_free
#define ECDSA_SIG_new          wolfSSL_ECDSA_SIG_new
#define ECDSA_SIG_get0         wolfSSL_ECDSA_SIG_get0
#define ECDSA_SIG_set0         wolfSSL_ECDSA_SIG_set0
#define ECDSA_do_sign          wolfSSL_ECDSA_do_sign
#define ECDSA_do_verify        wolfSSL_ECDSA_do_verify
#define d2i_ECDSA_SIG          wolfSSL_d2i_ECDSA_SIG
#define i2d_ECDSA_SIG          wolfSSL_i2d_ECDSA_SIG

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* header */

