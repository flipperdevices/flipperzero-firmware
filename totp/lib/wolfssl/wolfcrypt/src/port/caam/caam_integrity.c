/* caam_integrity.c
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

#if defined(__INTEGRITY) || defined(INTEGRITY)

/* build into Integrity kernel */
#include <bsp.h>
#include <wolfssl/wolfcrypt/port/caam/caam_driver.h>

#define CAAM_READ(reg) *(volatile unsigned int*)(reg)
#define CAAM_WRITE(reg, in) *(volatile unsigned int*)(reg) = (in);

#define DESC_COUNT 1
#define MAX_BUF 20
#define BUFFER_COUNT (MAX_BUF * DESC_COUNT)

/* CAAM descriptors can only be 64 unsigned ints */
#define MAX_DESC_SZ 64

/* 64 byte buffer for when data crosses a page boundary */
#define ALIGN_BUF 16

/* MAX_CTX is 64 bytes (sha512 digest) + 8 bytes (CAAM length value) */
#define MAX_CTX 18

#define MIN_READ_REG 0xF2100000
#define MAX_READ_REG 0XF2110000

struct JobRing {
    Address JobIn;
    Address JobOut;
    Address Desc;
    Value   page;    /* page allocation for descriptor to use */
};

struct buffer {
    Address data;
    Address dataSz;
};

/* CAAM descriptor */
struct DescStruct {
    struct IORequestStruct TheIORequest;
    struct CAAM_DEVICE*    caam;
    struct buffer          buf[MAX_BUF]; /* buffers holding data input address */
    UINT4                  desc[MAX_DESC_SZ]; /* max size of 64 word32 */
    UINT4                  aadSzBuf[4];       /* Formatted AAD size for CCM */
    UINT4                  alignBuf[ALIGN_BUF]; /* 64 byte buffer for non page
                                                   align */
    UINT4                  iv[MAX_CTX]; /* AES IV and also hash state */
    UINT4                  ctxBuf[MAX_CTX]; /* key */
    Address                output; /* address to output buffer */
    Address                ctxOut; /* address to update buffer holding state */
    Value                  alignIdx;/* index for align buffer */
    Value                  idx;     /* index for descriptor buffer */
    Value                  headIdx; /* for first portion of descriptor buffer */
    Value                  lastIdx; /* for last portion of descriptor buffer */
    Value                  outputIdx; /* idx to output buffer in "buf" */
    Value                  inputSz;   /* size of input buffer */
    Value                  ctxSz;     /* size of CTX/Key buffer */
    Value                  aadSz;     /* AAD size for CCM */
    Value                  lastFifo;
    Value                  type;
    Value                  state;
    Value                  DescriptorCount;
    Boolean                running; /* True if building/running descriptor is
                                       in process */
};

struct CAAM_DEVICE {
    struct IODeviceVectorStruct caamVector;
    struct IODescriptorStruct   IODescriptorArray[BUFFER_COUNT];
    struct DescStruct           DescArray[DESC_COUNT];
    volatile Value              InterruptStatus;
    CALL                        HandleInterruptCall;
    struct JobRing              ring;
};

#define DRIVER_NAME "wolfSSL_CAAM_Driver"

static struct CAAM_DEVICE caam;

/******************************************************************************
  Internal CAAM Job Ring and partition functions
  ****************************************************************************/

/* flush job ring and reset */
static Error caamReset(void)
{
    int t = 100000; /* time out counter for flushing job ring */

    /* make sure interrupts are masked in JRCFGR0_LS register */
    CAAM_WRITE(CAAM_BASE | 0x1054, CAAM_READ(CAAM_BASE | 0x1054) | 1);

    /* flush and reset job rings using JRCR0 register */
    CAAM_WRITE(CAAM_BASE | 0x106C, 1);

    /* check register JRINTR for if halt is in progress */
    while (t > 0 && ((CAAM_READ(CAAM_BASE | 0x104C) & 0x4) == 0x4)) t--;
    if (t == 0) {
        /*unrecoverable failure, the job ring is locked, up hard reset needed*/
        return NotRestartable;
    }

    /* now that flush has been done restart the job ring */
    t = 100000;
    CAAM_WRITE(CAAM_BASE | 0x106C, 1);
    while (t > 0 && ((CAAM_READ(CAAM_BASE | 0x106C) & 1) == 1)) t--;
    if (t == 0) {
        /*unrecoverable failure, reset bit did not return to 0 */
        return NotRestartable;
    }

    /* reset most registers and state machines in CAAM using MCFGR register
       also reset DMA */
    CAAM_WRITE(CAAM_BASE | 0x0004, 0x90000000);

    return Success;
}

/* returns MemoryMapMayNotBeEmpty if page/par is already owned
 * returns Success on success
 * all other returns is an error state
 */
static Error caamCreatePartition(unsigned char page, unsigned char par)
{
    /* check ownership of partition */
    if ((CAAM_READ(CAAM_BASE | 0x1FBC) & (0x3 << (par * 2))) > 0) {
        return MemoryMapMayNotBeEmpty;
    }

    /* set generic all access permissions, gets reset later */
    CAAM_WRITE(CAAM_BASE | (0x1108 + (par * 16)), 0xF);
    CAAM_WRITE(CAAM_BASE | (0x110C + (par * 16)), 0xF);
    CAAM_WRITE(CAAM_BASE | (0x1104 + (par * 16)), 0xFF);

    /* check ownership of page */
    CAAM_WRITE(CAAM_BASE | 0x10F4, (page << 16) | 0x5);
    /* wait for inquiry cmd to complete */
    while ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x0000C000) > 0 &&
       (CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000)  == 0) {
    }
    if ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x000000C0) == 0xC0) {
        /* owns the page can dealloc it */
        CAAM_WRITE(CAAM_BASE | 0x10F4, (page << 16) | 0x2);
        while ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x0000C000) > 0 &&
               (CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000)  == 0) {}
        if ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000)  > 0) {
            /* error while deallocating page */
            return MemoryMapMayNotBeEmpty; /* PSP set on page or is unavailable */
        }
    }
    else {
        /* check if owned by someone else */
        if ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x000000C0) != 0) {
            return MemoryMapMayNotBeEmpty;
        }
    }

    /* allocate page to partition */
    CAAM_WRITE(CAAM_BASE | 0x10F4, (page << 16) | (par << 8) | 0x1);
    /* wait for alloc cmd to complete */
    while ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x0000C000) > 0 &&
       (CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000)  == 0) {
    }

    if ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000) > 0) {
        return MemoryOperationNotPerformed;
    }

    /* double check ownership now of page */
    CAAM_WRITE(CAAM_BASE | 0x10F4, (page << 16) | 0x5);
    /* wait for inquiry cmd to complete */
    while ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x0000C000) > 0 &&
       (CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000)  == 0) {
    }
    if ((CAAM_READ(CAAM_BASE | 0x10FC) & 0x0000000F) == 0 ||
        (CAAM_READ(CAAM_BASE | 0x10FC) & 0x00003000) > 0) {
        /* page not owned */
        return MemoryOperationNotPerformed;
    }

    return Success;
}


