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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <machine/cpu.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH							( -1 )
#define portTICK_PERIOD_MS							( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT							4
#define portNOP()									__asm__ volatile ( "mov r0, r0" )
#define portCRITICAL_NESTING_IN_TCB					1
#define portIRQ_TRAP_YIELD							31
/*-----------------------------------------------------------*/

/* Task utilities. */

extern void vPortYield( void );

/*---------------------------------------------------------------------------*/

#define portYIELD()		asm __volatile__( " trap #%0 "::"i"(portIRQ_TRAP_YIELD):"memory")
/*---------------------------------------------------------------------------*/

extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()		vTaskEnterCritical()
#define portEXIT_CRITICAL()			vTaskExitCritical()
/*---------------------------------------------------------------------------*/

/* Critical section management. */
#define portDISABLE_INTERRUPTS() 	cpu_int_disable()
#define portENABLE_INTERRUPTS() 	cpu_int_enable()

/*---------------------------------------------------------------------------*/

#define portYIELD_FROM_ISR( xHigherPriorityTaskWoken ) do { if( xHigherPriorityTaskWoken != pdFALSE ) vTaskSwitchContext(); } while( 0 )

/*---------------------------------------------------------------------------*/

#define portSAVE_CONTEXT()				\
	asm __volatile__																								\
	(																												\
		"sub	r1, #68					\n" /* Make space on the stack for the context. */							\
		"std	r2, [r1] + 	0			\n"																			\
		"stq	r4, [r1] +	8			\n"																			\
		"stq	r8, [r1] +	24			\n"																			\
		"stq	r12, [r1] +	40			\n"																			\
		"mov	r6, rtt					\n"																			\
		"mov	r7, psr					\n"																			\
		"std	r6, [r1] +	56			\n"																			\
		"movhi	r2, #16384				\n"	/* Set the pointer to the IC. */										\
		"ldub	r3, [r2] + 2			\n"	/* Load the current interrupt mask. */									\
		"st		r3, [r1]+ 64			\n"	/* Store the interrupt mask on the stack. */ 							\
		"ld		r2, [r0]+short(pxCurrentTCB)	\n"	/* Load the pointer to the TCB. */								\
		"st		r1, [r2]				\n"	/* Save the stack pointer into the TCB. */								\
		"mov	r14, r1					\n"	/* Compiler expects r14 to be set to the function stack. */				\
	);
/*---------------------------------------------------------------------------*/

#define portRESTORE_CONTEXT()																						\
	asm __volatile__(																								\
		"ld		r2, [r0]+short(pxCurrentTCB)	\n"	/* Load the TCB to find the stack pointer and context. */		\
		"ld		r1, [r2]				\n"																			\
		"movhi	r2, #16384				\n"	/* Set the pointer to the IC. */										\
		"ld		r3, [r1] + 64			\n"	/* Load the previous interrupt mask. */									\
		"stb	r3, [r2] + 2  			\n"	/* Set the current interrupt mask to be the previous. */				\
		"ldd	r6, [r1] + 56			\n"	/* Restore context. */													\
		"mov	rtt, r6					\n"																			\
		"mov	psr, r7					\n"																			\
		"ldd	r2, [r1] + 0			\n"																			\
		"ldq	r4, [r1] +	8			\n"																			\
		"ldq	r8, [r1] +	24			\n"																			\
		"ldq	r12, [r1] +	40			\n"																			\
		"add	r1, #68					\n"																			\
		"rti							\n"																			\
	 );

/*---------------------------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
