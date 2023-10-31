/* afalg_hash.c
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

#if defined(WOLFSSL_AFALG_HASH) || (defined(WOLFSSL_AFALG_XILINX_SHA3) \
        && defined(WOLFSSL_SHA3))

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>
#include <wolfssl/wolfcrypt/port/af_alg/afalg_hash.h>

static const char WC_TYPE_HASH[] = "hash";


/* generic AF_ALG hash free */
static void AfalgHashFree(wolfssl_AFALG_Hash* hash)
{
    if (hash == NULL)
        return;

    if (hash->alFd > 0) {
        (void)close(hash->alFd);
        hash->alFd = -1; /* avoid possible double close on socket */
    }
    if (hash->rdFd > 0) {
        (void)close(hash->rdFd);
        hash->rdFd = -1; /* avoid possible double close on socket */
    }

    #if defined(WOLFSSL_AFALG_HASH_KEEP)
    if (hash->msg != NULL) {
        XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
        hash->msg = NULL;
    }
    #endif
}


/* generic hash init for AF_ALG, returns 0 on success */
static int AfalgHashInit(wolfssl_AFALG_Hash* hash, void* heap, int devId,
        const char* type)
{
    if (hash == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId; /* no async for now */
    XMEMSET(hash, 0, sizeof(wolfssl_AFALG_Hash));
    hash->heap = heap;

    hash->len  = 0;
    hash->used = 0;
    hash->msg  = NULL;
    hash->alFd = -1;
    hash->rdFd = -1;

    hash->alFd = wc_Afalg_Socket();
    if (hash->alFd < 0) {
        return WC_AFALG_SOCK_E;
    }

    hash->rdFd = wc_Afalg_CreateRead(hash->alFd, WC_TYPE_HASH, type);
    if (hash->rdFd < 0) {
        (void)close(hash->alFd);
        return WC_AFALG_SOCK_E;
    }

    return 0;

}


/* generic hash update for AF_ALG, returns 0 on success */
static int AfalgHashUpdate(wolfssl_AFALG_Hash* hash, const byte* in, word32 sz)
{
    if (hash == NULL || (sz > 0 && in == NULL)) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    /* keep full message to hash at end instead of incremental updates */
    if (hash->len < hash->used + sz) {
        if (hash->msg == NULL) {
            hash->msg = (byte*)XMALLOC(hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
        } else {
            byte* pt = (byte*)XREALLOC(hash->msg, hash->used + sz, hash->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
            }
            hash->msg = pt;
        }
        if (hash->msg == NULL) {
            return MEMORY_E;
        }
        hash->len = hash->used + sz;
    }
    if (sz > 0) {
        XMEMCPY(hash->msg + hash->used, in, sz);
        hash->used += sz;
    }
#else
    int ret;

    if ((ret = (int)send(hash->rdFd, in, sz, MSG_MORE)) < 0) {
        return ret;
    }
#endif
    return 0;
}


/* generic hash final for AF_ALG, return 0 on success */
static int AfalgHashFinal(wolfssl_AFALG_Hash* hash, byte* out, word32 outSz,
        const char* type)
{
    int   ret;

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    /* keep full message to out at end instead of incremental updates */
    if ((ret = (int)send(hash->rdFd, hash->msg, hash->used, 0)) < 0) {
        ret = WC_AFALG_SOCK_E;
        goto out;
    }
    XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
    hash->msg = NULL;
#else
    if ((ret = (int)send(hash->rdFd, NULL, 0, 0)) < 0) {
        ret = WC_AFALG_SOCK_E;
        goto out;
    }
#endif

    if ((ret = (int)read(hash->rdFd, out, outSz)) != (int)outSz) {
        ret = WC_AFALG_SOCK_E;
        goto out;
    }

    ret = 0;

out:

    AfalgHashFree(hash);

    if (ret != 0)
        return ret;
    else
        return AfalgHashInit(hash, hash->heap, 0, type);
}


/* generic function to get intermediate hash */
static int AfalgHashGet(wolfssl_AFALG_Hash* hash, byte* out, word32 outSz)
{
    int ret;

    if (hash == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)ret;
#ifdef WOLFSSL_AFALG_HASH_KEEP
    if ((ret = (int)send(hash->rdFd, hash->msg, hash->used, 0)) < 0) {
        return ret;
    }

    if ((ret = (int)read(hash->rdFd, out, outSz)) != (int)outSz) {
        return ret;
    }
    return 0;
#else
    (void)hash;
    (void)out;
    (void)outSz;

    WOLFSSL_MSG("Compile with WOLFSSL_AFALG_HASH_KEEP for this feature");
    return NOT_COMPILED_IN;
#endif
}


/* generic struct copy for AF_ALG, returns 0 on success */
static int AfalgHashCopy(wolfssl_AFALG_Hash* src, wolfssl_AFALG_Hash* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(dst, src, sizeof(wolfssl_AFALG_Hash));

#ifdef WOLFSSL_AFALG_HASH_KEEP
    dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (dst->msg == NULL) {
        return MEMORY_E;
    }
    if (src->len > 0)
        XMEMCPY(dst->msg, src->msg, src->len);
#endif

    dst->rdFd = accept(src->rdFd, NULL, 0);
    dst->alFd = accept(src->alFd, NULL, 0);

    if (dst->rdFd == -1 || dst->alFd == -1) {
        AfalgHashFree(dst);
        return -1;
    }

    return 0;
}


#if !defined(NO_SHA256) && defined(WOLFSSL_AFALG_HASH)
#include <wolfssl/wolfcrypt/sha256.h>

static const char WC_NAME_SHA256[] = "sha256";


/* create AF_ALG sockets for SHA256 operation */
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    return AfalgHashInit(sha, heap, devId, WC_NAME_SHA256);
}


int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    return AfalgHashUpdate(sha, in, sz);
}


