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

#include <xc.h>
#include <sys/asm.h>
#include "FreeRTOSConfig.h"
#include "ISR_Support.h"

	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern vPortIncrementTick
	.extern xISRStackTop
	.extern ulTaskHasFPUContext

	.global vPortStartFirstTask
	.global vPortYieldISR
	.global vPortTickInterruptHandler
	.global vPortInitialiseFPSCR


/******************************************************************/

	.set  nomips16
	.set  nomicromips
	.set  noreorder
	.set  noat

	/***************************************************************
	*  The following is needed to locate the
	*  vPortTickInterruptHandler function into the correct vector
	***************************************************************/
	#ifdef configTICK_INTERRUPT_VECTOR
		#if (configTICK_INTERRUPT_VECTOR == _CORE_TIMER_VECTOR)
			.equ     __vector_dispatch_0, vPortTickInterruptHandler
			.global  __vector_dispatch_0
			.section .vector_0, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_1_VECTOR)
			.equ     __vector_dispatch_4, vPortTickInterruptHandler
			.global  __vector_dispatch_4
			.section .vector_4, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_2_VECTOR)
			.equ     __vector_dispatch_9, vPortTickInterruptHandler
			.global  __vector_dispatch_9
			.section .vector_9, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_3_VECTOR)
			.equ     __vector_dispatch_14, vPortTickInterruptHandler
			.global  __vector_dispatch_14
			.section .vector_14, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_4_VECTOR)
			.equ     __vector_dispatch_19, vPortTickInterruptHandler
			.global  __vector_dispatch_19
			.section .vector_19, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_5_VECTOR)
			.equ     __vector_dispatch_24, vPortTickInterruptHandler
			.global  __vector_dispatch_24
			.section .vector_24, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_6_VECTOR)
			.equ     __vector_dispatch_28, vPortTickInterruptHandler
			.global  __vector_dispatch_28
			.section .vector_28, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_7_VECTOR)
			.equ     __vector_dispatch_32, vPortTickInterruptHandler
			.global  __vector_dispatch_32
			.section .vector_32, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_8_VECTOR)
			.equ     __vector_dispatch_36, vPortTickInterruptHandler
			.global  __vector_dispatch_36
			.section .vector_36, code, keep
		#elif (configTICK_INTERRUPT_VECTOR == _TIMER_9_VECTOR)
			.equ     __vector_dispatch_40, vPortTickInterruptHandler
			.global  __vector_dispatch_40
			.section .vector_40, code, keep
		#endif
	#else
		.equ     __vector_dispatch_4, vPortTickInterruptHandler
		.global  __vector_dispatch_4
		.section .vector_4, code, keep
	#endif

	.ent		vPortTickInterruptHandler

vPortTickInterruptHandler:

	portSAVE_CONTEXT

	jal 		vPortIncrementTick
	nop

	portRESTORE_CONTEXT

	.end vPortTickInterruptHandler

/******************************************************************/

	.set		noreorder
	.set 		noat
	.section .text, code
	.ent		vPortStartFirstTask

vPortStartFirstTask:

	/* Simply restore the context of the highest priority task that has been
	created so far. */
	portRESTORE_CONTEXT

	.end vPortStartFirstTask



/*******************************************************************/

	.set  nomips16
	.set  nomicromips
	.set  noreorder
	.set  noat
	/***************************************************************
	*  The following is needed to locate the vPortYieldISR function
	*  into the correct vector
	***************************************************************/
	.equ     __vector_dispatch_1, vPortYieldISR
	.global  __vector_dispatch_1
	.section .vector_1, code

	.ent  vPortYieldISR
