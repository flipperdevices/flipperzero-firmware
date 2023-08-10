/* aes_gcm_x86_asm
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

#ifdef WOLFSSL_USER_SETTINGS
#include "wolfssl/wolfcrypt/settings.h"
#endif

#ifndef HAVE_INTEL_AVX1
#define HAVE_INTEL_AVX1
#endif /* HAVE_INTEL_AVX1 */
#ifndef NO_AVX2_SUPPORT
#define HAVE_INTEL_AVX2
#endif /* NO_AVX2_SUPPORT */

.type	data, @object
L_aes_gcm_one:
.long	0x0,0x0,0x1,0x0
.type	data, @object
L_aes_gcm_two:
.long	0x0,0x0,0x2,0x0
.type	data, @object
L_aes_gcm_three:
.long	0x0,0x0,0x3,0x0
.type	data, @object
L_aes_gcm_four:
.long	0x0,0x0,0x4,0x0
.type	data, @object
L_aes_gcm_bswap_epi64:
.long	0x4050607,0x10203,0xc0d0e0f,0x8090a0b
.type	data, @object
L_aes_gcm_bswap_mask:
.long	0xc0d0e0f,0x8090a0b,0x4050607,0x10203
.type	data, @object
L_aes_gcm_mod2_128:
.long	0x1,0x0,0x0,0xc2000000
.type	data, @object
L_aes_gcm_avx1_one:
.long	0x0,0x0,0x1,0x0
.type	data, @object
L_aes_gcm_avx1_two:
.long	0x0,0x0,0x2,0x0
.type	data, @object
L_aes_gcm_avx1_three:
.long	0x0,0x0,0x3,0x0
.type	data, @object
L_aes_gcm_avx1_four:
.long	0x0,0x0,0x4,0x0
.type	data, @object
L_aes_gcm_avx1_bswap_epi64:
.long	0x4050607,0x10203,0xc0d0e0f,0x8090a0b
.type	data, @object
L_aes_gcm_avx1_bswap_mask:
.long	0xc0d0e0f,0x8090a0b,0x4050607,0x10203
.type	data, @object
L_aes_gcm_avx1_mod2_128:
.long	0x1,0x0,0x0,0xc2000000
.type	data, @object
L_aes_gcm_avx2_one:
.long	0x0,0x0,0x1,0x0
.type	data, @object
L_aes_gcm_avx2_two:
.long	0x0,0x0,0x2,0x0
.type	data, @object
L_aes_gcm_avx2_three:
.long	0x0,0x0,0x3,0x0
.type	data, @object
L_aes_gcm_avx2_four:
.long	0x0,0x0,0x4,0x0
.type	data, @object
L_avx2_aes_gcm_bswap_one:
.long	0x0,0x0,0x0,0x1000000
.type	data, @object
L_aes_gcm_avx2_bswap_epi64:
.long	0x4050607,0x10203,0xc0d0e0f,0x8090a0b
.type	data, @object
L_aes_gcm_avx2_bswap_mask:
.long	0xc0d0e0f,0x8090a0b,0x4050607,0x10203
.type	data, @object
L_aes_gcm_avx2_mod2_128:
.long	0x1,0x0,0x0,0xc2000000
.text
.globl	AES_GCM_encrypt
.type	AES_GCM_encrypt,@function
.align	16
AES_GCM_encrypt:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x70, %esp
        movl	144(%esp), %esi
        movl	168(%esp), %ebp
        movl	160(%esp), %edx
        pxor	%xmm0, %xmm0
        pxor	%xmm2, %xmm2
        cmpl	$12, %edx
        jne	L_AES_GCM_encrypt_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        pinsrd	$0x00, (%esi), %xmm0
        pinsrd	$0x01, 4(%esi), %xmm0
        pinsrd	$2, 8(%esi), %xmm0
        pinsrd	$3, %ecx, %xmm0
        # H = Encrypt X(=0) and T = Encrypt counter
        movdqa	%xmm0, %xmm5
        movdqa	(%ebp), %xmm1
        pxor	%xmm1, %xmm5
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_calc_iv_12_last
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_calc_iv_12_last
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_calc_iv_12_last:
        aesenclast	%xmm3, %xmm1
        aesenclast	%xmm3, %xmm5
        pshufb	L_aes_gcm_bswap_mask, %xmm1
        movdqu	%xmm5, 80(%esp)
        jmp	L_AES_GCM_encrypt_iv_done
