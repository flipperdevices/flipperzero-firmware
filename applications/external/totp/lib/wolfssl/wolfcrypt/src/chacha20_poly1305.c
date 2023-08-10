/* chacha.c
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
/*

DESCRIPTION
This library contains implementation for the ChaCha20 stream cipher and
the Poly1305 authenticator, both as as combined-mode,
or Authenticated Encryption with Additional Data (AEAD) algorithm.

*/

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)

#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
#include <wolfssl/wolfcrypt/misc.h>
#else
#define WOLFSSL_MISC_INCLUDED
#include <wolfcrypt/src/misc.c>
#endif

#define CHACHA20_POLY1305_AEAD_INITIAL_COUNTER  0
WOLFSSL_ABI
int wc_ChaCha20Poly1305_Encrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inPlaintext, const word32 inPlaintextLen,
                byte* outCiphertext,
                byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE])
{
    int ret;
    ChaChaPoly_Aead aead;

    /* Validate function arguments */
    if (!inKey || !inIV ||
        (inPlaintextLen > 0 && inPlaintext == NULL) ||
        !outCiphertext ||
        !outAuthTag)
    {
        return BAD_FUNC_ARG;
    }

    ret = wc_ChaCha20Poly1305_Init(&aead, inKey, inIV,
        CHACHA20_POLY1305_AEAD_ENCRYPT);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_UpdateAad(&aead, inAAD, inAADLen);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_UpdateData(&aead, inPlaintext, outCiphertext,
            inPlaintextLen);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_Final(&aead, outAuthTag);
    return ret;
}

WOLFSSL_ABI
int wc_ChaCha20Poly1305_Decrypt(
                const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
                const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
                const byte* inAAD, const word32 inAADLen,
                const byte* inCiphertext, const word32 inCiphertextLen,
                const byte inAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE],
                byte* outPlaintext)
{
    int ret;
    ChaChaPoly_Aead aead;
    byte calculatedAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];

    /* Validate function arguments */
    if (!inKey || !inIV ||
        (inCiphertextLen > 0 && inCiphertext == NULL) ||
        !inAuthTag ||
        !outPlaintext)
    {
        return BAD_FUNC_ARG;
    }

    XMEMSET(calculatedAuthTag, 0, sizeof(calculatedAuthTag));

    ret = wc_ChaCha20Poly1305_Init(&aead, inKey, inIV,
        CHACHA20_POLY1305_AEAD_DECRYPT);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_UpdateAad(&aead, inAAD, inAADLen);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_UpdateData(&aead, inCiphertext, outPlaintext,
            inCiphertextLen);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_Final(&aead, calculatedAuthTag);
    if (ret == 0)
        ret = wc_ChaCha20Poly1305_CheckTag(inAuthTag, calculatedAuthTag);
    return ret;
}

int wc_ChaCha20Poly1305_CheckTag(
    const byte authTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE],
    const byte authTagChk[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE])
{
    int ret = 0;
    if (authTag == NULL || authTagChk == NULL) {
        return BAD_FUNC_ARG;
    }
    if (ConstantCompare(authTag, authTagChk,
            CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE) != 0) {
        ret = MAC_CMP_FAILED_E;
    }
    return ret;
}

