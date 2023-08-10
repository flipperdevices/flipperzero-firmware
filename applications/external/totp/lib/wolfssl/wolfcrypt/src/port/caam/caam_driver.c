/* caam_driver.c
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

#if (defined(__INTEGRITY) || defined(INTEGRITY)) || \
    (defined(__QNX__) || defined(__QNXNTO__))

#if defined(__QNX__) || defined(__QNXNTO__)
    #include <sys/mman.h>
    #ifdef __aarch64__
        #include <aarch64/inout.h>
    #else
        #include <hw/inout.h>
    #endif
    #include <sys/iofunc.h>
    #include <sys/neutrino.h>

    #include <wolfssl/wolfcrypt/port/caam/caam_qnx.h>
#endif

#include <wolfssl/wolfcrypt/port/caam/caam_driver.h>
#include <wolfssl/wolfcrypt/port/caam/caam_error.h>

#include <string.h> /* for memcpy / memset */

struct JobRing {
    /* base address for job ring */
    CAAM_ADDRESS BaseAddr;

    /* physical address */
    CAAM_ADDRESS JobIn;
    CAAM_ADDRESS JobOut;
    CAAM_ADDRESS Desc;

    /* virtual address */
    void* VirtualIn;
    void* VirtualOut;
    void* VirtualDesc;
    Value   page;    /* page allocation for descriptor to use */
    CAAM_MUTEX jr_lock;
};

struct CAAM_DEVICE {
#if defined(__INTEGRITY) || defined(INTEGRITY)
    struct IODeviceVectorStruct caamVector;
    struct IODescriptorStruct   IODescriptorArray[BUFFER_COUNT];
    struct DescStruct           DescArray[DESC_COUNT];
    volatile Value              InterruptStatus;
    CALL                        HandleInterruptCall;
#endif
    struct JobRing ring;
    CAAM_ADDRESS   baseAddr;     /* base address for CAAM */
    unsigned short vrs;          /* era version number of CAAM */
};

#define DRIVER_NAME "wolfSSL_CAAM_Driver"

static struct CAAM_DEVICE caam;

/* function declarations */
Error caamAddJob(DESCSTRUCT* desc);
Error caamDoJob(DESCSTRUCT* desc);



/******************************************************************************
  Internal CAAM Job Ring and partition functions
  ****************************************************************************/

#ifdef CAAM_DEBUG_MODE
/* runs a descriptor in debug mode */
static Error caamDebugDesc(struct DescStruct* desc)
{
    int z;
    int sz;
    unsigned int flag = 0x20000000;

    /* clear and set desc size */
    sz = desc->desc[0] & 0x0000007F;
    CAAM_WRITE(CAAM_DECORR, 1); /* ask for DECO permissions */
    printf("CAAM_DECORR = 0x%08X\n", CAAM_READ(CAAM_DECORR));
    printf("STATUS : 0x%08X\n", CAAM_READ(CAAM_DOOPSTA_MS));
    printf("CAAM STATUS : 0x%08X\n", CAAM_READ(0x0FD4));
    printf("DECO DRG (bit 32 is valid -- running) : 0x%08X\n", CAAM_READ(0x8E04));

    printf("Descriptor input :\n");
    /* write descriptor into descriptor buffer */
    for (z = 0; z < sz; z = z + 1) {
        CAAM_WRITE(CAAM_DODESB + (z*4), desc->desc[z]);
        printf("\t0x%08X\n", desc->desc[z]);
    }
    printf("\n");

    printf("command size = %d\n", sz);
    if (sz > 4) {
        flag |= 0x10000000;
    }

    CAAM_WRITE(CAAM_DODAR+4, desc->caam->ring.Desc);
    /* set WHL bit since we loaded the entire descriptor */
    CAAM_WRITE(CAAM_DOJQCR_MS, flag);

    printf("CAAM STATUS : 0x%08X\n", CAAM_READ(0x0FD4));
    printf("DECO DRG (bit 32 is valid -- running) : 0x%08X\n", CAAM_READ(0x8E04));

    /* DECO buffer */
    printf("DECO BUFFER [0x%08X]:\n", CAAM_READ(CAAM_DODAR+4));
    printf("\tSTATUS : 0x%08X\n", CAAM_READ(CAAM_DOOPSTA_MS));
    printf("\tJRSTAR_JR0 : 0x%08X\n", CAAM_READ(0x1044));
    for (z = 0; z < sz; z = z + 1) {
        printf("\t0x%08X\n", CAAM_READ(CAAM_DODESB + (z*4)));
    }


    //D0JQCR_LS
    printf("Next command to be executed = 0x%08X\n", CAAM_READ(0x8804));
    printf("Desc          = 0x%08X\n", desc->caam->ring.Desc);


    /* DECO buffer */
    printf("DECO BUFFER [0x%08X]:\n", CAAM_READ(CAAM_DODAR+4));
    printf("\tSTATUS : 0x%08X\n", CAAM_READ(CAAM_DOOPSTA_MS));
    printf("\tJRSTAR_JR0 : 0x%08X\n", CAAM_READ(0x1044));
    for (z = 0; z < sz; z = z + 1) {
        printf("\t0x%08X\n", CAAM_READ(CAAM_DODESB + (z*4)));
    }

    printf("Next command to be executed = 0x%08X\n", CAAM_READ(0x8804));
    printf("CAAM STATUS : 0x%08X\n", CAAM_READ(0x0FD4));
    while ((CAAM_READ(0x8E04) & 0x80000000U) != 0U) {
        printf("DECO DRG (bit 32 is valid -- running) : 0x%08X\n",
                CAAM_READ(0x8E04));
        sleep(1);
    }
    CAAM_WRITE(CAAM_DECORR, 0); /* free DECO */
    printf("done with debug job\n");
    return Success;
}
#endif /* CAAM_DEBUG_MODE */


#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
static void printSecureMemoryInfo()
{
    unsigned int SMVID_MS, SMVID_LS;

    printf("SMSTA = 0x%08X\n", CAAM_READ(caam.ring.BaseAddr + 0x0FB4));
    printf("SMPO  = 0x%08X\n", CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMPO));
    SMVID_MS = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMVID_MS);
    SMVID_LS = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMVID_LS);
    printf("\tNumber Partitions : %d\n", ((SMVID_MS >> 12) & 0xFU));
    printf("\tNumber Pages : %d\n", (SMVID_MS & 0x3FFU));
    printf("\tPage Size : 2^%d\n", ((SMVID_LS >> 16) & 0x7U));
}
#endif

/* flush job ring and reset */
static Error caamReset(void)
{
    int t = 100000; /* time out counter for flushing job ring */

    /* make sure interrupts are masked in JRCFGR0_LS register */
    CAAM_WRITE(caam.ring.BaseAddr + JRCFGR_JR,
        CAAM_READ(caam.ring.BaseAddr + JRCFGR_JR) | 1);

    /* flush and reset job rings using JRCR0 register */
    CAAM_WRITE(caam.ring.BaseAddr + JRCR_JR, 1);

    /* check register JRINTR for if halt is in progress */
    while (t > 0 && ((CAAM_READ(caam.ring.BaseAddr + JRINTR_JR) & 0x4)
        == 0x4)) {
        t = t - 1;
    }
    if (t == 0) {
        /*unrecoverable failure, the job ring is locked, up hard reset needed*/
        return -1;
    }

    /* now that flush has been done restart the job ring */
    t = 100000;
    CAAM_WRITE(caam.ring.BaseAddr + JRCR_JR, 1);
    while (t > 0 && ((CAAM_READ(caam.ring.BaseAddr + JRCR_JR) & 1) == 1)) {
        t = t - 1;
    }
    if (t == 0) {
        /*unrecoverable failure, reset bit did not return to 0 */
        return -1;
    }

    if (caam.vrs < 9) {
        /* reset most registers and state machines in CAAM using MCFGR register
           also reset DMA */
        CAAM_WRITE(caam.baseAddr + CAAM_RSTA, 0x90000000);

        /* DDR */
        CAAM_WRITE(caam.baseAddr + CAAM_DRR, 1);
    }

    return Success;
}


