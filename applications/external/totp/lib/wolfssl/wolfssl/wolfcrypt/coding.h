/* coding.h
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
    \file wolfssl/wolfcrypt/coding.h
*/

#ifndef WOLF_CRYPT_CODING_H
#define WOLF_CRYPT_CODING_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif


WOLFSSL_API int Base64_Decode(const byte* in, word32 inLen, byte* out,
                               word32* outLen);

#if defined(OPENSSL_EXTRA) || defined(SESSION_CERTS) || defined(WOLFSSL_KEY_GEN) \
   || defined(WOLFSSL_CERT_GEN) || defined(HAVE_WEBSERVER) || !defined(NO_DSA)
    #ifndef WOLFSSL_BASE64_ENCODE
        #define WOLFSSL_BASE64_ENCODE
    #endif
#endif


#ifdef WOLFSSL_BASE64_ENCODE
    enum Escaped {
        WC_STD_ENC = 0,       /* normal \n line ending encoding */
        WC_ESC_NL_ENC,        /* use escape sequence encoding   */
        WC_NO_NL_ENC          /* no encoding at all             */
    }; /* Encoding types */

    /* encode isn't */
    WOLFSSL_API
    int Base64_Encode(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
    WOLFSSL_API
    int Base64_EncodeEsc(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
    WOLFSSL_API
    int Base64_Encode_NoNl(const byte* in, word32 inLen, byte* out,
                                  word32* outLen);
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
    defined(HAVE_WEBSERVER) || defined(HAVE_FIPS) || \
    defined(HAVE_ECC_CDH) || defined(HAVE_SELFTEST) || \
    defined(WOLFSSL_ENCRYPTED_KEYS)
    #ifndef WOLFSSL_BASE16
        #define WOLFSSL_BASE16
    #endif
#endif

#ifdef WOLFSSL_BASE16
    WOLFSSL_API
    int Base16_Decode(const byte* in, word32 inLen, byte* out, word32* outLen);
    WOLFSSL_API
    int Base16_Encode(const byte* in, word32 inLen, byte* out, word32* outLen);
#endif

 WOLFSSL_LOCAL int Base64_SkipNewline(const byte* in, word32* inLen,
            word32* outJ);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_CODING_H */

