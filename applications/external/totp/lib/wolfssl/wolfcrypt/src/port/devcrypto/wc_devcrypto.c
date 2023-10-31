/* wc_devcrypto.c
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

#if defined(WOLFSSL_DEVCRYPTO)

static volatile int fd;

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

int wc_DevCryptoInit(void)
{
    /* create descriptor */
    if ((fd = open("/dev/crypto", O_RDWR, 0)) < 0) {
        WOLFSSL_MSG("Error opening /dev/crypto is cryptodev module loaded?");
        return WC_DEVCRYPTO_E;
    }

#if defined(CIOCASYMFEAT) && defined(WOLFSSL_DEVCRYPTO_RSA)
    {
        word32 asymAva = 0;

        if (ioctl(fd, CIOCASYMFEAT, &asymAva) == -1) {
            WOLFSSL_MSG("Error checking which asym. operations are available");
            close(fd);
            return WC_DEVCRYPTO_E;
        }

        if ((asymAva & CRF_RSA_PUBLIC) == 0) {
            WOLFSSL_MSG("CRK_RSA_PUBLIC is not available");
            close(fd);
            return WC_DEVCRYPTO_E;
        }
    }
#endif

    return 0;
}


void wc_DevCryptoCleanup(void)
{
    close(fd);
}


/* sets up a context for talking to /dev/crypto
 * return 0 on success */
int wc_DevCryptoCreate(WC_CRYPTODEV* ctx, int type, byte* key, word32 keySz)
{
#if defined(CIOCGSESSINFO) && defined(DEBUG_DEVCRYPTO)
    struct session_info_op sesInfo;
#endif

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check on session type before creating descriptor */
    XMEMSET(ctx, 0, sizeof(WC_CRYPTODEV));

    /* clone the master fd */
    if (ioctl(fd, CRIOGET, &ctx->cfd) != 0) {
        WOLFSSL_MSG("Error cloning fd");
        return WC_DEVCRYPTO_E;
    }

    if (fcntl(ctx->cfd, F_SETFD, 1) == -1) {
        WOLFSSL_MSG("Error setting F_SETFD with fcntl");
        (void)close(ctx->cfd);
        return WC_DEVCRYPTO_E;
    }

    /* set up session */
    switch (type) {
        case CRYPTO_SHA1:
        case CRYPTO_SHA2_256:
            ctx->sess.mac = type;
            break;

    #ifndef NO_AES
        case CRYPTO_AES_CTR:
        case CRYPTO_AES_ECB:
        case CRYPTO_AES_GCM:
        case CRYPTO_AES_CBC:
            ctx->sess.cipher = type;
            ctx->sess.key    = (void*)key;
            ctx->sess.keylen = keySz;
            break;
    #endif

        case CRYPTO_MD5_HMAC:
        case CRYPTO_SHA1_HMAC:
        case CRYPTO_SHA2_256_HMAC:
        case CRYPTO_SHA2_384_HMAC:
        case CRYPTO_SHA2_512_HMAC:
            ctx->sess.cipher = 0;
            ctx->sess.mac    = type;
            ctx->sess.mackey    = (byte*)key;
            ctx->sess.mackeylen = keySz;
            break;

    #if defined(WOLFSSL_DEVCRYPTO_ECDSA)
    #endif /* WOLFSSL_DEVCRYPTO_ECDSA */

    #if defined(WOLFSSL_DEVCRYPTO_RSA)
        case CRYPTO_ASYM_RSA_KEYGEN:
        case CRYPTO_ASYM_RSA_PRIVATE:
        case CRYPTO_ASYM_RSA_PUBLIC:
            ctx->sess.acipher = type;
            break;
    #endif

    #if defined(WOLFSSL_DEVCRYPTO_ECDSA)
        case CRYPTO_ASYM_ECDSA_SIGN:
        case CRYPTO_ASYM_ECDSA_VERIFY:
        case CRYPTO_ASYM_ECC_KEYGEN:
        case CRYPTO_ASYM_ECC_ECDH:
            ctx->sess.acipher = type;
            break;
    #endif

    #if defined(WOLFSSL_DEVCRYPTO_CURVE25519)
        case CRYPTO_ASYM_MUL_MOD:
            ctx->sess.acipher = type;
            break;
    #endif /* WOLFSSL_DEVCRYPTO_CURVE25519 */

        default:
            WOLFSSL_MSG("Unknown / Unimplemented algorithm type");
            (void)close(ctx->cfd);
            return BAD_FUNC_ARG;
    }


    if (ioctl(ctx->cfd, CIOCGSESSION, &ctx->sess)) {
    #if defined(DEBUG_DEVCRYPTO)
        perror("CIOGSESSION error ");
    #endif
        (void)close(ctx->cfd);
        WOLFSSL_MSG("Error starting cryptodev session");
        return WC_DEVCRYPTO_E;
    }

#if defined(CIOCGSESSINFO) && defined(DEBUG_DEVCRYPTO)
    sesInfo.ses = ctx->sess.ses;
    if (ioctl(ctx->cfd, CIOCGSESSINFO, &sesInfo)) {
        (void)close(ctx->cfd);
        WOLFSSL_MSG("Error getting session info");
        return WC_DEVCRYPTO_E;
    }
    printf("Using %s with driver %s\n", sesInfo.hash_info.cra_name,
        sesInfo.hash_info.cra_driver_name);
#endif
    (void)key;
    (void)keySz;

    return 0;
}