/* free the page and dealloc */
static Error caamFreePage(unsigned int page)
{
    /* owns the page can dealloc it */
    CAAM_WRITE(caam.ring.BaseAddr + CAAM_SM_CMD, (page << 16U) | 0x2U);
    while ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00004000) > 0 &&
        (CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00003000)  == 0) {
        CAAM_CPU_CHILL();
    }
    if ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00003000)  > 0) {
        /* error while deallocating page */
        WOLFSSL_MSG("error while deallocating page");
        return MemoryMapMayNotBeEmpty; /* PSP set on page or is unavailable */
    }
    WOLFSSL_MSG("free'd page");
    return Success;
}

/* free the partition and dealloc */
Error caamFreePart(unsigned int part)
{
    unsigned int status;

    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    printf("freeing partition %d\n", part);
    #endif
    CAAM_WRITE(caam.ring.BaseAddr + CAAM_SM_CMD, (part << 8U) | 0x3U);

    status = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS);
    while (((status & 0x00004000U) > 0U) && ((status & 0x00003000U) == 0U)) {
        CAAM_CPU_CHILL();
        status = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS);
    }

    if (((status & 0x00003000U) > 0U) || ((status & 0x0000C000U) > 0U)) {
        /* error while deallocating page */
        WOLFSSL_MSG("error while deallocating partition");
        return MemoryMapMayNotBeEmpty; /* PSP set on page or is unavailable */
    }
    WOLFSSL_MSG("free'd partition");
    return Success;
}


/* find all partitions we own and free them */
static Error caamFreeAllPart()
{
    unsigned int SMPO;
    unsigned int i;

    WOLFSSL_MSG("Free all partitions");
    SMPO = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMPO);
    for (i = 0; i < 15U; i = i + 1U) {
        if ((SMPO & (0x3U << (i * 2U))) == (0x3U << (i * 2U))) {
            caamFreePart(i);
        }
    }

    return 0;
}


/* search through the partitions to find an unused one
 * returns negative value on failure, on success returns 0 or greater
 */
int caamFindUnusedPartition()
{
    unsigned int SMPO;
    unsigned int i;
    int ret = -1;

    SMPO = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMPO);
    for (i = 0; i < 15U; i = i + 1) {
        if ((SMPO & (0x3U << (i * 2U))) == 0U) {
            ret = (int)i;
            break;
        }
    }

    return ret;
}


/* flag contains how the partition is set i.e CSP flag and read/write access
 *      it also contains if locked
 */
static Error caamCreatePartition(unsigned int* page, unsigned int par,
        unsigned int flag)
{
    int testPage;
    unsigned int status;

    /* check ownership of partition */
    status = CAAM_READ(caam.ring.BaseAddr + CAAM_SM_SMPO);
    if ((status & (0x3U << (par * 2))) > 0) {
        if ((status & (0x3U << (par * 2))) == (0x3U << (par * 2))) {
            WOLFSSL_MSG("we own this partition!");
        }
        else {
            return MemoryMapMayNotBeEmpty;
        }
    }
    CAAM_WRITE(caam.ring.BaseAddr + CAAM_SMAPR + (par * 16), flag);

    /* dealloc page if we own it */
    for (testPage = 0; testPage < 16; testPage++) {
        CAAM_WRITE(caam.ring.BaseAddr + CAAM_SM_CMD, (testPage << 16) | 0x5);
        while ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00004000) > 0
            && (CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00003000)
            == 0) {
            CAAM_CPU_CHILL();
        }
        if ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x000000C0) ==
            0xC0) {
            if (caamFreePage(testPage) != Success) {
                continue;
            }
        }
        else if ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x000000C0)
            == 0x00) {
            WOLFSSL_MSG("testPage available and un-owned");
        }
        else {
            WOLFSSL_MSG("we don't own the testPage...");
            continue;
        }

        CAAM_WRITE(caam.ring.BaseAddr + CAAM_SM_CMD, (testPage << 16) |
            (par << 8) | 0x1);
        /* wait for alloc cmd to complete */
        while ((CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00004000) > 0 &&
           (CAAM_READ(caam.ring.BaseAddr + CAAM_SM_STATUS) & 0x00003000)  == 0) {
            CAAM_CPU_CHILL();
        }
        break;
    }
    if (testPage == 16)
        return -1;
    *page=testPage;

    return Success;
}


/* return a partitions physical address on success, returns 0 on fail */
CAAM_ADDRESS caamGetPartition(unsigned int part, int partSz, unsigned int flag)
{
    int err;

    (void)flag; /* flag is for future changes to flag passed when creating */

    /* create and claim the partition */
    err = caamCreatePartition(&part, part, CAAM_SM_CSP | CAAM_SM_SMAP_LOCK |
                CAAM_SM_CSP | CAAM_SM_ALL_RW);
    if (err != Success) {
        WOLFSSL_MSG("Error creating partitions for secure ecc key");
        return 0;
    }

    return (CAAM_ADDRESS)(CAAM_PAGE + (part << 12));
}


/* Gets the status of a job. Returns CAAM_WAITING if no output jobs ready to be
 * read.
 * If no jobs are done then return CAAM_WAITING
 * If jobs are done but does not match desc then return NoActivityReady
 * Status holds the error values if any */
static Error caamGetJob(struct CAAM_DEVICE* dev, unsigned int* status)
{
    CAAM_ADDRESS baseAddr;
    unsigned int reg;
    if (status) {
        *status = 0;
    }

    baseAddr = caam.ring.BaseAddr;

#ifdef CAAM_DEBUG_MODE
    (void)dev;
    return Success;
#endif

    /* Check number of done jobs in output list */
    reg = CAAM_READ(baseAddr + CAAM_ORJAR);
#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    printf("number of jobs in output list = 0x%08X\n", reg);
#endif
    if ((reg & 0x000003FF) > 0) {
        unsigned int *pt;

        if (CAAM_ADR_SYNC(caam.ring.VirtualOut, (2 * CAAM_JOBRING_SIZE *
                        sizeof(unsigned int))) != 0) {
            return -1;
        }

        /* sanity check on job out */
        pt = (unsigned int*)caam.ring.VirtualOut;
        if (pt[0] != (unsigned int)caam.ring.Desc) {
        #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
            printf("Job found %08X does not match expected job %08X\n",
                    pt[0], (unsigned int)caam.ring.Desc);
        #endif
            return -1;
        }
        *status = pt[1];

        /* increment jobs removed */
    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
        printf("\tjob 0x%08X done - result 0x%08X\n", pt[0], pt[1]);
        printf("removing job from list\n");
        fflush(stdout);
    #endif
        CAAM_WRITE(baseAddr + CAAM_ORJRR, 1);
    }
    else {
        /* check if the CAAM is idle and not processing any descriptors */
        if ((CAAM_READ(baseAddr + 0x0FD4) & 0x00000002) == 2 /* idle */
        && (CAAM_READ(baseAddr + 0x0FD4) & 0x00000001) == 0) {
            WOLFSSL_MSG("caam is idle.....");
            return NoActivityReady;
        }
        return CAAM_WAITING;
    }
    (void)dev;

    CAAM_WRITE(baseAddr + JRCFGR_JR, 0);
    if (*status == 0) {
        return Success;
    }
    return Failure;
}


/* instantiate RNG and create JDKEK, TDKEK, and TDSK key */
#define WC_RNG_START_SIZE 6
static unsigned int wc_rng_start[WC_RNG_START_SIZE] = {
    CAAM_HEAD | 0x00000006,
    CAAM_OP | CAAM_CLASS1 | CAAM_RNG | 0x00000004, /* Instantiate RNG handle 0
                                                      with TRNG */
    CAAM_JUMP | 0x02000001,  /* wait for Class1 RNG and jump to next cmd */
    CAAM_LOAD | 0x00880004,  /* Load to clear written register */
    0x00000001, /* reset done interrupt */
    CAAM_OP | CAAM_CLASS1 | CAAM_RNG | 0x00001000   /* Generate secure keys */
};


/* Initialize CAAM RNG
 * returns 0 on success */
