/* pkcs11.h
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

#ifndef _PKCS11_H_
#define _PKCS11_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL_PTR
#define NULL_PTR        0
#endif
#define CK_TRUE         1
#define CK_FALSE        0


#define CK_INVALID_HANDLE                     0UL

#define CKN_SURRENDER                         0UL

#define CKF_TOKEN_PRESENT                     0x00000001UL
#define CKF_REMOVABLE_DEVICE                  0x00000002UL
#define CKF_HW_SLOT                           0x00000004UL

#define CKF_HW                                0x00000001UL
#define CKF_ENCRYPT                           0x00000100UL
#define CKF_DECRYPT                           0x00000200UL
#define CKF_DIGEST                            0x00000400UL
#define CKF_SIGN                              0x00000800UL
#define CKF_SIGN_RECOVER                      0x00001000UL
#define CKF_VERIFY                            0x00002000UL
#define CKF_VERIFY_RECOVER                    0x00004000UL
#define CKF_GENERATE                          0x00008000UL
#define CKF_GENERATE_KEY_PAIR                 0x00010000UL
#define CKF_WRAP                              0x00020000UL
#define CKF_UNWRAP                            0x00040000UL
#define CKF_DERIVE                            0x00080000UL
#define CKF_EC_F_P                            0x00100000UL
#define CKF_EC_F_2M                           0x00200000UL
#define CKF_EC_ECPARAMETERS                   0x00400000UL
#define CKF_EC_NAMEDCURVE                     0x00800000UL
#define CKF_EC_UNCOMPRESS                     0x01000000UL
#define CKF_EC_COMPRESS                       0x02000000UL

#define CKF_LIBRARY_CANT_CREATE_OS_THREADS    0x00000001UL
#define CKF_OS_LOCKING_OK                     0x00000002UL

#define CKU_SO                                0UL
#define CKU_USER                              1UL
#define CKU_CONTEXT_SPECIFIC                  2UL

#define CKF_RW_SESSION                        0x00000002UL
#define CKF_SERIAL_SESSION                    0x00000004UL

#define CKO_PUBLIC_KEY                        0x00000002UL
#define CKO_PRIVATE_KEY                       0x00000003UL
#define CKO_SECRET_KEY                        0x00000004UL

#define CKK_RSA                               0x00000000UL
#define CKK_DH                                0x00000002UL
#define CKK_EC                                0x00000003UL
#define CKK_GENERIC_SECRET                    0x00000010UL
#define CKK_AES                               0x0000001FUL
#define CKK_MD5_HMAC                          0x00000027UL
#define CKK_SHA_1_HMAC                        0x00000028UL
#define CKK_SHA256_HMAC                       0x0000002bUL
#define CKK_SHA384_HMAC                       0x0000002cUL
#define CKK_SHA512_HMAC                       0x0000002dUL
#define CKK_SHA224_HMAC                       0x0000002eUL

#define CKA_CLASS                             0x00000000UL
#define CKA_TOKEN                             0x00000001UL
#define CKA_PRIVATE                           0x00000002UL
#define CKA_LABEL                             0x00000003UL
#define CKA_VALUE                             0x00000011UL
#define CKA_OBJECT_ID                         0x00000012UL
#define CKA_OWNER                             0x00000084UL
#define CKA_TRUSTED                           0x00000086UL
#define CKA_KEY_TYPE                          0x00000100UL
#define CKA_ID                                0x00000102UL
#define CKA_SENSITIVE                         0x00000103UL
#define CKA_ENCRYPT                           0x00000104UL
#define CKA_DECRYPT                           0x00000105UL
#define CKA_WRAP                              0x00000106UL
#define CKA_UNWRAP                            0x00000107UL
#define CKA_SIGN                              0x00000108UL
#define CKA_SIGN_RECOVER                      0x00000109UL
#define CKA_VERIFY                            0x0000010AUL
#define CKA_VERIFY_RECOVER                    0x0000010BUL
#define CKA_DERIVE                            0x0000010CUL
#define CKA_MODULUS                           0x00000120UL
#define CKA_MODULUS_BITS                      0x00000121UL
#define CKA_PUBLIC_EXPONENT                   0x00000122UL
#define CKA_PRIVATE_EXPONENT                  0x00000123UL
#define CKA_PRIME_1                           0x00000124UL
#define CKA_PRIME_2                           0x00000125UL
#define CKA_EXPONENT_1                        0x00000126UL
#define CKA_EXPONENT_2                        0x00000127UL
#define CKA_COEFFICIENT                       0x00000128UL
#define CKA_PUBLIC_KEY_INFO                   0x00000129UL
#define CKA_PRIME                             0x00000130UL
#define CKA_BASE                              0x00000132UL
#define CKA_PRIME_BITS                        0x00000133UL
#define CKA_VALUE_BITS                        0x00000160UL
#define CKA_VALUE_LEN                         0x00000161UL
#define CKA_EXTRACTABLE                       0x00000162UL
#define CKA_LOCAL                             0x00000163UL
#define CKA_NEVER_EXTRACTABLE                 0x00000164UL
#define CKA_ALWAYS_SENSITIVE                  0x00000165UL
#define CKA_KEY_GEN_MECHANISM                 0x00000166UL
#define CKA_MODIFIABLE                        0x00000170UL
#define CKA_COPYABLE                          0x00000171UL
#define CKA_DESTROYABLE                       0x00000172UL
#define CKA_EC_PARAMS                         0x00000180UL
#define CKA_EC_POINT                          0x00000181UL
#define CKA_ALWAYS_AUTHENTICATE               0x00000202UL
#define CKA_HW_FEATURE_TYPE                   0x00000300UL
#define CKA_RESET_ON_INIT                     0x00000301UL
#define CKA_HAS_RESET                         0x00000302UL

#define CKM_RSA_PKCS_KEY_PAIR_GEN             0x00000000UL
#define CKM_RSA_X_509                         0x00000003UL
#define CKM_DH_PKCS_KEY_PAIR_GEN              0x00000020UL
#define CKM_DH_PKCS_DERIVE                    0x00000021UL
#define CKM_MD5_HMAC                          0x00000211UL
#define CKM_SHA_1_HMAC                        0x00000221UL
#define CKM_SHA256_HMAC                       0x00000251UL
#define CKM_SHA224_HMAC                       0x00000256UL
#define CKM_SHA384_HMAC                       0x00000261UL
#define CKM_SHA512_HMAC                       0x00000271UL
#define CKM_GENERIC_SECRET_KEY_GEN            0x00000350UL
#define CKM_EC_KEY_PAIR_GEN                   0x00001040UL
#define CKM_ECDSA                             0x00001041UL
#define CKM_ECDH1_DERIVE                      0x00001050UL
#define CKM_ECDH1_COFACTOR_DERIVE             0x00001051UL
#define CKM_AES_KEY_GEN                       0x00001080UL
#define CKM_AES_CBC                           0x00001082UL
#define CKM_AES_GCM                           0x00001087UL

#define CKR_OK                                0x00000000UL
#define CKR_MECHANISM_INVALID                 0x00000070UL
#define CKR_SIGNATURE_INVALID                 0x000000C0UL

#define CKD_NULL                              0x00000001UL


typedef unsigned char     CK_BYTE;
typedef CK_BYTE           CK_CHAR;
typedef CK_BYTE           CK_UTF8CHAR;
typedef CK_BYTE           CK_BBOOL;
typedef unsigned long int CK_ULONG;
typedef long int          CK_LONG;
typedef CK_ULONG          CK_FLAGS;
typedef CK_BYTE*          CK_BYTE_PTR;
typedef CK_CHAR*          CK_CHAR_PTR;
typedef CK_UTF8CHAR*      CK_UTF8CHAR_PTR;
typedef CK_ULONG*         CK_ULONG_PTR;
typedef void*             CK_VOID_PTR;
typedef CK_VOID_PTR*      CK_VOID_PTR_PTR;


typedef CK_ULONG          CK_RV;


typedef struct CK_VERSION {
    CK_BYTE major;
    CK_BYTE minor;
} CK_VERSION;
typedef CK_VERSION* CK_VERSION_PTR;


/* Info Types */
typedef struct CK_INFO {
    CK_VERSION  cryptokiVersion;
    CK_UTF8CHAR manufacturerID[32];
    CK_FLAGS    flags;
    CK_UTF8CHAR libraryDescription[32];
    CK_VERSION  libraryVersion;
} CK_INFO;
typedef CK_INFO* CK_INFO_PTR;


