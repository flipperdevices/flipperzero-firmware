/* aria-crypt.c
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
This library provides the interfaces to the ARIA cipher, an encryption algorithm
developed by the Korean Agency for Technology (KATS). It uses a 128-bit block
size and a key size of 128, 192, or 256 bits.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ARIA

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/port/aria/aria-crypt.h>

/* return 0 on success or WC_INIT_E on failure */
int wc_AriaInitCrypt(wc_Aria* aria, MC_ALGID algo)
{
    MC_RV rv = MC_OK;

    MC_APIMODE gApimode = MC_MODE_KCMV;
    MC_ALGMODE algMode = MC_ALGMODE_GCM;
    MC_PADTYPE algPad = MC_PADTYPE_NONE;

    if (aria == NULL)
        return BAD_FUNC_ARG;

    if (rv == MC_OK)
        rv = MC_Initialize(NULL);

    if (rv == MC_OK)
        rv = wc_AriaFreeCrypt(aria);

    if (rv == MC_OK)
        rv = MC_OpenSession(&(aria->hSession));

    if (rv == MC_OK)
        rv = MC_SetApiMode(aria->hSession, gApimode);

    if (rv == MC_OK)
        rv = MC_SetOption(aria->hSession, algMode, algPad);

    if (rv == MC_OK) {
        aria->algo = algo;
        XMEMSET(aria->nonce,0,sizeof(aria->nonce));
        aria->nonceSz = 0;
    }

    if (rv != MC_OK) {
        if (aria->hSession != NULL) {
            MC_CloseSession(aria->hSession);
            aria->hSession = NULL;
        }
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return WC_INIT_E;
    }
    return 0;
}

/* return 0 on success or BAD_STATE_E on failure */
int wc_AriaFreeCrypt(wc_Aria* aria)
{
    MC_RV rv = MC_OK;

    if (aria == NULL)
        return 0;

    if (aria->hKey != NULL) {
        if (rv == MC_OK) rv = MC_DestroyObject(aria->hSession, aria->hKey);
        if (rv == MC_OK) aria->hKey = NULL;
    }
    if (aria->hSession != NULL) {
        if (rv == MC_OK) rv = MC_CloseSession(aria->hSession);
        if (rv == MC_OK) aria->hSession = NULL;
    }

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_STATE_E;
    }
    return 0;
}

/* return 0 on success or BAD_FUNC_ARG/PUBLIC_KEY_E on failure */
int wc_AriaSetKey(wc_Aria* aria, byte* key)
{
    MC_RV rv = MC_OK;
    MC_UINT keylen;
    if (aria->algo == MC_ALGID_ARIA_128BITKEY) {
        keylen = ARIA_128_KEY_SIZE;
    } else if (aria->algo == MC_ALGID_ARIA_192BITKEY) {
        keylen = ARIA_192_KEY_SIZE;
    } else if (aria->algo == MC_ALGID_ARIA_256BITKEY) {
        keylen = ARIA_256_KEY_SIZE;
    } else {
        WOLFSSL_MSG_EX("Unsupported algorithm: %d", aria->algo);
        return PUBLIC_KEY_E;
    }

    if (aria->hKey != NULL) {
        if (rv == MC_OK)
            rv = MC_DestroyObject(aria->hSession, aria->hKey);
        if (rv == MC_OK)
            aria->hKey = NULL;
    }
    if (rv == MC_OK)
        rv = MC_CreateObject(aria->hSession, (MC_UCHAR*)key, keylen, &(aria->hKey));

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_FUNC_ARG;
    }
    return 0;
}

static WARN_UNUSED_RESULT WC_INLINE int CheckAriaGcmIvSize(int ivSz) {
    return (ivSz == GCM_NONCE_MIN_SZ ||
            ivSz == GCM_NONCE_MID_SZ ||
            ivSz == GCM_NONCE_MAX_SZ);
}

/* return 0 on success or BAD_FUNC_ARG on failure */
int wc_AriaGcmSetExtIV(wc_Aria* aria, const byte* iv, word32 ivSz)
{
    int ret = 0;

    if (aria == NULL || iv == NULL || !CheckAriaGcmIvSize((int)ivSz)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMCPY((byte*)aria->nonce, iv, ivSz);
        aria->nonceSz = ivSz;
    }

    return ret;
}