int caamInitRng(struct CAAM_DEVICE* dev);
int caamInitRng(struct CAAM_DEVICE* dev)
{
    DESCSTRUCT desc;
    unsigned int reg, entropy_delay;
    int ret = 0, i;

    /* set up the job description for RNG initialization */
    memset(&desc, 0, sizeof(DESCSTRUCT));
    desc.desc[desc.idx++] = CAAM_HEAD; /* later will put size to header*/
    for (i = 1; i < WC_RNG_START_SIZE; i++) {
        desc.desc[desc.idx++] = wc_rng_start[i];
    }
    desc.caam = dev;

    /* Attempt to start the RNG, first trying the fastest entropy delay value
     * and increasing it after each failed attempt until either a success is hit
     * or the max delay value is.
     */
    for (entropy_delay = CAAM_ENT_DLY; entropy_delay <= CAAM_ENT_DLY_MAX;
            entropy_delay = entropy_delay + CAAM_ENT_DLY_INCREMENT) {

        /* Set up use of the TRNG for seeding wolfSSL HASH-DRBG */
        /* check out the status and see if already setup */
        CAAM_WRITE(caam.baseAddr + CAAM_RTMCTL, CAAM_PRGM);
        CAAM_WRITE(caam.baseAddr + CAAM_RTMCTL,
                CAAM_READ(caam.baseAddr + CAAM_RTMCTL) | CAAM_RTMCTL_RESET);

        /* Set up reading from TRNG */
        CAAM_WRITE(caam.baseAddr + CAAM_RTMCTL,
                CAAM_READ(caam.baseAddr + CAAM_RTMCTL) | CAAM_TRNG);

        /* Set up delay for TRNG
         * Shift left with RTSDCTL because 0-15 is for sample number
         * Also setting the max and min frequencies */
        CAAM_WRITE(caam.baseAddr + CAAM_RTSDCTL,
                (entropy_delay << 16) | CAAM_ENT_SAMPLE);
        CAAM_WRITE(caam.baseAddr + CAAM_RTFRQMIN,
                entropy_delay >> CAAM_ENT_MINSHIFT);
        CAAM_WRITE(caam.baseAddr + CAAM_RTFRQMAX,
                entropy_delay << CAAM_ENT_MAXSHIFT);

    #ifdef WOLFSSL_CAAM_PRINT
        printf("Attempt with entropy delay set to %d\n", entropy_delay);
        printf("Min delay of %d and max of %d\n",
                entropy_delay >> CAAM_ENT_MINSHIFT,
                entropy_delay << CAAM_ENT_MAXSHIFT);
    #endif

        /* Set back to run mode and clear RTMCL error bit */
        reg = CAAM_READ(caam.baseAddr + CAAM_RTMCTL) & (~CAAM_PRGM);
        CAAM_WRITE(caam.baseAddr + CAAM_RTMCTL, reg);
        reg = CAAM_READ(caam.baseAddr + CAAM_RTMCTL);
        reg |= CAAM_CTLERR;
        CAAM_WRITE(caam.baseAddr + CAAM_RTMCTL, reg);

        /* check out the status and see if already setup */
        reg = CAAM_READ(caam.baseAddr + CAAM_RDSTA);
        if (((reg >> 16) & 0xF) > 0) {
            WOLFSSL_MSG("RNG is in error state, resetting");
            caamReset();
        }

        if (reg & (1U << 30)) {
            WOLFSSL_MSG("JKDKEK rng was setup using a non deterministic key");
            return 0;
        }

        do {
            ret = caamDoJob(&desc);
        } while (ret == CAAM_WAITING);

        /* if this entropy delay frequency succeeded then break out, otherwise
         * try again with increasing the delay value */
        if (ret == Success) {
            WOLFSSL_MSG("Init RNG success");
            break;
        }
        WOLFSSL_MSG("Increasing entropy delay");
    }

    if (ret == Success)
        return 0;
    return -1;
}


/* Take in a descriptor and add it to the job list */
Error caamAddJob(DESCSTRUCT* desc)
{
    CAAM_ADDRESS baseAddr;

    /* clear and set desc size */
    desc->desc[0] &= 0xFFFFFF80;
    desc->desc[0] += desc->idx + (desc->startIdx << 16);

    CAAM_LOCK_MUTEX(&caam.ring.jr_lock);
    baseAddr = caam.ring.BaseAddr;

    /* check input slot is available and then add */
    if (CAAM_READ(baseAddr + CAAM_IRSAR_JR) > 0) {
        int i;
        unsigned int *pt;

        pt = (unsigned int*)caam.ring.VirtualDesc;
    #if defined(WOLFSSL_CAAM_PRINT)
        printf("Number of input ring slots available is %d\n",
            CAAM_READ(baseAddr + CAAM_IRSAR_JR));
        printf("Doing Job :\n");
    #endif
        for (i = 0; i < desc->idx; i = i + 1) {
            pt[i] = desc->desc[i];
    #if defined(WOLFSSL_CAAM_PRINT)
            printf("\tCMD %02d [%p] = 0x%08X\n", i+1, pt + i,
                  desc->desc[i]);
    #endif
        }

        pt    = (unsigned int*)caam.ring.VirtualIn;
        pt[0] = (unsigned int)caam.ring.Desc;

        /* Sync both the virtual in and the descriptor */
        if (CAAM_ADR_SYNC(caam.ring.VirtualIn,
                    (CAAM_JOBRING_SIZE * sizeof(unsigned int)) +
                    (desc->idx * sizeof(unsigned int))) != 0) {
            CAAM_UNLOCK_MUTEX(&caam.ring.jr_lock);
            return -1;
        }

    #ifdef CAAM_DEBUG_MODE
        caamDebugDesc(desc);
    #else
        #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
        printf("started job 0x%08X done\n", (unsigned int)caam.ring.Desc);
        #endif
        CAAM_WRITE(baseAddr + CAAM_IRJAR0, 0x00000001);
    #endif
    }
    else {
        CAAM_UNLOCK_MUTEX(&caam.ring.jr_lock);
        return CAAM_WAITING;
    }
    CAAM_UNLOCK_MUTEX(&caam.ring.jr_lock);
    return Success;
}


/* Synchronous job completion, add it to job queue and wait till finished */
Error caamDoJob(DESCSTRUCT* desc)
{
    Error ret;
    unsigned int status;

    ret = caamAddJob(desc);
    if (ret != Success) {
        return ret;
    }

    do {
        ret = caamGetJob(desc->caam, &status);
        CAAM_CPU_CHILL();
    } while (ret == CAAM_WAITING);

    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    printf("job status = 0x%08X, ret = %d\n", status, ret);
    #endif

    /* try to reset after error */
    if (status != 0 || ret != Success) {
        /* consider any job ring errors as fatal, and try reset */
        if (caamParseJRError(CAAM_READ(caam.ring.BaseAddr + JRINTR_JR)) != 0) {
            caamReset();
        }
        caamParseError(status);
        return ret;
    }

    return Success;
}


/******************************************************************************
  CAAM Blob Operations
  ****************************************************************************/

/* limit on size due to size of job ring being 64 unsigned int's */
int caamBlob(DESCSTRUCT* desc)
{
    void *vaddrOut, *vaddr, *keymod;
    Error err;
    unsigned int keyType = 0x00000C08; /* default red */
    unsigned int i = 0;
    int inputSz;
    int outputSz;

    if (desc->idx + 3 > MAX_DESC_SZ) {
        return Failure;
    }

    /* doing black blobs */
    if (desc->state) {
        WOLFSSL_MSG("making a black blob");
        keyType = 0x00000010;
    }

    desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS2 | keyType);

    /* add key modifier */
    keymod = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(keymod, desc->buf[i].dataSz);
    i++;

    inputSz = desc->buf[i].dataSz;
    if (desc->state && (desc->type == CAAM_BLOB_ENCAP)) {
        /* black keys with CCM have mac at the end */
        inputSz += BLACK_KEY_MAC_SZ;
    }

    vaddr = CAAM_ADR_MAP(desc->buf[i].data, inputSz, 1);

    /* add input */
    desc->desc[desc->idx++] = CAAM_SEQI + desc->buf[i].dataSz;
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr, inputSz);
    i++;
    desc->outputIdx = i;

    outputSz = desc->buf[i].dataSz;
    if (desc->state && (desc->type == CAAM_BLOB_DECAP)) {
        /* black keys with CCM have mac at the end */
        outputSz += BLACK_KEY_MAC_SZ;
    }
    vaddrOut = CAAM_ADR_MAP(desc->buf[i].data, outputSz, 0);

    /* add output */
    desc->desc[desc->idx++] = CAAM_SEQO + desc->buf[i].dataSz;
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddrOut, outputSz);
    if (desc->idx + 1 > MAX_DESC_SZ) {
        return Failure;
    }
    desc->desc[desc->idx] = CAAM_OP |  CAAM_OPID_BLOB | desc->type;

    if (desc->state) {
        desc->desc[desc->idx] |= 0x104; /* EKT and Black_key (key is covered) */
    }
    desc->idx++;
    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    CAAM_ADR_UNMAP(keymod, desc->buf[0].data, desc->buf[0].dataSz, 0);
    CAAM_ADR_UNMAP(vaddr, desc->buf[1].data, inputSz, 0);
    CAAM_ADR_UNMAP(vaddrOut, desc->buf[2].data, outputSz, 1);

    return err;
}


