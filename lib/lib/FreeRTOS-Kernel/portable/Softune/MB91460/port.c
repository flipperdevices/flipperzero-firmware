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

#include "FreeRTOS.h"
#include "task.h"
#include "mb91467d.h"

/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/*-----------------------------------------------------------*/
 
#pragma asm
#macro  SaveContext
	 ORCCR #0x20								;Switch to user stack
	 ST RP,@-R15								;Store RP
	 STM0 (R7,R6,R5,R4,R3,R2,R1,R0)				;Store R7-R0
	 STM1 (R14,R13,R12,R11,R10,R9,R8)			;Store R14-R8
	 ST MDH, @-R15								;Store MDH
	 ST MDL, @-R15								;Store MDL
	 
	 ANDCCR #0xDF								;Switch back to system stack
	 LD @R15+,R0								;Store PC to R0 
	 ORCCR #0x20								;Switch to user stack
	 ST R0,@-R15								;Store PC to User stack
	 
	 ANDCCR #0xDF								;Switch back to system stack
	 LD @R15+,R0								;Store PS to R0
	 ORCCR #0x20								;Switch to user stack
	 ST R0,@-R15								;Store PS to User stack
	 
	 LDI #_pxCurrentTCB, R0						;Get pxCurrentTCB address
	 LD @R0, R0									;Get the pxCurrentTCB->pxTopOfStack address
	 ST R15,@R0									;Store USP to pxCurrentTCB->pxTopOfStack
	 
	 ANDCCR #0xDF								;Switch back to system stack for the rest of tick ISR
#endm

#macro RestoreContext
	 LDI #_pxCurrentTCB, R0						;Get pxCurrentTCB address
	 LD @R0, R0									;Get the pxCurrentTCB->pxTopOfStack address
	 ORCCR #0x20								;Switch to user stack
	 LD @R0, R15								;Restore USP from pxCurrentTCB->pxTopOfStack

	 LD @R15+,R0								;Store PS to R0
	 ANDCCR #0xDF								;Switch to system stack
	 ST R0,@-R15								;Store PS to system stack

	 ORCCR #0x20								;Switch to user stack
	 LD @R15+,R0								;Store PC to R0
	 ANDCCR #0xDF								;Switch to system stack
	 ST R0,@-R15								;Store PC to system stack

	 ORCCR #0x20								;Switch back to retrieve the remaining context

	 LD @R15+, MDL								;Restore MDL
	 LD @R15+, MDH								;Restore MDH
	 LDM1 (R14,R13,R12,R11,R10,R9,R8)			;Restore R14-R8
	 LDM0 (R7,R6,R5,R4,R3,R2,R1,R0)				;Restore R7-R0
	 LD @R15+, RP								;Restore RP
	 
	 ANDCCR #0xDF								;Switch back to system stack for the rest of tick ISR
#endm
#pragma endasm

/*-----------------------------------------------------------*/

/*
 * Perform hardware setup to enable ticks from timer 1,
 */
