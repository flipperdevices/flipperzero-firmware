/* memory.h
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


/* submitted by eof */

/*!
    \file wolfssl/wolfcrypt/memory.h
*/

#ifndef WOLFSSL_MEMORY_H
#define WOLFSSL_MEMORY_H

#if !defined(STRING_USER) && !defined(WOLFSSL_LINUXKM)
#include <stdlib.h>
#endif

#ifndef WOLF_CRYPT_TYPES_H
#include <wolfssl/wolfcrypt/types.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
    WOLFSSL_API void wolfSSL_SetMemFailCount(int memFailCount);
#endif

#ifdef OPENSSL_EXTRA
    typedef void *(*wolfSSL_OSSL_Malloc_cb)(size_t, const char *, int);
    typedef void  (*wolfSSL_OSSL_Free_cb)(void *, const char *, int);
    typedef void *(*wolfSSL_OSSL_Realloc_cb)(void *, size_t, const char *, int);
#endif /* OPENSSL_EXTRA */

#ifdef WOLFSSL_STATIC_MEMORY
    #ifdef WOLFSSL_DEBUG_MEMORY
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, void* heap, int type, const char* func, unsigned int line);
        typedef void (*wolfSSL_Free_cb)(void *ptr, void* heap, int type, const char* func, unsigned int line);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, void* heap, int type, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type, const char* func, unsigned int line);
    #else
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, void* heap, int type);
        typedef void (*wolfSSL_Free_cb)(void *ptr, void* heap, int type);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, void* heap, int type);
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, void* heap, int type);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, void* heap, int type);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, void* heap, int type);
    #endif /* WOLFSSL_DEBUG_MEMORY */
#else
    #ifdef WOLFSSL_DEBUG_MEMORY
        typedef void *(*wolfSSL_Malloc_cb)(size_t size, const char* func, unsigned int line);
        typedef void (*wolfSSL_Free_cb)(void *ptr, const char* func, unsigned int line);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size, const char* func, unsigned int line);

        /* Public in case user app wants to use XMALLOC/XFREE */
        WOLFSSL_API void* wolfSSL_Malloc(size_t size, const char* func, unsigned int line);
        WOLFSSL_API void  wolfSSL_Free(void *ptr, const char* func, unsigned int line);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size, const char* func, unsigned int line);
    #else
        typedef void *(*wolfSSL_Malloc_cb)(size_t size);
        typedef void (*wolfSSL_Free_cb)(void *ptr);
        typedef void *(*wolfSSL_Realloc_cb)(void *ptr, size_t size);
        /* Public in case user app wants to use XMALLOC/XFREE */
        WOLFSSL_API void* wolfSSL_Malloc(size_t size);
        WOLFSSL_API void  wolfSSL_Free(void *ptr);
        WOLFSSL_API void* wolfSSL_Realloc(void *ptr, size_t size);
    #endif /* WOLFSSL_DEBUG_MEMORY */
#endif /* WOLFSSL_STATIC_MEMORY */

/* Public get/set functions */
WOLFSSL_API int wolfSSL_SetAllocators(wolfSSL_Malloc_cb mf,
                                      wolfSSL_Free_cb ff,
                                      wolfSSL_Realloc_cb rf);
WOLFSSL_API int wolfSSL_GetAllocators(wolfSSL_Malloc_cb* mf,
                                      wolfSSL_Free_cb* ff,
                                      wolfSSL_Realloc_cb* rf);