int wc_ChaCha20Poly1305_Init(ChaChaPoly_Aead* aead,
    const byte inKey[CHACHA20_POLY1305_AEAD_KEYSIZE],
    const byte inIV[CHACHA20_POLY1305_AEAD_IV_SIZE],
    int isEncrypt)
{
    int ret;
    byte authKey[CHACHA20_POLY1305_AEAD_KEYSIZE];

    /* check arguments */
    if (aead == NULL || inKey == NULL || inIV == NULL) {
        return BAD_FUNC_ARG;
    }

    /* setup aead context */
    XMEMSET(aead, 0, sizeof(ChaChaPoly_Aead));
    XMEMSET(authKey, 0, sizeof(authKey));
    aead->isEncrypt = isEncrypt ? 1 : 0;

    /* Initialize the ChaCha20 context (key and iv) */
    ret = wc_Chacha_SetKey(&aead->chacha, inKey,
        CHACHA20_POLY1305_AEAD_KEYSIZE);
    if (ret == 0) {
        ret = wc_Chacha_SetIV(&aead->chacha, inIV,
            CHACHA20_POLY1305_AEAD_INITIAL_COUNTER);
    }

    /* Create the Poly1305 key */
    if (ret == 0) {
        ret = wc_Chacha_Process(&aead->chacha, authKey, authKey,
            CHACHA20_POLY1305_AEAD_KEYSIZE);
    }

    /* Initialize Poly1305 context */
    if (ret == 0) {
        ret = wc_Poly1305SetKey(&aead->poly, authKey,
            CHACHA20_POLY1305_AEAD_KEYSIZE);
    }

    /* advance counter by 1 after creating Poly1305 key */
    if (ret == 0) {
        ret = wc_Chacha_SetIV(&aead->chacha, inIV,
            CHACHA20_POLY1305_AEAD_INITIAL_COUNTER + 1);
    }

    if (ret == 0) {
        aead->state = CHACHA20_POLY1305_STATE_READY;
    }

    return ret;
}

/* optional additional authentication data */
int wc_ChaCha20Poly1305_UpdateAad(ChaChaPoly_Aead* aead,
    const byte* inAAD, word32 inAADLen)
{
    int ret = 0;

    if (aead == NULL || (inAAD == NULL && inAADLen > 0)) {
        return BAD_FUNC_ARG;
    }
    if (aead->state != CHACHA20_POLY1305_STATE_READY &&
        aead->state != CHACHA20_POLY1305_STATE_AAD) {
        return BAD_STATE_E;
    }
    if (inAADLen > CHACHA20_POLY1305_MAX - aead->aadLen)
        return CHACHA_POLY_OVERFLOW;

    if (inAAD && inAADLen > 0) {
        ret = wc_Poly1305Update(&aead->poly, inAAD, inAADLen);
        if (ret == 0) {
            aead->aadLen += inAADLen;
            aead->state = CHACHA20_POLY1305_STATE_AAD;
        }
    }

    return ret;
}

/* inData and outData can be same pointer (inline) */
int wc_ChaCha20Poly1305_UpdateData(ChaChaPoly_Aead* aead,
    const byte* inData, byte* outData, word32 dataLen)
{
    int ret = 0;

    if (aead == NULL || inData == NULL || outData == NULL) {
        return BAD_FUNC_ARG;
    }
    if (aead->state != CHACHA20_POLY1305_STATE_READY &&
        aead->state != CHACHA20_POLY1305_STATE_AAD &&
        aead->state != CHACHA20_POLY1305_STATE_DATA) {
        return BAD_STATE_E;
    }
    if (dataLen > CHACHA20_POLY1305_MAX - aead->dataLen)
        return CHACHA_POLY_OVERFLOW;

    /* Pad the AAD */
    if (aead->state == CHACHA20_POLY1305_STATE_AAD) {
        ret = wc_Poly1305_Pad(&aead->poly, aead->aadLen);
    }

    /* advance state */
    aead->state = CHACHA20_POLY1305_STATE_DATA;

    /* Perform ChaCha20 encrypt/decrypt and Poly1305 auth calc */
    if (ret == 0) {
        if (aead->isEncrypt) {
            ret = wc_Chacha_Process(&aead->chacha, outData, inData, dataLen);
            if (ret == 0)
                ret = wc_Poly1305Update(&aead->poly, outData, dataLen);
        }
        else {
            ret = wc_Poly1305Update(&aead->poly, inData, dataLen);
            if (ret == 0)
                ret = wc_Chacha_Process(&aead->chacha, outData, inData, dataLen);
        }
    }
    if (ret == 0) {
        aead->dataLen += dataLen;
    }
    return ret;
}

