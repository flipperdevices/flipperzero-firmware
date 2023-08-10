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

/*

DESCRIPTION
This library provides the interface to the RSA.
RSA keys can be used to encrypt, decrypt, sign and verify data.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifndef NO_RSA

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

       #ifdef USE_WINDOWS_API
               #pragma code_seg(".fipsA$e")
               #pragma const_seg(".fipsB$e")
       #endif
#endif

#include <wolfssl/wolfcrypt/rsa.h>

#ifdef WOLFSSL_AFALG_XILINX_RSA
#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>
#endif
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
#include <xsecure_rsaclient.h>
#endif
#ifdef WOLFSSL_SE050
#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif
#ifdef WOLFSSL_HAVE_SP_RSA
#include <wolfssl/wolfcrypt/sp.h>
#endif

/*
Possible RSA enable options:
 * NO_RSA:                Overall control of RSA                    default: on
 *                                                                 (not defined)
 * WC_RSA_BLINDING:       Uses Blinding w/ Private Ops              default: on
                          Note: slower by ~20%
 * WOLFSSL_KEY_GEN:       Allows Private Key Generation             default: off
 * RSA_LOW_MEM:           NON CRT Private Operations, less memory   default: off
 * WC_NO_RSA_OAEP:        Disables RSA OAEP padding                 default: on
 *                                                                 (not defined)
 * WC_RSA_NONBLOCK:       Enables support for RSA non-blocking      default: off
 * WC_RSA_NONBLOCK_TIME:  Enables support for time based blocking   default: off
 *                        time calculation.
 * WC_RSA_NO_FERMAT_CHECK:Don't check for small difference in       default: off
 *                        p and q (Fermat's factorization is       (not defined)
 *                        possible when small difference).
*/

/*
RSA Key Size Configuration:
 * FP_MAX_BITS:         With USE_FAST_MATH only                     default: 4096
    If USE_FAST_MATH then use this to override default.
    Value is key size * 2. Example: RSA 3072 = 6144
*/


#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


enum {
    RSA_STATE_NONE = 0,

    RSA_STATE_ENCRYPT_PAD,
    RSA_STATE_ENCRYPT_EXPTMOD,
    RSA_STATE_ENCRYPT_RES,

    RSA_STATE_DECRYPT_EXPTMOD,
    RSA_STATE_DECRYPT_UNPAD,
    RSA_STATE_DECRYPT_RES
};


static void wc_RsaCleanup(RsaKey* key)
{
#if !defined(WOLFSSL_RSA_VERIFY_INLINE) && !defined(WOLFSSL_NO_MALLOC)
    if (key && key->data) {
        /* make sure any allocated memory is free'd */
        if (key->dataIsAlloc) {
        #ifndef WOLFSSL_RSA_PUBLIC_ONLY
            if (key->type == RSA_PRIVATE_DECRYPT ||
                key->type == RSA_PRIVATE_ENCRYPT) {
                ForceZero(key->data, key->dataLen);
            }
        #endif
            XFREE(key->data, key->heap, DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 0;
        }
        key->data = NULL;
        key->dataLen = 0;
    }
#else
    (void)key;
#endif
}

int wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId)
{
    int ret      = 0;
#if defined(HAVE_PKCS11)
    int isPkcs11 = 0;
#endif

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(HAVE_PKCS11)
    if (key->isPkcs11) {
        isPkcs11 = 1;
    }
#endif

    XMEMSET(key, 0, sizeof(RsaKey));

    key->type = RSA_TYPE_UNKNOWN;
    key->state = RSA_STATE_NONE;
    key->heap = heap;
#if !defined(WOLFSSL_RSA_VERIFY_INLINE) && !defined(WOLFSSL_NO_MALLOC)
    key->dataIsAlloc = 0;
    key->data = NULL;
#endif
    key->dataLen = 0;
#ifdef WC_RSA_BLINDING
    key->rng = NULL;
#endif

#ifdef WOLF_CRYPTO_CB
    key->devId = devId;
#else
    (void)devId;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #ifdef WOLFSSL_CERT_GEN
        XMEMSET(&key->certSignCtx, 0, sizeof(CertSignCtx));
    #endif

    #ifdef WC_ASYNC_ENABLE_RSA
        #if defined(HAVE_PKCS11)
            if (!isPkcs11)
        #endif
            {
                /* handle as async */
                ret = wolfAsync_DevCtxInit(&key->asyncDev,
                        WOLFSSL_ASYNC_MARKER_RSA, key->heap, devId);
                if (ret != 0)
                    return ret;
            }
    #endif /* WC_ASYNC_ENABLE_RSA */
#elif defined(HAVE_PKCS11)
    (void)isPkcs11;
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    ret = mp_init_multi(&key->n, &key->e, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY)
        return ret;

#if !defined(WOLFSSL_KEY_GEN) && !defined(OPENSSL_EXTRA) && defined(RSA_LOW_MEM)
    ret = mp_init_multi(&key->d, &key->p, &key->q, NULL, NULL, NULL);
#else
    ret = mp_init_multi(&key->d, &key->p, &key->q, &key->dP, &key->dQ, &key->u);
#endif
    if (ret != MP_OKAY) {
        mp_clear(&key->n);
        mp_clear(&key->e);
        return ret;
    }
#else
    ret = mp_init(&key->n);
    if (ret != MP_OKAY)
        return ret;
    ret = mp_init(&key->e);
    if (ret != MP_OKAY) {
        mp_clear(&key->n);
        return ret;
    }
#endif

#ifdef WOLFSSL_XILINX_CRYPT
    key->pubExp = 0;
    key->mod    = NULL;
#endif

#ifdef WOLFSSL_AFALG_XILINX_RSA
    key->alFd = WC_SOCK_NOTSET;
    key->rdFd = WC_SOCK_NOTSET;
#endif

#ifdef WOLFSSL_KCAPI_RSA
    key->handle = NULL;
#endif

    return ret;
}

int wc_InitRsaKey(RsaKey* key, void* heap)
{
    return wc_InitRsaKey_ex(key, heap, INVALID_DEVID);
}

#ifdef WOLF_PRIVATE_KEY_ID
int wc_InitRsaKey_Id(RsaKey* key, unsigned char* id, int len, void* heap,
                     int devId)
{
    int ret = 0;
#ifdef WOLFSSL_SE050
    /* SE050 TLS users store a word32 at id, need to cast back */
    word32* keyPtr = NULL;
#endif

    if (key == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0 && (len < 0 || len > RSA_MAX_ID_LEN))
        ret = BUFFER_E;

#if defined(HAVE_PKCS11)
    XMEMSET(key, 0, sizeof(RsaKey));
    key->isPkcs11 = 1;
#endif

    if (ret == 0)
        ret = wc_InitRsaKey_ex(key, heap, devId);
    if (ret == 0 && id != NULL && len != 0) {
        XMEMCPY(key->id, id, (size_t)len);
        key->idLen = len;
    #ifdef WOLFSSL_SE050
        /* Set SE050 ID from word32, populate RsaKey with public from SE050 */
        if (len == (int)sizeof(word32)) {
            keyPtr = (word32*)key->id;
            ret = wc_RsaUseKeyId(key, *keyPtr, 0);
        }
    #endif
    }

    return ret;
}

int wc_InitRsaKey_Label(RsaKey* key, const char* label, void* heap, int devId)
{
    int ret = 0;
    int labelLen = 0;

    if (key == NULL || label == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0) {
        labelLen = (int)XSTRLEN(label);
        if (labelLen == 0 || labelLen > RSA_MAX_LABEL_LEN)
            ret = BUFFER_E;
    }

#if defined(HAVE_PKCS11)
    XMEMSET(key, 0, sizeof(RsaKey));
    key->isPkcs11 = 1;
#endif

    if (ret == 0)
        ret = wc_InitRsaKey_ex(key, heap, devId);
    if (ret == 0) {
        XMEMCPY(key->label, label, (size_t)labelLen);
        key->labelLen = labelLen;
    }

    return ret;
}
#endif /* WOLF_PRIVATE_KEY_ID */


#ifdef WOLFSSL_XILINX_CRYPT
#define MAX_E_SIZE 4
/* Used to setup hardware state
 *
 * key  the RSA key to setup
 *
 * returns 0 on success
 */
int wc_InitRsaHw(RsaKey* key)
{
    unsigned char* m; /* RSA modulus */
    word32 e = 0;     /* RSA public exponent */
    int mSz;
    int eSz;
    int ret;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    mSz = mp_unsigned_bin_size(&(key->n));
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    if (mSz > WOLFSSL_XSECURE_RSA_KEY_SIZE) {
        return BAD_FUNC_ARG;
    }
    /* Allocate 4 bytes more for the public exponent. */
    m = (unsigned char*) XMALLOC(WOLFSSL_XSECURE_RSA_KEY_SIZE + 4, key->heap,
                                 DYNAMIC_TYPE_KEY);
#else
    m = (unsigned char*)XMALLOC(mSz, key->heap, DYNAMIC_TYPE_KEY);
#endif
    if (m == NULL) {
        return MEMORY_E;
    }

    if (mp_to_unsigned_bin(&(key->n), m) != MP_OKAY) {
        WOLFSSL_MSG("Unable to get RSA key modulus");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return MP_READ_E;
    }
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    XMEMSET(m + mSz, 0, WOLFSSL_XSECURE_RSA_KEY_SIZE + 4 - mSz);
#endif

    eSz = mp_unsigned_bin_size(&(key->e));
    if (eSz > MAX_E_SIZE) {
        WOLFSSL_MSG("Exponent of size 4 bytes expected");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_FUNC_ARG;
    }

    if (mp_to_unsigned_bin(&(key->e), (byte*)&e + (MAX_E_SIZE - eSz))
                != MP_OKAY) {
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        WOLFSSL_MSG("Unable to get RSA key exponent");
        return MP_READ_E;
    }

    /* check for existing mod buffer to avoid memory leak */
    if (key->mod != NULL) {
        XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    }

    key->pubExp = e;
    key->mod    = m;

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    ret = wc_InitXsecure(&(key->xSec));
    if (ret != 0) {
        WOLFSSL_MSG("Unable to initialize xSecure for RSA");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return ret;
    }
    XMEMCPY(&m[WOLFSSL_XSECURE_RSA_KEY_SIZE], &e, sizeof(e));
    key->mSz = mSz;
#else
    if (XSecure_RsaInitialize(&(key->xRsa), key->mod, NULL,
                (byte*)&(key->pubExp)) != XST_SUCCESS) {
        WOLFSSL_MSG("Unable to initialize RSA on hardware");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_STATE_E;
    }

#ifdef WOLFSSL_XILINX_PATCH
   /* currently a patch of xsecure_rsa.c for 2048 bit keys */
   if (wc_RsaEncryptSize(key) == 256) {
       if (XSecure_RsaSetSize(&(key->xRsa), 2048) != XST_SUCCESS) {
           WOLFSSL_MSG("Unable to set RSA key size on hardware");
           XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
           return BAD_STATE_E;
       }
   }
#endif
#endif
    return 0;
} /* WOLFSSL_XILINX_CRYPT*/

#elif defined(WOLFSSL_CRYPTOCELL)

int wc_InitRsaHw(RsaKey* key)
{
    CRYSError_t ret = 0;
    byte e[3];
    word32 eSz = sizeof(e);
    byte n[256];
    word32 nSz = sizeof(n);
    byte d[256];
    word32 dSz = sizeof(d);
    byte p[128];
    word32 pSz = sizeof(p);
    byte q[128];
    word32 qSz = sizeof(q);

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wc_RsaExportKey(key, e, &eSz, n, &nSz, d, &dSz, p, &pSz, q, &qSz);
    if (ret != 0)
        return MP_READ_E;

    ret = CRYS_RSA_Build_PubKey(&key->ctx.pubKey, e, eSz, n, nSz);
    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Build_PubKey failed");
        return ret;
    }

    ret =  CRYS_RSA_Build_PrivKey(&key->ctx.privKey, d, dSz, e, eSz, n, nSz);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Build_PrivKey failed");
        return ret;
    }
    key->type = RSA_PRIVATE;
    return 0;
}

static int cc310_RSA_GenerateKeyPair(RsaKey* key, int size, long e)
{
    CRYSError_t             ret = 0;
    CRYS_RSAKGData_t        KeyGenData;
    CRYS_RSAKGFipsContext_t FipsCtx;
    byte ex[3];
    word16 eSz = sizeof(ex);
    byte n[256];
    word16 nSz = sizeof(n);

    ret = CRYS_RSA_KG_GenerateKeyPair(&wc_rndState,
                        wc_rndGenVectFunc,
                        (byte*)&e,
                        3*sizeof(byte),
                        size,
                        &key->ctx.privKey,
                        &key->ctx.pubKey,
                        &KeyGenData,
                        &FipsCtx);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_KG_GenerateKeyPair failed");
        return ret;
    }

    ret = CRYS_RSA_Get_PubKey(&key->ctx.pubKey, ex, &eSz, n, &nSz);
    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_Get_PubKey failed");
        return ret;
    }
    ret = wc_RsaPublicKeyDecodeRaw(n, nSz, ex, eSz, key);

    key->type = RSA_PRIVATE;

    return ret;
}
#endif /* WOLFSSL_CRYPTOCELL */

#ifdef WOLFSSL_SE050
/* Use specified hardware key ID with RsaKey operations. Unlike devId,
 * keyId is a word32 so can handle key IDs larger than an int.
 *
 * key    initialized RsaKey struct
 * keyId  hardware key ID which stores RSA key
 * flags  optional flags, currently unused
 *
 * Return 0 on success, negative on error */
int wc_RsaUseKeyId(RsaKey* key, word32 keyId, word32 flags)
{
    (void)flags;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    return se050_rsa_use_key_id(key, keyId);
}

/* Get hardware key ID associated with this RsaKey structure.
 *
 * key    initialized RsaKey struct
 * keyId  [OUT] output for key ID associated with this structure
 *
 * Returns 0 on success, negative on error.
 */
int wc_RsaGetKeyId(RsaKey* key, word32* keyId)
{
    if (key == NULL || keyId == NULL) {
        return BAD_FUNC_ARG;
    }

    return se050_rsa_get_key_id(key, keyId);
}
#endif /* WOLFSSL_SE050 */

int wc_FreeRsaKey(RsaKey* key)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    wc_RsaCleanup(key);

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    wolfAsync_DevCtxFree(&key->asyncDev, WOLFSSL_ASYNC_MARKER_RSA);
#endif

#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    if (key->type == RSA_PRIVATE) {
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
        mp_forcezero(&key->u);
        mp_forcezero(&key->dQ);
        mp_forcezero(&key->dP);
#endif
        mp_forcezero(&key->q);
        mp_forcezero(&key->p);
        mp_forcezero(&key->d);
    }
    else {
        /* private part */
#if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
        mp_clear(&key->u);
        mp_clear(&key->dQ);
        mp_clear(&key->dP);
#endif
        mp_clear(&key->q);
        mp_clear(&key->p);
        mp_clear(&key->d);
    }
#endif /* WOLFSSL_RSA_PUBLIC_ONLY */

    /* public part */
    mp_clear(&key->e);
    mp_clear(&key->n);

#ifdef WOLFSSL_XILINX_CRYPT
    XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    key->mod = NULL;
#endif

#ifdef WOLFSSL_AFALG_XILINX_RSA
    /* make sure that sockets are closed on cleanup */
    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }
#endif

#ifdef WOLFSSL_KCAPI_RSA
    KcapiRsa_Free(key);
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(key, sizeof(RsaKey));
#endif

    return ret;
}

