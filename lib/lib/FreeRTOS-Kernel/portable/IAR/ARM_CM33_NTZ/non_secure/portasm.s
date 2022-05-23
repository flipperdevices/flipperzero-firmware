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
/* Including FreeRTOSConfig.h here will cause build errors if the header file
contains code not understood by the assembler - for example the 'extern' keyword.
To avoid errors place any such code inside a #ifdef __ICCARM__/#endif block so
the code is included in C files but excluded by the preprocessor in assembly
files (__ICCARM__ is defined by the IAR C compiler but not by the IAR assembler. */
#include "FreeRTOSConfig.h"

	EXTERN pxCurrentTCB
	EXTERN vTaskSwitchContext
	EXTERN vPortSVCHandler_C

	PUBLIC xIsPrivileged
	PUBLIC vResetPrivilege
	PUBLIC vRestoreContextOfFirstTask
	PUBLIC vRaisePrivilege
	PUBLIC vStartFirstTask
	PUBLIC ulSetInterruptMask
	PUBLIC vClearInterruptMask
	PUBLIC PendSV_Handler
	PUBLIC SVC_Handler
/*-----------------------------------------------------------*/

/*---------------- Unprivileged Functions -------------------*/

/*-----------------------------------------------------------*/

	SECTION .text:CODE:NOROOT(2)
	THUMB
/*-----------------------------------------------------------*/

xIsPrivileged:
	mrs r0, control							/* r0 = CONTROL. */
	tst r0, #1								/* Perform r0 & 1 (bitwise AND) and update the conditions flag. */
	ite ne
	movne r0, #0							/* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
	moveq r0, #1							/* CONTROL[0]==0. Return true to indicate that the processor is not privileged. */
	bx lr									/* Return. */
/*-----------------------------------------------------------*/

vResetPrivilege:
	mrs r0, control							/* r0 = CONTROL. */
	orr r0, r0, #1							/* r0 = r0 | 1. */
	msr control, r0							/* CONTROL = r0. */
	bx lr									/* Return to the caller. */
/*-----------------------------------------------------------*/

/*----------------- Privileged Functions --------------------*/

/*-----------------------------------------------------------*/

	SECTION privileged_functions:CODE:NOROOT(2)
	THUMB
/*-----------------------------------------------------------*/

vRestoreContextOfFirstTask:
	ldr  r2, =pxCurrentTCB					/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	ldr  r1, [r2]							/* Read pxCurrentTCB. */
	ldr  r0, [r1]							/* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */

#if ( configENABLE_MPU == 1 )
	dmb										/* Complete outstanding transfers before disabling MPU. */
	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r4, [r2]							/* Read the value of MPU_CTRL. */
	bic r4, r4, #1							/* r4 = r4 & ~1 i.e. Clear the bit 0 in r4. */
	str r4, [r2]							/* Disable MPU. */

	adds r1, #4								/* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	ldr r3, [r1]							/* r3 = *r1 i.e. r3 = MAIR0. */
	ldr r2, =0xe000edc0						/* r2 = 0xe000edc0 [Location of MAIR0]. */
	str r3, [r2]							/* Program MAIR0. */
	ldr r2, =0xe000ed98						/* r2 = 0xe000ed98 [Location of RNR]. */
	movs r3, #4								/* r3 = 4. */
	str r3, [r2]							/* Program RNR = 4. */
	adds r1, #4								/* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	ldr r2, =0xe000ed9c						/* r2 = 0xe000ed9c [Location of RBAR]. */
	ldmia r1!, {r4-r11}						/* Read 4 sets of RBAR/RLAR registers from TCB. */
	stmia r2!, {r4-r11}						/* Write 4 set of RBAR/RLAR registers using alias registers. */

	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r4, [r2]							/* Read the value of MPU_CTRL. */
	orr r4, r4, #1							/* r4 = r4 | 1 i.e. Set the bit 0 in r4. */
	str r4, [r2]							/* Enable MPU. */
	dsb										/* Force memory writes before continuing. */
#endif /* configENABLE_MPU */

#if ( configENABLE_MPU == 1 )
	ldm  r0!, {r1-r3}						/* Read from stack - r1 = PSPLIM, r2 = CONTROL and r3 = EXC_RETURN. */
	msr  psplim, r1							/* Set this task's PSPLIM value. */
	msr  control, r2						/* Set this task's CONTROL value. */
	adds r0, #32							/* Discard everything up to r0. */
	msr  psp, r0							/* This is now the new top of stack to use in the task. */
	isb
	mov  r0, #0
	msr  basepri, r0						/* Ensure that interrupts are enabled when the first task starts. */
	bx   r3									/* Finally, branch to EXC_RETURN. */
#else /* configENABLE_MPU */
	ldm  r0!, {r1-r2}						/* Read from stack - r1 = PSPLIM and r2 = EXC_RETURN. */
	msr  psplim, r1							/* Set this task's PSPLIM value. */
	movs r1, #2								/* r1 = 2. */
	msr  CONTROL, r1						/* Switch to use PSP in the thread mode. */
	adds r0, #32							/* Discard everything up to r0. */
	msr  psp, r0							/* This is now the new top of stack to use in the task. */
	isb
	mov  r0, #0
	msr  basepri, r0						/* Ensure that interrupts are enabled when the first task starts. */
	bx   r2									/* Finally, branch to EXC_RETURN. */
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

vRaisePrivilege:
	mrs  r0, control						/* Read the CONTROL register. */
	bic r0, r0, #1							/* Clear the bit 0. */
	msr  control, r0						/* Write back the new CONTROL value. */
	bx lr									/* Return to the caller. */
