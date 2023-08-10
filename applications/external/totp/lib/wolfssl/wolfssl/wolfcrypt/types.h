/* types.h
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
    \file wolfssl/wolfcrypt/types.h
*/
/*
DESCRIPTION
This library defines the primitive data types and abstraction macros to
decouple library dependencies with standard string, memory and so on.

*/
#ifndef WOLF_CRYPT_TYPES_H
#define WOLF_CRYPT_TYPES_H

    #include <wolfssl/wolfcrypt/settings.h>
    #include <wolfssl/wolfcrypt/wc_port.h>

    #ifdef __cplusplus
        extern "C" {
    #endif


    /*
     * This struct is used multiple time by other structs and
     * needs to be defined somewhere that all structs can import
     * (with minimal dependencies).
     */
    #ifdef HAVE_EX_DATA
        #ifdef HAVE_EX_DATA_CLEANUP_HOOKS
        typedef void (*wolfSSL_ex_data_cleanup_routine_t)(void *data);
        #endif
        typedef struct WOLFSSL_CRYPTO_EX_DATA {
            void* ex_data[MAX_EX_DATA];
            #ifdef HAVE_EX_DATA_CLEANUP_HOOKS
            wolfSSL_ex_data_cleanup_routine_t
                ex_data_cleanup_routines[MAX_EX_DATA];
            #endif
        } WOLFSSL_CRYPTO_EX_DATA;
        typedef void (WOLFSSL_CRYPTO_EX_new)(void* p, void* ptr,
                WOLFSSL_CRYPTO_EX_DATA* a, int idx, long argValue, void* arg);
        typedef int  (WOLFSSL_CRYPTO_EX_dup)(WOLFSSL_CRYPTO_EX_DATA* out,
                const WOLFSSL_CRYPTO_EX_DATA* in, void* inPtr, int idx,
                long argV, void* arg);
        typedef void (WOLFSSL_CRYPTO_EX_free)(void* p, void* ptr,
                WOLFSSL_CRYPTO_EX_DATA* a, int idx, long argValue, void* arg);
    #endif

    #if defined(WORDS_BIGENDIAN)
        #define BIG_ENDIAN_ORDER
    #endif

    #ifndef BIG_ENDIAN_ORDER
        #define LITTLE_ENDIAN_ORDER
    #endif

    #ifndef WOLFSSL_TYPES
        #ifndef byte
            /* If using C++ C17 or later and getting:
             *   "error: reference to 'byte' is ambiguous", this is caused by
             * cstddef conflict with "std::byte" in
             *   "enum class byte : unsigned char {};".
             * This can occur if the user application is using "std" as the
             * default namespace before including wolfSSL headers.
             * Workarounds: https://github.com/wolfSSL/wolfssl/issues/5400
             */
            typedef unsigned char  byte;
        #endif
            typedef   signed char  sword8;
            typedef unsigned char  word8;
        #ifdef WC_16BIT_CPU
            typedef          int   sword16;
            typedef unsigned int   word16;
            typedef          long  sword32;
            typedef unsigned long  word32;
        #else
            typedef          short sword16;
            typedef unsigned short word16;
            typedef          int   sword32;
            typedef unsigned int   word32;
        #endif
        typedef byte           word24[3];
    #endif


    /* constant pointer to a constant char */
    #ifdef WOLFSSL_NO_CONSTCHARCONST
        typedef const char*       wcchar;
    #else
        typedef const char* const wcchar;
    #endif

    #ifndef HAVE_ANONYMOUS_INLINE_AGGREGATES
        /* if a version is available, pivot on the version, otherwise guess it's
         * allowed, subject to override.
         */
        #if !defined(WOLF_C89) && (!defined(__STDC__)                \
            || (!defined(__STDC_VERSION__) && !defined(__cplusplus)) \
            || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201101L)) \
            || (defined(__cplusplus) && (__cplusplus >= 201103L)))
            #define HAVE_ANONYMOUS_INLINE_AGGREGATES 1
        #else
            #define HAVE_ANONYMOUS_INLINE_AGGREGATES 0
        #endif
    #endif

    /* With a true C89-dialect compiler (simulate with gcc -std=c89 -Wall
     * -Wextra -pedantic), a trailing comma on the last value in an enum
     * definition is a syntax error.  We use this macro to accommodate that
     * without disrupting clean flow/syntax when some enum values are
     * preprocessor-gated.
     */
    #if defined(WOLF_C89) || defined(WOLF_NO_TRAILING_ENUM_COMMAS)
        #define WOLF_ENUM_DUMMY_LAST_ELEMENT(prefix) _wolf_ ## prefix ## _enum_dummy_last_element
    #else
        #define WOLF_ENUM_DUMMY_LAST_ELEMENT(prefix) /* null expansion */
    #endif

    /* helpers for stringifying the expanded value of a macro argument rather
     * than its literal text:
     */
    #define _WC_STRINGIFY_L2(str) #str
    #define WC_STRINGIFY(str) _WC_STRINGIFY_L2(str)

    /* try to set SIZEOF_LONG or SIZEOF_LONG_LONG if user didn't */
    #if defined(_WIN32) || defined(HAVE_LIMITS_H)
        /* make sure both SIZEOF_LONG_LONG and SIZEOF_LONG are set,
         * otherwise causes issues with CTC_SETTINGS */
        #if !defined(SIZEOF_LONG_LONG) || !defined(SIZEOF_LONG)
            #include <limits.h>
            #if !defined(SIZEOF_LONG) && defined(ULONG_MAX) && \
                    (ULONG_MAX == 0xffffffffUL)
                #define SIZEOF_LONG 4
            #endif
            #if !defined(SIZEOF_LONG_LONG) && defined(ULLONG_MAX) && \
                    (ULLONG_MAX == 0xffffffffffffffffULL)
                #define SIZEOF_LONG_LONG 8
            #endif
        #endif
    #elif !defined(__BCPLUSPLUS__) && !defined(__EMSCRIPTEN__)
        #if !defined(SIZEOF_LONG_LONG) && !defined(SIZEOF_LONG)
            #if (defined(__alpha__) || defined(__ia64__) || \
                defined(_ARCH_PPC64) || defined(__mips64) || \
                defined(__x86_64__)  || defined(__s390x__ ) || \
                ((defined(sun) || defined(__sun)) && \
                 (defined(LP64) || defined(_LP64))) || \
                (defined(__riscv_xlen) && (__riscv_xlen == 64)) || \
                defined(__aarch64__) || \
                (defined(__DCC__) && (defined(__LP64) || defined(__LP64__))))
                /* long should be 64bit */
                #define SIZEOF_LONG 8
            #elif defined(__i386__) || defined(__CORTEX_M3__)
                /* long long should be 64bit */
                #define SIZEOF_LONG_LONG 8
            #endif
         #endif
    #endif

    #if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
        #define WORD64_AVAILABLE
        #define W64LIT(x) x##ui64
        #define SW64LIT(x) x##i64
        typedef          __int64 sword64;
        typedef unsigned __int64 word64;
    #elif defined(__EMSCRIPTEN__)
        #define WORD64_AVAILABLE
        #define W64LIT(x) x##ull
        #define SW64LIT(x) x##ll
        typedef          long long sword64;
        typedef unsigned long long word64;
    #elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
        #define WORD64_AVAILABLE
        #ifdef WOLF_C89
            #define W64LIT(x) x##UL
            #define SW64LIT(x) x##L
        #else
            #define W64LIT(x) x##ULL
            #define SW64LIT(x) x##LL
        #endif
        typedef          long sword64;
        typedef unsigned long word64;
    #elif defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG == 8
        #define WORD64_AVAILABLE
        #ifdef WOLF_C89
            #define W64LIT(x) x##UL
            #define SW64LIT(x) x##L
        #else
            #define W64LIT(x) x##ULL
            #define SW64LIT(x) x##LL
        #endif
        typedef          long long sword64;
        typedef unsigned long long word64;
    #elif defined(__SIZEOF_LONG_LONG__) && __SIZEOF_LONG_LONG__ == 8
        #define WORD64_AVAILABLE
        #ifdef WOLF_C89
            #define W64LIT(x) x##UL
            #define SW64LIT(x) x##L
        #else
            #define W64LIT(x) x##ULL
            #define SW64LIT(x) x##LL
        #endif
        typedef          long long sword64;
        typedef unsigned long long word64;
    #endif

#if defined(WORD64_AVAILABLE) && !defined(WC_16BIT_CPU)
    /* These platforms have 64-bit CPU registers.  */
    #if (defined(__alpha__) || defined(__ia64__) || defined(_ARCH_PPC64) || \
        (defined(__mips64) && \
         ((defined(_ABI64) && (_MIPS_SIM == _ABI64)) || \
          (defined(_ABIO64) && (_MIPS_SIM == _ABIO64)))) || \
         defined(__x86_64__) || defined(_M_X64)) || \
         defined(__aarch64__) || defined(__sparc64__) || defined(__s390x__ ) || \
        (defined(__riscv_xlen) && (__riscv_xlen == 64)) || defined(_M_ARM64) || \
        defined(__aarch64__) || \
        (defined(__DCC__) && (defined(__LP64) || defined(__LP64__)))
        #define WC_64BIT_CPU
    #elif (defined(sun) || defined(__sun)) && \
          (defined(LP64) || defined(_LP64))
        /* LP64 with GNU GCC compiler is reserved for when long int is 64 bits
         * and int uses 32 bits. When using Solaris Studio sparc and __sparc are
         * available for 32 bit detection but __sparc64__ could be missed. This
         * uses LP64 for checking 64 bit CPU arch. */
        #define WC_64BIT_CPU
    #else
        #define WC_32BIT_CPU
    #endif

    #if defined(NO_64BIT)
          typedef word32 wolfssl_word;
          #undef WORD64_AVAILABLE
    #else
        #ifdef WC_64BIT_CPU
          typedef word64 wolfssl_word;
        #else
          typedef word32 wolfssl_word;
          #ifdef WORD64_AVAILABLE
              #define WOLFCRYPT_SLOW_WORD64
          #endif
        #endif
    #endif

