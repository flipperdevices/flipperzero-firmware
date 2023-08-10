/* kcapi_hash.c
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
 * KCAPI hash options:
 *
 * WOLFSSL_KCAPI_HASH_KEEP: Cache the hash data instead of streaming.
 *     Needed to get the current hash and continue with more data.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_KCAPI_HASH)

#define FIPS_NO_WRAPPERS

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#include <wolfssl/wolfcrypt/port/kcapi/kcapi_hash.h>

/* generic KCAPI hash free */
void KcapiHashFree(wolfssl_KCAPI_Hash* hash)
{
    if (hash != NULL) {
        if (hash->handle != NULL) {
            kcapi_md_destroy(hash->handle);
            hash->handle = NULL;
        }

    #if defined(WOLFSSL_KCAPI_HASH_KEEP)
        if (hash->msg != NULL) {
            XFREE(hash->msg, hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
            hash->msg = NULL;
        }
    #endif
    }
}


/* generic hash init for KCAPI, returns 0 on success */
static int KcapiHashInit(wolfssl_KCAPI_Hash* hash, void* heap, int devId,
        const char* type)
{
    int ret = 0;

    if (hash == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        (void)devId; /* no async for now */
        XMEMSET(hash, 0, sizeof(wolfssl_KCAPI_Hash));
        hash->heap = heap;
#if defined(WOLFSSL_KCAPI_HASH_KEEP)
        hash->len  = 0;
        hash->used = 0;
        hash->msg  = NULL;
#endif
        hash->handle = NULL;
        XSTRNCPY(hash->type, type, sizeof(hash->type)-1);
    }

    return ret;

}


/* generic hash update for KCAPI, returns 0 on success */
static int KcapiHashUpdate(wolfssl_KCAPI_Hash* hash, const byte* in, word32 sz)
{
    int ret = 0;

    if (hash == NULL || (sz > 0 && in == NULL)) {
        ret = BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_KCAPI_HASH_KEEP
    if (ret == 0) {
        /* keep full message to hash at end instead of incremental updates */
        if (hash->len < hash->used + sz) {
            if (hash->msg == NULL) {
                hash->msg = (byte*)XMALLOC(hash->used + sz, hash->heap,
                                           DYNAMIC_TYPE_TMP_BUFFER);
                if (hash->msg == NULL) {
                    ret = MEMORY_E;
                }
            }
            else {
                byte* pt = (byte*)XREALLOC(hash->msg, hash->used + sz,
                                           hash->heap, DYNAMIC_TYPE_TMP_BUFFER);
                if (pt == NULL) {
                    ret = MEMORY_E;
                }
                else {
                    hash->msg = pt;
                }
            }

            if (ret == 0) {
                hash->len = hash->used + sz;
            }
        }

        if (ret == 0) {
            XMEMCPY(hash->msg + hash->used, in, sz);
            hash->used += sz;
        }
    }
#else
    if ((ret == 0) && (hash->handle == NULL)) {
        ret = kcapi_md_init(&hash->handle, hash->type, 0);
    }
    if (ret == 0) {
        ret = kcapi_md_update(hash->handle, in, sz);
        if (ret > 0) {
            ret = 0;
        }
    }
#endif

    return ret;
}


/* generic hash final for KCAPI, return 0 on success */
static int KcapiHashFinal(wolfssl_KCAPI_Hash* hash, byte* out, word32 outSz,
        const char* type)
{
    int   ret = 0;
    int   rc;
    void* heap = NULL;

    if (hash == NULL || out == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if ((ret == 0) && (hash->handle == NULL)) {
        ret = kcapi_md_init(&hash->handle, hash->type, 0);
    }
    if (ret == 0) {
        heap = hash->heap; /* keep because KcapiHashInit clears the pointer */
    #ifdef WOLFSSL_KCAPI_HASH_KEEP
        /* keep full message to out at end instead of incremental updates */
        ret = (int)kcapi_md_update(hash->handle, hash->msg, hash->used);
        XFREE(hash->msg, heap, DYNAMIC_TYPE_TMP_BUFFER);
        hash->msg = NULL;
        if (ret == 0)
    #endif
        {
            ret = (int)kcapi_md_final(hash->handle, out, outSz);
        }

        KcapiHashFree(hash);
        rc = KcapiHashInit(hash, heap, 0, type);
        if (ret >= 0) {
            ret = rc;
        }
    }

    return ret;
}


/* generic function to get intermediate hash */
static int KcapiHashGet(wolfssl_KCAPI_Hash* hash, byte* out, word32 outSz)
{
    int ret = 0;

    if (hash == NULL || out == NULL) {
        ret = BAD_FUNC_ARG;
    }

    #ifdef WOLFSSL_KCAPI_HASH_KEEP
    if ((ret == 0) && (hash->handle == NULL)) {
        ret = kcapi_md_init(&hash->handle, hash->type, 0);
    }
    if (ret == 0) {
        ret = (int)kcapi_md_update(hash->handle, hash->msg, hash->used);
        if (ret >= 0) {
            ret = (int)kcapi_md_final(hash->handle, out, outSz);
            if (ret >= 0) {
                ret = 0;
            }
        }
        kcapi_md_destroy(hash->handle);
        hash->handle = NULL;
    }
    #else
    if (ret == 0) {
        (void)hash;
        (void)out;
        (void)outSz;

        WOLFSSL_MSG("Compile with WOLFSSL_KCAPI_HASH_KEEP for this feature");
        ret = NOT_COMPILED_IN;
    }
    #endif

    return ret;
}


/* generic struct copy for KCAPI, returns 0 on success */
static int KcapiHashCopy(wolfssl_KCAPI_Hash* src, wolfssl_KCAPI_Hash* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMCPY(dst, src, sizeof(wolfssl_KCAPI_Hash));
        dst->handle = NULL;

    #ifdef WOLFSSL_KCAPI_HASH_KEEP
        dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (dst->msg == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        XMEMCPY(dst->msg, src->msg, src->len);
    #endif

        XMEMCPY(dst->type, src->type, sizeof(dst->type));
        ret = kcapi_md_init(&dst->handle, src->type, 0);
        if (ret < 0) {
            KcapiHashFree(dst);
            ret = -1;
        }
    }

    return ret;
}


#if defined(WOLFSSL_SHA224) && defined(WOLFSSL_KCAPI_HASH) && \
                                               !defined(WOLFSSL_NO_KCAPI_SHA224)
#include <wolfssl/wolfcrypt/sha256.h>

static const char WC_NAME_SHA224[] = "sha224";


/* create KCAPI handle for SHA224 operation */
int wc_InitSha224_ex(wc_Sha224* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devId, WC_NAME_SHA224);
}


int wc_Sha224Update(wc_Sha224* sha, const byte* in, word32 sz)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashUpdate(&sha->kcapi, in, sz);
}


int wc_Sha224Final(wc_Sha224* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA224_DIGEST_SIZE,
                          WC_NAME_SHA224);
}