/*-----------------------------------------------------------*/

vStartFirstTask:
	ldr r0, =0xe000ed08						/* Use the NVIC offset register to locate the stack. */
	ldr r0, [r0]							/* Read the VTOR register which gives the address of vector table. */
	ldr r0, [r0]							/* The first entry in vector table is stack pointer. */
	msr msp, r0								/* Set the MSP back to the start of the stack. */
	cpsie i									/* Globally enable interrupts. */
	cpsie f
	dsb
	isb
	svc 2									/* System call to start the first task. portSVC_START_SCHEDULER = 2. */
/*-----------------------------------------------------------*/

ulSetInterruptMask:
	mrs r0, basepri							/* r0 = basepri. Return original basepri value. */
	mov r1, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r1							/* Disable interrupts upto configMAX_SYSCALL_INTERRUPT_PRIORITY. */
	dsb
	isb
	bx lr									/* Return. */
/*-----------------------------------------------------------*/

vClearInterruptMask:
	msr basepri, r0							/* basepri = ulMask. */
	dsb
	isb
	bx lr									/* Return. */
/*-----------------------------------------------------------*/

PendSV_Handler:
	mrs r0, psp								/* Read PSP in r0. */
#if ( configENABLE_FPU == 1 )
	tst lr, #0x10							/* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	it eq
	vstmdbeq r0!, {s16-s31}					/* Store the FPU registers which are not saved automatically. */
#endif /* configENABLE_FPU */
#if ( configENABLE_MPU == 1 )
	mrs r1, psplim							/* r1 = PSPLIM. */
	mrs r2, control							/* r2 = CONTROL. */
	mov r3, lr								/* r3 = LR/EXC_RETURN. */
	stmdb r0!, {r1-r11}						/* Store on the stack - PSPLIM, CONTROL, LR and registers that are not automatically saved. */
#else /* configENABLE_MPU */
	mrs r2, psplim							/* r2 = PSPLIM. */
	mov r3, lr								/* r3 = LR/EXC_RETURN. */
	stmdb r0!, {r2-r11}						/* Store on the stack - PSPLIM, LR and registers that are not automatically. */
#endif /* configENABLE_MPU */

	ldr r2, =pxCurrentTCB					/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	ldr r1, [r2]							/* Read pxCurrentTCB. */
	str r0, [r1]							/* Save the new top of stack in TCB. */

	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0							/* Disable interrupts upto configMAX_SYSCALL_INTERRUPT_PRIORITY. */
	dsb
	isb
	bl vTaskSwitchContext
	mov r0, #0								/* r0 = 0. */
	msr basepri, r0							/* Enable interrupts. */

	ldr r2, =pxCurrentTCB					/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	ldr r1, [r2]							/* Read pxCurrentTCB. */
	ldr r0, [r1]							/* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */

#if ( configENABLE_MPU == 1 )
	dmb										/* Complete outstanding transfers before disabling MPU. */
	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r4, [r2]							/* Read the value of MPU_CTRL. */
	bic r4, r4, #1							/* r4 = r4 & ~1 i.e. Clear the bit 0 in r4. */
	str r4, [r2]							/* Disable MPU. */

	adds r1, #4								/* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	ldr r3, [r1]							/* r3 = *r1 i.e. r3 = MAIR0. */
	ldr r2, =0xe000edc0						/* r2 = 0xe000edc0 [Location of MAIR0]. */
	str r3, [r2]							/* Program MAIR0. */
	ldr r2, =0xe000ed98						/* r2 = 0xe000ed98 [Location of RNR]. */
	movs r3, #4								/* r3 = 4. */
	str r3, [r2]							/* Program RNR = 4. */
	adds r1, #4								/* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	ldr r2, =0xe000ed9c						/* r2 = 0xe000ed9c [Location of RBAR]. */
	ldmia r1!, {r4-r11}						/* Read 4 sets of RBAR/RLAR registers from TCB. */
	stmia r2!, {r4-r11}						/* Write 4 set of RBAR/RLAR registers using alias registers. */

	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r4, [r2]							/* Read the value of MPU_CTRL. */
	orr r4, r4, #1							/* r4 = r4 | 1 i.e. Set the bit 0 in r4. */
	str r4, [r2]							/* Enable MPU. */
	dsb										/* Force memory writes before continuing. */
#endif /* configENABLE_MPU */

#if ( configENABLE_MPU == 1 )
	ldmia r0!, {r1-r11}						/* Read from stack - r1 = PSPLIM, r2 = CONTROL, r3 = LR and r4-r11 restored. */
#else /* configENABLE_MPU */
	ldmia r0!, {r2-r11}						/* Read from stack - r2 = PSPLIM, r3 = LR and r4-r11 restored. */
#endif /* configENABLE_MPU */

#if ( configENABLE_FPU == 1 )
	tst r3, #0x10							/* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	it eq
	vldmiaeq r0!, {s16-s31}					/* Restore the FPU registers which are not restored automatically. */
#endif /* configENABLE_FPU */

 #if ( configENABLE_MPU == 1 )
	msr psplim, r1							/* Restore the PSPLIM register value for the task. */
	msr control, r2							/* Restore the CONTROL register value for the task. */
#else /* configENABLE_MPU */
	msr psplim, r2							/* Restore the PSPLIM register value for the task. */
#endif /* configENABLE_MPU */
	msr psp, r0								/* Remember the new top of stack for the task. */
	bx r3
/*-----------------------------------------------------------*/

SVC_Handler:
	tst lr, #4
	ite eq
	mrseq r0, msp
	mrsne r0, psp
	b vPortSVCHandler_C
/*-----------------------------------------------------------*/

	END
