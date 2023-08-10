/* rsa.c
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


#ifdef HAVE_CONFIG_H /* configure options when using autoconf */
    #include <config.h>
#endif

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_RSA

#define USER_CRYPTO_ERROR -101

#ifdef OPENSSL_EXTRA
    #include <wolfssl/openssl/rsa.h> /* include for openssl compatibility */
    #include <wolfssl/openssl/bn.h>
#endif
#include "user_rsa.h"

#ifdef DEBUG_WOLFSSL /* debug done without variadic to allow older compilers */
    #include <stdio.h>
    #define USER_DEBUG(x) printf x
#else
    #define USER_DEBUG(x) WC_DO_NOTHING
#endif

#define ASN_INTEGER    0x02
#define ASN_BIT_STRING 0x03
#define ASN_TAG_NULL   0x05
#define ASN_OBJECT_ID  0x06


/* Make sure compiler doesn't skip -- used from wolfSSL */
static inline void ForceZero(const void* mem, word32 len)
{
    volatile byte* z = (volatile byte*)mem;

    while (len--) *z++ = 0;
}

enum {
    RSA_PUBLIC_ENCRYPT  = 0,
    RSA_PUBLIC_DECRYPT  = 1,
    RSA_PRIVATE_ENCRYPT = 2,
    RSA_PRIVATE_DECRYPT = 3,

    RSA_BLOCK_TYPE_1 = 1,
    RSA_BLOCK_TYPE_2 = 2,

    RSA_MIN_SIZE = 512,
    RSA_MAX_SIZE = 4096, /* max allowed in IPP library */

    RSA_MIN_PAD_SZ   = 11      /* separator + 0 + pad value + 8 pads */
};


int wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId)
{

    USER_DEBUG(("Entering wc_InitRsaKey\n"));

    if (key == NULL)
        return USER_CRYPTO_ERROR;

    /* set full struct as 0 */
    ForceZero(key, sizeof(RsaKey));

    USER_DEBUG(("\tExit wc_InitRsaKey\n"));

    (void)devId;
    (void)heap;
    return 0;
}

int wc_InitRsaKey(RsaKey* key, void* heap)
{
    return wc_InitRsaKey_ex(key, heap, INVALID_DEVID);
}


