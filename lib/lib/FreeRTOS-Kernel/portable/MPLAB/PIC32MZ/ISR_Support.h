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

#include "FreeRTOSConfig.h"

#define portCONTEXT_SIZE                160
#define portEPC_STACK_LOCATION          152
#define portSTATUS_STACK_LOCATION       156
#define portFPCSR_STACK_LOCATION        0
#define portTASK_HAS_FPU_STACK_LOCATION     0
#define portFPU_CONTEXT_SIZE            264

/******************************************************************/
.macro  portSAVE_FPU_REGS    offset, base
    /* Macro to assist with saving just the FPU registers to the
     * specified address and base offset,
     * offset is a constant, base is the base pointer register  */

	sdc1		$f31, \offset + 248(\base)
	sdc1		$f30, \offset + 240(\base)
	sdc1		$f29, \offset + 232(\base)
	sdc1		$f28, \offset + 224(\base)
	sdc1		$f27, \offset + 216(\base)
	sdc1		$f26, \offset + 208(\base)
	sdc1		$f25, \offset + 200(\base)
	sdc1		$f24, \offset + 192(\base)
	sdc1		$f23, \offset + 184(\base)
	sdc1		$f22, \offset + 176(\base)
	sdc1		$f21, \offset + 168(\base)
	sdc1		$f20, \offset + 160(\base)
	sdc1		$f19, \offset + 152(\base)
	sdc1		$f18, \offset + 144(\base)
	sdc1		$f17, \offset + 136(\base)
	sdc1		$f16, \offset + 128(\base)
	sdc1		$f15, \offset + 120(\base)
	sdc1		$f14, \offset + 112(\base)
	sdc1		$f13, \offset + 104(\base)
	sdc1		$f12, \offset + 96(\base)
	sdc1		$f11, \offset + 88(\base)
	sdc1		$f10, \offset + 80(\base)
	sdc1		$f9, \offset + 72(\base)
	sdc1		$f8, \offset + 64(\base)
	sdc1		$f7, \offset + 56(\base)
	sdc1		$f6, \offset + 48(\base)
	sdc1		$f5, \offset + 40(\base)
	sdc1		$f4, \offset + 32(\base)
	sdc1		$f3, \offset + 24(\base)
	sdc1		$f2, \offset + 16(\base)
	sdc1		$f1, \offset + 8(\base)
	sdc1		$f0, \offset + 0(\base)

    .endm

/******************************************************************/
.macro  portLOAD_FPU_REGS    offset, base
    /* Macro to assist with loading just the FPU registers from the
     * specified address and base offset, offset is a constant,
     * base is the base pointer register  */

	ldc1		$f0, \offset + 0(\base)
	ldc1		$f1, \offset + 8(\base)
	ldc1		$f2, \offset + 16(\base)
	ldc1		$f3, \offset + 24(\base)
	ldc1		$f4, \offset + 32(\base)
	ldc1		$f5, \offset + 40(\base)
	ldc1		$f6, \offset + 48(\base)
	ldc1		$f7, \offset + 56(\base)
	ldc1		$f8, \offset + 64(\base)
	ldc1		$f9, \offset + 72(\base)
	ldc1		$f10, \offset + 80(\base)
	ldc1		$f11, \offset + 88(\base)
	ldc1		$f12, \offset + 96(\base)
	ldc1		$f13, \offset + 104(\base)
	ldc1		$f14, \offset + 112(\base)
	ldc1		$f15, \offset + 120(\base)
	ldc1		$f16, \offset + 128(\base)
	ldc1		$f17, \offset + 136(\base)
	ldc1		$f18, \offset + 144(\base)
	ldc1		$f19, \offset + 152(\base)
	ldc1		$f20, \offset + 160(\base)
	ldc1		$f21, \offset + 168(\base)
	ldc1		$f22, \offset + 176(\base)
	ldc1		$f23, \offset + 184(\base)
	ldc1		$f24, \offset + 192(\base)
	ldc1		$f25, \offset + 200(\base)
	ldc1		$f26, \offset + 208(\base)
	ldc1		$f27, \offset + 216(\base)
	ldc1		$f28, \offset + 224(\base)
	ldc1		$f29, \offset + 232(\base)
	ldc1		$f30, \offset + 240(\base)
	ldc1		$f31, \offset + 248(\base)

    .endm

