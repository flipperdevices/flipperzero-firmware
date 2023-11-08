/* mem_track.h
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

/* Memory and stack use tracking */

#ifndef WOLFSSL_MEM_TRACK_H
#define WOLFSSL_MEM_TRACK_H

/* The memory tracker overrides the wolfSSL memory callback system and uses a
 * static to track the total, peak and currently allocated bytes.
 *
 * If you are already using the memory callbacks then enabling this will
 * override the memory callbacks and prevent your memory callbacks from
 * working. This assumes malloc() and free() are available. Feel free to
 * customize this for your needs.

 * The enable this feature define the following:
 * #define USE_WOLFSSL_MEMORY
 * #define WOLFSSL_TRACK_MEMORY
 *
 * On startup call:
 * InitMemoryTracker();
 *
 * When ready to dump the memory report call:
 * ShowMemoryTracker();
 *
 * Report example:
 * total   Allocs =       228
 * total   Bytes  =     93442
 * peak    Bytes  =      8840
 * current Bytes  =         0
 *
 *
 * You can also:
 * #define WOLFSSL_DEBUG_MEMORY
 *
 * To print every alloc/free along with the function and line number.
 * Example output:
 * Alloc: 0x7fa14a500010 -> 120 at wc_InitRng:496
 * Free: 0x7fa14a500010 -> 120 at wc_FreeRng:606
 */

#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/wolfcrypt/types.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/memory.h"

#if defined(WOLFSSL_TRACK_MEMORY) || defined(HAVE_STACK_SIZE) || \
    defined(HAVE_STACK_SIZE_VERBOSE)
    #ifdef NO_STDIO_FILESYSTEM
        /* if wc_port.h/linuxkm_wc_port.h doesn't define printf, then the user
         * needs to define it.
         */
        #define wc_mem_printf(...) printf(__VA_ARGS__)
    #else
        #include <stdio.h>
        #define wc_mem_printf(...) fprintf(stderr, __VA_ARGS__)
    #endif
#endif

/* Track Memory */
#if defined(WOLFSSL_TRACK_MEMORY) && defined(USE_WOLFSSL_MEMORY) && \
    !defined(WOLFSSL_STATIC_MEMORY)

#define DO_MEM_STATS
#if (defined(__linux__) && !defined(WOLFSSL_LINUXKM)) || defined(__MACH__)
    #define DO_MEM_LIST
#endif

typedef struct memoryStats {
    long totalAllocs;     /* number of allocations */
    long totalDeallocs;   /* number of deallocations */
    long totalBytes;      /* total number of bytes allocated */
    long peakBytes;       /* concurrent max bytes */
    long currentBytes;    /* total current bytes in use */
#ifdef WOLFSSL_TRACK_MEMORY_VERBOSE
    long peakAllocsTripOdometer; /* peak number of concurrent allocations,
                                  * subject to reset by
                                  * wolfCrypt_heap_peak_checkpoint()
                                  */
    long peakBytesTripOdometer; /* peak concurrent bytes, subject to reset
                                 * by wolfCrypt_heap_peak_checkpoint()
                                 */
#endif
} memoryStats;

typedef struct memHint {
    size_t thisSize;      /* size of this memory */

#ifdef DO_MEM_LIST
    struct memHint* next;
    struct memHint* prev;
    #ifdef WOLFSSL_DEBUG_MEMORY
        const char* func;
        unsigned int line;
    #endif
#endif
    void*  thisMemory;    /* actual memory for user */
} memHint;

typedef struct memoryTrack {
    union {
        memHint hint;
        /* make sure we have strong alignment */
        byte    alignit[sizeof(memHint) + ((16-1) & ~(16-1))];
    } u;
} memoryTrack;

#ifdef DO_MEM_LIST
/* track allocations and report at end */
typedef struct memoryList {
    memHint* head;
    memHint* tail;
    word32   count;
} memoryList;
#endif


static memoryStats ourMemStats;

#ifdef DO_MEM_LIST
    #include <pthread.h>
    static memoryList ourMemList;
    static pthread_mutex_t memLock = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef WOLFSSL_DEBUG_MEMORY
static WC_INLINE void* TrackMalloc(size_t sz, const char* func,
                                   unsigned int line)