vPortYieldISR:

	#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
		/* Code sequence for FPU support, the context save requires advance
		knowledge of the stack frame size and if the current task actually uses the 
		FPU. */

		/* Make room for the context. First save the current status so it can be
		manipulated, and the cause and EPC registers so their original values are
		captured. */
		la		k0, ulTaskHasFPUContext
		lw		k0, 0(k0)
		beq		k0, zero, 1f
		addiu	sp, sp, -portCONTEXT_SIZE	/* always reserve space for the context. */
		addiu	sp, sp, -portFPU_CONTEXT_SIZE	/* reserve additional space for the FPU context. */
	1:
		mfc0	k1, _CP0_STATUS

		/* Also save s6 and s5 so they can be used.  Any nesting interrupts should
		maintain the values of these registers across the ISR. */
		sw		s6, 44(sp)
		sw		s5, 40(sp)
		sw		k1, portSTATUS_STACK_LOCATION(sp)
		sw		k0, portTASK_HAS_FPU_STACK_LOCATION(sp)

		/* Prepare to re-enabled interrupts above the kernel priority. */
		ins 	k1, zero, 10, 7         /* Clear IPL bits 0:6. */
		ins 	k1, zero, 18, 1         /* Clear IPL bit 7.  It would be an error here if this bit were set anyway. */
		ori		k1, k1, ( configMAX_SYSCALL_INTERRUPT_PRIORITY << 10 )
		ins		k1, zero, 1, 4          /* Clear EXL, ERL and UM. */

		/* s5 is used as the frame pointer. */
		add		s5, zero, sp

		/* Swap to the system stack.  This is not conditional on the nesting
		count as this interrupt is always the lowest priority and therefore
		the nesting is always 0. */
		la		sp, xISRStackTop
		lw		sp, (sp)

		/* Set the nesting count. */
		la		k0, uxInterruptNesting
		addiu	s6, zero, 1
		sw		s6, 0(k0)

		/* s6 holds the EPC value, this is saved with the rest of the context
		after interrupts are enabled. */
		mfc0 	s6, _CP0_EPC

		/* Re-enable interrupts above configMAX_SYSCALL_INTERRUPT_PRIORITY. */
		mtc0	k1, _CP0_STATUS

		/* Save the context into the space just created.  s6 is saved again
		here as it now contains the EPC value. */
		sw		ra, 120(s5)
		sw		s8, 116(s5)
		sw		t9, 112(s5)
		sw		t8, 108(s5)
		sw		t7, 104(s5)
		sw		t6, 100(s5)
		sw		t5, 96(s5)
		sw		t4, 92(s5)
		sw		t3, 88(s5)
		sw		t2, 84(s5)
		sw		t1, 80(s5)
		sw		t0, 76(s5)
		sw		a3, 72(s5)
		sw		a2, 68(s5)
		sw		a1, 64(s5)
		sw		a0, 60(s5)
		sw		v1, 56(s5)
		sw		v0, 52(s5)
		sw		s7, 48(s5)
		sw		s6, portEPC_STACK_LOCATION(s5)
		/* s5 and s6 has already been saved. */
		sw		s4, 36(s5)
		sw		s3, 32(s5)
		sw		s2, 28(s5)
		sw		s1, 24(s5)
		sw		s0, 20(s5)
		sw		$1, 16(s5)

		/* s7 is used as a scratch register as this should always be saved across
		nesting interrupts. */

		/* Save the AC0, AC1, AC2 and AC3. */
		mfhi	s7, $ac1
		sw		s7, 128(s5)
		mflo	s7, $ac1
		sw		s7, 124(s5)

		mfhi	s7, $ac2
		sw		s7, 136(s5)
		mflo	s7, $ac2
		sw		s7, 132(s5)

		mfhi	s7, $ac3
		sw		s7, 144(s5)
		mflo	s7, $ac3
		sw		s7, 140(s5)

		rddsp	s7
		sw		s7, 148(s5)

		mfhi	s7, $ac0
		sw		s7, 12(s5)
		mflo	s7, $ac0
		sw		s7, 8(s5)

		/* Test if FPU context save is required. */
		lw		s7, portTASK_HAS_FPU_STACK_LOCATION(s5)
		beq		s7, zero, 1f
		nop

		/* Save the FPU registers above the normal context. */
		portSAVE_FPU_REGS   (portCONTEXT_SIZE + 8), s5

		/* Save the FPU status register */
		cfc1	s7, $f31
		sw		s7, ( portCONTEXT_SIZE + portFPCSR_STACK_LOCATION )(s5)

	1:
		/* Save the stack pointer to the task. */
		la		s7, pxCurrentTCB
		lw		s7, (s7)
		sw		s5, (s7)

		/* Set the interrupt mask to the max priority that can use the API.  The
		yield handler will only be called at configKERNEL_INTERRUPT_PRIORITY which
		is below configMAX_SYSCALL_INTERRUPT_PRIORITY - so this can only ever
		raise the IPL value and never lower it. */
		di
		ehb
		mfc0	s7, _CP0_STATUS
		ins 	s7, zero, 10, 7
		ins 	s7, zero, 18, 1
		ori		s6, s7, ( configMAX_SYSCALL_INTERRUPT_PRIORITY << 10 ) | 1

		/* This mtc0 re-enables interrupts, but only above
		configMAX_SYSCALL_INTERRUPT_PRIORITY. */
		mtc0	s6, _CP0_STATUS
		ehb

		/* Clear the software interrupt in the core. */
		mfc0	s6, _CP0_CAUSE
		ins		s6, zero, 8, 1
		mtc0	s6, _CP0_CAUSE
		ehb

		/* Clear the interrupt in the interrupt controller. */
		la		s6, IFS0CLR
		addiu	s4, zero, 2
		sw		s4, (s6)

		jal		vTaskSwitchContext
		nop

		/* Clear the interrupt mask again.  The saved status value is still in s7. */
		mtc0	s7, _CP0_STATUS
		ehb

		/* Restore the stack pointer from the TCB. */
		la		s0, pxCurrentTCB
		lw		s0, (s0)
		lw		s5, (s0)

		/* Test if the FPU context needs restoring. */
		lw		s0, portTASK_HAS_FPU_STACK_LOCATION(s5)
		beq		s0, zero, 1f
		nop

		/* Restore the FPU status register. */
		lw		s0, ( portCONTEXT_SIZE + portFPCSR_STACK_LOCATION )(s5)
		ctc1	s0, $f31

		/* Restore the FPU registers. */
		portLOAD_FPU_REGS   ( portCONTEXT_SIZE + 8 ), s5

	1:
		/* Restore the rest of the context. */
		lw		s0, 128(s5)
		mthi	s0, $ac1
		lw		s0, 124(s5)
		mtlo		s0, $ac1

		lw		s0, 136(s5)
		mthi	s0, $ac2
		lw		s0, 132(s5)
		mtlo	s0, $ac2

		lw		s0, 144(s5)
		mthi	s0, $ac3
		lw		s0, 140(s5)
		mtlo	s0, $ac3

		lw		s0, 148(s5)
		wrdsp	s0

		lw		s0, 8(s5)
		mtlo	s0, $ac0
		lw		s0, 12(s5)
		mthi	s0, $ac0

		lw		$1, 16(s5)
		lw		s0, 20(s5)
		lw		s1, 24(s5)
		lw		s2, 28(s5)
		lw		s3, 32(s5)
		lw		s4, 36(s5)

		/* s5 is loaded later. */
		lw		s6, 44(s5)
		lw		s7, 48(s5)
		lw		v0, 52(s5)
		lw		v1, 56(s5)
		lw		a0, 60(s5)
		lw		a1, 64(s5)
		lw		a2, 68(s5)
		lw		a3, 72(s5)
		lw		t0, 76(s5)
		lw		t1, 80(s5)
		lw		t2, 84(s5)
		lw		t3, 88(s5)
		lw		t4, 92(s5)
		lw		t5, 96(s5)
		lw		t6, 100(s5)
		lw		t7, 104(s5)
		lw		t8, 108(s5)
		lw		t9, 112(s5)
		lw		s8, 116(s5)
		lw		ra, 120(s5)

		/* Protect access to the k registers, and others. */
		di
		ehb

		/* Set nesting back to zero.  As the lowest priority interrupt this
		interrupt cannot have nested. */
		la		k0, uxInterruptNesting
		sw		zero, 0(k0)

		/* Switch back to use the real stack pointer. */
		add		sp, zero, s5

		/* Restore the real s5 value. */
		lw		s5, 40(sp)

		/* Pop the FPU context value from the stack */
		lw		k0, portTASK_HAS_FPU_STACK_LOCATION(sp)
		la		k1, ulTaskHasFPUContext
		sw		k0, 0(k1)
		beq		k0, zero, 1f
		nop

		/* task has FPU context so adjust the stack frame after popping the
		status and epc values. */
		lw		k1, portSTATUS_STACK_LOCATION(sp)
		lw		k0, portEPC_STACK_LOCATION(sp)
		addiu	sp, sp, portFPU_CONTEXT_SIZE
		beq		zero, zero, 2f
		nop

	1:
		/* Pop the status and epc values. */
		lw		k1, portSTATUS_STACK_LOCATION(sp)
		lw		k0, portEPC_STACK_LOCATION(sp)

	2:
		/* Remove stack frame. */
		addiu	sp, sp, portCONTEXT_SIZE

	#else
		/* Code sequence for no FPU support, the context save requires advance
		knowledge of the stack frame size when no FPU is being used */

		/* Make room for the context. First save the current status so it can be
		manipulated, and the cause and EPC registers so thier original values are
		captured. */
		addiu	sp, sp, -portCONTEXT_SIZE
		mfc0	k1, _CP0_STATUS

		/* Also save s6 and s5 so they can be used.  Any nesting interrupts should
		maintain the values of these registers across the ISR. */
		sw		s6, 44(sp)
		sw		s5, 40(sp)
		sw		k1, portSTATUS_STACK_LOCATION(sp)

		/* Prepare to re-enabled interrupts above the kernel priority. */
		ins 	k1, zero, 10, 7         /* Clear IPL bits 0:6. */
		ins 	k1, zero, 18, 1         /* Clear IPL bit 7.  It would be an error here if this bit were set anyway. */
		ori		k1, k1, ( configMAX_SYSCALL_INTERRUPT_PRIORITY << 10 )
		ins		k1, zero, 1, 4          /* Clear EXL, ERL and UM. */

		/* s5 is used as the frame pointer. */
		add		s5, zero, sp

		/* Swap to the system stack.  This is not conditional on the nesting
		count as this interrupt is always the lowest priority and therefore
		the nesting is always 0. */
		la		sp, xISRStackTop
		lw		sp, (sp)

		/* Set the nesting count. */
		la		k0, uxInterruptNesting
		addiu	s6, zero, 1
		sw		s6, 0(k0)

		/* s6 holds the EPC value, this is saved with the rest of the context
		after interrupts are enabled. */
		mfc0 	s6, _CP0_EPC

		/* Re-enable interrupts above configMAX_SYSCALL_INTERRUPT_PRIORITY. */
		mtc0	k1, _CP0_STATUS

		/* Save the context into the space just created.  s6 is saved again
		here as it now contains the EPC value. */
		sw		ra, 120(s5)
		sw		s8, 116(s5)
		sw		t9, 112(s5)
		sw		t8, 108(s5)
		sw		t7, 104(s5)
		sw		t6, 100(s5)
		sw		t5, 96(s5)
		sw		t4, 92(s5)
		sw		t3, 88(s5)
		sw		t2, 84(s5)
		sw		t1, 80(s5)
		sw		t0, 76(s5)
		sw		a3, 72(s5)
		sw		a2, 68(s5)
		sw		a1, 64(s5)
		sw		a0, 60(s5)
		sw		v1, 56(s5)
		sw		v0, 52(s5)
		sw		s7, 48(s5)
		sw		s6, portEPC_STACK_LOCATION(s5)
		/* s5 and s6 has already been saved. */
		sw		s4, 36(s5)
		sw		s3, 32(s5)
		sw		s2, 28(s5)
		sw		s1, 24(s5)
		sw		s0, 20(s5)
		sw		$1, 16(s5)

		/* s7 is used as a scratch register as this should always be saved across
		nesting interrupts. */

		/* Save the AC0, AC1, AC2 and AC3. */
		mfhi	s7, $ac1
		sw		s7, 128(s5)
		mflo	s7, $ac1
		sw		s7, 124(s5)

		mfhi	s7, $ac2
		sw		s7, 136(s5)
		mflo	s7, $ac2
		sw		s7, 132(s5)

		mfhi	s7, $ac3
		sw		s7, 144(s5)
		mflo	s7, $ac3
		sw		s7, 140(s5)

		rddsp	s7
		sw		s7, 148(s5)

		mfhi	s7, $ac0
		sw		s7, 12(s5)
		mflo	s7, $ac0
		sw		s7, 8(s5)

		/* Save the stack pointer to the task. */
		la		s7, pxCurrentTCB
		lw		s7, (s7)
		sw		s5, (s7)

		/* Set the interrupt mask to the max priority that can use the API.  The
		yield handler will only be called at configKERNEL_INTERRUPT_PRIORITY which
		is below configMAX_SYSCALL_INTERRUPT_PRIORITY - so this can only ever
		raise the IPL value and never lower it. */
		di
		ehb
		mfc0	s7, _CP0_STATUS
		ins 	s7, zero, 10, 7
		ins 	s7, zero, 18, 1
		ori		s6, s7, ( configMAX_SYSCALL_INTERRUPT_PRIORITY << 10 ) | 1

		/* This mtc0 re-enables interrupts, but only above
		configMAX_SYSCALL_INTERRUPT_PRIORITY. */
		mtc0	s6, _CP0_STATUS
		ehb

		/* Clear the software interrupt in the core. */
		mfc0	s6, _CP0_CAUSE
		ins		s6, zero, 8, 1
		mtc0	s6, _CP0_CAUSE
		ehb

		/* Clear the interrupt in the interrupt controller. */
		la		s6, IFS0CLR
		addiu	s4, zero, 2
		sw		s4, (s6)

		jal		vTaskSwitchContext
		nop

		/* Clear the interrupt mask again.  The saved status value is still in s7. */
		mtc0	s7, _CP0_STATUS
		ehb

		/* Restore the stack pointer from the TCB. */
		la		s0, pxCurrentTCB
		lw		s0, (s0)
		lw		s5, (s0)

		/* Restore the rest of the context. */
		lw		s0, 128(s5)
		mthi	s0, $ac1
		lw		s0, 124(s5)
		mtlo	s0, $ac1

		lw		s0, 136(s5)
		mthi	s0, $ac2
		lw		s0, 132(s5)
		mtlo	s0, $ac2

		lw		s0, 144(s5)
		mthi	s0, $ac3
		lw		s0, 140(s5)
		mtlo	s0, $ac3

		lw		s0, 148(s5)
		wrdsp	s0

		lw		s0, 8(s5)
		mtlo	s0, $ac0
		lw		s0, 12(s5)
		mthi	s0, $ac0

		lw		$1, 16(s5)
		lw		s0, 20(s5)
		lw		s1, 24(s5)
		lw		s2, 28(s5)
		lw		s3, 32(s5)
		lw		s4, 36(s5)

		/* s5 is loaded later. */
		lw		s6, 44(s5)
		lw		s7, 48(s5)
		lw		v0, 52(s5)
		lw		v1, 56(s5)
		lw		a0, 60(s5)
		lw		a1, 64(s5)
		lw		a2, 68(s5)
		lw		a3, 72(s5)
		lw		t0, 76(s5)
		lw		t1, 80(s5)
		lw		t2, 84(s5)
		lw		t3, 88(s5)
		lw		t4, 92(s5)
		lw		t5, 96(s5)
		lw		t6, 100(s5)
		lw		t7, 104(s5)
		lw		t8, 108(s5)
		lw		t9, 112(s5)
		lw		s8, 116(s5)
		lw		ra, 120(s5)

		/* Protect access to the k registers, and others. */
		di
		ehb

		/* Set nesting back to zero.  As the lowest priority interrupt this
		interrupt cannot have nested. */
		la		k0, uxInterruptNesting
		sw		zero, 0(k0)

		/* Switch back to use the real stack pointer. */
		add		sp, zero, s5

		/* Restore the real s5 value. */
		lw		s5, 40(sp)

		/* Pop the status and epc values. */
		lw		k1, portSTATUS_STACK_LOCATION(sp)
		lw		k0, portEPC_STACK_LOCATION(sp)

		/* Remove stack frame. */
		addiu	sp, sp, portCONTEXT_SIZE

	#endif /* ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 ) */

	/* Restore the status and EPC registers and return */
	mtc0	k1, _CP0_STATUS
	mtc0 	k0, _CP0_EPC
	ehb
	eret
	nop

	.end	vPortYieldISR

