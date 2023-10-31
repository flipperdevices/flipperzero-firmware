/* devcrypto_rsa.c
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

#if defined(WOLFSSL_DEVCRYPTO_RSA)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>

static void wc_SetupRsaPublic(struct crypt_kop* kop, WC_CRYPTODEV* dev,
        const byte* in, word32 inSz, byte* m, word32 mSz, byte* e, word32 eSz,
        byte* out, word32 outSz)
{
    int inIdx  = 0;
    int outIdx = 0;

    kop->crk_op   = CRK_RSA_PUBLIC;
    kop->crk_pad1 = mSz;
    kop->ses = dev->sess.ses;

    kop->crk_param[inIdx].crp_p     = (byte*)in;
    kop->crk_param[inIdx].crp_nbits = inSz * WOLFSSL_BIT_SIZE;
    inIdx++;

    kop->crk_param[inIdx].crp_p     = m;
    kop->crk_param[inIdx].crp_nbits = mSz * WOLFSSL_BIT_SIZE;
    inIdx++;

    kop->crk_param[inIdx].crp_p     = e;
    kop->crk_param[inIdx].crp_nbits = eSz * WOLFSSL_BIT_SIZE;
    inIdx++;

    /* add output buffer */
    kop->crk_param[inIdx + outIdx].crp_p     = out;
    kop->crk_param[inIdx + outIdx].crp_nbits = outSz * WOLFSSL_BIT_SIZE;
    outIdx++;

    kop->crk_iparams   = inIdx;
    kop->crk_oparams   = outIdx;
#ifdef DEBUG_DEVCRYPTO
    printf("SetupRsaPublic:\n");
    printf("\tinSz = %u\n", inSz);
    printf("\tmSz  = %u\n", mSz);
    printf("\teSz  = %u\n", eSz);
    printf("\toutSz= %u\n", outSz);
    printf("\tiparams = %d\n", inIdx);
    printf("\toparams = %d\n", outIdx);
#endif
    (void)dev;
}


static void wc_SetupRsaPrivate(struct crypt_kop* kop, WC_CRYPTODEV* dev,
        const byte* in, word32 inSz, byte* d, word32 dSz, byte* n, word32 nSz,
        byte* p, word32 pSz, byte* q, word32 qSz, byte* dp, word32 dpSz,
        byte* dq, word32 dqSz, byte* u,  word32 uSz, byte* out, word32 outSz,
        int flag)
{
    int inIdx  = 0;
    int outIdx = 0;

    XMEMSET(kop, 0, sizeof(struct crypt_kop));
    kop->ses      = dev->sess.ses;
    kop->crk_op   = CRK_RSA_PRIVATE;
    kop->crk_flags= flag;

    kop->crk_param[inIdx].crp_p     = (byte*)in;
    kop->crk_param[inIdx].crp_nbits = inSz * WOLFSSL_BIT_SIZE;
    inIdx++;

    if (dpSz == 0 || dqSz == 0) {
        kop->crk_param[inIdx].crp_p     = n;
        kop->crk_param[inIdx].crp_nbits = dSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop->crk_param[inIdx].crp_p     = d;
        kop->crk_param[inIdx].crp_nbits = nSz * WOLFSSL_BIT_SIZE;
        inIdx++;
    }
    else {
        kop->crk_param[inIdx].crp_p     = p;
        kop->crk_param[inIdx].crp_nbits = pSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop->crk_param[inIdx].crp_p     = q;
        kop->crk_param[inIdx].crp_nbits = qSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop->crk_param[inIdx].crp_p     = dp;
        kop->crk_param[inIdx].crp_nbits = dpSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop->crk_param[inIdx].crp_p     = dq;
        kop->crk_param[inIdx].crp_nbits = dqSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop->crk_param[inIdx].crp_p     = u;
        kop->crk_param[inIdx].crp_nbits = uSz * WOLFSSL_BIT_SIZE;
        inIdx++;
    }

    /* add output buffer */
    kop->crk_param[inIdx + outIdx].crp_p     = out;
    kop->crk_param[inIdx + outIdx].crp_nbits = outSz * WOLFSSL_BIT_SIZE;
    outIdx++;

    kop->crk_iparams   = inIdx;
    kop->crk_oparams   = outIdx;
#ifdef DEBUG_DEVCRYPTO
    printf("SetupModExpCrt:\n");
    printf("\tinSz = %u\n", inSz);
    printf("\tdSz  = %u\n", dSz);
    printf("\tpSz  = %u\n", pSz);
    printf("\tqSz  = %u\n", qSz);
    printf("\tdpSz = %u\n", dpSz);
    printf("\tdqSz = %u\n", dqSz);
    printf("\tu    = %u\n", uSz);
    printf("\toutSz= %u\n", outSz);
    printf("\tiparams = %d\n", kop->crk_iparams);
    printf("\toparams = %d\n", kop->crk_oparams);
#endif
    (void)dev;
}

