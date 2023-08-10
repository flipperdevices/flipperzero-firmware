/* quickassist_sync.c
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

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_INTEL_QA_SYNC

#ifdef QAT_DEMO_MAIN
    #define QAT_DEBUG
#endif


#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif

#include <wolfssl/wolfcrypt/cryptocb.h>
#include <wolfssl/wolfcrypt/port/intel/quickassist_sync.h>

#include "cpa.h"
#include "cpa_cy_im.h"
#include "cpa_cy_sym.h"
#include "cpa_cy_rsa.h"
#include "cpa_cy_ln.h"
#include "cpa_cy_ecdh.h"
#include "cpa_cy_ecdsa.h"
#include "cpa_cy_dh.h"
#include "cpa_cy_drbg.h"
#include "cpa_cy_nrbg.h"
#include "cpa_cy_prime.h"

#include "icp_sal_user.h"
#include "icp_sal_poll.h"


#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* User space utils */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if 0
    /* Optional feature for partial QAT hashing support */
    /* This will process updates through hardware instead of caching them */
    #define QAT_HASH_ENABLE_PARTIAL
#endif
#ifdef QAT_HASH_ENABLE_PARTIAL
    #define MAX_QAT_HASH_BUFFERS 2
#endif

/* Detect QAT driver version */
#if defined(CPA_CY_API_VERSION_NUM_MAJOR) && CPA_CY_API_VERSION_NUM_MAJOR > 1
    #define QAT_V2
#endif

#ifdef QAT_V2
    /* quickassist/utilities/libusdm_drv/qae_mem.h */
    /* Provides user-space API's for accessing NUMA allocated memory through usdm_drv */
    #include "qae_mem.h"
#include "linux/include/qae_mem_utils.h"
#endif

#ifdef QAT_USE_POLLING_THREAD
    #include <pthread.h>
#endif

/* Tunable parameters */
#ifndef QAT_PROCESS_NAME
    #define QAT_PROCESS_NAME     "SSL"
#endif
#ifndef QAT_LIMIT_DEV_ACCESS
    #define QAT_LIMIT_DEV_ACCESS CPA_FALSE
#endif
#ifndef QAT_MAX_DEVICES
    #define QAT_MAX_DEVICES  (1)  /* maximum number of QAT cards */
#endif

#ifndef QAT_RETRY_LIMIT
    #define QAT_RETRY_LIMIT  (100)
#endif
#ifndef QAT_POLL_RESP_QUOTA
    #define QAT_POLL_RESP_QUOTA (0) /* all pending */
#endif

#if !defined(NO_AES) || !defined(NO_DES3)
    #define QAT_ENABLE_CRYPTO
#endif

/* Pre-declarations */
struct IntelQaDev;
struct wc_CryptoInfo;
struct WC_BIGINT;
struct WC_RNG;


#if defined(QAT_ENABLE_HASH) || defined(QAT_ENABLE_CRYPTO)
/* symmetric context */
typedef struct IntelQaSymCtx {
    CpaCySymOpData opData;
    CpaCySymSessionCtx symCtxSrc;
    CpaCySymSessionCtx symCtx;
    word32 symCtxSize;

    /* flags */
    word32 isOpen:1;
    word32 isCopy:1;
} IntelQaSymCtx;
#endif

typedef void (*IntelQaFreeFunc)(struct IntelQaDev*);


/* QuickAssist device */
typedef struct IntelQaDev {
	CpaInstanceHandle handle;
    int devId;
	void* heap;

    /* callback return info */
    int ret;
    byte* out;
    union {
        word32* outLenPtr;
        word32 outLen;
    };

    /* operations */
    IntelQaFreeFunc freeFunc;
    union {
    #ifdef QAT_ENABLE_CRYPTO
        struct {
            IntelQaSymCtx ctx;
            CpaBufferList bufferList;
            CpaFlatBuffer flatBuffer;
            byte* authTag;
            word32 authTagSz;
        } cipher;
    #endif
    } op;

#ifdef QAT_USE_POLLING_THREAD
    pthread_t pollingThread;
    byte pollingCy;
#endif
} IntelQaDev;


/* Interface */
static int IntelQaHardwareStart(const char*, int);
static void IntelQaHardwareStop(void);
static int IntelQaInit(void*);
static void IntelQaDeInit(int);
static int IntelQaNumInstances(void);
static int IntelQaOpen(IntelQaDev*, int);
static void IntelQaClose(IntelQaDev*);
static int IntelQaDevCopy(IntelQaDev*, IntelQaDev*);
static int IntelQaPoll(IntelQaDev*);
static int IntelQaGetCyInstanceCount(void);

#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        static int IntelQaSymAesCbcEncrypt(IntelQaDev*, byte*,
                const byte*, word32, const byte*, word32, const byte*, word32);
    #ifdef HAVE_AES_DECRYPT
        static int IntelQaSymAesCbcDecrypt(IntelQaDev*, byte*,
                const byte*, word32, const byte*, word32, const byte*, word32);
    #endif /* HAVE_AES_DECRYPT */
    #endif /* HAVE_AES_CBC */

    #ifdef HAVE_AESGCM
        static int IntelQaSymAesGcmEncrypt(IntelQaDev*, byte*,
                const byte*, word32, const byte*, word32, const byte*, word32,
                byte*, word32, const byte*, word32);
    #ifdef HAVE_AES_DECRYPT
        static int IntelQaSymAesGcmDecrypt(IntelQaDev*, byte*,
                const byte*, word32, const byte*, word32, const byte*, word32,
                const byte*, word32, const byte*, word32);
    #endif /* HAVE_AES_DECRYPT */
    #endif /* HAVE_AESGCM */
#endif /* !NO_AES */

#ifndef NO_DES3
    static int IntelQaSymDes3CbcEncrypt(IntelQaDev*, byte*,
            const byte*, word32, const byte*, word32, const byte* iv, word32);
    static int IntelQaSymDes3CbcDecrypt(IntelQaDev* dev, byte*,
            const byte*, word32, const byte*, word32, const byte* iv, word32);
#endif /*! NO_DES3 */

#ifdef WOLF_CRYPTO_CB
    static int IntelQaSymSync_CryptoDevCb(int, struct wc_CryptoInfo*,
			void*);
#endif /* WOLF_CRYPTO_CB */


#ifdef QAT_DEBUG
    #define QLOG(...) do { printf(__VA_ARGS__); } while (0)
#else
    #define QLOG(...) WC_DO_NOTHING
#endif


#define OS_HOST_TO_NW_32(uData) ByteReverseWord32(uData)


static CpaInstanceHandle* g_cyInstances = NULL;
static CpaInstanceInfo2* g_cyInstanceInfo = NULL;
static Cpa32U* g_cyInstMap = NULL;
static Cpa16U g_numInstances = 0;
static Cpa16U g_instCounter = 0;
static CpaBoolean g_cyServiceStarted = CPA_FALSE;
#ifdef QAT_USE_POLLING_CHECK
    static CpaBoolean* g_cyPolling = NULL;
    static pthread_mutex_t* g_PollLock;
#endif
static volatile int g_initCount = 0;
static pthread_mutex_t g_Hwlock = PTHREAD_MUTEX_INITIALIZER;


typedef struct qatCapabilities {
    /* capabilities */
    word32 supPartial:1;
    word32 supSha3:1;
} qatCapabilities_t;
static qatCapabilities_t g_qatCapabilities = {0};


#if defined(QAT_ENABLE_CRYPTO)
    static int IntelQaSymClose(IntelQaDev* dev, int doFree);
#endif


extern Cpa32U osalLogLevelSet(Cpa32U level);


static IntelQaDev qaDev;


/* -------------------------------------------------------------------------- */
/* Polling */
/* -------------------------------------------------------------------------- */

static WC_INLINE int SyncSleep(word32 ms)
{
    int ret = 0;
    struct timespec resTime, remTime;
    resTime.tv_sec = ms/1000;
    resTime.tv_nsec = (ms%1000)*1000000;
    do {
        ret = nanosleep(&resTime, &remTime);
        resTime = remTime;
    } while ((ret!=0) && (errno == EINTR));

    if (ret != 0) {
        QLOG("nanoSleep failed with code %d\n", ret);
        return BAD_FUNC_ARG;
    }

   return ret;
}

