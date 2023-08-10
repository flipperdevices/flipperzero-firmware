/* aes_asm.S
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



/* This file is in at&t asm syntax, see .asm for intel syntax */

/* See Intel Advanced Encryption Standard (AES) Instructions Set White Paper
 * by Intel Mobility Group, Israel Development Center, Israel Shay Gueron
 */

#ifdef WOLFSSL_X86_64_BUILD

/*
AES_CBC_encrypt (const unsigned char *in,
	unsigned char *out,
	unsigned char ivec[16],
	unsigned long length,
	const unsigned char *KS,
	int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_encrypt
AES_CBC_encrypt:
#else
.globl _AES_CBC_encrypt
_AES_CBC_encrypt:
#endif
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %rcx
# parameter 5: %r8
# parameter 6: %r9d
movq	%rcx, %r10
shrq	$4, %rcx
shlq	$60, %r10
je	NO_PARTS
addq	$1, %rcx
NO_PARTS:
subq	$16, %rsi
movdqa	(%rdx), %xmm1
LOOP:
pxor	(%rdi), %xmm1
pxor	(%r8), %xmm1
addq	$16,%rsi
addq	$16,%rdi
cmpl	$12, %r9d
aesenc	16(%r8),%xmm1
aesenc	32(%r8),%xmm1
aesenc	48(%r8),%xmm1
aesenc	64(%r8),%xmm1
aesenc	80(%r8),%xmm1
aesenc	96(%r8),%xmm1
aesenc	112(%r8),%xmm1
aesenc	128(%r8),%xmm1
aesenc	144(%r8),%xmm1
movdqa	160(%r8),%xmm2
jb	LAST
cmpl	$14, %r9d

aesenc	160(%r8),%xmm1
aesenc	176(%r8),%xmm1
movdqa	192(%r8),%xmm2
jb	LAST
aesenc	192(%r8),%xmm1
aesenc	208(%r8),%xmm1
movdqa	224(%r8),%xmm2
LAST:
decq	%rcx
aesenclast %xmm2,%xmm1
movdqu	%xmm1,(%rsi)
jne	LOOP
ret


#if defined(WOLFSSL_AESNI_BY4)

/*
AES_CBC_decrypt_by4 (const unsigned char *in,
  unsigned char *out,
  unsigned char ivec[16],
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_decrypt_by4
AES_CBC_decrypt_by4:
#else
.globl _AES_CBC_decrypt_by4
_AES_CBC_decrypt_by4:
#endif
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %rcx
# parameter 5: %r8
# parameter 6: %r9d

        movq        %rcx, %r10
        shrq        $4, %rcx
        shlq        $60, %r10
        je          DNO_PARTS_4
        addq        $1, %rcx
DNO_PARTS_4:
        movq        %rcx, %r10
        shlq        $62, %r10
        shrq        $62, %r10
        shrq        $2, %rcx
        movdqu      (%rdx),%xmm5
        je          DREMAINDER_4
        subq        $64, %rsi
DLOOP_4:
        movdqu      (%rdi), %xmm1
        movdqu      16(%rdi), %xmm2
        movdqu      32(%rdi), %xmm3
        movdqu      48(%rdi), %xmm4
        movdqa      %xmm1, %xmm6
        movdqa      %xmm2, %xmm7
        movdqa      %xmm3, %xmm8
        movdqa      %xmm4, %xmm15
        movdqa      (%r8), %xmm9
        movdqa      16(%r8), %xmm10
        movdqa      32(%r8), %xmm11
        movdqa      48(%r8), %xmm12
        pxor        %xmm9, %xmm1
        pxor        %xmm9, %xmm2
        pxor        %xmm9, %xmm3
        pxor        %xmm9, %xmm4
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm12, %xmm1
        aesdec      %xmm12, %xmm2
        aesdec      %xmm12, %xmm3
        aesdec      %xmm12, %xmm4
        movdqa      64(%r8), %xmm9
        movdqa      80(%r8), %xmm10
        movdqa      96(%r8), %xmm11
        movdqa      112(%r8), %xmm12
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm12, %xmm1
        aesdec      %xmm12, %xmm2
        aesdec      %xmm12, %xmm3
        aesdec      %xmm12, %xmm4
        movdqa      128(%r8), %xmm9
        movdqa      144(%r8), %xmm10
        movdqa      160(%r8), %xmm11
        cmpl        $12, %r9d
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        jb          DLAST_4
        movdqa      160(%r8), %xmm9
        movdqa      176(%r8), %xmm10
        movdqa      192(%r8), %xmm11
        cmpl        $14, %r9d
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        jb          DLAST_4
        movdqa      192(%r8), %xmm9
        movdqa      208(%r8), %xmm10
        movdqa      224(%r8), %xmm11
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
DLAST_4:
        addq        $64, %rdi
        addq        $64, %rsi
        decq        %rcx
        aesdeclast  %xmm11, %xmm1
        aesdeclast  %xmm11, %xmm2
        aesdeclast  %xmm11, %xmm3
        aesdeclast  %xmm11, %xmm4
        pxor        %xmm5, %xmm1
        pxor        %xmm6, %xmm2
        pxor        %xmm7, %xmm3
        pxor        %xmm8, %xmm4
        movdqu      %xmm1, (%rsi)
        movdqu      %xmm2, 16(%rsi)
        movdqu      %xmm3, 32(%rsi)
        movdqu      %xmm4, 48(%rsi)
        movdqa      %xmm15,%xmm5
        jne         DLOOP_4
        addq        $64, %rsi
DREMAINDER_4:
        cmpq        $0, %r10
        je          DEND_4
DLOOP_4_2:
        movdqu      (%rdi), %xmm1
        movdqa      %xmm1, %xmm15
        addq        $16, %rdi
        pxor        (%r8), %xmm1
        movdqu      160(%r8), %xmm2
        cmpl        $12, %r9d
        aesdec      16(%r8), %xmm1
        aesdec      32(%r8), %xmm1
        aesdec      48(%r8), %xmm1
        aesdec      64(%r8), %xmm1
        aesdec      80(%r8), %xmm1
        aesdec      96(%r8), %xmm1
        aesdec      112(%r8), %xmm1
        aesdec      128(%r8), %xmm1
        aesdec      144(%r8), %xmm1
        jb          DLAST_4_2
        movdqu      192(%r8), %xmm2
        cmpl        $14, %r9d
        aesdec      160(%r8), %xmm1
        aesdec      176(%r8), %xmm1
        jb          DLAST_4_2
        movdqu      224(%r8), %xmm2
        aesdec      192(%r8), %xmm1
        aesdec      208(%r8), %xmm1
DLAST_4_2:
        aesdeclast  %xmm2, %xmm1
        pxor        %xmm5, %xmm1
        movdqa      %xmm15, %xmm5
        movdqu      %xmm1, (%rsi)
        addq        $16, %rsi
        decq        %r10
        jne         DLOOP_4_2
DEND_4:
        ret

#elif defined(WOLFSSL_AESNI_BY6)

/*
AES_CBC_decrypt_by6 (const unsigned char *in,
  unsigned char *out,
  unsigned char ivec[16],
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_decrypt_by6
AES_CBC_decrypt_by6:
#else
.globl _AES_CBC_decrypt_by6
_AES_CBC_decrypt_by6:
#endif
# parameter 1: %rdi - in
# parameter 2: %rsi - out
# parameter 3: %rdx - ivec
# parameter 4: %rcx - length
# parameter 5: %r8  - KS
# parameter 6: %r9d - nr

        movq        %rcx, %r10
        shrq        $4, %rcx
        shlq        $60, %r10
        je          DNO_PARTS_6
        addq        $1, %rcx
DNO_PARTS_6:
        movq        %rax, %r12
        movq        %rdx, %r13
        movq        %rbx, %r14
        movq        $0, %rdx
        movq        %rcx, %rax
        movq        $6, %rbx
        div         %rbx
        movq        %rax, %rcx
        movq        %rdx, %r10
        movq        %r12, %rax
        movq        %r13, %rdx
        movq        %r14, %rbx
        cmpq        $0, %rcx
        movdqu      (%rdx), %xmm7
        je          DREMAINDER_6
        subq        $96, %rsi
DLOOP_6:
        movdqu      (%rdi), %xmm1
        movdqu      16(%rdi), %xmm2
        movdqu      32(%rdi), %xmm3
        movdqu      48(%rdi), %xmm4
        movdqu      64(%rdi), %xmm5
        movdqu      80(%rdi), %xmm6
        movdqa      (%r8), %xmm8
        movdqa      16(%r8), %xmm9
        movdqa      32(%r8), %xmm10
        movdqa      48(%r8), %xmm11
        pxor        %xmm8, %xmm1
        pxor        %xmm8, %xmm2
        pxor        %xmm8, %xmm3
        pxor        %xmm8, %xmm4
        pxor        %xmm8, %xmm5
        pxor        %xmm8, %xmm6
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm9, %xmm5
        aesdec      %xmm9, %xmm6
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        movdqa      64(%r8), %xmm8
        movdqa      80(%r8), %xmm9
        movdqa      96(%r8), %xmm10
        movdqa      112(%r8), %xmm11
        aesdec      %xmm8, %xmm1
        aesdec      %xmm8, %xmm2
        aesdec      %xmm8, %xmm3
        aesdec      %xmm8, %xmm4
        aesdec      %xmm8, %xmm5
        aesdec      %xmm8, %xmm6
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm9, %xmm5
        aesdec      %xmm9, %xmm6
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        movdqa      128(%r8), %xmm8
        movdqa      144(%r8), %xmm9
        movdqa      160(%r8), %xmm10
        cmpl        $12, %r9d
        aesdec      %xmm8, %xmm1
        aesdec      %xmm8, %xmm2
        aesdec      %xmm8, %xmm3
        aesdec      %xmm8, %xmm4
        aesdec      %xmm8, %xmm5
        aesdec      %xmm8, %xmm6
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm9, %xmm5
        aesdec      %xmm9, %xmm6
        jb          DLAST_6
        movdqa      160(%r8), %xmm8
        movdqa      176(%r8), %xmm9
        movdqa      192(%r8), %xmm10
        cmpl        $14, %r9d
        aesdec      %xmm8, %xmm1
        aesdec      %xmm8, %xmm2
        aesdec      %xmm8, %xmm3
        aesdec      %xmm8, %xmm4
        aesdec      %xmm8, %xmm5
        aesdec      %xmm8, %xmm6
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm9, %xmm5
        aesdec      %xmm9, %xmm6
        jb          DLAST_6
        movdqa      192(%r8), %xmm8
        movdqa      208(%r8), %xmm9
        movdqa      224(%r8), %xmm10
        aesdec      %xmm8, %xmm1
        aesdec      %xmm8, %xmm2
        aesdec      %xmm8, %xmm3
        aesdec      %xmm8, %xmm4
        aesdec      %xmm8, %xmm5
        aesdec      %xmm8, %xmm6
        aesdec      %xmm9, %xmm1
        aesdec      %xmm9, %xmm2
        aesdec      %xmm9, %xmm3
        aesdec      %xmm9, %xmm4
        aesdec      %xmm9, %xmm5
        aesdec      %xmm9, %xmm6
DLAST_6:
        addq        $96, %rsi
        aesdeclast  %xmm10, %xmm1
        aesdeclast  %xmm10, %xmm2
        aesdeclast  %xmm10, %xmm3
        aesdeclast  %xmm10, %xmm4
        aesdeclast  %xmm10, %xmm5
        aesdeclast  %xmm10, %xmm6
        movdqu      (%rdi), %xmm8
        movdqu      16(%rdi), %xmm9
        movdqu      32(%rdi), %xmm10
        movdqu      48(%rdi), %xmm11
        movdqu      64(%rdi), %xmm12
        movdqu      80(%rdi), %xmm13
        pxor        %xmm7, %xmm1
        pxor        %xmm8, %xmm2
        pxor        %xmm9, %xmm3
        pxor        %xmm10, %xmm4
        pxor        %xmm11, %xmm5
        pxor        %xmm12, %xmm6
        movdqu      %xmm13, %xmm7
        movdqu      %xmm1, (%rsi)
        movdqu      %xmm2, 16(%rsi)
        movdqu      %xmm3, 32(%rsi)
        movdqu      %xmm4, 48(%rsi)
        movdqu      %xmm5, 64(%rsi)
        movdqu      %xmm6, 80(%rsi)
        addq        $96, %rdi
        decq        %rcx
        jne         DLOOP_6
        addq        $96, %rsi
DREMAINDER_6:
        cmpq        $0, %r10
        je          DEND_6
DLOOP_6_2:
        movdqu      (%rdi), %xmm1
        movdqa      %xmm1, %xmm10
        addq        $16, %rdi
        pxor        (%r8), %xmm1
        movdqu      160(%r8), %xmm2
        cmpl        $12, %r9d
        aesdec      16(%r8), %xmm1
        aesdec      32(%r8), %xmm1
        aesdec      48(%r8), %xmm1
        aesdec      64(%r8), %xmm1
        aesdec      80(%r8), %xmm1
        aesdec      96(%r8), %xmm1
        aesdec      112(%r8), %xmm1
        aesdec      128(%r8), %xmm1
        aesdec      144(%r8), %xmm1
        jb          DLAST_6_2
        movdqu      192(%r8), %xmm2
        cmpl        $14, %r9d
        aesdec      160(%r8), %xmm1
        aesdec      176(%r8), %xmm1
        jb          DLAST_6_2
        movdqu      224(%r8), %xmm2
        aesdec      192(%r8), %xmm1
        aesdec      208(%r8), %xmm1
DLAST_6_2:
        aesdeclast  %xmm2, %xmm1
        pxor        %xmm7, %xmm1
        movdqa      %xmm10, %xmm7
        movdqu      %xmm1, (%rsi)
        addq        $16, %rsi
        decq        %r10
        jne         DLOOP_6_2
DEND_6:
        ret

#else /* WOLFSSL_AESNI_BYx */

