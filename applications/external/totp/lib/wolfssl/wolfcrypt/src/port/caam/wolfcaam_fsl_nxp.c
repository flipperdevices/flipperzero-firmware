/* wolfcaam_fsl_nxp.c
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

#if defined(WOLFSSL_IMXRT1170_CAAM)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#if defined(FSL_FEATURE_HAS_L1CACHE) || defined(__DCACHE_PRESENT)
/* Setup for if memory is cached */
AT_NONCACHEABLE_SECTION(static caam_job_ring_interface_t jr0);
AT_NONCACHEABLE_SECTION(static caam_job_ring_interface_t jr1);
AT_NONCACHEABLE_SECTION(static caam_job_ring_interface_t jr2);
AT_NONCACHEABLE_SECTION(static caam_job_ring_interface_t jr3);
#else
/* If not cached */
static caam_job_ring_interface_t jr0;
static caam_job_ring_interface_t jr1;
static caam_job_ring_interface_t jr2;
static caam_job_ring_interface_t jr3;
#endif

static wolfSSL_Mutex caamMutex;

/* Initialize CAAM resources.
 * return 0 on success */
int wc_CAAMInitInterface()
{
    CAAM_Type *base = CAAM;
    caam_config_t caamConfig;

    if (wc_InitMutex(&caamMutex) != 0) {
        WOLFSSL_MSG("Could not init mutex");
        return -1;
    }

    /* Get default configuration. */
    CAAM_GetDefaultConfig(&caamConfig);

    /* set the job rings */
    caamConfig.jobRingInterface[0] = &jr0;
    caamConfig.jobRingInterface[1] = &jr1;
    caamConfig.jobRingInterface[2] = &jr2;
    caamConfig.jobRingInterface[3] = &jr3;

    /* Init CAAM driver, including CAAM's internal RNG */
    if (CAAM_Init(base, &caamConfig) != kStatus_Success) {
        return -1;
    }

    return 0;
}


/* free up CAAM resources */
void wc_CAAMFreeInterface()
{
    wc_FreeMutex(&caamMutex);
    if (CAAM_Deinit(CAAM) != kStatus_Success) {
        WOLFSSL_MSG("Failed to deinit CAAM!");
    }
}


#if !defined(NO_SHA) || defined(WOLFSSL_SHA224) || !defined(NO_SHA256) \
    || defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
/* convert the wolfCrypt hash type to NXP enum */
static caam_hash_algo_t WC2NXP(int type)
{
    switch (type) {
        case WC_HASH_TYPE_SHA:
            return kCAAM_Sha1;
        case WC_HASH_TYPE_SHA224:
            return kCAAM_Sha224;
        case WC_HASH_TYPE_SHA256:
            return kCAAM_Sha256;
        case WC_HASH_TYPE_SHA384:
            return kCAAM_Sha384;
        case WC_HASH_TYPE_SHA512:
            return kCAAM_Sha512;
        default:
            return -1;
    }
}

/* Common init code for hash algorithms.
 * returns 0 on success
 */
