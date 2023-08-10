/* aria-cryptocb.c
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

DESCRIPTION
This library provides the interfaces to the ARIA cipher, an encryption algorithm
developed by the Korean Agency for Technology (KATS). It uses a 128-bit block
size and a key size of 128, 192, or 256 bits.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ARIA

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/port/aria/aria-cryptocb.h>

int wc_AriaInit(void)
{
    MC_RV rv = MC_OK;
    static char isInit = 0;

    if (isInit == 0) {
        if (rv == MC_OK)
            rv = MC_Initialize(NULL);
        if (rv == MC_OK)
            isInit = 1;

        #ifdef WOLF_CRYPTO_CB
        if (rv == MC_OK) {
            rv = wc_CryptoDev_RegisterDevice(WOLFSSL_ARIA_DEVID, wc_AriaCryptoCb, NULL);
        }
        #endif
    }

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return WC_INIT_E;
    }
    return 0;
}

/* return 0 on success or WC_INIT_E on failure */
int wc_AriaInitSha(MC_HSESSION* hSession, MC_ALGID algo)
{
    MC_RV rv = MC_OK;

    MC_APIMODE gApimode = MC_MODE_KCMV;
    MC_ALGORITHM mcAlg = {algo, NULL, 0};

    WOLFSSL_MSG_EX("AriaInitSha(0x%X)",algo);

    if (hSession == NULL) {
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = wc_AriaInit();

    if (rv == MC_OK)
        rv = MC_OpenSession(hSession);

    if (rv == MC_OK)
        rv = MC_SetApiMode(*hSession, gApimode);

    if (rv == MC_OK)
        rv = MC_DigestInit(*hSession, &mcAlg);

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return WC_INIT_E;
    }
    return 0;
}

/* return 0 on success or BAD_FUNC_ARG on failure */
int wc_AriaShaUpdate(MC_HSESSION hSession, byte* data, word32 len)
{
    MC_RV rv = MC_OK;
    WOLFSSL_ENTER("AriaShaUpdate");

    if (data == NULL) {
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = MC_DigestUpdate(hSession, data, len);

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_FUNC_ARG;
    }
    return 0;
}

/* return 0 on success or BAD_FUNC_ARG on failure */
int wc_AriaShaFinal(MC_HSESSION hSession, byte* out, word32* len)
{
    MC_RV rv = MC_OK;
    WOLFSSL_ENTER("AriaShaFinal");

    if (out == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Do an extra DigestUpdate noop just in case it is never explicitly called. */
    if (rv == MC_OK)
        rv = MC_DigestUpdate(hSession, NULL, 0);

    if (rv == MC_OK)
        rv = MC_DigestFinal(hSession, out, len);

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_FUNC_ARG;
    }
    /* WOLFSSL_MSG_EX("Digest len: %d", *len); */
    return 0;
}

/* return 0 on success or BAD_STATE_E on failure */
int wc_AriaFree(MC_HSESSION* hSession, MC_HOBJECT *obj1)
{
    MC_RV rv = MC_OK;
    WOLFSSL_ENTER("AriaFree");

    if (hSession == NULL && obj1 != NULL) {
        return BAD_FUNC_ARG;
    }

    if (obj1 != NULL) {
        if (rv == MC_OK)
            rv = MC_DestroyObject(*hSession, *obj1);
        if (rv == MC_OK)
            *obj1 = NULL;
    }

    if (hSession != NULL) {
        if (rv == MC_OK)
            rv = MC_CloseSession(*hSession);
        if (rv == MC_OK)
            *hSession = NULL;
    }

    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_STATE_E;
    }
    return 0;
}