/*
AES_CBC_decrypt_by8 (const unsigned char *in,
  unsigned char *out,
  unsigned char ivec[16],
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_decrypt_by8
AES_CBC_decrypt_by8:
#else
.globl _AES_CBC_decrypt_by8
_AES_CBC_decrypt_by8:
#endif
# parameter 1: %rdi - in
# parameter 2: %rsi - out
# parameter 3: %rdx - ivec
# parameter 4: %rcx - length
# parameter 5: %r8  - KS
# parameter 6: %r9d - nr

        movq        %rcx, %r10
        shrq        $4, %rcx
        shlq        $60, %r10
        je          DNO_PARTS_8
        addq        $1, %rcx
DNO_PARTS_8:
        movq        %rcx, %r10
        shlq        $61, %r10
        shrq        $61, %r10
        shrq        $3, %rcx
        movdqu      (%rdx), %xmm9
        je          DREMAINDER_8
        subq        $128, %rsi
DLOOP_8:
        movdqu      (%rdi), %xmm1
        movdqu      16(%rdi), %xmm2
        movdqu      32(%rdi), %xmm3
        movdqu      48(%rdi), %xmm4
        movdqu      64(%rdi), %xmm5
        movdqu      80(%rdi), %xmm6
        movdqu      96(%rdi), %xmm7
        movdqu      112(%rdi), %xmm8
        movdqa      (%r8), %xmm10
        movdqa      16(%r8), %xmm11
        movdqa      32(%r8), %xmm12
        movdqa      48(%r8), %xmm13
        pxor        %xmm10, %xmm1
        pxor        %xmm10, %xmm2
        pxor        %xmm10, %xmm3
        pxor        %xmm10, %xmm4
        pxor        %xmm10, %xmm5
        pxor        %xmm10, %xmm6
        pxor        %xmm10, %xmm7
        pxor        %xmm10, %xmm8
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        aesdec      %xmm11, %xmm7
        aesdec      %xmm11, %xmm8
        aesdec      %xmm12, %xmm1
        aesdec      %xmm12, %xmm2
        aesdec      %xmm12, %xmm3
        aesdec      %xmm12, %xmm4
        aesdec      %xmm12, %xmm5
        aesdec      %xmm12, %xmm6
        aesdec      %xmm12, %xmm7
        aesdec      %xmm12, %xmm8
        aesdec      %xmm13, %xmm1
        aesdec      %xmm13, %xmm2
        aesdec      %xmm13, %xmm3
        aesdec      %xmm13, %xmm4
        aesdec      %xmm13, %xmm5
        aesdec      %xmm13, %xmm6
        aesdec      %xmm13, %xmm7
        aesdec      %xmm13, %xmm8
        movdqa      64(%r8), %xmm10
        movdqa      80(%r8), %xmm11
        movdqa      96(%r8), %xmm12
        movdqa      112(%r8), %xmm13
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm10, %xmm7
        aesdec      %xmm10, %xmm8
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        aesdec      %xmm11, %xmm7
        aesdec      %xmm11, %xmm8
        aesdec      %xmm12, %xmm1
        aesdec      %xmm12, %xmm2
        aesdec      %xmm12, %xmm3
        aesdec      %xmm12, %xmm4
        aesdec      %xmm12, %xmm5
        aesdec      %xmm12, %xmm6
        aesdec      %xmm12, %xmm7
        aesdec      %xmm12, %xmm8
        aesdec      %xmm13, %xmm1
        aesdec      %xmm13, %xmm2
        aesdec      %xmm13, %xmm3
        aesdec      %xmm13, %xmm4
        aesdec      %xmm13, %xmm5
        aesdec      %xmm13, %xmm6
        aesdec      %xmm13, %xmm7
        aesdec      %xmm13, %xmm8
        movdqa      128(%r8), %xmm10
        movdqa      144(%r8), %xmm11
        movdqa      160(%r8), %xmm12
        cmpl        $12, %r9d
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm10, %xmm7
        aesdec      %xmm10, %xmm8
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        aesdec      %xmm11, %xmm7
        aesdec      %xmm11, %xmm8
        jb          DLAST_8
        movdqa      160(%r8), %xmm10
        movdqa      176(%r8), %xmm11
        movdqa      192(%r8), %xmm12
        cmpl        $14, %r9d
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm10, %xmm7
        aesdec      %xmm10, %xmm8
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        aesdec      %xmm11, %xmm7
        aesdec      %xmm11, %xmm8
        jb          DLAST_8
        movdqa      192(%r8), %xmm10
        movdqa      208(%r8), %xmm11
        movdqa      224(%r8), %xmm12
        aesdec      %xmm10, %xmm1
        aesdec      %xmm10, %xmm2
        aesdec      %xmm10, %xmm3
        aesdec      %xmm10, %xmm4
        aesdec      %xmm10, %xmm5
        aesdec      %xmm10, %xmm6
        aesdec      %xmm10, %xmm7
        aesdec      %xmm10, %xmm8
        aesdec      %xmm11, %xmm1
        aesdec      %xmm11, %xmm2
        aesdec      %xmm11, %xmm3
        aesdec      %xmm11, %xmm4
        aesdec      %xmm11, %xmm5
        aesdec      %xmm11, %xmm6
        aesdec      %xmm11, %xmm7
        aesdec      %xmm11, %xmm8
DLAST_8:
        addq        $128, %rsi
        aesdeclast  %xmm12, %xmm1
        aesdeclast  %xmm12, %xmm2
        aesdeclast  %xmm12, %xmm3
        aesdeclast  %xmm12, %xmm4
        aesdeclast  %xmm12, %xmm5
        aesdeclast  %xmm12, %xmm6
        aesdeclast  %xmm12, %xmm7
        aesdeclast  %xmm12, %xmm8
        movdqu      (%rdi), %xmm10
        movdqu      16(%rdi), %xmm11
        movdqu      32(%rdi), %xmm12
        movdqu      48(%rdi), %xmm13
        pxor        %xmm9, %xmm1
        pxor        %xmm10, %xmm2
        pxor        %xmm11, %xmm3
        pxor        %xmm12, %xmm4
        pxor        %xmm13, %xmm5
        movdqu      64(%rdi), %xmm10
        movdqu      80(%rdi), %xmm11
        movdqu      96(%rdi), %xmm12
        movdqu      112(%rdi), %xmm9
        pxor        %xmm10, %xmm6
        pxor        %xmm11, %xmm7
        pxor        %xmm12, %xmm8
        movdqu      %xmm1, (%rsi)
        movdqu      %xmm2, 16(%rsi)
        movdqu      %xmm3, 32(%rsi)
        movdqu      %xmm4, 48(%rsi)
        movdqu      %xmm5, 64(%rsi)
        movdqu      %xmm6, 80(%rsi)
        movdqu      %xmm7, 96(%rsi)
        movdqu      %xmm8, 112(%rsi)
        addq        $128, %rdi
        decq        %rcx
        jne         DLOOP_8
        addq        $128, %rsi
DREMAINDER_8:
        cmpq        $0, %r10
        je          DEND_8
DLOOP_8_2:
        movdqu      (%rdi), %xmm1
        movdqa      %xmm1, %xmm10
        addq        $16, %rdi
        pxor        (%r8), %xmm1
        movdqu      160(%r8), %xmm2
        cmpl        $12, %r9d
        aesdec      16(%r8), %xmm1
        aesdec      32(%r8), %xmm1
        aesdec      48(%r8), %xmm1
        aesdec      64(%r8), %xmm1
        aesdec      80(%r8), %xmm1
        aesdec      96(%r8), %xmm1
        aesdec      112(%r8), %xmm1
        aesdec      128(%r8), %xmm1
        aesdec      144(%r8), %xmm1
        jb          DLAST_8_2
        movdqu      192(%r8), %xmm2
        cmpl        $14, %r9d
        aesdec      160(%r8), %xmm1
        aesdec      176(%r8), %xmm1
        jb          DLAST_8_2
        movdqu      224(%r8), %xmm2
        aesdec      192(%r8), %xmm1
        aesdec      208(%r8), %xmm1
DLAST_8_2:
        aesdeclast  %xmm2, %xmm1
        pxor        %xmm9, %xmm1
        movdqa      %xmm10, %xmm9
        movdqu      %xmm1, (%rsi)
        addq        $16, %rsi
        decq        %r10
        jne         DLOOP_8_2
DEND_8:
        ret

#endif /* WOLFSSL_AESNI_BYx */


