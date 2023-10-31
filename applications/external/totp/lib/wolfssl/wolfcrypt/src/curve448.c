/* curve448.c
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

/* Implemented to: RFC 7748 */

/* Based On Daniel J Bernstein's curve25519 Public Domain ref10 work.
 * Reworked for curve448 by Sean Parkinson.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_CURVE448

#include <wolfssl/wolfcrypt/curve448.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

int wc_curve448_make_pub(int public_size, byte* pub, int private_size,
    const byte* priv)
{
    int ret;
    unsigned char basepoint[CURVE448_KEY_SIZE] = {5};

    if ((pub == NULL) || (priv == NULL)) {
        return ECC_BAD_ARG_E;
    }
    if ((public_size  != CURVE448_PUB_KEY_SIZE) ||
        (private_size != CURVE448_KEY_SIZE)) {
        return ECC_BAD_ARG_E;
    }

    fe448_init();

    /* compute public key */
    ret = curve448(pub, priv, basepoint);

    return ret;
}


/* Make a new curve448 private/public key.
 *
 * rng      [in]  Random number generator.
 * keysize  [in]  Size of the key to generate.
 * key      [in]  Curve448 key object.
 * returns BAD_FUNC_ARG when rng or key are NULL,
 *         ECC_BAD_ARG_E when keysize is not CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_make_key(WC_RNG* rng, int keysize, curve448_key* key)
{
    int  ret = 0;

    if ((key == NULL) || (rng == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* currently only a key size of 56 bytes is used */
    if ((ret == 0) && (keysize != CURVE448_KEY_SIZE)) {
        ret = ECC_BAD_ARG_E;
    }

    if (ret == 0) {
        /* random number for private key */
        ret = wc_RNG_GenerateBlock(rng, key->k, (word32)keysize);
    }
    if (ret == 0) {
        key->privSet = 1;

        /* clamp private */
        key->k[0] &= 0xfc;
        key->k[CURVE448_KEY_SIZE-1] |= 0x80;

        /* compute public */
        ret = wc_curve448_make_pub((int)sizeof(key->p), key->p,
                                   (int)sizeof(key->k), key->k);
        if (ret == 0) {
            key->pubSet = 1;
        }
        else {
            ForceZero(key->k, sizeof(key->k));
            XMEMSET(key->p, 0, sizeof(key->p));
        }
    }

    return ret;
}

#ifdef HAVE_CURVE448_SHARED_SECRET

/* Calculate the shared secret from the private key and peer's public key.
 * Calculation over curve448.
 * Secret encoded big-endian.
 *
 * private_key  [in]      Curve448 private key.
 * public_key   [in]      Curve448 public key.
 * out          [in]      Array to hold shared secret.
 * outLen       [in/out]  On in, the number of bytes in array.
 *                        On out, the number bytes put into array.
 * returns BAD_FUNC_ARG when a parameter is NULL or outLen is less than
 *         CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_shared_secret(curve448_key* private_key,
                              curve448_key* public_key,
                              byte* out, word32* outLen)
{
    return wc_curve448_shared_secret_ex(private_key, public_key, out, outLen,
                                        EC448_BIG_ENDIAN);
}

/* Calculate the shared secret from the private key and peer's public key.
 * Calculation over curve448.
 *
 * private_key  [in]      Curve448 private key.
 * public_key   [in]      Curve448 public key.
 * out          [in]      Array to hold shared secret.
 * outLen       [in/out]  On in, the number of bytes in array.
 *                        On out, the number bytes put into array.
 * endian       [in]      Endianness to use when encoding number in array.
 * returns BAD_FUNC_ARG when a parameter is NULL or outLen is less than
 *         CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_shared_secret_ex(curve448_key* private_key,
                                 curve448_key* public_key,
                                 byte* out, word32* outLen, int endian)
{
    unsigned char o[CURVE448_PUB_KEY_SIZE];
    int ret = 0;
    int i;

    /* sanity check */
    if ((private_key == NULL) || (public_key == NULL) || (out == NULL) ||
                        (outLen == NULL) || (*outLen < CURVE448_PUB_KEY_SIZE)) {
        ret = BAD_FUNC_ARG;
    }
    /* make sure we have a populated private and public key */
    if (ret == 0 && (!private_key->privSet || !public_key->pubSet)) {
        ret = ECC_BAD_ARG_E;
    }

    if (ret == 0) {
        ret = curve448(o, private_key->k, public_key->p);
    }
