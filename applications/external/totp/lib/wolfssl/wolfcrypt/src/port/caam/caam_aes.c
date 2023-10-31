/* caam_aes.c
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

#if defined(WOLFSSL_IMX6_CAAM) && !defined(NO_AES) && \
   !defined(NO_IMX6_CAAM_AES)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/aes.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/port/caam/caam_driver.h>

#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
#include <stdio.h>
#endif

int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir)
{
    int ret;

    if (aes == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (len > 32) {
        byte out[32]; /* max AES key size */
        word32 outSz;

        if (len != 64 && len != 72 && len != 80) {
            return BAD_FUNC_ARG;
        }

        outSz = sizeof(out);
        /* if length greater then 32 then try to unencapsulate */
        if ((ret = wc_caamOpenBlob((byte*)key, len, out, &outSz)) != 0) {
            return ret;
        }

        XMEMCPY((byte*)aes->key, out, outSz);
        aes->keylen = outSz;
    }
    else {
        if (len != 16 && len != 24 && len != 32) {
            return BAD_FUNC_ARG;
        }

        XMEMCPY((byte*)aes->key, key, len);
        aes->keylen = len;
    }

    switch (aes->keylen) {
        case 16: aes->rounds = 10; break;
        case 24: aes->rounds = 12; break;
        case 32: aes->rounds = 14; break;
        default:
            return BAD_FUNC_ARG;
    }

    if ((ret = wc_AesSetIV(aes, iv)) != 0) {
        return ret;
    }

#ifdef WOLFSSL_AES_COUNTER
    aes->left = 0;
#endif

    return 0;
}


int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz)
{
    word32  blocks;

    WOLFSSL_ENTER("wc_AesCbcEncrypt");
    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if (blocks > 0) {
        Buffer buf[4];
        word32 arg[4];
        word32 keySz;
        int ret;

        if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
           return ret;
        }

        /* Set buffers for key, cipher text, and plain text */
        buf[0].BufferType = DataBuffer;
        buf[0].TheAddress = (Address)aes->key;
        buf[0].Length     = keySz;

        buf[1].BufferType = DataBuffer;
        buf[1].TheAddress = (Address)aes->reg;
        buf[1].Length     = AES_BLOCK_SIZE;

        buf[2].BufferType = DataBuffer;
        buf[2].TheAddress = (Address)in;
        buf[2].Length     = blocks * AES_BLOCK_SIZE;

        buf[3].BufferType = DataBuffer | LastBuffer;
        buf[3].TheAddress = (Address)out;
        buf[3].Length     = blocks * AES_BLOCK_SIZE;

        arg[0] = CAAM_ENC;
        arg[1] = keySz;
        arg[2] = blocks * AES_BLOCK_SIZE;

        if ((ret = wc_caamAddAndWait(buf, 4, arg, CAAM_AESCBC)) != 0) {
            WOLFSSL_MSG("Error with CAAM AES CBC encrypt");
            return ret;
        }
    }

    return 0;
}


int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz)
{
    word32  blocks;

    WOLFSSL_ENTER("wc_AesCbcDecrypt");
    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if (blocks > 0) {
        Buffer buf[4];
        word32 arg[4];
        word32 keySz;
        int ret;

        if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
            return ret;
        }

        /* Set buffers for key, cipher text, and plain text */
        buf[0].BufferType = DataBuffer;
        buf[0].TheAddress = (Address)aes->key;
        buf[0].Length     = keySz;

        buf[1].BufferType = DataBuffer;
        buf[1].TheAddress = (Address)aes->reg;
        buf[1].Length     = AES_BLOCK_SIZE;

        buf[2].BufferType = DataBuffer;
        buf[2].TheAddress = (Address)in;
        buf[2].Length     = blocks * AES_BLOCK_SIZE;

        buf[3].BufferType = DataBuffer | LastBuffer;
        buf[3].TheAddress = (Address)out;
        buf[3].Length     = blocks * AES_BLOCK_SIZE;

        arg[0] = CAAM_DEC;
        arg[1] = keySz;
        arg[2] = blocks * AES_BLOCK_SIZE;

        if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESCBC)) != 0) {
            WOLFSSL_MSG("Error with CAAM AES CBC decrypt");
            return ret;
        }
    }

    return 0;
}