#ifdef WOLFSSL_RSA_KEY_CHECK
/* Check the pair-wise consistency of the RSA key. */
static int _ifc_pairwise_consistency_test(RsaKey* key, WC_RNG* rng)
{
    static const char* msg = "Everyone gets Friday off.";
    byte* sig;
    byte* plain;
    int ret = 0;
    word32 msgLen, plainLen, sigLen;

    msgLen = (word32)XSTRLEN(msg);
    ret = wc_RsaEncryptSize(key);
    if (ret < 0)
        return ret;
    sigLen = (word32)ret;

    WOLFSSL_MSG("Doing RSA consistency test");

    /* Sign and verify. */
    sig = (byte*)XMALLOC(sigLen, key->heap, DYNAMIC_TYPE_RSA);
    if (sig == NULL) {
        return MEMORY_E;
    }
    XMEMSET(sig, 0, sigLen);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("Pairwise CT sig", sig, sigLen);
#endif
    plain = sig;

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Do blocking async calls here, caller does not support WC_PENDING_E */
    do {
        if (ret == WC_PENDING_E)
            ret = wc_AsyncWait(ret, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
        if (ret >= 0)
#endif
            ret = wc_RsaSSL_Sign((const byte*)msg, msgLen, sig, sigLen, key, rng);
#ifdef WOLFSSL_ASYNC_CRYPT
    } while (ret == WC_PENDING_E);
#endif

    if (ret > 0) {
        sigLen = (word32)ret;
#ifdef WOLFSSL_ASYNC_CRYPT
        /* Do blocking async calls here, caller does not support WC_PENDING_E */
        do {
            if (ret == WC_PENDING_E)
                ret = wc_AsyncWait(ret, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret >= 0)
#endif
                ret = wc_RsaSSL_VerifyInline(sig, sigLen, &plain, key);
#ifdef WOLFSSL_ASYNC_CRYPT
        } while (ret == WC_PENDING_E);
#endif
    }

    if (ret > 0) {
        plainLen = (word32)ret;
        ret = (msgLen != plainLen) || (XMEMCMP(plain, msg, msgLen) != 0);
    }

    if (ret != 0)
        ret = RSA_KEY_PAIR_E;

    ForceZero(sig, sigLen);
    XFREE(sig, key->heap, DYNAMIC_TYPE_RSA);

    return ret;
}


int wc_CheckRsaKey(RsaKey* key)
{
    DECL_MP_INT_SIZE_DYN(tmp, mp_bitsused(&key->n), RSA_MAX_SIZE);
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG *rng = NULL;
#else
    WC_RNG rng[1];
#endif
    int ret = 0;

#ifdef WOLFSSL_CAAM
    /* can not perform these checks on an encrypted key */
    if (key->blackKey != 0) {
        return 0;
    }
#endif

#ifdef WOLFSSL_SMALL_STACK
    rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (rng == NULL) {
        return MEMORY_E;
    }
#endif
    NEW_MP_INT_SIZE(tmp, mp_bitsused(&key->n), NULL, DYNAMIC_TYPE_RSA);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (tmp == NULL) {
        XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
        return MEMORY_E;
    }
#endif

    ret = wc_InitRng(rng);

    if (ret == 0)
        SAVE_VECTOR_REGISTERS(ret = _svr_ret;);

    if (ret == 0) {
        if (INIT_MP_INT_SIZE(tmp, mp_bitsused(&key->n)) != MP_OKAY)
            ret = MP_INIT_E;
    }

    if (ret == 0) {
        if (key == NULL)
            ret = BAD_FUNC_ARG;
    }

    if (ret == 0)
        ret = _ifc_pairwise_consistency_test(key, rng);

    /* Check d is less than n. */
    if (ret == 0 ) {
        if (mp_cmp(&key->d, &key->n) != MP_LT) {
            ret = MP_EXPTMOD_E;
        }
    }
    /* Check p*q = n. */
    if (ret == 0 ) {
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        mp_memzero_add("RSA CheckKey tmp", tmp);
    #endif
        if (mp_mul(&key->p, &key->q, tmp) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
    }
    if (ret == 0 ) {
        if (mp_cmp(&key->n, tmp) != MP_EQ) {
            ret = MP_EXPTMOD_E;
        }
    }

#ifndef WC_RSA_NO_FERMAT_CHECK
    /* Fermat's Factorization works when difference between p and q
     * is less than (conservatively):
     *     n^(1/4) + 32
     *  ~= 2^(bit count of n)^(1/4) + 32) = 2^((bit count of n)/4 + 32)
     */
    if (ret == 0) {
        ret = mp_sub(&key->p, &key->q, tmp);
    }
    if (ret == 0) {
        if (mp_count_bits(tmp) <= (mp_count_bits(&key->n) / 4 + 32)) {
            ret = MP_EXPTMOD_E;
        }
    }
#endif

    /* Check dP, dQ and u if they exist */
    if (ret == 0 && !mp_iszero(&key->dP)) {
        if (mp_sub_d(&key->p, 1, tmp) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
        /* Check dP <= p-1. */
        if (ret == 0) {
            if (mp_cmp(&key->dP, tmp) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check e*dP mod p-1 = 1. (dP = 1/e mod p-1) */
        if (ret == 0) {
            if (mp_mulmod(&key->dP, &key->e, tmp, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }

        if (ret == 0) {
            if (mp_sub_d(&key->q, 1, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check dQ <= q-1. */
        if (ret == 0) {
            if (mp_cmp(&key->dQ, tmp) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check e*dP mod p-1 = 1. (dQ = 1/e mod q-1) */
        if (ret == 0) {
            if (mp_mulmod(&key->dQ, &key->e, tmp, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }

        /* Check u <= p. */
        if (ret == 0) {
            if (mp_cmp(&key->u, &key->p) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check u*q mod p = 1. (u = 1/q mod p) */
        if (ret == 0) {
            if (mp_mulmod(&key->u, &key->q, &key->p, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }
    }

    mp_forcezero(tmp);

    RESTORE_VECTOR_REGISTERS();

    wc_FreeRng(rng);
    FREE_MP_INT_SIZE(tmp, NULL, DYNAMIC_TYPE_RSA);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    mp_memzero_check(tmp);
#endif

    return ret;
}
#endif /* WOLFSSL_RSA_KEY_CHECK */


#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_PSS)
/* Uses MGF1 standard as a mask generation function
   hType: hash type used
   seed:  seed to use for generating mask
   seedSz: size of seed buffer
   out:   mask output after generation
   outSz: size of output buffer
 */
#if !defined(NO_SHA) || !defined(NO_SHA256) || defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
static int RsaMGF1(enum wc_HashType hType, byte* seed, word32 seedSz,
                                        byte* out, word32 outSz, void* heap)
{
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    byte* tmp = NULL;
    byte   tmpF = 0;     /* 1 if dynamic memory needs freed */
#else
    byte tmp[RSA_MAX_SIZE/8];
#endif
    /* needs to be large enough for seed size plus counter(4) */
    byte  tmpA[WC_MAX_DIGEST_SIZE + 4];
    word32 tmpSz = 0;
    int hLen;
    int ret;
    word32 counter;
    word32 idx;
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_HashAlg *hash;
#endif
    hLen    = wc_HashGetDigestSize(hType);
    counter = 0;
    idx     = 0;

    (void)heap;

    XMEMSET(tmpA, 0, sizeof(tmpA));
    /* check error return of wc_HashGetDigestSize */
    if (hLen < 0) {
        return hLen;
    }

    /* if tmp is not large enough than use some dynamic memory */
    if ((seedSz + 4) > sizeof(tmpA) || (word32)hLen > sizeof(tmpA)) {
        /* find largest amount of memory needed which will be the max of
         * hLen and (seedSz + 4) since tmp is used to store the hash digest */
        tmpSz = ((seedSz + 4) > (word32)hLen)? seedSz + 4: (word32)hLen;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        tmp = (byte*)XMALLOC(tmpSz, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (tmp == NULL) {
            return MEMORY_E;
        }
        tmpF = 1; /* make sure to free memory when done */
#else
        if (tmpSz > RSA_MAX_SIZE/8)
            return BAD_FUNC_ARG;
#endif
    }
    else {
        /* use array on the stack */
    #ifndef WOLFSSL_SMALL_STACK_CACHE
        tmpSz = sizeof(tmpA);
    #endif
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        tmp  = tmpA;
        tmpF = 0; /* no need to free memory at end */
#endif
    }

#ifdef WOLFSSL_SMALL_STACK_CACHE
    hash = (wc_HashAlg*)XMALLOC(sizeof(*hash), heap, DYNAMIC_TYPE_DIGEST);
    if (hash == NULL) {
    #if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        if (tmpF) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        }
    #endif
        return MEMORY_E;
    }
    ret = wc_HashInit_ex(hash, hType, heap, INVALID_DEVID);
    if (ret != 0) {
        XFREE(hash, heap, DYNAMIC_TYPE_DIGEST);
    #if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        if (tmpF) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        }
    #endif
        return ret;
    }
#endif

    do {
        int i = 0;
        XMEMCPY(tmp, seed, seedSz);

        /* counter to byte array appended to tmp */
        tmp[seedSz]     = (byte)((counter >> 24) & 0xFF);
        tmp[seedSz + 1] = (byte)((counter >> 16) & 0xFF);
        tmp[seedSz + 2] = (byte)((counter >>  8) & 0xFF);
        tmp[seedSz + 3] = (byte)((counter)       & 0xFF);

        /* hash and append to existing output */
#ifdef WOLFSSL_SMALL_STACK_CACHE
        ret = wc_HashUpdate(hash, hType, tmp, (seedSz + 4));
        if (ret == 0) {
            ret = wc_HashFinal(hash, hType, tmp);
        }
#else
        ret = wc_Hash(hType, tmp, (seedSz + 4), tmp, tmpSz);
#endif
        if (ret != 0) {
            /* check for if dynamic memory was needed, then free */
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
            if (tmpF) {
                XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            }
#endif
            return ret;
        }

        for (i = 0; i < hLen && idx < outSz; i++) {
            out[idx++] = tmp[i];
        }
        counter++;
    } while (idx < outSz);
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    /* check for if dynamic memory was needed, then free */
    if (tmpF) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
    }
#endif
#ifdef WOLFSSL_SMALL_STACK_CACHE
    wc_HashFree(hash, hType);
    XFREE(hash, heap, DYNAMIC_TYPE_DIGEST);
#endif

    return 0;
}
#endif /* SHA2 Hashes */

/* helper function to direct which mask generation function is used
   switched on type input
 */
static int RsaMGF(int type, byte* seed, word32 seedSz, byte* out,
                                                    word32 outSz, void* heap)
{
    int ret;

    switch(type) {
    #ifndef NO_SHA
        case WC_MGF1SHA1:
            ret = RsaMGF1(WC_HASH_TYPE_SHA, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifndef NO_SHA256
    #ifdef WOLFSSL_SHA224
        case WC_MGF1SHA224:
            ret = RsaMGF1(WC_HASH_TYPE_SHA224, seed, seedSz, out, outSz, heap);
            break;
    #endif
        case WC_MGF1SHA256:
            ret = RsaMGF1(WC_HASH_TYPE_SHA256, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case WC_MGF1SHA384:
            ret = RsaMGF1(WC_HASH_TYPE_SHA384, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case WC_MGF1SHA512:
            ret = RsaMGF1(WC_HASH_TYPE_SHA512, seed, seedSz, out, outSz, heap);
            break;
        #ifndef WOLFSSL_NOSHA512_224
        case WC_MGF1SHA512_224:
            ret = RsaMGF1(WC_HASH_TYPE_SHA512_224, seed, seedSz, out, outSz,
                heap);
            break;
        #endif
        #ifndef WOLFSSL_NOSHA512_256
        case WC_MGF1SHA512_256:
            ret = RsaMGF1(WC_HASH_TYPE_SHA512_256, seed, seedSz, out, outSz,
                heap);
            break;
        #endif
    #endif
        default:
            WOLFSSL_MSG("Unknown MGF type: check build options");
            ret = BAD_FUNC_ARG;
    }

    /* in case of default avoid unused warning */
    (void)seed;
    (void)seedSz;
    (void)out;
    (void)outSz;
    (void)heap;

    return ret;
}
#endif /* !WC_NO_RSA_OAEP || WC_RSA_PSS */


/* Padding */
#ifndef WOLFSSL_RSA_VERIFY_ONLY
#ifndef WC_NO_RNG
#ifndef WC_NO_RSA_OAEP
static int RsaPad_OAEP(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, byte padValue, WC_RNG* rng,
        enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
        void* heap)
{
    int ret;
    word32 hLen;
    int psLen;
    word32 idx;

    #if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        byte* dbMask = NULL;
        byte* lHash = NULL;
        byte* seed  = NULL;
    #else
        byte dbMask[RSA_MAX_SIZE/8 + RSA_PSS_PAD_SZ];
        /* must be large enough to contain largest hash */
        byte lHash[WC_MAX_DIGEST_SIZE];
        byte seed[WC_MAX_DIGEST_SIZE];
    #endif

    /* no label is allowed, but catch if no label provided and length > 0 */
    if (optLabel == NULL && labelLen > 0) {
        return BUFFER_E;
    }

    /* limit of label is the same as limit of hash function which is massive */
    ret = wc_HashGetDigestSize(hType);
    if (ret < 0) {
        return ret;
    }
    hLen = (word32)ret;

    #if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
        lHash = (byte*)XMALLOC(hLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (lHash == NULL) {
            return MEMORY_E;
        }
        seed = (byte*)XMALLOC(hLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (seed == NULL) {
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            return MEMORY_E;
        }
    #else
        /* hLen should never be larger than lHash since size is max digest size,
           but check before blindly calling wc_Hash */
        if (hLen > sizeof(lHash)) {
            WOLFSSL_MSG("OAEP lHash to small for digest!!");
            return MEMORY_E;
        }
    #endif

    if ((ret = wc_Hash(hType, optLabel, labelLen, lHash, hLen)) != 0) {
        WOLFSSL_MSG("OAEP hash type possibly not supported or lHash to small");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    /* handles check of location for idx as well as psLen, cast to int to check
       for pkcsBlockLen(k) - 2 * hLen - 2 being negative
       This check is similar to decryption where k > 2 * hLen + 2 as msg
       size approaches 0. In decryption if k is less than or equal -- then there
       is no possible room for msg.
       k = RSA key size
       hLen = hash digest size -- will always be >= 0 at this point
     */
    if ((2 * hLen + 2) > pkcsBlockLen) {
        WOLFSSL_MSG("OAEP pad error hash to big for RSA key size");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BAD_FUNC_ARG;
    }

    if (inputLen > (pkcsBlockLen - 2 * hLen - 2)) {
        WOLFSSL_MSG("OAEP pad error message too long");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BAD_FUNC_ARG;
    }

    /* concatenate lHash || PS || 0x01 || msg */
    idx = pkcsBlockLen - 1 - inputLen;
    psLen = (int)pkcsBlockLen - (int)inputLen - 2 * (int)hLen - 2;
    if (pkcsBlockLen < inputLen) { /*make sure not writing over end of buffer */
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BUFFER_E;
    }
    XMEMCPY(pkcsBlock + (pkcsBlockLen - inputLen), input, inputLen);
    pkcsBlock[idx--] = 0x01; /* PS and M separator */
    XMEMSET(pkcsBlock + idx - psLen + 1, 0, (size_t)psLen);
    idx -= (word32)psLen;

    idx = idx - hLen + 1;
    XMEMCPY(pkcsBlock + idx, lHash, hLen);

    /* generate random seed */
    if ((ret = wc_RNG_GenerateBlock(rng, seed, hLen)) != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    /* create maskedDB from dbMask */
    dbMask = (byte*)XMALLOC(pkcsBlockLen - hLen - 1, heap, DYNAMIC_TYPE_RSA);
    if (dbMask == NULL) {

            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        return MEMORY_E;
    }
#else
    if (pkcsBlockLen - hLen - 1 > sizeof(dbMask)) {
        return MEMORY_E;
    }
#endif
    XMEMSET(dbMask, 0, pkcsBlockLen - hLen - 1); /* help static analyzer */
    ret = RsaMGF(mgf, seed, hLen, dbMask, pkcsBlockLen - hLen - 1, heap);
    if (ret != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(dbMask, heap, DYNAMIC_TYPE_RSA);
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    xorbuf(pkcsBlock + hLen + 1, dbMask,pkcsBlockLen - hLen - 1);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(dbMask, heap, DYNAMIC_TYPE_RSA);
#endif

    /* create maskedSeed from seedMask */
    pkcsBlock[0] = 0x00;
    /* create seedMask inline */
    if ((ret = RsaMGF(mgf, pkcsBlock + hLen + 1, pkcsBlockLen - hLen - 1,
                                           pkcsBlock + 1, hLen, heap)) != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    /* xor created seedMask with seed to make maskedSeed */
    xorbuf(pkcsBlock + 1, seed, hLen);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    /* Seed must be zeroized now that it has been used. */
    wc_MemZero_Add("Pad OAEP seed", seed, hLen);
#endif

    /* Zeroize masking bytes so that padding can't be unmasked. */
    ForceZero(seed, hLen);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
        XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
    #elif defined(WOLFSSL_CHECK_MEM_ZERO)
        wc_MemZero_Check(seed, hLen);
    #endif
    (void)padValue;

    return 0;
}
#endif /* !WC_NO_RSA_OAEP */

#ifdef WC_RSA_PSS

/* 0x00 .. 0x00 0x01 | Salt | Gen Hash | 0xbc
 * XOR MGF over all bytes down to end of Salt
 * Gen Hash = HASH(8 * 0x00 | Message Hash | Salt)
 *
 * input         Digest of the message.
 * inputLen      Length of digest.
 * pkcsBlock     Buffer to write to.
 * pkcsBlockLen  Length of buffer to write to.
 * rng           Random number generator (for salt).
 * htype         Hash function to use.
 * mgf           Mask generation function.
 * saltLen       Length of salt to put in padding.
 * bits          Length of key in bits.
 * heap          Used for dynamic memory allocation.
 * returns 0 on success, PSS_SALTLEN_E when the salt length is invalid
 * and other negative values on error.
 */
static int RsaPad_PSS(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, WC_RNG* rng, enum wc_HashType hType, int mgf,
        int saltLen, int bits, void* heap)
{
    int   ret = 0;
    int   hLen, o, maskLen, hiBits;
    byte* m;
    byte* s;
#if defined(WOLFSSL_NO_MALLOC) && !defined(WOLFSSL_STATIC_MEMORY)
    byte msg[RSA_MAX_SIZE/8 + RSA_PSS_PAD_SZ];
#else
    byte* msg = NULL;
#endif
#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    byte* salt;
#else
    byte salt[WC_MAX_DIGEST_SIZE];
#endif

#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    if (pkcsBlockLen > RSA_MAX_SIZE/8) {
        return MEMORY_E;
    }
#endif

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;
    if ((int)inputLen != hLen) {
        return BAD_FUNC_ARG;
    }

    hiBits = (bits - 1) & 0x7;
    if (hiBits == 0) {
        /* Per RFC8017, set the leftmost 8emLen - emBits bits of the
           leftmost octet in DB to zero.
        */
        *(pkcsBlock++) = 0;
        pkcsBlockLen--;
    }

    if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
        saltLen = hLen;
        #ifdef WOLFSSL_SHA512
            /* See FIPS 186-4 section 5.5 item (e). */
            if (bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE) {
                saltLen = RSA_PSS_SALT_MAX_SZ;
            }
        #endif
    }
#ifndef WOLFSSL_PSS_LONG_SALT
    else if (saltLen > hLen) {
        return PSS_SALTLEN_E;
    }
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT) {
        return PSS_SALTLEN_E;
    }
#else
    else if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
        saltLen = (int)pkcsBlockLen - hLen - 2;
        if (saltLen < 0) {
            return PSS_SALTLEN_E;
        }
    }
    else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER) {
        return PSS_SALTLEN_E;
    }
#endif
    if ((int)pkcsBlockLen - hLen < saltLen + 2) {
        return PSS_SALTLEN_E;
    }
    maskLen = (int)pkcsBlockLen - 1 - hLen;

#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        msg = (byte*)XMALLOC(
                          (size_t)(RSA_PSS_PAD_SZ + inputLen + (word32)saltLen),
                          heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (msg == NULL) {
            return MEMORY_E;
        }
    #endif
    salt = s = m = msg;
    XMEMSET(m, 0, RSA_PSS_PAD_SZ);
    m += RSA_PSS_PAD_SZ;
    XMEMCPY(m, input, inputLen);
    m += inputLen;
    o = (int)(m - s);
    if (saltLen > 0) {
        ret = wc_RNG_GenerateBlock(rng, m, (word32)saltLen);
        if (ret == 0) {
            m += saltLen;
        }
    }
#else
    if ((int)pkcsBlockLen < RSA_PSS_PAD_SZ + (int)inputLen + saltLen) {
    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        msg = (byte*)XMALLOC(
                          (size_t)(RSA_PSS_PAD_SZ + inputLen + (word32)saltLen),
                          heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (msg == NULL) {
            return MEMORY_E;
        }
    #endif
        m = msg;
    }
    else {
        m = pkcsBlock;
    }
    s = m;
    XMEMSET(m, 0, RSA_PSS_PAD_SZ);
    m += RSA_PSS_PAD_SZ;
    XMEMCPY(m, input, inputLen);
    m += inputLen;
    o = 0;
    if (saltLen > 0) {
        ret = wc_RNG_GenerateBlock(rng, salt, (word32)saltLen);
        if (ret == 0) {
            XMEMCPY(m, salt, (size_t)saltLen);
            m += saltLen;
        }
    }
#endif
    if (ret == 0) {
        /* Put Hash at end of pkcsBlock - 1 */
        ret = wc_Hash(hType, s, (word32)(m - s), pkcsBlock + maskLen, (word32)hLen);
    }
    if (ret == 0) {
       /* Set the last eight bits or trailer field to the octet 0xbc */
        pkcsBlock[pkcsBlockLen - 1] = RSA_PSS_PAD_TERM;

        ret = RsaMGF(mgf, pkcsBlock + maskLen, (word32)hLen, pkcsBlock, (word32)maskLen, heap);
    }
    if (ret == 0) {
        /* Clear the first high bit when "8emLen - emBits" is non-zero.
           where emBits = n modBits - 1 */
        if (hiBits)
            pkcsBlock[0] &= (byte)((1 << hiBits) - 1);

        m = pkcsBlock + maskLen - saltLen - 1;
        *(m++) ^= 0x01;
        xorbuf(m, salt + o, (word32)saltLen);
    }

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
    /* msg is always not NULL as we bail on allocation failure */
    XFREE(msg, heap, DYNAMIC_TYPE_RSA_BUFFER);
#endif
    return ret;
}
#endif /* WC_RSA_PSS */
#endif /* !WC_NO_RNG */

static int RsaPad(const byte* input, word32 inputLen, byte* pkcsBlock,
                           word32 pkcsBlockLen, byte padValue, WC_RNG* rng)
{
    if (input == NULL || inputLen == 0 || pkcsBlock == NULL ||
                                                        pkcsBlockLen == 0) {
        return BAD_FUNC_ARG;
    }

    if (pkcsBlockLen - RSA_MIN_PAD_SZ < inputLen) {
        WOLFSSL_MSG("RsaPad error, invalid length");
        return RSA_PAD_E;
    }
    pkcsBlock[0] = 0x0;       /* set first byte to zero and advance */
    pkcsBlock++; pkcsBlockLen--;
    pkcsBlock[0] = padValue;  /* insert padValue */

    if (padValue == RSA_BLOCK_TYPE_1) {

        /* pad with 0xff bytes */
        XMEMSET(&pkcsBlock[1], 0xFF, pkcsBlockLen - inputLen - 2);
    }
    else {
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WC_NO_RNG)
        /* pad with non-zero random bytes */
        word32 padLen, i;
        int    ret;
        padLen = pkcsBlockLen - inputLen - 1;
        ret    = wc_RNG_GenerateBlock(rng, &pkcsBlock[1], padLen);
        if (ret != 0) {
            return ret;
        }

        /* remove zeros */
        for (i = 1; i < padLen; i++) {
            if (pkcsBlock[i] == 0) pkcsBlock[i] = 0x01;
        }
#else
        (void)rng;
        return RSA_WRONG_TYPE_E;
#endif
    }

    pkcsBlock[pkcsBlockLen-inputLen-1] = 0;     /* separator */
    XMEMCPY(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);

    return 0;
}

/* helper function to direct which padding is used */
int wc_RsaPad_ex(const byte* input, word32 inputLen, byte* pkcsBlock,
    word32 pkcsBlockLen, byte padValue, WC_RNG* rng, int padType,
    enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
    int saltLen, int bits, void* heap)
{
    int ret;

    switch (padType)
    {
        case WC_RSA_PKCSV15_PAD:
            /*WOLFSSL_MSG("wolfSSL Using RSA PKCSV15 padding");*/
            ret = RsaPad(input, inputLen, pkcsBlock, pkcsBlockLen,
                                                                 padValue, rng);
            break;

#ifndef WC_NO_RNG
    #ifndef WC_NO_RSA_OAEP
        case WC_RSA_OAEP_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA OAEP padding");
            ret = RsaPad_OAEP(input, inputLen, pkcsBlock, pkcsBlockLen,
                           padValue, rng, hType, mgf, optLabel, labelLen, heap);
            break;
    #endif

    #ifdef WC_RSA_PSS
        case WC_RSA_PSS_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA PSS padding");
            ret = RsaPad_PSS(input, inputLen, pkcsBlock, pkcsBlockLen, rng,
                                               hType, mgf, saltLen, bits, heap);
            break;
    #endif
#endif /* !WC_NO_RNG */

    #ifdef WC_RSA_NO_PADDING
        case WC_RSA_NO_PAD:
        {
            int bytes = (bits + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;

            WOLFSSL_MSG("wolfSSL Using NO padding");

            /* In the case of no padding being used check that input is exactly
             * the RSA key length */
            if ((bits <= 0) || (inputLen != (word32)bytes)) {
                WOLFSSL_MSG("Bad input size");
                ret = RSA_PAD_E;
            }
            else {
                XMEMCPY(pkcsBlock, input, inputLen);
                ret = 0;
            }
            break;
        }
    #endif

        default:
            WOLFSSL_MSG("Unknown RSA Pad Type");
            ret = RSA_PAD_E;
    }

    /* silence warning if not used with padding scheme */
    (void)input;
    (void)inputLen;
    (void)pkcsBlock;
    (void)pkcsBlockLen;
    (void)padValue;
    (void)rng;
    (void)padType;
    (void)hType;
    (void)mgf;
    (void)optLabel;
    (void)labelLen;
    (void)saltLen;
    (void)bits;
    (void)heap;

    return ret;
}
#endif /* WOLFSSL_RSA_VERIFY_ONLY */


/* UnPadding */
#if !defined(WC_NO_RSA_OAEP) && !defined(NO_HASH_WRAPPER)
/* UnPad plaintext, set start to *output, return length of plaintext,
 * < 0 on error */
static int RsaUnPad_OAEP(byte *pkcsBlock, unsigned int pkcsBlockLen,
                            byte **output, enum wc_HashType hType, int mgf,
                            byte* optLabel, word32 labelLen, void* heap)
{
    word32 hLen;
    int ret;
    byte h[WC_MAX_DIGEST_SIZE]; /* max digest size */
    word32 idx;
    word32 i;
    word32 inc;

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    byte* tmp  = NULL;
#else
    byte tmp[RSA_MAX_SIZE/8 + RSA_PSS_PAD_SZ];
#endif

    /* no label is allowed, but catch if no label provided and length > 0 */
    if (optLabel == NULL && labelLen > 0) {
        return BUFFER_E;
    }

    ret = wc_HashGetDigestSize(hType);
    if ((ret < 0) || (pkcsBlockLen < (2 * (word32)ret + 2))) {
        return BAD_FUNC_ARG;
    }
    hLen = (word32)ret;

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    tmp = (byte*)XMALLOC(pkcsBlockLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL) {
        return MEMORY_E;
    }
#endif
    XMEMSET(tmp, 0, pkcsBlockLen);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("OAEP UnPad temp", tmp, pkcsBlockLen);
#endif

    /* find seedMask value */
    if ((ret = RsaMGF(mgf, (byte*)(pkcsBlock + (hLen + 1)),
                            pkcsBlockLen - hLen - 1, tmp, hLen, heap)) != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
#endif
        return ret;
    }

    /* xor seedMask value with maskedSeed to get seed value */
    xorbuf(tmp, pkcsBlock + 1, hLen);

    /* get dbMask value */
    if ((ret = RsaMGF(mgf, tmp, hLen, tmp + hLen,
                                       pkcsBlockLen - hLen - 1, heap)) != 0) {
        ForceZero(tmp, hLen);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmp, NULL, DYNAMIC_TYPE_RSA_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
        wc_MemZero_Check(tmp, hLen);
#endif
        return ret;
    }

    /* get DB value by doing maskedDB xor dbMask */
    xorbuf(pkcsBlock + hLen + 1, tmp + hLen, pkcsBlockLen - hLen - 1);

    ForceZero(tmp, pkcsBlockLen);
#ifdef WOLFSSL_SMALL_STACK
    /* done with use of tmp buffer */
    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(tmp, pkcsBlockLen);
#endif

    /* advance idx to index of PS and msg separator, account for PS size of 0*/
    idx = hLen + 1 + hLen;
    /* Don't reveal length of message: look at every byte. */
    inc = 1;
    for (i = hLen + 1 + hLen; i < pkcsBlockLen - 1; i++) {
        /* Looking for non-zero byte. */
        inc &= 1 - (((word32)0 - pkcsBlock[i]) >> 31);
        idx += inc;
    }

    /* create hash of label for comparison with hash sent */
    if ((ret = wc_Hash(hType, optLabel, labelLen, h, hLen)) != 0) {
        return ret;
    }

    /* say no to chosen ciphertext attack.
       Comparison of lHash, Y, and separator value needs to all happen in
       constant time.
       Attackers should not be able to get error condition from the timing of
       these checks.
     */
    ret = 0;
    ret |= ConstantCompare(pkcsBlock + hLen + 1, h, (int)hLen);
    ret += pkcsBlock[idx++] ^ 0x01; /* separator value is 0x01 */
    ret += pkcsBlock[0]     ^ 0x00; /* Y, the first value, should be 0 */

    /* Return 0 data length on error. */
    idx = ctMaskSelWord32(ctMaskEq(ret, 0), idx, pkcsBlockLen);

    /* adjust pointer to correct location in array and return size of M */
    *output = (byte*)(pkcsBlock + idx);
    return (int)(pkcsBlockLen - idx);
}
#endif /* !WC_NO_RSA_OAEP */

#ifdef WC_RSA_PSS
/* 0x00 .. 0x00 0x01 | Salt | Gen Hash | 0xbc
 * MGF over all bytes down to end of Salt
 *
 * pkcsBlock     Buffer holding decrypted data.
 * pkcsBlockLen  Length of buffer.
 * htype         Hash function to use.
 * mgf           Mask generation function.
 * saltLen       Length of salt to put in padding.
 * bits          Length of key in bits.
 * heap          Used for dynamic memory allocation.
 * returns       the sum of salt length and SHA-256 digest size on success.
 *               Otherwise, PSS_SALTLEN_E for an incorrect salt length,
 *               WC_KEY_SIZE_E for an incorrect encoded message (EM) size
                 and other negative values on error.
 */
static int RsaUnPad_PSS(byte *pkcsBlock, unsigned int pkcsBlockLen,
                        byte **output, enum wc_HashType hType, int mgf,
                        int saltLen, int bits, void* heap)
{
    int   ret;
    byte* tmp;
    int   hLen, i, maskLen;
#ifdef WOLFSSL_SHA512
    int orig_bits = bits;
#endif
#if defined(WOLFSSL_NO_MALLOC) && !defined(WOLFSSL_STATIC_MEMORY)
    byte tmp_buf[RSA_MAX_SIZE/8];
    tmp = tmp_buf;

    if (pkcsBlockLen > RSA_MAX_SIZE/8) {
        return MEMORY_E;
    }
#endif

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;
    bits = (bits - 1) & 0x7;
    if ((pkcsBlock[0] & (0xff << bits)) != 0) {
        return BAD_PADDING_E;
    }
    if (bits == 0) {
        pkcsBlock++;
        pkcsBlockLen--;
    }
    maskLen = (int)pkcsBlockLen - 1 - hLen;
    if (maskLen < 0) {
        WOLFSSL_MSG("RsaUnPad_PSS: Hash too large");
        return WC_KEY_SIZE_E;
    }

    if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
        saltLen = hLen;
        #ifdef WOLFSSL_SHA512
            /* See FIPS 186-4 section 5.5 item (e). */
            if (orig_bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE)
                saltLen = RSA_PSS_SALT_MAX_SZ;
        #endif
    }
#ifndef WOLFSSL_PSS_LONG_SALT
    else if (saltLen > hLen)
        return PSS_SALTLEN_E;
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT)
        return PSS_SALTLEN_E;
    if (maskLen < saltLen + 1) {
        return PSS_SALTLEN_E;
    }
#else
    else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER)
        return PSS_SALTLEN_E;
    if (saltLen != RSA_PSS_SALT_LEN_DISCOVER && maskLen < saltLen + 1) {
        return WC_KEY_SIZE_E;
    }
#endif

    if (pkcsBlock[pkcsBlockLen - 1] != RSA_PSS_PAD_TERM) {
        WOLFSSL_MSG("RsaUnPad_PSS: Padding Term Error");
        return BAD_PADDING_E;
    }

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
    tmp = (byte*)XMALLOC((size_t)maskLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL) {
        return MEMORY_E;
    }
#endif

    if ((ret = RsaMGF(mgf, pkcsBlock + maskLen, (word32)hLen, tmp, (word32)maskLen,
                                                                  heap)) != 0) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    tmp[0] &= (byte)((1 << bits) - 1);
    pkcsBlock[0] &= (byte)((1 << bits) - 1);
#ifdef WOLFSSL_PSS_SALT_LEN_DISCOVER
    if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
        for (i = 0; i < maskLen - 1; i++) {
            if (tmp[i] != pkcsBlock[i]) {
                break;
            }
        }
        if (tmp[i] != (pkcsBlock[i] ^ 0x01)) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            WOLFSSL_MSG("RsaUnPad_PSS: Padding Error Match");
            return PSS_SALTLEN_RECOVER_E;
        }
        saltLen = maskLen - (i + 1);
    }
    else
#endif
    {
        for (i = 0; i < maskLen - 1 - saltLen; i++) {
            if (tmp[i] != pkcsBlock[i]) {
                XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
                WOLFSSL_MSG("RsaUnPad_PSS: Padding Error Match");
                return PSS_SALTLEN_E;
            }
        }
        if (tmp[i] != (pkcsBlock[i] ^ 0x01)) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            WOLFSSL_MSG("RsaUnPad_PSS: Padding Error End");
            return PSS_SALTLEN_E;
        }
    }
    xorbuf(pkcsBlock + i, tmp + i, (word32)(maskLen - i));

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
#endif

    *output = pkcsBlock + maskLen - saltLen;
    return saltLen + hLen;
}
#endif

/* UnPad plaintext, set start to *output, return length of plaintext,
 * < 0 on error */
static int RsaUnPad(const byte *pkcsBlock, unsigned int pkcsBlockLen,
                    byte **output, byte padValue)
{
    int    ret = BAD_FUNC_ARG;
    word16 i;

    if (output == NULL || pkcsBlockLen < 2 || pkcsBlockLen > 0xFFFF) {
        return BAD_FUNC_ARG;
    }

    if (padValue == RSA_BLOCK_TYPE_1) {
        /* First byte must be 0x00 and Second byte, block type, 0x01 */
        if (pkcsBlock[0] != 0 || pkcsBlock[1] != RSA_BLOCK_TYPE_1) {
            WOLFSSL_MSG("RsaUnPad error, invalid formatting");
            return RSA_PAD_E;
        }

        /* check the padding until we find the separator */
        for (i = 2; i < pkcsBlockLen; ) {
            if (pkcsBlock[i++] != 0xFF) {
                break;
            }
        }

        /* Minimum of 11 bytes of pre-message data and must have separator. */
        if (i < RSA_MIN_PAD_SZ || pkcsBlock[i-1] != 0) {
            WOLFSSL_MSG("RsaUnPad error, bad formatting");
            return RSA_PAD_E;
        }

        *output = (byte *)(pkcsBlock + i);
        ret = (int)pkcsBlockLen - i;
    }
#ifndef WOLFSSL_RSA_VERIFY_ONLY
    else {
        unsigned int j;
        word16 pastSep = 0;
        byte   invalid = 0;

        i = 0;
        /* Decrypted with private key - unpad must be constant time. */
        for (j = 2; j < pkcsBlockLen; j++) {
           /* Update i if not passed the separator and at separator. */
            i |= (word16)(~pastSep) & ctMask16Eq(pkcsBlock[j], 0x00) &
                (word16)(j + 1);
            pastSep |= ctMask16Eq(pkcsBlock[j], 0x00);
        }

        /* Minimum of 11 bytes of pre-message data - including leading 0x00. */
        invalid |= ctMaskLT(i, RSA_MIN_PAD_SZ);
        /* Must have seen separator. */
        invalid |= (byte)~pastSep;
        /* First byte must be 0x00. */
        invalid |= ctMaskNotEq(pkcsBlock[0], 0x00);
        /* Check against expected block type: padValue */
        invalid |= ctMaskNotEq(pkcsBlock[1], padValue);

        *output = (byte *)(pkcsBlock + i);
        ret = ((int)-1 + (int)(invalid >> 7)) & ((int)pkcsBlockLen - i);
    }
#endif

    return ret;
}

/* helper function to direct unpadding
 *
 * bits is the key modulus size in bits
 */
int wc_RsaUnPad_ex(byte* pkcsBlock, word32 pkcsBlockLen, byte** out,
                   byte padValue, int padType, enum wc_HashType hType,
                   int mgf, byte* optLabel, word32 labelLen, int saltLen,
                   int bits, void* heap)
{
    int ret;

    switch (padType) {
        case WC_RSA_PKCSV15_PAD:
            /*WOLFSSL_MSG("wolfSSL Using RSA PKCSV15 un-padding");*/
            ret = RsaUnPad(pkcsBlock, pkcsBlockLen, out, padValue);
            break;

    #ifndef WC_NO_RSA_OAEP
        case WC_RSA_OAEP_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA OAEP un-padding");
            ret = RsaUnPad_OAEP((byte*)pkcsBlock, pkcsBlockLen, out,
                                        hType, mgf, optLabel, labelLen, heap);
            break;
    #endif

    #ifdef WC_RSA_PSS
        case WC_RSA_PSS_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA PSS un-padding");
            ret = RsaUnPad_PSS((byte*)pkcsBlock, pkcsBlockLen, out, hType, mgf,
                                                           saltLen, bits, heap);
            break;
    #endif

    #ifdef WC_RSA_NO_PADDING
        case WC_RSA_NO_PAD:
            WOLFSSL_MSG("wolfSSL Using NO un-padding");

            /* In the case of no padding being used check that input is exactly
             * the RSA key length */
            if (bits <= 0 || pkcsBlockLen !=
                         ((word32)(bits+WOLFSSL_BIT_SIZE-1)/WOLFSSL_BIT_SIZE)) {
                WOLFSSL_MSG("Bad input size");
                ret = RSA_PAD_E;
            }
            else {
                if (out != NULL) {
                    *out = pkcsBlock;
                }
                ret = (int)pkcsBlockLen;
            }
            break;
    #endif /* WC_RSA_NO_PADDING */

        default:
            WOLFSSL_MSG("Unknown RSA UnPad Type");
            ret = RSA_PAD_E;
    }

    /* silence warning if not used with padding scheme */
    (void)hType;
    (void)mgf;
    (void)optLabel;
    (void)labelLen;
    (void)saltLen;
    (void)bits;
    (void)heap;

    return ret;
}

int wc_hash2mgf(enum wc_HashType hType)
{
    switch (hType) {
    case WC_HASH_TYPE_NONE:
        return WC_MGF1NONE;
    case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
        return WC_MGF1SHA1;
#else
        break;
#endif
    case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
        return WC_MGF1SHA224;
#else
        break;
#endif
    case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
        return WC_MGF1SHA256;
#else
        break;
#endif
    case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
        return WC_MGF1SHA384;
#else
        break;
#endif
    case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
        return WC_MGF1SHA512;
#else
        break;
#endif
    case WC_HASH_TYPE_MD2:
    case WC_HASH_TYPE_MD4:
    case WC_HASH_TYPE_MD5:
    case WC_HASH_TYPE_MD5_SHA:
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
    #endif
    case WC_HASH_TYPE_SHA3_224:
    case WC_HASH_TYPE_SHA3_256:
    case WC_HASH_TYPE_SHA3_384:
    case WC_HASH_TYPE_SHA3_512:
    case WC_HASH_TYPE_BLAKE2B:
    case WC_HASH_TYPE_BLAKE2S:
#ifdef WOLFSSL_SM3
    case WC_HASH_TYPE_SM3:
#endif
    #ifdef WOLFSSL_SHAKE128
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #ifdef WOLFSSL_SHAKE256
        case WC_HASH_TYPE_SHAKE256:
    #endif
    default:
        break;
    }
    WOLFSSL_MSG("Unrecognized or unsupported hash function");
    return WC_MGF1NONE;
}

#ifdef WC_RSA_NONBLOCK
static int wc_RsaFunctionNonBlock(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key)
{
    int    ret = 0;
    word32 keyLen, len;

    if (key == NULL || key->nb == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->nb->exptmod.state == TFM_EXPTMOD_NB_INIT) {
        if (mp_init(&key->nb->tmp) != MP_OKAY) {
            ret = MP_INIT_E;
        }

        if (ret == 0) {
            if (mp_read_unsigned_bin(&key->nb->tmp, (byte*)in, inLen) != MP_OKAY) {
                ret = MP_READ_E;
            }
        }
    }

    if (ret == 0) {
        switch(type) {
#if !defined(WOLFSSL_RSA_PUBLIC_ONLY)
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
            ret = fp_exptmod_nb(&key->nb->exptmod, &key->nb->tmp, &key->d,
                &key->n, &key->nb->tmp);
            if (ret == FP_WOULDBLOCK)
                return ret;
            if (ret != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            ret = fp_exptmod_nb(&key->nb->exptmod, &key->nb->tmp, &key->e,
                &key->n, &key->nb->tmp);
            if (ret == FP_WOULDBLOCK)
                return ret;
            if (ret != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;
        default:
            ret = RSA_WRONG_TYPE_E;
            break;
        }
    }

    if (ret == 0) {
        keyLen = wc_RsaEncryptSize(key);
        if (keyLen > *outLen)
            ret = RSA_BUFFER_E;
    }
    if (ret == 0) {
        len = mp_unsigned_bin_size(&key->nb->tmp);

        /* pad front w/ zeros to match key length */
        while (len < keyLen) {
            *out++ = 0x00;
            len++;
        }

        *outLen = keyLen;

        /* convert */
        if (mp_to_unsigned_bin(&key->nb->tmp, out) != MP_OKAY) {
             ret = MP_TO_E;
        }
    }

    mp_clear(&key->nb->tmp);

    return ret;
}
#endif /* WC_RSA_NONBLOCK */

#ifdef WOLFSSL_XILINX_CRYPT
/*
 * Xilinx hardened crypto acceleration.
 *
 * Returns 0 on success and negative values on error.
 */
static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int    ret = 0;
    word32 keyLen;
    (void)rng;

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        WOLFSSL_MSG("Output buffer is not big enough");
        return BAD_FUNC_ARG;
    }

    if (inLen != keyLen) {
        WOLFSSL_MSG("Expected that inLen equals RSA key length");
        return BAD_FUNC_ARG;
    }

    switch(type) {
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    #ifdef WOLFSSL_XILINX_CRYPTO_OLD
        /* Currently public exponent is loaded by default.
         * In SDK 2017.1 RSA exponent values are expected to be of 4 bytes
         * leading to private key operations with Xsecure_RsaDecrypt not being
         * supported */
        ret = RSA_WRONG_TYPE_E;
    #else
        {
            byte *d;
            int dSz;
#if !defined(WOLFSSL_XILINX_CRYPT_VERSAL)
            XSecure_Rsa rsa;
#endif

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
            dSz = WOLFSSL_XSECURE_RSA_KEY_SIZE * 2;
#else
            dSz = mp_unsigned_bin_size(&key->d);
#endif
            d = (byte*)XMALLOC(dSz, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            if (d == NULL) {
                ret = MEMORY_E;
            } else {
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
                XMEMSET(d, 0, dSz);
                XMEMCPY(d, key->mod, key->mSz);
                ret = mp_to_unsigned_bin(&key->d, &d[WOLFSSL_XSECURE_RSA_KEY_SIZE]);
#else
                ret = mp_to_unsigned_bin(&key->d, d);
                XSecure_RsaInitialize(&rsa, key->mod, NULL, d);
#endif
            }

            if (ret == 0) {
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
                WOLFSSL_XIL_DCACHE_FLUSH_RANGE((UINTPTR)d, dSz);
                WOLFSSL_XIL_DCACHE_FLUSH_RANGE((UINTPTR)in, inLen);
                if (XSecure_RsaPrivateDecrypt(&(key->xSec.cinst), XIL_CAST_U64(d),
                                              XIL_CAST_U64(in), inLen,
                                              XIL_CAST_U64(out)) != XST_SUCCESS) {
                    ret = BAD_STATE_E;
                }
                WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out, inLen);
#else
                if (XSecure_RsaPrivateDecrypt(&rsa, (u8*)in, inLen, out) !=
                        XST_SUCCESS) {
                    ret = BAD_STATE_E;
                }
#endif
            }

            if (d != NULL) {
                XFREE(d, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            }
        }
    #endif
        break;
    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
        WOLFSSL_XIL_DCACHE_FLUSH_RANGE((UINTPTR)key->mod,
                                       WOLFSSL_XSECURE_RSA_KEY_SIZE + 4);
        WOLFSSL_XIL_DCACHE_FLUSH_RANGE((UINTPTR)in, inLen);
        if (XSecure_RsaPublicEncrypt(&(key->xSec.cinst),
                                     XIL_CAST_U64(key->mod),
                                     XIL_CAST_U64(in), inLen,
                                     XIL_CAST_U64(out))) {
            WOLFSSL_MSG("RSA public operation failed");
            ret = BAD_STATE_E;
        }
        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE((UINTPTR)out, inLen);
#elif defined(WOLFSSL_XILINX_CRYPTO_OLD)
        if (XSecure_RsaDecrypt(&(key->xRsa), in, out) != XST_SUCCESS) {
            ret = BAD_STATE_E;
        }
#else
        /* starting at Xilinx release 2019 the function XSecure_RsaDecrypt was removed */
        if (XSecure_RsaPublicEncrypt(&(key->xRsa), (u8*)in, inLen, out) != XST_SUCCESS) {
            WOLFSSL_MSG("Error happened when calling hardware RSA public operation");
            ret = BAD_STATE_E;
        }
#endif
        break;
    default:
        ret = RSA_WRONG_TYPE_E;
    }

    *outLen = keyLen;

    return ret;
}

#elif defined(WOLFSSL_AFALG_XILINX_RSA)
#ifndef ERROR_OUT
#define ERROR_OUT(x) ret = (x); goto done
#endif

static const char WC_TYPE_ASYMKEY[] = "skcipher";
static const char WC_NAME_RSA[] = "xilinx-zynqmp-rsa";
#ifndef MAX_XILINX_RSA_KEY
    /* max key size of 4096 bits / 512 bytes */
    #define MAX_XILINX_RSA_KEY 512
#endif
static const byte XILINX_RSA_FLAG[] = {0x1};


/* AF_ALG implementation of RSA */
static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    struct msghdr   msg;
    struct cmsghdr* cmsg;
    struct iovec      iov;
    byte*  keyBuf   = NULL;
    word32 keyBufSz = 0;
    char cbuf[CMSG_SPACE(4) + CMSG_SPACE(sizeof(struct af_alg_iv) + 1)] = {0};
    int    ret = 0;
    int    op  = 0;    /* decryption vs encryption flag */
    word32 keyLen;

    /* input and output buffer need to be aligned */
    ALIGN64 byte outBuf[MAX_XILINX_RSA_KEY];
    ALIGN64 byte inBuf[MAX_XILINX_RSA_KEY];

    XMEMSET(&msg, 0, sizeof(struct msghdr));
    (void)rng;

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        ERROR_OUT(RSA_BUFFER_E);
    }

    if (keyLen > MAX_XILINX_RSA_KEY) {
        WOLFSSL_MSG("RSA key size larger than supported");
        ERROR_OUT(BAD_FUNC_ARG);
    }

    if ((keyBuf = (byte*)XMALLOC(keyLen * 2, key->heap, DYNAMIC_TYPE_KEY))
            == NULL) {
        ERROR_OUT(MEMORY_E);
    }

    if ((ret = mp_to_unsigned_bin(&(key->n), keyBuf)) != MP_OKAY) {
        ERROR_OUT(MP_TO_E);
    }

    switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
            op = 1; /* set as decrypt */
            {
                keyBufSz = mp_unsigned_bin_size(&(key->d));
                if ((mp_to_unsigned_bin(&(key->d), keyBuf + keyLen))
                        != MP_OKAY) {
                    ERROR_OUT(MP_TO_E);
                }
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                /* Seed must be zeroized now that it has been used. */
                wc_MemZero_Add("RSA Sync Priv Enc/Dec keyBuf", keyBuf + keyLen,
                    keyBufSz);
            #endif
            }
            break;

        case RSA_PUBLIC_DECRYPT:
        case RSA_PUBLIC_ENCRYPT: {
            word32 exp = 0;
            word32 eSz = mp_unsigned_bin_size(&(key->e));
            if ((mp_to_unsigned_bin(&(key->e), (byte*)&exp +
                            (sizeof(word32) - eSz))) != MP_OKAY) {
                ERROR_OUT(MP_TO_E);
            }
            keyBufSz = sizeof(word32);
            XMEMCPY(keyBuf + keyLen, (byte*)&exp, keyBufSz);
            break;
        }

        default:
            ERROR_OUT(RSA_WRONG_TYPE_E);
    }
    keyBufSz += keyLen; /* add size of modulus */

    /* check for existing sockets before creating new ones */
    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }

    /* create new sockets and set the key to use */
    if ((key->alFd = wc_Afalg_Socket()) < 0) {
        WOLFSSL_MSG("Unable to create socket");
        ERROR_OUT(key->alFd);
    }
    if ((key->rdFd = wc_Afalg_CreateRead(key->alFd, WC_TYPE_ASYMKEY,
                    WC_NAME_RSA)) < 0) {
        WOLFSSL_MSG("Unable to bind and create read/send socket");
        ERROR_OUT(key->rdFd);
    }
    if ((ret = setsockopt(key->alFd, SOL_ALG, ALG_SET_KEY, keyBuf,
                    keyBufSz)) < 0) {
        WOLFSSL_MSG("Error setting RSA key");
        ERROR_OUT(ret);
    }

    msg.msg_control    = cbuf;
    msg.msg_controllen = sizeof(cbuf);
    cmsg = CMSG_FIRSTHDR(&msg);
    if ((ret = wc_Afalg_SetOp(cmsg, op)) < 0) {
        ERROR_OUT(ret);
    }

    /* set flag in IV spot, needed for Xilinx hardware acceleration use */
    cmsg = CMSG_NXTHDR(&msg, cmsg);
    if ((ret = wc_Afalg_SetIv(cmsg, (byte*)XILINX_RSA_FLAG,
                    sizeof(XILINX_RSA_FLAG))) != 0) {
        ERROR_OUT(ret);
    }

    /* compose and send msg */
    XMEMCPY(inBuf, (byte*)in, inLen); /* for alignment */
    iov.iov_base = inBuf;
    iov.iov_len  = inLen;
    msg.msg_iov  = &iov;
    msg.msg_iovlen = 1;
    if ((ret = sendmsg(key->rdFd, &msg, 0)) <= 0) {
        ERROR_OUT(WC_AFALG_SOCK_E);
    }

    if ((ret = read(key->rdFd, outBuf, inLen)) <= 0) {
        ERROR_OUT(WC_AFALG_SOCK_E);
    }
    XMEMCPY(out, outBuf, ret);
    *outLen = keyLen;

done:
    /* clear key data and free buffer */
    if (keyBuf != NULL) {
        ForceZero(keyBuf, keyBufSz);
    }
    XFREE(keyBuf, key->heap, DYNAMIC_TYPE_KEY);

    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }

    return ret;
}

#elif defined(WOLFSSL_KCAPI_RSA)
static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                              word32* outLen, int type, RsaKey* key,
                              WC_RNG* rng)
{
    int ret;

    (void)rng;

    switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
            ret = KcapiRsa_Decrypt(key, in, inLen, out, outLen);
            break;

        case RSA_PUBLIC_DECRYPT:
        case RSA_PUBLIC_ENCRYPT:
            ret = KcapiRsa_Encrypt(key, in, inLen, out, outLen);
            break;

        default:
            ret = RSA_WRONG_TYPE_E;
    }

    return ret;
}
#else
#ifndef WOLF_CRYPTO_CB_ONLY_RSA
#ifdef WOLFSSL_HAVE_SP_RSA
static int RsaFunction_SP(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    (void)rng;

#ifndef WOLFSSL_SP_NO_2048
    if (mp_count_bits(&key->n) == 2048) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 1024) &&
                                             (mp_count_bits(&key->q) == 1024)) {
                return sp_RsaPrivate_2048(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_2048(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_2048(in, inLen, &key->e, &key->n, out, outLen);
        default:
            break;
        }
    }
#endif
#ifndef WOLFSSL_SP_NO_3072
    if (mp_count_bits(&key->n) == 3072) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 1536) &&
                                             (mp_count_bits(&key->q) == 1536)) {
                return sp_RsaPrivate_3072(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_3072(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_3072(in, inLen, &key->e, &key->n, out, outLen);
        default:
            break;
        }
    }
#endif
#ifdef WOLFSSL_SP_4096
    if (mp_count_bits(&key->n) == 4096) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 2048) &&
                                             (mp_count_bits(&key->q) == 2048)) {
                return sp_RsaPrivate_4096(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_4096(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_4096(in, inLen, &key->e, &key->n, out, outLen);
        default:
            break;
        }
    }
#endif

    /* SP not able to do operation. */
    return WC_KEY_SIZE_E;
}
#endif

#if !defined(WOLFSSL_SP_MATH)
#if !defined(WOLFSSL_RSA_PUBLIC_ONLY) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
static int RsaFunctionPrivate(mp_int* tmp, RsaKey* key, WC_RNG* rng)
{
    int    ret = 0;
#if defined(WC_RSA_BLINDING) && !defined(WC_NO_RNG)
    DECL_MP_INT_SIZE_DYN(rnd, mp_bitsused(&key->n), RSA_MAX_SIZE);
    DECL_MP_INT_SIZE_DYN(rndi, mp_bitsused(&key->n), RSA_MAX_SIZE);
#endif /* WC_RSA_BLINDING && !WC_NO_RNG */

    (void)rng;

#if defined(WC_RSA_BLINDING) && !defined(WC_NO_RNG)
    NEW_MP_INT_SIZE(rnd, mp_bitsused(&key->n), key->heap, DYNAMIC_TYPE_RSA);
    NEW_MP_INT_SIZE(rndi, mp_bitsused(&key->n), key->heap, DYNAMIC_TYPE_RSA);
#ifdef MP_INT_SIZE_CHECK_NULL
    if ((rnd == NULL) || (rndi == NULL)) {
        FREE_MP_INT_SIZE(rnd, key->heap, DYNAMIC_TYPE_RSA);
        FREE_MP_INT_SIZE(rndi, key->heap, DYNAMIC_TYPE_RSA);
        return MEMORY_E;
    }
#endif

    if ((INIT_MP_INT_SIZE(rnd, mp_bitsused(&key->n)) != MP_OKAY) ||
            (INIT_MP_INT_SIZE(rndi, mp_bitsused(&key->n)) != MP_OKAY)) {
        ret = MP_INIT_E;
    }

    if (ret == 0) {
        /* blind */
        ret = mp_rand(rnd, get_digit_count(&key->n), rng);
    }
    if (ret == 0) {
        /* rndi = 1/rnd mod n */
        if (mp_invmod(rnd, &key->n, rndi) != MP_OKAY) {
            ret = MP_INVMOD_E;
        }
    }
    if (ret == 0) {
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        mp_memzero_add("RSA Private rnd", rnd);
        mp_memzero_add("RSA Private rndi", rndi);
    #endif

        /* rnd = rnd^e */
    #ifndef WOLFSSL_SP_MATH_ALL
        if (mp_exptmod(rnd, &key->e, &key->n, rnd) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
    #else
        if (mp_exptmod_nct(rnd, &key->e, &key->n, rnd) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
    #endif
    }

    if (ret == 0) {
        /* tmp = tmp*rnd mod n */
        if (mp_mulmod(tmp, rnd, &key->n, tmp) != MP_OKAY) {
            ret = MP_MULMOD_E;
        }
    }
#endif /* WC_RSA_BLINDING && !WC_NO_RNG */

#ifdef RSA_LOW_MEM      /* half as much memory but twice as slow */
    if (ret == 0) {
        if (mp_exptmod(tmp, &key->d, &key->n, tmp) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
    }
#else
    if (ret == 0) {
        mp_int* tmpa = tmp;
#if defined(WC_RSA_BLINDING) && !defined(WC_NO_RNG)
        mp_int* tmpb = rnd;
#else
        DECL_MP_INT_SIZE_DYN(tmpb, mp_bitsused(&key->n), RSA_MAX_SIZE);
#endif

#if !defined(WC_RSA_BLINDING) || defined(WC_NO_RNG)
        NEW_MP_INT_SIZE(tmpb, mp_bitsused(&key->n), key->heap,
            DYNAMIC_TYPE_RSA);
    #ifdef MP_INT_SIZE_CHECK_NULL
        if (tmpb == NULL) {
            ret = MEMORY_E;
        }
    #endif
        if ((ret == 0) && INIT_MP_INT_SIZE(tmpb, mp_bitsused(&key->n)) !=
                MP_OKAY) {
            ret = MP_INIT_E;
        }
#endif

    #ifdef WOLFSSL_CHECK_MEM_ZERO
        if (ret == 0) {
            mp_memzero_add("RSA Sync tmpb", tmpb);
        }
    #endif

        /* tmpb = tmp^dQ mod q */
        if (ret == 0 && mp_exptmod(tmp, &key->dQ, &key->q, tmpb) != MP_OKAY)
            ret = MP_EXPTMOD_E;

        /* tmpa = tmp^dP mod p */
        if (ret == 0 && mp_exptmod(tmp, &key->dP, &key->p, tmpa) != MP_OKAY)
            ret = MP_EXPTMOD_E;

        /* tmp = (tmp - tmpb) * qInv (mod p) */
    #if (defined(WOLFSSL_SP_MATH) || (defined(WOLFSSL_SP_MATH_ALL)) && \
                                              !defined(WOLFSSL_SP_INT_NEGATIVE))
        if (ret == 0 && mp_submod(tmpa, tmpb, &key->p, tmp) != MP_OKAY)
            ret = MP_SUB_E;
    #else
        if (ret == 0 && mp_sub(tmpa, tmpb, tmp) != MP_OKAY)
            ret = MP_SUB_E;
    #endif

        if (ret == 0 && mp_mulmod(tmp, &key->u, &key->p, tmp) != MP_OKAY)
            ret = MP_MULMOD_E;

        /* tmp = tmpb + q * tmp */
        if (ret == 0 && mp_mul(tmp, &key->q, tmp) != MP_OKAY)
            ret = MP_MUL_E;

        if (ret == 0 && mp_add(tmp, tmpb, tmp) != MP_OKAY)
            ret = MP_ADD_E;

#if !defined(WC_RSA_BLINDING) || defined(WC_NO_RNG)
        mp_forcezero(tmpb);
        FREE_MP_INT_SIZE(tmpb, key->heap, DYNAMIC_TYPE_RSA);
    #if !defined(MP_INT_SIZE_CHECK_NULL) && defined(WOLFSSL_CHECK_MEM_ZERO)
        mp_memzero_check(tmpb);
    #endif
#endif
    }
#endif   /* RSA_LOW_MEM */

#if defined(WC_RSA_BLINDING) && !defined(WC_NO_RNG)
    /* unblind */
    if (ret == 0 && mp_mulmod(tmp, rndi, &key->n, tmp) != MP_OKAY)
        ret = MP_MULMOD_E;

    mp_forcezero(rndi);
    mp_forcezero(rnd);
    FREE_MP_INT_SIZE(rndi, key->heap, DYNAMIC_TYPE_RSA);
    FREE_MP_INT_SIZE(rnd, key->heap, DYNAMIC_TYPE_RSA);
#if !defined(MP_INT_SIZE_CHECK_NULL) && defined(WOLFSSL_CHECK_MEM_ZERO)
    mp_memzero_check(rnd);
    mp_memzero_check(rndi);
#endif
#endif /* WC_RSA_BLINDING && !WC_NO_RNG */
    return ret;
}
#endif

static int RsaFunctionSync(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    DECL_MP_INT_SIZE_DYN(tmp, mp_bitsused(&key->n), RSA_MAX_SIZE);
    int    ret = 0;

    (void)rng;

    NEW_MP_INT_SIZE(tmp, mp_bitsused(&key->n), key->heap, DYNAMIC_TYPE_RSA);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (tmp == NULL)
        return MEMORY_E;
#endif

    if (INIT_MP_INT_SIZE(tmp, mp_bitsused(&key->n)) != MP_OKAY)
        ret = MP_INIT_E;

#ifndef TEST_UNPAD_CONSTANT_TIME
    if (ret == 0 && mp_read_unsigned_bin(tmp, in, inLen) != MP_OKAY)
        ret = MP_READ_E;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    if (ret == 0) {
        mp_memzero_add("RSA sync tmp", tmp);
    }
#endif

    if (ret == 0) {
        switch(type) {
    #if !defined(WOLFSSL_RSA_PUBLIC_ONLY) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
        {
            ret = RsaFunctionPrivate(tmp, key, rng);
            break;
        }
    #endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            if (mp_exptmod_nct(tmp, &key->e, &key->n, tmp) != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;
        default:
            ret = RSA_WRONG_TYPE_E;
            break;
        }
    }

    if (ret == 0) {
        if (mp_to_unsigned_bin_len(tmp, out, (int)*outLen) != MP_OKAY)
             ret = MP_TO_E;
    }
#else
    (void)type;
    (void)key;
    XMEMCPY(out, in, inLen);
#endif

    mp_forcezero(tmp);
    FREE_MP_INT_SIZE(tmp, key->heap, DYNAMIC_TYPE_RSA);
#if !defined(MP_INT_SIZE_CHECK_NULL) && defined(WOLFSSL_CHECK_MEM_ZERO)
    mp_memzero_check(tmp);
#endif
    return ret;
}
#endif /* !WOLFSSL_SP_MATH */

static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret;
    word32 keyLen;

    ret = wc_RsaEncryptSize(key);
    if (ret < 0) {
        return ret;
    }
    keyLen = (word32)ret;

    if (inLen > keyLen) {
        WOLFSSL_MSG("Expected that inLen be no longer RSA key length");
        return BAD_FUNC_ARG;
    }
    if (keyLen > *outLen) {
        WOLFSSL_MSG("Expected that outLen be no shorter RSA key length");
        return RSA_BUFFER_E;
    }

    if (mp_iseven(&key->n)) {
        return MP_VAL;
    }

#ifdef WOLFSSL_HAVE_SP_RSA
    ret = RsaFunction_SP(in, inLen, out, outLen, type, key, rng);
    if (ret != WC_KEY_SIZE_E)
        return ret;
#endif /* WOLFSSL_HAVE_SP_RSA */

#if defined(WOLFSSL_SP_MATH)
    (void)rng;
#ifndef WOLFSSL_HAVE_SP_RSA
    (void)in;
    (void)inLen;
    (void)out;
    (void)outLen;
    (void)type;
    (void)key;
    #error RSA SP option invalid (enable WOLFSSL_HAVE_SP_RSA or disable WOLFSSL_SP_MATH)
    return NOT_COMPILED_IN;
#else
    WOLFSSL_MSG("SP Key Size Error");
    return WC_KEY_SIZE_E;
#endif
#else
    *outLen = keyLen;
    return RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
#endif /* WOLFSSL_SP_MATH */
}
#endif /* WOLF_CRYPTO_CB_ONLY_RSA */
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
static int wc_RsaFunctionAsync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = 0;

    (void)rng;

#ifdef WOLFSSL_ASYNC_CRYPT_SW
    if (wc_AsyncSwInit(&key->asyncDev, ASYNC_SW_RSA_FUNC)) {
        WC_ASYNC_SW* sw = &key->asyncDev.sw;
        sw->rsaFunc.in = in;
        sw->rsaFunc.inSz = inLen;
        sw->rsaFunc.out = out;
        sw->rsaFunc.outSz = outLen;
        sw->rsaFunc.type = type;
        sw->rsaFunc.key = key;
        sw->rsaFunc.rng = rng;
        return WC_PENDING_E;
    }
#endif /* WOLFSSL_ASYNC_CRYPT_SW */

    switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->d.raw.buf, key->d.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        #ifdef RSA_LOW_MEM
            ret = IntelQaRsaPrivate(&key->asyncDev, in, inLen,
                                    &key->d.raw, &key->n.raw,
                                    out, outLen);
        #else
            ret = IntelQaRsaCrtPrivate(&key->asyncDev, in, inLen,
                                &key->p.raw, &key->q.raw,
                                &key->dP.raw, &key->dQ.raw,
                                &key->u.raw,
                                out, outLen);
        #endif
    #else /* WOLFSSL_ASYNC_CRYPT_SW */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;
#endif

    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->e.raw.buf, key->e.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        ret = IntelQaRsaPublic(&key->asyncDev, in, inLen,
                               &key->e.raw, &key->n.raw,
                               out, outLen);
    #else /* WOLFSSL_ASYNC_CRYPT_SW */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;

    default:
        ret = RSA_WRONG_TYPE_E;
    }

    return ret;
}
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_RSA */

#if defined(WC_RSA_DIRECT) || defined(WC_RSA_NO_PADDING)
/* Function that does the RSA operation directly with no padding.
 *
 * in       buffer to do operation on
 * inLen    length of input buffer
 * out      buffer to hold results
 * outSz    gets set to size of result buffer. Should be passed in as length
 *          of out buffer. If the pointer "out" is null then outSz gets set to
 *          the expected buffer size needed and LENGTH_ONLY_E gets returned.
 * key      RSA key to use for encrypt/decrypt
 * type     if using private or public key {RSA_PUBLIC_ENCRYPT,
 *          RSA_PUBLIC_DECRYPT, RSA_PRIVATE_ENCRYPT, RSA_PRIVATE_DECRYPT}
 * rng      wolfSSL RNG to use if needed
 *
 * returns size of result on success
 */
int wc_RsaDirect(byte* in, word32 inLen, byte* out, word32* outSz,
        RsaKey* key, int type, WC_RNG* rng)
{
    int ret;

    if (in == NULL || outSz == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check on type of RSA operation */
    switch (type) {
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
        case RSA_PRIVATE_DECRYPT:
            break;
        default:
            WOLFSSL_MSG("Bad RSA type");
            return BAD_FUNC_ARG;
    }

    if ((ret = wc_RsaEncryptSize(key)) < 0) {
        return BAD_FUNC_ARG;
    }

    if (inLen != (word32)ret) {
        WOLFSSL_MSG("Bad input length. Should be RSA key size");
        return BAD_FUNC_ARG;
    }

    if (out == NULL) {
        *outSz = inLen;
        return LENGTH_ONLY_E;
    }

    switch (key->state) {
        case RSA_STATE_NONE:
        case RSA_STATE_ENCRYPT_PAD:
        case RSA_STATE_ENCRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_UNPAD:
            key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_EXPTMOD:
                                                  RSA_STATE_DECRYPT_EXPTMOD;

            key->dataLen = *outSz;

            ret = wc_RsaFunction(in, inLen, out, &key->dataLen, type, key, rng);
            if (ret >= 0 || ret == WC_PENDING_E) {
                key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_RES:
                                                  RSA_STATE_DECRYPT_RES;
            }
            if (ret < 0) {
                break;
            }

            FALL_THROUGH;

        case RSA_STATE_ENCRYPT_RES:
        case RSA_STATE_DECRYPT_RES:
            ret = (int)key->dataLen;
            break;

        default:
            ret = BAD_STATE_E;
    }

    /* if async pending then skip cleanup*/
    if (ret == WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        || ret == FP_WOULDBLOCK
    #endif
    ) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}
#endif /* WC_RSA_DIRECT || WC_RSA_NO_PADDING */

#if defined(WOLFSSL_CRYPTOCELL)
static int cc310_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key)
{
    CRYSError_t ret = 0;
    CRYS_RSAPrimeData_t primeData;
    int modulusSize = wc_RsaEncryptSize(key);

    /* The out buffer must be at least modulus size bytes long. */
    if (outLen < modulusSize)
        return BAD_FUNC_ARG;

    ret = CRYS_RSA_PKCS1v15_Encrypt(&wc_rndState,
                                    wc_rndGenVectFunc,
                                    &key->ctx.pubKey,
                                    &primeData,
                                    (byte*)in,
                                    inLen,
                                    out);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Encrypt failed");
        return -1;
    }

    return modulusSize;
}
static int cc310_RsaPublicDecrypt(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key)
{
    CRYSError_t ret = 0;
    CRYS_RSAPrimeData_t primeData;
    word16 actualOutLen = outLen;

    ret = CRYS_RSA_PKCS1v15_Decrypt(&key->ctx.privKey,
                                    &primeData,
                                    (byte*)in,
                                    inLen,
                                    out,
                                    &actualOutLen);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Decrypt failed");
        return -1;
    }
    return actualOutLen;
}

int cc310_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                  word32 outLen, RsaKey* key, CRYS_RSA_HASH_OpMode_t mode)
{
    CRYSError_t ret = 0;
    word16 actualOutLen = outLen*sizeof(byte);
    CRYS_RSAPrivUserContext_t  contextPrivate;

    ret =  CRYS_RSA_PKCS1v15_Sign(&wc_rndState,
                wc_rndGenVectFunc,
                &contextPrivate,
                &key->ctx.privKey,
                mode,
                (byte*)in,
                inLen,
                out,
                &actualOutLen);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Sign failed");
        return -1;
    }
    return actualOutLen;
}

int cc310_RsaSSL_Verify(const byte* in, word32 inLen, byte* sig,
                               RsaKey* key, CRYS_RSA_HASH_OpMode_t mode)
{
    CRYSError_t ret = 0;
    CRYS_RSAPubUserContext_t contextPub;

    /* verify the signature in the sig pointer */
    ret =  CRYS_RSA_PKCS1v15_Verify(&contextPub,
                &key->ctx.pubKey,
                mode,
                (byte*)in,
                inLen,
                sig);

    if (ret != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_RSA_PKCS1v15_Verify failed");
        return -1;
    }

    return ret;
}
#endif /* WOLFSSL_CRYPTOCELL */

#ifndef WOLF_CRYPTO_CB_ONLY_RSA
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(TEST_UNPAD_CONSTANT_TIME) &&     !defined(NO_RSA_BOUNDS_CHECK)
/* Check that 1 < in < n-1. (Requirement of 800-56B.) */
static int RsaFunctionCheckIn(const byte* in, word32 inLen, RsaKey* key,
    int checkSmallCt)
{
    int ret = 0;
    DECL_MP_INT_SIZE_DYN(c, mp_bitsused(&key->n), RSA_MAX_SIZE);

    NEW_MP_INT_SIZE(c, mp_bitsused(&key->n), key->heap, DYNAMIC_TYPE_RSA);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (c == NULL)
        ret = MEMORY_E;
#endif

    if (ret == 0 && INIT_MP_INT_SIZE(c, mp_bitsused(&key->n)) != MP_OKAY) {
        ret = MP_INIT_E;
    }
    if (ret == 0) {
        if (mp_read_unsigned_bin(c, in, inLen) != 0)
            ret = MP_READ_E;
    }
    if (ret == 0) {
        /* check c > 1 */
        if (checkSmallCt && (mp_cmp_d(c, 1) != MP_GT))
            ret = RSA_OUT_OF_RANGE_E;
    }
    if (ret == 0) {
        /* add c+1 */
        if (mp_add_d(c, 1, c) != MP_OKAY)
            ret = MP_ADD_E;
    }
    if (ret == 0) {
        /* check c+1 < n */
        if (mp_cmp(c, &key->n) != MP_LT)
            ret = RSA_OUT_OF_RANGE_E;
    }
    mp_clear(c);

    FREE_MP_INT_SIZE(c, key->heap, DYNAMIC_TYPE_RSA);

    return ret;
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY && !TEST_UNPAD_CONSTANT_TIME &&
        * !NO_RSA_BOUNDS_CHECK */
#endif /* WOLF_CRYPTO_CB_ONLY_RSA */

static int wc_RsaFunction_ex(const byte* in, word32 inLen, byte* out,
                             word32* outLen, int type, RsaKey* key, WC_RNG* rng,
                             int checkSmallCt)
{
    int ret = 0;
    (void)rng;
    (void)checkSmallCt;

    if (key == NULL || in == NULL || inLen == 0 || out == NULL ||
            outLen == NULL || *outLen == 0 || type == RSA_TYPE_UNKNOWN) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (key->devId != INVALID_DEVID)
    #endif
    {
        ret = wc_CryptoCb_Rsa(in, inLen, out, outLen, type, key, rng);
        #ifndef WOLF_CRYPTO_CB_ONLY_RSA
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable and try using software */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_RSA
        if (ret == CRYPTOCB_UNAVAILABLE) {
            return NO_VALID_DEVID;
        }
        return ret;
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_RSA
    SAVE_VECTOR_REGISTERS(return _svr_ret;);

#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(TEST_UNPAD_CONSTANT_TIME) && \
    !defined(NO_RSA_BOUNDS_CHECK)
    if (type == RSA_PRIVATE_DECRYPT &&
        key->state == RSA_STATE_DECRYPT_EXPTMOD) {

        ret = RsaFunctionCheckIn(in, inLen, key, checkSmallCt);
        if (ret != 0) {
            RESTORE_VECTOR_REGISTERS();
            return ret;
        }
    }
#endif /* !WOLFSSL_RSA_VERIFY_ONLY && !TEST_UNPAD_CONSTANT_TIME && \
        * !NO_RSA_BOUNDS_CHECK */

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                        key->n.raw.len > 0) {
        ret = wc_RsaFunctionAsync(in, inLen, out, outLen, type, key, rng);
    }
    else
#endif
#ifdef WC_RSA_NONBLOCK
    if (key->nb) {
        ret = wc_RsaFunctionNonBlock(in, inLen, out, outLen, type, key);
    }
    else
#endif
    {
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    }

    RESTORE_VECTOR_REGISTERS();

    /* handle error */
    if (ret < 0 && ret != WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        && ret != FP_WOULDBLOCK
    #endif
    ) {
        if (ret == MP_EXPTMOD_E) {
            /* This can happen due to incorrectly set FP_MAX_BITS or missing XREALLOC */
            WOLFSSL_MSG("RSA_FUNCTION MP_EXPTMOD_E: memory/config problem");
        }

        key->state = RSA_STATE_NONE;
        wc_RsaCleanup(key);
    }
    return ret;
#endif /* WOLF_CRYPTO_CB_ONLY_RSA */
}

int wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    /* Always check for ciphertext of 0 or 1. (Shouldn't for OAEP decrypt.) */
    return wc_RsaFunction_ex(in, inLen, out, outLen, type, key, rng, 1);
}

#ifndef WOLFSSL_RSA_VERIFY_ONLY
/* Internal Wrappers */
/* Gives the option of choosing padding type
   in : input to be encrypted
   inLen: length of input buffer
   out: encrypted output
   outLen: length of encrypted output buffer
   key   : wolfSSL initialized RSA key struct
   rng   : wolfSSL initialized random number struct
   rsa_type  : type of RSA: RSA_PUBLIC_ENCRYPT, RSA_PUBLIC_DECRYPT,
        RSA_PRIVATE_ENCRYPT or RSA_PRIVATE_DECRYPT
   pad_value: RSA_BLOCK_TYPE_1 or RSA_BLOCK_TYPE_2
   pad_type  : type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
        WC_RSA_NO_PAD or WC_RSA_PSS_PAD
   hash  : type of hash algorithm to use found in wolfssl/wolfcrypt/hash.h
   mgf   : type of mask generation function to use
   label : optional label
   labelSz : size of optional label buffer
   saltLen : Length of salt used in PSS
   rng : random number generator */
static int RsaPublicEncryptEx(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int rsa_type,
                            byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret = 0;
    int sz;
    int state;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = wc_RsaEncryptSize(key);
    if (sz > (int)outLen) {
        return RSA_BUFFER_E;
    }

    if (sz < RSA_MIN_PAD_SZ || sz > (int)RSA_MAX_SIZE/8) {
        return WC_KEY_SIZE_E;
    }

    if (inLen > (word32)(sz - RSA_MIN_PAD_SZ)) {
#ifdef WC_RSA_NO_PADDING
        /* In the case that no padding is used the input length can and should
         * be the same size as the RSA key. */
        if (pad_type != WC_RSA_NO_PAD)
#endif
        return RSA_BUFFER_E;
    }

#ifndef WOLFSSL_BIND
    state = key->state;
#else
    /* Bind9 shares the EVP_PKEY struct across multiple threads so let's just
     * force a restart on each RsaPublicEncryptEx call for it. */
    state = RSA_STATE_NONE;
#ifdef WOLFSSL_ASYNC_CRYPT
#error wolfSSL does not handle building bind support with async crypto
#endif
#endif
    switch (state) {
    case RSA_STATE_NONE:
    case RSA_STATE_ENCRYPT_PAD:
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                 pad_type != WC_RSA_PSS_PAD && key->n.raw.buf) {
            /* Async operations that include padding */
            if (rsa_type == RSA_PUBLIC_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaPublicEncrypt(in, inLen, out, outLen, key);
            }
            else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaSSL_Sign(in, inLen, out, outLen, key);
            }
        }
    #elif defined(WOLFSSL_CRYPTOCELL)
        if (rsa_type == RSA_PUBLIC_ENCRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_2) {

            return cc310_RsaPublicEncrypt(in, inLen, out, outLen, key);
        }
        else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                         pad_value == RSA_BLOCK_TYPE_1) {
            return cc310_RsaSSL_Sign(in, inLen, out, outLen, key,
                                  cc310_hashModeRSA(hash, 0));
        }
    #elif defined(WOLFSSL_SE050)
        if (rsa_type == RSA_PUBLIC_ENCRYPT && pad_value == RSA_BLOCK_TYPE_2) {
            return se050_rsa_public_encrypt(in, inLen, out, outLen, key,
                                            rsa_type, pad_value, pad_type, hash,
                                            mgf, label, labelSz, sz);
        }
        else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                              pad_value == RSA_BLOCK_TYPE_1) {
            return se050_rsa_sign(in, inLen, out, outLen, key, rsa_type,
                                  pad_value, pad_type, hash, mgf, label,
                                  labelSz, sz);
        }
    #elif defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY) || \
          (!defined(WOLFSSL_RENESAS_TSIP_TLS) && \
            defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY))
           /* SCE needs warpped key which is passed via
            * user ctx object of crypt-call back.
            */
       #ifdef WOLF_CRYPTO_CB
            if (key->devId != INVALID_DEVID) {
                /* SCE supports 1024 and 2048 bits */
                ret = wc_CryptoCb_Rsa(in, inLen, out,
                                    outLen, rsa_type, key, rng);
                if (ret != CRYPTOCB_UNAVAILABLE)
                    return ret;
                /* fall-through when unavailable */
                ret = 0; /* reset error code and try using software */
            }
       #endif
    #endif /* WOLFSSL_SE050 */

        key->state = RSA_STATE_ENCRYPT_PAD;
        ret = wc_RsaPad_ex(in, inLen, out, (word32)sz, pad_value, rng, pad_type,
                           hash, mgf, label, labelSz, saltLen,
                           mp_count_bits(&key->n), key->heap);
        if (ret < 0) {
            break;
        }

        key->state = RSA_STATE_ENCRYPT_EXPTMOD;
        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_EXPTMOD:

        key->dataLen = outLen;
        ret = wc_RsaFunction(out, (word32)sz, out, &key->dataLen, rsa_type, key,
                             rng);

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_ENCRYPT_RES;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_RES:
        ret = (int)key->dataLen;
        break;

    default:
        ret = BAD_STATE_E;
        break;
    }

    /* if async pending then return and skip done cleanup below */
    if (ret == WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        || ret == FP_WOULDBLOCK
    #endif
    ) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}

