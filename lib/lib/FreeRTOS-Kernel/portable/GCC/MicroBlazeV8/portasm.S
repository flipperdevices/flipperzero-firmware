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

/* FreeRTOS includes. */
#include "FreeRTOSConfig.h"

/* Xilinx library includes. */
#include "microblaze_exceptions_g.h"
#include "xparameters.h"

/* The context is oversized to allow functions called from the ISR to write
back into the caller stack. */
#if( XPAR_MICROBLAZE_USE_FPU != 0 )
	#define portCONTEXT_SIZE 136
	#define portMINUS_CONTEXT_SIZE -136
#else
	#define portCONTEXT_SIZE 132
	#define portMINUS_CONTEXT_SIZE -132
#endif

/* Offsets from the stack pointer at which saved registers are placed. */
#define portR31_OFFSET	4
#define portR30_OFFSET	8
#define portR29_OFFSET	12
#define portR28_OFFSET	16
#define portR27_OFFSET	20
#define portR26_OFFSET	24
#define portR25_OFFSET	28
#define portR24_OFFSET	32
#define portR23_OFFSET	36
#define portR22_OFFSET	40
#define portR21_OFFSET	44
#define portR20_OFFSET	48
#define portR19_OFFSET	52
#define portR18_OFFSET	56
#define portR17_OFFSET	60
#define portR16_OFFSET	64
#define portR15_OFFSET	68
#define portR14_OFFSET	72
#define portR13_OFFSET	76
#define portR12_OFFSET	80
#define portR11_OFFSET	84
#define portR10_OFFSET	88
#define portR9_OFFSET	92
#define portR8_OFFSET	96
#define portR7_OFFSET	100
#define portR6_OFFSET	104
#define portR5_OFFSET	108
#define portR4_OFFSET	112
#define portR3_OFFSET	116
#define portR2_OFFSET	120
#define portCRITICAL_NESTING_OFFSET 124
#define portMSR_OFFSET 128
#define portFSR_OFFSET 132

	.extern pxCurrentTCB
	.extern XIntc_DeviceInterruptHandler
	.extern vTaskSwitchContext
	.extern uxCriticalNesting
	.extern pulISRStack
	.extern ulTaskSwitchRequested
	.extern vPortExceptionHandler
	.extern pulStackPointerOnFunctionEntry

	.global _interrupt_handler
	.global VPortYieldASM
	.global vPortStartFirstTask
	.global vPortExceptionHandlerEntry


.macro portSAVE_CONTEXT

	/* Make room for the context on the stack. */
	addik r1, r1, portMINUS_CONTEXT_SIZE

	/* Stack general registers. */
	swi r31, r1, portR31_OFFSET
	swi r30, r1, portR30_OFFSET
	swi r29, r1, portR29_OFFSET
	swi r28, r1, portR28_OFFSET
	swi r27, r1, portR27_OFFSET
	swi r26, r1, portR26_OFFSET
	swi r25, r1, portR25_OFFSET
	swi r24, r1, portR24_OFFSET
	swi r23, r1, portR23_OFFSET
	swi r22, r1, portR22_OFFSET
	swi r21, r1, portR21_OFFSET
	swi r20, r1, portR20_OFFSET
	swi r19, r1, portR19_OFFSET
	swi r18, r1, portR18_OFFSET
	swi r17, r1, portR17_OFFSET
	swi r16, r1, portR16_OFFSET
	swi r15, r1, portR15_OFFSET
	/* R14 is saved later as it needs adjustment if a yield is performed. */
	swi r13, r1, portR13_OFFSET
	swi r12, r1, portR12_OFFSET
	swi r11, r1, portR11_OFFSET
	swi r10, r1, portR10_OFFSET
	swi r9, r1, portR9_OFFSET
	swi r8, r1, portR8_OFFSET
	swi r7, r1, portR7_OFFSET
	swi r6, r1, portR6_OFFSET
	swi r5, r1, portR5_OFFSET
	swi r4, r1, portR4_OFFSET
	swi r3, r1, portR3_OFFSET
	swi r2, r1, portR2_OFFSET

	/* Stack the critical section nesting value. */
	lwi r18, r0, uxCriticalNesting
	swi r18, r1, portCRITICAL_NESTING_OFFSET

	/* Stack MSR. */
	mfs r18, rmsr
	swi r18, r1, portMSR_OFFSET

	#if( XPAR_MICROBLAZE_USE_FPU != 0 )
		/* Stack FSR. */
		mfs r18, rfsr
		swi r18, r1, portFSR_OFFSET
	#endif

	/* Save the top of stack value to the TCB. */
	lwi r3, r0, pxCurrentTCB
	sw	r1, r0, r3

	.endm