#ifdef QAT_USE_POLLING_THREAD
static void* IntelQaPollingThread(void* context)
{
    IntelQaDev* dev = (IntelQaDev*)context;

    QLOG("Polling Thread Start\n");
    while (dev->pollingCy) {
        icp_sal_CyPollInstance(dev->handle, QAT_POLL_RESP_QUOTA);
        SyncSleep(10);
    }
    QLOG("Polling Thread Exit\n");
    pthread_exit(NULL);
}

static CpaStatus IntelQaStartPollingThread(IntelQaDev* dev)
{
    if (dev->pollingCy == 0) {
        dev->pollingCy = 1;

        QLOG("Polling Thread Created\n");

        if (pthread_create(&dev->pollingThread, NULL, IntelQaPollingThread,
                                                            (void*)dev) != 0) {
            QLOG("Failed create polling thread!\n");
            return CPA_STATUS_FAIL;
        }
    }
    return CPA_STATUS_SUCCESS;
}

static void IntelQaStopPollingThread(IntelQaDev* dev)
{
    dev->pollingCy = 0;
    pthread_join(dev->pollingThread, 0);
}
#endif /* QAT_USE_POLLING_THREAD */


/* -------------------------------------------------------------------------- */
/* Device */
/* -------------------------------------------------------------------------- */
void IntelQaHardwareStop(void)
{
    int i;
    CpaStatus status;

    g_initCount--;  /* track de-init count */
    if (g_initCount != 0) {
        return;
    }

    if (g_cyServiceStarted == CPA_TRUE) {
        g_cyServiceStarted = CPA_FALSE;
        for (i=0; i<g_numInstances; i++) {
            status = cpaCyStopInstance(g_cyInstances[i]);
            if (status != CPA_STATUS_SUCCESS) {
                QLOG("IntelQA: Could not stop instance: %d\n"
                        "\tInternal error has occur which probably can only be"
                        "fixed by a reboot\n", i);
            }
        }
    }

    status = icp_sal_userStop();
    if (status != CPA_STATUS_SUCCESS) {
        QLOG("IntelQA: Could not stop sal for user space (status %d)\n",
                status);
    }

    if (g_cyInstMap) {
        XFREE(g_cyInstMap, NULL, DYNAMIC_TYPE_ASYNC);
        g_cyInstMap = NULL;
    }

    if (g_cyInstanceInfo) {
        XFREE(g_cyInstanceInfo, NULL, DYNAMIC_TYPE_ASYNC);
        g_cyInstanceInfo = NULL;
    }

#ifdef QAT_USE_POLLING_CHECK
    if (g_cyPolling) {
        XFREE(g_cyPolling, NULL, DYNAMIC_TYPE_ASYNC);
        g_cyPolling = NULL;
    }
    if (g_PollLock) {
        for (i=0; i<g_numInstances; i++) {
            pthread_mutex_destroy(&g_PollLock[i]);
        }
        XFREE(g_PollLock, NULL, DYNAMIC_TYPE_ASYNC);
        g_PollLock = NULL;
    }
#endif

    if (g_cyInstances) {
        XFREE(g_cyInstances, NULL, DYNAMIC_TYPE_ASYNC);
        g_cyInstances = NULL;
        g_numInstances = 0;
    }

    qaeMemDestroy();

    QLOG("IntelQA: Stop\n");
}


int IntelQaHardwareStart(const char* process_name, int limitDevAccess)
{
    int ret = 0, i;
    CpaStatus status;

    g_initCount++;
    if (g_initCount > 1) {
        return 0;
    }

    status = qaeMemInit();
    if (status != CPA_STATUS_SUCCESS) {
        QLOG("IntelQA: Could not start qae mem for user space (status %d)\n"
                "\tHas the qaeMemDrv.ko module been loaded?\n",
                status);
        return ASYNC_INIT_E;
    }

    status = icp_sal_userStartMultiProcess(process_name,
        limitDevAccess ? CPA_TRUE : CPA_FALSE);
    if (status != CPA_STATUS_SUCCESS) {
        QLOG("IntelQA: Could not start sal for user space! status %d\n",
                status);
        ret = ASYNC_INIT_E; goto error;
    }

#ifdef QAT_DEBUG
    /* optionally enable debugging */
    //osalLogLevelSet(8);
#endif

    status = cpaCyGetNumInstances(&g_numInstances);
    if (status != CPA_STATUS_SUCCESS || g_numInstances == 0) {
        QLOG("IntelQA: Failed to get num of instances! status %d\n", status);
        ret = INVALID_DEVID; goto error;
    }

    /* Get handles / info */
    g_cyInstances = (CpaInstanceHandle*)XMALLOC(
        sizeof(CpaInstanceHandle) * g_numInstances, NULL, DYNAMIC_TYPE_ASYNC);
    if (g_cyInstances == NULL) {
        QLOG("IntelQA: Failed to allocate instances\n");
        ret = INVALID_DEVID; goto error;
    }

#ifdef QAT_USE_POLLING_CHECK
    g_cyPolling = (CpaBoolean*)XMALLOC(sizeof(CpaBoolean) * g_numInstances, NULL,
        DYNAMIC_TYPE_ASYNC);
    if (g_cyPolling == NULL) {
        QLOG("IntelQA: Failed to allocate polling status\n");
        ret = INVALID_DEVID; goto error;
    }
    g_PollLock = (pthread_mutex_t*)XMALLOC(sizeof(pthread_mutex_t) *
        g_numInstances, NULL, DYNAMIC_TYPE_ASYNC);
    if (g_PollLock == NULL) {
        QLOG("IntelQA: Failed to allocate polling locks\n");
        ret = INVALID_DEVID; goto error;
    }
    for (i=0; i<g_numInstances; i++) {
        pthread_mutex_init(&g_PollLock[i], NULL);
    }
#endif

    g_cyInstanceInfo = (CpaInstanceInfo2*)XMALLOC(
        sizeof(CpaInstanceInfo2) * g_numInstances, NULL, DYNAMIC_TYPE_ASYNC);
    if (g_cyInstanceInfo == NULL) {
        QLOG("IntelQA: Failed to allocate instance info\n");
        ret = INVALID_DEVID; goto error;
    }

    g_cyInstMap = (Cpa32U*)XMALLOC(
        sizeof(Cpa32U) * g_numInstances, NULL, DYNAMIC_TYPE_ASYNC);
    if (g_cyInstMap == NULL) {
        QLOG("IntelQA: Failed to allocate instance map\n");
        ret = INVALID_DEVID; goto error;
    }

    status = cpaCyGetInstances(g_numInstances, g_cyInstances);
    if (status != CPA_STATUS_SUCCESS) {
        QLOG("IntelQA: Failed to get IntelQA instances\n");
        ret = INVALID_DEVID; goto error;
    }

    /* start all instances */
    g_cyServiceStarted = CPA_TRUE;
    for (i=0; i<g_numInstances; i++) {
        Cpa32U coreAffinity = 0;
        CpaCySymCapabilitiesInfo capabilities;
        int j;
        XMEMSET(&capabilities, 0, sizeof(capabilities));

        status = cpaCyInstanceGetInfo2(g_cyInstances[i],
                                                    &g_cyInstanceInfo[i]);
        if (status != CPA_STATUS_SUCCESS) {
            QLOG("IntelQA: Error getting instance info for %d\n", i);
            ret = INVALID_DEVID; goto error;
        }

        /* loop of the instanceInfo coreAffinity bitmask to find the core */
        for (j=0; j<CPA_MAX_CORES; j++) {
            if (CPA_BITMAP_BIT_TEST(g_cyInstanceInfo[i].coreAffinity, j)) {
                coreAffinity = i;
                break;
            }
        }
        g_cyInstMap[i] = coreAffinity;

        /* capabilities */
        status = cpaCySymQueryCapabilities(g_cyInstances[i], &capabilities);
        if (status == CPA_STATUS_SUCCESS) {
            g_qatCapabilities.supPartial = capabilities.partialPacketSupported;
            if (capabilities.partialPacketSupported != CPA_TRUE) {
                QLOG("Warning: QAT does not support partial packets!\n");
            }
        }

        QLOG("Inst %d, Node: %d, Affin: %u, Dev: %u, Accel %u, "
                "EE %u, BDF %02X:%02X:%02X, isPolled %d\n",
                i, g_cyInstanceInfo[i].nodeAffinity, coreAffinity,
                g_cyInstanceInfo[i].physInstId.packageId,
                g_cyInstanceInfo[i].physInstId.acceleratorId,
                g_cyInstanceInfo[i].physInstId.executionEngineId,
                (Cpa8U)((g_cyInstanceInfo[i].physInstId.busAddress) >> 8),
                (Cpa8U)((g_cyInstanceInfo[i].physInstId.busAddress)
                                                            & 0xFF) >> 3,
                (Cpa8U)((g_cyInstanceInfo[i].physInstId.busAddress) & 3),
                g_cyInstanceInfo[i].isPolled);

        status = cpaCySetAddressTranslation(g_cyInstances[i],
            qaeVirtToPhysNUMA);
        if (status != CPA_STATUS_SUCCESS) {
            QLOG("IntelQA: Error setting memory config for inst %d\n", i);
            ret = INVALID_DEVID; goto error;
        }

        status = cpaCyStartInstance(g_cyInstances[i]);
        if (status != CPA_STATUS_SUCCESS) {
            QLOG("IntelQA: Error starting crypto instance %d\n", i);
            ret = INVALID_DEVID; goto error;
        }
    }

    QLOG("IntelQA: Instances %d\n", g_numInstances);
    return ret;

error:
    IntelQaHardwareStop();
    return ret;
}


