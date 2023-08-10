/* cmac.c
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
#ifdef WOLFSSL_QNX_CAAM
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif
#if defined(WOLFSSL_HASH_KEEP)
#include <wolfssl/wolfcrypt/hash.h>
#endif

#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_DIRECT)

#if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$n")
        #pragma const_seg(".fipsB$n")
    #endif
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/cmac.h>

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef WOLFSSL_HASH_KEEP
/* Some hardware have issues with update, this function stores the data to be
 * hashed into an array. Once ready, the Final operation is called on all of the
 * data to be hashed at once.
 * returns 0 on success
 */
int wc_CMAC_Grow(Cmac* cmac, const byte* in, int inSz)
{
    return _wc_Hash_Grow(&cmac->msg, &cmac->used, &cmac->len, in, inSz, NULL);
}
#endif /* WOLFSSL_HASH_KEEP */


/* Used by AES-SIV. See aes.c. */
void ShiftAndXorRb(byte* out, byte* in)
{
    int i, j, xorRb;
    int mask = 0, last = 0;
    byte Rb = 0x87;

    xorRb = (in[0] & 0x80) != 0;

    for (i = 1, j = AES_BLOCK_SIZE - 1; i <= AES_BLOCK_SIZE; i++, j--) {
        last = (in[j] & 0x80) ? 1 : 0;
        out[j] = (byte)((in[j] << 1) | mask);
        mask = last;
        if (xorRb) {
            out[j] ^= Rb;
            Rb = 0;
        }
    }
}