/*
AES_ECB_encrypt (const unsigned char *in,
	unsigned char *out,
	unsigned long length,
	const unsigned char *KS,
	int nr)
*/
#ifndef __APPLE__
.globl AES_ECB_encrypt
AES_ECB_encrypt:
#else
.globl _AES_ECB_encrypt
_AES_ECB_encrypt:
#endif
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %rcx
# parameter 5: %r8d
        movq    %rdx, %r10
        shrq    $4, %rdx
        shlq    $60, %r10
        je      EECB_NO_PARTS_4
        addq    $1, %rdx
EECB_NO_PARTS_4:
        movq    %rdx, %r10
        shlq    $62, %r10
        shrq    $62, %r10
        shrq    $2, %rdx
        je      EECB_REMAINDER_4
        subq    $64, %rsi
EECB_LOOP_4:
        movdqu  (%rdi), %xmm1
        movdqu  16(%rdi), %xmm2
        movdqu  32(%rdi), %xmm3
        movdqu  48(%rdi), %xmm4
        movdqa  (%rcx), %xmm9
        movdqa  16(%rcx), %xmm10
        movdqa  32(%rcx), %xmm11
        movdqa  48(%rcx), %xmm12
        pxor    %xmm9, %xmm1
        pxor    %xmm9, %xmm2
        pxor    %xmm9, %xmm3
        pxor    %xmm9, %xmm4
        aesenc  %xmm10, %xmm1
        aesenc  %xmm10, %xmm2
        aesenc  %xmm10, %xmm3
        aesenc  %xmm10, %xmm4
        aesenc  %xmm11, %xmm1
        aesenc  %xmm11, %xmm2
        aesenc  %xmm11, %xmm3
        aesenc  %xmm11, %xmm4
        aesenc  %xmm12, %xmm1
        aesenc  %xmm12, %xmm2
        aesenc  %xmm12, %xmm3
        aesenc  %xmm12, %xmm4
        movdqa  64(%rcx), %xmm9
        movdqa  80(%rcx), %xmm10
        movdqa  96(%rcx), %xmm11
        movdqa  112(%rcx), %xmm12
        aesenc  %xmm9, %xmm1
        aesenc  %xmm9, %xmm2
        aesenc  %xmm9, %xmm3
        aesenc  %xmm9, %xmm4
        aesenc  %xmm10, %xmm1
        aesenc  %xmm10, %xmm2
        aesenc  %xmm10, %xmm3
        aesenc  %xmm10, %xmm4
        aesenc  %xmm11, %xmm1
        aesenc  %xmm11, %xmm2
        aesenc  %xmm11, %xmm3
        aesenc  %xmm11, %xmm4
        aesenc  %xmm12, %xmm1
        aesenc  %xmm12, %xmm2
        aesenc  %xmm12, %xmm3
        aesenc  %xmm12, %xmm4
        movdqa  128(%rcx), %xmm9
        movdqa  144(%rcx), %xmm10
        movdqa  160(%rcx), %xmm11
        cmpl    $12, %r8d
        aesenc  %xmm9, %xmm1
        aesenc  %xmm9, %xmm2
        aesenc  %xmm9, %xmm3
        aesenc  %xmm9, %xmm4
        aesenc  %xmm10, %xmm1
        aesenc  %xmm10, %xmm2
        aesenc  %xmm10, %xmm3
        aesenc  %xmm10, %xmm4
        jb      EECB_LAST_4
        movdqa  160(%rcx), %xmm9
        movdqa  176(%rcx), %xmm10
        movdqa  192(%rcx), %xmm11
        cmpl    $14, %r8d
        aesenc  %xmm9, %xmm1
        aesenc  %xmm9, %xmm2
        aesenc  %xmm9, %xmm3
        aesenc  %xmm9, %xmm4
        aesenc  %xmm10, %xmm1
        aesenc  %xmm10, %xmm2
        aesenc  %xmm10, %xmm3
        aesenc  %xmm10, %xmm4
        jb      EECB_LAST_4
        movdqa  192(%rcx), %xmm9
        movdqa  208(%rcx), %xmm10
        movdqa  224(%rcx), %xmm11
        aesenc  %xmm9, %xmm1
        aesenc  %xmm9, %xmm2
        aesenc  %xmm9, %xmm3
        aesenc  %xmm9, %xmm4
        aesenc  %xmm10, %xmm1
        aesenc  %xmm10, %xmm2
        aesenc  %xmm10, %xmm3
        aesenc  %xmm10, %xmm4
