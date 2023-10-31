/* psa_hash.c
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

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_HASH)

#if !defined(NO_SHA)
#include <wolfssl/wolfcrypt/sha.h>
#endif

#if !defined(NO_SHA256)
#include <wolfssl/wolfcrypt/sha256.h>
#endif

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/psa/psa.h>

#if !defined(NO_SHA) || !defined(NO_SHA256) || defined(WOLFSSL_SHA224)
static int wc_psa_hash_init_and_setup(psa_hash_operation_t *ctx,
                                      psa_algorithm_t alg)
{
    psa_status_t s;

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(ctx, 0, sizeof(*ctx));

    PSA_LOCK();
    s = psa_hash_setup(ctx, alg);
    PSA_UNLOCK();

    if (s != PSA_SUCCESS) {
        PSA_LOCK();
        psa_hash_abort(ctx);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    return 0;
}

static int wc_psa_hash_update(psa_hash_operation_t *ctx, const uint8_t *input,
                              size_t input_length)
{
    psa_status_t s;

    if (ctx == NULL || (input == NULL && input_length > 0))
        return BAD_FUNC_ARG;

    PSA_LOCK();
    s = psa_hash_update(ctx, input, input_length);
    PSA_UNLOCK();

    if (s != PSA_SUCCESS) {
        PSA_LOCK();
        psa_hash_abort(ctx);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    return 0;
}

static int wc_psa_hash_finish_setup(psa_hash_operation_t *ctx,
                                    uint8_t *output, psa_algorithm_t alg)
{
    size_t hash_length;
    psa_status_t s;

    if (ctx == NULL || output == NULL)
        return BAD_FUNC_ARG;

    PSA_LOCK();
    s = psa_hash_finish(ctx, output, PSA_HASH_LENGTH(alg), &hash_length);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS) {
        PSA_LOCK();
        psa_hash_abort(ctx);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    PSA_LOCK();
    s = psa_hash_setup(ctx, alg);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS) {
        PSA_LOCK();
        psa_hash_abort(ctx);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    return 0;
}

static int wc_psa_hash_clone(const psa_hash_operation_t *src,
                             psa_hash_operation_t *dst)
{
    psa_status_t s;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    PSA_LOCK();
    psa_hash_abort(dst);
    PSA_UNLOCK();

    PSA_LOCK();
    s = psa_hash_clone(src, dst);
    PSA_UNLOCK();

    if (s != PSA_SUCCESS)
        return WC_HW_E;

    return 0;
}

static int wc_psa_hash_abort(psa_hash_operation_t *ctx)
{
    psa_status_t s;

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    PSA_LOCK();
    s = psa_hash_abort(ctx);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        return WC_HW_E;

    return 0;
}

static int wc_psa_get_hash(psa_hash_operation_t *ctx,
                           uint8_t *out, psa_algorithm_t alg)
{
    psa_hash_operation_t tmp;
    size_t hash_length;
    psa_status_t s;

    (void)hash_length;

    if (ctx == NULL || out == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(&tmp, 0, sizeof(tmp));
    PSA_LOCK();
    s = psa_hash_clone(ctx, &tmp);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS) {
        psa_hash_abort(&tmp);
        return WC_HW_E;
    }

    PSA_LOCK();
    s = psa_hash_finish(&tmp, out, PSA_HASH_LENGTH(alg), &hash_length);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS) {
        PSA_LOCK();
        psa_hash_abort(&tmp);
        PSA_UNLOCK();
        return WC_HW_E;
    }

    return 0;
}
#endif /* !defined(NO_SHA) || !defined(NO_SHA256)  || defined(WOLFSSL_SHA224)*/

#if !defined(NO_SHA)

int wc_InitSha_ex(wc_Sha *sha, void *heap, int devId)
{
    (void)heap;
    (void)devId;

    return wc_psa_hash_init_and_setup(&sha->psa_ctx, PSA_ALG_SHA_1);
}

int wc_ShaUpdate(wc_Sha *sha, const byte *in, word32 size)
{
    return wc_psa_hash_update(&sha->psa_ctx, in, size);
}

int wc_ShaFinal(wc_Sha *sha, byte *out)
{
    return wc_psa_hash_finish_setup(&sha->psa_ctx, out, PSA_ALG_SHA_1);
}

int wc_ShaGetHash(wc_Sha *sha, byte *out)
{

    return wc_psa_get_hash(&sha->psa_ctx, out, PSA_ALG_SHA_1);
}

int wc_ShaCopy(wc_Sha *src, wc_Sha *dst)
{
    return wc_psa_hash_clone(&src->psa_ctx, &dst->psa_ctx);
}

void wc_ShaFree(wc_Sha *sha)
{
    wc_psa_hash_abort(&sha->psa_ctx);
}
#endif /* !NO_SHA */

#if !defined(NO_SHA256)

int wc_InitSha256_ex(wc_Sha256 *sha, void *heap, int devId)
{
    (void)heap;
    (void)devId;

    return wc_psa_hash_init_and_setup(&sha->psa_ctx, PSA_ALG_SHA_256);
}

int wc_Sha256Update(wc_Sha256 *sha, const byte *in, word32 size)
{
    return wc_psa_hash_update(&sha->psa_ctx, in, size);
}

int wc_Sha256Final(wc_Sha256 *sha, byte *out)
{
    return wc_psa_hash_finish_setup(&sha->psa_ctx, out, PSA_ALG_SHA_256);
}

int wc_Sha256GetHash(wc_Sha256 *sha, byte *out)
{
    return wc_psa_get_hash(&sha->psa_ctx, out, PSA_ALG_SHA_256);
}

int wc_Sha256Copy(wc_Sha256 *src, wc_Sha256 *dst)
{
    return wc_psa_hash_clone(&src->psa_ctx, &dst->psa_ctx);
}

void wc_Sha256Free(wc_Sha256 *sha)
{
    wc_psa_hash_abort(&sha->psa_ctx);
}

#endif /* !NO_SHA256 */

#if defined(WOLFSSL_SHA224)

int wc_InitSha224_ex(wc_Sha224 *sha, void *heap, int devId)
{
    (void)heap;
    (void)devId;

    return wc_psa_hash_init_and_setup(&sha->psa_ctx, PSA_ALG_SHA_224);
}

int wc_Sha224Update(wc_Sha224 *sha, const byte *in, word32 size)
{
    return wc_psa_hash_update(&sha->psa_ctx, in, size);
}

int wc_Sha224Final(wc_Sha224 *sha, byte *out)
{
    return wc_psa_hash_finish_setup(&sha->psa_ctx, out, PSA_ALG_SHA_224);
}

int wc_Sha224GetHash(wc_Sha224 *sha, byte *out)
{
    return wc_psa_get_hash(&sha->psa_ctx, out, PSA_ALG_SHA_224);
}

int wc_Sha224Copy(wc_Sha224 *src, wc_Sha224 *dst)
{
    return wc_psa_hash_clone(&src->psa_ctx, &dst->psa_ctx);
}

void wc_Sha224Free(wc_Sha224 *sha)
{
    wc_psa_hash_abort(&sha->psa_ctx);
}

#endif /* WOLFSSL_SHA224 */

#endif /* WOLFSSL_HAVE_PSA && !WOLFSSL_PSA_NO_HASH */
