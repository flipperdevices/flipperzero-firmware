/* hash.c
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
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifndef NO_ASN
#include <wolfssl/wolfcrypt/asn.h>
#endif

#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/cryptocb.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#ifdef NO_ASN
enum Hash_Sum  {
    MD2h      = 646,
    MD5h      = 649,
    SHAh      =  88,
    SHA224h   = 417,
    SHA256h   = 414,
    SHA384h   = 415,
    SHA512h   = 416,
    SHA512_224h = 418,
    SHA512_256h = 419,
    SHA3_224h = 420,
    SHA3_256h = 421,
    SHA3_384h = 422,
    SHA3_512h = 423,
    SHAKE128h = 424,
    SHAKE256h = 425,
    SM3h      = 640     /* 0x2A,0x81,0x1C,0xCF,0x55,0x01,0x83,0x11 */
};
#endif /* !NO_ASN */

#if !defined(NO_PWDBASED) || !defined(NO_ASN)
/* function converts int hash type to enum */
enum wc_HashType wc_HashTypeConvert(int hashType)
{
    /* Default to hash type none as error */
    enum wc_HashType eHashType = WC_HASH_TYPE_NONE;
#if defined(HAVE_FIPS) || defined(HAVE_SELFTEST)
    /* original FIPSv1  and CAVP selftest require a mapping for unique hash
       type to wc_HashType */
    switch (hashType) {
    #ifndef NO_MD5
        case WC_MD5:
            eHashType = WC_HASH_TYPE_MD5;
            break;
    #endif /* !NO_MD5 */
    #ifndef NO_SHA
        case WC_SHA:
            eHashType = WC_HASH_TYPE_SHA;
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            eHashType = WC_HASH_TYPE_SHA224;
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case WC_SHA256:
            eHashType = WC_HASH_TYPE_SHA256;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            eHashType = WC_HASH_TYPE_SHA384;
            break;
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            eHashType = WC_HASH_TYPE_SHA512;
            break;

    #endif /* WOLFSSL_SHA512 */
    #ifdef WOLFSSL_SHA3
        case WC_SHA3_224:
            eHashType = WC_HASH_TYPE_SHA3_224;
            break;
        case WC_SHA3_256:
            eHashType = WC_HASH_TYPE_SHA3_256;
            break;
        case WC_SHA3_384:
            eHashType = WC_HASH_TYPE_SHA3_384;
            break;
        case WC_SHA3_512:
            eHashType = WC_HASH_TYPE_SHA3_512;
            break;
    #endif /* WOLFSSL_SHA3 */
    #ifdef WOLFSSL_SM3
        case WC_SM3:
            eHashType = WC_HASH_TYPE_SM3;
            break;
    #endif
        default:
            eHashType = WC_HASH_TYPE_NONE;
            break;
    }
#else
    /* current master uses same unique types as wc_HashType */
    if (hashType > 0 && hashType <= WC_HASH_TYPE_MAX) {
        eHashType = (enum wc_HashType)hashType;
    }
#endif
    return eHashType;
}
#endif /* !NO_PWDBASED || !NO_ASN */

#if !defined(NO_ASN) || !defined(NO_DH) || defined(HAVE_ECC)

int wc_HashGetOID(enum wc_HashType hash_type)
{
    int oid = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD2:
        #ifdef WOLFSSL_MD2
            oid = MD2h;
        #endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            oid = MD5h;
        #endif
            break;
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            oid = SHAh;
        #endif
            break;
        case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
            oid = SHA224h;
        #endif
            break;
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            oid = SHA256h;
        #endif
            break;
        case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
            oid = SHA384h;
        #endif
            break;
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            oid = SHA512h;
        #endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            oid = SHA512_224h;
        #endif
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            oid = SHA512_256h;
        #endif
            break;
    #endif
        case WC_HASH_TYPE_SHA3_224:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            oid = SHA3_224h;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_256:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            oid = SHA3_256h;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_384:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            oid = SHA3_384h;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_512:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            oid = SHA3_512h;
        #endif
            break;
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
            oid = SHAKE128h;
            break;
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
            oid = SHAKE256h;
            break;
    #endif
    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            oid = SM3h;
            break;
    #endif

        /* Not Supported */
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
        case WC_HASH_TYPE_NONE:
        default:
            oid = BAD_FUNC_ARG;
            break;
    }
    return oid;
}

