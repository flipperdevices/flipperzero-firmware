; /* aes_gcm_asm
;  *
;  * Copyright (C) 2006-2023 wolfSSL Inc.
;  *
;  * This file is part of wolfSSL.
;  *
;  * wolfSSL is free software; you can redistribute it and/or modify
;  * it under the terms of the GNU General Public License as published by
;  * the Free Software Foundation; either version 2 of the License, or
;  * (at your option) any later version.
;  *
;  * wolfSSL is distributed in the hope that it will be useful,
;  * but WITHOUT ANY WARRANTY; without even the implied warranty of
;  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  * GNU General Public License for more details.
;  *
;  * You should have received a copy of the GNU General Public License
;  * along with this program; if not, write to the Free Software
;  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
;  */
IF @Version LT 1200
; AVX2 instructions not recognized by old versions of MASM
IFNDEF NO_AVX2_SUPPORT
NO_AVX2_SUPPORT = 1
ENDIF
; MOVBE instruction not recognized by old versions of MASM
IFNDEF NO_MOVBE_SUPPORT
NO_MOVBE_SUPPORT = 1
ENDIF
ENDIF

IFNDEF HAVE_INTEL_AVX1
HAVE_INTEL_AVX1 = 1
ENDIF
IFNDEF NO_AVX2_SUPPORT
HAVE_INTEL_AVX2 = 1
ENDIF

IFNDEF _WIN64
_WIN64 = 1
ENDIF

_DATA SEGMENT
ALIGN 16
L_aes_gcm_one QWORD 0, 1
ptr_L_aes_gcm_one QWORD L_aes_gcm_one
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_two QWORD 0, 2
ptr_L_aes_gcm_two QWORD L_aes_gcm_two
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_three QWORD 0, 3
ptr_L_aes_gcm_three QWORD L_aes_gcm_three
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_four QWORD 0, 4
ptr_L_aes_gcm_four QWORD L_aes_gcm_four
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_five QWORD 0, 5
ptr_L_aes_gcm_five QWORD L_aes_gcm_five
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_six QWORD 0, 6
ptr_L_aes_gcm_six QWORD L_aes_gcm_six
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_seven QWORD 0, 7
ptr_L_aes_gcm_seven QWORD L_aes_gcm_seven
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_eight QWORD 0, 8
ptr_L_aes_gcm_eight QWORD L_aes_gcm_eight
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_bswap_epi64 QWORD 283686952306183, 579005069656919567
ptr_L_aes_gcm_bswap_epi64 QWORD L_aes_gcm_bswap_epi64
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_bswap_mask QWORD 579005069656919567, 283686952306183
ptr_L_aes_gcm_bswap_mask QWORD L_aes_gcm_bswap_mask
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_aes_gcm_mod2_128 QWORD 1, 13979173243358019584
ptr_L_aes_gcm_mod2_128 QWORD L_aes_gcm_mod2_128
_DATA ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt PROC
        push	r13
        push	rdi
        push	rsi
        push	r12
        push	rbx
        push	r14
        push	r15
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r12, r8
        mov	rax, r9
        mov	r8, QWORD PTR [rsp+96]
        mov	r9d, DWORD PTR [rsp+104]
        mov	r11d, DWORD PTR [rsp+112]
        mov	ebx, DWORD PTR [rsp+120]
        mov	r14d, DWORD PTR [rsp+128]
        mov	r15, QWORD PTR [rsp+136]
        mov	r10d, DWORD PTR [rsp+144]
        sub	rsp, 320
        movdqu	[rsp+160], xmm6
        movdqu	[rsp+176], xmm7
        movdqu	[rsp+192], xmm8
        movdqu	[rsp+208], xmm9
        movdqu	[rsp+224], xmm10
        movdqu	[rsp+240], xmm11
        movdqu	[rsp+256], xmm12
        movdqu	[rsp+272], xmm13
        movdqu	[rsp+288], xmm14
        movdqu	[rsp+304], xmm15
        pxor	xmm4, xmm4
        pxor	xmm6, xmm6
        cmp	ebx, 12
        mov	edx, ebx
        jne	L_AES_GCM_encrypt_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        pinsrq	xmm4, QWORD PTR [rax], 0
        pinsrd	xmm4, DWORD PTR [rax+8], 2
        pinsrd	xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        movdqa	xmm1, xmm4
        movdqa	xmm5, OWORD PTR [r15]
        pxor	xmm1, xmm5
        movdqa	xmm7, OWORD PTR [r15+16]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+32]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+48]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+64]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+80]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+96]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+112]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+128]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+144]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        cmp	r10d, 11
        movdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_calc_iv_12_last
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+176]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        cmp	r10d, 13
        movdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_calc_iv_12_last
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+208]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_calc_iv_12_last:
        aesenclast	xmm5, xmm7
        aesenclast	xmm1, xmm7
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	[rsp+144], xmm1
        jmp	L_AES_GCM_encrypt_iv_done
L_AES_GCM_encrypt_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        movdqa	xmm5, OWORD PTR [r15]
        aesenc	xmm5, [r15+16]
        aesenc	xmm5, [r15+32]
        aesenc	xmm5, [r15+48]
        aesenc	xmm5, [r15+64]
        aesenc	xmm5, [r15+80]
        aesenc	xmm5, [r15+96]
        aesenc	xmm5, [r15+112]
        aesenc	xmm5, [r15+128]
        aesenc	xmm5, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm9
        aesenc	xmm5, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm9
        aesenc	xmm5, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_calc_iv_1_aesenc_avx_last:
        aesenclast	xmm5, xmm9
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_encrypt_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_calc_iv_16_loop:
        movdqu	xmm8, [rax+rcx]
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm8
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_calc_iv_done
L_AES_GCM_encrypt_calc_iv_lt16:
        sub	rsp, 16
        pxor	xmm8, xmm8
        xor	ebx, ebx
        movdqu	[rsp], xmm8
L_AES_GCM_encrypt_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_calc_iv_loop
        movdqu	xmm8, [rsp]
        add	rsp, 16
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm8
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
L_AES_GCM_encrypt_calc_iv_done:
        ; T = Encrypt counter
        pxor	xmm0, xmm0
        shl	edx, 3
        pinsrq	xmm0, rdx, 0
        pxor	xmm4, xmm0
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        ;   Encrypt counter
        movdqa	xmm8, OWORD PTR [r15]
        pxor	xmm8, xmm4
        aesenc	xmm8, [r15+16]
        aesenc	xmm8, [r15+32]
        aesenc	xmm8, [r15+48]
        aesenc	xmm8, [r15+64]
        aesenc	xmm8, [r15+80]
        aesenc	xmm8, [r15+96]
        aesenc	xmm8, [r15+112]
        aesenc	xmm8, [r15+128]
        aesenc	xmm8, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_calc_iv_2_aesenc_avx_last:
        aesenclast	xmm8, xmm9
        movdqu	[rsp+144], xmm8
L_AES_GCM_encrypt_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_encrypt_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_calc_aad_16_loop:
        movdqu	xmm8, [r12+rcx]
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        pshufd	xmm1, xmm6, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm6, 17
        pclmulqdq	xmm0, xmm6, 0
        pxor	xmm1, xmm6
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm6
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm6, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm6, xmm2
        por	xmm7, xmm0
        por	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm6, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_calc_aad_done
L_AES_GCM_encrypt_calc_aad_lt16:
        sub	rsp, 16
        pxor	xmm8, xmm8
        xor	ebx, ebx
        movdqu	[rsp], xmm8
L_AES_GCM_encrypt_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_calc_aad_loop
        movdqu	xmm8, [rsp]
        add	rsp, 16
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        pshufd	xmm1, xmm6, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm6, 17
        pclmulqdq	xmm0, xmm6, 0
        pxor	xmm1, xmm6
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm6
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm6, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm6, xmm2
        por	xmm7, xmm0
        por	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm6, xmm2
L_AES_GCM_encrypt_calc_aad_done:
        ; Calculate counter and H
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm9, xmm5
        paddd	xmm4, OWORD PTR L_aes_gcm_one
        movdqa	xmm8, xmm5
        movdqu	[rsp+128], xmm4
        psrlq	xmm9, 63
        psllq	xmm8, 1
        pslldq	xmm9, 8
        por	xmm8, xmm9
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm8
        xor	rbx, rbx
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_encrypt_done_128
        and	r13d, 4294967168
        movdqa	xmm2, xmm6
        ; H ^ 1
        movdqu	[rsp], xmm5
        ; H ^ 2
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm5, 78
        movdqa	xmm11, xmm5
        movdqa	xmm8, xmm5
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm5
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm0, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm0, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm0, xmm14
        movdqu	[rsp+16], xmm0
        ; H ^ 3
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm1, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm1, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm1, xmm14
        movdqu	[rsp+32], xmm1
        ; H ^ 4
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm3, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm3, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm3, xmm14
        movdqu	[rsp+48], xmm3
        ; H ^ 5
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+64], xmm7
        ; H ^ 6
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+80], xmm7
        ; H ^ 7
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+96], xmm7
        ; H ^ 8
        pshufd	xmm9, xmm3, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm3, 17
        pclmulqdq	xmm8, xmm3, 0
        pxor	xmm9, xmm3
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+112], xmm7
        ; First 128 bytes of input
        movdqu	xmm8, [rsp+128]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [r15]
        movdqu	[rsp+128], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+16]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+32]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+48]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+64]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+80]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+96]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+112]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+128]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+144]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r10d, 11
        movdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_enc_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r10d, 13
        movdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_enc_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_enc_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [rdi]
        movdqu	xmm1, [rdi+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[rsi], xmm8
        movdqu	[rsi+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [rdi+32]
        movdqu	xmm1, [rdi+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[rsi+32], xmm10
        movdqu	[rsi+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [rdi+64]
        movdqu	xmm1, [rdi+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[rsi+64], xmm12
        movdqu	[rsi+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [rdi+96]
        movdqu	xmm1, [rdi+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[rsi+96], xmm14
        movdqu	[rsi+112], xmm15
        cmp	r13d, 128
        mov	ebx, 128
        jle	L_AES_GCM_encrypt_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_ghash_128:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        movdqu	xmm8, [rsp+128]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [r15]
        movdqu	[rsp+128], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqu	xmm7, [rsp+112]
        movdqu	xmm0, [rdx+-128]
        aesenc	xmm8, [r15+16]
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm0, xmm2
        pshufd	xmm1, xmm7, 78
        pshufd	xmm5, xmm0, 78
        pxor	xmm1, xmm7
        pxor	xmm5, xmm0
        movdqa	xmm3, xmm0
        pclmulqdq	xmm3, xmm7, 17
        aesenc	xmm9, [r15+16]
        aesenc	xmm10, [r15+16]
        movdqa	xmm2, xmm0
        pclmulqdq	xmm2, xmm7, 0
        aesenc	xmm11, [r15+16]
        aesenc	xmm12, [r15+16]
        pclmulqdq	xmm1, xmm5, 0
        aesenc	xmm13, [r15+16]
        aesenc	xmm14, [r15+16]
        aesenc	xmm15, [r15+16]
        pxor	xmm1, xmm2
        pxor	xmm1, xmm3
        movdqu	xmm7, [rsp+96]
        movdqu	xmm0, [rdx+-112]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+32]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+32]
        aesenc	xmm10, [r15+32]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+32]
        aesenc	xmm12, [r15+32]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+32]
        aesenc	xmm14, [r15+32]
        aesenc	xmm15, [r15+32]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+80]
        movdqu	xmm0, [rdx+-96]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+48]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+48]
        aesenc	xmm10, [r15+48]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+48]
        aesenc	xmm12, [r15+48]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+48]
        aesenc	xmm14, [r15+48]
        aesenc	xmm15, [r15+48]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+64]
        movdqu	xmm0, [rdx+-80]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+64]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+64]
        aesenc	xmm10, [r15+64]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+64]
        aesenc	xmm12, [r15+64]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+64]
        aesenc	xmm14, [r15+64]
        aesenc	xmm15, [r15+64]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+48]
        movdqu	xmm0, [rdx+-64]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+80]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+80]
        aesenc	xmm10, [r15+80]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+80]
        aesenc	xmm12, [r15+80]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+80]
        aesenc	xmm14, [r15+80]
        aesenc	xmm15, [r15+80]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+32]
        movdqu	xmm0, [rdx+-48]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+96]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+96]
        aesenc	xmm10, [r15+96]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+96]
        aesenc	xmm12, [r15+96]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+96]
        aesenc	xmm14, [r15+96]
        aesenc	xmm15, [r15+96]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+16]
        movdqu	xmm0, [rdx+-32]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+112]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+112]
        aesenc	xmm10, [r15+112]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+112]
        aesenc	xmm12, [r15+112]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+112]
        aesenc	xmm14, [r15+112]
        aesenc	xmm15, [r15+112]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp]
        movdqu	xmm0, [rdx+-16]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+128]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+128]
        aesenc	xmm10, [r15+128]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+128]
        aesenc	xmm12, [r15+128]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+128]
        aesenc	xmm14, [r15+128]
        aesenc	xmm15, [r15+128]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqa	xmm5, xmm1
        psrldq	xmm1, 8
        pslldq	xmm5, 8
        aesenc	xmm8, [r15+144]
        pxor	xmm2, xmm5
        pxor	xmm3, xmm1
        movdqa	xmm7, xmm2
        movdqa	xmm4, xmm2
        movdqa	xmm5, xmm2
        aesenc	xmm9, [r15+144]
        pslld	xmm7, 31
        pslld	xmm4, 30
        pslld	xmm5, 25
        aesenc	xmm10, [r15+144]
        pxor	xmm7, xmm4
        pxor	xmm7, xmm5
        aesenc	xmm11, [r15+144]
        movdqa	xmm4, xmm7
        pslldq	xmm7, 12
        psrldq	xmm4, 4
        aesenc	xmm12, [r15+144]
        pxor	xmm2, xmm7
        movdqa	xmm5, xmm2
        movdqa	xmm1, xmm2
        movdqa	xmm0, xmm2
        aesenc	xmm13, [r15+144]
        psrld	xmm5, 1
        psrld	xmm1, 2
        psrld	xmm0, 7
        aesenc	xmm14, [r15+144]
        pxor	xmm5, xmm1
        pxor	xmm5, xmm0
        aesenc	xmm15, [r15+144]
        pxor	xmm5, xmm4
        pxor	xmm2, xmm5
        pxor	xmm2, xmm3
        cmp	r10d, 11
        movdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r10d, 13
        movdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_aesenc_128_ghash_avx_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [rcx]
        movdqu	xmm1, [rcx+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[rdx], xmm8
        movdqu	[rdx+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [rcx+32]
        movdqu	xmm1, [rcx+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[rdx+32], xmm10
        movdqu	[rdx+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [rcx+64]
        movdqu	xmm1, [rcx+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[rdx+64], xmm12
        movdqu	[rdx+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [rcx+96]
        movdqu	xmm1, [rcx+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[rdx+96], xmm14
        movdqu	[rdx+112], xmm15
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_ghash_128
L_AES_GCM_encrypt_end_128:
        movdqa	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        pshufb	xmm8, xmm4
        pshufb	xmm9, xmm4
        pshufb	xmm10, xmm4
        pshufb	xmm11, xmm4
        pxor	xmm8, xmm2
        pshufb	xmm12, xmm4
        pshufb	xmm13, xmm4
        pshufb	xmm14, xmm4
        pshufb	xmm15, xmm4
        movdqu	xmm7, [rsp+112]
        pshufd	xmm1, xmm8, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm8, 17
        pclmulqdq	xmm0, xmm8, 0
        pxor	xmm1, xmm8
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm4, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+96]
        pshufd	xmm1, xmm9, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm9, 17
        pclmulqdq	xmm0, xmm9, 0
        pxor	xmm1, xmm9
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+80]
        pshufd	xmm1, xmm10, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm10, 17
        pclmulqdq	xmm0, xmm10, 0
        pxor	xmm1, xmm10
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+64]
        pshufd	xmm1, xmm11, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm11, 17
        pclmulqdq	xmm0, xmm11, 0
        pxor	xmm1, xmm11
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+48]
        pshufd	xmm1, xmm12, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm12, 17
        pclmulqdq	xmm0, xmm12, 0
        pxor	xmm1, xmm12
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+32]
        pshufd	xmm1, xmm13, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm13, 17
        pclmulqdq	xmm0, xmm13, 0
        pxor	xmm1, xmm13
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+16]
        pshufd	xmm1, xmm14, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm14, 17
        pclmulqdq	xmm0, xmm14, 0
        pxor	xmm1, xmm14
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp]
        pshufd	xmm1, xmm15, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm15, 17
        pclmulqdq	xmm0, xmm15, 0
        pxor	xmm1, xmm15
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm4
        movdqa	xmm1, xmm4
        movdqa	xmm2, xmm4
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm4, xmm0
        movdqa	xmm2, xmm4
        movdqa	xmm3, xmm4
        movdqa	xmm0, xmm4
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm4
        pxor	xmm6, xmm2
        movdqu	xmm5, [rsp]
L_AES_GCM_encrypt_done_128:
        mov	edx, r9d
        cmp	ebx, edx
        jge	L_AES_GCM_encrypt_done_enc
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_last_block_done
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        movdqu	xmm8, [rsp+128]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [r15]
        movdqu	[rsp+128], xmm9
        aesenc	xmm8, [r15+16]
        aesenc	xmm8, [r15+32]
        aesenc	xmm8, [r15+48]
        aesenc	xmm8, [r15+64]
        aesenc	xmm8, [r15+80]
        aesenc	xmm8, [r15+96]
        aesenc	xmm8, [r15+112]
        aesenc	xmm8, [r15+128]
        aesenc	xmm8, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_aesenc_block_aesenc_avx_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        add	ebx, 16
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_last_block_ghash
L_AES_GCM_encrypt_last_block_start:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        movdqu	xmm8, [rsp+128]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [r15]
        movdqu	[rsp+128], xmm9
        movdqa	xmm10, xmm6
        pclmulqdq	xmm10, xmm5, 16
        aesenc	xmm8, [r15+16]
        aesenc	xmm8, [r15+32]
        movdqa	xmm11, xmm6
        pclmulqdq	xmm11, xmm5, 1
        aesenc	xmm8, [r15+48]
        aesenc	xmm8, [r15+64]
        movdqa	xmm12, xmm6
        pclmulqdq	xmm12, xmm5, 0
        aesenc	xmm8, [r15+80]
        movdqa	xmm1, xmm6
        pclmulqdq	xmm1, xmm5, 17
        aesenc	xmm8, [r15+96]
        pxor	xmm10, xmm11
        movdqa	xmm2, xmm10
        psrldq	xmm10, 8
        pslldq	xmm2, 8
        aesenc	xmm8, [r15+112]
        movdqa	xmm3, xmm1
        pxor	xmm2, xmm12
        pxor	xmm3, xmm10
        movdqa	xmm0, OWORD PTR L_aes_gcm_mod2_128
        movdqa	xmm11, xmm2
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [r15+128]
        pshufd	xmm10, xmm2, 78
        pxor	xmm10, xmm11
        movdqa	xmm11, xmm10
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [r15+144]
        pshufd	xmm6, xmm10, 78
        pxor	xmm6, xmm11
        pxor	xmm6, xmm3
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_aesenc_gfmul_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        add	ebx, 16
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_last_block_start
L_AES_GCM_encrypt_last_block_ghash:
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
L_AES_GCM_encrypt_last_block_done:
        mov	ecx, r9d
        mov	edx, ecx
        and	ecx, 15
        jz	L_AES_GCM_encrypt_aesenc_last15_enc_avx_done
        movdqu	xmm4, [rsp+128]
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_epi64
        pxor	xmm4, [r15]
        aesenc	xmm4, [r15+16]
        aesenc	xmm4, [r15+32]
        aesenc	xmm4, [r15+48]
        aesenc	xmm4, [r15+64]
        aesenc	xmm4, [r15+80]
        aesenc	xmm4, [r15+96]
        aesenc	xmm4, [r15+112]
        aesenc	xmm4, [r15+128]
        aesenc	xmm4, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last
        aesenc	xmm4, xmm9
        aesenc	xmm4, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last
        aesenc	xmm4, xmm9
        aesenc	xmm4, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_aesenc_last15_enc_avx_aesenc_avx_last:
        aesenclast	xmm4, xmm9
        sub	rsp, 16
        xor	ecx, ecx
        movdqu	[rsp], xmm4
L_AES_GCM_encrypt_aesenc_last15_enc_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [rsi+rbx], r13b
        mov	BYTE PTR [rsp+rcx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_loop
        xor	r13, r13
        cmp	ecx, 16
        je	L_AES_GCM_encrypt_aesenc_last15_enc_avx_finish_enc
L_AES_GCM_encrypt_aesenc_last15_enc_avx_byte_loop:
        mov	BYTE PTR [rsp+rcx], r13b
        inc	ecx
        cmp	ecx, 16
        jl	L_AES_GCM_encrypt_aesenc_last15_enc_avx_byte_loop
L_AES_GCM_encrypt_aesenc_last15_enc_avx_finish_enc:
        movdqu	xmm4, [rsp]
        add	rsp, 16
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm4
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
L_AES_GCM_encrypt_aesenc_last15_enc_avx_done:
L_AES_GCM_encrypt_done_enc:
        mov	edx, r9d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        pinsrq	xmm0, rdx, 0
        pinsrq	xmm0, rcx, 1
        pxor	xmm6, xmm0
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
        pshufb	xmm6, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	xmm0, [rsp+144]
        pxor	xmm0, xmm6
        cmp	r14d, 16
        je	L_AES_GCM_encrypt_store_tag_16
        xor	rcx, rcx
        movdqu	[rsp], xmm0
L_AES_GCM_encrypt_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r8+rcx], r13b
        inc	ecx
        cmp	ecx, r14d
        jne	L_AES_GCM_encrypt_store_tag_loop
        jmp	L_AES_GCM_encrypt_store_tag_done
L_AES_GCM_encrypt_store_tag_16:
        movdqu	[r8], xmm0
L_AES_GCM_encrypt_store_tag_done:
        movdqu	xmm6, [rsp+160]
        movdqu	xmm7, [rsp+176]
        movdqu	xmm8, [rsp+192]
        movdqu	xmm9, [rsp+208]
        movdqu	xmm10, [rsp+224]
        movdqu	xmm11, [rsp+240]
        movdqu	xmm12, [rsp+256]
        movdqu	xmm13, [rsp+272]
        movdqu	xmm14, [rsp+288]
        movdqu	xmm15, [rsp+304]
        add	rsp, 320
        pop	r15
        pop	r14
        pop	rbx
        pop	r12
        pop	rsi
        pop	rdi
        pop	r13
        ret
AES_GCM_encrypt ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt PROC
        push	r13
        push	rdi
        push	rsi
        push	r12
        push	rbx
        push	r14
        push	r15
        push	rbp
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r12, r8
        mov	rax, r9
        mov	r8, QWORD PTR [rsp+104]
        mov	r9d, DWORD PTR [rsp+112]
        mov	r11d, DWORD PTR [rsp+120]
        mov	ebx, DWORD PTR [rsp+128]
        mov	r14d, DWORD PTR [rsp+136]
        mov	r15, QWORD PTR [rsp+144]
        mov	r10d, DWORD PTR [rsp+152]
        mov	rbp, QWORD PTR [rsp+160]
        sub	rsp, 328
        movdqu	[rsp+168], xmm6
        movdqu	[rsp+184], xmm7
        movdqu	[rsp+200], xmm8
        movdqu	[rsp+216], xmm9
        movdqu	[rsp+232], xmm10
        movdqu	[rsp+248], xmm11
        movdqu	[rsp+264], xmm12
        movdqu	[rsp+280], xmm13
        movdqu	[rsp+296], xmm14
        movdqu	[rsp+312], xmm15
        pxor	xmm4, xmm4
        pxor	xmm6, xmm6
        cmp	ebx, 12
        mov	edx, ebx
        jne	L_AES_GCM_decrypt_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        pinsrq	xmm4, QWORD PTR [rax], 0
        pinsrd	xmm4, DWORD PTR [rax+8], 2
        pinsrd	xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        movdqa	xmm1, xmm4
        movdqa	xmm5, OWORD PTR [r15]
        pxor	xmm1, xmm5
        movdqa	xmm7, OWORD PTR [r15+16]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+32]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+48]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+64]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+80]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+96]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+112]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+128]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+144]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        cmp	r10d, 11
        movdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_calc_iv_12_last
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+176]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        cmp	r10d, 13
        movdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_calc_iv_12_last
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+208]
        aesenc	xmm5, xmm7
        aesenc	xmm1, xmm7
        movdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_decrypt_calc_iv_12_last:
        aesenclast	xmm5, xmm7
        aesenclast	xmm1, xmm7
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	[rsp+144], xmm1
        jmp	L_AES_GCM_decrypt_iv_done
L_AES_GCM_decrypt_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        movdqa	xmm5, OWORD PTR [r15]
        aesenc	xmm5, [r15+16]
        aesenc	xmm5, [r15+32]
        aesenc	xmm5, [r15+48]
        aesenc	xmm5, [r15+64]
        aesenc	xmm5, [r15+80]
        aesenc	xmm5, [r15+96]
        aesenc	xmm5, [r15+112]
        aesenc	xmm5, [r15+128]
        aesenc	xmm5, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm9
        aesenc	xmm5, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm9
        aesenc	xmm5, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_calc_iv_1_aesenc_avx_last:
        aesenclast	xmm5, xmm9
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_decrypt_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_calc_iv_16_loop:
        movdqu	xmm8, [rax+rcx]
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm8
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_calc_iv_done
L_AES_GCM_decrypt_calc_iv_lt16:
        sub	rsp, 16
        pxor	xmm8, xmm8
        xor	ebx, ebx
        movdqu	[rsp], xmm8
L_AES_GCM_decrypt_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_calc_iv_loop
        movdqu	xmm8, [rsp]
        add	rsp, 16
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm8
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
L_AES_GCM_decrypt_calc_iv_done:
        ; T = Encrypt counter
        pxor	xmm0, xmm0
        shl	edx, 3
        pinsrq	xmm0, rdx, 0
        pxor	xmm4, xmm0
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm7, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm4, xmm2
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        ;   Encrypt counter
        movdqa	xmm8, OWORD PTR [r15]
        pxor	xmm8, xmm4
        aesenc	xmm8, [r15+16]
        aesenc	xmm8, [r15+32]
        aesenc	xmm8, [r15+48]
        aesenc	xmm8, [r15+64]
        aesenc	xmm8, [r15+80]
        aesenc	xmm8, [r15+96]
        aesenc	xmm8, [r15+112]
        aesenc	xmm8, [r15+128]
        aesenc	xmm8, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_calc_iv_2_aesenc_avx_last:
        aesenclast	xmm8, xmm9
        movdqu	[rsp+144], xmm8
L_AES_GCM_decrypt_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_decrypt_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_calc_aad_16_loop:
        movdqu	xmm8, [r12+rcx]
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        pshufd	xmm1, xmm6, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm6, 17
        pclmulqdq	xmm0, xmm6, 0
        pxor	xmm1, xmm6
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm6
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm6, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm6, xmm2
        por	xmm7, xmm0
        por	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm6, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_calc_aad_done
L_AES_GCM_decrypt_calc_aad_lt16:
        sub	rsp, 16
        pxor	xmm8, xmm8
        xor	ebx, ebx
        movdqu	[rsp], xmm8
L_AES_GCM_decrypt_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_calc_aad_loop
        movdqu	xmm8, [rsp]
        add	rsp, 16
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        pshufd	xmm1, xmm6, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm6, 17
        pclmulqdq	xmm0, xmm6, 0
        pxor	xmm1, xmm6
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm7, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm7, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm6
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm7, 1
        pslld	xmm6, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm6, xmm2
        por	xmm7, xmm0
        por	xmm6, xmm1
        movdqa	xmm0, xmm7
        movdqa	xmm1, xmm7
        movdqa	xmm2, xmm7
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm7, xmm0
        movdqa	xmm2, xmm7
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm7
        pxor	xmm6, xmm2