/******************************************************************************
  CAAM AES Operations
  ****************************************************************************/

static void caamAddFIFOLPhy(DESCSTRUCT* desc, unsigned int in, int inSz,
    unsigned int flush)
{
    /* if size is larger than short type then use extended length option */
    if (inSz > 0xFFFF) {
        desc->desc[desc->idx++] = (CAAM_FIFO_L | flush | FIFOS_EXT |
                                   CAAM_CLASS1 | FIFOL_TYPE_MSG);
        desc->desc[desc->idx++] = in;
        desc->desc[desc->idx++] = inSz;
    }
    else {
        desc->desc[desc->idx++] = (CAAM_FIFO_L | flush |
                                   CAAM_CLASS1 | FIFOL_TYPE_MSG) + inSz;
        desc->desc[desc->idx++] = in;
    }
}


static void caamAddFIFOL(DESCSTRUCT* desc, void* in, int inSz,
    unsigned int flush)
{
    caamAddFIFOLPhy(desc, CAAM_ADR_TO_PHYSICAL(in, inSz), inSz, flush);
}


static void caamAddFIFOSPhy(DESCSTRUCT* desc, unsigned int out, int outSz)
{
    /* if size is larger than short type then use extended length option */
    if (outSz > 0xFFFF) {
        desc->desc[desc->idx++] = CAAM_FIFO_S | FIFOS_TYPE_MSG | FIFOS_EXT;
        desc->desc[desc->idx++] = out;
        desc->desc[desc->idx++] = outSz;
    }
    else {
        desc->desc[desc->idx++] = (CAAM_FIFO_S | FIFOS_TYPE_MSG) + outSz;
        desc->desc[desc->idx++] = out;
    }
}


static void caamAddFIFOS(DESCSTRUCT* desc, void* out, int outSz)
{
    caamAddFIFOSPhy(desc, CAAM_ADR_TO_PHYSICAL(out, outSz), outSz);
}


int caamAesCmac(DESCSTRUCT* desc, int sz, unsigned int args[4])
{
    Error err;
    unsigned int keySz;
    unsigned int macSz = 0;
    void *vaddr[4] = {0};
    unsigned int vidx = 0;
    unsigned int ctx;
    unsigned int isBlackKey;
    int i;

    isBlackKey = args[2];
    keySz = args[1];

    /* Get CTX physical address */
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[1].data, desc->buf[1].dataSz, 1);
    ctx = CAAM_ADR_TO_PHYSICAL(vaddr[vidx], desc->buf[1].dataSz);
    vidx++;

    /* LOAD KEY */
    desc->desc[desc->idx] = (CAAM_KEY | CAAM_CLASS1 | CAAM_NWB) + keySz;
    if (isBlackKey) {
        desc->desc[desc->idx] |= CAAM_LOAD_BLACK_KEY;
        macSz = BLACK_KEY_MAC_SZ;
    }
    desc->idx++;
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[0].data, desc->buf[0].dataSz + macSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
            desc->buf[0].dataSz + macSz);
    DEBUG_PRINT_ARRAY(vaddr[vidx], keySz, "AES CMAC Key");
    vidx++;

    /* Load in CTX only when not initialization */
    if ((desc->state & CAAM_ALG_INIT) == 0) {
        int ofst = 0;
        desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS1 | ofst) +
            desc->buf[1].dataSz;
        desc->desc[desc->idx++] = ctx;
        DEBUG_PRINT_ARRAY(vaddr[0], 32, "AES CMAC CTX");
    }

    /* add protinfo to operation command */
    desc->desc[desc->idx++] = CAAM_OP | CAAM_CLASS1 | desc->type | desc->state;

    /* add in all input buffers */
    for (i = 2; i < sz; i = i + 1) {
        desc->desc[desc->idx] = (CAAM_FIFO_L | CAAM_CLASS1 | FIFOL_TYPE_MSG)
            + desc->buf[i].dataSz;
        if (i+1 == sz) {
            /* this is the last input buffer, signal the HW with LC1 bit */
            desc->desc[desc->idx] |= FIFOL_TYPE_LC1;
        }
        desc->idx++;

        vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
        DEBUG_PRINT_ARRAY(vaddr[vidx], desc->buf[i].dataSz, "AES CMAC Input");
        vidx++;
    }

    /* if there is no input buffers than add in a single FIFO LOAD to kick off
     * the operation */
    if (sz == 2) { /* only key and ctx buffer */
        desc->desc[desc->idx++] = CAAM_FIFO_L | FIFOL_TYPE_LC1 | CAAM_CLASS1 |
            FIFOL_TYPE_MSG;
        vaddr[vidx] = CAAM_ADR_MAP(0, 0, 0);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx], 0);
        vidx++;
    }

    desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 | 32;
    desc->desc[desc->idx++] = ctx;

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);
    DEBUG_PRINT_ARRAY(vaddr[0], 32, "AES CMAC CTX");

    vidx = 0;
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[1].data, desc->buf[1].dataSz, 1);
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[0].data, desc->buf[0].dataSz + macSz, 0);
    if (sz == 2) {
        CAAM_ADR_UNMAP(vaddr[vidx], 0, 0, 0);
    }
    else {
        for (i = 2; i < sz; i = i + 1) { /* unmap the input buffers */
            CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data,
                    desc->buf[i].dataSz, 0);
        }
    }

    return err;
}


