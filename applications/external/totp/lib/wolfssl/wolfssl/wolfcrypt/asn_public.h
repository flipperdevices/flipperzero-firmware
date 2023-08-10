/* asn_public.h
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
    \file wolfssl/wolfcrypt/asn_public.h
*/

/*
DESCRIPTION
This library defines the interface APIs for X509 certificates.

*/
#ifndef WOLF_CRYPT_ASN_PUBLIC_H
#define WOLF_CRYPT_ASN_PUBLIC_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/dsa.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* guard on redeclaration */
#ifndef WC_ECCKEY_TYPE_DEFINED
    typedef struct ecc_key ecc_key;
    #define WC_ECCKEY_TYPE_DEFINED
#endif
#ifndef WC_ED25519KEY_TYPE_DEFINED
    typedef struct ed25519_key ed25519_key;
    #define WC_ED25519KEY_TYPE_DEFINED
#endif
#ifndef WC_CURVE25519KEY_TYPE_DEFINED
    typedef struct curve25519_key curve25519_key;
    #define WC_CURVE25519KEY_TYPE_DEFINED
#endif
#ifndef WC_ED448KEY_TYPE_DEFINED
    typedef struct ed448_key ed448_key;
    #define WC_ED448KEY_TYPE_DEFINED
#endif
#ifndef WC_CURVE448KEY_TYPE_DEFINED
    typedef struct curve448_key curve448_key;
    #define WC_CURVE448KEY_TYPE_DEFINED
#endif
#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif
#ifndef WC_DH_TYPE_DEFINED
    typedef struct DhKey DhKey;
    #define WC_DH_TYPE_DEFINED
#endif
#ifndef WC_FALCONKEY_TYPE_DEFINED
    typedef struct falcon_key falcon_key;
    #define WC_FALCONKEY_TYPE_DEFINED
#endif
#ifndef WC_DILITHIUMKEY_TYPE_DEFINED
    typedef struct dilithium_key dilithium_key;
    #define WC_DILITHIUMKEY_TYPE_DEFINED
#endif
#ifndef WC_SPHINCSKEY_TYPE_DEFINED
    typedef struct sphincs_key sphincs_key;
    #define WC_SPHINCSKEY_TYPE_DEFINED
#endif

enum Ecc_Sum {
    ECC_SECP112R1_OID = 182,
    ECC_SECP112R2_OID = 183,
    ECC_SECP128R1_OID = 204,
    ECC_SECP128R2_OID = 205,
    ECC_SECP160R1_OID = 184,
    ECC_SECP160R2_OID = 206,
    ECC_SECP160K1_OID = 185,
    ECC_BRAINPOOLP160R1_OID = 98,
    ECC_SECP192R1_OID = 520,
    ECC_PRIME192V2_OID = 521,
    ECC_PRIME192V3_OID = 522,
    ECC_SECP192K1_OID = 207,
    ECC_BRAINPOOLP192R1_OID = 100,
    ECC_SECP224R1_OID = 209,
    ECC_SECP224K1_OID = 208,
    ECC_BRAINPOOLP224R1_OID = 102,
    ECC_PRIME239V1_OID = 523,
    ECC_PRIME239V2_OID = 524,
    ECC_PRIME239V3_OID = 525,
    ECC_SECP256R1_OID = 526,
    ECC_SECP256K1_OID = 186,
    ECC_BRAINPOOLP256R1_OID = 104,
    ECC_SM2P256V1_OID = 667,
    ECC_X25519_OID = 365,
    ECC_ED25519_OID = 256,
    ECC_BRAINPOOLP320R1_OID = 106,
    ECC_X448_OID = 362,
    ECC_ED448_OID = 257,
    ECC_SECP384R1_OID = 210,
    ECC_BRAINPOOLP384R1_OID = 108,
    ECC_BRAINPOOLP512R1_OID = 110,
    ECC_SECP521R1_OID = 211
};


enum EncPkcs8Types {
    ENC_PKCS8_VER_PKCS12 = 1,
    ENC_PKCS8_VER_PKCS5 =  5,

    ENC_PKCS8_PBES2 =  13,

    ENC_PKCS8_PBE_SHA1_RC4_128   = 1,
    ENC_PKCS8_PBE_SHA1_DES       = 2,
    ENC_PKCS8_PBE_SHA1_DES3      = 3,
    ENC_PKCS8_PBE_SHA1_40RC2_CBC = 6,

    ENC_PKCS8_PBES1_MD5_DES      = 3,
    ENC_PKCS8_PBES1_SHA1_DES     = 10,

    ENC_PKCS8_ALG_AES128CBC = 414,
    ENC_PKCS8_ALG_AES256CBC = 454,
    ENC_PKCS8_ALG_DES       = 69,
    ENC_PKCS8_ALG_DES3      = 652
};

/* Certificate file Type */
enum CertType {
    CERT_TYPE       = 0,
    PRIVATEKEY_TYPE,
    DH_PARAM_TYPE,
    DSA_PARAM_TYPE,
    CRL_TYPE,
    CA_TYPE,
    ECC_PRIVATEKEY_TYPE,
    DSA_PRIVATEKEY_TYPE,
    CERTREQ_TYPE,
    DSA_TYPE,
    ECC_TYPE,
    RSA_TYPE,
    PUBLICKEY_TYPE,
    RSA_PUBLICKEY_TYPE,
    ECC_PUBLICKEY_TYPE,
    TRUSTED_PEER_TYPE,
    EDDSA_PRIVATEKEY_TYPE,
    ED25519_TYPE,
    ED448_TYPE,
    PKCS12_TYPE,
    PKCS8_PRIVATEKEY_TYPE,
    PKCS8_ENC_PRIVATEKEY_TYPE,
    DETECT_CERT_TYPE,
    DH_PRIVATEKEY_TYPE,
    X942_PARAM_TYPE,
    FALCON_LEVEL1_TYPE,
    FALCON_LEVEL5_TYPE,
    DILITHIUM_LEVEL2_TYPE,
    DILITHIUM_LEVEL3_TYPE,
    DILITHIUM_LEVEL5_TYPE,
    SPHINCS_FAST_LEVEL1_TYPE,
    SPHINCS_FAST_LEVEL3_TYPE,
    SPHINCS_FAST_LEVEL5_TYPE,
    SPHINCS_SMALL_LEVEL1_TYPE,
    SPHINCS_SMALL_LEVEL3_TYPE,
    SPHINCS_SMALL_LEVEL5_TYPE,
    ECC_PARAM_TYPE,
    CHAIN_CERT_TYPE
};


