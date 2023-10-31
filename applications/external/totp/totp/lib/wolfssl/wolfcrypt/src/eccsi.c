/* eccsi.c
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

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef WOLFCRYPT_HAVE_ECCSI

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/eccsi.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#ifdef WOLFSSL_HAVE_SP_ECC
    #include <wolfssl/wolfcrypt/sp.h>
#endif

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

/**
 * Initialize the components of the ECCSI key and use the specified curve.
 *
 * Must be called before performing any operations.
 * Free the ECCSI key with wc_FreeEccsiKey() when no longer needed.
 *
 * @param  [in]  key    ECCSI key to initialize.
 * @param  [in]  heap   Heap hint.
 * @param  [in]  devId  Device identifier.
 *                      Use INVALID_DEVID when no device used.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int wc_InitEccsiKey_ex(EccsiKey* key, int keySz, int curveId, void* heap,
        int devId)
{
    int err = 0;
    ecc_key* ecc = NULL;
    ecc_key* pubkey = NULL;
    EccsiKeyParams* params = NULL;

    if (key == NULL) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        XMEMSET(key, 0, sizeof(*key));
        key->heap = heap;
        params = &key->params;

        err = wc_ecc_init_ex(&key->ecc, heap, devId);
    }
    if (err == 0) {
        ecc = &key->ecc;
        err = wc_ecc_init_ex(&key->pubkey, heap, devId);
    }
    if (err == 0) {
        key->pvt = wc_ecc_new_point_h(heap);
        if (key->pvt == NULL) {
            err = MEMORY_E;
        }
    }
    if (err == 0) {
        pubkey = &key->pubkey;
        err = mp_init_multi(&params->order,
#ifdef WOLFCRYPT_ECCSI_CLIENT
                &params->a, &params->b, &params->prime, &key->tmp, &key->ssk
#else
                NULL, NULL, NULL, NULL, NULL
#endif
                );
    }
    if (err == 0) {
        err = wc_ecc_set_curve(&key->ecc, keySz, curveId);
    }
    if (err == 0) {
        err = wc_ecc_set_curve(&key->pubkey, keySz, curveId);
    }

    if (err != 0) {
        wc_ecc_free(pubkey);
        wc_ecc_free(ecc);
    }

    return err;
}

/**
 * Initialize the components of the ECCSI key.
 * Default curve used: NIST_P256 (ECC_SECP256R1)
 *
 * Must be called before performing any operations.
 * Free the ECCSI key with wc_FreeEccsiKey() when no longer needed.
 *
 * @param  [in]  key    ECCSI key to initialize.
 * @param  [in]  heap   Heap hint.
 * @param  [in]  devId  Device identifier.
 *                      Use INVALID_DEVID when no device used.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int wc_InitEccsiKey(EccsiKey* key, void* heap, int devId)
{
    return wc_InitEccsiKey_ex(key, 32, ECC_SECP256R1, heap, devId);
}

/**
 * Frees memory associated with components of the ECCIS key.
 *
 * Must be called when finished with the ECCIS key.
 *
 * @param  [in]  key  ECCIS key.
 */
void wc_FreeEccsiKey(EccsiKey* key)
{
    if (key != NULL) {
        EccsiKeyParams* params = &key->params;

        wc_ecc_del_point_h(params->base, key->heap);
#ifdef WOLFCRYPT_ECCSI_CLIENT
        mp_free(&key->ssk);
        mp_free(&key->tmp);
        mp_free(&params->prime);
        mp_free(&params->b);
        mp_free(&params->a);
#endif
        mp_free(&params->order);
        wc_ecc_del_point_h(key->pvt, key->heap);
        wc_ecc_free(&key->pubkey);
        wc_ecc_free(&key->ecc);
        XMEMSET(key, 0, sizeof(*key));
    }
}

/*
 * Order, as a hex string in the ECC object, loaded into mp_int in key.
 * Flags that the order is available so it isn't loaded multiple times.
 *
 * @param  [in]  key  ECCSI key.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_load_order(EccsiKey* key)
{
    int err = 0;

    if (!key->params.haveOrder) {
        err = mp_read_radix(&key->params.order, key->ecc.dp->order,
                MP_RADIX_HEX);
        if (err == 0) {
            key->params.haveOrder = 1;
        }
    }

    return err;
}

#ifdef WOLFCRYPT_ECCSI_CLIENT
/*
 * Parameters, as a hex strings in the ECC object, loaded into mp_ints in key.
 *
 * Parameters loaded: order, A, B, prime.
 * Flags that each parameter is available so they aren't loaded multiple times.
 *
 * @param  [in]  key  ECCSI key.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_load_ecc_params(EccsiKey* key)
{
    int err = 0;
    EccsiKeyParams* params = &key->params;

    err = eccsi_load_order(key);
    if ((err == 0) && (!params->haveA)) {
        err = mp_read_radix(&params->a, key->ecc.dp->Af, MP_RADIX_HEX);
        if (err == 0) {
            params->haveA = 1;
        }
    }
    if ((err == 0) && (!params->haveB)) {
        err = mp_read_radix(&params->b, key->ecc.dp->Bf, MP_RADIX_HEX);
        if (err == 0) {
            params->haveB = 1;
        }
    }
    if ((err == 0) && (!params->havePrime)) {
        err = mp_read_radix(&params->prime, key->ecc.dp->prime, MP_RADIX_HEX);
        if (err == 0) {
            params->havePrime = 1;
        }
    }

    return err;
}
#endif /* WOLFCRYPT_ECCSI_CLIENT */

