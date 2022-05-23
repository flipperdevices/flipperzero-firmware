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
 * Implementation of functions defined in portable.h for the MicroBlaze port.
 *----------------------------------------------------------*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <string.h>

/* Hardware includes. */
#include <xintc_i.h>
#include <xil_exception.h>
#include <microblaze_exceptions_g.h>

/* Tasks are started with a critical section nesting of 0 - however, prior to
the scheduler being commenced interrupts should not be enabled, so the critical
nesting variable is initialised to a non-zero value. */
#define portINITIAL_NESTING_VALUE	( 0xff )

/* The bit within the MSR register that enabled/disables interrupts and 
exceptions respectively. */
#define portMSR_IE					( 0x02U )
#define portMSR_EE					( 0x100U )

/* If the floating point unit is included in the MicroBlaze build, then the
FSR register is saved as part of the task context.  portINITIAL_FSR is the value
given to the FSR register when the initial context is set up for a task being
created. */
#define portINITIAL_FSR				( 0U )
/*-----------------------------------------------------------*/

/*
 * Initialise the interrupt controller instance.
 */
static int32_t prvInitialiseInterruptController( void );

/* Ensure the interrupt controller instance variable is initialised before it is
 * used, and that the initialisation only happens once.
 */
static int32_t prvEnsureInterruptControllerIsInitialised( void );

/*-----------------------------------------------------------*/

/* Counts the nesting depth of calls to portENTER_CRITICAL().  Each task
maintains its own count, so this variable is saved as part of the task
context. */
volatile UBaseType_t uxCriticalNesting = portINITIAL_NESTING_VALUE;

/* This port uses a separate stack for interrupts.  This prevents the stack of
every task needing to be large enough to hold an entire interrupt stack on top
of the task stack. */
uint32_t *pulISRStack;

/* If an interrupt requests a context switch, then ulTaskSwitchRequested will
get set to 1.  ulTaskSwitchRequested is inspected just before the main interrupt
handler exits.  If, at that time, ulTaskSwitchRequested is set to 1, the kernel
will call vTaskSwitchContext() to ensure the task that runs immediately after
the interrupt exists is the highest priority task that is able to run.  This is
an unusual mechanism, but is used for this port because a single interrupt can
cause the servicing of multiple peripherals - and it is inefficient to call
vTaskSwitchContext() multiple times as each peripheral is serviced. */
volatile uint32_t ulTaskSwitchRequested = 0UL;

/* The instance of the interrupt controller used by this port.  This is required
by the Xilinx library API functions. */
static XIntc xInterruptControllerInstance;

/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been made.
 *
 * See the portable.h header file.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