/******************************************************************/

#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )

	.macro portFPUSetAndInc reg, dest
	mtc1	\reg, \dest
	cvt.d.w	\dest, \dest
	addiu	\reg, \reg, 1
	.endm

	.set	noreorder
	.set 	noat
	.section .text, code
	.ent	vPortInitialiseFPSCR

vPortInitialiseFPSCR:

	/* Initialize the floating point status register in CP1. The initial
	value is passed in a0. */
	ctc1		a0, $f31

	/* Clear the FPU registers */
	addiu			a0, zero, 0x0000
	portFPUSetAndInc	a0, $f0
	portFPUSetAndInc	a0, $f1
	portFPUSetAndInc	a0, $f2
	portFPUSetAndInc	a0, $f3
	portFPUSetAndInc	a0, $f4
	portFPUSetAndInc	a0, $f5
	portFPUSetAndInc	a0, $f6
	portFPUSetAndInc	a0, $f7
	portFPUSetAndInc	a0, $f8
	portFPUSetAndInc	a0, $f9
	portFPUSetAndInc	a0, $f10
	portFPUSetAndInc	a0, $f11
	portFPUSetAndInc	a0, $f12
	portFPUSetAndInc	a0, $f13
	portFPUSetAndInc	a0, $f14
	portFPUSetAndInc	a0, $f15
	portFPUSetAndInc	a0, $f16
	portFPUSetAndInc	a0, $f17
	portFPUSetAndInc	a0, $f18
	portFPUSetAndInc	a0, $f19
	portFPUSetAndInc	a0, $f20
	portFPUSetAndInc	a0, $f21
	portFPUSetAndInc	a0, $f22
	portFPUSetAndInc	a0, $f23
	portFPUSetAndInc	a0, $f24
	portFPUSetAndInc	a0, $f25
	portFPUSetAndInc	a0, $f26
	portFPUSetAndInc	a0, $f27
	portFPUSetAndInc	a0, $f28
	portFPUSetAndInc	a0, $f29
	portFPUSetAndInc	a0, $f30
	portFPUSetAndInc	a0, $f31

	jr		ra
	nop

	.end vPortInitialiseFPSCR

