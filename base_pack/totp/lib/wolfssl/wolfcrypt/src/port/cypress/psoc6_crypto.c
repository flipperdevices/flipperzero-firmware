/* psoc6_crypto.c
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

#if defined(WOLFSSL_PSOC6_CRYPTO)

#include <wolfssl/wolfcrypt/port/cypress/psoc6_crypto.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <stdint.h>

#ifdef HAVE_ECC
#include <wolfssl/wolfcrypt/ecc.h>
#endif

static CRYPTO_Type *crypto_base = PSOC6_CRYPTO_BASE;

/* Hook for device specific initialization */
int psoc6_crypto_port_init(void)
{
    Cy_Crypto_Core_Enable(crypto_base);
    return 0;
}

/* Sha-512 */

#ifdef WOLFSSL_SHA512
int wc_InitSha512(wc_Sha512* sha)
{
    cy_en_crypto_status_t res;
    if (!sha)
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemSet(crypto_base, sha, 0, sizeof(sha));
    res = Cy_Crypto_Core_Sha_Init(crypto_base, &sha->hash_state, CY_CRYPTO_MODE_SHA512, &sha->sha_buffers);
    if (res != CY_CRYPTO_SUCCESS)
       return (int)res;
    return (int) Cy_Crypto_Core_Sha_Start(crypto_base, &sha->hash_state);
}

int wc_InitSha512_ex(wc_Sha512* sha, void *heap, int devid)
{
    (void)heap;
    (void)devid;
    return wc_InitSha512(sha);
}

int wc_Sha512Update(wc_Sha512* sha, const byte* in, word32 sz)
{
    if ((!sha) || (!in))
        return BAD_FUNC_ARG;
    if (sz == 0)
        return 0;

    return (int)Cy_Crypto_Core_Sha_Update(crypto_base, &sha->hash_state, in, sz);
}

int wc_Sha512Final(wc_Sha512* sha, byte* hash)
{
    if ((!sha) || (!hash))
        return BAD_FUNC_ARG;
    return (int)Cy_Crypto_Core_Sha_Finish(crypto_base, &sha->hash_state, hash);
}

int wc_Sha512GetHash(wc_Sha512* sha, byte* hash)
{
    if ((!sha) || (!hash))
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemCpy(crypto_base, hash, sha->hash_state.hash, WC_SHA512_DIGEST_SIZE);
    return 0;
}

int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst)
{
    if ((!dst) || (!src))
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemCpy(crypto_base, dst, src, sizeof(wc_Sha512));
    return (int)Cy_Crypto_Core_Sha_Init(crypto_base, &dst->hash_state, CY_CRYPTO_MODE_SHA512, &dst->sha_buffers);
}

void wc_Sha512Free(wc_Sha512* sha)
{
    if (sha)
        Cy_Crypto_Core_Sha_Free(crypto_base, &sha->hash_state);
}

#endif

/* Sha-256 */

#ifndef NO_SHA256
int wc_InitSha256(wc_Sha256* sha)
{
    cy_en_crypto_status_t res;
    if (!sha)
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemSet(crypto_base, sha, 0, sizeof(sha));
    res = Cy_Crypto_Core_Sha_Init(crypto_base, &sha->hash_state, CY_CRYPTO_MODE_SHA256, &sha->sha_buffers);
    if (res != CY_CRYPTO_SUCCESS)
       return (int)res;
    return (int) Cy_Crypto_Core_Sha_Start(crypto_base, &sha->hash_state);
}

int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    if ((!sha) || (!in))
        return BAD_FUNC_ARG;
    if (sz == 0)
        return 0;

    return (int)Cy_Crypto_Core_Sha_Update(crypto_base, &sha->hash_state, in, sz);
}

int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    if ((!sha) || (!hash))
        return BAD_FUNC_ARG;
    return (int)Cy_Crypto_Core_Sha_Finish(crypto_base, &sha->hash_state, hash);
}

