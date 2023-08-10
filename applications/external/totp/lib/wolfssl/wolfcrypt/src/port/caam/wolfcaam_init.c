/*
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


/*
 * WOLFSSL_CAAM is used to enable CAAM support
 *
 * Different Hardware Ports
 * WOLFSSL_IMX6_CAAM build for QNX + IMX6
 * WOLFSSL_SECO_CAAM make use of NXP's SECO HSM library on i.MX8
 * WOLFSSL_IMXRT1170_CAAM make use of NXP's CAAM driver for RT1170 series boards
 *
 */
#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_CAAM)

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

#ifdef DEBUG_WOLFSSL
    #include <stdio.h>
#endif

/* determine which porting header to include */
#if defined(__INTEGRITY) || defined(INTEGRITY)
    #ifndef WC_CAAM_PASSWORD
        #define WC_CAAM_PASSWORD "!systempassword"
    #endif

    #include <INTEGRITY.h>
    static IODevice caam = NULLIODevice;
    #define CAAM_SEND_REQUEST(type, sz, arg, buf) \
        SynchronousSendIORequest(caam, (type), (const Value*)(arg), (buf))
#endif



#if defined(__INTEGRITY) || defined(INTEGRITY)
/* Allow runtime setting for CAAM IODevice in case user wants to use password
 * at run time.
 *
 * returns 0 on success
 *
 * NOTE this is how IODevice is defined in INTEGRITY "typedef struct
 *      IODeviceStruct        *IODevice;"
 */
int wc_caamSetResource(IODevice ioDev)
{
    WOLFSSL_MSG("Setting CAAM driver");
    caam = ioDev;
    return 0;
}
#endif