int wc_ChaCha20Poly1305_Final(ChaChaPoly_Aead* aead,
    byte outAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE])
{
    int ret = 0;

    if (aead == NULL || outAuthTag == NULL) {
        return BAD_FUNC_ARG;
    }
    if (aead->state != CHACHA20_POLY1305_STATE_AAD &&
        aead->state != CHACHA20_POLY1305_STATE_DATA) {
        return BAD_STATE_E;
    }

    /* Pad the AAD - Make sure it is done */
    if (aead->state == CHACHA20_POLY1305_STATE_AAD) {
        ret = wc_Poly1305_Pad(&aead->poly, aead->aadLen);
    }

    /* Pad the plaintext/ciphertext to 16 bytes */
    if (ret == 0) {
        ret = wc_Poly1305_Pad(&aead->poly, aead->dataLen);
    }

    /* Add the aad length and plaintext/ciphertext length */
    if (ret == 0) {
        ret = wc_Poly1305_EncodeSizes(&aead->poly, aead->aadLen,
            aead->dataLen);
    }

    /* Finalize the auth tag */
    if (ret == 0) {
        ret = wc_Poly1305Final(&aead->poly, outAuthTag);
    }

    /* reset and cleanup sensitive context */
    ForceZero(aead, sizeof(ChaChaPoly_Aead));

    return ret;
}

#ifdef HAVE_XCHACHA

int wc_XChaCha20Poly1305_Init(
    ChaChaPoly_Aead *aead,
    const byte *ad, word32 ad_len,
    const byte *nonce, word32 nonce_len,
    const byte *key, word32 key_len,
    int isEncrypt)
{
    byte authKey[CHACHA20_POLY1305_AEAD_KEYSIZE];
    int ret;

    if ((ad == NULL) || (nonce == NULL) || (key == NULL))
        return BAD_FUNC_ARG;

    if ((key_len != CHACHA20_POLY1305_AEAD_KEYSIZE) ||
        (nonce_len != XCHACHA20_POLY1305_AEAD_NONCE_SIZE))
        return BAD_FUNC_ARG;

    if ((ret = wc_XChacha_SetKey(&aead->chacha,
                                 key, key_len,
                                 nonce, nonce_len,
                                 0 /* counter */)) < 0)
        return ret;

    XMEMSET(authKey, 0, sizeof authKey);

    /* Create the Poly1305 key */
    if ((ret = wc_Chacha_Process(&aead->chacha, authKey, authKey,
                                 (word32)sizeof authKey)) < 0)
        return ret;
    /* advance to start of the next ChaCha block. */
    wc_Chacha_purge_current_block(&aead->chacha);

    /* Initialize Poly1305 context */
    if ((ret = wc_Poly1305SetKey(&aead->poly, authKey,
                                 (word32)sizeof authKey)) < 0)
        return ret;

    if ((ret = wc_Poly1305Update(&aead->poly, ad, (word32)ad_len)) < 0)
        return ret;

    if ((ret = wc_Poly1305_Pad(&aead->poly, (word32)ad_len)) < 0)
        return ret;

    aead->isEncrypt = isEncrypt ? 1 : 0;
    aead->state = CHACHA20_POLY1305_STATE_AAD;

    return 0;
}