L_AES_GCM_decrypt_calc_aad_done:
        ; Calculate counter and H
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm9, xmm5
        paddd	xmm4, OWORD PTR L_aes_gcm_one
        movdqa	xmm8, xmm5
        movdqu	[rsp+128], xmm4
        psrlq	xmm9, 63
        psllq	xmm8, 1
        pslldq	xmm9, 8
        por	xmm8, xmm9
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm8
        xor	ebx, ebx
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_decrypt_done_128
        and	r13d, 4294967168
        movdqa	xmm2, xmm6
        ; H ^ 1
        movdqu	[rsp], xmm5
        ; H ^ 2
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm5, 78
        movdqa	xmm11, xmm5
        movdqa	xmm8, xmm5
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm5
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm0, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm0, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm0, xmm14
        movdqu	[rsp+16], xmm0
        ; H ^ 3
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm1, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm1, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm1, xmm14
        movdqu	[rsp+32], xmm1
        ; H ^ 4
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm3, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm3, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm3, xmm14
        movdqu	[rsp+48], xmm3
        ; H ^ 5
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+64], xmm7
        ; H ^ 6
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+80], xmm7
        ; H ^ 7
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+96], xmm7
        ; H ^ 8
        pshufd	xmm9, xmm3, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm3, 17
        pclmulqdq	xmm8, xmm3, 0
        pxor	xmm9, xmm3
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+112], xmm7
L_AES_GCM_decrypt_ghash_128:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        movdqu	xmm8, [rsp+128]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [r15]
        movdqu	[rsp+128], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqu	xmm7, [rsp+112]
        movdqu	xmm0, [rcx]
        aesenc	xmm8, [r15+16]
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm0, xmm2
        pshufd	xmm1, xmm7, 78
        pshufd	xmm5, xmm0, 78
        pxor	xmm1, xmm7
        pxor	xmm5, xmm0
        movdqa	xmm3, xmm0
        pclmulqdq	xmm3, xmm7, 17
        aesenc	xmm9, [r15+16]
        aesenc	xmm10, [r15+16]
        movdqa	xmm2, xmm0
        pclmulqdq	xmm2, xmm7, 0
        aesenc	xmm11, [r15+16]
        aesenc	xmm12, [r15+16]
        pclmulqdq	xmm1, xmm5, 0
        aesenc	xmm13, [r15+16]
        aesenc	xmm14, [r15+16]
        aesenc	xmm15, [r15+16]
        pxor	xmm1, xmm2
        pxor	xmm1, xmm3
        movdqu	xmm7, [rsp+96]
        movdqu	xmm0, [rcx+16]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+32]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+32]
        aesenc	xmm10, [r15+32]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+32]
        aesenc	xmm12, [r15+32]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+32]
        aesenc	xmm14, [r15+32]
        aesenc	xmm15, [r15+32]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+80]
        movdqu	xmm0, [rcx+32]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+48]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+48]
        aesenc	xmm10, [r15+48]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+48]
        aesenc	xmm12, [r15+48]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+48]
        aesenc	xmm14, [r15+48]
        aesenc	xmm15, [r15+48]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+64]
        movdqu	xmm0, [rcx+48]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+64]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+64]
        aesenc	xmm10, [r15+64]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+64]
        aesenc	xmm12, [r15+64]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+64]
        aesenc	xmm14, [r15+64]
        aesenc	xmm15, [r15+64]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+48]
        movdqu	xmm0, [rcx+64]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+80]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+80]
        aesenc	xmm10, [r15+80]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+80]
        aesenc	xmm12, [r15+80]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+80]
        aesenc	xmm14, [r15+80]
        aesenc	xmm15, [r15+80]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+32]
        movdqu	xmm0, [rcx+80]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+96]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+96]
        aesenc	xmm10, [r15+96]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+96]
        aesenc	xmm12, [r15+96]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+96]
        aesenc	xmm14, [r15+96]
        aesenc	xmm15, [r15+96]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+16]
        movdqu	xmm0, [rcx+96]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+112]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+112]
        aesenc	xmm10, [r15+112]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+112]
        aesenc	xmm12, [r15+112]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+112]
        aesenc	xmm14, [r15+112]
        aesenc	xmm15, [r15+112]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp]
        movdqu	xmm0, [rcx+112]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [r15+128]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [r15+128]
        aesenc	xmm10, [r15+128]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [r15+128]
        aesenc	xmm12, [r15+128]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [r15+128]
        aesenc	xmm14, [r15+128]
        aesenc	xmm15, [r15+128]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqa	xmm5, xmm1
        psrldq	xmm1, 8
        pslldq	xmm5, 8
        aesenc	xmm8, [r15+144]
        pxor	xmm2, xmm5
        pxor	xmm3, xmm1
        movdqa	xmm7, xmm2
        movdqa	xmm4, xmm2
        movdqa	xmm5, xmm2
        aesenc	xmm9, [r15+144]
        pslld	xmm7, 31
        pslld	xmm4, 30
        pslld	xmm5, 25
        aesenc	xmm10, [r15+144]
        pxor	xmm7, xmm4
        pxor	xmm7, xmm5
        aesenc	xmm11, [r15+144]
        movdqa	xmm4, xmm7
        pslldq	xmm7, 12
        psrldq	xmm4, 4
        aesenc	xmm12, [r15+144]
        pxor	xmm2, xmm7
        movdqa	xmm5, xmm2
        movdqa	xmm1, xmm2
        movdqa	xmm0, xmm2
        aesenc	xmm13, [r15+144]
        psrld	xmm5, 1
        psrld	xmm1, 2
        psrld	xmm0, 7
        aesenc	xmm14, [r15+144]
        pxor	xmm5, xmm1
        pxor	xmm5, xmm0
        aesenc	xmm15, [r15+144]
        pxor	xmm5, xmm4
        pxor	xmm2, xmm5
        pxor	xmm2, xmm3
        cmp	r10d, 11
        movdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r10d, 13
        movdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_decrypt_aesenc_128_ghash_avx_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [rcx]
        movdqu	xmm1, [rcx+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[rdx], xmm8
        movdqu	[rdx+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [rcx+32]
        movdqu	xmm1, [rcx+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[rdx+32], xmm10
        movdqu	[rdx+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [rcx+64]
        movdqu	xmm1, [rcx+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[rdx+64], xmm12
        movdqu	[rdx+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [rcx+96]
        movdqu	xmm1, [rcx+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[rdx+96], xmm14
        movdqu	[rdx+112], xmm15
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_ghash_128
        movdqa	xmm6, xmm2
        movdqu	xmm5, [rsp]
L_AES_GCM_decrypt_done_128:
        mov	edx, r9d
        cmp	ebx, edx
        jge	L_AES_GCM_decrypt_done_dec
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_decrypt_last_block_done
L_AES_GCM_decrypt_last_block_start:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        movdqu	xmm1, [rcx]
        movdqa	xmm0, xmm5
        pshufb	xmm1, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm1, xmm6
        movdqu	xmm8, [rsp+128]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [r15]
        movdqu	[rsp+128], xmm9
        movdqa	xmm10, xmm1
        pclmulqdq	xmm10, xmm0, 16
        aesenc	xmm8, [r15+16]
        aesenc	xmm8, [r15+32]
        movdqa	xmm11, xmm1
        pclmulqdq	xmm11, xmm0, 1
        aesenc	xmm8, [r15+48]
        aesenc	xmm8, [r15+64]
        movdqa	xmm12, xmm1
        pclmulqdq	xmm12, xmm0, 0
        aesenc	xmm8, [r15+80]
        movdqa	xmm1, xmm1
        pclmulqdq	xmm1, xmm0, 17
        aesenc	xmm8, [r15+96]
        pxor	xmm10, xmm11
        movdqa	xmm2, xmm10
        psrldq	xmm10, 8
        pslldq	xmm2, 8
        aesenc	xmm8, [r15+112]
        movdqa	xmm3, xmm1
        pxor	xmm2, xmm12
        pxor	xmm3, xmm10
        movdqa	xmm0, OWORD PTR L_aes_gcm_mod2_128
        movdqa	xmm11, xmm2
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [r15+128]
        pshufd	xmm10, xmm2, 78
        pxor	xmm10, xmm11
        movdqa	xmm11, xmm10
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [r15+144]
        pshufd	xmm6, xmm10, 78
        pxor	xmm6, xmm11
        pxor	xmm6, xmm3
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_aesenc_gfmul_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        add	ebx, 16
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_last_block_start
L_AES_GCM_decrypt_last_block_done:
        mov	ecx, r9d
        mov	edx, ecx
        and	ecx, 15
        jz	L_AES_GCM_decrypt_aesenc_last15_dec_avx_done
        movdqu	xmm4, [rsp+128]
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_epi64
        pxor	xmm4, [r15]
        aesenc	xmm4, [r15+16]
        aesenc	xmm4, [r15+32]
        aesenc	xmm4, [r15+48]
        aesenc	xmm4, [r15+64]
        aesenc	xmm4, [r15+80]
        aesenc	xmm4, [r15+96]
        aesenc	xmm4, [r15+112]
        aesenc	xmm4, [r15+128]
        aesenc	xmm4, [r15+144]
        cmp	r10d, 11
        movdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last
        aesenc	xmm4, xmm9
        aesenc	xmm4, [r15+176]
        cmp	r10d, 13
        movdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last
        aesenc	xmm4, xmm9
        aesenc	xmm4, [r15+208]
        movdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_aesenc_last15_dec_avx_aesenc_avx_last:
        aesenclast	xmm4, xmm9
        sub	rsp, 32
        xor	ecx, ecx
        movdqu	[rsp], xmm4
        pxor	xmm0, xmm0
        movdqu	[rsp+16], xmm0
L_AES_GCM_decrypt_aesenc_last15_dec_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        mov	BYTE PTR [rsp+rcx+16], r13b
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [rsi+rbx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_decrypt_aesenc_last15_dec_avx_loop
        movdqu	xmm4, [rsp+16]
        add	rsp, 32
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm4
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
L_AES_GCM_decrypt_aesenc_last15_dec_avx_done:
L_AES_GCM_decrypt_done_dec:
        mov	edx, r9d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        pinsrq	xmm0, rdx, 0
        pinsrq	xmm0, rcx, 1
        pxor	xmm6, xmm0
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
        pshufb	xmm6, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	xmm0, [rsp+144]
        pxor	xmm0, xmm6
        cmp	r14d, 16
        je	L_AES_GCM_decrypt_cmp_tag_16
        sub	rsp, 16
        xor	rcx, rcx
        xor	rbx, rbx
        movdqu	[rsp], xmm0
L_AES_GCM_decrypt_cmp_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        xor	r13b, BYTE PTR [r8+rcx]
        or	bl, r13b
        inc	ecx
        cmp	ecx, r14d
        jne	L_AES_GCM_decrypt_cmp_tag_loop
        cmp	rbx, 0
        sete	bl
        add	rsp, 16
        xor	rcx, rcx
        jmp	L_AES_GCM_decrypt_cmp_tag_done
L_AES_GCM_decrypt_cmp_tag_16:
        movdqu	xmm1, [r8]
        pcmpeqb	xmm0, xmm1
        pmovmskb	rdx, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	ebx, ebx
        cmp	edx, 65535
        sete	bl
L_AES_GCM_decrypt_cmp_tag_done:
        mov	DWORD PTR [rbp], ebx
        movdqu	xmm6, [rsp+168]
        movdqu	xmm7, [rsp+184]
        movdqu	xmm8, [rsp+200]
        movdqu	xmm9, [rsp+216]
        movdqu	xmm10, [rsp+232]
        movdqu	xmm11, [rsp+248]
        movdqu	xmm12, [rsp+264]
        movdqu	xmm13, [rsp+280]
        movdqu	xmm14, [rsp+296]
        movdqu	xmm15, [rsp+312]
        add	rsp, 328
        pop	rbp
        pop	r15
        pop	r14
        pop	rbx
        pop	r12
        pop	rsi
        pop	rdi
        pop	r13
        ret
AES_GCM_decrypt ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_init_aesni PROC
        push	rdi
        push	rsi
        push	r12
        push	r13
        push	r14
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r10, r8
        mov	r11d, r9d
        mov	rax, QWORD PTR [rsp+80]
        mov	r8, QWORD PTR [rsp+88]
        mov	r9, QWORD PTR [rsp+96]
        sub	rsp, 80
        movdqu	[rsp+16], xmm6
        movdqu	[rsp+32], xmm7
        movdqu	[rsp+48], xmm8
        movdqu	[rsp+64], xmm15
        pxor	xmm4, xmm4
        mov	edx, r11d
        cmp	edx, 12
        jne	L_AES_GCM_init_aesni_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        pinsrq	xmm4, QWORD PTR [r10], 0
        pinsrd	xmm4, DWORD PTR [r10+8], 2
        pinsrd	xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        movdqa	xmm1, xmm4
        movdqa	xmm5, OWORD PTR [rdi]
        pxor	xmm1, xmm5
        movdqa	xmm6, OWORD PTR [rdi+16]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+32]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+48]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+64]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+80]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+96]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+112]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+128]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+144]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        cmp	esi, 11
        movdqa	xmm6, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_aesni_calc_iv_12_last
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+176]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        cmp	esi, 13
        movdqa	xmm6, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_aesni_calc_iv_12_last
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+208]
        aesenc	xmm5, xmm6
        aesenc	xmm1, xmm6
        movdqa	xmm6, OWORD PTR [rdi+224]
L_AES_GCM_init_aesni_calc_iv_12_last:
        aesenclast	xmm5, xmm6
        aesenclast	xmm1, xmm6
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	xmm15, xmm1
        jmp	L_AES_GCM_init_aesni_iv_done
L_AES_GCM_init_aesni_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        movdqa	xmm5, OWORD PTR [rdi]
        aesenc	xmm5, [rdi+16]
        aesenc	xmm5, [rdi+32]
        aesenc	xmm5, [rdi+48]
        aesenc	xmm5, [rdi+64]
        aesenc	xmm5, [rdi+80]
        aesenc	xmm5, [rdi+96]
        aesenc	xmm5, [rdi+112]
        aesenc	xmm5, [rdi+128]
        aesenc	xmm5, [rdi+144]
        cmp	esi, 11
        movdqa	xmm8, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm8
        aesenc	xmm5, [rdi+176]
        cmp	esi, 13
        movdqa	xmm8, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last
        aesenc	xmm5, xmm8
        aesenc	xmm5, [rdi+208]
        movdqa	xmm8, OWORD PTR [rdi+224]
L_AES_GCM_init_aesni_calc_iv_1_aesenc_avx_last:
        aesenclast	xmm5, xmm8
        pshufb	xmm5, OWORD PTR L_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_init_aesni_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_init_aesni_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_init_aesni_calc_iv_16_loop:
        movdqu	xmm7, [r10+rcx]
        pshufb	xmm7, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm7
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm6, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm6, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm6, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm6, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm6
        movdqa	xmm2, xmm6
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm6, xmm0
        movdqa	xmm2, xmm6
        movdqa	xmm3, xmm6
        movdqa	xmm0, xmm6
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm6
        pxor	xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_init_aesni_calc_iv_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_init_aesni_calc_iv_done
L_AES_GCM_init_aesni_calc_iv_lt16:
        sub	rsp, 16
        pxor	xmm7, xmm7
        xor	r13d, r13d
        movdqu	[rsp], xmm7
L_AES_GCM_init_aesni_calc_iv_loop:
        movzx	r12d, BYTE PTR [r10+rcx]
        mov	BYTE PTR [rsp+r13], r12b
        inc	ecx
        inc	r13d
        cmp	ecx, edx
        jl	L_AES_GCM_init_aesni_calc_iv_loop
        movdqu	xmm7, [rsp]
        add	rsp, 16
        pshufb	xmm7, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm7
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm6, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm6, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm6, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm6, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm6
        movdqa	xmm2, xmm6
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm6, xmm0
        movdqa	xmm2, xmm6
        movdqa	xmm3, xmm6
        movdqa	xmm0, xmm6
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm6
        pxor	xmm4, xmm2
L_AES_GCM_init_aesni_calc_iv_done:
        ; T = Encrypt counter
        pxor	xmm0, xmm0
        shl	edx, 3
        pinsrq	xmm0, rdx, 0
        pxor	xmm4, xmm0
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm6, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm6, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm6, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm6, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm6
        movdqa	xmm2, xmm6
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm6, xmm0
        movdqa	xmm2, xmm6
        movdqa	xmm3, xmm6
        movdqa	xmm0, xmm6
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm6
        pxor	xmm4, xmm2
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        ;   Encrypt counter
        movdqa	xmm7, OWORD PTR [rdi]
        pxor	xmm7, xmm4
        aesenc	xmm7, [rdi+16]
        aesenc	xmm7, [rdi+32]
        aesenc	xmm7, [rdi+48]
        aesenc	xmm7, [rdi+64]
        aesenc	xmm7, [rdi+80]
        aesenc	xmm7, [rdi+96]
        aesenc	xmm7, [rdi+112]
        aesenc	xmm7, [rdi+128]
        aesenc	xmm7, [rdi+144]
        cmp	esi, 11
        movdqa	xmm8, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last
        aesenc	xmm7, xmm8
        aesenc	xmm7, [rdi+176]
        cmp	esi, 13
        movdqa	xmm8, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last
        aesenc	xmm7, xmm8
        aesenc	xmm7, [rdi+208]
        movdqa	xmm8, OWORD PTR [rdi+224]
L_AES_GCM_init_aesni_calc_iv_2_aesenc_avx_last:
        aesenclast	xmm7, xmm8
        movdqu	xmm15, xmm7
L_AES_GCM_init_aesni_iv_done:
        movdqa	OWORD PTR [r9], xmm15
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm4, OWORD PTR L_aes_gcm_one
        movdqa	OWORD PTR [rax], xmm5
        movdqa	OWORD PTR [r8], xmm4
        movdqu	xmm6, [rsp+16]
        movdqu	xmm7, [rsp+32]
        movdqu	xmm8, [rsp+48]
        movdqu	xmm15, [rsp+64]
        add	rsp, 80
        pop	r14
        pop	r13
        pop	r12
        pop	rsi
        pop	rdi
        ret
AES_GCM_init_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_aad_update_aesni PROC
        mov	rax, rcx
        sub	rsp, 32
        movdqu	[rsp], xmm6
        movdqu	[rsp+16], xmm7
        movdqa	xmm5, OWORD PTR [r8]
        movdqa	xmm6, OWORD PTR [r9]
        xor	ecx, ecx
L_AES_GCM_aad_update_aesni_16_loop:
        movdqu	xmm7, [rax+rcx]
        pshufb	xmm7, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm5, xmm7
        pshufd	xmm1, xmm5, 78
        pshufd	xmm2, xmm6, 78
        movdqa	xmm3, xmm6
        movdqa	xmm0, xmm6
        pclmulqdq	xmm3, xmm5, 17
        pclmulqdq	xmm0, xmm5, 0
        pxor	xmm1, xmm5
        pxor	xmm2, xmm6
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm4, xmm0
        movdqa	xmm5, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm5, xmm1
        movdqa	xmm0, xmm4
        movdqa	xmm1, xmm5
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm4, 1
        pslld	xmm5, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm5, xmm2
        por	xmm4, xmm0
        por	xmm5, xmm1
        movdqa	xmm0, xmm4
        movdqa	xmm1, xmm4
        movdqa	xmm2, xmm4
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm4, xmm0
        movdqa	xmm2, xmm4
        movdqa	xmm3, xmm4
        movdqa	xmm0, xmm4
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm4
        pxor	xmm5, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_aad_update_aesni_16_loop
        movdqa	OWORD PTR [r8], xmm5
        movdqu	xmm6, [rsp]
        movdqu	xmm7, [rsp+16]
        add	rsp, 32
        ret
AES_GCM_aad_update_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_block_aesni PROC
        mov	r10, r8
        mov	r11, r9
        mov	rax, QWORD PTR [rsp+40]
        movdqu	xmm0, [rax]
        movdqa	xmm1, xmm0
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm1, OWORD PTR L_aes_gcm_one
        pxor	xmm0, [rcx]
        movdqu	[rax], xmm1
        aesenc	xmm0, [rcx+16]
        aesenc	xmm0, [rcx+32]
        aesenc	xmm0, [rcx+48]
        aesenc	xmm0, [rcx+64]
        aesenc	xmm0, [rcx+80]
        aesenc	xmm0, [rcx+96]
        aesenc	xmm0, [rcx+112]
        aesenc	xmm0, [rcx+128]
        aesenc	xmm0, [rcx+144]
        cmp	edx, 11
        movdqa	xmm1, OWORD PTR [rcx+160]
        jl	L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last
        aesenc	xmm0, xmm1
        aesenc	xmm0, [rcx+176]
        cmp	edx, 13
        movdqa	xmm1, OWORD PTR [rcx+192]
        jl	L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last
        aesenc	xmm0, xmm1
        aesenc	xmm0, [rcx+208]
        movdqa	xmm1, OWORD PTR [rcx+224]
L_AES_GCM_encrypt_block_aesni_aesenc_block_aesenc_avx_last:
        aesenclast	xmm0, xmm1
        movdqu	xmm1, [r11]
        pxor	xmm0, xmm1
        movdqu	[r10], xmm0
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        ret
AES_GCM_encrypt_block_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_ghash_block_aesni PROC
        sub	rsp, 32
        movdqu	[rsp], xmm6
        movdqu	[rsp+16], xmm7
        movdqa	xmm4, OWORD PTR [rdx]
        movdqa	xmm5, OWORD PTR [r8]
        movdqu	xmm7, [rcx]
        pshufb	xmm7, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm4, xmm7
        pshufd	xmm1, xmm4, 78
        pshufd	xmm2, xmm5, 78
        movdqa	xmm3, xmm5
        movdqa	xmm0, xmm5
        pclmulqdq	xmm3, xmm4, 17
        pclmulqdq	xmm0, xmm4, 0
        pxor	xmm1, xmm4
        pxor	xmm2, xmm5
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm6, xmm0
        movdqa	xmm4, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm6, xmm2
        pxor	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm4
        psrld	xmm0, 31
        psrld	xmm1, 31
        pslld	xmm6, 1
        pslld	xmm4, 1
        movdqa	xmm2, xmm0
        pslldq	xmm0, 4
        psrldq	xmm2, 12
        pslldq	xmm1, 4
        por	xmm4, xmm2
        por	xmm6, xmm0
        por	xmm4, xmm1
        movdqa	xmm0, xmm6
        movdqa	xmm1, xmm6
        movdqa	xmm2, xmm6
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm6, xmm0
        movdqa	xmm2, xmm6
        movdqa	xmm3, xmm6
        movdqa	xmm0, xmm6
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm6
        pxor	xmm4, xmm2
        movdqa	OWORD PTR [rdx], xmm4
        movdqu	xmm6, [rsp]
        movdqu	xmm7, [rsp+16]
        add	rsp, 32
        ret
AES_GCM_ghash_block_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_update_aesni PROC
        push	r13
        push	r12
        push	r14
        push	r15
        push	rdi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	r15, QWORD PTR [rsp+104]
        sub	rsp, 320
        movdqu	[rsp+160], xmm6
        movdqu	[rsp+176], xmm7
        movdqu	[rsp+192], xmm8
        movdqu	[rsp+208], xmm9
        movdqu	[rsp+224], xmm10
        movdqu	[rsp+240], xmm11
        movdqu	[rsp+256], xmm12
        movdqu	[rsp+272], xmm13
        movdqu	[rsp+288], xmm14
        movdqu	[rsp+304], xmm15
        movdqa	xmm6, OWORD PTR [r12]
        movdqa	xmm5, OWORD PTR [r14]
        movdqa	xmm9, xmm5
        movdqa	xmm8, xmm5
        psrlq	xmm9, 63
        psllq	xmm8, 1
        pslldq	xmm9, 8
        por	xmm8, xmm9
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm8
        xor	rdi, rdi
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_encrypt_update_aesni_done_128
        and	r13d, 4294967168
        movdqa	xmm2, xmm6
        ; H ^ 1
        movdqu	[rsp], xmm5
        ; H ^ 2
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm5, 78
        movdqa	xmm11, xmm5
        movdqa	xmm8, xmm5
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm5
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm0, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm0, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm0, xmm14
        movdqu	[rsp+16], xmm0
        ; H ^ 3
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm1, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm1, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm1, xmm14
        movdqu	[rsp+32], xmm1
        ; H ^ 4
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm3, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm3, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm3, xmm14
        movdqu	[rsp+48], xmm3
        ; H ^ 5
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+64], xmm7
        ; H ^ 6
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+80], xmm7
        ; H ^ 7
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+96], xmm7
        ; H ^ 8
        pshufd	xmm9, xmm3, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm3, 17
        pclmulqdq	xmm8, xmm3, 0
        pxor	xmm9, xmm3
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+112], xmm7
        ; First 128 bytes of input
        movdqu	xmm8, [r15]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [rax]
        movdqu	[r15], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+16]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+32]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+48]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+64]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+80]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+96]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+112]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+128]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+144]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r8d, 11
        movdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_aesni_enc_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r8d, 13
        movdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_aesni_enc_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_aesni_enc_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [r11]
        movdqu	xmm1, [r11+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[r10], xmm8
        movdqu	[r10+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [r11+32]
        movdqu	xmm1, [r11+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[r10+32], xmm10
        movdqu	[r10+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [r11+64]
        movdqu	xmm1, [r11+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[r10+64], xmm12
        movdqu	[r10+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [r11+96]
        movdqu	xmm1, [r11+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[r10+96], xmm14
        movdqu	[r10+112], xmm15
        cmp	r13d, 128
        mov	edi, 128
        jle	L_AES_GCM_encrypt_update_aesni_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_update_aesni_ghash_128:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        movdqu	xmm8, [r15]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [rax]
        movdqu	[r15], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqu	xmm7, [rsp+112]
        movdqu	xmm0, [rdx+-128]
        aesenc	xmm8, [rax+16]
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm0, xmm2
        pshufd	xmm1, xmm7, 78
        pshufd	xmm5, xmm0, 78
        pxor	xmm1, xmm7
        pxor	xmm5, xmm0
        movdqa	xmm3, xmm0
        pclmulqdq	xmm3, xmm7, 17
        aesenc	xmm9, [rax+16]
        aesenc	xmm10, [rax+16]
        movdqa	xmm2, xmm0
        pclmulqdq	xmm2, xmm7, 0
        aesenc	xmm11, [rax+16]
        aesenc	xmm12, [rax+16]
        pclmulqdq	xmm1, xmm5, 0
        aesenc	xmm13, [rax+16]
        aesenc	xmm14, [rax+16]
        aesenc	xmm15, [rax+16]
        pxor	xmm1, xmm2
        pxor	xmm1, xmm3
        movdqu	xmm7, [rsp+96]
        movdqu	xmm0, [rdx+-112]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+32]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+32]
        aesenc	xmm10, [rax+32]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+32]
        aesenc	xmm12, [rax+32]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+32]
        aesenc	xmm14, [rax+32]
        aesenc	xmm15, [rax+32]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+80]
        movdqu	xmm0, [rdx+-96]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+48]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+48]
        aesenc	xmm10, [rax+48]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+48]
        aesenc	xmm12, [rax+48]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+48]
        aesenc	xmm14, [rax+48]
        aesenc	xmm15, [rax+48]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+64]
        movdqu	xmm0, [rdx+-80]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+64]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+64]
        aesenc	xmm10, [rax+64]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+64]
        aesenc	xmm12, [rax+64]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+64]
        aesenc	xmm14, [rax+64]
        aesenc	xmm15, [rax+64]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+48]
        movdqu	xmm0, [rdx+-64]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+80]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+80]
        aesenc	xmm10, [rax+80]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+80]
        aesenc	xmm12, [rax+80]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+80]
        aesenc	xmm14, [rax+80]
        aesenc	xmm15, [rax+80]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+32]
        movdqu	xmm0, [rdx+-48]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+96]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+96]
        aesenc	xmm10, [rax+96]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+96]
        aesenc	xmm12, [rax+96]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+96]
        aesenc	xmm14, [rax+96]
        aesenc	xmm15, [rax+96]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+16]
        movdqu	xmm0, [rdx+-32]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+112]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+112]
        aesenc	xmm10, [rax+112]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+112]
        aesenc	xmm12, [rax+112]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+112]
        aesenc	xmm14, [rax+112]
        aesenc	xmm15, [rax+112]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp]
        movdqu	xmm0, [rdx+-16]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+128]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+128]
        aesenc	xmm10, [rax+128]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+128]
        aesenc	xmm12, [rax+128]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+128]
        aesenc	xmm14, [rax+128]
        aesenc	xmm15, [rax+128]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqa	xmm5, xmm1
        psrldq	xmm1, 8
        pslldq	xmm5, 8
        aesenc	xmm8, [rax+144]
        pxor	xmm2, xmm5
        pxor	xmm3, xmm1
        movdqa	xmm7, xmm2
        movdqa	xmm4, xmm2
        movdqa	xmm5, xmm2
        aesenc	xmm9, [rax+144]
        pslld	xmm7, 31
        pslld	xmm4, 30
        pslld	xmm5, 25
        aesenc	xmm10, [rax+144]
        pxor	xmm7, xmm4
        pxor	xmm7, xmm5
        aesenc	xmm11, [rax+144]
        movdqa	xmm4, xmm7
        pslldq	xmm7, 12
        psrldq	xmm4, 4
        aesenc	xmm12, [rax+144]
        pxor	xmm2, xmm7
        movdqa	xmm5, xmm2
        movdqa	xmm1, xmm2
        movdqa	xmm0, xmm2
        aesenc	xmm13, [rax+144]
        psrld	xmm5, 1
        psrld	xmm1, 2
        psrld	xmm0, 7
        aesenc	xmm14, [rax+144]
        pxor	xmm5, xmm1
        pxor	xmm5, xmm0
        aesenc	xmm15, [rax+144]
        pxor	xmm5, xmm4
        pxor	xmm2, xmm5
        pxor	xmm2, xmm3
        cmp	r8d, 11
        movdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r8d, 13
        movdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_aesni_aesenc_128_ghash_avx_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [rcx]
        movdqu	xmm1, [rcx+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[rdx], xmm8
        movdqu	[rdx+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [rcx+32]
        movdqu	xmm1, [rcx+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[rdx+32], xmm10
        movdqu	[rdx+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [rcx+64]
        movdqu	xmm1, [rcx+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[rdx+64], xmm12
        movdqu	[rdx+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [rcx+96]
        movdqu	xmm1, [rcx+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[rdx+96], xmm14
        movdqu	[rdx+112], xmm15
        add	edi, 128
        cmp	edi, r13d
        jl	L_AES_GCM_encrypt_update_aesni_ghash_128
L_AES_GCM_encrypt_update_aesni_end_128:
        movdqa	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        pshufb	xmm8, xmm4
        pshufb	xmm9, xmm4
        pshufb	xmm10, xmm4
        pshufb	xmm11, xmm4
        pxor	xmm8, xmm2
        pshufb	xmm12, xmm4
        pshufb	xmm13, xmm4
        pshufb	xmm14, xmm4
        pshufb	xmm15, xmm4
        movdqu	xmm7, [rsp+112]
        pshufd	xmm1, xmm8, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm8, 17
        pclmulqdq	xmm0, xmm8, 0
        pxor	xmm1, xmm8
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        movdqa	xmm4, xmm0
        movdqa	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+96]
        pshufd	xmm1, xmm9, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm9, 17
        pclmulqdq	xmm0, xmm9, 0
        pxor	xmm1, xmm9
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+80]
        pshufd	xmm1, xmm10, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm10, 17
        pclmulqdq	xmm0, xmm10, 0
        pxor	xmm1, xmm10
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+64]
        pshufd	xmm1, xmm11, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm11, 17
        pclmulqdq	xmm0, xmm11, 0
        pxor	xmm1, xmm11
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+48]
        pshufd	xmm1, xmm12, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm12, 17
        pclmulqdq	xmm0, xmm12, 0
        pxor	xmm1, xmm12
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+32]
        pshufd	xmm1, xmm13, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm13, 17
        pclmulqdq	xmm0, xmm13, 0
        pxor	xmm1, xmm13
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp+16]
        pshufd	xmm1, xmm14, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm14, 17
        pclmulqdq	xmm0, xmm14, 0
        pxor	xmm1, xmm14
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqu	xmm7, [rsp]
        pshufd	xmm1, xmm15, 78
        pshufd	xmm2, xmm7, 78
        movdqa	xmm3, xmm7
        movdqa	xmm0, xmm7
        pclmulqdq	xmm3, xmm15, 17
        pclmulqdq	xmm0, xmm15, 0
        pxor	xmm1, xmm15
        pxor	xmm2, xmm7
        pclmulqdq	xmm1, xmm2, 0
        pxor	xmm1, xmm0
        pxor	xmm1, xmm3
        movdqa	xmm2, xmm1
        pxor	xmm4, xmm0
        pxor	xmm6, xmm3
        pslldq	xmm2, 8
        psrldq	xmm1, 8
        pxor	xmm4, xmm2
        pxor	xmm6, xmm1
        movdqa	xmm0, xmm4
        movdqa	xmm1, xmm4
        movdqa	xmm2, xmm4
        pslld	xmm0, 31
        pslld	xmm1, 30
        pslld	xmm2, 25
        pxor	xmm0, xmm1
        pxor	xmm0, xmm2
        movdqa	xmm1, xmm0
        psrldq	xmm1, 4
        pslldq	xmm0, 12
        pxor	xmm4, xmm0
        movdqa	xmm2, xmm4
        movdqa	xmm3, xmm4
        movdqa	xmm0, xmm4
        psrld	xmm2, 1
        psrld	xmm3, 2
        psrld	xmm0, 7
        pxor	xmm2, xmm3
        pxor	xmm2, xmm0
        pxor	xmm2, xmm1
        pxor	xmm2, xmm4
        pxor	xmm6, xmm2
        movdqu	xmm5, [rsp]
L_AES_GCM_encrypt_update_aesni_done_128:
        mov	edx, r9d
        cmp	edi, edx
        jge	L_AES_GCM_encrypt_update_aesni_done_enc
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	edi, r13d
        jge	L_AES_GCM_encrypt_update_aesni_last_block_done
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        movdqu	xmm8, [r15]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [rax]
        movdqu	[r15], xmm9
        aesenc	xmm8, [rax+16]
        aesenc	xmm8, [rax+32]
        aesenc	xmm8, [rax+48]
        aesenc	xmm8, [rax+64]
        aesenc	xmm8, [rax+80]
        aesenc	xmm8, [rax+96]
        aesenc	xmm8, [rax+112]
        aesenc	xmm8, [rax+128]
        aesenc	xmm8, [rax+144]
        cmp	r8d, 11
        movdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+176]
        cmp	r8d, 13
        movdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+208]
        movdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_aesni_aesenc_block_aesenc_avx_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        add	edi, 16
        cmp	edi, r13d
        jge	L_AES_GCM_encrypt_update_aesni_last_block_ghash
L_AES_GCM_encrypt_update_aesni_last_block_start:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        movdqu	xmm8, [r15]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [rax]
        movdqu	[r15], xmm9
        movdqa	xmm10, xmm6
        pclmulqdq	xmm10, xmm5, 16
        aesenc	xmm8, [rax+16]
        aesenc	xmm8, [rax+32]
        movdqa	xmm11, xmm6
        pclmulqdq	xmm11, xmm5, 1
        aesenc	xmm8, [rax+48]
        aesenc	xmm8, [rax+64]
        movdqa	xmm12, xmm6
        pclmulqdq	xmm12, xmm5, 0
        aesenc	xmm8, [rax+80]
        movdqa	xmm1, xmm6
        pclmulqdq	xmm1, xmm5, 17
        aesenc	xmm8, [rax+96]
        pxor	xmm10, xmm11
        movdqa	xmm2, xmm10
        psrldq	xmm10, 8
        pslldq	xmm2, 8
        aesenc	xmm8, [rax+112]
        movdqa	xmm3, xmm1
        pxor	xmm2, xmm12
        pxor	xmm3, xmm10
        movdqa	xmm0, OWORD PTR L_aes_gcm_mod2_128
        movdqa	xmm11, xmm2
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [rax+128]
        pshufd	xmm10, xmm2, 78
        pxor	xmm10, xmm11
        movdqa	xmm11, xmm10
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [rax+144]
        pshufd	xmm6, xmm10, 78
        pxor	xmm6, xmm11
        pxor	xmm6, xmm3
        cmp	r8d, 11
        movdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+176]
        cmp	r8d, 13
        movdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+208]
        movdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_aesni_aesenc_gfmul_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm6, xmm8
        add	edi, 16
        cmp	edi, r13d
        jl	L_AES_GCM_encrypt_update_aesni_last_block_start
L_AES_GCM_encrypt_update_aesni_last_block_ghash:
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm6, 78
        movdqa	xmm11, xmm6
        movdqa	xmm8, xmm6
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm6
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm6, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm6, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm6, xmm14
L_AES_GCM_encrypt_update_aesni_last_block_done:
L_AES_GCM_encrypt_update_aesni_done_enc:
        movdqa	OWORD PTR [r12], xmm6
        movdqu	xmm6, [rsp+160]
        movdqu	xmm7, [rsp+176]
        movdqu	xmm8, [rsp+192]
        movdqu	xmm9, [rsp+208]
        movdqu	xmm10, [rsp+224]
        movdqu	xmm11, [rsp+240]
        movdqu	xmm12, [rsp+256]
        movdqu	xmm13, [rsp+272]
        movdqu	xmm14, [rsp+288]
        movdqu	xmm15, [rsp+304]
        add	rsp, 320
        pop	rdi
        pop	r15
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_encrypt_update_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_final_aesni PROC
        push	r13
        push	r12
        push	r14
        mov	rax, rcx
        mov	r10d, r9d
        mov	r9, rdx
        mov	r11d, DWORD PTR [rsp+64]
        mov	r12, QWORD PTR [rsp+72]
        mov	r14, QWORD PTR [rsp+80]
        sub	rsp, 144
        movdqu	[rsp+16], xmm6
        movdqu	[rsp+32], xmm7
        movdqu	[rsp+48], xmm8
        movdqu	[rsp+64], xmm9
        movdqu	[rsp+80], xmm10
        movdqu	[rsp+96], xmm11
        movdqu	[rsp+112], xmm12
        movdqu	[rsp+128], xmm13
        movdqa	xmm4, OWORD PTR [rax]
        movdqa	xmm5, OWORD PTR [r12]
        movdqa	xmm6, OWORD PTR [r14]
        movdqa	xmm8, xmm5
        movdqa	xmm7, xmm5
        psrlq	xmm8, 63
        psllq	xmm7, 1
        pslldq	xmm8, 8
        por	xmm7, xmm8
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm7
        mov	edx, r10d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        pinsrq	xmm0, rdx, 0
        pinsrq	xmm0, rcx, 1
        pxor	xmm4, xmm0
        pshufd	xmm8, xmm5, 78
        pshufd	xmm9, xmm4, 78
        movdqa	xmm10, xmm4
        movdqa	xmm7, xmm4
        pclmulqdq	xmm10, xmm5, 17
        pclmulqdq	xmm7, xmm5, 0
        pxor	xmm8, xmm5
        pxor	xmm9, xmm4
        pclmulqdq	xmm8, xmm9, 0
        pxor	xmm8, xmm7
        pxor	xmm8, xmm10
        movdqa	xmm9, xmm8
        movdqa	xmm4, xmm10
        pslldq	xmm9, 8
        psrldq	xmm8, 8
        pxor	xmm7, xmm9
        pxor	xmm4, xmm8
        movdqa	xmm11, xmm7
        movdqa	xmm12, xmm7
        movdqa	xmm13, xmm7
        pslld	xmm11, 31
        pslld	xmm12, 30
        pslld	xmm13, 25
        pxor	xmm11, xmm12
        pxor	xmm11, xmm13
        movdqa	xmm12, xmm11
        psrldq	xmm12, 4
        pslldq	xmm11, 12
        pxor	xmm7, xmm11
        movdqa	xmm13, xmm7
        movdqa	xmm9, xmm7
        movdqa	xmm8, xmm7
        psrld	xmm13, 1
        psrld	xmm9, 2
        psrld	xmm8, 7
        pxor	xmm13, xmm9
        pxor	xmm13, xmm8
        pxor	xmm13, xmm12
        pxor	xmm13, xmm7
        pxor	xmm4, xmm13
        pshufb	xmm4, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	xmm0, xmm6
        pxor	xmm0, xmm4
        cmp	r8d, 16
        je	L_AES_GCM_encrypt_final_aesni_store_tag_16
        xor	rcx, rcx
        movdqu	[rsp], xmm0
