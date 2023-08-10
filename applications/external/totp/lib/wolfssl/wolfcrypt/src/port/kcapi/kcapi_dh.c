/* kcapi_dh.c
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

#if defined(WOLFSSL_KCAPI_DH) && !defined(NO_DH)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#include <wolfssl/wolfcrypt/port/kcapi/kcapi_dh.h>
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/asn.h>

static const char WC_NAME_DH[] = "dh";

void KcapiDh_Free(DhKey* key)
{
    if (key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }
}

static int KcapiDh_SetParams(DhKey* key)
{
    int ret;
    unsigned char* pkcs3 = NULL;
    word32 len;

    ret = wc_DhParamsToDer(key, NULL, &len);
    if (ret == LENGTH_ONLY_E) {
        ret = 0;
        pkcs3 = (unsigned char*)XMALLOC(len, key->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (pkcs3 == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        ret = wc_DhParamsToDer(key, pkcs3, &len);
    }
    if (ret == 0) {
        ret = kcapi_kpp_dh_setparam_pkcs3(key->handle, pkcs3, len);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiDh_SetParams: Failed to set");
        }
    }

    if (pkcs3 != NULL) {
        XFREE(pkcs3, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return ret;
}

int KcapiDh_MakeKey(DhKey* key, byte* pub, word32* pubSz)
{
    int ret = 0;

    if (key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }
    if (ret == 0) {
        ret = kcapi_kpp_init(&key->handle, WC_NAME_DH, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiDh_MakeKey: Failed to initialization");
        }
    }

    if (ret == 0) {
        ret = KcapiDh_SetParams(key);
    }
    if (ret == 0) {
        ret = kcapi_kpp_setkey(key->handle, NULL, 0);
        if (ret >= 0) {
            ret = 0;
        }
    }
    if (ret == 0) {
        ret = (int)kcapi_kpp_keygen(key->handle, pub, *pubSz,
                               KCAPI_ACCESS_HEURISTIC);
    }

    return ret;
}

#ifdef WOLFSSL_DH_EXTRA
static int KcapiDh_SetPrivKey(DhKey* key)
{
    int ret;
    unsigned char* priv;
    int len;

    len = ret = mp_unsigned_bin_size(&key->priv);
    if (ret >= 0) {
        priv = (unsigned char*)XMALLOC(len, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (priv == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret >= 0) {
        ret = mp_to_unsigned_bin(&key->priv, priv);
    }
    if (ret >= 0) {
        ret = kcapi_kpp_setkey(key->handle, priv, len);
        if (ret >= 0) {
            ret = 0;
        }
    }

    return ret;
}
#endif

int KcapiDh_SharedSecret(DhKey* private_key, const byte* pub, word32 pubSz,
                         byte* out, word32* outlen)
{
    int ret = 0;

    if (private_key->handle == NULL) {
        ret = kcapi_kpp_init(&private_key->handle, WC_NAME_DH, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiDh_SharedSecret: Failed to initialization");
        }
        if (ret == 0) {
            ret = KcapiDh_SetParams(private_key);
        }
    }

#ifdef WOLFSSL_DH_EXTRA
    if (!mp_iszero(&private_key->priv)) {
        ret = KcapiDh_SetPrivKey(private_key);
    }
#endif

    if (ret == 0) {
        ret = (int)kcapi_kpp_ssgen(private_key->handle, pub, pubSz, out,
                                   *outlen, KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            *outlen = ret;
            ret = 0;
        }
    }

    return ret;
}

#endif /* WOLFSSL_KCAPI_DH && !NO_DH */
