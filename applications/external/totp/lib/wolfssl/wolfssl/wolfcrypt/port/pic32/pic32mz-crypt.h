/* pic32mz-crypt.h
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


#ifndef PIC32MZ_CRYPT_H
#define PIC32MZ_CRYPT_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_MICROCHIP_PIC32MZ

#ifndef MICROCHIP_PIC32
    #define MICROCHIP_PIC32
#endif

/* If algos aren't enabled then turn off */
#ifdef WOLFSSL_PIC32MZ_HASH
    #if defined(NO_MD5) && defined(NO_SHA) && defined(NO_SHA256)
        #undef WOLFSSL_PIC32MZ_HASH
    #endif
#endif

#ifdef WOLFSSL_PIC32MZ_CRYPT
    #if defined(NO_AES) && defined(NO_DES3)
        #undef WOLFSSL_PIC32MZ_CRYPT
    #endif
#endif

/* Enables support for large hashing */
/* requires exclusive access to crypto hardware done at application layer */
#define WOLFSSL_PIC32MZ_LARGE_HASH

#include <xc.h>
#include <sys/endian.h>
#include <sys/kmem.h>


/* PIC32 Crypto Structures */
typedef struct saCtrl {
    unsigned int CRYPTOALGO : 4;
    unsigned int MULTITASK : 3;
    unsigned int KEYSIZE : 2;
    unsigned int ENCTYPE : 1;
    unsigned int ALGO : 7;
    unsigned int : 3;
    unsigned int FLAGS : 1;
    unsigned int FB : 1;
    unsigned int LOADIV : 1;
    unsigned int LNC : 1;
    unsigned int IRFLAG : 1;
    unsigned int ICVONLY : 1;
    unsigned int OR_EN : 1;
    unsigned int NO_RX : 1;
    unsigned int : 1;
    unsigned int VERIFY : 1;
    unsigned int : 2;
} saCtrl;

typedef struct securityAssociation {
    saCtrl SA_CTRL;
    unsigned int SA_AUTHKEY[8];
    unsigned int SA_ENCKEY[8];
    unsigned int SA_AUTHIV[8];
    unsigned int SA_ENCIV[4];
} securityAssociation;

typedef struct bdCtrl {
    unsigned int BUFLEN : 16;
    unsigned int CBD_INT_EN : 1;
    unsigned int PKT_INT_EN : 1;
    unsigned int LIFM : 1;
    unsigned int LAST_BD : 1;
    unsigned int CRDMA_EN : 1;
    unsigned int UPD_RES : 1;
    unsigned int SA_FETCH_EN : 1;
    unsigned int SEC_CODE : 8;
    volatile unsigned int DESC_EN : 1;
} bdCtrl;

typedef struct bufferDescriptor {
    bdCtrl BD_CTRL;
    unsigned int SA_ADDR;
    unsigned int SRCADDR;
    unsigned int DSTADDR;
    unsigned int NXTPTR;
    unsigned int UPDPTR;
    unsigned int MSGLEN;
    unsigned int ENCOFF;
} bufferDescriptor;


/* Cache Updates Struct */
typedef struct hashUpdCache {
    unsigned char*  buf;
    unsigned int    bufLen;
    unsigned int    updLen;
    int             isCopy;
#ifdef WOLFSSL_PIC32MZ_LARGE_HASH
    unsigned int    finalLen;
#endif
} hashUpdCache;


/* Direction */
#define PIC32_ENCRYPTION     0b1
#define PIC32_DECRYPTION     0b0

/* Algorithm */
#define PIC32_ALGO_HMAC1     0b01000000
#define PIC32_ALGO_SHA256    0b00100000
#define PIC32_ALGO_SHA1      0b00010000
#define PIC32_ALGO_MD5       0b00001000

#define PIC32_ALGO_AES       0b00000100
#define PIC32_ALGO_TDES      0b00000010
#define PIC32_ALGO_DES       0b00000001

/* Crypto Algo */
/* AES */
#define PIC32_CRYPTOALGO_AES_GCM  0b1110
#define PIC32_CRYPTOALGO_RCTR     0b1101
#define PIC32_CRYPTOALGO_RCBC_MAC 0b1100
#define PIC32_CRYPTOALGO_ROFB     0b1011
#define PIC32_CRYPTOALGO_RCFB     0b1010
#define PIC32_CRYPTOALGO_RCBC     0b1001
#define PIC32_CRYPTOALGO_RECB     0b1000
/* Triple-DES */
#define PIC32_CRYPTOALGO_TOFB     0b0111
#define PIC32_CRYPTOALGO_TCFB     0b0110
#define PIC32_CRYPTOALGO_TCBC     0b0101
#define PIC32_CRYPTOALGO_TECB     0b0100
/* DES */
#define PIC32_CRYPTOALGO_OFB      0b0011
#define PIC32_CRYPTOALGO_CFB      0b0010
#define PIC32_CRYPTOALGO_CBC      0b0001
#define PIC32_CRYPTOALGO_ECB      0b0000

/* Key Size */
#define PIC32_KEYSIZE_256         0b10
#define PIC32_KEYSIZE_192         0b01
#define PIC32_KEYSIZE_128         0b00

/* PIC32 Minimum Buffer/Block Sizes */
#define PIC32_BLOCKSIZE_HASH    64
#define PIC32_BLOCKSIZE_HMAC    PIC32_BLOCKSIZE_HASH
#define PIC32_BLOCKSIZE_MD5     PIC32_BLOCKSIZE_HASH
#define PIC32_BLOCKSIZE_SHA1    PIC32_BLOCKSIZE_HASH
#define PIC32_BLOCKSIZE_SHA256  PIC32_BLOCKSIZE_HASH
#define PIC32_BLOCKSIZE_AES     16
#define PIC32_BLOCKSIZE_TDES    24
#define PIC32_BLOCKSIZE_DES     8

#define PIC32_DIGEST_SIZE       32


/* Helper Macros */
#define PIC32MZ_IF_RAM(addr) (KVA_TO_PA(addr) < 0x1D000000)

/* If EF part then Crypto engine supports swapping output bytes */
#define PIC32_NO_OUT_SWAP    ((__PIC32_FEATURE_SET0 == 'E') && \
                              (__PIC32_FEATURE_SET1 == 'C'))


#ifndef NO_AES
int wc_Pic32AesCrypt(word32 *key, int keyLen, word32 *iv, int ivLen,
        byte* out, const byte* in, word32 sz,
        int dir, int algo, int cryptoalgo);
#endif
#ifndef NO_DES3
int wc_Pic32DesCrypt(word32 *key, int keyLen, word32 *iv, int ivLen,
        byte* out, const byte* in, word32 sz,
        int dir, int algo, int cryptoalgo);
#endif

#ifdef WOLFSSL_PIC32MZ_HASH
#define WOLFSSL_NO_HASH_RAW

int wc_Pic32Hash(const byte* in, int inLen, word32* out, int outLen, int algo);
int wc_Pic32HashCopy(hashUpdCache* src, hashUpdCache* dst);

#ifndef NO_MD5
struct wc_Md5;
void wc_Md5Pic32Free(struct wc_Md5* md5);
#endif
#ifndef NO_SHA
struct wc_Sha;
void wc_ShaPic32Free(struct wc_Sha* sha);
#endif

#ifndef NO_SHA256
struct wc_Sha256;
void wc_Sha256Pic32Free(struct wc_Sha256* sha256);
#endif
#endif /* WOLFSSL_PIC32MZ_HASH */

#endif /* WOLFSSL_MICROCHIP_PIC32MZ */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* PIC32MZ_CRYPT_H */