/* Gets the status of a job. Returns Waiting if no output jobs ready to be
 * read.
 * If no jobs are done then return Waiting
 * If jobs are done but does not match desc then return NoActivityReady
 * Status holds the error values if any */
static Error caamGetJob(struct CAAM_DEVICE* dev, UINT4* status)
{
    UINT4 reg = CAAM_READ(CAAM_BASE | 0x1044); /* JRSTAR0 status */
    if (status) {
        *status = 0;
    }

    /* check for DECO, CCB, and Job Ring error state JRSTAR0 register */
    if (((reg & 0xF0000000) == 0x20000000) ||         /* CCB error */
        ((reg & 0xF0000000) == 0x40000000)||  /* DECO error */
        ((reg & 0xF0000000) == 0x60000000)) { /* Job Ring error */

        if ((reg & 0x0000000F) > 0) {
            *status = reg;
            return Failure;
        }
    }

    /* Check number of done jobs in output list */
    reg = CAAM_READ(CAAM_BASE | 0x103C);
    if ((reg & 0x000003FF) > 0) {
        UINT4* out = (UINT4*)(dev->ring.JobOut);
        if (status) {
            *status = out[1];
        }

        if ((dev->ring.Desc ^ 0xF0000000) != out[0]) {
            db_printf("CAAM job completed vs expected mismatch");
            return NoActivityReady;
        }

        if (out[1] > 0) {
            return Failure;
        }

        /* increment jobs removed */
        CAAM_WRITE(CAAM_BASE | 0x1034, 1);
    }
    else {
        /* check if the CAAM is idle and not processing any descriptors */
        if ((CAAM_READ(CAAM_BASE | 0x0FD4) & 0x00000002) == 2 /* idle */
        && (CAAM_READ(CAAM_BASE | 0x0FD4) & 0x00000001) == 0) {
            return NoActivityReady;
        }

        return Waiting;
    }

    return Success;
}


/* Initialize CAAM RNG
 * returns 0 on success */
static int caamInitRng(struct CAAM_DEVICE* dev)
{
    UINT4 reg, status;
    int ret = 0;

    /* Set up use of the TRNG for seeding wolfSSL HASH-DRBG */
    CAAM_WRITE(CAAM_RTMCTL, CAAM_PRGM);
    CAAM_WRITE(CAAM_RTMCTL, CAAM_READ(CAAM_RTMCTL) | 0x40); /* reset */

    /* Set up reading from TRNG */
    CAAM_WRITE(CAAM_RTMCTL, CAAM_READ(CAAM_RTMCTL) | CAAM_TRNG);

    /* Set up delay for TRNG @TODO Optimizations?
     * Shift left with RTSDCTL because 0-15 is for sample number
     * Also setting the max and min frequencies */
    CAAM_WRITE(CAAM_RTSDCTL, (CAAM_ENT_DLY << 16) | 0x09C4);
    CAAM_WRITE(CAAM_RTFRQMIN, CAAM_ENT_DLY >> 1); /* 1/2      */
    CAAM_WRITE(CAAM_RTFRQMAX, CAAM_ENT_DLY << 3); /* up to 8x */

    /* Set back to run mode and clear RTMCL error bit */
    reg = CAAM_READ(CAAM_RTMCTL) ^ CAAM_PRGM;

    CAAM_WRITE(CAAM_RTMCTL, reg);
    reg = CAAM_READ(CAAM_RTMCTL);
    reg |= CAAM_CTLERR;
    CAAM_WRITE(CAAM_RTMCTL, reg);

    /* check input slot is available and then add */
    if (CAAM_READ(CAAM_BASE | 0x1014) > 0) {
        UINT4* in = (UINT4*)dev->ring.JobIn;

        memcpy((unsigned char*)dev->ring.Desc, (unsigned char*)wc_rng_start,
        sizeof(wc_rng_start));

        in[0] = dev->ring.Desc ^ 0xF0000000; /* physical address */
        CAAM_WRITE(CAAM_IRJAR0, 0x00000001);
    }
    else {
        return Waiting;
    }

    do {
        ret = caamGetJob(dev, &status);
        /* @TODO use a better way to chill out CPU. */
    } while (ret == Waiting);

    return ret;
}


static Error caamDoJob(struct DescStruct* desc)
{
    Error ret;
    UINT4 status;

    /* clear and set desc size */
    desc->desc[0] &= 0xFFFFFF80;
    desc->desc[0] += desc->idx;

    /* check input slot is available and then add */
    if (CAAM_READ(CAAM_BASE | 0x1014) > 0) {
        UINT4* in = (UINT4*)desc->caam->ring.JobIn;

        memcpy((unsigned char*)desc->caam->ring.Desc, (unsigned char*)desc->desc,
        (desc->idx + 1) * sizeof(UINT4));

        in[0] = desc->caam->ring.Desc ^ 0xF0000000; /* physical address */
        CAAM_WRITE(CAAM_IRJAR0, 0x00000001);
    }
    else {
        return Waiting;
    }

    do {
        ret = caamGetJob(desc->caam, &status);
    /* @TODO use a better way to chill out CPU. */
    } while (ret == Waiting);

    if (status != 0 || ret != Success) {
        #if 0
        /* Used during testing to print out descriptor */
        {
        char msg[2048];
        char* pt = msg;
        int z;

        memset(msg, 0, sizeof(msg));
        for (z = 0; z < desc->idx; z++) {
                    snprintf(pt, sizeof(msg) - (z * 21), "desc[%d] = 0x%8.8x, ",
                z, desc->desc[z]);
            pt += 21;
        }
        snprintf(pt, sizeof(msg) - (z * 21), "status = 0x%8.8x\n", status);
        if (desc->buf[0].data != 0) { /* for testing */
            memcpy((char*)desc->buf[0].data, msg, sizeof(msg));
        }
        }
        #endif


        /* try to reset after error */
        caamReset();
        return ret;
    }

    return Success;
}


/* handle input or output buffers
 * NOTES: if sz == 0 then read all the rest of the buffers available
 * when align == 1 then there is no alignment constraints
 *
 * returns the data size in bytes on success. With failure a negative value is
 * returned.
 */