int wc_AriaSign(byte* in, word32 inSz, byte* out, word32* outSz, ecc_key* key)
{
    MC_HOBJECT hPrikey = 0;
    MC_HSESSION hSession = 0;

    const ecc_set_type* dp;
    MC_RV rv = MC_OK;

    MC_APIMODE gApimode = MC_MODE_KCMV;
    MC_ALGORITHM mcAlg = {MC_ALGID_NONE, NULL, 0};
    byte keyAsn1[ARIA_KEYASN1_MAXSZ];
    word32 keyAsn1Sz=(word32)sizeof(keyAsn1);

    WOLFSSL_ENTER("AriaSign");

    if (in == NULL || out == NULL || outSz == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = wc_AriaInit();

    if (rv == MC_OK)
        rv = MC_OpenSession(&hSession);

    if (rv == MC_OK)
        rv = MC_SetApiMode(hSession, gApimode);

    if (rv == MC_OK) {
        int ret = wc_BuildEccKeyDer(key,keyAsn1,&keyAsn1Sz,0,0);
        if (ret < 0) { rv = ret; }
        else { keyAsn1Sz = ret; }
    }

    WOLFSSL_MSG_EX("AriaSign key(%d):",keyAsn1Sz);
    WOLFSSL_BUFFER(keyAsn1,keyAsn1Sz);

    WOLFSSL_MSG_EX("AriaSign rv=%d",rv);

    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    if (dp->id == ECC_SECP256R1) {
        mcAlg.mcAlgId = MC_ALGID_SHA256WithECDSA_P256_r1;
    } else if (dp->id == ECC_SECP224R1) {
        mcAlg.mcAlgId = MC_ALGID_SHA256WithECDSA_P224_12;
    } else {
        rv = MC_ERR_UNSUPPORTED_ALGORITHM;
    }

    if (rv == MC_OK)
        rv = MC_CreateObject(hSession, keyAsn1, keyAsn1Sz, &hPrikey);
    WOLFSSL_MSG_EX("AriaSign CreateObject rv=%d",rv);

    if (rv == MC_OK)
        rv = MC_SignInit(hSession, &mcAlg, hPrikey);
    WOLFSSL_MSG_EX("AriaSign SignInit rv=%d",rv);

    if (rv == MC_OK)
        rv = MC_Sign(hSession, in, inSz, out, outSz);
    WOLFSSL_MSG_EX("AriaSign Sign rv=%d",rv);

    wc_AriaFree(&hSession, &hPrikey);
    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_STATE_E;
    }
    return 0;
}

int wc_AriaVerify(byte* sig, word32 sigSz, byte* hash, word32 hashSz,
        int* res, ecc_key* key)
{
    MC_HOBJECT hPubkey = 0;
    MC_HSESSION hSession = 0;

    const ecc_set_type* dp;
    MC_RV rv = MC_OK;

    MC_APIMODE gApimode = MC_MODE_KCMV;
    MC_ALGORITHM mcAlg = {MC_ALGID_NONE, NULL, 0};
    byte keyarr[ARIA_KEYASN1_MAXSZ];
    word32 keySz=sizeof(keyarr);

    WOLFSSL_ENTER("AriaVerify");

    if (sig == NULL || hash == NULL || res == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    *res = 0; /* Default to invalid signature */

    if (rv == MC_OK)
        rv = wc_AriaInit();

    if (rv == MC_OK)
        rv = MC_OpenSession(&hSession);

    if (rv == MC_OK)
        rv = MC_SetApiMode(hSession, gApimode);

    if (rv == MC_OK) {
        int ret = wc_EccPublicKeyToDer(key,keyarr,keySz,0);
        if (ret < 0) { rv = ret; }
        else { keySz = ret; }
    }

    WOLFSSL_MSG_EX("AriaVerify key(%d):",keySz);
    WOLFSSL_BUFFER(keyarr,keySz);

    WOLFSSL_MSG_EX("AriaVerify rv=%d",rv);

    if (key->dp != NULL) {
        dp = key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(key->idx);
    }

    if (dp->id == ECC_SECP256R1) {
        mcAlg.mcAlgId = MC_ALGID_SHA256WithECDSA_P256_r1;
    } else if (dp->id == ECC_SECP224R1) {
        mcAlg.mcAlgId = MC_ALGID_SHA256WithECDSA_P224_12;
    } else {
        rv = MC_ERR_UNSUPPORTED_ALGORITHM;
    }

    if (rv == MC_OK)
        rv = MC_CreateObject(hSession, keyarr, keySz, &hPubkey);
    WOLFSSL_MSG_EX("AriaVerify CreateObject rv=%d",rv);

    if (rv == MC_OK)
        rv = MC_VerifyInit(hSession, &mcAlg, hPubkey);
    WOLFSSL_MSG_EX("AriaVerify VerifyInit rv=%d",rv);

    if (rv == MC_OK)
        rv = MC_Verify(hSession, hash, hashSz, sig, sigSz);
    WOLFSSL_MSG_EX("AriaVerify Verify rv=%d",rv);

    wc_AriaFree(&hSession, &hPubkey);
    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_STATE_E;
    }
    *res = 1; /* Valid signature */
    return 0;
}

