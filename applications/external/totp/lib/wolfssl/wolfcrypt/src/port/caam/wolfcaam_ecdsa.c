/* wolfcaam_ecdsa.c
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

#if defined(WOLFSSL_CAAM) && defined(HAVE_ECC) && defined(WOLFSSL_CAAM_ECC)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_ecdsa.h>

#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/asn.h>

#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
#include <stdio.h>
#endif

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)
/* offload calls through devcrypto support */


/* create signature using CAAM
 * returns MP_OKAY on success
 */
static int wc_CAAM_DevEccSign(const byte* in, int inlen, byte* out,
    word32* outlen, WC_RNG *rng, ecc_key *key)
{
    const ecc_set_type* dp;
    int ret, keySz;
    byte r[MAX_ECC_BYTES] = {0};
    byte s[MAX_ECC_BYTES] = {0};

    byte pk[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};

    (void)rng;
    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    if (dp->id != ECC_SECP256R1 && dp->id != ECC_SECP384R1) {
        WOLFSSL_MSG("Limiting CAAM to P256 and P384 for now");
        return CRYPTOCB_UNAVAILABLE;
    }
    keySz  = wc_ecc_size(key);

    /* private key */
    if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(key), pk, keySz) != MP_OKAY)
    {
        return MP_TO_E;
    }

    ret = wc_DevCryptoEccSign(dp->id, key->blackKey, pk, keySz, in, inlen,
            r, keySz, s, keySz);

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
            WOLFSSL_MSG("Issue converting to signature\n");
            return -1;
        }
    }

    return ret;
}


/* verify with individual r and s signature parts
 * returns MP_OKAY on success and sets 'res' to 1 if verified
 */
static int wc_CAAM_DevEccVerify_ex(mp_int* r, mp_int *s, const byte* hash,
        word32 hashlen, int* res, ecc_key* key)
{
    const ecc_set_type* dp;
    int ret;
    int keySz;

    byte rbuf[MAX_ECC_BYTES] = {0};
    byte sbuf[MAX_ECC_BYTES] = {0};

    byte qx[MAX_ECC_BYTES] = {0};
    byte qy[MAX_ECC_BYTES] = {0};
    byte qxy[MAX_ECC_BYTES * 2] = {0};
    word32 qxLen, qyLen;

    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    /* Wx,y public key */
    keySz = wc_ecc_size(key);
    qxLen = qyLen = MAX_ECC_BYTES;
    wc_ecc_export_public_raw(key, qx, &qxLen, qy, &qyLen);
    XMEMCPY(qxy, qx, qxLen);
    XMEMCPY(qxy+qxLen, qy, qyLen);

    if (mp_to_unsigned_bin_len(r, rbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }
    if (mp_to_unsigned_bin_len(s, sbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }
    ret = wc_DevCryptoEccVerify(dp->id, qxy, qxLen + qyLen, hash, hashlen,
            rbuf, keySz, sbuf, keySz);

    *res = 0;
    if (ret == 0)
        *res = 1;

    return ret;
}


/* Does ECDH operation using CAAM and returns MP_OKAY on success */
static int wc_CAAM_DevEcdh(ecc_key* private_key, ecc_key* public_key, byte* out,
        word32* outlen)
{
    const ecc_set_type* dp;
    int ret, keySz;

    byte pk[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};
    byte qx[MAX_ECC_BYTES] = {0};
    byte qy[MAX_ECC_BYTES] = {0};
    byte qxy[MAX_ECC_BYTES * 2] = {0};
    word32 qxSz, qySz;

    if (private_key->dp != NULL) {
        dp = private_key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(private_key->idx);
    }

    keySz = wc_ecc_size(private_key);
    if (*outlen < (word32)keySz) {
        WOLFSSL_MSG("out buffer is to small");
        return BUFFER_E;
    }

    /* public key */
    qxSz = qySz = MAX_ECC_BYTES;
    wc_ecc_export_public_raw(public_key, qx, &qxSz, qy, &qySz);
    XMEMCPY(qxy, qx, qxSz);
    XMEMCPY(qxy+qxSz, qy, qySz);

    /* private key */
    if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(private_key), pk, keySz) !=
            MP_OKAY) {
        WOLFSSL_MSG("error getting private key buffer");
        return MP_TO_E;
    }

    ret = wc_DevCryptoEccEcdh(dp->id, private_key->blackKey, pk, keySz,
        qxy, qxSz + qySz, out, *outlen);
    if (ret == 0) {
        *outlen = keySz;
    }
    return ret;
}


