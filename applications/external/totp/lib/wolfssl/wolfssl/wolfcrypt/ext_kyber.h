/* ext_kyber.h
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

#ifndef EXT_KYBER_H
#define EXT_KYBER_H

#ifdef WOLFSSL_HAVE_KYBER
#include <wolfssl/wolfcrypt/kyber.h>

#if !defined(HAVE_LIBOQS) && !defined(HAVE_PQM4)
#error "This code requires liboqs or pqm4"
#endif

#if defined(WOLFSSL_WC_KYBER)
#error "This code is incompatible with wolfCrypt's implementation of Kyber."
#endif

#if defined (HAVE_LIBOQS)
    #include <oqs/kem.h>
    #define EXT_KYBER_MAX_PRIV_SZ OQS_KEM_kyber_1024_length_secret_key
    #define EXT_KYBER_MAX_PUB_SZ  OQS_KEM_kyber_1024_length_public_key
#elif defined(HAVE_PQM4)
    #include "api_kyber.h"
    #define PQM4_PUBLIC_KEY_LENGTH    CRYPTO_PUBLICKEYBYTES
    #define PQM4_PRIVATE_KEY_LENGTH   CRYPTO_SECRETKEYBYTES
    #define PQM4_SHARED_SECRET_LENGTH CRYPTO_BYTES
    #define PQM4_CIPHERTEXT_LENGTH    CRYPTO_CIPHERTEXTBYTES

    #define EXT_KYBER_MAX_PRIV_SZ PQM4_PRIVATE_KEY_LENGTH
    #define EXT_KYBER_MAX_PUB_SZ  PQM4_PUBLIC_KEY_LENGTH
#endif

struct KyberKey {
    /* Type of key: KYBER_LEVEL1
     *              KYBER_LEVEL3
     *              KYBER_LEVEL5
     *
     * Note we don't save the variant (SHAKE vs AES) as that is decided at
     * configuration time. */
    int type;
    byte priv[EXT_KYBER_MAX_PRIV_SZ];
    byte pub[EXT_KYBER_MAX_PUB_SZ];
};

#if defined (HAVE_LIBOQS)
WOLFSSL_LOCAL int ext_kyber_enabled(int id);
#endif
#endif /* WOLFSSL_HAVE_KYBER */
#endif /* EXT_KYBER_H */
