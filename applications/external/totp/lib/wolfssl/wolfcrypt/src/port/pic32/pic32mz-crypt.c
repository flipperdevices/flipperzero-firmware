/* pic32mz-crypt.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_MICROCHIP_PIC32MZ

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>

#ifdef WOLFSSL_PIC32MZ_CRYPT
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/des3.h>
#endif

#ifdef WOLFSSL_PIC32MZ_HASH
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#endif


#if defined(WOLFSSL_PIC32MZ_CRYPT) || defined(WOLFSSL_PIC32MZ_HASH)

static int Pic32GetBlockSize(int algo)
{
    switch (algo) {
        case PIC32_ALGO_HMAC1:
            return PIC32_BLOCKSIZE_HMAC;
        case PIC32_ALGO_SHA256:
            return PIC32_BLOCKSIZE_SHA256;
        case PIC32_ALGO_SHA1:
            return PIC32_BLOCKSIZE_SHA1;
        case PIC32_ALGO_MD5:
            return PIC32_BLOCKSIZE_MD5;
        case PIC32_ALGO_AES:
            return PIC32_BLOCKSIZE_AES;
        case PIC32_ALGO_TDES:
            return PIC32_BLOCKSIZE_TDES;
        case PIC32_ALGO_DES:
            return PIC32_BLOCKSIZE_DES;
    }
    return 0;
}

static int Pic32Crypto(const byte* pIn, int inLen, word32* pOut, int outLen,
    int dir, int algo, int cryptoalgo,

    /* For DES/AES only */
    word32* key, int keyLen, word32* iv, int ivLen)
{
    int ret = 0;
    int blockSize = Pic32GetBlockSize(algo);
    volatile bufferDescriptor bd __attribute__((aligned (8)));
    securityAssociation sa __attribute__((aligned (8)));
    securityAssociation *sa_p;
    bufferDescriptor *bd_p;
    byte *in_p;
    byte *out_p;
    unsigned int *updptrVal_p;
    word32* dst;
    word32 padRemain;
    int timeout = 0xFFFFFF;
    word32* in = (word32*)pIn;
    word32* out = pOut;
    word32 updptrVal = 0;
    int isDynamic = 0;

    /* check args */
    if (in == NULL || inLen <= 0 || out == NULL || blockSize == 0) {
        return BAD_FUNC_ARG;
    }

    /* check pointer alignment - must be word aligned */
    if (((size_t)in % sizeof(word32)) || ((size_t)out % sizeof(word32))) {
        /* dynamically allocate aligned pointers */
        isDynamic = 1;
        in = (word32*)XMALLOC(inLen, NULL, DYNAMIC_TYPE_AES_BUFFER);
        if (in == NULL)
            return MEMORY_E;
        if ((word32*)pIn == pOut) /* inline */
            out = (word32*)in;
        else {
            out = (word32*)XMALLOC(outLen, NULL, DYNAMIC_TYPE_AES_BUFFER);
            if (out == NULL) {
                XFREE(in, NULL, DYNAMIC_TYPE_AES_BUFFER);
                return MEMORY_E;
            }
        }
        XMEMCPY(in, pIn, inLen);
    }

    /* get uncached address */
    sa_p = KVA0_TO_KVA1(&sa);
    bd_p = KVA0_TO_KVA1(&bd);
    out_p= KVA0_TO_KVA1(out);
    in_p = KVA0_TO_KVA1(in);
    updptrVal_p = KVA0_TO_KVA1(&updptrVal);

    /* Sync cache if in physical memory (not flash) */
    if (PIC32MZ_IF_RAM(in_p)) {
        XMEMCPY(in_p, in, inLen);
    }

    /* Set up the Security Association */
    XMEMSET(sa_p, 0, sizeof(sa));
    sa_p->SA_CTRL.ALGO = algo;
    sa_p->SA_CTRL.ENCTYPE = dir;
    sa_p->SA_CTRL.FB = 1; /* first block */
    sa_p->SA_CTRL.LNC = 1; /* Load new set of keys */
    if (key) {
        /* cipher */
        sa_p->SA_CTRL.CRYPTOALGO = cryptoalgo;

        switch (keyLen) {
            case 32:
                sa_p->SA_CTRL.KEYSIZE = PIC32_KEYSIZE_256;
                break;
            case 24:
            case 8: /* DES */
                sa_p->SA_CTRL.KEYSIZE = PIC32_KEYSIZE_192;
                break;
            case 16:
                sa_p->SA_CTRL.KEYSIZE = PIC32_KEYSIZE_128;
                break;
        }

        dst = (word32*)KVA0_TO_KVA1(sa.SA_ENCKEY +
            (sizeof(sa.SA_ENCKEY)/sizeof(word32)) - (keyLen/sizeof(word32)));
        ByteReverseWords(dst, key, keyLen);

        if (iv && ivLen > 0) {
            sa_p->SA_CTRL.LOADIV = 1;
            dst = (word32*)KVA0_TO_KVA1(sa.SA_ENCIV +
                (sizeof(sa.SA_ENCIV)/sizeof(word32)) - (ivLen/sizeof(word32)));
            ByteReverseWords(dst, iv, ivLen);
        }
    }
    else {
        /* hashing */
        sa_p->SA_CTRL.LOADIV = 1;
        sa_p->SA_CTRL.IRFLAG = 0; /* immediate result for hashing */

        dst = (word32*)KVA0_TO_KVA1(sa.SA_AUTHIV +
            (sizeof(sa.SA_AUTHIV)/sizeof(word32)) - (outLen/sizeof(word32)));
        ByteReverseWords(dst, out, outLen);
    }

    /* Set up the Buffer Descriptor */
    XMEMSET(bd_p, 0, sizeof(bd));
    bd_p->BD_CTRL.BUFLEN = inLen;
    padRemain = (inLen % 4); /* make sure buffer is 4-byte multiple */
    if (padRemain != 0) {
        bd_p->BD_CTRL.BUFLEN += (4 - padRemain);
    }
    bd_p->BD_CTRL.SA_FETCH_EN = 1; /* Fetch the security association */
    bd_p->BD_CTRL.PKT_INT_EN = 1;  /* enable interrupt */
    bd_p->BD_CTRL.LAST_BD = 1;     /* last buffer desc in chain */
    bd_p->BD_CTRL.LIFM = 1;        /* last in frame */
    bd_p->SA_ADDR = (unsigned int)KVA_TO_PA(&sa);
    bd_p->SRCADDR = (unsigned int)KVA_TO_PA(in);
    if (key) {
        /* cipher */
        if (in != out)
            XMEMSET(out_p, 0, outLen); /* clear output buffer */
        bd_p->DSTADDR = (unsigned int)KVA_TO_PA(out);
        bd_p->UPDPTR = (unsigned int)KVA_TO_PA(updptrVal_p);
    }
    else {
        /* hashing */
        /* digest result returned in UPDPTR */
        bd_p->UPDPTR = (unsigned int)KVA_TO_PA(out);
    }
    bd_p->NXTPTR = (unsigned int)KVA_TO_PA(&bd);
    bd_p->MSGLEN = inLen;          /* actual message size */
    bd_p->BD_CTRL.DESC_EN = 1;     /* enable this descriptor */

    /* begin access to hardware */
    ret = wolfSSL_CryptHwMutexLock();
    if (ret == 0) {
        /* Software Reset the Crypto Engine */
        CECON = 1 << 6;
        while (CECON);

        /* Clear the interrupt flags */
        CEINTSRC = 0xF;

        /* Run the engine */
        CEBDPADDR = (unsigned int)KVA_TO_PA(&bd);
        CEINTEN = 0x07; /* enable DMA Packet Completion Interrupt */

        /* input swap, enable BD fetch and start DMA */
    #if PIC32_NO_OUT_SWAP
        CECON = 0x25;
    #else
        CECON = 0xa5; /* bit 7 = enable out swap */
    #endif

        /* wait for operation to complete */
        while (CEINTSRCbits.PKTIF == 0 && --timeout > 0) {};

        /* Clear the interrupt flags */
        CEINTSRC = 0xF;

        /* check for errors */
        if (CESTATbits.ERROP || timeout <= 0) {
        #if 0
            printf("PIC32 Crypto: ERROP %x, ERRPHASE %x, TIMEOUT %s\n",
                CESTATbits.ERROP, CESTATbits.ERRPHASE, timeout <= 0 ? "yes" : "no");
        #endif
            ret = ASYNC_OP_E;
        }

        wolfSSL_CryptHwMutexUnLock();

        /* copy result to output */
    #if PIC32_NO_OUT_SWAP
        /* swap bytes */
        ByteReverseWords(out, (word32*)out_p, outLen);
    #elif defined(_SYS_DEVCON_LOCAL_H)
        /* sync cache */
        SYS_DEVCON_DataCacheInvalidate((word32)out, outLen);
    #else
        XMEMCPY(out, out_p, outLen);
    #endif
    }

    /* handle unaligned */
    if (isDynamic) {
        /* return result */
        XMEMCPY(pOut, out, outLen);

        /* free dynamic buffers */
        XFREE(in, NULL, DYNAMIC_TYPE_AES_BUFFER);
        if ((word32*)pIn != pOut)
            XFREE(out, NULL, DYNAMIC_TYPE_AES_BUFFER);
    }

    return ret;
}
#endif /* WOLFSSL_PIC32MZ_CRYPT || WOLFSSL_PIC32MZ_HASH */