#elif defined(WC_16BIT_CPU)
        #undef WORD64_AVAILABLE
        typedef word16 wolfssl_word;
        #define MP_16BIT  /* for mp_int, mp_word needs to be twice as big as \
                           * mp_digit, no 64 bit type so make mp_digit 16 bit */

#else
        #undef WORD64_AVAILABLE
        typedef word32 wolfssl_word;
        #define MP_16BIT  /* for mp_int, mp_word needs to be twice as big as \
                           * mp_digit, no 64 bit type so make mp_digit 16 bit */
#endif

typedef struct w64wrapper {
#if defined(WORD64_AVAILABLE) && !defined(WOLFSSL_W64_WRAPPER_TEST)
    word64 n;
#else
    word32 n[2];
#endif /* WORD64_AVAILABLE && WOLFSSL_W64_WRAPPER_TEST */
} w64wrapper;

#ifdef WC_PTR_TYPE /* Allow user supplied type */
    typedef WC_PTR_TYPE wc_ptr_t;
#elif defined(HAVE_UINTPTR_T)
    #include <stdint.h>
    typedef uintptr_t wc_ptr_t;
#else /* fallback to architecture size_t for pointer size */
    #include <stddef.h> /* included for getting size_t type */
    typedef size_t wc_ptr_t;
