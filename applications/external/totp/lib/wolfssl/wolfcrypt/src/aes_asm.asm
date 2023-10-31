; /* aes_asm.asm
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



;
;
;  /* See Intel Advanced Encryption Standard (AES) Instructions Set White Paper
;   * by Israel, Intel Mobility Group Development Center, Israel Shay Gueron
;   */
;
;   /* This file is in intel asm syntax, see .s for at&t syntax */
;


fips_version = 0
IFDEF HAVE_FIPS
  fips_version = 1
  IFDEF HAVE_FIPS_VERSION
    fips_version = HAVE_FIPS_VERSION
  ENDIF
ENDIF

IF fips_version GE 2
  fipsAh SEGMENT ALIAS(".fipsA$h") 'CODE'
ELSE
  _text SEGMENT
ENDIF


;	/*
;	AES_CBC_encrypt[const	,unsigned	char*in
;	unsigned	,char*out
;	unsigned	,char	ivec+16
;	unsigned	,long	length
;	const	,unsigned	char*KS
;	int	nr]
;	*/
AES_CBC_encrypt PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi
;#	parameter	3:	rdx
;#	parameter	4:	rcx
;#	parameter	5:	r8
;#	parameter	6:	r9d

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
	mov rdi,rcx
	mov rsi,rdx
	mov rdx,r8
	mov rcx,r9
	mov r8,[rsp+40]
	mov r9d,[rsp+48]

	mov	r10,rcx
	shr	rcx,4
	shl	r10,60
	je	NO_PARTS
	add	rcx,1
NO_PARTS:
	sub	rsi,16
	movdqa	xmm1,[rdx]
LOOP_1:
	pxor	xmm1,[rdi]
	pxor	xmm1,[r8]
	add	rsi,16
	add	rdi,16
	cmp	r9d,12
	aesenc	xmm1,16[r8]
	aesenc	xmm1,32[r8]
	aesenc	xmm1,48[r8]
	aesenc	xmm1,64[r8]
	aesenc	xmm1,80[r8]
	aesenc	xmm1,96[r8]
	aesenc	xmm1,112[r8]
	aesenc	xmm1,128[r8]
	aesenc	xmm1,144[r8]
	movdqa	xmm2,160[r8]
	jb	LAST
	cmp	r9d,14

	aesenc	xmm1,160[r8]
	aesenc	xmm1,176[r8]
	movdqa	xmm2,192[r8]
	jb	LAST
	aesenc	xmm1,192[r8]
	aesenc	xmm1,208[r8]
	movdqa	xmm2,224[r8]
LAST:
	dec	rcx
	aesenclast	xmm1,xmm2
	movdqu	[rsi],xmm1
	jne	LOOP_1
	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
	ret
AES_CBC_encrypt ENDP


; void AES_CBC_decrypt_by4(const unsigned char* in,
;                          unsigned char* out,
;                          unsigned char ivec[16],
;                          unsigned long length,
;                          const unsigned char* KS,
;                          int nr)
AES_CBC_decrypt_by4 PROC
; parameter 1: rdi
; parameter 2: rsi
; parameter 3: rdx
; parameter 4: rcx
; parameter 5: r8
; parameter 6: r9d

        ; save rdi and rsi to rax and r11, restore before ret
        mov         rax, rdi
        mov         r11, rsi
        ; convert to what we had for att&t convention
        mov         rdi, rcx
        mov         rsi, rdx
        mov         rdx, r8
        mov         rcx,r9
        mov         r8, [rsp+40]
        mov         r9d, [rsp+48]
        ; on microsoft xmm6-xmm15 are non volatile,
        ; let's save on stack and restore at end
        sub         rsp, 8+8*16  ; 8 = align stack , 8 xmm6-12,15 16 bytes each
        movdqa      [rsp+0], xmm6
        movdqa      [rsp+16], xmm7
        movdqa      [rsp+32], xmm8
        movdqa      [rsp+48], xmm9
        movdqa      [rsp+64], xmm10
        movdqa      [rsp+80], xmm11
        movdqa      [rsp+96], xmm12
        movdqa      [rsp+112], xmm15
        ; back to our original code, more or less
        mov         r10, rcx
        shr         rcx, 4
        shl         r10, 60
        je          DNO_PARTS_4
        add         rcx, 1
DNO_PARTS_4:
        mov         r10, rcx
        shl         r10, 62
        shr         r10, 62
        shr         rcx, 2
        movdqu      xmm5, [rdx]
        je          DREMAINDER_4
        sub         rsi, 64
DLOOP_4:
        movdqu      xmm1, [rdi]
        movdqu      xmm2, 16[rdi]
        movdqu      xmm3, 32[rdi]
        movdqu      xmm4, 48[rdi]
        movdqa      xmm6, xmm1
        movdqa      xmm7, xmm2
        movdqa      xmm8, xmm3
        movdqa      xmm15, xmm4
        movdqa      xmm9, [r8]
        movdqa      xmm10, 16[r8]
        movdqa      xmm11, 32[r8]
        movdqa      xmm12, 48[r8]
        pxor        xmm1, xmm9
        pxor        xmm2, xmm9
        pxor        xmm3, xmm9
        pxor        xmm4, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm1, xmm12
        aesdec      xmm2, xmm12
        aesdec      xmm3, xmm12
        aesdec      xmm4, xmm12
        movdqa      xmm9, 64[r8]
        movdqa      xmm10, 80[r8]
        movdqa      xmm11, 96[r8]
        movdqa      xmm12, 112[r8]
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm1, xmm12
        aesdec      xmm2, xmm12
        aesdec      xmm3, xmm12
        aesdec      xmm4, xmm12
        movdqa      xmm9, 128[r8]
        movdqa      xmm10, 144[r8]
        movdqa      xmm11, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        jb          DLAST_4
        movdqa      xmm9, 160[r8]
        movdqa      xmm10, 176[r8]
        movdqa      xmm11, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        jb          DLAST_4
        movdqa      xmm9, 192[r8]
        movdqa      xmm10, 208[r8]
        movdqa      xmm11, 224[r8]
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
DLAST_4:
        add         rdi, 64
        add         rsi, 64
        dec         rcx
        aesdeclast  xmm1, xmm11
        aesdeclast  xmm2, xmm11
        aesdeclast  xmm3, xmm11
        aesdeclast  xmm4, xmm11
        pxor        xmm1, xmm5
        pxor        xmm2, xmm6
        pxor        xmm3, xmm7
        pxor        xmm4, xmm8
        movdqu      [rsi], xmm1
        movdqu      16[rsi], xmm2
        movdqu      32[rsi], xmm3
        movdqu      48[rsi], xmm4
        movdqa      xmm5, xmm15
        jne         DLOOP_4
        add         rsi, 64
