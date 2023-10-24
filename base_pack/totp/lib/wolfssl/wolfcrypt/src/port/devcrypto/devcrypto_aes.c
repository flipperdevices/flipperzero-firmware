/* devcrypto_aes.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>

#if !defined(NO_AES) && defined(WOLFSSL_DEVCRYPTO)

#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#if defined(HAVE_AES_CBC) && defined(WOLFSSL_DEVCRYPTO_CBC)
int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    struct crypt_op crt;
    int ret;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    /* encrypt only up to AES block size of date */
    sz = sz - (sz % AES_BLOCK_SIZE);
    if (aes->ctx.cfd == -1) {
            ret = wc_DevCryptoCreate(&aes->ctx, CRYPTO_AES_CBC,
                    (byte*)aes->devKey, aes->keylen);
            if (ret != 0)
                return ret;
    }
    wc_SetupCryptSym(&crt, &aes->ctx, (byte*)in, sz, out, (byte*)aes->reg,
            COP_ENCRYPT);
    ret = ioctl(aes->ctx.cfd, CIOCCRYPT, &crt);
    if (ret != 0) {
        return WC_DEVCRYPTO_E;
    }

    /* store iv for next call */
    XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

    return 0;
}

#ifdef HAVE_AES_DECRYPT
int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    struct crypt_op crt;
    int ret;

    if (aes == NULL || out == NULL || in == NULL || sz % AES_BLOCK_SIZE != 0) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(aes->tmp, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    if (aes->ctx.cfd == -1) {
        ret = wc_DevCryptoCreate(&aes->ctx, CRYPTO_AES_CBC,
                    (byte*)aes->devKey, aes->keylen);
        if (ret != 0)
            return ret;
    }
    wc_SetupCryptSym(&crt, &aes->ctx, (byte*)in, sz, out, (byte*)aes->reg,
            COP_DECRYPT);
    ret = ioctl(aes->ctx.cfd, CIOCCRYPT, &crt);
    if (ret != 0) {
        return WC_DEVCRYPTO_E;
    }

    XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
    return 0;
}
#endif /* HAVE_AES_DECRYPT */
#endif /* HAVE_AES_CBC && WOLFSSL_DEVCRYPTO_CBC */


#ifdef WOLFSSL_DEVCRYPTO_AES /* all AES algorithms supported */
int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
    const byte* iv, int dir)
{
#if defined(AES_MAX_KEY_SIZE)
    const word32 max_key_len = (AES_MAX_KEY_SIZE / 8);
#endif

    if (aes == NULL ||
            !((keylen == 16) || (keylen == 24) || (keylen == 32))) {
        return BAD_FUNC_ARG;
    }

#if defined(AES_MAX_KEY_SIZE)
    /* Check key length */
    if (keylen > max_key_len) {
        return BAD_FUNC_ARG;
    }
#endif
    aes->keylen = keylen;
    aes->rounds = keylen/4 + 6;

#ifdef WOLFSSL_AES_COUNTER
    aes->left = 0;
#endif
    aes->ctx.cfd = -1;
    XMEMCPY(aes->devKey, userKey, keylen);

    (void)dir;
    return wc_AesSetIV(aes, iv);
}


/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT) || defined(HAVE_AES_ECB)

/* common code between ECB encrypt and decrypt
 * returns 0 on success */
static int wc_DevCrypto_AesDirect(Aes* aes, byte* out, const byte* in,
        word32 sz, int dir)
{
    int ret;
    struct crypt_op crt;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (aes->ctx.cfd == -1) {
        ret = wc_DevCryptoCreate(&aes->ctx, CRYPTO_AES_ECB, (byte*)aes->devKey,
                aes->keylen);
        if (ret != 0)
            return ret;
    }

    wc_SetupCryptSym(&crt, &aes->ctx, (byte*)in, sz, out, NULL, dir);
    ret = ioctl(aes->ctx.cfd, CIOCCRYPT, &crt);
    if (ret != 0) {
        return WC_DEVCRYPTO_E;
    }
    return 0;
}
#endif


#if defined(WOLFSSL_AES_DIRECT) || defined(HAVE_AESCCM)
int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in)
{
    return wc_DevCrypto_AesDirect(aes, out, in, AES_BLOCK_SIZE, COP_ENCRYPT);
}


int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in)
{
    return wc_DevCrypto_AesDirect(aes, out, in, AES_BLOCK_SIZE, COP_DECRYPT);
}


int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                    const byte* iv, int dir)
{
    return wc_AesSetKey(aes, userKey, keylen, iv, dir);
}
#endif


/* AES-CTR */
#if defined(WOLFSSL_AES_COUNTER)

/* Increment AES counter */
static WC_INLINE void IncrementAesCounter(byte* inOutCtr)
{
    /* in network byte order so start at end and work back */
    int i;
    for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
        if (++inOutCtr[i])  /* we're done unless we overflow */
            return;
    }
}