#ifdef WOLFSSL_KEY_GEN
/* [ private black key ] [ x , y ] */
static int wc_CAAM_DevMakeEccKey(WC_RNG* rng, int keySize, ecc_key* key,
    int curveId)
{
    int ret;
    int blackKey = 1; /* default to using black encrypted keys */

    byte s[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};
    byte xy[MAX_ECC_BYTES*2] = {0};

    key->type = ECC_PRIVATEKEY;

    /* if set to default curve then assume SECP256R1 */
    if (keySize == 32 && curveId == ECC_CURVE_DEF) curveId = ECC_SECP256R1;

    if (curveId != ECC_SECP256R1 &&
        curveId != ECC_SECP384R1) {
        return CRYPTOCB_UNAVAILABLE;
    }

    ret = wc_DevCryptoEccKeyGen(curveId, blackKey, s, keySize, xy, keySize*2);
    if (wc_ecc_import_unsigned(key, xy, xy + keySize, s, curveId) != 0) {
        WOLFSSL_MSG("issue importing key");
        return -1;
    }
    key->blackKey = blackKey;

    (void)rng;
    return ret;
}
#endif /* WOLFSSL_KEY_GEN */

#endif /* WOLFSSL_DEVCRYPTO_ECDSA */

#ifndef WOLFSSL_IMXRT1170_CAAM
/* helper function get the ECDSEL value, this is a value that signals the
 * hardware to use preloaded curve parameters
 */
static word32 GetECDSEL(int curveId, word32 PD_BIT)
{
    word32 ecdsel = 0;

    switch (curveId) {
        case ECC_SECP192R1:
            ecdsel = (PD_BIT | CAAM_ECDSA_P192);
            break;

        case ECC_SECP224R1:
            ecdsel = (PD_BIT | CAAM_ECDSA_P224);
            break;

        case ECC_CURVE_DEF:
        case ECC_SECP256R1:
            ecdsel = (PD_BIT | CAAM_ECDSA_P256);
            break;

        case ECC_SECP384R1:
            ecdsel = (PD_BIT | CAAM_ECDSA_P384);
            break;

        case ECC_SECP521R1:
            ecdsel = (PD_BIT | CAAM_ECDSA_P521);
            break;

        default:
            WOLFSSL_MSG("not using preset curve parameters");
    }

    return ecdsel;
}


/* create signature using CAAM
 * returns MP_OKAY on success
 */
int wc_CAAM_EccSign(const byte* in, int inlen, byte* out, word32* outlen,
        WC_RNG *rng, ecc_key *key, int devId)
{
    const ecc_set_type* dp;
    word32 args[4] = {0};
    CAAM_BUFFER buf[9];
    int ret, keySz;
    word32 ecdsel = 0;
    byte r[MAX_ECC_BYTES] = {0};
    byte s[MAX_ECC_BYTES] = {0};
    word32 idx = 0;
    byte pk[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)
    if (devId == WOLFSSL_CAAM_DEVID) {
        return wc_CAAM_DevEccSign(in, inlen, out, outlen, rng, key);
    }
#endif

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
    ecdsel = GetECDSEL(dp->id, CAAM_ECDSA_PD);
    if (ecdsel == 0) {
        WOLFSSL_MSG("Unsupported curve type");
        return BAD_FUNC_ARG;
    }

    keySz  = wc_ecc_size(key);

    /* private key */
    if (key->blackKey == CAAM_BLACK_KEY_SM) {
        buf[idx].TheAddress = (CAAM_ADDRESS)key->blackKey;
        args[0] = CAAM_BLACK_KEY_SM; /* is a black key in sm */
        buf[idx].Length = keySz;
    }
    else {
        if (key->blackKey == CAAM_BLACK_KEY_CCM) {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(key), pk,
                    keySz + WC_CAAM_MAC_SZ) != MP_OKAY) {
                return MP_TO_E;
            }
            buf[idx].Length = keySz + WC_CAAM_MAC_SZ;
        }
        else {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(key), pk, keySz) !=
                    MP_OKAY) {
                return MP_TO_E;
            }
            buf[idx].Length = keySz;
        }
        buf[idx].TheAddress = (CAAM_ADDRESS)pk;
        args[0] = key->blackKey; /* potential black key, not in sm */
    }
    idx++;

    /* hash to sign */
    buf[idx].TheAddress = (CAAM_ADDRESS)in;
    buf[idx].Length = inlen;
    idx++;

    /* r output */
    buf[idx].TheAddress = (CAAM_ADDRESS)r;
    buf[idx].Length = keySz;
    idx++;

    /* s output */
    buf[idx].TheAddress = (CAAM_ADDRESS)s;
    buf[idx].Length = keySz;
    idx++;

    args[1] = ecdsel;
    args[2] = inlen;
    args[3] = keySz;

    ret = wc_caamAddAndWait(buf, idx, args, CAAM_ECDSA_SIGN);
    if (ret != 0)
        return -1;

    /* convert signature from raw bytes to signature format */
    {
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

    (void)devId;
    return MP_OKAY;
}