int IntelQaInit(void* threadId)
{
    int ret;
    int devId;
    (void)threadId;

    ret = pthread_mutex_lock(&g_Hwlock);
    if (ret != 0) {
        QLOG("IntelQaInit: mutex lock failed! %d\n", ret);
        return BAD_MUTEX_E;
    }

    ret = IntelQaHardwareStart(QAT_PROCESS_NAME, QAT_LIMIT_DEV_ACCESS);
    if (ret != 0) {
        pthread_mutex_unlock(&g_Hwlock);
        return ret;
    }

    if (g_numInstances <= 0) {
        pthread_mutex_unlock(&g_Hwlock);
        return ASYNC_INIT_E;
    }

    /* assign device id */
    devId = (g_instCounter % g_numInstances);
    g_instCounter++;

    pthread_mutex_unlock(&g_Hwlock);

    return devId;
}


int IntelQaNumInstances(void)
{
    return g_numInstances;
}


int IntelQaOpen(IntelQaDev* dev, int devId)
{
    if (dev == NULL) {
        return BAD_FUNC_ARG;
    }

    /* clear device info */
    XMEMSET(dev, 0, sizeof(IntelQaDev));

    if (g_cyInstances == NULL) {
        QLOG("IntelQA not initialized\n");
        return ASYNC_INIT_E;
    }

    dev->devId = devId;
    dev->handle = g_cyInstances[devId];

#ifdef QAT_USE_POLLING_THREAD
    /* start polling thread */
    IntelQaStartPollingThread(dev);
#endif

    return 0;
}


#if defined(QAT_ENABLE_CRYPTO)

static IntelQaSymCtx* IntelQaGetSymCtx(IntelQaDev* dev)
{
    return &dev->op.cipher.ctx;
}

#endif


void IntelQaClose(IntelQaDev* dev)
{
    if (dev) {
        QLOG("IntelQaClose %p\n", dev);
        /* close any active session */
        IntelQaSymClose(dev, 1);

    #ifdef QAT_USE_POLLING_THREAD
        IntelQaStopPollingThread(dev);
    #endif

        dev->handle = NULL;
    }
}

void IntelQaDeInit(int devId)
{
    (void)devId;

    if (pthread_mutex_lock(&g_Hwlock) == 0) {
        IntelQaHardwareStop();
        pthread_mutex_unlock(&g_Hwlock);
    }
}

int IntelQaPoll(IntelQaDev* dev)
{
    int ret = 0;
    CpaStatus status;

#ifdef QAT_USE_POLLING_CHECK
    pthread_mutex_t* lock = &g_PollLock[dev->qat.devId];
    if (pthread_mutex_lock(lock) == 0) {
        /* test if any other threads are polling */
        if (g_cyPolling[dev->qat.devId]) {
            pthread_mutex_unlock(lock);

            /* return success even though its busy, caller will treat as WC_PENDING_E */
            return 0;
        }

        g_cyPolling[dev->qat.devId] = 1;
        pthread_mutex_unlock(lock);
    }
#endif

    status = icp_sal_CyPollInstance(dev->handle, QAT_POLL_RESP_QUOTA);
    if (status != CPA_STATUS_SUCCESS && status != CPA_STATUS_RETRY) {
        QLOG("IntelQa: Poll failure %d\n", status);
        ret = -1;
    }

    {
        if (dev->ret != WC_PENDING_E) {
            /* perform cleanup */
            IntelQaFreeFunc freeFunc = dev->freeFunc;
            QLOG("IntelQaOpFree: Dev %p, FreeFunc %p\n", dev, freeFunc);
            if (freeFunc) {
                dev->freeFunc = NULL;
                freeFunc(dev);
            }
        }
    }

#ifdef QAT_USE_POLLING_CHECK
    /* indicate we are done polling */
    if (pthread_mutex_lock(lock) == 0) {
        g_cyPolling[dev->qat.devId] = 0;
        pthread_mutex_unlock(lock);
    }
#endif

    return ret;
}

static int IntelQaPollBlockRet(IntelQaDev* dev, int ret_wait)
{
    int ret;

    do {
        ret = IntelQaPoll(dev);

        if (dev->ret != ret_wait) {
            break;
        }
    } while (1);
    ret = dev->ret;

    return ret;
}

int IntelQaGetCyInstanceCount(void)
{
    return g_numInstances;
}

static WC_INLINE int IntelQaHandleCpaStatus(IntelQaDev* dev, CpaStatus status,
    int* ret, byte isAsync, void* callback, int* retryCount)
{
    int retry = 0;

    if (status == CPA_STATUS_SUCCESS) {
        if (isAsync && callback) {
            *ret = WC_PENDING_E;
        }
        else {
            *ret = IntelQaPollBlockRet(dev, WC_PENDING_E);
        }
    }
    else if (status == CPA_STATUS_RETRY) {
        (*retryCount)++;
        if ((*retryCount % (QAT_RETRY_LIMIT + 1)) == QAT_RETRY_LIMIT) {
            SyncSleep(10);
        }
        retry = 1;
    }
    else {
        *ret = ASYNC_OP_E;
    }

    return retry;
}

static WC_INLINE void IntelQaOpInit(IntelQaDev* dev, IntelQaFreeFunc freeFunc)
{
    dev->ret = WC_PENDING_E;
    dev->freeFunc = freeFunc;
}


/* -------------------------------------------------------------------------- */
/* Symmetric Algos */
/* -------------------------------------------------------------------------- */

#if defined(QAT_ENABLE_CRYPTO)

static int IntelQaSymOpen(IntelQaDev* dev, CpaCySymSessionSetupData* setup,
    CpaCySymCbFunc callback)
{
    int ret = 0;
    CpaStatus status = CPA_STATUS_SUCCESS;
    Cpa32U sessionCtxSize = 0;
    IntelQaSymCtx* ctx;

    /* arg check */
    if (dev == NULL || setup == NULL) {
        return BAD_FUNC_ARG;
    }

    ctx = IntelQaGetSymCtx(dev);

    /* Determine size of session context to allocate - use max size */
    status = cpaCySymSessionCtxGetSize(dev->handle, setup, &sessionCtxSize);

    if (ctx->symCtxSize > 0 && ctx->symCtxSize > sessionCtxSize) {
        QLOG("Symmetric context size error! Buf %d, Exp %d\n",
                ctx->symCtxSize, sessionCtxSize);
        return ASYNC_OP_E;
    }

    /* make sure session context is allocated */
    if (ctx->symCtx == NULL) {
        /* Allocate session context */
        ctx->symCtx = XMALLOC(sessionCtxSize, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA64);
        if (ctx->symCtx == NULL) {
            return MEMORY_E;
        }
    }
    ctx->symCtxSize = sessionCtxSize;

    if (!ctx->isOpen) {
        ctx->isOpen = 1;

        QLOG("IntelQaSymOpen: InitSession dev %p, symCtx %p\n",
                dev, ctx->symCtx);

        /* open symmetric session */
        status = cpaCySymInitSession(dev->handle, callback, setup, ctx->symCtx);
        if (status != CPA_STATUS_SUCCESS) {
            QLOG("cpaCySymInitSession failed! dev %p, status %d\n",
                    dev, status);
            XFREE(ctx->symCtx, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA64);
            ctx->symCtx = NULL;
            return ASYNC_INIT_E;
        }
    }

    if (ctx->symCtxSrc == NULL) {
        ctx->symCtxSrc = ctx->symCtx;
    }

    QLOG("IntelQaSymOpen: dev %p, symCtx %p (src %p), "
            "symCtxSize %d, isCopy %d, isOpen %d\n",
            dev, ctx->symCtx, ctx->symCtxSrc, ctx->symCtxSize,
            ctx->isCopy, ctx->isOpen);

    return ret;
}

