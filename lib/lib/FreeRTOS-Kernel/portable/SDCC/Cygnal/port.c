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
 * Implementation of functions defined in portable.h for the Cygnal port.
 *----------------------------------------------------------*/

/* Standard includes. */
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Constants required to setup timer 2 to produce the RTOS tick. */
#define portCLOCK_DIVISOR                               ( ( uint32_t ) 12 )
#define portMAX_TIMER_VALUE                             ( ( uint32_t ) 0xffff )
#define portENABLE_TIMER                                ( ( uint8_t ) 0x04 )
#define portTIMER_2_INTERRUPT_ENABLE                    ( ( uint8_t ) 0x20 )

/* The value used in the IE register when a task first starts. */
#define portGLOBAL_INTERRUPT_BIT                        ( ( StackType_t ) 0x80 )

/* The value used in the PSW register when a task first starts. */
#define portINITIAL_PSW                                 ( ( StackType_t ) 0x00 )

/* Macro to clear the timer 2 interrupt flag. */
#define portCLEAR_INTERRUPT_FLAG()                      TMR2CN &= ~0x80;

/* Used during a context switch to store the size of the stack being copied
to or from XRAM. */
data static uint8_t ucStackBytes;

/* Used during a context switch to point to the next byte in XRAM from/to which
a RAM byte is to be copied. */
xdata static StackType_t * data pxXRAMStack;

/* Used during a context switch to point to the next byte in RAM from/to which
an XRAM byte is to be copied. */
data static StackType_t * data pxRAMStack;

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/*
 * Setup the hardware to generate an interrupt off timer 2 at the required
 * frequency.
 */
static void prvSetupTimerInterrupt( void );

/*-----------------------------------------------------------*/
/*
 * Macro that copies the current stack from internal RAM to XRAM.  This is
 * required as the 8051 only contains enough internal RAM for a single stack,
 * but we have a stack for every task.
 */
#define portCOPY_STACK_TO_XRAM()                                                            \
{                                                                                           \
        /* pxCurrentTCB points to a TCB which itself points to the location into            \
        which the first stack byte should be copied. Set pxXRAMStack to point               \
        to the location into which the first stack byte is to be copied. */                 \
        pxXRAMStack = ( xdata StackType_t * ) *( ( xdata StackType_t ** ) pxCurrentTCB );   \
                                                                                            \
        /* Set pxRAMStack to point to the first byte to be coped from the stack. */         \
        pxRAMStack = ( data StackType_t * data ) configSTACK_START;                         \
                                                                                            \
        /* Calculate the size of the stack we are about to copy from the current            \
        stack pointer value. */                                                             \
        ucStackBytes = SP - ( configSTACK_START - 1 );                                      \
                                                                                            \
        /* Before starting to copy the stack, store the calculated stack size so            \
        the stack can be restored when the task is resumed. */                              \
        *pxXRAMStack = ucStackBytes;                                                        \
                                                                                            \
        /* Copy each stack byte in turn.  pxXRAMStack is incremented first as we            \
        have already stored the stack size into XRAM. */                                    \
        while( ucStackBytes )                                                               \
        {                                                                                   \
                pxXRAMStack++;                                                              \
                *pxXRAMStack = *pxRAMStack;                                                 \
                pxRAMStack++;                                                               \
                ucStackBytes--;                                                             \
        }                                                                                   \
}
/*-----------------------------------------------------------*/

/*
 * Macro that copies the stack of the task being resumed from XRAM into
 * internal RAM.
 */
#define portCOPY_XRAM_TO_STACK()                                                            \
{                                                                                           \
        /* Setup the pointers as per portCOPY_STACK_TO_XRAM(), but this time to             \
        copy the data back out of XRAM and into the stack. */                               \
        pxXRAMStack = ( xdata StackType_t * ) *( ( xdata StackType_t ** ) pxCurrentTCB );   \
        pxRAMStack = ( data StackType_t * data ) ( configSTACK_START - 1 );                 \
                                                                                            \
        /* The first value stored in XRAM was the size of the stack - i.e. the              \
        number of bytes we need to copy back. */                                            \
        ucStackBytes = pxXRAMStack[ 0 ];                                                    \
                                                                                            \
        /* Copy the required number of bytes back into the stack. */                        \
        do                                                                                  \
        {                                                                                   \
                pxXRAMStack++;                                                              \
                pxRAMStack++;                                                               \
                *pxRAMStack = *pxXRAMStack;                                                 \
                ucStackBytes--;                                                             \
        } while( ucStackBytes );                                                            \
                                                                                            \
        /* Restore the stack pointer ready to use the restored stack. */                    \
        SP = ( uint8_t ) pxRAMStack;                                                        \
}
/*-----------------------------------------------------------*/