static int _PrivateOperation(const byte* in, word32 inlen, byte* out,
    word32 outlen, RsaKey* key)
{
    int ret = 0;
    WC_CRYPTODEV*    dev;
    struct crypt_kop kop;
    int flag   = 0;
    byte* d    = NULL;
    byte* p    = NULL;
    byte* q    = NULL;
    byte* dq   = NULL;
    byte* dp   = NULL;
    byte* u    = NULL;
    byte* n    = NULL;
    word32 dSz, pSz, qSz, dpSz = 0, dqSz = 0, uSz = 0, nSz;

    dev = &key->ctx;
    dSz = nSz = wc_RsaEncryptSize(key);
    pSz = qSz = nSz / 2;
    if (outlen < dSz) {
        WOLFSSL_MSG("Output buffer is too small");
        return BAD_FUNC_ARG;
    }
    outlen = dSz;

    if (wc_DevCryptoCreate(dev, CRYPTO_ASYM_RSA_PRIVATE, NULL, 0) != 0) {
        WOLFSSL_MSG("Error getting RSA private session");
        return WC_DEVCRYPTO_E;
    }
    d = (byte*)XMALLOC(dSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    p = (byte*)XMALLOC(pSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    q = (byte*)XMALLOC(qSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    n = (byte*)XMALLOC(dSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (d == NULL || p == NULL || q == NULL) {
        ret = MEMORY_E;
    }

    if (ret == 0) {
        byte e[8];
        word32 eSz = 8;
        ret = wc_RsaExportKey(key, e, &eSz, n, &nSz, d, &dSz, p, &pSz, q, &qSz);
        if (ret != 0) {
            WOLFSSL_MSG("Error with key export");
        }
        if (!key->blackKey) { /* @TODO unexpected results with black key CRT form */
            dpSz = mp_unsigned_bin_size(&key->dP);
            dqSz = mp_unsigned_bin_size(&key->dQ);
            uSz  = mp_unsigned_bin_size(&key->u);
        }
    }

    /* get values for CRT if present */
    if (!key->blackKey) { /* @TODO unexpected results with black key CRT form */
        if (ret == 0 && dpSz > 0) {
            dSz = 0; nSz = 0;
            dq = (byte*)XMALLOC(dpSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            dp = (byte*)XMALLOC(dpSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            u  = (byte*)XMALLOC(uSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (dq == NULL || dp == NULL || u == NULL) {
                ret = MEMORY_E;
            }
        }
        if (ret == 0 && dq != NULL &&
            mp_to_unsigned_bin(&key->dQ, dq) != MP_OKAY) {
            ret = MP_READ_E;
        }

        if (ret == 0 && dp != NULL &&
            mp_to_unsigned_bin(&key->dP, dp) != MP_OKAY) {
            ret = MP_READ_E;
        }

        if (ret == 0 && u != NULL &&
            mp_to_unsigned_bin(&key->u, u) != MP_OKAY) {
            ret = MP_READ_E;
        }
    }

    if (ret == 0) {
        if (key->blackKey) {
            flag = (CAAM_KEY_COLOR_BLACK << 8);
        }
        wc_SetupRsaPrivate(&kop, dev, in, inlen, d, dSz, n, nSz, p, pSz, q, qSz,
            dp, dpSz, dq, dqSz, u, uSz, out, outlen, flag);
    }

    if (ret == 0) {
        if (ioctl(dev->cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with private RSA operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
    }

    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (p != NULL)
        XFREE(p, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (q != NULL)
        XFREE(q, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (dp != NULL)
        XFREE(dp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (dq != NULL)
        XFREE(dq, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (u != NULL)
        XFREE(u,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (n != NULL)
        XFREE(n,  NULL, DYNAMIC_TYPE_TMP_BUFFER);

    wc_DevCryptoFree(dev);
    return ret;
}


static int _PublicOperation(const byte* in, word32 inlen, byte* out,
    word32 outlen, RsaKey* key)
{
    int ret = 0;
    WC_CRYPTODEV*    dev;
    struct crypt_kop kop;
    byte* m    = NULL;
    byte* e    = NULL;
    word32 mSz = 0;
    word32 eSz = 0;

    dev = &key->ctx;

    key->ctx.cfd = -1;
    if (wc_DevCryptoCreate(dev, CRYPTO_ASYM_RSA_PUBLIC, NULL, 0) != 0) {
        WOLFSSL_MSG("Error getting RSA public session");
        return WC_DEVCRYPTO_E;
    }

    mSz = wc_RsaEncryptSize(key);
    eSz = mp_unsigned_bin_size(&key->e);

    m = (byte*)XMALLOC(mSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    e = (byte*)XMALLOC(eSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (m == NULL || e == NULL) {
        ret = MEMORY_E;
    }

    if (ret == 0) {
        ret = wc_RsaFlattenPublicKey(key, e, &eSz, m, &mSz);
        if (ret != 0) {
            WOLFSSL_MSG("Issue with getting RSA public key parts");
        }
    }

    if (ret == 0) {
        wc_SetupRsaPublic(&kop, dev, in, inlen, m, mSz, e, eSz, out, outlen);
        if (ioctl(dev->cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with public RSA operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
    }
    wc_DevCryptoFree(&key->ctx);

    if (m != NULL)
        XFREE(m, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (e != NULL)
        XFREE(e, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}


int wc_DevCrypto_RsaDecrypt(const byte* in, word32 inlen,
        byte* out, word32 outlen, RsaKey* key, int type)
{
    int ret = BAD_FUNC_ARG;

    switch (type) {
        case RSA_PUBLIC_DECRYPT:
            ret = _PublicOperation(in, inlen, out, outlen, key);
            break;

        case RSA_PRIVATE_DECRYPT:
            ret = _PrivateOperation(in, inlen, out, outlen, key);
            break;
    }

    return ret;
}


int wc_DevCrypto_RsaEncrypt(const byte* in, word32 inlen, byte* out,
        word32* outlen, RsaKey *key, int type)
{
    int ret = BAD_FUNC_ARG;

    switch (type) {
        case RSA_PUBLIC_ENCRYPT:
            ret = _PublicOperation(in, inlen, out, *outlen, key);
            break;

        case RSA_PRIVATE_ENCRYPT:
            ret = _PrivateOperation(in, inlen, out, *outlen, key);
            break;
    }
    if (ret == 0) {
        *outlen = inlen;
    }

    return ret;
}


#ifdef WOLFSSL_KEY_GEN
/* generate p and q prive values for RSA key generation
 * nSz is size of key in bits
 */
static int GeneratePandQ(byte* p, int pSz, byte* q, int qSz, byte* e, int eSz,
    int nSz, WC_RNG* rng)
{
    int isPrime = 0;
    int i = 0;
    int failCount;
    int err;

    /* The failCount value comes from NIST FIPS 186-4, section B.3.3,
     * process steps 4.7 and 5.8. */
    failCount = 5 * ((nSz * WOLFSSL_BIT_SIZE)/ 2);

    /* make p */
    do {
#ifdef SHOW_GEN
        printf(".");
        fflush(stdout);
#endif
        /* generate value */
        err = wc_RNG_GenerateBlock(rng, p, pSz);
        if (err == 0) {
            /* prime lower bound has the MSB set, set it in candidate */
            p[0] |= 0x80;
            /* make candidate odd */
            p[pSz-1] |= 0x01;
        }

        if (err == MP_OKAY)
            err = wc_CheckProbablePrime_ex(p, pSz, NULL, 0, e, eSz, nSz, &isPrime, rng);

        i++;
    } while (err == MP_OKAY && !isPrime && i < failCount);

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    /* make q */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, q, qSz);
            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                q[0] |= 0x80;
                /* make candidate odd */
                q[qSz-1] |= 0x01;
            }

            if (err == MP_OKAY)
                err = wc_CheckProbablePrime_ex(p, pSz, q, qSz, e, eSz, nSz, &isPrime, rng);

            i++;
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    return err;
}

int wc_DevCrypto_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
    int ret = 0;
    struct crypt_kop kop;
    int pSz, qSz, nSz, dSz, dpSz, dqSz, cSz;
    byte *p = NULL;
    byte *q = NULL;
    byte *n = NULL;
    byte *d = NULL;
    byte *dp = NULL;
    byte *dq = NULL;
    byte *c = NULL;
    int bSz = (size + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;
    int inIdx = 0, outIdx = 0;
    byte eBuf[8];
    int  eBufSz;

    key->ctx.cfd = -1;
    nSz = dSz = bSz;
    cSz = pSz = qSz = dpSz = dqSz = bSz/2;

    ret = wc_DevCryptoCreate(&key->ctx, CRYPTO_ASYM_RSA_KEYGEN, NULL, 0);
    if (ret == 0) {
        p  = (byte*)XMALLOC(pSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        q  = (byte*)XMALLOC(qSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        dp = (byte*)XMALLOC(dpSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        dq = (byte*)XMALLOC(dqSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        c  = (byte*)XMALLOC(cSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        n  = (byte*)XMALLOC(nSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        d  = (byte*)XMALLOC(dSz, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (p == NULL || q == NULL || dp == NULL || dq == NULL ||
            n == NULL || d == NULL || c == NULL) {
            ret = MEMORY_E;
        }
    }

    if (ret == 0) {
        mp_init(&key->e);
        mp_set_int(&key->e, (int)e);
        eBufSz = mp_unsigned_bin_size(&key->e);
        mp_to_unsigned_bin(&key->e, eBuf);
    }

    if (ret == 0 &&
            GeneratePandQ(p, pSz, q, qSz, eBuf, eBufSz, size, rng) != MP_OKAY) {
        WOLFSSL_MSG("Issue generating primes");
        ret = PRIME_GEN_E;
    }

    if (ret == 0) {
        XMEMSET(&kop, 0, sizeof(struct crypt_kop));
        kop.crk_op = CRK_RSA_KEYGEN;
        kop.crk_pad1 = size;

        kop.crk_param[inIdx].crp_p = eBuf;
        kop.crk_param[inIdx].crp_nbits = eBufSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = p;
        kop.crk_param[inIdx + outIdx].crp_nbits = pSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = q;
        kop.crk_param[inIdx + outIdx].crp_nbits = qSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = n;
        kop.crk_param[inIdx + outIdx].crp_nbits = nSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = d;
        kop.crk_param[inIdx + outIdx].crp_nbits = dSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = dp;
        kop.crk_param[inIdx + outIdx].crp_nbits = dpSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = dq;
        kop.crk_param[inIdx + outIdx].crp_nbits = dqSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p = c;
        kop.crk_param[inIdx + outIdx].crp_nbits = cSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.ses = key->ctx.sess.ses;
        kop.crk_flags |= (CAAM_KEY_COLOR_BLACK << 8);
        kop.crk_iparams = inIdx;
        kop.crk_oparams = outIdx;

        if (ioctl(key->ctx.cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with RSA keygen operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
    }
    wc_DevCryptoFree(&key->ctx);

    if (ret == 0) {
        key->type = RSA_PRIVATE;
        if (kop.crk_flags & (CAAM_KEY_COLOR_BLACK << 8)) {
            key->blackKey = 1;
        }
        mp_read_unsigned_bin(&key->n, n, nSz);
    #ifndef WOLFSSL_RSA_PUBLIC_ONLY
        mp_read_unsigned_bin(&key->p, p, pSz);
        mp_read_unsigned_bin(&key->q, q, qSz);
        mp_read_unsigned_bin(&key->d, d, dSz);
    #if defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM)
        mp_read_unsigned_bin(&key->dP, dp, dpSz);
        mp_read_unsigned_bin(&key->dQ, dq, dqSz);
        mp_read_unsigned_bin(&key->u,  c,  cSz);
    #endif
    #endif
    }

    if (p != NULL)
        XFREE(p, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (q != NULL)
        XFREE(q, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (dp != NULL)
        XFREE(dp, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (dq != NULL)
        XFREE(dq, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (c != NULL)
        XFREE(c, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (n != NULL)
        XFREE(n, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (d != NULL) {
        XFREE(d, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    (void)rng;
    return ret;
}
#endif /* WOLFSSL_KEY_GEN */

void wc_DevCrypto_RsaFree(RsaKey* rsa)
{
    wc_DevCryptoFree(&rsa->ctx);
}

#endif /* WOLFSSL_DEVCRYPTO */