L_AES_GCM_encrypt_final_aesni_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r9+rcx], r13b
        inc	ecx
        cmp	ecx, r8d
        jne	L_AES_GCM_encrypt_final_aesni_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_aesni_store_tag_done
L_AES_GCM_encrypt_final_aesni_store_tag_16:
        movdqu	[r9], xmm0
L_AES_GCM_encrypt_final_aesni_store_tag_done:
        movdqu	xmm6, [rsp+16]
        movdqu	xmm7, [rsp+32]
        movdqu	xmm8, [rsp+48]
        movdqu	xmm9, [rsp+64]
        movdqu	xmm10, [rsp+80]
        movdqu	xmm11, [rsp+96]
        movdqu	xmm12, [rsp+112]
        movdqu	xmm13, [rsp+128]
        add	rsp, 144
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_encrypt_final_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_update_aesni PROC
        push	r13
        push	r12
        push	r14
        push	r15
        push	rdi
        push	rsi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+88]
        mov	r12, QWORD PTR [rsp+96]
        mov	r14, QWORD PTR [rsp+104]
        mov	r15, QWORD PTR [rsp+112]
        sub	rsp, 328
        movdqu	[rsp+168], xmm6
        movdqu	[rsp+184], xmm7
        movdqu	[rsp+200], xmm8
        movdqu	[rsp+216], xmm9
        movdqu	[rsp+232], xmm10
        movdqu	[rsp+248], xmm11
        movdqu	[rsp+264], xmm12
        movdqu	[rsp+280], xmm13
        movdqu	[rsp+296], xmm14
        movdqu	[rsp+312], xmm15
        movdqa	xmm6, OWORD PTR [r12]
        movdqa	xmm5, OWORD PTR [r14]
        movdqa	xmm9, xmm5
        movdqa	xmm8, xmm5
        psrlq	xmm9, 63
        psllq	xmm8, 1
        pslldq	xmm9, 8
        por	xmm8, xmm9
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm8
        xor	edi, edi
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_decrypt_update_aesni_done_128
        and	r13d, 4294967168
        movdqa	xmm2, xmm6
        ; H ^ 1
        movdqu	[rsp], xmm5
        ; H ^ 2
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm5, 78
        movdqa	xmm11, xmm5
        movdqa	xmm8, xmm5
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm5
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm0, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm0, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm0, xmm14
        movdqu	[rsp+16], xmm0
        ; H ^ 3
        pshufd	xmm9, xmm5, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm5, 17
        pclmulqdq	xmm8, xmm5, 0
        pxor	xmm9, xmm5
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm1, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm1, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm1, xmm14
        movdqu	[rsp+32], xmm1
        ; H ^ 4
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm0, 78
        movdqa	xmm11, xmm0
        movdqa	xmm8, xmm0
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm0
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm3, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm3, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm3, xmm14
        movdqu	[rsp+48], xmm3
        ; H ^ 5
        pshufd	xmm9, xmm0, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm0, 17
        pclmulqdq	xmm8, xmm0, 0
        pxor	xmm9, xmm0
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+64], xmm7
        ; H ^ 6
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm1, 78
        movdqa	xmm11, xmm1
        movdqa	xmm8, xmm1
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm1
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+80], xmm7
        ; H ^ 7
        pshufd	xmm9, xmm1, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm1, 17
        pclmulqdq	xmm8, xmm1, 0
        pxor	xmm9, xmm1
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+96], xmm7
        ; H ^ 8
        pshufd	xmm9, xmm3, 78
        pshufd	xmm10, xmm3, 78
        movdqa	xmm11, xmm3
        movdqa	xmm8, xmm3
        pclmulqdq	xmm11, xmm3, 17
        pclmulqdq	xmm8, xmm3, 0
        pxor	xmm9, xmm3
        pxor	xmm10, xmm3
        pclmulqdq	xmm9, xmm10, 0
        pxor	xmm9, xmm8
        pxor	xmm9, xmm11
        movdqa	xmm10, xmm9
        movdqa	xmm7, xmm11
        pslldq	xmm10, 8
        psrldq	xmm9, 8
        pxor	xmm8, xmm10
        pxor	xmm7, xmm9
        movdqa	xmm12, xmm8
        movdqa	xmm13, xmm8
        movdqa	xmm14, xmm8
        pslld	xmm12, 31
        pslld	xmm13, 30
        pslld	xmm14, 25
        pxor	xmm12, xmm13
        pxor	xmm12, xmm14
        movdqa	xmm13, xmm12
        psrldq	xmm13, 4
        pslldq	xmm12, 12
        pxor	xmm8, xmm12
        movdqa	xmm14, xmm8
        movdqa	xmm10, xmm8
        movdqa	xmm9, xmm8
        psrld	xmm14, 1
        psrld	xmm10, 2
        psrld	xmm9, 7
        pxor	xmm14, xmm10
        pxor	xmm14, xmm9
        pxor	xmm14, xmm13
        pxor	xmm14, xmm8
        pxor	xmm7, xmm14
        movdqu	[rsp+112], xmm7
L_AES_GCM_decrypt_update_aesni_ghash_128:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        movdqu	xmm8, [r15]
        movdqa	xmm1, OWORD PTR L_aes_gcm_bswap_epi64
        movdqa	xmm0, xmm8
        pshufb	xmm8, xmm1
        movdqa	xmm9, xmm0
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pshufb	xmm9, xmm1
        movdqa	xmm10, xmm0
        paddd	xmm10, OWORD PTR L_aes_gcm_two
        pshufb	xmm10, xmm1
        movdqa	xmm11, xmm0
        paddd	xmm11, OWORD PTR L_aes_gcm_three
        pshufb	xmm11, xmm1
        movdqa	xmm12, xmm0
        paddd	xmm12, OWORD PTR L_aes_gcm_four
        pshufb	xmm12, xmm1
        movdqa	xmm13, xmm0
        paddd	xmm13, OWORD PTR L_aes_gcm_five
        pshufb	xmm13, xmm1
        movdqa	xmm14, xmm0
        paddd	xmm14, OWORD PTR L_aes_gcm_six
        pshufb	xmm14, xmm1
        movdqa	xmm15, xmm0
        paddd	xmm15, OWORD PTR L_aes_gcm_seven
        pshufb	xmm15, xmm1
        paddd	xmm0, OWORD PTR L_aes_gcm_eight
        movdqa	xmm7, OWORD PTR [rax]
        movdqu	[r15], xmm0
        pxor	xmm8, xmm7
        pxor	xmm9, xmm7
        pxor	xmm10, xmm7
        pxor	xmm11, xmm7
        pxor	xmm12, xmm7
        pxor	xmm13, xmm7
        pxor	xmm14, xmm7
        pxor	xmm15, xmm7
        movdqu	xmm7, [rsp+112]
        movdqu	xmm0, [rcx]
        aesenc	xmm8, [rax+16]
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm0, xmm2
        pshufd	xmm1, xmm7, 78
        pshufd	xmm5, xmm0, 78
        pxor	xmm1, xmm7
        pxor	xmm5, xmm0
        movdqa	xmm3, xmm0
        pclmulqdq	xmm3, xmm7, 17
        aesenc	xmm9, [rax+16]
        aesenc	xmm10, [rax+16]
        movdqa	xmm2, xmm0
        pclmulqdq	xmm2, xmm7, 0
        aesenc	xmm11, [rax+16]
        aesenc	xmm12, [rax+16]
        pclmulqdq	xmm1, xmm5, 0
        aesenc	xmm13, [rax+16]
        aesenc	xmm14, [rax+16]
        aesenc	xmm15, [rax+16]
        pxor	xmm1, xmm2
        pxor	xmm1, xmm3
        movdqu	xmm7, [rsp+96]
        movdqu	xmm0, [rcx+16]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+32]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+32]
        aesenc	xmm10, [rax+32]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+32]
        aesenc	xmm12, [rax+32]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+32]
        aesenc	xmm14, [rax+32]
        aesenc	xmm15, [rax+32]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+80]
        movdqu	xmm0, [rcx+32]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+48]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+48]
        aesenc	xmm10, [rax+48]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+48]
        aesenc	xmm12, [rax+48]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+48]
        aesenc	xmm14, [rax+48]
        aesenc	xmm15, [rax+48]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+64]
        movdqu	xmm0, [rcx+48]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+64]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+64]
        aesenc	xmm10, [rax+64]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+64]
        aesenc	xmm12, [rax+64]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+64]
        aesenc	xmm14, [rax+64]
        aesenc	xmm15, [rax+64]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+48]
        movdqu	xmm0, [rcx+64]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+80]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+80]
        aesenc	xmm10, [rax+80]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+80]
        aesenc	xmm12, [rax+80]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+80]
        aesenc	xmm14, [rax+80]
        aesenc	xmm15, [rax+80]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+32]
        movdqu	xmm0, [rcx+80]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+96]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+96]
        aesenc	xmm10, [rax+96]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+96]
        aesenc	xmm12, [rax+96]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+96]
        aesenc	xmm14, [rax+96]
        aesenc	xmm15, [rax+96]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp+16]
        movdqu	xmm0, [rcx+96]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+112]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+112]
        aesenc	xmm10, [rax+112]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+112]
        aesenc	xmm12, [rax+112]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+112]
        aesenc	xmm14, [rax+112]
        aesenc	xmm15, [rax+112]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqu	xmm7, [rsp]
        movdqu	xmm0, [rcx+112]
        pshufd	xmm4, xmm7, 78
        pshufb	xmm0, OWORD PTR L_aes_gcm_bswap_mask
        aesenc	xmm8, [rax+128]
        pxor	xmm4, xmm7
        pshufd	xmm5, xmm0, 78
        pxor	xmm5, xmm0
        movdqa	xmm6, xmm0
        pclmulqdq	xmm6, xmm7, 17
        aesenc	xmm9, [rax+128]
        aesenc	xmm10, [rax+128]
        pclmulqdq	xmm7, xmm0, 0
        aesenc	xmm11, [rax+128]
        aesenc	xmm12, [rax+128]
        pclmulqdq	xmm4, xmm5, 0
        aesenc	xmm13, [rax+128]
        aesenc	xmm14, [rax+128]
        aesenc	xmm15, [rax+128]
        pxor	xmm1, xmm7
        pxor	xmm2, xmm7
        pxor	xmm1, xmm6
        pxor	xmm3, xmm6
        pxor	xmm1, xmm4
        movdqa	xmm5, xmm1
        psrldq	xmm1, 8
        pslldq	xmm5, 8
        aesenc	xmm8, [rax+144]
        pxor	xmm2, xmm5
        pxor	xmm3, xmm1
        movdqa	xmm7, xmm2
        movdqa	xmm4, xmm2
        movdqa	xmm5, xmm2
        aesenc	xmm9, [rax+144]
        pslld	xmm7, 31
        pslld	xmm4, 30
        pslld	xmm5, 25
        aesenc	xmm10, [rax+144]
        pxor	xmm7, xmm4
        pxor	xmm7, xmm5
        aesenc	xmm11, [rax+144]
        movdqa	xmm4, xmm7
        pslldq	xmm7, 12
        psrldq	xmm4, 4
        aesenc	xmm12, [rax+144]
        pxor	xmm2, xmm7
        movdqa	xmm5, xmm2
        movdqa	xmm1, xmm2
        movdqa	xmm0, xmm2
        aesenc	xmm13, [rax+144]
        psrld	xmm5, 1
        psrld	xmm1, 2
        psrld	xmm0, 7
        aesenc	xmm14, [rax+144]
        pxor	xmm5, xmm1
        pxor	xmm5, xmm0
        aesenc	xmm15, [rax+144]
        pxor	xmm5, xmm4
        pxor	xmm2, xmm5
        pxor	xmm2, xmm3
        cmp	r8d, 11
        movdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+176]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        cmp	r8d, 13
        movdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_128_ghash_avx_done
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+208]
        aesenc	xmm8, xmm7
        aesenc	xmm9, xmm7
        aesenc	xmm10, xmm7
        aesenc	xmm11, xmm7
        aesenc	xmm12, xmm7
        aesenc	xmm13, xmm7
        aesenc	xmm14, xmm7
        aesenc	xmm15, xmm7
        movdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_aesni_aesenc_128_ghash_avx_done:
        aesenclast	xmm8, xmm7
        aesenclast	xmm9, xmm7
        movdqu	xmm0, [rcx]
        movdqu	xmm1, [rcx+16]
        pxor	xmm8, xmm0
        pxor	xmm9, xmm1
        movdqu	[rdx], xmm8
        movdqu	[rdx+16], xmm9
        aesenclast	xmm10, xmm7
        aesenclast	xmm11, xmm7
        movdqu	xmm0, [rcx+32]
        movdqu	xmm1, [rcx+48]
        pxor	xmm10, xmm0
        pxor	xmm11, xmm1
        movdqu	[rdx+32], xmm10
        movdqu	[rdx+48], xmm11
        aesenclast	xmm12, xmm7
        aesenclast	xmm13, xmm7
        movdqu	xmm0, [rcx+64]
        movdqu	xmm1, [rcx+80]
        pxor	xmm12, xmm0
        pxor	xmm13, xmm1
        movdqu	[rdx+64], xmm12
        movdqu	[rdx+80], xmm13
        aesenclast	xmm14, xmm7
        aesenclast	xmm15, xmm7
        movdqu	xmm0, [rcx+96]
        movdqu	xmm1, [rcx+112]
        pxor	xmm14, xmm0
        pxor	xmm15, xmm1
        movdqu	[rdx+96], xmm14
        movdqu	[rdx+112], xmm15
        add	edi, 128
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_aesni_ghash_128
        movdqa	xmm6, xmm2
        movdqu	xmm5, [rsp]
L_AES_GCM_decrypt_update_aesni_done_128:
        mov	edx, r9d
        cmp	edi, edx
        jge	L_AES_GCM_decrypt_update_aesni_done_dec
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	edi, r13d
        jge	L_AES_GCM_decrypt_update_aesni_last_block_done
L_AES_GCM_decrypt_update_aesni_last_block_start:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        movdqu	xmm1, [rcx]
        movdqa	xmm0, xmm5
        pshufb	xmm1, OWORD PTR L_aes_gcm_bswap_mask
        pxor	xmm1, xmm6
        movdqu	xmm8, [r15]
        movdqa	xmm9, xmm8
        pshufb	xmm8, OWORD PTR L_aes_gcm_bswap_epi64
        paddd	xmm9, OWORD PTR L_aes_gcm_one
        pxor	xmm8, [rax]
        movdqu	[r15], xmm9
        movdqa	xmm10, xmm1
        pclmulqdq	xmm10, xmm0, 16
        aesenc	xmm8, [rax+16]
        aesenc	xmm8, [rax+32]
        movdqa	xmm11, xmm1
        pclmulqdq	xmm11, xmm0, 1
        aesenc	xmm8, [rax+48]
        aesenc	xmm8, [rax+64]
        movdqa	xmm12, xmm1
        pclmulqdq	xmm12, xmm0, 0
        aesenc	xmm8, [rax+80]
        movdqa	xmm1, xmm1
        pclmulqdq	xmm1, xmm0, 17
        aesenc	xmm8, [rax+96]
        pxor	xmm10, xmm11
        movdqa	xmm2, xmm10
        psrldq	xmm10, 8
        pslldq	xmm2, 8
        aesenc	xmm8, [rax+112]
        movdqa	xmm3, xmm1
        pxor	xmm2, xmm12
        pxor	xmm3, xmm10
        movdqa	xmm0, OWORD PTR L_aes_gcm_mod2_128
        movdqa	xmm11, xmm2
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [rax+128]
        pshufd	xmm10, xmm2, 78
        pxor	xmm10, xmm11
        movdqa	xmm11, xmm10
        pclmulqdq	xmm11, xmm0, 16
        aesenc	xmm8, [rax+144]
        pshufd	xmm6, xmm10, 78
        pxor	xmm6, xmm11
        pxor	xmm6, xmm3
        cmp	r8d, 11
        movdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+176]
        cmp	r8d, 13
        movdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last
        aesenc	xmm8, xmm9
        aesenc	xmm8, [rax+208]
        movdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_aesni_aesenc_gfmul_last:
        aesenclast	xmm8, xmm9
        movdqu	xmm9, [rcx]
        pxor	xmm8, xmm9
        movdqu	[rdx], xmm8
        add	edi, 16
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_aesni_last_block_start
L_AES_GCM_decrypt_update_aesni_last_block_done:
L_AES_GCM_decrypt_update_aesni_done_dec:
        movdqa	OWORD PTR [r12], xmm6
        movdqu	xmm6, [rsp+168]
        movdqu	xmm7, [rsp+184]
        movdqu	xmm8, [rsp+200]
        movdqu	xmm9, [rsp+216]
        movdqu	xmm10, [rsp+232]
        movdqu	xmm11, [rsp+248]
        movdqu	xmm12, [rsp+264]
        movdqu	xmm13, [rsp+280]
        movdqu	xmm14, [rsp+296]
        movdqu	xmm15, [rsp+312]
        add	rsp, 328
        pop	rsi
        pop	rdi
        pop	r15
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_decrypt_update_aesni ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_final_aesni PROC
        push	r13
        push	r12
        push	r14
        push	rbp
        push	r15
        mov	rax, rcx
        mov	r10d, r9d
        mov	r9, rdx
        mov	r11d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	rbp, QWORD PTR [rsp+104]
        sub	rsp, 160
        movdqu	[rsp+16], xmm6
        movdqu	[rsp+32], xmm7
        movdqu	[rsp+48], xmm8
        movdqu	[rsp+64], xmm9
        movdqu	[rsp+80], xmm10
        movdqu	[rsp+96], xmm11
        movdqu	[rsp+112], xmm12
        movdqu	[rsp+128], xmm13
        movdqu	[rsp+144], xmm15
        movdqa	xmm6, OWORD PTR [rax]
        movdqa	xmm5, OWORD PTR [r12]
        movdqa	xmm15, OWORD PTR [r14]
        movdqa	xmm8, xmm5
        movdqa	xmm7, xmm5
        psrlq	xmm8, 63
        psllq	xmm7, 1
        pslldq	xmm8, 8
        por	xmm7, xmm8
        pshufd	xmm5, xmm5, 255
        psrad	xmm5, 31
        pand	xmm5, OWORD PTR L_aes_gcm_mod2_128
        pxor	xmm5, xmm7
        mov	edx, r10d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        pinsrq	xmm0, rdx, 0
        pinsrq	xmm0, rcx, 1
        pxor	xmm6, xmm0
        pshufd	xmm8, xmm5, 78
        pshufd	xmm9, xmm6, 78
        movdqa	xmm10, xmm6
        movdqa	xmm7, xmm6
        pclmulqdq	xmm10, xmm5, 17
        pclmulqdq	xmm7, xmm5, 0
        pxor	xmm8, xmm5
        pxor	xmm9, xmm6
        pclmulqdq	xmm8, xmm9, 0
        pxor	xmm8, xmm7
        pxor	xmm8, xmm10
        movdqa	xmm9, xmm8
        movdqa	xmm6, xmm10
        pslldq	xmm9, 8
        psrldq	xmm8, 8
        pxor	xmm7, xmm9
        pxor	xmm6, xmm8
        movdqa	xmm11, xmm7
        movdqa	xmm12, xmm7
        movdqa	xmm13, xmm7
        pslld	xmm11, 31
        pslld	xmm12, 30
        pslld	xmm13, 25
        pxor	xmm11, xmm12
        pxor	xmm11, xmm13
        movdqa	xmm12, xmm11
        psrldq	xmm12, 4
        pslldq	xmm11, 12
        pxor	xmm7, xmm11
        movdqa	xmm13, xmm7
        movdqa	xmm9, xmm7
        movdqa	xmm8, xmm7
        psrld	xmm13, 1
        psrld	xmm9, 2
        psrld	xmm8, 7
        pxor	xmm13, xmm9
        pxor	xmm13, xmm8
        pxor	xmm13, xmm12
        pxor	xmm13, xmm7
        pxor	xmm6, xmm13
        pshufb	xmm6, OWORD PTR L_aes_gcm_bswap_mask
        movdqu	xmm0, xmm15
        pxor	xmm0, xmm6
        cmp	r8d, 16
        je	L_AES_GCM_decrypt_final_aesni_cmp_tag_16
        sub	rsp, 16
        xor	rcx, rcx
        xor	r15, r15
        movdqu	[rsp], xmm0
L_AES_GCM_decrypt_final_aesni_cmp_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        xor	r13b, BYTE PTR [r9+rcx]
        or	r15b, r13b
        inc	ecx
        cmp	ecx, r8d
        jne	L_AES_GCM_decrypt_final_aesni_cmp_tag_loop
        cmp	r15, 0
        sete	r15b
        add	rsp, 16
        xor	rcx, rcx
        jmp	L_AES_GCM_decrypt_final_aesni_cmp_tag_done
L_AES_GCM_decrypt_final_aesni_cmp_tag_16:
        movdqu	xmm1, [r9]
        pcmpeqb	xmm0, xmm1
        pmovmskb	rdx, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	r15d, r15d
        cmp	edx, 65535
        sete	r15b
L_AES_GCM_decrypt_final_aesni_cmp_tag_done:
        mov	DWORD PTR [rbp], r15d
        movdqu	xmm6, [rsp+16]
        movdqu	xmm7, [rsp+32]
        movdqu	xmm8, [rsp+48]
        movdqu	xmm9, [rsp+64]
        movdqu	xmm10, [rsp+80]
        movdqu	xmm11, [rsp+96]
        movdqu	xmm12, [rsp+112]
        movdqu	xmm13, [rsp+128]
        movdqu	xmm15, [rsp+144]
        add	rsp, 160
        pop	r15
        pop	rbp
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_decrypt_final_aesni ENDP
_text ENDS
IFDEF HAVE_INTEL_AVX1
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_one QWORD 0, 1
ptr_L_avx1_aes_gcm_one QWORD L_avx1_aes_gcm_one
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_two QWORD 0, 2
ptr_L_avx1_aes_gcm_two QWORD L_avx1_aes_gcm_two
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_three QWORD 0, 3
ptr_L_avx1_aes_gcm_three QWORD L_avx1_aes_gcm_three
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_four QWORD 0, 4
ptr_L_avx1_aes_gcm_four QWORD L_avx1_aes_gcm_four
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_five QWORD 0, 5
ptr_L_avx1_aes_gcm_five QWORD L_avx1_aes_gcm_five
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_six QWORD 0, 6
ptr_L_avx1_aes_gcm_six QWORD L_avx1_aes_gcm_six
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_seven QWORD 0, 7
ptr_L_avx1_aes_gcm_seven QWORD L_avx1_aes_gcm_seven
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_eight QWORD 0, 8
ptr_L_avx1_aes_gcm_eight QWORD L_avx1_aes_gcm_eight
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_bswap_epi64 QWORD 283686952306183, 579005069656919567
ptr_L_avx1_aes_gcm_bswap_epi64 QWORD L_avx1_aes_gcm_bswap_epi64
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_bswap_mask QWORD 579005069656919567, 283686952306183
ptr_L_avx1_aes_gcm_bswap_mask QWORD L_avx1_aes_gcm_bswap_mask
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx1_aes_gcm_mod2_128 QWORD 1, 13979173243358019584
ptr_L_avx1_aes_gcm_mod2_128 QWORD L_avx1_aes_gcm_mod2_128
_DATA ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_avx1 PROC
        push	r13
        push	rdi
        push	rsi
        push	r12
        push	rbx
        push	r14
        push	r15
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r12, r8
        mov	rax, r9
        mov	r8, QWORD PTR [rsp+96]
        mov	r9d, DWORD PTR [rsp+104]
        mov	r11d, DWORD PTR [rsp+112]
        mov	ebx, DWORD PTR [rsp+120]
        mov	r14d, DWORD PTR [rsp+128]
        mov	r15, QWORD PTR [rsp+136]
        mov	r10d, DWORD PTR [rsp+144]
        sub	rsp, 320
        vmovdqu	OWORD PTR [rsp+160], xmm6
        vmovdqu	OWORD PTR [rsp+176], xmm7
        vmovdqu	OWORD PTR [rsp+192], xmm8
        vmovdqu	OWORD PTR [rsp+208], xmm9
        vmovdqu	OWORD PTR [rsp+224], xmm10
        vmovdqu	OWORD PTR [rsp+240], xmm11
        vmovdqu	OWORD PTR [rsp+256], xmm12
        vmovdqu	OWORD PTR [rsp+272], xmm13
        vmovdqu	OWORD PTR [rsp+288], xmm14
        vmovdqu	OWORD PTR [rsp+304], xmm15
        vpxor	xmm4, xmm4, xmm4
        vpxor	xmm6, xmm6, xmm6
        mov	edx, ebx
        cmp	edx, 12
        jne	L_AES_GCM_encrypt_avx1_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        vmovq	xmm4, QWORD PTR [rax]
        vpinsrd	xmm4, xmm4, DWORD PTR [rax+8], 2
        vpinsrd	xmm4, xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	xmm5, OWORD PTR [r15]
        vpxor	xmm1, xmm4, xmm5
        vmovdqa	xmm7, OWORD PTR [r15+16]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+32]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+48]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+64]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+80]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+96]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+112]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+128]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+144]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        cmp	r10d, 11
        vmovdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+176]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        cmp	r10d, 13
        vmovdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+208]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm7
        vaesenclast	xmm1, xmm1, xmm7
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vmovdqu	OWORD PTR [rsp+144], xmm1
        jmp	L_AES_GCM_encrypt_avx1_iv_done
L_AES_GCM_encrypt_avx1_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqa	xmm5, OWORD PTR [r15]
        vaesenc	xmm5, xmm5, [r15+16]
        vaesenc	xmm5, xmm5, [r15+32]
        vaesenc	xmm5, xmm5, [r15+48]
        vaesenc	xmm5, xmm5, [r15+64]
        vaesenc	xmm5, xmm5, [r15+80]
        vaesenc	xmm5, xmm5, [r15+96]
        vaesenc	xmm5, xmm5, [r15+112]
        vaesenc	xmm5, xmm5, [r15+128]
        vaesenc	xmm5, xmm5, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm9
        vaesenc	xmm5, xmm5, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm9
        vaesenc	xmm5, xmm5, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm9
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_encrypt_avx1_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_avx1_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_avx1_calc_iv_16_loop:
        vmovdqu	xmm8, OWORD PTR [rax+rcx]
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx1_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_avx1_calc_iv_done
L_AES_GCM_encrypt_avx1_calc_iv_lt16:
        sub	rsp, 16
        vpxor	xmm8, xmm8, xmm8
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm8
L_AES_GCM_encrypt_avx1_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx1_calc_iv_loop
        vmovdqu	xmm8, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
L_AES_GCM_encrypt_avx1_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqa	xmm8, OWORD PTR [r15]
        vpxor	xmm8, xmm8, xmm4
        vaesenc	xmm8, xmm8, [r15+16]
        vaesenc	xmm8, xmm8, [r15+32]
        vaesenc	xmm8, xmm8, [r15+48]
        vaesenc	xmm8, xmm8, [r15+64]
        vaesenc	xmm8, xmm8, [r15+80]
        vaesenc	xmm8, xmm8, [r15+96]
        vaesenc	xmm8, xmm8, [r15+112]
        vaesenc	xmm8, xmm8, [r15+128]
        vaesenc	xmm8, xmm8, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqu	OWORD PTR [rsp+144], xmm8
L_AES_GCM_encrypt_avx1_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_encrypt_avx1_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_avx1_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_avx1_calc_aad_16_loop:
        vmovdqu	xmm8, OWORD PTR [r12+rcx]
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm6, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm6, xmm6, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx1_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_avx1_calc_aad_done
L_AES_GCM_encrypt_avx1_calc_aad_lt16:
        sub	rsp, 16
        vpxor	xmm8, xmm8, xmm8
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm8
L_AES_GCM_encrypt_avx1_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx1_calc_aad_loop
        vmovdqu	xmm8, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm6, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm6, xmm6, xmm2
L_AES_GCM_encrypt_avx1_calc_aad_done:
        ; Calculate counter and H
        vpsrlq	xmm9, xmm5, 63
        vpsllq	xmm8, xmm5, 1
        vpslldq	xmm9, xmm9, 8
        vpor	xmm8, xmm8, xmm9
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpaddd	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_one
        vpxor	xmm5, xmm5, xmm8
        vmovdqu	OWORD PTR [rsp+128], xmm4
        xor	ebx, ebx
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_encrypt_avx1_done_128
        and	r13d, 4294967168
        vmovdqa	xmm2, xmm6
        ; H ^ 1
        vmovdqu	OWORD PTR [rsp], xmm5
        ; H ^ 2
        vpclmulqdq	xmm8, xmm5, xmm5, 0
        vpclmulqdq	xmm0, xmm5, xmm5, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm0, xmm0, xmm14
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm0, 78
        vpclmulqdq	xmm11, xmm0, xmm5, 17
        vpclmulqdq	xmm8, xmm0, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm0
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm1, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm1, xmm1, xmm14
        vmovdqu	OWORD PTR [rsp+32], xmm1
        ; H ^ 4
        vpclmulqdq	xmm8, xmm0, xmm0, 0
        vpclmulqdq	xmm3, xmm0, xmm0, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm3, xmm3, xmm14
        vmovdqu	OWORD PTR [rsp+48], xmm3
        ; H ^ 5
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm0, 78
        vpshufd	xmm10, xmm1, 78
        vpclmulqdq	xmm11, xmm1, xmm0, 17
        vpclmulqdq	xmm8, xmm1, xmm0, 0
        vpxor	xmm9, xmm9, xmm0
        vpxor	xmm10, xmm10, xmm1
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+64], xmm7
        ; H ^ 6
        vpclmulqdq	xmm8, xmm1, xmm1, 0
        vpclmulqdq	xmm7, xmm1, xmm1, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+80], xmm7
        ; H ^ 7
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm1, 78
        vpshufd	xmm10, xmm3, 78
        vpclmulqdq	xmm11, xmm3, xmm1, 17
        vpclmulqdq	xmm8, xmm3, xmm1, 0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm3
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+96], xmm7
        ; H ^ 8
        vpclmulqdq	xmm8, xmm3, xmm3, 0
        vpclmulqdq	xmm7, xmm3, xmm3, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+112], xmm7
        ; First 128 bytes of input
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [r15]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+16]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+32]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+48]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+64]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+80]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+96]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+112]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+128]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+144]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r10d, 11
        vmovdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r10d, 13
        vmovdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_aesenc_128_enc_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi]
        vmovdqu	xmm1, OWORD PTR [rdi+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [rsi], xmm8
        vmovdqu	OWORD PTR [rsi+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi+32]
        vmovdqu	xmm1, OWORD PTR [rdi+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [rsi+32], xmm10
        vmovdqu	OWORD PTR [rsi+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi+64]
        vmovdqu	xmm1, OWORD PTR [rdi+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [rsi+64], xmm12
        vmovdqu	OWORD PTR [rsi+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi+96]
        vmovdqu	xmm1, OWORD PTR [rdi+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [rsi+96], xmm14
        vmovdqu	OWORD PTR [rsi+112], xmm15
        cmp	r13d, 128
        mov	ebx, 128
        jle	L_AES_GCM_encrypt_avx1_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_avx1_ghash_128:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [r15]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vmovdqu	xmm0, OWORD PTR [rdx+-128]
        vaesenc	xmm8, xmm8, [r15+16]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm2
        vpshufd	xmm1, xmm7, 78
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm3, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+16]
        vaesenc	xmm10, xmm10, [r15+16]
        vpclmulqdq	xmm2, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+16]
        vaesenc	xmm12, xmm12, [r15+16]
        vpclmulqdq	xmm1, xmm1, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+16]
        vaesenc	xmm14, xmm14, [r15+16]
        vaesenc	xmm15, xmm15, [r15+16]
        vpxor	xmm1, xmm1, xmm2
        vpxor	xmm1, xmm1, xmm3
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm0, OWORD PTR [rdx+-112]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+32]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+32]
        vaesenc	xmm10, xmm10, [r15+32]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+32]
        vaesenc	xmm12, xmm12, [r15+32]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+32]
        vaesenc	xmm14, xmm14, [r15+32]
        vaesenc	xmm15, xmm15, [r15+32]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vmovdqu	xmm0, OWORD PTR [rdx+-96]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+48]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+48]
        vaesenc	xmm10, xmm10, [r15+48]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+48]
        vaesenc	xmm12, xmm12, [r15+48]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+48]
        vaesenc	xmm14, xmm14, [r15+48]
        vaesenc	xmm15, xmm15, [r15+48]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm0, OWORD PTR [rdx+-80]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+64]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+64]
        vaesenc	xmm10, xmm10, [r15+64]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+64]
        vaesenc	xmm12, xmm12, [r15+64]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+64]
        vaesenc	xmm14, xmm14, [r15+64]
        vaesenc	xmm15, xmm15, [r15+64]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vmovdqu	xmm0, OWORD PTR [rdx+-64]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+80]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+80]
        vaesenc	xmm10, xmm10, [r15+80]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+80]
        vaesenc	xmm12, xmm12, [r15+80]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+80]
        vaesenc	xmm14, xmm14, [r15+80]
        vaesenc	xmm15, xmm15, [r15+80]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm0, OWORD PTR [rdx+-48]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+96]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+96]
        vaesenc	xmm10, xmm10, [r15+96]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+96]
        vaesenc	xmm12, xmm12, [r15+96]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+96]
        vaesenc	xmm14, xmm14, [r15+96]
        vaesenc	xmm15, xmm15, [r15+96]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vmovdqu	xmm0, OWORD PTR [rdx+-32]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+112]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+112]
        vaesenc	xmm10, xmm10, [r15+112]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+112]
        vaesenc	xmm12, xmm12, [r15+112]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+112]
        vaesenc	xmm14, xmm14, [r15+112]
        vaesenc	xmm15, xmm15, [r15+112]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm0, OWORD PTR [rdx+-16]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+128]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+128]
        vaesenc	xmm10, xmm10, [r15+128]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+128]
        vaesenc	xmm12, xmm12, [r15+128]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+128]
        vaesenc	xmm14, xmm14, [r15+128]
        vaesenc	xmm15, xmm15, [r15+128]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vpslldq	xmm5, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vaesenc	xmm8, xmm8, [r15+144]
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm3, xmm3, xmm1
        vaesenc	xmm9, xmm9, [r15+144]
        vpslld	xmm7, xmm2, 31
        vpslld	xmm4, xmm2, 30
        vpslld	xmm5, xmm2, 25
        vaesenc	xmm10, xmm10, [r15+144]
        vpxor	xmm7, xmm7, xmm4
        vpxor	xmm7, xmm7, xmm5
        vaesenc	xmm11, xmm11, [r15+144]
        vpsrldq	xmm4, xmm7, 4
        vpslldq	xmm7, xmm7, 12
        vaesenc	xmm12, xmm12, [r15+144]
        vpxor	xmm2, xmm2, xmm7
        vpsrld	xmm5, xmm2, 1
        vaesenc	xmm13, xmm13, [r15+144]
        vpsrld	xmm1, xmm2, 2
        vpsrld	xmm0, xmm2, 7
        vaesenc	xmm14, xmm14, [r15+144]
        vpxor	xmm5, xmm5, xmm1
        vpxor	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, [r15+144]
        vpxor	xmm5, xmm5, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm2, xmm2, xmm3
        cmp	r10d, 11
        vmovdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r10d, 13
        vmovdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_aesenc_128_ghash_avx_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_avx1_ghash_128