/*
 * Get the base point, hex encoded in the ECC object, as an ecc_point.
 *
 * Flags that base is available so it isn't loaded multiple times.

 * @param  [in]   key   ECCSI key.
 * @param  [out]  base  Base point of curve.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_load_base(EccsiKey* key)
{
    int err = 0;
    EccsiKeyParams* params = &key->params;

    if (!params->haveBase) {
        if (params->base == NULL) {
            params->base = wc_ecc_new_point_h(key->heap);
            if (params->base == NULL) {
                err = MEMORY_E;
            }
        }
        if (err == 0) {
            err = mp_read_radix(params->base->x, key->ecc.dp->Gx, MP_RADIX_HEX);
        }
        if (err == 0) {
            err = mp_read_radix(params->base->y, key->ecc.dp->Gy, MP_RADIX_HEX);
        }
        if (err == 0) {
            err = mp_set(params->base->z, 1);
        }
        if (err == 0) {
            params->haveBase = 1;
        }
    }

    return err;
}

/*
 * Encode the base point of the curve.
 *
 * Base point is hex encoded in the ECC object or cached as an ECC point from
 * previous load calls.
 *
 * @param  [in]   key     ECCSI key.
 * @param  [out]  data    Buffer to encode base point into.
 * @param  [out]  dataSz  Length of base point in bytes.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_encode_base(EccsiKey* key, byte* data, word32* dataSz)
{
    int err;
    int idx = wc_ecc_get_curve_idx(key->ecc.dp->id);

    err = eccsi_load_base(key);
    if (err == 0) {
        err = wc_ecc_export_point_der(idx, key->params.base, data, dataSz);
    }

    return err;
}

#ifndef WOLFSSL_HAVE_SP_ECC
/*
 * Convert the KPAK to montgomery form.
 *
 * The KPAK is needed in Montgomery form for verification.
 *
 * @param  [in]  key      ECCSI key.
 * @return  0 on success.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_kpak_to_mont(EccsiKey* key)
{
    int err = 0;
    ecc_point* kpak = &key->ecc.pubkey;
    mp_int* mu = &key->tmp;
    mp_int* prime = &key->params.prime;

    if (!key->kpakMont) {
        err = mp_montgomery_calc_normalization(mu, prime);
        if (err == 0) {
            err = mp_mulmod(kpak->x, mu, prime, kpak->x);
        }
        if (err == 0) {
            err = mp_mulmod(kpak->y, mu, prime, kpak->y);
        }
        if (err == 0) {
            err = mp_mulmod(kpak->z, mu, prime, kpak->z);
        }
        if (err == 0) {
            key->kpakMont = 1;
        }
    }

    return err;
}
#endif

/*
 * Convert the KPAK from montgomery form.
 *
 * The KPAK is needed in Montgomery form for verification.
 *
 * @param  [in]  key      ECCSI key.
 * @return  0 on success.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_kpak_from_mont(EccsiKey* key)
{
    int err = 0;
    ecc_point* kpak = &key->ecc.pubkey;
    mp_digit mp;
    mp_int* prime = &key->params.prime;

    if (key->kpakMont) {
        err = mp_montgomery_setup(prime, &mp);
        if (err == 0) {
            err = mp_montgomery_reduce(kpak->x, prime, mp);
        }
        if (err == 0) {
            err = mp_montgomery_reduce(kpak->y, prime, mp);
        }
        if (err == 0) {
            err = mp_montgomery_reduce(kpak->z, prime, mp);
        }
        if (err == 0) {
            key->kpakMont = 0;
        }
    }

    return err;
}

/*
 * Compute HS = hash( G | KPAK | ID | PVT )
 *
 * Use when making a (SSK,PVT) pair, signing and verifying.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   id        Identity to create hash from.
 * @param  [in]   idSz      Length of identity in bytes.
 * @param  [in]   pvt       Public Validation Token (PVT) as an ECC point.
 * @param  [out]  hash      Buffer to hold hash data.
 * @param  [out]  hashSz    Length of hash data in bytes.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_compute_hs(EccsiKey* key, enum wc_HashType hashType,
        const byte* id, word32 idSz, ecc_point* pvt, byte* hash, byte* hashSz)
{
    int err;
    word32 dataSz = 0;
    int idx = wc_ecc_get_curve_idx(key->ecc.dp->id);
    ecc_point* kpak = &key->ecc.pubkey;
    int hash_inited = 0;

    /* HS = hash( G | KPAK | ID | PVT ) */
    err = wc_HashInit_ex(&key->hash, hashType, key->heap, INVALID_DEVID);
    if (err == 0) {
        hash_inited = 1;
        /* Base Point - G */
        dataSz = sizeof(key->data);
        err = eccsi_encode_base(key, key->data, &dataSz);
    }
    if (err == 0) {
        err = wc_HashUpdate(&key->hash, hashType, key->data, dataSz);
    }
    if (err == 0) {
        err = eccsi_kpak_from_mont(key);
    }
    if (err == 0) {
        dataSz = sizeof(key->data);
        /* KPAK - public key */
        err = wc_ecc_export_point_der(idx, kpak, key->data, &dataSz);
    }
    if (err == 0) {
        err = wc_HashUpdate(&key->hash, hashType, key->data, dataSz);
    }
    if (err == 0) {
        /* Id - Signer's ID */
        err = wc_HashUpdate(&key->hash, hashType, id, idSz);
    }
    if (err == 0) {
        dataSz = sizeof(key->data);
        /* PVT - Public Validation Token */
        err = wc_ecc_export_point_der(idx, pvt, key->data, &dataSz);
    }
    if (err == 0) {
        /* PVT - Public Validation Token */
        err = wc_HashUpdate(&key->hash, hashType, key->data, dataSz);
    }
    if (err == 0) {
        err = wc_HashFinal(&key->hash, hashType, hash);
    }

    if (err == 0) {
        *hashSz = (byte)wc_HashGetDigestSize(hashType);
    }

    if (hash_inited) {
        (void)wc_HashFree(&key->hash, hashType);
    }

    return err;
}

#ifdef WOLFCRYPT_ECCSI_KMS
/**
 * Generate KMS Secret Auth Key (KSAK) and KMS Public Auth Key (KPAK).
 *
 * RFC 6507, Section 4.2
 *
 * Called when establishing a new KMS.\n
 * KSAK must be kept secret while KPAK is required by clients for signing
 * and verifying.\n
 * Export key using wc_ExportEccsiKey(), once generated, to reuse the key.\n
 * Export KPAK using wc_ExportEccsiPublicKey(), once generate to send to
 * clients.
 *
 * Creates a random private key and multiplies it by the base point to calculate
 * the public key.
 *
 * @param  [in]  key      ECCSI key.
 * @param  [in]  rng      Random number generator.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or rng is NULL.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_MakeEccsiKey(EccsiKey* key, WC_RNG* rng)
{
    int err = 0;

    if ((key == NULL) || (rng == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = wc_ecc_make_key_ex(rng, key->ecc.dp->size, &key->ecc,
                key->ecc.dp->id);
    }

    return err;
}

/*
 * Encode a point into a buffer.
 *
 * X and y ordinate of point concatenated. Each number is zero padded tosize.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]      point    ECC point to encode.
 * @param  [in]      size     Size of prime in bytes - maximum ordinate length.
 * @param  [out]     data     Buffer to hold encoded data.
 *                            NULL when needing length of encoded data.
 * @param  [in,out]  sz       In, the size of the buffer in bytes.
 *                            Out, the size of the encoded data in bytes.
 * @param  [in]      raw      On 0, prepend descriptor byte.
 *                            On 1, only include ordinates.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or sz is NULL.
 * @return  LENGTH_ONLY_E when data is NULL - sz will hold the size in bytes of
 *          the encoded data.
 * @return  BUFFER_E when size of buffer is too small.
 */
static int eccsi_encode_point(ecc_point* point, word32 size, byte* data,
        word32* sz, int raw)
{
    int err = 0;

    if (data == NULL) {
        *sz = size * 2 + !raw;
        err = LENGTH_ONLY_E;
    }
    if ((err == 0) && (*sz < size * 2 + !raw)) {
        err = BUFFER_E;
    }

    if (err == 0) {
        if (!raw) {
            data[0] = 0x04;
            data++;
        }

        /* Write out the point's x ordinate into key size bytes. */
        err = mp_to_unsigned_bin_len(point->x, data, (int)size);
    }
    if (err == 0) {
        data += size;
        /* Write out the point's y ordinate into key size bytes. */
        err = mp_to_unsigned_bin_len(point->y, data, (int)size);
    }
    if (err == 0) {
        *sz = size * 2 + !raw;
    }

    return err;
}

/*
 * Decode the data into an ECC point.
 *
 * X and y ordinate of point concatenated. Each number is zero padded to
 * key size. Supports prepended descriptor byte (0x04).
 *
 * @param  [out]  point  ECC point to encode.
 * @param  [in]   size   Size of prime in bytes - maximum ordinate length.
 * @param  [in]   data   Encoded public key.
 * @param  [in]   sz     Size of the encoded public key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or z is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  ASN_PARSE_E when format byte is invalid.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_decode_point(ecc_point* point, word32 size, const byte* data,
        word32 sz)
{
    int err = 0;

    if ((sz != size * 2) && (sz != size * 2 + 1)) {
        err = BUFFER_E;
    }

    if ((err == 0) && (sz & 1)) {
        if (data[0] != 0x04) {
            err = ASN_PARSE_E;
        }
        data++;
    }

    if (err == 0) {
        /* Read the public key point's x value from key size bytes. */
        err = mp_read_unsigned_bin(point->x, data, size);
    }
    if (err == 0) {
        data += size;
        /* Read the public key point's y value from key size bytes. */
        err = mp_read_unsigned_bin(point->y, data, size);
    }
    if (err == 0) {
        err = mp_set(point->z, 1);
    }

    return err;
}

/*
 * Encode the ECCSI key.
 *
 * Encodes the private key as big-endian bytes of fixed length.
 * Encodes the public key x and y ordinates as big-endian bytes of fixed length.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [out]     data  Buffer to hold encoded ECCSI key.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails (WOLFSSL_SMALL_STACK).
 */
static int eccsi_encode_key(EccsiKey* key, byte* data)
{
    int err;
    word32 sz = (word32)key->ecc.dp->size * 2;

    /* Write out the secret value into key size bytes. */
    err = mp_to_unsigned_bin_len(wc_ecc_key_get_priv(&key->ecc), data,
        key->ecc.dp->size);
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Write the public key. */
        err = eccsi_encode_point(&key->ecc.pubkey, (word32)key->ecc.dp->size,
                data, &sz, 1);
    }

    return err;
}