#ifdef WOLFSSL_PIC32MZ_HASH

#ifdef WOLFSSL_PIC32MZ_LARGE_HASH

/* tunable large hash block size */
#ifndef PIC32_BLOCK_SIZE
    #define PIC32_BLOCK_SIZE 256
#endif

#define PIC32MZ_MIN_BLOCK    64
#define PIC32MZ_MAX_BLOCK    (32*1024)

#ifndef PIC32MZ_MAX_BD
    #define PIC32MZ_MAX_BD   2
#endif

#if PIC32_BLOCK_SIZE < PIC32MZ_MIN_BLOCK
    #error Encryption block size must be at least 64 bytes.
#endif

/* Crypt Engine descriptor */
typedef struct {
    int currBd;
    int err;
    unsigned int msgSize;
    uint32_t processed;
    uint32_t dbPtr;
    int engine_ready;
    volatile bufferDescriptor   bd[PIC32MZ_MAX_BD] __attribute__((aligned (8)));
    securityAssociation         sa                 __attribute__((aligned (8)));
} pic32mz_desc;

static pic32mz_desc gLHDesc __attribute__((coherent));
static uint8_t gLHDataBuf[PIC32MZ_MAX_BD][PIC32_BLOCK_SIZE] __attribute__((aligned (4), coherent));

