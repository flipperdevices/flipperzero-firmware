/* crypto.h
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



/* Defines Microchip CRYPTO API layer */


#ifndef MC_CRYPTO_API_H
#define MC_CRYPTO_API_H

#ifdef __cplusplus
    extern "C" {
#endif

/* MD5 */
typedef struct CRYPT_MD5_CTX {
    int holder[28];   /* big enough to hold internal, but check on init */
} CRYPT_MD5_CTX;

int CRYPT_MD5_Initialize(CRYPT_MD5_CTX*);
int CRYPT_MD5_DataAdd(CRYPT_MD5_CTX*, const unsigned char*, unsigned int);
int CRYPT_MD5_Finalize(CRYPT_MD5_CTX*, unsigned char*);
int CRYPT_MD5_DataSizeSet(CRYPT_MD5_CTX* md5, unsigned int sz);

enum {
    CRYPT_MD5_DIGEST_SIZE = 16
};


/* SHA */
typedef struct CRYPT_SHA_CTX {
    int holder[29];   /* big enough to hold internal, but check on init */
} CRYPT_SHA_CTX;

int CRYPT_SHA_Initialize(CRYPT_SHA_CTX*);
int CRYPT_SHA_DataAdd(CRYPT_SHA_CTX*, const unsigned char*, unsigned int);
int CRYPT_SHA_Finalize(CRYPT_SHA_CTX*, unsigned char*);
int CRYPT_SHA_DataSizeSet(CRYPT_SHA_CTX* sha, unsigned int sz);

enum {
    CRYPT_SHA_DIGEST_SIZE = 20
};


/* SHA-256 */
typedef struct CRYPT_SHA256_CTX {
    int holder[32];   /* big enough to hold internal, but check on init */
} CRYPT_SHA256_CTX;

int CRYPT_SHA256_Initialize(CRYPT_SHA256_CTX*);
int CRYPT_SHA256_DataAdd(CRYPT_SHA256_CTX*, const unsigned char*, unsigned int);
int CRYPT_SHA256_Finalize(CRYPT_SHA256_CTX*, unsigned char*);
int CRYPT_SHA256_DataSizeSet(CRYPT_SHA256_CTX* sha256, unsigned int sz);

enum {
    CRYPT_SHA256_DIGEST_SIZE = 32
};


/* SHA-384 */
typedef struct CRYPT_SHA384_CTX {
    long long holder[36];   /* big enough to hold internal, but check on init */
} CRYPT_SHA384_CTX;

int CRYPT_SHA384_Initialize(CRYPT_SHA384_CTX*);
int CRYPT_SHA384_DataAdd(CRYPT_SHA384_CTX*, const unsigned char*, unsigned int);
int CRYPT_SHA384_Finalize(CRYPT_SHA384_CTX*, unsigned char*);

enum {
    CRYPT_SHA384_DIGEST_SIZE = 48
};


/* SHA-512 */
typedef struct CRYPT_SHA512_CTX {
    long long holder[36];   /* big enough to hold internal, but check on init */
} CRYPT_SHA512_CTX;

int CRYPT_SHA512_Initialize(CRYPT_SHA512_CTX*);
int CRYPT_SHA512_DataAdd(CRYPT_SHA512_CTX*, const unsigned char*, unsigned int);
int CRYPT_SHA512_Finalize(CRYPT_SHA512_CTX*, unsigned char*);

enum {
    CRYPT_SHA512_DIGEST_SIZE = 64
};


/* HMAC */
typedef struct CRYPT_HMAC_CTX {
    /* big enough to hold internal, but check on init */
    #ifdef WOLF_PRIVATE_KEY_ID
    long long holder[108];
    #else
    long long holder[98];
    #endif
} CRYPT_HMAC_CTX;

int CRYPT_HMAC_SetKey(CRYPT_HMAC_CTX*, int, const unsigned char*, unsigned int);
int CRYPT_HMAC_DataAdd(CRYPT_HMAC_CTX*, const unsigned char*, unsigned int);
int CRYPT_HMAC_Finalize(CRYPT_HMAC_CTX*, unsigned char*);

/* HMAC types */
enum {
    CRYPT_HMAC_SHA    = 4,
    CRYPT_HMAC_SHA256 = 6,
    CRYPT_HMAC_SHA384 = 7,
    CRYPT_HMAC_SHA512 = 8
};


/* Huffman */
int CRYPT_HUFFMAN_Compress(unsigned char*, unsigned int, const unsigned char*,
                           unsigned int, unsigned int);
int CRYPT_HUFFMAN_DeCompress(unsigned char*, unsigned int, const unsigned char*,
                             unsigned int);

/* flag to use static huffman */
enum {
    CRYPT_HUFFMAN_COMPRESS_STATIC = 1
};


/* RNG */
typedef struct CRYPT_RNG_CTX {
    int holder[66];   /* big enough to hold internal, but check on init */
} CRYPT_RNG_CTX;

int CRYPT_RNG_Initialize(CRYPT_RNG_CTX*);
int CRYPT_RNG_Get(CRYPT_RNG_CTX*, unsigned char*);
int CRYPT_RNG_BlockGenerate(CRYPT_RNG_CTX*, unsigned char*, unsigned int);


/* TDES */
typedef struct CRYPT_TDES_CTX {
    int holder[104];   /* big enough to hold internal, but check on init */
} CRYPT_TDES_CTX;

int CRYPT_TDES_KeySet(CRYPT_TDES_CTX*, const unsigned char*,
                      const unsigned char*, int);
int CRYPT_TDES_IvSet(CRYPT_TDES_CTX*, const unsigned char*);
int CRYPT_TDES_CBC_Encrypt(CRYPT_TDES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);
int CRYPT_TDES_CBC_Decrypt(CRYPT_TDES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);

/* key direction flags for setup */
enum {
    CRYPT_TDES_ENCRYPTION = 0,
    CRYPT_TDES_DECRYPTION = 1
};


/* AES */
typedef struct CRYPT_AES_CTX {
    /* big enough to hold internal, but check on init */
    #ifdef WOLF_PRIVATE_KEY_ID
    int holder[110];
    #else
    int holder[92];
    #endif
} CRYPT_AES_CTX;

/* key */
int CRYPT_AES_KeySet(CRYPT_AES_CTX*, const unsigned char*, unsigned int,
                      const unsigned char*, int);
int CRYPT_AES_IvSet(CRYPT_AES_CTX*, const unsigned char*);

/* cbc */
int CRYPT_AES_CBC_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);
int CRYPT_AES_CBC_Decrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*, unsigned int);