/* verify with individual r and s signature parts
 * returns MP_OKAY on success and sets 'res' to 1 if verified
 */
static int wc_CAAM_EccVerify_ex(mp_int* r, mp_int *s, const byte* hash,
        word32 hashlen, int* res, ecc_key* key)
{
    const ecc_set_type* dp;
    word32 args[4] = {0};
    CAAM_BUFFER buf[9];
    int ret;
    int keySz;
    word32 idx = 0;
    word32 ecdsel = 0;

    byte rbuf[MAX_ECC_BYTES] = {0};
    byte sbuf[MAX_ECC_BYTES] = {0};

    byte qx[MAX_ECC_BYTES] = {0};
    byte qy[MAX_ECC_BYTES] = {0};
    byte qxy[MAX_ECC_BYTES * 2] = {0};
    byte tmp[MAX_ECC_BYTES * 2] = {0};
    word32 qxLen, qyLen;

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
    ecdsel = GetECDSEL(dp->id, CAAM_ECDSA_PD);

    if (ecdsel == 0) {
        WOLFSSL_MSG("Curve parameters not supported");
        return CRYPTOCB_UNAVAILABLE;
    }

    /* Wx,y public key */
    keySz = wc_ecc_size(key);
    if (key->securePubKey > 0) {
        buf[idx].TheAddress = (CAAM_ADDRESS)key->securePubKey;
        buf[idx].Length = keySz * 2;
        args[0] = 1; /* using public key in secure memory */
    }
    else {
        qxLen = qyLen = MAX_ECC_BYTES;
        wc_ecc_export_public_raw(key, qx, &qxLen, qy, &qyLen);
        XMEMCPY(qxy, qx, qxLen);
        XMEMCPY(qxy+qxLen, qy, qyLen);
        buf[idx].TheAddress = (CAAM_ADDRESS)qxy;
        buf[idx].Length = qxLen + qyLen;
    }
    idx++;

    buf[idx].TheAddress = (CAAM_ADDRESS)hash;
    buf[idx].Length = hashlen;
    idx++;

    if (mp_to_unsigned_bin_len(r, rbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }

    buf[idx].TheAddress = (CAAM_ADDRESS)rbuf;
    buf[idx].Length = keySz;
    idx++;

    if (mp_to_unsigned_bin_len(s, sbuf, keySz) != MP_OKAY) {
        return MP_TO_E;
    }

    buf[idx].TheAddress = (CAAM_ADDRESS)sbuf;
    buf[idx].Length = keySz;
    idx++;

    /* temporary scratch buffer, the manual calls for it and HW expects it */
    buf[idx].TheAddress = (CAAM_ADDRESS)tmp;
    buf[idx].Length = sizeof(tmp);
    idx++;

    args[1] = ecdsel;
    args[2] = hashlen;
    args[3] = wc_ecc_size(key);
    ret = wc_caamAddAndWait(buf, idx, args, CAAM_ECDSA_VERIFY);

    *res = 0;
    if (ret == 0)
        *res = 1;

    return MP_OKAY;
}


/* Verify with ASN1 syntax around the signature
 * returns MP_OKAY on success
 */
int wc_CAAM_EccVerify(const byte* sig, word32 siglen, const byte* hash,
        word32 hashlen, int* res, ecc_key* key, int devId)
{
    int ret;
    mp_int r, s;

    ret = DecodeECC_DSA_Sig(sig, siglen, &r, &s);
    if (ret == 0) {
    #if defined(WOLFSSL_DEVCRYPTO_ECDSA)
        if (devId == WOLFSSL_CAAM_DEVID) {
            ret = wc_CAAM_DevEccVerify_ex(&r, &s, hash, hashlen, res, key);
        }
        else
    #endif
        {
            ret = wc_CAAM_EccVerify_ex(&r, &s, hash, hashlen, res, key);
        }
        mp_free(&r);
        mp_free(&s);
    }

    (void)devId;
    return ret;
}


/* Does ECDH operation using CAAM and returns MP_OKAY on success */
int wc_CAAM_Ecdh(ecc_key* private_key, ecc_key* public_key, byte* out,
        word32* outlen, int devId)
{
    const ecc_set_type* dp;
    word32 args[4] = {0};
    CAAM_BUFFER buf[9];
    int ret, keySz;
    word32 ecdsel = 0; /* ecc parameters in hardware */
    word32 idx    = 0;

    byte pk[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};
    byte qx[MAX_ECC_BYTES] = {0};
    byte qy[MAX_ECC_BYTES] = {0};
    byte qxy[MAX_ECC_BYTES * 2] = {0};
    word32 qxSz, qySz;

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)
    if (devId == WOLFSSL_CAAM_DEVID) {
        return wc_CAAM_DevEcdh(private_key, public_key, out, outlen);
    }
#endif

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
    ecdsel = GetECDSEL(dp->id, CAAM_ECDSA_KEYGEN_PD);
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
    if (public_key->securePubKey > 0) {
        buf[idx].TheAddress = (CAAM_ADDRESS)public_key->securePubKey;
        buf[idx].Length = keySz * 2;
        args[1] = 1; /* using public key with secure memory address */
    }
    else {
        qxSz = qySz = MAX_ECC_BYTES;
        wc_ecc_export_public_raw(public_key, qx, &qxSz, qy, &qySz);
        XMEMCPY(qxy, qx, qxSz);
        XMEMCPY(qxy+qxSz, qy, qySz);
        buf[idx].TheAddress = (CAAM_ADDRESS)qxy;
        buf[idx].Length = qxSz + qySz;
    }
    idx++;

    /* private key */
    if (private_key->blackKey > 0 &&
        (private_key->blackKey != CAAM_BLACK_KEY_CCM &&
         private_key->blackKey != CAAM_BLACK_KEY_ECB)) {
        buf[idx].TheAddress = (CAAM_ADDRESS)private_key->blackKey;
        args[0] = CAAM_BLACK_KEY_SM; /* is a black key */
        buf[idx].Length = sizeof(unsigned int);
    }
    else {
        if (keySz > MAX_ECC_BYTES) {
            return BUFFER_E;
        }

        if (private_key->blackKey == CAAM_BLACK_KEY_CCM) {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(private_key), pk,
                keySz + WC_CAAM_MAC_SZ) != MP_OKAY) {
                return MP_TO_E;
            }
            buf[idx].Length = keySz + WC_CAAM_MAC_SZ;
        }
        else {
            if (mp_to_unsigned_bin_len(wc_ecc_key_get_priv(private_key), pk,
                    keySz) != MP_OKAY) {
                return MP_TO_E;
            }
            buf[idx].Length = keySz;
        }

        buf[idx].TheAddress = (CAAM_ADDRESS)pk;
        args[0] = private_key->blackKey; /* potential black key, but not sm */
    }