static void reset_engine(int algo)
{
    int i;

    wolfSSL_CryptHwMutexLock();

    /* Software reset */
    CECON = 1 << 6;
    while (CECON);

    /* Clear the interrupt flags */
    CEINTSRC = 0xF;

    /* Make sure everything is clear first before we setup */
    XMEMSET(&gLHDesc, 0, sizeof(pic32mz_desc));

    /* Set up the Security Association */
    gLHDesc.sa.SA_CTRL.ALGO = algo;
    gLHDesc.sa.SA_CTRL.LNC = 1;
    gLHDesc.sa.SA_CTRL.FB = 1;
    gLHDesc.sa.SA_CTRL.ENCTYPE = 1;
    gLHDesc.sa.SA_CTRL.LOADIV = 1;

    /* Set up the Buffer Descriptor */
    gLHDesc.err = 0;
    for (i = 0; i < PIC32MZ_MAX_BD; i++) {
        XMEMSET((void *)&gLHDesc.bd[i], 0, sizeof(gLHDesc.bd[i]));
        gLHDesc.bd[i].BD_CTRL.LAST_BD = 1;
        gLHDesc.bd[i].BD_CTRL.LIFM = 1;
        gLHDesc.bd[i].BD_CTRL.PKT_INT_EN = 1;
        gLHDesc.bd[i].SA_ADDR = KVA_TO_PA(&gLHDesc.sa);
        gLHDesc.bd[i].SRCADDR = KVA_TO_PA(&gLHDataBuf[i]);
        if (PIC32MZ_MAX_BD > i+1)
            gLHDesc.bd[i].NXTPTR = KVA_TO_PA(&gLHDesc.bd[i+1]);
        else
            gLHDesc.bd[i].NXTPTR = KVA_TO_PA(&gLHDesc.bd[0]);
        XMEMSET((void *)&gLHDataBuf[i], 0, PIC32_BLOCK_SIZE);
    }
    gLHDesc.bd[0].BD_CTRL.SA_FETCH_EN = 1; /* Fetch the security association on the first BD */
    gLHDesc.dbPtr = 0;
    gLHDesc.currBd = 0;
    gLHDesc.msgSize = 0;
    gLHDesc.processed = 0;
    CEBDPADDR = KVA_TO_PA(&(gLHDesc.bd[0]));

    CEPOLLCON = 10;

#if PIC32_NO_OUT_SWAP
    CECON = 0x27;
#else
    CECON = 0xa7;
#endif
}