#if defined(HAVE_AES_ECB)
/* is assumed that input size is a multiple of AES_BLOCK_SIZE */
int wc_AesEcbEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz)
{
    word32 blocks;
    Buffer buf[3];
    word32 arg[4];
    word32 keySz;
    int    ret;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
        return ret;
    }

    /* Set buffers for key, cipher text, and plain text */
    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (Address)aes->key;
    buf[0].Length     = keySz;

    buf[1].BufferType = DataBuffer;
    buf[1].TheAddress = (Address)in;
    buf[1].Length     = blocks * AES_BLOCK_SIZE;

    buf[2].BufferType = DataBuffer | LastBuffer;
    buf[2].TheAddress = (Address)out;
    buf[2].Length     = blocks * AES_BLOCK_SIZE;

    arg[0] = CAAM_ENC;
    arg[1] = keySz;
    arg[2] = blocks * AES_BLOCK_SIZE;

    if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESECB)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES ECB encrypt");
        return ret;
    }

    return 0;
}


int wc_AesEcbDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz)
{
    word32  blocks;
    Buffer buf[3];
    word32 arg[4];
    word32 keySz;
    int    ret;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    blocks = sz / AES_BLOCK_SIZE;

    if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
        return ret;
    }

    /* Set buffers for key, cipher text, and plain text */
    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (Address)aes->key;
    buf[0].Length     = keySz;

    buf[1].BufferType = DataBuffer;
    buf[1].TheAddress = (Address)in;
    buf[1].Length     = blocks * AES_BLOCK_SIZE;

    buf[2].BufferType = DataBuffer | LastBuffer;
    buf[2].TheAddress = (Address)out;
    buf[2].Length     = blocks * AES_BLOCK_SIZE;

    arg[0] = CAAM_DEC;
    arg[1] = keySz;
    arg[2] = blocks * AES_BLOCK_SIZE;

    if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESECB)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES ECB decrypt");
        return ret;
    }

    return 0;
}
#endif

/* AES-CTR */
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


int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz)
{
    byte* tmp;
    Buffer buf[4];
    word32 arg[4];
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
        /* Set buffers for key, cipher text, and plain text */
        buf[0].BufferType = DataBuffer;
        buf[0].TheAddress = (Address)aes->key;
        buf[0].Length     = keySz;

        buf[1].BufferType = DataBuffer;
        buf[1].TheAddress = (Address)aes->reg;
        buf[1].Length     = AES_BLOCK_SIZE;

        buf[2].BufferType = DataBuffer;
        buf[2].TheAddress = (Address)in;
        buf[2].Length     = blocks * AES_BLOCK_SIZE;

        buf[3].BufferType = DataBuffer | LastBuffer;
        buf[3].TheAddress = (Address)out;
        buf[3].Length     = blocks * AES_BLOCK_SIZE;

        arg[0] = CAAM_ENC;
        arg[1] = keySz;
        arg[2] = blocks * AES_BLOCK_SIZE;

        if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESCTR)) != 0) {
            WOLFSSL_MSG("Error with CAAM AES CTR encrypt");
            return ret;
        }

        out += blocks * AES_BLOCK_SIZE;
        in  += blocks * AES_BLOCK_SIZE;
        sz  -= blocks * AES_BLOCK_SIZE;
    }

    if (sz) {
        if ((ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg)) != 0)
            return ret;
        IncrementAesCounter((byte*)aes->reg);

        aes->left = AES_BLOCK_SIZE;
        tmp = (byte*)aes->tmp;

        while (sz--) {
            *(out++) = *(in++) ^ *(tmp++);
            aes->left--;
        }
    }

    return 0;
}
#endif