/* used to route crypto operations through crypto callback */
static int wc_CAAM_router(int devId, wc_CryptoInfo* info, void* ctx)
{
    int ret = CRYPTOCB_UNAVAILABLE;

    (void)ctx;
    (void)devId;
    switch (info->algo_type) {
        case WC_ALGO_TYPE_PK:
            switch (info->pk.type) {
            #if defined(HAVE_ECC) && defined(WOLFSSL_CAAM_ECC)
                case WC_PK_TYPE_ECDSA_SIGN:
                    ret = wc_CAAM_EccSign(info->pk.eccsign.in,
                            info->pk.eccsign.inlen, info->pk.eccsign.out,
                            info->pk.eccsign.outlen, info->pk.eccsign.rng,
                            info->pk.eccsign.key, devId);
                    break;

                case WC_PK_TYPE_ECDSA_VERIFY:
                    ret = wc_CAAM_EccVerify(info->pk.eccverify.sig,
                            info->pk.eccverify.siglen, info->pk.eccverify.hash,
                            info->pk.eccverify.hashlen, info->pk.eccverify.res,
                            info->pk.eccverify.key, devId);
                    break;

                #ifdef WOLFSSL_KEY_GEN
                case WC_PK_TYPE_EC_KEYGEN:
                    ret = wc_CAAM_MakeEccKey(info->pk.eckg.rng,
                            info->pk.eckg.size, info->pk.eckg.key,
                            info->pk.eckg.curveId, devId);
                    break;
                #endif /* WOLFSSL_KEY_GEN */

                case WC_PK_TYPE_ECDH:
                    ret = wc_CAAM_Ecdh(info->pk.ecdh.private_key,
                              info->pk.ecdh.public_key,
                              info->pk.ecdh.out,
                              info->pk.ecdh.outlen, devId);
                   break;

                case WC_PK_TYPE_EC_CHECK_PRIV_KEY:
                    ret = wc_CAAM_EccCheckPrivKey(info->pk.ecc_check.key,
                                   info->pk.ecc_check.pubKey,
                                   info->pk.ecc_check.pubKeySz);
                    break;
            #endif /* HAVE_ECC && WOLFSSL_CAAM_ECC */
            #if !defined(NO_RSA) && defined(WOLFSSL_DEVCRYPTO_RSA)
                case WC_PK_TYPE_RSA:
                    ret = wc_CAAM_Rsa(info->pk.rsa.in,
                                   info->pk.rsa.inLen,
                                   info->pk.rsa.out,
                                   info->pk.rsa.outLen,
                                   info->pk.rsa.type,
                                   info->pk.rsa.key,
                                   info->pk.rsa.rng);
                    break;

                #ifdef WOLFSSL_KEY_GEN
                case WC_PK_TYPE_RSA_KEYGEN:
                    ret = wc_CAAM_MakeRsaKey(info->pk.rsakg.key,
                                info->pk.rsakg.size,
                                info->pk.rsakg.e,
                                info->pk.rsakg.rng);
                    break;
                #endif
            #endif /* !NO_RSA */
            #if defined(HAVE_CURVE25519) && defined(WOLFSSL_CAAM_CURVE25519)
                case WC_PK_TYPE_CURVE25519_KEYGEN:
                    ret = wc_CAAM_MakeCurve25519Key(info->pk.curve25519kg.key,
                        info->pk.curve25519kg.size,
                        info->pk.curve25519kg.rng);
                    break;

                case WC_PK_TYPE_CURVE25519:
                    ret = wc_CAAM_Curve25519(info->pk.curve25519.out,
                        info->pk.curve25519.outlen,
                        info->pk.curve25519.private_key,
                        info->pk.curve25519.public_key,
                        info->pk.curve25519.endian);
                    break;
            #endif /* HAVE_CURVE25519 && WOLFSSL_CAAM_CURVE25519 */
                default:
                    WOLFSSL_MSG("unsupported public key operation");
            }
            break;

        case WC_ALGO_TYPE_CMAC:
        #if defined(WOLFSSL_CMAC) && defined(WOLFSSL_CAAM_CMAC)
        #ifdef WOLFSSL_SECO_CAAM
            if (devId != WOLFSSL_SECO_DEVID)
                break;
        #endif
        #if defined(WOLFSSL_CMAC) && !defined(NO_AES) && \
            defined(WOLFSSL_AES_DIRECT)
            ret = wc_CAAM_Cmac(info->cmac.cmac,
                        info->cmac.key,
                        info->cmac.keySz,
                        info->cmac.in,
                        info->cmac.inSz,
                        info->cmac.out,
                        info->cmac.outSz,
                        info->cmac.type,
                        info->cmac.ctx);
        #else
            WOLFSSL_MSG("CMAC not compiled in");
            ret = NOT_COMPILED_IN;
        #endif
        #endif /* WOLFSSL_CMAC && WOLFSSL_CAAM_CMAC */
            break;

        case WC_ALGO_TYPE_HASH:
        #ifdef WOLFSSL_CAAM_HASH
            switch(info->hash.type) {
                #ifdef WOLFSSL_SHA224
                case WC_HASH_TYPE_SHA224:
                    ret = wc_CAAM_Sha224Hash(info->hash.sha224,
                                info->hash.in,
                                info->hash.inSz,
                                info->hash.digest);
                    break;
                #endif

                case WC_HASH_TYPE_SHA256:
                    ret = wc_CAAM_Sha256Hash(info->hash.sha256,
                              info->hash.in,
                              info->hash.inSz,
                              info->hash.digest);
                    break;

                #ifdef WOLFSSL_SHA384
                case WC_HASH_TYPE_SHA384:
                    ret = wc_CAAM_Sha384Hash(info->hash.sha384,
                              info->hash.in,
                              info->hash.inSz,
                              info->hash.digest);
                    break;
                #endif
                #ifdef WOLFSSL_SHA512
                case WC_HASH_TYPE_SHA512:
                    ret = wc_CAAM_Sha512Hash(info->hash.sha512,
                              info->hash.in,
                              info->hash.inSz,
                              info->hash.digest);
                    break;
                #endif
                default:
                    WOLFSSL_MSG("Unknown or unsupported hash type");
                    ret = CRYPTOCB_UNAVAILABLE;
            }
        #endif /* WOLFSSL_CAAM_HASH */
        break;

        case WC_ALGO_TYPE_HMAC:
        #if defined(WOLFSSL_CAAM_HMAC)
            ret = wc_CAAM_Hmac(info->hmac.hmac,
                        info->hmac.macType,
                        info->hmac.in, info->hmac.inSz,
                        info->hmac.digest);
       #endif
            break;

        case WC_ALGO_TYPE_CIPHER:
        #if defined(WOLFSSL_CAAM_CIPHER)
        #ifdef WOLFSSL_SECO_CAAM
            if (devId != WOLFSSL_SECO_DEVID)
                break; /* only call to SECO if using WOLFSSL_SECO_DEVID */
        #endif
            switch (info->cipher.type) {
            #if defined(HAVE_AESCCM) && defined(WOLFSSL_CAAM_AESCCM)
                case WC_CIPHER_AES_CCM:
                    if (info->cipher.enc == 1) {
                        ret = wc_CAAM_AesCcmEncrypt(
                                          info->cipher.aesccm_enc.aes,
                                          info->cipher.aesccm_enc.in,
                                          info->cipher.aesccm_enc.out,
                                          info->cipher.aesccm_enc.sz,
                                          info->cipher.aesccm_enc.nonce,
                                          info->cipher.aesccm_enc.nonceSz,
                                          info->cipher.aesccm_enc.authTag,
                                          info->cipher.aesccm_enc.authTagSz,
                                          info->cipher.aesccm_enc.authIn,
                                          info->cipher.aesccm_enc.authInSz);
                    }
                    else {
                        ret = wc_CAAM_AesCcmDecrypt(
                                      info->cipher.aesccm_dec.aes,
                                      info->cipher.aesccm_dec.in,
                                      info->cipher.aesccm_dec.out,
                                      info->cipher.aesccm_dec.sz,
                                      info->cipher.aesccm_dec.nonce,
                                      info->cipher.aesccm_dec.nonceSz,
                                      info->cipher.aesccm_dec.authTag,
                                      info->cipher.aesccm_dec.authTagSz,
                                      info->cipher.aesccm_dec.authIn,
                                      info->cipher.aesccm_dec.authInSz);
                    }
                    break;
                #endif /* HAVE_AESCCM */
                #if defined(HAVE_AESGCM) && defined(WOLFSSL_CAAM_AESGCM)
                case WC_CIPHER_AES_GCM:
                    if (info->cipher.enc == 1) {
                        ret = wc_CAAM_AesGcmEncrypt(
                                      info->cipher.aesgcm_enc.aes,
                                      info->cipher.aesgcm_enc.in,
                                      info->cipher.aesgcm_enc.out,
                                      info->cipher.aesgcm_enc.sz,
                                      info->cipher.aesgcm_enc.iv,
                                      info->cipher.aesgcm_enc.ivSz,
                                      info->cipher.aesgcm_enc.authTag,
                                      info->cipher.aesgcm_enc.authTagSz,
                                      info->cipher.aesgcm_enc.authIn,
                                      info->cipher.aesgcm_enc.authInSz);
                    }
                    else {
                        ret = wc_CAAM_AesGcmDecrypt(
                                  info->cipher.aesgcm_dec.aes,
                                  info->cipher.aesgcm_dec.in,
                                  info->cipher.aesgcm_dec.out,
                                  info->cipher.aesgcm_dec.sz,
                                  info->cipher.aesgcm_dec.iv,
                                  info->cipher.aesgcm_dec.ivSz,
                                  info->cipher.aesgcm_dec.authTag,
                                  info->cipher.aesgcm_dec.authTagSz,
                                  info->cipher.aesgcm_dec.authIn,
                                  info->cipher.aesgcm_dec.authInSz);
                    }
                    break;
                #endif /* HAVE_AESGCM && WOLFSSL_CAAM_AESGCM */

                case WC_CIPHER_AES_CBC:
                    if (info->cipher.enc == 1) {
                        ret = wc_CAAM_AesCbcEncrypt(info->cipher.aescbc.aes,
                                info->cipher.aescbc.out,
                                info->cipher.aescbc.in,
                                info->cipher.aescbc.sz);
                    }
                    else {
                        ret = wc_CAAM_AesCbcDecrypt(info->cipher.aescbc.aes,
                                info->cipher.aescbc.out,
                                info->cipher.aescbc.in,
                                info->cipher.aescbc.sz);
                    }
                    break;

            #ifdef WOLFSSL_AES_COUNTER
                case WC_CIPHER_AES_CTR:
                    ret = wc_CAAM_AesCtrEncrypt(info->cipher.aesctr.aes,
                                info->cipher.aesctr.out,
                                info->cipher.aesctr.in,
                                info->cipher.aesctr.sz);
                    break;
            #endif /* WOLFSSL_AES_COUNTER */

            #if defined(HAVE_AES_ECB)
                case WC_CIPHER_AES_ECB:
                    if (info->cipher.enc == 1) {
                        ret = wc_CAAM_AesEcbEncrypt(info->cipher.aesecb.aes,
                                    info->cipher.aesecb.out,
                                    info->cipher.aesecb.in,
                                    info->cipher.aesecb.sz);
                    }
                    else {
                        ret = wc_CAAM_AesEcbDecrypt(info->cipher.aesecb.aes,
                                    info->cipher.aesecb.out,
                                    info->cipher.aesecb.in,
                                    info->cipher.aesecb.sz);
                        }
            #endif /* HAVE_AES_ECB */
            }
        #endif /* WOLFSSL_CAAM_CIPHER */
            break;

        case WC_ALGO_TYPE_RNG:
        case WC_ALGO_TYPE_SEED:
        case WC_ALGO_TYPE_NONE:
        default:
            /* Not implemented yet with CAAM */
            ret = CRYPTOCB_UNAVAILABLE;
    }

    return ret;
}


