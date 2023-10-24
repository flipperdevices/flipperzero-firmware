/* pkcs7.h
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
    \file wolfssl/wolfcrypt/pkcs7.h
*/

#ifndef WOLF_CRYPT_PKCS7_H
#define WOLF_CRYPT_PKCS7_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_PKCS7

#ifndef NO_ASN
    #include <wolfssl/wolfcrypt/asn.h>
#endif
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/random.h>
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif
#ifndef NO_DES3
    #include <wolfssl/wolfcrypt/des3.h>
#endif
#include <wolfssl/wolfcrypt/wc_encrypt.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Max number of certificates that PKCS7 structure can parse */
#ifndef MAX_PKCS7_CERTS
#ifdef OPENSSL_ALL
    #define MAX_PKCS7_CERTS 15
#else
    #define MAX_PKCS7_CERTS 4
#endif
#endif

#ifndef MAX_ORI_TYPE_SZ
    #define MAX_ORI_TYPE_SZ  MAX_OID_SZ
#endif
#ifndef MAX_ORI_VALUE_SZ
    #define MAX_ORI_VALUE_SZ 512
#endif

#ifndef MAX_SIGNED_ATTRIBS_SZ
    #define MAX_SIGNED_ATTRIBS_SZ 7
#endif

#ifndef MAX_AUTH_ATTRIBS_SZ
    #define MAX_AUTH_ATTRIBS_SZ 7
#endif

#ifndef MAX_UNAUTH_ATTRIBS_SZ
    #define MAX_UNAUTH_ATTRIBS_SZ 7
#endif

/* bitmap flag for attributes */
#define WOLFSSL_NO_ATTRIBUTES 0x1
#define WOLFSSL_CONTENT_TYPE_ATTRIBUTE 0x2
#define WOLFSSL_SIGNING_TIME_ATTRIBUTE 0x4
#define WOLFSSL_MESSAGE_DIGEST_ATTRIBUTE 0x8

/* PKCS#7 content types, ref RFC 2315 (Section 14) */
enum PKCS7_TYPES {
    PKCS7_MSG                 = 650,  /* 1.2.840.113549.1.7   */
    DATA                      = 651,  /* 1.2.840.113549.1.7.1 */
    SIGNED_DATA               = 652,  /* 1.2.840.113549.1.7.2 */
    ENVELOPED_DATA            = 653,  /* 1.2.840.113549.1.7.3 */
    SIGNED_AND_ENVELOPED_DATA = 654,  /* 1.2.840.113549.1.7.4 */
    DIGESTED_DATA             = 655,  /* 1.2.840.113549.1.7.5 */
    ENCRYPTED_DATA            = 656,  /* 1.2.840.113549.1.7.6 */
#if defined(HAVE_LIBZ) && !defined(NO_PKCS7_COMPRESSED_DATA)
    COMPRESSED_DATA           = 678,  /* 1.2.840.113549.1.9.16.1.9,  RFC 3274 */
#endif
    FIRMWARE_PKG_DATA         = 685,  /* 1.2.840.113549.1.9.16.1.16, RFC 4108 */
    AUTH_ENVELOPED_DATA       = 692   /* 1.2.840.113549.1.9.16.1.23, RFC 5083 */
};

enum PKCS7_STATE {
    WC_PKCS7_START = 0,

    /* decode encrypted */
    WC_PKCS7_STAGE2,
    WC_PKCS7_STAGE3,
    WC_PKCS7_STAGE4,
    WC_PKCS7_STAGE5,
    WC_PKCS7_STAGE6,

    WC_PKCS7_VERIFY_STAGE2,
    WC_PKCS7_VERIFY_STAGE3,
    WC_PKCS7_VERIFY_STAGE4,
    WC_PKCS7_VERIFY_STAGE5,
    WC_PKCS7_VERIFY_STAGE6,

    /* parse info set */
    WC_PKCS7_INFOSET_START,
    WC_PKCS7_INFOSET_BER,
    WC_PKCS7_INFOSET_STAGE1,
    WC_PKCS7_INFOSET_STAGE2,
    WC_PKCS7_INFOSET_END,