EECB_LAST_4:
        addq    $64, %rdi
        addq    $64, %rsi
        decq    %rdx
        aesenclast %xmm11, %xmm1
        aesenclast %xmm11, %xmm2
        aesenclast %xmm11, %xmm3
        aesenclast %xmm11, %xmm4
        movdqu  %xmm1, (%rsi)
        movdqu  %xmm2, 16(%rsi)
        movdqu  %xmm3, 32(%rsi)
        movdqu  %xmm4, 48(%rsi)
        jne     EECB_LOOP_4
        addq    $64, %rsi
EECB_REMAINDER_4:
        cmpq    $0, %r10
        je      EECB_END_4
EECB_LOOP_4_2:
        movdqu  (%rdi), %xmm1
        addq    $16, %rdi
        pxor    (%rcx), %xmm1
        movdqu  160(%rcx), %xmm2
        aesenc  16(%rcx), %xmm1
        aesenc  32(%rcx), %xmm1
        aesenc  48(%rcx), %xmm1
        aesenc  64(%rcx), %xmm1
        aesenc  80(%rcx), %xmm1
        aesenc  96(%rcx), %xmm1
        aesenc  112(%rcx), %xmm1
        aesenc  128(%rcx), %xmm1
        aesenc  144(%rcx), %xmm1
        cmpl    $12, %r8d
        jb      EECB_LAST_4_2
        movdqu  192(%rcx), %xmm2
        aesenc  160(%rcx), %xmm1
        aesenc  176(%rcx), %xmm1
        cmpl    $14, %r8d
        jb      EECB_LAST_4_2
        movdqu  224(%rcx), %xmm2
        aesenc  192(%rcx), %xmm1
        aesenc  208(%rcx), %xmm1
EECB_LAST_4_2:
        aesenclast %xmm2, %xmm1
        movdqu  %xmm1, (%rsi)
        addq    $16, %rsi
        decq    %r10
        jne     EECB_LOOP_4_2
EECB_END_4:
        ret


/*
AES_ECB_decrypt (const unsigned char *in,
  unsigned char *out,
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_ECB_decrypt
AES_ECB_decrypt:
#else
.globl _AES_ECB_decrypt
_AES_ECB_decrypt:
#endif
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %rcx
# parameter 5: %r8d

        movq    %rdx, %r10
        shrq    $4, %rdx
        shlq    $60, %r10
        je      DECB_NO_PARTS_4
        addq    $1, %rdx
DECB_NO_PARTS_4:
        movq    %rdx, %r10
        shlq    $62, %r10
        shrq    $62, %r10
        shrq    $2, %rdx
        je      DECB_REMAINDER_4
        subq    $64, %rsi
DECB_LOOP_4:
        movdqu  (%rdi), %xmm1
        movdqu  16(%rdi), %xmm2
        movdqu  32(%rdi), %xmm3
        movdqu  48(%rdi), %xmm4
        movdqa  (%rcx), %xmm9
        movdqa  16(%rcx), %xmm10
        movdqa  32(%rcx), %xmm11
        movdqa  48(%rcx), %xmm12
        pxor    %xmm9, %xmm1
        pxor    %xmm9, %xmm2
        pxor    %xmm9, %xmm3
        pxor    %xmm9, %xmm4
        aesdec  %xmm10, %xmm1
        aesdec  %xmm10, %xmm2
        aesdec  %xmm10, %xmm3
        aesdec  %xmm10, %xmm4
        aesdec  %xmm11, %xmm1
        aesdec  %xmm11, %xmm2
        aesdec  %xmm11, %xmm3
        aesdec  %xmm11, %xmm4
        aesdec  %xmm12, %xmm1
        aesdec  %xmm12, %xmm2
        aesdec  %xmm12, %xmm3
        aesdec  %xmm12, %xmm4
        movdqa  64(%rcx), %xmm9
        movdqa  80(%rcx), %xmm10
        movdqa  96(%rcx), %xmm11
        movdqa  112(%rcx), %xmm12
        aesdec  %xmm9, %xmm1
        aesdec  %xmm9, %xmm2
        aesdec  %xmm9, %xmm3
        aesdec  %xmm9, %xmm4
        aesdec  %xmm10, %xmm1
        aesdec  %xmm10, %xmm2
        aesdec  %xmm10, %xmm3
        aesdec  %xmm10, %xmm4
        aesdec  %xmm11, %xmm1
        aesdec  %xmm11, %xmm2
        aesdec  %xmm11, %xmm3
        aesdec  %xmm11, %xmm4
        aesdec  %xmm12, %xmm1
        aesdec  %xmm12, %xmm2
        aesdec  %xmm12, %xmm3
        aesdec  %xmm12, %xmm4
        movdqa  128(%rcx), %xmm9
        movdqa  144(%rcx), %xmm10
        movdqa  160(%rcx), %xmm11
        cmpl    $12, %r8d
        aesdec  %xmm9, %xmm1
        aesdec  %xmm9, %xmm2
        aesdec  %xmm9, %xmm3
        aesdec  %xmm9, %xmm4
        aesdec  %xmm10, %xmm1
        aesdec  %xmm10, %xmm2
        aesdec  %xmm10, %xmm3
        aesdec  %xmm10, %xmm4
        jb      DECB_LAST_4
        movdqa  160(%rcx), %xmm9
        movdqa  176(%rcx), %xmm10
        movdqa  192(%rcx), %xmm11
        cmpl    $14, %r8d
        aesdec  %xmm9, %xmm1
        aesdec  %xmm9, %xmm2
        aesdec  %xmm9, %xmm3
        aesdec  %xmm9, %xmm4
        aesdec  %xmm10, %xmm1
        aesdec  %xmm10, %xmm2
        aesdec  %xmm10, %xmm3
        aesdec  %xmm10, %xmm4
        jb      DECB_LAST_4
        movdqa  192(%rcx), %xmm9
        movdqa  208(%rcx), %xmm10
        movdqa  224(%rcx), %xmm11
        aesdec  %xmm9, %xmm1
        aesdec  %xmm9, %xmm2
        aesdec  %xmm9, %xmm3
        aesdec  %xmm9, %xmm4
        aesdec  %xmm10, %xmm1
        aesdec  %xmm10, %xmm2
        aesdec  %xmm10, %xmm3
        aesdec  %xmm10, %xmm4
DECB_LAST_4:
        addq    $64, %rdi
        addq    $64, %rsi
        decq    %rdx
        aesdeclast %xmm11, %xmm1
        aesdeclast %xmm11, %xmm2
        aesdeclast %xmm11, %xmm3
        aesdeclast %xmm11, %xmm4
        movdqu  %xmm1, (%rsi)
        movdqu  %xmm2, 16(%rsi)
        movdqu  %xmm3, 32(%rsi)
        movdqu  %xmm4, 48(%rsi)
        jne     DECB_LOOP_4
        addq    $64, %rsi
DECB_REMAINDER_4:
        cmpq    $0, %r10
        je      DECB_END_4
DECB_LOOP_4_2:
        movdqu  (%rdi), %xmm1
        addq    $16, %rdi
        pxor    (%rcx), %xmm1
        movdqu  160(%rcx), %xmm2
        cmpl    $12, %r8d
        aesdec  16(%rcx), %xmm1
        aesdec  32(%rcx), %xmm1
        aesdec  48(%rcx), %xmm1
        aesdec  64(%rcx), %xmm1
        aesdec  80(%rcx), %xmm1
        aesdec  96(%rcx), %xmm1
        aesdec  112(%rcx), %xmm1
        aesdec  128(%rcx), %xmm1
        aesdec  144(%rcx), %xmm1
        jb      DECB_LAST_4_2
        cmpl    $14, %r8d
        movdqu  192(%rcx), %xmm2
        aesdec  160(%rcx), %xmm1
        aesdec  176(%rcx), %xmm1
        jb      DECB_LAST_4_2
        movdqu  224(%rcx), %xmm2
        aesdec  192(%rcx), %xmm1
        aesdec  208(%rcx), %xmm1
DECB_LAST_4_2:
        aesdeclast %xmm2, %xmm1
        movdqu  %xmm1, (%rsi)
        addq    $16, %rsi
        decq    %r10
        jne     DECB_LOOP_4_2
DECB_END_4:
        ret




/*
void AES_128_Key_Expansion(const unsigned char* userkey,
   unsigned char* key_schedule);
*/
.align  16,0x90
#ifndef __APPLE__
.globl AES_128_Key_Expansion
AES_128_Key_Expansion:
#else
.globl _AES_128_Key_Expansion
_AES_128_Key_Expansion:
#endif
# parameter 1: %rdi
# parameter 2: %rsi
movl    $10, 240(%rsi)

movdqu  (%rdi), %xmm1
movdqa    %xmm1, (%rsi)