static void update_engine(const byte *input, word32 len, word32 *hash)
{
    int total;
    
    gLHDesc.bd[gLHDesc.currBd].UPDPTR = KVA_TO_PA(hash);

    /* Add the data to the current buffer. If the buffer fills, start processing it
       and fill the next one. */
    while (len) {
        /* If we've been given the message size, we can process along the
           way.
           Enable the current buffer descriptor if it is full. */
        if (gLHDesc.dbPtr >= PIC32_BLOCK_SIZE) {
            /* Wrap up the buffer descriptor and enable it so the engine can process */
            gLHDesc.bd[gLHDesc.currBd].MSGLEN = gLHDesc.msgSize;
            gLHDesc.bd[gLHDesc.currBd].BD_CTRL.BUFLEN = gLHDesc.dbPtr;
            gLHDesc.bd[gLHDesc.currBd].BD_CTRL.LAST_BD = 0;
            gLHDesc.bd[gLHDesc.currBd].BD_CTRL.LIFM = 0;
            gLHDesc.bd[gLHDesc.currBd].BD_CTRL.DESC_EN = 1;
            /* Move to the next buffer descriptor, or wrap around. */
            gLHDesc.currBd++;
            if (gLHDesc.currBd >= PIC32MZ_MAX_BD)
                gLHDesc.currBd = 0;
            /* Wait until the engine has processed the new BD. */
            while (gLHDesc.bd[gLHDesc.currBd].BD_CTRL.DESC_EN);
            gLHDesc.bd[gLHDesc.currBd].UPDPTR = KVA_TO_PA(hash);
            gLHDesc.dbPtr = 0;
        }
        if (!PIC32MZ_IF_RAM(input)) {
            /* If we're inputting from flash, let the BD have
               the address and max the buffer size */
            gLHDesc.bd[gLHDesc.currBd].SRCADDR = KVA_TO_PA(input);
            total = (len > PIC32MZ_MAX_BLOCK ? PIC32MZ_MAX_BLOCK : len);
            gLHDesc.dbPtr = total;
            len -= total;
            input += total;
        }
        else {
            if (len > PIC32_BLOCK_SIZE - gLHDesc.dbPtr) {
                /* We have more data than can be put in the buffer. Fill what we can.*/
                total = PIC32_BLOCK_SIZE - gLHDesc.dbPtr;
                XMEMCPY(&gLHDataBuf[gLHDesc.currBd][gLHDesc.dbPtr], input, total);
                len -= total;
                gLHDesc.dbPtr = PIC32_BLOCK_SIZE;
                input += total;
            }
            else {
                /* Fill up what we have, but don't turn on the engine.*/
                XMEMCPY(&gLHDataBuf[gLHDesc.currBd][gLHDesc.dbPtr], input, len);
                gLHDesc.dbPtr += len;
                len = 0;
            }
        }
    }
}

static void start_engine(void)
{
    /* Wrap up the last buffer descriptor and enable it */
    int bufferLen;

    bufferLen = gLHDesc.dbPtr;
    if (bufferLen % 4)
        bufferLen = (bufferLen + 4) - (bufferLen % 4);
    /* initialize the MSGLEN on engine startup to avoid infinite loop when
     * length is less than 257 (size of PIC32_BLOCK_SIZE) */
    gLHDesc.bd[gLHDesc.currBd].MSGLEN = gLHDesc.msgSize;
    gLHDesc.bd[gLHDesc.currBd].BD_CTRL.BUFLEN = bufferLen;
    gLHDesc.bd[gLHDesc.currBd].BD_CTRL.LAST_BD = 1;
    gLHDesc.bd[gLHDesc.currBd].BD_CTRL.LIFM = 1;
    gLHDesc.bd[gLHDesc.currBd].BD_CTRL.DESC_EN = 1;
}