L_AES_GCM_encrypt_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        movdqa	(%ebp), %xmm1
        aesenc	16(%ebp), %xmm1
        aesenc	32(%ebp), %xmm1
        aesenc	48(%ebp), %xmm1
        aesenc	64(%ebp), %xmm1
        aesenc	80(%ebp), %xmm1
        aesenc	96(%ebp), %xmm1
        aesenc	112(%ebp), %xmm1
        aesenc	128(%ebp), %xmm1
        aesenc	144(%ebp), %xmm1
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last
        aesenc	%xmm5, %xmm1
        aesenc	176(%ebp), %xmm1
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last
        aesenc	%xmm5, %xmm1
        aesenc	208(%ebp), %xmm1
        movdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last:
        aesenclast	%xmm5, %xmm1
        pshufb	L_aes_gcm_bswap_mask, %xmm1
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_encrypt_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_calc_iv_16_loop:
        movdqu	(%esi,%ecx,1), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_calc_iv_16_loop
        movl	160(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_calc_iv_done
L_AES_GCM_encrypt_calc_iv_lt16:
        subl	$16, %esp
        pxor	%xmm4, %xmm4
        xorl	%ebx, %ebx
        movdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_calc_iv_loop
        movdqu	(%esp), %xmm4
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
L_AES_GCM_encrypt_calc_iv_done:
        # T = Encrypt counter
        pxor	%xmm4, %xmm4
        shll	$3, %edx
        pinsrd	$0x00, %edx, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        #   Encrypt counter
        movdqa	(%ebp), %xmm4
        pxor	%xmm0, %xmm4
        aesenc	16(%ebp), %xmm4
        aesenc	32(%ebp), %xmm4
        aesenc	48(%ebp), %xmm4
        aesenc	64(%ebp), %xmm4
        aesenc	80(%ebp), %xmm4
        aesenc	96(%ebp), %xmm4
        aesenc	112(%ebp), %xmm4
        aesenc	128(%ebp), %xmm4
        aesenc	144(%ebp), %xmm4
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	176(%ebp), %xmm4
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	208(%ebp), %xmm4
        movdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last:
        aesenclast	%xmm5, %xmm4
        movdqu	%xmm4, 80(%esp)
L_AES_GCM_encrypt_iv_done:
        movl	140(%esp), %esi
        # Additional authentication data
        movl	156(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_encrypt_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_calc_aad_16_loop:
        movdqu	(%esi,%ecx,1), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm2, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm2, %xmm7
        pclmulqdq	$0x00, %xmm2, %xmm4
        pxor	%xmm2, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm2, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm2
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm2
        por	%xmm4, %xmm3
        por	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm2
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_calc_aad_16_loop
        movl	156(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_calc_aad_done
L_AES_GCM_encrypt_calc_aad_lt16:
        subl	$16, %esp
        pxor	%xmm4, %xmm4
        xorl	%ebx, %ebx
        movdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_calc_aad_loop
        movdqu	(%esp), %xmm4
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm2, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm2, %xmm7
        pclmulqdq	$0x00, %xmm2, %xmm4
        pxor	%xmm2, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm2, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm2
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm2
        por	%xmm4, %xmm3
        por	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm2
L_AES_GCM_encrypt_calc_aad_done:
        movdqu	%xmm2, 96(%esp)
        movl	132(%esp), %esi
        movl	136(%esp), %edi
        # Calculate counter and H
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        movdqa	%xmm1, %xmm5
        paddd	L_aes_gcm_one, %xmm0
        movdqa	%xmm1, %xmm4
        movdqu	%xmm0, 64(%esp)
        psrlq	$63, %xmm5
        psllq	$0x01, %xmm4
        pslldq	$8, %xmm5
        por	%xmm5, %xmm4
        pshufd	$0xff, %xmm1, %xmm1
        psrad	$31, %xmm1
        pand	L_aes_gcm_mod2_128, %xmm1
        pxor	%xmm4, %xmm1
        xorl	%ebx, %ebx
        movl	152(%esp), %eax
        cmpl	$0x40, %eax
        jl	L_AES_GCM_encrypt_done_64
        andl	$0xffffffc0, %eax
        movdqa	%xmm2, %xmm6
        # H ^ 1
        movdqu	%xmm1, (%esp)
        # H ^ 2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm0
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm0
        movdqu	%xmm0, 16(%esp)
        # H ^ 3
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm0, %xmm6
        movdqa	%xmm0, %xmm7
        movdqa	%xmm0, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm0, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm3
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm3
        movdqu	%xmm3, 32(%esp)
        # H ^ 4
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm0, %xmm6
        movdqa	%xmm0, %xmm7
        movdqa	%xmm0, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm0, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm3
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm3
        movdqu	%xmm3, 48(%esp)
        # First 64 bytes of input
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm4
        movdqa	L_aes_gcm_bswap_epi64, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pshufb	%xmm3, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pshufb	%xmm3, %xmm5
        paddd	L_aes_gcm_two, %xmm6
        pshufb	%xmm3, %xmm6
        paddd	L_aes_gcm_three, %xmm7
        pshufb	%xmm3, %xmm7
        movdqu	64(%esp), %xmm3
        paddd	L_aes_gcm_four, %xmm3
        movdqu	%xmm3, 64(%esp)
        movdqa	(%ebp), %xmm3
        pxor	%xmm3, %xmm4
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm3, %xmm7
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_enc_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_enc_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_enc_done:
        aesenclast	%xmm3, %xmm4
        aesenclast	%xmm3, %xmm5
        movdqu	(%esi), %xmm0
        movdqu	16(%esi), %xmm1
        pxor	%xmm0, %xmm4
        pxor	%xmm1, %xmm5
        movdqu	%xmm4, (%edi)
        movdqu	%xmm5, 16(%edi)
        aesenclast	%xmm3, %xmm6
        aesenclast	%xmm3, %xmm7
        movdqu	32(%esi), %xmm0
        movdqu	48(%esi), %xmm1
        pxor	%xmm0, %xmm6
        pxor	%xmm1, %xmm7
        movdqu	%xmm6, 32(%edi)
        movdqu	%xmm7, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        movl	%esi, %ecx
        movl	%edi, %edx
        jle	L_AES_GCM_encrypt_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm4
        movdqa	L_aes_gcm_bswap_epi64, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pshufb	%xmm3, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pshufb	%xmm3, %xmm5
        paddd	L_aes_gcm_two, %xmm6
        pshufb	%xmm3, %xmm6
        paddd	L_aes_gcm_three, %xmm7
        pshufb	%xmm3, %xmm7
        movdqu	64(%esp), %xmm3
        paddd	L_aes_gcm_four, %xmm3
        movdqu	%xmm3, 64(%esp)
        movdqa	(%ebp), %xmm3
        pxor	%xmm3, %xmm4
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm3, %xmm7
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_aesenc_64_ghash_avx_done:
        aesenclast	%xmm3, %xmm4
        aesenclast	%xmm3, %xmm5
        movdqu	(%ecx), %xmm0
        movdqu	16(%ecx), %xmm1
        pxor	%xmm0, %xmm4
        pxor	%xmm1, %xmm5
        movdqu	%xmm4, (%edx)
        movdqu	%xmm5, 16(%edx)
        aesenclast	%xmm3, %xmm6
        aesenclast	%xmm3, %xmm7
        movdqu	32(%ecx), %xmm0
        movdqu	48(%ecx), %xmm1
        pxor	%xmm0, %xmm6
        pxor	%xmm1, %xmm7
        movdqu	%xmm6, 32(%edx)
        movdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        movdqu	96(%esp), %xmm6
        movdqu	48(%esp), %xmm3
        movdqu	-64(%edx), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm6, %xmm4
        pshufd	$0x4e, %xmm3, %xmm5
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm3, %xmm5
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm7
        pclmulqdq	$0x11, %xmm3, %xmm7
        movdqa	%xmm4, %xmm6
        pclmulqdq	$0x00, %xmm3, %xmm6
        pclmulqdq	$0x00, %xmm1, %xmm5
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqu	32(%esp), %xmm3
        movdqu	-48(%edx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	16(%esp), %xmm3
        movdqu	-32(%edx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	(%esp), %xmm3
        movdqu	-16(%edx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqa	%xmm5, %xmm1
        psrldq	$8, %xmm5
        pslldq	$8, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm5, %xmm7
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        movdqa	%xmm6, %xmm1
        pslld	$31, %xmm3
        pslld	$30, %xmm0
        pslld	$25, %xmm1
        pxor	%xmm0, %xmm3
        pxor	%xmm1, %xmm3
        movdqa	%xmm3, %xmm0
        pslldq	$12, %xmm3
        psrldq	$4, %xmm0
        pxor	%xmm3, %xmm6
        movdqa	%xmm6, %xmm1
        movdqa	%xmm6, %xmm5
        movdqa	%xmm6, %xmm4
        psrld	$0x01, %xmm1
        psrld	$2, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm7, %xmm6
        movdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_ghash_64
L_AES_GCM_encrypt_end_64:
        movdqu	96(%esp), %xmm2
        # Block 1
        movdqa	L_aes_gcm_bswap_mask, %xmm4
        movdqu	(%edx), %xmm1
        pshufb	%xmm4, %xmm1
        movdqu	48(%esp), %xmm3
        pxor	%xmm2, %xmm1
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm3, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm0
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm0
        pxor	%xmm5, %xmm2
        # Block 2
        movdqa	L_aes_gcm_bswap_mask, %xmm4
        movdqu	16(%edx), %xmm1
        pshufb	%xmm4, %xmm1
        movdqu	32(%esp), %xmm3
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm3, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        pxor	%xmm4, %xmm0
        pxor	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm0
        pxor	%xmm5, %xmm2
        # Block 3
        movdqa	L_aes_gcm_bswap_mask, %xmm4
        movdqu	32(%edx), %xmm1
        pshufb	%xmm4, %xmm1
        movdqu	16(%esp), %xmm3
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm3, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        pxor	%xmm4, %xmm0
        pxor	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm0
        pxor	%xmm5, %xmm2
        # Block 4
        movdqa	L_aes_gcm_bswap_mask, %xmm4
        movdqu	48(%edx), %xmm1
        pshufb	%xmm4, %xmm1
        movdqu	(%esp), %xmm3
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm3, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        pxor	%xmm4, %xmm0
        pxor	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm0
        pxor	%xmm5, %xmm2
        movdqa	%xmm0, %xmm4
        movdqa	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm0
        movdqa	%xmm0, %xmm6
        movdqa	%xmm0, %xmm7
        movdqa	%xmm0, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm0, %xmm6
        pxor	%xmm6, %xmm2
        movdqu	(%esp), %xmm1
L_AES_GCM_encrypt_done_64:
        movl	152(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_encrypt_done_enc
        movl	152(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	64(%esp), %xmm4
        movdqa	%xmm4, %xmm5
        pshufb	L_aes_gcm_bswap_epi64, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pxor	(%ebp), %xmm4
        movdqu	%xmm5, 64(%esp)
        aesenc	16(%ebp), %xmm4
        aesenc	32(%ebp), %xmm4
        aesenc	48(%ebp), %xmm4
        aesenc	64(%ebp), %xmm4
        aesenc	80(%ebp), %xmm4
        aesenc	96(%ebp), %xmm4
        aesenc	112(%ebp), %xmm4
        aesenc	128(%ebp), %xmm4
        aesenc	144(%ebp), %xmm4
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	176(%ebp), %xmm4
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	208(%ebp), %xmm4
        movdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last:
        aesenclast	%xmm5, %xmm4
        movdqu	(%ecx), %xmm5
        pxor	%xmm5, %xmm4
        movdqu	%xmm4, (%edx)
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_last_block_ghash
L_AES_GCM_encrypt_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	64(%esp), %xmm4
        movdqa	%xmm4, %xmm5
        pshufb	L_aes_gcm_bswap_epi64, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pxor	(%ebp), %xmm4
        movdqu	%xmm5, 64(%esp)
        movdqu	%xmm2, %xmm0
        pclmulqdq	$16, %xmm1, %xmm0
        aesenc	16(%ebp), %xmm4
        aesenc	32(%ebp), %xmm4
        movdqu	%xmm2, %xmm3
        pclmulqdq	$0x01, %xmm1, %xmm3
        aesenc	48(%ebp), %xmm4
        aesenc	64(%ebp), %xmm4
        aesenc	80(%ebp), %xmm4
        movdqu	%xmm2, %xmm5
        pclmulqdq	$0x11, %xmm1, %xmm5
        aesenc	96(%ebp), %xmm4
        pxor	%xmm3, %xmm0
        movdqa	%xmm0, %xmm6
        psrldq	$8, %xmm0
        pslldq	$8, %xmm6
        aesenc	112(%ebp), %xmm4
        movdqu	%xmm2, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm3
        pxor	%xmm3, %xmm6
        pxor	%xmm0, %xmm5
        movdqa	L_aes_gcm_mod2_128, %xmm7
        movdqa	%xmm6, %xmm3
        pclmulqdq	$16, %xmm7, %xmm3
        aesenc	128(%ebp), %xmm4
        pshufd	$0x4e, %xmm6, %xmm0
        pxor	%xmm3, %xmm0
        movdqa	%xmm0, %xmm3
        pclmulqdq	$16, %xmm7, %xmm3
        aesenc	144(%ebp), %xmm4
        pshufd	$0x4e, %xmm0, %xmm2
        pxor	%xmm3, %xmm2
        pxor	%xmm5, %xmm2
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_gfmul_last
        aesenc	%xmm5, %xmm4
        aesenc	176(%ebp), %xmm4
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_gfmul_last
        aesenc	%xmm5, %xmm4
        aesenc	208(%ebp), %xmm4
        movdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_aesenc_gfmul_last:
        aesenclast	%xmm5, %xmm4
        movdqu	(%ecx), %xmm5
        pxor	%xmm5, %xmm4
        movdqu	%xmm4, (%edx)
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_last_block_start
L_AES_GCM_encrypt_last_block_ghash:
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm2, %xmm6
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm2, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm2
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
L_AES_GCM_encrypt_last_block_done:
        movl	152(%esp), %ecx
        movl	%ecx, %edx
        andl	$15, %ecx
        jz	L_AES_GCM_encrypt_aesenc_last15_enc_avx_done
        movdqu	64(%esp), %xmm0
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        pxor	(%ebp), %xmm0
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        aesenc	96(%ebp), %xmm0
        aesenc	112(%ebp), %xmm0
        aesenc	128(%ebp), %xmm0
        aesenc	144(%ebp), %xmm0
        cmpl	$11, 172(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last
        aesenc	%xmm5, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 172(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last
        aesenc	%xmm5, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last:
        aesenclast	%xmm5, %xmm0
        subl	$16, %esp
        xorl	%ecx, %ecx
        movdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_aesenc_last15_enc_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        xorb	(%esp,%ecx,1), %al
        movb	%al, (%edi,%ebx,1)
        movb	%al, (%esp,%ecx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_loop
        xorl	%eax, %eax
        cmpl	$16, %ecx
        je	L_AES_GCM_encrypt_aesenc_last15_enc_avx_finish_enc
L_AES_GCM_encrypt_aesenc_last15_enc_avx_byte_loop:
        movb	%al, (%esp,%ecx,1)
        incl	%ecx
        cmpl	$16, %ecx
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_byte_loop
L_AES_GCM_encrypt_aesenc_last15_enc_avx_finish_enc:
        movdqu	(%esp), %xmm0
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm2, %xmm6
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm2, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm2
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
L_AES_GCM_encrypt_aesenc_last15_enc_avx_done:
L_AES_GCM_encrypt_done_enc:
        movl	148(%esp), %edi
        movl	164(%esp), %ebx
        movl	152(%esp), %edx
        movl	156(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        pinsrd	$0x00, %edx, %xmm4
        pinsrd	$2, %ecx, %xmm4
        movl	152(%esp), %edx
        movl	156(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        pinsrd	$0x01, %edx, %xmm4
        pinsrd	$3, %ecx, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm2, %xmm6
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm2, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm2
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
        pshufb	L_aes_gcm_bswap_mask, %xmm2
        movdqu	80(%esp), %xmm4
        pxor	%xmm2, %xmm4
        cmpl	$16, %ebx
        je	L_AES_GCM_encrypt_store_tag_16
        xorl	%ecx, %ecx
        movdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	%ebx, %ecx
        jne	L_AES_GCM_encrypt_store_tag_loop
        jmp	L_AES_GCM_encrypt_store_tag_done
L_AES_GCM_encrypt_store_tag_16:
        movdqu	%xmm4, (%edi)
L_AES_GCM_encrypt_store_tag_done:
        addl	$0x70, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt,.-AES_GCM_encrypt
.text
.globl	AES_GCM_decrypt
.type	AES_GCM_decrypt,@function
.align	16
AES_GCM_decrypt:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xb0, %esp
        movl	208(%esp), %esi
        movl	232(%esp), %ebp
        movl	224(%esp), %edx
        pxor	%xmm0, %xmm0
        pxor	%xmm2, %xmm2
        cmpl	$12, %edx
        jne	L_AES_GCM_decrypt_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        pinsrd	$0x00, (%esi), %xmm0
        pinsrd	$0x01, 4(%esi), %xmm0
        pinsrd	$2, 8(%esi), %xmm0
        pinsrd	$3, %ecx, %xmm0
        # H = Encrypt X(=0) and T = Encrypt counter
        movdqa	%xmm0, %xmm5
        movdqa	(%ebp), %xmm1
        pxor	%xmm1, %xmm5
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_calc_iv_12_last
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_calc_iv_12_last
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm1
        aesenc	%xmm3, %xmm5
        movdqa	224(%ebp), %xmm3
L_AES_GCM_decrypt_calc_iv_12_last:
        aesenclast	%xmm3, %xmm1
        aesenclast	%xmm3, %xmm5
        pshufb	L_aes_gcm_bswap_mask, %xmm1
        movdqu	%xmm5, 80(%esp)
        jmp	L_AES_GCM_decrypt_iv_done
L_AES_GCM_decrypt_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        movdqa	(%ebp), %xmm1
        aesenc	16(%ebp), %xmm1
        aesenc	32(%ebp), %xmm1
        aesenc	48(%ebp), %xmm1
        aesenc	64(%ebp), %xmm1
        aesenc	80(%ebp), %xmm1
        aesenc	96(%ebp), %xmm1
        aesenc	112(%ebp), %xmm1
        aesenc	128(%ebp), %xmm1
        aesenc	144(%ebp), %xmm1
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last
        aesenc	%xmm5, %xmm1
        aesenc	176(%ebp), %xmm1
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last
        aesenc	%xmm5, %xmm1
        aesenc	208(%ebp), %xmm1
        movdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last:
        aesenclast	%xmm5, %xmm1
        pshufb	L_aes_gcm_bswap_mask, %xmm1
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_decrypt_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_calc_iv_16_loop:
        movdqu	(%esi,%ecx,1), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_calc_iv_16_loop
        movl	224(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_calc_iv_done
L_AES_GCM_decrypt_calc_iv_lt16:
        subl	$16, %esp
        pxor	%xmm4, %xmm4
        xorl	%ebx, %ebx
        movdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_calc_iv_loop
        movdqu	(%esp), %xmm4
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
L_AES_GCM_decrypt_calc_iv_done:
        # T = Encrypt counter
        pxor	%xmm4, %xmm4
        shll	$3, %edx
        pinsrd	$0x00, %edx, %xmm4
        pxor	%xmm4, %xmm0
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm0, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm0
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm0
        por	%xmm4, %xmm3
        por	%xmm5, %xmm0
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        #   Encrypt counter
        movdqa	(%ebp), %xmm4
        pxor	%xmm0, %xmm4
        aesenc	16(%ebp), %xmm4
        aesenc	32(%ebp), %xmm4
        aesenc	48(%ebp), %xmm4
        aesenc	64(%ebp), %xmm4
        aesenc	80(%ebp), %xmm4
        aesenc	96(%ebp), %xmm4
        aesenc	112(%ebp), %xmm4
        aesenc	128(%ebp), %xmm4
        aesenc	144(%ebp), %xmm4
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	176(%ebp), %xmm4
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last
        aesenc	%xmm5, %xmm4
        aesenc	208(%ebp), %xmm4
        movdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last:
        aesenclast	%xmm5, %xmm4
        movdqu	%xmm4, 80(%esp)
L_AES_GCM_decrypt_iv_done:
        movl	204(%esp), %esi
        # Additional authentication data
        movl	220(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_decrypt_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_calc_aad_16_loop:
        movdqu	(%esi,%ecx,1), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm2, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm2, %xmm7
        pclmulqdq	$0x00, %xmm2, %xmm4
        pxor	%xmm2, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm2, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm2
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm2
        por	%xmm4, %xmm3
        por	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm2
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_calc_aad_16_loop
        movl	220(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_calc_aad_done
L_AES_GCM_decrypt_calc_aad_lt16:
        subl	$16, %esp
        pxor	%xmm4, %xmm4
        xorl	%ebx, %ebx
        movdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_calc_aad_loop
        movdqu	(%esp), %xmm4
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm2, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm2, %xmm7
        pclmulqdq	$0x00, %xmm2, %xmm4
        pxor	%xmm2, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm4, %xmm3
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm3
        pxor	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm2, %xmm5
        psrld	$31, %xmm4
        psrld	$31, %xmm5
        pslld	$0x01, %xmm3
        pslld	$0x01, %xmm2
        movdqa	%xmm4, %xmm6
        pslldq	$4, %xmm4
        psrldq	$12, %xmm6
        pslldq	$4, %xmm5
        por	%xmm6, %xmm2
        por	%xmm4, %xmm3
        por	%xmm5, %xmm2
        movdqa	%xmm3, %xmm4
        movdqa	%xmm3, %xmm5
        movdqa	%xmm3, %xmm6
        pslld	$31, %xmm4
        pslld	$30, %xmm5
        pslld	$25, %xmm6
        pxor	%xmm5, %xmm4
        pxor	%xmm6, %xmm4
        movdqa	%xmm4, %xmm5
        psrldq	$4, %xmm5
        pslldq	$12, %xmm4
        pxor	%xmm4, %xmm3
        movdqa	%xmm3, %xmm6
        movdqa	%xmm3, %xmm7
        movdqa	%xmm3, %xmm4
        psrld	$0x01, %xmm6
        psrld	$2, %xmm7
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm6
        pxor	%xmm4, %xmm6
        pxor	%xmm5, %xmm6
        pxor	%xmm3, %xmm6
        pxor	%xmm6, %xmm2
L_AES_GCM_decrypt_calc_aad_done:
        movdqu	%xmm2, 96(%esp)
        movl	196(%esp), %esi
        movl	200(%esp), %edi
        # Calculate counter and H
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        movdqa	%xmm1, %xmm5
        paddd	L_aes_gcm_one, %xmm0
        movdqa	%xmm1, %xmm4
        movdqu	%xmm0, 64(%esp)
        psrlq	$63, %xmm5
        psllq	$0x01, %xmm4
        pslldq	$8, %xmm5
        por	%xmm5, %xmm4
        pshufd	$0xff, %xmm1, %xmm1
        psrad	$31, %xmm1
        pand	L_aes_gcm_mod2_128, %xmm1
        pxor	%xmm4, %xmm1
        xorl	%ebx, %ebx
        cmpl	$0x40, 216(%esp)
        movl	216(%esp), %eax
        jl	L_AES_GCM_decrypt_done_64
        andl	$0xffffffc0, %eax
        movdqa	%xmm2, %xmm6
        # H ^ 1
        movdqu	%xmm1, (%esp)
        # H ^ 2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm1, %xmm6
        movdqa	%xmm1, %xmm7
        movdqa	%xmm1, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm1, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm0
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm0
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm0
        movdqu	%xmm0, 16(%esp)
        # H ^ 3
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm0, %xmm6
        movdqa	%xmm0, %xmm7
        movdqa	%xmm0, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm0, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm3
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm3
        movdqu	%xmm3, 32(%esp)
        # H ^ 4
        pshufd	$0x4e, %xmm0, %xmm5
        pshufd	$0x4e, %xmm0, %xmm6
        movdqa	%xmm0, %xmm7
        movdqa	%xmm0, %xmm4
        pclmulqdq	$0x11, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm0, %xmm4
        pxor	%xmm0, %xmm5
        pxor	%xmm0, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm3
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm3
        movdqu	%xmm3, 48(%esp)
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_ghash_64
L_AES_GCM_decrypt_ghash_64_inplace:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm4
        movdqa	L_aes_gcm_bswap_epi64, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pshufb	%xmm3, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pshufb	%xmm3, %xmm5
        paddd	L_aes_gcm_two, %xmm6
        pshufb	%xmm3, %xmm6
        paddd	L_aes_gcm_three, %xmm7
        pshufb	%xmm3, %xmm7
        movdqu	64(%esp), %xmm3
        paddd	L_aes_gcm_four, %xmm3
        movdqu	%xmm3, 64(%esp)
        movdqa	(%ebp), %xmm3
        pxor	%xmm3, %xmm4
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm3, %xmm7
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decryptinplace_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decryptinplace_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	224(%ebp), %xmm3
L_AES_GCM_decryptinplace_aesenc_64_ghash_avx_done:
        aesenclast	%xmm3, %xmm4
        aesenclast	%xmm3, %xmm5
        movdqu	(%ecx), %xmm0
        movdqu	16(%ecx), %xmm1
        pxor	%xmm0, %xmm4
        pxor	%xmm1, %xmm5
        movdqu	%xmm0, 112(%esp)
        movdqu	%xmm1, 128(%esp)
        movdqu	%xmm4, (%edx)
        movdqu	%xmm5, 16(%edx)
        aesenclast	%xmm3, %xmm6
        aesenclast	%xmm3, %xmm7
        movdqu	32(%ecx), %xmm0
        movdqu	48(%ecx), %xmm1
        pxor	%xmm0, %xmm6
        pxor	%xmm1, %xmm7
        movdqu	%xmm0, 144(%esp)
        movdqu	%xmm1, 160(%esp)
        movdqu	%xmm6, 32(%edx)
        movdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        movdqu	96(%esp), %xmm6
        movdqu	48(%esp), %xmm3
        movdqu	112(%esp), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm6, %xmm4
        pshufd	$0x4e, %xmm3, %xmm5
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm3, %xmm5
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm7
        pclmulqdq	$0x11, %xmm3, %xmm7
        movdqa	%xmm4, %xmm6
        pclmulqdq	$0x00, %xmm3, %xmm6
        pclmulqdq	$0x00, %xmm1, %xmm5
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqu	32(%esp), %xmm3
        movdqu	128(%esp), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	16(%esp), %xmm3
        movdqu	144(%esp), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	(%esp), %xmm3
        movdqu	160(%esp), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqa	%xmm5, %xmm1
        psrldq	$8, %xmm5
        pslldq	$8, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm5, %xmm7
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        movdqa	%xmm6, %xmm1
        pslld	$31, %xmm3
        pslld	$30, %xmm0
        pslld	$25, %xmm1
        pxor	%xmm0, %xmm3
        pxor	%xmm1, %xmm3
        movdqa	%xmm3, %xmm0
        pslldq	$12, %xmm3
        psrldq	$4, %xmm0
        pxor	%xmm3, %xmm6
        movdqa	%xmm6, %xmm1
        movdqa	%xmm6, %xmm5
        movdqa	%xmm6, %xmm4
        psrld	$0x01, %xmm1
        psrld	$2, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm7, %xmm6
        movdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_ghash_64_done
L_AES_GCM_decrypt_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm4
        movdqa	L_aes_gcm_bswap_epi64, %xmm3
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pshufb	%xmm3, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pshufb	%xmm3, %xmm5
        paddd	L_aes_gcm_two, %xmm6
        pshufb	%xmm3, %xmm6
        paddd	L_aes_gcm_three, %xmm7
        pshufb	%xmm3, %xmm7
        movdqu	64(%esp), %xmm3
        paddd	L_aes_gcm_four, %xmm3
        movdqu	%xmm3, 64(%esp)
        movdqa	(%ebp), %xmm3
        pxor	%xmm3, %xmm4
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm3, %xmm7
        movdqa	16(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	32(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	48(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	64(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	80(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	96(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	112(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	128(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	144(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	176(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_aesenc_64_ghash_avx_done
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	208(%ebp), %xmm3
        aesenc	%xmm3, %xmm4
        aesenc	%xmm3, %xmm5
        aesenc	%xmm3, %xmm6
        aesenc	%xmm3, %xmm7
        movdqa	224(%ebp), %xmm3
L_AES_GCM_decrypt_aesenc_64_ghash_avx_done:
        aesenclast	%xmm3, %xmm4
        aesenclast	%xmm3, %xmm5
        movdqu	(%ecx), %xmm0
        movdqu	16(%ecx), %xmm1
        pxor	%xmm0, %xmm4
        pxor	%xmm1, %xmm5
        movdqu	%xmm0, (%ecx)
        movdqu	%xmm1, 16(%ecx)
        movdqu	%xmm4, (%edx)
        movdqu	%xmm5, 16(%edx)
        aesenclast	%xmm3, %xmm6
        aesenclast	%xmm3, %xmm7
        movdqu	32(%ecx), %xmm0
        movdqu	48(%ecx), %xmm1
        pxor	%xmm0, %xmm6
        pxor	%xmm1, %xmm7
        movdqu	%xmm0, 32(%ecx)
        movdqu	%xmm1, 48(%ecx)
        movdqu	%xmm6, 32(%edx)
        movdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        movdqu	96(%esp), %xmm6
        movdqu	48(%esp), %xmm3
        movdqu	(%ecx), %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm6, %xmm4
        pshufd	$0x4e, %xmm3, %xmm5
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm3, %xmm5
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm7
        pclmulqdq	$0x11, %xmm3, %xmm7
        movdqa	%xmm4, %xmm6
        pclmulqdq	$0x00, %xmm3, %xmm6
        pclmulqdq	$0x00, %xmm1, %xmm5
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqu	32(%esp), %xmm3
        movdqu	16(%ecx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	16(%esp), %xmm3
        movdqu	32(%ecx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqu	(%esp), %xmm3
        movdqu	48(%ecx), %xmm4
        pshufd	$0x4e, %xmm3, %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        pxor	%xmm3, %xmm0
        pshufd	$0x4e, %xmm4, %xmm1
        pxor	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pclmulqdq	$0x11, %xmm3, %xmm2
        pclmulqdq	$0x00, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm0
        pxor	%xmm3, %xmm5
        pxor	%xmm3, %xmm6
        pxor	%xmm2, %xmm5
        pxor	%xmm2, %xmm7
        pxor	%xmm0, %xmm5
        movdqa	%xmm5, %xmm1
        psrldq	$8, %xmm5
        pslldq	$8, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm5, %xmm7
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        movdqa	%xmm6, %xmm1
        pslld	$31, %xmm3
        pslld	$30, %xmm0
        pslld	$25, %xmm1
        pxor	%xmm0, %xmm3
        pxor	%xmm1, %xmm3
        movdqa	%xmm3, %xmm0
        pslldq	$12, %xmm3
        psrldq	$4, %xmm0
        pxor	%xmm3, %xmm6
        movdqa	%xmm6, %xmm1
        movdqa	%xmm6, %xmm5
        movdqa	%xmm6, %xmm4
        psrld	$0x01, %xmm1
        psrld	$2, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm6
        pxor	%xmm7, %xmm6
        movdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_ghash_64
L_AES_GCM_decrypt_ghash_64_done:
        movdqa	%xmm6, %xmm2
        movdqu	(%esp), %xmm1
L_AES_GCM_decrypt_done_64:
        movl	216(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_decrypt_done_dec
        movl	216(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_last_block_done
L_AES_GCM_decrypt_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	(%ecx), %xmm5
        pshufb	L_aes_gcm_bswap_mask, %xmm5
        pxor	%xmm2, %xmm5
        movdqu	%xmm5, (%esp)
        movdqu	64(%esp), %xmm4
        movdqa	%xmm4, %xmm5
        pshufb	L_aes_gcm_bswap_epi64, %xmm4
        paddd	L_aes_gcm_one, %xmm5
        pxor	(%ebp), %xmm4
        movdqu	%xmm5, 64(%esp)
        movdqu	(%esp), %xmm0
        pclmulqdq	$16, %xmm1, %xmm0
        aesenc	16(%ebp), %xmm4
        aesenc	32(%ebp), %xmm4
        movdqu	(%esp), %xmm3
        pclmulqdq	$0x01, %xmm1, %xmm3
        aesenc	48(%ebp), %xmm4
        aesenc	64(%ebp), %xmm4
        aesenc	80(%ebp), %xmm4
        movdqu	(%esp), %xmm5
        pclmulqdq	$0x11, %xmm1, %xmm5
        aesenc	96(%ebp), %xmm4
        pxor	%xmm3, %xmm0
        movdqa	%xmm0, %xmm6
        psrldq	$8, %xmm0
        pslldq	$8, %xmm6
        aesenc	112(%ebp), %xmm4
        movdqu	(%esp), %xmm3
        pclmulqdq	$0x00, %xmm1, %xmm3
        pxor	%xmm3, %xmm6
        pxor	%xmm0, %xmm5
        movdqa	L_aes_gcm_mod2_128, %xmm7
        movdqa	%xmm6, %xmm3
        pclmulqdq	$16, %xmm7, %xmm3
        aesenc	128(%ebp), %xmm4
        pshufd	$0x4e, %xmm6, %xmm0
        pxor	%xmm3, %xmm0
        movdqa	%xmm0, %xmm3
        pclmulqdq	$16, %xmm7, %xmm3
        aesenc	144(%ebp), %xmm4
        pshufd	$0x4e, %xmm0, %xmm2
        pxor	%xmm3, %xmm2
        pxor	%xmm5, %xmm2
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_aesenc_gfmul_last
        aesenc	%xmm5, %xmm4
        aesenc	176(%ebp), %xmm4
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_aesenc_gfmul_last
        aesenc	%xmm5, %xmm4
        aesenc	208(%ebp), %xmm4
        movdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_aesenc_gfmul_last:
        aesenclast	%xmm5, %xmm4
        movdqu	(%ecx), %xmm5
        pxor	%xmm5, %xmm4
        movdqu	%xmm4, (%edx)
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_last_block_start
L_AES_GCM_decrypt_last_block_done:
        movl	216(%esp), %ecx
        movl	%ecx, %edx
        andl	$15, %ecx
        jz	L_AES_GCM_decrypt_aesenc_last15_dec_avx_done
        movdqu	64(%esp), %xmm0
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        pxor	(%ebp), %xmm0
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        aesenc	96(%ebp), %xmm0
        aesenc	112(%ebp), %xmm0
        aesenc	128(%ebp), %xmm0
        aesenc	144(%ebp), %xmm0
        cmpl	$11, 236(%esp)
        movdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last
        aesenc	%xmm5, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 236(%esp)
        movdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last
        aesenc	%xmm5, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last:
        aesenclast	%xmm5, %xmm0
        subl	$32, %esp
        xorl	%ecx, %ecx
        movdqu	%xmm0, (%esp)
        pxor	%xmm4, %xmm4
        movdqu	%xmm4, 16(%esp)
L_AES_GCM_decrypt_aesenc_last15_dec_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        movb	%al, 16(%esp,%ecx,1)
        xorb	(%esp,%ecx,1), %al
        movb	%al, (%edi,%ebx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_loop
        movdqu	16(%esp), %xmm0
        addl	$32, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm2, %xmm6
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm2, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm2
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
L_AES_GCM_decrypt_aesenc_last15_dec_avx_done:
L_AES_GCM_decrypt_done_dec:
        movl	212(%esp), %esi
        movl	228(%esp), %ebp
        movl	216(%esp), %edx
        movl	220(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        pinsrd	$0x00, %edx, %xmm4
        pinsrd	$2, %ecx, %xmm4
        movl	216(%esp), %edx
        movl	220(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        pinsrd	$0x01, %edx, %xmm4
        pinsrd	$3, %ecx, %xmm4
        pxor	%xmm4, %xmm2
        pshufd	$0x4e, %xmm1, %xmm5
        pshufd	$0x4e, %xmm2, %xmm6
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        pclmulqdq	$0x11, %xmm1, %xmm7
        pclmulqdq	$0x00, %xmm1, %xmm4
        pxor	%xmm1, %xmm5
        pxor	%xmm2, %xmm6
        pclmulqdq	$0x00, %xmm6, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm6
        movdqa	%xmm7, %xmm2
        pslldq	$8, %xmm6
        psrldq	$8, %xmm5
        pxor	%xmm6, %xmm4
        pxor	%xmm5, %xmm2
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        movdqa	%xmm4, %xmm7
        pslld	$31, %xmm5
        pslld	$30, %xmm6
        pslld	$25, %xmm7
        pxor	%xmm6, %xmm5
        pxor	%xmm7, %xmm5
        movdqa	%xmm5, %xmm7
        psrldq	$4, %xmm7
        pslldq	$12, %xmm5
        pxor	%xmm5, %xmm4
        movdqa	%xmm4, %xmm5
        movdqa	%xmm4, %xmm6
        psrld	$0x01, %xmm5
        psrld	$2, %xmm6
        pxor	%xmm6, %xmm5
        pxor	%xmm4, %xmm5
        psrld	$7, %xmm4
        pxor	%xmm7, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
        pshufb	L_aes_gcm_bswap_mask, %xmm2
        movdqu	80(%esp), %xmm4
        pxor	%xmm2, %xmm4
        movl	240(%esp), %edi
        cmpl	$16, %ebp
        je	L_AES_GCM_decrypt_cmp_tag_16
        subl	$16, %esp
        xorl	%ecx, %ecx
        xorl	%ebx, %ebx
        movdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_cmp_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        xorb	(%esi,%ecx,1), %al
        orb	%al, %bl
        incl	%ecx
        cmpl	%ebp, %ecx
        jne	L_AES_GCM_decrypt_cmp_tag_loop
        cmpb	$0x00, %bl
        sete	%bl
        addl	$16, %esp
        xorl	%ecx, %ecx
        jmp	L_AES_GCM_decrypt_cmp_tag_done
L_AES_GCM_decrypt_cmp_tag_16:
        movdqu	(%esi), %xmm5
        pcmpeqb	%xmm5, %xmm4
        pmovmskb	%xmm4, %edx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%ebx, %ebx
        cmpl	$0xffff, %edx
        sete	%bl
L_AES_GCM_decrypt_cmp_tag_done:
        movl	%ebx, (%edi)
        addl	$0xb0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt,.-AES_GCM_decrypt
#ifdef WOLFSSL_AESGCM_STREAM
.text
.globl	AES_GCM_init_aesni
.type	AES_GCM_init_aesni,@function
.align	16
AES_GCM_init_aesni:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	36(%esp), %ebp
        movl	44(%esp), %esi
        movl	60(%esp), %edi
        pxor	%xmm4, %xmm4
        movl	48(%esp), %edx
        cmpl	$12, %edx
        jne	L_AES_GCM_init_aesni_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        pinsrd	$0x00, (%esi), %xmm4
        pinsrd	$0x01, 4(%esi), %xmm4
        pinsrd	$2, 8(%esi), %xmm4
        pinsrd	$3, %ecx, %xmm4
        # H = Encrypt X(=0) and T = Encrypt counter
        movdqa	%xmm4, %xmm1
        movdqa	(%ebp), %xmm5
        pxor	%xmm5, %xmm1
        movdqa	16(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	32(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	48(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	64(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	80(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	96(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	112(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	128(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	144(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        cmpl	$11, 40(%esp)
        movdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_init_aesni_calc_iv_12_last
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	176(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        cmpl	$13, 40(%esp)
        movdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_init_aesni_calc_iv_12_last
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	208(%ebp), %xmm7
        aesenc	%xmm7, %xmm5
        aesenc	%xmm7, %xmm1
        movdqa	224(%ebp), %xmm7
L_AES_GCM_init_aesni_calc_iv_12_last:
        aesenclast	%xmm7, %xmm5
        aesenclast	%xmm7, %xmm1
        pshufb	L_aes_gcm_bswap_mask, %xmm5
        movdqu	%xmm1, (%edi)
        jmp	L_AES_GCM_init_aesni_iv_done
L_AES_GCM_init_aesni_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        movdqa	(%ebp), %xmm5
        aesenc	16(%ebp), %xmm5
        aesenc	32(%ebp), %xmm5
        aesenc	48(%ebp), %xmm5
        aesenc	64(%ebp), %xmm5
        aesenc	80(%ebp), %xmm5
        aesenc	96(%ebp), %xmm5
        aesenc	112(%ebp), %xmm5
        aesenc	128(%ebp), %xmm5
        aesenc	144(%ebp), %xmm5
        cmpl	$11, 40(%esp)
        movdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last
        aesenc	%xmm1, %xmm5
        aesenc	176(%ebp), %xmm5
        cmpl	$13, 40(%esp)
        movdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last
        aesenc	%xmm1, %xmm5
        aesenc	208(%ebp), %xmm5
        movdqa	224(%ebp), %xmm1
L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last:
        aesenclast	%xmm1, %xmm5
        pshufb	L_aes_gcm_bswap_mask, %xmm5
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_init_aesni_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_init_aesni_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_init_aesni_calc_iv_16_loop:
        movdqu	(%esi,%ecx,1), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm7
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm7
        pxor	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm4, %xmm1
        psrld	$31, %xmm0
        psrld	$31, %xmm1
        pslld	$0x01, %xmm7
        pslld	$0x01, %xmm4
        movdqa	%xmm0, %xmm2
        pslldq	$4, %xmm0
        psrldq	$12, %xmm2
        pslldq	$4, %xmm1
        por	%xmm2, %xmm4
        por	%xmm0, %xmm7
        por	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm7, %xmm1
        movdqa	%xmm7, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm7
        movdqa	%xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm7, %xmm2
        pxor	%xmm2, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_aesni_calc_iv_16_loop
        movl	48(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_init_aesni_calc_iv_done
L_AES_GCM_init_aesni_calc_iv_lt16:
        subl	$16, %esp
        pxor	%xmm0, %xmm0
        xorl	%ebx, %ebx
        movdqu	%xmm0, (%esp)
L_AES_GCM_init_aesni_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_aesni_calc_iv_loop
        movdqu	(%esp), %xmm0
        addl	$16, %esp
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm7
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm7
        pxor	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm4, %xmm1
        psrld	$31, %xmm0
        psrld	$31, %xmm1
        pslld	$0x01, %xmm7
        pslld	$0x01, %xmm4
        movdqa	%xmm0, %xmm2
        pslldq	$4, %xmm0
        psrldq	$12, %xmm2
        pslldq	$4, %xmm1
        por	%xmm2, %xmm4
        por	%xmm0, %xmm7
        por	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm7, %xmm1
        movdqa	%xmm7, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm7
        movdqa	%xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm7, %xmm2
        pxor	%xmm2, %xmm4
L_AES_GCM_init_aesni_calc_iv_done:
        # T = Encrypt counter
        pxor	%xmm0, %xmm0
        shll	$3, %edx
        pinsrd	$0x00, %edx, %xmm0
        pxor	%xmm0, %xmm4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm7
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm7
        pxor	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm4, %xmm1
        psrld	$31, %xmm0
        psrld	$31, %xmm1
        pslld	$0x01, %xmm7
        pslld	$0x01, %xmm4
        movdqa	%xmm0, %xmm2
        pslldq	$4, %xmm0
        psrldq	$12, %xmm2
        pslldq	$4, %xmm1
        por	%xmm2, %xmm4
        por	%xmm0, %xmm7
        por	%xmm1, %xmm4
        movdqa	%xmm7, %xmm0
        movdqa	%xmm7, %xmm1
        movdqa	%xmm7, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm7
        movdqa	%xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm7, %xmm2
        pxor	%xmm2, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        #   Encrypt counter
        movdqa	(%ebp), %xmm0
        pxor	%xmm4, %xmm0
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        aesenc	96(%ebp), %xmm0
        aesenc	112(%ebp), %xmm0
        aesenc	128(%ebp), %xmm0
        aesenc	144(%ebp), %xmm0
        cmpl	$11, 40(%esp)
        movdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 40(%esp)
        movdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm1
L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last:
        aesenclast	%xmm1, %xmm0
        movdqu	%xmm0, (%edi)
L_AES_GCM_init_aesni_iv_done:
        movl	52(%esp), %ebp
        movl	56(%esp), %edi
        pshufb	L_aes_gcm_bswap_epi64, %xmm4
        paddd	L_aes_gcm_one, %xmm4
        movdqa	%xmm5, (%ebp)
        movdqa	%xmm4, (%edi)
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_init_aesni,.-AES_GCM_init_aesni
.text
.globl	AES_GCM_aad_update_aesni
.type	AES_GCM_aad_update_aesni,@function
.align	16
AES_GCM_aad_update_aesni:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %esi
        movl	16(%esp), %edx
        movl	20(%esp), %edi
        movl	24(%esp), %eax
        movdqa	(%edi), %xmm5
        movdqa	(%eax), %xmm6
        xorl	%ecx, %ecx
L_AES_GCM_aad_update_aesni_16_loop:
        movdqu	(%esi,%ecx,1), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm5
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm6, %xmm2
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm6, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm4
        movdqa	%xmm3, %xmm5
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm4
        pxor	%xmm1, %xmm5
        movdqa	%xmm4, %xmm0
        movdqa	%xmm5, %xmm1
        psrld	$31, %xmm0
        psrld	$31, %xmm1
        pslld	$0x01, %xmm4
        pslld	$0x01, %xmm5
        movdqa	%xmm0, %xmm2
        pslldq	$4, %xmm0
        psrldq	$12, %xmm2
        pslldq	$4, %xmm1
        por	%xmm2, %xmm5
        por	%xmm0, %xmm4
        por	%xmm1, %xmm5
        movdqa	%xmm4, %xmm0
        movdqa	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm4
        movdqa	%xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm4, %xmm2
        pxor	%xmm2, %xmm5
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_aad_update_aesni_16_loop
        movdqa	%xmm5, (%edi)
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_aad_update_aesni,.-AES_GCM_aad_update_aesni
.text
.globl	AES_GCM_encrypt_block_aesni
.type	AES_GCM_encrypt_block_aesni,@function
.align	16
AES_GCM_encrypt_block_aesni:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %ecx
        movl	16(%esp), %eax
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        movdqu	(%edx), %xmm0
        movdqa	%xmm0, %xmm1
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pxor	(%ecx), %xmm0
        movdqu	%xmm1, (%edx)
        aesenc	16(%ecx), %xmm0
        aesenc	32(%ecx), %xmm0
        aesenc	48(%ecx), %xmm0
        aesenc	64(%ecx), %xmm0
        aesenc	80(%ecx), %xmm0
        aesenc	96(%ecx), %xmm0
        aesenc	112(%ecx), %xmm0
        aesenc	128(%ecx), %xmm0
        aesenc	144(%ecx), %xmm0
        cmpl	$11, %eax
        movdqa	160(%ecx), %xmm1
        jl	L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	176(%ecx), %xmm0
        cmpl	$13, %eax
        movdqa	192(%ecx), %xmm1
        jl	L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	208(%ecx), %xmm0
        movdqa	224(%ecx), %xmm1
L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last:
        aesenclast	%xmm1, %xmm0
        movdqu	(%esi), %xmm1
        pxor	%xmm1, %xmm0
        movdqu	%xmm0, (%edi)
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_block_aesni,.-AES_GCM_encrypt_block_aesni
.text
.globl	AES_GCM_ghash_block_aesni
.type	AES_GCM_ghash_block_aesni,@function
.align	16
AES_GCM_ghash_block_aesni:
        movl	4(%esp), %edx
        movl	8(%esp), %eax
        movl	12(%esp), %ecx
        movdqa	(%eax), %xmm4
        movdqa	(%ecx), %xmm5
        movdqu	(%edx), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm6
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm6
        pxor	%xmm1, %xmm4
        movdqa	%xmm6, %xmm0
        movdqa	%xmm4, %xmm1
        psrld	$31, %xmm0
        psrld	$31, %xmm1
        pslld	$0x01, %xmm6
        pslld	$0x01, %xmm4
        movdqa	%xmm0, %xmm2
        pslldq	$4, %xmm0
        psrldq	$12, %xmm2
        pslldq	$4, %xmm1
        por	%xmm2, %xmm4
        por	%xmm0, %xmm6
        por	%xmm1, %xmm4
        movdqa	%xmm6, %xmm0
        movdqa	%xmm6, %xmm1
        movdqa	%xmm6, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm6
        movdqa	%xmm6, %xmm2
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm6, %xmm2
        pxor	%xmm2, %xmm4
        movdqa	%xmm4, (%eax)
        ret
.size	AES_GCM_ghash_block_aesni,.-AES_GCM_ghash_block_aesni
.text
.globl	AES_GCM_encrypt_update_aesni
.type	AES_GCM_encrypt_update_aesni,@function
.align	16
AES_GCM_encrypt_update_aesni:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x60, %esp
        movl	144(%esp), %esi
        movdqa	(%esi), %xmm4
        movdqu	%xmm4, 64(%esp)
        movl	136(%esp), %esi
        movl	140(%esp), %ebp
        movdqa	(%esi), %xmm6
        movdqa	(%ebp), %xmm5
        movdqu	%xmm6, 80(%esp)
        movl	116(%esp), %ebp
        movl	124(%esp), %edi
        movl	128(%esp), %esi
        movdqa	%xmm5, %xmm1
        movdqa	%xmm5, %xmm0
        psrlq	$63, %xmm1
        psllq	$0x01, %xmm0
        pslldq	$8, %xmm1
        por	%xmm1, %xmm0
        pshufd	$0xff, %xmm5, %xmm5
        psrad	$31, %xmm5
        pand	L_aes_gcm_mod2_128, %xmm5
        pxor	%xmm0, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 132(%esp)
        movl	132(%esp), %eax
        jl	L_AES_GCM_encrypt_update_aesni_done_64
        andl	$0xffffffc0, %eax
        movdqa	%xmm6, %xmm2
        # H ^ 1
        movdqu	%xmm5, (%esp)
        # H ^ 2
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm4
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm4
        movdqu	%xmm4, 16(%esp)
        # H ^ 3
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm7
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm7
        movdqu	%xmm7, 32(%esp)
        # H ^ 4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm4, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm7
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm7
        movdqu	%xmm7, 48(%esp)
        # First 64 bytes of input
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm0
        movdqa	L_aes_gcm_bswap_epi64, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pshufb	%xmm7, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pshufb	%xmm7, %xmm1
        paddd	L_aes_gcm_two, %xmm2
        pshufb	%xmm7, %xmm2
        paddd	L_aes_gcm_three, %xmm3
        pshufb	%xmm7, %xmm3
        movdqu	64(%esp), %xmm7
        paddd	L_aes_gcm_four, %xmm7
        movdqu	%xmm7, 64(%esp)
        movdqa	(%ebp), %xmm7
        pxor	%xmm7, %xmm0
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm7, %xmm3
        movdqa	16(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	32(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	48(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	64(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	80(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	96(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	112(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	128(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	144(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$11, 120(%esp)
        movdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_aesni_enc_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	176(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$13, 120(%esp)
        movdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_aesni_enc_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	208(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_aesni_enc_done:
        aesenclast	%xmm7, %xmm0
        aesenclast	%xmm7, %xmm1
        movdqu	(%esi), %xmm4
        movdqu	16(%esi), %xmm5
        pxor	%xmm4, %xmm0
        pxor	%xmm5, %xmm1
        movdqu	%xmm0, (%edi)
        movdqu	%xmm1, 16(%edi)
        aesenclast	%xmm7, %xmm2
        aesenclast	%xmm7, %xmm3
        movdqu	32(%esi), %xmm4
        movdqu	48(%esi), %xmm5
        pxor	%xmm4, %xmm2
        pxor	%xmm5, %xmm3
        movdqu	%xmm2, 32(%edi)
        movdqu	%xmm3, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        jle	L_AES_GCM_encrypt_update_aesni_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_update_aesni_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm0
        movdqa	L_aes_gcm_bswap_epi64, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pshufb	%xmm7, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pshufb	%xmm7, %xmm1
        paddd	L_aes_gcm_two, %xmm2
        pshufb	%xmm7, %xmm2
        paddd	L_aes_gcm_three, %xmm3
        pshufb	%xmm7, %xmm3
        movdqu	64(%esp), %xmm7
        paddd	L_aes_gcm_four, %xmm7
        movdqu	%xmm7, 64(%esp)
        movdqa	(%ebp), %xmm7
        pxor	%xmm7, %xmm0
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm7, %xmm3
        movdqa	16(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	32(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	48(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	64(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	80(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	96(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	112(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	128(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	144(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$11, 120(%esp)
        movdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	176(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$13, 120(%esp)
        movdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	208(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_aesni_aesenc_64_ghash_avx_done:
        aesenclast	%xmm7, %xmm0
        aesenclast	%xmm7, %xmm1
        movdqu	(%ecx), %xmm4
        movdqu	16(%ecx), %xmm5
        pxor	%xmm4, %xmm0
        pxor	%xmm5, %xmm1
        movdqu	%xmm0, (%edx)
        movdqu	%xmm1, 16(%edx)
        aesenclast	%xmm7, %xmm2
        aesenclast	%xmm7, %xmm3
        movdqu	32(%ecx), %xmm4
        movdqu	48(%ecx), %xmm5
        pxor	%xmm4, %xmm2
        pxor	%xmm5, %xmm3
        movdqu	%xmm2, 32(%edx)
        movdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        movdqu	80(%esp), %xmm2
        movdqu	48(%esp), %xmm7
        movdqu	-64(%edx), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm2, %xmm0
        pshufd	$0x4e, %xmm7, %xmm1
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm7, %xmm1
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm3
        pclmulqdq	$0x11, %xmm7, %xmm3
        movdqa	%xmm0, %xmm2
        pclmulqdq	$0x00, %xmm7, %xmm2
        pclmulqdq	$0x00, %xmm5, %xmm1
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqu	32(%esp), %xmm7
        movdqu	-48(%edx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	16(%esp), %xmm7
        movdqu	-32(%edx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	(%esp), %xmm7
        movdqu	-16(%edx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqa	%xmm1, %xmm5
        psrldq	$8, %xmm1
        pslldq	$8, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm1, %xmm3
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        movdqa	%xmm2, %xmm5
        pslld	$31, %xmm7
        pslld	$30, %xmm4
        pslld	$25, %xmm5
        pxor	%xmm4, %xmm7
        pxor	%xmm5, %xmm7
        movdqa	%xmm7, %xmm4
        pslldq	$12, %xmm7
        psrldq	$4, %xmm4
        pxor	%xmm7, %xmm2
        movdqa	%xmm2, %xmm5
        movdqa	%xmm2, %xmm1
        movdqa	%xmm2, %xmm0
        psrld	$0x01, %xmm5
        psrld	$2, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm1, %xmm5
        pxor	%xmm0, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm3, %xmm2
        movdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_aesni_ghash_64
L_AES_GCM_encrypt_update_aesni_end_64:
        movdqu	80(%esp), %xmm6
        # Block 1
        movdqa	L_aes_gcm_bswap_mask, %xmm0
        movdqu	(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        movdqu	48(%esp), %xmm7
        pxor	%xmm6, %xmm5
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm7, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm0, %xmm4
        movdqa	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm4
        pxor	%xmm1, %xmm6
        # Block 2
        movdqa	L_aes_gcm_bswap_mask, %xmm0
        movdqu	16(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        movdqu	32(%esp), %xmm7
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm7, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        pxor	%xmm0, %xmm4
        pxor	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm4
        pxor	%xmm1, %xmm6
        # Block 3
        movdqa	L_aes_gcm_bswap_mask, %xmm0
        movdqu	32(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        movdqu	16(%esp), %xmm7
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm7, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        pxor	%xmm0, %xmm4
        pxor	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm4
        pxor	%xmm1, %xmm6
        # Block 4
        movdqa	L_aes_gcm_bswap_mask, %xmm0
        movdqu	48(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        movdqu	(%esp), %xmm7
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm7, %xmm2
        movdqa	%xmm7, %xmm3
        movdqa	%xmm7, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm7, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        pxor	%xmm0, %xmm4
        pxor	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm4
        pxor	%xmm1, %xmm6
        movdqa	%xmm4, %xmm0
        movdqa	%xmm4, %xmm1
        movdqa	%xmm4, %xmm2
        pslld	$31, %xmm0
        pslld	$30, %xmm1
        pslld	$25, %xmm2
        pxor	%xmm1, %xmm0
        pxor	%xmm2, %xmm0
        movdqa	%xmm0, %xmm1
        psrldq	$4, %xmm1
        pslldq	$12, %xmm0
        pxor	%xmm0, %xmm4
        movdqa	%xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        psrld	$0x01, %xmm2
        psrld	$2, %xmm3
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm2
        pxor	%xmm0, %xmm2
        pxor	%xmm1, %xmm2
        pxor	%xmm4, %xmm2
        pxor	%xmm2, %xmm6
        movdqu	(%esp), %xmm5
L_AES_GCM_encrypt_update_aesni_done_64:
        movl	132(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_encrypt_update_aesni_done_enc
        movl	132(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_aesni_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	64(%esp), %xmm0
        movdqa	%xmm0, %xmm1
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pxor	(%ebp), %xmm0
        movdqu	%xmm1, 64(%esp)
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        aesenc	96(%ebp), %xmm0
        aesenc	112(%ebp), %xmm0
        aesenc	128(%ebp), %xmm0
        aesenc	144(%ebp), %xmm0
        cmpl	$11, 120(%esp)
        movdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 120(%esp)
        movdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last
        aesenc	%xmm1, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm1
L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last:
        aesenclast	%xmm1, %xmm0
        movdqu	(%ecx), %xmm1
        pxor	%xmm1, %xmm0
        movdqu	%xmm0, (%edx)
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm6
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_aesni_last_block_ghash
L_AES_GCM_encrypt_update_aesni_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	64(%esp), %xmm0
        movdqa	%xmm0, %xmm1
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pxor	(%ebp), %xmm0
        movdqu	%xmm1, 64(%esp)
        movdqu	%xmm6, %xmm4
        pclmulqdq	$16, %xmm5, %xmm4
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        movdqu	%xmm6, %xmm7
        pclmulqdq	$0x01, %xmm5, %xmm7
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        movdqu	%xmm6, %xmm1
        pclmulqdq	$0x11, %xmm5, %xmm1
        aesenc	96(%ebp), %xmm0
        pxor	%xmm7, %xmm4
        movdqa	%xmm4, %xmm2
        psrldq	$8, %xmm4
        pslldq	$8, %xmm2
        aesenc	112(%ebp), %xmm0
        movdqu	%xmm6, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm7
        pxor	%xmm7, %xmm2
        pxor	%xmm4, %xmm1
        movdqa	L_aes_gcm_mod2_128, %xmm3
        movdqa	%xmm2, %xmm7
        pclmulqdq	$16, %xmm3, %xmm7
        aesenc	128(%ebp), %xmm0
        pshufd	$0x4e, %xmm2, %xmm4
        pxor	%xmm7, %xmm4
        movdqa	%xmm4, %xmm7
        pclmulqdq	$16, %xmm3, %xmm7
        aesenc	144(%ebp), %xmm0
        pshufd	$0x4e, %xmm4, %xmm6
        pxor	%xmm7, %xmm6
        pxor	%xmm1, %xmm6
        cmpl	$11, 120(%esp)
        movdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last
        aesenc	%xmm1, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 120(%esp)
        movdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last
        aesenc	%xmm1, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm1
L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last:
        aesenclast	%xmm1, %xmm0
        movdqu	(%ecx), %xmm1
        pxor	%xmm1, %xmm0
        movdqu	%xmm0, (%edx)
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm0, %xmm6
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_aesni_last_block_start
L_AES_GCM_encrypt_update_aesni_last_block_ghash:
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm6, %xmm2
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm6, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm6
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm6
L_AES_GCM_encrypt_update_aesni_last_block_done:
L_AES_GCM_encrypt_update_aesni_done_enc:
        movl	136(%esp), %esi
        movl	144(%esp), %edi
        movdqu	64(%esp), %xmm4
        movdqa	%xmm6, (%esi)
        movdqu	%xmm4, (%edi)
        addl	$0x60, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt_update_aesni,.-AES_GCM_encrypt_update_aesni
.text
.globl	AES_GCM_encrypt_final_aesni
.type	AES_GCM_encrypt_final_aesni,@function
.align	16
AES_GCM_encrypt_final_aesni:
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	32(%esp), %ebp
        movl	52(%esp), %esi
        movl	56(%esp), %edi
        movdqa	(%ebp), %xmm4
        movdqa	(%esi), %xmm5
        movdqa	(%edi), %xmm6
        movdqa	%xmm5, %xmm1
        movdqa	%xmm5, %xmm0
        psrlq	$63, %xmm1
        psllq	$0x01, %xmm0
        pslldq	$8, %xmm1
        por	%xmm1, %xmm0
        pshufd	$0xff, %xmm5, %xmm5
        psrad	$31, %xmm5
        pand	L_aes_gcm_mod2_128, %xmm5
        pxor	%xmm0, %xmm5
        movl	44(%esp), %edx
        movl	48(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        pinsrd	$0x00, %edx, %xmm0
        pinsrd	$2, %ecx, %xmm0
        movl	44(%esp), %edx
        movl	48(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        pinsrd	$0x01, %edx, %xmm0
        pinsrd	$3, %ecx, %xmm0
        pxor	%xmm0, %xmm4
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm4
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm4
        movdqu	%xmm6, %xmm0
        pxor	%xmm4, %xmm0
        movl	36(%esp), %edi
        cmpl	$16, 40(%esp)
        je	L_AES_GCM_encrypt_final_aesni_store_tag_16
        xorl	%ecx, %ecx
        movdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_final_aesni_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	40(%esp), %ecx
        jne	L_AES_GCM_encrypt_final_aesni_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_aesni_store_tag_done
L_AES_GCM_encrypt_final_aesni_store_tag_16:
        movdqu	%xmm0, (%edi)
L_AES_GCM_encrypt_final_aesni_store_tag_done:
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_final_aesni,.-AES_GCM_encrypt_final_aesni
.text
.globl	AES_GCM_decrypt_update_aesni
.type	AES_GCM_decrypt_update_aesni,@function
.align	16
AES_GCM_decrypt_update_aesni:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xa0, %esp
        movl	208(%esp), %esi
        movdqa	(%esi), %xmm4
        movdqu	%xmm4, 64(%esp)
        movl	200(%esp), %esi
        movl	204(%esp), %ebp
        movdqa	(%esi), %xmm6
        movdqa	(%ebp), %xmm5
        movdqu	%xmm6, 80(%esp)
        movl	180(%esp), %ebp
        movl	188(%esp), %edi
        movl	192(%esp), %esi
        movdqa	%xmm5, %xmm1
        movdqa	%xmm5, %xmm0
        psrlq	$63, %xmm1
        psllq	$0x01, %xmm0
        pslldq	$8, %xmm1
        por	%xmm1, %xmm0
        pshufd	$0xff, %xmm5, %xmm5
        psrad	$31, %xmm5
        pand	L_aes_gcm_mod2_128, %xmm5
        pxor	%xmm0, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 196(%esp)
        movl	196(%esp), %eax
        jl	L_AES_GCM_decrypt_update_aesni_done_64
        andl	$0xffffffc0, %eax
        movdqa	%xmm6, %xmm2
        # H ^ 1
        movdqu	%xmm5, (%esp)
        # H ^ 2
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm5, %xmm2
        movdqa	%xmm5, %xmm3
        movdqa	%xmm5, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm5, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm4
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm4
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm4
        movdqu	%xmm4, 16(%esp)
        # H ^ 3
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm4, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm7
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm7
        movdqu	%xmm7, 32(%esp)
        # H ^ 4
        pshufd	$0x4e, %xmm4, %xmm1
        pshufd	$0x4e, %xmm4, %xmm2
        movdqa	%xmm4, %xmm3
        movdqa	%xmm4, %xmm0
        pclmulqdq	$0x11, %xmm4, %xmm3
        pclmulqdq	$0x00, %xmm4, %xmm0
        pxor	%xmm4, %xmm1
        pxor	%xmm4, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm7
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm7
        movdqu	%xmm7, 48(%esp)
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_update_aesni_ghash_64
L_AES_GCM_decrypt_update_aesni_ghash_64_inplace:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm0
        movdqa	L_aes_gcm_bswap_epi64, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pshufb	%xmm7, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pshufb	%xmm7, %xmm1
        paddd	L_aes_gcm_two, %xmm2
        pshufb	%xmm7, %xmm2
        paddd	L_aes_gcm_three, %xmm3
        pshufb	%xmm7, %xmm3
        movdqu	64(%esp), %xmm7
        paddd	L_aes_gcm_four, %xmm7
        movdqu	%xmm7, 64(%esp)
        movdqa	(%ebp), %xmm7
        pxor	%xmm7, %xmm0
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm7, %xmm3
        movdqa	16(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	32(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	48(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	64(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	80(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	96(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	112(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	128(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	144(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$11, 184(%esp)
        movdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_aesniinplace_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	176(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$13, 184(%esp)
        movdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_aesniinplace_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	208(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_aesniinplace_aesenc_64_ghash_avx_done:
        aesenclast	%xmm7, %xmm0
        aesenclast	%xmm7, %xmm1
        movdqu	(%ecx), %xmm4
        movdqu	16(%ecx), %xmm5
        pxor	%xmm4, %xmm0
        pxor	%xmm5, %xmm1
        movdqu	%xmm4, 96(%esp)
        movdqu	%xmm5, 112(%esp)
        movdqu	%xmm0, (%edx)
        movdqu	%xmm1, 16(%edx)
        aesenclast	%xmm7, %xmm2
        aesenclast	%xmm7, %xmm3
        movdqu	32(%ecx), %xmm4
        movdqu	48(%ecx), %xmm5
        pxor	%xmm4, %xmm2
        pxor	%xmm5, %xmm3
        movdqu	%xmm4, 128(%esp)
        movdqu	%xmm5, 144(%esp)
        movdqu	%xmm2, 32(%edx)
        movdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        movdqu	80(%esp), %xmm2
        movdqu	48(%esp), %xmm7
        movdqu	96(%esp), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm2, %xmm0
        pshufd	$0x4e, %xmm7, %xmm1
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm7, %xmm1
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm3
        pclmulqdq	$0x11, %xmm7, %xmm3
        movdqa	%xmm0, %xmm2
        pclmulqdq	$0x00, %xmm7, %xmm2
        pclmulqdq	$0x00, %xmm5, %xmm1
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqu	32(%esp), %xmm7
        movdqu	112(%esp), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	16(%esp), %xmm7
        movdqu	128(%esp), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	(%esp), %xmm7
        movdqu	144(%esp), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqa	%xmm1, %xmm5
        psrldq	$8, %xmm1
        pslldq	$8, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm1, %xmm3
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        movdqa	%xmm2, %xmm5
        pslld	$31, %xmm7
        pslld	$30, %xmm4
        pslld	$25, %xmm5
        pxor	%xmm4, %xmm7
        pxor	%xmm5, %xmm7
        movdqa	%xmm7, %xmm4
        pslldq	$12, %xmm7
        psrldq	$4, %xmm4
        pxor	%xmm7, %xmm2
        movdqa	%xmm2, %xmm5
        movdqa	%xmm2, %xmm1
        movdqa	%xmm2, %xmm0
        psrld	$0x01, %xmm5
        psrld	$2, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm1, %xmm5
        pxor	%xmm0, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm3, %xmm2
        movdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_aesni_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_update_aesni_ghash_64_done
L_AES_GCM_decrypt_update_aesni_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # Encrypt 64 bytes of counter
        movdqu	64(%esp), %xmm0
        movdqa	L_aes_gcm_bswap_epi64, %xmm7
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pshufb	%xmm7, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pshufb	%xmm7, %xmm1
        paddd	L_aes_gcm_two, %xmm2
        pshufb	%xmm7, %xmm2
        paddd	L_aes_gcm_three, %xmm3
        pshufb	%xmm7, %xmm3
        movdqu	64(%esp), %xmm7
        paddd	L_aes_gcm_four, %xmm7
        movdqu	%xmm7, 64(%esp)
        movdqa	(%ebp), %xmm7
        pxor	%xmm7, %xmm0
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm7, %xmm3
        movdqa	16(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	32(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	48(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	64(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	80(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	96(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	112(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	128(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	144(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$11, 184(%esp)
        movdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	176(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        cmpl	$13, 184(%esp)
        movdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_64_ghash_avx_done
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	208(%ebp), %xmm7
        aesenc	%xmm7, %xmm0
        aesenc	%xmm7, %xmm1
        aesenc	%xmm7, %xmm2
        aesenc	%xmm7, %xmm3
        movdqa	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_aesni_aesenc_64_ghash_avx_done:
        aesenclast	%xmm7, %xmm0
        aesenclast	%xmm7, %xmm1
        movdqu	(%ecx), %xmm4
        movdqu	16(%ecx), %xmm5
        pxor	%xmm4, %xmm0
        pxor	%xmm5, %xmm1
        movdqu	%xmm4, (%ecx)
        movdqu	%xmm5, 16(%ecx)
        movdqu	%xmm0, (%edx)
        movdqu	%xmm1, 16(%edx)
        aesenclast	%xmm7, %xmm2
        aesenclast	%xmm7, %xmm3
        movdqu	32(%ecx), %xmm4
        movdqu	48(%ecx), %xmm5
        pxor	%xmm4, %xmm2
        pxor	%xmm5, %xmm3
        movdqu	%xmm4, 32(%ecx)
        movdqu	%xmm5, 48(%ecx)
        movdqu	%xmm2, 32(%edx)
        movdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        movdqu	80(%esp), %xmm2
        movdqu	48(%esp), %xmm7
        movdqu	(%ecx), %xmm0
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm2, %xmm0
        pshufd	$0x4e, %xmm7, %xmm1
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm7, %xmm1
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm3
        pclmulqdq	$0x11, %xmm7, %xmm3
        movdqa	%xmm0, %xmm2
        pclmulqdq	$0x00, %xmm7, %xmm2
        pclmulqdq	$0x00, %xmm5, %xmm1
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqu	32(%esp), %xmm7
        movdqu	16(%ecx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	16(%esp), %xmm7
        movdqu	32(%ecx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqu	(%esp), %xmm7
        movdqu	48(%ecx), %xmm0
        pshufd	$0x4e, %xmm7, %xmm4
        pshufb	L_aes_gcm_bswap_mask, %xmm0
        pxor	%xmm7, %xmm4
        pshufd	$0x4e, %xmm0, %xmm5
        pxor	%xmm0, %xmm5
        movdqa	%xmm0, %xmm6
        pclmulqdq	$0x11, %xmm7, %xmm6
        pclmulqdq	$0x00, %xmm0, %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm4
        pxor	%xmm7, %xmm1
        pxor	%xmm7, %xmm2
        pxor	%xmm6, %xmm1
        pxor	%xmm6, %xmm3
        pxor	%xmm4, %xmm1
        movdqa	%xmm1, %xmm5
        psrldq	$8, %xmm1
        pslldq	$8, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm1, %xmm3
        movdqa	%xmm2, %xmm7
        movdqa	%xmm2, %xmm4
        movdqa	%xmm2, %xmm5
        pslld	$31, %xmm7
        pslld	$30, %xmm4
        pslld	$25, %xmm5
        pxor	%xmm4, %xmm7
        pxor	%xmm5, %xmm7
        movdqa	%xmm7, %xmm4
        pslldq	$12, %xmm7
        psrldq	$4, %xmm4
        pxor	%xmm7, %xmm2
        movdqa	%xmm2, %xmm5
        movdqa	%xmm2, %xmm1
        movdqa	%xmm2, %xmm0
        psrld	$0x01, %xmm5
        psrld	$2, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm1, %xmm5
        pxor	%xmm0, %xmm5
        pxor	%xmm4, %xmm5
        pxor	%xmm5, %xmm2
        pxor	%xmm3, %xmm2
        movdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_aesni_ghash_64
L_AES_GCM_decrypt_update_aesni_ghash_64_done:
        movdqa	%xmm2, %xmm6
        movdqu	(%esp), %xmm5
L_AES_GCM_decrypt_update_aesni_done_64:
        movl	196(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_decrypt_update_aesni_done_dec
        movl	196(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_update_aesni_last_block_done
L_AES_GCM_decrypt_update_aesni_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        movdqu	(%ecx), %xmm1
        pshufb	L_aes_gcm_bswap_mask, %xmm1
        pxor	%xmm6, %xmm1
        movdqu	%xmm1, (%esp)
        movdqu	64(%esp), %xmm0
        movdqa	%xmm0, %xmm1
        pshufb	L_aes_gcm_bswap_epi64, %xmm0
        paddd	L_aes_gcm_one, %xmm1
        pxor	(%ebp), %xmm0
        movdqu	%xmm1, 64(%esp)
        movdqu	(%esp), %xmm4
        pclmulqdq	$16, %xmm5, %xmm4
        aesenc	16(%ebp), %xmm0
        aesenc	32(%ebp), %xmm0
        movdqu	(%esp), %xmm7
        pclmulqdq	$0x01, %xmm5, %xmm7
        aesenc	48(%ebp), %xmm0
        aesenc	64(%ebp), %xmm0
        aesenc	80(%ebp), %xmm0
        movdqu	(%esp), %xmm1
        pclmulqdq	$0x11, %xmm5, %xmm1
        aesenc	96(%ebp), %xmm0
        pxor	%xmm7, %xmm4
        movdqa	%xmm4, %xmm2
        psrldq	$8, %xmm4
        pslldq	$8, %xmm2
        aesenc	112(%ebp), %xmm0
        movdqu	(%esp), %xmm7
        pclmulqdq	$0x00, %xmm5, %xmm7
        pxor	%xmm7, %xmm2
        pxor	%xmm4, %xmm1
        movdqa	L_aes_gcm_mod2_128, %xmm3
        movdqa	%xmm2, %xmm7
        pclmulqdq	$16, %xmm3, %xmm7
        aesenc	128(%ebp), %xmm0
        pshufd	$0x4e, %xmm2, %xmm4
        pxor	%xmm7, %xmm4
        movdqa	%xmm4, %xmm7
        pclmulqdq	$16, %xmm3, %xmm7
        aesenc	144(%ebp), %xmm0
        pshufd	$0x4e, %xmm4, %xmm6
        pxor	%xmm7, %xmm6
        pxor	%xmm1, %xmm6
        cmpl	$11, 184(%esp)
        movdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last
        aesenc	%xmm1, %xmm0
        aesenc	176(%ebp), %xmm0
        cmpl	$13, 184(%esp)
        movdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last
        aesenc	%xmm1, %xmm0
        aesenc	208(%ebp), %xmm0
        movdqa	224(%ebp), %xmm1
L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last:
        aesenclast	%xmm1, %xmm0
        movdqu	(%ecx), %xmm1
        pxor	%xmm1, %xmm0
        movdqu	%xmm0, (%edx)
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_aesni_last_block_start
L_AES_GCM_decrypt_update_aesni_last_block_done:
L_AES_GCM_decrypt_update_aesni_done_dec:
        movl	200(%esp), %esi
        movl	208(%esp), %edi
        movdqu	64(%esp), %xmm4
        movdqa	%xmm6, (%esi)
        movdqu	%xmm4, (%edi)
        addl	$0xa0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_update_aesni,.-AES_GCM_decrypt_update_aesni
.text
.globl	AES_GCM_decrypt_final_aesni
.type	AES_GCM_decrypt_final_aesni,@function
.align	16
AES_GCM_decrypt_final_aesni:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	36(%esp), %ebp
        movl	56(%esp), %esi
        movl	60(%esp), %edi
        movdqa	(%ebp), %xmm6
        movdqa	(%esi), %xmm5
        movdqa	(%edi), %xmm7
        movdqa	%xmm5, %xmm1
        movdqa	%xmm5, %xmm0
        psrlq	$63, %xmm1
        psllq	$0x01, %xmm0
        pslldq	$8, %xmm1
        por	%xmm1, %xmm0
        pshufd	$0xff, %xmm5, %xmm5
        psrad	$31, %xmm5
        pand	L_aes_gcm_mod2_128, %xmm5
        pxor	%xmm0, %xmm5
        movl	48(%esp), %edx
        movl	52(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        pinsrd	$0x00, %edx, %xmm0
        pinsrd	$2, %ecx, %xmm0
        movl	48(%esp), %edx
        movl	52(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        pinsrd	$0x01, %edx, %xmm0
        pinsrd	$3, %ecx, %xmm0
        pxor	%xmm0, %xmm6
        pshufd	$0x4e, %xmm5, %xmm1
        pshufd	$0x4e, %xmm6, %xmm2
        movdqa	%xmm6, %xmm3
        movdqa	%xmm6, %xmm0
        pclmulqdq	$0x11, %xmm5, %xmm3
        pclmulqdq	$0x00, %xmm5, %xmm0
        pxor	%xmm5, %xmm1
        pxor	%xmm6, %xmm2
        pclmulqdq	$0x00, %xmm2, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm2
        movdqa	%xmm3, %xmm6
        pslldq	$8, %xmm2
        psrldq	$8, %xmm1
        pxor	%xmm2, %xmm0
        pxor	%xmm1, %xmm6
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        movdqa	%xmm0, %xmm3
        pslld	$31, %xmm1
        pslld	$30, %xmm2
        pslld	$25, %xmm3
        pxor	%xmm2, %xmm1
        pxor	%xmm3, %xmm1
        movdqa	%xmm1, %xmm3
        psrldq	$4, %xmm3
        pslldq	$12, %xmm1
        pxor	%xmm1, %xmm0
        movdqa	%xmm0, %xmm1
        movdqa	%xmm0, %xmm2
        psrld	$0x01, %xmm1
        psrld	$2, %xmm2
        pxor	%xmm2, %xmm1
        pxor	%xmm0, %xmm1
        psrld	$7, %xmm0
        pxor	%xmm3, %xmm1
        pxor	%xmm0, %xmm1
        pxor	%xmm1, %xmm6
        pshufb	L_aes_gcm_bswap_mask, %xmm6
        movdqu	%xmm7, %xmm0
        pxor	%xmm6, %xmm0
        movl	40(%esp), %esi
        movl	64(%esp), %edi
        cmpl	$16, 44(%esp)
        je	L_AES_GCM_decrypt_final_aesni_cmp_tag_16
        subl	$16, %esp
        xorl	%ecx, %ecx
        xorl	%ebx, %ebx
        movdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_final_aesni_cmp_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        xorb	(%esi,%ecx,1), %al
        orb	%al, %bl
        incl	%ecx
        cmpl	44(%esp), %ecx
        jne	L_AES_GCM_decrypt_final_aesni_cmp_tag_loop
        cmpb	$0x00, %bl
        sete	%bl
        addl	$16, %esp
        xorl	%ecx, %ecx
        jmp	L_AES_GCM_decrypt_final_aesni_cmp_tag_done
L_AES_GCM_decrypt_final_aesni_cmp_tag_16:
        movdqu	(%esi), %xmm1
        pcmpeqb	%xmm1, %xmm0
        pmovmskb	%xmm0, %edx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%ebx, %ebx
        cmpl	$0xffff, %edx
        sete	%bl
L_AES_GCM_decrypt_final_aesni_cmp_tag_done:
        movl	%ebx, (%edi)
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_final_aesni,.-AES_GCM_decrypt_final_aesni
#endif /* WOLFSSL_AESGCM_STREAM */
#ifdef HAVE_INTEL_AVX1
.text
.globl	AES_GCM_encrypt_avx1
.type	AES_GCM_encrypt_avx1,@function
.align	16
AES_GCM_encrypt_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x70, %esp
        movl	144(%esp), %esi
        movl	168(%esp), %ebp
        movl	160(%esp), %edx
        vpxor	%xmm0, %xmm0, %xmm0
        vpxor	%xmm2, %xmm2, %xmm2
        cmpl	$12, %edx
        jne	L_AES_GCM_encrypt_avx1_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        vpinsrd	$0x00, (%esi), %xmm0, %xmm0
        vpinsrd	$0x01, 4(%esi), %xmm0, %xmm0
        vpinsrd	$2, 8(%esi), %xmm0, %xmm0
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	(%ebp), %xmm1
        vpxor	%xmm1, %xmm0, %xmm5
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_calc_iv_12_last
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_calc_iv_12_last
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_avx1_calc_iv_12_last:
        vaesenclast	%xmm3, %xmm1, %xmm1
        vaesenclast	%xmm3, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm1, %xmm1
        vmovdqu	%xmm5, 80(%esp)
        jmp	L_AES_GCM_encrypt_avx1_iv_done
L_AES_GCM_encrypt_avx1_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqa	(%ebp), %xmm1
        vaesenc	16(%ebp), %xmm1, %xmm1
        vaesenc	32(%ebp), %xmm1, %xmm1
        vaesenc	48(%ebp), %xmm1, %xmm1
        vaesenc	64(%ebp), %xmm1, %xmm1
        vaesenc	80(%ebp), %xmm1, %xmm1
        vaesenc	96(%ebp), %xmm1, %xmm1
        vaesenc	112(%ebp), %xmm1, %xmm1
        vaesenc	128(%ebp), %xmm1, %xmm1
        vaesenc	144(%ebp), %xmm1, %xmm1
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm5, %xmm1, %xmm1
        vaesenc	176(%ebp), %xmm1, %xmm1
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm5, %xmm1, %xmm1
        vaesenc	208(%ebp), %xmm1, %xmm1
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm1, %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm1, %xmm1
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_encrypt_avx1_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_avx1_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_avx1_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx1_calc_iv_16_loop
        movl	160(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_avx1_calc_iv_done
L_AES_GCM_encrypt_avx1_calc_iv_lt16:
        subl	$16, %esp
        vpxor	%xmm4, %xmm4, %xmm4
        xorl	%ebx, %ebx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_avx1_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx1_calc_iv_loop
        vmovdqu	(%esp), %xmm4
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
L_AES_GCM_encrypt_avx1_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm4, %xmm4, %xmm4
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        #   Encrypt counter
        vmovdqa	(%ebp), %xmm4
        vpxor	%xmm0, %xmm4, %xmm4
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vaesenc	96(%ebp), %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm4, %xmm4
        vaesenc	128(%ebp), %xmm4, %xmm4
        vaesenc	144(%ebp), %xmm4, %xmm4
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm4, %xmm4
        vmovdqu	%xmm4, 80(%esp)
L_AES_GCM_encrypt_avx1_iv_done:
        movl	140(%esp), %esi
        # Additional authentication data
        movl	156(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_encrypt_avx1_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_avx1_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_avx1_calc_aad_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm2, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm4
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm2, %xmm2
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm2, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm2, %xmm2
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm2, %xmm2
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm2, %xmm2
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm2, %xmm2
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx1_calc_aad_16_loop
        movl	156(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_avx1_calc_aad_done
L_AES_GCM_encrypt_avx1_calc_aad_lt16:
        subl	$16, %esp
        vpxor	%xmm4, %xmm4, %xmm4
        xorl	%ebx, %ebx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_avx1_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx1_calc_aad_loop
        vmovdqu	(%esp), %xmm4
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm2, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm4
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm2, %xmm2
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm2, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm2, %xmm2
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm2, %xmm2
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm2, %xmm2
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm2, %xmm2
L_AES_GCM_encrypt_avx1_calc_aad_done:
        vmovdqu	%xmm2, 96(%esp)
        movl	132(%esp), %esi
        movl	136(%esp), %edi
        # Calculate counter and H
        vpsrlq	$63, %xmm1, %xmm5
        vpsllq	$0x01, %xmm1, %xmm4
        vpslldq	$8, %xmm5, %xmm5
        vpor	%xmm5, %xmm4, %xmm4
        vpshufd	$0xff, %xmm1, %xmm1
        vpsrad	$31, %xmm1, %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm0, %xmm0
        vpand	L_aes_gcm_avx1_mod2_128, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, 64(%esp)
        xorl	%ebx, %ebx
        cmpl	$0x40, 152(%esp)
        movl	152(%esp), %eax
        jl	L_AES_GCM_encrypt_avx1_done_64
        andl	$0xffffffc0, %eax
        vmovdqa	%xmm2, %xmm6
        # H ^ 1
        vmovdqu	%xmm1, (%esp)
        # H ^ 2
        vpclmulqdq	$0x00, %xmm1, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm1, %xmm1, %xmm0
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm0, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm0, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm0, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm3
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm3, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm3
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm3, 48(%esp)
        # First 64 bytes of input
        vmovdqu	64(%esp), %xmm4
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm3
        vpaddd	L_aes_gcm_avx1_one, %xmm4, %xmm5
        vpshufb	%xmm3, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx1_two, %xmm4, %xmm6
        vpshufb	%xmm3, %xmm6, %xmm6
        vpaddd	L_aes_gcm_avx1_three, %xmm4, %xmm7
        vpshufb	%xmm3, %xmm7, %xmm7
        vpshufb	%xmm3, %xmm4, %xmm4
        vmovdqu	64(%esp), %xmm3
        vpaddd	L_aes_gcm_avx1_four, %xmm3, %xmm3
        vmovdqu	%xmm3, 64(%esp)
        vmovdqa	(%ebp), %xmm3
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_avx1_aesenc_64_enc_done:
        vaesenclast	%xmm3, %xmm4, %xmm4
        vaesenclast	%xmm3, %xmm5, %xmm5
        vmovdqu	(%esi), %xmm0
        vmovdqu	16(%esi), %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	%xmm0, (%esi)
        vmovdqu	%xmm1, 16(%esi)
        vmovdqu	%xmm4, (%edi)
        vmovdqu	%xmm5, 16(%edi)
        vaesenclast	%xmm3, %xmm6, %xmm6
        vaesenclast	%xmm3, %xmm7, %xmm7
        vmovdqu	32(%esi), %xmm0
        vmovdqu	48(%esi), %xmm1
        vpxor	%xmm0, %xmm6, %xmm6
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm0, 32(%esi)
        vmovdqu	%xmm1, 48(%esi)
        vmovdqu	%xmm6, 32(%edi)
        vmovdqu	%xmm7, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        movl	%esi, %ecx
        movl	%edi, %edx
        jle	L_AES_GCM_encrypt_avx1_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_avx1_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm4
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm3
        vpaddd	L_aes_gcm_avx1_one, %xmm4, %xmm5
        vpshufb	%xmm3, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx1_two, %xmm4, %xmm6
        vpshufb	%xmm3, %xmm6, %xmm6
        vpaddd	L_aes_gcm_avx1_three, %xmm4, %xmm7
        vpshufb	%xmm3, %xmm7, %xmm7
        vpshufb	%xmm3, %xmm4, %xmm4
        vmovdqu	64(%esp), %xmm3
        vpaddd	L_aes_gcm_avx1_four, %xmm3, %xmm3
        vmovdqu	%xmm3, 64(%esp)
        vmovdqa	(%ebp), %xmm3
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_encrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_encrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm3, %xmm4, %xmm4
        vaesenclast	%xmm3, %xmm5, %xmm5
        vmovdqu	(%ecx), %xmm0
        vmovdqu	16(%ecx), %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	%xmm4, (%edx)
        vmovdqu	%xmm5, 16(%edx)
        vaesenclast	%xmm3, %xmm6, %xmm6
        vaesenclast	%xmm3, %xmm7, %xmm7
        vmovdqu	32(%ecx), %xmm0
        vmovdqu	48(%ecx), %xmm1
        vpxor	%xmm0, %xmm6, %xmm6
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm6, 32(%edx)
        vmovdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        vmovdqu	96(%esp), %xmm6
        vmovdqu	48(%esp), %xmm3
        vmovdqu	-64(%edx), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm3, %xmm5
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm7
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm6
        vpclmulqdq	$0x00, %xmm1, %xmm5, %xmm5
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqu	32(%esp), %xmm3
        vmovdqu	-48(%edx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	16(%esp), %xmm3
        vmovdqu	-32(%edx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	(%esp), %xmm3
        vmovdqu	-16(%edx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpslld	$31, %xmm6, %xmm3
        vpslld	$30, %xmm6, %xmm0
        vpslld	$25, %xmm6, %xmm1
        vpxor	%xmm0, %xmm3, %xmm3
        vpxor	%xmm1, %xmm3, %xmm3
        vpsrldq	$4, %xmm3, %xmm0
        vpslldq	$12, %xmm3, %xmm3
        vpxor	%xmm3, %xmm6, %xmm6
        vpsrld	$0x01, %xmm6, %xmm1
        vpsrld	$2, %xmm6, %xmm5
        vpsrld	$7, %xmm6, %xmm4
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        vmovdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_avx1_ghash_64
L_AES_GCM_encrypt_avx1_end_64:
        vmovdqu	96(%esp), %xmm2
        # Block 1
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm4
        vmovdqa	(%edx), %xmm1
        vpshufb	%xmm4, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm3, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm3, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm3, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm0
        vmovdqa	%xmm7, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm0, %xmm0
        vpxor	%xmm5, %xmm2, %xmm2
        # Block 2
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm4
        vmovdqa	16(%edx), %xmm1
        vpshufb	%xmm4, %xmm1, %xmm1
        vmovdqu	32(%esp), %xmm3
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm3, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm3, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm3, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm7, %xmm2, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm0, %xmm0
        vpxor	%xmm5, %xmm2, %xmm2
        # Block 3
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm4
        vmovdqa	32(%edx), %xmm1
        vpshufb	%xmm4, %xmm1, %xmm1
        vmovdqu	16(%esp), %xmm3
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm3, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm3, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm3, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm7, %xmm2, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm0, %xmm0
        vpxor	%xmm5, %xmm2, %xmm2
        # Block 4
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm4
        vmovdqa	48(%edx), %xmm1
        vpshufb	%xmm4, %xmm1, %xmm1
        vmovdqu	(%esp), %xmm3
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm3, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm3, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm3, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm7, %xmm2, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm0, %xmm0
        vpxor	%xmm5, %xmm2, %xmm2
        vpslld	$31, %xmm0, %xmm4
        vpslld	$30, %xmm0, %xmm5
        vpslld	$25, %xmm0, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm6
        vpsrld	$2, %xmm0, %xmm7
        vpsrld	$7, %xmm0, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm0, %xmm6, %xmm6
        vpxor	%xmm6, %xmm2, %xmm2
        vmovdqu	(%esp), %xmm1
L_AES_GCM_encrypt_avx1_done_64:
        movl	152(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_encrypt_avx1_done_enc
        movl	152(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_avx1_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm5
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm5, %xmm4
        vpaddd	L_aes_gcm_avx1_one, %xmm5, %xmm5
        vmovdqu	%xmm5, 64(%esp)
        vpxor	(%ebp), %xmm4, %xmm4
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vaesenc	96(%ebp), %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm4, %xmm4
        vaesenc	128(%ebp), %xmm4, %xmm4
        vaesenc	144(%ebp), %xmm4, %xmm4
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_avx1_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm4, %xmm4
        vmovdqu	(%ecx), %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vmovdqu	%xmm4, (%edx)
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_avx1_last_block_ghash
L_AES_GCM_encrypt_avx1_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm5
        vmovdqu	%xmm2, %xmm7
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm5, %xmm4
        vpaddd	L_aes_gcm_avx1_one, %xmm5, %xmm5
        vmovdqu	%xmm5, 64(%esp)
        vpxor	(%ebp), %xmm4, %xmm4
        vpclmulqdq	$16, %xmm1, %xmm7, %xmm0
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x01, %xmm1, %xmm7, %xmm3
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x11, %xmm1, %xmm7, %xmm5
        vaesenc	96(%ebp), %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpslldq	$8, %xmm0, %xmm6
        vpsrldq	$8, %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x00, %xmm1, %xmm7, %xmm3
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqa	L_aes_gcm_avx1_mod2_128, %xmm7
        vpclmulqdq	$16, %xmm7, %xmm6, %xmm3
        vaesenc	128(%ebp), %xmm4, %xmm4
        vpshufd	$0x4e, %xmm6, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpclmulqdq	$16, %xmm7, %xmm0, %xmm3
        vaesenc	144(%ebp), %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm2
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm5, %xmm2, %xmm2
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_gfmul_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_gfmul_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_avx1_aesenc_gfmul_last:
        vaesenclast	%xmm5, %xmm4, %xmm4
        vmovdqu	(%ecx), %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vmovdqu	%xmm4, (%edx)
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        addl	$16, %ebx
        vpxor	%xmm4, %xmm2, %xmm2
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_avx1_last_block_start
L_AES_GCM_encrypt_avx1_last_block_ghash:
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm2, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm2, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm2, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm2, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm2
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
L_AES_GCM_encrypt_avx1_last_block_done:
        movl	152(%esp), %ecx
        movl	%ecx, %edx
        andl	$15, %ecx
        jz	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_done
        vmovdqu	64(%esp), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm0, %xmm0
        vpxor	(%ebp), %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 172(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	%xmm5, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 172(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	%xmm5, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm0, %xmm0
        subl	$16, %esp
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        xorb	(%esp,%ecx,1), %al
        movb	%al, (%edi,%ebx,1)
        movb	%al, (%esp,%ecx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_loop
        xorl	%eax, %eax
        cmpl	$16, %ecx
        je	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_finish_enc
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_byte_loop:
        movb	%al, (%esp,%ecx,1)
        incl	%ecx
        cmpl	$16, %ecx
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_byte_loop
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_finish_enc:
        vmovdqu	(%esp), %xmm0
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm2, %xmm2
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm2, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm2, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm2, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm2, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm2
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_done:
L_AES_GCM_encrypt_avx1_done_enc:
        movl	148(%esp), %edi
        movl	164(%esp), %ebx
        movl	152(%esp), %edx
        movl	156(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        vpinsrd	$0x00, %edx, %xmm4, %xmm4
        vpinsrd	$2, %ecx, %xmm4, %xmm4
        movl	152(%esp), %edx
        movl	156(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        vpinsrd	$0x01, %edx, %xmm4, %xmm4
        vpinsrd	$3, %ecx, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm2, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm2, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm2, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm2, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm2
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm2, %xmm2
        vpxor	80(%esp), %xmm2, %xmm4
        cmpl	$16, %ebx
        je	L_AES_GCM_encrypt_avx1_store_tag_16
        xorl	%ecx, %ecx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_encrypt_avx1_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	%ebx, %ecx
        jne	L_AES_GCM_encrypt_avx1_store_tag_loop
        jmp	L_AES_GCM_encrypt_avx1_store_tag_done
L_AES_GCM_encrypt_avx1_store_tag_16:
        vmovdqu	%xmm4, (%edi)
L_AES_GCM_encrypt_avx1_store_tag_done:
        addl	$0x70, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt_avx1,.-AES_GCM_encrypt_avx1
.text
.globl	AES_GCM_decrypt_avx1
.type	AES_GCM_decrypt_avx1,@function
.align	16
AES_GCM_decrypt_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xb0, %esp
        movl	208(%esp), %esi
        movl	232(%esp), %ebp
        movl	224(%esp), %edx
        vpxor	%xmm0, %xmm0, %xmm0
        vpxor	%xmm2, %xmm2, %xmm2
        cmpl	$12, %edx
        jne	L_AES_GCM_decrypt_avx1_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        vpinsrd	$0x00, (%esi), %xmm0, %xmm0
        vpinsrd	$0x01, 4(%esi), %xmm0, %xmm0
        vpinsrd	$2, 8(%esi), %xmm0, %xmm0
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	(%ebp), %xmm1
        vpxor	%xmm1, %xmm0, %xmm5
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1_calc_iv_12_last
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1_calc_iv_12_last
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm1, %xmm1
        vaesenc	%xmm3, %xmm5, %xmm5
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_decrypt_avx1_calc_iv_12_last:
        vaesenclast	%xmm3, %xmm1, %xmm1
        vaesenclast	%xmm3, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm1, %xmm1
        vmovdqu	%xmm5, 80(%esp)
        jmp	L_AES_GCM_decrypt_avx1_iv_done
L_AES_GCM_decrypt_avx1_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqa	(%ebp), %xmm1
        vaesenc	16(%ebp), %xmm1, %xmm1
        vaesenc	32(%ebp), %xmm1, %xmm1
        vaesenc	48(%ebp), %xmm1, %xmm1
        vaesenc	64(%ebp), %xmm1, %xmm1
        vaesenc	80(%ebp), %xmm1, %xmm1
        vaesenc	96(%ebp), %xmm1, %xmm1
        vaesenc	112(%ebp), %xmm1, %xmm1
        vaesenc	128(%ebp), %xmm1, %xmm1
        vaesenc	144(%ebp), %xmm1, %xmm1
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm5, %xmm1, %xmm1
        vaesenc	176(%ebp), %xmm1, %xmm1
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm5, %xmm1, %xmm1
        vaesenc	208(%ebp), %xmm1, %xmm1
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm1, %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm1, %xmm1
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_decrypt_avx1_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_avx1_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_avx1_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx1_calc_iv_16_loop
        movl	224(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_avx1_calc_iv_done
L_AES_GCM_decrypt_avx1_calc_iv_lt16:
        subl	$16, %esp
        vpxor	%xmm4, %xmm4, %xmm4
        xorl	%ebx, %ebx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_avx1_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx1_calc_iv_loop
        vmovdqu	(%esp), %xmm4
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
L_AES_GCM_decrypt_avx1_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm4, %xmm4, %xmm4
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm4, %xmm4
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm0, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm0
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm0, %xmm0
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm0, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm0, %xmm0
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm0, %xmm0
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm0, %xmm0
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        #   Encrypt counter
        vmovdqa	(%ebp), %xmm4
        vpxor	%xmm0, %xmm4, %xmm4
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vaesenc	96(%ebp), %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm4, %xmm4
        vaesenc	128(%ebp), %xmm4, %xmm4
        vaesenc	144(%ebp), %xmm4, %xmm4
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm4, %xmm4
        vmovdqu	%xmm4, 80(%esp)
L_AES_GCM_decrypt_avx1_iv_done:
        movl	204(%esp), %esi
        # Additional authentication data
        movl	220(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_decrypt_avx1_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_avx1_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_avx1_calc_aad_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm2, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm4
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm2, %xmm2
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm2, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm2, %xmm2
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm2, %xmm2
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm2, %xmm2
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm2, %xmm2
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx1_calc_aad_16_loop
        movl	220(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_avx1_calc_aad_done
L_AES_GCM_decrypt_avx1_calc_aad_lt16:
        subl	$16, %esp
        vpxor	%xmm4, %xmm4, %xmm4
        xorl	%ebx, %ebx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_avx1_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx1_calc_aad_loop
        vmovdqu	(%esp), %xmm4
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm2, %xmm5
        vpshufd	$0x4e, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm4
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqa	%xmm4, %xmm3
        vmovdqa	%xmm7, %xmm2
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm5, %xmm2, %xmm2
        vpsrld	$31, %xmm3, %xmm4
        vpsrld	$31, %xmm2, %xmm5
        vpslld	$0x01, %xmm3, %xmm3
        vpslld	$0x01, %xmm2, %xmm2
        vpsrldq	$12, %xmm4, %xmm6
        vpslldq	$4, %xmm4, %xmm4
        vpslldq	$4, %xmm5, %xmm5
        vpor	%xmm6, %xmm2, %xmm2
        vpor	%xmm4, %xmm3, %xmm3
        vpor	%xmm5, %xmm2, %xmm2
        vpslld	$31, %xmm3, %xmm4
        vpslld	$30, %xmm3, %xmm5
        vpslld	$25, %xmm3, %xmm6
        vpxor	%xmm5, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vmovdqa	%xmm4, %xmm5
        vpsrldq	$4, %xmm5, %xmm5
        vpslldq	$12, %xmm4, %xmm4
        vpxor	%xmm4, %xmm3, %xmm3
        vpsrld	$0x01, %xmm3, %xmm6
        vpsrld	$2, %xmm3, %xmm7
        vpsrld	$7, %xmm3, %xmm4
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm5, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm6, %xmm2, %xmm2
L_AES_GCM_decrypt_avx1_calc_aad_done:
        vmovdqu	%xmm2, 96(%esp)
        movl	196(%esp), %esi
        movl	200(%esp), %edi
        # Calculate counter and H
        vpsrlq	$63, %xmm1, %xmm5
        vpsllq	$0x01, %xmm1, %xmm4
        vpslldq	$8, %xmm5, %xmm5
        vpor	%xmm5, %xmm4, %xmm4
        vpshufd	$0xff, %xmm1, %xmm1
        vpsrad	$31, %xmm1, %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm0, %xmm0
        vpand	L_aes_gcm_avx1_mod2_128, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, 64(%esp)
        xorl	%ebx, %ebx
        cmpl	$0x40, 216(%esp)
        movl	216(%esp), %eax
        jl	L_AES_GCM_decrypt_avx1_done_64
        andl	$0xffffffc0, %eax
        vmovdqa	%xmm2, %xmm6
        # H ^ 1
        vmovdqu	%xmm1, (%esp)
        # H ^ 2
        vpclmulqdq	$0x00, %xmm1, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm1, %xmm1, %xmm0
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm0, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm0, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm0, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm3
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm3, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm3
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm3, 48(%esp)
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_avx1_ghash_64
L_AES_GCM_decrypt_avx1_ghash_64_inplace:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm4
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm3
        vpaddd	L_aes_gcm_avx1_one, %xmm4, %xmm5
        vpshufb	%xmm3, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx1_two, %xmm4, %xmm6
        vpshufb	%xmm3, %xmm6, %xmm6
        vpaddd	L_aes_gcm_avx1_three, %xmm4, %xmm7
        vpshufb	%xmm3, %xmm7, %xmm7
        vpshufb	%xmm3, %xmm4, %xmm4
        vmovdqu	64(%esp), %xmm3
        vpaddd	L_aes_gcm_avx1_four, %xmm3, %xmm3
        vmovdqu	%xmm3, 64(%esp)
        vmovdqa	(%ebp), %xmm3
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_decrypt_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm3, %xmm4, %xmm4
        vaesenclast	%xmm3, %xmm5, %xmm5
        vmovdqu	(%ecx), %xmm0
        vmovdqu	16(%ecx), %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	%xmm0, 112(%esp)
        vmovdqu	%xmm1, 128(%esp)
        vmovdqu	%xmm4, (%edx)
        vmovdqu	%xmm5, 16(%edx)
        vaesenclast	%xmm3, %xmm6, %xmm6
        vaesenclast	%xmm3, %xmm7, %xmm7
        vmovdqu	32(%ecx), %xmm0
        vmovdqu	48(%ecx), %xmm1
        vpxor	%xmm0, %xmm6, %xmm6
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm0, 144(%esp)
        vmovdqu	%xmm1, 160(%esp)
        vmovdqu	%xmm6, 32(%edx)
        vmovdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        vmovdqu	96(%esp), %xmm6
        vmovdqu	48(%esp), %xmm3
        vmovdqu	112(%esp), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm3, %xmm5
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm7
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm6
        vpclmulqdq	$0x00, %xmm1, %xmm5, %xmm5
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqu	32(%esp), %xmm3
        vmovdqu	128(%esp), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	16(%esp), %xmm3
        vmovdqu	144(%esp), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	(%esp), %xmm3
        vmovdqu	160(%esp), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpslld	$31, %xmm6, %xmm3
        vpslld	$30, %xmm6, %xmm0
        vpslld	$25, %xmm6, %xmm1
        vpxor	%xmm0, %xmm3, %xmm3
        vpxor	%xmm1, %xmm3, %xmm3
        vpsrldq	$4, %xmm3, %xmm0
        vpslldq	$12, %xmm3, %xmm3
        vpxor	%xmm3, %xmm6, %xmm6
        vpsrld	$0x01, %xmm6, %xmm1
        vpsrld	$2, %xmm6, %xmm5
        vpsrld	$7, %xmm6, %xmm4
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        vmovdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx1_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_avx1_ghash_64_done
L_AES_GCM_decrypt_avx1_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm4
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm3
        vpaddd	L_aes_gcm_avx1_one, %xmm4, %xmm5
        vpshufb	%xmm3, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx1_two, %xmm4, %xmm6
        vpshufb	%xmm3, %xmm6, %xmm6
        vpaddd	L_aes_gcm_avx1_three, %xmm4, %xmm7
        vpshufb	%xmm3, %xmm7, %xmm7
        vpshufb	%xmm3, %xmm4, %xmm4
        vmovdqu	64(%esp), %xmm3
        vpaddd	L_aes_gcm_avx1_four, %xmm3, %xmm3
        vmovdqu	%xmm3, 64(%esp)
        vmovdqa	(%ebp), %xmm3
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqa	16(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	32(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	48(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	64(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	80(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	96(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	112(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	128(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	144(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	176(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm3
        jl	L_AES_GCM_decrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	208(%ebp), %xmm3
        vaesenc	%xmm3, %xmm4, %xmm4
        vaesenc	%xmm3, %xmm5, %xmm5
        vaesenc	%xmm3, %xmm6, %xmm6
        vaesenc	%xmm3, %xmm7, %xmm7
        vmovdqa	224(%ebp), %xmm3
L_AES_GCM_decrypt_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm3, %xmm4, %xmm4
        vaesenclast	%xmm3, %xmm5, %xmm5
        vmovdqu	(%ecx), %xmm0
        vmovdqu	16(%ecx), %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	%xmm0, (%ecx)
        vmovdqu	%xmm1, 16(%ecx)
        vmovdqu	%xmm4, (%edx)
        vmovdqu	%xmm5, 16(%edx)
        vaesenclast	%xmm3, %xmm6, %xmm6
        vaesenclast	%xmm3, %xmm7, %xmm7
        vmovdqu	32(%ecx), %xmm0
        vmovdqu	48(%ecx), %xmm1
        vpxor	%xmm0, %xmm6, %xmm6
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm0, 32(%ecx)
        vmovdqu	%xmm1, 48(%ecx)
        vmovdqu	%xmm6, 32(%edx)
        vmovdqu	%xmm7, 48(%edx)
        # ghash encrypted counter
        vmovdqu	96(%esp), %xmm6
        vmovdqu	48(%esp), %xmm3
        vmovdqu	(%ecx), %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm3, %xmm5
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm7
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm6
        vpclmulqdq	$0x00, %xmm1, %xmm5, %xmm5
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vmovdqu	32(%esp), %xmm3
        vmovdqu	16(%ecx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	16(%esp), %xmm3
        vmovdqu	32(%ecx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqu	(%esp), %xmm3
        vmovdqu	48(%ecx), %xmm4
        vpshufd	$0x4e, %xmm3, %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm3, %xmm4, %xmm2
        vpclmulqdq	$0x00, %xmm3, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm1, %xmm0, %xmm0
        vpxor	%xmm3, %xmm5, %xmm5
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm0, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpslld	$31, %xmm6, %xmm3
        vpslld	$30, %xmm6, %xmm0
        vpslld	$25, %xmm6, %xmm1
        vpxor	%xmm0, %xmm3, %xmm3
        vpxor	%xmm1, %xmm3, %xmm3
        vpsrldq	$4, %xmm3, %xmm0
        vpslldq	$12, %xmm3, %xmm3
        vpxor	%xmm3, %xmm6, %xmm6
        vpsrld	$0x01, %xmm6, %xmm1
        vpsrld	$2, %xmm6, %xmm5
        vpsrld	$7, %xmm6, %xmm4
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        vmovdqu	%xmm6, 96(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx1_ghash_64
L_AES_GCM_decrypt_avx1_ghash_64_done:
        vmovdqa	%xmm6, %xmm2
        vmovdqu	(%esp), %xmm1
L_AES_GCM_decrypt_avx1_done_64:
        movl	216(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_decrypt_avx1_done_dec
        movl	216(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_avx1_last_block_done
L_AES_GCM_decrypt_avx1_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	(%ecx), %xmm7
        pshufb	L_aes_gcm_avx1_bswap_mask, %xmm7
        pxor	%xmm2, %xmm7
        vmovdqu	64(%esp), %xmm5
        vmovdqu	%xmm7, %xmm7
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm5, %xmm4
        vpaddd	L_aes_gcm_avx1_one, %xmm5, %xmm5
        vmovdqu	%xmm5, 64(%esp)
        vpxor	(%ebp), %xmm4, %xmm4
        vpclmulqdq	$16, %xmm1, %xmm7, %xmm0
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x01, %xmm1, %xmm7, %xmm3
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x11, %xmm1, %xmm7, %xmm5
        vaesenc	96(%ebp), %xmm4, %xmm4
        vpxor	%xmm3, %xmm0, %xmm0
        vpslldq	$8, %xmm0, %xmm6
        vpsrldq	$8, %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm4, %xmm4
        vpclmulqdq	$0x00, %xmm1, %xmm7, %xmm3
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm0, %xmm5, %xmm5
        vmovdqa	L_aes_gcm_avx1_mod2_128, %xmm7
        vpclmulqdq	$16, %xmm7, %xmm6, %xmm3
        vaesenc	128(%ebp), %xmm4, %xmm4
        vpshufd	$0x4e, %xmm6, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpclmulqdq	$16, %xmm7, %xmm0, %xmm3
        vaesenc	144(%ebp), %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm2
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm5, %xmm2, %xmm2
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_aesenc_gfmul_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_aesenc_gfmul_last
        vaesenc	%xmm5, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_avx1_aesenc_gfmul_last:
        vaesenclast	%xmm5, %xmm4, %xmm4
        vmovdqu	(%ecx), %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vmovdqu	%xmm4, (%edx)
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx1_last_block_start
L_AES_GCM_decrypt_avx1_last_block_done:
        movl	216(%esp), %ecx
        movl	%ecx, %edx
        andl	$15, %ecx
        jz	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_done
        vmovdqu	64(%esp), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm0, %xmm0
        vpxor	(%ebp), %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 236(%esp)
        vmovdqa	160(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	%xmm5, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 236(%esp)
        vmovdqa	192(%ebp), %xmm5
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	%xmm5, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm5
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last:
        vaesenclast	%xmm5, %xmm0, %xmm0
        subl	$32, %esp
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
        vpxor	%xmm4, %xmm4, %xmm4
        vmovdqu	%xmm4, 16(%esp)
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        movb	%al, 16(%esp,%ecx,1)
        xorb	(%esp,%ecx,1), %al
        movb	%al, (%edi,%ebx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_loop
        vmovdqu	16(%esp), %xmm0
        addl	$32, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm2, %xmm2
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm2, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm2, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm2, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm2, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm2
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_done:
L_AES_GCM_decrypt_avx1_done_dec:
        movl	212(%esp), %esi
        movl	228(%esp), %ebp
        movl	216(%esp), %edx
        movl	220(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        vpinsrd	$0x00, %edx, %xmm4, %xmm4
        vpinsrd	$2, %ecx, %xmm4, %xmm4
        movl	216(%esp), %edx
        movl	220(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        vpinsrd	$0x01, %edx, %xmm4, %xmm4
        vpinsrd	$3, %ecx, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm1, %xmm5
        vpshufd	$0x4e, %xmm2, %xmm6
        vpclmulqdq	$0x11, %xmm1, %xmm2, %xmm7
        vpclmulqdq	$0x00, %xmm1, %xmm2, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm2, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm6
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm6, %xmm4, %xmm4
        vpxor	%xmm5, %xmm7, %xmm2
        vpslld	$31, %xmm4, %xmm5
        vpslld	$30, %xmm4, %xmm6
        vpslld	$25, %xmm4, %xmm7
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm7, %xmm5, %xmm5
        vpsrldq	$4, %xmm5, %xmm7
        vpslldq	$12, %xmm5, %xmm5
        vpxor	%xmm5, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm5
        vpsrld	$2, %xmm4, %xmm6
        vpxor	%xmm6, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpsrld	$7, %xmm4, %xmm4
        vpxor	%xmm7, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm2, %xmm2
        vpxor	80(%esp), %xmm2, %xmm4
        movl	240(%esp), %edi
        cmpl	$16, %ebp
        je	L_AES_GCM_decrypt_avx1_cmp_tag_16
        subl	$16, %esp
        xorl	%ecx, %ecx
        xorl	%ebx, %ebx
        vmovdqu	%xmm4, (%esp)
L_AES_GCM_decrypt_avx1_cmp_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        xorb	(%esi,%ecx,1), %al
        orb	%al, %bl
        incl	%ecx
        cmpl	%ebp, %ecx
        jne	L_AES_GCM_decrypt_avx1_cmp_tag_loop
        cmpb	$0x00, %bl
        sete	%bl
        addl	$16, %esp
        xorl	%ecx, %ecx
        jmp	L_AES_GCM_decrypt_avx1_cmp_tag_done
L_AES_GCM_decrypt_avx1_cmp_tag_16:
        vmovdqu	(%esi), %xmm5
        vpcmpeqb	%xmm5, %xmm4, %xmm4
        vpmovmskb	%xmm4, %edx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%ebx, %ebx
        cmpl	$0xffff, %edx
        sete	%bl
L_AES_GCM_decrypt_avx1_cmp_tag_done:
        movl	%ebx, (%edi)
        addl	$0xb0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_avx1,.-AES_GCM_decrypt_avx1
#ifdef WOLFSSL_AESGCM_STREAM
.text
.globl	AES_GCM_init_avx1
.type	AES_GCM_init_avx1,@function
.align	16
AES_GCM_init_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	36(%esp), %ebp
        movl	44(%esp), %esi
        movl	60(%esp), %edi
        vpxor	%xmm4, %xmm4, %xmm4
        movl	48(%esp), %edx
        cmpl	$12, %edx
        jne	L_AES_GCM_init_avx1_iv_not_12
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        movl	$0x1000000, %ecx
        vpinsrd	$0x00, (%esi), %xmm4, %xmm4
        vpinsrd	$0x01, 4(%esi), %xmm4, %xmm4
        vpinsrd	$2, 8(%esi), %xmm4, %xmm4
        vpinsrd	$3, %ecx, %xmm4, %xmm4
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	(%ebp), %xmm5
        vpxor	%xmm5, %xmm4, %xmm1
        vmovdqa	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        cmpl	$11, 40(%esp)
        vmovdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_init_avx1_calc_iv_12_last
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        cmpl	$13, 40(%esp)
        vmovdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_init_avx1_calc_iv_12_last
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm1, %xmm1
        vmovdqa	224(%ebp), %xmm7
L_AES_GCM_init_avx1_calc_iv_12_last:
        vaesenclast	%xmm7, %xmm5, %xmm5
        vaesenclast	%xmm7, %xmm1, %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm5, %xmm5
        vmovdqu	%xmm1, (%edi)
        jmp	L_AES_GCM_init_avx1_iv_done
L_AES_GCM_init_avx1_iv_not_12:
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqa	(%ebp), %xmm5
        vaesenc	16(%ebp), %xmm5, %xmm5
        vaesenc	32(%ebp), %xmm5, %xmm5
        vaesenc	48(%ebp), %xmm5, %xmm5
        vaesenc	64(%ebp), %xmm5, %xmm5
        vaesenc	80(%ebp), %xmm5, %xmm5
        vaesenc	96(%ebp), %xmm5, %xmm5
        vaesenc	112(%ebp), %xmm5, %xmm5
        vaesenc	128(%ebp), %xmm5, %xmm5
        vaesenc	144(%ebp), %xmm5, %xmm5
        cmpl	$11, 40(%esp)
        vmovdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm1, %xmm5, %xmm5
        vaesenc	176(%ebp), %xmm5, %xmm5
        cmpl	$13, 40(%esp)
        vmovdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm1, %xmm5, %xmm5
        vaesenc	208(%ebp), %xmm5, %xmm5
        vmovdqa	224(%ebp), %xmm1
L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm1, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm5, %xmm5
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_init_avx1_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_init_avx1_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_init_avx1_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm4, %xmm1
        vpshufd	$0x4e, %xmm5, %xmm2
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm7
        vmovdqa	%xmm3, %xmm4
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm1, %xmm4, %xmm4
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        vpslld	$31, %xmm7, %xmm0
        vpslld	$30, %xmm7, %xmm1
        vpslld	$25, %xmm7, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm7, %xmm7
        vpsrld	$0x01, %xmm7, %xmm2
        vpsrld	$2, %xmm7, %xmm3
        vpsrld	$7, %xmm7, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm2, %xmm4, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_avx1_calc_iv_16_loop
        movl	48(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_init_avx1_calc_iv_done
L_AES_GCM_init_avx1_calc_iv_lt16:
        subl	$16, %esp
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_init_avx1_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_avx1_calc_iv_loop
        vmovdqu	(%esp), %xmm0
        addl	$16, %esp
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm4, %xmm1
        vpshufd	$0x4e, %xmm5, %xmm2
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm7
        vmovdqa	%xmm3, %xmm4
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm1, %xmm4, %xmm4
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        vpslld	$31, %xmm7, %xmm0
        vpslld	$30, %xmm7, %xmm1
        vpslld	$25, %xmm7, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm7, %xmm7
        vpsrld	$0x01, %xmm7, %xmm2
        vpsrld	$2, %xmm7, %xmm3
        vpsrld	$7, %xmm7, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm2, %xmm4, %xmm4
L_AES_GCM_init_avx1_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm0, %xmm0, %xmm0
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm4, %xmm1
        vpshufd	$0x4e, %xmm5, %xmm2
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm7
        vmovdqa	%xmm3, %xmm4
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm7, %xmm7
        vpxor	%xmm1, %xmm4, %xmm4
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        vpslld	$31, %xmm7, %xmm0
        vpslld	$30, %xmm7, %xmm1
        vpslld	$25, %xmm7, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm7, %xmm7
        vpsrld	$0x01, %xmm7, %xmm2
        vpsrld	$2, %xmm7, %xmm3
        vpsrld	$7, %xmm7, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm2, %xmm4, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        #   Encrypt counter
        vmovdqa	(%ebp), %xmm0
        vpxor	%xmm4, %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 40(%esp)
        vmovdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 40(%esp)
        vmovdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm1
L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edi)
L_AES_GCM_init_avx1_iv_done:
        movl	52(%esp), %ebp
        movl	56(%esp), %edi
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm4, %xmm4
        vpaddd	L_aes_gcm_avx1_one, %xmm4, %xmm4
        vmovdqa	%xmm5, (%ebp)
        vmovdqa	%xmm4, (%edi)
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_init_avx1,.-AES_GCM_init_avx1
.text
.globl	AES_GCM_aad_update_avx1
.type	AES_GCM_aad_update_avx1,@function
.align	16
AES_GCM_aad_update_avx1:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %esi
        movl	16(%esp), %edx
        movl	20(%esp), %edi
        movl	24(%esp), %eax
        vmovdqa	(%edi), %xmm5
        vmovdqa	(%eax), %xmm6
        xorl	%ecx, %ecx
L_AES_GCM_aad_update_avx1_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm5, %xmm5
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm6, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm6, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm4
        vmovdqa	%xmm3, %xmm5
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm4, %xmm4
        vpxor	%xmm1, %xmm5, %xmm5
        vpsrld	$31, %xmm4, %xmm0
        vpsrld	$31, %xmm5, %xmm1
        vpslld	$0x01, %xmm4, %xmm4
        vpslld	$0x01, %xmm5, %xmm5
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm5, %xmm5
        vpor	%xmm0, %xmm4, %xmm4
        vpor	%xmm1, %xmm5, %xmm5
        vpslld	$31, %xmm4, %xmm0
        vpslld	$30, %xmm4, %xmm1
        vpslld	$25, %xmm4, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm2
        vpsrld	$2, %xmm4, %xmm3
        vpsrld	$7, %xmm4, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm2, %xmm5, %xmm5
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_aad_update_avx1_16_loop
        vmovdqa	%xmm5, (%edi)
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_aad_update_avx1,.-AES_GCM_aad_update_avx1
.text
.globl	AES_GCM_encrypt_block_avx1
.type	AES_GCM_encrypt_block_avx1,@function
.align	16
AES_GCM_encrypt_block_avx1:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %ecx
        movl	16(%esp), %eax
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        vmovdqu	(%edx), %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx1_one, %xmm1, %xmm1
        vmovdqu	%xmm1, (%edx)
        vpxor	(%ecx), %xmm0, %xmm0
        vaesenc	16(%ecx), %xmm0, %xmm0
        vaesenc	32(%ecx), %xmm0, %xmm0
        vaesenc	48(%ecx), %xmm0, %xmm0
        vaesenc	64(%ecx), %xmm0, %xmm0
        vaesenc	80(%ecx), %xmm0, %xmm0
        vaesenc	96(%ecx), %xmm0, %xmm0
        vaesenc	112(%ecx), %xmm0, %xmm0
        vaesenc	128(%ecx), %xmm0, %xmm0
        vaesenc	144(%ecx), %xmm0, %xmm0
        cmpl	$11, %eax
        vmovdqa	160(%ecx), %xmm1
        jl	L_AES_GCM_encrypt_block_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	176(%ecx), %xmm0, %xmm0
        cmpl	$13, %eax
        vmovdqa	192(%ecx), %xmm1
        jl	L_AES_GCM_encrypt_block_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	208(%ecx), %xmm0, %xmm0
        vmovdqa	224(%ecx), %xmm1
L_AES_GCM_encrypt_block_avx1_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm1, %xmm0, %xmm0
        vmovdqu	(%esi), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edi)
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_block_avx1,.-AES_GCM_encrypt_block_avx1
.text
.globl	AES_GCM_ghash_block_avx1
.type	AES_GCM_ghash_block_avx1,@function
.align	16
AES_GCM_ghash_block_avx1:
        movl	4(%esp), %edx
        movl	8(%esp), %eax
        movl	12(%esp), %ecx
        vmovdqa	(%eax), %xmm4
        vmovdqa	(%ecx), %xmm5
        vmovdqu	(%edx), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm4, %xmm1
        vpshufd	$0x4e, %xmm5, %xmm2
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm6
        vmovdqa	%xmm3, %xmm4
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm6, %xmm6
        vpxor	%xmm1, %xmm4, %xmm4
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        vpslld	$31, %xmm6, %xmm0
        vpslld	$30, %xmm6, %xmm1
        vpslld	$25, %xmm6, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        vpsrld	$0x01, %xmm6, %xmm2
        vpsrld	$2, %xmm6, %xmm3
        vpsrld	$7, %xmm6, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm6, %xmm2, %xmm2
        vpxor	%xmm2, %xmm4, %xmm4
        vmovdqa	%xmm4, (%eax)
        ret
.size	AES_GCM_ghash_block_avx1,.-AES_GCM_ghash_block_avx1
.text
.globl	AES_GCM_encrypt_update_avx1
.type	AES_GCM_encrypt_update_avx1,@function
.align	16
AES_GCM_encrypt_update_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x60, %esp
        movl	144(%esp), %esi
        vmovdqa	(%esi), %xmm4
        vmovdqu	%xmm4, 64(%esp)
        movl	136(%esp), %esi
        movl	140(%esp), %ebp
        vmovdqa	(%esi), %xmm6
        vmovdqa	(%ebp), %xmm5
        vmovdqu	%xmm6, 80(%esp)
        movl	116(%esp), %ebp
        movl	124(%esp), %edi
        movl	128(%esp), %esi
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx1_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 132(%esp)
        movl	132(%esp), %eax
        jl	L_AES_GCM_encrypt_update_avx1_done_64
        andl	$0xffffffc0, %eax
        vmovdqa	%xmm6, %xmm2
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        # H ^ 2
        vpclmulqdq	$0x00, %xmm5, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm5, %xmm5, %xmm4
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vmovdqu	%xmm4, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm4, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm0, %xmm0
        vpxor	%xmm1, %xmm3, %xmm7
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm7, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm4, %xmm4, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm4, %xmm7
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm7, 48(%esp)
        # First 64 bytes of input
        vmovdqu	64(%esp), %xmm0
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm1
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_two, %xmm0, %xmm2
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx1_three, %xmm0, %xmm3
        vpshufb	%xmm7, %xmm3, %xmm3
        vpshufb	%xmm7, %xmm0, %xmm0
        vmovdqu	64(%esp), %xmm7
        vpaddd	L_aes_gcm_avx1_four, %xmm7, %xmm7
        vmovdqu	%xmm7, 64(%esp)
        vmovdqa	(%ebp), %xmm7
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqa	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 120(%esp)
        vmovdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 120(%esp)
        vmovdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_avx1_aesenc_64_enc_done:
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vmovdqu	(%esi), %xmm4
        vmovdqu	16(%esi), %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vmovdqu	%xmm4, (%esi)
        vmovdqu	%xmm5, 16(%esi)
        vmovdqu	%xmm0, (%edi)
        vmovdqu	%xmm1, 16(%edi)
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%esi), %xmm4
        vmovdqu	48(%esi), %xmm5
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm4, 32(%esi)
        vmovdqu	%xmm5, 48(%esi)
        vmovdqu	%xmm2, 32(%edi)
        vmovdqu	%xmm3, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        movl	%esi, %ecx
        movl	%edi, %edx
        jle	L_AES_GCM_encrypt_update_avx1_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_update_avx1_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm0
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm1
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_two, %xmm0, %xmm2
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx1_three, %xmm0, %xmm3
        vpshufb	%xmm7, %xmm3, %xmm3
        vpshufb	%xmm7, %xmm0, %xmm0
        vmovdqu	64(%esp), %xmm7
        vpaddd	L_aes_gcm_avx1_four, %xmm7, %xmm7
        vmovdqu	%xmm7, 64(%esp)
        vmovdqa	(%ebp), %xmm7
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqa	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 120(%esp)
        vmovdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 120(%esp)
        vmovdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vmovdqu	(%ecx), %xmm4
        vmovdqu	16(%ecx), %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ecx), %xmm4
        vmovdqu	48(%ecx), %xmm5
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        vmovdqu	80(%esp), %xmm2
        vmovdqu	48(%esp), %xmm7
        vmovdqu	-64(%edx), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm2
        vpclmulqdq	$0x00, %xmm5, %xmm1, %xmm1
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqu	32(%esp), %xmm7
        vmovdqu	-48(%edx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	16(%esp), %xmm7
        vmovdqu	-32(%edx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	(%esp), %xmm7
        vmovdqu	-16(%edx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm5
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm1, %xmm3, %xmm3
        vpslld	$31, %xmm2, %xmm7
        vpslld	$30, %xmm2, %xmm4
        vpslld	$25, %xmm2, %xmm5
        vpxor	%xmm4, %xmm7, %xmm7
        vpxor	%xmm5, %xmm7, %xmm7
        vpsrldq	$4, %xmm7, %xmm4
        vpslldq	$12, %xmm7, %xmm7
        vpxor	%xmm7, %xmm2, %xmm2
        vpsrld	$0x01, %xmm2, %xmm5
        vpsrld	$2, %xmm2, %xmm1
        vpsrld	$7, %xmm2, %xmm0
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm3, %xmm2, %xmm2
        vmovdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_avx1_ghash_64
L_AES_GCM_encrypt_update_avx1_end_64:
        movdqu	80(%esp), %xmm6
        # Block 1
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm0
        vmovdqu	(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        vmovdqu	48(%esp), %xmm7
        pxor	%xmm6, %xmm5
        # ghash_gfmul_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm7, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm7, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm7, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqa	%xmm0, %xmm4
        vmovdqa	%xmm3, %xmm6
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm4, %xmm4
        vpxor	%xmm1, %xmm6, %xmm6
        # Block 2
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm0
        vmovdqu	16(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        vmovdqu	32(%esp), %xmm7
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm7, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm7, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm7, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm3, %xmm6, %xmm6
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm4, %xmm4
        vpxor	%xmm1, %xmm6, %xmm6
        # Block 3
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm0
        vmovdqu	32(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        vmovdqu	16(%esp), %xmm7
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm7, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm7, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm7, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm3, %xmm6, %xmm6
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm4, %xmm4
        vpxor	%xmm1, %xmm6, %xmm6
        # Block 4
        vmovdqa	L_aes_gcm_avx1_bswap_mask, %xmm0
        vmovdqu	48(%edx), %xmm5
        pshufb	%xmm0, %xmm5
        vmovdqu	(%esp), %xmm7
        # ghash_gfmul_xor_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm7, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm7, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm7, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm4, %xmm4
        vpxor	%xmm3, %xmm6, %xmm6
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm4, %xmm4
        vpxor	%xmm1, %xmm6, %xmm6
        vpslld	$31, %xmm4, %xmm0
        vpslld	$30, %xmm4, %xmm1
        vpslld	$25, %xmm4, %xmm2
        vpxor	%xmm1, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vmovdqa	%xmm0, %xmm1
        vpsrldq	$4, %xmm1, %xmm1
        vpslldq	$12, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        vpsrld	$0x01, %xmm4, %xmm2
        vpsrld	$2, %xmm4, %xmm3
        vpsrld	$7, %xmm4, %xmm0
        vpxor	%xmm3, %xmm2, %xmm2
        vpxor	%xmm0, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm2, %xmm6, %xmm6
        vmovdqu	(%esp), %xmm5
L_AES_GCM_encrypt_update_avx1_done_64:
        movl	132(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_encrypt_update_avx1_done_enc
        movl	132(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_avx1_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx1_one, %xmm1, %xmm1
        vmovdqu	%xmm1, 64(%esp)
        vpxor	(%ebp), %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 120(%esp)
        vmovdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 120(%esp)
        vmovdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_block_aesenc_avx_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm1
L_AES_GCM_encrypt_update_avx1_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm1, %xmm0, %xmm0
        vmovdqu	(%ecx), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edx)
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_avx1_last_block_ghash
L_AES_GCM_encrypt_update_avx1_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm1
        vmovdqu	%xmm6, %xmm3
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx1_one, %xmm1, %xmm1
        vmovdqu	%xmm1, 64(%esp)
        vpxor	(%ebp), %xmm0, %xmm0
        vpclmulqdq	$16, %xmm5, %xmm3, %xmm4
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x01, %xmm5, %xmm3, %xmm7
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x11, %xmm5, %xmm3, %xmm1
        vaesenc	96(%ebp), %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpslldq	$8, %xmm4, %xmm2
        vpsrldq	$8, %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x00, %xmm5, %xmm3, %xmm7
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqa	L_aes_gcm_avx1_mod2_128, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm2, %xmm7
        vaesenc	128(%ebp), %xmm0, %xmm0
        vpshufd	$0x4e, %xmm2, %xmm4
        vpxor	%xmm7, %xmm4, %xmm4
        vpclmulqdq	$16, %xmm3, %xmm4, %xmm7
        vaesenc	144(%ebp), %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        cmpl	$11, 120(%esp)
        vmovdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 120(%esp)
        vmovdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm1
L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last:
        vaesenclast	%xmm1, %xmm0, %xmm0
        vmovdqu	(%ecx), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edx)
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        addl	$16, %ebx
        vpxor	%xmm0, %xmm6, %xmm6
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_avx1_last_block_start
L_AES_GCM_encrypt_update_avx1_last_block_ghash:
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm6, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm6, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm0, %xmm0
        vpxor	%xmm1, %xmm3, %xmm6
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
L_AES_GCM_encrypt_update_avx1_last_block_done:
L_AES_GCM_encrypt_update_avx1_done_enc:
        movl	136(%esp), %esi
        movl	144(%esp), %edi
        vmovdqu	64(%esp), %xmm4
        vmovdqa	%xmm6, (%esi)
        vmovdqu	%xmm4, (%edi)
        addl	$0x60, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt_update_avx1,.-AES_GCM_encrypt_update_avx1
.text
.globl	AES_GCM_encrypt_final_avx1
.type	AES_GCM_encrypt_final_avx1,@function
.align	16
AES_GCM_encrypt_final_avx1:
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	32(%esp), %ebp
        movl	52(%esp), %esi
        movl	56(%esp), %edi
        vmovdqa	(%ebp), %xmm4
        vmovdqa	(%esi), %xmm5
        vmovdqa	(%edi), %xmm6
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx1_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        movl	44(%esp), %edx
        movl	48(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	44(%esp), %edx
        movl	48(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        vpinsrd	$0x01, %edx, %xmm0, %xmm0
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm4, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm0, %xmm0
        vpxor	%xmm1, %xmm3, %xmm4
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm6, %xmm4, %xmm0
        movl	36(%esp), %edi
        cmpl	$16, 40(%esp)
        je	L_AES_GCM_encrypt_final_avx1_store_tag_16
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_final_avx1_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	40(%esp), %ecx
        jne	L_AES_GCM_encrypt_final_avx1_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_avx1_store_tag_done
L_AES_GCM_encrypt_final_avx1_store_tag_16:
        vmovdqu	%xmm0, (%edi)
L_AES_GCM_encrypt_final_avx1_store_tag_done:
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_final_avx1,.-AES_GCM_encrypt_final_avx1
.text
.globl	AES_GCM_decrypt_update_avx1
.type	AES_GCM_decrypt_update_avx1,@function
.align	16
AES_GCM_decrypt_update_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xa0, %esp
        movl	208(%esp), %esi
        vmovdqa	(%esi), %xmm4
        vmovdqu	%xmm4, 64(%esp)
        movl	200(%esp), %esi
        movl	204(%esp), %ebp
        vmovdqa	(%esi), %xmm6
        vmovdqa	(%ebp), %xmm5
        vmovdqu	%xmm6, 80(%esp)
        movl	180(%esp), %ebp
        movl	188(%esp), %edi
        movl	192(%esp), %esi
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx1_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 196(%esp)
        movl	196(%esp), %eax
        jl	L_AES_GCM_decrypt_update_avx1_done_64
        andl	$0xffffffc0, %eax
        vmovdqa	%xmm6, %xmm2
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        # H ^ 2
        vpclmulqdq	$0x00, %xmm5, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm5, %xmm5, %xmm4
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vmovdqu	%xmm4, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm4, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm0, %xmm0
        vpxor	%xmm1, %xmm3, %xmm7
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm7, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm4, %xmm4, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm4, %xmm7
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        vmovdqu	%xmm7, 48(%esp)
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_update_avx1_ghash_64
L_AES_GCM_decrypt_update_avx1_ghash_64_inplace:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm0
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm1
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_two, %xmm0, %xmm2
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx1_three, %xmm0, %xmm3
        vpshufb	%xmm7, %xmm3, %xmm3
        vpshufb	%xmm7, %xmm0, %xmm0
        vmovdqu	64(%esp), %xmm7
        vpaddd	L_aes_gcm_avx1_four, %xmm7, %xmm7
        vmovdqu	%xmm7, 64(%esp)
        vmovdqa	(%ebp), %xmm7
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqa	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 184(%esp)
        vmovdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 184(%esp)
        vmovdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_avx1inplace_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vmovdqu	(%ecx), %xmm4
        vmovdqu	16(%ecx), %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vmovdqu	%xmm4, 96(%esp)
        vmovdqu	%xmm5, 112(%esp)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ecx), %xmm4
        vmovdqu	48(%ecx), %xmm5
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm4, 128(%esp)
        vmovdqu	%xmm5, 144(%esp)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        vmovdqu	80(%esp), %xmm2
        vmovdqu	48(%esp), %xmm7
        vmovdqu	96(%esp), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm2
        vpclmulqdq	$0x00, %xmm5, %xmm1, %xmm1
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqu	32(%esp), %xmm7
        vmovdqu	112(%esp), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	16(%esp), %xmm7
        vmovdqu	128(%esp), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	(%esp), %xmm7
        vmovdqu	144(%esp), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm5
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm1, %xmm3, %xmm3
        vpslld	$31, %xmm2, %xmm7
        vpslld	$30, %xmm2, %xmm4
        vpslld	$25, %xmm2, %xmm5
        vpxor	%xmm4, %xmm7, %xmm7
        vpxor	%xmm5, %xmm7, %xmm7
        vpsrldq	$4, %xmm7, %xmm4
        vpslldq	$12, %xmm7, %xmm7
        vpxor	%xmm7, %xmm2, %xmm2
        vpsrld	$0x01, %xmm2, %xmm5
        vpsrld	$2, %xmm2, %xmm1
        vpsrld	$7, %xmm2, %xmm0
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm3, %xmm2, %xmm2
        vmovdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx1_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_update_avx1_ghash_64_done
L_AES_GCM_decrypt_update_avx1_ghash_64:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	64(%esp), %xmm0
        vmovdqa	L_aes_gcm_avx1_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx1_one, %xmm0, %xmm1
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx1_two, %xmm0, %xmm2
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx1_three, %xmm0, %xmm3
        vpshufb	%xmm7, %xmm3, %xmm3
        vpshufb	%xmm7, %xmm0, %xmm0
        vmovdqu	64(%esp), %xmm7
        vpaddd	L_aes_gcm_avx1_four, %xmm7, %xmm7
        vmovdqu	%xmm7, 64(%esp)
        vmovdqa	(%ebp), %xmm7
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqa	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 184(%esp)
        vmovdqa	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 184(%esp)
        vmovdqa	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqa	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_avx1_aesenc_64_ghash_avx_aesenc_64_enc_done:
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vmovdqu	(%ecx), %xmm4
        vmovdqu	16(%ecx), %xmm5
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vmovdqu	%xmm4, (%ecx)
        vmovdqu	%xmm5, 16(%ecx)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ecx), %xmm4
        vmovdqu	48(%ecx), %xmm5
        vpxor	%xmm4, %xmm2, %xmm2
        vpxor	%xmm5, %xmm3, %xmm3
        vmovdqu	%xmm4, 32(%ecx)
        vmovdqu	%xmm5, 48(%ecx)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # ghash encrypted counter
        vmovdqu	80(%esp), %xmm2
        vmovdqu	48(%esp), %xmm7
        vmovdqu	(%ecx), %xmm0
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm2
        vpclmulqdq	$0x00, %xmm5, %xmm1, %xmm1
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vmovdqu	32(%esp), %xmm7
        vmovdqu	16(%ecx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	16(%esp), %xmm7
        vmovdqu	32(%ecx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	(%esp), %xmm7
        vmovdqu	48(%ecx), %xmm0
        vpshufd	$0x4e, %xmm7, %xmm4
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpshufd	$0x4e, %xmm0, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm7, %xmm0, %xmm6
        vpclmulqdq	$0x00, %xmm7, %xmm0, %xmm7
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm4
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm6, %xmm3, %xmm3
        vpxor	%xmm4, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm5
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm1, %xmm3, %xmm3
        vpslld	$31, %xmm2, %xmm7
        vpslld	$30, %xmm2, %xmm4
        vpslld	$25, %xmm2, %xmm5
        vpxor	%xmm4, %xmm7, %xmm7
        vpxor	%xmm5, %xmm7, %xmm7
        vpsrldq	$4, %xmm7, %xmm4
        vpslldq	$12, %xmm7, %xmm7
        vpxor	%xmm7, %xmm2, %xmm2
        vpsrld	$0x01, %xmm2, %xmm5
        vpsrld	$2, %xmm2, %xmm1
        vpsrld	$7, %xmm2, %xmm0
        vpxor	%xmm1, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm2, %xmm2
        vpxor	%xmm3, %xmm2, %xmm2
        vmovdqu	%xmm2, 80(%esp)
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx1_ghash_64
L_AES_GCM_decrypt_update_avx1_ghash_64_done:
        vmovdqa	%xmm2, %xmm6
        vmovdqu	(%esp), %xmm5
L_AES_GCM_decrypt_update_avx1_done_64:
        movl	196(%esp), %edx
        cmpl	%edx, %ebx
        jge	L_AES_GCM_decrypt_update_avx1_done_dec
        movl	196(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_update_avx1_last_block_done
L_AES_GCM_decrypt_update_avx1_last_block_start:
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        vmovdqu	(%ecx), %xmm1
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm6, %xmm1, %xmm1
        vmovdqu	%xmm1, (%esp)
        vmovdqu	64(%esp), %xmm1
        vmovdqu	(%esp), %xmm3
        vpshufb	L_aes_gcm_avx1_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx1_one, %xmm1, %xmm1
        vmovdqu	%xmm1, 64(%esp)
        vpxor	(%ebp), %xmm0, %xmm0
        vpclmulqdq	$16, %xmm5, %xmm3, %xmm4
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x01, %xmm5, %xmm3, %xmm7
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x11, %xmm5, %xmm3, %xmm1
        vaesenc	96(%ebp), %xmm0, %xmm0
        vpxor	%xmm7, %xmm4, %xmm4
        vpslldq	$8, %xmm4, %xmm2
        vpsrldq	$8, %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm0, %xmm0
        vpclmulqdq	$0x00, %xmm5, %xmm3, %xmm7
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqa	L_aes_gcm_avx1_mod2_128, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm2, %xmm7
        vaesenc	128(%ebp), %xmm0, %xmm0
        vpshufd	$0x4e, %xmm2, %xmm4
        vpxor	%xmm7, %xmm4, %xmm4
        vpclmulqdq	$16, %xmm3, %xmm4, %xmm7
        vaesenc	144(%ebp), %xmm0, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        cmpl	$11, 184(%esp)
        vmovdqa	160(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 184(%esp)
        vmovdqa	192(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last
        vaesenc	%xmm1, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqa	224(%ebp), %xmm1
L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last:
        vaesenclast	%xmm1, %xmm0, %xmm0
        vmovdqu	(%ecx), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edx)
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx1_last_block_start
L_AES_GCM_decrypt_update_avx1_last_block_done:
L_AES_GCM_decrypt_update_avx1_done_dec:
        movl	200(%esp), %esi
        movl	208(%esp), %edi
        vmovdqu	64(%esp), %xmm4
        vmovdqa	%xmm6, (%esi)
        vmovdqu	%xmm4, (%edi)
        addl	$0xa0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_update_avx1,.-AES_GCM_decrypt_update_avx1
.text
.globl	AES_GCM_decrypt_final_avx1
.type	AES_GCM_decrypt_final_avx1,@function
.align	16
AES_GCM_decrypt_final_avx1:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	36(%esp), %ebp
        movl	56(%esp), %esi
        movl	60(%esp), %edi
        vmovdqa	(%ebp), %xmm6
        vmovdqa	(%esi), %xmm5
        vmovdqa	(%edi), %xmm7
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx1_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        movl	48(%esp), %edx
        movl	52(%esp), %ecx
        shll	$3, %edx
        shll	$3, %ecx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	48(%esp), %edx
        movl	52(%esp), %ecx
        shrl	$29, %edx
        shrl	$29, %ecx
        vpinsrd	$0x01, %edx, %xmm0, %xmm0
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        # ghash_gfmul_red_avx
        vpshufd	$0x4e, %xmm5, %xmm1
        vpshufd	$0x4e, %xmm6, %xmm2
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm6, %xmm2, %xmm2
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpslldq	$8, %xmm1, %xmm2
        vpsrldq	$8, %xmm1, %xmm1
        vpxor	%xmm2, %xmm0, %xmm0
        vpxor	%xmm1, %xmm3, %xmm6
        vpslld	$31, %xmm0, %xmm1
        vpslld	$30, %xmm0, %xmm2
        vpslld	$25, %xmm0, %xmm3
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm3, %xmm1, %xmm1
        vpsrldq	$4, %xmm1, %xmm3
        vpslldq	$12, %xmm1, %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vpsrld	$0x01, %xmm0, %xmm1
        vpsrld	$2, %xmm0, %xmm2
        vpxor	%xmm2, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpsrld	$7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vpshufb	L_aes_gcm_avx1_bswap_mask, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm0
        movl	40(%esp), %esi
        movl	64(%esp), %edi
        cmpl	$16, 44(%esp)
        je	L_AES_GCM_decrypt_final_avx1_cmp_tag_16
        subl	$16, %esp
        xorl	%ecx, %ecx
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_final_avx1_cmp_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        xorb	(%esi,%ecx,1), %al
        orb	%al, %bl
        incl	%ecx
        cmpl	44(%esp), %ecx
        jne	L_AES_GCM_decrypt_final_avx1_cmp_tag_loop
        cmpb	$0x00, %bl
        sete	%bl
        addl	$16, %esp
        xorl	%ecx, %ecx
        jmp	L_AES_GCM_decrypt_final_avx1_cmp_tag_done
L_AES_GCM_decrypt_final_avx1_cmp_tag_16:
        vmovdqu	(%esi), %xmm1
        vpcmpeqb	%xmm1, %xmm0, %xmm0
        vpmovmskb	%xmm0, %edx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%ebx, %ebx
        cmpl	$0xffff, %edx
        sete	%bl
L_AES_GCM_decrypt_final_avx1_cmp_tag_done:
        movl	%ebx, (%edi)
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_final_avx1,.-AES_GCM_decrypt_final_avx1
#endif /* WOLFSSL_AESGCM_STREAM */
#endif /* HAVE_INTEL_AVX1 */
#ifdef HAVE_INTEL_AVX2
.text
.globl	AES_GCM_encrypt_avx2
.type	AES_GCM_encrypt_avx2,@function
.align	16
AES_GCM_encrypt_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x70, %esp
        movl	144(%esp), %esi
        movl	168(%esp), %ebp
        movl	160(%esp), %edx
        vpxor	%xmm4, %xmm4, %xmm4
        cmpl	$12, %edx
        je	L_AES_GCM_encrypt_avx2_iv_12
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqu	(%ebp), %xmm5
        vaesenc	16(%ebp), %xmm5, %xmm5
        vaesenc	32(%ebp), %xmm5, %xmm5
        vaesenc	48(%ebp), %xmm5, %xmm5
        vaesenc	64(%ebp), %xmm5, %xmm5
        vaesenc	80(%ebp), %xmm5, %xmm5
        vaesenc	96(%ebp), %xmm5, %xmm5
        vaesenc	112(%ebp), %xmm5, %xmm5
        vaesenc	128(%ebp), %xmm5, %xmm5
        vaesenc	144(%ebp), %xmm5, %xmm5
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	176(%ebp), %xmm5, %xmm5
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	208(%ebp), %xmm5, %xmm5
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_encrypt_avx2_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_avx2_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_avx2_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx2_calc_iv_16_loop
        movl	160(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_avx2_calc_iv_done
L_AES_GCM_encrypt_avx2_calc_iv_lt16:
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_avx2_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx2_calc_iv_loop
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
L_AES_GCM_encrypt_avx2_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm0, %xmm0, %xmm0
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm4, %xmm4
        #   Encrypt counter
        vmovdqu	(%ebp), %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vaesenc	16(%ebp), %xmm6, %xmm6
        vaesenc	32(%ebp), %xmm6, %xmm6
        vaesenc	48(%ebp), %xmm6, %xmm6
        vaesenc	64(%ebp), %xmm6, %xmm6
        vaesenc	80(%ebp), %xmm6, %xmm6
        vaesenc	96(%ebp), %xmm6, %xmm6
        vaesenc	112(%ebp), %xmm6, %xmm6
        vaesenc	128(%ebp), %xmm6, %xmm6
        vaesenc	144(%ebp), %xmm6, %xmm6
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm6, %xmm6
        vaesenc	176(%ebp), %xmm6, %xmm6
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm6, %xmm6
        vaesenc	208(%ebp), %xmm6, %xmm6
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm6, %xmm6
        jmp	L_AES_GCM_encrypt_avx2_iv_done
L_AES_GCM_encrypt_avx2_iv_12:
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        vmovdqu	L_avx2_aes_gcm_bswap_one, %xmm4
        vmovdqu	(%ebp), %xmm5
        vpblendd	$7, (%esi), %xmm4, %xmm4
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	16(%ebp), %xmm7
        vpxor	%xmm5, %xmm4, %xmm6
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm6, %xmm6
        vmovdqu	32(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	48(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	64(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	80(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	96(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	112(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	128(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	144(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	176(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	208(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_avx2_calc_iv_12_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vaesenclast	%xmm0, %xmm6, %xmm6
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
L_AES_GCM_encrypt_avx2_iv_done:
        vmovdqu	%xmm6, 80(%esp)
        vpxor	%xmm6, %xmm6, %xmm6
        movl	140(%esp), %esi
        # Additional authentication data
        movl	156(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_encrypt_avx2_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_encrypt_avx2_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_encrypt_avx2_calc_aad_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm6, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm6, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm6, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm6
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm6, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm6, %xmm6
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm6, %xmm6
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx2_calc_aad_16_loop
        movl	156(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_encrypt_avx2_calc_aad_done
L_AES_GCM_encrypt_avx2_calc_aad_lt16:
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_avx2_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_encrypt_avx2_calc_aad_loop
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm6, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm6, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm6, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm6
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm6, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm6, %xmm6
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm6, %xmm6
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
L_AES_GCM_encrypt_avx2_calc_aad_done:
        movl	132(%esp), %esi
        movl	136(%esp), %edi
        # Calculate counter and H
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm4
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 152(%esp)
        movl	152(%esp), %eax
        jl	L_AES_GCM_encrypt_avx2_done_64
        andl	$0xffffffc0, %eax
        vmovdqu	%xmm4, 64(%esp)
        vmovdqu	%xmm6, 96(%esp)
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm3
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        vmovdqu	%xmm5, %xmm2
        # H ^ 2
        vpclmulqdq	$0x00, %xmm2, %xmm2, %xmm5
        vpclmulqdq	$0x11, %xmm2, %xmm2, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm0, %xmm2, %xmm6
        vpclmulqdq	$0x01, %xmm0, %xmm2, %xmm5
        vpclmulqdq	$0x00, %xmm0, %xmm2, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        vpslldq	$8, %xmm6, %xmm5
        vpsrldq	$8, %xmm6, %xmm6
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm2, %xmm1
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm1, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm2
        vmovdqu	%xmm2, 48(%esp)
        vmovdqu	96(%esp), %xmm6
        # First 64 bytes of input
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_avx2_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_avx2_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_encrypt_avx2_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%esi), %xmm7
        vmovdqu	16(%esi), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, (%edi)
        vmovdqu	%xmm1, 16(%edi)
        vmovdqu	32(%esi), %xmm7
        vmovdqu	48(%esi), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm2, 32(%edi)
        vmovdqu	%xmm3, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        movl	%esi, %ecx
        movl	%edi, %edx
        jle	L_AES_GCM_encrypt_avx2_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_avx2_ghash_64:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_encrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	-64(%edx), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	-48(%edx), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	-32(%edx), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	-16(%edx), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_avx2_ghash_64
L_AES_GCM_encrypt_avx2_end_64:
        vmovdqu	%xmm6, 96(%esp)
        vmovdqu	48(%edx), %xmm3
        vmovdqu	(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm5
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm4
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm6
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	32(%edx), %xmm3
        vmovdqu	16(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vmovdqu	16(%edx), %xmm3
        vmovdqu	32(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vmovdqu	96(%esp), %xmm0
        vmovdqu	(%edx), %xmm3
        vmovdqu	48(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpxor	%xmm0, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vpslldq	$8, %xmm5, %xmm7
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm7, %xmm4, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm4, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vmovdqu	(%esp), %xmm5
        vmovdqu	64(%esp), %xmm4
L_AES_GCM_encrypt_avx2_done_64:
        cmpl	152(%esp), %ebx
        je	L_AES_GCM_encrypt_avx2_done_enc
        movl	152(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_avx2_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_block
        vmovdqu	%xmm4, %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx2_one, %xmm1, %xmm1
        vpxor	(%ebp), %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm2
        jl	L_AES_GCM_encrypt_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm2
        jl	L_AES_GCM_encrypt_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqu	224(%ebp), %xmm2
L_AES_GCM_encrypt_avx2_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm2, %xmm0, %xmm0
        vmovdqu	%xmm1, %xmm4
        vmovdqu	(%ecx), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edx)
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_avx2_last_block_ghash
L_AES_GCM_encrypt_avx2_last_block_start:
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vmovdqu	%xmm4, 64(%esp)
        # aesenc_gfmul_sb
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm4
        vpxor	(%ebp), %xmm7, %xmm7
        vaesenc	16(%ebp), %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpslldq	$8, %xmm3, %xmm2
        vpsrldq	$8, %xmm3, %xmm3
        vaesenc	32(%ebp), %xmm7, %xmm7
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	48(%ebp), %xmm7, %xmm7
        vaesenc	64(%ebp), %xmm7, %xmm7
        vaesenc	80(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	96(%ebp), %xmm7, %xmm7
        vaesenc	112(%ebp), %xmm7, %xmm7
        vaesenc	128(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vaesenc	144(%ebp), %xmm7, %xmm7
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        vmovdqu	160(%ebp), %xmm0
        cmpl	$11, 172(%esp)
        jl	L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	176(%ebp), %xmm7, %xmm7
        vmovdqu	192(%ebp), %xmm0
        cmpl	$13, 172(%esp)
        jl	L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	208(%ebp), %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last:
        vaesenclast	%xmm0, %xmm7, %xmm7
        vmovdqu	(%esi,%ebx,1), %xmm3
        vpxor	%xmm1, %xmm2, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqu	%xmm7, (%edi,%ebx,1)
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm7, %xmm7
        vpxor	%xmm7, %xmm6, %xmm6
        vmovdqu	64(%esp), %xmm4
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_avx2_last_block_start
L_AES_GCM_encrypt_avx2_last_block_ghash:
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm6
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm2, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm0, %xmm6, %xmm6
L_AES_GCM_encrypt_avx2_last_block_done:
        movl	152(%esp), %ecx
        movl	152(%esp), %edx
        andl	$15, %ecx
        jz	L_AES_GCM_encrypt_avx2_done_enc
        # aesenc_last15_enc
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm4
        vpxor	(%ebp), %xmm4, %xmm4
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vaesenc	96(%ebp), %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm4, %xmm4
        vaesenc	128(%ebp), %xmm4, %xmm4
        vaesenc	144(%ebp), %xmm4, %xmm4
        cmpl	$11, 172(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	%xmm0, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 172(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	%xmm0, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm4, %xmm4
        xorl	%ecx, %ecx
        vpxor	%xmm0, %xmm0, %xmm0
        vmovdqu	%xmm4, (%esp)
        vmovdqu	%xmm0, 16(%esp)
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        xorb	(%esp,%ecx,1), %al
        movb	%al, 16(%esp,%ecx,1)
        movb	%al, (%edi,%ebx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_loop
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_finish_enc:
        vmovdqu	16(%esp), %xmm4
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm6, %xmm6
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm6
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm2, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm0, %xmm6, %xmm6
L_AES_GCM_encrypt_avx2_done_enc:
        vmovdqu	80(%esp), %xmm7
        # calc_tag
        movl	152(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$0x00, %ecx, %xmm0, %xmm0
        movl	156(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	152(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$0x01, %ecx, %xmm0, %xmm0
        movl	156(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm6, %xmm0, %xmm0
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm0, %xmm4
        vpclmulqdq	$0x01, %xmm5, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm0, %xmm2
        vpxor	%xmm3, %xmm4, %xmm4
        vpslldq	$8, %xmm4, %xmm3
        vpsrldq	$8, %xmm4, %xmm4
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$0x11, %xmm5, %xmm0, %xmm0
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm0, %xmm0
        movl	148(%esp), %edi
        movl	164(%esp), %ebx
        # store_tag
        cmpl	$16, %ebx
        je	L_AES_GCM_encrypt_avx2_store_tag_16
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_avx2_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	%ebx, %ecx
        jne	L_AES_GCM_encrypt_avx2_store_tag_loop
        jmp	L_AES_GCM_encrypt_avx2_store_tag_done
L_AES_GCM_encrypt_avx2_store_tag_16:
        vmovdqu	%xmm0, (%edi)
L_AES_GCM_encrypt_avx2_store_tag_done:
        addl	$0x70, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt_avx2,.-AES_GCM_encrypt_avx2
.text
.globl	AES_GCM_decrypt_avx2
.type	AES_GCM_decrypt_avx2,@function
.align	16
AES_GCM_decrypt_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xb0, %esp
        movl	208(%esp), %esi
        movl	232(%esp), %ebp
        vpxor	%xmm4, %xmm4, %xmm4
        movl	224(%esp), %edx
        cmpl	$12, %edx
        je	L_AES_GCM_decrypt_avx2_iv_12
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqu	(%ebp), %xmm5
        vaesenc	16(%ebp), %xmm5, %xmm5
        vaesenc	32(%ebp), %xmm5, %xmm5
        vaesenc	48(%ebp), %xmm5, %xmm5
        vaesenc	64(%ebp), %xmm5, %xmm5
        vaesenc	80(%ebp), %xmm5, %xmm5
        vaesenc	96(%ebp), %xmm5, %xmm5
        vaesenc	112(%ebp), %xmm5, %xmm5
        vaesenc	128(%ebp), %xmm5, %xmm5
        vaesenc	144(%ebp), %xmm5, %xmm5
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	176(%ebp), %xmm5, %xmm5
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	208(%ebp), %xmm5, %xmm5
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_decrypt_avx2_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_avx2_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_avx2_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx2_calc_iv_16_loop
        movl	224(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_avx2_calc_iv_done
L_AES_GCM_decrypt_avx2_calc_iv_lt16:
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_avx2_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx2_calc_iv_loop
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
L_AES_GCM_decrypt_avx2_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm0, %xmm0, %xmm0
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm4, %xmm4
        #   Encrypt counter
        vmovdqu	(%ebp), %xmm6
        vpxor	%xmm4, %xmm6, %xmm6
        vaesenc	16(%ebp), %xmm6, %xmm6
        vaesenc	32(%ebp), %xmm6, %xmm6
        vaesenc	48(%ebp), %xmm6, %xmm6
        vaesenc	64(%ebp), %xmm6, %xmm6
        vaesenc	80(%ebp), %xmm6, %xmm6
        vaesenc	96(%ebp), %xmm6, %xmm6
        vaesenc	112(%ebp), %xmm6, %xmm6
        vaesenc	128(%ebp), %xmm6, %xmm6
        vaesenc	144(%ebp), %xmm6, %xmm6
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm6, %xmm6
        vaesenc	176(%ebp), %xmm6, %xmm6
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm6, %xmm6
        vaesenc	208(%ebp), %xmm6, %xmm6
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm6, %xmm6
        jmp	L_AES_GCM_decrypt_avx2_iv_done
L_AES_GCM_decrypt_avx2_iv_12:
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        vmovdqu	L_avx2_aes_gcm_bswap_one, %xmm4
        vmovdqu	(%ebp), %xmm5
        vpblendd	$7, (%esi), %xmm4, %xmm4
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	16(%ebp), %xmm7
        vpxor	%xmm5, %xmm4, %xmm6
        vaesenc	%xmm7, %xmm5, %xmm5
        vaesenc	%xmm7, %xmm6, %xmm6
        vmovdqu	32(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	48(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	64(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	80(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	96(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	112(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	128(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	144(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	176(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_decrypt_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	208(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm6, %xmm6
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_decrypt_avx2_calc_iv_12_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vaesenclast	%xmm0, %xmm6, %xmm6
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
L_AES_GCM_decrypt_avx2_iv_done:
        vmovdqu	%xmm6, 80(%esp)
        vpxor	%xmm6, %xmm6, %xmm6
        movl	204(%esp), %esi
        # Additional authentication data
        movl	220(%esp), %edx
        cmpl	$0x00, %edx
        je	L_AES_GCM_decrypt_avx2_calc_aad_done
        xorl	%ecx, %ecx
        cmpl	$16, %edx
        jl	L_AES_GCM_decrypt_avx2_calc_aad_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_decrypt_avx2_calc_aad_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm6, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm6, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm6, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm6
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm6, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm6, %xmm6
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm6, %xmm6
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx2_calc_aad_16_loop
        movl	220(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_decrypt_avx2_calc_aad_done
L_AES_GCM_decrypt_avx2_calc_aad_lt16:
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_avx2_calc_aad_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_decrypt_avx2_calc_aad_loop
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm6, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm6, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm6, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm6, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm7
        vpxor	%xmm2, %xmm3, %xmm6
        # ghash_mid
        vpsrld	$31, %xmm7, %xmm0
        vpsrld	$31, %xmm6, %xmm1
        vpslld	$0x01, %xmm7, %xmm7
        vpslld	$0x01, %xmm6, %xmm6
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm6, %xmm6
        vpor	%xmm0, %xmm7, %xmm7
        vpor	%xmm1, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm7, %xmm0
        vpshufd	$0x4e, %xmm7, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
L_AES_GCM_decrypt_avx2_calc_aad_done:
        movl	196(%esp), %esi
        movl	200(%esp), %edi
        # Calculate counter and H
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm4
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 216(%esp)
        movl	216(%esp), %eax
        jl	L_AES_GCM_decrypt_avx2_done_64
        andl	$0xffffffc0, %eax
        vmovdqu	%xmm4, 64(%esp)
        vmovdqu	%xmm6, 96(%esp)
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm3
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        vmovdqu	%xmm5, %xmm2
        # H ^ 2
        vpclmulqdq	$0x00, %xmm2, %xmm2, %xmm5
        vpclmulqdq	$0x11, %xmm2, %xmm2, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm0, %xmm2, %xmm6
        vpclmulqdq	$0x01, %xmm0, %xmm2, %xmm5
        vpclmulqdq	$0x00, %xmm0, %xmm2, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        vpslldq	$8, %xmm6, %xmm5
        vpsrldq	$8, %xmm6, %xmm6
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm2, %xmm1
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm1, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm2
        vmovdqu	%xmm2, 48(%esp)
        vmovdqu	96(%esp), %xmm6
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_avx2_ghash_64
L_AES_GCM_decrypt_avx2_ghash_64_inplace:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_decrypt_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm7, 112(%esp)
        vmovdqu	%xmm4, 128(%esp)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm7, 144(%esp)
        vmovdqu	%xmm4, 160(%esp)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	112(%esp), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	128(%esp), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	144(%esp), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	160(%esp), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx2_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_avx2_ghash_64_done
L_AES_GCM_decrypt_avx2_ghash_64:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_decrypt_avx2_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm7, (%ecx)
        vmovdqu	%xmm4, 16(%ecx)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm7, 32(%ecx)
        vmovdqu	%xmm4, 48(%ecx)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	(%ecx), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	16(%ecx), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	32(%ecx), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	48(%ecx), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx2_ghash_64
L_AES_GCM_decrypt_avx2_ghash_64_done:
        vmovdqu	(%esp), %xmm5
        vmovdqu	64(%esp), %xmm4
L_AES_GCM_decrypt_avx2_done_64:
        cmpl	216(%esp), %ebx
        jge	L_AES_GCM_decrypt_avx2_done_dec
        movl	216(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_avx2_last_block_done
L_AES_GCM_decrypt_avx2_last_block_start:
        vmovdqu	(%esi,%ebx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm6, %xmm0, %xmm4
        # aesenc_gfmul_sb
        vpclmulqdq	$0x01, %xmm5, %xmm4, %xmm2
        vpclmulqdq	$16, %xmm5, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm4, %xmm4
        vpxor	(%ebp), %xmm7, %xmm7
        vaesenc	16(%ebp), %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpslldq	$8, %xmm3, %xmm2
        vpsrldq	$8, %xmm3, %xmm3
        vaesenc	32(%ebp), %xmm7, %xmm7
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	48(%ebp), %xmm7, %xmm7
        vaesenc	64(%ebp), %xmm7, %xmm7
        vaesenc	80(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	96(%ebp), %xmm7, %xmm7
        vaesenc	112(%ebp), %xmm7, %xmm7
        vaesenc	128(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vaesenc	144(%ebp), %xmm7, %xmm7
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        vmovdqu	160(%ebp), %xmm0
        cmpl	$11, 236(%esp)
        jl	L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	176(%ebp), %xmm7, %xmm7
        vmovdqu	192(%ebp), %xmm0
        cmpl	$13, 236(%esp)
        jl	L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	208(%ebp), %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last:
        vaesenclast	%xmm0, %xmm7, %xmm7
        vmovdqu	(%esi,%ebx,1), %xmm3
        vpxor	%xmm1, %xmm2, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqu	%xmm7, (%edi,%ebx,1)
        vmovdqu	64(%esp), %xmm4
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_avx2_last_block_start
L_AES_GCM_decrypt_avx2_last_block_done:
        movl	216(%esp), %ecx
        movl	216(%esp), %edx
        andl	$15, %ecx
        jz	L_AES_GCM_decrypt_avx2_done_dec
        # aesenc_last15_dec
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm4
        vpxor	(%ebp), %xmm4, %xmm4
        vaesenc	16(%ebp), %xmm4, %xmm4
        vaesenc	32(%ebp), %xmm4, %xmm4
        vaesenc	48(%ebp), %xmm4, %xmm4
        vaesenc	64(%ebp), %xmm4, %xmm4
        vaesenc	80(%ebp), %xmm4, %xmm4
        vaesenc	96(%ebp), %xmm4, %xmm4
        vaesenc	112(%ebp), %xmm4, %xmm4
        vaesenc	128(%ebp), %xmm4, %xmm4
        vaesenc	144(%ebp), %xmm4, %xmm4
        cmpl	$11, 236(%esp)
        vmovdqu	160(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	%xmm1, %xmm4, %xmm4
        vaesenc	176(%ebp), %xmm4, %xmm4
        cmpl	$13, 236(%esp)
        vmovdqu	192(%ebp), %xmm1
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	%xmm1, %xmm4, %xmm4
        vaesenc	208(%ebp), %xmm4, %xmm4
        vmovdqu	224(%ebp), %xmm1
L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last:
        vaesenclast	%xmm1, %xmm4, %xmm4
        xorl	%ecx, %ecx
        vpxor	%xmm0, %xmm0, %xmm0
        vmovdqu	%xmm4, (%esp)
        vmovdqu	%xmm0, 16(%esp)
L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_loop:
        movzbl	(%esi,%ebx,1), %eax
        movb	%al, 16(%esp,%ecx,1)
        xorb	(%esp,%ecx,1), %al
        movb	%al, (%edi,%ebx,1)
        incl	%ebx
        incl	%ecx
        cmpl	%edx, %ebx
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_loop
        vmovdqu	16(%esp), %xmm4
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm4, %xmm4
        vpxor	%xmm4, %xmm6, %xmm6
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm6
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm2, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm0, %xmm6, %xmm6
L_AES_GCM_decrypt_avx2_done_dec:
        vmovdqu	80(%esp), %xmm7
        # calc_tag
        movl	216(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$0x00, %ecx, %xmm0, %xmm0
        movl	220(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	216(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$0x01, %ecx, %xmm0, %xmm0
        movl	220(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm6, %xmm0, %xmm0
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm0, %xmm4
        vpclmulqdq	$0x01, %xmm5, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm0, %xmm2
        vpxor	%xmm3, %xmm4, %xmm4
        vpslldq	$8, %xmm4, %xmm3
        vpsrldq	$8, %xmm4, %xmm4
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$0x11, %xmm5, %xmm0, %xmm0
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm4, %xmm0, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm7, %xmm0, %xmm0
        movl	212(%esp), %edi
        movl	228(%esp), %ebx
        movl	240(%esp), %ebp
        # cmp_tag
        cmpl	$16, %ebx
        je	L_AES_GCM_decrypt_avx2_cmp_tag_16
        xorl	%edx, %edx
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_avx2_cmp_tag_loop:
        movzbl	(%esp,%edx,1), %eax
        xorb	(%edi,%edx,1), %al
        orb	%al, %cl
        incl	%edx
        cmpl	%ebx, %edx
        jne	L_AES_GCM_decrypt_avx2_cmp_tag_loop
        cmpb	$0x00, %cl
        sete	%cl
        jmp	L_AES_GCM_decrypt_avx2_cmp_tag_done
L_AES_GCM_decrypt_avx2_cmp_tag_16:
        vmovdqu	(%edi), %xmm1
        vpcmpeqb	%xmm1, %xmm0, %xmm0
        vpmovmskb	%xmm0, %edx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%ecx, %ecx
        cmpl	$0xffff, %edx
        sete	%cl
L_AES_GCM_decrypt_avx2_cmp_tag_done:
        movl	%ecx, (%ebp)
        addl	$0xb0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_avx2,.-AES_GCM_decrypt_avx2
#ifdef WOLFSSL_AESGCM_STREAM
.text
.globl	AES_GCM_init_avx2
.type	AES_GCM_init_avx2,@function
.align	16
AES_GCM_init_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$32, %esp
        movl	52(%esp), %ebp
        movl	60(%esp), %esi
        movl	76(%esp), %edi
        vpxor	%xmm4, %xmm4, %xmm4
        movl	64(%esp), %edx
        cmpl	$12, %edx
        je	L_AES_GCM_init_avx2_iv_12
        # Calculate values when IV is not 12 bytes
        # H = Encrypt X(=0)
        vmovdqu	(%ebp), %xmm5
        vaesenc	16(%ebp), %xmm5, %xmm5
        vaesenc	32(%ebp), %xmm5, %xmm5
        vaesenc	48(%ebp), %xmm5, %xmm5
        vaesenc	64(%ebp), %xmm5, %xmm5
        vaesenc	80(%ebp), %xmm5, %xmm5
        vaesenc	96(%ebp), %xmm5, %xmm5
        vaesenc	112(%ebp), %xmm5, %xmm5
        vaesenc	128(%ebp), %xmm5, %xmm5
        vaesenc	144(%ebp), %xmm5, %xmm5
        cmpl	$11, 56(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	176(%ebp), %xmm5, %xmm5
        cmpl	$13, 56(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	208(%ebp), %xmm5, %xmm5
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
        # Calc counter
        # Initialization vector
        cmpl	$0x00, %edx
        movl	$0x00, %ecx
        je	L_AES_GCM_init_avx2_calc_iv_done
        cmpl	$16, %edx
        jl	L_AES_GCM_init_avx2_calc_iv_lt16
        andl	$0xfffffff0, %edx
L_AES_GCM_init_avx2_calc_iv_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm6
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm6, %xmm0
        vpshufd	$0x4e, %xmm6, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_avx2_calc_iv_16_loop
        movl	64(%esp), %edx
        cmpl	%edx, %ecx
        je	L_AES_GCM_init_avx2_calc_iv_done
L_AES_GCM_init_avx2_calc_iv_lt16:
        vpxor	%xmm0, %xmm0, %xmm0
        xorl	%ebx, %ebx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_init_avx2_calc_iv_loop:
        movzbl	(%esi,%ecx,1), %eax
        movb	%al, (%esp,%ebx,1)
        incl	%ecx
        incl	%ebx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_init_avx2_calc_iv_loop
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm6
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm6, %xmm0
        vpshufd	$0x4e, %xmm6, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
L_AES_GCM_init_avx2_calc_iv_done:
        # T = Encrypt counter
        vpxor	%xmm0, %xmm0, %xmm0
        shll	$3, %edx
        vpinsrd	$0x00, %edx, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm6
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm6, %xmm0
        vpshufd	$0x4e, %xmm6, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm4, %xmm4
        #   Encrypt counter
        vmovdqu	(%ebp), %xmm7
        vpxor	%xmm4, %xmm7, %xmm7
        vaesenc	16(%ebp), %xmm7, %xmm7
        vaesenc	32(%ebp), %xmm7, %xmm7
        vaesenc	48(%ebp), %xmm7, %xmm7
        vaesenc	64(%ebp), %xmm7, %xmm7
        vaesenc	80(%ebp), %xmm7, %xmm7
        vaesenc	96(%ebp), %xmm7, %xmm7
        vaesenc	112(%ebp), %xmm7, %xmm7
        vaesenc	128(%ebp), %xmm7, %xmm7
        vaesenc	144(%ebp), %xmm7, %xmm7
        cmpl	$11, 56(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	176(%ebp), %xmm7, %xmm7
        cmpl	$13, 56(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	208(%ebp), %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	%xmm0, %xmm7, %xmm7
        jmp	L_AES_GCM_init_avx2_iv_done
L_AES_GCM_init_avx2_iv_12:
        # # Calculate values when IV is 12 bytes
        # Set counter based on IV
        vmovdqu	L_avx2_aes_gcm_bswap_one, %xmm4
        vmovdqu	(%ebp), %xmm5
        vpblendd	$7, (%esi), %xmm4, %xmm4
        # H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	16(%ebp), %xmm6
        vpxor	%xmm5, %xmm4, %xmm7
        vaesenc	%xmm6, %xmm5, %xmm5
        vaesenc	%xmm6, %xmm7, %xmm7
        vmovdqu	32(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	48(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	64(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	80(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	96(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	112(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	128(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	144(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        cmpl	$11, 56(%esp)
        vmovdqu	160(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	176(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        cmpl	$13, 56(%esp)
        vmovdqu	192(%ebp), %xmm0
        jl	L_AES_GCM_init_avx2_calc_iv_12_last
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	208(%ebp), %xmm0
        vaesenc	%xmm0, %xmm5, %xmm5
        vaesenc	%xmm0, %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_init_avx2_calc_iv_12_last:
        vaesenclast	%xmm0, %xmm5, %xmm5
        vaesenclast	%xmm0, %xmm7, %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm5, %xmm5
L_AES_GCM_init_avx2_iv_done:
        vmovdqu	%xmm7, (%edi)
        movl	68(%esp), %ebp
        movl	72(%esp), %edi
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm4
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vmovdqu	%xmm5, (%ebp)
        vmovdqu	%xmm4, (%edi)
        addl	$32, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_init_avx2,.-AES_GCM_init_avx2
.text
.globl	AES_GCM_aad_update_avx2
.type	AES_GCM_aad_update_avx2,@function
.align	16
AES_GCM_aad_update_avx2:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %esi
        movl	16(%esp), %edx
        movl	20(%esp), %edi
        movl	24(%esp), %eax
        vmovdqu	(%edi), %xmm4
        vmovdqu	(%eax), %xmm5
        xorl	%ecx, %ecx
L_AES_GCM_aad_update_avx2_16_loop:
        vmovdqu	(%esi,%ecx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm6
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm6, %xmm0
        vpshufd	$0x4e, %xmm6, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        addl	$16, %ecx
        cmpl	%edx, %ecx
        jl	L_AES_GCM_aad_update_avx2_16_loop
        vmovdqu	%xmm4, (%edi)
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_aad_update_avx2,.-AES_GCM_aad_update_avx2
.text
.globl	AES_GCM_encrypt_block_avx2
.type	AES_GCM_encrypt_block_avx2,@function
.align	16
AES_GCM_encrypt_block_avx2:
        pushl	%esi
        pushl	%edi
        movl	12(%esp), %ecx
        movl	16(%esp), %eax
        movl	20(%esp), %edi
        movl	24(%esp), %esi
        movl	28(%esp), %edx
        vmovdqu	(%edx), %xmm3
        # aesenc_block
        vmovdqu	%xmm3, %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx2_one, %xmm1, %xmm1
        vpxor	(%ecx), %xmm0, %xmm0
        vaesenc	16(%ecx), %xmm0, %xmm0
        vaesenc	32(%ecx), %xmm0, %xmm0
        vaesenc	48(%ecx), %xmm0, %xmm0
        vaesenc	64(%ecx), %xmm0, %xmm0
        vaesenc	80(%ecx), %xmm0, %xmm0
        vaesenc	96(%ecx), %xmm0, %xmm0
        vaesenc	112(%ecx), %xmm0, %xmm0
        vaesenc	128(%ecx), %xmm0, %xmm0
        vaesenc	144(%ecx), %xmm0, %xmm0
        cmpl	$11, %eax
        vmovdqu	160(%ecx), %xmm2
        jl	L_AES_GCM_encrypt_block_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	176(%ecx), %xmm0, %xmm0
        cmpl	$13, %eax
        vmovdqu	192(%ecx), %xmm2
        jl	L_AES_GCM_encrypt_block_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	208(%ecx), %xmm0, %xmm0
        vmovdqu	224(%ecx), %xmm2
L_AES_GCM_encrypt_block_avx2_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm2, %xmm0, %xmm0
        vmovdqu	%xmm1, %xmm3
        vmovdqu	(%esi), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edi)
        vmovdqu	%xmm3, (%edx)
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_block_avx2,.-AES_GCM_encrypt_block_avx2
.text
.globl	AES_GCM_ghash_block_avx2
.type	AES_GCM_ghash_block_avx2,@function
.align	16
AES_GCM_ghash_block_avx2:
        movl	4(%esp), %edx
        movl	8(%esp), %eax
        movl	12(%esp), %ecx
        vmovdqu	(%eax), %xmm4
        vmovdqu	(%ecx), %xmm5
        vmovdqu	(%edx), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm4, %xmm4
        # ghash_gfmul_avx
        vpclmulqdq	$16, %xmm4, %xmm5, %xmm2
        vpclmulqdq	$0x01, %xmm4, %xmm5, %xmm1
        vpclmulqdq	$0x00, %xmm4, %xmm5, %xmm0
        vpclmulqdq	$0x11, %xmm4, %xmm5, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm1, %xmm0, %xmm6
        vpxor	%xmm2, %xmm3, %xmm4
        # ghash_mid
        vpsrld	$31, %xmm6, %xmm0
        vpsrld	$31, %xmm4, %xmm1
        vpslld	$0x01, %xmm6, %xmm6
        vpslld	$0x01, %xmm4, %xmm4
        vpsrldq	$12, %xmm0, %xmm2
        vpslldq	$4, %xmm0, %xmm0
        vpslldq	$4, %xmm1, %xmm1
        vpor	%xmm2, %xmm4, %xmm4
        vpor	%xmm0, %xmm6, %xmm6
        vpor	%xmm1, %xmm4, %xmm4
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm6, %xmm0
        vpshufd	$0x4e, %xmm6, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm4, %xmm4
        vmovdqu	%xmm4, (%eax)
        ret
.size	AES_GCM_ghash_block_avx2,.-AES_GCM_ghash_block_avx2
.text
.globl	AES_GCM_encrypt_update_avx2
.type	AES_GCM_encrypt_update_avx2,@function
.align	16
AES_GCM_encrypt_update_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0x60, %esp
        movl	144(%esp), %esi
        vmovdqu	(%esi), %xmm4
        vmovdqu	%xmm4, 64(%esp)
        movl	136(%esp), %esi
        movl	140(%esp), %ebp
        vmovdqu	(%esi), %xmm6
        vmovdqu	(%ebp), %xmm5
        vmovdqu	%xmm6, 80(%esp)
        movl	116(%esp), %ebp
        movl	124(%esp), %edi
        movl	128(%esp), %esi
        # Calculate H
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 132(%esp)
        movl	132(%esp), %eax
        jl	L_AES_GCM_encrypt_update_avx2_done_64
        andl	$0xffffffc0, %eax
        vmovdqu	%xmm4, 64(%esp)
        vmovdqu	%xmm6, 80(%esp)
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm3
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        vmovdqu	%xmm5, %xmm2
        # H ^ 2
        vpclmulqdq	$0x00, %xmm2, %xmm2, %xmm5
        vpclmulqdq	$0x11, %xmm2, %xmm2, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm0, %xmm2, %xmm6
        vpclmulqdq	$0x01, %xmm0, %xmm2, %xmm5
        vpclmulqdq	$0x00, %xmm0, %xmm2, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        vpslldq	$8, %xmm6, %xmm5
        vpsrldq	$8, %xmm6, %xmm6
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm2, %xmm1
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm1, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm2
        vmovdqu	%xmm2, 48(%esp)
        vmovdqu	80(%esp), %xmm6
        # First 64 bytes of input
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 120(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 120(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_avx2_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%esi), %xmm7
        vmovdqu	16(%esi), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, (%edi)
        vmovdqu	%xmm1, 16(%edi)
        vmovdqu	32(%esi), %xmm7
        vmovdqu	48(%esi), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm2, 32(%edi)
        vmovdqu	%xmm3, 48(%edi)
        cmpl	$0x40, %eax
        movl	$0x40, %ebx
        movl	%esi, %ecx
        movl	%edi, %edx
        jle	L_AES_GCM_encrypt_update_avx2_end_64
        # More 64 bytes of input
L_AES_GCM_encrypt_update_avx2_ghash_64:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 120(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 120(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_encrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	-64(%edx), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	-48(%edx), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	-32(%edx), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	-16(%edx), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_avx2_ghash_64
L_AES_GCM_encrypt_update_avx2_end_64:
        vmovdqu	%xmm6, 80(%esp)
        vmovdqu	48(%edx), %xmm3
        vmovdqu	(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm5
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm4
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm6
        vpxor	%xmm1, %xmm5, %xmm5
        vmovdqu	32(%edx), %xmm3
        vmovdqu	16(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vmovdqu	16(%edx), %xmm3
        vmovdqu	32(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vmovdqu	80(%esp), %xmm0
        vmovdqu	(%edx), %xmm3
        vmovdqu	48(%esp), %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm3, %xmm3
        vpxor	%xmm0, %xmm3, %xmm3
        vpclmulqdq	$16, %xmm3, %xmm7, %xmm2
        vpclmulqdq	$0x01, %xmm3, %xmm7, %xmm1
        vpclmulqdq	$0x00, %xmm3, %xmm7, %xmm0
        vpclmulqdq	$0x11, %xmm3, %xmm7, %xmm3
        vpxor	%xmm1, %xmm2, %xmm2
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm0, %xmm4, %xmm4
        vpslldq	$8, %xmm5, %xmm7
        vpsrldq	$8, %xmm5, %xmm5
        vpxor	%xmm7, %xmm4, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        # ghash_red
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm2
        vpclmulqdq	$16, %xmm2, %xmm4, %xmm0
        vpshufd	$0x4e, %xmm4, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm6, %xmm6
        vmovdqu	(%esp), %xmm5
        vmovdqu	64(%esp), %xmm4
L_AES_GCM_encrypt_update_avx2_done_64:
        cmpl	132(%esp), %ebx
        je	L_AES_GCM_encrypt_update_avx2_done_enc
        movl	132(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_avx2_last_block_done
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_block
        vmovdqu	%xmm4, %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm1, %xmm0
        vpaddd	L_aes_gcm_avx2_one, %xmm1, %xmm1
        vpxor	(%ebp), %xmm0, %xmm0
        vaesenc	16(%ebp), %xmm0, %xmm0
        vaesenc	32(%ebp), %xmm0, %xmm0
        vaesenc	48(%ebp), %xmm0, %xmm0
        vaesenc	64(%ebp), %xmm0, %xmm0
        vaesenc	80(%ebp), %xmm0, %xmm0
        vaesenc	96(%ebp), %xmm0, %xmm0
        vaesenc	112(%ebp), %xmm0, %xmm0
        vaesenc	128(%ebp), %xmm0, %xmm0
        vaesenc	144(%ebp), %xmm0, %xmm0
        cmpl	$11, 120(%esp)
        vmovdqu	160(%ebp), %xmm2
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	176(%ebp), %xmm0, %xmm0
        cmpl	$13, 120(%esp)
        vmovdqu	192(%ebp), %xmm2
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_block_aesenc_avx_last
        vaesenc	%xmm2, %xmm0, %xmm0
        vaesenc	208(%ebp), %xmm0, %xmm0
        vmovdqu	224(%ebp), %xmm2
L_AES_GCM_encrypt_update_avx2_aesenc_block_aesenc_avx_last:
        vaesenclast	%xmm2, %xmm0, %xmm0
        vmovdqu	%xmm1, %xmm4
        vmovdqu	(%ecx), %xmm1
        vpxor	%xmm1, %xmm0, %xmm0
        vmovdqu	%xmm0, (%edx)
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm0, %xmm6, %xmm6
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jge	L_AES_GCM_encrypt_update_avx2_last_block_ghash
L_AES_GCM_encrypt_update_avx2_last_block_start:
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vmovdqu	%xmm4, 64(%esp)
        # aesenc_gfmul_sb
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm4
        vpxor	(%ebp), %xmm7, %xmm7
        vaesenc	16(%ebp), %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpslldq	$8, %xmm3, %xmm2
        vpsrldq	$8, %xmm3, %xmm3
        vaesenc	32(%ebp), %xmm7, %xmm7
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	48(%ebp), %xmm7, %xmm7
        vaesenc	64(%ebp), %xmm7, %xmm7
        vaesenc	80(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	96(%ebp), %xmm7, %xmm7
        vaesenc	112(%ebp), %xmm7, %xmm7
        vaesenc	128(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vaesenc	144(%ebp), %xmm7, %xmm7
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        vmovdqu	160(%ebp), %xmm0
        cmpl	$11, 120(%esp)
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	176(%ebp), %xmm7, %xmm7
        vmovdqu	192(%ebp), %xmm0
        cmpl	$13, 120(%esp)
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	208(%ebp), %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last:
        vaesenclast	%xmm0, %xmm7, %xmm7
        vmovdqu	(%esi,%ebx,1), %xmm3
        vpxor	%xmm1, %xmm2, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqu	%xmm7, (%edi,%ebx,1)
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm7, %xmm7
        vpxor	%xmm7, %xmm6, %xmm6
        vmovdqu	64(%esp), %xmm4
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_encrypt_update_avx2_last_block_start
L_AES_GCM_encrypt_update_avx2_last_block_ghash:
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm6, %xmm2
        vpclmulqdq	$0x01, %xmm5, %xmm6, %xmm1
        vpclmulqdq	$0x00, %xmm5, %xmm6, %xmm0
        vpxor	%xmm1, %xmm2, %xmm2
        vpslldq	$8, %xmm2, %xmm1
        vpsrldq	$8, %xmm2, %xmm2
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm6, %xmm6
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm0, %xmm1, %xmm1
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm1, %xmm0
        vpshufd	$0x4e, %xmm1, %xmm1
        vpxor	%xmm2, %xmm6, %xmm6
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm0, %xmm6, %xmm6
L_AES_GCM_encrypt_update_avx2_last_block_done:
L_AES_GCM_encrypt_update_avx2_done_enc:
        movl	136(%esp), %esi
        movl	144(%esp), %edi
        vmovdqu	%xmm6, (%esi)
        vmovdqu	%xmm4, (%edi)
        addl	$0x60, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_encrypt_update_avx2,.-AES_GCM_encrypt_update_avx2
.text
.globl	AES_GCM_encrypt_final_avx2
.type	AES_GCM_encrypt_final_avx2,@function
.align	16
AES_GCM_encrypt_final_avx2:
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	32(%esp), %ebp
        movl	52(%esp), %esi
        movl	56(%esp), %edi
        vmovdqu	(%ebp), %xmm4
        vmovdqu	(%esi), %xmm5
        vmovdqu	(%edi), %xmm6
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        # calc_tag
        movl	44(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$0x00, %ecx, %xmm0, %xmm0
        movl	48(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	44(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$0x01, %ecx, %xmm0, %xmm0
        movl	48(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm0, %xmm7
        vpclmulqdq	$0x01, %xmm5, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm0, %xmm2
        vpxor	%xmm3, %xmm7, %xmm7
        vpslldq	$8, %xmm7, %xmm3
        vpsrldq	$8, %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$0x11, %xmm5, %xmm0, %xmm0
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm6, %xmm0, %xmm0
        movl	36(%esp), %edi
        # store_tag
        cmpl	$16, 40(%esp)
        je	L_AES_GCM_encrypt_final_avx2_store_tag_16
        xorl	%ecx, %ecx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_encrypt_final_avx2_store_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        movb	%al, (%edi,%ecx,1)
        incl	%ecx
        cmpl	40(%esp), %ecx
        jne	L_AES_GCM_encrypt_final_avx2_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_avx2_store_tag_done
L_AES_GCM_encrypt_final_avx2_store_tag_16:
        vmovdqu	%xmm0, (%edi)
L_AES_GCM_encrypt_final_avx2_store_tag_done:
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        ret
.size	AES_GCM_encrypt_final_avx2,.-AES_GCM_encrypt_final_avx2
.text
.globl	AES_GCM_decrypt_update_avx2
.type	AES_GCM_decrypt_update_avx2,@function
.align	16
AES_GCM_decrypt_update_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$0xa0, %esp
        movl	208(%esp), %esi
        vmovdqu	(%esi), %xmm4
        movl	200(%esp), %esi
        movl	204(%esp), %ebp
        vmovdqu	(%esi), %xmm6
        vmovdqu	(%ebp), %xmm5
        movl	180(%esp), %ebp
        movl	188(%esp), %edi
        movl	192(%esp), %esi
        # Calculate H
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        xorl	%ebx, %ebx
        cmpl	$0x40, 196(%esp)
        movl	196(%esp), %eax
        jl	L_AES_GCM_decrypt_update_avx2_done_64
        andl	$0xffffffc0, %eax
        vmovdqu	%xmm4, 64(%esp)
        vmovdqu	%xmm6, 80(%esp)
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm3
        # H ^ 1
        vmovdqu	%xmm5, (%esp)
        vmovdqu	%xmm5, %xmm2
        # H ^ 2
        vpclmulqdq	$0x00, %xmm2, %xmm2, %xmm5
        vpclmulqdq	$0x11, %xmm2, %xmm2, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm0
        vmovdqu	%xmm0, 16(%esp)
        # H ^ 3
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm0, %xmm2, %xmm6
        vpclmulqdq	$0x01, %xmm0, %xmm2, %xmm5
        vpclmulqdq	$0x00, %xmm0, %xmm2, %xmm4
        vpxor	%xmm5, %xmm6, %xmm6
        vpslldq	$8, %xmm6, %xmm5
        vpsrldq	$8, %xmm6, %xmm6
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm2, %xmm1
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm6, %xmm1, %xmm1
        vpxor	%xmm5, %xmm1, %xmm1
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm1, 32(%esp)
        # H ^ 4
        vpclmulqdq	$0x00, %xmm0, %xmm0, %xmm5
        vpclmulqdq	$0x11, %xmm0, %xmm0, %xmm6
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm3, %xmm5, %xmm4
        vpshufd	$0x4e, %xmm5, %xmm5
        vpxor	%xmm4, %xmm5, %xmm5
        vpxor	%xmm5, %xmm6, %xmm2
        vmovdqu	%xmm2, 48(%esp)
        vmovdqu	80(%esp), %xmm6
        cmpl	%esi, %edi
        jne	L_AES_GCM_decrypt_update_avx2_ghash_64
L_AES_GCM_decrypt_update_avx2_ghash_64_inplace:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 184(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 184(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_avx2_inplace_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm7, 96(%esp)
        vmovdqu	%xmm4, 112(%esp)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm7, 128(%esp)
        vmovdqu	%xmm4, 144(%esp)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	96(%esp), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	112(%esp), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	128(%esp), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	144(%esp), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx2_ghash_64_inplace
        jmp	L_AES_GCM_decrypt_update_avx2_ghash_64_done
L_AES_GCM_decrypt_update_avx2_ghash_64:
        # aesenc_64_ghash
        leal	(%esi,%ebx,1), %ecx
        leal	(%edi,%ebx,1), %edx
        # aesenc_64
        # aesenc_ctr
        vmovdqu	64(%esp), %xmm4
        vmovdqu	L_aes_gcm_avx2_bswap_epi64, %xmm7
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm1
        vpshufb	%xmm7, %xmm4, %xmm0
        vpaddd	L_aes_gcm_avx2_two, %xmm4, %xmm2
        vpshufb	%xmm7, %xmm1, %xmm1
        vpaddd	L_aes_gcm_avx2_three, %xmm4, %xmm3
        vpshufb	%xmm7, %xmm2, %xmm2
        vpaddd	L_aes_gcm_avx2_four, %xmm4, %xmm4
        vpshufb	%xmm7, %xmm3, %xmm3
        # aesenc_xor
        vmovdqu	(%ebp), %xmm7
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm7, %xmm1, %xmm1
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm7, %xmm3, %xmm3
        vmovdqu	16(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	32(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	48(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	64(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	80(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	96(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	112(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	128(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	144(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$11, 184(%esp)
        vmovdqu	160(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	176(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        cmpl	$13, 184(%esp)
        vmovdqu	192(%ebp), %xmm7
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	208(%ebp), %xmm7
        vaesenc	%xmm7, %xmm0, %xmm0
        vaesenc	%xmm7, %xmm1, %xmm1
        vaesenc	%xmm7, %xmm2, %xmm2
        vaesenc	%xmm7, %xmm3, %xmm3
        vmovdqu	224(%ebp), %xmm7
L_AES_GCM_decrypt_update_avx2_aesenc_64_ghash_aesenc_64_enc_done:
        # aesenc_last
        vaesenclast	%xmm7, %xmm0, %xmm0
        vaesenclast	%xmm7, %xmm1, %xmm1
        vaesenclast	%xmm7, %xmm2, %xmm2
        vaesenclast	%xmm7, %xmm3, %xmm3
        vmovdqu	(%ecx), %xmm7
        vmovdqu	16(%ecx), %xmm4
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm4, %xmm1, %xmm1
        vmovdqu	%xmm7, (%ecx)
        vmovdqu	%xmm4, 16(%ecx)
        vmovdqu	%xmm0, (%edx)
        vmovdqu	%xmm1, 16(%edx)
        vmovdqu	32(%ecx), %xmm7
        vmovdqu	48(%ecx), %xmm4
        vpxor	%xmm7, %xmm2, %xmm2
        vpxor	%xmm4, %xmm3, %xmm3
        vmovdqu	%xmm7, 32(%ecx)
        vmovdqu	%xmm4, 48(%ecx)
        vmovdqu	%xmm2, 32(%edx)
        vmovdqu	%xmm3, 48(%edx)
        # pclmul_1
        vmovdqu	(%ecx), %xmm1
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vmovdqu	48(%esp), %xmm2
        vpxor	%xmm6, %xmm1, %xmm1
        vpclmulqdq	$16, %xmm2, %xmm1, %xmm5
        vpclmulqdq	$0x01, %xmm2, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm2, %xmm1, %xmm6
        vpclmulqdq	$0x11, %xmm2, %xmm1, %xmm7
        # pclmul_2
        vmovdqu	16(%ecx), %xmm1
        vmovdqu	32(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	32(%ecx), %xmm1
        vmovdqu	16(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # pclmul_n
        vmovdqu	48(%ecx), %xmm1
        vmovdqu	(%esp), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm1, %xmm1
        vpxor	%xmm2, %xmm5, %xmm5
        vpclmulqdq	$16, %xmm0, %xmm1, %xmm2
        vpxor	%xmm3, %xmm5, %xmm5
        vpclmulqdq	$0x01, %xmm0, %xmm1, %xmm3
        vpxor	%xmm4, %xmm6, %xmm6
        vpclmulqdq	$0x00, %xmm0, %xmm1, %xmm4
        vpclmulqdq	$0x11, %xmm0, %xmm1, %xmm1
        vpxor	%xmm1, %xmm7, %xmm7
        # aesenc_pclmul_l
        vpxor	%xmm2, %xmm5, %xmm5
        vpxor	%xmm4, %xmm6, %xmm6
        vpxor	%xmm3, %xmm5, %xmm5
        vpslldq	$8, %xmm5, %xmm1
        vpsrldq	$8, %xmm5, %xmm5
        vmovdqu	L_aes_gcm_avx2_mod2_128, %xmm0
        vpxor	%xmm1, %xmm6, %xmm6
        vpxor	%xmm5, %xmm7, %xmm7
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpclmulqdq	$16, %xmm0, %xmm6, %xmm3
        vpshufd	$0x4e, %xmm6, %xmm6
        vpxor	%xmm3, %xmm6, %xmm6
        vpxor	%xmm7, %xmm6, %xmm6
        # aesenc_64_ghash - end
        addl	$0x40, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx2_ghash_64
L_AES_GCM_decrypt_update_avx2_ghash_64_done:
        vmovdqu	(%esp), %xmm5
        vmovdqu	64(%esp), %xmm4
L_AES_GCM_decrypt_update_avx2_done_64:
        cmpl	196(%esp), %ebx
        jge	L_AES_GCM_decrypt_update_avx2_done_dec
        movl	196(%esp), %eax
        andl	$0xfffffff0, %eax
        cmpl	%eax, %ebx
        jge	L_AES_GCM_decrypt_update_avx2_last_block_done
L_AES_GCM_decrypt_update_avx2_last_block_start:
        vmovdqu	(%esi,%ebx,1), %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_epi64, %xmm4, %xmm7
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpaddd	L_aes_gcm_avx2_one, %xmm4, %xmm4
        vmovdqu	%xmm4, 64(%esp)
        vpxor	%xmm6, %xmm0, %xmm4
        # aesenc_gfmul_sb
        vpclmulqdq	$0x01, %xmm5, %xmm4, %xmm2
        vpclmulqdq	$16, %xmm5, %xmm4, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm4, %xmm1
        vpclmulqdq	$0x11, %xmm5, %xmm4, %xmm4
        vpxor	(%ebp), %xmm7, %xmm7
        vaesenc	16(%ebp), %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpslldq	$8, %xmm3, %xmm2
        vpsrldq	$8, %xmm3, %xmm3
        vaesenc	32(%ebp), %xmm7, %xmm7
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	48(%ebp), %xmm7, %xmm7
        vaesenc	64(%ebp), %xmm7, %xmm7
        vaesenc	80(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vpxor	%xmm1, %xmm2, %xmm2
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm2, %xmm1
        vaesenc	96(%ebp), %xmm7, %xmm7
        vaesenc	112(%ebp), %xmm7, %xmm7
        vaesenc	128(%ebp), %xmm7, %xmm7
        vpshufd	$0x4e, %xmm2, %xmm2
        vaesenc	144(%ebp), %xmm7, %xmm7
        vpxor	%xmm3, %xmm4, %xmm4
        vpxor	%xmm4, %xmm2, %xmm2
        vmovdqu	160(%ebp), %xmm0
        cmpl	$11, 184(%esp)
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	176(%ebp), %xmm7, %xmm7
        vmovdqu	192(%ebp), %xmm0
        cmpl	$13, 184(%esp)
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	%xmm0, %xmm7, %xmm7
        vaesenc	208(%ebp), %xmm7, %xmm7
        vmovdqu	224(%ebp), %xmm0
L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last:
        vaesenclast	%xmm0, %xmm7, %xmm7
        vmovdqu	(%esi,%ebx,1), %xmm3
        vpxor	%xmm1, %xmm2, %xmm6
        vpxor	%xmm3, %xmm7, %xmm7
        vmovdqu	%xmm7, (%edi,%ebx,1)
        vmovdqu	64(%esp), %xmm4
        addl	$16, %ebx
        cmpl	%eax, %ebx
        jl	L_AES_GCM_decrypt_update_avx2_last_block_start
L_AES_GCM_decrypt_update_avx2_last_block_done:
L_AES_GCM_decrypt_update_avx2_done_dec:
        movl	200(%esp), %esi
        movl	208(%esp), %edi
        vmovdqu	64(%esp), %xmm4
        vmovdqu	%xmm6, (%esi)
        vmovdqu	%xmm4, (%edi)
        addl	$0xa0, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_update_avx2,.-AES_GCM_decrypt_update_avx2
.text
.globl	AES_GCM_decrypt_final_avx2
.type	AES_GCM_decrypt_final_avx2,@function
.align	16
AES_GCM_decrypt_final_avx2:
        pushl	%ebx
        pushl	%esi
        pushl	%edi
        pushl	%ebp
        subl	$16, %esp
        movl	36(%esp), %ebp
        movl	56(%esp), %esi
        movl	60(%esp), %edi
        vmovdqu	(%ebp), %xmm4
        vmovdqu	(%esi), %xmm5
        vmovdqu	(%edi), %xmm6
        vpsrlq	$63, %xmm5, %xmm1
        vpsllq	$0x01, %xmm5, %xmm0
        vpslldq	$8, %xmm1, %xmm1
        vpor	%xmm1, %xmm0, %xmm0
        vpshufd	$0xff, %xmm5, %xmm5
        vpsrad	$31, %xmm5, %xmm5
        vpand	L_aes_gcm_avx2_mod2_128, %xmm5, %xmm5
        vpxor	%xmm0, %xmm5, %xmm5
        # calc_tag
        movl	48(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$0x00, %ecx, %xmm0, %xmm0
        movl	52(%esp), %ecx
        shll	$3, %ecx
        vpinsrd	$2, %ecx, %xmm0, %xmm0
        movl	48(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$0x01, %ecx, %xmm0, %xmm0
        movl	52(%esp), %ecx
        shrl	$29, %ecx
        vpinsrd	$3, %ecx, %xmm0, %xmm0
        vpxor	%xmm4, %xmm0, %xmm0
        # ghash_gfmul_red
        vpclmulqdq	$16, %xmm5, %xmm0, %xmm7
        vpclmulqdq	$0x01, %xmm5, %xmm0, %xmm3
        vpclmulqdq	$0x00, %xmm5, %xmm0, %xmm2
        vpxor	%xmm3, %xmm7, %xmm7
        vpslldq	$8, %xmm7, %xmm3
        vpsrldq	$8, %xmm7, %xmm7
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$0x11, %xmm5, %xmm0, %xmm0
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm2, %xmm3, %xmm3
        vpclmulqdq	$16, L_aes_gcm_avx2_mod2_128, %xmm3, %xmm2
        vpshufd	$0x4e, %xmm3, %xmm3
        vpxor	%xmm7, %xmm0, %xmm0
        vpxor	%xmm3, %xmm0, %xmm0
        vpxor	%xmm2, %xmm0, %xmm0
        vpshufb	L_aes_gcm_avx2_bswap_mask, %xmm0, %xmm0
        vpxor	%xmm6, %xmm0, %xmm0
        movl	40(%esp), %esi
        movl	64(%esp), %edi
        # cmp_tag
        cmpl	$16, 44(%esp)
        je	L_AES_GCM_decrypt_final_avx2_cmp_tag_16
        xorl	%ecx, %ecx
        xorl	%edx, %edx
        vmovdqu	%xmm0, (%esp)
L_AES_GCM_decrypt_final_avx2_cmp_tag_loop:
        movzbl	(%esp,%ecx,1), %eax
        xorb	(%esi,%ecx,1), %al
        orb	%al, %dl
        incl	%ecx
        cmpl	44(%esp), %ecx
        jne	L_AES_GCM_decrypt_final_avx2_cmp_tag_loop
        cmpb	$0x00, %dl
        sete	%dl
        jmp	L_AES_GCM_decrypt_final_avx2_cmp_tag_done
L_AES_GCM_decrypt_final_avx2_cmp_tag_16:
        vmovdqu	(%esi), %xmm1
        vpcmpeqb	%xmm1, %xmm0, %xmm0
        vpmovmskb	%xmm0, %ecx
        # %%edx == 0xFFFF then return 1 else => return 0
        xorl	%edx, %edx
        cmpl	$0xffff, %ecx
        sete	%dl
L_AES_GCM_decrypt_final_avx2_cmp_tag_done:
        movl	%edx, (%edi)
        addl	$16, %esp
        popl	%ebp
        popl	%edi
        popl	%esi
        popl	%ebx
        ret
.size	AES_GCM_decrypt_final_avx2,.-AES_GCM_decrypt_final_avx2
#endif /* WOLFSSL_AESGCM_STREAM */
#endif /* HAVE_INTEL_AVX2 */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
