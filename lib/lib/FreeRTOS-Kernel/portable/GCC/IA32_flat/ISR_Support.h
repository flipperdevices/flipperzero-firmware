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

	.extern ulTopOfSystemStack
	.extern ulInterruptNesting

/*-----------------------------------------------------------*/

.macro portFREERTOS_INTERRUPT_ENTRY

	/* Save general purpose registers. */
	pusha

	/* If ulInterruptNesting is zero the rest of the task context will need
	saving and a stack switch might be required. */
	movl	ulInterruptNesting, %eax
	test	%eax, %eax
	jne		2f

	/* Interrupts are not nested, so save the rest of the task context. */
	.if configSUPPORT_FPU == 1

		/* If the task has a buffer allocated to save the FPU context then
		save the FPU context now. */
		movl	pucPortTaskFPUContextBuffer, %eax
		test	%eax, %eax
		je		1f
		fnsave	( %eax ) /* Save FLOP context into ucTempFPUBuffer array. */
		fwait

		1:
		/* Save the address of the FPU context, if any. */
		push	pucPortTaskFPUContextBuffer

	.endif /* configSUPPORT_FPU */

	/* Find the TCB. */
	movl 	pxCurrentTCB, %eax

	/* Stack location is first item in the TCB. */
	movl	%esp, (%eax)

	/* Switch stacks. */
	movl 	ulTopOfSystemStack, %esp
	movl	%esp, %ebp

	2:
	/* Increment nesting count. */
	add 	$1, ulInterruptNesting

.endm
/*-----------------------------------------------------------*/

.macro portINTERRUPT_EPILOGUE

	cli
	sub		$1, ulInterruptNesting

	/* If the nesting has unwound to zero. */
	movl	ulInterruptNesting, %eax
	test	%eax, %eax
	jne		2f

	/* If a yield was requested then select a new TCB now. */
	movl	ulPortYieldPending, %eax
	test	%eax, %eax
	je		1f
	movl	$0, ulPortYieldPending
	call	vTaskSwitchContext

	1:
	/* Stack location is first item in the TCB. */
	movl 	pxCurrentTCB, %eax
	movl	(%eax), %esp

	.if configSUPPORT_FPU == 1

		/* Restore address of task's FPU context buffer. */
		pop 	pucPortTaskFPUContextBuffer

		/* If the task has a buffer allocated in which its FPU context is saved,
		then restore it now. */
		movl	pucPortTaskFPUContextBuffer, %eax
		test	%eax, %eax
		je		1f
		frstor	( %eax )
		1:
	.endif

	2:
	popa

.endm
/*-----------------------------------------------------------*/

.macro portFREERTOS_INTERRUPT_EXIT

	portINTERRUPT_EPILOGUE
	/* EOI. */
	movl	$0x00, (0xFEE000B0)
	iret

.endm