/* Signature type, by OID sum */
enum Ctc_SigType {
    CTC_SHAwDSA      = 517,
    CTC_SHA256wDSA   = 416,
    CTC_MD2wRSA      = 646,
    CTC_MD5wRSA      = 648,
    CTC_SHAwRSA      = 649,
    CTC_SHAwECDSA    = 520,
    CTC_SHA224wRSA   = 658,
    CTC_SHA224wECDSA = 523,
    CTC_SHA256wRSA   = 655,
    CTC_SHA256wECDSA = 524,
    CTC_SHA384wRSA   = 656,
    CTC_SHA384wECDSA = 525,
    CTC_SHA512wRSA   = 657,
    CTC_SHA512wECDSA = 526,

    /* https://csrc.nist.gov/projects/computer-security-objects-register/algorithm-registration */
    CTC_SHA3_224wECDSA = 423,
    CTC_SHA3_256wECDSA = 424,
    CTC_SHA3_384wECDSA = 425,
    CTC_SHA3_512wECDSA = 426,
    CTC_SHA3_224wRSA = 427,
    CTC_SHA3_256wRSA = 428,
    CTC_SHA3_384wRSA = 429,
    CTC_SHA3_512wRSA = 430,

    CTC_RSASSAPSS    = 654,

    CTC_SM3wSM2      = 740, /* 1.2.156.10197.1.501 */

    CTC_ED25519      = 256,
    CTC_ED448        = 257,

    CTC_FALCON_LEVEL1 = 268,
    CTC_FALCON_LEVEL5 = 271,

    CTC_DILITHIUM_LEVEL2     = 213,
    CTC_DILITHIUM_LEVEL3     = 216,
    CTC_DILITHIUM_LEVEL5     = 220,

    CTC_SPHINCS_FAST_LEVEL1  = 281,
    CTC_SPHINCS_FAST_LEVEL3  = 283,
    CTC_SPHINCS_FAST_LEVEL5  = 282,
    CTC_SPHINCS_SMALL_LEVEL1 = 287,
    CTC_SPHINCS_SMALL_LEVEL3 = 285,
    CTC_SPHINCS_SMALL_LEVEL5 = 286
};

enum Ctc_Encoding {
    CTC_UTF8       = 0x0c, /* utf8      */
    CTC_PRINTABLE  = 0x13  /* printable */
};

#ifndef WC_CTC_NAME_SIZE
    #define WC_CTC_NAME_SIZE 64
#endif
#ifndef WC_CTC_MAX_ALT_SIZE
    #define WC_CTC_MAX_ALT_SIZE 16384
#endif
#ifdef WOLFSSL_CERT_EXT
    #ifndef WC_CTC_MAX_CRLINFO_SZ
        #define WC_CTC_MAX_CRLINFO_SZ 200
    #endif
#endif

enum Ctc_Misc {
    CTC_COUNTRY_SIZE  =     2,
    CTC_NAME_SIZE     = WC_CTC_NAME_SIZE,
    CTC_DATE_SIZE     =    32,
    CTC_MAX_ALT_SIZE  = WC_CTC_MAX_ALT_SIZE, /* may be huge, default: 16384 */
    CTC_SERIAL_SIZE   =    20,
    CTC_GEN_SERIAL_SZ =    16,
    CTC_FILETYPE_ASN1 =     2,
    CTC_FILETYPE_PEM  =     1,
    CTC_FILETYPE_DEFAULT =  2,
#ifdef WOLFSSL_CERT_EXT
    /* AKID could contains: hash + (Option) AuthCertIssuer,AuthCertSerialNum
     * We support only hash */
    CTC_MAX_SKID_SIZE = 32, /* SHA256_DIGEST_SIZE */
    CTC_MAX_AKID_SIZE = 32, /* SHA256_DIGEST_SIZE */
    CTC_MAX_CERTPOL_SZ = 200, /* RFC 5280 Section 4.2.1.4 */
    CTC_MAX_CERTPOL_NB = 2, /* Max number of Certificate Policy */
    CTC_MAX_CRLINFO_SZ = WC_CTC_MAX_CRLINFO_SZ, /* Arbitrary size that should be
                                                 * enough for at least two
                                                 * distribution points. */
#endif /* WOLFSSL_CERT_EXT */
    WOLF_ENUM_DUMMY_LAST_ELEMENT(Ctc_Misc)
};

/* DER buffer */
typedef struct DerBuffer {
    byte*  buffer;
    void*  heap;
    word32 length;
    int    type;    /* enum CertType */
    int    dynType; /* DYNAMIC_TYPE_* */
} DerBuffer;

typedef struct WOLFSSL_ASN1_TIME {
    unsigned char data[CTC_DATE_SIZE]; /* date bytes */
    int length;
    int type;
} WOLFSSL_ASN1_TIME;

enum {
    IV_SZ   = 32,                   /* max iv sz */
#ifdef OPENSSL_ALL
    NAME_SZ = 160,                  /* larger max one line, allows for longer
                                       encryption password support */
#else
    NAME_SZ = 80,                   /* max one line */
#endif

    PEM_PASS_READ  = 0,
    PEM_PASS_WRITE = 1
};

