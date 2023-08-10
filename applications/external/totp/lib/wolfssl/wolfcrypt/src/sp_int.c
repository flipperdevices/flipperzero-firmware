/* sp_int.c
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

/* Implementation by Sean Parkinson. */

/*
DESCRIPTION
This library provides single precision (SP) integer math functions.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL)

#if (!defined(WOLFSSL_SMALL_STACK) && !defined(SP_ALLOC)) || \
    defined(WOLFSSL_SP_NO_MALLOC)
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
#pragma GCC diagnostic push
/* We are statically declaring a variable smaller than sp_int.
 * We track available memory in the 'size' field.
 * Disable warnings of sp_int being partly outside array bounds of variable.
 */
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* SP Build Options:
 * WOLFSSL_HAVE_SP_RSA:         Enable SP RSA support
 * WOLFSSL_HAVE_SP_DH:          Enable SP DH support
 * WOLFSSL_HAVE_SP_ECC:         Enable SP ECC support
 * WOLFSSL_SP_MATH:             Use only single precision math and algorithms
 *      it supports (no fastmath tfm.c or normal integer.c)
 * WOLFSSL_SP_MATH_ALL          Implementation of all MP functions
 *      (replacement for tfm.c and integer.c)
 * WOLFSSL_SP_SMALL:            Use smaller version of code and avoid large
 *      stack variables
 * WOLFSSL_SP_NO_MALLOC:        Always use stack, no heap XMALLOC/XFREE allowed
 * WOLFSSL_SP_NO_2048:          Disable RSA/DH 2048-bit support
 * WOLFSSL_SP_NO_3072:          Disable RSA/DH 3072-bit support
 * WOLFSSL_SP_4096:             Enable RSA/RH 4096-bit support
 * WOLFSSL_SP_NO_256            Disable ECC 256-bit SECP256R1 support
 * WOLFSSL_SP_384               Enable ECC 384-bit SECP384R1 support
 * WOLFSSL_SP_521               Enable ECC 521-bit SECP521R1 support
 * WOLFSSL_SP_ASM               Enable assembly speedups (detect platform)
 * WOLFSSL_SP_X86_64_ASM        Enable Intel x64 assembly implementation
 * WOLFSSL_SP_ARM32_ASM         Enable Aarch32 assembly implementation
 * WOLFSSL_SP_ARM64_ASM         Enable Aarch64 assembly implementation
 * WOLFSSL_SP_ARM_CORTEX_M_ASM  Enable Cortex-M assembly implementation
 * WOLFSSL_SP_ARM_THUMB_ASM     Enable ARM Thumb assembly implementation
 *      (used with -mthumb)
 * WOLFSSL_SP_X86_64            Enable Intel x86 64-bit assembly speedups
 * WOLFSSL_SP_X86               Enable Intel x86 assembly speedups
 * WOLFSSL_SP_ARM64             Enable Aarch64 assembly speedups
 * WOLFSSL_SP_ARM32             Enable ARM32 assembly speedups
 * WOLFSSL_SP_ARM32_UDIV        Enable word divide asm that uses UDIV instr
 * WOLFSSL_SP_ARM_THUMB         Enable ARM Thumb assembly speedups
 *                              (explicitly uses register 'r7')
 * WOLFSSL_SP_PPC64             Enable PPC64 assembly speedups
 * WOLFSSL_SP_PPC               Enable PPC assembly speedups
 * WOLFSSL_SP_MIPS64            Enable MIPS64 assembly speedups
 * WOLFSSL_SP_MIPS              Enable MIPS assembly speedups
 * WOLFSSL_SP_RISCV64           Enable RISCV64 assembly speedups
 * WOLFSSL_SP_RISCV32           Enable RISCV32 assembly speedups
 * WOLFSSL_SP_S390X             Enable S390X assembly speedups
 * SP_WORD_SIZE                 Force 32 or 64 bit mode
 * WOLFSSL_SP_NONBLOCK          Enables "non blocking" mode for SP math, which
 *      will return FP_WOULDBLOCK for long operations and function must be
 *      called again until complete.
 * WOLFSSL_SP_FAST_NCT_EXPTMOD  Enables the faster non-constant time modular
 *      exponentiation implementation.
 * WOLFSSL_SP_INT_NEGATIVE      Enables negative values to be used.
 * WOLFSSL_SP_INT_DIGIT_ALIGN   Enable when unaligned access of sp_int_digit
 *                              pointer is not allowed.
 * WOLFSSL_SP_NO_DYN_STACK      Disable use of dynamic stack items.
 *                              Dynamic arrays used when not small stack.
 * WOLFSSL_SP_FAST_MODEXP       Allow fast mod_exp with small C code
 * WOLFSSL_SP_LOW_MEM           Use algorithms that use less memory.
 */

/* TODO: WOLFSSL_SP_SMALL is incompatible with clang-12+ -Os. */
#if defined(__clang__) && defined(__clang_major__) && \
    (__clang_major__ >= 12) && defined(WOLFSSL_SP_SMALL)
    #undef WOLFSSL_SP_SMALL
#endif

#include <wolfssl/wolfcrypt/sp_int.h>

/* DECL_SP_INT: Declare one variable of type 'sp_int'. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    /* Declare a variable that will be assigned a value on XMALLOC. */
    #define DECL_SP_INT(n, s)   \
        sp_int* n = NULL
#else
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
        !defined(WOLFSSL_SP_NO_DYN_STACK)
        /* Declare a variable on the stack with the required data size. */
        #define DECL_SP_INT(n, s)               \
            byte    n##d[MP_INT_SIZEOF(s)];     \
            sp_int* (n) = (sp_int*)n##d
    #else
        /* Declare a variable on the stack. */
        #define DECL_SP_INT(n, s)               \
            sp_int n[1]
    #endif
#endif

/* ALLOC_SP_INT: Allocate an 'sp_int' of required size. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    /* Dynamically allocate just enough data to support size. */
    #define ALLOC_SP_INT(n, s, err, h)                                         \
    do {                                                                       \
        if (((err) == MP_OKAY) && ((s) > SP_INT_DIGITS)) {                     \
            (err) = MP_VAL;                                                    \
        }                                                                      \
        if ((err) == MP_OKAY) {                                                \
            (n) = (sp_int*)XMALLOC(MP_INT_SIZEOF(s), (h),                      \
                DYNAMIC_TYPE_BIGINT);                                          \
            if ((n) == NULL) {                                                 \
                (err) = MP_MEM;                                                \
            }                                                                  \
        }                                                                      \
    }                                                                          \
    while (0)

    /* Dynamically allocate just enough data to support size - and set size. */
    #define ALLOC_SP_INT_SIZE(n, s, err, h)                                    \
    do {                                                                       \
        ALLOC_SP_INT(n, s, err, h);                                            \
        if ((err) == MP_OKAY) {                                                \
            (n)->size = (s);                                                   \
        }                                                                      \
    }                                                                          \
    while (0)
#else
    /* Array declared on stack - check size is valid. */
    #define ALLOC_SP_INT(n, s, err, h)                                         \
    do {                                                                       \
        if (((err) == MP_OKAY) && ((s) > SP_INT_DIGITS)) {                     \
            (err) = MP_VAL;                                                    \
        }                                                                      \
    }                                                                          \
    while (0)

    /* Array declared on stack - set the size field. */
    #define ALLOC_SP_INT_SIZE(n, s, err, h)                                    \
    do {                                                                       \
        ALLOC_SP_INT(n, s, err, h);                                            \
        if ((err) == MP_OKAY) {                                                \
            (n)->size = (unsigned int)(s);                                     \
        }                                                                      \
    }                                                                          \
    while (0)
#endif

/* FREE_SP_INT: Free an 'sp_int' variable. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    /* Free dynamically allocated data. */
    #define FREE_SP_INT(n, h)                   \
    do {                                        \
        if ((n) != NULL) {                      \
            XFREE(n, h, DYNAMIC_TYPE_BIGINT);   \
        }                                       \
    }                                           \
    while (0)
#else
    /* Nothing to do as declared on stack. */
    #define FREE_SP_INT(n, h) WC_DO_NOTHING
#endif


/* Declare a variable that will be assigned a value on XMALLOC. */
#define DECL_DYN_SP_INT_ARRAY(n, s, c)  \
    sp_int* n##d = NULL;            \
    sp_int* (n)[c] = { NULL, }

/* DECL_SP_INT_ARRAY: Declare array of 'sp_int'. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    /* Declare a variable that will be assigned a value on XMALLOC. */
    #define DECL_SP_INT_ARRAY(n, s, c)  \
        DECL_DYN_SP_INT_ARRAY(n, s, c)
#else
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
        !defined(WOLFSSL_SP_NO_DYN_STACK)
        /* Declare a variable on the stack with the required data size. */
        #define DECL_SP_INT_ARRAY(n, s, c)          \
            byte    n##d[MP_INT_SIZEOF(s) * (c)];   \
            sp_int* (n)[c] = { NULL, }
    #else
        /* Declare a variable on the stack. */
        #define DECL_SP_INT_ARRAY(n, s, c)      \
            sp_int n##d[c];                     \
            sp_int* (n)[c]
    #endif
#endif

/* Dynamically allocate just enough data to support multiple sp_ints of the
 * required size. Use pointers into data to make up array and set sizes.
 */
#define ALLOC_DYN_SP_INT_ARRAY(n, s, c, err, h)                                \
do {                                                                           \
    if (((err) == MP_OKAY) && ((s) > SP_INT_DIGITS)) {                         \
        (err) = MP_VAL;                                                        \
    }                                                                          \
    if ((err) == MP_OKAY) {                                                    \
        n##d = (sp_int*)XMALLOC(MP_INT_SIZEOF(s) * (c), (h),                   \
                                                         DYNAMIC_TYPE_BIGINT); \
        if (n##d == NULL) {                                                    \
            (err) = MP_MEM;                                                    \
        }                                                                      \
        else {                                                                 \
            int n##ii;                                                         \
            (n)[0] = n##d;                                                     \
            (n)[0]->size = (s);                                                \
            for (n##ii = 1; n##ii < (int)(c); n##ii++) {                       \
                (n)[n##ii] = MP_INT_NEXT((n)[n##ii-1], s);                     \
                (n)[n##ii]->size = (s);                                        \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}                                                                              \
while (0)

/* ALLOC_SP_INT_ARRAY: Allocate an array of 'sp_int's of required size. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    #define ALLOC_SP_INT_ARRAY(n, s, c, err, h) \
        ALLOC_DYN_SP_INT_ARRAY(n, s, c, err, h)
#else
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
        !defined(WOLFSSL_SP_NO_DYN_STACK)
        /* Data declared on stack that supports multiple sp_ints of the
         * required size. Use pointers into data to make up array and set sizes.
         */
        #define ALLOC_SP_INT_ARRAY(n, s, c, err, h)                            \
        do {                                                                   \
            if (((err) == MP_OKAY) && ((s) > SP_INT_DIGITS)) {                 \
                (err) = MP_VAL;                                                \
            }                                                                  \
            if ((err) == MP_OKAY) {                                            \
                int n##ii;                                                     \
                (n)[0] = (sp_int*)n##d;                                        \
                ((sp_int_minimal*)(n)[0])->size = (s);                         \
                for (n##ii = 1; n##ii < (int)(c); n##ii++) {                   \
                    (n)[n##ii] = MP_INT_NEXT((n)[n##ii-1], s);                 \
                    ((sp_int_minimal*)(n)[n##ii])->size = (s);                 \
                }                                                              \
            }                                                                  \
        }                                                                      \
        while (0)
    #else
        /* Data declared on stack that supports multiple sp_ints of the
         * required size. Set into array and set sizes.
         */
        #define ALLOC_SP_INT_ARRAY(n, s, c, err, h)                            \
        do {                                                                   \
            if (((err) == MP_OKAY) && ((s) > SP_INT_DIGITS)) {                 \
                (err) = MP_VAL;                                                \
            }                                                                  \
            if ((err) == MP_OKAY) {                                            \
                int n##ii;                                                     \
                for (n##ii = 0; n##ii < (int)(c); n##ii++) {                   \
                    (n)[n##ii] = &n##d[n##ii];                                 \
                    (n)[n##ii]->size = (s);                                    \
                }                                                              \
            }                                                                  \
        }                                                                      \
        while (0)
    #endif
#endif

/* Free data variable that was dynamically allocated. */
#define FREE_DYN_SP_INT_ARRAY(n, h)             \
do {                                            \
    if (n##d != NULL) {                         \
        XFREE(n##d, h, DYNAMIC_TYPE_BIGINT);    \
    }                                           \
}                                               \
while (0)

/* FREE_SP_INT_ARRAY: Free an array of 'sp_int'. */
#if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
    !defined(WOLFSSL_SP_NO_MALLOC)
    #define FREE_SP_INT_ARRAY(n, h)                 \
        FREE_DYN_SP_INT_ARRAY(n, h)
#else
    /* Nothing to do as data declared on stack. */
    #define FREE_SP_INT_ARRAY(n, h) WC_DO_NOTHING
#endif


#ifndef WOLFSSL_NO_ASM
    #ifdef __IAR_SYSTEMS_ICC__
        #define __asm__        asm
        #define __volatile__   volatile
    #endif /* __IAR_SYSTEMS_ICC__ */
    #ifdef __KEIL__
        #define __asm__        __asm
        #define __volatile__   volatile
    #endif

    #if defined(WOLFSSL_SP_X86_64) && SP_WORD_SIZE == 64
/*
 * CPU: x86_64
 */

#ifndef _MSC_VER
/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "movq	%%rax, %[l]	\n\t"                    \
        "movq	%%rdx, %[h]	\n\t"                    \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "memory", "%rax", "%rdx", "cc"                 \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "movq	$0   , %[o]	\n\t"                    \
        "movq	%%rax, %[l]	\n\t"                    \
        "movq	%%rdx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "%rax", "%rdx", "cc"                           \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "adcq	$0   , %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "%rax", "%rdx", "cc"                           \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "%rax", "%rdx", "cc"                           \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "adcq	$0   , %[o]	\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "adcq	$0   , %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "%rax", "%rdx", "cc"                           \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "movq	%[b], %%rax	\n\t"                    \
        "mulq	%[a]		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "adcq	$0   , %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "%rax", "%rdx", "cc"                           \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "movq	%[a], %%rax	\n\t"                    \
        "mulq	%%rax		\n\t"                    \
        "movq	%%rax, %[l]	\n\t"                    \
        "movq	%%rdx, %[h]	\n\t"                    \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "m" (va)                                   \
        : "memory", "%rax", "%rdx", "cc"                 \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "movq	%[a], %%rax	\n\t"                    \
        "mulq	%%rax		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        "adcq	$0   , %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "m" (va)                                   \
        : "%rax", "%rdx", "cc"                           \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "movq	%[a], %%rax	\n\t"                    \
        "mulq	%%rax		\n\t"                    \
        "addq	%%rax, %[l]	\n\t"                    \
        "adcq	%%rdx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "%rax", "%rdx", "cc"                           \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "addq	%[a], %[l]	\n\t"                    \
        "adcq	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "cc"                                           \
    )
/* Add va, variable in a register, into: vh | vl */
#define SP_ASM_ADDC_REG(vl, vh, va)                      \
    __asm__ __volatile__ (                               \
        "addq	%[a], %[l]	\n\t"                    \
        "adcq	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subq	%[a], %[l]	\n\t"                    \
        "sbbq	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB_REG(vl, vh, va)                      \
    __asm__ __volatile__ (                               \
        "subq	%[a], %[l]	\n\t"                    \
        "sbbq	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "addq	%[a], %[l]	\n\t"                    \
        "adcq	%[b], %[h]	\n\t"                    \
        "adcq	%[c], %[o]	\n\t"                    \
        "addq	%[a], %[l]	\n\t"                    \
        "adcq	%[b], %[h]	\n\t"                    \
        "adcq	%[c], %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
/* Index of highest bit set. */
#define SP_ASM_HI_BIT_SET_IDX(va, vi)                    \
    __asm__ __volatile__ (                               \
        "bsr	%[a], %[i]	\n\t"                    \
        : [i] "=r" (vi)                                  \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
#else
#include <intrin.h>

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    vl = _umul128(va, vb, &vh)

/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    do {                                                 \
        vl = _umul128(va, vb, &vh);                      \
        vo = 0;                                          \
    }                                                    \
    while (0)

/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, vb, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
        c = _addcarry_u64(c, vh, vth, &vh);              \
            _addcarry_u64(c, vo,   0, &vo);              \
    }                                                    \
    while (0)

/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, vb, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
            _addcarry_u64(c, vh, vth, &vh);              \
    }                                                    \
    while (0)

/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, vb, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
        c = _addcarry_u64(c, vh, vth, &vh);              \
            _addcarry_u64(c, vo,   0, &vo);              \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
        c = _addcarry_u64(c, vh, vth, &vh);              \
            _addcarry_u64(c, vo,   0, &vo);              \
    }                                                    \
    while (0)
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, vb, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
            _addcarry_u64(c, vh, vth, &vh);              \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
        c = _addcarry_u64(c, vh, vth, &vh);              \
            _addcarry_u64(c, vo,   0, &vo);              \
    }                                                    \
    while (0)

 /* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    vl = _umul128(va, va, &vh)

/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, va, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
        c = _addcarry_u64(c, vh, vth, &vh);              \
            _addcarry_u64(c, vo,   0, &vo);              \
    }                                                    \
    while (0)

/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    do {                                                 \
        unsigned __int64 vtl, vth;                       \
        unsigned char c;                                 \
        vtl = _umul128(va, va, &vth);                    \
        c = _addcarry_u64(0, vl, vtl, &vl);              \
            _addcarry_u64(c, vh, vth, &vh);              \
    }                                                    \
    while (0)

/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    do {                                                 \
        unsigned char c;                                 \
        c = _addcarry_u64(0, vl, va, &vl);               \
            _addcarry_u64(c, vh,  0, &vh);               \
    }                                                    \
    while (0)

/* Add va, variable in a register, into: vh | vl */
#define SP_ASM_ADDC_REG(vl, vh, va)                      \
    do {                                                 \
        unsigned char c;                                 \
        c = _addcarry_u64(0, vl, va, &vl);               \
            _addcarry_u64(c, vh,  0, &vh);               \
    }                                                    \
    while (0)

/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    do {                                                 \
        unsigned char c;                                 \
        c = _subborrow_u64(0, vl, va, &vl);              \
            _subborrow_u64(c, vh,  0, &vh);              \
    }                                                    \
    while (0)

/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    do {                                                 \
        unsigned char c;                                 \
        c = _addcarry_u64(0, vl, va, &vl);               \
        c = _addcarry_u64(c, vh, vb, &vh);               \
            _addcarry_u64(c, vo, vc, &vo);               \
        c = _addcarry_u64(0, vl, va, &vl);               \
        c = _addcarry_u64(c, vh, vb, &vh);               \
            _addcarry_u64(c, vo, vc, &vo);               \
    }                                                    \
    while (0)
/* Index of highest bit set. */
#define SP_ASM_HI_BIT_SET_IDX(va, vi)                    \
    do {                                                 \
        unsigned long idx;                               \
        _BitScanReverse64(&idx, va);                     \
        vi = idx;                                        \
    }                                                    \
    while (0)
#endif

#if !defined(WOLFSSL_SP_DIV_WORD_HALF) && (!defined(_MSC_VER) || \
    _MSC_VER >= 1920)
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * Using divq instruction on Intel x64.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
#ifndef _MSC_VER
    __asm__ __volatile__ (
        "divq %2"
        : "+a" (lo)
        : "d" (hi), "r" (d)
        : "cc"
    );
    return lo;
#elif defined(_MSC_VER) && _MSC_VER >= 1920
    return _udiv128(hi, lo, d, NULL);
#endif
}
#define SP_ASM_DIV_WORD
#endif

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_X86_64 && SP_WORD_SIZE == 64 */

    #if defined(WOLFSSL_SP_X86) && SP_WORD_SIZE == 32
/*
 * CPU: x86
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "movl	%%eax, %[l]	\n\t"                    \
        "movl	%%edx, %[h]	\n\t"                    \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "memory", "eax", "edx", "cc"                   \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "movl	$0   , %[o]	\n\t"                    \
        "movl	%%eax, %[l]	\n\t"                    \
        "movl	%%edx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "eax", "edx", "cc"                             \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "adcl	$0   , %[o]	\n\t"                    \
        : [l] "+rm" (vl), [h] "+rm" (vh), [o] "+rm" (vo) \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "eax", "edx", "cc"                             \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "eax", "edx", "cc"                             \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "adcl	$0   , %[o]	\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "adcl	$0   , %[o]	\n\t"                    \
        : [l] "+rm" (vl), [h] "+rm" (vh), [o] "+rm" (vo) \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "eax", "edx", "cc"                             \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "movl	%[b], %%eax	\n\t"                    \
        "mull	%[a]		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "adcl	$0   , %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "m" (va), [b] "m" (vb)                     \
        : "eax", "edx", "cc"                             \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "movl	%[a], %%eax	\n\t"                    \
        "mull	%%eax		\n\t"                    \
        "movl	%%eax, %[l]	\n\t"                    \
        "movl	%%edx, %[h]	\n\t"                    \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "m" (va)                                   \
        : "memory", "eax", "edx", "cc"                   \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "movl	%[a], %%eax	\n\t"                    \
        "mull	%%eax		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        "adcl	$0   , %[o]	\n\t"                    \
        : [l] "+rm" (vl), [h] "+rm" (vh), [o] "+rm" (vo) \
        : [a] "m" (va)                                   \
        : "eax", "edx", "cc"                             \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "movl	%[a], %%eax	\n\t"                    \
        "mull	%%eax		\n\t"                    \
        "addl	%%eax, %[l]	\n\t"                    \
        "adcl	%%edx, %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "eax", "edx", "cc"                             \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "addl	%[a], %[l]	\n\t"                    \
        "adcl	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "cc"                                           \
    )
/* Add va, variable in a register, into: vh | vl */
#define SP_ASM_ADDC_REG(vl, vh, va)                      \
    __asm__ __volatile__ (                               \
        "addl	%[a], %[l]	\n\t"                    \
        "adcl	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subl	%[a], %[l]	\n\t"                    \
        "sbbl	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "m" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB_REG(vl, vh, va)                      \
    __asm__ __volatile__ (                               \
        "subl	%[a], %[l]	\n\t"                    \
        "sbbl	$0  , %[h]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "addl	%[a], %[l]	\n\t"                    \
        "adcl	%[b], %[h]	\n\t"                    \
        "adcl	%[c], %[o]	\n\t"                    \
        "addl	%[a], %[l]	\n\t"                    \
        "adcl	%[b], %[h]	\n\t"                    \
        "adcl	%[c], %[o]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
/* Index of highest bit set. */
#define SP_ASM_HI_BIT_SET_IDX(va, vi)                    \
    __asm__ __volatile__ (                               \
        "bsr	%[a], %[i]	\n\t"                    \
        : [i] "=r" (vi)                                  \
        : [a] "r" (va)                                   \
        : "cC"                                           \
    )

#ifndef WOLFSSL_SP_DIV_WORD_HALF
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * Using divl instruction on Intel x64.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
    __asm__ __volatile__ (
        "divl %2"
        : "+a" (lo)
        : "d" (hi), "r" (d)
        : "cc"
    );
    return lo;
}
#define SP_ASM_DIV_WORD
#endif

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_X86 && SP_WORD_SIZE == 32 */

    #if defined(WOLFSSL_SP_ARM64) && SP_WORD_SIZE == 64
/*
 * CPU: Aarch64
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[b]	\n\t"            \
        "umulh	%[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory", "cc"                                 \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[b]		\n\t"            \
        "umulh	%[h], %[a], %[b]	\n\t"            \
        "mov	%[l], x8		\n\t"            \
        "mov	%[o], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "x8"                                           \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[b]		\n\t"            \
        "umulh	x9, %[a], %[b]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adcs	%[h], %[h], x9		\n\t"            \
        "adc	%[o], %[o], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "x8", "x9", "cc"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[b]		\n\t"            \
        "umulh	x9, %[a], %[b]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adc	%[h], %[h], x9		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "x8", "x9", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[b]		\n\t"            \
        "umulh	x9, %[a], %[b]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adcs	%[h], %[h], x9		\n\t"            \
        "adc	%[o], %[o], xzr		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adcs	%[h], %[h], x9		\n\t"            \
        "adc	%[o], %[o], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "x8", "x9", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[b]		\n\t"            \
        "umulh	x9, %[a], %[b]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adc	%[h], %[h], x9		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adcs	%[h], %[h], x9		\n\t"            \
        "adc	%[o], %[o], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "x8", "x9", "cc"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[a]	\n\t"            \
        "umulh	%[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[a]		\n\t"            \
        "umulh	x9, %[a], %[a]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adcs	%[h], %[h], x9		\n\t"            \
        "adc	%[o], %[o], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "x8", "x9", "cc"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "mul	x8, %[a], %[a]		\n\t"            \
        "umulh	x9, %[a], %[a]		\n\t"            \
        "adds	%[l], %[l], x8		\n\t"            \
        "adc	%[h], %[h], x9		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "x8", "x9", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adc	%[h], %[h], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subs	%[l], %[l], %[a]	\n\t"            \
        "sbc	%[h], %[h], xzr		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adc	%[o], %[o], %[c]	\n\t"            \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adc	%[o], %[o], %[c]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
/* Count leading zeros. */
#define SP_ASM_LZCNT(va, vn)                             \
    __asm__ __volatile__ (                               \
        "clz	%[n], %[a]	\n\t"                    \
        : [n] "=r" (vn)                                  \
        : [a] "r" (va)                                   \
        :                                                \
    )

#ifndef WOLFSSL_SP_DIV_WORD_HALF
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * Using udiv instruction on Aarch64.
 * Constant time.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
    __asm__ __volatile__ (
        "lsr	x3, %[d], 48\n\t"
        "mov	x5, 16\n\t"
        "cmp	x3, 0\n\t"
        "mov	x4, 63\n\t"
        "csel	x3, x5, xzr, eq\n\t"
        "sub	x4, x4, x3\n\t"
        "lsl	%[d], %[d], x3\n\t"
        "lsl	%[hi], %[hi], x3\n\t"
        "lsr	x5, %[lo], x4\n\t"
        "lsl	%[lo], %[lo], x3\n\t"
        "orr	%[hi], %[hi], x5, lsr 1\n\t"

        "lsr	x5, %[d], 32\n\t"
        "add	x5, x5, 1\n\t"

        "udiv	x3, %[hi], x5\n\t"
        "lsl	x6, x3, 32\n\t"
        "mul	x4, %[d], x6\n\t"
        "umulh	x3, %[d], x6\n\t"
        "subs	%[lo], %[lo], x4\n\t"
        "sbc	%[hi], %[hi], x3\n\t"

        "udiv	x3, %[hi], x5\n\t"
        "lsl	x3, x3, 32\n\t"
        "add	x6, x6, x3\n\t"
        "mul	x4, %[d], x3\n\t"
        "umulh	x3, %[d], x3\n\t"
        "subs	%[lo], %[lo], x4\n\t"
        "sbc	%[hi], %[hi], x3\n\t"

        "lsr	x3, %[lo], 32\n\t"
        "orr	x3, x3, %[hi], lsl 32\n\t"

        "udiv	x3, x3, x5\n\t"
        "add	x6, x6, x3\n\t"
        "mul	x4, %[d], x3\n\t"
        "umulh	x3, %[d], x3\n\t"
        "subs	%[lo], %[lo], x4\n\t"
        "sbc	%[hi], %[hi], x3\n\t"

        "lsr	x3, %[lo], 32\n\t"
        "orr	x3, x3, %[hi], lsl 32\n\t"

        "udiv	x3, x3, x5\n\t"
        "add	x6, x6, x3\n\t"
        "mul	x4, %[d], x3\n\t"
        "sub	%[lo], %[lo], x4\n\t"

        "udiv	x3, %[lo], %[d]\n\t"
        "add	%[hi], x6, x3\n\t"

        : [hi] "+r" (hi), [lo] "+r" (lo), [d] "+r" (d)
        :
        : "x3", "x4", "x5", "x6", "cc"
    );

    return hi;
}
#define SP_ASM_DIV_WORD
#endif

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_ARM64 && SP_WORD_SIZE == 64 */

    #if (defined(WOLFSSL_SP_ARM32) || defined(WOLFSSL_SP_ARM_CORTEX_M)) && \
        SP_WORD_SIZE == 32
/*
 * CPU: ARM32 or Cortex-M4 and similar
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "umull	%[l], %[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory"                                       \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "umull	%[l], %[h], %[a], %[b]	\n\t"            \
        "mov	%[o], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        :                                                \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "umull	r8, r9, %[a], %[b]	\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adcs	%[h], %[h], r9		\n\t"            \
        "adc	%[o], %[o], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r8", "r9", "cc"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "umlal	%[l], %[h], %[a], %[b]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        :                                                \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "umull	r8, r9, %[a], %[b]	\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adcs	%[h], %[h], r9		\n\t"            \
        "adc	%[o], %[o], #0		\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adcs	%[h], %[h], r9		\n\t"            \
        "adc	%[o], %[o], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r8", "r9", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "umull	r8, r9, %[a], %[b]	\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adc	%[h], %[h], r9		\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adcs	%[h], %[h], r9		\n\t"            \
        "adc	%[o], %[o], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r8", "r9", "cc"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "umull	%[l], %[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "umull	r8, r9, %[a], %[a]	\n\t"            \
        "adds	%[l], %[l], r8		\n\t"            \
        "adcs	%[h], %[h], r9		\n\t"            \
        "adc	%[o], %[o], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "r8", "r9", "cc"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "umlal	%[l], %[h], %[a], %[a]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adc	%[h], %[h], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subs	%[l], %[l], %[a]	\n\t"            \
        "sbc	%[h], %[h], #0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adc	%[o], %[o], %[c]	\n\t"            \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adc	%[o], %[o], %[c]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH >= 7)
/* Count leading zeros - instruction only available on ARMv7 and newer. */
#define SP_ASM_LZCNT(va, vn)                             \
    __asm__ __volatile__ (                               \
        "clz	%[n], %[a]	\n\t"                    \
        : [n] "=r" (vn)                                  \
        : [a] "r" (va)                                   \
        :                                                \
    )
#endif

#ifndef WOLFSSL_SP_DIV_WORD_HALF
#ifndef WOLFSSL_SP_ARM32_UDIV
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * No division instruction used - does operation bit by bit.
 * Constant time.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
    sp_int_digit r = 0;
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
    static const char debruijn32[32] = {
        0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
        1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
    };
    static const sp_uint32 debruijn32_mul = 0x076be629;
#endif

    __asm__ __volatile__ (
        /* Shift d so that top bit is set. */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, %[m]\n\t"
        "mov	r5, %[d]\n\t"
        "orr	r5, r5, r5, lsr #1\n\t"
        "orr	r5, r5, r5, lsr #2\n\t"
        "orr	r5, r5, r5, lsr #4\n\t"
        "orr	r5, r5, r5, lsr #8\n\t"
        "orr	r5, r5, r5, lsr #16\n\t"
        "add	r5, r5, #1\n\t"
        "mul	r5, r5, r4\n\t"
        "lsr	r5, r5, #27\n\t"
        "ldrb	r5, [%[t], r5]\n\t"
#else
        "clz	r5, %[d]\n\t"
#endif
        "rsb	r6, r5, #31\n\t"
        "lsl	%[d], %[d], r5\n\t"
        "lsl	%[hi], %[hi], r5\n\t"
        "lsr	r9, %[lo], r6\n\t"
        "lsl	%[lo], %[lo], r5\n\t"
        "orr	%[hi], %[hi], r9, lsr #1\n\t"

        "lsr	r5, %[d], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[lo]\n\t"
        "mov	r9, %[hi]\n\t"
        /* Do top 32 */
        "subs	r8, r5, r9\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r9, r9, r8\n\t"
        /* Next 30 bits */
        "mov	r4, #29\n\t"
        "\n1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r9, r9, r9\n\t"
        "subs	r8, r5, r9\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r9, r9, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"

        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"

        /* Handle difference has hi word > 0. */
        "umull	r4, r5, %[r], %[d]\n\t"
        "subs	r4, %[lo], r4\n\t"
        "sbc	r5, %[hi], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[d]\n\t"
        "subs	r4, %[lo], r4\n\t"
        "sbc	r5, %[hi], r5\n\t"
        "add	%[r], %[r], r5\n\t"

        /* Add 1 to result if bottom half of difference is >= d. */
        "mul	r4, %[r], %[d]\n\t"
        "subs	r4, %[lo], r4\n\t"
        "subs	r9, %[d], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        "subs	r9, r9, #1\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r), [hi] "+r" (hi), [lo] "+r" (lo), [d] "+r" (d)
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        : [t] "r" (debruijn32), [m] "m" (debruijn32_mul)
#else
        :
#endif
        : "r4", "r5", "r6", "r8", "r9", "cc"
    );

    return r;
}
#else
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * Using udiv instruction on arm32
 * Constant time.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
    __asm__ __volatile__ (
        "lsrs	r3, %[d], #24\n\t"
	"it	eq\n\t"
        "moveq	r3, #8\n\t"
	"it	ne\n\t"
        "movne	r3, #0\n\t"
        "rsb	r4, r3, #31\n\t"
        "lsl	%[d], %[d], r3\n\t"
        "lsl	%[hi], %[hi], r3\n\t"
        "lsr	r5, %[lo], r4\n\t"
        "lsl	%[lo], %[lo], r3\n\t"
        "orr	%[hi], %[hi], r5, lsr #1\n\t"

        "lsr	r5, %[d], 16\n\t"
        "add	r5, r5, 1\n\t"

        "udiv	r3, %[hi], r5\n\t"
        "lsl	r6, r3, 16\n\t"
        "umull	r4, r3, %[d], r6\n\t"
        "subs	%[lo], %[lo], r4\n\t"
        "sbc	%[hi], %[hi], r3\n\t"

        "udiv	r3, %[hi], r5\n\t"
        "lsl	r3, r3, 16\n\t"
        "add	r6, r6, r3\n\t"
        "umull	r4, r3, %[d], r3\n\t"
        "subs	%[lo], %[lo], r4\n\t"
        "sbc	%[hi], %[hi], r3\n\t"

        "lsr	r3, %[lo], 16\n\t"
        "orr	r3, r3, %[hi], lsl 16\n\t"

        "udiv	r3, r3, r5\n\t"
        "add	r6, r6, r3\n\t"
        "umull	r4, r3, %[d], r3\n\t"
        "subs	%[lo], %[lo], r4\n\t"
        "sbc	%[hi], %[hi], r3\n\t"

        "lsr	r3, %[lo], 16\n\t"
        "orr	r3, r3, %[hi], lsl 16\n\t"

        "udiv	r3, r3, r5\n\t"
        "add	r6, r6, r3\n\t"
        "mul	r4, %[d], r3\n\t"
        "sub	%[lo], %[lo], r4\n\t"

        "udiv	r3, %[lo], %[d]\n\t"
        "add	%[hi], r6, r3\n\t"

        : [hi] "+r" (hi), [lo] "+r" (lo), [d] "+r" (d)
        :
        : "r3", "r4", "r5", "r6", "cc"
    );

    return hi;
}
#endif

#define SP_ASM_DIV_WORD
#endif

#define SP_INT_ASM_AVAILABLE

    #endif /* (WOLFSSL_SP_ARM32 || ARM_CORTEX_M) && SP_WORD_SIZE == 32 */

    #if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
/*
 * CPU: ARM Thumb (like Cortex-M0)
 */

/* Compile with -fomit-frame-pointer, or similar, if compiler complains about
 * usage of register 'r7'.
 */

#if defined(__clang__)

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "muls	%[l], r6		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r6, r4			\n\t"            \
        "lsrs	%[h], r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "muls	r4, r6			\n\t"            \
        "adds	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r6, r4			\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r4		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "muls	%[l], r6		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	%[h], r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	%[o], #0		\n\t"            \
        "adcs	%[h], %[o]		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#endif
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r4, r6			\n\t"            \
        "adds	%[l], %[l], r4		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r6, r4			\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r4		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r4, r6			\n\t"            \
        "adds	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r6, r4			\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r4		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "movs	r8, %[a]		\n\t"            \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[h], %[a]		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[a]		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        /* ah * bh */                                    \
        "movs	%[a], r8		\n\t"            \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "movs	%[a], r8		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
#ifndef DEBUG
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6			\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7			\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r7		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "movs	r8, %[a]		\n\t"            \
        /* al * bl */                                    \
        "uxth	r5, %[a]		\n\t"            \
        "uxth	r6, %[b]		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[h], %[a]		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[a]		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r6, %[b], #16		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "lsrs	r6, r5, #16		\n\t"            \
        "lsls	r5, r5, #16		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        /* ah * bh */                                    \
        "movs	%[a], r8		\n\t"            \
        "lsrs	r5, %[a], #16		\n\t"            \
        "lsrs	r6, %[b], #16		\n\t"            \
        "muls	r6, r5			\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adds	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r6, %[b]		\n\t"            \
        "muls	r5, r6			\n\t"            \
        "lsrs	r6, r5, #16		\n\t"            \
        "lsls	r5, r5, #16		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        "adcs	%[o], %[a]		\n\t"            \
        "movs	%[a], r8		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "lsrs	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        "mov	%[l], r6		\n\t"            \
        "mov	%[h], r5		\n\t"            \
        /* al * al */                                    \
        "muls	%[l], %[l]		\n\t"            \
        /* ah * ah */                                    \
        "muls	%[h], %[h]		\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r5			\n\t"            \
        "lsrs	r5, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "lsrs	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "muls	r6, r6			\n\t"            \
        /* ah * ah */                                    \
        "muls	r4, r4			\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r4		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        "lsrs	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r4			\n\t"            \
        "lsrs	r4, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r4		\n\t"            \
        "adcs	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va)                                   \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "lsrs	r6, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "muls	r6, r6			\n\t"            \
        /* ah * ah */                                    \
        "muls	r6, r6			\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        "lsrs	r6, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r6			\n\t"            \
        "lsrs	r6, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], r6		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subs	%[l], %[l], %[a]	\n\t"            \
        "movs	r5, #0			\n\t"            \
        "sbcs	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[b]		\n\t"            \
        "adcs	%[o], %[c]		\n\t"            \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[b]		\n\t"            \
        "adcs	%[o], %[c]		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb), [c] "l" (vc)       \
        : "cc"                                           \
    )

#elif defined(WOLFSSL_KEIL)

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "muls	%[l], r6, %[l]		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r6, r4, r6		\n\t"            \
        "lsrs	%[h], r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "muls	r4, r6, r4		\n\t"            \
        "adds	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r6, r4, r6		\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r4		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "muls	%[l], r6, %[l]		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	%[h], r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	%[o], #0		\n\t"            \
        "adcs	%[h], %[h], %[o]	\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "muls	r5, r6, r5		\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth   r6, %[a]                \n\t"            \
        "uxth   r5, %[b]                \n\t"            \
        "muls   r5, r6, r5              \n\t"            \
        "adds   %[l], %[l], r5          \n\t"            \
        "movs   r5, #0                  \n\t"            \
        "adcs   %[h], %[h], r5          \n\t"            \
        "adcs   %[o], %[o], r5          \n\t"            \
        /* al * bh */                                    \
        "lsrs   r5, %[b], #16           \n\t"            \
        "muls   r6, r5, r6              \n\t"            \
        "lsrs   r5, r6, #16             \n\t"            \
        "lsls   r6, r6, #16             \n\t"            \
        "adds   %[l], %[l], r6          \n\t"            \
        "adcs   %[h], %[h], r5          \n\t"            \
        "movs   r5, #0                  \n\t"            \
        "adcs   %[o], %[o], r5          \n\t"            \
        /* ah * bh */                                    \
        "lsrs   r6, %[a], #16           \n\t"            \
        "lsrs   r5, %[b], #16           \n\t"            \
        "muls   r5, r6, r5              \n\t"            \
        "adds   %[h], %[h], r5          \n\t"            \
        "movs   r5, #0                  \n\t"            \
        "adcs   %[o], %[o], r5          \n\t"            \
        /* ah * bl */                                    \
        "uxth   r5, %[b]                \n\t"            \
        "muls   r6, r5, r6              \n\t"            \
        "lsrs   r5, r6, #16             \n\t"            \
        "lsls   r6, r6, #16             \n\t"            \
        "adds   %[l], %[l], r6          \n\t"            \
        "adcs   %[h], %[h], r5          \n\t"            \
        "movs   r5, #0                  \n\t"            \
        "adcs   %[o], %[o], r5          \n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#endif
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r4, r6, r4		\n\t"            \
        "adds	%[l], %[l], r4		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r6, r4, r6		\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r4		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r4, %[b], #16		\n\t"            \
        "muls	r4, r6, r4		\n\t"            \
        "adds	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "muls	r6, r4, r6		\n\t"            \
        "lsrs	r4, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r4		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "movs	r8, %[a]		\n\t"            \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r5, r6, r5		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[h], %[h], %[a]	\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[h], %[a]	\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        /* al * bh */                                    \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        /* ah * bh */                                    \
        "movs	%[a], r8		\n\t"            \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r5, %[b], #16		\n\t"            \
        "muls	r5, r6, r5		\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	r5, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "movs	%[a], r8		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
#ifndef DEBUG
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "adds	%[l], %[l], r7		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsrs	r6, %[a], #16		\n\t"            \
        "lsrs	r7, %[b], #16		\n\t"            \
        "muls	r7, r6, r7		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "muls	r6, r7, r6		\n\t"            \
        "lsrs	r7, r6, #16		\n\t"            \
        "lsls	r6, r6, #16		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r7		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "movs	r8, %[a]		\n\t"            \
        /* al * bl */                                    \
        "uxth	r5, %[a]		\n\t"            \
        "uxth	r6, %[b]		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adcs	%[h], %[h], %[a]	\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], %[a]	\n\t"            \
        /* al * bh */                                    \
        "lsrs	r6, %[b], #16		\n\t"            \
        "muls	r5, r6, r5		\n\t"            \
        "lsrs	r6, r5, #16		\n\t"            \
        "lsls	r5, r5, #16		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[h], r6		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        /* ah * bh */                                    \
        "movs	%[a], r8		\n\t"            \
        "lsrs	r5, %[a], #16		\n\t"            \
        "lsrs	r6, %[b], #16		\n\t"            \
        "muls	r6, r5, r6		\n\t"            \
        "movs	%[a], #0		\n\t"            \
        "adds	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        /* ah * bl */                                    \
        "uxth	r6, %[b]		\n\t"            \
        "muls	r5, r6, r5		\n\t"            \
        "lsrs	r6, r5, #16		\n\t"            \
        "lsls	r5, r5, #16		\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "adds	%[l], %[l], r5		\n\t"            \
        "adcs	%[h], %[h], r6		\n\t"            \
        "adcs	%[o], %[o], %[a]	\n\t"            \
        "movs	%[a], r8		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "lsrs	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        "mov	%[l], r6		\n\t"            \
        "mov	%[h], r5		\n\t"            \
        /* al * al */                                    \
        "muls	%[l], %[l], %[l]	\n\t"            \
        /* ah * ah */                                    \
        "muls	%[h], %[h], %[h]	\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	r5, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "lsrs	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "muls	r6, r6, r6		\n\t"            \
        /* ah * ah */                                    \
        "muls	r4, r4, r4		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r4		\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        "lsrs	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r4, r6		\n\t"            \
        "lsrs	r4, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r4		\n\t"            \
        "adcs	%[o], %[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va)                                   \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "lsrs	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "muls	r6, r6, r6		\n\t"            \
        /* ah * ah */                                    \
        "muls	r5, r5, r5		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        "lsrs	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "muls	r6, r5, r6		\n\t"            \
        "lsrs	r5, r6, #15		\n\t"            \
        "lsls	r6, r6, #17		\n\t"            \
        "adds	%[l], %[l], r6		\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "movs	r5, #0			\n\t"            \
        "adcs	%[h], %[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subs	%[l], %[l], %[a]	\n\t"            \
        "movs	r5, #0			\n\t"            \
        "sbcs	%[h], %[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adcs	%[o], %[o], %[c]	\n\t"            \
        "adds	%[l], %[l], %[a]	\n\t"            \
        "adcs	%[h], %[h], %[b]	\n\t"            \
        "adcs	%[o], %[o], %[c]	\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb), [c] "l" (vc)       \
        : "cc"                                           \
    )

#elif defined(__GNUC__)

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "mul	%[l], r6		\n\t"            \
        /* al * bh */                                    \
        "lsr	r4, %[b], #16		\n\t"            \
        "mul	r6, r4			\n\t"            \
        "lsr	%[h], r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "mul	r4, r6			\n\t"            \
        "add	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "mul	r6, r4			\n\t"            \
        "lsr	r4, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r4		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	%[l], %[b]		\n\t"            \
        "mul	%[l], r6		\n\t"            \
        /* al * bh */                                    \
        "lsr	r5, %[b], #16		\n\t"            \
        "mul	r6, r5			\n\t"            \
        "lsr	%[h], r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "mov	%[o], #0		\n\t"            \
        "adc	%[h], %[o]		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "mul	r5, r6			\n\t"            \
        "add	%[h], %[h], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r5, %[b]		\n\t"            \
        "mul	r6, r5			\n\t"            \
        "lsr	r5, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[l], %[l], r7		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[h], %[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth   r6, %[a]                \n\t"            \
        "uxth   r5, %[b]                \n\t"            \
        "mul    r5, r6                  \n\t"            \
        "add    %[l], %[l], r5          \n\t"            \
        "mov    r5, #0                  \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "adc    %[o], r5                \n\t"            \
        /* al * bh */                                    \
        "lsr    r5, %[b], #16           \n\t"            \
        "mul    r6, r5                  \n\t"            \
        "lsr    r5, r6, #16             \n\t"            \
        "lsl    r6, r6, #16             \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "mov    r5, #0                  \n\t"            \
        "adc    %[o], r5                \n\t"            \
        /* ah * bh */                                    \
        "lsr    r6, %[a], #16           \n\t"            \
        "lsr    r5, %[b], #16           \n\t"            \
        "mul    r5, r6                  \n\t"            \
        "add    %[h], %[h], r5          \n\t"            \
        "mov    r5, #0                  \n\t"            \
        "adc    %[o], r5                \n\t"            \
        /* ah * bl */                                    \
        "uxth   r5, %[b]                \n\t"            \
        "mul    r6, r5                  \n\t"            \
        "lsr    r5, r6, #16             \n\t"            \
        "lsl    r6, r6, #16             \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "mov    r5, #0                  \n\t"            \
        "adc    %[o], r5                \n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "cc"                               \
    )
#endif
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r4, %[b]		\n\t"            \
        "mul	r4, r6			\n\t"            \
        "add	%[l], %[l], r4		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsr	r4, %[b], #16		\n\t"            \
        "mul	r6, r4			\n\t"            \
        "lsr	r4, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r4		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "lsr	r4, %[b], #16		\n\t"            \
        "mul	r4, r6			\n\t"            \
        "add	%[h], %[h], r4		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r4, %[b]		\n\t"            \
        "mul	r6, r4			\n\t"            \
        "lsr	r4, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r4		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r4", "r5", "r6", "cc"                         \
    )
#if !defined(WOLFSSL_SP_SMALL) && !defined(DEBUG)
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[l], %[l], r7		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[l], %[l], r7		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* al * bh */                                    \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[h], %[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[h], %[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mov    r8, %[a]                \n\t"            \
        /* al * bl */                                    \
        "uxth   r6, %[a]                \n\t"            \
        "uxth   r5, %[b]                \n\t"            \
        "mul    r5, r6                  \n\t"            \
        "add    %[l], %[l], r5          \n\t"            \
        "mov    %[a], #0                \n\t"            \
        "adc    %[h], %[a]              \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        "add    %[l], %[l], r5          \n\t"            \
        "adc    %[h], %[a]              \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        /* al * bh */                                    \
        "lsr    r5, %[b], #16           \n\t"            \
        "mul    r6, r5                  \n\t"            \
        "lsr    r5, r6, #16             \n\t"            \
        "lsl    r6, r6, #16             \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        /* ah * bh */                                    \
        "mov    %[a], r8                \n\t"            \
        "lsr    r6, %[a], #16           \n\t"            \
        "lsr    r5, %[b], #16           \n\t"            \
        "mul    r5, r6                  \n\t"            \
        "add    %[h], %[h], r5          \n\t"            \
        "mov    %[a], #0                \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        "add    %[h], %[h], r5          \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        /* ah * bl */                                    \
        "uxth   r5, %[b]                \n\t"            \
        "mul    r6, r5                  \n\t"            \
        "lsr    r5, r6, #16             \n\t"            \
        "lsl    r6, r6, #16             \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        "add    %[l], %[l], r6          \n\t"            \
        "adc    %[h], r5                \n\t"            \
        "adc    %[o], %[a]              \n\t"            \
        "mov    %[a], r8                \n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
#ifndef DEBUG
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        /* al * bl */                                    \
        "uxth	r6, %[a]		\n\t"            \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[l], %[l], r7		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        "add	%[l], %[l], r7		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        /* al * bh */                                    \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bh */                                    \
        "lsr	r6, %[a], #16		\n\t"            \
        "lsr	r7, %[b], #16		\n\t"            \
        "mul	r7, r6			\n\t"            \
        "add	%[h], %[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[h], %[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r7, %[b]		\n\t"            \
        "mul	r6, r7			\n\t"            \
        "lsr	r7, r6, #16		\n\t"            \
        "lsl	r6, r6, #16		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r7		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r7", "cc"                         \
    )
#else
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mov	r8, %[a]		\n\t"            \
        /* al * bl */                                    \
        "uxth	r5, %[a]		\n\t"            \
        "uxth	r6, %[b]		\n\t"            \
        "mul	r6, r5			\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "mov	%[a], #0		\n\t"            \
        "adc	%[h], %[a]		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], %[a]		\n\t"            \
        /* al * bh */                                    \
        "lsr	r6, %[b], #16		\n\t"            \
        "mul	r5, r6			\n\t"            \
        "lsr	r6, r5, #16		\n\t"            \
        "lsl	r5, r5, #16		\n\t"            \
        "add	%[l], %[l], r5		\n\t"            \
        "adc	%[h], r6		\n\t"            \
        "add	%[l], %[l], r5		\n\t"            \
        "adc	%[h], r6		\n\t"            \
        "adc	%[o], %[a]		\n\t"            \
        /* ah * bh */                                    \
        "mov    %[a], r8                \n\t"            \
        "lsr	r5, %[a], #16		\n\t"            \
        "lsr	r6, %[b], #16		\n\t"            \
        "mul	r6, r5			\n\t"            \
        "mov    %[a], #0                \n\t"            \
        "add	%[h], %[h], r6		\n\t"            \
        "adc	%[o], %[a]		\n\t"            \
        "add	%[h], %[h], r6		\n\t"            \
        "adc	%[o], %[a]		\n\t"            \
        /* ah * bl */                                    \
        "uxth	r6, %[b]		\n\t"            \
        "mul	r5, r6			\n\t"            \
        "lsr	r6, r5, #16		\n\t"            \
        "lsl	r5, r5, #16		\n\t"            \
        "add	%[l], %[l], r5		\n\t"            \
        "adc	%[h], r6		\n\t"            \
        "adc	%[o], %[a]		\n\t"            \
        "add	%[l], %[l], r5		\n\t"            \
        "adc	%[h], r6		\n\t"            \
        "adc	%[o], %[a]		\n\t"            \
        "mov    %[a], r8                \n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb)                     \
        : "r5", "r6", "r8", "cc"                         \
    )
#endif
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "lsr	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        "mov	%[l], r6		\n\t"            \
        "mov	%[h], r5		\n\t"            \
        /* al * al */                                    \
        "mul	%[l], %[l]		\n\t"            \
        /* ah * ah */                                    \
        "mul	%[h], %[h]		\n\t"            \
        /* 2 * al * ah */                                \
        "mul	r6, r5			\n\t"            \
        "lsr	r5, r6, #15		\n\t"            \
        "lsl	r6, r6, #17		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        : [h] "+l" (vh), [l] "+l" (vl)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "lsr	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "mul	r6, r6			\n\t"            \
        /* ah * ah */                                    \
        "mul	r4, r4			\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r4		\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[o], r5		\n\t"            \
        "lsr	r4, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "mul	r6, r4			\n\t"            \
        "lsr	r4, r6, #15		\n\t"            \
        "lsl	r6, r6, #17		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r4		\n\t"            \
        "adc	%[o], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va)                                   \
        : "r4", "r5", "r6", "cc"                         \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "lsr	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* al * al */                                    \
        "mul	r6, r6			\n\t"            \
        /* ah * ah */                                    \
        "mul	r5, r5			\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        "lsr	r5, %[a], #16		\n\t"            \
        "uxth	r6, %[a]		\n\t"            \
        /* 2 * al * ah */                                \
        "mul	r6, r5			\n\t"            \
        "lsr	r5, r6, #15		\n\t"            \
        "lsl	r6, r6, #17		\n\t"            \
        "add	%[l], %[l], r6		\n\t"            \
        "adc	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "r6", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "mov	r5, #0			\n\t"            \
        "adc	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "sub	%[l], %[l], %[a]	\n\t"            \
        "mov	r5, #0			\n\t"            \
        "sbc	%[h], r5		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh)                   \
        : [a] "l" (va)                                   \
        : "r5", "cc"                                     \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "adc	%[h], %[b]		\n\t"            \
        "adc	%[o], %[c]		\n\t"            \
        "add	%[l], %[l], %[a]	\n\t"            \
        "adc	%[h], %[b]		\n\t"            \
        "adc	%[o], %[c]		\n\t"            \
        : [l] "+l" (vl), [h] "+l" (vh), [o] "+l" (vo)    \
        : [a] "l" (va), [b] "l" (vb), [c] "l" (vc)       \
        : "cc"                                           \
    )

#endif

#ifdef WOLFSSL_SP_DIV_WORD_HALF
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * No division instruction used - does operation bit by bit.
 * Constant time.
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
                                          sp_int_digit d)
{
    __asm__ __volatile__ (
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs	r3, %[d], #24\n\t"
#else
        "lsr	r3, %[d], #24\n\t"
#endif
        "beq	2%=f\n\t"
	"\n1%=:\n\t"
        "movs	r3, #0\n\t"
        "b	3%=f\n\t"
	"\n2%=:\n\t"
        "mov	r3, #8\n\t"
	"\n3%=:\n\t"
        "movs	r4, #31\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs	r4, r4, r3\n\t"
#else
        "sub	r4, r4, r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls	%[d], %[d], r3\n\t"
#else
        "lsl	%[d], %[d], r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls	%[hi], %[hi], r3\n\t"
#else
        "lsl	%[hi], %[hi], r3\n\t"
#endif
        "mov	r5, %[lo]\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs	r5, r5, r4\n\t"
#else
        "lsr	r5, r5, r4\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls	%[lo], %[lo], r3\n\t"
#else
        "lsl	%[lo], %[lo], r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs	r5, r5, #1\n\t"
#else
        "lsr	r5, r5, #1\n\t"
#endif
#if defined(WOLFSSL_KEIL)
        "orrs	%[hi], %[hi], r5\n\t"
#elif defined(__clang__)
        "orrs	%[hi], r5\n\t"
#else
        "orr	%[hi], r5\n\t"
#endif

        "movs   r3, #0\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r5, %[d], #1\n\t"
#else
        "lsr    r5, %[d], #1\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r5, r5, #1\n\t"
#else
        "add    r5, r5, #1\n\t"
#endif
        "mov    r8, %[lo]\n\t"
        "mov    r9, %[hi]\n\t"
        /* Do top 32 */
        "movs   r6, r5\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r6, r6, %[hi]\n\t"
#else
        "sub    r6, r6, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "sbcs   r6, r6, r6\n\t"
#elif defined(__clang__)
        "sbcs   r6, r6\n\t"
#else
        "sbc    r6, r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r3\n\t"
#else
        "add    r3, r3, r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r3, r3, r6\n\t"
#else
        "sub    r3, r3, r6\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "ands   r6, r6, r5\n\t"
#elif defined(__clang__)
        "ands   r6, r5\n\t"
#else
        "and    r6, r5\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   %[hi], %[hi], r6\n\t"
#else
        "sub    %[hi], %[hi], r6\n\t"
#endif
        "movs   r4, #29\n\t"
        "\n"
    "L_sp_div_word_loop%=:\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[lo], %[lo], #1\n\t"
#else
        "lsl    %[lo], %[lo], #1\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   %[hi], %[hi], %[hi]\n\t"
#elif defined(__clang__)
        "adcs   %[hi], %[hi]\n\t"
#else
        "adc    %[hi], %[hi]\n\t"
#endif
        "movs   r6, r5\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r6, r6, %[hi]\n\t"
#else
        "sub    r6, r6, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "sbcs   r6, r6, r6\n\t"
#elif defined(__clang__)
        "sbcs   r6, r6\n\t"
#else
        "sbc    r6, r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r3\n\t"
#else
        "add    r3, r3, r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r3, r3, r6\n\t"
#else
        "sub    r3, r3, r6\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "ands   r6, r6, r5\n\t"
#elif defined(__clang__)
        "ands   r6, r5\n\t"
#else
        "and    r6, r5\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   %[hi], %[hi], r6\n\t"
#else
        "sub    %[hi], %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r4, r4, #1\n\t"
#else
        "sub    r4, r4, #1\n\t"
#endif
        "bpl    L_sp_div_word_loop%=\n\t"
        "movs   r7, #0\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r3\n\t"
#else
        "add    r3, r3, r3\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, #1\n\t"
#else
        "add    r3, r3, #1\n\t"
#endif
        /* r * d - Start */
        "uxth   %[hi], r3\n\t"
        "uxth   r4, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   r4, %[hi], r4\n\t"
#elif defined(__clang__)
        "muls   r4, %[hi]\n\t"
#else
        "mul    r4, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[d], #16\n\t"
#else
        "lsr    r6, %[d], #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r5, %[hi], #16\n\t"
#else
        "lsr    r5, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r7\n\t"
#elif defined(__clang__)
        "adcs   r5, r7\n\t"
#else
        "adc    r5, r7\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   %[hi], r3, #16\n\t"
#else
        "lsr    %[hi], r3, #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   r6, %[hi], r6\n\t"
#elif defined(__clang__)
        "muls   r6, %[hi]\n\t"
#else
        "mul    r6, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r5, r5, r6\n\t"
#else
        "add    r5, r5, r6\n\t"
#endif
        "uxth   r6, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[hi], #16\n\t"
#else
        "lsr    r6, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r6\n\t"
#elif defined(__clang__)
        "adcs   r5, r6\n\t"
#else
        "adc    r5, r6\n\t"
#endif
        /* r * d - Done */
        "mov    %[hi], r8\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   %[hi], %[hi], r4\n\t"
#else
        "sub    %[hi], %[hi], r4\n\t"
#endif
        "movs   r4, %[hi]\n\t"
        "mov    %[hi], r9\n\t"
#ifdef WOLFSSL_KEIL
        "sbcs   %[hi], %[hi], r5\n\t"
#elif defined(__clang__)
        "sbcs   %[hi], r5\n\t"
#else
        "sbc    %[hi], r5\n\t"
#endif
        "movs   r5, %[hi]\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r5\n\t"
#else
        "add    r3, r3, r5\n\t"
#endif
        /* r * d - Start */
        "uxth   %[hi], r3\n\t"
        "uxth   r4, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   r4, %[hi], r4\n\t"
#elif defined(__clang__)
        "muls   r4, %[hi]\n\t"
#else
        "mul    r4, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[d], #16\n\t"
#else
        "lsr    r6, %[d], #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r5, %[hi], #16\n\t"
#else
        "lsr    r5, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r7\n\t"
#elif defined(__clang__)
        "adcs   r5, r7\n\t"
#else
        "adc    r5, r7\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   %[hi], r3, #16\n\t"
#else
        "lsr    %[hi], r3, #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   r6, %[hi], r6\n\t"
#elif defined(__clang__)
        "muls   r6, %[hi]\n\t"
#else
        "mul    r6, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r5, r5, r6\n\t"
#else
        "add    r5, r5, r6\n\t"
#endif
        "uxth   r6, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[hi], #16\n\t"
#else
        "lsr    r6, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r6\n\t"
#elif defined(__clang__)
        "adcs   r5, r6\n\t"
#else
        "adc    r5, r6\n\t"
#endif
        /* r * d - Done */
        "mov    %[hi], r8\n\t"
        "mov    r6, r9\n\t"
#ifdef WOLFSSL_KEIL
        "subs   r4, %[hi], r4\n\t"
#else
#ifdef __clang__
        "subs   r4, %[hi], r4\n\t"
#else
        "sub    r4, %[hi], r4\n\t"
#endif
#endif
#ifdef WOLFSSL_KEIL
        "sbcs   r6, r6, r5\n\t"
#elif defined(__clang__)
        "sbcs   r6, r5\n\t"
#else
        "sbc    r6, r5\n\t"
#endif
        "movs   r5, r6\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r5\n\t"
#else
        "add    r3, r3, r5\n\t"
#endif
        /* r * d - Start */
        "uxth   %[hi], r3\n\t"
        "uxth   r4, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   r4, %[hi], r4\n\t"
#elif defined(__clang__)
        "muls   r4, %[hi]\n\t"
#else
        "mul    r4, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[d], #16\n\t"
#else
        "lsr    r6, %[d], #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r5, %[hi], #16\n\t"
#else
        "lsr    r5, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r7\n\t"
#elif defined(__clang__)
        "adcs   r5, r7\n\t"
#else
        "adc    r5, r7\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   %[hi], r3, #16\n\t"
#else
        "lsr    %[hi], r3, #16\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "muls   r6, %[hi], r6\n\t"
#elif defined(__clang__)
        "muls   r6, %[hi]\n\t"
#else
        "mul    r6, %[hi]\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r5, r5, r6\n\t"
#else
        "add    r5, r5, r6\n\t"
#endif
        "uxth   r6, %[d]\n\t"
#ifdef WOLFSSL_KEIL
        "muls   %[hi], r6, %[hi]\n\t"
#elif defined(__clang__)
        "muls   %[hi], r6\n\t"
#else
        "mul    %[hi], r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsrs   r6, %[hi], #16\n\t"
#else
        "lsr    r6, %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "lsls   %[hi], %[hi], #16\n\t"
#else
        "lsl    %[hi], %[hi], #16\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r4, r4, %[hi]\n\t"
#else
        "add    r4, r4, %[hi]\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "adcs   r5, r5, r6\n\t"
#elif defined(__clang__)
        "adcs   r5, r6\n\t"
#else
        "adc    r5, r6\n\t"
#endif
        /* r * d - Done */
        "mov    %[hi], r8\n\t"
        "mov    r6, r9\n\t"
#ifdef WOLFSSL_KEIL
        "subs   r4, %[hi], r4\n\t"
#else
#ifdef __clang__
        "subs   r4, %[hi], r4\n\t"
#else
        "sub    r4, %[hi], r4\n\t"
#endif
#endif
#ifdef WOLFSSL_KEIL
        "sbcs   r6, r6, r5\n\t"
#elif defined(__clang__)
        "sbcs   r6, r5\n\t"
#else
        "sbc    r6, r5\n\t"
#endif
        "movs   r5, r6\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "adds   r3, r3, r5\n\t"
#else
        "add    r3, r3, r5\n\t"
#endif
        "movs   r6, %[d]\n\t"
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r6, r6, r4\n\t"
#else
        "sub    r6, r6, r4\n\t"
#endif
#ifdef WOLFSSL_KEIL
        "sbcs   r6, r6, r6\n\t"
#elif defined(__clang__)
        "sbcs   r6, r6\n\t"
#else
        "sbc    r6, r6\n\t"
#endif
#if defined(__clang__) || defined(WOLFSSL_KEIL)
        "subs   r3, r3, r6\n\t"
#else
        "sub    r3, r3, r6\n\t"
#endif
        "movs   %[hi], r3\n\t"
        : [hi] "+l" (hi), [lo] "+l" (lo), [d] "+l" (d)
        :
        : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "cc"
    );
    return (uint32_t)(size_t)hi;
}

#define SP_ASM_DIV_WORD
#endif /* !WOLFSSL_SP_DIV_WORD_HALF */

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_ARM_THUMB && SP_WORD_SIZE == 32 */

    #if defined(WOLFSSL_SP_PPC64) && SP_WORD_SIZE == 64
/*
 * CPU: PPC64
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "mulld	%[l], %[a], %[b]	\n\t"            \
        "mulhdu	%[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory"                                       \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mulhdu	%[h], %[a], %[b]	\n\t"            \
        "mulld	%[l], %[a], %[b]	\n\t"            \
        "li	%[o], 0			\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        :                                                \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[b]		\n\t"            \
        "mulhdu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[b]		\n\t"            \
        "mulhdu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[b]		\n\t"            \
        "mulhdu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[b]		\n\t"            \
        "mulhdu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "mulld	%[l], %[a], %[a]	\n\t"            \
        "mulhdu	%[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[a]		\n\t"            \
        "mulhdu	17, %[a], %[a]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "16", "17", "cc"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "mulld	16, %[a], %[a]		\n\t"            \
        "mulhdu	17, %[a], %[a]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "16", "17", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "addze	%[h], %[h]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subfc	%[l], %[a], %[l]	\n\t"            \
        "li    16, 0			\n\t"            \
        "subfe %[h], 16, %[h]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "16", "cc"                                     \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "adde	%[h], %[h], %[b]	\n\t"            \
        "adde	%[o], %[o], %[c]	\n\t"            \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "adde	%[h], %[h], %[b]	\n\t"            \
        "adde	%[o], %[o], %[c]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
/* Count leading zeros. */
#define SP_ASM_LZCNT(va, vn)                             \
    __asm__ __volatile__ (                               \
        "cntlzd	%[n], %[a]	\n\t"                    \
        : [n] "=r" (vn)                                  \
        : [a] "r" (va)                                   \
        :                                                \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_PPC64 && SP_WORD_SIZE == 64 */

    #if defined(WOLFSSL_SP_PPC) && SP_WORD_SIZE == 32
/*
 * CPU: PPC 32-bit
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "mullw	%[l], %[a], %[b]	\n\t"            \
        "mulhwu	%[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory"                                       \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mulhwu	%[h], %[a], %[b]	\n\t"            \
        "mullw	%[l], %[a], %[b]	\n\t"            \
        "li	%[o], 0			\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[b]		\n\t"            \
        "mulhwu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[b]		\n\t"            \
        "mulhwu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[b]		\n\t"            \
        "mulhwu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[b]		\n\t"            \
        "mulhwu	17, %[a], %[b]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "16", "17", "cc"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "mullw	%[l], %[a], %[a]	\n\t"            \
        "mulhwu	%[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[a]		\n\t"            \
        "mulhwu	17, %[a], %[a]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        "addze	%[o], %[o]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "16", "17", "cc"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "mullw	16, %[a], %[a]		\n\t"            \
        "mulhwu	17, %[a], %[a]		\n\t"            \
        "addc	%[l], %[l], 16		\n\t"            \
        "adde	%[h], %[h], 17		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "16", "17", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "addze	%[h], %[h]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "cc"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "subfc	%[l], %[a], %[l]	\n\t"            \
        "li	16, 0			\n\t"            \
        "subfe	%[h], 16, %[h]		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "16", "cc"                                     \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "adde	%[h], %[h], %[b]	\n\t"            \
        "adde	%[o], %[o], %[c]	\n\t"            \
        "addc	%[l], %[l], %[a]	\n\t"            \
        "adde	%[h], %[h], %[b]	\n\t"            \
        "adde	%[o], %[o], %[c]	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )
/* Count leading zeros. */
#define SP_ASM_LZCNT(va, vn)                             \
    __asm__ __volatile__ (                               \
        "cntlzw	%[n], %[a]	\n\t"                    \
        : [n] "=r" (vn)                                  \
        : [a] "r" (va)                                   \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_PPC && SP_WORD_SIZE == 64 */

    #if defined(WOLFSSL_SP_MIPS64) && SP_WORD_SIZE == 64
/*
 * CPU: MIPS 64-bit
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory", "$lo", "$hi"                         \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        "move	%[o], $0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$lo", "$hi"                                   \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[a]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory", "$lo", "$hi"                         \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[a]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "dmultu	%[a], %[a]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "daddu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "daddu	%[h], %[h], $11		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$10", "$11", "$12", "$lo", "$hi"              \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "daddu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$12"                                          \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "move	$12, %[l]		\n\t"            \
        "dsubu	%[l], $12, %[a]		\n\t"            \
        "sltu	$12, $12, %[l]		\n\t"            \
        "dsubu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$12"                                          \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "daddu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], %[b]	\n\t"            \
        "sltu	$12, %[h], %[b]		\n\t"            \
        "daddu	%[o], %[o], %[c]	\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "daddu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        "daddu	%[h], %[h], %[b]	\n\t"            \
        "sltu	$12, %[h], %[b]		\n\t"            \
        "daddu	%[o], %[o], %[c]	\n\t"            \
        "daddu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "$12"                                          \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_MIPS64 && SP_WORD_SIZE == 64 */

    #if defined(WOLFSSL_SP_MIPS) && SP_WORD_SIZE == 32
/*
 * CPU: MIPS 32-bit
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory", "%lo", "%hi"                         \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        "move	%[o], $0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "%lo", "%hi"                                   \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[b]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[a]		\n\t"            \
        "mflo	%[l]			\n\t"            \
        "mfhi	%[h]			\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory", "%lo", "%hi"                         \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[a]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "sltu	$12, %[h], $11		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "multu	%[a], %[a]		\n\t"            \
        "mflo	$10			\n\t"            \
        "mfhi	$11			\n\t"            \
        "addu	%[l], %[l], $10		\n\t"            \
        "sltu	$12, %[l], $10		\n\t"            \
        "addu	%[h], %[h], $11		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$10", "$11", "$12", "%lo", "%hi"              \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "addu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$12"                                          \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "move	$12, %[l]		\n\t"            \
        "subu	%[l], $12, %[a]		\n\t"            \
        "sltu	$12, $12, %[l]		\n\t"            \
        "subu	%[h], %[h], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "$12"                                          \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "addu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], %[b]	\n\t"            \
        "sltu	$12, %[h], %[b]		\n\t"            \
        "addu	%[o], %[o], %[c]	\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[l], %[l], %[a]	\n\t"            \
        "sltu	$12, %[l], %[a]		\n\t"            \
        "addu	%[h], %[h], $12		\n\t"            \
        "sltu	$12, %[h], $12		\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        "addu	%[h], %[h], %[b]	\n\t"            \
        "sltu	$12, %[h], %[b]		\n\t"            \
        "addu	%[o], %[o], %[c]	\n\t"            \
        "addu	%[o], %[o], $12		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "$12"                                          \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_MIPS && SP_WORD_SIZE == 32 */

    #if defined(WOLFSSL_SP_RISCV64) && SP_WORD_SIZE == 64
/*
 * CPU: RISCV 64-bit
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[b]	\n\t"            \
        "mulhu	%[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory"                                       \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mulhu	%[h], %[a], %[b]	\n\t"            \
        "mul	%[l], %[a], %[b]	\n\t"            \
        "add	%[o], zero, zero	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        :                                                \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[a]	\n\t"            \
        "mulhu	%[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[a]		\n\t"            \
        "mulhu	a6, %[a], %[a]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "a5", "a6", "a7"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[a]		\n\t"            \
        "mulhu	a6, %[a], %[a]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a5", "a6", "a7"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a7"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "add	a7, %[l], zero		\n\t"            \
        "sub	%[l], a7, %[a]		\n\t"            \
        "sltu	a7, a7, %[l]		\n\t"            \
        "sub	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a7"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], %[b]	\n\t"            \
        "sltu	a7, %[h], %[b]		\n\t"            \
        "add	%[o], %[o], %[c]	\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], %[b]	\n\t"            \
        "sltu	a7, %[h], %[b]		\n\t"            \
        "add	%[o], %[o], %[c]	\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "a7"                                           \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_RISCV64 && SP_WORD_SIZE == 64 */

    #if defined(WOLFSSL_SP_RISCV32) && SP_WORD_SIZE == 32
/*
 * CPU: RISCV 32-bit
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[b]	\n\t"            \
        "mulhu	%[h], %[a], %[b]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory"                                       \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mulhu	%[h], %[a], %[b]	\n\t"            \
        "mul	%[l], %[a], %[b]	\n\t"            \
        "add	%[o], zero, zero	\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        :                                                \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[b]		\n\t"            \
        "mulhu	a6, %[a], %[b]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "a5", "a6", "a7"                               \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "mul	%[l], %[a], %[a]	\n\t"            \
        "mulhu	%[h], %[a], %[a]	\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory"                                       \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[a]		\n\t"            \
        "mulhu	a6, %[a], %[a]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "sltu	a7, %[h], a6		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "a5", "a6", "a7"                               \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "mul	a5, %[a], %[a]		\n\t"            \
        "mulhu	a6, %[a], %[a]		\n\t"            \
        "add	%[l], %[l], a5		\n\t"            \
        "sltu	a7, %[l], a5		\n\t"            \
        "add	%[h], %[h], a6		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a5", "a6", "a7"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a7"                                           \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "add	a7, %[l], zero		\n\t"            \
        "sub	%[l], a7, %[a]		\n\t"            \
        "sltu	a7, a7, %[l]		\n\t"            \
        "sub	%[h], %[h], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "a7"                                           \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], %[b]	\n\t"            \
        "sltu	a7, %[h], %[b]		\n\t"            \
        "add	%[o], %[o], %[c]	\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[l], %[l], %[a]	\n\t"            \
        "sltu	a7, %[l], %[a]		\n\t"            \
        "add	%[h], %[h], a7		\n\t"            \
        "sltu	a7, %[h], a7		\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        "add	%[h], %[h], %[b]	\n\t"            \
        "sltu	a7, %[h], %[b]		\n\t"            \
        "add	%[o], %[o], %[c]	\n\t"            \
        "add	%[o], %[o], a7		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "a7"                                           \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_RISCV32 && SP_WORD_SIZE == 32 */

    #if defined(WOLFSSL_SP_S390X) && SP_WORD_SIZE == 64
/*
 * CPU: Intel s390x
 */

/* Multiply va by vb and store double size result in: vh | vl */
#define SP_ASM_MUL(vl, vh, va, vb)                       \
    __asm__ __volatile__ (                               \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "lgr	%[l], %%r1		\n\t"            \
        "lgr	%[h], %%r0		\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "memory", "r0", "r1"                           \
    )
/* Multiply va by vb and store double size result in: vo | vh | vl */
#define SP_ASM_MUL_SET(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "lghi	%[o], 0			\n\t"            \
        "lgr	%[l], %%r1		\n\t"            \
        "lgr	%[h], %%r0		\n\t"            \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "=r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r0", "r1"                                     \
    )
/* Multiply va by vb and add double size result into: vo | vh | vl */
#define SP_ASM_MUL_ADD(vl, vh, vo, va, vb)               \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "alcgr	%[o], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r0", "r1", "r10", "cc"                        \
    )
/* Multiply va by vb and add double size result into: vh | vl */
#define SP_ASM_MUL_ADD_NO(vl, vh, va, vb)                \
    __asm__ __volatile__ (                               \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r0", "r1", "cc"                               \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl */
#define SP_ASM_MUL_ADD2(vl, vh, vo, va, vb)              \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "alcgr	%[o], %%r10	\n\t"                    \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "alcgr	%[o], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r0", "r1", "r10", "cc"                        \
    )
/* Multiply va by vb and add double size result twice into: vo | vh | vl
 * Assumes first add will not overflow vh | vl
 */
#define SP_ASM_MUL_ADD2_NO(vl, vh, vo, va, vb)           \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %[b]		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "alcgr	%[o], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb)                     \
        : "r0", "r1", "r10", "cc"                        \
    )
/* Square va and store double size result in: vh | vl */
#define SP_ASM_SQR(vl, vh, va)                           \
    __asm__ __volatile__ (                               \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %%r1		\n\t"            \
        "lgr	%[l], %%r1		\n\t"            \
        "lgr	%[h], %%r0		\n\t"            \
        : [h] "+r" (vh), [l] "+r" (vl)                   \
        : [a] "r" (va)                                   \
        : "memory", "r0", "r1"                           \
    )
/* Square va and add double size result into: vo | vh | vl */
#define SP_ASM_SQR_ADD(vl, vh, vo, va)                   \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %%r1		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        "alcgr	%[o], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va)                                   \
        : "r0", "r1", "r10", "cc"                        \
    )
/* Square va and add double size result into: vh | vl */
#define SP_ASM_SQR_ADD_NO(vl, vh, va)                    \
    __asm__ __volatile__ (                               \
        "lgr	%%r1, %[a]		\n\t"            \
        "mlgr	%%r0, %%r1		\n\t"            \
        "algr	%[l], %%r1	\n\t"                    \
        "alcgr	%[h], %%r0	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "r0", "r1", "cc"                               \
    )
/* Add va into: vh | vl */
#define SP_ASM_ADDC(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "algr	%[l], %[a]	\n\t"                    \
        "alcgr	%[h], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "r10", "cc"                                    \
    )
/* Sub va from: vh | vl */
#define SP_ASM_SUBB(vl, vh, va)                          \
    __asm__ __volatile__ (                               \
        "lghi	%%r10, 0	\n\t"                    \
        "slgr	%[l], %[a]	\n\t"                    \
        "slbgr	%[h], %%r10	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh)                   \
        : [a] "r" (va)                                   \
        : "r10", "cc"                                    \
    )
/* Add two times vc | vb | va into vo | vh | vl */
#define SP_ASM_ADD_DBL_3(vl, vh, vo, va, vb, vc)         \
    __asm__ __volatile__ (                               \
        "algr	%[l], %[a]	\n\t"                    \
        "alcgr	%[h], %[b]	\n\t"                    \
        "alcgr	%[o], %[c]	\n\t"                    \
        "algr	%[l], %[a]	\n\t"                    \
        "alcgr	%[h], %[b]	\n\t"                    \
        "alcgr	%[o], %[c]	\n\t"                    \
        : [l] "+r" (vl), [h] "+r" (vh), [o] "+r" (vo)    \
        : [a] "r" (va), [b] "r" (vb), [c] "r" (vc)       \
        : "cc"                                           \
    )

#define SP_INT_ASM_AVAILABLE

    #endif /* WOLFSSL_SP_S390X && SP_WORD_SIZE == 64 */

#ifdef SP_INT_ASM_AVAILABLE
    #ifndef SP_INT_NO_ASM
        #define SQR_MUL_ASM
    #endif
    #ifndef SP_ASM_ADDC_REG
        #define SP_ASM_ADDC_REG  SP_ASM_ADDC
    #endif /* SP_ASM_ADDC_REG */
    #ifndef SP_ASM_SUBB_REG
        #define SP_ASM_SUBB_REG  SP_ASM_SUBB
    #endif /* SP_ASM_ADDC_REG */
#endif /* SQR_MUL_ASM */

#endif /* !WOLFSSL_NO_ASM */


#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || \
    !defined(NO_DSA) || !defined(NO_DH) || \
    (defined(HAVE_ECC) && defined(HAVE_COMP_KEY)) || defined(OPENSSL_EXTRA) || \
    (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_PUBLIC_ONLY))
#ifndef WC_NO_CACHE_RESISTANT
    /* Mask of address for constant time operations. */
    const size_t sp_off_on_addr[2] =
    {
        (size_t) 0,
        (size_t)-1
    };
#endif
#endif


#if defined(WOLFSSL_HAVE_SP_DH) || defined(WOLFSSL_HAVE_SP_RSA)

#ifdef __cplusplus
extern "C" {
#endif

/* Modular exponentiation implementations using Single Precision. */
WOLFSSL_LOCAL int sp_ModExp_1024(sp_int* base, sp_int* exp, sp_int* mod,
    sp_int* res);
WOLFSSL_LOCAL int sp_ModExp_1536(sp_int* base, sp_int* exp, sp_int* mod,
    sp_int* res);
WOLFSSL_LOCAL int sp_ModExp_2048(sp_int* base, sp_int* exp, sp_int* mod,
    sp_int* res);
WOLFSSL_LOCAL int sp_ModExp_3072(sp_int* base, sp_int* exp, sp_int* mod,
    sp_int* res);
WOLFSSL_LOCAL int sp_ModExp_4096(sp_int* base, sp_int* exp, sp_int* mod,
    sp_int* res);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_RSA */


#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(OPENSSL_ALL)
static int _sp_mont_red(sp_int* a, const sp_int* m, sp_int_digit mp);
#endif
#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(WOLFCRYPT_HAVE_ECCSI) || defined(WOLFCRYPT_HAVE_SAKKE) || \
    defined(OPENSSL_ALL)
static void _sp_mont_setup(const sp_int* m, sp_int_digit* rho);
#endif

/* Determine when mp_add_d is required. */
#if !defined(NO_PWDBASED) || defined(WOLFSSL_KEY_GEN) || !defined(NO_DH) || \
    !defined(NO_DSA) || (defined(HAVE_ECC) && defined(HAVE_COMP_KEY)) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(OPENSSL_EXTRA)
#define WOLFSSL_SP_ADD_D
#endif
/* Determine when mp_sub_d is required. */
#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || defined(HAVE_ECC) || !defined(NO_DSA)
#define WOLFSSL_SP_SUB_D
#endif
/* Determine when mp_read_radix with a radix of 10 is required. */
#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(NO_RSA) && \
    !defined(WOLFSSL_RSA_VERIFY_ONLY)) || defined(HAVE_ECC) || \
    !defined(NO_DSA) || defined(OPENSSL_EXTRA)
#define WOLFSSL_SP_READ_RADIX_16
#endif
/* Determine when mp_read_radix with a radix of 10 is required. */
#if defined(WOLFSSL_SP_MATH_ALL) && !defined(NO_RSA) && \
    !defined(WOLFSSL_RSA_VERIFY_ONLY)
#define WOLFSSL_SP_READ_RADIX_10
#endif
/* Determine when mp_invmod is required. */
#if defined(HAVE_ECC) || !defined(NO_DSA) || defined(OPENSSL_EXTRA) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))
#define WOLFSSL_SP_INVMOD
#endif
/* Determine when mp_invmod_mont_ct is required. */
#if defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)
#define WOLFSSL_SP_INVMOD_MONT_CT
#endif

/* Determine when mp_prime_gen is required. */
#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH) || \
    (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN))
#define WOLFSSL_SP_PRIME_GEN
#endif

/* Set the multi-precision number to zero.
 *
 * Assumes a is not NULL.
 *
 * @param  [out]  a  SP integer to set to zero.
 */
static void _sp_zero(sp_int* a)
{
    sp_int_minimal* am = (sp_int_minimal *)a;

    am->used = 0;
    am->dp[0] = 0;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    am->sign = MP_ZPOS;
#endif
}


/* Initialize the multi-precision number to be zero with a given max size.
 *
 * @param  [out]  a     SP integer.
 * @param  [in]   size  Number of words to say are available.
 */
static void _sp_init_size(sp_int* a, unsigned int size)
{
    volatile sp_int_minimal* am = (sp_int_minimal *)a;

#ifdef HAVE_WOLF_BIGINT
    wc_bigint_init((struct WC_BIGINT*)&am->raw);
#endif
    _sp_zero((sp_int*)am);

    am->size = size;
}

/* Initialize the multi-precision number to be zero with a given max size.
 *
 * @param  [out]  a     SP integer.
 * @param  [in]   size  Number of words to say are available.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL.
 */
int sp_init_size(sp_int* a, unsigned int size)
{
    int err = MP_OKAY;

    /* Validate parameters. Don't use size more than max compiled. */
    if ((a == NULL) || ((size <= 0) || (size > SP_INT_DIGITS))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        _sp_init_size(a, size);
    }

    return err;
}

/* Initialize the multi-precision number to be zero.
 *
 * @param  [out]  a  SP integer.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL.
 */
int sp_init(sp_int* a)
{
    int err = MP_OKAY;

    /* Validate parameter. */
    if (a == NULL) {
        err = MP_VAL;
    }
    else {
        /* Assume complete sp_int with SP_INT_DIGITS digits. */
        _sp_init_size(a, SP_INT_DIGITS);
    }

    return err;
}

#if !defined(WOLFSSL_RSA_PUBLIC_ONLY) || !defined(NO_DH) || defined(HAVE_ECC)
/* Initialize up to six multi-precision numbers to be zero.
 *
 * @param  [out]  n1  SP integer.
 * @param  [out]  n2  SP integer.
 * @param  [out]  n3  SP integer.
 * @param  [out]  n4  SP integer.
 * @param  [out]  n5  SP integer.
 * @param  [out]  n6  SP integer.
 *
 * @return  MP_OKAY on success.
 */
int sp_init_multi(sp_int* n1, sp_int* n2, sp_int* n3, sp_int* n4, sp_int* n5,
    sp_int* n6)
{
    /* Initialize only those pointers that are valid. */
    if (n1 != NULL) {
        _sp_init_size(n1, SP_INT_DIGITS);
    }
    if (n2 != NULL) {
        _sp_init_size(n2, SP_INT_DIGITS);
    }
    if (n3 != NULL) {
        _sp_init_size(n3, SP_INT_DIGITS);
    }
    if (n4 != NULL) {
        _sp_init_size(n4, SP_INT_DIGITS);
    }
    if (n5 != NULL) {
        _sp_init_size(n5, SP_INT_DIGITS);
    }
    if (n6 != NULL) {
        _sp_init_size(n6, SP_INT_DIGITS);
    }

    return MP_OKAY;
}
#endif /* !WOLFSSL_RSA_PUBLIC_ONLY || !NO_DH || HAVE_ECC */

/* Free the memory allocated in the multi-precision number.
 *
 * @param  [in]  a  SP integer.
 */
void sp_free(sp_int* a)
{
    if (a != NULL) {
    #ifdef HAVE_WOLF_BIGINT
        wc_bigint_free(&a->raw);
    #endif
    }
}

#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || defined(HAVE_ECC)
/* Grow multi-precision number to be able to hold l digits.
 * This function does nothing as the number of digits is fixed.
 *
 * @param  [in,out]  a  SP integer.
 * @param  [in]      l  Number of digits to grow to.
 *
 * @return  MP_OKAY on success
 * @return  MP_MEM if the number of digits requested is more than available.
 */
int sp_grow(sp_int* a, int l)
{
    int err = MP_OKAY;

    /* Validate parameter. */
    if ((a == NULL) || (l < 0)) {
        err = MP_VAL;
    }
    /* Ensure enough words allocated for grow. */
    if ((err == MP_OKAY) && ((unsigned int)l > a->size)) {
        err = MP_MEM;
    }
    if (err == MP_OKAY) {
        unsigned int i;

        /* Put in zeros up to the new length. */
        for (i = a->used; i < (unsigned int)l; i++) {
            a->dp[i] = 0;
        }
    }

    return err;
}
#endif /* (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) || !NO_DH || HAVE_ECC */

#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(HAVE_ECC)
/* Set the multi-precision number to zero.
 *
 * @param  [out]  a  SP integer to set to zero.
 */
void sp_zero(sp_int* a)
{
    /* Make an sp_int with valid pointer zero. */
    if (a != NULL) {
        _sp_zero(a);
    }
}
#endif /* (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) || HAVE_ECC */

/* Clear the data from the multi-precision number, set to zero and free.
 *
 * @param  [out]  a  SP integer.
 */
void sp_clear(sp_int* a)
{
    /* Clear when valid pointer passed in. */
    if (a != NULL) {
        unsigned int i;

        /* Only clear the digits being used. */
        for (i = 0; i < a->used; i++) {
            a->dp[i] = 0;
        }
        /* Set back to zero and free. */
        _sp_zero(a);
        sp_free(a);
    }
}

#if !defined(NO_RSA) || !defined(NO_DH) || defined(HAVE_ECC) || \
    !defined(NO_DSA) || defined(WOLFSSL_SP_PRIME_GEN)
/* Ensure the data in the multi-precision number is zeroed.
 *
 * Use when security sensitive data needs to be wiped.
 *
 * @param  [in]  a  SP integer.
 */
void sp_forcezero(sp_int* a)
{
    /* Zeroize when a vald pointer passed in. */
    if (a != NULL) {
        /* Ensure all data zeroized - data not zeroed when used decreases. */
        ForceZero(a->dp, a->size * SP_WORD_SIZEOF);
        /* Set back to zero. */
    #ifdef HAVE_WOLF_BIGINT
        /* Zeroize the raw data as well. */
        wc_bigint_zero(&a->raw);
    #endif
        /* Make value zero and free. */
        _sp_zero(a);
        sp_free(a);
    }
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY || !NO_DH || HAVE_ECC */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    !defined(NO_RSA) || defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY)
/* Copy value of multi-precision number a into r.
 *
 * @param  [in]   a  SP integer - source.
 * @param  [out]  r  SP integer - destination.
 */
static void _sp_copy(const sp_int* a, sp_int* r)
{
    /* Copy words across. */
    if (a->used == 0) {
        r->dp[0] = 0;
    }
    else {
        XMEMCPY(r->dp, a->dp, a->used * SP_WORD_SIZEOF);
    }
    /* Set number of used words in result. */
    r->used = a->used;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Set sign of result. */
    r->sign = a->sign;
#endif
}

/* Copy value of multi-precision number a into r.
 *
 * @param  [in]   a  SP integer - source.
 * @param  [out]  r  SP integer - destination.
 *
 * @return  MP_OKAY on success.
 */
int sp_copy(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Only copy if different pointers. */
    if (a != r) {
        /* Validated space in result. */
        if ((err == MP_OKAY) && (a->used > r->size)) {
            err = MP_VAL;
        }
        if (err == MP_OKAY) {
            _sp_copy(a, r);
        }
    }

    return err;
}
#endif

#if ((defined(WOLFSSL_SP_MATH_ALL) && ((!defined(WOLFSSL_RSA_VERIFY_ONLY) && \
      !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH))) || \
     defined(OPENSSL_ALL)) && defined(WC_PROTECT_ENCRYPTED_MEM)

/* Copy 2 numbers into two results based on y. Copy a fixed number of digits.
 *
 * Constant time implementation.
 * When y is 0, r1 = a2 and r2 = a1.
 * When y is 1, r1 = a1 and r2 = a2.
 *
 * @param [in]  a1    First number to copy.
 * @param [in]  a2    Second number to copy.
 * @param [out] r1    First result number to copy into.
 * @param [out] r2    Second result number to copy into.
 * @param [in]  y     Indicates which number goes into which result number.
 * @param [in]  used  Number of digits to copy.
 */
static void _sp_copy_2_ct(const sp_int* a1, const sp_int* a2, sp_int* r1,
    sp_int* r2, int y, unsigned int used)
{
    unsigned int i;

    /* Copy data - constant time. */
    for (i = 0; i < used; i++) {
        r1->dp[i] = (a1->dp[i] & ((sp_digit)wc_off_on_addr[y  ])) +
                    (a2->dp[i] & ((sp_digit)wc_off_on_addr[y^1]));
        r2->dp[i] = (a1->dp[i] & ((sp_digit)wc_off_on_addr[y^1])) +
                    (a2->dp[i] & ((sp_digit)wc_off_on_addr[y  ]));
    }
    /* Copy used. */
    r1->used = (a1->used & ((int)wc_off_on_addr[y  ])) +
               (a2->used & ((int)wc_off_on_addr[y^1]));
    r2->used = (a1->used & ((int)wc_off_on_addr[y^1])) +
               (a2->used & ((int)wc_off_on_addr[y  ]));
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Copy sign. */
    r1->sign = (a1->sign & ((int)wc_off_on_addr[y  ])) +
               (a2->sign & ((int)wc_off_on_addr[y^1]));
    r2->sign = (a1->sign & ((int)wc_off_on_addr[y^1])) +
               (a2->sign & ((int)wc_off_on_addr[y  ]));
#endif
}

#endif

#if defined(WOLFSSL_SP_MATH_ALL) || (defined(HAVE_ECC) && defined(FP_ECC))
/* Initializes r and copies in value from a.
 *
 * @param  [out]  r  SP integer - destination.
 * @param  [in]   a  SP integer - source.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL.
 */
int sp_init_copy(sp_int* r, const sp_int* a)
{
    int err;

    /* Initialize r and copy value in a into it. */
    err = sp_init(r);
    if (err == MP_OKAY) {
        err = sp_copy(a, r);
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || (HAVE_ECC && FP_ECC) */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || !defined(NO_DSA)
/* Exchange the values in a and b.
 *
 * Avoid using this API as three copy operations are performed.
 *
 * @param  [in,out]  a  SP integer to swap.
 * @param  [in,out]  b  SP integer to swap.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or b is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_exch(sp_int* a, sp_int* b)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL)) {
        err = MP_VAL;
    }
    /* Check space for a in b and b in a. */
    if ((err == MP_OKAY) && ((a->size < b->used) || (b->size < a->used))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Declare temporary for swapping. */
        DECL_SP_INT(t, a->used);

        /* Create temporary for swapping. */
        ALLOC_SP_INT(t, a->used, err, NULL);
        if (err == MP_OKAY) {
            /* Cache allocated size of a and b. */
            unsigned int asize = a->size;
            unsigned int bsize = b->size;
            /* Copy all of SP int: t <- a, a <- b, b <- t. */
            XMEMCPY(t, a, MP_INT_SIZEOF(a->used));
            XMEMCPY(a, b, MP_INT_SIZEOF(b->used));
            XMEMCPY(b, t, MP_INT_SIZEOF(t->used));
            /* Put back size of a and b. */
            a->size = asize;
            b->size = bsize;
        }

        FREE_SP_INT(t, NULL);
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) || !NO_DH ||
        * !NO_DSA */

#if defined(HAVE_ECC) && defined(ECC_TIMING_RESISTANT) && \
    !defined(WC_NO_CACHE_RESISTANT)
/* Conditional swap of SP int values in constant time.
 *
 * @param [in]  a     First SP int to conditionally swap.
 * @param [in]  b     Second SP int to conditionally swap.
 * @param [in]  cnt   Count of words to copy.
 * @param [in]  swap  When value is 1 then swap.
 * @param [in]  t     Temporary SP int to use in swap.
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_cond_swap_ct_ex(sp_int* a, sp_int* b, int cnt, int swap, sp_int* t)
{
    unsigned int i;
    sp_int_digit mask = (sp_int_digit)0 - (sp_int_digit)swap;

    /* XOR other fields in sp_int into temp - mask set when swapping. */
    t->used = (a->used ^ b->used) & (unsigned int)mask;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    t->sign = (a->sign ^ b->sign) & (unsigned int)mask;
#endif

    /* XOR requested words into temp - mask set when swapping. */
    for (i = 0; i < (unsigned int)cnt; i++) {
        t->dp[i] = (a->dp[i] ^ b->dp[i]) & mask;
    }

    /* XOR temporary - when mask set then result will be b. */
    a->used ^= t->used;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    a->sign ^= t->sign;
#endif
    for (i = 0; i < (unsigned int)cnt; i++) {
        a->dp[i] ^= t->dp[i];
    }

    /* XOR temporary - when mask set then result will be a. */
    b->used ^= t->used;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    b->sign ^= b->sign;
#endif
    for (i = 0; i < (unsigned int)cnt; i++) {
        b->dp[i] ^= t->dp[i];
    }

    return MP_OKAY;
}

/* Conditional swap of SP int values in constant time.
 *
 * @param [in]  a     First SP int to conditionally swap.
 * @param [in]  b     Second SP int to conditionally swap.
 * @param [in]  cnt   Count of words to copy.
 * @param [in]  swap  When value is 1 then swap.
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_cond_swap_ct(sp_int* a, sp_int* b, int cnt, int swap)
{
    int err = MP_OKAY;
    DECL_SP_INT(t, (size_t)cnt);

    /* Allocate temporary to hold masked xor of a and b. */
    ALLOC_SP_INT(t, cnt, err, NULL);

    if (err == MP_OKAY) {
        err = sp_cond_swap_ct_ex(a, b, cnt, swap, t);
        FREE_SP_INT(t, NULL);
    }

    return err;
}
#endif /* HAVE_ECC && ECC_TIMING_RESISTANT && !WC_NO_CACHE_RESISTANT */

#ifdef WOLFSSL_SP_INT_NEGATIVE
/* Calculate the absolute value of the multi-precision number.
 *
 * @param  [in]   a  SP integer to calculate absolute value of.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL.
 */
int sp_abs(const sp_int* a, sp_int* r)
{
    int err;

    /* Copy a into r - copy fails when r is NULL. */
    err = sp_copy(a, r);
    if (err == MP_OKAY) {
        r->sign = MP_ZPOS;
    }

    return err;
}
#endif /* WOLFSSL_SP_INT_NEGATIVE */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY))
/* Compare absolute value of two multi-precision numbers.
 *
 * @param  [in]  a  SP integer.
 * @param  [in]  b  SP integer.
 *
 * @return  MP_GT when a is greater than b.
 * @return  MP_LT when a is less than b.
 * @return  MP_EQ when a is equals b.
 */
static int _sp_cmp_abs(const sp_int* a, const sp_int* b)
{
    int ret = MP_EQ;

    /* Check number of words first. */
    if (a->used > b->used) {
        ret = MP_GT;
    }
    else if (a->used < b->used) {
        ret = MP_LT;
    }
    else {
        int i;

        /* Starting from most significant word, compare words.
         * Stop when different and set comparison return.
         */
        for (i = (int)(a->used - 1); i >= 0; i--) {
            if (a->dp[i] > b->dp[i]) {
                ret = MP_GT;
                break;
            }
            else if (a->dp[i] < b->dp[i]) {
                ret = MP_LT;
                break;
            }
        }
        /* If we made to the end then ret is MP_EQ from initialization. */
    }

    return ret;
}
#endif

#if defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_PUBLIC_ONLY)
/* Compare absolute value of two multi-precision numbers.
 *
 * Pointers are compared such that NULL is less than not NULL.
 *
 * @param  [in]  a  SP integer.
 * @param  [in]  b  SP integer.
 *
 * @return  MP_GT when a is greater than b.
 * @return  MP_LT when a is less than b.
 * @return  MP_EQ when a equals b.
 */
int sp_cmp_mag(const sp_int* a, const sp_int* b)
{
    int ret;

    /* Do pointer checks first. Both NULL returns equal. */
    if (a == b) {
        ret = MP_EQ;
    }
    /* Nothing is smaller than something. */
    else if (a == NULL) {
        ret = MP_LT;
    }
    /* Something is larger than nothing. */
    else if (b == NULL) {
        ret = MP_GT;
    }
    else
    {
        /* Compare values - a and b are not NULL. */
        ret = _sp_cmp_abs(a, b);
    }

    return ret;
}
#endif

#if defined(WOLFSSL_SP_MATH_ALL) || defined(HAVE_ECC) || !defined(NO_DSA) || \
    defined(OPENSSL_EXTRA) || !defined(NO_DH) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY))
/* Compare two multi-precision numbers.
 *
 * Assumes a and b are not NULL.
 *
 * @param  [in]  a  SP integer.
 * @param  [in]  a  SP integer.
 *
 * @return  MP_GT when a is greater than b.
 * @return  MP_LT when a is less than b.
 * @return  MP_EQ when a is equals b.
 */
static int _sp_cmp(const sp_int* a, const sp_int* b)
{
    int ret;

#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Check sign first. */
    if (a->sign > b->sign) {
        ret = MP_LT;
    }
    else if (a->sign < b->sign) {
        ret = MP_GT;
    }
    else /* (a->sign == b->sign) */ {
#endif
        /* Compare values. */
        ret = _sp_cmp_abs(a, b);
#ifdef WOLFSSL_SP_INT_NEGATIVE
        if (a->sign == MP_NEG) {
            /* MP_GT = 1, MP_LT = -1, MP_EQ = 0
             * Swapping MP_GT and MP_LT results.
             */
            ret = -ret;
        }
    }
#endif

    return ret;
}
#endif

#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DSA) || defined(HAVE_ECC) || !defined(NO_DH) || \
    defined(WOLFSSL_SP_MATH_ALL)
/* Compare two multi-precision numbers.
 *
 * Pointers are compared such that NULL is less than not NULL.
 *
 * @param  [in]  a  SP integer.
 * @param  [in]  a  SP integer.
 *
 * @return  MP_GT when a is greater than b.
 * @return  MP_LT when a is less than b.
 * @return  MP_EQ when a is equals b.
 */
int sp_cmp(const sp_int* a, const sp_int* b)
{
    int ret;

    /* Check pointers first. Both NULL returns equal. */
    if (a == b) {
        ret = MP_EQ;
    }
    /* Nothing is smaller than something. */
    else if (a == NULL) {
        ret = MP_LT;
    }
    /* Something is larger than nothing. */
    else if (b == NULL) {
        ret = MP_GT;
    }
    else
    {
        /* Compare values - a and b are not NULL. */
        ret = _sp_cmp(a, b);
    }

    return ret;
}
#endif

/*************************
 * Bit check/set functions
 *************************/

#if (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    (defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)) || \
    defined(OPENSSL_EXTRA)
/* Check if a bit is set
 *
 * When a is NULL, result is 0.
 *
 * @param  [in]  a  SP integer.
 * @param  [in]  b  Bit position to check.
 *
 * @return  0 when bit is not set.
 * @return  1 when bit is set.
 */
int sp_is_bit_set(const sp_int* a, unsigned int b)
{
    int ret = 0;
    /* Index of word. */
    unsigned int i = b >> SP_WORD_SHIFT;

    /* Check parameters. */
    if ((a != NULL) && (i < a->used)) {
        /* Shift amount to get bit down to index 0. */
        unsigned int s = b & SP_WORD_MASK;

        /* Get and mask bit. */
        ret = (int)((a->dp[i] >> s) & (sp_int_digit)1);
    }

    return ret;
}
#endif /* (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) ||
        * (WOLFSSL_SP_MATH_ALL && HAVE_ECC) */

/* Count the number of bits in the multi-precision number.
 *
 * When a is NULL, result is 0.
 *
 * @param  [in]  a  SP integer.
 *
 * @return  Number of bits in the SP integer value.
 */
int sp_count_bits(const sp_int* a)
{
    int n = -1;

    /* Check parameter. */
    if ((a != NULL) && (a->used > 0)) {
        /* Get index of last word. */
        n = (int)(a->used - 1);
        /* Don't count leading zeros. */
        while ((n >= 0) && (a->dp[n] == 0)) {
            n--;
        }
    }

    /* -1 indicates SP integer value was zero. */
    if (n < 0) {
        n = 0;
    }
    else {
        /* Get the most significant word. */
        sp_int_digit d = a->dp[n];
        /* Count of bits up to last word. */
        n *= SP_WORD_SIZE;

    #ifdef SP_ASM_HI_BIT_SET_IDX
        {
            sp_int_digit hi;
            /* Get index of highest set bit. */
            SP_ASM_HI_BIT_SET_IDX(d, hi);
            /* Add bits up to and including index. */
            n += (int)hi + 1;
        }
    #elif defined(SP_ASM_LZCNT)
        {
            sp_int_digit lz;
            /* Count number of leading zeros in highest non-zero digit. */
            SP_ASM_LZCNT(d, lz);
            /* Add non-leading zero bits count. */
            n += SP_WORD_SIZE - (int)lz;
        }
    #else
        /* Check if top word has more than half the bits set. */
        if (d > SP_HALF_MAX) {
            /* Set count to a full last word. */
            n += SP_WORD_SIZE;
            /* Don't count leading zero bits. */
            while ((d & ((sp_int_digit)1 << (SP_WORD_SIZE - 1))) == 0) {
                n--;
                d <<= 1;
            }
        }
        else {
            /* Add to count until highest set bit is shifted out. */
            while (d != 0) {
                n++;
                d >>= 1;
            }
        }
    #endif
    }

    return n;
}

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH) || \
    (defined(HAVE_ECC) && defined(FP_ECC)) || \
    (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN))

/* Number of entries in array of number of least significant zero bits. */
#define SP_LNZ_CNT      16
/* Number of bits the array checks. */
#define SP_LNZ_BITS     4
/* Mask to apply to check with array. */
#define SP_LNZ_MASK     0xf
/* Number of least significant zero bits in first SP_LNZ_CNT numbers. */
static const int sp_lnz[SP_LNZ_CNT] = {
   4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/* Count the number of least significant zero bits.
 *
 * When a is not NULL, result is 0.
 *
 * @param  [in]   a  SP integer to use.
 *
 * @return  Number of least significant zero bits.
 */
#if !defined(HAVE_ECC) || !defined(HAVE_COMP_KEY)
static
#endif /* !HAVE_ECC || HAVE_COMP_KEY */
int sp_cnt_lsb(const sp_int* a)
{
    unsigned int bc = 0;

    /* Check for number with a value. */
    if ((a != NULL) && (!sp_iszero(a))) {
        unsigned int i;
        unsigned int j;

        /* Count least significant words that are zero. */
        for (i = 0; i < a->used && a->dp[i] == 0; i++, bc += SP_WORD_SIZE) {
        }

        /* Use 4-bit table to get count. */
        for (j = 0; j < SP_WORD_SIZE; j += SP_LNZ_BITS) {
            /* Get number of lesat significant 0 bits in nibble. */
            int cnt = sp_lnz[(a->dp[i] >> j) & SP_LNZ_MASK];
            /* Done if not all 4 bits are zero. */
            if (cnt != 4) {
                /* Add checked bits and count in last 4 bits checked. */
                bc += j + (unsigned int)cnt;
                break;
            }
        }
    }

    return (int)bc;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH || (HAVE_ECC && FP_ECC) */

#if !defined(WOLFSSL_RSA_VERIFY_ONLY) || defined(WOLFSSL_ASN_TEMPLATE) || \
    (defined(WOLFSSL_SP_MATH_ALL) && !defined(NO_ASN))
/* Determine if the most significant byte of the encoded multi-precision number
 * has the top bit set.
 *
 * When a is NULL, result is 0.
 *
 * @param  [in]  a  SP integer.
 *
 * @return  1 when the top bit of top byte is set.
 * @return  0 when the top bit of top byte is not set.
 */
int sp_leading_bit(const sp_int* a)
{
    int bit = 0;

    /* Check if we have a number and value to use. */
    if ((a != NULL) && (a->used > 0)) {
        /* Get top word. */
        sp_int_digit d = a->dp[a->used - 1];

    #if SP_WORD_SIZE > 8
        /* Remove bottom 8 bits until highest 8 bits left. */
        while (d > (sp_int_digit)0xff) {
            d >>= 8;
        }
    #endif
        /* Get the highest bit of the 8-bit value. */
        bit = (int)(d >> 7);
    }

    return bit;
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY */

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || \
    !defined(NO_RSA)
/* Set one bit of a: a |= 1 << i
 * The field 'used' is updated in a.
 *
 * @param  [in,out]  a  SP integer to set bit into.
 * @param  [in]      i  Index of bit to set.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL, index is negative or index is too large.
 */
int sp_set_bit(sp_int* a, int i)
{
    int err = MP_OKAY;
    /* Get index of word to set. */
    unsigned int w = (unsigned int)(i >> SP_WORD_SHIFT);

    /* Check for valid number and and space for bit. */
    if ((a == NULL) || (i < 0) || (w >= a->size)) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Amount to shift up to set bit in word. */
        unsigned int s = (unsigned int)(i & (SP_WORD_SIZE - 1));
        unsigned int j;

        /* Set to zero all unused words up to and including word to have bit
         * set.
         */
        for (j = a->used; j <= w; j++) {
            a->dp[j] = 0;
        }
        /* Set bit in word. */
        a->dp[w] |= (sp_int_digit)1 << s;
        /* Update used if necessary */
        if (a->used <= w) {
            a->used = w + 1;
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH || HAVE_ECC ||
        * WOLFSSL_KEY_GEN || OPENSSL_EXTRA || !NO_RSA */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WOLFSSL_KEY_GEN) || !defined(NO_DH)
/* Exponentiate 2 to the power of e: a = 2^e
 * This is done by setting the 'e'th bit.
 *
 * @param  [out]  a  SP integer to hold result.
 * @param  [in]   e  Exponent.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL, e is negative or 2^exponent is too large.
 */
int sp_2expt(sp_int* a, int e)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (e < 0)) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Set number to zero and then set bit. */
        _sp_zero(a);
        err = sp_set_bit(a, e);
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) ||
        * WOLFSSL_KEY_GEN || !NO_DH */

/**********************
 * Digit/Long functions
 **********************/

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_RSA) || !defined(NO_DH) || \
    defined(HAVE_ECC)
/* Set the multi-precision number to be the value of the digit.
 *
 * @param  [out]  a  SP integer to become number.
 * @param  [in]   d  Digit to be set.
 */
static void _sp_set(sp_int* a, sp_int_digit d)
{
    /* Use sp_int_minimal to support allocated byte arrays as sp_ints. */
    sp_int_minimal* am = (sp_int_minimal*)a;

    am->dp[0] = d;
    /* d == 0 => used = 0, d > 0 => used = 1 */
    am->used = (d > 0);
#ifdef WOLFSSL_SP_INT_NEGATIVE
    am->sign = MP_ZPOS;
#endif
}

/* Set the multi-precision number to be the value of the digit.
 *
 * @param  [out]  a  SP integer to become number.
 * @param  [in]   d  Digit to be set.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL.
 */
int sp_set(sp_int* a, sp_int_digit d)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if (a == NULL) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        _sp_set(a, d);
    }

    return err;
}
#endif

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_RSA) || defined(OPENSSL_EXTRA)
/* Set a number into the multi-precision number.
 *
 * Number may be larger than the size of a digit.
 *
 * @param  [out]  a  SP integer to set.
 * @param  [in]   n  Long value to set.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL.
 */
int sp_set_int(sp_int* a, unsigned long n)
{
    int err = MP_OKAY;

    if (a == NULL) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
    #if SP_WORD_SIZE < SP_ULONG_BITS
        /* Assign if value first in one word. */
        if (n <= (sp_int_digit)SP_DIGIT_MAX) {
    #endif
            a->dp[0] = (sp_int_digit)n;
            a->used = (n != 0);
    #if SP_WORD_SIZE < SP_ULONG_BITS
        }
        else {
            unsigned int i;

            /* Assign value word by word. */
            for (i = 0; (i < a->size) && (n > 0); i++,n >>= SP_WORD_SIZE) {
                a->dp[i] = (sp_int_digit)n;
            }
            /* Update number of words used. */
            a->used = i;
            /* Check for overflow. */
            if ((i == a->size) && (n != 0)) {
                err = MP_VAL;
            }
        }
    #endif
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        a->sign = MP_ZPOS;
    #endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || !NO_RSA  */

#if defined(WOLFSSL_SP_MATH_ALL) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || defined(HAVE_ECC)
/* Compare a one digit number with a multi-precision number.
 *
 * When a is NULL, MP_LT is returned.
 *
 * @param  [in]  a  SP integer to compare.
 * @param  [in]  d  Digit to compare with.
 *
 * @return  MP_GT when a is greater than d.
 * @return  MP_LT when a is less than d.
 * @return  MP_EQ when a is equals d.
 */
int sp_cmp_d(const sp_int* a, sp_int_digit d)
{
    int ret = MP_EQ;

    /* No SP integer is always less - even when d is zero. */
    if (a == NULL) {
        ret = MP_LT;
    }
    else
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Check sign first. */
    if (a->sign == MP_NEG) {
        ret = MP_LT;
    }
    else
#endif
    {
        /* Check if SP integer as more than one word. */
        if (a->used > 1) {
            ret = MP_GT;
        }
        /* Special case for zero. */
        else if (a->used == 0) {
            if (d != 0) {
                ret = MP_LT;
            }
            /* ret initialized to equal. */
        }
        else {
            /* The single word in the SP integer can now be compared with d. */
            if (a->dp[0] > d) {
                ret = MP_GT;
            }
            else if (a->dp[0] < d) {
                ret = MP_LT;
            }
            /* ret initialized to equal. */
        }
    }

    return ret;
}
#endif

#if defined(WOLFSSL_SP_ADD_D) || (defined(WOLFSSL_SP_INT_NEGATIVE) && \
    defined(WOLFSSL_SP_SUB_D)) || defined(WOLFSSL_SP_READ_RADIX_10)
/* Add a one digit number to the multi-precision number.
 *
 * @param  [in]   a  SP integer be added to.
 * @param  [in]   d  Digit to add.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when result is too large for fixed size dp array.
 */
static int _sp_add_d(const sp_int* a, sp_int_digit d, sp_int* r)
{
    int err = MP_OKAY;

    /* Special case of zero means we want result to have a digit when not adding
     * zero. */
    if (a->used == 0) {
        r->dp[0] = d;
        r->used = (d > 0);
    }
    else {
        unsigned int i = 0;
        sp_int_digit a0 = a->dp[0];

        /* Set used of result - updated if overflow seen. */
        r->used = a->used;

        r->dp[0] = a0 + d;
        /* Check for carry. */
        if (r->dp[0] < a0) {
            /* Do carry through all words. */
            for (++i; i < a->used; i++) {
                r->dp[i] = a->dp[i] + 1;
                if (r->dp[i] != 0) {
                   break;
                }
            }
            /* Add another word if required. */
            if (i == a->used) {
                /* Check result has enough space for another word. */
                if (i < r->size) {
                    r->used++;
                    r->dp[i] = 1;
                }
                else {
                    err = MP_VAL;
                }
            }
        }
        /* When result is not the same as input, copy rest of digits. */
        if ((err == MP_OKAY) && (r != a)) {
            /* Copy any words that didn't update with carry. */
            for (++i; i < a->used; i++) {
                r->dp[i] = a->dp[i];
            }
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_ADD_D || (WOLFSSL_SP_INT_NEGATIVE && WOLFSSL_SP_SUB_D) ||
        * defined(WOLFSSL_SP_READ_RADIX_10) */

#if (defined(WOLFSSL_SP_INT_NEGATIVE) && defined(WOLFSSL_SP_ADD_D)) || \
    defined(WOLFSSL_SP_SUB_D) || defined(WOLFSSL_SP_INVMOD) || \
    defined(WOLFSSL_SP_INVMOD_MONT_CT) || (defined(WOLFSSL_SP_PRIME_GEN) && \
    !defined(WC_NO_RNG))
/* Sub a one digit number from the multi-precision number.
 *
 * @param  [in]   a  SP integer be subtracted from.
 * @param  [in]   d  Digit to subtract.
 * @param  [out]  r  SP integer to store result in.
 */
static void _sp_sub_d(const sp_int* a, sp_int_digit d, sp_int* r)
{
    /* Set result used to be same as input. Updated with clamp. */
    r->used = a->used;
    /* Only possible when not handling negatives. */
    if (a->used == 0) {
        /* Set result to zero as no negative support. */
        r->dp[0] = 0;
    }
    else {
        unsigned int i = 0;
        sp_int_digit a0 = a->dp[0];

        r->dp[0] = a0 - d;
        /* Check for borrow. */
        if (r->dp[0] > a0) {
            /* Do borrow through all words. */
            for (++i; i < a->used; i++) {
                r->dp[i] = a->dp[i] - 1;
                if (r->dp[i] != SP_DIGIT_MAX) {
                   break;
                }
            }
        }
        /* When result is not the same as input, copy rest of digits. */
        if (r != a) {
            /* Copy any words that didn't update with borrow. */
            for (++i; i < a->used; i++) {
                r->dp[i] = a->dp[i];
            }
        }
        /* Remove leading zero words. */
        sp_clamp(r);
    }
}
#endif /* (WOLFSSL_SP_INT_NEGATIVE && WOLFSSL_SP_ADD_D) || WOLFSSL_SP_SUB_D
        * WOLFSSL_SP_INVMOD || WOLFSSL_SP_INVMOD_MONT_CT ||
        * WOLFSSL_SP_PRIME_GEN */

#ifdef WOLFSSL_SP_ADD_D
/* Add a one digit number to the multi-precision number.
 *
 * @param  [in]   a  SP integer be added to.
 * @param  [in]   d  Digit to add.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when result is too large for fixed size dp array.
 */
int sp_add_d(const sp_int* a, sp_int_digit d, sp_int* r)
{
    int err = MP_OKAY;

    /* Check validity of parameters. */
    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }

#ifndef WOLFSSL_SP_INT_NEGATIVE
    /* Check for space in result especially when carry adds a new word. */
    if ((err == MP_OKAY) && (a->used + 1 > r->size)) {
         err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Positive only so just use internal function. */
        err = _sp_add_d(a, d, r);
    }
#else
    /* Check for space in result especially when carry adds a new word. */
    if ((err == MP_OKAY) && (a->sign == MP_ZPOS) && (a->used + 1 > r->size)) {
         err = MP_VAL;
    }
    /* Check for space in result - no carry but borrow possible. */
    if ((err == MP_OKAY) && (a->sign == MP_NEG) && (a->used > r->size)) {
         err = MP_VAL;
    }
    if (err == MP_OKAY) {
        if (a->sign == MP_ZPOS) {
            /* Positive, so use internal function. */
            r->sign = MP_ZPOS;
            err = _sp_add_d(a, d, r);
        }
        else if ((a->used > 1) || (a->dp[0] > d)) {
            /* Negative value bigger than digit so subtract digit. */
            r->sign = MP_NEG;
            _sp_sub_d(a, d, r);
        }
        else {
            /* Negative value smaller or equal to digit. */
            r->sign = MP_ZPOS;
            /* Subtract negative value from digit. */
            r->dp[0] = d - a->dp[0];
            /* Result is a digit equal to or greater than zero. */
            r->used = (r->dp[0] > 0);
        }
    }
#endif

    return err;
}
#endif /* WOLFSSL_SP_ADD_D */

#ifdef WOLFSSL_SP_SUB_D
/* Sub a one digit number from the multi-precision number.
 *
 * @param  [in]   a  SP integer be subtracted from.
 * @param  [in]   d  Digit to subtract.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL.
 */
int sp_sub_d(const sp_int* a, sp_int_digit d, sp_int* r)
{
    int err = MP_OKAY;

    /* Check validity of parameters. */
    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
#ifndef WOLFSSL_SP_INT_NEGATIVE
    /* Check for space in result. */
    if ((err == MP_OKAY) && (a->used > r->size)) {
         err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Positive only so just use internal function. */
        _sp_sub_d(a, d, r);
    }
#else
    /* Check for space in result especially when borrow adds a new word. */
    if ((err == MP_OKAY) && (a->sign == MP_NEG) && (a->used + 1 > r->size)) {
         err = MP_VAL;
    }
    /* Check for space in result - no carry but borrow possible. */
    if ((err == MP_OKAY) && (a->sign == MP_ZPOS) && (a->used > r->size)) {
         err = MP_VAL;
    }
    if (err == MP_OKAY) {
        if (a->sign == MP_NEG) {
            /* Subtracting from negative use internal add. */
            r->sign = MP_NEG;
            err = _sp_add_d(a, d, r);
        }
        else if ((a->used > 1) || (a->dp[0] >= d)) {
            /* Positive number greater than or equal to digit - subtract digit.
             */
            r->sign = MP_ZPOS;
            _sp_sub_d(a, d, r);
        }
        else {
            /* Positive value smaller than digit. */
            r->sign = MP_NEG;
            /* Subtract positive value from digit. */
            r->dp[0] = d - a->dp[0];
            /* Result is a digit equal to or greater than zero. */
            r->used = 1;
        }
    }
#endif

    return err;
}
#endif /* WOLFSSL_SP_SUB_D */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WOLFSSL_SP_SMALL) && (defined(WOLFSSL_SP_MATH_ALL) || \
    !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))) || \
    (defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA))
/* Multiply a by digit n and put result into r shifting up o digits.
 *   r = (a * n) << (o * SP_WORD_SIZE)
 *
 * @param  [in]   a  SP integer to be multiplied.
 * @param  [in]   d  SP digit to multiply by.
 * @param  [out]  r  SP integer result.
 * @param  [in]   o  Number of digits to move result up by.
 * @return  MP_OKAY on success.
 * @return  MP_VAL when result is too large for sp_int.
 */
static int _sp_mul_d(const sp_int* a, sp_int_digit d, sp_int* r, unsigned int o)
{
    int err = MP_OKAY;
    unsigned int i;
#ifndef SQR_MUL_ASM
    sp_int_word t = 0;
#else
    sp_int_digit l = 0;
    sp_int_digit h = 0;
#endif

#ifdef WOLFSSL_SP_SMALL
    /* Zero out offset words. */
    for (i = 0; i < o; i++) {
        r->dp[i] = 0;
    }
#else
    /* Don't use the offset. Only when doing small code size div. */
    (void)o;
#endif

    /* Multiply each word of a by n. */
    for (i = 0; i < a->used; i++, o++) {
    #ifndef SQR_MUL_ASM
        /* Add product to top word of previous result. */
        t += (sp_int_word)a->dp[i] * d;
        /* Store low word. */
        r->dp[o] = (sp_int_digit)t;
        /* Move top word down. */
        t >>= SP_WORD_SIZE;
    #else
        /* Multiply and add into low and high from previous result.
         * No overflow of possible with add. */
        SP_ASM_MUL_ADD_NO(l, h, a->dp[i], d);
        /* Store low word. */
        r->dp[o] = l;
        /* Move high word into low word and set high word to 0. */
        l = h;
        h = 0;
    #endif
    }

    /* Check whether new word to be appended to result. */
#ifndef SQR_MUL_ASM
    if (t > 0)
#else
    if (l > 0)
#endif
    {
        /* Validate space available in result. */
        if (o == r->size) {
            err = MP_VAL;
        }
        else {
            /* Store new top word. */
        #ifndef SQR_MUL_ASM
            r->dp[o++] = (sp_int_digit)t;
        #else
            r->dp[o++] = l;
        #endif
        }
    }
    /* Update number of words in result. */
    r->used = o;
    /* In case n is zero. */
    sp_clamp(r);

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) ||
        *  WOLFSSL_SP_SMALL || (WOLFSSL_KEY_GEN && !NO_RSA) */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    (defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA))
/* Multiply a by digit n and put result into r. r = a * n
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   n  Digit to multiply by.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or b is NULL, or a has maximum number of digits used.
 */
int sp_mul_d(const sp_int* a, sp_int_digit d, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Check space for product result - _sp_mul_d checks when new word added. */
    if ((err == MP_OKAY) && (a->used > r->size)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        err = _sp_mul_d(a, d, r, 0);
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        /* Update sign. */
        if (d == 0) {
            r->sign = MP_ZPOS;
        }
        else {
            r->sign = a->sign;
        }
    #endif
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) ||
        * (WOLFSSL_KEY_GEN && !NO_RSA) */

/* Predefine complicated rules of when to compile in sp_div_d and sp_mod_d. */
#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
    defined(OPENSSL_EXTRA) || defined(WC_MP_TO_RADIX)
#define WOLFSSL_SP_DIV_D
#endif
#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || \
    (defined(HAVE_ECC) && (defined(FP_ECC) || defined(HAVE_COMP_KEY))) || \
    (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN))
#define WOLFSSL_SP_MOD_D
#endif

#if (defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
     (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
      !defined(WOLFSSL_RSA_PUBLIC_ONLY))) || \
    defined(WOLFSSL_SP_DIV_D) || defined(WOLFSSL_SP_MOD_D)
#ifndef SP_ASM_DIV_WORD
/* Divide a two digit number by a digit number and return. (hi | lo) / d
 *
 * @param  [in]  hi  SP integer digit. High digit of the dividend.
 * @param  [in]  lo  SP integer digit. Lower digit of the dividend.
 * @param  [in]  d   SP integer digit. Number to divide by.
 * @return  The division result.
 */
static WC_INLINE sp_int_digit sp_div_word(sp_int_digit hi, sp_int_digit lo,
    sp_int_digit d)
{
#ifdef WOLFSSL_SP_DIV_WORD_HALF
    sp_int_digit r;

    /* Trial division using half of the bits in d. */

    /* Check for shortcut when no high word set. */
    if (hi == 0) {
        r = lo / d;
    }
    else {
        /* Half the bits of d. */
        sp_int_digit divh = d >> SP_HALF_SIZE;
        /* Number to divide in one value. */
        sp_int_word w = ((sp_int_word)hi << SP_WORD_SIZE) | lo;
        sp_int_word trial;
        sp_int_digit r2;

        /* Calculation for top SP_WORD_SIZE / 2 bits of dividend. */
        /* Divide high word by top half of divisor. */
        r = hi / divh;
        /* When result too big then assume only max value. */
        if (r > SP_HALF_MAX) {
            r = SP_HALF_MAX;
        }
        /* Shift up result for trial division calucation. */
        r <<= SP_HALF_SIZE;
        /* Calculate trial value. */
        trial = r * (sp_int_word)d;
        /* Decrease r while trial is too big. */
        while (trial > w) {
            r -= (sp_int_digit)1 << SP_HALF_SIZE;
            trial -= (sp_int_word)d << SP_HALF_SIZE;
        }
        /* Subtract trial. */
        w -= trial;

        /* Calculation for remaining second SP_WORD_SIZE / 2 bits. */
        /* Divide top SP_WORD_SIZE of remainder by top half of divisor. */
        r2 = ((sp_int_digit)(w >> SP_HALF_SIZE)) / divh;
        /* Calculate trial value. */
        trial = r2 * (sp_int_word)d;
        /* Decrease r while trial is too big. */
        while (trial > w) {
            r2--;
            trial -= d;
        }
        /* Subtract trial. */
        w -= trial;
        /* Update result. */
        r += r2;

        /* Calculation for remaining bottom SP_WORD_SIZE bits. */
        r2 = ((sp_int_digit)w) / d;
        /* Update result. */
        r += r2;
    }

    return r;
#else
    sp_int_word w;
    sp_int_digit r;

    /* Use built-in divide. */
    w = ((sp_int_word)hi << SP_WORD_SIZE) | lo;
    w /= d;
    r = (sp_int_digit)w;

    return r;
#endif /* WOLFSSL_SP_DIV_WORD_HALF */
}
#endif /* !SP_ASM_DIV_WORD */
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if (defined(WOLFSSL_SP_DIV_D) || defined(WOLFSSL_SP_MOD_D)) && \
    !defined(WOLFSSL_SP_SMALL)

#if SP_WORD_SIZE == 64
    /* 2^64 / 3 */
    #define SP_DIV_3_CONST      0x5555555555555555L
    /* 2^64 / 10 */
    #define SP_DIV_10_CONST     0x1999999999999999L
#elif SP_WORD_SIZE == 32
    /* 2^32 / 3 */
    #define SP_DIV_3_CONST      0x55555555
    /* 2^32 / 10 */
    #define SP_DIV_10_CONST     0x19999999
#elif SP_WORD_SIZE == 16
    /* 2^16 / 3 */
    #define SP_DIV_3_CONST      0x5555
    /* 2^16 / 10 */
    #define SP_DIV_10_CONST     0x1999
#elif SP_WORD_SIZE == 8
    /* 2^8 / 3 */
    #define SP_DIV_3_CONST      0x55
    /* 2^8 / 10 */
    #define SP_DIV_10_CONST     0x19
#endif

#if !defined(WOLFSSL_SP_SMALL) && (SP_WORD_SIZE < 64)
/* Divide by 3: r = a / 3 and rem = a % 3
 *
 * Used in checking prime: (a % 3) == 0?.
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [out]  r    SP integer that is the quotient. May be NULL.
 * @param  [out]  rem  SP integer that is the remainder. May be NULL.
 */
static void _sp_div_3(const sp_int* a, sp_int* r, sp_int_digit* rem)
{
#ifndef SQR_MUL_ASM
    sp_int_word t;
    sp_int_digit tt;
#else
    sp_int_digit l = 0;
    sp_int_digit tt = 0;
    sp_int_digit t = SP_DIV_3_CONST;
    sp_int_digit lm = 0;
    sp_int_digit hm = 0;
#endif
    sp_int_digit tr = 0;
    /* Quotient fixup. */
    static const unsigned char sp_r6[6] = { 0, 0, 0, 1, 1, 1 };
    /* Remainder fixup. */
    static const unsigned char sp_rem6[6] = { 0, 1, 2, 0, 1, 2 };

    /* Check whether only mod value needed. */
    if (r == NULL) {
        unsigned int i;

        /*    2^2 mod 3 = 4 mod 3 = 1.
         * => 2^(2*n) mod 3 = (2^2 mod 3)^n mod 3 = 1^n mod 3 = 1
         * => (2^(2*n) * x) mod 3 = (2^(2*n) mod 3) * (x mod 3) = x mod 3
         *
         * Calculate mod 3 on sum of digits as SP_WORD_SIZE is a multiple of 2.
         */
    #ifndef SQR_MUL_ASM
        t = 0;
        /* Sum the digits. */
        for (i = 0; i < a->used; i++) {
            t += a->dp[i];
        }
        /* Sum digits of sum. */
        t = (t >> SP_WORD_SIZE) + (t & SP_MASK);
        /* Get top digit after multipling by (2^SP_WORD_SIZE) / 3. */
        tt = (sp_int_digit)((t * SP_DIV_3_CONST) >> SP_WORD_SIZE);
        /* Subtract trial division. */
        tr = (sp_int_digit)(t - (sp_int_word)tt * 3);
    #else
        /* Sum the digits. */
        for (i = 0; i < a->used; i++) {
            SP_ASM_ADDC_REG(l, tr, a->dp[i]);
        }
        /* Sum digits of sum - can get carry. */
        SP_ASM_ADDC_REG(l, tt, tr);
        /* Multiply digit by (2^SP_WORD_SIZE) / 3. */
        SP_ASM_MUL(lm, hm, l, t);
        /* Add remainder multiplied by (2^SP_WORD_SIZE) / 3 to top digit. */
        hm += tt * SP_DIV_3_CONST;
        /* Subtract trial division from digit. */
        tr = l - (hm * 3);
    #endif
        /* tr is 0..5 but need 0..2 */
        /* Fix up remainder. */
        tr = sp_rem6[tr];
        *rem = tr;
    }
    /* At least result needed - remainder is calculated anyway. */
    else {
        int i;

        /* Divide starting at most significant word down to least. */
        for (i = (int)(a->used - 1); i >= 0; i--) {
    #ifndef SQR_MUL_ASM
            /* Combine remainder from last operation with this word. */
            t = ((sp_int_word)tr << SP_WORD_SIZE) | a->dp[i];
            /* Get top digit after multipling by (2^SP_WORD_SIZE) / 3. */
            tt = (sp_int_digit)((t * SP_DIV_3_CONST) >> SP_WORD_SIZE);
            /* Subtract trial division. */
            tr = (sp_int_digit)(t - (sp_int_word)tt * 3);
    #else
            /* Multiply digit by (2^SP_WORD_SIZE) / 3. */
            SP_ASM_MUL(l, tt, a->dp[i], t);
            /* Add remainder multiplied by (2^SP_WORD_SIZE) / 3 to top digit. */
            tt += tr * SP_DIV_3_CONST;
            /* Subtract trial division from digit. */
            tr = a->dp[i] - (tt * 3);
    #endif
            /* tr is 0..5 but need 0..2 */
            /* Fix up result. */
            tt += sp_r6[tr];
            /* Fix up remainder. */
            tr = sp_rem6[tr];
            /* Store result of digit divided by 3. */
            r->dp[i] = tt;
        }

        /* Set the used amount to maximal amount. */
        r->used = a->used;
        /* Remove leading zeros. */
        sp_clamp(r);
        /* Return remainder if required. */
        if (rem != NULL) {
            *rem = tr;
        }
    }
}
#endif /* !(WOLFSSL_SP_SMALL && (SP_WORD_SIZE < 64) */

/* Divide by 10: r = a / 10 and rem = a % 10
 *
 * Used when writing with a radix of 10 - decimal number.
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [out]  r    SP integer that is the quotient. May be NULL.
 * @param  [out]  rem  SP integer that is the remainder. May be NULL.
 */
static void _sp_div_10(const sp_int* a, sp_int* r, sp_int_digit* rem)
{
    int i;
#ifndef SQR_MUL_ASM
    sp_int_word t;
    sp_int_digit tt;
#else
    sp_int_digit l = 0;
    sp_int_digit tt = 0;
    sp_int_digit t = SP_DIV_10_CONST;
#endif
    sp_int_digit tr = 0;

    /* Check whether only mod value needed. */
    if (r == NULL) {
        /* Divide starting at most significant word down to least. */
        for (i = (int)(a->used - 1); i >= 0; i--) {
    #ifndef SQR_MUL_ASM
            /* Combine remainder from last operation with this word. */
            t = ((sp_int_word)tr << SP_WORD_SIZE) | a->dp[i];
            /* Get top digit after multipling by (2^SP_WORD_SIZE) / 10. */
            tt = (sp_int_digit)((t * SP_DIV_10_CONST) >> SP_WORD_SIZE);
            /* Subtract trial division. */
            tr = (sp_int_digit)(t - (sp_int_word)tt * 10);
    #else
            /* Multiply digit by (2^SP_WORD_SIZE) / 10. */
            SP_ASM_MUL(l, tt, a->dp[i], t);
            /* Add remainder multiplied by (2^SP_WORD_SIZE) / 10 to top digit.
             */
            tt += tr * SP_DIV_10_CONST;
            /* Subtract trial division from digit. */
            tr = a->dp[i] - (tt * 10);
    #endif
            /* tr is 0..99 but need 0..9 */
            /* Fix up remainder. */
            tr = tr % 10;
        }
        *rem = tr;
    }
    /* At least result needed - remainder is calculated anyway. */
    else {
        /* Divide starting at most significant word down to least. */
        for (i = (int)(a->used - 1); i >= 0; i--) {
    #ifndef SQR_MUL_ASM
            /* Combine remainder from last operation with this word. */
            t = ((sp_int_word)tr << SP_WORD_SIZE) | a->dp[i];
            /* Get top digit after multipling by (2^SP_WORD_SIZE) / 10. */
            tt = (sp_int_digit)((t * SP_DIV_10_CONST) >> SP_WORD_SIZE);
            /* Subtract trial division. */
            tr = (sp_int_digit)(t - (sp_int_word)tt * 10);
    #else
            /* Multiply digit by (2^SP_WORD_SIZE) / 10. */
            SP_ASM_MUL(l, tt, a->dp[i], t);
            /* Add remainder multiplied by (2^SP_WORD_SIZE) / 10 to top digit.
             */
            tt += tr * SP_DIV_10_CONST;
            /* Subtract trial division from digit. */
            tr = a->dp[i] - (tt * 10);
    #endif
            /* tr is 0..99 but need 0..9 */
            /* Fix up result. */
            tt += tr / 10;
            /* Fix up remainder. */
            tr %= 10;
            /* Store result of digit divided by 10. */
            r->dp[i] = tt;
        }

        /* Set the used amount to maximal amount. */
        r->used = a->used;
        /* Remove leading zeros. */
        sp_clamp(r);
        /* Return remainder if required. */
        if (rem != NULL) {
            *rem = tr;
        }
    }
}
#endif /* (WOLFSSL_SP_DIV_D || WOLFSSL_SP_MOD_D) && !WOLFSSL_SP_SMALL */

#if defined(WOLFSSL_SP_DIV_D) || defined(WOLFSSL_SP_MOD_D)
/* Divide by small number: r = a / d and rem = a % d
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [in]   d    Digit to divide by.
 * @param  [out]  r    SP integer that is the quotient. May be NULL.
 * @param  [out]  rem  SP integer that is the remainder. May be NULL.
 */
static void _sp_div_small(const sp_int* a, sp_int_digit d, sp_int* r,
    sp_int_digit* rem)
{
    int i;
#ifndef SQR_MUL_ASM
    sp_int_word t;
    sp_int_digit tt;
#else
    sp_int_digit l = 0;
    sp_int_digit tt = 0;
#endif
    sp_int_digit tr = 0;
    sp_int_digit m = SP_DIGIT_MAX / d;

#ifndef WOLFSSL_SP_SMALL
    /* Check whether only mod value needed. */
    if (r == NULL) {
        /* Divide starting at most significant word down to least. */
        for (i = (int)(a->used - 1); i >= 0; i--) {
        #ifndef SQR_MUL_ASM
            /* Combine remainder from last operation with this word. */
            t = ((sp_int_word)tr << SP_WORD_SIZE) | a->dp[i];
            /* Get top digit after multipling. */
            tt = (sp_int_digit)((t * m) >> SP_WORD_SIZE);
            /* Subtract trial division. */
            tr = (sp_int_digit)t - (sp_int_digit)(tt * d);
        #else
            /* Multiply digit. */
            SP_ASM_MUL(l, tt, a->dp[i], m);
            /* Add multiplied remainder to top digit. */
            tt += tr * m;
            /* Subtract trial division from digit. */
            tr = a->dp[i] - (tt * d);
        #endif
            /* tr < d * d */
            /* Fix up remainder. */
            tr = tr % d;
        }
        *rem = tr;
    }
    /* At least result needed - remainder is calculated anyway. */
    else
#endif /* !WOLFSSL_SP_SMALL */
    {
        /* Divide starting at most significant word down to least. */
        for (i = (int)(a->used - 1); i >= 0; i--) {
        #ifndef SQR_MUL_ASM
            /* Combine remainder from last operation with this word. */
            t = ((sp_int_word)tr << SP_WORD_SIZE) | a->dp[i];
            /* Get top digit after multipling. */
            tt = (sp_int_digit)((t * m) >> SP_WORD_SIZE);
            /* Subtract trial division. */
            tr = (sp_int_digit)t - (sp_int_digit)(tt * d);
        #else
            /* Multiply digit. */
            SP_ASM_MUL(l, tt, a->dp[i], m);
            /* Add multiplied remainder to top digit. */
            tt += tr * m;
            /* Subtract trial division from digit. */
            tr = a->dp[i] - (tt * d);
        #endif
            /* tr < d * d */
            /* Fix up result. */
            tt += tr / d;
            /* Fix up remainder. */
            tr %= d;
            /* Store result of dividing the digit. */
        #ifdef WOLFSSL_SP_SMALL
            if (r != NULL)
        #endif
            {
                r->dp[i] = tt;
            }
        }

    #ifdef WOLFSSL_SP_SMALL
        if (r != NULL)
    #endif
        {
            /* Set the used amount to maximal amount. */
            r->used = a->used;
            /* Remove leading zeros. */
            sp_clamp(r);
        }
        /* Return remainder if required. */
        if (rem != NULL) {
            *rem = tr;
        }
    }
}
#endif

#ifdef WOLFSSL_SP_DIV_D
/* Divide a multi-precision number by a digit size number and calculate
 * remainder.
 *   r = a / d; rem = a % d
 *
 * Use trial division algorithm.
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [in]   d    Digit to divide by.
 * @param  [out]  r    SP integer that is the quotient. May be NULL.
 * @param  [out]  rem  Digit that is the remainder. May be NULL.
 */
static void _sp_div_d(const sp_int* a, sp_int_digit d, sp_int* r,
    sp_int_digit* rem)
{
    int i;
#ifndef SQR_MUL_ASM
    sp_int_word w = 0;
#else
    sp_int_digit l;
    sp_int_digit h = 0;
#endif
    sp_int_digit t;

    /* Divide starting at most significant word down to least. */
    for (i = (int)(a->used - 1); i >= 0; i--) {
    #ifndef SQR_MUL_ASM
        /* Combine remainder from last operation with this word and divide. */
        t = sp_div_word((sp_int_digit)w, a->dp[i], d);
        /* Combine remainder from last operation with this word. */
        w = (w << SP_WORD_SIZE) | a->dp[i];
        /* Subtract to get modulo result. */
        w -= (sp_int_word)t * d;
    #else
        /* Get current word. */
        l = a->dp[i];
        /* Combine remainder from last operation with this word and divide. */
        t = sp_div_word(h, l, d);
        /* Subtract to get modulo result. */
        h = l - t * d;
    #endif
        /* Store result of dividing the digit. */
        if (r != NULL) {
            r->dp[i] = t;
        }
    }
    if (r != NULL) {
        /* Set the used amount to maximal amount. */
        r->used = a->used;
        /* Remove leading zeros. */
        sp_clamp(r);
    }

    /* Return remainder if required. */
    if (rem != NULL) {
    #ifndef SQR_MUL_ASM
        *rem = (sp_int_digit)w;
    #else
        *rem = h;
    #endif
    }
}

/* Divide a multi-precision number by a digit size number and calculate
 * remainder.
 *   r = a / d; rem = a % d
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [in]   d    Digit to divide by.
 * @param  [out]  r    SP integer that is the quotient. May be NULL.
 * @param  [out]  rem  Digit that is the remainder. May be NULL.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL or d is 0.
 */
int sp_div_d(const sp_int* a, sp_int_digit d, sp_int* r, sp_int_digit* rem)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (d == 0)) {
        err = MP_VAL;
    }
    /* Check space for maximal sized result. */
    if ((err == MP_OKAY) && (r != NULL) && (a->used > r->size)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
#if !defined(WOLFSSL_SP_SMALL)
    #if SP_WORD_SIZE < 64
        if (d == 3) {
            /* Fast implementation for divisor of 3. */
            _sp_div_3(a, r, rem);
        }
        else
    #endif
        if (d == 10) {
            /* Fast implementation for divisor of 10 - sp_todecimal(). */
            _sp_div_10(a, r, rem);
        }
        else
#endif
        if (d <= SP_HALF_MAX) {
            /* For small divisors. */
            _sp_div_small(a, d, r, rem);
        }
        else
        {
            _sp_div_d(a, d, r, rem);
        }

    #ifdef WOLFSSL_SP_INT_NEGATIVE
        if (r != NULL) {
            r->sign = a->sign;
        }
    #endif
    }

    return err;
}
#endif /* WOLFSSL_SP_DIV_D */

#ifdef WOLFSSL_SP_MOD_D
/* Calculate a modulo the digit d into r: r = a mod d
 *
 * @param  [in]   a  SP integer to reduce.
 * @param  [in]   d  Digit to that is the modulus.
 * @param  [out]  r  Digit that is the result.
 */
static void _sp_mod_d(const sp_int* a, const sp_int_digit d, sp_int_digit* r)
{
    int i;
#ifndef SQR_MUL_ASM
    sp_int_word w = 0;
#else
    sp_int_digit h = 0;
#endif

    /* Divide starting at most significant word down to least. */
    for (i = (int)(a->used - 1); i >= 0; i--) {
    #ifndef SQR_MUL_ASM
        /* Combine remainder from last operation with this word and divide. */
        sp_int_digit t = sp_div_word((sp_int_digit)w, a->dp[i], d);
        /* Combine remainder from last operation with this word. */
        w = (w << SP_WORD_SIZE) | a->dp[i];
        /* Subtract to get modulo result. */
        w -= (sp_int_word)t * d;
    #else
        /* Combine remainder from last operation with this word and divide. */
        sp_int_digit t = sp_div_word(h, a->dp[i], d);
        /* Subtract to get modulo result. */
        h = a->dp[i] - t * d;
    #endif
    }

    /* Return remainder. */
#ifndef SQR_MUL_ASM
    *r = (sp_int_digit)w;
#else
    *r = h;
#endif
}

/* Calculate a modulo the digit d into r: r = a mod d
 *
 * @param  [in]   a  SP integer to reduce.
 * @param  [in]   d  Digit to that is the modulus.
 * @param  [out]  r  Digit that is the result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL or d is 0.
 */
#if !defined(WOLFSSL_SP_MATH_ALL) && (!defined(HAVE_ECC) || \
    !defined(HAVE_COMP_KEY)) && !defined(OPENSSL_EXTRA)
static
#endif /* !WOLFSSL_SP_MATH_ALL && (!HAVE_ECC || !HAVE_COMP_KEY) */
int sp_mod_d(const sp_int* a, sp_int_digit d, sp_int_digit* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (r == NULL) || (d == 0)) {
        err = MP_VAL;
    }

#if 0
    sp_print(a, "a");
    sp_print_digit(d, "m");
#endif

    if (err == MP_OKAY) {
        /* Check whether d is a power of 2. */
        if ((d & (d - 1)) == 0) {
            if (a->used == 0) {
                *r = 0;
            }
            else {
                *r = a->dp[0] & (d - 1);
            }
        }
#if !defined(WOLFSSL_SP_SMALL)
    #if SP_WORD_SIZE < 64
        else if (d == 3) {
            /* Fast implementation for divisor of 3. */
            _sp_div_3(a, NULL, r);
        }
    #endif
        else if (d == 10) {
            /* Fast implementation for divisor of 10. */
            _sp_div_10(a, NULL, r);
        }
#endif
        else if (d <= SP_HALF_MAX) {
            /* For small divisors. */
            _sp_div_small(a, d, NULL, r);
        }
        else {
            _sp_mod_d(a, d, r);
        }

    #ifdef WOLFSSL_SP_INT_NEGATIVE
        if (a->sign == MP_NEG) {
            *r = d - *r;
        }
    #endif
    }

#if 0
    sp_print_digit(*r, "rmod");
#endif

    return err;
}
#endif /* WOLFSSL_SP_MOD_D */

#if defined(HAVE_ECC) || !defined(NO_DSA) || defined(OPENSSL_EXTRA) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))
/* Divides a by 2 and stores in r: r = a >> 1
 *
 * @param  [in]   a  SP integer to divide.
 * @param  [out]  r  SP integer to hold result.
 */
static void _sp_div_2(const sp_int* a, sp_int* r)
{
    int i;

    /* Shift down each word by 1 and include bottom bit of next at top. */
    for (i = 0; i < (int)a->used - 1; i++) {
        r->dp[i] = (a->dp[i] >> 1) | (a->dp[i+1] << (SP_WORD_SIZE - 1));
    }
    /* Last word only needs to be shifted down. */
    r->dp[i] = a->dp[i] >> 1;
    /* Set used to be all words seen. */
    r->used = (unsigned int)i + 1;
    /* Remove leading zeros. */
    sp_clamp(r);
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Same sign in result. */
    r->sign = a->sign;
#endif
}

#if defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)
/* Divides a by 2 and stores in r: r = a >> 1
 *
 * @param  [in]   a  SP integer to divide.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL.
 */
int sp_div_2(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;

    /* Only when a public API. */
    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure maximal size is supported by result. */
    if ((err == MP_OKAY) && (a->used > r->size)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        _sp_div_2(a, r);
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && HAVE_ECC */
#endif /* HAVE_ECC || !NO_DSA || OPENSSL_EXTRA ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)
/* Divides a by 2 mod m and stores in r: r = (a / 2) mod m
 *
 * r = a / 2 (mod m) - constant time (a < m and positive)
 *
 * @param  [in]   a  SP integer to divide.
 * @param  [in]   m  SP integer that is modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, m or r is NULL.
 */
int sp_div_2_mod_ct(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Check result has enough space for a + m. */
    if ((err == MP_OKAY) && (m->used + 1 > r->size)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
    #ifndef SQR_MUL_ASM
        sp_int_word  w = 0;
    #else
        sp_int_digit l = 0;
        sp_int_digit h;
        sp_int_digit t;
    #endif
        /* Mask to apply to modulus. */
        sp_int_digit mask = (sp_int_digit)0 - (a->dp[0] & 1);
        unsigned int i;

    #if 0
        sp_print(a, "a");
        sp_print(m, "m");
    #endif

        /* Add a to m, if a is odd, into r in constant time. */
        for (i = 0; i < m->used; i++) {
            /* Mask to apply to a - set when used value at index. */
            sp_int_digit mask_a = (sp_int_digit)0 - (i < a->used);

        #ifndef SQR_MUL_ASM
            /* Conditionally add modulus. */
            w         += m->dp[i] & mask;
            /* Conditionally add a. */
            w         += a->dp[i] & mask_a;
            /* Store low digit in result. */
            r->dp[i]   = (sp_int_digit)w;
            /* Move high digit down. */
            w        >>= DIGIT_BIT;
        #else
            /* No high digit. */
            h        = 0;
            /* Conditionally use modulus. */
            t        = m->dp[i] & mask;
            /* Add with carry modulus. */
            SP_ASM_ADDC_REG(l, h, t);
            /* Conditionally use a. */
            t        = a->dp[i] & mask_a;
            /* Add with carry a. */
            SP_ASM_ADDC_REG(l, h, t);
            /* Store low digit in result. */
            r->dp[i] = l;
            /* Move high digit down. */
            l        = h;
        #endif
        }
        /* Store carry. */
    #ifndef SQR_MUL_ASM
        r->dp[i] = (sp_int_digit)w;
    #else
        r->dp[i] = l;
    #endif
        /* Used includes carry - set or not. */
        r->used = i + 1;
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        r->sign = MP_ZPOS;
    #endif
        /* Divide conditional sum by 2. */
        _sp_div_2(r, r);

    #if 0
        sp_print(r, "rd2");
    #endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && HAVE_ECC */

/************************
 * Add/Subtract Functions
 ************************/

#if !defined(WOLFSSL_RSA_VERIFY_ONLY) || defined(WOLFSSL_SP_INVMOD)
/* Add offset b to a into r: r = a + (b << (o * SP_WORD_SIZEOF))
 *
 * @param  [in]   a  SP integer to add to.
 * @param  [in]   b  SP integer to add.
 * @param  [out]  r  SP integer to store result in.
 * @param  [in]   o  Number of digits to offset b.
 */
static void _sp_add_off(const sp_int* a, const sp_int* b, sp_int* r, int o)
{
    unsigned int i = 0;
#ifndef SQR_MUL_ASM
    sp_int_word t = 0;
#else
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit t = 0;
#endif

#ifdef SP_MATH_NEED_ADD_OFF
    unsigned int j;

    /* Copy a into result up to offset. */
    for (; (i < o) && (i < a->used); i++) {
        r->dp[i] = a->dp[i];
    }
    /* Set result to 0 for digits beyonf those in a. */
    for (; i < o; i++) {
        r->dp[i] = 0;
    }

    /* Add each digit from a and b where both have values. */
    for (j = 0; (i < a->used) && (j < b->used); i++, j++) {
    #ifndef SQR_MUL_ASM
        t += a->dp[i];
        t += b->dp[j];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = a->dp[i];
        SP_ASM_ADDC(l, h, t);
        t = b->dp[j];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
    /* Either a and/or b are out of digits. Add carry and remaining a digits. */
    for (; i < a->used; i++) {
    #ifndef SQR_MUL_ASM
        t += a->dp[i];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = a->dp[i];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
    /* a is out of digits. Add carry and remaining b digits. */
    for (; j < b->used; i++, j++) {
    #ifndef SQR_MUL_ASM
        t += b->dp[j];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = b->dp[j];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
#else
    (void)o;

    /* Add each digit from a and b where both have values. */
    for (; (i < a->used) && (i < b->used); i++) {
    #ifndef SQR_MUL_ASM
        t += a->dp[i];
        t += b->dp[i];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = a->dp[i];
        SP_ASM_ADDC(l, h, t);
        t = b->dp[i];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
    /* Either a and/or b are out of digits. Add carry and remaining a digits. */
    for (; i < a->used; i++) {
    #ifndef SQR_MUL_ASM
        t += a->dp[i];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = a->dp[i];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
    /* a is out of digits. Add carry and remaining b digits. */
    for (; i < b->used; i++) {
    #ifndef SQR_MUL_ASM
        t += b->dp[i];
        r->dp[i] = (sp_int_digit)t;
        t >>= SP_WORD_SIZE;
    #else
        t = b->dp[i];
        SP_ASM_ADDC(l, h, t);
        r->dp[i] = l;
        l = h;
        h = 0;
    #endif
    }
#endif

    /* Set used based on last digit put in. */
    r->used = i;
    /* Put in carry. */
#ifndef SQR_MUL_ASM
    r->dp[i] = (sp_int_digit)t;
    r->used += (t != 0);
#else
    r->dp[i] = l;
    r->used += (l != 0);
#endif

    /* Remove leading zeros. */
    sp_clamp(r);
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY */

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_INT_NEGATIVE) || \
    !defined(NO_DH) || defined(HAVE_ECC) || (!defined(NO_RSA) && \
    !defined(WOLFSSL_RSA_VERIFY_ONLY))
/* Sub offset b from a into r: r = a - (b << (o * SP_WORD_SIZEOF))
 * a must be greater than b.
 *
 * When using offset, r == a is faster.
 *
 * @param  [in]   a  SP integer to subtract from.
 * @param  [in]   b  SP integer to subtract.
 * @param  [out]  r  SP integer to store result in.
 * @param  [in]   o  Number of digits to offset b.
 */
static void _sp_sub_off(const sp_int* a, const sp_int* b, sp_int* r,
    unsigned int o)
{
    unsigned int i = 0;
    unsigned int j;
#ifndef SQR_MUL_ASM
    sp_int_sword t = 0;
#else
    sp_int_digit l = 0;
    sp_int_digit h = 0;
#endif

    /* Need to copy digits up to offset into result. */
    if (r != a) {
        for (; (i < o) && (i < a->used); i++) {
            r->dp[i] = a->dp[i];
        }
    }
    else {
        i = o;
    }
    /* Index to add at is the offset now. */

    for (j = 0; (i < a->used) && (j < b->used); i++, j++) {
    #ifndef SQR_MUL_ASM
        /* Add a into and subtract b from current value. */
        t += a->dp[i];
        t -= b->dp[j];
        /* Store low digit in result. */
        r->dp[i] = (sp_int_digit)t;
        /* Move high digit down. */
        t >>= SP_WORD_SIZE;
    #else
        /* Add a into and subtract b from current value. */
        SP_ASM_ADDC(l, h, a->dp[i]);
        SP_ASM_SUBB(l, h, b->dp[j]);
        /* Store low digit in result. */
        r->dp[i] = l;
        /* Move high digit down. */
        l = h;
        /* High digit is 0 when positive or -1 on negative. */
        h = (sp_int_digit)0 - (h >> (SP_WORD_SIZE - 1));
    #endif
    }
    for (; i < a->used; i++) {
    #ifndef SQR_MUL_ASM
        /* Add a into current value. */
        t += a->dp[i];
        /* Store low digit in result. */
        r->dp[i] = (sp_int_digit)t;
        /* Move high digit down. */
        t >>= SP_WORD_SIZE;
    #else
        /* Add a into current value. */
        SP_ASM_ADDC(l, h, a->dp[i]);
        /* Store low digit in result. */
        r->dp[i] = l;
        /* Move high digit down. */
        l = h;
        /* High digit is 0 when positive or -1 on negative. */
        h = (sp_int_digit)0 - (h >> (SP_WORD_SIZE - 1));
    #endif
    }

    /* Set used based on last digit put in. */
    r->used = i;
    /* Remove leading zeros. */
    sp_clamp(r);
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_SP_INT_NEGATIVE || !NO_DH ||
        * HAVE_ECC || (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if !defined(WOLFSSL_RSA_VERIFY_ONLY) || defined(WOLFSSL_SP_INVMOD)
/* Add b to a into r: r = a + b
 *
 * @param  [in]   a  SP integer to add to.
 * @param  [in]   b  SP integer to add.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b, or r is NULL.
 */
int sp_add(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Check that r as big as a and b plus one word. */
    if ((err == MP_OKAY) && ((a->used >= r->size) || (b->used >= r->size))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
    #ifndef WOLFSSL_SP_INT_NEGATIVE
        /* Add two positive numbers. */
        _sp_add_off(a, b, r, 0);
    #else
        /* Same sign then add absolute values and use sign. */
        if (a->sign == b->sign) {
            _sp_add_off(a, b, r, 0);
            r->sign = a->sign;
        }
        /* Different sign and abs(a) >= abs(b). */
        else if (_sp_cmp_abs(a, b) != MP_LT) {
            /* Subtract absolute values and use sign of a unless result 0. */
            _sp_sub_off(a, b, r, 0);
            if (sp_iszero(r)) {
                r->sign = MP_ZPOS;
            }
            else {
                r->sign = a->sign;
            }
        }
        /* Different sign and abs(a) < abs(b). */
        else {
            /* Reverse subtract absolute values and use sign of b. */
            _sp_sub_off(b, a, r, 0);
            r->sign = b->sign;
        }
    #endif
    }

    return err;
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY))
/* Subtract b from a into r: r = a - b
 *
 * a must be greater than b unless WOLFSSL_SP_INT_NEGATIVE is defined.
 *
 * @param  [in]   a  SP integer to subtract from.
 * @param  [in]   b  SP integer to subtract.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b, or r is NULL.
 */
int sp_sub(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Check that r as big as a and b plus one word. */
    if ((err == MP_OKAY) && ((a->used >= r->size) || (b->used >= r->size))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
    #ifndef WOLFSSL_SP_INT_NEGATIVE
        /* Subtract positive numbers b from a. */
        _sp_sub_off(a, b, r, 0);
    #else
        /* Different sign. */
        if (a->sign != b->sign) {
            /* Add absolute values and use sign of a. */
            _sp_add_off(a, b, r, 0);
            r->sign = a->sign;
        }
        /* Same sign and abs(a) >= abs(b). */
        else if (_sp_cmp_abs(a, b) != MP_LT) {
            /* Subtract absolute values and use sign of a unless result 0. */
            _sp_sub_off(a, b, r, 0);
            if (sp_iszero(r)) {
                r->sign = MP_ZPOS;
            }
            else {
                r->sign = a->sign;
            }
        }
        /* Same sign and abs(a) < abs(b). */
        else {
            /* Reverse subtract absolute values and use opposite sign of a */
            _sp_sub_off(b, a, r, 0);
            r->sign = 1 - a->sign;
        }
    #endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY)*/

/****************************
 * Add/Subtract mod functions
 ****************************/

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    (!defined(WOLFSSL_SP_MATH) && defined(WOLFSSL_CUSTOM_CURVES)) || \
    defined(WOLFCRYPT_HAVE_ECCSI) || defined(WOLFCRYPT_HAVE_SAKKE)
/* Add two value and reduce: r = (a + b) % m
 *
 * @param  [in]   a  SP integer to add.
 * @param  [in]   b  SP integer to add with.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_addmod(const sp_int* a, const sp_int* b, const sp_int* m,
    sp_int* r)
{
    int err = MP_OKAY;
    /* Calculate used based on digits used in a and b. */
    unsigned int used = ((a->used >= b->used) ? a->used + 1 : b->used + 1);
    DECL_SP_INT(t, used);

    /* Allocate a temporary SP int to hold sum. */
    ALLOC_SP_INT_SIZE(t, used, err, NULL);

    if (err == MP_OKAY) {
        /* Do sum. */
        err = sp_add(a, b, t);
    }
    if (err == MP_OKAY) {
        /* Mod result. */
        err = sp_mod(t, m, r);
    }

    FREE_SP_INT(t, NULL);
    return err;
}

/* Add two value and reduce: r = (a + b) % m
 *
 * @param  [in]   a  SP integer to add.
 * @param  [in]   b  SP integer to add with.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b, m or r is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_addmod(const sp_int* a, const sp_int* b, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure a and b aren't too big a number to operate on. */
    else if (a->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    else if (b->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }


#if 0
    if (err == MP_OKAY) {
        sp_print(a, "a");
        sp_print(b, "b");
        sp_print(m, "m");
    }
#endif
    if (err == MP_OKAY) {
        /* Do add and modular reduction. */
        err = _sp_addmod(a, b, m, r);
    }
#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rma");
    }
#endif

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_CUSTOM_CURVES) ||
        * WOLFCRYPT_HAVE_ECCSI || WOLFCRYPT_HAVE_SAKKE */

#if defined(WOLFSSL_SP_MATH_ALL) && (!defined(WOLFSSL_RSA_VERIFY_ONLY) || \
    defined(HAVE_ECC))
/* Sub b from a and reduce: r = (a - b) % m
 * Result is always positive.
 *
 * @param  [in]   a  SP integer to subtract from
 * @param  [in]   b  SP integer to subtract.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_submod(const sp_int* a, const sp_int* b, const sp_int* m,
    sp_int* r)
{
    int err = MP_OKAY;
#ifndef WOLFSSL_SP_INT_NEGATIVE
    unsigned int used = ((a->used >= m->used) ?
        ((a->used >= b->used) ? (a->used + 1) : (b->used + 1)) :
        ((b->used >= m->used)) ? (b->used + 1) : (m->used + 1));
    DECL_SP_INT_ARRAY(t, used, 2);

    ALLOC_SP_INT_ARRAY(t, used, 2, err, NULL);
    if (err == MP_OKAY) {
        /* Reduce a to less than m. */
        if (_sp_cmp(a, m) != MP_LT) {
            err = sp_mod(a, m, t[0]);
            a = t[0];
        }
    }
    if (err == MP_OKAY) {
        /* Reduce b to less than m. */
        if (_sp_cmp(b, m) != MP_LT) {
            err = sp_mod(b, m, t[1]);
            b = t[1];
        }
    }
    if (err == MP_OKAY) {
        /* Add m to a if a smaller than b. */
        if (_sp_cmp(a, b) == MP_LT) {
            err = sp_add(a, m, t[0]);
            a = t[0];
        }
    }
    if (err == MP_OKAY) {
        /* Subtract b from a. */
        err = sp_sub(a, b, r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
#else /* WOLFSSL_SP_INT_NEGATIVE */
    unsigned int used = ((a->used >= b->used) ? a->used + 1 : b->used + 1);
    DECL_SP_INT(t, used);

    ALLOC_SP_INT_SIZE(t, used, err, NULL);
    /* Subtract b from a into temporary. */
    if (err == MP_OKAY) {
        err = sp_sub(a, b, t);
    }
    if (err == MP_OKAY) {
        /* Reduce result mod m into result. */
        err = sp_mod(t, m, r);
    }
    FREE_SP_INT(t, NULL);
#endif /* WOLFSSL_SP_INT_NEGATIVE */

    return err;
}

/* Sub b from a and reduce: r = (a - b) % m
 * Result is always positive.
 *
 * @param  [in]   a  SP integer to subtract from
 * @param  [in]   b  SP integer to subtract.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b, m or r is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_submod(const sp_int* a, const sp_int* b, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure a, b and m aren't too big a number to operate on. */
    else if (a->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    else if (b->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    else if (m->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(a, "a");
        sp_print(b, "b");
        sp_print(m, "m");
    }
#endif
    if (err == MP_OKAY) {
        /* Do submod. */
        err = _sp_submod(a, b, m, r);
    }
#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rms");
    }
#endif

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL */

#if defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)
/* Add two value and reduce: r = (a + b) % m
 *
 * r = a + b (mod m) - constant time (a < m and b < m, a, b and m are positive)
 *
 * Assumes a, b, m and r are not NULL.
 * m and r must not be the same pointer.
 *
 * @param  [in]   a  SP integer to add.
 * @param  [in]   b  SP integer to add with.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 */
int sp_addmod_ct(const sp_int* a, const sp_int* b, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
#ifndef SQR_MUL_ASM
    sp_int_sword w;
    sp_int_sword s;
#else
    sp_int_digit wl;
    sp_int_digit wh;
    sp_int_digit sl;
    sp_int_digit sh;
    sp_int_digit t;
#endif
    sp_int_digit mask;
    sp_int_digit mask_a = (sp_int_digit)-1;
    sp_int_digit mask_b = (sp_int_digit)-1;
    unsigned int i;

    /* Check result is as big as modulus. */
    if (m->used > r->size) {
        err = MP_VAL;
    }
    /* Validate parameters. */
    if ((err == MP_OKAY) && (r == m)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
#if 0
        sp_print(a, "a");
        sp_print(b, "b");
        sp_print(m, "m");
#endif

        /* Add a to b into r. Do the subtract of modulus but don't store result.
         * When subtract result is negative, the overflow will be negative.
         * Only need to subtract mod when result is positive - overflow is
         * positive.
         */
    #ifndef SQR_MUL_ASM
        w = 0;
        s = 0;
    #else
        wl = 0;
        sl = 0;
        sh = 0;
    #endif
        /* Constant time - add modulus digits worth from a and b. */
        for (i = 0; i < m->used; i++) {
            /* Values past 'used' are not initialized. */
            mask_a += (i == a->used);
            mask_b += (i == b->used);

        #ifndef SQR_MUL_ASM
            /* Add next digits from a and b to current value. */
            w         += a->dp[i] & mask_a;
            w         += b->dp[i] & mask_b;
            /* Store low digit in result. */
            r->dp[i]   = (sp_int_digit)w;
            /* Add result to reducing value. */
            s         += (sp_int_digit)w;
            /* Subtract next digit of modulus. */
            s         -= m->dp[i];
            /* Move high digit of reduced result down. */
            s        >>= DIGIT_BIT;
            /* Move high digit of sum result down. */
            w        >>= DIGIT_BIT;
        #else
            wh = 0;
            /* Add next digits from a and b to current value. */
            t = a->dp[i] & mask_a;
            SP_ASM_ADDC_REG(wl, wh, t);
            t = b->dp[i] & mask_b;
            SP_ASM_ADDC_REG(wl, wh, t);
            /* Store low digit in result. */
            r->dp[i] = wl;
            /* Add result to reducing value. */
            SP_ASM_ADDC_REG(sl, sh, wl);
            /* Subtract next digit of modulus. */
            SP_ASM_SUBB(sl, sh, m->dp[i]);
            /* Move high digit of reduced result down. */
            sl = sh;
            /* High digit is 0 when positive or -1 on negative. */
            sh = (sp_int_digit)0 - (sh >> (SP_WORD_SIZE-1));
            /* Move high digit of sum result down. */
            wl = wh;
        #endif
        }
    #ifndef SQR_MUL_ASM
        /* Add carry into reduced result. */
        s += (sp_int_digit)w;
        /* s will be positive when subtracting modulus is needed. */
        mask = (sp_int_digit)0 - (s >= 0);
    #else
        /* Add carry into reduced result. */
        SP_ASM_ADDC_REG(sl, sh, wl);
        /* s will be positive when subtracting modulus is needed. */
        mask = (sh >> (SP_WORD_SIZE-1)) - 1;
    #endif

        /* Constant time, conditionally, subtract modulus from sum. */
    #ifndef SQR_MUL_ASM
        w = 0;
    #else
        wl = 0;
        wh = 0;
    #endif
        for (i = 0; i < m->used; i++) {
        #ifndef SQR_MUL_ASM
            /* Add result to current value and conditionally subtract modulus.
             */
            w         += r->dp[i];
            w         -= m->dp[i] & mask;
            /* Store low digit in result. */
            r->dp[i]   = (sp_int_digit)w;
            /* Move high digit of sum result down. */
            w        >>= DIGIT_BIT;
        #else
            /* Add result to current value and conditionally subtract modulus.
             */
            SP_ASM_ADDC(wl, wh, r->dp[i]);
            t = m->dp[i] & mask;
            SP_ASM_SUBB_REG(wl, wh, t);
            /* Store low digit in result. */
            r->dp[i] = wl;
            /* Move high digit of sum result down. */
            wl = wh;
            /* High digit is 0 when positive or -1 on negative. */
            wh = (sp_int_digit)0 - (wl >> (SP_WORD_SIZE-1));
        #endif
        }
        /* Result will always have digits equal to or less than those in
         * modulus. */
        r->used = i;
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        r->sign = MP_ZPOS;
    #endif /* WOLFSSL_SP_INT_NEGATIVE */
        /* Remove leading zeros. */
        sp_clamp(r);

#if 0
        sp_print(r, "rma");
#endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && HAVE_ECC */

#if defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC)
/* Sub b from a and reduce: r = (a - b) % m
 * Result is always positive.
 *
 * r = a - b (mod m) - constant time (a < m and b < m, a, b and m are positive)
 *
 * Assumes a, b, m and r are not NULL.
 * m and r must not be the same pointer.
 *
 * @param  [in]   a  SP integer to subtract from
 * @param  [in]   b  SP integer to subtract.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 */
int sp_submod_ct(const sp_int* a, const sp_int* b, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
#ifndef SQR_MUL_ASM
    sp_int_sword w;
#else
    sp_int_digit l;
    sp_int_digit h;
    sp_int_digit t;
#endif
    sp_int_digit mask;
    sp_int_digit mask_a = (sp_int_digit)-1;
    sp_int_digit mask_b = (sp_int_digit)-1;
    unsigned int i;

    /* Check result is as big as modulus plus one digit. */
    if (m->used > r->size) {
        err = MP_VAL;
    }
    /* Validate parameters. */
    if ((err == MP_OKAY) && (r == m)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
#if 0
        sp_print(a, "a");
        sp_print(b, "b");
        sp_print(m, "m");
#endif

        /* In constant time, subtract b from a putting result in r. */
    #ifndef SQR_MUL_ASM
        w = 0;
    #else
        l = 0;
        h = 0;
    #endif
        for (i = 0; i < m->used; i++) {
            /* Values past 'used' are not initialized. */
            mask_a += (i == a->used);
            mask_b += (i == b->used);

        #ifndef SQR_MUL_ASM
            /* Add a to and subtract b from current value. */
            w         += a->dp[i] & mask_a;
            w         -= b->dp[i] & mask_b;
            /* Store low digit in result. */
            r->dp[i]   = (sp_int_digit)w;
            /* Move high digit down. */
            w        >>= DIGIT_BIT;
        #else
            /* Add a and subtract b from current value. */
            t = a->dp[i] & mask_a;
            SP_ASM_ADDC_REG(l, h, t);
            t = b->dp[i] & mask_b;
            SP_ASM_SUBB_REG(l, h, t);
            /* Store low digit in result. */
            r->dp[i] = l;
            /* Move high digit down. */
            l = h;
            /* High digit is 0 when positive or -1 on negative. */
            h = (sp_int_digit)0 - (l >> (SP_WORD_SIZE - 1));
        #endif
        }
        /* When w is negative then we need to add modulus to make result
         * positive. */
    #ifndef SQR_MUL_ASM
        mask = (sp_int_digit)0 - (w < 0);
    #else
        mask = h;
    #endif
        /* Constant time, conditionally, add modulus to difference. */
    #ifndef SQR_MUL_ASM
        w = 0;
    #else
        l = 0;
    #endif
        for (i = 0; i < m->used; i++) {
        #ifndef SQR_MUL_ASM
            /* Add result and conditionally modulus to current value. */
            w         += r->dp[i];
            w         += m->dp[i] & mask;
            /* Store low digit in result. */
            r->dp[i]   = (sp_int_digit)w;
            /* Move high digit down. */
            w        >>= DIGIT_BIT;
        #else
            h = 0;
            /* Add result and conditionally modulus to current value. */
            SP_ASM_ADDC(l, h, r->dp[i]);
            t = m->dp[i] & mask;
            SP_ASM_ADDC_REG(l, h, t);
            /* Store low digit in result. */
            r->dp[i] = l;
            /* Move high digit down. */
            l = h;
        #endif
        }
        /* Result will always have digits equal to or less than those in
         * modulus. */
        r->used = i;
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        r->sign = MP_ZPOS;
    #endif /* WOLFSSL_SP_INT_NEGATIVE */
        /* Remove leading zeros. */
        sp_clamp(r);

#if 0
        sp_print(r, "rms");
#endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && HAVE_ECC */

/********************
 * Shifting functoins
 ********************/

#if !defined(NO_DH) || defined(HAVE_ECC) || (!defined(NO_RSA) && \
    defined(WC_RSA_BLINDING) && !defined(WOLFSSL_RSA_VERIFY_ONLY))
/* Left shift the multi-precision number by a number of digits.
 *
 * @param  [in,out]  a  SP integer to shift.
 * @param  [in]      s  Number of digits to shift.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL, s is negative or the result is too big.
 */
int sp_lshd(sp_int* a, int s)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (s < 0)) {
        err = MP_VAL;
    }
    /* Ensure number has enough digits for operation. */
    if ((err == MP_OKAY) && (a->used + (unsigned int)s > a->size)) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Move up digits. */
        XMEMMOVE(a->dp + s, a->dp, a->used * SP_WORD_SIZEOF);
        /* Back fill with zeros. */
        XMEMSET(a->dp, 0, (size_t)s * SP_WORD_SIZEOF);
        /* Update used. */
        a->used += (unsigned int)s;
        /* Remove leading zeros. */
        sp_clamp(a);
    }

    return err;
}
#endif

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))
/* Left shift the multi-precision number by n bits.
 * Bits may be larger than the word size.
 *
 * Used by sp_mul_2d() and other internal functions.
 *
 * @param  [in,out]  a  SP integer to shift.
 * @param  [in]      n  Number of bits to shift left.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when the result is too big.
 */
static int sp_lshb(sp_int* a, int n)
{
    int err = MP_OKAY;

    if (a->used != 0) {
        /* Calculate number of digits to shift. */
        unsigned int s = (unsigned int)n >> SP_WORD_SHIFT;

        /* Ensure number has enough digits for result. */
        if (a->used + s >= a->size) {
            err = MP_VAL;
        }
        if (err == MP_OKAY) {
            /* Get count of bits to move in digit. */
            n &= SP_WORD_MASK;
            /* Check whether this is a complicated case. */
            if (n != 0) {
                unsigned int i;

                /* Shift up starting at most significant digit. */
                /* Get new most significant digit. */
                sp_int_digit v = a->dp[a->used - 1] >> (SP_WORD_SIZE - n);
                /* Shift up each digit. */
                for (i = a->used - 1; i >= 1; i--) {
                    a->dp[i + s] = (a->dp[i] << n) |
                                   (a->dp[i - 1] >> (SP_WORD_SIZE - n));
                }
                /* Shift up least significant digit. */
                a->dp[s] = a->dp[0] << n;
                /* Add new high digit unless zero. */
                if (v != 0) {
                    a->dp[a->used + s] = v;
                    a->used++;
                }
            }
            /* Only digits to move and ensure not zero. */
            else if (s > 0) {
                /* Move up digits. */
                XMEMMOVE(a->dp + s, a->dp, a->used * SP_WORD_SIZEOF);
            }

            /* Update used digit count. */
            a->used += s;
            /* Back fill with zeros. */
            XMEMSET(a->dp, 0, SP_WORD_SIZEOF * s);
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#ifdef WOLFSSL_SP_MATH_ALL
/* Shift a right by c digits: a = a >> (n * SP_WORD_SIZE)
 *
 * @param  [in, out] a  SP integer to shift.
 * @param  [in]      c  Number of digits to shift.
 */
void sp_rshd(sp_int* a, int c)
{
    /* Do shift if we have an SP int. */
    if ((a != NULL) && (c > 0)) {
        /* Make zero if shift removes all digits. */
        if ((unsigned int)c >= a->used) {
            _sp_zero(a);
        }
        else {
            unsigned int i;

            /* Update used digits count. */
            a->used -= (unsigned int)c;
            /* Move digits down. */
            for (i = 0; i < a->used; i++, c++) {
                a->dp[i] = a->dp[c];
            }
        }
    }
}
#endif /* WOLFSSL_SP_MATH_ALL */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WOLFSSL_HAVE_SP_DH)
/* Shift a right by n bits into r: r = a >> n
 *
 * @param  [in]   a  SP integer to shift.
 * @param  [in]   n  Number of bits to shift.
 * @param  [out]  r  SP integer to store result in.
 */
int sp_rshb(const sp_int* a, int n, sp_int* r)
{
    int err = MP_OKAY;
    /* Number of digits to shift down. */
    unsigned int i = (unsigned int)(n >> SP_WORD_SHIFT);

    if ((a == NULL) || (n < 0)) {
        err = MP_VAL;
    }
    /* Handle case where shifting out all digits. */
    if ((err == MP_OKAY) && (i >= a->used)) {
        _sp_zero(r);
    }
    /* Change callers when more error cases returned. */
    else if ((err == MP_OKAY) && (a->used - i > r->size)) {
        err = MP_VAL;
    }
    else if (err == MP_OKAY) {
        unsigned int j;

        /* Number of bits to shift in digits. */
        n &= SP_WORD_SIZE - 1;
        /* Handle simple case. */
        if (n == 0) {
            /* Set the count of used digits. */
            r->used = a->used - i;
            /* Move digits down. */
            if (r == a) {
                XMEMMOVE(r->dp, r->dp + i, SP_WORD_SIZEOF * r->used);
            }
            else {
                XMEMCPY(r->dp, a->dp + i, SP_WORD_SIZEOF * r->used);
            }
        }
        else {
            /* Move the bits down starting at least significant digit. */
            for (j = 0; i < a->used-1; i++, j++)
                r->dp[j] = (a->dp[i] >> n) | (a->dp[i+1] << (SP_WORD_SIZE - n));
            /* Most significant digit has no higher digit to pull from. */
            r->dp[j] = a->dp[i] >> n;
            /* Set the count of used digits. */
            r->used = j + (r->dp[j] > 0);
        }
#ifdef WOLFSSL_SP_INT_NEGATIVE
        if (sp_iszero(r)) {
            /* Set zero sign. */
            r->sign = MP_ZPOS;
        }
        else {
            /* Retain sign. */
            r->sign = a->sign;
        }
#endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) || WOLFSSL_HAVE_SP_DH */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))
static void _sp_div_same_size(sp_int* a, const sp_int* d, sp_int* r)
{
    unsigned int i;

    /* Compare top digits of dividend with those of divisor up to last. */
    for (i = d->used - 1; i > 0; i--) {
        /* Break if top divisor is not equal to dividend. */
        if (a->dp[a->used - d->used + i] != d->dp[i]) {
            break;
        }
    }
    /* Check if top dividend is greater than or equal to divisor. */
    if (a->dp[a->used - d->used + i] >= d->dp[i]) {
        /* Update quotient result. */
        r->dp[a->used - d->used] += 1;
        /* Get 'used' to restore - ensure zeros put into quotient. */
        i = a->used;
        /* Subtract d from top of a. */
        _sp_sub_off(a, d, a, a->used - d->used);
        /* Restore 'used' on remainder. */
        a->used = i;
    }
}

/* Divide a by d and return the quotient in r and the remainder in a.
 *   r = a / d; a = a % d
 *
 * Note: a is constantly having multiplies of d subtracted.
 *
 * @param  [in, out] a      SP integer to be divided and remainder on out.
 * @param  [in]      d      SP integer to divide by.
 * @param  [out]     r      SP integer that is the quotient.
 * @param  [out]     trial  SP integer that is product in trial division.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when operation fails - only when compiling small code.
 */
static int _sp_div_impl(sp_int* a, const sp_int* d, sp_int* r, sp_int* trial)
{
    int err = MP_OKAY;
    unsigned int i;
#ifdef WOLFSSL_SP_SMALL
    int c;
#else
    unsigned int j;
    unsigned int o;
    #ifndef SQR_MUL_ASM
    sp_int_sword sw;
    #else
    sp_int_digit sl;
    sp_int_digit sh;
    sp_int_digit st;
    #endif
#endif /* WOLFSSL_SP_SMALL */
    sp_int_digit t;
    sp_int_digit dt;

    /* Set result size to clear. */
    r->used = a->used - d->used + 1;
    /* Set all potentially used digits to zero. */
    for (i = 0; i < r->used; i++) {
        r->dp[i] = 0;
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    r->sign = MP_ZPOS;
#endif
    /* Get the most significant digit (will have top bit set). */
    dt = d->dp[d->used-1];

    /* Handle when a >= d ^ (2 ^ (SP_WORD_SIZE * x)). */
    _sp_div_same_size(a, d, r);

    /* Keep subtracting multiples of d as long as the digit count of a is
     * greater than equal to d.
     */
    for (i = a->used - 1; i >= d->used; i--) {
        /* When top digits equal, guestimate maximum multiplier.
         * Worst case, multiplier is actually SP_DIGIT_MAX - 1.
         * That is, for w (word size in bits) > 1, n > 1, let:
         *   a = 2^((n+1)*w-1), d = 2^(n*w-1) + 2^((n-1)*w) - 1, t = 2^w - 2
         * Then,
         *     d * t
         *   = (2^(n*w-1) + 2^((n-1)*w) - 1) * (2^w - 2)
         *   = 2^((n+1)*w-1) - 2^(n*w) + 2^(n*w) - 2^((n-1)*w+1) - 2^w + 2
         *   = 2^((n+1)*w-1) - 2^((n-1)*w+1) - 2^w + 2
         *   = a - 2^((n-1)*w+1) - 2^w + 2
         * d > 2^((n-1)*w+1) + 2^w - 2, when w > 1, n > 1
         */
        if (a->dp[i] == dt) {
            t = SP_DIGIT_MAX;
        }
        else {
            /* Calculate trial quotient by dividing top word of dividend by top
             * digit of divisor.
             * Some implementations segfault when quotient > SP_DIGIT_MAX.
             * Implementations in assembly, using builtins or using
             * digits only (WOLFSSL_SP_DIV_WORD_HALF).
             */
            t = sp_div_word(a->dp[i], a->dp[i-1], dt);
        }
#ifdef WOLFSSL_SP_SMALL
        do {
            /* Calculate trial from trial quotient. */
            err = _sp_mul_d(d, t, trial, i - d->used);
            if (err != MP_OKAY) {
                break;
            }
            /* Check if trial is bigger. */
            c = _sp_cmp_abs(trial, a);
            if (c == MP_GT) {
                /* Decrement trial quotient and try again. */
                t--;
            }
        }
        while (c == MP_GT);

        if (err != MP_OKAY) {
            break;
        }

        /* Subtract the trial and add qoutient to result. */
        _sp_sub_off(a, trial, a, 0);
        r->dp[i - d->used] += t;
        /* Handle overflow of digit. */
        if (r->dp[i - d->used] < t) {
            r->dp[i + 1 - d->used]++;
        }
#else
        /* Index of lowest digit trial is subtracted from. */
        o = i - d->used;
        do {
        #ifndef SQR_MUL_ASM
            sp_int_word tw = 0;
        #else
            sp_int_digit tl = 0;
            sp_int_digit th = 0;
        #endif

            /* Multiply divisor by trial quotient. */
            for (j = 0; j < d->used; j++) {
            #ifndef SQR_MUL_ASM
                tw += (sp_int_word)d->dp[j] * t;
                trial->dp[j] = (sp_int_digit)tw;
                tw >>= SP_WORD_SIZE;
            #else
                SP_ASM_MUL_ADD_NO(tl, th, d->dp[j], t);
                trial->dp[j] = tl;
                tl = th;
                th = 0;
            #endif
            }
          #ifndef SQR_MUL_ASM
            trial->dp[j] = (sp_int_digit)tw;
          #else
            trial->dp[j] = tl;
          #endif

            /* Check trial quotient isn't larger than dividend. */
            for (j = d->used; j > 0; j--) {
                if (trial->dp[j] != a->dp[j + o]) {
                    break;
                }
            }
            /* Decrement trial quotient if larger and try again. */
            if (trial->dp[j] > a->dp[j + o]) {
                t--;
            }
        }
        while (trial->dp[j] > a->dp[j + o]);

    #ifndef SQR_MUL_ASM
        sw = 0;
    #else
        sl = 0;
        sh = 0;
    #endif
        /* Subtract trial - don't need to update used. */
        for (j = 0; j <= d->used; j++) {
        #ifndef SQR_MUL_ASM
            sw += a->dp[j + o];
            sw -= trial->dp[j];
            a->dp[j + o] = (sp_int_digit)sw;
            sw >>= SP_WORD_SIZE;
        #else
            st = a->dp[j + o];
            SP_ASM_ADDC(sl, sh, st);
            st = trial->dp[j];
            SP_ASM_SUBB(sl, sh, st);
            a->dp[j + o] = sl;
            sl = sh;
            sh = (sp_int_digit)0 - (sl >> (SP_WORD_SIZE - 1));
        #endif
        }

        r->dp[o] = t;
#endif /* WOLFSSL_SP_SMALL */
    }
    /* Update used. */
    a->used = i + 1;
    if (a->used == d->used) {
        /* Finish div now that length of dividend is same as divisor. */
        _sp_div_same_size(a, d, r);
    }

    return err;
}

/* Divide a by d and return the quotient in r and the remainder in rem.
 *   r = a / d; rem = a % d
 *
 * @param  [in]   a     SP integer to be divided.
 * @param  [in]   d     SP integer to divide by.
 * @param  [out]  r     SP integer that is the quotient.
 * @param  [out]  rem   SP integer that is the remainder.
 * @param  [in]   used  Number of digits in temporaries to use.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_div(const sp_int* a, const sp_int* d, sp_int* r, sp_int* rem,
    unsigned int used)
{
    int err = MP_OKAY;
    int ret;
    int done = 0;
    int s = 0;
    sp_int* sa = NULL;
    sp_int* sd = NULL;
    sp_int* tr = NULL;
    sp_int* trial = NULL;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    unsigned int signA = MP_ZPOS;
    unsigned int signD = MP_ZPOS;
#endif /* WOLFSSL_SP_INT_NEGATIVE */
    /* Intermediates will always be less than or equal to dividend. */
    DECL_SP_INT_ARRAY(td, used, 4);

#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Cache sign for results. */
    signA = a->sign;
    signD = d->sign;
#endif /* WOLFSSL_SP_INT_NEGATIVE */

    /* Handle simple case of: dividend < divisor. */
    ret = _sp_cmp_abs(a, d);
    if (ret == MP_LT) {
        /* a = 0 * d + a */
        if ((rem != NULL) && (a != rem)) {
            _sp_copy(a, rem);
        }
        if (r != NULL) {
            _sp_set(r, 0);
        }
        done = 1;
    }
    /* Handle simple case of: dividend == divisor. */
    else if (ret == MP_EQ) {
        /* a = 1 * d + 0 */
        if (rem != NULL) {
            _sp_set(rem, 0);
        }
        if (r != NULL) {
            _sp_set(r, 1);
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            r->sign = (signA == signD) ? MP_ZPOS : MP_NEG;
        #endif /* WOLFSSL_SP_INT_NEGATIVE */
        }
        done = 1;
    }
    else if (sp_count_bits(a) == sp_count_bits(d)) {
        /* a is greater than d but same bit length - subtract. */
        if (rem != NULL) {
            _sp_sub_off(a, d, rem, 0);
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            rem->sign = signA;
        #endif
        }
        if (r != NULL) {
            _sp_set(r, 1);
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            r->sign = (signA == signD) ? MP_ZPOS : MP_NEG;
        #endif /* WOLFSSL_SP_INT_NEGATIVE */
        }
        done = 1;
    }

    /* Allocate temporary 'sp_int's and assign. */
    if ((!done) && (err == MP_OKAY)) {
    #if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
        !defined(WOLFSSL_SP_NO_MALLOC)
        int cnt = 4;
        /* Reuse remainder sp_int where possible. */
        if ((rem != NULL) && (rem != d) && (rem->size > a->used)) {
            sa = rem;
            cnt--;
        }
        /* Reuse result sp_int where possible. */
        if ((r != NULL) && (r != d)) {
            tr = r;
            cnt--;
        }
        /* Macro always has code associated with it and checks err first. */
        ALLOC_SP_INT_ARRAY(td, used, cnt, err, NULL);
    #else
        ALLOC_SP_INT_ARRAY(td, used, 4, err, NULL);
    #endif
    }
    if ((!done) && (err == MP_OKAY)) {
    #if (defined(WOLFSSL_SMALL_STACK) || defined(SP_ALLOC)) && \
        !defined(WOLFSSL_SP_NO_MALLOC)
        int i = 2;

        /* Set to temporary when not reusing. */
        if (sa == NULL) {
            sa = td[i++];
            _sp_init_size(sa, used);
        }
        if (tr == NULL) {
            tr = td[i];
            _sp_init_size(tr, a->used - d->used + 2);
        }
    #else
        sa    = td[2];
        tr    = td[3];

        _sp_init_size(sa, used);
        _sp_init_size(tr, a->used - d->used + 2);
    #endif
        sd    = td[0];
        trial = td[1];

        /* Initialize sizes to minimal values. */
        _sp_init_size(sd, d->used + 1);
        _sp_init_size(trial, used);

        /* Move divisor to top of word. Adjust dividend as well. */
        s = sp_count_bits(d);
        s = SP_WORD_SIZE - (s & SP_WORD_MASK);
        _sp_copy(a, sa);
        /* Only shift if top bit of divisor no set. */
        if (s != SP_WORD_SIZE) {
            err = sp_lshb(sa, s);
            if (err == MP_OKAY) {
                _sp_copy(d, sd);
                d = sd;
                err = sp_lshb(sd, s);
            }
        }
    }
    if ((!done) && (err == MP_OKAY) && (d->used > 0)) {
        /* Do division: tr = sa / d, sa = sa % d. */
        err = _sp_div_impl(sa, d, tr, trial);
        /* Return the remainder if required. */
        if ((err == MP_OKAY) && (rem != NULL)) {
            /* Move result back down if moved up for divisor value. */
            if (s != SP_WORD_SIZE) {
                (void)sp_rshb(sa, s, sa);
            }
            _sp_copy(sa, rem);
            sp_clamp(rem);
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            rem->sign = (rem->used == 0) ? MP_ZPOS : signA;
        #endif
        }
        /* Return the quotient if required. */
        if ((err == MP_OKAY) && (r != NULL)) {
            _sp_copy(tr, r);
            sp_clamp(r);
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            if ((r->used == 0) || (signA == signD)) {
                r->sign = MP_ZPOS;
            }
            else {
                r->sign = MP_NEG;
            }
        #endif /* WOLFSSL_SP_INT_NEGATIVE */
        }
    }

    FREE_SP_INT_ARRAY(td, NULL);
    return err;
}

/* Divide a by d and return the quotient in r and the remainder in rem.
 *   r = a / d; rem = a % d
 *
 * @param  [in]   a    SP integer to be divided.
 * @param  [in]   d    SP integer to divide by.
 * @param  [out]  r    SP integer that is the quotient.
 * @param  [out]  rem  SP integer that is the remainder.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or d is NULL, r and rem are NULL, or d is 0.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_div(const sp_int* a, const sp_int* d, sp_int* r, sp_int* rem)
{
    int err = MP_OKAY;
    unsigned int used = 1;

    /* Validate parameters. */
    if ((a == NULL) || (d == NULL) || ((r == NULL) && (rem == NULL))) {
        err = MP_VAL;
    }
    /* a / 0 = infinity. */
    if ((err == MP_OKAY) && sp_iszero(d)) {
        err = MP_VAL;
    }
    /* Ensure quotient result has enough memory. */
    if ((err == MP_OKAY) && (r != NULL) && (r->size < a->used - d->used + 2)) {
        err = MP_VAL;
    }
    if ((err == MP_OKAY) && (rem != NULL)) {
        /* Ensure remainder has enough memory. */
        if ((a->used <= d->used) && (rem->size < a->used + 1)) {
            err = MP_VAL;
        }
        else if ((a->used > d->used) && (rem->size < d->used + 1)) {
            err = MP_VAL;
        }
    }
    if (err == MP_OKAY) {
        if (a->used == SP_INT_DIGITS) {
            /* May need to shift number being divided left into a new word. */
            int bits = SP_WORD_SIZE - (sp_count_bits(d) % SP_WORD_SIZE);
            if ((bits != SP_WORD_SIZE) &&
                    (sp_count_bits(a) + bits > SP_INT_DIGITS * SP_WORD_SIZE)) {
                err = MP_VAL;
            }
            else {
                used = SP_INT_DIGITS;
            }
        }
        else {
            used = a->used + 1;
        }
    }

    if (err == MP_OKAY) {
    #if 0
        sp_print(a, "a");
        sp_print(d, "b");
    #endif
        /* Do operation. */
        err = _sp_div(a, d, r, rem, used);
    #if 0
        if (err == MP_OKAY) {
            if (rem != NULL) {
                sp_print(rem, "rdr");
            }
            if (r != NULL) {
                sp_print(r, "rdw");
            }
        }
    #endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC || \
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
     !defined(WOLFSSL_RSA_PUBLIC_ONLY))
#ifndef FREESCALE_LTC_TFM
#ifdef WOLFSSL_SP_INT_NEGATIVE
/* Calculate the remainder of dividing a by m: r = a mod m. r is m.
 *
 * @param  [in]   a  SP integer to reduce.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    /* Remainder will start as a. */
    DECL_SP_INT(t, (a == NULL) ? 1 : a->used + 1);

    /* In case remainder is modulus - allocate temporary. */
    ALLOC_SP_INT(t, a->used + 1, err, NULL);
    if (err == MP_OKAY) {
        _sp_init_size(t, a->used + 1);
        /* Use divide to calculate remainder and don't get quotient. */
        err = sp_div(a, m, NULL, t);
    }
    if (err == MP_OKAY) {
        /* Make remainder positive and copy into result. */
        if ((!sp_iszero(t)) && (t->sign != m->sign)) {
            err = sp_add(t, m, r);
        }
        else {
            _sp_copy(t, r);
        }
    }
    FREE_SP_INT(t, NULL);

    return err;
}
#endif

/* Calculate the remainder of dividing a by m: r = a mod m.
 *
 * @param  [in]   a  SP integer to reduce.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to store result in.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, m or r is NULL or m is 0.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_mod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure a isn't too big a number to operate on. */
    else if (a->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }

#ifndef WOLFSSL_SP_INT_NEGATIVE
    if (err == MP_OKAY) {
        /* Use divide to calculate remainder and don't get quotient. */
        err = sp_div(a, m, NULL, r);
    }
#else
    if ((err == MP_OKAY) && (r != m)) {
        err = sp_div(a, m, NULL, r);
        if ((err == MP_OKAY) && (!sp_iszero(r)) && (r->sign != m->sign)) {
            err = sp_add(r, m, r);
        }
    }
    else if (err == MP_OKAY) {
        err = _sp_mod(a, m, r);
    }
#endif /* WOLFSSL_SP_INT_NEGATIVE */

    return err;
}
#endif /* !FREESCALE_LTC_TFM */
#endif /* WOLFSSL_SP_MATH_ALL || !NO_DH || HAVE_ECC || \
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(HAVE_ECC) || !defined(NO_RSA)

/* START SP_MUL implementations. */
/* This code is generated.
 * To generate:
 *   cd scripts/sp/sp_int
 *   ./gen.sh
 * File sp_mul.c contains code.
 */

#ifdef SQR_MUL_ASM
/* Multiply a by b into r where a and b have same no. digits. r = a * b
 *
 * Optimised code for when number of digits in a and b are the same.
 *
 * @param  [in]   a    SP integer to mulitply.
 * @param  [in]   b    SP integer to mulitply by.
 * @param  [out]  r    SP integer to hod reult.
 *
 * @return  MP_OKAY otherwise.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_nxn(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    int j;
    unsigned int k;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
    sp_int_digit t[a->used];
#else
    sp_int_digit t[SP_INT_DIGITS / 2];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * a->used, NULL,
        DYNAMIC_TYPE_BIGINT);
    if (t == NULL) {
        err = MP_MEM;
    }
#endif
    if (err == MP_OKAY) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o;
        const sp_int_digit* dp;

        h = 0;
        l = 0;
        SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
        t[0] = h;
        h = 0;
        o = 0;
        for (k = 1; k <= a->used - 1; k++) {
            j = (int)k;
            dp = a->dp;
            for (; j >= 0; dp++, j--) {
                SP_ASM_MUL_ADD(l, h, o, dp[0], b->dp[j]);
            }
            t[k] = l;
            l = h;
            h = o;
            o = 0;
        }
        for (; k <= (a->used - 1) * 2; k++) {
            i = k - (b->used - 1);
            dp = &b->dp[b->used - 1];
            for (; i < a->used; i++, dp--) {
                SP_ASM_MUL_ADD(l, h, o, a->dp[i], dp[0]);
            }
            r->dp[k] = l;
            l = h;
            h = o;
            o = 0;
        }
        r->dp[k] = l;
        XMEMCPY(r->dp, t, a->used * sizeof(sp_int_digit));
        r->used = k + 1;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}

/* Multiply a by b into r. r = a * b
 *
 * @param  [in]   a    SP integer to mulitply.
 * @param  [in]   b    SP integer to mulitply by.
 * @param  [out]  r    SP integer to hod reult.
 *
 * @return  MP_OKAY otherwise.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    int j;
    unsigned int k;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
    sp_int_digit t[a->used + b->used];
#else
    sp_int_digit t[SP_INT_DIGITS];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * (a->used + b->used), NULL,
        DYNAMIC_TYPE_BIGINT);
    if (t == NULL) {
        err = MP_MEM;
    }
#endif
    if (err == MP_OKAY) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o;

        h = 0;
        l = 0;
        SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
        t[0] = h;
        h = 0;
        o = 0;
        for (k = 1; k <= b->used - 1; k++) {
            i = 0;
            j = (int)k;
            for (; (i < a->used) && (j >= 0); i++, j--) {
                SP_ASM_MUL_ADD(l, h, o, a->dp[i], b->dp[j]);
            }
            t[k] = l;
            l = h;
            h = o;
            o = 0;
        }
        for (; k <= (a->used - 1) + (b->used - 1); k++) {
            j = (int)(b->used - 1);
            i = k - (unsigned int)j;
            for (; (i < a->used) && (j >= 0); i++, j--) {
                SP_ASM_MUL_ADD(l, h, o, a->dp[i], b->dp[j]);
            }
            t[k] = l;
            l = h;
            h = o;
            o = 0;
        }
        t[k] = l;
        r->used = k + 1;
        XMEMCPY(r->dp, t, r->used * sizeof(sp_int_digit));
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#else
/* Multiply a by b into r. r = a * b
 *
 * @param  [in]   a    SP integer to mulitply.
 * @param  [in]   b    SP integer to mulitply by.
 * @param  [out]  r    SP integer to hod reult.
 *
 * @return  MP_OKAY otherwise.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    int j;
    unsigned int k;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
    sp_int_digit t[a->used + b->used];
#else
    sp_int_digit t[SP_INT_DIGITS];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * (a->used + b->used), NULL,
        DYNAMIC_TYPE_BIGINT);
    if (t == NULL) {
        err = MP_MEM;
    }
#endif
    if (err == MP_OKAY) {
        sp_int_word w;
        sp_int_word l;
        sp_int_word h;
    #ifdef SP_WORD_OVERFLOW
        sp_int_word o;
    #endif

        w = (sp_int_word)a->dp[0] * b->dp[0];
        t[0] = (sp_int_digit)w;
        l = (sp_int_digit)(w >> SP_WORD_SIZE);
        h = 0;
    #ifdef SP_WORD_OVERFLOW
        o = 0;
    #endif
        for (k = 1; k <= (a->used - 1) + (b->used - 1); k++) {
            i = k - (b->used - 1);
            i &= (((unsigned int)i >> (sizeof(i) * 8 - 1)) - 1U);
            j = (int)(k - i);
            for (; (i < a->used) && (j >= 0); i++, j--) {
                w = (sp_int_word)a->dp[i] * b->dp[j];
                l += (sp_int_digit)w;
                h += (sp_int_digit)(w >> SP_WORD_SIZE);
            #ifdef SP_WORD_OVERFLOW
                h += (sp_int_digit)(l >> SP_WORD_SIZE);
                l &= SP_MASK;
                o += (sp_int_digit)(h >> SP_WORD_SIZE);
                h &= SP_MASK;
            #endif
            }
            t[k] = (sp_int_digit)l;
            l >>= SP_WORD_SIZE;
            l += (sp_int_digit)h;
            h >>= SP_WORD_SIZE;
        #ifdef SP_WORD_OVERFLOW
            h += o & SP_MASK;
            o >>= SP_WORD_SIZE;
        #endif
        }
        t[k] = (sp_int_digit)l;
        r->used = k + 1;
        XMEMCPY(r->dp, t, r->used * sizeof(sp_int_digit));
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#endif

#ifndef WOLFSSL_SP_SMALL
#if !defined(WOLFSSL_HAVE_SP_ECC) && defined(HAVE_ECC)
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 256)
#ifndef SQR_MUL_ASM
/* Multiply a by b and store in r: r = a * b
 *
 * Long-hand implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_4(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_word* w = NULL;
#else
    sp_int_word w[16];
#endif
    const sp_int_digit* da = a->dp;
    const sp_int_digit* db = b->dp;

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    w = (sp_int_word*)XMALLOC(sizeof(sp_int_word) * 16, NULL,
        DYNAMIC_TYPE_BIGINT);
    if (w == NULL) {
        err = MP_MEM;
    }
#endif

    if (err == MP_OKAY) {
        w[0] = (sp_int_word)da[0] * db[0];
        w[1] = (sp_int_word)da[0] * db[1];
        w[2] = (sp_int_word)da[1] * db[0];
        w[3] = (sp_int_word)da[0] * db[2];
        w[4] = (sp_int_word)da[1] * db[1];
        w[5] = (sp_int_word)da[2] * db[0];
        w[6] = (sp_int_word)da[0] * db[3];
        w[7] = (sp_int_word)da[1] * db[2];
        w[8] = (sp_int_word)da[2] * db[1];
        w[9] = (sp_int_word)da[3] * db[0];
        w[10] = (sp_int_word)da[1] * db[3];
        w[11] = (sp_int_word)da[2] * db[2];
        w[12] = (sp_int_word)da[3] * db[1];
        w[13] = (sp_int_word)da[2] * db[3];
        w[14] = (sp_int_word)da[3] * db[2];
        w[15] = (sp_int_word)da[3] * db[3];

        r->dp[0] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[1];
        w[0] += (sp_int_digit)w[2];
        r->dp[1] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[1] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[1];
        w[2] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[2];
        w[0] += (sp_int_digit)w[3];
        w[0] += (sp_int_digit)w[4];
        w[0] += (sp_int_digit)w[5];
        r->dp[2] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[3] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[3];
        w[4] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[4];
        w[5] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[5];
        w[0] += (sp_int_digit)w[6];
        w[0] += (sp_int_digit)w[7];
        w[0] += (sp_int_digit)w[8];
        w[0] += (sp_int_digit)w[9];
        r->dp[3] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[6] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[6];
        w[7] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[7];
        w[8] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[8];
        w[9] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[9];
        w[0] += (sp_int_digit)w[10];
        w[0] += (sp_int_digit)w[11];
        w[0] += (sp_int_digit)w[12];
        r->dp[4] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[10] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[10];
        w[11] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[11];
        w[12] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[12];
        w[0] += (sp_int_digit)w[13];
        w[0] += (sp_int_digit)w[14];
        r->dp[5] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[13] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[13];
        w[14] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[14];
        w[0] += (sp_int_digit)w[15];
        r->dp[6] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[15] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[15];
        r->dp[7] = (sp_int_digit)w[0];

        r->used = 8;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (w != NULL) {
        XFREE(w, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#else /* SQR_MUL_ASM */
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_4(const sp_int* a, const sp_int* b, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit t[4];

    SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
    r->dp[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
    r->dp[5] = l;
    l = h;
    h = o;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[3], b->dp[3]);
    r->dp[6] = l;
    r->dp[7] = h;
    XMEMCPY(r->dp, t, 4 * sizeof(sp_int_digit));
    r->used = 8;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_6(const sp_int* a, const sp_int* b, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit t[6];

    SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[0]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[0]);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[1]);
    r->dp[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[2]);
    r->dp[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[3]);
    r->dp[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[4]);
    r->dp[9] = l;
    l = h;
    h = o;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[5], b->dp[5]);
    r->dp[10] = l;
    r->dp[11] = h;
    XMEMCPY(r->dp, t, 6 * sizeof(sp_int_digit));
    r->used = 12;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 256)
#ifdef SQR_MUL_ASM
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_8(const sp_int* a, const sp_int* b, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit t[8];

    SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[0]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[0]);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[0]);
    t[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[0]);
    t[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[1]);
    r->dp[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[2]);
    r->dp[9] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[3]);
    r->dp[10] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[4]);
    r->dp[11] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[5]);
    r->dp[12] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[6]);
    r->dp[13] = l;
    l = h;
    h = o;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[7], b->dp[7]);
    r->dp[14] = l;
    r->dp[15] = h;
    XMEMCPY(r->dp, t, 8 * sizeof(sp_int_digit));
    r->used = 16;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_12(const sp_int* a, const sp_int* b, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit t[12];

    SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[0]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[0]);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[0]);
    t[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[0]);
    t[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[0]);
    t[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[0]);
    t[9] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[0]);
    t[10] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[1]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[0]);
    t[11] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[2]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[1]);
    r->dp[12] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[3]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[2]);
    r->dp[13] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[4]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[3]);
    r->dp[14] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[5]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[4]);
    r->dp[15] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[6]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[5]);
    r->dp[16] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[7]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[6]);
    r->dp[17] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[8]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[7]);
    r->dp[18] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[9]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[8]);
    r->dp[19] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[10]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[9]);
    r->dp[20] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[11]);
    SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[10]);
    r->dp[21] = l;
    l = h;
    h = o;
    SP_ASM_MUL_ADD_NO(l, h, a->dp[11], b->dp[11]);
    r->dp[22] = l;
    r->dp[23] = h;
    XMEMCPY(r->dp, t, 12 * sizeof(sp_int_digit));
    r->used = 24;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#endif /* !WOLFSSL_HAVE_SP_ECC && HAVE_ECC */

#if defined(SQR_MUL_ASM) && (defined(WOLFSSL_SP_INT_LARGE_COMBA) || \
    (!defined(WOLFSSL_SP_MATH) && defined(WOLFCRYPT_HAVE_SAKKE) && \
    (SP_WORD_SIZE == 64)))
    #if SP_INT_DIGITS >= 32
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_16(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#else
    sp_int_digit t[16];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
     t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * 16, NULL,
         DYNAMIC_TYPE_BIGINT);
     if (t == NULL) {
         err = MP_MEM;
     }
#endif
    if (err == MP_OKAY) {
        SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
        t[0] = h;
        h = 0;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
        t[1] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
        t[2] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
        t[3] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[0]);
        t[4] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[0]);
        t[5] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[0]);
        t[6] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[0]);
        t[7] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[0]);
        t[8] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[0]);
        t[9] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[0]);
        t[10] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[0]);
        t[11] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[0]);
        t[12] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[0]);
        t[13] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[0]);
        t[14] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[0]);
        t[15] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[1]);
        r->dp[16] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[2]);
        r->dp[17] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[3]);
        r->dp[18] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[4]);
        r->dp[19] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[5]);
        r->dp[20] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[6]);
        r->dp[21] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[7]);
        r->dp[22] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[8]);
        r->dp[23] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[9]);
        r->dp[24] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[10]);
        r->dp[25] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[11]);
        r->dp[26] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[12]);
        r->dp[27] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[13]);
        r->dp[28] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[14]);
        r->dp[29] = l;
        l = h;
        h = o;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[15], b->dp[15]);
        r->dp[30] = l;
        r->dp[31] = h;
        XMEMCPY(r->dp, t, 16 * sizeof(sp_int_digit));
        r->used = 32;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
    #endif /* SP_INT_DIGITS >= 32 */
#endif /* SQR_MUL_ASM && (WOLFSSL_SP_INT_LARGE_COMBA || !WOLFSSL_SP_MATH &&
        * WOLFCRYPT_HAVE_SAKKE && SP_WORD_SIZE == 64 */

#if defined(SQR_MUL_ASM) && defined(WOLFSSL_SP_INT_LARGE_COMBA)
    #if SP_INT_DIGITS >= 48
/* Multiply a by b and store in r: r = a * b
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_24(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#else
    sp_int_digit t[24];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
     t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * 24, NULL,
         DYNAMIC_TYPE_BIGINT);
     if (t == NULL) {
         err = MP_MEM;
     }
#endif
    if (err == MP_OKAY) {
        SP_ASM_MUL(h, l, a->dp[0], b->dp[0]);
        t[0] = h;
        h = 0;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[0]);
        t[1] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[0], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[0]);
        t[2] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[0]);
        t[3] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[0]);
        t[4] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[0]);
        t[5] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[0]);
        t[6] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[0]);
        t[7] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[0]);
        t[8] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[0]);
        t[9] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[0]);
        t[10] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[0]);
        t[11] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[0]);
        t[12] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[0]);
        t[13] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[0]);
        t[14] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[0]);
        t[15] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[0]);
        t[16] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[0]);
        t[17] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[0]);
        t[18] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[0]);
        t[19] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[0]);
        t[20] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[0]);
        t[21] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[0]);
        t[22] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[0], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[1]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[0]);
        t[23] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[1], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[2]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[1]);
        r->dp[24] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[2], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[3]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[2]);
        r->dp[25] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[3], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[4]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[3]);
        r->dp[26] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[4], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[5]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[4]);
        r->dp[27] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[5], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[6]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[5]);
        r->dp[28] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[6], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[7]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[6]);
        r->dp[29] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[7], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[8]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[7]);
        r->dp[30] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[8], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[9]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[8]);
        r->dp[31] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[9], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[10]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[9]);
        r->dp[32] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[10], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[11]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[10]);
        r->dp[33] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[11], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[12]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[11]);
        r->dp[34] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[12], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[13]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[12]);
        r->dp[35] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[13], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[14]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[13]);
        r->dp[36] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[14], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[15]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[14]);
        r->dp[37] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[15], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[16]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[15]);
        r->dp[38] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[16], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[17]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[16]);
        r->dp[39] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[17], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[18]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[17]);
        r->dp[40] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[18], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[19]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[18]);
        r->dp[41] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[19], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[20]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[19]);
        r->dp[42] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[20], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[21]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[20]);
        r->dp[43] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[21], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[22]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[21]);
        r->dp[44] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD(l, h, o, a->dp[22], b->dp[23]);
        SP_ASM_MUL_ADD(l, h, o, a->dp[23], b->dp[22]);
        r->dp[45] = l;
        l = h;
        h = o;
        SP_ASM_MUL_ADD_NO(l, h, a->dp[23], b->dp[23]);
        r->dp[46] = l;
        r->dp[47] = h;
        XMEMCPY(r->dp, t, 24 * sizeof(sp_int_digit));
        r->used = 48;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
    #endif /* SP_INT_DIGITS >= 48 */

    #if SP_INT_DIGITS >= 64
/* Multiply a by b and store in r: r = a * b
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_32(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* a1;
    sp_int* b1;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    sp_int_digit cb;
    DECL_SP_INT_ARRAY(t, 16, 2);
    DECL_SP_INT_ARRAY(z, 33, 2);

    ALLOC_SP_INT_ARRAY(t, 16, 2, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 33, 2, err, NULL);
    if (err == MP_OKAY) {
        a1 = t[0];
        b1 = t[1];
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[16], sizeof(sp_int_digit) * 16);
        a1->used = 16;
        XMEMCPY(b1->dp, &b->dp[16], sizeof(sp_int_digit) * 16);
        b1->used = 16;

        /* z2 = a1 * b1 */
        err = _sp_mul_16(a1, b1, z2);
    }
    if (err == MP_OKAY) {
        l = a1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, a->dp[0]);
        a1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 16; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;
        /* b01 = b0 + b1 */
        l = b1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, b->dp[0]);
        b1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 16; i++) {
            SP_ASM_ADDC(l, h, b1->dp[i]);
            SP_ASM_ADDC(l, h, b->dp[i]);
            b1->dp[i] = l;
            l = h;
            h = 0;
        }
        cb = l;

        /* z0 = a0 * b0 */
        err = _sp_mul_16(a, b, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) * (b0 + b1) */
        err = _sp_mul_16(a1, b1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 32) + (z1 - z0 - z2) << 16) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 16 */
        z1->dp[32] = ca & cb;
        l = 0;
        if (ca) {
            h = 0;
            for (i = 0; i < 16; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 16]);
                SP_ASM_ADDC(l, h, b1->dp[i]);
                z1->dp[i + 16] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[32] += l;
        l = 0;
        if (cb) {
            h = 0;
            for (i = 0; i < 16; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 16]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 16] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[32] += l;
        /* z1 = z1 - z0 - z1 */
        l = 0;
        h = 0;
        for (i = 0; i < 32; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 16; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 16]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 16] = l;
            l = h;
            h = 0;
        }
        for (; i < 33; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 16] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 32  */
        l = 0;
        h = 0;
        for (i = 0; i < 17; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 32]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        for (; i < 32; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        r->used = 64;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 64 */

    #if SP_INT_DIGITS >= 96
/* Multiply a by b and store in r: r = a * b
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_48(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* a1;
    sp_int* b1;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    sp_int_digit cb;
    DECL_SP_INT_ARRAY(t, 24, 2);
    DECL_SP_INT_ARRAY(z, 49, 2);

    ALLOC_SP_INT_ARRAY(t, 24, 2, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 49, 2, err, NULL);
    if (err == MP_OKAY) {
        a1 = t[0];
        b1 = t[1];
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[24], sizeof(sp_int_digit) * 24);
        a1->used = 24;
        XMEMCPY(b1->dp, &b->dp[24], sizeof(sp_int_digit) * 24);
        b1->used = 24;

        /* z2 = a1 * b1 */
        err = _sp_mul_24(a1, b1, z2);
    }
    if (err == MP_OKAY) {
        l = a1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, a->dp[0]);
        a1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 24; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;
        /* b01 = b0 + b1 */
        l = b1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, b->dp[0]);
        b1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 24; i++) {
            SP_ASM_ADDC(l, h, b1->dp[i]);
            SP_ASM_ADDC(l, h, b->dp[i]);
            b1->dp[i] = l;
            l = h;
            h = 0;
        }
        cb = l;

        /* z0 = a0 * b0 */
        err = _sp_mul_24(a, b, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) * (b0 + b1) */
        err = _sp_mul_24(a1, b1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 48) + (z1 - z0 - z2) << 24) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 24 */
        z1->dp[48] = ca & cb;
        l = 0;
        if (ca) {
            h = 0;
            for (i = 0; i < 24; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 24]);
                SP_ASM_ADDC(l, h, b1->dp[i]);
                z1->dp[i + 24] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[48] += l;
        l = 0;
        if (cb) {
            h = 0;
            for (i = 0; i < 24; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 24]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 24] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[48] += l;
        /* z1 = z1 - z0 - z1 */
        l = 0;
        h = 0;
        for (i = 0; i < 48; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 24; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 24]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 24] = l;
            l = h;
            h = 0;
        }
        for (; i < 49; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 24] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 48  */
        l = 0;
        h = 0;
        for (i = 0; i < 25; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 48]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        for (; i < 48; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        r->used = 96;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 96 */

    #if SP_INT_DIGITS >= 128
/* Multiply a by b and store in r: r = a * b
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_64(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* a1;
    sp_int* b1;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    sp_int_digit cb;
    DECL_SP_INT_ARRAY(t, 32, 2);
    DECL_SP_INT_ARRAY(z, 65, 2);

    ALLOC_SP_INT_ARRAY(t, 32, 2, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 65, 2, err, NULL);
    if (err == MP_OKAY) {
        a1 = t[0];
        b1 = t[1];
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[32], sizeof(sp_int_digit) * 32);
        a1->used = 32;
        XMEMCPY(b1->dp, &b->dp[32], sizeof(sp_int_digit) * 32);
        b1->used = 32;

        /* z2 = a1 * b1 */
        err = _sp_mul_32(a1, b1, z2);
    }
    if (err == MP_OKAY) {
        l = a1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, a->dp[0]);
        a1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 32; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;
        /* b01 = b0 + b1 */
        l = b1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, b->dp[0]);
        b1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 32; i++) {
            SP_ASM_ADDC(l, h, b1->dp[i]);
            SP_ASM_ADDC(l, h, b->dp[i]);
            b1->dp[i] = l;
            l = h;
            h = 0;
        }
        cb = l;

        /* z0 = a0 * b0 */
        err = _sp_mul_32(a, b, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) * (b0 + b1) */
        err = _sp_mul_32(a1, b1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 64) + (z1 - z0 - z2) << 32) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 32 */
        z1->dp[64] = ca & cb;
        l = 0;
        if (ca) {
            h = 0;
            for (i = 0; i < 32; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 32]);
                SP_ASM_ADDC(l, h, b1->dp[i]);
                z1->dp[i + 32] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[64] += l;
        l = 0;
        if (cb) {
            h = 0;
            for (i = 0; i < 32; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 32]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 32] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[64] += l;
        /* z1 = z1 - z0 - z1 */
        l = 0;
        h = 0;
        for (i = 0; i < 64; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 32; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 32]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        for (; i < 65; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 64  */
        l = 0;
        h = 0;
        for (i = 0; i < 33; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 64]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 64] = l;
            l = h;
            h = 0;
        }
        for (; i < 64; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 64] = l;
            l = h;
            h = 0;
        }
        r->used = 128;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 128 */

    #if SP_INT_DIGITS >= 192
/* Multiply a by b and store in r: r = a * b
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mul_96(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* a1;
    sp_int* b1;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    sp_int_digit cb;
    DECL_SP_INT_ARRAY(t, 48, 2);
    DECL_SP_INT_ARRAY(z, 97, 2);

    ALLOC_SP_INT_ARRAY(t, 48, 2, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 97, 2, err, NULL);
    if (err == MP_OKAY) {
        a1 = t[0];
        b1 = t[1];
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[48], sizeof(sp_int_digit) * 48);
        a1->used = 48;
        XMEMCPY(b1->dp, &b->dp[48], sizeof(sp_int_digit) * 48);
        b1->used = 48;

        /* z2 = a1 * b1 */
        err = _sp_mul_48(a1, b1, z2);
    }
    if (err == MP_OKAY) {
        l = a1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, a->dp[0]);
        a1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 48; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;
        /* b01 = b0 + b1 */
        l = b1->dp[0];
        h = 0;
        SP_ASM_ADDC(l, h, b->dp[0]);
        b1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 48; i++) {
            SP_ASM_ADDC(l, h, b1->dp[i]);
            SP_ASM_ADDC(l, h, b->dp[i]);
            b1->dp[i] = l;
            l = h;
            h = 0;
        }
        cb = l;

        /* z0 = a0 * b0 */
        err = _sp_mul_48(a, b, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) * (b0 + b1) */
        err = _sp_mul_48(a1, b1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 96) + (z1 - z0 - z2) << 48) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 48 */
        z1->dp[96] = ca & cb;
        l = 0;
        if (ca) {
            h = 0;
            for (i = 0; i < 48; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 48]);
                SP_ASM_ADDC(l, h, b1->dp[i]);
                z1->dp[i + 48] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[96] += l;
        l = 0;
        if (cb) {
            h = 0;
            for (i = 0; i < 48; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 48]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 48] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[96] += l;
        /* z1 = z1 - z0 - z1 */
        l = 0;
        h = 0;
        for (i = 0; i < 96; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 48; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 48]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        for (; i < 97; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 96  */
        l = 0;
        h = 0;
        for (i = 0; i < 49; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 96]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 96] = l;
            l = h;
            h = 0;
        }
        for (; i < 96; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 96] = l;
            l = h;
            h = 0;
        }
        r->used = 192;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 192 */

#endif /* SQR_MUL_ASM && WOLFSSL_SP_INT_LARGE_COMBA */
#endif /* !WOLFSSL_SP_SMALL */

/* Multiply a by b and store in r: r = a * b
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b or is NULL; or the result will be too big for fixed
 *          data length.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_mul(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    unsigned int sign = MP_ZPOS;
#endif

    if ((a == NULL) || (b == NULL) || (r == NULL)) {
        err = MP_VAL;
    }

    /* Need extra digit during calculation. */
    if ((err == MP_OKAY) && (a->used + b->used > r->size)) {
        err = MP_VAL;
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(a, "a");
        sp_print(b, "b");
    }
#endif

    if (err == MP_OKAY) {
    #ifdef WOLFSSL_SP_INT_NEGATIVE
        sign = a->sign ^ b->sign;
    #endif

        if ((a->used == 0) || (b->used == 0)) {
            _sp_zero(r);
        }
        else
#ifndef WOLFSSL_SP_SMALL
#if !defined(WOLFSSL_HAVE_SP_ECC) && defined(HAVE_ECC)
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 256)
        if ((a->used == 4) && (b->used == 4)) {
            err = _sp_mul_4(a, b, r);
        }
        else
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
        if ((a->used == 6) && (b->used == 6)) {
            err = _sp_mul_6(a, b, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 256)
#ifdef SQR_MUL_ASM
        if ((a->used == 8) && (b->used == 8)) {
            err = _sp_mul_8(a, b, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
        if ((a->used == 12) && (b->used == 12)) {
            err = _sp_mul_12(a, b, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#endif /* !WOLFSSL_HAVE_SP_ECC && HAVE_ECC */
#if defined(SQR_MUL_ASM) && (defined(WOLFSSL_SP_INT_LARGE_COMBA) || \
    (!defined(WOLFSSL_SP_MATH) && defined(WOLFCRYPT_HAVE_SAKKE) && \
    (SP_WORD_SIZE == 64)))
    #if SP_INT_DIGITS >= 32
        if ((a->used == 16) && (b->used == 16)) {
            err = _sp_mul_16(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 32 */
#endif /* SQR_MUL_ASM && (WOLFSSL_SP_INT_LARGE_COMBA || !WOLFSSL_SP_MATH &&
        * WOLFCRYPT_HAVE_SAKKE && SP_WORD_SIZE == 64 */
#if defined(SQR_MUL_ASM) && defined(WOLFSSL_SP_INT_LARGE_COMBA)
    #if SP_INT_DIGITS >= 48
        if ((a->used == 24) && (b->used == 24)) {
            err = _sp_mul_24(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 48 */
    #if SP_INT_DIGITS >= 64
        if ((a->used == 32) && (b->used == 32)) {
            err = _sp_mul_32(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 64 */
    #if SP_INT_DIGITS >= 96
        if ((a->used == 48) && (b->used == 48)) {
            err = _sp_mul_48(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 96 */
    #if SP_INT_DIGITS >= 128
        if ((a->used == 64) && (b->used == 64)) {
            err = _sp_mul_64(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 128 */
    #if SP_INT_DIGITS >= 192
        if ((a->used == 96) && (b->used == 96)) {
            err = _sp_mul_96(a, b, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 192 */
#endif /* SQR_MUL_ASM && WOLFSSL_SP_INT_LARGE_COMBA */
#endif /* !WOLFSSL_SP_SMALL */

#ifdef SQR_MUL_ASM
        if (a->used == b->used) {
            err = _sp_mul_nxn(a, b, r);
        }
        else
#endif
        {
            err = _sp_mul(a, b, r);
        }
    }

#ifdef WOLFSSL_SP_INT_NEGATIVE
    if (err == MP_OKAY) {
        r->sign = (r->used == 0) ? MP_ZPOS : sign;
    }
#endif

#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rmul");
    }
#endif

    return err;
}
/* END SP_MUL implementations. */

#endif

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(WOLFCRYPT_HAVE_ECCSI) || \
    (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)) || defined(OPENSSL_ALL)
/* Multiply a by b mod m and store in r: r = (a * b) mod m
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mulmod_tmp(const sp_int* a, const sp_int* b, const sp_int* m,
    sp_int* r)
{
    int err = MP_OKAY;
    /* Create temporary for multiplication result. */
    DECL_SP_INT(t, a->used + b->used);

    ALLOC_SP_INT(t, a->used + b->used, err, NULL);
    if (err == MP_OKAY) {
        err = sp_init_size(t, a->used + b->used);
    }

    /* Multiply and reduce. */
    if (err == MP_OKAY) {
        err = sp_mul(a, b, t);
    }
    if (err == MP_OKAY) {
        err = sp_mod(t, m, r);
    }

    /* Dispose of an allocated SP int. */
    FREE_SP_INT(t, NULL);

    return err;
}

/* Multiply a by b mod m and store in r: r = (a * b) mod m
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_mulmod(const sp_int* a, const sp_int* b, const sp_int* m,
    sp_int* r)
{
    int err = MP_OKAY;

    /* Use r as intermediate result if not same as pointer m which is needed
     * after first intermediate result.
     */
    if (r != m) {
        /* Multiply and reduce. */
        err = sp_mul(a, b, r);
        if (err == MP_OKAY) {
            err = sp_mod(r, m, r);
        }
    }
    else {
        /* Do operation using temporary. */
        err = _sp_mulmod_tmp(a, b, m, r);
    }

    return err;
}

/* Multiply a by b mod m and store in r: r = (a * b) mod m
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   b  SP integer to multiply.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b, m or r is NULL; m is 0; or a * b is too big for
 *          fixed data length.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_mulmod(const sp_int* a, const sp_int* b, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure result SP int is big enough for intermediates. */
    if ((err == MP_OKAY) && (r != m) && (a->used + b->used > r->size)) {
        err = MP_VAL;
    }

#if 0
    if (err == 0) {
        sp_print(a, "a");
        sp_print(b, "b");
        sp_print(m, "m");
    }
#endif

    if (err == MP_OKAY) {
        err = _sp_mulmod(a, b, m, r);
    }

#if 0
    if (err == 0) {
        sp_print(r, "rmm");
    }
#endif

    return err;
}
#endif

#ifdef WOLFSSL_SP_INVMOD
/* Calculates the multiplicative inverse in the field. r*a = x*m + 1
 * Right-shift Algorithm. NOT constant time.
 *
 * Algorithm:
 *   1. u = m, v = a, b = 0, c = 1
 *   2. While v != 1 and u != 0
 *     2.1. If u even
 *       2.1.1. u /= 2
 *       2.1.2. b = (b / 2) mod m
 *     2.2. Else if v even
 *       2.2.1. v /= 2
 *       2.2.2. c = (c / 2) mod m
 *     2.3. Else if u >= v
 *       2.3.1. u -= v
 *       2.3.2. b = (c - b) mod m
 *     2.4. Else (v > u)
 *       2.4.1. v -= u
 *       2.4.2. c = (b - c) mod m
 *  3. NO_INVERSE if u == 0
 *
 * @param  [in]   a  SP integer to find inverse of.
 * @param  [in]   m  SP integer this is the modulus.
 * @param  [in]   u  SP integer to use in calculation.
 * @param  [in]   v  SP integer to use in calculation.
 * @param  [in]   b  SP integer to use in calculation
 * @param  [out]  c  SP integer that is the inverse.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when no inverse.
 */
static int _sp_invmod_bin(const sp_int* a, const sp_int* m, sp_int* u,
    sp_int* v, sp_int* b, sp_int* c)
{
    int err = MP_OKAY;

    /* 1. u = m, v = a, b = 0, c = 1 */
    _sp_copy(m, u);
    if (a != v) {
        _sp_copy(a, v);
    }
    _sp_zero(b);
    _sp_set(c, 1);

    /* 2. While v != 1 and u != 0 */
    while (!sp_isone(v) && !sp_iszero(u)) {
        /* 2.1. If u even */
        if ((u->dp[0] & 1) == 0) {
            /* 2.1.1. u /= 2 */
            _sp_div_2(u, u);
            /* 2.1.2. b = (b / 2) mod m */
            if (sp_isodd(b)) {
                _sp_add_off(b, m, b, 0);
            }
            _sp_div_2(b, b);
        }
        /* 2.2. Else if v even */
        else if ((v->dp[0] & 1) == 0) {
            /* 2.2.1. v /= 2 */
            _sp_div_2(v, v);
            /* 2.1.2. c = (c / 2) mod m */
            if (sp_isodd(c)) {
                _sp_add_off(c, m, c, 0);
            }
            _sp_div_2(c, c);
        }
        /* 2.3. Else if u >= v */
        else if (_sp_cmp_abs(u, v) != MP_LT) {
            /* 2.3.1. u -= v */
            _sp_sub_off(u, v, u, 0);
            /* 2.3.2. b = (c - b) mod m */
            if (_sp_cmp_abs(b, c) == MP_LT) {
                _sp_add_off(b, m, b, 0);
            }
            _sp_sub_off(b, c, b, 0);
        }
        /* 2.4. Else (v > u) */
        else {
            /* 2.4.1. v -= u */
            _sp_sub_off(v, u, v, 0);
            /* 2.4.2. c = (b - c) mod m */
            if (_sp_cmp_abs(c, b) == MP_LT) {
                _sp_add_off(c, m, c, 0);
            }
            _sp_sub_off(c, b, c, 0);
        }
    }
    /* 3. NO_INVERSE if u == 0 */
    if (sp_iszero(u)) {
        err = MP_VAL;
    }

    return err;
}

#if !defined(WOLFSSL_SP_LOW_MEM) && !defined(WOLFSSL_SP_SMALL) && \
    (!defined(NO_RSA) || !defined(NO_DH))
/* Calculates the multiplicative inverse in the field. r*a = x*m + 1
 * Extended Euclidean Algorithm. NOT constant time.
 *
 * Creates two new SP ints.
 *
 * Algorithm:
 *  1. x = m, y = a, b = 1, c = 0
 *  2. while x > 1
 *   2.1. d = x / y, r = x mod y
 *   2.2. c -= d * b
 *   2.3. x = y, y = r
 *   2.4. s = b, b = c, c = s
 *  3. If y != 0 then NO_INVERSE
 *  4. If c < 0 then c += m
 *  5. inv = c
 *
 * @param  [in]   a    SP integer to find inverse of.
 * @param  [in]   m    SP integer this is the modulus.
 * @param  [in]   u    SP integer to use in calculation.
 * @param  [in]   v    SP integer to use in calculation.
 * @param  [in]   b    SP integer to use in calculation
 * @param  [in]   c    SP integer to use in calculation
 * @param  [out]  inv  SP integer that is the inverse.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when no inverse.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_invmod_div(const sp_int* a, const sp_int* m, sp_int* x,
    sp_int* y, sp_int* b, sp_int* c, sp_int* inv)
{
    int err = MP_OKAY;
    sp_int* s;
#ifndef WOLFSSL_SP_INT_NEGATIVE
    int bneg = 0;
    int cneg = 0;
    int neg;
#endif
    DECL_SP_INT(d, m->used + 1);

    ALLOC_SP_INT(d, m->used + 1, err, NULL);
    if (err == MP_OKAY) {
        mp_init(d);

        /* 1. x = m, y = a, b = 1, c = 0 */
        if (a != y) {
            _sp_copy(a, y);
        }
        _sp_copy(m, x);
        _sp_set(b, 1);
        _sp_zero(c);
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* 2. while x > 1 */
    while ((err == MP_OKAY) && (!sp_isone(x)) && (!sp_iszero(x))) {
        /* 2.1. d = x / y, r = x mod y */
        err = sp_div(x, y, d, x);
        if (err == MP_OKAY) {
            /* 2.2. c -= d * b */
            if (sp_isone(d)) {
                /* c -= 1 * b */
                err = sp_sub(c, b, c);
            }
            else {
                /* d *= b */
                err = sp_mul(d, b, d);
                /* c -= d */
                if (err == MP_OKAY) {
                    err = sp_sub(c, d, c);
                }
            }
            /* 2.3. x = y, y = r */
            s = y; y = x; x = s;
            /* 2.4. s = b, b = c, c = s */
            s = b; b = c; c = s;
        }
    }
    /* 3. If y != 0 then NO_INVERSE */
    if ((err == MP_OKAY) && (!sp_iszero(y))) {
        err = MP_VAL;
    }
    /* 4. If c < 0 then c += m */
    if ((err == MP_OKAY) && sp_isneg(c)) {
        err = sp_add(c, m, c);
    }
    if (err == MP_OKAY) {
        /* 5. inv = c */
        err = sp_copy(c, inv);
    }
#else
    /* 2. while x > 1 */
    while ((err == MP_OKAY) && (!sp_isone(x)) && (!sp_iszero(x))) {
        /* 2.1. d = x / y, r = x mod y */
        err = sp_div(x, y, d, x);
        if (err == MP_OKAY) {
            if (sp_isone(d)) {
                /* c -= 1 * b */
                if ((bneg ^ cneg) == 1) {
                    /* c -= -b or -c -= b, therefore add. */
                    _sp_add_off(c, b, c, 0);
                }
                else if (_sp_cmp_abs(c, b) == MP_LT) {
                    /* |c| < |b| and same sign, reverse subtract and negate. */
                    _sp_sub_off(b, c, c, 0);
                    cneg = !cneg;
                }
                else {
                    /* |c| >= |b| */
                    _sp_sub_off(c, b, c, 0);
                }
            }
            else {
                /* d *= b */
                err = sp_mul(d, b, d);
                /* c -= d */
                if (err == MP_OKAY) {
                    if ((bneg ^ cneg) == 1) {
                        /* c -= -d or -c -= d, therefore add. */
                        _sp_add_off(c, d, c, 0);
                    }
                    else if (_sp_cmp_abs(c, d) == MP_LT) {
                        /* |c| < |d| and same sign, reverse subtract and negate.
                         */
                        _sp_sub_off(d, c, c, 0);
                        cneg = !cneg;
                    }
                    else {
                        _sp_sub_off(c, d, c, 0);
                    }
                }
            }
            /* 2.3. x = y, y = r */
            s = y; y = x; x = s;
            /* 2.4. s = b, b = c, c = s */
            s = b; b = c; c = s;
            neg = bneg; bneg = cneg; cneg = neg;
        }
    }
    /* 3. If y != 0 then NO_INVERSE */
    if ((err == MP_OKAY) && (!sp_iszero(y))) {
        err = MP_VAL;
    }
    /* 4. If c < 0 then c += m */
    if ((err == MP_OKAY) && cneg) {
        /* c = m - |c| */
        _sp_sub_off(m, c, c, 0);
    }
    if (err == MP_OKAY) {
        /* 5. inv = c */
        err = sp_copy(c, inv);
    }
#endif

    FREE_SP_INT(d, NULL);
    return err;
}
#endif

/* Calculates the multiplicative inverse in the field.
 * Right-shift Algorithm or Extended Euclidean Algorithm. NOT constant time.
 *
 * r*a = x*m + 1
 *
 * @param  [in]   a  SP integer to find inverse of.
 * @param  [in]   m  SP integer this is the modulus.
 * @param  [out]  r  SP integer to hold result. r cannot be m.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when m is even and a divides m evenly.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_invmod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    sp_int* u = NULL;
    sp_int* v = NULL;
    sp_int* b = NULL;
    DECL_SP_INT_ARRAY(t, m->used + 1, 3);
    DECL_SP_INT(c, 2 * m->used + 1);

    /* Allocate SP ints:
     *  - x3 one word larger than modulus
     *  - x1 one word longer than twice modulus used
     */
    ALLOC_SP_INT_ARRAY(t, m->used + 1, 3, err, NULL);
    ALLOC_SP_INT(c, 2 * m->used + 1, err, NULL);
    if (err == MP_OKAY) {
        u = t[0];
        v = t[1];
        b = t[2];
        /* c allocated separately and larger for even mod case. */
    }

    /* Initialize intermediate values with minimal sizes. */
    if (err == MP_OKAY) {
        err = sp_init_size(u, m->used + 1);
    }
    if (err == MP_OKAY) {
        err = sp_init_size(v, m->used + 1);
    }
    if (err == MP_OKAY) {
        err = sp_init_size(b, m->used + 1);
    }
    if (err == MP_OKAY) {
        err = sp_init_size(c, 2 * m->used + 1);
    }

    if (err == MP_OKAY) {
        const sp_int* mm = m;
        const sp_int* ma = a;
        int evenMod = 0;

        if (sp_iseven(m)) {
            /* a^-1 mod m = m + ((1 - m*(m^-1 % a)) / a) */
            mm = a;
            ma = v;
            _sp_copy(a, u);
            err = sp_mod(m, a, v);
            /* v == 0 when a divides m evenly - no inverse.  */
            if ((err == MP_OKAY) && sp_iszero(v)) {
                err = MP_VAL;
            }
            evenMod = 1;
        }

        if (err == MP_OKAY) {
            /* Calculate inverse. */
        #if !defined(WOLFSSL_SP_LOW_MEM) && !defined(WOLFSSL_SP_SMALL) && \
            (!defined(NO_RSA) || !defined(NO_DH))
            if (sp_count_bits(mm) >= 1024) {
                err = _sp_invmod_div(ma, mm, u, v, b, c, c);
            }
            else
        #endif
            {
                err = _sp_invmod_bin(ma, mm, u, v, b, c);
            }
        }

        /* Fixup for even modulus. */
        if ((err == MP_OKAY) && evenMod) {
            /* Finish operation.
             *    a^-1 mod m = m + ((1 - m*c) / a)
             * => a^-1 mod m = m - ((m*c - 1) / a)
             */
            err = sp_mul(c, m, c);
            if (err == MP_OKAY) {
                _sp_sub_d(c, 1, c);
                err = sp_div(c, a, c, NULL);
            }
            if (err == MP_OKAY) {
                err = sp_sub(m, c, r);
            }
        }
        else if (err == MP_OKAY) {
            _sp_copy(c, r);
        }
    }

    FREE_SP_INT(c, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

/* Calculates the multiplicative inverse in the field.
 * Right-shift Algorithm or Extended Euclidean Algorithm. NOT constant time.
 *
 * r*a = x*m + 1
 *
 * @param  [in]   a  SP integer to find inverse of.
 * @param  [in]   m  SP integer this is the modulus.
 * @param  [out]  r  SP integer to hold result. r cannot be m.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, m or r is NULL; a or m is zero; a and m are even or
 *          m is negative.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_invmod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || (r == NULL) || (r == m)) {
        err = MP_VAL;
    }
    if ((err == MP_OKAY) && (m->used * 2 > r->size)) {
        err = MP_VAL;
    }

#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Don't support negative modulus. */
    if ((err == MP_OKAY) && (m->sign == MP_NEG)) {
        err = MP_VAL;
    }
#endif

    if (err == MP_OKAY) {
        /* Ensure number is less than modulus. */
        if (_sp_cmp_abs(a, m) != MP_LT) {
            err = sp_mod(a, m, r);
            a = r;
        }
    }

#ifdef WOLFSSL_SP_INT_NEGATIVE
    if ((err == MP_OKAY) && (a->sign == MP_NEG)) {
        /* Make 'a' positive */
        err = sp_add(m, a, r);
        a = r;
    }
#endif

    /* 0 != n*m + 1 (+ve m), r*a mod 0 is always 0 (never 1)  */
    if ((err == MP_OKAY) && (sp_iszero(a) || sp_iszero(m))) {
        err = MP_VAL;
    }
    /* r*2*x != n*2*y + 1 for integer x,y */
    if ((err == MP_OKAY) && sp_iseven(a) && sp_iseven(m)) {
        err = MP_VAL;
    }
    /* 1*1 = 0*m + 1  */
    if ((err == MP_OKAY) && sp_isone(a)) {
        _sp_set(r, 1);
    }
    else if (err == MP_OKAY) {
        err = _sp_invmod(a, m, r);
    }

    return err;
}
#endif /* WOLFSSL_SP_INVMOD */

#ifdef WOLFSSL_SP_INVMOD_MONT_CT

/* Number of entries to pre-compute.
 * Many pre-defined primes have multiple of 8 consecutive 1s.
 * P-256 modulus - 2 => 32x1, 31x0, 1x1, 96x0, 94x1, 1x0, 1x1.
 */
#define CT_INV_MOD_PRE_CNT      8

/* Calculates the multiplicative inverse in the field - constant time.
 *
 * Modulus (m) must be a prime and greater than 2.
 * For prime m, inv = a ^ (m-2) mod m as 1 = a ^ (m-1) mod m.
 *
 * Algorithm:
 *  pre = pre-computed values, m = modulus, a = value to find inverse of,
 *  e = exponent
 *  Pre-calc:
 *   1. pre[0] = 2^0 * a mod m
 *   2. For i in 2..CT_INV_MOD_PRE_CNT
 *    2.1. pre[i-1] = ((pre[i-2] ^ 2) * a) mod m
 *  Calc inverse:
 *   1. e = m - 2
 *   2. j = Count leading 1's up to CT_INV_MOD_PRE_CNT
 *   3. t = pre[j-1]
 *   4. s = 0
 *   5. j = 0
 *   6. For i index of next top bit..0
 *    6.1. bit = e[i]
 *    6.2. j += bit
 *    6.3. s += 1
 *    6.4. if j == CT_INV_MOD_PRE_CNT or (bit == 0 and j > 0)
 *     6.4.1. s -= 1 - bit
 *     6.4.2. For s downto 1
 *      6.4.2.1. t = (t ^ 2) mod m
 *     6.4.3. s = 1 - bit
 *     6.4.4. t = (t * pre[j-1]) mod m
 *     6.4.5. j = 0
 *   7. For s downto 1
 *    7.1. t = (t ^ 2) mod m
 *   8. If j > 0 then r = (t * pre[j-1]) mod m
 *   9. Else r = t
 *
 * @param  [in]   a   SP integer, Montgomery form, to find inverse of.
 * @param  [in]   m   SP integer this is the modulus.
 * @param  [out]  r   SP integer to hold result.
 * @param  [in]   mp  SP integer digit that is the bottom digit of inv(-m).
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_invmod_mont_ct(const sp_int* a, const sp_int* m, sp_int* r,
    sp_int_digit mp)
{
    int err = MP_OKAY;
    int i;
    int j = 0;
    int s = 0;
    sp_int* t = NULL;
    sp_int* e = NULL;
#ifndef WOLFSSL_SP_NO_MALLOC
    DECL_DYN_SP_INT_ARRAY(pre, m->used * 2 + 1, CT_INV_MOD_PRE_CNT + 2);
#else
    DECL_SP_INT_ARRAY(pre, m->used * 2 + 1, CT_INV_MOD_PRE_CNT + 2);
#endif

#ifndef WOLFSSL_SP_NO_MALLOC
    ALLOC_DYN_SP_INT_ARRAY(pre, m->used * 2 + 1, CT_INV_MOD_PRE_CNT + 2, err,
        NULL);
#else
    ALLOC_SP_INT_ARRAY(pre, m->used * 2 + 1, CT_INV_MOD_PRE_CNT + 2, err, NULL);
#endif
    if (err == MP_OKAY) {
        t = pre[CT_INV_MOD_PRE_CNT + 0];
        e = pre[CT_INV_MOD_PRE_CNT + 1];
        /* Space for sqr and mul result. */
        _sp_init_size(t, m->used * 2 + 1);
        /* e = mod - 2 */
        _sp_init_size(e, m->used + 1);

        /* Create pre-computation results: ((2^(1..8))-1).a. */
        _sp_init_size(pre[0], m->used * 2 + 1);
        /* 1. pre[0] = 2^0 * a mod m
         *    Start with 1.a = a.
         */
        _sp_copy(a, pre[0]);
        /* 2. For i in 2..CT_INV_MOD_PRE_CNT
         *    For rest of entries in table.
         */
        for (i = 1; (err == MP_OKAY) && (i < CT_INV_MOD_PRE_CNT); i++) {
            /* 2.1 pre[i-1] = ((pre[i-1] ^ 2) * a) mod m */
            /* Previous value ..1 -> ..10 */
            _sp_init_size(pre[i], m->used * 2 + 1);
            err = sp_sqr(pre[i-1], pre[i]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(pre[i], m, mp);
            }
            /* ..10 -> ..11 */
            if (err == MP_OKAY) {
                err = sp_mul(pre[i], a, pre[i]);
            }
            if (err == MP_OKAY) {
                err = _sp_mont_red(pre[i], m, mp);
            }
        }
    }

    if (err == MP_OKAY) {
        /* 1. e = m - 2 */
        _sp_sub_d(m, 2, e);
        /* 2. j = Count leading 1's up to CT_INV_MOD_PRE_CNT
         *    One or more of the top bits is 1 so count.
         */
        for (i = sp_count_bits(e)-2, j = 1; i >= 0; i--, j++) {
            if ((!sp_is_bit_set(e, (unsigned int)i)) ||
                    (j == CT_INV_MOD_PRE_CNT)) {
                break;
            }
        }
        /* 3. Set tmp to product of leading bits. */
        _sp_copy(pre[j-1], t);

        /* 4. s = 0 */
        s = 0;
        /* 5. j = 0 */
        j = 0;
        /* 6. For i index of next top bit..0
         *    Do remaining bits in exponent.
         */
        for (; (err == MP_OKAY) && (i >= 0); i--) {
            /* 6.1. bit = e[i] */
            int bit = sp_is_bit_set(e, (unsigned int)i);

            /* 6.2. j += bit
             *      Update count of consequitive 1 bits.
             */
            j += bit;
            /* 6.3. s += 1
             *      Update count of squares required.
             */
            s++;

            /* 6.4. if j == CT_INV_MOD_PRE_CNT or (bit == 0 and j > 0)
             *      Check if max 1 bits or 0 and have seen at least one 1 bit.
             */
            if ((j == CT_INV_MOD_PRE_CNT) || ((!bit) && (j > 0))) {
                /* 6.4.1. s -= 1 - bit */
                bit = 1 - bit;
                s -= bit;
                /* 6.4.2. For s downto 1
                 *        Do s squares.
                 */
                for (; (err == MP_OKAY) && (s > 0); s--) {
                    /* 6.4.2.1. t = (t ^ 2) mod m */
                    err = sp_sqr(t, t);
                    if (err == MP_OKAY) {
                        err = _sp_mont_red(t, m, mp);
                    }
                }
                /* 6.4.3. s = 1 - bit */
                s = bit;

                /* 6.4.4. t = (t * pre[j-1]) mod m */
                if (err == MP_OKAY) {
                    err = sp_mul(t, pre[j-1], t);
                }
                if (err == MP_OKAY) {
                    err = _sp_mont_red(t, m, mp);
                }
                /* 6.4.5. j = 0
                 *        Reset number of 1 bits seen.
                 */
                j = 0;
            }
        }
    }
    if (err == MP_OKAY) {
        /* 7. For s downto 1
         *    Do s squares - total remaining. */
        for (; (err == MP_OKAY) && (s > 0); s--) {
            /* 7.1. t = (t ^ 2) mod m */
            err = sp_sqr(t, t);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t, m, mp);
            }
        }
    }
    if (err == MP_OKAY) {
        /* 8. If j > 0 then r = (t * pre[j-1]) mod m */
        if (j > 0) {
            err = sp_mul(t, pre[j-1], r);
            if (err == MP_OKAY) {
                err = _sp_mont_red(r, m, mp);
            }
        }
        /* 9. Else r = t */
        else {
            _sp_copy(t, r);
        }
    }

#ifndef WOLFSSL_SP_NO_MALLOC
    FREE_DYN_SP_INT_ARRAY(pre, NULL);
#else
    FREE_SP_INT_ARRAY(pre, NULL);
#endif
    return err;
}

/* Calculates the multiplicative inverse in the field - constant time.
 *
 * Modulus (m) must be a prime and greater than 2.
 * For prime m, inv = a ^ (m-2) mod m as 1 = a ^ (m-1) mod m.
 *
 * @param  [in]   a   SP integer, Montgomery form, to find inverse of.
 * @param  [in]   m   SP integer this is the modulus.
 * @param  [out]  r   SP integer to hold result.
 * @param  [in]   mp  SP integer digit that is the bottom digit of inv(-m).
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, m or r is NULL; a is 0 or m is less than 3.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_invmod_mont_ct(const sp_int* a, const sp_int* m, sp_int* r,
    sp_int_digit mp)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure m is not too big. */
    else if (m->used * 2 >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    /* check that r can hold the range of the modulus result */
    else if (m->used > r->size) {
        err = MP_VAL;
    }

    /* 0 != n*m + 1 (+ve m), r*a mod 0 is always 0 (never 1) */
    if ((err == MP_OKAY) && (sp_iszero(a) || sp_iszero(m) ||
            ((m->used == 1) && (m->dp[0] < 3)))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Do operation. */
        err = _sp_invmod_mont_ct(a, m, r, mp);
    }

    return err;
}

#endif /* WOLFSSL_SP_INVMOD_MONT_CT */


/**************************
 * Exponentiation functions
 **************************/

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH) || \
    defined(OPENSSL_ALL)

#ifndef WC_PROTECT_ENCRYPTED_MEM

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Process the exponent one bit at a time.
 * Is constant time and can be cache attack resistant.
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  1. s = 0
 *  2. t[0] = b mod m.
 *  3. t[1] = t[0]
 *  4. For i in (bits-1)...0
 *   4.1. t[s] = t[s] ^ 2
 *   4.2. y = e[i]
 *   4.3  j = y & s
 *   4.4  s = s | y
 *   4.5. t[j] = t[j] * b
 *  5. r = t[1]
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_ex(const sp_int* b, const sp_int* e, int bits,
    const sp_int* m, sp_int* r)
{
    int i;
    int err = MP_OKAY;
    int done = 0;
    /* 1. s = 0 */
    int s = 0;
#ifdef WC_NO_CACHE_RESISTANT
    DECL_SP_INT_ARRAY(t, 2 * m->used + 1, 2);
#else
    DECL_SP_INT_ARRAY(t, 2 * m->used + 1, 3);
#endif

    /* Allocate temporaries. */
#ifdef WC_NO_CACHE_RESISTANT
    ALLOC_SP_INT_ARRAY(t, 2 * m->used + 1, 2, err, NULL);
#else
    /* Working SP int needed when cache resistant. */
    ALLOC_SP_INT_ARRAY(t, 2 * m->used + 1, 3, err, NULL);
#endif
    if (err == MP_OKAY) {
        /* Initialize temporaries. */
        _sp_init_size(t[0], 2 * m->used + 1);
        _sp_init_size(t[1], 2 * m->used + 1);
    #ifndef WC_NO_CACHE_RESISTANT
        _sp_init_size(t[2], 2 * m->used + 1);
    #endif

        /* 2. t[0] = b mod m
         * Ensure base is less than modulus - set fake working value to base.
         */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[0]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[0])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into working variable. */
            _sp_copy(b, t[0]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        /* 3. t[1] = t[0]
         *    Set real working value to base.
         */
        _sp_copy(t[0], t[1]);

        /* 4. For i in (bits-1)...0 */
        for (i = bits - 1; (err == MP_OKAY) && (i >= 0); i--) {
#ifdef WC_NO_CACHE_RESISTANT
            /* 4.1. t[s] = t[s] ^ 2 */
            err = sp_sqrmod(t[s], m, t[s]);
            if (err == MP_OKAY) {
                /* 4.2. y = e[i] */
                int y = (e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1;
                /* 4.3. j = y & s */
                int j = y & s;
                /* 4.4  s = s | y */
                s |= y;
                /* 4.5. t[j] = t[j] * b */
                err = _sp_mulmod(t[j], b, m, t[j]);
            }
#else
            /* 4.1. t[s] = t[s] ^ 2 */
            _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[s^1]) +
                               ((size_t)t[1] & sp_off_on_addr[s  ])),
                     t[2]);
            err = sp_sqrmod(t[2], m, t[2]);
            _sp_copy(t[2],
                     (sp_int*)(((size_t)t[0] & sp_off_on_addr[s^1]) +
                               ((size_t)t[1] & sp_off_on_addr[s  ])));

            if (err == MP_OKAY) {
                /* 4.2. y = e[i] */
                int y = (int)((e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1);
                /* 4.3. j = y & s */
                int j = y & s;
                /* 4.4  s = s | y */
                s |= y;
                /* 4.5. t[j] = t[j] * b */
                _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[j^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[j  ])),
                         t[2]);
                err = _sp_mulmod(t[2], b, m, t[2]);
                _sp_copy(t[2],
                         (sp_int*)(((size_t)t[0] & sp_off_on_addr[j^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[j  ])));
            }
#endif
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* 5. r = t[1] */
        _sp_copy(t[1], r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

#else

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Process the exponent one bit at a time with base in Montgomery form.
 * Is constant time and cache attack resistant.
 *
 * Based on work by Marc Joye, Sung-Ming Yen, "The Montgomery Powering Ladder",
 * Cryptographic Hardware and Embedded Systems, CHES 2002
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  1. t[1] = b mod m.
 *  2. t[0] = 1
 *  3. For i in (bits-1)...0
 *   3.1. y = e[i]
 *   3.2. t[2] = t[0] * t[1]
 *   3.3. t[3] = t[y] ^ 2
 *   3.4. t[y] = t[3], t[y^1] = t[2]
 *  4. r = t[0]
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_ex(const sp_int* b, const sp_int* e, int bits,
    const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    int done = 0;
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, 4);

    /* Allocate temporaries. */
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, 4, err, NULL);
    if (err == MP_OKAY) {
        /* Initialize temporaries. */
        _sp_init_size(t[0], m->used * 2 + 1);
        _sp_init_size(t[1], m->used * 2 + 1);
        _sp_init_size(t[2], m->used * 2 + 1);
        _sp_init_size(t[3], m->used * 2 + 1);

        /* 1. Ensure base is less than modulus. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[1]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[1])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into working variable. */
            err = sp_copy(b, t[1]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        int i;

        /* 2. t[0] = 1 */
        _sp_set(t[0], 1);

        /* 3. For i in (bits-1)...0 */
        for (i = bits - 1; (err == MP_OKAY) && (i >= 0); i--) {
            /* 3.1. y = e[i] */
            int y = (e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1;

            /* 3.2. t[2] = t[0] * t[1] */
            err = sp_mulmod(t[0], t[1], m, t[2]);
            /* 3.3. t[3] = t[y] ^ 2 */
            if (err == MP_OKAY) {
                _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[y^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[y  ])),
                         t[3]);
                err = sp_sqrmod(t[3], m, t[3]);
            }
            /* 3.4. t[y] = t[3], t[y^1] = t[2] */
            if (err == MP_OKAY) {
                _sp_copy_2_ct(t[2], t[3], t[0], t[1], y, m->used);
            }
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* 4. r = t[0] */
        err = sp_copy(t[0], r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

#endif /* WC_PROTECT_ENCRYPTED_MEM */

#endif

#if (defined(WOLFSSL_SP_MATH_ALL) && ((!defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH))) || \
    defined(OPENSSL_ALL)
#ifndef WC_NO_HARDEN
#if !defined(WC_NO_CACHE_RESISTANT)

#ifndef WC_PROTECT_ENCRYPTED_MEM

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Process the exponent one bit at a time with base in Montgomery form.
 * Is constant time and cache attack resistant.
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  1. t[0] = b mod m.
 *  2. s = 0
 *  3. t[0] = ToMont(t[0])
 *  4. t[1] = t[0]
 *  5. bm = t[0]
 *  6. For i in (bits-1)...0
 *   6.1. t[s] = t[s] ^ 2
 *   6.2. y = e[i]
 *   6.3  j = y & s
 *   6.4  s = s | y
 *   6.5. t[j] = t[j] * bm
 *  7. t[1] = FromMont(t[1])
 *  8. r = t[1]
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_mont_ex(const sp_int* b, const sp_int* e, int bits,
    const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    int done = 0;
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, 4);

    /* Allocate temporaries. */
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, 4, err, NULL);
    if (err == MP_OKAY) {
        /* Initialize temporaries. */
        _sp_init_size(t[0], m->used * 2 + 1);
        _sp_init_size(t[1], m->used * 2 + 1);
        _sp_init_size(t[2], m->used * 2 + 1);
        _sp_init_size(t[3], m->used * 2 + 1);

        /* 1. Ensure base is less than modulus. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[0]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[0])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into working variable. */
            _sp_copy(b, t[0]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        int i;
        /* 2. s = 0 */
        int s = 0;
        sp_int_digit mp;

        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
        /* 3. t[0] = ToMont(t[0])
         *    Convert base to Montgomery form - as fake working value.
         */
        err = sp_mont_norm(t[1], m);
        if (err == MP_OKAY) {
            err = sp_mul(t[0], t[1], t[0]);
        }
        if (err == MP_OKAY) {
            /* t[0] = t[0] mod m, temporary size has to be bigger than t[0]. */
            err = _sp_div(t[0], m, NULL, t[0], t[0]->used + 1);
        }
        if (err == MP_OKAY) {
            /* 4. t[1] = t[0]
             *    Set real working value to base.
             */
            _sp_copy(t[0], t[1]);
            /* 5. bm = t[0]. */
            _sp_copy(t[0], t[2]);
        }

        /* 6. For i in (bits-1)...0 */
        for (i = bits - 1; (err == MP_OKAY) && (i >= 0); i--) {
            /* 6.1. t[s] = t[s] ^ 2 */
            _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[s^1]) +
                               ((size_t)t[1] & sp_off_on_addr[s  ])),
                     t[3]);
            err = sp_sqr(t[3], t[3]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[3], m, mp);
            }
            _sp_copy(t[3],
                     (sp_int*)(((size_t)t[0] & sp_off_on_addr[s^1]) +
                               ((size_t)t[1] & sp_off_on_addr[s  ])));

            if (err == MP_OKAY) {
                /* 6.2. y = e[i] */
                int y = (int)((e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1);
                /* 6.3  j = y & s */
                int j = y & s;
                /* 6.4  s = s | y */
                s |= y;

                /* 6.5. t[j] = t[j] * bm */
                _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[j^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[j  ])),
                         t[3]);
                err = sp_mul(t[3], t[2], t[3]);
                if (err == MP_OKAY) {
                    err = _sp_mont_red(t[3], m, mp);
                }
                _sp_copy(t[3],
                         (sp_int*)(((size_t)t[0] & sp_off_on_addr[j^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[j  ])));
            }
        }
        if (err == MP_OKAY) {
            /* 7. t[1] = FromMont(t[1]) */
            err = _sp_mont_red(t[1], m, mp);
            /* Reduction implementation returns number to range: 0..m-1. */
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* 8. r = t[1] */
        _sp_copy(t[1], r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

#else

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Process the exponent one bit at a time with base in Montgomery form.
 * Is constant time and cache attack resistant.
 *
 * Based on work by Marc Joye, Sung-Ming Yen, "The Montgomery Powering Ladder",
 * Cryptographic Hardware and Embedded Systems, CHES 2002
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  1. t[1] = b mod m.
 *  2. t[0] = ToMont(1)
 *  3. t[1] = ToMont(t[1])
 *  4. For i in (bits-1)...0
 *   4.1. y = e[i]
 *   4.2. t[2] = t[0] * t[1]
 *   4.3. t[3] = t[y] ^ 2
 *   4.4. t[y] = t[3], t[y^1] = t[2]
 *  5. t[0] = FromMont(t[0])
 *  6. r = t[0]
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_mont_ex(const sp_int* b, const sp_int* e, int bits,
    const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    int done = 0;
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, 4);

    /* Allocate temporaries. */
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, 4, err, NULL);
    if (err == MP_OKAY) {
        /* Initialize temporaries. */
        _sp_init_size(t[0], m->used * 2 + 1);
        _sp_init_size(t[1], m->used * 2 + 1);
        _sp_init_size(t[2], m->used * 2 + 1);
        _sp_init_size(t[3], m->used * 2 + 1);

        /* 1. Ensure base is less than modulus. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[1]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[1])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into working variable. */
            err = sp_copy(b, t[1]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        int i;
        sp_int_digit mp;

        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
        /* 2. t[0] = ToMont(1)
          *    Calculate 1 in Montgomery form.
          */
        err = sp_mont_norm(t[0], m);
        if (err == MP_OKAY) {
            /* 3. t[1] = ToMont(t[1])
             *    Convert base to Montgomery form.
             */
            err = sp_mulmod(t[1], t[0], m, t[1]);
        }

        /* 4. For i in (bits-1)...0 */
        for (i = bits - 1; (err == MP_OKAY) && (i >= 0); i--) {
            /* 4.1. y = e[i] */
            int y = (e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1;

            /* 4.2. t[2] = t[0] * t[1] */
            err = sp_mul(t[0], t[1], t[2]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[2], m, mp);
            }
            /* 4.3. t[3] = t[y] ^ 2 */
            if (err == MP_OKAY) {
                _sp_copy((sp_int*)(((size_t)t[0] & sp_off_on_addr[y^1]) +
                                   ((size_t)t[1] & sp_off_on_addr[y  ])),
                         t[3]);
                err = sp_sqr(t[3], t[3]);
            }
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[3], m, mp);
            }
            /* 4.4. t[y] = t[3], t[y^1] = t[2] */
            if (err == MP_OKAY) {
                _sp_copy_2_ct(t[2], t[3], t[0], t[1], y, m->used);
            }
        }

        if (err == MP_OKAY) {
            /* 5. t[0] = FromMont(t[0]) */
            err = _sp_mont_red(t[0], m, mp);
            /* Reduction implementation returns number to range: 0..m-1. */
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* 6. r = t[0] */
        err = sp_copy(t[0], r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

#endif /* WC_PROTECT_ENCRYPTED_MEM */

#else

#ifdef SP_ALLOC
#define SP_ALLOC_PREDEFINED
#endif
/* Always allocate large array of sp_ints unless defined WOLFSSL_SP_NO_MALLOC */
#define SP_ALLOC

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Creates a window of precalculated exponents with base in Montgomery form.
 * Is constant time but NOT cache attack resistant.
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  w: window size based on bits.
 *  1. t[1] = b mod m.
 *  2. t[0] = MontNorm(m) = ToMont(1)
 *  3. t[1] = ToMont(t[1])
 *  4. For i in 2..(2 ^ w) - 1
 *   4.1 if i[0] == 0 then t[i] = t[i/2] ^ 2
 *   4.2 if i[0] == 1 then t[i] = t[i-1] * t[1]
 *  5. cb = w * (bits / w)
 *  5. tr = t[e / (2 ^ cb)]
 *  6. For i in cb..w
 *   6.1. y = e[(i-1)..(i-w)]
 *   6.2. tr = tr ^ (2 * w)
 *   6.3. tr = tr * t[y]
 *  7. tr = FromMont(tr)
 *  8. r = tr
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_mont_ex(const sp_int* b, const sp_int* e, int bits,
    const sp_int* m, sp_int* r)
{
    int i;
    int c;
    int y;
    int winBits;
    int preCnt;
    int err = MP_OKAY;
    int done = 0;
    sp_int_digit mask;
    sp_int* tr = NULL;
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, (1 << 6) + 1);

    /* Window bits based on number of pre-calculations versus number of loop
     * calculcations.
     * Exponents for RSA and DH will result in 6-bit windows.
     */
    if (bits > 450) {
        winBits = 6;
    }
    else if (bits <= 21) {
        winBits = 1;
    }
    else if (bits <= 36) {
        winBits = 3;
    }
    else if (bits <= 140) {
        winBits = 4;
    }
    else {
        winBits = 5;
    }
    /* An entry for each possible 0..2^winBits-1 value. */
    preCnt = 1 << winBits;
    /* Mask for calculating index into pre-computed table. */
    mask = preCnt - 1;

    /* Allocate sp_ints for:
     *  - pre-computation table
     *  - temporary result
     */
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, preCnt + 1, err, NULL);
    if (err == MP_OKAY) {
        /* Set variable to use allocate memory. */
        tr = t[preCnt];

        /* Initialize all allocated. */
        for (i = 0; i < preCnt; i++) {
            _sp_init_size(t[i], m->used * 2 + 1);
        }
        _sp_init_size(tr, m->used * 2 + 1);

        /* 1. t[1] = b mod m. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[1]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[1])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into entry of table to contain b^1. */
            _sp_copy(b, t[1]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        sp_int_digit mp;
        sp_int_digit n;

        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
        /* 2. t[0] = MontNorm(m) = ToMont(1) */
        err = sp_mont_norm(t[0], m);
        if (err == MP_OKAY) {
            /* 3. t[1] = ToMont(t[1]) */
            err = sp_mul(t[1], t[0], t[1]);
        }
        if (err == MP_OKAY) {
            /* t[1] = t[1] mod m, temporary size has to be bigger than t[1]. */
            err = _sp_div(t[1], m, NULL, t[1], t[1]->used + 1);
        }

        /* 4. For i in 2..(2 ^ w) - 1 */
        for (i = 2; (i < preCnt) && (err == MP_OKAY); i++) {
            /* 4.1 if i[0] == 0 then t[i] = t[i/2] ^ 2 */
            if ((i & 1) == 0) {
                err = sp_sqr(t[i/2], t[i]);
            }
            /* 4.2 if i[0] == 1 then t[i] = t[i-1] * t[1] */
            else {
                err = sp_mul(t[i-1], t[1], t[i]);
            }
            /* Montgomery reduce square or multiplication result. */
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[i], m, mp);
            }
        }

        if (err == MP_OKAY) {
            /* 5. cb = w * (bits / w) */
            i = (bits - 1) >> SP_WORD_SHIFT;
            n = e->dp[i--];
            /* Find top bit index in last word. */
            c = bits & (SP_WORD_SIZE - 1);
            if (c == 0) {
                c = SP_WORD_SIZE;
            }
            /* Use as many bits from top to make remaining a multiple of window
             * size.
             */
            if ((bits % winBits) != 0) {
                c -= bits % winBits;
            }
            else {
                c -= winBits;
            }

            /* 5. tr = t[e / (2 ^ cb)] */
            y = (int)(n >> c);
            n <<= SP_WORD_SIZE - c;
            /* 5. Copy table value for first window. */
            _sp_copy(t[y], tr);

            /* 6. For i in cb..w */
            for (; (i >= 0) || (c >= winBits); ) {
                int j;

                /* 6.1. y = e[(i-1)..(i-w)] */
                if (c == 0) {
                    /* Bits up to end of digit */
                    n = e->dp[i--];
                    y = (int)(n >> (SP_WORD_SIZE - winBits));
                    n <<= winBits;
                    c = SP_WORD_SIZE - winBits;
                }
                else if (c < winBits) {
                    /* Bits to end of digit and part of next */
                    y = (int)(n >> (SP_WORD_SIZE - winBits));
                    n = e->dp[i--];
                    c = winBits - c;
                    y |= (int)(n >> (SP_WORD_SIZE - c));
                    n <<= c;
                    c = SP_WORD_SIZE - c;
                }
                else {
                    /* Bits from middle of digit */
                    y = (int)((n >> (SP_WORD_SIZE - winBits)) & mask);
                    n <<= winBits;
                    c -= winBits;
                }

                /* 6.2. tr = tr ^ (2 * w) */
                for (j = 0; (j < winBits) && (err == MP_OKAY); j++) {
                    err = sp_sqr(tr, tr);
                    if (err == MP_OKAY) {
                        err = _sp_mont_red(tr, m, mp);
                    }
                }

                /* 6.3. tr = tr * t[y] */
                if (err == MP_OKAY) {
                    err = sp_mul(tr, t[y], tr);
                }
                if (err == MP_OKAY) {
                    err = _sp_mont_red(tr, m, mp);
                }
            }
        }

        if (err == MP_OKAY) {
            /* 7. tr = FromMont(tr) */
            err = _sp_mont_red(tr, m, mp);
            /* Reduction implementation returns number to range: 0..m-1. */
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* 8. r = tr */
        _sp_copy(tr, r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

#ifndef SP_ALLOC_PREDEFINED
#undef SP_ALLOC
#undef SP_ALLOC_PREDEFINED
#endif

#endif /* !WC_NO_CACHE_RESISTANT */
#endif /* !WC_NO_HARDEN */

/* w = Log2(SP_WORD_SIZE) - 1 */
#if SP_WORD_SIZE == 8
    #define EXP2_WINSIZE    2
#elif SP_WORD_SIZE == 16
    #define EXP2_WINSIZE    3
#elif SP_WORD_SIZE == 32
    #define EXP2_WINSIZE    4
#elif SP_WORD_SIZE == 64
    #define EXP2_WINSIZE    5
#else
    #error "sp_exptmod_base_2: Unexpected SP_WORD_SIZE"
#endif
/* Mask is all bits in window set. */
#define EXP2_MASK           ((1 << EXP2_WINSIZE) - 1)

/* Internal. Exponentiates 2 to the power of e modulo m into r: r = 2 ^ e mod m
 * Is constant time and cache attack resistant.
 *
 * Calculates value to make mod operations constant time expect when
 * WC_NO_HARDERN defined or modulus fits in one word.
 *
 * Algorithm:
 *  b: base, e: exponent, m: modulus, r: result, bits: #bits to use
 *  w: window size based on #bits in word.
 *  1. if Words(m) > 1 then tr = MontNorm(m) = ToMont(1)
 *     else                 tr = 1
 *  2. if Words(m) > 1 and HARDEN then a = m * (2 ^ (2^w))
 *     else                            a = 0
 *  3. cb = w * (bits / w)
 *  4. y = e / (2 ^ cb)
 *  5. tr = (tr * (2 ^ y) + a) mod m
 *  6. For i in cb..w
 *   6.1. y = e[(i-1)..(i-w)]
 *   6.2. tr = tr ^ (2 * w)
 *   6.3. tr = ((tr * (2 ^ y) + a) mod m
 *  7. if Words(m) > 1 then tr = FromMont(tr)
 *  8. r = tr
 *
 * @param  [in]   e       SP integer that is the exponent.
 * @param  [in]   digits  Number of digits in base to use. May be greater than
 *                        count of bits in b.
 * @param  [in]   m       SP integer that is the modulus.
 * @param  [out]  r       SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_base_2(const sp_int* e, int digits, const sp_int* m,
    sp_int* r)
{
    int i = 0;
    int c = 0;
    int y;
    int err = MP_OKAY;
    sp_int_digit mp = 0;
    sp_int_digit n = 0;
#ifndef WC_NO_HARDEN
    sp_int* a = NULL;
    sp_int* tr = NULL;
    DECL_SP_INT_ARRAY(d, m->used * 2 + 1, 2);
#else
    DECL_SP_INT(tr, m->used * 2 + 1);
#endif
    int useMont = (m->used > 1);

#if 0
    sp_print_int(2, "a");
    sp_print(e, "b");
    sp_print(m, "m");
#endif

#ifndef WC_NO_HARDEN
    /* Allocate sp_ints for:
     *  - constant time add value for mod operation
     *  - temporary result
     */
    ALLOC_SP_INT_ARRAY(d, m->used * 2 + 1, 2, err, NULL);
#else
    /* Allocate sp_int for temporary result. */
    ALLOC_SP_INT(tr, m->used * 2 + 1, err, NULL);
#endif
    if (err == MP_OKAY) {
    #ifndef WC_NO_HARDEN
        a  = d[0];
        tr = d[1];

        _sp_init_size(a, m->used * 2 + 1);
    #endif
        _sp_init_size(tr, m->used * 2 + 1);

    }

    if ((err == MP_OKAY) && useMont) {
        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
    }
    if (err == MP_OKAY) {
        /* 1. if Words(m) > 1 then tr = MontNorm(m) = ToMont(1)
         *    else                 tr = 1
         */
        if (useMont) {
            /* Calculate Montgomery normalizer for modulus - 1 in Montgomery
             * form.
             */
            err = sp_mont_norm(tr, m);
        }
        else {
             /* For single word modulus don't use Montgomery form. */
            err = sp_set(tr, 1);
        }
    }
    /* 2. if Words(m) > 1 and HARDEN then a = m * (2 ^ (2^w))
     *    else                            a = 0
     */
#ifndef WC_NO_HARDEN
    if ((err == MP_OKAY) && useMont) {
        err = sp_mul_2d(m, 1 << EXP2_WINSIZE, a);
    }
#endif

    if (err == MP_OKAY) {
        /* 3. cb = w * (bits / w) */
        i = digits - 1;
        n = e->dp[i--];
        c = SP_WORD_SIZE;
    #if EXP2_WINSIZE != 1
        c -= (digits * SP_WORD_SIZE) % EXP2_WINSIZE;
        if (c != SP_WORD_SIZE) {
            /* 4. y = e / (2 ^ cb) */
            y = (int)(n >> c);
            n <<= SP_WORD_SIZE - c;
        }
        else
    #endif
        {
            /* 4. y = e / (2 ^ cb) */
            y = (int)((n >> (SP_WORD_SIZE - EXP2_WINSIZE)) & EXP2_MASK);
            n <<= EXP2_WINSIZE;
            c -= EXP2_WINSIZE;
        }

        /* 5. tr = (tr * (2 ^ y) + a) mod m */
        err = sp_mul_2d(tr, y, tr);
    }
#ifndef WC_NO_HARDEN
    if ((err == MP_OKAY) && useMont) {
        /* Add value to make mod operation constant time. */
        err = sp_add(tr, a, tr);
    }
#endif
    if (err == MP_OKAY) {
        err = sp_mod(tr, m, tr);
    }
    /* 6. For i in cb..w */
    for (; (err == MP_OKAY) && ((i >= 0) || (c >= EXP2_WINSIZE)); ) {
        int j;

        /* 6.1. y = e[(i-1)..(i-w)] */
        if (c == 0) {
            /* Bits from next digit. */
            n = e->dp[i--];
            y = (int)(n >> (SP_WORD_SIZE - EXP2_WINSIZE));
            n <<= EXP2_WINSIZE;
            c = SP_WORD_SIZE - EXP2_WINSIZE;
        }
    #if (EXP2_WINSIZE != 1) && (EXP2_WINSIZE != 2) && (EXP2_WINSIZE != 4)
        else if (c < EXP2_WINSIZE) {
            /* Bits to end of digit and part of next */
            y = (int)(n >> (SP_WORD_SIZE - EXP2_WINSIZE));
            n = e->dp[i--];
            c = EXP2_WINSIZE - c;
            y |= (int)(n >> (SP_WORD_SIZE - c));
            n <<= c;
            c = SP_WORD_SIZE - c;
        }
    #endif
        else {
            /* Bits from middle of digit */
            y = (int)((n >> (SP_WORD_SIZE - EXP2_WINSIZE)) & EXP2_MASK);
            n <<= EXP2_WINSIZE;
            c -= EXP2_WINSIZE;
        }

        /* 6.2. tr = tr ^ (2 * w) */
        for (j = 0; (j < EXP2_WINSIZE) && (err == MP_OKAY); j++) {
            err = sp_sqr(tr, tr);
            if (err == MP_OKAY) {
                if (useMont) {
                    err = _sp_mont_red(tr, m, mp);
                }
                else {
                    err = sp_mod(tr, m, tr);
                }
            }
        }

        /* 6.3. tr = ((tr * (2 ^ y) + a) mod m */
        if (err == MP_OKAY) {
            err = sp_mul_2d(tr, y, tr);
        }
    #ifndef WC_NO_HARDEN
        if ((err == MP_OKAY) && useMont) {
            /* Add value to make mod operation constant time. */
            err = sp_add(tr, a, tr);
        }
    #endif
        if (err == MP_OKAY) {
            /* Reduce current result by modulus. */
            err = sp_mod(tr, m, tr);
        }
    }

    /* 7. if Words(m) > 1 then tr = FromMont(tr) */
    if ((err == MP_OKAY) && useMont) {
        err = _sp_mont_red(tr, m, mp);
        /* Reduction implementation returns number to range: 0..m-1. */
    }
    if (err == MP_OKAY) {
        /* 8. r = tr */
        _sp_copy(tr, r);
    }

#if 0
    sp_print(r, "rme");
#endif

#ifndef WC_NO_HARDEN
    FREE_SP_INT_ARRAY(d, NULL);
#else
    FREE_SP_INT(tr, m->used * 2 + 1);
#endif
    return err;
}
#endif

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)) || \
    defined(OPENSSL_ALL)
/* Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 *
 * Error returned when parameters r == e or r == m and base >= modulus.
 *
 * @param  [in]   b       SP integer that is the base.
 * @param  [in]   e       SP integer that is the exponent.
 * @param  [in]   digits  Number of digits in exponent to use. May be greater
 *                        than count of digits in e.
 * @param  [in]   m       SP integer that is the modulus.
 * @param  [out]  r       SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when b, e, m or r is NULL, digits is negative, or m <= 0 or
 *          e is negative.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_exptmod_ex(const sp_int* b, const sp_int* e, int digits, const sp_int* m,
    sp_int* r)
{
    int err = MP_OKAY;
    int done = 0;
    int mBits = sp_count_bits(m);
    int bBits = sp_count_bits(b);
    int eBits = sp_count_bits(e);

    if ((b == NULL) || (e == NULL) || (m == NULL) || (r == NULL) ||
             (digits < 0)) {
        err = MP_VAL;
    }
    /* Ensure m is not too big. */
    else if (m->used * 2 >= SP_INT_DIGITS) {
        err = MP_VAL;
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(b, "a");
        sp_print(e, "b");
        sp_print(m, "m");
    }
#endif

    /* Check for invalid modulus. */
    if ((err == MP_OKAY) && sp_iszero(m)) {
        err = MP_VAL;
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Check for unsupported negative values of exponent and modulus. */
    if ((err == MP_OKAY) && ((e->sign == MP_NEG) || (m->sign == MP_NEG))) {
        err = MP_VAL;
    }
#endif

    /* Check for degenerate cases. */
    if ((err == MP_OKAY) && sp_isone(m)) {
        _sp_set(r, 0);
        done = 1;
    }
    if ((!done) && (err == MP_OKAY) && sp_iszero(e)) {
        _sp_set(r, 1);
        done = 1;
    }

    /* Ensure base is less than modulus. */
    if ((!done) && (err == MP_OKAY) && (_sp_cmp_abs(b, m) != MP_LT)) {
        if ((r == e) || (r == m)) {
            err = MP_VAL;
        }
        if (err == MP_OKAY) {
            err = sp_mod(b, m, r);
        }
        if (err == MP_OKAY) {
            b = r;
        }
    }
    /* Check for degenerate case of base. */
    if ((!done) && (err == MP_OKAY) && sp_iszero(b)) {
        _sp_set(r, 0);
        done = 1;
    }

    /* Ensure SP integers have space for intermediate values. */
    if ((!done) && (err == MP_OKAY) && (m->used * 2 >= r->size)) {
        err = MP_VAL;
    }

    if ((!done) && (err == MP_OKAY)) {
        /* Use code optimized for specific sizes if possible */
#if (defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL)) && \
    (defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH))
    #ifndef WOLFSSL_SP_NO_2048
        if ((mBits == 1024) && sp_isodd(m) && (bBits <= 1024) &&
                (eBits <= 1024)) {
            err = sp_ModExp_1024((sp_int*)b, (sp_int*)e, (sp_int*)m, r);
            done = 1;
        }
        else if ((mBits == 2048) && sp_isodd(m) && (bBits <= 2048) &&
                 (eBits <= 2048)) {
            err = sp_ModExp_2048((sp_int*)b, (sp_int*)e, (sp_int*)m, r);
            done = 1;
        }
        else
    #endif
    #ifndef WOLFSSL_SP_NO_3072
        if ((mBits == 1536) && sp_isodd(m) && (bBits <= 1536) &&
                (eBits <= 1536)) {
            err = sp_ModExp_1536((sp_int*)b, (sp_int*)e, (sp_int*)m, r);
            done = 1;
        }
        else if ((mBits == 3072) && sp_isodd(m) && (bBits <= 3072) &&
                 (eBits <= 3072)) {
            err = sp_ModExp_3072((sp_int*)b, (sp_int*)e, (sp_int*)m, r);
            done = 1;
        }
        else
    #endif
    #ifdef WOLFSSL_SP_4096
        if ((mBits == 4096) && sp_isodd(m) && (bBits <= 4096) &&
                (eBits <= 4096)) {
            err = sp_ModExp_4096((sp_int*)b, (sp_int*)e, (sp_int*)m, r);
            done = 1;
        }
        else
    #endif
#endif
        {
            /* SP does not support size. */
        }
    }
#if defined(WOLFSSL_SP_MATH_ALL) || !defined(NO_DH) || defined(OPENSSL_ALL)
#if (defined(WOLFSSL_RSA_VERIFY_ONLY) || defined(WOLFSSL_RSA_PUBLIC_ONLY)) && \
    defined(NO_DH)
    if ((!done) && (err == MP_OKAY)) {
        /* Use non-constant time version - fastest. */
        err = sp_exptmod_nct(b, e, m, r);
    }
#else
#if defined(WOLFSSL_SP_MATH_ALL) || defined(OPENSSL_ALL)
    if ((!done) && (err == MP_OKAY) && (b->used == 1) && (b->dp[0] == 2) &&
         mp_isodd(m)) {
        /* Use the generic base 2 implementation. */
        err = _sp_exptmod_base_2(e, digits, m, r);
    }
    else if ((!done) && (err == MP_OKAY) && ((m->used > 1) && mp_isodd(m))) {
    #ifndef WC_NO_HARDEN
        /* Use constant time version hardened against timing attacks and
         * cache attacks when WC_NO_CACHE_RESISTANT not defined. */
        err = _sp_exptmod_mont_ex(b, e, digits * SP_WORD_SIZE, m, r);
    #else
        /* Use non-constant time version - fastest. */
        err = sp_exptmod_nct(b, e, m, r);
    #endif
    }
    else
#endif /* WOLFSSL_SP_MATH_ALL || OPENSSL_ALL */
    if ((!done) && (err == MP_OKAY)) {
        /* Otherwise use the generic implementation hardened against
         * timing and cache attacks. */
        err = _sp_exptmod_ex(b, e, digits * SP_WORD_SIZE, m, r);
    }
#endif /* WOLFSSL_RSA_VERIFY_ONLY || WOLFSSL_RSA_PUBLIC_ONLY */
#else
    if ((!done) && (err == MP_OKAY)) {
        err = MP_VAL;
    }
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH */

    (void)mBits;
    (void)bBits;
    (void)eBits;
    (void)digits;

#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rme");
    }
#endif
    return err;
}
#endif

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    !defined(NO_DH) || (!defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)) || \
    defined(OPENSSL_ALL)
/* Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 *
 * @param  [in]   b  SP integer that is the base.
 * @param  [in]   e  SP integer that is the exponent.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when b, e, m or r is NULL; or m <= 0 or e is negative.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_exptmod(const sp_int* b, const sp_int* e, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((b == NULL) || (e == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    SAVE_VECTOR_REGISTERS(err = _svr_ret;);
    if (err == MP_OKAY) {
        err = sp_exptmod_ex(b, e, (int)e->used, m, r);
    }
    RESTORE_VECTOR_REGISTERS();
    return err;
}
#endif

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH)
#if defined(WOLFSSL_SP_FAST_NCT_EXPTMOD) || !defined(WOLFSSL_SP_SMALL)

/* Internal. Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Creates a window of precalculated exponents with base in Montgomery form.
 * Sliding window and is NOT constant time.
 *
 * n-bit window is: (b^(2^(n-1))*b^0)...(b^(2^(n-1))*b^(2^(n-1)-1))
 * e.g. when n=6, b^32..b^63
 * Algorithm:
 *   1. Ensure base is less than modulus.
 *   2. Convert base to Montgomery form
 *   3. Set result to table entry for top window bits, or
 *      if less than windows bits in exponent, 1 in Montgomery form.
 *   4. While at least window bits left:
 *     4.1. Count number of and skip leading 0 bits unless less then window bits
 *          left.
 *     4.2. Montgomery square result for each leading 0 and window bits if bits
 *          left.
 *     4.3. Break if less than window bits left.
 *     4.4. Get top window bits from expononent and drop.
 *     4.5. Montgomery multiply result by table entry.
 *   5. While bits left:
 *     5.1. Montogmery square result
 *     5.2. If exponent bit set
 *       5.2.1. Montgomery multiply result by Montgomery form of base.
 *   6. Convert result back from Montgomery form.
 *
 * @param  [in]   b     SP integer that is the base.
 * @param  [in]   e     SP integer that is the exponent.
 * @param  [in]   bits  Number of bits in exponent to use. May be greater than
 *                      count of bits in e.
 * @param  [in]   m     SP integer that is the modulus.
 * @param  [out]  r     SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_nct(const sp_int* b, const sp_int* e, const sp_int* m,
    sp_int* r)
{
    int i = 0;
    int bits;
    int winBits;
    int preCnt;
    int err = MP_OKAY;
    int done = 0;
    sp_int* tr = NULL;
    sp_int* bm = NULL;
    /* Maximum winBits is 6 and preCnt is (1 << (winBits - 1)). */
#ifndef WOLFSSL_SP_NO_MALLOC
    DECL_DYN_SP_INT_ARRAY(t, m->used * 2 + 1, (1 << 5) + 2);
#else
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, (1 << 5) + 2);
#endif

    bits = sp_count_bits(e);

    /* Window bits based on number of pre-calculations versus number of loop
     * calculations.
     * Exponents for RSA and DH will result in 6-bit windows.
     * Note: for 4096-bit values, 7-bit window is slightly better.
     */
    if (bits > 450) {
        winBits = 6;
    }
    else if (bits <= 21) {
        winBits = 1;
    }
    else if (bits <= 36) {
        winBits = 3;
    }
    else if (bits <= 140) {
        winBits = 4;
    }
    else {
        winBits = 5;
    }
    /* Top bit of exponent fixed as 1 for pre-calculated window. */
    preCnt = 1 << (winBits - 1);

    /* Allocate sp_ints for:
     *  - pre-computation table
     *  - temporary result
     *  - Montgomery form of base
     */
#ifndef WOLFSSL_SP_NO_MALLOC
    ALLOC_DYN_SP_INT_ARRAY(t, m->used * 2 + 1, (size_t)preCnt + 2, err, NULL);
#else
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, (size_t)preCnt + 2, err, NULL);
#endif
    if (err == MP_OKAY) {
        /* Set variables to use allocate memory. */
        tr = t[preCnt + 0];
        bm = t[preCnt + 1];

        /* Initialize all allocated  */
        for (i = 0; i < preCnt; i++) {
            _sp_init_size(t[i], m->used * 2 + 1);
        }
        _sp_init_size(tr, m->used * 2 + 1);
        _sp_init_size(bm, m->used * 2 + 1);

        /* 1. Ensure base is less than modulus. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, bm);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(bm)) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into Montogmery base variable. */
            _sp_copy(b, bm);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        int y = 0;
        int c = 0;
        sp_int_digit mp;

        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
        /* Calculate Montgomery normalizer for modulus. */
        err = sp_mont_norm(t[0], m);
        if (err == MP_OKAY) {
            /* 2. Convert base to Montgomery form. */
            err = sp_mul(bm, t[0], bm);
        }
        if (err == MP_OKAY) {
            /* bm = bm mod m, temporary size has to be bigger than bm->used. */
            err = _sp_div(bm, m, NULL, bm, bm->used + 1);
        }
        if (err == MP_OKAY) {
            /* Copy Montgomery form of base into first element of table. */
            _sp_copy(bm, t[0]);
        }
        /* Calculate b^(2^(winBits-1)) */
        for (i = 1; (i < winBits) && (err == MP_OKAY); i++) {
            err = sp_sqr(t[0], t[0]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[0], m, mp);
            }
        }
        /* For each table entry after first. */
        for (i = 1; (i < preCnt) && (err == MP_OKAY); i++) {
            /* Multiply previous entry by the base in Mont form into table. */
            err = sp_mul(t[i-1], bm, t[i]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[i], m, mp);
            }
        }

        /* 3. Set result to table entry for top window bits, or
         *    if less than windows bits in exponent, 1 in Montgomery form.
         */
        if (err == MP_OKAY) {
            sp_int_digit n;
            /* Mask for calculating index into pre-computed table. */
            sp_int_digit mask = (sp_int_digit)preCnt - 1;

            /* Find the top bit. */
            i = (bits - 1) >> SP_WORD_SHIFT;
            n = e->dp[i--];
            c = bits % SP_WORD_SIZE;
            if (c == 0) {
                c = SP_WORD_SIZE;
            }
            /* Put top bit at highest offset in digit. */
            n <<= SP_WORD_SIZE - c;

            if (bits >= winBits) {
                /* Top bit set. Copy from window. */
                if (c < winBits) {
                    /* Bits to end of digit and part of next */
                    y = (int)((n >> (SP_WORD_SIZE - winBits)) & mask);
                    n = e->dp[i--];
                    c = winBits - c;
                    y |= (int)(n >> (SP_WORD_SIZE - c));
                    n <<= c;
                    c = SP_WORD_SIZE - c;
                }
                else {
                    /* Bits from middle of digit */
                    y = (int)((n >> (SP_WORD_SIZE - winBits)) & mask);
                    n <<= winBits;
                    c -= winBits;
                }
                _sp_copy(t[y], tr);
            }
            else {
                /* 1 in Montgomery form. */
                err = sp_mont_norm(tr, m);
            }

            /* 4. While at least window bits left. */
            while ((err == MP_OKAY) && ((i >= 0) || (c >= winBits))) {
                /* Number of squares to before due to top bits being 0. */
                int sqrs = 0;

                /* 4.1. Count number of and skip leading 0 bits unless less
                 *      than window bits.
                 */
                do {
                    /* Make sure n has bits from the right digit. */
                    if (c == 0) {
                        n = e->dp[i--];
                        c = SP_WORD_SIZE;
                    }
                    /* Mask off the next bit. */
                    if ((n & ((sp_int_digit)1 << (SP_WORD_SIZE - 1))) != 0) {
                        break;
                    }

                    /* Another square needed. */
                    sqrs++;
                    /* Skip bit. */
                    n <<= 1;
                    c--;
                }
                while ((err == MP_OKAY) && ((i >= 0) || (c >= winBits)));

                if ((err == MP_OKAY) && ((i >= 0) || (c >= winBits))) {
                    /* Add squares needed before using table entry. */
                    sqrs += winBits;
                }

                /* 4.2. Montgomery square result for each leading 0 and window
                 *      bits if bits left.
                 */
                for (; (err == MP_OKAY) && (sqrs > 0); sqrs--) {
                    err = sp_sqr(tr, tr);
                    if (err == MP_OKAY) {
                        err = _sp_mont_red(tr, m, mp);
                    }
                }

                /* 4.3. Break if less than window bits left. */
                if ((err == MP_OKAY) && (i < 0) && (c < winBits)) {
                    break;
                }

                /* 4.4. Get top window bits from exponent and drop. */
                if (err == MP_OKAY) {
                    if (c == 0) {
                        /* Bits from next digit. */
                        n = e->dp[i--];
                        y = (int)(n >> (SP_WORD_SIZE - winBits));
                        n <<= winBits;
                        c = SP_WORD_SIZE - winBits;
                    }
                    else if (c < winBits) {
                        /* Bits to end of digit and part of next. */
                        y = (int)(n >> (SP_WORD_SIZE - winBits));
                        n = e->dp[i--];
                        c = winBits - c;
                        y |= (int)(n >> (SP_WORD_SIZE - c));
                        n <<= c;
                        c = SP_WORD_SIZE - c;
                    }
                    else {
                        /* Bits from middle of digit. */
                        y = (int)(n >> (SP_WORD_SIZE - winBits));
                        n <<= winBits;
                        c -= winBits;
                    }
                    y &= (int)mask;
                }

                /* 4.5. Montgomery multiply result by table entry. */
                if (err == MP_OKAY) {
                    err = sp_mul(tr, t[y], tr);
                }
                if (err == MP_OKAY) {
                    err = _sp_mont_red(tr, m, mp);
                }
            }

            /* Finished multiplying in table entries. */
            if ((err == MP_OKAY) && (c > 0)) {
                /* Handle remaining bits.
                 * Window values have top bit set and can't be used. */
                n = e->dp[0];
                /*  5. While bits left: */
                for (--c; (err == MP_OKAY) && (c >= 0); c--) {
                    /* 5.1. Montogmery square result */
                    err = sp_sqr(tr, tr);
                    if (err == MP_OKAY) {
                        err = _sp_mont_red(tr, m, mp);
                    }
                    /* 5.2. If exponent bit set */
                    if ((err == MP_OKAY) && ((n >> c) & 1)) {
                        /* 5.2.1. Montgomery multiply result by Montgomery form
                         * of base.
                         */
                        err = sp_mul(tr, bm, tr);
                        if (err == MP_OKAY) {
                            err = _sp_mont_red(tr, m, mp);
                        }
                    }
                }
            }
        }

        if (err == MP_OKAY) {
            /* 6. Convert result back from Montgomery form. */
            err = _sp_mont_red(tr, m, mp);
            /* Reduction implementation returns number to range: 0..m-1. */
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* Copy temporary result into parameter. */
        _sp_copy(tr, r);
    }

#ifndef WOLFSSL_SP_NO_MALLOC
    FREE_DYN_SP_INT_ARRAY(t, NULL);
#else
    FREE_SP_INT_ARRAY(t, NULL);
#endif
    return err;
}

#else
/* Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Non-constant time implementation.
 *
 * Algorithm:
 *   1. Convert base to Montgomery form
 *   2. Set result to base (assumes exponent is not zero)
 *   3. For each bit in exponent starting at second highest
 *     3.1. Montogmery square result
 *     3.2. If exponent bit set
 *       3.2.1. Montgomery multiply result by Montgomery form of base.
 *   4. Convert result back from Montgomery form.
 *
 * @param  [in]   b  SP integer that is the base.
 * @param  [in]   e  SP integer that is the exponent.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when b, e, m or r is NULL; or m <= 0 or e is negative.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_exptmod_nct(const sp_int* b, const sp_int* e, const sp_int* m,
    sp_int* r)
{
    int i;
    int err = MP_OKAY;
    int done = 0;
    int y = 0;
    int bits = sp_count_bits(e);
    sp_int_digit mp;
    DECL_SP_INT_ARRAY(t, m->used * 2 + 1, 2);

    /* Allocate memory for:
     *  - Montgomery form of base
     *  - Temporary result (in case r is same var as another parameter). */
    ALLOC_SP_INT_ARRAY(t, m->used * 2 + 1, 2, err, NULL);
    if (err == MP_OKAY) {
        _sp_init_size(t[0], m->used * 2 + 1);
        _sp_init_size(t[1], m->used * 2 + 1);

        /* Ensure base is less than modulus and copy into temp. */
        if (_sp_cmp_abs(b, m) != MP_LT) {
            err = sp_mod(b, m, t[0]);
            /* Handle base == modulus. */
            if ((err == MP_OKAY) && sp_iszero(t[0])) {
                _sp_set(r, 0);
                done = 1;
            }
        }
        else {
            /* Copy base into temp. */
            _sp_copy(b, t[0]);
        }
    }

    if ((!done) && (err == MP_OKAY)) {
        /* Calculate Montgomery multiplier for reduction. */
        _sp_mont_setup(m, &mp);
        /* Calculate Montgomery normalizer for modulus. */
        err = sp_mont_norm(t[1], m);
        if (err == MP_OKAY) {
            /* 1. Convert base to Montgomery form. */
            err = sp_mul(t[0], t[1], t[0]);
        }
        if (err == MP_OKAY) {
            /* t[0] = t[0] mod m, temporary size has to be bigger than t[0]. */
            err = _sp_div(t[0], m, NULL, t[0], t[0]->used + 1);
        }
        if (err == MP_OKAY) {
            /* 2. Result starts as Montgomery form of base (assuming e > 0). */
            _sp_copy(t[0], t[1]);
        }

        /* 3. For each bit in exponent starting at second highest. */
        for (i = bits - 2; (err == MP_OKAY) && (i >= 0); i--) {
            /* 3.1. Montgomery square result. */
            err = sp_sqr(t[0], t[0]);
            if (err == MP_OKAY) {
                err = _sp_mont_red(t[0], m, mp);
            }
            if (err == MP_OKAY) {
                /* Get bit and index i. */
                y = (e->dp[i >> SP_WORD_SHIFT] >> (i & SP_WORD_MASK)) & 1;
                /* 3.2. If exponent bit set */
                if (y != 0) {
                    /* 3.2.1. Montgomery multiply result by Mont of base. */
                    err = sp_mul(t[0], t[1], t[0]);
                    if (err == MP_OKAY) {
                        err = _sp_mont_red(t[0], m, mp);
                    }
                }
            }
        }
        if (err == MP_OKAY) {
            /* 4. Convert from Montgomery form. */
            err = _sp_mont_red(t[0], m, mp);
            /* Reduction implementation returns number of range 0..m-1. */
        }
    }
    if ((!done) && (err == MP_OKAY)) {
        /* Copy temporary result into parameter. */
        _sp_copy(t[0], r);
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}
#endif /* WOLFSSL_SP_FAST_NCT_EXPTMOD || !WOLFSSL_SP_SMALL */

/* Exponentiates b to the power of e modulo m into r: r = b ^ e mod m
 * Non-constant time implementation.
 *
 * @param  [in]   b  SP integer that is the base.
 * @param  [in]   e  SP integer that is the exponent.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when b, e, m or r is NULL; or m <= 0 or e is negative.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_exptmod_nct(const sp_int* b, const sp_int* e, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((b == NULL) || (e == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(b, "a");
        sp_print(e, "b");
        sp_print(m, "m");
    }
#endif

    if (err != MP_OKAY) {
    }
    /* Handle special cases. */
    else if (sp_iszero(m)) {
        err = MP_VAL;
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    else if ((e->sign == MP_NEG) || (m->sign == MP_NEG)) {
        err = MP_VAL;
    }
#endif
    /* x mod 1 is always 0. */
    else if (sp_isone(m)) {
        _sp_set(r, 0);
    }
    /* b^0 mod m = 1 mod m = 1. */
    else if (sp_iszero(e)) {
        _sp_set(r, 1);
    }
    /* 0^x mod m = 0 mod m = 0. */
    else if (sp_iszero(b)) {
        _sp_set(r, 0);
    }
    /* Ensure SP integers have space for intermediate values. */
    else if (m->used * 2 >= r->size) {
        err = MP_VAL;
    }
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WOLFSSL_RSA_PUBLIC_ONLY)
    else if (mp_iseven(m)) {
        err = _sp_exptmod_ex(b, e, (int)(e->used * SP_WORD_SIZE), m, r);
    }
#endif
    else {
        err = _sp_exptmod_nct(b, e, m, r);
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rme");
    }
#endif

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH */

/***************
 * 2^e functions
 ***************/

#if defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
/* Divide by 2^e: r = a >> e and rem = bits shifted out
 *
 * @param  [in]   a    SP integer to divide.
 * @param  [in]   e    Exponent bits (dividing by 2^e).
 * @param  [in]   m    SP integer that is the modulus.
 * @param  [out]  r    SP integer to hold result.
 * @param  [out]  rem  SP integer to hold remainder.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a is NULL or e is negative.
 */
int sp_div_2d(const sp_int* a, int e, sp_int* r, sp_int* rem)
{
    int err = MP_OKAY;

    if ((a == NULL) || (e < 0)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Number of bits remaining after shift. */
        int remBits = sp_count_bits(a) - e;

        if (remBits <= 0) {
            /* Shifting down by more bits than in number. */
            _sp_zero(r);
            if (rem != NULL) {
                err = sp_copy(a, rem);
            }
        }
        else {
            if (rem != NULL) {
                /* Copy a in to remainder. */
                err = sp_copy(a, rem);
            }
            if (err == MP_OKAY) {
                /* Shift a down by into result. */
                err = sp_rshb(a, e, r);
            }
            if ((err == MP_OKAY) && (rem != NULL)) {
                /* Set used and mask off top digit of remainder. */
                rem->used = ((unsigned int)e + SP_WORD_SIZE - 1) >>
                    SP_WORD_SHIFT;
                e &= SP_WORD_MASK;
                if (e > 0) {
                    rem->dp[rem->used - 1] &= ((sp_int_digit)1 << e) - 1;
                }

                /* Remove leading zeros from remainder. */
                sp_clamp(rem);
            #ifdef WOLFSSL_SP_INT_NEGATIVE
                rem->sign = MP_ZPOS;
            #endif
            }
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY */

#if defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)
/* The bottom e bits: r = a & ((1 << e) - 1)
 *
 * @param  [in]   a  SP integer to reduce.
 * @param  [in]   e  Modulus bits (modulus equals 2^e).
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL, e is negative or e is too large for
 *          result.
 */
int sp_mod_2d(const sp_int* a, int e, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int digits = ((unsigned int)e + SP_WORD_SIZE - 1) >> SP_WORD_SHIFT;

    if ((a == NULL) || (r == NULL) || (e < 0)) {
        err = MP_VAL;
    }
    if ((err == MP_OKAY) && (digits > r->size)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Copy a into r if not same pointer. */
        if (a != r) {
            XMEMCPY(r->dp, a->dp, digits * SP_WORD_SIZEOF);
            r->used = a->used;
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            r->sign = a->sign;
        #endif
        }

        /* Modify result if a is bigger or same digit size. */
    #ifndef WOLFSSL_SP_INT_NEGATIVE
        if (digits <= a->used)
    #else
        /* Need to make negative positive and mask. */
        if ((a->sign == MP_NEG) || (digits <= a->used))
    #endif
        {
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            if (a->sign == MP_NEG) {
                unsigned int i;
                sp_int_digit carry = 0;

                /* Negate value. */
                for (i = 0; i < r->used; i++) {
                    sp_int_digit next = r->dp[i] > 0;
                    r->dp[i] = (sp_int_digit)0 - r->dp[i] - carry;
                    carry |= next;
                }
                for (; i < digits; i++) {
                    r->dp[i] = (sp_int_digit)0 - carry;
                }
                r->sign = MP_ZPOS;
            }
        #endif
            /* Set used and mask off top digit of result. */
            r->used = digits;
            e &= SP_WORD_MASK;
            if (e > 0) {
                r->dp[r->used - 1] &= ((sp_int_digit)1 << e) - 1;
            }
            sp_clamp(r);
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY */

#if (defined(WOLFSSL_SP_MATH_ALL) && (!defined(WOLFSSL_RSA_VERIFY_ONLY) || \
    !defined(NO_DH))) || defined(OPENSSL_ALL)
/* Multiply by 2^e: r = a << e
 *
 * @param  [in]   a  SP integer to multiply.
 * @param  [in]   e  Multiplier bits (multiplier equals 2^e).
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL, e is negative, or result is too big for
 *          result size.
 */
int sp_mul_2d(const sp_int* a, int e, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (r == NULL) || (e < 0)) {
        err = MP_VAL;
    }

    /* Ensure result has enough allocated digits for result. */
    if ((err == MP_OKAY) &&
            ((unsigned int)(sp_count_bits(a) + e) > r->size * SP_WORD_SIZE)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Copy a into r as left shift function works on the number. */
        if (a != r) {
            err = sp_copy(a, r);
        }
    }

    if (err == MP_OKAY) {
#if 0
        sp_print(a, "a");
        sp_print_int(e, "n");
#endif
        err = sp_lshb(r, e);
#if 0
        sp_print(r, "rsl");
#endif
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY */

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(HAVE_ECC) || (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY))

/* START SP_SQR implementations */
/* This code is generated.
 * To generate:
 *   cd scripts/sp/sp_int
 *   ./gen.sh
 * File sp_sqr.c contains code.
 */

#if !defined(WOLFSSL_SP_MATH) || !defined(WOLFSSL_SP_SMALL)
#ifdef SQR_MUL_ASM
/* Square a and store in r. r = a * a
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    int j;
    unsigned int k;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
    sp_int_digit t[((a->used + 1) / 2) * 2 + 1];
#else
    sp_int_digit t[(SP_INT_DIGITS + 1) / 2];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    t = (sp_int_digit*)XMALLOC(
        sizeof(sp_int_digit) * (((a->used + 1) / 2) * 2 + 1), NULL,
        DYNAMIC_TYPE_BIGINT);
    if (t == NULL) {
        err = MP_MEM;
    }
#endif
    if ((err == MP_OKAY) && (a->used <= 1)) {
        sp_int_digit l;
        sp_int_digit h;

        h = 0;
        l = 0;
        SP_ASM_SQR(h, l, a->dp[0]);
        r->dp[0] = h;
        r->dp[1] = l;
    }
    else if (err == MP_OKAY) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o;
        sp_int_digit* p = t;

        h = 0;
        l = 0;
        SP_ASM_SQR(h, l, a->dp[0]);
        t[0] = h;
        h = 0;
        o = 0;
        for (k = 1; k < (a->used + 1) / 2; k++) {
            i = k;
            j = (int)(k - 1);
            for (; (j >= 0); i++, j--) {
                SP_ASM_MUL_ADD2(l, h, o, a->dp[i], a->dp[j]);
            }
            t[k * 2 - 1] = l;
            l = h;
            h = o;
            o = 0;

            SP_ASM_SQR_ADD(l, h, o, a->dp[k]);
            i = k + 1;
            j = (int)(k - 1);
            for (; (j >= 0); i++, j--) {
                SP_ASM_MUL_ADD2(l, h, o, a->dp[i], a->dp[j]);
            }
            t[k * 2] = l;
            l = h;
            h = o;
            o = 0;
        }
        for (; k < a->used; k++) {
            i = k;
            j = (int)(k - 1);
            for (; (i < a->used); i++, j--) {
                SP_ASM_MUL_ADD2(l, h, o, a->dp[i], a->dp[j]);
            }
            p[k * 2 - 1] = l;
            l = h;
            h = o;
            o = 0;

            SP_ASM_SQR_ADD(l, h, o, a->dp[k]);
            i = k + 1;
            j = (int)(k - 1);
            for (; (i < a->used); i++, j--) {
                SP_ASM_MUL_ADD2(l, h, o, a->dp[i], a->dp[j]);
            }
            p[k * 2] = l;
            l = h;
            h = o;
            o = 0;

            p = r->dp;
        }
        r->dp[k * 2 - 1] = l;
        XMEMCPY(r->dp, t, (((a->used + 1) / 2) * 2 + 1) * sizeof(sp_int_digit));
    }

    if (err == MP_OKAY) {
        r->used = a->used * 2;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#else /* !SQR_MUL_ASM */
/* Square a and store in r. r = a * a
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    int j;
    unsigned int k;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
    sp_int_digit t[a->used * 2];
#else
    sp_int_digit t[SP_INT_DIGITS];
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * (a->used * 2), NULL,
        DYNAMIC_TYPE_BIGINT);
    if (t == NULL) {
        err = MP_MEM;
    }
#endif
    if (err == MP_OKAY) {
        sp_int_word w;
        sp_int_word l;
        sp_int_word h;
    #ifdef SP_WORD_OVERFLOW
        sp_int_word o;
    #endif

        w = (sp_int_word)a->dp[0] * a->dp[0];
        t[0] = (sp_int_digit)w;
        l = (sp_int_digit)(w >> SP_WORD_SIZE);
        h = 0;
    #ifdef SP_WORD_OVERFLOW
        o = 0;
    #endif
        for (k = 1; k <= (a->used - 1) * 2; k++) {
            i = k / 2;
            j = (int)(k - i);
            if (i == (unsigned int)j) {
                w = (sp_int_word)a->dp[i] * a->dp[j];
                l += (sp_int_digit)w;
                h += (sp_int_digit)(w >> SP_WORD_SIZE);
            #ifdef SP_WORD_OVERFLOW
                h += (sp_int_digit)(l >> SP_WORD_SIZE);
                l &= SP_MASK;
                o += (sp_int_digit)(h >> SP_WORD_SIZE);
                h &= SP_MASK;
            #endif
            }
            for (++i, --j; (i < a->used) && (j >= 0); i++, j--) {
                w = (sp_int_word)a->dp[i] * a->dp[j];
                l += (sp_int_digit)w;
                h += (sp_int_digit)(w >> SP_WORD_SIZE);
            #ifdef SP_WORD_OVERFLOW
                h += (sp_int_digit)(l >> SP_WORD_SIZE);
                l &= SP_MASK;
                o += (sp_int_digit)(h >> SP_WORD_SIZE);
                h &= SP_MASK;
            #endif
                l += (sp_int_digit)w;
                h += (sp_int_digit)(w >> SP_WORD_SIZE);
            #ifdef SP_WORD_OVERFLOW
                h += (sp_int_digit)(l >> SP_WORD_SIZE);
                l &= SP_MASK;
                o += (sp_int_digit)(h >> SP_WORD_SIZE);
                h &= SP_MASK;
            #endif
            }
            t[k] = (sp_int_digit)l;
            l >>= SP_WORD_SIZE;
            l += (sp_int_digit)h;
            h >>= SP_WORD_SIZE;
        #ifdef SP_WORD_OVERFLOW
            h += o & SP_MASK;
            o >>= SP_WORD_SIZE;
        #endif
        }
        t[k] = (sp_int_digit)l;
        r->used = k + 1;
        XMEMCPY(r->dp, t, r->used * sizeof(sp_int_digit));
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#endif /* SQR_MUL_ASM */
#endif /* !WOLFSSL_SP_MATH || !WOLFSSL_SP_SMALL */

#ifndef WOLFSSL_SP_SMALL
#if !defined(WOLFSSL_HAVE_SP_ECC) && defined(HAVE_ECC)
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 256)
#ifndef SQR_MUL_ASM
/* Square a and store in r. r = a * a
 *
 * Long-hand implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_4(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_word* w = NULL;
#else
    sp_int_word w[10];
#endif
    const sp_int_digit* da = a->dp;

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    w = (sp_int_word*)XMALLOC(sizeof(sp_int_word) * 10, NULL,
        DYNAMIC_TYPE_BIGINT);
    if (w == NULL) {
        err = MP_MEM;
    }
#endif


    if (err == MP_OKAY) {
        w[0] = (sp_int_word)da[0] * da[0];
        w[1] = (sp_int_word)da[0] * da[1];
        w[2] = (sp_int_word)da[0] * da[2];
        w[3] = (sp_int_word)da[1] * da[1];
        w[4] = (sp_int_word)da[0] * da[3];
        w[5] = (sp_int_word)da[1] * da[2];
        w[6] = (sp_int_word)da[1] * da[3];
        w[7] = (sp_int_word)da[2] * da[2];
        w[8] = (sp_int_word)da[2] * da[3];
        w[9] = (sp_int_word)da[3] * da[3];

        r->dp[0] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[1];
        w[0] += (sp_int_digit)w[1];
        r->dp[1] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[1] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[1];
        w[0] += (sp_int_digit)w[1];
        w[0] += (sp_int_digit)w[2];
        w[0] += (sp_int_digit)w[2];
        w[0] += (sp_int_digit)w[3];
        r->dp[2] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[2] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[2];
        w[0] += (sp_int_digit)w[2];
        w[3] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[3];
        w[0] += (sp_int_digit)w[4];
        w[0] += (sp_int_digit)w[4];
        w[0] += (sp_int_digit)w[5];
        w[0] += (sp_int_digit)w[5];
        r->dp[3] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[4] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[4];
        w[0] += (sp_int_digit)w[4];
        w[5] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[5];
        w[0] += (sp_int_digit)w[5];
        w[0] += (sp_int_digit)w[6];
        w[0] += (sp_int_digit)w[6];
        w[0] += (sp_int_digit)w[7];
        r->dp[4] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[6] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[6];
        w[0] += (sp_int_digit)w[6];
        w[7] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[7];
        w[0] += (sp_int_digit)w[8];
        w[0] += (sp_int_digit)w[8];
        r->dp[5] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[8] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[8];
        w[0] += (sp_int_digit)w[8];
        w[0] += (sp_int_digit)w[9];
        r->dp[6] = (sp_int_digit)w[0];
        w[0] >>= SP_WORD_SIZE;
        w[9] >>= SP_WORD_SIZE;
        w[0] += (sp_int_digit)w[9];
        r->dp[7] = (sp_int_digit)w[0];

        r->used = 8;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (w != NULL) {
        XFREE(w, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
#else /* SQR_MUL_ASM */
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_4(const sp_int* a, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit t[4];

    SP_ASM_SQR(h, l, a->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
    r->dp[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[2], a->dp[3]);
    r->dp[5] = l;
    l = h;
    h = o;
    SP_ASM_SQR_ADD_NO(l, h, a->dp[3]);
    r->dp[6] = l;
    r->dp[7] = h;
    XMEMCPY(r->dp, t, 4 * sizeof(sp_int_digit));
    r->used = 8;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_6(const sp_int* a, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit tl = 0;
    sp_int_digit th = 0;
    sp_int_digit to;
    sp_int_digit t[6];

#if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
    to = 0;
#endif

    SP_ASM_SQR(h, l, a->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[4]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[4]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[3]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[5]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[2], a->dp[4]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[3]);
    r->dp[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[2], a->dp[5]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[3], a->dp[4]);
    r->dp[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[3], a->dp[5]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[4]);
    r->dp[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[4], a->dp[5]);
    r->dp[9] = l;
    l = h;
    h = o;
    SP_ASM_SQR_ADD_NO(l, h, a->dp[5]);
    r->dp[10] = l;
    r->dp[11] = h;
    XMEMCPY(r->dp, t, 6 * sizeof(sp_int_digit));
    r->used = 12;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 256)
#ifdef SQR_MUL_ASM
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_8(const sp_int* a, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit tl = 0;
    sp_int_digit th = 0;
    sp_int_digit to;
    sp_int_digit t[8];

#if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
    to = 0;
#endif

    SP_ASM_SQR(h, l, a->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[4]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[4]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[3]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[4]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[3]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[4]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[1], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[5]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[4]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[2], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[5]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[9] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[3], a->dp[7]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[4], a->dp[6]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[5]);
    r->dp[10] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[4], a->dp[7]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[5], a->dp[6]);
    r->dp[11] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[5], a->dp[7]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[6]);
    r->dp[12] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[6], a->dp[7]);
    r->dp[13] = l;
    l = h;
    h = o;
    SP_ASM_SQR_ADD_NO(l, h, a->dp[7]);
    r->dp[14] = l;
    r->dp[15] = h;
    XMEMCPY(r->dp, t, 8 * sizeof(sp_int_digit));
    r->used = 16;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_12(const sp_int* a, sp_int* r)
{
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit tl = 0;
    sp_int_digit th = 0;
    sp_int_digit to;
    sp_int_digit t[12];

#if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
    to = 0;
#endif

    SP_ASM_SQR(h, l, a->dp[0]);
    t[0] = h;
    h = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
    t[1] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
    t[2] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
    t[3] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[4]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
    t[4] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[4]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[3]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[5] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[4]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[3]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[6] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[5]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[4]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[7] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[5]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[4]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[8] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[6]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[5]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[9] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[6]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[5]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[10] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[7]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[6]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    t[11] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[1], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[7]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[6]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[12] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[2], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[8]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[7]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[13] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[3], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[8]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[7]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[14] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[4], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[9]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[8]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[15] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[5], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[9]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[8]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[16] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_SET(tl, th, to, a->dp[6], a->dp[11]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[10]);
    SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[9]);
    SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
    r->dp[17] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[7], a->dp[11]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[8], a->dp[10]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[9]);
    r->dp[18] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[8], a->dp[11]);
    SP_ASM_MUL_ADD2(l, h, o, a->dp[9], a->dp[10]);
    r->dp[19] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[9], a->dp[11]);
    SP_ASM_SQR_ADD(l, h, o, a->dp[10]);
    r->dp[20] = l;
    l = h;
    h = o;
    o = 0;
    SP_ASM_MUL_ADD2(l, h, o, a->dp[10], a->dp[11]);
    r->dp[21] = l;
    l = h;
    h = o;
    SP_ASM_SQR_ADD_NO(l, h, a->dp[11]);
    r->dp[22] = l;
    r->dp[23] = h;
    XMEMCPY(r->dp, t, 12 * sizeof(sp_int_digit));
    r->used = 24;
    sp_clamp(r);

    return MP_OKAY;
}
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#endif /* !WOLFSSL_HAVE_SP_ECC && HAVE_ECC */

#if defined(SQR_MUL_ASM) && (defined(WOLFSSL_SP_INT_LARGE_COMBA) || \
    (!defined(WOLFSSL_SP_MATH) && defined(WOLFCRYPT_HAVE_SAKKE) && \
    (SP_WORD_SIZE == 64)))
    #if SP_INT_DIGITS >= 32
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_16(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit tl = 0;
    sp_int_digit th = 0;
    sp_int_digit to;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#else
    sp_int_digit t[16];
#endif

#if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
    to = 0;
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
     t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * 16, NULL,
         DYNAMIC_TYPE_BIGINT);
     if (t == NULL) {
         err = MP_MEM;
     }
#endif
    if (err == MP_OKAY) {
        SP_ASM_SQR(h, l, a->dp[0]);
        t[0] = h;
        h = 0;
        SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
        t[1] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
        t[2] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
        t[3] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[4]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
        t[4] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[4]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[3]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[5] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[4]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[3]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[6] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[4]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[7] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[5]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[4]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[8] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[5]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[9] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[6]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[5]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[10] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[6]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[11] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[7]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[6]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[12] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[7]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[13] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[8]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[7]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[14] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[8]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[15] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[1], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[9]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[8]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[16] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[2], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[9]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[17] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[3], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[10]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[9]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[18] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[4], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[10]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[19] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[5], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[11]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[10]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[20] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[6], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[11]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[21] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[7], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[12]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[11]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[22] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[8], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[12]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[23] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[9], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[13]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[12]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[24] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[10], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[13]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[25] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[11], a->dp[15]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[12], a->dp[14]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[13]);
        r->dp[26] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[12], a->dp[15]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[13], a->dp[14]);
        r->dp[27] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[13], a->dp[15]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[14]);
        r->dp[28] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[14], a->dp[15]);
        r->dp[29] = l;
        l = h;
        h = o;
        SP_ASM_SQR_ADD_NO(l, h, a->dp[15]);
        r->dp[30] = l;
        r->dp[31] = h;
        XMEMCPY(r->dp, t, 16 * sizeof(sp_int_digit));
        r->used = 32;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
    #endif /* SP_INT_DIGITS >= 32 */
#endif /* SQR_MUL_ASM && (WOLFSSL_SP_INT_LARGE_COMBA || !WOLFSSL_SP_MATH &&
        * WOLFCRYPT_HAVE_SAKKE && SP_WORD_SIZE == 64 */

#if defined(SQR_MUL_ASM) && defined(WOLFSSL_SP_INT_LARGE_COMBA)
    #if SP_INT_DIGITS >= 48
/* Square a and store in r. r = a * a
 *
 * Comba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_24(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    sp_int_digit l = 0;
    sp_int_digit h = 0;
    sp_int_digit o = 0;
    sp_int_digit tl = 0;
    sp_int_digit th = 0;
    sp_int_digit to;
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    sp_int_digit* t = NULL;
#else
    sp_int_digit t[24];
#endif

#if defined(WOLFSSL_SP_ARM_THUMB) && SP_WORD_SIZE == 32
    to = 0;
#endif

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
     t = (sp_int_digit*)XMALLOC(sizeof(sp_int_digit) * 24, NULL,
         DYNAMIC_TYPE_BIGINT);
     if (t == NULL) {
         err = MP_MEM;
     }
#endif
    if (err == MP_OKAY) {
        SP_ASM_SQR(h, l, a->dp[0]);
        t[0] = h;
        h = 0;
        SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[1]);
        t[1] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2_NO(l, h, o, a->dp[0], a->dp[2]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[1]);
        t[2] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[3]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[2]);
        t[3] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[0], a->dp[4]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[1], a->dp[3]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[2]);
        t[4] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[4]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[3]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[5] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[4]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[3]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[6] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[5]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[4]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[7] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[5]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[4]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[8] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[6]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[5]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[9] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[6]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[5]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[10] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[7]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[6]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[11] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[7]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[6]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[12] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[8]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[7]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[13] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[8]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[7]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[14] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[9]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[8]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[15] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[9]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[8]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[16] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[10]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[9]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[17] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[10]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[9]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[18] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[11]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[10]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[19] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[11]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[10]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[20] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[12]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[11]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[21] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[12]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[11]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[22] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[0], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[1], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[13]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[12]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        t[23] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[1], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[2], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[13]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[12]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[24] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[2], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[3], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[14]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[13]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[25] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[3], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[4], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[14]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[13]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[26] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[4], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[5], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[15]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[14]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[27] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[5], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[6], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[15]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[14]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[28] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[6], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[7], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[16]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[15]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[29] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[7], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[8], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[16]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[15]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[30] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[8], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[9], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[17]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[16]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[31] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[9], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[10], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[17]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[16]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[32] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[10], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[11], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[18]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[17]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[33] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[11], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[12], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[18]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[17]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[34] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[12], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[13], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[19]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[17], a->dp[18]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[35] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[13], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[14], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[17], a->dp[19]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[18]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[36] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[14], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[15], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[17], a->dp[20]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[18], a->dp[19]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[37] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[15], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[16], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[17], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[18], a->dp[20]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[19]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[38] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[16], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[17], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[18], a->dp[21]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[19], a->dp[20]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[39] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[17], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[18], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[19], a->dp[21]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[20]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[40] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_SET(tl, th, to, a->dp[18], a->dp[23]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[19], a->dp[22]);
        SP_ASM_MUL_ADD(tl, th, to, a->dp[20], a->dp[21]);
        SP_ASM_ADD_DBL_3(l, h, o, tl, th, to);
        r->dp[41] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[19], a->dp[23]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[20], a->dp[22]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[21]);
        r->dp[42] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[20], a->dp[23]);
        SP_ASM_MUL_ADD2(l, h, o, a->dp[21], a->dp[22]);
        r->dp[43] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[21], a->dp[23]);
        SP_ASM_SQR_ADD(l, h, o, a->dp[22]);
        r->dp[44] = l;
        l = h;
        h = o;
        o = 0;
        SP_ASM_MUL_ADD2(l, h, o, a->dp[22], a->dp[23]);
        r->dp[45] = l;
        l = h;
        h = o;
        SP_ASM_SQR_ADD_NO(l, h, a->dp[23]);
        r->dp[46] = l;
        r->dp[47] = h;
        XMEMCPY(r->dp, t, 24 * sizeof(sp_int_digit));
        r->used = 48;
        sp_clamp(r);
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_SP_NO_MALLOC)
    if (t != NULL) {
        XFREE(t, NULL, DYNAMIC_TYPE_BIGINT);
    }
#endif
    return err;
}
    #endif /* SP_INT_DIGITS >= 48 */

    #if SP_INT_DIGITS >= 64
/* Square a and store in r. r = a * a
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_32(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    DECL_SP_INT(a1, 16);
    DECL_SP_INT_ARRAY(z, 33, 2);

    ALLOC_SP_INT(a1, 16, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 33, 2, err, NULL);
    if (err == MP_OKAY) {
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[16], sizeof(sp_int_digit) * 16);
        a1->used = 16;

        /* z2 = a1 ^ 2 */
        err = _sp_sqr_16(a1, z2);
    }
    if (err == MP_OKAY) {
        l = 0;
        h = 0;
        for (i = 0; i < 16; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;

        /* z0 = a0 ^ 2 */
        err = _sp_sqr_16(a, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) ^ 2 */
        err = _sp_sqr_16(a1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 32) + (z1 - z0 - z2) << 16) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 16 */
        z1->dp[32] = ca;
        l = 0;
        if (ca) {
            l = z1->dp[0 + 16];
            h = 0;
            SP_ASM_ADDC(l, h, a1->dp[0]);
            SP_ASM_ADDC(l, h, a1->dp[0]);
            z1->dp[0 + 16] = l;
            l = h;
            h = 0;
            for (i = 1; i < 16; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 16]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 16] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[32] += l;
        /* z1 = z1 - z0 - z1 */
        l = z1->dp[0];
        h = 0;
        SP_ASM_SUBB(l, h, z0->dp[0]);
        SP_ASM_SUBB(l, h, z2->dp[0]);
        z1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 32; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 16; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 16]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 16] = l;
            l = h;
            h = 0;
        }
        for (; i < 33; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 16] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 32  */
        l = 0;
        h = 0;
        for (i = 0; i < 17; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 32]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        for (; i < 32; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        r->used = 64;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT(a1, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 64 */

    #if SP_INT_DIGITS >= 96
/* Square a and store in r. r = a * a
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_48(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    DECL_SP_INT(a1, 24);
    DECL_SP_INT_ARRAY(z, 49, 2);

    ALLOC_SP_INT(a1, 24, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 49, 2, err, NULL);
    if (err == MP_OKAY) {
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[24], sizeof(sp_int_digit) * 24);
        a1->used = 24;

        /* z2 = a1 ^ 2 */
        err = _sp_sqr_24(a1, z2);
    }
    if (err == MP_OKAY) {
        l = 0;
        h = 0;
        for (i = 0; i < 24; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;

        /* z0 = a0 ^ 2 */
        err = _sp_sqr_24(a, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) ^ 2 */
        err = _sp_sqr_24(a1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 48) + (z1 - z0 - z2) << 24) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 24 */
        z1->dp[48] = ca;
        l = 0;
        if (ca) {
            l = z1->dp[0 + 24];
            h = 0;
            SP_ASM_ADDC(l, h, a1->dp[0]);
            SP_ASM_ADDC(l, h, a1->dp[0]);
            z1->dp[0 + 24] = l;
            l = h;
            h = 0;
            for (i = 1; i < 24; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 24]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 24] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[48] += l;
        /* z1 = z1 - z0 - z1 */
        l = z1->dp[0];
        h = 0;
        SP_ASM_SUBB(l, h, z0->dp[0]);
        SP_ASM_SUBB(l, h, z2->dp[0]);
        z1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 48; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 24; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 24]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 24] = l;
            l = h;
            h = 0;
        }
        for (; i < 49; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 24] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 48  */
        l = 0;
        h = 0;
        for (i = 0; i < 25; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 48]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        for (; i < 48; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        r->used = 96;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT(a1, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 96 */

    #if SP_INT_DIGITS >= 128
/* Square a and store in r. r = a * a
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_64(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    DECL_SP_INT(a1, 32);
    DECL_SP_INT_ARRAY(z, 65, 2);

    ALLOC_SP_INT(a1, 32, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 65, 2, err, NULL);
    if (err == MP_OKAY) {
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[32], sizeof(sp_int_digit) * 32);
        a1->used = 32;

        /* z2 = a1 ^ 2 */
        err = _sp_sqr_32(a1, z2);
    }
    if (err == MP_OKAY) {
        l = 0;
        h = 0;
        for (i = 0; i < 32; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;

        /* z0 = a0 ^ 2 */
        err = _sp_sqr_32(a, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) ^ 2 */
        err = _sp_sqr_32(a1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 64) + (z1 - z0 - z2) << 32) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 32 */
        z1->dp[64] = ca;
        l = 0;
        if (ca) {
            l = z1->dp[0 + 32];
            h = 0;
            SP_ASM_ADDC(l, h, a1->dp[0]);
            SP_ASM_ADDC(l, h, a1->dp[0]);
            z1->dp[0 + 32] = l;
            l = h;
            h = 0;
            for (i = 1; i < 32; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 32]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 32] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[64] += l;
        /* z1 = z1 - z0 - z1 */
        l = z1->dp[0];
        h = 0;
        SP_ASM_SUBB(l, h, z0->dp[0]);
        SP_ASM_SUBB(l, h, z2->dp[0]);
        z1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 64; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 32; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 32]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        for (; i < 65; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 32] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 64  */
        l = 0;
        h = 0;
        for (i = 0; i < 33; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 64]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 64] = l;
            l = h;
            h = 0;
        }
        for (; i < 64; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 64] = l;
            l = h;
            h = 0;
        }
        r->used = 128;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT(a1, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 128 */

    #if SP_INT_DIGITS >= 192
/* Square a and store in r. r = a * a
 *
 * Karatsuba implementation.
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqr_96(const sp_int* a, sp_int* r)
{
    int err = MP_OKAY;
    unsigned int i;
    sp_int_digit l;
    sp_int_digit h;
    sp_int* z0;
    sp_int* z1;
    sp_int* z2;
    sp_int_digit ca;
    DECL_SP_INT(a1, 48);
    DECL_SP_INT_ARRAY(z, 97, 2);

    ALLOC_SP_INT(a1, 48, err, NULL);
    ALLOC_SP_INT_ARRAY(z, 97, 2, err, NULL);
    if (err == MP_OKAY) {
        z1 = z[0];
        z2 = z[1];
        z0 = r;

        XMEMCPY(a1->dp, &a->dp[48], sizeof(sp_int_digit) * 48);
        a1->used = 48;

        /* z2 = a1 ^ 2 */
        err = _sp_sqr_48(a1, z2);
    }
    if (err == MP_OKAY) {
        l = 0;
        h = 0;
        for (i = 0; i < 48; i++) {
            SP_ASM_ADDC(l, h, a1->dp[i]);
            SP_ASM_ADDC(l, h, a->dp[i]);
            a1->dp[i] = l;
            l = h;
            h = 0;
        }
        ca = l;

        /* z0 = a0 ^ 2 */
        err = _sp_sqr_48(a, z0);
    }
    if (err == MP_OKAY) {
        /* z1 = (a0 + a1) ^ 2 */
        err = _sp_sqr_48(a1, z1);
    }
    if (err == MP_OKAY) {
        /* r = (z2 << 96) + (z1 - z0 - z2) << 48) + z0 */
        /* r = z0 */
        /* r += (z1 - z0 - z2) << 48 */
        z1->dp[96] = ca;
        l = 0;
        if (ca) {
            l = z1->dp[0 + 48];
            h = 0;
            SP_ASM_ADDC(l, h, a1->dp[0]);
            SP_ASM_ADDC(l, h, a1->dp[0]);
            z1->dp[0 + 48] = l;
            l = h;
            h = 0;
            for (i = 1; i < 48; i++) {
                SP_ASM_ADDC(l, h, z1->dp[i + 48]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                SP_ASM_ADDC(l, h, a1->dp[i]);
                z1->dp[i + 48] = l;
                l = h;
                h = 0;
            }
        }
        z1->dp[96] += l;
        /* z1 = z1 - z0 - z1 */
        l = z1->dp[0];
        h = 0;
        SP_ASM_SUBB(l, h, z0->dp[0]);
        SP_ASM_SUBB(l, h, z2->dp[0]);
        z1->dp[0] = l;
        l = h;
        h = 0;
        for (i = 1; i < 96; i++) {
            l += z1->dp[i];
            SP_ASM_SUBB(l, h, z0->dp[i]);
            SP_ASM_SUBB(l, h, z2->dp[i]);
            z1->dp[i] = l;
            l = h;
            h = 0;
        }
        z1->dp[i] += l;
        /* r += z1 << 16 */
        l = 0;
        h = 0;
        for (i = 0; i < 48; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 48]);
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        for (; i < 97; i++) {
            SP_ASM_ADDC(l, h, z1->dp[i]);
            r->dp[i + 48] = l;
            l = h;
            h = 0;
        }
        /* r += z2 << 96  */
        l = 0;
        h = 0;
        for (i = 0; i < 49; i++) {
            SP_ASM_ADDC(l, h, r->dp[i + 96]);
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 96] = l;
            l = h;
            h = 0;
        }
        for (; i < 96; i++) {
            SP_ASM_ADDC(l, h, z2->dp[i]);
            r->dp[i + 96] = l;
            l = h;
            h = 0;
        }
        r->used = 192;
        sp_clamp(r);
    }

    FREE_SP_INT_ARRAY(z, NULL);
    FREE_SP_INT(a1, NULL);
    return err;
}
    #endif /* SP_INT_DIGITS >= 192 */

#endif /* SQR_MUL_ASM && WOLFSSL_SP_INT_LARGE_COMBA */
#endif /* !WOLFSSL_SP_SMALL */

/* Square a and store in r. r = a * a
 *
 * @param  [in]   a  SP integer to square.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or r is NULL, or the result will be too big for fixed
 *          data length.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_sqr(const sp_int* a, sp_int* r)
{
#if defined(WOLFSSL_SP_MATH) && defined(WOLFSSL_SP_SMALL)
    return sp_mul(a, a, r);
#else
    int err = MP_OKAY;

    if ((a == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Need extra digit during calculation. */
    if ((err == MP_OKAY) && (a->used * 2 > r->size)) {
        err = MP_VAL;
    }

#if 0
    if (err == MP_OKAY) {
        sp_print(a, "a");
    }
#endif

    if (err == MP_OKAY) {
        if (a->used == 0) {
            _sp_zero(r);
        }
    else
#ifndef WOLFSSL_SP_SMALL
#if !defined(WOLFSSL_HAVE_SP_ECC) && defined(HAVE_ECC)
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 256)
        if (a->used == 4) {
            err = _sp_sqr_4(a, r);
        }
        else
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 64 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
        if (a->used == 6) {
            err = _sp_sqr_6(a, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 64 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 256)
#ifdef SQR_MUL_ASM
        if (a->used == 8) {
            err = _sp_sqr_8(a, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#if (SP_WORD_SIZE == 32 && SP_INT_BITS >= 384)
#ifdef SQR_MUL_ASM
        if (a->used == 12) {
            err = _sp_sqr_12(a, r);
        }
        else
#endif /* SQR_MUL_ASM */
#endif /* SP_WORD_SIZE == 32 */
#endif /* !WOLFSSL_HAVE_SP_ECC && HAVE_ECC */
#if defined(SQR_MUL_ASM) && (defined(WOLFSSL_SP_INT_LARGE_COMBA) || \
    (!defined(WOLFSSL_SP_MATH) && defined(WOLFCRYPT_HAVE_SAKKE) && \
    (SP_WORD_SIZE == 64)))
    #if SP_INT_DIGITS >= 32
        if (a->used == 16) {
            err = _sp_sqr_16(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 32 */
#endif /* SQR_MUL_ASM && (WOLFSSL_SP_INT_LARGE_COMBA || !WOLFSSL_SP_MATH &&
        * WOLFCRYPT_HAVE_SAKKE && SP_WORD_SIZE == 64 */
#if defined(SQR_MUL_ASM) && defined(WOLFSSL_SP_INT_LARGE_COMBA)
    #if SP_INT_DIGITS >= 48
        if (a->used == 24) {
            err = _sp_sqr_24(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 48 */
    #if SP_INT_DIGITS >= 64
        if (a->used == 32) {
            err = _sp_sqr_32(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 64 */
    #if SP_INT_DIGITS >= 96
        if (a->used == 48) {
            err = _sp_sqr_48(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 96 */
    #if SP_INT_DIGITS >= 128
        if (a->used == 64) {
            err = _sp_sqr_64(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 128 */
    #if SP_INT_DIGITS >= 192
        if (a->used == 96) {
            err = _sp_sqr_96(a, r);
        }
        else
    #endif /* SP_INT_DIGITS >= 192 */
#endif /* SQR_MUL_ASM && WOLFSSL_SP_INT_LARGE_COMBA */
#endif /* !WOLFSSL_SP_SMALL */
        {
            err = _sp_sqr(a, r);
        }
    }

#ifdef WOLFSSL_SP_INT_NEGATIVE
    if (err == MP_OKAY) {
        r->sign = MP_ZPOS;
    }
#endif

#if 0
    if (err == MP_OKAY) {
        sp_print(r, "rsqr");
    }
#endif

    return err;
#endif /* WOLFSSL_SP_MATH && WOLFSSL_SP_SMALL */
}
/* END SP_SQR implementations */

#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH || HAVE_ECC ||
        * (!NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY) */

#if defined(WOLFSSL_SP_MATH_ALL) || \
    (!defined(NO_RSA) && !defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    !defined(WOLFSSL_RSA_PUBLIC_ONLY)) || !defined(NO_DH) || defined(HAVE_ECC)
/* Square a mod m and store in r: r = (a * a) mod m
 *
 * @param  [in]   a  SP integer to square.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_sqrmod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;
    /* Create temporary for multiplication result. */
    DECL_SP_INT(t, a->used * 2);

    ALLOC_SP_INT(t, a->used * 2, err, NULL);
    if (err == MP_OKAY) {
        err = sp_init_size(t, a->used * 2);
    }

    /* Square and reduce. */
    if (err == MP_OKAY) {
        err = sp_sqr(a, t);
    }
    if (err == MP_OKAY) {
        err = sp_mod(t, m, r);
    }

    /* Dispose of an allocated SP int. */
    FREE_SP_INT(t, NULL);
    return err;
}

/* Square a mod m and store in r: r = (a * a) mod m
 *
 * @param  [in]   a  SP integer to square.
 * @param  [in]   m  SP integer that is the modulus.
 * @param  [out]  r  SP integer result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, m or r is NULL; or m is 0; or a squared is too big
 *          for fixed data length.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_sqrmod(const sp_int* a, const sp_int* m, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Ensure r has space for intermediate result. */
    if ((err == MP_OKAY) && (r != m) && (a->used * 2 > r->size)) {
        err = MP_VAL;
    }
    /* Ensure a is not too big. */
    if ((err == MP_OKAY) && (r == m) && (a->used * 2 > SP_INT_DIGITS)) {
        err = MP_VAL;
    }

    /* Use r as intermediate result if not same as pointer m which is needed
     * after first intermediate result.
     */
    if ((err == MP_OKAY) && (r != m)) {
        /* Square and reduce. */
        err = sp_sqr(a, r);
        if (err == MP_OKAY) {
            err = sp_mod(r, m, r);
        }
    }
    else if (err == MP_OKAY) {
        /* Do operation with temporary. */
        err = _sp_sqrmod(a, m, r);
    }

    return err;
}
#endif /* !WOLFSSL_RSA_VERIFY_ONLY */

/**********************
 * Montgomery functions
 **********************/

#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_DH) || \
    defined(WOLFCRYPT_HAVE_ECCSI) || defined(WOLFCRYPT_HAVE_SAKKE) || \
    defined(OPENSSL_ALL)
/* Reduce a number in Montgomery form.
 *
 * Assumes a and m are not NULL and m is not 0.
 *
 * DigitMask(a,i) := mask out the 'i'th digit in place.
 *
 * Algorithm:
 *  1. mask = (1 << (NumBits(m) % WORD_SIZE)) - 1
 *  2. For i = 0..NumDigits(m)-1
 *   2.1. mu = (mp * DigitMask(a, i)) & WORD_MASK
 *   2.2. If i == NumDigits(m)-1 and mask != 0 then mu & = mask
 *   2.3. a += mu * DigitMask(m, 0)
 *   2.4. For j = 1 up to NumDigits(m)-2
 *    2.4.1 a += mu * DigitMask(m, j)
 *   2.5 a += mu * DigitMask(m, NumDigits(m)-1))
 * 3. a >>= NumBits(m)
 * 4. a = a % m
 *
 * @param  [in,out]  a   SP integer to Montgomery reduce.
 * @param  [in]      m   SP integer that is the modulus.
 * @param  [in]      mp  SP integer digit that is the bottom digit of inv(-m).
 *
 * @return  MP_OKAY on success.
 */
static int _sp_mont_red(sp_int* a, const sp_int* m, sp_int_digit mp)
{
#if !defined(SQR_MUL_ASM)
    unsigned int i;
    int bits;
    sp_int_word w;
    sp_int_digit mu;

#if 0
    sp_print(a, "a");
    sp_print(m, "m");
#endif

    /* Count bits in modulus. */
    bits = sp_count_bits(m);

    /* Adding numbers into m->used * 2 digits - zero out unused digits. */
    for (i = a->used; i < m->used * 2; i++) {
        a->dp[i] = 0;
    }

    /* Special case when modulus is 1 digit or less. */
    if (m->used <= 1) {
        /* mu = (mp * DigitMask(a, i)) & WORD_MASK */
        mu = mp * a->dp[0];
        /* a += mu * m */
        w = a->dp[0];
        w += (sp_int_word)mu * m->dp[0];
        a->dp[0] = (sp_int_digit)w;
        w >>= SP_WORD_SIZE;
        w += a->dp[1];
        a->dp[1] = (sp_int_digit)w;
        w >>= SP_WORD_SIZE;
        a->dp[2] = (sp_int_digit)w;
        a->used = 3;
        /* mp is SP_WORD_SIZE */
        bits = SP_WORD_SIZE;
    }
    else {
        /* 1. mask = (1 << (NumBits(m) % WORD_SIZE)) - 1
         *    Mask when last digit of modulus doesn't have highest bit set.
         */
        sp_int_digit mask = (sp_int_digit)
            (((sp_int_digit)1 << (bits & (SP_WORD_SIZE - 1))) - 1);
        /* Overflow. */
        sp_int_word o = 0;

        /* 2. For i = 0..NumDigits(m)-1 */
        for (i = 0; i < m->used; i++) {
            unsigned int j;

            /* 2.1. mu = (mp * DigitMask(a, i)) & WORD_MASK */
            mu = mp * a->dp[i];
            /* 2.2. If i == NumDigits(m)-1 and mask != 0 then mu & = mask */
            if ((i == m->used - 1) && (mask != 0)) {
                mu &= mask;
            }

            /* 2.3. a += mu * DigitMask(m, 0) */
            w = a->dp[i];
            w += (sp_int_word)mu * m->dp[0];
            a->dp[i] = (sp_int_digit)w;
            w >>= SP_WORD_SIZE;
            /* 2.4. For j = 1 up to NumDigits(m)-2 */
            for (j = 1; j < m->used - 1; j++) {
                /* 2.4.1 a += mu * DigitMask(m, j) */
                w += a->dp[i + j];
                w += (sp_int_word)mu * m->dp[j];
                a->dp[i + j] = (sp_int_digit)w;
                w >>= SP_WORD_SIZE;
            }
            /* Handle overflow. */
            w += o;
            w += a->dp[i + j];
            o = (sp_int_digit)(w >> SP_WORD_SIZE);
            /* 2.5 a += mu * DigitMask(m, NumDigits(m)-1)) */
            w = ((sp_int_word)mu * m->dp[j]) + (sp_int_digit)w;
            a->dp[i + j] = (sp_int_digit)w;
            w >>= SP_WORD_SIZE;
            o += w;
        }
        /* Handle overflow. */
        o += a->dp[m->used * 2 - 1];
        a->dp[m->used * 2 - 1] = (sp_int_digit)o;
        o >>= SP_WORD_SIZE;
        a->dp[m->used * 2] = (sp_int_digit)o;
        a->used = m->used * 2 + 1;
    }

    /* Remove leading zeros. */
    sp_clamp(a);
    /* 3. a >>= NumBits(m) */
    (void)sp_rshb(a, bits, a);

    /* 4. a = a mod m */
    if (_sp_cmp_abs(a, m) != MP_LT) {
        _sp_sub_off(a, m, a, 0);
    }

#if 0
    sp_print(a, "rr");
#endif

    return MP_OKAY;
#else /* !SQR_MUL_ASM */
    unsigned int i;
    unsigned int j;
    int bits;
    sp_int_digit mu;
    sp_int_digit o;
    sp_int_digit mask;

#if 0
    sp_print(a, "a");
    sp_print(m, "m");
#endif

    bits = sp_count_bits(m);
    mask = ((sp_int_digit)1 << (bits & (SP_WORD_SIZE - 1))) - 1;

    for (i = a->used; i < m->used * 2; i++) {
        a->dp[i] = 0;
    }

    if (m->used <= 1) {
        sp_int_digit l;
        sp_int_digit h;

        /* mu = (mp * DigitMask(a, i)) & WORD_MASK */
        mu = mp * a->dp[0];
        /* a += mu * m */
        l = a->dp[0];
        h = 0;
        SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[0]);
        a->dp[0] = l;
        l = h;
        h = 0;
        SP_ASM_ADDC(l, h, a->dp[1]);
        a->dp[1] = l;
        a->dp[2] = h;
        a->used = m->used * 2 + 1;
        /* mp is SP_WORD_SIZE */
        bits = SP_WORD_SIZE;
    }
#if !defined(WOLFSSL_SP_MATH) && defined(HAVE_ECC)
#if SP_WORD_SIZE == 64
#if SP_INT_DIGITS >= 8
    else if ((m->used == 4) && (mask == 0)) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o2;

        l = 0;
        h = 0;
        o = 0;
        o2 = 0;
        /* For i = 0..NumDigits(m)-1 */
        for (i = 0; i < 4; i++) {
            /* mu = (mp * DigitMask(a, i)) & WORD_MASK */
            mu = mp * a->dp[0];
            l = a->dp[0];
            /* a = (a + mu * m) >> WORD_SIZE */
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[0]);
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[1]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[1]);
            a->dp[0] = l;
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[2]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[2]);
            a->dp[1] = l;
            l = h;
            h = o2;
            o2 = 0;
            SP_ASM_ADDC_REG(l, h, o);
            SP_ASM_ADDC(l, h, a->dp[i + 3]);
            SP_ASM_MUL_ADD(l, h, o2, mu, m->dp[3]);
            a->dp[2] = l;
            o = h;
            l = h;
            h = 0;
        }
        /* Handle overflow. */
        h = o2;
        SP_ASM_ADDC(l, h, a->dp[7]);
        a->dp[3] = l;
        a->dp[4] = h;
        a->used = 5;

        /* Remove leading zeros. */
        sp_clamp(a);

        /* a = a mod m */
        if (_sp_cmp_abs(a, m) != MP_LT) {
            _sp_sub_off(a, m, a, 0);
        }

        return MP_OKAY;
    }
#endif /* SP_INT_DIGITS >= 8 */
#if SP_INT_DIGITS >= 12
    else if ((m->used == 6) && (mask == 0)) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o2;

        l = 0;
        h = 0;
        o = 0;
        o2 = 0;
        /* For i = 0..NumDigits(m)-1 */
        for (i = 0; i < 6; i++) {
            /* mu = (mp * DigitMask(a, i)) & WORD_MASK */
            mu = mp * a->dp[0];
            l = a->dp[0];
            /* a = (a + mu * m) >> WORD_SIZE */
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[0]);
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[1]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[1]);
            a->dp[0] = l;
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[2]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[2]);
            a->dp[1] = l;
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[3]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[3]);
            a->dp[2] = l;
            l = h;
            h = 0;
            SP_ASM_ADDC(l, h, a->dp[4]);
            SP_ASM_MUL_ADD_NO(l, h, mu, m->dp[4]);
            a->dp[3] = l;
            l = h;
            h = o2;
            o2 = 0;
            SP_ASM_ADDC_REG(l, h, o);
            SP_ASM_ADDC(l, h, a->dp[i + 5]);
            SP_ASM_MUL_ADD(l, h, o2, mu, m->dp[5]);
            a->dp[4] = l;
            o = h;
            l = h;
            h = 0;
        }
        /* Handle overflow. */
        h = o2;
        SP_ASM_ADDC(l, h, a->dp[11]);
        a->dp[5] = l;
        a->dp[6] = h;
        a->used = 7;

        /* Remove leading zeros. */
        sp_clamp(a);

        /* a = a mod m */
        if (_sp_cmp_abs(a, m) != MP_LT) {
            _sp_sub_off(a, m, a, 0);
        }

        return MP_OKAY;
    }
#endif /* SP_INT_DIGITS >= 12 */
#elif SP_WORD_SIZE == 32
    else if ((m->used <= 12) && (mask == 0)) {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o2;
        sp_int_digit* ad;
        const sp_int_digit* md;

        o = 0;
        o2 = 0;
        ad = a->dp;
        /* For i = 0..NumDigits(m)-1 */
        for (i = 0; i < m->used; i++) {
            md = m->dp;
            /*  mu = (mp * DigitMask(a, i)) & WORD_MASK */
            mu = mp * ad[0];

            /* a = (a + mu * m, 0) >> WORD_SIZE */
            l = ad[0];
            h = 0;
            SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
            l = h;
            for (j = 1; j + 1 < m->used - 1; j += 2) {
                h = 0;
                SP_ASM_ADDC(l, h, ad[j]);
                SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
                ad[j - 1] = l;
                l = 0;
                SP_ASM_ADDC(h, l, ad[j + 1]);
                SP_ASM_MUL_ADD_NO(h, l, mu, *(md++));
                ad[j] = h;
            }
            for (; j < m->used - 1; j++) {
                h = 0;
                SP_ASM_ADDC(l, h, ad[j]);
                SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
                ad[j - 1] = l;
                l = h;
            }
            h = o2;
            o2 = 0;
            SP_ASM_ADDC_REG(l, h, o);
            SP_ASM_ADDC(l, h, ad[i + j]);
            SP_ASM_MUL_ADD(l, h, o2, mu, *md);
            ad[j - 1] = l;
            o = h;
        }
        /* Handle overflow. */
        l = o;
        h = o2;
        SP_ASM_ADDC(l, h, a->dp[m->used * 2 - 1]);
        a->dp[m->used  - 1] = l;
        a->dp[m->used] = h;
        a->used = m->used + 1;

        /* Remove leading zeros. */
        sp_clamp(a);

        /* a = a mod m */
        if (_sp_cmp_abs(a, m) != MP_LT) {
            _sp_sub_off(a, m, a, 0);
        }

        return MP_OKAY;
    }
#endif /* SP_WORD_SIZE == 64 | 32 */
#endif /* !WOLFSSL_SP_MATH && HAVE_ECC */
    else {
        sp_int_digit l;
        sp_int_digit h;
        sp_int_digit o2;
        sp_int_digit* ad;
        const sp_int_digit* md;

        o = 0;
        o2 = 0;
        ad = a->dp;
        /* 2. For i = 0..NumDigits(m)-1 */
        for (i = 0; i < m->used; i++, ad++) {
            md = m->dp;
            /* 2.1. mu = (mp * DigitMask(a, i)) & WORD_MASK */
            mu = mp * ad[0];
            /* 2.2. If i == NumDigits(m)-1 and mask != 0 then mu & = mask */
            if ((i == m->used - 1) && (mask != 0)) {
                mu &= mask;
            }

            /* 2.3 a += mu * DigitMask(m, 0) */
            l = ad[0];
            h = 0;
            SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
            ad[0] = l;
            l = h;
            /* 2.4. If i == NumDigits(m)-1 and mask != 0 then mu & = mask */
            for (j = 1; j + 1 < m->used - 1; j += 2) {
                h = 0;
                /* 2.4.1. a += mu * DigitMask(m, j) */
                SP_ASM_ADDC(l, h, ad[j + 0]);
                SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
                ad[j + 0] = l;
                l = 0;
                /* 2.4.1. a += mu * DigitMask(m, j) */
                SP_ASM_ADDC(h, l, ad[j + 1]);
                SP_ASM_MUL_ADD_NO(h, l, mu, *(md++));
                ad[j + 1] = h;
            }
            for (; j < m->used - 1; j++) {
                h = 0;
                /* 2.4.1. a += mu * DigitMask(m, j) */
                SP_ASM_ADDC(l, h, ad[j]);
                SP_ASM_MUL_ADD_NO(l, h, mu, *(md++));
                ad[j] = l;
                l = h;
            }
            h = o2;
            o2 = 0;
            SP_ASM_ADDC_REG(l, h, o);
            /* 2.5 a += mu * DigitMask(m, NumDigits(m)-1) */
            SP_ASM_ADDC(l, h, ad[j]);
            SP_ASM_MUL_ADD(l, h, o2, mu, *md);
            ad[j] = l;
            o = h;
        }
        /* Handle overflow. */
        l = o;
        h = o2;
        SP_ASM_ADDC(l, h, a->dp[m->used * 2 - 1]);
        a->dp[m->used * 2 - 1] = l;
        a->dp[m->used * 2] = h;
        a->used = m->used * 2 + 1;
    }

    /* Remove leading zeros. */
    sp_clamp(a);
    (void)sp_rshb(a, bits, a);

    /* a = a mod m */
    if (_sp_cmp_abs(a, m) != MP_LT) {
        _sp_sub_off(a, m, a, 0);
    }

#if 0
    sp_print(a, "rr");
#endif

    return MP_OKAY;
#endif /* !SQR_MUL_ASM */
}

#if !defined(WOLFSSL_RSA_VERIFY_ONLY) || \
    (defined(WOLFSSL_SP_MATH_ALL) && defined(HAVE_ECC))
/* Reduce a number in Montgomery form.
 *
 * @param  [in,out]  a   SP integer to Montgomery reduce.
 * @param  [in]      m   SP integer that is the modulus.
 * @param  [in]      mp  SP integer digit that is the bottom digit of inv(-m).
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or m is NULL or m is zero.
 */
int sp_mont_red(sp_int* a, const sp_int* m, sp_int_digit mp)
{
    int err;

    /* Validate parameters. */
    if ((a == NULL) || (m == NULL) || sp_iszero(m)) {
        err = MP_VAL;
    }
    /* Ensure a has enough space for calculation. */
    else if (a->size < m->used * 2 + 1) {
        err = MP_VAL;
    }
    else {
        /* Perform Montogomery Reduction. */
        err = _sp_mont_red(a, m, mp);
    }

    return err;
}
#endif

/* Calculate the bottom digit of the inverse of negative m.
 * (rho * m) mod 2^n = -1, where n is the number of bits in a digit.
 *
 * Used when performing Montgomery Reduction.
 * m must be odd.
 * Jeffrey Hurchalla’s method.
 *   https://arxiv.org/pdf/2204.04342.pdf
 *
 * @param  [in]   m   SP integer that is the modulus.
 * @param  [out]  mp  SP integer digit that is the bottom digit of inv(-m).
 */
static void _sp_mont_setup(const sp_int* m, sp_int_digit* rho)
{
    sp_int_digit d = m->dp[0];
    sp_int_digit x = (3 * d) ^ 2;
    sp_int_digit y = 1 - d * x;

#if SP_WORD_SIZE >= 16
    x *= 1 + y; y *= y;
#endif
#if SP_WORD_SIZE >= 32
    x *= 1 + y; y *= y;
#endif
#if SP_WORD_SIZE >= 64
    x *= 1 + y; y *= y;
#endif
    x *= 1 + y;

    /* rho = -1/m mod d, subtract x (unsigned) from 0, assign negative */
    *rho = (sp_int_digit)((sp_int_sdigit)0 - (sp_int_sdigit)x);
}

/* Calculate the bottom digit of the inverse of negative m.
 * (rho * m) mod 2^n = -1, where n is the number of bits in a digit.
 *
 * Used when performing Montgomery Reduction.
 *
 * @param  [in]   m   SP integer that is the modulus.
 * @param  [out]  mp  SP integer digit that is the bottom digit of inv(-m).
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when m or rho is NULL.
 */
int sp_mont_setup(const sp_int* m, sp_int_digit* rho)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((m == NULL) || (rho == NULL)) {
        err = MP_VAL;
    }
    /* Calculation only works with odd modulus. */
    if ((err == MP_OKAY) && !sp_isodd(m)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Calculate negative of inverse mod 2^n. */
        _sp_mont_setup(m, rho);
    }

    return err;
}

/* Calculate the normalization value of m.
 *   norm = 2^k - m, where k is the number of bits in m
 *
 * @param  [out]  norm   SP integer that normalises numbers into Montgomery
 *                       form.
 * @param  [in]   m      SP integer that is the modulus.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when norm or m is NULL, or number of bits in m is maximual.
 */
int sp_mont_norm(sp_int* norm, const sp_int* m)
{
    int err = MP_OKAY;
    unsigned int bits = 0;

    /* Validate parameters. */
    if ((norm == NULL) || (m == NULL)) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* Find top bit and ensure norm has enough space. */
        bits = (unsigned int)sp_count_bits(m);
        if (bits >= norm->size * SP_WORD_SIZE) {
            err = MP_VAL;
        }
    }
    if (err == MP_OKAY) {
        /* Round up for case when m is less than a word - no advantage in using
         * a smaller mask and would take more operations.
         */
        if (bits < SP_WORD_SIZE) {
            bits = SP_WORD_SIZE;
        }
        /* Smallest number greater than m of form 2^n. */
        _sp_zero(norm);
        err = sp_set_bit(norm, (int)bits);
    }
    if (err == MP_OKAY) {
        /* norm = 2^n % m */
        err = sp_sub(norm, m, norm);
    }
    if ((err == MP_OKAY) && (bits == SP_WORD_SIZE)) {
        /* Sub made norm one word and now finish calculation. */
        norm->dp[0] %= m->dp[0];
    }
    if (err == MP_OKAY) {
        /* Remove leading zeros. */
        sp_clamp(norm);
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_HAVE_SP_DH ||
        * WOLFCRYPT_HAVE_ECCSI || WOLFCRYPT_HAVE_SAKKE */

/*********************************
 * To and from binary and strings.
 *********************************/

/* Calculate the number of 8-bit values required to represent the
 * multi-precision number.
 *
 * When a is NULL, return s 0.
 *
 * @param  [in]  a  SP integer.
 *
 * @return  The count of 8-bit values.
 * @return  0 when a is NULL.
 */
int sp_unsigned_bin_size(const sp_int* a)
{
    int cnt = 0;

    if (a != NULL) {
        cnt = (sp_count_bits(a) + 7) / 8;
    }

    return cnt;
}

/* Convert a number as an array of bytes in big-endian format to a
 * multi-precision number.
 *
 * @param  [out]  a     SP integer.
 * @param  [in]   in    Array of bytes.
 * @param  [in]   inSz  Number of data bytes in array.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when the number is too big to fit in an SP.
 */
int sp_read_unsigned_bin(sp_int* a, const byte* in, word32 inSz)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || ((in == NULL) && (inSz > 0))) {
        err = MP_VAL;
    }

    /* Check a has enough space for number. */
    if ((err == MP_OKAY) && (inSz > (word32)a->size * SP_WORD_SIZEOF)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Load full digits at a time from in. */
        int i;
        int j = 0;

        a->used = (inSz + SP_WORD_SIZEOF - 1) / SP_WORD_SIZEOF;

    #if defined(BIG_ENDIAN_ORDER) && !defined(WOLFSSL_SP_INT_DIGIT_ALIGN)
        /* Data endian matches respresentation of number.
         * Directly copy if we don't have alignment issues.
         */
        for (i = (int)(inSz-1); i > SP_WORD_SIZEOF-1; i -= SP_WORD_SIZEOF) {
            a->dp[j++] = *(sp_int_digit*)(in + i - (SP_WORD_SIZEOF - 1));
        }
    #else
        /* Construct digit from required number of bytes. */
        for (i = (int)(inSz-1); i >= SP_WORD_SIZEOF - 1; i -= SP_WORD_SIZEOF) {
            a->dp[j]  = ((sp_int_digit)in[i - 0] <<  0)
        #if SP_WORD_SIZE >= 16
                      | ((sp_int_digit)in[i - 1] <<  8)
        #endif
        #if SP_WORD_SIZE >= 32
                      | ((sp_int_digit)in[i - 2] << 16) |
                        ((sp_int_digit)in[i - 3] << 24)
        #endif
        #if SP_WORD_SIZE >= 64
                      | ((sp_int_digit)in[i - 4] << 32) |
                        ((sp_int_digit)in[i - 5] << 40) |
                        ((sp_int_digit)in[i - 6] << 48) |
                        ((sp_int_digit)in[i - 7] << 56)
        #endif
                                                       ;
            j++;
        }
    #endif

#if SP_WORD_SIZE >= 16
        /* Handle leftovers. */
        if (i >= 0) {
    #ifdef BIG_ENDIAN_ORDER
            int s;

            /* Place remaining bytes into last digit. */
            a->dp[a->used - 1] = 0;
            for (s = 0; i >= 0; i--,s += 8) {
                a->dp[j] |= ((sp_int_digit)in[i]) << s;
            }
    #else
            /* Cast digits to an array of bytes so we can insert directly. */
            byte *d = (byte*)a->dp;

            /* Zero out all bytes in last digit. */
            a->dp[a->used - 1] = 0;
            /* Place remaining bytes directly into digit. */
            switch (i) {
            #if SP_WORD_SIZE >= 64
                case 6: d[inSz - 1 - 6] = in[6]; FALL_THROUGH;
                case 5: d[inSz - 1 - 5] = in[5]; FALL_THROUGH;
                case 4: d[inSz - 1 - 4] = in[4]; FALL_THROUGH;
                case 3: d[inSz - 1 - 3] = in[3]; FALL_THROUGH;
            #endif
            #if SP_WORD_SIZE >= 32
                case 2: d[inSz - 1 - 2] = in[2]; FALL_THROUGH;
                case 1: d[inSz - 1 - 1] = in[1]; FALL_THROUGH;
            #endif
                case 0: d[inSz - 1 - 0] = in[0];
            }
    #endif /* LITTLE_ENDIAN_ORDER */
        }
#endif
        sp_clamp(a);
    }

    return err;
}

/* Convert the multi-precision number to an array of bytes in big-endian format.
 *
 * The array must be large enough for encoded number - use mp_unsigned_bin_size
 * to calculate the number of bytes required.
 *
 * @param  [in]   a    SP integer.
 * @param  [out]  out  Array to put encoding into.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or out is NULL.
 */
int sp_to_unsigned_bin(const sp_int* a, byte* out)
{
    /* Write assuming output buffer is big enough. */
    return sp_to_unsigned_bin_len(a, out, sp_unsigned_bin_size(a));
}

/* Convert the multi-precision number to an array of bytes in big-endian format.
 *
 * The array must be large enough for encoded number - use mp_unsigned_bin_size
 * to calculate the number of bytes required.
 * Front-pads the output array with zeros to make number the size of the array.
 *
 * @param  [in]   a      SP integer.
 * @param  [out]  out    Array to put encoding into.
 * @param  [in]   outSz  Size of the array in bytes.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or out is NULL.
 */
int sp_to_unsigned_bin_len(const sp_int* a, byte* out, int outSz)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (out == NULL) || (outSz < 0)) {
        err = MP_VAL;
    }

#if SP_WORD_SIZE > 8
    if (err == MP_OKAY) {
        /* Start at the end of the buffer - least significant byte. */
        int j = outSz - 1;

        if (!sp_iszero(a)) {
            unsigned int i;

            /* Put each digit in. */
            for (i = 0; (j >= 0) && (i < a->used); i++) {
                int b;
                sp_int_digit d = a->dp[i];
                /* Place each byte of a digit into the buffer. */
                for (b = 0; b < SP_WORD_SIZE; b += 8) {
                    out[j--] = (byte)d;
                    d >>= 8;
                    /* Stop if the output buffer is filled. */
                    if (j < 0) {
                        if ((i < a->used - 1) || (d > 0)) {
                            err = MP_VAL;
                        }
                        break;
                    }
                }
            }
        }
        /* Front pad buffer with 0s. */
        for (; j >= 0; j--) {
            out[j] = 0;
        }
    }
#else
    if ((err == MP_OKAY) && ((unsigned int)outSz < a->used)) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        unsigned int i;
        int j;

        XMEMSET(out, 0, (unsigned int)outSz - a->used);

        for (i = 0, j = outSz - 1; i < a->used; i++, j--) {
            out[j] = a->dp[i];
        }
    }
#endif

    return err;
}

#if defined(WOLFSSL_SP_MATH_ALL) && !defined(NO_RSA) && \
    !defined(WOLFSSL_RSA_VERIFY_ONLY)
/* Store the number in big-endian format in array at an offset.
 * The array must be large enough for encoded number - use mp_unsigned_bin_size
 * to calculate the number of bytes required.
 *
 * @param  [in]   o    Offset into array o start encoding.
 * @param  [in]   a    SP integer.
 * @param  [out]  out  Array to put encoding into.
 *
 * @return  Index of next byte after data.
 * @return  MP_VAL when a or out is NULL.
 */
int sp_to_unsigned_bin_at_pos(int o, const sp_int* a, unsigned char* out)
{
    /* Get length of data that will be written. */
    int len = sp_unsigned_bin_size(a);
    /* Write number to buffer at offset. */
    int ret = sp_to_unsigned_bin_len(a, out + o, len);

    if (ret == MP_OKAY) {
        /* Return offset of next byte after number. */
        ret = o + len;
    }

    return ret;
}
#endif /* WOLFSSL_SP_MATH_ALL && !NO_RSA && !WOLFSSL_RSA_VERIFY_ONLY */

#ifdef WOLFSSL_SP_READ_RADIX_16
/* Convert hexadecimal number as string in big-endian format to a
 * multi-precision number.
 *
 * Assumes negative sign and leading zeros have been stripped.
 *
 * @param  [out]  a   SP integer.
 * @param  [in]   in  NUL terminated string.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when radix not supported, value is negative, or a character
 *          is not valid.
 */
static int _sp_read_radix_16(sp_int* a, const char* in)
{
    int err = MP_OKAY;
    int i;
    unsigned int s = 0;
    unsigned int j = 0;
    sp_int_digit d;

    /* Make all nibbles in digit 0. */
    d = 0;
    /* Step through string a character at a time starting at end - least
     * significant byte. */
    for (i = (int)(XSTRLEN(in) - 1); i >= 0; i--) {
        /* Convert character from hex. */
        int ch = (int)HexCharToByte(in[i]);
        /* Check for invalid character. */
        if (ch < 0) {
            err = MP_VAL;
            break;
        }

        /* Check whether we have filled the digit. */
        if (s == SP_WORD_SIZE) {
            /* Store digit and move index to next in a. */
            a->dp[j++] = d;
            /* Fail if we are out of space in a. */
            if (j >= a->size) {
                err = MP_VAL;
                break;
            }
            /* Set shift back to 0 - lowest nibble. */
            s = 0;
            /* Make all nibbles in digit 0. */
            d = 0;
        }

        /* Put next nibble into digit. */
        d |= ((sp_int_digit)ch) << s;
        /* Update shift for next nibble. */
        s += 4;
    }

    if (err == MP_OKAY) {
        /* If space, store last digit. */
        if (j < a->size) {
            a->dp[j] = d;
        }
        /* Update used count. */
        a->used = j + 1;
        /* Remove leading zeros. */
        sp_clamp(a);
    }

    return err;
}
#endif /* WOLFSSL_SP_READ_RADIX_16 */

#ifdef WOLFSSL_SP_READ_RADIX_10
/* Convert decimal number as string in big-endian format to a multi-precision
 * number.
 *
 * Assumes negative sign and leading zeros have been stripped.
 *
 * @param  [out]  a   SP integer.
 * @param  [in]   in  NUL terminated string.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when radix not supported, value is negative, or a character
 *          is not valid.
 */
static int _sp_read_radix_10(sp_int* a, const char* in)
{
    int  err = MP_OKAY;
    int  i;
    char ch;

    /* Start with a being zero. */
    _sp_zero(a);

    /* Process all characters. */
    for (i = 0; in[i] != '\0'; i++) {
        /* Get character. */
        ch = in[i];
        /* Check character is valid. */
        if ((ch >= '0') && (ch <= '9')) {
            /* Assume '0'..'9' are continuous valus as characters. */
            ch -= '0';
        }
        else {
            /* Return error on invalid character. */
            err = MP_VAL;
            break;
        }

        /* Multiply a by 10. */
        err = _sp_mul_d(a, 10, a, 0);
        if (err != MP_OKAY) {
            break;
        }
        /* Add character value. */
        err = _sp_add_d(a, (sp_int_digit)ch, a);
        if (err != MP_OKAY) {
            break;
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_READ_RADIX_10 */

#if defined(WOLFSSL_SP_READ_RADIX_16) || defined(WOLFSSL_SP_READ_RADIX_10)
/* Convert a number as string in big-endian format to a big number.
 * Only supports base-16 (hexadecimal) and base-10 (decimal).
 *
 * Negative values supported when WOLFSSL_SP_INT_NEGATIVE is defined.
 *
 * @param  [out]  a      SP integer.
 * @param  [in]   in     NUL terminated string.
 * @param  [in]   radix  Number of values in a digit.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or in is NULL, radix not supported, value is negative,
 *          or a character is not valid.
 */
int sp_read_radix(sp_int* a, const char* in, int radix)
{
    int err = MP_OKAY;
#ifdef WOLFSSL_SP_INT_NEGATIVE
    unsigned int sign = MP_ZPOS;
#endif

    if ((a == NULL) || (in == NULL)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
    #ifndef WOLFSSL_SP_INT_NEGATIVE
        if (*in == '-') {
            err = MP_VAL;
        }
        else
    #endif
        {
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            if (*in == '-') {
                /* Make number negative if signed string. */
                sign = MP_NEG;
                in++;
            }
        #endif /* WOLFSSL_SP_INT_NEGATIVE */
            /* Skip leading zeros. */
            while (*in == '0') {
                in++;
            }

            if (radix == 16) {
                err = _sp_read_radix_16(a, in);
            }
        #ifdef WOLFSSL_SP_READ_RADIX_10
            else if (radix == 10) {
                err = _sp_read_radix_10(a, in);
            }
        #endif
            else {
                err = MP_VAL;
            }

        #ifdef WOLFSSL_SP_INT_NEGATIVE
            /* Ensure not negative when zero. */
            if (err == MP_OKAY) {
                if (sp_iszero(a)) {
                    a->sign = MP_ZPOS;
                }
                else {
                    a->sign = sign;
                }
            }
        #endif
        }
    }

    return err;
}
#endif /* WOLFSSL_SP_READ_RADIX_16 || WOLFSSL_SP_READ_RADIX_10 */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WC_MP_TO_RADIX)
/* Put the big-endian, hex string encoding of a into str.
 *
 * Assumes str is large enough for result.
 * Use sp_radix_size() to calculate required length.
 *
 * @param  [in]   a    SP integer to convert.
 * @param  [out]  str  String to hold hex string result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or str is NULL.
 */
int sp_tohex(const sp_int* a, char* str)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (str == NULL)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Quick out if number is zero. */
        if (sp_iszero(a) == MP_YES) {
        #ifndef WC_DISABLE_RADIX_ZERO_PAD
            /* Make string represent complete bytes. */
            *str++ = '0';
        #endif /* WC_DISABLE_RADIX_ZERO_PAD */
            *str++ = '0';
        }
        else {
            int i;
            int j;
            sp_int_digit d;

        #ifdef WOLFSSL_SP_INT_NEGATIVE
            if (a->sign == MP_NEG) {
                /* Add negative sign character. */
                *str = '-';
                str++;
            }
        #endif /* WOLFSSL_SP_INT_NEGATIVE */

            /* Start at last digit - most significant digit. */
            i = (int)(a->used - 1);
            d = a->dp[i];
        #ifndef WC_DISABLE_RADIX_ZERO_PAD
            /* Find highest non-zero byte in most-significant word. */
            for (j = SP_WORD_SIZE - 8; j >= 0 && i >= 0; j -= 8) {
                /* When a byte at this index is not 0 break out to start
                 * writing.
                 */
                if (((d >> j) & 0xff) != 0) {
                    break;
                }
                /* Skip this digit if it was 0. */
                if (j == 0) {
                    j = SP_WORD_SIZE - 8;
                    d = a->dp[--i];
                }
            }
            /* Start with high nibble of byte. */
            j += 4;
        #else
            /* Find highest non-zero nibble in most-significant word. */
            for (j = SP_WORD_SIZE - 4; j >= 0; j -= 4) {
                /* When a nibble at this index is not 0 break out to start
                 * writing.
                 */
                if (((d >> j) & 0xf) != 0) {
                    break;
                }
                /* Skip this digit if it was 0. */
                if (j == 0) {
                    j = SP_WORD_SIZE - 4;
                    d = a->dp[--i];
                }
            }
        #endif /* WC_DISABLE_RADIX_ZERO_PAD */
            /* Write out as much as required from most-significant digit. */
            for (; j >= 0; j -= 4) {
                *(str++) = ByteToHex((byte)(d >> j));
            }
            /* Write rest of digits. */
            for (--i; i >= 0; i--) {
                /* Get digit from memory. */
                d = a->dp[i];
                /* Write out all nibbles of digit. */
                for (j = SP_WORD_SIZE - 4; j >= 0; j -= 4) {
                    *(str++) = (char)ByteToHex((byte)(d >> j));
                }
            }
        }
        /* Terminate string. */
        *str = '\0';
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) || WC_MP_TO_RADIX */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
    defined(WC_MP_TO_RADIX)
/* Put the big-endian, decimal string encoding of a into str.
 *
 * Assumes str is large enough for result.
 * Use sp_radix_size() to calculate required length.
 *
 * @param  [in]   a    SP integer to convert.
 * @param  [out]  str  String to hold hex string result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or str is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_todecimal(const sp_int* a, char* str)
{
    int err = MP_OKAY;
    int i;
    int j;
    sp_int_digit d = 0;

    /* Validate parameters. */
    if ((a == NULL) || (str == NULL)) {
        err = MP_VAL;
    }
    /* Quick out if number is zero. */
    else if (sp_iszero(a) == MP_YES) {
        *str++ = '0';
        *str = '\0';
    }
    else if (a->used >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    else {
        /* Temporary that is divided by 10. */
        DECL_SP_INT(t, a->used + 1);

        ALLOC_SP_INT_SIZE(t, a->used + 1, err, NULL);
        if (err == MP_OKAY) {
            _sp_copy(a, t);
        }
        if (err == MP_OKAY) {
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            if (a->sign == MP_NEG) {
                /* Add negative sign character. */
                *str = '-';
                str++;
            }
        #endif /* WOLFSSL_SP_INT_NEGATIVE */

            /* Write out little endian. */
            i = 0;
            do {
                /* Divide by 10 and get remainder of division. */
                (void)sp_div_d(t, 10, t, &d);
                /* Write out remainder as a character. */
                str[i++] = (char)('0' + d);
            }
            /* Keep going while we there is a value to write. */
            while (!sp_iszero(t));
            /* Terminate string. */
            str[i] = '\0';

            if (err == MP_OKAY) {
                /* Reverse string to big endian. */
                for (j = 0; j <= (i - 1) / 2; j++) {
                    int c = (unsigned char)str[j];
                    str[j] = str[i - 1 - j];
                    str[i - 1 - j] = (char)c;
                }
            }
        }

        FREE_SP_INT(t, NULL);
    }

    return err;
}
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_KEY_GEN || HAVE_COMP_KEY */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WC_MP_TO_RADIX)
/* Put the string version, big-endian, of a in str using the given radix.
 *
 * @param  [in]   a      SP integer to convert.
 * @param  [out]  str    String to hold hex string result.
 * @param  [in]   radix  Base of character.
 *                       Valid values: MP_RADIX_HEX, MP_RADIX_DEC.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or str is NULL, or radix not supported.
 */
int sp_toradix(const sp_int* a, char* str, int radix)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (str == NULL)) {
        err = MP_VAL;
    }
    /* Handle base 16 if requested. */
    else if (radix == MP_RADIX_HEX) {
        err = sp_tohex(a, str);
    }
#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_KEY_GEN) || \
    defined(HAVE_COMP_KEY)
    /* Handle base 10 if requested. */
    else if (radix == MP_RADIX_DEC) {
        err = sp_todecimal(a, str);
    }
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_KEY_GEN || HAVE_COMP_KEY */
    else {
        /* Base not supported. */
        err = MP_VAL;
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) || WC_MP_TO_RADIX */

#if (defined(WOLFSSL_SP_MATH_ALL) && !defined(WOLFSSL_RSA_VERIFY_ONLY)) || \
    defined(WC_MP_TO_RADIX)
/* Calculate the length of the string version, big-endian, of a using the given
 * radix.
 *
 * @param  [in]   a      SP integer to convert.
 * @param  [in]   radix  Base of character.
 *                       Valid values: MP_RADIX_HEX, MP_RADIX_DEC.
 * @param  [out]  size   The number of characters in encoding.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or size is NULL, or radix not supported.
 */
int sp_radix_size(const sp_int* a, int radix, int* size)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (size == NULL)) {
        err = MP_VAL;
    }
    /* Handle base 16 if requested. */
    else if (radix == MP_RADIX_HEX) {
        if (a->used == 0) {
        #ifndef WC_DISABLE_RADIX_ZERO_PAD
            /* 00 and '\0' */
            *size = 2 + 1;
        #else
            /* Zero and '\0' */
            *size = 1 + 1;
        #endif /* WC_DISABLE_RADIX_ZERO_PAD */
        }
        else {
            /* Count of nibbles. */
            int cnt = (sp_count_bits(a) + 3) / 4;
        #ifndef WC_DISABLE_RADIX_ZERO_PAD
            /* Must have even number of nibbles to have complete bytes. */
            if (cnt & 1) {
                cnt++;
            }
        #endif /* WC_DISABLE_RADIX_ZERO_PAD */
        #ifdef WOLFSSL_SP_INT_NEGATIVE
            /* Add to count of characters for negative sign. */
            if (a->sign == MP_NEG) {
                cnt++;
            }
        #endif /* WOLFSSL_SP_INT_NEGATIVE */
            /* One more for \0 */
            *size = cnt + 1;
        }
    }
#if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_KEY_GEN) || \
    defined(HAVE_COMP_KEY)
    /* Handle base 10 if requested. */
    else if (radix == MP_RADIX_DEC) {
        int i;
        sp_int_digit d;

        /* quick out if its zero */
        if (sp_iszero(a) == MP_YES) {
            /* Zero and '\0' */
            *size = 1 + 1;
        }
        else {
            DECL_SP_INT(t, a->used);

            /* Temporary to be divided by 10. */
            ALLOC_SP_INT(t, a->used, err, NULL);
            if (err == MP_OKAY) {
                t->size = a->used;
                _sp_copy(a, t);
            }

            if (err == MP_OKAY) {
                /* Count number of times number can be divided by 10. */
                for (i = 0; !sp_iszero(t); i++) {
                    (void)sp_div_d(t, 10, t, &d);
                }
            #ifdef WOLFSSL_SP_INT_NEGATIVE
                /* Add to count of characters for negative sign. */
                if (a->sign == MP_NEG) {
                    i++;
                }
            #endif /* WOLFSSL_SP_INT_NEGATIVE */
                /* One more for \0 */
                *size = i + 1;
            }

            FREE_SP_INT(t, NULL);
        }
    }
#endif /* WOLFSSL_SP_MATH_ALL || WOLFSSL_KEY_GEN || HAVE_COMP_KEY */
    else {
        /* Base not supported. */
        err = MP_VAL;
    }

    return err;
}
#endif /* (WOLFSSL_SP_MATH_ALL && !WOLFSSL_RSA_VERIFY_ONLY) || WC_MP_TO_RADIX */

/***************************************
 * Prime number generation and checking.
 ***************************************/

#if defined(WOLFSSL_KEY_GEN) && (!defined(NO_RSA) || !defined(NO_DH) || \
    !defined(NO_DSA)) && !defined(WC_NO_RNG)
#ifndef WOLFSSL_SP_MILLER_RABIN_CNT
/* Always done 8 iterations of Miller-Rabin on check of primality when
 * generating.
 */
#define WOLFSSL_SP_MILLER_RABIN_CNT     8
#endif

/* Generate a random prime for RSA only.
 *
 * @param  [out]  r     SP integer to hold result.
 * @param  [in]   len   Number of bytes in prime. Use -ve to indicate the two
 *                      lowest bits must be set.
 * @param  [in]   rng   Random number generator.
 * @param  [in]   heap  Heap hint. Unused.
 *
 * @return  MP_OKAY on success
 * @return  MP_VAL when r or rng is NULL, length is not supported or random
 *          number generator fails.
 */
int sp_rand_prime(sp_int* r, int len, WC_RNG* rng, void* heap)
{
    static const byte USE_BBS = 3;
    int  err = MP_OKAY;
    byte low_bits = 1;
    int  isPrime = MP_NO;
#if defined(WOLFSSL_SP_MATH_ALL) || defined(BIG_ENDIAN_ORDER)
    int  bits = 0;
#endif /* WOLFSSL_SP_MATH_ALL */
    unsigned int digits = 0;

    (void)heap;

    /* Check NULL parameters and 0 is not prime so 0 bytes is invalid. */
    if ((r == NULL) || (rng == NULL) || (len == 0)) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Get type. */
        if (len < 0) {
            low_bits = USE_BBS;
            len = -len;
        }

        /* Get number of digits required to handle required number of bytes. */
        digits = ((unsigned int)len + SP_WORD_SIZEOF - 1) / SP_WORD_SIZEOF;
        /* Ensure result has space. */
        if (r->size < digits) {
            err = MP_VAL;
        }
    }

    if (err == MP_OKAY) {
    #ifndef WOLFSSL_SP_MATH_ALL
        /* For minimal maths, support only what's in SP and needed for DH. */
    #if defined(WOLFSSL_HAVE_SP_DH) && defined(WOLFSSL_KEY_GEN)
        if (len == 32) {
        }
        else
    #endif /* WOLFSSL_HAVE_SP_DH && WOLFSSL_KEY_GEN */
        /* Generate RSA primes that are half the modulus length. */
    #ifdef WOLFSSL_SP_4096
        if (len == 256) {
            /* Support 2048-bit operations compiled in. */
        }
        else
    #endif
    #ifndef WOLFSSL_SP_NO_3072
        if (len == 192) {
            /* Support 1536-bit operations compiled in. */
        }
        else
    #endif
    #ifndef WOLFSSL_SP_NO_2048
        if (len == 128) {
            /* Support 1024-bit operations compiled in. */
        }
        else
    #endif
        {
            /* Bit length not supported in SP. */
            err = MP_VAL;
        }
    #endif /* !WOLFSSL_SP_MATH_ALL */

    #ifdef WOLFSSL_SP_INT_NEGATIVE
        /* Generated number is always positive. */
        r->sign = MP_ZPOS;
    #endif /* WOLFSSL_SP_INT_NEGATIVE */
        /* Set number of digits that will be used. */
        r->used = digits;
    #if defined(WOLFSSL_SP_MATH_ALL) || defined(BIG_ENDIAN_ORDER)
        /* Calculate number of bits in last digit. */
        bits = (len * 8) & SP_WORD_MASK;
    #endif /* WOLFSSL_SP_MATH_ALL || BIG_ENDIAN_ORDER */
    }

    /* Assume the candidate is probably prime and then test until it is proven
     * composite.
     */
    while ((err == MP_OKAY) && (isPrime == MP_NO)) {
#ifdef SHOW_GEN
        printf(".");
        fflush(stdout);
#endif /* SHOW_GEN */
        /* Generate bytes into digit array. */
        err = wc_RNG_GenerateBlock(rng, (byte*)r->dp, (word32)len);
        if (err != 0) {
            err = MP_VAL;
            break;
        }

        /* Set top bits to ensure bit length required is generated.
         * Also set second top to help ensure product of two primes is
         * going to be twice the number of bits of each.
         */
#ifdef LITTLE_ENDIAN_ORDER
        ((byte*)r->dp)[len-1]             |= 0x80 | 0x40;
#else
        ((byte*)(r->dp + r->used - 1))[0] |= 0x80 | 0x40;
#endif /* LITTLE_ENDIAN_ORDER */

#ifdef BIG_ENDIAN_ORDER
        /* Bytes were put into wrong place when less than full digit. */
        if (bits != 0) {
            r->dp[r->used - 1] >>= SP_WORD_SIZE - bits;
        }
#endif /* BIG_ENDIAN_ORDER */
#ifdef WOLFSSL_SP_MATH_ALL
        /* Mask top digit when less than a digit requested. */
        if (bits > 0) {
            r->dp[r->used - 1] &= ((sp_int_digit)1 << bits) - 1;
        }
#endif /* WOLFSSL_SP_MATH_ALL */
        /* Set mandatory low bits
         *  - bottom bit to make odd.
         *  - For BBS, second lowest too to make Blum integer (3 mod 4).
         */
        r->dp[0] |= low_bits;

        /* Running Miller-Rabin up to 3 times gives us a 2^{-80} chance
         * of a 1024-bit candidate being a false positive, when it is our
         * prime candidate. (Note 4.49 of Handbook of Applied Cryptography.)
         */
        err = sp_prime_is_prime_ex(r, WOLFSSL_SP_MILLER_RABIN_CNT, &isPrime,
            rng);
    }

    return err;
}
#endif /* WOLFSSL_KEY_GEN && (!NO_DH || !NO_DSA) && !WC_NO_RNG */

#ifdef WOLFSSL_SP_PRIME_GEN
/* Miller-Rabin test of "a" to the base of "b" as described in
 * HAC pp. 139 Algorithm 4.24
 *
 * Sets result to 0 if definitely composite or 1 if probably prime.
 * Randomly the chance of error is no more than 1/4 and often
 * very much lower.
 *
 * a is assumed to be odd.
 *
 * @param  [in]   a       SP integer to check.
 * @param  [in]   b       SP integer that is a small prime.
 * @param  [out]  result  MP_YES when number is likey prime.
 *                        MP_NO otherwise.
 * @param  [in]   n1      SP integer temporary.
 * @param  [in]   r       SP integer temporary.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int sp_prime_miller_rabin(const sp_int* a, sp_int* b, int* result,
    sp_int* n1, sp_int* r)
{
    int err = MP_OKAY;
    int s = 0;
    sp_int* y = b;

    /* Assume not prime. */
    *result = MP_NO;

    /* Ensure small prime is 2 or more. */
    if (sp_cmp_d(b, 1) != MP_GT) {
        err = MP_VAL;
    }
    if (err == MP_OKAY) {
        /* n1 = a - 1 (a is assumed odd.) */
        (void)sp_copy(a, n1);
        n1->dp[0]--;

        /* Set 2**s * r = n1 */
        /* Count the number of least significant bits which are zero. */
        s = sp_cnt_lsb(n1);
        /* Divide n - 1 by 2**s into r. */
        (void)sp_rshb(n1, s, r);

        /* Compute y = b**r mod a */
        err = sp_exptmod(b, r, a, y);
    }
    if (err == MP_OKAY) {
        /* Assume probably prime until shown otherwise. */
        *result = MP_YES;

        /* If y != 1 and y != n1 do */
        if ((sp_cmp_d(y, 1) != MP_EQ) && (_sp_cmp(y, n1) != MP_EQ)) {
            int j = 1;
            /* While j <= s-1 and y != n1 */
            while ((j <= (s - 1)) && (_sp_cmp(y, n1) != MP_EQ)) {
                /* Square for bit shifted down. */
                err = sp_sqrmod(y, a, y);
                if (err != MP_OKAY) {
                    break;
                }

                /* If y == 1 then composite. */
                if (sp_cmp_d(y, 1) == MP_EQ) {
                    *result = MP_NO;
                    break;
                }
                ++j;
            }

            /* If y != n1 then composite. */
            if ((*result == MP_YES) && (_sp_cmp(y, n1) != MP_EQ)) {
                *result = MP_NO;
            }
        }
    }

    return err;
}

#if SP_WORD_SIZE == 8
/* Number of pre-computed primes. First n primes - fitting in a digit. */
#define SP_PRIME_SIZE      54

static const sp_int_digit sp_primes[SP_PRIME_SIZE] = {
    0x02, 0x03, 0x05, 0x07, 0x0B, 0x0D, 0x11, 0x13,
    0x17, 0x1D, 0x1F, 0x25, 0x29, 0x2B, 0x2F, 0x35,
    0x3B, 0x3D, 0x43, 0x47, 0x49, 0x4F, 0x53, 0x59,
    0x61, 0x65, 0x67, 0x6B, 0x6D, 0x71, 0x7F, 0x83,
    0x89, 0x8B, 0x95, 0x97, 0x9D, 0xA3, 0xA7, 0xAD,
    0xB3, 0xB5, 0xBF, 0xC1, 0xC5, 0xC7, 0xD3, 0xDF,
    0xE3, 0xE5, 0xE9, 0xEF, 0xF1, 0xFB
};
#else
/* Number of pre-computed primes. First n primes. */
#define SP_PRIME_SIZE      256

/* The first 256 primes. */
static const sp_uint16 sp_primes[SP_PRIME_SIZE] = {
    0x0002, 0x0003, 0x0005, 0x0007, 0x000B, 0x000D, 0x0011, 0x0013,
    0x0017, 0x001D, 0x001F, 0x0025, 0x0029, 0x002B, 0x002F, 0x0035,
    0x003B, 0x003D, 0x0043, 0x0047, 0x0049, 0x004F, 0x0053, 0x0059,
    0x0061, 0x0065, 0x0067, 0x006B, 0x006D, 0x0071, 0x007F, 0x0083,
    0x0089, 0x008B, 0x0095, 0x0097, 0x009D, 0x00A3, 0x00A7, 0x00AD,
    0x00B3, 0x00B5, 0x00BF, 0x00C1, 0x00C5, 0x00C7, 0x00D3, 0x00DF,
    0x00E3, 0x00E5, 0x00E9, 0x00EF, 0x00F1, 0x00FB, 0x0101, 0x0107,
    0x010D, 0x010F, 0x0115, 0x0119, 0x011B, 0x0125, 0x0133, 0x0137,

    0x0139, 0x013D, 0x014B, 0x0151, 0x015B, 0x015D, 0x0161, 0x0167,
    0x016F, 0x0175, 0x017B, 0x017F, 0x0185, 0x018D, 0x0191, 0x0199,
    0x01A3, 0x01A5, 0x01AF, 0x01B1, 0x01B7, 0x01BB, 0x01C1, 0x01C9,
    0x01CD, 0x01CF, 0x01D3, 0x01DF, 0x01E7, 0x01EB, 0x01F3, 0x01F7,
    0x01FD, 0x0209, 0x020B, 0x021D, 0x0223, 0x022D, 0x0233, 0x0239,
    0x023B, 0x0241, 0x024B, 0x0251, 0x0257, 0x0259, 0x025F, 0x0265,
    0x0269, 0x026B, 0x0277, 0x0281, 0x0283, 0x0287, 0x028D, 0x0293,
    0x0295, 0x02A1, 0x02A5, 0x02AB, 0x02B3, 0x02BD, 0x02C5, 0x02CF,

    0x02D7, 0x02DD, 0x02E3, 0x02E7, 0x02EF, 0x02F5, 0x02F9, 0x0301,
    0x0305, 0x0313, 0x031D, 0x0329, 0x032B, 0x0335, 0x0337, 0x033B,
    0x033D, 0x0347, 0x0355, 0x0359, 0x035B, 0x035F, 0x036D, 0x0371,
    0x0373, 0x0377, 0x038B, 0x038F, 0x0397, 0x03A1, 0x03A9, 0x03AD,
    0x03B3, 0x03B9, 0x03C7, 0x03CB, 0x03D1, 0x03D7, 0x03DF, 0x03E5,
    0x03F1, 0x03F5, 0x03FB, 0x03FD, 0x0407, 0x0409, 0x040F, 0x0419,
    0x041B, 0x0425, 0x0427, 0x042D, 0x043F, 0x0443, 0x0445, 0x0449,
    0x044F, 0x0455, 0x045D, 0x0463, 0x0469, 0x047F, 0x0481, 0x048B,

    0x0493, 0x049D, 0x04A3, 0x04A9, 0x04B1, 0x04BD, 0x04C1, 0x04C7,
    0x04CD, 0x04CF, 0x04D5, 0x04E1, 0x04EB, 0x04FD, 0x04FF, 0x0503,
    0x0509, 0x050B, 0x0511, 0x0515, 0x0517, 0x051B, 0x0527, 0x0529,
    0x052F, 0x0551, 0x0557, 0x055D, 0x0565, 0x0577, 0x0581, 0x058F,
    0x0593, 0x0595, 0x0599, 0x059F, 0x05A7, 0x05AB, 0x05AD, 0x05B3,
    0x05BF, 0x05C9, 0x05CB, 0x05CF, 0x05D1, 0x05D5, 0x05DB, 0x05E7,
    0x05F3, 0x05FB, 0x0607, 0x060D, 0x0611, 0x0617, 0x061F, 0x0623,
    0x062B, 0x062F, 0x063D, 0x0641, 0x0647, 0x0649, 0x064D, 0x0653
};
#endif

/* Compare the first n primes with a.
 *
 * @param [in]  a       Number to check.
 * @param [out] result  Whether number was found to be prime.
 * @return  0 when no small prime matches.
 * @return  1 when small prime matches.
 */
static WC_INLINE int sp_cmp_primes(const sp_int* a, int* result)
{
    int i;
    int haveRes = 0;

    *result = MP_NO;
    /* Check one digit a against primes table. */
    for (i = 0; i < SP_PRIME_SIZE; i++) {
        if (sp_cmp_d(a, sp_primes[i]) == MP_EQ) {
            *result = MP_YES;
            haveRes = 1;
            break;
        }
    }

    return haveRes;
}

/* Using composites is only faster when using 64-bit values. */
#if !defined(WOLFSSL_SP_SMALL) && (SP_WORD_SIZE == 64)
/* Number of composites. */
#define SP_COMP_CNT     38

/* Products of small primes that fit into 64-bits. */
static sp_int_digit sp_comp[SP_COMP_CNT] = {
    0x088886ffdb344692, 0x34091fa96ffdf47b, 0x3c47d8d728a77ebb,
    0x077ab7da9d709ea9, 0x310df3e7bd4bc897, 0xe657d7a1fd5161d1,
    0x02ad3dbe0cca85ff, 0x0787f9a02c3388a7, 0x1113c5cc6d101657,
    0x2456c94f936bdb15, 0x4236a30b85ffe139, 0x805437b38eada69d,
    0x00723e97bddcd2af, 0x00a5a792ee239667, 0x00e451352ebca269,
    0x013a7955f14b7805, 0x01d37cbd653b06ff, 0x0288fe4eca4d7cdf,
    0x039fddb60d3af63d, 0x04cd73f19080fb03, 0x0639c390b9313f05,
    0x08a1c420d25d388f, 0x0b4b5322977db499, 0x0e94c170a802ee29,
    0x11f6a0e8356100df, 0x166c8898f7b3d683, 0x1babda0a0afd724b,
    0x2471b07c44024abf, 0x2d866dbc2558ad71, 0x3891410d45fb47df,
    0x425d5866b049e263, 0x51f767298e2cf13b, 0x6d9f9ece5fc74f13,
    0x7f5ffdb0f56ee64d, 0x943740d46a1bc71f, 0xaf2d7ca25cec848f,
    0xcec010484e4ad877, 0xef972c3cfafbcd25
};

/* Index of next prime after those used to create composite. */
static int sp_comp_idx[SP_COMP_CNT] = {
     15,  25,  34,  42,  50,  58,  65,  72,  79,  86,  93, 100, 106, 112, 118,
    124, 130, 136, 142, 148, 154, 160, 166, 172, 178, 184, 190, 196, 202, 208,
    214, 220, 226, 232, 238, 244, 250, 256
};
#endif

/* Determines whether any of the first n small primes divide a evenly.
 *
 * @param [in]      a        Number to check.
 * @param [in, out] haveRes  Boolean indicating a no prime result found.
 * @param [in, out] result   Whether a is known to be prime.
 * @return  MP_OKAY on success.
 * @return  Negative on failure.
 */
static WC_INLINE int sp_div_primes(const sp_int* a, int* haveRes, int* result)
{
    int i;
#if !defined(WOLFSSL_SP_SMALL) && (SP_WORD_SIZE == 64)
    int j;
#endif
    sp_int_digit d;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || (SP_WORD_SIZE < 64)
    /* Do trial division of a with all known small primes. */
    for (i = 0; i < SP_PRIME_SIZE; i++) {
        /* Small prime divides a when remainder is 0. */
        err = sp_mod_d(a, (sp_int_digit)sp_primes[i], &d);
        if ((err != MP_OKAY) || (d == 0)) {
            *result = MP_NO;
            *haveRes = 1;
            break;
        }
    }
#else
    /* Start with first prime in composite. */
    i = 0;
    for (j = 0; (!(*haveRes)) && (j < SP_COMP_CNT); j++) {
        /* Reduce a down to a single word.  */
        err = sp_mod_d(a, sp_comp[j], &d);
        if ((err != MP_OKAY) || (d == 0)) {
            *result = MP_NO;
            *haveRes = 1;
            break;
        }
        /* Do trial division of d with small primes that make up composite. */
        for (; i < sp_comp_idx[j]; i++) {
            /* Small prime divides a when remainder is 0. */
            if (d % sp_primes[i] == 0) {
                *result = MP_NO;
                *haveRes = 1;
                break;
            }
        }
    }
#endif

    return err;
}

/* Check whether a is prime by checking t iterations of Miller-Rabin.
 *
 * @param  [in]   a       SP integer to check.
 * @param  [in]   trials  Number of trials of Miller-Rabin test to perform.
 * @param  [out]  result  MP_YES when number is prime.
 *                        MP_NO otherwise.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_prime_trials(const sp_int* a, int trials, int* result)
{
    int err = MP_OKAY;
    int i;
    sp_int* n1;
    sp_int* r;
    DECL_SP_INT_ARRAY(t, a->used + 1, 2);
    DECL_SP_INT(b, a->used * 2 + 1);

    ALLOC_SP_INT_ARRAY(t, a->used + 1, 2, err, NULL);
    /* Allocate number that will hold modular exponentiation result. */
    ALLOC_SP_INT(b, a->used * 2 + 1, err, NULL);
    if (err == MP_OKAY) {
        n1 = t[0];
        r  = t[1];

        _sp_init_size(n1, a->used + 1);
        _sp_init_size(r, a->used + 1);
        _sp_init_size(b, a->used * 2 + 1);

        /* Do requested number of trials of Miller-Rabin test. */
        for (i = 0; i < trials; i++) {
            /* Miller-Rabin test with known small prime. */
            _sp_set(b, sp_primes[i]);
            err = sp_prime_miller_rabin(a, b, result, n1, r);
            if ((err != MP_OKAY) || (*result == MP_NO)) {
                break;
            }
        }

        /* Clear temporary values. */
        sp_clear(n1);
        sp_clear(r);
        sp_clear(b);
    }

    /* Free allocated temporary. */
    FREE_SP_INT(b, NULL);
    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

/* Check whether a is prime.
 * Checks against a number of small primes and does t iterations of
 * Miller-Rabin.
 *
 * @param  [in]   a       SP integer to check.
 * @param  [in]   trials  Number of trials of Miller-Rabin test to perform.
 * @param  [out]  result  MP_YES when number is prime.
 *                        MP_NO otherwise.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a or result is NULL, or trials is out of range.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_prime_is_prime(const sp_int* a, int trials, int* result)
{
    int         err = MP_OKAY;
    int         haveRes = 0;

    /* Validate parameters. */
    if ((a == NULL) || (result == NULL)) {
        if (result != NULL) {
            *result = MP_NO;
        }
        err = MP_VAL;
    }
    else if (a->used * 2 >= SP_INT_DIGITS) {
        err = MP_VAL;
    }
    /* Check validity of Miller-Rabin iterations count.
     * Must do at least one and need a unique pre-computed prime for each
     * iteration.
     */
    if ((err == MP_OKAY) && ((trials <= 0) || (trials > SP_PRIME_SIZE))) {
        *result = MP_NO;
        err = MP_VAL;
    }

    /* Short-cut, 1 is not prime. */
    if ((err == MP_OKAY) && sp_isone(a)) {
        *result = MP_NO;
        haveRes = 1;
    }

    SAVE_VECTOR_REGISTERS(err = _svr_ret;);

    /* Check against known small primes when a has 1 digit. */
    if ((err == MP_OKAY) && (!haveRes) && (a->used == 1) &&
            (a->dp[0] <= sp_primes[SP_PRIME_SIZE - 1])) {
        haveRes = sp_cmp_primes(a, result);
    }

    /* Check all small primes for even divisibility. */
    if ((err == MP_OKAY) && (!haveRes)) {
        err = sp_div_primes(a, &haveRes, result);
    }

    /* Check a number of iterations of Miller-Rabin with small primes. */
    if ((err == MP_OKAY) && (!haveRes)) {
        err = _sp_prime_trials(a, trials, result);
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}

#ifndef WC_NO_RNG
/* Check whether a is prime by doing t iterations of Miller-Rabin.
 *
 * t random numbers should give a (1/4)^t chance of a false prime.
 *
 * @param  [in]   a       SP integer to check.
 * @param  [in]   trials  Number of iterations of Miller-Rabin test to perform.
 * @param  [out]  result  MP_YES when number is prime.
 *                        MP_NO otherwise.
 * @param  [in]   rng     Random number generator for Miller-Rabin testing.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, result or rng is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_prime_random_trials(const sp_int* a, int trials, int* result,
    WC_RNG* rng)
{
    int err = MP_OKAY;
    int bits = sp_count_bits(a);
    word32 baseSz = ((word32)bits + 7) / 8;
    DECL_SP_INT_ARRAY(ds, a->used + 1, 2);
    DECL_SP_INT_ARRAY(d, a->used * 2 + 1, 2);

    ALLOC_SP_INT_ARRAY(ds, a->used + 1, 2, err, NULL);
    ALLOC_SP_INT_ARRAY(d, a->used * 2 + 1, 2, err, NULL);
    if (err == MP_OKAY) {
        sp_int* c  = ds[0];
        sp_int* n1 = ds[1];
        sp_int* b  = d[0];
        sp_int* r  = d[1];

        _sp_init_size(c , a->used + 1);
        _sp_init_size(n1, a->used + 1);
        _sp_init_size(b , a->used * 2 + 1);
        _sp_init_size(r , a->used * 2 + 1);

        _sp_sub_d(a, 2, c);

        bits &= SP_WORD_MASK;

        /* Keep trying random numbers until all trials complete. */
        while (trials > 0) {
            /* Generate random trial number. */
            err = wc_RNG_GenerateBlock(rng, (byte*)b->dp, baseSz);
            if (err != MP_OKAY) {
                break;
            }
            b->used = a->used;
        #ifdef BIG_ENDIAN_ORDER
            /* Fix top digit if fewer bytes than a full digit generated. */
            if (((baseSz * 8) & SP_WORD_MASK) != 0) {
                b->dp[b->used-1] >>=
                    SP_WORD_SIZE - ((baseSz * 8) & SP_WORD_MASK);
            }
        #endif /* BIG_ENDIAN_ORDER */

            /* Ensure the top word has no more bits than necessary. */
            if (bits > 0) {
                b->dp[b->used - 1] &= ((sp_int_digit)1 << bits) - 1;
                sp_clamp(b);
            }

            /* Can't use random value it is: 0, 1, a-2, a-1, >= a  */
            if ((sp_cmp_d(b, 2) != MP_GT) || (_sp_cmp(b, c) != MP_LT)) {
                continue;
            }

            /* Perform Miller-Rabin test with random value. */
            err = sp_prime_miller_rabin(a, b, result, n1, r);
            if ((err != MP_OKAY) || (*result == MP_NO)) {
                break;
            }

            /* Trial complete. */
            trials--;
        }

        /* Zeroize temporary values used when generating private prime. */
        sp_forcezero(n1);
        sp_forcezero(r);
        sp_forcezero(b);
        sp_forcezero(c);
    }

    FREE_SP_INT_ARRAY(d, NULL);
    FREE_SP_INT_ARRAY(ds, NULL);
    return err;
}
#endif /*!WC_NO_RNG */

/* Check whether a is prime.
 * Checks against a number of small primes and does t iterations of
 * Miller-Rabin.
 *
 * @param  [in]   a       SP integer to check.
 * @param  [in]   trials  Number of iterations of Miller-Rabin test to perform.
 * @param  [out]  result  MP_YES when number is prime.
 *                        MP_NO otherwise.
 * @param  [in]   rng     Random number generator for Miller-Rabin testing.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, result or rng is NULL.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_prime_is_prime_ex(const sp_int* a, int trials, int* result, WC_RNG* rng)
{
    int err = MP_OKAY;
    int ret = MP_YES;
    int haveRes = 0;

    if ((a == NULL) || (result == NULL) || (rng == NULL)) {
        err = MP_VAL;
    }
#ifndef WC_NO_RNG
    if ((err == MP_OKAY) && (a->used * 2 >= SP_INT_DIGITS)) {
        err = MP_VAL;
    }
#endif
#ifdef WOLFSSL_SP_INT_NEGATIVE
    if ((err == MP_OKAY) && (a->sign == MP_NEG)) {
        err = MP_VAL;
    }
#endif

    /* Ensure trials is valid. Maximum based on number of small primes
     * available. */
    if ((err == MP_OKAY) && ((trials <= 0) || (trials > SP_PRIME_SIZE))) {
        err = MP_VAL;
    }

    if ((err == MP_OKAY) && sp_isone(a)) {
        ret = MP_NO;
        haveRes = 1;
    }

    SAVE_VECTOR_REGISTERS(err = _svr_ret;);

    /* Check against known small primes when a has 1 digit. */
    if ((err == MP_OKAY) && (!haveRes) && (a->used == 1) &&
            (a->dp[0] <= (sp_int_digit)sp_primes[SP_PRIME_SIZE - 1])) {
        haveRes = sp_cmp_primes(a, &ret);
    }

    /* Check all small primes for even divisibility. */
    if ((err == MP_OKAY) && (!haveRes)) {
        err = sp_div_primes(a, &haveRes, &ret);
    }

#ifndef WC_NO_RNG
    /* Check a number of iterations of Miller-Rabin with random large values. */
    if ((err == MP_OKAY) && (!haveRes)) {
        err = _sp_prime_random_trials(a, trials, &ret, rng);
    }
#else
    (void)trials;
#endif /* !WC_NO_RNG */

    if (result != NULL) {
        *result = ret;
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}
#endif /* WOLFSSL_SP_PRIME_GEN */

#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)

/* Calculates the Greatest Common Denominator (GCD) of a and b into r.
 *
 * Find the largest number that divides both a and b without remainder.
 * r <= a, r <= b, a % r == 0, b % r == 0
 *
 * a and b are positive integers.
 *
 * Euclidian Algorithm:
 *  1. If a > b then a = b, b = a
 *  2. u = a
 *  3. v = b % a
 *  4. While v != 0
 *   4.1. t = u % v
 *   4.2. u <= v, v <= t, t <= u
 *  5. r = u
 *
 * @param  [in]   a  SP integer of first operand.
 * @param  [in]   b  SP integer of second operand.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static WC_INLINE int _sp_gcd(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    sp_int* u = NULL;
    sp_int* v = NULL;
    sp_int* t = NULL;
    /* Used for swapping sp_ints. */
    sp_int* s;
    /* Determine maximum digit length numbers will reach. */
    unsigned int used = (a->used >= b->used) ? a->used + 1 : b->used + 1;
    DECL_SP_INT_ARRAY(d, used, 3);

    SAVE_VECTOR_REGISTERS(err = _svr_ret;);

    ALLOC_SP_INT_ARRAY(d, used, 3, err, NULL);
    if (err == MP_OKAY) {
        u = d[0];
        v = d[1];
        t = d[2];

        _sp_init_size(u, used);
        _sp_init_size(v, used);
        _sp_init_size(t, used);

        /* 1. If a > b then a = b, b = a.
         *    Make a <= b.
         */
        if (_sp_cmp(a, b) == MP_GT) {
            const sp_int* tmp;
            tmp = a;
            a = b;
            b = tmp;
        }
        /* 2. u = a, v = b mod a */
        _sp_copy(a, u);
        /* 3. v = b mod a */
        if (a->used == 1) {
            err = sp_mod_d(b, a->dp[0], &v->dp[0]);
            v->used = (v->dp[0] != 0);
        }
        else {
            err = sp_mod(b, a, v);
        }
    }

    /* 4. While v != 0 */
    /* Keep reducing larger by smaller until smaller is 0 or u and v both one
     * digit.
     */
    while ((err == MP_OKAY) && (!sp_iszero(v)) && (u->used > 1)) {
        /* u' = v, v' = u mod v */
        /* 4.1 t = u mod v */
        if (v->used == 1) {
            err = sp_mod_d(u, v->dp[0], &t->dp[0]);
            t->used = (t->dp[0] != 0);
        }
        else {
            err = sp_mod(u, v, t);
        }
        /* 4.2. u <= v, v <= t, t <= u */
        s = u; u = v; v = t; t = s;
    }
    /* Only one digit remaining in u and v. */
    while ((err == MP_OKAY) && (!sp_iszero(v))) {
        /* u' = v, v' = u mod v */
        /* 4.1 t = u mod v */
        t->dp[0] = u->dp[0] % v->dp[0];
        t->used = (t->dp[0] != 0);
        /* 4.2. u <= v, v <= t, t <= u */
        s = u; u = v; v = t; t = s;
    }
    if (err == MP_OKAY) {
        /* 5. r = u */
        _sp_copy(u, r);
    }

    FREE_SP_INT_ARRAY(d, NULL);

    RESTORE_VECTOR_REGISTERS();

    return err;
}

/* Calculates the Greatest Common Denominator (GCD) of a and b into r.
 *
 * Find the largest number that divides both a and b without remainder.
 * r <= a, r <= b, a % r == 0, b % r == 0
 *
 * a and b are positive integers.
 *
 * @param  [in]   a  SP integer of first operand.
 * @param  [in]   b  SP integer of second operand.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b or r is NULL or too large.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_gcd(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
    /* Check that we have space in numbers to do work. */
    else if ((a->used >= SP_INT_DIGITS) || (b->used >= SP_INT_DIGITS)) {
        err = MP_VAL;
    }
    /* Check that r is large enough to hold maximum sized result. */
    else if (((a->used <= b->used) && (r->size < a->used)) ||
             ((b->used < a->used) && (r->size < b->used))) {
        err = MP_VAL;
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Algorithm doesn't work with negative numbers. */
    else if ((a->sign == MP_NEG) || (b->sign == MP_NEG)) {
        err = MP_VAL;
    }
#endif
    else if (sp_iszero(a)) {
        /* GCD of 0 and 0 is undefined - all integers divide 0. */
        if (sp_iszero(b)) {
            err = MP_VAL;
        }
        else {
            /* GCD of 0 and b is b - b divides 0. */
            err = sp_copy(b, r);
        }
    }
    else if (sp_iszero(b)) {
        /* GCD of 0 and a is a - a divides 0. */
        err = sp_copy(a, r);
    }
    else {
        /* Calculate GCD. */
        err = _sp_gcd(a, b, r);
    }

    return err;
}

#endif /* WOLFSSL_SP_MATH_ALL && !NO_RSA && WOLFSSL_KEY_GEN */

#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN) && \
    (!defined(WC_RSA_BLINDING) || defined(HAVE_FIPS) || defined(HAVE_SELFTEST))

/* Calculates the Lowest Common Multiple (LCM) of a and b and stores in r.
 * Smallest number divisible by both numbers.
 *
 * a and b are positive integers.
 *
 * lcm(a, b) = (a / gcd(a, b)) * b
 * Divide the common divisor from a and multiply by b.
 *
 * Algorithm:
 *  1. t0 = gcd(a, b)
 *  2. If a > b then
 *   2.1. t1 = a / t0
 *   2.2. r = b * t1
 *  3. Else
 *   3.1. t1 = b / t0
 *   3.2. r = a * t1
 *
 * @param  [in]   a  SP integer of first operand.
 * @param  [in]   b  SP integer of second operand.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
static int _sp_lcm(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;
    /* Determine maximum digit length numbers will reach. */
    unsigned int used = ((a->used >= b->used) ? a->used + 1: b->used + 1);
    DECL_SP_INT_ARRAY(t, used, 2);

    ALLOC_SP_INT_ARRAY(t, used, 2, err, NULL);
    if (err == MP_OKAY) {
        _sp_init_size(t[0], used);
        _sp_init_size(t[1], used);

        SAVE_VECTOR_REGISTERS(err = _svr_ret;);

        if (err == MP_OKAY) {
            /* 1. t0 = gcd(a, b) */
            err = sp_gcd(a, b, t[0]);
        }

        if (err == MP_OKAY) {
            /* Divide the greater by the common divisor and multiply by other
             * to operate on the smallest length numbers.
             */
            /* 2. If a > b then */
            if (_sp_cmp_abs(a, b) == MP_GT) {
                /* 2.1. t1 = a / t0 */
                err = sp_div(a, t[0], t[1], NULL);
                if (err == MP_OKAY) {
                    /* 2.2. r = b * t1 */
                    err = sp_mul(b, t[1], r);
                }
            }
            /* 3. Else */
            else {
                /* 3.1. t1 = b / t0 */
                err = sp_div(b, t[0], t[1], NULL);
                if (err == MP_OKAY) {
                    /* 3.2. r = a * t1 */
                    err = sp_mul(a, t[1], r);
                }
            }
        }

        RESTORE_VECTOR_REGISTERS();
    }

    FREE_SP_INT_ARRAY(t, NULL);
    return err;
}

/* Calculates the Lowest Common Multiple (LCM) of a and b and stores in r.
 * Smallest number divisible by both numbers.
 *
 * a and b are positive integers.
 *
 * @param  [in]   a  SP integer of first operand.
 * @param  [in]   b  SP integer of second operand.
 * @param  [out]  r  SP integer to hold result.
 *
 * @return  MP_OKAY on success.
 * @return  MP_VAL when a, b or r is NULL; or a or b is zero.
 * @return  MP_MEM when dynamic memory allocation fails.
 */
int sp_lcm(const sp_int* a, const sp_int* b, sp_int* r)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL) || (r == NULL)) {
        err = MP_VAL;
    }
#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* Ensure a and b are positive. */
    else if ((a->sign == MP_NEG) || (b->sign >= MP_NEG)) {
        err = MP_VAL;
    }
#endif
    /* Ensure r has space for maximumal result. */
    else if (r->size < a->used + b->used) {
        err = MP_VAL;
    }

    /* LCM of 0 and any number is undefined as 0 is not in the set of values
     * being used.
     */
    if ((err == MP_OKAY) && (mp_iszero(a) || mp_iszero(b))) {
        err = MP_VAL;
    }

    if (err == MP_OKAY) {
        /* Do operation. */
        err = _sp_lcm(a, b, r);
    }

    return err;
}

#endif /* WOLFSSL_SP_MATH_ALL && !NO_RSA && WOLFSSL_KEY_GEN */

/* Returns the run time settings.
 *
 * @return  Settings value.
 */
word32 CheckRunTimeSettings(void)
{
    return CTC_SETTINGS;
}

/* Returns the fast math settings.
 *
 * @return  Setting - number of bits in a digit.
 */
word32 CheckRunTimeFastMath(void)
{
    return SP_WORD_SIZE;
}

#ifdef WOLFSSL_CHECK_MEM_ZERO
/* Add an MP to check.
 *
 * @param [in] name  Name of address to check.
 * @param [in] sp    sp_int that needs to be checked.
 */
void sp_memzero_add(const char* name, sp_int* sp)
{
    wc_MemZero_Add(name, sp->dp, sp->size * sizeof(sp_digit));
}

/* Check the memory in the data pointer for memory that must be zero.
 *
 * @param [in] sp    sp_int that needs to be checked.
 */
void sp_memzero_check(sp_int* sp)
{
    wc_MemZero_Check(sp->dp, sp->size * sizeof(sp_digit));
}
#endif /* WOLFSSL_CHECK_MEM_ZERO */

#if (!defined(WOLFSSL_SMALL_STACK) && !defined(SP_ALLOC)) || \
    defined(WOLFSSL_SP_NO_MALLOC)
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(WOLFSSL_SP_NO_DYN_STACK)
#pragma GCC diagnostic pop
#endif
#endif

#endif /* WOLFSSL_SP_MATH || WOLFSSL_SP_MATH_ALL */