L_AES_GCM_encrypt_avx1_end_128:
        vmovdqa	xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpshufb	xmm8, xmm8, xmm4
        vpshufb	xmm9, xmm9, xmm4
        vpshufb	xmm10, xmm10, xmm4
        vpshufb	xmm11, xmm11, xmm4
        vpxor	xmm8, xmm8, xmm2
        vpshufb	xmm12, xmm12, xmm4
        vpshufb	xmm13, xmm13, xmm4
        vpshufb	xmm14, xmm14, xmm4
        vpshufb	xmm15, xmm15, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm5, OWORD PTR [rsp+16]
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm15, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm15, 17
        vpclmulqdq	xmm0, xmm7, xmm15, 0
        vpxor	xmm1, xmm1, xmm15
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm4, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm14, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm14, 17
        vpclmulqdq	xmm0, xmm5, xmm14, 0
        vpxor	xmm1, xmm1, xmm14
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm5, OWORD PTR [rsp+48]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm13, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm13, 17
        vpclmulqdq	xmm0, xmm7, xmm13, 0
        vpxor	xmm1, xmm1, xmm13
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm12, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm12, 17
        vpclmulqdq	xmm0, xmm5, xmm12, 0
        vpxor	xmm1, xmm1, xmm12
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm5, OWORD PTR [rsp+80]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm11, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm11, 17
        vpclmulqdq	xmm0, xmm7, xmm11, 0
        vpxor	xmm1, xmm1, xmm11
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm10, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm10, 17
        vpclmulqdq	xmm0, xmm5, xmm10, 0
        vpxor	xmm1, xmm1, xmm10
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm5, OWORD PTR [rsp+112]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm9, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm9, 17
        vpclmulqdq	xmm0, xmm7, xmm9, 0
        vpxor	xmm1, xmm1, xmm9
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm8, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm8, 17
        vpclmulqdq	xmm0, xmm5, xmm8, 0
        vpxor	xmm1, xmm1, xmm8
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm4, 31
        vpslld	xmm1, xmm4, 30
        vpslld	xmm2, xmm4, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm4, xmm4, xmm0
        vpsrld	xmm2, xmm4, 1
        vpsrld	xmm3, xmm4, 2
        vpsrld	xmm0, xmm4, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm4
        vpxor	xmm6, xmm6, xmm2
        vmovdqu	xmm5, OWORD PTR [rsp]
L_AES_GCM_encrypt_avx1_done_128:
        mov	edx, r9d
        cmp	ebx, edx
        jge	L_AES_GCM_encrypt_avx1_done_enc
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_avx1_last_block_done
        vmovdqu	xmm9, OWORD PTR [rsp+128]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [rsp+128], xmm9
        vpxor	xmm8, xmm8, [r15]
        vaesenc	xmm8, xmm8, [r15+16]
        vaesenc	xmm8, xmm8, [r15+32]
        vaesenc	xmm8, xmm8, [r15+48]
        vaesenc	xmm8, xmm8, [r15+64]
        vaesenc	xmm8, xmm8, [r15+80]
        vaesenc	xmm8, xmm8, [r15+96]
        vaesenc	xmm8, xmm8, [r15+112]
        vaesenc	xmm8, xmm8, [r15+128]
        vaesenc	xmm8, xmm8, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_aesenc_block_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_aesenc_block_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_aesenc_block_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqu	xmm9, OWORD PTR [rdi+rbx]
        vpxor	xmm8, xmm8, xmm9
        vmovdqu	OWORD PTR [rsi+rbx], xmm8
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        add	ebx, 16
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_avx1_last_block_ghash
L_AES_GCM_encrypt_avx1_last_block_start:
        vmovdqu	xmm13, OWORD PTR [rdi+rbx]
        vmovdqu	xmm9, OWORD PTR [rsp+128]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [rsp+128], xmm9
        vpxor	xmm8, xmm8, [r15]
        vpclmulqdq	xmm10, xmm6, xmm5, 16
        vaesenc	xmm8, xmm8, [r15+16]
        vaesenc	xmm8, xmm8, [r15+32]
        vpclmulqdq	xmm11, xmm6, xmm5, 1
        vaesenc	xmm8, xmm8, [r15+48]
        vaesenc	xmm8, xmm8, [r15+64]
        vpclmulqdq	xmm12, xmm6, xmm5, 0
        vaesenc	xmm8, xmm8, [r15+80]
        vpclmulqdq	xmm1, xmm6, xmm5, 17
        vaesenc	xmm8, xmm8, [r15+96]
        vpxor	xmm10, xmm10, xmm11
        vpslldq	xmm2, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vaesenc	xmm8, xmm8, [r15+112]
        vpxor	xmm2, xmm2, xmm12
        vpxor	xmm3, xmm1, xmm10
        vmovdqa	xmm0, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpclmulqdq	xmm11, xmm2, xmm0, 16
        vaesenc	xmm8, xmm8, [r15+128]
        vpshufd	xmm10, xmm2, 78
        vpxor	xmm10, xmm10, xmm11
        vpclmulqdq	xmm11, xmm10, xmm0, 16
        vaesenc	xmm8, xmm8, [r15+144]
        vpshufd	xmm10, xmm10, 78
        vpxor	xmm10, xmm10, xmm11
        vpxor	xmm6, xmm10, xmm3
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_aesenc_gfmul_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqa	xmm0, xmm13
        vpxor	xmm8, xmm8, xmm0
        vmovdqu	OWORD PTR [rsi+rbx], xmm8
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        add	ebx, 16
        vpxor	xmm6, xmm6, xmm8
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_avx1_last_block_start
L_AES_GCM_encrypt_avx1_last_block_ghash:
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
L_AES_GCM_encrypt_avx1_last_block_done:
        mov	ecx, r9d
        mov	edx, ecx
        and	ecx, 15
        jz	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_done
        vmovdqu	xmm4, OWORD PTR [rsp+128]
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpxor	xmm4, xmm4, [r15]
        vaesenc	xmm4, xmm4, [r15+16]
        vaesenc	xmm4, xmm4, [r15+32]
        vaesenc	xmm4, xmm4, [r15+48]
        vaesenc	xmm4, xmm4, [r15+64]
        vaesenc	xmm4, xmm4, [r15+80]
        vaesenc	xmm4, xmm4, [r15+96]
        vaesenc	xmm4, xmm4, [r15+112]
        vaesenc	xmm4, xmm4, [r15+128]
        vaesenc	xmm4, xmm4, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm9
        vaesenc	xmm4, xmm4, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm9
        vaesenc	xmm4, xmm4, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_aesenc_avx_last:
        vaesenclast	xmm4, xmm4, xmm9
        sub	rsp, 16
        xor	ecx, ecx
        vmovdqu	OWORD PTR [rsp], xmm4
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [rsi+rbx], r13b
        mov	BYTE PTR [rsp+rcx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_loop
        xor	r13, r13
        cmp	ecx, 16
        je	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_finish_enc
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_byte_loop:
        mov	BYTE PTR [rsp+rcx], r13b
        inc	ecx
        cmp	ecx, 16
        jl	L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_byte_loop
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_finish_enc:
        vmovdqu	xmm4, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm4
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
L_AES_GCM_encrypt_avx1_aesenc_last15_enc_avx_done:
L_AES_GCM_encrypt_avx1_done_enc:
        mov	edx, r9d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        vmovq	xmm0, rdx
        vmovq	xmm1, rcx
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
        vpshufb	xmm6, xmm6, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vmovdqu	xmm0, OWORD PTR [rsp+144]
        vpxor	xmm0, xmm0, xmm6
        cmp	r14d, 16
        je	L_AES_GCM_encrypt_avx1_store_tag_16
        xor	rcx, rcx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_avx1_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r8+rcx], r13b
        inc	ecx
        cmp	ecx, r14d
        jne	L_AES_GCM_encrypt_avx1_store_tag_loop
        jmp	L_AES_GCM_encrypt_avx1_store_tag_done
L_AES_GCM_encrypt_avx1_store_tag_16:
        vmovdqu	OWORD PTR [r8], xmm0
L_AES_GCM_encrypt_avx1_store_tag_done:
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+160]
        vmovdqu	xmm7, OWORD PTR [rsp+176]
        vmovdqu	xmm8, OWORD PTR [rsp+192]
        vmovdqu	xmm9, OWORD PTR [rsp+208]
        vmovdqu	xmm10, OWORD PTR [rsp+224]
        vmovdqu	xmm11, OWORD PTR [rsp+240]
        vmovdqu	xmm12, OWORD PTR [rsp+256]
        vmovdqu	xmm13, OWORD PTR [rsp+272]
        vmovdqu	xmm14, OWORD PTR [rsp+288]
        vmovdqu	xmm15, OWORD PTR [rsp+304]
        add	rsp, 320
        pop	r15
        pop	r14
        pop	rbx
        pop	r12
        pop	rsi
        pop	rdi
        pop	r13
        ret
AES_GCM_encrypt_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_avx1 PROC
        push	r13
        push	rdi
        push	rsi
        push	r12
        push	rbx
        push	r14
        push	r15
        push	rbp
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r12, r8
        mov	rax, r9
        mov	r8, QWORD PTR [rsp+104]
        mov	r9d, DWORD PTR [rsp+112]
        mov	r11d, DWORD PTR [rsp+120]
        mov	ebx, DWORD PTR [rsp+128]
        mov	r14d, DWORD PTR [rsp+136]
        mov	r15, QWORD PTR [rsp+144]
        mov	r10d, DWORD PTR [rsp+152]
        mov	rbp, QWORD PTR [rsp+160]
        sub	rsp, 328
        vmovdqu	OWORD PTR [rsp+168], xmm6
        vmovdqu	OWORD PTR [rsp+184], xmm7
        vmovdqu	OWORD PTR [rsp+200], xmm8
        vmovdqu	OWORD PTR [rsp+216], xmm9
        vmovdqu	OWORD PTR [rsp+232], xmm10
        vmovdqu	OWORD PTR [rsp+248], xmm11
        vmovdqu	OWORD PTR [rsp+264], xmm12
        vmovdqu	OWORD PTR [rsp+280], xmm13
        vmovdqu	OWORD PTR [rsp+296], xmm14
        vmovdqu	OWORD PTR [rsp+312], xmm15
        vpxor	xmm4, xmm4, xmm4
        vpxor	xmm6, xmm6, xmm6
        cmp	ebx, 12
        mov	edx, ebx
        jne	L_AES_GCM_decrypt_avx1_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        vmovq	xmm4, QWORD PTR [rax]
        vpinsrd	xmm4, xmm4, DWORD PTR [rax+8], 2
        vpinsrd	xmm4, xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	xmm5, OWORD PTR [r15]
        vpxor	xmm1, xmm4, xmm5
        vmovdqa	xmm7, OWORD PTR [r15+16]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+32]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+48]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+64]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+80]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+96]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+112]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+128]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+144]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        cmp	r10d, 11
        vmovdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+176]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        cmp	r10d, 13
        vmovdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+208]
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm1, xmm1, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm7
        vaesenclast	xmm1, xmm1, xmm7
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vmovdqu	OWORD PTR [rsp+144], xmm1
        jmp	L_AES_GCM_decrypt_avx1_iv_done
L_AES_GCM_decrypt_avx1_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqa	xmm5, OWORD PTR [r15]
        vaesenc	xmm5, xmm5, [r15+16]
        vaesenc	xmm5, xmm5, [r15+32]
        vaesenc	xmm5, xmm5, [r15+48]
        vaesenc	xmm5, xmm5, [r15+64]
        vaesenc	xmm5, xmm5, [r15+80]
        vaesenc	xmm5, xmm5, [r15+96]
        vaesenc	xmm5, xmm5, [r15+112]
        vaesenc	xmm5, xmm5, [r15+128]
        vaesenc	xmm5, xmm5, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm9
        vaesenc	xmm5, xmm5, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm9
        vaesenc	xmm5, xmm5, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm9
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_decrypt_avx1_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_avx1_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_avx1_calc_iv_16_loop:
        vmovdqu	xmm8, OWORD PTR [rax+rcx]
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx1_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_avx1_calc_iv_done
L_AES_GCM_decrypt_avx1_calc_iv_lt16:
        sub	rsp, 16
        vpxor	xmm8, xmm8, xmm8
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm8
L_AES_GCM_decrypt_avx1_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx1_calc_iv_loop
        vmovdqu	xmm8, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
L_AES_GCM_decrypt_avx1_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm4, xmm4, xmm2
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqa	xmm8, OWORD PTR [r15]
        vpxor	xmm8, xmm8, xmm4
        vaesenc	xmm8, xmm8, [r15+16]
        vaesenc	xmm8, xmm8, [r15+32]
        vaesenc	xmm8, xmm8, [r15+48]
        vaesenc	xmm8, xmm8, [r15+64]
        vaesenc	xmm8, xmm8, [r15+80]
        vaesenc	xmm8, xmm8, [r15+96]
        vaesenc	xmm8, xmm8, [r15+112]
        vaesenc	xmm8, xmm8, [r15+128]
        vaesenc	xmm8, xmm8, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqu	OWORD PTR [rsp+144], xmm8
L_AES_GCM_decrypt_avx1_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_decrypt_avx1_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_avx1_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_avx1_calc_aad_16_loop:
        vmovdqu	xmm8, OWORD PTR [r12+rcx]
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm6, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm6, xmm6, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx1_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_avx1_calc_aad_done
L_AES_GCM_decrypt_avx1_calc_aad_lt16:
        sub	rsp, 16
        vpxor	xmm8, xmm8, xmm8
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm8
L_AES_GCM_decrypt_avx1_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx1_calc_aad_loop
        vmovdqu	xmm8, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm6, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm7, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm7, xmm7, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm7, 31
        vpslld	xmm1, xmm7, 30
        vpslld	xmm2, xmm7, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm7, xmm7, xmm0
        vpsrld	xmm2, xmm7, 1
        vpsrld	xmm3, xmm7, 2
        vpsrld	xmm0, xmm7, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm6, xmm6, xmm2
L_AES_GCM_decrypt_avx1_calc_aad_done:
        ; Calculate counter and H
        vpsrlq	xmm9, xmm5, 63
        vpsllq	xmm8, xmm5, 1
        vpslldq	xmm9, xmm9, 8
        vpor	xmm8, xmm8, xmm9
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpaddd	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_one
        vpxor	xmm5, xmm5, xmm8
        vmovdqu	OWORD PTR [rsp+128], xmm4
        xor	ebx, ebx
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_decrypt_avx1_done_128
        and	r13d, 4294967168
        vmovdqa	xmm2, xmm6
        ; H ^ 1
        vmovdqu	OWORD PTR [rsp], xmm5
        ; H ^ 2
        vpclmulqdq	xmm8, xmm5, xmm5, 0
        vpclmulqdq	xmm0, xmm5, xmm5, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm0, xmm0, xmm14
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm0, 78
        vpclmulqdq	xmm11, xmm0, xmm5, 17
        vpclmulqdq	xmm8, xmm0, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm0
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm1, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm1, xmm1, xmm14
        vmovdqu	OWORD PTR [rsp+32], xmm1
        ; H ^ 4
        vpclmulqdq	xmm8, xmm0, xmm0, 0
        vpclmulqdq	xmm3, xmm0, xmm0, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm3, xmm3, xmm14
        vmovdqu	OWORD PTR [rsp+48], xmm3
        ; H ^ 5
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm0, 78
        vpshufd	xmm10, xmm1, 78
        vpclmulqdq	xmm11, xmm1, xmm0, 17
        vpclmulqdq	xmm8, xmm1, xmm0, 0
        vpxor	xmm9, xmm9, xmm0
        vpxor	xmm10, xmm10, xmm1
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+64], xmm7
        ; H ^ 6
        vpclmulqdq	xmm8, xmm1, xmm1, 0
        vpclmulqdq	xmm7, xmm1, xmm1, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+80], xmm7
        ; H ^ 7
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm1, 78
        vpshufd	xmm10, xmm3, 78
        vpclmulqdq	xmm11, xmm3, xmm1, 17
        vpclmulqdq	xmm8, xmm3, xmm1, 0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm3
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+96], xmm7
        ; H ^ 8
        vpclmulqdq	xmm8, xmm3, xmm3, 0
        vpclmulqdq	xmm7, xmm3, xmm3, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+112], xmm7
L_AES_GCM_decrypt_avx1_ghash_128:
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [rsi+rbx]
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [r15]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vmovdqu	xmm0, OWORD PTR [rcx]
        vaesenc	xmm8, xmm8, [r15+16]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm2
        vpshufd	xmm1, xmm7, 78
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm3, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+16]
        vaesenc	xmm10, xmm10, [r15+16]
        vpclmulqdq	xmm2, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+16]
        vaesenc	xmm12, xmm12, [r15+16]
        vpclmulqdq	xmm1, xmm1, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+16]
        vaesenc	xmm14, xmm14, [r15+16]
        vaesenc	xmm15, xmm15, [r15+16]
        vpxor	xmm1, xmm1, xmm2
        vpxor	xmm1, xmm1, xmm3
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm0, OWORD PTR [rcx+16]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+32]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+32]
        vaesenc	xmm10, xmm10, [r15+32]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+32]
        vaesenc	xmm12, xmm12, [r15+32]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+32]
        vaesenc	xmm14, xmm14, [r15+32]
        vaesenc	xmm15, xmm15, [r15+32]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+48]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+48]
        vaesenc	xmm10, xmm10, [r15+48]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+48]
        vaesenc	xmm12, xmm12, [r15+48]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+48]
        vaesenc	xmm14, xmm14, [r15+48]
        vaesenc	xmm15, xmm15, [r15+48]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm0, OWORD PTR [rcx+48]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+64]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+64]
        vaesenc	xmm10, xmm10, [r15+64]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+64]
        vaesenc	xmm12, xmm12, [r15+64]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+64]
        vaesenc	xmm14, xmm14, [r15+64]
        vaesenc	xmm15, xmm15, [r15+64]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+80]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+80]
        vaesenc	xmm10, xmm10, [r15+80]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+80]
        vaesenc	xmm12, xmm12, [r15+80]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+80]
        vaesenc	xmm14, xmm14, [r15+80]
        vaesenc	xmm15, xmm15, [r15+80]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm0, OWORD PTR [rcx+80]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+96]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+96]
        vaesenc	xmm10, xmm10, [r15+96]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+96]
        vaesenc	xmm12, xmm12, [r15+96]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+96]
        vaesenc	xmm14, xmm14, [r15+96]
        vaesenc	xmm15, xmm15, [r15+96]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+112]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+112]
        vaesenc	xmm10, xmm10, [r15+112]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+112]
        vaesenc	xmm12, xmm12, [r15+112]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+112]
        vaesenc	xmm14, xmm14, [r15+112]
        vaesenc	xmm15, xmm15, [r15+112]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm0, OWORD PTR [rcx+112]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [r15+128]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [r15+128]
        vaesenc	xmm10, xmm10, [r15+128]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [r15+128]
        vaesenc	xmm12, xmm12, [r15+128]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [r15+128]
        vaesenc	xmm14, xmm14, [r15+128]
        vaesenc	xmm15, xmm15, [r15+128]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vpslldq	xmm5, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vaesenc	xmm8, xmm8, [r15+144]
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm3, xmm3, xmm1
        vaesenc	xmm9, xmm9, [r15+144]
        vpslld	xmm7, xmm2, 31
        vpslld	xmm4, xmm2, 30
        vpslld	xmm5, xmm2, 25
        vaesenc	xmm10, xmm10, [r15+144]
        vpxor	xmm7, xmm7, xmm4
        vpxor	xmm7, xmm7, xmm5
        vaesenc	xmm11, xmm11, [r15+144]
        vpsrldq	xmm4, xmm7, 4
        vpslldq	xmm7, xmm7, 12
        vaesenc	xmm12, xmm12, [r15+144]
        vpxor	xmm2, xmm2, xmm7
        vpsrld	xmm5, xmm2, 1
        vaesenc	xmm13, xmm13, [r15+144]
        vpsrld	xmm1, xmm2, 2
        vpsrld	xmm0, xmm2, 7
        vaesenc	xmm14, xmm14, [r15+144]
        vpxor	xmm5, xmm5, xmm1
        vpxor	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, [r15+144]
        vpxor	xmm5, xmm5, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm2, xmm2, xmm3
        cmp	r10d, 11
        vmovdqa	xmm7, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r10d, 13
        vmovdqa	xmm7, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_aesenc_128_ghash_avx_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_avx1_ghash_128
        vmovdqa	xmm6, xmm2
        vmovdqu	xmm5, OWORD PTR [rsp]
L_AES_GCM_decrypt_avx1_done_128:
        mov	edx, r9d
        cmp	ebx, edx
        jge	L_AES_GCM_decrypt_avx1_done_dec
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_decrypt_avx1_last_block_done
L_AES_GCM_decrypt_avx1_last_block_start:
        vmovdqu	xmm13, OWORD PTR [rdi+rbx]
        vmovdqa	xmm0, xmm5
        vpshufb	xmm1, xmm13, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm1, xmm1, xmm6
        vmovdqu	xmm9, OWORD PTR [rsp+128]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [rsp+128], xmm9
        vpxor	xmm8, xmm8, [r15]
        vpclmulqdq	xmm10, xmm1, xmm0, 16
        vaesenc	xmm8, xmm8, [r15+16]
        vaesenc	xmm8, xmm8, [r15+32]
        vpclmulqdq	xmm11, xmm1, xmm0, 1
        vaesenc	xmm8, xmm8, [r15+48]
        vaesenc	xmm8, xmm8, [r15+64]
        vpclmulqdq	xmm12, xmm1, xmm0, 0
        vaesenc	xmm8, xmm8, [r15+80]
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vaesenc	xmm8, xmm8, [r15+96]
        vpxor	xmm10, xmm10, xmm11
        vpslldq	xmm2, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vaesenc	xmm8, xmm8, [r15+112]
        vpxor	xmm2, xmm2, xmm12
        vpxor	xmm3, xmm1, xmm10
        vmovdqa	xmm0, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpclmulqdq	xmm11, xmm2, xmm0, 16
        vaesenc	xmm8, xmm8, [r15+128]
        vpshufd	xmm10, xmm2, 78
        vpxor	xmm10, xmm10, xmm11
        vpclmulqdq	xmm11, xmm10, xmm0, 16
        vaesenc	xmm8, xmm8, [r15+144]
        vpshufd	xmm10, xmm10, 78
        vpxor	xmm10, xmm10, xmm11
        vpxor	xmm6, xmm10, xmm3
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_aesenc_gfmul_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqa	xmm0, xmm13
        vpxor	xmm8, xmm8, xmm0
        vmovdqu	OWORD PTR [rsi+rbx], xmm8
        add	ebx, 16
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_avx1_last_block_start
L_AES_GCM_decrypt_avx1_last_block_done:
        mov	ecx, r9d
        mov	edx, ecx
        and	ecx, 15
        jz	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_done
        vmovdqu	xmm4, OWORD PTR [rsp+128]
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpxor	xmm4, xmm4, [r15]
        vaesenc	xmm4, xmm4, [r15+16]
        vaesenc	xmm4, xmm4, [r15+32]
        vaesenc	xmm4, xmm4, [r15+48]
        vaesenc	xmm4, xmm4, [r15+64]
        vaesenc	xmm4, xmm4, [r15+80]
        vaesenc	xmm4, xmm4, [r15+96]
        vaesenc	xmm4, xmm4, [r15+112]
        vaesenc	xmm4, xmm4, [r15+128]
        vaesenc	xmm4, xmm4, [r15+144]
        cmp	r10d, 11
        vmovdqa	xmm9, OWORD PTR [r15+160]
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm9
        vaesenc	xmm4, xmm4, [r15+176]
        cmp	r10d, 13
        vmovdqa	xmm9, OWORD PTR [r15+192]
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm9
        vaesenc	xmm4, xmm4, [r15+208]
        vmovdqa	xmm9, OWORD PTR [r15+224]
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_aesenc_avx_last:
        vaesenclast	xmm4, xmm4, xmm9
        sub	rsp, 32
        xor	ecx, ecx
        vmovdqu	OWORD PTR [rsp], xmm4
        vpxor	xmm0, xmm0, xmm0
        vmovdqu	OWORD PTR [rsp+16], xmm0
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        mov	BYTE PTR [rsp+rcx+16], r13b
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [rsi+rbx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_loop
        vmovdqu	xmm4, OWORD PTR [rsp+16]
        add	rsp, 32
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm4
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
L_AES_GCM_decrypt_avx1_aesenc_last15_dec_avx_done:
L_AES_GCM_decrypt_avx1_done_dec:
        mov	edx, r9d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        vmovq	xmm0, rdx
        vmovq	xmm1, rcx
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
        vpshufb	xmm6, xmm6, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vmovdqu	xmm0, OWORD PTR [rsp+144]
        vpxor	xmm0, xmm0, xmm6
        cmp	r14d, 16
        je	L_AES_GCM_decrypt_avx1_cmp_tag_16
        sub	rsp, 16
        xor	rcx, rcx
        xor	rbx, rbx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_avx1_cmp_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        xor	r13b, BYTE PTR [r8+rcx]
        or	bl, r13b
        inc	ecx
        cmp	ecx, r14d
        jne	L_AES_GCM_decrypt_avx1_cmp_tag_loop
        cmp	rbx, 0
        sete	bl
        add	rsp, 16
        xor	rcx, rcx
        jmp	L_AES_GCM_decrypt_avx1_cmp_tag_done
L_AES_GCM_decrypt_avx1_cmp_tag_16:
        vmovdqu	xmm1, OWORD PTR [r8]
        vpcmpeqb	xmm0, xmm0, xmm1
        vpmovmskb	rdx, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	ebx, ebx
        cmp	edx, 65535
        sete	bl
L_AES_GCM_decrypt_avx1_cmp_tag_done:
        mov	DWORD PTR [rbp], ebx
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+168]
        vmovdqu	xmm7, OWORD PTR [rsp+184]
        vmovdqu	xmm8, OWORD PTR [rsp+200]
        vmovdqu	xmm9, OWORD PTR [rsp+216]
        vmovdqu	xmm10, OWORD PTR [rsp+232]
        vmovdqu	xmm11, OWORD PTR [rsp+248]
        vmovdqu	xmm12, OWORD PTR [rsp+264]
        vmovdqu	xmm13, OWORD PTR [rsp+280]
        vmovdqu	xmm14, OWORD PTR [rsp+296]
        vmovdqu	xmm15, OWORD PTR [rsp+312]
        add	rsp, 328
        pop	rbp
        pop	r15
        pop	r14
        pop	rbx
        pop	r12
        pop	rsi
        pop	rdi
        pop	r13
        ret
AES_GCM_decrypt_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_init_avx1 PROC
        push	rdi
        push	rsi
        push	r12
        push	r13
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r10, r8
        mov	r11d, r9d
        mov	rax, QWORD PTR [rsp+72]
        mov	r8, QWORD PTR [rsp+80]
        mov	r9, QWORD PTR [rsp+88]
        sub	rsp, 80
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vmovdqu	OWORD PTR [rsp+48], xmm8
        vmovdqu	OWORD PTR [rsp+64], xmm15
        vpxor	xmm4, xmm4, xmm4
        mov	edx, r11d
        cmp	edx, 12
        jne	L_AES_GCM_init_avx1_iv_not_12
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        mov	ecx, 16777216
        vmovq	xmm4, QWORD PTR [r10]
        vpinsrd	xmm4, xmm4, DWORD PTR [r10+8], 2
        vpinsrd	xmm4, xmm4, ecx, 3
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqa	xmm5, OWORD PTR [rdi]
        vpxor	xmm1, xmm4, xmm5
        vmovdqa	xmm6, OWORD PTR [rdi+16]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+32]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+48]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+64]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+80]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+96]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+112]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+128]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+144]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        cmp	esi, 11
        vmovdqa	xmm6, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+176]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        cmp	esi, 13
        vmovdqa	xmm6, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx1_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+208]
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm1, xmm1, xmm6
        vmovdqa	xmm6, OWORD PTR [rdi+224]
L_AES_GCM_init_avx1_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm6
        vaesenclast	xmm1, xmm1, xmm6
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vmovdqu	xmm15, xmm1
        jmp	L_AES_GCM_init_avx1_iv_done
L_AES_GCM_init_avx1_iv_not_12:
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqa	xmm5, OWORD PTR [rdi]
        vaesenc	xmm5, xmm5, [rdi+16]
        vaesenc	xmm5, xmm5, [rdi+32]
        vaesenc	xmm5, xmm5, [rdi+48]
        vaesenc	xmm5, xmm5, [rdi+64]
        vaesenc	xmm5, xmm5, [rdi+80]
        vaesenc	xmm5, xmm5, [rdi+96]
        vaesenc	xmm5, xmm5, [rdi+112]
        vaesenc	xmm5, xmm5, [rdi+128]
        vaesenc	xmm5, xmm5, [rdi+144]
        cmp	esi, 11
        vmovdqa	xmm8, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm8
        vaesenc	xmm5, xmm5, [rdi+176]
        cmp	esi, 13
        vmovdqa	xmm8, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm8
        vaesenc	xmm5, xmm5, [rdi+208]
        vmovdqa	xmm8, OWORD PTR [rdi+224]
L_AES_GCM_init_avx1_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm8
        vpshufb	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_init_avx1_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_init_avx1_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_init_avx1_calc_iv_16_loop:
        vmovdqu	xmm7, OWORD PTR [r10+rcx]
        vpshufb	xmm7, xmm7, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm7
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm6, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm6, 31
        vpslld	xmm1, xmm6, 30
        vpslld	xmm2, xmm6, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm6, xmm6, xmm0
        vpsrld	xmm2, xmm6, 1
        vpsrld	xmm3, xmm6, 2
        vpsrld	xmm0, xmm6, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm6
        vpxor	xmm4, xmm4, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_init_avx1_calc_iv_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_init_avx1_calc_iv_done
L_AES_GCM_init_avx1_calc_iv_lt16:
        sub	rsp, 16
        vpxor	xmm7, xmm7, xmm7
        xor	r13d, r13d
        vmovdqu	OWORD PTR [rsp], xmm7
L_AES_GCM_init_avx1_calc_iv_loop:
        movzx	r12d, BYTE PTR [r10+rcx]
        mov	BYTE PTR [rsp+r13], r12b
        inc	ecx
        inc	r13d
        cmp	ecx, edx
        jl	L_AES_GCM_init_avx1_calc_iv_loop
        vmovdqu	xmm7, OWORD PTR [rsp]
        add	rsp, 16
        vpshufb	xmm7, xmm7, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm7
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm6, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm6, 31
        vpslld	xmm1, xmm6, 30
        vpslld	xmm2, xmm6, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm6, xmm6, xmm0
        vpsrld	xmm2, xmm6, 1
        vpsrld	xmm3, xmm6, 2
        vpsrld	xmm0, xmm6, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm6
        vpxor	xmm4, xmm4, xmm2
