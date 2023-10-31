/* sakke.h
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
    \file wolfssl/wolfcrypt/sakke.h
*/


#ifndef WOLF_CRYPT_SAKKE_H
#define WOLF_CRYPT_SAKKE_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFCRYPT_HAVE_SAKKE

#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/hmac.h>

#define WOLFCRYPT_SAKKE_KMS
#define WOLFCRYPT_SAKKE_CLIENT

#define SAKKE_ID_MAX_SIZE       128

/* Maximum number of loops of attempting to generate a key. */
#ifndef SAKKE_MAX_GEN_COUNT
    #define SAKKE_MAX_GEN_COUNT     10
#endif


/** MP integer in projective form. */
typedef ecc_point mp_proj;

/** SAKKE ECC parameters in usable format. */
typedef struct SakkeKeyParams {
    /** Prime as an MP integer. */
    mp_int prime;
    /** Q (order) as an MP integer. */
    mp_int q;
    /** G (pairing base) as an MP integer. */
    mp_int g;
    /** Temporary MP integer used during operations. */
    mp_int a;
    /** Base point for elliptic curve operations as an ECC point. */
    ecc_point* base;

    /** Bit indicate prime is set as an MP integer in SAKKE key. */
    byte havePrime:1;
    /** Bit indicates q (order) is set as an MP integer in SAKKE key. */
    byte haveQ:1;
    /** Bit indicates g (pairing base) is set as an MP integer in SAKKE key. */
    byte haveG:1;
    /** Bit indicates a is set as an MP integer in SAKKE key. */
    byte haveA:1;
    /** Bit indicates base point is set as an ECC point in SAKKE key. */
    byte haveBase:1;
} SakkeKeyParams;

/** Temporary values to use in SAKKE calculations. */
typedef struct SakkeKeyTmp {
    /** Temporary MP integer used during operations. */
    mp_int m1;
    /** Temporary MP integer used during operations. */
    mp_int m2;

#ifdef WOLFCRYPT_SAKKE_CLIENT
    /** Temporary elliptic curve point for use in operations. */
    ecc_point* p1;
    /** Temporary elliptic curve point for use in operations. */
    ecc_point* p2;
    /** Temporary MP projective integer for use in operations. */
    mp_proj* p3;
#endif
} SakkeKeyTmp;

#ifdef WOLFCRYPT_SAKKE_CLIENT
/** SAKKE data for the intermediate point I. */
typedef struct SakkeKeyPointI {
    /** Temporary elliptic curve point for use in operations. */
    ecc_point* i;
    /** Table associated with point I. */
    byte* table;
    /** Length of table */
    word32 tableLen;
    /** Identity associated with point I. */
    byte id[SAKKE_ID_MAX_SIZE];
    /** Size of identity associated with point I. */
    word16 idSz;
} SakkeKeyPointI;

/** SAKKE data for the Receiver Secret Key (RSK). */
typedef struct SakkeKeyRsk {
    /** RSK (Receiver Secret Key). */
    ecc_point* rsk;
    /** Table associated with point I. */
    byte* table;
    /** Length of table */
    word32 tableLen;
    /** Indicates whether an RSK value has been set. */
    byte set:1;
} SakkeKeyRsk;
#endif

/**
 * SAKKE key.
 */
typedef struct SakkeKey {
    /** ECC key to perform elliptic curve operations with. */
    ecc_key ecc;

    /** ECC parameter in forms that can be used in computation. */
    SakkeKeyParams params;
    /** Temporaries used during calculations. */
    SakkeKeyTmp tmp;

#ifdef WOLFCRYPT_SAKKE_CLIENT
    /** Data relating to the RSK (Receiver Secret Key). */
    SakkeKeyRsk rsk;
    /** Identity to perform operations with. */
    byte id[SAKKE_ID_MAX_SIZE];
    /** Size of identity in bytes. */
    word16 idSz;

    /** Data relating to the intermediate point I. */
    SakkeKeyPointI i;

    /** Generic hash algorithm object. */
    wc_HashAlg hash;
    /** Temporary buffer for use in operations. */
    byte data[(MAX_ECC_BYTES * 2) + 1];
#endif

    /** Heap hint for dynamic memory allocation. */
    void* heap;

    /** Bit indicates Z, public key, is in montgomery form. */
    byte zMont:1;
    /** Bit indicate MP integers have been initialized. */
    byte mpInit:1;
} SakkeKey;