static int caamAddIO(struct DescStruct* desc, UINT4 options, UINT4 sz,
    UINT4 align, UINT4* idx)
{
    int i, outSz = 0;

    if (align == 0) {
        return -1; /* programming error */
    }

    for (i = *idx; i < desc->DescriptorCount; i++) {
        /* input must be a multiple of "align" bytes */
        struct buffer* buf = &desc->buf[i];
        int blocks = buf->dataSz / align;
        Address data   = buf->data;
        Address dataSz = buf->dataSz;

        if (outSz >= sz && sz != 0) {
            break;
        }

        if (dataSz % align > 0) {
            /* store potential overlap */
            int tmpSz  = dataSz % align;
            int add = (tmpSz < (align - desc->alignIdx)) ? tmpSz :
                align - desc->alignIdx;
            unsigned char* local = (unsigned char*)desc->alignBuf;

            /* if already something in the buffer then add from front */
            if (desc->alignIdx > 0) {
                memcpy((unsigned char*)&local[desc->alignIdx],
                (unsigned char*)data, add);
                data += add;
            }
            else {
                memcpy((unsigned char*)&local[desc->alignIdx],
                (unsigned char*)data + (blocks * align), add);
            }
            dataSz -= add;
            desc->alignIdx += add;
        }

        if (desc->alignIdx == align) {
            desc->lastFifo = desc->idx;
            if (desc->idx + 2 > MAX_DESC_SZ) {
                return -1;
            }
            desc->desc[desc->idx++] = options + desc->alignIdx;
            desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->alignBuf);
            ASP_FlushCaches((Address)desc->alignBuf, desc->alignIdx);
            outSz += desc->alignIdx;
        }

        if (blocks > 0) {
            desc->lastFifo = desc->idx;
            if (desc->idx + 2 > MAX_DESC_SZ) {
                return -1;
            }
            desc->desc[desc->idx++] = options + (blocks * align);
            desc->desc[desc->idx++] = BSP_VirtualToPhysical(data);
            outSz += (blocks * align);

            /* only one buffer available for align cases so exit here and make
            a new descriptor after running current one */
            if (desc->alignIdx == align) {
                desc->alignIdx = 0;
                i++; /* start at next buffer */
                break;
            }
        }
    }

    *idx = i;
    return outSz;
}


/******************************************************************************
  IODevice Register Read and Write
  ****************************************************************************/

static Error caamReadRegister(IODeviceVector ioCaam, Value reg, Value *out)
{
    if (reg < MIN_READ_REG || reg > MAX_READ_REG) {
         return IllegalRegisterNumber;
    }

    switch (reg) {
    case CAAM_STATUS:
    case CAAM_VERSION_MS:
    case CAAM_VERSION_LS:
    case CAMM_SUPPORT_MS:
    case CAMM_SUPPORT_LS:
    case CAAM_RTMCTL:
        *out = CAAM_READ(reg);
        break;

    default:
        return IllegalRegisterNumber;
    }

    (void)ioCaam;
    return Success;
}


static Error caamWriteRegister(IODeviceVector ioCaam, Value reg, Value in)
{
    /* Should be no need for writes */
    return OperationNotAllowedOnTheUniversalIODevice;
}


/******************************************************************************
  CAAM Blob Operations
  ****************************************************************************/

/* limit on size due to size of job ring being 64 word32's */
static Error caamBlob(struct DescStruct* desc)
{
    Error err;
    UINT4 keyType = 0x00000C08; /* default red */
    UINT4 i = 0;
    int sz = 0, ret;

    if (desc->idx + 3 > MAX_DESC_SZ) {
        return Failure;
    }

    /*default to Red Key type, with offset of 12 and 8 byte load to context 2*/
    desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS2 | CAAM_IMM | keyType);

    /* add key modifier */
    if (i < desc->DescriptorCount) {
        UINT4* pt;
        Address data   = desc->buf[i].data;
        Address dataSz = desc->buf[i].dataSz;

        pt = (UINT4*)data;
        if (dataSz < 8) { /* expecting 8 bytes for key modifier*/
            return TooManyBuffers;
        }
        desc->desc[desc->idx++] = pt[0];
        desc->desc[desc->idx++] = pt[1];
    }

    /* add input */
    while (sz < desc->inputSz && i < desc->DescriptorCount) {
        ret = caamAddIO(desc, CAAM_SEQI, desc->inputSz - sz, 1, &i);
        if (ret < 0) { /* handle error case */
            return TooManyBuffers;
        }
        sz += ret;
    }
    desc->outputIdx = i;

    /* add output */
    if (caamAddIO(desc, CAAM_SEQO, 0, 1, &i) < 0) {
        return TooManyBuffers;
    }

    if (desc->idx + 1 > MAX_DESC_SZ) {
        return Failure;
    }
    desc->desc[desc->idx++] = CAAM_OP |  CAAM_OPID_BLOB | desc->type;

    if ((err = caamDoJob(desc)) != Success) {
        return err;
    }

    /* flush output buffers */
    for (i = desc->outputIdx; i < desc->DescriptorCount; i++) {
        ASP_FlushCaches(desc->buf[i].data, desc->buf[i].dataSz);
    }

    return Success;
}


/******************************************************************************
  CAAM AES Operations
  ****************************************************************************/

/* returns amount written on success and negative value in error case.
 * Is different from caamAddIO in that it only adds a single input buffer
 * rather than multiple ones.
 */
static int caamAesInput(struct DescStruct* desc, UINT4* idx, int align,
    UINT4 totalSz)
{
    int sz;
    UINT4 i = *idx;

    /* handle alignment constraints on input */
    if (desc->alignIdx > 0) {
        sz = desc->alignIdx;

        /* if there is more input buffers then add part of it */
        if (i < desc->outputIdx && i < desc->DescriptorCount) {
            sz = align - desc->alignIdx;
            sz = (sz <= desc->buf[i].dataSz) ? sz : desc->buf[i].dataSz;
            memcpy((unsigned char*)(desc->alignBuf) + desc->alignIdx,
                   (unsigned char*)(desc->buf[i].data), sz);

            desc->buf[i].dataSz -= sz;
            desc->buf[i].data   += sz;
            sz += desc->alignIdx;
        }

        if (desc->idx + 2 > MAX_DESC_SZ) {
            return -1;
        }
        ASP_FlushCaches((Address)desc->alignBuf, sz);
        desc->desc[desc->idx++] = (CAAM_FIFO_L | FIFOL_TYPE_LC1 |
                                   CAAM_CLASS1 | FIFOL_TYPE_MSG) + sz;
        desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->alignBuf);
        desc->alignIdx = 0;
    }
    else {
        sz = desc->buf[i].dataSz;
        if ((totalSz + sz) == desc->inputSz) { /* not an issue on final */
            align = 1;
        }

        desc->alignIdx = sz % align;
        if (desc->alignIdx != 0) {
            sz -= desc->alignIdx;
            memcpy((unsigned char*)desc->alignBuf,
                   (unsigned char*)(desc->buf[i].data) + sz,
                   desc->alignIdx);
        }

        if (desc->idx + 2 > MAX_DESC_SZ) {
            return -1;
        }
        desc->desc[desc->idx++] = (CAAM_FIFO_L | FIFOL_TYPE_LC1 |
                                   CAAM_CLASS1 | FIFOL_TYPE_MSG) + sz;
        desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->buf[i].data);
        i++;
    }

    *idx = i;
    return sz;
}