#ifdef WOLFSSL_ECDHX_SHARED_NOT_ZERO
    if (ret == 0) {
        byte t = 0;
        for (i = 0; i < CURVE448_PUB_KEY_SIZE; i++) {
            t |= o[i];
        }
        if (t == 0) {
            ret = ECC_OUT_OF_RANGE_E;
        }
    }
#endif
    if (ret == 0) {
        if (endian == EC448_BIG_ENDIAN) {
            /* put shared secret key in Big Endian format */
            for (i = 0; i < CURVE448_PUB_KEY_SIZE; i++) {
                 out[i] = o[CURVE448_PUB_KEY_SIZE - i -1];
            }
        }
        else {
            /* put shared secret key in Little Endian format */
            XMEMCPY(out, o, CURVE448_PUB_KEY_SIZE);
        }

        *outLen = CURVE448_PUB_KEY_SIZE;
    }

    ForceZero(o, CURVE448_PUB_KEY_SIZE);

    return ret;
}

#endif /* HAVE_CURVE448_SHARED_SECRET */

#ifdef HAVE_CURVE448_KEY_EXPORT

/* Export the curve448 public key.
 * Public key encoded big-endian.
 *
 * key     [in]      Curve448 public key.
 * out     [in]      Array to hold public key.
 * outLen  [in/out]  On in, the number of bytes in array.
 *                   On out, the number bytes put into array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when outLen is less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_public(curve448_key* key, byte* out, word32* outLen)
{
    return wc_curve448_export_public_ex(key, out, outLen, EC448_BIG_ENDIAN);
}

/* Export the curve448 public key.
 *
 * key     [in]      Curve448 public key.
 * out     [in]      Array to hold public key.
 * outLen  [in/out]  On in, the number of bytes in array.
 *                   On out, the number bytes put into array.
 * endian  [in]      Endianness to use when encoding number in array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when outLen is less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_public_ex(curve448_key* key, byte* out, word32* outLen,
                                 int endian)
{
    int ret = 0;

    if ((key == NULL) || (out == NULL) || (outLen == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* check and set outgoing key size */
    if ((ret == 0) && (*outLen < CURVE448_PUB_KEY_SIZE)) {
        *outLen = CURVE448_PUB_KEY_SIZE;
        ret = ECC_BAD_ARG_E;
    }
    if (ret == 0) {
        /* calculate public if missing */
        if (!key->pubSet) {
            ret = wc_curve448_make_pub((int)sizeof(key->p), key->p,
                                       (int)sizeof(key->k), key->k);
            key->pubSet = (ret == 0);
        }
    }
    if (ret == 0) {
        *outLen = CURVE448_PUB_KEY_SIZE;
        if (endian == EC448_BIG_ENDIAN) {
            int i;
            /* read keys in Big Endian format */
            for (i = 0; i < CURVE448_PUB_KEY_SIZE; i++) {
                out[i] = key->p[CURVE448_PUB_KEY_SIZE - i - 1];
            }
        }
        else {
            XMEMCPY(out, key->p, CURVE448_PUB_KEY_SIZE);
        }
    }

    return ret;
}

#endif /* HAVE_CURVE448_KEY_EXPORT */

#ifdef HAVE_CURVE448_KEY_IMPORT

