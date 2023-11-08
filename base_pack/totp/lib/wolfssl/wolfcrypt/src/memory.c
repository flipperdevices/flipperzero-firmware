/* memory.c
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

#ifdef WOLFSSL_LINUXKM
    /* inhibit "#undef current" in linuxkm_wc_port.h, included from wc_port.h,
     * because needed in linuxkm_memory.c, included below.
     */
    #define WOLFSSL_NEED_LINUX_CURRENT
#endif

#include <wolfssl/wolfcrypt/settings.h>

/*
Possible memory options:
 * NO_WOLFSSL_MEMORY:               Disables wolf memory callback support. When not defined settings.h defines USE_WOLFSSL_MEMORY.
 * WOLFSSL_STATIC_MEMORY:           Turns on the use of static memory buffers and functions.
                                        This allows for using static memory instead of dynamic.
 * WOLFSSL_STATIC_ALIGN:            Define defaults to 16 to indicate static memory alignment.
 * HAVE_IO_POOL:                    Enables use of static thread safe memory pool for input/output buffers.
 * XMALLOC_OVERRIDE:                Allows override of the XMALLOC, XFREE and XREALLOC macros.
 * XMALLOC_USER:                    Allows custom XMALLOC, XFREE and XREALLOC functions to be defined.
 * WOLFSSL_NO_MALLOC:               Disables the fall-back case to use STDIO malloc/free when no callbacks are set.
 * WOLFSSL_TRACK_MEMORY:            Enables memory tracking for total stats and list of allocated memory.
 * WOLFSSL_DEBUG_MEMORY:            Enables extra function and line number args for memory callbacks.
 * WOLFSSL_DEBUG_MEMORY_PRINT:      Enables printing of each malloc/free.
 * WOLFSSL_MALLOC_CHECK:            Reports malloc or alignment failure using WOLFSSL_STATIC_ALIGN
 * WOLFSSL_FORCE_MALLOC_FAIL_TEST:  Used for internal testing to induce random malloc failures.
 * WOLFSSL_HEAP_TEST:               Used for internal testing of heap hint
 * WOLFSSL_MEM_FAIL_COUNT:          Fail memory allocation at a count from
 *                                  environment variable: MEM_FAIL_CNT.
 */

#ifdef WOLFSSL_ZEPHYR
#undef realloc
void *z_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        ptr = malloc(size);
    else
        ptr = realloc(ptr, size);

    return ptr;
}
#define realloc z_realloc
#endif

#ifdef USE_WOLFSSL_MEMORY

#include <wolfssl/wolfcrypt/memory.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#if defined(WOLFSSL_DEBUG_MEMORY) && defined(WOLFSSL_DEBUG_MEMORY_PRINT)
#include <stdio.h>
#endif

#ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
    static int gMemFailCountSeed;
    static int gMemFailCount;
    void wolfSSL_SetMemFailCount(int memFailCount)
    {
        if (gMemFailCountSeed == 0) {
            gMemFailCountSeed = memFailCount;
            gMemFailCount = memFailCount;
        }
    }
#endif
#if defined(WOLFSSL_MALLOC_CHECK) || defined(WOLFSSL_TRACK_MEMORY_FULL) || \
                                                     defined(WOLFSSL_MEMORY_LOG)
    #include <stdio.h>
#endif


/* Set these to default values initially. */
static wolfSSL_Malloc_cb  malloc_function = NULL;
static wolfSSL_Free_cb    free_function = NULL;
static wolfSSL_Realloc_cb realloc_function = NULL;

int wolfSSL_SetAllocators(wolfSSL_Malloc_cb  mf,
                          wolfSSL_Free_cb    ff,
                          wolfSSL_Realloc_cb rf)
{
    malloc_function = mf;
    free_function = ff;
    realloc_function = rf;
    return 0;
}

int wolfSSL_GetAllocators(wolfSSL_Malloc_cb*  mf,
                          wolfSSL_Free_cb*    ff,
                          wolfSSL_Realloc_cb* rf)
{
    if (mf) *mf = malloc_function;
    if (ff) *ff = free_function;
    if (rf) *rf = realloc_function;
    return 0;
}

#ifdef WOLFSSL_MEM_FAIL_COUNT
static wolfSSL_Mutex memFailMutex;
int mem_fail_allocs = 0;
int mem_fail_frees = 0;
int mem_fail_cnt = 0;

void wc_MemFailCount_Init()
{
    char* cnt;
    wc_InitMutex(&memFailMutex);
    cnt = getenv("MEM_FAIL_CNT");
    if (cnt != NULL) {
        fprintf(stderr, "MemFailCount At: %d\n", mem_fail_cnt);
        mem_fail_cnt = atoi(cnt);
    }
}
static int wc_MemFailCount_AllocMem(void)
{
    int ret = 1;

    wc_LockMutex(&memFailMutex);
    if ((mem_fail_cnt > 0) && (mem_fail_cnt <= mem_fail_allocs + 1)) {
        ret = 0;
    }
    else {
        mem_fail_allocs++;
    }
    wc_UnLockMutex(&memFailMutex);

    return ret;
}
static void wc_MemFailCount_FreeMem(void)
{
    wc_LockMutex(&memFailMutex);
    mem_fail_frees++;
    wc_UnLockMutex(&memFailMutex);
}
void wc_MemFailCount_Free()
{
    wc_FreeMutex(&memFailMutex);
    fprintf(stderr, "MemFailCount Total: %d\n", mem_fail_allocs);
    fprintf(stderr, "MemFailCount Frees: %d\n", mem_fail_frees);
}
#endif

#ifndef WOLFSSL_STATIC_MEMORY
#ifdef WOLFSSL_CHECK_MEM_ZERO

#ifndef WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN
/* Number of entries in table of addresses to check. */
#define WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN    256
#endif