L_AES_GCM_init_avx1_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm6, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm6, 31
        vpslld	xmm1, xmm6, 30
        vpslld	xmm2, xmm6, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm6, xmm6, xmm0
        vpsrld	xmm2, xmm6, 1
        vpsrld	xmm3, xmm6, 2
        vpsrld	xmm0, xmm6, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm6
        vpxor	xmm4, xmm4, xmm2
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqa	xmm7, OWORD PTR [rdi]
        vpxor	xmm7, xmm7, xmm4
        vaesenc	xmm7, xmm7, [rdi+16]
        vaesenc	xmm7, xmm7, [rdi+32]
        vaesenc	xmm7, xmm7, [rdi+48]
        vaesenc	xmm7, xmm7, [rdi+64]
        vaesenc	xmm7, xmm7, [rdi+80]
        vaesenc	xmm7, xmm7, [rdi+96]
        vaesenc	xmm7, xmm7, [rdi+112]
        vaesenc	xmm7, xmm7, [rdi+128]
        vaesenc	xmm7, xmm7, [rdi+144]
        cmp	esi, 11
        vmovdqa	xmm8, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm7, xmm7, xmm8
        vaesenc	xmm7, xmm7, [rdi+176]
        cmp	esi, 13
        vmovdqa	xmm8, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last
        vaesenc	xmm7, xmm7, xmm8
        vaesenc	xmm7, xmm7, [rdi+208]
        vmovdqa	xmm8, OWORD PTR [rdi+224]
L_AES_GCM_init_avx1_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm7, xmm7, xmm8
        vmovdqu	xmm15, xmm7
L_AES_GCM_init_avx1_iv_done:
        vmovdqa	OWORD PTR [r9], xmm15
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_one
        vmovdqa	OWORD PTR [rax], xmm5
        vmovdqa	OWORD PTR [r8], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm8, OWORD PTR [rsp+48]
        vmovdqu	xmm15, OWORD PTR [rsp+64]
        add	rsp, 80
        pop	r13
        pop	r12
        pop	rsi
        pop	rdi
        ret
AES_GCM_init_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_aad_update_avx1 PROC
        mov	rax, rcx
        sub	rsp, 32
        vmovdqu	OWORD PTR [rsp], xmm6
        vmovdqu	OWORD PTR [rsp+16], xmm7
        vmovdqa	xmm5, OWORD PTR [r8]
        vmovdqa	xmm6, OWORD PTR [r9]
        xor	ecx, ecx
L_AES_GCM_aad_update_avx1_16_loop:
        vmovdqu	xmm7, OWORD PTR [rax+rcx]
        vpshufb	xmm7, xmm7, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm7
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm5, 78
        vpshufd	xmm2, xmm6, 78
        vpclmulqdq	xmm3, xmm6, xmm5, 17
        vpclmulqdq	xmm0, xmm6, xmm5, 0
        vpxor	xmm1, xmm1, xmm5
        vpxor	xmm2, xmm2, xmm6
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm4, xmm0
        vmovdqa	xmm5, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm5, xmm5, xmm1
        vpsrld	xmm0, xmm4, 31
        vpsrld	xmm1, xmm5, 31
        vpslld	xmm4, xmm4, 1
        vpslld	xmm5, xmm5, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm5, xmm5, xmm2
        vpor	xmm4, xmm4, xmm0
        vpor	xmm5, xmm5, xmm1
        vpslld	xmm0, xmm4, 31
        vpslld	xmm1, xmm4, 30
        vpslld	xmm2, xmm4, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm4, xmm4, xmm0
        vpsrld	xmm2, xmm4, 1
        vpsrld	xmm3, xmm4, 2
        vpsrld	xmm0, xmm4, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm4
        vpxor	xmm5, xmm5, xmm2
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_aad_update_avx1_16_loop
        vmovdqa	OWORD PTR [r8], xmm5
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp]
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        add	rsp, 32
        ret
AES_GCM_aad_update_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_block_avx1 PROC
        mov	r10, r8
        mov	r11, r9
        mov	rax, QWORD PTR [rsp+40]
        vmovdqu	xmm1, OWORD PTR [rax]
        vpshufb	xmm0, xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm1, xmm1, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [rax], xmm1
        vpxor	xmm0, xmm0, [rcx]
        vaesenc	xmm0, xmm0, [rcx+16]
        vaesenc	xmm0, xmm0, [rcx+32]
        vaesenc	xmm0, xmm0, [rcx+48]
        vaesenc	xmm0, xmm0, [rcx+64]
        vaesenc	xmm0, xmm0, [rcx+80]
        vaesenc	xmm0, xmm0, [rcx+96]
        vaesenc	xmm0, xmm0, [rcx+112]
        vaesenc	xmm0, xmm0, [rcx+128]
        vaesenc	xmm0, xmm0, [rcx+144]
        cmp	edx, 11
        vmovdqa	xmm1, OWORD PTR [rcx+160]
        jl	L_AES_GCM_encrypt_block_avx1_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vaesenc	xmm0, xmm0, [rcx+176]
        cmp	edx, 13
        vmovdqa	xmm1, OWORD PTR [rcx+192]
        jl	L_AES_GCM_encrypt_block_avx1_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vaesenc	xmm0, xmm0, [rcx+208]
        vmovdqa	xmm1, OWORD PTR [rcx+224]
L_AES_GCM_encrypt_block_avx1_aesenc_block_last:
        vaesenclast	xmm0, xmm0, xmm1
        vmovdqu	xmm1, OWORD PTR [r11]
        vpxor	xmm0, xmm0, xmm1
        vmovdqu	OWORD PTR [r10], xmm0
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vzeroupper
        ret
AES_GCM_encrypt_block_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_ghash_block_avx1 PROC
        sub	rsp, 32
        vmovdqu	OWORD PTR [rsp], xmm6
        vmovdqu	OWORD PTR [rsp+16], xmm7
        vmovdqa	xmm4, OWORD PTR [rdx]
        vmovdqa	xmm5, OWORD PTR [r8]
        vmovdqu	xmm7, OWORD PTR [rcx]
        vpshufb	xmm7, xmm7, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm7
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm4, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpxor	xmm1, xmm1, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm6, xmm0
        vmovdqa	xmm4, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm4, xmm4, xmm1
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        vpslld	xmm0, xmm6, 31
        vpslld	xmm1, xmm6, 30
        vpslld	xmm2, xmm6, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm6, xmm6, xmm0
        vpsrld	xmm2, xmm6, 1
        vpsrld	xmm3, xmm6, 2
        vpsrld	xmm0, xmm6, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm6
        vpxor	xmm4, xmm4, xmm2
        vmovdqa	OWORD PTR [rdx], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp]
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        add	rsp, 32
        ret
AES_GCM_ghash_block_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_update_avx1 PROC
        push	r13
        push	r12
        push	r14
        push	r15
        push	rdi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	r15, QWORD PTR [rsp+104]
        sub	rsp, 320
        vmovdqu	OWORD PTR [rsp+160], xmm6
        vmovdqu	OWORD PTR [rsp+176], xmm7
        vmovdqu	OWORD PTR [rsp+192], xmm8
        vmovdqu	OWORD PTR [rsp+208], xmm9
        vmovdqu	OWORD PTR [rsp+224], xmm10
        vmovdqu	OWORD PTR [rsp+240], xmm11
        vmovdqu	OWORD PTR [rsp+256], xmm12
        vmovdqu	OWORD PTR [rsp+272], xmm13
        vmovdqu	OWORD PTR [rsp+288], xmm14
        vmovdqu	OWORD PTR [rsp+304], xmm15
        vmovdqa	xmm6, OWORD PTR [r12]
        vmovdqa	xmm5, OWORD PTR [r14]
        vpsrlq	xmm9, xmm5, 63
        vpsllq	xmm8, xmm5, 1
        vpslldq	xmm9, xmm9, 8
        vpor	xmm8, xmm8, xmm9
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm8
        xor	edi, edi
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_encrypt_update_avx1_done_128
        and	r13d, 4294967168
        vmovdqa	xmm2, xmm6
        ; H ^ 1
        vmovdqu	OWORD PTR [rsp], xmm5
        ; H ^ 2
        vpclmulqdq	xmm8, xmm5, xmm5, 0
        vpclmulqdq	xmm0, xmm5, xmm5, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm0, xmm0, xmm14
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm0, 78
        vpclmulqdq	xmm11, xmm0, xmm5, 17
        vpclmulqdq	xmm8, xmm0, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm0
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm1, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm1, xmm1, xmm14
        vmovdqu	OWORD PTR [rsp+32], xmm1
        ; H ^ 4
        vpclmulqdq	xmm8, xmm0, xmm0, 0
        vpclmulqdq	xmm3, xmm0, xmm0, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm3, xmm3, xmm14
        vmovdqu	OWORD PTR [rsp+48], xmm3
        ; H ^ 5
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm0, 78
        vpshufd	xmm10, xmm1, 78
        vpclmulqdq	xmm11, xmm1, xmm0, 17
        vpclmulqdq	xmm8, xmm1, xmm0, 0
        vpxor	xmm9, xmm9, xmm0
        vpxor	xmm10, xmm10, xmm1
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+64], xmm7
        ; H ^ 6
        vpclmulqdq	xmm8, xmm1, xmm1, 0
        vpclmulqdq	xmm7, xmm1, xmm1, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+80], xmm7
        ; H ^ 7
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm1, 78
        vpshufd	xmm10, xmm3, 78
        vpclmulqdq	xmm11, xmm3, xmm1, 17
        vpclmulqdq	xmm8, xmm3, xmm1, 0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm3
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+96], xmm7
        ; H ^ 8
        vpclmulqdq	xmm8, xmm3, xmm3, 0
        vpclmulqdq	xmm7, xmm3, xmm3, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+112], xmm7
        ; First 128 bytes of input
        vmovdqu	xmm0, OWORD PTR [r15]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [r15], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+16]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+32]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+48]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+64]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+80]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+96]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+112]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+128]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+144]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 11
        vmovdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx1_aesenc_128_enc_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [r11]
        vmovdqu	xmm1, OWORD PTR [r11+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [r10], xmm8
        vmovdqu	OWORD PTR [r10+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [r11+32]
        vmovdqu	xmm1, OWORD PTR [r11+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [r10+32], xmm10
        vmovdqu	OWORD PTR [r10+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [r11+64]
        vmovdqu	xmm1, OWORD PTR [r11+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [r10+64], xmm12
        vmovdqu	OWORD PTR [r10+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [r11+96]
        vmovdqu	xmm1, OWORD PTR [r11+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [r10+96], xmm14
        vmovdqu	OWORD PTR [r10+112], xmm15
        cmp	r13d, 128
        mov	edi, 128
        jle	L_AES_GCM_encrypt_update_avx1_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_update_avx1_ghash_128:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        vmovdqu	xmm0, OWORD PTR [r15]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [r15], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vmovdqu	xmm0, OWORD PTR [rdx+-128]
        vaesenc	xmm8, xmm8, [rax+16]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm2
        vpshufd	xmm1, xmm7, 78
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm3, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+16]
        vaesenc	xmm10, xmm10, [rax+16]
        vpclmulqdq	xmm2, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+16]
        vaesenc	xmm12, xmm12, [rax+16]
        vpclmulqdq	xmm1, xmm1, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+16]
        vaesenc	xmm14, xmm14, [rax+16]
        vaesenc	xmm15, xmm15, [rax+16]
        vpxor	xmm1, xmm1, xmm2
        vpxor	xmm1, xmm1, xmm3
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm0, OWORD PTR [rdx+-112]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+32]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+32]
        vaesenc	xmm10, xmm10, [rax+32]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+32]
        vaesenc	xmm12, xmm12, [rax+32]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+32]
        vaesenc	xmm14, xmm14, [rax+32]
        vaesenc	xmm15, xmm15, [rax+32]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vmovdqu	xmm0, OWORD PTR [rdx+-96]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+48]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+48]
        vaesenc	xmm10, xmm10, [rax+48]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+48]
        vaesenc	xmm12, xmm12, [rax+48]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+48]
        vaesenc	xmm14, xmm14, [rax+48]
        vaesenc	xmm15, xmm15, [rax+48]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm0, OWORD PTR [rdx+-80]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+64]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+64]
        vaesenc	xmm10, xmm10, [rax+64]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+64]
        vaesenc	xmm12, xmm12, [rax+64]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+64]
        vaesenc	xmm14, xmm14, [rax+64]
        vaesenc	xmm15, xmm15, [rax+64]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vmovdqu	xmm0, OWORD PTR [rdx+-64]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+80]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+80]
        vaesenc	xmm10, xmm10, [rax+80]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+80]
        vaesenc	xmm12, xmm12, [rax+80]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+80]
        vaesenc	xmm14, xmm14, [rax+80]
        vaesenc	xmm15, xmm15, [rax+80]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm0, OWORD PTR [rdx+-48]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+96]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+96]
        vaesenc	xmm10, xmm10, [rax+96]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+96]
        vaesenc	xmm12, xmm12, [rax+96]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+96]
        vaesenc	xmm14, xmm14, [rax+96]
        vaesenc	xmm15, xmm15, [rax+96]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vmovdqu	xmm0, OWORD PTR [rdx+-32]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+112]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+112]
        vaesenc	xmm10, xmm10, [rax+112]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+112]
        vaesenc	xmm12, xmm12, [rax+112]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+112]
        vaesenc	xmm14, xmm14, [rax+112]
        vaesenc	xmm15, xmm15, [rax+112]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm0, OWORD PTR [rdx+-16]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+128]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+128]
        vaesenc	xmm10, xmm10, [rax+128]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+128]
        vaesenc	xmm12, xmm12, [rax+128]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+128]
        vaesenc	xmm14, xmm14, [rax+128]
        vaesenc	xmm15, xmm15, [rax+128]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vpslldq	xmm5, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vaesenc	xmm8, xmm8, [rax+144]
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm3, xmm3, xmm1
        vaesenc	xmm9, xmm9, [rax+144]
        vpslld	xmm7, xmm2, 31
        vpslld	xmm4, xmm2, 30
        vpslld	xmm5, xmm2, 25
        vaesenc	xmm10, xmm10, [rax+144]
        vpxor	xmm7, xmm7, xmm4
        vpxor	xmm7, xmm7, xmm5
        vaesenc	xmm11, xmm11, [rax+144]
        vpsrldq	xmm4, xmm7, 4
        vpslldq	xmm7, xmm7, 12
        vaesenc	xmm12, xmm12, [rax+144]
        vpxor	xmm2, xmm2, xmm7
        vpsrld	xmm5, xmm2, 1
        vaesenc	xmm13, xmm13, [rax+144]
        vpsrld	xmm1, xmm2, 2
        vpsrld	xmm0, xmm2, 7
        vaesenc	xmm14, xmm14, [rax+144]
        vpxor	xmm5, xmm5, xmm1
        vpxor	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, [rax+144]
        vpxor	xmm5, xmm5, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm2, xmm2, xmm3
        cmp	r8d, 11
        vmovdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx1_aesenc_128_ghash_avx_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        add	edi, 128
        cmp	edi, r13d
        jl	L_AES_GCM_encrypt_update_avx1_ghash_128
L_AES_GCM_encrypt_update_avx1_end_128:
        vmovdqa	xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpshufb	xmm8, xmm8, xmm4
        vpshufb	xmm9, xmm9, xmm4
        vpshufb	xmm10, xmm10, xmm4
        vpshufb	xmm11, xmm11, xmm4
        vpxor	xmm8, xmm8, xmm2
        vpshufb	xmm12, xmm12, xmm4
        vpshufb	xmm13, xmm13, xmm4
        vpshufb	xmm14, xmm14, xmm4
        vpshufb	xmm15, xmm15, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm5, OWORD PTR [rsp+16]
        ; ghash_gfmul_avx
        vpshufd	xmm1, xmm15, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm15, 17
        vpclmulqdq	xmm0, xmm7, xmm15, 0
        vpxor	xmm1, xmm1, xmm15
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vmovdqa	xmm4, xmm0
        vmovdqa	xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm14, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm14, 17
        vpclmulqdq	xmm0, xmm5, xmm14, 0
        vpxor	xmm1, xmm1, xmm14
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm5, OWORD PTR [rsp+48]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm13, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm13, 17
        vpclmulqdq	xmm0, xmm7, xmm13, 0
        vpxor	xmm1, xmm1, xmm13
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm12, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm12, 17
        vpclmulqdq	xmm0, xmm5, xmm12, 0
        vpxor	xmm1, xmm1, xmm12
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm5, OWORD PTR [rsp+80]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm11, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm11, 17
        vpclmulqdq	xmm0, xmm7, xmm11, 0
        vpxor	xmm1, xmm1, xmm11
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm10, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm10, 17
        vpclmulqdq	xmm0, xmm5, xmm10, 0
        vpxor	xmm1, xmm1, xmm10
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm5, OWORD PTR [rsp+112]
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm9, 78
        vpshufd	xmm2, xmm7, 78
        vpclmulqdq	xmm3, xmm7, xmm9, 17
        vpclmulqdq	xmm0, xmm7, xmm9, 0
        vpxor	xmm1, xmm1, xmm9
        vpxor	xmm2, xmm2, xmm7
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        ; ghash_gfmul_xor_avx
        vpshufd	xmm1, xmm8, 78
        vpshufd	xmm2, xmm5, 78
        vpclmulqdq	xmm3, xmm5, xmm8, 17
        vpclmulqdq	xmm0, xmm5, xmm8, 0
        vpxor	xmm1, xmm1, xmm8
        vpxor	xmm2, xmm2, xmm5
        vpclmulqdq	xmm1, xmm1, xmm2, 0
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm1, xmm1, xmm3
        vpxor	xmm4, xmm4, xmm0
        vpxor	xmm6, xmm6, xmm3
        vpslldq	xmm2, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vpxor	xmm4, xmm4, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpslld	xmm0, xmm4, 31
        vpslld	xmm1, xmm4, 30
        vpslld	xmm2, xmm4, 25
        vpxor	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm2
        vmovdqa	xmm1, xmm0
        vpsrldq	xmm1, xmm1, 4
        vpslldq	xmm0, xmm0, 12
        vpxor	xmm4, xmm4, xmm0
        vpsrld	xmm2, xmm4, 1
        vpsrld	xmm3, xmm4, 2
        vpsrld	xmm0, xmm4, 7
        vpxor	xmm2, xmm2, xmm3
        vpxor	xmm2, xmm2, xmm0
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm2, xmm2, xmm4
        vpxor	xmm6, xmm6, xmm2
        vmovdqu	xmm5, OWORD PTR [rsp]
L_AES_GCM_encrypt_update_avx1_done_128:
        mov	edx, r9d
        cmp	edi, edx
        jge	L_AES_GCM_encrypt_update_avx1_done_enc
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	edi, r13d
        jge	L_AES_GCM_encrypt_update_avx1_last_block_done
        vmovdqu	xmm9, OWORD PTR [r15]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [r15], xmm9
        vpxor	xmm8, xmm8, [rax]
        vaesenc	xmm8, xmm8, [rax+16]
        vaesenc	xmm8, xmm8, [rax+32]
        vaesenc	xmm8, xmm8, [rax+48]
        vaesenc	xmm8, xmm8, [rax+64]
        vaesenc	xmm8, xmm8, [rax+80]
        vaesenc	xmm8, xmm8, [rax+96]
        vaesenc	xmm8, xmm8, [rax+112]
        vaesenc	xmm8, xmm8, [rax+128]
        vaesenc	xmm8, xmm8, [rax+144]
        cmp	r8d, 11
        vmovdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_block_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+176]
        cmp	r8d, 13
        vmovdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_block_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+208]
        vmovdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx1_aesenc_block_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqu	xmm9, OWORD PTR [r11+rdi]
        vpxor	xmm8, xmm8, xmm9
        vmovdqu	OWORD PTR [r10+rdi], xmm8
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm8
        add	edi, 16
        cmp	edi, r13d
        jge	L_AES_GCM_encrypt_update_avx1_last_block_ghash
L_AES_GCM_encrypt_update_avx1_last_block_start:
        vmovdqu	xmm13, OWORD PTR [r11+rdi]
        vmovdqu	xmm9, OWORD PTR [r15]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [r15], xmm9
        vpxor	xmm8, xmm8, [rax]
        vpclmulqdq	xmm10, xmm6, xmm5, 16
        vaesenc	xmm8, xmm8, [rax+16]
        vaesenc	xmm8, xmm8, [rax+32]
        vpclmulqdq	xmm11, xmm6, xmm5, 1
        vaesenc	xmm8, xmm8, [rax+48]
        vaesenc	xmm8, xmm8, [rax+64]
        vpclmulqdq	xmm12, xmm6, xmm5, 0
        vaesenc	xmm8, xmm8, [rax+80]
        vpclmulqdq	xmm1, xmm6, xmm5, 17
        vaesenc	xmm8, xmm8, [rax+96]
        vpxor	xmm10, xmm10, xmm11
        vpslldq	xmm2, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vaesenc	xmm8, xmm8, [rax+112]
        vpxor	xmm2, xmm2, xmm12
        vpxor	xmm3, xmm1, xmm10
        vmovdqa	xmm0, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpclmulqdq	xmm11, xmm2, xmm0, 16
        vaesenc	xmm8, xmm8, [rax+128]
        vpshufd	xmm10, xmm2, 78
        vpxor	xmm10, xmm10, xmm11
        vpclmulqdq	xmm11, xmm10, xmm0, 16
        vaesenc	xmm8, xmm8, [rax+144]
        vpshufd	xmm10, xmm10, 78
        vpxor	xmm10, xmm10, xmm11
        vpxor	xmm6, xmm10, xmm3
        cmp	r8d, 11
        vmovdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+176]
        cmp	r8d, 13
        vmovdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+208]
        vmovdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx1_aesenc_gfmul_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqa	xmm0, xmm13
        vpxor	xmm8, xmm8, xmm0
        vmovdqu	OWORD PTR [r10+rdi], xmm8
        vpshufb	xmm8, xmm8, OWORD PTR L_avx1_aes_gcm_bswap_mask
        add	edi, 16
        vpxor	xmm6, xmm6, xmm8
        cmp	edi, r13d
        jl	L_AES_GCM_encrypt_update_avx1_last_block_start
L_AES_GCM_encrypt_update_avx1_last_block_ghash:
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm6, 78
        vpclmulqdq	xmm11, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm6
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm6, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm6, xmm6, xmm14
L_AES_GCM_encrypt_update_avx1_last_block_done:
L_AES_GCM_encrypt_update_avx1_done_enc:
        vmovdqa	OWORD PTR [r12], xmm6
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+160]
        vmovdqu	xmm7, OWORD PTR [rsp+176]
        vmovdqu	xmm8, OWORD PTR [rsp+192]
        vmovdqu	xmm9, OWORD PTR [rsp+208]
        vmovdqu	xmm10, OWORD PTR [rsp+224]
        vmovdqu	xmm11, OWORD PTR [rsp+240]
        vmovdqu	xmm12, OWORD PTR [rsp+256]
        vmovdqu	xmm13, OWORD PTR [rsp+272]
        vmovdqu	xmm14, OWORD PTR [rsp+288]
        vmovdqu	xmm15, OWORD PTR [rsp+304]
        add	rsp, 320
        pop	rdi
        pop	r15
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_encrypt_update_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_final_avx1 PROC
        push	r13
        push	r12
        push	r14
        mov	rax, rcx
        mov	r10d, r9d
        mov	r9, rdx
        mov	r11d, DWORD PTR [rsp+64]
        mov	r12, QWORD PTR [rsp+72]
        mov	r14, QWORD PTR [rsp+80]
        sub	rsp, 144
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vmovdqu	OWORD PTR [rsp+48], xmm8
        vmovdqu	OWORD PTR [rsp+64], xmm9
        vmovdqu	OWORD PTR [rsp+80], xmm10
        vmovdqu	OWORD PTR [rsp+96], xmm11
        vmovdqu	OWORD PTR [rsp+112], xmm12
        vmovdqu	OWORD PTR [rsp+128], xmm13
        vmovdqa	xmm4, OWORD PTR [rax]
        vmovdqa	xmm5, OWORD PTR [r12]
        vmovdqa	xmm6, OWORD PTR [r14]
        vpsrlq	xmm8, xmm5, 63
        vpsllq	xmm7, xmm5, 1
        vpslldq	xmm8, xmm8, 8
        vpor	xmm7, xmm7, xmm8
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm7
        mov	edx, r10d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        vmovq	xmm0, rdx
        vmovq	xmm1, rcx
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_red_avx
        vpshufd	xmm8, xmm5, 78
        vpshufd	xmm9, xmm4, 78
        vpclmulqdq	xmm10, xmm4, xmm5, 17
        vpclmulqdq	xmm7, xmm4, xmm5, 0
        vpxor	xmm8, xmm8, xmm5
        vpxor	xmm9, xmm9, xmm4
        vpclmulqdq	xmm8, xmm8, xmm9, 0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm8, xmm8, xmm10
        vpslldq	xmm9, xmm8, 8
        vpsrldq	xmm8, xmm8, 8
        vpxor	xmm7, xmm7, xmm9
        vpxor	xmm4, xmm10, xmm8
        vpslld	xmm11, xmm7, 31
        vpslld	xmm12, xmm7, 30
        vpslld	xmm13, xmm7, 25
        vpxor	xmm11, xmm11, xmm12
        vpxor	xmm11, xmm11, xmm13
        vpsrldq	xmm12, xmm11, 4
        vpslldq	xmm11, xmm11, 12
        vpxor	xmm7, xmm7, xmm11
        vpsrld	xmm13, xmm7, 1
        vpsrld	xmm9, xmm7, 2
        vpsrld	xmm8, xmm7, 7
        vpxor	xmm13, xmm13, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm13, xmm13, xmm12
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm4, xmm4, xmm13
        vpshufb	xmm4, xmm4, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm4, xmm6
        cmp	r8d, 16
        je	L_AES_GCM_encrypt_final_avx1_store_tag_16
        xor	rcx, rcx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_final_avx1_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r9+rcx], r13b
        inc	ecx
        cmp	ecx, r8d
        jne	L_AES_GCM_encrypt_final_avx1_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_avx1_store_tag_done
L_AES_GCM_encrypt_final_avx1_store_tag_16:
        vmovdqu	OWORD PTR [r9], xmm0
L_AES_GCM_encrypt_final_avx1_store_tag_done:
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm8, OWORD PTR [rsp+48]
        vmovdqu	xmm9, OWORD PTR [rsp+64]
        vmovdqu	xmm10, OWORD PTR [rsp+80]
        vmovdqu	xmm11, OWORD PTR [rsp+96]
        vmovdqu	xmm12, OWORD PTR [rsp+112]
        vmovdqu	xmm13, OWORD PTR [rsp+128]
        add	rsp, 144
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_encrypt_final_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_update_avx1 PROC
        push	r13
        push	r12
        push	r14
        push	r15
        push	rdi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	r15, QWORD PTR [rsp+104]
        sub	rsp, 328
        vmovdqu	OWORD PTR [rsp+168], xmm6
        vmovdqu	OWORD PTR [rsp+184], xmm7
        vmovdqu	OWORD PTR [rsp+200], xmm8
        vmovdqu	OWORD PTR [rsp+216], xmm9
        vmovdqu	OWORD PTR [rsp+232], xmm10
        vmovdqu	OWORD PTR [rsp+248], xmm11
        vmovdqu	OWORD PTR [rsp+264], xmm12
        vmovdqu	OWORD PTR [rsp+280], xmm13
        vmovdqu	OWORD PTR [rsp+296], xmm14
        vmovdqu	OWORD PTR [rsp+312], xmm15
        vmovdqa	xmm6, OWORD PTR [r12]
        vmovdqa	xmm5, OWORD PTR [r14]
        vpsrlq	xmm9, xmm5, 63
        vpsllq	xmm8, xmm5, 1
        vpslldq	xmm9, xmm9, 8
        vpor	xmm8, xmm8, xmm9
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm8
        xor	edi, edi
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_decrypt_update_avx1_done_128
        and	r13d, 4294967168
        vmovdqa	xmm2, xmm6
        ; H ^ 1
        vmovdqu	OWORD PTR [rsp], xmm5
        ; H ^ 2
        vpclmulqdq	xmm8, xmm5, xmm5, 0
        vpclmulqdq	xmm0, xmm5, xmm5, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm0, xmm0, xmm14
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm5, 78
        vpshufd	xmm10, xmm0, 78
        vpclmulqdq	xmm11, xmm0, xmm5, 17
        vpclmulqdq	xmm8, xmm0, xmm5, 0
        vpxor	xmm9, xmm9, xmm5
        vpxor	xmm10, xmm10, xmm0
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm1, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm1, xmm1, xmm14
        vmovdqu	OWORD PTR [rsp+32], xmm1
        ; H ^ 4
        vpclmulqdq	xmm8, xmm0, xmm0, 0
        vpclmulqdq	xmm3, xmm0, xmm0, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm3, xmm3, xmm14
        vmovdqu	OWORD PTR [rsp+48], xmm3
        ; H ^ 5
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm0, 78
        vpshufd	xmm10, xmm1, 78
        vpclmulqdq	xmm11, xmm1, xmm0, 17
        vpclmulqdq	xmm8, xmm1, xmm0, 0
        vpxor	xmm9, xmm9, xmm0
        vpxor	xmm10, xmm10, xmm1
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+64], xmm7
        ; H ^ 6
        vpclmulqdq	xmm8, xmm1, xmm1, 0
        vpclmulqdq	xmm7, xmm1, xmm1, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+80], xmm7
        ; H ^ 7
        ; ghash_gfmul_red_avx
        vpshufd	xmm9, xmm1, 78
        vpshufd	xmm10, xmm3, 78
        vpclmulqdq	xmm11, xmm3, xmm1, 17
        vpclmulqdq	xmm8, xmm3, xmm1, 0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm3
        vpclmulqdq	xmm9, xmm9, xmm10, 0
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm9, xmm9, xmm11
        vpslldq	xmm10, xmm9, 8
        vpsrldq	xmm9, xmm9, 8
        vpxor	xmm8, xmm8, xmm10
        vpxor	xmm7, xmm11, xmm9
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+96], xmm7
        ; H ^ 8
        vpclmulqdq	xmm8, xmm3, xmm3, 0
        vpclmulqdq	xmm7, xmm3, xmm3, 17
        vpslld	xmm12, xmm8, 31
        vpslld	xmm13, xmm8, 30
        vpslld	xmm14, xmm8, 25
        vpxor	xmm12, xmm12, xmm13
        vpxor	xmm12, xmm12, xmm14
        vpsrldq	xmm13, xmm12, 4
        vpslldq	xmm12, xmm12, 12
        vpxor	xmm8, xmm8, xmm12
        vpsrld	xmm14, xmm8, 1
        vpsrld	xmm10, xmm8, 2
        vpsrld	xmm9, xmm8, 7
        vpxor	xmm14, xmm14, xmm10
        vpxor	xmm14, xmm14, xmm9
        vpxor	xmm14, xmm14, xmm13
        vpxor	xmm14, xmm14, xmm8
        vpxor	xmm7, xmm7, xmm14
        vmovdqu	OWORD PTR [rsp+112], xmm7
L_AES_GCM_decrypt_update_avx1_ghash_128:
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        vmovdqu	xmm0, OWORD PTR [r15]
        vmovdqa	xmm1, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm9, xmm0, OWORD PTR L_avx1_aes_gcm_one
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx1_aes_gcm_two
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx1_aes_gcm_three
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx1_aes_gcm_four
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx1_aes_gcm_five
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx1_aes_gcm_six
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx1_aes_gcm_seven
        vpshufb	xmm15, xmm15, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_eight
        vmovdqa	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [r15], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vmovdqu	xmm0, OWORD PTR [rcx]
        vaesenc	xmm8, xmm8, [rax+16]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm2
        vpshufd	xmm1, xmm7, 78
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm3, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+16]
        vaesenc	xmm10, xmm10, [rax+16]
        vpclmulqdq	xmm2, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+16]
        vaesenc	xmm12, xmm12, [rax+16]
        vpclmulqdq	xmm1, xmm1, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+16]
        vaesenc	xmm14, xmm14, [rax+16]
        vaesenc	xmm15, xmm15, [rax+16]
        vpxor	xmm1, xmm1, xmm2
        vpxor	xmm1, xmm1, xmm3
        vmovdqu	xmm7, OWORD PTR [rsp+96]
        vmovdqu	xmm0, OWORD PTR [rcx+16]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+32]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+32]
        vaesenc	xmm10, xmm10, [rax+32]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+32]
        vaesenc	xmm12, xmm12, [rax+32]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+32]
        vaesenc	xmm14, xmm14, [rax+32]
        vaesenc	xmm15, xmm15, [rax+32]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+48]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+48]
        vaesenc	xmm10, xmm10, [rax+48]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+48]
        vaesenc	xmm12, xmm12, [rax+48]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+48]
        vaesenc	xmm14, xmm14, [rax+48]
        vaesenc	xmm15, xmm15, [rax+48]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+64]
        vmovdqu	xmm0, OWORD PTR [rcx+48]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+64]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+64]
        vaesenc	xmm10, xmm10, [rax+64]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+64]
        vaesenc	xmm12, xmm12, [rax+64]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+64]
        vaesenc	xmm14, xmm14, [rax+64]
        vaesenc	xmm15, xmm15, [rax+64]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+80]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+80]
        vaesenc	xmm10, xmm10, [rax+80]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+80]
        vaesenc	xmm12, xmm12, [rax+80]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+80]
        vaesenc	xmm14, xmm14, [rax+80]
        vaesenc	xmm15, xmm15, [rax+80]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm0, OWORD PTR [rcx+80]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+96]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+96]
        vaesenc	xmm10, xmm10, [rax+96]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+96]
        vaesenc	xmm12, xmm12, [rax+96]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+96]
        vaesenc	xmm14, xmm14, [rax+96]
        vaesenc	xmm15, xmm15, [rax+96]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+112]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+112]
        vaesenc	xmm10, xmm10, [rax+112]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+112]
        vaesenc	xmm12, xmm12, [rax+112]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+112]
        vaesenc	xmm14, xmm14, [rax+112]
        vaesenc	xmm15, xmm15, [rax+112]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vmovdqu	xmm7, OWORD PTR [rsp]
        vmovdqu	xmm0, OWORD PTR [rcx+112]
        vpshufd	xmm4, xmm7, 78
        vpshufb	xmm0, xmm0, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vaesenc	xmm8, xmm8, [rax+128]
        vpxor	xmm4, xmm4, xmm7
        vpshufd	xmm5, xmm0, 78
        vpxor	xmm5, xmm5, xmm0
        vpclmulqdq	xmm6, xmm0, xmm7, 17
        vaesenc	xmm9, xmm9, [rax+128]
        vaesenc	xmm10, xmm10, [rax+128]
        vpclmulqdq	xmm7, xmm0, xmm7, 0
        vaesenc	xmm11, xmm11, [rax+128]
        vaesenc	xmm12, xmm12, [rax+128]
        vpclmulqdq	xmm4, xmm4, xmm5, 0
        vaesenc	xmm13, xmm13, [rax+128]
        vaesenc	xmm14, xmm14, [rax+128]
        vaesenc	xmm15, xmm15, [rax+128]
        vpxor	xmm1, xmm1, xmm7
        vpxor	xmm2, xmm2, xmm7
        vpxor	xmm1, xmm1, xmm6
        vpxor	xmm3, xmm3, xmm6
        vpxor	xmm1, xmm1, xmm4
        vpslldq	xmm5, xmm1, 8
        vpsrldq	xmm1, xmm1, 8
        vaesenc	xmm8, xmm8, [rax+144]
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm3, xmm3, xmm1
        vaesenc	xmm9, xmm9, [rax+144]
        vpslld	xmm7, xmm2, 31
        vpslld	xmm4, xmm2, 30
        vpslld	xmm5, xmm2, 25
        vaesenc	xmm10, xmm10, [rax+144]
        vpxor	xmm7, xmm7, xmm4
        vpxor	xmm7, xmm7, xmm5
        vaesenc	xmm11, xmm11, [rax+144]
        vpsrldq	xmm4, xmm7, 4
        vpslldq	xmm7, xmm7, 12
        vaesenc	xmm12, xmm12, [rax+144]
        vpxor	xmm2, xmm2, xmm7
        vpsrld	xmm5, xmm2, 1
        vaesenc	xmm13, xmm13, [rax+144]
        vpsrld	xmm1, xmm2, 2
        vpsrld	xmm0, xmm2, 7
        vaesenc	xmm14, xmm14, [rax+144]
        vpxor	xmm5, xmm5, xmm1
        vpxor	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, [rax+144]
        vpxor	xmm5, xmm5, xmm4
        vpxor	xmm2, xmm2, xmm5
        vpxor	xmm2, xmm2, xmm3
        cmp	r8d, 11
        vmovdqa	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqa	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqa	xmm7, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_avx1_aesenc_128_ghash_avx_done:
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+32]
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vpxor	xmm10, xmm10, xmm0
        vpxor	xmm11, xmm11, xmm1
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+96]
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vpxor	xmm14, xmm14, xmm0
        vpxor	xmm15, xmm15, xmm1
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        add	edi, 128
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_avx1_ghash_128
        vmovdqa	xmm6, xmm2
        vmovdqu	xmm5, OWORD PTR [rsp]