#ifdef __cplusplus
    extern "C" {
#endif

WOLFSSL_API int wc_InitSakkeKey(SakkeKey* key, void* heap, int devId);
WOLFSSL_API int wc_InitSakkeKey_ex(SakkeKey* key, int keySize, int curveId,
        void* heap, int devId);
WOLFSSL_API void wc_FreeSakkeKey(SakkeKey* key);

WOLFSSL_API int wc_MakeSakkeKey(SakkeKey* key, WC_RNG* rng);
WOLFSSL_API int wc_MakeSakkePublicKey(SakkeKey* key, ecc_point* pub);

WOLFSSL_API int wc_MakeSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk);
WOLFSSL_API int wc_ValidateSakkeRsk(SakkeKey* key, const byte* id, word16 idSz,
        ecc_point* rsk, int* valid);

WOLFSSL_API int wc_ExportSakkeKey(SakkeKey* key, byte* data, word32* sz);
WOLFSSL_API int wc_ImportSakkeKey(SakkeKey* key, const byte* data, word32 sz);
WOLFSSL_API int wc_ExportSakkePrivateKey(SakkeKey* key, byte* data, word32* sz);
WOLFSSL_API int wc_ImportSakkePrivateKey(SakkeKey* key, const byte* data,
        word32 sz);
WOLFSSL_API int wc_ExportSakkePublicKey(SakkeKey* key, byte* data,
        word32* sz, int raw);
WOLFSSL_API int wc_ImportSakkePublicKey(SakkeKey* key, const byte* data,
        word32 sz, int trusted);

WOLFSSL_API int wc_EncodeSakkeRsk(const SakkeKey* key, ecc_point* rsk,
        byte* out, word32* sz, int raw);
WOLFSSL_API int wc_DecodeSakkeRsk(const SakkeKey* key, const byte* data,
        word32 sz, ecc_point* rsk);
WOLFSSL_API int wc_ImportSakkeRsk(SakkeKey* key, const byte* data, word32 sz);

WOLFSSL_API int wc_GetSakkeAuthSize(SakkeKey* key, word16* authSz);

WOLFSSL_API int wc_SetSakkeIdentity(SakkeKey* key, const byte* id, word16 idSz);
WOLFSSL_API int wc_MakeSakkePointI(SakkeKey* key, const byte* id, word16 idSz);
WOLFSSL_API int wc_GetSakkePointI(SakkeKey* key, byte* data, word32* sz);
WOLFSSL_API int wc_SetSakkePointI(SakkeKey* key, const byte* id, word16 idSz,
        const byte* data, word32 sz);
WOLFSSL_API int wc_GenerateSakkePointITable(SakkeKey* key, byte* table,
        word32* len);
WOLFSSL_API int wc_SetSakkePointITable(SakkeKey* key, byte* table, word32 len);
WOLFSSL_API int wc_ClearSakkePointITable(SakkeKey* key);

WOLFSSL_API int wc_MakeSakkeEncapsulatedSSV(SakkeKey* key,
        enum wc_HashType hashType, byte* ssv, word16 ssvSz, byte* auth,
        word16* authSz);

WOLFSSL_API int wc_GenerateSakkeRskTable(const SakkeKey* key,
        const ecc_point* rsk, byte* table, word32* len);
WOLFSSL_API int wc_SetSakkeRsk(SakkeKey* key, const ecc_point* rsk, byte* table,
        word32 len);

WOLFSSL_API int wc_GenerateSakkeSSV(SakkeKey* key, WC_RNG* rng, byte* ssv,
        word16* ssvSz);
WOLFSSL_API int wc_DeriveSakkeSSV(SakkeKey* key, enum wc_HashType hashType,
        byte* ssv, word16 ssvSz, const byte* auth,
        word16 authSz);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFCRYPT_HAVE_SAKKE */

#endif /* WOLF_CRYPT_SAKKE_H */