#if 0
    {
        int z;
        unsigned char* pt = (unsigned char*)buf[idx].TheAddress;
        printf("sending private key [%d] :", buf[idx].Length);
        for (z = 0; z < buf[idx].Length; z++)
            printf("%02X", pt[z]);
        printf("\n");
    }
#endif
    idx++;

    /* output shared secret */
    buf[idx].TheAddress = (CAAM_ADDRESS)out;
    buf[idx].Length = keySz;
    idx++;

    args[2] = ecdsel;
    args[3] = keySz;
    ret = wc_caamAddAndWait(buf, idx, args, CAAM_ECDSA_ECDH);
    (void)devId;
    if (ret == 0) {
        *outlen = keySz;
        return MP_OKAY;
    }
    else {
        return -1;
    }
}


#ifdef WOLFSSL_KEY_GEN
/* [ private black key ] [ x , y ] */
int wc_CAAM_MakeEccKey(WC_RNG* rng, int keySize, ecc_key* key, int curveId,
    int devId)
{
    word32 args[4]     = {0};
    CAAM_BUFFER buf[2] = {0};
    word32 ecdsel = 0;

    int ret;

    byte s[MAX_ECC_BYTES + WC_CAAM_MAC_SZ] = {0};
    byte xy[MAX_ECC_BYTES*2] = {0};

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)
    if (devId == WOLFSSL_CAAM_DEVID) {
        return wc_CAAM_DevMakeEccKey(rng, keySize, key, curveId);
    }