L_AES_GCM_decrypt_update_avx1_done_128:
        mov	edx, r9d
        cmp	edi, edx
        jge	L_AES_GCM_decrypt_update_avx1_done_dec
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	edi, r13d
        jge	L_AES_GCM_decrypt_update_avx1_last_block_done
L_AES_GCM_decrypt_update_avx1_last_block_start:
        vmovdqu	xmm13, OWORD PTR [r11+rdi]
        vmovdqa	xmm0, xmm5
        vpshufb	xmm1, xmm13, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm1, xmm1, xmm6
        vmovdqu	xmm9, OWORD PTR [r15]
        vpshufb	xmm8, xmm9, OWORD PTR L_avx1_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm9, OWORD PTR L_avx1_aes_gcm_one
        vmovdqu	OWORD PTR [r15], xmm9
        vpxor	xmm8, xmm8, [rax]
        vpclmulqdq	xmm10, xmm1, xmm0, 16
        vaesenc	xmm8, xmm8, [rax+16]
        vaesenc	xmm8, xmm8, [rax+32]
        vpclmulqdq	xmm11, xmm1, xmm0, 1
        vaesenc	xmm8, xmm8, [rax+48]
        vaesenc	xmm8, xmm8, [rax+64]
        vpclmulqdq	xmm12, xmm1, xmm0, 0
        vaesenc	xmm8, xmm8, [rax+80]
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vaesenc	xmm8, xmm8, [rax+96]
        vpxor	xmm10, xmm10, xmm11
        vpslldq	xmm2, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vaesenc	xmm8, xmm8, [rax+112]
        vpxor	xmm2, xmm2, xmm12
        vpxor	xmm3, xmm1, xmm10
        vmovdqa	xmm0, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpclmulqdq	xmm11, xmm2, xmm0, 16
        vaesenc	xmm8, xmm8, [rax+128]
        vpshufd	xmm10, xmm2, 78
        vpxor	xmm10, xmm10, xmm11
        vpclmulqdq	xmm11, xmm10, xmm0, 16
        vaesenc	xmm8, xmm8, [rax+144]
        vpshufd	xmm10, xmm10, 78
        vpxor	xmm10, xmm10, xmm11
        vpxor	xmm6, xmm10, xmm3
        cmp	r8d, 11
        vmovdqa	xmm9, OWORD PTR [rax+160]
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+176]
        cmp	r8d, 13
        vmovdqa	xmm9, OWORD PTR [rax+192]
        jl	L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last
        vaesenc	xmm8, xmm8, xmm9
        vaesenc	xmm8, xmm8, [rax+208]
        vmovdqa	xmm9, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_avx1_aesenc_gfmul_last:
        vaesenclast	xmm8, xmm8, xmm9
        vmovdqa	xmm0, xmm13
        vpxor	xmm8, xmm8, xmm0
        vmovdqu	OWORD PTR [r10+rdi], xmm8
        add	edi, 16
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_avx1_last_block_start
L_AES_GCM_decrypt_update_avx1_last_block_done:
L_AES_GCM_decrypt_update_avx1_done_dec:
        vmovdqa	OWORD PTR [r12], xmm6
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+168]
        vmovdqu	xmm7, OWORD PTR [rsp+184]
        vmovdqu	xmm8, OWORD PTR [rsp+200]
        vmovdqu	xmm9, OWORD PTR [rsp+216]
        vmovdqu	xmm10, OWORD PTR [rsp+232]
        vmovdqu	xmm11, OWORD PTR [rsp+248]
        vmovdqu	xmm12, OWORD PTR [rsp+264]
        vmovdqu	xmm13, OWORD PTR [rsp+280]
        vmovdqu	xmm14, OWORD PTR [rsp+296]
        vmovdqu	xmm15, OWORD PTR [rsp+312]
        add	rsp, 328
        pop	rdi
        pop	r15
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_decrypt_update_avx1 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_final_avx1 PROC
        push	r13
        push	r12
        push	r14
        push	rbp
        push	r15
        mov	rax, rcx
        mov	r10d, r9d
        mov	r9, rdx
        mov	r11d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	rbp, QWORD PTR [rsp+104]
        sub	rsp, 160
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vmovdqu	OWORD PTR [rsp+48], xmm8
        vmovdqu	OWORD PTR [rsp+64], xmm9
        vmovdqu	OWORD PTR [rsp+80], xmm10
        vmovdqu	OWORD PTR [rsp+96], xmm11
        vmovdqu	OWORD PTR [rsp+112], xmm12
        vmovdqu	OWORD PTR [rsp+128], xmm13
        vmovdqu	OWORD PTR [rsp+144], xmm15
        vmovdqa	xmm6, OWORD PTR [rax]
        vmovdqa	xmm5, OWORD PTR [r12]
        vmovdqa	xmm15, OWORD PTR [r14]
        vpsrlq	xmm8, xmm5, 63
        vpsllq	xmm7, xmm5, 1
        vpslldq	xmm8, xmm8, 8
        vpor	xmm7, xmm7, xmm8
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx1_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm7
        mov	edx, r10d
        mov	ecx, r11d
        shl	rdx, 3
        shl	rcx, 3
        vmovq	xmm0, rdx
        vmovq	xmm1, rcx
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_red_avx
        vpshufd	xmm8, xmm5, 78
        vpshufd	xmm9, xmm6, 78
        vpclmulqdq	xmm10, xmm6, xmm5, 17
        vpclmulqdq	xmm7, xmm6, xmm5, 0
        vpxor	xmm8, xmm8, xmm5
        vpxor	xmm9, xmm9, xmm6
        vpclmulqdq	xmm8, xmm8, xmm9, 0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm8, xmm8, xmm10
        vpslldq	xmm9, xmm8, 8
        vpsrldq	xmm8, xmm8, 8
        vpxor	xmm7, xmm7, xmm9
        vpxor	xmm6, xmm10, xmm8
        vpslld	xmm11, xmm7, 31
        vpslld	xmm12, xmm7, 30
        vpslld	xmm13, xmm7, 25
        vpxor	xmm11, xmm11, xmm12
        vpxor	xmm11, xmm11, xmm13
        vpsrldq	xmm12, xmm11, 4
        vpslldq	xmm11, xmm11, 12
        vpxor	xmm7, xmm7, xmm11
        vpsrld	xmm13, xmm7, 1
        vpsrld	xmm9, xmm7, 2
        vpsrld	xmm8, xmm7, 7
        vpxor	xmm13, xmm13, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm13, xmm13, xmm12
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm6, xmm6, xmm13
        vpshufb	xmm6, xmm6, OWORD PTR L_avx1_aes_gcm_bswap_mask
        vpxor	xmm0, xmm6, xmm15
        cmp	r8d, 16
        je	L_AES_GCM_decrypt_final_avx1_cmp_tag_16
        sub	rsp, 16
        xor	rcx, rcx
        xor	r15, r15
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_final_avx1_cmp_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        xor	r13b, BYTE PTR [r9+rcx]
        or	r15b, r13b
        inc	ecx
        cmp	ecx, r8d
        jne	L_AES_GCM_decrypt_final_avx1_cmp_tag_loop
        cmp	r15, 0
        sete	r15b
        add	rsp, 16
        xor	rcx, rcx
        jmp	L_AES_GCM_decrypt_final_avx1_cmp_tag_done
L_AES_GCM_decrypt_final_avx1_cmp_tag_16:
        vmovdqu	xmm1, OWORD PTR [r9]
        vpcmpeqb	xmm0, xmm0, xmm1
        vpmovmskb	rdx, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	r15d, r15d
        cmp	edx, 65535
        sete	r15b
L_AES_GCM_decrypt_final_avx1_cmp_tag_done:
        mov	DWORD PTR [rbp], r15d
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        vmovdqu	xmm8, OWORD PTR [rsp+48]
        vmovdqu	xmm9, OWORD PTR [rsp+64]
        vmovdqu	xmm10, OWORD PTR [rsp+80]
        vmovdqu	xmm11, OWORD PTR [rsp+96]
        vmovdqu	xmm12, OWORD PTR [rsp+112]
        vmovdqu	xmm13, OWORD PTR [rsp+128]
        vmovdqu	xmm15, OWORD PTR [rsp+144]
        add	rsp, 160
        pop	r15
        pop	rbp
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_decrypt_final_avx1 ENDP
_text ENDS
ENDIF
IFDEF HAVE_INTEL_AVX2
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_one QWORD 0, 1
ptr_L_avx2_aes_gcm_one QWORD L_avx2_aes_gcm_one
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_two QWORD 0, 2
ptr_L_avx2_aes_gcm_two QWORD L_avx2_aes_gcm_two
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_three QWORD 0, 3
ptr_L_avx2_aes_gcm_three QWORD L_avx2_aes_gcm_three
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_four QWORD 0, 4
ptr_L_avx2_aes_gcm_four QWORD L_avx2_aes_gcm_four
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_five QWORD 0, 5
ptr_L_avx2_aes_gcm_five QWORD L_avx2_aes_gcm_five
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_six QWORD 0, 6
ptr_L_avx2_aes_gcm_six QWORD L_avx2_aes_gcm_six
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_seven QWORD 0, 7
ptr_L_avx2_aes_gcm_seven QWORD L_avx2_aes_gcm_seven
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_eight QWORD 0, 8
ptr_L_avx2_aes_gcm_eight QWORD L_avx2_aes_gcm_eight
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_bswap_one QWORD 0, 72057594037927936
ptr_L_avx2_aes_gcm_bswap_one QWORD L_avx2_aes_gcm_bswap_one
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_bswap_epi64 QWORD 283686952306183, 579005069656919567
ptr_L_avx2_aes_gcm_bswap_epi64 QWORD L_avx2_aes_gcm_bswap_epi64
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_bswap_mask QWORD 579005069656919567, 283686952306183
ptr_L_avx2_aes_gcm_bswap_mask QWORD L_avx2_aes_gcm_bswap_mask
_DATA ENDS
_DATA SEGMENT
ALIGN 16
L_avx2_aes_gcm_mod2_128 QWORD 1, 13979173243358019584
ptr_L_avx2_aes_gcm_mod2_128 QWORD L_avx2_aes_gcm_mod2_128
_DATA ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_avx2 PROC
        push	r13
        push	rdi
        push	r12
        push	r15
        push	rbx
        push	r14
        push	rsi
        mov	rdi, rcx
        mov	r12, r8
        mov	rax, r9
        mov	r15, QWORD PTR [rsp+96]
        mov	r8, rdx
        mov	r10d, DWORD PTR [rsp+104]
        mov	r11d, DWORD PTR [rsp+112]
        mov	ebx, DWORD PTR [rsp+120]
        mov	r14d, DWORD PTR [rsp+128]
        mov	rsi, QWORD PTR [rsp+136]
        mov	r9d, DWORD PTR [rsp+144]
        sub	rsp, 320
        vmovdqu	OWORD PTR [rsp+160], xmm6
        vmovdqu	OWORD PTR [rsp+176], xmm7
        vmovdqu	OWORD PTR [rsp+192], xmm8
        vmovdqu	OWORD PTR [rsp+208], xmm9
        vmovdqu	OWORD PTR [rsp+224], xmm10
        vmovdqu	OWORD PTR [rsp+240], xmm11
        vmovdqu	OWORD PTR [rsp+256], xmm12
        vmovdqu	OWORD PTR [rsp+272], xmm13
        vmovdqu	OWORD PTR [rsp+288], xmm14
        vmovdqu	OWORD PTR [rsp+304], xmm15
        vpxor	xmm4, xmm4, xmm4
        vpxor	xmm6, xmm6, xmm6
        mov	edx, ebx
        cmp	edx, 12
        je	L_AES_GCM_encrypt_avx2_iv_12
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqu	xmm5, OWORD PTR [rsi]
        vaesenc	xmm5, xmm5, [rsi+16]
        vaesenc	xmm5, xmm5, [rsi+32]
        vaesenc	xmm5, xmm5, [rsi+48]
        vaesenc	xmm5, xmm5, [rsi+64]
        vaesenc	xmm5, xmm5, [rsi+80]
        vaesenc	xmm5, xmm5, [rsi+96]
        vaesenc	xmm5, xmm5, [rsi+112]
        vaesenc	xmm5, xmm5, [rsi+128]
        vaesenc	xmm5, xmm5, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_encrypt_avx2_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_avx2_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_avx2_calc_iv_16_loop:
        vmovdqu	xmm0, OWORD PTR [rax+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx2_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_avx2_calc_iv_done
L_AES_GCM_encrypt_avx2_calc_iv_lt16:
        vpxor	xmm0, xmm0, xmm0
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_avx2_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx2_calc_iv_loop
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
L_AES_GCM_encrypt_avx2_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqu	xmm15, OWORD PTR [rsi]
        vpxor	xmm15, xmm15, xmm4
        vaesenc	xmm15, xmm15, [rsi+16]
        vaesenc	xmm15, xmm15, [rsi+32]
        vaesenc	xmm15, xmm15, [rsi+48]
        vaesenc	xmm15, xmm15, [rsi+64]
        vaesenc	xmm15, xmm15, [rsi+80]
        vaesenc	xmm15, xmm15, [rsi+96]
        vaesenc	xmm15, xmm15, [rsi+112]
        vaesenc	xmm15, xmm15, [rsi+128]
        vaesenc	xmm15, xmm15, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm15, xmm15, xmm0
        vaesenc	xmm15, xmm15, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm15, xmm15, xmm0
        vaesenc	xmm15, xmm15, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm15, xmm15, xmm0
        jmp	L_AES_GCM_encrypt_avx2_iv_done
L_AES_GCM_encrypt_avx2_iv_12:
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        vmovdqu	xmm4, OWORD PTR L_avx2_aes_gcm_bswap_one
        vmovdqu	xmm5, OWORD PTR [rsi]
        vpblendd	xmm4, xmm4, [rax], 7
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	xmm7, OWORD PTR [rsi+16]
        vpxor	xmm15, xmm4, xmm5
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rsi+32]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+48]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+64]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+80]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+96]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+112]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+128]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+144]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+176]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+208]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm0
        vaesenclast	xmm15, xmm15, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
L_AES_GCM_encrypt_avx2_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_encrypt_avx2_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_encrypt_avx2_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_encrypt_avx2_calc_aad_16_loop:
        vmovdqu	xmm0, OWORD PTR [r12+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm6, 16
        vpclmulqdq	xmm1, xmm5, xmm6, 1
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm6, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx2_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_encrypt_avx2_calc_aad_done
L_AES_GCM_encrypt_avx2_calc_aad_lt16:
        vpxor	xmm0, xmm0, xmm0
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_avx2_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_encrypt_avx2_calc_aad_loop
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm6, 16
        vpclmulqdq	xmm1, xmm5, xmm6, 1
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm6, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
L_AES_GCM_encrypt_avx2_calc_aad_done:
        ; Calculate counter and H
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm5, xmm5, xmm0
        xor	ebx, ebx
        cmp	r10d, 128
        mov	r13d, r10d
        jl	L_AES_GCM_encrypt_avx2_done_128
        and	r13d, 4294967168
        vmovdqu	OWORD PTR [rsp+128], xmm4
        vmovdqu	OWORD PTR [rsp+144], xmm15
        vmovdqu	xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128
        ; H ^ 1 and H ^ 2
        vpclmulqdq	xmm9, xmm5, xmm5, 0
        vpclmulqdq	xmm10, xmm5, xmm5, 17
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm0, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp], xmm5
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3 and H ^ 4
        vpclmulqdq	xmm11, xmm0, xmm5, 16
        vpclmulqdq	xmm10, xmm0, xmm5, 1
        vpclmulqdq	xmm9, xmm0, xmm5, 0
        vpclmulqdq	xmm12, xmm0, xmm5, 17
        vpclmulqdq	xmm13, xmm0, xmm0, 0
        vpclmulqdq	xmm14, xmm0, xmm0, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm2, xmm13, xmm14
        vpxor	xmm1, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+32], xmm1
        vmovdqu	OWORD PTR [rsp+48], xmm2
        ; H ^ 5 and H ^ 6
        vpclmulqdq	xmm11, xmm1, xmm0, 16
        vpclmulqdq	xmm10, xmm1, xmm0, 1
        vpclmulqdq	xmm9, xmm1, xmm0, 0
        vpclmulqdq	xmm12, xmm1, xmm0, 17
        vpclmulqdq	xmm13, xmm1, xmm1, 0
        vpclmulqdq	xmm14, xmm1, xmm1, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+64], xmm7
        vmovdqu	OWORD PTR [rsp+80], xmm0
        ; H ^ 7 and H ^ 8
        vpclmulqdq	xmm11, xmm2, xmm1, 16
        vpclmulqdq	xmm10, xmm2, xmm1, 1
        vpclmulqdq	xmm9, xmm2, xmm1, 0
        vpclmulqdq	xmm12, xmm2, xmm1, 17
        vpclmulqdq	xmm13, xmm2, xmm2, 0
        vpclmulqdq	xmm14, xmm2, xmm2, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+96], xmm7
        vmovdqu	OWORD PTR [rsp+112], xmm0
        ; First 128 bytes of input
        ; aesenc_128
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rsi]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+16]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+32]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+48]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+64]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+80]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+96]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+112]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+128]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+144]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r9d, 11
        vmovdqu	xmm7, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r9d, 13
        vmovdqu	xmm7, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_aesenc_128_enc_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi]
        vmovdqu	xmm1, OWORD PTR [rdi+16]
        vmovdqu	xmm2, OWORD PTR [rdi+32]
        vmovdqu	xmm3, OWORD PTR [rdi+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [r8], xmm8
        vmovdqu	OWORD PTR [r8+16], xmm9
        vmovdqu	OWORD PTR [r8+32], xmm10
        vmovdqu	OWORD PTR [r8+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rdi+64]
        vmovdqu	xmm1, OWORD PTR [rdi+80]
        vmovdqu	xmm2, OWORD PTR [rdi+96]
        vmovdqu	xmm3, OWORD PTR [rdi+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [r8+64], xmm12
        vmovdqu	OWORD PTR [r8+80], xmm13
        vmovdqu	OWORD PTR [r8+96], xmm14
        vmovdqu	OWORD PTR [r8+112], xmm15
        cmp	r13d, 128
        mov	ebx, 128
        jle	L_AES_GCM_encrypt_avx2_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_avx2_ghash_128:
        ; aesenc_128_ghash
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [r8+rbx]
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rsi]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        ; aesenc_pclmul_1
        vmovdqu	xmm1, OWORD PTR [rdx+-128]
        vmovdqu	xmm0, OWORD PTR [rsi+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vmovdqu	xmm2, OWORD PTR [rsp+112]
        vpxor	xmm1, xmm1, xmm6
        vpclmulqdq	xmm5, xmm1, xmm2, 16
        vpclmulqdq	xmm3, xmm1, xmm2, 1
        vpclmulqdq	xmm6, xmm1, xmm2, 0
        vpclmulqdq	xmm7, xmm1, xmm2, 17
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_2
        vmovdqu	xmm1, OWORD PTR [rdx+-112]
        vmovdqu	xmm0, OWORD PTR [rsp+96]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+32]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-96]
        vmovdqu	xmm0, OWORD PTR [rsp+80]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+48]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-80]
        vmovdqu	xmm0, OWORD PTR [rsp+64]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+64]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-64]
        vmovdqu	xmm0, OWORD PTR [rsp+48]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+80]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-48]
        vmovdqu	xmm0, OWORD PTR [rsp+32]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+96]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-32]
        vmovdqu	xmm0, OWORD PTR [rsp+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+112]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-16]
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+128]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_l
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm6, xmm6, xmm4
        vpxor	xmm5, xmm5, xmm3
        vpslldq	xmm1, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vmovdqu	xmm4, OWORD PTR [rsi+144]
        vmovdqu	xmm0, OWORD PTR L_avx2_aes_gcm_mod2_128
        vaesenc	xmm8, xmm8, xmm4
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm7, xmm7, xmm5
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm9, xmm9, xmm4
        vaesenc	xmm10, xmm10, xmm4
        vaesenc	xmm11, xmm11, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm12, xmm12, xmm4
        vaesenc	xmm13, xmm13, xmm4
        vaesenc	xmm14, xmm14, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm6, xmm6, xmm7
        vaesenc	xmm15, xmm15, xmm4
        cmp	r9d, 11
        vmovdqu	xmm7, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r9d, 13
        vmovdqu	xmm7, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_aesenc_128_ghash_avx_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm2, OWORD PTR [rcx+32]
        vmovdqu	xmm3, OWORD PTR [rcx+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm2, OWORD PTR [rcx+96]
        vmovdqu	xmm3, OWORD PTR [rcx+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        ; aesenc_128_ghash - end
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_avx2_ghash_128
L_AES_GCM_encrypt_avx2_end_128:
        vmovdqu	xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpshufb	xmm8, xmm8, xmm4
        vpshufb	xmm9, xmm9, xmm4
        vpshufb	xmm10, xmm10, xmm4
        vpshufb	xmm11, xmm11, xmm4
        vpshufb	xmm12, xmm12, xmm4
        vpshufb	xmm13, xmm13, xmm4
        vpshufb	xmm14, xmm14, xmm4
        vpshufb	xmm15, xmm15, xmm4
        vpxor	xmm8, xmm8, xmm6
        vmovdqu	xmm7, OWORD PTR [rsp]
        vpclmulqdq	xmm5, xmm7, xmm15, 16
        vpclmulqdq	xmm1, xmm7, xmm15, 1
        vpclmulqdq	xmm4, xmm7, xmm15, 0
        vpclmulqdq	xmm6, xmm7, xmm15, 17
        vpxor	xmm5, xmm5, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vpclmulqdq	xmm2, xmm7, xmm14, 16
        vpclmulqdq	xmm1, xmm7, xmm14, 1
        vpclmulqdq	xmm0, xmm7, xmm14, 0
        vpclmulqdq	xmm3, xmm7, xmm14, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+32]
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vpclmulqdq	xmm2, xmm15, xmm13, 16
        vpclmulqdq	xmm1, xmm15, xmm13, 1
        vpclmulqdq	xmm0, xmm15, xmm13, 0
        vpclmulqdq	xmm3, xmm15, xmm13, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm12, 16
        vpclmulqdq	xmm1, xmm7, xmm12, 1
        vpclmulqdq	xmm0, xmm7, xmm12, 0
        vpclmulqdq	xmm3, xmm7, xmm12, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+64]
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vpclmulqdq	xmm2, xmm15, xmm11, 16
        vpclmulqdq	xmm1, xmm15, xmm11, 1
        vpclmulqdq	xmm0, xmm15, xmm11, 0
        vpclmulqdq	xmm3, xmm15, xmm11, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm10, 16
        vpclmulqdq	xmm1, xmm7, xmm10, 1
        vpclmulqdq	xmm0, xmm7, xmm10, 0
        vpclmulqdq	xmm3, xmm7, xmm10, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+96]
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vpclmulqdq	xmm2, xmm15, xmm9, 16
        vpclmulqdq	xmm1, xmm15, xmm9, 1
        vpclmulqdq	xmm0, xmm15, xmm9, 0
        vpclmulqdq	xmm3, xmm15, xmm9, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm8, 16
        vpclmulqdq	xmm1, xmm7, xmm8, 1
        vpclmulqdq	xmm0, xmm7, xmm8, 0
        vpclmulqdq	xmm3, xmm7, xmm8, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpslldq	xmm7, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vpxor	xmm4, xmm4, xmm7
        vpxor	xmm6, xmm6, xmm5
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm4, xmm2, 16
        vpshufd	xmm1, xmm4, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm5, OWORD PTR [rsp]
        vmovdqu	xmm4, OWORD PTR [rsp+128]
        vmovdqu	xmm15, OWORD PTR [rsp+144]
L_AES_GCM_encrypt_avx2_done_128:
        cmp	ebx, r10d
        je	L_AES_GCM_encrypt_avx2_done_enc
        mov	r13d, r10d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_avx2_last_block_done
        ; aesenc_block
        vmovdqu	xmm1, xmm4
        vpshufb	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm0, xmm0, [rsi]
        vmovdqu	xmm2, OWORD PTR [rsi+16]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+32]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+48]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+64]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+80]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+96]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+112]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+128]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rsi+144]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm4, xmm1
        cmp	r9d, 11
        vmovdqu	xmm1, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rsi+176]
        vaesenc	xmm0, xmm0, xmm2
        cmp	r9d, 13
        vmovdqu	xmm1, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rsi+208]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm1, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_aesenc_block_last:
        vaesenclast	xmm0, xmm0, xmm1
        vmovdqu	xmm1, OWORD PTR [rdi+rbx]
        vpxor	xmm0, xmm0, xmm1
        vmovdqu	OWORD PTR [r8+rbx], xmm0
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        add	ebx, 16
        cmp	ebx, r13d
        jge	L_AES_GCM_encrypt_avx2_last_block_ghash
L_AES_GCM_encrypt_avx2_last_block_start:
        vmovdqu	xmm12, OWORD PTR [rdi+rbx]
        vpshufb	xmm11, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        ; aesenc_gfmul_sb
        vpclmulqdq	xmm2, xmm6, xmm5, 1
        vpclmulqdq	xmm3, xmm6, xmm5, 16
        vpclmulqdq	xmm1, xmm6, xmm5, 0
        vpclmulqdq	xmm8, xmm6, xmm5, 17
        vpxor	xmm11, xmm11, [rsi]
        vaesenc	xmm11, xmm11, [rsi+16]
        vpxor	xmm3, xmm3, xmm2
        vpslldq	xmm2, xmm3, 8
        vpsrldq	xmm3, xmm3, 8
        vaesenc	xmm11, xmm11, [rsi+32]
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm11, xmm11, [rsi+48]
        vaesenc	xmm11, xmm11, [rsi+64]
        vaesenc	xmm11, xmm11, [rsi+80]
        vpshufd	xmm2, xmm2, 78
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm11, xmm11, [rsi+96]
        vaesenc	xmm11, xmm11, [rsi+112]
        vaesenc	xmm11, xmm11, [rsi+128]
        vpshufd	xmm2, xmm2, 78
        vaesenc	xmm11, xmm11, [rsi+144]
        vpxor	xmm8, xmm8, xmm3
        vpxor	xmm2, xmm2, xmm8
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        cmp	r9d, 11
        jl	L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm11, xmm11, [rsi+176]
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        cmp	r9d, 13
        jl	L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm11, xmm11, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_aesenc_gfmul_sb_last:
        vaesenclast	xmm11, xmm11, xmm0
        vpxor	xmm6, xmm2, xmm1
        vpxor	xmm11, xmm11, xmm12
        vmovdqu	OWORD PTR [r8+rbx], xmm11
        vpshufb	xmm11, xmm11, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm11
        add	ebx, 16
        cmp	ebx, r13d
        jl	L_AES_GCM_encrypt_avx2_last_block_start
L_AES_GCM_encrypt_avx2_last_block_ghash:
        ; ghash_gfmul_red
        vpclmulqdq	xmm10, xmm6, xmm5, 16
        vpclmulqdq	xmm9, xmm6, xmm5, 1
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm10, xmm10, xmm9
        vpslldq	xmm9, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm6, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm9, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm6, xmm6, xmm10
        vpxor	xmm6, xmm6, xmm9
        vpxor	xmm6, xmm6, xmm8
L_AES_GCM_encrypt_avx2_last_block_done:
        mov	ecx, r10d
        mov	edx, r10d
        and	ecx, 15
        jz	L_AES_GCM_encrypt_avx2_done_enc
        ; aesenc_last15_enc
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpxor	xmm4, xmm4, [rsi]
        vaesenc	xmm4, xmm4, [rsi+16]
        vaesenc	xmm4, xmm4, [rsi+32]
        vaesenc	xmm4, xmm4, [rsi+48]
        vaesenc	xmm4, xmm4, [rsi+64]
        vaesenc	xmm4, xmm4, [rsi+80]
        vaesenc	xmm4, xmm4, [rsi+96]
        vaesenc	xmm4, xmm4, [rsi+112]
        vaesenc	xmm4, xmm4, [rsi+128]
        vaesenc	xmm4, xmm4, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm0
        vaesenc	xmm4, xmm4, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm0
        vaesenc	xmm4, xmm4, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_aesenc_avx_last:
        vaesenclast	xmm4, xmm4, xmm0
        xor	ecx, ecx
        vpxor	xmm0, xmm0, xmm0
        vmovdqu	OWORD PTR [rsp], xmm4
        vmovdqu	OWORD PTR [rsp+16], xmm0
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [rsp+rcx+16], r13b
        mov	BYTE PTR [r8+rbx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_loop
L_AES_GCM_encrypt_avx2_aesenc_last15_enc_avx_finish_enc:
        vmovdqu	xmm4, OWORD PTR [rsp+16]
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm4
        ; ghash_gfmul_red
        vpclmulqdq	xmm2, xmm6, xmm5, 16
        vpclmulqdq	xmm1, xmm6, xmm5, 1
        vpclmulqdq	xmm0, xmm6, xmm5, 0
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm6, xmm6, xmm5, 17
        vpclmulqdq	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm6, xmm6, xmm0
L_AES_GCM_encrypt_avx2_done_enc:
        ; calc_tag
        shl	r10, 3
        shl	r11, 3
        vmovq	xmm0, r10
        vmovq	xmm1, r11
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm6
        ; ghash_gfmul_red
        vpclmulqdq	xmm4, xmm0, xmm5, 16
        vpclmulqdq	xmm3, xmm0, xmm5, 1
        vpclmulqdq	xmm2, xmm0, xmm5, 0
        vpxor	xmm4, xmm4, xmm3
        vpslldq	xmm3, xmm4, 8
        vpsrldq	xmm4, xmm4, 8
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm0, xmm0, xmm5, 17
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm0, xmm0, xmm4
        vpxor	xmm0, xmm0, xmm3
        vpxor	xmm0, xmm0, xmm2
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm15
        ; store_tag
        cmp	r14d, 16
        je	L_AES_GCM_encrypt_avx2_store_tag_16
        xor	rcx, rcx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_avx2_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r15+rcx], r13b
        inc	ecx
        cmp	ecx, r14d
        jne	L_AES_GCM_encrypt_avx2_store_tag_loop
        jmp	L_AES_GCM_encrypt_avx2_store_tag_done
L_AES_GCM_encrypt_avx2_store_tag_16:
        vmovdqu	OWORD PTR [r15], xmm0
L_AES_GCM_encrypt_avx2_store_tag_done:
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+160]
        vmovdqu	xmm7, OWORD PTR [rsp+176]
        vmovdqu	xmm8, OWORD PTR [rsp+192]
        vmovdqu	xmm9, OWORD PTR [rsp+208]
        vmovdqu	xmm10, OWORD PTR [rsp+224]
        vmovdqu	xmm11, OWORD PTR [rsp+240]
        vmovdqu	xmm12, OWORD PTR [rsp+256]
        vmovdqu	xmm13, OWORD PTR [rsp+272]
        vmovdqu	xmm14, OWORD PTR [rsp+288]
        vmovdqu	xmm15, OWORD PTR [rsp+304]
        add	rsp, 320
        pop	rsi
        pop	r14
        pop	rbx
        pop	r15
        pop	r12
        pop	rdi
        pop	r13
        ret