    /* decode enveloped data */
    WC_PKCS7_ENV_2,
    WC_PKCS7_ENV_3,
    WC_PKCS7_ENV_4,
    WC_PKCS7_ENV_5,

    /* decode auth enveloped */
    WC_PKCS7_AUTHENV_2,
    WC_PKCS7_AUTHENV_3,
    WC_PKCS7_AUTHENV_4,
    WC_PKCS7_AUTHENV_5,
    WC_PKCS7_AUTHENV_6,
    WC_PKCS7_AUTHENV_ATRB,
    WC_PKCS7_AUTHENV_ATRBEND,
    WC_PKCS7_AUTHENV_7,

    /* decryption state types */
    WC_PKCS7_DECRYPT_KTRI,
    WC_PKCS7_DECRYPT_KTRI_2,
    WC_PKCS7_DECRYPT_KTRI_3,


    WC_PKCS7_DECRYPT_KARI,
    WC_PKCS7_DECRYPT_KEKRI,
    WC_PKCS7_DECRYPT_PWRI,
    WC_PKCS7_DECRYPT_ORI,

    WC_PKCS7_DECRYPT_DONE

};

enum Pkcs7_Misc {
    PKCS7_NONCE_SZ        = 16,
    MAX_ENCRYPTED_KEY_SZ  = 512,    /* max enc. key size, RSA <= 4096 */
    MAX_CONTENT_KEY_LEN   = 32,     /* highest current cipher is AES-256-CBC */
    MAX_CONTENT_IV_SIZE   = 16,     /* highest current is AES128 */
#ifndef NO_AES
    MAX_CONTENT_BLOCK_LEN = AES_BLOCK_SIZE,
#else
    MAX_CONTENT_BLOCK_LEN = DES_BLOCK_SIZE,
#endif
    MAX_RECIP_SZ          = MAX_VERSION_SZ +
                            MAX_SEQ_SZ + WC_ASN_NAME_MAX + MAX_SN_SZ +
                            MAX_SEQ_SZ + MAX_ALGO_SZ + 1 + MAX_ENCRYPTED_KEY_SZ,
    WOLF_ENUM_DUMMY_LAST_ELEMENT(Pkcs7_Misc)
};

enum Cms_Options {
    CMS_SKID = 1,
    CMS_ISSUER_AND_SERIAL_NUMBER = 2
};
#define DEGENERATE_SID 3

/* CMS/PKCS#7 RecipientInfo types, RFC 5652, Section 6.2 */
enum Pkcs7_RecipientInfo_Types {
    PKCS7_KTRI  = 0,
    PKCS7_KARI  = 1,
    PKCS7_KEKRI = 2,
    PKCS7_PWRI  = 3,
    PKCS7_ORI   = 4
};

typedef struct PKCS7Attrib {
    const byte* oid;
    word32 oidSz;
    const byte* value;
    word32 valueSz;
} PKCS7Attrib;


typedef struct PKCS7DecodedAttrib {
    struct PKCS7DecodedAttrib* next;
    byte* oid;
    word32 oidSz;
    byte* value;
    word32 valueSz;
} PKCS7DecodedAttrib;

typedef struct PKCS7State PKCS7State;
typedef struct Pkcs7Cert Pkcs7Cert;
typedef struct Pkcs7EncodedRecip Pkcs7EncodedRecip;
typedef struct PKCS7 PKCS7;
typedef struct PKCS7 PKCS7_SIGNED;
typedef struct PKCS7SignerInfo PKCS7SignerInfo;

/* OtherRecipientInfo decrypt callback prototype */
typedef int (*CallbackOriDecrypt)(PKCS7* pkcs7, byte* oriType, word32 oriTypeSz,
                                  byte* oriValue, word32 oriValueSz,
                                  byte* decryptedKey, word32* decryptedKeySz,
                                  void* ctx);
typedef int (*CallbackOriEncrypt)(PKCS7* pkcs7, byte* cek, word32 cekSz,
                                  byte* oriType, word32* oriTypeSz,
                                  byte* oriValue, word32* oriValueSz,
                                  void* ctx);