/* returns enum Success on success, all other return values should be
 * considered an error.
 *
 * ofst    is the amount of leftover buffer from previous calls
 * inputSz is the amount of input in bytes that is being matched to output
 */
static Error caamAesOutput(struct DescStruct* desc, int* ofst, UINT4 inputSz)
{
    int offset = *ofst;

    if (desc->output != 0 && offset > 0 && inputSz > 0) {
        UINT4 addSz;

        /* handle potential leftovers */
        addSz = (inputSz >= offset) ? offset : inputSz;

        inputSz -= addSz;
        desc->desc[desc->idx++] = CAAM_FIFO_S | FIFOS_TYPE_MSG + addSz;
        if (inputSz > 0) { /* check if expecting more output */
            desc->desc[desc->idx - 1] |= CAAM_FIFOS_CONT;
        }
        desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->output);

        if (addSz == offset) {
            /* reset */
            desc->output = 0;
            offset       = 0;
        }
        else {
            offset -= addSz;
            desc->output += addSz;

            if (offset < 0) {
                return TransferFailed;
            }
        }
    }

    for (; desc->lastIdx < desc->DescriptorCount; desc->lastIdx++) {
        struct buffer* buf = &desc->buf[desc->lastIdx];

        if (inputSz > 0) {
            int tmp;

            if (buf->dataSz <= inputSz) {
                tmp = buf->dataSz;
            }
            else {
                offset = buf->dataSz - inputSz;
                tmp    = inputSz;
                desc->output = buf->data + tmp;
            }
            inputSz -= tmp;
            if (desc->idx + 2 > MAX_DESC_SZ) {
                return TransferFailed;
            }
            desc->desc[desc->idx++] = CAAM_FIFO_S | FIFOS_TYPE_MSG + tmp;
            if (inputSz > 0) { /* check if expecting more output */
                desc->desc[desc->idx - 1] |= CAAM_FIFOS_CONT;
            }
            desc->desc[desc->idx++] = BSP_VirtualToPhysical(buf->data);
        }
        else {
            break;
        }
    }

    *ofst = offset;
    return Success;
}


/* check size of output and get starting buffer for it */
static Error caamAesOutSz(struct DescStruct* desc, UINT4 i)
{
    int sz = 0;

    for (desc->outputIdx = i; desc->outputIdx < desc->DescriptorCount &&
    sz < desc->inputSz; desc->outputIdx++) {
        sz += desc->buf[desc->outputIdx].dataSz;
    }
    desc->lastIdx = desc->outputIdx;

    /* make certain that output size is same as input */
    sz = 0;
    for (; desc->lastIdx < desc->DescriptorCount; desc->lastIdx++) {
        sz += desc->buf[desc->lastIdx].dataSz;
    }
    if (sz != desc->inputSz) {
        return SizeIsTooLarge;
    }
    desc->lastIdx = desc->outputIdx;

    return Success;
}


/* AES operations follow the buffer sequence of KEY -> (IV) -> Input -> Output
 */
static Error caamAes(struct DescStruct* desc)
{
    struct buffer* ctx[3];
    struct buffer* iv[3];
    Value ofst = 0;
    Error err;
    UINT4 i, totalSz = 0;
    int ctxIdx = 0;
    int ivIdx  = 0;
    int offset = 0;
    int align  = 1;
    int sz     = 0;

    int ctxSz = desc->ctxSz;

    if (desc->state != CAAM_ENC && desc->state != CAAM_DEC) {
        return IllegalStatusNumber;
    }

    if (ctxSz != 16 && ctxSz != 24 && ctxSz != 32) {
        return ArgumentError;
    }

    /* get key */
    for (i = 0; i < desc->DescriptorCount; i++) {
        struct buffer* buf = &desc->buf[i];
        unsigned char* local = (unsigned char*)desc->ctxBuf;

        if (sz < ctxSz && sz < (MAX_CTX * sizeof(UINT4))) {
            ctx[ctxIdx] = buf;
            sz += buf->dataSz;

            memcpy((unsigned char*)&local[offset],
                   (unsigned char*)ctx[ctxIdx]->data, ctx[ctxIdx]->dataSz);
            offset += ctx[ctxIdx]->dataSz;
            ctxIdx++;
        }
        else {
            break;
        }
    }

    /* sanity checks on size of key */
    if (sz > ctxSz) {
        return SizeIsTooLarge;
    }
    if (ctxSz > (MAX_CTX * sizeof(UINT4)) - 16) {
        return ArgumentError;
    }

    /* Flush cache of ctx buffer then :
       Add KEY Load command          0x0220000X
       Add address to read key from  0xXXXXXXXX */
    ASP_FlushCaches((Address)desc->ctxBuf, ctxSz);
    if (desc->idx + 2 > MAX_DESC_SZ) {
        return TransferFailed;
    }
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1 | CAAM_NWB) + ctxSz;
    desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->ctxBuf);

    /* get IV if needed by algorithm */
    switch (desc->type) {
        case CAAM_AESECB:
            break;

        case CAAM_AESCTR:
            ofst = 0x00001000;
            /* fall through because states are the same only the offset changes */

        case CAAM_AESCBC:
        {
            int maxSz = 16; /* default to CBC/CTR max size */

            sz = 0;
            offset = 0;
            for (; i < desc->DescriptorCount; i++) {
                struct buffer* buf = &desc->buf[i];
                unsigned char* local = (unsigned char*)desc->iv;

                if (sz < maxSz) {
                    iv[ivIdx] = buf;

                    if (buf->dataSz + sz > maxSz) {
                        return SizeIsTooLarge;
                    }

                    sz += buf->dataSz;
                    memcpy((unsigned char*)&local[offset],
                        (unsigned char*)iv[ivIdx]->data, iv[ivIdx]->dataSz);
                    offset += iv[ivIdx]->dataSz;
                    ivIdx++;
                }
                else {
                    break;
                }
            }

            if (sz != maxSz) {
                /* invalid IV size */
                return SizeIsTooLarge;
            }

            ASP_FlushCaches((Address)desc->iv, maxSz);
            if (desc->idx + 2 > MAX_DESC_SZ) {
                return TransferFailed;
            }
            desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS1 | ofst) + maxSz;
            desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->iv);
         }
         break;

        default:
            return OperationNotImplemented;
    }

    /* write operation */
    if (desc->idx + 1 > MAX_DESC_SZ) {
        return TransferFailed;
    }
    desc->desc[desc->idx++] = CAAM_OP | CAAM_CLASS1 | desc->type |
             CAAM_ALG_UPDATE | desc->state;

    /* find output buffers */
    if (caamAesOutSz(desc, i) != Success) {
        return SizeIsTooLarge;
    }

    /* set alignment constraints */
    if (desc->type == CAAM_AESCBC || desc->type == CAAM_AESECB) {
        align = 16;
    }

    /* indefinite loop for input/output buffers */
    desc->headIdx = desc->idx;
    desc->output  = 0;
    offset = 0; /* store left over amount for output buffer */
    do {
        desc->idx = desc->headIdx; /* reset for each loop */

        /* add a single input buffer (multiple ones was giving deco watch dog
         * time out errors on the FIFO load of 1c.
         * @TODO this could be a place for optimization if more data could be
         * loaded in at one time */
        if ((sz = caamAesInput(desc, &i, align, totalSz)) < 0) {
            return TransferFailed;
        }
        totalSz += sz;

        if (caamAesOutput(desc, &offset, sz) != Success) {
            return TransferFailed;
        }

        /* store updated IV */
        if (ivIdx > 0) {
            if (desc->idx + 2 > MAX_DESC_SZ) {
                return TransferFailed;
            }
            desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 | ofst | 16;
            desc->desc[desc->idx++] = BSP_VirtualToPhysical((Address)desc->iv);
        }

        if ((err = caamDoJob(desc)) != Success) {
            return err;
        }
        ASP_FlushCaches((Address)desc->iv, 16);
    } while (desc->lastIdx < desc->DescriptorCount || offset > 0);

    /* flush output buffers */
    for (i = desc->outputIdx; i < desc->lastIdx; i++) {
        ASP_FlushCaches(desc->buf[i].data, desc->buf[i].dataSz);
    }

    /* handle case with IV */
    if (ivIdx > 0) {
        unsigned char* pt = (unsigned char*)desc->iv;
        ASP_FlushCaches((Address)pt, 16);
        for (i = 0; i < ivIdx; i++) {
            memcpy((unsigned char*)iv[i]->data, pt, iv[i]->dataSz);
            pt += iv[i]->dataSz;
            ASP_FlushCaches(iv[i]->data, iv[i]->dataSz);
        }
    }

    return Success;
}