typedef int (wc_pem_password_cb)(char* passwd, int sz, int rw, void* userdata);
#ifndef OPENSSL_COEXIST
/* In the past, wc_pem_password_cb was called pem_password_cb, which is the same
 * name as an identical typedef in OpenSSL. We don't want to break existing code
 * that uses the name pem_password_cb, so we define it here as a macro alias for
 * wc_pem_password_cb. In cases where a user needs to use both OpenSSL and
 * wolfSSL headers in the same code, they should define OPENSSL_COEXIST to
 * avoid errors stemming from the typedef being declared twice. */
#define pem_password_cb wc_pem_password_cb
#endif

typedef struct EncryptedInfo {
    long     consumed;         /* tracks PEM bytes consumed */

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wc_pem_password_cb* passwd_cb;
    void*               passwd_userdata;

    int      cipherType;
    word32   keySz;
    word32   ivSz;             /* salt or encrypted IV size */

    char     name[NAME_SZ];    /* cipher name, such as "DES-CBC" */
    byte     iv[IV_SZ];        /* salt or encrypted IV */

    word16   set:1;            /* if encryption set */
#endif
} EncryptedInfo;


#define WOLFSSL_ASN1_INTEGER_MAX 20
typedef struct WOLFSSL_ASN1_INTEGER {
    /* size can be increased set at 20 for tag, length then to hold at least 16
     * byte type */
    unsigned char  intData[WOLFSSL_ASN1_INTEGER_MAX];
    /* ASN_INTEGER | LENGTH | hex of number */
    unsigned char  negative;   /* negative number flag */

    unsigned char* data;
    unsigned int   dataMax;   /* max size of data buffer */
    unsigned int   isDynamic:1; /* flag for if data pointer dynamic (1 is yes 0 is no) */

    int length;   /* Length of DER encoding. */
    int type;     /* ASN.1 type. Includes negative flag. */
} WOLFSSL_ASN1_INTEGER;


#if defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_CERT_EXT)
#ifdef WOLFSSL_EKU_OID
    #ifndef CTC_MAX_EKU_NB
        #define CTC_MAX_EKU_NB 1
    #endif
    #ifndef CTC_MAX_EKU_OID_SZ
        #define CTC_MAX_EKU_OID_SZ 30
    #endif
#else
    #undef CTC_MAX_EKU_OID_SZ
    #define CTC_MAX_EKU_OID_SZ 0
#endif
#endif /* WOLFSSL_CERT_GEN || WOLFSSL_CERT_EXT */

#if defined(WOLFSSL_CERT_GEN) || defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
#ifdef WOLFSSL_MULTI_ATTRIB
#ifndef CTC_MAX_ATTRIB
    #define CTC_MAX_ATTRIB 4
#endif

/* ASN Encoded Name field */
typedef struct NameAttrib {
    int  sz;                     /* actual string value length */
    int  id;                     /* id of name */
    int  type;                   /* enc of name */
    char value[CTC_NAME_SIZE];   /* name */
} NameAttrib;
#endif /* WOLFSSL_MULTI_ATTRIB */
#endif /* WOLFSSL_CERT_GEN || OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef WOLFSSL_CUSTOM_OID
typedef struct CertOidField {
    byte*  oid;
    byte*  val;
    int    oidSz;
    int    valSz;
    char   enc;
} CertOidField;

typedef struct CertExtension {
    char* oid;
    byte  crit;
    byte* val;
    int   valSz;
} CertExtension;
#endif

#if defined(WOLFSSL_CERT_GEN) || defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
typedef struct CertName {
    char country[CTC_NAME_SIZE];
    char countryEnc;
    char state[CTC_NAME_SIZE];
    char stateEnc;
    char street[CTC_NAME_SIZE];
    char streetEnc;
    char locality[CTC_NAME_SIZE];
    char localityEnc;
    char sur[CTC_NAME_SIZE];
    char surEnc;
#ifdef WOLFSSL_CERT_NAME_ALL
    char givenName[CTC_NAME_SIZE];
    char givenNameEnc;
    char initials[CTC_NAME_SIZE];
    char initialsEnc;
    char dnQualifier[CTC_NAME_SIZE];
    char dnQualifierEnc;
    char dnName[CTC_NAME_SIZE];
    char dnNameEnc;
#endif /* WOLFSSL_CERT_NAME_ALL */
    char org[CTC_NAME_SIZE];
    char orgEnc;
    char unit[CTC_NAME_SIZE];
    char unitEnc;
    char commonName[CTC_NAME_SIZE];
    char commonNameEnc;
    char serialDev[CTC_NAME_SIZE];
    char serialDevEnc;
    char userId[CTC_NAME_SIZE];
    char userIdEnc;
    char postalCode[CTC_NAME_SIZE];
    char postalCodeEnc;
#ifdef WOLFSSL_CERT_EXT
    char busCat[CTC_NAME_SIZE];
    char busCatEnc;
    char joiC[CTC_NAME_SIZE];
    char joiCEnc;
    char joiSt[CTC_NAME_SIZE];
    char joiStEnc;
#endif
    char email[CTC_NAME_SIZE];  /* !!!! email has to be last !!!! */
#ifdef WOLFSSL_MULTI_ATTRIB
    NameAttrib name[CTC_MAX_ATTRIB];
#endif
#ifdef WOLFSSL_CUSTOM_OID
    CertOidField custom;
#endif
} CertName;
#endif /* WOLFSSL_CERT_GEN || OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL*/

#ifndef NUM_CUSTOM_EXT
#define NUM_CUSTOM_EXT 16
#endif /* NUM_CUSTOM_EXT */

