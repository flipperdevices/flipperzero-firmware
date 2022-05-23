;/*
; * FreeRTOS Kernel <DEVELOPMENT BRANCH>
; * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
; *
; * SPDX-License-Identifier: MIT
; *
; * Permission is hereby granted, free of charge, to any person obtaining a copy of
; * this software and associated documentation files (the "Software"), to deal in
; * the Software without restriction, including without limitation the rights to
; * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
; * the Software, and to permit persons to whom the Software is furnished to do so,
; * subject to the following conditions:
; *
; * The above copyright notice and this permission notice shall be included in all
; * copies or substantial portions of the Software.
; *
; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
; * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
; * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
; * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; *
; * https://www.FreeRTOS.org
; * https://github.com/FreeRTOS
; *
; */
	.macro portSAVE_CONTEXT

	; Save r0 to r14 and pr.
	movml.l r15, @-r15

	; Save mac1, mach and gbr
	sts.l	macl, @-r15
	sts.l	mach, @-r15
	stc.l	gbr, @-r15

	; Get the address of pxCurrentTCB
	mov.l	#_pxCurrentTCB, r0

	; Get the address of pxTopOfStack from the TCB.
	mov.l	@r0, r0

	; Save the stack pointer in pxTopOfStack.
	mov.l	r15, @r0

	.endm

;-----------------------------------------------------------

	.macro portRESTORE_CONTEXT

	; Get the address of the pxCurrentTCB variable.
	mov.l	#_pxCurrentTCB, r0

	; Get the address of the task stack from pxCurrentTCB.
	mov.l	@r0, r0

	; Get the task stack itself into the stack pointer.
	mov.l	@r0, r15

	; Restore system registers.
	ldc.l	@r15+, gbr
	lds.l	@r15+, mach
	lds.l	@r15+, macl

	; Restore r0 to r14 and PR
	movml.l	@r15+, r15

	; Pop the SR and PC to jump to the start of the task.
	rte
	nop

	.endm
;-----------------------------------------------------------