DREMAINDER_4:
        cmp         r10, 0
        je          DEND_4
DLOOP_4_2:
        movdqu      xmm1, [rdi]
        movdqa      xmm15, xmm1
        add         rdi, 16
        pxor        xmm1, [r8]
        movdqu      xmm2, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, 16[r8]
        aesdec      xmm1, 32[r8]
        aesdec      xmm1, 48[r8]
        aesdec      xmm1, 64[r8]
        aesdec      xmm1, 80[r8]
        aesdec      xmm1, 96[r8]
        aesdec      xmm1, 112[r8]
        aesdec      xmm1, 128[r8]
        aesdec      xmm1, 144[r8]
        jb          DLAST_4_2
        movdqu      xmm2, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, 160[r8]
        aesdec      xmm1, 176[r8]
        jb          DLAST_4_2
        movdqu      xmm2, 224[r8]
        aesdec      xmm1, 192[r8]
        aesdec      xmm1, 208[r8]
DLAST_4_2:
        aesdeclast  xmm1, xmm2
        pxor        xmm1, xmm5
        movdqa      xmm5, xmm15
        movdqu      [rsi], xmm1
        add         rsi, 16
        dec         r10
        jne         DLOOP_4_2
DEND_4:
        ; restore non volatile rdi,rsi
        mov         rdi, rax
        mov         rsi, r11
        ; restore non volatile xmms from stack
        movdqa      xmm6, [rsp+0]
        movdqa      xmm7, [rsp+16]
        movdqa      xmm8, [rsp+32]
        movdqa      xmm9, [rsp+48]
        movdqa      xmm10, [rsp+64]
        movdqa      xmm11, [rsp+80]
        movdqa      xmm12, [rsp+96]
        movdqa      xmm15, [rsp+112]
        add         rsp, 8+8*16 ; 8 = align stack , 8 xmm6-12,15 16 bytes each
        ret
AES_CBC_decrypt_by4 ENDP


; void AES_CBC_decrypt_by6(const unsigned char *in,
;                          unsigned char *out,
;                          unsigned char ivec[16],
;                          unsigned long length,
;                          const unsigned char *KS,
;                          int nr)
AES_CBC_decrypt_by6 PROC
; parameter 1: rdi - in
; parameter 2: rsi - out
; parameter 3: rdx - ivec
; parameter 4: rcx - length
; parameter 5: r8  - KS
; parameter 6: r9d - nr

        ; save rdi and rsi to rax and r11, restore before ret
        mov         rax, rdi
        mov         r11, rsi
        ; convert to what we had for att&t convention
        mov         rdi, rcx
        mov         rsi, rdx
        mov         rdx, r8
        mov         rcx, r9
        mov         r8, [rsp+40]
        mov         r9d, [rsp+48]
        ; on microsoft xmm6-xmm15 are non volatile,
        ; let's save on stack and restore at end
        sub         rsp, 8+9*16  ; 8 = align stack , 9 xmm6-14 16 bytes each
        movdqa      [rsp+0], xmm6
        movdqa      [rsp+16], xmm7
        movdqa      [rsp+32], xmm8
        movdqa      [rsp+48], xmm9
        movdqa      [rsp+64], xmm10
        movdqa      [rsp+80], xmm11
        movdqa      [rsp+96], xmm12
        movdqa      [rsp+112], xmm13
        movdqa      [rsp+128], xmm14
        ; back to our original code, more or less
        mov         r10, rcx
        shr         rcx, 4
        shl         r10, 60
        je          DNO_PARTS_6
        add         rcx, 1
DNO_PARTS_6:
        mov         r12, rax
        mov         r13, rdx
        mov         r14, rbx
        mov         rdx, 0
        mov         rax, rcx
        mov         rbx, 6
        div         rbx
        mov         rcx, rax
        mov         r10, rdx
        mov         rax, r12
        mov         rdx, r13
        mov         rbx, r14
        cmp         rcx, 0
        movdqu      xmm7, [rdx]
        je          DREMAINDER_6
        sub         rsi, 96