#else
static WC_INLINE void* TrackMalloc(size_t sz)
#endif
{
    memoryTrack* mt;
    memHint* header;

    if (sz == 0)
        return NULL;

#ifdef FREERTOS
    mt = (memoryTrack*)pvPortMalloc(sizeof(memoryTrack) + sz);
#else
    mt = (memoryTrack*)malloc(sizeof(memoryTrack) + sz);
#endif
    if (mt == NULL)
        return NULL;

    header = &mt->u.hint;
    header->thisSize   = sz;
    header->thisMemory = (byte*)mt + sizeof(memoryTrack);

#ifdef WOLFSSL_DEBUG_MEMORY
#ifdef WOLFSSL_DEBUG_MEMORY_PRINT
    wc_mem_printf("Alloc: %p -> %u at %s:%d\n",
        header->thisMemory, (word32)sz, func, line);
#else
    (void)func;
    (void)line;
#endif
#endif

#ifdef DO_MEM_STATS
    ourMemStats.totalAllocs++;
    ourMemStats.totalBytes   += sz;
    ourMemStats.currentBytes += sz;
#ifdef WOLFSSL_TRACK_MEMORY_VERBOSE
    if (ourMemStats.peakAllocsTripOdometer < ourMemStats.totalAllocs -
            ourMemStats.totalDeallocs) {
        ourMemStats.peakAllocsTripOdometer = ourMemStats.totalAllocs -
            ourMemStats.totalDeallocs;
    }
    if (ourMemStats.peakBytesTripOdometer < ourMemStats.currentBytes)
#endif
    {
    #ifdef WOLFSSL_TRACK_MEMORY_VERBOSE
        ourMemStats.peakBytesTripOdometer = ourMemStats.currentBytes;
    #endif
        if (ourMemStats.currentBytes > ourMemStats.peakBytes)
            ourMemStats.peakBytes = ourMemStats.currentBytes;
    }
#endif /* DO_MEM_STATS */

#ifdef DO_MEM_LIST
    if (pthread_mutex_lock(&memLock) == 0) {
    #ifdef WOLFSSL_DEBUG_MEMORY
        header->func = func;
        header->line = line;
    #endif

        /* Setup event */
        header->next = NULL;
        if (ourMemList.tail == NULL)  {
            ourMemList.head = header;
            header->prev = NULL;
        }
        else {
            ourMemList.tail->next = header;
            header->prev = ourMemList.tail;
        }
        ourMemList.tail = header;      /* add to the end either way */
        ourMemList.count++;

        pthread_mutex_unlock(&memLock);
    }
#endif /* DO_MEM_LIST */

    return header->thisMemory;
}


#ifdef WOLFSSL_DEBUG_MEMORY
static WC_INLINE void TrackFree(void* ptr, const char* func, unsigned int line)
#else
static WC_INLINE void TrackFree(void* ptr)
#endif
{
    memoryTrack* mt;
    memHint* header;
    size_t sz;

    if (ptr == NULL) {
        return;
    }

    mt = (memoryTrack*)((byte*)ptr - sizeof(memoryTrack));
    header = &mt->u.hint;
    sz = header->thisSize;

#ifdef DO_MEM_LIST
    if (pthread_mutex_lock(&memLock) == 0)
    {
#endif

#ifdef DO_MEM_STATS
        ourMemStats.currentBytes -= header->thisSize;
        ourMemStats.totalDeallocs++;
#endif

#ifdef DO_MEM_LIST
        if (header == ourMemList.head && header == ourMemList.tail) {
            ourMemList.head = NULL;
            ourMemList.tail = NULL;
        }
        else if (header == ourMemList.head) {
            ourMemList.head = header->next;
            ourMemList.head->prev = NULL;
        }
        else if (header == ourMemList.tail) {
            ourMemList.tail = header->prev;
            ourMemList.tail->next = NULL;
        }
        else {
            memHint* next = header->next;
            memHint* prev = header->prev;
            if (next)
                next->prev = prev;
            if (prev)
                prev->next = next;
        }
        ourMemList.count--;

        pthread_mutex_unlock(&memLock);
    }
#endif

#ifdef WOLFSSL_DEBUG_MEMORY
#ifdef WOLFSSL_DEBUG_MEMORY_PRINT
    wc_mem_printf("Free: %p -> %u at %s:%d\n", ptr, (word32)sz, func, line);
#else
    (void)func;
    (void)line;
#endif
#endif
    (void)sz;

#ifdef FREERTOS
    vPortFree(mt);
#else
    free(mt);
#endif
}


