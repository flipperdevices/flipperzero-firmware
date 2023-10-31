/* devcrypto_ecdsa.c
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

#if defined(WOLFSSL_DEVCRYPTO_ECDSA)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>


static int CurveIDToFlag(int curveId)
{
    int ret = 0;

    switch (curveId) {
        case ECC_SECP256R1:
            ret = CAAM_EC_CURVE_P_256;
            break;
        case ECC_SECP384R1:
            ret = CAAM_EC_CURVE_P_384;
            break;
        default:
            WOLFSSL_MSG("Curve id not found");
    }
    return ret;
}

int wc_DevCryptoEccKeyGen(int curveId, int enc, byte* pri, word32 priSz,
    byte* pub, word32 pubSz)
{
    struct crypt_kop kop;
    int inIdx  = 0;
    int outIdx = 0;
    int ret    = 0;
    WC_CRYPTODEV ctx;
    int ecdsel;

    ecdsel = CurveIDToFlag(curveId);
    if (ecdsel == 0) {
        ret = NOT_COMPILED_IN;
    }

    if (ret == 0) {
        ret = wc_DevCryptoCreate(&ctx, CRYPTO_ASYM_ECC_KEYGEN, NULL, 0);
    }

    if (ret == 0) {
        kop.crk_op = CRK_ECC_KEYGEN;
        kop.ses    = ctx.sess.ses;
        kop.crk_flags = ecdsel;
        if (enc) {
            kop.crk_flags |= (CAAM_KEY_COLOR_BLACK << 8);
        }

        kop.crk_param[inIdx + outIdx].crp_p     = pri;
        kop.crk_param[inIdx + outIdx].crp_nbits = priSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p     = pub;
        kop.crk_param[inIdx + outIdx].crp_nbits = pubSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_iparams   = inIdx;
        kop.crk_oparams   = outIdx;

        if (ioctl(ctx.cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with ECC keygen operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
        wc_DevCryptoFree(&ctx);
    }
    return ret;
}


int wc_DevCryptoEccEcdh(int curveId, int enc, byte* pri, word32 priSz,
    byte* pub, word32 pubSz, byte* out, word32 outSz)
{
    struct crypt_kop kop;
    int inIdx  = 0;
    int outIdx = 0;
    int ret    = 0;
    int ecdsel;
    WC_CRYPTODEV ctx;

    ecdsel = CurveIDToFlag(curveId);
    if (ecdsel == 0) {
        ret = NOT_COMPILED_IN;
    }

    if (ret == 0) {
        ret = wc_DevCryptoCreate(&ctx, CRYPTO_ASYM_ECC_ECDH, NULL, 0);
    }

    if (ret == 0) {
        XMEMSET(&kop, 0, sizeof(kop));
        kop.crk_op = CRK_ECC_ECDH;
        kop.ses    = ctx.sess.ses;
        kop.crk_flags = CurveIDToFlag(curveId);
        if (enc) {
            kop.crk_flags |= (CAAM_KEY_COLOR_BLACK << 8);
        }

        kop.crk_param[inIdx].crp_p     = pub;
        kop.crk_param[inIdx].crp_nbits = pubSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx].crp_p     = pri;
        kop.crk_param[inIdx].crp_nbits = priSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx + outIdx].crp_p     = out;
        kop.crk_param[inIdx + outIdx].crp_nbits = outSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_iparams   = inIdx;
        kop.crk_oparams   = outIdx;

        if (ioctl(ctx.cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with ECC keygen operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
        wc_DevCryptoFree(&ctx);
    }
    return ret;
}


int wc_DevCryptoEccSign(int curveId, int enc, byte* pri, word32 priSz,
    const byte* hash, word32 hashSz, byte* r, word32 rSz, byte* s, word32 sSz)
{
    struct crypt_kop kop;
    int inIdx  = 0;
    int outIdx = 0;
    int ret    = 0;
    int ecdsel;
    WC_CRYPTODEV ctx;

    ecdsel = CurveIDToFlag(curveId);
    if (ecdsel == 0) {
        ret = NOT_COMPILED_IN;
    }

    if (ret == 0) {
        ret = wc_DevCryptoCreate(&ctx, CRYPTO_ASYM_ECDSA_SIGN, NULL, 0);
    }

    if (ret == 0) {
        XMEMSET(&kop, 0, sizeof(kop));
        kop.crk_op = CRK_ECDSA_SIGN;
        kop.ses    = ctx.sess.ses;
        kop.crk_flags = CurveIDToFlag(curveId);
        if (enc) {
            kop.crk_flags |= (CAAM_KEY_COLOR_BLACK << 8);
        }

        kop.crk_param[inIdx].crp_p     = pri;
        kop.crk_param[inIdx].crp_nbits = priSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx].crp_p     = (byte*)hash;
        kop.crk_param[inIdx].crp_nbits = hashSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx + outIdx].crp_p     = r;
        kop.crk_param[inIdx + outIdx].crp_nbits = rSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_param[inIdx + outIdx].crp_p     = s;
        kop.crk_param[inIdx + outIdx].crp_nbits = sSz * WOLFSSL_BIT_SIZE;
        outIdx++;

        kop.crk_iparams   = inIdx;
        kop.crk_oparams   = outIdx;
        if (ioctl(ctx.cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with ECC keygen operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
        wc_DevCryptoFree(&ctx);
    }
    return ret;
}

int wc_DevCryptoEccVerify(int curveId, byte* pub, word32 pubSz,
    const byte* hash, word32 hashSz, byte* r, word32 rSz, byte* s, word32 sSz)
{
    struct crypt_kop kop;
    int inIdx  = 0;
    int outIdx = 0;
    int ret    = 0;
    int ecdsel;
    WC_CRYPTODEV ctx;

    ecdsel = CurveIDToFlag(curveId);
    if (ecdsel == 0) {
        ret = NOT_COMPILED_IN;
    }

    if (ret == 0) {
        ret = wc_DevCryptoCreate(&ctx, CRYPTO_ASYM_ECDSA_VERIFY, NULL, 0);
    }
    if (ret == 0) {
        kop.crk_op = CRK_ECDSA_VERIFY;
        kop.ses    = ctx.sess.ses;
        kop.crk_flags = CurveIDToFlag(curveId);

        kop.crk_param[inIdx].crp_p     = pub;
        kop.crk_param[inIdx].crp_nbits = pubSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx].crp_p     = (byte*)hash;
        kop.crk_param[inIdx].crp_nbits = hashSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx].crp_p     = r;
        kop.crk_param[inIdx].crp_nbits = rSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_param[inIdx].crp_p     = s;
        kop.crk_param[inIdx].crp_nbits = sSz * WOLFSSL_BIT_SIZE;
        inIdx++;

        kop.crk_iparams   = inIdx;
        kop.crk_oparams   = outIdx;
        if (ioctl(ctx.cfd, CIOCKEY, &kop)) {
        #if defined(DEBUG_DEVCRYPTO)
            perror("Error value with ECC keygen operation was ");
        #endif
            WOLFSSL_MSG("Error with call to ioctl");
            ret = WC_DEVCRYPTO_E;
        }
        wc_DevCryptoFree(&ctx);
    }

    if (ret == 0) {
        if (kop.crk_status == 0) {
            ret = SIG_VERIFY_E;
        }
    }
    return ret;
}
#endif /* WOLFSSL_DEVCRYPTO_ECDSA */