/* for user to fill for certificate generation */
typedef struct Cert {
    int      version;                   /* x509 version  */
    byte     serial[CTC_SERIAL_SIZE];   /* serial number */
    int      serialSz;                  /* serial size */
    int      sigType;                   /* signature algo type */
#if defined(WOLFSSL_CERT_GEN) || defined(OPENSSL_EXTRA) \
 || defined(OPENSSL_EXTRA_X509_SMALL)
    CertName issuer;                    /* issuer info */
    CertName subject;                   /* subject info */
#endif /* WOLFSSL_CERT_GEN || OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */
    int      daysValid;                 /* validity days */
    int      selfSigned;                /* self signed flag */
    int      isCA;                      /* is this going to be a CA */
    byte     pathLen;                   /* max depth of valid certification
                                         * paths that include this cert */
    /* internal use only */
    int      bodySz;                    /* pre sign total size */
    int      keyType;                   /* public key type of subject */
#ifdef WOLFSSL_ALT_NAMES
    byte     altNames[CTC_MAX_ALT_SIZE]; /* altNames copy */
    int      altNamesSz;                 /* altNames size in bytes */
#endif
    byte     beforeDate[CTC_DATE_SIZE];  /* before date copy */
    int      beforeDateSz;               /* size of copy */
    byte     afterDate[CTC_DATE_SIZE];   /* after date copy */
    int      afterDateSz;                /* size of copy */
#ifdef WOLFSSL_CERT_EXT
    byte    skid[CTC_MAX_SKID_SIZE];     /* Subject Key Identifier */
    int     skidSz;                      /* SKID size in bytes */
    byte    akid[CTC_MAX_AKID_SIZE
#ifdef WOLFSSL_AKID_NAME
              + sizeof(CertName) + CTC_SERIAL_SIZE
#endif
              ]; /* Authority Key
                                                         * Identifier */
    int     akidSz;                      /* AKID size in bytes */
#ifdef WOLFSSL_AKID_NAME
    byte    rawAkid;                     /* Set to true if akid is a
                                          * AuthorityKeyIdentifier object.
                                          * Set to false if akid is just a
                                          * KeyIdentifier object. */
#endif
    word16  keyUsage;                    /* Key Usage */
    byte    extKeyUsage;                 /* Extended Key Usage */
#ifndef IGNORE_NETSCAPE_CERT_TYPE
    byte    nsCertType;                  /* Netscape Certificate Type */
#endif
#ifdef WOLFSSL_EKU_OID
    /* Extended Key Usage OIDs */
    byte    extKeyUsageOID[CTC_MAX_EKU_NB][CTC_MAX_EKU_OID_SZ];
    byte    extKeyUsageOIDSz[CTC_MAX_EKU_NB];
#endif
    char    certPolicies[CTC_MAX_CERTPOL_NB][CTC_MAX_CERTPOL_SZ];
    word16  certPoliciesNb;              /* Number of Cert Policy */
    byte    crlInfo[CTC_MAX_CRLINFO_SZ]; /* CRL Distribution points */
    int     crlInfoSz;
#endif
#if defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_CERT_REQ)
    byte     issRaw[sizeof(CertName)];   /* raw issuer info */
    byte     sbjRaw[sizeof(CertName)];   /* raw subject info */
#endif
#ifdef WOLFSSL_CERT_REQ
    char     challengePw[CTC_NAME_SIZE];
    char     unstructuredName[CTC_NAME_SIZE];
    int      challengePwPrintableString; /* encode as PrintableString */
#endif /* WOLFSSL_CERT_REQ */
#ifdef WOLFSSL_CUSTOM_OID
    /* user oid and value to go in req extensions */
    CertOidField extCustom;

    /* Extensions to go into X.509 certificates */
    CertExtension customCertExt[NUM_CUSTOM_EXT];
    int customCertExtCount;
#endif /* WOLFSSL_CUSTOM_OID */
    void*   decodedCert;      /* internal DecodedCert allocated from heap */
    byte*   der;              /* Pointer to buffer of current DecodedCert cache */
    void*   heap;             /* heap hint */
    byte    basicConstSet:1;  /* Indicator for when Basic Constraint is set */
    byte    pathLenSet:1;     /* Indicator for when path length is set */
#ifdef WOLFSSL_ALT_NAMES
    byte    altNamesCrit:1;   /* Indicator of criticality of SAN extension */
#endif
} Cert;


/* Initialize and Set Certificate defaults:
   version    = 3 (0x2)
   serial     = 0 (Will be randomly generated)
   sigType    = SHA_WITH_RSA
   issuer     = blank
   daysValid  = 500
   selfSigned = 1 (true) use subject as issuer
   subject    = blank
   isCA       = 0 (false)
   keyType    = RSA_KEY (default)
*/
WOLFSSL_ABI WOLFSSL_API int   wc_InitCert(Cert* cert);
WOLFSSL_ABI WOLFSSL_API Cert* wc_CertNew(void* heap);
WOLFSSL_ABI WOLFSSL_API void  wc_CertFree(Cert* cert);
WOLFSSL_API int wc_InitCert_ex(Cert* cert, void* heap, int devId);
WOLFSSL_API int wc_MakeCert_ex(Cert* cert, byte* derBuffer, word32 derSz,
                               int keyType, void* key, WC_RNG* rng);
WOLFSSL_ABI
WOLFSSL_API int wc_MakeCert(Cert* cert, byte* derBuffer, word32 derSz,
                            RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng);
#ifdef WOLFSSL_CERT_REQ
    WOLFSSL_API int wc_MakeCertReq_ex(Cert* cert, byte* derBuffer, word32 derSz,
                                      int keyType, void* key);
    WOLFSSL_ABI
    WOLFSSL_API int wc_MakeCertReq(Cert* cert, byte* derBuffer, word32 derSz,
                   RsaKey* rsaKey, ecc_key* eccKey);
#endif
WOLFSSL_API int wc_SignCert_ex(int requestSz, int sType, byte* buf,
                               word32 buffSz, int keyType, void* key,
                               WC_RNG* rng);
WOLFSSL_API int wc_SignCert(int requestSz, int sType, byte* buf, word32 buffSz,
                            RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng);
WOLFSSL_ABI
WOLFSSL_API int wc_MakeSelfCert(Cert* cert, byte* buf, word32 buffSz,
                                RsaKey* key, WC_RNG* rng);
