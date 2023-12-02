/* hash.h
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

/*!
    \file wolfssl/wolfcrypt/hash.h
*/

#ifndef WOLF_CRYPT_HASH_H
#define WOLF_CRYPT_HASH_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_MD5
    #include <wolfssl/wolfcrypt/md5.h>
#endif
#ifndef NO_SHA
    #include <wolfssl/wolfcrypt/sha.h>
#endif
#if defined(WOLFSSL_SHA224) || !defined(NO_SHA256)
    #include <wolfssl/wolfcrypt/sha256.h>
#endif
#if defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512)
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef HAVE_BLAKE2
    #include <wolfssl/wolfcrypt/blake2.h>
#endif
#ifdef WOLFSSL_SHA3
    #include <wolfssl/wolfcrypt/sha3.h>
#endif
#ifndef NO_MD4
    #include <wolfssl/wolfcrypt/md4.h>
#endif
#ifdef WOLFSSL_MD2
    #include <wolfssl/wolfcrypt/md2.h>
#endif
#if defined(HAVE_BLAKE2) || defined(HAVE_BLAKE2S)
    #include <wolfssl/wolfcrypt/blake2.h>
#endif
#ifdef WOLFSSL_SM3
    #include <wolfssl/wolfcrypt/sm3.h>
#endif