extern void *_SDA2_BASE_, *_SDA_BASE_;
const uint32_t ulR2 = ( uint32_t ) &_SDA2_BASE_;
const uint32_t ulR13 = ( uint32_t ) &_SDA_BASE_;

	/* Place a few bytes of known values on the bottom of the stack.
	This is essential for the Microblaze port and these lines must
	not be omitted. */
	*pxTopOfStack = ( StackType_t ) 0x00000000;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;
	pxTopOfStack--;

	#if( XPAR_MICROBLAZE_USE_FPU != 0 )
		/* The FSR value placed in the initial task context is just 0. */
		*pxTopOfStack = portINITIAL_FSR;
		pxTopOfStack--;
	#endif

	/* The MSR value placed in the initial task context should have interrupts
	disabled.  Each task will enable interrupts automatically when it enters
	the running state for the first time. */
	*pxTopOfStack = mfmsr() & ~portMSR_IE;
	
	#if( MICROBLAZE_EXCEPTIONS_ENABLED == 1 )
	{
		/* Ensure exceptions are enabled for the task. */
		*pxTopOfStack |= portMSR_EE;
	}
	#endif

	pxTopOfStack--;

	/* First stack an initial value for the critical section nesting.  This
	is initialised to zero. */
	*pxTopOfStack = ( StackType_t ) 0x00;

	/* R0 is always zero. */
	/* R1 is the SP. */

	/* Place an initial value for all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) ulR2;	/* R2 - read only small data area. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x03;	/* R3 - return values and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x04;	/* R4 - return values and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters;/* R5 contains the function call parameters. */

	#ifdef portPRE_LOAD_STACK_FOR_DEBUGGING
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x06;	/* R6 - other parameters and temporaries.  Used as the return address from vPortTaskEntryPoint. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x07;	/* R7 - other parameters and temporaries. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x08;	/* R8 - other parameters and temporaries. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x09;	/* R9 - other parameters and temporaries. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x0a;	/* R10 - other parameters and temporaries. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x0b;	/* R11 - temporaries. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x0c;	/* R12 - temporaries. */
		pxTopOfStack--;
	#else
		pxTopOfStack-= 8;
	#endif

	*pxTopOfStack = ( StackType_t ) ulR13;	/* R13 - read/write small data area. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;	/* R14 - return address for interrupt. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) NULL;	/* R15 - return address for subroutine. */

	#ifdef portPRE_LOAD_STACK_FOR_DEBUGGING
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x10;	/* R16 - return address for trap (debugger). */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x11;	/* R17 - return address for exceptions, if configured. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x12;	/* R18 - reserved for assembler and compiler temporaries. */
		pxTopOfStack--;
	#else
		pxTopOfStack -= 4;
	#endif

	*pxTopOfStack = ( StackType_t ) 0x00;	/* R19 - must be saved across function calls. Callee-save.  Seems to be interpreted as the frame pointer. */

	#ifdef portPRE_LOAD_STACK_FOR_DEBUGGING
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x14;	/* R20 - reserved for storing a pointer to the Global Offset Table (GOT) in Position Independent Code (PIC). Non-volatile in non-PIC code. Must be saved across function calls. Callee-save.  Not used by FreeRTOS. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x15;	/* R21 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x16;	/* R22 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x17;	/* R23 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x18;	/* R24 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x19;	/* R25 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1a;	/* R26 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1b;	/* R27 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1c;	/* R28 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1d;	/* R29 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1e;	/* R30 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x1f;	/* R31 - must be saved across function calls. Callee-save. */
		pxTopOfStack--;
	#else
		pxTopOfStack -= 13;
	#endif

	/* Return a pointer to the top of the stack that has been generated so this
	can	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void ( vPortStartFirstTask )( void );
extern uint32_t _stack[];

	/* Setup the hardware to generate the tick.  Interrupts are disabled when
	this function is called.

	This port uses an application defined callback function to install the tick
	interrupt handler because the kernel will run on lots of different
	MicroBlaze and FPGA configurations - not all of	which will have the same
	timer peripherals defined or available.  An example definition of
	vApplicationSetupTimerInterrupt() is provided in the official demo
	application that accompanies this port. */
	vApplicationSetupTimerInterrupt();

	/* Reuse the stack from main() as the stack for the interrupts/exceptions. */
	pulISRStack = ( uint32_t * ) _stack;

	/* Ensure there is enough space for the functions called from the interrupt
	service routines to write back into the stack frame of the caller. */
	pulISRStack -= 2;

	/* Restore the context of the first task that is going to run.  From here
	on, the created tasks will be executing. */
	vPortStartFirstTask();

	/* Should not get here as the tasks are now running! */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch called by portYIELD or taskYIELD.
 */