DLOOP_6:
        movdqu      xmm1, [rdi]
        movdqu      xmm2, 16[rdi]
        movdqu      xmm3, 32[rdi]
        movdqu      xmm4, 48[rdi]
        movdqu      xmm5, 64[rdi]
        movdqu      xmm6, 80[rdi]
        movdqa      xmm8, [r8]
        movdqa      xmm9, 16[r8]
        movdqa      xmm10, 32[r8]
        movdqa      xmm11, 48[r8]
        pxor        xmm1, xmm8
        pxor        xmm2, xmm8
        pxor        xmm3, xmm8
        pxor        xmm4, xmm8
        pxor        xmm5, xmm8
        pxor        xmm6, xmm8
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm5, xmm9
        aesdec      xmm6, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        movdqa      xmm8, 64[r8]
        movdqa      xmm9, 80[r8]
        movdqa      xmm10, 96[r8]
        movdqa      xmm11, 112[r8]
        aesdec      xmm1, xmm8
        aesdec      xmm2, xmm8
        aesdec      xmm3, xmm8
        aesdec      xmm4, xmm8
        aesdec      xmm5, xmm8
        aesdec      xmm6, xmm8
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm5, xmm9
        aesdec      xmm6, xmm9
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        movdqa      xmm8, 128[r8]
        movdqa      xmm9, 144[r8]
        movdqa      xmm10, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, xmm8
        aesdec      xmm2, xmm8
        aesdec      xmm3, xmm8
        aesdec      xmm4, xmm8
        aesdec      xmm5, xmm8
        aesdec      xmm6, xmm8
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm5, xmm9
        aesdec      xmm6, xmm9
        jb          DLAST_6
        movdqa      xmm8, 160[r8]
        movdqa      xmm9, 176[r8]
        movdqa      xmm10, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, xmm8
        aesdec      xmm2, xmm8
        aesdec      xmm3, xmm8
        aesdec      xmm4, xmm8
        aesdec      xmm5, xmm8
        aesdec      xmm6, xmm8
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm5, xmm9
        aesdec      xmm6, xmm9
        jb          DLAST_6
        movdqa      xmm8, 192[r8]
        movdqa      xmm9, 208[r8]
        movdqa      xmm10, 224[r8]
        aesdec      xmm1, xmm8
        aesdec      xmm2, xmm8
        aesdec      xmm3, xmm8
        aesdec      xmm4, xmm8
        aesdec      xmm5, xmm8
        aesdec      xmm6, xmm8
        aesdec      xmm1, xmm9
        aesdec      xmm2, xmm9
        aesdec      xmm3, xmm9
        aesdec      xmm4, xmm9
        aesdec      xmm5, xmm9
        aesdec      xmm6, xmm9
DLAST_6:
        add         rsi, 96
        aesdeclast  xmm1, xmm10
        aesdeclast  xmm2, xmm10
        aesdeclast  xmm3, xmm10
        aesdeclast  xmm4, xmm10
        aesdeclast  xmm5, xmm10
        aesdeclast  xmm6, xmm10
        movdqu      xmm8, [rdi]
        movdqu      xmm9, 16[rdi]
        movdqu      xmm10, 32[rdi]
        movdqu      xmm11, 48[rdi]
        movdqu      xmm12, 64[rdi]
        movdqu      xmm13, 80[rdi]
        pxor        xmm1, xmm7
        pxor        xmm2, xmm8
        pxor        xmm3, xmm9
        pxor        xmm4, xmm10
        pxor        xmm5, xmm11
        pxor        xmm6, xmm12
        movdqu      xmm7, xmm13
        movdqu      [rsi], xmm1
        movdqu      16[rsi], xmm2
        movdqu      32[rsi], xmm3
        movdqu      48[rsi], xmm4
        movdqu      64[rsi], xmm5
        movdqu      80[rsi], xmm6
        add         rdi, 96
        dec         rcx
        jne         DLOOP_6
        add         rsi, 96
DREMAINDER_6:
        cmp         r10, 0
        je          DEND_6
DLOOP_6_2:
        movdqu      xmm1, [rdi]
        movdqa      xmm10, xmm1
        add         rdi, 16
        pxor        xmm1, [r8]
        movdqu      xmm2, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, 16[r8]
        aesdec      xmm1, 32[r8]
        aesdec      xmm1, 48[r8]
        aesdec      xmm1, 64[r8]
        aesdec      xmm1, 80[r8]
        aesdec      xmm1, 96[r8]
        aesdec      xmm1, 112[r8]
        aesdec      xmm1, 128[r8]
        aesdec      xmm1, 144[r8]
        jb          DLAST_6_2
        movdqu      xmm2, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, 160[r8]
        aesdec      xmm1, 176[r8]
        jb          DLAST_6_2
        movdqu      xmm2, 224[r8]
        aesdec      xmm1, 192[r8]
        aesdec      xmm1, 208[r8]
DLAST_6_2:
        aesdeclast  xmm1, xmm2
        pxor        xmm1, xmm7
        movdqa      xmm7, xmm10
        movdqu      [rsi], xmm1
        add         rsi, 16
        dec         r10
        jne         DLOOP_6_2
DEND_6:
        ; restore non volatile rdi,rsi
        mov         rdi, rax
        mov         rsi, r11
        ; restore non volatile xmms from stack
        movdqa      xmm6, [rsp+0]
        movdqa      xmm7, [rsp+16]
        movdqa      xmm8, [rsp+32]
        movdqa      xmm9, [rsp+48]
        movdqa      xmm10, [rsp+64]
        movdqa      xmm11, [rsp+80]
        movdqa      xmm12, [rsp+96]
        movdqa      xmm13, [rsp+112]
        movdqa      xmm14, [rsp+128]
        add         rsp, 8+9*16 ; 8 = align stack , 9 xmm6-14 16 bytes each
        ret
AES_CBC_decrypt_by6 ENDP


; void AES_CBC_decrypt_by8(const unsigned char *in,
;                          unsigned char *out,
;                          unsigned char ivec[16],
;                          unsigned long length,
;                          const unsigned char *KS,
;                          int nr)
AES_CBC_decrypt_by8 PROC
; parameter 1: rdi - in
; parameter 2: rsi - out
; parameter 3: rdx - ivec
; parameter 4: rcx - length
; parameter 5: r8  - KS
; parameter 6: r9d - nr

        ; save rdi and rsi to rax and r11, restore before ret
        mov         rax, rdi
        mov         r11, rsi
        ; convert to what we had for att&t convention
        mov         rdi, rcx
        mov         rsi, rdx
        mov         rdx, r8
        mov         rcx,r9
        mov         r8, [rsp+40]
        mov         r9d, [rsp+48]
        ; on microsoft xmm6-xmm15 are non volatile,
        ; let's save on stack and restore at end
        sub         rsp, 8+8*16  ; 8 = align stack , 8 xmm6-13 16 bytes each
        movdqa      [rsp+0], xmm6
        movdqa      [rsp+16], xmm7
        movdqa      [rsp+32], xmm8
        movdqa      [rsp+48], xmm9
        movdqa      [rsp+64], xmm10
        movdqa      [rsp+80], xmm11
        movdqa      [rsp+96], xmm12
        movdqa      [rsp+112], xmm13
        ; back to our original code, more or less
        mov         r10, rcx
        shr         rcx, 4
        shl         r10, 60
        je          DNO_PARTS_8
        add         rcx, 1