int wc_Sha224GetHash(wc_Sha224* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA224_DIGEST_SIZE);
}


int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}
#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256) && defined(WOLFSSL_KCAPI_HASH)
#include <wolfssl/wolfcrypt/sha256.h>

static const char WC_NAME_SHA256[] = "sha256";


int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devid)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devid, WC_NAME_SHA256);
}


int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashUpdate(&sha->kcapi, in, sz);
}


int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA256_DIGEST_SIZE,
                          WC_NAME_SHA256);
}

int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA256_DIGEST_SIZE);
}


int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}
#endif /* !NO_SHA256 */

#if defined(WOLFSSL_SHA384) && defined(WOLFSSL_KCAPI_HASH)
#include <wolfssl/wolfcrypt/sha512.h>

static const char WC_NAME_SHA384[] = "sha384";

/* create KCAPI handle for SHA384 operation */
int wc_InitSha384_ex(wc_Sha384* sha, void* heap, int devid)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devid, WC_NAME_SHA384);
}


int wc_Sha384Update(wc_Sha384* sha, const byte* in, word32 sz)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashUpdate(&sha->kcapi, in, sz);
}


int wc_Sha384Final(wc_Sha384* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA384_DIGEST_SIZE,
                          WC_NAME_SHA384);
}

int wc_Sha384GetHash(wc_Sha384* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA384_DIGEST_SIZE);
}


int wc_Sha384Copy(wc_Sha384* src, wc_Sha384* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}
#endif /* WOLFSSL_SHA384 */

#if defined(WOLFSSL_SHA512) && defined(WOLFSSL_KCAPI_HASH)
#include <wolfssl/wolfcrypt/sha512.h>

static const char WC_NAME_SHA512[] = "sha512";

/* create KCAPI handle for SHA512 operation */
int wc_InitSha512_ex(wc_Sha512* sha, void* heap, int devid)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devid, WC_NAME_SHA512);
}


int wc_Sha512Update(wc_Sha512* sha, const byte* in, word32 sz)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashUpdate(&sha->kcapi, in, sz);
}


int wc_Sha512Final(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE,
                          WC_NAME_SHA512);
}

int wc_Sha512GetHash(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE);
}


int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if !defined(WOLFSSL_NOSHA512_224)
static const char WC_NAME_SHA512_224[] = "sha512-224";

/* create KCAPI handle for SHA512 operation */
int wc_InitSha512_224_ex(wc_Sha512* sha, void* heap, int devid)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devid, WC_NAME_SHA512_224);
}


int wc_Sha512_224Final(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE,
                          WC_NAME_SHA512_224);
}
int wc_Sha512_224GetHash(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE);
}


int wc_Sha512_224Copy(wc_Sha512* src, wc_Sha512* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}
#endif /* !WOLFSSL_NOSHA512_224 */

#if !defined(WOLFSSL_NOSHA512_256)
static const char WC_NAME_SHA512_256[] = "sha512-256";

/* create KCAPI handle for SHA512 operation */
int wc_InitSha512_256_ex(wc_Sha512* sha, void* heap, int devid)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashInit(&sha->kcapi, heap, devid, WC_NAME_SHA512_256);
}


int wc_Sha512_256Final(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashFinal(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE,
                          WC_NAME_SHA512_256);
}
int wc_Sha512_256GetHash(wc_Sha512* sha, byte* hash)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashGet(&sha->kcapi, hash, WC_SHA512_DIGEST_SIZE);
}


int wc_Sha512_256Copy(wc_Sha512* src, wc_Sha512* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }
    return KcapiHashCopy(&src->kcapi, &dst->kcapi);
}
#endif /* !WOLFSSL_NOSHA512_256 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#endif /* WOLFSSL_SHA512 */

#endif /* WOLFSSL_KCAPI_HASH */