/******************************************************************************
  CAAM AEAD Operations
  ****************************************************************************/

/* AEAD operations follow the buffer sequence of KEY -> (IV or B0 | CTR0) -> (AD)
 * -> Input -> Output
 *
 */
static Error caamAead(struct DescStruct* desc)
{
    struct buffer* ctx[3];
    struct buffer* iv[3];
    Value ofst    = 0;
    UINT4 state   = CAAM_ALG_INIT;
    UINT4 totalSz = 0;
    Error err;
    UINT4 i;
    int ctxIdx = 0;
    int ivIdx  = 0;
    int offset = 0;
    int sz     = 0;
    int ivSz   = 32; /* size of B0 | CTR0 for CCM mode */
    int ctxSz  = desc->ctxSz;
    int align  = 16; /* input should be multiples of 16 bytes unless is final */
    int opIdx;

    if (desc->state != CAAM_ENC && desc->state != CAAM_DEC) {
        return IllegalStatusNumber;
    }

    /* sanity check is valid AES key size */
    if (ctxSz != 16 && ctxSz != 24 && ctxSz != 32) {
        return ArgumentError;
    }

    /* get key */
    for (i = 0; i < desc->DescriptorCount; i++) {
        struct buffer* buf = &desc->buf[i];
        unsigned char* local = (unsigned char*)desc->ctxBuf;

        if (sz < ctxSz && sz < (MAX_CTX * sizeof(UINT4))) {
            ctx[ctxIdx] = buf;
            sz += buf->dataSz;

            memcpy((unsigned char*)&local[offset],
                   (unsigned char*)ctx[ctxIdx]->data, ctx[ctxIdx]->dataSz);
            offset += ctx[ctxIdx]->dataSz;
            ctxIdx++;
        }
        else {
            break;
        }
    }

    /* sanity checks on size of key */
    if (sz > ctxSz) {
        return SizeIsTooLarge;
    }

    /* Flush cache of ctx buffer then :
       Add KEY Load command          0x0220000X
       Add address to read key from  0xXXXXXXXX */
    ASP_FlushCaches((Address)desc->ctxBuf, ctxSz);
    if (desc->idx + 2 > MAX_DESC_SZ) {
        return TransferFailed;
    }
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1 | CAAM_NWB) + ctxSz;
    desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->ctxBuf);

    desc->headIdx = desc->idx;
    desc->output  = 0;
    offset = 0; /* store left over amount for output buffer */
    do {
        desc->idx = desc->headIdx; /* reset for each loop */

        /* write operation */
        if (desc->idx + 1 > MAX_DESC_SZ) {
            return TransferFailed;
        }
        opIdx = desc->idx;
        desc->desc[desc->idx++] = CAAM_OP | CAAM_CLASS1 | state | desc->type |
                                  desc->state;

        /* get IV if needed by algorithm */
        switch (desc->type) {
            case CAAM_AESCCM:
                if ((state & CAAM_ALG_INIT) == CAAM_ALG_INIT) {
                    sz = 0;
                    offset = 0;
                    for (; i < desc->DescriptorCount; i++) {
                        struct buffer* buf = &desc->buf[i];
                        unsigned char* local = (unsigned char*)desc->iv;

                        if (sz < ivSz) {
                            iv[ivIdx] = buf;

                            if (buf->dataSz + sz > ivSz) {
                                return SizeIsTooLarge;
                            }

                            sz += buf->dataSz;
                            memcpy((unsigned char*)&local[offset],
                            (unsigned char*)iv[ivIdx]->data, iv[ivIdx]->dataSz);
                            offset += iv[ivIdx]->dataSz;
                            ivIdx++;
                        }
                        else {
                            break;
                        }
                    }

                    if (sz != ivSz) {
                        /* invalid IV size */
                        return SizeIsTooLarge;
                    }
                    offset = 0;
                }

                ASP_FlushCaches((Address)desc->iv, ivSz);
                if (desc->idx + 2 > MAX_DESC_SZ) {
                    return TransferFailed;
                }
                desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS1 | ofst)
                                           + ivSz;
                desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->iv);
                break;

            default:
                return OperationNotImplemented;
        }


        /********* handle AAD -- is only done with Init **********************/
        if ((state & CAAM_ALG_INIT) == CAAM_ALG_INIT) {
            if ((desc->type == CAAM_AESCCM) && (desc->aadSz > 0)) {
                /* set formatted AAD buffer size for CCM */
                ASP_FlushCaches((Address)desc->aadSzBuf, sizeof(desc->aadSzBuf));
                desc->desc[desc->idx++] = CAAM_FIFO_L | CAAM_CLASS1 |
                    FIFOL_TYPE_AAD + desc->aadSz;
                desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->aadSzBuf);

                /* now set aadSz to unformatted version for getting buffers */
                if (desc->aadSz == 2) {
                    unsigned char* pt = (unsigned char*)desc->aadSzBuf;
                    desc->aadSz = (((UINT4)pt[0] & 0xFF) << 8) |
                           ((UINT4)pt[1] & 0xFF);
                }
                else {
                    unsigned char* pt = (unsigned char*)desc->aadSzBuf;
                    desc->aadSz = (((UINT4)pt[2] & 0xFF) << 24) |
                                  (((UINT4)pt[3] & 0xFF) << 16) |
                                  (((UINT4)pt[4] & 0xFF) <<  8) |
                                   ((UINT4)pt[5] & 0xFF);
                }
            }

            /* get additional data buffers */
            if (desc->aadSz > 0) {
                sz = 0;
                for (; i < desc->DescriptorCount; i++) {
                    struct buffer* buf = &desc->buf[i];
                    if (sz < desc->aadSz) {
                        if (desc->idx + 2 > MAX_DESC_SZ) {
                            return TransferFailed;
                        }
                        desc->lastFifo = desc->idx;
                        desc->desc[desc->idx++] = CAAM_FIFO_L | CAAM_CLASS1 |
                                                  FIFOL_TYPE_AAD + buf->dataSz;
                        desc->desc[desc->idx++] = BSP_VirtualToPhysical(buf->data);
                        sz += buf->dataSz;
                    }
                    else {
                        break;
                    }
                }

                /* flush AAD from FIFO and pad it to 16 byte block */
                desc->desc[desc->lastFifo] |= FIFOL_TYPE_FC1;
            }

            /* find output buffers */
            if (caamAesOutSz(desc, i) != Success) {
                return SizeIsTooLarge;
            }
        }

        /* handle alignment constraints on input */
        if ((sz = caamAesInput(desc, &i, align, totalSz)) < 0) {
            return TransferFailed;
        }
        totalSz += sz;

        /* handle output buffers  */
        if (caamAesOutput(desc, &offset, sz) != Success) {
            return TransferFailed;
        }

        /* store updated IV, if is last then set offset and final for MAC */
        if ((desc->lastIdx == desc->DescriptorCount) && (offset == 0)) {
            ivSz = 16;
            if (desc->state == CAAM_ENC) {
                ofst = 32 << 8; /* offset is in 15-8 bits */
            }
            else {
                ofst = 0;
            }
            desc->desc[opIdx] |= CAAM_ALG_FINAL;
        }
        else {
            /* if not final then store and use ctr and encrypted ctr from
                context dword 2,3 and 4,5. Also store MAC and AAD info from
                context dword 6. */
            ivSz = 56;
            ofst = 0;
        }

        if (desc->idx + 2 > MAX_DESC_SZ) {
            return TransferFailed;
        }
        desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 | ofst | ivSz;
        desc->desc[desc->idx++] = BSP_VirtualToPhysical((Address)desc->iv);

        if ((err = caamDoJob(desc)) != Success) {
            return err;
        }
        state = CAAM_ALG_UPDATE;
    } while (desc->lastIdx < desc->DescriptorCount || offset > 0);

    /* flush output buffers */
    for (i = desc->outputIdx; i < desc->lastIdx; i++) {
        ASP_FlushCaches(desc->buf[i].data, desc->buf[i].dataSz);
    }

    /* handle case with IV (This is also the output of MAC with AES-CCM) */
    if (ivIdx > 0) {
        unsigned char* pt = (unsigned char*)desc->iv;
        ASP_FlushCaches((Address)pt, ivSz);
        for (i = 0; i < ivIdx; i++) {
            memcpy((unsigned char*)iv[i]->data, pt, iv[i]->dataSz);
            pt += iv[i]->dataSz;
            ASP_FlushCaches(iv[i]->data, iv[i]->dataSz);
        }
    }

    return Success;
}