#endif /* ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 ) */
	
#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )

	/**********************************************************************/
	/* Test read back								*/
	/* a0 = address to store registers				*/

	.set		noreorder
	.set 		noat
	.section	.text, code
	.ent		vPortFPUReadback
	.global		vPortFPUReadback

vPortFPUReadback:
	sdc1		$f0, 0(a0)
	sdc1		$f1, 8(a0)
	sdc1		$f2, 16(a0)
	sdc1		$f3, 24(a0)
	sdc1		$f4, 32(a0)
	sdc1		$f5, 40(a0)
	sdc1		$f6, 48(a0)
	sdc1		$f7, 56(a0)
	sdc1		$f8, 64(a0)
	sdc1		$f9, 72(a0)
	sdc1		$f10, 80(a0)
	sdc1		$f11, 88(a0)
	sdc1		$f12, 96(a0)
	sdc1		$f13, 104(a0)
	sdc1		$f14, 112(a0)
	sdc1		$f15, 120(a0)
	sdc1		$f16, 128(a0)
	sdc1		$f17, 136(a0)
	sdc1		$f18, 144(a0)
	sdc1		$f19, 152(a0)
	sdc1		$f20, 160(a0)
	sdc1		$f21, 168(a0)
	sdc1		$f22, 176(a0)
	sdc1		$f23, 184(a0)
	sdc1		$f24, 192(a0)
	sdc1		$f25, 200(a0)
	sdc1		$f26, 208(a0)
	sdc1		$f27, 216(a0)
	sdc1		$f28, 224(a0)
	sdc1		$f29, 232(a0)
	sdc1		$f30, 240(a0)
	sdc1		$f31, 248(a0)

	jr		ra
	nop

	.end vPortFPUReadback

#endif /* ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 ) */