/******************************************************************/
.macro	portSAVE_CONTEXT

	/* Make room for the context. First save the current status so it can be
	manipulated, and the cause and EPC registers so their original values are
	captured. */
	mfc0		k0, _CP0_CAUSE
	addiu		sp, sp, -portCONTEXT_SIZE

	#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
		/* Test if we are already using the system stack. Only tasks may use the
		FPU so if we are already in a nested interrupt then the FPU context does
		not require saving. */
		la			k1, uxInterruptNesting
		lw			k1, 0(k1)
		bne			k1, zero, 2f
		nop

		/* Test if the current task needs the FPU context saving. */
		la			k1, ulTaskHasFPUContext
		lw			k1, 0(k1)
		beq			k1, zero, 1f
		nop

		/* Adjust the stack to account for the additional FPU context.*/
		addiu		sp, sp, -portFPU_CONTEXT_SIZE

	1:
		/* Save the ulTaskHasFPUContext flag. */
		sw			k1, portTASK_HAS_FPU_STACK_LOCATION(sp)

	2:
	#endif

	mfc0		k1, _CP0_STATUS

	/* Also save s7, s6 and s5 so they can be used.  Any nesting interrupts
	should maintain the values of these registers across the ISR. */
	sw			s7, 48(sp)
	sw			s6, 44(sp)
	sw			s5, 40(sp)
	sw			k1, portSTATUS_STACK_LOCATION(sp)

	/* Prepare to enable interrupts above the current priority. */
	srl			k0, k0, 0xa
	ins 		k1, k0, 10, 7
	srl			k0, k0, 0x7 /* This copies the MSB of the IPL, but it would be an error if it was set anyway. */
	ins 		k1, k0, 18, 1
	ins			k1, zero, 1, 4

	/* s5 is used as the frame pointer. */
	add			s5, zero, sp

	/* Check the nesting count value. */
	la			k0, uxInterruptNesting
	lw			s6, (k0)

	/* If the nesting count is 0 then swap to the the system stack, otherwise
	the system stack is already being used. */
	bne			s6, zero, 1f
	nop

	/* Swap to the system stack. */
	la			sp, xISRStackTop
	lw			sp, (sp)

	/* Increment and save the nesting count. */
1:	addiu		s6, s6, 1
	sw			s6, 0(k0)

	/* s6 holds the EPC value, this is saved after interrupts are re-enabled. */
	mfc0 		s6, _CP0_EPC

	/* Re-enable interrupts. */
	mtc0		k1, _CP0_STATUS

	/* Save the context into the space just created.  s6 is saved again
	here as it now contains the EPC value.  No other s registers need be
	saved. */
	sw			ra, 120(s5)
	sw			s8, 116(s5)
	sw			t9, 112(s5)
	sw			t8, 108(s5)
	sw			t7, 104(s5)
	sw			t6, 100(s5)
	sw			t5, 96(s5)
	sw			t4, 92(s5)
	sw			t3, 88(s5)
	sw			t2, 84(s5)
	sw			t1, 80(s5)
	sw			t0, 76(s5)
	sw			a3, 72(s5)
	sw			a2, 68(s5)
	sw			a1, 64(s5)
	sw			a0, 60(s5)
	sw			v1, 56(s5)
	sw			v0, 52(s5)
	sw			s6, portEPC_STACK_LOCATION(s5)
	sw			$1, 16(s5)

	/* Save the AC0, AC1, AC2, AC3 registers from the DSP.  s6 is used as a
	scratch register. */
	mfhi		s6, $ac1
	sw			s6, 128(s5)
	mflo		s6, $ac1
	sw			s6, 124(s5)

	mfhi		s6, $ac2
	sw			s6, 136(s5)
	mflo		s6, $ac2
	sw			s6, 132(s5)

	mfhi		s6, $ac3
	sw			s6, 144(s5)
	mflo		s6, $ac3
	sw			s6, 140(s5)

	/* Save the DSP Control register */
	rddsp		s6
	sw			s6, 148(s5)

	/* ac0 is done separately to match the MX port. */
	mfhi		s6, $ac0
	sw			s6, 12(s5)
	mflo		s6, $ac0
	sw			s6, 8(s5)

	/* Save the FPU context if the nesting count was zero. */
	#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
		la			s6, uxInterruptNesting
		lw			s6, 0(s6)
		addiu		s6, s6, -1
		bne			s6, zero, 1f
		nop

		/* Test if the current task needs the FPU context saving. */
		lw			s6, portTASK_HAS_FPU_STACK_LOCATION(s5)
		beq			s6, zero, 1f
		nop

		/* Save the FPU registers. */
		portSAVE_FPU_REGS ( portCONTEXT_SIZE + 8 ), s5

		/* Save the FPU status register */
		cfc1		s6, $f31
		sw			s6, (portCONTEXT_SIZE + portFPCSR_STACK_LOCATION)(s5)

		1:
	#endif

	/* Update the task stack pointer value if nesting is zero. */
	la			s6, uxInterruptNesting
	lw			s6, (s6)
	addiu		s6, s6, -1
	bne			s6, zero, 1f
	nop

	/* Save the stack pointer. */
	la			s6, uxSavedTaskStackPointer
	sw			s5, (s6)