/* returns 0 on success */
int wc_InitCmac_ex(Cmac* cmac, const byte* key, word32 keySz,
                int type, void* unused, void* heap, int devId)
{
    int ret;
#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    byte useSW = 0;
#endif

    (void)unused;
    (void)heap;

    if (cmac == NULL || type != WC_CMAC_AES) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    /* save if we should use SW crypt, restore after memset */
    useSW = cmac->useSWCrypt;
#endif
    XMEMSET(cmac, 0, sizeof(Cmac));

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (devId != INVALID_DEVID)
    #endif
    {
        cmac->devId = devId;
        cmac->devCtx = NULL;

        ret = wc_CryptoCb_Cmac(cmac, key, keySz, NULL, 0, NULL, NULL,
                type, unused);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#else
    (void)devId;
#endif

    if (key == NULL || keySz == 0) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    cmac->useSWCrypt = useSW;
    if (cmac->useSWCrypt == 1) {
        cmac->aes.useSWCrypt = 1;
    }
#endif

    ret = wc_AesSetKey(&cmac->aes, key, keySz, NULL, AES_ENCRYPTION);
    if (ret == 0) {
        byte l[AES_BLOCK_SIZE];

        XMEMSET(l, 0, AES_BLOCK_SIZE);
        ret = wc_AesEncryptDirect(&cmac->aes, l, l);
        if (ret == 0) {
            ShiftAndXorRb(cmac->k1, l);
            ShiftAndXorRb(cmac->k2, cmac->k1);
            ForceZero(l, AES_BLOCK_SIZE);
        }
    }
    return ret;
}


int wc_InitCmac(Cmac* cmac, const byte* key, word32 keySz,
                int type, void* unused)
{
#ifdef WOLFSSL_QNX_CAAM
    int devId = WOLFSSL_CAAM_DEVID;
#else
    int devId = INVALID_DEVID;
#endif
    return wc_InitCmac_ex(cmac, key, keySz, type, unused, NULL, devId);
}



int wc_CmacUpdate(Cmac* cmac, const byte* in, word32 inSz)
{
    int ret = 0;

    if ((cmac == NULL) || (in == NULL && inSz != 0)) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (cmac->devId != INVALID_DEVID)
    #endif
    {
        ret = wc_CryptoCb_Cmac(cmac, NULL, 0, in, inSz,
                NULL, NULL, 0, NULL);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
        ret = 0; /* reset error code */
    }
#endif

    while (inSz != 0) {
        word32 add = min(inSz, AES_BLOCK_SIZE - cmac->bufferSz);
        XMEMCPY(&cmac->buffer[cmac->bufferSz], in, add);

        cmac->bufferSz += add;
        in += add;
        inSz -= add;

        if (cmac->bufferSz == AES_BLOCK_SIZE && inSz != 0) {
            if (cmac->totalSz != 0) {
                xorbuf(cmac->buffer, cmac->digest, AES_BLOCK_SIZE);
            }
            ret = wc_AesEncryptDirect(&cmac->aes, cmac->digest, cmac->buffer);
            if (ret == 0) {
                cmac->totalSz += AES_BLOCK_SIZE;
                cmac->bufferSz = 0;
            }
        }
    }

    return ret;
}


int wc_CmacFinal(Cmac* cmac, byte* out, word32* outSz)
{
    int ret;
    const byte* subKey;
    word32 remainder;

    if (cmac == NULL || out == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }
    if (*outSz < WC_CMAC_TAG_MIN_SZ || *outSz > WC_CMAC_TAG_MAX_SZ) {
        return BUFFER_E;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (cmac->devId != INVALID_DEVID)
    #endif
    {
        ret = wc_CryptoCb_Cmac(cmac, NULL, 0, NULL, 0, out, outSz, 0, NULL);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif

    if (cmac->bufferSz == AES_BLOCK_SIZE) {
        subKey = cmac->k1;
    }
    else {
        /* ensure we will have a valid remainder value */
        if (cmac->bufferSz > AES_BLOCK_SIZE) {
            return BAD_STATE_E;
        }
        remainder = AES_BLOCK_SIZE - cmac->bufferSz;

        if (remainder == 0) {
            remainder = AES_BLOCK_SIZE;
        }
        if (remainder > 1) {
            XMEMSET(cmac->buffer + AES_BLOCK_SIZE - remainder, 0, remainder);
        }

        cmac->buffer[AES_BLOCK_SIZE - remainder] = 0x80;
        subKey = cmac->k2;
    }
    xorbuf(cmac->buffer, cmac->digest, AES_BLOCK_SIZE);
    xorbuf(cmac->buffer, subKey, AES_BLOCK_SIZE);
    ret = wc_AesEncryptDirect(&cmac->aes, cmac->digest, cmac->buffer);
    if (ret == 0) {
        XMEMCPY(out, cmac->digest, *outSz);
    }

#if defined(WOLFSSL_HASH_KEEP)
    /* TODO: msg is leaked if wc_CmacFinal() is not called
     * e.g. when multiple calls to wc_CmacUpdate() and one fails but
     * wc_CmacFinal() not called. */
    if (cmac->msg != NULL) {
        XFREE(cmac->msg, cmac->heap, DYNAMIC_TYPE_TMP_BUFFER);
        cmac->msg = NULL;
    }
#endif
    wc_AesFree(&cmac->aes);
    ForceZero(cmac, sizeof(Cmac));

    return ret;
}


int wc_AesCmacGenerate(byte* out, word32* outSz,
                       const byte* in, word32 inSz,
                       const byte* key, word32 keySz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    Cmac *cmac;
#else
    Cmac cmac[1];
#endif

    if (out == NULL || (in == NULL && inSz > 0) || key == NULL || keySz == 0) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    if ((cmac = (Cmac *)XMALLOC(sizeof *cmac, NULL,
                                DYNAMIC_TYPE_CMAC)) == NULL) {
        return MEMORY_E;
    }
#endif
#ifdef WOLFSSL_CHECK_MEM_ZERO
    XMEMSET(((unsigned char *)cmac) + sizeof(Aes), 0xff,
        sizeof(Cmac) - sizeof(Aes));
    /* Aes part is checked by wc_AesFree. */
    wc_MemZero_Add("wc_AesCmacGenerate cmac",
        ((unsigned char *)cmac) + sizeof(Aes), sizeof(Cmac) - sizeof(Aes));
#endif

    ret = wc_InitCmac(cmac, key, keySz, WC_CMAC_AES, NULL);
    if (ret == 0) {
        ret = wc_CmacUpdate(cmac, in, inSz);
    }
    if (ret == 0) {
        ret = wc_CmacFinal(cmac, out, outSz);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (cmac) {
        XFREE(cmac, NULL, DYNAMIC_TYPE_CMAC);
    }
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(cmac, sizeof(Cmac));
#endif

    return ret;
}


int wc_AesCmacVerify(const byte* check, word32 checkSz,
                     const byte* in, word32 inSz,
                     const byte* key, word32 keySz)
{
    int ret;
    byte a[AES_BLOCK_SIZE];
    word32 aSz = sizeof(a);
    int compareRet;

    if (check == NULL || checkSz == 0 || (in == NULL && inSz != 0) ||
        key == NULL || keySz == 0) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(a, 0, aSz);
    ret = wc_AesCmacGenerate(a, &aSz, in, inSz, key, keySz);
    compareRet = ConstantCompare(check, a, (int)min(checkSz, aSz));

    if (ret == 0)
        ret = compareRet ? 1 : 0;

    return ret;
}

#endif /* WOLFSSL_CMAC && NO_AES && WOLFSSL_AES_DIRECT */