DNO_PARTS_8:
        mov         r10, rcx
        shl         r10, 61
        shr         r10, 61
        shr         rcx, 3
        movdqu      xmm9, [rdx]
        je          DREMAINDER_8
        sub         rsi, 128
DLOOP_8:
        movdqu      xmm1, [rdi]
        movdqu      xmm2, 16[rdi]
        movdqu      xmm3, 32[rdi]
        movdqu      xmm4, 48[rdi]
        movdqu      xmm5, 64[rdi]
        movdqu      xmm6, 80[rdi]
        movdqu      xmm7, 96[rdi]
        movdqu      xmm8, 112[rdi]
        movdqa      xmm10, [r8]
        movdqa      xmm11, 16[r8]
        movdqa      xmm12, 32[r8]
        movdqa      xmm13, 48[r8]
        pxor        xmm1, xmm10
        pxor        xmm2, xmm10
        pxor        xmm3, xmm10
        pxor        xmm4, xmm10
        pxor        xmm5, xmm10
        pxor        xmm6, xmm10
        pxor        xmm7, xmm10
        pxor        xmm8, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        aesdec      xmm7, xmm11
        aesdec      xmm8, xmm11
        aesdec      xmm1, xmm12
        aesdec      xmm2, xmm12
        aesdec      xmm3, xmm12
        aesdec      xmm4, xmm12
        aesdec      xmm5, xmm12
        aesdec      xmm6, xmm12
        aesdec      xmm7, xmm12
        aesdec      xmm8, xmm12
        aesdec      xmm1, xmm13
        aesdec      xmm2, xmm13
        aesdec      xmm3, xmm13
        aesdec      xmm4, xmm13
        aesdec      xmm5, xmm13
        aesdec      xmm6, xmm13
        aesdec      xmm7, xmm13
        aesdec      xmm8, xmm13
        movdqa      xmm10, 64[r8]
        movdqa      xmm11, 80[r8]
        movdqa      xmm12, 96[r8]
        movdqa      xmm13, 112[r8]
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm7, xmm10
        aesdec      xmm8, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        aesdec      xmm7, xmm11
        aesdec      xmm8, xmm11
        aesdec      xmm1, xmm12
        aesdec      xmm2, xmm12
        aesdec      xmm3, xmm12
        aesdec      xmm4, xmm12
        aesdec      xmm5, xmm12
        aesdec      xmm6, xmm12
        aesdec      xmm7, xmm12
        aesdec      xmm8, xmm12
        aesdec      xmm1, xmm13
        aesdec      xmm2, xmm13
        aesdec      xmm3, xmm13
        aesdec      xmm4, xmm13
        aesdec      xmm5, xmm13
        aesdec      xmm6, xmm13
        aesdec      xmm7, xmm13
        aesdec      xmm8, xmm13
        movdqa      xmm10, 128[r8]
        movdqa      xmm11, 144[r8]
        movdqa      xmm12, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm7, xmm10
        aesdec      xmm8, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        aesdec      xmm7, xmm11
        aesdec      xmm8, xmm11
        jb          DLAST_8
        movdqa      xmm10, 160[r8]
        movdqa      xmm11, 176[r8]
        movdqa      xmm12, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm7, xmm10
        aesdec      xmm8, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        aesdec      xmm7, xmm11
        aesdec      xmm8, xmm11
        jb          DLAST_8
        movdqa      xmm10, 192[r8]
        movdqa      xmm11, 208[r8]
        movdqa      xmm12, 224[r8]
        aesdec      xmm1, xmm10
        aesdec      xmm2, xmm10
        aesdec      xmm3, xmm10
        aesdec      xmm4, xmm10
        aesdec      xmm5, xmm10
        aesdec      xmm6, xmm10
        aesdec      xmm7, xmm10
        aesdec      xmm8, xmm10
        aesdec      xmm1, xmm11
        aesdec      xmm2, xmm11
        aesdec      xmm3, xmm11
        aesdec      xmm4, xmm11
        aesdec      xmm5, xmm11
        aesdec      xmm6, xmm11
        aesdec      xmm7, xmm11
        aesdec      xmm8, xmm11
DLAST_8:
        add         rsi, 128
        aesdeclast  xmm1, xmm12
        aesdeclast  xmm2, xmm12
        aesdeclast  xmm3, xmm12
        aesdeclast  xmm4, xmm12
        aesdeclast  xmm5, xmm12
        aesdeclast  xmm6, xmm12
        aesdeclast  xmm7, xmm12
        aesdeclast  xmm8, xmm12
        movdqu      xmm10, [rdi]
        movdqu      xmm11, 16[rdi]
        movdqu      xmm12, 32[rdi]
        movdqu      xmm13, 48[rdi]
        pxor        xmm1, xmm9
        pxor        xmm2, xmm10
        pxor        xmm3, xmm11
        pxor        xmm4, xmm12
        pxor        xmm5, xmm13
        movdqu      xmm10, 64[rdi]
        movdqu      xmm11, 80[rdi]
        movdqu      xmm12, 96[rdi]
        movdqu      xmm9, 112[rdi]
        pxor        xmm6, xmm10
        pxor        xmm7, xmm11
        pxor        xmm8, xmm12
        movdqu      [rsi], xmm1
        movdqu      16[rsi], xmm2
        movdqu      32[rsi], xmm3
        movdqu      48[rsi], xmm4
        movdqu      64[rsi], xmm5
        movdqu      80[rsi], xmm6
        movdqu      96[rsi], xmm7
        movdqu      112[rsi], xmm8
        add         rdi, 128
        dec         rcx
        jne         DLOOP_8
        add         rsi, 128