/**
 * Export the ECCSI key as encoded public/private ECC key.
 *
 * Use when saving the KMS key pair.
 *
 * Private key, x ordinate of public key and y ordinate of public key
 * concatenated. Each number is zero padded to key size.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [out]     data  Buffer to hold encoded ECCSI key.
 *                         NULL when requesting required length.
 * @param  [in,out]  sz    On in, size of buffer in bytes.
 *                         On out, size of encoded ECCSI key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or sz is NULL
 * @return  BAD_STATE_E when no key to export.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 * @return  BUFFER_E when the buffer passed in is too small.
 * @return  MEMORY_E when dynamic memory allocation fails (WOLFSSL_SMALL_STACK).
 */
int wc_ExportEccsiKey(EccsiKey* key, byte* data, word32* sz)
{
    int err = 0;

    if ((key == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY)) {
        err = BAD_STATE_E;
    }

    if (err == 0) {
        if (data == NULL) {
            *sz = (word32)(key->ecc.dp->size * 3);
            err = LENGTH_ONLY_E;
        }
        else if (*sz < (word32)key->ecc.dp->size * 3) {
            err = BUFFER_E;
        }
        else {
            *sz = (word32)(key->ecc.dp->size * 3);
        }
    }
    if (err == 0) {
        err = eccsi_kpak_from_mont(key);
    }
    if (err == 0) {
        /* Encode key */
        err = eccsi_encode_key(key, data);
    }

    return err;
}

/*
 * Import the ECCSI key as encoded public/private ECC key.
 *
 * Decodes the private key as big-endian bytes of fixed length.
 * Decodes the public key x and y ordinates as big-endian bytes of fixed length.
 *
 * @param  [in]  key   ECCSI key.
 * @param  [in]  data  Buffer holding encoded ECCSI key.
 * @return  0 on success.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_decode_key(EccsiKey* key, const byte* data)
{
    int err;

    /* Read the secret value from key size bytes. */
    err = mp_read_unsigned_bin(wc_ecc_key_get_priv(&key->ecc), data,
        (word32)key->ecc.dp->size);
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Read public key. */
        err = eccsi_decode_point(&key->ecc.pubkey, (word32)key->ecc.dp->size,
                data, (word32)(key->ecc.dp->size * 2));
    }

    return err;
}

/**
 * Import the ECCSI key as encoded public/private ECC key.
 *
 * Use when restoring the KMS key pair.
 *
 * Private key, x ordinate of public key and y ordinate of public key
 * concatenated. Each number is zero padded to key size.
 *
 * @param  [in]  key   ECCSI key.
 * @param  [in]  data  Buffer holding encoded ECCSI key.
 * @param  [in]  sz    Size of encoded ECCSI key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or data is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_ImportEccsiKey(EccsiKey* key, const byte* data, word32 sz)
{
    int err = 0;

    if ((key == NULL) || (data == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (sz != (word32)key->ecc.dp->size * 3)) {
        err = BUFFER_E;
    }

    if (err == 0) {
        key->kpakMont = 0;

        /* Decode key */
        err = eccsi_decode_key(key, data);
    }
    if (err == 0) {
        key->ecc.type = ECC_PRIVATEKEY;
    }

    return err;
}

/**
 * Export the ECCSI private key.
 *
 * Use when saving the KMS key.
 *
 * Private key is zero padded to key size.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [out]     data  Buffer to hold encoded ECCSI private key.
 *                         NULL when requesting required length.
 * @param  [in,out]  sz    On in, size of buffer in bytes.
 *                         On out, size of encoded ECCSI private key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or sz is NULL
 * @return  BAD_STATE_E when no key to export.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 * @return  BUFFER_E when the buffer passed in is too small.
 * @return  MEMORY_E when dynamic memory allocation fails (WOLFSSL_SMALL_STACK).
 */
int wc_ExportEccsiPrivateKey(EccsiKey* key, byte* data, word32* sz)
{
    int err = 0;

    if ((key == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY)) {
        err = BAD_STATE_E;
    }

    if (err == 0) {
        if (data == NULL) {
            *sz = (word32)key->ecc.dp->size;
            err = LENGTH_ONLY_E;
        }
        else if (*sz < (word32)key->ecc.dp->size) {
            err = BUFFER_E;
        }
        else {
            *sz = (word32)key->ecc.dp->size;
        }
    }
    if (err == 0) {
        err = mp_to_unsigned_bin_len(wc_ecc_key_get_priv(&key->ecc), data,
            key->ecc.dp->size);
    }

    return err;
}

/**
 * Import the ECCSI private key.
 *
 * Use when restoring the KMS key pair.
 *
 * Private key is zero padded to key size.
 *
 * @param  [in]  key   ECCSI key.
 * @param  [in]  data  Buffer holding encoded ECCSI private key.
 * @param  [in]  sz    Size of encoded ECCSI private key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or data is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_ImportEccsiPrivateKey(EccsiKey* key, const byte* data, word32 sz)
{
    int err = 0;

    if ((key == NULL) || (data == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (sz != (word32)key->ecc.dp->size)) {
        err = BUFFER_E;
    }

    if (err == 0) {
        err = mp_read_unsigned_bin(wc_ecc_key_get_priv(&key->ecc), data,
            (word32)key->ecc.dp->size);
    }

    return err;
}

/**
 * Export the KMS Public Auth Key (KPAK) from the ECCSI object.
 *
 * KPAK is required by all clients in order to perform cryptographic operations.
 *
 * X and y ordinate of public key concatenated. Each number is zero padded to
 * key size.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]      key      ECCSI key.
 * @param  [out]     data     Buffer to hold the encoded public key.
 * @param  [in,out]  sz       On in, size of buffer in bytes.
 *                            On out, length of encoded public key in bytes.
 * @param  [in]      raw   On 0, prepend descriptor byte.
 *                         On 1, only include ordinates.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or sz is NULL.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 * @return  BUFFER_E when the buffer passed in is too small.
 */
int wc_ExportEccsiPublicKey(EccsiKey* key, byte* data, word32* sz, int raw)
{
    int err = 0;

    if ((key == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY) &&
            (key->ecc.type != ECC_PUBLICKEY)) {
        err = BAD_STATE_E;
    }

    if ((err == 0) && (data != NULL)) {
        err = eccsi_kpak_from_mont(key);
    }
    if (err == 0) {
        /* Write out public key. */
        err = eccsi_encode_point(&key->ecc.pubkey, (word32)key->ecc.dp->size,
            data, sz, raw);
    }

    return err;
}

/*
 * Generates an (SSK, PVT) Pair - signing key pair.
 *
 * RFC 6507, Section 5.1.1
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   rng       Random number generator.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   id        Identity to create hash from.
 * @param  [in]   idSz      Length of identity in bytes.
 * @param  [out]  ssk       Secret Signing Key as an MP integer.
 * @param  [out]  pvt       Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_make_pair(EccsiKey* key, WC_RNG* rng,
        enum wc_HashType hashType, const byte* id, word32 idSz, mp_int* ssk,
        ecc_point* pvt)
{
    int err = 0;
    byte hashSz = 0;
    int genTryCnt = 0;

    do {
        /* Don't infinitely make pairs when random number generator fails. */
        if ((++genTryCnt) > ECCSI_MAX_GEN_COUNT) {
            err = RNG_FAILURE_E;
        }

        if (err == 0) {
            wc_ecc_free(&key->pubkey);

            /* Step 1 and 2: Generate ephemeral key - v, PVT = [v]G */
            err = wc_ecc_make_key_ex(rng, key->ecc.dp->size, &key->pubkey,
                    key->ecc.dp->id);
        }
        if (err == 0) {
            err = wc_ecc_copy_point(&key->pubkey.pubkey, pvt);
        }

        /* Step 3: Compute HS */
        if (err == 0) {
            hashSz = (byte)sizeof(key->data);
            err = eccsi_compute_hs(key, hashType, id, idSz, pvt, key->data,
                    &hashSz);
        }

        /* Step 4: Compute SSK = ( KSAK + HS * v ) modulo q */
        if (err == 0) {
            err = mp_read_unsigned_bin(ssk, key->data, hashSz);
        }
        if (err == 0) {
            err = mp_mulmod(ssk, wc_ecc_key_get_priv(&key->pubkey),
                &key->params.order, ssk);
        }
        if (err == 0) {
            err = mp_addmod(ssk, wc_ecc_key_get_priv(&key->ecc),
                &key->params.order, ssk);
        }
    }
    while ((err == 0) && (mp_iszero(ssk) ||
            (mp_cmp(ssk, wc_ecc_key_get_priv(&key->ecc)) == MP_EQ)));
    /* Step 5: ensure SSK and HS are non-zero (code lines above) */

    /* Step 6: Copy out SSK (done during calc) and PVT. Erase v */
    mp_forcezero(wc_ecc_key_get_priv(&key->pubkey));

    return err;
}