int caamAead(DESCSTRUCT* desc, CAAM_BUFFER* buf, unsigned int args[4])
{
    int fifol = 0; /* last index for FIFO */
    Value ofst = 0;
    Error err;
    int ivSz = 0;
    void *iv, *key, *in, *out, *tag;
    int keySz;
    int tagSz;
    int inSz;
    int outSz;
    int idx = 0;
    void* aadCtxBuf = NULL;
    int state = CAAM_ALG_INITF; /* single shot mode */
    unsigned int aadSz;

    if (desc->state != CAAM_ENC && desc->state != CAAM_DEC) {
        return CAAM_ARGS_E;
    }

    keySz = buf[idx].Length;
    if (keySz != 16 && keySz != 24 && keySz != 32) {
        WOLFSSL_MSG("Bad AES key size found");
        return CAAM_ARGS_E;
    }
    aadSz = (args[0] >> 16) & 0xFFFF;

    /* map and copy over key */
    key = (void*)buf[idx].TheAddress;
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1 | CAAM_NWB) + keySz;
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(key, keySz);
    DEBUG_PRINT_ARRAY(key, keySz, "AES AEAD Key");
    idx++;

    /* write operation, in the case of decryption add IVC to check tag */
    desc->desc[desc->idx] = CAAM_OP | CAAM_CLASS1 | desc->type |
             state | desc->state;
    if (desc->state == CAAM_DEC) {
        desc->desc[desc->idx] |= CAAM_ALG_IVC;
    }
    desc->idx++;

    /* get IV if needed by algorithm */
    switch (desc->type) {
        case CAAM_AESGCM:
        {
            ivSz = buf[idx].Length;
            iv   = (void*)buf[idx].TheAddress;
            fifol = desc->idx;
            desc->desc[desc->idx++] = (CAAM_FIFO_L | CAAM_CLASS1 |
                FIFOL_TYPE_FC1 | FIFOL_TYPE_IV) + ivSz;
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(iv, ivSz);
            idx++;
         }
        break;

        case CAAM_AESCCM:
        {
            ivSz = buf[idx].Length;
            iv   = (void*)buf[idx].TheAddress;
            desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS1 | ofst) +
                ivSz;
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(iv, ivSz);
            idx++;
         }
         break;

        default:
            WOLFSSL_MSG("Mode of AES not implemented");
            return CAAM_ARGS_E;
    }
    DEBUG_PRINT_ARRAY(iv, ivSz, "AES AEAD IV");

    /* get input */
    inSz = buf[idx].Length;
    in   = (void*)buf[idx].TheAddress;
    DEBUG_PRINT_ARRAY(in, inSz, "AES AEAD Input");
    idx++;

    /* set output */
    outSz = buf[idx].Length;
    out   = (void*)buf[idx].TheAddress;
    idx++;

    /* TAG in/out */
    tagSz = buf[idx].Length;
    tag   = (void*)buf[idx].TheAddress;
    idx++;

    /* set aad if present */
    if (aadSz > 0) {
        /* special AAD size setup of context register for CCM mode */
        if (desc->type == CAAM_AESCCM) {
            unsigned char* pt;

            aadCtxBuf = CAAM_ADR_MAP(0, 4, 0);
            pt = (unsigned char*)aadCtxBuf;
            memset(pt, 0, 4);
            if (aadSz <= 0xFEFF) {
                pt[0] = (aadSz & 0xFF00) >> 8;
                pt[1] = (aadSz & 0x00FF);
                desc->desc[desc->idx++] = (CAAM_FIFO_L | CAAM_CLASS1 |
                    FIFOL_TYPE_AAD) + 2;
            }
            else {
                pt[0] = (aadSz >> 24) & 0xFF;
                pt[1] = (aadSz >> 16) & 0xFF;
                pt[2] = (aadSz >> 8 ) & 0xFF;
                pt[3] = aadSz & 0xFF;
                fifol = desc->idx;
                desc->desc[desc->idx++] = (CAAM_FIFO_L | CAAM_CLASS1 |
                    FIFOL_TYPE_AAD) + 4;
            }
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(aadCtxBuf, 4);
            CAAM_ADR_SYNC(aadCtxBuf, 4);
        }

        /* Load in AAD */
        fifol = desc->idx;
        desc->desc[desc->idx++] = (CAAM_FIFO_L | CAAM_CLASS1 | FIFOL_TYPE_FC1 |
            FIFOL_TYPE_AAD) + aadSz;
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL((void*)buf[idx].TheAddress,
            buf[idx].Length);
        DEBUG_PRINT_ARRAY((void*)buf[idx].TheAddress, aadSz, "AES AEAD AAD");
        idx++;
    }

    /* set input/output in descriptor */
    if (outSz > 0) {
        caamAddFIFOS(desc, out, outSz);
    }

    if (inSz > 0) {
        fifol = desc->idx;
        caamAddFIFOL(desc, in, inSz, FIFOL_TYPE_FC1);
    }

    if (desc->state == CAAM_DEC) {
        fifol = desc->idx;
        desc->desc[desc->idx++] = (CAAM_FIFO_L | FIFOL_TYPE_FC1 |
                                   CAAM_CLASS1 | FIFOL_TYPE_IVC) + tagSz;
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(tag, tagSz);
    }

    /* remove FC1 and set LC1 for last FIFO load */
    desc->desc[fifol] &= ~FIFOL_TYPE_FC1;
    desc->desc[fifol] |= FIFOL_TYPE_LC1;

    /* store updated IV / MAC */
    if (desc->state == CAAM_ENC) {
    switch (desc->type) {
        case CAAM_AESGCM:
            desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 | tagSz;
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(tag, tagSz);
            break;

        case CAAM_AESCCM:
            desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 |
                (32 << 8) | tagSz;
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(tag, tagSz);
            break;
    }
    }

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    if (aadCtxBuf != NULL)
        CAAM_ADR_UNMAP(aadCtxBuf, 0, 4, 0);

    DEBUG_PRINT_ARRAY(tag, tagSz, "AES AEAD Tag Out");
    DEBUG_PRINT_ARRAY(out, outSz, "AES AEAD Output");
    return err;
}


static int caamAesInternal(DESCSTRUCT* desc,
    unsigned int keyPhy, int keySz,
    unsigned int ivPhy,  int ivSz,
    unsigned int inPhy,  int inSz,
    unsigned int outPhy, int outSz)
{
    Value ofst = 0;
    Error err;
    int state = CAAM_ALG_UPDATE;

    if (desc->state != CAAM_ENC && desc->state != CAAM_DEC) {
        return CAAM_ARGS_E;
    }

    if (keySz != 16 && keySz != 24 && keySz != 32) {
        WOLFSSL_MSG("Bad AES key size found");
        return CAAM_ARGS_E;
    }

    /* map and copy over key */
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1 | CAAM_NWB) + keySz;
    desc->desc[desc->idx++] = keyPhy;

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

            if (ivSz != maxSz) {
                WOLFSSL_MSG("Invalid AES-CBC IV size\n");
                return CAAM_ARGS_E;
            }

            desc->desc[desc->idx++] = (CAAM_LOAD_CTX | CAAM_CLASS1 | ofst) +
                    maxSz;
            desc->desc[desc->idx++] = ivPhy;
         }
         break;

        default:
            WOLFSSL_MSG("Mode of AES not implemented");
            return CAAM_ARGS_E;
    }

    /* write operation */
    desc->desc[desc->idx++] = CAAM_OP | CAAM_CLASS1 | desc->type |
             state | desc->state;

    /* set input/output in descriptor */
    caamAddFIFOLPhy(desc, inPhy, inSz, FIFOL_TYPE_LC1);
    caamAddFIFOSPhy(desc, outPhy, outSz);

    /* store updated IV */
    switch (desc->type) {
        case CAAM_AESCBC:
        case CAAM_AESCTR:
        if (ivSz > 0) {
            desc->desc[desc->idx++] = CAAM_STORE_CTX | CAAM_CLASS1 | ofst | 16;
            desc->desc[desc->idx++] = ivPhy;
        }
        break;
    }

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    return err;
}


/* Scattered memory, does a mapping for each variable. Less performant than
 * caamAesCombined.
 * AES operations follow the buffer sequence of KEY -> (IV) -> Input -> Output
 */
int caamAes(DESCSTRUCT* desc, CAAM_BUFFER* buf, unsigned int args[4])
{
    void *iv = NULL, *key, *in, *out;
    int ivSz = 0, keySz, inSz, outSz;
    int idx = 0;

    unsigned int keyPhy = 0, inPhy = 0, outPhy = 0, ivPhy = 0;

    /* map and copy over key */
    key    = (void*)buf[idx].TheAddress;
    keySz  = buf[idx].Length;
    keyPhy = CAAM_ADR_TO_PHYSICAL(key, keySz);
    idx++;

    /* get IV if needed by algorithm */
    switch (desc->type) {
        case CAAM_AESECB:
            break;

        case CAAM_AESCTR:
        case CAAM_AESCBC:
        {
            ivSz  = buf[idx].Length;
            iv    = (void*)buf[idx].TheAddress;
            ivPhy = CAAM_ADR_TO_PHYSICAL(iv, ivSz);
            idx++;
         }
         break;

        default:
            WOLFSSL_MSG("Mode of AES not implemented");
            return CAAM_ARGS_E;
    }

    /* get input */
    inSz  = buf[idx].Length;
    in    = (void*)buf[idx].TheAddress;
    inPhy = CAAM_ADR_TO_PHYSICAL(in, inSz);
    idx++;

    /* set output */
    outSz  = buf[idx].Length;
    out    = (void*)buf[idx].TheAddress;
    outPhy = CAAM_ADR_TO_PHYSICAL(out, outSz);
    idx++;

    return caamAesInternal(desc, keyPhy, keySz, ivPhy, ivSz, inPhy, inSz,
            outPhy, outSz);
}


/* When a single buffer is used [key] [in] [iv] [out] for optimization
 * phyMem is non 0 when the physical memory location is already known
 */
