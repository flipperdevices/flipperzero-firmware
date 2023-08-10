/* stack.h
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

/* stack.h for openSSL */

#ifndef WOLFSSL_STACK_H_
#define WOLFSSL_STACK_H_

#include <wolfssl/openssl/compat_types.h>
#include <wolfssl/openssl/ssl.h>

#ifdef __cplusplus
    extern "C" {
#endif

typedef void (*wolfSSL_sk_freefunc)(void *);

WOLFSSL_API void wolfSSL_sk_GENERIC_pop_free(WOLFSSL_STACK* sk, wolfSSL_sk_freefunc f);
WOLFSSL_API void wolfSSL_sk_GENERIC_free(WOLFSSL_STACK *sk);
WOLFSSL_API int wolfSSL_sk_GENERIC_push(WOLFSSL_STACK *sk, void *data);
WOLFSSL_API void wolfSSL_sk_pop_free(WOLFSSL_STACK *st, void (*func) (void *));
WOLFSSL_API WOLFSSL_STACK *wolfSSL_sk_new_null(void);

WOLFSSL_API int wolfSSL_sk_CIPHER_push(WOLFSSL_STACK *st,WOLFSSL_CIPHER *cipher);
WOLFSSL_API WOLFSSL_CIPHER* wolfSSL_sk_CIPHER_pop(WOLF_STACK_OF(WOLFSSL_CIPHER)* sk);
WOLFSSL_API WOLFSSL_STACK* wolfSSL_sk_new_cipher(void);

#define OPENSSL_sk_free       wolfSSL_sk_free
#define OPENSSL_sk_pop_free   wolfSSL_sk_pop_free
#define OPENSSL_sk_new_null   wolfSSL_sk_new_null
#define OPENSSL_sk_push       wolfSSL_sk_push

/* provides older OpenSSL API compatibility  */
#define sk_free         OPENSSL_sk_free
#define sk_pop_free     OPENSSL_sk_pop_free
#define sk_new_null     OPENSSL_sk_new_null
#define sk_push         OPENSSL_sk_push

#ifdef  __cplusplus
}
#endif

#endif