/* Check hardware support
 *
 * returns 0 on success
 */
int wc_caamInit(void)
{
    int ret = 0;
    WOLFSSL_MSG("Starting interface with CAAM driver");
    if (CAAM_INIT_INTERFACE() != 0) {
        WOLFSSL_MSG("Error initializing CAAM");
        return -1;
    }

#if 0
    /* check that for implemented modules
     * bits 0-3 AES, 4-7 DES, 12-15 Hashing , 16-19 RNG, 28-31 public key module  */
    reg = WC_CAAM_READ(CAMM_SUPPORT_LS);

    #ifndef WC_NO_RNG
    if (((reg & 0x000F0000) >> 16) > 0) {
        WOLFSSL_MSG("Found CAAM RNG hardware module");
        if ((WC_CAAM_READ(CAAM_RTMCTL) & 0x40000001) != 0x40000001) {
             WOLFSSL_MSG("Error CAAM RNG has not been set up");
        }
    }
    #endif

    #ifndef NO_SHA256
    if ((reg & 0x0000F000) > 0) {
        WOLFSSL_MSG("Found CAAM MDHA module");
    }
    else {
        WOLFSSL_MSG("Hashing not supported by CAAM");
        return WC_HW_E;
    }
    #endif

    #ifndef NO_AES
    if ((reg & 0x0000000F) > 0) {
        WOLFSSL_MSG("Found CAAM AES module");
    }
    else {
        WOLFSSL_MSG("AES not supported by CAAM");
        return WC_HW_E;
    }
    #endif

    #ifdef HAVE_ECC
    if ((reg & 0xF0000000) > 0) {
        WOLFSSL_MSG("Found CAAM Public Key module");
    }
    else {
        WOLFSSL_MSG("Public Key not supported by CAAM");
    }
    #endif
#endif

#ifdef WOLFSSL_SECO_CAAM
    ret = wc_CryptoDev_RegisterDevice(WOLFSSL_SECO_DEVID, wc_CAAM_router,
            NULL);
#endif
    if (ret == 0) {
        ret = wc_CryptoDev_RegisterDevice(WOLFSSL_CAAM_DEVID, wc_CAAM_router,
            NULL);
    }
    return ret;
}