int caamAesCombined(DESCSTRUCT* desc, CAAM_BUFFER* buf, unsigned int args[4],
    unsigned int phyMem)
{
    int idx = 0;
    unsigned int keyPhy;
    int keySz, inSz, ivSz = 0, outSz;
    void* pt;

    keySz = buf[idx].Length;
    pt    = (void*)buf[idx].TheAddress;
    idx++;

    /* get IV if needed by algorithm */
    switch (desc->type) {
        case CAAM_AESECB:
            break;

        case CAAM_AESCTR:
        case CAAM_AESCBC:
        {
            ivSz = buf[idx].Length;
            idx++;
         }
         break;

        default:
            WOLFSSL_MSG("Mode of AES not implemented");
            return CAAM_ARGS_E;
    }

    /* get input */
    inSz = buf[idx].Length;
    idx++;

    /* set output */
    outSz = buf[idx].Length;
    idx++;

    if (phyMem != 0) {
        keyPhy = phyMem;
    }
    else {
        keyPhy = CAAM_ADR_TO_PHYSICAL(pt, keySz + inSz + ivSz + outSz);
    }

    return caamAesInternal(desc, keyPhy, keySz,
            keyPhy + keySz + inSz,ivSz,            /* IV */
            keyPhy + keySz, inSz,                  /* IN buffer */
            keyPhy + keySz + inSz + ivSz, outSz);  /* OUT buffer */
}


/* ECDSA generate black key
 *
 * return Success on success. All other return values are considered a fail
 *         case.
 */
int caamECDSAMake(DESCSTRUCT* desc, CAAM_BUFFER* buf, unsigned int args[4])
{
    Error err;
    unsigned int part = 0;
    unsigned int isBlackKey = 0;
    unsigned int pdECDSEL   = 0;
    unsigned int phys;
    void *vaddr[2];

    if (args != NULL) {
        isBlackKey = args[0];
        pdECDSEL   = args[1];
    }
    vaddr[0] = NULL;
    vaddr[1] = NULL;

    desc->desc[desc->idx++] = pdECDSEL;
    if (isBlackKey == CAAM_BLACK_KEY_SM) {
        unsigned char* pt;

        /* create secure partition for private key out */
        part = caamFindUnusedPartition();
        if ((int)part < 0) {
            WOLFSSL_MSG("error finding an unused partition for new key");
            return -1;
        }

        /* create and claim the partition */
        err = caamCreatePartition(&part, part, CAAM_SM_CSP | CAAM_SM_SMAP_LOCK |
                CAAM_SM_CSP | CAAM_SM_ALL_RW);
        if (err != Success) {
            WOLFSSL_MSG("error creating partition for secure ecc key");
            return -1;
        }

        /* map secure partition to virtual address */
        phys = (CAAM_PAGE + (part << 12));
        pt = (unsigned char*)buf[0].TheAddress;
        pt[0] = (phys >> 24) & 0xFF;
        pt[1] = (phys >> 16) & 0xFF;
        pt[2] = (phys >> 8) & 0xFF;
        pt[3] = (phys) & 0xFF;
        desc->desc[desc->idx++] = phys;
    }
    else {
        vaddr[0] = CAAM_ADR_MAP(0, buf[0].Length, 0);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[0], buf[0].Length);

    }
    vaddr[1] = CAAM_ADR_MAP(0, buf[1].Length, 0);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[1], buf[1].Length);

    /* add protinfo to operation command */
    desc->startIdx = desc->idx;

    /* add operation command               OPTYPE        PROTOID */
    desc->desc[desc->idx] = CAAM_OP | CAAM_PROT_UNIDI | desc->type;
    if (isBlackKey == CAAM_BLACK_KEY_SM || isBlackKey == CAAM_BLACK_KEY_CCM) {
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESCCM;
    }
    if (isBlackKey == CAAM_BLACK_KEY_ECB) {
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESECB;
    }
    desc->desc[desc->idx++] |= CAAM_PKHA_ECC;

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    if (isBlackKey == CAAM_BLACK_KEY_SM) {
        /* store partition number holding black keys */
        if (err != Success)
            caamFreePart(part);
        else
            args[2] = part;
    }
    else {
        /* copy non black keys out to buffers */
        CAAM_ADR_UNMAP(vaddr[0], buf[0].TheAddress, buf[0].Length, 1);
    }
    CAAM_ADR_UNMAP(vaddr[1], buf[1].TheAddress, buf[1].Length, 1);

    return err;
}



/* ECDSA verify signature
 *
 * return Success on success. All other return values are considered a fail
 *         case.
 */
int caamECDSAVerify(DESCSTRUCT* desc, CAAM_BUFFER* buf, int sz,
        unsigned int args[4])
{
    unsigned int isBlackKey = 0;
    unsigned int pdECDSEL   = 0;
    unsigned int msgSz = 0;
    unsigned int vidx = 0;
    unsigned int L;
    int i = 0;
    Error err;
    void *vaddr[MAX_ECDSA_VERIFY_ADDR];

    if (args != NULL) {
        isBlackKey = args[0];
        pdECDSEL   = args[1];
        msgSz      = args[2];
    }

    if (sz > MAX_ECDSA_VERIFY_ADDR) {
       return -1;
    }

    if (pdECDSEL == 0) {
        return -1;
    }
    else {
        L = args[3]; /* keysize */
        desc->desc[desc->idx++] = pdECDSEL;
    }

    /* public key */
    if (isBlackKey != CAAM_BLACK_KEY_SM) {
        vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
        vidx = vidx + 1;
    }
    else {
        desc->desc[desc->idx++] = desc->buf[i].data;
    }
    i = i + 1;

    /* message */
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
    vidx = vidx + 1; i = i + 1;

    /* r */
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
    vidx = vidx + 1; i = i + 1;

    /* s */
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
    vidx = vidx + 1; i = i + 1;

    /* tmp buffer */
    vaddr[vidx] = CAAM_ADR_MAP(0, 2*L, 0);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx], 2*L);
    if (msgSz > 0) {
        desc->desc[desc->idx++] = msgSz;
    }

    /* add protinfo to operation command */
    desc->startIdx = desc->idx;

    /* add operation command               OPTYPE        PROTOID */
    desc->desc[desc->idx] = CAAM_OP | CAAM_PROT_UNIDI | desc->type;
    if (msgSz > 0) {
        desc->desc[desc->idx] |= CAAM_ECDSA_MESREP_HASHED;
    }
    desc->desc[desc->idx++] |= CAAM_PKHA_ECC;

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    vidx = 0; i = 0;
    if (!isBlackKey) {
        CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0);
    }
    i = i + 1;

    /* msg , r, s, tmp */
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0);
    i = i + 1;
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0);
    i = i + 1;
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0);
    i = i + 1;
    CAAM_ADR_UNMAP(vaddr[vidx++], 0, 2*L, 0);

    return err;
}


/* ECDSA generate signature
 *
 * return Success on success. All other return values are considered a fail
 *         case.
 */
int caamECDSASign(DESCSTRUCT* desc, int sz, unsigned int args[4])
{
    Error err;
    unsigned int isBlackKey = 0;
    unsigned int pdECDSEL   = 0;
    unsigned int msgSz = 0;
    unsigned int vidx = 0;
    int i = 0;
    void *vaddr[MAX_ECDSA_SIGN_ADDR];

    if ((args == NULL) || (sz > MAX_ECDSA_SIGN_ADDR)) {
        return -1;
    }

    isBlackKey = args[0];
    pdECDSEL   = args[1];
    msgSz      = args[2];
    if (pdECDSEL == 0) {
        return -1;
    }

    /* using parameters already in hardware */
    desc->desc[desc->idx++] = pdECDSEL;

    /* private key */
    if (isBlackKey != CAAM_BLACK_KEY_SM) {
        vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
    #if 0
        {
            unsigned int z; unsigned char* pt;
            printf("private :");
            pt = (unsigned char*)desc->buf[i].data;
            for (z = 0; z < desc->buf[i].dataSz; z++)
                printf("%02X", pt[z]);
            printf("\n");
        }
    #endif
        vidx++;
    }
    else {
        desc->desc[desc->idx++] = desc->buf[i].data;
    }
    i++;

    for (; i < sz; i = i + 1) {
        vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    #if 0
        {
            unsigned int z;
            unsigned char *pt = (unsigned char*)vaddr[vidx];
            printf("input index %d/%d\n", i, sz);
            for (z = 0; z < desc->buf[i].dataSz; z++)
                printf("%02X", pt[z]);
            printf("\n");
        }
    #endif
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
        vidx++;
    }

    desc->desc[desc->idx++] = msgSz;

    /* add protinfo to operation command */
    desc->startIdx = desc->idx;

    /* add operation command               OPTYPE        PROTOID */
    desc->desc[desc->idx] = CAAM_OP | CAAM_PROT_UNIDI | desc->type;
    if (isBlackKey == CAAM_BLACK_KEY_SM || isBlackKey == CAAM_BLACK_KEY_CCM) {
        /* set flag to use AES-CCM with black key */
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESCCM;
    }
    if (isBlackKey == CAAM_BLACK_KEY_ECB) {
        /* set flag to use AES-ECB with black key */
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESECB;
    }

    /* add protinfo to operation command */
    desc->desc[desc->idx++] |= CAAM_ECDSA_MESREP_HASHED | CAAM_PKHA_ECC;

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    vidx = 0; i = 0;
    if (isBlackKey != CAAM_BLACK_KEY_SM) {
        CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0);
    }
    i++;

    /* msg */
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 0); i++;

    /* copy out the r and s values */
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 1); i++;
    CAAM_ADR_UNMAP(vaddr[vidx++], desc->buf[i].data, desc->buf[i].dataSz, 1); i++;

    return err;
}


