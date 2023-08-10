/* wolfcaam_aes.c
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

#if defined(WOLFSSL_CAAM) && !defined(NO_AES)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_aes.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if (defined(HAVE_AESGCM) && defined(WOLFSSL_CAAM_AESGCM)) || \
    defined(HAVE_AESCCM)
/* return 0 on success */
static int wc_CAAM_AesAeadCommon(Aes* aes, const byte* in, byte* out, word32 sz,
        const byte* nonce, word32 nonceSz, byte* authTag, word32 authTagSz,
        const byte* authIn, word32 authInSz, int dir, int type)
{
    CAAM_BUFFER buf[7];
    int ret, idx = 0;
    word32 arg[4];
    word32 keySz;

    if (aes == NULL) {
        return BAD_FUNC_ARG;
    }

    if (wc_AesGetKeySize(aes, &keySz) != 0 && aes->blackKey == 0) {
        return BAD_FUNC_ARG;
    }

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)aes->devKey;
    buf[idx].Length     = keySz;
    idx++;

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)nonce;
    buf[idx].Length     = nonceSz;
    idx++;

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)in;
    buf[idx].Length     = sz;
    idx++;

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)out;
    buf[idx].Length     = sz;
    idx++;

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)authTag;
    buf[idx].Length     = authTagSz;
    idx++;

    buf[idx].BufferType = DataBuffer | LastBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)authIn;
    buf[idx].Length     = authInSz;
    idx++;

    /* authInSz must fit into a short (note that only 16 bits are ava in CAAM
     * for AAD size anyway) */
    arg[0] = ((authInSz & 0xFFFF)  << 16) | (dir & 0xFFFF);
    arg[1] = ((nonceSz & 0xFF) << 24) | ((authTagSz & 0xFF) << 16) |
                (keySz & 0xFFFF);
    arg[2] = sz;
    arg[3] = aes->blackKey;

    if ((ret = wc_caamAddAndWait(buf, idx, arg, type)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES AEAD operation");
        return ret;
    }

    return 0;
}
#endif /* HAVE_AESGCM || HAVE_AESCCM */


#if defined(HAVE_AESCCM)
#ifndef WOLFSSL_SECO_CAAM
/* B0 is [ reserved | adata | M | L ] [ nonce ] [ l(m) ]
 * Ctr is current counter
 */
static word32 CreateB0CTR(byte* B0Ctr0, const byte* nonce, word32 nonceSz,
    word32 authInSz, word32 authTagSz, word32 inSz)
{
    word32 i;
    word32 lenSz;
    byte mask = 0xFF;
    const word32 wordSz = (word32)sizeof(word32);

    /* set up B0 and CTR0 similar to how wolfcrypt/src/aes.c does */
    XMEMCPY(B0Ctr0+1, nonce, nonceSz);
    XMEMCPY(B0Ctr0+AES_BLOCK_SIZE+1, nonce, nonceSz);
    lenSz = AES_BLOCK_SIZE - 1 - (byte)nonceSz;
    B0Ctr0[0] = 0;
    if (authInSz > 0) {
        B0Ctr0[0] |= 0x40; /* set aad bit */
    }

    /* add size of tag encoded as (sz - 2)/2 (i.e M) */
    B0Ctr0[0] |= (((byte)authTagSz - 2) / 2) << 3;

    /* size of l(m) */
    B0Ctr0[0] |= lenSz - 1;

    /* add in l(m), length of message to be encrypted and sent */
    for (i = 0; i < lenSz; i++) {
        if (mask && i >= wordSz)
            mask = 0x00;
        B0Ctr0[AES_BLOCK_SIZE - 1 - i] = (inSz >> ((8 * i) & mask)) & mask;
        B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE - 1 - i] = 0;
    }
    B0Ctr0[AES_BLOCK_SIZE] = lenSz - 1;

    return 0;
}
#endif


/* plaintext in ciphertext and mac out
 * return 0 on success
 */