#endif

/* Gives the option of choosing padding type
   in : input to be decrypted
   inLen: length of input buffer
   out:  decrypted message
   outLen: length of decrypted message in bytes
   outPtr: optional inline output pointer (if provided doing inline)
   key   : wolfSSL initialized RSA key struct
   rsa_type  : type of RSA: RSA_PUBLIC_ENCRYPT, RSA_PUBLIC_DECRYPT,
        RSA_PRIVATE_ENCRYPT or RSA_PRIVATE_DECRYPT
   pad_value: RSA_BLOCK_TYPE_1 or RSA_BLOCK_TYPE_2
   pad_type  : type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
        WC_RSA_NO_PAD, WC_RSA_PSS_PAD
   hash  : type of hash algorithm to use found in wolfssl/wolfcrypt/hash.h
   mgf   : type of mask generation function to use
   label : optional label
   labelSz : size of optional label buffer
   saltLen : Length of salt used in PSS
   rng : random number generator */
static int RsaPrivateDecryptEx(const byte* in, word32 inLen, byte* out,
                            word32 outLen, byte** outPtr, RsaKey* key,
                            int rsa_type, byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret = RSA_WRONG_TYPE_E;
    byte* pad = NULL;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    switch (key->state) {
    case RSA_STATE_NONE:
        key->dataLen = inLen;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        /* Async operations that include padding */
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
            if (rsa_type == RSA_PRIVATE_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaPrivateDecrypt(in, inLen, out, &key->dataLen,
                                               key);
#endif
            }
            else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaSSL_Verify(in, inLen, out, &key->dataLen, key);
            }
        }
    #elif defined(WOLFSSL_CRYPTOCELL)
        if (rsa_type == RSA_PRIVATE_DECRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_2) {
            ret = cc310_RsaPublicDecrypt(in, inLen, out, outLen, key);
            if (outPtr != NULL)
                *outPtr = out; /* for inline */
            return ret;
        }
        else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                            pad_value == RSA_BLOCK_TYPE_1) {
            return cc310_RsaSSL_Verify(in, inLen, out, key,
                                       cc310_hashModeRSA(hash, 0));
        }
    #elif defined(WOLFSSL_SE050)
        if (rsa_type == RSA_PRIVATE_DECRYPT && pad_value == RSA_BLOCK_TYPE_2) {
            ret = se050_rsa_private_decrypt(in, inLen, out, outLen, key,
                                            rsa_type, pad_value, pad_type, hash,
                                            mgf, label, labelSz);
            if (outPtr != NULL) {
                *outPtr = out;
            }
            return ret;
        }
        else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
            ret = se050_rsa_verify(in, inLen, out, outLen, key, rsa_type,
                                   pad_value, pad_type, hash, mgf, label,
                                   labelSz);
            if (outPtr != NULL) {
                *outPtr = out;
            }
            return ret;
        }
    #elif defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY) || \
          (!defined(WOLFSSL_RENESAS_TSIP_TLS) && \
            defined(WOLFSSL_RENESAS_TSIP_CRYPTONLY))
           #ifdef WOLF_CRYPTO_CB
                if (key->devId != INVALID_DEVID) {
                    ret = wc_CryptoCb_Rsa(in, inLen, out,
                                        outLen, rsa_type, key, rng);
                    if (ret != CRYPTOCB_UNAVAILABLE)
                      return ret;
                    /* fall-through when unavailable */
                    ret = 0; /* reset error code and try using software */
                }
           #endif

    #endif /* WOLFSSL_CRYPTOCELL */