typedef int (*CallbackDecryptContent)(PKCS7* pkcs7, int encryptOID,
                                   byte* iv, int ivSz, byte* aad, word32 aadSz,
                                   byte* authTag, word32 authTagSz, byte* in,
                                   int inSz, byte* out, void* ctx);
typedef int (*CallbackWrapCEK)(PKCS7* pkcs7, byte* cek, word32 cekSz,
                                  byte* keyId, word32 keyIdSz,
                                  byte* originKey, word32 originKeySz,
                                  byte* out, word32 outSz,
                                  int keyWrapAlgo, int type, int dir);

#if defined(HAVE_PKCS7_RSA_RAW_SIGN_CALLBACK) && !defined(NO_RSA)
/* RSA sign raw digest callback, user builds DigestInfo */
typedef int (*CallbackRsaSignRawDigest)(PKCS7* pkcs7, byte* digest,
                                   word32 digestSz, byte* out, word32 outSz,
                                   byte* privateKey, word32 privateKeySz,
                                   int devId, int hashOID);
#endif

/* Public Structure Warning:
 * Existing members must not be changed to maintain backwards compatibility!
 */
struct PKCS7 {
    WC_RNG* rng;
    PKCS7Attrib* signedAttribs;
    byte*  content;               /* inner content, not owner             */
    byte*  contentDynamic;        /* content if constructed OCTET_STRING  */
    byte*  singleCert;            /* recipient cert, DER, not owner       */
    const byte* issuer;           /* issuer name of singleCert            */
    byte*  privateKey;            /* private key, DER, not owner          */
    void*  heap;                  /* heap hint for dynamic memory         */
#ifdef ASN_BER_TO_DER
    byte*  der;                   /* DER encoded version of message       */
    word32 derSz;
#endif
    byte*  cert[MAX_PKCS7_CERTS]; /* array of certs parsed from bundle */
    byte*  verifyCert;            /* cert from array used for verify */
    word32 verifyCertSz;

    /* Encrypted-data Content Type */
    byte*        encryptionKey;         /* block cipher encryption key */
    PKCS7Attrib* unprotectedAttribs;    /* optional */
    PKCS7DecodedAttrib* decodedAttrib;  /* linked list of decoded attribs */

    /* Enveloped-data optional ukm, not owner */
    byte*  ukm;
    word32 ukmSz;

    word32 encryptionKeySz;       /* size of key buffer, bytes */
    word32 unprotectedAttribsSz;
    word32 contentSz;             /* content size                         */
    word32 singleCertSz;          /* size of recipient cert buffer, bytes */
    word32 issuerSz;              /* length of issuer name                */
    word32 issuerSnSz;            /* length of serial number              */

    word32 publicKeySz;
    word32 publicKeyOID;          /* key OID (RSAk, ECDSAk, etc) */
    word32 privateKeySz;          /* size of private key buffer, bytes    */
    word32 signedAttribsSz;
    int contentOID;               /* PKCS#7 content type OID sum          */
    int hashOID;
    int encryptOID;               /* key encryption algorithm OID         */
    int keyWrapOID;               /* key wrap algorithm OID               */
    int keyAgreeOID;              /* key agreement algorithm OID          */
    int devId;                    /* device ID for HW based private key   */
    byte issuerHash[KEYID_SIZE];  /* hash of all alt Names                */
    byte issuerSn[MAX_SN_SZ];     /* singleCert's serial number           */
    byte publicKey[MAX_RSA_INT_SZ + MAX_RSA_E_SZ]; /* MAX RSA key size (m + e)*/
    word32 certSz[MAX_PKCS7_CERTS];

     /* flags - up to 16-bits */
    word16 isDynamic:1;
    word16 noDegenerate:1; /* allow degenerate case in verify function */
    word16 detached:1;     /* generate detached SignedData signature bundles */

    byte contentType[MAX_OID_SZ]; /* custom contentType byte array */
    word32 contentTypeSz;         /* size of contentType, bytes */