/* Alignment to maintain when adding length to allocated pointer.
 * Intel x64 wants to use aligned loads of XMM registers.
 */
#define MEM_ALIGN       16

/* An address that is meant to be all zeros for its length. */
typedef struct MemZero {
    /* Name of address to check. */
    const char* name;
    /* Address to check. */
    const void* addr;
    /* Length of data that must be zero. */
    size_t len;
} MemZero;

/* List of addresses to check. */
static MemZero memZero[WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN];
/* Next index to place address at.
 * -1 indicates uninitialized.
 * If nextIdx is equal to WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN then all entries
 * have been used.
 */
static int nextIdx = -1;
/* Mutex to protect modifying list of addresses to check. */
static wolfSSL_Mutex zeroMutex;

/* Initialize the table of addresses and the mutex.
 */
void wc_MemZero_Init()
{
    /* Clear the table to more easily see what is valid. */
    XMEMSET(memZero, 0, sizeof(memZero));
    /* Initialize mutex. */
    wc_InitMutex(&zeroMutex);
    /* Next index is first entry. */
    nextIdx = 0;
}

/* Free the mutex and check we have not any uncheck addresses.
 */
void wc_MemZero_Free()
{
    /* Free mutex. */
    wc_FreeMutex(&zeroMutex);
    /* Make sure we checked all addresses. */
    if (nextIdx > 0) {
        int i;
        fprintf(stderr, "[MEM_ZERO] Unseen: %d\n", nextIdx);
        for (i = 0; i < nextIdx; i++) {
            fprintf(stderr, "  %s - %p:%ld\n", memZero[i].name, memZero[i].addr,
                memZero[i].len);
        }
    }
    /* Uninitialized value in next index. */
    nextIdx = -1;
}

/* Add an address to check.
 *
 * @param [in] name  Name of address to check.
 * @param [in] addr  Address that needs to be checked.
 * @param [in] len   Length of data that must be zero.
 */
void wc_MemZero_Add(const char* name, const void* addr, size_t len)
{
    /* Initialize if not done. */
    if (nextIdx == -1) {
        wc_MemZero_Init();
    }

    /* Add an entry to the table while locked. */
    wc_LockMutex(&zeroMutex);
    if (nextIdx < WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN) {
        /* Fill in the next entry and update next index. */
        memZero[nextIdx].name = name;
        memZero[nextIdx].addr = addr;
        memZero[nextIdx].len  = len;
        nextIdx++;
    }
    else {
        /* Abort when too many entries. */
        fprintf(stderr, "\n[MEM_ZERO] Too many addresses to check\n");
        fprintf(stderr, "[MEM_ZERO] WOLFSSL_MEM_CHECK_ZERO_CACHE_LEN\n");
        abort();
    }
    wc_UnLockMutex(&zeroMutex);
}

/* Check the memory in the range of the address for memory that must be zero.
 *
 * @param [in] addr  Start address of memory that is to be checked.
 * @param [in] len   Length of data associated with address.
 */
void wc_MemZero_Check(void* addr, size_t len)
{
    int i;
    size_t j;

    wc_LockMutex(&zeroMutex);
    /* Look at each address for overlap with address passes in. */
    for (i = 0; i < nextIdx; i++) {
        if ((memZero[i].addr < addr) ||
               ((size_t)memZero[i].addr >= (size_t)addr + len)) {
            /* Check address not part of memory to check. */
            continue;
        }

        /* Address is in range of memory being freed - check each byte zero. */
        for (j = 0; j < memZero[i].len; j++) {
            if (((unsigned char*)memZero[i].addr)[j] != 0) {
                /* Byte not zero - abort! */
                fprintf(stderr, "\n[MEM_ZERO] %s:%p + %ld is not zero\n",
                    memZero[i].name, memZero[i].addr, j);
                fprintf(stderr, "[MEM_ZERO] Checking %p:%ld\n", addr, len);
                abort();
            }
        }
        /* Update next index to write to. */
        nextIdx--;
        if (nextIdx > 0) {
            /* Remove entry. */
            XMEMCPY(memZero + i, memZero + i + 1,
                sizeof(MemZero) * (nextIdx - i));
            /* Clear out top to make it easier to see what is to be checked. */
            XMEMSET(&memZero[nextIdx], 0, sizeof(MemZero));
        }
        /* Need to check this index again with new data. */
        i--;
    }
    wc_UnLockMutex(&zeroMutex);
}
#endif /* WOLFSSL_CHECK_MEM_ZERO */

#ifdef WOLFSSL_DEBUG_MEMORY
void* wolfSSL_Malloc(size_t size, const char* func, unsigned int line)
#else
void* wolfSSL_Malloc(size_t size)
#endif
{
    void* res = 0;

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (!wc_MemFailCount_AllocMem()) {
        WOLFSSL_MSG("MemFailCnt: Fail malloc");
        return NULL;
    }
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    /* Space for requested size. */
    size += MEM_ALIGN;
#endif

    if (malloc_function) {
    #ifdef WOLFSSL_DEBUG_MEMORY
        res = malloc_function(size, func, line);
    #else
        res = malloc_function(size);
    #endif
    }
    else {
    #ifndef WOLFSSL_NO_MALLOC
        #ifdef WOLFSSL_TRAP_MALLOC_SZ
        if (size > WOLFSSL_TRAP_MALLOC_SZ) {
            WOLFSSL_MSG("Malloc too big!");
            return NULL;
        }
        #endif

        res = malloc(size);
    #else
        WOLFSSL_MSG("No malloc available");
    #endif
    }

#ifdef WOLFSSL_CHECK_MEM_ZERO
    /* Restore size to requested value. */
    size -= MEM_ALIGN;
    if (res != NULL) {
        /* Place size at front of allocated data and move pointer passed it. */
        *(size_t*)res = size;
        res = ((unsigned char*)res) + MEM_ALIGN;
    }
#endif

#ifdef WOLFSSL_DEBUG_MEMORY
#if defined(WOLFSSL_DEBUG_MEMORY_PRINT) && !defined(WOLFSSL_TRACK_MEMORY)
    fprintf(stderr, "Alloc: %p -> %u at %s:%u\n", res, (word32)size, func, line);
#else
    (void)func;
    (void)line;
#endif
#endif

#ifdef WOLFSSL_MALLOC_CHECK
    if (res == NULL)
        WOLFSSL_MSG("wolfSSL_malloc failed");
#endif

#ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
    if (res && --gMemFailCount == 0) {
        fprintf(stderr, "\n---FORCED MEM FAIL TEST---\n");
        if (free_function) {
        #ifdef WOLFSSL_DEBUG_MEMORY
            free_function(res, func, line);
        #else
            free_function(res);
        #endif
        }
        else {
            free(res); /* clear */
        }
        gMemFailCount = gMemFailCountSeed; /* reset */
        return NULL;
    }
#endif

    return res;
}

