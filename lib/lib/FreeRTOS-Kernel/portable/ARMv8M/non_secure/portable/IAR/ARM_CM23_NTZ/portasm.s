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

#if ( configENABLE_FPU == 1 )
	#error Cortex-M23 does not have a Floating Point Unit (FPU) and therefore configENABLE_FPU must be set to 0.
#endif
/*-----------------------------------------------------------*/

/*---------------- Unprivileged Functions -------------------*/

/*-----------------------------------------------------------*/

	SECTION .text:CODE:NOROOT(2)
	THUMB
/*-----------------------------------------------------------*/

xIsPrivileged:
	mrs r0, control							/* r0 = CONTROL. */
	movs r1, #1								/* r1 = 1. */
	tst r0, r1								/* Perform r0 & r1 (bitwise AND) and update the conditions flag. */
	beq running_privileged					/* If the result of previous AND operation was 0, branch. */
	movs r0, #0								/* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
	bx lr									/* Return. */
	running_privileged:
		movs r0, #1							/* CONTROL[0]==0. Return true to indicate that the processor is privileged. */
		bx lr								/* Return. */

/*-----------------------------------------------------------*/

vResetPrivilege:
	mrs r0, control							/* r0 = CONTROL. */
	movs r1, #1								/* r1 = 1. */
	orrs r0, r1								/* r0 = r0 | r1. */
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
	ldr r3, [r2]							/* Read the value of MPU_CTRL. */
	movs r4, #1								/* r4 = 1. */
	bics r3, r4								/* r3 = r3 & ~r4 i.e. Clear the bit 0 in r3. */
	str r3, [r2]							/* Disable MPU. */

	adds r1, #4								/* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	ldr  r4, [r1]							/* r4 = *r1 i.e. r4 = MAIR0. */
	ldr  r2, =0xe000edc0					/* r2 = 0xe000edc0 [Location of MAIR0]. */
	str  r4, [r2]							/* Program MAIR0. */
	ldr  r2, =0xe000ed98					/* r2 = 0xe000ed98 [Location of RNR]. */
	adds r1, #4								/* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	movs r4, #4								/* r4 = 4. */
	str  r4, [r2]							/* Program RNR = 4. */
	ldmia r1!, {r5,r6}						/* Read first set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write first set of RBAR/RLAR registers. */
	movs r4, #5								/* r4 = 5. */
	str  r4, [r2]							/* Program RNR = 5. */
	ldmia r1!, {r5,r6}						/* Read second set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write second set of RBAR/RLAR registers. */
	movs r4, #6								/* r4 = 6. */
	str  r4, [r2]							/* Program RNR = 6. */
	ldmia r1!, {r5,r6}						/* Read third set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write third set of RBAR/RLAR registers. */
	movs r4, #7								/* r4 = 7. */
	str  r4, [r2]							/* Program RNR = 7. */
	ldmia r1!, {r5,r6}						/* Read fourth set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write fourth set of RBAR/RLAR registers. */

	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r3, [r2]							/* Read the value of MPU_CTRL. */
	movs r4, #1								/* r4 = 1. */
	orrs r3, r4								/* r3 = r3 | r4 i.e. Set the bit 0 in r3. */
	str r3, [r2]							/* Enable MPU. */
	dsb										/* Force memory writes before continuing. */
#endif /* configENABLE_MPU */

#if ( configENABLE_MPU == 1 )
	ldm  r0!, {r1-r3}						/* Read from stack - r1 = PSPLIM, r2 = CONTROL and r3 = EXC_RETURN. */
	msr  psplim, r1							/* Set this task's PSPLIM value. */
	msr  control, r2						/* Set this task's CONTROL value. */
	adds r0, #32							/* Discard everything up to r0. */
	msr  psp, r0							/* This is now the new top of stack to use in the task. */
	isb
	bx   r3									/* Finally, branch to EXC_RETURN. */
