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

#if defined(HAVE_FIPS) && \
        (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))
/* Set the DER/BER encoding of the ASN.1 INTEGER header.
 *
 * len        Length of data to encode.
 * firstByte  First byte of data, most significant byte of integer, to encode.
 * output     Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static int SetASNInt(int len, byte firstByte, byte* output)
{
    word32 idx = 0;

    if (output)
        output[idx] = ASN_INTEGER;
    idx++;
    if (firstByte & 0x80)
        len++;
    idx += SetLength(len, output ? output + idx : NULL);
    if (firstByte & 0x80) {
        if (output)
            output[idx] = 0x00;
        idx++;
    }

    return idx;
}

static int SetASNIntMP(mp_int* n, int maxSz, byte* output)
{
    int idx = 0;
    int leadingBit;
    int length;
    int err;

    leadingBit = mp_leading_bit(n);
    length = mp_unsigned_bin_size(n);
    idx = SetASNInt(length, leadingBit ? 0x80 : 0x00, output);
    if (maxSz >= 0 && (idx + length) > maxSz)
        return BUFFER_E;

    if (output) {
        err = mp_to_unsigned_bin(n, output + idx);
        if (err != MP_OKAY)
            return MP_TO_E;
    }
    idx += length;

    return idx;
}

static mp_int* GetRsaInt(RsaKey* key, int idx)
{
    if (idx == 0)
        return &key->n;
    if (idx == 1)
        return &key->e;
    if (idx == 2)
        return &key->d;
    if (idx == 3)
        return &key->p;
    if (idx == 4)
        return &key->q;
    if (idx == 5)
        return &key->dP;
    if (idx == 6)
        return &key->dQ;
    if (idx == 7)
        return &key->u;

    return NULL;
}

/* Release Tmp RSA resources */
static WC_INLINE void FreeTmpRsas(byte** tmps, void* heap)
{
    int i;

    (void)heap;

    for (i = 0; i < RSA_INTS; i++)
        XFREE(tmps[i], heap, DYNAMIC_TYPE_RSA);
}


/* Convert RsaKey key to DER format, write to output (inLen), return bytes
   written */
static int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen)
{
    word32 seqSz, verSz, rawLen, intTotalLen = 0;
    word32 sizes[RSA_INTS];
    int    i, j, outLen, ret = 0;

    byte  seq[MAX_SEQ_SZ];
    byte  ver[MAX_VERSION_SZ];
    byte* tmps[RSA_INTS];

    if (!key || !output)
        return BAD_FUNC_ARG;

    if (key->type != RSA_PRIVATE)
        return BAD_FUNC_ARG;

    for (i = 0; i < RSA_INTS; i++)
        tmps[i] = NULL;

    /* write all big ints from key to DER tmps */
    for (i = 0; i < RSA_INTS; i++) {
        int mpSz;
        mp_int* keyInt = GetRsaInt(key, i);

        rawLen = mp_unsigned_bin_size(keyInt) + 1;
        tmps[i] = (byte*)XMALLOC(rawLen + MAX_SEQ_SZ, key->heap,
                                 DYNAMIC_TYPE_RSA);
        if (tmps[i] == NULL) {
            ret = MEMORY_E;
            break;
        }

        mpSz = SetASNIntMP(keyInt, MAX_RSA_INT_SZ, tmps[i]);
        if (mpSz < 0) {
            ret = mpSz;
            break;
        }
        intTotalLen += (sizes[i] = mpSz);
    }

    if (ret != 0) {
        FreeTmpRsas(tmps, key->heap);
        return ret;
    }

    /* make headers */
    verSz = SetMyVersion(0, ver, FALSE);
    seqSz = SetSequence(verSz + intTotalLen, seq);

    outLen = seqSz + verSz + intTotalLen;
    if (outLen > (int)inLen) {
        FreeTmpRsas(tmps, key->heap);
        return BAD_FUNC_ARG;
    }

    /* write to output */
    XMEMCPY(output, seq, seqSz);
    j = seqSz;
    XMEMCPY(output + j, ver, verSz);
    j += verSz;

    for (i = 0; i < RSA_INTS; i++) {
        XMEMCPY(output + j, tmps[i], sizes[i]);
        j += sizes[i];
    }
    FreeTmpRsas(tmps, key->heap);

    return outLen;
}
#endif

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
