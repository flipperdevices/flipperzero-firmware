/* wc_pkcs11.h
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

#ifndef _WOLFPKCS11_H_
#define _WOLFPKCS11_H_

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_PKCS11

#ifndef WOLF_CRYPTO_CB
    #error PKCS11 support requires ./configure --enable-cryptocb or WOLF_CRYPTO_CB to be defined
#endif

#include <wolfssl/wolfcrypt/cryptocb.h>
#include <wolfssl/wolfcrypt/pkcs11.h>

#ifdef __cplusplus
    extern "C" {
#endif


typedef struct Pkcs11Dev {
#ifndef HAVE_PKCS11_STATIC
    void*             dlHandle;         /* Handle to library  */
#endif
    CK_FUNCTION_LIST* func;             /* Array of functions */
    void*             heap;
} Pkcs11Dev;

typedef struct Pkcs11Token {
    CK_FUNCTION_LIST* func;             /* Table of PKCS#11 function from lib */
    CK_SLOT_ID        slotId;           /* Id of slot to use                  */
    CK_SESSION_HANDLE handle;           /* Handle to active session           */
    CK_UTF8CHAR_PTR   userPin;          /* User's PIN to login with           */
    CK_ULONG          userPinSz;        /* Size of user's PIN in bytes        */
    byte              userPinLogin:1;   /* Login with User's PIN              */
} Pkcs11Token;

typedef struct Pkcs11Session {
    CK_FUNCTION_LIST* func;             /* Table of PKCS#11 function from lib */
    CK_SLOT_ID        slotId;           /* Id of slot to use                  */
    CK_SESSION_HANDLE handle;           /* Handle to active session           */
} Pkcs11Session;

/* Types of keys that can be stored. */
enum Pkcs11KeyType {
    PKCS11_KEY_TYPE_AES_GCM,
    PKCS11_KEY_TYPE_AES_CBC,
    PKCS11_KEY_TYPE_HMAC,
    PKCS11_KEY_TYPE_RSA,
    PKCS11_KEY_TYPE_EC,
};

WOLFSSL_API int wc_Pkcs11_Initialize(Pkcs11Dev* dev, const char* library,
                                     void* heap);
WOLFSSL_API int wc_Pkcs11_Initialize_ex(Pkcs11Dev* dev, const char* library,
                                        void* heap, CK_RV* rvp);
WOLFSSL_API void wc_Pkcs11_Finalize(Pkcs11Dev* dev);

WOLFSSL_API int wc_Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev,
    int slotId, const char* tokenName, const unsigned char *userPin,
    int userPinSz);
WOLFSSL_API int wc_Pkcs11Token_Init_NoLogin(Pkcs11Token* token, Pkcs11Dev* dev,
    int slotId, const char* tokenName);
WOLFSSL_API int wc_Pkcs11Token_InitName(Pkcs11Token* token, Pkcs11Dev* dev,
    const char* tokenName, int tokenSz,
    const unsigned char* userPin, int userPinSz);
WOLFSSL_API int wc_Pkcs11Token_InitName_NoLogin(Pkcs11Token* token,
    Pkcs11Dev* dev, const char* tokenName, int tokenSz);

WOLFSSL_API void wc_Pkcs11Token_Final(Pkcs11Token* token);
WOLFSSL_API int wc_Pkcs11Token_Open(Pkcs11Token* token, int readWrite);
WOLFSSL_API void wc_Pkcs11Token_Close(Pkcs11Token* token);

WOLFSSL_API int wc_Pkcs11StoreKey(Pkcs11Token* token, int type, int clear,
    void* key);

WOLFSSL_API int wc_Pkcs11_CryptoDevCb(int devId, wc_CryptoInfo* info,
    void* ctx);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_PKCS11 */

#endif /* _WOLFPKCS11_H_ */