#ifdef WOLFSSL_DEBUG_MEMORY
static WC_INLINE void* TrackRealloc(void* ptr, size_t sz, const char* func,
                                    unsigned int line)
#else
static WC_INLINE void* TrackRealloc(void* ptr, size_t sz)
#endif
{
#ifdef WOLFSSL_DEBUG_MEMORY
    void* ret = TrackMalloc(sz, func, line);
#else
    void* ret = TrackMalloc(sz);
#endif

    if (ptr) {
        /* if realloc is bigger, don't overread old ptr */
        memoryTrack* mt;
        memHint* header;

        mt = (memoryTrack*)((byte*)ptr - sizeof(memoryTrack));
        header = &mt->u.hint;

        if (header->thisSize < sz)
            sz = header->thisSize;
    }

    if (ret && ptr)
        XMEMCPY(ret, ptr, sz);

    if (ret) {
    #ifdef WOLFSSL_DEBUG_MEMORY
        TrackFree(ptr, func, line);
    #else
        TrackFree(ptr);
    #endif
    }

    return ret;
}

static wolfSSL_Malloc_cb mfDefault = NULL;
static wolfSSL_Free_cb ffDefault = NULL;
static wolfSSL_Realloc_cb rfDefault = NULL;

static WC_INLINE int InitMemoryTracker(void)
{
    int ret;

    ret = wolfSSL_GetAllocators(&mfDefault, &ffDefault, &rfDefault);
    if (ret < 0) {
        wc_mem_printf("wolfSSL GetAllocators failed to get the defaults\n");
    }
    ret = wolfSSL_SetAllocators(TrackMalloc, TrackFree, TrackRealloc);
    if (ret < 0) {
        wc_mem_printf("wolfSSL SetAllocators failed for track memory\n");
        return ret;
    }

#ifdef DO_MEM_LIST
    if (pthread_mutex_lock(&memLock) == 0)
#endif
    {
    #ifdef DO_MEM_STATS
        ourMemStats.totalAllocs  = 0;
        ourMemStats.totalDeallocs = 0;
        ourMemStats.totalBytes   = 0;
        ourMemStats.peakBytes    = 0;
        ourMemStats.currentBytes = 0;
    #ifdef WOLFSSL_TRACK_MEMORY_VERBOSE
        ourMemStats.peakAllocsTripOdometer = 0;
        ourMemStats.peakBytesTripOdometer    = 0;
    #endif
    #endif /* DO_MEM_STATS */

    #ifdef DO_MEM_LIST
        XMEMSET(&ourMemList, 0, sizeof(ourMemList));

        pthread_mutex_unlock(&memLock);
    #endif
    }

    return ret;
}

static WC_INLINE void ShowMemoryTracker(void)
{
#ifdef DO_MEM_LIST
    if (pthread_mutex_lock(&memLock) == 0)
#endif
    {
    #ifdef DO_MEM_STATS
        wc_mem_printf("total   Allocs   = %9ld\n", ourMemStats.totalAllocs);
        wc_mem_printf("total   Deallocs = %9ld\n", ourMemStats.totalDeallocs);
        wc_mem_printf("total   Bytes    = %9ld\n", ourMemStats.totalBytes);
        wc_mem_printf("peak    Bytes    = %9ld\n", ourMemStats.peakBytes);
        wc_mem_printf("current Bytes    = %9ld\n", ourMemStats.currentBytes);
    #endif

    #ifdef DO_MEM_LIST
        if (ourMemList.count > 0) {
            /* print list of allocations */
            memHint* header;
            for (header = ourMemList.head;
                 header != NULL;
                 header = header->next) {
        #ifdef WOLFSSL_DEBUG_MEMORY
                wc_mem_printf("Leak: Ptr %p, Size %u, Func %s, Line %d\n",
                    (byte*)header + sizeof(memHint),
                    (unsigned int)header->thisSize, header->func, header->line);
        #else
                wc_mem_printf("Leak: Ptr %p, Size %u\n",
                    (byte*)header + sizeof(memHint),
                    (unsigned int)header->thisSize);
        #endif
            }
        }

        pthread_mutex_unlock(&memLock);
    #endif
    }
}

