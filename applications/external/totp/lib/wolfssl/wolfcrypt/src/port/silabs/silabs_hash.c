/* silabs_hash.c
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

/* Silicon Labs Secure Element Manager Hashing Function */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif


#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_SILABS_SE_ACCEL)

#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <wolfssl/wolfcrypt/port/silabs/silabs_hash.h>

static sl_se_hash_type_t wc_silabs_gethashtype(enum wc_HashType type)
{
 /* set init state */
    switch (type) {
    case WC_HASH_TYPE_SHA:
        return SL_SE_HASH_SHA1;
        break;
    case WC_HASH_TYPE_SHA224:
        return SL_SE_HASH_SHA224;
        break;
    case WC_HASH_TYPE_SHA256:
        return SL_SE_HASH_SHA256;
#ifdef WOLFSSL_SILABS_SHA384
    case WC_HASH_TYPE_SHA384:
        return SL_SE_HASH_SHA384;
#endif
#ifdef WOLFSSL_SILABS_SHA512
    case WC_HASH_TYPE_SHA512:
        return SL_SE_HASH_SHA512;
#endif
    default:
        break;
    }
    return SL_SE_HASH_NONE;
}

int wc_silabs_se_hash_init (wc_silabs_sha_t* sha, enum wc_HashType type)
{
    int ret = 0;
    sl_status_t rr;
    sl_se_hash_type_t ht = wc_silabs_gethashtype(type);

    if (ht == SL_SE_HASH_NONE) {
        return NOT_COMPILED_IN;
    }

    /* set sizes and state */
    XMEMSET(sha, 0, sizeof(wc_silabs_sha_t));

    /* set init state */
#ifdef WOLFSSL_SILABS_SE_ACCEL_3
    rr = sl_se_hash_starts(&sha->hash_ctx, &sha->cmd_ctx, ht,
        &sha->hash_type_ctx);
#else
    rr = sl_se_hash_multipart_starts(&sha->hash_type_ctx, &sha->cmd_ctx, ht);
#endif
    if (rr != SL_STATUS_OK) {
      ret = WC_HW_E;
    }

    return ret;
}

int wc_silabs_se_hash_update(wc_silabs_sha_t* sha, const byte* data,
    word32 len)
{
    int ret = 0;
    sl_status_t status;

#ifdef WOLFSSL_SILABS_SE_ACCEL_3
    status = sl_se_hash_update(&sha->hash_ctx, data, len);
#else
    status = sl_se_hash_multipart_update(&sha->hash_type_ctx, &sha->cmd_ctx,
        data, len);
#endif
    if (status != SL_STATUS_OK) {
        ret = WC_HW_E;
    }
    return ret;
}

int wc_silabs_se_hash_final(wc_silabs_sha_t* sha, byte* hash, word32 len)
{
    int ret = 0;
    sl_status_t status;

#ifdef WOLFSSL_SILABS_SE_ACCEL_3
    status = sl_se_hash_finish(&sha->hash_ctx, hash, len);
#else
    status = sl_se_hash_multipart_finish(&sha->hash_type_ctx, &sha->cmd_ctx,
        hash, len);
#endif
    if (status != SL_STATUS_OK) {
        ret = WC_HW_E;
    }
    return ret;
}


static int wc_HashUpdate_ex(wc_silabs_sha_t* sha, const byte* data, word32 len)
{
    int ret = 0;

    if (sha == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    ret = wolfSSL_CryptHwMutexLock();
    if (ret == 0) {
        ret = wc_silabs_se_hash_update(sha, data, len);
        wolfSSL_CryptHwMutexUnLock();
    }
    return ret;
}

static int wc_HashFinal_ex(wc_silabs_sha_t* sha, byte* hash, word32 len)
{
    int ret = 0;

    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wolfSSL_CryptHwMutexLock();
    if (ret == 0) {
        ret = wc_silabs_se_hash_final(sha, hash, len);
        wolfSSL_CryptHwMutexUnLock();
    }

    return ret;
}

#ifndef NO_SHA

int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    (void)heap;

    return wc_silabs_se_hash_init(&sha->silabsCtx, WC_HASH_TYPE_SHA);
}

int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len)
{
    return wc_HashUpdate_ex(&sha->silabsCtx, data, len);
}

int wc_ShaFinal(wc_Sha* sha, byte* hash)
{
    int ret = wc_HashFinal_ex(&sha->silabsCtx, hash, WC_SHA_DIGEST_SIZE);

    (void)wc_InitSha(sha);  /* reset state */

    return ret;
}

#endif /* ! NO_SHA */

#ifndef NO_SHA256
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    (void)heap;

    return wc_silabs_se_hash_init(&sha->silabsCtx, WC_HASH_TYPE_SHA256);
}


int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len)
{
    return wc_HashUpdate_ex(&sha->silabsCtx, data, len);
}

int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    int ret = wc_HashFinal_ex(&sha->silabsCtx, hash, WC_SHA256_DIGEST_SIZE);

    (void)wc_InitSha256(sha);  /* reset state */

    return ret;
}
#endif /* ! NO_SHA256 */

#ifdef WOLFSSL_SHA224
int wc_InitSha224_ex(wc_Sha224* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    (void)heap;

    return wc_silabs_se_hash_init(&sha->silabsCtx, WC_HASH_TYPE_SHA224);
}


int wc_Sha224Update(wc_Sha224* sha, const byte* data, word32 len)
{
    return wc_HashUpdate_ex(&sha->silabsCtx, data, len);
}

int wc_Sha224Final(wc_Sha224* sha, byte* hash)
{
    int ret = wc_HashFinal_ex(&sha->silabsCtx, hash, WC_SHA224_DIGEST_SIZE);

    (void)wc_InitSha224(sha);  /* reset state */

    return ret;
}
#endif /* WOLFSSL_SHA224 */

#ifdef WOLFSSL_SILABS_SHA384
int wc_InitSha384_ex(wc_Sha384* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    (void)heap;

    return wc_silabs_se_hash_init(&sha->silabsCtx, WC_HASH_TYPE_SHA384);
}


int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len)
{
    return wc_HashUpdate_ex(&sha->silabsCtx, data, len);
}

int wc_Sha384Final(wc_Sha384* sha, byte* hash)
{
    int ret = wc_HashFinal_ex(&sha->silabsCtx, hash, WC_SHA384_DIGEST_SIZE);

    (void)wc_InitSha384(sha);  /* reset state */

    return ret;
}
#endif /* WOLFSSL_SILABS_SHA384 */

#ifdef WOLFSSL_SILABS_SHA512
int wc_InitSha512_ex(wc_Sha512* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId;
    (void)heap;

    return wc_silabs_se_hash_init(&sha->silabsCtx, WC_HASH_TYPE_SHA512);
}


int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len)
{
    return wc_HashUpdate_ex(&sha->silabsCtx, data, len);
}

int wc_Sha512Final(wc_Sha512* sha, byte* hash)
{
    int ret = wc_HashFinal_ex(&sha->silabsCtx, hash, WC_SHA512_DIGEST_SIZE);

    (void)wc_InitSha512(sha);  /* reset state */

    return ret;
}
#endif /* WOLFSSL_SILABS_SHA512 */

#endif /* WOLFSSL_SILABS_SE_ACCEL */