/* Slot Types */
typedef CK_ULONG    CK_SLOT_ID;
typedef CK_SLOT_ID* CK_SLOT_ID_PTR;

typedef struct CK_SLOT_INFO {
    CK_UTF8CHAR   slotDescription[64];
    CK_UTF8CHAR   manufacturerID[32];
    CK_FLAGS      flags;

    CK_VERSION    hardwareVersion;
    CK_VERSION    firmwareVersion;
} CK_SLOT_INFO;
typedef CK_SLOT_INFO* CK_SLOT_INFO_PTR;


/* Token Types */
typedef struct CK_TOKEN_INFO {
    CK_UTF8CHAR   label[32];
    CK_UTF8CHAR   manufacturerID[32];
    CK_UTF8CHAR   model[16];
    CK_CHAR       serialNumber[16];
    CK_FLAGS      flags;
    CK_ULONG      ulMaxSessionCount;
    CK_ULONG      ulSessionCount;
    CK_ULONG      ulMaxRwSessionCount;
    CK_ULONG      ulRwSessionCount;
    CK_ULONG      ulMaxPinLen;
    CK_ULONG      ulMinPinLen;
    CK_ULONG      ulTotalPublicMemory;
    CK_ULONG      ulFreePublicMemory;
    CK_ULONG      ulTotalPrivateMemory;
    CK_ULONG      ulFreePrivateMemory;
    CK_VERSION    hardwareVersion;
    CK_VERSION    firmwareVersion;
    CK_CHAR       utcTime[16];
} CK_TOKEN_INFO;
typedef CK_TOKEN_INFO* CK_TOKEN_INFO_PTR;