int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    if ((!sha) || (!hash))
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemCpy(crypto_base, hash, sha->hash_state.hash, WC_SHA256_DIGEST_SIZE);
    return 0;
}

int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    if ((!dst) || (!src))
        return BAD_FUNC_ARG;
    Cy_Crypto_Core_MemCpy(crypto_base, dst, src, sizeof(wc_Sha256));
    return (int)Cy_Crypto_Core_Sha_Init(crypto_base, &dst->hash_state, CY_CRYPTO_MODE_SHA256, &dst->sha_buffers);
}

void wc_Sha256Free(wc_Sha256* sha)
{
    if (sha)
        Cy_Crypto_Core_Sha_Free(crypto_base, &sha->hash_state);
}
#endif /* NO_SHA256 */

/* ECDSA */
#ifdef HAVE_ECC

#define MAX_ECC_KEYSIZE 66 /* Supports up to secp521r1 */
static cy_en_crypto_ecc_curve_id_t psoc6_get_curve_id(int size)
{
    switch(size) {
        case 24:
            return CY_CRYPTO_ECC_ECP_SECP192R1;
        case 28:
            return CY_CRYPTO_ECC_ECP_SECP224R1;
        case 32:
            return CY_CRYPTO_ECC_ECP_SECP256R1;
        case 48:
            return CY_CRYPTO_ECC_ECP_SECP384R1;
        case 66:
            return CY_CRYPTO_ECC_ECP_SECP521R1;
        default:
            return CY_CRYPTO_ECC_ECP_NONE;
    }
}

int psoc6_ecc_verify_hash_ex(MATH_INT_T *r, MATH_INT_T *s, const byte* hash,
                    word32 hashlen, int* verif_res, ecc_key* key)
{
    uint8_t signature_buf[MAX_ECC_KEYSIZE * 2];
    cy_stc_crypto_ecc_key ecc_key;
    uint8_t stat = 0;
    int res = -1;
    int szModulus;
    int szkbin;
    uint8_t x[MAX_ECC_KEYSIZE], y[MAX_ECC_KEYSIZE];

    if (!key || !verif_res || !r || !s || !hash)
        return -BAD_FUNC_ARG;

    /* retrieve and check sizes */
    szModulus = mp_unsigned_bin_size(key->pubkey.x);
    szkbin = mp_unsigned_bin_size(r);
    if (szModulus > MAX_ECC_KEYSIZE)
        return -BAD_FUNC_ARG;

    /* Prepare ECC key */
    ecc_key.type = PK_PUBLIC;
    ecc_key.curveID = psoc6_get_curve_id(szModulus);
    ecc_key.k = NULL;
    ecc_key.pubkey.x = x;
    ecc_key.pubkey.y = y;

    res = mp_to_unsigned_bin(key->pubkey.x, x);
    if (res == MP_OKAY)
        res = mp_to_unsigned_bin(key->pubkey.y, y);
    Cy_Crypto_Core_InvertEndianness(x, szModulus);
    Cy_Crypto_Core_InvertEndianness(y, szModulus);

    /* Prepare signature buffer */
    if (res == MP_OKAY)
        res = mp_to_unsigned_bin(r, signature_buf);
    if (res == MP_OKAY)
        res = mp_to_unsigned_bin(s, signature_buf + szkbin);
    Cy_Crypto_Core_InvertEndianness(signature_buf, szkbin);
    Cy_Crypto_Core_InvertEndianness(signature_buf + szkbin, szkbin);

    /* perform HW ECDSA */
    if (res == MP_OKAY)
        res = Cy_Crypto_Core_ECC_VerifyHash(crypto_base, signature_buf, hash, hashlen, &stat, &ecc_key);
    if (res == 0) {
        *verif_res = stat;
    }
    return res;
}
#endif /* HAVE_ECC */



#endif /* defined(WOLFSSL_PSOC6_CRYPTO) */