int wc_CAAM_AesCcmEncrypt(Aes* aes, const byte* in, byte* out, word32 sz,
        const byte* nonce, word32 nonceSz, byte* authTag, word32 authTagSz,
        const byte* authIn, word32 authInSz)
{
#ifndef WOLFSSL_SECO_CAAM
    byte B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
#endif

    if (aes == NULL || (sz != 0 && (in == NULL || out == NULL)) ||
        nonce == NULL || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
            authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    /* sanity check on tag size */
    if (wc_AesCcmCheckTagSize(authTagSz) != 0) {
        return BAD_FUNC_ARG;
    }

#ifndef WOLFSSL_SECO_CAAM
    CreateB0CTR(B0Ctr0, nonce, nonceSz, authInSz, authTagSz, sz);
    return wc_CAAM_AesAeadCommon(aes, in, out, sz, B0Ctr0, 2*AES_BLOCK_SIZE,
        authTag, authTagSz, authIn, authInSz, CAAM_ENC, CAAM_AESCCM);
#else
    return wc_CAAM_AesAeadCommon(aes, in, out, sz, nonce, nonceSz,
        authTag, authTagSz, authIn, authInSz, CAAM_ENC, CAAM_AESCCM);
#endif
}


/* ciphertext in plaintext out
 * return 0 on success
 */
int wc_CAAM_AesCcmDecrypt(Aes* aes, const byte* in, byte* out, word32 sz,
        const byte* nonce, word32 nonceSz, const byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz)
{
    int ret;
#ifndef WOLFSSL_SECO_CAAM
    byte B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
#endif

    /* sanity check on arguments */
    if (aes == NULL || (sz != 0 && (in == NULL || out == NULL)) ||
        nonce == NULL || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
        authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    /* sanity check on tag size */
    if (wc_AesCcmCheckTagSize(authTagSz) != 0) {
        return BAD_FUNC_ARG;
    }

#ifndef WOLFSSL_SECO_CAAM
    CreateB0CTR(B0Ctr0, nonce, nonceSz, authInSz, authTagSz, sz);
    ret = wc_CAAM_AesAeadCommon(aes, in, out, sz, B0Ctr0, 2*AES_BLOCK_SIZE,
            (byte*)authTag, authTagSz, authIn, authInSz, CAAM_DEC, CAAM_AESCCM);
#else
    ret = wc_CAAM_AesAeadCommon(aes, in, out, sz, nonce, nonceSz,
            (byte*)authTag, authTagSz, authIn, authInSz, CAAM_DEC, CAAM_AESCCM);
#endif

    if (ret != 0) {
        /* If the authTag check fails, don't keep the decrypted data.
         * Unfortunately, you need the decrypted data to calculate the
         * check value. */
        #if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2) && \
            defined(ACVP_VECTOR_TESTING)
            WOLFSSL_MSG("Preserve output for vector responses");
        #else
            if (sz > 0)
                XMEMSET(out, 0, sz);
        #endif
    }

    return ret;
}
#endif /* HAVE_AESCCM */


#if defined(HAVE_AESGCM) && defined(WOLFSSL_CAAM_AESGCM)
int wc_CAAM_AesGcmEncrypt(Aes* aes, const byte* in, byte* out, word32 sz,
        const byte* nonce, word32 nonceSz, byte* authTag, word32 authTagSz,
        const byte* authIn, word32 authInSz)
{
    return wc_CAAM_AesAeadCommon(aes, in, out, sz, nonce, nonceSz, authTag,
                            authTagSz, authIn, authInSz, CAAM_ENC, CAAM_AESGCM);
}


int wc_CAAM_AesGcmDecrypt(Aes* aes, const byte* in, byte* out, word32 sz,
        const byte* nonce, word32 nonceSz, const byte* authTag,
        word32 authTagSz, const byte* authIn, word32 authInSz)
{
    return wc_CAAM_AesAeadCommon(aes, in, out, sz, nonce, nonceSz,
            (byte*)authTag, authTagSz, authIn, authInSz, CAAM_DEC, CAAM_AESGCM);
}
#endif


static int wc_CAAM_AesCbcCtrCommon(Aes* aes, byte* out, const byte* in,
    word32 sz, int dir, int mode)
{
    word32  blocks;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if (blocks > 0) {
        CAAM_BUFFER buf[5];
        word32 arg[4];
        word32 keySz;
        int ret;

        if (wc_AesGetKeySize(aes, &keySz) != 0 && aes->blackKey == 0) {
            return BAD_FUNC_ARG;
        }

        /* Set buffers for key, cipher text, and plain text */
        buf[0].BufferType = DataBuffer;
        buf[0].TheAddress = (CAAM_ADDRESS)(void*)aes->devKey;
        buf[0].Length     = keySz;

        buf[1].BufferType = DataBuffer;
        buf[1].TheAddress = (CAAM_ADDRESS)aes->reg;
        buf[1].Length     = AES_BLOCK_SIZE;

        buf[2].BufferType = DataBuffer;
        buf[2].TheAddress = (CAAM_ADDRESS)in;
        buf[2].Length     = blocks * AES_BLOCK_SIZE;

        buf[3].BufferType = DataBuffer | LastBuffer;
        buf[3].TheAddress = (CAAM_ADDRESS)out;
        buf[3].Length     = blocks * AES_BLOCK_SIZE;

        /* buffer for updated IV */
        buf[4].BufferType = DataBuffer;
        buf[4].TheAddress = (CAAM_ADDRESS)aes->reg;
        buf[4].Length     = AES_BLOCK_SIZE;

        arg[0] = dir;
        arg[1] = keySz;
        arg[2] = blocks * AES_BLOCK_SIZE;
        arg[3] = aes->blackKey;

        if ((ret = wc_caamAddAndWait(buf, 5, arg, mode)) != 0) {
            WOLFSSL_MSG("Error with CAAM AES CBC operation");
            return ret;
        }
    }

    return 0;
}

#ifdef WOLFSSL_AES_COUNTER
/* Increment AES counter (from wolfcrypt/src/aes.c) */
static WC_INLINE void IncrementAesCounter(byte* inOutCtr)
{
    /* in network byte order so start at end and work back */
    int i;
    for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
        if (++inOutCtr[i])  /* we're done unless we overflow */
            return;
    }
}

int wc_CAAM_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    byte*  tmp;
    word32 keySz;
    int ret, blocks;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
        return ret;
    }

    /* consume any unused bytes left in aes->tmp */
    tmp = (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left;
    while (aes->left && sz) {
        *(out++) = *(in++) ^ *(tmp++);
        aes->left--;
        sz--;
    }

    /* do full blocks to then get potential left over amount */
    blocks = sz / AES_BLOCK_SIZE;
    if (blocks > 0) {
        ret = wc_CAAM_AesCbcCtrCommon(aes, out, in, blocks * AES_BLOCK_SIZE,
            CAAM_ENC, CAAM_AESCTR);

        out += blocks * AES_BLOCK_SIZE;
        in  += blocks * AES_BLOCK_SIZE;
        sz  -= blocks * AES_BLOCK_SIZE;
    }

    if (sz) {
        if ((ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg))
            != 0) {
            return ret;
        }
        IncrementAesCounter((byte*)aes->reg);

        aes->left = AES_BLOCK_SIZE;
        tmp = (byte*)aes->tmp;

        while (sz--) {
            *(out++) = *(in++) ^ *(tmp++);
            aes->left--;
        }
    }

    return ret;
}
#endif /* WOLFSSL_AES_COUNTER */

