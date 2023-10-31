/* modes.h
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
#ifndef WOLFSSL_OPENSSL_MODES_H
#define WOLFSSL_OPENSSL_MODES_H

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/openssl/ssl.h>

typedef void (*WOLFSSL_CBC128_CB) (const unsigned char *in,
        unsigned char *out, size_t len, const void *key,
        unsigned char *iv, int enc);

WOLFSSL_API size_t wolfSSL_CRYPTO_cts128_encrypt(const unsigned char *in,
        unsigned char *out, size_t len, const void *key,
        unsigned char *iv, WOLFSSL_CBC128_CB cbc);
WOLFSSL_API size_t wolfSSL_CRYPTO_cts128_decrypt(const unsigned char *in,
        unsigned char *out, size_t len, const void *key,
        unsigned char *iv, WOLFSSL_CBC128_CB cbc);

#define WOLFSSL_CTS128_BLOCK_SZ         16

/* Compatibility layer defines */
#define CRYPTO_cts128_encrypt           wolfSSL_CRYPTO_cts128_encrypt
#define CRYPTO_cts128_decrypt           wolfSSL_CRYPTO_cts128_decrypt
#define cbc128_f                        WOLFSSL_CBC128_CB

#endif /* WOLFSSL_OPENSSL_MODES_H */
