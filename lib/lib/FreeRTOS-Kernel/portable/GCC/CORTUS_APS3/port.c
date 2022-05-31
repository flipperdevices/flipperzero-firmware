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

/* Standard includes. */
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Machine includes */
#include <machine/counter.h>
#include <machine/ic.h>
/*-----------------------------------------------------------*/

/* The initial PSR has the Previous Interrupt Enabled (PIEN) flag set. */
#define portINITIAL_PSR			( 0x00020000 )

/*-----------------------------------------------------------*/

/*
 * Perform any hardware configuration necessary to generate the tick interrupt.
 */
static void prvSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

StackType_t *pxPortInitialiseStack( StackType_t * pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Make space on the stack for the context - this leaves a couple of spaces
	empty.  */
	pxTopOfStack -= 20;

	/* Fill the registers with known values to assist debugging. */
	pxTopOfStack[ 16 ] = 0;
	pxTopOfStack[ 15 ] = portINITIAL_PSR;
	pxTopOfStack[ 14 ] = ( uint32_t ) pxCode;
	pxTopOfStack[ 13 ] = 0x00000000UL; /* R15. */
	pxTopOfStack[ 12 ] = 0x00000000UL; /* R14. */
	pxTopOfStack[ 11 ] = 0x0d0d0d0dUL;
	pxTopOfStack[ 10 ] = 0x0c0c0c0cUL;
	pxTopOfStack[ 9 ] = 0x0b0b0b0bUL;
	pxTopOfStack[ 8 ] = 0x0a0a0a0aUL;
	pxTopOfStack[ 7 ] = 0x09090909UL;
	pxTopOfStack[ 6 ] = 0x08080808UL;
	pxTopOfStack[ 5 ] = 0x07070707UL;
	pxTopOfStack[ 4 ] = 0x06060606UL;
	pxTopOfStack[ 3 ] = 0x05050505UL;
	pxTopOfStack[ 2 ] = 0x04040404UL;
	pxTopOfStack[ 1 ] = 0x03030303UL;
	pxTopOfStack[ 0 ] = ( uint32_t ) pvParameters;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Set-up the timer interrupt. */
	prvSetupTimerInterrupt();

	/* Integrated Interrupt Controller: Enable all interrupts. */
	ic->ien = 1;

	/* Restore callee saved registers. */
	portRESTORE_CONTEXT();

	/* Should not get here. */
	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
	/* Enable timer interrupts */
	counter1->reload = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1;
	counter1->value = counter1->reload;
	counter1->mask = 1;

	/* Set the IRQ Handler priority and enable it. */
	irq[ IRQ_COUNTER1 ].ien = 1;
}
/*-----------------------------------------------------------*/

/* Trap 31 handler. */
void interrupt31_handler( void ) __attribute__((naked));
void interrupt31_handler( void )
{
	portSAVE_CONTEXT();
	__asm volatile ( "call vTaskSwitchContext" );
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

static void prvProcessTick( void ) __attribute__((noinline));
static void prvProcessTick( void )
{
	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}
		
	/* Clear the Tick Interrupt. */
	counter1->expired = 0;
}
/*-----------------------------------------------------------*/

/* Timer 1 interrupt handler, used for tick interrupt. */
void interrupt7_handler( void ) __attribute__((naked));
void interrupt7_handler( void )
{
	portSAVE_CONTEXT();
	prvProcessTick();
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Nothing to do. Unlikely to want to end. */
}
/*-----------------------------------------------------------*/