/* free up all resources used for CAAM */
int wc_caamFree(void)
{
    CAAM_FREE_INTERFACE();
    return 0;
}


#if defined(__INTEGRITY) || defined(INTEGRITY)
word32 wc_caamReadRegister(word32 reg)
{
    word32 out = 0;

    if (caam == NULLIODevice) {
         WOLFSSL_MSG("Error CAAM IODevice not found! Bad password?");
         return 0;
    }

    if (ReadIODeviceRegister(caam, reg, &out) != Success) {
         WOLFSSL_MSG("Error reading register");
    }

    return (word32)out;
}


/* returns 0 on success */
int wc_caamWriteRegister(word32 reg, word32 value)
{
    if (caam == NULLIODevice) {
         WOLFSSL_MSG("Error CAAM IODevice not found! Bad password?");
         return -1;
    }

    if (WriteIODeviceRegister(caam, reg, value) != Success) {
         WOLFSSL_MSG("Error writing to register");
    }
    return 0;
}
#endif


/* return 0 on success and WC_HW_E on failure. Can also return WC_HW_WAIT_E
 * in the case that the driver is waiting for a resource or RAN_BLOCK_E if
 * waiting for entropy. */
int wc_caamAddAndWait(CAAM_BUFFER* buf, int sz, word32 arg[4], word32 type)
{
    int ret;
#ifdef DEBUG_WOLFSSL
    static int wait = 0;
#endif

#if defined(__INTEGRITY) || defined(INTEGRITY)
    if (caam == NULLIODevice) {
        WOLFSSL_MSG("Error CAAM IODevice not found! Bad password?");
        return WC_HW_E;
    }
#endif

    if ((ret = CAAM_SEND_REQUEST(type, sz, arg, buf)) != Success) {
        /* if waiting for resource or RNG return waiting */
        if (ret == CAAM_WAITING) {
        #ifdef DEBUG_WOLFSSL
            if (wait == 0) {
                wait = 1;
                WOLFSSL_MSG("Waiting on entropy from driver");
            }
            fprintf(stderr, ".");
        #endif
            return RAN_BLOCK_E;
        }

        if (ret == ResourceNotAvailable) {
            WOLFSSL_MSG("Waiting on CAAM driver");
            return WC_HW_WAIT_E;
        }

        return WC_HW_E;
    }
#ifdef DEBUG_WOLFSSL
    if (wait) {
        wait = 0;
        fprintf(stderr, "\n");
    }
#endif

    (void)ret;
    return 0;
}