/*
 * Macro to push the current execution context onto the stack, before the stack
 * is moved to XRAM.
 */
#define portSAVE_CONTEXT()                                                                  \
{                                                                                           \
        _asm                                                                                \
                /* Push ACC first, as when restoring the context it must be restored        \
                last (it is used to set the IE register). */                                \
                push        ACC                                                             \
                /* Store the IE register then disable interrupts. */                        \
                push        IE                                                              \
                clr                _EA                                                      \
                push        DPL                                                             \
                push        DPH                                                             \
                push        b                                                               \
                push        ar2                                                             \
                push        ar3                                                             \
                push        ar4                                                             \
                push        ar5                                                             \
                push        ar6                                                             \
                push        ar7                                                             \
                push        ar0                                                             \
                push        ar1                                                             \
                push        PSW                                                             \
        _endasm;                                                                            \
                PSW = 0;                                                                    \
        _asm                                                                                \
                push        _bp                                                             \
        _endasm;                                                                            \
}
/*-----------------------------------------------------------*/

/*
 * Macro that restores the execution context from the stack.  The execution
 * context was saved into the stack before the stack was copied into XRAM.
 */
#define portRESTORE_CONTEXT()                                                               \
{                                                                                           \
        _asm                                                                                \
                pop                _bp                                                      \
                pop                PSW                                                      \
                pop                ar1                                                      \
                pop                ar0                                                      \
                pop                ar7                                                      \
                pop                ar6                                                      \
                pop                ar5                                                      \
                pop                ar4                                                      \
                pop                ar3                                                      \
                pop                ar2                                                      \
                pop                b                                                        \
                pop                DPH                                                      \
                pop                DPL                                                      \
                /* The next byte of the stack is the IE register.  Only the global          \
                enable bit forms part of the task context.  Pop off the IE then set         \
                the global enable bit to match that of the stored IE register. */           \
                pop                ACC                                                      \
                JB                ACC.7,0098$                                               \
                CLR                IE.7                                                     \
                LJMP        0099$                                                           \
        0098$:                                                                              \
                SETB        IE.7                                                            \
        0099$:                                                                              \
                /* Finally pop off the ACC, which was the first register saved. */          \
                pop                ACC                                                      \
                reti                                                                        \
        _endasm;                                                                            \
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
uint32_t ulAddress;
StackType_t *pxStartOfStack;

        /* Leave space to write the size of the stack as the first byte. */
        pxStartOfStack = pxTopOfStack;
        pxTopOfStack++;

        /* Place a few bytes of known values on the bottom of the stack.
        This is just useful for debugging and can be uncommented if required.
        *pxTopOfStack = 0x11;
        pxTopOfStack++;
        *pxTopOfStack = 0x22;
        pxTopOfStack++;
        *pxTopOfStack = 0x33;
        pxTopOfStack++;
        */

        /* Simulate how the stack would look after a call to the scheduler tick
        ISR.

        The return address that would have been pushed by the MCU. */
        ulAddress = ( uint32_t ) pxCode;
        *pxTopOfStack = ( StackType_t ) ulAddress;
        ulAddress >>= 8;
        pxTopOfStack++;
        *pxTopOfStack = ( StackType_t ) ( ulAddress );
        pxTopOfStack++;

        /* Next all the registers will have been pushed by portSAVE_CONTEXT(). */
        *pxTopOfStack = 0xaa;        /* acc */
        pxTopOfStack++;

        /* We want tasks to start with interrupts enabled. */
        *pxTopOfStack = portGLOBAL_INTERRUPT_BIT;
        pxTopOfStack++;

        /* The function parameters will be passed in the DPTR and B register as
        a three byte generic pointer is used. */
        ulAddress = ( uint32_t ) pvParameters;
        *pxTopOfStack = ( StackType_t ) ulAddress;        /* DPL */
        ulAddress >>= 8;
        *pxTopOfStack++;
        *pxTopOfStack = ( StackType_t ) ulAddress;        /* DPH */
        ulAddress >>= 8;
        pxTopOfStack++;
        *pxTopOfStack = ( StackType_t ) ulAddress;        /* b */
        pxTopOfStack++;

        /* The remaining registers are straight forward. */
        *pxTopOfStack = 0x02;        /* R2 */
        pxTopOfStack++;
        *pxTopOfStack = 0x03;        /* R3 */
        pxTopOfStack++;
        *pxTopOfStack = 0x04;        /* R4 */
        pxTopOfStack++;
        *pxTopOfStack = 0x05;        /* R5 */
        pxTopOfStack++;
        *pxTopOfStack = 0x06;        /* R6 */
        pxTopOfStack++;
        *pxTopOfStack = 0x07;        /* R7 */
        pxTopOfStack++;
        *pxTopOfStack = 0x00;        /* R0 */
        pxTopOfStack++;
        *pxTopOfStack = 0x01;        /* R1 */
        pxTopOfStack++;
        *pxTopOfStack = 0x00;        /* PSW */
        pxTopOfStack++;
        *pxTopOfStack = 0xbb;        /* BP */

        /* Dont increment the stack size here as we don't want to include
        the stack size byte as part of the stack size count.

        Finally we place the stack size at the beginning. */
        *pxStartOfStack = ( StackType_t ) ( pxTopOfStack - pxStartOfStack );

        /* Unlike most ports, we return the start of the stack as this is where the
        size of the stack is stored. */
        return pxStartOfStack;
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
        /* Setup timer 2 to generate the RTOS tick. */
        prvSetupTimerInterrupt();

        /* Make sure we start with the expected SFR page.  This line should not
        really be required. */
        SFRPAGE = 0;

        /* Copy the stack for the first task to execute from XRAM into the stack,
        restore the task context from the new stack, then start running the task. */
        portCOPY_XRAM_TO_STACK();
        portRESTORE_CONTEXT();

        /* Should never get here! */
        return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
        /* Not implemented for this port. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch.  The first thing we do is save the registers so we
 * can use a naked attribute.
 */
void vPortYield( void ) _naked
{
        /* Save the execution context onto the stack, then copy the entire stack
        to XRAM.  This is necessary as the internal RAM is only large enough to
        hold one stack, and we want one per task.

        PERFORMANCE COULD BE IMPROVED BY ONLY COPYING TO XRAM IF A TASK SWITCH
        IS REQUIRED. */
        portSAVE_CONTEXT();
        portCOPY_STACK_TO_XRAM();

        /* Call the standard scheduler context switch function. */
        vTaskSwitchContext();

        /* Copy the stack of the task about to execute from XRAM into RAM and
        restore it's context ready to run on exiting. */
        portCOPY_XRAM_TO_STACK();
        portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 1
        void vTimer2ISR( void ) interrupt 5 _naked
        {
                /* Preemptive context switch function triggered by the timer 2 ISR.
                This does the same as vPortYield() (see above) with the addition
                of incrementing the RTOS tick count. */

                portSAVE_CONTEXT();
                portCOPY_STACK_TO_XRAM();

                if( xTaskIncrementTick() != pdFALSE )
                {
                        vTaskSwitchContext();
                }

                portCLEAR_INTERRUPT_FLAG();
                portCOPY_XRAM_TO_STACK();
                portRESTORE_CONTEXT();
        }
#else
        void vTimer2ISR( void ) interrupt 5
        {
                /* When using the cooperative scheduler the timer 2 ISR is only
                required to increment the RTOS tick count. */

                xTaskIncrementTick();
                portCLEAR_INTERRUPT_FLAG();
        }
#endif
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void )
{
uint8_t ucOriginalSFRPage;

/* Constants calculated to give the required timer capture values. */
const uint32_t ulTicksPerSecond = configCPU_CLOCK_HZ / portCLOCK_DIVISOR;
const uint32_t ulCaptureTime = ulTicksPerSecond / configTICK_RATE_HZ;
const uint32_t ulCaptureValue = portMAX_TIMER_VALUE - ulCaptureTime;
const uint8_t ucLowCaptureByte = ( uint8_t ) ( ulCaptureValue & ( uint32_t ) 0xff );
const uint8_t ucHighCaptureByte = ( uint8_t ) ( ulCaptureValue >> ( uint32_t ) 8 );

        /* NOTE:  This uses a timer only present on 8052 architecture. */

        /* Remember the current SFR page so we can restore it at the end of the
        function. */
        ucOriginalSFRPage = SFRPAGE;
        SFRPAGE = 0;

        /* TMR2CF can be left in its default state. */
        TMR2CF = ( uint8_t ) 0;

        /* Setup the overflow reload value. */
        RCAP2L = ucLowCaptureByte;
        RCAP2H = ucHighCaptureByte;

        /* The initial load is performed manually. */
        TMR2L = ucLowCaptureByte;
        TMR2H = ucHighCaptureByte;

        /* Enable the timer 2 interrupts. */
        IE |= portTIMER_2_INTERRUPT_ENABLE;

        /* Interrupts are disabled when this is called so the timer can be started
        here. */
        TMR2CN = portENABLE_TIMER;

        /* Restore the original SFR page. */
        SFRPAGE = ucOriginalSFRPage;
}