#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE) && \
    !defined(WOLFSSL_NO_MALLOC)
        /* verify the tmp ptr is NULL, otherwise indicates bad state */
        if (key->data != NULL) {
            ret = BAD_STATE_E;
            break;
        }

        /* if not doing this inline then allocate a buffer for it */
        if (outPtr == NULL) {
            key->data = (byte*)XMALLOC(inLen, key->heap,
                                                      DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 1;
            if (key->data == NULL) {
                ret = MEMORY_E;
                break;
            }
            XMEMCPY(key->data, in, inLen);
        }
        else {
            key->dataIsAlloc = 0;
            key->data = out;
        }
#endif

        key->state = RSA_STATE_DECRYPT_EXPTMOD;
        FALL_THROUGH;

    case RSA_STATE_DECRYPT_EXPTMOD:
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE) && \
    !defined(WOLFSSL_NO_MALLOC)
        ret = wc_RsaFunction_ex(key->data, inLen, key->data, &key->dataLen,
                                                   rsa_type, key, rng,
                                                   pad_type != WC_RSA_OAEP_PAD);
#else
        ret = wc_RsaFunction_ex(in, inLen, out, &key->dataLen, rsa_type, key,
                                              rng, pad_type != WC_RSA_OAEP_PAD);
#endif

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_DECRYPT_UNPAD;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_DECRYPT_UNPAD:
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE) && \
    !defined(WOLFSSL_NO_MALLOC)
        ret = wc_RsaUnPad_ex(key->data, key->dataLen, &pad, pad_value, pad_type,
                             hash, mgf, label, labelSz, saltLen,
                             mp_count_bits(&key->n), key->heap);