int wc_CAAM_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_CAAM_AesCbcCtrCommon(aes, out, in, sz, CAAM_ENC, CAAM_AESCBC);
}


int wc_CAAM_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_CAAM_AesCbcCtrCommon(aes, out, in, sz, CAAM_DEC, CAAM_AESCBC);
}

#if defined(HAVE_AES_ECB)
static int wc_CAAM_AesEcbCommon(Aes* aes, byte* out, const byte* in, word32 sz,
    int dir)
{
    word32 blocks;
    CAAM_BUFFER buf[4];
    word32 arg[4];
    word32 keySz = 0;
    int    ret;
    int    idx = 0;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if (wc_AesGetKeySize(aes, &keySz) != 0 && aes->blackKey == 0) {
        return BAD_FUNC_ARG;
    }

    /* Set buffers for key, cipher text, and plain text */
    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)aes->devKey;
    buf[idx].Length     = keySz;
    idx++;

    buf[idx].BufferType = DataBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)in;
    buf[idx].Length     = blocks * AES_BLOCK_SIZE;
    idx++;

    buf[idx].BufferType = DataBuffer | LastBuffer;
    buf[idx].TheAddress = (CAAM_ADDRESS)out;
    buf[idx].Length     = blocks * AES_BLOCK_SIZE;
    idx++;

    arg[0] = dir;
    arg[1] = keySz;
    arg[2] = blocks * AES_BLOCK_SIZE;
    arg[3] = aes->blackKey;

    if ((ret = wc_caamAddAndWait(buf, idx, arg, CAAM_AESECB)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES ECB encrypt");
        return ret;
    }

    return 0;
}


/* is assumed that input size is a multiple of AES_BLOCK_SIZE */
int wc_CAAM_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_CAAM_AesEcbCommon(aes, out, in, sz, CAAM_ENC);
}


int wc_CAAM_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_CAAM_AesEcbCommon(aes, out, in, sz, CAAM_DEC);
}
#endif /* HAVE_AES_ECB */
#endif /* WOLFSSL_CAAM && !NO_AES */