static WC_INLINE int CleanupMemoryTracker(void)
{
    /* restore default allocators */
    return wolfSSL_SetAllocators(mfDefault, ffDefault, rfDefault);
}
#endif /* WOLFSSL_TRACK_MEMORY && USE_WOLFSSL_MEMORY && \
          !WOLFSSL_STATIC_MEMORY */


#ifdef HAVE_STACK_SIZE

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

typedef void* (*thread_func)(void* args);
#define STACK_CHECK_VAL 0x01

struct stack_size_debug_context {
  unsigned char *myStack;
  size_t stackSize;
#ifdef HAVE_STACK_SIZE_VERBOSE
  size_t *stackSizeHWM_ptr;
  thread_func fn;
  void *args;
#endif
};

struct func_args; /* forward declaration */

#ifdef HAVE_STACK_SIZE_VERBOSE

/* per-subtest stack high water mark tracking.
 *
 * enable with
 *
 * ./configure --enable-stacksize=verbose [...]
 */

static void* debug_stack_size_verbose_shim(
    struct stack_size_debug_context *shim_args)
{
    StackSizeCheck_myStack = shim_args->myStack;
    StackSizeCheck_stackSize = shim_args->stackSize;
    StackSizeCheck_stackSizeHWM_ptr = shim_args->stackSizeHWM_ptr;
    return shim_args->fn(shim_args->args);
}

static WC_INLINE int StackSizeSetOffset(const char *funcname, void *p)
{
    if (StackSizeCheck_myStack == NULL)
        return -BAD_FUNC_ARG;

    StackSizeCheck_stackOffsetPointer = p;

    printf("setting stack relative offset reference mark in %s to +%lu\n",
        funcname, (unsigned long)((char*)(StackSizeCheck_myStack +
                                  StackSizeCheck_stackSize) - (char *)p));

    return 0;
}

static WC_INLINE ssize_t StackSizeHWM(void)
{
    size_t i;
    ssize_t used;

    if (StackSizeCheck_myStack == NULL)
        return -BAD_FUNC_ARG;

    for (i = 0; i < StackSizeCheck_stackSize; i++) {
        if (StackSizeCheck_myStack[i] != STACK_CHECK_VAL) {
            break;
        }
    }

    used = StackSizeCheck_stackSize - i;
    if ((ssize_t)*StackSizeCheck_stackSizeHWM_ptr < used)
      *StackSizeCheck_stackSizeHWM_ptr = used;

    return used;
}

static WC_INLINE ssize_t StackSizeHWM_OffsetCorrected(void)
{
    ssize_t used = StackSizeHWM();
    if (used < 0)
        return used;
    if (StackSizeCheck_stackOffsetPointer) {
        used -= (ssize_t)(((char *)StackSizeCheck_myStack +
            StackSizeCheck_stackSize) -
            (char *)StackSizeCheck_stackOffsetPointer);
    }
    return used;
}

static
#ifdef __GNUC__
__attribute__((unused)) __attribute__((noinline))
#endif
int StackSizeHWMReset(void)
{
    volatile ssize_t i;

    if (StackSizeCheck_myStack == NULL)
        return -BAD_FUNC_ARG;

    for (i = (ssize_t)((char *)&i - (char *)StackSizeCheck_myStack) -
                                          (ssize_t)sizeof(i) - 1; i >= 0; --i) {
        StackSizeCheck_myStack[i] = STACK_CHECK_VAL;
    }

    return 0;
}

#define STACK_SIZE_CHECKPOINT(...) ({  \
    ssize_t HWM = StackSizeHWM_OffsetCorrected();    \
    __VA_ARGS__;                                     \
    printf("    relative stack peak usage = %ld bytes\n", (long int)HWM);  \
    StackSizeHWMReset();                             \
    })

#define STACK_SIZE_CHECKPOINT_MSG(msg) ({             \
    ssize_t HWM = StackSizeHWM_OffsetCorrected();     \
    wc_mem_printf("%ld\t%s\n", (long int)HWM, msg);   \
    StackSizeHWMReset();                              \
    })