/******************************************************************************
  CAAM SHA Operations
  ****************************************************************************/
static int shaSize(struct DescStruct* desc)
{
    /* sanity check on dataSz for context */
    switch (desc->type) {
        case CAAM_MD5:
            return CAAM_MD5_CTXSZ;

        case CAAM_SHA:
            return CAAM_SHA_CTXSZ;

        case CAAM_SHA224:
            return CAAM_SHA224_CTXSZ;

        case CAAM_SHA256:
            return CAAM_SHA256_CTXSZ;

        case CAAM_SHA384:
            return CAAM_SHA384_CTXSZ;

        case CAAM_SHA512:
            return CAAM_SHA512_CTXSZ;

        default:
            return 0;
    }
}

/* SHA operations
 * start: the index to start traversing through buffers. It's needed to allow
 *       for HMAC to reuse this code.
 *
 * return Success on success. All other return values are considered a fail
 *         case.
 */
static Error caamSha(struct DescStruct* desc, int start)
{
    struct buffer* ctx[3];
    Error err;
    UINT4 i;
    int sz     = 0;
    int ctxIdx = 0;
    int offset = 0;

    int ctxSz = shaSize(desc);

    /* get context */
    for (i = start; i < desc->DescriptorCount; i++) {
        struct buffer* buf = &desc->buf[i];
        unsigned char* local = (unsigned char*)desc->iv;

        if (sz < ctxSz && sz < (MAX_CTX * sizeof(UINT4))) {
            ctx[ctxIdx] = buf;
            sz += buf->dataSz;

            if (ctx[ctxIdx]->dataSz + offset > (MAX_CTX * sizeof(UINT4))) {
                return SizeIsTooLarge;
            }
            memcpy((unsigned char*)&local[offset], (unsigned char*)ctx[ctxIdx]->data,
            ctx[ctxIdx]->dataSz);
            offset += ctx[ctxIdx]->dataSz;
            ctxIdx++;
        }
        else {
            break;
        }
    }
    if (sz > ctxSz || ctxSz > (MAX_CTX * sizeof(UINT4))) {
        return SizeIsTooLarge;
    }

    ASP_FlushCaches((Address)desc->iv, ctxSz);
    /*Manage Context (current digest + 8 byte running message length)*/
    if ((desc->state & CAAM_ALG_INIT) != CAAM_ALG_INIT) {
        /* don't load into the class 2 context register on inti.
           Found that loading in caused context to not get set. */
        if (desc->idx + 2 > MAX_DESC_SZ) {
            return TransferFailed;
        }
        desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS2) + ctxSz;
        desc->desc[desc->idx++] = BSP_VirtualToPhysical((Address)desc->iv);
    }

    /* add operation command */
    desc->desc[desc->idx++] = CAAM_OP | CAAM_CLASS2 | desc->state |
        desc->type;

    /* Check case where there is no input.
       In all cases the FIFO Load should be flushed. */
    if (i == desc->DescriptorCount) {
        desc->lastFifo = desc->idx;
        if (desc->idx + 1 > MAX_DESC_SZ) {
            return TransferFailed;
        }
        desc->desc[desc->idx++] = CAAM_FIFO_L | CAAM_CLASS2 |
        FIFOL_TYPE_MSG | CAAM_IMM;
    }

    /* save index for looping over input */
    desc->headIdx = desc->idx;
    do {
        desc->idx = desc->headIdx; /* reset for each loop */
        if (i < desc->DescriptorCount) {
            /* input must be a multiple of 64 bytes unless in final call */
            if (((desc->state & CAAM_ALG_FINAL) == CAAM_ALG_FINAL)) {
                if (caamAddIO(desc, (CAAM_FIFO_L | CAAM_CLASS2 |
                    FIFOL_TYPE_MSG), 0, 1, &i) < 0) {
                    return TooManyBuffers;
                }
            }
            else {
                if (caamAddIO(desc, (CAAM_FIFO_L | CAAM_CLASS2 |
                    FIFOL_TYPE_MSG), 0, 64, &i) < 0) {
                    return TooManyBuffers;
                }
            }
        }

        desc->desc[desc->lastFifo] |= FIFOL_TYPE_LC2;

        /* set context out */
        if (desc->idx + 2 > MAX_DESC_SZ) {
            return TransferFailed;
        }
        desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS2 + ctxSz;
        desc->desc[desc->idx++] = BSP_VirtualToPhysical(desc->iv);

        if ((err = caamDoJob(desc)) != Success) {
            return err;
        }
        /* flush context output for each loop */
        ASP_FlushCaches((Address)desc->iv, ctxSz);
    } while (i < desc->DescriptorCount);

    /* store context to buffers */
    {
        unsigned char* pt = (unsigned char*)desc->iv;
        for (i = 0; i < ctxIdx; i++) {
            memcpy((unsigned char*)ctx[i]->data, pt, ctx[i]->dataSz);
            pt += ctx[i]->dataSz;
            ASP_FlushCaches(ctx[i]->data, ctx[i]->dataSz);
        }
    }

    return Success;
}


