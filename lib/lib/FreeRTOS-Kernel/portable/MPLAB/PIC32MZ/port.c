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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the PIC32MZ port.
  *----------------------------------------------------------*/

/* Microchip specific headers. */
#include <xc.h>

/* Standard headers. */
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#if !defined(__PIC32MZ__)
    #error This port is designed to work with XC32 on PIC32MZ MCUs.  Please update your C compiler version or settings.
#endif

#if( ( configMAX_SYSCALL_INTERRUPT_PRIORITY >= 0x7 ) || ( configMAX_SYSCALL_INTERRUPT_PRIORITY == 0 ) )
	#error configMAX_SYSCALL_INTERRUPT_PRIORITY must be less than 7 and greater than 0
#endif

/* Hardware specifics. */
#define portTIMER_PRESCALE	8
#define portPRESCALE_BITS	1

/* Bits within various registers. */
#define portIE_BIT					( 0x00000001 )
#define portEXL_BIT					( 0x00000002 )
#define portMX_BIT					( 0x01000000 ) /* Allow access to DSP instructions. */
#define portCU1_BIT					( 0x20000000 ) /* enable CP1 for parts with hardware. */
#define portFR_BIT					( 0x04000000 ) /* Enable 64 bit floating point registers. */

/* Bits within the CAUSE register. */
#define portCORE_SW_0				( 0x00000100 )
#define portCORE_SW_1				( 0x00000200 )

/* The EXL bit is set to ensure interrupts do not occur while the context of
the first task is being restored. */
#if ( __mips_hard_float == 1 )
    #define portINITIAL_SR			( portIE_BIT | portEXL_BIT | portMX_BIT | portFR_BIT | portCU1_BIT )
#else
    #define portINITIAL_SR			( portIE_BIT | portEXL_BIT | portMX_BIT )
#endif

/* The initial value to store into the FPU status and control register. This is
 only used on parts that support a hardware FPU. */
#define portINITIAL_FPSCR			(0x1000000) /* High perf on denormal ops */


/*
By default port.c generates its tick interrupt from TIMER1.  The user can
override this behaviour by:
	1: Providing their own implementation of vApplicationSetupTickTimerInterrupt(),
	   which is the function that configures the timer.  The function is defined
	   as a weak symbol in this file so if the same function name is used in the
	   application code then the version in the application code will be linked
	   into the application in preference to the version defined in this file.
	2: Define configTICK_INTERRUPT_VECTOR to the vector number of the timer used
	   to generate the tick interrupt.  For example, when timer 1 is used then
	   configTICK_INTERRUPT_VECTOR is set to _TIMER_1_VECTOR.
	   configTICK_INTERRUPT_VECTOR should be defined in FreeRTOSConfig.h.
	3: Define configCLEAR_TICK_TIMER_INTERRUPT() to clear the interrupt in the
	   timer used to generate the tick interrupt.  For example, when timer 1 is
	   used configCLEAR_TICK_TIMER_INTERRUPT() is defined to
	   IFS0CLR = _IFS0_T1IF_MASK.
*/
#ifndef configTICK_INTERRUPT_VECTOR
	#define configTICK_INTERRUPT_VECTOR _TIMER_1_VECTOR
	#define configCLEAR_TICK_TIMER_INTERRUPT() IFS0CLR = _IFS0_T1IF_MASK
#else
	#ifndef configCLEAR_TICK_TIMER_INTERRUPT
		#error If configTICK_INTERRUPT_VECTOR is defined in application code then configCLEAR_TICK_TIMER_INTERRUPT must also be defined in application code.
	#endif
#endif

/* Let the user override the pre-loading of the initial RA with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger - in which case configTASK_RETURN_ADDRESS can be defined as 0 (NULL). */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if( configCHECK_FOR_STACK_OVERFLOW > 2 )

	/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define portISR_STACK_FILL_BYTE	0xee

	static const uint8_t ucExpectedStackBytes[] = {
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE };	\

	#define portCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
	/* Define the function away. */
	#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* Records the interrupt nesting depth.  This is initialised to one as it is
decremented to 0 when the first task starts. */
volatile UBaseType_t uxInterruptNesting = 0x01;

/* Stores the task stack pointer when a switch is made to use the system stack. */
UBaseType_t uxSavedTaskStackPointer = 0;

/* The stack used by interrupt service routines that cause a context switch. */
__attribute__ ((aligned(8))) StackType_t xISRStack[ configISR_STACK_SIZE ] = { 0 };

/* The top of stack value ensures there is enough space to store 6 registers on
the callers stack, as some functions seem to want to do this.  8 byte alignment
is required to allow double word floating point stack pushes generated by the
compiler. */
const StackType_t * const xISRStackTop = &( xISRStack[ ( configISR_STACK_SIZE & ~portBYTE_ALIGNMENT_MASK ) - 8 ] );

/* Saved as part of the task context. Set to pdFALSE if the task does not
 require an FPU context. */
#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )
	uint32_t ulTaskHasFPUContext = 0;
