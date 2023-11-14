/* asm.c
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

/*
 * Based on public domain TomsFastMath 0.10 by Tom St Denis, tomstdenis@iahu.ca,
 * http://math.libtomcrypt.com
 */


/******************************************************************/
/* fp_montgomery_reduce.c asm or generic */


/* Each platform needs to query info type 1 from cpuid to see if aesni is
 * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
 */

#if defined(HAVE_INTEL_MULX)
#ifndef _MSC_VER
    #define cpuid(reg, leaf, sub)\
            __asm__ __volatile__ ("cpuid":\
             "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3]) :\
             "a" (leaf), "c"(sub));
#else
    #include <intrin.h>
    #define cpuid(a,b,c) __cpuidex((int*)a,b,c)
#endif /* _MSC_VER */

#define EAX 0
#define EBX 1
#define ECX 2
#define EDX 3

#define CPUID_AVX1   0x1
#define CPUID_AVX2   0x2
#define CPUID_RDRAND 0x4
#define CPUID_RDSEED 0x8
#define CPUID_BMI2   0x10   /* MULX, RORX */
#define CPUID_ADX    0x20   /* ADCX, ADOX */

#define IS_INTEL_AVX1       (cpuid_flags&CPUID_AVX1)
#define IS_INTEL_AVX2       (cpuid_flags&CPUID_AVX2)
#define IS_INTEL_BMI2       (cpuid_flags&CPUID_BMI2)
#define IS_INTEL_ADX        (cpuid_flags&CPUID_ADX)
#define IS_INTEL_RDRAND     (cpuid_flags&CPUID_RDRAND)
#define IS_INTEL_RDSEED     (cpuid_flags&CPUID_RDSEED)
#define SET_FLAGS

static word32 cpuid_check = 0 ;
static word32 cpuid_flags = 0 ;

static word32 cpuid_flag(word32 leaf, word32 sub, word32 num, word32 bit) {
    int got_intel_cpu = 0;
    int got_amd_cpu = 0;
    unsigned int reg[5];

    reg[4] = '\0' ;
    cpuid(reg, 0, 0);

    /* check for intel cpu */
    if( memcmp((char *)&(reg[EBX]), "Genu", 4) == 0 &&
        memcmp((char *)&(reg[EDX]), "ineI", 4) == 0 &&
        memcmp((char *)&(reg[ECX]), "ntel", 4) == 0) {
        got_intel_cpu = 1;
    }

    /* check for AMD cpu */
    if( memcmp((char *)&(reg[EBX]), "Auth", 4) == 0 &&
        memcmp((char *)&(reg[EDX]), "enti", 4) == 0 &&
        memcmp((char *)&(reg[ECX]), "cAMD", 4) == 0) {
        got_amd_cpu = 1;
    }
    if (got_intel_cpu || got_amd_cpu) {
        cpuid(reg, leaf, sub);
        return((reg[num]>>bit)&0x1) ;
    }
    return 0 ;
}

WC_INLINE static int set_cpuid_flags(void) {
    if(cpuid_check == 0) {
        if(cpuid_flag(7, 0, EBX, 8)){  cpuid_flags |= CPUID_BMI2 ; }
        if(cpuid_flag(7, 0, EBX,19)){  cpuid_flags |= CPUID_ADX  ; }
		cpuid_check = 1 ;
		return 0 ;
    }
    return 1 ;
}

#define RETURN return
#define IF_HAVE_INTEL_MULX(func, ret)    \
   if(cpuid_check==0)set_cpuid_flags() ; \
   if(IS_INTEL_BMI2 && IS_INTEL_ADX){  func;  ret ;  }

#else
    #define IF_HAVE_INTEL_MULX(func, ret) WC_DO_NOTHING
#endif

#if defined(TFM_X86) && !defined(TFM_SSE2)
/* x86-32 code */

#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                          \
__asm__(                                                  \
   "movl %5,%%eax \n\t"                                   \
   "mull %4       \n\t"                                   \
   "addl %1,%%eax \n\t"                                   \
   "adcl $0,%%edx \n\t"                                   \
   "addl %%eax,%0 \n\t"                                   \
   "adcl $0,%%edx \n\t"                                   \
   "movl %%edx,%1 \n\t"                                   \
:"=g"(_c[LO]), "=r"(cy)                                   \
:"0"(_c[LO]), "1"(cy), "r"(mu), "r"(*tmpm++)              \
: "%eax", "%edx", "cc")

#define PROPCARRY                           \
__asm__(                                    \
   "addl   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbl %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%eax", "cc")

/******************************************************************/
#elif defined(TFM_X86_64)
/* x86-64 code */

#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                          \
__asm__(                                                  \
   "movq %5,%%rax \n\t"                                   \
   "mulq %4       \n\t"                                   \
   "addq %1,%%rax \n\t"                                   \
   "adcq $0,%%rdx \n\t"                                   \
   "addq %%rax,%0 \n\t"                                   \
   "adcq $0,%%rdx \n\t"                                   \
   "movq %%rdx,%1 \n\t"                                   \
:"=g"(_c[LO]), "=r"(cy)                                   \
:"0"(_c[LO]), "1"(cy), "r"(mu), "r"(*tmpm++)              \
: "%rax", "%rdx", "cc")

#if defined(HAVE_INTEL_MULX)
#define MULX_INNERMUL8(x,y,z,cy)                                       \
    __asm__  volatile (                                                \
        "movq	%[yn], %%rdx\n\t"                                      \
        "xorq	%%rcx, %%rcx\n\t"                                      \
        "movq   0(%[c]), %%r8\n\t"                                     \
        "movq   8(%[c]), %%r9\n\t"                                     \
        "movq   16(%[c]), %%r10\n\t"                                   \
        "movq   24(%[c]), %%r11\n\t"                                   \
        "movq   32(%[c]), %%r12\n\t"                                   \
        "movq   40(%[c]), %%r13\n\t"                                   \
        "movq   48(%[c]), %%r14\n\t"                                   \
        "movq   56(%[c]), %%r15\n\t"                                   \
                                                                       \
        "mulx	0(%[xp]), %%rax, %%rcx\n\t"                            \
        "adcxq	%[cy], %%r8\n\t"                                       \
        "adoxq	%%rax, %%r8\n\t"                                       \
        "mulx	8(%[xp]), %%rax, %[cy]\n\t"                            \
        "adcxq	%%rcx, %%r9\n\t"                                       \
        "adoxq	%%rax, %%r9\n\t"                                       \
        "mulx	16(%[xp]), %%rax, %%rcx\n\t"                           \
        "adcxq	%[cy], %%r10\n\t"                                      \
        "adoxq	%%rax, %%r10\n\t"                                      \
        "mulx	24(%[xp]), %%rax, %[cy]\n\t"                           \
        "adcxq	%%rcx, %%r11\n\t"                                      \
        "adoxq	%%rax, %%r11\n\t"                                      \
        "mulx	32(%[xp]), %%rax, %%rcx\n\t"                           \
        "adcxq	%[cy], %%r12\n\t"                                      \
        "adoxq	%%rax, %%r12\n\t"                                      \
        "mulx	40(%[xp]), %%rax, %[cy]\n\t"                           \
        "adcxq	%%rcx, %%r13\n\t"                                      \
        "adoxq	%%rax, %%r13\n\t"                                      \
        "mulx	48(%[xp]), %%rax, %%rcx\n\t"                           \
        "adcxq	%[cy], %%r14\n\t"                                      \
        "adoxq	%%rax, %%r14\n\t"                                      \
        "adcxq	%%rcx, %%r15\n\t"                                      \
        "mulx	56(%[xp]), %%rax, %[cy]\n\t"                           \
        "movq	$0, %%rdx\n\t"                                         \
        "adoxq	%%rdx, %%rax\n\t"                                      \
        "adcxq	%%rdx, %[cy]\n\t"                                      \
        "adoxq	%%rdx, %[cy]\n\t"                                      \
        "addq   %%rax, %%r15\n\t"                                      \
        "adcq   $0, %[cy]\n\t"                                         \
                                                                       \
        "movq   %%r8,   0(%[c])\n\t"                                   \
        "movq   %%r9,   8(%[c])\n\t"                                   \
        "movq   %%r10, 16(%[c])\n\t"                                   \
        "movq   %%r11, 24(%[c])\n\t"                                   \
        "movq   %%r12, 32(%[c])\n\t"                                   \
        "movq   %%r13, 40(%[c])\n\t"                                   \
        "movq   %%r14, 48(%[c])\n\t"                                   \
        "movq   %%r15, 56(%[c])\n\t"                                   \
        : [cy] "+r" (cy)                                               \
        : [xp] "r" (x), [c] "r" (c_mulx), [yn] "rm" (y)                \
        :"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", \
         "%rdx", "%rax", "%rcx" \
    )

#define INNERMUL8_MULX \
{\
    MULX_INNERMUL8(tmpm, mu, _c, cy);\
}
#endif

#define INNERMUL8 \
 __asm__(                    \
 "movq 0(%5),%%rax    \n\t"  \
 "movq 0(%2),%%r10    \n\t"  \
 "movq 0x8(%5),%%r11  \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x8(%2),%%r10  \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0(%0)    \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x10(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x10(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x8(%0)  \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x18(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x18(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x10(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x20(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x20(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x18(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x28(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x28(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x20(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x30(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x30(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x28(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "movq 0x38(%5),%%r11 \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq 0x38(%2),%%r10 \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x30(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
 "movq %%r11,%%rax    \n\t"  \
 "mulq %4             \n\t"  \
 "addq %%r10,%%rax    \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "addq %3,%%rax       \n\t"  \
 "adcq $0,%%rdx       \n\t"  \
 "movq %%rax,0x38(%0) \n\t"  \
 "movq %%rdx,%1       \n\t"  \
 \
:"=r"(_c), "=r"(cy)                    \
: "0"(_c),  "1"(cy), "g"(mu), "r"(tmpm)\
: "%rax", "%rdx", "%r10", "%r11", "cc")

#define PROPCARRY                           \
__asm__(                                    \
   "addq   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbq %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%rax", "cc")

/******************************************************************/
#elif defined(TFM_SSE2)
/* SSE2 code (assumes 32-bit fp_digits) */
/* XMM register assignments:
 * xmm0  *tmpm++, then Mu * (*tmpm++)
 * xmm1  c[x], then Mu
 * xmm2  mp
 * xmm3  cy
 * xmm4  _c[LO]
 */

#define MONT_START \
   __asm__("movd %0,%%mm2"::"g"(mp))

#define MONT_FINI \
   __asm__("emms")

#define LOOP_START          \
__asm__(                    \
"movd %0,%%mm1        \n\t" \
"pxor %%mm3,%%mm3     \n\t" \
"pmuludq %%mm2,%%mm1  \n\t" \
:: "g"(c[x]))

/* pmuludq on mmx registers does a 32x32->64 multiply. */
#define INNERMUL               \
__asm__(                       \
   "movd %1,%%mm4        \n\t" \
   "movd %2,%%mm0        \n\t" \
   "paddq %%mm4,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm0  \n\t" \
   "paddq %%mm0,%%mm3    \n\t" \
   "movd %%mm3,%0        \n\t" \
   "psrlq $32, %%mm3     \n\t" \
:"=g"(_c[LO]) : "0"(_c[LO]), "g"(*tmpm++) );

#define INNERMUL8 \
__asm__(                       \
   "movd 0(%1),%%mm4     \n\t" \
   "movd 0(%2),%%mm0     \n\t" \
   "paddq %%mm4,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm0  \n\t" \
   "movd 4(%2),%%mm5     \n\t" \
   "paddq %%mm0,%%mm3    \n\t" \
   "movd 4(%1),%%mm6     \n\t" \
   "movd %%mm3,0(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "movd 8(%2),%%mm6     \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd 8(%1),%%mm7     \n\t" \
   "movd %%mm3,4(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm7,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm6  \n\t" \
   "movd 12(%2),%%mm7    \n\t" \
   "paddq %%mm6,%%mm3    \n\t" \
   "movd 12(%1),%%mm5     \n\t" \
   "movd %%mm3,8(%0)     \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm5,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm7  \n\t" \
   "movd 16(%2),%%mm5    \n\t" \
   "paddq %%mm7,%%mm3    \n\t" \
   "movd 16(%1),%%mm6    \n\t" \
   "movd %%mm3,12(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "movd 20(%2),%%mm6    \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd 20(%1),%%mm7    \n\t" \
   "movd %%mm3,16(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm7,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm6  \n\t" \
   "movd 24(%2),%%mm7    \n\t" \
   "paddq %%mm6,%%mm3    \n\t" \
   "movd 24(%1),%%mm5     \n\t" \
   "movd %%mm3,20(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm5,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm7  \n\t" \
   "movd 28(%2),%%mm5    \n\t" \
   "paddq %%mm7,%%mm3    \n\t" \
   "movd 28(%1),%%mm6    \n\t" \
   "movd %%mm3,24(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
\
   "paddq %%mm6,%%mm3    \n\t" \
   "pmuludq %%mm1,%%mm5  \n\t" \
   "paddq %%mm5,%%mm3    \n\t" \
   "movd %%mm3,28(%0)    \n\t" \
   "psrlq $32, %%mm3     \n\t" \
:"=r"(_c) : "0"(_c), "r"(tmpm) );

/* TAO switched tmpm from "g" to "r" after gcc tried to index the indexed stack
   pointer */

#define LOOP_END \
__asm__( "movd %%mm3,%0  \n" :"=r"(cy))

#define PROPCARRY                           \
__asm__(                                    \
   "addl   %1,%0    \n\t"                   \
   "setb   %%al     \n\t"                   \
   "movzbl %%al,%1 \n\t"                    \
:"=g"(_c[LO]), "=r"(cy)                     \
:"0"(_c[LO]), "1"(cy)                       \
: "%eax", "cc")

/******************************************************************/
#elif defined(TFM_ARM)
   /* ARMv4 code */

#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp


#ifdef __thumb__

#define INNERMUL                    \
__asm__(                            \
    " LDR    r0,%1            \n\t" \
    " ADDS   r0,r0,%0         \n\t" \
    " ITE    CS               \n\t" \
    " MOVCS  %0,#1            \n\t" \
    " MOVCC  %0,#0            \n\t" \
    " UMLAL  r0,%0,%3,%4      \n\t" \
    " STR    r0,%1            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(*tmpm++),"m"(_c[0]):"r0","cc");

#define PROPCARRY                  \
__asm__(                           \
    " LDR   r0,%1            \n\t" \
    " ADDS  r0,r0,%0         \n\t" \
    " STR   r0,%1            \n\t" \
    " ITE   CS               \n\t" \
    " MOVCS %0,#1            \n\t" \
    " MOVCC %0,#0            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"m"(_c[0]):"r0","cc");


/* TAO thumb mode uses ite (if then else) to detect carry directly
 * fixed unmatched constraint warning by changing 1 to m  */

#else  /* __thumb__ */

#define INNERMUL                    \
__asm__(                            \
    " LDR    r0,%1            \n\t" \
    " ADDS   r0,r0,%0         \n\t" \
    " MOVCS  %0,#1            \n\t" \
    " MOVCC  %0,#0            \n\t" \
    " UMLAL  r0,%0,%3,%4      \n\t" \
    " STR    r0,%1            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(*tmpm++),"1"(_c[0]):"r0","cc");

#define PROPCARRY                  \
__asm__(                           \
    " LDR   r0,%1            \n\t" \
    " ADDS  r0,r0,%0         \n\t" \
    " STR   r0,%1            \n\t" \
    " MOVCS %0,#1            \n\t" \
    " MOVCC %0,#0            \n\t" \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"r0","cc");

#endif /* __thumb__ */

#elif defined(TFM_PPC32)

/* PPC32 */
#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                     \
__asm__(                             \
   " mullw    16,%3,%4       \n\t"   \
   " mulhwu   17,%3,%4       \n\t"   \
   " addc     16,16,%2       \n\t"   \
   " addze    17,17          \n\t"   \
   " addc     %1,16,%5       \n\t"   \
   " addze    %0,17          \n\t"   \
:"=r"(cy),"=r"(_c[0]):"0"(cy),"r"(mu),"r"(tmpm[0]),"1"(_c[0]):"16", "17", "cc"); ++tmpm;

#define PROPCARRY                    \
__asm__(                             \
   " addc     %1,%3,%2      \n\t"    \
   " xor      %0,%2,%2      \n\t"    \
   " addze    %0,%2         \n\t"    \
:"=r"(cy),"=r"(_c[0]):"0"(cy),"1"(_c[0]):"cc");

#elif defined(TFM_PPC64)

/* PPC64 */
#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                      \
__asm__(                              \
   " mulld    r16,%3,%4       \n\t"   \
   " mulhdu   r17,%3,%4       \n\t"   \
   " addc     r16,16,%0       \n\t"   \
   " addze    r17,r17         \n\t"   \
   " ldx      r18,0,%1        \n\t"   \
   " addc     r16,r16,r18     \n\t"   \
   " addze    %0,r17          \n\t"   \
   " sdx      r16,0,%1        \n\t"   \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"r"(mu),"r"(tmpm[0]),"1"(_c[0]):"r16", "r17", "r18","cc"); ++tmpm;

#define PROPCARRY                     \
__asm__(                              \
   " ldx      r16,0,%1       \n\t"    \
   " addc     r16,r16,%0     \n\t"    \
   " sdx      r16,0,%1       \n\t"    \
   " xor      %0,%0,%0       \n\t"    \
   " addze    %0,%0          \n\t"    \
:"=r"(cy),"=m"(_c[0]):"0"(cy),"1"(_c[0]):"r16","cc");

/******************************************************************/

#elif defined(TFM_AVR32)

/* AVR32 */
#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                    \
__asm__(                            \
    " ld.w   r2,%1            \n\t" \
    " add    r2,%0            \n\t" \
    " eor    r3,r3            \n\t" \
    " acr    r3               \n\t" \
    " macu.d r2,%3,%4         \n\t" \
    " st.w   %1,r2            \n\t" \
    " mov    %0,r3            \n\t" \
:"=r"(cy),"=r"(_c):"0"(cy),"r"(mu),"r"(*tmpm++),"1"(_c):"r2","r3");

#define PROPCARRY                    \
__asm__(                             \
   " ld.w     r2,%1         \n\t"    \
   " add      r2,%0         \n\t"    \
   " st.w     %1,r2         \n\t"    \
   " eor      %0,%0         \n\t"    \
   " acr      %0            \n\t"    \
:"=r"(cy),"=r"(&_c[0]):"0"(cy),"1"(&_c[0]):"r2","cc");

/******************************************************************/
#elif defined(TFM_MIPS)

/* MIPS */
#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                     \
__asm__(                             \
   " multu    %3,%4          \n\t"   \
   " mflo     $12            \n\t"   \
   " mfhi     $13            \n\t"   \
   " addu     $12,$12,%0     \n\t"   \
   " sltu     $10,$12,%0     \n\t"   \
   " addu     $13,$13,$10    \n\t"   \
   " lw       $10,%1         \n\t"   \
   " addu     $12,$12,$10    \n\t"   \
   " sltu     $10,$12,$10    \n\t"   \
   " addu     %0,$13,$10     \n\t"   \
   " sw       $12,%1         \n\t"   \
:"+r"(cy),"+m"(_c[0]):""(cy),"r"(mu),"r"(tmpm[0]),""(_c[0]):"$10","$12","$13"); ++tmpm;

#define PROPCARRY                    \
__asm__(                             \
   " lw       $10,%1        \n\t"    \
   " addu     $10,$10,%0    \n\t"    \
   " sw       $10,%1        \n\t"    \
   " sltu     %0,$10,%0     \n\t"    \
:"+r"(cy),"+m"(_c[0]):""(cy),""(_c[0]):"$10");

/******************************************************************/
#else

/* ISO C code */
#define MONT_START
#define MONT_FINI
#define LOOP_END
#define LOOP_START \
   mu = c[x] * mp

#define INNERMUL                                      \
   do { fp_word t;                                    \
   t  = ((fp_word)_c[0] + (fp_word)cy) +              \
                (((fp_word)mu) * ((fp_word)*tmpm++)); \
   _c[0] = (fp_digit)t;                               \
   cy = (fp_digit)(t >> DIGIT_BIT);                   \
   } while (0)

#define PROPCARRY \
   do { fp_digit t = _c[0] += cy; cy = (t < cy); } while (0)

#endif
/******************************************************************/


#define LO  0
/* end fp_montogomery_reduce.c asm */


/* start fp_sqr_comba.c asm */
#if defined(TFM_X86)

/* x86-32 optimized */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

#define SQRADD(i, j)                                      \
__asm__(                                                  \
     "movl  %3,%%eax     \n\t"                            \
     "mull  %%eax        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"+m"(c0), "+m"(c1), "+m"(c2)                        \
     : "m"(i)                                             \
     :"%eax","%edx","cc");

#define SQRADD2(i, j)                                     \
__asm__(                                                  \
     "movl  %3,%%eax     \n\t"                            \
     "mull  %4           \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"+m"(c0), "+m"(c1), "+m"(c2)                        \
     : "m"(i), "m"(j)                                     \
     :"%eax","%edx", "cc");

#define SQRADDSC(i, j)                                    \
__asm__(                                                  \
     "movl  %3,%%eax     \n\t"                            \
     "mull  %4           \n\t"                            \
     "movl  %%eax,%0     \n\t"                            \
     "movl  %%edx,%1     \n\t"                            \
     "xorl  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2)                     \
     : "g"(i), "g"(j)                                     \
     :"%eax","%edx","cc");

#define SQRADDAC(i, j)                                    \
__asm__(                                                  \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %7           \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2)                     \
     : "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j)       \
     :"%eax","%edx","cc");

#define SQRADDDB                                          \
__asm__(                                                  \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2)                        \
     : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1),     \
       "r"(sc2)                                           \
     : "cc");

#elif defined(TFM_X86_64)
/* x86-64 optimized */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

#define SQRADD(i, j)                                      \
__asm__(                                                  \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %%rax        \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "x"(i) :"%rax","%rdx","cc");

#define SQRADD2(i, j)                                     \
__asm__(                                                  \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)  :"%rax","%rdx","cc");

#define SQRADDSC(i, j)                                    \
__asm__(                                                  \
     "movq  %3,%%rax     \n\t"                            \
     "mulq  %4           \n\t"                            \
     "movq  %%rax,%0     \n\t"                            \
     "movq  %%rdx,%1     \n\t"                            \
     "xorq  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "g"(i), "g"(j) :"%rax","%rdx","cc");

#define SQRADDAC(i, j)                                                         \
__asm__(                                                  \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "g"(i), "g"(j) :"%rax","%rdx","cc");

#define SQRADDDB                                          \
__asm__(                                                  \
     "addq %6,%0         \n\t"                            \
     "adcq %7,%1         \n\t"                            \
     "adcq %8,%2         \n\t"                            \
     "addq %6,%0         \n\t"                            \
     "adcq %7,%1         \n\t"                            \
     "adcq %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) : "cc");

#elif defined(TFM_SSE2)

/* SSE2 Optimized */
#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI \
   __asm__("emms");

#define SQRADD(i, j)                                      \
__asm__(                                                  \
     "movd  %6,%%mm0     \n\t"                            \
     "pmuludq %%mm0,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "adcl  %%eax,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i) :"%eax","cc");

#define SQRADD2(i, j)                                     \
__asm__(                                                  \
     "movd  %6,%%mm0     \n\t"                            \
     "movd  %7,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%%edx  \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)  :"%eax","%edx","cc");

#define SQRADDSC(i, j)                                                         \
__asm__(                                                  \
     "movd  %3,%%mm0     \n\t"                            \
     "movd  %4,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%0     \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%1     \n\t"                            \
     "xorl  %2,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "m"(i), "m"(j));

/* TAO removed sc0,1,2 as input to remove warning so %6,%7 become %3,%4 */

#define SQRADDAC(i, j)                                                         \
__asm__(                                                  \
     "movd  %6,%%mm0     \n\t"                            \
     "movd  %7,%%mm1     \n\t"                            \
     "pmuludq %%mm1,%%mm0\n\t"                            \
     "movd  %%mm0,%%eax  \n\t"                            \
     "psrlq $32,%%mm0    \n\t"                            \
     "movd  %%mm0,%%edx  \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(sc0), "=r"(sc1), "=r"(sc2): "0"(sc0), "1"(sc1), "2"(sc2), "m"(i), "m"(j)  :"%eax","%edx","cc");

#define SQRADDDB                                          \
__asm__(                                                  \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     "addl %6,%0         \n\t"                            \
     "adcl %7,%1         \n\t"                            \
     "adcl %8,%2         \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(sc0), "r"(sc1), "r"(sc2) : "cc");

#elif defined(TFM_ARM)

/* ARM code */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)                                             \
__asm__(                                                         \
"  UMULL  r0,r1,%6,%6              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(i) : "r0", "r1", "cc");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)                                            \
__asm__(                                                         \
"  UMULL  r0,r1,%6,%7              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j) : "r0", "r1", "cc");

#define SQRADDSC(i, j)                                           \
__asm__(                                                         \
"  UMULL  %0,%1,%3,%4              \n\t"                         \
"  SUB    %2,%2,%2                 \n\t"                         \
:"=r"(sc0), "=r"(sc1), "=r"(sc2) : "r"(i), "r"(j) : "cc");

/* TAO removed sc0,1,2 as input to remove warning so %6,%7 become %3,%4 */

#define SQRADDAC(i, j)                                           \
__asm__(                                                         \
"  UMULL  r0,r1,%6,%7              \n\t"                         \
"  ADDS   %0,%0,r0                 \n\t"                         \
"  ADCS   %1,%1,r1                 \n\t"                         \
"  ADC    %2,%2,#0                 \n\t"                         \
:"=r"(sc0), "=r"(sc1), "=r"(sc2) : "0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j) : "r0", "r1", "cc");

#define SQRADDDB                                                 \
__asm__(                                                         \
"  ADDS  %0,%0,%3                     \n\t"                      \
"  ADCS  %1,%1,%4                     \n\t"                      \
"  ADC   %2,%2,%5                     \n\t"                      \
"  ADDS  %0,%0,%3                     \n\t"                      \
"  ADCS  %1,%1,%4                     \n\t"                      \
"  ADC   %2,%2,%5                     \n\t"                      \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "cc");

#elif defined(TFM_PPC32)

/* PPC32 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)             \
__asm__(                         \
   " mullw  16,%6,%6       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " mulhwu 16,%6,%6       \n\t" \
   " adde   %1,%1,16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"16","cc");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
__asm__(                         \
   " mullw  16,%6,%7       \n\t" \
   " mulhwu 17,%6,%7       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " adde   %1,%1,17       \n\t" \
   " addze  %2,%2          \n\t" \
   " addc   %0,%0,16       \n\t" \
   " adde   %1,%1,17       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"16", "17","cc");

#define SQRADDSC(i, j)            \
__asm__(                          \
   " mullw  %0,%6,%7        \n\t" \
   " mulhwu %1,%6,%7        \n\t" \
   " xor    %2,%2,%2        \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "cc");

#define SQRADDAC(i, j)           \
__asm__(                         \
   " mullw  16,%6,%7       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " mulhwu 16,%6,%7       \n\t" \
   " adde   %1,%1,16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"16", "cc");

#define SQRADDDB                  \
__asm__(                          \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "cc");

#elif defined(TFM_PPC64)
/* PPC64 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)              \
__asm__(                          \
   " mulld  r16,%6,%6       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " mulhdu r16,%6,%6       \n\t" \
   " adde   %1,%1,r16       \n\t" \
   " addze  %2,%2           \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"r16","cc");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)             \
__asm__(                          \
   " mulld  r16,%6,%7       \n\t" \
   " mulhdu r17,%6,%7       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " adde   %1,%1,r17       \n\t" \
   " addze  %2,%2           \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " adde   %1,%1,r17       \n\t" \
   " addze  %2,%2           \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r16", "r17","cc");

#define SQRADDSC(i, j)            \
__asm__(                          \
   " mulld  %0,%6,%7        \n\t" \
   " mulhdu %1,%6,%7        \n\t" \
   " xor    %2,%2,%2        \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "cc");

#define SQRADDAC(i, j)            \
__asm__(                          \
   " mulld  r16,%6,%7       \n\t" \
   " addc   %0,%0,r16       \n\t" \
   " mulhdu r16,%6,%7       \n\t" \
   " adde   %1,%1,r16       \n\t" \
   " addze  %2,%2           \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"r16", "cc");

#define SQRADDDB                  \
__asm__(                          \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
   " addc   %0,%0,%3        \n\t" \
   " adde   %1,%1,%4        \n\t" \
   " adde   %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "cc");


#elif defined(TFM_AVR32)

/* AVR32 */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)             \
__asm__(                         \
   " mulu.d r2,%6,%6       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2             \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"r2","r3");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
__asm__(                         \
   " mulu.d r2,%6,%7       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2,            \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2,            \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r2", "r3");

#define SQRADDSC(i, j)            \
__asm__(                          \
   " mulu.d r2,%6,%7        \n\t" \
   " mov    %0,r2           \n\t" \
   " mov    %1,r3           \n\t" \
   " eor    %2,%2           \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "r2", "r3");

#define SQRADDAC(i, j)           \
__asm__(                         \
   " mulu.d r2,%6,%7       \n\t" \
   " add    %0,%0,r2       \n\t" \
   " adc    %1,%1,r3       \n\t" \
   " acr    %2             \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"r2", "r3");

#define SQRADDDB                  \
__asm__(                          \
   " add    %0,%0,%3        \n\t" \
   " adc    %1,%1,%4        \n\t" \
   " adc    %2,%2,%5        \n\t" \
   " add    %0,%0,%3        \n\t" \
   " adc    %1,%1,%4        \n\t" \
   " adc    %2,%2,%5        \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "cc");

#elif defined(TFM_MIPS)

/* MIPS */
#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)              \
__asm__(                          \
   " multu  %6,%6          \n\t"  \
   " mflo   $12            \n\t"  \
   " mfhi   $13            \n\t"  \
   " addu    %0,%0,$12     \n\t"  \
   " sltu   $12,%0,$12     \n\t"  \
   " addu    %1,%1,$13     \n\t"  \
   " sltu   $13,%1,$13     \n\t"  \
   " addu    %1,%1,$12     \n\t"  \
   " sltu   $12,%1,$12     \n\t"  \
   " addu    %2,%2,$13     \n\t"  \
   " addu    %2,%2,$12     \n\t"  \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i):"$12","$13");

/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)            \
__asm__(                         \
   " multu  %6,%7          \n\t" \
   " mflo   $12            \n\t" \
   " mfhi   $13            \n\t" \
                                 \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $14,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $15,%1,$13     \n\t" \
   " addu    %1,%1,$14     \n\t" \
   " sltu   $14,%1,$14     \n\t" \
   " addu    %2,%2,$15     \n\t" \
   " addu    %2,%2,$14     \n\t" \
                                 \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $14,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $15,%1,$13     \n\t" \
   " addu    %1,%1,$14     \n\t" \
   " sltu   $14,%1,$14     \n\t" \
   " addu    %2,%2,$15     \n\t" \
   " addu    %2,%2,$14     \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"$12", "$13", "$14", "$15");

#define SQRADDSC(i, j)            \
__asm__(                          \
   " multu  %6,%7          \n\t"  \
   " mflo   %0             \n\t"  \
   " mfhi   %1             \n\t"  \
   " xor    %2,%2,%2       \n\t"  \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i),"r"(j) : "cc");

#define SQRADDAC(i, j)           \
__asm__(                         \
   " multu  %6,%7          \n\t" \
   " mflo   $12            \n\t" \
   " mfhi   $13            \n\t" \
   " addu    %0,%0,$12     \n\t" \
   " sltu   $12,%0,$12     \n\t" \
   " addu    %1,%1,$13     \n\t" \
   " sltu   $13,%1,$13     \n\t" \
   " addu    %1,%1,$12     \n\t" \
   " sltu   $12,%1,$12     \n\t" \
   " addu    %2,%2,$13     \n\t" \
   " addu    %2,%2,$12     \n\t" \
:"=r"(sc0), "=r"(sc1), "=r"(sc2):"0"(sc0), "1"(sc1), "2"(sc2), "r"(i), "r"(j):"$12", "$13", "$14");

#define SQRADDDB                  \
__asm__(                          \
   " addu    %0,%0,%3       \n\t" \
   " sltu   $10,%0,%3       \n\t" \
   " addu    %1,%1,$10      \n\t" \
   " sltu   $10,%1,$10      \n\t" \
   " addu    %1,%1,%4       \n\t" \
   " sltu   $11,%1,%4       \n\t" \
   " addu    %2,%2,$10      \n\t" \
   " addu    %2,%2,$11      \n\t" \
   " addu    %2,%2,%5       \n\t" \
                                  \
   " addu    %0,%0,%3       \n\t" \
   " sltu   $10,%0,%3       \n\t" \
   " addu    %1,%1,$10      \n\t" \
   " sltu   $10,%1,$10      \n\t" \
   " addu    %1,%1,%4       \n\t" \
   " sltu   $11,%1,%4       \n\t" \
   " addu    %2,%2,$10      \n\t" \
   " addu    %2,%2,$11      \n\t" \
   " addu    %2,%2,%5       \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2) : "r"(sc0), "r"(sc1), "r"(sc2), "0"(c0), "1"(c1), "2"(c2) : "$10", "$11");

#else

#define TFM_ISO

/* ISO C portable code */

#define COMBA_START

#define CLEAR_CARRY \
   c0 = c1 = c2 = 0;

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define CARRY_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_FINI

/* multiplies point i and j, updates carry "c1" and digit c2 */
#define SQRADD(i, j)                                 \
   do { fp_word t;                                   \
   t = c0 + ((fp_word)i) * ((fp_word)j);  c0 = (fp_digit)t;    \
   t = c1 + (t >> DIGIT_BIT);             c1 = (fp_digit)t;    \
                                          c2 +=(fp_digit) (t >> DIGIT_BIT); \
   } while (0);


/* for squaring some of the terms are doubled... */
#define SQRADD2(i, j)                                                 \
   do { fp_word t;                                                    \
   t  = ((fp_word)i) * ((fp_word)j);                                  \
   tt = (fp_word)c0 + t;                 c0 = (fp_digit)tt;           \
   tt = (fp_word)c1 + (tt >> DIGIT_BIT); c1 = (fp_digit)tt;           \
                                         c2 +=(fp_digit)(tt >> DIGIT_BIT);     \
   tt = (fp_word)c0 + t;                 c0 = (fp_digit)tt;                    \
   tt = (fp_word)c1 + (tt >> DIGIT_BIT); c1 = (fp_digit)tt;            \
                                         c2 +=(fp_digit)(tt >> DIGIT_BIT);     \
   } while (0);

#define SQRADDSC(i, j)                                                         \
   do { fp_word t;                                                             \
      t =  ((fp_word)i) * ((fp_word)j);                                        \
      sc0 = (fp_digit)t; sc1 = (t >> DIGIT_BIT); sc2 = 0;                      \
   } while (0);

#define SQRADDAC(i, j)                                                         \
   do { fp_word t;                                                             \
   t = sc0 + ((fp_word)i) * ((fp_word)j);  sc0 =  (fp_digit)t;                 \
   t = sc1 + (t >> DIGIT_BIT);             sc1 =  (fp_digit)t;                 \
                                           sc2 += (fp_digit)(t >> DIGIT_BIT);  \
   } while (0);

#define SQRADDDB                                                               \
   do { fp_word t;                                                             \
   t = ((fp_word)sc0) + ((fp_word)sc0) + c0; c0 = (fp_digit)t;                 \
   t = ((fp_word)sc1) + ((fp_word)sc1) + c1 + (t >> DIGIT_BIT);                \
                                             c1 = (fp_digit)t;                 \
   c2 = c2 + (fp_digit)(((fp_word)sc2) + ((fp_word)sc2) + (t >> DIGIT_BIT));   \
   } while (0);

#endif

#ifdef TFM_SMALL_SET
    #include "fp_sqr_comba_small_set.i"
#endif

#if defined(TFM_SQR3) && FP_SIZE >= 6
    #include "fp_sqr_comba_3.i"
#endif
#if defined(TFM_SQR4) && FP_SIZE >= 8
    #include "fp_sqr_comba_4.i"
#endif
#if defined(TFM_SQR6) && FP_SIZE >= 12
    #include "fp_sqr_comba_6.i"
#endif
#if defined(TFM_SQR7) && FP_SIZE >= 14
    #include "fp_sqr_comba_7.i"
#endif
#if defined(TFM_SQR8) && FP_SIZE >= 16
    #include "fp_sqr_comba_8.i"
#endif
#if defined(TFM_SQR9) && FP_SIZE >= 18
    #include "fp_sqr_comba_9.i"
#endif
#if defined(TFM_SQR12) && FP_SIZE >= 24
    #include "fp_sqr_comba_12.i"
#endif
#if defined(TFM_SQR17) && FP_SIZE >= 34
    #include "fp_sqr_comba_17.i"
#endif
#if defined(TFM_SQR20) && FP_SIZE >= 40
    #include "fp_sqr_comba_20.i"
#endif
#if defined(TFM_SQR24) && FP_SIZE >= 48
    #include "fp_sqr_comba_24.i"
#endif
#if defined(TFM_SQR28) && FP_SIZE >= 56
    #include "fp_sqr_comba_28.i"
#endif
#if defined(TFM_SQR32) && FP_SIZE >= 64
    #include "fp_sqr_comba_32.i"
#endif
#if defined(TFM_SQR48) && FP_SIZE >= 96
    #include "fp_sqr_comba_48.i"
#endif
#if defined(TFM_SQR64) && FP_SIZE >= 128
    #include "fp_sqr_comba_64.i"
#endif
/* end fp_sqr_comba.c asm */

/* start fp_mul_comba.c asm */
/* these are the combas.  Worship them. */
#if defined(TFM_X86)
/* Generic x86 optimized code */

/* anything you need at the start */
#define COMBA_START

/* clear the chaining variables */
#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

/* forward the carry to the next digit */
#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

/* store the first sum */
#define COMBA_STORE(x) \
   x = c0;

/* store the second sum [carry] */
#define COMBA_STORE2(x) \
   x = c1;

/* anything you need at the end */
#define COMBA_FINI

/* this should multiply i and j  */
#define MULADD(i, j)                                      \
__asm__(                                                  \
     "movl  %6,%%eax     \n\t"                            \
     "mull  %7           \n\t"                            \
     "addl  %%eax,%0     \n\t"                            \
     "adcl  %%edx,%1     \n\t"                            \
     "adcl  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)  :"%eax","%edx","cc");

#elif defined(TFM_X86_64)
/* x86-64 optimized */

/* anything you need at the start */
#define COMBA_START

/* clear the chaining variables */
#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

/* forward the carry to the next digit */
#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

/* store the first sum */
#define COMBA_STORE(x) \
   x = c0;

/* store the second sum [carry] */
#define COMBA_STORE2(x) \
   x = c1;

/* anything you need at the end */
#define COMBA_FINI

/* this should multiply i and j  */
#define MULADD(i, j)                                      \
__asm__  (                                                \
     "movq  %6,%%rax     \n\t"                            \
     "mulq  %7           \n\t"                            \
     "addq  %%rax,%0     \n\t"                            \
     "adcq  %%rdx,%1     \n\t"                            \
     "adcq  $0,%2        \n\t"                            \
     :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "g"(i), "g"(j)  :"%rax","%rdx","cc");


#if defined(HAVE_INTEL_MULX)
#define MULADD_BODY(a,b,carry,c)                        \
    __asm__ volatile(                                   \
         "movq  %[a0],%%rdx\n\t"                        \
         "xorq  %%rcx, %%rcx\n\t"                       \
         "movq  0(%[cp]),%%r8\n\t"                      \
         "movq  8(%[cp]),%%r9\n\t"                      \
         "movq  16(%[cp]),%%r10\n\t"                    \
         "movq  24(%[cp]),%%r11\n\t"                    \
                                                        \
         "mulx  (%[bp]),%%rax, %%rbx\n\t"               \
         "adcxq  %[ca], %%r8\n\t"                       \
         "adoxq  %%rax, %%r8\n\t"                       \
         "mulx  8(%[bp]),%%rax, %%rcx\n\t"              \
         "adcxq  %%rbx, %%r9\n\t"                       \
         "adoxq  %%rax, %%r9\n\t"                       \
         "mulx  16(%[bp]),%%rax, %%rbx\n\t"             \
         "adcxq  %%rcx, %%r10\n\t"                      \
         "adoxq  %%rax, %%r10\n\t"                      \
         "mulx  24(%[bp]),%%rax, %%rcx\n\t"             \
         "adcxq  %%rbx, %%r11\n\t"                      \
         "mov $0, %[ca]\n\t"                            \
         "adoxq  %%rax, %%r11\n\t"                      \
         "adcxq  %%rcx, %[ca]\n\t"                      \
         "mov $0, %%rdx\n\t"                            \
         "adoxq  %%rdx, %[ca]\n\t"                      \
                                                        \
         "movq  %%r8, 0(%[cp])\n\t"                     \
         "movq  %%r9, 8(%[cp])\n\t"                     \
         "movq  %%r10, 16(%[cp])\n\t"                   \
         "movq  %%r11, 24(%[cp])\n\t"                   \
      : [ca] "+r" (carry)                               \
      : [a0] "r" (a->dp[ix]), [bp] "r" (&(b->dp[iy])),  \
        [cp] "r" (&(c->dp[iz]))                         \
      : "%r8", "%r9", "%r10", "%r11",                   \
        "%rdx", "%rax", "%rcx", "%rbx"                  \
    )

#define TFM_INTEL_MUL_COMBA(a, b, ca, c)   \
    for (iz=0; iz<pa; iz++) c->dp[iz] = 0; \
    for (ix=0; ix<a->used; ix++) {         \
        ca = 0;                            \
        for (iy=0; iy<b->used; iy+=4) {    \
            iz = ix + iy;                  \
            MULADD_BODY(a, b, ca, c);      \
        }                                  \
        c->dp[ix + iy] = ca;               \
    }
#endif

#elif defined(TFM_SSE2)
/* use SSE2 optimizations */

/* anything you need at the start */
#define COMBA_START

/* clear the chaining variables */
#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

/* forward the carry to the next digit */
#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

/* store the first sum */
#define COMBA_STORE(x) \
   x = c0;

/* store the second sum [carry] */
#define COMBA_STORE2(x) \
   x = c1;

/* anything you need at the end */
#define COMBA_FINI \
   __asm__("emms");

/* this should multiply i and j  */
#define MULADD(i, j)                                     \
__asm__(                                                 \
    "movd  %6,%%mm0     \n\t"                            \
    "movd  %7,%%mm1     \n\t"                            \
    "pmuludq %%mm1,%%mm0\n\t"                            \
    "movd  %%mm0,%%eax  \n\t"                            \
    "psrlq $32,%%mm0    \n\t"                            \
    "addl  %%eax,%0     \n\t"                            \
    "movd  %%mm0,%%eax  \n\t"                            \
    "adcl  %%eax,%1     \n\t"                            \
    "adcl  $0,%2        \n\t"                            \
    :"=r"(c0), "=r"(c1), "=r"(c2): "0"(c0), "1"(c1), "2"(c2), "m"(i), "m"(j)  :"%eax","cc");

#elif defined(TFM_ARM)
/* ARM code */

#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

#define MULADD(i, j)                                          \
__asm__(                                                      \
"  UMULL  r0,r1,%6,%7           \n\t"                         \
"  ADDS   %0,%0,r0              \n\t"                         \
"  ADCS   %1,%1,r1              \n\t"                         \
"  ADC    %2,%2,#0              \n\t"                         \
:"=r"(c0), "=r"(c1), "=r"(c2) : "0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j) : "r0", "r1", "cc");

#elif defined(TFM_PPC32)
/* For 32-bit PPC */

#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

/* untested: will mulhwu change the flags?  Docs say no */
#define MULADD(i, j)             \
__asm__(                         \
   " mullw  16,%6,%7       \n\t" \
   " addc   %0,%0,16       \n\t" \
   " mulhwu 16,%6,%7       \n\t" \
   " adde   %1,%1,16       \n\t" \
   " addze  %2,%2          \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"16");

#elif defined(TFM_PPC64)
/* For 64-bit PPC */

#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

/* untested: will mulhdu change the flags?  Docs say no */
#define MULADD(i, j)              \
____asm__(                        \
   " mulld  r16,%6,%7       \n\t" \
   " addc   %0,%0,16        \n\t" \
   " mulhdu r16,%6,%7       \n\t" \
   " adde   %1,%1,16        \n\t" \
   " addze  %2,%2           \n\t" \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r16");

#elif defined(TFM_AVR32)

/* ISO C code */

#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

#define MULADD(i, j)             \
____asm__(                       \
   " mulu.d r2,%6,%7        \n\t"\
   " add    %0,r2           \n\t"\
   " adc    %1,%1,r3        \n\t"\
   " acr    %2              \n\t"\
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"r2","r3");

#elif defined(TFM_MIPS)

/* MIPS */
#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

#define MULADD(i, j)              \
__asm__(                          \
   " multu  %6,%7          \n\t"  \
   " mflo   $12            \n\t"  \
   " mfhi   $13            \n\t"  \
   " addu    %0,%0,$12     \n\t"  \
   " sltu   $12,%0,$12     \n\t"  \
   " addu    %1,%1,$13     \n\t"  \
   " sltu   $13,%1,$13     \n\t"  \
   " addu    %1,%1,$12     \n\t"  \
   " sltu   $12,%1,$12     \n\t"  \
   " addu    %2,%2,$13     \n\t"  \
   " addu    %2,%2,$12     \n\t"  \
:"=r"(c0), "=r"(c1), "=r"(c2):"0"(c0), "1"(c1), "2"(c2), "r"(i), "r"(j):"$12","$13");

#else
/* ISO C code */

#define COMBA_START

#define COMBA_CLEAR \
   c0 = c1 = c2 = 0;

#define COMBA_FORWARD \
   do { c0 = c1; c1 = c2; c2 = 0; } while (0);

#define COMBA_STORE(x) \
   x = c0;

#define COMBA_STORE2(x) \
   x = c1;

#define COMBA_FINI

#define MULADD(i, j)                                                                                                                                  \
   do { fp_word t;                                      \
   t = (fp_word)c0 + ((fp_word)i) * ((fp_word)j);       \
   c0 = (fp_digit)t;                                    \
   t = (fp_word)c1 + (t >> DIGIT_BIT);                  \
   c1 = (fp_digit)t;                                    \
   c2 += (fp_digit)(t >> DIGIT_BIT);                    \
   } while (0);

#endif


#ifdef TFM_SMALL_SET
    #include "fp_mul_comba_small_set.i"
#endif

#if defined(TFM_MUL3) && FP_SIZE >= 6
    #include "fp_mul_comba_3.i"
#endif
#if defined(TFM_MUL4) && FP_SIZE >= 8
    #include "fp_mul_comba_4.i"
#endif
#if defined(TFM_MUL6) && FP_SIZE >= 12
    #include "fp_mul_comba_6.i"
#endif
#if defined(TFM_MUL7) && FP_SIZE >= 14
    #include "fp_mul_comba_7.i"
#endif
#if defined(TFM_MUL8) && FP_SIZE >= 16
    #include "fp_mul_comba_8.i"
#endif
#if defined(TFM_MUL9) && FP_SIZE >= 18
    #include "fp_mul_comba_9.i"
#endif
#if defined(TFM_MUL12) && FP_SIZE >= 24
    #include "fp_mul_comba_12.i"
#endif
#if defined(TFM_MUL17) && FP_SIZE >= 34
    #include "fp_mul_comba_17.i"
#endif
#if defined(TFM_MUL20) && FP_SIZE >= 40
    #include "fp_mul_comba_20.i"
#endif
#if defined(TFM_MUL24) && FP_SIZE >= 48
    #include "fp_mul_comba_24.i"
#endif
#if defined(TFM_MUL28) && FP_SIZE >= 56
    #include "fp_mul_comba_28.i"
#endif
#if defined(TFM_MUL32) && FP_SIZE >= 64
    #include "fp_mul_comba_32.i"
#endif
#if defined(TFM_MUL48) && FP_SIZE >= 96
    #include "fp_mul_comba_48.i"
#endif
#if defined(TFM_MUL64) && FP_SIZE >= 128
    #include "fp_mul_comba_64.i"
#endif

/* end fp_mul_comba.c asm */