/* Session Types */
typedef CK_ULONG           CK_SESSION_HANDLE;
typedef CK_SESSION_HANDLE* CK_SESSION_HANDLE_PTR;

typedef CK_ULONG          CK_USER_TYPE;

typedef CK_ULONG          CK_STATE;

typedef struct CK_SESSION_INFO {
    CK_SLOT_ID    slotID;
    CK_STATE      state;
    CK_FLAGS      flags;
    CK_ULONG      ulDeviceError;
} CK_SESSION_INFO;
typedef CK_SESSION_INFO* CK_SESSION_INFO_PTR;


/* Object Types */
typedef CK_ULONG          CK_OBJECT_HANDLE;
typedef CK_OBJECT_HANDLE* CK_OBJECT_HANDLE_PTR;

typedef CK_ULONG         CK_OBJECT_CLASS;
typedef CK_OBJECT_CLASS* CK_OBJECT_CLASS_PTR;

typedef CK_ULONG          CK_KEY_TYPE;

typedef CK_ULONG          CK_ATTRIBUTE_TYPE;

typedef struct CK_ATTRIBUTE {
    CK_ATTRIBUTE_TYPE type;
    CK_VOID_PTR       pValue;
    CK_ULONG          ulValueLen;
} CK_ATTRIBUTE;
typedef CK_ATTRIBUTE* CK_ATTRIBUTE_PTR;


/* Mechanism Types */
typedef CK_ULONG           CK_MECHANISM_TYPE;
typedef CK_MECHANISM_TYPE* CK_MECHANISM_TYPE_PTR;

typedef struct CK_MECHANISM {
    CK_MECHANISM_TYPE mechanism;
    CK_VOID_PTR       pParameter;
    CK_ULONG          ulParameterLen;
} CK_MECHANISM;
typedef CK_MECHANISM* CK_MECHANISM_PTR;

typedef struct CK_MECHANISM_INFO {
    CK_ULONG ulMinKeySize;
    CK_ULONG ulMaxKeySize;
    CK_FLAGS flags;
} CK_MECHANISM_INFO;
typedef CK_MECHANISM_INFO * CK_MECHANISM_INFO_PTR;


typedef CK_ULONG CK_NOTIFICATION;

typedef CK_RV (*CK_NOTIFY)(CK_SESSION_HANDLE hSession, CK_NOTIFICATION event,
                           CK_VOID_PTR pApplication);


/* Threading types. */
typedef CK_RV (*CK_CREATEMUTEX)(CK_VOID_PTR_PTR ppMutex);
typedef CK_RV (*CK_DESTROYMUTEX)(CK_VOID_PTR pMutex);
typedef CK_RV (*CK_LOCKMUTEX)(CK_VOID_PTR pMutex);
typedef CK_RV (*CK_UNLOCKMUTEX)(CK_VOID_PTR pMutex);