WOLFSSL_ABI WOLFSSL_API int wc_SetIssuer(Cert* cert, const char* issuerFile);
WOLFSSL_ABI WOLFSSL_API int wc_SetSubject(Cert* cert, const char* subjectFile);
#ifdef WOLFSSL_ALT_NAMES
    WOLFSSL_ABI WOLFSSL_API int wc_SetAltNames(Cert* cert, const char* file);
#endif

#ifdef WOLFSSL_CERT_GEN_CACHE
WOLFSSL_ABI WOLFSSL_API void wc_SetCert_Free(Cert* cert);
#endif

WOLFSSL_ABI
WOLFSSL_API int wc_SetIssuerBuffer(Cert* cert, const byte* der, int derSz);
WOLFSSL_ABI
WOLFSSL_API int wc_SetSubjectBuffer(Cert* cert, const byte* der, int derSz);
WOLFSSL_ABI
WOLFSSL_API int wc_SetAltNamesBuffer(Cert* cert, const byte* der, int derSz);
WOLFSSL_ABI
WOLFSSL_API int wc_SetDatesBuffer(Cert* cert, const byte* der, int derSz);

#ifndef NO_ASN_TIME
WOLFSSL_API int wc_GetCertDates(Cert* cert, struct tm* before,
    struct tm* after);
#endif

#ifdef WOLFSSL_CERT_EXT
WOLFSSL_API int wc_SetAuthKeyIdFromPublicKey_ex(Cert *cert, int keyType,
                                                void* key);
WOLFSSL_API int wc_SetAuthKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey,
                                             ecc_key *eckey);
WOLFSSL_API int wc_SetAuthKeyIdFromCert(Cert *cert, const byte *der, int derSz);
WOLFSSL_API int wc_SetAuthKeyId(Cert *cert, const char* file);
WOLFSSL_API int wc_SetSubjectKeyIdFromPublicKey_ex(Cert *cert, int keyType,
                                                   void* key);
WOLFSSL_API int wc_SetSubjectKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey,
                                                ecc_key *eckey);
WOLFSSL_API int wc_SetSubjectKeyId(Cert *cert, const char* file);
WOLFSSL_ABI WOLFSSL_API int wc_GetSubjectRaw(byte **subjectRaw, Cert *cert);
WOLFSSL_ABI
WOLFSSL_API int wc_SetSubjectRaw(Cert* cert, const byte* der, int derSz);
WOLFSSL_ABI
WOLFSSL_API int wc_SetIssuerRaw(Cert* cert, const byte* der, int derSz);

/* Set the KeyUsage.
 * Value is a string separated tokens with ','. Accepted tokens are :
 * digitalSignature,nonRepudiation,contentCommitment,keyCertSign,cRLSign,
 * dataEncipherment,keyAgreement,keyEncipherment,encipherOnly and decipherOnly.
 *
 * nonRepudiation and contentCommitment are for the same usage.
 */
WOLFSSL_API int wc_SetKeyUsage(Cert *cert, const char *value);

/* Set ExtendedKeyUsage
 * Value is a string separated tokens with ','. Accepted tokens are :
 * any,serverAuth,clientAuth,codeSigning,emailProtection,timeStamping,OCSPSigning
 */
WOLFSSL_API int wc_SetExtKeyUsage(Cert *cert, const char *value);


#ifdef WOLFSSL_EKU_OID
/* Set ExtendedKeyUsage with unique OID
 * oid is expected to be in byte representation
 */
WOLFSSL_API int wc_SetExtKeyUsageOID(Cert *cert, const char *oid, word32 sz,
                                     byte idx, void* heap);
#endif /* WOLFSSL_EKU_OID */

#if defined(WOLFSSL_ASN_TEMPLATE) && defined(WOLFSSL_CUSTOM_OID) && \
    defined(HAVE_OID_ENCODING)
WOLFSSL_API int wc_SetCustomExtension(Cert *cert, int critical, const char *oid,
                                      const byte *der, word32 derSz);
#endif

#endif /* WOLFSSL_CERT_EXT */

WOLFSSL_API int wc_GetDateInfo(const byte* certDate, int certDateSz,
    const byte** date, byte* format, int* length);
#ifndef NO_ASN_TIME
WOLFSSL_API int wc_GetDateAsCalendarTime(const byte* date, int length,
    byte format, struct tm* timearg);
#endif

#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)

    WOLFSSL_API int wc_PemGetHeaderFooter(int type, const char** header,
        const char** footer);

#endif

WOLFSSL_API  int wc_AllocDer(DerBuffer** pDer, word32 length, int type, void* heap);
WOLFSSL_API void wc_FreeDer(DerBuffer** pDer);

#ifdef WOLFSSL_PEM_TO_DER
    WOLFSSL_API int wc_PemToDer(const unsigned char* buff, long longSz, int type,
              DerBuffer** pDer, void* heap, EncryptedInfo* info, int* keyFormat);

    WOLFSSL_API int wc_KeyPemToDer(const unsigned char* pem, int pemSz,
                        unsigned char* buff, int buffSz, const char* pass);
    WOLFSSL_API int wc_CertPemToDer(const unsigned char* pem, int pemSz,
                        unsigned char* buff, int buffSz, int type);
#endif /* WOLFSSL_PEM_TO_DER */

#if defined(WOLFSSL_CERT_EXT) || defined(WOLFSSL_PUB_PEM_TO_DER)
    #if !defined(NO_FILESYSTEM) && defined(WOLFSSL_PEM_TO_DER)
        WOLFSSL_API int wc_PemPubKeyToDer(const char* fileName,
                                          unsigned char* derBuf, int derSz);
        WOLFSSL_API int wc_PemPubKeyToDer_ex(const char* fileName, DerBuffer** der);
    #endif

    WOLFSSL_API int wc_PubKeyPemToDer(const unsigned char* pem, int pemSz,
                           unsigned char* buff, int buffSz);
#endif /* WOLFSSL_CERT_EXT || WOLFSSL_PUB_PEM_TO_DER */