int wc_AriaDerive(ecc_key* private_key, ecc_key* public_key,
        byte* out, word32* outSz)
{
    MC_HOBJECT hPrikey = 0;
    MC_HSESSION hSession = 0;

    const ecc_set_type* dp;
    MC_RV rv = MC_OK;

    MC_APIMODE gApimode = MC_MODE_KCMV;
    MC_ALGORITHM mcAlg = {MC_ALGID_NONE, NULL, 0};
    byte pubAsn1[ARIA_KEYASN1_MAXSZ];
    word32 pubAsn1Sz=sizeof(pubAsn1);
    byte privAsn1[ARIA_KEYASN1_MAXSZ];
    word32 privAsn1Sz=sizeof(privAsn1);

    WOLFSSL_ENTER("AriaDerive");

    if (private_key == NULL || public_key == NULL ||
            out == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    if (rv == MC_OK)
        rv = wc_AriaInit();

    if (rv == MC_OK)
        rv = MC_OpenSession(&hSession);

    if (rv == MC_OK)
        rv = MC_SetApiMode(hSession, gApimode);

    if (rv == MC_OK) {
        int ret = wc_EccPublicKeyToDer(public_key,pubAsn1,pubAsn1Sz,0);
        if (ret < 0) {
            rv = ret;
        } else {
            pubAsn1Sz = ret;
        }
        WOLFSSL_MSG_EX("AriaDerive PublicKeyToDer ret=%d",ret);
    }
    WOLFSSL_MSG_EX("AriaVerify pubAsn1(%d):",pubAsn1Sz);
    WOLFSSL_BUFFER(pubAsn1,pubAsn1Sz);
    mcAlg.pParam=pubAsn1;
    mcAlg.nParam=pubAsn1Sz;

    if (rv == MC_OK) {
        int ret = wc_BuildEccKeyDer(private_key,privAsn1,&privAsn1Sz,0,0);
        if (ret < 0) {
            rv = ret;
        } else {
            privAsn1Sz = ret;
        }
        WOLFSSL_MSG_EX("AriaDerive PrivateKeyToDer ret=%d",ret);
    }
    WOLFSSL_MSG_EX("AriaVerify privAsn1(%d):",privAsn1Sz);
    WOLFSSL_BUFFER(privAsn1,privAsn1Sz);

    if (private_key->dp != NULL) {
        dp = private_key->dp;
    }
    else {
        dp = wc_ecc_get_curve_params(private_key->idx);
    }

    if (dp->id == ECC_SECP256R1) {
        mcAlg.mcAlgId = MC_ALGID_ECDH_P256_r1;
    } else if (dp->id == ECC_SECP224R1) {
        mcAlg.mcAlgId = MC_ALGID_ECDH_P224_12;
    } else {
        rv = MC_ERR_UNSUPPORTED_ALGORITHM;
    }

    if (rv == MC_OK)
        rv = MC_CreateObject(hSession, privAsn1, privAsn1Sz, &hPrikey);
    WOLFSSL_MSG_EX("AriaDerive CreateObject rv=%d",rv);

    if (rv == MC_OK)
        rv = MC_DeriveKey(hSession, &mcAlg, hPrikey, out, outSz);
    WOLFSSL_MSG_EX("AriaDerive DeriveKey rv=%d",rv);

    wc_AriaFree(&hSession, &hPrikey);
    if (rv != MC_OK) {
        WOLFSSL_MSG(MC_GetErrorString(rv));
        return BAD_STATE_E;
    }
    return 0;
}

#ifdef WOLF_CRYPTO_CB
    static void printOutput(const char* strName, unsigned char* data,
            unsigned int dataSz)
    {
        #ifdef DEBUG_WOLFSSL
        WOLFSSL_MSG_EX("%s (%d):", strName,dataSz);
        WOLFSSL_BUFFER(data,dataSz);
        #else
        #if 0
            unsigned int i;
            int line = 1;

            printf("%s:\n",strName);
            printf("    ");
            for(i=1; i<=dataSz; i++)
            {
                printf(",0x%02X", data[i-1]);
                if(!(i%16) && i!= dataSz) printf("\n    ");
                else if(!(i%4)) printf(" ");
            }
            printf("\n");
        #else
            (void)strName;
            (void)data;
            (void)dataSz;
        #endif
        #endif
    }

    int wc_AriaCryptoCb(int devIdArg, wc_CryptoInfo* info, void* ctx)
    {
        int ret = CRYPTOCB_UNAVAILABLE; /* return this to bypass HW and use SW */
        (void)ctx;

        if (info == NULL)
            return BAD_FUNC_ARG;

    #ifdef DEBUG_CRYPTOCB
        wc_CryptoCb_InfoString(info);
    #endif

        if (info->algo_type == WC_ALGO_TYPE_PK) {
            if (info->pk.type == WC_PK_TYPE_ECDSA_SIGN) {
                /* set devId to invalid, so software is used */
                info->pk.eccsign.key->devId = INVALID_DEVID;

                printOutput((char *)"eccsign.in (before)",
                            (byte *)info->pk.eccsign.in,info->pk.eccsign.inlen);
                printOutput((char *)"eccsign.out(before)",
                            (byte *)info->pk.eccsign.out,*(info->pk.eccsign.outlen));
                printOutput((char *)"eccsign.key(before)",
                            (byte *)info->pk.eccsign.key,sizeof(info->pk.eccsign.key));

                byte buf[ARIA_KEYASN1_MAXSZ];
                word32 bufSz = sizeof(buf);
                ret = wc_AriaSign((byte *)info->pk.eccsign.in,info->pk.eccsign.inlen,
                            buf,&bufSz,
                            info->pk.eccsign.key);
                if (ret != 0) {
                    ret = CRYPTOCB_UNAVAILABLE;
                } else {
                    memcpy(info->pk.eccsign.out, buf, bufSz);
                    *(info->pk.eccsign.outlen) = bufSz;
                }

                printOutput((char *)"eccsign.in (after)",
                            (byte *)info->pk.eccsign.in,info->pk.eccsign.inlen);
                printOutput((char *)"eccsign.out(after)",
                            (byte *)info->pk.eccsign.out,*(info->pk.eccsign.outlen));
                printOutput((char *)"eccsign.key(after)",
                            (byte *)info->pk.eccsign.key,sizeof(info->pk.eccsign.key));

                /* reset devId */
                info->pk.eccsign.key->devId = devIdArg;
            }
            else if (info->pk.type == WC_PK_TYPE_ECDSA_VERIFY) {
                /* set devId to invalid, so software is used */
                info->pk.eccverify.key->devId = INVALID_DEVID;

                printOutput((char *)"eccverify.sig (before)",
                            (byte *)info->pk.eccverify.sig,info->pk.eccverify.siglen);
                printOutput((char *)"eccverify.hash(before)",
                            (byte *)info->pk.eccverify.hash,info->pk.eccverify.hashlen);
                printOutput((char *)"eccverify.key (before)",
                            (byte *)info->pk.eccverify.key,sizeof(info->pk.eccverify.key));

                ret = wc_AriaVerify((byte *)info->pk.eccverify.sig,info->pk.eccverify.siglen,
                                (byte *)info->pk.eccverify.hash, info->pk.eccverify.hashlen,
                                info->pk.eccverify.res, info->pk.eccverify.key);

                printOutput((char *)"eccverify.sig (after)",
                            (byte *)info->pk.eccverify.sig,info->pk.eccverify.siglen);
                printOutput((char *)"eccverify.hash(after)",
                            (byte *)info->pk.eccverify.hash,info->pk.eccverify.hashlen);
                printOutput((char *)"eccverify.key (after)",
                            (byte *)info->pk.eccverify.key,sizeof(info->pk.eccverify.key));

                if (ret != 0)
                    ret = CRYPTOCB_UNAVAILABLE;
                /* reset devId */
                info->pk.eccverify.key->devId = devIdArg;
            }
            else if (info->pk.type == WC_PK_TYPE_ECDH) {
                /* set devId to invalid, so software is used */
                info->pk.ecdh.private_key->devId = INVALID_DEVID;

                ret = wc_AriaDerive(
                    info->pk.ecdh.private_key, info->pk.ecdh.public_key,
                    info->pk.ecdh.out, info->pk.ecdh.outlen);

                if (ret != 0)
                    ret = CRYPTOCB_UNAVAILABLE;
                /* reset devId */
                info->pk.ecdh.private_key->devId = devIdArg;
            }
        }
        else if (info->algo_type == WC_ALGO_TYPE_HASH) {
            if (info->hash.type == WC_HASH_TYPE_SHA256) {
                if (info->hash.sha256 == NULL)
                    return CRYPTOCB_UNAVAILABLE;

                /* set devId to invalid, so software is used */
                info->hash.sha256->devId = INVALID_DEVID;

                if (info->hash.sha256->hSession == NULL) {
                    ret = wc_AriaInitSha(&(info->hash.sha256->hSession), MC_ALGID_SHA256);
                }

                if (((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                    && (info->hash.in != NULL)) {
                    ret = wc_AriaShaUpdate(info->hash.sha256->hSession,
                                        (byte *) info->hash.in, info->hash.inSz);
                }
                if (((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                    && (info->hash.digest != NULL)) {
                    MC_UINT digestSz = 32;
                    ret = wc_AriaShaFinal(info->hash.sha256->hSession,
                                        info->hash.digest, &digestSz);
                    if ((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                        ret = wc_AriaFree(&(info->hash.sha256->hSession),NULL);
                }
                if (ret != 0)
                    ret = CRYPTOCB_UNAVAILABLE;
                /* reset devId */
                info->hash.sha256->devId = devIdArg;
            }
            else if (info->hash.type == WC_HASH_TYPE_SHA384) {
                if (info->hash.sha384 == NULL)
                    return CRYPTOCB_UNAVAILABLE;

                /* set devId to invalid, so software is used */
                info->hash.sha384->devId = INVALID_DEVID;

                if (info->hash.sha384->hSession == NULL) {
                    ret = wc_AriaInitSha(&(info->hash.sha384->hSession), MC_ALGID_SHA384);
                }

                if (((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                    && (info->hash.in != NULL)) {
                    ret = wc_AriaShaUpdate(info->hash.sha384->hSession,
                                        (byte *) info->hash.in, info->hash.inSz);
                }
                if (((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                    && (info->hash.digest != NULL)) {
                    MC_UINT digestSz = 48;
                    ret = wc_AriaShaFinal(info->hash.sha384->hSession,
                                        info->hash.digest, &digestSz);
                    if ((ret == 0) || (ret == CRYPTOCB_UNAVAILABLE))
                        ret = wc_AriaFree(&(info->hash.sha384->hSession),NULL);
                }
                if (ret != 0) ret = CRYPTOCB_UNAVAILABLE;
                /* reset devId */
                info->hash.sha384->devId = devIdArg;
            }
        }

        return ret;
    }
#endif /* WOLF_CRYPTO_CB */

#endif /* HAVE_ARIA */