enum wc_HashType wc_OidGetHash(int oid)
{
    enum wc_HashType hash_type = WC_HASH_TYPE_NONE;
    switch (oid)
    {
    #ifdef WOLFSSL_MD2
        case MD2h:
            hash_type = WC_HASH_TYPE_MD2;
            break;
    #endif
        case MD5h:
        #ifndef NO_MD5
            hash_type = WC_HASH_TYPE_MD5;
        #endif
            break;
        case SHAh:
        #ifndef NO_SHA
            hash_type = WC_HASH_TYPE_SHA;
        #endif
            break;
        case SHA224h:
        #ifdef WOLFSSL_SHA224
            hash_type = WC_HASH_TYPE_SHA224;
        #endif
            break;
        case SHA256h:
        #ifndef NO_SHA256
            hash_type = WC_HASH_TYPE_SHA256;
        #endif
            break;
        case SHA384h:
        #ifdef WOLFSSL_SHA384
            hash_type = WC_HASH_TYPE_SHA384;
        #endif
            break;
        case SHA512h:
        #ifdef WOLFSSL_SHA512
            hash_type = WC_HASH_TYPE_SHA512;
        #endif
            break;
    #ifdef WOLFSSL_SHA3
        case SHA3_224h:
            hash_type = WC_HASH_TYPE_SHA3_224;
            break;
        case SHA3_256h:
            hash_type = WC_HASH_TYPE_SHA3_256;
            break;
        case SHA3_384h:
            hash_type = WC_HASH_TYPE_SHA3_384;
            break;
        case SHA3_512h:
            hash_type = WC_HASH_TYPE_SHA3_512;
            break;
    #endif /* WOLFSSL_SHA3 */
    #ifdef WOLFSSL_SM3
        case SM3h:
            hash_type = WC_HASH_TYPE_SM3;
            break;
    #endif
        default:
            break;
    }
    return hash_type;
}
#endif /* !NO_ASN || !NO_DH || HAVE_ECC */

#ifndef NO_HASH_WRAPPER

/* Get Hash digest size */
int wc_HashGetDigestSize(enum wc_HashType hash_type)
{
    int dig_size = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD2:
        #ifdef WOLFSSL_MD2
            dig_size = MD2_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_MD4:
        #ifndef NO_MD4
            dig_size = MD4_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            dig_size = WC_MD5_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            dig_size = WC_SHA_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
            dig_size = WC_SHA224_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            dig_size = WC_SHA256_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
            dig_size = WC_SHA384_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            dig_size = WC_SHA512_DIGEST_SIZE;
        #endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            dig_size = WC_SHA512_224_DIGEST_SIZE;
        #endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            dig_size = WC_SHA512_256_DIGEST_SIZE;
        #endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_MD5_SHA: /* Old TLS Specific */
        #if !defined(NO_MD5) && !defined(NO_SHA)
            dig_size = (int)WC_MD5_DIGEST_SIZE + (int)WC_SHA_DIGEST_SIZE;
        #endif
            break;

        case WC_HASH_TYPE_SHA3_224:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            dig_size = WC_SHA3_224_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_256:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            dig_size = WC_SHA3_256_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_384:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            dig_size = WC_SHA3_384_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_512:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            dig_size = WC_SHA3_512_DIGEST_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
        #if defined(HAVE_BLAKE2) || defined(HAVE_BLAKE2S)
            dig_size = BLAKE2S_OUTBYTES;
        #endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            dig_size = WC_SM3_DIGEST_SIZE;
            break;
    #endif

        /* Not Supported */
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            dig_size = BAD_FUNC_ARG;
            break;
    }
    return dig_size;
}


