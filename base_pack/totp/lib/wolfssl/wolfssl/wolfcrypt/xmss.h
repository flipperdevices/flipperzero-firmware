/* xmss.h
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
    \file wolfssl/wolfcrypt/xmss.h
 */

#ifndef WOLF_CRYPT_XMSS_H
#define WOLF_CRYPT_XMSS_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef WOLFSSL_HAVE_XMSS

/* Note on XMSS/XMSS^MT pub/priv key sizes:
 *   - The XMSS/XMSS^MT pub key has a defined format and size.
 *   - The XMSS/XMSS^MT private key is implementation and parameter
 *     specific. It does not have a standardized format or size.
 *
 * The XMSS/XMSS^MT public and secret key format and length is:
 *   PK = OID || root || SEED;
 *   PK_len = 4 + 2 * n
 *
 *   SK = OID || (implementation defined)
 *   SK_len = 4 + (implementation defined)
 *
 * where n is the number of bytes in the hash function, which is 32
 * in this SHA256 implementation.
 *
 * However the private key is implementation specific. For example,
 * in xmss-reference the private key size varies from 137 bytes to
 * 1377 bytes between slow and fast implementations with param name
 * "XMSSMT-SHA2_20/2_256".
 *
 * References:
 *   - RFC 8391
 *   - Table 2 of Kampanakis, Fluhrer, IACR, 2017.
 * */

#define XMSS_SHA256_PUBLEN (68)

/* Supported XMSS/XMSS^MT parameter set names:
 * We are supporting all SHA256 parameter sets with n=32 and
 * Winternitz=16, from RFC 8391 and NIST SP 800-208.
 *
 *         ----------------------------------------------------------
 *         | Name                     OID         n   w  len  h   d  |
 * XMSS:   | "XMSS-SHA2_10_256"       0x00000001  32  16  67  10  1  |
 *         | "XMSS-SHA2_16_256"       0x00000002  32  16  67  16  1  |
 *         | "XMSS-SHA2_20_256"       0x00000003  32  16  67  20  1  |
 *         |                                                         |
 * XMSSMT: | "XMSSMT-SHA2_20/2_256"   0x00000001  32  16  67  20  2  |
 *         | "XMSSMT-SHA2_20/4_256"   0x00000002  32  16  67  20  4  |
 *         | "XMSSMT-SHA2_40/2_256"   0x00000003  32  16  67  40  2  |
 *         | "XMSSMT-SHA2_40/4_256"   0x00000004  32  16  67  40  4  |
 *         | "XMSSMT-SHA2_40/8_256"   0x00000005  32  16  67  40  8  |
 *         | "XMSSMT-SHA2_60/3_256"   0x00000006  32  16  67  60  3  |
 *         | "XMSSMT-SHA2_60/6_256"   0x00000007  32  16  67  60  6  |
 *         | "XMSSMT-SHA2_60/12_256"  0x00000008  32  16  67  60  12 |
 *         ----------------------------------------------------------
 *
 * Note that some XMSS and XMSSMT names do have overlapping OIDs.
 *
 * References:
 *   1. NIST SP 800-208
 *   2. RFC 8391
 * */

#define XMSS_NAME_LEN       (16) /* strlen("XMSS-SHA2_10_256") */
#define XMSSMT_NAME_MIN_LEN (20) /* strlen("XMSSMT-SHA2_20/2_256") */
#define XMSSMT_NAME_MAX_LEN (21) /* strlen("XMSSMT-SHA2_60/12_256") */

typedef struct XmssKey XmssKey;

/* Return codes returned by private key callbacks. */
enum wc_XmssRc {
  WC_XMSS_RC_NONE,
  WC_XMSS_RC_BAD_ARG,            /* Bad arg in read or write callback. */
  WC_XMSS_RC_WRITE_FAIL,         /* Write or update private key failed. */
  WC_XMSS_RC_READ_FAIL,          /* Read private key failed. */
  WC_XMSS_RC_SAVED_TO_NV_MEMORY, /* Wrote private key to nonvolatile storage. */
  WC_XMSS_RC_READ_TO_MEMORY      /* Read private key from storage. */
};

/* enum wc_XmssState is to help track the state of an XMSS Key. */
enum wc_XmssState {
    WC_XMSS_STATE_FREED,      /* Key has been freed from memory. */
    WC_XMSS_STATE_INITED,     /* Key has been inited, ready to set params.*/
    WC_XMSS_STATE_PARMSET,    /* Params are set, ready to MakeKey or Reload. */
    WC_XMSS_STATE_OK,         /* Able to sign signatures and verify. */
    WC_XMSS_STATE_VERIFYONLY, /* A public only XmssKey. */
    WC_XMSS_STATE_BAD,        /* Can't guarantee key's state. */
    WC_XMSS_STATE_NOSIGS      /* Signatures exhausted. */
};

/* Private key write and read callbacks. */
typedef enum wc_XmssRc (*write_private_key_cb)(const byte * priv, word32 privSz, void *context);
typedef enum wc_XmssRc (*read_private_key_cb)(byte * priv, word32 privSz, void *context);

#ifdef __cplusplus
    extern "C" {
#endif
WOLFSSL_API int  wc_XmssKey_Init(XmssKey * key, void * heap, int devId);
WOLFSSL_API int  wc_XmssKey_SetParamStr(XmssKey * key, const char * str);
#ifndef WOLFSSL_XMSS_VERIFY_ONLY
WOLFSSL_API int  wc_XmssKey_SetWriteCb(XmssKey * key,
    write_private_key_cb write_cb);
WOLFSSL_API int  wc_XmssKey_SetReadCb(XmssKey * key,
    read_private_key_cb read_cb);
WOLFSSL_API int  wc_XmssKey_SetContext(XmssKey * key, void * context);
WOLFSSL_API int  wc_XmssKey_MakeKey(XmssKey * key, WC_RNG * rng);
WOLFSSL_API int  wc_XmssKey_Reload(XmssKey * key);
WOLFSSL_API int  wc_XmssKey_GetPrivLen(const XmssKey * key, word32 * len);
WOLFSSL_API int  wc_XmssKey_Sign(XmssKey * key, byte * sig, word32 * sigSz,
    const byte * msg, int msgSz);
WOLFSSL_API int  wc_XmssKey_SigsLeft(XmssKey * key);
#endif /* ifndef WOLFSSL_XMSS_VERIFY_ONLY */
WOLFSSL_API void wc_XmssKey_Free(XmssKey * key);
WOLFSSL_API int  wc_XmssKey_GetSigLen(const XmssKey * key, word32 * len);
WOLFSSL_API int  wc_XmssKey_GetPubLen(const XmssKey * key, word32 * len);
WOLFSSL_API int  wc_XmssKey_ExportPub(XmssKey * keyDst, const XmssKey * keySrc);
WOLFSSL_API int  wc_XmssKey_ExportPubRaw(const XmssKey * key, byte * out,
    word32 * outLen);
WOLFSSL_API int  wc_XmssKey_ImportPubRaw(XmssKey * key, const byte * in,
    word32 inLen);
WOLFSSL_API int  wc_XmssKey_Verify(XmssKey * key, const byte * sig, word32 sigSz,
    const byte * msg, int msgSz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_HAVE_XMSS */
#endif /* WOLF_CRYPT_XMSS_H */