void vPortYield( void )
{
extern void VPortYieldASM( void );

	/* Perform the context switch in a critical section to assure it is
	not interrupted by the tick ISR.  It is not a problem to do this as
	each task maintains its own interrupt status. */
	portENTER_CRITICAL();
	{
		/* Jump directly to the yield function to ensure there is no
		compiler generated prologue code. */
		asm volatile (	"bralid r14, VPortYieldASM		\n\t" \
						"or r0, r0, r0					\n\t" );
	}
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

void vPortEnableInterrupt( uint8_t ucInterruptID )
{
int32_t lReturn;

	/* An API function is provided to enable an interrupt in the interrupt
	controller because the interrupt controller instance variable is private
	to this file. */
	lReturn = prvEnsureInterruptControllerIsInitialised();
	if( lReturn == pdPASS )
	{
		XIntc_Enable( &xInterruptControllerInstance, ucInterruptID );
	}

	configASSERT( lReturn );
}
/*-----------------------------------------------------------*/

void vPortDisableInterrupt( uint8_t ucInterruptID )
{
int32_t lReturn;

	/* An API function is provided to disable an interrupt in the interrupt
	controller because the interrupt controller instance variable is private
	to this file. */
	lReturn = prvEnsureInterruptControllerIsInitialised();

	if( lReturn == pdPASS )
	{
		XIntc_Disable( &xInterruptControllerInstance, ucInterruptID );
	}

	configASSERT( lReturn );
}
/*-----------------------------------------------------------*/

BaseType_t xPortInstallInterruptHandler( uint8_t ucInterruptID, XInterruptHandler pxHandler, void *pvCallBackRef )
{
int32_t lReturn;

	/* An API function is provided to install an interrupt handler because the
	interrupt controller instance variable is private to this file. */

	lReturn = prvEnsureInterruptControllerIsInitialised();

	if( lReturn == pdPASS )
	{
		lReturn = XIntc_Connect( &xInterruptControllerInstance, ucInterruptID, pxHandler, pvCallBackRef );
	}

	if( lReturn == XST_SUCCESS )
	{
		lReturn = pdPASS;
	}

	configASSERT( lReturn == pdPASS );

	return lReturn;
}
/*-----------------------------------------------------------*/

static int32_t prvEnsureInterruptControllerIsInitialised( void )
{
static int32_t lInterruptControllerInitialised = pdFALSE;
int32_t lReturn;

	/* Ensure the interrupt controller instance variable is initialised before
	it is used, and that the initialisation only happens once. */
	if( lInterruptControllerInitialised != pdTRUE )
	{
		lReturn = prvInitialiseInterruptController();

		if( lReturn == pdPASS )
		{
			lInterruptControllerInitialised = pdTRUE;
		}
	}
	else
	{
		lReturn = pdPASS;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

/*
 * Handler for the timer interrupt.  This is the handler that the application
 * defined callback function vApplicationSetupTimerInterrupt() should install.
 */
void vPortTickISR( void *pvUnused )
{
extern void vApplicationClearTimerInterrupt( void );

	/* Ensure the unused parameter does not generate a compiler warning. */
	( void ) pvUnused;

	/* This port uses an application defined callback function to clear the tick
	interrupt because the kernel will run on lots of different MicroBlaze and
	FPGA configurations - not all of which will have the same timer peripherals
	defined or available.  An example definition of
	vApplicationClearTimerInterrupt() is provided in the official demo
	application that accompanies this port. */
	vApplicationClearTimerInterrupt();

	/* Increment the RTOS tick - this might cause a task to unblock. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		/* Force vTaskSwitchContext() to be called as the interrupt exits. */
		ulTaskSwitchRequested = 1;
	}
}
/*-----------------------------------------------------------*/

static int32_t prvInitialiseInterruptController( void )
{
int32_t lStatus;

	lStatus = XIntc_Initialize( &xInterruptControllerInstance, configINTERRUPT_CONTROLLER_TO_USE );

	if( lStatus == XST_SUCCESS )
	{
		/* Initialise the exception table. */
		Xil_ExceptionInit();

	    /* Service all pending interrupts each time the handler is entered. */
	    XIntc_SetIntrSvcOption( xInterruptControllerInstance.BaseAddress, XIN_SVC_ALL_ISRS_OPTION );

	    /* Install exception handlers if the MicroBlaze is configured to handle
	    exceptions, and the application defined constant
	    configINSTALL_EXCEPTION_HANDLERS is set to 1. */
		#if ( MICROBLAZE_EXCEPTIONS_ENABLED == 1 ) && ( configINSTALL_EXCEPTION_HANDLERS == 1 )
	    {
	    	vPortExceptionsInstallHandlers();
	    }
		#endif /* MICROBLAZE_EXCEPTIONS_ENABLED */

		/* Start the interrupt controller.  Interrupts are enabled when the
		scheduler starts. */
		lStatus = XIntc_Start( &xInterruptControllerInstance, XIN_REAL_MODE );

		if( lStatus == XST_SUCCESS )
		{
			lStatus = pdPASS;
		}
		else
		{
			lStatus = pdFAIL;
		}
	}

	configASSERT( lStatus == pdPASS );

	return lStatus;
}
/*-----------------------------------------------------------*/