#else
        ret = wc_RsaUnPad_ex(out, key->dataLen, &pad, pad_value, pad_type, hash,
                             mgf, label, labelSz, saltLen,
                             mp_count_bits(&key->n), key->heap);
#endif
        if (rsa_type == RSA_PUBLIC_DECRYPT && ret > (int)outLen)
            ret = RSA_BUFFER_E;
        else if (ret >= 0 && pad != NULL) {
            /* only copy output if not inline */
            if (outPtr == NULL) {
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_VERIFY_INLINE) && \
    !defined(WOLFSSL_NO_MALLOC)
                if (rsa_type == RSA_PRIVATE_DECRYPT) {
                    word32 i = 0;
                    word32 j;
                    int start = (int)((size_t)pad - (size_t)key->data);

                    for (j = 0; j < key->dataLen; j++) {
                        signed char c;
                        out[i] = key->data[j];
                        c  = (signed char)ctMaskGTE((int)j, start);
                        c &= (signed char)ctMaskLT((int)i, (int)outLen);
                        /* 0 - no add, -1 add */
                        i += (word32)((byte)(-c));
                    }
                }
                else
#endif
                {
                    XMEMCPY(out, pad, (size_t)ret);
                }
            }
            else
                *outPtr = pad;

#if !defined(WOLFSSL_RSA_VERIFY_ONLY)
            ret = ctMaskSelInt(ctMaskLTE(ret, (int)outLen), ret, RSA_BUFFER_E);
    #ifndef WOLFSSL_RSA_DECRYPT_TO_0_LEN
            ret = ctMaskSelInt(ctMaskNotEq(ret, 0), ret, RSA_BUFFER_E);
    #endif