#endif
    key->type = ECC_PRIVATEKEY;

    /* if set to default curve then assume SECP256R1 */
    if (keySize == 32 && curveId == ECC_CURVE_DEF) curveId = ECC_SECP256R1;
    if (keySize == 48 && curveId == ECC_CURVE_DEF) curveId = ECC_SECP384R1;

    if (curveId != ECC_SECP256R1 && curveId != ECC_SECP384R1) {
        /* currently only implemented P256/P384 support */
        return CRYPTOCB_UNAVAILABLE;
    }

    ecdsel = GetECDSEL(curveId, CAAM_ECDSA_KEYGEN_PD);
    if (ecdsel == 0) {
        WOLFSSL_MSG("unknown key type or size");
        return CRYPTOCB_UNAVAILABLE;
    }

    (void)rng;
    (void)devId;

    if (key->blackKey == 0) {
    #ifdef WOLFSSL_CAAM_NO_BLACK_KEY
        args[0] = 0;
    #elif defined(WOLFSSL_CAAM_BLACK_KEY_AESCCM)
        args[0] = CAAM_BLACK_KEY_CCM;
    #elif defined(WOLFSSL_CAAM_BLACK_KEY_SM)
        args[0] = CAAM_BLACK_KEY_SM;
    #else
        args[0] = CAAM_BLACK_KEY_ECB;
    #endif
    }
    else {
        /* type of black key was already set in the ecc key struct */
        args[0] = key->blackKey;
    }

    args[1] = ecdsel;
    args[3] = keySize;

    buf[0].TheAddress = (CAAM_ADDRESS)s;
    if (args[0] == CAAM_BLACK_KEY_SM) {
        /* only get a physical address */
        buf[0].Length = sizeof(unsigned int);
    }
    else if (args[0] == CAAM_BLACK_KEY_CCM) {
        /* account for additional MAC */
        buf[0].Length = keySize + WC_CAAM_MAC_SZ;
    }
    else {
        buf[0].Length = keySize;
    }
    buf[1].TheAddress = (CAAM_ADDRESS)xy;
    buf[1].Length     = keySize*2;
    key->blackKey     = args[0];

    ret = wc_caamAddAndWait(buf, 2, args, CAAM_ECDSA_KEYPAIR);
    if (args[0] == CAAM_BLACK_KEY_SM && ret == 0) {
        unsigned char* pt = (unsigned char*)buf[0].TheAddress;
        key->blackKey     = (pt[0] << 24) | (pt[1] << 16) | (pt[2] << 8) | pt[3];
        if (wc_ecc_import_unsigned(key, xy, xy + keySize, NULL, curveId) != 0) {
            WOLFSSL_MSG("issue importing public key");
            return -1;
        }
        key->partNum = args[2];
        return MP_OKAY;
    }
    else if (ret == 0) {
        if (wc_ecc_import_unsigned(key, xy, xy + keySize,
                   s, curveId) != 0) {
            WOLFSSL_MSG("issue importing key");
            return -1;
        }
        key->blackKey = args[0];
        return MP_OKAY;
    }
    return -1;
}
#endif /* WOLFSSL_KEY_GEN */
#endif /* WOLFSSL_IMXRT1170_CAAM */

/* if dealing with a black encrypted key then it can not be checked */
int wc_CAAM_EccCheckPrivKey(ecc_key* key, const byte* pubKey, word32 pubKeySz) {
    (void)pubKey;
    (void)pubKeySz;

    if (key->dp->id == ECC_SECP256R1 && key->blackKey > 0) {
        return 0;
    }
    return CRYPTOCB_UNAVAILABLE;
}

#endif /* WOLFSSL_QNX_CAAM && HAVE_ECC */