/* Get Hash block size */
int wc_HashGetBlockSize(enum wc_HashType hash_type)
{
    int block_size = HASH_TYPE_E; /* Default to hash type error */
    switch (hash_type)
    {
        case WC_HASH_TYPE_MD2:
        #ifdef WOLFSSL_MD2
            block_size = MD2_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_MD4:
        #ifndef NO_MD4
            block_size = MD4_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            block_size = WC_MD5_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            block_size = WC_SHA_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
            block_size = WC_SHA224_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            block_size = WC_SHA256_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
            block_size = WC_SHA384_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            block_size = WC_SHA512_BLOCK_SIZE;
        #endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            block_size = WC_SHA512_224_BLOCK_SIZE;
        #endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        #if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            block_size = WC_SHA512_256_BLOCK_SIZE;
        #endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_MD5_SHA: /* Old TLS Specific */
        #if !defined(NO_MD5) && !defined(NO_SHA)
            block_size = (int)WC_MD5_BLOCK_SIZE + (int)WC_SHA_BLOCK_SIZE;
        #endif
            break;

        case WC_HASH_TYPE_SHA3_224:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            block_size = WC_SHA3_224_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_256:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            block_size = WC_SHA3_256_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_384:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            block_size = WC_SHA3_384_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_SHA3_512:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            block_size = WC_SHA3_512_BLOCK_SIZE;
        #endif
            break;
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
        #if defined(HAVE_BLAKE2) || defined(HAVE_BLAKE2S)
            block_size = BLAKE2S_BLOCKBYTES;
        #endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            block_size = WC_SM3_BLOCK_SIZE;
            break;
    #endif

        /* Not Supported */
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            block_size = BAD_FUNC_ARG;
            break;
    }
    return block_size;
}

/* Generic Hashing Wrapper */
int wc_Hash(enum wc_HashType hash_type, const byte* data,
    word32 data_len, byte* hash, word32 hash_len)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */
    int dig_size;

    /* Validate hash buffer size */
    dig_size = wc_HashGetDigestSize(hash_type);
    if (dig_size < 0) {
        return dig_size;
    }

    if (hash_len < (word32)dig_size) {
        return BUFFER_E;
    }

    /* Suppress possible unused arg if all hashing is disabled */
    (void)data;
    (void)data_len;
    (void)hash;
    (void)hash_len;

    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaHash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Hash(data, data_len, hash);
#endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            ret = wc_Sha512_224Hash(data, data_len, hash);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            ret = wc_Sha512_256Hash(data, data_len, hash);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_MD5_SHA:
#if !defined(NO_MD5) && !defined(NO_SHA)
            ret = wc_Md5Hash(data, data_len, hash);
            if (ret == 0) {
                ret = wc_ShaHash(data, data_len, &hash[WC_MD5_DIGEST_SIZE]);
            }
#endif
            break;

        case WC_HASH_TYPE_SHA3_224:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            ret = wc_Sha3_224Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA3_256:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            ret = wc_Sha3_256Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA3_384:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            ret = wc_Sha3_384Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA3_512:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            ret = wc_Sha3_512Hash(data, data_len, hash);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_Sm3Hash(data, data_len, hash);
            break;
    #endif

        /* Not Supported */
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }
    return ret;
}

