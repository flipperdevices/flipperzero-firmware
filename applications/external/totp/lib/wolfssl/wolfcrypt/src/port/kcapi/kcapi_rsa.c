/* kcapi_rsa.c
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

#if defined(WOLFSSL_KCAPI_RSA) && !defined(NO_RSA)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#include <wolfssl/wolfcrypt/port/kcapi/kcapi_rsa.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/asn.h>

static const char WC_NAME_RSA[] = "rsa";

void KcapiRsa_Free(RsaKey* key)
{
    if (key->handle != NULL) {
        kcapi_akcipher_destroy(key->handle);
        key->handle = NULL;
    }
}

#if !defined(WOLFSSL_RSA_PUBLIC_ONLY) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
static int KcapiRsa_SetPrivKey(RsaKey* key)
{
    int ret = 0;
    unsigned char* priv = NULL;
    int len;

    len = wc_RsaKeyToDer(key, NULL, 0);
    if (len < 0) {
        ret = len;
    }
    if (ret == 0) {
        priv = (unsigned char*)XMALLOC(len, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (priv == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        len = ret = wc_RsaKeyToDer(key, priv, len);
        if (ret < 0) {
            WOLFSSL_MSG("KcapiRsa_SetPrivKey: to DER failed");
        }
    }

    if (ret >= 0) {
        ret = kcapi_akcipher_setkey(key->handle, priv, len);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiRsa_SetPrivKey: Failed to set");
        }
    }

    if (priv != NULL) {
        XFREE(priv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return ret;
}

int KcapiRsa_Decrypt(RsaKey* key, const byte* in, word32 inLen, byte* out,
                     word32* outLen)
{
    int ret = 0;

    if (key->handle == NULL) {
        ret = kcapi_akcipher_init(&key->handle, WC_NAME_RSA, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiRsa_Decrypt: Failed initialization");
        }
        else {
            ret = KcapiRsa_SetPrivKey(key);
        }
    }
    if (ret == 0) {
        ret = (int)kcapi_akcipher_decrypt(key->handle, in, inLen, out, *outLen,
                                  KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            *outLen = ret;
            ret = 0;
        }
    }

    return ret;
}
#endif /* !WOLFSSL_RSA_PUBLIC_ONLY && !WOLFSSL_RSA_VERIFY_ONLY */

static int KcapiRsa_SetPubKey(RsaKey* key)
{
    int ret = 0;
    unsigned char* pub = NULL;

    word32 len = 0, idx = 0, len2;

    len  = MAX_SEQ_SZ;                  /* Sequence */
    len += ASN_TAG_SZ + MAX_LENGTH_SZ;  /* Integer */
    len += mp_leading_bit(&key->n) ? 1 : 0;
    len += mp_unsigned_bin_size(&key->n);
    len += ASN_TAG_SZ + MAX_LENGTH_SZ;  /* Integer */
    len += mp_leading_bit(&key->e) ? 1 : 0;
    len += mp_unsigned_bin_size(&key->e);

    pub = (unsigned char*)XMALLOC(len, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (pub == NULL) {
        ret = MEMORY_E;
    }

    if (ret == 0) {
        idx = len;
        len2 = mp_unsigned_bin_size(&key->e);
        idx -= len2;
        ret = mp_to_unsigned_bin(&key->e, pub + idx);
    }
    if (ret >= 0) {
        if (mp_leading_bit(&key->e)) {
            pub[--idx] = 0x00;
            len2++;
        }
        idx -= SetLength(len2, NULL);
        SetLength(len2, pub + idx);
        pub[--idx] = ASN_INTEGER;

        len2 = mp_unsigned_bin_size(&key->n);
        idx -= len2;
        ret = mp_to_unsigned_bin(&key->n, pub + idx);
    }
    if (ret >= 0) {
        if (mp_leading_bit(&key->n)) {
            pub[--idx] = 0x00;
            len2++;
        }
        idx -= SetLength(len2, NULL);
        SetLength(len2, pub + idx);
        pub[--idx] = ASN_INTEGER;

        len2 = len - idx;
        idx -= SetSequence(len2, NULL);
        SetSequence(len2, pub + idx);

        ret = kcapi_akcipher_setpubkey(key->handle, pub + idx, len - idx);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiRsa_SetPubKey: Failed to set");
        }
    }

    if (pub != NULL) {
        XFREE(pub, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return ret;
}

int KcapiRsa_Encrypt(RsaKey* key, const byte* in, word32 inLen, byte* out,
                     word32* outLen)
{
    int ret = 0;

    if (key->handle == NULL) {
        ret = kcapi_akcipher_init(&key->handle, WC_NAME_RSA, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiRsa_Encrypt: Failed initialization");
        }
        else {
            ret = KcapiRsa_SetPubKey(key);
        }
    }
    if (ret == 0) {
        ret = (int)kcapi_akcipher_encrypt(key->handle, in, inLen, out, *outLen,
                                    KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            *outLen = ret;
            ret = 0;
        }
    }

    return ret;
}

#endif /* WOLFSSL_KCAPI_RSA && !NO_RSA */