#ifdef WOLFSSL_DEBUG_MEMORY
void wolfSSL_Free(void *ptr, const char* func, unsigned int line)
#else
void wolfSSL_Free(void *ptr)
#endif
{
#ifdef WOLFSSL_DEBUG_MEMORY
#if defined(WOLFSSL_DEBUG_MEMORY_PRINT) && !defined(WOLFSSL_TRACK_MEMORY)
    fprintf(stderr, "Free: %p at %s:%u\n", ptr, func, line);
#else
    (void)func;
    (void)line;
#endif
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    /* Move pointer back to originally allocated pointer. */
    ptr = ((unsigned char*)ptr) - MEM_ALIGN;
    /* Check that the pointer is zero where required. */
    wc_MemZero_Check(((unsigned char*)ptr) + MEM_ALIGN, *(size_t*)ptr);
#endif
#ifdef WOLFSSL_MEM_FAIL_COUNT
    wc_MemFailCount_FreeMem();
#endif

    if (free_function) {
    #ifdef WOLFSSL_DEBUG_MEMORY
        free_function(ptr, func, line);
    #else
        free_function(ptr);
    #endif
    }
    else {
    #ifndef WOLFSSL_NO_MALLOC
        free(ptr);
    #else
        WOLFSSL_MSG("No free available");
    #endif
    }
}

#ifdef WOLFSSL_DEBUG_MEMORY
void* wolfSSL_Realloc(void *ptr, size_t size, const char* func, unsigned int line)
#else
void* wolfSSL_Realloc(void *ptr, size_t size)
#endif
{
#ifdef WOLFSSL_CHECK_MEM_ZERO
    /* Can't check data that has been freed during realloc.
     * Manually allocated new memory, copy data and free original pointer.
     */
#ifdef WOLFSSL_DEBUG_MEMORY
    void* res = wolfSSL_Malloc(size, func, line);
#else
    void* res = wolfSSL_Malloc(size);
#endif
    if (ptr != NULL) {
        /* Copy the minimum of old and new size. */
        size_t copySize = *(size_t*)(((unsigned char*)ptr) - MEM_ALIGN);
        if (size < copySize) {
            copySize = size;
        }
        XMEMCPY(res, ptr, copySize);
        /* Dispose of old pointer. */
    #ifdef WOLFSSL_DEBUG_MEMORY
        wolfSSL_Free(ptr, func, line);
    #else
        wolfSSL_Free(ptr);
    #endif
    }

    /* Return new pointer with data copied into it. */
    return res;
#else
    void* res = 0;

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (!wc_MemFailCount_AllocMem()) {
        WOLFSSL_MSG("MemFailCnt: Fail realloc");
        return NULL;
    }
#endif

    if (realloc_function) {
    #ifdef WOLFSSL_DEBUG_MEMORY
        res = realloc_function(ptr, size, func, line);
    #else
        res = realloc_function(ptr, size);
    #endif
    }
    else {
    #ifndef WOLFSSL_NO_MALLOC
        res = realloc(ptr, size);
    #else
        WOLFSSL_MSG("No realloc available");
    #endif
    }

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (ptr != NULL) {
        wc_MemFailCount_FreeMem();
    }
#endif

    return res;
#endif
}
#endif /* WOLFSSL_STATIC_MEMORY */

#ifdef WOLFSSL_STATIC_MEMORY

struct wc_Memory {
    byte*  buffer;
    struct wc_Memory* next;
    word32 sz;
};


/* returns amount of memory used on success. On error returns negative value
   wc_Memory** list is the list that new buckets are prepended to
 */
static int create_memory_buckets(byte* buffer, word32 bufSz,
                              word32 buckSz, word32 buckNum, wc_Memory** list) {
    word32 i;
    byte*  pt  = buffer;
    int    ret = 0;
    word32 memSz = (word32)sizeof(wc_Memory);
    word32 padSz = -(int)memSz & (WOLFSSL_STATIC_ALIGN - 1);

    /* if not enough space available for bucket size then do not try */
    if (buckSz + memSz + padSz > bufSz) {
        return ret;
    }

    for (i = 0; i < buckNum; i++) {
        if ((buckSz + memSz + padSz) <= (bufSz - ret)) {
            /* create a new struct and set its values */
            wc_Memory* mem = (struct wc_Memory*)(pt);
            mem->sz = buckSz;
            mem->buffer = (byte*)pt + padSz + memSz;
            mem->next = NULL;

            /* add the newly created struct to front of list */
            if (*list == NULL) {
                *list = mem;
            } else {
                mem->next = *list;
                *list = mem;
            }

            /* advance pointer and keep track of memory used */
            ret += buckSz + padSz + memSz;
            pt  += buckSz + padSz + memSz;
        }
        else {
            break; /* not enough space left for more buckets of this size */
        }
    }

    return ret;
}

