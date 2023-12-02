/* kdf.h
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

/*!
    \file wolfssl/wolfcrypt/kdf.h
*/

#ifndef NO_KDF

#ifndef WOLF_CRYPT_KDF_H
#define WOLF_CRYPT_KDF_H

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 5)
    #include <wolfssl/wolfcrypt/fips.h>
#endif

#include <wolfssl/wolfcrypt/hmac.h>

#ifdef __cplusplus
    extern "C" {
#endif

enum max_prf {
#ifdef HAVE_FFDHE_8192
    MAX_PRF_HALF        = 516, /* Maximum half secret len */
#elif defined(HAVE_FFDHE_6144)
    MAX_PRF_HALF        = 388, /* Maximum half secret len */
#else
    MAX_PRF_HALF        = 260, /* Maximum half secret len */
#endif
    MAX_PRF_LABSEED     = 128, /* Maximum label + seed len */
    MAX_PRF_DIG         = 224  /* Maximum digest len      */
};


#ifdef WOLFSSL_HAVE_PRF
WOLFSSL_API int wc_PRF(byte* result, word32 resLen, const byte* secret,
                    word32 secLen, const byte* seed, word32 seedLen, int hash,
                    void* heap, int devId);
WOLFSSL_API int wc_PRF_TLSv1(byte* digest, word32 digLen, const byte* secret,
                    word32 secLen, const byte* label, word32 labLen,
                    const byte* seed, word32 seedLen, void* heap, int devId);
WOLFSSL_API int wc_PRF_TLS(byte* digest, word32 digLen, const byte* secret,
                    word32 secLen, const byte* label, word32 labLen,
                    const byte* seed, word32 seedLen, int useAtLeastSha256,
                    int hash_type, void* heap, int devId);
#endif /* WOLFSSL_HAVE_PRF */

#ifdef HAVE_HKDF

enum {
/*
    MAX_HKDF_LABEL_SZ   = OPAQUE16_LEN +
                          OPAQUE8_LEN + PROTOCOL_LABEL_SZ + MAX_LABEL_SZ +
                          OPAQUE8_LEN + WC_MAX_DIGEST_SIZE
*/
    MAX_TLS13_HKDF_LABEL_SZ = 47 + WC_MAX_DIGEST_SIZE
};

WOLFSSL_API int wc_Tls13_HKDF_Extract(byte* prk, const byte* salt,
                          word32 saltLen, byte* ikm, word32 ikmLen, int digest);

WOLFSSL_API int wc_Tls13_HKDF_Expand_Label(byte* okm, word32 okmLen,
                             const byte* prk, word32 prkLen,
                             const byte* protocol, word32 protocolLen,
                             const byte* label, word32 labelLen,
                             const byte* info, word32 infoLen,
                             int digest);
#if defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                    \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
WOLFSSL_API int wc_Tls13_HKDF_Expand_Label_Alloc(byte* okm, word32 okmLen,
    const byte* prk, word32 prkLen, const byte* protocol, word32 protocolLen,
    const byte* label, word32 labelLen, const byte* info, word32 infoLen,
    int digest, void* heap);
#endif /* !defined(HAVE_FIPS) || FIPS_VERSION_GE(5,3) */

#endif /* HAVE_HKDF */

#ifdef WOLFSSL_WOLFSSH

WOLFSSL_API int wc_SSH_KDF(byte hashId, byte keyId,
        byte* key, word32 keySz,
        const byte* k, word32 kSz,
        const byte* h, word32 hSz,
        const byte* sessionId, word32 sessionIdSz);

#endif /* WOLFSSL_WOLFSSH */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_KDF_H */

#endif /* NO_KDF */

