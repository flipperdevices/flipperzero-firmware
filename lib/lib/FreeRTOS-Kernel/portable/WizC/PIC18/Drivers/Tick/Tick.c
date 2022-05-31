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

/* 
Changes from V3.0.0
	+ ISRcode is pulled inline and portTICKisr() is therefore
	  deleted from this file.

	+ Prescaler logic for Timer1 added to allow for a wider
	  range of TickRates.

Changes from V3.0.1
*/

#include <FreeRTOS.h>
#include <task.h>

/* IO port constants. */
#define portBIT_SET		(1)
#define portBIT_CLEAR	(0)

/* 
 * Hardware setup for the tick.
 * We use a compare match on timer1. Depending on MPU-frequency
 * and requested tickrate, a prescaled value with a matching
 * prescaler are determined.
 */
#define	portTIMER_COMPARE_BASE			((APROCFREQ/4)/configTICK_RATE_HZ)

#if portTIMER_COMPARE_BASE   < 0x10000
	#define	portTIMER_COMPARE_VALUE		(portTIMER_COMPARE_BASE)
	#define portTIMER_COMPARE_PS1		(portBIT_CLEAR)
	#define portTIMER_COMPARE_PS0		(portBIT_CLEAR)
#elif portTIMER_COMPARE_BASE < 0x20000
	#define	portTIMER_COMPARE_VALUE		(portTIMER_COMPARE_BASE / 2)
	#define portTIMER_COMPARE_PS1		(portBIT_CLEAR)
	#define portTIMER_COMPARE_PS0		(portBIT_SET)
#elif portTIMER_COMPARE_BASE < 0x40000
	#define	portTIMER_COMPARE_VALUE		(portTIMER_COMPARE_BASE / 4)
	#define portTIMER_COMPARE_PS1		(portBIT_SET)
	#define portTIMER_COMPARE_PS0		(portBIT_CLEAR)
#elif portTIMER_COMPARE_BASE < 0x80000
	#define	portTIMER_COMPARE_VALUE		(portTIMER_COMPARE_BASE / 8)
	#define portTIMER_COMPARE_PS1		(portBIT_SET)
	#define portTIMER_COMPARE_PS0		(portBIT_SET)
#else
	#error "TickRate out of range"
#endif

/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
void portSetupTick( void )
{
	/*
	 * Interrupts are disabled when this function is called.
	 */

	/*
	 * Setup CCP1
	 * Provide the tick interrupt using a compare match on timer1.
	 */

	/*
	 * Set the compare match value.
	 */
	CCPR1H = ( uint8_t ) ( ( portTIMER_COMPARE_VALUE >> 8 ) & 0xff );
	CCPR1L = ( uint8_t )   ( portTIMER_COMPARE_VALUE & 0xff );

	/*
	 * Set Compare Special Event Trigger Mode
	 */
	bCCP1M3 	= portBIT_SET;
	bCCP1M2 	= portBIT_CLEAR;
	bCCP1M1 	= portBIT_SET;
	bCCP1M0		= portBIT_SET;

	/*
	 * Enable CCP1 interrupt
	 */
	bCCP1IE 	= portBIT_SET;

	/*
	 * We are only going to use the global interrupt bit, so disable
	 * interruptpriorities and enable peripheral interrupts.
	 */
	bIPEN		= portBIT_CLEAR;
	bPEIE		= portBIT_SET;

	/*
	 * Set up timer1
	 * It will produce the system tick.
	 */

	/*
	 * Clear the time count
	 */
	TMR1H = ( uint8_t ) 0x00;
	TMR1L = ( uint8_t ) 0x00;

	/*
	 * Setup the timer
	 */
	bRD16		= portBIT_SET;				// 16-bit
	bT1CKPS1	= portTIMER_COMPARE_PS1;	// prescaler
	bT1CKPS0	= portTIMER_COMPARE_PS0;	// prescaler
	bT1OSCEN	= portBIT_SET;				// Oscillator enable
	bT1SYNC		= portBIT_SET;				// No external clock sync
	bTMR1CS		= portBIT_CLEAR;			// Internal clock
	
	bTMR1ON		= portBIT_SET;				// Start timer1
}