#else /* configENABLE_MPU */
	ldm  r0!, {r1-r2}						/* Read from stack - r1 = PSPLIM and r2 = EXC_RETURN. */
	msr  psplim, r1							/* Set this task's PSPLIM value. */
	movs r1, #2								/* r1 = 2. */
	msr  CONTROL, r1						/* Switch to use PSP in the thread mode. */
	adds r0, #32							/* Discard everything up to r0. */
	msr  psp, r0							/* This is now the new top of stack to use in the task. */
	isb
	bx   r2									/* Finally, branch to EXC_RETURN. */
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

vRaisePrivilege:
	mrs  r0, control						/* Read the CONTROL register. */
	movs r1, #1								/* r1 = 1. */
	bics r0, r1								/* Clear the bit 0. */
	msr  control, r0						/* Write back the new CONTROL value. */
	bx lr									/* Return to the caller. */
/*-----------------------------------------------------------*/

vStartFirstTask:
	ldr r0, =0xe000ed08						/* Use the NVIC offset register to locate the stack. */
	ldr r0, [r0]							/* Read the VTOR register which gives the address of vector table. */
	ldr r0, [r0]							/* The first entry in vector table is stack pointer. */
	msr msp, r0								/* Set the MSP back to the start of the stack. */
	cpsie i									/* Globally enable interrupts. */
	dsb
	isb
	svc 2									/* System call to start the first task. portSVC_START_SCHEDULER = 2. */
	nop
/*-----------------------------------------------------------*/

ulSetInterruptMask:
	mrs r0, PRIMASK
	cpsid i
	bx lr
/*-----------------------------------------------------------*/

vClearInterruptMask:
	msr PRIMASK, r0
	bx lr
/*-----------------------------------------------------------*/

PendSV_Handler:
	mrs r0, psp								/* Read PSP in r0. */
	ldr r2, =pxCurrentTCB					/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	ldr r1, [r2]							/* Read pxCurrentTCB. */
#if ( configENABLE_MPU == 1 )
	subs r0, r0, #44						/* Make space for PSPLIM, CONTROL, LR and the remaining registers on the stack. */
	str r0, [r1]							/* Save the new top of stack in TCB. */
	mrs r1, psplim							/* r1 = PSPLIM. */
	mrs r2, control							/* r2 = CONTROL. */
	mov r3, lr								/* r3 = LR/EXC_RETURN. */
	stmia r0!, {r1-r7}						/* Store on the stack - PSPLIM, CONTROL, LR and low registers that are not automatically saved. */
	mov r4, r8								/* r4 = r8. */
	mov r5, r9								/* r5 = r9. */
	mov r6, r10								/* r6 = r10. */
	mov r7, r11								/* r7 = r11. */
	stmia r0!, {r4-r7}						/* Store the high registers that are not saved automatically. */
#else /* configENABLE_MPU */
	subs r0, r0, #40						/* Make space for PSPLIM, LR and the remaining registers on the stack. */
	str r0, [r1]							/* Save the new top of stack in TCB. */
	mrs r2, psplim							/* r2 = PSPLIM. */
	mov r3, lr								/* r3 = LR/EXC_RETURN. */
	stmia r0!, {r2-r7}						/* Store on the stack - PSPLIM, LR and low registers that are not automatically saved. */
	mov r4, r8								/* r4 = r8. */
	mov r5, r9								/* r5 = r9. */
	mov r6, r10								/* r6 = r10. */
	mov r7, r11								/* r7 = r11. */
	stmia r0!, {r4-r7}						/* Store the high registers that are not saved automatically. */
#endif /* configENABLE_MPU */

	cpsid i
	bl vTaskSwitchContext
	cpsie i

	ldr r2, =pxCurrentTCB					/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	ldr r1, [r2]							/* Read pxCurrentTCB. */
	ldr r0, [r1]							/* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */

