/* port/ti/ti-hash.c
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

#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_TI_HASH)

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/port/ti/ti-hash.h>
#include <wolfssl/wolfcrypt/port/ti/ti-ccm.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/hash.h>

#ifndef TI_DUMMY_BUILD
#include "inc/hw_memmap.h"
#include "inc/hw_shamd5.h"
#include "inc/hw_ints.h"
#include "driverlib/shamd5.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#else
#define SHAMD5_ALGO_MD5 1
#define SHAMD5_ALGO_SHA1 2
#define SHAMD5_ALGO_SHA256 3
#define SHAMD5_ALGO_SHA224 4
#endif

static int hashInit(wolfssl_TI_Hash *hash) {
    if (!wolfSSL_TI_CCMInit())return 1;
    hash->used = 0;
    hash->msg  = 0;
    hash->len  = 0;
    return 0;
}

static int hashUpdate(wolfssl_TI_Hash *hash, const byte* data, word32 len)
{
    void *p;

    if ((hash== NULL) || (data == NULL))return BAD_FUNC_ARG;

    if (hash->len < hash->used+len) {
        if (hash->msg == NULL) {
            p = XMALLOC(hash->used+len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        } else {
            p = XREALLOC(hash->msg, hash->used+len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
        if (p == 0)return 1;
        hash->msg = p;
        hash->len = hash->used+len;
    }
    XMEMCPY(hash->msg+hash->used, data, len);
    hash->used += len;
    return 0;
}

static int hashGetHash(wolfssl_TI_Hash *hash, byte* result, word32 algo, word32 hsize)
{
    uint32_t h[16];
#ifndef TI_DUMMY_BUILD
    wolfSSL_TI_lockCCM();
    ROM_SHAMD5Reset(SHAMD5_BASE);
    ROM_SHAMD5ConfigSet(SHAMD5_BASE, algo);
    ROM_SHAMD5DataProcess(SHAMD5_BASE,
                   (uint32_t *)hash->msg, hash->used, h);
    wolfSSL_TI_unlockCCM();
#else
    (void) hash;
    (void) algo;

    XMEMSET(h, 0, sizeof(h));
#endif
    if (result == NULL) {
        return BAD_FUNC_ARG;
    }
    XMEMCPY(result, h, hsize);

    return 0;
}

static int hashCopy(wolfssl_TI_Hash *src, wolfssl_TI_Hash *dst) {
    XMEMCPY(dst, src, sizeof(wolfssl_TI_Hash));
    return 0;
}

static int hashFinal(wolfssl_TI_Hash *hash, byte* result, word32 algo, word32 hsize)
{
    hashGetHash(hash, result, algo, hsize);
    XFREE(hash->msg, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    hashInit(hash);
    return 0;
}

static int hashHash(const byte* data, word32 len, byte* hash, word32 algo, word32 hsize)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    wolfssl_TI_Hash* hash_desc;
#else
    wolfssl_TI_Hash  hash_desc[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
    hash_desc = (wolfssl_TI_Hash*)XMALLOC(sizeof(wolfssl_TI_Hash), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_desc == NULL)
        return MEMORY_E;
#endif

    if ((ret = hashInit(hash_desc)) != 0) {
        WOLFSSL_MSG("Hash Init failed");
    }
    else {
        hashUpdate(hash_desc, data, len);
        hashFinal(hash_desc, hash, algo, hsize);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash_desc, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

static int hashFree(wolfssl_TI_Hash *hash)
{
    XFREE(hash->msg, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    hashInit(hash);
    return 0;
}

#if !defined(NO_MD5)
WOLFSSL_API int wc_InitMd5_ex(Md5* md5, void* heap, int devId)
{
    if (md5 == NULL)
        return 1;
    (void)heap;
    (void)devId;
    return hashInit((wolfssl_TI_Hash *)md5);
}
WOLFSSL_API int wc_InitMd5(Md5* md5)
{
    return wc_InitMd5_ex(md5, NULL, INVALID_DEVID);
}

WOLFSSL_API int wc_Md5Update(Md5* md5, const byte* data, word32 len)
{
    return hashUpdate((wolfssl_TI_Hash *)md5, data, len);
}

WOLFSSL_API int wc_Md5Final(Md5* md5, byte* hash)
{
    return hashFinal((wolfssl_TI_Hash *)md5, hash, SHAMD5_ALGO_MD5, MD5_DIGEST_SIZE);
}

WOLFSSL_API int wc_Md5GetHash(Md5* md5, byte* hash)
{
    return hashGetHash((wolfssl_TI_Hash *)md5, hash, SHAMD5_ALGO_MD5, MD5_DIGEST_SIZE);
}

WOLFSSL_API int wc_Md5Copy(Md5* src, Md5* dst) {
	return hashCopy((wolfssl_TI_Hash *)src, (wolfssl_TI_Hash *)dst);
}

WOLFSSL_API int wc_Md5Hash(const byte*data, word32 len, byte*hash)
{
    return hashHash(data, len, hash, SHAMD5_ALGO_MD5, MD5_DIGEST_SIZE);
}

WOLFSSL_API void wc_Md5Free(Md5* md5)
{
    hashFree((wolfssl_TI_Hash *)md5);
}

#endif /* !NO_MD5 */

