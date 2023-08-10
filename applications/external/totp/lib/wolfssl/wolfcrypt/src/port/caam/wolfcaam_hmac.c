/* wolfcaam_hmac.c
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

#if defined(WOLFSSL_CAAM) && !defined(NO_HMAC) && defined(WOLFSSL_CAAM_HMAC)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/asn.h>

#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
#include <stdio.h>
#endif

#if defined(WOLFSSL_DEVCRYPTO_HMAC)
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

/* HSM lib does not support HMAC with QXP board, use devcrypto instead */
int wc_CAAM_Hmac(Hmac* hmac, int macType, const byte* msg, int msgSz,
    byte* digest)
{
    int ret = 0;

    if (hmac->ctx.cfd == -1 && hmac->keyLen > 0) {
        ret = wc_DevCrypto_HmacSetKey(hmac, macType, hmac->keyRaw,
                    hmac->keyLen);
        if (ret != 0) {
            WOLFSSL_MSG("Error with set key");
            if (ret == HASH_TYPE_E) {
                ret = CRYPTOCB_UNAVAILABLE; /* that hash type is not supported*/
            }
        }
    }

    if (ret == 0 && msgSz > 0) {
        ret = wc_DevCrypto_HmacUpdate(hmac, msg, msgSz);
        if (ret != 0) {
            WOLFSSL_MSG("Issue with hmac update");
        }
    }

    if (ret == 0 && digest != NULL) {
        ret = wc_DevCrypto_HmacFinal(hmac, digest);
        if (ret != 0) {
            WOLFSSL_MSG("Issue with hmac final");
        }
        else {
            wc_DevCrypto_HmacFree(hmac);
        }
    }
    return ret;
}
#else
int wc_CAAM_Hmac(Hmac* hmac, int macType, const byte* msg, int msgSz,
    byte* digest)
{
    (void)hmac;
    (void)macType;
    (void)msg;
    (void)msgSz;
    (void)digest;
    return CRYPTOCB_UNAVAILABLE;
}
#endif /* WOLFSSL_DEVCRYPTO_HMAC */

#endif