void wait_engine(char *hash, int hash_sz)
{
    int i;
    unsigned int engineRunning;

    do {
        engineRunning = 0;
        for (i = 0; i < PIC32MZ_MAX_BD; i++) {
            engineRunning = engineRunning || gLHDesc.bd[i].BD_CTRL.DESC_EN;
        }
    } while (engineRunning);

#if PIC32_NO_OUT_SWAP
    /* swap bytes */
    ByteReverseWords(hash, KVA0_TO_KVA1(hash), hash_sz);
#else
    /* copy output - hardware already swapped */
    XMEMCPY(hash, KVA0_TO_KVA1(hash), hash_sz);
#endif

    wolfSSL_CryptHwMutexUnLock();
}

#endif /* WOLFSSL_PIC32MZ_LARGE_HASH */

int wc_Pic32Hash(const byte* in, int inLen, word32* out, int outLen, int algo)
{
    return Pic32Crypto(in, inLen, out, outLen, PIC32_ENCRYPTION, algo, 0,
        NULL, 0, NULL, 0);
}

int wc_Pic32HashCopy(hashUpdCache* src, hashUpdCache* dst)
{
    /* mark destination as copy, so cache->buf is not free'd */
    if (dst) {
        dst->isCopy = 1;
    }
    return 0;
}

static int wc_Pic32HashUpdate(hashUpdCache* cache, byte* stdBuf, int stdBufLen,
    word32* digest, int digestSz, const byte* data, int len, int algo, void* heap)
{
    int ret = 0;
    word32 newLenUpd, newLenPad, padRemain;
    byte* newBuf;
    int isNewBuf = 0;

#ifdef WOLFSSL_PIC32MZ_LARGE_HASH
    /* if final length is set then pass straight to hardware */
    if (cache->finalLen) {
        if (cache->bufLen == 0) {
            reset_engine(algo);
            gLHDesc.msgSize = cache->finalLen;
        }
        update_engine(data, len, digest);
        cache->bufLen += len; /* track progress for blockType */
        return 0;
    }
#endif

    /* cache updates */
    /* calculate new len */
    newLenUpd = cache->updLen + len;

    /* calculate padded len - pad buffer at 64-bytes for hardware */
    newLenPad = newLenUpd;
    padRemain = (newLenUpd % PIC32_BLOCKSIZE_HASH);
    if (padRemain != 0) {
        newLenPad += (PIC32_BLOCKSIZE_HASH - padRemain);
    }

    /* determine buffer source */
    if (newLenPad <= stdBufLen) {
        /* use standard buffer */
        newBuf = stdBuf;
    }
    else if (newLenPad > cache->bufLen) {
        /* alloc buffer */
        newBuf = (byte*)XMALLOC(newLenPad, heap, DYNAMIC_TYPE_HASH_TMP);
        if (newBuf == NULL) {
            if (cache->buf != stdBuf && !cache->isCopy) {
                XFREE(cache->buf, heap, DYNAMIC_TYPE_HASH_TMP);
                cache->buf = NULL;
                cache->updLen = cache->bufLen = 0;
            }
            return MEMORY_E;
        }
        isNewBuf = 1;
        cache->isCopy = 0; /* no longer using copy buffer */
    }
    else {
        /* use existing buffer */
        newBuf = cache->buf;
    }
    if (cache->buf && cache->updLen > 0) {
        XMEMCPY(newBuf, cache->buf, cache->updLen);
        if (isNewBuf && cache->buf != stdBuf) {
            XFREE(cache->buf, heap, DYNAMIC_TYPE_HASH_TMP);
            cache->buf = NULL;
        }
    }
    XMEMCPY(newBuf + cache->updLen, data, len);

    cache->buf = newBuf;
    cache->updLen = newLenUpd;
    cache->bufLen = newLenPad;

    return ret;
}