int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    return AfalgHashFinal(sha, hash, WC_SHA256_DIGEST_SIZE, WC_NAME_SHA256);
}


int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    return AfalgHashGet(sha, hash, WC_SHA256_DIGEST_SIZE);
}


int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    return AfalgHashCopy(src, dst);
}
#endif /* !NO_SHA256 */



#if defined(WOLFSSL_SHA3) && defined(WOLFSSL_AFALG_XILINX_SHA3)
#include <wolfssl/wolfcrypt/sha3.h>

static const char WC_NAME_SHA3[] = "xilinx-keccak-384";

void wc_Sha3_384_Free(wc_Sha3* sha)
{
    AfalgHashFree(sha);
}


/* create AF_ALG sockets for SHA256 operation */
int wc_InitSha3_384(wc_Sha3* sha, void* heap, int devId)
{
    return AfalgHashInit(sha, heap, devId, WC_NAME_SHA3);
}


int wc_Sha3_384_Update(wc_Sha3* sha, const byte* in, word32 sz)
{
#ifndef WOLFSSL_AFALG_HASH_KEEP
    if (sz % 4) {
        WOLFSSL_MSG("Alignment issue. Message size needs to be divisible by 4")
        return BAD_FUNC_ARG;
    }
#endif

    return AfalgHashUpdate(sha, in, sz);
}


int wc_Sha3_384_Final(wc_Sha3* sha, byte* hash)
{
    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    if (sha->used % 4) {
        WOLFSSL_MSG("Alignment issue. Message size needs to be divisible by 4");
        return BAD_FUNC_ARG;
    }
#endif

    return AfalgHashFinal(sha, hash, WC_SHA3_384_DIGEST_SIZE, WC_NAME_SHA3);
}


int wc_Sha3_384_GetHash(wc_Sha3* sha, byte* hash)
{
    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    if (sha->used % 4) {
        WOLFSSL_MSG("Alignment issue. Message size needs to be divisible by 4");
        return BAD_FUNC_ARG;
    }
#endif

    return AfalgHashGet(sha, hash, WC_SHA3_384_DIGEST_SIZE);
}

int wc_Sha3_384_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    return AfalgHashCopy(src, dst);
}
#endif /* WOLFSSL_SHA3 && WOLFSSL_AFALG_XILINX_SHA3 */

#endif /* WOLFSSL_AFALG */