    int sidType;                  /* SignerIdentifier type to use, of type
                                     Pkcs7_SignerIdentifier_Types, default to
                                     SID_ISSUER_AND_SERIAL_NUMBER */
    byte issuerSubjKeyId[KEYID_SIZE];  /* SubjectKeyIdentifier of singleCert  */
    Pkcs7Cert* certList;          /* certificates list for SignedData set */
    Pkcs7EncodedRecip* recipList; /* recipients list */
    byte* cek;                    /* content encryption key, random, dynamic */
    word32 cekSz;                 /* size of cek, bytes */
    byte* pass;                   /* password, for PWRI decryption */
    word32 passSz;                /* size of pass, bytes */
    int kekEncryptOID;            /* KEK encryption algorithm OID */

    CallbackOriEncrypt oriEncryptCb;  /* ORI encrypt callback */
    CallbackOriDecrypt oriDecryptCb;  /* ORI decrypt callback */
    void* oriEncryptCtx;              /* ORI encrypt user context ptr */
    void* oriDecryptCtx;              /* ORI decrypt user context ptr */

    PKCS7Attrib* authAttribs;     /* authenticated attribs */
    word32 authAttribsSz;
    PKCS7Attrib* unauthAttribs;   /* unauthenticated attribs */
    word32 unauthAttribsSz;

#ifndef NO_PKCS7_STREAM
    PKCS7State* stream;
#endif
    word32 state;

    word16 defaultSignedAttribs; /* set which default signed attribs */

    byte version; /* 1 for RFC 2315 and 3 for RFC 4108 */
    PKCS7SignerInfo* signerInfo;
    CallbackDecryptContent decryptionCb;
    CallbackWrapCEK        wrapCEKCb;
    void*            decryptionCtx;

    byte* signature;
    byte* plainDigest;
    byte* pkcs7Digest;
    word32 signatureSz;
    word32 plainDigestSz;
    word32 pkcs7DigestSz;

#if defined(HAVE_PKCS7_RSA_RAW_SIGN_CALLBACK) && !defined(NO_RSA)
    CallbackRsaSignRawDigest rsaSignRawDigestCb;
#endif

    /* used by DecodeEnvelopedData with multiple encrypted contents */
    byte*  cachedEncryptedContent;
    word32 cachedEncryptedContentSz;
    word16 contentCRLF:1; /* have content line endings been converted to CRLF */
    /* !! NEW DATA MEMBERS MUST BE ADDED AT END !! */
};

WOLFSSL_API PKCS7* wc_PKCS7_New(void* heap, int devId);
WOLFSSL_API int  wc_PKCS7_Init(PKCS7* pkcs7, void* heap, int devId);
WOLFSSL_API int  wc_PKCS7_InitWithCert(PKCS7* pkcs7, byte* der, word32 derSz);
WOLFSSL_API int  wc_PKCS7_AddCertificate(PKCS7* pkcs7, byte* der, word32 derSz);
WOLFSSL_API void wc_PKCS7_Free(PKCS7* pkcs7);

WOLFSSL_API int wc_PKCS7_GetAttributeValue(PKCS7* pkcs7, const byte* oid,
        word32 oidSz, byte* out, word32* outSz);

WOLFSSL_API int wc_PKCS7_SetSignerIdentifierType(PKCS7* pkcs7, int type);
WOLFSSL_API int wc_PKCS7_SetContentType(PKCS7* pkcs7, byte* contentType,
                                        word32 sz);
WOLFSSL_API int wc_PKCS7_GetPadSize(word32 inputSz, word32 blockSz);
WOLFSSL_API int wc_PKCS7_PadData(byte* in, word32 inSz, byte* out, word32 outSz,
                                 word32 blockSz);

/* CMS/PKCS#7 Data */
WOLFSSL_API int  wc_PKCS7_EncodeData(PKCS7* pkcs7, byte* output,
                                       word32 outputSz);