int wc_HashInit_ex(wc_HashAlg* hash, enum wc_HashType type, void* heap,
    int devId)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_InitMd5_ex(&hash->md5, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_InitSha_ex(&hash->sha, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_InitSha224_ex(&hash->sha224, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_InitSha256_ex(&hash->sha256, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_InitSha384_ex(&hash->sha384, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_InitSha512_ex(&hash->sha512, heap, devId);
#endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            ret = wc_InitSha512_224_ex(&hash->sha512, heap, devId);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            ret = wc_InitSha512_256_ex(&hash->sha512, heap, devId);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_SHA3_224:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            ret = wc_InitSha3_224(&hash->sha3, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA3_256:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            ret = wc_InitSha3_256(&hash->sha3, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA3_384:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            ret = wc_InitSha3_384(&hash->sha3, heap, devId);
#endif
            break;
        case WC_HASH_TYPE_SHA3_512:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            ret = wc_InitSha3_512(&hash->sha3, heap, devId);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_InitSm3(&hash->sm3, heap, devId);
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    (void)heap;
    (void)devId;

    return ret;
}

int wc_HashInit(wc_HashAlg* hash, enum wc_HashType type)
{
    return wc_HashInit_ex(hash, type, NULL, INVALID_DEVID);
}

int wc_HashUpdate(wc_HashAlg* hash, enum wc_HashType type, const byte* data,
                  word32 dataSz)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || (data == NULL && dataSz > 0))
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5Update(&hash->md5, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaUpdate(&hash->sha, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Update(&hash->sha224, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Update(&hash->sha256, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Update(&hash->sha384, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Update(&hash->sha512, data, dataSz);
#endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            ret = wc_Sha512_224Update(&hash->sha512, data, dataSz);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            ret = wc_Sha512_256Update(&hash->sha512, data, dataSz);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_SHA3_224:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            ret = wc_Sha3_224_Update(&hash->sha3, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA3_256:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            ret = wc_Sha3_256_Update(&hash->sha3, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA3_384:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            ret = wc_Sha3_384_Update(&hash->sha3, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA3_512:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            ret = wc_Sha3_512_Update(&hash->sha3, data, dataSz);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_Sm3Update(&hash->sm3, data, dataSz);
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashFinal(wc_HashAlg* hash, enum wc_HashType type, byte* out)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5Final(&hash->md5, out);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaFinal(&hash->sha, out);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Final(&hash->sha224, out);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Final(&hash->sha256, out);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Final(&hash->sha384, out);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Final(&hash->sha512, out);
#endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            ret = wc_Sha512_224Final(&hash->sha512, out);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            ret = wc_Sha512_256Final(&hash->sha512, out);
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_SHA3_224:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            ret = wc_Sha3_224_Final(&hash->sha3, out);
#endif
            break;
        case WC_HASH_TYPE_SHA3_256:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            ret = wc_Sha3_256_Final(&hash->sha3, out);
#endif
            break;
        case WC_HASH_TYPE_SHA3_384:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            ret = wc_Sha3_384_Final(&hash->sha3, out);
#endif
            break;
        case WC_HASH_TYPE_SHA3_512:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            ret = wc_Sha3_512_Final(&hash->sha3, out);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_Sm3Final(&hash->sm3, out);
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashFree(wc_HashAlg* hash, enum wc_HashType type)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_Md5Free(&hash->md5);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            wc_ShaFree(&hash->sha);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            wc_Sha224Free(&hash->sha224);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            wc_Sha256Free(&hash->sha256);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            wc_Sha384Free(&hash->sha384);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            wc_Sha512Free(&hash->sha512);
            ret = 0;
#endif
            break;
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
            wc_Sha512_224Free(&hash->sha512);
            ret = 0;
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
            wc_Sha512_256Free(&hash->sha512);
            ret = 0;
#endif
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */
            break;
    #endif
        case WC_HASH_TYPE_SHA3_224:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
            wc_Sha3_224_Free(&hash->sha3);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA3_256:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
            wc_Sha3_256_Free(&hash->sha3);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA3_384:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
            wc_Sha3_384_Free(&hash->sha3);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA3_512:
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
            wc_Sha3_512_Free(&hash->sha3);
            ret = 0;
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            wc_Sm3Free(&hash->sm3);
            ret = 0;
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_HashSetFlags(wc_HashAlg* hash, enum wc_HashType type, word32 flags)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5SetFlags(&hash->md5, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaSetFlags(&hash->sha, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224SetFlags(&hash->sha224, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256SetFlags(&hash->sha256, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384SetFlags(&hash->sha384, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
    #endif
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512SetFlags(&hash->sha512, flags);
#endif
            break;

        case WC_HASH_TYPE_SHA3_224:
        case WC_HASH_TYPE_SHA3_256:
        case WC_HASH_TYPE_SHA3_384:
        case WC_HASH_TYPE_SHA3_512:
#ifdef WOLFSSL_SHA3
            ret = wc_Sha3_SetFlags(&hash->sha3, flags);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_Sm3SetFlags(&hash->sm3, flags);
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
        case WC_HASH_TYPE_NONE:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}
int wc_HashGetFlags(wc_HashAlg* hash, enum wc_HashType type, word32* flags)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5GetFlags(&hash->md5, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaGetFlags(&hash->sha, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224GetFlags(&hash->sha224, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256GetFlags(&hash->sha256, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384GetFlags(&hash->sha384, flags);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
    #endif
    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
    #endif
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512GetFlags(&hash->sha512, flags);
#endif
            break;

        case WC_HASH_TYPE_SHA3_224:
        case WC_HASH_TYPE_SHA3_256:
        case WC_HASH_TYPE_SHA3_384:
        case WC_HASH_TYPE_SHA3_512:
#ifdef WOLFSSL_SHA3
            ret = wc_Sha3_GetFlags(&hash->sha3, flags);
#endif
            break;

    #ifdef WOLFSSL_SM3
        case WC_HASH_TYPE_SM3:
            ret = wc_Sm3GetFlags(&hash->sm3, flags);
            break;
    #endif

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_BLAKE2B:
        case WC_HASH_TYPE_BLAKE2S:
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
        case WC_HASH_TYPE_SHAKE128:
    #endif
    #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
        case WC_HASH_TYPE_SHAKE256:
    #endif
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}
#endif /* WOLFSSL_HASH_FLAGS */


#if !defined(WOLFSSL_TI_HASH)

#if !defined(NO_MD5)
    int wc_Md5Hash(const byte* data, word32 len, byte* hash)
    {
        int ret;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Md5* md5;
    #else
        wc_Md5  md5[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        md5 = (wc_Md5*)XMALLOC(sizeof(wc_Md5), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (md5 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitMd5(md5)) != 0) {
            WOLFSSL_MSG("InitMd5 failed");
        }
        else {
            if ((ret = wc_Md5Update(md5, data, len)) != 0) {
                WOLFSSL_MSG("Md5Update failed");
            }
            else if ((ret = wc_Md5Final(md5, hash)) != 0) {
                WOLFSSL_MSG("Md5Final failed");
            }
            wc_Md5Free(md5);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(md5, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_MD5 */

#if !defined(NO_SHA)
    int wc_ShaHash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha* sha;
    #else
        wc_Sha sha[1];
    #endif
        int devId = INVALID_DEVID;

    #ifdef WOLFSSL_SMALL_STACK
        sha = (wc_Sha*)XMALLOC(sizeof(wc_Sha), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha == NULL)
            return MEMORY_E;
    #endif

    #ifdef WOLF_CRYPTO_CB
        /* only use devId if its not an empty hash */
        if (data != NULL && len > 0)
            devId = wc_CryptoCb_GetDevIdAtIndex(0);
    #endif

        if ((ret = wc_InitSha_ex(sha, NULL, devId)) != 0) {
            WOLFSSL_MSG("InitSha failed");
        }
        else {
            if ((ret = wc_ShaUpdate(sha, data, len)) != 0) {
                WOLFSSL_MSG("ShaUpdate failed");
            }
            else if ((ret = wc_ShaFinal(sha, hash)) != 0) {
                WOLFSSL_MSG("ShaFinal failed");
            }
            wc_ShaFree(sha);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA */

#if defined(WOLFSSL_SHA224)
    int wc_Sha224Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha224* sha224;
    #else
        wc_Sha224 sha224[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha224 = (wc_Sha224*)XMALLOC(sizeof(wc_Sha224), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha224 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha224(sha224)) != 0) {
            WOLFSSL_MSG("InitSha224 failed");
        }
        else {
            if ((ret = wc_Sha224Update(sha224, data, len)) != 0) {
                WOLFSSL_MSG("Sha224Update failed");
            }
            else if ((ret = wc_Sha224Final(sha224, hash)) != 0) {
                WOLFSSL_MSG("Sha224Final failed");
            }
            wc_Sha224Free(sha224);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha224, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

    return ret;
}
#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256)
    int wc_Sha256Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha256* sha256;
    #else
        wc_Sha256 sha256[1];
    #endif
        int devId = INVALID_DEVID;

    #ifdef WOLFSSL_SMALL_STACK
        sha256 = (wc_Sha256*)XMALLOC(sizeof(wc_Sha256), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha256 == NULL)
            return MEMORY_E;
    #endif

    #ifdef WOLF_CRYPTO_CB
        /* only use devId if its not an empty hash */
        if (data != NULL && len > 0)
            devId = wc_CryptoCb_GetDevIdAtIndex(0);
    #endif

        if ((ret = wc_InitSha256_ex(sha256, NULL, devId)) != 0) {
            WOLFSSL_MSG("InitSha256 failed");
        }
        else {
            if ((ret = wc_Sha256Update(sha256, data, len)) != 0) {
                WOLFSSL_MSG("Sha256Update failed");
            }
            else if ((ret = wc_Sha256Final(sha256, hash)) != 0) {
                WOLFSSL_MSG("Sha256Final failed");
            }
            wc_Sha256Free(sha256);
        }


    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA256 */

#endif /* !defined(WOLFSSL_TI_HASH) */


#if defined(WOLFSSL_SHA512)
    int wc_Sha512Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha512* sha512;
    #else
        wc_Sha512 sha512[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha512 = (wc_Sha512*)XMALLOC(sizeof(wc_Sha512), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha512 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha512(sha512)) != 0) {
            WOLFSSL_MSG("InitSha512 failed");
        }
        else {
            if ((ret = wc_Sha512Update(sha512, data, len)) != 0) {
                WOLFSSL_MSG("Sha512Update failed");
            }
            else if ((ret = wc_Sha512Final(sha512, hash)) != 0) {
                WOLFSSL_MSG("Sha512Final failed");
            }
            wc_Sha512Free(sha512);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha512, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#ifndef WOLFSSL_NOSHA512_224
    int wc_Sha512_224Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha512* sha512;
    #else
        wc_Sha512 sha512[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha512 = (wc_Sha512*)XMALLOC(sizeof(wc_Sha512), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha512 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha512_224(sha512)) != 0) {
            WOLFSSL_MSG("wc_InitSha512_224 failed");
        }
        else {
            if ((ret = wc_Sha512_224Update(sha512, data, len)) != 0) {
                WOLFSSL_MSG("wc_Sha512_224_Update failed");
            }
            else if ((ret = wc_Sha512_224Final(sha512, hash)) != 0) {
                WOLFSSL_MSG("wc_Sha512_224_Final failed");
            }
            wc_Sha512_224Free(sha512);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha512, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA512_224 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
#ifndef WOLFSSL_NOSHA512_256
    int wc_Sha512_256Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha512* sha512;
    #else
        wc_Sha512 sha512[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha512 = (wc_Sha512*)XMALLOC(sizeof(wc_Sha512), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha512 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha512_256(sha512)) != 0) {
            WOLFSSL_MSG("wc_InitSha512_256 failed");
        }
        else {
            if ((ret = wc_Sha512_256Update(sha512, data, len)) != 0) {
                WOLFSSL_MSG("wc_Sha512_256_Update failed");
            }
            else if ((ret = wc_Sha512_256Final(sha512, hash)) != 0) {
                WOLFSSL_MSG("wc_Sha512_256_Final failed");
            }
            wc_Sha512_256Free(sha512);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha512, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA512_256 */
#endif /* !HAVE_FIPS && !HAVE_SELFTEST */

#endif /* WOLFSSL_SHA512 */

#if defined(WOLFSSL_SHA384)
    int wc_Sha384Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha384* sha384;
    #else
        wc_Sha384 sha384[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha384 = (wc_Sha384*)XMALLOC(sizeof(wc_Sha384), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha384 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha384(sha384)) != 0) {
            WOLFSSL_MSG("InitSha384 failed");
        }
        else {
            if ((ret = wc_Sha384Update(sha384, data, len)) != 0) {
                WOLFSSL_MSG("Sha384Update failed");
            }
            else if ((ret = wc_Sha384Final(sha384, hash)) != 0) {
                WOLFSSL_MSG("Sha384Final failed");
            }
            wc_Sha384Free(sha384);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha384, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* WOLFSSL_SHA384 */

#if defined(WOLFSSL_SHA3)
#if !defined(WOLFSSL_NOSHA3_224)
    int wc_Sha3_224Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha3* sha3;
    #else
        wc_Sha3 sha3[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha3 = (wc_Sha3*)XMALLOC(sizeof(wc_Sha3), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha3 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha3_224(sha3, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitSha3_224 failed");
        }
        else {
            if ((ret = wc_Sha3_224_Update(sha3, data, len)) != 0) {
                WOLFSSL_MSG("Sha3_224_Update failed");
            }
            else if ((ret = wc_Sha3_224_Final(sha3, hash)) != 0) {
                WOLFSSL_MSG("Sha3_224_Final failed");
            }
            wc_Sha3_224_Free(sha3);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha3, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA3_224 */

#if !defined(WOLFSSL_NOSHA3_256)
    int wc_Sha3_256Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha3* sha3;
    #else
        wc_Sha3 sha3[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha3 = (wc_Sha3*)XMALLOC(sizeof(wc_Sha3), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha3 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha3_256(sha3, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitSha3_256 failed");
        }
        else {
            if ((ret = wc_Sha3_256_Update(sha3, data, len)) != 0) {
                WOLFSSL_MSG("Sha3_256_Update failed");
            }
            else if ((ret = wc_Sha3_256_Final(sha3, hash)) != 0) {
                WOLFSSL_MSG("Sha3_256_Final failed");
            }
            wc_Sha3_256_Free(sha3);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha3, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA3_256 */

#if !defined(WOLFSSL_NOSHA3_384)
    int wc_Sha3_384Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha3* sha3;
    #else
        wc_Sha3 sha3[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha3 = (wc_Sha3*)XMALLOC(sizeof(wc_Sha3), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha3 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha3_384(sha3, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitSha3_384 failed");
        }
        else {
            if ((ret = wc_Sha3_384_Update(sha3, data, len)) != 0) {
                WOLFSSL_MSG("Sha3_384_Update failed");
            }
            else if ((ret = wc_Sha3_384_Final(sha3, hash)) != 0) {
                WOLFSSL_MSG("Sha3_384_Final failed");
            }
            wc_Sha3_384_Free(sha3);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha3, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA3_384 */

#if !defined(WOLFSSL_NOSHA3_512)
    int wc_Sha3_512Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sha3* sha3;
    #else
        wc_Sha3 sha3[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha3 = (wc_Sha3*)XMALLOC(sizeof(wc_Sha3), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (sha3 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha3_512(sha3, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitSha3_512 failed");
        }
        else {
            if ((ret = wc_Sha3_512_Update(sha3, data, len)) != 0) {
                WOLFSSL_MSG("Sha3_512_Update failed");
            }
            else if ((ret = wc_Sha3_512_Final(sha3, hash)) != 0) {
                WOLFSSL_MSG("Sha3_512_Final failed");
            }
            wc_Sha3_512_Free(sha3);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha3, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA3_512 */

#ifdef WOLFSSL_SHAKE128
    int wc_Shake128Hash(const byte* data, word32 len, byte* hash,
                        word32 hashLen)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Shake* shake;
    #else
        wc_Shake shake[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        shake = (wc_Shake*)XMALLOC(sizeof(wc_Shake), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (shake == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitShake128(shake, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitShake128 failed");
        }
        else {
            if ((ret = wc_Shake128_Update(shake, data, len)) != 0) {
                WOLFSSL_MSG("Shake128_Update failed");
            }
            else if ((ret = wc_Shake128_Final(shake, hash, hashLen)) != 0) {
                WOLFSSL_MSG("Shake128_Final failed");
            }
            wc_Shake128_Free(shake);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(shake, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* WOLFSSL_SHAKE_128 */

#ifdef WOLFSSL_SHAKE256
    int wc_Shake256Hash(const byte* data, word32 len, byte* hash,
                        word32 hashLen)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Shake* shake;
    #else
        wc_Shake shake[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        shake = (wc_Shake*)XMALLOC(sizeof(wc_Shake), NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (shake == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitShake256(shake, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitShake256 failed");
        }
        else {
            if ((ret = wc_Shake256_Update(shake, data, len)) != 0) {
                WOLFSSL_MSG("Shake256_Update failed");
            }
            else if ((ret = wc_Shake256_Final(shake, hash, hashLen)) != 0) {
                WOLFSSL_MSG("Shake256_Final failed");
            }
            wc_Shake256_Free(shake);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(shake, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* WOLFSSL_SHAKE_256 */
#endif /* WOLFSSL_SHA3 */

#ifdef WOLFSSL_SM3
    int wc_Sm3Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        wc_Sm3* sm3;
    #else
        wc_Sm3 sm3[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sm3 = (wc_Sm3*)XMALLOC(sizeof(wc_Sm3), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sm3 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSm3(sm3, NULL, INVALID_DEVID)) != 0) {
            WOLFSSL_MSG("InitSm3 failed");
        }
        else {
            if ((ret = wc_Sm3Update(sm3, data, len)) != 0) {
                WOLFSSL_MSG("Sm3Update failed");
            }
            else if ((ret = wc_Sm3Final(sm3, hash)) != 0) {
                WOLFSSL_MSG("Sm3Final failed");
            }
            wc_Sm3Free(sm3);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sm3, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !WOLFSSL_NOSHA3_224 */

#endif /* !NO_HASH_WRAPPER */

#ifdef WOLFSSL_HASH_KEEP
int _wc_Hash_Grow(byte** msg, word32* used, word32* len, const byte* in,
                        int inSz, void* heap)
{
    if (*len < *used + inSz) {
        if (*msg == NULL) {
            *msg = (byte*)XMALLOC(*used + inSz, heap, DYNAMIC_TYPE_TMP_BUFFER);
        }
        else {
            byte* pt = (byte*)XREALLOC(*msg, *used + inSz, heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
            }
            *msg = pt;
        }
        if (*msg == NULL) {
            return MEMORY_E;
        }
        *len = *used + inSz;
    }
    XMEMCPY(*msg + *used, in, inSz);
    *used += inSz;
    return 0;
}
#endif /* WOLFSSL_HASH_KEEP */

