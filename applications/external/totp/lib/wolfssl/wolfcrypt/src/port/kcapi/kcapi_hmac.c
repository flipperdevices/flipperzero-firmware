/* kcapi_hmac.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_KCAPI_HMAC) && !defined(NO_HMAC)

#define FIPS_NO_WRAPPERS

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#include <wolfssl/wolfcrypt/port/kcapi/kcapi_hmac.h>
#include <wolfssl/wolfcrypt/hmac.h>

#if !defined(NO_SHA) && !defined(WOLFSSL_NO_KCAPI_HMAC_SHA1)
static const char WC_NAME_HMAC_SHA1[] = "hmac(sha1)";
#endif
#if defined(WOLFSSL_SHA224) && !defined(WOLFSSL_NO_KCAPI_HMAC_SHA224)
static const char WC_NAME_HMAC_SHA224[] = "hmac(sha224)";
#endif
#if !defined(NO_SHA256) && !defined(WOLFSSL_NO_KCAPI_HMAC_SHA256)
static const char WC_NAME_HMAC_SHA256[] = "hmac(sha256)";
#endif
#if defined(WOLFSSL_SHA384) && !defined(WOLFSSL_NO_KCAPI_HMAC_SHA384)
static const char WC_NAME_HMAC_SHA384[] = "hmac(sha384)";
#endif
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NO_KCAPI_HMAC_SHA512)
static const char WC_NAME_HMAC_SHA512[] = "hmac(sha512)";
#endif

int wc_HmacInit(Hmac* hmac, void* heap, int devId)
{
    int ret = 0;

    if (hmac == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMSET(hmac, 0, sizeof(*hmac));
        hmac->handle = NULL;
        hmac->heap = heap;
        (void)devId;
    }


    return ret;
}

void wc_HmacFree(Hmac* hmac)
{
    if (hmac != NULL) {
        if (hmac->handle != NULL) {
            kcapi_md_destroy(hmac->handle);
            hmac->handle = NULL;
        }
    }
}

int wc_HmacSetKey(Hmac* hmac, int type, const byte* key, word32 length)
{
    int ret = 0;
    const char* ciphername = NULL;

    if ((hmac == NULL || (key == NULL && length != 0))) {
        return BAD_FUNC_ARG;
    }

#ifdef HAVE_FIPS
    if (length < HMAC_FIPS_MIN_KEY)
        return HMAC_MIN_KEYLEN_E;
#endif

    if (ret == 0) {
        switch (type) {
        #ifndef NO_MD5
            case WC_MD5:
                return wc_HmacSetKey_Software(hmac, type, key, length);
        #endif
        #ifndef NO_SHA
            case WC_SHA:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA1)
                ciphername = WC_NAME_HMAC_SHA1;
                break;
            #else
                return wc_HmacSetKey_Software(hmac, type, key, length);
            #endif
        #endif
        #ifdef WOLFSSL_SHA224
            case WC_SHA224:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA224)
                ciphername = WC_NAME_HMAC_SHA224;
                break;
            #else
                return wc_HmacSetKey_Software(hmac, type, key, length);
            #endif
        #endif
        #ifndef NO_SHA256
            case WC_SHA256:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA256)
                ciphername = WC_NAME_HMAC_SHA256;
                break;
            #else
                return wc_HmacSetKey_Software(hmac, type, key, length);
            #endif
        #endif
        #ifdef WOLFSSL_SHA384
            case WC_SHA384:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA384)
                ciphername = WC_NAME_HMAC_SHA384;
                break;
            #else
                return wc_HmacSetKey_Software(hmac, type, key, length);
            #endif
        #endif
        #ifdef WOLFSSL_SHA512
            case WC_SHA512:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA512)
                ciphername = WC_NAME_HMAC_SHA512;
                break;
            #else
                return wc_HmacSetKey_Software(hmac, type, key, length);
            #endif
        #endif
    #ifdef WOLFSSL_SHA3
    #ifndef WOLFSSL_NOSHA3_224
        case WC_SHA3_224:
    #endif
    #ifndef WOLFSSL_NOSHA3_256
        case WC_SHA3_256:
    #endif
    #ifndef WOLFSSL_NOSHA3_384
        case WC_SHA3_384:
    #endif
    #ifndef WOLFSSL_NOSHA3_512
        case WC_SHA3_512:
    #endif
            return wc_HmacSetKey_Software(hmac, type, key, length);
    #endif
            default:
                ret = BAD_FUNC_ARG;
                break;
        }
        hmac->macType = type;
    }

    if (hmac->handle != NULL) {
        kcapi_md_destroy(hmac->handle);
        hmac->handle = NULL;
    }
    if (ret == 0) {
        ret = kcapi_md_init(&hmac->handle, ciphername, 0);
    }
    if (ret == 0) {
        ret = kcapi_md_setkey(hmac->handle, key, length);
    }

    return ret;
}

int wc_HmacUpdate(Hmac* hmac, const byte* msg, word32 length)
{
    int ret = 0;

    if (hmac == NULL || (msg == NULL && length > 0)) {
        return BAD_FUNC_ARG;
    }

    switch (hmac->macType) {
        case WC_MD5:
        case WC_SHA3_224:
        case WC_SHA3_256:
        case WC_SHA3_384:
        case WC_SHA3_512:
    #if defined(WOLFSSL_NO_KCAPI_HMAC_SHA1)
        case WC_SHA:
    #endif
    #if defined(WOLFSSL_NO_KCAPI_HMAC_SHA224)
        case WC_SHA224:
    #endif
    #if defined(WOLFSSL_NO_KCAPI_HMAC_SHA256)
        case WC_SHA256:
    #endif
    #if defined(WOLFSSL_NO_KCAPI_HMAC_SHA384)
        case WC_SHA384:
    #endif
    #if defined(WOLFSSL_NO_KCAPI_HMAC_SHA512)
        case WC_SHA512:
    #endif
            ret = wc_HmacUpdate_Software(hmac, msg, length);
            break;
        default:
            ret = (int)kcapi_md_update(hmac->handle, msg, length);
            if (ret >= 0) {
                ret = 0;
            }
            break;
    }

    return ret;
}

int wc_HmacFinal(Hmac* hmac, byte* hash)
{
    int ret = 0;
    int len = 0;

    if (hmac == NULL || hash == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        switch (hmac->macType) {
        #ifndef NO_MD5
            case WC_MD5:
                return wc_HmacFinal_Software(hmac, hash);
        #endif
        #ifndef NO_SHA
            case WC_SHA:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA1)
                len = WC_SHA_DIGEST_SIZE;
                break;
            #else
                return wc_HmacFinal_Software(hmac, hash);
            #endif
        #endif
        #ifdef WOLFSSL_SHA224
            case WC_SHA224:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA224)
                len = WC_SHA224_DIGEST_SIZE;
                break;
            #else
                return wc_HmacFinal_Software(hmac, hash);
            #endif
        #endif
        #ifndef NO_SHA256
            case WC_SHA256:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA256)
                len = WC_SHA256_DIGEST_SIZE;
                break;
            #else
                return wc_HmacFinal_Software(hmac, hash);
            #endif
        #endif
        #ifdef WOLFSSL_SHA384
            case WC_SHA384:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA384)
                len = WC_SHA384_DIGEST_SIZE;
                break;
            #else
                return wc_HmacFinal_Software(hmac, hash);
            #endif
        #endif
        #ifdef WOLFSSL_SHA512
            case WC_SHA512:
            #if !defined(WOLFSSL_NO_KCAPI_HMAC_SHA512)
                len = WC_SHA512_DIGEST_SIZE;
                break;
            #else
                return wc_HmacFinal_Software(hmac, hash);
            #endif
        #endif
        #ifdef WOLFSSL_SHA3
        #ifndef WOLFSSL_NOSHA3_224
            case WC_SHA3_224:
        #endif
        #ifndef WOLFSSL_NOSHA3_256
            case WC_SHA3_256:
        #endif
        #ifndef WOLFSSL_NOSHA3_384
            case WC_SHA3_384:
        #endif
        #ifndef WOLFSSL_NOSHA3_512
            case WC_SHA3_512:
        #endif
                return wc_HmacFinal_Software(hmac, hash);
        #endif
        }
        ret = (int)kcapi_md_final(hmac->handle, hash, len);
    }
    if (ret >= 0) {
        ret = 0;
    }

    return ret;
}

#endif /* WOLFSSL_KCAPI_HMAC && !NO_HMAC */