/* Import a curve448 public key from a byte array.
 * Public key encoded in big-endian.
 *
 * in      [in]  Array holding public key.
 * inLen   [in]  Number of bytes of data in array.
 * key     [in]  Curve448 public key.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when inLen is less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_public(const byte* in, word32 inLen, curve448_key* key)
{
    return wc_curve448_import_public_ex(in, inLen, key, EC448_BIG_ENDIAN);
}

/* Import a curve448 public key from a byte array.
 *
 * in      [in]  Array holding public key.
 * inLen   [in]  Number of bytes of data in array.
 * key     [in]  Curve448 public key.
 * endian  [in]  Endianness of encoded number in byte array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when inLen is less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_public_ex(const byte* in, word32 inLen,
                                 curve448_key* key, int endian)
{
    int ret = 0;

    /* sanity check */
    if ((key == NULL) || (in == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* check size of incoming keys */
    if ((ret == 0) && (inLen != CURVE448_PUB_KEY_SIZE)) {
       ret = ECC_BAD_ARG_E;
    }

    if (ret == 0) {
        if (endian == EC448_BIG_ENDIAN) {
            int i;
            /* read keys in Big Endian format */
            for (i = 0; i < CURVE448_PUB_KEY_SIZE; i++) {
                key->p[i] = in[CURVE448_PUB_KEY_SIZE - i - 1];
            }
        }
        else
            XMEMCPY(key->p, in, inLen);
        key->pubSet = 1;
    }

    return ret;
}

/* Check the public key value (big or little endian)
 *
 * pub     [in]  Public key bytes.
 * pubSz   [in]  Size of public key in bytes.
 * endian  [in]  Public key bytes passed in as big-endian or little-endian.
 * returns BAD_FUNC_ARGS when pub is NULL,
 *         ECC_BAD_ARG_E when key length is not 56 bytes, public key value is
 *         zero or one;
 *         BUFFER_E when size of public key is zero;
 *         0 otherwise.
 */
int wc_curve448_check_public(const byte* pub, word32 pubSz, int endian)
{
    int ret = 0;

    if (pub == NULL) {
        ret = BAD_FUNC_ARG;
    }

    /* Check for empty key data */
    if ((ret == 0) && (pubSz == 0)) {
        ret = BUFFER_E;
    }

    /* Check key length */
    if ((ret == 0) && (pubSz != CURVE448_PUB_KEY_SIZE)) {
        ret = ECC_BAD_ARG_E;
    }

    if (ret == 0) {
        word32 i;

        if (endian == EC448_LITTLE_ENDIAN) {
            /* Check for value of zero or one */
            for (i = CURVE448_PUB_KEY_SIZE - 1; i > 0; i--) {
                if (pub[i] != 0) {
                    break;
                }
            }
            if ((i == 0) && (pub[0] == 0 || pub[0] == 1)) {
                return ECC_BAD_ARG_E;
            }
            /* Check for order-1 or higher */
            for (i = CURVE448_PUB_KEY_SIZE - 1; i > 28; i--) {
                if (pub[i] != 0xff) {
                    break;
                }
            }
            if ((i == 28) && (pub[i] == 0xff)) {
                return ECC_BAD_ARG_E;
            }
            if ((i == 28) && (pub[i] == 0xfe)) {
                for (--i; i > 0; i--) {
                    if (pub[i] != 0xff) {
                        break;
                    }
                }
                if ((i == 0) && (pub[i] >= 0xfe)) {
                    return ECC_BAD_ARG_E;
                }
            }
        }
        else {
            /* Check for value of zero or one */
            for (i = 0; i < CURVE448_PUB_KEY_SIZE-1; i++) {
                if (pub[i] != 0) {
                    break;
                }
            }
            if ((i == CURVE448_PUB_KEY_SIZE - 1) &&
                (pub[i] == 0 || pub[i] == 1)) {
                ret = ECC_BAD_ARG_E;
            }
            /* Check for order-1 or higher */
            for (i = 0; i < 27; i++) {
                if (pub[i] != 0xff) {
                    break;
                }
            }
            if ((i == 27) && (pub[i] == 0xff)) {
                return ECC_BAD_ARG_E;
            }
            if ((i == 27) && (pub[i] == 0xfe)) {
                for (++i; i < CURVE448_PUB_KEY_SIZE - 1; i--) {
                    if (pub[i] != 0xff) {
                        break;
                    }
                }
                if ((i == CURVE448_PUB_KEY_SIZE) && (pub[i] >= 0xfe)) {
                    return ECC_BAD_ARG_E;
                }
            }
        }
    }

    return ret;
}

#endif /* HAVE_CURVE448_KEY_IMPORT */


#ifdef HAVE_CURVE448_KEY_EXPORT

/* Export the curve448 private key raw form.
 * Private key encoded big-endian.
 *
 * key     [in]      Curve448 private key.
 * out     [in]      Array to hold private key.
 * outLen  [in/out]  On in, the number of bytes in array.
 *                   On out, the number bytes put into array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when outLen is less than CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_private_raw(curve448_key* key, byte* out, word32* outLen)
{
    return wc_curve448_export_private_raw_ex(key, out, outLen,
                                             EC448_BIG_ENDIAN);
}

/* Export the curve448 private key raw form.
 *
 * key     [in]      Curve448 private key.
 * out     [in]      Array to hold private key.
 * outLen  [in/out]  On in, the number of bytes in array.
 *                   On out, the number bytes put into array.
 * endian  [in]      Endianness to use when encoding number in array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when outLen is less than CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_private_raw_ex(curve448_key* key, byte* out,
                                      word32* outLen, int endian)
{
    int ret = 0;

    /* sanity check */
    if ((key == NULL) || (out == NULL) || (outLen == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* check size of outgoing buffer */
    if ((ret == 0) && (*outLen < CURVE448_KEY_SIZE)) {
        *outLen = CURVE448_KEY_SIZE;
        ret = ECC_BAD_ARG_E;
    }
    if (ret == 0) {
        *outLen = CURVE448_KEY_SIZE;

        if (endian == EC448_BIG_ENDIAN) {
            int i;
            /* put the key in Big Endian format */
            for (i = 0; i < CURVE448_KEY_SIZE; i++) {
                out[i] = key->k[CURVE448_KEY_SIZE - i - 1];
            }
        }
        else {
            XMEMCPY(out, key->k, CURVE448_KEY_SIZE);
        }
    }

    return ret;
}

/* Export the curve448 private and public keys in raw form.
 * Private and public key encoded big-endian.
 *
 * key     [in]      Curve448 private key.
 * priv    [in]      Array to hold private key.
 * privSz  [in/out]  On in, the number of bytes in private key array.
 *                   On out, the number bytes put into private key array.
 * pub     [in]      Array to hold public key.
 * pubSz   [in/out]  On in, the number of bytes in public key array.
 *                   On out, the number bytes put into public key array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE or pubSz is
 *         less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_key_raw(curve448_key* key, byte* priv, word32 *privSz,
                               byte* pub, word32 *pubSz)
{
    return wc_curve448_export_key_raw_ex(key, priv, privSz, pub, pubSz,
                                         EC448_BIG_ENDIAN);
}

/* Export the curve448 private and public keys in raw form.
 *
 * key     [in]      Curve448 private key.
 * priv    [in]      Array to hold private key.
 * privSz  [in/out]  On in, the number of bytes in private key array.
 *                   On out, the number bytes put into private key array.
 * pub     [in]      Array to hold public key.
 * pubSz   [in/out]  On in, the number of bytes in public key array.
 *                   On out, the number bytes put into public key array.
 * endian  [in]      Endianness to use when encoding number in array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE or pubSz is
 *         less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_export_key_raw_ex(curve448_key* key, byte* priv, word32 *privSz,
                                  byte* pub, word32 *pubSz, int endian)
{
    int ret;

    /* export private part */
    ret = wc_curve448_export_private_raw_ex(key, priv, privSz, endian);
    if (ret == 0) {
        /* export public part */
        ret = wc_curve448_export_public_ex(key, pub, pubSz, endian);
    }

    return ret;
}

#endif /* HAVE_CURVE448_KEY_EXPORT */

#ifdef HAVE_CURVE448_KEY_IMPORT

/* Import curve448 private and public keys from a byte arrays.
 * Private and public keys encoded in big-endian.
 *
 * piv     [in]  Array holding private key.
 * privSz  [in]  Number of bytes of data in private key array.
 * pub     [in]  Array holding public key.
 * pubSz   [in]  Number of bytes of data in public key array.
 * key     [in]  Curve448 private/public key.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE or pubSz is
 *         less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_private_raw(const byte* priv, word32 privSz,
                                   const byte* pub, word32 pubSz,
                                   curve448_key* key)
{
    return wc_curve448_import_private_raw_ex(priv, privSz, pub, pubSz, key,
                                             EC448_BIG_ENDIAN);
}

/* Import curve448 private and public keys from a byte arrays.
 *
 * piv     [in]  Array holding private key.
 * privSz  [in]  Number of bytes of data in private key array.
 * pub     [in]  Array holding public key.
 * pubSz   [in]  Number of bytes of data in public key array.
 * key     [in]  Curve448 private/public key.
 * endian  [in]  Endianness of encoded numbers in byte arrays.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE or pubSz is
 *         less than CURVE448_PUB_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_private_raw_ex(const byte* priv, word32 privSz,
                                      const byte* pub, word32 pubSz,
                                      curve448_key* key, int endian)
{
    int ret;

    /* import private part */
    ret = wc_curve448_import_private_ex(priv, privSz, key, endian);
    if (ret == 0) {
        /* import public part */
        return wc_curve448_import_public_ex(pub, pubSz, key, endian);
    }

    return ret;
}

