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
#include <FreeRTOSConfig.h>

	RSEG    CODE:CODE(2)
	thumb

	EXTERN pxCurrentTCB
	EXTERN vTaskSwitchContext
	EXTERN vPortSVCHandler_C

	PUBLIC xPortPendSVHandler
	PUBLIC vPortSVCHandler
	PUBLIC vPortStartFirstTask
	PUBLIC vPortEnableVFP
	PUBLIC vPortRestoreContextOfFirstTask
	PUBLIC xIsPrivileged
	PUBLIC vResetPrivilege

/*-----------------------------------------------------------*/

xPortPendSVHandler:
	mrs r0, psp
	isb
	/* Get the location of the current TCB. */
	ldr	r3, =pxCurrentTCB
	ldr	r2, [r3]

	/* Is the task using the FPU context?  If so, push high vfp registers. */
	tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16-s31}

	/* Save the core registers. */
	mrs r1, control
	stmdb r0!, {r1, r4-r11, r14}

	/* Save the new top of stack into the first member of the TCB. */
	str r0, [r2]

	stmdb sp!, {r0, r3}
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	dsb
	isb
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r0, r3}

	/* The first item in pxCurrentTCB is the task top of stack. */
	ldr r1, [r3]
	ldr r0, [r1]
	/* Move onto the second item in the TCB... */
	add r1, r1, #4

	dmb					/* Complete outstanding transfers before disabling MPU. */
	ldr r2, =0xe000ed94	/* MPU_CTRL register. */
	ldr r3, [r2]		/* Read the value of MPU_CTRL. */
	bic r3, r3, #1		/* r3 = r3 & ~1 i.e. Clear the bit 0 in r3. */
	str r3, [r2]		/* Disable MPU. */

	/* Region Base Address register. */
	ldr r2, =0xe000ed9c
	/* Read 4 sets of MPU registers [MPU Region # 4 - 7]. */
	ldmia r1!, {r4-r11}
	/* Write 4 sets of MPU registers [MPU Region # 4 - 7]. */
	stmia r2, {r4-r11}

	#ifdef configTOTAL_MPU_REGIONS
		#if ( configTOTAL_MPU_REGIONS == 16 )
			/* Read 4 sets of MPU registers [MPU Region # 8 - 11]. */
			ldmia r1!, {r4-r11}
			/* Write 4 sets of MPU registers. [MPU Region # 8 - 11]. */
			stmia r2, {r4-r11}
			/* Read 4 sets of MPU registers [MPU Region # 12 - 15]. */
			ldmia r1!, {r4-r11}
			/* Write 4 sets of MPU registers. [MPU Region # 12 - 15]. */
			stmia r2, {r4-r11}
		#endif /* configTOTAL_MPU_REGIONS == 16. */
	#endif /* configTOTAL_MPU_REGIONS */

	ldr r2, =0xe000ed94	/* MPU_CTRL register. */
	ldr r3, [r2]		/* Read the value of MPU_CTRL. */
	orr r3, r3, #1		/* r3 = r3 | 1 i.e. Set the bit 0 in r3. */
	str r3, [r2]		/* Enable MPU. */
	dsb					/* Force memory writes before continuing. */

	/* Pop the registers that are not automatically saved on exception entry. */
	ldmia r0!, {r3-r11, r14}
	msr control, r3

	/* Is the task using the FPU context?  If so, pop the high vfp registers
	too. */
	tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}

	msr psp, r0
	isb

	bx r14


/*-----------------------------------------------------------*/

vPortSVCHandler:
	#ifndef USE_PROCESS_STACK	/* Code should not be required if a main() is using the process stack. */
		tst lr, #4
		ite eq
		mrseq r0, msp
		mrsne r0, psp
	#else
		mrs r0, psp
	#endif
		b vPortSVCHandler_C

/*-----------------------------------------------------------*/

