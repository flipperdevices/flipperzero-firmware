/* rc4.h
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



/*  rc4.h defines mini des openssl compatibility layer
 *
 */

#ifndef WOLFSSL_RC4_COMPAT_H_
#define WOLFSSL_RC4_COMPAT_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/openssl/ssl.h> /* included for size_t */

#ifdef __cplusplus
    extern "C" {
#endif

/* applications including wolfssl/openssl/rc4.h are expecting to have access to
 * the size of RC4_KEY structures. */
typedef struct WOLFSSL_RC4_KEY {
    /* big enough for Arc4 from wolfssl/wolfcrypt/arc4.h */
    void* holder[(272 + WC_ASYNC_DEV_SIZE) / sizeof(void*)];
} WOLFSSL_RC4_KEY;
typedef WOLFSSL_RC4_KEY RC4_KEY;

WOLFSSL_API void wolfSSL_RC4_set_key(WOLFSSL_RC4_KEY* key, int len,
        const unsigned char* data);
WOLFSSL_API void wolfSSL_RC4(WOLFSSL_RC4_KEY* key, size_t len,
        const unsigned char* in, unsigned char* out);

#define RC4         wolfSSL_RC4
#define RC4_set_key wolfSSL_RC4_set_key

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_RC4_COMPAT_H_ */