.macro portRESTORE_CONTEXT

	/* Load the top of stack value from the TCB. */
	lwi r18, r0, pxCurrentTCB
	lw	r1, r0, r18

	/* Restore the general registers. */
	lwi r31, r1, portR31_OFFSET
	lwi r30, r1, portR30_OFFSET
	lwi r29, r1, portR29_OFFSET
	lwi r28, r1, portR28_OFFSET
	lwi r27, r1, portR27_OFFSET
	lwi r26, r1, portR26_OFFSET
	lwi r25, r1, portR25_OFFSET
	lwi r24, r1, portR24_OFFSET
	lwi r23, r1, portR23_OFFSET
	lwi r22, r1, portR22_OFFSET
	lwi r21, r1, portR21_OFFSET
	lwi r20, r1, portR20_OFFSET
	lwi r19, r1, portR19_OFFSET
	lwi r17, r1, portR17_OFFSET
	lwi r16, r1, portR16_OFFSET
	lwi r15, r1, portR15_OFFSET
	lwi r14, r1, portR14_OFFSET
	lwi r13, r1, portR13_OFFSET
	lwi r12, r1, portR12_OFFSET
	lwi r11, r1, portR11_OFFSET
	lwi r10, r1, portR10_OFFSET
	lwi r9, r1, portR9_OFFSET
	lwi r8, r1, portR8_OFFSET
	lwi r7, r1, portR7_OFFSET
	lwi r6, r1, portR6_OFFSET
	lwi r5, r1, portR5_OFFSET
	lwi r4, r1, portR4_OFFSET
	lwi r3, r1, portR3_OFFSET
	lwi r2, r1, portR2_OFFSET

	/* Reload the rmsr from the stack. */
	lwi r18, r1, portMSR_OFFSET
	mts rmsr, r18

	#if( XPAR_MICROBLAZE_USE_FPU != 0 )
		/* Reload the FSR from the stack. */
		lwi r18, r1, portFSR_OFFSET
		mts rfsr, r18
	#endif

	/* Load the critical nesting value. */
	lwi r18, r1, portCRITICAL_NESTING_OFFSET
	swi r18, r0, uxCriticalNesting

	/* Test the critical nesting value.  If it is non zero then the task last
	exited the running state using a yield.  If it is zero, then the task
	last exited the running state through an interrupt. */
	xori r18, r18, 0
	bnei r18, exit_from_yield

	/* r18 was being used as a temporary.  Now restore its true value from the
	stack. */
	lwi r18, r1, portR18_OFFSET

	/* Remove the stack frame. */
	addik r1, r1, portCONTEXT_SIZE

	/* Return using rtid so interrupts are re-enabled as this function is
	exited. */
	rtid r14, 0
	or r0, r0, r0

	.endm

/* This function is used to exit portRESTORE_CONTEXT() if the task being
returned to last left the Running state by calling taskYIELD() (rather than
being preempted by an interrupt). */
	.text
	.align  4
exit_from_yield:

	/* r18 was being used as a temporary.  Now restore its true value from the
	stack. */
	lwi r18, r1, portR18_OFFSET

	/* Remove the stack frame. */
	addik r1, r1, portCONTEXT_SIZE

	/* Return to the task. */
	rtsd r14, 0
	or r0, r0, r0


	.text
	.align  4
_interrupt_handler:

	portSAVE_CONTEXT

	/* Stack the return address. */
	swi r14, r1, portR14_OFFSET

	/* Switch to the ISR stack. */
	lwi r1, r0, pulISRStack

	/* The parameter to the interrupt handler. */
	ori r5, r0, configINTERRUPT_CONTROLLER_TO_USE

	/* Execute any pending interrupts. */
	bralid r15, XIntc_DeviceInterruptHandler
	or r0, r0, r0

	/* See if a new task should be selected to execute. */
	lwi r18, r0, ulTaskSwitchRequested
	or r18, r18, r0

	/* If ulTaskSwitchRequested is already zero, then jump straight to
	restoring the task that is already in the Running state. */
	beqi r18, task_switch_not_requested

	/* Set ulTaskSwitchRequested back to zero as a task switch is about to be
	performed. */
	swi r0, r0, ulTaskSwitchRequested

	/* ulTaskSwitchRequested was not 0 when tested.  Select the next task to
	execute. */
	bralid r15, vTaskSwitchContext
	or r0, r0, r0

task_switch_not_requested:

	/* Restore the context of the next task scheduled to execute. */
	portRESTORE_CONTEXT


	.text
	.align  4
VPortYieldASM:

	portSAVE_CONTEXT

	/* Modify the return address so a return is done to the instruction after
	the call to VPortYieldASM. */
	addi r14, r14, 8
	swi r14, r1, portR14_OFFSET

	/* Switch to use the ISR stack. */
	lwi r1, r0, pulISRStack

	/* Select the next task to execute. */
	bralid r15, vTaskSwitchContext
	or r0, r0, r0

	/* Restore the context of the next task scheduled to execute. */
	portRESTORE_CONTEXT

	.text
	.align  4
vPortStartFirstTask:

	portRESTORE_CONTEXT



#if ( MICROBLAZE_EXCEPTIONS_ENABLED == 1 ) && ( configINSTALL_EXCEPTION_HANDLERS == 1 )

	.text
	.align 4
vPortExceptionHandlerEntry:

	/* Take a copy of the stack pointer before vPortExecptionHandler is called,
	storing its value prior to the function stack frame being created. */
	swi r1, r0, pulStackPointerOnFunctionEntry
	bralid r15, vPortExceptionHandler
	or r0, r0, r0

#endif /* ( MICROBLAZE_EXCEPTIONS_ENABLED == 1 ) && ( configINSTALL_EXCEPTION_HANDLERS == 1 ) */