int wc_CAAM_HashInit(caam_handle_t* hndl, caam_hash_ctx_t* ctx, int type)
{
    if (hndl == NULL || ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    /* only using job ring0 for now */
    hndl->jobRing = kCAAM_JobRing0;
    if (CAAM_HASH_Init(CAAM, hndl, ctx, WC2NXP(type), NULL, 0u)
            != kStatus_Success) {
        return WC_HW_E;
    }
    return 0;
}


/* All hashing algorithms have common code except the 'type' to perform.
 * This helper function implements the common code.
 * returns 0 on success
 */
static int wc_CAAM_CommonHash(caam_handle_t* hndl, caam_hash_ctx_t *ctx,
    const byte* in, int inSz, byte* digest, size_t digestSz, int hashType)
{
    int ret = 0;
    status_t status;

    if (in != NULL && inSz > 0) {
        byte *alignedIn  = NULL;
        byte *tmpIn  = NULL;

        if ((wc_ptr_t)in % CAAM_BUFFER_ALIGN) {
            /* input not aligned */
            tmpIn = (byte*)XMALLOC(inSz + CAAM_BUFFER_ALIGN, NULL,
                DYNAMIC_TYPE_TMP_BUFFER);
            alignedIn = tmpIn + (CAAM_BUFFER_ALIGN -
                ((wc_ptr_t)tmpIn % CAAM_BUFFER_ALIGN));
            XMEMCPY(alignedIn, in, inSz);
        }
        else {
            alignedIn = (byte*)in;
        }

        status = CAAM_HASH_Update(ctx, alignedIn, inSz);
        if (tmpIn != NULL) {
            XFREE(tmpIn, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
        if (status != kStatus_Success) {
            return WC_HW_E;
        }
    }

    if (digest != NULL) {
        byte *tmpOut = NULL;

        byte *alignedOut = NULL;
        size_t sz = digestSz;

        if ((wc_ptr_t)digest % CAAM_BUFFER_ALIGN) {
            /* input not aligned */
            tmpOut = (byte*)XMALLOC(sz + CAAM_BUFFER_ALIGN, NULL,
                DYNAMIC_TYPE_TMP_BUFFER);
            alignedOut = tmpOut + (CAAM_BUFFER_ALIGN -
                ((wc_ptr_t)tmpOut % CAAM_BUFFER_ALIGN));
        }
        else {
            alignedOut = digest;
        }

        status = CAAM_HASH_Finish(ctx, alignedOut, &sz);
        if (tmpOut != NULL) {
            XMEMCPY(digest, alignedOut, sz);
            XFREE(tmpOut, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
        if (status != kStatus_Success) {
             WOLFSSL_MSG("Failed on CAAM_HASH_Finish");
             return WC_HW_E;
         }
        else {
             ret = wc_CAAM_HashInit(hndl, ctx, hashType);
        }
    }

    return ret;
}
#endif


#if !defined(NO_SHA)
/* SHA returns 0 on success */
int wc_CAAM_ShaHash(wc_Sha* sha, const byte* in, word32 inSz, byte* digest)
{
    return wc_CAAM_CommonHash(&sha->hndl, &sha->ctx, in, inSz, digest,
        WC_SHA_DIGEST_SIZE, WC_HASH_TYPE_SHA);

}
#endif

#ifdef WOLFSSL_SHA224
/* SHA224 returns 0 on success */
int wc_CAAM_Sha224Hash(wc_Sha224* sha, const byte* in, word32 inSz,
    byte* digest)
{
    return wc_CAAM_CommonHash(&sha->hndl, &sha->ctx, in, inSz, digest,
        WC_SHA224_DIGEST_SIZE, WC_HASH_TYPE_SHA224);
}
#endif

#if !defined(NO_SHA256)
/* SHA256 returns 0 on success */
int wc_CAAM_Sha256Hash(wc_Sha256* sha, const byte* in, word32 inSz,
    byte* digest)
{
    return wc_CAAM_CommonHash(&sha->hndl, &sha->ctx, in, inSz, digest,
        WC_SHA256_DIGEST_SIZE, WC_HASH_TYPE_SHA256);
}
#endif

#ifdef WOLFSSL_SHA384
/* SHA384 returns 0 on success */
int wc_CAAM_Sha384Hash(wc_Sha384* sha, const byte* in, word32 inSz,
    byte* digest)
{
    return wc_CAAM_CommonHash(&sha->hndl, &sha->ctx, in, inSz, digest,
        WC_SHA384_DIGEST_SIZE, WC_HASH_TYPE_SHA384);

}
#endif

#ifdef WOLFSSL_SHA512
/* SHA512 returns 0 on success */
int wc_CAAM_Sha512Hash(wc_Sha512* sha, const byte* in, word32 inSz,
    byte* digest)
{
    return wc_CAAM_CommonHash(&sha->hndl, &sha->ctx, in, inSz, digest,
        WC_SHA512_DIGEST_SIZE, WC_HASH_TYPE_SHA512);
}
#endif

#ifndef NO_AES_CBC
/* AES-CBC returns 0 on success */
static int DoAesCBC(unsigned int args[4], CAAM_BUFFER *buf, int sz)
{
    status_t status;
    caam_handle_t hndl;

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    if (args[0] == CAAM_DEC) {
        status = CAAM_AES_DecryptCbc(CAAM, &hndl,
            (const uint8_t*)buf[2].TheAddress, (uint8_t*)buf[3].TheAddress,
            buf[3].Length, (const uint8_t*)buf[4].TheAddress,
            (const uint8_t*)buf[0].TheAddress, buf[0].Length);

        /* store updated CBC state */
        XMEMCPY((byte*)buf[4].TheAddress,
                (byte*)buf[2].TheAddress + buf[2].Length - AES_BLOCK_SIZE,
                AES_BLOCK_SIZE);
    }
    else {
        status = CAAM_AES_EncryptCbc(CAAM, &hndl,
            (const uint8_t*)buf[2].TheAddress, (uint8_t*)buf[3].TheAddress,
            buf[3].Length, (const uint8_t*)buf[4].TheAddress,
            (const uint8_t*)buf[0].TheAddress, buf[0].Length);

        /* store updated CBC state */
        XMEMCPY((byte*)buf[4].TheAddress,
            (byte*)buf[3].TheAddress + buf[3].Length - AES_BLOCK_SIZE,
            AES_BLOCK_SIZE);
    }

    if (status != kStatus_Success) {
        return -1;
    }
   return 0;
}
#endif

#ifdef WOLFSSL_AES_COUNTER
/* AES-CTR returns 0 on success */
static int DoAesCTR(unsigned int args[4], CAAM_BUFFER *buf, int sz)
{
    status_t status;
    caam_handle_t hndl;
    byte *tmpIn  = NULL;
    byte *tmpOut = NULL;
    byte *alignedIn  = NULL;
    byte *alignedOut = NULL;

    if (buf[2].TheAddress % CAAM_BUFFER_ALIGN) {
        /* input not aligned */
        tmpIn = (byte*)XMALLOC(buf[2].Length + CAAM_BUFFER_ALIGN, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        alignedIn = tmpIn + (CAAM_BUFFER_ALIGN -
            ((wc_ptr_t)tmpIn % CAAM_BUFFER_ALIGN));
        XMEMCPY(alignedIn, (byte*)buf[2].TheAddress, buf[2].Length);
    }
    else {
        alignedIn = (byte*)buf[2].TheAddress;
    }

    if (buf[3].TheAddress % CAAM_BUFFER_ALIGN) {
        /* output not aligned */
        tmpOut = (byte*)XMALLOC(buf[3].Length + CAAM_BUFFER_ALIGN, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        alignedOut = tmpOut + (CAAM_BUFFER_ALIGN -
            ((wc_ptr_t)tmpOut % CAAM_BUFFER_ALIGN));
    }
    else {
        alignedOut = (byte*)buf[3].TheAddress;
    }

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    status = CAAM_AES_CryptCtr(CAAM, &hndl, alignedIn, alignedOut,
        buf[3].Length, (byte*)buf[4].TheAddress, (byte*)buf[0].TheAddress,
        buf[0].Length, (byte*)buf[2].TheAddress, NULL);
    if (tmpOut != NULL) {
        XMEMCPY((byte*)buf[3].TheAddress, alignedOut, buf[3].Length);
        XFREE(tmpOut, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (tmpIn != NULL) {
        XFREE(tmpIn, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (status != kStatus_Success) {
        return -1;
    }
   return 0;
}
#endif


#if defined(HAVE_ECC) && defined(WOLFSSL_CAAM_ECC)
#include <wolfssl/wolfcrypt/asn.h>

/* helper function get the ECDSEL value, this is a value that signals the
 * hardware to use preloaded curve parameters
 */
static word32 GetECDSEL(int curveId)
{
    word32 ecdsel = 0;

    switch (curveId) {
        case ECC_SECP192R1:
            ecdsel = CAAM_ECDSA_P192;
            break;

        case ECC_SECP224R1:
            ecdsel = CAAM_ECDSA_P224;
            break;

        case ECC_CURVE_DEF:
        case ECC_SECP256R1:
            ecdsel = CAAM_ECDSA_P256;
            break;

        case ECC_SECP384R1:
            ecdsel = CAAM_ECDSA_P384;
            break;

        case ECC_SECP521R1:
            ecdsel = CAAM_ECDSA_P521;
            break;

        default:
            WOLFSSL_MSG("not using preset curve parameters");
    }

    return ecdsel;
}


/* ECC sign operation on hardware, can handle black keys
 * returns 0 on success
 */
int wc_CAAM_EccSign(const byte* in, int inlen, byte* out, word32* outlen,
        WC_RNG *rng, ecc_key *key, int devId)
{
    const ecc_set_type* dp;
    int ret = 0;
    ALIGN16 byte k[MAX_ECC_BYTES + WC_CAAM_MAC_SZ];
    word32 kSz = MAX_ECC_BYTES + WC_CAAM_MAC_SZ;

    /* input needs to be aligned */
    byte *alignedIn  = NULL;
    byte *tmpIn  = NULL;

    ALIGN16 byte r[MAX_ECC_BYTES] = {0};
    ALIGN16 byte s[MAX_ECC_BYTES] = {0};
    word32 rSz = MAX_ECC_BYTES;
    word32 sSz = MAX_ECC_BYTES;
    int keySz;
    word32 ecdsel;
    word32 enc = 0;
    status_t status;
    caam_handle_t hndl;

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    (void)rng;
    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    if (dp->id != ECC_SECP256R1 && dp->id != ECC_SECP384R1) {
        WOLFSSL_MSG("Limiting CAAM to P256/P384 for now");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* check for known predetermined parameters */
    ecdsel = GetECDSEL(dp->id);
    if (ecdsel == 0) {
        WOLFSSL_MSG("Unsupported curve type");
        return BAD_FUNC_ARG;
    }
    keySz = kSz = wc_ecc_size(key);

    /* private key */
    if (key->blackKey == CAAM_BLACK_KEY_SM) {
        ret = -1; /* only handling black keys not SM (secure memory) ones */
    }
    else {
        if (key->blackKey == CAAM_BLACK_KEY_CCM) {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(key), k,
                    kSz + WC_CAAM_MAC_SZ) != MP_OKAY) {
                return MP_TO_E;
            }
        }
        else {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(key), k, kSz) !=
                    MP_OKAY) {
                return MP_TO_E;
            }
        }
    }

    ecdsel = GetECDSEL(dp->id);
    if (ecdsel == 0) {
        WOLFSSL_MSG("unknown key type or size");
        return CRYPTOCB_UNAVAILABLE;
    }

    switch (key->blackKey) {
        case 0:
            enc = 0;
            break;
        case CAAM_BLACK_KEY_ECB:
            enc = CAAM_PKHA_ENC_PRI_AESECB;
            break;
        default:
            WOLFSSL_MSG("unknown/unsupported key type");
            return BAD_FUNC_ARG;
    }

    if ((wc_ptr_t)in % CAAM_BUFFER_ALIGN) {
        /* input not aligned */
        tmpIn = (byte*)XMALLOC(inlen + CAAM_BUFFER_ALIGN, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        alignedIn = tmpIn + (CAAM_BUFFER_ALIGN -
            ((wc_ptr_t)tmpIn % CAAM_BUFFER_ALIGN));
        XMEMCPY(alignedIn, in, inlen);
    }
    else {
        alignedIn = (byte*)in;
    }


    status = CAAM_ECC_Sign(CAAM, &hndl, k, kSz, alignedIn, inlen, r, rSz, s,
        sSz, ecdsel, enc);
    if (tmpIn != NULL) {
        XFREE(tmpIn, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    if (status != kStatus_Success) {
        ret = -1;
    }

    /* convert signature from raw bytes to signature format */
    if (ret == 0) {
        mp_int mpr, mps;

        mp_init(&mpr);
        mp_init(&mps);

        mp_read_unsigned_bin(&mpr, r, keySz);
        mp_read_unsigned_bin(&mps, s, keySz);

        ret = StoreECC_DSA_Sig(out, outlen, &mpr, &mps);
        mp_free(&mpr);
        mp_free(&mps);
        if (ret != 0) {
            WOLFSSL_MSG("Issue converting to signature");
            return -1;
        }
    }

    return ret;
}


/* helper function to handle r/s with verify
 * returns 0 on success
 */
static int wc_CAAM_EccVerify_ex(mp_int* r, mp_int *s, const byte* hash,
        word32 hashlen, int* res, ecc_key* key)
{
    const ecc_set_type* dp;
    int keySz;
    word32 ecdsel = 0;

    /* input needs to be aligned */
    byte *alignedIn  = NULL;
    byte *tmpIn  = NULL;

    ALIGN16 byte rbuf[MAX_ECC_BYTES] = {0};
    ALIGN16 byte sbuf[MAX_ECC_BYTES] = {0};

    ALIGN16 byte qx[MAX_ECC_BYTES] = {0};
    ALIGN16 byte qy[MAX_ECC_BYTES] = {0};
    ALIGN16 byte qxy[MAX_ECC_BYTES * 2] = {0};
    word32 qxLen, qyLen;

    status_t status;
    caam_handle_t hndl;

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    /* right now only support P256/P384 @TODO */
    if (dp->id != ECC_SECP256R1 && dp->id != ECC_SECP384R1) {
        WOLFSSL_MSG("Only support P256 and P384 verify with CAAM for now");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* check for known predetermined parameters */
    ecdsel = GetECDSEL(dp->id);

    if (ecdsel == 0) {
        WOLFSSL_MSG("Curve parameters not supported");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* Wx,y public key */
    keySz = wc_ecc_size(key);
    qxLen = qyLen = MAX_ECC_BYTES;

    if (wc_ecc_export_public_raw(key, qx, &qxLen, qy, &qyLen) != 0) {
        WOLFSSL_MSG("Issue exporting public key part");
        return -1;
    }
    XMEMCPY(qxy, qx, qxLen);
    XMEMCPY(qxy+qxLen, qy, qyLen);

    if (mp_to_unsigned_bin_len(r, rbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }

    if (mp_to_unsigned_bin_len(s, sbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }

    if ((wc_ptr_t)hash % CAAM_BUFFER_ALIGN) {
        /* input not aligned */
        tmpIn = (byte*)XMALLOC(hashlen + CAAM_BUFFER_ALIGN, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        alignedIn = tmpIn + (CAAM_BUFFER_ALIGN -
            ((wc_ptr_t)tmpIn % CAAM_BUFFER_ALIGN));
        XMEMCPY(alignedIn, hash, hashlen);
    }
    else {
        alignedIn = (byte*)hash;
    }

    status = CAAM_ECC_Verify(CAAM, &hndl, qxy, qxLen+qyLen, rbuf,
        keySz, sbuf, keySz, alignedIn, hashlen, ecdsel);
    if (tmpIn != NULL) {
        XFREE(tmpIn, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    *res = 0;
    if (status == kStatus_Success) {
        *res = 1;
    }

    return MP_OKAY;
}


/* ECC verify operation using hardware
 * returns 0 on success, and sets "res" with verify result (1 for verify ok)
 */
int wc_CAAM_EccVerify(const byte* sig, word32 siglen, const byte* hash,
        word32 hashlen, int* res, ecc_key* key, int devId)
{
    int ret;
    mp_int r, s;

    ret = DecodeECC_DSA_Sig(sig, siglen, &r, &s);
    if (ret == 0) {
        ret = wc_CAAM_EccVerify_ex(&r, &s, hash, hashlen, res, key);
        mp_free(&r);
        mp_free(&s);
    }

    (void)devId;
    return ret;
}


/* ECDH operation using hardware, can handle black keys
 * returns 0 on success
 */
int wc_CAAM_Ecdh(ecc_key* private_key, ecc_key* public_key, byte* out,
        word32* outlen, int devId)
{
    const ecc_set_type* dp;
    int keySz;
    word32 ecdsel = 0; /* ecc parameters in hardware */
    int enc;
    status_t status;

    ALIGN16 byte k[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};
    ALIGN16 byte qx[MAX_ECC_BYTES] = {0};
    ALIGN16 byte qy[MAX_ECC_BYTES] = {0};
    ALIGN16 byte qxy[MAX_ECC_BYTES * 2] = {0};
    word32 qxSz, qySz;
    caam_handle_t hndl;

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    if (private_key->dp != NULL) {
        dp = private_key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(private_key->idx);
    }

    if (dp->id != ECC_SECP256R1 && dp->id != ECC_SECP384R1) {
        return CRYPTOCB_UNAVAILABLE;
    }

    /* check for known predetermined parameters */
    ecdsel = GetECDSEL(dp->id);
    if (ecdsel == 0) { /* predefined value not known, loading all parameters */
        WOLFSSL_MSG("Unsupported curve parameters");
        return CRYPTOCB_UNAVAILABLE;
    }

    keySz = wc_ecc_size(private_key);
    if (*outlen < (word32)keySz) {
        WOLFSSL_MSG("out buffer is to small");
        return BUFFER_E;
    }

    /* public key */
    qxSz = qySz = MAX_ECC_BYTES;
    if (wc_ecc_export_public_raw(public_key, qx, &qxSz, qy, &qySz) != 0) {
        WOLFSSL_MSG("Issue exporting public key part");
        return -1;
    }
    XMEMCPY(qxy, qx, qxSz);
    XMEMCPY(qxy+qxSz, qy, qySz);

    /* private key */
    if (keySz > MAX_ECC_BYTES) {
        return BUFFER_E;
    }

    enc = 0;
    if (private_key->blackKey == CAAM_BLACK_KEY_ECB) {
        enc = CAAM_PKHA_ENC_PRI_AESECB;
    }

    if (private_key->blackKey == CAAM_BLACK_KEY_CCM) {
        if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(private_key), k,
                keySz + WC_CAAM_MAC_SZ) != MP_OKAY) {
            return MP_TO_E;
        }
    }
    else {
        if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(private_key), k, keySz)
                != MP_OKAY) {
            return MP_TO_E;
        }
    }

    if (*outlen < (word32)keySz) {
        return -1;
    }

    status = CAAM_ECC_ECDH(CAAM, &hndl, k, keySz, qxy, keySz*2, out, keySz,
        ecdsel, enc);
    if (status == kStatus_Success) {
        *outlen = keySz;
        return MP_OKAY;
    }
    else {
        return -1;
    }
}

#ifdef WOLFSSL_KEY_GEN
/* creates a [ private black key ] [ x , y ]
 * returns 0 on success
 */
int wc_CAAM_MakeEccKey(WC_RNG* rng, int keySize, ecc_key* key, int curveId,
    int devId)
{
    int ret = 0;
    ALIGN16 byte xy[MAX_ECC_BYTES * 2];
    ALIGN16 byte k[MAX_ECC_BYTES];
    word32 kSz = MAX_ECC_BYTES;
    word32 xySz = MAX_ECC_BYTES * 2;
    word32 ecdsel;
    word32 enc;
    status_t status;
    caam_handle_t hndl;

    XMEMSET(xy, 0, MAX_ECC_BYTES * 2);
    XMEMSET(k, 0, MAX_ECC_BYTES);

    /* @TODO running on alternate job rings - performance enhancement */
    hndl.jobRing = kCAAM_JobRing0;

    key->type = ECC_PRIVATEKEY;

    /* if set to default curve then assume SECP256R1 */
    if (keySize == 32 && curveId == ECC_CURVE_DEF) curveId = ECC_SECP256R1;
    if (keySize == 48 && curveId == ECC_CURVE_DEF) curveId = ECC_SECP384R1;

    if (curveId != ECC_SECP256R1 && curveId != ECC_SECP384R1) {
        /* currently only implemented P256/P384 support */
        return CRYPTOCB_UNAVAILABLE;
    }

    ecdsel = GetECDSEL(curveId);
    if (ecdsel == 0) {
        WOLFSSL_MSG("unknown key type or size");
        return CRYPTOCB_UNAVAILABLE;
    }

    if (key->blackKey == CAAM_BLACK_KEY_ECB) {
        enc = CAAM_PKHA_ENC_PRI_AESECB;
    }

    if (key->blackKey == 0) {
    #ifdef WOLFSSL_CAAM_NO_BLACK_KEY
        enc = 0;
    #else
        key->blackKey = CAAM_BLACK_KEY_ECB;
        enc = CAAM_PKHA_ENC_PRI_AESECB;
    #endif
    }

    status = CAAM_ECC_Keygen(CAAM, &hndl, k, &kSz, xy, &xySz, ecdsel,
        enc);
    if (status != kStatus_Success) {
        ret = -1;
    }

    if (ret == 0 &&
            wc_ecc_import_unsigned(key, xy, xy + keySize, k, curveId) != 0) {
        WOLFSSL_MSG("issue importing key");
        ret = -1;
    }

    return ret;
}
#endif /* WOLFSSL_KEY_GEN */
#endif /* HAVE_ECC */


/* Do a synchronous operations and block till done
 * returns 0 on success */
int SynchronousSendRequest(int type, unsigned int args[4], CAAM_BUFFER *buf,
        int sz)
{
    int ret = 0;

    caam_handle_t hndl;
    hndl.jobRing = kCAAM_JobRing0;

    switch (type) {
    case CAAM_ENTROPY:
        if (CAAM_RNG_GetRandomData(CAAM, &hndl, kCAAM_RngStateHandle0,
                (uint8_t *)buf[0].TheAddress, buf[0].Length,
                kCAAM_RngDataAny, NULL) != kStatus_Success) {
            ret = WC_HW_E;
        }
        break;

#ifndef NO_AES_CBC
    case CAAM_AESCBC:
        ret = DoAesCBC(args, buf, sz);
        break;
#endif

#ifdef WOLFSSL_AES_COUNTER
    case CAAM_AESCTR:
        ret = DoAesCTR(args, buf, sz);
        break;
#endif

#ifdef CAAM_BLOB_EXPANSION
    case CAAM_BLOB_ENCAP:
        if (CAAM_Blob(CAAM, &hndl, (byte*)buf[1].TheAddress,
                buf[1].Length, (byte*)buf[2].TheAddress,
                buf[2].Length, (byte*)buf[0].TheAddress,
                buf[0].Length, CAAM_ENCAP_BLOB,
                (args[0] == 0)? CAAM_RED_BLOB : CAAM_BLACK_BLOB)
                != kStatus_Success) {
            ret = WC_HW_E;
        }
        break;

    case CAAM_BLOB_DECAP:
        if (CAAM_Blob(CAAM, &hndl, (byte*)buf[1].TheAddress, buf[1].Length,
                (byte*)buf[2].TheAddress, buf[2].Length,
                (byte*)buf[0].TheAddress, buf[0].Length,
                CAAM_DECAP_BLOB,
                (args[0] == 0)? CAAM_RED_BLOB : CAAM_BLACK_BLOB)
                != kStatus_Success) {
            ret = WC_HW_E;
        }
        break;
#endif

    default:
        WOLFSSL_MSG("Unknown/unsupported type");
        return -1;
    }

    if (ret != 0) {
        return -1;
    }

    return Success;
}

#endif /* WOLFSSL_IMXRT1170_CAAM */