/**
 * Generates an (SSK, PVT) Pair - signing key pair.
 *
 * RFC 6507, Section 5.1.1
 *
 * ID should include information to indicate a revocation date.\n
 * SSK must be zeroized after sending to client.\n
 * SSK is sent to signing client only.\n
 * PVT is sent to all client types.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   rng       Random number generator.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   id        Identity to create hash from.
 * @param  [in]   idSz      Length of identity in bytes.
 * @param  [out]  ssk       Secret Signing Key as an MP integer.
 * @param  [out]  pvt       Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, rng, id, ssk or pvt is NULL.
 * @return  BAD_STATE_E when curve not set (key not set).
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_MakeEccsiPair(EccsiKey* key, WC_RNG* rng, enum wc_HashType hashType,
        const byte* id, word32 idSz, mp_int* ssk, ecc_point* pvt)
{
    int err = 0;

    if ((key == NULL) || (rng == NULL) || (id == NULL) || (ssk == NULL) ||
            (pvt == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY)) {
        err = BAD_STATE_E;
    }

    if (err == 0) {
        err = eccsi_load_order(key);
    }
    if (err == 0) {
        err = eccsi_make_pair(key, rng, hashType, id, idSz, ssk, pvt);
    }

    return err;
}

/**
 * Encode the SSK and PVT into a buffer.
 *
 * SSK and PVT required by client signing messages.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [in]      ssk   Secret Signing Key as an MP integer.
 * @param  [in]      pvt   Public Validation Token (PVT) as an ECC point.
 * @param  [out]     data  Buffer to encode key pair into.
 * @param  [in,out]  sz    In, size of buffer in bytes.
 *                         Out, size of encoded pair data in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ssk, pvt or sz is NULL.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 */
int wc_EncodeEccsiPair(const EccsiKey* key, mp_int* ssk, ecc_point* pvt,
        byte* data, word32* sz)
{
    int err = 0;

    if ((key == NULL) || (ssk == NULL) || (pvt == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if ((err == 0) && (data == NULL)) {
        *sz = (word32)(key->ecc.dp->size * 3);
        err = LENGTH_ONLY_E;
    }
    if ((err == 0) && (*sz < (word32)(key->ecc.dp->size * 3))) {
        err = BUFFER_E;
    }

    if (err == 0) {
        err = mp_to_unsigned_bin_len(ssk, data, key->ecc.dp->size);
    }
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Write out the PVT's x ordinate into key size bytes. */
        err = mp_to_unsigned_bin_len(pvt->x, data, key->ecc.dp->size);
    }
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Write out the PVT's y ordinate into key size bytes. */
        err = mp_to_unsigned_bin_len(pvt->y, data, key->ecc.dp->size);
    }
    if (err == 0) {
        *sz = (word32)(key->ecc.dp->size * 3);
    }

    return err;
}

/**
 * Encode the Secret Signing Key (SSK).
 *
 * Use when saving the key pair.
 *
 * SSK is zero padded to key size.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [in]      ssk   Secret Signing Key as an MP integer.
 * @param  [out]     data  Buffer to hold encoded SSK.
 *                         NULL when requesting required length.
 * @param  [in,out]  sz    On in, size of buffer in bytes.
 *                         On out, size of encoded ECCSI key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ssk or sz is NULL
 * @return  BAD_STATE_E when no key to export.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 * @return  BUFFER_E when the buffer passed in is too small.
 * @return  MEMORY_E when dynamic memory allocation fails (WOLFSSL_SMALL_STACK).
 */
int wc_EncodeEccsiSsk(const EccsiKey* key, mp_int* ssk, byte* data, word32* sz)
{
    int err = 0;

    if ((key == NULL) || (ssk == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY)) {
        err = BAD_STATE_E;
    }

    if (err == 0) {
        if (data == NULL) {
            *sz = (word32)key->ecc.dp->size;
            err = LENGTH_ONLY_E;
        }
        else if (*sz < (word32)key->ecc.dp->size) {
            err = BUFFER_E;
        }
        else {
            *sz = (word32)key->ecc.dp->size;
        }
    }
    if (err == 0) {
        err = mp_to_unsigned_bin_len(ssk, data, key->ecc.dp->size);
    }

    return err;
}

/**
 * Decode the Secret Signing Key (SSK).
 *
 * Use when restoring the key pair.
 *
 * SSK is zero padded to key size.
 *
 * @param  [in]   key   ECCSI key.
 * @param  [in]   data  Buffer holding encoded ECCSI key.
 * @param  [in]   sz    Size of encoded ECCSI key in bytes.
 * @param  [out]  ssk   Secret Signing Key as an MP integer.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, data or ssk is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_DecodeEccsiSsk(const EccsiKey* key, const byte* data, word32 sz,
        mp_int* ssk)
{
    int err = 0;

    if ((key == NULL) || (data == NULL) || (ssk == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (sz != (word32)key->ecc.dp->size)) {
        err = BUFFER_E;
    }

    if (err == 0) {
        err = mp_read_unsigned_bin(ssk, data, (word32)key->ecc.dp->size);
    }

    return err;
}

/**
 * Encode the PVT into a buffer.
 *
 * PVT required by client verifying messages.
 *
 * X and y ordinate of public key concatenated. Each number is zero padded to
 * key size.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]      key   ECCSI key.
 * @param  [in]      pvt   Public Validation Token (PVT) as an ECC point.
 * @param  [out]     data  Buffer to encode key pair into.
 * @param  [in,out]  sz    In, size of buffer in bytes.
 *                         Out, size of encoded pair data in bytes.
 * @param  [in]      raw   On 0, prepend descriptor byte.
 *                         On 1, only include ordinates.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, pvt or sz is NULL.
 * @return  BAD_STATE_E when PVT has not been set.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 */
int wc_EncodeEccsiPvt(const EccsiKey* key, ecc_point* pvt, byte* data,
        word32* sz, int raw)
{
    int err = 0;

    if ((key == NULL) || (pvt == NULL) || (sz == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = eccsi_encode_point(pvt, (word32)key->ecc.dp->size, data, sz, raw);
    }

    return err;
}

#endif /* WOLFCRYPT_ECCSI_KMS */

#ifdef WOLFCRYPT_ECCSI_CLIENT
/**
 * Decode the SSK and PVT data into separate variables.
 *
 * A signing client decodes the data so that it can validate the pair and sign.
 *
 * @param  [in]   key   ECCSI key.
 * @param  [in]   data  Buffer holding key pair data.
 * @param  [in]   sz    Size of data in bytes.
 * @param  [out]  ssk   Secret Signing Key as an MP integer.
 * @param  [out]  pvt   Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, data, ssk or pvt is NULL.
 * @return  LENGTH_ONLY_E when data is NULL - sz is set.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_DecodeEccsiPair(const EccsiKey* key, const byte* data, word32 sz,
        mp_int* ssk, ecc_point* pvt)
{
    int err = 0;

    if ((key == NULL) || (data == NULL) || (ssk == NULL) || (pvt == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (sz != (word32)(key->ecc.dp->size * 3))) {
        err = BUFFER_E;
    }

    if (err == 0) {
        /* Read the SSK value from key size bytes. */
        err = mp_read_unsigned_bin(ssk, data, (word32)key->ecc.dp->size);
    }
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Read the PVT's x value from key size bytes. */
        err = mp_read_unsigned_bin(pvt->x, data, (word32)key->ecc.dp->size);
    }
    if (err == 0) {
        data += key->ecc.dp->size;
        /* Read the PVT's y value from key size bytes. */
        err = mp_read_unsigned_bin(pvt->y, data, (word32)key->ecc.dp->size);
    }
    if (err == 0) {
        err = mp_set(pvt->z, 1);
    }

    return err;
}

