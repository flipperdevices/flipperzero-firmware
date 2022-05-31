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

#include "PriorityDefinitions.h"

	PUBLIC _prvStartFirstTask
	PUBLIC ___interrupt_27

	EXTERN _pxCurrentTCB
	EXTERN _vTaskSwitchContext

	RSEG CODE:CODE(4)

_prvStartFirstTask:

		/* When starting the scheduler there is nothing that needs moving to the
		interrupt stack because the function is not called from an interrupt.
		Just ensure the current stack is the user stack. */
		SETPSW		U

		/* Obtain the location of the stack associated with which ever task
		pxCurrentTCB is currently pointing to. */
		MOV.L		#_pxCurrentTCB, R15
		MOV.L		[R15], R15
		MOV.L		[R15], R0

		/* Restore the registers from the stack of the task pointed to by
		pxCurrentTCB. */
		POP			R15

		/* Accumulator low 32 bits. */
		MVTACLO		R15
		POP			R15

		/* Accumulator high 32 bits. */
		MVTACHI		R15
		POP			R15

		/* Floating point status word. */
		MVTC		R15, FPSW

		/* R1 to R15 - R0 is not included as it is the SP. */
		POPM		R1-R15

		/* This pops the remaining registers. */
		RTE
		NOP
		NOP

/*-----------------------------------------------------------*/

/* The software interrupt - overwrite the default 'weak' definition. */
___interrupt_27:

		/* Re-enable interrupts. */
		SETPSW		I

		/* Move the data that was automatically pushed onto the interrupt stack when
		the interrupt occurred from the interrupt stack to the user stack.

		R15 is saved before it is clobbered. */
		PUSH.L		R15

		/* Read the user stack pointer. */
		MVFC		USP, R15

		/* Move the address down to the data being moved. */
		SUB			#12, R15
		MVTC		R15, USP

		/* Copy the data across, R15, then PC, then PSW. */
		MOV.L		[ R0 ], [ R15 ]
		MOV.L 		4[ R0 ], 4[ R15 ]
		MOV.L		8[ R0 ], 8[ R15 ]

		/* Move the interrupt stack pointer to its new correct position. */
		ADD		#12, R0

		/* All the rest of the registers are saved directly to the user stack. */
		SETPSW		U

		/* Save the rest of the general registers (R15 has been saved already). */
		PUSHM		R1-R14

		/* Save the FPSW and accumulator. */
		MVFC		FPSW, R15
		PUSH.L		R15
		MVFACHI 	R15
		PUSH.L		R15

		/* Middle word. */
		MVFACMI	R15

		/* Shifted left as it is restored to the low order word. */
		SHLL		#16, R15
		PUSH.L		R15

		/* Save the stack pointer to the TCB. */
		MOV.L		#_pxCurrentTCB, R15
		MOV.L		[ R15 ], R15
		MOV.L		R0, [ R15 ]

		/* Ensure the interrupt mask is set to the syscall priority while the kernel
		structures are being accessed. */
		MVTIPL		#configMAX_SYSCALL_INTERRUPT_PRIORITY

		/* Select the next task to run. */
		BSR.A		_vTaskSwitchContext

		/* Reset the interrupt mask as no more data structure access is required. */
		MVTIPL		#configKERNEL_INTERRUPT_PRIORITY

		/* Load the stack pointer of the task that is now selected as the Running
		state task from its TCB. */
		MOV.L		#_pxCurrentTCB,R15
		MOV.L		[ R15 ], R15
		MOV.L		[ R15 ], R0

		/* Restore the context of the new task.  The PSW (Program Status Word) and
		PC will be popped by the RTE instruction. */
		POP			R15
		MVTACLO 	R15
		POP			R15
		MVTACHI 	R15
		POP			R15
		MVTC		R15, FPSW
		POPM		R1-R15
		RTE
		NOP
		NOP

/*-----------------------------------------------------------*/

		END