AES_GCM_encrypt_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_avx2 PROC
        push	r13
        push	rdi
        push	r12
        push	r14
        push	rbx
        push	r15
        push	rsi
        push	rbp
        mov	rdi, rcx
        mov	r12, r8
        mov	rax, r9
        mov	r14, QWORD PTR [rsp+104]
        mov	r8, rdx
        mov	r10d, DWORD PTR [rsp+112]
        mov	r11d, DWORD PTR [rsp+120]
        mov	ebx, DWORD PTR [rsp+128]
        mov	r15d, DWORD PTR [rsp+136]
        mov	rsi, QWORD PTR [rsp+144]
        mov	r9d, DWORD PTR [rsp+152]
        mov	rbp, QWORD PTR [rsp+160]
        sub	rsp, 328
        vmovdqu	OWORD PTR [rsp+168], xmm6
        vmovdqu	OWORD PTR [rsp+184], xmm7
        vmovdqu	OWORD PTR [rsp+200], xmm8
        vmovdqu	OWORD PTR [rsp+216], xmm9
        vmovdqu	OWORD PTR [rsp+232], xmm10
        vmovdqu	OWORD PTR [rsp+248], xmm11
        vmovdqu	OWORD PTR [rsp+264], xmm12
        vmovdqu	OWORD PTR [rsp+280], xmm13
        vmovdqu	OWORD PTR [rsp+296], xmm14
        vmovdqu	OWORD PTR [rsp+312], xmm15
        vpxor	xmm4, xmm4, xmm4
        vpxor	xmm6, xmm6, xmm6
        mov	edx, ebx
        cmp	edx, 12
        je	L_AES_GCM_decrypt_avx2_iv_12
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqu	xmm5, OWORD PTR [rsi]
        vaesenc	xmm5, xmm5, [rsi+16]
        vaesenc	xmm5, xmm5, [rsi+32]
        vaesenc	xmm5, xmm5, [rsi+48]
        vaesenc	xmm5, xmm5, [rsi+64]
        vaesenc	xmm5, xmm5, [rsi+80]
        vaesenc	xmm5, xmm5, [rsi+96]
        vaesenc	xmm5, xmm5, [rsi+112]
        vaesenc	xmm5, xmm5, [rsi+128]
        vaesenc	xmm5, xmm5, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_decrypt_avx2_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_avx2_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_avx2_calc_iv_16_loop:
        vmovdqu	xmm0, OWORD PTR [rax+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx2_calc_iv_16_loop
        mov	edx, ebx
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_avx2_calc_iv_done
L_AES_GCM_decrypt_avx2_calc_iv_lt16:
        vpxor	xmm0, xmm0, xmm0
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_avx2_calc_iv_loop:
        movzx	r13d, BYTE PTR [rax+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx2_calc_iv_loop
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
L_AES_GCM_decrypt_avx2_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqu	xmm15, OWORD PTR [rsi]
        vpxor	xmm15, xmm15, xmm4
        vaesenc	xmm15, xmm15, [rsi+16]
        vaesenc	xmm15, xmm15, [rsi+32]
        vaesenc	xmm15, xmm15, [rsi+48]
        vaesenc	xmm15, xmm15, [rsi+64]
        vaesenc	xmm15, xmm15, [rsi+80]
        vaesenc	xmm15, xmm15, [rsi+96]
        vaesenc	xmm15, xmm15, [rsi+112]
        vaesenc	xmm15, xmm15, [rsi+128]
        vaesenc	xmm15, xmm15, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm15, xmm15, xmm0
        vaesenc	xmm15, xmm15, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm15, xmm15, xmm0
        vaesenc	xmm15, xmm15, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm15, xmm15, xmm0
        jmp	L_AES_GCM_decrypt_avx2_iv_done
L_AES_GCM_decrypt_avx2_iv_12:
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        vmovdqu	xmm4, OWORD PTR L_avx2_aes_gcm_bswap_one
        vmovdqu	xmm5, OWORD PTR [rsi]
        vpblendd	xmm4, xmm4, [rax], 7
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	xmm7, OWORD PTR [rsi+16]
        vpxor	xmm15, xmm4, xmm5
        vaesenc	xmm5, xmm5, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rsi+32]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+48]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+64]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+80]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+96]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+112]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+128]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+144]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        cmp	r9d, 11
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+176]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        cmp	r9d, 13
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        jl	L_AES_GCM_decrypt_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+208]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm15, xmm15, xmm0
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm0
        vaesenclast	xmm15, xmm15, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
L_AES_GCM_decrypt_avx2_iv_done:
        ; Additional authentication data
        mov	edx, r11d
        cmp	edx, 0
        je	L_AES_GCM_decrypt_avx2_calc_aad_done
        xor	ecx, ecx
        cmp	edx, 16
        jl	L_AES_GCM_decrypt_avx2_calc_aad_lt16
        and	edx, 4294967280
L_AES_GCM_decrypt_avx2_calc_aad_16_loop:
        vmovdqu	xmm0, OWORD PTR [r12+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm6, 16
        vpclmulqdq	xmm1, xmm5, xmm6, 1
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm6, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx2_calc_aad_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_decrypt_avx2_calc_aad_done
L_AES_GCM_decrypt_avx2_calc_aad_lt16:
        vpxor	xmm0, xmm0, xmm0
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_avx2_calc_aad_loop:
        movzx	r13d, BYTE PTR [r12+rcx]
        mov	BYTE PTR [rsp+rbx], r13b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_decrypt_avx2_calc_aad_loop
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm6, 16
        vpclmulqdq	xmm1, xmm5, xmm6, 1
        vpclmulqdq	xmm0, xmm5, xmm6, 0
        vpclmulqdq	xmm3, xmm5, xmm6, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm7, xmm0, xmm1
        vpxor	xmm6, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm7, 31
        vpsrld	xmm1, xmm6, 31
        vpslld	xmm7, xmm7, 1
        vpslld	xmm6, xmm6, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm6, xmm6, xmm2
        vpor	xmm7, xmm7, xmm0
        vpor	xmm6, xmm6, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm7, xmm2, 16
        vpshufd	xmm1, xmm7, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
L_AES_GCM_decrypt_avx2_calc_aad_done:
        ; Calculate counter and H
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm5, xmm5, xmm0
        xor	ebx, ebx
        cmp	r10d, 128
        mov	r13d, r10d
        jl	L_AES_GCM_decrypt_avx2_done_128
        and	r13d, 4294967168
        vmovdqu	OWORD PTR [rsp+128], xmm4
        vmovdqu	OWORD PTR [rsp+144], xmm15
        vmovdqu	xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128
        ; H ^ 1 and H ^ 2
        vpclmulqdq	xmm9, xmm5, xmm5, 0
        vpclmulqdq	xmm10, xmm5, xmm5, 17
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm0, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp], xmm5
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3 and H ^ 4
        vpclmulqdq	xmm11, xmm0, xmm5, 16
        vpclmulqdq	xmm10, xmm0, xmm5, 1
        vpclmulqdq	xmm9, xmm0, xmm5, 0
        vpclmulqdq	xmm12, xmm0, xmm5, 17
        vpclmulqdq	xmm13, xmm0, xmm0, 0
        vpclmulqdq	xmm14, xmm0, xmm0, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm2, xmm13, xmm14
        vpxor	xmm1, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+32], xmm1
        vmovdqu	OWORD PTR [rsp+48], xmm2
        ; H ^ 5 and H ^ 6
        vpclmulqdq	xmm11, xmm1, xmm0, 16
        vpclmulqdq	xmm10, xmm1, xmm0, 1
        vpclmulqdq	xmm9, xmm1, xmm0, 0
        vpclmulqdq	xmm12, xmm1, xmm0, 17
        vpclmulqdq	xmm13, xmm1, xmm1, 0
        vpclmulqdq	xmm14, xmm1, xmm1, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+64], xmm7
        vmovdqu	OWORD PTR [rsp+80], xmm0
        ; H ^ 7 and H ^ 8
        vpclmulqdq	xmm11, xmm2, xmm1, 16
        vpclmulqdq	xmm10, xmm2, xmm1, 1
        vpclmulqdq	xmm9, xmm2, xmm1, 0
        vpclmulqdq	xmm12, xmm2, xmm1, 17
        vpclmulqdq	xmm13, xmm2, xmm2, 0
        vpclmulqdq	xmm14, xmm2, xmm2, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+96], xmm7
        vmovdqu	OWORD PTR [rsp+112], xmm0
L_AES_GCM_decrypt_avx2_ghash_128:
        ; aesenc_128_ghash
        lea	rcx, QWORD PTR [rdi+rbx]
        lea	rdx, QWORD PTR [r8+rbx]
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rsi]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        ; aesenc_pclmul_1
        vmovdqu	xmm1, OWORD PTR [rcx]
        vmovdqu	xmm0, OWORD PTR [rsi+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vmovdqu	xmm2, OWORD PTR [rsp+112]
        vpxor	xmm1, xmm1, xmm6
        vpclmulqdq	xmm5, xmm1, xmm2, 16
        vpclmulqdq	xmm3, xmm1, xmm2, 1
        vpclmulqdq	xmm6, xmm1, xmm2, 0
        vpclmulqdq	xmm7, xmm1, xmm2, 17
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_2
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm0, OWORD PTR [rsp+96]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+32]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+32]
        vmovdqu	xmm0, OWORD PTR [rsp+80]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+48]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vmovdqu	xmm0, OWORD PTR [rsp+64]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+64]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+64]
        vmovdqu	xmm0, OWORD PTR [rsp+48]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+80]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm0, OWORD PTR [rsp+32]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+96]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+96]
        vmovdqu	xmm0, OWORD PTR [rsp+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+112]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rsi+128]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_l
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm6, xmm6, xmm4
        vpxor	xmm5, xmm5, xmm3
        vpslldq	xmm1, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vmovdqu	xmm4, OWORD PTR [rsi+144]
        vmovdqu	xmm0, OWORD PTR L_avx2_aes_gcm_mod2_128
        vaesenc	xmm8, xmm8, xmm4
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm7, xmm7, xmm5
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm9, xmm9, xmm4
        vaesenc	xmm10, xmm10, xmm4
        vaesenc	xmm11, xmm11, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm12, xmm12, xmm4
        vaesenc	xmm13, xmm13, xmm4
        vaesenc	xmm14, xmm14, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm6, xmm6, xmm7
        vaesenc	xmm15, xmm15, xmm4
        cmp	r9d, 11
        vmovdqu	xmm7, OWORD PTR [rsi+160]
        jl	L_AES_GCM_decrypt_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r9d, 13
        vmovdqu	xmm7, OWORD PTR [rsi+192]
        jl	L_AES_GCM_decrypt_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_aesenc_128_ghash_avx_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm2, OWORD PTR [rcx+32]
        vmovdqu	xmm3, OWORD PTR [rcx+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm2, OWORD PTR [rcx+96]
        vmovdqu	xmm3, OWORD PTR [rcx+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        ; aesenc_128_ghash - end
        add	ebx, 128
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_avx2_ghash_128
        vmovdqu	xmm5, OWORD PTR [rsp]
        vmovdqu	xmm4, OWORD PTR [rsp+128]
        vmovdqu	xmm15, OWORD PTR [rsp+144]
L_AES_GCM_decrypt_avx2_done_128:
        cmp	ebx, r10d
        jge	L_AES_GCM_decrypt_avx2_done_dec
        mov	r13d, r10d
        and	r13d, 4294967280
        cmp	ebx, r13d
        jge	L_AES_GCM_decrypt_avx2_last_block_done
L_AES_GCM_decrypt_avx2_last_block_start:
        vmovdqu	xmm11, OWORD PTR [rdi+rbx]
        vpshufb	xmm10, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpshufb	xmm12, xmm11, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm12, xmm12, xmm6
        ; aesenc_gfmul_sb
        vpclmulqdq	xmm2, xmm12, xmm5, 1
        vpclmulqdq	xmm3, xmm12, xmm5, 16
        vpclmulqdq	xmm1, xmm12, xmm5, 0
        vpclmulqdq	xmm8, xmm12, xmm5, 17
        vpxor	xmm10, xmm10, [rsi]
        vaesenc	xmm10, xmm10, [rsi+16]
        vpxor	xmm3, xmm3, xmm2
        vpslldq	xmm2, xmm3, 8
        vpsrldq	xmm3, xmm3, 8
        vaesenc	xmm10, xmm10, [rsi+32]
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm10, xmm10, [rsi+48]
        vaesenc	xmm10, xmm10, [rsi+64]
        vaesenc	xmm10, xmm10, [rsi+80]
        vpshufd	xmm2, xmm2, 78
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm10, xmm10, [rsi+96]
        vaesenc	xmm10, xmm10, [rsi+112]
        vaesenc	xmm10, xmm10, [rsi+128]
        vpshufd	xmm2, xmm2, 78
        vaesenc	xmm10, xmm10, [rsi+144]
        vpxor	xmm8, xmm8, xmm3
        vpxor	xmm2, xmm2, xmm8
        vmovdqu	xmm0, OWORD PTR [rsi+160]
        cmp	r9d, 11
        jl	L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm10, xmm10, [rsi+176]
        vmovdqu	xmm0, OWORD PTR [rsi+192]
        cmp	r9d, 13
        jl	L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm10, xmm10, [rsi+208]
        vmovdqu	xmm0, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_aesenc_gfmul_sb_last:
        vaesenclast	xmm10, xmm10, xmm0
        vpxor	xmm6, xmm2, xmm1
        vpxor	xmm10, xmm10, xmm11
        vmovdqu	OWORD PTR [r8+rbx], xmm10
        add	ebx, 16
        cmp	ebx, r13d
        jl	L_AES_GCM_decrypt_avx2_last_block_start
L_AES_GCM_decrypt_avx2_last_block_done:
        mov	ecx, r10d
        mov	edx, r10d
        and	ecx, 15
        jz	L_AES_GCM_decrypt_avx2_done_dec
        ; aesenc_last15_dec
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpxor	xmm4, xmm4, [rsi]
        vaesenc	xmm4, xmm4, [rsi+16]
        vaesenc	xmm4, xmm4, [rsi+32]
        vaesenc	xmm4, xmm4, [rsi+48]
        vaesenc	xmm4, xmm4, [rsi+64]
        vaesenc	xmm4, xmm4, [rsi+80]
        vaesenc	xmm4, xmm4, [rsi+96]
        vaesenc	xmm4, xmm4, [rsi+112]
        vaesenc	xmm4, xmm4, [rsi+128]
        vaesenc	xmm4, xmm4, [rsi+144]
        cmp	r9d, 11
        vmovdqu	xmm1, OWORD PTR [rsi+160]
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm1
        vaesenc	xmm4, xmm4, [rsi+176]
        cmp	r9d, 13
        vmovdqu	xmm1, OWORD PTR [rsi+192]
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last
        vaesenc	xmm4, xmm4, xmm1
        vaesenc	xmm4, xmm4, [rsi+208]
        vmovdqu	xmm1, OWORD PTR [rsi+224]
L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_aesenc_avx_last:
        vaesenclast	xmm4, xmm4, xmm1
        xor	ecx, ecx
        vpxor	xmm0, xmm0, xmm0
        vmovdqu	OWORD PTR [rsp], xmm4
        vmovdqu	OWORD PTR [rsp+16], xmm0
L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_loop:
        movzx	r13d, BYTE PTR [rdi+rbx]
        mov	BYTE PTR [rsp+rcx+16], r13b
        xor	r13b, BYTE PTR [rsp+rcx]
        mov	BYTE PTR [r8+rbx], r13b
        inc	ebx
        inc	ecx
        cmp	ebx, edx
        jl	L_AES_GCM_decrypt_avx2_aesenc_last15_dec_avx_loop
        vmovdqu	xmm4, OWORD PTR [rsp+16]
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm4
        ; ghash_gfmul_red
        vpclmulqdq	xmm2, xmm6, xmm5, 16
        vpclmulqdq	xmm1, xmm6, xmm5, 1
        vpclmulqdq	xmm0, xmm6, xmm5, 0
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm6, xmm6, xmm5, 17
        vpclmulqdq	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm6, xmm6, xmm2
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm6, xmm6, xmm0
L_AES_GCM_decrypt_avx2_done_dec:
        ; calc_tag
        shl	r10, 3
        shl	r11, 3
        vmovq	xmm0, r10
        vmovq	xmm1, r11
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm6
        ; ghash_gfmul_red
        vpclmulqdq	xmm4, xmm0, xmm5, 16
        vpclmulqdq	xmm3, xmm0, xmm5, 1
        vpclmulqdq	xmm2, xmm0, xmm5, 0
        vpxor	xmm4, xmm4, xmm3
        vpslldq	xmm3, xmm4, 8
        vpsrldq	xmm4, xmm4, 8
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm0, xmm0, xmm5, 17
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm0, xmm0, xmm4
        vpxor	xmm0, xmm0, xmm3
        vpxor	xmm0, xmm0, xmm2
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm15
        ; cmp_tag
        cmp	r15d, 16
        je	L_AES_GCM_decrypt_avx2_cmp_tag_16
        xor	rdx, rdx
        xor	rax, rax
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_avx2_cmp_tag_loop:
        movzx	r13d, BYTE PTR [rsp+rdx]
        xor	r13b, BYTE PTR [r14+rdx]
        or	al, r13b
        inc	edx
        cmp	edx, r15d
        jne	L_AES_GCM_decrypt_avx2_cmp_tag_loop
        cmp	rax, 0
        sete	al
        jmp	L_AES_GCM_decrypt_avx2_cmp_tag_done
L_AES_GCM_decrypt_avx2_cmp_tag_16:
        vmovdqu	xmm1, OWORD PTR [r14]
        vpcmpeqb	xmm0, xmm0, xmm1
        vpmovmskb	rdx, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	eax, eax
        cmp	edx, 65535
        sete	al
L_AES_GCM_decrypt_avx2_cmp_tag_done:
        mov	DWORD PTR [rbp], eax
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+168]
        vmovdqu	xmm7, OWORD PTR [rsp+184]
        vmovdqu	xmm8, OWORD PTR [rsp+200]
        vmovdqu	xmm9, OWORD PTR [rsp+216]
        vmovdqu	xmm10, OWORD PTR [rsp+232]
        vmovdqu	xmm11, OWORD PTR [rsp+248]
        vmovdqu	xmm12, OWORD PTR [rsp+264]
        vmovdqu	xmm13, OWORD PTR [rsp+280]
        vmovdqu	xmm14, OWORD PTR [rsp+296]
        vmovdqu	xmm15, OWORD PTR [rsp+312]
        add	rsp, 328
        pop	rbp
        pop	rsi
        pop	r15
        pop	rbx
        pop	r14
        pop	r12
        pop	rdi
        pop	r13
        ret
AES_GCM_decrypt_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_init_avx2 PROC
        push	rbx
        push	rdi
        push	rsi
        push	r12
        mov	rdi, rcx
        mov	rsi, rdx
        mov	r10, r8
        mov	r11d, r9d
        mov	rax, QWORD PTR [rsp+72]
        mov	r8, QWORD PTR [rsp+80]
        mov	r9, QWORD PTR [rsp+88]
        sub	rsp, 48
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vpxor	xmm4, xmm4, xmm4
        mov	edx, r11d
        cmp	edx, 12
        je	L_AES_GCM_init_avx2_iv_12
        ; Calculate values when IV is not 12 bytes
        ; H = Encrypt X(=0)
        vmovdqu	xmm5, OWORD PTR [rdi]
        vaesenc	xmm5, xmm5, [rdi+16]
        vaesenc	xmm5, xmm5, [rdi+32]
        vaesenc	xmm5, xmm5, [rdi+48]
        vaesenc	xmm5, xmm5, [rdi+64]
        vaesenc	xmm5, xmm5, [rdi+80]
        vaesenc	xmm5, xmm5, [rdi+96]
        vaesenc	xmm5, xmm5, [rdi+112]
        vaesenc	xmm5, xmm5, [rdi+128]
        vaesenc	xmm5, xmm5, [rdi+144]
        cmp	esi, 11
        vmovdqu	xmm0, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rdi+176]
        cmp	esi, 13
        vmovdqu	xmm0, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm5, xmm5, [rdi+208]
        vmovdqu	xmm0, OWORD PTR [rdi+224]
L_AES_GCM_init_avx2_calc_iv_1_aesenc_avx_last:
        vaesenclast	xmm5, xmm5, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ; Calc counter
        ; Initialization vector
        cmp	edx, 0
        mov	rcx, 0
        je	L_AES_GCM_init_avx2_calc_iv_done
        cmp	edx, 16
        jl	L_AES_GCM_init_avx2_calc_iv_lt16
        and	edx, 4294967280
L_AES_GCM_init_avx2_calc_iv_16_loop:
        vmovdqu	xmm0, OWORD PTR [r10+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm6, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm6, xmm2, 16
        vpshufd	xmm1, xmm6, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_init_avx2_calc_iv_16_loop
        mov	edx, r11d
        cmp	ecx, edx
        je	L_AES_GCM_init_avx2_calc_iv_done
L_AES_GCM_init_avx2_calc_iv_lt16:
        vpxor	xmm0, xmm0, xmm0
        xor	ebx, ebx
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_init_avx2_calc_iv_loop:
        movzx	r12d, BYTE PTR [r10+rcx]
        mov	BYTE PTR [rsp+rbx], r12b
        inc	ecx
        inc	ebx
        cmp	ecx, edx
        jl	L_AES_GCM_init_avx2_calc_iv_loop
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm6, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm6, xmm2, 16
        vpshufd	xmm1, xmm6, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
L_AES_GCM_init_avx2_calc_iv_done:
        ; T = Encrypt counter
        vpxor	xmm0, xmm0, xmm0
        shl	edx, 3
        vmovq	xmm0, rdx
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm6, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm6, xmm2, 16
        vpshufd	xmm1, xmm6, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        ;   Encrypt counter
        vmovdqu	xmm7, OWORD PTR [rdi]
        vpxor	xmm7, xmm7, xmm4
        vaesenc	xmm7, xmm7, [rdi+16]
        vaesenc	xmm7, xmm7, [rdi+32]
        vaesenc	xmm7, xmm7, [rdi+48]
        vaesenc	xmm7, xmm7, [rdi+64]
        vaesenc	xmm7, xmm7, [rdi+80]
        vaesenc	xmm7, xmm7, [rdi+96]
        vaesenc	xmm7, xmm7, [rdi+112]
        vaesenc	xmm7, xmm7, [rdi+128]
        vaesenc	xmm7, xmm7, [rdi+144]
        cmp	esi, 11
        vmovdqu	xmm0, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm7, xmm7, xmm0
        vaesenc	xmm7, xmm7, [rdi+176]
        cmp	esi, 13
        vmovdqu	xmm0, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last
        vaesenc	xmm7, xmm7, xmm0
        vaesenc	xmm7, xmm7, [rdi+208]
        vmovdqu	xmm0, OWORD PTR [rdi+224]
L_AES_GCM_init_avx2_calc_iv_2_aesenc_avx_last:
        vaesenclast	xmm7, xmm7, xmm0
        jmp	L_AES_GCM_init_avx2_iv_done
L_AES_GCM_init_avx2_iv_12:
        ; # Calculate values when IV is 12 bytes
        ; Set counter based on IV
        vmovdqu	xmm4, OWORD PTR L_avx2_aes_gcm_bswap_one
        vmovdqu	xmm5, OWORD PTR [rdi]
        vpblendd	xmm4, xmm4, [r10], 7
        ; H = Encrypt X(=0) and T = Encrypt counter
        vmovdqu	xmm6, OWORD PTR [rdi+16]
        vpxor	xmm7, xmm4, xmm5
        vaesenc	xmm5, xmm5, xmm6
        vaesenc	xmm7, xmm7, xmm6
        vmovdqu	xmm0, OWORD PTR [rdi+32]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+48]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+64]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+80]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+96]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+112]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+128]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+144]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        cmp	esi, 11
        vmovdqu	xmm0, OWORD PTR [rdi+160]
        jl	L_AES_GCM_init_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+176]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        cmp	esi, 13
        vmovdqu	xmm0, OWORD PTR [rdi+192]
        jl	L_AES_GCM_init_avx2_calc_iv_12_last
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+208]
        vaesenc	xmm5, xmm5, xmm0
        vaesenc	xmm7, xmm7, xmm0
        vmovdqu	xmm0, OWORD PTR [rdi+224]
L_AES_GCM_init_avx2_calc_iv_12_last:
        vaesenclast	xmm5, xmm5, xmm0
        vaesenclast	xmm7, xmm7, xmm0
        vpshufb	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_bswap_mask
L_AES_GCM_init_avx2_iv_done:
        vmovdqu	OWORD PTR [r9], xmm7
        vpshufb	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        vmovdqu	OWORD PTR [rax], xmm5
        vmovdqu	OWORD PTR [r8], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        add	rsp, 48
        pop	r12
        pop	rsi
        pop	rdi
        pop	rbx
        ret
AES_GCM_init_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_aad_update_avx2 PROC
        mov	rax, rcx
        sub	rsp, 16
        vmovdqu	OWORD PTR [rsp], xmm6
        vmovdqu	xmm4, OWORD PTR [r8]
        vmovdqu	xmm5, OWORD PTR [r9]
        xor	ecx, ecx
L_AES_GCM_aad_update_avx2_16_loop:
        vmovdqu	xmm0, OWORD PTR [rax+rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm6, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm6, xmm2, 16
        vpshufd	xmm1, xmm6, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        add	ecx, 16
        cmp	ecx, edx
        jl	L_AES_GCM_aad_update_avx2_16_loop
        vmovdqu	OWORD PTR [r8], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp]
        add	rsp, 16
        ret
AES_GCM_aad_update_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_block_avx2 PROC
        mov	r10, r8
        mov	r11, r9
        mov	rax, QWORD PTR [rsp+40]
        sub	rsp, 152
        vmovdqu	xmm3, OWORD PTR [rax]
        ; aesenc_block
        vmovdqu	xmm1, xmm3
        vpshufb	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm0, xmm0, [rcx]
        vmovdqu	xmm2, OWORD PTR [rcx+16]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+32]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+48]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+64]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+80]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+96]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+112]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+128]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rcx+144]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm3, xmm1
        cmp	edx, 11
        vmovdqu	xmm1, OWORD PTR [rcx+160]
        jl	L_AES_GCM_encrypt_block_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rcx+176]
        vaesenc	xmm0, xmm0, xmm2
        cmp	edx, 13
        vmovdqu	xmm1, OWORD PTR [rcx+192]
        jl	L_AES_GCM_encrypt_block_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rcx+208]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm1, OWORD PTR [rcx+224]
L_AES_GCM_encrypt_block_avx2_aesenc_block_last:
        vaesenclast	xmm0, xmm0, xmm1
        vmovdqu	xmm1, OWORD PTR [r11]
        vpxor	xmm0, xmm0, xmm1
        vmovdqu	OWORD PTR [r10], xmm0
        vmovdqu	OWORD PTR [rax], xmm3
        vzeroupper
        add	rsp, 152
        ret
AES_GCM_encrypt_block_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_ghash_block_avx2 PROC
        sub	rsp, 16
        vmovdqu	OWORD PTR [rsp], xmm6
        vmovdqu	xmm4, OWORD PTR [rdx]
        vmovdqu	xmm5, OWORD PTR [r8]
        vmovdqu	xmm0, OWORD PTR [rcx]
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm4, xmm4, xmm0
        ; ghash_gfmul_avx
        vpclmulqdq	xmm2, xmm5, xmm4, 16
        vpclmulqdq	xmm1, xmm5, xmm4, 1
        vpclmulqdq	xmm0, xmm5, xmm4, 0
        vpclmulqdq	xmm3, xmm5, xmm4, 17
        vpxor	xmm2, xmm2, xmm1
        vpslldq	xmm1, xmm2, 8
        vpsrldq	xmm2, xmm2, 8
        vpxor	xmm6, xmm0, xmm1
        vpxor	xmm4, xmm3, xmm2
        ; ghash_mid
        vpsrld	xmm0, xmm6, 31
        vpsrld	xmm1, xmm4, 31
        vpslld	xmm6, xmm6, 1
        vpslld	xmm4, xmm4, 1
        vpsrldq	xmm2, xmm0, 12
        vpslldq	xmm0, xmm0, 4
        vpslldq	xmm1, xmm1, 4
        vpor	xmm4, xmm4, xmm2
        vpor	xmm6, xmm6, xmm0
        vpor	xmm4, xmm4, xmm1
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm6, xmm2, 16
        vpshufd	xmm1, xmm6, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm4, xmm4, xmm1
        vmovdqu	OWORD PTR [rdx], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp]
        add	rsp, 16
        ret
