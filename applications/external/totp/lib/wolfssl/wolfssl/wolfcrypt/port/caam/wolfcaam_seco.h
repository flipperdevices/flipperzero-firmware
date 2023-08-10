/* wolfcaam_seco.h
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


/* This file is for interacting with the driver code */
#ifndef WOLFCAAM_SECO_H
#define WOLFCAAM_SECO_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_SECO_CAAM

/* unique devId for SECO use on crypto callbacks */
#ifndef WOLFSSL_SECO_DEVID
    #define WOLFSSL_SECO_DEVID 8
#endif

#define Error int
#define Value int
#define Boolean int
#define Success 1
#define Failure 0
#define INTERRUPT_Panic() WC_DO_NOTHING
#define MemoryMapMayNotBeEmpty -1
#define CAAM_WAITING -2
#define NoActivityReady -1
#define MemoryOperationNotPerformed -1

#include <stdint.h>
#define CAAM_ADDRESS intptr_t
#ifndef WOLFSSL_CAAM_BUFFER
#define WOLFSSL_CAAM_BUFFER
    typedef struct CAAM_BUFFER {
        int BufferType;
        CAAM_ADDRESS TheAddress;
        int Length;
    } CAAM_BUFFER;
#endif


#define DataBuffer 0
#define LastBuffer 0
#define Success 1

#include <wolfssl/wolfcrypt/port/caam/wolfcaam_x25519.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_ecdsa.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_cmac.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_aes.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_rsa.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_hash.h>
#include <wolfssl/wolfcrypt/cryptocb.h>

#define ResourceNotAvailable -3
#define CAAM_WAITING -2

/* key stays after key store is closed */
#define CAAM_KEY_PERSISTENT 0

/* key is deleted when key store is closed */
#define CAAM_KEY_TRANSIENT  1

/* key is used as a key encryption key */
#define CAAM_KEY_KEK        2

/* list of key types available */
#define CAAM_KEYTYPE_ECDSA_P256 0
#define CAAM_KEYTYPE_ECDSA_P384 1
#define CAAM_KEYTYPE_ECDSA_P521 2
#define CAAM_KEYTYPE_AES128     3
#define CAAM_KEYTYPE_AES192     4
#define CAAM_KEYTYPE_AES256     5
#define CAAM_KEYTYPE_HMAC224    6
#define CAAM_KEYTYPE_HMAC256    7
#define CAAM_KEYTYPE_HMAC384    8
#define CAAM_KEYTYPE_HMAC512    9

/* flags for key management */
#define CAAM_UPDATE_KEY   1
#define CAAM_GENERATE_KEY 2
#define CAAM_DELETE_KEY   4

/* flags for key store open */
#define CAAM_KEYSTORE_CREATE 1
#define CAAM_KEYSTORE_UPDATE 0

#define MAX_GROUP 1023

WOLFSSL_LOCAL int SynchronousSendRequest(int type, unsigned int args[4],
        CAAM_BUFFER *buf, int sz);
WOLFSSL_LOCAL int wc_SECOInitInterface(void);
WOLFSSL_LOCAL void wc_SECOFreeInterface(void);

WOLFSSL_API int wc_SECO_OpenHSM(word32 keyId, word32 nonce, word16 maxUpdates,
                                byte flag);
WOLFSSL_API int wc_SECO_CloseHSM(void);

WOLFSSL_API int wc_SECO_GenerateKey(int flags, int group, byte* out, int outSz,
    int keyType, int keyInfo, unsigned int* keyIdOut);
WOLFSSL_API int wc_SECO_DeleteKey(unsigned int keyId, int group, int keyTypeIn);

#if defined(WOLFSSL_CMAC)
WOLFSSL_API void wc_SECO_CMACSetKeyID(Cmac* cmac, int keyId);
WOLFSSL_API int wc_SECO_CMACGetKeyID(Cmac* cmac);
#endif

WOLFSSL_API void wc_SECO_AesSetKeyID(Aes* aes, int keyId);
WOLFSSL_API int wc_SECO_AesGetKeyID(Aes* aes);

WOLFSSL_LOCAL int wc_SECO_ExportKEK(byte* out, byte outSz, byte isCommon);
WOLFSSL_API word32 wc_SECO_WrapKey(word32 keyID, byte* in, word32 inSz,
    byte* iv, word32 ivSz, int keyType, int keyInfo, int group);

#define CAAM_SEND_REQUEST(type, sz, arg, buf) \
        SynchronousSendRequest((type), (arg), (buf), (sz))
#define CAAM_INIT_INTERFACE wc_SECOInitInterface
#define CAAM_FREE_INTERFACE wc_SECOFreeInterface
#endif

#endif /* WOLFCAAM_SECO_H */