#ifdef WOLFSSL_CAAM_BLOB
/* Create a red or black blob
 *
 * mod : key modifier, expected 8 bytes for RED key types and 16 for BLACK
 *       if 'mod' is null than 0's are used
 *
 * returns 0 on success
 */
int wc_caamCreateBlob_ex(byte* data, word32 dataSz, byte* out, word32* outSz,
        int type, byte* mod, word32 modSz)
{
    CAAM_BUFFER in[3];
    word32 arg[4];
    int ret;
    byte local[WC_CAAM_BLACK_KEYMOD_SZ] = {0};
    byte* keyMod;
    int   keyModSz;

    keyMod = mod;
    XMEMSET(local, 0, sizeof(local));
    if (data == NULL || out == NULL || outSz == NULL ||
            *outSz < dataSz + WC_CAAM_BLOB_SZ) {
        return BAD_FUNC_ARG;
    }

    if (type == WC_CAAM_BLOB_RED) {
        arg[0] = 0;
        if (mod != NULL) {
            if (modSz != WC_CAAM_RED_KEYMOD_SZ) {
                WOLFSSL_MSG("bad key mod red size");
                return BAD_FUNC_ARG;
            }
        }
        keyModSz = WC_CAAM_RED_KEYMOD_SZ;
    }
    else if (type == WC_CAAM_BLOB_BLACK) {
        arg[0] = 1;
        if (mod != NULL) {
            if (modSz != WC_CAAM_BLACK_KEYMOD_SZ) {
                WOLFSSL_MSG("bad key mod black size");
                return BAD_FUNC_ARG;
            }
        }
        keyModSz = WC_CAAM_BLACK_KEYMOD_SZ;
    }
    else {
        WOLFSSL_MSG("unknown blob type!");
        return BAD_FUNC_ARG;
    }

    if (mod == NULL) {
        WOLFSSL_MSG("using local all 0's key modifier");
        keyMod = local;
    }

    in[0].BufferType = DataBuffer;
    in[0].TheAddress = (CAAM_ADDRESS)keyMod;
    in[0].Length = keyModSz;

    in[1].BufferType = DataBuffer;
    in[1].TheAddress = (CAAM_ADDRESS)data;
    in[1].Length = dataSz;

    in[2].BufferType = DataBuffer | LastBuffer;
    in[2].TheAddress = (CAAM_ADDRESS)out;
    in[2].Length = dataSz + WC_CAAM_BLOB_SZ;

    arg[2] = dataSz;
    arg[3] = keyModSz;

    if ((ret = wc_caamAddAndWait(in, 3, arg, CAAM_BLOB_ENCAP)) != 0) {
        WOLFSSL_MSG("Error with CAAM blob create");
        return ret;
    }

    *outSz = dataSz + WC_CAAM_BLOB_SZ;
    return 0;
}