#endif

    enum {
        WOLFSSL_WORD_SIZE  = sizeof(wolfssl_word),
        WOLFSSL_BIT_SIZE   = 8,
        WOLFSSL_WORD_BITS  = WOLFSSL_WORD_SIZE * WOLFSSL_BIT_SIZE
    };

    #define WOLFSSL_MAX_16BIT 0xffffU
    #define WOLFSSL_MAX_32BIT 0xffffffffU

    #ifndef WARN_UNUSED_RESULT
        #if defined(WOLFSSL_LINUXKM) && defined(__must_check)
            #define WARN_UNUSED_RESULT __must_check
        #elif defined(__GNUC__) && (__GNUC__ >= 4)
            #define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
        #else
            #define WARN_UNUSED_RESULT
        #endif
    #endif /* WARN_UNUSED_RESULT */

    #ifndef WC_MAYBE_UNUSED
        #if (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__)
            #define WC_MAYBE_UNUSED __attribute__((unused))
        #else
            #define WC_MAYBE_UNUSED
        #endif
    #endif /* WC_MAYBE_UNUSED */

    #ifndef WC_DO_NOTHING
        #define WC_DO_NOTHING do {} while (0)
        #ifdef _MSC_VER
            /* disable buggy MSC warning around while(0),
             *"warning C4127: conditional expression is constant"
             */
            #pragma warning(disable: 4127)
        #endif
    #endif

    /* use inlining if compiler allows */
    #ifndef WC_INLINE
    #ifndef NO_INLINE
        #ifdef _MSC_VER
            #define WC_INLINE __inline
        #elif defined(__GNUC__)
               #ifdef WOLFSSL_VXWORKS
                   #define WC_INLINE __inline__
               #else
                   #define WC_INLINE inline
               #endif
        #elif defined(__IAR_SYSTEMS_ICC__)
            #define WC_INLINE inline
        #elif defined(THREADX)
            #define WC_INLINE _Inline
        #elif defined(__ghc__)
            #ifndef __cplusplus
                #define WC_INLINE __inline
            #else
                #define WC_INLINE inline
            #endif
        #elif defined(__CCRX__)
            #define WC_INLINE inline
        #elif defined(__DCC__)
            #ifndef __cplusplus
                #define WC_INLINE __inline__
            #else
                #define WC_INLINE inline
            #endif
        #else
            #define WC_INLINE
        #endif
    #else
        #define WC_INLINE WC_MAYBE_UNUSED
    #endif
    #endif

    #if defined(HAVE_FIPS) || defined(HAVE_SELFTEST)
        #define INLINE WC_INLINE
    #endif

    /* set up rotate style */
    #if (defined(_MSC_VER) || defined(__BCPLUSPLUS__)) && \
        !defined(WOLFSSL_SGX) && !defined(INTIME_RTOS)
        #define INTEL_INTRINSICS
        #define FAST_ROTATE
    #elif defined(__MWERKS__) && TARGET_CPU_PPC
        #define PPC_INTRINSICS
        #define FAST_ROTATE
    #elif defined(__CCRX__)
        #define FAST_ROTATE
    #elif defined(__GNUC__)  && (defined(__i386__) || defined(__x86_64__))
        /* GCC does peephole optimizations which should result in using rotate
           instructions  */
        #define FAST_ROTATE
    #endif

    /* set up thread local storage if available */
    #ifdef HAVE_THREAD_LS
        #if defined(_MSC_VER)
            #define THREAD_LS_T __declspec(thread)
        /* Thread local storage only in FreeRTOS v8.2.1 and higher */
        #elif defined(FREERTOS) || defined(FREERTOS_TCP) || \
                                                         defined(WOLFSSL_ZEPHYR)
            #define THREAD_LS_T
        #else
            #define THREAD_LS_T __thread
        #endif
    #else
        #define THREAD_LS_T
    #endif

    #ifndef FALL_THROUGH
        /* GCC 7 has new switch() fall-through detection */
        #if defined(__GNUC__)
            #if defined(fallthrough)
                #define FALL_THROUGH fallthrough
            #elif ((__GNUC__ > 7) || ((__GNUC__ == 7) && (__GNUC_MINOR__ >= 1)))
                #define FALL_THROUGH ; __attribute__ ((fallthrough))
            #elif defined(__clang__) && defined(__clang_major__) && \
                    (__clang_major__ >= 12)
                #define FALL_THROUGH ; __attribute__ ((fallthrough))
            #endif
        #endif
    #endif /* FALL_THROUGH */
    #if !defined(FALL_THROUGH) || defined(__XC32)
        /* use stub for fall through by default or for Microchip compiler */
        #undef  FALL_THROUGH
        #define FALL_THROUGH
    #endif

    /* Micrium will use Visual Studio for compilation but not the Win32 API */
    #if defined(_WIN32) && !defined(MICRIUM) && !defined(FREERTOS) && \
        !defined(FREERTOS_TCP) && !defined(EBSNET) && \
        !defined(WOLFSSL_UTASKER) && !defined(INTIME_RTOS)
        #define USE_WINDOWS_API
    #endif

    #define XSTR_SIZEOF(x) (sizeof(x) - 1) /* -1 to not count the null char */

    /* idea to add global alloc override by Moises Guimaraes  */
    /* default to libc stuff */
    /* XREALLOC is used once in normal math lib, not in fast math lib */
    /* XFREE on some embedded systems doesn't like free(0) so test  */
    #if defined(HAVE_IO_POOL)
        WOLFSSL_API void* XMALLOC(size_t n, void* heap, int type);
        WOLFSSL_API void* XREALLOC(void *p, size_t n, void* heap, int type);
        WOLFSSL_API void XFREE(void *p, void* heap, int type);
    #elif (defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_INTEL_QA)) || \
          defined(HAVE_INTEL_QA_SYNC)
        #ifndef HAVE_INTEL_QA_SYNC
            #include <wolfssl/wolfcrypt/port/intel/quickassist_mem.h>
            #undef USE_WOLFSSL_MEMORY
            #ifdef WOLFSSL_DEBUG_MEMORY
                #define XMALLOC(s, h, t)     IntelQaMalloc((s), (h), (t), __func__, __LINE__)
                #define XFREE(p, h, t)       IntelQaFree((p), (h), (t), __func__, __LINE__)
                #define XREALLOC(p, n, h, t) IntelQaRealloc((p), (n), (h), (t), __func__, __LINE__)
            #else
                #define XMALLOC(s, h, t)     IntelQaMalloc((s), (h), (t))
                #define XFREE(p, h, t)       IntelQaFree((p), (h), (t))
                #define XREALLOC(p, n, h, t) IntelQaRealloc((p), (n), (h), (t))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #else
            #include <wolfssl/wolfcrypt/port/intel/quickassist_sync.h>
            #undef USE_WOLFSSL_MEMORY
            #ifdef WOLFSSL_DEBUG_MEMORY
                #define XMALLOC(s, h, t)     wc_CryptoCb_IntelQaMalloc((s), (h), (t), __func__, __LINE__)
                #define XFREE(p, h, t)       wc_CryptoCb_IntelQaFree((p), (h), (t), __func__, __LINE__)
                #define XREALLOC(p, n, h, t) wc_CryptoCb_IntelQaRealloc((p), (n), (h), (t), __func__, __LINE__)
            #else
                #define XMALLOC(s, h, t)     wc_CryptoCb_IntelQaMalloc((s), (h), (t))
                #define XFREE(p, h, t)       wc_CryptoCb_IntelQaFree((p), (h), (t))
                #define XREALLOC(p, n, h, t) wc_CryptoCb_IntelQaRealloc((p), (n), (h), (t))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #endif
    #elif defined(XMALLOC_USER)
        /* prototypes for user heap override functions */
        #include <stddef.h>  /* for size_t */
        extern void *XMALLOC(size_t n, void* heap, int type);
        extern void *XREALLOC(void *p, size_t n, void* heap, int type);
        extern void XFREE(void *p, void* heap, int type);
    #elif defined(WOLFSSL_MEMORY_LOG)
        #define XMALLOC(n, h, t)        xmalloc(n, h, t, __func__, __FILE__, __LINE__)
        #define XREALLOC(p, n, h, t)    xrealloc(p, n, h, t, __func__,  __FILE__, __LINE__)
        #define XFREE(p, h, t)          xfree(p, h, t, __func__, __FILE__, __LINE__)

        /* prototypes for user heap override functions */
        #include <stddef.h>  /* for size_t */
        #include <stdlib.h>
        WOLFSSL_API void *xmalloc(size_t n, void* heap, int type,
                const char* func, const char* file, unsigned int line);
        WOLFSSL_API void *xrealloc(void *p, size_t n, void* heap, int type,
                const char* func, const char* file, unsigned int line);
        WOLFSSL_API void xfree(void *p, void* heap, int type, const char* func,
                const char* file, unsigned int line);
    #elif defined(XMALLOC_OVERRIDE)
        /* override the XMALLOC, XFREE and XREALLOC macros */
    #elif defined(WOLFSSL_TELIT_M2MB)
        /* Telit M2MB SDK requires use m2mb_os API's, not std malloc/free */
        /* Use of malloc/free will cause CPU reboot */
        #define XMALLOC(s, h, t)     ((void)h, (void)t, m2mb_os_malloc((s)))
        #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
            #define XFREE(p, h, t)       m2mb_os_free(xp)
        #else
            #define XFREE(p, h, t)       {void* xp = (p); if (xp) m2mb_os_free(xp);}
        #endif
        #define XREALLOC(p, n, h, t) m2mb_os_realloc((p), (n))

    #elif defined(NO_WOLFSSL_MEMORY)
        #ifdef WOLFSSL_NO_MALLOC
            /* this platform does not support heap use */
            #ifdef WOLFSSL_MALLOC_CHECK
                #include <stdio.h>
                static inline void* malloc_check(size_t sz) {
                    fprintf(stderr, "wolfSSL_malloc failed");
                    return NULL;
                };
                #define XMALLOC(s, h, t)     malloc_check((s))
                #define XFREE(p, h, t)       WC_DO_NOTHING
                #define XREALLOC(p, n, h, t) (NULL)
            #else
                #define XMALLOC(s, h, t)     (NULL)
                #define XFREE(p, h, t)       WC_DO_NOTHING
                #define XREALLOC(p, n, h, t) (NULL)
            #endif
        #else
        /* just use plain C stdlib stuff if desired */
        #include <stdlib.h>
        #define XMALLOC(s, h, t)     ((void)(h), (void)(t), malloc((size_t)(s)))
        #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
            #define XFREE(p, h, t)       ((void)(h), (void)(t), free(p))
        #else
            #define XFREE(p, h, t)       {void* xp = (p); if (xp) free(xp);}
        #endif
        #define XREALLOC(p, n, h, t) \
            ((void)(h), (void)(t), realloc((p), (size_t)(n)))
        #endif

    #elif defined(WOLFSSL_LINUXKM)

        /* definitions are in linuxkm/linuxkm_wc_port.h */

    #elif !defined(MICRIUM_MALLOC) && !defined(EBSNET) \
            && !defined(WOLFSSL_SAFERTOS) && !defined(FREESCALE_MQX) \
            && !defined(FREESCALE_KSDK_MQX) && !defined(FREESCALE_FREE_RTOS) \
            && !defined(WOLFSSL_LEANPSK) && !defined(WOLFSSL_uITRON4)
        /* default C runtime, can install different routines at runtime via cbs */
        #ifndef WOLFSSL_MEMORY_H
            #include <wolfssl/wolfcrypt/memory.h>
        #endif
        #ifdef WOLFSSL_STATIC_MEMORY
            #ifdef WOLFSSL_DEBUG_MEMORY
                #define XMALLOC(s, h, t)     wolfSSL_Malloc((s), (h), (t), __func__, __LINE__)
                #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
                    #define XFREE(p, h, t)       wolfSSL_Free(xp, h, t, __func__, __LINE__)
                #else
                    #define XFREE(p, h, t)       {void* xp = (p); if (xp) wolfSSL_Free(xp, h, t, __func__, __LINE__);}
                #endif
                #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), (h), (t), __func__, __LINE__)
            #else
                #define XMALLOC(s, h, t)     wolfSSL_Malloc((s), (h), (t))
                #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
                    #define XFREE(p, h, t)       wolfSSL_Free(xp, h, t)
                #else
                    #define XFREE(p, h, t)       {void* xp = (p); if (xp) wolfSSL_Free(xp, h, t);}
                #endif
                #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), (h), (t))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #elif (!defined(FREERTOS) && !defined(FREERTOS_TCP)) || defined(WOLFSSL_TRACK_MEMORY)
            #ifdef WOLFSSL_DEBUG_MEMORY
                #define XMALLOC(s, h, t)     ((void)(h), (void)(t), wolfSSL_Malloc((s), __func__, __LINE__))
                #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
                    #define XFREE(p, h, t)       wolfSSL_Free(xp, __func__, __LINE__)
                #else
                    #define XFREE(p, h, t)       {void* xp = (p); if (xp) wolfSSL_Free(xp, __func__, __LINE__);}
                #endif
                #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), __func__, __LINE__)
            #else
                #define XMALLOC(s, h, t)     ((void)(h), (void)(t), wolfSSL_Malloc((s)))
                #ifdef WOLFSSL_XFREE_NO_NULLNESS_CHECK
                    #define XFREE(p, h, t)       wolfSSL_Free(p)
                #else
                    #define XFREE(p, h, t)       {void* xp = (p); if (xp) wolfSSL_Free(xp);}
                #endif
                #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #endif /* WOLFSSL_STATIC_MEMORY */
    #endif

    /* declare/free variable handling for async and smallstack */
    #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLFSSL_SMALL_STACK)
        #define WC_DECLARE_VAR_IS_HEAP_ALLOC
        #define WC_DECLARE_VAR(VAR_NAME, VAR_TYPE, VAR_SIZE, HEAP) \
            VAR_TYPE* VAR_NAME = (VAR_TYPE*)XMALLOC(sizeof(VAR_TYPE) * (VAR_SIZE), (HEAP), DYNAMIC_TYPE_WOLF_BIGINT)
        #define WC_DECLARE_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            VAR_TYPE* VAR_NAME[VAR_ITEMS]; \
            int idx##VAR_NAME, inner_idx_##VAR_NAME
        #define WC_INIT_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            for (idx##VAR_NAME=0; idx##VAR_NAME<(VAR_ITEMS); idx##VAR_NAME++) { \
                (VAR_NAME)[idx##VAR_NAME] = (VAR_TYPE*)XMALLOC(VAR_SIZE, (HEAP), DYNAMIC_TYPE_WOLF_BIGINT); \
                if ((VAR_NAME)[idx##VAR_NAME] == NULL) { \
                    for (inner_idx_##VAR_NAME = 0; inner_idx_##VAR_NAME < idx##VAR_NAME; inner_idx_##VAR_NAME++) { \
                        XFREE((VAR_NAME)[inner_idx_##VAR_NAME], (HEAP), DYNAMIC_TYPE_WOLF_BIGINT); \
                        (VAR_NAME)[inner_idx_##VAR_NAME] = NULL; \
                    } \
                    for (inner_idx_##VAR_NAME = idx##VAR_NAME + 1; inner_idx_##VAR_NAME < (VAR_ITEMS); inner_idx_##VAR_NAME++) { \
                        (VAR_NAME)[inner_idx_##VAR_NAME] = NULL; \
                    } \
                    break; \
                } \
            }
        #define WC_FREE_VAR(VAR_NAME, HEAP) \
            XFREE(VAR_NAME, (HEAP), DYNAMIC_TYPE_WOLF_BIGINT)
        #define WC_FREE_ARRAY(VAR_NAME, VAR_ITEMS, HEAP) \
            for (idx##VAR_NAME=0; idx##VAR_NAME<(VAR_ITEMS); idx##VAR_NAME++) { \
                XFREE((VAR_NAME)[idx##VAR_NAME], (HEAP), DYNAMIC_TYPE_WOLF_BIGINT); \
            }

        #define WC_DECLARE_ARRAY_DYNAMIC_DEC(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            WC_DECLARE_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP)
        #define WC_DECLARE_ARRAY_DYNAMIC_EXE(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            WC_INIT_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP)
        #define WC_FREE_ARRAY_DYNAMIC(VAR_NAME, VAR_ITEMS, HEAP) \
            WC_FREE_ARRAY(VAR_NAME, VAR_ITEMS, HEAP)
    #else
        #undef WC_DECLARE_VAR_IS_HEAP_ALLOC
        #define WC_DECLARE_VAR(VAR_NAME, VAR_TYPE, VAR_SIZE, HEAP) \
            VAR_TYPE VAR_NAME[VAR_SIZE]
        #define WC_DECLARE_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            VAR_TYPE VAR_NAME[VAR_ITEMS][VAR_SIZE]
        #define WC_INIT_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) WC_DO_NOTHING
        #define WC_FREE_VAR(VAR_NAME, HEAP) WC_DO_NOTHING /* nothing to free, its stack */
        #define WC_FREE_ARRAY(VAR_NAME, VAR_ITEMS, HEAP) WC_DO_NOTHING /* nothing to free, its stack */

        #define WC_DECLARE_ARRAY_DYNAMIC_DEC(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            VAR_TYPE* VAR_NAME[VAR_ITEMS]; \
            int idx##VAR_NAME, inner_idx_##VAR_NAME
        #define WC_DECLARE_ARRAY_DYNAMIC_EXE(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            for (idx##VAR_NAME=0; idx##VAR_NAME<(VAR_ITEMS); idx##VAR_NAME++) { \
                (VAR_NAME)[idx##VAR_NAME] = (VAR_TYPE*)XMALLOC(VAR_SIZE, (HEAP), DYNAMIC_TYPE_TMP_BUFFER); \
                if ((VAR_NAME)[idx##VAR_NAME] == NULL) { \
                    for (inner_idx_##VAR_NAME = 0; inner_idx_##VAR_NAME < idx##VAR_NAME; inner_idx_##VAR_NAME++) { \
                        XFREE((VAR_NAME)[inner_idx_##VAR_NAME], HEAP, DYNAMIC_TYPE_TMP_BUFFER); \
                        (VAR_NAME)[inner_idx_##VAR_NAME] = NULL; \
                    } \
                    for (inner_idx_##VAR_NAME = idx##VAR_NAME + 1; inner_idx_##VAR_NAME < (VAR_ITEMS); inner_idx_##VAR_NAME++) { \
                        (VAR_NAME)[inner_idx_##VAR_NAME] = NULL; \
                    } \
                    break; \
                } \
            }
        #define WC_FREE_ARRAY_DYNAMIC(VAR_NAME, VAR_ITEMS, HEAP) \
            for (idx##VAR_NAME=0; idx##VAR_NAME<(VAR_ITEMS); idx##VAR_NAME++) { \
                XFREE((VAR_NAME)[idx##VAR_NAME], (HEAP), DYNAMIC_TYPE_TMP_BUFFER); \
            }
    #endif

    #if defined(HAVE_FIPS) || defined(HAVE_SELFTEST)
        /* These are here for the FIPS code that can't be changed. New definitions don't need to be added here. */
        #define DECLARE_VAR                 WC_DECLARE_VAR
        #define DECLARE_ARRAY               WC_DECLARE_ARRAY
        #define FREE_VAR                    WC_FREE_VAR
        #define FREE_ARRAY                  WC_FREE_ARRAY
        #define DECLARE_ARRAY_DYNAMIC_DEC   WC_DECLARE_ARRAY_DYNAMIC_DEC
        #define DECLARE_ARRAY_DYNAMIC_EXE   WC_DECLARE_ARRAY_DYNAMIC_EXE
        #define FREE_ARRAY_DYNAMIC          WC_FREE_ARRAY_DYNAMIC
    #endif /* HAVE_FIPS */

    #if !defined(USE_WOLF_STRTOK) && \
            ((defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)) || \
             defined(WOLFSSL_TIRTOS) || defined(WOLF_C99))
        #define USE_WOLF_STRTOK
    #endif
    #if !defined(USE_WOLF_STRSEP) && (defined(WOLF_C89) || defined(WOLF_C99))
        #define USE_WOLF_STRSEP
    #endif
    #if !defined(XSTRLCPY) && !defined(USE_WOLF_STRLCPY)
        #define USE_WOLF_STRLCPY
    #endif
    #if !defined(XSTRLCAT) && !defined(USE_WOLF_STRLCAT)
        #define USE_WOLF_STRLCAT
    #endif

        #ifndef STRING_USER
        #if defined(WOLFSSL_LINUXKM)
            #include <linux/string.h>
        #else
            #include <string.h>
        #endif

            #define XMEMCPY(d,s,l)    memcpy((d),(s),(l))
            #define XMEMSET(b,c,l)    memset((b),(c),(l))
            #define XMEMCMP(s1,s2,n)  memcmp((s1),(s2),(n))
            #define XMEMMOVE(d,s,l)   memmove((d),(s),(l))

        #define XSTRLEN(s1)       strlen((s1))
        #define XSTRNCPY(s1,s2,n) strncpy((s1),(s2),(n))
        /* strstr, strncmp, strcmp, and strncat only used by wolfSSL proper,
         * not required for wolfCrypt only */
        #define XSTRSTR(s1,s2)    strstr((s1),(s2))
        #define XSTRNSTR(s1,s2,n) mystrnstr((s1),(s2),(n))
        #define XSTRNCMP(s1,s2,n) strncmp((s1),(s2),(n))
        #define XSTRCMP(s1,s2)    strcmp((s1),(s2))
        #define XSTRNCAT(s1,s2,n) strncat((s1),(s2),(n))

        #ifdef USE_WOLF_STRSEP
            #define XSTRSEP(s1,d) wc_strsep((s1),(d))
        #else
            #define XSTRSEP(s1,d) strsep((s1),(d))
        #endif

        #ifndef XSTRCASECMP
        #if defined(MICROCHIP_PIC32) && (__XC32_VERSION >= 1000)
            /* XC32 supports str[n]casecmp in version >= 1.0. */
            #define XSTRCASECMP(s1,s2) strcasecmp((s1),(s2))
        #elif defined(MICROCHIP_PIC32) || defined(WOLFSSL_TIRTOS) || \
                defined(WOLFSSL_ZEPHYR)
            /* XC32 version < 1.0 does not support strcasecmp. */
            #define USE_WOLF_STRCASECMP
            #define XSTRCASECMP(s1,s2) wc_strcasecmp(s1,s2)
        #elif defined(USE_WINDOWS_API) || defined(FREERTOS_TCP_WINSIM)
            #define XSTRCASECMP(s1,s2) _stricmp((s1),(s2))
        #else
            #if defined(HAVE_STRINGS_H) && defined(WOLF_C99) && \
                !defined(WOLFSSL_SGX)
                #include <strings.h>
            #endif
            #if defined(WOLFSSL_DEOS)
                #define XSTRCASECMP(s1,s2) stricmp((s1),(s2))
            #elif defined(WOLFSSL_CMSIS_RTOSv2) || defined(WOLFSSL_AZSPHERE) \
                    || defined(WOLF_C89)
                #define USE_WOLF_STRCASECMP
                #define XSTRCASECMP(s1,s2) wc_strcasecmp(s1, s2)
            #elif defined(WOLF_C89)
                #define XSTRCASECMP(s1,s2) strcmp((s1),(s2))
            #else
                #define XSTRCASECMP(s1,s2) strcasecmp((s1),(s2))
            #endif
        #endif
        #endif /* !XSTRCASECMP */

        #ifndef XSTRNCASECMP
        #if defined(MICROCHIP_PIC32) && (__XC32_VERSION >= 1000)
            /* XC32 supports str[n]casecmp in version >= 1.0. */
            #define XSTRNCASECMP(s1,s2,n) strncasecmp((s1),(s2),(n))
        #elif defined(MICROCHIP_PIC32) || defined(WOLFSSL_TIRTOS) || \
                defined(WOLFSSL_ZEPHYR)
            /* XC32 version < 1.0 does not support strncasecmp. */
            #define USE_WOLF_STRNCASECMP
            #define XSTRNCASECMP(s1,s2) wc_strncasecmp(s1,s2)
        #elif defined(USE_WINDOWS_API) || defined(FREERTOS_TCP_WINSIM)
            #define XSTRNCASECMP(s1,s2,n) _strnicmp((s1),(s2),(n))
        #else
            #if defined(HAVE_STRINGS_H) && defined(WOLF_C99) && \
                !defined(WOLFSSL_SGX)
                #include <strings.h>
            #endif
            #if defined(WOLFSSL_DEOS)
                #define XSTRNCASECMP(s1,s2,n) strnicmp((s1),(s2),(n))
            #elif defined(WOLFSSL_CMSIS_RTOSv2) || defined(WOLFSSL_AZSPHERE) \
                    || defined(WOLF_C89)
                #define USE_WOLF_STRNCASECMP
                #define XSTRNCASECMP(s1,s2,n) wc_strncasecmp(s1, s2 ,n)
            #elif defined(WOLF_C89)
                #define XSTRNCASECMP(s1,s2,n) strncmp((s1),(s2),(n))
            #else
                #define XSTRNCASECMP(s1,s2,n) strncasecmp((s1),(s2),(n))
            #endif
        #endif
        #endif /* !XSTRNCASECMP */

        /* snprintf is used in asn.c for GetTimeString, PKCS7 test, and when
           debugging is turned on */
        #ifndef USE_WINDOWS_API
            #ifndef XSNPRINTF
            #if defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) && \
                (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
                 defined(WOLFSSL_CERT_EXT) || defined(HAVE_PKCS7))
                /* case where stdio is not included else where but is needed
                   for snprintf */
                #include <stdio.h>
            #endif
            #if defined(WOLFSSL_ESPIDF) && \
                (!defined(NO_ASN_TIME) && defined(HAVE_PKCS7))
                    #include<stdarg.h>
                    /* later gcc than 7.1 introduces -Wformat-truncation    */
                    /* In cases when truncation is expected the caller needs*/
                    /* to check the return value from the function so that  */
                    /* compiler doesn't complain.                           */
                    /* xtensa-esp32-elf v8.2.0 warns truncation at          */
                    /* GetAsnTimeString()                                   */
                    static WC_INLINE
                    int _xsnprintf_(char *s, size_t n, const char *format, ...)
                    {
                        va_list ap;
                        int ret;

                        if ((int)n <= 0) return -1;

                        va_start(ap, format);

                        ret = XVSNPRINTF(s, n, format, ap);
                        if (ret < 0)
                            ret = -1;

                        va_end(ap);

                        return ret;
                    }
                #define XSNPRINTF _xsnprintf_
            #elif defined(WOLF_C89)
                #define XSPRINTF sprintf
            #else
                #define XSNPRINTF snprintf
            #endif
            #endif
        #else
            #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)
                #if defined(_MSC_VER) && (_MSC_VER >= 1900)
                    /* Beginning with the UCRT in Visual Studio 2015 and
                       Windows 10, snprintf is no longer identical to
                       _snprintf. The snprintf function behavior is now
                       C99 standard compliant. */
                    #include <stdio.h>
                    #define XSNPRINTF snprintf
                #else
                    /* 4996 warning to use MS extensions e.g., _sprintf_s
                       instead of _snprintf */
                    #if !defined(__MINGW32__)
                    #pragma warning(disable: 4996)
                    #endif
                    static WC_INLINE
                    int xsnprintf(char *buffer, size_t bufsize,
                            const char *format, ...) {
                        va_list ap;
                        int ret;

                        if ((int)bufsize <= 0) return -1;
                        va_start(ap, format);
                        ret = XVSNPRINTF(buffer, bufsize, format, ap);
                        if (ret >= (int)bufsize)
                            ret = -1;
                        va_end(ap);
                        return ret;
                    }
                    #define XSNPRINTF xsnprintf
                #endif /* (_MSC_VER >= 1900) */
            #else
                #define XSNPRINTF snprintf
            #endif /* _MSC_VER */
        #endif /* USE_WINDOWS_API */

        #if defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA) || \
            defined(HAVE_ALPN) || defined(WOLFSSL_SNIFFER)
            /* use only Thread Safe version of strtok */
            #if defined(USE_WOLF_STRTOK)
                #define XSTRTOK(s1,d,ptr) wc_strtok((s1),(d),(ptr))
            #elif defined(USE_WINDOWS_API) || defined(INTIME_RTOS)
                #define XSTRTOK(s1,d,ptr) strtok_s((s1),(d),(ptr))
            #else
                #define XSTRTOK(s1,d,ptr) strtok_r((s1),(d),(ptr))
            #endif
        #endif

        #if defined(WOLFSSL_CERT_EXT) || defined(HAVE_OCSP) || \
            defined(HAVE_CRL_IO) || defined(HAVE_HTTP_CLIENT) || \
            !defined(NO_CRYPT_BENCHMARK) || defined(OPENSSL_EXTRA)

            #ifndef XATOI /* if custom XATOI is not already defined */
                #include <stdlib.h>
                #define XATOI(s)          atoi((s))
            #endif
        #endif
    #endif

    #ifdef USE_WOLF_STRTOK
        WOLFSSL_API char* wc_strtok(char *str, const char *delim, char **nextp);
    #endif
    #ifdef USE_WOLF_STRSEP
        WOLFSSL_API char* wc_strsep(char **stringp, const char *delim);
    #endif

    #ifdef USE_WOLF_STRLCPY
        WOLFSSL_API size_t wc_strlcpy(char *dst, const char *src, size_t dstSize);
        #define XSTRLCPY(s1,s2,n) wc_strlcpy((s1),(s2),(n))
    #endif
    #ifdef USE_WOLF_STRLCAT
        WOLFSSL_API size_t wc_strlcat(char *dst, const char *src, size_t dstSize);
        #define XSTRLCAT(s1,s2,n) wc_strlcat((s1),(s2),(n))
    #endif
    #ifdef USE_WOLF_STRCASECMP
        WOLFSSL_API int wc_strcasecmp(const char *s1, const char *s2);
    #endif
    #ifdef USE_WOLF_STRNCASECMP
        WOLFSSL_API int wc_strncasecmp(const char *s1, const char *s2, size_t n);
    #endif

    #if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
        #ifndef XGETENV
            #include <stdlib.h>
            #define XGETENV getenv
        #endif
    #endif /* !NO_FILESYSTEM && !NO_STDIO_FILESYSTEM */

    #ifndef CTYPE_USER
        #ifndef WOLFSSL_LINUXKM
            #include <ctype.h>
        #endif
        #if defined(HAVE_ECC) || defined(HAVE_OCSP) || \
        defined(WOLFSSL_KEY_GEN) || !defined(NO_DSA) || \
        defined(OPENSSL_EXTRA)
            #define XTOUPPER(c)     toupper((c))
        #endif
        #if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
        #define XISALNUM(c)     isalnum((c))
        #define XISASCII(c)     isascii((c))
        #define XISSPACE(c)     isspace((c))
        #endif
        /* needed by wolfSSL_check_domain_name() */
        #define XTOLOWER(c)      tolower((c))
    #endif

    #ifndef OFFSETOF
        #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 4))
            #define OFFSETOF(type, field) __builtin_offsetof(type, field)
        #else
            #define OFFSETOF(type, field) ((size_t)&(((type *)0)->field))
        #endif
    #endif


    /* memory allocation types for user hints */
    enum {
        DYNAMIC_TYPE_CA           = 1,
        DYNAMIC_TYPE_CERT         = 2,
        DYNAMIC_TYPE_KEY          = 3,
        DYNAMIC_TYPE_FILE         = 4,
        DYNAMIC_TYPE_SUBJECT_CN   = 5,
        DYNAMIC_TYPE_PUBLIC_KEY   = 6,
        DYNAMIC_TYPE_SIGNER       = 7,
        DYNAMIC_TYPE_NONE         = 8,
        DYNAMIC_TYPE_BIGINT       = 9,
        DYNAMIC_TYPE_RSA          = 10,
        DYNAMIC_TYPE_METHOD       = 11,
        DYNAMIC_TYPE_OUT_BUFFER   = 12,
        DYNAMIC_TYPE_IN_BUFFER    = 13,
        DYNAMIC_TYPE_INFO         = 14,
        DYNAMIC_TYPE_DH           = 15,
        DYNAMIC_TYPE_DOMAIN       = 16,
        DYNAMIC_TYPE_SSL          = 17,
        DYNAMIC_TYPE_CTX          = 18,
        DYNAMIC_TYPE_WRITEV       = 19,
        DYNAMIC_TYPE_OPENSSL      = 20,
        DYNAMIC_TYPE_DSA          = 21,
        DYNAMIC_TYPE_CRL          = 22,
        DYNAMIC_TYPE_REVOKED      = 23,
        DYNAMIC_TYPE_CRL_ENTRY    = 24,
        DYNAMIC_TYPE_CERT_MANAGER = 25,
        DYNAMIC_TYPE_CRL_MONITOR  = 26,
        DYNAMIC_TYPE_OCSP_STATUS  = 27,
        DYNAMIC_TYPE_OCSP_ENTRY   = 28,
        DYNAMIC_TYPE_ALTNAME      = 29,
        DYNAMIC_TYPE_SUITES       = 30,
        DYNAMIC_TYPE_CIPHER       = 31,
        DYNAMIC_TYPE_RNG          = 32,
        DYNAMIC_TYPE_ARRAYS       = 33,
        DYNAMIC_TYPE_DTLS_POOL    = 34,
        DYNAMIC_TYPE_SOCKADDR     = 35,
        DYNAMIC_TYPE_LIBZ         = 36,
        DYNAMIC_TYPE_ECC          = 37,
        DYNAMIC_TYPE_TMP_BUFFER   = 38,
        DYNAMIC_TYPE_DTLS_MSG     = 39,
        DYNAMIC_TYPE_X509         = 40,
        DYNAMIC_TYPE_TLSX         = 41,
        DYNAMIC_TYPE_OCSP         = 42,
        DYNAMIC_TYPE_SIGNATURE    = 43,
        DYNAMIC_TYPE_HASHES       = 44,
        DYNAMIC_TYPE_SRP          = 45,
        DYNAMIC_TYPE_COOKIE_PWD   = 46,
        DYNAMIC_TYPE_USER_CRYPTO  = 47,
        DYNAMIC_TYPE_OCSP_REQUEST = 48,
        DYNAMIC_TYPE_X509_EXT     = 49,
        DYNAMIC_TYPE_X509_STORE   = 50,
        DYNAMIC_TYPE_X509_CTX     = 51,
        DYNAMIC_TYPE_URL          = 52,
        DYNAMIC_TYPE_DTLS_FRAG    = 53,
        DYNAMIC_TYPE_DTLS_BUFFER  = 54,
        DYNAMIC_TYPE_SESSION_TICK = 55,
        DYNAMIC_TYPE_PKCS         = 56,
        DYNAMIC_TYPE_MUTEX        = 57,
        DYNAMIC_TYPE_PKCS7        = 58,
        DYNAMIC_TYPE_AES_BUFFER   = 59,
        DYNAMIC_TYPE_WOLF_BIGINT  = 60,
        DYNAMIC_TYPE_ASN1         = 61,
        DYNAMIC_TYPE_LOG          = 62,
        DYNAMIC_TYPE_WRITEDUP     = 63,
        DYNAMIC_TYPE_PRIVATE_KEY  = 64,
        DYNAMIC_TYPE_HMAC         = 65,
        DYNAMIC_TYPE_ASYNC        = 66,
        DYNAMIC_TYPE_ASYNC_NUMA   = 67,
        DYNAMIC_TYPE_ASYNC_NUMA64 = 68,
        DYNAMIC_TYPE_CURVE25519   = 69,
        DYNAMIC_TYPE_ED25519      = 70,
        DYNAMIC_TYPE_SECRET       = 71,
        DYNAMIC_TYPE_DIGEST       = 72,
        DYNAMIC_TYPE_RSA_BUFFER   = 73,
        DYNAMIC_TYPE_DCERT        = 74,
        DYNAMIC_TYPE_STRING       = 75,
        DYNAMIC_TYPE_PEM          = 76,
        DYNAMIC_TYPE_DER          = 77,
        DYNAMIC_TYPE_CERT_EXT     = 78,
        DYNAMIC_TYPE_ALPN         = 79,
        DYNAMIC_TYPE_ENCRYPTEDINFO= 80,
        DYNAMIC_TYPE_DIRCTX       = 81,
        DYNAMIC_TYPE_HASHCTX      = 82,
        DYNAMIC_TYPE_SEED         = 83,
        DYNAMIC_TYPE_SYMMETRIC_KEY= 84,
        DYNAMIC_TYPE_ECC_BUFFER   = 85,
        DYNAMIC_TYPE_SALT         = 87,
        DYNAMIC_TYPE_HASH_TMP     = 88,
        DYNAMIC_TYPE_BLOB         = 89,
        DYNAMIC_TYPE_NAME_ENTRY   = 90,
        DYNAMIC_TYPE_CURVE448     = 91,
        DYNAMIC_TYPE_ED448        = 92,
        DYNAMIC_TYPE_AES          = 93,
        DYNAMIC_TYPE_CMAC         = 94,
        DYNAMIC_TYPE_FALCON       = 95,
        DYNAMIC_TYPE_SESSION      = 96,
        DYNAMIC_TYPE_DILITHIUM    = 97,
        DYNAMIC_TYPE_SPHINCS      = 98,
        DYNAMIC_TYPE_SM4_BUFFER   = 99,
        DYNAMIC_TYPE_SNIFFER_SERVER     = 1000,
        DYNAMIC_TYPE_SNIFFER_SESSION    = 1001,
        DYNAMIC_TYPE_SNIFFER_PB         = 1002,
        DYNAMIC_TYPE_SNIFFER_PB_BUFFER  = 1003,
        DYNAMIC_TYPE_SNIFFER_TICKET_ID  = 1004,
        DYNAMIC_TYPE_SNIFFER_NAMED_KEY  = 1005,
        DYNAMIC_TYPE_SNIFFER_KEY        = 1006
    };

    /* max error buffer string size */
    #ifndef WOLFSSL_MAX_ERROR_SZ
        #define WOLFSSL_MAX_ERROR_SZ 80
    #endif

    /* stack protection */
    enum {
        MIN_STACK_BUFFER = 8
    };


    /* Algorithm Types */
    enum wc_AlgoType {
        WC_ALGO_TYPE_NONE = 0,
        WC_ALGO_TYPE_HASH = 1,
        WC_ALGO_TYPE_CIPHER = 2,
        WC_ALGO_TYPE_PK = 3,
        WC_ALGO_TYPE_RNG = 4,
        WC_ALGO_TYPE_SEED = 5,
        WC_ALGO_TYPE_HMAC = 6,
        WC_ALGO_TYPE_CMAC = 7,

        WC_ALGO_TYPE_MAX = WC_ALGO_TYPE_CMAC
    };

    /* hash types */
    enum wc_HashType {
    #if defined(HAVE_SELFTEST) || (defined(HAVE_FIPS) && \
        ((! defined(HAVE_FIPS_VERSION)) || \
         defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION <= 2)))
        /* In selftest build, WC_* types are not mapped to WC_HASH_TYPE types.
         * Values here are based on old selftest hmac.h enum, with additions.
         * These values are fixed for backwards FIPS compatibility */
        WC_HASH_TYPE_NONE = 15,
        WC_HASH_TYPE_MD2 = 16,
        WC_HASH_TYPE_MD4 = 17,
        WC_HASH_TYPE_MD5 = 0,
        WC_HASH_TYPE_SHA = 1, /* SHA-1 (not old SHA-0) */
        WC_HASH_TYPE_SHA224 = 8,
        WC_HASH_TYPE_SHA256 = 2,
        WC_HASH_TYPE_SHA384 = 5,
        WC_HASH_TYPE_SHA512 = 4,
        WC_HASH_TYPE_MD5_SHA = 18,
        WC_HASH_TYPE_SHA3_224 = 10,
        WC_HASH_TYPE_SHA3_256 = 11,
        WC_HASH_TYPE_SHA3_384 = 12,
        WC_HASH_TYPE_SHA3_512 = 13,
        WC_HASH_TYPE_BLAKE2B = 14,
        WC_HASH_TYPE_BLAKE2S = 19,
        WC_HASH_TYPE_MAX = WC_HASH_TYPE_BLAKE2S,
        #ifndef WOLFSSL_NOSHA512_224
            #define WOLFSSL_NOSHA512_224
        #endif
        #ifndef WOLFSSL_NOSHA512_256
            #define WOLFSSL_NOSHA512_256
        #endif
    #else
        WC_HASH_TYPE_NONE = 0,
        WC_HASH_TYPE_MD2 = 1,
        WC_HASH_TYPE_MD4 = 2,
        WC_HASH_TYPE_MD5 = 3,
        WC_HASH_TYPE_SHA = 4, /* SHA-1 (not old SHA-0) */
        WC_HASH_TYPE_SHA224 = 5,
        WC_HASH_TYPE_SHA256 = 6,
        WC_HASH_TYPE_SHA384 = 7,
        WC_HASH_TYPE_SHA512 = 8,
        WC_HASH_TYPE_MD5_SHA = 9,
        WC_HASH_TYPE_SHA3_224 = 10,
        WC_HASH_TYPE_SHA3_256 = 11,
        WC_HASH_TYPE_SHA3_384 = 12,
        WC_HASH_TYPE_SHA3_512 = 13,
        WC_HASH_TYPE_BLAKE2B = 14,
        WC_HASH_TYPE_BLAKE2S = 15,
        #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_BLAKE2S
        #ifndef WOLFSSL_NOSHA512_224
            WC_HASH_TYPE_SHA512_224 = 16,
            #undef _WC_HASH_TYPE_MAX
            #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_SHA512_224
        #endif
        #ifndef WOLFSSL_NOSHA512_256
            WC_HASH_TYPE_SHA512_256 = 17,
            #undef _WC_HASH_TYPE_MAX
            #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_SHA512_256
        #endif
        #ifdef WOLFSSL_SHAKE128
            WC_HASH_TYPE_SHAKE128 = 18,
            #undef _WC_HASH_TYPE_MAX
            #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_SHAKE128
        #endif
        #ifdef WOLFSSL_SHAKE256
            WC_HASH_TYPE_SHAKE256 = 19,
            #undef _WC_HASH_TYPE_MAX
            #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_SHAKE256
        #endif
        #ifdef WOLFSSL_SM3
            WC_HASH_TYPE_SM3     = 20,
            #undef _WC_HASH_TYPE_MAX
            #define _WC_HASH_TYPE_MAX WC_HASH_TYPE_SM3
        #endif
        WC_HASH_TYPE_MAX = _WC_HASH_TYPE_MAX
        #undef _WC_HASH_TYPE_MAX

    #endif /* HAVE_SELFTEST */
    };

    /* cipher types */
    enum wc_CipherType {
        WC_CIPHER_NONE = 0,
        WC_CIPHER_AES = 1,
        WC_CIPHER_AES_CBC = 2,
        WC_CIPHER_AES_GCM = 3,
        WC_CIPHER_AES_CTR = 4,
        WC_CIPHER_AES_XTS = 5,
        WC_CIPHER_AES_CFB = 6,
        WC_CIPHER_AES_CCM = 12,
        WC_CIPHER_AES_ECB = 13,
        WC_CIPHER_DES3 = 7,
        WC_CIPHER_DES = 8,
        WC_CIPHER_CHACHA = 9,

        WC_CIPHER_MAX = WC_CIPHER_AES_CCM
    };

    /* PK=public key (asymmetric) based algorithms */
    enum wc_PkType {
        WC_PK_TYPE_NONE = 0,
        WC_PK_TYPE_RSA = 1,
        WC_PK_TYPE_DH = 2,
        WC_PK_TYPE_ECDH = 3,
        WC_PK_TYPE_ECDSA_SIGN = 4,
        WC_PK_TYPE_ECDSA_VERIFY = 5,
        WC_PK_TYPE_ED25519_SIGN = 6,
        WC_PK_TYPE_CURVE25519 = 7,
        WC_PK_TYPE_RSA_KEYGEN = 8,
        WC_PK_TYPE_EC_KEYGEN = 9,
        WC_PK_TYPE_RSA_CHECK_PRIV_KEY = 10,
        WC_PK_TYPE_EC_CHECK_PRIV_KEY = 11,
        WC_PK_TYPE_ED448 = 12,
        WC_PK_TYPE_CURVE448 = 13,
        WC_PK_TYPE_ED25519_VERIFY = 14,
        WC_PK_TYPE_ED25519_KEYGEN = 15,
        WC_PK_TYPE_CURVE25519_KEYGEN = 16,
        WC_PK_TYPE_RSA_GET_SIZE = 17,
        WC_PK_TYPE_MAX = WC_PK_TYPE_RSA_GET_SIZE
    };


    /* settings detection for compile vs runtime math incompatibilities */
    enum {
    #if !defined(USE_FAST_MATH) && !defined(SIZEOF_LONG) && !defined(SIZEOF_LONG_LONG)
        CTC_SETTINGS = 0x0
    #elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
        CTC_SETTINGS = 0x1
    #elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
        CTC_SETTINGS = 0x2
    #elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 4)
        CTC_SETTINGS = 0x4
    #elif defined(USE_FAST_MATH) && !defined(SIZEOF_LONG) && !defined(SIZEOF_LONG_LONG)
        CTC_SETTINGS = 0x8
    #elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
        CTC_SETTINGS = 0x10
    #elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
        CTC_SETTINGS = 0x20
    #elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 4)
        CTC_SETTINGS = 0x40
    #else
        #error "bad math long / long long settings"
    #endif
    };


    WOLFSSL_API word32 CheckRunTimeSettings(void);

    /* If user uses RSA, DH, DSA, or ECC math lib directly then fast math and long
       types need to match at compile time and run time, CheckCtcSettings will
       return 1 if a match otherwise 0 */
    #define CheckCtcSettings() (CTC_SETTINGS == CheckRunTimeSettings())

    /* invalid device id */
    #define INVALID_DEVID    (-2)

    #if defined(HAVE_FIPS) && FIPS_VERSION_LT(5,3)
        #ifdef XASM_LINK
            #error User-supplied XASM_LINK is not compatible with this FIPS version.
        #else
            /* use version in FIPS <=5.2 aes.c */
        #endif
    #elif defined(XASM_LINK)
        /* keep user-supplied definition */
    #elif defined(WOLFSSL_NO_ASM)
        #define XASM_LINK(f) /* null expansion */
    #elif defined(_MSC_VER)
        #define XASM_LINK(f) /* null expansion */
    #elif defined(__APPLE__)
        #define XASM_LINK(f) asm("_" f)
    #elif defined(__GNUC__)
        /* use alternate keyword for compatibility with -std=c99 */
        #define XASM_LINK(f) __asm__(f)
    #else
        #define XASM_LINK(f) asm(f)
    #endif

    /* AESNI requires alignment and ARMASM gains some performance from it.
     * Xilinx RSA operations require alignment.
     */
    #if defined(WOLFSSL_AESNI) || defined(WOLFSSL_ARMASM) || \
        defined(USE_INTEL_SPEEDUP) || defined(WOLFSSL_AFALG_XILINX) || \
        defined(WOLFSSL_XILINX)
          #ifndef WOLFSSL_USE_ALIGN
              #define WOLFSSL_USE_ALIGN
          #endif
    #endif /* WOLFSSL_AESNI || WOLFSSL_ARMASM || USE_INTEL_SPEEDUP || WOLFSSL_AFALG_XILINX */

    #ifdef WOLFSSL_USE_ALIGN
        #if !defined(ALIGN16)
            #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__) || \
                defined(__llvm__)
                #define ALIGN16 __attribute__ ( (aligned (16)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN16 __declspec (align (16))
            #else
                #define ALIGN16
            #endif
        #endif /* !ALIGN16 */

        #if !defined (ALIGN32)
            #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__) || \
                defined(__llvm__)
                #define ALIGN32 __attribute__ ( (aligned (32)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN32 __declspec (align (32))
            #else
                #define ALIGN32
            #endif
        #endif /* !ALIGN32 */

        #if !defined(ALIGN64)
            #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__) || \
                defined(__llvm__)
                #define ALIGN64 __attribute__ ( (aligned (64)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN64 __declspec (align (64))
            #else
                #define ALIGN64
            #endif
        #endif /* !ALIGN64 */

        #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__) || \
            defined(__llvm__)
            #define ALIGN128 __attribute__ ( (aligned (128)))
        #elif defined(_MSC_VER)
            /* disable align warning, we want alignment ! */
            #pragma warning(disable: 4324)
            #define ALIGN128 __declspec (align (128))
        #else
            #define ALIGN128
        #endif

        #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)  || \
            defined(__llvm__)
            #define ALIGN256 __attribute__ ( (aligned (256)))
        #elif defined(_MSC_VER)
            /* disable align warning, we want alignment ! */
            #pragma warning(disable: 4324)
            #define ALIGN256 __declspec (align (256))
        #else
            #define ALIGN256
        #endif

    #else
        #ifndef ALIGN16
            #define ALIGN16
        #endif
        #ifndef ALIGN32
            #define ALIGN32
        #endif
        #ifndef ALIGN64
            #define ALIGN64
        #endif
        #ifndef ALIGN128
            #define ALIGN128
        #endif
        #ifndef ALIGN256
            #define ALIGN256
        #endif
    #endif /* WOLFSSL_USE_ALIGN */

    #if !defined(PEDANTIC_EXTENSION)
        #if defined(__GNUC__)
            #define PEDANTIC_EXTENSION __extension__
        #else
            #define PEDANTIC_EXTENSION
        #endif
    #endif /* !PEDANTIC_EXTENSION */


    #ifndef TRUE
        #define TRUE  1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif

    #ifdef SINGLE_THREADED
        #if defined(WC_32BIT_CPU)
            typedef void*        THREAD_RETURN;
        #else
            typedef unsigned int THREAD_RETURN;
        #endif
        typedef void*            THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET) || \
          defined(FREESCALE_MQX)
        typedef unsigned int  THREAD_RETURN;
        typedef int           THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(WOLFSSL_NUCLEUS)
        typedef unsigned int  THREAD_RETURN;
        typedef intptr_t      THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(WOLFSSL_TIRTOS)
        typedef void          THREAD_RETURN;
        #define WOLFSSL_THREAD_VOID_RETURN
        typedef Task_Handle   THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(WOLFSSL_ZEPHYR)
        typedef void            THREAD_RETURN;
        #define WOLFSSL_THREAD_VOID_RETURN
        typedef struct {
            struct k_thread tid;
            k_thread_stack_t* threadStack;
        } THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(NETOS)
        typedef UINT        THREAD_RETURN;
        typedef struct {
            TX_THREAD tid;
            void* threadStack;
        } THREAD_TYPE;
        #define WOLFSSL_THREAD
        #define INFINITE TX_WAIT_FOREVER
        #define WAIT_OBJECT_0 TX_NO_WAIT
    #elif defined(WOLFSSL_LINUXKM)
        typedef unsigned int  THREAD_RETURN;
        typedef size_t        THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif (defined(_POSIX_THREADS) || defined(HAVE_PTHREAD)) && \
        !defined(__MINGW32__)
        #ifndef __MACH__
            #include <pthread.h>
            typedef struct COND_TYPE {
                pthread_mutex_t mutex;
                pthread_cond_t cond;
            } COND_TYPE;
        #else
            #include <dispatch/dispatch.h>
            typedef dispatch_semaphore_t COND_TYPE;
        #endif
        typedef void*         THREAD_RETURN;
        typedef pthread_t     THREAD_TYPE;
        #define WOLFSSL_COND
        #define WOLFSSL_THREAD
        #ifndef HAVE_SELFTEST
            #define WOLFSSL_THREAD_NO_JOIN
        #endif
    #elif defined(FREERTOS)
        typedef unsigned int   THREAD_RETURN;
        typedef TaskHandle_t   THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(_MSC_VER)
        typedef unsigned      THREAD_RETURN;
        typedef uintptr_t     THREAD_TYPE;
        typedef HANDLE        COND_TYPE;
        #define WOLFSSL_COND
        #define INVALID_THREAD_VAL ((THREAD_TYPE)(INVALID_HANDLE_VALUE))
        #define WOLFSSL_THREAD __stdcall
        #define WOLFSSL_THREAD_NO_JOIN __cdecl
    #else
        typedef unsigned int  THREAD_RETURN;
        typedef size_t        THREAD_TYPE;
        #define WOLFSSL_THREAD __stdcall
    #endif


    #ifndef SINGLE_THREADED
        /* Necessary headers should already be included. */

        #ifndef INVALID_THREAD_VAL
            #define INVALID_THREAD_VAL ((THREAD_TYPE)(-1))
        #endif

        #ifndef WOLFSSL_THREAD_VOID_RETURN
            #define WOLFSSL_RETURN_FROM_THREAD(x) return (THREAD_RETURN)(x)
        #else
            #define WOLFSSL_RETURN_FROM_THREAD(x) \
                do { (void)(x); return; } while(0)
        #endif

        /* List of defines/types and what they mean:
         * THREAD_RETURN - return type of a thread callback
         * THREAD_TYPE - type that should be passed into thread handling API
         * INVALID_THREAD_VAL - a value that THREAD_TYPE can be checked against
         *                      to check if the value is an invalid thread
         * WOLFSSL_THREAD - attribute that should be used to declare thread
         *                  callbacks
         * WOLFSSL_THREAD_NO_JOIN - attribute that should be used to declare
         *                          thread callbacks that don't require cleanup
         * WOLFSSL_COND - defined if this system suports signaling
         * COND_TYPE - type that should be passed into the signaling API
         * WOLFSSL_THREAD_VOID_RETURN - defined if the thread callback has a
         *                              void return
         * WOLFSSL_RETURN_FROM_THREAD - define used to correctly return from a
         *                              thread callback
         * THREAD_CB - thread callback type for regular threading API
         * THREAD_CB_NOJOIN - thread callback type for threading API that don't
         *                    require cleanup
         *
         * Other defines/types are specific for the threading implementation
         */

        /* Internal wolfSSL threading interface. It does NOT need to be ported
         * during initial porting efforts. This is a very basic interface. Some
         * areas don't use this interface on purpose as they need more control
         * over threads.
         *
         * It is currently used for:
         * - CRL monitor
         * - Testing
         * - Entropy generation */

        /* We don't support returns from threads */
        typedef THREAD_RETURN (WOLFSSL_THREAD *THREAD_CB)(void* arg);
        WOLFSSL_API int wolfSSL_NewThread(THREAD_TYPE* thread,
            THREAD_CB cb, void* arg);
        #ifdef WOLFSSL_THREAD_NO_JOIN
            /* Create a thread that will be automatically cleaned up. We can't
             * return a handle/pointer to the new thread because there are no
             * guarantees for how long it will be valid. */
            typedef THREAD_RETURN (WOLFSSL_THREAD_NO_JOIN *THREAD_CB_NOJOIN)
                    (void* arg);
            WOLFSSL_API int wolfSSL_NewThreadNoJoin(THREAD_CB_NOJOIN cb,
                    void* arg);
        #endif
        WOLFSSL_API int wolfSSL_JoinThread(THREAD_TYPE thread);

        #ifdef WOLFSSL_COND
            WOLFSSL_API int wolfSSL_CondInit(COND_TYPE* cond);
            WOLFSSL_API int wolfSSL_CondFree(COND_TYPE* cond);
            WOLFSSL_API int wolfSSL_CondSignal(COND_TYPE* cond);
            WOLFSSL_API int wolfSSL_CondWait(COND_TYPE* cond);
        #endif
    #else
        #define WOLFSSL_RETURN_FROM_THREAD(x) return (THREAD_RETURN)(x)
    #endif /* SINGLE_THREADED */

    #if defined(HAVE_STACK_SIZE)
        #define EXIT_TEST(ret) return (THREAD_RETURN)((size_t)(ret))
    #else
        #define EXIT_TEST(ret) return ret
    #endif


    #if (defined(__IAR_SYSTEMS_ICC__) && (__IAR_SYSTEMS_ICC__ > 8)) || \
         defined(__GNUC__)
        #define WOLFSSL_PACK __attribute__ ((packed))
    #else
        #define WOLFSSL_PACK
    #endif

    #ifndef __GNUC_PREREQ
        #if defined(__GNUC__) && defined(__GNUC_MINOR__)
            #define __GNUC_PREREQ(maj, min) \
                ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
        #else
            #define __GNUC_PREREQ(maj, min) (0) /* not GNUC */
        #endif
    #endif

    #if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)
        #define WC_NORETURN __attribute__((noreturn))
    #else
        #define WC_NORETURN
    #endif

    #if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
        defined(WOLFSSL_DEBUG_MATH) || defined(DEBUG_WOLFSSL) || \
        defined(WOLFSSL_PUBLIC_MP) || defined(OPENSSL_EXTRA) || \
            (defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT))
        #undef  WC_MP_TO_RADIX
        #define WC_MP_TO_RADIX
    #endif

    #if defined(__GNUC__) && __GNUC__ > 5
        #define PRAGMA_GCC_DIAG_PUSH _Pragma("GCC diagnostic push")
        #define PRAGMA_GCC(str) _Pragma(str)
        #define PRAGMA_GCC_DIAG_POP _Pragma("GCC diagnostic pop")
        #define PRAGMA_DIAG_PUSH PRAGMA_GCC_DIAG_PUSH
        #define PRAGMA(str) PRAGMA_GCC(str)
        #define PRAGMA_DIAG_POP PRAGMA_GCC_DIAG_POP
    #else
        #define PRAGMA_GCC_DIAG_PUSH /* null expansion */
        #define PRAGMA_GCC(str) /* null expansion */
        #define PRAGMA_GCC_DIAG_POP /* null expansion */
    #endif

    #ifdef __clang__
        #define PRAGMA_CLANG_DIAG_PUSH _Pragma("clang diagnostic push")
        #define PRAGMA_CLANG(str) _Pragma(str)
        #define PRAGMA_CLANG_DIAG_POP _Pragma("clang diagnostic pop")
        #define PRAGMA_DIAG_PUSH PRAGMA_CLANG_DIAG_PUSH
        #define PRAGMA(str) PRAGMA_CLANG(str)
        #define PRAGMA_DIAG_POP PRAGMA_CLANG_DIAG_POP
    #else
        #define PRAGMA_CLANG_DIAG_PUSH /* null expansion */
        #define PRAGMA_CLANG(str) /* null expansion */
        #define PRAGMA_CLANG_DIAG_POP /* null expansion */
    #endif

    #ifndef PRAGMA_DIAG_PUSH
        #define PRAGMA_DIAG_PUSH /* null expansion */
    #endif
    #ifndef PRAGMA
        #define PRAGMA(str) /* null expansion */
    #endif
    #ifndef PRAGMA_DIAG_POP
        #define PRAGMA_DIAG_POP /* null expansion */
    #endif

    #ifdef DEBUG_VECTOR_REGISTER_ACCESS
        WOLFSSL_API extern THREAD_LS_T int wc_svr_count;
        WOLFSSL_API extern THREAD_LS_T const char *wc_svr_last_file;
        WOLFSSL_API extern THREAD_LS_T int wc_svr_last_line;

        #ifdef DEBUG_VECTOR_REGISTERS_ABORT_ON_FAIL
            #define DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE abort();
        #elif defined(DEBUG_VECTOR_REGISTERS_EXIT_ON_FAIL)
            #define DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE exit(1);
        #else
            #define DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE
        #endif

        #define SAVE_VECTOR_REGISTERS(...) {                            \
            ++wc_svr_count;                                             \
            if (wc_svr_count > 5) {                                     \
                fprintf(stderr,                                         \
                        "%s @ L%d : incr : wc_svr_count %d (last op %s L%d)\n", \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        wc_svr_count,                                   \
                        wc_svr_last_file,                               \
                        wc_svr_last_line);                              \
                DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE                \
            }                                                           \
            wc_svr_last_file = __FILE__;                                \
            wc_svr_last_line = __LINE__;                                \
        }
        #define ASSERT_SAVED_VECTOR_REGISTERS(fail_clause) {            \
            if (wc_svr_count <= 0) {                                    \
                fprintf(stderr,                                         \
                        "ASSERT_SAVED_VECTOR_REGISTERS : %s @ L%d : wc_svr_count %d (last op %s L%d)\n", \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        wc_svr_count,                                   \
                        wc_svr_last_file,                               \
                        wc_svr_last_line);                              \
                DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE                \
                { fail_clause }                                         \
            }                                                           \
        }
        #define ASSERT_RESTORED_VECTOR_REGISTERS(fail_clause) {         \
            if (wc_svr_count != 0) {                                    \
                fprintf(stderr,                                         \
                        "ASSERT_RESTORED_VECTOR_REGISTERS : %s @ L%d : wc_svr_count %d (last op %s L%d)\n", \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        wc_svr_count,                                   \
                        wc_svr_last_file,                               \
                        wc_svr_last_line);                              \
                DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE                \
                { fail_clause }                                         \
            }                                                           \
        }
        #define RESTORE_VECTOR_REGISTERS(...) {                         \
            --wc_svr_count;                                             \
            if ((wc_svr_count > 4) || (wc_svr_count < 0)) {             \
                fprintf(stderr,                                         \
                        "%s @ L%d : decr : wc_svr_count %d (last op %s L%d)\n", \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        wc_svr_count,                                   \
                        wc_svr_last_file,                               \
                        wc_svr_last_line);                              \
                DEBUG_VECTOR_REGISTERS_EXTRA_FAIL_CLAUSE                \
            }                                                           \
            wc_svr_last_file = __FILE__;                                \
            wc_svr_last_line = __LINE__;                                \
        }
    #else
        #ifndef SAVE_VECTOR_REGISTERS
            #define SAVE_VECTOR_REGISTERS(...) WC_DO_NOTHING
        #endif
        #ifndef ASSERT_SAVED_VECTOR_REGISTERS
            #define ASSERT_SAVED_VECTOR_REGISTERS(...) WC_DO_NOTHING
        #endif
        #ifndef ASSERT_RESTORED_VECTOR_REGISTERS
            #define ASSERT_RESTORED_VECTOR_REGISTERS(...) WC_DO_NOTHING
        #endif
        #ifndef RESTORE_VECTOR_REGISTERS
            #define RESTORE_VECTOR_REGISTERS() WC_DO_NOTHING
        #endif
    #endif


    #if FIPS_VERSION_GE(5,1)
        #define WC_SPKRE_F(x,y) wolfCrypt_SetPrivateKeyReadEnable_fips((x),(y))
        #define PRIVATE_KEY_LOCK() WC_SPKRE_F(0,WC_KEYTYPE_ALL)
        #define PRIVATE_KEY_UNLOCK() WC_SPKRE_F(1,WC_KEYTYPE_ALL)
    #else
        #define PRIVATE_KEY_LOCK() WC_DO_NOTHING
        #define PRIVATE_KEY_UNLOCK() WC_DO_NOTHING
    #endif


    #ifdef _MSC_VER
        /* disable buggy MSC warning (incompatible with clang-tidy
         * readability-avoid-const-params-in-decls)
         * "warning C4028: formal parameter x different from declaration"
         */
        #pragma warning(disable: 4028)
    #endif


    /* opaque math variable type */
    #if defined(USE_FAST_MATH)
        struct fp_int;
        #define MATH_INT_T struct fp_int
    #elif defined(USE_INTEGER_HEAP_MATH)
        struct mp_int;
        #define MATH_INT_T struct mp_int
    #else
        struct sp_int;
        #define MATH_INT_T struct sp_int
    #endif


    #ifdef __cplusplus
        }   /* extern "C" */
    #endif

#endif /* WOLF_CRYPT_TYPES_H */