#else
            if (outLen < (word32)ret)
                ret = RSA_BUFFER_E;
#endif
        }

        key->state = RSA_STATE_DECRYPT_RES;
        FALL_THROUGH;

    case RSA_STATE_DECRYPT_RES:
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
            ret = key->asyncDev.event.ret;
            if (ret >= 0) {
                /* convert result */
                byte* dataLen = (byte*)&key->dataLen;
                ret = (dataLen[0] << 8) | (dataLen[1]);

                if (outPtr)
                    *outPtr = in;
            }
        }
    #endif
        break;

    default:
        ret = BAD_STATE_E;
        break;
    }

    /* if async pending then return and skip done cleanup below */
    if (ret == WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        || ret == FP_WOULDBLOCK
    #endif
    ) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}


#ifndef WOLFSSL_RSA_VERIFY_ONLY
/* Public RSA Functions */
int wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                     RsaKey* key, WC_RNG* rng)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PUBLIC_ENCRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}


#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_NO_PADDING)
int wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                    word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                    enum wc_HashType hash, int mgf, byte* label,
                    word32 labelSz)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPublicEncryptEx(in, inLen, out, outLen, key, RSA_PUBLIC_ENCRYPT,
        RSA_BLOCK_TYPE_2, type, hash, mgf, label, labelSz, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif /* WC_NO_RSA_OAEP */
#endif


#ifndef WOLFSSL_RSA_PUBLIC_ONLY
int wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}


#ifndef WC_NO_RSA_OAEP
int wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen, byte** out,
                                  RsaKey* key, int type, enum wc_HashType hash,
                                  int mgf, byte* label, word32 labelSz)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, type, hash,
        mgf, label, labelSz, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif /* WC_NO_RSA_OAEP */


int wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                                 word32 outLen, RsaKey* key)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}

#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_NO_PADDING)
int wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int type,
                            enum wc_HashType hash, int mgf, byte* label,
                            word32 labelSz)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, type, hash, mgf, label,
        labelSz, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif /* WC_NO_RSA_OAEP || WC_RSA_NO_PADDING */
#endif /* WOLFSSL_RSA_PUBLIC_ONLY */

#if !defined(WOLFSSL_CRYPTOCELL)
int wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif

#ifndef WOLFSSL_RSA_VERIFY_ONLY
int wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                                 RsaKey* key)
{
    return wc_RsaSSL_Verify_ex(in, inLen, out, outLen, key, WC_RSA_PKCSV15_PAD);
}

int  wc_RsaSSL_Verify_ex(const byte* in, word32 inLen, byte* out, word32 outLen,
                         RsaKey* key, int pad_type)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = wc_RsaSSL_Verify_ex2(in, inLen, out, outLen, key, pad_type,
            WC_HASH_TYPE_NONE);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}

int  wc_RsaSSL_Verify_ex2(const byte* in, word32 inLen, byte* out, word32 outLen,
                         RsaKey* key, int pad_type, enum wc_HashType hash)
{
    WC_RNG* rng;
    int ret;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    ret = RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, pad_type,
        hash, wc_hash2mgf(hash), NULL, 0, RSA_PSS_SALT_LEN_DEFAULT, rng);
#else
    ret = RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, pad_type,
        hash, wc_hash2mgf(hash), NULL, 0, RSA_PSS_SALT_LEN_DISCOVER, rng);
#endif
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif

#ifdef WC_RSA_PSS
/* Verify the message signed with RSA-PSS.
 * The input buffer is reused for the output buffer.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                           enum wc_HashType hash, int mgf, RsaKey* key)
{
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    return wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf,
                                                 RSA_PSS_SALT_LEN_DEFAULT, key);
#else
    return wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf,
                                                RSA_PSS_SALT_LEN_DISCOVER, key);
#endif
}

/* Verify the message signed with RSA-PSS.
 * The input buffer is reused for the output buffer.
 *
 * in       Buffer holding encrypted data.
 * inLen    Length of data in buffer.
 * out      Pointer to address containing the PSS data.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * key      Public RSA key.
 * saltLen  Length of salt used. RSA_PSS_SALT_LEN_DEFAULT (-1) indicates salt
 *          length is the same as the hash length. RSA_PSS_SALT_LEN_DISCOVER
 *          indicates salt length is determined from the data.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyInline_ex(byte* in, word32 inLen, byte** out,
                              enum wc_HashType hash, int mgf, int saltLen,
                              RsaKey* key)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}

/* Verify the message signed with RSA-PSS.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out, word32 outLen,
                     enum wc_HashType hash, int mgf, RsaKey* key)
{
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    return wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash, mgf,
                                                 RSA_PSS_SALT_LEN_DEFAULT, key);
#else
    return wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash, mgf,
                                                RSA_PSS_SALT_LEN_DISCOVER, key);
#endif
}

/* Verify the message signed with RSA-PSS.
 *
 * in       Buffer holding encrypted data.
 * inLen    Length of data in buffer.
 * out      Pointer to address containing the PSS data.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * key      Public RSA key.
 * saltLen  Length of salt used. RSA_PSS_SALT_LEN_DEFAULT (-1) indicates salt
 *          length is the same as the hash length. RSA_PSS_SALT_LEN_DISCOVER
 *          indicates salt length is determined from the data.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_Verify_ex(byte* in, word32 inLen, byte* out, word32 outLen,
                        enum wc_HashType hash, int mgf, int saltLen,
                        RsaKey* key)
{
    WC_RNG* rng;
    int ret;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#else
    rng = NULL;
#endif
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}


/* Checks the PSS data to ensure that the signature matches.
 * Salt length is equal to hash length.
 *
 * in        Hash of the data that is being verified.
 * inSz      Length of hash.
 * sig       Buffer holding PSS data.
 * sigSz     Size of PSS data.
 * hashType  Hash algorithm.
 * returns BAD_PADDING_E when the PSS data is invalid, BAD_FUNC_ARG when
 * NULL is passed in to in or sig or inSz is not the same as the hash
 * algorithm length and 0 on success.
 */
int wc_RsaPSS_CheckPadding(const byte* in, word32 inSz, byte* sig,
                           word32 sigSz, enum wc_HashType hashType)
{
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    return wc_RsaPSS_CheckPadding_ex(in, inSz, sig, sigSz, hashType, RSA_PSS_SALT_LEN_DEFAULT, 0);
#else
    return wc_RsaPSS_CheckPadding_ex(in, inSz, sig, sigSz, hashType, RSA_PSS_SALT_LEN_DISCOVER, 0);
#endif
}

/* Checks the PSS data to ensure that the signature matches.
 *
 * in        Hash of the data that is being verified.
 * inSz      Length of hash.
 * sig       Buffer holding PSS data.
 * sigSz     Size of PSS data.
 * hashType  Hash algorithm.
 * saltLen   Length of salt used. RSA_PSS_SALT_LEN_DEFAULT (-1) indicates salt
 *           length is the same as the hash length. RSA_PSS_SALT_LEN_DISCOVER
 *           indicates salt length is determined from the data.
 * bits      Can be used to calculate salt size in FIPS case
 * returns BAD_PADDING_E when the PSS data is invalid, BAD_FUNC_ARG when
 * NULL is passed in to in or sig or inSz is not the same as the hash
 * algorithm length and 0 on success.
 */