DREMAINDER_8:
        cmp         r10, 0 
        je          DEND_8
DLOOP_8_2:
        movdqu      xmm1, [rdi]
        movdqa      xmm10, xmm1
        add         rdi, 16
        pxor        xmm1, [r8]
        movdqu      xmm2, 160[r8]
        cmp         r9d, 12
        aesdec      xmm1, 16[r8]
        aesdec      xmm1, 32[r8]
        aesdec      xmm1, 48[r8]
        aesdec      xmm1, 64[r8]
        aesdec      xmm1, 80[r8]
        aesdec      xmm1, 96[r8]
        aesdec      xmm1, 112[r8]
        aesdec      xmm1, 128[r8]
        aesdec      xmm1, 144[r8]
        jb          DLAST_8_2
        movdqu      xmm2, 192[r8]
        cmp         r9d, 14
        aesdec      xmm1, 160[r8]
        aesdec      xmm1, 176[r8]
        jb          DLAST_8_2
        movdqu      xmm2, 224[r8]
        aesdec      xmm1, 192[r8]
        aesdec      xmm1, 208[r8]
DLAST_8_2:
        aesdeclast  xmm1, xmm2
        pxor        xmm1, xmm9
        movdqa      xmm9, xmm10
        movdqu      [rsi], xmm1
        add         rsi, 16
        dec         r10
        jne         DLOOP_8_2
DEND_8:
        ; restore non volatile rdi,rsi
        mov         rdi, rax
        mov         rsi, r11
        ; restore non volatile xmms from stack
        movdqa      xmm6, [rsp+0]
        movdqa      xmm7, [rsp+16]
        movdqa      xmm8, [rsp+32]
        movdqa      xmm9, [rsp+48]
        movdqa      xmm10, [rsp+64]
        movdqa      xmm11, [rsp+80]
        movdqa      xmm12, [rsp+96]
        movdqa      xmm13, [rsp+112]
        add         rsp, 8+8*16 ; 8 = align stack , 8 xmm6-13 16 bytes each
        ret
AES_CBC_decrypt_by8 ENDP


;	/*
;	AES_ECB_encrypt[const	,unsigned	char*in
;	unsigned	,char*out
;	unsigned	,long	length
;	const	,unsigned	char*KS
;	int	nr]
;	*/
;	.	globl	AES_ECB_encrypt
AES_ECB_encrypt PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi
;#	parameter	3:	rdx
;#	parameter	4:	rcx
;#	parameter	5:	r8d

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
    mov rdi,rcx
	mov rsi,rdx
	mov rdx,r8
	mov rcx,r9
	mov r8d,[rsp+40]

; on microsoft xmm6-xmm15 are non volaitle, let's save on stack and restore at end
	sub rsp,8+4*16  ; 8 = align stack , 4 xmm9-12, 16 bytes each
	movdqa [rsp+0], xmm9
	movdqa [rsp+16], xmm10
	movdqa [rsp+32], xmm11
	movdqa [rsp+48], xmm12


	mov	r10,rdx
	shr	rdx,4
	shl	r10,60
	je	EECB_NO_PARTS_4
	add	rdx,1
EECB_NO_PARTS_4:
	mov	r10,rdx
	shl	r10,62
	shr	r10,62
	shr	rdx,2
	je	EECB_REMAINDER_4
	sub	rsi,64
EECB_LOOP_4:
	movdqu  xmm1,[rdi]
	movdqu	xmm2,16[rdi]
	movdqu	xmm3,32[rdi]
	movdqu	xmm4,48[rdi]
	movdqa  xmm9,[rcx]
	movdqa	xmm10,16[rcx]
	movdqa	xmm11,32[rcx]
	movdqa	xmm12,48[rcx]
	pxor	xmm1,xmm9
	pxor	xmm2,xmm9
	pxor	xmm3,xmm9
	pxor	xmm4,xmm9
	aesenc	xmm1,xmm10
	aesenc	xmm2,xmm10
	aesenc	xmm3,xmm10
	aesenc	xmm4,xmm10
	aesenc	xmm1,xmm11
	aesenc	xmm2,xmm11
	aesenc	xmm3,xmm11
	aesenc	xmm4,xmm11
	aesenc	xmm1,xmm12
	aesenc	xmm2,xmm12
	aesenc	xmm3,xmm12
	aesenc	xmm4,xmm12
	movdqa	xmm9,64[rcx]
	movdqa	xmm10,80[rcx]
	movdqa	xmm11,96[rcx]
	movdqa	xmm12,112[rcx]
	aesenc	xmm1,xmm9
	aesenc	xmm2,xmm9
	aesenc	xmm3,xmm9
	aesenc	xmm4,xmm9
	aesenc	xmm1,xmm10
	aesenc	xmm2,xmm10
	aesenc	xmm3,xmm10
	aesenc	xmm4,xmm10
	aesenc	xmm1,xmm11
	aesenc	xmm2,xmm11
	aesenc	xmm3,xmm11
	aesenc	xmm4,xmm11
	aesenc	xmm1,xmm12
	aesenc	xmm2,xmm12
	aesenc	xmm3,xmm12
	aesenc	xmm4,xmm12
	movdqa	xmm9,128[rcx]
	movdqa	xmm10,144[rcx]
	movdqa	xmm11,160[rcx]
	cmp	r8d,12
	aesenc	xmm1,xmm9
	aesenc	xmm2,xmm9
	aesenc	xmm3,xmm9
	aesenc	xmm4,xmm9
	aesenc	xmm1,xmm10
	aesenc	xmm2,xmm10
	aesenc	xmm3,xmm10
	aesenc	xmm4,xmm10
	jb	EECB_LAST_4
	movdqa	xmm9,160[rcx]
	movdqa	xmm10,176[rcx]
	movdqa	xmm11,192[rcx]
	cmp	r8d,14
	aesenc	xmm1,xmm9
	aesenc	xmm2,xmm9
	aesenc	xmm3,xmm9
	aesenc	xmm4,xmm9
	aesenc	xmm1,xmm10
	aesenc	xmm2,xmm10
	aesenc	xmm3,xmm10
	aesenc	xmm4,xmm10
	jb	EECB_LAST_4
	movdqa	xmm9,192[rcx]
	movdqa	xmm10,208[rcx]
	movdqa	xmm11,224[rcx]
	aesenc	xmm1,xmm9
	aesenc	xmm2,xmm9
	aesenc	xmm3,xmm9
	aesenc	xmm4,xmm9
	aesenc	xmm1,xmm10
	aesenc	xmm2,xmm10
	aesenc	xmm3,xmm10
	aesenc	xmm4,xmm10