/* free up descriptor and session used with ctx */
void wc_DevCryptoFree(WC_CRYPTODEV* ctx)
{
    if (ctx != NULL && ctx->cfd >= 0) {
        if (ioctl(ctx->cfd, CIOCFSESSION, &ctx->sess.ses)) {
            WOLFSSL_MSG("Error stopping cryptodev session");
        }
        (void)close(ctx->cfd);
        ctx->cfd = -1;
    }
}


/* setup crypt_op structure */
void wc_SetupCrypt(struct crypt_op* crt, WC_CRYPTODEV* dev,
        byte* src, int srcSz, byte* dst, byte* dig, int flag, int op)

{
    XMEMSET(crt, 0, sizeof(struct crypt_op));
    crt->ses = dev->sess.ses;
    crt->src = src;
    crt->len = srcSz;
    crt->dst = dst;
    crt->op  = op;
    crt->mac = dig;
    crt->flags = flag;
}


/* setup crypt_op structure for symmetric key operations */
void wc_SetupCryptSym(struct crypt_op* crt, WC_CRYPTODEV* dev,
        byte* src, word32 srcSz, byte* dst, byte* iv, int flag)

{
    XMEMSET(crt, 0, sizeof(struct crypt_op));
    crt->ses    = dev->sess.ses;
    crt->src    = src;
    crt->len    = srcSz;
    crt->dst    = dst;
    crt->iv     = iv;
    crt->op     = flag;
}


/* setup crypt_auth_op structure for aead operations */
void wc_SetupCryptAead(struct crypt_auth_op* crt, WC_CRYPTODEV* dev,
         byte* src, word32 srcSz, byte* dst, byte* iv, word32 ivSz, int flag,
         byte* authIn, word32 authInSz, byte* authTag, word32 authTagSz)
{
    XMEMSET(crt, 0, sizeof(struct crypt_op));
    crt->ses    = dev->sess.ses;
    crt->src    = src;
    crt->len    = srcSz;
    crt->dst    = dst;
    crt->iv     = iv;
    crt->iv_len = ivSz;
    crt->op     = flag;

    /* also set auth in and tag */
    crt->auth_src = authIn;
    crt->auth_len = authInSz;
    crt->tag = authTag;
    crt->tag_len = authTagSz;
}

#endif /* WOLFSSL_DEVCRYPTO */