int wolfSSL_init_memory_heap(WOLFSSL_HEAP* heap)
{
    word32 wc_MemSz[WOLFMEM_DEF_BUCKETS] = { WOLFMEM_BUCKETS };
    word32 wc_Dist[WOLFMEM_DEF_BUCKETS]  = { WOLFMEM_DIST };

    if (heap == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(heap, 0, sizeof(WOLFSSL_HEAP));

    XMEMCPY(heap->sizeList, wc_MemSz, sizeof(wc_MemSz));
    XMEMCPY(heap->distList, wc_Dist,  sizeof(wc_Dist));

    if (wc_InitMutex(&(heap->memory_mutex)) != 0) {
        WOLFSSL_MSG("Error creating heap memory mutex");
        return BAD_MUTEX_E;
    }

    return 0;
}

int wc_LoadStaticMemory(WOLFSSL_HEAP_HINT** pHint,
    unsigned char* buf, unsigned int sz, int flag, int maxSz)
{
    int ret;
    WOLFSSL_HEAP*      heap;
    WOLFSSL_HEAP_HINT* hint;
    word32 idx = 0;

    if (pHint == NULL || buf == NULL) {
        return BAD_FUNC_ARG;
    }

    if ((sizeof(WOLFSSL_HEAP) + sizeof(WOLFSSL_HEAP_HINT)) > sz - idx) {
        return BUFFER_E; /* not enough memory for structures */
    }

    /* check if hint has already been assigned */
    if (*pHint == NULL) {
        heap = (WOLFSSL_HEAP*)buf;
        idx += sizeof(WOLFSSL_HEAP);
        hint = (WOLFSSL_HEAP_HINT*)(buf + idx);
        idx += sizeof(WOLFSSL_HEAP_HINT);

        ret = wolfSSL_init_memory_heap(heap);
        if (ret != 0) {
            return ret;
        }

        XMEMSET(hint, 0, sizeof(WOLFSSL_HEAP_HINT));
        hint->memory = heap;
    }
    else {
    #ifdef WOLFSSL_HEAP_TEST
        /* do not load in memory if test has been set */
        if (heap == (void*)WOLFSSL_HEAP_TEST) {
            return 0;
        }
    #endif

        hint = (WOLFSSL_HEAP_HINT*)(*pHint);
        heap = hint->memory;
    }

    ret = wolfSSL_load_static_memory(buf + idx, sz - idx, flag, heap);
    if (ret != 1) {
        WOLFSSL_MSG("Error partitioning memory");
        return -1;
    }

    /* determine what max applies too */
    if ((flag & WOLFMEM_IO_POOL) || (flag & WOLFMEM_IO_POOL_FIXED)) {
        heap->maxIO = maxSz;
    }
    else { /* general memory used in handshakes */
        heap->maxHa = maxSz;
    }

    heap->flag |= flag;
    *pHint = hint;

    (void)maxSz;

    return 0;
}

int wolfSSL_load_static_memory(byte* buffer, word32 sz, int flag,
                                                             WOLFSSL_HEAP* heap)
{
    word32 ava = sz;
    byte*  pt  = buffer;
    int    ret = 0;
    word32 memSz = (word32)sizeof(wc_Memory);
    word32 padSz = -(int)memSz & (WOLFSSL_STATIC_ALIGN - 1);

    WOLFSSL_ENTER("wolfSSL_load_static_memory");

    if (buffer == NULL) {
        return BAD_FUNC_ARG;
    }

    /* align pt */
    while ((wc_ptr_t)pt % WOLFSSL_STATIC_ALIGN && pt < (buffer + sz)) {
        *pt = 0x00;
        pt++;
        ava--;
    }

#ifdef WOLFSSL_DEBUG_MEMORY
    fprintf(stderr, "Allocated %d bytes for static memory @ %p\n", ava, pt);
#endif

    /* divide into chunks of memory and add them to available list */
    while (ava >= (heap->sizeList[0] + padSz + memSz)) {
        /* creating only IO buffers from memory passed in, max TLS is 16k */
        if (flag & WOLFMEM_IO_POOL || flag & WOLFMEM_IO_POOL_FIXED) {
            if ((ret = create_memory_buckets(pt, ava,
                                          WOLFMEM_IO_SZ, 1, &(heap->io))) < 0) {
                WOLFSSL_LEAVE("wolfSSL_load_static_memory", ret);
                return ret;
            }

            /* check if no more room left for creating IO buffers */
            if (ret == 0) {
                break;
            }

            /* advance pointer in buffer for next buckets and keep track
               of how much memory is left available */
            pt  += ret;
            ava -= ret;
        }
        else {
            int i;
            /* start at largest and move to smaller buckets */
            for (i = (WOLFMEM_MAX_BUCKETS - 1); i >= 0; i--) {
                if ((heap->sizeList[i] + padSz + memSz) <= ava) {
                    if ((ret = create_memory_buckets(pt, ava, heap->sizeList[i],
                                     heap->distList[i], &(heap->ava[i]))) < 0) {
                        WOLFSSL_LEAVE("wolfSSL_load_static_memory", ret);
                        return ret;
                    }

                    /* advance pointer in buffer for next buckets and keep track
                       of how much memory is left available */
                    pt  += ret;
                    ava -= ret;
                }
            }
        }
    }

    return 1;
}


/* returns the size of management memory needed for each bucket.
 * This is memory that is used to keep track of and align memory buckets. */
int wolfSSL_MemoryPaddingSz(void)
{
    word32 memSz = (word32)sizeof(wc_Memory);
    word32 padSz = -(int)memSz & (WOLFSSL_STATIC_ALIGN - 1);
    return memSz + padSz;
}


/* Used to calculate memory size for optimum use with buckets.
   returns the suggested size rounded down to the nearest bucket. */
int wolfSSL_StaticBufferSz(byte* buffer, word32 sz, int flag)
{
    word32 bucketSz[WOLFMEM_MAX_BUCKETS] = {WOLFMEM_BUCKETS};
    word32 distList[WOLFMEM_MAX_BUCKETS] = {WOLFMEM_DIST};

    word32 ava = sz;
    byte*  pt  = buffer;
    word32 memSz = (word32)sizeof(wc_Memory);
    word32 padSz = -(int)memSz & (WOLFSSL_STATIC_ALIGN - 1);

    WOLFSSL_ENTER("wolfSSL_static_size");

    if (buffer == NULL) {
        return BAD_FUNC_ARG;
    }

    /* align pt */
    while ((wc_ptr_t)pt % WOLFSSL_STATIC_ALIGN && pt < (buffer + sz)) {
        pt++;
        ava--;
    }

    /* creating only IO buffers from memory passed in, max TLS is 16k */
    if (flag & WOLFMEM_IO_POOL || flag & WOLFMEM_IO_POOL_FIXED) {
        if (ava < (memSz + padSz + WOLFMEM_IO_SZ)) {
            return 0; /* not enough room for even one bucket */
        }

        ava = ava % (memSz + padSz + WOLFMEM_IO_SZ);
    }
    else {
        int i, k;

        if (ava < (bucketSz[0] + padSz + memSz)) {
            return 0; /* not enough room for even one bucket */
        }

        while ((ava >= (bucketSz[0] + padSz + memSz)) && (ava > 0)) {
            /* start at largest and move to smaller buckets */
            for (i = (WOLFMEM_MAX_BUCKETS - 1); i >= 0; i--) {
                for (k = distList[i]; k > 0; k--) {
                    if ((bucketSz[i] + padSz + memSz) <= ava) {
                        ava -= bucketSz[i] + padSz + memSz;
                    }
                }
            }
        }
    }

    return sz - ava; /* round down */
}


int FreeFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io)
{
    WOLFSSL_MSG("Freeing fixed IO buffer");

    /* check if fixed buffer was set */
    if (*io == NULL) {
        return 1;
    }

    if (heap == NULL) {
        WOLFSSL_MSG("No heap to return fixed IO too");
    }
    else {
        /* put IO buffer back into IO pool */
        (*io)->next = heap->io;
        heap->io    = *io;
        *io         = NULL;
    }

    return 1;
}


int SetFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io)
{
    WOLFSSL_MSG("Setting fixed IO for SSL");
    if (heap == NULL) {
        return MEMORY_E;
    }

    *io = heap->io;

    if (*io != NULL) {
        heap->io = (*io)->next;
        (*io)->next = NULL;
    }
    else { /* failed to grab an IO buffer */
        return 0;
    }

    return 1;
}


int wolfSSL_GetMemStats(WOLFSSL_HEAP* heap, WOLFSSL_MEM_STATS* stats)
{
        word32     i;
        wc_Memory* pt;

        XMEMSET(stats, 0, sizeof(WOLFSSL_MEM_STATS));

        stats->totalAlloc = heap->alloc;
        stats->totalFr    = heap->frAlc;
        stats->curAlloc   = stats->totalAlloc - stats->totalFr;
        stats->maxHa      = heap->maxHa;
        stats->maxIO      = heap->maxIO;
        for (i = 0; i < WOLFMEM_MAX_BUCKETS; i++) {
            stats->blockSz[i] = heap->sizeList[i];
            for (pt = heap->ava[i]; pt != NULL; pt = pt->next) {
                stats->avaBlock[i] += 1;
            }
        }

        for (pt = heap->io; pt != NULL; pt = pt->next) {
            stats->avaIO++;
        }

        stats->flag       = heap->flag; /* flag used */

    return 1;
}