static WC_INLINE int wc_XChaCha20Poly1305_crypt_oneshot(
    byte *dst, const size_t dst_space,
    const byte *src, const size_t src_len,
    const byte *ad, const size_t ad_len,
    const byte *nonce, const size_t nonce_len,
    const byte *key, const size_t key_len,
    int isEncrypt)
{
    int ret;
    long int dst_len = isEncrypt ?
        (long int)src_len + POLY1305_DIGEST_SIZE :
        (long int)src_len - POLY1305_DIGEST_SIZE;
    const byte *src_i;
    byte *dst_i;
    size_t src_len_rem;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    ChaChaPoly_Aead *aead = (ChaChaPoly_Aead *)XMALLOC(sizeof *aead, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (aead == NULL)
        return MEMORY_E;
#else
    ChaChaPoly_Aead aead_buf, *aead = &aead_buf;
#endif

    if ((dst == NULL) || (src == NULL)) {
        ret = BAD_FUNC_ARG;
        goto out;
    }

    if ((long int)dst_space < dst_len) {
        ret = BUFFER_E;
        goto out;
    }

    if ((ret = wc_XChaCha20Poly1305_Init(aead, ad, (word32)ad_len,
                                         nonce, (word32)nonce_len,
                                         key, (word32)key_len, 1)) < 0)
        goto out;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_XChaCha20Poly1305_crypt_oneshot aead", aead,
        sizeof(ChaChaPoly_Aead));
#endif

    /* process the input in 16k pieces to accommodate src_lens that don't fit in a word32,
     * and to exploit hot cache for the input data.
     */
    src_i = src;
    src_len_rem = isEncrypt ? src_len : (size_t)dst_len;
    dst_i = dst;
    while (src_len_rem > 0) {
        word32 this_src_len =
            (src_len_rem > 16384) ?
            16384 :
            (word32)src_len_rem;

        if ((ret = wc_Chacha_Process(&aead->chacha, dst_i, src_i, this_src_len)) < 0)
            goto out;

        if ((ret = wc_Poly1305Update(&aead->poly, isEncrypt ? dst_i : src_i, this_src_len)) < 0)
            goto out;

        src_len_rem -= (size_t)this_src_len;
        src_i += this_src_len;
        dst_i += this_src_len;
    }

    if (aead->poly.leftover) {
        if ((ret = wc_Poly1305_Pad(&aead->poly, (word32)aead->poly.leftover)) < 0)
            return ret;
    }

#ifdef WORD64_AVAILABLE
    ret = wc_Poly1305_EncodeSizes64(&aead->poly, ad_len, isEncrypt ? src_len : (size_t)dst_len);
#else
    ret = wc_Poly1305_EncodeSizes(&aead->poly, ad_len, isEncrypt ? src_len : (size_t)dst_len);
#endif
    if (ret < 0)
        goto out;

    if (isEncrypt)
        ret = wc_Poly1305Final(&aead->poly, dst + src_len);
    else {
        byte outAuthTag[POLY1305_DIGEST_SIZE];

        if ((ret = wc_Poly1305Final(&aead->poly, outAuthTag)) < 0)
            goto out;

        if (ConstantCompare(outAuthTag, src + dst_len, POLY1305_DIGEST_SIZE) != 0) {
            ret = MAC_CMP_FAILED_E;
            goto out;
        }
    }

  out:

    ForceZero(aead, sizeof *aead);

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    XFREE(aead, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(aead, sizeof(ChaChaPoly_Aead));
#endif

    return ret;
}

int wc_XChaCha20Poly1305_Encrypt(
    byte *dst, const size_t dst_space,
    const byte *src, const size_t src_len,
    const byte *ad, const size_t ad_len,
    const byte *nonce, const size_t nonce_len,
    const byte *key, const size_t key_len)
{
    return wc_XChaCha20Poly1305_crypt_oneshot(dst, dst_space, src, src_len, ad, ad_len, nonce, nonce_len, key, key_len, 1);
}

int wc_XChaCha20Poly1305_Decrypt(
    byte *dst, const size_t dst_space,
    const byte *src, const size_t src_len,
    const byte *ad, const size_t ad_len,
    const byte *nonce, const size_t nonce_len,
    const byte *key, const size_t key_len)
{
    return wc_XChaCha20Poly1305_crypt_oneshot(dst, dst_space, src, src_len, ad, ad_len, nonce, nonce_len, key, key_len, 0);
}

#endif /* HAVE_XCHACHA */

#endif /* HAVE_CHACHA && HAVE_POLY1305 */