/* ECDH generate shared secret
 *
 * return Success on success. All other return values are considered a fail
 *         case.
 */
int caamECDSA_ECDH(DESCSTRUCT* desc, int sz, unsigned int args[4])
{
    Error err;
    unsigned int isBlackKey = 0;
    unsigned int peerBlackKey = 0;
    unsigned int pdECDSEL   = 0;
    unsigned int vidx = 0;
    int i = 0;
    void* vaddr[sz];

    if (args != NULL) {
        isBlackKey   = args[0];
        peerBlackKey = args[1];
        pdECDSEL     = args[2];
    }

    if (pdECDSEL == 0) {
        return -1;
    }
    else {
        /* using parameters already in hardware */
        desc->desc[desc->idx++] = pdECDSEL;
    }

    /* public key */
    if (peerBlackKey != CAAM_BLACK_KEY_SM) {
        vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
        desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
        DEBUG_PRINT_ARRAY((void*)desc->buf[i].data, desc->buf[i].dataSz,
                "ECDH Public Key");
        vidx++;
    }
    else {
        desc->desc[desc->idx++] = desc->buf[i].data;
    }
    i++;

    /* private key */
    if (isBlackKey != CAAM_BLACK_KEY_SM) {
        if (isBlackKey == CAAM_BLACK_KEY_CCM) {
            vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz +
                BLACK_KEY_MAC_SZ, 1);
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz + BLACK_KEY_MAC_SZ);
            DEBUG_PRINT_ARRAY((void*)desc->buf[i].data, desc->buf[i].dataSz +
                BLACK_KEY_MAC_SZ, "ECDH Private Key w/CCM ");
        }
        else {
            vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
            desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
            DEBUG_PRINT_ARRAY((void*)desc->buf[i].data, desc->buf[i].dataSz,
                    "ECDH Private Key");
        }
        vidx++;
    }
    else {
        desc->desc[desc->idx++] = desc->buf[i].data;
    }
    i++;

    /* shared output */
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
            desc->buf[i].dataSz);
    i++; vidx++;

    /* add protinfo to operation command */
    desc->startIdx = desc->idx;

    /* add operation command               OPTYPE        PROTOID */
    desc->desc[desc->idx] = CAAM_OP | CAAM_PROT_UNIDI | desc->type;
    if (isBlackKey == CAAM_BLACK_KEY_SM || isBlackKey == CAAM_BLACK_KEY_CCM) {
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESCCM;
    }
    if (isBlackKey == CAAM_BLACK_KEY_ECB) {
        desc->desc[desc->idx] |= CAAM_PKHA_ENC_PRI_AESECB;
    }

    /* add protinfo to operation command */
    desc->desc[desc->idx++] |= CAAM_PKHA_ECC;

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    vidx = 0; i = 0;
    if (pdECDSEL == 0) {
        /* unmap prime key */
        CAAM_ADR_UNMAP(vaddr[vidx], desc->buf[i].data, desc->buf[i].dataSz, 0);
        vidx++; i++;
    }

    if (peerBlackKey != 1) {
        CAAM_ADR_UNMAP(vaddr[vidx], desc->buf[i].data, desc->buf[i].dataSz, 0);
        vidx++;
    }
    i++;

    if (isBlackKey != CAAM_BLACK_KEY_SM) {
        CAAM_ADR_UNMAP(vaddr[vidx], desc->buf[i].data, desc->buf[i].dataSz, 0);
        vidx++;
    }
    i++;
    CAAM_ADR_UNMAP(vaddr[vidx], desc->buf[i].data, desc->buf[i].dataSz, 1);
    vidx++; i++;

    if (pdECDSEL == 0) {
        /* unmap A , B*/
        CAAM_ADR_UNMAP(vaddr[vidx], desc->buf[i].data, desc->buf[i].dataSz, 0);
        vidx++; i++;
    }
    return err;
}


/******************************************************************************
  IODevice Start, Transfer and Finish Buffer
  ****************************************************************************/
/* If Entropy is not ready then return CAAM_WAITING */
static int caamTRNG(unsigned char *out, int outSz)
{
    int sz = 0;

    CAAM_ADDRESS  reg; /* RTENT reg to read */
    unsigned char* local;
    int ofst = sizeof(unsigned int);

    /* Check ENT_VAL bit to make sure entropy is ready */
    if ((CAAM_READ(caam.baseAddr + CAAM_RTMCTL) & CAAM_ENTVAL) != CAAM_ENTVAL) {
        return CAAM_WAITING;
    }

    /* check state of TRNG */
    if ((CAAM_READ(caam.baseAddr + CAAM_RTSTATUS) & 0x0000FFFF) > 0) {
        WOLFSSL_MSG("RNG in a bad state");
        return Failure;
    }

    /* read entropy from RTENT registers */
    reg   = caam.baseAddr + CAAM_RTENT0;
    sz    = outSz;
    local = out;

    while (sz > 3 && reg <= caam.baseAddr + CAAM_RTENT_MAX) {
        unsigned int data = CAAM_READ(reg);
        *((unsigned int*)local) = data;
        reg    += ofst;
        local  += ofst;
        sz     -= ofst;
    }

    if (reg > CAAM_RTENT_MAX && sz > 0) {
        return -1;
    }

    /* handle non unsigned int size amount left over */
    if (sz > 0) {
        unsigned int tmp = CAAM_READ(reg);
        memcpy(local, (unsigned char*)&tmp, sz);
    }

    /* read the max RTENT to trigger new entropy generation */
    if (reg != CAAM_RTENT_MAX) {
        CAAM_READ(caam.baseAddr + CAAM_RTENT_MAX);
    }

    return Success;
}


/* out is the virtual address to store resulting RNG data in */
static int caamRNG(unsigned char *out, int outSz)
{
    DESCSTRUCT desc;
    int ret;

    /* set up the job description for RNG initialization */
    caamDescInit(&desc, 0, NULL, NULL, 0);
    desc.desc[desc.idx++] = CAAM_OP | CAAM_CLASS1 | CAAM_RNG;
    desc.desc[desc.idx++] = (CAAM_FIFO_S | FIFOS_TYPE_RNG) + outSz;
    desc.desc[desc.idx++] = CAAM_ADR_TO_PHYSICAL(out, outSz);

    do {
        ret = caamDoJob(&desc);
    } while (ret == CAAM_WAITING);

    return ret;
}


int caamEntropy(unsigned char *out, int outSz)
{
    if (caam.vrs < 9) {
        /* can access TRNG directly */
        return caamTRNG(out, outSz);
    }
    else {
        /* use CAAM RNG as source of entropy */
        return caamRNG(out, outSz);
    }
}


/* cover a plain text key and make it a black key */
int caamKeyCover(DESCSTRUCT* desc, int sz, unsigned int args[4])
{
    Error err;
    unsigned int vidx = 0;
    int i = 0;
    void* vaddr[2];

    (void)args;

    if (sz > 2) {
        return -1;
    }

    /* add input key */
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1) +
        desc->buf[i].dataSz;
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz, 1);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
                desc->buf[i].dataSz);
    vidx++;
    i++;

    /* add output */
    desc->desc[desc->idx++] = (CAAM_FIFO_S | CAAM_CLASS1 | args[0] |
        desc->state) + desc->buf[i].dataSz;
    vaddr[vidx] = CAAM_ADR_MAP(desc->buf[i].data, desc->buf[i].dataSz +
            BLACK_KEY_MAC_SZ, 0);
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[vidx],
            desc->buf[i].dataSz + BLACK_KEY_MAC_SZ);