1:
	.endm

/******************************************************************/
.macro	portRESTORE_CONTEXT

	/* Restore the stack pointer from the TCB.  This is only done if the
	nesting count is 1. */
	la			s6, uxInterruptNesting
	lw			s6, (s6)
	addiu		s6, s6, -1
	bne			s6, zero, 1f
	nop
	la			s6, uxSavedTaskStackPointer
	lw			s5, (s6)

    #if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
		/* Restore the FPU context if required. */
		lw			s6, portTASK_HAS_FPU_STACK_LOCATION(s5)
		beq			s6, zero, 1f
		nop

		/* Restore the FPU registers. */
		portLOAD_FPU_REGS   ( portCONTEXT_SIZE + 8 ), s5

		/* Restore the FPU status register. */
		lw			s6, ( portCONTEXT_SIZE + portFPCSR_STACK_LOCATION )(s5)
		ctc1		s6, $f31
   	#endif

1:

	/* Restore the context. */
	lw			s6, 128(s5)
	mthi		s6, $ac1
	lw			s6, 124(s5)
	mtlo		s6, $ac1

	lw			s6, 136(s5)
	mthi		s6, $ac2
	lw			s6, 132(s5)
	mtlo		s6, $ac2

	lw			s6, 144(s5)
	mthi		s6, $ac3
	lw			s6, 140(s5)
	mtlo		s6, $ac3

	/* Restore DSPControl. */
	lw			s6, 148(s5)
	wrdsp		s6

	lw			s6, 8(s5)
	mtlo		s6, $ac0
	lw			s6, 12(s5)
	mthi		s6, $ac0
	lw			$1, 16(s5)

	/* s6 is loaded as it was used as a scratch register and therefore saved
	as part of the interrupt context. */
	lw			s7, 48(s5)
	lw			s6, 44(s5)
	lw			v0, 52(s5)
	lw			v1, 56(s5)
	lw			a0, 60(s5)
	lw			a1, 64(s5)
	lw			a2, 68(s5)
	lw			a3, 72(s5)
	lw			t0, 76(s5)
	lw			t1, 80(s5)
	lw			t2, 84(s5)
	lw			t3, 88(s5)
	lw			t4, 92(s5)
	lw			t5, 96(s5)
	lw			t6, 100(s5)
	lw			t7, 104(s5)
	lw			t8, 108(s5)
	lw			t9, 112(s5)
	lw			s8, 116(s5)
	lw			ra, 120(s5)

	/* Protect access to the k registers, and others. */
	di
	ehb

	/* Decrement the nesting count. */
	la			k0, uxInterruptNesting
	lw			k1, (k0)
	addiu		k1, k1, -1
	sw			k1, 0(k0)

	#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
		/* If the nesting count is now zero then the FPU context may be restored. */
		bne			k1, zero, 1f
		nop

		/* Restore the value of ulTaskHasFPUContext */
		la			k0, ulTaskHasFPUContext
		lw			k1, 0(s5)
		sw			k1, 0(k0)

		/* If the task does not have an FPU context then adjust the stack normally. */
		beq			k1, zero, 1f
		nop

		/* Restore the STATUS and EPC registers */
		lw			k0, portSTATUS_STACK_LOCATION(s5)
		lw			k1, portEPC_STACK_LOCATION(s5)

		/* Leave the stack in its original state.  First load sp from s5, then
		restore s5 from the stack. */
		add			sp, zero, s5
		lw			s5, 40(sp)

		/* Adjust the stack pointer to remove the FPU context */
		addiu		sp, sp,	portFPU_CONTEXT_SIZE
		beq			zero, zero, 2f
		nop

		1:  /* Restore the STATUS and EPC registers */
		lw			k0, portSTATUS_STACK_LOCATION(s5)
		lw			k1, portEPC_STACK_LOCATION(s5)

		/* Leave the stack in its original state.  First load sp from s5, then
		restore s5 from the stack. */
		add			sp, zero, s5
		lw			s5, 40(sp)

		2:  /* Adjust the stack pointer */
		addiu		sp, sp, portCONTEXT_SIZE

	#else

		/* Restore the frame when there is no hardware FP support. */
		lw			k0, portSTATUS_STACK_LOCATION(s5)
		lw			k1, portEPC_STACK_LOCATION(s5)

		/* Leave the stack in its original state.  First load sp from s5, then
		restore s5 from the stack. */
		add			sp, zero, s5
		lw			s5, 40(sp)

		addiu		sp, sp,	portCONTEXT_SIZE

	#endif // ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )

	mtc0		k0, _CP0_STATUS
	mtc0 		k1, _CP0_EPC
	ehb
	eret
	nop

	.endm