/* CMS/PKCS#7 SignedData */
WOLFSSL_API int  wc_PKCS7_SetDetached(PKCS7* pkcs7, word16 flag);
WOLFSSL_API int  wc_PKCS7_NoDefaultSignedAttribs(PKCS7* pkcs7);
WOLFSSL_API int  wc_PKCS7_SetDefaultSignedAttribs(PKCS7* pkcs7, word16 flag);
WOLFSSL_API int  wc_PKCS7_EncodeSignedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_EncodeSignedData_ex(PKCS7* pkcs7, const byte* hashBuf,
                                          word32 hashSz, byte* outputHead,
                                          word32* outputHeadSz,
                                          byte* outputFoot,
                                          word32* outputFootSz);
WOLFSSL_API void wc_PKCS7_AllowDegenerate(PKCS7* pkcs7, word16 flag);
WOLFSSL_API int  wc_PKCS7_VerifySignedData(PKCS7* pkcs7,
                                          byte* pkiMsg, word32 pkiMsgSz);
WOLFSSL_API int  wc_PKCS7_VerifySignedData_ex(PKCS7* pkcs7, const byte* hashBuf,
                                          word32 hashSz, byte* pkiMsgHead,
                                          word32 pkiMsgHeadSz, byte* pkiMsgFoot,
                                          word32 pkiMsgFootSz);

WOLFSSL_API int  wc_PKCS7_GetSignerSID(PKCS7* pkcs7, byte* out, word32* outSz);

/* CMS single-shot API for Signed FirmwarePkgData */
WOLFSSL_API int  wc_PKCS7_EncodeSignedFPD(PKCS7* pkcs7, byte* privateKey,
                                          word32 privateKeySz, int signOID,
                                          int hashOID, byte* content,
                                          word32 contentSz,
                                          PKCS7Attrib* signedAttribs,
                                          word32 signedAttribsSz, byte* output,
                                          word32 outputSz);
#ifndef NO_PKCS7_ENCRYPTED_DATA
/* CMS single-shot API for Signed Encrypted FirmwarePkgData */
WOLFSSL_API int  wc_PKCS7_EncodeSignedEncryptedFPD(PKCS7* pkcs7,
                                          byte* encryptKey, word32 encryptKeySz,
                                          byte* privateKey, word32 privateKeySz,
                                          int encryptOID, int signOID,
                                          int hashOID, byte* content,
                                          word32 contentSz,
                                          PKCS7Attrib* unprotectedAttribs,
                                          word32 unprotectedAttribsSz,
                                          PKCS7Attrib* signedAttribs,
                                          word32 signedAttribsSz,
                                          byte* output, word32 outputSz);
#endif /* NO_PKCS7_ENCRYPTED_DATA */
#if defined(HAVE_LIBZ) && !defined(NO_PKCS7_COMPRESSED_DATA)
/* CMS single-shot API for Signed Compressed FirmwarePkgData */
WOLFSSL_API int  wc_PKCS7_EncodeSignedCompressedFPD(PKCS7* pkcs7,
                                          byte* privateKey, word32 privateKeySz,
                                          int signOID, int hashOID,
                                          byte* content, word32 contentSz,
                                          PKCS7Attrib* signedAttribs,
                                          word32 signedAttribsSz, byte* output,
                                          word32 outputSz);

#ifndef NO_PKCS7_ENCRYPTED_DATA
/* CMS single-shot API for Signed Encrypted Compressed FirmwarePkgData */
WOLFSSL_API int  wc_PKCS7_EncodeSignedEncryptedCompressedFPD(PKCS7* pkcs7,
                                          byte* encryptKey, word32 encryptKeySz,
                                          byte* privateKey, word32 privateKeySz,
                                          int encryptOID, int signOID,
                                          int hashOID, byte* content,
                                          word32 contentSz,
                                          PKCS7Attrib* unprotectedAttribs,
                                          word32 unprotectedAttribsSz,
                                          PKCS7Attrib* signedAttribs,
                                          word32 signedAttribsSz,
                                          byte* output, word32 outputSz);
#endif /* !NO_PKCS7_ENCRYPTED_DATA */
#endif /* HAVE_LIBZ && !NO_PKCS7_COMPRESSED_DATA */