/* return 0 on success or BAD_FUNC_ARG on failure */
int wc_AriaGcmSetIV(wc_Aria* aria, word32 ivSz,
                const byte* ivFixed, word32 ivFixedSz,
                WC_RNG* rng)
{
    int ret = 0;

    if (aria == NULL || rng == NULL || !CheckAriaGcmIvSize((int)ivSz) ||
        (ivFixed == NULL && ivFixedSz != 0) ||
        (ivFixed != NULL && ivFixedSz != AES_IV_FIXED_SZ)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        byte* iv = (byte*)aria->nonce;

        if (ivFixedSz)
            XMEMCPY(iv, ivFixed, ivFixedSz);

        ret = wc_RNG_GenerateBlock(rng, iv + ivFixedSz, ivSz - ivFixedSz);
    }

    if (ret == 0) {
        aria->nonceSz = ivSz;
    }

    return ret;
}

/* 'out' buffer is expected to be 'inSz + authTagSz'
    * return 0 on success or BAD_FUNC_ARG/ENCRYPT_ERROR on failure */
int wc_AriaEncrypt(wc_Aria* aria, byte* out, byte* in, word32 inSz,
                        byte* iv, word32 ivSz, byte* aad, word32 aadSz,
                        byte* authTag, word32 authTagSz)
{
    MC_RV rv = MC_OK;

    MC_ALGPARAM param;
    MC_UINT outSz = inSz + authTagSz;
    MC_ALGORITHM mcAlg;
    XMEMSET(&param,0,sizeof(MC_ALGPARAM));
    param.pNonce = iv;
    param.pAData = aad;
    param.nNonce = ivSz;
    param.nAData = aadSz;
    param.nTLen = authTagSz;
    param.nDataLen = inSz;
    XMEMSET(&mcAlg,0,sizeof(MC_ALGORITHM));
    mcAlg.mcAlgId = aria->algo;
    mcAlg.pParam = (MC_UCHAR*)&param;
    mcAlg.nParam = sizeof(param);

    if (authTag == NULL || iv == NULL || authTagSz > ARIA_BLOCK_SIZE ||
                authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ ||
                ivSz == 0 || ivSz > ARIA_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }
    if (out == NULL || in == NULL || inSz == 0) {
        return BAD_FUNC_ARG;
    }
    if (out <= in + inSz && in <= out + inSz) { /* check for overlapping range */
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = MC_EncryptInit(aria->hSession, &mcAlg, aria->hKey);

    if (rv == MC_OK)
        rv = MC_Encrypt(aria->hSession, in, inSz, out, &outSz);

    if (rv == MC_OK)
        XMEMCPY(authTag, out + inSz, authTagSz);

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return ENCRYPT_ERROR;
    }
    return 0;
}

/* 'in' buffer is expected to be 'inSz + authTagSz'
    * return 0 on success or BAD_FUNC_ARG/ENCRYPT_ERROR on failure */
int wc_AriaDecrypt(wc_Aria* aria, byte* out, byte* in, word32 inSz,
                        byte* iv, word32 ivSz, byte* aad, word32 aadSz,
                        byte* authTag, word32 authTagSz)
{
    MC_RV rv = MC_OK;

    MC_ALGPARAM param;
    MC_UINT outSz = inSz;
    MC_ALGORITHM mcAlg;
    XMEMSET(&param,0,sizeof(MC_ALGPARAM));
    param.pNonce = iv;
    param.pAData = aad;
    param.nNonce = ivSz;
    param.nAData = aadSz;
    param.nTLen = authTagSz;
    param.nDataLen = inSz;
    XMEMSET(&mcAlg,0,sizeof(MC_ALGORITHM));
    mcAlg.mcAlgId = aria->algo;
    mcAlg.pParam = (MC_UCHAR*)&param;
    mcAlg.nParam = sizeof(param);

    if (authTag == NULL || iv == NULL || authTagSz > ARIA_BLOCK_SIZE ||
                authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ ||
                ivSz == 0 || ivSz > ARIA_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }
    if (out == NULL || in == NULL || inSz == 0) {
        return BAD_FUNC_ARG;
    }
    if (out <= in + inSz && in <= out + inSz) { /* check for overlapping range */
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = MC_DecryptInit(aria->hSession, &mcAlg, aria->hKey);

    if (rv == MC_OK) {
        XMEMCPY((byte*)in + inSz, authTag, authTagSz);
        inSz += authTagSz;
    }
    if (rv == MC_OK)
        rv = MC_Decrypt(aria->hSession, in, inSz, out, &outSz);

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return DECRYPT_ERROR;
    }
    return 0;
}
#endif /* HAVE_ARIA */