#ifdef WOLFSSL_DEBUG_MEMORY
void* wolfSSL_Malloc(size_t size, void* heap, int type, const char* func, unsigned int line)
#else
void* wolfSSL_Malloc(size_t size, void* heap, int type)
#endif
{
    void* res = 0;
    wc_Memory* pt = NULL;
    int   i;

    /* check for testing heap hint was set */
#ifdef WOLFSSL_HEAP_TEST
    if (heap == (void*)WOLFSSL_HEAP_TEST) {
        return malloc(size);
    }
#endif

    /* if no heap hint then use dynamic memory*/
    if (heap == NULL) {
        #ifdef WOLFSSL_HEAP_TEST
            /* allow using malloc for creating ctx and method */
            if (type == DYNAMIC_TYPE_CTX || type == DYNAMIC_TYPE_METHOD ||
                                            type == DYNAMIC_TYPE_CERT_MANAGER) {
                WOLFSSL_MSG("ERROR allowing null heap hint for ctx/method");
                res = malloc(size);
            }
            else {
                WOLFSSL_MSG("ERROR null heap hint passed into XMALLOC");
                res = NULL;
            }
        #else
        #ifndef WOLFSSL_NO_MALLOC
            #ifdef FREERTOS
                res = pvPortMalloc(size);
            #elif defined(WOLFSSL_EMBOS)
                res = OS_HEAP_malloc(size);
            #else
                res = malloc(size);
            #endif

            #ifdef WOLFSSL_DEBUG_MEMORY
                fprintf(stderr, "Alloc: %p -> %u at %s:%d\n", res, (word32)size, func, line);
            #endif
        #else
            WOLFSSL_MSG("No heap hint found to use and no malloc");
            #ifdef WOLFSSL_DEBUG_MEMORY
            fprintf(stderr, "ERROR: at %s:%d\n", func, line);
            #endif
        #endif /* WOLFSSL_NO_MALLOC */
        #endif /* WOLFSSL_HEAP_TEST */
    }
    else {
        WOLFSSL_HEAP_HINT* hint = (WOLFSSL_HEAP_HINT*)heap;
        WOLFSSL_HEAP*      mem  = hint->memory;

        if (wc_LockMutex(&(mem->memory_mutex)) != 0) {
            WOLFSSL_MSG("Bad memory_mutex lock");
            return NULL;
        }

        /* case of using fixed IO buffers */
        if (mem->flag & WOLFMEM_IO_POOL_FIXED &&
                                             (type == DYNAMIC_TYPE_OUT_BUFFER ||
                                              type == DYNAMIC_TYPE_IN_BUFFER)) {
            if (type == DYNAMIC_TYPE_OUT_BUFFER) {
                pt = hint->outBuf;
            }
            if (type == DYNAMIC_TYPE_IN_BUFFER) {
                pt = hint->inBuf;
            }
        }
        else {
            /* check if using IO pool flag */
            if (mem->flag & WOLFMEM_IO_POOL &&
                                             (type == DYNAMIC_TYPE_OUT_BUFFER ||
                                              type == DYNAMIC_TYPE_IN_BUFFER)) {
                if (mem->io != NULL) {
                    pt      = mem->io;
                    mem->io = pt->next;
                }
            }

            /* general static memory */
            if (pt == NULL) {
                for (i = 0; i < WOLFMEM_MAX_BUCKETS; i++) {
                    if ((word32)size <= mem->sizeList[i]) {
                        if (mem->ava[i] != NULL) {
                            pt = mem->ava[i];
                            mem->ava[i] = pt->next;
                            break;
                        }
                    #ifdef WOLFSSL_DEBUG_STATIC_MEMORY
                        else {
                            fprintf(stderr, "Size: %ld, Empty: %d\n", size,
                                                              mem->sizeList[i]);
                        }
                    #endif
                    }
                }
            }
        }

        if (pt != NULL) {
            mem->inUse += pt->sz;
            mem->alloc += 1;
            res = pt->buffer;

        #ifdef WOLFSSL_DEBUG_MEMORY
            fprintf(stderr, "Alloc: %p -> %u at %s:%d\n", pt->buffer, pt->sz, func, line);
        #endif

            /* keep track of connection statistics if flag is set */
            if (mem->flag & WOLFMEM_TRACK_STATS) {
                WOLFSSL_MEM_CONN_STATS* stats = hint->stats;
                if (stats != NULL) {
                    stats->curMem += pt->sz;
                    if (stats->peakMem < stats->curMem) {
                        stats->peakMem = stats->curMem;
                    }
                    stats->curAlloc++;
                    if (stats->peakAlloc < stats->curAlloc) {
                        stats->peakAlloc = stats->curAlloc;
                    }
                    stats->totalAlloc++;
                }
            }
        }
        else {
            WOLFSSL_MSG("ERROR ran out of static memory");
            #ifdef WOLFSSL_DEBUG_MEMORY
            fprintf(stderr, "Looking for %lu bytes at %s:%d\n", size, func, line);
            #endif
        }

        wc_UnLockMutex(&(mem->memory_mutex));
    }

    #ifdef WOLFSSL_MALLOC_CHECK
        if ((wc_ptr_t)res % WOLFSSL_STATIC_ALIGN) {
            WOLFSSL_MSG("ERROR memory is not aligned");
            res = NULL;
        }
    #endif


    (void)i;
    (void)pt;
    (void)type;

    return res;
}


#ifdef WOLFSSL_DEBUG_MEMORY
void wolfSSL_Free(void *ptr, void* heap, int type, const char* func, unsigned int line)
#else
void wolfSSL_Free(void *ptr, void* heap, int type)
#endif
{
    int i;
    wc_Memory* pt;

    if (ptr) {
        /* check for testing heap hint was set */
    #ifdef WOLFSSL_HEAP_TEST
        if (heap == (void*)WOLFSSL_HEAP_TEST) {
        #ifdef WOLFSSL_DEBUG_MEMORY
            fprintf(stderr, "Free: %p at %s:%d\n", pt, func, line);
        #endif
            return free(ptr);
        }
    #endif

        if (heap == NULL) {
        #ifdef WOLFSSL_HEAP_TEST
            /* allow using malloc for creating ctx and method */
            if (type == DYNAMIC_TYPE_CTX || type == DYNAMIC_TYPE_METHOD ||
                                            type == DYNAMIC_TYPE_CERT_MANAGER) {
                WOLFSSL_MSG("ERROR allowing null heap hint for ctx/method");
            }
            else {
                WOLFSSL_MSG("ERROR null heap hint passed into XFREE");
            }
        #endif
        #ifndef WOLFSSL_NO_MALLOC
            #ifdef FREERTOS
                vPortFree(ptr);
            #elif defined(WOLFSSL_EMBOS)
                OS_HEAP_free(ptr);
            #else
                free(ptr);
            #endif
        #else
            WOLFSSL_MSG("Error trying to call free when turned off");
        #endif /* WOLFSSL_NO_MALLOC */
        }
        else {
            WOLFSSL_HEAP_HINT* hint = (WOLFSSL_HEAP_HINT*)heap;
            WOLFSSL_HEAP*      mem  = hint->memory;
            word32 padSz = -(int)sizeof(wc_Memory) & (WOLFSSL_STATIC_ALIGN - 1);

            /* get memory struct and add it to available list */
            pt = (wc_Memory*)((byte*)ptr - sizeof(wc_Memory) - padSz);
            if (wc_LockMutex(&(mem->memory_mutex)) != 0) {
                WOLFSSL_MSG("Bad memory_mutex lock");
                return;
            }

            /* case of using fixed IO buffers */
            if (mem->flag & WOLFMEM_IO_POOL_FIXED &&
                                             (type == DYNAMIC_TYPE_OUT_BUFFER ||
                                              type == DYNAMIC_TYPE_IN_BUFFER)) {
                /* fixed IO pools are free'd at the end of SSL lifetime
                   using FreeFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io) */
            }
            else if (mem->flag & WOLFMEM_IO_POOL && pt->sz == WOLFMEM_IO_SZ &&
                                             (type == DYNAMIC_TYPE_OUT_BUFFER ||
                                              type == DYNAMIC_TYPE_IN_BUFFER)) {
                pt->next = mem->io;
                mem->io  = pt;
            }
            else { /* general memory free */
                for (i = 0; i < WOLFMEM_MAX_BUCKETS; i++) {
                    if (pt->sz == mem->sizeList[i]) {
                        pt->next = mem->ava[i];
                        mem->ava[i] = pt;
                        break;
                    }
                }
            }
            mem->inUse -= pt->sz;
            mem->frAlc += 1;

        #ifdef WOLFSSL_DEBUG_MEMORY
            fprintf(stderr, "Free: %p -> %u at %s:%d\n", pt->buffer, pt->sz, func, line);
        #endif

            /* keep track of connection statistics if flag is set */
            if (mem->flag & WOLFMEM_TRACK_STATS) {
                WOLFSSL_MEM_CONN_STATS* stats = hint->stats;
                if (stats != NULL) {
                    /* avoid under flow */
                    if (stats->curMem > pt->sz) {
                        stats->curMem -= pt->sz;
                    }
                    else {
                        stats->curMem = 0;
                    }

                    if (stats->curAlloc > 0) {
                        stats->curAlloc--;
                    }
                    stats->totalFr++;
                }
            }
            wc_UnLockMutex(&(mem->memory_mutex));
        }
    }

    (void)i;
    (void)pt;
    (void)type;
}