static int wc_Pic32HashFinal(hashUpdCache* cache, byte* stdBuf,
    word32* digest, byte* hash, int digestSz, int algo, void* heap)
{
    int ret = 0;

    /* if room add the pad */
    if (cache->buf && cache->updLen < cache->bufLen) {
        cache->buf[cache->updLen] = 0x80;
    }

#ifdef WOLFSSL_PIC32MZ_LARGE_HASH
    if (cache->finalLen) {
        /* Only submit to hardware if update data provided matches expected */
        if (cache->bufLen == cache->finalLen) {
            start_engine();
            wait_engine((char*)digest, digestSz);
            XMEMCPY(hash, digest, digestSz);
        }
        else {
            wolfSSL_CryptHwMutexUnLock();
            ret = BUFFER_E;
        }
        cache->finalLen = 0;
    }
    else
#endif
    {
        if (cache->updLen == 0) {
            /* handle empty input */
            switch (algo) {
                case PIC32_ALGO_SHA256: {
                    const char* sha256EmptyHash =
                        "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9"
                        "\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52"
                        "\xb8\x55";
                    XMEMCPY(hash, sha256EmptyHash, digestSz);
                    break;
                }
                case PIC32_ALGO_SHA1: {
                    const char* shaEmptyHash =
                        "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18"
                        "\x90\xaf\xd8\x07\x09";
                    XMEMCPY(hash, shaEmptyHash, digestSz);
                    break;
                }
                case PIC32_ALGO_MD5: {
                    const char* md5EmptyHash =
                        "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42"
                        "\x7e";
                    XMEMCPY(hash, md5EmptyHash, digestSz);
                    break;
                }
            } /* switch */
        }
        else {
            ret = wc_Pic32Hash(cache->buf, cache->updLen, digest, digestSz, algo);
            if (ret == 0) {
                XMEMCPY(hash, digest, digestSz);
            }
        }

        if (cache->buf && cache->buf != stdBuf && !cache->isCopy) {
            XFREE(cache->buf, heap, DYNAMIC_TYPE_HASH_TMP);
            cache->buf = NULL;
        }
    }

    cache->buf = NULL;
    cache->bufLen = cache->updLen = 0;

    return ret;
}

static void wc_Pic32HashFree(hashUpdCache* cache, void* heap)
{
    if (cache && cache->buf && !cache->isCopy) {
        XFREE(cache->buf, heap, DYNAMIC_TYPE_HASH_TMP);
        cache->buf = NULL;
    }
}

/* API's for compatibility with Harmony wrappers - not used */
#ifndef NO_MD5
    int wc_InitMd5_ex(wc_Md5* md5, void* heap, int devId)
    {
        if (md5 == NULL)
            return BAD_FUNC_ARG;

        XMEMSET(md5, 0, sizeof(wc_Md5));
        md5->heap = heap;
        (void)devId;
        return 0;
    }
    int wc_Md5Update(wc_Md5* md5, const byte* data, word32 len)
    {
        if (md5 == NULL || (data == NULL && len > 0))
            return BAD_FUNC_ARG;
        return wc_Pic32HashUpdate(&md5->cache, (byte*)md5->buffer,
            sizeof(md5->buffer), md5->digest, MD5_DIGEST_SIZE,
            data, len, PIC32_ALGO_MD5, md5->heap);
    }
    int wc_Md5Final(wc_Md5* md5, byte* hash)
    {
        int ret;

        if (md5 == NULL || hash == NULL)
            return BAD_FUNC_ARG;

        ret = wc_Pic32HashFinal(&md5->cache, (byte*)md5->buffer,
            md5->digest, hash, MD5_DIGEST_SIZE,
            PIC32_ALGO_MD5, md5->heap);

        wc_InitMd5_ex(md5, md5->heap, INVALID_DEVID);  /* reset state */

        return ret;
    }
    void wc_Md5SizeSet(wc_Md5* md5, word32 len)
    {
        if (md5) {
        #ifdef WOLFSSL_PIC32MZ_LARGE_HASH
            md5->cache.finalLen = len;
        #else
            (void)len;
        #endif
        }
    }
    void wc_Md5Pic32Free(wc_Md5* md5)
    {
        if (md5) {
            wc_Pic32HashFree(&md5->cache, md5->heap);
        }
    }