/* Import curve448 private key from a byte array.
 * Private key encoded in big-endian.
 *
 * piv     [in]  Array holding private key.
 * privSz  [in]  Number of bytes of data in private key array.
 * key     [in]  Curve448 private/public key.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_private(const byte* priv, word32 privSz,
                               curve448_key* key)
{
    return wc_curve448_import_private_ex(priv, privSz, key, EC448_BIG_ENDIAN);
}

/* Import curve448 private key from a byte array.
 *
 * piv     [in]  Array holding private key.
 * privSz  [in]  Number of bytes of data in private key array.
 * key     [in]  Curve448 private/public key.
 * endian  [in]  Endianness of encoded number in byte array.
 * returns BAD_FUNC_ARG when a parameter is NULL,
 *         ECC_BAD_ARG_E when privSz is less than CURVE448_KEY_SIZE,
 *         0 otherwise.
 */
int wc_curve448_import_private_ex(const byte* priv, word32 privSz,
                                  curve448_key* key, int endian)
{
    int ret = 0;

    /* sanity check */
    if ((key == NULL) || (priv == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* check size of incoming keys */
    if ((ret == 0) && ((int)privSz != CURVE448_KEY_SIZE)) {
        ret = ECC_BAD_ARG_E;
    }

    if (ret == 0) {
        if (endian == EC448_BIG_ENDIAN) {
            int i;
            /* read the key in Big Endian format */
            for (i = 0; i < CURVE448_KEY_SIZE; i++) {
                key->k[i] = priv[CURVE448_KEY_SIZE - i - 1];
            }
        }
        else {
            XMEMCPY(key->k, priv, CURVE448_KEY_SIZE);
        }

        /* Clamp the key */
        key->k[0] &= 0xfc;
        key->k[CURVE448_KEY_SIZE-1] |= 0x80;

        key->privSet = 1;
    }

    return ret;
}

#endif /* HAVE_CURVE448_KEY_IMPORT */


/* Initialize the curve448 key.
 *
 * key  [in]  Curve448 key object.
 * returns BAD_FUNC_ARG when key is NULL,
 *         0 otherwise.
 */
int wc_curve448_init(curve448_key* key)
{
    int ret = 0;

    if (key == NULL) {
       ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMSET(key, 0, sizeof(*key));

        fe448_init();

    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Add("wc_curve448_init key->k", &key->k, CURVE448_KEY_SIZE);
    #endif
    }

    return ret;
}


/* Clears the curve448 key data.
 *
 * key  [in]  Curve448 key object.
 */
void wc_curve448_free(curve448_key* key)
{
    if (key != NULL) {
        ForceZero(key->k, sizeof(key->k));
        XMEMSET(key->p, 0, sizeof(key->p));
        key->pubSet = 0;
        key->privSet = 0;
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(key, sizeof(curve448_key));
    #endif
    }
}


/* Get the curve448 key's size.
 *
 * key  [in]  Curve448 key object.
 * returns 0 if key is NULL,
 *         CURVE448_KEY_SIZE otherwise.
 */
int wc_curve448_size(curve448_key* key)
{
    int ret = 0;

    if (key != NULL) {
        ret = CURVE448_KEY_SIZE;
    }

    return ret;
}

#endif /* HAVE_CURVE448 */