#ifdef WOLFSSL_CERT_GEN
    #if !defined(NO_FILESYSTEM) && defined(WOLFSSL_PEM_TO_DER)
        WOLFSSL_API int wc_PemCertToDer(const char* fileName,
                                        unsigned char* derBuf, int derSz);
        WOLFSSL_API int wc_PemCertToDer_ex(const char* fileName, DerBuffer** der);
    #endif
#endif /* WOLFSSL_CERT_GEN */

#ifdef WOLFSSL_DER_TO_PEM
    WOLFSSL_ABI
    WOLFSSL_API int wc_DerToPem(const byte* der, word32 derSz, byte* output,
                                word32 outputSz, int type);
    WOLFSSL_API int wc_DerToPemEx(const byte* der, word32 derSz, byte* output,
                                word32 outputSz, byte *cipherIno, int type);
#endif

#ifndef NO_RSA
    #if !defined(HAVE_USER_RSA)
    WOLFSSL_API int wc_RsaPublicKeyDecode_ex(const byte* input, word32* inOutIdx,
        word32 inSz, const byte** n, word32* nSz, const byte** e, word32* eSz);
    /* For FIPS v1/v2 and selftest this is in rsa.h */
    #if (!defined(HAVE_SELFTEST) || \
            (defined(HAVE_SELFTEST) && defined(WOLFSSL_CERT_GEN) && \
                !defined(WOLFSSL_KEY_GEN))) && \
        (!defined(HAVE_FIPS) || \
         !defined(HAVE_FIPS_VERSION) || \
         ((HAVE_FIPS_VERSION > 2) && \
         (! ((HAVE_FIPS_VERSION == 5) && (HAVE_FIPS_VERSION_MINOR == 0)))))
    WOLFSSL_API int wc_RsaKeyToPublicDer(RsaKey* key, byte* output, word32 inLen);
    #endif
    #endif /* !HAVE_USER_RSA */
    WOLFSSL_API int wc_RsaPublicKeyDerSize(RsaKey* key, int with_header);
    WOLFSSL_API int wc_RsaKeyToPublicDer_ex(RsaKey* key, byte* output, word32 inLen,
        int with_header);

    /* For FIPS v1/v2 and selftest rsa.h is replaced. */
    #if defined(HAVE_SELFTEST) || (defined(HAVE_FIPS) && \
        (!defined(HAVE_FIPS_VERSION) || (FIPS_VERSION_LE(5,2))))
    WOLFSSL_API int wc_RsaPrivateKeyValidate(const byte* input,
        word32* inOutIdx, int* keySz, word32 inSz);
    #endif
#endif

#ifndef NO_DSA
    /* DSA parameter DER helper functions */
    WOLFSSL_API int wc_DsaParamsDecode(const byte* input, word32* inOutIdx, DsaKey* key,
                        word32 inSz);
    WOLFSSL_API int wc_DsaKeyToParamsDer(DsaKey* key, byte* output,
        word32 inLen);
    WOLFSSL_API int wc_DsaKeyToParamsDer_ex(DsaKey* key, byte* output,
        word32* inLen);
#endif

#if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
WOLFSSL_LOCAL int wc_DhKeyToDer(DhKey* key, byte* output, word32* outSz, int exportPriv);
WOLFSSL_API int wc_DhParamsToDer(DhKey* key, byte* out, word32* outSz);
WOLFSSL_API int wc_DhPubKeyToDer(DhKey* key, byte* out, word32* outSz);
WOLFSSL_API int wc_DhPrivKeyToDer(DhKey* key, byte* out, word32* outSz);
#endif

#ifdef HAVE_ECC
    /* private key helpers */
    WOLFSSL_ABI
    WOLFSSL_API int wc_EccPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                           ecc_key* key, word32 inSz);
    WOLFSSL_LOCAL int wc_BuildEccKeyDer(ecc_key* key, byte* output, word32 *inLen,
                                        int pubIn, int curveIn);
    WOLFSSL_ABI
    WOLFSSL_API int wc_EccKeyToDer(ecc_key* key, byte* output, word32 inLen);
    WOLFSSL_API int wc_EccPrivateKeyToDer(ecc_key* key, byte* output,
                                          word32 inLen);
    WOLFSSL_API int wc_EccKeyDerSize(ecc_key* key, int pub);
    WOLFSSL_API int wc_EccPrivateKeyToPKCS8(ecc_key* key, byte* output,
                                            word32* outLen);
    WOLFSSL_API int wc_EccKeyToPKCS8(ecc_key* key, byte* output,
                                     word32* outLen);

    /* public key helper */
    WOLFSSL_ABI
    WOLFSSL_API int wc_EccPublicKeyDecode(const byte* input, word32* inOutIdx,
                          ecc_key* key, word32 inSz);
    WOLFSSL_ABI WOLFSSL_API int wc_EccPublicKeyToDer(ecc_key* key, byte* output,
                                         word32 inLen, int with_AlgCurve);
    WOLFSSL_API int wc_EccPublicKeyToDer_ex(ecc_key* key, byte* output,
                                         word32 inLen, int with_AlgCurve,
                                         int comp);
    WOLFSSL_API int wc_EccPublicKeyDerSize(ecc_key* key, int with_AlgCurve);
#endif

/* RFC 5958 (Asymmetric Key Packages) */
#if !defined(WC_ENABLE_ASYM_KEY_EXPORT) && \
    ((defined(HAVE_ED25519)    && defined(HAVE_ED25519_KEY_EXPORT)) || \
     (defined(HAVE_CURVE25519) && defined(HAVE_CURVE25519_KEY_EXPORT)) || \
     (defined(HAVE_ED448)      && defined(HAVE_ED448_KEY_EXPORT)) || \
     (defined(HAVE_CURVE448)   && defined(HAVE_CURVE448_KEY_EXPORT)) || \
     (defined(HAVE_PQC) && (defined(HAVE_FALCON) || \
                            defined(HAVE_DILITHIUM) || defined(HAVE_SPHINCS))))
    #define WC_ENABLE_ASYM_KEY_EXPORT
#endif