typedef struct CK_C_INITIALIZE_ARGS {
    CK_CREATEMUTEX CreateMutex;
    CK_DESTROYMUTEX DestroyMutex;
    CK_LOCKMUTEX LockMutex;
    CK_UNLOCKMUTEX UnlockMutex;
    CK_FLAGS flags;
    CK_VOID_PTR pReserved;
} CK_C_INITIALIZE_ARGS;
typedef CK_C_INITIALIZE_ARGS* CK_C_INITIALIZE_ARGS_PTR;


/* Cryptographic algorithm types. */
typedef CK_ULONG CK_EC_KDF_TYPE;

typedef struct CK_ECDH1_DERIVE_PARAMS {
    CK_EC_KDF_TYPE kdf;
    CK_ULONG ulSharedDataLen;
    CK_BYTE_PTR pSharedData;
    CK_ULONG ulPublicDataLen;
    CK_BYTE_PTR pPublicData;
} CK_ECDH1_DERIVE_PARAMS;
typedef CK_ECDH1_DERIVE_PARAMS* CK_ECDH1_DERIVE_PARAMS_PTR;


typedef struct CK_GCM_PARAMS {
    CK_BYTE_PTR       pIv;
    CK_ULONG          ulIvLen;
    CK_ULONG          ulIvBits;
    CK_BYTE_PTR       pAAD;
    CK_ULONG          ulAADLen;
    CK_ULONG          ulTagBits;
} CK_GCM_PARAMS;
typedef CK_GCM_PARAMS* CK_GCM_PARAMS_PTR;

/* Function list types. */
typedef struct CK_FUNCTION_LIST CK_FUNCTION_LIST;
typedef CK_FUNCTION_LIST* CK_FUNCTION_LIST_PTR;
typedef CK_FUNCTION_LIST_PTR* CK_FUNCTION_LIST_PTR_PTR;

typedef CK_RV (*CK_C_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

#ifdef HAVE_PKCS11_STATIC
CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);
#endif

struct CK_FUNCTION_LIST {
    CK_VERSION version;