/**
 * Decode the PVT data into an ECC point.
 *
 * A verifying client decodes the data so that it can verify a message.
 *
 * X and y ordinate of public key concatenated. Each number is zero padded to
 * key size.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]   key   ECCSI key.
 * @param  [in]   data  Buffer holding PVT data.
 * @param  [in]   sz    Size of data in bytes.
 * @param  [out]  pvt   Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, data, ssk or pvt is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  ASN_PARSE_E when format byte is invalid.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_DecodeEccsiPvt(const EccsiKey* key, const byte* data, word32 sz,
        ecc_point* pvt)
{
    int err = 0;

    if ((key == NULL) || (data == NULL) || (pvt == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = eccsi_decode_point(pvt, (word32)key->ecc.dp->size, data, sz);
    }

    return err;
}

/**
 * Decode the PVT data, from a signature, into an ECC point.
 *
 * A verifying client decodes the data so that it can calculate the identity
 * hash.
 *
 * X and y ordinate of public key concatenated. Each number is zero padded to
 * key size.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]   key   ECCSI key.
 * @param  [in]   sig   Buffer holding signature data.
 * @param  [in]   sz    Size of data in bytes.
 * @param  [out]  pvt   Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, data, ssk or pvt is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  ASN_PARSE_E when format byte is invalid.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_DecodeEccsiPvtFromSig(const EccsiKey* key, const byte* sig, word32 sz,
        ecc_point* pvt)
{
    int err = 0;

    if ((key == NULL) || (sig == NULL) || (pvt == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        word32 rSz = (word32)(key->ecc.dp->size * 2);
        err = eccsi_decode_point(pvt, (word32)key->ecc.dp->size, sig + rSz,
                sz - rSz);
    }

    return err;
}

/**
 * Import the KMS Public Auth Key (KPAK) into the ECCSI object.
 *
 * Clients import the KPAK to perform cryptographic operations.
 *
 * X and y ordinate of public key concatenated. Each number is zero padded to
 * key size.
 * Descriptor byte (0x04) is prepended when not raw.
 *
 * @param  [in]  key      ECCSI key.
 * @param  [in]  data     Encoded public key as an array of bytes.
 * @param  [in]  sz       Length of encoded KPAK in bytes.
 * @param  [in]  trusted  1 when public key is trusted.
 *                        0 when validation is required to be performed.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or data is NULL.
 * @return  BUFFER_E when size of data is not equal to the expected size.
 * @return  ASN_PARSE_E when format byte is invalid.
 * @return  ECC_OUT_OF_RANGE_E when point is invalid.
 * @return  ECC_INF_E when point is at infinity and invalid.
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 */
int wc_ImportEccsiPublicKey(EccsiKey* key, const byte* data, word32 sz,
        int trusted)
{
    int err = 0;

    if ((key == NULL) || (data == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        key->kpakMont = 0;

        /* Read the public key. */
        err = eccsi_decode_point(&key->ecc.pubkey, (word32)key->ecc.dp->size,
                data, sz);
    }
    if (err == 0) {
        key->ecc.type = ECC_PUBLICKEY;
    }
    if ((err == 0) && (!trusted)) {
       err = wc_ecc_check_key(&key->ecc);
    }

    return err;
}

/*
 * Scalar multiply the base point of the curve and add a point.
 *
 * @param  [in]   key   ECCSI key.
 * @param  [in]   n     MP integer representing scalar to multiply by.
 * @param  [in]   a     ECC point to add.
 * @param  [out]  res   ECC point representation of the resulting point.
 * @param  [in]   mp    Montgomery reduction multiplier.
 * @param  [in]   map   0 indicates to leave in projective representation.
 *                      1 indicates map projective point to affine.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_mulmod_base_add(EccsiKey* key, const mp_int* n,
        ecc_point* a, ecc_point* res, mp_digit mp, int map)
{
    int err = 0;

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
    if ((key->ecc.idx != ECC_CUSTOM_IDX) &&
            (ecc_sets[key->ecc.idx].id == ECC_SECP256R1)) {
        err = sp_ecc_mulmod_base_add_256(n, a, 1, res, map, key->heap);
    }
    else
#endif
#endif
#ifndef WOLFSSL_SP_MATH
    {
        EccsiKeyParams* params = &key->params;
        err = wc_ecc_mulmod(n, params->base, params->base, &params->a,
                &params->prime, 0);
        key->params.haveBase = 0;
        if (err == 0) {
            err = ecc_projective_add_point(params->base, a, res, &params->a,
                    &params->prime, mp);
        }
        if ((err == 0) && map) {
            err = ecc_map(res, &params->prime, mp);
        }
    }
#else
    {
        err = NOT_COMPILED_IN;
    }
    (void)mp;
#endif

    return err;
}

/*
 * Scalar multiply a point on the curve.
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   n      MP integer representing scalar to multiply by.
 * @param  [in]   point  ECC point representation of a point on the curve.
 * @param  [out]  res    ECC point representation of the resulting point.
 * @param  [in]   map    0 indicates to leave in projective representation.
 *                       1 indicates map projective point to affine.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_mulmod_point(EccsiKey* key, const mp_int* n, ecc_point* point,
        ecc_point* res, int map)
{
    int err;

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
    if ((key->ecc.idx != ECC_CUSTOM_IDX) &&
            (ecc_sets[key->ecc.idx].id == ECC_SECP256R1)) {
        err = sp_ecc_mulmod_256(n, point, res, map, key->heap);
    }
    else
#endif
#endif
    {
        EccsiKeyParams* params = &key->params;

        err = wc_ecc_mulmod(n, point, res, &params->a, &params->prime, map);
    }

    return err;
}

/*
 * Scalar multiply a point on the curve and add a.
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   n      MP integer representing scalar to multiply by.
 * @param  [in]   point  ECC point representation of a point on the curve.
 * @param  [in]   a      ECC point to add.
 * @param  [out]  res    ECC point representation of the resulting point.
 * @param  [in]   mp     Montgomery reduction multiplier.
 * @param  [in]   map    0 indicates to leave in projective representation.
 *                       1 indicates map projective point to affine.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_mulmod_point_add(EccsiKey* key, const mp_int* n,
        ecc_point* point, ecc_point* a, ecc_point* res, mp_digit mp, int map)
{
#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
    int err = NOT_COMPILED_IN;

    if ((key->ecc.idx != ECC_CUSTOM_IDX) &&
            (ecc_sets[key->ecc.idx].id == ECC_SECP256R1)) {
        err = sp_ecc_mulmod_add_256(n, point, a, 0, res, map, key->heap);
    }

    (void)mp;

    return err;
#endif
#else
    int err;
    EccsiKeyParams* params = &key->params;

    err = wc_ecc_mulmod(n, point, res, &params->a, &params->prime, 0);
    if (err == 0) {
        err = ecc_projective_add_point(res, a, res, &key->params.a,
                &params->prime, mp);
    }
    if ((err == 0) && map) {
        err = ecc_map(res, &params->prime, mp);
    }

    return err;
#endif
}

/**
 * Validate an (SSV, PVT) Pair.
 *
 * RFC 6507, Section 5.1.2
 *
 * A signing client should validate the key pair before first use.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   id        Identity to create hash from.
 * @param  [in]   idSz      Length of identity in bytes.
 * @param  [in]   ssk       Secret Signing Key as an MP integer.
 * @param  [in]   pvt       Public Validation Token (PVT) as an ECC point.
 * @param  [out]  valid     1 when pair is valid and 0 otherwise.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, id, ssk, pvt or valid is NULL.
 * @return  BAD_STATE_E when curve not set (key not set).
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  IS_POINT_E when point is not on the curve.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_ValidateEccsiPair(EccsiKey* key, enum wc_HashType hashType,
        const byte* id, word32 idSz, const mp_int* ssk, ecc_point* pvt,
        int* valid)
{
    int err = 0;
    ecc_point* res = NULL;
    mp_int* hs = NULL;
    mp_digit mp = 0;
    byte hashSz = 0;
    EccsiKeyParams* params = NULL;

    if ((key == NULL) || (id == NULL) || (ssk == NULL) || (pvt == NULL) ||
            (valid == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY) &&
            (key->ecc.type != ECC_PUBLICKEY)) {
        err = BAD_STATE_E;
    }

    if (err != 0)
        return err;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    params = &key->params;
    hs = &key->tmp;
    res = &key->pubkey.pubkey;

    err = eccsi_load_base(key);

    if (err == 0) {
       err = eccsi_load_ecc_params(key);
    }
    if (err == 0) {
        err = mp_montgomery_setup(&params->prime, &mp);
    }

    /* Step 1: Validate PVT is on curve */
    if (err == 0) {
        err = wc_ecc_is_point(pvt, &params->a, &params->b, &params->prime);
        if (err == -1) {
            err = IS_POINT_E;
        }
    }

    /* Step 2: Compute HS = hash( G | KPAK | ID | PVT ) */
    if (err == 0) {
        hashSz = (byte)sizeof(key->data);
        /* Converts KPAK from mont. */
        err = eccsi_compute_hs(key, hashType, id, idSz, pvt, key->data,
                &hashSz);
    }

    /* Step 3: Validate that KPAK = [SSK]G - [HS]PVT */
    if (err == 0) {
        err = mp_read_unsigned_bin(hs, key->data, hashSz);
    }
    /* [HS]PVT */
    if (err == 0) {
        err = eccsi_mulmod_point(key, hs, pvt, res, 0);
    }
    /* -[HS]PVT */
    if (err == 0) {
        err = mp_sub(&params->prime, res->y, res->y);
    }
    /* [SSK]G + -[HS]PVT */
    if (err == 0) {
        err = eccsi_mulmod_base_add(key, ssk, res, res, mp, 1);
    }
    if (valid != NULL) {
        *valid = (err == 0);
        if (err == 0) {
            ecc_point* kpak = &key->ecc.pubkey;
            /* Compare KPAK and [SSK]G + -[HS]PVT */
            *valid = (wc_ecc_cmp_point(res, kpak) == MP_EQ);
        }
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}

/**
 * Validate Public Validation Token (PVT) is on the curve.
 *
 * RFC 6507, Section 5.1.2, Step 1
 *
 * A verifying client should validate the PVT before first use.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   pvt       Public Validation Token (PVT) as an ECC point.
 * @param  [out]  valid     1 when PVT is valid and 0 otherwise.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, pvt or valid is NULL.
 * @return  BAD_STATE_E when curve not set (key not set).
 * @return  MP_MEM or MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_ValidateEccsiPvt(EccsiKey* key, const ecc_point* pvt, int* valid)
{
    int err = 0;

    if ((key == NULL)| (pvt == NULL) || (valid == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = wc_ecc_set_curve(&key->pubkey, key->ecc.dp->size,
                key->ecc.dp->id);
    }
    if (err == 0) {
        err = wc_ecc_copy_point(pvt, &key->pubkey.pubkey);
    }
    if (err == 0) {
        *valid = (wc_ecc_check_key(&key->pubkey) == 0);
    }

    return err;
}

/**
 * Creates the Hash of the ID and PVT with the ECCSI key.
 *
 * The hash ID is required as input to the sign and verify operations.\n
 * Signing clients may cache this value.
 *
 * RFC 6507, Section 5.2.1, Step 3
 *
 * Set the calculated hash internally for use.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   id        Identity to create hash from.
 * @param  [in]   idSz      Length of identity in bytes.
 * @param  [in]   pvt       Public Validation Token (PVT) as an ECC point.
 * @param  [out]  hash      Buffer to hold hash result.
 * @param  [out]  hashSz    Length of hash data in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, id, pvt, hash or hashSz is NULL.
 * @return  BAD_FUNC_ARG when hash size doesn't match curve size.
 * @return  BAD_STATE_E when public key not set.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_HashEccsiId(EccsiKey* key, enum wc_HashType hashType, const byte* id,
        word32 idSz, ecc_point* pvt, byte* hash, byte* hashSz)
{
    int err = 0;
    int dgstSz = -1;
    int curveSz = -1;

    if ((key == NULL) || (id == NULL) || (pvt == NULL) || (hash == NULL) ||
            (hashSz == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY) &&
            (key->ecc.type != ECC_PUBLICKEY)) {
        err = BAD_STATE_E;
    }
    /* Ensure digest output size matches curve size (RFC 6507 4.1). */
    if (err == 0) {
        dgstSz = wc_HashGetDigestSize(hashType);
        if (dgstSz < 0) {
            err = dgstSz;
        }
    }
    if (err == 0) {
        curveSz = wc_ecc_get_curve_size_from_id(key->ecc.dp->id);
        if (curveSz < 0) {
            err = curveSz;
        }
    }
    if ((err == 0) && (dgstSz != curveSz)) {
        err = BAD_FUNC_ARG;
    }
    /* Load the curve parameters for operations */
    if (err == 0) {
       err = eccsi_load_ecc_params(key);
    }
    if (err == 0) {
        err = eccsi_compute_hs(key, hashType, id, idSz, pvt, hash, hashSz);
    }
    if (err == 0) {
        XMEMCPY(key->idHash, hash, *hashSz);
        key->idHashSz = *hashSz;
    }

    return err;
}