EECB_LAST_4:
	add	rdi,64
	add	rsi,64
	dec	rdx
	aesenclast	xmm1,xmm11
	aesenclast	xmm2,xmm11
	aesenclast	xmm3,xmm11
	aesenclast	xmm4,xmm11
	movdqu	[rsi],xmm1
	movdqu	16[rsi],xmm2
	movdqu	32[rsi],xmm3
	movdqu	48[rsi],xmm4
	jne	EECB_LOOP_4
	add	rsi,64
EECB_REMAINDER_4:
	cmp	r10,0
	je	EECB_END_4
EECB_LOOP_4_2:
	movdqu  xmm1,[rdi]
	add	rdi,16
	pxor	xmm1,[rcx]
	movdqu	xmm2,160[rcx]
	aesenc	xmm1,16[rcx]
	aesenc	xmm1,32[rcx]
	aesenc	xmm1,48[rcx]
	aesenc	xmm1,64[rcx]
	aesenc	xmm1,80[rcx]
	aesenc	xmm1,96[rcx]
	aesenc	xmm1,112[rcx]
	aesenc	xmm1,128[rcx]
	aesenc	xmm1,144[rcx]
	cmp	r8d,12
	jb	EECB_LAST_4_2
	movdqu	xmm2,192[rcx]
	aesenc	xmm1,160[rcx]
	aesenc	xmm1,176[rcx]
	cmp	r8d,14
	jb	EECB_LAST_4_2
	movdqu	xmm2,224[rcx]
	aesenc	xmm1,192[rcx]
	aesenc	xmm1,208[rcx]
EECB_LAST_4_2:
	aesenclast	xmm1,xmm2
	movdqu	[rsi],xmm1
	add	rsi,16
	dec	r10
	jne	EECB_LOOP_4_2
EECB_END_4:
	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
	; restore non volatile xmms from stack
	movdqa xmm9, [rsp+0]
	movdqa xmm10, [rsp+16]
	movdqa xmm11, [rsp+32]
	movdqa xmm12, [rsp+48]
	add rsp,8+4*16 ; 8 = align stack , 4 xmm9-12 16 bytes each
	ret
AES_ECB_encrypt ENDP

;	/*
;	AES_ECB_decrypt[const	,unsigned	char*in
;	unsigned	,char*out
;	unsigned	,long	length
;	const	,unsigned	char*KS
;	int	nr]
;	*/
;	.	globl	AES_ECB_decrypt
AES_ECB_decrypt PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi
;#	parameter	3:	rdx
;#	parameter	4:	rcx
;#	parameter	5:	r8d

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
	mov rdi,rcx
	mov rsi,rdx
	mov rdx,r8
	mov rcx,r9
	mov r8d,[rsp+40]

; on microsoft xmm6-xmm15 are non volaitle, let's save on stack and restore at end
	sub rsp,8+4*16  ; 8 = align stack , 4 xmm9-12, 16 bytes each
	movdqa [rsp+0], xmm9
	movdqa [rsp+16], xmm10
	movdqa [rsp+32], xmm11
	movdqa [rsp+48], xmm12

	mov	r10,rdx
	shr	rdx,4
	shl	r10,60
	je	DECB_NO_PARTS_4
	add	rdx,1
DECB_NO_PARTS_4:
	mov	r10,rdx
	shl	r10,62
	shr	r10,62
	shr	rdx,2
	je	DECB_REMAINDER_4
	sub	rsi,64