static int IntelQaSymClose(IntelQaDev* dev, int doFree)
{
    int ret = 0;
    CpaStatus status = CPA_STATUS_SUCCESS;
    IntelQaSymCtx* ctx;

    if (dev == NULL) {
        return BAD_FUNC_ARG;
    }

    ctx = IntelQaGetSymCtx(dev);

    QLOG("IntelQaSymClose: dev %p, ctx %p, symCtx %p (src %p), "
            "symCtxSize %d, isCopy %d, isOpen %d, doFree %d\n",
            dev, ctx, ctx->symCtx, ctx->symCtxSrc, ctx->symCtxSize,
            ctx->isCopy, ctx->isOpen, doFree);

    if (ctx->symCtx == ctx->symCtxSrc && ctx->symCtx != NULL) {
        if (ctx->isOpen) {
            ctx->isOpen = 0;
            QLOG("IntelQaSymClose: RemoveSession dev %p, symCtx %p\n",
                    dev, ctx->symCtx);
            status = cpaCySymRemoveSession(dev->handle, ctx->symCtx);
            if (status == CPA_STATUS_RETRY) {
                QLOG("cpaCySymRemoveSession retry!\n");
                /* treat this as error, since session should not be active */
                ret = ASYNC_OP_E;
            }
            else if (status != CPA_STATUS_SUCCESS) {
                QLOG("cpaCySymRemoveSession failed! status %d\n", status);
                ret = ASYNC_OP_E;
            }
        }
    }

    if (doFree) {
        XFREE(ctx->symCtx, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA64);
        ctx->symCtx = NULL;
        ctx->symCtxSrc = NULL;
        ctx->symCtxSize = 0;
    }

    return ret;
}

#endif /* QAT_ENABLE_CRYPTO */


/* -------------------------------------------------------------------------- */
/* AES/DES Algo */
/* -------------------------------------------------------------------------- */

#ifdef QAT_ENABLE_CRYPTO

static void IntelQaSymCipherFree(IntelQaDev* dev)
{
    IntelQaSymCtx* ctx = &dev->op.cipher.ctx;
    CpaCySymOpData* opData = &ctx->opData;
    CpaBufferList* pDstBuffer = &dev->op.cipher.bufferList;

    if (opData) {
        if (opData->pAdditionalAuthData) {
            XFREE(opData->pAdditionalAuthData, dev->heap,
                    DYNAMIC_TYPE_ASYNC_NUMA);
            opData->pAdditionalAuthData = NULL;
        }
        if (opData->pIv) {
            XFREE(opData->pIv, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);
            opData->pIv = NULL;
        }
        XMEMSET(opData, 0, sizeof(CpaCySymOpData));
    }
    if (pDstBuffer) {
        if (pDstBuffer->pBuffers) {
            if (pDstBuffer->pBuffers->pData) {
                XFREE(pDstBuffer->pBuffers->pData, dev->heap,
                        DYNAMIC_TYPE_ASYNC_NUMA);
                pDstBuffer->pBuffers->pData = NULL;
            }
            XMEMSET(pDstBuffer->pBuffers, 0, sizeof(CpaFlatBuffer));
        }
        if (pDstBuffer->pPrivateMetaData) {
            XFREE(pDstBuffer->pPrivateMetaData, dev->heap,
                    DYNAMIC_TYPE_ASYNC_NUMA);
            pDstBuffer->pPrivateMetaData = NULL;
        }
        XMEMSET(pDstBuffer, 0, sizeof(CpaBufferList));
    }

    /* close and free sym context */
    IntelQaSymClose(dev, 1);

    /* clear temp pointers */
    dev->out = NULL;
    dev->outLen = 0;
#ifndef NO_AES
    if (dev->op.cipher.authTag != NULL) {
        XMEMSET(dev->op.cipher.authTag, 0, dev->op.cipher.authTagSz);
        XFREE(dev->op.cipher.authTag, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);
        dev->op.cipher.authTag = NULL;
    }
    dev->op.cipher.authTagSz = 0;
#endif
}

