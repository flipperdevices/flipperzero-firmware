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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* The critical nesting value is initialised to a non zero value to ensure
interrupts don't accidentally become enabled before the scheduler is started. */
#define portINITIAL_CRITICAL_NESTING  ( ( uint16_t ) 10 )

/* Initial PSW value allocated to a newly created task.
 *   1100011000000000
 *   ||||||||-------------- Fill byte
 *   |||||||--------------- Carry Flag cleared
 *   |||||----------------- In-service priority Flags set to low level
 *   ||||------------------ Register bank Select 0 Flag cleared
 *   |||------------------- Auxiliary Carry Flag cleared
 *   ||-------------------- Register bank Select 1 Flag cleared
 *   |--------------------- Zero Flag set
 *   ---------------------- Global Interrupt Flag set (enabled)
 */
#define portPSW		  ( 0xc6UL )

/* The address of the pxCurrentTCB variable, but don't know or need to know its
type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/* Each task maintains a count of the critical section nesting depth.  Each time
a critical section is entered the count is incremented.  Each time a critical
section is exited the count is decremented - with interrupts only being
re-enabled if the count is zero.

usCriticalNesting will get set to zero when the scheduler starts, but must
not be initialised to zero as that could cause problems during the startup
sequence. */
volatile uint16_t usCriticalNesting = portINITIAL_CRITICAL_NESTING;

/*-----------------------------------------------------------*/

/*
 * Sets up the periodic ISR used for the RTOS tick using the interval timer.
 * The application writer can define configSETUP_TICK_INTERRUPT() (in
 * FreeRTOSConfig.h) such that their own tick interrupt configuration is used
 * in place of prvSetupTimerInterrupt().
 */
static void prvSetupTimerInterrupt( void );
#ifndef configSETUP_TICK_INTERRUPT
	/* The user has not provided their own tick interrupt configuration so use
    the definition in this file (which uses the interval timer). */
	#define configSETUP_TICK_INTERRUPT() prvSetupTimerInterrupt()
#endif /* configSETUP_TICK_INTERRUPT */

/*
 * Defined in portasm.s87, this function starts the scheduler by loading the
 * context of the first task to run.
 */
extern void vPortStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See the header file portable.h.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
uint32_t *pulLocal;

	/* With large code and large data sizeof( StackType_t ) == 2, and
	sizeof( StackType_t * ) == 4.  With small code and small data
	sizeof( StackType_t ) == 2 and sizeof( StackType_t * ) == 2. */

	#if __DATA_MODEL__ == __DATA_MODEL_FAR__
	{
		/* Parameters are passed in on the stack, and written using a 32-bit value
		hence a space is left for the second two bytes. */
		pxTopOfStack--;

		/* Write in the parameter value. */
		pulLocal =  ( uint32_t * ) pxTopOfStack;
		*pulLocal = ( uint32_t ) pvParameters;
		pxTopOfStack--;

		/* The return address, leaving space for the first two bytes of	the
		32-bit value.  See the comments above the prvTaskExitError() prototype
		at the top of this file. */
		pxTopOfStack--;
		pulLocal = ( uint32_t * ) pxTopOfStack;
		*pulLocal = ( uint32_t ) prvTaskExitError;
		pxTopOfStack--;

		/* The start address / PSW value is also written in as a 32-bit value,
		so leave a space for the second two bytes. */
		pxTopOfStack--;

		/* Task function start address combined with the PSW. */
		pulLocal = ( uint32_t * ) pxTopOfStack;
		*pulLocal = ( ( ( uint32_t ) pxCode ) | ( portPSW << 24UL ) );
		pxTopOfStack--;

		/* An initial value for the AX register. */
		*pxTopOfStack = ( StackType_t ) 0x1111;
		pxTopOfStack--;
	}
	#else
	{
		/* The return address, leaving space for the first two bytes of	the
		32-bit value.  See the comments above the prvTaskExitError() prototype
		at the top of this file. */
		pxTopOfStack--;
		pulLocal = ( uint32_t * ) pxTopOfStack;
		*pulLocal = ( uint32_t ) prvTaskExitError;
		pxTopOfStack--;

		/* Task function.  Again as it is written as a 32-bit value a space is
		left on the stack for the second two bytes. */
		pxTopOfStack--;

		/* Task function start address combined with the PSW. */
		pulLocal = ( uint32_t * ) pxTopOfStack;
		*pulLocal = ( ( ( uint32_t ) pxCode ) | ( portPSW << 24UL ) );
		pxTopOfStack--;

		/* The parameter is passed in AX. */
		*pxTopOfStack = ( StackType_t ) pvParameters;
		pxTopOfStack--;
	}
	#endif

	/* An initial value for the HL register. */
	*pxTopOfStack = ( StackType_t ) 0x2222;
	pxTopOfStack--;

	/* CS and ES registers. */
	*pxTopOfStack = ( StackType_t ) 0x0F00;
	pxTopOfStack--;

	/* The remaining general purpose registers DE and BC */
	*pxTopOfStack = ( StackType_t ) 0xDEDE;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xBCBC;
	pxTopOfStack--;

	/* Finally the critical section nesting count is set to zero when the task
	first starts. */
	*pxTopOfStack = ( StackType_t ) portNO_CRITICAL_SECTION_NESTING;

	/* Return a pointer to the top of the stack that has been generated so it
	can	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( usCriticalNesting == ~0U );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Setup the hardware to generate the tick.  Interrupts are disabled when
	this function is called. */
	configSETUP_TICK_INTERRUPT();

	/* Restore the context of the first task that is going to run. */
	vPortStartFirstTask();

	/* Execution should not reach here as the tasks are now running!
	prvSetupTimerInterrupt() is called here to prevent the compiler outputting
	a warning about a statically declared function not being referenced in the
	case that the application writer has provided their own tick interrupt
	configuration routine (and defined configSETUP_TICK_INTERRUPT() such that
	their own routine will be called in place of prvSetupTimerInterrupt()). */
	prvSetupTimerInterrupt();
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the RL78 port will get stopped. */
}
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
const uint16_t usClockHz = 15000UL; /* Internal clock. */
const uint16_t usCompareMatch = ( usClockHz / configTICK_RATE_HZ ) + 1UL;

	/* Use the internal 15K clock. */
	OSMC = ( uint8_t ) 0x16;

	#ifdef RTCEN
	{
		/* Supply the interval timer clock. */
		RTCEN = ( uint8_t ) 1U;

		/* Disable INTIT interrupt. */
		ITMK = ( uint8_t ) 1;

		/* Disable ITMC operation. */
		ITMC = ( uint8_t ) 0x0000;

		/* Clear INIT interrupt. */
		ITIF = ( uint8_t ) 0;

		/* Set interval and enable interrupt operation. */
		ITMC = usCompareMatch | 0x8000U;

		/* Enable INTIT interrupt. */
		ITMK = ( uint8_t ) 0;
	}
	#endif

	#ifdef TMKAEN
	{
		/* Supply the interval timer clock. */
		TMKAEN = ( uint8_t ) 1U;

		/* Disable INTIT interrupt. */
		TMKAMK = ( uint8_t ) 1;

		/* Disable ITMC operation. */
		ITMC = ( uint8_t ) 0x0000;

		/* Clear INIT interrupt. */
		TMKAIF = ( uint8_t ) 0;

		/* Set interval and enable interrupt operation. */
		ITMC = usCompareMatch | 0x8000U;

		/* Enable INTIT interrupt. */
		TMKAMK = ( uint8_t ) 0;
	}
	#endif
}
/*-----------------------------------------------------------*/