#if 0
    /* sanity check can we load it? used for debugging and testing */
    desc->desc[desc->idx++] = (CAAM_KEY | CAAM_CLASS1 | 0x500000) +
        desc->buf[i].dataSz;
    desc->desc[desc->idx++] = CAAM_ADR_TO_PHYSICAL(vaddr[1], desc->buf[1].dataSz);
#endif

    do {
        err = caamDoJob(desc);
    } while (err == CAAM_WAITING);

    CAAM_ADR_UNMAP(vaddr[0], desc->buf[0].data, desc->buf[0].dataSz, 0);
    CAAM_ADR_UNMAP(vaddr[1], desc->buf[1].data, desc->buf[1].dataSz +
            BLACK_KEY_MAC_SZ, 1);
    return err;
}


/******************************************************************************
  Init
  ****************************************************************************/

/* initialize a DESCSTRUCT for an operation */
void caamDescInit(DESCSTRUCT* desc, int type, unsigned int args[4],
        CAAM_BUFFER* buf, int sz)
{
    int i;

    desc->type   = type;
    desc->idx    = 0;
    desc->output = 0;
    desc->ctxOut = 0;
    desc->outputIdx = 0;
    desc->alignIdx = 0;
    desc->lastFifo = 0;
    if (args == NULL) {
        desc->state    = 0;
        desc->ctxSz    = 0;
        desc->inputSz  = 0;
    }
    else {
        desc->state    = args[0] & 0xFFFF;
        desc->ctxSz    = args[1] & 0xFFFF;
        desc->inputSz  = args[2];
    }
    desc->aadSz    = 0;
    desc->DescriptorCount = sz;
    desc->startIdx = 0;
    desc->desc[desc->idx++] = CAAM_HEAD; /* later will put size to header*/

    for (i = 0; i < sz; i = i + 1) {
        desc->buf[i].data   = buf[i].TheAddress;
        desc->buf[i].dataSz = buf[i].Length;
    }
}


static int SetupJobRing(struct JobRing* r)
{
    /* get environment specific addresses to use for job rings */
    CAAM_SET_JOBRING_ADDR(&r->BaseAddr, &r->JobIn, &r->VirtualIn);

    /* register the in/out and sizes of job ring */
    r->Desc     = r->JobIn + (CAAM_JOBRING_SIZE * sizeof(unsigned int));
    r->JobOut   = r->Desc + (CAAM_DESC_MAX * CAAM_JOBRING_SIZE);

    CAAM_INIT_MUTEX(&caam.ring.jr_lock);

    r->VirtualDesc = r->VirtualIn  + (CAAM_JOBRING_SIZE * sizeof(unsigned int));
    r->VirtualOut  = r->VirtualDesc + (CAAM_DESC_MAX * CAAM_JOBRING_SIZE);

    memset(r->VirtualIn,   0, CAAM_JOBRING_SIZE * sizeof(unsigned int));
    memset(r->VirtualDesc, 0, CAAM_DESC_MAX * CAAM_JOBRING_SIZE);
    memset(r->VirtualOut,  0, 2 * CAAM_JOBRING_SIZE * sizeof(unsigned int));

    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    printf("Setting JOB IN  address 0x%08X, size %d\n",
        (unsigned int)caam.ring.JobIn, CAAM_JOBRING_SIZE);
    printf("Setting JOB OUT address 0x%08X, size %d\n",
        (unsigned int)caam.ring.JobOut, CAAM_JOBRING_SIZE);
    printf("Setting DESC 0x%08X\n", (unsigned int)caam.ring.Desc);
    #endif

    /* set the job ring in/out address's */
    CAAM_WRITE(r->BaseAddr + CAAM_IRBAR0, r->JobIn);
    CAAM_WRITE(r->BaseAddr + CAAM_ORBAR, r->JobOut);

    /* Initialize job ring sizes */
    CAAM_WRITE(r->BaseAddr + CAAM_IRSR0, CAAM_JOBRING_SIZE);
    CAAM_WRITE(r->BaseAddr + CAAM_ORSR0, CAAM_JOBRING_SIZE);

    /* if the job ring is busy, park it, flush it, and reset it */
    if (((CAAM_READ(r->BaseAddr + CAAM_STATUS) & 0x00000001) != 0) &&
        (CAAM_READ(r->BaseAddr + JRINTR_JR) != 0)) {
        unsigned int reg;

    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
        printf("Job ring is busy, trying to flush it\n");
    #endif

        /* JRCR job ring command register */
        CAAM_WRITE(r->BaseAddr + JRCR_JR, 0x2); /* park it  */
        CAAM_WRITE(r->BaseAddr + JRCR_JR, 0x1); /* flush it */
        do {
            reg = CAAM_READ(r->BaseAddr + JRINTR_JR);
        } while (!(reg & 0x8) && (reg != 0)); /* bit 3 hot signals is halted */

        if (reg != 0) {
            CAAM_WRITE(r->BaseAddr + JRCR_JR, 0x1); /* reset it */
        }
    }
    return 0;
}


int InitCAAM(void)
{
    Error ret;

    /* map to memory addresses needed for accessing CAAM */
#if defined(WOLFSSL_CAAM_PRINT)
    printf("Using CAAM_BASE 0x%04X\n", CAAM_BASE);
#endif
    ret = CAAM_SET_BASEADDR(&caam.baseAddr);
    if (ret != 0) {
        return ret;
    }

#if defined(WOLFSSL_CAAM_PRINT)
    printf("Using base address : 0x%04X\n", caam.baseAddr);
#endif
    ret = SetupJobRing(&caam.ring);
    if (ret != 0) {
        WOLFSSL_MSG("Error initializing job ring");
        INTERRUPT_Panic();
        return ret;
    }
#if defined(WOLFSSL_CAAM_PRINT)
    printf("Using base ring address : 0x%04X\n", caam.ring.BaseAddr);
#endif

    /* get CHA era */
    caam.vrs = CAAM_READ(caam.ring.BaseAddr + CAAM_CHA_CCBVID) >> 24;
#if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    printf("CAAM version = %04X\n",
        CAAM_READ(caam.ring.BaseAddr + CAAM_VERSION_MS) & 0xFFFF);
    printf("CAAM era = %d\n", caam.vrs);
    printf("RNG revision number = %08X\n",
        CAAM_READ(caam.ring.BaseAddr + CAAM_CRNR_LS));
    printSecureMemoryInfo();

    /* Check if CAAM supports AES-GCM */
    {
        unsigned int chaVerLS =
            CAAM_READ(caam.ring.BaseAddr + CAAM_CHA_VERSION_LS);
        printf("\nCHA support\n");
        printf("CHA Version LS = %04X\n", chaVerLS);
        if (!(chaVerLS & CAAM_AES_HIGH_PERFORMANCE)) {
            printf("High Performance AES module not supported, NO AES-GCM/XTS\n");
        }
        if (chaVerLS & CAAM_AES_LOW_POWER) {
            printf("Low Power AES module found\n");
        }
        printf("\n");
    }
#endif

    /* when on i.MX8 with SECO the SECO has control of this */
    if (caam.vrs < 9) {
        /* set DECO watchdog to time out and flush jobs that cause the DECO to
           hang */
        CAAM_WRITE(caam.baseAddr + CAAM_RSTA,
                   CAAM_READ(caam.baseAddr + CAAM_RSTA) | 0x40000000);

        /* start up RNG if not already started */
        if (caamInitRng(&caam) != 0) {
            WOLFSSL_MSG("Error initializing RNG");
            INTERRUPT_Panic();
            return -1;
        }
    }

    WOLFSSL_MSG("Successfully initialized CAAM driver");
    #if defined(WOLFSSL_CAAM_DEBUG) || defined(WOLFSSL_CAAM_PRINT)
    fflush(stdout);
    #endif
    return 0;
}


int CleanupCAAM()
{
    caamFreeAllPart();
    CAAM_UNSET_JOBRING_ADDR(caam.ring.BaseAddr, caam.ring.JobIn,
        caam.ring.VirtualIn);
    CAAM_FREE_MUTEX(&caam.ring.jr_lock);
    CAAM_UNSET_BASEADDR(caam.baseAddr);
    return 0;
}

#endif /* __INTEGRITY || INTEGRITY || __QNX__ || __QNXNTO__ */