/**
 * Set the identity hash for use with signing/verification.
 *
 * @param  [in]  key     ECCSI key.
 * @param  [in]  hash    Buffer with hash of identity.
 * @param  [in]  hashSz  Length of hash data in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or hash is NULL, or hashSz is greater than
 *          WC_MAX_DIGEST_SIZE.
 */
int wc_SetEccsiHash(EccsiKey* key, const byte* hash, byte hashSz)
{
    int err = 0;

    if ((key == NULL) || (hash == NULL) || (hashSz > WC_MAX_DIGEST_SIZE)) {
        err = BAD_FUNC_ARG;
    }
    if (err == 0) {
        XMEMCPY(key->idHash, hash, hashSz);
        key->idHashSz = hashSz;
    }

    return err;
}

/**
 * Set an (SSV, PVT) Pair for signing.
 *
 * @param  [in]   key  ECCSI key.
 * @param  [in]   ssk  Secret Signing Key as an MP integer.
 * @param  [in]   pvt  Public Validation Token (PVT) as an ECC point.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ssk or pvt is NULL.
 * @return  MP math errors when copy fails
 */
int wc_SetEccsiPair(EccsiKey* key, const mp_int* ssk, const ecc_point* pvt)
{
    int err = 0;

    if ((key == NULL) || (ssk == NULL) || (pvt == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = mp_copy(ssk, &key->ssk);
    }

    if (err == 0) {
        err = wc_ecc_copy_point(pvt, key->pvt);
    }

    return err;
}

#ifdef ECCSI_ORDER_MORE_BITS_THAN_PRIME
/*
 * Fit the number to the maximum number of bytes.
 *
 * If the number is too big then subtract from order.
 * RFC 6507, Section 5.2.1, Note at end.
 * This should only happen when order is larger than prime in bits.
 *
 * @param  [in]   a      MP integer to fix.
 * @param  [in]   order  MP integer representing order of curve.
 * @param  [in]   m      Maximum number of bytes to encode into.
 * @param  [out]  r      MP integer that is the result after fixing.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_fit_to_octets(const mp_int* a, mp_int* order, int m,
        mp_int* r)
{
    int err;

    if (mp_count_bits(a) > m * 8) {
        err = mp_sub(order, (mp_int*)a, r);
    }
    else
    {
        err = mp_copy(a, r);
    }

    return err;
}
#else
/*
 * Fit the number to the maximum number of bytes.
 *
 * If the number is too big then subtract from order.
 * RFC 6507, Section 5.2.1, Note at end.
 * This should only happen when order is larger than prime in bits.
 *
 * @param  [in]   a      MP integer to fix.
 * @param  [in]   order  MP integer representing order of curve.
 * @param  [in]   m      Maximum number of bytes to encode into.
 * @param  [out]  r      MP integer that is the result after fixing.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
static int eccsi_fit_to_octets(const mp_int* a, const mp_int* order, int m,
        mp_int* r)
{
    (void)order;
    (void)m;

    /* Duplicate line to stop static analyzer complaining. */
    return mp_copy(a, r);
}
#endif