    CK_RV (*C_Initialize)(CK_VOID_PTR pInitArgs);
    CK_RV (*C_Finalize)(CK_VOID_PTR pReserved);
    CK_RV (*C_GetInfo)(CK_INFO_PTR pInfo);
    CK_RV (*C_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);
    CK_RV (*C_GetSlotList)(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList,
                           CK_ULONG_PTR pulCount);
    CK_RV (*C_GetSlotInfo)(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo);
    CK_RV (*C_GetTokenInfo)(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo);
    CK_RV (*C_GetMechanismList)(CK_SLOT_ID slotID,
                                CK_MECHANISM_TYPE_PTR pMechanismList,
                                CK_ULONG_PTR pulCount);
    CK_RV (*C_GetMechanismInfo)(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type,
                                CK_MECHANISM_INFO_PTR pInfo);
    CK_RV (*C_InitToken)(CK_SLOT_ID slotID, CK_UTF8CHAR_PTR pPin,
                         CK_ULONG ulPinLen, CK_UTF8CHAR_PTR pLabel);
    CK_RV (*C_InitPIN)(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pPin,
                       CK_ULONG ulPinLen);
    CK_RV (*C_SetPIN)(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pOldPin,
                      CK_ULONG ulOldLen, CK_UTF8CHAR_PTR pNewPin,
                      CK_ULONG ulNewLen);
    CK_RV (*C_OpenSession)(CK_SLOT_ID slotID, CK_FLAGS flags,
                           CK_VOID_PTR pApplication, CK_NOTIFY Notify,
                           CK_SESSION_HANDLE_PTR phSession);
    CK_RV (*C_CloseSession)(CK_SESSION_HANDLE hSession);
    CK_RV (*C_CloseAllSessions)(CK_SLOT_ID slotID);
    CK_RV (*C_GetSessionInfo)(CK_SESSION_HANDLE hSession,
                              CK_SESSION_INFO_PTR pInfo);
    CK_RV (*C_GetOperationState)(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pOperationState,
                                 CK_ULONG_PTR pulOperationStateLen);
    CK_RV (*C_SetOperationState)(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pOperationState,
                                 CK_ULONG ulOperationStateLen,
                                 CK_OBJECT_HANDLE hEncryptionKey,
                                 CK_OBJECT_HANDLE hAuthenticationKey);
    CK_RV (*C_Login)(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType,
                     CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen);
    CK_RV (*C_Logout)(CK_SESSION_HANDLE hSession);
    CK_RV (*C_CreateObject)(CK_SESSION_HANDLE hSession,
                            CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR phObject);
    CK_RV (*C_CopyObject)(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject,
                          CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount,
                          CK_OBJECT_HANDLE_PTR phNewObject);
    CK_RV (*C_DestroyObject)(CK_SESSION_HANDLE hSession,
                             CK_OBJECT_HANDLE hObject);
    CK_RV (*C_GetObjectSize)(CK_SESSION_HANDLE hSession,
                             CK_OBJECT_HANDLE hObject, CK_ULONG_PTR pulSize);
    CK_RV (*C_GetAttributeValue)(CK_SESSION_HANDLE hSession,
                                 CK_OBJECT_HANDLE hObject,
                                 CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
    CK_RV (*C_SetAttributeValue)(CK_SESSION_HANDLE hSession,
                                 CK_OBJECT_HANDLE hObject,
                                 CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
    CK_RV (*C_FindObjectsInit)(CK_SESSION_HANDLE hSession,
                               CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
    CK_RV (*C_FindObjects)(CK_SESSION_HANDLE hSession,
                           CK_OBJECT_HANDLE_PTR phObject,
                           CK_ULONG ulMaxObjectCount,
                           CK_ULONG_PTR pulObjectCount);
    CK_RV (*C_FindObjectsFinal)(CK_SESSION_HANDLE hSession);
    CK_RV (*C_EncryptInit)(CK_SESSION_HANDLE hSession,
                           CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
    CK_RV (*C_Encrypt)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                       CK_ULONG ulDataLen, CK_BYTE_PTR pEncryptedData,
                       CK_ULONG_PTR pulEncryptedDataLen);
    CK_RV (*C_EncryptUpdate)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart,
                             CK_ULONG ulPartLen, CK_BYTE_PTR pEncryptedPart,
                             CK_ULONG_PTR pulEncryptedPartLen);
    CK_RV (*C_EncryptFinal)(CK_SESSION_HANDLE hSession,
                            CK_BYTE_PTR pLastEncryptedPart,
                            CK_ULONG_PTR pulLastEncryptedPartLen);
    CK_RV (*C_DecryptInit)(CK_SESSION_HANDLE hSession,
                           CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
    CK_RV (*C_Decrypt)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData,
                       CK_ULONG ulEncryptedDataLen, CK_BYTE_PTR pData,
                       CK_ULONG_PTR pulDataLen);
    CK_RV (*C_DecryptUpdate)(CK_SESSION_HANDLE hSession,
                             CK_BYTE_PTR pEncryptedPart,
                             CK_ULONG ulEncryptedPartLen, CK_BYTE_PTR pPart,
                             CK_ULONG_PTR pulPartLen);
    CK_RV (*C_DecryptFinal)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastPart,
                            CK_ULONG_PTR pulLastPartLen);
    CK_RV (*C_DigestInit)(CK_SESSION_HANDLE hSession,
                          CK_MECHANISM_PTR pMechanism);
    CK_RV (*C_Digest)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                      CK_ULONG ulDataLen, CK_BYTE_PTR pDigest,
                      CK_ULONG_PTR pulDigestLen);
    CK_RV (*C_DigestUpdate)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart,
                            CK_ULONG ulPartLen);
    CK_RV (*C_DigestKey)(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey);
    CK_RV (*C_DigestFinal)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest,
                           CK_ULONG_PTR pulDigestLen);
    CK_RV (*C_SignInit)(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                        CK_OBJECT_HANDLE hKey);
    CK_RV (*C_Sign)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                    CK_ULONG ulDataLen, CK_BYTE_PTR pSignature,
                    CK_ULONG_PTR pulSignatureLen);
    CK_RV (*C_SignUpdate)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart,
                          CK_ULONG ulPartLen);
    CK_RV (*C_SignFinal)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature,
                         CK_ULONG_PTR pulSignatureLen);
    CK_RV (*C_SignRecoverInit)(CK_SESSION_HANDLE hSession,
                               CK_MECHANISM_PTR pMechanism,
                               CK_OBJECT_HANDLE hKey);
    CK_RV (*C_SignRecover)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                           CK_ULONG ulDataLen, CK_BYTE_PTR pSignature,
                           CK_ULONG_PTR pulSignatureLen);
    CK_RV (*C_VerifyInit)(CK_SESSION_HANDLE hSession,
                          CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);
    CK_RV (*C_Verify)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                      CK_ULONG ulDataLen, CK_BYTE_PTR pSignature,
                      CK_ULONG ulSignatureLen);
    CK_RV (*C_VerifyUpdate)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart,
                            CK_ULONG ulPartLen);
    CK_RV (*C_VerifyFinal)(CK_SESSION_HANDLE hSession,
                           CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen);
    CK_RV (*C_VerifyRecoverInit)(CK_SESSION_HANDLE hSession,
                                 CK_MECHANISM_PTR pMechanism,
                                 CK_OBJECT_HANDLE hKey);
    CK_RV (*C_VerifyRecover)(CK_SESSION_HANDLE hSession,
                             CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen,
                             CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen);
    CK_RV (*C_DigestEncryptUpdate)(CK_SESSION_HANDLE hSession,
                                   CK_BYTE_PTR pPart, CK_ULONG ulPartLen,
                                   CK_BYTE_PTR pEncryptedPart,
                                   CK_ULONG_PTR pulEncryptedPartLen);
    CK_RV (*C_DecryptDigestUpdate)(CK_SESSION_HANDLE hSession,
                                   CK_BYTE_PTR pEncryptedPart,
                                   CK_ULONG ulEncryptedPartLen,
                                   CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen);
    CK_RV (*C_SignEncryptUpdate)(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pPart, CK_ULONG ulPartLen,
                                 CK_BYTE_PTR pEncryptedPart,
                                 CK_ULONG_PTR pulEncryptedPartLen);
    CK_RV (*C_DecryptVerifyUpdate)(CK_SESSION_HANDLE hSession,
                                   CK_BYTE_PTR pEncryptedPart,
                                   CK_ULONG ulEncryptedPartLen,
                                   CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen);
    CK_RV (*C_GenerateKey)(CK_SESSION_HANDLE hSession,
                           CK_MECHANISM_PTR pMechanism,
                           CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount,
                           CK_OBJECT_HANDLE_PTR phKey);
    CK_RV (*C_GenerateKeyPair)(CK_SESSION_HANDLE hSession,
                               CK_MECHANISM_PTR pMechanism,
                               CK_ATTRIBUTE_PTR pPublicKeyTemplate,
                               CK_ULONG ulPublicKeyAttributeCount,
                               CK_ATTRIBUTE_PTR pPrivateKeyTemplate,
                               CK_ULONG ulPrivateKeyAttributeCount,
                               CK_OBJECT_HANDLE_PTR phPublicKey,
                               CK_OBJECT_HANDLE_PTR phPrivateKey);
    CK_RV (*C_WrapKey)(CK_SESSION_HANDLE hSession,
                       CK_MECHANISM_PTR pMechanism,
                       CK_OBJECT_HANDLE hWrappingKey, CK_OBJECT_HANDLE hKey,
                       CK_BYTE_PTR pWrappedKey,
                       CK_ULONG_PTR pulWrappedKeyLen);
    CK_RV (*C_UnwrapKey)(CK_SESSION_HANDLE hSession,
                         CK_MECHANISM_PTR pMechanism,
                         CK_OBJECT_HANDLE hUnwrappingKey,
                         CK_BYTE_PTR pWrappedKey, CK_ULONG ulWrappedKeyLen,
                         CK_ATTRIBUTE_PTR pTemplate,
                         CK_ULONG ulAttributeCount,
                         CK_OBJECT_HANDLE_PTR phKey);
    CK_RV (*C_DeriveKey)(CK_SESSION_HANDLE hSession,
                         CK_MECHANISM_PTR pMechanism,
                         CK_OBJECT_HANDLE hBaseKey,
                         CK_ATTRIBUTE_PTR pTemplate,
                         CK_ULONG ulAttributeCount,
                         CK_OBJECT_HANDLE_PTR phKey);
    CK_RV (*C_SeedRandom)(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSeed,
                          CK_ULONG ulSeedLen);
    CK_RV (*C_GenerateRandom)(CK_SESSION_HANDLE hSession,
                              CK_BYTE_PTR RandomData, CK_ULONG ulRandomLen);
    CK_RV (*C_GetFunctionStatus)(CK_SESSION_HANDLE hSession);
    CK_RV (*C_CancelFunction)(CK_SESSION_HANDLE hSession);
    CK_RV (*C_WaitForSlotEvent)(CK_FLAGS flags, CK_SLOT_ID_PTR pSlot,
                                CK_VOID_PTR pRserved);

};

#ifdef __cplusplus
}
#endif

#endif /* _PKCS11_H_ */