/******************************************************************************
  CAAM TRNG Operations
  ****************************************************************************/

/* If Entropy is not ready then return Waiting */
static Error caamRng(struct DescStruct* desc)
{
    int sz = 0;
    int i;

    Address  reg; /* RTENT reg to read */
    int ofst = sizeof(UINT4);


    /* Check ENT_VAL bit to make sure entropy is ready */
    if ((CAAM_READ(CAAM_RTMCTL) & CAAM_ENTVAL) !=
            CAAM_ENTVAL) {
        return Waiting;
    }

    /* check state of TRNG */
    if ((CAAM_READ(CAAM_RTSTATUS) & 0x0000FFFF) > 0) {
        return Failure;
    }

    /* read entropy from RTENT registers */
    reg = CAAM_RTENT0;

    for (i = 0; i < desc->DescriptorCount; i++) {
        struct buffer* buf = &desc->buf[i];
        unsigned char* local = (unsigned char*)buf->data;
        sz = buf->dataSz;

        while (sz > 3 && reg <= CAAM_RTENT11) {
            *((UINT4*)local) = CAAM_READ(reg);
            reg    += ofst;
            local  += ofst;
            sz     -= ofst;
        }

        if (reg > CAAM_RTENT11 && sz > 0) {
            return SizeIsTooLarge;
        }

        /* handle non word32 size amount left over */
        if (sz > 0) {
            UINT4 tmp = CAAM_READ(reg);
            memcpy(local, (unsigned char*)&tmp, sz);
        }

        ASP_FlushCaches(buf->data, buf->dataSz);
    }


    /* read RTENT11 to trigger new entropy generation */
    if (reg != CAAM_RTENT11) {
        CAAM_READ(CAAM_RTENT11);
    }

    return Success;
}


/******************************************************************************
  IODevice Start, Transfer and Finish Buffer
  ****************************************************************************/
/* args[0] holds the state such as encrypt/decrypt or init/update/final
 * args[1] holds the ctx/key size
 * args[2] holds the input size
 * args[3] dependent on algo (such as AAD size with AES-CCM) */
static Error caamTransferStart(IODeviceVector ioCaam,
    Value type, const volatile Value args[4])
{
    struct CAAM_DEVICE* local = (struct CAAM_DEVICE*)ioCaam;
    struct DescStruct*  desc;

    /* currently only one desc is available for use */
    desc = &local->DescArray[0];

    /* check if the desc is idle before using */
    if (GetIORequestStatus((IORequest)desc) != IdleIORequest) {
         return ResourceNotAvailable;
    }

    desc->idx    = 0;
    desc->output = 0;
    desc->ctxOut = 0;
    desc->outputIdx = 0;
    desc->alignIdx = 0;
    desc->lastFifo = 0;
    desc->state    = args[0];
    desc->ctxSz    = args[1];
    desc->inputSz  = args[2];
    desc->aadSz    = 0;
    desc->desc[desc->idx++] = CAAM_HEAD; /* later will put size to header*/

    switch (type) {
        case CAAM_AESECB:
        case CAAM_AESCBC:
            if (desc->inputSz % 16 != 0) {
                return ArgumentError;
            }
            /* fall through to break */
        case CAAM_AESCTR:
            break;

        case CAAM_AESCCM:
            memset((unsigned char*)desc->aadSzBuf, 0, sizeof(desc->aadSzBuf));
            if (args[3] > 0) {
                /* encode the length in */
                if (args[3] <= 0xFEFF) {
                    unsigned char* pt = (unsigned char*)desc->aadSzBuf;
                    desc->aadSz = 2;
                    pt[0] = ((args[3] & 0xFF00) >> 8);
                    pt[1] =  (args[3] & 0x00FF);
                }
                else if (args[3] <= 0xFFFFFFFF) {
                    unsigned char* pt = (unsigned char*)desc->aadSzBuf;
                    desc->aadSz = 6;
                    pt[0] = 0xFF; pt[1] = 0xFE;
                    pt[2] = ((args[3] & 0xFF000000) >> 24);
                    pt[3] = ((args[3] & 0x00FF0000) >> 16);
                    pt[4] = ((args[3] & 0x0000FF00) >>  8);
                    pt[5] =  (args[3] & 0x000000FF);
                }
            }
            break;

        case CAAM_MD5:
        case CAAM_SHA:
        case CAAM_SHA224:
        case CAAM_SHA256:
        case CAAM_SHA384:
        case CAAM_SHA512:
            break;

        case CAAM_BLOB_ENCAP:
        case CAAM_BLOB_DECAP:
            break;

        case CAAM_ENTROPY:
            break;

        default:
            /* unknown type */
            return UsageNotSupported;
    }

    desc->DescriptorCount = 0;
    desc->type    = type;
    desc->running = true;
    StartIORequest((IORequest)desc);

    /* For now only require READ permissions */
    SetIORequestBufferPermissions((IORequest)desc, MEMORY_READ);
    return Success;
}