DECB_LOOP_4:
	movdqu  xmm1,[rdi]
	movdqu	xmm2,16[rdi]
	movdqu	xmm3,32[rdi]
	movdqu	xmm4,48[rdi]
	movdqa  xmm9,[rcx]
	movdqa	xmm10,16[rcx]
	movdqa	xmm11,32[rcx]
	movdqa	xmm12,48[rcx]
	pxor	xmm1,xmm9
	pxor	xmm2,xmm9
	pxor	xmm3,xmm9
	pxor	xmm4,xmm9
	aesdec	xmm1,xmm10
	aesdec	xmm2,xmm10
	aesdec	xmm3,xmm10
	aesdec	xmm4,xmm10
	aesdec	xmm1,xmm11
	aesdec	xmm2,xmm11
	aesdec	xmm3,xmm11
	aesdec	xmm4,xmm11
	aesdec	xmm1,xmm12
	aesdec	xmm2,xmm12
	aesdec	xmm3,xmm12
	aesdec	xmm4,xmm12
	movdqa	xmm9,64[rcx]
	movdqa	xmm10,80[rcx]
	movdqa	xmm11,96[rcx]
	movdqa	xmm12,112[rcx]
	aesdec	xmm1,xmm9
	aesdec	xmm2,xmm9
	aesdec	xmm3,xmm9
	aesdec	xmm4,xmm9
	aesdec	xmm1,xmm10
	aesdec	xmm2,xmm10
	aesdec	xmm3,xmm10
	aesdec	xmm4,xmm10
	aesdec	xmm1,xmm11
	aesdec	xmm2,xmm11
	aesdec	xmm3,xmm11
	aesdec	xmm4,xmm11
	aesdec	xmm1,xmm12
	aesdec	xmm2,xmm12
	aesdec	xmm3,xmm12
	aesdec	xmm4,xmm12
	movdqa	xmm9,128[rcx]
	movdqa	xmm10,144[rcx]
	movdqa	xmm11,160[rcx]
	cmp	r8d,12
	aesdec	xmm1,xmm9
	aesdec	xmm2,xmm9
	aesdec	xmm3,xmm9
	aesdec	xmm4,xmm9
	aesdec	xmm1,xmm10
	aesdec	xmm2,xmm10
	aesdec	xmm3,xmm10
	aesdec	xmm4,xmm10
	jb	DECB_LAST_4
	movdqa	xmm9,160[rcx]
	movdqa	xmm10,176[rcx]
	movdqa	xmm11,192[rcx]
	cmp	r8d,14
	aesdec	xmm1,xmm9
	aesdec	xmm2,xmm9
	aesdec	xmm3,xmm9
	aesdec	xmm4,xmm9
	aesdec	xmm1,xmm10
	aesdec	xmm2,xmm10
	aesdec	xmm3,xmm10
	aesdec	xmm4,xmm10
	jb	DECB_LAST_4
	movdqa	xmm9,192[rcx]
	movdqa	xmm10,208[rcx]
	movdqa	xmm11,224[rcx]
	aesdec	xmm1,xmm9
	aesdec	xmm2,xmm9
	aesdec	xmm3,xmm9
	aesdec	xmm4,xmm9
	aesdec	xmm1,xmm10
	aesdec	xmm2,xmm10
	aesdec	xmm3,xmm10
	aesdec	xmm4,xmm10
DECB_LAST_4:
	add	rdi,64
	add	rsi,64
	dec	rdx
	aesdeclast	xmm1,xmm11
	aesdeclast	xmm2,xmm11
	aesdeclast	xmm3,xmm11
	aesdeclast	xmm4,xmm11
	movdqu	[rsi],xmm1
	movdqu	16[rsi],xmm2
	movdqu	32[rsi],xmm3
	movdqu	48[rsi],xmm4
	jne	DECB_LOOP_4
	add	rsi,64
DECB_REMAINDER_4:
	cmp	r10,0
	je	DECB_END_4
DECB_LOOP_4_2:
	movdqu  xmm1,[rdi]
	add	rdi,16
	pxor	xmm1,[rcx]
	movdqu	xmm2,160[rcx]
	cmp	r8d,12
	aesdec	xmm1,16[rcx]
	aesdec	xmm1,32[rcx]
	aesdec	xmm1,48[rcx]
	aesdec	xmm1,64[rcx]
	aesdec	xmm1,80[rcx]
	aesdec	xmm1,96[rcx]
	aesdec	xmm1,112[rcx]
	aesdec	xmm1,128[rcx]
	aesdec	xmm1,144[rcx]
	jb	DECB_LAST_4_2
	cmp	r8d,14
	movdqu	xmm2,192[rcx]
	aesdec	xmm1,160[rcx]
	aesdec	xmm1,176[rcx]
	jb	DECB_LAST_4_2
	movdqu	xmm2,224[rcx]
	aesdec	xmm1,192[rcx]
	aesdec	xmm1,208[rcx]
DECB_LAST_4_2:
	aesdeclast	xmm1,xmm2
	movdqu	[rsi],xmm1
	add	rsi,16
	dec	r10
	jne	DECB_LOOP_4_2
DECB_END_4:
	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
	; restore non volatile xmms from stack
	movdqa xmm9, [rsp+0]
	movdqa xmm10, [rsp+16]
	movdqa xmm11, [rsp+32]
	movdqa xmm12, [rsp+48]
	add rsp,8+4*16 ; 8 = align stack , 4 xmm9-12 16 bytes each
	ret
AES_ECB_decrypt ENDP



;	/*
;	void	,AES_128_Key_Expansion[const	unsigned	char*userkey
;	unsigned	char*key_schedule]/
;	*/
;	.	align	16,0x90
;	.	globl	AES_128_Key_Expansion
AES_128_Key_Expansion PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
	mov rdi,rcx
	mov rsi,rdx

	mov	dword ptr 240[rsi],10

	movdqu	xmm1,[rdi]
	movdqa	[rsi],xmm1


