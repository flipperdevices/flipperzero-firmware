/* wolfcaam.h
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

#ifndef WOLF_CRYPT_CAAM_INIT_H
#define WOLF_CRYPT_CAAM_INIT_H

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

/* include for porting layer */
#ifdef WOLFSSL_QNX_CAAM
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam_qnx.h>
#elif defined(WOLFSSL_SECO_CAAM)
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam_seco.h>
#elif defined(WOLFSSL_IMXRT1170_CAAM)
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam_fsl_nxp.h>
#endif

#if defined(WOLFSSL_IMX6_CAAM) || defined(WOLFSSL_IMX6_CAAM_RNG) || \
    defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_SECO_CAAM) || \
	defined(WOLFSSL_IMXRT1170_CAAM)


/* unique devId for CAAM use on crypto callbacks */
#ifndef WOLFSSL_CAAM_DEVID
    #define WOLFSSL_CAAM_DEVID 7
#endif

/* black key stored in secure memory location */
#define CAAM_BLACK_KEY_SM 1

/* black key encrypted with AES-CCM (has MAC) */
#define CAAM_BLACK_KEY_CCM 2

/* black key encrypted with AES-ECB (no MAC) */
#define CAAM_BLACK_KEY_ECB 3

#if defined(__INTEGRITY) || defined(INTEGRITY)
    #include <INTEGRITY.h>
    typedef Buffer CAAM_BUFFER;
#endif

WOLFSSL_LOCAL int wc_caamInit(void);
WOLFSSL_LOCAL int wc_caamFree(void);
WOLFSSL_LOCAL int wc_caamInitRng(void);
WOLFSSL_LOCAL int wc_caamFreeRng(void);

WOLFSSL_LOCAL word32 wc_caamReadRegister(word32 reg);
WOLFSSL_LOCAL void wc_caamWriteRegister(word32 reg, word32 value);
WOLFSSL_LOCAL int  wc_caamAddAndWait(CAAM_BUFFER* buf, int sz, word32 arg[4],
        word32 type);

WOLFSSL_LOCAL int caamFindUnusedPartition(void);
WOLFSSL_LOCAL CAAM_ADDRESS caamGetPartition(int part, int sz);
WOLFSSL_LOCAL int caamFreePart(int partNum);
WOLFSSL_LOCAL int caamWriteToPartition(CAAM_ADDRESS addr, const unsigned char* in, int inSz);
WOLFSSL_LOCAL int caamReadPartition(CAAM_ADDRESS addr, unsigned char* out, int outSz);

WOLFSSL_API int wc_caamOpenBlob(byte* data, word32 dataSz, byte* out,
	word32* outSz);
WOLFSSL_API int wc_caamCreateBlob(byte* data, word32 dataSz, byte* out,
	word32* outSz);

WOLFSSL_API int wc_caamOpenBlob_ex(byte* data, word32 dataSz, byte* out,
        word32* outSz, int type, byte* mod, word32 modSz);
WOLFSSL_API int wc_caamCreateBlob_ex(byte* data, word32 dataSz, byte* out,
        word32* outSz, int type, byte* mod, word32 modSz);
WOLFSSL_API int wc_caamCoverKey(byte* in, word32 inSz, byte* out, word32* outSz,
        int flag);

/* additional size that is added by CAAM when creating a blob */
#define WC_CAAM_BLOB_SZ 48
#define WC_CAAM_MAC_SZ 16
#define WC_CAAM_BLOB_RED   1
#define WC_CAAM_BLOB_BLACK 2
#define WC_CAAM_RED_KEYMOD_SZ 16
#define WC_CAAM_BLACK_KEYMOD_SZ 16
#define WC_SM_BLOB_KEYMOD_SZ 8
#define WC_CAAM_MAX_ENTROPY 44

#if !defined(WOLFSSL_QNX_CAAM) && !defined(WOLFSSL_SECO_CAAM) && \
	!defined(WOLFSSL_IMXRT1170_CAAM)
    WOLFSSL_API int wc_caamSetResource(IODevice ioDev);
    #ifndef WC_CAAM_READ
        #define WC_CAAM_READ(reg)      wc_caamReadRegister((reg))
    #endif
    #ifndef WC_CAAM_WRITE
        #define WC_CAAM_WRITE(reg, x)  wc_caamWriteRegister((reg), (x))
    #endif
#endif


#define CAAM_AESCTR 0x00100000
#define CAAM_AESCBC 0x00100100
#define CAAM_AESECB 0x00100200
#define CAAM_AESCFB 0x00100300
#define CAAM_AESOFB 0x00100400
#define CAAM_CMAC   0x00100600
#define CAAM_AESCCM 0x00100800
#define CAAM_AESGCM 0x00100900

#define CAAM_MD5    0x00400000
#define CAAM_SHA    0x00410000
#define CAAM_SHA224 0x00420000
#define CAAM_SHA256 0x00430000
#define CAAM_SHA384 0x00440000
#define CAAM_SHA512 0x00450000

#define CAAM_HMAC_MD5    0x00400010
#define CAAM_HMAC_SHA    0x00410010
#define CAAM_HMAC_SHA224 0x00420010
#define CAAM_HMAC_SHA256 0x00430010
#define CAAM_HMAC_SHA384 0x00440010
#define CAAM_HMAC_SHA512 0x00450010

#define CAAM_ECDSA_KEYPAIR 0x00140000
#define CAAM_ECDSA_SIGN    0x00150000
#define CAAM_ECDSA_VERIFY  0x00160000
#define CAAM_ECDSA_ECDH    0x00170000

#define CAAM_BLOB_ENCAP 0x07000000
#define CAAM_BLOB_DECAP 0x06000000
#define CAAM_FIFO_S   0x60000000
#define CAAM_FIFO_CCM_FLAG 0x00140000

#define CAAM_ENC    0x00000001
#define CAAM_DEC    0x00000000
#define CAAM_ALG_INIT   0x00000004
#define CAAM_ALG_INITF  0x0000000C
#define CAAM_ALG_UPDATE 0x00000000
#define CAAM_ALG_FINAL  0x00000008

/* ECDSA ECDSEL (pre defined flags for ECDSA parameters i.e. order) */
#define CAAM_ECDSEL_SHIFT 7
#define CAAM_ECDSA_PD 0x00400000
#define CAAM_ECDSA_KEYGEN_PD 0x02000000
#define CAAM_ECDSA_P192 (0x00 << CAAM_ECDSEL_SHIFT)
#define CAAM_ECDSA_P224 (0x01 << CAAM_ECDSEL_SHIFT)
#define CAAM_ECDSA_P256 (0x02 << CAAM_ECDSEL_SHIFT)
#define CAAM_ECDSA_P384 (0x03 << CAAM_ECDSEL_SHIFT)
#define CAAM_ECDSA_P521 (0x04 << CAAM_ECDSEL_SHIFT)

#define CAAM_ECDSA_BRAINPOOL_P256 (0x0B << CAAM_ECDSEL_SHIFT)

#define CAAM_ENTROPY 0x00500001

/* wolfSSL specific flags */
#define CAAM_FIND_PART 0xFFFFFFFF
#define CAAM_GET_PART 0xFFFFFFFE
#define CAAM_FREE_PART 0xFFFFFFFD
#define CAAM_READ_PART 0xFFFFFFFC
#define CAAM_WRITE_PART 0xFFFFFFFB

#endif /* WOLFSSL_IMX6_CAAM */

#endif /* WOLF_CRYPT_CAAM_INIT_H */