/* create a red key blob
 * returns 0 on success */
int wc_caamCreateBlob(byte* data, word32 dataSz, byte* out, word32* outSz)
{
    return wc_caamCreateBlob_ex(data, dataSz, out, outSz, WC_CAAM_BLOB_RED,
            NULL, 0);
}


/* uncover black or red keys
 * returns 0 on success */
int wc_caamOpenBlob_ex(byte* data, word32 dataSz, byte* out, word32* outSz,
        int type, byte* mod, word32 modSz)
{
    CAAM_BUFFER in[3];
    word32      arg[4];
    int   ret;
    byte  local[WC_CAAM_BLACK_KEYMOD_SZ];
    byte* keyMod;
    int   keyModSz;

    keyMod = mod;
    XMEMSET(local, 0, sizeof(local));

    if (data == NULL || out == NULL || outSz == NULL ||
            *outSz < dataSz - WC_CAAM_BLOB_SZ) {
        WOLFSSL_MSG("NULL argument or outSz is too small");
        return BAD_FUNC_ARG;
    }

    if (type == WC_CAAM_BLOB_RED) {
        arg[0] = 0;
        if (mod != NULL) {
            if (modSz != WC_CAAM_RED_KEYMOD_SZ) {
                WOLFSSL_MSG("bad key mod red size");
                return BAD_FUNC_ARG;
            }
        }
        keyModSz = WC_CAAM_RED_KEYMOD_SZ;
    }
    else if (type == WC_CAAM_BLOB_BLACK) {
        arg[0] = 1;
        if (mod != NULL) {
            if (modSz != WC_CAAM_BLACK_KEYMOD_SZ) {
                WOLFSSL_MSG("bad key mod black size");
                return BAD_FUNC_ARG;
            }
        }
        keyModSz = WC_CAAM_BLACK_KEYMOD_SZ;
    }
    else {
        WOLFSSL_MSG("unknown blob type!");
        return BAD_FUNC_ARG;
    }

    if (mod == NULL) {
        WOLFSSL_MSG("using local all 0's key modifier");
        keyMod = local;
    }

    in[0].BufferType = DataBuffer;
    in[0].TheAddress = (CAAM_ADDRESS)keyMod;
    in[0].Length = keyModSz;

    in[1].BufferType = DataBuffer;
    in[1].TheAddress = (CAAM_ADDRESS)data;
    in[1].Length = dataSz;

    in[2].BufferType = DataBuffer | LastBuffer;
    in[2].TheAddress = (CAAM_ADDRESS)out;
    in[2].Length = dataSz - WC_CAAM_BLOB_SZ;

    arg[2] = dataSz;
    arg[3] = keyModSz;

    if ((ret = wc_caamAddAndWait(in, 3, arg, CAAM_BLOB_DECAP)) != 0) {
        WOLFSSL_MSG("Error with CAAM blob open");
        return ret;
    }

    *outSz = dataSz - WC_CAAM_BLOB_SZ;
    return 0;
}


/* open a red blob
 * returns 0 on success */