static int IntelQaSymCipher(IntelQaDev* dev, byte* out, const byte* in,
    word32 inOutSz, const byte* key, word32 keySz, const byte* iv, word32 ivSz,
    CpaCySymOp symOperation, CpaCySymCipherAlgorithm cipherAlgorithm,
    CpaCySymCipherDirection cipherDirection,

    /* for auth ciphers (CCM or GCM) */
    CpaCySymHashAlgorithm hashAlgorithm,
    byte* authTag, word32 authTagSz,
    const byte* authIn, word32 authInSz)
{
    int ret;
    CpaStatus status = CPA_STATUS_SUCCESS;
    CpaCySymOpData* opData = NULL;
    CpaCySymSessionSetupData setup;
    const Cpa32U numBuffers = 1;
    CpaBufferList* bufferList = NULL;
    CpaFlatBuffer* flatBuffer = NULL;
    Cpa8U* ivBuf = NULL;
    Cpa8U* dataBuf = NULL;
    Cpa32U dataLen = inOutSz;
    Cpa8U* metaBuf = NULL;
    Cpa32U metaSize = 0;
    Cpa8U* authInBuf = NULL;
    Cpa32U authInSzAligned = authInSz;
    Cpa8U* authTagBuf = NULL;
    IntelQaSymCtx* ctx;
    CpaBoolean verifyResult = CPA_FALSE;

    QLOG("IntelQaSymCipher: dev %p, out %p, in %p, inOutSz %d, op %d, "
            "algo %d, dir %d, hash %d\n",
            dev, out, in, inOutSz, symOperation, cipherAlgorithm,
            cipherDirection, hashAlgorithm);

    /* check args */
    if (out == NULL || in == NULL || inOutSz == 0 ||
        key == NULL || keySz == 0 || iv == NULL || ivSz == 0) {
        return BAD_FUNC_ARG;
    }
    if (hashAlgorithm != CPA_CY_SYM_HASH_NONE &&
        (authTag == NULL || authTagSz == 0)) {
        return BAD_FUNC_ARG;
    }

    /* get meta size */
    status = cpaCyBufferListGetMetaSize(dev->handle, numBuffers, &metaSize);
    if (status != CPA_STATUS_SUCCESS && metaSize <= 0) {
        ret = BUFFER_E; goto exit;
    }

    /* if authtag provided then it will be appended to end of input */
    if (authTag && authTagSz > 0) {
        dataLen += authTagSz;
    }

    /* allocate buffers */
    ctx = &dev->op.cipher.ctx;
    opData = &ctx->opData;
    bufferList = &dev->op.cipher.bufferList;
    flatBuffer = &dev->op.cipher.flatBuffer;
    metaBuf = XMALLOC(metaSize, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);
    dataBuf = XMALLOC(dataLen, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);
    XMEMCPY(dataBuf, in, inOutSz);
    ivBuf = XMALLOC(AES_BLOCK_SIZE, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);
    XMEMCPY(ivBuf, iv, ivSz);
    authTagBuf = XMALLOC(authTagSz, dev->heap, DYNAMIC_TYPE_ASYNC_NUMA);

    /* check allocations */
    if (ivBuf == NULL || metaBuf == NULL || dataBuf == NULL ||
        authTagBuf == NULL) {
        ret = MEMORY_E; goto exit;
    }

    /* AAD */
    if (authIn && authInSz > 0) {
        /* make sure AAD is block aligned */
        if (authInSzAligned % AES_BLOCK_SIZE) {
            authInSzAligned += AES_BLOCK_SIZE -
                (authInSzAligned % AES_BLOCK_SIZE);
        }

        authInBuf = XMALLOC(authInSzAligned, dev->heap,
                DYNAMIC_TYPE_ASYNC_NUMA);
        XMEMCPY(authInBuf, authIn, authInSz);
        if (authInBuf == NULL) {
            ret = MEMORY_E; goto exit;
        }
        /* clear remainder */
        XMEMSET(authInBuf + authInSz, 0, authInSzAligned - authInSz);
    }

    /* init buffers */
    XMEMSET(&setup, 0, sizeof(CpaCySymSessionSetupData));
    XMEMSET(opData, 0, sizeof(CpaCySymOpData));
    XMEMSET(bufferList, 0, sizeof(CpaBufferList));
    XMEMSET(flatBuffer, 0, sizeof(CpaFlatBuffer));
    XMEMSET(metaBuf, 0, metaSize);

    bufferList->pBuffers = flatBuffer;
    bufferList->numBuffers = numBuffers;
    bufferList->pPrivateMetaData = metaBuf;
    flatBuffer->dataLenInBytes = dataLen;
    flatBuffer->pData = dataBuf;

    /* setup */
    setup.sessionPriority = CPA_CY_PRIORITY_NORMAL;
    setup.symOperation = symOperation;
    setup.cipherSetupData.cipherAlgorithm = cipherAlgorithm;
    setup.cipherSetupData.cipherKeyLenInBytes = keySz;
    setup.cipherSetupData.pCipherKey = (byte*)key;
    setup.cipherSetupData.cipherDirection = cipherDirection;

    /* setup auth ciphers */
    if (hashAlgorithm != CPA_CY_SYM_HASH_NONE) {
        setup.algChainOrder =
            (cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT) ?
                CPA_CY_SYM_ALG_CHAIN_ORDER_CIPHER_THEN_HASH :
                CPA_CY_SYM_ALG_CHAIN_ORDER_HASH_THEN_CIPHER;

        setup.hashSetupData.hashAlgorithm = hashAlgorithm;
        setup.hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
        setup.hashSetupData.digestResultLenInBytes = authTagSz;
        setup.hashSetupData.authModeSetupData.aadLenInBytes = authInSz;

        if (cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT)
            setup.digestIsAppended = CPA_TRUE;
        else
            setup.digestIsAppended = CPA_FALSE;
    }

    /* open session */
    ret = IntelQaSymOpen(dev, &setup, NULL);
    if (ret != 0) {
        goto exit;
    }

    /* operation data */
    opData->sessionCtx = ctx->symCtx;
    opData->packetType = CPA_CY_SYM_PACKET_TYPE_FULL;
    opData->pIv = ivBuf;
    opData->ivLenInBytes = ivSz;
    opData->cryptoStartSrcOffsetInBytes = 0;
    opData->messageLenToCipherInBytes = inOutSz;
    if (authIn && authInSz > 0) {
        opData->pAdditionalAuthData = authInBuf;
    }
    if (cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT) {
        if (authTag && authTagSz > 0) {
            /* append digest to end of data buffer */
            XMEMCPY(flatBuffer->pData + inOutSz, authTag, authTagSz);
        }
    }
    else {
        if (authTag && authTagSz > 0) {
            XMEMCPY(authTagBuf, authTag, authTagSz);
        }
    }

    /* store info needed for output */
    dev->out = out;
    dev->outLen = inOutSz;
    if (cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT) {
        dev->op.cipher.authTag = authTagBuf;
        dev->op.cipher.authTagSz = authTagSz;
        opData->pDigestResult = authTagBuf;
    }
    else {
        dev->op.cipher.authTag = NULL;
        dev->op.cipher.authTagSz = 0;
    }
    IntelQaOpInit(dev, IntelQaSymCipherFree);

    /* perform symmetric AES operation async */
    /* use same buffer list for in-place operation */
    status = cpaCySymPerformOp(dev->handle, dev, opData,
            bufferList, bufferList, &verifyResult);

    if (symOperation == CPA_CY_SYM_OP_ALGORITHM_CHAINING &&
        cipherAlgorithm == CPA_CY_SYM_CIPHER_AES_GCM &&
        cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT &&
        hashAlgorithm == CPA_CY_SYM_HASH_AES_GCM) {
        if (verifyResult == CPA_FALSE) {
            ret = AES_GCM_AUTH_E;
        }
    }
exit:

    if (ret != 0) {
        QLOG("cpaCySymPerformOp Cipher failed! dev %p, status %d, ret %d\n",
            dev, status, ret);
    }

    /* Capture the inline decrypt into the output. */
    XMEMCPY(out, dataBuf, inOutSz);
    if (cipherDirection == CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT) {
        if (authTag != NULL && authTagSz > 0) {
            XMEMCPY(authTag, authTagBuf, authTagSz);
        }
    }

    /* handle cleanup */
    IntelQaSymCipherFree(dev);

    return ret;
}

#ifdef HAVE_AES_CBC
int IntelQaSymAesCbcEncrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz)
{
    int ret = IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_CIPHER, CPA_CY_SYM_CIPHER_AES_CBC,
        CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT,
        CPA_CY_SYM_HASH_NONE, NULL, 0, NULL, 0);

    XMEMCPY((byte*)iv, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    return ret;
}

#ifdef HAVE_AES_DECRYPT
int IntelQaSymAesCbcDecrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz)
{
    byte nextIv[AES_BLOCK_SIZE];
    int ret;

    XMEMCPY(nextIv, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    ret = IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_CIPHER, CPA_CY_SYM_CIPHER_AES_CBC,
        CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT,
        CPA_CY_SYM_HASH_NONE, NULL, 0, NULL, 0);

    XMEMCPY((byte*)iv, nextIv, AES_BLOCK_SIZE);
    return ret;
}
#endif /* HAVE_AES_DECRYPT */
#endif /* HAVE_AES_CBC */


#ifdef HAVE_AESGCM
int IntelQaSymAesGcmEncrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz,
            byte* authTag, word32 authTagSz,
            const byte* authIn, word32 authInSz)
{
    return IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_ALGORITHM_CHAINING, CPA_CY_SYM_CIPHER_AES_GCM,
        CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT,
        CPA_CY_SYM_HASH_AES_GCM, authTag, authTagSz, authIn, authInSz);
}
#ifdef HAVE_AES_DECRYPT
int IntelQaSymAesGcmDecrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz,
            const byte* authTag, word32 authTagSz,
            const byte* authIn, word32 authInSz)
{
    return IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_ALGORITHM_CHAINING, CPA_CY_SYM_CIPHER_AES_GCM,
        CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT,
        CPA_CY_SYM_HASH_AES_GCM, (byte*)authTag, authTagSz, authIn, authInSz);
}
#endif /* HAVE_AES_DECRYPT */
#endif /* HAVE_AESGCM */

#ifndef NO_DES3
int IntelQaSymDes3CbcEncrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz)
{
    return IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_CIPHER, CPA_CY_SYM_CIPHER_3DES_CBC,
        CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT,
        CPA_CY_SYM_HASH_NONE, NULL, 0, NULL, 0);
}

int IntelQaSymDes3CbcDecrypt(IntelQaDev* dev,
            byte* out, const byte* in, word32 sz,
            const byte* key, word32 keySz,
            const byte* iv, word32 ivSz)
{
    return IntelQaSymCipher(dev, out, in, sz,
        key, keySz, iv, ivSz,
        CPA_CY_SYM_OP_CIPHER, CPA_CY_SYM_CIPHER_3DES_CBC,
        CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT,
        CPA_CY_SYM_HASH_NONE, NULL, 0, NULL, 0);
}
#endif /* !NO_DES3 */

#endif /* QAT_ENABLE_CRYPTO */


#ifdef WOLF_CRYPTO_CB

