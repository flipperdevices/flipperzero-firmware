/* evp.c
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

#if !defined(WOLFSSL_EVP_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning evp.c does not need to be compiled separately from ssl.c
    #endif
#elif defined(WOLFCRYPT_ONLY)
#else

#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)

#if !defined(HAVE_PKCS7) && \
      ((defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && \
       (HAVE_FIPS_VERSION == 2)) || defined(HAVE_SELFTEST))
    #include <wolfssl/wolfcrypt/aes.h>
#endif


#include <wolfssl/openssl/ecdsa.h>
#include <wolfssl/openssl/evp.h>
#include <wolfssl/openssl/kdf.h>
#include <wolfssl/wolfcrypt/wolfmath.h>

static const struct s_ent {
    const enum wc_HashType macType;
    const int nid;
    const char *name;
} md_tbl[] = {
#ifndef NO_MD4
    {WC_HASH_TYPE_MD4, NID_md4, "MD4"},
#endif /* NO_MD4 */

#ifndef NO_MD5
    {WC_HASH_TYPE_MD5, NID_md5, "MD5"},
#endif /* NO_MD5 */

#ifndef NO_SHA
    {WC_HASH_TYPE_SHA, NID_sha1, "SHA1"},
    {WC_HASH_TYPE_SHA, NID_sha1, "SHA"}, /* Leave for backwards compatibility */
#endif /* NO_SHA */

#ifdef WOLFSSL_SHA224
    {WC_HASH_TYPE_SHA224, NID_sha224, "SHA224"},
#endif /* WOLFSSL_SHA224 */
#ifndef NO_SHA256
    {WC_HASH_TYPE_SHA256, NID_sha256, "SHA256"},
#endif

#ifdef WOLFSSL_SHA384
    {WC_HASH_TYPE_SHA384, NID_sha384, "SHA384"},
#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA512
    {WC_HASH_TYPE_SHA512, NID_sha512, "SHA512"},
#endif /* WOLFSSL_SHA512 */

#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
    {WC_HASH_TYPE_SHA512_224, NID_sha512_224, "SHA512_224"},
#endif /* WOLFSSL_SHA512 && !WOLFSSL_NOSHA512_224 */

#if defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
    {WC_HASH_TYPE_SHA512_256, NID_sha512_256, "SHA512_256"},
#endif /* WOLFSSL_SHA512 && !WOLFSSL_NOSHA512_256 */

#ifndef WOLFSSL_NOSHA3_224
    {WC_HASH_TYPE_SHA3_224, NID_sha3_224, "SHA3_224"},
#endif
#ifndef WOLFSSL_NOSHA3_256
    {WC_HASH_TYPE_SHA3_256, NID_sha3_256, "SHA3_256"},
#endif
#ifndef WOLFSSL_NOSHA3_384
    {WC_HASH_TYPE_SHA3_384, NID_sha3_384, "SHA3_384"},
#endif
#ifndef WOLFSSL_NOSHA3_512
    {WC_HASH_TYPE_SHA3_512, NID_sha3_512, "SHA3_512"},
#endif
#ifdef WOLFSSL_SM3
    {WC_HASH_TYPE_SM3, NID_sm3, "SM3"},
#endif /* WOLFSSL_SHA512 */
#ifdef HAVE_BLAKE2
    {WC_HASH_TYPE_BLAKE2B, NID_blake2b512, "BLAKE2B512"},
#endif
#ifdef HAVE_BLAKE2S
    {WC_HASH_TYPE_BLAKE2S, NID_blake2s256, "BLAKE2S256"},
#endif
#ifdef WOLFSSL_SHAKE128
    {WC_HASH_TYPE_SHAKE128, NID_shake128, "SHAKE128"},
#endif
#ifdef WOLFSSL_SHAKE256
    {WC_HASH_TYPE_SHAKE256, NID_shake256, "SHAKE256"},
#endif
    {WC_HASH_TYPE_NONE, 0, NULL}
};
#endif /* OPENSSL_EXTRA || HAVE_CURL */

#if defined(OPENSSL_EXTRA)

#ifndef NO_AES
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_CBC[] = "AES-128-CBC";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_CBC[] = "AES-192-CBC";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_CBC[] = "AES-256-CBC";
    #endif
    #endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */

    #ifdef WOLFSSL_AES_OFB
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_OFB[] = "AES-128-OFB";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_OFB[] = "AES-192-OFB";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_OFB[] = "AES-256-OFB";
    #endif
    #endif /* WOLFSSL_AES_OFB */

    #ifdef WOLFSSL_AES_XTS
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_XTS[] = "AES-128-XTS";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_XTS[] = "AES-256-XTS";
    #endif
    #endif /* WOLFSSL_AES_XTS */

    #ifdef WOLFSSL_AES_CFB
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_CFB1[] = "AES-128-CFB1";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_CFB1[] = "AES-192-CFB1";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_CFB1[] = "AES-256-CFB1";
    #endif

    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_CFB8[] = "AES-128-CFB8";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_CFB8[] = "AES-192-CFB8";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_CFB8[] = "AES-256-CFB8";
    #endif

    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_CFB128[] = "AES-128-CFB128";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_CFB128[] = "AES-192-CFB128";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_CFB128[] = "AES-256-CFB128";
    #endif
    #endif /* WOLFSSL_AES_CFB */

    #ifdef HAVE_AESGCM
        #ifdef WOLFSSL_AES_128
            static const char EVP_AES_128_GCM[] = "AES-128-GCM";
        #endif
        #ifdef WOLFSSL_AES_192
            static const char EVP_AES_192_GCM[] = "AES-192-GCM";
        #endif
        #ifdef WOLFSSL_AES_256
            static const char EVP_AES_256_GCM[] = "AES-256-GCM";
        #endif
    #endif /* HAVE_AESGCM */

    #ifdef HAVE_AESCCM
        #ifdef WOLFSSL_AES_128
            static const char EVP_AES_128_CCM[] = "AES-128-CCM";
        #endif
        #ifdef WOLFSSL_AES_192
            static const char EVP_AES_192_CCM[] = "AES-192-CCM";
        #endif
        #ifdef WOLFSSL_AES_256
            static const char EVP_AES_256_CCM[] = "AES-256-CCM";
        #endif
    #endif /* HAVE_AESCCM */

    #ifdef WOLFSSL_AES_COUNTER
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_CTR[] = "AES-128-CTR";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_CTR[] = "AES-192-CTR";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_CTR[] = "AES-256-CTR";
    #endif
    #endif

    #ifdef HAVE_AES_ECB
    #ifdef WOLFSSL_AES_128
        static const char EVP_AES_128_ECB[] = "AES-128-ECB";
    #endif
    #ifdef WOLFSSL_AES_192
        static const char EVP_AES_192_ECB[] = "AES-192-ECB";
    #endif
    #ifdef WOLFSSL_AES_256
        static const char EVP_AES_256_ECB[] = "AES-256-ECB";
    #endif
    #endif
#endif

#ifdef HAVE_ARIA
    #include <wolfssl/wolfcrypt/port/aria/aria-crypt.h>
    static const char EVP_ARIA_128_GCM[] = "ARIA-128-GCM";
    static const char EVP_ARIA_192_GCM[] = "ARIA-192-GCM";
    static const char EVP_ARIA_256_GCM[] = "ARIA-256-GCM";
#endif

#ifndef NO_DES3
    static const char EVP_DES_CBC[] = "DES-CBC";
    static const char EVP_DES_ECB[] = "DES-ECB";

    static const char EVP_DES_EDE3_CBC[] = "DES-EDE3-CBC";
    static const char EVP_DES_EDE3_ECB[] = "DES-EDE3-ECB";
#endif

#ifndef NO_RC4
    static const char EVP_ARC4[] = "ARC4";
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    static const char EVP_CHACHA20_POLY1305[] = "CHACHA20-POLY1305";
#endif

#ifdef HAVE_CHACHA
    static const char EVP_CHACHA20[] = "CHACHA20";
#endif

#ifdef WOLFSSL_SM4_ECB
    static const char EVP_SM4_ECB[] = "SM4-ECB";
#endif /* WOLFSSL_SM4_ECB */
#ifdef WOLFSSL_SM4_CBC
    static const char EVP_SM4_CBC[] = "SM4-CBC";
#endif /* WOLFSSL_SM4_CBC */
#ifdef WOLFSSL_SM4_CTR
    static const char EVP_SM4_CTR[] = "SM4-CTR";
#endif /* WOLFSSL_SM4_CTR */
#ifdef WOLFSSL_SM4_GCM
    static const char EVP_SM4_GCM[] = "SM4-GCM";
#endif /* WOLFSSL_SM4_GCM */
#ifdef WOLFSSL_SM4_CCM
    static const char EVP_SM4_CCM[] = "SM4-CCM";
#endif /* WOLFSSL_SM4_CCM */

static const char EVP_NULL[] = "NULL";

#define EVP_CIPHER_TYPE_MATCHES(x, y) (XSTRCMP(x,y) == 0)

#define EVP_PKEY_PRINT_LINE_WIDTH_MAX  80
#define EVP_PKEY_PRINT_DIGITS_PER_LINE 15

static unsigned int cipherType(const WOLFSSL_EVP_CIPHER *cipher);

static enum wc_HashType EvpMd2MacType(const WOLFSSL_EVP_MD *md);

/* Getter function for cipher key length
 *
 * c  WOLFSSL_EVP_CIPHER structure to get key length from
 *
 * NOTE: OpenSSL_add_all_ciphers() should be called first before using this
 *       function
 *
 * Returns size of key in bytes
 */
int wolfSSL_EVP_Cipher_key_length(const WOLFSSL_EVP_CIPHER* c)
{
    WOLFSSL_ENTER("wolfSSL_EVP_Cipher_key_length");

    if (c == NULL) {
        return 0;
    }

    switch (cipherType(c)) {
#if !defined(NO_AES)
  #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
      case AES_128_CBC_TYPE: return 16;
      case AES_192_CBC_TYPE: return 24;
      case AES_256_CBC_TYPE: return 32;
  #endif
  #if defined(WOLFSSL_AES_CFB)
      case AES_128_CFB1_TYPE: return 16;
      case AES_192_CFB1_TYPE: return 24;
      case AES_256_CFB1_TYPE: return 32;
      case AES_128_CFB8_TYPE: return 16;
      case AES_192_CFB8_TYPE: return 24;
      case AES_256_CFB8_TYPE: return 32;
      case AES_128_CFB128_TYPE: return 16;
      case AES_192_CFB128_TYPE: return 24;
      case AES_256_CFB128_TYPE: return 32;
  #endif
  #if defined(WOLFSSL_AES_OFB)
      case AES_128_OFB_TYPE: return 16;
      case AES_192_OFB_TYPE: return 24;
      case AES_256_OFB_TYPE: return 32;
  #endif
  #if defined(WOLFSSL_AES_XTS)
      /* Two keys for XTS. */
      case AES_128_XTS_TYPE: return 16 * 2;
      case AES_256_XTS_TYPE: return 32 * 2;
  #endif
  #if defined(HAVE_AESGCM)
      case AES_128_GCM_TYPE: return 16;
      case AES_192_GCM_TYPE: return 24;
      case AES_256_GCM_TYPE: return 32;
  #endif
  #if defined(HAVE_AESCCM)
      case AES_128_CCM_TYPE: return 16;
      case AES_192_CCM_TYPE: return 24;
      case AES_256_CCM_TYPE: return 32;
  #endif
  #if defined(WOLFSSL_AES_COUNTER)
      case AES_128_CTR_TYPE: return 16;
      case AES_192_CTR_TYPE: return 24;
      case AES_256_CTR_TYPE: return 32;
  #endif
  #if defined(HAVE_AES_ECB)
      case AES_128_ECB_TYPE: return 16;
      case AES_192_ECB_TYPE: return 24;
      case AES_256_ECB_TYPE: return 32;
  #endif
#endif /* !NO_AES */
  #ifndef NO_DES3
      case DES_CBC_TYPE:      return 8;
      case DES_EDE3_CBC_TYPE: return 24;
      case DES_ECB_TYPE:      return 8;
      case DES_EDE3_ECB_TYPE: return 24;
  #endif
  #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
      case CHACHA20_POLY1305_TYPE: return 32;
  #endif
  #ifdef HAVE_CHACHA
      case CHACHA20_TYPE: return CHACHA_MAX_KEY_SZ;
  #endif
  #ifdef WOLFSSL_SM4_ECB
      case SM4_ECB_TYPE:      return 16;
  #endif
  #ifdef WOLFSSL_SM4_CBC
      case SM4_CBC_TYPE:      return 16;
  #endif
  #ifdef WOLFSSL_SM4_CTR
      case SM4_CTR_TYPE:      return 16;
  #endif
  #ifdef WOLFSSL_SM4_GCM
      case SM4_GCM_TYPE:      return 16;
  #endif
  #ifdef WOLFSSL_SM4_CCM
      case SM4_CCM_TYPE:      return 16;
  #endif
      default:
          return 0;
      }
}


int  wolfSSL_EVP_EncryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 1);
}

int  wolfSSL_EVP_EncryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        WOLFSSL_ENGINE *impl,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    (void) impl;
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 1);
}

int  wolfSSL_EVP_DecryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    WOLFSSL_ENTER("wolfSSL_EVP_CipherInit");
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 0);
}

int  wolfSSL_EVP_DecryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        WOLFSSL_ENGINE *impl,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    (void) impl;
    WOLFSSL_ENTER("wolfSSL_EVP_DecryptInit");
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 0);
}


WOLFSSL_EVP_CIPHER_CTX *wolfSSL_EVP_CIPHER_CTX_new(void)
{
    WOLFSSL_EVP_CIPHER_CTX *ctx = (WOLFSSL_EVP_CIPHER_CTX*)XMALLOC(sizeof(*ctx),
                                                 NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (ctx) {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_new");
        wolfSSL_EVP_CIPHER_CTX_init(ctx);
    }
    return ctx;
}

void wolfSSL_EVP_CIPHER_CTX_free(WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    if (ctx) {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_free");
        wolfSSL_EVP_CIPHER_CTX_cleanup(ctx);
        XFREE(ctx, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
}

int wolfSSL_EVP_CIPHER_CTX_reset(WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    int ret = WOLFSSL_FAILURE;

    if (ctx != NULL) {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_reset");
        wolfSSL_EVP_CIPHER_CTX_cleanup(ctx);
        ret = WOLFSSL_SUCCESS;
    }

    return ret;
}

unsigned long wolfSSL_EVP_CIPHER_CTX_mode(const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
  if (ctx == NULL) return 0;
  return ctx->flags & WOLFSSL_EVP_CIPH_MODE;
}

unsigned long wolfSSL_EVP_CIPHER_CTX_flags(const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    if (ctx == NULL) return 0;
    return ctx->flags;
}

int  wolfSSL_EVP_EncryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_EncryptFinal");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else
        return WOLFSSL_FAILURE;
}


int  wolfSSL_EVP_CipherInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv,
                                    int enc)
{
    (void)impl;
    return wolfSSL_EVP_CipherInit(ctx, type, key, iv, enc);
}

int  wolfSSL_EVP_EncryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_EncryptFinal_ex");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else
        return WOLFSSL_FAILURE;
}

int  wolfSSL_EVP_DecryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && !ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_DecryptFinal");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else {
        return WOLFSSL_FAILURE;
    }
}

int  wolfSSL_EVP_DecryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && !ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_DecryptFinal_ex");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else {
        return WOLFSSL_FAILURE;
    }
}

#ifdef DEBUG_WOLFSSL_EVP
#define PRINT_BUF(b, sz) { int _i; for(_i=0; _i<(sz); _i++) { \
  printf("%02x(%c),", (b)[_i], (b)[_i]); if ((_i+1)%8==0)printf("\n");}}
#else
#define PRINT_BUF(b, sz) WC_DO_NOTHING
#endif

static int fillBuff(WOLFSSL_EVP_CIPHER_CTX *ctx, const unsigned char *in, int sz)
{
    if (sz > 0) {
        int fill;

        if ((sz+ctx->bufUsed) > ctx->block_size) {
            fill = ctx->block_size - ctx->bufUsed;
        } else {
            fill = sz;
        }
        XMEMCPY(&(ctx->buf[ctx->bufUsed]), in, (size_t)fill);
        ctx->bufUsed += fill;
        return fill;
    } else return 0;
}

static int evpCipherBlock(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out,
                                   const unsigned char *in, int inLen)
{
    int ret = 0;
    word32 inl = (word32)inLen;

    switch (ctx->cipherType) {
#if !defined(NO_AES)
    #if defined(HAVE_AES_CBC)
        case AES_128_CBC_TYPE:
        case AES_192_CBC_TYPE:
        case AES_256_CBC_TYPE:
            if (ctx->enc)
                ret = wc_AesCbcEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesCbcDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if defined(WOLFSSL_AES_COUNTER)
        case AES_128_CTR_TYPE:
        case AES_192_CTR_TYPE:
        case AES_256_CTR_TYPE:
            ret = wc_AesCtrEncrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if defined(HAVE_AES_ECB)
        case AES_128_ECB_TYPE:
        case AES_192_ECB_TYPE:
        case AES_256_ECB_TYPE:
            if (ctx->enc)
                ret = wc_AesEcbEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesEcbDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if defined(WOLFSSL_AES_OFB)
        case AES_128_OFB_TYPE:
        case AES_192_OFB_TYPE:
        case AES_256_OFB_TYPE:
            if (ctx->enc)
                ret = wc_AesOfbEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesOfbDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if defined(WOLFSSL_AES_CFB)
    #if !defined(HAVE_SELFTEST) && !defined(HAVE_FIPS)
        case AES_128_CFB1_TYPE:
        case AES_192_CFB1_TYPE:
        case AES_256_CFB1_TYPE:
            if (ctx->enc)
                ret = wc_AesCfb1Encrypt(&ctx->cipher.aes, out, in,
                        inl * WOLFSSL_BIT_SIZE);
            else
                ret = wc_AesCfb1Decrypt(&ctx->cipher.aes, out, in,
                        inl * WOLFSSL_BIT_SIZE);
            break;

        case AES_128_CFB8_TYPE:
        case AES_192_CFB8_TYPE:
        case AES_256_CFB8_TYPE:
            if (ctx->enc)
                ret = wc_AesCfb8Encrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesCfb8Decrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif /* !HAVE_SELFTEST && !HAVE_FIPS */

        case AES_128_CFB128_TYPE:
        case AES_192_CFB128_TYPE:
        case AES_256_CFB128_TYPE:
            if (ctx->enc)
                ret = wc_AesCfbEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesCfbDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
#if defined(WOLFSSL_AES_XTS)
    case AES_128_XTS_TYPE:
    case AES_256_XTS_TYPE:
        if (ctx->enc)
            ret = wc_AesXtsEncrypt(&ctx->cipher.xts, out, in, inl,
                    ctx->iv, (word32)ctx->ivSz);
        else
            ret = wc_AesXtsDecrypt(&ctx->cipher.xts, out, in, inl,
                    ctx->iv, (word32)ctx->ivSz);
        break;
#endif
#endif /* !NO_AES */
    #ifndef NO_DES3
        case DES_CBC_TYPE:
            if (ctx->enc)
                ret = wc_Des_CbcEncrypt(&ctx->cipher.des, out, in, inl);
            else
                ret = wc_Des_CbcDecrypt(&ctx->cipher.des, out, in, inl);
            break;
        case DES_EDE3_CBC_TYPE:
            if (ctx->enc)
                ret = wc_Des3_CbcEncrypt(&ctx->cipher.des3, out, in, inl);
            else
                ret = wc_Des3_CbcDecrypt(&ctx->cipher.des3, out, in, inl);
            break;
        #if defined(WOLFSSL_DES_ECB)
        case DES_ECB_TYPE:
            ret = wc_Des_EcbEncrypt(&ctx->cipher.des, out, in, inl);
            break;
        case DES_EDE3_ECB_TYPE:
            ret = wc_Des3_EcbEncrypt(&ctx->cipher.des3, out, in, inl);
            break;
        #endif
    #endif
    #ifndef NO_RC4
        case ARC4_TYPE:
            wc_Arc4Process(&ctx->cipher.arc4, out, in, inl);
            break;
    #endif
#if defined(WOLFSSL_SM4_ECB)
        case SM4_ECB_TYPE:
            if (ctx->enc)
                wc_Sm4EcbEncrypt(&ctx->cipher.sm4, out, in, inl);
            else
                wc_Sm4EcbDecrypt(&ctx->cipher.sm4, out, in, inl);
            break;
#endif
#if defined(WOLFSSL_SM4_CBC)
        case SM4_CBC_TYPE:
            if (ctx->enc)
                wc_Sm4CbcEncrypt(&ctx->cipher.sm4, out, in, inl);
            else
                wc_Sm4CbcDecrypt(&ctx->cipher.sm4, out, in, inl);
            break;
#endif
#if defined(WOLFSSL_SM4_CTR)
        case SM4_CTR_TYPE:
            wc_Sm4CtrEncrypt(&ctx->cipher.sm4, out, in, inl);
            break;
#endif

        default:
            ret = WOLFSSL_FAILURE;
    }

    (void)in;
    (void)inl;
    (void)out;

    return (ret == 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
}

#if defined(HAVE_AESGCM) || defined(WOLFSSL_SM4_GCM)
#if defined(WOLFSSL_SM4_GCM) || !defined(WOLFSSL_AESGCM_STREAM)
static int wolfSSL_EVP_CipherUpdate_GCM_AAD(WOLFSSL_EVP_CIPHER_CTX *ctx,
        const unsigned char *in, int inl) {
    if (in && inl > 0) {
        byte* tmp = (byte*)XREALLOC(ctx->authIn,
                (size_t)(ctx->authInSz + inl), NULL, DYNAMIC_TYPE_OPENSSL);
        if (tmp) {
            ctx->authIn = tmp;
            XMEMCPY(ctx->authIn + ctx->authInSz, in, (size_t)inl);
            ctx->authInSz += inl;
        }
        else {
            WOLFSSL_MSG("realloc error");
            return MEMORY_E;
        }
    }
    return 0;
}
#endif /* WOLFSSL_AESGCM_STREAM */

static int wolfSSL_EVP_CipherUpdate_GCM(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inLen)
{
    word32 inl = (word32)inLen;

#if defined(WOLFSSL_SM4_GCM) || !defined(WOLFSSL_AESGCM_STREAM)
#if defined(WOLFSSL_SM4_GCM) && defined(WOLFSSL_AESGCM_STREAM)
    if (ctx->cipherType == SM4_GCM_TYPE)
#endif
    {
        int ret = 0;

        *outl = inl;
        if (out) {
            /* Buffer input for one-shot API */
            if (inl > 0) {
                byte* tmp;
                tmp = (byte*)XREALLOC(ctx->authBuffer,
                        (size_t)(ctx->authBufferLen + inl), NULL,
                        DYNAMIC_TYPE_OPENSSL);
                if (tmp) {
                    XMEMCPY(tmp + ctx->authBufferLen, in, (size_t)inl);
                    ctx->authBufferLen += inl;
                    ctx->authBuffer = tmp;
                    *outl = 0;
                }
                else {
                    ret = MEMORY_E;
                }
            }
        }
        else {
            ret = wolfSSL_EVP_CipherUpdate_GCM_AAD(ctx, in, inl);
        }

        if (ret != 0) {
            *outl = 0;
            return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }
#endif
#if defined(WOLFSSL_SM4_GCM) && defined(WOLFSSL_AESGCM_STREAM)
    else
#endif
#if defined(WOLFSSL_AESGCM_STREAM)
    {
        int ret;

        /* When out is NULL then this is AAD. */
        if (out == NULL) {
            if (ctx->enc) {
                ret = wc_AesGcmEncryptUpdate(&ctx->cipher.aes, NULL, NULL, 0,
                    in, inl);
            }
            else {
                ret = wc_AesGcmDecryptUpdate(&ctx->cipher.aes, NULL, NULL, 0,
                    in, inl);
            }
        }
        /* When out is not NULL then this is plaintext/cipher text. */
        else {
            if (ctx->enc) {
                ret = wc_AesGcmEncryptUpdate(&ctx->cipher.aes, out, in, inl,
                    NULL, 0);
            }
            else {
                ret = wc_AesGcmDecryptUpdate(&ctx->cipher.aes, out, in, inl,
                    NULL, 0);
            }
        }
        *outl = (int)inl;
        if (ret == 0) {
            ret = WOLFSSL_SUCCESS;
        }
        else {
            ret = WOLFSSL_FAILURE;
        }
        return ret;
    }
#endif /* WOLFSSL_AESGCM_STREAM */
}
#endif /* HAVE_AESGCM || WOLFSSL_SM4_GCM */

#if defined(HAVE_AESCCM) || defined(WOLFSSL_SM4_CCM)
static int wolfSSL_EVP_CipherUpdate_CCM_AAD(WOLFSSL_EVP_CIPHER_CTX *ctx,
        const unsigned char *in, int inl) {
    if (in && inl > 0) {
        byte* tmp = (byte*)XREALLOC(ctx->authIn,
                (size_t)(ctx->authInSz + inl), NULL, DYNAMIC_TYPE_OPENSSL);
        if (tmp) {
            ctx->authIn = tmp;
            XMEMCPY(ctx->authIn + ctx->authInSz, in, (size_t)inl);
            ctx->authInSz += inl;
        }
        else {
            WOLFSSL_MSG("realloc error");
            return MEMORY_E;
        }
    }
    return 0;
}

static int wolfSSL_EVP_CipherUpdate_CCM(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl)
{
    int ret = 0;

    *outl = inl;
    if (out) {
        /* Buffer input for one-shot API */
        if (inl > 0) {
            byte* tmp;
            tmp = (byte*)XREALLOC(ctx->authBuffer,
                    (size_t)(ctx->authBufferLen + inl), NULL,
                    DYNAMIC_TYPE_OPENSSL);
            if (tmp) {
                XMEMCPY(tmp + ctx->authBufferLen, in, (size_t)inl);
                ctx->authBufferLen += inl;
                ctx->authBuffer = tmp;
                *outl = 0;
            }
            else {
                ret = MEMORY_E;
            }
        }
    }
    else {
        ret = wolfSSL_EVP_CipherUpdate_CCM_AAD(ctx, in, inl);
    }

    if (ret != 0) {
        *outl = 0;
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_AESCCM || WOLFSSL_SM4_CCM */

#if defined(HAVE_ARIA)
static int wolfSSL_EVP_CipherUpdate_AriaGCM_AAD(WOLFSSL_EVP_CIPHER_CTX *ctx,
        const unsigned char *in, int inl)
{
    if (in && inl > 0) {
        byte* tmp = (byte*)XREALLOC(ctx->authIn,
                (size_t)ctx->authInSz + inl, NULL, DYNAMIC_TYPE_OPENSSL);
        if (tmp) {
            ctx->authIn = tmp;
            XMEMCPY(ctx->authIn + ctx->authInSz, in, (size_t)inl);
            ctx->authInSz += inl;
        }
        else {
            WOLFSSL_MSG("realloc error");
            return MEMORY_E;
        }
    }
    return 0;
}

static int wolfSSL_EVP_CipherUpdate_AriaGCM(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl)
{
    int ret = 0;

    *outl = inl;
    if (out) {
        /* Buffer input for one-shot API */
        if (inl > 0) {
            byte* tmp;
            int size = ctx->authBufferLen + inl;
            if (ctx->enc == 0) { /* Append extra space for the tag */
                size = WC_ARIA_GCM_GET_CIPHERTEXT_SIZE(size);
            }
            tmp = (byte*)XREALLOC(ctx->authBuffer,
                    (size_t)size, NULL,
                    DYNAMIC_TYPE_OPENSSL);
            if (tmp) {
                XMEMCPY(tmp + ctx->authBufferLen, in, (size_t)inl);
                ctx->authBufferLen += inl;
                ctx->authBuffer = tmp;
                *outl = 0;
            }
            else {
                ret = MEMORY_E;
            }
        }
    }
    else {
        ret = wolfSSL_EVP_CipherUpdate_AriaGCM_AAD(ctx, in, inl);
    }

    if (ret != 0) {
        *outl = 0;
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_ARIA */


/* returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure */
int wolfSSL_EVP_CipherUpdate(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl)
{
    int blocks;

    WOLFSSL_ENTER("wolfSSL_EVP_CipherUpdate");
    if ((ctx == NULL) || (outl == NULL)) {
        WOLFSSL_MSG("Bad argument");
        return WOLFSSL_FAILURE;
    }

    *outl = 0;

    if ((inl == 0) && (in == NULL)) {
        /* Nothing to do in this case. Just return. */
        return WOLFSSL_SUCCESS;
    }

    if ((inl < 0) || (in == NULL)) {
        WOLFSSL_MSG("Bad argument");
        return WOLFSSL_FAILURE;
    }

    switch (ctx->cipherType) {
#if !defined(NO_AES) && defined(HAVE_AESGCM)
        case AES_128_GCM_TYPE:
        case AES_192_GCM_TYPE:
        case AES_256_GCM_TYPE:
            /* if out == NULL, in/inl contains the additional auth data */
            return wolfSSL_EVP_CipherUpdate_GCM(ctx, out, outl, in, inl);
#endif /* !defined(NO_AES) && defined(HAVE_AESGCM) */
#if !defined(NO_AES) && defined(HAVE_AESCCM)
        case AES_128_CCM_TYPE:
        case AES_192_CCM_TYPE:
        case AES_256_CCM_TYPE:
            /* if out == NULL, in/inl contains the
             * additional auth data */
            return wolfSSL_EVP_CipherUpdate_CCM(ctx, out, outl, in, inl);
#endif /* !defined(NO_AES) && defined(HAVE_AESCCM) */
#if defined(HAVE_ARIA)
        case ARIA_128_GCM_TYPE:
        case ARIA_192_GCM_TYPE:
        case ARIA_256_GCM_TYPE:
            /* if out == NULL, in/inl contains the additional auth data */
            return wolfSSL_EVP_CipherUpdate_AriaGCM(ctx, out, outl, in, inl);
#endif /* defined(HAVE_ARIA) */
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CHACHA20_POLY1305_TYPE:
            if (out == NULL) {
                if (wc_ChaCha20Poly1305_UpdateAad(&ctx->cipher.chachaPoly, in,
                                                  (word32)inl) != 0) {
                    WOLFSSL_MSG("wc_ChaCha20Poly1305_UpdateAad failed");
                    return WOLFSSL_FAILURE;
                }
                else {
                    *outl = inl;
                    return WOLFSSL_SUCCESS;
                }
            }
            else {
                if (wc_ChaCha20Poly1305_UpdateData(&ctx->cipher.chachaPoly, in,
                                                   out, (word32)inl) != 0) {
                    WOLFSSL_MSG("wc_ChaCha20Poly1305_UpdateData failed");
                    return WOLFSSL_FAILURE;
                }
                else {
                    *outl = inl;
                    return WOLFSSL_SUCCESS;
                }
            }
#endif
#ifdef HAVE_CHACHA
        case CHACHA20_TYPE:
            if (wc_Chacha_Process(&ctx->cipher.chacha, out, in, (word32)inl) !=
                    0) {
                WOLFSSL_MSG("wc_ChaCha_Process failed");
                return WOLFSSL_FAILURE;
            }
            *outl = inl;
            return WOLFSSL_SUCCESS;
#endif
#ifdef WOLFSSL_SM4_GCM
        case SM4_GCM_TYPE:
            /* if out == NULL, in/inl contains the additional auth data */
            return wolfSSL_EVP_CipherUpdate_GCM(ctx, out, outl, in, inl);
#endif
#ifdef WOLFSSL_SM4_CCM
        case SM4_CCM_TYPE:
            /* if out == NULL, in/inl contains the
             * additional auth data */
            return wolfSSL_EVP_CipherUpdate_CCM(ctx, out, outl, in, inl);
#endif
        default:
            /* fall-through */
            break;
    }

    if (out == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* if(inl == 0)wolfSSL_EVP_CipherUpdate_GCM to get tag */
    if (inl == 0) {
        return WOLFSSL_SUCCESS;
    }
    if (ctx->bufUsed > 0) { /* concatenate them if there is anything */
        int fill = fillBuff(ctx, in, inl);
        inl -= fill;
        in  += fill;
    }

    /* check if the buff is full, and if so flash it out */
    if (ctx->bufUsed == ctx->block_size) {
        byte* output = out;

        /* During decryption we save the last block to check padding on Final.
         * Update the last block stored if one has already been stored */
        if (ctx->enc == 0) {
            if (ctx->lastUsed == 1) {
                XMEMCPY(out, ctx->lastBlock, (size_t)ctx->block_size);
                *outl+= ctx->block_size;
                out  += ctx->block_size;
            }
            output = ctx->lastBlock; /* redirect output to last block buffer */
            ctx->lastUsed = 1;
        }

        PRINT_BUF(ctx->buf, ctx->block_size);
        if (evpCipherBlock(ctx, output, ctx->buf, ctx->block_size) == 0) {
            return WOLFSSL_FAILURE;
        }
        PRINT_BUF(out, ctx->block_size);
        ctx->bufUsed = 0;

        /* if doing encryption update the new output block, decryption will
         * always have the last block saved for when Final is called */
        if ((ctx->enc != 0)) {
            *outl+= ctx->block_size;
            out  += ctx->block_size;
        }
    }

    blocks = inl / ctx->block_size;
    if (blocks > 0) {
        /* During decryption we save the last block to check padding on Final.
         * Update the last block stored if one has already been stored */
        if ((ctx->enc == 0) && (ctx->lastUsed == 1)) {
            PRINT_BUF(ctx->lastBlock, ctx->block_size);
            XMEMCPY(out, ctx->lastBlock, (size_t)ctx->block_size);
            *outl += ctx->block_size;
            out += ctx->block_size;
            ctx->lastUsed = 0;
        }

        /* process blocks */
        if (evpCipherBlock(ctx, out, in, blocks * ctx->block_size) == 0) {
            return WOLFSSL_FAILURE;
        }
        PRINT_BUF(in, ctx->block_size*blocks);
        PRINT_BUF(out,ctx->block_size*blocks);
        inl  -= ctx->block_size * blocks;
        in   += ctx->block_size * blocks;
        if (ctx->enc == 0) {
            if ((ctx->flags & WOLFSSL_EVP_CIPH_NO_PADDING) ||
                    (ctx->block_size == 1)) {
                ctx->lastUsed = 0;
                *outl += ctx->block_size * blocks;
            } else {
                /* in the case of decryption and padding, store the last block
                 * here in order to verify the padding when Final is called */
                if (inl == 0) { /* if not 0 then we know leftovers are checked*/
                    ctx->lastUsed = 1;
                    blocks = blocks - 1; /* save last block to check padding in
                                          * EVP_CipherFinal call */
                    XMEMCPY(ctx->lastBlock, &out[ctx->block_size * blocks],
                            (size_t)ctx->block_size);
                }
                *outl += ctx->block_size * blocks;
            }
        } else {
            *outl += ctx->block_size * blocks;
        }
    }


    if (inl > 0) {
        /* put fraction into buff */
        fillBuff(ctx, in, inl);
        /* no increase of outl */
    }
    (void)out; /* silence warning in case not read */

    return WOLFSSL_SUCCESS;
}

static void padBlock(WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    int i;
    for (i = ctx->bufUsed; i < ctx->block_size; i++)
        ctx->buf[i] = (byte)(ctx->block_size - ctx->bufUsed);
}

static int checkPad(WOLFSSL_EVP_CIPHER_CTX *ctx, unsigned char *buff)
{
    int i;
    int n;
    n = buff[ctx->block_size-1];
    if (n > ctx->block_size) return -1;
    for (i = 0; i < n; i++) {
        if (buff[ctx->block_size-i-1] != n)
            return -1;
    }
    return ctx->block_size - n;
}

#if (defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
     defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)) && \
    ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) || FIPS_VERSION_GE(2,0))
static WC_INLINE void IncCtr(byte* ctr, word32 ctrSz)
{
    int i;
    for (i = (int)ctrSz-1; i >= 0; i--) {
        if (++ctr[i])
            break;
    }
}
#endif

int wolfSSL_EVP_CipherFinal(WOLFSSL_EVP_CIPHER_CTX *ctx, unsigned char *out,
                            int *outl)
{
    int ret = WOLFSSL_SUCCESS;

    if (!ctx || !outl)
        return WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_EVP_CipherFinal");
    switch (ctx->cipherType) {
#if defined(HAVE_AESGCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
        case AES_128_GCM_TYPE:
        case AES_192_GCM_TYPE:
        case AES_256_GCM_TYPE:
#ifndef WOLFSSL_AESGCM_STREAM
            if ((ctx->authBuffer && ctx->authBufferLen > 0)
             || (ctx->authBufferLen == 0)) {
                if (ctx->enc)
                    ret = wc_AesGcmEncrypt(&ctx->cipher.aes, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);
                else
                    ret = wc_AesGcmDecrypt(&ctx->cipher.aes, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);

                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                    *outl = ctx->authBufferLen;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                    *outl = 0;
                }

                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
                ctx->authBufferLen = 0;

                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.aes.reg, ctx->cipher.aes.nonceSz);
                    ctx->authIncIv = 0;
                }
            }
            else {
                *outl = 0;
            }
#else
            /* No data to return - all handled in Update. */
            *outl = 0;
            if (ctx->enc) {
                ret = wc_AesGcmEncryptFinal(&ctx->cipher.aes, ctx->authTag,
                    (word32)ctx->authTagSz);
            }
            else {
                ret = wc_AesGcmDecryptFinal(&ctx->cipher.aes, ctx->authTag,
                    (word32)ctx->authTagSz);
                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.aes.reg, ctx->cipher.aes.nonceSz);
                }
            }
            if (ret == 0) {
                /* Reinitialize for subsequent wolfSSL_EVP_Cipher calls. */
                if (wc_AesGcmInit(&ctx->cipher.aes, NULL, 0,
                                  (byte*)ctx->cipher.aes.reg,
                                  (word32)ctx->ivSz) != 0) {
                    WOLFSSL_MSG("wc_AesGcmInit failed");
                    ret = WOLFSSL_FAILURE;
                }
                else {
                    ret = WOLFSSL_SUCCESS;
                }
            }
            else {
                ret = WOLFSSL_FAILURE;
            }
#endif /* WOLFSSL_AESGCM_STREAM */
            if (ret == WOLFSSL_SUCCESS) {
                if (ctx->authIncIv) {
                    ctx->authIncIv = 0;
                }
                else {
                    /* Clear IV, since IV reuse is not recommended for AES GCM. */
                    XMEMSET(ctx->iv, 0, AES_BLOCK_SIZE);
                }
                if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
#endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */
#if defined(HAVE_AESCCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
        case AES_128_CCM_TYPE:
        case AES_192_CCM_TYPE:
        case AES_256_CCM_TYPE:
            if ((ctx->authBuffer && ctx->authBufferLen > 0)
             || (ctx->authBufferLen == 0)) {
                if (ctx->enc) {
                    ret = wc_AesCcmEncrypt(&ctx->cipher.aes, out,
                        ctx->authBuffer, (word32)ctx->authBufferLen,
                        ctx->iv, (word32)ctx->ivSz, ctx->authTag,
                        (word32)ctx->authTagSz, ctx->authIn,
                        (word32)ctx->authInSz);
                }
                else {
                    ret = wc_AesCcmDecrypt(&ctx->cipher.aes, out,
                        ctx->authBuffer, (word32)ctx->authBufferLen,
                        ctx->iv, (word32)ctx->ivSz, ctx->authTag,
                        (word32)ctx->authTagSz, ctx->authIn,
                        (word32)ctx->authInSz);
                }

                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                    *outl = ctx->authBufferLen;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                    *outl = 0;
                }

                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
                ctx->authBufferLen = 0;

                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.aes.reg, ctx->cipher.aes.nonceSz);
                    ctx->authIncIv = 0;
                }
            }
            else {
                *outl = 0;
            }
            if (ret == WOLFSSL_SUCCESS) {
                if (ctx->authIncIv) {
                    ctx->authIncIv = 0;
                }
                else {
                    /* Clear IV, since IV reuse is not recommended
                     * for AES CCM. */
                    XMEMSET(ctx->iv, 0, AES_BLOCK_SIZE);
                }
                if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
#endif /* HAVE_AESCCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */
#if defined(HAVE_ARIA) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
        case ARIA_128_GCM_TYPE:
        case ARIA_192_GCM_TYPE:
        case ARIA_256_GCM_TYPE:
            if ((ctx->authBuffer && ctx->authBufferLen > 0)
             || (ctx->authBufferLen == 0)) {
                if (ctx->enc)
                    ret = wc_AriaEncrypt(&ctx->cipher.aria, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authIn, ctx->authInSz,
                            ctx->authTag, ctx->authTagSz);
                else
                    ret = wc_AriaDecrypt(&ctx->cipher.aria, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authIn, ctx->authInSz,
                            ctx->authTag, ctx->authTagSz);

                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                    *outl = ctx->authBufferLen;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                    *outl = 0;
                }

                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
                ctx->authBufferLen = 0;

                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.aria.nonce,
                           ctx->cipher.aria.nonceSz);
                    ctx->authIncIv = 0;
                }
            }
            else {
                *outl = 0;
            }
            if (ret == WOLFSSL_SUCCESS) {
                if (ctx->authIncIv) {
                    ctx->authIncIv = 0;
                }
                else {
                    /* Clear IV, since IV reuse is not recommended for AES GCM. */
                    XMEMSET(ctx->iv, 0, ARIA_BLOCK_SIZE);
                }
                if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
#endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CHACHA20_POLY1305_TYPE:
            if (wc_ChaCha20Poly1305_Final(&ctx->cipher.chachaPoly,
                                          ctx->authTag) != 0) {
                WOLFSSL_MSG("wc_ChaCha20Poly1305_Final failed");
                return WOLFSSL_FAILURE;
            }
            else {
                *outl = 0;
                return WOLFSSL_SUCCESS;
            }
            break;
#endif
#ifdef WOLFSSL_SM4_GCM
        case SM4_GCM_TYPE:
            if ((ctx->authBuffer && ctx->authBufferLen > 0) ||
                     (ctx->authBufferLen == 0)) {
                if (ctx->enc)
                    ret = wc_Sm4GcmEncrypt(&ctx->cipher.sm4, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);
                else
                    ret = wc_Sm4GcmDecrypt(&ctx->cipher.sm4, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);

                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                    *outl = ctx->authBufferLen;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                    *outl = 0;
                }

                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
                ctx->authBufferLen = 0;

                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.sm4.iv, ctx->cipher.sm4.nonceSz);
                    ctx->authIncIv = 0;
                }
            }
            else {
                *outl = 0;
            }
            if (ret == WOLFSSL_SUCCESS) {
                if (ctx->authIncIv) {
                    ctx->authIncIv = 0;
                }
                else {
                    /* Clear IV, since IV reuse is not recommended for SM4 GCM.
                     */
                    XMEMSET(ctx->iv, 0, SM4_BLOCK_SIZE);
                }
                if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
#endif
#ifdef WOLFSSL_SM4_CCM
        case SM4_CCM_TYPE:
            if ((ctx->authBuffer && ctx->authBufferLen > 0) ||
                    (ctx->authBufferLen == 0)) {
                if (ctx->enc)
                    ret = wc_Sm4CcmEncrypt(&ctx->cipher.sm4, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);
                else
                    ret = wc_Sm4CcmDecrypt(&ctx->cipher.sm4, out,
                            ctx->authBuffer, ctx->authBufferLen,
                            ctx->iv, ctx->ivSz, ctx->authTag, ctx->authTagSz,
                            ctx->authIn, ctx->authInSz);

                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                    *outl = ctx->authBufferLen;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                    *outl = 0;
                }

                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
                ctx->authBufferLen = 0;

                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.sm4.iv, ctx->cipher.sm4.nonceSz);
                    ctx->authIncIv = 0;
                }
            }
            else {
                *outl = 0;
            }
            if (ret == WOLFSSL_SUCCESS) {
                if (ctx->authIncIv) {
                    ctx->authIncIv = 0;
                }
                else {
                    /* Clear IV, since IV reuse is not recommended
                     * for SM4 CCM. */
                    XMEMSET(ctx->iv, 0, SM4_BLOCK_SIZE);
                }
                if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
#endif
        default:
            if (!out)
                return WOLFSSL_FAILURE;

            if (ctx->flags & WOLFSSL_EVP_CIPH_NO_PADDING) {
                if (ctx->bufUsed != 0) return WOLFSSL_FAILURE;
                *outl = 0;
            }
            else if (ctx->enc) {
                if (ctx->block_size == 1) {
                    *outl = 0;
                }
                else if ((ctx->bufUsed >= 0) && (ctx->block_size != 1)) {
                    padBlock(ctx);
                    PRINT_BUF(ctx->buf, ctx->block_size);
                    if (evpCipherBlock(ctx, out, ctx->buf, ctx->block_size) == 0) {
                        WOLFSSL_MSG("Final Cipher Block failed");
                        ret = WOLFSSL_FAILURE;
                    }
                    else {
                        PRINT_BUF(out, ctx->block_size);
                        *outl = ctx->block_size;
                    }
                }
            }
            else {
                if (ctx->block_size == 1) {
                    *outl = 0;
                }
                else if ((ctx->bufUsed % ctx->block_size) != 0) {
                    *outl = 0;
                    /* not enough padding for decrypt */
                    WOLFSSL_MSG("Final Cipher Block not enough padding");
                    ret = WOLFSSL_FAILURE;
                }
                else if (ctx->lastUsed) {
                    int fl;
                    PRINT_BUF(ctx->lastBlock, ctx->block_size);
                    if ((fl = checkPad(ctx, ctx->lastBlock)) >= 0) {
                        XMEMCPY(out, ctx->lastBlock, (size_t)fl);
                        *outl = fl;
                        if (ctx->lastUsed == 0 && ctx->bufUsed == 0) {
                            /* return error in cases where the block length is
                             * incorrect */
                            WOLFSSL_MSG("Final Cipher Block bad length");
                            ret = WOLFSSL_FAILURE;
                        }
                    }
                    else {
                        ret = WOLFSSL_FAILURE;
                    }
                }
                else if (ctx->lastUsed == 0 && ctx->bufUsed == 0) {
                    /* return error in cases where the block length is
                     * incorrect */
                    ret = WOLFSSL_FAILURE;
                }
            }
            break;
    }

    if (ret == WOLFSSL_SUCCESS) {
#if (defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
     defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)) && \
        ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
            || FIPS_VERSION_GE(2,0))
        byte tmp = 0;

        /*
         * This flag needs to retain its value between wolfSSL_EVP_CipherFinal
         * calls. wolfSSL_EVP_CipherInit will clear it, so we save and restore
         * it here.
         */
        if (FALSE
        #ifdef HAVE_AESGCM
            || ctx->cipherType == AES_128_GCM_TYPE ||
            ctx->cipherType == AES_192_GCM_TYPE ||
            ctx->cipherType == AES_256_GCM_TYPE
        #endif
        #ifdef HAVE_AESCCM
            || ctx->cipherType == AES_128_CCM_TYPE ||
            ctx->cipherType == AES_192_CCM_TYPE ||
            ctx->cipherType == AES_256_CCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_GCM
            || ctx->cipherType == SM4_GCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_CCM
            || ctx->cipherType == SM4_CCM_TYPE
        #endif
            ) {
            tmp = ctx->authIvGenEnable;
        }
#endif

        /* reset cipher state after final */
        ret = wolfSSL_EVP_CipherInit(ctx, NULL, NULL, NULL, -1);

#if (defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
     defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)) && \
    ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) || FIPS_VERSION_GE(2,0))
        if (FALSE
        #ifdef HAVE_AESGCM
            || ctx->cipherType == AES_128_GCM_TYPE ||
            ctx->cipherType == AES_192_GCM_TYPE ||
            ctx->cipherType == AES_256_GCM_TYPE
        #endif
        #ifdef HAVE_AESCCM
            || ctx->cipherType == AES_128_CCM_TYPE ||
            ctx->cipherType == AES_192_CCM_TYPE ||
            ctx->cipherType == AES_256_CCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_GCM
            || ctx->cipherType == SM4_GCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_CCM
            || ctx->cipherType == SM4_CCM_TYPE
        #endif
            ) {
            ctx->authIvGenEnable = (tmp == 1);
        }
#endif
    }

    return ret;
}


#ifdef WOLFSSL_EVP_DECRYPT_LEGACY
/* This is a version of DecryptFinal to work with data encrypted with
 * wolfSSL_EVP_EncryptFinal() with the broken padding. (pre-v3.12.0)
 * Only call this after wolfSSL_EVP_CipherFinal() fails on a decrypt.
 * Note, you don't know if the padding is good or bad with the old
 * encrypt, but it is likely to be or bad. It will update the output
 * length with the block_size so the last block is still captured. */
int  wolfSSL_EVP_DecryptFinal_legacy(WOLFSSL_EVP_CIPHER_CTX *ctx,
        unsigned char *out, int *outl)
{
    int fl;
    if (ctx == NULL || out == NULL || outl == NULL)
        return BAD_FUNC_ARG;

    WOLFSSL_ENTER("wolfSSL_EVP_DecryptFinal_legacy");
    if (ctx->block_size == 1) {
        *outl = 0;
        return WOLFSSL_SUCCESS;
    }
    if ((ctx->bufUsed % ctx->block_size) != 0) {
        *outl = 0;
        /* not enough padding for decrypt */
        return WOLFSSL_FAILURE;
    }
    /* The original behavior of CipherFinal() was like it is now,
     * but checkPad would return 0 in case of a bad pad. It would
     * treat the pad as 0, and leave the data in the output buffer,
     * and not try to copy anything. This converts checkPad's -1 error
     * code to block_size.
     */
    if (ctx->lastUsed) {
        PRINT_BUF(ctx->lastBlock, ctx->block_size);
        if ((fl = checkPad(ctx, ctx->lastBlock)) < 0) {
            fl = ctx->block_size;
        }
        else {
            XMEMCPY(out, ctx->lastBlock, (size_t)fl);
        }
        *outl = fl;
    }
    /* return error in cases where the block length is incorrect */
    if (ctx->lastUsed == 0 && ctx->bufUsed == 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif

int wolfSSL_EVP_CIPHER_CTX_block_size(const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    if (ctx == NULL) return BAD_FUNC_ARG;
    switch (ctx->cipherType) {
#if !defined(NO_AES) || !defined(NO_DES3) || defined(WOLFSSL_SM4)
#if !defined(NO_AES)
#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    case AES_128_CBC_TYPE:
    case AES_192_CBC_TYPE:
    case AES_256_CBC_TYPE:
#endif
#if defined(HAVE_AESGCM)
    case AES_128_GCM_TYPE:
    case AES_192_GCM_TYPE:
    case AES_256_GCM_TYPE:
#endif
#if defined(HAVE_AESCCM)
    case AES_128_CCM_TYPE:
    case AES_192_CCM_TYPE:
    case AES_256_CCM_TYPE:
#endif
#if defined(WOLFSSL_AES_COUNTER)
    case AES_128_CTR_TYPE:
    case AES_192_CTR_TYPE:
    case AES_256_CTR_TYPE:
#endif
#if defined(WOLFSSL_AES_CFB)
    case AES_128_CFB1_TYPE:
    case AES_192_CFB1_TYPE:
    case AES_256_CFB1_TYPE:
    case AES_128_CFB8_TYPE:
    case AES_192_CFB8_TYPE:
    case AES_256_CFB8_TYPE:
    case AES_128_CFB128_TYPE:
    case AES_192_CFB128_TYPE:
    case AES_256_CFB128_TYPE:
#endif
#if defined(WOLFSSL_AES_OFB)
    case AES_128_OFB_TYPE:
    case AES_192_OFB_TYPE:
    case AES_256_OFB_TYPE:
#endif
#if defined(WOLFSSL_AES_XTS)
    case AES_128_XTS_TYPE:
    case AES_256_XTS_TYPE:
#endif
#if defined(HAVE_ARIA)
    case ARIA_128_GCM_TYPE:
    case ARIA_192_GCM_TYPE:
    case ARIA_256_GCM_TYPE:
#endif

    case AES_128_ECB_TYPE:
    case AES_192_ECB_TYPE:
    case AES_256_ECB_TYPE:
#endif /* !NO_AES */
#ifndef NO_DES3
    case DES_CBC_TYPE:
    case DES_ECB_TYPE:
    case DES_EDE3_CBC_TYPE:
    case DES_EDE3_ECB_TYPE:
#endif
#ifdef WOLFSSL_SM4_ECB
    case SM4_ECB_TYPE:
#endif
#ifdef WOLFSSL_SM4_CBC
    case SM4_CBC_TYPE:
#endif
#ifdef WOLFSSL_SM4_CTR
    case SM4_CTR_TYPE:
#endif
#ifdef WOLFSSL_SM4_GCM
    case SM4_GCM_TYPE:
#endif
#ifdef WOLFSSL_SM4_CCM
    case SM4_CCM_TYPE:
#endif
        return ctx->block_size;
#endif /* !NO_AES || !NO_DES3 || WOLFSSL_SM4 */
    default:
        return 0;
    }
}

static unsigned int cipherType(const WOLFSSL_EVP_CIPHER *cipher)
{
    if (cipher == NULL) return 0; /* dummy for #ifdef */
#ifndef NO_DES3
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_DES_CBC))
        return DES_CBC_TYPE;
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_DES_EDE3_CBC))
        return DES_EDE3_CBC_TYPE;
#if !defined(NO_DES3)
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_DES_ECB))
        return DES_ECB_TYPE;
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_DES_EDE3_ECB))
        return DES_EDE3_ECB_TYPE;
#endif /* NO_DES3 && HAVE_AES_ECB */
#endif
#if !defined(NO_AES)
#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CBC))
        return AES_128_CBC_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CBC))
        return AES_192_CBC_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CBC))
        return AES_256_CBC_TYPE;
    #endif
#endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */
#if defined(HAVE_AESGCM)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_GCM))
        return AES_128_GCM_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_GCM))
        return AES_192_GCM_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_GCM))
        return AES_256_GCM_TYPE;
    #endif
#endif /* HAVE_AESGCM */
#if defined(HAVE_AESCCM)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CCM))
        return AES_128_CCM_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CCM))
        return AES_192_CCM_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CCM))
        return AES_256_CCM_TYPE;
    #endif
#endif /* HAVE_AESCCM */
#if defined(WOLFSSL_AES_COUNTER)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CTR))
        return AES_128_CTR_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CTR))
        return AES_192_CTR_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CTR))
        return AES_256_CTR_TYPE;
    #endif
#endif /* HAVE_AES_CBC */
#if defined(HAVE_AES_ECB)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_ECB))
        return AES_128_ECB_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_ECB))
        return AES_192_ECB_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_ECB))
        return AES_256_ECB_TYPE;
    #endif
#endif /*HAVE_AES_CBC */
#if defined(WOLFSSL_AES_XTS)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_XTS))
        return AES_128_XTS_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_XTS))
        return AES_256_XTS_TYPE;
    #endif
#endif /* WOLFSSL_AES_XTS */
#if defined(WOLFSSL_AES_CFB)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CFB1))
        return AES_128_CFB1_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CFB1))
        return AES_192_CFB1_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CFB1))
        return AES_256_CFB1_TYPE;
    #endif
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CFB8))
        return AES_128_CFB8_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CFB8))
        return AES_192_CFB8_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CFB8))
        return AES_256_CFB8_TYPE;
    #endif
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_CFB128))
        return AES_128_CFB128_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_CFB128))
        return AES_192_CFB128_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_CFB128))
        return AES_256_CFB128_TYPE;
    #endif
#endif /*HAVE_AES_CBC */
#if defined(WOLFSSL_AES_OFB)
    #ifdef WOLFSSL_AES_128
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_128_OFB))
      return AES_128_OFB_TYPE;
    #endif
    #ifdef WOLFSSL_AES_192
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_192_OFB))
      return AES_192_OFB_TYPE;
    #endif
    #ifdef WOLFSSL_AES_256
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_AES_256_OFB))
      return AES_256_OFB_TYPE;
    #endif
#endif
#endif /* !NO_AES */
#if defined(HAVE_ARIA)
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_ARIA_128_GCM))
        return ARIA_128_GCM_TYPE;
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_ARIA_192_GCM))
        return ARIA_192_GCM_TYPE;
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_ARIA_256_GCM))
        return ARIA_256_GCM_TYPE;
#endif /* HAVE_ARIA */

#ifndef NO_RC4
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_ARC4))
      return ARC4_TYPE;
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_CHACHA20_POLY1305))
        return CHACHA20_POLY1305_TYPE;
#endif

#ifdef HAVE_CHACHA
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_CHACHA20))
        return CHACHA20_TYPE;
#endif

#ifdef WOLFSSL_SM4_ECB
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_SM4_ECB))
        return SM4_ECB_TYPE;
#endif
#ifdef WOLFSSL_SM4_CBC
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_SM4_CBC))
        return SM4_CBC_TYPE;
#endif
#ifdef WOLFSSL_SM4_CTR
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_SM4_CTR))
        return SM4_CTR_TYPE;
#endif
#ifdef WOLFSSL_SM4_GCM
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_SM4_GCM))
        return SM4_GCM_TYPE;
#endif
#ifdef WOLFSSL_SM4_CCM
    else if (EVP_CIPHER_TYPE_MATCHES(cipher, EVP_SM4_CCM))
        return SM4_CCM_TYPE;
#endif

      else return 0;
}

int wolfSSL_EVP_CIPHER_block_size(const WOLFSSL_EVP_CIPHER *cipher)
{
    if (cipher == NULL)
        return BAD_FUNC_ARG;

    switch (cipherType(cipher)) {
#if !defined(NO_AES)
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
        case AES_128_CBC_TYPE:
        case AES_192_CBC_TYPE:
        case AES_256_CBC_TYPE:
            return AES_BLOCK_SIZE;
    #endif
    #if defined(HAVE_AESGCM)
        case AES_128_GCM_TYPE:
        case AES_192_GCM_TYPE:
        case AES_256_GCM_TYPE:
            return 1;
    #endif
    #if defined(HAVE_AESCCM)
        case AES_128_CCM_TYPE:
        case AES_192_CCM_TYPE:
        case AES_256_CCM_TYPE:
            return 1;
    #endif
    #if defined(WOLFSSL_AES_COUNTER)
        case AES_128_CTR_TYPE:
        case AES_192_CTR_TYPE:
        case AES_256_CTR_TYPE:
            return 1;
    #endif
    #if defined(HAVE_AES_ECB)
        case AES_128_ECB_TYPE:
        case AES_192_ECB_TYPE:
        case AES_256_ECB_TYPE:
            return AES_BLOCK_SIZE;
    #endif
    #if defined(WOLFSSL_AES_CFB)
        case AES_128_CFB1_TYPE:
        case AES_192_CFB1_TYPE:
        case AES_256_CFB1_TYPE:
        case AES_128_CFB8_TYPE:
        case AES_192_CFB8_TYPE:
        case AES_256_CFB8_TYPE:
        case AES_128_CFB128_TYPE:
        case AES_192_CFB128_TYPE:
        case AES_256_CFB128_TYPE:
            return 1;
    #endif
    #if defined(WOLFSSL_AES_OFB)
        case AES_128_OFB_TYPE:
        case AES_192_OFB_TYPE:
        case AES_256_OFB_TYPE:
            return 1;
    #endif
    #if defined(WOLFSSL_AES_XTS)
        case AES_128_XTS_TYPE:
        case AES_256_XTS_TYPE:
            return 1;
    #endif
  #endif /* NO_AES */

  #ifndef NO_RC4
        case ARC4_TYPE:
            return 1;
  #endif
#if defined(HAVE_ARIA)
    case ARIA_128_GCM_TYPE:
    case ARIA_192_GCM_TYPE:
    case ARIA_256_GCM_TYPE:
        return 1;
#endif

#ifndef NO_DES3
        case DES_CBC_TYPE: return 8;
        case DES_EDE3_CBC_TYPE: return 8;
        case DES_ECB_TYPE: return 8;
        case DES_EDE3_ECB_TYPE: return 8;
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CHACHA20_POLY1305_TYPE:
            return 1;
#endif

#ifdef HAVE_CHACHA
        case CHACHA20_TYPE:
            return 1;
#endif

#ifdef WOLFSSL_SM4_ECB
       case SM4_ECB_TYPE:
            return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_CBC
       case SM4_CBC_TYPE:
            return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_CTR
       case SM4_CTR_TYPE:
            return 1;
#endif
#ifdef WOLFSSL_SM4_GCM
       case SM4_GCM_TYPE:
            return 1;
#endif
#ifdef WOLFSSL_SM4_CCM
       case SM4_CCM_TYPE:
            return 1;
#endif

        default:
            return 0;
    }
}

unsigned long WOLFSSL_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher)
{
    switch (cipherType(cipher)) {
#if !defined(NO_AES)
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
        case AES_128_CBC_TYPE:
        case AES_192_CBC_TYPE:
        case AES_256_CBC_TYPE:
            return WOLFSSL_EVP_CIPH_CBC_MODE;
    #endif
    #if defined(HAVE_AESGCM)
        case AES_128_GCM_TYPE:
        case AES_192_GCM_TYPE:
        case AES_256_GCM_TYPE:
            return WOLFSSL_EVP_CIPH_GCM_MODE |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
    #if defined(HAVE_AESCCM)
        case AES_128_CCM_TYPE:
        case AES_192_CCM_TYPE:
        case AES_256_CCM_TYPE:
            return WOLFSSL_EVP_CIPH_CCM_MODE |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
    #if defined(WOLFSSL_AES_COUNTER)
        case AES_128_CTR_TYPE:
        case AES_192_CTR_TYPE:
        case AES_256_CTR_TYPE:
            return WOLFSSL_EVP_CIPH_CTR_MODE;
    #endif
    #if defined(WOLFSSL_AES_CFB)
        case AES_128_CFB1_TYPE:
        case AES_192_CFB1_TYPE:
        case AES_256_CFB1_TYPE:
        case AES_128_CFB8_TYPE:
        case AES_192_CFB8_TYPE:
        case AES_256_CFB8_TYPE:
        case AES_128_CFB128_TYPE:
        case AES_192_CFB128_TYPE:
        case AES_256_CFB128_TYPE:
            return WOLFSSL_EVP_CIPH_CFB_MODE;
    #endif
    #if defined(WOLFSSL_AES_OFB)
        case AES_128_OFB_TYPE:
        case AES_192_OFB_TYPE:
        case AES_256_OFB_TYPE:
            return WOLFSSL_EVP_CIPH_OFB_MODE;
    #endif
    #if defined(WOLFSSL_AES_XTS)
        case AES_128_XTS_TYPE:
        case AES_256_XTS_TYPE:
            return WOLFSSL_EVP_CIPH_XTS_MODE;
    #endif
        case AES_128_ECB_TYPE:
        case AES_192_ECB_TYPE:
        case AES_256_ECB_TYPE:
            return WOLFSSL_EVP_CIPH_ECB_MODE;
#endif /* !NO_AES */
    #if defined(HAVE_ARIA)
        case ARIA_128_GCM_TYPE:
        case ARIA_192_GCM_TYPE:
        case ARIA_256_GCM_TYPE:
            return WOLFSSL_EVP_CIPH_GCM_MODE |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
    #ifndef NO_DES3
        case DES_CBC_TYPE:
        case DES_EDE3_CBC_TYPE:
            return WOLFSSL_EVP_CIPH_CBC_MODE;
        case DES_ECB_TYPE:
        case DES_EDE3_ECB_TYPE:
            return WOLFSSL_EVP_CIPH_ECB_MODE;
    #endif
    #ifndef NO_RC4
        case ARC4_TYPE:
            return EVP_CIPH_STREAM_CIPHER;
    #endif
    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CHACHA20_POLY1305_TYPE:
            return WOLFSSL_EVP_CIPH_STREAM_CIPHER |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
    #ifdef HAVE_CHACHA
        case CHACHA20_TYPE:
            return WOLFSSL_EVP_CIPH_STREAM_CIPHER;
    #endif
    #ifdef WOLFSSL_SM4_ECB
        case SM4_ECB_TYPE:
            return WOLFSSL_EVP_CIPH_ECB_MODE;
    #endif
    #ifdef WOLFSSL_SM4_CBC
        case SM4_CBC_TYPE:
            return WOLFSSL_EVP_CIPH_CBC_MODE;
    #endif
    #ifdef WOLFSSL_SM4_CTR
        case SM4_CTR_TYPE:
            return WOLFSSL_EVP_CIPH_CTR_MODE;
    #endif
    #ifdef WOLFSSL_SM4_GCM
        case SM4_GCM_TYPE:
            return WOLFSSL_EVP_CIPH_GCM_MODE |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
    #ifdef WOLFSSL_SM4_CCM
        case SM4_CCM_TYPE:
            return WOLFSSL_EVP_CIPH_CCM_MODE |
                    WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
    #endif
        default:
            return 0;
    }
}

unsigned long WOLFSSL_EVP_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher)
{
    if (cipher == NULL)
        return 0;
    return WOLFSSL_CIPHER_mode(cipher) & WOLFSSL_EVP_CIPH_MODE;
}

void wolfSSL_EVP_CIPHER_CTX_set_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags)
{
    if (ctx != NULL) {
        ctx->flags |= (unsigned long)flags;
    }
}

void wolfSSL_EVP_CIPHER_CTX_clear_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags)
{
    if (ctx != NULL) {
        ctx->flags &= (unsigned long)~flags;
    }
}

unsigned long wolfSSL_EVP_CIPHER_flags(const WOLFSSL_EVP_CIPHER *cipher)
{
    if (cipher == NULL)
        return 0;
    return WOLFSSL_CIPHER_mode(cipher);
}

int  wolfSSL_EVP_CIPHER_CTX_set_padding(WOLFSSL_EVP_CIPHER_CTX *ctx,
    int padding)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;
    if (padding) {
        ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_NO_PADDING;
    }
    else {
        ctx->flags |=  WOLFSSL_EVP_CIPH_NO_PADDING;
    }
    return 1;
}

int wolfSSL_EVP_add_digest(const WOLFSSL_EVP_MD *digest)
{
    (void)digest;
    /* nothing to do */
    return 0;
}


/* Frees the WOLFSSL_EVP_PKEY_CTX passed in.
 *
 * return WOLFSSL_SUCCESS on success
 */
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
void wolfSSL_EVP_PKEY_CTX_free(WOLFSSL_EVP_PKEY_CTX *ctx)
#else
int wolfSSL_EVP_PKEY_CTX_free(WOLFSSL_EVP_PKEY_CTX *ctx)
#endif
{
    if (ctx == NULL)
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
        return;
#else
        return 0;
#endif
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_free");
    if (ctx->pkey != NULL)
        wolfSSL_EVP_PKEY_free(ctx->pkey);
    if (ctx->peerKey != NULL)
        wolfSSL_EVP_PKEY_free(ctx->peerKey);
    XFREE(ctx, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
#if !defined(OPENSSL_VERSION_NUMBER) || OPENSSL_VERSION_NUMBER < 0x10100000L
    return WOLFSSL_SUCCESS;
#endif
}


/* Creates a new WOLFSSL_EVP_PKEY_CTX structure.
 *
 * pkey  key structure to use with new WOLFSSL_EVP_PKEY_CTX
 * e     engine to use. It should be NULL at this time.
 *
 * return the new structure on success and NULL if failed.
 */
WOLFSSL_EVP_PKEY_CTX *wolfSSL_EVP_PKEY_CTX_new(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_ENGINE *e)
{
    WOLFSSL_EVP_PKEY_CTX* ctx;

    if (pkey == NULL) return 0;
    if (e != NULL) return 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_new");

    ctx = (WOLFSSL_EVP_PKEY_CTX*)XMALLOC(sizeof(WOLFSSL_EVP_PKEY_CTX), NULL,
            DYNAMIC_TYPE_PUBLIC_KEY);
    if (ctx == NULL) return NULL;
    XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_PKEY_CTX));
    ctx->pkey = pkey;
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    ctx->padding = RSA_PKCS1_PADDING;
    ctx->md = NULL;
#endif
#ifdef HAVE_ECC
    if (pkey->ecc && pkey->ecc->group) {
        /* set curve NID from pkey if available */
        ctx->curveNID = pkey->ecc->group->curve_nid;
    }
    else {
        ctx->curveNID = ECC_CURVE_DEF;
    }
#endif
    if (wolfSSL_EVP_PKEY_up_ref(pkey) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("Couldn't increase key reference count");
    }
    return ctx;
}


/* Sets the type of RSA padding to use.
 *
 * ctx     structure to set padding in.
 * padding RSA padding type
 *
 * returns WOLFSSL_SUCCESS on success.
 */
int wolfSSL_EVP_PKEY_CTX_set_rsa_padding(WOLFSSL_EVP_PKEY_CTX *ctx, int padding)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set_rsa_padding");
    ctx->padding = padding;
    return WOLFSSL_SUCCESS;
}

/* Sets the message digest type for RSA padding to use.
 *
 * ctx  structure to set padding in.
 * md   Message digest
 *
 * returns WOLFSSL_SUCCESS on success.
 */
int wolfSSL_EVP_PKEY_CTX_set_signature_md(WOLFSSL_EVP_PKEY_CTX *ctx,
    const EVP_MD* md)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set_signature_md");
#ifndef NO_RSA
    ctx->md = md;
#else
    (void)md;
#endif
    return WOLFSSL_SUCCESS;
}

/* create a PKEY context and return it */
WOLFSSL_EVP_PKEY_CTX *wolfSSL_EVP_PKEY_CTX_new_id(int id, WOLFSSL_ENGINE *e)
{
    WOLFSSL_EVP_PKEY* pkey;
    WOLFSSL_EVP_PKEY_CTX* ctx = NULL;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_new_id");

    pkey = wolfSSL_EVP_PKEY_new_ex(NULL);
    if (pkey) {
        pkey->type = id;
        ctx = wolfSSL_EVP_PKEY_CTX_new(pkey, e);
        /* wolfSSL_EVP_PKEY_CTX_new calls wolfSSL_EVP_PKEY_up_ref so we need
         * to always call wolfSSL_EVP_PKEY_free (either to free it if an
         * error occurred in the previous function or to decrease the reference
         * count so that pkey is actually free'd when wolfSSL_EVP_PKEY_CTX_free
         * is called) */
        wolfSSL_EVP_PKEY_free(pkey);
    }
    return ctx;
}

/* Returns WOLFSSL_SUCCESS or error */
int wolfSSL_EVP_PKEY_CTX_set_rsa_keygen_bits(WOLFSSL_EVP_PKEY_CTX *ctx, int bits)
{
    if (ctx) {
        ctx->nbits = bits;
    }
    return WOLFSSL_SUCCESS;
}


int wolfSSL_EVP_PKEY_derive_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_derive_init");

    if (!ctx) {
        return WOLFSSL_FAILURE;
    }
    wolfSSL_EVP_PKEY_free(ctx->peerKey);
    ctx->op = EVP_PKEY_OP_DERIVE;
    ctx->padding = 0;
    ctx->nbits = 0;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_EVP_PKEY_derive_set_peer(WOLFSSL_EVP_PKEY_CTX *ctx, WOLFSSL_EVP_PKEY *peer)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_derive_set_peer");

    if (!ctx || ctx->op != EVP_PKEY_OP_DERIVE) {
        return WOLFSSL_FAILURE;
    }
    wolfSSL_EVP_PKEY_free(ctx->peerKey);
    ctx->peerKey = peer;
    if (!wolfSSL_EVP_PKEY_up_ref(peer)) {
        ctx->peerKey = NULL;
        return WOLFSSL_FAILURE;
    }
    return WOLFSSL_SUCCESS;
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_EVP_PKEY_CTX_ctrl_str(WOLFSSL_EVP_PKEY_CTX *ctx,
                          const char *name, const char *value)
{
    WOLFSSL_STUB("wolfSSL_EVP_PKEY_CTX_ctrl_str");
    (void)ctx;
    (void)name;
    (void)value;
    return WOLFSSL_FAILURE;
}
#endif /* NO_WOLFSSL_STUB */

#if (!defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)) || defined(HAVE_ECC) || \
    defined(HAVE_HKDF)
int wolfSSL_EVP_PKEY_derive(WOLFSSL_EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen)
{
    int len;
#ifdef HAVE_HKDF
    enum wc_HashType hkdfHashType;
#endif

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_derive");

    if (!ctx || ctx->op != EVP_PKEY_OP_DERIVE || !ctx->pkey || (!ctx->peerKey
        && ctx->pkey->type != EVP_PKEY_HKDF) || !keylen || (ctx->pkey->type
        != EVP_PKEY_HKDF && ctx->pkey->type != ctx->peerKey->type)) {
        return WOLFSSL_FAILURE;
    }
    switch (ctx->pkey->type) {
#ifndef NO_DH
    case EVP_PKEY_DH:
        /* Use DH */
        if (!ctx->pkey->dh || !ctx->peerKey->dh) {
            return WOLFSSL_FAILURE;
        }
        /* set internal peer key if not done */
        if (!ctx->peerKey->dh->inSet) {
            if (SetDhInternal(ctx->peerKey->dh) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("SetDhInternal failed");
                return WOLFSSL_FAILURE;
            }
        }
        if (!ctx->peerKey->dh->pub_key) {
            WOLFSSL_MSG("SetDhInternal failed, pub_key is NULL");
            return WOLFSSL_FAILURE;
        }
        if ((len = wolfSSL_DH_size(ctx->pkey->dh)) <= 0) {
            return WOLFSSL_FAILURE;
        }
        if (key) {
            if (*keylen < (size_t)len) {
                return WOLFSSL_FAILURE;
            }
            /* computed DH agreement can be less than DH size if leading zeros */
            if (wolfSSL_DH_compute_key(key, ctx->peerKey->dh->pub_key,
                                       ctx->pkey->dh) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }
        *keylen = (size_t)len;
        break;
#endif
#if defined(HAVE_ECC) && !defined(WOLF_CRYPTO_CB_ONLY_ECC)
    case EVP_PKEY_EC:
        /* Use ECDH */
        if (!ctx->pkey->ecc || !ctx->peerKey->ecc) {
            return WOLFSSL_FAILURE;
        }
        /* set internal key if not done */
        if (!ctx->pkey->ecc->inSet) {
            if (SetECKeyInternal(ctx->pkey->ecc) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("SetECKeyInternal failed");
                return WOLFSSL_FAILURE;
            }
        }
        if (!ctx->peerKey->ecc->exSet || !ctx->peerKey->ecc->pub_key->internal) {
            if (SetECKeyExternal(ctx->peerKey->ecc) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("SetECKeyExternal failed");
                return WOLFSSL_FAILURE;
            }
        }
        if (!(len = wc_ecc_size((ecc_key*)ctx->pkey->ecc->internal))) {
            return WOLFSSL_FAILURE;
        }
        if (key) {
            word32 len32 = (word32)len;
#if defined(ECC_TIMING_RESISTANT) && !defined(HAVE_SELFTEST) \
    && (!defined(HAVE_FIPS) || \
         (defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION > 2))

            WC_RNG rng;
            if (wc_InitRng(&rng) != MP_OKAY) {
                WOLFSSL_MSG("Init RNG failed");
                return WOLFSSL_FAILURE;
            }
            ((ecc_key*)ctx->pkey->ecc->internal)->rng = &rng;
#endif
            if (*keylen < len32) {
                WOLFSSL_MSG("buffer too short");
#if defined(ECC_TIMING_RESISTANT) && !defined(HAVE_SELFTEST) \
    && (!defined(HAVE_FIPS) || \
         (defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION > 2))
                ((ecc_key*)ctx->pkey->ecc->internal)->rng = NULL;
                wc_FreeRng(&rng);
#endif
                return WOLFSSL_FAILURE;
            }
            if (wc_ecc_shared_secret_ssh((ecc_key*)ctx->pkey->ecc->internal,
                                         (ecc_point*)ctx->peerKey->ecc->pub_key->internal,
                                         key, &len32) != MP_OKAY) {
                WOLFSSL_MSG("wc_ecc_shared_secret failed");
#if defined(ECC_TIMING_RESISTANT) && !defined(HAVE_SELFTEST) \
    && (!defined(HAVE_FIPS) || \
         (defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION > 2))
                ((ecc_key*)ctx->pkey->ecc->internal)->rng = NULL;
                wc_FreeRng(&rng);
#endif
                return WOLFSSL_FAILURE;
            }
#if defined(ECC_TIMING_RESISTANT) && !defined(HAVE_SELFTEST) \
    && (!defined(HAVE_FIPS) || \
         (defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION > 2))
            ((ecc_key*)ctx->pkey->ecc->internal)->rng = NULL;
            wc_FreeRng(&rng);
#endif
            len = (int)len32;
        }
        *keylen = (size_t)len;
        break;
#endif
#ifdef HAVE_HKDF
    case EVP_PKEY_HKDF:
        (void)len;

        hkdfHashType = EvpMd2MacType(ctx->pkey->hkdfMd);
        if (hkdfHashType == WC_HASH_TYPE_NONE) {
            WOLFSSL_MSG("Invalid hash type for HKDF.");
            return WOLFSSL_FAILURE;
        }
        if (ctx->pkey->hkdfMode == EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND) {
            if (wc_HKDF(hkdfHashType, ctx->pkey->hkdfKey, ctx->pkey->hkdfKeySz,
                        ctx->pkey->hkdfSalt, ctx->pkey->hkdfSaltSz,
                        ctx->pkey->hkdfInfo, ctx->pkey->hkdfInfoSz, key,
                        (word32)*keylen) != 0) {
                WOLFSSL_MSG("wc_HKDF failed.");
                return WOLFSSL_FAILURE;
            }
        }
        else if (ctx->pkey->hkdfMode == EVP_PKEY_HKDEF_MODE_EXTRACT_ONLY) {
            if (wc_HKDF_Extract(hkdfHashType, ctx->pkey->hkdfSalt,
                                ctx->pkey->hkdfSaltSz, ctx->pkey->hkdfKey,
                                ctx->pkey->hkdfKeySz, key) != 0) {
                WOLFSSL_MSG("wc_HKDF_Extract failed.");
                return WOLFSSL_FAILURE;
            }
            else {
                int hkdfHashSz = wolfSSL_EVP_MD_size(ctx->pkey->hkdfMd);
                if (hkdfHashSz <= 0) {
                    WOLFSSL_MSG("Failed to get block size for HKDF hash.");
                    return WOLFSSL_FAILURE;
                }
                /* Length of extract only is always the length of the hash. */
                *keylen = (size_t)hkdfHashSz;
            }
        }
        else if (ctx->pkey->hkdfMode == EVP_PKEY_HKDEF_MODE_EXPAND_ONLY) {
            if (wc_HKDF_Expand(hkdfHashType, ctx->pkey->hkdfKey,
                               ctx->pkey->hkdfKeySz, ctx->pkey->hkdfInfo,
                               ctx->pkey->hkdfInfoSz, key,
                               (word32)*keylen) != 0) {
                WOLFSSL_MSG("wc_HKDF_Expand failed.");
                return WOLFSSL_FAILURE;
            }
        }
        else {
            WOLFSSL_MSG("Invalid HKDF mode.");
            return WOLFSSL_FAILURE;
        }
        break;
#endif /* HAVE_HKDF */
    default:
        WOLFSSL_MSG("Unknown key type");
        return WOLFSSL_FAILURE;
    }
    return WOLFSSL_SUCCESS;
}
#endif /* (!NO_DH && WOLFSSL_DH_EXTRA) || HAVE_ECC || HAVE_HKDF */

#ifdef HAVE_HKDF
int wolfSSL_EVP_PKEY_CTX_set_hkdf_md(WOLFSSL_EVP_PKEY_CTX* ctx,
                                     const WOLFSSL_EVP_MD* md)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set_hkdf_md");

    if (ctx == NULL || ctx->pkey == NULL || md == NULL) {
        WOLFSSL_MSG("Bad argument.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        ctx->pkey->hkdfMd = md;
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_CTX_set_hkdf_md", ret);

    return ret;
}

int wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt(WOLFSSL_EVP_PKEY_CTX* ctx,
                                        const byte* salt, int saltSz)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt");

    if (ctx == NULL || ctx->pkey == NULL || saltSz < 0) {
        WOLFSSL_MSG("Bad argument.");
        ret = WOLFSSL_FAILURE;
    }
    if (ret == WOLFSSL_SUCCESS && ctx->pkey->type != EVP_PKEY_HKDF) {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY type is not HKDF.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && salt != NULL && saltSz > 0) {
        if (ctx->pkey->hkdfSalt != NULL) {
            XFREE(ctx->pkey->hkdfSalt, NULL, DYNAMIC_TYPE_SALT);
        }
        ctx->pkey->hkdfSalt = (byte*)XMALLOC((size_t)saltSz, NULL,
            DYNAMIC_TYPE_SALT);
        if (ctx->pkey->hkdfSalt == NULL) {
            WOLFSSL_MSG("Failed to allocate HKDF salt buffer.");
            ret = WOLFSSL_FAILURE;
        }
        else {
            XMEMCPY(ctx->pkey->hkdfSalt, salt, (size_t)saltSz);
            ctx->pkey->hkdfSaltSz = (word32)saltSz;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt", ret);

    return ret;
}

int wolfSSL_EVP_PKEY_CTX_set1_hkdf_key(WOLFSSL_EVP_PKEY_CTX* ctx,
                                       const byte* key, int keySz)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set1_hkdf_key");

    if (ctx == NULL || ctx->pkey == NULL || key == NULL || keySz <= 0) {
        WOLFSSL_MSG("Bad argument.");
        ret = WOLFSSL_FAILURE;
    }
    if (ret == WOLFSSL_SUCCESS && ctx->pkey->type != EVP_PKEY_HKDF) {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY type is not HKDF.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        if (ctx->pkey->hkdfKey != NULL) {
            XFREE(ctx->pkey->hkdfKey, NULL, DYNAMIC_TYPE_KEY);
        }
        ctx->pkey->hkdfKey = (byte*)XMALLOC((size_t)keySz, NULL,
            DYNAMIC_TYPE_KEY);
        if (ctx->pkey->hkdfKey == NULL) {
            WOLFSSL_MSG("Failed to allocate HKDF key buffer.");
            ret = WOLFSSL_FAILURE;
        }
        else {
            XMEMCPY(ctx->pkey->hkdfKey, key, (size_t)keySz);
            ctx->pkey->hkdfKeySz = (word32)keySz;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_CTX_set1_hkdf_key", ret);

    return ret;
}

int wolfSSL_EVP_PKEY_CTX_add1_hkdf_info(WOLFSSL_EVP_PKEY_CTX* ctx,
                                        const byte* info, int infoSz)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_add1_hkdf_info");

    if (ctx == NULL || ctx->pkey == NULL || infoSz < 0) {
        WOLFSSL_MSG("Bad argument.");
        ret = WOLFSSL_FAILURE;
    }
    if (ret == WOLFSSL_SUCCESS && ctx->pkey->type != EVP_PKEY_HKDF) {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY type is not HKDF.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && info != NULL && infoSz > 0) {
        unsigned char* p;
        /* If there's already info in the buffer, append. */
        p = (byte*)XREALLOC(ctx->pkey->hkdfInfo,
            (size_t)(ctx->pkey->hkdfInfoSz + (word32)infoSz), NULL,
            DYNAMIC_TYPE_INFO);
        if (p == NULL) {
            WOLFSSL_MSG("Failed to reallocate larger HKDF info buffer.");
            ret = WOLFSSL_FAILURE;
        }
        else {
            ctx->pkey->hkdfInfo = p;
            XMEMCPY(ctx->pkey->hkdfInfo + ctx->pkey->hkdfInfoSz, info,
                    (size_t)infoSz);
            ctx->pkey->hkdfInfoSz += (word32)infoSz;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_CTX_add1_hkdf_info", ret);

    return ret;
}

int wolfSSL_EVP_PKEY_CTX_hkdf_mode(WOLFSSL_EVP_PKEY_CTX* ctx, int mode)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_hkdf_mode");

    if (ctx == NULL || ctx->pkey == NULL) {
        WOLFSSL_MSG("Bad argument.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS &&
        mode != EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND &&
        mode != EVP_PKEY_HKDEF_MODE_EXTRACT_ONLY &&
        mode != EVP_PKEY_HKDEF_MODE_EXPAND_ONLY) {
        WOLFSSL_MSG("Invalid HKDF mode.");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        ctx->pkey->hkdfMode = mode;
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_CTX_hkdf_mode", ret);

    return ret;
}
#endif /* HAVE_HKDF */

/* Uses the WOLFSSL_EVP_PKEY_CTX to decrypt a buffer.
 *
 * ctx    EVP_PKEY context of operation.
 * out    Decrypted output buffer. If NULL, puts the maximum output buffer size
          in outLen and returns success.
 * outLen If out is NULL, see above. If out is non-NULL, on input outLen holds
 *        the size of out. On output holds the length of actual decryption.
 * in     Encrypted input buffer.
 * inLen  Length of encrypted data.
 *
 * Returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure.
 */
int wolfSSL_EVP_PKEY_decrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outLen,
                     const unsigned char *in, size_t inLen)
{
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_decrypt");

    if (ctx == NULL || ctx->pkey == NULL) {
        WOLFSSL_MSG("Bad parameter.");
        return 0;
    }

    (void)out;
    (void)outLen;
    (void)in;
    (void)inLen;
    (void)len;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        if (out == NULL) {
            if (ctx->pkey->rsa == NULL) {
                WOLFSSL_MSG("Internal wolfCrypt RSA object is NULL.");
                return WOLFSSL_FAILURE;
            }
            len = wolfSSL_RSA_size(ctx->pkey->rsa);
            if (len <= 0) {
                WOLFSSL_MSG("Error getting RSA size.");
                return WOLFSSL_FAILURE;
            }
            if (outLen == NULL) {
                WOLFSSL_MSG("outLen is NULL.");
                return WOLFSSL_FAILURE;
            }

            *outLen = (size_t)len;
            return WOLFSSL_SUCCESS;
        }

        len = wolfSSL_RSA_private_decrypt((int)inLen, (unsigned char*)in, out,
              ctx->pkey->rsa, ctx->padding);
        if (len < 0) break;
        else {
            *outLen = (size_t)len;
            return WOLFSSL_SUCCESS;
        }
#endif /* NO_RSA */

    case EVP_PKEY_EC:
        WOLFSSL_MSG("EVP_PKEY_EC not implemented.");
        FALL_THROUGH;
    default:
        break;
    }

    return WOLFSSL_FAILURE;
}


/* Initialize a WOLFSSL_EVP_PKEY_CTX structure for decryption
 *
 * ctx    WOLFSSL_EVP_PKEY_CTX structure to use with decryption
 *
 * Returns WOLFSSL_FAILURE on failure and WOLFSSL_SUCCESS on success
 */
int wolfSSL_EVP_PKEY_decrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_decrypt_init");
    switch (ctx->pkey->type) {
    case EVP_PKEY_RSA:
        ctx->op = EVP_PKEY_OP_DECRYPT;
        return WOLFSSL_SUCCESS;
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Uses the WOLFSSL_EVP_PKEY_CTX to encrypt a buffer.
 *
 * ctx    EVP_PKEY context of operation.
 * out    Encrypted output buffer. If NULL, puts the maximum output buffer size
 *        in outlen and returns success.
 * outLen If out is NULL, see above. If out is non-NULL, on input outLen holds
 *        the size of out. On output holds the length of actual encryption.
 * in     Plaintext input buffer.
 * inLen  Length of plaintext.
 *
 * Returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure.
 */
int wolfSSL_EVP_PKEY_encrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outLen,
                     const unsigned char *in, size_t inLen)
{
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_encrypt");

    if (ctx == NULL || ctx->pkey == NULL) {
        WOLFSSL_MSG("Bad parameter.");
        return 0;
    }

    if (ctx->op != EVP_PKEY_OP_ENCRYPT) {
        WOLFSSL_MSG("ctx->op must be set to EVP_PKEY_OP_ENCRYPT. Use "
            "wolfSSL_EVP_PKEY_encrypt_init.");
        return WOLFSSL_FAILURE;
    }

    (void)out;
    (void)outLen;
    (void)in;
    (void)inLen;
    (void)len;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        if (out == NULL) {
            if (ctx->pkey->rsa == NULL) {
                WOLFSSL_MSG("Internal wolfCrypt RSA object is NULL.");
                return WOLFSSL_FAILURE;
            }
            len = wolfSSL_RSA_size(ctx->pkey->rsa);
            if (len <= 0) {
                WOLFSSL_MSG("Error getting RSA size.");
                return WOLFSSL_FAILURE;
            }
            if (outLen == NULL) {
                WOLFSSL_MSG("outLen is NULL.");
                return WOLFSSL_FAILURE;
            }

            *outLen = (size_t)len;
            return WOLFSSL_SUCCESS;
        }

        len = wolfSSL_RSA_public_encrypt((int)inLen, (unsigned char *)in, out,
                  ctx->pkey->rsa, ctx->padding);
        if (len < 0)
            break;
        else {
            *outLen = (size_t)len;
            return WOLFSSL_SUCCESS;
        }
#endif /* NO_RSA */

    case EVP_PKEY_EC:
        WOLFSSL_MSG("EVP_PKEY_EC not implemented");
        FALL_THROUGH;
    default:
        break;
    }

    return WOLFSSL_FAILURE;
}


/* Initialize a WOLFSSL_EVP_PKEY_CTX structure to encrypt data
 *
 * ctx    WOLFSSL_EVP_PKEY_CTX structure to use with encryption
 *
 * Returns WOLFSSL_FAILURE on failure and WOLFSSL_SUCCESS on success
 */
int wolfSSL_EVP_PKEY_encrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_encrypt_init");

    switch (ctx->pkey->type) {
    case EVP_PKEY_RSA:
        ctx->op = EVP_PKEY_OP_ENCRYPT;
        return WOLFSSL_SUCCESS;
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}
/******************************************************************************
* wolfSSL_EVP_PKEY_sign_init -  initializes a public key algorithm context for
* a signing operation.
*
* RETURNS:
* returns WOLFSSL_SUCCESS on success, otherwise returns -2
*/
int wolfSSL_EVP_PKEY_sign_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    int ret = -2;

    WOLFSSL_MSG("wolfSSL_EVP_PKEY_sign_init");
    if (!ctx  || !ctx->pkey)
        return ret;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
        case EVP_PKEY_RSA:
            ctx->op = EVP_PKEY_OP_SIGN;
            ret = WOLFSSL_SUCCESS;
            break;
#endif /* NO_RSA */

#ifndef NO_DSA
        case EVP_PKEY_DSA:
            ctx->op = EVP_PKEY_OP_SIGN;
            ret = WOLFSSL_SUCCESS;
            break;
#endif /* NO_DSA */

#ifdef HAVE_ECC
        case EVP_PKEY_EC:
            ctx->op = EVP_PKEY_OP_SIGN;
            ret = WOLFSSL_SUCCESS;
            break;
#endif /* HAVE_ECC */

        default:
            ret = -2;
    }
    return ret;
}
/******************************************************************************
* wolfSSL_EVP_PKEY_sign - performs a public key signing operation using ctx
* The data to be signed should be hashed since the function does not hash the data.
*
* RETURNS:
* returns WOLFSSL_SUCCESS on success, otherwise returns WOLFSSL_FAILURE
*/

int wolfSSL_EVP_PKEY_sign(WOLFSSL_EVP_PKEY_CTX *ctx, unsigned char *sig,
                        size_t *siglen, const unsigned char *tbs, size_t tbslen)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_sign");

    if (!ctx || ctx->op != EVP_PKEY_OP_SIGN || !ctx->pkey || !siglen)
        return WOLFSSL_FAILURE;

    (void)sig;
    (void)siglen;
    (void)tbs;
    (void)tbslen;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA: {
        unsigned int usiglen = (unsigned int)*siglen;
        if (!sig) {
            int len;
            if (!ctx->pkey->rsa)
                return WOLFSSL_FAILURE;
            len = wc_RsaEncryptSize((RsaKey*)ctx->pkey->rsa->internal);
            if (len < 0)
                return WOLFSSL_FAILURE;
            *siglen = (size_t)len;
            return WOLFSSL_SUCCESS;
        }
        /* wolfSSL_RSA_sign_generic_padding performs a check that the output
         * sig buffer is large enough */
        if (wolfSSL_RSA_sign_generic_padding(wolfSSL_EVP_MD_type(ctx->md), tbs,
                (unsigned int)tbslen, sig, &usiglen, ctx->pkey->rsa, 1,
                ctx->padding) != WOLFSSL_SUCCESS) {
            return WOLFSSL_FAILURE;
        }
        *siglen = (size_t)usiglen;
        return WOLFSSL_SUCCESS;
    }
#endif /* NO_RSA */

#ifndef NO_DSA
    case EVP_PKEY_DSA: {
        int bytes;
        int ret;
        if (!ctx->pkey->dsa)
            return WOLFSSL_FAILURE;
        bytes = wolfSSL_BN_num_bytes(ctx->pkey->dsa->q);
        if (bytes == WOLFSSL_FAILURE)
            return WOLFSSL_FAILURE;
        bytes *= 2;
        if (!sig) {
            *siglen = (size_t)bytes;
            return WOLFSSL_SUCCESS;
        }
        if ((int)*siglen < bytes)
            return WOLFSSL_FAILURE;
        ret = wolfSSL_DSA_do_sign(tbs, sig, ctx->pkey->dsa);
        /* wolfSSL_DSA_do_sign() can return WOLFSSL_FATAL_ERROR */
        if (ret != WOLFSSL_SUCCESS)
            return ret;
        if (bytes == WOLFSSL_FAILURE)
            return WOLFSSL_FAILURE;
        *siglen = (size_t)bytes;
        return WOLFSSL_SUCCESS;
    }
#endif /* NO_DSA */

#ifdef HAVE_ECC
    case EVP_PKEY_EC: {
        int ret;
        WOLFSSL_ECDSA_SIG *ecdsaSig;
        if (!sig) {
            WOLFSSL_EC_KEY *key = ctx->pkey->ecc;
            ecc_key* eckey;
            if (!key)
                return WOLFSSL_FAILURE;
            /* set internal key if not done */
            if (key->inSet == 0 && SetECKeyInternal(key) != WOLFSSL_SUCCESS)
                return WOLFSSL_FAILURE;
            eckey = (ecc_key*)ctx->pkey->ecc->internal;
            if (!eckey)
                return WOLFSSL_FAILURE;
            ret = wc_ecc_sig_size(eckey);
            if (ret == 0)
                return WOLFSSL_FAILURE;
            *siglen = (size_t)ret;
            return WOLFSSL_SUCCESS;
        }
        ecdsaSig = wolfSSL_ECDSA_do_sign(tbs, (int)tbslen, ctx->pkey->ecc);
        if (ecdsaSig == NULL)
            return WOLFSSL_FAILURE;
        ret = wolfSSL_i2d_ECDSA_SIG(ecdsaSig, NULL);
        if (ret == 0 || ret > (int)*siglen) {
            wolfSSL_ECDSA_SIG_free(ecdsaSig);
            return WOLFSSL_FAILURE;
        }
        ret = wolfSSL_i2d_ECDSA_SIG(ecdsaSig, &sig);
        wolfSSL_ECDSA_SIG_free(ecdsaSig);
        if (ret == 0)
            return WOLFSSL_FAILURE;
        *siglen = (size_t)ret;
        return WOLFSSL_SUCCESS;
    }
#endif /* HAVE_ECC */

    default:
        break;
    }
    return WOLFSSL_FAILURE;
}

/******************************************************************************
* wolfSSL_EVP_PKEY_verify_init - initializes a public key algorithm context for
* a verification operation.
*
* RETURNS:
* returns WOLFSSL_SUCCESS on success, WOLFSSL_FAILURE on failure. In particular
* a return value of -2 indicates the operation is not supported by the public
* key algorithm.
*/

int wolfSSL_EVP_PKEY_verify_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_verify_init");

    if (!ctx || !ctx->pkey)
        return WOLFSSL_FAILURE;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
        case EVP_PKEY_RSA:
            ctx->op = EVP_PKEY_OP_VERIFY;
            return WOLFSSL_SUCCESS;
#endif /* NO_RSA */

#ifndef NO_DSA
        case EVP_PKEY_DSA:
            ctx->op = EVP_PKEY_OP_VERIFY;
            return WOLFSSL_SUCCESS;
#endif /* NO_DSA */

#ifdef HAVE_ECC
        case EVP_PKEY_EC:
            ctx->op = EVP_PKEY_OP_VERIFY;
            return WOLFSSL_SUCCESS;
#endif /* HAVE_ECC */

        default:
            return -2;
    }
}

/******************************************************************************
* wolfSSL_EVP_PKEY_verify - verifies a signature using ctx
*
* RETURNS:
* returns WOLFSSL_SUCCESS on success, WOLFSSL_FAILURE on failure. In particular
* a return value of -2 indicates the operation is not supported by the public
* key algorithm.
*/

int wolfSSL_EVP_PKEY_verify(WOLFSSL_EVP_PKEY_CTX *ctx, const unsigned char *sig,
                            size_t siglen, const unsigned char *tbs,
                            size_t tbslen)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_verify");

    if (!ctx || ctx->op != EVP_PKEY_OP_VERIFY || !ctx->pkey)
        return WOLFSSL_FAILURE;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        return wolfSSL_RSA_verify_ex(WC_HASH_TYPE_NONE, tbs,
            (unsigned int)tbslen, sig, (unsigned int)siglen, ctx->pkey->rsa,
            ctx->padding);
#endif /* NO_RSA */

#ifndef NO_DSA
     case EVP_PKEY_DSA: {
        int dsacheck = 0;
        if (wolfSSL_DSA_do_verify(tbs, (unsigned char *)sig, ctx->pkey->dsa,
            &dsacheck) != WOLFSSL_SUCCESS || dsacheck != 1)
            return WOLFSSL_FAILURE;
        return WOLFSSL_SUCCESS;
     }
#endif /* NO_DSA */

#ifdef HAVE_ECC
    case EVP_PKEY_EC: {
        int ret;
        WOLFSSL_ECDSA_SIG *ecdsaSig = wolfSSL_d2i_ECDSA_SIG(
            NULL, (const unsigned char **)&sig, (long)siglen);
        if (ecdsaSig == NULL)
            return WOLFSSL_FAILURE;
        ret = wolfSSL_ECDSA_do_verify(tbs, (int)tbslen, ecdsaSig,
            ctx->pkey->ecc);
        wolfSSL_ECDSA_SIG_free(ecdsaSig);
        return ret;
    }
#endif /* HAVE_ECC */

    default:
        return -2;
    }
}

/* Get the size in bits for WOLFSSL_EVP_PKEY key
 *
 * pkey WOLFSSL_EVP_PKEY structure to get key size of
 *
 * returns the size in bits of key on success
 */
int wolfSSL_EVP_PKEY_bits(const WOLFSSL_EVP_PKEY *pkey)
{
    int bytes;

    if (pkey == NULL) return 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_bits");
    if ((bytes = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey)) ==0) return 0;
    return bytes*8;
}


int wolfSSL_EVP_PKEY_paramgen_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    (void)ctx;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_EVP_PKEY_CTX_set_ec_paramgen_curve_nid(WOLFSSL_EVP_PKEY_CTX *ctx,
        int nid)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_CTX_set_ec_paramgen_curve_nid");
#ifdef HAVE_ECC
    if (ctx != NULL && ctx->pkey != NULL && ctx->pkey->type == EVP_PKEY_EC) {
        ctx->curveNID = nid;
        return WOLFSSL_SUCCESS;
    }
    else
#endif
    {
#ifndef HAVE_ECC
        (void)ctx;
        (void)nid;
        WOLFSSL_MSG("Support not compiled in");
#else
        WOLFSSL_MSG("Bad parameter");
#endif
        return WOLFSSL_FAILURE;
    }
}

int wolfSSL_EVP_PKEY_paramgen(WOLFSSL_EVP_PKEY_CTX* ctx,
                              WOLFSSL_EVP_PKEY** pkey)
{
    int ret = WOLFSSL_SUCCESS;
    int ownPkey = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_paramgen");

    if (ctx == NULL || pkey == NULL) {
        WOLFSSL_MSG("Bad parameter");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && *pkey == NULL) {
        /* Only ECC is supported currently. */
        if (ctx->pkey == NULL || ctx->pkey->type != EVP_PKEY_EC) {
            WOLFSSL_MSG("Key not set or key type not supported.");
            ret = WOLFSSL_FAILURE;
        }
        else {
            *pkey = wolfSSL_EVP_PKEY_new();
            if (*pkey == NULL) {
                WOLFSSL_MSG("Failed to create WOLFSSL_EVP_PKEY.");
                ret = WOLFSSL_FAILURE;
            }
            else {
                (*pkey)->type = ctx->pkey->type;
                ownPkey = 1;
            }
        }
    }
    if (ret == WOLFSSL_SUCCESS) {
        switch ((*pkey)->type) {
        #ifdef HAVE_ECC
            /* For ECC parameter generation we just need to set the group, which
             * wolfSSL_EC_KEY_new_by_curve_name will do. */
            case EVP_PKEY_EC:
                (*pkey)->ecc = wolfSSL_EC_KEY_new_by_curve_name(ctx->curveNID);
                if ((*pkey)->ecc == NULL) {
                    WOLFSSL_MSG("Failed to create WOLFSSL_EC_KEY.");
                    ret = WOLFSSL_FAILURE;
                }
                else {
                    (*pkey)->ownEcc = 1;
                }
                break;
        #endif
            default:
                ret = WOLFSSL_FAILURE;
                break;
        }
    }

    if (ret != WOLFSSL_SUCCESS && ownPkey) {
        wolfSSL_EVP_PKEY_free(*pkey);
        *pkey = NULL;
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_paramgen", ret);

    return ret;
}

/* wolfSSL only supports writing out named curves so no need to store the flag.
 * In short, it is preferred to write out the name of the curve chosen instead
 * of the explicit parameters.
 * The difference is nicely explained and illustrated in section
 * "ECDH and Named Curves" of
 * https://wiki.openssl.org/index.php/Elliptic_Curve_Diffie_Hellman */
int wolfSSL_EVP_PKEY_CTX_set_ec_param_enc(WOLFSSL_EVP_PKEY_CTX *ctx,
        int flag)
{
    (void)ctx;
    (void)flag;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_EVP_PKEY_keygen_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    (void)ctx;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_EVP_PKEY_keygen(WOLFSSL_EVP_PKEY_CTX *ctx,
  WOLFSSL_EVP_PKEY **ppkey)
{
    int ret = WOLFSSL_FAILURE;
    int ownPkey = 0;
    WOLFSSL_EVP_PKEY* pkey;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_keygen");

    if (ctx == NULL || ppkey == NULL) {
        return BAD_FUNC_ARG;
    }

    pkey = *ppkey;
    if (pkey == NULL) {
        if (ctx->pkey == NULL ||
                (ctx->pkey->type != EVP_PKEY_EC &&
                 ctx->pkey->type != EVP_PKEY_RSA &&
                 ctx->pkey->type != EVP_PKEY_DH)) {
            WOLFSSL_MSG("Key not set or key type not supported");
            return BAD_FUNC_ARG;
        }
        pkey = wolfSSL_EVP_PKEY_new();
        if (pkey == NULL) {
            return MEMORY_E;
        }
        ownPkey = 1;
        pkey->type = ctx->pkey->type;
    }

    switch (pkey->type) {
#if !defined(HAVE_FAST_RSA) && defined(WOLFSSL_KEY_GEN) && \
    !defined(NO_RSA) && !defined(HAVE_USER_RSA)
        case EVP_PKEY_RSA:
            pkey->rsa = wolfSSL_RSA_generate_key(ctx->nbits, WC_RSA_EXPONENT,
                NULL, NULL);
            if (pkey->rsa) {
                pkey->ownRsa = 1;
                pkey->pkey_sz = wolfSSL_i2d_RSAPrivateKey(pkey->rsa,
                        (unsigned char**)&pkey->pkey.ptr);
                ret = WOLFSSL_SUCCESS;
            }
            break;
#endif
#ifdef HAVE_ECC
        case EVP_PKEY_EC:
            /* pkey->ecc may not be NULL, if, for example, it was populated by a
             * prior call to wolfSSL_EVP_PKEY_paramgen. */
            if (pkey->ecc == NULL) {
                pkey->ecc = wolfSSL_EC_KEY_new_by_curve_name(ctx->curveNID);
            }
            if (pkey->ecc) {
                ret = wolfSSL_EC_KEY_generate_key(pkey->ecc);
                if (ret == WOLFSSL_SUCCESS) {
                    pkey->ownEcc = 1;
                }
            }
            break;
#endif
#if !defined(NO_DH) && (!defined(HAVE_FIPS) || FIPS_VERSION_GT(2,0))
        case EVP_PKEY_DH:
            pkey->dh = wolfSSL_DH_new();
            if (pkey->dh) {
                pkey->ownDh = 1;
                /* load DH params from CTX */
                ret = wolfSSL_DH_LoadDer(pkey->dh,
                        (const unsigned char*)ctx->pkey->pkey.ptr,
                        ctx->pkey->pkey_sz);
                if (ret == WOLFSSL_SUCCESS) {
                    ret = wolfSSL_DH_generate_key(pkey->dh);
                }
                if (ret == WOLFSSL_SUCCESS) {
                    /* copy private/public key from external to internal */
                    ret = SetDhInternal(pkey->dh);
                }
            }
            break;
#endif
        default:
            break;
    }

    if (ret != WOLFSSL_SUCCESS && ownPkey) {
        wolfSSL_EVP_PKEY_free(pkey);
        pkey = NULL;
    }

    *ppkey = pkey;

    return ret;
}

/* Get the size in bytes for WOLFSSL_EVP_PKEY key
 *
 * pkey WOLFSSL_EVP_PKEY structure to get key size of
 *
 * returns the size of a key on success which is the maximum size of a
 *         signature
 */
int wolfSSL_EVP_PKEY_size(WOLFSSL_EVP_PKEY *pkey)
{
    if (pkey == NULL) return 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_size");

    switch (pkey->type) {
#ifndef NO_RSA
    case EVP_PKEY_RSA:
        return (int)wolfSSL_RSA_size((const WOLFSSL_RSA*)(pkey->rsa));
#endif /* !NO_RSA */

#ifndef NO_DSA
    case EVP_PKEY_DSA:
        if (pkey->dsa == NULL ||
                (!pkey->dsa->exSet &&
                        SetDsaExternal(pkey->dsa) != WOLFSSL_SUCCESS))
            return WOLFSSL_FAILURE;
        return wolfSSL_BN_num_bytes(pkey->dsa->p);
#endif

#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        if (pkey->ecc == NULL || pkey->ecc->internal == NULL) {
            WOLFSSL_MSG("No ECC key has been set");
            break;
        }
        return wc_ecc_size((ecc_key*)(pkey->ecc->internal));
#endif /* HAVE_ECC */

    default:
        break;
    }
    return 0;
}


int wolfSSL_EVP_PKEY_copy_parameters(WOLFSSL_EVP_PKEY *to,
        const WOLFSSL_EVP_PKEY *from)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_copy_parameters");

    if (!to || !from) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (to->type == EVP_PKEY_NONE) {
        to->type = from->type;
    }
    else if (to->type != from->type) {
        WOLFSSL_MSG("Different key types");
        return WOLFSSL_FAILURE;
    }

    switch(from->type) {
#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        if (from->ecc) {
            if (!to->ecc) {
                if ((to->ecc = wolfSSL_EC_KEY_new()) == NULL) {
                    WOLFSSL_MSG("wolfSSL_EC_KEY_new error");
                    return WOLFSSL_FAILURE;
                }
                to->ownEcc = 1;
            }
            to->ecc->group->curve_idx = from->ecc->group->curve_idx;
            to->ecc->group->curve_nid = from->ecc->group->curve_nid;
            to->ecc->group->curve_oid = from->ecc->group->curve_oid;
        }
        else {
            WOLFSSL_MSG("Missing ECC struct");
            return WOLFSSL_FAILURE;
        }
        break;
#endif
#ifndef NO_DSA
    case EVP_PKEY_DSA:
        if (from->dsa) {
            WOLFSSL_BIGNUM* cpy;
            if (!to->dsa) {
                if ((to->dsa = wolfSSL_DSA_new()) == NULL) {
                    WOLFSSL_MSG("wolfSSL_DSA_new error");
                    return WOLFSSL_FAILURE;
                }
                to->ownDsa = 1;
            }

            /* free existing BIGNUMs if needed before copying over new */
            wolfSSL_BN_free(to->dsa->p);
            wolfSSL_BN_free(to->dsa->g);
            wolfSSL_BN_free(to->dsa->q);
            to->dsa->p = NULL;
            to->dsa->g = NULL;
            to->dsa->q = NULL;

            if (!(cpy = wolfSSL_BN_dup(from->dsa->p))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error");
                return WOLFSSL_FAILURE;
            }
            to->dsa->p = cpy;
            if (!(cpy = wolfSSL_BN_dup(from->dsa->q))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error");
                return WOLFSSL_FAILURE;
            }
            to->dsa->q = cpy;
            if (!(cpy = wolfSSL_BN_dup(from->dsa->g))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error");
                return WOLFSSL_FAILURE;
            }
            to->dsa->g = cpy;
        }
        else {
            WOLFSSL_MSG("Missing DSA struct");
            return WOLFSSL_FAILURE;
        }
        break;
#endif
#ifndef NO_DH
    case EVP_PKEY_DH:
        if (from->dh) {
            WOLFSSL_BIGNUM* cpy;
            if (!to->dh) {
                if ((to->dh = wolfSSL_DH_new()) == NULL) {
                    WOLFSSL_MSG("wolfSSL_DH_new error");
                    return WOLFSSL_FAILURE;
                }
                to->ownDh = 1;
            }

            /* free existing BIGNUMs if needed before copying over new */
            wolfSSL_BN_free(to->dh->p);
            wolfSSL_BN_free(to->dh->g);
            wolfSSL_BN_free(to->dh->q);
            to->dh->p = NULL;
            to->dh->g = NULL;
            to->dh->q = NULL;

            if (!(cpy = wolfSSL_BN_dup(from->dh->p))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error, DH p");
                return WOLFSSL_FAILURE;
            }
            to->dh->p = cpy;
            if (!(cpy = wolfSSL_BN_dup(from->dh->g))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error, DH g");
                return WOLFSSL_FAILURE;
            }
            to->dh->g = cpy;
            if (!(cpy = wolfSSL_BN_dup(from->dh->q))) {
                WOLFSSL_MSG("wolfSSL_BN_dup error, DH q");
                return WOLFSSL_FAILURE;
            }
            to->dh->q = cpy;
        }
        else {
            WOLFSSL_MSG("Missing DH struct");
            return WOLFSSL_FAILURE;
        }
        break;
#endif
#ifndef NO_RSA
    case EVP_PKEY_RSA:
#endif
    default:
        WOLFSSL_MSG("Copy parameters not available for this key type");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_EVP_PKEY_missing_parameters(WOLFSSL_EVP_PKEY *pkey)
{
    (void)pkey;
    /* not using missing params callback and returning zero to indicate success */
    return 0;
}
#endif

/* wolfSSL_EVP_PKEY_cmp
 * returns 0 on success, -1 on failure.
 *
 * This behavior is different from openssl.
 *  EVP_PKEY_cmp returns:
 *    1 : two keys match
 *    0 : do not match
 *    -1: key types are different
 *    -2: the operation is not supported
 * If you want this function behave the same as openSSL,
 * define WOLFSSL_ERROR_CODE_OPENSSL so that WS_RETURN_CODE translates return
 * codes to match OpenSSL equivalent behavior.
 */
int wolfSSL_EVP_PKEY_cmp(const WOLFSSL_EVP_PKEY *a, const WOLFSSL_EVP_PKEY *b)
{
    int ret = -1; /* failure */
    int a_sz = 0, b_sz = 0;

    if (a == NULL || b == NULL)
        return WS_RETURN_CODE(ret, WOLFSSL_FAILURE);

    /* check its the same type of key */
    if (a->type != b->type)
        return WS_RETURN_CODE(ret, -1);

    /* get size based on key type */
    switch (a->type) {
#ifndef NO_RSA
    case EVP_PKEY_RSA:
        a_sz = (int)wolfSSL_RSA_size((const WOLFSSL_RSA*)(a->rsa));
        b_sz = (int)wolfSSL_RSA_size((const WOLFSSL_RSA*)(b->rsa));
        break;
#endif /* !NO_RSA */
#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        if (a->ecc == NULL || a->ecc->internal == NULL ||
            b->ecc == NULL || b->ecc->internal == NULL) {
            return ret;
        }
        a_sz = wc_ecc_size((ecc_key*)(a->ecc->internal));
        b_sz = wc_ecc_size((ecc_key*)(b->ecc->internal));
        break;
#endif /* HAVE_ECC */
    default:
        return WS_RETURN_CODE(ret, -2);
    } /* switch (a->type) */

    /* check size */
    if (a_sz <= 0 || b_sz <= 0 || a_sz != b_sz) {
        return WS_RETURN_CODE(ret, WOLFSSL_FAILURE);
    }

    /* check public key size */
    if (a->pkey_sz > 0 && b->pkey_sz > 0 && a->pkey_sz != b->pkey_sz) {
        return WS_RETURN_CODE(ret, WOLFSSL_FAILURE);
    }

    /* check public key */
    if (a->pkey.ptr && b->pkey.ptr) {
        if (XMEMCMP(a->pkey.ptr, b->pkey.ptr, (size_t)a->pkey_sz) != 0) {
            return WS_RETURN_CODE(ret, WOLFSSL_FAILURE);
        }
    }
#if defined(WOLFSSL_ERROR_CODE_OPENSSL)
    ret = 1; /* the keys match */
#else
    ret = 0; /* success */
#endif
    return ret;
}

/**
 * validate DH algorithm parameters
 * @param dh_key  a pointer to WOLFSSL_EVP_PKEY_CTX structure
 * @return WOLFSSL_SUCCESS on success, otherwise failure
 */
static int DH_param_check(WOLFSSL_DH* dh_key)
{
    int ret = WOLFSSL_SUCCESS;
    WOLFSSL_BN_CTX* ctx = NULL;
    WOLFSSL_BIGNUM *num1 = NULL;
    WOLFSSL_BIGNUM *num2 = NULL;

    WOLFSSL_ENTER("DH_param_check");

    ctx = wolfSSL_BN_CTX_new();
    if (ctx == NULL) {
        WOLFSSL_MSG("failed to allocate memory");
        return WOLFSSL_FAILURE;
    }

    num1 = wolfSSL_BN_new();
    num2 = wolfSSL_BN_new();
    if (num1 == NULL || num2 == NULL) {
        WOLFSSL_MSG("failed to assign big number");
        ret = WOLFSSL_FAILURE;
    }

    /* prime check */
    if (ret == WOLFSSL_SUCCESS &&
        wolfSSL_BN_is_odd(dh_key->p) == 0){
        WOLFSSL_MSG("dh_key->p is not prime");
        ret = WOLFSSL_FAILURE;
    } /* TODO safe prime check. need BN_rshift1 */

    /* generator check */
    if (ret == WOLFSSL_SUCCESS &&
       (wolfSSL_BN_is_one(dh_key->g) ||
        wolfSSL_BN_is_negative(dh_key->g) ||
        wolfSSL_BN_is_zero(dh_key->g))) {
        WOLFSSL_MSG("dh_key->g is not suitable generator");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS &&
        wolfSSL_BN_cmp(dh_key->p, dh_key->g) <= 0) {
        WOLFSSL_MSG("dh_key->g is not suitable generator");
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS &&
        dh_key->q != NULL)
    {
        if (ret == WOLFSSL_SUCCESS &&
            wolfSSL_BN_mod_exp(num1, dh_key->g, dh_key->q, dh_key->p, ctx) ==
            WOLFSSL_FAILURE) {
            WOLFSSL_MSG("BN_mod_exp failed");
            ret = WOLFSSL_FAILURE;
        }
        else
            if (ret == WOLFSSL_SUCCESS &&
                wolfSSL_BN_is_one(num1) == WOLFSSL_FAILURE) {
                WOLFSSL_MSG("dh_key->g is not suitable generator");
                ret = WOLFSSL_FAILURE;
            }
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
        /* test if the number q is prime. */
        if (ret == WOLFSSL_SUCCESS &&
            (wolfSSL_BN_is_prime_ex(dh_key->q, 64, ctx, NULL) <= 0)) {
            WOLFSSL_MSG("dh_key->q is not prime or error during check.");
            ret = WOLFSSL_FAILURE;
        } /* else TODO check q div q - 1. need BN_div */
#endif
    }

    /* clean up */
    wolfSSL_BN_CTX_free(ctx);
    wolfSSL_BN_free(num1);
    wolfSSL_BN_free(num2);

    WOLFSSL_LEAVE("DH_param_check", WOLFSSL_SUCCESS);
    return ret;
}
/**
 * validate the algorithm parameters
 * @param ctx  a pointer to WOLFSSL_EVP_PKEY_CTX structure
 * @return WOLFSSL_SUCCESS on success, otherwise failure
 */
int wolfSSL_EVP_PKEY_param_check(WOLFSSL_EVP_PKEY_CTX* ctx)
{
    int type;
    int ret;
    WOLFSSL_DH* dh_key = NULL;

    /* sanity check */
    if (ctx == NULL) {
        return WOLFSSL_FAILURE;
    }

    type = wolfSSL_EVP_PKEY_type(wolfSSL_EVP_PKEY_base_id(ctx->pkey));
    switch (type) {
        #if !defined(NO_RSA)
            case EVP_PKEY_RSA:
                WOLFSSL_MSG("EVP_PKEY_RSA not yet implemented");
                return WOLFSSL_FAILURE;
        #endif
        #if defined(HAVE_ECC)
            case EVP_PKEY_EC:
                WOLFSSL_MSG("EVP_PKEY_EC not yet implemented");
                return WOLFSSL_FAILURE;
        #endif
        #if !defined(NO_DSA)
            case EVP_PKEY_DSA:
                WOLFSSL_MSG("EVP_PKEY_DSA not yet implemented");
                return WOLFSSL_FAILURE;
        #endif
        #if defined(OPENSSL_ALL) || defined(WOLFSSL_QT) || defined(WOLFSSL_OPENSSH)
        #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA) && !defined(NO_FILESYSTEM)
            case EVP_PKEY_DH:
                dh_key = wolfSSL_EVP_PKEY_get1_DH(ctx->pkey);
                if (dh_key != NULL) {
                    ret = DH_param_check(dh_key);
                    wolfSSL_DH_free(dh_key);
                }
                else
                    ret = WOLFSSL_FAILURE;
            return ret;
        #endif
        #endif
        default:
            WOLFSSL_MSG("Unknown PKEY type");
            break;
    }

    (void)ret;
    (void)DH_param_check;
    (void)dh_key;
    return WOLFSSL_FAILURE;
}

/* Initialize structure for signing
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to initialize
 * type is the type of message digest to use
 *
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_EVP_SignInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_SignInit");
    return wolfSSL_EVP_DigestInit(ctx,type);
}

int wolfSSL_EVP_SignInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_SignInit");
    return wolfSSL_EVP_DigestInit_ex(ctx,type,impl);
}


/* Update structure with data for signing
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to update
 * data buffer holding data to update with for sign
 * len  length of data buffer
 *
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_EVP_SignUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("EVP_SignUpdate(");
    return wolfSSL_EVP_DigestUpdate(ctx, data, len);
}
static const WOLFSSL_EVP_MD* wolfSSL_macType2EVP_md(enum wc_HashType type)
{
    const struct s_ent *ent ;

    for (ent = md_tbl; ent->name != NULL; ent++) {
        if (ent->macType == type) {
            return ent->name;
        }
    }
    return NULL;
}

/* Finalize structure for signing
 *
 * ctx    WOLFSSL_EVP_MD_CTX structure to finalize
 * sigret buffer to hold resulting signature
 * siglen length of sigret buffer
 * pkey   key to sign with
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_SignFinal(WOLFSSL_EVP_MD_CTX *ctx, unsigned char *sigret,
                  unsigned int *siglen, WOLFSSL_EVP_PKEY *pkey)
{
    unsigned int mdsize;
    unsigned char md[WC_MAX_DIGEST_SIZE];
    int ret;
    (void)sigret;
    (void)siglen;

    WOLFSSL_ENTER("EVP_SignFinal");
    if (ctx == NULL)
        return WOLFSSL_FAILURE;

    ret = wolfSSL_EVP_DigestFinal(ctx, md, &mdsize);
    if (ret <= 0)
        return ret;

    switch (pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA: {
        int nid;
        const WOLFSSL_EVP_MD *ctxmd;

        ctxmd = wolfSSL_EVP_MD_CTX_md(ctx);
        if (ctxmd == NULL)
            return WOLFSSL_FAILURE;

        nid = wolfSSL_EVP_MD_type(ctxmd);
        if (nid < 0)
            return WOLFSSL_FAILURE;
        return wolfSSL_RSA_sign(nid, md, mdsize, sigret,
                                siglen, pkey->rsa);
    }
#endif /* NO_RSA */
#ifndef NO_DSA
    case EVP_PKEY_DSA: {
        int bytes;
        ret = wolfSSL_DSA_do_sign(md, sigret, pkey->dsa);
        /* wolfSSL_DSA_do_sign() can return WOLFSSL_FATAL_ERROR */
        if (ret != WOLFSSL_SUCCESS)
            return ret;
        bytes = wolfSSL_BN_num_bytes(pkey->dsa->q);
        if (bytes == WOLFSSL_FAILURE || (int)*siglen < bytes * 2)
            return WOLFSSL_FAILURE;
        *siglen = (unsigned int)(bytes * 2);
        return WOLFSSL_SUCCESS;
    }
#endif
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Initialize structure for verifying signature
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to initialize
 * type is the type of message digest to use
 *
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_EVP_VerifyInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyInit");
    return wolfSSL_EVP_DigestInit(ctx,type);
}


/* Update structure for verifying signature
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to update
 * data buffer holding data to update with for verify
 * len  length of data buffer
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_VerifyUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyUpdate");
    return wolfSSL_EVP_DigestUpdate(ctx, data, len);
}


/* Finalize structure for verifying signature
 *
 * ctx    WOLFSSL_EVP_MD_CTX structure to finalize
 * sig    buffer holding signature
 * siglen length of sig buffer
 * pkey   key to verify with
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_VerifyFinal(WOLFSSL_EVP_MD_CTX *ctx,
        const unsigned char*sig, unsigned int siglen, WOLFSSL_EVP_PKEY *pkey)
{
    int ret;
    unsigned char md[WC_MAX_DIGEST_SIZE];
    unsigned int mdsize;

    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyFinal");
    ret = wolfSSL_EVP_DigestFinal(ctx, md, &mdsize);
    if (ret <= 0) return ret;

    (void)sig;
    (void)siglen;

    switch (pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA: {
        int nid;
        const WOLFSSL_EVP_MD *ctxmd = wolfSSL_EVP_MD_CTX_md(ctx);
        if (ctxmd == NULL) break;
        nid = wolfSSL_EVP_MD_type(ctxmd);
        if (nid < 0) break;
        return wolfSSL_RSA_verify(nid, md, mdsize, sig,
                (unsigned int)siglen, pkey->rsa);
    }
#endif /* NO_RSA */

    case EVP_PKEY_DSA:
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}

int wolfSSL_EVP_add_cipher(const WOLFSSL_EVP_CIPHER *cipher)
{
    (void)cipher;
    /* nothing to do */
    return 0;
}


WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_mac_key(int type, WOLFSSL_ENGINE* e,
                                          const unsigned char* key, int keylen)
{
    WOLFSSL_EVP_PKEY* pkey;

    (void)e;

    if (type != EVP_PKEY_HMAC || (key == NULL && keylen != 0))
        return NULL;

    pkey = wolfSSL_EVP_PKEY_new();
    if (pkey != NULL) {
        pkey->pkey.ptr = (char*)XMALLOC((size_t)keylen, NULL,
            DYNAMIC_TYPE_PUBLIC_KEY);
        if (pkey->pkey.ptr == NULL && keylen > 0) {
            wolfSSL_EVP_PKEY_free(pkey);
            pkey = NULL;
        }
        else {
            if (keylen) {
                XMEMCPY(pkey->pkey.ptr, key, (size_t)keylen);
            }
            pkey->pkey_sz = keylen;
            pkey->type = pkey->save_type = type;
        }
    }

    return pkey;
}


#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_DIRECT)
WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_CMAC_key(WOLFSSL_ENGINE* e,
        const unsigned char* priv, size_t len, const WOLFSSL_EVP_CIPHER *cipher)
{
    WOLFSSL_EVP_PKEY* pkey;
    WOLFSSL_CMAC_CTX* ctx;
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_new_CMAC_key");

    if (priv == NULL || len == 0 || cipher == NULL) {
        WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_new_CMAC_key", BAD_FUNC_ARG);
        return NULL;
    }

    ctx = wolfSSL_CMAC_CTX_new();
    if (ctx == NULL) {
        WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_new_CMAC_key", 0);
        return NULL;
    }

    ret = wolfSSL_CMAC_Init(ctx, priv, len, cipher, e);
    if (ret == WOLFSSL_FAILURE) {
        wolfSSL_CMAC_CTX_free(ctx);
        WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_new_CMAC_key", 0);
        return NULL;
    }

    pkey = wolfSSL_EVP_PKEY_new();
    if (pkey != NULL) {
        pkey->pkey.ptr = (char*)XMALLOC((size_t)len, NULL,
            DYNAMIC_TYPE_PUBLIC_KEY);
        if (pkey->pkey.ptr == NULL && len > 0) {
            wolfSSL_EVP_PKEY_free(pkey);
            pkey = NULL;
            wolfSSL_CMAC_CTX_free(ctx);
        }
        else {
            if (len) {
                XMEMCPY(pkey->pkey.ptr, priv, (size_t)len);
            }
            pkey->pkey_sz = (int)len;
            pkey->type = pkey->save_type = EVP_PKEY_CMAC;
            pkey->cmacCtx = ctx;
        }
    }
    else {
        wolfSSL_CMAC_CTX_free(ctx);
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_PKEY_new_CMAC_key", 0);
    return pkey;
}
#endif /* defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_DIRECT) */

const unsigned char* wolfSSL_EVP_PKEY_get0_hmac(const WOLFSSL_EVP_PKEY* pkey,
                                                size_t* len)
{
    if (pkey == NULL || len == NULL)
        return NULL;

    *len = (size_t)pkey->pkey_sz;

    return (const unsigned char*)pkey->pkey.ptr;
}

/* Initialize an EVP_DigestSign/Verify operation.
 * Initialize a digest for RSA and ECC keys, or HMAC for HMAC key.
 */
static int wolfSSL_evp_digest_pk_init(WOLFSSL_EVP_MD_CTX *ctx,
                                      WOLFSSL_EVP_PKEY_CTX **pctx,
                                      const WOLFSSL_EVP_MD *type,
                                      WOLFSSL_ENGINE *e,
                                      WOLFSSL_EVP_PKEY *pkey)
{
    if (!type) {
        int default_digest;
        if (wolfSSL_EVP_PKEY_get_default_digest_nid(pkey, &default_digest)
                != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Could not get default digest");
            return WOLFSSL_FAILURE;
        }
        type = wolfSSL_EVP_get_digestbynid(default_digest);
        if (!type) {
            return BAD_FUNC_ARG;
        }
    }

    if (pkey->type == EVP_PKEY_HMAC) {
        int                  hashType;

    #ifndef NO_SHA256
        if (XSTRCMP(type, "SHA256") == 0) {
            hashType = WC_SHA256;
        } else
    #endif
    #ifndef NO_SHA
        if ((XSTRCMP(type, "SHA") == 0) || (XSTRCMP(type, "SHA1") == 0)) {
            hashType = WC_SHA;
        } else
    #endif /* NO_SHA */
    #ifdef WOLFSSL_SHA224
        if (XSTRCMP(type, "SHA224") == 0) {
            hashType = WC_SHA224;
        } else
    #endif
    #ifdef WOLFSSL_SHA384
        if (XSTRCMP(type, "SHA384") == 0) {
            hashType = WC_SHA384;
        } else
    #endif
    #ifdef WOLFSSL_SHA512
        if (XSTRCMP(type, "SHA512") == 0) {
            hashType = WC_SHA512;
        } else
    #endif
#ifdef WOLFSSL_SHA3
    #ifndef WOLFSSL_NOSHA3_224
        if (XSTRCMP(type, "SHA3_224") == 0) {
            hashType = WC_SHA3_224;
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_256
        if (XSTRCMP(type, "SHA3_256") == 0) {
            hashType = WC_SHA3_256;
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_384
        if (XSTRCMP(type, "SHA3_384") == 0) {
            hashType = WC_SHA3_384;
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_512
        if (XSTRCMP(type, "SHA3_512") == 0) {
            hashType = WC_SHA3_512;
        } else
    #endif
#endif
    #ifdef WOLFSSL_SM3
        if (XSTRCMP(type, "SM3") == 0) {
            hashType = WC_SM3;
        } else
    #endif
    #ifndef NO_MD5
        if (XSTRCMP(type, "MD5") == 0) {
            hashType = WC_MD5;
        } else
    #endif
             return BAD_FUNC_ARG;

        {
            size_t keySz = 0;
            const unsigned char* key;

            key = wolfSSL_EVP_PKEY_get0_hmac(pkey, &keySz);

            if (wc_HmacInit(&ctx->hash.hmac, NULL, INVALID_DEVID) != 0)
                return WOLFSSL_FAILURE;

            if (wc_HmacSetKey(&ctx->hash.hmac, hashType, key, (word32)keySz) != 0)
                return WOLFSSL_FAILURE;
        }

        ctx->isHMAC = 1;
    }
    else if (wolfSSL_EVP_DigestInit(ctx, type) != 1)
            return WOLFSSL_FAILURE;

    if (ctx->pctx == NULL) {
        ctx->pctx = wolfSSL_EVP_PKEY_CTX_new(pkey, e);
        if (ctx->pctx == NULL)
            return WOLFSSL_FAILURE;
    }
    if (pctx != NULL)
        *pctx = ctx->pctx;
    return WOLFSSL_SUCCESS;
}

/* Update an EVP_DigestSign/Verify operation.
 * Update a digest for RSA and ECC keys, or HMAC for HMAC key.
 */
static int wolfssl_evp_digest_pk_update(WOLFSSL_EVP_MD_CTX *ctx,
                                        const void *d, unsigned int cnt)
{
    if (ctx->isHMAC) {
        if (wc_HmacUpdate(&ctx->hash.hmac, (const byte *)d, cnt) != 0)
            return WOLFSSL_FAILURE;

        return WOLFSSL_SUCCESS;
    }
    else
        return wolfSSL_EVP_DigestUpdate(ctx, d, cnt);
}

/* Finalize an EVP_DigestSign/Verify operation - common part only.
 * Finalize a digest for RSA and ECC keys, or HMAC for HMAC key.
 * Copies the digest so that you can keep updating.
 */
static int wolfssl_evp_digest_pk_final(WOLFSSL_EVP_MD_CTX *ctx,
                                       unsigned char *md, unsigned int* mdlen)
{
    int  ret;

    if (ctx->isHMAC) {
        Hmac hmacCopy;

        if (wolfSSL_HmacCopy(&hmacCopy, &ctx->hash.hmac) != WOLFSSL_SUCCESS)
            return WOLFSSL_FAILURE;
        ret = wc_HmacFinal(&hmacCopy, md) == 0;
        wc_HmacFree(&hmacCopy);
        return ret;
    }
    else {
        WOLFSSL_EVP_MD_CTX ctxCopy;
        wolfSSL_EVP_MD_CTX_init(&ctxCopy);

        if (wolfSSL_EVP_MD_CTX_copy_ex(&ctxCopy, ctx) != WOLFSSL_SUCCESS)
            return WOLFSSL_FAILURE;

        ret = wolfSSL_EVP_DigestFinal(&ctxCopy, md, mdlen);
        wolfSSL_EVP_MD_CTX_cleanup(&ctxCopy);
        return ret;
    }
}

/* Get the length of the mac based on the digest algorithm. */
static unsigned int wolfssl_mac_len(unsigned char macType)
{
    unsigned int hashLen;

    switch (macType) {
    #ifndef NO_MD5
        case WC_MD5:
            hashLen = WC_MD5_DIGEST_SIZE;
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case WC_SHA:
            hashLen = WC_SHA_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            hashLen = WC_SHA224_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case WC_SHA256:
            hashLen = WC_SHA256_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            hashLen = WC_SHA384_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            hashLen = WC_SHA512_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            hashLen = BLAKE2B_OUTBYTES;
            break;
    #endif /* HAVE_BLAKE2 */

    #ifdef WOLFSSL_SHA3
        #ifndef WOLFSSL_NOSHA3_224
        case WC_SHA3_224:
            hashLen = WC_SHA3_224_DIGEST_SIZE;
            break;
        #endif
        #ifndef WOLFSSL_NOSHA3_256
        case WC_SHA3_256:
            hashLen = WC_SHA3_256_DIGEST_SIZE;
            break;
        #endif
        #ifndef WOLFSSL_NOSHA3_384
        case WC_SHA3_384:
            hashLen = WC_SHA3_384_DIGEST_SIZE;
            break;
        #endif
        #ifndef WOLFSSL_NOSHA3_512
        case WC_SHA3_512:
            hashLen = WC_SHA3_512_DIGEST_SIZE;
            break;
        #endif
    #endif

    #ifdef WOLFSSL_SM3
        case WC_SM3:
            hashLen = WC_SM3_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SM3 */

        default:
            hashLen = 0;
    }

    return hashLen;
}

int wolfSSL_EVP_DigestSignInit(WOLFSSL_EVP_MD_CTX *ctx,
                               WOLFSSL_EVP_PKEY_CTX **pctx,
                               const WOLFSSL_EVP_MD *type,
                               WOLFSSL_ENGINE *e,
                               WOLFSSL_EVP_PKEY *pkey)
{
    WOLFSSL_ENTER("EVP_DigestSignInit");

    if (ctx == NULL || pkey == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_evp_digest_pk_init(ctx, pctx, type, e, pkey);
}


int wolfSSL_EVP_DigestSignUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *d,
                                 unsigned int cnt)
{
    WOLFSSL_ENTER("EVP_DigestSignUpdate");

    if (ctx == NULL || d == NULL)
        return BAD_FUNC_ARG;

    return wolfssl_evp_digest_pk_update(ctx, d, cnt);
}

int wolfSSL_EVP_DigestSignFinal(WOLFSSL_EVP_MD_CTX *ctx, unsigned char *sig,
                                size_t *siglen)
{
    unsigned char digest[WC_MAX_DIGEST_SIZE];
    unsigned int  hashLen;
    int           ret = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("EVP_DigestSignFinal");

    if (ctx == NULL || siglen == NULL)
        return WOLFSSL_FAILURE;

    /* Return the maximum size of the signature when sig is NULL. */
    if (ctx->isHMAC) {
        hashLen = wolfssl_mac_len(ctx->hash.hmac.macType);

        if (sig == NULL) {
            *siglen = hashLen;
            return WOLFSSL_SUCCESS;
        }
    }
#ifndef NO_RSA
    else if (ctx->pctx->pkey->type == EVP_PKEY_RSA) {
        if (sig == NULL) {
            *siglen = (size_t)wolfSSL_RSA_size(ctx->pctx->pkey->rsa);
            return WOLFSSL_SUCCESS;
        }
    }
#endif /* !NO_RSA */
#ifdef HAVE_ECC
    else if (ctx->pctx->pkey->type == EVP_PKEY_EC) {
        if (sig == NULL) {
            /* SEQ + INT + INT */
            *siglen = (size_t)ecc_sets[ctx->pctx->pkey->ecc->group->curve_idx].
                size * 2 + 8;
            return WOLFSSL_SUCCESS;
        }
    }
#endif

    if (wolfssl_evp_digest_pk_final(ctx, digest, &hashLen) <= 0)
        return WOLFSSL_FAILURE;

    if (ctx->isHMAC) {
        /* Copy the HMAC result as signature. */
        if ((unsigned int)(*siglen) > hashLen)
            *siglen = hashLen;
        /* May be a truncated signature. */

        XMEMCPY(sig, digest, (size_t)*siglen);
        ret = WOLFSSL_SUCCESS;
    }
    else {
        /* Sign the digest. */
        switch (ctx->pctx->pkey->type) {
    #if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
        case EVP_PKEY_RSA: {
            unsigned int sigSz = (unsigned int)*siglen;
            int nid;
            const WOLFSSL_EVP_MD *md = wolfSSL_EVP_MD_CTX_md(ctx);
            if (md == NULL)
                break;
            nid = wolfSSL_EVP_MD_type(md);
            if (nid < 0)
                break;
            ret = wolfSSL_RSA_sign_generic_padding(nid, digest, hashLen,
                    sig, &sigSz, ctx->pctx->pkey->rsa, 1, ctx->pctx->padding);
            if (ret >= 0)
                *siglen = sigSz;
            break;
        }
    #endif /* NO_RSA */

    #ifdef HAVE_ECC
        case EVP_PKEY_EC: {
            int len;
            WOLFSSL_ECDSA_SIG *ecdsaSig;
            ecdsaSig = wolfSSL_ECDSA_do_sign(digest, (int)hashLen,
                                             ctx->pctx->pkey->ecc);
            if (ecdsaSig == NULL)
                break;
            len = wolfSSL_i2d_ECDSA_SIG(ecdsaSig, &sig);
            wolfSSL_ECDSA_SIG_free(ecdsaSig);
            if (len == 0)
                break;
            *siglen = (size_t)len;
            ret = WOLFSSL_SUCCESS;
            break;
        }
    #endif
        default:
            break;
        }
    }

    ForceZero(digest, sizeof(digest));
    return ret;
}

int wolfSSL_EVP_DigestVerifyInit(WOLFSSL_EVP_MD_CTX *ctx,
                                 WOLFSSL_EVP_PKEY_CTX **pctx,
                                 const WOLFSSL_EVP_MD *type,
                                 WOLFSSL_ENGINE *e,
                                 WOLFSSL_EVP_PKEY *pkey)
{
    WOLFSSL_ENTER("EVP_DigestVerifyInit");

    if (ctx == NULL || type == NULL || pkey == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_evp_digest_pk_init(ctx, pctx, type, e, pkey);
}


int wolfSSL_EVP_DigestVerifyUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *d,
                                   size_t cnt)
{
    WOLFSSL_ENTER("EVP_DigestVerifyUpdate");

    if (ctx == NULL || d == NULL)
        return BAD_FUNC_ARG;

    return wolfssl_evp_digest_pk_update(ctx, d, (unsigned int)cnt);
}


int wolfSSL_EVP_DigestVerifyFinal(WOLFSSL_EVP_MD_CTX *ctx,
                                  const unsigned char *sig, size_t siglen)
{
    unsigned char digest[WC_MAX_DIGEST_SIZE];
    unsigned int  hashLen;

    WOLFSSL_ENTER("EVP_DigestVerifyFinal");

    if (ctx == NULL || sig == NULL)
        return WOLFSSL_FAILURE;

    if (ctx->isHMAC) {

        hashLen = wolfssl_mac_len(ctx->hash.hmac.macType);

        if (siglen > hashLen)
            return WOLFSSL_FAILURE;
        /* May be a truncated signature. */
    }

    if (wolfssl_evp_digest_pk_final(ctx, digest, &hashLen) <= 0)
        return WOLFSSL_FAILURE;

    if (ctx->isHMAC) {
        /* Check HMAC result matches the signature. */
        if (XMEMCMP(sig, digest, (size_t)siglen) == 0)
            return WOLFSSL_SUCCESS;
        return WOLFSSL_FAILURE;
    }
    else {
        /* Verify the signature with the digest. */
        switch (ctx->pctx->pkey->type) {
    #if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
        case EVP_PKEY_RSA: {
            int nid;
            const WOLFSSL_EVP_MD *md = wolfSSL_EVP_MD_CTX_md(ctx);
            if (md == NULL)
                return WOLFSSL_FAILURE;
            nid = wolfSSL_EVP_MD_type(md);
            if (nid < 0)
                return WOLFSSL_FAILURE;
            return wolfSSL_RSA_verify_ex(nid, digest, hashLen, sig,
                                      (unsigned int)siglen,
                                      ctx->pctx->pkey->rsa, ctx->pctx->padding);
        }
    #endif /* NO_RSA */

    #ifdef HAVE_ECC
        case EVP_PKEY_EC: {
            int ret;
            WOLFSSL_ECDSA_SIG *ecdsaSig;
            ecdsaSig = wolfSSL_d2i_ECDSA_SIG(NULL, &sig, (long)siglen);
            if (ecdsaSig == NULL)
                return WOLFSSL_FAILURE;
            ret = wolfSSL_ECDSA_do_verify(digest, (int)hashLen, ecdsaSig,
                                          ctx->pctx->pkey->ecc);
            wolfSSL_ECDSA_SIG_free(ecdsaSig);
            return ret;
        }
    #endif
        default:
            break;
        }
    }

    return WOLFSSL_FAILURE;
}


#ifdef WOLFSSL_APACHE_HTTPD
#if !defined(USE_WINDOWS_API) && !defined(MICROCHIP_PIC32)
    #include <termios.h>
#endif

#ifndef XGETPASSWD
    static int XGETPASSWD(char* buf, int bufSz) {
        int ret = WOLFSSL_SUCCESS;

        /* turn off echo for passwords */
    #ifdef USE_WINDOWS_API
        DWORD originalTerm;
        DWORD newTerm;
        CONSOLE_SCREEN_BUFFER_INFO screenOrig;
        HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
        if (GetConsoleMode(stdinHandle, &originalTerm) == 0) {
            WOLFSSL_MSG("Couldn't get the original terminal settings");
            return WOLFSSL_FAILURE;
        }
        newTerm = originalTerm;
        newTerm &= ~ENABLE_ECHO_INPUT;
        if (SetConsoleMode(stdinHandle, newTerm) == 0) {
            WOLFSSL_MSG("Couldn't turn off echo");
            return WOLFSSL_FAILURE;
        }
    #else
        struct termios originalTerm;
        struct termios newTerm;
        if (tcgetattr(STDIN_FILENO, &originalTerm) != 0) {
            WOLFSSL_MSG("Couldn't get the original terminal settings");
            return WOLFSSL_FAILURE;
        }
        XMEMCPY(&newTerm, &originalTerm, sizeof(struct termios));

        newTerm.c_lflag &= ~ECHO;
        newTerm.c_lflag |= (ICANON | ECHONL);
        if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) != 0) {
            WOLFSSL_MSG("Couldn't turn off echo");
            return WOLFSSL_FAILURE;
        }
    #endif

        if (XFGETS(buf, bufSz, stdin) == NULL) {
            ret = WOLFSSL_FAILURE;
        }

        /* restore default echo */
    #ifdef USE_WINDOWS_API
        if (SetConsoleMode(stdinHandle, originalTerm) == 0) {
            WOLFSSL_MSG("Couldn't restore the terminal settings");
            return WOLFSSL_FAILURE;
        }
    #else
        if (tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm) != 0) {
            WOLFSSL_MSG("Couldn't restore the terminal settings");
            return WOLFSSL_FAILURE;
        }
    #endif
        return ret;
    }
#endif

/* returns 0 on success and -2 or -1 on failure */
int wolfSSL_EVP_read_pw_string(char* buf, int bufSz, const char* banner, int v)
{
    printf("%s", banner);
    if (XGETPASSWD(buf, bufSz) == WOLFSSL_FAILURE) {
        return -1;
    }
    (void)v; /* fgets always sanity checks size of input vs buffer */
    return 0;
}
#endif /* WOLFSSL_APACHE_HTTPD */

#if !defined(NO_PWDBASED) && !defined(NO_SHA)
int wolfSSL_PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
                                               const unsigned char *salt,
                                               int saltlen, int iter,
                                               int keylen, unsigned char *out)
{
    const char *nostring = "";
    int ret = 0;

    if (pass == NULL) {
        passlen = 0;
        pass = nostring;
    }
    else if (passlen == -1) {
        passlen = (int)XSTRLEN(pass);
    }

    ret = wc_PBKDF2((byte*)out, (byte*)pass, passlen, (byte*)salt, saltlen,
                    iter, keylen, WC_SHA);
    if (ret == 0)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FAILURE;
}
#endif /* !NO_PWDBASED !NO_SHA*/

#if !defined(NO_PWDBASED)
int wolfSSL_PKCS5_PBKDF2_HMAC(const char *pass, int passlen,
                                           const unsigned char *salt,
                                           int saltlen, int iter,
                                           const WOLFSSL_EVP_MD *digest,
                                           int keylen, unsigned char *out)
{
    const char *nostring = "";
    int ret = 0;

    if (pass == NULL) {
        passlen = 0;
        pass = nostring;
    } else if (passlen == -1) {
        passlen = (int)XSTRLEN(pass);
    }

    ret = wc_PBKDF2((byte*)out, (byte*)pass, passlen, (byte*)salt, saltlen,
                    iter, keylen, EvpMd2MacType(digest));
    if (ret == 0)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FAILURE;
}
#endif /* !NO_PWDBASED */


#if defined(HAVE_SCRYPT) && defined(HAVE_PBKDF2) && !defined(NO_PWDBASED) && \
                                                    !defined(NO_SHA256)
/**
 * Derives a key from the specified password and the salt using SCRYPT
 * algorithm.
 *
 * Parameters:
 * - pass      :password data. no need to be null-terminated. NULL is accepted.
 * - passlen   :length of the password. Must be 0 when pass is NULL.
 * - salt      :salt. NULL is accepted.
 * - saltlen   :length of the salt. Must be 0 when salt is NULL.
 * - N         :cost parameter. Must be grater or equal to 2 and be a power of 2.
 * - r         :block size. Must 1 or greater.
 * - p         :parallelism
 * - maxmem    :maximum size of buffer used for calculation in definition,
 *              Not referred in this implementation.
 * - key       :derived key.
 * - keylen    :length of the derived key
 *
 * Returns:
 *   1 on success, otherwise 0.
 */
int wolfSSL_EVP_PBE_scrypt(const char *pass, size_t passlen,
                            const unsigned char *salt, size_t saltlen,
                            word64 N, word64 r, word64 p,
                            word64 maxmem, unsigned char *key, size_t keylen)
{
    int ret;
    int exp = 0;

    (void)maxmem;

    WOLFSSL_ENTER("wolfSSL_EVP_PBE_scrypt");

    if (r > INT32_MAX || p > INT32_MAX) {
        WOLFSSL_MSG("Doesn't support greater than 32 bit values of r and p");
        return WOLFSSL_FAILURE;
    }
    /* N must be a power of 2 and > 2.
       if (N & (N-1)) is zero, it means N is a power of 2.
     */
    if (N < 2 || (N & (N-1)) || r <= 0 || p <= 0)
        return WOLFSSL_FAILURE;

    if (key == NULL)
        return WOLFSSL_SUCCESS;

    /* get exponent of power of 2. Confirmed N is power of 2. */
    while (N != 1) {
        N >>= 1;
        exp++;
    }

    ret = wc_scrypt(key, (const byte*)pass, (int)passlen, salt, (int)saltlen,
                                            exp, (int)r, (int)p, (int)keylen);

    WOLFSSL_LEAVE("wolfSSL_EVP_PBE_scrypt", ret);

    if (ret == 0)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FAILURE;
}
#endif /* HAVE_SCRYPT && HAVE_PBKDF2 && !NO_PWDBASED && !NO_SHA */

static const struct cipher{
        unsigned char type;
        const char *name;
        int nid;
} cipher_tbl[] = {

#ifndef NO_AES
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    #ifdef WOLFSSL_AES_128
    {AES_128_CBC_TYPE, EVP_AES_128_CBC, NID_aes_128_cbc},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CBC_TYPE, EVP_AES_192_CBC, NID_aes_192_cbc},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CBC_TYPE, EVP_AES_256_CBC, NID_aes_256_cbc},
    #endif
    #endif

    #ifdef WOLFSSL_AES_CFB
    #ifdef WOLFSSL_AES_128
    {AES_128_CFB1_TYPE, EVP_AES_128_CFB1, NID_aes_128_cfb1},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CFB1_TYPE, EVP_AES_192_CFB1, NID_aes_192_cfb1},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CFB1_TYPE, EVP_AES_256_CFB1, NID_aes_256_cfb1},
    #endif

    #ifdef WOLFSSL_AES_128
    {AES_128_CFB8_TYPE, EVP_AES_128_CFB8, NID_aes_128_cfb8},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CFB8_TYPE, EVP_AES_192_CFB8, NID_aes_192_cfb8},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CFB8_TYPE, EVP_AES_256_CFB8, NID_aes_256_cfb8},
    #endif

    #ifdef WOLFSSL_AES_128
    {AES_128_CFB128_TYPE, EVP_AES_128_CFB128, NID_aes_128_cfb128},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CFB128_TYPE, EVP_AES_192_CFB128, NID_aes_192_cfb128},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CFB128_TYPE, EVP_AES_256_CFB128, NID_aes_256_cfb128},
    #endif
    #endif

    #ifdef HAVE_AES_OFB
    #ifdef WOLFSSL_AES_128
    {AES_128_OFB_TYPE, EVP_AES_128_OFB, NID_aes_128_ofb},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_OFB_TYPE, EVP_AES_192_OFB, NID_aes_192_ofb},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_OFB_TYPE, EVP_AES_256_OFB, NID_aes_256_ofb},
    #endif
    #endif

    #ifdef HAVE_AES_XTS
    #ifdef WOLFSSL_AES_128
    {AES_128_XTS_TYPE, EVP_AES_128_XTS, NID_aes_128_xts},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_XTS_TYPE, EVP_AES_256_XTS, NID_aes_256_xts},
    #endif
    #endif

    #ifdef HAVE_AESGCM
    #ifdef WOLFSSL_AES_128
    {AES_128_GCM_TYPE, EVP_AES_128_GCM, NID_aes_128_gcm},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_GCM_TYPE, EVP_AES_192_GCM, NID_aes_192_gcm},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_GCM_TYPE, EVP_AES_256_GCM, NID_aes_256_gcm},
    #endif
    #endif

    #ifdef HAVE_AESCCM
    #ifdef WOLFSSL_AES_128
    {AES_128_CCM_TYPE, EVP_AES_128_CCM, NID_aes_128_ccm},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CCM_TYPE, EVP_AES_192_CCM, NID_aes_192_ccm},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CCM_TYPE, EVP_AES_256_CCM, NID_aes_256_ccm},
    #endif
    #endif

    #ifdef WOLFSSL_AES_COUNTER
    #ifdef WOLFSSL_AES_128
        {AES_128_CTR_TYPE, EVP_AES_128_CTR, NID_aes_128_ctr},
    #endif
    #ifdef WOLFSSL_AES_192
        {AES_192_CTR_TYPE, EVP_AES_192_CTR, NID_aes_192_ctr},
    #endif
    #ifdef WOLFSSL_AES_256
        {AES_256_CTR_TYPE, EVP_AES_256_CTR, NID_aes_256_ctr},
    #endif
    #endif

    #ifdef HAVE_AES_ECB
    #ifdef WOLFSSL_AES_128
        {AES_128_ECB_TYPE, EVP_AES_128_ECB, NID_aes_128_ecb},
    #endif
    #ifdef WOLFSSL_AES_192
        {AES_192_ECB_TYPE, EVP_AES_192_ECB, NID_aes_192_ecb},
    #endif
    #ifdef WOLFSSL_AES_256
        {AES_256_ECB_TYPE, EVP_AES_256_ECB, NID_aes_256_ecb},
    #endif
    #endif
#endif

#ifdef HAVE_ARIA
    {ARIA_128_GCM_TYPE, EVP_ARIA_128_GCM, NID_aria_128_gcm},
    {ARIA_192_GCM_TYPE, EVP_ARIA_192_GCM, NID_aria_192_gcm},
    {ARIA_256_GCM_TYPE, EVP_ARIA_256_GCM, NID_aria_256_gcm},
#endif

#ifndef NO_DES3
    {DES_CBC_TYPE, EVP_DES_CBC, NID_des_cbc},
    {DES_ECB_TYPE, EVP_DES_ECB, NID_des_ecb},

    {DES_EDE3_CBC_TYPE, EVP_DES_EDE3_CBC, NID_des_ede3_cbc},
    {DES_EDE3_ECB_TYPE, EVP_DES_EDE3_ECB, NID_des_ede3_ecb},
#endif

#ifndef NO_RC4
    {ARC4_TYPE, EVP_ARC4, NID_undef},
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    {CHACHA20_POLY1305_TYPE, EVP_CHACHA20_POLY1305, NID_chacha20_poly1305},
#endif

#ifdef HAVE_CHACHA
    {CHACHA20_TYPE, EVP_CHACHA20, NID_chacha20},
#endif

#ifdef WOLFSSL_SM4_ECB
    {SM4_ECB_TYPE, EVP_SM4_ECB, NID_sm4_ecb},
#endif
#ifdef WOLFSSL_SM4_CBC
    {SM4_CBC_TYPE, EVP_SM4_CBC, NID_sm4_cbc},
#endif
#ifdef WOLFSSL_SM4_CTR
    {SM4_CTR_TYPE, EVP_SM4_CTR, NID_sm4_ctr},
#endif
#ifdef WOLFSSL_SM4_GCM
    {SM4_GCM_TYPE, EVP_SM4_GCM, NID_sm4_gcm},
#endif
#ifdef WOLFSSL_SM4_CCM
    {SM4_CCM_TYPE, EVP_SM4_CCM, NID_sm4_ccm},
#endif

    { 0, NULL, 0}
};

/* returns cipher using provided ctx type */
const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_CIPHER_CTX_cipher(
    const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    const struct cipher* c;

    if (!ctx || !ctx->cipherType) {
        return NULL;
    }

    for (c = cipher_tbl; c->type != 0; c++) {
        if (ctx->cipherType == c->type) {
            return wolfSSL_EVP_get_cipherbyname(c->name);
        }
    }

    return NULL;
}

int wolfSSL_EVP_CIPHER_nid(const WOLFSSL_EVP_CIPHER *cipher)
{
    const struct cipher* c;

    if (!cipher) {
        return 0;
    }

    for (c = cipher_tbl; c->type != 0; c++) {
        if (XSTRCMP(cipher, c->name) == 0) {
            return c->nid;
        }
    }

    return 0;
}

const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbyname(const char *name)
{
    const struct alias {
        const char *name;
        const char *alias;
    } cipher_alias_tbl[] = {
#ifndef NO_DES3
        {EVP_DES_CBC, "des"},
        {EVP_DES_ECB, "des-ecb"},
        {EVP_DES_EDE3_CBC, "des3"},
        {EVP_DES_EDE3_CBC, "3des"},
        {EVP_DES_EDE3_ECB, "des-ede3"},
        {EVP_DES_EDE3_ECB, "des-ede3-ecb"},
#endif
#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        #ifdef WOLFSSL_AES_128
            {EVP_AES_128_CBC, "aes128-cbc"},
            {EVP_AES_128_CBC, "aes128"},
        #endif
        #ifdef WOLFSSL_AES_192
            {EVP_AES_192_CBC, "aes192-cbc"},
            {EVP_AES_192_CBC, "aes192"},
        #endif
        #ifdef WOLFSSL_AES_256
            {EVP_AES_256_CBC, "aes256-cbc"},
            {EVP_AES_256_CBC, "aes256"},
        #endif
    #endif
    #ifdef HAVE_AES_ECB
        #ifdef WOLFSSL_AES_128
            {EVP_AES_128_ECB, "aes128-ecb"},
        #endif
        #ifdef WOLFSSL_AES_192
            {EVP_AES_192_ECB, "aes192-ecb"},
        #endif
        #ifdef WOLFSSL_AES_256
            {EVP_AES_256_ECB, "aes256-ecb"},
        #endif
    #endif
    #ifdef HAVE_AESGCM
        #ifdef WOLFSSL_AES_128
            {EVP_AES_128_GCM, "aes-128-gcm"},
            {EVP_AES_128_GCM, "id-aes128-GCM"},
        #endif
        #ifdef WOLFSSL_AES_192
            {EVP_AES_192_GCM, "aes-192-gcm"},
            {EVP_AES_192_GCM, "id-aes192-GCM"},
        #endif
        #ifdef WOLFSSL_AES_256
            {EVP_AES_256_GCM, "aes-256-gcm"},
            {EVP_AES_256_GCM, "id-aes256-GCM"},
        #endif
    #endif
    #ifdef HAVE_AESCCM
        #ifdef WOLFSSL_AES_128
            {EVP_AES_128_CCM, "aes-128-ccm"},
            {EVP_AES_128_CCM, "id-aes128-CCM"},
        #endif
        #ifdef WOLFSSL_AES_192
            {EVP_AES_192_CCM, "aes-192-ccm"},
            {EVP_AES_192_CCM, "id-aes192-CCM"},
        #endif
        #ifdef WOLFSSL_AES_256
            {EVP_AES_256_CCM, "aes-256-ccm"},
            {EVP_AES_256_CCM, "id-aes256-CCM"},
        #endif
    #endif
#endif
#ifdef HAVE_ARIA
        {EVP_ARIA_128_GCM, "aria-128-gcm"},
        {EVP_ARIA_128_GCM, "id-aria128-GCM"},
        {EVP_ARIA_192_GCM, "aria-192-gcm"},
        {EVP_ARIA_192_GCM, "id-aria192-GCM"},
        {EVP_ARIA_256_GCM, "aria-256-gcm"},
        {EVP_ARIA_256_GCM, "id-aria256-GCM"},
#endif
#ifdef WOLFSSL_SM4_EBC
        {EVP_SM4_ECB, "sm4-ecb"},
#endif
#ifdef WOLFSSL_SM4_CBC
        {EVP_SM4_CBC, "sm4"},
        {EVP_SM4_CBC, "sm4-cbc"},
#endif
#ifdef WOLFSSL_SM4_CTR
        {EVP_SM4_CTR, "sm4-ctr"},
#endif
#ifdef WOLFSSL_SM4_GCM
        {EVP_SM4_GCM, "sm4-gcm"},
#endif
#ifdef WOLFSSL_SM4_CCM
        {EVP_SM4_CCM, "sm4-ccm"},
#endif
#ifndef NO_RC4
        {EVP_ARC4, "RC4"},
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        {EVP_CHACHA20_POLY1305, "chacha20-poly1305"},
#endif
#ifdef HAVE_CHACHA
        {EVP_CHACHA20, "chacha20"},
#endif
        { NULL, NULL}
    };

    const struct cipher *ent;
    const struct alias  *al;

    WOLFSSL_ENTER("EVP_get_cipherbyname");

    for (al = cipher_alias_tbl; al->name != NULL; al++) {
        /* Accept any case alternative version of an alias. */
        if (XSTRCASECMP(name, al->alias) == 0) {
            name = al->name;
            break;
        }
    }

    for (ent = cipher_tbl; ent->name != NULL; ent++) {
        /* Accept any case alternative version of name. */
        if (XSTRCASECMP(name, ent->name) == 0) {
            return (WOLFSSL_EVP_CIPHER *)ent->name;
        }
    }

    return NULL;
}

/*
 * return an EVP_CIPHER structure when cipher NID is passed.
 *
 * id  cipher NID
 *
 * return WOLFSSL_EVP_CIPHER
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbynid(int id)
{
    WOLFSSL_ENTER("EVP_get_cipherbynid");

    switch(id) {

#ifndef NO_AES
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_cbc:
            return wolfSSL_EVP_aes_128_cbc();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_cbc:
            return wolfSSL_EVP_aes_192_cbc();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_cbc:
            return wolfSSL_EVP_aes_256_cbc();
        #endif
    #endif
    #ifdef WOLFSSL_AES_COUNTER
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_ctr:
            return wolfSSL_EVP_aes_128_ctr();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_ctr:
            return wolfSSL_EVP_aes_192_ctr();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_ctr:
            return wolfSSL_EVP_aes_256_ctr();
        #endif
    #endif /* WOLFSSL_AES_COUNTER */
    #ifdef HAVE_AES_ECB
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_ecb:
            return wolfSSL_EVP_aes_128_ecb();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_ecb:
            return wolfSSL_EVP_aes_192_ecb();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_ecb:
            return wolfSSL_EVP_aes_256_ecb();
        #endif
    #endif /* HAVE_AES_ECB */
    #ifdef HAVE_AESGCM
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_gcm:
            return wolfSSL_EVP_aes_128_gcm();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_gcm:
            return wolfSSL_EVP_aes_192_gcm();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_gcm:
            return wolfSSL_EVP_aes_256_gcm();
        #endif
    #endif
    #ifdef HAVE_AESCCM
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_ccm:
            return wolfSSL_EVP_aes_128_ccm();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_ccm:
            return wolfSSL_EVP_aes_192_ccm();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_ccm:
            return wolfSSL_EVP_aes_256_ccm();
        #endif
    #endif
#endif

#ifdef HAVE_ARIA
    case NID_aria_128_gcm:
        return wolfSSL_EVP_aria_128_gcm();
    case NID_aria_192_gcm:
        return wolfSSL_EVP_aria_192_gcm();
    case NID_aria_256_gcm:
        return wolfSSL_EVP_aria_256_gcm();
#endif

#ifndef NO_DES3
        case NID_des_cbc:
            return wolfSSL_EVP_des_cbc();
#ifdef WOLFSSL_DES_ECB
        case NID_des_ecb:
            return wolfSSL_EVP_des_ecb();
#endif
        case NID_des_ede3_cbc:
            return wolfSSL_EVP_des_ede3_cbc();
#ifdef WOLFSSL_DES_ECB
        case NID_des_ede3_ecb:
            return wolfSSL_EVP_des_ede3_ecb();
#endif
#endif /*NO_DES3*/

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case NID_chacha20_poly1305:
            return wolfSSL_EVP_chacha20_poly1305();
#endif

#ifdef HAVE_CHACHA
        case NID_chacha20:
            return wolfSSL_EVP_chacha20();
#endif

#ifdef WOLFSSL_SM4_ECB
        case NID_sm4_ecb:
            return wolfSSL_EVP_sm4_ecb();
#endif
#ifdef WOLFSSL_SM4_CBC
        case NID_sm4_cbc:
            return wolfSSL_EVP_sm4_cbc();
#endif
#ifdef WOLFSSL_SM4_CTR
        case NID_sm4_ctr:
            return wolfSSL_EVP_sm4_ctr();
#endif
#ifdef WOLFSSL_SM4_GCM
        case NID_sm4_gcm:
            return wolfSSL_EVP_sm4_gcm();
#endif
#ifdef WOLFSSL_SM4_CCM
        case NID_sm4_ccm:
            return wolfSSL_EVP_sm4_ccm();
#endif

        default:
            WOLFSSL_MSG("Bad cipher id value");
    }

    return NULL;
}

void wolfSSL_EVP_init(void)
{
    /* Does nothing. */
}

    /* returns WOLFSSL_SUCCESS on success */
    int wolfSSL_EVP_MD_CTX_copy(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in)
    {
        return wolfSSL_EVP_MD_CTX_copy_ex(out, in);
    }
    /* Deep copy of EVP_MD hasher
     * return WOLFSSL_SUCCESS on success */
    static int wolfSSL_EVP_MD_Copy_Hasher(WOLFSSL_EVP_MD_CTX* des,
            const WOLFSSL_EVP_MD_CTX* src)
    {
        if (src->isHMAC) {
            return wolfSSL_HmacCopy(&des->hash.hmac, (Hmac*)&src->hash.hmac);
        }
        else {
            int ret;

            switch (src->macType) {
                case WC_HASH_TYPE_MD5:
            #ifndef NO_MD5
                    ret = wc_Md5Copy((wc_Md5*)&src->hash.digest,
                            (wc_Md5*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* !NO_MD5 */
                    break;
                case WC_HASH_TYPE_SHA:
            #ifndef NO_SHA
                    ret = wc_ShaCopy((wc_Sha*)&src->hash.digest,
                            (wc_Sha*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* !NO_SHA */
                    break;
                case WC_HASH_TYPE_SHA224:
            #ifdef WOLFSSL_SHA224
                    ret = wc_Sha224Copy((wc_Sha224*)&src->hash.digest,
                            (wc_Sha224*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* WOLFSSL_SHA224 */
                    break;
                case WC_HASH_TYPE_SHA256:
            #ifndef NO_SHA256
                    ret = wc_Sha256Copy((wc_Sha256*)&src->hash.digest,
                            (wc_Sha256*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* !NO_SHA256 */
                    break;
                case WC_HASH_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                    ret = wc_Sha384Copy((wc_Sha384*)&src->hash.digest,
                            (wc_Sha384*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* WOLFSSL_SHA384 */
                    break;
                case WC_HASH_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                    ret = wc_Sha512Copy((wc_Sha512*)&src->hash.digest,
                        (wc_Sha512*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif /* WOLFSSL_SHA512 */
                    break;
            #ifndef WOLFSSL_NOSHA512_224
                case WC_HASH_TYPE_SHA512_224:
            #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
                defined(WOLFSSL_SHA512)
                    ret = wc_Sha512_224Copy((wc_Sha512*)&src->hash.digest,
                        (wc_Sha512*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
            #endif /* !WOLFSSL_NOSHA512_224 */
            #ifndef WOLFSSL_NOSHA512_256
                case WC_HASH_TYPE_SHA512_256:
            #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
                defined(WOLFSSL_SHA512)
                    ret = wc_Sha512_256Copy((wc_Sha512*)&src->hash.digest,
                        (wc_Sha512*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
            #endif /* !WOLFSSL_NOSHA512_256 */
                case WC_HASH_TYPE_SHA3_224:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
                    ret = wc_Sha3_224_Copy((wc_Sha3*)&src->hash.digest,
                            (wc_Sha3*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_256:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
                    ret = wc_Sha3_256_Copy((wc_Sha3*)&src->hash.digest,
                            (wc_Sha3*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_384:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
                    ret = wc_Sha3_384_Copy((wc_Sha3*)&src->hash.digest,
                            (wc_Sha3*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_512:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
                    ret = wc_Sha3_512_Copy((wc_Sha3*)&src->hash.digest,
                        (wc_Sha3*)&des->hash.digest);
            #else
                    ret = NOT_COMPILED_IN;
            #endif
                    break;
            #ifdef WOLFSSL_SM3
                case WC_HASH_TYPE_SM3:
                    ret = wc_Sm3Copy(&src->hash.digest.sm3,
                        &des->hash.digest.sm3);
                    break;
            #endif
                case WC_HASH_TYPE_NONE:
                case WC_HASH_TYPE_MD2:
                case WC_HASH_TYPE_MD4:
                case WC_HASH_TYPE_MD5_SHA:
                case WC_HASH_TYPE_BLAKE2B:
                case WC_HASH_TYPE_BLAKE2S:
            #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
                case WC_HASH_TYPE_SHAKE128:
            #endif
            #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
                case WC_HASH_TYPE_SHAKE256:
            #endif
                default:
                    ret = BAD_FUNC_ARG;
                    break;
            }
            return ret == 0 ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        }
    }

    /* copies structure in to the structure out
     *
     * returns WOLFSSL_SUCCESS on success */
    int wolfSSL_EVP_MD_CTX_copy_ex(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in)
    {
        if ((out == NULL) || (in == NULL)) return WOLFSSL_FAILURE;
        WOLFSSL_ENTER("EVP_CIPHER_MD_CTX_copy_ex");
        wolfSSL_EVP_MD_CTX_cleanup(out);
        XMEMCPY(out, in, sizeof(WOLFSSL_EVP_MD_CTX));
        if (in->pctx != NULL) {
            out->pctx = wolfSSL_EVP_PKEY_CTX_new(in->pctx->pkey, NULL);
            if (out->pctx == NULL)
                return WOLFSSL_FAILURE;
        }
        return wolfSSL_EVP_MD_Copy_Hasher(out, (WOLFSSL_EVP_MD_CTX*)in);
    }
    #ifndef NO_AES

    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_cbc");
        return EVP_AES_128_CBC;
    }
    #endif /* WOLFSSL_AES_128 */


    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_cbc");
        return EVP_AES_192_CBC;
    }
    #endif /* WOLFSSL_AES_192 */


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_cbc");
        return EVP_AES_256_CBC;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_CBC */

    #ifdef WOLFSSL_AES_CFB
#if !defined(HAVE_SELFTEST) && !defined(HAVE_FIPS)
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb1(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_cfb1");
        return EVP_AES_128_CFB1;
    }
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb1(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_cfb1");
        return EVP_AES_192_CFB1;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb1(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_cfb1");
        return EVP_AES_256_CFB1;
    }
    #endif /* WOLFSSL_AES_256 */

    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb8(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_cfb8");
        return EVP_AES_128_CFB8;
    }
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb8(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_cfb8");
        return EVP_AES_192_CFB8;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb8(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_cfb8");
        return EVP_AES_256_CFB8;
    }
    #endif /* WOLFSSL_AES_256 */
#endif /* !HAVE_SELFTEST && !HAVE_FIPS */

    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cfb128(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_cfb128");
        return EVP_AES_128_CFB128;
    }
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cfb128(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_cfb128");
        return EVP_AES_192_CFB128;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cfb128(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_cfb128");
        return EVP_AES_256_CFB128;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* WOLFSSL_AES_CFB */

    #ifdef WOLFSSL_AES_OFB
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ofb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ofb");
        return EVP_AES_128_OFB;
    }
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ofb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ofb");
        return EVP_AES_192_OFB;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ofb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ofb");
        return EVP_AES_256_OFB;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* WOLFSSL_AES_OFB */

    #ifdef WOLFSSL_AES_XTS
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_xts(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_xts");
        return EVP_AES_128_XTS;
    }
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_xts(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_xts");
        return EVP_AES_256_XTS;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* WOLFSSL_AES_XTS */

    #ifdef HAVE_AESGCM
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_gcm");
        return EVP_AES_128_GCM;
    }
    #endif /* WOLFSSL_GCM_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_gcm");
        return EVP_AES_192_GCM;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_gcm");
        return EVP_AES_256_GCM;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AESGCM */

    #ifdef HAVE_AESCCM
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ccm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ccm");
        return EVP_AES_128_CCM;
    }
    #endif /* WOLFSSL_CCM_128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ccm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ccm");
        return EVP_AES_192_CCM;
    }
    #endif /* WOLFSSL_AES_192 */

    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ccm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ccm");
        return EVP_AES_256_CCM;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AESCCM */

    #ifdef WOLFSSL_AES_COUNTER
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ctr");
        return EVP_AES_128_CTR;
    }
    #endif /* WOLFSSL_AES_2128 */

    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ctr");
        return EVP_AES_192_CTR;
    }
    #endif /* WOLFSSL_AES_192 */


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ctr");
        return EVP_AES_256_CTR;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* WOLFSSL_AES_COUNTER */

    #ifdef HAVE_AES_ECB
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ecb");
        return EVP_AES_128_ECB;
    }
    #endif /* WOLFSSL_AES_128 */


    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ecb");
        return EVP_AES_192_ECB;
    }
    #endif /* WOLFSSL_AES_192*/


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ecb");
        return EVP_AES_256_ECB;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_ECB */
    #endif /* NO_AES */

#ifdef HAVE_ARIA
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_128_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aria_128_gcm");
        return EVP_ARIA_128_GCM;
    }
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_192_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aria_192_gcm");
        return EVP_ARIA_192_GCM;
    }
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aria_256_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aria_256_gcm");
        return EVP_ARIA_256_GCM;
    }
#endif /* HAVE_ARIA */

#ifndef NO_DES3
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_cbc");
        return EVP_DES_CBC;
    }
#ifdef WOLFSSL_DES_ECB
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ecb");
        return EVP_DES_ECB;
    }
#endif
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ede3_cbc");
        return EVP_DES_EDE3_CBC;
    }
#ifdef WOLFSSL_DES_ECB
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ede3_ecb");
        return EVP_DES_EDE3_ECB;
    }
#endif
#endif /* NO_DES3 */

#ifndef NO_RC4
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc4(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_rc4");
        return EVP_ARC4;
    }
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_chacha20_poly1305(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_chacha20_poly1305");
        return EVP_CHACHA20_POLY1305;
    }
#endif

#ifdef HAVE_CHACHA
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_chacha20(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_chacha20");
        return EVP_CHACHA20;
    }
#endif

#ifdef WOLFSSL_SM4_ECB
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_sm4_ecb");
        return EVP_SM4_ECB;
    }
#endif
#ifdef WOLFSSL_SM4_CBC
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_sm4_cbc");
        return EVP_SM4_CBC;
    }
#endif
#ifdef WOLFSSL_SM4_CTR
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_sm4_ctr");
        return EVP_SM4_CTR;
    }
#endif
#ifdef WOLFSSL_SM4_GCM
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_gcm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_sm4_gcm");
        return EVP_SM4_GCM;
    }
#endif
#ifdef WOLFSSL_SM4_CCM
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_sm4_ccm(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_sm4_ccm");
        return EVP_SM4_CCM;
    }
#endif

    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_enc_null(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_enc_null");
        return EVP_NULL;
    }
    void wolfSSL_EVP_CIPHER_CTX_init(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_init");
        if (ctx) {
            XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_CIPHER_CTX));
            ctx->cipherType = WOLFSSL_EVP_CIPH_TYPE_INIT;   /* not yet initialized */
            ctx->keyLen     = 0;
            ctx->enc        = 1;      /* start in encrypt mode */
        }
    }

    /* This function allows cipher specific parameters to be
    determined and set. */
    int wolfSSL_EVP_CIPHER_CTX_ctrl(WOLFSSL_EVP_CIPHER_CTX *ctx, int type, \
                                    int arg, void *ptr)
    {
        int ret = WOLFSSL_FAILURE;
#if defined(HAVE_AESGCM) || (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
#ifndef WC_NO_RNG
        WC_RNG rng;
#endif
#endif
        if (ctx == NULL)
            return WOLFSSL_FAILURE;

        (void)arg;
        (void)ptr;

        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_ctrl");

        switch(type) {
            case EVP_CTRL_INIT:
                wolfSSL_EVP_CIPHER_CTX_init(ctx);
                if(ctx)
                    ret = WOLFSSL_SUCCESS;
                break;
            case EVP_CTRL_SET_KEY_LENGTH:
                ret = wolfSSL_EVP_CIPHER_CTX_set_key_length(ctx, arg);
                break;
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || defined(HAVE_ARIA) || \
        defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM) || \
        (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
            case EVP_CTRL_AEAD_SET_IVLEN:
                if ((ctx->flags & WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER) == 0)
                    break;
            #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
                if (ctx->cipherType == CHACHA20_POLY1305_TYPE) {
                    if (arg != CHACHA20_POLY1305_AEAD_IV_SIZE) {
                        break;
                    }
                }
                else
            #endif /* HAVE_CHACHA && HAVE_POLY1305 */
            #if defined(WOLFSSL_SM4_GCM)
                if (ctx->cipherType == SM4_GCM_TYPE) {
                    if (arg <= 0 || arg > SM4_BLOCK_SIZE) {
                        break;
                    }
                }
                else
            #endif
            #if defined(WOLFSSL_SM4_CCM)
                if (ctx->cipherType == SM4_CCM_TYPE) {
                    if (arg <= 0 || arg > SM4_BLOCK_SIZE) {
                        break;
                    }
                }
                else
            #endif
                {
                    if (arg <= 0 || arg > AES_BLOCK_SIZE)
                        break;
                }
                ret = wolfSSL_EVP_CIPHER_CTX_set_iv_length(ctx, arg);
                break;

#if defined(HAVE_AESGCM) || defined(WOLFSSL_SM4_GCM) || \
    (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
            case EVP_CTRL_AEAD_SET_IV_FIXED:
                if ((ctx->flags & WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER) == 0)
                    break;
                if (arg == -1) {
                    /* arg == -1 copies ctx->ivSz from ptr */
                    ret = wolfSSL_EVP_CIPHER_CTX_set_iv(ctx, (byte*)ptr, ctx->ivSz);
                }
#ifndef WC_NO_RNG
                else {
                    /*
                     * Fixed field must be at least 4 bytes and invocation
                     * field at least 8.
                     */
                    if ((arg < 4) || (ctx->ivSz - arg) < 8) {
                        WOLFSSL_MSG("Fixed field or invocation field too short");
                        break;
                    }
                    /* arg is 4...(ctx->ivSz - 8) */
                    XMEMCPY(ctx->iv, ptr, (size_t)arg);
                    if (wc_InitRng(&rng) != 0) {
                        WOLFSSL_MSG("wc_InitRng failed");
                        break;
                    }
                    if (wc_RNG_GenerateBlock(&rng, ctx->iv + arg,
                            (word32)(ctx->ivSz - arg)) == 0) {
                        ret = WOLFSSL_SUCCESS;
                    } else {
                        /* rng is freed immediately after if block so no need
                         * to do it here
                         */
                        WOLFSSL_MSG("wc_RNG_GenerateBlock failed");
                    }
                    if (wc_FreeRng(&rng) != 0) {
                        WOLFSSL_MSG("wc_FreeRng failed");
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
            #if defined(HAVE_AESGCM) || defined(WOLFSSL_SM4_GCM)
                if (ret == WOLFSSL_SUCCESS) {
                    /*
                     * OpenSSL requires that a EVP_CTRL_AEAD_SET_IV_FIXED
                     * command be issued before a EVP_CTRL_GCM_IV_GEN command.
                     * This flag is used to enforce that.
                     */
                    ctx->authIvGenEnable = 1;
                }
            #endif
#endif /* !WC_NO_RNG */
                break;
#endif /* HAVE_AESGCM || WOLFSSL_SM4_GCM || (HAVE_CHACHA && HAVE_POLY1305) */
#if (defined(HAVE_AESGCM) || defined(WOLFSSL_SM4_GCM)) && !defined(_WIN32) && \
    !defined(HAVE_SELFTEST) && (!defined(HAVE_FIPS) || FIPS_VERSION_GE(2,0))
            /*
             * Using EVP_CTRL_GCM_IV_GEN is a way to do AES-GCM encrypt/decrypt
             * multiple times with EVP_Cipher without having to call
             * EVP_CipherInit between each iteration. The IV is incremented for
             * each subsequent EVP_Cipher call to prevent IV reuse.
             */
            case EVP_CTRL_GCM_IV_GEN:
                if ((ctx->flags & WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER) == 0)
                    break;
                if (!ctx->authIvGenEnable) {
                    WOLFSSL_MSG("Must use EVP_CTRL_AEAD_SET_IV_FIXED before "
                                "EVP_CTRL_GCM_IV_GEN");
                    break;
                }
                if (ctx->cipher.aes.keylen == 0 || ctx->ivSz == 0) {
                    WOLFSSL_MSG("Key or IV not set");
                    break;
                }
                if (ptr == NULL) {
                    WOLFSSL_MSG("Destination buffer for IV bytes NULL.");
                    break;
                }
                if (arg <= 0 || arg > ctx->ivSz) {
                    XMEMCPY(ptr, ctx->iv, (size_t)ctx->ivSz);
                }
                else {
                    /*
                     * Copy the last "arg" bytes of ctx->iv into the buffer at
                     * "ptr." Not sure why OpenSSL does this, but it does.
                     */
                    XMEMCPY(ptr, ctx->iv + ctx->ivSz - arg, (size_t)arg);
                }

                /*
                 * The gcmIncIV flag indicates that the IV should be incremented
                 * after the next cipher operation.
                 */
                ctx->authIncIv = 1;
                ret = WOLFSSL_SUCCESS;
                break;
#endif /* (HAVE_AESGCM || WOLFSSL_SM4_GCM) && !_WIN32 && !HAVE_SELFTEST &&
        * !HAVE_FIPS || FIPS_VERSION >= 2)*/
            case EVP_CTRL_AEAD_SET_TAG:
                if ((ctx->flags & WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER) == 0)
                    break;
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
                if (ctx->cipherType == CHACHA20_POLY1305_TYPE) {
                    if (arg != CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE) {
                        break;
                    }
                    ctx->authTagSz = arg;
                    ret = WOLFSSL_SUCCESS;
                    if (ptr != NULL) {
                        XMEMCPY(ctx->authTag, ptr, (size_t)arg);
                    }
                    break;
                }
                else
#endif /* HAVE_CHACHA && HAVE_POLY1305 */
#if defined(WOFLSSL_SM4_GCM)
                if (ctx->cipherType == WOLFSSL_SM4_GCM) {
                    if ((arg <= 0) || (arg > SM4_BLOCK_SIZE) || (ptr == NULL)) {
                        break;
                    }

                    XMEMCPY(ctx->authTag, ptr, (size_t)arg);
                    ctx->authTagSz = arg;
                    ret = WOLFSSL_SUCCESS;
                    break;
                }
                else
#endif
#if defined(WOFLSSL_SM4_CCM)
                if (ctx->cipherType == WOLFSSL_SM4_CCM) {
                    if ((arg <= 0) || (arg > SM4_BLOCK_SIZE) || (ptr == NULL)) {
                        break;
                    }

                    XMEMCPY(ctx->authTag, ptr, (size_t)arg);
                    ctx->authTagSz = arg;
                    ret = WOLFSSL_SUCCESS;
                    break;
                }
                else
#endif
                {
                    if(arg <= 0 || arg > 16 || (ptr == NULL))
                        break;

                    XMEMCPY(ctx->authTag, ptr, (size_t)arg);
                    ctx->authTagSz = arg;
                    ret = WOLFSSL_SUCCESS;
                    break;
                }
            case EVP_CTRL_AEAD_GET_TAG:
                if ((ctx->flags & WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER) == 0)
                    break;

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
                if (ctx->cipherType == CHACHA20_POLY1305_TYPE) {
                    if (arg != CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE) {
                        break;
                    }
                }
                else
#endif /* HAVE_CHACHA && HAVE_POLY1305 */
#if defined(WOLFSSL_SM4_GCM)
                if (ctx->cipherType == WOLFSSL_SM4_GCM) {
                    if (arg <= 0 || arg > SM4_BLOCK_SIZE) {
                        break;
                    }
                }
                else
#endif
#if defined(WOLFSSL_SM4_CCM)
                if (ctx->cipherType == WOLFSSL_SM4_CCM) {
                    if (arg <= 0 || arg > SM4_BLOCK_SIZE) {
                        break;
                    }
                }
                else
#endif
                {
                    if (arg <= 0 || arg > AES_BLOCK_SIZE)
                        break;
                }

                if (ptr != NULL) {
                    XMEMCPY(ptr, ctx->authTag, (size_t)arg);
                    ret = WOLFSSL_SUCCESS;
                }
                break;
#endif /* HAVE_AESGCM || HAVE_AESCCM || WOLFSSL_SM4_GCM || WOLFSSL_SM4_CCM ||
        * HAVE_ARIA || (HAVE_CHACHA && HAVE_POLY1305) */
            default:
                WOLFSSL_MSG("EVP_CIPHER_CTX_ctrl operation not yet handled");
                break;
        }
        return ret;
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_cleanup(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        int ret = WOLFSSL_SUCCESS;
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_cleanup");
        if (ctx) {
#if (!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
            switch (ctx->cipherType) {
#if (defined(HAVE_AESGCM) && defined(WOLFSSL_AESGCM_STREAM)) || \
    defined(HAVE_AESCCM) || \
    defined(HAVE_AESCBC) || \
    defined(WOLFSSL_AES_COUNTER) || \
    defined(HAVE_AES_ECB) || \
    defined(HAVE_AES_CFB) || \
    defined(HAVE_AES_OFB) || \
    defined(WOLFSSL_AES_XTS)

    #if defined(HAVE_AESGCM) && defined(WOLFSSL_AESGCM_STREAM)
                case AES_128_GCM_TYPE:
                case AES_192_GCM_TYPE:
                case AES_256_GCM_TYPE:
    #endif /* HAVE_AESGCM && WOLFSSL_AESGCM_STREAM */
    #if defined(HAVE_AESCCM)
                case AES_128_CCM_TYPE:
                case AES_192_CCM_TYPE:
                case AES_256_CCM_TYPE:
    #endif /* HAVE_AESCCM */
    #ifdef HAVE_AESCBC
                case AES_128_CBC_TYPE:
                case AES_192_CBC_TYPE:
                case AES_256_CBC_TYPE:
    #endif
    #ifdef WOLFSSL_AES_COUNTER
                case AES_128_CTR_TYPE:
                case AES_192_CTR_TYPE:
                case AES_256_CTR_TYPE:
    #endif
    #ifdef HAVE_AES_ECB
                case AES_128_ECB_TYPE:
                case AES_192_ECB_TYPE:
                case AES_256_ECB_TYPE:
    #endif
    #ifdef HAVE_AES_CFB
                case AES_128_CFB1_TYPE:
                case AES_192_CFB1_TYPE:
                case AES_256_CFB1_TYPE:
                case AES_128_CFB8_TYPE:
                case AES_192_CFB8_TYPE:
                case AES_256_CFB8_TYPE:
                case AES_128_CFB128_TYPE:
                case AES_192_CFB128_TYPE:
                case AES_256_CFB128_TYPE:
    #endif
    #ifdef HAVE_AES_OFB
                case AES_128_OFB_TYPE:
                case AES_192_OFB_TYPE:
                case AES_256_OFB_TYPE:
    #endif
    #ifdef WOLFSSL_AES_XTS
                case AES_128_XTS_TYPE:
                case AES_256_XTS_TYPE:
    #endif
                    wc_AesFree(&ctx->cipher.aes);
                    break;
#endif /* AES */
    #ifdef HAVE_ARIA
                case ARIA_128_GCM_TYPE:
                case ARIA_192_GCM_TYPE:
                case ARIA_256_GCM_TYPE:
                    {
                        int result = wc_AriaFreeCrypt(&ctx->cipher.aria);
                        if (result != 0) {
                            WOLFSSL_MSG("wc_AriaFreeCrypt failure");
                            ret = result;
                        }
                    }
                    break;
    #endif
            }

#endif /* not FIPS or FIPS v2+ */

#ifdef WOLFSSL_SM4
            switch (ctx->cipherType) {
    #ifdef WOLFSSL_SM4_ECB
                case SM4_ECB_TYPE:
    #endif
    #ifdef WOLFSSL_SM4_CBC
                case SM4_CBC_TYPE:
    #endif
    #ifdef WOLFSSL_SM4_CTR
                case SM4_CTR_TYPE:
    #endif
    #ifdef WOLFSSL_SM4_GCM
                case SM4_GCM_TYPE:
    #endif
    #ifdef WOLFSSL_SM4_CCM
                case SM4_CCM_TYPE:
    #endif
                    wc_Sm4Free(&ctx->cipher.sm4);
            }
#endif

            ctx->cipherType = WOLFSSL_EVP_CIPH_TYPE_INIT;  /* not yet initialized  */
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
            if (ctx->key) {
                ForceZero(ctx->key, (word32)ctx->keyLen);
                XFREE(ctx->key, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->key = NULL;
            }
#endif
            ctx->keyLen     = 0;
#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || defined(HAVE_ARIA) || \
    defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)
            if (ctx->authBuffer) {
                XFREE(ctx->authBuffer, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authBuffer = NULL;
            }
            ctx->authBufferLen = 0;
            if (ctx->authIn) {
                XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authIn = NULL;
            }
            ctx->authInSz = 0;
            ctx->authIvGenEnable = 0;
            ctx->authIncIv = 0;
#endif
        }

        return ret;
    }

    /* Permanent stub for Qt compilation. */
    #if defined(WOLFSSL_QT) && !defined(NO_WOLFSSL_STUB)
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc2_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_rc2_cbc");
        WOLFSSL_STUB("EVP_rc2_cbc");
        return NULL;
    }
    #endif

#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)

    int wolfSSL_EVP_BytesToKey(const WOLFSSL_EVP_CIPHER* type,
                       const WOLFSSL_EVP_MD* md, const byte* salt,
                       const byte* data, int sz, int count, byte* key, byte* iv)
    {
        int ret;
        int hashType = WC_HASH_TYPE_NONE;
    #ifdef WOLFSSL_SMALL_STACK
        EncryptedInfo* info;
    #else
        EncryptedInfo  info[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        info = (EncryptedInfo*)XMALLOC(sizeof(EncryptedInfo), NULL,
                                       DYNAMIC_TYPE_ENCRYPTEDINFO);
        if (info == NULL) {
            WOLFSSL_MSG("malloc failed");
            return WOLFSSL_FAILURE;
        }
    #endif

        XMEMSET(info, 0, sizeof(EncryptedInfo));

        ret = wc_EncryptedInfoGet(info, type);
        if (ret < 0)
            goto end;

        if (data == NULL) {
            ret = (int)info->keySz;
            goto end;
        }

        ret = wolfSSL_EVP_get_hashinfo(md, &hashType, NULL);
        if (ret == WOLFSSL_FAILURE)
            goto end;

        ret = wc_PBKDF1_ex(key, (int)info->keySz, iv, (int)info->ivSz, data, sz,
                           salt, EVP_SALT_SIZE, count, hashType, NULL);
        if (ret == 0)
            ret = (int)info->keySz;

    end:
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
    #endif
        if (ret < 0)
            return 0; /* failure - for compatibility */

        return ret;
    }

#endif /* WOLFSSL_ENCRYPTED_KEYS && !NO_PWDBASED */


#ifndef NO_AES
#if defined(WOLFSSL_AES_128) || defined(WOLFSSL_AES_192) || \
    defined(WOLFSSL_AES_256)
    #define AES_SIZE_ANY
#endif

#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_COUNTER) || \
    defined(HAVE_AES_ECB) || defined(WOLFSSL_AES_CFB) || \
    defined(WOLFSSL_AES_OFB) || defined(WOLFSSL_AES_DIRECT)
    #define AES_SET_KEY
#endif

#if defined(AES_SIZE_ANY) && defined(AES_SET_KEY)
    static int   AesSetKey_ex(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir, int direct)
    {
        int ret;
        /* wc_AesSetKey clear aes.reg if iv == NULL.
           Keep IV for openSSL compatibility */
        if (iv == NULL)
            XMEMCPY((byte *)aes->tmp, (byte *)aes->reg, AES_BLOCK_SIZE);
        if (direct) {
        #if defined(WOLFSSL_AES_DIRECT)
            ret = wc_AesSetKeyDirect(aes, key, len, iv, dir);
        #else
            ret = NOT_COMPILED_IN;
        #endif
        }
        else {
            ret = wc_AesSetKey(aes, key, len, iv, dir);
        }
        if (iv == NULL)
            XMEMCPY((byte *)aes->reg, (byte *)aes->tmp, AES_BLOCK_SIZE);
        return ret;
    }
#endif /* AES_ANY_SIZE && AES_SET_KEY */
#endif /* NO_AES */

#if defined(HAVE_AESGCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
    static int EvpCipherInitAesGCM(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                   const WOLFSSL_EVP_CIPHER* type,
                                   const byte* key, const byte* iv, int enc)
    {
        int ret = WOLFSSL_SUCCESS;

        if (ctx->authIn) {
            XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
            ctx->authIn = NULL;
        }
        ctx->authInSz = 0;

        ctx->block_size = AES_BLOCK_SIZE;
        ctx->authTagSz = AES_BLOCK_SIZE;
        if (ctx->ivSz == 0) {
            ctx->ivSz = GCM_NONCE_MID_SZ;
        }
        ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
        ctx->flags |= WOLFSSL_EVP_CIPH_GCM_MODE |
                      WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
        if (enc == 0 || enc == 1) {
            ctx->enc = enc ? 1 : 0;
        }

    #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_GCM))) {
            WOLFSSL_MSG("EVP_AES_128_GCM");
            ctx->cipherType = AES_128_GCM_TYPE;
            ctx->keyLen = AES_128_KEY_SIZE;
        }
    #endif
    #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_GCM))) {
            WOLFSSL_MSG("EVP_AES_192_GCM");
            ctx->cipherType = AES_192_GCM_TYPE;
            ctx->keyLen = AES_192_KEY_SIZE;
        }
    #endif
    #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_GCM))) {
            WOLFSSL_MSG("EVP_AES_256_GCM");
            ctx->cipherType = AES_256_GCM_TYPE;
            ctx->keyLen = AES_256_KEY_SIZE;
        }
    #endif

    #ifndef WOLFSSL_AESGCM_STREAM
        if (ret == WOLFSSL_SUCCESS && key &&
            wc_AesGcmSetKey(&ctx->cipher.aes, key, ctx->keyLen)) {
            WOLFSSL_MSG("wc_AesGcmSetKey() failed");
            ret = WOLFSSL_FAILURE;
        }
    #endif /* !WOLFSSL_AESGCM_STREAM */
        if (ret == WOLFSSL_SUCCESS && iv &&
            wc_AesGcmSetExtIV(&ctx->cipher.aes, iv, (word32)ctx->ivSz)) {
            WOLFSSL_MSG("wc_AesGcmSetExtIV() failed");
            ret = WOLFSSL_FAILURE;
        }
    #ifdef WOLFSSL_AESGCM_STREAM
        /*
         * Initialize with key and IV if available. wc_AesGcmInit will fail
         * if called with IV only and no key has been set.
         */
        if (ret == WOLFSSL_SUCCESS &&
            (key || (iv && ctx->cipher.aes.gcmKeySet)) &&
            wc_AesGcmInit(&ctx->cipher.aes, key,
                          (key == NULL) ? 0 : (word32)ctx->keyLen, iv,
                          (iv == NULL) ? 0 : (word32)ctx->ivSz) != 0) {
            WOLFSSL_MSG("wc_AesGcmInit() failed");
            ret = WOLFSSL_FAILURE;
        }
    #endif /* WOLFSSL_AESGCM_STREAM */

        /*
         * OpenSSL clears this flag, which permits subsequent use of
         * EVP_CTRL_GCM_IV_GEN, when EVP_CipherInit is called with no key.
         * If a key is provided, the flag retains its value.
         */
        if (ret == WOLFSSL_SUCCESS && key == NULL) {
            ctx->authIvGenEnable = 0;
        }

        return ret;
    }

    static int EvpCipherAesGCM(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* dst,
                               byte* src, word32 len)
    {
        int ret = WOLFSSL_FAILURE;

    #ifndef WOLFSSL_AESGCM_STREAM
        /* No destination means only AAD. */
        if (src != NULL && dst == NULL) {
            ret = wolfSSL_EVP_CipherUpdate_GCM_AAD(ctx, src, len);
        }
        else if (src != NULL && dst != NULL) {
            if (ctx->enc) {
                ret = wc_AesGcmEncrypt(&ctx->cipher.aes, dst, src,
                        len, ctx->iv, ctx->ivSz, ctx->authTag,
                        ctx->authTagSz, ctx->authIn,
                        ctx->authInSz);
            }
            else {
                ret = wc_AesGcmDecrypt(&ctx->cipher.aes, dst, src,
                        len, ctx->iv, ctx->ivSz, ctx->authTag,
                        ctx->authTagSz, ctx->authIn,
                        ctx->authInSz);
            }
            if (ctx->authIncIv) {
                IncCtr((byte*)ctx->cipher.aes.reg,
                       ctx->cipher.aes.nonceSz);
                ctx->authIncIv = 0;
            }
        }
    #else
        /*
         * No need to call wc_AesGcmInit. Should have been called by
         * wolfSSL_EVP_CipherInit.
         */
        /* NULL dst and non-NULL src means only AAD. */
        if (src != NULL && dst == NULL) {
            if (ctx->enc) {
                ret = wc_AesGcmEncryptUpdate(&ctx->cipher.aes, NULL,
                    NULL, 0, src, len);
            }
            else {
                ret = wc_AesGcmDecryptUpdate(&ctx->cipher.aes, NULL,
                    NULL, 0, src, len);
            }
        }
        /* Only plain/cipher text. */
        else if (src != NULL && dst != NULL) {
            if (ctx->enc) {
                ret = wc_AesGcmEncryptUpdate(&ctx->cipher.aes, dst, src,
                    len, NULL, 0);
            }
            else {
                ret = wc_AesGcmDecryptUpdate(&ctx->cipher.aes, dst, src,
                    len, NULL, 0);
            }
        }
        /*
         * src == NULL is analogous to other "final"-type functions
         * (e.g. EVP_CipherFinal). Calculates tag on encrypt
         * and checks tag on decrypt.
         */
        else {
            if (ctx->enc) {
                /* Calculate authentication tag. */
                ret = wc_AesGcmEncryptFinal(&ctx->cipher.aes,
                    ctx->authTag, (word32)ctx->authTagSz);
                /*
                 * wc_AesGcmEncryptFinal increments the IV in
                 * ctx->cipher.aes.reg, so we don't call IncCtr here.
                 */
            }
            else {
                /* Calculate authentication tag and compare. */
                ret = wc_AesGcmDecryptFinal(&ctx->cipher.aes,
                    ctx->authTag, (word32)ctx->authTagSz);
                if (ctx->authIncIv) {
                    IncCtr((byte*)ctx->cipher.aes.reg,
                           ctx->cipher.aes.nonceSz);
                }
            }
            /* Reinitialize for subsequent wolfSSL_EVP_Cipher calls. */
            if (wc_AesGcmInit(&ctx->cipher.aes, NULL, 0,
                              (byte*)ctx->cipher.aes.reg,
                              (word32)ctx->ivSz) != 0) {
                WOLFSSL_MSG("wc_AesGcmInit failed");
                return WOLFSSL_FATAL_ERROR;
            }
            ctx->authIncIv = 0;
        }
    #endif /* WOLFSSL_AESGCM_STREAM */
        if (src == NULL) {
            /*
             * Clear any leftover AAD on final (final is when src is
             * NULL).
             */
            if (ctx->authIn != NULL) {
                XMEMSET(ctx->authIn, 0, (size_t)ctx->authInSz);
            }
            ctx->authInSz = 0;
        }
        if (ret == 0) {
            ret = (int)len;
        }

        return ret;
    }
#endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */

    /* return WOLFSSL_SUCCESS on ok, 0 on failure to match API compatibility */
#if defined(HAVE_AESCCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
    static int EvpCipherInitAesCCM(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                   const WOLFSSL_EVP_CIPHER* type,
                                   const byte* key, const byte* iv, int enc)
    {
        int ret = WOLFSSL_SUCCESS;

        if (ctx->authIn) {
            XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
            ctx->authIn = NULL;
        }
        ctx->authInSz = 0;

        ctx->block_size = AES_BLOCK_SIZE;
        ctx->authTagSz = AES_BLOCK_SIZE;
        if (ctx->ivSz == 0) {
            ctx->ivSz = GCM_NONCE_MID_SZ;
        }
        ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
        ctx->flags |= WOLFSSL_EVP_CIPH_CCM_MODE |
                      WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
        if (enc == 0 || enc == 1) {
            ctx->enc = enc ? 1 : 0;
        }

    #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CCM))) {
            WOLFSSL_MSG("EVP_AES_128_CCM");
            ctx->cipherType = AES_128_CCM_TYPE;
            ctx->keyLen = AES_128_KEY_SIZE;
        }
    #endif
    #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CCM))) {
            WOLFSSL_MSG("EVP_AES_192_CCM");
            ctx->cipherType = AES_192_CCM_TYPE;
            ctx->keyLen = AES_192_KEY_SIZE;
        }
    #endif
    #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CCM))) {
            WOLFSSL_MSG("EVP_AES_256_CCM");
            ctx->cipherType = AES_256_CCM_TYPE;
            ctx->keyLen = AES_256_KEY_SIZE;
        }
    #endif

        if (ret == WOLFSSL_SUCCESS && key &&
            wc_AesCcmSetKey(&ctx->cipher.aes, key, (word32)ctx->keyLen)) {
            WOLFSSL_MSG("wc_AesCcmSetKey() failed");
            ret = WOLFSSL_FAILURE;
        }
        if (ret == WOLFSSL_SUCCESS && iv &&
            wc_AesCcmSetNonce(&ctx->cipher.aes, iv, (word32)ctx->ivSz)) {
            WOLFSSL_MSG("wc_AesCcmSetNonce() failed");
            ret = WOLFSSL_FAILURE;
        }

        /*
         * OpenSSL clears this flag, which permits subsequent use of
         * EVP_CTRL_CCM_IV_GEN, when EVP_CipherInit is called with no key.
         * If a key is provided, the flag retains its value.
         */
        if (ret == WOLFSSL_SUCCESS && key == NULL) {
            ctx->authIvGenEnable = 0;
        }

        return ret;
    }

    static int EvpCipherAesCCM(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* dst,
                               byte* src, word32 len)
    {
        int ret = WOLFSSL_FAILURE;

        /* No destination means only AAD. */
        if (src != NULL && dst == NULL) {
            ret = wolfSSL_EVP_CipherUpdate_CCM_AAD(ctx, src, (int)len);
        }
        else if (src != NULL && dst != NULL) {
            if (ctx->enc) {
                ret = wc_AesCcmEncrypt(&ctx->cipher.aes, dst, src,
                        len, ctx->iv, (word32)ctx->ivSz, ctx->authTag,
                        (word32)ctx->authTagSz, ctx->authIn,
                        (word32)ctx->authInSz);
            }
            else {
                ret = wc_AesCcmDecrypt(&ctx->cipher.aes, dst, src,
                        len, ctx->iv, (word32)ctx->ivSz, ctx->authTag,
                        (word32)ctx->authTagSz, ctx->authIn,
                        (word32)ctx->authInSz);
            }
            if (ctx->authIncIv) {
                IncCtr((byte*)ctx->cipher.aes.reg,
                       ctx->cipher.aes.nonceSz);
                ctx->authIncIv = 0;
            }
        }
        if (src == NULL) {
            /*
             * Clear any leftover AAD on final (final is when src is
             * NULL).
             */
            if (ctx->authIn != NULL) {
                XMEMSET(ctx->authIn, 0, (size_t)ctx->authInSz);
            }
            ctx->authInSz = 0;
        }
        if (ret == 0) {
            ret = (int)len;
        }

        return ret;
    }
#endif /* HAVE_AESCCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */

#if defined(HAVE_ARIA) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
    static int EvpCipherInitAriaGCM(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    const byte* key, const byte* iv, int enc)
    {
        int ret = WOLFSSL_SUCCESS;

        if (ctx->cipherType == ARIA_128_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_128_GCM))) {
            WOLFSSL_MSG("EVP_ARIA_128_GCM");
            ctx->cipherType = ARIA_128_GCM_TYPE;
            ctx->keyLen = ARIA_128_KEY_SIZE;
        } else if (ctx->cipherType == ARIA_192_GCM_TYPE ||
                   (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_192_GCM))) {
            WOLFSSL_MSG("EVP_ARIA_192_GCM");
            ctx->cipherType = ARIA_192_GCM_TYPE;
            ctx->keyLen = ARIA_192_KEY_SIZE;
        } else if (ctx->cipherType == ARIA_256_GCM_TYPE ||
                   (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_256_GCM))) {
            WOLFSSL_MSG("EVP_ARIA_256_GCM");
            ctx->cipherType = ARIA_256_GCM_TYPE;
            ctx->keyLen = ARIA_256_KEY_SIZE;
        } else {
            WOLFSSL_MSG("Unrecognized cipher type");
            return WOLFSSL_FAILURE;
        }

        if (ctx->authIn) {
            XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
            ctx->authIn = NULL;
        }
        ctx->authInSz = 0;

        ctx->block_size = AES_BLOCK_SIZE;
        ctx->authTagSz = AES_BLOCK_SIZE;
        if (ctx->ivSz == 0) {
            ctx->ivSz = GCM_NONCE_MID_SZ;
        }
        ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
        ctx->flags |= WOLFSSL_EVP_CIPH_GCM_MODE |
                      WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
        if (enc == 0 || enc == 1) {
            ctx->enc = enc ? 1 : 0;
        }

        switch(ctx->cipherType) {
            case ARIA_128_GCM_TYPE:
                ret = wc_AriaInitCrypt(&ctx->cipher.aria, MC_ALGID_ARIA_128BITKEY);
                break;
            case ARIA_192_GCM_TYPE:
                ret = wc_AriaInitCrypt(&ctx->cipher.aria, MC_ALGID_ARIA_192BITKEY);
                break;
            case ARIA_256_GCM_TYPE:
                ret = wc_AriaInitCrypt(&ctx->cipher.aria, MC_ALGID_ARIA_256BITKEY);
                break;
            default:
                WOLFSSL_MSG("Not implemented cipherType");
                return WOLFSSL_NOT_IMPLEMENTED; /* This should never happen */
        }
        if (ret != 0) {
            WOLFSSL_MSG(MC_GetErrorString(ret));
            WOLFSSL_MSG(MC_GetError(ctx->cipher.aria.hSession));
            return WOLFSSL_FAILURE;
        }

        if (key && wc_AriaSetKey(&ctx->cipher.aria, (byte *)key)) {
            WOLFSSL_MSG("wc_AriaSetKey() failed");
            return WOLFSSL_FAILURE;
        }
        if (iv && wc_AriaGcmSetExtIV(&ctx->cipher.aria, iv, ctx->ivSz)) {
            WOLFSSL_MSG("wc_AriaGcmSetIV() failed");
            return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }
#endif /* HAVE_ARIA && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */

    /* return WOLFSSL_SUCCESS on ok, 0 on failure to match API compatibility */
    int wolfSSL_EVP_CipherInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                               const WOLFSSL_EVP_CIPHER* type, const byte* key,
                               const byte* iv, int enc)
    {
        int ret = 0;
        (void)key;
        (void)iv;
        (void)enc;

        WOLFSSL_ENTER("wolfSSL_EVP_CipherInit");
        if (ctx == NULL) {
            WOLFSSL_MSG("no ctx");
            return WOLFSSL_FAILURE;
        }

        if (type == NULL && ctx->cipherType == WOLFSSL_EVP_CIPH_TYPE_INIT) {
            WOLFSSL_MSG("no type set");
            return WOLFSSL_FAILURE;
        }
        if (ctx->cipherType == WOLFSSL_EVP_CIPH_TYPE_INIT){
            /* only first EVP_CipherInit invoke. ctx->cipherType is set below */
            XMEMSET(&ctx->cipher, 0, sizeof(ctx->cipher));
            ctx->flags   = 0;
        }
        /* always clear buffer state */
        ctx->bufUsed = 0;
        ctx->lastUsed = 0;

#ifdef HAVE_WOLFSSL_EVP_CIPHER_CTX_IV
        if (!iv && ctx->ivSz) {
            iv = ctx->iv;
        }
#endif

#ifndef NO_AES
    #if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CBC_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CBC))) {
            WOLFSSL_MSG("EVP_AES_128_CBC");
            ctx->cipherType = AES_128_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = AES_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                            iv, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CBC_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CBC))) {
            WOLFSSL_MSG("EVP_AES_192_CBC");
            ctx->cipherType = AES_192_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = AES_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                            iv, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CBC_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CBC))) {
            WOLFSSL_MSG("EVP_AES_256_CBC");
            ctx->cipherType = AES_256_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = AES_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                            iv, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 0);
                if (ret != 0){
                    WOLFSSL_MSG("AesSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */
    #if defined(HAVE_AESGCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
        || FIPS_VERSION_GE(2,0))
        if (FALSE
        #ifdef WOLFSSL_AES_128
            || ctx->cipherType == AES_128_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_GCM))
        #endif
        #ifdef WOLFSSL_AES_192
            || ctx->cipherType == AES_192_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_GCM))
        #endif
        #ifdef WOLFSSL_AES_256
            || ctx->cipherType == AES_256_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_GCM))
        #endif
          ) {
            if (EvpCipherInitAesGCM(ctx, type, key, iv, enc)
                != WOLFSSL_SUCCESS) {
                return WOLFSSL_FAILURE;
            }
        }
    #endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
            * HAVE_FIPS_VERSION >= 2 */
    #if defined(HAVE_AESCCM) && \
        ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
            || FIPS_VERSION_GE(2,0))
        if (FALSE
        #ifdef WOLFSSL_AES_128
            || ctx->cipherType == AES_128_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CCM))
        #endif
        #ifdef WOLFSSL_AES_192
            || ctx->cipherType == AES_192_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CCM))
        #endif
        #ifdef WOLFSSL_AES_256
            || ctx->cipherType == AES_256_CCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CCM))
        #endif
          ) {
            if (EvpCipherInitAesCCM(ctx, type, key, iv, enc)
                != WOLFSSL_SUCCESS) {
                return WOLFSSL_FAILURE;
            }
        }
    #endif /* HAVE_AESCCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
            * HAVE_FIPS_VERSION >= 2 */
#ifdef WOLFSSL_AES_COUNTER
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CTR_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CTR))) {
            WOLFSSL_MSG("EVP_AES_128_CTR");
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->cipherType = AES_128_CTR_TYPE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB)
            ctx->cipher.aes.left = 0;
#endif
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret =  AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 1);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CTR_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CTR))) {
            WOLFSSL_MSG("EVP_AES_192_CTR");
            ctx->cipherType = AES_192_CTR_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB)
            ctx->cipher.aes.left = 0;
#endif
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 1);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CTR_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CTR))) {
            WOLFSSL_MSG("EVP_AES_256_CTR");
            ctx->cipherType = AES_256_CTR_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->ivSz       = AES_BLOCK_SIZE;
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB)
            ctx->cipher.aes.left = 0;
#endif
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 1);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_256 */
#endif /* WOLFSSL_AES_COUNTER */
    #ifdef HAVE_AES_ECB
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_ECB_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_ECB))) {
            WOLFSSL_MSG("EVP_AES_128_ECB");
            ctx->cipherType = AES_128_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    NULL, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 1);
            }
            if (ret != 0)
                return WOLFSSL_FAILURE;
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_ECB_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_ECB))) {
            WOLFSSL_MSG("EVP_AES_192_ECB");
            ctx->cipherType = AES_192_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    NULL, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 1);
            }
            if (ret != 0)
                return WOLFSSL_FAILURE;
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_ECB_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_ECB))) {
            WOLFSSL_MSG("EVP_AES_256_ECB");
            ctx->cipherType = AES_256_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    NULL, ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, 1);
            }
            if (ret != 0)
                return WOLFSSL_FAILURE;
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_ECB */
    #ifdef WOLFSSL_AES_CFB
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CFB1_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CFB1))) {
            WOLFSSL_MSG("EVP_AES_128_CFB1");
            ctx->cipherType = AES_128_CFB1_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CFB1_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CFB1))) {
            WOLFSSL_MSG("EVP_AES_192_CFB1");
            ctx->cipherType = AES_192_CFB1_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CFB1_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CFB1))) {
            WOLFSSL_MSG("EVP_AES_256_CFB1");
            ctx->cipherType = AES_256_CFB1_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0){
                    WOLFSSL_MSG("AesSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CFB8_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CFB8))) {
            WOLFSSL_MSG("EVP_AES_128_CFB8");
            ctx->cipherType = AES_128_CFB8_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CFB8_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CFB8))) {
            WOLFSSL_MSG("EVP_AES_192_CFB8");
            ctx->cipherType = AES_192_CFB8_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CFB8_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CFB8))) {
            WOLFSSL_MSG("EVP_AES_256_CFB8");
            ctx->cipherType = AES_256_CFB8_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0){
                    WOLFSSL_MSG("AesSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CFB128_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_CFB128))) {
            WOLFSSL_MSG("EVP_AES_128_CFB128");
            ctx->cipherType = AES_128_CFB128_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CFB128_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_CFB128))) {
            WOLFSSL_MSG("EVP_AES_192_CFB128");
            ctx->cipherType = AES_192_CFB128_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CFB128_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_CFB128))) {
            WOLFSSL_MSG("EVP_AES_256_CFB128");
            ctx->cipherType = AES_256_CFB128_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CFB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0){
                    WOLFSSL_MSG("AesSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* WOLFSSL_AES_CFB */
    #ifdef WOLFSSL_AES_OFB
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_OFB_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_OFB))) {
            WOLFSSL_MSG("EVP_AES_128_OFB");
            ctx->cipherType = AES_128_OFB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_OFB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_OFB_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_192_OFB))) {
            WOLFSSL_MSG("EVP_AES_192_OFB");
            ctx->cipherType = AES_192_OFB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_OFB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_OFB_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_OFB))) {
            WOLFSSL_MSG("EVP_AES_256_OFB");
            ctx->cipherType = AES_256_OFB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_OFB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = 1;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = AesSetKey_ex(&ctx->cipher.aes, key, (word32)ctx->keyLen,
                    iv, AES_ENCRYPTION, 0);
                if (ret != 0){
                    WOLFSSL_MSG("AesSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_OFB */
    #ifdef WOLFSSL_AES_XTS
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_XTS_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_128_XTS))) {
            WOLFSSL_MSG("EVP_AES_128_XTS");
            ctx->cipherType = AES_128_XTS_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_XTS_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = 1;
            ctx->ivSz       = AES_BLOCK_SIZE;

            if (iv != NULL) {
                if (iv != ctx->iv) /* Valgrind error when src == dst */
                    XMEMCPY(ctx->iv, iv, (size_t)ctx->ivSz);
            }
            else
                XMEMSET(ctx->iv, 0, AES_BLOCK_SIZE);

            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesXtsSetKey(&ctx->cipher.xts, key,
                    (word32)ctx->keyLen,
                    ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, NULL, 0);
                if (ret != 0) {
                    WOLFSSL_MSG("wc_AesXtsSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_XTS_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_AES_256_XTS))) {
            WOLFSSL_MSG("EVP_AES_256_XTS");
            ctx->cipherType = AES_256_XTS_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_XTS_MODE;
            ctx->keyLen     = 64;
            ctx->block_size = 1;
            ctx->ivSz       = AES_BLOCK_SIZE;

            if (iv != NULL) {
                if (iv != ctx->iv) /* Valgrind error when src == dst */
                    XMEMCPY(ctx->iv, iv, (size_t)ctx->ivSz);
            }
            else
                XMEMSET(ctx->iv, 0, AES_BLOCK_SIZE);

            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesXtsSetKey(&ctx->cipher.xts, key,
                    (word32)ctx->keyLen,
                    ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION, NULL, 0);
                if (ret != 0) {
                    WOLFSSL_MSG("wc_AesXtsSetKey() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_XTS */
#endif /* NO_AES */
    #if defined(HAVE_ARIA)
        if (ctx->cipherType == ARIA_128_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_128_GCM))
            || ctx->cipherType == ARIA_192_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_192_GCM))
            || ctx->cipherType == ARIA_256_GCM_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARIA_256_GCM))
          ) {
            if (EvpCipherInitAriaGCM(ctx, type, key, iv, enc)
                != WOLFSSL_SUCCESS) {
                return WOLFSSL_FAILURE;
            }
        }
    #endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
            * HAVE_FIPS_VERSION >= 2 */


#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        if (ctx->cipherType == CHACHA20_POLY1305_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_CHACHA20_POLY1305))) {
            WOLFSSL_MSG("EVP_CHACHA20_POLY1305");
            ctx->cipherType = CHACHA20_POLY1305_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
            ctx->keyLen     = CHACHA20_POLY1305_AEAD_KEYSIZE;
            ctx->block_size = CHACHA_CHUNK_BYTES;
            ctx->authTagSz  = CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE;
            ctx->ivSz       = CHACHA20_POLY1305_AEAD_IV_SIZE;
            if (enc == 0 || enc == 1) {
                ctx->enc    = (byte) enc;
            }

            /* wolfSSL_EVP_CipherInit() may be called multiple times to
             * set key or iv alone. A common use case is to set key
             * and then init with another iv again and again after
             * update/finals. We need to preserve the key for those calls
             * since wc_ChaCha20Poly1305_Init() does not. */
            if (key != NULL) {
                if (!ctx->key) {
                    ctx->key = (byte*)XMALLOC((size_t)ctx->keyLen, NULL,
                                              DYNAMIC_TYPE_OPENSSL);
                    if (!ctx->key) {
                        return MEMORY_E;
                    }
                }
                XMEMCPY(ctx->key, key, (size_t)ctx->keyLen);
            }
            if ((ctx->key != NULL && iv != NULL) && wc_ChaCha20Poly1305_Init(
                    &ctx->cipher.chachaPoly, ctx->key, iv, ctx->enc) != 0) {
                WOLFSSL_MSG("wc_ChaCha20Poly1305_Init() failed");
                return WOLFSSL_FAILURE;
            }
        }
#endif
#ifdef HAVE_CHACHA
        if (ctx->cipherType == CHACHA20_TYPE ||
            (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_CHACHA20))) {
            WOLFSSL_MSG("EVP_CHACHA20");
            ctx->cipherType = CHACHA20_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->keyLen     = CHACHA_MAX_KEY_SZ;
            ctx->block_size = 1;
            ctx->ivSz       = WOLFSSL_EVP_CHACHA_IV_BYTES;
            if (enc == 0 || enc == 1) {
                ctx->enc    = (byte) enc;
            }
            if (key != NULL && wc_Chacha_SetKey(&ctx->cipher.chacha, key,
                    (word32)ctx->keyLen) != 0) {
                WOLFSSL_MSG("wc_Chacha_SetKey() failed");
                return WOLFSSL_FAILURE;
            }
            if (iv != NULL) {
                /* a bit silly. chacha takes an iv+counter and internally
                 * combines them to a new iv. EVP is given exactly *one* iv,
                 * so to pass it into chacha, we have to revert that first.
                 * The counter comes first in little-endian */
                word32 counter = (word32)iv[0] + (word32)(iv[1] << 8) +
                    (word32)(iv[2] << 16) + (word32)(iv[3] << 24);
                if (wc_Chacha_SetIV(&ctx->cipher.chacha,
                                    iv + sizeof(counter), counter) != 0) {

                    WOLFSSL_MSG("wc_Chacha_SetIV() failed");
                    return WOLFSSL_FAILURE;
                }
            }
        }
#endif
#ifdef WOLFSSL_SM4_ECB
        if (ctx->cipherType == SM4_ECB_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_SM4_ECB))) {
            WOLFSSL_MSG("EVP_SM4_ECB");
            ctx->cipherType = SM4_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = SM4_KEY_SIZE;
            ctx->block_size = SM4_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Sm4SetKey(&ctx->cipher.sm4, key, ctx->keyLen);
            }
            if (ret != 0) {
                return WOLFSSL_FAILURE;
            }
        }
#endif
#ifdef WOLFSSL_SM4_CBC
        if (ctx->cipherType == SM4_CBC_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_SM4_CBC))) {
            WOLFSSL_MSG("EVP_SM4_CBC");
            ctx->cipherType = SM4_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = SM4_KEY_SIZE;
            ctx->block_size = SM4_BLOCK_SIZE;
            ctx->ivSz       = SM4_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key != NULL) {
                ret = wc_Sm4SetKey(&ctx->cipher.sm4, key, ctx->keyLen);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv != NULL) {
                ret = wc_Sm4SetIV(&ctx->cipher.sm4, iv);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
        }
#endif
#ifdef WOLFSSL_SM4_CTR
        if (ctx->cipherType == SM4_CTR_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_SM4_CTR))) {
            WOLFSSL_MSG("EVP_SM4_CTR");
            ctx->cipherType = SM4_CTR_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = SM4_KEY_SIZE;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->ivSz       = SM4_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key != NULL) {
                ret = wc_Sm4SetKey(&ctx->cipher.sm4, key, ctx->keyLen);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv != NULL) {
                ret = wc_Sm4SetIV(&ctx->cipher.sm4, iv);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
        }
#endif
#ifdef WOLFSSL_SM4_GCM
        if (ctx->cipherType == SM4_GCM_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_SM4_GCM))) {
            WOLFSSL_MSG("EVP_SM4_GCM");
            ctx->cipherType = SM4_GCM_TYPE;
            ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags |= WOLFSSL_EVP_CIPH_GCM_MODE |
                          WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->keyLen     = SM4_KEY_SIZE;
            if (ctx->ivSz == 0) {
                ctx->ivSz = GCM_NONCE_MID_SZ;
            }
            ctx->authTagSz  = SM4_BLOCK_SIZE;
            if (ctx->authIn) {
                XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authIn = NULL;
            }
            ctx->authInSz = 0;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key != NULL) {
                ret = wc_Sm4GcmSetKey(&ctx->cipher.sm4, key, ctx->keyLen);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv != NULL) {
                XMEMCPY(ctx->iv, iv, (size_t)ctx->ivSz);
            }
        }
#endif
#ifdef WOLFSSL_SM4_CCM
        if (ctx->cipherType == SM4_CCM_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_SM4_CCM))) {
            WOLFSSL_MSG("EVP_SM4_CCM");
            ctx->cipherType = SM4_CCM_TYPE;
            ctx->flags &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags |= WOLFSSL_EVP_CIPH_CCM_MODE |
                          WOLFSSL_EVP_CIPH_FLAG_AEAD_CIPHER;
            ctx->block_size = NO_PADDING_BLOCK_SIZE;
            ctx->keyLen     = SM4_KEY_SIZE;
            if (ctx->ivSz == 0) {
                ctx->ivSz = GCM_NONCE_MID_SZ;
            }
            ctx->authTagSz  = SM4_BLOCK_SIZE;
            if (ctx->authIn) {
                XFREE(ctx->authIn, NULL, DYNAMIC_TYPE_OPENSSL);
                ctx->authIn = NULL;
            }
            ctx->authInSz = 0;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key != NULL) {
                ret = wc_Sm4SetKey(&ctx->cipher.sm4, key, ctx->keyLen);
                if (ret != 0) {
                    return WOLFSSL_FAILURE;
                }
            }
            if (iv != NULL) {
                XMEMCPY(ctx->iv, iv, (size_t)ctx->ivSz);
            }
        }
#endif
#ifndef NO_DES3
        if (ctx->cipherType == DES_CBC_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_DES_CBC))) {
            WOLFSSL_MSG("EVP_DES_CBC");
            ctx->cipherType = DES_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 8;
            ctx->block_size = DES_BLOCK_SIZE;
            ctx->ivSz       = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des_SetKey(&ctx->cipher.des, key, iv,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }

            if (iv && key == NULL)
                wc_Des_SetIV(&ctx->cipher.des, iv);
        }
#ifdef WOLFSSL_DES_ECB
        else if (ctx->cipherType == DES_ECB_TYPE ||
                 (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_DES_ECB))) {
            WOLFSSL_MSG("EVP_DES_ECB");
            ctx->cipherType = DES_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 8;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                WOLFSSL_MSG("Des_SetKey");
                ret = wc_Des_SetKey(&ctx->cipher.des, key, NULL,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
#endif
        else if (ctx->cipherType == DES_EDE3_CBC_TYPE ||
                 (type &&
                  EVP_CIPHER_TYPE_MATCHES(type, EVP_DES_EDE3_CBC))) {
            WOLFSSL_MSG("EVP_DES_EDE3_CBC");
            ctx->cipherType = DES_EDE3_CBC_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = DES_BLOCK_SIZE;
            ctx->ivSz       = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des3_SetKey(&ctx->cipher.des3, key, iv,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }

            if (iv && key == NULL) {
                ret = wc_Des3_SetIV(&ctx->cipher.des3, iv);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
        else if (ctx->cipherType == DES_EDE3_ECB_TYPE ||
                 (type &&
                  EVP_CIPHER_TYPE_MATCHES(type, EVP_DES_EDE3_ECB))) {
            WOLFSSL_MSG("EVP_DES_EDE3_ECB");
            ctx->cipherType = DES_EDE3_ECB_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des3_SetKey(&ctx->cipher.des3, key, NULL,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return WOLFSSL_FAILURE;
            }
        }
#endif /* NO_DES3 */
#ifndef NO_RC4
        if (ctx->cipherType == ARC4_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_ARC4))) {
            WOLFSSL_MSG("ARC4");
            ctx->cipherType = ARC4_TYPE;
            ctx->flags     &= (unsigned long)~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_STREAM_CIPHER;
            ctx->block_size = 1;
            if (ctx->keyLen == 0)  /* user may have already set */
                ctx->keyLen = 16;  /* default to 128 */
            if (key)
                wc_Arc4SetKey(&ctx->cipher.arc4, key, (word32)ctx->keyLen);
        }
#endif /* NO_RC4 */
        if (ctx->cipherType == NULL_CIPHER_TYPE ||
                (type && EVP_CIPHER_TYPE_MATCHES(type, EVP_NULL))) {
            WOLFSSL_MSG("NULL cipher");
            ctx->cipherType = NULL_CIPHER_TYPE;
            ctx->keyLen = 0;
            ctx->block_size = 16;
        }
#ifdef HAVE_WOLFSSL_EVP_CIPHER_CTX_IV
        if (iv && iv != ctx->iv) {
            if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
                return WOLFSSL_FAILURE;
            }
        }
#endif
        (void)ret; /* remove warning. If execution reaches this point, ret=0 */
        return WOLFSSL_SUCCESS;
    }


    int wolfSSL_EVP_CIPHER_CTX_nid(const WOLFSSL_EVP_CIPHER_CTX *ctx)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_nid");
        if (ctx == NULL) {
            WOLFSSL_ERROR_MSG("Bad parameters");
            return NID_undef;
        }

        switch (ctx->cipherType) {
#ifndef NO_AES
#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
            case AES_128_CBC_TYPE :
                return NID_aes_128_cbc;
            case AES_192_CBC_TYPE :
                return NID_aes_192_cbc;
            case AES_256_CBC_TYPE :
                return NID_aes_256_cbc;
#endif
#ifdef HAVE_AESGCM
            case AES_128_GCM_TYPE :
                return NID_aes_128_gcm;
            case AES_192_GCM_TYPE :
                return NID_aes_192_gcm;
            case AES_256_GCM_TYPE :
                return NID_aes_256_gcm;
#endif
#ifdef HAVE_AESCCM
            case AES_128_CCM_TYPE :
                return NID_aes_128_ccm;
            case AES_192_CCM_TYPE :
                return NID_aes_192_ccm;
            case AES_256_CCM_TYPE :
                return NID_aes_256_ccm;
#endif
#ifdef HAVE_AES_ECB
            case AES_128_ECB_TYPE :
                return NID_aes_128_ecb;
            case AES_192_ECB_TYPE :
                return NID_aes_192_ecb;
            case AES_256_ECB_TYPE :
                return NID_aes_256_ecb;
#endif
#ifdef WOLFSSL_AES_COUNTER
            case AES_128_CTR_TYPE :
                return NID_aes_128_ctr;
            case AES_192_CTR_TYPE :
                return NID_aes_192_ctr;
            case AES_256_CTR_TYPE :
                return NID_aes_256_ctr;
#endif

#endif /* NO_AES */

#ifdef HAVE_ARIA
            case ARIA_128_GCM_TYPE :
                return NID_aria_128_gcm;
            case ARIA_192_GCM_TYPE :
                return NID_aria_192_gcm;
            case ARIA_256_GCM_TYPE :
                return NID_aria_256_gcm;
#endif

#ifndef NO_DES3
            case DES_CBC_TYPE :
                return NID_des_cbc;

            case DES_EDE3_CBC_TYPE :
                return NID_des_ede3_cbc;
#endif
#ifdef WOLFSSL_DES_ECB
            case DES_ECB_TYPE :
                return NID_des_ecb;
            case DES_EDE3_ECB_TYPE :
                return NID_des_ede3_ecb;
#endif

            case ARC4_TYPE :
                return NID_rc4;

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
            case CHACHA20_POLY1305_TYPE:
                return NID_chacha20_poly1305;
#endif

#ifdef HAVE_CHACHA
            case CHACHA20_TYPE:
                return NID_chacha20;
#endif

#ifdef WOLFSSL_SM4_ECB
            case SM4_ECB_TYPE:
                return NID_sm4_ecb;
#endif

#ifdef WOLFSSL_SM4_CBC
            case SM4_CBC_TYPE:
                return NID_sm4_cbc;
#endif

#ifdef WOLFSSL_SM4_CTR
            case SM4_CTR_TYPE:
                return NID_sm4_ctr;
#endif

#ifdef WOLFSSL_SM4_GCM
            case SM4_GCM_TYPE:
                return NID_sm4_gcm;
#endif

#ifdef WOLFSSL_SM4_CCM
            case SM4_CCM_TYPE:
                return NID_sm4_ccm;
#endif

            case NULL_CIPHER_TYPE :
                WOLFSSL_ERROR_MSG("Null cipher has no NID");
                FALL_THROUGH;
            default:
                return NID_undef;
        }
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_key_length");
        if (ctx)
            return ctx->keyLen;
        else
            return WOLFSSL_FAILURE;
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_set_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                             int keylen)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_set_key_length");
        if (ctx)
            ctx->keyLen = keylen;
        else
            return WOLFSSL_FAILURE;

        return WOLFSSL_SUCCESS;
    }

#ifdef HAVE_WOLFSSL_EVP_CIPHER_CTX_IV
    /* returns WOLFSSL_SUCCESS on success, otherwise returns WOLFSSL_FAILURE */
    int wolfSSL_EVP_CIPHER_CTX_set_iv_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                             int ivLen)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_set_iv_length");
        if (ctx)
            ctx->ivSz= ivLen;
        else
            return WOLFSSL_FAILURE;

        return WOLFSSL_SUCCESS;
    }
#endif

#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM) || \
    (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
    /* returns WOLFSSL_SUCCESS on success, otherwise returns WOLFSSL_FAILURE */
    int wolfSSL_EVP_CIPHER_CTX_set_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* iv,
                                             int ivLen)
    {
        int expectedIvLen;

        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_set_iv");
        if (!ctx || !iv || !ivLen) {
            return WOLFSSL_FAILURE;
        }

        expectedIvLen = wolfSSL_EVP_CIPHER_CTX_iv_length(ctx);

        if (expectedIvLen == 0 || expectedIvLen != ivLen) {
            WOLFSSL_MSG("Wrong ivLen value");
            return WOLFSSL_FAILURE;
        }

        return wolfSSL_EVP_CipherInit(ctx, NULL, NULL, iv, -1);
    }
#endif

#if !defined(NO_AES) || !defined(NO_DES3)
    /* returns WOLFSSL_SUCCESS on success, otherwise returns WOLFSSL_FAILURE */
    int wolfSSL_EVP_CIPHER_CTX_get_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* iv,
                                      int ivLen)
    {
        int expectedIvLen;

        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_get_iv");

        if (ctx == NULL || iv == NULL || ivLen == 0) {
            WOLFSSL_MSG("Bad parameter");
            return WOLFSSL_FAILURE;
        }

        expectedIvLen = wolfSSL_EVP_CIPHER_CTX_iv_length(ctx);
        if (expectedIvLen == 0 || expectedIvLen != ivLen) {
            WOLFSSL_MSG("Wrong ivLen value");
            return WOLFSSL_FAILURE;
        }

        XMEMCPY(iv, ctx->iv, (size_t)ivLen);

        return WOLFSSL_SUCCESS;
    }
#endif /* !NO_AES || !NO_DES3 */

    /* Return length on ok */
    int wolfSSL_EVP_Cipher(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* dst, byte* src,
                           word32 len)
    {
        int ret = WOLFSSL_FAILURE;

        WOLFSSL_ENTER("wolfSSL_EVP_Cipher");

        if (ctx == NULL || ((src == NULL || dst == NULL) &&
            (TRUE
        #ifdef HAVE_AESGCM
            && ctx->cipherType != AES_128_GCM_TYPE &&
             ctx->cipherType != AES_192_GCM_TYPE &&
             ctx->cipherType != AES_256_GCM_TYPE
        #endif
        #ifdef HAVE_AESCCM
            && ctx->cipherType != AES_128_CCM_TYPE &&
             ctx->cipherType != AES_192_CCM_TYPE &&
             ctx->cipherType != AES_256_CCM_TYPE
        #endif
        #ifdef HAVE_ARIA
            && ctx->cipherType != ARIA_128_GCM_TYPE &&
             ctx->cipherType != ARIA_192_GCM_TYPE &&
             ctx->cipherType != ARIA_256_GCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_GCM
            && ctx->cipherType != SM4_GCM_TYPE
        #endif
        #ifdef WOLFSSL_SM4_CCM
            && ctx->cipherType != SM4_CCM_TYPE
        #endif
            ))) {
            WOLFSSL_MSG("Bad argument.");
            return WOLFSSL_FATAL_ERROR;
        }

        if (ctx->cipherType == WOLFSSL_EVP_CIPH_TYPE_INIT) {
            WOLFSSL_MSG("Cipher operation not initialized. Call "
                        "wolfSSL_EVP_CipherInit.");
            return WOLFSSL_FATAL_ERROR;
        }

        switch (ctx->cipherType) {

#ifndef NO_AES
#ifdef HAVE_AES_CBC
            case AES_128_CBC_TYPE :
            case AES_192_CBC_TYPE :
            case AES_256_CBC_TYPE :
                WOLFSSL_MSG("AES CBC");
                if (ctx->enc)
                    ret = wc_AesCbcEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesCbcDecrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);
                break;
#endif /* HAVE_AES_CBC */

#ifdef WOLFSSL_AES_CFB
#if !defined(HAVE_SELFTEST) && !defined(HAVE_FIPS)
            case AES_128_CFB1_TYPE:
            case AES_192_CFB1_TYPE:
            case AES_256_CFB1_TYPE:
                WOLFSSL_MSG("AES CFB1");
                if (ctx->enc)
                    ret = wc_AesCfb1Encrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesCfb1Decrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
            case AES_128_CFB8_TYPE:
            case AES_192_CFB8_TYPE:
            case AES_256_CFB8_TYPE:
                WOLFSSL_MSG("AES CFB8");
                if (ctx->enc)
                    ret = wc_AesCfb8Encrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesCfb8Decrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif /* !HAVE_SELFTEST && !HAVE_FIPS */
            case AES_128_CFB128_TYPE:
            case AES_192_CFB128_TYPE:
            case AES_256_CFB128_TYPE:
                WOLFSSL_MSG("AES CFB128");
                if (ctx->enc)
                    ret = wc_AesCfbEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesCfbDecrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif /* WOLFSSL_AES_CFB */
#if defined(WOLFSSL_AES_OFB)
            case AES_128_OFB_TYPE:
            case AES_192_OFB_TYPE:
            case AES_256_OFB_TYPE:
                WOLFSSL_MSG("AES OFB");
                if (ctx->enc)
                    ret = wc_AesOfbEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesOfbDecrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif /* WOLFSSL_AES_OFB */
#if defined(WOLFSSL_AES_XTS)
            case AES_128_XTS_TYPE:
            case AES_256_XTS_TYPE:
                WOLFSSL_MSG("AES XTS");
                if (ctx->enc)
                    ret = wc_AesXtsEncrypt(&ctx->cipher.xts, dst, src, len,
                            ctx->iv, (word32)ctx->ivSz);
                else
                    ret = wc_AesXtsDecrypt(&ctx->cipher.xts, dst, src, len,
                            ctx->iv, (word32)ctx->ivSz);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif /* WOLFSSL_AES_XTS */

#if defined(HAVE_AESGCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
            case AES_128_GCM_TYPE :
            case AES_192_GCM_TYPE :
            case AES_256_GCM_TYPE :
                WOLFSSL_MSG("AES GCM");
                ret = EvpCipherAesGCM(ctx, dst, src, len);
                break;
#endif /* HAVE_AESGCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */
#if defined(HAVE_AESCCM) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
            case AES_128_CCM_TYPE :
            case AES_192_CCM_TYPE :
            case AES_256_CCM_TYPE :
                WOLFSSL_MSG("AES CCM");
                ret = EvpCipherAesCCM(ctx, dst, src, len);
                break;
#endif /* HAVE_AESCCM && ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */
#ifdef HAVE_AES_ECB
            case AES_128_ECB_TYPE :
            case AES_192_ECB_TYPE :
            case AES_256_ECB_TYPE :
                WOLFSSL_MSG("AES ECB");
                if (ctx->enc)
                    ret = wc_AesEcbEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesEcbDecrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);
                break;
#endif
#ifdef WOLFSSL_AES_COUNTER
            case AES_128_CTR_TYPE :
            case AES_192_CTR_TYPE :
            case AES_256_CTR_TYPE :
                WOLFSSL_MSG("AES CTR");
                ret = wc_AesCtrEncrypt(&ctx->cipher.aes, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif /* WOLFSSL_AES_COUNTER */
#endif /* NO_AES */

#if defined(HAVE_ARIA) && ((!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) \
    || FIPS_VERSION_GE(2,0))
            case ARIA_128_GCM_TYPE :
            case ARIA_192_GCM_TYPE :
            case ARIA_256_GCM_TYPE :
                WOLFSSL_MSG("ARIA GCM");
                if (ctx->enc) {
                    ret = wc_AriaEncrypt(&ctx->cipher.aria, dst, src, len,
                                         ctx->iv, ctx->ivSz, NULL, 0,
                                         ctx->authTag, ctx->authTagSz);
                }
                else {
                    ret = wc_AriaDecrypt(&ctx->cipher.aria, dst, src, len,
                                         ctx->iv, ctx->ivSz, NULL, 0,
                                         ctx->authTag, ctx->authTagSz);
                }
                break;
#endif /* HAVE_ARIA&& ((!HAVE_FIPS && !HAVE_SELFTEST) ||
        * HAVE_FIPS_VERSION >= 2 */

#ifndef NO_DES3
            case DES_CBC_TYPE :
                WOLFSSL_MSG("DES CBC");
                if (ctx->enc)
                    wc_Des_CbcEncrypt(&ctx->cipher.des, dst, src, len);
                else
                    wc_Des_CbcDecrypt(&ctx->cipher.des, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / DES_BLOCK_SIZE) * DES_BLOCK_SIZE);
                break;
            case DES_EDE3_CBC_TYPE :
                WOLFSSL_MSG("DES3 CBC");
                if (ctx->enc)
                    ret = wc_Des3_CbcEncrypt(&ctx->cipher.des3, dst, src, len);
                else
                    ret = wc_Des3_CbcDecrypt(&ctx->cipher.des3, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / DES_BLOCK_SIZE) * DES_BLOCK_SIZE);
                break;
#ifdef WOLFSSL_DES_ECB
            case DES_ECB_TYPE :
                WOLFSSL_MSG("DES ECB");
                ret = wc_Des_EcbEncrypt(&ctx->cipher.des, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / DES_BLOCK_SIZE) * DES_BLOCK_SIZE);
                break;
            case DES_EDE3_ECB_TYPE :
                WOLFSSL_MSG("DES3 ECB");
                ret = wc_Des3_EcbEncrypt(&ctx->cipher.des3, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / DES_BLOCK_SIZE) * DES_BLOCK_SIZE);
                break;
#endif
#endif /* !NO_DES3 */

#ifndef NO_RC4
            case ARC4_TYPE :
                WOLFSSL_MSG("ARC4");
                wc_Arc4Process(&ctx->cipher.arc4, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif

            /* TODO: Chacha??? */

#ifdef WOLFSSL_SM4_ECB
            case SM4_ECB_TYPE :
                WOLFSSL_MSG("Sm4 ECB");
                if (ctx->enc)
                    ret = wc_Sm4EcbEncrypt(&ctx->cipher.sm4, dst, src, len);
                else
                    ret = wc_Sm4EcbDecrypt(&ctx->cipher.sm4, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / SM4_BLOCK_SIZE) * SM4_BLOCK_SIZE);
                break;
#endif
#ifdef WOLFSSL_SM4_CBC
            case SM4_CBC_TYPE :
                WOLFSSL_MSG("Sm4 CBC");
                if (ctx->enc)
                    ret = wc_Sm4CbcEncrypt(&ctx->cipher.sm4, dst, src, len);
                else
                    ret = wc_Sm4CbcDecrypt(&ctx->cipher.sm4, dst, src, len);
                if (ret == 0)
                    ret = (int)((len / SM4_BLOCK_SIZE) * SM4_BLOCK_SIZE);
                break;
#endif
#ifdef WOLFSSL_SM4_CTR
            case SM4_CTR_TYPE :
                WOLFSSL_MSG("AES CTR");
                ret = wc_Sm4CtrEncrypt(&ctx->cipher.sm4, dst, src, len);
                if (ret == 0)
                    ret = (int)len;
                break;
#endif
#ifdef WOLFSSL_SM4_GCM
            case SM4_GCM_TYPE :
                WOLFSSL_MSG("SM4 GCM");
                /* No destination means only AAD. */
                if (src != NULL && dst == NULL) {
                    ret = wolfSSL_EVP_CipherUpdate_GCM_AAD(ctx, src, len);
                }
                else if (src != NULL && dst != NULL) {
                    if (ctx->enc) {
                        ret = wc_Sm4GcmEncrypt(&ctx->cipher.sm4, dst, src,
                                len, ctx->iv, ctx->ivSz, ctx->authTag,
                                ctx->authTagSz, ctx->authIn,
                                ctx->authInSz);
                    }
                    else {
                        ret = wc_Sm4GcmDecrypt(&ctx->cipher.sm4, dst, src,
                                len, ctx->iv, ctx->ivSz, ctx->authTag,
                                ctx->authTagSz, ctx->authIn,
                                ctx->authInSz);
                    }
                    if (ctx->authIncIv) {
                        IncCtr((byte*)ctx->cipher.sm4.iv,
                               ctx->cipher.sm4.nonceSz);
                        ctx->authIncIv = 0;
                    }
                }
                break;
#endif
#ifdef WOLFSSL_SM4_CCM
            case SM4_CCM_TYPE :
                WOLFSSL_MSG("SM4 CCM");
                /* No destination means only AAD. */
                if (src != NULL && dst == NULL) {
                    ret = wolfSSL_EVP_CipherUpdate_CCM_AAD(ctx, src, len);
                }
                else if (src != NULL && dst != NULL) {
                    if (ctx->enc) {
                        ret = wc_Sm4CcmEncrypt(&ctx->cipher.sm4, dst, src,
                                len, ctx->iv, ctx->ivSz, ctx->authTag,
                                ctx->authTagSz, ctx->authIn,
                                ctx->authInSz);
                    }
                    else {
                        ret = wc_Sm4CcmDecrypt(&ctx->cipher.sm4, dst, src,
                                len, ctx->iv, ctx->ivSz, ctx->authTag,
                                ctx->authTagSz, ctx->authIn,
                                ctx->authInSz);
                    }
                    if (ctx->authIncIv) {
                        IncCtr((byte*)ctx->cipher.sm4.iv,
                               ctx->cipher.sm4.nonceSz);
                        ctx->authIncIv = 0;
                    }
                }
                if (src == NULL) {
                    /*
                     * Clear any leftover AAD on final (final is when src is
                     * NULL).
                     */
                    if (ctx->authIn != NULL) {
                        XMEMSET(ctx->authIn, 0, (size_t)ctx->authInSz);
                    }
                    ctx->authInSz = 0;
                }
                if (ret == 0) {
                    ret = (int)len;
                }
                break;
#endif

            case NULL_CIPHER_TYPE :
                WOLFSSL_MSG("NULL CIPHER");
                XMEMCPY(dst, src, (size_t)len);
                ret = (int)len;
                break;

            default: {
                WOLFSSL_MSG("bad type");
                return WOLFSSL_FATAL_ERROR;
            }
        }

        if (ret < 0) {
            if (ret == AES_GCM_AUTH_E) {
                WOLFSSL_MSG("wolfSSL_EVP_Cipher failure: bad AES-GCM tag.");
            }
            WOLFSSL_MSG("wolfSSL_EVP_Cipher failure");
            return WOLFSSL_FATAL_ERROR;
        }

        if (wolfSSL_StoreExternalIV(ctx) != WOLFSSL_SUCCESS) {
            return WOLFSSL_FATAL_ERROR;
        }

        WOLFSSL_MSG("wolfSSL_EVP_Cipher success");
        return ret;
    }

static void clearEVPPkeyKeys(WOLFSSL_EVP_PKEY *pkey)
{
    if(pkey == NULL)
        return;
    WOLFSSL_ENTER("clearEVPPkeyKeys");
#ifndef NO_RSA
    if (pkey->rsa != NULL && pkey->ownRsa == 1) {
        wolfSSL_RSA_free(pkey->rsa);
        pkey->rsa = NULL;
    }
    pkey->ownRsa = 0;
#endif
#ifndef NO_DSA
    if (pkey->dsa != NULL && pkey->ownDsa == 1) {
        wolfSSL_DSA_free(pkey->dsa);
        pkey->dsa = NULL;
    }
    pkey->ownDsa = 0;
#endif
#ifndef NO_DH
    if (pkey->dh != NULL && pkey->ownDh == 1) {
        wolfSSL_DH_free(pkey->dh);
        pkey->dh = NULL;
    }
    pkey->ownDh = 0;
#endif
#ifdef HAVE_ECC
    if (pkey->ecc != NULL && pkey->ownEcc == 1) {
        wolfSSL_EC_KEY_free(pkey->ecc);
        pkey->ecc = NULL;
    }
    pkey->ownEcc = 0;
#endif
}

#ifndef NO_RSA
#if defined(WOLFSSL_KEY_GEN) && !defined(HAVE_USER_RSA)
static int PopulateRSAEvpPkeyDer(WOLFSSL_EVP_PKEY *pkey)
{
    int ret = 0;
    int derSz = 0;
    word32 pkcs8Sz = 0;
    byte* derBuf = NULL;
    RsaKey* rsa = NULL;
    WOLFSSL_RSA *key = NULL;

    if (pkey == NULL || pkey->rsa == NULL || pkey->rsa->internal == NULL) {
        WOLFSSL_MSG("bad parameter");
        return WOLFSSL_FAILURE;
    }

    key = pkey->rsa;
    rsa = (RsaKey*)pkey->rsa->internal;

    /* Get DER size */
    if (rsa->type == RSA_PRIVATE) {
        ret = wc_RsaKeyToDer(rsa, NULL, 0);
        if (ret > 0) {
            derSz = ret;
        #ifdef HAVE_PKCS8
            if (key->pkcs8HeaderSz) {
                ret = wc_CreatePKCS8Key(NULL, &pkcs8Sz, NULL, (word32)derSz,
                    RSAk, NULL, 0);
                if (ret == LENGTH_ONLY_E)
                    ret = 0;
            }
        #endif
        }
    }
    else {
        ret = wc_RsaKeyToPublicDer(rsa, NULL, 0);
        if (ret > 0)
            derSz = ret;
    }

    if (derSz == 0 || ret < 0) {
        WOLFSSL_MSG("Error getting RSA DER size");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_NO_REALLOC
    derBuf = (byte*)XMALLOC((size_t)derSz, pkey->heap, DYNAMIC_TYPE_DER);
    if (derBuf != NULL) {
        XMEMCPY(derBuf, pkey->pkey.ptr, (size_t)pkey->pkey_sz);
        XFREE(pkey->pkey.ptr, pkey->heap, DYNAMIC_TYPE_DER);
        pkey->pkey.ptr = NULL;
    }
#else
    derBuf = (byte*)XREALLOC(pkey->pkey.ptr, (size_t)derSz,
            pkey->heap, DYNAMIC_TYPE_DER);
#endif
    if (derBuf == NULL) {
        WOLFSSL_MSG("PopulateRSAEvpPkeyDer malloc failed");
        return WOLFSSL_FAILURE;
    }

    /* Old pointer is invalid from this point on */
    pkey->pkey.ptr = (char*)derBuf;

    if (rsa->type == RSA_PRIVATE) {
        ret = wc_RsaKeyToDer(rsa, derBuf, (word32)derSz);
        if (ret > 0) {
            derSz = ret;
        #ifdef HAVE_PKCS8
            if (key->pkcs8HeaderSz) {
                byte* keyBuf = derBuf;
                int keySz = derSz;
                word32 sz = pkcs8Sz;
                /* Need new buffer for PKCS8 since we can't
                 * do this in-place */
                derBuf = (byte*)XMALLOC((size_t)pkcs8Sz, pkey->heap,
                    DYNAMIC_TYPE_DER);
                if (derBuf != NULL) {
                    ret = wc_CreatePKCS8Key(derBuf, &sz, keyBuf, (word32)keySz,
                        RSAk, NULL, 0);
                    XFREE(keyBuf, pkey->heap, DYNAMIC_TYPE_DER);
                    pkey->pkey.ptr = (char*)derBuf;
                }
                else {
                    ret = MEMORY_E;
                }
                derSz = (int)sz;
            }
        #endif
        }
    }
    else {
        /* Public key to DER */
        ret = wc_RsaKeyToPublicDer(rsa, derBuf, (word32)derSz);
        if (ret > 0)
            derSz = ret;
    }

    if (ret < 0) {
        WOLFSSL_MSG("PopulateRSAEvpPkeyDer failed");
        return WOLFSSL_FAILURE;
    }
    else {
        pkey->pkey_sz = derSz;
        return WOLFSSL_SUCCESS;
    }
}
#endif

WOLFSSL_RSA* wolfSSL_EVP_PKEY_get0_RSA(WOLFSSL_EVP_PKEY *pkey)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_get0_RSA");

    if (pkey == NULL)
        return NULL;

    return pkey->rsa;
}

WOLFSSL_RSA* wolfSSL_EVP_PKEY_get1_RSA(WOLFSSL_EVP_PKEY* pkey)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_get1_RSA");

    if (pkey == NULL || pkey->rsa == NULL)
        return NULL;

    if (wolfSSL_RSA_up_ref(pkey->rsa) != WOLFSSL_SUCCESS)
        return NULL;

    return pkey->rsa;
}

/* with set1 functions the pkey struct does not own the RSA structure
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_PKEY_set1_RSA(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_RSA *key)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_set1_RSA");
    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    if (wolfSSL_RSA_up_ref(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_RSA_up_ref failed");
        return WOLFSSL_FAILURE;
    }

    clearEVPPkeyKeys(pkey);
    pkey->rsa    = key;
    pkey->ownRsa = 1; /* pkey does not own RSA but needs to call free on it */
    pkey->type   = EVP_PKEY_RSA;
    pkey->pkcs8HeaderSz = key->pkcs8HeaderSz;
    if (key->inSet == 0) {
        if (SetRsaInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

#if defined(WOLFSSL_KEY_GEN) && !defined(HAVE_USER_RSA)
    if (PopulateRSAEvpPkeyDer(pkey) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("PopulateRSAEvpPkeyDer failed");
        return WOLFSSL_FAILURE;
    }
#endif /* WOLFSSL_KEY_GEN && !HAVE_USER_RSA */

#ifdef WC_RSA_BLINDING
    if (key->ownRng == 0) {
        if (wc_RsaSetRNG((RsaKey*)pkey->rsa->internal, &pkey->rng) != 0) {
            WOLFSSL_MSG("Error setting RSA rng");
            return WOLFSSL_FAILURE;
        }
    }
#endif
    return WOLFSSL_SUCCESS;
}
#endif /* !NO_RSA */

#if !defined (NO_DSA) && !defined(HAVE_SELFTEST) && defined(WOLFSSL_KEY_GEN)
/* with set1 functions the pkey struct does not own the DSA structure
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_PKEY_set1_DSA(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_DSA *key)
{
    int derMax = 0;
    int derSz  = 0;
    DsaKey* dsa  = NULL;
    byte* derBuf = NULL;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_set1_DSA");

    if((pkey == NULL) || (key == NULL))return WOLFSSL_FAILURE;
    clearEVPPkeyKeys(pkey);
    pkey->dsa    = key;
    pkey->ownDsa = 0; /* pkey does not own DSA */
    pkey->type   = EVP_PKEY_DSA;
    if (key->inSet == 0) {
        if (SetDsaInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return WOLFSSL_FAILURE;
        }
    }
    dsa = (DsaKey*)key->internal;

    /* 4 > size of pub, priv, p, q, g + ASN.1 additional information */
    derMax = 4 * wolfSSL_BN_num_bytes(key->g) + AES_BLOCK_SIZE;

    derBuf = (byte*)XMALLOC((size_t)derMax, pkey->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (derBuf == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }

    if (dsa->type == DSA_PRIVATE) {
        /* Private key to DER */
        derSz = wc_DsaKeyToDer(dsa, derBuf, (word32)derMax);
    }
    else {
        /* Public key to DER */
        derSz = wc_DsaKeyToPublicDer(dsa, derBuf, (word32)derMax);
    }

    if (derSz < 0) {
        if (dsa->type == DSA_PRIVATE) {
            WOLFSSL_MSG("wc_DsaKeyToDer failed");
        }
        else {
            WOLFSSL_MSG("wc_DsaKeyToPublicDer failed");
        }
        XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    pkey->pkey.ptr = (char*)XMALLOC((size_t)derSz, pkey->heap,
        DYNAMIC_TYPE_DER);
    if (pkey->pkey.ptr == NULL) {
        WOLFSSL_MSG("key malloc failed");
        XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }
    pkey->pkey_sz = derSz;
    XMEMCPY(pkey->pkey.ptr, derBuf, (size_t)derSz);
    XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_TMP_BUFFER);

    return WOLFSSL_SUCCESS;
}

WOLFSSL_DSA* wolfSSL_EVP_PKEY_get0_DSA(struct WOLFSSL_EVP_PKEY *pkey)
{
    if (!pkey) {
        return NULL;
    }
    return pkey->dsa;
}

WOLFSSL_DSA* wolfSSL_EVP_PKEY_get1_DSA(WOLFSSL_EVP_PKEY* key)
{
    WOLFSSL_DSA* local;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_get1_DSA");

    if (key == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return NULL;
    }

    local = wolfSSL_DSA_new();
    if (local == NULL) {
        WOLFSSL_MSG("Error creating a new WOLFSSL_DSA structure");
        return NULL;
    }

    if (key->type == EVP_PKEY_DSA) {
        if (wolfSSL_DSA_LoadDer(local, (const unsigned char*)key->pkey.ptr,
                    key->pkey_sz) != SSL_SUCCESS) {
            /* now try public key */
            if (wolfSSL_DSA_LoadDer_ex(local,
                        (const unsigned char*)key->pkey.ptr, key->pkey_sz,
                        WOLFSSL_DSA_LOAD_PUBLIC) != SSL_SUCCESS) {
                wolfSSL_DSA_free(local);
                local = NULL;
            }
        }
    }
    else {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY does not hold a DSA key");
        wolfSSL_DSA_free(local);
        local = NULL;
    }
    return local;
}
#endif /* !NO_DSA && !HAVE_SELFTEST && WOLFSSL_KEY_GEN */

#ifdef HAVE_ECC
WOLFSSL_EC_KEY *wolfSSL_EVP_PKEY_get0_EC_KEY(WOLFSSL_EVP_PKEY *pkey)
{
    WOLFSSL_EC_KEY *eckey = NULL;
    if (pkey && pkey->type == EVP_PKEY_EC) {
#ifdef HAVE_ECC
        eckey = pkey->ecc;
#endif
    }
    return eckey;
}

WOLFSSL_EC_KEY* wolfSSL_EVP_PKEY_get1_EC_KEY(WOLFSSL_EVP_PKEY* key)
{
    WOLFSSL_EC_KEY* local = NULL;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_get1_EC_KEY");

    if (key == NULL || key->type != EVP_PKEY_EC) {
        return NULL;
    }
    if (key->type == EVP_PKEY_EC) {
        if (key->ecc != NULL) {
            if (wolfSSL_EC_KEY_up_ref(key->ecc) != WOLFSSL_SUCCESS) {
                return NULL;
            }
            local = key->ecc;
        }
        else {
            key->ecc = local = wolfSSL_EC_KEY_new();
            if (local == NULL) {
                WOLFSSL_MSG("Error creating a new WOLFSSL_EC_KEY structure");
                return NULL;
            }
            if (wolfSSL_EC_KEY_LoadDer(local,
                        (const unsigned char*)key->pkey.ptr,
                        key->pkey_sz) != WOLFSSL_SUCCESS) {
                /* now try public key */
                if (wolfSSL_EC_KEY_LoadDer_ex(local,
                        (const unsigned char*)key->pkey.ptr, key->pkey_sz,
                        WOLFSSL_EC_KEY_LOAD_PUBLIC) != WOLFSSL_SUCCESS) {

                    wolfSSL_EC_KEY_free(local);
                    local = NULL;
                }
            }
        }
    }
    else {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY does not hold an EC key");
    }

    return local;
}
#endif /* HAVE_ECC */

#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT) || defined(WOLFSSL_OPENSSH)
#if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA) && !defined(NO_FILESYSTEM)
/* with set1 functions the pkey struct does not own the DH structure
 * Build the following DH Key format from the passed in WOLFSSL_DH
 * then store in WOLFSSL_EVP_PKEY in DER format.
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_PKEY_set1_DH(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_DH *key)
{
    byte havePublic = 0, havePrivate = 0;
    int ret;
    word32 derSz = 0;
    byte* derBuf = NULL;
    DhKey* dhkey = NULL;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_set1_DH");

    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    clearEVPPkeyKeys(pkey);

    if (wolfSSL_DH_up_ref(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("Failed to increase dh key ref count");
        return WOLFSSL_FAILURE;
    }

    pkey->dh    = key;
    pkey->ownDh = 1; /* pkey does not own DH but needs to call free on it */
    pkey->type  = EVP_PKEY_DH;
    if (key->inSet == 0) {
        if (SetDhInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDhInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    dhkey = (DhKey*)key->internal;

    havePublic  = mp_unsigned_bin_size(&dhkey->pub)  > 0;
    havePrivate = mp_unsigned_bin_size(&dhkey->priv) > 0;

    /* Get size of DER buffer only */
    if (havePublic && !havePrivate) {
        ret = wc_DhPubKeyToDer(dhkey, NULL, &derSz);
    } else if (havePrivate && !havePublic) {
        ret = wc_DhPrivKeyToDer(dhkey, NULL, &derSz);
    } else {
        ret = wc_DhParamsToDer(dhkey,NULL,&derSz);
    }

    if (derSz == 0 || ret != LENGTH_ONLY_E) {
       WOLFSSL_MSG("Failed to get size of DH Key");
       return WOLFSSL_FAILURE;
    }

    derBuf = (byte*)XMALLOC((size_t)derSz, pkey->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (derBuf == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }

    /* Fill DER buffer */
    if (havePublic && !havePrivate) {
        ret = wc_DhPubKeyToDer(dhkey, derBuf, &derSz);
    } else if (havePrivate && !havePublic) {
        ret = wc_DhPrivKeyToDer(dhkey, derBuf, &derSz);
    } else {
        ret = wc_DhParamsToDer(dhkey,derBuf,&derSz);
    }

    if (ret <= 0) {
        WOLFSSL_MSG("Failed to export DH Key");
        XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* Store DH key into pkey (DER format) */
    pkey->pkey.ptr = (char*)derBuf;
    pkey->pkey_sz = (int)derSz;

    return WOLFSSL_SUCCESS;
}

WOLFSSL_DH* wolfSSL_EVP_PKEY_get0_DH(WOLFSSL_EVP_PKEY* key)
{
    if (!key) {
        return NULL;
    }
    return key->dh;
}

WOLFSSL_DH* wolfSSL_EVP_PKEY_get1_DH(WOLFSSL_EVP_PKEY* key)
{
    WOLFSSL_DH* local = NULL;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_get1_DH");

    if (key == NULL || key->dh == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return NULL;
    }

    if (key->type == EVP_PKEY_DH) {
        /* if key->dh already exists copy instead of re-importing from DER */
        if (key->dh != NULL) {
            if (wolfSSL_DH_up_ref(key->dh) != WOLFSSL_SUCCESS) {
                return NULL;
            }
            local = key->dh;
        }
        else {
#if !defined(NO_DH) && (!defined(HAVE_FIPS) || (defined(HAVE_FIPS_VERSION) && \
    (HAVE_FIPS_VERSION>2)))
            local = wolfSSL_DH_new();
            if (local == NULL) {
                WOLFSSL_MSG("Error creating a new WOLFSSL_DH structure");
                return NULL;
            }
            if (wolfSSL_DH_LoadDer(local, (const unsigned char*)key->pkey.ptr,
                        key->pkey_sz) != SSL_SUCCESS) {
                wolfSSL_DH_free(local);
                WOLFSSL_MSG("Error wolfSSL_DH_LoadDer");
                local = NULL;
            }
#else
            WOLFSSL_MSG("EVP_PKEY does not hold DH struct");
            return NULL;
#endif
        }
    }
    else {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY does not hold a DH key");
        wolfSSL_DH_free(local);
        return NULL;
    }

    return local;
}
#endif /* NO_DH && WOLFSSL_DH_EXTRA && NO_FILESYSTEM */

int wolfSSL_EVP_PKEY_assign(WOLFSSL_EVP_PKEY *pkey, int type, void *key)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_assign");

    /* pkey and key checked if NULL in subsequent assign functions */
    switch(type) {
    #ifndef NO_RSA
        case EVP_PKEY_RSA:
            ret = wolfSSL_EVP_PKEY_assign_RSA(pkey, (WOLFSSL_RSA*)key);
            break;
    #endif
    #ifndef NO_DSA
        case EVP_PKEY_DSA:
            ret = wolfSSL_EVP_PKEY_assign_DSA(pkey, (WOLFSSL_DSA*)key);
            break;
    #endif
    #ifdef HAVE_ECC
        case EVP_PKEY_EC:
            ret = wolfSSL_EVP_PKEY_assign_EC_KEY(pkey, (WOLFSSL_EC_KEY*)key);
            break;
    #endif
    #ifndef NO_DH
         case EVP_PKEY_DH:
            ret = wolfSSL_EVP_PKEY_assign_DH(pkey, (WOLFSSL_DH*)key);
            break;
    #endif
        default:
            WOLFSSL_MSG("Unknown EVP_PKEY type in wolfSSL_EVP_PKEY_assign.");
            ret = WOLFSSL_FAILURE;
    }

    return ret;
}
#endif /* WOLFSSL_QT || OPENSSL_ALL */

#if defined(HAVE_ECC)
/* try and populate public pkey_sz and pkey.ptr */
static int ECC_populate_EVP_PKEY(EVP_PKEY* pkey, WOLFSSL_EC_KEY *key)
{
    int derSz = 0;
    byte* derBuf = NULL;
    ecc_key* ecc;

    if (pkey == NULL || key == NULL || key->internal == NULL)
        return WOLFSSL_FAILURE;

    ecc = (ecc_key*)key->internal;
    if (ecc->type == ECC_PRIVATEKEY || ecc->type == ECC_PRIVATEKEY_ONLY) {
#ifdef HAVE_PKCS8
        if (key->pkcs8HeaderSz) {
            /* when key has pkcs8 header the pkey should too */
            if (wc_EccKeyToPKCS8(ecc, NULL, (word32*)&derSz) == LENGTH_ONLY_E) {
                derBuf = (byte*)XMALLOC((size_t)derSz, pkey->heap,
                    DYNAMIC_TYPE_OPENSSL);
                if (derBuf) {
                    if (wc_EccKeyToPKCS8(ecc, derBuf, (word32*)&derSz) >= 0) {
                        if (pkey->pkey.ptr) {
                            XFREE(pkey->pkey.ptr, pkey->heap, DYNAMIC_TYPE_OPENSSL);
                        }
                        pkey->pkey_sz = (int)derSz;
                        pkey->pkey.ptr = (char*)derBuf;
                        pkey->pkcs8HeaderSz = key->pkcs8HeaderSz;
                        return WOLFSSL_SUCCESS;
                    }
                    else {
                        XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_OPENSSL);
                        derBuf = NULL;
                    }
                }
            }
        }
        else
#endif /* HAVE_PKCS8 */
        {
            if (ecc->type == ECC_PRIVATEKEY_ONLY) {
                if (wc_ecc_make_pub(ecc, NULL) != MP_OKAY) {
                    return WOLFSSL_FAILURE;
                }
            }

            /* if not, the pkey will be traditional ecc key */
            if ((derSz = wc_EccKeyDerSize(ecc, 1)) > 0) {
                derBuf = (byte*)XMALLOC((size_t)derSz, pkey->heap,
                    DYNAMIC_TYPE_OPENSSL);
                if (derBuf) {
                    if (wc_EccKeyToDer(ecc, derBuf, (word32)derSz) >= 0) {
                        if (pkey->pkey.ptr) {
                            XFREE(pkey->pkey.ptr, pkey->heap, DYNAMIC_TYPE_OPENSSL);
                        }
                        pkey->pkey_sz = (int)derSz;
                        pkey->pkey.ptr = (char*)derBuf;
                        return WOLFSSL_SUCCESS;
                    }
                    else {
                        XFREE(derBuf, pkey->heap, DYNAMIC_TYPE_OPENSSL);
                        derBuf = NULL;
                    }
                }
            }
        }
    }
    else if (ecc->type == ECC_PUBLICKEY) {
        if ((derSz = wc_EccPublicKeyDerSize(ecc, 1)) > 0) {
            derBuf = (byte*)XREALLOC(pkey->pkey.ptr, (size_t)derSz, NULL,
                    DYNAMIC_TYPE_OPENSSL);
            if (derBuf != NULL) {
                pkey->pkey.ptr = (char*)derBuf;
                if ((derSz = wc_EccPublicKeyToDer(ecc, derBuf, (word32)derSz,
                        1)) < 0) {
                    XFREE(derBuf, NULL, DYNAMIC_TYPE_OPENSSL);
                    derBuf = NULL;
                }
            }
        }
    }
    if (derBuf != NULL) {
        pkey->pkey_sz = (int)derSz;
        return WOLFSSL_SUCCESS;
    }
    else {
        return WOLFSSL_FAILURE;
    }
}

int wolfSSL_EVP_PKEY_set1_EC_KEY(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_EC_KEY *key)
{
#ifdef HAVE_ECC
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_set1_EC_KEY");
    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;
    clearEVPPkeyKeys(pkey);
    if (wolfSSL_EC_KEY_up_ref(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_up_ref failed");
        return WOLFSSL_FAILURE;
    }
    pkey->ecc    = key;
    pkey->ownEcc = 1; /* pkey needs to call free on key */
    pkey->type   = EVP_PKEY_EC;
    return ECC_populate_EVP_PKEY(pkey, key);
#else
    (void)pkey;
    (void)key;
    return WOLFSSL_FAILURE;
#endif /* HAVE_ECC */
}

void* wolfSSL_EVP_X_STATE(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_X_STATE");

    if (ctx) {
        switch (ctx->cipherType) {
            case ARC4_TYPE:
                WOLFSSL_MSG("returning arc4 state");
                return (void*)&ctx->cipher.arc4.x;

            default:
                WOLFSSL_MSG("bad x state type");
                return 0;
        }
    }

    return NULL;
}
int wolfSSL_EVP_PKEY_assign_EC_KEY(EVP_PKEY* pkey, WOLFSSL_EC_KEY* key)
{
    int ret;

    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    /* try and populate public pkey_sz and pkey.ptr */
    ret = ECC_populate_EVP_PKEY(pkey, key);
    if (ret == WOLFSSL_SUCCESS) { /* take ownership of key if can be used */
        clearEVPPkeyKeys(pkey); /* clear out any previous keys */

        pkey->type = EVP_PKEY_EC;
        pkey->ecc = key;
        pkey->ownEcc = 1;
    }
    return ret;
}
#endif /* HAVE_ECC */

#ifndef NO_WOLFSSL_STUB
const WOLFSSL_EVP_MD* wolfSSL_EVP_ripemd160(void)
{
    WOLFSSL_MSG("wolfSSL_ripemd160");
    WOLFSSL_STUB("EVP_ripemd160");
    return NULL;
}
#endif


int wolfSSL_EVP_MD_pkey_type(const WOLFSSL_EVP_MD* type)
{
    int ret = BAD_FUNC_ARG;

    WOLFSSL_ENTER("wolfSSL_EVP_MD_pkey_type");

    if (type != NULL) {
        if (XSTRCMP(type, "MD5") == 0) {
            ret = NID_md5WithRSAEncryption;
        }
        else if (XSTRCMP(type, "SHA1") == 0) {
            ret = NID_sha1WithRSAEncryption;
        }
        else if (XSTRCMP(type, "SHA224") == 0) {
            ret = NID_sha224WithRSAEncryption;
        }
        else if (XSTRCMP(type, "SHA256") == 0) {
            ret = NID_sha256WithRSAEncryption;
        }
        else if (XSTRCMP(type, "SHA384") == 0) {
            ret = NID_sha384WithRSAEncryption;
        }
        else if (XSTRCMP(type, "SHA512") == 0) {
            ret = NID_sha512WithRSAEncryption;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_EVP_MD_pkey_type", ret);

    return ret;
}



int wolfSSL_EVP_CIPHER_CTX_iv_length(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_CIPHER_CTX_iv_length");

    if (ctx == NULL) {
        WOLFSSL_MSG("No context");
        return 0;
    }

    switch (ctx->cipherType) {

#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
        case AES_128_CBC_TYPE :
        case AES_192_CBC_TYPE :
        case AES_256_CBC_TYPE :
            WOLFSSL_MSG("AES CBC");
            return AES_BLOCK_SIZE;
#endif
#if (!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
#ifdef HAVE_AESGCM
        case AES_128_GCM_TYPE :
        case AES_192_GCM_TYPE :
        case AES_256_GCM_TYPE :
            WOLFSSL_MSG("AES GCM");
            if (ctx->ivSz != 0) {
                return ctx->ivSz;
            }
            return GCM_NONCE_MID_SZ;
#endif
#ifdef HAVE_AESCCM
        case AES_128_CCM_TYPE :
        case AES_192_CCM_TYPE :
        case AES_256_CCM_TYPE :
            WOLFSSL_MSG("AES CCM");
            if (ctx->ivSz != 0) {
                return ctx->ivSz;
            }
            return CCM_NONCE_MIN_SZ;
#endif
#endif /* (HAVE_FIPS && !HAVE_SELFTEST) || HAVE_FIPS_VERSION >= 2 */
#ifdef WOLFSSL_AES_COUNTER
        case AES_128_CTR_TYPE :
        case AES_192_CTR_TYPE :
        case AES_256_CTR_TYPE :
            WOLFSSL_MSG("AES CTR");
            return AES_BLOCK_SIZE;
#endif
#ifndef NO_DES3
        case DES_CBC_TYPE :
            WOLFSSL_MSG("DES CBC");
            return DES_BLOCK_SIZE;

        case DES_EDE3_CBC_TYPE :
            WOLFSSL_MSG("DES EDE3 CBC");
            return DES_BLOCK_SIZE;
#endif
#ifndef NO_RC4
        case ARC4_TYPE :
            WOLFSSL_MSG("ARC4");
            return 0;
#endif
#ifdef WOLFSSL_AES_CFB
#if !defined(HAVE_SELFTEST) && !defined(HAVE_FIPS)
        case AES_128_CFB1_TYPE:
        case AES_192_CFB1_TYPE:
        case AES_256_CFB1_TYPE:
            WOLFSSL_MSG("AES CFB1");
            return AES_BLOCK_SIZE;
        case AES_128_CFB8_TYPE:
        case AES_192_CFB8_TYPE:
        case AES_256_CFB8_TYPE:
            WOLFSSL_MSG("AES CFB8");
            return AES_BLOCK_SIZE;
#endif /* !HAVE_SELFTEST && !HAVE_FIPS */
        case AES_128_CFB128_TYPE:
        case AES_192_CFB128_TYPE:
        case AES_256_CFB128_TYPE:
            WOLFSSL_MSG("AES CFB128");
            return AES_BLOCK_SIZE;
#endif /* WOLFSSL_AES_CFB */
#if defined(WOLFSSL_AES_OFB)
        case AES_128_OFB_TYPE:
        case AES_192_OFB_TYPE:
        case AES_256_OFB_TYPE:
            WOLFSSL_MSG("AES OFB");
            return AES_BLOCK_SIZE;
#endif /* WOLFSSL_AES_OFB */
#ifdef WOLFSSL_AES_XTS
        case AES_128_XTS_TYPE:
        case AES_256_XTS_TYPE:
            WOLFSSL_MSG("AES XTS");
            return AES_BLOCK_SIZE;
#endif /* WOLFSSL_AES_XTS */
#ifdef HAVE_ARIA
        case ARIA_128_GCM_TYPE :
        case ARIA_192_GCM_TYPE :
        case ARIA_256_GCM_TYPE :
            WOLFSSL_MSG("ARIA GCM");
            if (ctx->ivSz != 0) {
                return ctx->ivSz;
            }
            return GCM_NONCE_MID_SZ;
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CHACHA20_POLY1305_TYPE:
            WOLFSSL_MSG("CHACHA20 POLY1305");
            return CHACHA20_POLY1305_AEAD_IV_SIZE;
#endif /* HAVE_CHACHA HAVE_POLY1305 */
#ifdef HAVE_CHACHA
        case CHACHA20_TYPE:
            WOLFSSL_MSG("CHACHA20");
            return WOLFSSL_EVP_CHACHA_IV_BYTES;
#endif /* HAVE_CHACHA */
#ifdef WOLFSSL_SM4_CBC
        case SM4_CBC_TYPE :
            WOLFSSL_MSG("SM4 CBC");
            return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_CTR
        case SM4_CTR_TYPE :
            WOLFSSL_MSG("SM4 CTR");
            return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_GCM
        case SM4_GCM_TYPE :
            WOLFSSL_MSG("SM4 GCM");
            if (ctx->ivSz != 0) {
                return ctx->ivSz;
            }
            return GCM_NONCE_MID_SZ;
#endif
#ifdef WOLFSSL_SM4_CCM
        case SM4_CCM_TYPE :
            WOLFSSL_MSG("SM4 CCM");
            if (ctx->ivSz != 0) {
                return ctx->ivSz;
            }
            return CCM_NONCE_MIN_SZ;
#endif

        case NULL_CIPHER_TYPE :
            WOLFSSL_MSG("NULL");
            return 0;

        default: {
            WOLFSSL_MSG("bad type");
        }
    }
    return 0;
}

int wolfSSL_EVP_CIPHER_iv_length(const WOLFSSL_EVP_CIPHER* cipher)
{
    const char *name = (const char *)cipher;
    WOLFSSL_MSG("wolfSSL_EVP_CIPHER_iv_length");

#ifndef NO_AES
#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
    #ifdef WOLFSSL_AES_128
    if (XSTRCMP(name, EVP_AES_128_CBC) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRCMP(name, EVP_AES_192_CBC) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRCMP(name, EVP_AES_256_CBC) == 0)
        return AES_BLOCK_SIZE;
    #endif
#endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */
#if (!defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
#ifdef HAVE_AESGCM
    #ifdef WOLFSSL_AES_128
    if (XSTRCMP(name, EVP_AES_128_GCM) == 0)
        return GCM_NONCE_MID_SZ;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRCMP(name, EVP_AES_192_GCM) == 0)
        return GCM_NONCE_MID_SZ;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRCMP(name, EVP_AES_256_GCM) == 0)
        return GCM_NONCE_MID_SZ;
    #endif
#endif /* HAVE_AESGCM */
#ifdef HAVE_AESCCM
    #ifdef WOLFSSL_AES_128
    if (XSTRCMP(name, EVP_AES_128_CCM) == 0)
        return CCM_NONCE_MIN_SZ;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRCMP(name, EVP_AES_192_CCM) == 0)
        return CCM_NONCE_MIN_SZ;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRCMP(name, EVP_AES_256_CCM) == 0)
        return CCM_NONCE_MIN_SZ;
    #endif
#endif /* HAVE_AESCCM */
#endif /* (HAVE_FIPS && !HAVE_SELFTEST) || HAVE_FIPS_VERSION >= 2 */
#ifdef WOLFSSL_AES_COUNTER
    #ifdef WOLFSSL_AES_128
    if (XSTRCMP(name, EVP_AES_128_CTR) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRCMP(name, EVP_AES_192_CTR) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRCMP(name, EVP_AES_256_CTR) == 0)
        return AES_BLOCK_SIZE;
    #endif
#endif
#ifdef WOLFSSL_AES_XTS
    #ifdef WOLFSSL_AES_128
    if (XSTRCMP(name, EVP_AES_128_XTS) == 0)
        return AES_BLOCK_SIZE;
    #endif /* WOLFSSL_AES_128 */

    #ifdef WOLFSSL_AES_256
    if (XSTRCMP(name, EVP_AES_256_XTS) == 0)
        return AES_BLOCK_SIZE;
    #endif /* WOLFSSL_AES_256 */
#endif /* WOLFSSL_AES_XTS */

#endif
#ifdef HAVE_ARIA
    if (XSTRCMP(name, EVP_ARIA_128_GCM) == 0)
        return GCM_NONCE_MID_SZ;
    if (XSTRCMP(name, EVP_ARIA_192_GCM) == 0)
        return GCM_NONCE_MID_SZ;
    if (XSTRCMP(name, EVP_ARIA_256_GCM) == 0)
        return GCM_NONCE_MID_SZ;
#endif /* HAVE_ARIA */

#ifndef NO_DES3
    if ((XSTRCMP(name, EVP_DES_CBC) == 0) ||
           (XSTRCMP(name, EVP_DES_EDE3_CBC) == 0)) {
        return DES_BLOCK_SIZE;
    }
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    if (XSTRCMP(name, EVP_CHACHA20_POLY1305) == 0)
        return CHACHA20_POLY1305_AEAD_IV_SIZE;
#endif

#ifdef HAVE_CHACHA
    if (XSTRCMP(name, EVP_CHACHA20) == 0)
        return WOLFSSL_EVP_CHACHA_IV_BYTES;
#endif

#ifdef WOLFSSL_SM4_CBC
    if (XSTRCMP(name, EVP_SM4_CBC) == 0)
        return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_CTR
    if (XSTRCMP(name, EVP_SM4_CTR) == 0)
        return SM4_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_SM4_GCM
    if (XSTRCMP(name, EVP_SM4_GCM) == 0)
        return GCM_NONCE_MID_SZ;
#endif
#ifdef WOLFSSL_SM4_CCM
    if (XSTRCMP(name, EVP_SM4_CCM) == 0)
        return CCM_NONCE_MIN_SZ;
#endif

    (void)name;

    return 0;
}


int wolfSSL_EVP_X_STATE_LEN(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_X_STATE_LEN");

    if (ctx) {
        switch (ctx->cipherType) {
            case ARC4_TYPE:
                WOLFSSL_MSG("returning arc4 state size");
                return sizeof(Arc4);

            default:
                WOLFSSL_MSG("bad x state type");
                return 0;
        }
    }

    return 0;
}


/* return of pkey->type which will be EVP_PKEY_RSA for example.
 *
 * type  type of EVP_PKEY
 *
 * returns type or if type is not found then NID_undef
 */
int wolfSSL_EVP_PKEY_type(int type)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_type");

    switch (type) {
        case EVP_PKEY_RSA:
            return EVP_PKEY_RSA;
        case EVP_PKEY_DSA:
            return EVP_PKEY_DSA;
        case EVP_PKEY_EC:
            return EVP_PKEY_EC;
        case EVP_PKEY_DH:
            return EVP_PKEY_DH;
        default:
            return NID_undef;
    }
}


int wolfSSL_EVP_PKEY_id(const WOLFSSL_EVP_PKEY *pkey)
{
    if (pkey != NULL)
        return pkey->type;
    return 0;
}


int wolfSSL_EVP_PKEY_base_id(const WOLFSSL_EVP_PKEY *pkey)
{
    if (pkey == NULL)
        return NID_undef;
    return wolfSSL_EVP_PKEY_type(pkey->type);
}

int wolfSSL_EVP_PKEY_get_default_digest_nid(WOLFSSL_EVP_PKEY *pkey, int *pnid)
{
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_get_default_digest_nid");

    if (!pkey || !pnid) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    switch (pkey->type) {
    case EVP_PKEY_HMAC:
#ifndef NO_DSA
    case EVP_PKEY_DSA:
#endif
#ifndef NO_RSA
    case EVP_PKEY_RSA:
#endif
#ifdef HAVE_ECC
    case EVP_PKEY_EC:
#endif
        *pnid = NID_sha256;
        return WOLFSSL_SUCCESS;
    default:
        return WOLFSSL_FAILURE;
    }
}

#if defined(OPENSSL_ALL) || defined(WOLFSSL_WPAS_SMALL)
WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKCS82PKEY(const WOLFSSL_PKCS8_PRIV_KEY_INFO* p8)
{
    if (p8 == NULL || p8->pkey.ptr == NULL) {
        return NULL;
    }

    return wolfSSL_d2i_PrivateKey_EVP(NULL, (unsigned char**)&p8->pkey.ptr,
        p8->pkey_sz);
}

/* in wolf PKCS8_PRIV_KEY_INFO and WOLFSSL_EVP_PKEY are same type */
/* this function just casts and returns pointer */
WOLFSSL_PKCS8_PRIV_KEY_INFO* wolfSSL_EVP_PKEY2PKCS8(const WOLFSSL_EVP_PKEY* pkey)
{
    return (WOLFSSL_PKCS8_PRIV_KEY_INFO*)pkey;
}
#endif

/* increments ref count of WOLFSSL_EVP_PKEY. Return 1 on success, 0 on error */
int wolfSSL_EVP_PKEY_up_ref(WOLFSSL_EVP_PKEY* pkey)
{
    if (pkey) {
        int ret;
        wolfSSL_RefInc(&pkey->ref, &ret);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (ret != 0) {
            WOLFSSL_MSG("Failed to lock pkey mutex");
        }
    #else
        (void)ret;
    #endif

        return WOLFSSL_SUCCESS;
    }

    return WOLFSSL_FAILURE;
}

#ifndef NO_RSA
int wolfSSL_EVP_PKEY_assign_RSA(EVP_PKEY* pkey, WOLFSSL_RSA* key)
{
    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    clearEVPPkeyKeys(pkey);
    pkey->type = EVP_PKEY_RSA;
    pkey->rsa = key;
    pkey->ownRsa = 1;

    /* try and populate pkey_sz and pkey.ptr */
    if (key->internal) {
        RsaKey* rsa = (RsaKey*)key->internal;
        int ret = wc_RsaKeyToDer(rsa, NULL, 0);
        if (ret > 0) {
            word32 derSz = (word32)ret;
            byte* derBuf = (byte*)XMALLOC((size_t)derSz, NULL,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (derBuf != NULL) {
                ret = wc_RsaKeyToDer(rsa, derBuf, derSz);
                if (ret >= 0) {
                    pkey->pkey_sz = ret;
                    pkey->pkey.ptr = (char*)derBuf;
                }
                else { /* failure - okay to ignore */
                    XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                    derBuf = NULL;
                }
            }
        }
    }

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_RSA */

#ifndef NO_DSA
int wolfSSL_EVP_PKEY_assign_DSA(EVP_PKEY* pkey, WOLFSSL_DSA* key)
{
    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    clearEVPPkeyKeys(pkey);
    pkey->type = EVP_PKEY_DSA;
    pkey->dsa = key;
    pkey->ownDsa = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_DSA */

#ifndef NO_DH
int wolfSSL_EVP_PKEY_assign_DH(EVP_PKEY* pkey, WOLFSSL_DH* key)
{
    if (pkey == NULL || key == NULL)
        return WOLFSSL_FAILURE;

    clearEVPPkeyKeys(pkey);
    pkey->type = EVP_PKEY_DH;
    pkey->dh = key;
    pkey->ownDh = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_DH */
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(HAVE_CURL)
/* EVP Digest functions used with cURL build too */

static enum wc_HashType EvpMd2MacType(const WOLFSSL_EVP_MD *md)
{
    if (md != NULL) {
        const struct s_ent *ent;
        for (ent = md_tbl; ent->name != NULL; ent++) {
            if (XSTRCMP((const char *)md, ent->name) == 0) {
                return ent->macType;
            }
        }
    }
    return WC_HASH_TYPE_NONE;
}

int wolfSSL_EVP_DigestInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl)
{
    (void) impl;
    WOLFSSL_ENTER("wolfSSL_EVP_DigestInit_ex");
    return wolfSSL_EVP_DigestInit(ctx, type);
}

/* this function makes the assumption that out buffer is big enough for digest*/
int wolfSSL_EVP_Digest(const unsigned char* in, int inSz, unsigned char* out,
                              unsigned int* outSz, const WOLFSSL_EVP_MD* evp,
                              WOLFSSL_ENGINE* eng)
{
    int err;
    int hashType = WC_HASH_TYPE_NONE;
    int hashSz;

    WOLFSSL_ENTER("wolfSSL_EVP_Digest");
    if (in == NULL || out == NULL || evp == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return WOLFSSL_FAILURE;
    }

    err = wolfSSL_EVP_get_hashinfo(evp, &hashType, &hashSz);
    if (err != WOLFSSL_SUCCESS)
        return err;

    if (wc_Hash((enum wc_HashType)hashType, in, (word32)inSz, out,
            (word32)hashSz) != 0) {
        return WOLFSSL_FAILURE;
    }

    if (outSz != NULL)
        *outSz = (unsigned int)hashSz;

    (void)eng;
    return WOLFSSL_SUCCESS;
}

static const struct alias {
            const char *name;
            const char *alias;
} digest_alias_tbl[] =
{
    {"MD4", "ssl3-md4"},
    {"MD5", "ssl3-md5"},
    {"SHA1", "ssl3-sha1"},
    {"SHA1", "SHA"},
    { NULL, NULL}
};

const WOLFSSL_EVP_MD *wolfSSL_EVP_get_digestbyname(const char *name)
{
    char nameUpper[15]; /* 15 bytes should be enough for any name */
    size_t i;

    const struct alias  *al;
    const struct s_ent *ent;

    for (i = 0; i < sizeof(nameUpper) && name[i] != '\0'; i++) {
        nameUpper[i] = (char)XTOUPPER((unsigned char) name[i]);
    }
    if (i < sizeof(nameUpper))
        nameUpper[i] = '\0';
    else
        return NULL;

    name = nameUpper;
    for (al = digest_alias_tbl; al->name != NULL; al++)
        if(XSTRCMP(name, al->alias) == 0) {
            name = al->name;
            break;
        }

    for (ent = md_tbl; ent->name != NULL; ent++)
        if(XSTRCMP(name, ent->name) == 0) {
            return (EVP_MD *)ent->name;
        }
    return NULL;
}

/* Returns the NID of the WOLFSSL_EVP_MD passed in.
 *
 * type - pointer to WOLFSSL_EVP_MD for which to return NID value
 *
 * Returns NID on success, or NID_undef if none exists.
 */
int wolfSSL_EVP_MD_type(const WOLFSSL_EVP_MD* type)
{
    const struct s_ent *ent ;
    WOLFSSL_ENTER("EVP_MD_type");

    if (type == NULL) {
        WOLFSSL_MSG("MD type arg is NULL");
        return NID_undef;
    }

    for( ent = md_tbl; ent->name != NULL; ent++){
        if(XSTRCMP((const char *)type, ent->name) == 0) {
            return ent->nid;
        }
    }
    return NID_undef;
}

#ifndef NO_MD4

    /* return a pointer to MD4 EVP type */
    const WOLFSSL_EVP_MD* wolfSSL_EVP_md4(void)
    {
        WOLFSSL_ENTER("EVP_md4");
        return EVP_get_digestbyname("MD4");
    }

#endif /* !NO_MD4 */


#ifndef NO_MD5

    const WOLFSSL_EVP_MD* wolfSSL_EVP_md5(void)
    {
        WOLFSSL_ENTER("EVP_md5");
        return EVP_get_digestbyname("MD5");
    }

#endif /* !NO_MD5 */

#ifdef HAVE_BLAKE2
    /* return EVP_MD
     * @param none
     * @return "blake2b512"
     */
    const WOLFSSL_EVP_MD* wolfSSL_EVP_blake2b512(void)
    {
        WOLFSSL_ENTER("EVP_blake2b512");
        return EVP_get_digestbyname("BLAKE2b512");
    }

#endif

#ifdef HAVE_BLAKE2S
    /* return EVP_MD
     * @param none
     * @return "blake2s256"
     */
    const WOLFSSL_EVP_MD* wolfSSL_EVP_blake2s256(void)
    {
        WOLFSSL_ENTER("EVP_blake2s256");
        return EVP_get_digestbyname("BLAKE2s256");
    }

#endif


#ifndef NO_WOLFSSL_STUB
    void wolfSSL_EVP_set_pw_prompt(const char *prompt)
    {
        (void)prompt;
        WOLFSSL_STUB("EVP_set_pw_prompt");
    }
#endif

#ifndef NO_WOLFSSL_STUB
    const WOLFSSL_EVP_MD* wolfSSL_EVP_mdc2(void)
    {
        WOLFSSL_STUB("EVP_mdc2");
        return NULL;
    }
#endif

#ifndef NO_SHA
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha1(void)
    {
        WOLFSSL_ENTER("EVP_sha1");
        return EVP_get_digestbyname("SHA1");
    }
#endif /* NO_SHA */

#ifdef WOLFSSL_SHA224

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha224(void)
    {
        WOLFSSL_ENTER("EVP_sha224");
        return EVP_get_digestbyname("SHA224");
    }

#endif /* WOLFSSL_SHA224 */


    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha256(void)
    {
        WOLFSSL_ENTER("EVP_sha256");
        return EVP_get_digestbyname("SHA256");
    }

#ifdef WOLFSSL_SHA384

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha384(void)
    {
        WOLFSSL_ENTER("EVP_sha384");
        return EVP_get_digestbyname("SHA384");
    }

#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA512

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512(void)
    {
        WOLFSSL_ENTER("EVP_sha512");
        return EVP_get_digestbyname("SHA512");
    }

#ifndef WOLFSSL_NOSHA512_224

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512_224(void)
    {
        WOLFSSL_ENTER("EVP_sha512_224");
        return EVP_get_digestbyname("SHA512_224");
    }

#endif /* !WOLFSSL_NOSHA512_224 */

#ifndef WOLFSSL_NOSHA512_256
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512_256(void)
    {
        WOLFSSL_ENTER("EVP_sha512_256");
        return EVP_get_digestbyname("SHA512_256");
    }

#endif /* !WOLFSSL_NOSHA512_224 */

#endif /* WOLFSSL_SHA512 */

#ifdef WOLFSSL_SHA3
#ifndef WOLFSSL_NOSHA3_224
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_224(void)
    {
        WOLFSSL_ENTER("EVP_sha3_224");
        return EVP_get_digestbyname("SHA3_224");
    }
#endif /* WOLFSSL_NOSHA3_224 */


#ifndef WOLFSSL_NOSHA3_256
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_256(void)
    {
        WOLFSSL_ENTER("EVP_sha3_256");
        return EVP_get_digestbyname("SHA3_256");
    }
#endif /* WOLFSSL_NOSHA3_256 */

#ifndef WOLFSSL_NOSHA3_384
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_384(void)
    {
        WOLFSSL_ENTER("EVP_sha3_384");
        return EVP_get_digestbyname("SHA3_384");
    }
#endif /* WOLFSSL_NOSHA3_384 */

#ifndef WOLFSSL_NOSHA3_512
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha3_512(void)
    {
        WOLFSSL_ENTER("EVP_sha3_512");
        return EVP_get_digestbyname("SHA3_512");
    }
#endif /* WOLFSSL_NOSHA3_512 */

#ifdef WOLFSSL_SHAKE128
    const WOLFSSL_EVP_MD* wolfSSL_EVP_shake128(void)
    {
        WOLFSSL_ENTER("EVP_shake128");
        return EVP_get_digestbyname("SHAKE128");
    }
#endif /* WOLFSSL_SHAKE128 */

#ifdef WOLFSSL_SHAKE256
    const WOLFSSL_EVP_MD* wolfSSL_EVP_shake256(void)
    {
        WOLFSSL_ENTER("EVP_shake256");
        return EVP_get_digestbyname("SHAKE256");
    }
#endif /* WOLFSSL_SHAKE256 */

#endif /* WOLFSSL_SHA3 */

#ifdef WOLFSSL_SM3
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sm3(void)
    {
        WOLFSSL_ENTER("EVP_sm3");
        return EVP_get_digestbyname("SM3");
    }
#endif /* WOLFSSL_SM3 */


    WOLFSSL_EVP_MD_CTX *wolfSSL_EVP_MD_CTX_new(void)
    {
        WOLFSSL_EVP_MD_CTX* ctx;
        WOLFSSL_ENTER("EVP_MD_CTX_new");
        ctx = (WOLFSSL_EVP_MD_CTX*)XMALLOC(sizeof(*ctx), NULL,
                                                       DYNAMIC_TYPE_OPENSSL);
        if (ctx){
            wolfSSL_EVP_MD_CTX_init(ctx);
        }
        return ctx;
    }

    void wolfSSL_EVP_MD_CTX_free(WOLFSSL_EVP_MD_CTX *ctx)
    {
        if (ctx) {
            WOLFSSL_ENTER("EVP_MD_CTX_free");
            wolfSSL_EVP_MD_CTX_cleanup(ctx);
            XFREE(ctx, NULL, DYNAMIC_TYPE_OPENSSL);
        }
    }

    /* returns the NID of message digest used by the ctx */
    int wolfSSL_EVP_MD_CTX_type(const WOLFSSL_EVP_MD_CTX *ctx)
    {
        WOLFSSL_ENTER("EVP_MD_CTX_type");

        if (ctx) {
            const struct s_ent *ent;

            if (ctx->isHMAC) {
                return NID_hmac;
            }

            for(ent = md_tbl; ent->name != NULL; ent++) {
                if (ctx->macType == ent->macType) {
                    return ent->nid;
                }
            }
            /* Return whatever we got */
            return ctx->macType;
        }
        return 0;
    }


    /* returns digest size */
    int wolfSSL_EVP_MD_CTX_size(const WOLFSSL_EVP_MD_CTX *ctx) {
        return(wolfSSL_EVP_MD_size(wolfSSL_EVP_MD_CTX_md(ctx)));
    }
    /* returns block size */
    int wolfSSL_EVP_MD_CTX_block_size(const WOLFSSL_EVP_MD_CTX *ctx) {
        return(wolfSSL_EVP_MD_block_size(wolfSSL_EVP_MD_CTX_md(ctx)));
    }

    void wolfSSL_EVP_MD_CTX_init(WOLFSSL_EVP_MD_CTX* ctx)
    {
        WOLFSSL_ENTER("EVP_CIPHER_MD_CTX_init");
        XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_MD_CTX));
    }

    const WOLFSSL_EVP_MD *wolfSSL_EVP_MD_CTX_md(const WOLFSSL_EVP_MD_CTX *ctx)
    {
        const struct s_ent *ent;
        if (ctx == NULL)
            return NULL;
        WOLFSSL_ENTER("EVP_MD_CTX_md");
        if (ctx->isHMAC) {
            return "HMAC";
        }
        for(ent = md_tbl; ent->name != NULL; ent++) {
            if(ctx->macType == ent->macType) {
                return (const WOLFSSL_EVP_MD *)ent->name;
            }
        }
        return (WOLFSSL_EVP_MD *)NULL;
    }

    /* return alias name if has
     * @param n message digest type name
     * @return alias name, otherwise NULL
     */
    static const char* hasAliasName(const char* n)
    {

        const char* aliasnm = NULL;
        const struct alias  *al;

        for (al = digest_alias_tbl; al->name != NULL; al++)
            if(XSTRCMP(n, al->name) == 0) {
                aliasnm = al->alias;
                break;
            }

        return aliasnm;
    }


    struct do_all_md {
        void *arg;
        void (*fn) (const WOLFSSL_EVP_MD *m,
                    const char* from, const char* to, void *arg);
    };

    /* do all md algorithm
     * @param nm a pointer to WOLFSSL_OBJ_NAME
     * @param arg arguments to pass to the callback
     * @return none
     */
    static void md_do_all_func(const WOLFSSL_OBJ_NAME* nm, void* arg)
    {
        struct do_all_md *md = (struct do_all_md*)arg;

        const struct s_ent *ent;

        /* sanity check */
        if (md == NULL || nm == NULL || md->fn == NULL ||
            nm->type != WOLFSSL_OBJ_NAME_TYPE_MD_METH)
            return;

        /* loop all md */
        for (ent = md_tbl; ent->name != NULL; ent++){
            /* check if the md has alias */
            if(hasAliasName(ent->name) != NULL) {
                md->fn(NULL, ent->name, ent->name, md->arg);
            }
            else {
                md->fn(ent->name, ent->name, NULL, md->arg);
            }
        }
    }

    /* call md_do_all function to do all md algorithm via a callback function
     * @param fn a callback function to be called with all 'md'
     * @param args arguments to pass to the callback
     * @return none
     */
    void wolfSSL_EVP_MD_do_all(void (*fn) (const WOLFSSL_EVP_MD *m,
                 const char* from, const char* to, void* xx), void* args)
    {
        struct do_all_md md;

        md.fn = fn;
        md.arg = args;

        wolfSSL_OBJ_NAME_do_all(WOLFSSL_OBJ_NAME_TYPE_MD_METH,
                        md_do_all_func, &md);
    }

    /* call "fn" based on OBJ_NAME type
     * @param type OBJ_NAME type
     * @param fn a callback function
     * @param args arguments to pass to the callback
     * @return none
     */
    void wolfSSL_OBJ_NAME_do_all(int type,
                void (*fn)(const WOLFSSL_OBJ_NAME*, void* arg), void* arg)
    {
        WOLFSSL_OBJ_NAME objnm;

        /* sanity check */
        if (!fn)
            return;

        objnm.type = type;

        switch(type) {
            case WOLFSSL_OBJ_NAME_TYPE_MD_METH:
                fn(&objnm, arg);
                break;
            case WOLFSSL_OBJ_NAME_TYPE_CIPHER_METH:
            case WOLFSSL_OBJ_NAME_TYPE_PKEY_METH:
            case WOLFSSL_OBJ_NAME_TYPE_COMP_METH:
            case WOLFSSL_OBJ_NAME_TYPE_NUM:
                WOLFSSL_MSG("not implemented");
                FALL_THROUGH;
            case WOLFSSL_OBJ_NAME_TYPE_UNDEF:
            default:
                break;
        }
    }

    int wolfSSL_EVP_MD_CTX_cleanup(WOLFSSL_EVP_MD_CTX* ctx)
    {
        int ret = WOLFSSL_SUCCESS;
        WOLFSSL_ENTER("wolfSSL_EVP_MD_CTX_cleanup");
    #ifdef OPENSSL_EXTRA
        if (ctx->pctx != NULL)
            wolfSSL_EVP_PKEY_CTX_free(ctx->pctx);
    #endif

        if (ctx->isHMAC) {
            wc_HmacFree(&ctx->hash.hmac);
        }
        else {
            switch (ctx->macType) {
                case WC_HASH_TYPE_MD5:
            #ifndef NO_MD5
                    wc_Md5Free((wc_Md5*)&ctx->hash.digest);
            #endif /* !NO_MD5 */
                    break;
                case WC_HASH_TYPE_SHA:
            #ifndef NO_SHA
                    wc_ShaFree((wc_Sha*)&ctx->hash.digest);
            #endif /* !NO_SHA */
                    break;
                case WC_HASH_TYPE_SHA224:
            #ifdef WOLFSSL_SHA224
                    wc_Sha224Free((wc_Sha224*)&ctx->hash.digest);
            #endif /* WOLFSSL_SHA224 */
                    break;
                case WC_HASH_TYPE_SHA256:
            #ifndef NO_SHA256
                    wc_Sha256Free((wc_Sha256*)&ctx->hash.digest);
            #endif /* !NO_SHA256 */
                    break;
                case WC_HASH_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                    wc_Sha384Free((wc_Sha384*)&ctx->hash.digest);
            #endif /* WOLFSSL_SHA384 */
                    break;
                case WC_HASH_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                    wc_Sha512Free((wc_Sha512*)&ctx->hash.digest);
            #endif /* WOLFSSL_SHA512 */
                    break;
            #ifndef WOLFSSL_NOSHA512_224
                case WC_HASH_TYPE_SHA512_224:
            #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
                defined(WOLFSSL_SHA512)
                    wc_Sha512_224Free((wc_Sha512*)&ctx->hash.digest);
            #endif
                    break;
            #endif /* !WOLFSSL_NOSHA512_224 */
            #ifndef WOLFSSL_NOSHA512_256
                case WC_HASH_TYPE_SHA512_256:
             #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
                 defined(WOLFSSL_SHA512)
                    wc_Sha512_256Free((wc_Sha512*)&ctx->hash.digest);
            #endif
                    break;
            #endif /* !WOLFSSL_NOSHA512_256 */
                case WC_HASH_TYPE_SHA3_224:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
                    wc_Sha3_224_Free((wc_Sha3*)&ctx->hash.digest);
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_256:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
                    wc_Sha3_256_Free((wc_Sha3*)&ctx->hash.digest);
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_384:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
                    wc_Sha3_384_Free((wc_Sha3*)&ctx->hash.digest);
            #endif
                    break;
                case WC_HASH_TYPE_SHA3_512:
            #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
                    wc_Sha3_512_Free((wc_Sha3*)&ctx->hash.digest);
            #endif
                    break;
            #ifdef WOLFSSL_SM3
                case WC_HASH_TYPE_SM3:
                    wc_Sm3Free(&ctx->hash.digest.sm3);
                    break;
            #endif
                case WC_HASH_TYPE_NONE:
                    /* Not an error since an unused struct could be free'd or
                     * reset. */
                    break;
                case WC_HASH_TYPE_MD2:
                case WC_HASH_TYPE_MD4:
                case WC_HASH_TYPE_MD5_SHA:
                case WC_HASH_TYPE_BLAKE2B:
                case WC_HASH_TYPE_BLAKE2S:
            #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
                case WC_HASH_TYPE_SHAKE128:
            #endif
            #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
                case WC_HASH_TYPE_SHAKE256:
            #endif
                default:
                    ret = WOLFSSL_FAILURE;
                    break;
            }
        }
        ForceZero(ctx, sizeof(*ctx));
        ctx->macType = WC_HASH_TYPE_NONE;
        return ret;
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestInit(WOLFSSL_EVP_MD_CTX* ctx,
                               const WOLFSSL_EVP_MD* md)
    {
        int ret = WOLFSSL_SUCCESS;

        WOLFSSL_ENTER("EVP_DigestInit");

        if (ctx == NULL) {
            return BAD_FUNC_ARG;
        }


    #ifdef WOLFSSL_ASYNC_CRYPT
        /* compile-time validation of ASYNC_CTX_SIZE */
        typedef char async_test[WC_ASYNC_DEV_SIZE >= sizeof(WC_ASYNC_DEV) ?
                                                                        1 : -1];
        (void)sizeof(async_test);
    #endif

        /* Set to 0 if no match */
        ctx->macType = EvpMd2MacType(md);
        if (md == NULL) {
             XMEMSET(&ctx->hash.digest, 0, sizeof(WOLFSSL_Hasher));
        } else
    #ifndef NO_SHA
        if ((XSTRCMP(md, "SHA") == 0) || (XSTRCMP(md, "SHA1") == 0)) {
             ret = wolfSSL_SHA_Init(&(ctx->hash.digest.sha));
        } else
    #endif
    #ifndef NO_SHA256
        if (XSTRCMP(md, "SHA256") == 0) {
             ret = wolfSSL_SHA256_Init(&(ctx->hash.digest.sha256));
        } else
    #endif
    #ifdef WOLFSSL_SHA224
        if (XSTRCMP(md, "SHA224") == 0) {
             ret = wolfSSL_SHA224_Init(&(ctx->hash.digest.sha224));
        } else
    #endif
    #ifdef WOLFSSL_SHA384
        if (XSTRCMP(md, "SHA384") == 0) {
             ret = wolfSSL_SHA384_Init(&(ctx->hash.digest.sha384));
        } else
    #endif
    #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
        defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_224)
        if (XSTRCMP(md, "SHA512_224") == 0) {
             ret = wolfSSL_SHA512_224_Init(&(ctx->hash.digest.sha512));
        } else
    #endif
    #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
        defined(WOLFSSL_SHA512) && !defined(WOLFSSL_NOSHA512_256)
        if (XSTRCMP(md, "SHA512_256") == 0) {
             ret = wolfSSL_SHA512_256_Init(&(ctx->hash.digest.sha512));
        } else
    #endif
    #ifdef WOLFSSL_SHA512
        if (XSTRCMP(md, "SHA512") == 0) {
             ret = wolfSSL_SHA512_Init(&(ctx->hash.digest.sha512));
        } else
    #endif
    #ifndef NO_MD4
        if (XSTRCMP(md, "MD4") == 0) {
            wolfSSL_MD4_Init(&(ctx->hash.digest.md4));
        } else
    #endif
    #ifndef NO_MD5
        if (XSTRCMP(md, "MD5") == 0) {
            ret = wolfSSL_MD5_Init(&(ctx->hash.digest.md5));
        } else
    #endif
#ifdef WOLFSSL_SHA3
    #ifndef WOLFSSL_NOSHA3_224
        if (XSTRCMP(md, "SHA3_224") == 0) {
             ret = wolfSSL_SHA3_224_Init(&(ctx->hash.digest.sha3_224));
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_256
        if (XSTRCMP(md, "SHA3_256") == 0) {
             ret = wolfSSL_SHA3_256_Init(&(ctx->hash.digest.sha3_256));
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_384
        if (XSTRCMP(md, "SHA3_384") == 0) {
             ret = wolfSSL_SHA3_384_Init(&(ctx->hash.digest.sha3_384));
        } else
    #endif
    #ifndef WOLFSSL_NOSHA3_512
        if (XSTRCMP(md, "SHA3_512") == 0) {
             ret = wolfSSL_SHA3_512_Init(&(ctx->hash.digest.sha3_512));
        } else
    #endif
#endif
    #ifdef WOLFSSL_SM3
        if (XSTRCMP(md, "SM3") == 0) {
             ret = wc_InitSm3(&ctx->hash.digest.sm3, NULL, INVALID_DEVID);
             if (ret == 0) {
                ret = WOLFSSL_SUCCESS;
             }
             else {
                ret = WOLFSSL_FAILURE;
             }
        } else
    #endif
        {
             ctx->macType = WC_HASH_TYPE_NONE;
             return BAD_FUNC_ARG;
        }

        return ret;
    }

    /* WOLFSSL_SUCCESS on ok, WOLFSSL_FAILURE on failure */
    int wolfSSL_EVP_DigestUpdate(WOLFSSL_EVP_MD_CTX* ctx, const void* data,
                                size_t sz)
    {
        int ret = WOLFSSL_FAILURE;
        enum wc_HashType macType;

        WOLFSSL_ENTER("EVP_DigestUpdate");

        macType = EvpMd2MacType(EVP_MD_CTX_md(ctx));
        switch (macType) {
            case WC_HASH_TYPE_MD4:
        #ifndef NO_MD4
                wolfSSL_MD4_Update((MD4_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
                ret = WOLFSSL_SUCCESS;
        #endif
                break;
            case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
                ret = wolfSSL_MD5_Update((MD5_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
                ret = wolfSSL_SHA_Update((SHA_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
                ret = wolfSSL_SHA224_Update((SHA224_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
                ret = wolfSSL_SHA256_Update((SHA256_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif /* !NO_SHA256 */
                break;
            case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
                ret = wolfSSL_SHA384_Update((SHA384_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
                ret = wolfSSL_SHA512_Update((SHA512_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif /* WOLFSSL_SHA512 */
                break;

        #ifndef WOLFSSL_NOSHA512_224
            case WC_HASH_TYPE_SHA512_224:
        #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
            defined(WOLFSSL_SHA512)
                ret = wolfSSL_SHA512_224_Update((SHA512_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
        #endif /* !WOLFSSL_NOSHA512_224 */

        #ifndef WOLFSSL_NOSHA512_256
            case WC_HASH_TYPE_SHA512_256:
        #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
            defined(WOLFSSL_SHA512)
                ret = wolfSSL_SHA512_256_Update((SHA512_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif /* WOLFSSL_SHA512 */
                break;
        #endif /* !WOLFSSL_NOSHA512_256 */

            case WC_HASH_TYPE_SHA3_224:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
                ret = wolfSSL_SHA3_224_Update((SHA3_224_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA3_256:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
                ret = wolfSSL_SHA3_256_Update((SHA3_256_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA3_384:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
                ret = wolfSSL_SHA3_384_Update((SHA3_384_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
            case WC_HASH_TYPE_SHA3_512:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
                ret = wolfSSL_SHA3_512_Update((SHA3_512_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
        #endif
                break;
        #ifdef WOLFSSL_SM3
            case WC_HASH_TYPE_SM3:
                ret = wc_Sm3Update(&ctx->hash.digest.sm3, data, (word32)sz);
                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                }
                break;
        #endif
            case WC_HASH_TYPE_NONE:
            case WC_HASH_TYPE_MD2:
            case WC_HASH_TYPE_MD5_SHA:
            case WC_HASH_TYPE_BLAKE2B:
            case WC_HASH_TYPE_BLAKE2S:
        #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
            case WC_HASH_TYPE_SHAKE128:
        #endif
        #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
            case WC_HASH_TYPE_SHAKE256:
        #endif
            default:
                return WOLFSSL_FAILURE;
        }

        return ret;
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestFinal(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                               unsigned int* s)
    {
        int ret = WOLFSSL_FAILURE;
        enum wc_HashType macType;

        WOLFSSL_ENTER("EVP_DigestFinal");
        macType = EvpMd2MacType(EVP_MD_CTX_md(ctx));
        switch (macType) {
            case WC_HASH_TYPE_MD4:
        #ifndef NO_MD4
                wolfSSL_MD4_Final(md, (MD4_CTX*)&ctx->hash);
                if (s) *s = MD4_DIGEST_SIZE;
                ret = WOLFSSL_SUCCESS;
        #endif
                break;
            case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
                ret = wolfSSL_MD5_Final(md, (MD5_CTX*)&ctx->hash);
                if (s) *s = WC_MD5_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
                ret = wolfSSL_SHA_Final(md, (SHA_CTX*)&ctx->hash);
                if (s) *s = WC_SHA_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
                ret = wolfSSL_SHA224_Final(md, (SHA224_CTX*)&ctx->hash);
                if (s) *s = WC_SHA224_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
                ret = wolfSSL_SHA256_Final(md, (SHA256_CTX*)&ctx->hash);
                if (s) *s = WC_SHA256_DIGEST_SIZE;
        #endif /* !NO_SHA256 */
                break;
            case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
                ret = wolfSSL_SHA384_Final(md, (SHA384_CTX*)&ctx->hash);
                if (s) *s = WC_SHA384_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
                ret = wolfSSL_SHA512_Final(md, (SHA512_CTX*)&ctx->hash);
                if (s) *s = WC_SHA512_DIGEST_SIZE;
        #endif /* WOLFSSL_SHA512 */
                break;
        #ifndef WOLFSSL_NOSHA512_224
            case WC_HASH_TYPE_SHA512_224:
        #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
            defined(WOLFSSL_SHA512)
                ret = wolfSSL_SHA512_224_Final(md, (SHA512_CTX*)&ctx->hash);
                if (s) *s = WC_SHA512_224_DIGEST_SIZE;
        #endif
                break;
        #endif /* !WOLFSSL_NOSHA512_224 */
        #ifndef WOLFSSL_NOSHA512_256
            case WC_HASH_TYPE_SHA512_256:
        #if !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST) && \
            defined(WOLFSSL_SHA512)
                ret = wolfSSL_SHA512_256_Final(md, (SHA512_CTX*)&ctx->hash);
                if (s) *s = WC_SHA512_256_DIGEST_SIZE;
        #endif
                break;
        #endif /* !WOLFSSL_NOSHA512_256 */
            case WC_HASH_TYPE_SHA3_224:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
                ret = wolfSSL_SHA3_224_Final(md, (SHA3_224_CTX*)&ctx->hash);
                if (s) *s = WC_SHA3_224_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA3_256:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
                ret = wolfSSL_SHA3_256_Final(md, (SHA3_256_CTX*)&ctx->hash);
                if (s) *s = WC_SHA3_256_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA3_384:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
                ret = wolfSSL_SHA3_384_Final(md, (SHA3_384_CTX*)&ctx->hash);
                if (s) *s = WC_SHA3_384_DIGEST_SIZE;
        #endif
                break;
            case WC_HASH_TYPE_SHA3_512:
        #if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
                ret = wolfSSL_SHA3_512_Final(md, (SHA3_512_CTX*)&ctx->hash);
                if (s) *s = WC_SHA3_512_DIGEST_SIZE;
        #endif
                break;
        #ifdef WOLFSSL_SM3
            case WC_HASH_TYPE_SM3:
                ret = wc_Sm3Final(&ctx->hash.digest.sm3, md);
                if (ret == 0) {
                    ret = WOLFSSL_SUCCESS;
                }
                else {
                    ret = WOLFSSL_FAILURE;
                }
                if (s) *s = WC_SM3_DIGEST_SIZE;
                break;
        #endif
            case WC_HASH_TYPE_NONE:
            case WC_HASH_TYPE_MD2:
            case WC_HASH_TYPE_MD5_SHA:
            case WC_HASH_TYPE_BLAKE2B:
            case WC_HASH_TYPE_BLAKE2S:
        #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE128)
            case WC_HASH_TYPE_SHAKE128:
        #endif
        #if defined(WOLFSSL_SHA3) && defined(WOLFSSL_SHAKE256)
            case WC_HASH_TYPE_SHAKE256:
        #endif
            default:
                return WOLFSSL_FAILURE;
        }

        return ret;
    }

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestFinal_ex(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                                   unsigned int* s)
    {
        WOLFSSL_ENTER("EVP_DigestFinal_ex");
        return EVP_DigestFinal(ctx, md, s);
    }

    void wolfSSL_EVP_cleanup(void)
    {
        /* nothing to do here */
    }

const WOLFSSL_EVP_MD* wolfSSL_EVP_get_digestbynid(int id)
{
    WOLFSSL_MSG("wolfSSL_get_digestbynid");

    switch(id) {
#ifndef NO_MD5
        case NID_md5:
            return wolfSSL_EVP_md5();
#endif
#ifndef NO_SHA
        case NID_sha1:
            return wolfSSL_EVP_sha1();
#endif
#ifdef WOLFSSL_SHA224
        case NID_sha224:
            return wolfSSL_EVP_sha224();
#endif
#ifndef NO_SHA256
        case NID_sha256:
            return wolfSSL_EVP_sha256();
#endif
#ifdef WOLFSSL_SHA384
        case NID_sha384:
            return wolfSSL_EVP_sha384();
#endif
#ifdef WOLFSSL_SHA512
        case NID_sha512:
            return wolfSSL_EVP_sha512();
#endif
#ifdef WOLFSSL_SM3
        case NID_sm3:
            return wolfSSL_EVP_sm3();
#endif
        default:
            WOLFSSL_MSG("Bad digest id value");
    }

    return NULL;
}
int wolfSSL_EVP_MD_block_size(const WOLFSSL_EVP_MD* type)
{
    WOLFSSL_MSG("wolfSSL_EVP_MD_block_size");

    if (type == NULL) {
        WOLFSSL_MSG("No md type arg");
        return BAD_FUNC_ARG;
    }

#ifndef NO_SHA
    if ((XSTRCMP(type, "SHA") == 0) || (XSTRCMP(type, "SHA1") == 0)) {
        return WC_SHA_BLOCK_SIZE;
    } else
#endif
#ifndef NO_SHA256
    if (XSTRCMP(type, "SHA256") == 0) {
        return WC_SHA256_BLOCK_SIZE;
    } else
#endif
#ifndef NO_MD4
    if (XSTRCMP(type, "MD4") == 0) {
        return MD4_BLOCK_SIZE;
    } else
#endif
#ifndef NO_MD5
    if (XSTRCMP(type, "MD5") == 0) {
        return WC_MD5_BLOCK_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA224
    if (XSTRCMP(type, "SHA224") == 0) {
        return WC_SHA224_BLOCK_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA384
    if (XSTRCMP(type, "SHA384") == 0) {
        return WC_SHA384_BLOCK_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA512
    if (XSTRCMP(type, "SHA512") == 0) {
        return WC_SHA512_BLOCK_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA3
#ifndef WOLFSSL_NOSHA3_224
    if (XSTRCMP(type, "SHA3_224") == 0) {
        return WC_SHA3_224_BLOCK_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_256
    if (XSTRCMP(type, "SHA3_256") == 0) {
        return WC_SHA3_256_BLOCK_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_384
    if (XSTRCMP(type, "SHA3_384") == 0) {
        return WC_SHA3_384_BLOCK_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_512
    if (XSTRCMP(type, "SHA3_512") == 0) {
        return WC_SHA3_512_BLOCK_SIZE;
    }
#endif
#endif /* WOLFSSL_SHA3 */
#ifdef WOLFSSL_SM3
    if (XSTRCMP(type, "SM3") == 0) {
        return WC_SM3_BLOCK_SIZE;
    } else
#endif

    return BAD_FUNC_ARG;
}

int wolfSSL_EVP_MD_size(const WOLFSSL_EVP_MD* type)
{
    WOLFSSL_MSG("wolfSSL_EVP_MD_size");

    if (type == NULL) {
        WOLFSSL_MSG("No md type arg");
        return BAD_FUNC_ARG;
    }

#ifndef NO_SHA
    if ((XSTRCMP(type, "SHA") == 0) || (XSTRCMP(type, "SHA1") == 0)) {
        return WC_SHA_DIGEST_SIZE;
    } else
#endif
#ifndef NO_SHA256
    if (XSTRCMP(type, "SHA256") == 0) {
        return WC_SHA256_DIGEST_SIZE;
    } else
#endif
#ifndef NO_MD4
    if (XSTRCMP(type, "MD4") == 0) {
        return MD4_DIGEST_SIZE;
    } else
#endif
#ifndef NO_MD5
    if (XSTRCMP(type, "MD5") == 0) {
        return WC_MD5_DIGEST_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA224
    if (XSTRCMP(type, "SHA224") == 0) {
        return WC_SHA224_DIGEST_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA384
    if (XSTRCMP(type, "SHA384") == 0) {
        return WC_SHA384_DIGEST_SIZE;
    } else
#endif
#ifdef WOLFSSL_SHA512
    if (XSTRCMP(type, "SHA512") == 0) {
        return WC_SHA512_DIGEST_SIZE;
    } else
#ifndef WOLFSSL_NOSHA512_224
    if (XSTRCMP(type, "SHA512_224") == 0) {
        return WC_SHA512_224_DIGEST_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA512_256
    if (XSTRCMP(type, "SHA512_256") == 0) {
        return WC_SHA512_256_DIGEST_SIZE;
    } else
#endif
#endif
#ifdef WOLFSSL_SHA3
#ifndef WOLFSSL_NOSHA3_224
    if (XSTRCMP(type, "SHA3_224") == 0) {
        return WC_SHA3_224_DIGEST_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_256
    if (XSTRCMP(type, "SHA3_256") == 0) {
        return WC_SHA3_256_DIGEST_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_384
    if (XSTRCMP(type, "SHA3_384") == 0) {
        return WC_SHA3_384_DIGEST_SIZE;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_512
    if (XSTRCMP(type, "SHA3_512") == 0) {
        return WC_SHA3_512_DIGEST_SIZE;
    } else
#endif
#endif /* WOLFSSL_SHA3 */
#ifdef WOLFSSL_SM3
    if (XSTRCMP(type, "SM3") == 0) {
        return WC_SM3_DIGEST_SIZE;
    }
#endif

    return BAD_FUNC_ARG;
}

#endif /* OPENSSL_EXTRA  || HAVE_CURL */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* Subset of OPENSSL_EXTRA for PKEY operations PKEY free is needed by the
 * subset of X509 API */

WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new(void){
    return wolfSSL_EVP_PKEY_new_ex(NULL);
}

WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_ex(void* heap)
{
    WOLFSSL_EVP_PKEY* pkey;
    int ret;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_new_ex");
    pkey = (WOLFSSL_EVP_PKEY*)XMALLOC(sizeof(WOLFSSL_EVP_PKEY), heap,
            DYNAMIC_TYPE_PUBLIC_KEY);
    if (pkey != NULL) {
        XMEMSET(pkey, 0, sizeof(WOLFSSL_EVP_PKEY));
        pkey->heap = heap;
        pkey->type = WOLFSSL_EVP_PKEY_DEFAULT;

#ifndef HAVE_FIPS
        ret = wc_InitRng_ex(&pkey->rng, heap, INVALID_DEVID);
#else
        ret = wc_InitRng(&pkey->rng);
#endif
        if (ret != 0){
            /* Free directly since mutex for ref count not set yet */
            XFREE(pkey, heap, DYNAMIC_TYPE_PUBLIC_KEY);
            WOLFSSL_MSG("Issue initializing RNG");
            return NULL;
        }

        wolfSSL_RefInit(&pkey->ref, &ret);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (ret != 0){
            wolfSSL_EVP_PKEY_free(pkey);
            WOLFSSL_MSG("Issue initializing mutex");
            return NULL;
        }
    #else
        (void)ret;
    #endif
    }
    else {
        WOLFSSL_MSG("memory failure");
    }

    return pkey;
}

void wolfSSL_EVP_PKEY_free(WOLFSSL_EVP_PKEY* key)
{
    int doFree = 0;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_free");
    if (key != NULL) {
        int ret;
        wolfSSL_RefDec(&key->ref, &doFree, &ret);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (ret != 0) {
            WOLFSSL_MSG("Couldn't lock pkey mutex");
        }
    #else
        (void)ret;
    #endif

        if (doFree) {
            wc_FreeRng(&key->rng);

            if (key->pkey.ptr != NULL) {
                XFREE(key->pkey.ptr, key->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                key->pkey.ptr = NULL;
            }
            switch(key->type)
            {
                #ifndef NO_RSA
                case EVP_PKEY_RSA:
                    if (key->rsa != NULL && key->ownRsa == 1) {
                        wolfSSL_RSA_free(key->rsa);
                        key->rsa = NULL;
                    }
                    break;
                #endif /* NO_RSA */

                #if defined(HAVE_ECC) && defined(OPENSSL_EXTRA)
                case EVP_PKEY_EC:
                    if (key->ecc != NULL && key->ownEcc == 1) {
                        wolfSSL_EC_KEY_free(key->ecc);
                        key->ecc = NULL;
                    }
                    break;
                #endif /* HAVE_ECC && OPENSSL_EXTRA */

                #ifndef NO_DSA
                case EVP_PKEY_DSA:
                    if (key->dsa != NULL && key->ownDsa == 1) {
                        wolfSSL_DSA_free(key->dsa);
                        key->dsa = NULL;
                    }
                    break;
                #endif /* NO_DSA */

                #if !defined(NO_DH) && (defined(WOLFSSL_QT) || \
                       defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL))
                case EVP_PKEY_DH:
                    if (key->dh != NULL && key->ownDh == 1) {
                        wolfSSL_DH_free(key->dh);
                        key->dh = NULL;
                    }
                    break;
                #endif /* ! NO_DH ... */

                #ifdef HAVE_HKDF
                case EVP_PKEY_HKDF:
                    if (key->hkdfSalt != NULL) {
                        XFREE(key->hkdfSalt, NULL, DYNAMIC_TYPE_SALT);
                        key->hkdfSalt = NULL;
                    }
                    if (key->hkdfKey != NULL) {
                        XFREE(key->hkdfKey, NULL, DYNAMIC_TYPE_KEY);
                        key->hkdfKey = NULL;
                    }
                    if (key->hkdfInfo != NULL) {
                        XFREE(key->hkdfInfo, NULL, DYNAMIC_TYPE_INFO);
                        key->hkdfInfo = NULL;
                    }
                    key->hkdfSaltSz = 0;
                    key->hkdfKeySz = 0;
                    key->hkdfInfoSz = 0;
                    break;
                #endif /* HAVE_HKDF */

                #if defined(WOLFSSL_CMAC) && defined(OPENSSL_EXTRA) && \
                    defined(WOLFSSL_AES_DIRECT)
                case EVP_PKEY_CMAC:
                    if (key->cmacCtx != NULL) {
                        wolfSSL_CMAC_CTX_free(key->cmacCtx);
                        key->cmacCtx = NULL;
                    }
                    break;
                #endif /* defined(WOLFSSL_CMAC) ... */

                default:
                    break;
            }

            wolfSSL_RefFree(&key->ref);
            XFREE(key, key->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        }
    }
}
#if defined(OPENSSL_EXTRA) && !defined(NO_BIO)

/* Indent writes white spaces of the number specified by "indents"
 * to the BIO. The number of white spaces is limited from 0 to
 * EVP_PKEY_PRINT_INDENT_MAX.
 * returns the amount written to BIO.
 */
static int Indent(WOLFSSL_BIO* out, int indents)
{
    int i;
    char space = ' ';
    if (out == NULL) {
        return 0;
    }
    if (indents > EVP_PKEY_PRINT_INDENT_MAX) {
        indents = EVP_PKEY_PRINT_INDENT_MAX;
    }
    for (i = 0; i < indents; i++) {
        if (wolfSSL_BIO_write(out, &space, 1) < 0) {
            break;
        }
    }
    return indents -i;
}
/* PrintHexWithColon dump byte-data specified by "input" to the "out".
 * Each line has leading white spaces( "indent" gives the number ) plus
 * four spaces, then hex coded 15 byte data with separator ":" follow.
 * Each line looks like:
 * "    00:e6:ab: --- 9f:ef:"
 * Parameters:
 * out     bio to output dump data
 * input   buffer holding data to dump
 * inlen   input data size
 * indent  the number of spaces for indent
 * blower  true if lower case uses
 * Returns 1 on success, 0 on failure.
 */
static int PrintHexWithColon(WOLFSSL_BIO* out, const byte* input,
    int inlen, int indent, byte blower)
{
#ifdef WOLFSSL_SMALL_STACK
    byte*  buff = NULL;
#else
    byte   buff[EVP_PKEY_PRINT_LINE_WIDTH_MAX] = { 0 };
#endif /* WOLFSSL_SMALL_STACK */
    int    ret = WOLFSSL_SUCCESS;
    word32 in = 0;
    word32 i;
    int    idx;
    const  byte* data;
    word32 outSz;
    byte   outHex[3];

    if (!out || !input || inlen <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (indent < 0) {
        indent = 0;
    }
    if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }

    data = input;

#ifdef WOLFSSL_SMALL_STACK
    buff = (byte*)XMALLOC(EVP_PKEY_PRINT_LINE_WIDTH_MAX, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (!buff) {
        return WOLFSSL_FAILURE;
    }
#endif

    /* print pub element */
    idx = 0;

    for (in = 0; in < (word32)inlen && ret == WOLFSSL_SUCCESS; in +=
                                        EVP_PKEY_PRINT_DIGITS_PER_LINE ) {
        Indent(out, indent);
        for (i = 0; (i < EVP_PKEY_PRINT_DIGITS_PER_LINE) &&
                                        (in + i < (word32)inlen); i++) {

            if (ret == WOLFSSL_SUCCESS) {
                outSz = sizeof(outHex);
                ret = Base16_Encode((const byte*)&data[in + i], 1,
                                                    outHex, &outSz) == 0;
            }
            if (ret == WOLFSSL_SUCCESS) {
                if (blower) {
                    outHex[0] = (byte)XTOLOWER(outHex[0]);
                    outHex[1] = (byte)XTOLOWER(outHex[1]);
                }
                XMEMCPY(buff + idx, outHex, 2);
                idx += 2;

                if (in + i != (word32)inlen -1) {
                    XMEMSET(buff + idx, ':', 1);
                    idx += 1;
                }
            }
        }
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_BIO_write(out, buff, idx) > 0;
        }
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_BIO_write(out, "\n", 1) > 0;
        }
        if (ret == WOLFSSL_SUCCESS) {
            XMEMSET(buff, 0, EVP_PKEY_PRINT_LINE_WIDTH_MAX);
            idx = 0;
        }
    }
#ifdef WOLFSSL_SMALL_STACK
    XFREE(buff, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return ret;
}
#if !defined(NO_RSA)
/* PrintPubKeyRSA is a helper function for wolfSSL_EVP_PKEY_print_public
 * to parse a DER format RSA public key specified in the second parameter.
 * Parameters:
 * out     bio to output dump data
 * pkey    buffer holding public key data
 * pkeySz  public key data size
 * indent  the number of spaces for indent
 * bitlen  bit size of the given key
 * pctx    context(not used)
 * Returns 1 on success, 0 on failure.
*/
static int PrintPubKeyRSA(WOLFSSL_BIO* out, const byte* pkey, int pkeySz,
    int indent, int bitlen, ASN1_PCTX* pctx)
{
    byte   buff[8] = { 0 };
    int    res = WOLFSSL_FAILURE;
    word32 inOutIdx = 0;
    word32 nSz;             /* size of modulus */
    word32 eSz;             /* size of public exponent */
    const byte*  n   = NULL;
    const byte*  e   = NULL; /* pointer to modulus/exponent */
    word32 i;
    unsigned long exponent = 0;
#ifdef WOLFSSL_SMALL_STACK
    mp_int* a = NULL;
#else
    mp_int a[1];
#endif
    char line[32] = { 0 };

    (void)pctx;

#ifdef WOLFSSL_SMALL_STACK
    a = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (a == NULL) {
        return WOLFSSL_FAILURE;
    }
#endif

    if( mp_init(a) != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return WOLFSSL_FAILURE;
    }
    if (indent < 0) {
        indent = 0;
    }
    if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }

    do {
        int idx;
        int wsz;

        /* parse key to get modulus and exponent */
        if (wc_RsaPublicKeyDecode_ex(pkey, &inOutIdx, (word32)pkeySz,
                                                &n, &nSz, &e, &eSz) != 0) {
            break;
        }

        /* print out public key elements */
        idx = 0;
        XMEMSET(buff, 0, sizeof(buff));
        Indent(out, indent);
        XSTRNCPY(line, "RSA Public-Key: (", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (mp_set_int(a, (unsigned long)bitlen) != 0) {
            break;
        }
        if (mp_todecimal(a, (char*)buff) != 0) {
            break;
        }
        wsz = (int)XSTRLEN((const char*)buff);

        if (wolfSSL_BIO_write(out, buff + idx, wsz) <= 0) {
            break;
        }
        XSTRNCPY(line, " bit)\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        /* print Modulus */
        Indent(out, indent);
        XSTRNCPY(line, "Modulus:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }

        /* print modulus with leading zero if exists */
        if (*n & 0x80 && *(n-1) == 0) {
            n--;
            nSz++;
        }

        if (PrintHexWithColon(out, n, (int)nSz,
                    indent + 4, 1/* lower case */) != WOLFSSL_SUCCESS) {
            break;
        }
        /* print public Exponent */
        idx = 0;
        Indent(out, indent);
        XSTRNCPY(line, "Exponent: ", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        for (i = 0; i < eSz; i++) {
            exponent <<= 8;
            exponent += e[i];
        }

        XMEMSET(buff, 0, sizeof(buff));
        if (mp_set_int(a, exponent) != 0) {
            break;
        }
        if (mp_todecimal(a, (char*)buff) != 0) {
            break;
        }
        wsz = (int)XSTRLEN((const char*)buff);

        if (wolfSSL_BIO_write(out, buff + idx, wsz) <= 0) {
            break;
        }
        XSTRNCPY(line, " (0x", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        XMEMSET(buff, 0, sizeof(buff));
        if (mp_tohex(a, (char*)buff) != 0) {
            break;
        }
        if (wolfSSL_BIO_write(out, buff, (int)XSTRLEN((char*)buff)) <= 0) {
            break;
        }
        XSTRNCPY(line, ")\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }

        res = WOLFSSL_SUCCESS;
    } while (0);

    mp_free(a);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
    return res;
}
#endif /* !NO_RSA */

#if defined(HAVE_ECC)
/* PrintPubKeyEC is a helper function for wolfSSL_EVP_PKEY_print_public
 * to parse a DER format ECC public key specified in the second parameter.
 * Parameters:
 * out     bio to output dump data
 * pkey    buffer holding public key data
 * pkeySz  public key data size
 * indent  the number of spaces for indent
 * bitlen  bit size of the given key
 * pctx    context(not used)
 * Returns 1 on success, 0 on failure.
*/
static int PrintPubKeyEC(WOLFSSL_BIO* out, const byte* pkey, int pkeySz,
    int indent, int bitlen, ASN1_PCTX* pctx)
{
    byte*   pub = NULL;
    word32  pubSz = 0;
    byte    buff[8] = { 0 };
    int     res = WOLFSSL_SUCCESS;
    word32  inOutIdx = 0;
    int     curveId = 0;
    const byte* curveOID = NULL;
    word32  oidSz = 0;
    const char* OIDName = NULL;
    const char* nistCurveName = NULL;
    int idx = 0;
    int wsz = 0;
#ifdef WOLFSSL_SMALL_STACK
    mp_int* a = NULL;
    ecc_key* key = NULL;
#else
    mp_int  a[1];
    ecc_key key[1];
#endif
    char line[32] = { 0 };
    (void)pctx;

#ifdef WOLFSSL_SMALL_STACK
    a = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (a == NULL) {
        WOLFSSL_MSG("Failed to allocate memory for mp_int");
        return WOLFSSL_FAILURE;
    }
    XMEMSET(a, 0, sizeof(mp_int));

    key = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
    if (key == NULL) {
        WOLFSSL_MSG("Failed to allocate memory for ecc_key");
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
        return WOLFSSL_FAILURE;
    }
#endif

    if (mp_init(a) != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(key, NULL, DYNAMIC_TYPE_ECC);
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return WOLFSSL_FAILURE;
    }

    if (wc_ecc_init(key) != 0) {
        /* Return early so we don't have to remember if init succeeded
         * or not. */
        mp_free(a);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(key, NULL, DYNAMIC_TYPE_ECC);
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return WOLFSSL_FAILURE;
    }

    if (indent < 0) {
        indent = 0;
    }
    else if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }

    if (res == WOLFSSL_SUCCESS) {
        res = wc_EccPublicKeyDecode(pkey, &inOutIdx, key, (word32)pkeySz) == 0;
    }

    if (res == WOLFSSL_SUCCESS) {
        curveId = wc_ecc_get_oid(key->dp->oidSum, &curveOID, &oidSz);
        res = curveId > 0 && oidSz > 0;
    }

    /* get NIST curve name */
    if (res == WOLFSSL_SUCCESS) {
        int nid = EccEnumToNID(curveId);
        if (nid != -1) {
            /* look up object name and nist curve name*/
            OIDName = wolfSSL_OBJ_nid2sn(nid);
            nistCurveName = wolfSSL_EC_curve_nid2nist(nid);
            res = (nistCurveName != NULL) && (OIDName != NULL);
        }
        else {
            res = WOLFSSL_FAILURE;
        }
    }
    if (res == WOLFSSL_SUCCESS) {
        pub = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
        if (pub != NULL) {
            pubSz = ECC_BUFSIZE;
            XMEMSET(pub, 0, ECC_BUFSIZE);

            PRIVATE_KEY_UNLOCK();
            res = wc_ecc_export_x963(key, pub, &pubSz) == 0;
            PRIVATE_KEY_LOCK();
        }
        else {
            res = WOLFSSL_FAILURE;
        }
    }
    if (res == WOLFSSL_SUCCESS) {
        idx = 0;
        res = Indent(out, indent) >= 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        XSTRNCPY(line, "Public-Key: (", sizeof(line));
        res = wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = mp_set_int(a, (unsigned long)bitlen) == 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = mp_todecimal(a, (char*)buff) == 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        wsz = (int)XSTRLEN((const char*)buff);
    }
    if (res == WOLFSSL_SUCCESS) {
        res = wolfSSL_BIO_write(out, buff + idx, wsz) >= 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        XSTRNCPY(line, " bit)\n", sizeof(line));
        res = wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = Indent(out, indent) >= 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        /* print pub element */
        XSTRNCPY(line, "pub:\n", sizeof(line));
        res = wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        /* upper case */
        res = PrintHexWithColon(out, pub, (int)pubSz, indent + 4, 0);
    }
    if (res == WOLFSSL_SUCCESS) {
        res = Indent(out, indent) >= 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        /* print OID in name */
        XSTRNCPY(line, "ASN1 OID: ", sizeof(line));
        res = wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = wolfSSL_BIO_write(out, OIDName, (int)XSTRLEN(OIDName)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = wolfSSL_BIO_write(out, "\n", 1) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = Indent(out, indent) >= 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        /* print NIST curve name */
        XSTRNCPY(line, "NIST CURVE: ", sizeof(line));
        res = wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = wolfSSL_BIO_write(out, nistCurveName,
            (int)XSTRLEN(nistCurveName)) > 0;
    }
    if (res == WOLFSSL_SUCCESS) {
        res = wolfSSL_BIO_write(out, "\n", 1) > 0;
    }

    if (pub != NULL) {
        XFREE(pub, NULL, DYNAMIC_TYPE_ECC_BUFFER);
        pub = NULL;
    }

    wc_ecc_free(key);
    mp_free(a);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(key, NULL, DYNAMIC_TYPE_ECC);
    XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif

    return res;
}
#endif /* HAVE_ECC */

#if !defined(NO_DSA)
/* PrintPubKeyDSA is a helper function for wolfSSL_EVP_PKEY_print_public
 * to parse a DER format DSA public key specified in the second parameter.
 * Parameters:
 * out     bio to output dump data
 * pkey    buffer holding public key data
 * pkeySz  public key data size
 * indent  the number of spaces for indent
 * bitlen  bit size of the given key
 * pctx    context(not used)
 * Returns 1 on success, 0 on failure.
*/
static int PrintPubKeyDSA(WOLFSSL_BIO* out, const byte* pkey, int pkeySz,
    int indent, int bitlen, ASN1_PCTX* pctx)
{

    byte    buff[8] = { 0 };
    int     length;
    int     res = WOLFSSL_FAILURE;
    word32  inOutIdx = 0;
    word32  oid;
    byte    tagFound;
#ifdef WOLFSSL_SMALL_STACK
    mp_int* a = NULL;
#else
    mp_int  a[1];
#endif
    char line[32] = { 0 };

#ifdef WOLFSSL_SMALL_STACK
    a = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (a == NULL) {
        return WOLFSSL_FAILURE;
    }
#endif

    if( mp_init(a) != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return WOLFSSL_FAILURE;
    }

    inOutIdx = 0;
    (void)pctx;

    if (indent < 0) {
        indent = 0;
    }
    if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }

    do {
        byte *p = NULL, * q = NULL, * g = NULL, * y = NULL;
        int pSz, qSz, gSz, ySz;
        int idx;
        int wsz;

        if (GetSequence(pkey, &inOutIdx, &length, (word32)pkeySz) < 0) {
            break;
        }
        if (GetSequence(pkey, &inOutIdx, &length, (word32)pkeySz) < 0) {
            break;
        }
        if (GetObjectId(pkey, &inOutIdx, &oid, oidIgnoreType, (word32)pkeySz) !=
                0) {
            break;
        }
        if (GetSequence(pkey, &inOutIdx, &length, (word32)pkeySz) < 0) {
            break;
        }
        /* find P */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, &length, (word32)pkeySz) <= 0) {
            break;
        }
        p   = (byte*)(pkey + inOutIdx);
        pSz = length;

        if (bitlen == 0) {
            if (*p == 0) {
                bitlen = (pSz - 1) * 8;    /* remove leading zero */
            }
            else {
                bitlen = pSz * 8;
            }
        }

        inOutIdx += (word32)length;
        /* find Q */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, &length, (word32)pkeySz) <= 0) {
            break;
        }
        q = (byte*)(pkey + inOutIdx);
        qSz = length;
        inOutIdx += (word32)length;

        /* find G */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, &length, (word32)pkeySz) <= 0) {
            break;
        }
        g = (byte*)(pkey + inOutIdx);
        gSz = length;
        inOutIdx += (word32)length;
        /* find Y */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_BIT_STRING) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, &length, (word32)pkeySz) <= 0) {
            break;
        }
        inOutIdx++;     /* skip the first byte( unused byte number)*/

        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, &length, (word32)pkeySz) <= 0) {
            break;
        }
        y = (byte*)(pkey + inOutIdx);
        ySz = length;

        idx = 0;
        XMEMSET(buff, 0, sizeof(buff));
        Indent(out, indent);
        XSTRNCPY(line, "DSA Public-Key: (", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (mp_set_int(a, (unsigned long)bitlen) != 0) {
            break;
        }
        if (mp_todecimal(a, (char*)buff) != 0) {
            break;
        }
        wsz = (int)XSTRLEN((const char*)buff);
        if (wolfSSL_BIO_write(out, buff + idx, wsz) <= 0) {
            break;
        }
        XSTRNCPY(line, " bit)\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        /* print pub element */
        Indent(out, indent);
        XSTRNCPY(line, "pub:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, y, ySz, indent + 4, 0/* upper case */)
                                                        != WOLFSSL_SUCCESS) {
            break;
        }
        /* print P element */
        Indent(out, indent);
        XSTRNCPY(line, "P:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, p, pSz, indent + 4, 0/* upper case */)
                                                        != WOLFSSL_SUCCESS) {
            break;
        }
        /* print Q element */
        Indent(out, indent);
        XSTRNCPY(line, "Q:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, q, qSz, indent + 4, 0/* upper case */)
                                                         != WOLFSSL_SUCCESS) {
            break;
        }
        /* print G element */
        Indent(out, indent);
        XSTRNCPY(line, "G:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, g, gSz, indent + 4, 0/* upper case */)
                                                        != WOLFSSL_SUCCESS) {
            break;
        }

        res = WOLFSSL_SUCCESS;
    } while (0);

    mp_free(a);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
    return res;
}
#endif /* !NO_DSA */

#if defined(WOLFSSL_DH_EXTRA)
/* PrintPubKeyDH is a helper function for wolfSSL_EVP_PKEY_print_public
 * to parse a DER format DH public key specified in the second parameter.
 * Parameters:
 * out     bio to output dump data
 * pkey    buffer holding public key data
 * pkeySz  public key data size
 * indent  the number of spaces for indent
 * bitlen  bit size of the given key
 * pctx    context(not used)
 * Returns 1 on success, 0 on failure.
*/
static int PrintPubKeyDH(WOLFSSL_BIO* out, const byte* pkey, int pkeySz,
    int indent, int bitlen, ASN1_PCTX* pctx)
{

    byte    buff[8] = { 0 };
    int     res = WOLFSSL_FAILURE;
    word32  length;
    word32  inOutIdx;
    word32  oid;
    byte    tagFound;
    byte*   prime = NULL;
    byte    generator;
    byte*   publicKey = NULL;
    word32  outSz;
    byte    outHex[3];
#ifdef WOLFSSL_SMALL_STACK
    mp_int* a = NULL;
#else
    mp_int  a[1];
#endif
    char line[32] = { 0 };

#ifdef WOLFSSL_SMALL_STACK
    a = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (a == NULL) {
        return WOLFSSL_FAILURE;
    }
#endif

    if( mp_init(a) != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return WOLFSSL_FAILURE;
    }

    inOutIdx = 0;
    (void)pctx;

    if (indent < 0) {
        indent = 0;
    }
    if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }

    do {
        int primeSz;
        int publicKeySz;
        int idx;
        int wsz;

        if (GetSequence(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) < 0) {
            break;
        }
        if (GetSequence(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) < 0) {
            break;
        }
        if (GetObjectId(pkey, &inOutIdx, &oid, oidIgnoreType, (word32)pkeySz) <
                0) {
            break;
        }
        if (GetSequence(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) < 0) {
            break;
        }
        /* get prime element */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) <= 0) {
            break;
        }
        prime     = (byte*)(pkey + inOutIdx);
        primeSz   = (int)length;
        inOutIdx += length;

        /* get generator element */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) <= 0) {
            break;
        }
        if (length != 1) {
            break;
        }
        generator = *(pkey + inOutIdx);
        inOutIdx += length;

        /* get public-key element */
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_BIT_STRING) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) <= 0) {
            break;
        }
        inOutIdx ++;
        if (GetASNTag(pkey, &inOutIdx, &tagFound, (word32)pkeySz) != 0) {
            break;
        }
        if (tagFound != ASN_INTEGER) {
            break;
        }
        if (GetLength(pkey, &inOutIdx, (int*)&length, (word32)pkeySz) <= 0) {
            break;
        }
        publicKeySz = (int)length;
        publicKey = (byte*)(pkey + inOutIdx);

        if (bitlen == 0) {
            if (*publicKey == 0) {
                bitlen = (publicKeySz - 1) * 8;
            }
            else {
                bitlen = publicKeySz * 8;
            }
        }

        /* print elements */
        idx = 0;
        Indent(out, indent);
        XSTRNCPY(line, "DH Public-Key: (", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (mp_set_int(a, (unsigned long)bitlen) != 0) {
            break;
        }
        if (mp_todecimal(a, (char*)buff) != 0) {
            break;
        }
        wsz = (int)XSTRLEN((const char*)buff);
        if (wolfSSL_BIO_write(out, buff + idx, wsz) <= 0) {
            break;
        }
        XSTRNCPY(line, " bit)\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        Indent(out, indent);
        XSTRNCPY(line, "public-key:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, publicKey,
                                publicKeySz, indent + 4, 0/* upper case */)
                                                    != WOLFSSL_SUCCESS) {
            break;
        }
        Indent(out, indent);
        XSTRNCPY(line, "prime:\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (PrintHexWithColon(out, prime, primeSz,
                                            indent + 4, 0/* upper case */)
                != WOLFSSL_SUCCESS) {
            break;
        }
        idx = 0;
        XMEMSET(buff, 0, sizeof(buff));
        Indent(out, indent);
        XSTRNCPY(line, "generator: ", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        if (mp_set_int(a, generator) != 0) {
            break;
        }
        if (mp_todecimal(a, (char*)buff) != 0) {
            break;
        }
        wsz = (int)XSTRLEN((const char*)buff);
        if (wolfSSL_BIO_write(out, buff + idx, wsz) <= 0) {
            break;
        }
        XSTRNCPY(line, " (0x", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }
        idx = 0;
        XMEMSET(buff, 0, sizeof(buff));
        outSz = sizeof(outHex);
        if (Base16_Encode((const byte*)&generator, 1, outHex, &outSz ) != 0) {
            break;
        }
        if (idx + 2 < (int)sizeof(buff) ) {
            XMEMCPY(buff + idx, outHex, 2);
            idx += 2;
        }
        if (wolfSSL_BIO_write(out, buff, idx) <= 0 ) {
            break;
        }
        XSTRNCPY(line, ")\n", sizeof(line));
        if (wolfSSL_BIO_write(out, line, (int)XSTRLEN(line)) <= 0) {
            break;
        }

        res = WOLFSSL_SUCCESS;
    } while (0);

    mp_free(a);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(a, NULL, DYNAMIC_TYPE_BIGINT);
#endif
    return res;
}
#endif /* WOLFSSL_DH_EXTRA */

/* wolfSSL_EVP_PKEY_print_public parses the specified key then
 * outputs public key info in human readable format to the specified BIO.
 * White spaces of the same number which 'indent" gives, will be added to
 * each line to output and ignores pctx parameter.
 * Parameters:
 * out     bio to output dump data
 * pkey    buffer holding public key data
 * indent  the number of spaces for indent
 * pctx    context(not used)
 * Returns 1 on success, 0 or negative on error, -2 means specified key
 * algo is not supported.
 * Can handle RSA, ECC, DSA and DH public keys.
 */
int wolfSSL_EVP_PKEY_print_public(WOLFSSL_BIO* out,
    const WOLFSSL_EVP_PKEY* pkey, int indent, ASN1_PCTX* pctx)
{
    int res;
#if !defined(NO_RSA) || defined(HAVE_ECC) || !defined(NO_DSA) || \
    defined(WOLFSSL_DH_EXTRA)
    int keybits;    /* bit length of the key */
#endif

    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_print_public");

    if (pkey == NULL || out == NULL) {
        return 0;
    }
#if !defined(NO_RSA) || defined(HAVE_ECC) || !defined(NO_DSA) || \
    defined(WOLFSSL_DH_EXTRA)
    if (indent < 0) {
        indent = 0;
    }
    if (indent > EVP_PKEY_PRINT_INDENT_MAX) {
        indent = EVP_PKEY_PRINT_INDENT_MAX;
    }
#endif

    switch (pkey->type) {
        case EVP_PKEY_RSA:

#if !defined(NO_RSA)
            keybits = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey) * 8;
            res     = PrintPubKeyRSA(
                        out,
                        (byte*)(pkey->pkey.ptr), /* buffer for pkey raw data */
                        pkey->pkey_sz,           /* raw pkey size */
                        indent,                  /* indent size */
                        keybits,                 /* bit length of the key */
                        pctx);                   /* not used */
#else
            res = WOLFSSL_UNKNOWN;       /* not supported algo */
#endif
            break;

        case EVP_PKEY_EC:

#if defined(HAVE_ECC)
            keybits = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey) * 8;
            res     = PrintPubKeyEC(
                        out,
                        (byte*)(pkey->pkey.ptr),  /* buffer for pkey raw data */
                        pkey->pkey_sz,            /* raw pkey size */
                        indent,                   /* indent size */
                        keybits,                  /* bit length of the key */
                        pctx);                    /* not used */
#else
            res = WOLFSSL_UNKNOWN;       /* not supported algo */
#endif
            break;

        case EVP_PKEY_DSA:

#if !defined(NO_DSA)
            keybits = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey) * 8;
            res     = PrintPubKeyDSA(
                        out,
                        (byte*)(pkey->pkey.ptr),  /* buffer for pkey raw data */
                        pkey->pkey_sz,            /* raw pkey size */
                        indent,                   /* indent size */
                        keybits,                  /* bit length of the key */
                        pctx);                    /* not used */
#else
            res = WOLFSSL_UNKNOWN;       /* not supported algo */
#endif
            break;

        case EVP_PKEY_DH:

#if defined(WOLFSSL_DH_EXTRA)
            keybits = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey) * 8;
            res     = PrintPubKeyDH(
                        out,
                        (byte*)(pkey->pkey.ptr),  /* buffer for pkey raw data */
                        pkey->pkey_sz,            /* raw pkey size */
                        indent,                   /* indent size */
                        keybits,                  /* bit length of the key */
                        pctx);                    /* not used */
#else
            res = WOLFSSL_UNKNOWN;       /* not supported algo */
#endif
            break;

        default:
            res = WOLFSSL_UNKNOWN;      /* not supported algo */
            break;
    }
    return res;
}
#endif /* OPENSSL_EXTRA && !NO_BIO */

int wolfSSL_EVP_get_hashinfo(const WOLFSSL_EVP_MD* evp,
    int* pHash, int* pHashSz)
{
    enum wc_HashType hash = WC_HASH_TYPE_NONE;
    int hashSz;

    if (XSTRLEN(evp) < 3) {
        /* do not try comparing strings if size is too small */
        return WOLFSSL_FAILURE;
    }

#ifndef NO_SHA
    if ((XSTRCMP("SHA", evp) == 0) || (XSTRCMP("SHA1", evp) == 0)) {
        hash = WC_HASH_TYPE_SHA;
    } else
#endif
#ifdef WOLFSSL_SHA224
    if (XSTRCMP("SHA224", evp) == 0) {
        hash = WC_HASH_TYPE_SHA224;
    } else
#endif
#ifndef NO_SHA256
    if (XSTRCMP("SHA256", evp) == 0) {
        hash = WC_HASH_TYPE_SHA256;
    } else
#endif
#ifdef WOLFSSL_SHA384
    if (XSTRCMP("SHA384", evp) == 0) {
        hash = WC_HASH_TYPE_SHA384;
    } else
#endif
#ifdef WOLFSSL_SHA512
    if (XSTRCMP("SHA512", evp) == 0) {
        hash = WC_HASH_TYPE_SHA512;
    } else
#ifndef WOLFSSL_NOSHA512_224
    if (XSTRCMP("SHA512_224", evp) == 0) {
        hash = WC_HASH_TYPE_SHA512_224;
    } else
#endif
#ifndef WOLFSSL_NOSHA512_256
    if (XSTRCMP("SHA512_256", evp) == 0) {
        hash = WC_HASH_TYPE_SHA512_256;
    } else
#endif
#endif
#ifdef WOLFSSL_SHA3
#ifndef WOLFSSL_NOSHA3_224
    if (XSTRCMP("SHA3_224", evp) == 0) {
        hash = WC_HASH_TYPE_SHA3_224;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_256
    if (XSTRCMP("SHA3_256", evp) == 0) {
        hash = WC_HASH_TYPE_SHA3_256;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_384
    if (XSTRCMP("SHA3_384", evp) == 0) {
        hash = WC_HASH_TYPE_SHA3_384;
    } else
#endif
#ifndef WOLFSSL_NOSHA3_512
    if (XSTRCMP("SHA3_512", evp) == 0) {
        hash = WC_HASH_TYPE_SHA3_512;
    } else
#endif
#endif /* WOLFSSL_SHA3 */
#ifdef WOLFSSL_SM3
    if (XSTRCMP("SM3", evp) == 0) {
        hash = WC_HASH_TYPE_SM3;
    } else
#endif
#ifdef WOLFSSL_MD2
    if (XSTRCMP("MD2", evp) == 0) {
        hash = WC_HASH_TYPE_MD2;
    } else
#endif
#ifndef NO_MD4
    if (XSTRCMP("MD4", evp) == 0) {
        hash = WC_HASH_TYPE_MD4;
    } else
#endif
#ifndef NO_MD5
    if (XSTRCMP("MD5", evp) == 0) {
        hash = WC_HASH_TYPE_MD5;
    } else
#endif
    {
        if (XSTRNCMP("SHA", evp, 3) == 0) {
            WOLFSSL_MSG("Unknown SHA hash");
        }
        return WOLFSSL_FAILURE;
    }

    if (pHash)
        *pHash = hash;

    hashSz = wc_HashGetDigestSize(hash);
    if (pHashSz)
        *pHashSz = hashSz;

    if (hashSz < 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* Base64 encoding APIs */
#if defined(WOLFSSL_BASE64_ENCODE) || defined(WOLFSSL_BASE64_DECODE)

/*  wolfSSL_EVP_ENCODE_CTX_new allocates WOLFSSL_EVP_ENCODE_CTX
 *  Returns WOLFSSL_EVP_ENCODE_CTX structure on success, NULL on failure.
 */
struct WOLFSSL_EVP_ENCODE_CTX* wolfSSL_EVP_ENCODE_CTX_new(void)
{
    WOLFSSL_EVP_ENCODE_CTX* ctx = NULL;
    WOLFSSL_ENTER("wolfSSL_EVP_ENCODE_CTX_new");
    ctx = (WOLFSSL_EVP_ENCODE_CTX*)XMALLOC(sizeof(WOLFSSL_EVP_ENCODE_CTX),
             NULL, DYNAMIC_TYPE_OPENSSL );

    if (ctx != NULL) {
        XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_ENCODE_CTX) );
        ctx->heap = NULL;
        return ctx;
    }
    return NULL;
}
/*  wolfSSL_EVP_ENCODE_CTX_free frees specified WOLFSSL_EVP_ENCODE_CTX struct.
 */
void wolfSSL_EVP_ENCODE_CTX_free(WOLFSSL_EVP_ENCODE_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_EVP_ENCODE_CTX_free");
    if (ctx != NULL) {
        XFREE(ctx, ctx->heap, DYNAMIC_TYPE_OPENSSL);
    }
}
#endif /* WOLFSSL_BASE64_ENCODE || WOLFSSL_BASE64_DECODE */
#if defined(WOLFSSL_BASE64_ENCODE)

/* Assume that out has enough space */
int wolfSSL_EVP_EncodeBlock(unsigned char *out, const unsigned char *in,
                            int inLen)
{
    word32 ret = (word32)-1;
    WOLFSSL_ENTER("wolfSSL_EVP_EncodeBlock");

    if (out == NULL || in == NULL)
        return WOLFSSL_FATAL_ERROR;

    if (Base64_Encode(in, (word32)inLen, out, &ret) == 0)
        return (int)ret;
    else
        return WOLFSSL_FATAL_ERROR;
}

/* Assume that out has enough space */
int wolfSSL_EVP_DecodeBlock(unsigned char *out, const unsigned char *in,
                            int inLen)
{
    word32 ret = (word32)-1;
    WOLFSSL_ENTER("wolfSSL_EVP_DecodeBlock");

    if (out == NULL || in == NULL)
        return WOLFSSL_FATAL_ERROR;

    if (Base64_Decode(in, (word32)inLen, out, &ret) == 0)
        return (int)ret;
    else
        return WOLFSSL_FATAL_ERROR;

}

/*  wolfSSL_EVP_EncodeInit initializes specified WOLFSSL_EVP_ENCODE_CTX object
 *  for the subsequent wolfSSL_EVP_EncodeUpdate.
 */
void wolfSSL_EVP_EncodeInit(WOLFSSL_EVP_ENCODE_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_EVP_EncodeInit");

    /* clean up ctx */
    if (ctx != NULL) {
        ctx->remaining = 0;
        XMEMSET(ctx->data, 0, sizeof(ctx->data));
    }
}
/*  wolfSSL_EVP_EncodeUpdate encodes the input data in 48-byte units
 *  and outputs it to out. If less than 48 bytes of data remain, save it in
 *  ctx. The data given in the subsequent wolfSSL_EVP_EncodeUpdate
 *  is combined with the data stored in CTX and used for encoding.
 *  Returns 1 on success, 0 on error.
 */
int  wolfSSL_EVP_EncodeUpdate(WOLFSSL_EVP_ENCODE_CTX* ctx,
            unsigned char* out, int* outl, const unsigned char* in, int inl)
{
    int res;
    word32 outsz = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_EncodeUpdate");

    if (ctx == NULL || out == NULL || in == NULL || outl == NULL)
        return 0;

    *outl = 0;

    /* if the remaining data exists in the ctx, add input data to them
     * to create a block(48bytes) for encoding
     */
    if (ctx->remaining > 0 && inl > 0) {
        int cpysz = (int)min(
            (word32)(BASE64_ENCODE_BLOCK_SIZE - ctx->remaining), (word32)inl);
        XMEMCPY(ctx->data + ctx->remaining, in, (size_t)cpysz);
        ctx->remaining += cpysz;
        in             += cpysz;
        inl            -= cpysz;

        /* check if a block for encoding exists in ctx.data, if so encode it */
        if (ctx->remaining >= BASE64_ENCODE_BLOCK_SIZE) {
            /* Base64_Encode asks the out buff size via the 4th param*/
            outsz = BASE64_ENCODE_RESULT_BLOCK_SIZE + 1;
            res = Base64_Encode(ctx->data, BASE64_ENCODE_BLOCK_SIZE, out,
                &outsz);
            if (res == 0) {
                ctx->remaining = 0;
                *outl = (int)outsz;
            }
            else
                return 0;   /* return with error */
        }
        else {
            /* could not create a block */
            *outl = 0;
            return  1;
        }
    }
    /* Here, there is no data left in ctx, so try processing the data of
     * the specified input data.
     */

    while (inl >= BASE64_ENCODE_BLOCK_SIZE) {
        outsz = BASE64_ENCODE_RESULT_BLOCK_SIZE + 1;/* 64 byte and one for LF*/
        res = Base64_Encode(in, BASE64_ENCODE_BLOCK_SIZE,out,&outsz);
        if (res == 0) {
            in    += BASE64_ENCODE_BLOCK_SIZE;
            inl   -= BASE64_ENCODE_BLOCK_SIZE;
            out   += outsz;
            *outl += (int)outsz;
        }
        else {
            *outl = 0;
            return  0;
        }
    }

    /* if remaining data exists, copy them into ctx for the next call*/
    if (inl > 0) {
        XMEMSET(ctx->data, 0, sizeof(ctx->data));
        XMEMCPY(ctx->data, in, (size_t)inl);
        ctx->remaining = inl;
    }

    return 1;   /* returns 1 on success, 0 on error */
}
/*  wolfSSL_EVP_EncodeFinal encodes data in ctx and outputs to out.
 */
void wolfSSL_EVP_EncodeFinal(WOLFSSL_EVP_ENCODE_CTX* ctx,
                unsigned char* out, int* outl)
{
    word32 outsz = 0;
    int res;

    WOLFSSL_ENTER("wolfSSL_EVP_EncodeFinal");

    if (outl == NULL)
        return;

    if (ctx == NULL || out == NULL) {
        *outl = 0;
        return;
    }
    if (ctx->remaining >= BASE64_ENCODE_RESULT_BLOCK_SIZE) {
        *outl = 0;
        return;
    }
    /* process remaining data in ctx */
    outsz = BASE64_ENCODE_RESULT_BLOCK_SIZE + 1; /* 64 byte and one for LF*/
    res = Base64_Encode(ctx->data, (word32)ctx->remaining, out, &outsz);
    if (res == 0)
        *outl = (int)outsz;
    else
        *outl = 0;

    ctx->remaining = 0;
    XMEMSET(ctx->data, 0, sizeof(ctx->data));

    return;
}
#endif /* WOLFSSL_BASE64_ENCODE */
#if defined(WOLFSSL_BASE64_DECODE)

/*  wolfSSL_EVP_DecodeInit initializes specified WOLFSSL_EVP_ENCODE_CTX struct
 *  for subsequent wolfSSL_EVP_DecodeUpdate.
 */
void wolfSSL_EVP_DecodeInit(WOLFSSL_EVP_ENCODE_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_EVP_DecodeInit");
    /* clean up ctx */
    if (ctx != NULL) {
        ctx->remaining = 0;
        XMEMSET(ctx->data, 0, sizeof(ctx->data));
    }
}
/*  wolfSSL_EVP_DecodeUpdate encodes the input data in 4-byte units
 *  and outputs it to out. If less than 4 bytes of data remain, save it in
 *  ctx. The data given in the subsequent wolfSSL_EVP_DecodeUpdate
 *  is combined with the data stored in CTX and used for decoding.
 *  Returns 1 or 0 on success, -1 on error. Return value 0 indicates that
 *  clients should call wolfSSL_EVP_DecodeFinal as next call.
 */
int  wolfSSL_EVP_DecodeUpdate(WOLFSSL_EVP_ENCODE_CTX* ctx,
            unsigned char* out, int* outl, const unsigned char* in, int inl)
{
    word32 outsz = 0;
    word32 j = 0;
    word32 inLen;
    int    res;
    int    pad = 0;
    int    i;
    unsigned char c;
    int pad3 = 0;
    int pad4 = 0;
    byte e[4];

    WOLFSSL_ENTER("wolfSSL_EVP_DecodeUpdate");

    if (outl == NULL)
        return -1;

    if (ctx == NULL || out == NULL || in == NULL) {
        *outl = 0;
        return -1;
    }

    if (inl == 0) {
        *outl = 0;
        return 1;
    }

    inLen = (word32)inl;
    *outl = 0;

    /* if the remaining data exist in the ctx, add input data to them to create
    a block(4bytes) for decoding*/
    if (ctx->remaining > 0 && inl > 0) {

        int cpySz = (int)min(
            (word32)(BASE64_DECODE_BLOCK_SIZE - ctx->remaining), (word32)inl);

        for ( i = 0; cpySz > 0 && inLen > 0; i++) {
            if (Base64_SkipNewline(in, &inLen, &j) == ASN_INPUT_E) {
                return -1;  /* detected an illegal char in input */
            }
            c = in[j++];

            if (c == '=')
                pad = 1;

            *(ctx->data + ctx->remaining + i) = c;
            inLen--;
            cpySz--;
        }

        outsz = sizeof(ctx->data);
        res = Base64_Decode( ctx->data, BASE64_DECODE_BLOCK_SIZE, out, &outsz);
        if (res == 0) {
            *outl += (int)outsz;
            out   += outsz;

            ctx->remaining = 0;
            XMEMSET(ctx->data, 0, sizeof(ctx->data));
        }
        else {
            *outl = 0;
            return -1;   /* return with error */
        }
    }

    /* Base64_Decode is not a streaming process, so it processes
     * the input data and exits. If a line break or whitespace
     * character is found in the input data, it will be skipped,
     * but if the end point of the input data is reached as a result,
     * Base64_Decode will stop processing there. The data cleansing is
     * required before Base64_Decode so that the processing does not
     * stop within 4 bytes, which is the unit of Base64 decoding processing.
     * The logic that exists before calling Base64_Decode in a While Loop is
     * a data cleansing process that removes line breaks and whitespace.
     */
    while (inLen > 3) {
        if ((res = Base64_SkipNewline(in, &inLen, &j)) != 0) {
            if (res == BUFFER_E) {
                break;
            }
            else {
                *outl = 0;
                return -1;
            }
        }
        e[0] = in[j++];
        if (e[0] == '\0') {
            break;
        }
        inLen--;
        if ((res = Base64_SkipNewline(in, &inLen, &j)) != 0) {
            if (res == BUFFER_E) {
                break;
            }
            else {
                *outl = 0;
                return -1;
            }
        }
        e[1] = in[j++];
        inLen--;
        if ((res = Base64_SkipNewline(in, &inLen, &j)) != 0) {
            if (res == BUFFER_E) {
                break;
            }
            else {
                *outl = 0;
                return -1;
            }
        }
        e[2] = in[j++];
        inLen--;
        if ((res = Base64_SkipNewline(in, &inLen, &j)) != 0) {
            if (res == BUFFER_E) {
                break;
            }
            else {
                *outl = 0;
                return -1;
            }
        }
        e[3] = in[j++];
        inLen--;

        if (e[0] == '=')
            pad = 1;
        if (e[1] == '=')
            pad = 1;
        if (e[2] == '=') {
            pad = 1;
            pad3 = 1;
        }
        if (e[3] == '=') {
            pad = 1;
            pad4 = 1;
        }
        if (pad3 && !pad4) {
            *outl = 0;
            return -1;
        }

        /* decode four bytes */
        outsz = sizeof(ctx->data);
        res = Base64_Decode( e, BASE64_DECODE_BLOCK_SIZE, out, &outsz);
        if (res < 0) {
            *outl = 0;
            return -1;
        }

        *outl += (int)outsz;
        out   += outsz;
    }
    /* copy left data to ctx */
    if (inLen > 0) {

        XMEMSET(ctx->data, 0, sizeof(ctx->data));

        i = 0;
        while (inLen > 0) {
            c = in[j++];
            if (c== '\n' || c == '\r' || c == ' ') {
                inLen--;
                continue;
            }
            if (c == '=') {
                pad = 1;
            }
            ctx->data[i++] = c;
            ctx->remaining++;
            inLen--;
        }

        if (pad)
            return 0;   /* indicates that clients should call DecodeFinal */
        else
            return 1;

    }
    /* if the last data is '\n', remove it */
    c = in[j - 1];
    if (c == '\n') {
        c = (in[j - 2]);
        if (c == '=')
            return 0;
        else
            return 1;
    }
    if (c == '=')
        return 0;
    else
        return 1;

}
/*  wolfSSL_EVP_DecodeFinal decode remaining data in ctx
 *  to outputs to out.
 *  Returns 1 on success, -1 on failure.
 */
int  wolfSSL_EVP_DecodeFinal(WOLFSSL_EVP_ENCODE_CTX* ctx,
                unsigned char* out, int* outl)
{
    word32 outsz = 0;
    word32 inLen;
    word32 j = 0;

    WOLFSSL_ENTER("wolfSSL_EVP_DecodeFinal");

    if (outl == NULL)
        return -1;

    if (ctx == NULL || out == NULL ) {
        *outl = 0;
        return -1;
    }

    if (ctx->remaining > 0) {
        int res;

        inLen = (word32)ctx->remaining;
        if ((res = Base64_SkipNewline(ctx->data, &inLen, &j)) != 0) {
            *outl = 0;
            if (res == BUFFER_E) /* means no valid data to decode in buffer */
                return  1; /* returns as success with no output */
            else
                return -1;
        }


        outsz = (word32)ctx->remaining;
        res = Base64_Decode(ctx->data, (word32)ctx->remaining, out, &outsz);
        if (res == 0) {
            *outl = (int)outsz;
            return 1;
        }
        else {
            *outl = 0;
            return -1;
        }
    }
    else {
        *outl = 0;
        return 1;
    }
}
#endif /* WOLFSSL_BASE64_DECODE */

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#endif /* WOLFSSL_EVP_INCLUDED */