int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;
    struct crypt_op crt;
    byte* tmp;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    /* consume any unused bytes left in aes->tmp */
    tmp = (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left;
    while (aes->left && sz) {
        *(out++) = *(in++) ^ *(tmp++);
        aes->left--;
        sz--;
    }

    if (aes->ctx.cfd == -1) {
        ret = wc_DevCryptoCreate(&aes->ctx, CRYPTO_AES_CTR, (byte*)aes->devKey,
                aes->keylen);
        if (ret != 0)
            return ret;
    }

    if (sz > 0) {
        /* clear previously leftover data */
        tmp = (byte*)aes->tmp;
        XMEMSET(tmp, 0, AES_BLOCK_SIZE);

        /* update IV */
        wc_SetupCryptSym(&crt, &aes->ctx, (byte*)in, sz, out, (byte*)aes->reg,
                COP_ENCRYPT);
        ret = ioctl(aes->ctx.cfd, CIOCCRYPT, &crt);
        if (ret != 0) {
            return WC_DEVCRYPTO_E;
        }

        /* adjust counter after call to hardware */
        while (sz >= AES_BLOCK_SIZE) {
            IncrementAesCounter((byte*)aes->reg);
            sz  -= AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
            in  += AES_BLOCK_SIZE;
        }
    }

    /* create key stream for later if needed */
    if (sz > 0) {
        Aes tmpAes;
        if ((ret = wc_AesSetKey(&tmpAes, (byte*)aes->devKey, aes->keylen,
                                (byte*)aes->reg, AES_ENCRYPTION)) != 0)
            return ret;
        if ((ret = wc_AesEncryptDirect(&tmpAes, (byte*)aes->tmp,
                                       (const byte*)aes->reg)) != 0)
            return ret;
        wc_AesFree(&tmpAes);
        IncrementAesCounter((byte*)aes->reg);

        aes->left = AES_BLOCK_SIZE - (sz % AES_BLOCK_SIZE);
    }

    return 0;
}
#endif /* WOLFSSL_AES_COUNTER */


#ifdef HAVE_AESGCM

int wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len)
{
    return wc_AesSetKey(aes, key, len, NULL, AES_ENCRYPTION);
}



/* common code for AES-GCM encrypt/decrypt */
static int wc_DevCrypto_AesGcm(Aes* aes, byte* out, byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz,
                   int dir)
{
    struct crypt_auth_op crt = {0};
    int ret;
    byte scratch[AES_BLOCK_SIZE];

    /* argument checks */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }

    /* Account for NULL in/out buffers. Up to tag size is still written into
     * in/out buffers */
    if (out == NULL)
        out = scratch;
    if (in == NULL)
        in = scratch;

    XMEMSET(scratch, 0, AES_BLOCK_SIZE);
    if (aes->ctx.cfd == -1) {
        ret = wc_DevCryptoCreate(&aes->ctx, CRYPTO_AES_GCM, (byte*)aes->devKey,
                aes->keylen);
        if (ret != 0)
            return ret;
    }

    /* if decrypting then the tag is expected to be at the end of "in" buffer */
    if (dir == COP_DECRYPT) {
        XMEMCPY(in + sz, authTag, authTagSz);
        sz += authTagSz;
    }
    else{
        /* get full tag from hardware */
        authTagSz = AES_BLOCK_SIZE;
    }
    wc_SetupCryptAead(&crt, &aes->ctx, (byte*)in, sz, out, (byte*)iv, ivSz,
                      dir, (byte*)authIn, authInSz, authTag, authTagSz);
    ret = ioctl(aes->ctx.cfd, CIOCAUTHCRYPT, &crt);
    if (ret != 0) {
        if (dir == COP_DECRYPT) {
            return AES_GCM_AUTH_E;
        }
        else {
            return WC_DEVCRYPTO_E;
        }
    }

    /* after encryption the tag has been placed at the end of "out" buffer */
    if (dir == COP_ENCRYPT) {
        XMEMCPY(authTag, out + sz, authTagSz);
    }
    return 0;
}


/* it is assumed that "out" buffer has enough room for cipher text + tag */
int wc_AesGcmEncrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    return wc_DevCrypto_AesGcm(aes, out, (byte*)in, sz, iv, ivSz,
                               authTag, authTagSz, authIn, authInSz,
                               COP_ENCRYPT);
}

#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)
/* it is assumed that "in" buffer has enough room for cipher text + tag */
int wc_AesGcmDecrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                     const byte* iv, word32 ivSz,
                     const byte* authTag, word32 authTagSz,
                     const byte* authIn, word32 authInSz)
{
    return wc_DevCrypto_AesGcm(aes, out, (byte*)in, sz, iv, ivSz,
                               (byte*)authTag, authTagSz, authIn, authInSz,
                               COP_DECRYPT);
}
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */
#endif /* HAVE_AESGCM */


#ifdef HAVE_AES_ECB
int wc_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_DevCrypto_AesDirect(aes, out, in, sz, COP_ENCRYPT);
}


int wc_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_DevCrypto_AesDirect(aes, out, in, sz, COP_DECRYPT);
}
#endif /* HAVE_AES_ECB */
#endif /* WOLFSSL_DEVCRYPTO_AES */
#endif /* !NO_AES && WOLFSSL_DEVCRYPTO */