#ifdef WOLFSSL_STATIC_MEMORY
    #define WOLFSSL_STATIC_TIMEOUT 1
    #ifndef WOLFSSL_STATIC_ALIGN
        #define WOLFSSL_STATIC_ALIGN 16
    #endif
    #ifndef WOLFMEM_MAX_BUCKETS
        #define WOLFMEM_MAX_BUCKETS  9
    #endif
    #define WOLFMEM_DEF_BUCKETS  9     /* number of default memory blocks */
    #ifndef WOLFMEM_IO_SZ
        #define WOLFMEM_IO_SZ        16992 /* 16 byte aligned */
    #endif
    #ifndef WOLFMEM_BUCKETS
        #ifndef SESSION_CERTS
            /* default size of chunks of memory to separate into */
            #ifndef LARGEST_MEM_BUCKET
                #define LARGEST_MEM_BUCKET 16128
            #endif
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3456,4544,\
                                    LARGEST_MEM_BUCKET
        #elif defined (OPENSSL_EXTRA)
            /* extra storage in structs for multiple attributes and order */
            #ifndef LARGEST_MEM_BUCKET
                #ifdef WOLFSSL_TLS13
                    #define LARGEST_MEM_BUCKET 30400
                #else
                    #define LARGEST_MEM_BUCKET 25600
                #endif
            #endif
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3360,4480,\
                                    LARGEST_MEM_BUCKET
        #elif defined (WOLFSSL_CERT_EXT)
            /* certificate extensions requires 24k for the SSL struct */
            #ifndef LARGEST_MEM_BUCKET
                #define LARGEST_MEM_BUCKET 24576
            #endif
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3456,4544,\
                                    LARGEST_MEM_BUCKET
        #else
            /* increase 23k for object member of WOLFSSL_X509_NAME_ENTRY */
            #ifndef LARGEST_MEM_BUCKET
                #define LARGEST_MEM_BUCKET 23440
            #endif
            #define WOLFMEM_BUCKETS 64,128,256,512,1024,2432,3456,4544,\
                                    LARGEST_MEM_BUCKET
        #endif
    #endif
    #ifndef WOLFMEM_DIST
        #ifndef WOLFSSL_STATIC_MEMORY_SMALL
            #define WOLFMEM_DIST    49,10,6,14,5,6,9,1,1
        #else
            /* Low resource and not RSA */
            #define WOLFMEM_DIST    29, 7,6, 9,4,4,0,0,0
        #endif
    #endif

    /* flags for loading static memory (one hot bit) */
    #define WOLFMEM_GENERAL       0x01
    #define WOLFMEM_IO_POOL       0x02
    #define WOLFMEM_IO_POOL_FIXED 0x04
    #define WOLFMEM_TRACK_STATS   0x08

    #ifndef WOLFSSL_MEM_GUARD
    #define WOLFSSL_MEM_GUARD
        typedef struct WOLFSSL_MEM_STATS      WOLFSSL_MEM_STATS;
        typedef struct WOLFSSL_MEM_CONN_STATS WOLFSSL_MEM_CONN_STATS;
    #endif

    struct WOLFSSL_MEM_CONN_STATS {
        word32 peakMem;   /* peak memory usage    */
        word32 curMem;    /* current memory usage */
        word32 peakAlloc; /* peak memory allocations */
        word32 curAlloc;  /* current memory allocations */
        word32 totalAlloc;/* total memory allocations for lifetime */
        word32 totalFr;   /* total frees for lifetime */
    };

    struct WOLFSSL_MEM_STATS {
        word32 curAlloc;  /* current memory allocations */
        word32 totalAlloc;/* total memory allocations for lifetime */
        word32 totalFr;   /* total frees for lifetime */
        word32 totalUse;  /* total amount of memory used in blocks */
        word32 avaIO;     /* available IO specific pools */
        word32 maxHa;     /* max number of concurrent handshakes allowed */
        word32 maxIO;     /* max number of concurrent IO connections allowed */
        word32 blockSz[WOLFMEM_MAX_BUCKETS]; /* block sizes in stacks */
        word32 avaBlock[WOLFMEM_MAX_BUCKETS];/* ava block sizes */
        word32 usedBlock[WOLFMEM_MAX_BUCKETS];
        int    flag; /* flag used */
    };

    typedef struct wc_Memory wc_Memory; /* internal structure for mem bucket */
    typedef struct WOLFSSL_HEAP {
        wc_Memory* ava[WOLFMEM_MAX_BUCKETS];
        wc_Memory* io;                  /* list of buffers to use for IO */
        word32     maxHa;               /* max concurrent handshakes */
        word32     curHa;
        word32     maxIO;               /* max concurrent IO connections */
        word32     curIO;
        word32     sizeList[WOLFMEM_MAX_BUCKETS];/* memory sizes in ava list */
        word32     distList[WOLFMEM_MAX_BUCKETS];/* general distribution */
        word32     inUse; /* amount of memory currently in use */
        word32     ioUse;
        word32     alloc; /* total number of allocs */
        word32     frAlc; /* total number of frees  */
        int        flag;
        wolfSSL_Mutex memory_mutex;
    } WOLFSSL_HEAP;

    /* structure passed into XMALLOC as heap hint
     * having this abstraction allows tracking statistics of individual ssl's
     */
    typedef struct WOLFSSL_HEAP_HINT {
        WOLFSSL_HEAP*           memory;
        WOLFSSL_MEM_CONN_STATS* stats;  /* hold individual connection stats */
        wc_Memory*  outBuf; /* set if using fixed io buffers */
        wc_Memory*  inBuf;
        byte        haFlag; /* flag used for checking handshake count */
    } WOLFSSL_HEAP_HINT;

    WOLFSSL_API int wc_LoadStaticMemory(WOLFSSL_HEAP_HINT** pHint,
            unsigned char* buf, unsigned int sz, int flag, int max);

    WOLFSSL_LOCAL int wolfSSL_init_memory_heap(WOLFSSL_HEAP* heap);
    WOLFSSL_LOCAL int wolfSSL_load_static_memory(byte* buffer, word32 sz,
                                                  int flag, WOLFSSL_HEAP* heap);
    WOLFSSL_LOCAL int wolfSSL_GetMemStats(WOLFSSL_HEAP* heap,
                                                      WOLFSSL_MEM_STATS* stats);
    WOLFSSL_LOCAL int SetFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io);
    WOLFSSL_LOCAL int FreeFixedIO(WOLFSSL_HEAP* heap, wc_Memory** io);

    WOLFSSL_API int wolfSSL_StaticBufferSz(byte* buffer, word32 sz, int flag);
    WOLFSSL_API int wolfSSL_MemoryPaddingSz(void);
#endif /* WOLFSSL_STATIC_MEMORY */

#ifdef WOLFSSL_STACK_LOG
    WOLFSSL_API void __attribute__((no_instrument_function))
            __cyg_profile_func_enter(void *func,  void *caller);
    WOLFSSL_API void __attribute__((no_instrument_function))
            __cyg_profile_func_exit(void *func, void *caller);
#endif /* WOLFSSL_STACK_LOG */

#ifdef WOLFSSL_MEM_FAIL_COUNT
WOLFSSL_LOCAL void wc_MemFailCount_Init(void);
WOLFSSL_LOCAL void wc_MemFailCount_Free(void);
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
WOLFSSL_LOCAL void wc_MemZero_Init(void);
WOLFSSL_LOCAL void wc_MemZero_Free(void);
WOLFSSL_LOCAL void wc_MemZero_Add(const char* name, const void* addr,
    size_t len);
WOLFSSL_LOCAL void wc_MemZero_Check(void* addr, size_t len);
#endif

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_MEMORY_H */

