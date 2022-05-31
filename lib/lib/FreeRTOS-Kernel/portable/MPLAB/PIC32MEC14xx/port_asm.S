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
#include "ISR_Support.h"

/* Microchip includes. */
#include <xc.h>
#include <sys/asm.h>

	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern vPortIncrementTick
	.extern xISRStackTop

	PORT_CPP_JTVIC_BASE = 0xBFFFC000
	PORT_CCP_JTVIC_GIRQ24_SRC = 0xBFFFC100

	.global vPortStartFirstTask .text
	.global vPortYieldISR .text
	.global vPortTickInterruptHandler .text


/******************************************************************/


/***************************************************************
*  The following is needed to locate the
*  vPortTickInterruptHandler function into the correct vector
*  MEC14xx - This ISR will only be used if HW timers' interrupts
*  in GIRQ23 are disaggregated.
*
***************************************************************/

	.set  noreorder
	.set  noat
	.set  micromips

	.section .text, code
	.ent    vPortTickInterruptHandler

#if configTIMERS_DISAGGREGATED_ISRS == 0

	.globl girq23_isr

girq23_isr:
vPortTickInterruptHandler:

	portSAVE_CONTEXT

	jal		girq23_handler
	nop

	portRESTORE_CONTEXT

.end vPortTickInterruptHandler

#else

	.globl girq23_b4

girq23_b4:
vPortTickInterruptHandler:

	portSAVE_CONTEXT

	jal		vPortIncrementTick
	nop

	portRESTORE_CONTEXT

.end vPortTickInterruptHandler

#endif /* #if configTIMERS_DISAGGREGATED_ISRS == 0 */

/******************************************************************/

	.set	micromips
	.set	noreorder
	.set	noat

	.section .text, code
	.ent	vPortStartFirstTask

vPortStartFirstTask:

	/* Simply restore the context of the highest priority task that has
	been created so far. */
	portRESTORE_CONTEXT

.end vPortStartFirstTask



/*******************************************************************/

/***************************************************************
*  The following is needed to locate the vPortYieldISR function into the correct
* vector.
***************************************************************/

	.set micromips
	.set noreorder
	.set noat

	.section .text, code

	.global vPortYieldISR


#if configCPU_DISAGGREGATED_ISRS == 0
	.global girq24_isr
	.ent girq24_isr
girq24_isr:
	la		k0, PORT_CPP_JTVIC_BASE
	lw		k0, 0x10C(k0)
	andi	k1, k0, 0x2
	bgtz	k1, vPortYieldISR
	nop

	portSAVE_CONTEXT

	jal		girq24_b_0_2

	portRESTORE_CONTEXT

	.end girq24_isr

#else
	.global girq24_b1
girq24_b1:
#endif
		.ent  vPortYieldISR
vPortYieldISR:

	/* Make room for the context. First save the current status so it can be
	manipulated, and the cause and EPC registers so thier original values
	are captured. */
	addiu	sp, sp, -portCONTEXT_SIZE
	mfc0	k1, _CP0_STATUS

	/* Also save s6 and s5 so they can be used.  Any nesting interrupts should
	maintain the values of these registers across the ISR. */
	sw		s6, 44(sp)
	sw		s5, 40(sp)
	sw		k1, portSTATUS_STACK_LOCATION(sp)

	/* Prepare to re-enable interrupts above the kernel priority. */
	ins		k1, zero, 10, 7		 /* Clear IPL bits 0:6. */
	ins		k1, zero, 18, 1		 /* Clear IPL bit 7  */
	ori		k1, k1, ( configMAX_SYSCALL_INTERRUPT_PRIORITY << 10 )
	ins		k1, zero, 1, 4		  /* Clear EXL, ERL and UM. */

	/* s5 is used as the frame pointer. */
	add		s5, zero, sp

	/* Swap to the system stack.  This is not conditional on the nesting
	count as this interrupt is always the lowest priority and therefore
	the nesting is always 0. */
	la		sp, xISRStackTop
	lw		sp, (sp)

	/* Set the nesting count. */
	la		k0, uxInterruptNesting
	addiu   s6, zero, 1
	sw		s6, 0(k0)

	/* s6 holds the EPC value, this is saved with the rest of the context
	after interrupts are enabled. */
	mfc0	s6, _CP0_EPC

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

	/* s7 is used as a scratch register as this should always be saved acro ss
	nesting interrupts. */
	mfhi	s7
	sw		s7, 12(s5)
	mflo	s7
	sw		s7, 8(s5)

	/* Save the stack pointer to the task. */
	la		s7, pxCurrentTCB
	lw		s7, (s7)
	sw		s5, (s7)

	/* Set the interrupt mask to the max priority that can use the API.
	The yield handler will only be called at configKERNEL_INTERRUPT_PRIORITY
	which is below configMAX_SYSCALL_INTERRUPT_PRIORITY - so this can only
	ever raise the IPL value and never lower it. */
	di
	ehb
	mfc0	s7, _CP0_STATUS
	ins		s7, zero, 10, 7
	ins		s7, zero, 18, 1
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

	/* Clear the interrupt in the interrupt controller.
	MEC14xx GIRQ24 Source bit[1] = 1 to clear */
	la		s6, PORT_CCP_JTVIC_GIRQ24_SRC
	addiu	s4, zero, 2
	sw		s4, (s6)
	jal		vTaskSwitchContext
	nop

	/* Clear the interrupt mask again. The saved status value is still in s7 */
	mtc0	s7, _CP0_STATUS
	ehb

	/* Restore the stack pointer from the TCB. */
	la		s0, pxCurrentTCB
	lw		s0, (s0)
	lw		s5, (s0)

	/* Restore the rest of the context. */
	lw		s0, 8(s5)
	mtlo	s0
	lw		s0, 12(s5)
	mthi	s0

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

	mtc0	k1, _CP0_STATUS
	mtc0	k0, _CP0_EPC
	ehb
	eret
	nop

.end	vPortYieldISR