/* EnvelopedData and AuthEnvelopedData RecipientInfo functions */
WOLFSSL_API int  wc_PKCS7_AddRecipient_KTRI(PKCS7* pkcs7, const byte* cert,
                                          word32 certSz, int options);
WOLFSSL_API int  wc_PKCS7_AddRecipient_KARI(PKCS7* pkcs7, const byte* cert,
                                          word32 certSz, int keyWrapOID,
                                          int keyAgreeOID, byte* ukm,
                                          word32 ukmSz, int options);

WOLFSSL_API int  wc_PKCS7_SetKey(PKCS7* pkcs7, byte* key, word32 keySz);
WOLFSSL_API int  wc_PKCS7_AddRecipient_KEKRI(PKCS7* pkcs7, int keyWrapOID,
                                          byte* kek, word32 kekSz,
                                          byte* keyID, word32 keyIdSz,
                                          void* timePtr, byte* otherOID,
                                          word32 otherOIDSz, byte* other,
                                          word32 otherSz, int options);

WOLFSSL_API int  wc_PKCS7_SetPassword(PKCS7* pkcs7, byte* passwd, word32 pLen);
WOLFSSL_API int  wc_PKCS7_AddRecipient_PWRI(PKCS7* pkcs7, byte* passwd,
                                          word32 pLen, byte* salt,
                                          word32 saltSz, int kdfOID,
                                          int prfOID, int iterations,
                                          int kekEncryptOID, int options);
WOLFSSL_API int  wc_PKCS7_SetOriEncryptCtx(PKCS7* pkcs7, void* ctx);
WOLFSSL_API int  wc_PKCS7_SetOriDecryptCtx(PKCS7* pkcs7, void* ctx);
WOLFSSL_API int  wc_PKCS7_SetOriDecryptCb(PKCS7* pkcs7, CallbackOriDecrypt cb);
WOLFSSL_API int  wc_PKCS7_AddRecipient_ORI(PKCS7* pkcs7, CallbackOriEncrypt cb,
                                           int options);
WOLFSSL_API int  wc_PKCS7_SetWrapCEKCb(PKCS7* pkcs7,
        CallbackWrapCEK wrapCEKCb);

#if defined(HAVE_PKCS7_RSA_RAW_SIGN_CALLBACK) && !defined(NO_RSA)
WOLFSSL_API int  wc_PKCS7_SetRsaSignRawDigestCb(PKCS7* pkcs7,
        CallbackRsaSignRawDigest cb);
#endif

/* CMS/PKCS#7 EnvelopedData */
WOLFSSL_API int  wc_PKCS7_EncodeEnvelopedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_DecodeEnvelopedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);

/* CMS/PKCS#7 AuthEnvelopedData */
WOLFSSL_API int  wc_PKCS7_EncodeAuthEnvelopedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_DecodeAuthEnvelopedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);

/* CMS/PKCS#7 EncryptedData */
#ifndef NO_PKCS7_ENCRYPTED_DATA
WOLFSSL_API int  wc_PKCS7_EncodeEncryptedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_DecodeEncryptedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);
WOLFSSL_API int  wc_PKCS7_SetDecodeEncryptedCb(PKCS7* pkcs7,
        CallbackDecryptContent decryptionCb);
WOLFSSL_API int  wc_PKCS7_SetDecodeEncryptedCtx(PKCS7* pkcs7, void* ctx);
#endif /* NO_PKCS7_ENCRYPTED_DATA */

/* CMS/PKCS#7 CompressedData */
#if defined(HAVE_LIBZ) && !defined(NO_PKCS7_COMPRESSED_DATA)
WOLFSSL_API int wc_PKCS7_EncodeCompressedData(PKCS7* pkcs7, byte* output,
                                              word32 outputSz);
WOLFSSL_API int wc_PKCS7_DecodeCompressedData(PKCS7* pkcs7, byte* pkiMsg,
                                              word32 pkiMsgSz, byte* output,
                                              word32 outputSz);
#endif /* HAVE_LIBZ && !NO_PKCS7_COMPRESSED_DATA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_PKCS7 */
#endif /* WOLF_CRYPT_PKCS7_H */