#if !defined(WC_ENABLE_ASYM_KEY_IMPORT) && \
    ((defined(HAVE_ED25519)    && defined(HAVE_ED25519_KEY_IMPORT)) || \
     (defined(HAVE_CURVE25519) && defined(HAVE_CURVE25519_KEY_IMPORT)) || \
     (defined(HAVE_ED448)      && defined(HAVE_ED448_KEY_IMPORT)) || \
     (defined(HAVE_CURVE448)   && defined(HAVE_CURVE448_KEY_IMPORT)) || \
     (defined(HAVE_PQC) && (defined(HAVE_FALCON) || \
                            defined(HAVE_DILITHIUM) || defined(HAVE_SPHINCS))))
    #define WC_ENABLE_ASYM_KEY_IMPORT
#endif

#ifdef HAVE_ED25519
#ifdef HAVE_ED25519_KEY_IMPORT
WOLFSSL_API int wc_Ed25519PrivateKeyDecode(const byte* input, word32* inOutIdx,
                               ed25519_key* key, word32 inSz);
WOLFSSL_API int wc_Ed25519PublicKeyDecode(const byte* input, word32* inOutIdx,
                              ed25519_key* key, word32 inSz);
#endif
#ifdef HAVE_ED25519_KEY_EXPORT
WOLFSSL_API int wc_Ed25519KeyToDer(ed25519_key* key, byte* output,
                                   word32 inLen);
WOLFSSL_API int wc_Ed25519PrivateKeyToDer(ed25519_key* key, byte* output,
                                          word32 inLen);
WOLFSSL_API int wc_Ed25519PublicKeyToDer(ed25519_key* key, byte* output,
                                         word32 inLen, int withAlg);
#endif
#endif /* HAVE_ED25519 */

#ifdef HAVE_CURVE25519
#ifdef HAVE_CURVE25519_KEY_IMPORT
WOLFSSL_API int wc_Curve25519PrivateKeyDecode(
    const byte* input, word32* inOutIdx, curve25519_key* key, word32 inSz);
WOLFSSL_API int wc_Curve25519PublicKeyDecode(
    const byte* input, word32* inOutIdx, curve25519_key* key, word32 inSz);
#endif
#ifdef HAVE_CURVE25519_KEY_EXPORT
WOLFSSL_API int wc_Curve25519PrivateKeyToDer(
    curve25519_key* key, byte* output, word32 inLen);
WOLFSSL_API int wc_Curve25519PublicKeyToDer(
    curve25519_key* key, byte* output, word32 inLen, int withAlg);
#endif
#endif /* HAVE_CURVE25519 */

#ifdef HAVE_ED448
#ifdef HAVE_ED448_KEY_IMPORT
WOLFSSL_API int wc_Ed448PrivateKeyDecode(
    const byte* input, word32* inOutIdx, ed448_key* key, word32 inSz);
WOLFSSL_API int wc_Ed448PublicKeyDecode(
    const byte* input, word32* inOutIdx, ed448_key* key, word32 inSz);
#endif
#ifdef HAVE_ED448_KEY_EXPORT
WOLFSSL_API int wc_Ed448KeyToDer(ed448_key* key, byte* output, word32 inLen);
WOLFSSL_API int wc_Ed448PrivateKeyToDer(
    ed448_key* key, byte* output, word32 inLen);
WOLFSSL_API int wc_Ed448PublicKeyToDer(
    ed448_key* key, byte* output, word32 inLen, int withAlg);
#endif
#endif /* HAVE_ED448 */

#ifdef HAVE_CURVE448
#ifdef HAVE_CURVE448_KEY_IMPORT
WOLFSSL_API int wc_Curve448PrivateKeyDecode(const byte* input, word32* inOutIdx,
                               curve448_key* key, word32 inSz);
WOLFSSL_API int wc_Curve448PublicKeyDecode(const byte* input, word32* inOutIdx,
                              curve448_key* key, word32 inSz);
#endif
#ifdef HAVE_CURVE448_KEY_EXPORT
WOLFSSL_API int wc_Curve448PrivateKeyToDer(curve448_key* key, byte* output,
                                           word32 inLen);
WOLFSSL_API int wc_Curve448PublicKeyToDer(curve448_key* key, byte* output,
                                          word32 inLen, int withAlg);
#endif
#endif /* HAVE_CURVE448 */


/* DER encode signature */
WOLFSSL_API word32 wc_EncodeSignature(byte* out, const byte* digest,
                                      word32 digSz, int hashOID);
WOLFSSL_API int wc_GetCTC_HashOID(int type);

WOLFSSL_API int wc_GetPkcs8TraditionalOffset(byte* input,
                                             word32* inOutIdx, word32 sz);
WOLFSSL_API int wc_CreatePKCS8Key(byte* out, word32* outSz,
        byte* key, word32 keySz, int algoID, const byte* curveOID,
        word32 oidSz);
WOLFSSL_API int wc_EncryptPKCS8Key(byte* key, word32 keySz, byte* out, word32* outSz,
        const char* password, int passwordSz, int vPKCS, int pbeOid,
        int encAlgId, byte* salt, word32 saltSz, int itt, WC_RNG* rng,
        void* heap);
WOLFSSL_API int wc_DecryptPKCS8Key(byte* input, word32 sz, const char* password,
        int passwordSz);
WOLFSSL_API int wc_CreateEncryptedPKCS8Key(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int pbeOid, int encAlgId, byte* salt, word32 saltSz, int itt,
        WC_RNG* rng, void* heap);

#ifndef NO_ASN_TIME
/* Time */
/* Returns seconds (Epoch/UTC)
 * timePtr: is "time_t", which is typically "long"
 * Example:
    long lTime;
    rc = wc_GetTime(&lTime, (word32)sizeof(lTime));
*/
WOLFSSL_API int wc_GetTime(void* timePtr, word32 timeSize);

typedef time_t (*wc_time_cb)(time_t* t);
WOLFSSL_API int wc_SetTimeCb(wc_time_cb f);
WOLFSSL_API time_t wc_Time(time_t* t);
#endif