static void prvSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 * 
 * See the header file portable.h.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Place a few bytes of known values on the bottom of the stack. 
	This is just useful for debugging. */

	*pxTopOfStack = 0x11111111;
	pxTopOfStack--;
	*pxTopOfStack = 0x22222222;
	pxTopOfStack--;
	*pxTopOfStack = 0x33333333;
	pxTopOfStack--;

	/* This is a redundant push to the stack, it may be required if 
	in some implementations of the compiler the parameter to the task 
	is passed on to the stack rather than in R4 register. */
	*pxTopOfStack = (StackType_t)(pvParameters);
	pxTopOfStack--;                  
    
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* RP */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00007777;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00006666;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00005555;	/* R5 */
	pxTopOfStack--;
	
	/* In the current implementation of the compiler the first 
	parameter to the task (or function) is passed via R4 parameter 
	to the task, hence the pvParameters pointer is copied into the R4 
	register. See compiler manual section 4.6.2 for more information. */
	*pxTopOfStack = ( StackType_t ) (pvParameters);	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00003333;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00002222;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00001111;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000001;	/* R0 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x0000EEEE;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x0000DDDD;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x0000CCCC;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x0000BBBB;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x0000AAAA;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00009999;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00008888;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x11110000;	/* MDH */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x22220000;	/* MDL */
	pxTopOfStack--;

	/* The start of the task code. */
	*pxTopOfStack = ( StackType_t ) pxCode;	/* PC */
	pxTopOfStack--;
	 
    /* PS - User Mode, USP, ILM=31, Interrupts enabled */
	*pxTopOfStack = ( StackType_t ) 0x001F0030;	/* PS */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Setup the hardware to generate the tick. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task that is going to run. */
	#pragma asm
		RestoreContext
	#pragma endasm

	/* Simulate a function call end as generated by the compiler.  We will now
	jump to the start of the task the context of which we have just restored. */	
	__asm(" reti ");

	/* Should not get here. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented - unlikely to ever be required as there is nothing to
	return to. */
}
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
/* The peripheral clock divided by 32 is used by the timer. */
const uint16_t usReloadValue = ( uint16_t ) ( ( ( configPER_CLOCK_HZ / configTICK_RATE_HZ ) / 32UL ) - 1UL );

	/* Setup RLT0 to generate a tick interrupt. */

	TMCSR0_CNTE = 0;		/* Count Disable */
    TMCSR0_CSL = 0x2;		/* CLKP/32 */
    TMCSR0_MOD = 0;			/* Software trigger */
    TMCSR0_RELD = 1;		/* Reload */
    
    TMCSR0_UF = 0;			/* Clear underflow flag */
	TMRLR0 = usReloadValue;
	TMCSR0_INTE = 1;		/* Interrupt Enable */
	TMCSR0_CNTE = 1;		/* Count Enable */
	TMCSR0_TRG = 1;			/* Trigger */
	
    PORTEN = 0x3;			/* Port Enable */
}
/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 1

	/* 
	 * Tick ISR for preemptive scheduler. The tick count is incremented 
	 * after the context is saved. Then the context is switched if required,
	 * and last the context of the task which is to be resumed is restored.
	 */

	#pragma asm

	.global _ReloadTimer0_IRQHandler
	_ReloadTimer0_IRQHandler:

	ANDCCR #0xEF							;Disable Interrupts
	SaveContext								;Save context
	ORCCR #0x10								;Re-enable Interrupts

	LDI #0xFFFB,R1
	LDI #_tmcsr0, R0
	AND R1,@R0								;Clear RLT0 interrupt flag

	CALL32	 _xTaskIncrementTick,R12		;Increment Tick
	CALL32	 _vTaskSwitchContext,R12		;Switch context if required

	ANDCCR #0xEF							;Disable Interrupts
	RestoreContext							;Restore context
	ORCCR #0x10								;Re-enable Interrupts

	RETI

	#pragma endasm
	
#else
	
	/* 
	 * Tick ISR for the cooperative scheduler.  All this does is increment the
	 * tick count.  We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 */
	__interrupt void ReloadTimer0_IRQHandler( void )
	{
		/* Clear RLT0 interrupt flag */
		TMCSR0_UF = 0; 
		xTaskIncrementTick();
	}

#endif

/*
 * Manual context switch. We can use a __nosavereg attribute  as the context 
 * would be saved by PortSAVE_CONTEXT().  The context is switched and then 
 * the context of the new task is restored saved. 
 */
#pragma asm

	.global _vPortYieldDelayed
	_vPortYieldDelayed:

	ANDCCR #0xEF							;Disable Interrupts
	SaveContext								;Save context
	ORCCR #0x10								;Re-enable Interrupts

	LDI #_dicr, R0
	BANDL #0x0E, @R0						;Clear Delayed interrupt flag

	CALL32	 _vTaskSwitchContext,R12		;Switch context if required

	ANDCCR #0xEF							;Disable Interrupts
	RestoreContext							;Restore context
	ORCCR #0x10								;Re-enable Interrupts

	RETI

#pragma endasm
/*-----------------------------------------------------------*/

/*
 * Manual context switch. We can use a __nosavereg attribute  as the context 
 * would be saved by PortSAVE_CONTEXT().  The context is switched and then 
 * the context of the new task is restored saved. 
 */ 	 
#pragma asm

	.global _vPortYield
	_vPortYield:

	SaveContext								;Save context
	CALL32	 _vTaskSwitchContext,R12		;Switch context if required
	RestoreContext							;Restore context
	
	RETI

#pragma endasm
/*-----------------------------------------------------------*/