#ifdef WOLFSSL_DEBUG_MEMORY
void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type, const char* func, unsigned int line)
#else
void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type)
#endif
{
    void* res = 0;
    wc_Memory* pt = NULL;
    int    i;

    /* check for testing heap hint was set */
#ifdef WOLFSSL_HEAP_TEST
    if (heap == (void*)WOLFSSL_HEAP_TEST) {
        return realloc(ptr, size);
    }
#endif

    if (heap == NULL) {
        #ifdef WOLFSSL_HEAP_TEST
            WOLFSSL_MSG("ERROR null heap hint passed in to XREALLOC");
        #endif
        #ifndef WOLFSSL_NO_MALLOC
            res = realloc(ptr, size);
        #else
            WOLFSSL_MSG("NO heap found to use for realloc");
        #endif /* WOLFSSL_NO_MALLOC */
    }
    else {
        WOLFSSL_HEAP_HINT* hint = (WOLFSSL_HEAP_HINT*)heap;
        WOLFSSL_HEAP*      mem  = hint->memory;
        word32 padSz = -(int)sizeof(wc_Memory) & (WOLFSSL_STATIC_ALIGN - 1);

        if (ptr == NULL) {
        #ifdef WOLFSSL_DEBUG_MEMORY
            return wolfSSL_Malloc(size, heap, type, func, line);
        #else
            return wolfSSL_Malloc(size, heap, type);
        #endif
        }

        if (wc_LockMutex(&(mem->memory_mutex)) != 0) {
            WOLFSSL_MSG("Bad memory_mutex lock");
            return NULL;
        }

        /* case of using fixed IO buffers or IO pool */
        if (((mem->flag & WOLFMEM_IO_POOL)||(mem->flag & WOLFMEM_IO_POOL_FIXED))
                                          && (type == DYNAMIC_TYPE_OUT_BUFFER ||
                                              type == DYNAMIC_TYPE_IN_BUFFER)) {
            /* no realloc, is fixed size */
            pt = (wc_Memory*)((byte*)ptr - padSz - sizeof(wc_Memory));
            if (pt->sz < size) {
                WOLFSSL_MSG("Error IO memory was not large enough");
                res = NULL; /* return NULL in error case */
            }
            res = pt->buffer;
        }
        else {
        /* general memory */
            for (i = 0; i < WOLFMEM_MAX_BUCKETS; i++) {
                if ((word32)size <= mem->sizeList[i]) {
                    if (mem->ava[i] != NULL) {
                        pt = mem->ava[i];
                        mem->ava[i] = pt->next;
                        break;
                    }
                }
            }

            if (pt != NULL && res == NULL) {
                res = pt->buffer;

                /* copy over original information and free ptr */
                word32 prvSz = ((wc_Memory*)((byte*)ptr - padSz -
                                               sizeof(wc_Memory)))->sz;
                prvSz = (prvSz > pt->sz)? pt->sz: prvSz;
                XMEMCPY(pt->buffer, ptr, prvSz);
                mem->inUse += pt->sz;
                mem->alloc += 1;

                /* free memory that was previously being used */
                wc_UnLockMutex(&(mem->memory_mutex));
                wolfSSL_Free(ptr, heap, type
            #ifdef WOLFSSL_DEBUG_MEMORY
                    , func, line
            #endif
                );
                if (wc_LockMutex(&(mem->memory_mutex)) != 0) {
                    WOLFSSL_MSG("Bad memory_mutex lock");
                    return NULL;
                }
            }
        }
        wc_UnLockMutex(&(mem->memory_mutex));
    }

    #ifdef WOLFSSL_MALLOC_CHECK
        if ((wc_ptr_t)res % WOLFSSL_STATIC_ALIGN) {
            WOLFSSL_MSG("ERROR memory is not aligned");
            res = NULL;
        }
    #endif

    (void)i;
    (void)pt;
    (void)type;

    return res;
}
#endif /* WOLFSSL_STATIC_MEMORY */

#endif /* USE_WOLFSSL_MEMORY */


#ifdef HAVE_IO_POOL