#if !defined(NO_SHA)
WOLFSSL_API int wc_InitSha_ex(Md5* sha, void* heap, int devId)
{
    if (sha == NULL)
        return 1;
    (void)heap;
    (void)devId;
    return hashInit((wolfssl_TI_Hash *)sha);
}
WOLFSSL_API int wc_InitSha(Sha* sha)
{
    return wc_InitSha_ex(sha, NULL, INVALID_DEVID);
}

WOLFSSL_API int wc_ShaUpdate(Sha* sha, const byte* data, word32 len)
{
    return hashUpdate((wolfssl_TI_Hash *)sha, data, len);
}

WOLFSSL_API int wc_ShaFinal(Sha* sha, byte* hash)
{
    return hashFinal((wolfssl_TI_Hash *)sha, hash, SHAMD5_ALGO_SHA1, SHA_DIGEST_SIZE);
}

WOLFSSL_API int wc_ShaGetHash(Sha* sha, byte* hash)
{
    return hashGetHash(sha, hash, SHAMD5_ALGO_SHA1, SHA_DIGEST_SIZE);
}

WOLFSSL_API int wc_ShaCopy(Sha* src, Sha* dst) {
	return hashCopy((wolfssl_TI_Hash *)src, (wolfssl_TI_Hash *)dst);
}

WOLFSSL_API int wc_ShaHash(const byte*data, word32 len, byte*hash)
{
    return hashHash(data, len, hash, SHAMD5_ALGO_SHA1, SHA_DIGEST_SIZE);
}

WOLFSSL_API void wc_ShaFree(Sha* sha)
{
    hashFree((wolfssl_TI_Hash *)sha);
}

#endif /* !NO_SHA */

#if defined(WOLFSSL_SHA224)
WOLFSSL_API int wc_InitSha224_ex(Sha224* sha224, void* heap, int devId)
{
    if (sha224 == NULL)
        return 1;
    (void)heap;
    (void)devId;
    return hashInit((wolfssl_TI_Hash *)sha224);
}
WOLFSSL_API int wc_InitSha224(Sha224* sha224)
{
    return wc_InitSha224_ex(sha224, NULL, INVALID_DEVID);
}

WOLFSSL_API int wc_Sha224Update(Sha224* sha224, const byte* data, word32 len)
{
    return hashUpdate((wolfssl_TI_Hash *)sha224, data, len);
}

WOLFSSL_API int wc_Sha224Final(Sha224* sha224, byte* hash)
{
    return hashFinal((wolfssl_TI_Hash *)sha224, hash, SHAMD5_ALGO_SHA224, SHA224_DIGEST_SIZE);
}

WOLFSSL_API int wc_Sha224GetHash(Sha224* sha224, byte* hash)
{
    return hashGetHash(sha224, hash, SHAMD5_ALGO_SHA224, SHA224_DIGEST_SIZE);
}

WOLFSSL_API int wc_Sha224Hash(const byte* data, word32 len, byte*hash)
{
    return hashHash(data, len, hash, SHAMD5_ALGO_SHA224, SHA224_DIGEST_SIZE);
}

WOLFSSL_API void wc_Sha224Free(Sha224* sha224)
{
    hashFree((wolfssl_TI_Hash *)sha224);
}

#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256)
WOLFSSL_API int wc_InitSha256_ex(Sha256* sha256, void* heap, int devId)
{
    if (sha256 == NULL)
        return 1;
    (void)heap;
    (void)devId;
    return hashInit((wolfssl_TI_Hash *)sha256);
}

WOLFSSL_API int wc_InitSha256(Sha256* sha256)
{
    return wc_InitSha256_ex(sha256, NULL, INVALID_DEVID);
}

WOLFSSL_API int wc_Sha256Update(Sha256* sha256, const byte* data, word32 len)
{
    return hashUpdate((wolfssl_TI_Hash *)sha256, data, len);
}

WOLFSSL_API int wc_Sha256Final(Sha256* sha256, byte* hash)
{
    return hashFinal((wolfssl_TI_Hash *)sha256, hash, SHAMD5_ALGO_SHA256, SHA256_DIGEST_SIZE);
}

WOLFSSL_API int wc_Sha256GetHash(Sha256* sha256, byte* hash)
{
    return hashGetHash(sha256, hash, SHAMD5_ALGO_SHA256, SHA256_DIGEST_SIZE);
}

WOLFSSL_API int wc_Sha256Hash(const byte* data, word32 len, byte*hash)
{
    return hashHash(data, len, hash, SHAMD5_ALGO_SHA256, SHA256_DIGEST_SIZE);
}

WOLFSSL_API void wc_Sha256Free(Sha256* sha256)
{
    hashFree((wolfssl_TI_Hash *)sha256);
}

#endif /* !NO_SHA256 */

#endif