vPortStartFirstTask:
	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Clear the bit that indicates the FPU is in use in case the FPU was used
	before the scheduler was started - which would otherwise result in the
	unnecessary leaving of space in the SVC stack for lazy saving of FPU
	registers. */
	mov r0, #0
	msr control, r0
	/* Call SVC to start the first task. */
	cpsie i
	cpsie f
	dsb
	isb
	svc 0

/*-----------------------------------------------------------*/

vPortRestoreContextOfFirstTask:
	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Restore the context. */
	ldr	r3, =pxCurrentTCB
	ldr r1, [r3]
	/* The first item in the TCB is the task top of stack. */
	ldr r0, [r1]
	/* Move onto the second item in the TCB... */
	add r1, r1, #4

	dmb					/* Complete outstanding transfers before disabling MPU. */
	ldr r2, =0xe000ed94	/* MPU_CTRL register. */
	ldr r3, [r2]		/* Read the value of MPU_CTRL. */
	bic r3, r3, #1		/* r3 = r3 & ~1 i.e. Clear the bit 0 in r3. */
	str r3, [r2]		/* Disable MPU. */

	/* Region Base Address register. */
	ldr r2, =0xe000ed9c
	/* Read 4 sets of MPU registers [MPU Region # 4 - 7]. */
	ldmia r1!, {r4-r11}
	/* Write 4 sets of MPU registers [MPU Region # 4 - 7]. */
	stmia r2, {r4-r11}

	#ifdef configTOTAL_MPU_REGIONS
		#if ( configTOTAL_MPU_REGIONS == 16 )
			/* Read 4 sets of MPU registers [MPU Region # 8 - 11]. */
			ldmia r1!, {r4-r11}
			/* Write 4 sets of MPU registers. [MPU Region # 8 - 11]. */
			stmia r2, {r4-r11}
			/* Read 4 sets of MPU registers [MPU Region # 12 - 15]. */
			ldmia r1!, {r4-r11}
			/* Write 4 sets of MPU registers. [MPU Region # 12 - 15]. */
			stmia r2, {r4-r11}
		#endif /* configTOTAL_MPU_REGIONS == 16. */
	#endif /* configTOTAL_MPU_REGIONS */

	ldr r2, =0xe000ed94	/* MPU_CTRL register. */
	ldr r3, [r2]		/* Read the value of MPU_CTRL. */
	orr r3, r3, #1		/* r3 = r3 | 1 i.e. Set the bit 0 in r3. */
	str r3, [r2]		/* Enable MPU. */
	dsb					/* Force memory writes before continuing. */

	/* Pop the registers that are not automatically saved on exception entry. */
	ldmia r0!, {r3-r11, r14}
	msr control, r3
	/* Restore the task stack pointer. */
	msr psp, r0
	mov r0, #0
	msr	basepri, r0
	bx r14

/*-----------------------------------------------------------*/

vPortEnableVFP:
	/* The FPU enable bits are in the CPACR. */
	ldr.w r0, =0xE000ED88
	ldr	r1, [r0]

	/* Enable CP10 and CP11 coprocessors, then save back. */
	orr	r1, r1, #( 0xf << 20 )
	str r1, [r0]
	bx	r14

/*-----------------------------------------------------------*/

xIsPrivileged:
	mrs r0, control		/* r0 = CONTROL. */
	tst r0, #1			/* Perform r0 & 1 (bitwise AND) and update the conditions flag. */
	ite ne
	movne r0, #0		/* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
	moveq r0, #1		/* CONTROL[0]==0. Return true to indicate that the processor is privileged. */
	bx lr				/* Return. */
/*-----------------------------------------------------------*/

vResetPrivilege:
	mrs r0, control		/* r0 = CONTROL. */
	orr r0, r0, #1		/* r0 = r0 | 1. */
	msr control, r0		/* CONTROL = r0. */
	bx lr				/* Return to the caller. */
/*-----------------------------------------------------------*/

	END