/* Example for user io pool, shared build may need definitions in lib proper */

#include <wolfssl/wolfcrypt/types.h>
#include <stdlib.h>

#ifndef HAVE_THREAD_LS
    #error "Oops, simple I/O pool example needs thread local storage"
#endif


/* allow simple per thread in and out pools */
/* use 17k size since max record size is 16k plus overhead */
static THREAD_LS_T byte pool_in[17*1024];
static THREAD_LS_T byte pool_out[17*1024];


void* XMALLOC(size_t n, void* heap, int type)
{
    (void)heap;

    if (type == DYNAMIC_TYPE_IN_BUFFER) {
        if (n < sizeof(pool_in))
            return pool_in;
        else
            return NULL;
    }

    if (type == DYNAMIC_TYPE_OUT_BUFFER) {
        if (n < sizeof(pool_out))
            return pool_out;
        else
            return NULL;
    }

    return malloc(n);
}

void* XREALLOC(void *p, size_t n, void* heap, int type)
{
    (void)heap;

    if (type == DYNAMIC_TYPE_IN_BUFFER) {
        if (n < sizeof(pool_in))
            return pool_in;
        else
            return NULL;
    }

    if (type == DYNAMIC_TYPE_OUT_BUFFER) {
        if (n < sizeof(pool_out))
            return pool_out;
        else
            return NULL;
    }

    return realloc(p, n);
}

void XFREE(void *p, void* heap, int type)
{
    (void)heap;

    if (type == DYNAMIC_TYPE_IN_BUFFER)
        return;  /* do nothing, static pool */

    if (type == DYNAMIC_TYPE_OUT_BUFFER)
        return;  /* do nothing, static pool */

    free(p);
}

#endif /* HAVE_IO_POOL */

#ifdef WOLFSSL_MEMORY_LOG
void *xmalloc(size_t n, void* heap, int type, const char* func,
              const char* file, unsigned int line)
{
    void*   p = NULL;
    word32* p32;

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (!wc_MemFailCount_AllocMem()) {
        WOLFSSL_MSG("MemFailCnt: Fail malloc");
        return NULL;
    }
#endif

    if (malloc_function) {
#ifndef WOLFSSL_STATIC_MEMORY
        p32 = malloc_function(n + sizeof(word32) * 4);
#else
        p32 = malloc_function(n + sizeof(word32) * 4, heap, type);
#endif
    }
    else
        p32 = malloc(n + sizeof(word32) * 4);

    if (p32 != NULL) {
        p32[0] = (word32)n;
        p = (void*)(p32 + 4);

        fprintf(stderr, "Alloc: %p -> %u (%d) at %s:%s:%u\n", p, (word32)n,
                                                        type, func, file, line);
    }

    (void)heap;

    return p;
}
void *xrealloc(void *p, size_t n, void* heap, int type, const char* func,
               const char* file, unsigned int line)
{
    void*   newp = NULL;
    word32* p32;
    word32* oldp32 = NULL;
    word32  oldLen;

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (!wc_MemFailCount_AllocMem()) {
        WOLFSSL_MSG("MemFailCnt: Fail malloc");
        return NULL;
    }
#endif

    if (p != NULL) {
        oldp32 = (word32*)p;
        oldp32 -= 4;
        oldLen = oldp32[0];
    }

    if (realloc_function) {
#ifndef WOLFSSL_STATIC_MEMORY
        p32 = realloc_function(oldp32, n + sizeof(word32) * 4);
#else
        p32 = realloc_function(oldp32, n + sizeof(word32) * 4, heap, type);
#endif
    }
    else
        p32 = realloc(oldp32, n + sizeof(word32) * 4);

    if (p32 != NULL) {
        p32[0] = (word32)n;
        newp = (void*)(p32 + 4);

        if (p != NULL) {
            fprintf(stderr, "Free: %p -> %u (%d) at %s:%s:%u\n", p, oldLen,
                                                        type, func, file, line);
        }
        fprintf(stderr, "Alloc: %p -> %u (%d) at %s:%s:%u\n", newp, (word32)n,
                                                        type, func, file, line);
    }

#ifdef WOLFSSL_MEM_FAIL_COUNT
    if (p != NULL) {
        wc_MemFailCount_FreeMem();
    }
#endif

    (void)heap;

    return newp;
}
void xfree(void *p, void* heap, int type, const char* func, const char* file,
           unsigned int line)
{
    word32* p32 = (word32*)p;

    if (p != NULL) {
    #ifdef WOLFSSL_MEM_FAIL_COUNT
        wc_MemFailCount_FreeMem();
    #endif
        p32 -= 4;

        fprintf(stderr, "Free: %p -> %u (%d) at %s:%s:%u\n", p, p32[0], type,
                                                              func, file, line);

        if (free_function) {
#ifndef WOLFSSL_STATIC_MEMORY
            free_function(p32);
#else
            free_function(p32, heap, type);
#endif
        }
        else
            free(p32);
    }

    (void)heap;
}
#endif /* WOLFSSL_MEMORY_LOG */

#ifdef WOLFSSL_STACK_LOG
/* Note: this code only works with GCC using -finstrument-functions. */
void __attribute__((no_instrument_function))
     __cyg_profile_func_enter(void *func,  void *caller)
{
    register void* sp asm("sp");
    fprintf(stderr, "ENTER: %016lx %p\n", (unsigned long)(wc_ptr_t)func, sp);
    (void)caller;
}

void __attribute__((no_instrument_function))
     __cyg_profile_func_exit(void *func, void *caller)
{
    register void* sp asm("sp");
    fprintf(stderr, "EXIT: %016lx %p\n", (unsigned long)(wc_ptr_t)func, sp);
    (void)caller;
}
#endif

#ifdef WOLFSSL_LINUXKM
    #include "../../linuxkm/linuxkm_memory.c"
#endif
