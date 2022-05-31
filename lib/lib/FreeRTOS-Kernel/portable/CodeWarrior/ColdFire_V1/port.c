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

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"


#define portINITIAL_FORMAT_VECTOR		( ( StackType_t ) 0x4000 )

/* Supervisor mode set. */
#define portINITIAL_STATUS_REGISTER		( ( StackType_t ) 0x2000)

/* The clock prescale into the timer peripheral. */
#define portPRESCALE_VALUE				( ( uint8_t ) 10 )

/* The clock frequency into the RTC. */
#define portRTC_CLOCK_HZ				( ( uint32_t ) 1000 )

asm void interrupt VectorNumber_VL1swi vPortYieldISR( void );
static void prvSetupTimerInterrupt( void );

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
static uint32_t ulCriticalNesting = 0x9999UL;

/*-----------------------------------------------------------*/

StackType_t *pxPortInitialiseStack( StackType_t * pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{

uint32_t ulOriginalA5;

	__asm{ MOVE.L A5, ulOriginalA5 };


	*pxTopOfStack = (StackType_t) 0xDEADBEEF;
	pxTopOfStack--;

	/* Exception stack frame starts with the return address. */
	*pxTopOfStack = ( StackType_t ) pxCode;
	pxTopOfStack--;

	*pxTopOfStack = ( portINITIAL_FORMAT_VECTOR << 16UL ) | ( portINITIAL_STATUS_REGISTER );
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) 0x0; /*FP*/
	pxTopOfStack -= 14; /* A5 to D0. */

	/* Parameter in A0. */
	*( pxTopOfStack + 8 ) = ( StackType_t ) pvParameters;

	/* A5 must be maintained as it is resurved by the compiler. */
	*( pxTopOfStack + 13 ) = ulOriginalA5;

	return pxTopOfStack;  
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void vPortStartFirstTask( void );

	ulCriticalNesting = 0UL;

	/* Configure a timer to generate the tick interrupt. */
	prvSetupTimerInterrupt();

	/* Start the first task executing. */
	vPortStartFirstTask();

	return pdFALSE;
}
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{				
	/* Prescale by 1 - ie no prescale. */
	RTCSC |= 8;
	
	/* Compare match value. */
	RTCMOD = portRTC_CLOCK_HZ / configTICK_RATE_HZ;
	
	/* Enable the RTC to generate interrupts - interrupts are already disabled
	when this code executes. */
	RTCSC_RTIE = 1;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented as there is nothing to return to. */
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	if( ulCriticalNesting == 0UL )
	{
		/* Guard against context switches being pended simultaneously with a
		critical section being entered. */
		do
		{
			portDISABLE_INTERRUPTS();
			if( INTC_FRC == 0UL )
			{
				break;
			}

			portENABLE_INTERRUPTS();

		} while( 1 );
	}
	ulCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	ulCriticalNesting--;
	if( ulCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

void vPortYieldHandler( void )
{
uint32_t ulSavedInterruptMask;

	ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* Note this will clear all forced interrupts - this is done for speed. */
		INTC_CFRC = 0x3E;
		vTaskSwitchContext();
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulSavedInterruptMask );
}
/*-----------------------------------------------------------*/

void interrupt VectorNumber_Vrtc vPortTickISR( void )
{
uint32_t ulSavedInterruptMask;

	/* Clear the interrupt. */
	RTCSC |= RTCSC_RTIF_MASK;

	/* Increment the RTOS tick. */
	ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		if( xTaskIncrementTick() != pdFALSE )
		{
			taskYIELD();
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulSavedInterruptMask );
}