ASSISTS:
aeskeygenassist $1, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 16(%rsi)
aeskeygenassist $2, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 32(%rsi)
aeskeygenassist $4, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 48(%rsi)
aeskeygenassist $8, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 64(%rsi)
aeskeygenassist $16, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 80(%rsi)
aeskeygenassist $32, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 96(%rsi)
aeskeygenassist $64, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 112(%rsi)
aeskeygenassist $0x80, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 128(%rsi)
aeskeygenassist $0x1b, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 144(%rsi)
aeskeygenassist $0x36, %xmm1, %xmm2
call PREPARE_ROUNDKEY_128
movdqa %xmm1, 160(%rsi)
ret

PREPARE_ROUNDKEY_128:
pshufd $255, %xmm2, %xmm2
movdqa %xmm1, %xmm3
pslldq $4, %xmm3
pxor %xmm3, %xmm1
pslldq $4, %xmm3
pxor %xmm3, %xmm1
pslldq $4, %xmm3
pxor %xmm3, %xmm1
pxor %xmm2, %xmm1
ret


/*
void AES_192_Key_Expansion (const unsigned char *userkey,
  unsigned char *key)
*/
#ifndef __APPLE__
.globl AES_192_Key_Expansion
AES_192_Key_Expansion:
#else
.globl _AES_192_Key_Expansion
_AES_192_Key_Expansion:
#endif
# parameter 1: %rdi
# parameter 2: %rsi

movdqu (%rdi), %xmm1
movq 16(%rdi), %xmm3
movdqa %xmm1, (%rsi)
movdqa %xmm3, %xmm5

aeskeygenassist $0x1, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
shufpd $0, %xmm1, %xmm5
movdqa %xmm5, 16(%rsi)
movdqa %xmm1, %xmm6
shufpd $1, %xmm3, %xmm6
movdqa %xmm6, 32(%rsi)

aeskeygenassist $0x2, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
movdqa %xmm1, 48(%rsi)
movdqa %xmm3, %xmm5

aeskeygenassist $0x4, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
shufpd $0, %xmm1, %xmm5
movdqa %xmm5, 64(%rsi)
movdqa %xmm1, %xmm6
shufpd $1, %xmm3, %xmm6
movdqa %xmm6, 80(%rsi)

aeskeygenassist $0x8, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
movdqa %xmm1, 96(%rsi)
movdqa %xmm3, %xmm5

aeskeygenassist $0x10, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
shufpd $0, %xmm1, %xmm5
movdqa %xmm5, 112(%rsi)
movdqa %xmm1, %xmm6
shufpd $1, %xmm3, %xmm6
movdqa %xmm6, 128(%rsi)

aeskeygenassist $0x20, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
movdqa %xmm1, 144(%rsi)
movdqa %xmm3, %xmm5

aeskeygenassist $0x40, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
shufpd $0, %xmm1, %xmm5
movdqa %xmm5, 160(%rsi)
movdqa %xmm1, %xmm6
shufpd $1, %xmm3, %xmm6
movdqa %xmm6, 176(%rsi)

aeskeygenassist $0x80, %xmm3, %xmm2
call PREPARE_ROUNDKEY_192
movdqa %xmm1, 192(%rsi)
movdqa %xmm3, 208(%rsi)
ret

PREPARE_ROUNDKEY_192:
pshufd $0x55, %xmm2, %xmm2
movdqu %xmm1, %xmm4
pslldq $4, %xmm4
pxor   %xmm4, %xmm1

pslldq $4, %xmm4
pxor   %xmm4, %xmm1
pslldq $4, %xmm4
pxor  %xmm4, %xmm1
pxor   %xmm2, %xmm1
pshufd $0xff, %xmm1, %xmm2
movdqu %xmm3, %xmm4
pslldq $4, %xmm4
pxor   %xmm4, %xmm3
pxor   %xmm2, %xmm3
ret


/*
void AES_256_Key_Expansion (const unsigned char *userkey,
  unsigned char *key)
*/
#ifndef __APPLE__
.globl AES_256_Key_Expansion
AES_256_Key_Expansion:
#else
.globl _AES_256_Key_Expansion
_AES_256_Key_Expansion:
#endif
# parameter 1: %rdi
# parameter 2: %rsi

movdqu (%rdi), %xmm1
movdqu 16(%rdi), %xmm3
movdqa %xmm1, (%rsi)
movdqa %xmm3, 16(%rsi)

aeskeygenassist $0x1, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 32(%rsi)
aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 48(%rsi)
aeskeygenassist $0x2, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 64(%rsi)
aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 80(%rsi)
aeskeygenassist $0x4, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 96(%rsi)
aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 112(%rsi)
aeskeygenassist $0x8, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 128(%rsi)
aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 144(%rsi)
aeskeygenassist $0x10, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 160(%rsi)
aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 176(%rsi)
aeskeygenassist $0x20, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 192(%rsi)

aeskeygenassist $0x0, %xmm1, %xmm2
call MAKE_RK256_b
movdqa %xmm3, 208(%rsi)
aeskeygenassist $0x40, %xmm3, %xmm2
call MAKE_RK256_a
movdqa %xmm1, 224(%rsi)

ret

MAKE_RK256_a:
pshufd $0xff, %xmm2, %xmm2
movdqa %xmm1, %xmm4
pslldq $4, %xmm4
pxor   %xmm4, %xmm1
pslldq $4, %xmm4
pxor  %xmm4, %xmm1
pslldq $4, %xmm4
pxor  %xmm4, %xmm1
pxor   %xmm2, %xmm1
ret

MAKE_RK256_b:
pshufd $0xaa, %xmm2, %xmm2
movdqa %xmm3, %xmm4
pslldq $4, %xmm4
pxor   %xmm4, %xmm3
pslldq $4, %xmm4
pxor  %xmm4, %xmm3
pslldq $4, %xmm4
pxor  %xmm4, %xmm3
pxor   %xmm2, %xmm3
ret

#elif defined WOLFSSL_X86_BUILD