int wc_caamOpenBlob(byte* data, word32 dataSz, byte* out, word32* outSz)
{
    return wc_caamOpenBlob_ex(data, dataSz, out, outSz, WC_CAAM_BLOB_RED,
            NULL, 0);
}
#endif /* WOLFSSL_CAAM_BLOB */

/* outSz gets set to key size plus 16 for mac and padding 
 * return 0 on success
 */
int wc_caamCoverKey(byte* in, word32 inSz, byte* out, word32* outSz, int flag)
{
    CAAM_BUFFER buf[2];
    word32 arg[4];
    int ret;

    if (*outSz < inSz + WC_CAAM_MAC_SZ) {
        return BUFFER_E;
    }

    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (CAAM_ADDRESS)in;
    buf[0].Length = inSz;

    buf[1].BufferType = DataBuffer;
    buf[1].TheAddress = (CAAM_ADDRESS)out;
    buf[1].Length = inSz;

    (void)flag; /* for now defaulting to use highest security AES-CCM here */
    arg[0] = CAAM_FIFO_CCM_FLAG;
    arg[1] = inSz;
    if ((ret = wc_caamAddAndWait(buf, 2, arg, CAAM_FIFO_S)) != 0) {
        WOLFSSL_MSG("Error with CAAM blob create");
        return ret;
    }

    *outSz = inSz + WC_CAAM_MAC_SZ;
    return 0;
}


/* return 0 or greater on success for the partition number available
 * returns a negative value on failure
 */
int caamFindUnusedPartition()
{
    CAAM_BUFFER buf[1];
    word32 arg[4];
    int ret = 0;

    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (CAAM_ADDRESS)&ret;
    buf[0].Length     = sizeof(int);

    if ((wc_caamAddAndWait(buf, 1, arg, CAAM_FIND_PART)) != 0) {
        WOLFSSL_MSG("Error finding a partition to use");
        return -1;
    }

    return ret;
}


/* return the address of the given partition number "part" */
CAAM_ADDRESS caamGetPartition(int part, int sz)
{
    CAAM_BUFFER buf[1];
    word32 arg[4];
    CAAM_ADDRESS ret = 0;

    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (CAAM_ADDRESS)(&ret);
    buf[0].Length     = sizeof(int);

    arg[0] = part;
    arg[1] = sz;

    if ((wc_caamAddAndWait(buf, 1, arg, CAAM_GET_PART)) != 0) {
        WOLFSSL_MSG("Error getting a partition");
        return -1;
    }

    return ret;
}


/* Internal function to free a secure partition
 * return 0 on success */
int caamFreePart(int partNum)
{
    word32 arg[4];

    arg[0] = partNum;

    if ((wc_caamAddAndWait(NULL, 0, arg, CAAM_FREE_PART)) != 0) {
        WOLFSSL_MSG("Error freeing a partition");
        return -1;
    }

    return 0;
}


/* Internal function to help write to a secure partition
 * return 0 on success */
int caamWriteToPartition(CAAM_ADDRESS addr, const unsigned char* in, int inSz)
{
    CAAM_BUFFER buf[1];
    word32 arg[4];

    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (CAAM_ADDRESS)in;
    buf[0].Length = inSz;

    arg[0] = addr;
    arg[1] = inSz;

    if ((wc_caamAddAndWait(buf, 1, arg, CAAM_WRITE_PART)) != 0) {
        WOLFSSL_MSG("Error writing to a partition");
        return -1;
    }

    return 0;
}


/* Internal function to help read from a secure partition
 * return 0 on success */
int caamReadPartition(CAAM_ADDRESS addr, unsigned char* out, int outSz)
{
    CAAM_BUFFER buf[1];
    word32 arg[4];

    buf[0].BufferType = DataBuffer;
    buf[0].TheAddress = (CAAM_ADDRESS)out;
    buf[0].Length = outSz;

    arg[0] = addr;
    arg[1] = outSz;

    if ((wc_caamAddAndWait(buf, 1, arg, CAAM_READ_PART)) != 0) {
        WOLFSSL_MSG("Error reading a partition");
        return -1;
    }

    return 0;
}

#endif /* WOLFSSL_CAAM */
