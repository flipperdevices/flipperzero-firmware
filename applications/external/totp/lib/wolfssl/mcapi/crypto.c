/* crypto.c
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


/* Implements Microchip CRYPTO API layer */
#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif
#include <wolfssl/wolfcrypt/settings.h>

#ifdef MICROCHIP_MPLAB_HARMONY
    #include "system_config.h"
    #include "crypto/crypto.h"
#else
    #include "crypto.h"
#endif

#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/compress.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifndef NO_MD5
/* Initialize MD5 */
int CRYPT_MD5_Initialize(CRYPT_MD5_CTX* md5)
{
    typedef char md5_test[sizeof(CRYPT_MD5_CTX) >= sizeof(Md5) ? 1 : -1];
    (void)sizeof(md5_test);

    if (md5 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitMd5((Md5*)md5);
}

int CRYPT_MD5_DataSizeSet(CRYPT_MD5_CTX* md5, unsigned int sz)
{
    if (md5 == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Md5SizeSet((Md5*)md5, sz);
#else
    (void)sz;
#endif

    return 0;
}

/* Add data to MD5 */
int CRYPT_MD5_DataAdd(CRYPT_MD5_CTX* md5, const unsigned char* input,
                      unsigned int sz)
{
    if (md5 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Md5Update((Md5*)md5, input, sz);
}


/* Get MD5 Final into digest */
int CRYPT_MD5_Finalize(CRYPT_MD5_CTX* md5, unsigned char* digest)
{
    if (md5 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Md5Final((Md5*)md5, digest);
}
#endif

#ifndef NO_SHA

/* Initialize SHA */
int CRYPT_SHA_Initialize(CRYPT_SHA_CTX* sha)
{
    typedef char sha_test[sizeof(CRYPT_SHA_CTX) >= sizeof(Sha) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha((wc_Sha*)sha);
}

int CRYPT_SHA_DataSizeSet(CRYPT_SHA_CTX* sha, unsigned int sz)
{
    if (sha == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_ShaSizeSet((wc_Sha*)sha, sz);
#else
    (void)sz;
#endif

    return 0;
}

/* Add data to SHA */
int CRYPT_SHA_DataAdd(CRYPT_SHA_CTX* sha, const unsigned char* input,
                       unsigned int sz)
{
    if (sha == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_ShaUpdate((wc_Sha*)sha, input, sz);
}


/* Get SHA Final into digest */
int CRYPT_SHA_Finalize(CRYPT_SHA_CTX* sha, unsigned char* digest)
{
    if (sha == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_ShaFinal((wc_Sha*)sha, digest);
}
#endif

#ifndef NO_SHA256

/* Initialize SHA-256 */
int CRYPT_SHA256_Initialize(CRYPT_SHA256_CTX* sha256)
{
    typedef char sha_test[sizeof(CRYPT_SHA256_CTX) >= sizeof(wc_Sha256) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha256 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha256((wc_Sha256*)sha256);
}

int CRYPT_SHA256_DataSizeSet(CRYPT_SHA256_CTX* sha256, unsigned int sz)
{
    if (sha256 == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_PIC32MZ_HASH
    wc_Sha256SizeSet((wc_Sha256*)sha256, sz);
#else
    (void)sz;
#endif

    return 0;
}

/* Add data to SHA-256 */
int CRYPT_SHA256_DataAdd(CRYPT_SHA256_CTX* sha256, const unsigned char* input,
                         unsigned int sz)
{
    if (sha256 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha256Update((wc_Sha256*)sha256, input, sz);
}


/* Get SHA-256 Final into digest */
int CRYPT_SHA256_Finalize(CRYPT_SHA256_CTX* sha256, unsigned char* digest)
{
    if (sha256 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha256Final((wc_Sha256*)sha256, digest);
}
#endif

#ifdef WOLFSSL_SHA512
#ifdef WOLFSSL_SHA384
/* Initialize SHA-384 */
int CRYPT_SHA384_Initialize(CRYPT_SHA384_CTX* sha384)
{
    typedef char sha_test[sizeof(CRYPT_SHA384_CTX) >= sizeof(wc_Sha384) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha384 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha384((wc_Sha384*)sha384);
}


/* Add data to SHA-384 */
int CRYPT_SHA384_DataAdd(CRYPT_SHA384_CTX* sha384, const unsigned char* input,
                         unsigned int sz)
{
    if (sha384 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha384Update((wc_Sha384*)sha384, input, sz);
}


/* Get SHA-384 Final into digest */
int CRYPT_SHA384_Finalize(CRYPT_SHA384_CTX* sha384, unsigned char* digest)
{
    if (sha384 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha384Final((wc_Sha384*)sha384, digest);
}
#endif


/* Initialize SHA-512 */
int CRYPT_SHA512_Initialize(CRYPT_SHA512_CTX* sha512)
{
    typedef char sha_test[sizeof(CRYPT_SHA512_CTX) >= sizeof(wc_Sha512) ? 1 : -1];
    (void)sizeof(sha_test);

    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    return wc_InitSha512((wc_Sha512*)sha512);
}


/* Add data to SHA-512 */
int CRYPT_SHA512_DataAdd(CRYPT_SHA512_CTX* sha512, const unsigned char* input,
                         unsigned int sz)
{
    if (sha512 == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha512Update((wc_Sha512*)sha512, input, sz);
}


/* Get SHA-512 Final into digest */
int CRYPT_SHA512_Finalize(CRYPT_SHA512_CTX* sha512, unsigned char* digest)
{
    if (sha512 == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_Sha512Final((wc_Sha512*)sha512, digest);
}
#endif

#ifndef NO_HMAC
/* Set HMAC key with type */
int CRYPT_HMAC_SetKey(CRYPT_HMAC_CTX* hmac, int type, const unsigned char* key,
                      unsigned int sz)
{
    /* compile-time check to verify CRYPT_HMAC_CTX is large enough to hold Hmac */
    typedef char hmac_test[sizeof(CRYPT_HMAC_CTX) >= sizeof(Hmac) ? 1 : -1];
    (void)sizeof(hmac_test);

    if (hmac == NULL || key == NULL)
        return BAD_FUNC_ARG;

    if (type != CRYPT_HMAC_SHA && type != CRYPT_HMAC_SHA256 &&
        type != CRYPT_HMAC_SHA384 && type != CRYPT_HMAC_SHA512) {
        return BAD_FUNC_ARG;  /* bad hmac type */
    }

    return wc_HmacSetKey((Hmac*)hmac, type, key, sz);
}


int CRYPT_HMAC_DataAdd(CRYPT_HMAC_CTX* hmac, const unsigned char* input,
                       unsigned int sz)
{
    if (hmac == NULL || input == NULL)
        return BAD_FUNC_ARG;

    return wc_HmacUpdate((Hmac*)hmac, input, sz);
}


/* Get HMAC Final into digest */
int CRYPT_HMAC_Finalize(CRYPT_HMAC_CTX* hmac, unsigned char* digest)
{
    if (hmac == NULL || digest == NULL)
        return BAD_FUNC_ARG;

    return wc_HmacFinal((Hmac*)hmac, digest);
}

#endif

#ifdef HAVE_LIBZ

/* Huffman Compression, set flag to do static, otherwise dynamic */
/* return compressed size, otherwise < 0 for error */
int CRYPT_HUFFMAN_Compress(unsigned char* out, unsigned int outSz,
                           const unsigned char* in, unsigned int inSz,
                           unsigned int flags)
{
    if (out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Compress(out, outSz, in, inSz, flags);
}


/* Huffman DeCompression, self determines type */
/* return decompressed size, otherwise < 0 for error */
int CRYPT_HUFFMAN_DeCompress(unsigned char* out, unsigned int outSz,
                             const unsigned char* in, unsigned int inSz)
{
    if (out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_DeCompress(out, outSz, in, inSz);
}

#endif

#ifndef NO_RNG

/* RNG Initialize, < 0 on error */
int CRYPT_RNG_Initialize(CRYPT_RNG_CTX* rng)
{
    typedef char rng_test[sizeof(CRYPT_RNG_CTX) >= sizeof(WC_RNG) ? 1 : -1];
    (void)sizeof(rng_test);

    if (rng == NULL)
        return BAD_FUNC_ARG;

    return wc_InitRng((WC_RNG*)rng);
}


/* RNG Get single bytes, < 0 on error */
int CRYPT_RNG_Get(CRYPT_RNG_CTX* rng, unsigned char* b)
{
    if (rng == NULL || b == NULL)
        return BAD_FUNC_ARG;

    return wc_RNG_GenerateByte((WC_RNG*)rng, (byte*)b);
}


/* RNG Block Generation of sz bytes, < 0 on error */
int CRYPT_RNG_BlockGenerate(CRYPT_RNG_CTX* rng, unsigned char* b,
                            unsigned int sz)
{
    if (rng == NULL || b == NULL)
        return BAD_FUNC_ARG;

    return wc_RNG_GenerateBlock((WC_RNG*)rng, b, sz);
}

#endif

#ifndef NO_DES3
/* Triple DES Key Set, may have iv, will have direction */
int CRYPT_TDES_KeySet(CRYPT_TDES_CTX* tdes, const unsigned char* key,
                      const unsigned char* iv, int dir)
{
    typedef char tdes_test[sizeof(CRYPT_TDES_CTX) >= sizeof(Des3) ? 1 : -1];
    (void)sizeof(tdes_test);

    if (tdes == NULL || key == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_SetKey((Des3*)tdes, key, iv, dir);
}


/* Triple DES Iv Set, sometimes added later */
int CRYPT_TDES_IvSet(CRYPT_TDES_CTX* tdes, const unsigned char* iv)
{
    if (tdes == NULL || iv == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_SetIV((Des3*)tdes, iv);
}


/* Triple DES CBC Encrypt */
int CRYPT_TDES_CBC_Encrypt(CRYPT_TDES_CTX* tdes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)
{
    if (tdes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_CbcEncrypt((Des3*)tdes, out, in, inSz);
}


/* Triple DES CBC Decrypt */
int CRYPT_TDES_CBC_Decrypt(CRYPT_TDES_CTX* tdes, unsigned char* out,
                           const unsigned char* in, unsigned int inSz)
{
    if (tdes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_Des3_CbcDecrypt((Des3*)tdes, out, in, inSz);
}

#endif

#ifndef NO_AES

/* AES Key Set, may have iv, will have direction */
int CRYPT_AES_KeySet(CRYPT_AES_CTX* aes, const unsigned char* key,
                     unsigned int keyLen, const unsigned char* iv, int dir)
{
    typedef char aes_test[sizeof(CRYPT_AES_CTX) >= sizeof(Aes) ? 1 : -1];
    (void)sizeof(aes_test);

    if (aes == NULL || key == NULL)
        return BAD_FUNC_ARG;

    return wc_AesSetKey((Aes*)aes, key, keyLen, iv, dir);
}


/* AES Iv Set, sometimes added later */
int CRYPT_AES_IvSet(CRYPT_AES_CTX* aes, const unsigned char* iv)
{
    if (aes == NULL || iv == NULL)
        return BAD_FUNC_ARG;

    return wc_AesSetIV((Aes*)aes, iv);
}


/* AES CBC Encrypt */
int CRYPT_AES_CBC_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCbcEncrypt((Aes*)aes, out, in, inSz);
}


/* AES CBC Decrypt */
int CRYPT_AES_CBC_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCbcDecrypt((Aes*)aes, out, in, inSz);
}
#endif

#ifdef WOLFSSL_AES_COUNTER
/* AES CTR Encrypt (used for decrypt too, with ENCRYPT key setup) */
int CRYPT_AES_CTR_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                          const unsigned char* in, unsigned int inSz)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_AesCtrEncrypt((Aes*)aes, out, in, inSz);
}
#endif

#ifdef WOLFSSL_AES_DIRECT

/* AES Direct mode encrypt, one block at a time */
int CRYPT_AES_DIRECT_Encrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                             const unsigned char* in)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    wc_AesEncryptDirect((Aes*)aes, out, in);

    return 0;
}


/* AES Direct mode decrypt, one block at a time */
int CRYPT_AES_DIRECT_Decrypt(CRYPT_AES_CTX* aes, unsigned char* out,
                             const unsigned char* in)
{
    if (aes == NULL || out == NULL || in == NULL)
        return BAD_FUNC_ARG;

    wc_AesDecryptDirect((Aes*)aes, out, in);

    return 0;
}
#endif

#ifndef NO_RSA

/* RSA Initialize */
int CRYPT_RSA_Initialize(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    rsa->holder = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_RSA);
    if (rsa->holder == NULL)
        return -1;

    return wc_InitRsaKey((RsaKey*)rsa->holder, NULL);
}


/* RSA Free resources */
int CRYPT_RSA_Free(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    wc_FreeRsaKey((RsaKey*)rsa->holder);
    XFREE(rsa->holder, NULL, DYNAMIC_TYPE_RSA);
    rsa->holder = NULL;

    return 0;
}


/* RSA Public key decode ASN.1 */
int CRYPT_RSA_PublicKeyDecode(CRYPT_RSA_CTX* rsa, const unsigned char* in,
                              unsigned int inSz)
{
    unsigned int idx = 0;
    (void)idx;

    if (rsa == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPublicKeyDecode(in, &idx, (RsaKey*)rsa->holder, inSz);
}


/* RSA Private key decode ASN.1 */
int CRYPT_RSA_PrivateKeyDecode(CRYPT_RSA_CTX* rsa, const unsigned char* in,
                               unsigned int inSz)
{
    unsigned int idx = 0;
    (void)idx;

    if (rsa == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPrivateKeyDecode(in, &idx, (RsaKey*)rsa->holder, inSz);
}


/* RSA Public Encrypt */
int CRYPT_RSA_PublicEncrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                            unsigned int outSz, const unsigned char* in,
                            unsigned int inSz, CRYPT_RNG_CTX* rng)
{
    if (rsa == NULL || in == NULL || out == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPublicEncrypt(in, inSz, out, outSz, (RsaKey*)rsa->holder,
                            (WC_RNG*)rng);
}


/* RSA Private Decrypt */
int CRYPT_RSA_PrivateDecrypt(CRYPT_RSA_CTX* rsa, unsigned char* out,
                             unsigned int outSz, const unsigned char* in,
                             unsigned int inSz)
{
    if (rsa == NULL || in == NULL || out == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaPrivateDecrypt(in, inSz, out, outSz, (RsaKey*)rsa->holder);
}


/* RSA Get Encrypt size helper */
int CRYPT_RSA_EncryptSizeGet(CRYPT_RSA_CTX* rsa)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

    return wc_RsaEncryptSize((RsaKey*)rsa->holder);
}


int CRYPT_RSA_SetRng(CRYPT_RSA_CTX* rsa, CRYPT_RNG_CTX* rng)
{
    if (rsa == NULL)
        return BAD_FUNC_ARG;

#ifdef WC_RSA_BLINDING
    return wc_RsaSetRNG((RsaKey*)rsa->holder, (WC_RNG*)rng);
#else
    (void)rng;

    return 0;
#endif
}
#endif

#ifdef HAVE_ECC

/* ECC init */
int CRYPT_ECC_Initialize(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    ecc->holder = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (ecc->holder == NULL)
        return -1;

    wc_ecc_init((ecc_key*)ecc->holder);

    return 0;
}


/* ECC free resources */
int CRYPT_ECC_Free(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    wc_ecc_free((ecc_key*)ecc->holder);
    XFREE(ecc->holder, NULL, DYNAMIC_TYPE_ECC);
    ecc->holder = NULL;

    return 0;
}


/* ECC Public x963 Export */
int CRYPT_ECC_PublicExport(CRYPT_ECC_CTX* ecc, unsigned char* out,
                           unsigned int outSz, unsigned int* usedSz)
{
    int          ret;
    unsigned int inOut = outSz;

    if (ecc == NULL || out == NULL)
        return BAD_FUNC_ARG;

    ret = wc_ecc_export_x963((ecc_key*)ecc->holder, out, &inOut);
    *usedSz = inOut;

    return ret;
}


/* ECC Public x963 Import */
int CRYPT_ECC_PublicImport(CRYPT_ECC_CTX* ecc, const unsigned char* in,
                           unsigned int inSz)
{
    if (ecc == NULL || in == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_import_x963(in, inSz, (ecc_key*)ecc->holder);
}


/* ECC Private x963 Import */
int CRYPT_ECC_PrivateImport(CRYPT_ECC_CTX* ecc, const unsigned char* priv,
         unsigned int privSz, const unsigned char* pub, unsigned int pubSz)
{
    if (ecc == NULL || priv == NULL || pub == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_import_private_key(priv, privSz, pub, pubSz,
                                 (ecc_key*)ecc->holder);
}


/* ECC DHE Make key */
int CRYPT_ECC_DHE_KeyMake(CRYPT_ECC_CTX* ecc, CRYPT_RNG_CTX* rng, int keySz)
{
    if (ecc == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_make_key((WC_RNG*)rng, keySz, (ecc_key*)ecc->holder);
}


/* ECC DHE Make shared secret with our private and peer public */
int CRYPT_ECC_DHE_SharedSecretMake(CRYPT_ECC_CTX* priv, CRYPT_ECC_CTX* pub,
                  unsigned char* out, unsigned int outSz, unsigned int* usedSz)
{
    int ret;
    unsigned int inOut = outSz;
#if defined(ECC_TIMING_RESISTANT)
    WC_RNG rng;
#endif

    if (priv == NULL || pub == NULL || out == NULL || usedSz == NULL)
        return BAD_FUNC_ARG;

#if defined(ECC_TIMING_RESISTANT)
    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;
    ret = wc_ecc_set_rng((ecc_key*)priv->holder, &rng);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }
#endif

    ret = wc_ecc_shared_secret((ecc_key*)priv->holder, (ecc_key*)pub->holder,
                            out, &inOut);
    *usedSz = inOut;

#if defined(ECC_TIMING_RESISTANT)
    wc_FreeRng(&rng);
#endif

    return ret;
}


/* ECC DSA Hash Sign */
int CRYPT_ECC_DSA_HashSign(CRYPT_ECC_CTX* ecc, CRYPT_RNG_CTX* rng,
                           unsigned char* sig, unsigned int sigSz,
                           unsigned int* usedSz, const unsigned char* in,
                           unsigned int inSz)
{
    int ret;
    unsigned int inOut = sigSz;

    if (ecc == NULL || rng == NULL || sig == NULL || usedSz == NULL ||
                                                                in == NULL)
        return BAD_FUNC_ARG;

    ret = wc_ecc_sign_hash(in, inSz, sig, &inOut, (WC_RNG*)rng,
                       (ecc_key*)ecc->holder);
    *usedSz = inOut;

    return ret;
}


/* ECC DSA Hash Verify */
int CRYPT_ECC_DSA_HashVerify(CRYPT_ECC_CTX* ecc, const unsigned char* sig,
                             unsigned int sigSz, unsigned char* hash,
                             unsigned int hashSz, int* status)
{
    if (ecc == NULL || sig == NULL || hash == NULL || status == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_verify_hash(sig, sigSz, hash, hashSz, status,
                          (ecc_key*)ecc->holder);
}


/* ECC get key size helper */
int CRYPT_ECC_KeySizeGet(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_size((ecc_key*)ecc->holder);
}


/* ECC get signature size helper */
int CRYPT_ECC_SignatureSizeGet(CRYPT_ECC_CTX* ecc)
{
    if (ecc == NULL)
        return BAD_FUNC_ARG;

    return wc_ecc_sig_size((ecc_key*)ecc->holder);
}

#endif

/* Save error string from err to str which needs to be >= 80 chars */
int CRYPT_ERROR_StringGet(int err, char* str)
{
    if (str == NULL)
        return BAD_FUNC_ARG;

    wc_ErrorString(err, str);

    return 0;
}