#ifdef WOLFSSL_ENCRYPTED_KEYS
    WOLFSSL_API int wc_EncryptedInfoGet(EncryptedInfo* info,
        const char* cipherInfo);
#endif


#ifdef WOLFSSL_CERT_PIV

typedef struct _wc_CertPIV {
    const byte*  cert;
    word32       certSz;
    const byte*  certErrDet;
    word32       certErrDetSz;
    const byte*  nonce;         /* Identiv Only */
    word32       nonceSz;       /* Identiv Only */
    const byte*  signedNonce;   /* Identiv Only */
    word32       signedNonceSz; /* Identiv Only */

    /* flags */
    word16       compression:2;
    word16       isX509:1;
    word16       isIdentiv:1;
} wc_CertPIV;

WOLFSSL_API int wc_ParseCertPIV(wc_CertPIV* cert, const byte* buf, word32 totalSz);
#endif /* WOLFSSL_CERT_PIV */

/* Forward declaration needed, as DecodedCert is defined in asn.h.*/
struct DecodedCert;

WOLFSSL_API void wc_InitDecodedCert(
    struct DecodedCert* cert, const byte* source, word32 inSz, void* heap);
WOLFSSL_API void wc_FreeDecodedCert(struct DecodedCert* cert);
WOLFSSL_API int  wc_ParseCert(
    struct DecodedCert* cert, int type, int verify, void* cm);

WOLFSSL_API int wc_GetPubKeyDerFromCert(struct DecodedCert* cert,
                                        byte* derKey, word32* derKeySz);

#ifdef WOLFSSL_FPKI
WOLFSSL_API int wc_GetUUIDFromCert(struct DecodedCert* cert,
                                    byte* uuid, word32* uuidSz);
WOLFSSL_API int wc_GetFASCNFromCert(struct DecodedCert* cert,
                                    byte* fascn, word32* fascnSz);
#endif /* WOLFSSL_FPKI */

#if !defined(XFPRINTF) || defined(NO_FILESYSTEM) || \
    defined(NO_STDIO_FILESYSTEM) && defined(WOLFSSL_ASN_PRINT)
#undef WOLFSSL_ASN_PRINT
#endif

#ifdef WOLFSSL_ASN_PRINT

enum Asn1PrintOpt {
    /* Offset into DER/BER data to start decoding from. */
    ASN1_PRINT_OPT_OFFSET,
    /* Length of DER/BER encoding to parse. */
    ASN1_PRINT_OPT_LENGTH,
    /* Number of spaces to indent for each change in depth. */
    ASN1_PRINT_OPT_INDENT,
    /* Draw branches instead of indenting. */
    ASN1_PRINT_OPT_DRAW_BRANCH,
    /* Show raw data of primitive types as octets. */
    ASN1_PRINT_OPT_SHOW_DATA,
    /* Show header data as octets. */
    ASN1_PRINT_OPT_SHOW_HEADER_DATA,
    /* Show the wolfSSL OID value for OBJECT_ID. */
    ASN1_PRINT_OPT_SHOW_OID,
    /* Don't show text representations of primitive types. */
    ASN1_PRINT_OPT_SHOW_NO_TEXT,
    /* Don't show dump text representations of primitive types. */
    ASN1_PRINT_OPT_SHOW_NO_DUMP_TEXT,
};

/* ASN.1 print options. */
typedef struct Asn1PrintOptions {
    /* Offset into DER/BER encoding to start parsing from. */
    word32 offset;
    /* Length of DER/BER encoding to parse. */
    word32 length;
    /* Number of spaces to indent for each change in depth. */
    word8 indent;
    /* Draw branches instead of indenting. */
    word8 draw_branch:1;
    /* Show raw data of primitive types as octets. */
    word8 show_data:1;
    /* Show header data as octets. */
    word8 show_header_data:1;
    /* Show the wolfSSL OID value for OBJECT_ID. */
    word8 show_oid:1;
    /* Don't show text representations of primitive types. */
    word8 show_no_text:1;
    /* Don't show dump text representations of primitive types. */
    word8 show_no_dump_text:1;
} Asn1PrintOptions;

/* ASN.1 item data. */
typedef struct Asn1Item {
    /* Tag of current item. */
    unsigned char  tag;
    /* Whether current item is constructed. */
    unsigned char  cons;
    /* Length of data in current ASN.1 item. */
    word32         len;
    /* Index into data of ASN.1 item data. */
    word32         data_idx;
} Asn1Item;

/* Maximum supported depth of ASN.1 items. */
#define ASN_MAX_DEPTH       16

/* ASN.1 parsing state. */
typedef struct Asn1 {
    /* ASN.1 item data. */
    Asn1Item         item;
    /* Current depth of ASN.1 item. */
    unsigned char    depth;
    /* End indices of ASN.1 items at different depths. */
    word32           end_idx[ASN_MAX_DEPTH];

    /* Buffer to print. */
    unsigned char*   data;
    /* Maximum number of bytes to process. */
    word32           max;
    /* Starting offset of current ASN.1 item. */
    word32           offset;
    /* Current offset into ASN.1 data. */
    word32           curr;
    /* Next part of ASN.1 item expected. */
    unsigned char    part;

    /* File pointer to print to. */
    XFILE            file;
} Asn1;

WOLFSSL_API int wc_Asn1PrintOptions_Init(Asn1PrintOptions* opts);
WOLFSSL_API int wc_Asn1PrintOptions_Set(Asn1PrintOptions* opts,
    enum Asn1PrintOpt opt, word32 val);

WOLFSSL_API int wc_Asn1_Init(Asn1* asn1);
WOLFSSL_API int wc_Asn1_SetFile(Asn1* asn1, XFILE file);
WOLFSSL_API int wc_Asn1_PrintAll(Asn1* asn1, Asn1PrintOptions* opts,
    unsigned char* data, word32 len);

#endif /* WOLFSSL_ASN_PRINT */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_ASN_PUBLIC_H */