ASSISTS:
	aeskeygenassist	xmm2,xmm1,1
	call	PREPARE_ROUNDKEY_128
	movdqa	16[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,2
	call	PREPARE_ROUNDKEY_128
	movdqa	32[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,4
	call	PREPARE_ROUNDKEY_128
	movdqa	48[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,8
	call	PREPARE_ROUNDKEY_128
	movdqa	64[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,16
	call	PREPARE_ROUNDKEY_128
	movdqa	80[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,32
	call	PREPARE_ROUNDKEY_128
	movdqa	96[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,64
	call	PREPARE_ROUNDKEY_128
	movdqa	112[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,80h
	call	PREPARE_ROUNDKEY_128
	movdqa	128[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,1bh
	call	PREPARE_ROUNDKEY_128
	movdqa	144[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,36h
	call	PREPARE_ROUNDKEY_128
	movdqa	160[rsi],xmm1
	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
	ret

PREPARE_ROUNDKEY_128:
	pshufd	xmm2,xmm2,255
	movdqa	xmm3,xmm1
	pslldq	xmm3,4
	pxor	xmm1,xmm3
	pslldq	xmm3,4
	pxor	xmm1,xmm3
	pslldq	xmm3,4
	pxor	xmm1,xmm3
	pxor	xmm1,xmm2
	ret
AES_128_Key_Expansion ENDP

;	/*
;	void	,AES_192_Key_Expansion[const	unsigned	char*userkey
;	unsigned	char*key]
;	*/
;	.	globl	AES_192_Key_Expansion
AES_192_Key_Expansion PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
    mov rdi,rcx
	mov rsi,rdx

; on microsoft xmm6-xmm15 are non volaitle, let's save on stack and restore at end
	sub rsp,8+1*16  ; 8 = align stack , 1 xmm6, 16 bytes each
	movdqa [rsp+0], xmm6

	movdqu  xmm1,[rdi]
	movq	xmm3,qword ptr 16[rdi]
	movdqa	[rsi],xmm1
	movdqa	xmm5,xmm3

	aeskeygenassist	xmm2,xmm3,1h
	call	PREPARE_ROUNDKEY_192
	shufpd	xmm5,xmm1,0
	movdqa	16[rsi],xmm5
	movdqa	xmm6,xmm1
	shufpd	xmm6,xmm3,1
	movdqa	32[rsi],xmm6

	aeskeygenassist	xmm2,xmm3,2h
	call	PREPARE_ROUNDKEY_192
	movdqa	48[rsi],xmm1
	movdqa	xmm5,xmm3

	aeskeygenassist	xmm2,xmm3,4h
	call	PREPARE_ROUNDKEY_192
	shufpd	xmm5,xmm1,0
	movdqa	64[rsi],xmm5
	movdqa	xmm6,xmm1
	shufpd	xmm6,xmm3,1
	movdqa	80[rsi],xmm6

	aeskeygenassist	xmm2,xmm3,8h
	call	PREPARE_ROUNDKEY_192
	movdqa	96[rsi],xmm1
	movdqa	xmm5,xmm3

	aeskeygenassist	xmm2,xmm3,10h
	call	PREPARE_ROUNDKEY_192
	shufpd	xmm5,xmm1,0
	movdqa	112[rsi],xmm5
	movdqa	xmm6,xmm1
	shufpd	xmm6,xmm3,1
	movdqa	128[rsi],xmm6

	aeskeygenassist	xmm2,xmm3,20h
	call	PREPARE_ROUNDKEY_192
	movdqa	144[rsi],xmm1
	movdqa	xmm5,xmm3

	aeskeygenassist	xmm2,xmm3,40h
	call	PREPARE_ROUNDKEY_192
	shufpd	xmm5,xmm1,0
	movdqa	160[rsi],xmm5
	movdqa	xmm6,xmm1
	shufpd	xmm6,xmm3,1
	movdqa	176[rsi],xmm6

	aeskeygenassist	xmm2,xmm3,80h
	call	PREPARE_ROUNDKEY_192
	movdqa	192[rsi],xmm1
	movdqa	208[rsi],xmm3
	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
; restore non volatile xmms from stack
	movdqa xmm6, [rsp+0]
	add rsp,8+1*16 ; 8 = align stack , 1 xmm6 16 bytes each
	ret

PREPARE_ROUNDKEY_192:
	pshufd	xmm2,xmm2,55h
	movdqu	xmm4,xmm1
	pslldq	xmm4,4
	pxor	xmm1,xmm4

	pslldq	xmm4,4
	pxor	xmm1,xmm4
	pslldq	xmm4,4
	pxor	xmm1,xmm4
	pxor	xmm1,xmm2
	pshufd	xmm2,xmm1,0ffh
	movdqu	xmm4,xmm3
	pslldq	xmm4,4
	pxor	xmm3,xmm4
	pxor	xmm3,xmm2
	ret
AES_192_Key_Expansion ENDP

;	/*
;	void	,AES_256_Key_Expansion[const	unsigned	char*userkey
;	unsigned	char*key]
;	*/
;	.	globl	AES_256_Key_Expansion
AES_256_Key_Expansion PROC
;#	parameter	1:	rdi
;#	parameter	2:	rsi

; save rdi and rsi to rax and r11, restore before ret
	mov rax,rdi
	mov r11,rsi

; convert to what we had for att&t convention
    mov rdi,rcx
	mov rsi,rdx

	movdqu  xmm1,[rdi]
	movdqu	xmm3,16[rdi]
	movdqa	[rsi],xmm1
	movdqa	16[rsi],xmm3

	aeskeygenassist	xmm2,xmm3,1h
	call	MAKE_RK256_a
	movdqa	32[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	48[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,2h
	call	MAKE_RK256_a
	movdqa	64[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	80[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,4h
	call	MAKE_RK256_a
	movdqa	96[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	112[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,8h
	call	MAKE_RK256_a
	movdqa	128[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	144[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,10h
	call	MAKE_RK256_a
	movdqa	160[rsi],xmm1
	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	176[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,20h
	call	MAKE_RK256_a
	movdqa	192[rsi],xmm1

	aeskeygenassist	xmm2,xmm1,0h
	call	MAKE_RK256_b
	movdqa	208[rsi],xmm3
	aeskeygenassist	xmm2,xmm3,40h
	call	MAKE_RK256_a
	movdqa	224[rsi],xmm1

	; restore non volatile rdi,rsi
	mov rdi,rax
	mov rsi,r11
	ret
AES_256_Key_Expansion ENDP

MAKE_RK256_a:
	pshufd	xmm2,xmm2,0ffh
	movdqa	xmm4,xmm1
	pslldq	xmm4,4
	pxor	xmm1,xmm4
	pslldq	xmm4,4
	pxor	xmm1,xmm4
	pslldq	xmm4,4
	pxor	xmm1,xmm4
	pxor	xmm1,xmm2
	ret

MAKE_RK256_b:
	pshufd	xmm2,xmm2,0aah
	movdqa	xmm4,xmm3
	pslldq	xmm4,4
	pxor	xmm3,xmm4
	pslldq	xmm4,4
	pxor	xmm3,xmm4
	pslldq	xmm4,4
	pxor	xmm3,xmm4
	pxor	xmm3,xmm2
	ret


IF fips_version GE 2
  fipsAh ENDS
ELSE
  _text ENDS
ENDIF

END