/* three functions needed for cert and key gen */
#if defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA)
/* return 1 if there is a leading bit*/
int wc_Rsa_leading_bit(void* bn)
{
    int ret = 0;
    int dataSz;
    Ipp32u* data;
    Ipp32u  q;
    int qSz = sizeof(Ipp32u);

    if (ippsExtGet_BN(NULL, &dataSz, NULL, bn) != ippStsNoErr) {
        USER_DEBUG(("ippsExtGet_BN Rsa leading bit error\n"));
        return USER_CRYPTO_ERROR;
    }

    /* convert from size in binary to Ipp32u */
    dataSz = dataSz / 32 + ((dataSz % 32)? 1 : 0);
    data = (Ipp32u*)XMALLOC(dataSz * sizeof(Ipp32u), NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (data == NULL) {
        USER_DEBUG(("Rsa leading bit memory error\n"));
        return 0;
    }

    /* extract value from BN */
    if (ippsExtGet_BN(NULL, NULL, data, bn) != ippStsNoErr) {
        USER_DEBUG(("Rsa leading bit error\n"));
        XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return 0;
    }

    /* use method like what's used in wolfssl tfm.c  */
    q = data[dataSz - 1];

    ret = 0;
    while (qSz > 0) {
        if (q != 0)
            ret = (q & 0x80) != 0;
        q >>= 8;
        qSz--;
    }

    XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return ret;
}


/* get the size in bytes of BN */
int wc_Rsa_unsigned_bin_size(void* bn)
{
    int ret = 0;
    if (ippsExtGet_BN(NULL, &ret, NULL, bn) != ippStsNoErr) {
        USER_DEBUG(("Rsa unsigned bin size error\n"));
        return USER_CRYPTO_ERROR;
    }
    return (ret / 8) + ((ret % 8)? 1: 0); /* size in bytes */
}

#ifndef MP_OKAY
#define MP_OKAY 0
#endif

/* extract the bn value to a unsigned byte array and return MP_OKAY on success */
int wc_Rsa_to_unsigned_bin(void* bn, byte* in, int inLen)
{
    if (ippsGetOctString_BN((Ipp8u*)in, inLen, bn) != ippStsNoErr) {
        USER_DEBUG(("Rsa to unsigned bin error\n"));
        return USER_CRYPTO_ERROR;
    }
    return MP_OKAY;
}
#endif /* WOLFSSL_CERT_GEN || WOLFSSL_KEY_GEN || OPENSSL_EXTRA */


#ifdef OPENSSL_EXTRA /* functions needed for openssl compatibility layer */
static int SetIndividualExternal(WOLFSSL_BIGNUM** bn, IppsBigNumState* in)
{
    IppStatus ret;
    byte* data;
    int sz;

    USER_DEBUG(("Entering SetIndividualExternal\n"));

    if (bn == NULL || in == NULL) {
        USER_DEBUG(("inputs NULL error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (*bn == NULL) {
        *bn = wolfSSL_BN_new();
        if (*bn == NULL) {
            USER_DEBUG(("SetIndividualExternal alloc failed\n"));
            return USER_CRYPTO_ERROR;
        }
    }

    /* get size of array needed and extract oct array of data */
    ret = ippsGetSize_BN(in, &sz);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    data = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (data == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsGetOctString_BN(data, sz, in);
    if (ret != ippStsNoErr) {
        XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return USER_CRYPTO_ERROR;
    }

    /* store the data into a wolfSSL Big Number */
    *bn = wolfSSL_BN_bin2bn(data, sz, *bn);

    XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return 0;
}


static int SetIndividualInternal(WOLFSSL_BIGNUM* bn, IppsBigNumState** mpi)
{
    int length, ctxSz, sz;
    IppStatus ret;
    Ipp8u* data;

    USER_DEBUG(("Entering SetIndividualInternal\n"));

    if (bn == NULL || bn->internal == NULL) {
        USER_DEBUG(("bn NULL error\n"));
        return USER_CRYPTO_ERROR;
    }

    length = wolfSSL_BN_num_bytes(bn);

    /* if not IPP BN then create one */
    if (*mpi == NULL) {
        ret = ippsBigNumGetSize(length, &ctxSz);
        if (ret != ippStsNoErr)
            return USER_CRYPTO_ERROR;

        *mpi = (IppsBigNumState*)XMALLOC(ctxSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
        if (*mpi == NULL)
            return USER_CRYPTO_ERROR;

        ret = ippsBigNumInit(length, *mpi);
        if (ret != ippStsNoErr) {
            XFREE(*mpi, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            return USER_CRYPTO_ERROR;
        }

    }

    /* get the size of array needed and check IPP BigNum */
    if (ippsGetSize_BN(*mpi, &sz) != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    if (sz < length) {
        USER_DEBUG(("big num size is too small\n"));
        return USER_CRYPTO_ERROR;
    }

    data = (Ipp8u*)XMALLOC(length, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (data == NULL)
        return USER_CRYPTO_ERROR;

    /* extract the wolfSSL BigNum and store it into IPP BigNum */
    if (wolfSSL_BN_bn2bin(bn, data) < 0) {
        XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        USER_DEBUG(("error in getting bin from wolfssl bn\n"));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsSetOctString_BN(data, length, *mpi);
    if (ret != ippStsNoErr) {
        XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return USER_CRYPTO_ERROR;
    }

    XFREE(data, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return 0;
}


/* WolfSSL -> OpenSSL */
int SetRsaExternal(WOLFSSL_RSA* rsa)
{
    RsaKey* key;
    USER_DEBUG(("Entering SetRsaExternal\n"));

    if (rsa == NULL || rsa->internal == NULL) {
        USER_DEBUG(("rsa key NULL error\n"));
        return USER_CRYPTO_ERROR;
    }

    key = (RsaKey*)rsa->internal;

    if (SetIndividualExternal(&rsa->n, key->n) != 0) {
        USER_DEBUG(("rsa n key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (SetIndividualExternal(&rsa->e, key->e) != 0) {
        USER_DEBUG(("rsa e key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (key->type == RSA_PRIVATE) {
        if (SetIndividualExternal(&rsa->d, key->dipp) != 0) {
            USER_DEBUG(("rsa d key error\n"));
            return USER_CRYPTO_ERROR;
        }

        if (SetIndividualExternal(&rsa->p, key->pipp) != 0) {
            USER_DEBUG(("rsa p key error\n"));
            return USER_CRYPTO_ERROR;
        }

        if (SetIndividualExternal(&rsa->q, key->qipp) != 0) {
            USER_DEBUG(("rsa q key error\n"));
            return USER_CRYPTO_ERROR;
        }

        if (SetIndividualExternal(&rsa->dmp1, key->dPipp) != 0) {
            USER_DEBUG(("rsa dP key error\n"));
            return USER_CRYPTO_ERROR;
        }

        if (SetIndividualExternal(&rsa->dmq1, key->dQipp) != 0) {
            USER_DEBUG(("rsa dQ key error\n"));
            return USER_CRYPTO_ERROR;
        }

        if (SetIndividualExternal(&rsa->iqmp, key->uipp) != 0) {
            USER_DEBUG(("rsa u key error\n"));
            return USER_CRYPTO_ERROR;
        }
    }

    rsa->exSet = 1;

    /* SSL_SUCCESS */
    return 1;
}


/* Openssl -> WolfSSL */
int SetRsaInternal(WOLFSSL_RSA* rsa)
{
    int ctxSz, pSz, qSz;
    IppStatus ret;
    RsaKey* key;
    USER_DEBUG(("Entering SetRsaInternal\n"));

    if (rsa == NULL || rsa->internal == NULL) {
        USER_DEBUG(("rsa key NULL error\n"));
        return USER_CRYPTO_ERROR;
    }

    key = (RsaKey*)rsa->internal;

    if (SetIndividualInternal(rsa->n, &key->n) != 0) {
        USER_DEBUG(("rsa n key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (SetIndividualInternal(rsa->e, &key->e) != 0) {
        USER_DEBUG(("rsa e key error\n"));
        return USER_CRYPTO_ERROR;
    }

    /* public key */
    key->type = RSA_PUBLIC;

    if (rsa->d != NULL) {
        if (SetIndividualInternal(rsa->d, &key->dipp) != 0) {
            USER_DEBUG(("rsa d key error\n"));
            return USER_CRYPTO_ERROR;
        }

        /* private key */
        key->type = RSA_PRIVATE;
    }

    if (rsa->p != NULL &&
        SetIndividualInternal(rsa->p, &key->pipp) != 0) {
        USER_DEBUG(("rsa p key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (rsa->q != NULL &&
        SetIndividualInternal(rsa->q, &key->qipp) != 0) {
        USER_DEBUG(("rsa q key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (rsa->dmp1 != NULL &&
        SetIndividualInternal(rsa->dmp1, &key->dPipp) != 0) {
        USER_DEBUG(("rsa dP key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (rsa->dmq1 != NULL &&
        SetIndividualInternal(rsa->dmq1, &key->dQipp) != 0) {
        USER_DEBUG(("rsa dQ key error\n"));
        return USER_CRYPTO_ERROR;
    }

    if (rsa->iqmp != NULL &&
        SetIndividualInternal(rsa->iqmp, &key->uipp) != 0) {
        USER_DEBUG(("rsa u key error\n"));
        return USER_CRYPTO_ERROR;
    }

    rsa->inSet = 1;

    /* get sizes of IPP BN key states created from input */
    ret = ippsGetSize_BN(key->n, &key->nSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsGetSize_BN(key->e, &key->eSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->sz = key->nSz; /* set modulus size */

    /* convert to size in bits */
    key->nSz = key->nSz * 8;
    key->eSz = key->eSz * 8;

    /* set up public key state */
    ret = ippsRSA_GetSizePublicKey(key->nSz, key->eSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePublicKey error %s\n",
                ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->pPub = (IppsRSAPublicKeyState*)XMALLOC(ctxSz, NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPub == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSA_InitPublicKey(key->nSz, key->eSz, key->pPub, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPublicKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsRSA_SetPublicKey(key->n, key->e, key->pPub);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_SetPublicKey error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    if (key->pipp != NULL && key->qipp != NULL && key->dipp != NULL &&
            key->dPipp != NULL && key->dQipp != NULL && key->uipp != NULL) {
        /* get bn sizes needed for private key set up */
        ret = ippsGetSize_BN(key->pipp, &pSz);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }

        ret = ippsGetSize_BN(key->qipp, &qSz);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }

        /* store sizes needed for creating tmp private keys */
        ret = ippsGetSize_BN(key->dipp, &key->dSz);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }

        /* convert to size in bits */
        key->dSz = key->dSz * 8;
        pSz = pSz * 8;
        qSz = qSz * 8;

        /* set up private key state */
        ret = ippsRSA_GetSizePrivateKeyType2(pSz, qSz, &ctxSz);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsRSA_GetSizePrivateKey error %s\n",
                    ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }

        key->prvSz = ctxSz;
        key->pPrv = (IppsRSAPrivateKeyState*)XMALLOC(ctxSz, 0,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
        if (key->pPrv == NULL)
            return USER_CRYPTO_ERROR;

        ret = ippsRSA_InitPrivateKeyType2(pSz, qSz, key->pPrv, ctxSz);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsRSA_InitPrivateKey error %s\n",
                        ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }

        ret = ippsRSA_SetPrivateKeyType2(key->pipp, key->qipp, key->dPipp,
                key->dQipp, key->uipp, key->pPrv);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsRSA_SetPrivateKey error %s\n", ippGetStatusString(ret)));
            return USER_CRYPTO_ERROR;
        }
    }

    /* SSL_SUCCESS */
    return 1;
}
#endif /* OPENSSLEXTRA */


/* Padding scheme function used in wolfSSL for signing needed for matching
   existing API signing scheme
    input : the msg to be signed
    inputLen : length of input msg
    pkcsBlock : the outputted padded msg
    pkcsBlockLen : length of outputted padded msg buffer
    padValue : the padded value after first 00 , is either 01 or 02
    rng : random number generator structure
 */
static int wc_RsaPad(const byte* input, word32 inputLen, byte* pkcsBlock,
                   word32 pkcsBlockLen, byte padValue, WC_RNG* rng)
{
    if (inputLen == 0 || pkcsBlockLen == 0) {
        return USER_CRYPTO_ERROR;
    }

    pkcsBlock[0] = 0x0;       /* set first byte to zero and advance */
    pkcsBlock++; pkcsBlockLen--;
    pkcsBlock[0] = padValue;  /* insert padValue */

    if (padValue == RSA_BLOCK_TYPE_1) {
        if (pkcsBlockLen < inputLen + 2) {
            return USER_CRYPTO_ERROR;
        }

        /* pad with 0xff bytes */
        XMEMSET(&pkcsBlock[1], 0xFF, pkcsBlockLen - inputLen - 2);
    }
    else {
        /* pad with non-zero random bytes */
        word32 padLen, i;
        int    ret;

        if (pkcsBlockLen < inputLen + 1) {
            return USER_CRYPTO_ERROR;
        }

        padLen = pkcsBlockLen - inputLen - 1;
        ret = wc_RNG_GenerateBlock(rng, &pkcsBlock[1], padLen);

        if (ret != 0)
            return ret;

        /* remove zeros */
        for (i = 1; i < padLen; i++)
            if (pkcsBlock[i] == 0) pkcsBlock[i] = 0x01;
    }

    pkcsBlock[pkcsBlockLen-inputLen-1] = 0;     /* separator */
    XMEMCPY(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);

    return 0;
}


/* UnPad plaintext, set start to *output, return length of plaintext,
 * < 0 on error */
static int RsaUnPad(const byte *pkcsBlock, unsigned int pkcsBlockLen,
                       byte **output, byte padValue)
{
    word32 maxOutputLen = (pkcsBlockLen > 10) ? (pkcsBlockLen - 10) : 0,
           invalid = 0,
           i = 1,
           outputLen;

    if (pkcsBlockLen == 0) {
        return USER_CRYPTO_ERROR;
    }

    if (pkcsBlock[0] != 0x0) /* skip past zero */
        invalid = 1;
    pkcsBlock++; pkcsBlockLen--;

    /* Require block type padValue */
    invalid = (pkcsBlock[0] != padValue) || invalid;

    /* verify the padding until we find the separator */
    if (padValue == RSA_BLOCK_TYPE_1) {
        while (i<pkcsBlockLen && pkcsBlock[i++] == 0xFF) {/* Null body */}
    }
    else {
        while (i<pkcsBlockLen && pkcsBlock[i++]) {/* Null body */}
    }

    if(!(i==pkcsBlockLen || pkcsBlock[i-1]==0)) {
        USER_DEBUG(("RsaUnPad error, bad formatting\n"));
        return USER_CRYPTO_ERROR;
    }

    outputLen = pkcsBlockLen - i;
    invalid = (outputLen > maxOutputLen) || invalid;

    if (invalid) {
        USER_DEBUG(("RsaUnPad error, bad formatting\n"));
        return USER_CRYPTO_ERROR;
    }

    *output = (byte *)(pkcsBlock + i);
    return outputLen;
}


/* Set up memory and structure for a Big Number
 * returns ippStsNoErr on success
 */
static IppStatus init_bn(IppsBigNumState** in, int sz)
{
    int ctxSz;
    IppStatus ret;

    ret = ippsBigNumGetSize(sz, &ctxSz);
    if (ret != ippStsNoErr) {
        return ret;
    }

    *in = (IppsBigNumState*)XMALLOC(ctxSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (*in == NULL) {
        return ippStsNoMemErr;
    }

    ret = ippsBigNumInit(sz, *in);
    if (ret != ippStsNoErr) {
        XFREE(*in, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        *in = NULL;
        return ret;
    }

    return ippStsNoErr;
}


/* Set up memory and structure for a Montgomery struct
 * returns ippStsNoErr on success
 */
static IppStatus init_mont(IppsMontState** mont, int* ctxSz,
                                                         IppsBigNumState* modul)
{
    int       mSz;
    Ipp32u*   m;
    IppStatus ret;

    ret = ippsExtGet_BN(NULL, ctxSz, NULL, modul);
    if (ret != ippStsNoErr) {
        return ret;
    }

    /* convert bits to Ipp32u array size and round up
       32 is number of bits in type */
    mSz = (*ctxSz/32)+((*ctxSz % 32)? 1: 0);
    m = (Ipp32u*)XMALLOC(mSz * sizeof(Ipp32u), 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (m == NULL) {
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return ippStsNoMemErr;
    }

    ret = ippsExtGet_BN(NULL, NULL, m, modul);
    if (ret != ippStsNoErr) {
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return ret;
    }

    ret = ippsMontGetSize(IppsSlidingWindows, mSz, ctxSz);
    if (ret != ippStsNoErr) {
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return ret;
    }

    /* 2. Allocate working buffer using malloc */
    *mont = (IppsMontState*)XMALLOC(*ctxSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (*mont == NULL) {
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return ippStsNoMemErr;
    }
    ret = ippsMontInit(IppsSlidingWindows, mSz, *mont);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMontInit error of %s\n", ippGetStatusString(ret)));
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(*mont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        *mont = NULL;
        return ret;
    }

    /* 3. Call the function MontSet to set big number module */
    ret = ippsMontSet(m, mSz, *mont);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMontSet error of %s\n", ippGetStatusString(ret)));
        XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(*mont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        *mont = NULL;
        return ret;
    }

    XFREE(m, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return ippStsNoErr;
}



int wc_FreeRsaKey(RsaKey* key)
{
    if (key == NULL)
        return 0;

    USER_DEBUG(("Entering wc_FreeRsaKey\n"));

    if (key->pPub != NULL) {
        XFREE(key->pPub, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->pPub = NULL;
    }

    if (key->pPrv != NULL) {
        /* write over sensitive information */
        ForceZero(key->pPrv, key->prvSz);
        XFREE(key->pPrv, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->pPrv = NULL;
    }

    if (key->n != NULL) {
        XFREE(key->n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->n = NULL;
    }

    if (key->e != NULL) {
        XFREE(key->e, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->e = NULL;
    }

    if (key->dipp != NULL) {
        XFREE(key->dipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->dipp = NULL;
    }

    if (key->pipp != NULL) {
        XFREE(key->pipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->pipp = NULL;
    }

    if (key->qipp != NULL) {
        XFREE(key->qipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->qipp = NULL;
    }

    if (key->dPipp != NULL) {
        XFREE(key->dPipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->dPipp = NULL;
    }

    if (key->dQipp != NULL) {
        XFREE(key->dQipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->dQipp = NULL;
    }

    if (key->uipp != NULL) {
        XFREE(key->uipp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        key->uipp = NULL;
    }

    USER_DEBUG(("\tExit wc_FreeRsaKey\n"));
    (void)key;

    return 0;
}


/* Some parsing functions from wolfSSL code needed to match wolfSSL API used */
static int GetLength(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx)
{
    int     length = 0;
    word32  idx = *inOutIdx;
    byte    b;

    *len = 0;    /* default length */

    if ((idx + 1) > maxIdx) {   /* for first read */
        USER_DEBUG(("GetLength bad index on input\n"));
        return USER_CRYPTO_ERROR;
    }

    b = input[idx++];
    if (b >= 0x80) {
        word32 bytes = b & 0x7F;

        if ((idx + bytes) > maxIdx) {   /* for reading bytes */
            USER_DEBUG(("GetLength bad long length\n"));
            return USER_CRYPTO_ERROR;
        }

        while (bytes--) {
            b = input[idx++];
            length = (length << 8) | b;
        }
    }
    else
        length = b;

    if ((idx + length) > maxIdx) {   /* for user of length */
        USER_DEBUG(("GetLength value exceeds buffer length\n"));
        return USER_CRYPTO_ERROR;
    }

    *inOutIdx = idx;
    if (length > 0)
        *len = length;

    return length;
}

static int GetASNHeader(const byte* input, byte tag, word32* inOutIdx, int* len,
                        word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;
    int    length;

    if ((idx + 1) > maxIdx)
        return USER_CRYPTO_ERROR;

    b = input[idx++];
    if (b != tag)
        return USER_CRYPTO_ERROR;

    if (GetLength(input, &idx, &length, maxIdx) < 0)
        return USER_CRYPTO_ERROR;

    *len      = length;
    *inOutIdx = idx;
    return length;
}

static int GetASNInt(const byte* input, word32* inOutIdx, int* len,
                     word32 maxIdx)
{
    int    ret;

    ret = GetASNHeader(input, ASN_INTEGER, inOutIdx, len, maxIdx);
    if (ret < 0)
        return ret;

    if (*len > 0) {
        /* remove leading zero, unless there is only one 0x00 byte */
        if ((input[*inOutIdx] == 0x00) && (*len > 1)) {
            (*inOutIdx)++;
            (*len)--;

            if (*len > 0 && (input[*inOutIdx] & 0x80) == 0)
                return USER_CRYPTO_ERROR;
        }
    }

    return 0;
}

static int GetInt(IppsBigNumState** mpi, const byte* input, word32* inOutIdx,
                  word32 maxIdx)
{
    IppStatus ret;
    word32 idx = *inOutIdx;
    int    length;
    int    ctxSz;

    if (GetASNInt(input, &idx, &length, maxIdx) < 0) {
        return USER_CRYPTO_ERROR;
    }

    ret = ippsBigNumGetSize(length, &ctxSz);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    *mpi = (IppsBigNumState*)XMALLOC(ctxSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (*mpi == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsBigNumInit(length, *mpi);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    ret = ippsSetOctString_BN((Ipp8u*)input + idx, length, *mpi);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    *inOutIdx = idx + length;
    return 0;
}


static int GetSequence(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx)
{
    int    length = -1;
    word32 idx    = *inOutIdx;

    if ((idx + 1) > maxIdx)
        return USER_CRYPTO_ERROR;

    if (input[idx++] != (0x10 | 0x20) ||
            GetLength(input, &idx, &length, maxIdx) < 0)
        return USER_CRYPTO_ERROR;

    *len      = length;
    *inOutIdx = idx;

    return length;
}


static int GetMyVersion(const byte* input, word32* inOutIdx,
                               int* version, word32 maxIdx)
{
    word32 idx = *inOutIdx;

    if ((idx + 3) > maxIdx)
        return USER_CRYPTO_ERROR;

    if (input[idx++] != 0x02)
        return USER_CRYPTO_ERROR;

    if (input[idx++] != 0x01)
        return USER_CRYPTO_ERROR;

    *version  = input[idx++];
    *inOutIdx = idx;

    return *version;
}


int wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx, RsaKey* key,
                        word32 inSz)
{
    int    version, length;
    int  ctxSz, pSz, qSz;
    IppStatus ret;

    if (input == NULL || inOutIdx == NULL || key == NULL) {
        return USER_CRYPTO_ERROR;
    }

    USER_DEBUG(("Entering wc_RsaPrivateKeyDecode\n"));

    /* read in key information */
    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return USER_CRYPTO_ERROR;

    if (GetMyVersion(input, inOutIdx, &version, inSz) < 0)
        return USER_CRYPTO_ERROR;

    key->type = RSA_PRIVATE;

    if (GetInt(&key->n,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->e,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->dipp,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->pipp,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->qipp,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->dPipp, input, inOutIdx, inSz) < 0 ||
        GetInt(&key->dQipp, input, inOutIdx, inSz) < 0 ||
        GetInt(&key->uipp,  input, inOutIdx, inSz) < 0 )
        return USER_CRYPTO_ERROR;

    /* get sizes of IPP BN key states created from input */
    ret = ippsGetSize_BN(key->n, &key->nSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsGetSize_BN(key->e, &key->eSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->sz = key->nSz; /* set modulus size */

    /* convert to size in bits */
    key->nSz = key->nSz * 8;
    key->eSz = key->eSz * 8;

    /* set up public key state */
    ret = ippsRSA_GetSizePublicKey(key->nSz, key->eSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePublicKey error %s\n",
                ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->pPub = (IppsRSAPublicKeyState*)XMALLOC(ctxSz, NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPub == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSA_InitPublicKey(key->nSz, key->eSz, key->pPub, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPublicKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsRSA_SetPublicKey(key->n, key->e, key->pPub);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_SetPublicKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    /* get bn sizes needed for private key set up */
    ret = ippsGetSize_BN(key->pipp, &pSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsGetSize_BN(key->qipp, &qSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    /* store sizes needed for creating tmp private keys */
    ret = ippsGetSize_BN(key->dipp, &key->dSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    /* convert to size in bits */
    key->dSz = key->dSz * 8;
    pSz = pSz * 8;
    qSz = qSz * 8;

    /* set up private key state */
    ret = ippsRSA_GetSizePrivateKeyType2(pSz, qSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePrivateKey error %s\n",
                ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->prvSz = ctxSz;
    key->pPrv = (IppsRSAPrivateKeyState*)XMALLOC(ctxSz, 0,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPrv == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSA_InitPrivateKeyType2(pSz, qSz, key->pPrv, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPrivateKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsRSA_SetPrivateKeyType2(key->pipp, key->qipp, key->dPipp,
            key->dQipp, key->uipp, key->pPrv);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_SetPrivateKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    USER_DEBUG(("\tExit wc_RsaPrivateKeyDecode\n"));

    return 0;
}


int wc_RsaPublicKeyDecode_ex(const byte* input, word32* inOutIdx,
        word32 inSz, const byte** n, word32* nSz, const byte** e, word32* eSz)
{
    IppStatus ret = 0;
    int length;
#if defined(OPENSSL_EXTRA) || defined(RSA_DECODE_EXTRA)
    byte b;
#endif

    if (input == NULL || inOutIdx == NULL) {
        return USER_CRYPTO_ERROR;
    }

    USER_DEBUG(("Entering wc_RsaPublicKeyDecode_ex\n"));

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return USER_CRYPTO_ERROR;

#if defined(OPENSSL_EXTRA) || defined(RSA_DECODE_EXTRA)
    if ((*inOutIdx + 1) > inSz)
        return USER_CRYPTO_ERROR;

    b = input[*inOutIdx];
    if (b != ASN_INTEGER) {
        /* not from decoded cert, will have algo id, skip past */
        if (GetSequence(input, inOutIdx, &length, inSz) < 0)
            return USER_CRYPTO_ERROR;

        b = input[(*inOutIdx)++];
        if (b != ASN_OBJECT_ID)
            return USER_CRYPTO_ERROR;

        if (GetLength(input, inOutIdx, &length, inSz) < 0)
            return USER_CRYPTO_ERROR;

        *inOutIdx += length;   /* skip past */

        /* could have NULL tag and 0 terminator, but may not */
        b = input[(*inOutIdx)++];

        if (b == ASN_TAG_NULL) {
            b = input[(*inOutIdx)++];
            if (b != 0)
                return USER_CRYPTO_ERROR;
        }
        else {
            /* go back, didn't have it */
            (*inOutIdx)--;
        }

        /* should have bit tag length and seq next */
        b = input[(*inOutIdx)++];
        if (b != ASN_BIT_STRING)
            return USER_CRYPTO_ERROR;

        if (GetLength(input, inOutIdx, &length, inSz) <= 0)
            return USER_CRYPTO_ERROR;

        /* could have 0 */
        b = input[(*inOutIdx)++];
        if (b != 0)
            (*inOutIdx)--;

        if (GetSequence(input, inOutIdx, &length, inSz) < 0)
            return USER_CRYPTO_ERROR;
    }
#endif /* OPENSSL_EXTRA || RSA_DECODE_EXTRA */

    /* Get modulus */
    ret = GetASNInt(input, inOutIdx, &length, inSz);
    if (ret < 0) {
        return USER_CRYPTO_ERROR;
    }
    if (nSz)
        *nSz = length;
    if (n)
        *n = &input[*inOutIdx];
    *inOutIdx += length;

    /* Get exponent */
    ret = GetASNInt(input, inOutIdx, &length, inSz);
    if (ret < 0) {
        return USER_CRYPTO_ERROR;
    }
    if (eSz)
        *eSz = length;
    if (e)
        *e = &input[*inOutIdx];
    *inOutIdx += length;

    USER_DEBUG(("\tExit wc_RsaPublicKeyDecode_ex\n"));

    return ret;
}

/* read in a public RSA key */
int wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx, RsaKey* key,
                       word32 inSz)
{
    IppStatus ret;
    const byte *n = NULL, *e = NULL;
    word32 nSz = 0, eSz = 0;

    if (key == NULL)
        return USER_CRYPTO_ERROR;

    USER_DEBUG(("Entering wc_RsaPublicKeyDecode\n"));

    ret = wc_RsaPublicKeyDecode_ex(input, inOutIdx, inSz, &n, &nSz, &e, &eSz);
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecodeRaw(n, nSz, e, eSz, key);
    }

    USER_DEBUG(("\tExit RsaPublicKeyDecode\n"));

    return ret;
}

/* import RSA public key elements (n, e) into RsaKey structure (key) */
int wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz, const byte* e,
                             word32 eSz, RsaKey* key)
{
    IppStatus ret;
    int ctxSz;

    USER_DEBUG(("Entering wc_RsaPublicKeyDecodeRaw\n"));

    if (n == NULL || e == NULL || key == NULL)
        return USER_CRYPTO_ERROR;

    /* set up IPP key states -- read in n */
    ret = init_bn(&key->n, nSz);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    ret = ippsSetOctString_BN((Ipp8u*)n, nSz, key->n);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    /* read in e */
    ret = init_bn(&key->e, eSz);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    ret = ippsSetOctString_BN((Ipp8u*)e, eSz, key->e);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    /* store size and convert to binary */
    key->sz = nSz;
    nSz = nSz * 8;
    eSz = eSz * 8;

    /* set up public key state */
    ret = ippsRSA_GetSizePublicKey(nSz, eSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePublicKey error %s\n",
                ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->pPub = (IppsRSAPublicKeyState*)XMALLOC(ctxSz, NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPub == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSA_InitPublicKey(nSz, eSz, key->pPub, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPublicKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsRSA_SetPublicKey(key->n,key->e, key->pPub);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_SetPublicKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    key->nSz = nSz;
    key->eSz = eSz;
    key->type = RSA_PUBLIC;

    return 0;
}


/* encrypt using PKCS v15 */
int wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out, word32 outLen,
                     RsaKey* key, WC_RNG* rng)
{
    IppStatus ret;
    Ipp8u* scratchBuffer;
    int    scratchSz;

    if (key == NULL || in == NULL || out == NULL)
        return USER_CRYPTO_ERROR;

    if (key->pPub == NULL || outLen < key->sz)
        return USER_CRYPTO_ERROR;

    /* set size of scratch buffer */
    ret = ippsRSA_GetBufferSizePublicKey(&scratchSz, key->pPub);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    scratchBuffer = (Ipp8u*)XMALLOC(scratchSz*(sizeof(Ipp8u)), 0,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (scratchBuffer == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSAEncrypt_PKCSv15((Ipp8u*)in, inLen, NULL, (Ipp8u*)out,
            key->pPub, scratchBuffer);
    if (ret != ippStsNoErr) {
        XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        USER_DEBUG(("encrypt error of %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    (void)rng;
    return key->sz;
}


/* decrypt using PLCS v15 */
int wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out, word32 outLen,
                     RsaKey* key)
{
    IppStatus ret;
    Ipp8u* scratchBuffer;
    int    scratchSz;
    int    outSz;

    if (in == NULL || out == NULL || key == NULL)
        return USER_CRYPTO_ERROR;

    if (key->pPrv == NULL || inLen != key->sz)
        return USER_CRYPTO_ERROR;

    outSz = outLen;

    /* set size of scratch buffer */
    ret = ippsRSA_GetBufferSizePrivateKey(&scratchSz, key->pPrv);
    if (ret != ippStsNoErr) {
        return USER_CRYPTO_ERROR;
    }

    scratchBuffer = (Ipp8u*)XMALLOC(scratchSz*(sizeof(Ipp8u)), 0,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (scratchBuffer == NULL) {
        return USER_CRYPTO_ERROR;
    }

    /* perform decryption using IPP */
    ret = ippsRSADecrypt_PKCSv15((Ipp8u*)in, (Ipp8u*)out, &outSz, key->pPrv,
            scratchBuffer);
    if (ret != ippStsNoErr) {
        XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        USER_DEBUG(("decrypt error of %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return outSz;
}


/* out is a pointer that is set to the location in byte array "in" where input
 data has been decrypted */
int wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    int outSz;
    byte* tmp;

    USER_DEBUG(("Entering wc_RsaPrivateDecryptInline\n"));

    /* allocate a buffer for max decrypted text */
    tmp = (byte*)XMALLOC(key->sz, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (tmp == NULL)
        return USER_CRYPTO_ERROR;

    outSz = wc_RsaPrivateDecrypt(in, inLen, tmp, key->sz, key);
    if (outSz >= 0) {
        XMEMCPY(in, tmp, outSz);
        *out = in;
    }
    else {
        XFREE(tmp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return USER_CRYPTO_ERROR;
    }

    XFREE(tmp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    USER_DEBUG(("\tExit wc_RsaPrivateDecryptInline\n"));

    return outSz;
}


/* Used to clean up memory when exiting, clean up memory used */
static int FreeHelper(IppsBigNumState* pTxt, IppsBigNumState* cTxt,
        Ipp8u* scratchBuffer, void* pPub)
{
    if (pTxt != NULL)
        XFREE(pTxt, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (cTxt != NULL)
        XFREE(cTxt, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (scratchBuffer != NULL)
        XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (pPub != NULL)
        XFREE(pPub, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return 0;
}


/* for Rsa Verify
    in : byte array to be verified
    inLen : length of input array
    out : pointer to location of in byte array that has been verified
 */
int wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{

    int ctxSz;
    int scratchSz;
    Ipp8u* scratchBuffer = NULL;
    IppStatus ret;
    IppsRSAPrivateKeyState* pPub = NULL;
    IppsBigNumState* pTxt = NULL;
    IppsBigNumState* cTxt = NULL;

    USER_DEBUG(("Entering wc_RsaSSL_VerifyInline\n"));

    if (key == NULL || key->n == NULL || key->e == NULL) {
        USER_DEBUG(("n or e element was null\n"));
        return USER_CRYPTO_ERROR;
    }

    if (in == NULL || inLen == 0 || out == NULL)
        return USER_CRYPTO_ERROR;

    /* set up a private key state using public key values */
    ret = ippsRSA_GetSizePrivateKeyType1(key->nSz, key->eSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePrivateKey error %s\n",
                ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    pPub = (IppsRSAPrivateKeyState*)XMALLOC(ctxSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (pPub == NULL)
        return USER_CRYPTO_ERROR;

    ret = ippsRSA_InitPrivateKeyType1(key->nSz, key->eSz, pPub, ctxSz);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        USER_DEBUG(("ippsRSA_InitPrivateKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    ret = ippsRSA_SetPrivateKeyType1(key->n, key->e, pPub);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        USER_DEBUG(("ippsRSA_SetPrivateKey error %s\n",
                    ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    /* set size of scratch buffer */
    ret = ippsRSA_GetBufferSizePrivateKey(&scratchSz, pPub);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }

    scratchBuffer = (Ipp8u*)XMALLOC(scratchSz*(sizeof(Ipp8u)), 0,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (scratchBuffer == NULL) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }

    /* load plain and cipher into big num states */
    ret = init_bn(&pTxt, key->sz);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }
    ret = ippsSetOctString_BN((Ipp8u*)in, key->sz, pTxt);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }

    /* set up cipher to hold signature */
    ret = init_bn(&cTxt, key->sz);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }
    ret = ippsSetOctString_BN((Ipp8u*)in, key->sz, cTxt);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        return USER_CRYPTO_ERROR;
    }

    /* decrypt using public key information */
    ret = ippsRSA_Decrypt(cTxt, pTxt, pPub, scratchBuffer);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        USER_DEBUG(("decrypt error of %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    /* extract big num struct to octet string */
    ret = ippsGetOctString_BN((Ipp8u*)in, key->sz, pTxt);
    if (ret != ippStsNoErr) {
        FreeHelper(pTxt, cTxt, scratchBuffer, pPub);
        USER_DEBUG(("BN get string error of %s\n", ippGetStatusString(ret)));
        return USER_CRYPTO_ERROR;
    }

    FreeHelper(pTxt, cTxt, scratchBuffer, pPub);

    /* unpad the decrypted information and return size of array */
    return RsaUnPad(in, inLen, out, RSA_BLOCK_TYPE_1);
}


/* sets up and call VerifyInline to verify a signature */
int wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out, word32 outLen,
                     RsaKey* key)
{
    int plainLen;
    byte*  tmp;
    byte*  pad = 0;

    if (out == NULL || in == NULL || key == NULL)
        return USER_CRYPTO_ERROR;

    tmp = (byte*)XMALLOC(inLen, key->heap, DYNAMIC_TYPE_USER_CRYPTO);
    if (tmp == NULL) {
        return USER_CRYPTO_ERROR;
    }

    XMEMCPY(tmp, in, inLen);

    /* verify signature and test if output buffer is large enough */
    plainLen = wc_RsaSSL_VerifyInline(tmp, inLen, &pad, key);
    if (plainLen < 0) {
        XFREE(tmp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        return plainLen;
    }

    if (plainLen > (int)outLen)
        plainLen = USER_CRYPTO_ERROR;
    else
        XMEMCPY(out, pad, plainLen);

    ForceZero(tmp, inLen);
    XFREE(tmp, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    return plainLen;
}


/* Check if a > b , if so c = a mod b
   return ippStsNoErr on success */
static IppStatus reduce(IppsBigNumState* a, IppsBigNumState* b,
        IppsBigNumState* c)
{
    IppStatus ret;

    if ((ret = ippsMod_BN(a, b, c)) != ippStsNoErr)
        return ret;

    return ippStsNoErr;
}


static IppStatus exptmod(IppsBigNumState* a, IppsBigNumState* b,
        IppsMontState* mont, IppsBigNumState* out, IppsBigNumState* one)
{
    IppStatus ret;

    ret = ippsMontForm(a, mont, a);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMontForm error of %s\n", ippGetStatusString(ret)));
        return ret;
    }

    /* a = a^b mod mont */
    ret = ippsMontExp(a, b, mont, out);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMontExp error of %s\n", ippGetStatusString(ret)));
        return ret;
    }

    /* convert back from montgomery */
    ret = ippsMontMul(out, one, mont, out);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMontMul error of %s\n", ippGetStatusString(ret)));
        return ret;
    }

    return ippStsNoErr;
}


static void Free_BN(IppsBigNumState* bn)
{
    int sz, ctxSz;
    IppStatus ret;

    if (bn != NULL) {
        ret = ippStsNoErr;
        ret |= ippsGetSize_BN(bn, &sz);
        ret |= ippsBigNumGetSize(sz, &ctxSz);
        if (ret == ippStsNoErr) {
            ForceZero(bn, ctxSz);
        }
        else {
            USER_DEBUG(("Issue with clearing a struct in RsaSSL_Sign free\n"));
        }
        XFREE(bn, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    }
}


/* free up memory used during CRT sign operation */
static void FreeSignHelper(IppsBigNumState* one, IppsBigNumState* tmp,
        IppsBigNumState* tmpP, IppsBigNumState* tmpQ, IppsBigNumState* tmpa,
        IppsBigNumState* tmpb)
{
    Free_BN(one);
    Free_BN(tmp);
    Free_BN(tmpP);
    Free_BN(tmpQ);
    Free_BN(tmpa);
    Free_BN(tmpb);
}


/* for Rsa Sign */
int wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                      RsaKey* key, WC_RNG* rng)
{
    int sz, pSz, qSz;
    IppStatus ret;
    word32 outSz;

    IppsMontState* pMont = NULL;
    IppsMontState* qMont = NULL;

    IppsBigNumState* one  = NULL;
    IppsBigNumState* tmp  = NULL;
    IppsBigNumState* tmpP = NULL;
    IppsBigNumState* tmpQ = NULL;
    IppsBigNumState* tmpa = NULL;
    IppsBigNumState* tmpb = NULL;

    IppsBigNumSGN sa, sb;

    Ipp8u o[1];
    o[0] = 1;

    USER_DEBUG(("Entering wc_RsaSSL_Sign\n"));

    if (in == NULL || out == NULL || key == NULL || rng == NULL) {
        USER_DEBUG(("Bad argument to wc_RsaSSL_Sign\n"));
        return USER_CRYPTO_ERROR;
    }

    sz = key->sz;


    /* sanity check on key being used */
    if (key->pipp == NULL || key->qipp == NULL || key->uipp == NULL ||
            key->dPipp == NULL || key->dQipp == NULL) {
        USER_DEBUG(("Bad key argument to wc_RsaSSL_Sign\n"));
        return USER_CRYPTO_ERROR;
    }

    if (sz > (int)outLen) {
        USER_DEBUG(("Bad argument outLen to wc_RsaSSL_Sign\n"));
        return USER_CRYPTO_ERROR;
    }

    if (sz < RSA_MIN_PAD_SZ) {
        USER_DEBUG(("Key size is too small\n"));
        return USER_CRYPTO_ERROR;
    }

    if (inLen > (word32)(sz - RSA_MIN_PAD_SZ)) {
        USER_DEBUG(("Bad argument inLen to wc_RsaSSL_Sign\n"));
        return USER_CRYPTO_ERROR;
    }

    /* Set up needed pkcs v15 padding */
    if (wc_RsaPad(in, inLen, out, sz, RSA_BLOCK_TYPE_1, rng) != 0) {
        USER_DEBUG(("RSA Padding error\n"));
        return USER_CRYPTO_ERROR;
    }

    /* tmp = input to sign */
    ret = init_bn(&tmp, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }
    ret = ippsSetOctString_BN(out, sz, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsSetOctString_BN error of %s\n",
                                                      ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpP = tmp mod p */
    ret = init_bn(&tmpP, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpQ = tmp mod q */
    ret = init_bn(&tmpQ, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpa */
    ret = init_bn(&tmpa, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpb */
    ret = init_bn(&tmpb, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* one : used for conversion from Montgomery to classical */
    ret = init_bn(&one, sz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_BN error of %s\n", ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }
    ret = ippsSetOctString_BN(o, 1, one);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsSetOctString_BN error of %s\n",
                    ippGetStatusString(ret)));
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /**
      Set up Montgomery state
      */
    ret = init_mont(&pMont, &pSz, key->pipp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_mont error of %s\n", ippGetStatusString(ret)));
        if (pMont != NULL) {
            XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        }
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    ret = init_mont(&qMont, &qSz, key->qipp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("init_mont error of %s\n", ippGetStatusString(ret)));
        if (qMont != NULL) {
            XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        }
        ForceZero(pMont, pSz);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /**
      Check and reduce input
      This is needed for calls to MontExp since required value of a < modulus
      */
    ret = reduce(tmp, key->pipp, tmpP);
    if (ret != ippStsNoErr)
    {
        USER_DEBUG(("reduce error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    ret = reduce(tmp, key->qipp, tmpQ);
    if (ret != ippStsNoErr)
    {
        USER_DEBUG(("reduce error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpa = (tmp mod p)^dP mod p */
    ret = exptmod(tmpP, key->dPipp, pMont, tmpa, one);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("exptmod error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmpb = (tmp mod q)^dQ mod q */
    ret = exptmod(tmpQ, key->dQipp, qMont, tmpb, one);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("exptmod error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* tmp = (tmpa - tmpb) * qInv (mod p) */
    ret = ippsSub_BN(tmpa, tmpb, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsSub_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    ret = ippsMul_BN(tmp, key->uipp, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsMul_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* mod performed the same was as wolfSSL fp_mod -- tmpa is just scratch */
   ret = ippsDiv_BN(tmp, key->pipp, tmpa, tmp);
   if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsDiv_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
   }

    /* Check sign of values and perform conditional add */
    ret = ippsExtGet_BN(&sa, NULL, NULL, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsExtGet_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }
    ret = ippsExtGet_BN(&sb, NULL, NULL, key->pipp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsExtGet_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }
    if (sa != sb) {
        ret = ippsAdd_BN(tmp, key->pipp, tmp);
        if (ret != ippStsNoErr) {
            USER_DEBUG(("ippsAdd_BN error of %s\n", ippGetStatusString(ret)));
            ForceZero(pMont, pSz);
            ForceZero(qMont, qSz);
            XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
            return USER_CRYPTO_ERROR;
        }
    }

    /* tmp = tmpb + q * tmp */
    ret = ippsMul_BN(tmp, key->qipp, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsSub_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }


    ret = ippsAdd_BN(tmp, tmpb, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsSub_BN error of %s\n", ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    /* Extract the output */
    ret = ippsGetOctString_BN(out, sz, tmp);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetOctString_BN error of %s\n",
                    ippGetStatusString(ret)));
        ForceZero(pMont, pSz);
        ForceZero(qMont, qSz);
        XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);
        return USER_CRYPTO_ERROR;
    }

    outSz = sz;

    /* clear memory and free */
    ForceZero(pMont, pSz);
    ForceZero(qMont, qSz);
    XFREE(qMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    XFREE(pMont, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    FreeSignHelper(one, tmp, tmpP, tmpQ, tmpa, tmpb);

    return outSz;
}


int wc_RsaEncryptSize(RsaKey* key)
{
    if (key == NULL)
        return 0;

    return key->sz;
}


/* flatten RsaKey structure into individual elements (e, n) */
int wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz, byte* n,
                           word32* nSz)
{
    int sz, bytSz;
    IppStatus ret;

    USER_DEBUG(("Entering wc_RsaFlattenPublicKey\n"));

    if (key == NULL || e == NULL || eSz == NULL || n == NULL || nSz == NULL)
       return USER_CRYPTO_ERROR;

    bytSz = sizeof(byte) * 8;
    ret = ippsExtGet_BN(NULL, &sz, NULL, key->e);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    /* sz is in bits change to bytes */
    sz = (sz / bytSz) + ((sz % bytSz)? 1 : 0);

    if (*eSz < (word32)sz)
        return USER_CRYPTO_ERROR;

    ret = ippsGetOctString_BN(e, sz, key->e);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    *eSz = (word32)sz;

    /* flatten n */
    ret = ippsExtGet_BN(NULL, &sz, NULL, key->n);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    /* sz is in bits change to bytes */
    sz = (sz / bytSz) + ((sz % bytSz)? 1: 0);

    if (*nSz < (word32)sz)
        return USER_CRYPTO_ERROR;

    ret = ippsGetOctString_BN(n, sz, key->n);
    if (ret != ippStsNoErr)
        return USER_CRYPTO_ERROR;

    *nSz = (word32)sz;

    return 0;
}


IppStatus wolfSSL_rng(Ipp32u* pData, int nBits, void* pEbsParams);
IppStatus wolfSSL_rng(Ipp32u* pData, int nBits, void* pEbsParams)
{
    int nBytes;

    if (pData == NULL) {
        USER_DEBUG(("error with wolfSSL_rng argument\n"));
        return ippStsErr;
    }

    nBytes = (nBits/8) + ((nBits % 8)? 1: 0);
    if (wc_RNG_GenerateBlock((WC_RNG*)pEbsParams, (byte*)pData, nBytes) != 0) {
        USER_DEBUG(("error in generating random wolfSSL block\n"));
        return ippStsErr;
    }

    return ippStsNoErr;
}


#ifdef WOLFSSL_KEY_GEN
/* Make an RSA key for size bits, with e specified, 65537 is a good e */
int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
    IppStatus ret;
    int scratchSz;
    int i; /* for trys on calling make key */
    int ctxSz;

    IppsBigNumState* pSrcPublicExp = NULL;
    Ipp8u* scratchBuffer = NULL;
    Ipp8u  eAry[8];
    int trys = 8; /* Miller-Rabin test parameter */
    IppsPrimeState* pPrime = NULL;

    int qBitSz; /* size of q factor */
    int bytSz; /* size of key in bytes */
    int leng;

    USER_DEBUG(("Entering wc_MakeRsaKey\n"));

    /* get byte size and individual private key size -- round up */
    qBitSz = (size / 2) + ((size % 2)? 1: 0);
    bytSz  = (size / 8) + ((size % 8)? 1: 0);

    if (key == NULL || rng == NULL) {
        USER_DEBUG(("Error, NULL argument passed in\n"));
        return USER_CRYPTO_ERROR;
    }

    if (e < 3 || (e&1) == 0)
        return USER_CRYPTO_ERROR;

    if (size > RSA_MAX_SIZE || size < RSA_MIN_SIZE)
        return USER_CRYPTO_ERROR;

    key->type = RSA_PRIVATE;
    key->sz   = bytSz;

    /* initialize prime number */
    ret = ippsPrimeGetSize(size, &ctxSz); /* size in bits */
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsPrimeGetSize error of %s\n", ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    pPrime = (IppsPrimeState*)XMALLOC(ctxSz, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (pPrime == NULL) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    ret = ippsPrimeInit(size, pPrime);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsPrimeInit error of %s\n", ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* define RSA privete key type 2 */
    /* length in bits of p and q factors */
    ret = ippsRSA_GetSizePrivateKeyType2(qBitSz, qBitSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePrivateKeyType2 error of %s\n",
                ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    key->prvSz = ctxSz; /* used when freeing private key */
    key->pPrv = (IppsRSAPrivateKeyState*)XMALLOC(ctxSz, NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPrv == NULL) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* length in bits of p and q factors */
    ret = ippsRSA_InitPrivateKeyType2(qBitSz, qBitSz, key->pPrv, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPrivateKeyType2 error of %s\n",
                ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* allocate scratch buffer */
    ret = ippsRSA_GetBufferSizePrivateKey(&scratchSz, key->pPrv);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetBufferSizePrivateKey error of %s\n",
                ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    scratchBuffer = (Ipp8u*)XMALLOC(scratchSz, 0, DYNAMIC_TYPE_USER_CRYPTO);
    if (scratchBuffer == NULL) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up initial value of pScrPublicExp */
    leng = (int)sizeof(long); /* # of Ipp32u in long */

    /* place the value of e into the array eAry then load into BN */
    for (i = 0; i < leng; i++) {
        eAry[i] = (e >> (8 * (leng - 1 - i))) & 0XFF;
    }
    ret = init_bn(&pSrcPublicExp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    ret = ippsSetOctString_BN(eAry, leng, pSrcPublicExp);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* initializing key->n */
    ret = init_bn(&key->n, bytSz);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* initializing public exponent key->e */
    ret = init_bn(&key->e, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* private exponent key->dipp */
    ret = init_bn(&key->dipp, bytSz);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* call IPP to generate keys, if inseficent entropy error call again */
    ret = ippStsInsufficientEntropy;
    while (ret == ippStsInsufficientEntropy) {
        ret = ippsRSA_GenerateKeys(pSrcPublicExp, key->n, key->e,
                key->dipp, key->pPrv, scratchBuffer, trys, pPrime,
                wolfSSL_rng, rng);
        if (ret == ippStsNoErr) {
            break;
        }

        /* catch all errors other than entropy error */
        if (ret != ippStsInsufficientEntropy) {
            USER_DEBUG(("ippsRSA_GeneratKeys error of %s\n",
                    ippGetStatusString(ret)));
            ret = USER_CRYPTO_ERROR;
            goto makeKeyEnd;
        }
    }

    /* get bn sizes needed for private key set up */
    ret = ippsExtGet_BN(NULL, &key->eSz, NULL, key->e);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    ret = ippsExtGet_BN(NULL, &key->nSz, NULL, key->n);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsGetSize_BN error %s\n", ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up public key state */
    ret = ippsRSA_GetSizePublicKey(key->nSz, key->eSz, &ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetSizePublicKey error %s nSz = %d eSz = %d\n",
                ippGetStatusString(ret), key->nSz, key->eSz));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    key->pPub = (IppsRSAPublicKeyState*)XMALLOC(ctxSz, NULL,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
    if (key->pPub == NULL) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    ret = ippsRSA_InitPublicKey(key->nSz, key->eSz, key->pPub, ctxSz);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_InitPublicKey error %s\n",
                    ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    ret = ippsRSA_SetPublicKey(key->n, key->e, key->pPub);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_SetPublicKey error %s\n",
                    ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* get private key information for key struct */
    leng = size/16; /* size of q, p, u, dP, dQ */
    ret = init_bn(&key->pipp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up q BN for key */
    ret = init_bn(&key->qipp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up dP BN for key */
    ret = init_bn(&key->dPipp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up dQ BN for key */
    ret = init_bn(&key->dQipp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* set up u BN for key */
    ret = init_bn(&key->uipp, leng);
    if (ret != ippStsNoErr) {
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }

    /* get values from created key */
    ret = ippsRSA_GetPrivateKeyType2(key->pipp, key->qipp, key->dPipp,
            key->dQipp, key->uipp, key->pPrv);
    if (ret != ippStsNoErr) {
        USER_DEBUG(("ippsRSA_GetPrivateKeyType2 error %s\n",
                ippGetStatusString(ret)));
        ret = USER_CRYPTO_ERROR;
        goto makeKeyEnd;
    }
    ret = 0; /* success case */

makeKeyEnd:
    /* clean up memory used */
    XFREE(pSrcPublicExp, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    XFREE(scratchBuffer, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    XFREE(pPrime, NULL, DYNAMIC_TYPE_USER_CRYPTO);

    if (ret != 0) { /* with fail case free RSA components created */
        wc_FreeRsaKey(key);
    }

    return ret;
}

#endif

#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA)

/********** duplicate code needed -- future refactor */
#define MAX_VERSION_SZ 5
#define MAX_SEQ_SZ 5
#define ASN_CONTEXT_SPECIFIC 0x80
#define ASN_CONSTRUCTED 0x20
#define ASN_LONG_LENGTH 0x80
#define ASN_SEQUENCE 0x10
#define RSA_INTS 8
#define FALSE 0
#define TRUE 1

#define MAX_LENGTH_SZ 4
#define RSAk 645
#define keyType 2
#define MAX_RSA_INT_SZ 517
#define MAX_RSA_E_SZ 16
#define MAX_ALGO_SZ 20

static word32 BytePrecision(word32 value)
{
    word32 i;
    for (i = sizeof(value); i; --i)
        if (value >> ((i - 1) * WOLFSSL_BIT_SIZE))
            break;

    return i;
}


static int SetMyVersion(word32 version, byte* output, int header)
{
    int i = 0;

    if (output == NULL)
        return USER_CRYPTO_ERROR;

    if (header) {
        output[i++] = ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED;
        output[i++] = ASN_BIT_STRING;
    }
    output[i++] = ASN_INTEGER;
    output[i++] = 0x01;
    output[i++] = (byte)version;

    return i;
}


static word32 SetLength(word32 length, byte* output)
{
    word32 i = 0, j;

    if (length < 0x80)
        output[i++] = (byte)length;
    else {
        output[i++] = (byte)(BytePrecision(length) | ASN_LONG_LENGTH);

        for (j = BytePrecision(length); j; --j) {
            output[i] = (byte)(length >> ((j - 1) * WOLFSSL_BIT_SIZE));
            i++;
        }
    }

    return i;
}


static word32 SetSequence(word32 len, byte* output)
{
    output[0] = ASN_SEQUENCE | ASN_CONSTRUCTED;
    return SetLength(len, output + 1) + 1;
}


static word32 SetAlgoID(int algoOID, byte* output, int type, int curveSz)
{
    /* adding TAG_NULL and 0 to end */

    /* RSA keyType */
    #ifndef NO_RSA
        static const byte RSA_AlgoID[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
                                            0x01, 0x01, 0x01, 0x05, 0x00};
    #endif /* NO_RSA */

    int    algoSz = 0;
    int    tagSz  = 2;   /* tag null and terminator */
    word32 idSz, seqSz;
    const  byte* algoName = 0;
    byte ID_Length[MAX_LENGTH_SZ];
    byte seqArray[MAX_SEQ_SZ + 1];  /* add object_id to end */

    if (type == keyType) {    /* keyType */
        switch (algoOID) {
        #ifndef NO_RSA
            case RSAk:
                algoSz = sizeof(RSA_AlgoID);
                algoName = RSA_AlgoID;
                break;
        #endif /* NO_RSA */
        default:
            /* unknown key algo */
            return 0;
        }
    }
    else {
        /* unknown algo type */
        return 0;
    }

    idSz  = SetLength(algoSz - tagSz, ID_Length); /* don't include tags */
    seqSz = SetSequence(idSz + algoSz + 1 + curveSz, seqArray);
                 /* +1 for object id, curveID of curveSz follows for ecc */
    seqArray[seqSz++] = ASN_OBJECT_ID;

    XMEMCPY(output, seqArray, seqSz);
    XMEMCPY(output + seqSz, ID_Length, idSz);
    XMEMCPY(output + seqSz + idSz, algoName, algoSz);

    return seqSz + idSz + algoSz;

}


/* Write a public RSA key to output */
static int SetRsaPublicKey(byte* output, RsaKey* key,
                           int outLen, int with_header)
{
#ifdef WOLFSSL_SMALL_STACK
    byte* n = NULL;
    byte* e = NULL;
#else
    byte n[MAX_RSA_INT_SZ];
    byte e[MAX_RSA_E_SZ];
#endif
    byte seq[MAX_SEQ_SZ];
    byte len[MAX_LENGTH_SZ + 1];  /* trailing 0 */
    int  nSz;
    int  eSz;
    int  seqSz;
    int  lenSz;
    int  idx;
    int  rawLen;
    int  leadingBit;
    int  err;

    if (output == NULL || key == NULL || outLen < MAX_SEQ_SZ)
        return USER_CRYPTO_ERROR;

    /* n */
#ifdef WOLFSSL_SMALL_STACK
    n = (byte*)XMALLOC(MAX_RSA_INT_SZ, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (n == NULL)
        return USER_CRYPTO_ERROR;
#endif

    leadingBit = wc_Rsa_leading_bit(key->n);
    rawLen = wc_Rsa_unsigned_bin_size(key->n);
    if ((int)rawLen < 0) {
        return USER_CRYPTO_ERROR;
    }

    rawLen = rawLen + leadingBit;
    n[0] = ASN_INTEGER;
    nSz  = SetLength(rawLen, n + 1) + 1;  /* int tag */

    if ( (nSz + rawLen) < MAX_RSA_INT_SZ) {
        if (leadingBit)
            n[nSz] = 0;
        err = ippsGetOctString_BN((Ipp8u*)n + nSz, rawLen - leadingBit, key->n);
        if (err == ippStsNoErr)
            nSz += rawLen;
        else {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
            return USER_CRYPTO_ERROR;
        }
    }
    else {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
        return USER_CRYPTO_ERROR;
    }

    /* e */
#ifdef WOLFSSL_SMALL_STACK
    e = (byte*)XMALLOC(MAX_RSA_E_SZ, NULL, DYNAMIC_TYPE_USER_CRYPTO);
    if (e == NULL) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
        return USER_CRYPTO_ERROR;
    }
#endif

    leadingBit = wc_Rsa_leading_bit(key->e);
    rawLen = wc_Rsa_unsigned_bin_size(key->e);
    if ((int)rawLen < 0) {
        return USER_CRYPTO_ERROR;
    }

    rawLen = rawLen + leadingBit;
    e[0] = ASN_INTEGER;
    eSz  = SetLength(rawLen, e + 1) + 1;  /* int tag */

    if ( (eSz + rawLen) < MAX_RSA_E_SZ) {
        if (leadingBit)
            e[eSz] = 0;
        err = ippsGetOctString_BN((Ipp8u*)e + eSz, rawLen - leadingBit, key->e);
        if (err == ippStsNoErr)
            eSz += rawLen;
        else {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            XFREE(e, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
            return USER_CRYPTO_ERROR;
        }
    }
    else {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(e, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
        return USER_CRYPTO_ERROR;
    }

    seqSz  = SetSequence(nSz + eSz, seq);

    /* check output size */
    if ( (seqSz + nSz + eSz) > outLen) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
        XFREE(e,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
        return USER_CRYPTO_ERROR;
    }

    /* headers */
    if (with_header) {
        int  algoSz;
#ifdef WOLFSSL_SMALL_STACK
        byte* algo;

        algo = (byte*)XMALLOC(MAX_ALGO_SZ, NULL, DYNAMIC_TYPE_USER_CRYPTO);
        if (algo == NULL) {
            XFREE(n, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            XFREE(e, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            return USER_CRYPTO_ERROR;
        }
#else
        byte algo[MAX_ALGO_SZ];
#endif
        algoSz = SetAlgoID(RSAk, algo, keyType, 0);
        lenSz  = SetLength(seqSz + nSz + eSz + 1, len);
        len[lenSz++] = 0;   /* trailing 0 */

        /* write, 1 is for ASN_BIT_STRING */
        idx = SetSequence(nSz + eSz + seqSz + lenSz + 1 + algoSz, output);

        /* check output size */
        if ( (idx + algoSz + 1 + lenSz + seqSz + nSz + eSz) > outLen) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(n,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
                XFREE(e,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
                XFREE(algo, NULL, DYNAMIC_TYPE_USER_CRYPTO);
            #endif

            return USER_CRYPTO_ERROR;
        }

        /* algo */
        XMEMCPY(output + idx, algo, algoSz);
        idx += algoSz;
        /* bit string */
        output[idx++] = ASN_BIT_STRING;
        /* length */
        XMEMCPY(output + idx, len, lenSz);
        idx += lenSz;
#ifdef WOLFSSL_SMALL_STACK
        XFREE(algo, NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif
    }
    else
        idx = 0;

    /* seq */
    XMEMCPY(output + idx, seq, seqSz);
    idx += seqSz;
    /* n */
    XMEMCPY(output + idx, n, nSz);
    idx += nSz;
    /* e */
    XMEMCPY(output + idx, e, eSz);
    idx += eSz;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(n,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
    XFREE(e,    NULL, DYNAMIC_TYPE_USER_CRYPTO);
#endif

    return idx;
}


static IppsBigNumState* GetRsaInt(RsaKey* key, int idx)
{
    if (idx == 0)
        return key->n;
    if (idx == 1)
        return key->e;
    if (idx == 2)
        return key->dipp;
    if (idx == 3)
        return key->pipp;
    if (idx == 4)
        return key->qipp;
    if (idx == 5)
        return key->dPipp;
    if (idx == 6)
        return key->dQipp;
    if (idx == 7)
        return key->uipp;

    return NULL;
}


/* Release Tmp RSA resources */
static WC_INLINE void FreeTmpRsas(byte** tmps, void* heap)
{
    int i;

    (void)heap;

    for (i = 0; i < RSA_INTS; i++)
        XFREE(tmps[i], heap, DYNAMIC_TYPE_USER_CRYPTO);
}


/* Convert RsaKey key to DER format, write to output (inLen), return bytes
   written */
int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen)
{
    word32 seqSz, verSz, rawLen, intTotalLen = 0;
    word32 sizes[RSA_INTS];
    int    i, j, outLen, ret = 0, lbit;

    byte  seq[MAX_SEQ_SZ];
    byte  ver[MAX_VERSION_SZ];
    byte* tmps[RSA_INTS];

    USER_DEBUG(("Entering RsaKeyToDer\n"));

    if (!key)
        return USER_CRYPTO_ERROR;

    if (key->type != RSA_PRIVATE)
        return USER_CRYPTO_ERROR;

    for (i = 0; i < RSA_INTS; i++)
        tmps[i] = NULL;

    /* write all big ints from key to DER tmps */
    for (i = 0; i < RSA_INTS; i++) {
        Ipp32u isZero;
        IppsBigNumState* keyInt = GetRsaInt(key, i);

        ippsCmpZero_BN(keyInt, &isZero); /* makes isZero 0 if true */
        rawLen = wc_Rsa_unsigned_bin_size(keyInt);
        if ((int)rawLen < 0) {
            return USER_CRYPTO_ERROR;
        }

        /* leading zero */
        if (!isZero || wc_Rsa_leading_bit(keyInt))
            lbit = 1;
        else
            lbit = 0;

        rawLen += lbit;

        tmps[i] = (byte*)XMALLOC(rawLen + MAX_SEQ_SZ, key->heap,
                                                      DYNAMIC_TYPE_USER_CRYPTO);
        if (tmps[i] == NULL) {
            ret = USER_CRYPTO_ERROR;
            break;
        }

        tmps[i][0] = ASN_INTEGER;
        sizes[i] = SetLength(rawLen, tmps[i] + 1) + 1 + lbit; /* tag & lbit */

        if (sizes[i] <= MAX_SEQ_SZ) {
            int err;

            /* leading zero */
            if (lbit)
                tmps[i][sizes[i]-1] = 0x00;

            /* extract data*/
            err = ippsGetOctString_BN((Ipp8u*)(tmps[i] + sizes[i]),
                    rawLen - lbit, keyInt);
            if (err == ippStsOk) {
                sizes[i] += (rawLen-lbit); /* lbit included in rawLen */
                intTotalLen += sizes[i];
                ret = 0;
            }
            else {
                ret = USER_CRYPTO_ERROR;
                USER_DEBUG(("ippsGetOctString_BN error %s\n",
                                                      ippGetStatusString(err)));
                break;
            }
        }
        else {
            ret = USER_CRYPTO_ERROR;
            break;
        }
    }

    if (ret != 0) {
        FreeTmpRsas(tmps, key->heap);
        return ret;
    }

    /* make headers */
    verSz = SetMyVersion(0, ver, FALSE);
    seqSz = SetSequence(verSz + intTotalLen, seq);

    outLen = seqSz + verSz + intTotalLen;
    if (output) {
        if (outLen > (int)inLen) {
            return USER_CRYPTO_ERROR;
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
    }
    FreeTmpRsas(tmps, key->heap);

    return outLen;
}


/* Convert Rsa Public key to DER format, write to output (inLen), return bytes
   written
*/
int wc_RsaKeyToPublicDer(RsaKey* key, byte* output, word32 inLen)
{
    return SetRsaPublicKey(output, key, inLen, 1);
}

/* Returns public DER version of the RSA key. If with_header is 0 then only a
 * seq + n + e is returned in ASN.1 DER format */
int wc_RsaKeyToPublicDer_ex(RsaKey* key, byte* output, word32 inLen,
    int with_header)
{
    return SetRsaPublicKey(output, key, inLen, with_header);
}

#endif /* WOLFSSL_KEY_GEN || OPENSSL_EXTRA */

#ifdef WC_RSA_BLINDING

int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng)
{
    if (key == NULL)
        return USER_CRYPTO_ERROR;

    (void)rng;

    return 0;
}

#endif /* WC_RSA_BLINDING */

#endif /* NO_RSA */

