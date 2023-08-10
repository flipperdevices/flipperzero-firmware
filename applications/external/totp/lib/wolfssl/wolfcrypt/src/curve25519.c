/* curve25519.c
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


 /* Based On Daniel J Bernstein's curve25519 Public Domain ref10 work. */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_CURVE25519

#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(FREESCALE_LTC_ECC)
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif
#ifdef WOLFSSL_SE050
    #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

const curve25519_set_type curve25519_sets[] = {
    {
        CURVE25519_KEYSIZE,
        "CURVE25519",
    }
};

static const unsigned char kCurve25519BasePoint[CURVE25519_KEYSIZE] = {9};

/* Curve25519 private key must be less than order */
/* These functions clamp private k and check it */
static WC_INLINE int curve25519_priv_clamp(byte* priv)
{
    priv[0]  &= 248;
    priv[CURVE25519_KEYSIZE-1] &= 127;
    priv[CURVE25519_KEYSIZE-1] |= 64;
    return 0;
}
static WC_INLINE int curve25519_priv_clamp_check(const byte* priv)
{
    /* check that private part of key has been clamped */
    int ret = 0;
    if ((priv[0] & ~248) ||
        (priv[CURVE25519_KEYSIZE-1] & 128)) {
        ret = ECC_BAD_ARG_E;
    }
    return ret;
}

static WC_INLINE void curve25519_copy_point(byte* out, const byte* point,
    int endian)
{
    if (endian == EC25519_BIG_ENDIAN) {
        int i;
        /* put shared secret key in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++) {
            out[i] = point[CURVE25519_KEYSIZE - i -1];
        }
    }
    else { /* put shared secret key in Little Endian format */
        XMEMCPY(out, point, CURVE25519_KEYSIZE);
    }
}

/* compute the public key from an existing private key, using bare vectors.
 *
 * return value is propagated from curve25519() (0 on success), or
 * ECC_BAD_ARG_E, and the byte vectors are little endian.
 */
int wc_curve25519_make_pub(int public_size, byte* pub, int private_size,
                           const byte* priv)
{
    int ret;
#ifdef FREESCALE_LTC_ECC
    const ECPoint* basepoint = nxp_ltc_curve25519_GetBasePoint();
    ECPoint wc_pub;
#endif

    if ( (public_size != CURVE25519_KEYSIZE) ||
        (private_size != CURVE25519_KEYSIZE)) {
        return ECC_BAD_ARG_E;
    }
    if ((pub == NULL) || (priv == NULL)) {
        return ECC_BAD_ARG_E;
    }

    /* check clamping */
    ret = curve25519_priv_clamp_check(priv);
    if (ret != 0)
        return ret;

#ifdef FREESCALE_LTC_ECC
    /* input basepoint on Weierstrass curve */
    ret = nxp_ltc_curve25519(&wc_pub, priv, basepoint, kLTC_Weierstrass);
    if (ret == 0) {
        XMEMCPY(pub, wc_pub.point, CURVE25519_KEYSIZE);
    }
#else
    fe_init();

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    ret = curve25519(pub, priv, kCurve25519BasePoint);

    RESTORE_VECTOR_REGISTERS();
#endif

    return ret;
}

/* compute the public key from an existing private key, with supplied basepoint,
 * using bare vectors.
 *
 * return value is propagated from curve25519() (0 on success),
 * and the byte vectors are little endian.
 */
int wc_curve25519_generic(int public_size, byte* pub,
                          int private_size, const byte* priv,
                          int basepoint_size, const byte* basepoint)
{
#ifdef FREESCALE_LTC_ECC
    /* unsupported with NXP LTC, only supports single basepoint with
     * nxp_ltc_curve25519_GetBasePoint() */
    return WC_HW_E;
#else
    int ret;

    if ((public_size != CURVE25519_KEYSIZE) ||
        (private_size != CURVE25519_KEYSIZE) ||
        (basepoint_size != CURVE25519_KEYSIZE)) {
        return ECC_BAD_ARG_E;
    }
    if ((pub == NULL) || (priv == NULL) || (basepoint == NULL))
        return ECC_BAD_ARG_E;

    /* check clamping */
    ret = curve25519_priv_clamp_check(priv);
    if (ret != 0)
        return ret;

    fe_init();

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    ret = curve25519(pub, priv, basepoint);

    RESTORE_VECTOR_REGISTERS();

    return ret;
#endif /* FREESCALE_LTC_ECC */
}