/*
 * Compute the HE = hash( HS | r | M ), hash value of signature.
 *
 * Partial result required for signing and verification.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   r         MP integer that is the first signature element.
 * @param  [in]   msg       Message of signature.
 * @param  [in]   msgSz     Length of message in bytes.
 * @param  [out]  he        Signature hash.
 * @param  [out]  heSz      Length of signature hash in bytes
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_compute_he(EccsiKey* key, enum wc_HashType hashType,
        mp_int* r, const byte* msg, word32 msgSz, byte* he, word32* heSz)
{
    int err = 0;
    word32 dataSz = (word32)key->ecc.dp->size;
    int hash_inited = 0;

    /* HE = hash( HS | r | M ) */
    err = wc_HashInit_ex(&key->hash, hashType, key->heap, INVALID_DEVID);
    if (err == 0) {
        hash_inited = 1;
        /* HS */
        err = wc_HashUpdate(&key->hash, hashType, key->idHash, key->idHashSz);
    }
    if (err == 0) {
        err = mp_to_unsigned_bin_len(r, key->data, (int)dataSz);
    }
    if (err == 0) {
        /* r */
        err = wc_HashUpdate(&key->hash, hashType, key->data, dataSz);
    }
    if (err == 0) {
        /* M */
        err = wc_HashUpdate(&key->hash, hashType, msg, msgSz);
    }
    if (err == 0) {
        err = wc_HashFinal(&key->hash, hashType, he);
    }
    if (err == 0) {
        *heSz = (word32)wc_HashGetDigestSize(hashType);
    }

    if (hash_inited) {
        (void)wc_HashFree(&key->hash, hashType);
    }

    return err;
}

/*
 * Encode the signature = ( r | s | PVT )
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   r      MP integer that is the first signature element.
 * @param  [in]   s      MP integer that is the second signature element.
 * @param  [in]   pvt    ECC point representing Public Validation Token.
 * @param  [out]  sig    Signature of message.
 * @param  [out]  sigSz  Length of signature in bytes.
 */
static int eccsi_encode_sig(const EccsiKey* key, mp_int* r, mp_int* s,
        byte* sig, word32* sigSz)
{
    int err;
    word32 sz = (word32)key->ecc.dp->size;

    err = mp_to_unsigned_bin_len(r, sig, (int)sz);
    if (err == 0) {
        err = mp_to_unsigned_bin_len(s, sig + sz, (int)sz);
    }
    if (err == 0) {
        *sigSz = (word32)(key->ecc.dp->size * 2 + 1);
        err = wc_ecc_export_point_der(wc_ecc_get_curve_idx(key->ecc.dp->id),
                 key->pvt, sig + sz * 2, sigSz);
    }
    if (err == 0) {
        *sigSz = sz * 4 + 1;
    }

    return err;
}

/*
 * Sign the ECCSI hash (of ID with the key) to two mp_int objects: r and s.
 *
 * RFC 6507, Section 5.2.1, Steps 1 to 4
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   rng       Random number generator.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   msg       Message to sign.
 * @param  [in]   msgSz     Length of message in bytes.
 * @param  [out]  r         First big number integer part of signature.
 * @param  [out]  s         Second big number integer part of signature.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_gen_sig(EccsiKey* key, WC_RNG* rng, enum wc_HashType hashType,
        const byte* msg, word32 msgSz, mp_int* r, mp_int* s)
{
    int err = 0;
    int sz = key->ecc.dp->size;
    word32 heSz = 0;
    const mp_int* jx = NULL;
    mp_int* he = &key->tmp;
    int genTryCnt = 0;

    do {
        /* Don't infinitely gen sigs when random number generator fails. */
        if ((++genTryCnt) > ECCSI_MAX_GEN_COUNT) {
            err = RNG_FAILURE_E;
        }

        if (err == 0) {
            wc_ecc_free(&key->pubkey);

            /* Step 1 and 2: Generate ephemeral key - j, J = [j]G, r = Jx */
            err = wc_ecc_make_key_ex(rng, sz, &key->pubkey, key->ecc.dp->id);
        }
        if (err == 0) {
            jx = key->pubkey.pubkey.x;
            err = eccsi_fit_to_octets(jx, &key->params.order, sz, r);
        }

        /* Step 3: Compute HE = hash( HS | r | M ) */
        if (err == 0) {
            err = eccsi_compute_he(key, hashType, r, msg, msgSz, key->data,
                    &heSz);
        }

        /* Step 4: Verify that HE + r * SSK is non-zero modulo q */
        if (err == 0) {
            err = mp_read_unsigned_bin(he, key->data, heSz);
        }
        /* s' = r * SSK */
        if (err == 0) {
            err = mp_mulmod(r, &key->ssk, &key->params.order, s);
        }
        /* s' = HE + r * SSK */
        if (err == 0) {
            err = mp_addmod(he, s, &key->params.order, s);
        }
    }
    while ((err == 0) && (mp_iszero(s) || (mp_cmp(s, he) == MP_EQ)));

    return err;
}


/**
 * Sign the ECCSI hash (of ID with the key).
 *
 * RFC 6507, Section 5.2.1
 *
 * Must have imported KPAK using wc_ImportEccsiPublicKey() before calling.\n
 * Use wc_HashEccsiId() to calculate the hash and wc_SetEccsiHash() to set
 * the identity hash to use.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   rng       Random number generator.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   msg       Message to sign.
 * @param  [in]   msgSz     Length of message in bytes.
 * @param  [out]  sig       Signature of message.
 * @param  [out]  sigSz     Length of signature in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, rng, msg or sigSz is NULL.
 * @return  BAD_STATE_E when the curve or id hash has not been set (no key set).
 * @return  LENGTH_ONLY_E when sig is NULL - sigSz is set.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
int wc_SignEccsiHash(EccsiKey* key, WC_RNG* rng, enum wc_HashType hashType,
        const byte* msg, word32 msgSz, byte* sig, word32* sigSz)
{
    int err = 0;
    mp_int* r = NULL;
    mp_int* s = NULL;
    mp_int* j = NULL;
    word32 sz = 0;

    if ((key == NULL) || (rng == NULL) || (msg == NULL) || (sigSz == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (key->ecc.type != ECC_PUBLICKEY) &&
            (key->ecc.type != ECC_PRIVATEKEY)) {
        err = BAD_STATE_E;
    }
    if ((err == 0) && (sig != NULL) && (key->idHashSz == 0)) {
        err = BAD_STATE_E;
    }

    if (err == 0)  {
        sz = (word32)key->ecc.dp->size;
        if (sig == NULL) {
            *sigSz = sz * 4 + 1;
            err = LENGTH_ONLY_E;
        }
    }
    if ((err == 0) && (*sigSz < sz * 4 + 1)) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        r = key->pubkey.pubkey.y;
        s = key->pubkey.pubkey.z;

        err = eccsi_load_order(key);
    }

    if (err == 0) {
        /* Steps 1 to 4. */
        err = eccsi_gen_sig(key, rng, hashType, msg, msgSz, r, s);
    }

    /* Step 5: s' = ( (( HE + r * SSK )^-1) * j ) modulo q, erase j */
    if (err == 0) {
        err = mp_invmod(s, &key->params.order, s);
    }
    if (err == 0) {
        j = wc_ecc_key_get_priv(&key->pubkey);
        err = mp_mulmod(s, j, &key->params.order, s);
    }
    if (err == 0) {
        mp_forcezero(j);

        /* Step 6: s = s' fitted */
        err = eccsi_fit_to_octets(s, &key->params.order, (int)sz, s);
    }

    /* Step 7: Output Signature = ( r | s | PVT ) */
    if (err == 0) {
        err = eccsi_encode_sig(key, r, s, sig, sigSz);
    }

    return err;
}