int IntelQaSymSync_CryptoDevCb(int devId, struct wc_CryptoInfo* info, void* ctx)
{
    int rc = NOT_COMPILED_IN; /* return this to bypass HW and use SW */
    IntelQaDev* dev;

    if (info == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

    (void)devId;
    dev = (IntelQaDev*)ctx;

    #ifdef QAT_ENABLE_CRYPTO
    if (info->algo_type == WC_ALGO_TYPE_CIPHER) {
        QLOG("CryptoDevCb Cipher: Type %d\n", info->cipher.type);

        #ifndef NO_AES
        if (info->cipher.type == WC_CIPHER_AES_CBC) {
            Aes* aes = info->cipher.aescbc.aes;
            if (aes == NULL)
                return BAD_FUNC_ARG;

            if (info->cipher.enc) {
                rc = IntelQaSymAesCbcEncrypt(dev,
                        info->cipher.aescbc.out,
                        info->cipher.aescbc.in,
                        info->cipher.aescbc.sz,
                        (byte*)aes->devKey, aes->keylen,
                        (byte*)aes->reg, AES_BLOCK_SIZE);
            }
            else {
                rc = IntelQaSymAesCbcDecrypt(dev,
                        info->cipher.aescbc.out,
                        info->cipher.aescbc.in,
                        info->cipher.aescbc.sz,
                        (byte*)aes->devKey, aes->keylen,
                        (byte*)aes->reg, AES_BLOCK_SIZE);
            }
        }
        #endif /* !NO_AES */

        #ifdef HAVE_AESGCM
        if (info->cipher.type == WC_CIPHER_AES_GCM) {
            if (info->cipher.enc) {
                Aes* aes = info->cipher.aesgcm_enc.aes;
                if (aes == NULL)
                    return BAD_FUNC_ARG;

                rc = IntelQaSymAesGcmEncrypt(dev,
                        info->cipher.aesgcm_enc.out,
                        info->cipher.aesgcm_enc.in,
                        info->cipher.aesgcm_enc.sz,
                        (const byte*)aes->devKey, aes->keylen,
                        info->cipher.aesgcm_enc.iv,
                        info->cipher.aesgcm_enc.ivSz,
                        info->cipher.aesgcm_enc.authTag,
                        info->cipher.aesgcm_enc.authTagSz,
                        info->cipher.aesgcm_enc.authIn,
                        info->cipher.aesgcm_enc.authInSz);
            }
            else {
                Aes* aes = info->cipher.aesgcm_dec.aes;
                if (aes == NULL)
                    return BAD_FUNC_ARG;

                rc = IntelQaSymAesGcmDecrypt(dev,
                        info->cipher.aesgcm_dec.out,
                        info->cipher.aesgcm_dec.in,
                        info->cipher.aesgcm_dec.sz,
                        (const byte*)aes->devKey, aes->keylen,
                        info->cipher.aesgcm_dec.iv,
                        info->cipher.aesgcm_dec.ivSz,
                        info->cipher.aesgcm_dec.authTag,
                        info->cipher.aesgcm_dec.authTagSz,
                        info->cipher.aesgcm_dec.authIn,
                        info->cipher.aesgcm_dec.authInSz);
            }
        }
        #endif /* HAVE_AESGCM */

        #ifndef NO_DES3
        if (info->cipher.type == WC_CIPHER_DES3) {
            Des3* des = info->cipher.des3.des;
            if (des == NULL)
                return BAD_FUNC_ARG;

            if (info->cipher.enc) {
                rc = IntelQaSymDes3CbcEncrypt(dev,
                        info->cipher.des3.out,
                        info->cipher.des3.in,
                        info->cipher.des3.sz,
                        (byte*)des->devKey, DES3_KEYLEN,
                        (byte*)des->reg, DES_BLOCK_SIZE);
            }
            else {
                rc = IntelQaSymDes3CbcDecrypt(dev,
                        info->cipher.des3.out,
                        info->cipher.des3.in,
                        info->cipher.des3.sz,
                        (byte*)des->devKey, DES3_KEYLEN,
                        (byte*)des->reg, DES_BLOCK_SIZE);
            }
        }
        #endif /* !NO_DES3 */
    }
    #endif /* QAT_ENABLE_CRYPTO */

    return rc;
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                 */
/* -------------------------------------------------------------------------- */

int wc_CryptoCb_InitIntelQa(void)
{
    int devId, rc;

    devId = IntelQaInit(NULL);
    if (devId < 0) {
        QLOG("Couldn't init the Intel QA\n");
        devId = INVALID_DEVID;
    }
    else {
        rc = IntelQaOpen(&qaDev, devId);
        if (rc != 0) {
            QLOG("Couldn't open the device\n");
            IntelQaDeInit(devId);
            devId = INVALID_DEVID;
        }
        else {
            rc = wc_CryptoCb_RegisterDevice(devId,
                    IntelQaSymSync_CryptoDevCb, &qaDev);
            if (rc != 0) {
                QLOG("Couldn't register the device\n");
                IntelQaClose(&qaDev);
                IntelQaDeInit(devId);
                devId = INVALID_DEVID;
            }
        }
    }

    return devId;
}


void wc_CryptoCb_CleanupIntelQa(int* id)
{
    if (INVALID_DEVID != *id) {
        wc_CryptoCb_UnRegisterDevice(*id);
        IntelQaClose(&qaDev);
        IntelQaDeInit(*id);
        *id = INVALID_DEVID;
    }
}

#endif /* WOLF_CRYPTO_CB */


/* -------------------------------------------------------------------------- */
/* Memory allocator and deallocator                                           */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* use thread local for QAE variables (removing mutex requirement) */
#ifdef USE_QAE_THREAD_LS
    #include <pthread.h> /* for threadId tracking */
    #define QAE_THREAD_LS THREAD_LS_T
#else
    #define QAE_THREAD_LS
#endif

/* these are used to align memory to a byte boundary */
#define ALIGNMENT_BASE     (16ul)
#define ALIGNMENT_HW       (64ul)
#define WOLF_MAGIC_NUM      0xA576F6C6641736EBUL /* (0xA)WolfAsyn(0xB) */
#define WOLF_HEADER_ALIGN   ALIGNMENT_BASE

#define QAE_NOT_NUMA_PAGE 0xFFFF
typedef struct qaeMemHeader {
#ifdef WOLFSSL_TRACK_MEMORY
    struct qaeMemHeader* next;
    struct qaeMemHeader* prev;
    #ifdef WOLFSSL_DEBUG_MEMORY
        const char* func;
        unsigned int line;
    #endif
#endif
    uint64_t magic;
    void* heap;
#ifdef USE_QAE_THREAD_LS
    pthread_t threadId;
#endif
    size_t size;
    word16 count;
    word16 isNuma:1;
    word16 reservedBits:15; /* use for future bits */
    word16 type;
    word16 numa_page_offset; /* use QAE_NOT_NUMA_PAGE if not NUMA */
} ALIGN16 qaeMemHeader;

#ifdef WOLFSSL_TRACK_MEMORY
    typedef struct qaeMemStats {
        long totalAllocs;     /* number of allocations */
        long totalDeallocs;   /* number of deallocations */
        long totalBytes;      /* total number of bytes allocated */
        long peakBytes;       /* concurrent max bytes */
        long currentBytes;    /* total current bytes in use */
    } qaeMemStats;

    /* track allocations and report at end */
    typedef struct qaeMemList {
        qaeMemHeader* head;
        qaeMemHeader* tail;
        uint32_t count;
    } qaeMemList;
#endif /* WOLFSSL_TRACK_MEMORY */


/* local variables */
#ifndef USE_QAE_THREAD_LS
    static pthread_mutex_t g_memLock = PTHREAD_MUTEX_INITIALIZER;
#endif


#ifdef WOLFSSL_TRACK_MEMORY
    static qaeMemStats g_memStats;
    static qaeMemList g_memList;
    static pthread_mutex_t g_memStatLock = PTHREAD_MUTEX_INITIALIZER;
#endif

static WC_INLINE int qaeMemTypeIsNuma(int type)
{
    int isNuma = 0;

    switch (type) {
        case DYNAMIC_TYPE_ASYNC_NUMA:
        case DYNAMIC_TYPE_ASYNC_NUMA64:
        case DYNAMIC_TYPE_WOLF_BIGINT:
        case DYNAMIC_TYPE_PRIVATE_KEY:
        case DYNAMIC_TYPE_PUBLIC_KEY:
        case DYNAMIC_TYPE_AES_BUFFER:
        case DYNAMIC_TYPE_RSA_BUFFER:
        case DYNAMIC_TYPE_ECC_BUFFER:
        case DYNAMIC_TYPE_SIGNATURE:
        case DYNAMIC_TYPE_DIGEST:
        case DYNAMIC_TYPE_SECRET:
        case DYNAMIC_TYPE_SEED:
        case DYNAMIC_TYPE_SALT:
        {
            isNuma = 1;
            break;
        }
        case DYNAMIC_TYPE_OUT_BUFFER:
        case DYNAMIC_TYPE_IN_BUFFER:
        {
        #if !defined(WC_ASYNC_NO_CRYPT) && !defined(WC_ASYNC_NO_HASH)
            isNuma = 1;
        #else
            isNuma = 0;
        #endif
            break;
        }
        default:
            isNuma = 0;
            break;
    }
    return isNuma;
}


static void _qaeMemFree(void *ptr, void* heap, int type
#ifdef WOLFSSL_DEBUG_MEMORY
    , const char* func, unsigned int line
#endif
)
{
    qaeMemHeader* header = NULL;
    size_t size;
    void* origPtr = ptr;

    if (ptr == NULL)
        return;

    /* adjust for header and align */
    ptr = (byte*)(((size_t)ptr - ((size_t)ptr % WOLF_HEADER_ALIGN)) -
        sizeof(qaeMemHeader));
    header = (qaeMemHeader*)ptr;

    /* check for header magic */
    if (header->magic != WOLF_MAGIC_NUM) {
        printf("Free: Header magic not found! %p\n", ptr);
        return;
    }

    /* cache values for later */
    size = header->size;

#ifdef WOLFSSL_DEBUG_MEMORY
#ifdef WOLFSSL_DEBUG_MEMORY_PRINT
    printf("Free: %p (%u) at %s:%u, heap %p, type %d, count %d\n",
        origPtr, (unsigned int)size, func, line, heap, type, header->count);
#else
    (void)func;
    (void)line;
#endif
#endif
    (void)type;

    /* adjust free count */
    header->count--;

    /* check header count */
    if (header->count > 0) {
        /* go ahead and return if still in use */
        return;
    }

#ifdef WOLFSSL_TRACK_MEMORY
    if (pthread_mutex_lock(&g_memStatLock) == 0) {
        g_memStats.currentBytes -= size;
        g_memStats.totalDeallocs++;

        if (header == g_memList.head && header == g_memList.tail) {
            g_memList.head = NULL;
            g_memList.tail = NULL;
        }
        else if (header == g_memList.head) {
            g_memList.head = header->next;
            g_memList.head->prev = NULL;
        }
        else if (header == g_memList.tail) {
            g_memList.tail = header->prev;
            g_memList.tail->next = NULL;
        }
        else {
            qaeMemHeader* next = header->next;
            qaeMemHeader* prev = header->prev;
            if (next)
                next->prev = prev;
            if (prev)
                prev->next = next;
        }
        g_memList.count--;

        pthread_mutex_unlock(&g_memStatLock);
    }
#endif

    (void)heap;
    (void)size;
    (void)origPtr;

#ifdef WOLFSSL_DEBUG_MEMORY
    /* make sure magic is gone */
    header->magic = 0;
#endif

    /* free type */
    if (header->isNuma && header->numa_page_offset != QAE_NOT_NUMA_PAGE) {
        qaeMemFreeNUMA(&ptr);
    }
    else {
        free(ptr);
    }
}


static void* _qaeMemAlloc(size_t size, void* heap, int type
#ifdef WOLFSSL_DEBUG_MEMORY
    , const char* func, unsigned int line
#endif
)
{
    void* ptr = NULL;
    qaeMemHeader* header = NULL;
    int isNuma;
    int alignment = ALIGNMENT_BASE;
    word16 page_offset = QAE_NOT_NUMA_PAGE;

    /* make sure all allocations are aligned */
    if ((size % WOLF_HEADER_ALIGN) != 0) {
        size += (WOLF_HEADER_ALIGN - (size % WOLF_HEADER_ALIGN));
    }

    isNuma = qaeMemTypeIsNuma(type);
    if (type == DYNAMIC_TYPE_ASYNC_NUMA64)
        alignment = ALIGNMENT_HW;

    /* allocate type */
    if (isNuma) {
        /* Node is typically 0 */
        page_offset = 0;
        ptr = qaeMemAllocNUMA((Cpa32U)(size + sizeof(qaeMemHeader)), 0,
            alignment);
    }
    else {
        isNuma = 0;
        ptr = malloc(size + sizeof(qaeMemHeader));
    }

    /* add header */
    if (ptr) {
        header = (qaeMemHeader*)ptr;
        ptr = (byte*)ptr + sizeof(qaeMemHeader);
        header->magic = WOLF_MAGIC_NUM;
        header->heap = heap;
        header->size = size;
        header->type = type;
        header->count = 1;
        header->isNuma = isNuma;
        header->numa_page_offset = page_offset;
    #ifdef USE_QAE_THREAD_LS
        header->threadId = pthread_self();
    #endif

    #ifdef WOLFSSL_TRACK_MEMORY
        if (pthread_mutex_lock(&g_memStatLock) == 0) {
            g_memStats.totalAllocs++;
            g_memStats.totalBytes   += size;
            g_memStats.currentBytes += size;
            if (g_memStats.currentBytes > g_memStats.peakBytes)
                g_memStats.peakBytes = g_memStats.currentBytes;

        #ifdef WOLFSSL_DEBUG_MEMORY
            header->func = func;
            header->line = line;
        #endif

            /* Setup event */
            header->next = NULL;
            if (g_memList.tail == NULL)  {
                g_memList.head = header;
            }
            else {
                g_memList.tail->next = header;
                header->prev = g_memList.tail;
            }
            g_memList.tail = header;      /* add to the end either way */
            g_memList.count++;

            pthread_mutex_unlock(&g_memStatLock);
        }
    #endif
    }

#ifdef WOLFSSL_DEBUG_MEMORY
#ifdef WOLFSSL_DEBUG_MEMORY_PRINT
    printf("Alloc: %p (%u) at %s:%u, heap %p, type %d\n",
        ptr, (unsigned int)size, func, line, heap, type);
#else
    (void)func;
    (void)line;
#endif
#endif

    (void)heap;

    return ptr;
}

/* Public Functions */
void* wc_CryptoCb_IntelQaMalloc(size_t size, void* heap, int type
#ifdef WOLFSSL_DEBUG_MEMORY
    , const char* func, unsigned int line
#endif
)
{
    void* ptr;

#ifndef USE_QAE_THREAD_LS
    int ret = pthread_mutex_lock(&g_memLock);
    if (ret != 0) {
        printf("Alloc: Error(%d) on mutex lock\n", ret);
        return NULL;
    }
#endif

    ptr = _qaeMemAlloc(size, heap, type
    #ifdef WOLFSSL_DEBUG_MEMORY
        , func, line
    #endif
    );

#ifndef USE_QAE_THREAD_LS
    pthread_mutex_unlock(&g_memLock);
#endif

    return ptr;
}

void wc_CryptoCb_IntelQaFree(void *ptr, void* heap, int type
#ifdef WOLFSSL_DEBUG_MEMORY
    , const char* func, unsigned int line
#endif
)
{
#ifndef USE_QAE_THREAD_LS
    int ret = pthread_mutex_lock(&g_memLock);
    if (ret != 0) {
        printf("Free: Error(%d) on mutex lock\n", ret);
        return;
    }
#endif

    _qaeMemFree(ptr, heap, type
    #ifdef WOLFSSL_DEBUG_MEMORY
        , func, line
    #endif
    );

#ifndef USE_QAE_THREAD_LS
    pthread_mutex_unlock(&g_memLock);
#endif
}

void* wc_CryptoCb_IntelQaRealloc(void *ptr, size_t size, void* heap, int type
#ifdef WOLFSSL_DEBUG_MEMORY
    , const char* func, unsigned int line
#endif
)
{
    void* newPtr = NULL;
    void* origPtr = ptr;
    qaeMemHeader* header = NULL;
    byte allocNew = 1;
    int newIsNuma = -1, ptrIsNuma = -1;
    size_t copySize = 0;

#ifndef USE_QAE_THREAD_LS
    int ret = pthread_mutex_lock(&g_memLock);
    if (ret != 0) {
        printf("Realloc: Error(%d) on mutex lock\n", ret);
        return NULL;
    }
#endif

    (void)heap;

    if (ptr) {
        /* get header pointer and align */
        header = (qaeMemHeader*)(((size_t)ptr -
            ((size_t)ptr % WOLF_HEADER_ALIGN)) - sizeof(qaeMemHeader));
        if (header->magic == WOLF_MAGIC_NUM) {
            newIsNuma = qaeMemTypeIsNuma(type);
            ptrIsNuma = (header->numa_page_offset != QAE_NOT_NUMA_PAGE) ? 1 : 0;

            /* for non-NUMA, treat as normal REALLOC */
            if (newIsNuma == 0 && ptrIsNuma == 0) {
                allocNew = 1;
            }
            /* confirm input is aligned, otherwise allocate new */
            else if (((size_t)ptr % WOLF_HEADER_ALIGN) != 0) {
                allocNew = 1;
            }
            /* if matching NUMA type and size fits, use existing */
            else if (newIsNuma == ptrIsNuma && header->size >= size) {

            #ifdef USE_QAE_THREAD_LS
                if (header->threadId != pthread_self()) {
                    allocNew = 1;
                #if 0
                    printf("Realloc %p from different thread! orig %lx this %lx\n",
                        origPtr, header->threadId, pthread_self());
                #endif
                }
                else
            #endif
                {
                    /* use existing pointer and increment counter */
                    header->count++;
                    newPtr = origPtr;
                    allocNew = 0;
                }
            }

            copySize = header->size;
        }
        else {
            copySize = size;
        }
    }

    if (allocNew) {
        newPtr = _qaeMemAlloc(size, heap, type
        #ifdef WOLFSSL_DEBUG_MEMORY
            , func, line
        #endif
        );
        if (newPtr && ptr) {
            /* only copy min of new and old size to new pointer */
            if (copySize > size)
                copySize = size;
            XMEMCPY(newPtr, ptr, copySize);

            if (newIsNuma == 0 && ptrIsNuma == 0) {
                /* for non-NUMA, treat as normal REALLOC and free old pointer */
                _qaeMemFree(ptr, heap, type
                #ifdef WOLFSSL_DEBUG_MEMORY
                    , func, line
                #endif
                );
            }
        }
    }

#ifndef USE_QAE_THREAD_LS
    pthread_mutex_unlock(&g_memLock);
#endif

#ifdef WOLFSSL_DEBUG_MEMORY
#ifdef WOLFSSL_DEBUG_MEMORY_PRINT
    if (allocNew) {
        printf("Realloc: New %p -> %p (%u) at %s:%u, heap %p, type %d\n",
            origPtr, newPtr, (unsigned int)size, func, line, heap, type);
    }
    else {
        printf("Realloc: Reuse %p (%u) at %s:%u, heap %p, type %d, count %d\n",
             origPtr, (unsigned int)size, func, line, header->heap, header->type, header->count);
    }
#else
    (void)func;
    (void)line;
#endif
#endif

    return newPtr;
}


#ifdef WOLFSSL_TRACK_MEMORY
int InitMemoryTracker(void)
{
    if (pthread_mutex_lock(&g_memStatLock) == 0) {
        g_memStats.totalAllocs  = 0;
        g_memStats.totalDeallocs= 0;
        g_memStats.totalBytes   = 0;
        g_memStats.peakBytes    = 0;
        g_memStats.currentBytes = 0;

        XMEMSET(&g_memList, 0, sizeof(g_memList));

        pthread_mutex_unlock(&g_memStatLock);
    }

    return 0;
}

void ShowMemoryTracker(void)
{
    if (pthread_mutex_lock(&g_memStatLock) == 0) {
        printf("total   Allocs = %9ld\n", g_memStats.totalAllocs);
        printf("total Deallocs = %9ld\n", g_memStats.totalDeallocs);
        printf("total    Bytes = %9ld\n", g_memStats.totalBytes);
        printf("peak     Bytes = %9ld\n", g_memStats.peakBytes);
        printf("current  Bytes = %9ld\n", g_memStats.currentBytes);

        if (g_memList.count > 0) {

            /* print list of allocations */
            qaeMemHeader* header;
            for (header = g_memList.head; header != NULL; header = header->next) {
                printf("Leak: Ptr %p, Size %u, Type %d, Heap %p"
                #ifdef WOLFSSL_DEBUG_MEMORY
                    ", Func %s, Line %d"
                #endif
                    "\n",
                    (byte*)header + sizeof(qaeMemHeader), (unsigned int)header->size,
                    header->type, header->heap
                #ifdef WOLFSSL_DEBUG_MEMORY
                    , header->func, header->line
                #endif
                );
            }
        }

        pthread_mutex_unlock(&g_memStatLock);

        /* cleanup lock */
        pthread_mutex_destroy(&g_memStatLock);
    }
}
#endif /* WOLFSSL_TRACK_MEMORY */

#ifdef QAT_DEMO_MAIN

/* AES GCM */
static const byte aesgcm_k[] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
    0x77, 0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x11, 0x22
};

static const byte aesgcm_iv[] = {
    0xca, 0xfe, 0xca, 0xfe, 0xca, 0xfe, 0xca, 0xfe,
    0xca, 0xfe, 0xca, 0xfe
};

static const byte aesgcm_a[] = {
    0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad,
    0xde, 0xad, 0xde, 0xad, 0xde, 0xad, 0xde, 0xad,
    0xde, 0xad, 0xde, 0xad
};

static const byte aesgcm_p[] = {
    0x79, 0x84, 0x86, 0x44, 0x68, 0x45, 0x15, 0x61,
    0x86, 0x54, 0x66, 0x56, 0x54, 0x54, 0x31, 0x54,
    0x64, 0x64, 0x68, 0x45, 0x15, 0x15, 0x61, 0x61,
    0x51, 0x51, 0x51, 0x51, 0x51, 0x56, 0x14, 0x11,
    0x72, 0x13, 0x51, 0x82, 0x84, 0x56, 0x74, 0x53,
    0x45, 0x34, 0x65, 0x15, 0x46, 0x14, 0x67, 0x55,
    0x16, 0x14, 0x67, 0x54, 0x65, 0x47, 0x14, 0x67,
    0x46, 0x74, 0x65, 0x46
};

static const byte aesgcm_c[] = {
    0x59, 0x85, 0x02, 0x97, 0xE0, 0x4D, 0xFC, 0x5C,
    0x03, 0xCC, 0x83, 0x64, 0xCE, 0x28, 0x0B, 0x95,
    0x78, 0xEC, 0x93, 0x40, 0xA1, 0x8D, 0x21, 0xC5,
    0x48, 0x6A, 0x39, 0xBA, 0x4F, 0x4B, 0x8C, 0x95,
    0x6F, 0x8C, 0xF6, 0x9C, 0xD0, 0xA5, 0x8D, 0x67,
    0xA1, 0x32, 0x11, 0xE7, 0x2E, 0xF6, 0x63, 0xAF,
    0xDE, 0xD4, 0x7D, 0xEC, 0x15, 0x01, 0x58, 0xCB,
    0xE3, 0x7B, 0xC6, 0x94,
};

static byte aesgcm_t[] = {
    0x5D, 0x10, 0x3F, 0xC7, 0x22, 0xC7, 0x21, 0x29
};


/* simple example of using AES-GCM encrypt with Intel QA */
int main(int argc, char** argv)
{
#if !defined(NO_AES) && defined(HAVE_AESGCM)
    int ret;
    IntelQaDev dev;
    byte out[256];
    byte tmp[256];
    word32 tmpLen;
#endif

#ifdef QAT_DEBUG
    wolfSSL_Debugging_ON();
#endif

    IntelQaInit(NULL);

#ifndef NO_AES
#ifdef HAVE_AESGCM
    /* AES Test */
    IntelQaOpen(&dev, 0);
    dev.event.ret = WC_PENDING_E;
    tmpLen = sizeof(aesgcm_t);
    XMEMSET(out, 0, sizeof(out));
    XMEMSET(tmp, 0, sizeof(tmp));

    ret = IntelQaSymAesGcmEncrypt(&dev, out, aesgcm_p, sizeof(aesgcm_p),
        aesgcm_k, sizeof(aesgcm_k), aesgcm_iv, sizeof(aesgcm_iv),
        tmp, tmpLen, aesgcm_a, sizeof(aesgcm_a));
    printf("AES GCM Encrypt: Ret=%d, Tag Len=%d\n", ret, tmpLen);
    IntelQaClose(&dev);
#endif /* HAVE_AESGCM */
#endif /* NO_AES */

    IntelQaDeInit(0);

    return 0;
}

#endif

#endif /* HAVE_INTEL_QA_SYNC */