/*
AES_CBC_encrypt (const unsigned char *in,
	unsigned char *out,
	unsigned char ivec[16],
	unsigned long length,
	const unsigned char *KS,
	int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_encrypt
AES_CBC_encrypt:
#else
.globl _AES_CBC_encrypt
_AES_CBC_encrypt:
#endif
        # parameter 1: stack[4] => %edi
        # parameter 2: stack[8] => %esi
        # parameter 3: stack[12] => %edx
        # parameter 4: stack[16] => %ecx
        # parameter 5: stack[20] => %eax
        # parameter 6: stack[24] => %ebx
        push	%edi
        push	%esi
        push	%ebx
        push	%ebp
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        movl	32(%esp), %ecx
        movl	36(%esp), %eax
        movl	40(%esp), %ebx

        movl	%ecx, %ebp
        shrl	$4, %ecx
        shll	$60, %ebp
        je	NO_PARTS
        addl	$1, %ecx
        NO_PARTS:
        subl	$16, %esi
        movdqa	(%edx), %xmm1
        LOOP:
        pxor	(%edi), %xmm1
        pxor	(%eax), %xmm1
        addl	$16,%esi
        addl	$16,%edi
        cmpl	$12, %ebx
        aesenc	16(%eax),%xmm1
        aesenc	32(%eax),%xmm1
        aesenc	48(%eax),%xmm1
        aesenc	64(%eax),%xmm1
        aesenc	80(%eax),%xmm1
        aesenc	96(%eax),%xmm1
        aesenc	112(%eax),%xmm1
        aesenc	128(%eax),%xmm1
        aesenc	144(%eax),%xmm1
        movdqa	160(%eax),%xmm2
        jb	LAST
        cmpl	$14, %ebx

        aesenc	160(%eax),%xmm1
        aesenc	176(%eax),%xmm1
        movdqa	192(%eax),%xmm2
        jb	LAST
        aesenc	192(%eax),%xmm1
        aesenc	208(%eax),%xmm1
        movdqa	224(%eax),%xmm2
        LAST:
        decl	%ecx
        aesenclast %xmm2,%xmm1
        movdqu	%xmm1,(%esi)
        jne	LOOP

        pop	%ebp
        pop	%ebx
        pop	%esi
        pop	%edi
        ret


/*
AES_CBC_decrypt_by4 (const unsigned char *in,
  unsigned char *out,
  unsigned char ivec[16],
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_CBC_decrypt_by4
AES_CBC_decrypt_by4:
#else
.globl _AES_CBC_decrypt_by4
_AES_CBC_decrypt_by4:
#endif
# parameter 1: stack[4] => %edi
# parameter 2: stack[8] => %esi
# parameter 3: stack[12] => %edx
# parameter 4: stack[16] => %ecx
# parameter 5: stack[20] => %eax
# parameter 6: stack[24] => %ebx
        push	%edi
        push	%esi
        push	%ebx
        push	%ebp
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        movl	32(%esp), %ecx
        movl	36(%esp), %eax
        movl	40(%esp), %ebx
        subl	$16, %esp

        movdqu      (%edx), %xmm0
        movl        %ecx, %ebp
        shrl        $4, %ecx
        shll        $60, %ebp
        movdqu      %xmm0, (%esp)
        je          DNO_PARTS_4
        addl        $1, %ecx
DNO_PARTS_4:
        movl        %ecx, %ebp
        shll        $62, %ebp
        shrl        $62, %ebp
        shrl        $2, %ecx
        je          DREMAINDER_4
        subl        $64, %esi
DLOOP_4:
        movdqu      (%edi), %xmm1
        movdqu      16(%edi), %xmm2
        movdqu      32(%edi), %xmm3
        movdqu      48(%edi), %xmm4
        movdqa      (%eax), %xmm5
        movdqa      16(%eax), %xmm6
        movdqa      32(%eax), %xmm7
        movdqa      48(%eax), %xmm0
        pxor        %xmm5, %xmm1
        pxor        %xmm5, %xmm2
        pxor        %xmm5, %xmm3
        pxor        %xmm5, %xmm4
        aesdec      %xmm6, %xmm1
        aesdec      %xmm6, %xmm2
        aesdec      %xmm6, %xmm3
        aesdec      %xmm6, %xmm4
        aesdec      %xmm7, %xmm1
        aesdec      %xmm7, %xmm2
        aesdec      %xmm7, %xmm3
        aesdec      %xmm7, %xmm4
        aesdec      %xmm0, %xmm1
        aesdec      %xmm0, %xmm2
        aesdec      %xmm0, %xmm3
        aesdec      %xmm0, %xmm4
        movdqa      64(%eax), %xmm5
        movdqa      80(%eax), %xmm6
        movdqa      96(%eax), %xmm7
        movdqa      112(%eax), %xmm0
        aesdec      %xmm5, %xmm1
        aesdec      %xmm5, %xmm2
        aesdec      %xmm5, %xmm3
        aesdec      %xmm5, %xmm4
        aesdec      %xmm6, %xmm1
        aesdec      %xmm6, %xmm2
        aesdec      %xmm6, %xmm3
        aesdec      %xmm6, %xmm4
        aesdec      %xmm7, %xmm1
        aesdec      %xmm7, %xmm2
        aesdec      %xmm7, %xmm3
        aesdec      %xmm7, %xmm4
        aesdec      %xmm0, %xmm1
        aesdec      %xmm0, %xmm2
        aesdec      %xmm0, %xmm3
        aesdec      %xmm0, %xmm4
        movdqa      128(%eax), %xmm5
        movdqa      144(%eax), %xmm6
        movdqa      160(%eax), %xmm7
        cmpl        $12, %ebx
        aesdec      %xmm5, %xmm1
        aesdec      %xmm5, %xmm2
        aesdec      %xmm5, %xmm3
        aesdec      %xmm5, %xmm4
        aesdec      %xmm6, %xmm1
        aesdec      %xmm6, %xmm2
        aesdec      %xmm6, %xmm3
        aesdec      %xmm6, %xmm4
        jb          DLAST_4
        movdqa      160(%eax), %xmm5
        movdqa      176(%eax), %xmm6
        movdqa      192(%eax), %xmm7
        cmpl        $14, %ebx
        aesdec      %xmm5, %xmm1
        aesdec      %xmm5, %xmm2
        aesdec      %xmm5, %xmm3
        aesdec      %xmm5, %xmm4
        aesdec      %xmm6, %xmm1
        aesdec      %xmm6, %xmm2
        aesdec      %xmm6, %xmm3
        aesdec      %xmm6, %xmm4
        jb          DLAST_4
        movdqa      192(%eax), %xmm5
        movdqa      208(%eax), %xmm6
        movdqa      224(%eax), %xmm7
        aesdec      %xmm5, %xmm1
        aesdec      %xmm5, %xmm2
        aesdec      %xmm5, %xmm3
        aesdec      %xmm5, %xmm4
        aesdec      %xmm6, %xmm1
        aesdec      %xmm6, %xmm2
        aesdec      %xmm6, %xmm3
        aesdec      %xmm6, %xmm4
DLAST_4:
        addl        $64, %esi
        aesdeclast  %xmm7, %xmm1
        aesdeclast  %xmm7, %xmm2
        aesdeclast  %xmm7, %xmm3
        aesdeclast  %xmm7, %xmm4
        movdqu      (%esp), %xmm0
        movdqu      (%edi), %xmm5
        movdqu      16(%edi), %xmm6
        movdqu      32(%edi), %xmm7
        pxor        %xmm0, %xmm1
        pxor        %xmm5, %xmm2
        pxor        %xmm6, %xmm3
        pxor        %xmm7, %xmm4
        movdqu      48(%edi), %xmm0
        movdqu      %xmm1, (%esi)
        movdqu      %xmm2, 16(%esi)
        movdqu      %xmm3, 32(%esi)
        movdqu      %xmm4, 48(%esi)
        movdqu      %xmm0, (%esp)
        addl        $64, %edi
        decl        %ecx
        jne         DLOOP_4
        addl        $64, %esi
DREMAINDER_4:
        cmpl        $0, %ebp
        je          DEND_4
DLOOP_4_2:
        movdqu      (%edi), %xmm1
        movdqa      %xmm1, %xmm5
        addl        $16, %edi
        pxor        (%eax), %xmm1
        movdqu      160(%eax), %xmm2
        cmpl        $12, %ebx
        aesdec      16(%eax), %xmm1
        aesdec      32(%eax), %xmm1
        aesdec      48(%eax), %xmm1
        aesdec      64(%eax), %xmm1
        aesdec      80(%eax), %xmm1
        aesdec      96(%eax), %xmm1
        aesdec      112(%eax), %xmm1
        aesdec      128(%eax), %xmm1
        aesdec      144(%eax), %xmm1
        jb          DLAST_4_2
        movdqu      192(%eax), %xmm2
        cmpl        $14, %ebx
        aesdec      160(%eax), %xmm1
        aesdec      176(%eax), %xmm1
        jb          DLAST_4_2
        movdqu      224(%eax), %xmm2
        aesdec      192(%eax), %xmm1
        aesdec      208(%eax), %xmm1
DLAST_4_2:
        aesdeclast  %xmm2, %xmm1
        pxor        %xmm0, %xmm1
        movdqa      %xmm5, %xmm0
        movdqu      %xmm1, (%esi)
        addl        $16, %esi
        decl        %ebp
        jne         DLOOP_4_2
DEND_4:

        addl	$16, %esp
        pop	%ebp
        pop	%ebx
        pop	%esi
        pop	%edi
        ret

/*
AES_ECB_encrypt (const unsigned char *in,
	unsigned char *out,
	unsigned long length,
	const unsigned char *KS,
	int nr)
*/
#ifndef __APPLE__
.globl AES_ECB_encrypt
AES_ECB_encrypt:
#else
.globl _AES_ECB_encrypt
_AES_ECB_encrypt:
#endif
# parameter 1: stack[4] => %edi
# parameter 2: stack[8] => %esi
# parameter 3: stack[12] => %edx
# parameter 4: stack[16] => %ecx
# parameter 5: stack[20] => %eax
        push	%edi
        push	%esi
        push	%ebx
        movl	16(%esp), %edi
        movl	20(%esp), %esi
        movl	24(%esp), %edx
        movl	28(%esp), %ecx
        movl	32(%esp), %eax

        movl    %edx, %ebx
        shrl    $4, %edx
        shll    $60, %ebx
        je      EECB_NO_PARTS_4
        addl    $1, %edx
EECB_NO_PARTS_4:
        movl    %edx, %ebx
        shll    $62, %ebx
        shrl    $62, %ebx
        shrl    $2, %edx
        je      EECB_REMAINDER_4
        subl    $64, %esi