/*
 * Decode the s part of the signature = ( r | s | PVT )
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   sig    Signature of message.
 * @param  [in]   sigSz  Length of signature in bytes.
 * @param  [out]  s      MP integer that is the second signature element.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_decode_sig_s(const EccsiKey* key, const byte* sig,
        word32 sigSz, mp_int* s)
{
    int err = 0;
    word32 sz = (word32)key->ecc.dp->size;

    if (sigSz != sz * 4 + 1) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = mp_read_unsigned_bin(s, sig + sz, sz);
    }

    return err;
}

/*
 * Decode the r and pvt part of the signature = ( r | s | PVT )
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   sig    Signature of message.
 * @param  [in]   sigSz  Length of signature in bytes.
 * @param  [out]  r      MP integer that is the first signature element.
 * @param  [out]  pvt    ECC point representing Public Validation Token.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other -ve value when an internal operation fails.
 */
static int eccsi_decode_sig_r_pvt(const EccsiKey* key, const byte* sig,
        word32 sigSz, mp_int* r, ecc_point* pvt)
{
    int err = 0;
    word32 sz = (word32)key->ecc.dp->size;

    if (sigSz != sz * 4 + 1) {
        err = BAD_FUNC_ARG;
    }

    if (err == 0) {
        err = mp_read_unsigned_bin(r, sig, sz);
    }
    if (err == 0) {
        /* must free previous public point otherwise wc_ecc_import_point_der
         * could leak memory */
        mp_clear(pvt->x);
        mp_clear(pvt->y);
        mp_clear(pvt->z);

        err = wc_ecc_import_point_der(sig + sz * 2, sz * 2 + 1,
                wc_ecc_get_curve_idx(key->ecc.dp->id), pvt);
    }

    return err;
}

/*
 * Calculate Y point as part of verification process.
 *
 * Y = [HS]PVT + KPAK
 *
 * @param  [in]   key      ECCSI key.
 * @param  [in]   pvt      ECC point representing Public Validation Token.
 * @param  [in]   mp       Montgomery reduction multiplier.
 * @param  [out]  y        ECC point representing calculated value Y.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other value when an an internal operation fails.
 */
static int eccsi_calc_y(EccsiKey* key, ecc_point* pvt, mp_digit mp,
        ecc_point* y)
{
    int err;
    mp_int* hs = &key->ssk;

    err = mp_read_unsigned_bin(hs, key->idHash, key->idHashSz);
#ifndef WOLFSSL_HAVE_SP_ECC
    /* Need KPAK in montgomery form. */
    if (err == 0) {
        err = eccsi_kpak_to_mont(key);
    }
#endif
    /* [HS]PVT + KPAK */
    if (err == 0) {
        ecc_point* kpak = &key->ecc.pubkey;
        err = eccsi_mulmod_point_add(key, hs, pvt, kpak, y, mp, 1);
    }

    return err;
}

/*
 * Calculate J point as part of verification process.
 *
 * J = [s]( [HE]G + [r]Y )
 *
 * @param  [in]   key    ECCSI key.
 * @param  [in]   hem    MP int representation of HE = Hash (hs, r and message).
 * @param  [in]   sig    Signature of message.
 * @param  [in]   sigSz  Length of signature in bytes.
 * @param  [in]   y      ECC point representing [r]Y.
 * @param  [in]   mp     Montgomery reduction multiplier.
 * @param  [out]  j      ECC point representing calculated value J.
 * @return  0 on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other value when an an internal operation fails.
 */
static int eccsi_calc_j(EccsiKey* key, const mp_int* hem, const byte* sig,
        word32 sigSz, ecc_point* y, mp_digit mp, ecc_point* j)
{
    int err;
    mp_int* s = &key->tmp;

    /* [HE]G + [r]Y */
    err = eccsi_mulmod_base_add(key, hem, y, j, mp, 1);
    if (err == 0) {
        err = eccsi_decode_sig_s(key, sig, sigSz, s);
    }
    /* [s]( [HE]G + [r]Y ) */
    if (err == 0) {
        err = eccsi_mulmod_point(key, s, j, j, 1);
    }

    return err;
}

/**
 * Verify the ECCSI hash (of ID with the key).
 *
 * RFC 6507, Section 5.2.2
 *
 * Must have imported KPAK using wc_ImportEccsiPublicKey() before calling.\n
 * Use wc_HashEccsiId() to calculate the hash and wc_SetEccsiHash() to set
 * the identity hash to use.
 *
 * @param  [in]   key       ECCSI key.
 * @param  [in]   hashType  Type of hash algorithm. e.g. WC_SHA256
 * @param  [in]   msg       Message to verify.
 * @param  [in]   msgSz     Length of message in bytes.
 * @param  [in]   sig       Signature of message.
 * @param  [in]   sigSz     Length of signature in bytes.
 * @param  [out]  verified  1 when the signature was verified and 0 otherwise.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, hash, msg, sig or ret is NULL.
 * @return  BAD_STATE_E when the curve or id hash has not been set (no key set).
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Other value when an an internal operation fails.
 */
int wc_VerifyEccsiHash(EccsiKey* key, enum wc_HashType hashType,
        const byte* msg, word32 msgSz, const byte* sig, word32 sigSz,
        int* verified)
{
    int err = 0;
    byte* he = NULL;
    word32 heSz = 0;
    mp_int* r = NULL;
    mp_int* jx = NULL;
    mp_int* hem = NULL;
    ecc_point* pvt = NULL;
    ecc_point* y = NULL;
    ecc_point* j = NULL;
    mp_digit mp = 0;
    EccsiKeyParams* params = NULL;

    if ((key == NULL) || (msg == NULL) || (sig == NULL) || (verified == NULL)) {
        err = BAD_FUNC_ARG;
    }
    if ((err == 0) && (key->ecc.type != ECC_PRIVATEKEY) &&
            (key->ecc.type != ECC_PUBLICKEY)) {
        err = BAD_STATE_E;
    }
    if ((err == 0) && (key->idHashSz == 0)) {
        err = BAD_STATE_E;
    }

    if (err != 0)
        return err;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    /* Decode the signature into components. */
    r = wc_ecc_key_get_priv(&key->pubkey);
    pvt = &key->pubkey.pubkey;
    err = eccsi_decode_sig_r_pvt(key, sig, sigSz, r, pvt);

    /* Load the curve parameters for operations */
    if (err == 0) {
        err = eccsi_load_base(key);
    }
    if (err == 0) {
        err = eccsi_load_ecc_params(key);
    }
    if (err == 0) {
        params = &key->params;
        err = mp_montgomery_setup(&params->prime, &mp);
    }

    /* Step 1: Validate PVT is on curve */
    if (err == 0) {
        err = wc_ecc_is_point(pvt, &params->a, &params->b, &params->prime);
    }

    /* Step 2: Compute HS = hash( G | KPAK | ID | PVT )
     * HS is key->idHash, key->idHashSz */

    /* Step 3: Compute HE = hash( HS | r | M ) */
    if (err == 0) {
        he = key->data;
        err = eccsi_compute_he(key, hashType, r, msg, msgSz, he, &heSz);
    }

    /* Step 4: Y = [HS]PVT + KPAK */
    if (err == 0) {
        y = pvt;
        err = eccsi_calc_y(key, pvt, mp, y);
    }

    /* Step 5: Compute J = [s]( [HE]G + [r]Y ) */
    /* [r]Y */
    if (err == 0) {
        hem = &key->tmp;
        err = mp_read_unsigned_bin(hem, he, heSz);
    }
    if (err == 0) {
        err = eccsi_mulmod_point(key, r, y, y, 0);
    }
    if (err == 0) {
        j = params->base;
        err = eccsi_calc_j(key, hem, sig, sigSz, y, mp, j);
        key->params.haveBase = 0;
    }

    /* Step 6: Jx fitting, compare with r */
    if (err == 0) {
        jx = &key->tmp;
        err = eccsi_fit_to_octets(j->x, &params->order, key->ecc.dp->size, jx);
    }

    if (verified != NULL) {
        *verified = ((err == 0) && (mp_cmp(jx, r) == MP_EQ));
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}
#endif /* WOLFCRYPT_ECCSI_CLIENT */

#endif /* WOLFCRYPT_HAVE_ECCSI */