int wc_RsaPSS_CheckPadding_ex2(const byte* in, word32 inSz, byte* sig,
                               word32 sigSz, enum wc_HashType hashType,
                               int saltLen, int bits, void* heap)
{
    int ret = 0;
    byte sigCheckBuf[WC_MAX_DIGEST_SIZE*2 + RSA_PSS_PAD_SZ];
    byte *sigCheck = sigCheckBuf;

    (void)bits;

    if (in == NULL || sig == NULL ||
                               inSz != (word32)wc_HashGetDigestSize(hashType)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
            saltLen = (int)inSz;
            #ifdef WOLFSSL_SHA512
                /* See FIPS 186-4 section 5.5 item (e). */
                if (bits == 1024 && inSz == WC_SHA512_DIGEST_SIZE) {
                    saltLen = RSA_PSS_SALT_MAX_SZ;
                }
            #endif
        }
#ifndef WOLFSSL_PSS_LONG_SALT
        else if (saltLen > (int)inSz) {
            ret = PSS_SALTLEN_E;
        }
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
        else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT) {
            ret = PSS_SALTLEN_E;
        }
#else
        else if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
            saltLen = sigSz - inSz;
            if (saltLen < 0) {
                ret = PSS_SALTLEN_E;
            }
        }
        else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER) {
            ret = PSS_SALTLEN_E;
        }
#endif
    }

    /* Sig = Salt | Exp Hash */
    if (ret == 0) {
        if (sigSz != inSz + (word32)saltLen) {
            ret = PSS_SALTLEN_E;
        }
    }

#ifdef WOLFSSL_PSS_LONG_SALT
    /* if long salt is larger then default maximum buffer then allocate a buffer */
    if ((ret == 0) &&
            (sizeof(sigCheckBuf) < (RSA_PSS_PAD_SZ + inSz + (word32)saltLen))) {
        sigCheck = (byte*)XMALLOC(
                              (size_t)(RSA_PSS_PAD_SZ + inSz + (word32)saltLen),
                              heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (sigCheck == NULL) {
            ret = MEMORY_E;
        }
    }
#else
    if (ret == 0 && sizeof(sigCheckBuf) < (RSA_PSS_PAD_SZ + inSz + (word32)saltLen)) {
        ret = BUFFER_E;
    }
#endif

    /* Exp Hash = HASH(8 * 0x00 | Message Hash | Salt) */
    if (ret == 0) {
        XMEMSET(sigCheck, 0, RSA_PSS_PAD_SZ);
        XMEMCPY(sigCheck + RSA_PSS_PAD_SZ, in, inSz);
        XMEMCPY(sigCheck + RSA_PSS_PAD_SZ + inSz, sig, (size_t)saltLen);
        ret = wc_Hash(hashType, sigCheck, RSA_PSS_PAD_SZ + inSz + (word32)saltLen,
                      sigCheck, inSz);
    }
    if (ret == 0) {
        if (XMEMCMP(sigCheck, sig + saltLen, inSz) != 0) {
            WOLFSSL_MSG("RsaPSS_CheckPadding: Padding Error");
            ret = BAD_PADDING_E;
        }
    }

#ifdef WOLFSSL_PSS_LONG_SALT
    if (sigCheck != NULL && sigCheck != sigCheckBuf) {
        XFREE(sigCheck, heap, DYNAMIC_TYPE_RSA_BUFFER);
    }
#endif

    (void)heap; /* unused if memory is disabled */
    return ret;
}
int wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inSz, byte* sig,
                               word32 sigSz, enum wc_HashType hashType,
                               int saltLen, int bits)
{
    return wc_RsaPSS_CheckPadding_ex2(in, inSz, sig, sigSz, hashType, saltLen,
        bits, NULL);
}


/* Verify the message signed with RSA-PSS.
 * The input buffer is reused for the output buffer.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * digest Hash of the data that is being verified.
 * digestLen Length of hash.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyCheckInline(byte* in, word32 inLen, byte** out,
                           const byte* digest, word32 digestLen,
                           enum wc_HashType hash, int mgf, RsaKey* key)
{
    int ret = 0, verify, saltLen, hLen, bits = 0;

    hLen = wc_HashGetDigestSize(hash);
    if (hLen < 0)
        return BAD_FUNC_ARG;
    if ((word32)hLen != digestLen)
        return BAD_FUNC_ARG;

    saltLen = hLen;
    #ifdef WOLFSSL_SHA512
        /* See FIPS 186-4 section 5.5 item (e). */
        bits = mp_count_bits(&key->n);
        if (bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE)
            saltLen = RSA_PSS_SALT_MAX_SZ;
    #endif

    verify = wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf, saltLen, key);
    if (verify > 0)
        ret = wc_RsaPSS_CheckPadding_ex(digest, digestLen, *out, (word32)verify,
                                        hash, saltLen, bits);
    if (ret == 0)
        ret = verify;

    return ret;
}


/* Verify the message signed with RSA-PSS.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * outLen Length of the output.
 * digest Hash of the data that is being verified.
 * digestLen Length of hash.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyCheck(byte* in, word32 inLen, byte* out, word32 outLen,
                          const byte* digest, word32 digestLen,
                          enum wc_HashType hash, int mgf,
                          RsaKey* key)
{
    int ret = 0, verify, saltLen, hLen, bits = 0;

    hLen = wc_HashGetDigestSize(hash);
    if (hLen < 0)
        return hLen;
    if ((word32)hLen != digestLen)
        return BAD_FUNC_ARG;

    saltLen = hLen;
    #ifdef WOLFSSL_SHA512
        /* See FIPS 186-4 section 5.5 item (e). */
        bits = mp_count_bits(&key->n);
        if (bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE)
            saltLen = RSA_PSS_SALT_MAX_SZ;
    #endif

    verify = wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash,
                                 mgf, saltLen, key);
    if (verify > 0)
        ret = wc_RsaPSS_CheckPadding_ex(digest, digestLen, out, (word32)verify,
                                        hash, saltLen, bits);
    if (ret == 0)
        ret = verify;

    return ret;
}

#endif

#if !defined(WOLFSSL_RSA_PUBLIC_ONLY) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
int wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                   RsaKey* key, WC_RNG* rng)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}

#ifdef WC_RSA_PSS
/* Sign the hash of a message using RSA-PSS.
 * Salt length is equal to hash length.
 *
 * in      Buffer holding hash of message.
 * inLen   Length of data in buffer (hash length).
 * out     Buffer to write encrypted signature into.
 * outLen  Size of buffer to write to.
 * hash    Hash algorithm.
 * mgf     Mask generation function.
 * key     Public RSA key.
 * rng     Random number generator.
 * returns the length of the encrypted signature on success, a negative value
 * indicates failure.
 */
int wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                       enum wc_HashType hash, int mgf, RsaKey* key, WC_RNG* rng)
{
    return wc_RsaPSS_Sign_ex(in, inLen, out, outLen, hash, mgf,
                                            RSA_PSS_SALT_LEN_DEFAULT, key, rng);
}

/* Sign the hash of a message using RSA-PSS.
 *
 * in       Buffer holding hash of message.
 * inLen    Length of data in buffer (hash length).
 * out      Buffer to write encrypted signature into.
 * outLen   Size of buffer to write to.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * saltLen  Length of salt used. RSA_PSS_SALT_LEN_DEFAULT (-1) indicates salt
 *          length is the same as the hash length. RSA_PSS_SALT_LEN_DISCOVER
 *          indicates salt length is determined from the data.
 * key      Public RSA key.
 * rng      Random number generator.
 * returns the length of the encrypted signature on success, a negative value
 * indicates failure.
 */
int wc_RsaPSS_Sign_ex(const byte* in, word32 inLen, byte* out, word32 outLen,
                      enum wc_HashType hash, int mgf, int saltLen, RsaKey* key,
                      WC_RNG* rng)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}
#endif
#endif

int wc_RsaEncryptSize(const RsaKey* key)
{
    int ret;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = mp_unsigned_bin_size(&key->n);

#ifdef WOLF_CRYPTO_CB
    if (ret == 0 && key->devId != INVALID_DEVID) {
        if (wc_CryptoCb_RsaGetSize(key, &ret) == CRYPTOCB_UNAVAILABLE) {
            ret = 2048/8; /* hardware handles, use 2048-bit as default */
        }
    }
#endif

    return ret;
}