#define STACK_SIZE_CHECKPOINT_WITH_MAX_CHECK(max, ...) ({  \
    ssize_t HWM = StackSizeHWM_OffsetCorrected();    \
    int _ret;                                        \
    __VA_ARGS__;                                     \
    printf("    relative stack peak usage = %ld bytes\n", (long int)HWM);  \
    _ret = StackSizeHWMReset();                      \
    if ((max >= 0) && (HWM > (ssize_t)(max))) {      \
        wc_mem_printf(                               \
            "    relative stack usage at %s L%d exceeds designated " \
                "max %ld bytes.\n", \
            __FILE__, __LINE__, (long int)(max));    \
        _ret = -1;                                   \
    }                                                \
    _ret;                                            \
    })


#if defined(__GNUC__) || defined(__clang__)
#define STACK_SIZE_INIT() \
    (void)StackSizeSetOffset(__FUNCTION__, __builtin_frame_address(0))
#endif

#endif /* HAVE_STACK_SIZE_VERBOSE */

static WC_INLINE int StackSizeCheck(struct func_args* args, thread_func tf)
{
    size_t         i;
    int            ret;
    void*          status;
    unsigned char* myStack = NULL;
    size_t         stackSize = 1024*1024*2;
    pthread_attr_t myAttr;
    pthread_t      threadId;
#ifdef HAVE_STACK_SIZE_VERBOSE
    struct stack_size_debug_context shim_args;
#endif

#ifdef PTHREAD_STACK_MIN
    if (stackSize < PTHREAD_STACK_MIN)
        stackSize = PTHREAD_STACK_MIN;
#endif

    ret = posix_memalign((void**)&myStack, sysconf(_SC_PAGESIZE), stackSize);
    if (ret != 0 || myStack == NULL) {
        wc_mem_printf("posix_memalign failed\n");
        return -1;
    }

    XMEMSET(myStack, STACK_CHECK_VAL, stackSize);

    ret = pthread_attr_init(&myAttr);
    if (ret != 0) {
        wc_mem_printf("attr_init failed\n");
        return ret;
    }

    ret = pthread_attr_setstack(&myAttr, myStack, stackSize);
    if (ret != 0) {
        wc_mem_printf("attr_setstackaddr failed\n");
        return ret;
    }

#ifdef HAVE_STACK_SIZE_VERBOSE
    StackSizeCheck_stackSizeHWM = 0;
    shim_args.myStack = myStack;
    shim_args.stackSize = stackSize;
    shim_args.stackSizeHWM_ptr = &StackSizeCheck_stackSizeHWM;
    shim_args.fn = tf;
    shim_args.args = args;
    ret = pthread_create(&threadId, &myAttr,
        (thread_func)debug_stack_size_verbose_shim, (void *)&shim_args);
#else
    ret = pthread_create(&threadId, &myAttr, tf, args);
#endif
    if (ret != 0) {
        printf("ret = %d\n", ret);
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(threadId, &status);
    if (ret != 0) {
        wc_mem_printf("pthread_join failed\n");
        return ret;
    }

    for (i = 0; i < stackSize; i++) {
        if (myStack[i] != STACK_CHECK_VAL) {
            break;
        }
    }

    free(myStack);
#ifdef HAVE_STACK_SIZE_VERBOSE
    printf("stack used = %lu\n", StackSizeCheck_stackSizeHWM > (stackSize - i)
        ? (unsigned long)StackSizeCheck_stackSizeHWM
        : (unsigned long)(stackSize - i));
    StackSizeCheck_myStack = NULL;
    StackSizeCheck_stackOffsetPointer = NULL;
#else
    {
      size_t used = stackSize - i;
      printf("stack used = %lu\n", (unsigned long)used);
    }
#endif

    return (int)((size_t)status);
}

static WC_INLINE int StackSizeCheck_launch(struct func_args* args,
    thread_func tf, pthread_t *threadId, void **stack_context)
{
    int ret;
    unsigned char* myStack = NULL;
    size_t stackSize = 1024*1024*2;
    pthread_attr_t myAttr;
    struct stack_size_debug_context* shim_args;

#ifdef PTHREAD_STACK_MIN
    if (stackSize < PTHREAD_STACK_MIN)
        stackSize = PTHREAD_STACK_MIN;
#endif

    shim_args = (struct stack_size_debug_context *)malloc(sizeof *shim_args);
    if (shim_args == NULL) {
        perror("malloc");
        return -1;
    }

    ret = posix_memalign((void**)&myStack, sysconf(_SC_PAGESIZE), stackSize);
    if (ret != 0 || myStack == NULL) {
        wc_mem_printf("posix_memalign failed\n");
        free(shim_args);
        return -1;
    }

    XMEMSET(myStack, STACK_CHECK_VAL, stackSize);

    ret = pthread_attr_init(&myAttr);
    if (ret != 0) {
        wc_mem_printf("attr_init failed\n");
        free(shim_args);
        free(myStack);
        return ret;
    }

    ret = pthread_attr_setstack(&myAttr, myStack, stackSize);
    if (ret != 0) {
        wc_mem_printf("attr_setstackaddr failed\n");
    }

    shim_args->myStack = myStack;
    shim_args->stackSize = stackSize;
#ifdef HAVE_STACK_SIZE_VERBOSE
    shim_args->stackSizeHWM_ptr = &StackSizeCheck_stackSizeHWM;
    shim_args->fn = tf;
    shim_args->args = args;
    ret = pthread_create(threadId, &myAttr,
        (thread_func)debug_stack_size_verbose_shim, (void *)shim_args);
#else
    ret = pthread_create(threadId, &myAttr, tf, args);
#endif
    if (ret != 0) {
        fprintf(stderr,"pthread_create failed: %s",strerror(ret));
        exit(EXIT_FAILURE);
    }

    *stack_context = (void *)shim_args;

    return 0;
}

static WC_INLINE int StackSizeCheck_reap(pthread_t threadId,
    void *stack_context)
{
    struct stack_size_debug_context *shim_args =
        (struct stack_size_debug_context *)stack_context;
    size_t i;
    void *status;
    int ret = pthread_join(threadId, &status);
    if (ret != 0) {
        wc_mem_printf("pthread_join failed\n");
        return ret;
    }

    for (i = 0; i < shim_args->stackSize; i++) {
        if (shim_args->myStack[i] != STACK_CHECK_VAL) {
            break;
        }
    }

    free(shim_args->myStack);
#ifdef HAVE_STACK_SIZE_VERBOSE
    printf("stack used = %lu\n",
        *shim_args->stackSizeHWM_ptr > (shim_args->stackSize - i)
        ? (unsigned long)*shim_args->stackSizeHWM_ptr
        : (unsigned long)(shim_args->stackSize - i));
#else
    {
      size_t used = shim_args->stackSize - i;
      printf("stack used = %lu\n", (unsigned long)used);
    }
#endif
    free(shim_args);

    return (int)((size_t)status);
}

#endif /* HAVE_STACK_SIZE */


#ifdef STACK_TRAP

/* good settings
   ./configure --enable-debug --disable-shared C_EXTRA_FLAGS="-DUSER_TIME \
        -DTFM_TIMING_RESISTANT -DPOSITIVE_EXP_ONLY -DSTACK_TRAP"

*/

#ifdef HAVE_STACK_SIZE
    /* client only for now, setrlimit will fail if pthread_create() called */
    /* STACK_SIZE does pthread_create() on client */
    #error "can't use STACK_TRAP with STACK_SIZE, setrlimit will fail"
#endif /* HAVE_STACK_SIZE */

static WC_INLINE void StackTrap(void)
{
    struct rlimit  rl;
    if (getrlimit(RLIMIT_STACK, &rl) != 0) {
        wc_mem_printf("getrlimit failed\n");
    }
    printf("rlim_cur = %llu\n", rl.rlim_cur);
    rl.rlim_cur = 1024*21;  /* adjust trap size here */
    if (setrlimit(RLIMIT_STACK, &rl) != 0) {
        wc_mem_printf("setrlimit failed\n");
    }
}

#else /* STACK_TRAP */

static WC_INLINE void StackTrap(void)
{
}

#endif /* STACK_TRAP */

/* Stubs when not used */
#ifndef STACK_SIZE_CHECKPOINT
#define STACK_SIZE_CHECKPOINT(...) (__VA_ARGS__)
#endif
#ifndef STACK_SIZE_CHECKPOINT_MSG
#define STACK_SIZE_CHECKPOINT_MSG(msg) WC_DO_NOTHING
#endif
#ifndef STACK_SIZE_CHECKPOINT_WITH_MAX_CHECK
#define STACK_SIZE_CHECKPOINT_WITH_MAX_CHECK(max, ...) (__VA_ARGS__, 0)
#endif
#ifndef STACK_SIZE_INIT
#define STACK_SIZE_INIT() WC_DO_NOTHING
#endif

#endif /* WOLFSSL_MEM_TRACK_H */