static Error caamTransferBuffer(IODeviceVector TheIODeviceVector,
    IORequest req, IODescriptor NewIODescriptor,
    Address data, Address dataSz)
{
    struct DescStruct* desc = (struct DescStruct*)req;
    Error  err;

    switch (desc->type) {
        case CAAM_AESECB:
        case CAAM_AESCTR:
        case CAAM_AESCBC:
        case CAAM_AESCCM:

        case CAAM_MD5:
        case CAAM_SHA:
        case CAAM_SHA224:
        case CAAM_SHA256:
        case CAAM_SHA384:
        case CAAM_SHA512:

        case CAAM_BLOB_ENCAP:
        case CAAM_BLOB_DECAP:
        case CAAM_ENTROPY:
            { /* set buffer for transfer finish */
                struct buffer* buf;
                if (desc->DescriptorCount >= MAX_BUF) {
                    return TooManyBuffers;
                }
                buf = &desc->buf[desc->DescriptorCount];
                buf->data = data;
                buf->dataSz = dataSz;
            }
                err = Success;
            break;

        default:
            err = UsageNotSupported;
    }

    if (err != Success) {
        desc->running = false;
        DismissIORequest(req);
        return err;
    }

    desc->DescriptorCount++;
    return Success;
}


static Error caamTransferFinish(IODeviceVector ioCaam, IORequest req)
{
    struct DescStruct* desc = (struct DescStruct*)req;
    Error ret;

    /* construct desc */
    switch (desc->type) {
        case CAAM_AESECB:
        case CAAM_AESCTR:
        case CAAM_AESCBC:
            ret = caamAes(desc);
            break;

        case CAAM_AESCCM:
            ret = caamAead(desc);
            break;

        case CAAM_MD5:
        case CAAM_SHA:
        case CAAM_SHA224:
        case CAAM_SHA256:
        case CAAM_SHA384:
        case CAAM_SHA512:
            ret = caamSha(desc, 0);
            break;

        case CAAM_ENTROPY:
            ret = caamRng(desc);
            break;

        case CAAM_BLOB_ENCAP:
        case CAAM_BLOB_DECAP:
            ret = caamBlob(desc);
            break;

        default:
            ret = UsageNotSupported;
    }

    desc->running = false;
    DismissIORequest(req);
    return ret;
}


/******************************************************************************
  IODevice Interrupt and Init
  ****************************************************************************/

static Error caamTransferWrite(IODeviceVector ioCaam,
    IORequest req, Value dataSz, const volatile Value *data)
{
    DismissIORequest(req);
    return UsageNotSupported;
}


static void caamTransferAbort(IODeviceVector ioCaam, IORequest req)
{
    DismissIORequest(req);
}


static void caamTransferRecall(IODeviceVector ioCaam, IODescriptor req)
{

}


static void HandleInterrupt(Address id)
{
    struct CAAM_DEVICE* local = (struct CAAM_DEVICE*)id;
    Value InterruptStatus = INTERRUPT_AtomicWrite(&local->InterruptStatus, 0);
    int i;

    /* Loop through descriptors and try to dismiss them */
    for (i = 0; i < DESC_COUNT; i++) {
        struct DescStruct* desc = &local->DescArray[i];
        if (InterruptStatus & (1 << i)) {
            desc->running = false;
            if (GetIORequestStatus((IORequest)desc) == IORequestSuspended) {
                ContinueIORequest((IORequest)desc);
            }
            else {
                DismissIORequest((IORequest)desc);
            }
        }
    }
}


static Error caamCreate(IODeviceVector ioCaam)
{
    return Success;
}


void  InitCAAM(void)
{
    /* get IO vector and set it up */
    IODeviceVector ioCaam = &caam.caamVector;
    unsigned int reg;
    int   i;
    Error ret;


    ioCaam->Create         = &caamCreate;
    ioCaam->ReadRegister   = &caamReadRegister;
    ioCaam->WriteRegister  = &caamWriteRegister;

    ioCaam->TransferStart  = &caamTransferStart;
    ioCaam->TransferBuffer = &caamTransferBuffer;
    ioCaam->TransferWrite  = &caamTransferWrite;
    ioCaam->TransferFinish = &caamTransferFinish;
    ioCaam->TransferAbort  = &caamTransferAbort;
    ioCaam->TransferRecall = &caamTransferRecall;
#ifdef HARDWARE_CACHE_COHERENCY
    ioCaam->IOSynchronizationNotRequired = 1;
#endif

    RegisterIODeviceVector(ioCaam, DRIVER_NAME);
    RequestIOTerminationTask(ioCaam, 10);

    /* Initialize descriptors */
    for (i = 0; i < BUFFER_COUNT; i++) {
        InitializeIODescriptor(ioCaam, &caam.IODescriptorArray[i]);
    }

    /* Initialize Descriptors */
    for (i = 0; i < DESC_COUNT; i++) {
         InitializeIORequest(ioCaam, &caam.DescArray[i].TheIORequest,
                             IOREQUEST_STANDARD);
         caam.DescArray[i].running = false;
         caam.DescArray[i].caam    = &caam;
    }


    /* call interrupt to make IORequests available */
    caam.InterruptStatus = 0;
    INTERRUPT_InitCall(&caam.HandleInterruptCall,
        &HandleInterrupt, "Start up CAAM IORequest");

    /* set clock speed for CAAM. Setting it here to allow for restricting
       access */
    #define REGS_CCM_BASE     (0xf20c4000)
    #define HW_CCM_CCGR0_ADDR (0xf20c4068)
    #define CG(x) (3 << (x*2))

    reg = CG(6) | CG(5) | CG(4);
    *(volatile unsigned int*)HW_CCM_CCGR0_ADDR =
    *(volatile unsigned int*)HW_CCM_CCGR0_ADDR | reg;

    /* set up job ring */

    /* @TODO create partition in physical memory for job rings
       current partition security is set to the default */
    for (i = 1; i < CAAM_PAGE_MAX; i++) {
        ret = caamCreatePartition(i, i);
        if (ret == 0) {
            break;
        }

        if (ret != MemoryMapMayNotBeEmpty) {
            INTERRUPT_Panic();
        }
    }

    if (ret != 0) {
        INTERRUPT_Panic();
    }

    caam.ring.page = i;
    caam.ring.JobIn  =  (CAAM_PAGE + (i << 12));
    caam.ring.JobOut  = caam.ring.JobIn  + 16;
    caam.ring.Desc    = caam.ring.JobOut + 16;

    /* set physical address of job rings */
    CAAM_WRITE(CAAM_IRBAR0, caam.ring.JobIn  ^ 0xF0000000);
    CAAM_WRITE(CAAM_ORBAR0, caam.ring.JobOut ^ 0xF0000000);

    /* Initialize job ring sizes to 1 */
    CAAM_WRITE(CAAM_IRSR0, 1);
    CAAM_WRITE(CAAM_ORSR0, 1);

    /* set DECO watchdog to time out and flush jobs that cause the DECO to hang */
    CAAM_WRITE((CAAM_BASE | 0x0004), CAAM_READ(CAAM_BASE | 0x0004) | 0x40000000);

    /* start up RNG if not already started */
    if (caamInitRng(&caam) != 0) {
        INTERRUPT_Panic();
    }
}

void (*__ghsentry_bspuserinit_InitCAAM)(void) = &InitCAAM;

#endif /* INTEGRITY */