EECB_LOOP_4:
        movdqu  (%edi), %xmm1
        movdqu  16(%edi), %xmm2
        movdqu  32(%edi), %xmm3
        movdqu  48(%edi), %xmm4
        movdqa  (%ecx), %xmm5
        movdqa  16(%ecx), %xmm6
        movdqa  32(%ecx), %xmm7
        movdqa  48(%ecx), %xmm0
        pxor    %xmm5, %xmm1
        pxor    %xmm5, %xmm2
        pxor    %xmm5, %xmm3
        pxor    %xmm5, %xmm4
        aesenc  %xmm6, %xmm1
        aesenc  %xmm6, %xmm2
        aesenc  %xmm6, %xmm3
        aesenc  %xmm6, %xmm4
        aesenc  %xmm7, %xmm1
        aesenc  %xmm7, %xmm2
        aesenc  %xmm7, %xmm3
        aesenc  %xmm7, %xmm4
        aesenc  %xmm0, %xmm1
        aesenc  %xmm0, %xmm2
        aesenc  %xmm0, %xmm3
        aesenc  %xmm0, %xmm4
        movdqa  64(%ecx), %xmm5
        movdqa  80(%ecx), %xmm6
        movdqa  96(%ecx), %xmm7
        movdqa  112(%ecx), %xmm0
        aesenc  %xmm5, %xmm1
        aesenc  %xmm5, %xmm2
        aesenc  %xmm5, %xmm3
        aesenc  %xmm5, %xmm4
        aesenc  %xmm6, %xmm1
        aesenc  %xmm6, %xmm2
        aesenc  %xmm6, %xmm3
        aesenc  %xmm6, %xmm4
        aesenc  %xmm7, %xmm1
        aesenc  %xmm7, %xmm2
        aesenc  %xmm7, %xmm3
        aesenc  %xmm7, %xmm4
        aesenc  %xmm0, %xmm1
        aesenc  %xmm0, %xmm2
        aesenc  %xmm0, %xmm3
        aesenc  %xmm0, %xmm4
        movdqa  128(%ecx), %xmm5
        movdqa  144(%ecx), %xmm6
        movdqa  160(%ecx), %xmm7
        cmpl    $12, %eax
        aesenc  %xmm5, %xmm1
        aesenc  %xmm5, %xmm2
        aesenc  %xmm5, %xmm3
        aesenc  %xmm5, %xmm4
        aesenc  %xmm6, %xmm1
        aesenc  %xmm6, %xmm2
        aesenc  %xmm6, %xmm3
        aesenc  %xmm6, %xmm4
        jb      EECB_LAST_4
        movdqa  160(%ecx), %xmm5
        movdqa  176(%ecx), %xmm6
        movdqa  192(%ecx), %xmm7
        cmpl    $14, %eax
        aesenc  %xmm5, %xmm1
        aesenc  %xmm5, %xmm2
        aesenc  %xmm5, %xmm3
        aesenc  %xmm5, %xmm4
        aesenc  %xmm6, %xmm1
        aesenc  %xmm6, %xmm2
        aesenc  %xmm6, %xmm3
        aesenc  %xmm6, %xmm4
        jb      EECB_LAST_4
        movdqa  192(%ecx), %xmm5
        movdqa  208(%ecx), %xmm6
        movdqa  224(%ecx), %xmm7
        aesenc  %xmm5, %xmm1
        aesenc  %xmm5, %xmm2
        aesenc  %xmm5, %xmm3
        aesenc  %xmm5, %xmm4
        aesenc  %xmm6, %xmm1
        aesenc  %xmm6, %xmm2
        aesenc  %xmm6, %xmm3
        aesenc  %xmm6, %xmm4
EECB_LAST_4:
        addl    $64, %edi
        addl    $64, %esi
        decl    %edx
        aesenclast %xmm7, %xmm1
        aesenclast %xmm7, %xmm2
        aesenclast %xmm7, %xmm3
        aesenclast %xmm7, %xmm4
        movdqu  %xmm1, (%esi)
        movdqu  %xmm2, 16(%esi)
        movdqu  %xmm3, 32(%esi)
        movdqu  %xmm4, 48(%esi)
        jne     EECB_LOOP_4
        addl    $64, %esi
EECB_REMAINDER_4:
        cmpl    $0, %ebx
        je      EECB_END_4
EECB_LOOP_4_2:
        movdqu  (%edi), %xmm1
        addl    $16, %edi
        pxor    (%ecx), %xmm1
        movdqu  160(%ecx), %xmm2
        aesenc  16(%ecx), %xmm1
        aesenc  32(%ecx), %xmm1
        aesenc  48(%ecx), %xmm1
        aesenc  64(%ecx), %xmm1
        aesenc  80(%ecx), %xmm1
        aesenc  96(%ecx), %xmm1
        aesenc  112(%ecx), %xmm1
        aesenc  128(%ecx), %xmm1
        aesenc  144(%ecx), %xmm1
        cmpl    $12, %eax
        jb      EECB_LAST_4_2
        movdqu  192(%ecx), %xmm2
        aesenc  160(%ecx), %xmm1
        aesenc  176(%ecx), %xmm1
        cmpl    $14, %eax
        jb      EECB_LAST_4_2
        movdqu  224(%ecx), %xmm2
        aesenc  192(%ecx), %xmm1
        aesenc  208(%ecx), %xmm1
EECB_LAST_4_2:
        aesenclast %xmm2, %xmm1
        movdqu  %xmm1, (%esi)
        addl    $16, %esi
        decl    %ebx
        jne     EECB_LOOP_4_2
EECB_END_4:

        pop	%ebx
        pop	%esi
        pop	%edi
        ret


/*
AES_ECB_decrypt (const unsigned char *in,
  unsigned char *out,
  unsigned long length,
  const unsigned char *KS,
  int nr)
*/
#ifndef __APPLE__
.globl AES_ECB_decrypt
AES_ECB_decrypt:
#else
.globl _AES_ECB_decrypt
_AES_ECB_decrypt:
#endif
# parameter 1: stack[4] => %edi
# parameter 2: stack[8] => %esi
# parameter 3: stack[12] => %edx
# parameter 4: stack[16] => %ecx
# parameter 5: stack[20] => %eax
        push	%edi
        push	%esi
        push	%ebx
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        movl	32(%esp), %ecx
        movl	36(%esp), %eax


        movl    %edx, %ebx
        shrl    $4, %edx
        shll    $60, %ebx
        je      DECB_NO_PARTS_4
        addl    $1, %edx
DECB_NO_PARTS_4:
        movl    %edx, %ebx
        shll    $62, %ebx
        shrl    $62, %ebx
        shrl    $2, %edx
        je      DECB_REMAINDER_4
        subl    $64, %esi
DECB_LOOP_4:
        movdqu  (%edi), %xmm1
        movdqu  16(%edi), %xmm2
        movdqu  32(%edi), %xmm3
        movdqu  48(%edi), %xmm4
        movdqa  (%ecx), %xmm5
        movdqa  16(%ecx), %xmm6
        movdqa  32(%ecx), %xmm7
        movdqa  48(%ecx), %xmm0
        pxor    %xmm5, %xmm1
        pxor    %xmm5, %xmm2
        pxor    %xmm5, %xmm3
        pxor    %xmm5, %xmm4
        aesdec  %xmm6, %xmm1
        aesdec  %xmm6, %xmm2
        aesdec  %xmm6, %xmm3
        aesdec  %xmm6, %xmm4
        aesdec  %xmm7, %xmm1
        aesdec  %xmm7, %xmm2
        aesdec  %xmm7, %xmm3
        aesdec  %xmm7, %xmm4
        aesdec  %xmm0, %xmm1
        aesdec  %xmm0, %xmm2
        aesdec  %xmm0, %xmm3
        aesdec  %xmm0, %xmm4
        movdqa  64(%ecx), %xmm5
        movdqa  80(%ecx), %xmm6
        movdqa  96(%ecx), %xmm7
        movdqa  112(%ecx), %xmm0
        aesdec  %xmm5, %xmm1
        aesdec  %xmm5, %xmm2
        aesdec  %xmm5, %xmm3
        aesdec  %xmm5, %xmm4
        aesdec  %xmm6, %xmm1
        aesdec  %xmm6, %xmm2
        aesdec  %xmm6, %xmm3
        aesdec  %xmm6, %xmm4
        aesdec  %xmm7, %xmm1
        aesdec  %xmm7, %xmm2
        aesdec  %xmm7, %xmm3
        aesdec  %xmm7, %xmm4
        aesdec  %xmm0, %xmm1
        aesdec  %xmm0, %xmm2
        aesdec  %xmm0, %xmm3
        aesdec  %xmm0, %xmm4
        movdqa  128(%ecx), %xmm5
        movdqa  144(%ecx), %xmm6
        movdqa  160(%ecx), %xmm7
        cmpl    $12, %eax
        aesdec  %xmm5, %xmm1
        aesdec  %xmm5, %xmm2
        aesdec  %xmm5, %xmm3
        aesdec  %xmm5, %xmm4
        aesdec  %xmm6, %xmm1
        aesdec  %xmm6, %xmm2
        aesdec  %xmm6, %xmm3
        aesdec  %xmm6, %xmm4
        jb      DECB_LAST_4
        movdqa  160(%ecx), %xmm5
        movdqa  176(%ecx), %xmm6
        movdqa  192(%ecx), %xmm7
        cmpl    $14, %eax
        aesdec  %xmm5, %xmm1
        aesdec  %xmm5, %xmm2
        aesdec  %xmm5, %xmm3
        aesdec  %xmm5, %xmm4
        aesdec  %xmm6, %xmm1
        aesdec  %xmm6, %xmm2
        aesdec  %xmm6, %xmm3
        aesdec  %xmm6, %xmm4
        jb      DECB_LAST_4
        movdqa  192(%ecx), %xmm5
        movdqa  208(%ecx), %xmm6
        movdqa  224(%ecx), %xmm7
        aesdec  %xmm5, %xmm1
        aesdec  %xmm5, %xmm2
        aesdec  %xmm5, %xmm3
        aesdec  %xmm5, %xmm4
        aesdec  %xmm6, %xmm1
        aesdec  %xmm6, %xmm2
        aesdec  %xmm6, %xmm3
        aesdec  %xmm6, %xmm4