/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)
int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in)
{
     Buffer buf[3];
     word32 arg[4];
     word32 keySz;
     int ret;

     if (aes == NULL || out == NULL || in == NULL) {
         return BAD_FUNC_ARG;
     }

     if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
         return ret;
     }

     /* Set buffers for key, cipher text, and plain text */
     buf[0].BufferType = DataBuffer;
     buf[0].TheAddress = (Address)aes->key;
     buf[0].Length     = keySz;

     buf[1].BufferType = DataBuffer;
     buf[1].TheAddress = (Address)in;
     buf[1].Length     = AES_BLOCK_SIZE;

     buf[2].BufferType = DataBuffer | LastBuffer;
     buf[2].TheAddress = (Address)out;
     buf[2].Length     = AES_BLOCK_SIZE;

     arg[0] = CAAM_ENC;
     arg[1] = keySz;
     arg[2] = AES_BLOCK_SIZE;

     if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESECB)) != 0) {
         WOLFSSL_MSG("Error with CAAM AES direct encrypt");
         return ret;
     }

     return ret;
}


int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in)
{
     Buffer buf[3];
     word32 arg[4];
     word32 keySz;
     int ret;

     if (aes == NULL || out == NULL || in == NULL) {
         return BAD_FUNC_ARG;
     }

     if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
         return ret;
     }

     /* Set buffers for key, cipher text, and plain text */
     buf[0].BufferType = DataBuffer;
     buf[0].TheAddress = (Address)aes->key;
     buf[0].Length     = keySz;

     buf[1].BufferType = DataBuffer;
     buf[1].TheAddress = (Address)in;
     buf[1].Length     = AES_BLOCK_SIZE;

     buf[2].BufferType = DataBuffer | LastBuffer;
     buf[2].TheAddress = (Address)out;
     buf[2].Length     = AES_BLOCK_SIZE;

     arg[0] = CAAM_DEC;
     arg[1] = keySz;
     arg[2] = AES_BLOCK_SIZE;

     if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESECB)) != 0) {
         WOLFSSL_MSG("Error with CAAM AES direct decrypt");
         return ret;
     }

     return 0;
}


int  wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir)
{
     return wc_AesSetKey(aes, key, len, iv, dir);
}
#endif

#ifdef HAVE_AESCCM
int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
{
    Buffer buf[5];
    word32 arg[4];
    word32 keySz;
    word32 i;
    byte B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    int lenSz;
    byte mask = 0xFF;
    const word32 wordSz = (word32)sizeof(word32);
    int ret;

    /* sanity check on arguments */
    if (aes == NULL || out == NULL || in == NULL || nonce == NULL
            || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
        authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    if ((ret = wc_AesCcmCheckTagSize(authTagSz)) != 0) {
        return ret;
    }

    if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
         return ret;
    }

    /* set up B0 and CTR0 similar to how wolfcrypt/src/aes.c does */
    XMEMCPY(B0Ctr0+1, nonce, nonceSz);
    XMEMCPY(B0Ctr0+AES_BLOCK_SIZE+1, nonce, nonceSz);
    lenSz = AES_BLOCK_SIZE - 1 - (byte)nonceSz;
    B0Ctr0[0] = (authInSz > 0 ? 64 : 0)
         + (8 * (((byte)authTagSz - 2) / 2))
         + (lenSz - 1);
    for (i = 0; i < lenSz; i++) {
        if (mask && i >= wordSz)
            mask = 0x00;
        B0Ctr0[AES_BLOCK_SIZE - 1 - i] = (inSz >> ((8 * i) & mask)) & mask;
        B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE - 1 - i] = 0;
    }
    B0Ctr0[AES_BLOCK_SIZE] = lenSz - 1;

    /* Set buffers for key, cipher text, and plain text */
    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (Address)aes->key;
    buf[0].Length     = keySz;

    buf[1].BufferType = DataBuffer;
    buf[1].TheAddress = (Address)B0Ctr0;
    buf[1].Length     = AES_BLOCK_SIZE + AES_BLOCK_SIZE;

    buf[2].BufferType = DataBuffer;
    buf[2].TheAddress = (Address)authIn;
    buf[2].Length     = authInSz;

    buf[3].BufferType = DataBuffer;
    buf[3].TheAddress = (Address)in;
    buf[3].Length     = inSz;

    buf[4].BufferType = DataBuffer | LastBuffer;
    buf[4].TheAddress = (Address)out;
    buf[4].Length     = inSz;

    arg[0] = CAAM_ENC;
    arg[1] = keySz;
    arg[2] = inSz;
    arg[3] = authInSz;

    if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESCCM)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES-CCM encrypt");
        return ret;
    }

    XMEMCPY(authTag, B0Ctr0, authTagSz);
    return 0;
}


