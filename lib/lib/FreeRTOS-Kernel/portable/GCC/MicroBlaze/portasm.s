/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */
	.extern pxCurrentTCB
	.extern vTaskISRHandler
	.extern vTaskSwitchContext
	.extern uxCriticalNesting
	.extern pulISRStack

	.global __FreeRTOS_interrupt_handler
	.global VPortYieldASM
	.global vStartFirstTask


.macro portSAVE_CONTEXT
	/* Make room for the context on the stack. */
	addik r1, r1, -132
	/* Save r31 so it can then be used. */
	swi r31, r1, 4
	/* Copy the msr into r31 - this is stacked later. */
	mfs r31, rmsr
	/* Stack general registers. */
	swi r30, r1, 12
	swi r29, r1, 16
	swi r28, r1, 20
	swi r27, r1, 24
	swi r26, r1, 28
	swi r25, r1, 32
	swi r24, r1, 36
	swi r23, r1, 40
	swi r22, r1, 44
	swi r21, r1, 48
	swi r20, r1, 52
	swi r19, r1, 56
	swi r18, r1, 60
	swi r17, r1, 64
	swi r16, r1, 68
	swi r15, r1, 72
	swi r13, r1, 80
	swi r12, r1, 84
	swi r11, r1, 88
	swi r10, r1, 92
	swi r9, r1, 96
	swi r8, r1, 100
	swi r7, r1, 104
	swi r6, r1, 108
	swi r5, r1, 112
	swi r4, r1, 116
	swi r3, r1, 120
	swi r2, r1, 124
	/* Stack the critical section nesting value. */
	lwi r3, r0, uxCriticalNesting
	swi r3, r1, 128
	/* Save the top of stack value to the TCB. */
	lwi r3, r0, pxCurrentTCB
	sw	r1, r0, r3
	
	.endm

.macro portRESTORE_CONTEXT
	/* Load the top of stack value from the TCB. */
	lwi r3, r0, pxCurrentTCB
	lw	r1, r0, r3	
	/* Restore the general registers. */
	lwi r31, r1, 4		
	lwi r30, r1, 12		
	lwi r29, r1, 16	
	lwi r28, r1, 20	
	lwi r27, r1, 24	
	lwi r26, r1, 28	
	lwi r25, r1, 32	
	lwi r24, r1, 36	
	lwi r23, r1, 40	
	lwi r22, r1, 44	
	lwi r21, r1, 48	
	lwi r20, r1, 52	
	lwi r19, r1, 56	
	lwi r18, r1, 60	
	lwi r17, r1, 64	
	lwi r16, r1, 68	
	lwi r15, r1, 72	
	lwi r14, r1, 76	
	lwi r13, r1, 80	
	lwi r12, r1, 84	
	lwi r11, r1, 88	
	lwi r10, r1, 92	
	lwi r9, r1, 96	
	lwi r8, r1, 100	
	lwi r7, r1, 104
	lwi r6, r1, 108
	lwi r5, r1, 112
	lwi r4, r1, 116
	lwi r2, r1, 124

	/* Load the critical nesting value. */
	lwi r3, r1, 128
	swi r3, r0, uxCriticalNesting

	/* Obtain the MSR value from the stack. */
	lwi r3, r1, 8

	/* Are interrupts enabled in the MSR?  If so return using an return from 
	interrupt instruction to ensure interrupts are enabled only once the task
	is running again. */
	andi r3, r3, 2
	beqid r3, 36
	or r0, r0, r0

	/* Reload the rmsr from the stack, clear the enable interrupt bit in the
	value before saving back to rmsr register, then return enabling interrupts
	as we return. */
	lwi r3, r1, 8
	andi r3, r3, ~2
	mts rmsr, r3
	lwi r3, r1, 120
	addik r1, r1, 132
	rtid r14, 0
	or r0, r0, r0

	/* Reload the rmsr from the stack, place it in the rmsr register, and
	return without enabling interrupts. */
	lwi r3, r1, 8
	mts rmsr, r3
	lwi r3, r1, 120
	addik r1, r1, 132
	rtsd r14, 0
	or r0, r0, r0

	.endm

	.text
	.align  2


__FreeRTOS_interrupt_handler:
	portSAVE_CONTEXT
	/* Entered via an interrupt so interrupts must be enabled in msr. */
	ori r31, r31, 2
	/* Stack msr. */
	swi r31, r1, 8
	/* Stack the return address.  As we entered via an interrupt we do
	not need to modify the return address prior to stacking. */
	swi r14, r1, 76
	/* Now switch to use the ISR stack. */
	lwi r3, r0, pulISRStack
	add r1, r3, r0
	bralid r15, vTaskISRHandler
	or r0, r0, r0
	portRESTORE_CONTEXT


VPortYieldASM:
	portSAVE_CONTEXT
	/* Stack msr. */
	swi r31, r1, 8
	/* Modify the return address so we return to the instruction after the
	exception. */
	addi r14, r14, 8
	swi r14, r1, 76
	/* Now switch to use the ISR stack. */
	lwi r3, r0, pulISRStack
	add r1, r3, r0
	bralid r15, vTaskSwitchContext
	or r0, r0, r0
	portRESTORE_CONTEXT

vStartFirstTask:
	portRESTORE_CONTEXT
	
	