/* generate a new private key, as a bare vector.
 *
 * return value is propagated from wc_RNG_GenerateBlock(() (0 on success),
 * or BAD_FUNC_ARG/ECC_BAD_ARG_E, and the byte vector is little endian.
 */
int wc_curve25519_make_priv(WC_RNG* rng, int keysize, byte* key)
{
    int ret;

    if (key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    /* currently only a key size of 32 bytes is used */
    if (keysize != CURVE25519_KEYSIZE)
        return ECC_BAD_ARG_E;

    /* random number for private key */
    ret = wc_RNG_GenerateBlock(rng, key, (word32)keysize);
    if (ret == 0) {
        /* Clamp the private key */
        ret = curve25519_priv_clamp(key);
    }

    return ret;
}

/* generate a new keypair.
 *
 * return value is propagated from wc_curve25519_make_private() or
 * wc_curve25519_make_pub() (0 on success).
 */
int wc_curve25519_make_key(WC_RNG* rng, int keysize, curve25519_key* key)
{
    int ret;

    if (key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLF_CRYPTO_CB
    if (key->devId != INVALID_DEVID) {
        ret = wc_CryptoCb_Curve25519Gen(rng, keysize, key);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif

#ifdef WOLFSSL_SE050
    ret = se050_curve25519_create_key(key, keysize);
#else
    ret = wc_curve25519_make_priv(rng, keysize, key->k);
    if (ret == 0) {
        key->privSet = 1;
        ret = wc_curve25519_make_pub((int)sizeof(key->p.point), key->p.point,
                                     (int)sizeof(key->k), key->k);
        key->pubSet = (ret == 0);
    }
#endif
    return ret;
}

#ifdef HAVE_CURVE25519_SHARED_SECRET

int wc_curve25519_shared_secret(curve25519_key* private_key,
                                curve25519_key* public_key,
                                byte* out, word32* outlen)
{
    return wc_curve25519_shared_secret_ex(private_key, public_key,
                                          out, outlen, EC25519_BIG_ENDIAN);
}

int wc_curve25519_shared_secret_ex(curve25519_key* private_key,
                                   curve25519_key* public_key,
                                   byte* out, word32* outlen, int endian)
{
    int ret;
    ECPoint o;

    /* sanity check */
    if (private_key == NULL || public_key == NULL ||
        out == NULL || outlen == NULL || *outlen < CURVE25519_KEYSIZE) {
        return BAD_FUNC_ARG;
    }

    /* make sure we have a populated private and public key */
    if (!public_key->pubSet
    #ifndef WOLFSSL_SE050
        || !private_key->privSet
    #endif
    ) {
        return ECC_BAD_ARG_E;
    }

    /* avoid implementation fingerprinting - make sure signed bit is not set */
    if (public_key->p.point[CURVE25519_KEYSIZE-1] & 0x80) {
        return ECC_BAD_ARG_E;
    }

#ifdef WOLF_CRYPTO_CB
    if (private_key->devId != INVALID_DEVID) {
        ret = wc_CryptoCb_Curve25519(private_key, public_key, out, outlen,
            endian);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif

    XMEMSET(&o, 0, sizeof(o));

#ifdef FREESCALE_LTC_ECC
    /* input point P on Curve25519 */
    ret = nxp_ltc_curve25519(&o, private_key->k, &public_key->p,
        kLTC_Curve25519);
#else
    #ifdef WOLFSSL_SE050
    if (!private_key->privSet) {
        /* use NXP SE050: "privSet" is not set */
        ret = se050_curve25519_shared_secret(private_key, public_key, &o);
    }
    else
    #endif
    {
    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    ret = curve25519(o.point, private_key->k, public_key->p.point);

    RESTORE_VECTOR_REGISTERS();
    }
#endif
#ifdef WOLFSSL_ECDHX_SHARED_NOT_ZERO
    if (ret == 0) {
        int i;
        byte t = 0;
        for (i = 0; i < CURVE25519_KEYSIZE; i++) {
            t |= o.point[i];
        }
        if (t == 0) {
            ret = ECC_OUT_OF_RANGE_E;
        }
    }
#endif
    if (ret != 0) {
        ForceZero(&o, sizeof(o));
        return ret;
    }

    curve25519_copy_point(out, o.point, endian);
    *outlen = CURVE25519_KEYSIZE;

    ForceZero(&o, sizeof(o));

    return ret;
}

#endif /* HAVE_CURVE25519_SHARED_SECRET */

#ifdef HAVE_CURVE25519_KEY_EXPORT

/* export curve25519 public key (Big endian)
 * return 0 on success */
int wc_curve25519_export_public(curve25519_key* key, byte* out, word32* outLen)
{
    return wc_curve25519_export_public_ex(key, out, outLen, EC25519_BIG_ENDIAN);
}

/* export curve25519 public key (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_public_ex(curve25519_key* key, byte* out,
                                   word32* outLen, int endian)
{
    int ret = 0;

    if (key == NULL || out == NULL || outLen == NULL) {
        return BAD_FUNC_ARG;
    }

    /* check and set outgoing key size */
    if (*outLen < CURVE25519_KEYSIZE) {
        *outLen = CURVE25519_KEYSIZE;
        return ECC_BAD_ARG_E;
    }

    /* calculate public if missing */
    if (!key->pubSet) {
        ret = wc_curve25519_make_pub((int)sizeof(key->p.point), key->p.point,
                                     (int)sizeof(key->k), key->k);
        key->pubSet = (ret == 0);
    }
    /* export public point with endianness */
    curve25519_copy_point(out, key->p.point, endian);
    *outLen = CURVE25519_KEYSIZE;

    return ret;
}

#endif /* HAVE_CURVE25519_KEY_EXPORT */

#ifdef HAVE_CURVE25519_KEY_IMPORT

/* import curve25519 public key (Big endian)
 *  return 0 on success */
int wc_curve25519_import_public(const byte* in, word32 inLen,
                                curve25519_key* key)
{
    return wc_curve25519_import_public_ex(in, inLen, key, EC25519_BIG_ENDIAN);
}

/* import curve25519 public key (Big or Little endian)
 * return 0 on success */
int wc_curve25519_import_public_ex(const byte* in, word32 inLen,
                                curve25519_key* key, int endian)
{
#ifdef FREESCALE_LTC_ECC
    ltc_pkha_ecc_point_t ltcPoint;
#endif

    /* sanity check */
    if (key == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    /* check size of incoming keys */
    if (inLen != CURVE25519_KEYSIZE) {
       return ECC_BAD_ARG_E;
    }

    /* import public point with endianness */
    curve25519_copy_point(key->p.point, in, endian);
    key->pubSet = 1;

    key->dp = &curve25519_sets[0];

    /* LTC needs also Y coordinate - let's compute it */
#ifdef FREESCALE_LTC_ECC
    ltcPoint.X = &key->p.point[0];
    ltcPoint.Y = &key->p.pointY[0];
    LTC_PKHA_Curve25519ComputeY(&ltcPoint);
#endif

    return 0;
}

/* Check the public key value (big or little endian)
 *
 * pub     Public key bytes.
 * pubSz   Size of public key in bytes.
 * endian  Public key bytes passed in as big-endian or little-endian.
 * returns BAD_FUNC_ARGS when pub is NULL,
 *         BUFFER_E when size of public key is zero;
 *         ECC_OUT_OF_RANGE_E if the high bit is set;
 *         ECC_BAD_ARG_E if key length is not 32 bytes, public key value is
 *         zero or one; and
 *         0 otherwise.
 */
int wc_curve25519_check_public(const byte* pub, word32 pubSz, int endian)
{
    word32 i;

    if (pub == NULL)
        return BAD_FUNC_ARG;

    /* Check for empty key data */
    if (pubSz == 0)
        return BUFFER_E;

    /* Check key length */
    if (pubSz != CURVE25519_KEYSIZE)
        return ECC_BAD_ARG_E;


    if (endian == EC25519_LITTLE_ENDIAN) {
        /* Check for value of zero or one */
        for (i = CURVE25519_KEYSIZE - 1; i > 0; i--) {
            if (pub[i] != 0)
                break;
        }
        if (i == 0 && (pub[0] == 0 || pub[0] == 1))
            return ECC_BAD_ARG_E;

        /* Check high bit set */
        if (pub[CURVE25519_KEYSIZE - 1] & 0x80)
            return ECC_OUT_OF_RANGE_E;

        /* Check for order-1 or higher. */
        if (pub[CURVE25519_KEYSIZE - 1] == 0x7f) {
            for (i = CURVE25519_KEYSIZE - 2; i > 0; i--) {
                if (pub[i] != 0xff)
                    break;
            }
            if (i == 0 && (pub[0] >= 0xec))
                return ECC_BAD_ARG_E;
         }
    }
    else {
        /* Check for value of zero or one */
        for (i = 0; i < CURVE25519_KEYSIZE - 1; i++) {
            if (pub[i] != 0)
                break;
        }
        if (i == CURVE25519_KEYSIZE - 1 && (pub[i] == 0 || pub[i] == 1))
            return ECC_BAD_ARG_E;

        /* Check high bit set */
        if (pub[0] & 0x80)
            return ECC_OUT_OF_RANGE_E;

        /* Check for order-1 or higher. */
        if (pub[0] == 0x7f) {
            for (i = 1; i < CURVE25519_KEYSIZE - 1; i++) {
                if (pub[i] != 0)
                    break;
            }
            if (i == CURVE25519_KEYSIZE - 1 && (pub[i] >= 0xec))
                return ECC_BAD_ARG_E;
         }
    }

    return 0;
}

#endif /* HAVE_CURVE25519_KEY_IMPORT */


#ifdef HAVE_CURVE25519_KEY_EXPORT

/* export curve25519 private key only raw (Big endian)
 * outLen is in/out size
 * return 0 on success */
int wc_curve25519_export_private_raw(curve25519_key* key, byte* out,
                                     word32* outLen)
{
    return wc_curve25519_export_private_raw_ex(key, out, outLen,
                                               EC25519_BIG_ENDIAN);
}

/* export curve25519 private key only raw (Big or Little endian)
 * outLen is in/out size
 * return 0 on success */
int wc_curve25519_export_private_raw_ex(curve25519_key* key, byte* out,
                                        word32* outLen, int endian)
{
    /* sanity check */
    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    /* check size of outgoing buffer */
    if (*outLen < CURVE25519_KEYSIZE) {
        *outLen = CURVE25519_KEYSIZE;
        return ECC_BAD_ARG_E;
    }

    /* export private scalar with endianness */
    curve25519_copy_point(out, key->k, endian);
    *outLen = CURVE25519_KEYSIZE;

    return 0;
}

/* curve25519 key pair export (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_key_raw(curve25519_key* key,
                                 byte* priv, word32 *privSz,
                                 byte* pub, word32 *pubSz)
{
    return wc_curve25519_export_key_raw_ex(key, priv, privSz,
                                           pub, pubSz, EC25519_BIG_ENDIAN);
}

/* curve25519 key pair export (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_key_raw_ex(curve25519_key* key,
                                    byte* priv, word32 *privSz,
                                    byte* pub, word32 *pubSz,
                                    int endian)
{
    int ret;

    /* export private part */
    ret = wc_curve25519_export_private_raw_ex(key, priv, privSz, endian);
    if (ret != 0)
        return ret;

    /* export public part */
    return wc_curve25519_export_public_ex(key, pub, pubSz, endian);
}

#endif /* HAVE_CURVE25519_KEY_EXPORT */

#ifdef HAVE_CURVE25519_KEY_IMPORT

/* curve25519 private key import (Big endian)
 * Public key to match private key needs to be imported too
 * return 0 on success */
int wc_curve25519_import_private_raw(const byte* priv, word32 privSz,
                                     const byte* pub, word32 pubSz,
                                     curve25519_key* key)
{
    return wc_curve25519_import_private_raw_ex(priv, privSz, pub, pubSz,
                                               key, EC25519_BIG_ENDIAN);
}

/* curve25519 private key import (Big or Little endian)
 * Public key to match private key needs to be imported too
 * return 0 on success */
int wc_curve25519_import_private_raw_ex(const byte* priv, word32 privSz,
                                        const byte* pub, word32 pubSz,
                                        curve25519_key* key, int endian)
{
    int ret;

    /* import private part */
    ret = wc_curve25519_import_private_ex(priv, privSz, key, endian);
    if (ret != 0)
        return ret;

    /* import public part */
    return wc_curve25519_import_public_ex(pub, pubSz, key, endian);
}

/* curve25519 private key import only. (Big endian)
 * return 0 on success */
int wc_curve25519_import_private(const byte* priv, word32 privSz,
                                 curve25519_key* key)
{
    return wc_curve25519_import_private_ex(priv, privSz,
                                           key, EC25519_BIG_ENDIAN);
}

/* curve25519 private key import only. (Big or Little endian)
 * return 0 on success */
int wc_curve25519_import_private_ex(const byte* priv, word32 privSz,
                                    curve25519_key* key, int endian)
{
    /* sanity check */
    if (key == NULL || priv == NULL) {
        return BAD_FUNC_ARG;
    }

    /* check size of incoming keys */
    if ((int)privSz != CURVE25519_KEYSIZE) {
        return ECC_BAD_ARG_E;
    }

#ifdef WOLFSSL_SE050
    /* release NXP resources if set */
    se050_curve25519_free_key(key);
#endif

    /* import private scalar with endianness */
    curve25519_copy_point(key->k, priv, endian);
    key->privSet = 1;

    key->dp = &curve25519_sets[0];

    /* Clamp the key */
    return curve25519_priv_clamp(key->k);
}

#endif /* HAVE_CURVE25519_KEY_IMPORT */

int wc_curve25519_init_ex(curve25519_key* key, void* heap, int devId)
{
    if (key == NULL)
       return BAD_FUNC_ARG;

    XMEMSET(key, 0, sizeof(*key));

    /* currently the format for curve25519 */
    key->dp = &curve25519_sets[0];

#ifdef WOLF_CRYPTO_CB
    key->devId = devId;
#else
    (void)devId;
#endif
    (void)heap; /* if needed for XMALLOC/XFREE in future */

#ifndef FREESCALE_LTC_ECC
    fe_init();
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_curve25519_init_ex key->k", key->k, CURVE25519_KEYSIZE);
#endif

    return 0;
}

int wc_curve25519_init(curve25519_key* key)
{
    return wc_curve25519_init_ex(key, NULL, INVALID_DEVID);
}

/* Clean the memory of a key */
void wc_curve25519_free(curve25519_key* key)
{
    if (key == NULL)
       return;

#ifdef WOLFSSL_SE050
    se050_curve25519_free_key(key);
#endif

    key->dp = NULL;
    ForceZero(key->k, sizeof(key->k));
    XMEMSET(&key->p, 0, sizeof(key->p));
    key->pubSet = 0;
    key->privSet = 0;
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(key, sizeof(curve25519_key));
#endif
}

/* get key size */
int wc_curve25519_size(curve25519_key* key)
{
    if (key == NULL)
        return 0;

    return key->dp->size;
}

#endif /*HAVE_CURVE25519*/