#if ( configENABLE_MPU == 1 )
	dmb										/* Complete outstanding transfers before disabling MPU. */
	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r3, [r2]							/* Read the value of MPU_CTRL. */
	movs r4, #1								/* r4 = 1. */
	bics r3, r4								/* r3 = r3 & ~r4 i.e. Clear the bit 0 in r3. */
	str r3, [r2]							/* Disable MPU. */

	adds r1, #4								/* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	ldr  r4, [r1]							/* r4 = *r1 i.e. r4 = MAIR0. */
	ldr  r2, =0xe000edc0					/* r2 = 0xe000edc0 [Location of MAIR0]. */
	str  r4, [r2]							/* Program MAIR0. */
	ldr  r2, =0xe000ed98					/* r2 = 0xe000ed98 [Location of RNR]. */
	adds r1, #4								/* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	movs r4, #4								/* r4 = 4. */
	str  r4, [r2]							/* Program RNR = 4. */
	ldmia r1!, {r5,r6}						/* Read first set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write first set of RBAR/RLAR registers. */
	movs r4, #5								/* r4 = 5. */
	str  r4, [r2]							/* Program RNR = 5. */
	ldmia r1!, {r5,r6}						/* Read second set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write second set of RBAR/RLAR registers. */
	movs r4, #6								/* r4 = 6. */
	str  r4, [r2]							/* Program RNR = 6. */
	ldmia r1!, {r5,r6}						/* Read third set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write third set of RBAR/RLAR registers. */
	movs r4, #7								/* r4 = 7. */
	str  r4, [r2]							/* Program RNR = 7. */
	ldmia r1!, {r5,r6}						/* Read fourth set of RBAR/RLAR from TCB. */
	ldr  r3, =0xe000ed9c					/* r3 = 0xe000ed9c [Location of RBAR]. */
	stmia r3!, {r5,r6}						/* Write fourth set of RBAR/RLAR registers. */

	ldr r2, =0xe000ed94						/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
	ldr r3, [r2]							/* Read the value of MPU_CTRL. */
	movs r4, #1								/* r4 = 1. */
	orrs r3, r4								/* r3 = r3 | r4 i.e. Set the bit 0 in r3. */
	str r3, [r2]							/* Enable MPU. */
	dsb										/* Force memory writes before continuing. */
#endif /* configENABLE_MPU */

#if ( configENABLE_MPU == 1 )
	adds r0, r0, #28						/* Move to the high registers. */
	ldmia r0!, {r4-r7}						/* Restore the high registers that are not automatically restored. */
	mov r8, r4								/* r8 = r4. */
	mov r9, r5								/* r9 = r5. */
	mov r10, r6								/* r10 = r6. */
	mov r11, r7								/* r11 = r7. */
	msr psp, r0								/* Remember the new top of stack for the task. */
	subs r0, r0, #44						/* Move to the starting of the saved context. */
	ldmia r0!, {r1-r7}						/* Read from stack - r1 = PSPLIM, r2 = CONTROL, r3 = LR and r4-r7 restored. */
	msr psplim, r1							/* Restore the PSPLIM register value for the task. */
	msr control, r2							/* Restore the CONTROL register value for the task. */
	bx r3
#else /* configENABLE_MPU */
	adds r0, r0, #24						/* Move to the high registers. */
	ldmia r0!, {r4-r7}						/* Restore the high registers that are not automatically restored. */
	mov r8, r4								/* r8 = r4. */
	mov r9, r5								/* r9 = r5. */
	mov r10, r6								/* r10 = r6. */
	mov r11, r7								/* r11 = r7. */
	msr psp, r0								/* Remember the new top of stack for the task. */
	subs r0, r0, #40						/* Move to the starting of the saved context. */
	ldmia r0!, {r2-r7}						/* Read from stack - r2 = PSPLIM, r3 = LR and r4-r7 restored. */
	msr psplim, r2							/* Restore the PSPLIM register value for the task. */
	bx r3
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

SVC_Handler:
	movs r0, #4
	mov r1, lr
	tst r0, r1
	beq stacking_used_msp
	mrs r0, psp
	b vPortSVCHandler_C
	stacking_used_msp:
		mrs r0, msp
		b vPortSVCHandler_C
/*-----------------------------------------------------------*/

	END