DECB_LAST_4:
        addl    $64, %edi
        addl    $64, %esi
        decl    %edx
        aesdeclast %xmm7, %xmm1
        aesdeclast %xmm7, %xmm2
        aesdeclast %xmm7, %xmm3
        aesdeclast %xmm7, %xmm4
        movdqu  %xmm1, (%esi)
        movdqu  %xmm2, 16(%esi)
        movdqu  %xmm3, 32(%esi)
        movdqu  %xmm4, 48(%esi)
        jne     DECB_LOOP_4
        addl    $64, %esi
DECB_REMAINDER_4:
        cmpl    $0, %ebx
        je      DECB_END_4
DECB_LOOP_4_2:
        movdqu  (%edi), %xmm1
        addl    $16, %edi
        pxor    (%ecx), %xmm1
        movdqu  160(%ecx), %xmm2
        cmpl    $12, %eax
        aesdec  16(%ecx), %xmm1
        aesdec  32(%ecx), %xmm1
        aesdec  48(%ecx), %xmm1
        aesdec  64(%ecx), %xmm1
        aesdec  80(%ecx), %xmm1
        aesdec  96(%ecx), %xmm1
        aesdec  112(%ecx), %xmm1
        aesdec  128(%ecx), %xmm1
        aesdec  144(%ecx), %xmm1
        jb      DECB_LAST_4_2
        cmpl    $14, %eax
        movdqu  192(%ecx), %xmm2
        aesdec  160(%ecx), %xmm1
        aesdec  176(%ecx), %xmm1
        jb      DECB_LAST_4_2
        movdqu  224(%ecx), %xmm2
        aesdec  192(%ecx), %xmm1
        aesdec  208(%ecx), %xmm1
DECB_LAST_4_2:
        aesdeclast %xmm2, %xmm1
        movdqu  %xmm1, (%esi)
        addl    $16, %esi
        decl    %ebx
        jne     DECB_LOOP_4_2
DECB_END_4:
        pop	%ebx
        pop	%esi
        pop	%edi
        ret



/*
void AES_128_Key_Expansion(const unsigned char* userkey,
   unsigned char* key_schedule);
*/
.align  16,0x90
#ifndef __APPLE__
.globl AES_128_Key_Expansion
AES_128_Key_Expansion:
#else
.globl _AES_128_Key_Expansion
_AES_128_Key_Expansion:
#endif
        # parameter 1: stack[4] => %eax
        # parameter 2: stack[8] => %edx
        movl	4(%esp), %eax
        movl	8(%esp), %edx

        movl    $10, 240(%edx)

        movdqu  (%eax), %xmm1
        movdqa    %xmm1, (%edx)


ASSISTS:
        aeskeygenassist $1, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 16(%edx)
        aeskeygenassist $2, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 32(%edx)
        aeskeygenassist $4, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 48(%edx)
        aeskeygenassist $8, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 64(%edx)
        aeskeygenassist $16, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 80(%edx)
        aeskeygenassist $32, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 96(%edx)
        aeskeygenassist $64, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 112(%edx)
        aeskeygenassist $0x80, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 128(%edx)
        aeskeygenassist $0x1b, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 144(%edx)
        aeskeygenassist $0x36, %xmm1, %xmm2
        call PREPARE_ROUNDKEY_128
        movdqa %xmm1, 160(%edx)
        ret

PREPARE_ROUNDKEY_128:
        pshufd $255, %xmm2, %xmm2
        movdqa %xmm1, %xmm3
        pslldq $4, %xmm3
        pxor %xmm3, %xmm1
        pslldq $4, %xmm3
        pxor %xmm3, %xmm1
        pslldq $4, %xmm3
        pxor %xmm3, %xmm1
        pxor %xmm2, %xmm1
        ret


/*
void AES_192_Key_Expansion (const unsigned char *userkey,
  unsigned char *key)
*/
#ifndef __APPLE__
.globl AES_192_Key_Expansion
AES_192_Key_Expansion:
#else
.globl _AES_192_Key_Expansion
_AES_192_Key_Expansion:
#endif
        # parameter 1: stack[4] => %eax
        # parameter 2: stack[8] => %edx
        movl	4(%esp), %eax
        movl	8(%esp), %edx

        movdqu (%eax), %xmm1
        movq 16(%eax), %xmm3
        movdqa %xmm1, (%edx)
        movdqa %xmm3, %xmm5

        aeskeygenassist $0x1, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        shufpd $0, %xmm1, %xmm5
        movdqa %xmm5, 16(%edx)
        movdqa %xmm1, %xmm6
        shufpd $1, %xmm3, %xmm6
        movdqa %xmm6, 32(%edx)

        aeskeygenassist $0x2, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        movdqa %xmm1, 48(%edx)
        movdqa %xmm3, %xmm5

        aeskeygenassist $0x4, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        shufpd $0, %xmm1, %xmm5
        movdqa %xmm5, 64(%edx)
        movdqa %xmm1, %xmm6
        shufpd $1, %xmm3, %xmm6
        movdqa %xmm6, 80(%edx)

        aeskeygenassist $0x8, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        movdqa %xmm1, 96(%edx)
        movdqa %xmm3, %xmm5

        aeskeygenassist $0x10, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        shufpd $0, %xmm1, %xmm5
        movdqa %xmm5, 112(%edx)
        movdqa %xmm1, %xmm6
        shufpd $1, %xmm3, %xmm6
        movdqa %xmm6, 128(%edx)

        aeskeygenassist $0x20, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        movdqa %xmm1, 144(%edx)
        movdqa %xmm3, %xmm5

        aeskeygenassist $0x40, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        shufpd $0, %xmm1, %xmm5
        movdqa %xmm5, 160(%edx)
        movdqa %xmm1, %xmm6
        shufpd $1, %xmm3, %xmm6
        movdqa %xmm6, 176(%edx)

        aeskeygenassist $0x80, %xmm3, %xmm2
        call PREPARE_ROUNDKEY_192
        movdqa %xmm1, 192(%edx)
        movdqa %xmm3, 208(%edx)
        ret

PREPARE_ROUNDKEY_192:
        pshufd $0x55, %xmm2, %xmm2
        movdqu %xmm1, %xmm4
        pslldq $4, %xmm4
        pxor   %xmm4, %xmm1

        pslldq $4, %xmm4
        pxor   %xmm4, %xmm1
        pslldq $4, %xmm4
        pxor  %xmm4, %xmm1
        pxor   %xmm2, %xmm1
        pshufd $0xff, %xmm1, %xmm2
        movdqu %xmm3, %xmm4
        pslldq $4, %xmm4
        pxor   %xmm4, %xmm3
        pxor   %xmm2, %xmm3
        ret


/*
void AES_256_Key_Expansion (const unsigned char *userkey,
  unsigned char *key)
*/
#ifndef __APPLE__
.globl AES_256_Key_Expansion
AES_256_Key_Expansion:
#else
.globl _AES_256_Key_Expansion
_AES_256_Key_Expansion:
#endif
        # parameter 1: stack[4] => %eax
        # parameter 2: stack[8] => %edx
        movl	4(%esp), %eax
        movl	8(%esp), %edx

        movdqu (%eax), %xmm1
        movdqu 16(%eax), %xmm3
        movdqa %xmm1, (%edx)
        movdqa %xmm3, 16(%edx)

        aeskeygenassist $0x1, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 32(%edx)
        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 48(%edx)
        aeskeygenassist $0x2, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 64(%edx)
        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 80(%edx)
        aeskeygenassist $0x4, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 96(%edx)
        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 112(%edx)
        aeskeygenassist $0x8, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 128(%edx)
        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 144(%edx)
        aeskeygenassist $0x10, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 160(%edx)
        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 176(%edx)
        aeskeygenassist $0x20, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 192(%edx)

        aeskeygenassist $0x0, %xmm1, %xmm2
        call MAKE_RK256_b
        movdqa %xmm3, 208(%edx)
        aeskeygenassist $0x40, %xmm3, %xmm2
        call MAKE_RK256_a
        movdqa %xmm1, 224(%edx)

        ret

MAKE_RK256_a:
        pshufd $0xff, %xmm2, %xmm2
        movdqa %xmm1, %xmm4
        pslldq $4, %xmm4
        pxor   %xmm4, %xmm1
        pslldq $4, %xmm4
        pxor  %xmm4, %xmm1
        pslldq $4, %xmm4
        pxor  %xmm4, %xmm1
        pxor   %xmm2, %xmm1
        ret

MAKE_RK256_b:
        pshufd $0xaa, %xmm2, %xmm2
        movdqa %xmm3, %xmm4
        pslldq $4, %xmm4
        pxor   %xmm4, %xmm3
        pslldq $4, %xmm4
        pxor  %xmm4, %xmm3
        pslldq $4, %xmm4
        pxor  %xmm4, %xmm3
        pxor   %xmm2, %xmm3
        ret

#endif /* WOLFSSL_X86_64_BUILD */

#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",%progbits
#endif