#ifndef WOLFSSL_RSA_VERIFY_ONLY
/* flatten RsaKey structure into individual elements (e, n) */
int wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz, byte* n,
                                                                   word32* nSz)
{
    int sz, ret;

    if (key == NULL || e == NULL || eSz == NULL || n == NULL || nSz == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = mp_unsigned_bin_size(&key->e);
    if ((word32)sz > *eSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->e, e);
    if (ret != MP_OKAY)
        return ret;
    *eSz = (word32)sz;

    sz = wc_RsaEncryptSize(key);
    if ((word32)sz > *nSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->n, n);
    if (ret != MP_OKAY)
        return ret;
    *nSz = (word32)sz;

    return 0;
}
#endif

#ifndef WOLFSSL_RSA_VERIFY_ONLY
static int RsaGetValue(mp_int* in, byte* out, word32* outSz)
{
    word32 sz;
    int ret = 0;

    /* Parameters ensured by calling function. */

    sz = (word32)mp_unsigned_bin_size(in);
    if (sz > *outSz)
        ret = RSA_BUFFER_E;

    if (ret == 0)
        ret = mp_to_unsigned_bin(in, out);

    if (ret == MP_OKAY)
        *outSz = sz;

    return ret;
}


int wc_RsaExportKey(RsaKey* key,
                    byte* e, word32* eSz, byte* n, word32* nSz,
                    byte* d, word32* dSz, byte* p, word32* pSz,
                    byte* q, word32* qSz)
{
    int ret = BAD_FUNC_ARG;

    if (key && e && eSz && n && nSz && d && dSz && p && pSz && q && qSz)
        ret = 0;

    if (ret == 0)
        ret = RsaGetValue(&key->e, e, eSz);
    if (ret == 0)
        ret = RsaGetValue(&key->n, n, nSz);
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    if (ret == 0)
        ret = RsaGetValue(&key->d, d, dSz);
    if (ret == 0)
        ret = RsaGetValue(&key->p, p, pSz);
    if (ret == 0)
        ret = RsaGetValue(&key->q, q, qSz);
#else
    /* no private parts to key */
    if (d == NULL || p == NULL || q == NULL || dSz == NULL || pSz == NULL
            || qSz == NULL) {
        ret = BAD_FUNC_ARG;
    }
    else {
        *dSz = 0;
        *pSz = 0;
        *qSz = 0;
    }
#endif /* WOLFSSL_RSA_PUBLIC_ONLY */

    return ret;
}
#endif


#ifdef WOLFSSL_KEY_GEN

/* Check that |p-q| > 2^((size/2)-100) */
static int wc_CompareDiffPQ(mp_int* p, mp_int* q, int size, int* valid)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *c = NULL, *d = NULL;
#else
    mp_int c[1], d[1];
#endif
    int ret;

    if (p == NULL || q == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    if (((c = (mp_int *)XMALLOC(sizeof(*c), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL) ||
        ((d = (mp_int *)XMALLOC(sizeof(*d), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL))
        ret = MEMORY_E;
    else
        ret = 0;

    if (ret == 0)
#endif
        ret = mp_init_multi(c, d, NULL, NULL, NULL, NULL);

    /* c = 2^((size/2)-100) */
    if (ret == 0)
        ret = mp_2expt(c, (size/2)-100);

    /* d = |p-q| */
    if (ret == 0)
        ret = mp_sub(p, q, d);

#ifdef WOLFSSL_CHECK_MEM_ZERO
    if (ret == 0)
        mp_memzero_add("Comare PQ d", d);
#endif

#if !defined(WOLFSSL_SP_MATH) && (!defined(WOLFSSL_SP_MATH_ALL) || \
                                               defined(WOLFSSL_SP_INT_NEGATIVE))
    if (ret == 0)
        ret = mp_abs(d, d);
#endif

    /* compare */
    if (ret == 0)
        *valid = (mp_cmp(d, c) == MP_GT);

#ifdef WOLFSSL_SMALL_STACK
    if (d != NULL) {
        mp_forcezero(d);
        XFREE(d, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
    if (c != NULL) {
        mp_clear(c);
        XFREE(c, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
#else
    mp_forcezero(d);
    mp_clear(c);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    mp_memzero_check(d);
#endif
#endif

    return ret;
}


/* The lower_bound value is floor(2^(0.5) * 2^((nlen/2)-1)) where nlen is 4096.
 * This number was calculated using a small test tool written with a common
 * large number math library. Other values of nlen may be checked with a subset
 * of lower_bound. */
static const byte lower_bound[] = {
    0xB5, 0x04, 0xF3, 0x33, 0xF9, 0xDE, 0x64, 0x84,
    0x59, 0x7D, 0x89, 0xB3, 0x75, 0x4A, 0xBE, 0x9F,
    0x1D, 0x6F, 0x60, 0xBA, 0x89, 0x3B, 0xA8, 0x4C,
    0xED, 0x17, 0xAC, 0x85, 0x83, 0x33, 0x99, 0x15,
/* 512 */
    0x4A, 0xFC, 0x83, 0x04, 0x3A, 0xB8, 0xA2, 0xC3,
    0xA8, 0xB1, 0xFE, 0x6F, 0xDC, 0x83, 0xDB, 0x39,
    0x0F, 0x74, 0xA8, 0x5E, 0x43, 0x9C, 0x7B, 0x4A,
    0x78, 0x04, 0x87, 0x36, 0x3D, 0xFA, 0x27, 0x68,
/* 1024 */
    0xD2, 0x20, 0x2E, 0x87, 0x42, 0xAF, 0x1F, 0x4E,
    0x53, 0x05, 0x9C, 0x60, 0x11, 0xBC, 0x33, 0x7B,
    0xCA, 0xB1, 0xBC, 0x91, 0x16, 0x88, 0x45, 0x8A,
    0x46, 0x0A, 0xBC, 0x72, 0x2F, 0x7C, 0x4E, 0x33,
    0xC6, 0xD5, 0xA8, 0xA3, 0x8B, 0xB7, 0xE9, 0xDC,
    0xCB, 0x2A, 0x63, 0x43, 0x31, 0xF3, 0xC8, 0x4D,
    0xF5, 0x2F, 0x12, 0x0F, 0x83, 0x6E, 0x58, 0x2E,
    0xEA, 0xA4, 0xA0, 0x89, 0x90, 0x40, 0xCA, 0x4A,
/* 2048 */
    0x81, 0x39, 0x4A, 0xB6, 0xD8, 0xFD, 0x0E, 0xFD,
    0xF4, 0xD3, 0xA0, 0x2C, 0xEB, 0xC9, 0x3E, 0x0C,
    0x42, 0x64, 0xDA, 0xBC, 0xD5, 0x28, 0xB6, 0x51,
    0xB8, 0xCF, 0x34, 0x1B, 0x6F, 0x82, 0x36, 0xC7,
    0x01, 0x04, 0xDC, 0x01, 0xFE, 0x32, 0x35, 0x2F,
    0x33, 0x2A, 0x5E, 0x9F, 0x7B, 0xDA, 0x1E, 0xBF,
    0xF6, 0xA1, 0xBE, 0x3F, 0xCA, 0x22, 0x13, 0x07,
    0xDE, 0xA0, 0x62, 0x41, 0xF7, 0xAA, 0x81, 0xC2,
/* 3072 */
    0xC1, 0xFC, 0xBD, 0xDE, 0xA2, 0xF7, 0xDC, 0x33,
    0x18, 0x83, 0x8A, 0x2E, 0xAF, 0xF5, 0xF3, 0xB2,
    0xD2, 0x4F, 0x4A, 0x76, 0x3F, 0xAC, 0xB8, 0x82,
    0xFD, 0xFE, 0x17, 0x0F, 0xD3, 0xB1, 0xF7, 0x80,
    0xF9, 0xAC, 0xCE, 0x41, 0x79, 0x7F, 0x28, 0x05,
    0xC2, 0x46, 0x78, 0x5E, 0x92, 0x95, 0x70, 0x23,
    0x5F, 0xCF, 0x8F, 0x7B, 0xCA, 0x3E, 0xA3, 0x3B,
    0x4D, 0x7C, 0x60, 0xA5, 0xE6, 0x33, 0xE3, 0xE1
/* 4096 */
};


/* returns 1 on key size ok and 0 if not ok */
static WC_INLINE int RsaSizeCheck(int size)
{
    if (size < RSA_MIN_SIZE || size > RSA_MAX_SIZE) {
        return 0;
    }

#ifdef HAVE_FIPS
    /* Key size requirements for CAVP */
    switch (size) {
        case 1024:
        case 2048:
        case 3072:
        case 4096:
            return 1;
    }

    return 0;
#else
    return 1; /* allow unusual key sizes in non FIPS mode */
#endif /* HAVE_FIPS */
}


static int _CheckProbablePrime(mp_int* p, mp_int* q, mp_int* e, int nlen,
                                    int* isPrime, WC_RNG* rng)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    mp_int *tmp1 = NULL, *tmp2 = NULL;
#else
    mp_int tmp1[1], tmp2[2];
#endif
    mp_int* prime;

    if (p == NULL || e == NULL || isPrime == NULL)
        return BAD_FUNC_ARG;

    if (!RsaSizeCheck(nlen))
        return BAD_FUNC_ARG;

    *isPrime = MP_NO;

#ifdef WOLFSSL_SMALL_STACK
    if (((tmp1 = (mp_int *)XMALLOC(sizeof(*tmp1), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL) ||
        ((tmp2 = (mp_int *)XMALLOC(sizeof(*tmp2), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL)) {
        ret = MEMORY_E;
        goto notOkay;
    }
#endif

    ret = mp_init_multi(tmp1, tmp2, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY) goto notOkay;

    if (q != NULL) {
        int valid = 0;
        /* 5.4 - check that |p-q| <= (2^(1/2))(2^((nlen/2)-1)) */
        ret = wc_CompareDiffPQ(p, q, nlen, &valid);
        if ((ret != MP_OKAY) || (!valid)) goto notOkay;
        prime = q;
    }
    else
        prime = p;

    /* 4.4,5.5 - Check that prime >= (2^(1/2))(2^((nlen/2)-1))
     *           This is a comparison against lowerBound */
    ret = mp_read_unsigned_bin(tmp1, lower_bound, (word32)nlen/16);
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp(prime, tmp1);
    if (ret == MP_LT) goto exit;

    /* 4.5,5.6 - Check that GCD(p-1, e) == 1 */
    ret = mp_sub_d(prime, 1, tmp1);  /* tmp1 = prime-1 */
    if (ret != MP_OKAY) goto notOkay;
#ifdef WOLFSSL_CHECK_MEM_ZERO
    mp_memzero_add("Check Probable Prime tmp1", tmp1);
#endif
    ret = mp_gcd(tmp1, e, tmp2);  /* tmp2 = gcd(prime-1, e) */
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp_d(tmp2, 1);
    if (ret != MP_EQ) goto exit; /* e divides p-1 */

    /* 4.5.1,5.6.1 - Check primality of p with 8 rounds of M-R.
     * mp_prime_is_prime_ex() performs test divisions against the first 256
     * prime numbers. After that it performs 8 rounds of M-R using random
     * bases between 2 and n-2.
     * mp_prime_is_prime() performs the same test divisions and then does
     * M-R with the first 8 primes. Both functions set isPrime as a
     * side-effect. */
    if (rng != NULL)
        ret = mp_prime_is_prime_ex(prime, 8, isPrime, rng);
    else
        ret = mp_prime_is_prime(prime, 8, isPrime);
    if (ret != MP_OKAY) goto notOkay;

exit:
    ret = MP_OKAY;

notOkay:

#ifdef WOLFSSL_SMALL_STACK
    if (tmp1 != NULL) {
        mp_forcezero(tmp1);
        XFREE(tmp1, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
    if (tmp2 != NULL) {
        mp_clear(tmp2);
        XFREE(tmp2, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
#else
    mp_forcezero(tmp1);
    mp_clear(tmp2);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    mp_memzero_check(tmp1);
#endif
#endif

    return ret;
}


int wc_CheckProbablePrime_ex(const byte* pRaw, word32 pRawSz,
                          const byte* qRaw, word32 qRawSz,
                          const byte* eRaw, word32 eRawSz,
                          int nlen, int* isPrime, WC_RNG* rng)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *p = NULL, *q = NULL, *e = NULL;
#else
    mp_int p[1], q[1], e[1];
#endif
    mp_int* Q = NULL;
    int ret;

    if (pRaw == NULL || pRawSz == 0 ||
        eRaw == NULL || eRawSz == 0 ||
        isPrime == NULL) {

        return BAD_FUNC_ARG;
    }

    if ((qRaw != NULL && qRawSz == 0) || (qRaw == NULL && qRawSz != 0))
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK

    if (((p = (mp_int *)XMALLOC(sizeof(*p), NULL, DYNAMIC_TYPE_RSA_BUFFER)) == NULL) ||
        ((q = (mp_int *)XMALLOC(sizeof(*q), NULL, DYNAMIC_TYPE_RSA_BUFFER)) == NULL) ||
        ((e = (mp_int *)XMALLOC(sizeof(*e), NULL, DYNAMIC_TYPE_RSA_BUFFER)) == NULL))
        ret = MEMORY_E;
    else
        ret = 0;
    if (ret == 0)
#endif
        ret = mp_init_multi(p, q, e, NULL, NULL, NULL);

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(p, pRaw, pRawSz);

    if (ret == MP_OKAY) {
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        mp_memzero_add("wc_CheckProbablePrime_ex p", p);
    #endif
        if (qRaw != NULL) {
            ret = mp_read_unsigned_bin(q, qRaw, qRawSz);
            if (ret == MP_OKAY) {
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                mp_memzero_add("wc_CheckProbablePrime_ex q", q);
            #endif
                Q = q;
            }
        }
    }

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(e, eRaw, eRawSz);

    if (ret == MP_OKAY) {
        SAVE_VECTOR_REGISTERS(ret = _svr_ret;);

        if (ret == MP_OKAY)
            ret = _CheckProbablePrime(p, Q, e, nlen, isPrime, rng);

        RESTORE_VECTOR_REGISTERS();
    }

    ret = (ret == MP_OKAY) ? 0 : PRIME_GEN_E;

#ifdef WOLFSSL_SMALL_STACK
    if (p != NULL) {
        mp_forcezero(p);
        XFREE(p, NULL, DYNAMIC_TYPE_RSA_BUFFER);
    }
    if (q != NULL) {
        mp_forcezero(q);
        XFREE(q, NULL, DYNAMIC_TYPE_RSA_BUFFER);
    }
    if (e != NULL) {
        mp_clear(e);
        XFREE(e, NULL, DYNAMIC_TYPE_RSA_BUFFER);
    }
#else
    mp_forcezero(p);
    mp_forcezero(q);
    mp_clear(e);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    mp_memzero_check(p);
    mp_memzero_check(q);
#endif
#endif

    return ret;
}


int wc_CheckProbablePrime(const byte* pRaw, word32 pRawSz,
                          const byte* qRaw, word32 qRawSz,
                          const byte* eRaw, word32 eRawSz,
                          int nlen, int* isPrime)
{
    return wc_CheckProbablePrime_ex(pRaw, pRawSz, qRaw, qRawSz,
                          eRaw, eRawSz, nlen, isPrime, NULL);
}

#if !defined(HAVE_FIPS) || (defined(HAVE_FIPS) && \
        defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
/* Make an RSA key for size bits, with e specified, 65537 is a good e */
int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
#ifndef WC_NO_RNG
#if !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SE050)
#ifdef WOLFSSL_SMALL_STACK
    mp_int *p = NULL;
    mp_int *q = NULL;
    mp_int *tmp1 = NULL;
    mp_int *tmp2 = NULL;
    mp_int *tmp3 = NULL;
#else
    mp_int p_buf, *p = &p_buf;
    mp_int q_buf, *q = &q_buf;
    mp_int tmp1_buf, *tmp1 = &tmp1_buf;
    mp_int tmp2_buf, *tmp2 = &tmp2_buf;
    mp_int tmp3_buf, *tmp3 = &tmp3_buf;
#endif /* WOLFSSL_SMALL_STACK */
    int i, failCount, isPrime = 0;
    word32 primeSz;
    byte* buf = NULL;
#endif /* !WOLFSSL_CRYPTOCELL && !WOLFSSL_SE050 */
    int err;

    if (key == NULL || rng == NULL) {
        err = BAD_FUNC_ARG;
        goto out;
    }

    if (!RsaSizeCheck(size)) {
        err = BAD_FUNC_ARG;
        goto out;
    }

    if (e < 3 || (e & 1) == 0) {
        err = BAD_FUNC_ARG;
        goto out;
    }

#if defined(WOLFSSL_CRYPTOCELL)
    err = cc310_RSA_GenerateKeyPair(key, size, e);
    goto out;
#elif defined(WOLFSSL_SE050)
    err = se050_rsa_create_key(key, size, e);
    goto out;
#else
    /* software crypto */

#ifdef WOLFSSL_SMALL_STACK
    p = (mp_int *)XMALLOC(sizeof *p, key->heap, DYNAMIC_TYPE_RSA);
    q = (mp_int *)XMALLOC(sizeof *q, key->heap, DYNAMIC_TYPE_RSA);
    tmp1 = (mp_int *)XMALLOC(sizeof *tmp1, key->heap, DYNAMIC_TYPE_RSA);
    tmp2 = (mp_int *)XMALLOC(sizeof *tmp2, key->heap, DYNAMIC_TYPE_RSA);
    tmp3 = (mp_int *)XMALLOC(sizeof *tmp3, key->heap, DYNAMIC_TYPE_RSA);

    if ((p == NULL) ||
        (q == NULL) ||
        (tmp1 == NULL) ||
        (tmp2 == NULL) ||
        (tmp3 == NULL)) {
      err = MEMORY_E;
      goto out;
    }
#endif
#ifdef WOLFSSL_CHECK_MEM_ZERO
    XMEMSET(p, 0, sizeof(*p));
    XMEMSET(q, 0, sizeof(*q));
    XMEMSET(tmp1, 0, sizeof(*tmp1));
    XMEMSET(tmp2, 0, sizeof(*tmp2));
    XMEMSET(tmp3, 0, sizeof(*tmp3));
#endif

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (key->devId != INVALID_DEVID)
    #endif
    {
        err = wc_CryptoCb_MakeRsaKey(key, size, e, rng);
        #ifndef WOLF_CRYPTO_CB_ONLY_RSA
        if (err != CRYPTOCB_UNAVAILABLE)
            goto out;
        /* fall-through when unavailable */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_RSA
        if (err == CRYPTOCB_UNAVAILABLE)
            err = NO_VALID_DEVID;
            goto out;
        }
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_RSA
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
    defined(WC_ASYNC_ENABLE_RSA_KEYGEN)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA) {
    #ifdef HAVE_CAVIUM
        /* TODO: Not implemented */
    #elif defined(HAVE_INTEL_QA)
        err = IntelQaRsaKeyGen(&key->asyncDev, key, size, e, rng);
        goto out;
    #else
        if (wc_AsyncSwInit(&key->asyncDev, ASYNC_SW_RSA_MAKE)) {
            WC_ASYNC_SW* sw = &key->asyncDev.sw;
            sw->rsaMake.rng = rng;
            sw->rsaMake.key = key;
            sw->rsaMake.size = size;
            sw->rsaMake.e = e;
            err = WC_PENDING_E;
            goto out;
        }
    #endif
    }
#endif

    err = mp_init_multi(p, q, tmp1, tmp2, tmp3, NULL);

    if (err == MP_OKAY)
        err = mp_set_int(tmp3, (unsigned long)e);

    /* The failCount value comes from NIST FIPS 186-4, section B.3.3,
     * process steps 4.7 and 5.8. */
    failCount = 5 * (size / 2);
    primeSz = (word32)size / 16; /* size is the size of n in bits.
                            primeSz is in bytes. */

    /* allocate buffer to work with */
    if (err == MP_OKAY) {
        buf = (byte*)XMALLOC(primeSz, key->heap, DYNAMIC_TYPE_RSA);
        if (buf == NULL)
            err = MEMORY_E;
    }

    SAVE_VECTOR_REGISTERS(err = _svr_ret;);

    /* make p */
    if (err == MP_OKAY) {
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Add("RSA gen buf", buf, primeSz);
        mp_memzero_add("RSA gen p", p);
        mp_memzero_add("RSA gen q", q);
        mp_memzero_add("RSA gen tmp1", tmp1);
        mp_memzero_add("RSA gen tmp2", tmp2);
        mp_memzero_add("RSA gen tmp3", tmp3);
    #endif
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);
            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(p, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = _CheckProbablePrime(p, NULL, tmp3, size, &isPrime, rng);

#ifdef HAVE_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    /* make q */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);
            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(q, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = _CheckProbablePrime(p, q, tmp3, size, &isPrime, rng);

#ifndef WC_RSA_NO_FERMAT_CHECK
            if (err == MP_OKAY && isPrime) {
                /* Fermat's Factorization works when difference between p and q
                 * is less than (conservatively):
                 *     n^(1/4) + 32
                 *  ~= 2^(bit count of n)^(1/4) + 32)
                 *   = 2^((bit count of n)/4 + 32)
                 */
                err = mp_sub(p, q, tmp1);
                if (err == MP_OKAY && mp_count_bits(tmp1) <= (size / 4) + 32) {
                    isPrime = 0;
                }
            }
#endif

#ifdef HAVE_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    if (buf) {
        ForceZero(buf, primeSz);
        XFREE(buf, key->heap, DYNAMIC_TYPE_RSA);
    }

    if (err == MP_OKAY && mp_cmp(p, q) < 0) {
        err = mp_copy(p, tmp1);
        if (err == MP_OKAY)
            err = mp_copy(q, p);
        if (err == MP_OKAY)
            mp_copy(tmp1, q);
    }

    /* Setup RsaKey buffers */
    if (err == MP_OKAY)
        err = mp_init_multi(&key->n, &key->e, &key->d, &key->p, &key->q, NULL);
    if (err == MP_OKAY)
        err = mp_init_multi(&key->dP, &key->dQ, &key->u, NULL, NULL, NULL);

    /* Software Key Calculation */
    if (err == MP_OKAY)                /* tmp1 = p-1 */
        err = mp_sub_d(p, 1, tmp1);
    if (err == MP_OKAY)                /* tmp2 = q-1 */
        err = mp_sub_d(q, 1, tmp2);
#ifdef WC_RSA_BLINDING
    if (err == MP_OKAY)                /* tmp3 = order of n */
        err = mp_mul(tmp1, tmp2, tmp3);
#else
    if (err == MP_OKAY)                /* tmp3 = lcm(p-1, q-1), last loop */
        err = mp_lcm(tmp1, tmp2, tmp3);
#endif
    /* make key */
    if (err == MP_OKAY)                /* key->e = e */
        err = mp_set_int(&key->e, (unsigned long)e);
#ifdef WC_RSA_BLINDING
    /* Blind the inverse operation with a value that is invertable */
    if (err == MP_OKAY) {
        do {
            err = mp_rand(&key->p, get_digit_count(tmp3), rng);
            if (err == MP_OKAY)
                err = mp_set_bit(&key->p, 0);
            if (err == MP_OKAY)
                err = mp_set_bit(&key->p, size - 1);
            if (err == MP_OKAY)
                err = mp_gcd(&key->p, tmp3, &key->q);
        }
        while ((err == MP_OKAY) && !mp_isone(&key->q));
    }
    /* 8/16-bit word size requires a full multiply when e=0x10001 */
    if (err == MP_OKAY)
        err = mp_mul(&key->p, &key->e, &key->e);
#endif
    if (err == MP_OKAY)                /* key->d = 1/e mod lcm(p-1, q-1) */
        err = mp_invmod(&key->e, tmp3, &key->d);
#ifdef WC_RSA_BLINDING
    /* Take off blinding from d and reset e */
    if (err == MP_OKAY)
        err = mp_mulmod(&key->d, &key->p, tmp3, &key->d);
    if (err == MP_OKAY)
        err = mp_set_int(&key->e, (unsigned long)e);
#endif
    if (err == MP_OKAY)                /* key->n = pq */
        err = mp_mul(p, q, &key->n);
    if (err == MP_OKAY)                /* key->dP = d mod(p-1) */
        err = mp_mod(&key->d, tmp1, &key->dP);
    if (err == MP_OKAY)                /* key->dQ = d mod(q-1) */
        err = mp_mod(&key->d, tmp2, &key->dQ);
#ifdef WOLFSSL_MP_INVMOD_CONSTANT_TIME
    if (err == MP_OKAY)                /* key->u = 1/q mod p */
        err = mp_invmod(q, p, &key->u);
#else
    if (err == MP_OKAY)
        err = mp_sub_d(p, 2, tmp3);
    if (err == MP_OKAY)                /* key->u = 1/q mod p = q^p-2 mod p */
        err = mp_exptmod(q, tmp3, p, &key->u);
#endif
    if (err == MP_OKAY)
        err = mp_copy(p, &key->p);
    if (err == MP_OKAY)
        err = mp_copy(q, &key->q);

#ifdef HAVE_WOLF_BIGINT
    /* make sure raw unsigned bin version is available */
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->n, &key->n.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->e, &key->e.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->d, &key->d.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->p, &key->p.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->q, &key->q.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->dP, &key->dP.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->dQ, &key->dQ.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->u, &key->u.raw);
#endif

    if (err == MP_OKAY)
        key->type = RSA_PRIVATE;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    if (err == MP_OKAY) {
        mp_memzero_add("Make RSA key d", &key->d);
        mp_memzero_add("Make RSA key p", &key->p);
        mp_memzero_add("Make RSA key q", &key->q);
        mp_memzero_add("Make RSA key dP", &key->dP);
        mp_memzero_add("Make RSA key dQ", &key->dQ);
        mp_memzero_add("Make RSA key u", &key->u);
    }
#endif

    RESTORE_VECTOR_REGISTERS();

    /* Last value p - 1. */
    mp_forcezero(tmp1);
    /* Last value q - 1. */
    mp_forcezero(tmp2);
    /* Last value p - 2. */
    mp_forcezero(tmp3);
    mp_forcezero(p);
    mp_forcezero(q);

#ifdef WOLFSSL_RSA_KEY_CHECK
    /* Perform the pair-wise consistency test on the new key. */
    if (err == 0)
        err = _ifc_pairwise_consistency_test(key, rng);
#endif

    if (err != 0) {
        wc_FreeRsaKey(key);
        goto out;
    }

#if defined(WOLFSSL_XILINX_CRYPT) || defined(WOLFSSL_CRYPTOCELL)
    if (wc_InitRsaHw(key) != 0) {
        return BAD_STATE_E;
    }
#endif

    err = 0;
#endif /* WOLF_CRYPTO_CB_ONLY_RSA */
#endif /* WOLFSSL_CRYPTOCELL / SW only */
  out:

#if !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SE050)
#ifdef WOLFSSL_SMALL_STACK
    if (p)
        XFREE(p, key->heap, DYNAMIC_TYPE_RSA);
    if (q)
        XFREE(q, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp1)
        XFREE(tmp1, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp2)
        XFREE(tmp2, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp3)
        XFREE(tmp3, key->heap, DYNAMIC_TYPE_RSA);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    mp_memzero_check(p);
    mp_memzero_check(q);
    mp_memzero_check(tmp1);
    mp_memzero_check(tmp2);
    mp_memzero_check(tmp3);
#endif /* WOLFSSL_SMALL_STACK */
#endif /* !WOLFSSL_CRYPTOCELL && !WOLFSSL_SE050 */

    return err;

#else
    return NOT_COMPILED_IN;
#endif
}
#endif /* !FIPS || FIPS_VER >= 2 */
#endif /* WOLFSSL_KEY_GEN */


#ifdef WC_RSA_BLINDING
int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng)
{
    if (key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    key->rng = rng;

    return 0;
}
#endif /* WC_RSA_BLINDING */

#ifdef WC_RSA_NONBLOCK
int wc_RsaSetNonBlock(RsaKey* key, RsaNb* nb)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    if (nb) {
        XMEMSET(nb, 0, sizeof(RsaNb));
    }

    /* Allow nb == NULL to clear non-block mode */
    key->nb = nb;

    return 0;
}
#ifdef WC_RSA_NONBLOCK_TIME
int wc_RsaSetNonBlockTime(RsaKey* key, word32 maxBlockUs, word32 cpuMHz)
{
    if (key == NULL || key->nb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* calculate maximum number of instructions to block */
    key->nb->exptmod.maxBlockInst = cpuMHz * maxBlockUs;

    return 0;
}
#endif /* WC_RSA_NONBLOCK_TIME */
#endif /* WC_RSA_NONBLOCK */

#endif /* NO_RSA */