/* ctr (counter), use Encrypt both ways with ENCRYPT key setup */
int CRYPT_AES_CTR_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                          const unsigned char*, unsigned int);

/* direct, one block at a time */
int CRYPT_AES_DIRECT_Encrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*);
int CRYPT_AES_DIRECT_Decrypt(CRYPT_AES_CTX*, unsigned char*,
                           const unsigned char*);

/* key direction flags for setup, ctr always uses ENCRYPT flag */
enum {
    CRYPT_AES_ENCRYPTION = 0,
    CRYPT_AES_DECRYPTION = 1,
    CRYPT_AES_BLOCK_SIZE = 16
};



/* RSA */
typedef struct CRYPT_RSA_CTX {
    void* holder;
} CRYPT_RSA_CTX;

/* init/free */
int CRYPT_RSA_Initialize(CRYPT_RSA_CTX*);
int CRYPT_RSA_Free(CRYPT_RSA_CTX*);

/* key decode */
int CRYPT_RSA_PublicKeyDecode(CRYPT_RSA_CTX*, const unsigned char*,
                              unsigned int);
int CRYPT_RSA_PrivateKeyDecode(CRYPT_RSA_CTX*, const unsigned char*,
                               unsigned int);

/* encrypt/decrypt */
int CRYPT_RSA_PublicEncrypt(CRYPT_RSA_CTX*, unsigned char*,
                            unsigned int, const unsigned char*, unsigned int,
                            CRYPT_RNG_CTX*);
int CRYPT_RSA_PrivateDecrypt(CRYPT_RSA_CTX*, unsigned char*,
                             unsigned int, const unsigned char*, unsigned int);

/* helpers */
int CRYPT_RSA_EncryptSizeGet(CRYPT_RSA_CTX*);
int CRYPT_RSA_SetRng(CRYPT_RSA_CTX*, CRYPT_RNG_CTX*);



/* ECC */
typedef struct CRYPT_ECC_CTX {
    void* holder;
} CRYPT_ECC_CTX;

/* init/free */
int CRYPT_ECC_Initialize(CRYPT_ECC_CTX*);
int CRYPT_ECC_Free(CRYPT_ECC_CTX*);

/* key coders */
int CRYPT_ECC_PublicExport(CRYPT_ECC_CTX*, unsigned char*, unsigned int,
                           unsigned int*);
int CRYPT_ECC_PublicImport(CRYPT_ECC_CTX*, const unsigned char*, unsigned int);
int CRYPT_ECC_PrivateImport(CRYPT_ECC_CTX*, const unsigned char*, unsigned int,
                            const unsigned char*, unsigned int);

/* dhe */
int CRYPT_ECC_DHE_KeyMake(CRYPT_ECC_CTX*, CRYPT_RNG_CTX*, int);
int CRYPT_ECC_DHE_SharedSecretMake(CRYPT_ECC_CTX*, CRYPT_ECC_CTX*,
                                   unsigned char*, unsigned int, unsigned int*);

/* dsa */
int CRYPT_ECC_DSA_HashSign(CRYPT_ECC_CTX*, CRYPT_RNG_CTX*, unsigned char*,
        unsigned int, unsigned int*, const unsigned char*, unsigned int);
int CRYPT_ECC_DSA_HashVerify(CRYPT_ECC_CTX*, const unsigned char*,
                     unsigned int, unsigned char*, unsigned int, int*);

/* helpers */
int CRYPT_ECC_KeySizeGet(CRYPT_ECC_CTX*);
int CRYPT_ECC_SignatureSizeGet(CRYPT_ECC_CTX*);


/* Error string helper, string needs to be >= 80 chars */
int CRYPT_ERROR_StringGet(int, char*);


#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* MC_CRYPTO_API_H */

