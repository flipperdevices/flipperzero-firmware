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


	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern ulMaxSyscallInterruptPriorityConst

	.global _vector_14
	.global _lc_ref__vector_pp_14
	.global SVC_Handler
	.global vPortStartFirstTask
	.global vPortEnableVFP
	.global ulPortSetInterruptMask
	.global vPortClearInterruptMask

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
_vector_14: .type func

	mrs r0, psp
	isb

	;Get the location of the current TCB.
	ldr.w	r3, =pxCurrentTCB
	ldr	r2, [r3]

	;Is the task using the FPU context?  If so, push high vfp registers.
	tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16-s31}

	;Save the core registers.
	stmdb r0!, {r4-r11, r14}

	;Save the new top of stack into the first member of the TCB.
	str r0, [r2]

	stmdb sp!, {r0, r3}
	ldr.w r0, =ulMaxSyscallInterruptPriorityConst
	ldr r0, [r0]
	msr basepri, r0
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r0, r3}

	;The first item in pxCurrentTCB is the task top of stack.
	ldr r1, [r3]
	ldr r0, [r1]

	;Pop the core registers.
	ldmia r0!, {r4-r11, r14}

	;Is the task using the FPU context?  If so, pop the high vfp registers too.
	tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}

	msr psp, r0
	isb
	bx r14

	.size	_vector_14, $-_vector_14
	.endsec

;-----------------------------------------------------------

; This function is an XMC4000 silicon errata workaround.  It will get used when
; the SILICON_BUG_PMC_CM_001 linker macro is defined.
	.section .text
	.thumb
	.align 4
_lc_ref__vector_pp_14: .type func

	mrs r0, psp
	isb

	;Get the location of the current TCB.
	ldr.w	r3, =pxCurrentTCB
	ldr	r2, [r3]

	;Is the task using the FPU context?  If so, push high vfp registers.
	tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16-s31}

	;Save the core registers.
	stmdb r0!, {r4-r11, r14}

	;Save the new top of stack into the first member of the TCB.
	str r0, [r2]

	stmdb sp!, {r3}
	ldr.w r0, =ulMaxSyscallInterruptPriorityConst
	ldr r0, [r0]
	msr basepri, r0
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3}

	;The first item in pxCurrentTCB is the task top of stack.
	ldr r1, [r3]
	ldr r0, [r1]

	;Pop the core registers.
	ldmia r0!, {r4-r11, r14}

	;Is the task using the FPU context?  If so, pop the high vfp registers too.
	tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}

	msr psp, r0
	isb
	push { lr }
	pop { pc } ; XMC4000 specific errata workaround.  Do not used "bx lr" here.

	.size	_lc_ref__vector_pp_14, $-_lc_ref__vector_pp_14
	.endsec

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
SVC_Handler: .type func
	;Get the location of the current TCB.
	ldr.w	r3, =pxCurrentTCB
	ldr r1, [r3]
	ldr r0, [r1]
	;Pop the core registers.
	ldmia r0!, {r4-r11, r14}
	msr psp, r0
	isb
	mov r0, #0
	msr	basepri, r0
	bx r14
	.size	SVC_Handler, $-SVC_Handler
	.endsec

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
vPortStartFirstTask .type func
	;Use the NVIC offset register to locate the stack.
	ldr.w r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	;Set the msp back to the start of the stack.
	msr msp, r0
	;Call SVC to start the first task.
	cpsie i
	cpsie f
	dsb
	isb
	svc 0
	.size	vPortStartFirstTask, $-vPortStartFirstTask
	.endsec

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
vPortEnableVFP .type func
	;The FPU enable bits are in the CPACR.
	ldr.w r0, =0xE000ED88
	ldr	r1, [r0]

	;Enable CP10 and CP11 coprocessors, then save back.
	orr	r1, r1, #( 0xf << 20 )
	str r1, [r0]
	bx	r14
	.size	vPortEnableVFP, $-vPortEnableVFP
	.endsec

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
ulPortSetInterruptMask:
	mrs r0, basepri
	ldr.w r1, =ulMaxSyscallInterruptPriorityConst
	ldr r1, [r1]
	msr basepri, r1
	bx r14
	.size	ulPortSetInterruptMask, $-ulPortSetInterruptMask
	.endsec

;-----------------------------------------------------------

	.section .text
	.thumb
	.align 4
vPortClearInterruptMask:
	msr basepri, r0
	bx r14
	.size	vPortClearInterruptMask, $-vPortClearInterruptMask
	.endsec

;-----------------------------------------------------------

	.end