#ifdef __cplusplus
    extern "C" {
#endif

#if !defined(HAVE_FIPS) && !defined(NO_OLD_WC_NAMES)
    #define MAX_DIGEST_SIZE WC_MAX_DIGEST_SIZE
#endif


/* Supported Message Authentication Codes from page 43 */
enum wc_MACAlgorithm {
    no_mac,
    md5_mac,
    sha_mac,
    sha224_mac,
    sha256_mac,     /* needs to match external KDF_MacAlgorithm */
    sha384_mac,
    sha512_mac,
    rmd_mac,
    blake2b_mac,
    sm3_mac,
};

enum wc_HashFlags {
    WC_HASH_FLAG_NONE =     0x00000000,
    WC_HASH_FLAG_WILLCOPY = 0x00000001, /* flag to indicate hash will be copied */
    WC_HASH_FLAG_ISCOPY =   0x00000002, /* hash is copy */
#ifdef WOLFSSL_SHA3
    WC_HASH_SHA3_KECCAK256 =0x00010000, /* Older KECCAK256 */
#endif
    WOLF_ENUM_DUMMY_LAST_ELEMENT(WC_HASH)
};

#ifndef NO_HASH_WRAPPER
typedef union {
    #ifndef NO_MD5
        wc_Md5 md5;
    #endif
    #ifndef NO_SHA
        wc_Sha sha;
    #endif
    #ifdef WOLFSSL_SHA224
        wc_Sha224 sha224;
    #endif
    #ifndef NO_SHA256
        wc_Sha256 sha256;
    #endif
    #ifdef WOLFSSL_SHA384
        wc_Sha384 sha384;
    #endif
    #ifdef WOLFSSL_SHA512
        wc_Sha512 sha512;
    #endif
    #ifdef WOLFSSL_SHA3
        wc_Sha3 sha3;
    #endif
    #ifdef WOLFSSL_SM3
        wc_Sm3 sm3;
    #endif
} wc_HashAlg;
#endif /* !NO_HASH_WRAPPER */

/* Find largest possible digest size
   Note if this gets up to the size of 80 or over check smallstack build */
#if defined(WOLFSSL_SHA3)
    #define WC_MAX_DIGEST_SIZE WC_SHA3_512_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA3_224_BLOCK_SIZE /* 224 is the largest block size */
#elif defined(WOLFSSL_SHA512)
    #define WC_MAX_DIGEST_SIZE WC_SHA512_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA512_BLOCK_SIZE
#elif defined(HAVE_BLAKE2)
    #define WC_MAX_DIGEST_SIZE BLAKE2B_OUTBYTES
    #define WC_MAX_BLOCK_SIZE  BLAKE2B_BLOCKBYTES
#elif defined(WOLFSSL_SHA384)
    #define WC_MAX_DIGEST_SIZE WC_SHA384_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA384_BLOCK_SIZE
#elif !defined(NO_SHA256)
    #define WC_MAX_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA256_BLOCK_SIZE
#elif defined(WOLFSSL_SM3)
    #define WC_MAX_DIGEST_SIZE WC_SM3_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SM3_BLOCK_SIZE
#elif defined(WOLFSSL_SHA224)
    #define WC_MAX_DIGEST_SIZE WC_SHA224_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA224_BLOCK_SIZE
#elif !defined(NO_SHA)
    #define WC_MAX_DIGEST_SIZE WC_SHA_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_SHA_BLOCK_SIZE
#elif !defined(NO_MD5)
    #define WC_MAX_DIGEST_SIZE WC_MD5_DIGEST_SIZE
    #define WC_MAX_BLOCK_SIZE  WC_MD5_BLOCK_SIZE
#else
    #define WC_MAX_DIGEST_SIZE 64 /* default to max size of 64 */
    #define WC_MAX_BLOCK_SIZE  128
#endif

#if !defined(NO_ASN) || !defined(NO_DH) || defined(HAVE_ECC)
WOLFSSL_API int wc_HashGetOID(enum wc_HashType hash_type);
WOLFSSL_API enum wc_HashType wc_OidGetHash(int oid);
#endif

WOLFSSL_API enum wc_HashType wc_HashTypeConvert(int hashType);

#ifndef NO_HASH_WRAPPER

WOLFSSL_API int wc_HashGetDigestSize(enum wc_HashType hash_type);
WOLFSSL_API int wc_HashGetBlockSize(enum wc_HashType hash_type);
WOLFSSL_API int wc_Hash(enum wc_HashType hash_type,
    const byte* data, word32 data_len,
    byte* hash, word32 hash_len);

/* generic hash operation wrappers */
WOLFSSL_API int wc_HashInit_ex(wc_HashAlg* hash, enum wc_HashType type,
    void* heap, int devId);
WOLFSSL_API int wc_HashInit(wc_HashAlg* hash, enum wc_HashType type);
WOLFSSL_API int wc_HashUpdate(wc_HashAlg* hash, enum wc_HashType type,
    const byte* data, word32 dataSz);
WOLFSSL_API int wc_HashFinal(wc_HashAlg* hash, enum wc_HashType type,
    byte* out);
WOLFSSL_API int wc_HashFree(wc_HashAlg* hash, enum wc_HashType type);

#ifdef WOLFSSL_HASH_FLAGS
    WOLFSSL_API int wc_HashSetFlags(wc_HashAlg* hash, enum wc_HashType type,
        word32 flags);
    WOLFSSL_API int wc_HashGetFlags(wc_HashAlg* hash, enum wc_HashType type,
        word32* flags);
#endif

#ifndef NO_MD5
#include <wolfssl/wolfcrypt/md5.h>
WOLFSSL_API int wc_Md5Hash(const byte* data, word32 len, byte* hash);
#endif

#ifndef NO_SHA
#include <wolfssl/wolfcrypt/sha.h>
WOLFSSL_API int wc_ShaHash(const byte* data, word32 len, byte* hash);
#endif

#ifdef WOLFSSL_SHA224
#include <wolfssl/wolfcrypt/sha256.h>
WOLFSSL_API int wc_Sha224Hash(const byte* data, word32 len, byte* hash);
#endif /* defined(WOLFSSL_SHA224) */

#ifndef NO_SHA256
#include <wolfssl/wolfcrypt/sha256.h>
WOLFSSL_API int wc_Sha256Hash(const byte* data, word32 len, byte* hash);
#endif

#ifdef WOLFSSL_SHA384
#include <wolfssl/wolfcrypt/sha512.h>
WOLFSSL_API int wc_Sha384Hash(const byte* data, word32 len, byte* hash);
#endif /* defined(WOLFSSL_SHA384) */

#ifdef WOLFSSL_SHA512
#include <wolfssl/wolfcrypt/sha512.h>
WOLFSSL_API int wc_Sha512Hash(const byte* data, word32 len, byte* hash);
WOLFSSL_API int wc_Sha512_224Hash(const byte* data, word32 len, byte* hash);
WOLFSSL_API int wc_Sha512_256Hash(const byte* data, word32 len, byte* hash);
#endif /* WOLFSSL_SHA512 */

#ifdef WOLFSSL_SHA3
#include <wolfssl/wolfcrypt/sha3.h>
WOLFSSL_API int wc_Sha3_224Hash(const byte* data, word32 len, byte* hash);
WOLFSSL_API int wc_Sha3_256Hash(const byte* data, word32 len, byte* hash);
WOLFSSL_API int wc_Sha3_384Hash(const byte* data, word32 len, byte* hash);
WOLFSSL_API int wc_Sha3_512Hash(const byte* data, word32 len, byte* hash);
#ifdef WOLFSSL_SHAKE128
WOLFSSL_API int wc_Shake128Hash(const byte* data, word32 len, byte* hash,
    word32 hashLen);
#endif
#ifdef WOLFSSL_SHAKE256
WOLFSSL_API int wc_Shake256Hash(const byte* data, word32 len, byte* hash,
    word32 hashLen);
#endif
#endif /* WOLFSSL_SHA3 */

#ifdef WOLFSSL_SM3
WOLFSSL_API int wc_Sm3Hash(const byte* data, word32 len, byte* hash);
#endif

#endif /* !NO_HASH_WRAPPER */

#if defined(WOLFSSL_HASH_KEEP)
WOLFSSL_LOCAL int _wc_Hash_Grow(byte** msg, word32* used, word32* len,
        const byte* in, int inSz, void* heap);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_HASH_H */
