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

	.thumb

	.ref pxCurrentTCB
	.ref vTaskSwitchContext
	.ref ulMaxSyscallInterruptPriority

	.def xPortPendSVHandler
	.def ulPortGetIPSR
	.def vPortSVCHandler
	.def vPortStartFirstTask

NVICOffsetConst:					.word 	0xE000ED08
CPACRConst:							.word 	0xE000ED88
pxCurrentTCBConst:					.word	pxCurrentTCB
ulMaxSyscallInterruptPriorityConst: .word ulMaxSyscallInterruptPriority

; -----------------------------------------------------------

	.align 4
ulPortGetIPSR: .asmfunc
 	mrs r0, ipsr
 	bx r14
 	.endasmfunc
 ; -----------------------------------------------------------

	.align 4
vPortSetInterruptMask: .asmfunc
	push {r0}
	ldr r0, ulMaxSyscallInterruptPriorityConst
	msr basepri, r0
	pop {r0}
	bx r14
	.endasmfunc
; -----------------------------------------------------------

	.align 4
xPortPendSVHandler: .asmfunc
	mrs r0, psp
	isb

	;/* Get the location of the current TCB. */
	ldr	r3, pxCurrentTCBConst
	ldr	r2, [r3]

	;/* Save the core registers. */
	stmdb r0!, {r4-r11}

	;/* Save the new top of stack into the first member of the TCB. */
	str r0, [r2]

	stmdb sp!, {r3, r14}
	ldr r0, ulMaxSyscallInterruptPriorityConst
	ldr r1, [r0]
	msr basepri, r1
	dsb
	isb
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	;/* The first item in pxCurrentTCB is the task top of stack. */
	ldr r1, [r3]
	ldr r0, [r1]

	;/* Pop the core registers. */
	ldmia r0!, {r4-r11}

	msr psp, r0
	isb
	bx r14
	.endasmfunc

; -----------------------------------------------------------

	.align 4
vPortSVCHandler: .asmfunc
	;/* Get the location of the current TCB. */
	ldr	r3, pxCurrentTCBConst
	ldr r1, [r3]
	ldr r0, [r1]
	;/* Pop the core registers. */
	ldmia r0!, {r4-r11}
	msr psp, r0
	isb
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd
	bx r14
	.endasmfunc

; -----------------------------------------------------------

	.align 4
vPortStartFirstTask: .asmfunc
	;/* Use the NVIC offset register to locate the stack. */
	ldr r0, NVICOffsetConst
	ldr r0, [r0]
	ldr r0, [r0]
	;/* Set the msp back to the start of the stack. */
	msr msp, r0
	;/* Clear the bit that indicates the FPU is in use in case the FPU was used
	;before the scheduler was started - which would otherwise result in the
	;unnecessary leaving of space in the SVC stack for lazy saving of FPU
	;registers. */
	mov r0, #0
	msr control, r0
	;/* Call SVC to start the first task. */
	cpsie i
	cpsie f
	dsb
	isb
	svc #0
	.endasmfunc

; -----------------------------------------------------------