AES_GCM_ghash_block_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_update_avx2 PROC
        push	r12
        push	r13
        push	r14
        push	r15
        push	rdi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r13, QWORD PTR [rsp+96]
        mov	r14, QWORD PTR [rsp+104]
        sub	rsp, 312
        vmovdqu	OWORD PTR [rsp+152], xmm6
        vmovdqu	OWORD PTR [rsp+168], xmm7
        vmovdqu	OWORD PTR [rsp+184], xmm8
        vmovdqu	OWORD PTR [rsp+200], xmm9
        vmovdqu	OWORD PTR [rsp+216], xmm10
        vmovdqu	OWORD PTR [rsp+232], xmm11
        vmovdqu	OWORD PTR [rsp+248], xmm12
        vmovdqu	OWORD PTR [rsp+264], xmm13
        vmovdqu	OWORD PTR [rsp+280], xmm14
        vmovdqu	OWORD PTR [rsp+296], xmm15
        vmovdqu	xmm6, OWORD PTR [r12]
        vmovdqu	xmm5, OWORD PTR [r13]
        vmovdqu	xmm4, OWORD PTR [r14]
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm0
        xor	edi, edi
        cmp	r9d, 128
        mov	r15d, r9d
        jl	L_AES_GCM_encrypt_update_avx2_done_128
        and	r15d, 4294967168
        vmovdqu	OWORD PTR [rsp+128], xmm4
        vmovdqu	xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128
        ; H ^ 1 and H ^ 2
        vpclmulqdq	xmm9, xmm5, xmm5, 0
        vpclmulqdq	xmm10, xmm5, xmm5, 17
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm0, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp], xmm5
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3 and H ^ 4
        vpclmulqdq	xmm11, xmm0, xmm5, 16
        vpclmulqdq	xmm10, xmm0, xmm5, 1
        vpclmulqdq	xmm9, xmm0, xmm5, 0
        vpclmulqdq	xmm12, xmm0, xmm5, 17
        vpclmulqdq	xmm13, xmm0, xmm0, 0
        vpclmulqdq	xmm14, xmm0, xmm0, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm2, xmm13, xmm14
        vpxor	xmm1, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+32], xmm1
        vmovdqu	OWORD PTR [rsp+48], xmm2
        ; H ^ 5 and H ^ 6
        vpclmulqdq	xmm11, xmm1, xmm0, 16
        vpclmulqdq	xmm10, xmm1, xmm0, 1
        vpclmulqdq	xmm9, xmm1, xmm0, 0
        vpclmulqdq	xmm12, xmm1, xmm0, 17
        vpclmulqdq	xmm13, xmm1, xmm1, 0
        vpclmulqdq	xmm14, xmm1, xmm1, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+64], xmm7
        vmovdqu	OWORD PTR [rsp+80], xmm0
        ; H ^ 7 and H ^ 8
        vpclmulqdq	xmm11, xmm2, xmm1, 16
        vpclmulqdq	xmm10, xmm2, xmm1, 1
        vpclmulqdq	xmm9, xmm2, xmm1, 0
        vpclmulqdq	xmm12, xmm2, xmm1, 17
        vpclmulqdq	xmm13, xmm2, xmm2, 0
        vpclmulqdq	xmm14, xmm2, xmm2, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+96], xmm7
        vmovdqu	OWORD PTR [rsp+112], xmm0
        ; First 128 bytes of input
        ; aesenc_128
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+16]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+32]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+48]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+64]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+80]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+96]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+112]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+128]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+144]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 11
        vmovdqu	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqu	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_128_enc_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx2_aesenc_128_enc_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [r11]
        vmovdqu	xmm1, OWORD PTR [r11+16]
        vmovdqu	xmm2, OWORD PTR [r11+32]
        vmovdqu	xmm3, OWORD PTR [r11+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [r10], xmm8
        vmovdqu	OWORD PTR [r10+16], xmm9
        vmovdqu	OWORD PTR [r10+32], xmm10
        vmovdqu	OWORD PTR [r10+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [r11+64]
        vmovdqu	xmm1, OWORD PTR [r11+80]
        vmovdqu	xmm2, OWORD PTR [r11+96]
        vmovdqu	xmm3, OWORD PTR [r11+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [r10+64], xmm12
        vmovdqu	OWORD PTR [r10+80], xmm13
        vmovdqu	OWORD PTR [r10+96], xmm14
        vmovdqu	OWORD PTR [r10+112], xmm15
        cmp	r15d, 128
        mov	edi, 128
        jle	L_AES_GCM_encrypt_update_avx2_end_128
        ; More 128 bytes of input
L_AES_GCM_encrypt_update_avx2_ghash_128:
        ; aesenc_128_ghash
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        ; aesenc_pclmul_1
        vmovdqu	xmm1, OWORD PTR [rdx+-128]
        vmovdqu	xmm0, OWORD PTR [rax+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vmovdqu	xmm2, OWORD PTR [rsp+112]
        vpxor	xmm1, xmm1, xmm6
        vpclmulqdq	xmm5, xmm1, xmm2, 16
        vpclmulqdq	xmm3, xmm1, xmm2, 1
        vpclmulqdq	xmm6, xmm1, xmm2, 0
        vpclmulqdq	xmm7, xmm1, xmm2, 17
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_2
        vmovdqu	xmm1, OWORD PTR [rdx+-112]
        vmovdqu	xmm0, OWORD PTR [rsp+96]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+32]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-96]
        vmovdqu	xmm0, OWORD PTR [rsp+80]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+48]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-80]
        vmovdqu	xmm0, OWORD PTR [rsp+64]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+64]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-64]
        vmovdqu	xmm0, OWORD PTR [rsp+48]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+80]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-48]
        vmovdqu	xmm0, OWORD PTR [rsp+32]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+96]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-32]
        vmovdqu	xmm0, OWORD PTR [rsp+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+112]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rdx+-16]
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+128]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_l
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm6, xmm6, xmm4
        vpxor	xmm5, xmm5, xmm3
        vpslldq	xmm1, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vmovdqu	xmm4, OWORD PTR [rax+144]
        vmovdqu	xmm0, OWORD PTR L_avx2_aes_gcm_mod2_128
        vaesenc	xmm8, xmm8, xmm4
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm7, xmm7, xmm5
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm9, xmm9, xmm4
        vaesenc	xmm10, xmm10, xmm4
        vaesenc	xmm11, xmm11, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm12, xmm12, xmm4
        vaesenc	xmm13, xmm13, xmm4
        vaesenc	xmm14, xmm14, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm6, xmm6, xmm7
        vaesenc	xmm15, xmm15, xmm4
        cmp	r8d, 11
        vmovdqu	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqu	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx2_aesenc_128_ghash_avx_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm2, OWORD PTR [rcx+32]
        vmovdqu	xmm3, OWORD PTR [rcx+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm2, OWORD PTR [rcx+96]
        vmovdqu	xmm3, OWORD PTR [rcx+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        ; aesenc_128_ghash - end
        add	edi, 128
        cmp	edi, r15d
        jl	L_AES_GCM_encrypt_update_avx2_ghash_128
L_AES_GCM_encrypt_update_avx2_end_128:
        vmovdqu	xmm4, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpshufb	xmm8, xmm8, xmm4
        vpshufb	xmm9, xmm9, xmm4
        vpshufb	xmm10, xmm10, xmm4
        vpshufb	xmm11, xmm11, xmm4
        vpshufb	xmm12, xmm12, xmm4
        vpshufb	xmm13, xmm13, xmm4
        vpshufb	xmm14, xmm14, xmm4
        vpshufb	xmm15, xmm15, xmm4
        vpxor	xmm8, xmm8, xmm6
        vmovdqu	xmm7, OWORD PTR [rsp]
        vpclmulqdq	xmm5, xmm7, xmm15, 16
        vpclmulqdq	xmm1, xmm7, xmm15, 1
        vpclmulqdq	xmm4, xmm7, xmm15, 0
        vpclmulqdq	xmm6, xmm7, xmm15, 17
        vpxor	xmm5, xmm5, xmm1
        vmovdqu	xmm7, OWORD PTR [rsp+16]
        vpclmulqdq	xmm2, xmm7, xmm14, 16
        vpclmulqdq	xmm1, xmm7, xmm14, 1
        vpclmulqdq	xmm0, xmm7, xmm14, 0
        vpclmulqdq	xmm3, xmm7, xmm14, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+32]
        vmovdqu	xmm7, OWORD PTR [rsp+48]
        vpclmulqdq	xmm2, xmm15, xmm13, 16
        vpclmulqdq	xmm1, xmm15, xmm13, 1
        vpclmulqdq	xmm0, xmm15, xmm13, 0
        vpclmulqdq	xmm3, xmm15, xmm13, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm12, 16
        vpclmulqdq	xmm1, xmm7, xmm12, 1
        vpclmulqdq	xmm0, xmm7, xmm12, 0
        vpclmulqdq	xmm3, xmm7, xmm12, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+64]
        vmovdqu	xmm7, OWORD PTR [rsp+80]
        vpclmulqdq	xmm2, xmm15, xmm11, 16
        vpclmulqdq	xmm1, xmm15, xmm11, 1
        vpclmulqdq	xmm0, xmm15, xmm11, 0
        vpclmulqdq	xmm3, xmm15, xmm11, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm10, 16
        vpclmulqdq	xmm1, xmm7, xmm10, 1
        vpclmulqdq	xmm0, xmm7, xmm10, 0
        vpclmulqdq	xmm3, xmm7, xmm10, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vmovdqu	xmm15, OWORD PTR [rsp+96]
        vmovdqu	xmm7, OWORD PTR [rsp+112]
        vpclmulqdq	xmm2, xmm15, xmm9, 16
        vpclmulqdq	xmm1, xmm15, xmm9, 1
        vpclmulqdq	xmm0, xmm15, xmm9, 0
        vpclmulqdq	xmm3, xmm15, xmm9, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpclmulqdq	xmm2, xmm7, xmm8, 16
        vpclmulqdq	xmm1, xmm7, xmm8, 1
        vpclmulqdq	xmm0, xmm7, xmm8, 0
        vpclmulqdq	xmm3, xmm7, xmm8, 17
        vpxor	xmm2, xmm2, xmm1
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm4, xmm4, xmm0
        vpslldq	xmm7, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vpxor	xmm4, xmm4, xmm7
        vpxor	xmm6, xmm6, xmm5
        ; ghash_red
        vmovdqu	xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpclmulqdq	xmm0, xmm4, xmm2, 16
        vpshufd	xmm1, xmm4, 78
        vpxor	xmm1, xmm1, xmm0
        vpclmulqdq	xmm0, xmm1, xmm2, 16
        vpshufd	xmm1, xmm1, 78
        vpxor	xmm1, xmm1, xmm0
        vpxor	xmm6, xmm6, xmm1
        vmovdqu	xmm5, OWORD PTR [rsp]
        vmovdqu	xmm4, OWORD PTR [rsp+128]
L_AES_GCM_encrypt_update_avx2_done_128:
        cmp	edi, r9d
        je	L_AES_GCM_encrypt_update_avx2_done_enc
        mov	r15d, r9d
        and	r15d, 4294967280
        cmp	edi, r15d
        jge	L_AES_GCM_encrypt_update_avx2_last_block_done
        ; aesenc_block
        vmovdqu	xmm1, xmm4
        vpshufb	xmm0, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm0, xmm0, [rax]
        vmovdqu	xmm2, OWORD PTR [rax+16]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+32]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+48]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+64]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+80]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+96]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+112]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+128]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm2, OWORD PTR [rax+144]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm4, xmm1
        cmp	r8d, 11
        vmovdqu	xmm1, OWORD PTR [rax+160]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rax+176]
        vaesenc	xmm0, xmm0, xmm2
        cmp	r8d, 13
        vmovdqu	xmm1, OWORD PTR [rax+192]
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_block_last
        vaesenc	xmm0, xmm0, xmm1
        vmovdqu	xmm2, OWORD PTR [rax+208]
        vaesenc	xmm0, xmm0, xmm2
        vmovdqu	xmm1, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx2_aesenc_block_last:
        vaesenclast	xmm0, xmm0, xmm1
        vmovdqu	xmm1, OWORD PTR [r11+rdi]
        vpxor	xmm0, xmm0, xmm1
        vmovdqu	OWORD PTR [r10+rdi], xmm0
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm0
        add	edi, 16
        cmp	edi, r15d
        jge	L_AES_GCM_encrypt_update_avx2_last_block_ghash
L_AES_GCM_encrypt_update_avx2_last_block_start:
        vmovdqu	xmm12, OWORD PTR [r11+rdi]
        vpshufb	xmm11, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        ; aesenc_gfmul_sb
        vpclmulqdq	xmm2, xmm6, xmm5, 1
        vpclmulqdq	xmm3, xmm6, xmm5, 16
        vpclmulqdq	xmm1, xmm6, xmm5, 0
        vpclmulqdq	xmm8, xmm6, xmm5, 17
        vpxor	xmm11, xmm11, [rax]
        vaesenc	xmm11, xmm11, [rax+16]
        vpxor	xmm3, xmm3, xmm2
        vpslldq	xmm2, xmm3, 8
        vpsrldq	xmm3, xmm3, 8
        vaesenc	xmm11, xmm11, [rax+32]
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm11, xmm11, [rax+48]
        vaesenc	xmm11, xmm11, [rax+64]
        vaesenc	xmm11, xmm11, [rax+80]
        vpshufd	xmm2, xmm2, 78
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm11, xmm11, [rax+96]
        vaesenc	xmm11, xmm11, [rax+112]
        vaesenc	xmm11, xmm11, [rax+128]
        vpshufd	xmm2, xmm2, 78
        vaesenc	xmm11, xmm11, [rax+144]
        vpxor	xmm8, xmm8, xmm3
        vpxor	xmm2, xmm2, xmm8
        vmovdqu	xmm0, OWORD PTR [rax+160]
        cmp	r8d, 11
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm11, xmm11, [rax+176]
        vmovdqu	xmm0, OWORD PTR [rax+192]
        cmp	r8d, 13
        jl	L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm11, xmm11, [rax+208]
        vmovdqu	xmm0, OWORD PTR [rax+224]
L_AES_GCM_encrypt_update_avx2_aesenc_gfmul_sb_last:
        vaesenclast	xmm11, xmm11, xmm0
        vpxor	xmm6, xmm2, xmm1
        vpxor	xmm11, xmm11, xmm12
        vmovdqu	OWORD PTR [r10+rdi], xmm11
        vpshufb	xmm11, xmm11, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm6, xmm6, xmm11
        add	edi, 16
        cmp	edi, r15d
        jl	L_AES_GCM_encrypt_update_avx2_last_block_start
L_AES_GCM_encrypt_update_avx2_last_block_ghash:
        ; ghash_gfmul_red
        vpclmulqdq	xmm10, xmm6, xmm5, 16
        vpclmulqdq	xmm9, xmm6, xmm5, 1
        vpclmulqdq	xmm8, xmm6, xmm5, 0
        vpxor	xmm10, xmm10, xmm9
        vpslldq	xmm9, xmm10, 8
        vpsrldq	xmm10, xmm10, 8
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm6, xmm6, xmm5, 17
        vpclmulqdq	xmm8, xmm9, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm6, xmm6, xmm10
        vpxor	xmm6, xmm6, xmm9
        vpxor	xmm6, xmm6, xmm8
L_AES_GCM_encrypt_update_avx2_last_block_done:
L_AES_GCM_encrypt_update_avx2_done_enc:
        vmovdqu	OWORD PTR [r12], xmm6
        vmovdqu	OWORD PTR [r14], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+152]
        vmovdqu	xmm7, OWORD PTR [rsp+168]
        vmovdqu	xmm8, OWORD PTR [rsp+184]
        vmovdqu	xmm9, OWORD PTR [rsp+200]
        vmovdqu	xmm10, OWORD PTR [rsp+216]
        vmovdqu	xmm11, OWORD PTR [rsp+232]
        vmovdqu	xmm12, OWORD PTR [rsp+248]
        vmovdqu	xmm13, OWORD PTR [rsp+264]
        vmovdqu	xmm14, OWORD PTR [rsp+280]
        vmovdqu	xmm15, OWORD PTR [rsp+296]
        add	rsp, 312
        pop	rdi
        pop	r15
        pop	r14
        pop	r13
        pop	r12
        ret
AES_GCM_encrypt_update_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_encrypt_final_avx2 PROC
        push	r12
        push	r13
        mov	eax, DWORD PTR [rsp+56]
        mov	r10, QWORD PTR [rsp+64]
        mov	r11, QWORD PTR [rsp+72]
        sub	rsp, 48
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vmovdqu	xmm4, OWORD PTR [rcx]
        vmovdqu	xmm5, OWORD PTR [r10]
        vmovdqu	xmm6, OWORD PTR [r11]
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm0
        ; calc_tag
        shl	r9, 3
        shl	rax, 3
        vmovq	xmm0, r9
        vmovq	xmm1, rax
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm4
        ; ghash_gfmul_red
        vpclmulqdq	xmm7, xmm0, xmm5, 16
        vpclmulqdq	xmm3, xmm0, xmm5, 1
        vpclmulqdq	xmm2, xmm0, xmm5, 0
        vpxor	xmm7, xmm7, xmm3
        vpslldq	xmm3, xmm7, 8
        vpsrldq	xmm7, xmm7, 8
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm0, xmm0, xmm5, 17
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm0, xmm0, xmm7
        vpxor	xmm0, xmm0, xmm3
        vpxor	xmm0, xmm0, xmm2
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm6
        ; store_tag
        cmp	r8d, 16
        je	L_AES_GCM_encrypt_final_avx2_store_tag_16
        xor	r12, r12
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_encrypt_final_avx2_store_tag_loop:
        movzx	r13d, BYTE PTR [rsp+r12]
        mov	BYTE PTR [rdx+r12], r13b
        inc	r12d
        cmp	r12d, r8d
        jne	L_AES_GCM_encrypt_final_avx2_store_tag_loop
        jmp	L_AES_GCM_encrypt_final_avx2_store_tag_done
L_AES_GCM_encrypt_final_avx2_store_tag_16:
        vmovdqu	OWORD PTR [rdx], xmm0
L_AES_GCM_encrypt_final_avx2_store_tag_done:
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        add	rsp, 48
        pop	r13
        pop	r12
        ret
AES_GCM_encrypt_final_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_update_avx2 PROC
        push	r13
        push	r12
        push	r14
        push	r15
        push	rdi
        mov	rax, rcx
        mov	r10, r8
        mov	r8d, edx
        mov	r11, r9
        mov	r9d, DWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        mov	r14, QWORD PTR [rsp+96]
        mov	r15, QWORD PTR [rsp+104]
        sub	rsp, 328
        vmovdqu	OWORD PTR [rsp+168], xmm6
        vmovdqu	OWORD PTR [rsp+184], xmm7
        vmovdqu	OWORD PTR [rsp+200], xmm8
        vmovdqu	OWORD PTR [rsp+216], xmm9
        vmovdqu	OWORD PTR [rsp+232], xmm10
        vmovdqu	OWORD PTR [rsp+248], xmm11
        vmovdqu	OWORD PTR [rsp+264], xmm12
        vmovdqu	OWORD PTR [rsp+280], xmm13
        vmovdqu	OWORD PTR [rsp+296], xmm14
        vmovdqu	OWORD PTR [rsp+312], xmm15
        vmovdqu	xmm6, OWORD PTR [r12]
        vmovdqu	xmm5, OWORD PTR [r14]
        vmovdqu	xmm4, OWORD PTR [r15]
        ; Calculate H
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm0
        xor	edi, edi
        cmp	r9d, 128
        mov	r13d, r9d
        jl	L_AES_GCM_decrypt_update_avx2_done_128
        and	r13d, 4294967168
        vmovdqu	OWORD PTR [rsp+128], xmm4
        vmovdqu	OWORD PTR [rsp+144], xmm15
        vmovdqu	xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128
        ; H ^ 1 and H ^ 2
        vpclmulqdq	xmm9, xmm5, xmm5, 0
        vpclmulqdq	xmm10, xmm5, xmm5, 17
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpclmulqdq	xmm8, xmm9, xmm3, 16
        vpshufd	xmm9, xmm9, 78
        vpxor	xmm9, xmm9, xmm8
        vpxor	xmm0, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp], xmm5
        vmovdqu	OWORD PTR [rsp+16], xmm0
        ; H ^ 3 and H ^ 4
        vpclmulqdq	xmm11, xmm0, xmm5, 16
        vpclmulqdq	xmm10, xmm0, xmm5, 1
        vpclmulqdq	xmm9, xmm0, xmm5, 0
        vpclmulqdq	xmm12, xmm0, xmm5, 17
        vpclmulqdq	xmm13, xmm0, xmm0, 0
        vpclmulqdq	xmm14, xmm0, xmm0, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm2, xmm13, xmm14
        vpxor	xmm1, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+32], xmm1
        vmovdqu	OWORD PTR [rsp+48], xmm2
        ; H ^ 5 and H ^ 6
        vpclmulqdq	xmm11, xmm1, xmm0, 16
        vpclmulqdq	xmm10, xmm1, xmm0, 1
        vpclmulqdq	xmm9, xmm1, xmm0, 0
        vpclmulqdq	xmm12, xmm1, xmm0, 17
        vpclmulqdq	xmm13, xmm1, xmm1, 0
        vpclmulqdq	xmm14, xmm1, xmm1, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+64], xmm7
        vmovdqu	OWORD PTR [rsp+80], xmm0
        ; H ^ 7 and H ^ 8
        vpclmulqdq	xmm11, xmm2, xmm1, 16
        vpclmulqdq	xmm10, xmm2, xmm1, 1
        vpclmulqdq	xmm9, xmm2, xmm1, 0
        vpclmulqdq	xmm12, xmm2, xmm1, 17
        vpclmulqdq	xmm13, xmm2, xmm2, 0
        vpclmulqdq	xmm14, xmm2, xmm2, 17
        vpxor	xmm11, xmm11, xmm10
        vpslldq	xmm10, xmm11, 8
        vpsrldq	xmm11, xmm11, 8
        vpxor	xmm10, xmm10, xmm9
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm10, xmm10, xmm9
        vpxor	xmm13, xmm13, xmm8
        vpclmulqdq	xmm9, xmm10, xmm3, 16
        vpclmulqdq	xmm8, xmm13, xmm3, 16
        vpshufd	xmm10, xmm10, 78
        vpshufd	xmm13, xmm13, 78
        vpxor	xmm12, xmm12, xmm11
        vpxor	xmm13, xmm13, xmm8
        vpxor	xmm10, xmm10, xmm12
        vpxor	xmm0, xmm13, xmm14
        vpxor	xmm7, xmm10, xmm9
        vmovdqu	OWORD PTR [rsp+96], xmm7
        vmovdqu	OWORD PTR [rsp+112], xmm0
L_AES_GCM_decrypt_update_avx2_ghash_128:
        ; aesenc_128_ghash
        lea	rcx, QWORD PTR [r11+rdi]
        lea	rdx, QWORD PTR [r10+rdi]
        ; aesenc_ctr
        vmovdqu	xmm0, OWORD PTR [rsp+128]
        vmovdqu	xmm1, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpaddd	xmm9, xmm0, OWORD PTR L_avx2_aes_gcm_one
        vpshufb	xmm8, xmm0, xmm1
        vpaddd	xmm10, xmm0, OWORD PTR L_avx2_aes_gcm_two
        vpshufb	xmm9, xmm9, xmm1
        vpaddd	xmm11, xmm0, OWORD PTR L_avx2_aes_gcm_three
        vpshufb	xmm10, xmm10, xmm1
        vpaddd	xmm12, xmm0, OWORD PTR L_avx2_aes_gcm_four
        vpshufb	xmm11, xmm11, xmm1
        vpaddd	xmm13, xmm0, OWORD PTR L_avx2_aes_gcm_five
        vpshufb	xmm12, xmm12, xmm1
        vpaddd	xmm14, xmm0, OWORD PTR L_avx2_aes_gcm_six
        vpshufb	xmm13, xmm13, xmm1
        vpaddd	xmm15, xmm0, OWORD PTR L_avx2_aes_gcm_seven
        vpshufb	xmm14, xmm14, xmm1
        vpaddd	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_eight
        vpshufb	xmm15, xmm15, xmm1
        ; aesenc_xor
        vmovdqu	xmm7, OWORD PTR [rax]
        vmovdqu	OWORD PTR [rsp+128], xmm0
        vpxor	xmm8, xmm8, xmm7
        vpxor	xmm9, xmm9, xmm7
        vpxor	xmm10, xmm10, xmm7
        vpxor	xmm11, xmm11, xmm7
        vpxor	xmm12, xmm12, xmm7
        vpxor	xmm13, xmm13, xmm7
        vpxor	xmm14, xmm14, xmm7
        vpxor	xmm15, xmm15, xmm7
        ; aesenc_pclmul_1
        vmovdqu	xmm1, OWORD PTR [rcx]
        vmovdqu	xmm0, OWORD PTR [rax+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vmovdqu	xmm2, OWORD PTR [rsp+112]
        vpxor	xmm1, xmm1, xmm6
        vpclmulqdq	xmm5, xmm1, xmm2, 16
        vpclmulqdq	xmm3, xmm1, xmm2, 1
        vpclmulqdq	xmm6, xmm1, xmm2, 0
        vpclmulqdq	xmm7, xmm1, xmm2, 17
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_2
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm0, OWORD PTR [rsp+96]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+32]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+32]
        vmovdqu	xmm0, OWORD PTR [rsp+80]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+48]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+48]
        vmovdqu	xmm0, OWORD PTR [rsp+64]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+64]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+64]
        vmovdqu	xmm0, OWORD PTR [rsp+48]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+80]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm0, OWORD PTR [rsp+32]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+96]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+96]
        vmovdqu	xmm0, OWORD PTR [rsp+16]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+112]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_n
        vmovdqu	xmm1, OWORD PTR [rcx+112]
        vmovdqu	xmm0, OWORD PTR [rsp]
        vpshufb	xmm1, xmm1, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm5, xmm5, xmm2
        vpclmulqdq	xmm2, xmm1, xmm0, 16
        vpxor	xmm5, xmm5, xmm3
        vpclmulqdq	xmm3, xmm1, xmm0, 1
        vpxor	xmm6, xmm6, xmm4
        vpclmulqdq	xmm4, xmm1, xmm0, 0
        vpclmulqdq	xmm1, xmm1, xmm0, 17
        vmovdqu	xmm0, OWORD PTR [rax+128]
        vpxor	xmm7, xmm7, xmm1
        vaesenc	xmm8, xmm8, xmm0
        vaesenc	xmm9, xmm9, xmm0
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm11, xmm11, xmm0
        vaesenc	xmm12, xmm12, xmm0
        vaesenc	xmm13, xmm13, xmm0
        vaesenc	xmm14, xmm14, xmm0
        vaesenc	xmm15, xmm15, xmm0
        ; aesenc_pclmul_l
        vpxor	xmm5, xmm5, xmm2
        vpxor	xmm6, xmm6, xmm4
        vpxor	xmm5, xmm5, xmm3
        vpslldq	xmm1, xmm5, 8
        vpsrldq	xmm5, xmm5, 8
        vmovdqu	xmm4, OWORD PTR [rax+144]
        vmovdqu	xmm0, OWORD PTR L_avx2_aes_gcm_mod2_128
        vaesenc	xmm8, xmm8, xmm4
        vpxor	xmm6, xmm6, xmm1
        vpxor	xmm7, xmm7, xmm5
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm9, xmm9, xmm4
        vaesenc	xmm10, xmm10, xmm4
        vaesenc	xmm11, xmm11, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpclmulqdq	xmm3, xmm6, xmm0, 16
        vaesenc	xmm12, xmm12, xmm4
        vaesenc	xmm13, xmm13, xmm4
        vaesenc	xmm14, xmm14, xmm4
        vpshufd	xmm6, xmm6, 78
        vpxor	xmm6, xmm6, xmm3
        vpxor	xmm6, xmm6, xmm7
        vaesenc	xmm15, xmm15, xmm4
        cmp	r8d, 11
        vmovdqu	xmm7, OWORD PTR [rax+160]
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+176]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        cmp	r8d, 13
        vmovdqu	xmm7, OWORD PTR [rax+192]
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_128_ghash_avx_done
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+208]
        vaesenc	xmm8, xmm8, xmm7
        vaesenc	xmm9, xmm9, xmm7
        vaesenc	xmm10, xmm10, xmm7
        vaesenc	xmm11, xmm11, xmm7
        vaesenc	xmm12, xmm12, xmm7
        vaesenc	xmm13, xmm13, xmm7
        vaesenc	xmm14, xmm14, xmm7
        vaesenc	xmm15, xmm15, xmm7
        vmovdqu	xmm7, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_avx2_aesenc_128_ghash_avx_done:
        ; aesenc_last
        vaesenclast	xmm8, xmm8, xmm7
        vaesenclast	xmm9, xmm9, xmm7
        vaesenclast	xmm10, xmm10, xmm7
        vaesenclast	xmm11, xmm11, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx]
        vmovdqu	xmm1, OWORD PTR [rcx+16]
        vmovdqu	xmm2, OWORD PTR [rcx+32]
        vmovdqu	xmm3, OWORD PTR [rcx+48]
        vpxor	xmm8, xmm8, xmm0
        vpxor	xmm9, xmm9, xmm1
        vpxor	xmm10, xmm10, xmm2
        vpxor	xmm11, xmm11, xmm3
        vmovdqu	OWORD PTR [rdx], xmm8
        vmovdqu	OWORD PTR [rdx+16], xmm9
        vmovdqu	OWORD PTR [rdx+32], xmm10
        vmovdqu	OWORD PTR [rdx+48], xmm11
        vaesenclast	xmm12, xmm12, xmm7
        vaesenclast	xmm13, xmm13, xmm7
        vaesenclast	xmm14, xmm14, xmm7
        vaesenclast	xmm15, xmm15, xmm7
        vmovdqu	xmm0, OWORD PTR [rcx+64]
        vmovdqu	xmm1, OWORD PTR [rcx+80]
        vmovdqu	xmm2, OWORD PTR [rcx+96]
        vmovdqu	xmm3, OWORD PTR [rcx+112]
        vpxor	xmm12, xmm12, xmm0
        vpxor	xmm13, xmm13, xmm1
        vpxor	xmm14, xmm14, xmm2
        vpxor	xmm15, xmm15, xmm3
        vmovdqu	OWORD PTR [rdx+64], xmm12
        vmovdqu	OWORD PTR [rdx+80], xmm13
        vmovdqu	OWORD PTR [rdx+96], xmm14
        vmovdqu	OWORD PTR [rdx+112], xmm15
        ; aesenc_128_ghash - end
        add	edi, 128
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_avx2_ghash_128
        vmovdqu	xmm5, OWORD PTR [rsp]
        vmovdqu	xmm4, OWORD PTR [rsp+128]
        vmovdqu	xmm15, OWORD PTR [rsp+144]
L_AES_GCM_decrypt_update_avx2_done_128:
        cmp	edi, r9d
        jge	L_AES_GCM_decrypt_update_avx2_done_dec
        mov	r13d, r9d
        and	r13d, 4294967280
        cmp	edi, r13d
        jge	L_AES_GCM_decrypt_update_avx2_last_block_done
L_AES_GCM_decrypt_update_avx2_last_block_start:
        vmovdqu	xmm11, OWORD PTR [r11+rdi]
        vpshufb	xmm10, xmm4, OWORD PTR L_avx2_aes_gcm_bswap_epi64
        vpshufb	xmm12, xmm11, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpaddd	xmm4, xmm4, OWORD PTR L_avx2_aes_gcm_one
        vpxor	xmm12, xmm12, xmm6
        ; aesenc_gfmul_sb
        vpclmulqdq	xmm2, xmm12, xmm5, 1
        vpclmulqdq	xmm3, xmm12, xmm5, 16
        vpclmulqdq	xmm1, xmm12, xmm5, 0
        vpclmulqdq	xmm8, xmm12, xmm5, 17
        vpxor	xmm10, xmm10, [rax]
        vaesenc	xmm10, xmm10, [rax+16]
        vpxor	xmm3, xmm3, xmm2
        vpslldq	xmm2, xmm3, 8
        vpsrldq	xmm3, xmm3, 8
        vaesenc	xmm10, xmm10, [rax+32]
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm10, xmm10, [rax+48]
        vaesenc	xmm10, xmm10, [rax+64]
        vaesenc	xmm10, xmm10, [rax+80]
        vpshufd	xmm2, xmm2, 78
        vpxor	xmm2, xmm2, xmm1
        vpclmulqdq	xmm1, xmm2, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vaesenc	xmm10, xmm10, [rax+96]
        vaesenc	xmm10, xmm10, [rax+112]
        vaesenc	xmm10, xmm10, [rax+128]
        vpshufd	xmm2, xmm2, 78
        vaesenc	xmm10, xmm10, [rax+144]
        vpxor	xmm8, xmm8, xmm3
        vpxor	xmm2, xmm2, xmm8
        vmovdqu	xmm0, OWORD PTR [rax+160]
        cmp	r8d, 11
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm10, xmm10, [rax+176]
        vmovdqu	xmm0, OWORD PTR [rax+192]
        cmp	r8d, 13
        jl	L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last
        vaesenc	xmm10, xmm10, xmm0
        vaesenc	xmm10, xmm10, [rax+208]
        vmovdqu	xmm0, OWORD PTR [rax+224]
L_AES_GCM_decrypt_update_avx2_aesenc_gfmul_sb_last:
        vaesenclast	xmm10, xmm10, xmm0
        vpxor	xmm6, xmm2, xmm1
        vpxor	xmm10, xmm10, xmm11
        vmovdqu	OWORD PTR [r10+rdi], xmm10
        add	edi, 16
        cmp	edi, r13d
        jl	L_AES_GCM_decrypt_update_avx2_last_block_start
L_AES_GCM_decrypt_update_avx2_last_block_done:
L_AES_GCM_decrypt_update_avx2_done_dec:
        vmovdqu	OWORD PTR [r12], xmm6
        vmovdqu	OWORD PTR [r15], xmm4
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+168]
        vmovdqu	xmm7, OWORD PTR [rsp+184]
        vmovdqu	xmm8, OWORD PTR [rsp+200]
        vmovdqu	xmm9, OWORD PTR [rsp+216]
        vmovdqu	xmm10, OWORD PTR [rsp+232]
        vmovdqu	xmm11, OWORD PTR [rsp+248]
        vmovdqu	xmm12, OWORD PTR [rsp+264]
        vmovdqu	xmm13, OWORD PTR [rsp+280]
        vmovdqu	xmm14, OWORD PTR [rsp+296]
        vmovdqu	xmm15, OWORD PTR [rsp+312]
        add	rsp, 328
        pop	rdi
        pop	r15
        pop	r14
        pop	r12
        pop	r13
        ret
AES_GCM_decrypt_update_avx2 ENDP
_text ENDS
_text SEGMENT READONLY PARA
AES_GCM_decrypt_final_avx2 PROC
        push	r12
        push	r13
        push	r14
        mov	eax, DWORD PTR [rsp+64]
        mov	r10, QWORD PTR [rsp+72]
        mov	r11, QWORD PTR [rsp+80]
        mov	r12, QWORD PTR [rsp+88]
        sub	rsp, 48
        vmovdqu	OWORD PTR [rsp+16], xmm6
        vmovdqu	OWORD PTR [rsp+32], xmm7
        vmovdqu	xmm4, OWORD PTR [rcx]
        vmovdqu	xmm5, OWORD PTR [r10]
        vmovdqu	xmm6, OWORD PTR [r11]
        vpsrlq	xmm1, xmm5, 63
        vpsllq	xmm0, xmm5, 1
        vpslldq	xmm1, xmm1, 8
        vpor	xmm0, xmm0, xmm1
        vpshufd	xmm5, xmm5, 255
        vpsrad	xmm5, xmm5, 31
        vpand	xmm5, xmm5, OWORD PTR L_avx2_aes_gcm_mod2_128
        vpxor	xmm5, xmm5, xmm0
        ; calc_tag
        shl	r9, 3
        shl	rax, 3
        vmovq	xmm0, r9
        vmovq	xmm1, rax
        vpunpcklqdq	xmm0, xmm0, xmm1
        vpxor	xmm0, xmm0, xmm4
        ; ghash_gfmul_red
        vpclmulqdq	xmm7, xmm0, xmm5, 16
        vpclmulqdq	xmm3, xmm0, xmm5, 1
        vpclmulqdq	xmm2, xmm0, xmm5, 0
        vpxor	xmm7, xmm7, xmm3
        vpslldq	xmm3, xmm7, 8
        vpsrldq	xmm7, xmm7, 8
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm0, xmm0, xmm5, 17
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm3, xmm3, xmm2
        vpclmulqdq	xmm2, xmm3, OWORD PTR L_avx2_aes_gcm_mod2_128, 16
        vpshufd	xmm3, xmm3, 78
        vpxor	xmm0, xmm0, xmm7
        vpxor	xmm0, xmm0, xmm3
        vpxor	xmm0, xmm0, xmm2
        vpshufb	xmm0, xmm0, OWORD PTR L_avx2_aes_gcm_bswap_mask
        vpxor	xmm0, xmm0, xmm6
        ; cmp_tag
        cmp	r8d, 16
        je	L_AES_GCM_decrypt_final_avx2_cmp_tag_16
        xor	r13, r13
        xor	r10, r10
        vmovdqu	OWORD PTR [rsp], xmm0
L_AES_GCM_decrypt_final_avx2_cmp_tag_loop:
        movzx	r14d, BYTE PTR [rsp+r13]
        xor	r14b, BYTE PTR [rdx+r13]
        or	r10b, r14b
        inc	r13d
        cmp	r13d, r8d
        jne	L_AES_GCM_decrypt_final_avx2_cmp_tag_loop
        cmp	r10, 0
        sete	r10b
        jmp	L_AES_GCM_decrypt_final_avx2_cmp_tag_done
L_AES_GCM_decrypt_final_avx2_cmp_tag_16:
        vmovdqu	xmm1, OWORD PTR [rdx]
        vpcmpeqb	xmm0, xmm0, xmm1
        vpmovmskb	r13, xmm0
        ; %%edx == 0xFFFF then return 1 else => return 0
        xor	r10d, r10d
        cmp	r13d, 65535
        sete	r10b
L_AES_GCM_decrypt_final_avx2_cmp_tag_done:
        mov	DWORD PTR [r12], r10d
        vzeroupper
        vmovdqu	xmm6, OWORD PTR [rsp+16]
        vmovdqu	xmm7, OWORD PTR [rsp+32]
        add	rsp, 48
        pop	r14
        pop	r13
        pop	r12
        ret
AES_GCM_decrypt_final_avx2 ENDP
_text ENDS
ENDIF
END