#endif /* !NO_MD5 */
#ifndef NO_SHA
    int wc_InitSha_ex(wc_Sha* sha, void* heap, int devId)
    {
        if (sha == NULL)
            return BAD_FUNC_ARG;

        XMEMSET(sha, 0, sizeof(wc_Sha));
        sha->heap = heap;
        (void)devId;
        return 0;
    }
    int wc_ShaUpdate(wc_Sha* sha, const byte* data, word32 len)
    {
        if (sha == NULL || (data == NULL && len > 0))
            return BAD_FUNC_ARG;
        return wc_Pic32HashUpdate(&sha->cache, (byte*)sha->buffer,
            sizeof(sha->buffer), sha->digest, SHA_DIGEST_SIZE,
            data, len, PIC32_ALGO_SHA1, sha->heap);
    }
    int wc_ShaFinal(wc_Sha* sha, byte* hash)
    {
        int ret;

        if (sha == NULL || hash == NULL)
            return BAD_FUNC_ARG;

        ret = wc_Pic32HashFinal(&sha->cache, (byte*)sha->buffer,
            sha->digest, hash, SHA_DIGEST_SIZE,
            PIC32_ALGO_SHA1, sha->heap);

        wc_InitSha_ex(sha, sha->heap, INVALID_DEVID);  /* reset state */

        return ret;
    }
    void wc_ShaSizeSet(wc_Sha* sha, word32 len)
    {
        if (sha) {
        #ifdef WOLFSSL_PIC32MZ_LARGE_HASH
            sha->cache.finalLen = len;
        #else
            (void)len;
        #endif
        }
    }
    void wc_ShaPic32Free(wc_Sha* sha)
    {
        if (sha) {
            wc_Pic32HashFree(&sha->cache, sha->heap);
        }
    }
#endif /* !NO_SHA */
#ifndef NO_SHA256
    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        XMEMSET(sha256, 0, sizeof(wc_Sha256));
        sha256->heap = heap;
        (void)devId;
        return 0;
    }
    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        if (sha256 == NULL || (data == NULL && len > 0))
            return BAD_FUNC_ARG;
        return wc_Pic32HashUpdate(&sha256->cache, (byte*)sha256->buffer,
            sizeof(sha256->buffer), sha256->digest, SHA256_DIGEST_SIZE,
            data, len, PIC32_ALGO_SHA256, sha256->heap);
    }
    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret;

        if (sha256 == NULL || hash == NULL)
            return BAD_FUNC_ARG;

        ret = wc_Pic32HashFinal(&sha256->cache, (byte*)sha256->buffer,
            sha256->digest, hash, SHA256_DIGEST_SIZE,
            PIC32_ALGO_SHA256, sha256->heap);

        wc_InitSha256_ex(sha256, sha256->heap, INVALID_DEVID);  /* reset state */

        return ret;
    }
    void wc_Sha256SizeSet(wc_Sha256* sha256, word32 len)
    {
        if (sha256) {
        #ifdef WOLFSSL_PIC32MZ_LARGE_HASH
            sha256->cache.finalLen = len;
        #else
            (void)len;
        #endif
        }
    }
    void wc_Sha256Pic32Free(wc_Sha256* sha256)
    {
        if (sha256) {
            wc_Pic32HashFree(&sha256->cache, sha256->heap);
        }
    }
#endif /* !NO_SHA256 */
#endif /* WOLFSSL_PIC32MZ_HASH */


#ifdef WOLFSSL_PIC32MZ_CRYPT
#if !defined(NO_AES)
    int wc_Pic32AesCrypt(word32 *key, int keyLen, word32 *iv, int ivLen,
        byte* out, const byte* in, word32 sz,
        int dir, int algo, int cryptoalgo)
    {
        return Pic32Crypto(in, sz, (word32*)out, sz, dir, algo, cryptoalgo,
            key, keyLen, iv, ivLen);
    }
#endif /* !NO_AES */

#ifndef NO_DES3
    int wc_Pic32DesCrypt(word32 *key, int keyLen, word32 *iv, int ivLen,
        byte* out, const byte* in, word32 sz,
        int dir, int algo, int cryptoalgo)
    {
        return Pic32Crypto(in, sz, (word32*)out, sz, dir, algo, cryptoalgo,
            key, keyLen, iv, ivLen);
    }
#endif /* !NO_DES3 */
#endif /* WOLFSSL_PIC32MZ_CRYPT */

#endif /* WOLFSSL_MICROCHIP_PIC32MZ */