#endif

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Ensure 8 byte alignment is maintained when leaving this function. */
	pxTopOfStack--;
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) 0xDEADBEEF;
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) 0x12345678;	/* Word to which the stack pointer will be left pointing after context restore. */
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) _CP0_GET_CAUSE();
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) portINITIAL_SR;/* CP0_STATUS */
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) pxCode; 		/* CP0_EPC */
	pxTopOfStack--;

	*pxTopOfStack = (StackType_t) 0x00000000;	/* DSPControl */
	pxTopOfStack -= 7;  						/* Includes space for AC1 - AC3. */

	*pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;	/* ra */
	pxTopOfStack -= 15;

	*pxTopOfStack = (StackType_t) pvParameters; /* Parameters to pass in. */
	pxTopOfStack -= 15;

	*pxTopOfStack = (StackType_t) pdFALSE; /*by default disable FPU context save on parts with FPU */

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
	configASSERT( uxSavedTaskStackPointer == 0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.  This function uses peripheral timer 1.
 * The function is declared weak so an application writer can use a different
 * timer by redefining this implementation.  If a different timer is used then
 * configTICK_INTERRUPT_VECTOR must also be defined in FreeRTOSConfig.h to
 * ensure the RTOS provided tick interrupt handler is installed on the correct
 * vector number.  When Timer 1 is used the vector number is defined as
 * _TIMER_1_VECTOR.
 */
__attribute__(( weak )) void vApplicationSetupTickTimerInterrupt( void )
{
const uint32_t ulCompareMatch = ( (configPERIPHERAL_CLOCK_HZ / portTIMER_PRESCALE) / configTICK_RATE_HZ ) - 1UL;

	T1CON = 0x0000;
	T1CONbits.TCKPS = portPRESCALE_BITS;
	PR1 = ulCompareMatch;
	IPC1bits.T1IP = configKERNEL_INTERRUPT_PRIORITY;

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T1IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T1IE = 1;

	/* Start the timer. */
	T1CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( uxInterruptNesting == 1000UL );
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void vPortStartFirstTask( void );
extern void *pxCurrentTCB;

	#if ( configCHECK_FOR_STACK_OVERFLOW > 2 )
	{
		/* Fill the ISR stack to make it easy to asses how much is being used. */
		memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
	}
	#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

	/* Clear the software interrupt flag. */
	IFS0CLR = _IFS0_CS0IF_MASK;

	/* Set software timer priority. */
	IPC0CLR = _IPC0_CS0IP_MASK;
	IPC0SET = ( configKERNEL_INTERRUPT_PRIORITY << _IPC0_CS0IP_POSITION );

	/* Enable software interrupt. */
	IEC0CLR = _IEC0_CS0IE_MASK;
	IEC0SET = 1 << _IEC0_CS0IE_POSITION;

	/* Setup the timer to generate the tick.  Interrupts will have been
	disabled by the time we get here. */
	vApplicationSetupTickTimerInterrupt();

	/* Kick off the highest priority task that has been created so far.
	Its stack location is loaded into uxSavedTaskStackPointer. */
	uxSavedTaskStackPointer = *( UBaseType_t * ) pxCurrentTCB;
	vPortStartFirstTask();

	/* Should never get here as the tasks will now be executing!  Call the task
	exit error function to prevent compiler warnings about a static function
	not being called in the case that the application writer overrides this
	functionality by defining configTASK_RETURN_ADDRESS. */
	prvTaskExitError();

	return pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortIncrementTick( void )
{
UBaseType_t uxSavedStatus;

	uxSavedStatus = uxPortSetInterruptMaskFromISR();
	{
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* Pend a context switch. */
			_CP0_BIS_CAUSE( portCORE_SW_0 );
		}
	}
	vPortClearInterruptMaskFromISR( uxSavedStatus );

	/* Look for the ISR stack getting near or past its limit. */
	portCHECK_ISR_STACK();

	/* Clear timer interrupt. */
	configCLEAR_TICK_TIMER_INTERRUPT();
}
/*-----------------------------------------------------------*/

UBaseType_t uxPortSetInterruptMaskFromISR( void )
{
UBaseType_t uxSavedStatusRegister;

	__builtin_disable_interrupts();
	uxSavedStatusRegister = _CP0_GET_STATUS() | 0x01;
	/* This clears the IPL bits, then sets them to
	configMAX_SYSCALL_INTERRUPT_PRIORITY.  This function should not be called
	from an interrupt that has a priority above
	configMAX_SYSCALL_INTERRUPT_PRIORITY so, when used correctly, the action
	can only result in the IPL being unchanged or raised, and therefore never
	lowered. */
	_CP0_SET_STATUS( ( ( uxSavedStatusRegister & ( ~portALL_IPL_BITS ) ) ) | ( configMAX_SYSCALL_INTERRUPT_PRIORITY << portIPL_SHIFT ) );

	return uxSavedStatusRegister;
}
/*-----------------------------------------------------------*/

void vPortClearInterruptMaskFromISR( UBaseType_t uxSavedStatusRegister )
{
	_CP0_SET_STATUS( uxSavedStatusRegister );
}
/*-----------------------------------------------------------*/

#if ( __mips_hard_float == 1 ) && ( configUSE_TASK_FPU_SUPPORT == 1 )

	void vPortTaskUsesFPU(void)
	{
	extern void vPortInitialiseFPSCR( uint32_t uxFPSCRInit );

		portENTER_CRITICAL();

		/* Initialise the floating point status register. */
		vPortInitialiseFPSCR(portINITIAL_FPSCR);

		/* A task is registering the fact that it needs a FPU context. Set the
		FPU flag (saved as part of the task context). */
		ulTaskHasFPUContext = pdTRUE;

		portEXIT_CRITICAL();
	}

#endif /* __mips_hard_float == 1 */

/*-----------------------------------------------------------*/




