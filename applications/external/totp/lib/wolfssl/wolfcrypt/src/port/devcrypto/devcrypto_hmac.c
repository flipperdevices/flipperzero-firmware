/* devcrypto_hmac.c
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

#if defined(WOLFSSL_DEVCRYPTO_HMAC)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

static int InternalTypeToDevcrypto(int t)
{
    switch (t) {
        case WC_HASH_TYPE_MD5:
            return CRYPTO_MD5_HMAC;
        case WC_HASH_TYPE_SHA:
            return CRYPTO_SHA1_HMAC;
        case WC_HASH_TYPE_SHA256:
            return CRYPTO_SHA2_256_HMAC;
        case WC_HASH_TYPE_SHA384:
            return CRYPTO_SHA2_384_HMAC;
        case WC_HASH_TYPE_SHA512:
            return CRYPTO_SHA2_512_HMAC;
        default:
            WOLFSSL_MSG("Unsupported HMAC hash type with devcrypto");
    }
    return HASH_TYPE_E;
}


int wc_DevCrypto_HmacSetKey(Hmac* hmac, int t, const byte* key, word32 keySz)
{
    int hType;

    hmac->ctx.cfd = -1;
    hType = InternalTypeToDevcrypto(t);
    if (hType < 0) {
        return hType;
    }
    else {
        return wc_DevCryptoCreate(&hmac->ctx, hType, (byte*)key, keySz);
    }
}


int wc_DevCrypto_HmacUpdate(Hmac* hmac, const byte* input, word32 inputSz)
{
    WC_CRYPTODEV*   dev;
    struct crypt_op crt;

    if (inputSz == 0) {
        return 0;
    }

    if ((dev = &hmac->ctx) == NULL) {
        WOLFSSL_MSG("Unsupported hash type");
        return BAD_FUNC_ARG;
    }

    wc_SetupCrypt(&crt, dev, (byte*)input, inputSz, NULL, NULL,
            COP_FLAG_UPDATE, COP_ENCRYPT);
    if (ioctl(dev->cfd, CIOCCRYPT, &crt)) {
        WOLFSSL_MSG("Error with call to ioctl");
        return WC_DEVCRYPTO_E;
    }

    return 0;

}

int wc_DevCrypto_HmacFinal(Hmac* hmac, byte* out)
{
    WC_CRYPTODEV*   dev;
    struct crypt_op crt;

    if ((dev = &hmac->ctx) == NULL) {
        WOLFSSL_MSG("Unsupported hash type");
        return BAD_FUNC_ARG;
    }

    wc_SetupCrypt(&crt, dev, NULL, 0, NULL, out, COP_FLAG_FINAL, COP_ENCRYPT);
    if (ioctl(dev->cfd, CIOCCRYPT, &crt)) {
        WOLFSSL_MSG("Error with call to ioctl");
        return WC_DEVCRYPTO_E;
    }

    return 0;
}

int wc_DevCrypto_HmacInit(Hmac* hmac, void* heap, int devId)
{
    (void)hmac;
    (void)heap;
    (void)devId;
    return 0;
}


void wc_DevCrypto_HmacFree(Hmac* hmac)
{
    wc_DevCryptoFree(&hmac->ctx);
}
#endif /* WOLFSSL_DEVCRYPTO */