#ifdef HAVE_AES_DECRYPT
int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz)
{
    Buffer buf[5];
    word32 arg[4];
    word32 keySz;
    word32 i;
    byte B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    byte tag[AES_BLOCK_SIZE];
    int lenSz;
    byte mask = 0xFF;
    const word32 wordSz = (word32)sizeof(word32);
    int ret;

    /* sanity check on arguments */
    if (aes == NULL || out == NULL || in == NULL || nonce == NULL
            || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
        authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    if ((ret = wc_AesCcmCheckTagSize(authTagSz)) != 0) {
        return ret;
    }

    if ((ret = wc_AesGetKeySize(aes, &keySz)) != 0) {
         return ret;
    }

    /* set up B0 and CTR0 similar to how wolfcrypt/src/aes.c does */
    XMEMCPY(B0Ctr0+1, nonce, nonceSz);
    XMEMCPY(B0Ctr0+AES_BLOCK_SIZE+1, nonce, nonceSz);
    lenSz = AES_BLOCK_SIZE - 1 - (byte)nonceSz;
    B0Ctr0[0] = (authInSz > 0 ? 64 : 0)
         + (8 * (((byte)authTagSz - 2) / 2))
         + (lenSz - 1);
    for (i = 0; i < lenSz; i++) {
        if (mask && i >= wordSz)
            mask = 0x00;
        B0Ctr0[AES_BLOCK_SIZE - 1 - i] = (inSz >> ((8 * i) & mask)) & mask;
        B0Ctr0[AES_BLOCK_SIZE + AES_BLOCK_SIZE - 1 - i] = 0;
    }
    B0Ctr0[AES_BLOCK_SIZE] = lenSz - 1;
    if ((ret = wc_AesEncryptDirect(aes, tag, B0Ctr0 + AES_BLOCK_SIZE)) != 0)
        return ret;

    /* Set buffers for key, cipher text, and plain text */
    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (Address)aes->key;
    buf[0].Length     = keySz;

    buf[1].BufferType = DataBuffer;
    buf[1].TheAddress = (Address)B0Ctr0;
    buf[1].Length     = AES_BLOCK_SIZE + AES_BLOCK_SIZE;

    buf[2].BufferType = DataBuffer;
    buf[2].TheAddress = (Address)authIn;
    buf[2].Length     = authInSz;

    buf[3].BufferType = DataBuffer;
    buf[3].TheAddress = (Address)in;
    buf[3].Length     = inSz;

    buf[4].BufferType = DataBuffer | LastBuffer;
    buf[4].TheAddress = (Address)out;
    buf[4].Length     = inSz;

    arg[0] = CAAM_DEC;
    arg[1] = keySz;
    arg[2] = inSz;
    arg[3] = authInSz;

    if ((ret = wc_caamAddAndWait(buf, arg, CAAM_AESCCM)) != 0) {
        WOLFSSL_MSG("Error with CAAM AES-CCM derypt");
        return ret;
    }

    xorbuf(tag, B0Ctr0, authTagSz);
    if (ConstantCompare(tag, authTag, authTagSz) != 0) {
        /* If the authTag check fails, don't keep the decrypted data.
         * Unfortunately, you need the decrypted data to calculate the
         * check value. */
        XMEMSET(out, 0, inSz);
        ret = AES_CCM_AUTH_E;
    }

    ForceZero(tag, AES_BLOCK_SIZE);
    ForceZero(B0Ctr0, AES_BLOCK_SIZE * 2);

    return ret;

}
#endif /* HAVE_AES_DECRYPT */
#endif /* HAVE_AESCCM */

#endif /* WOLFSSL_IMX6_CAAM && !NO_AES */

