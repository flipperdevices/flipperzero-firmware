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
* Implementation of functions defined in portable.h for the RXv3 DPFPU port.
*----------------------------------------------------------*/

#warning Testing for DFPU support in this port is not yet complete

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "string.h"

/* Hardware specifics. */
#if ( configINCLUDE_PLATFORM_H_INSTEAD_OF_IODEFINE_H == 1 )

    #include "platform.h"

#else /* configINCLUDE_PLATFORM_H_INSTEAD_OF_IODEFINE_H */

    #include "iodefine.h"

#endif /* configINCLUDE_PLATFORM_H_INSTEAD_OF_IODEFINE_H */

/*-----------------------------------------------------------*/

/* Tasks should start with interrupts enabled and in Supervisor mode, therefore
 * PSW is set with U and I set, and PM and IPL clear. */
#define portINITIAL_PSW     ( ( StackType_t ) 0x00030000 )
#define portINITIAL_FPSW    ( ( StackType_t ) 0x00000100 )
#define portINITIAL_DPSW    ( ( StackType_t ) 0x00000100 )
#define portINITIAL_DCMR    ( ( StackType_t ) 0x00000000 )
#define portINITIAL_DECNT   ( ( StackType_t ) 0x00000001 )

/* Tasks are not created with a DPFPU context, but can be given a DPFPU context
 * after they have been created.  A variable is stored as part of the tasks context
 * that holds portNO_DPFPU_CONTEXT if the task does not have a DPFPU context, or
 * any other value if the task does have a DPFPU context. */
#define portNO_DPFPU_CONTEXT    ( ( StackType_t ) 0 )
#define portHAS_DPFPU_CONTEXT   ( ( StackType_t ) 1 )

/* The space on the stack required to hold the DPFPU data registers.  This is 16
 * 64-bit registers. */
#define portDPFPU_DATA_REGISTER_WORDS   ( 16 * 2 )

/*-----------------------------------------------------------*/

/*
 * Function to start the first task executing - written in asm code as direct
 * access to registers is required.
 */
static void prvStartFirstTask( void );

/*
 * Software interrupt handler.  Performs the actual context switch (saving and
 * restoring of registers).  Written in asm code as direct register access is
 * required.
 */
__interrupt void vSoftwareInterruptISR( void );

/*
 * The tick ISR handler.  The peripheral used is configured by the application
 * via a hook/callback function.
 */
__interrupt void vTickISR( void );

/*-----------------------------------------------------------*/

/* Saved as part of the task context.  If ulPortTaskHasDPFPUContext is non-zero
 * then a DPFPU context must be saved and restored for the task. */
#if ( configUSE_TASK_DPFPU_SUPPORT == 1 )

    StackType_t ulPortTaskHasDPFPUContext = portNO_DPFPU_CONTEXT;

#endif /* configUSE_TASK_DPFPU_SUPPORT */

/* This is accessed by the inline assembler functions so is file scope for
 * convenience. */
extern void * pxCurrentTCB;
extern void vTaskSwitchContext( void );

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* R0 is not included as it is the stack pointer. */

    *pxTopOfStack = 0x00;
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_PSW;
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxCode;

    /* When debugging it can be useful if every register is set to a known
     * value.  Otherwise code space can be saved by just setting the registers
     * that need to be set. */
    #ifdef USE_FULL_REGISTER_INITIALISATION
        {
            pxTopOfStack--;
            *pxTopOfStack = 0xffffffff; /* r15. */
            pxTopOfStack--;
            *pxTopOfStack = 0xeeeeeeee;
            pxTopOfStack--;
            *pxTopOfStack = 0xdddddddd;
            pxTopOfStack--;
            *pxTopOfStack = 0xcccccccc;
            pxTopOfStack--;
            *pxTopOfStack = 0xbbbbbbbb;
            pxTopOfStack--;
            *pxTopOfStack = 0xaaaaaaaa;
            pxTopOfStack--;
            *pxTopOfStack = 0x99999999;
            pxTopOfStack--;
            *pxTopOfStack = 0x88888888;
            pxTopOfStack--;
            *pxTopOfStack = 0x77777777;
            pxTopOfStack--;
            *pxTopOfStack = 0x66666666;
            pxTopOfStack--;
            *pxTopOfStack = 0x55555555;
            pxTopOfStack--;
            *pxTopOfStack = 0x44444444;
            pxTopOfStack--;
            *pxTopOfStack = 0x33333333;
            pxTopOfStack--;
            *pxTopOfStack = 0x22222222;
            pxTopOfStack--;
        }
    #else /* ifdef USE_FULL_REGISTER_INITIALISATION */
        {
            pxTopOfStack -= 15;
        }
    #endif /* ifdef USE_FULL_REGISTER_INITIALISATION */

    *pxTopOfStack = ( StackType_t ) pvParameters; /* R1 */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_FPSW;
    pxTopOfStack--;
    *pxTopOfStack = 0x11111111; /* Accumulator 1. */
    pxTopOfStack--;
    *pxTopOfStack = 0x22222222; /* Accumulator 1. */
    pxTopOfStack--;
    *pxTopOfStack = 0x33333333; /* Accumulator 1. */
    pxTopOfStack--;
    *pxTopOfStack = 0x44444444; /* Accumulator 0. */
    pxTopOfStack--;
    *pxTopOfStack = 0x55555555; /* Accumulator 0. */
    pxTopOfStack--;
    *pxTopOfStack = 0x66666666; /* Accumulator 0. */

    #if ( configUSE_TASK_DPFPU_SUPPORT == 1 )
        {
            /* The task will start without a DPFPU context.  A task that
             * uses the DPFPU hardware must call vPortTaskUsesDPFPU() before
             * executing any floating point instructions. */
            pxTopOfStack--;
            *pxTopOfStack = portNO_DPFPU_CONTEXT;
        }
    #elif ( configUSE_TASK_DPFPU_SUPPORT == 2 )
        {
            /* The task will start with a DPFPU context.  Leave enough
             * space for the registers - and ensure they are initialised if desired. */
            #ifdef USE_FULL_REGISTER_INITIALISATION
                {
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1515.1515; /* DR15. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1414.1414; /* DR14. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1313.1313; /* DR13. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1212.1212; /* DR12. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1111.1111; /* DR11. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 1010.1010; /* DR10. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  909.0909; /* DR9. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  808.0808; /* DR8. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  707.0707; /* DR7. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  606.0606; /* DR6. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  505.0505; /* DR5. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  404.0404; /* DR4. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  303.0303; /* DR3. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  202.0202; /* DR2. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack =  101.0101; /* DR1. */
                    pxTopOfStack -= 2;
                    *(double *)pxTopOfStack = 9876.54321;/* DR0. */
                }
            #else /* ifdef USE_FULL_REGISTER_INITIALISATION */
                {
                    pxTopOfStack -= portDPFPU_DATA_REGISTER_WORDS;
                    memset( pxTopOfStack, 0x00, portDPFPU_DATA_REGISTER_WORDS * sizeof( StackType_t ) );
                }
            #endif /* ifdef USE_FULL_REGISTER_INITIALISATION */
            pxTopOfStack--;
            *pxTopOfStack = portINITIAL_DECNT; /* DECNT. */
            pxTopOfStack--;
            *pxTopOfStack = portINITIAL_DCMR;  /* DCMR. */
            pxTopOfStack--;
            *pxTopOfStack = portINITIAL_DPSW;  /* DPSW. */
        }
    #elif ( configUSE_TASK_DPFPU_SUPPORT == 0 )
        {
            /* Omit DPFPU support. */
        }
    #else /* if ( configUSE_TASK_DPFPU_SUPPORT == 1 ) */
        {
            #error Invalid configUSE_TASK_DPFPU_SUPPORT setting - configUSE_TASK_DPFPU_SUPPORT must be set to 0, 1, 2, or left undefined.
        }
    #endif /* if ( configUSE_TASK_DPFPU_SUPPORT == 1 ) */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_DPFPU_SUPPORT == 1 )

    void vPortTaskUsesDPFPU( void )
    {
        /* A task is registering the fact that it needs a DPFPU context.  Set the
         * DPFPU flag (which is saved as part of the task context). */
        ulPortTaskHasDPFPUContext = portHAS_DPFPU_CONTEXT;
    }

#endif /* configUSE_TASK_DPFPU_SUPPORT */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    extern void vApplicationSetupTimerInterrupt( void );

    /* Use pxCurrentTCB just so it does not get optimised away. */
    if( pxCurrentTCB != NULL )
    {
        /* Call an application function to set up the timer that will generate the
         * tick interrupt.  This way the application can decide which peripheral to
         * use.  A demo application is provided to show a suitable example. */
        vApplicationSetupTimerInterrupt();

        /* Enable the software interrupt. */
        _IEN( _ICU_SWINT ) = 1;

        /* Ensure the software interrupt is clear. */
        _IR( _ICU_SWINT ) = 0;

        /* Ensure the software interrupt is set to the kernel priority. */
        _IPR( _ICU_SWINT ) = configKERNEL_INTERRUPT_PRIORITY;

        /* Start the first task. */
        prvStartFirstTask();
    }

    /* Should not get here. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    configASSERT( pxCurrentTCB == NULL );

    /* The following line is just to prevent the symbol getting optimised away. */
    ( void ) vTaskSwitchContext();
}
/*-----------------------------------------------------------*/

static void prvStartFirstTask( void )
{
    __asm volatile
    (

        /* When starting the scheduler there is nothing that needs moving to the
         * interrupt stack because the function is not called from an interrupt.
         * Just ensure the current stack is the user stack. */
        "SETPSW		U						\n"\


        /* Obtain the location of the stack associated with which ever task
         * pxCurrentTCB is currently pointing to. */
        "MOV.L		#_pxCurrentTCB, R15		\n"\
        "MOV.L		[R15], R15				\n"\
        "MOV.L		[R15], R0				\n"\


        /* Restore the registers from the stack of the task pointed to by
         * pxCurrentTCB. */

        #if ( configUSE_TASK_DPFPU_SUPPORT == 1 )

            /* The restored ulPortTaskHasDPFPUContext is to be zero here.
             * So, it is never necessary to restore the DPFPU context here. */
            "POP		R15									\n"\
            "MOV.L		#_ulPortTaskHasDPFPUContext, R14	\n"\
            "MOV.L		R15, [R14]							\n"\

        #elif ( configUSE_TASK_DPFPU_SUPPORT == 2 )

            /* Restore the DPFPU context. */
            "DPOPM.L	DPSW-DECNT				\n"\
            "DPOPM.D	DR0-DR15				\n"\

        #endif /* if ( configUSE_TASK_DPFPU_SUPPORT == 1 ) */

        "POP		R15						\n"\

        /* Accumulator low 32 bits. */
        "MVTACLO	R15, A0					\n"\
        "POP		R15						\n"\

        /* Accumulator high 32 bits. */
        "MVTACHI	R15, A0					\n"\
        "POP		R15						\n"\

        /* Accumulator guard. */
        "MVTACGU	R15, A0					\n"\
        "POP		R15						\n"\

        /* Accumulator low 32 bits. */
        "MVTACLO	R15, A1					\n"\
        "POP		R15						\n"\

        /* Accumulator high 32 bits. */
        "MVTACHI	R15, A1					\n"\
        "POP		R15						\n"\

        /* Accumulator guard. */
        "MVTACGU	R15, A1					\n"\
        "POP		R15						\n"\

        /* Floating point status word. */
        "MVTC		R15, FPSW 				\n"\

        /* R1 to R15 - R0 is not included as it is the SP. */
        "POPM		R1-R15 					\n"\

        /* This pops the remaining registers. */
        "RTE								\n"\
        "NOP								\n"\
        "NOP								\n"
    );
}
/*-----------------------------------------------------------*/

#pragma vector = VECT( ICU, SWINT )
__interrupt void vSoftwareInterruptISR( void )
{
    __asm volatile
    (
        /* Re-enable interrupts. */
        "SETPSW		I							\n"\


        /* Move the data that was automatically pushed onto the interrupt stack when
         * the interrupt occurred from the interrupt stack to the user stack.
         *
         * R15 is saved before it is clobbered. */
        "PUSH.L		R15							\n"\

        /* Read the user stack pointer. */
        "MVFC		USP, R15					\n"\

        /* Move the address down to the data being moved. */
        "SUB		#12, R15					\n"\
        "MVTC		R15, USP					\n"\

        /* Copy the data across, R15, then PC, then PSW. */
        "MOV.L		[ R0 ], [ R15 ]				\n"\
        "MOV.L 		4[ R0 ], 4[ R15 ]			\n"\
        "MOV.L		8[ R0 ], 8[ R15 ]			\n"\

        /* Move the interrupt stack pointer to its new correct position. */
        "ADD		#12, R0						\n"\

        /* All the rest of the registers are saved directly to the user stack. */
        "SETPSW		U							\n"\

        /* Save the rest of the general registers (R15 has been saved already). */
        "PUSHM		R1-R14						\n"\

        /* Save the FPSW and accumulators. */
        "MVFC		FPSW, R15					\n"\
        "PUSH.L		R15							\n"\
        "MVFACGU	#0, A1, R15					\n"\
        "PUSH.L		R15							\n"\
        "MVFACHI	#0, A1, R15					\n"\
        "PUSH.L		R15							\n"\
        "MVFACLO	#0, A1, R15					\n" /* Low order word. */ \
        "PUSH.L		R15							\n"\
        "MVFACGU	#0, A0, R15					\n"\
        "PUSH.L		R15							\n"\
        "MVFACHI	#0, A0, R15					\n"\
        "PUSH.L		R15							\n"\
        "MVFACLO	#0, A0, R15					\n" /* Low order word. */ \
        "PUSH.L		R15							\n"\

        #if ( configUSE_TASK_DPFPU_SUPPORT == 1 )

            /* Does the task have a DPFPU context that needs saving?  If
             * ulPortTaskHasDPFPUContext is 0 then no. */
            "MOV.L		#_ulPortTaskHasDPFPUContext, R15	\n"\
            "MOV.L		[R15], R15							\n"\
            "CMP		#0, R15								\n"\

            /* Save the DPFPU context, if any. */
            "BEQ.B		__lab1						\n"\
            "DPUSHM.D	DR0-DR15					\n"\
            "DPUSHM.L	DPSW-DECNT					\n"\
            "__lab1:								\n"\

            /* Save ulPortTaskHasDPFPUContext itself. */
            "PUSH.L		R15							\n"\

        #elif ( configUSE_TASK_DPFPU_SUPPORT == 2 )

            /* Save the DPFPU context, always. */
            "DPUSHM.D	DR0-DR15					\n"\
            "DPUSHM.L	DPSW-DECNT					\n"\

        #endif /* if ( configUSE_TASK_DPFPU_SUPPORT == 1 ) */


        /* Save the stack pointer to the TCB. */
        "MOV.L		#_pxCurrentTCB, R15			\n"\
        "MOV.L		[ R15 ], R15				\n"\
        "MOV.L		R0, [ R15 ]					\n"\


        /* Ensure the interrupt mask is set to the syscall priority while the kernel
         * structures are being accessed. */
        "MVTIPL		%0 							\n"\

        /* Select the next task to run. */
        "BSR.A		_vTaskSwitchContext			\n"\

        /* Reset the interrupt mask as no more data structure access is required. */
        "MVTIPL		%1							\n"\


        /* Load the stack pointer of the task that is now selected as the Running
         * state task from its TCB. */
        "MOV.L		#_pxCurrentTCB,R15			\n"\
        "MOV.L		[ R15 ], R15				\n"\
        "MOV.L		[ R15 ], R0					\n"\


        /* Restore the context of the new task.  The PSW (Program Status Word) and
         * PC will be popped by the RTE instruction. */

        #if ( configUSE_TASK_DPFPU_SUPPORT == 1 )

            /* Is there a DPFPU context to restore?  If the restored
             * ulPortTaskHasDPFPUContext is zero then no. */
            "POP		R15									\n"\
            "MOV.L		#_ulPortTaskHasDPFPUContext, R14	\n"\
            "MOV.L		R15, [R14]							\n"\
            "CMP		#0, R15								\n"\

            /* Restore the DPFPU context, if any. */
            "BEQ.B		__lab2						\n"\
            "DPOPM.L	DPSW-DECNT					\n"\
            "DPOPM.D	DR0-DR15					\n"\
            "__lab2:								\n"\

        #elif ( configUSE_TASK_DPFPU_SUPPORT == 2 )

            /* Restore the DPFPU context, always. */
            "DPOPM.L	DPSW-DECNT					\n"\
            "DPOPM.D	DR0-DR15					\n"\

        #endif /* if( configUSE_TASK_DPFPU_SUPPORT == 1 ) */

        "POP		R15							\n"\

        /* Accumulator low 32 bits. */
        "MVTACLO	R15, A0						\n"\
        "POP		R15							\n"\

        /* Accumulator high 32 bits. */
        "MVTACHI	R15, A0						\n"\
        "POP		R15							\n"\

        /* Accumulator guard. */
        "MVTACGU	R15, A0						\n"\
        "POP		R15							\n"\

        /* Accumulator low 32 bits. */
        "MVTACLO	R15, A1						\n"\
        "POP		R15							\n"\

        /* Accumulator high 32 bits. */
        "MVTACHI	R15, A1						\n"\
        "POP		R15							\n"\

        /* Accumulator guard. */
        "MVTACGU	R15, A1						\n"\
        "POP		R15							\n"\
        "MVTC		R15, FPSW					\n"\
        "POPM		R1-R15						\n"\
        "RTE									\n"\
        "NOP									\n"\
        "NOP									  "
        portCDT_NO_PARSE( :: ) "i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ), "i" ( configKERNEL_INTERRUPT_PRIORITY )
    );
}
/*-----------------------------------------------------------*/

#pragma vector = _VECT( configTICK_VECTOR )
__interrupt void vTickISR( void )
{
    /* Re-enable interrupts. */
    __enable_interrupt();

    /* Increment the tick, and perform any processing the new tick value
     * necessitates.  Ensure IPL is at the max syscall value first. */
    __set_interrupt_level( configMAX_SYSCALL_INTERRUPT_PRIORITY );
    {
        if( xTaskIncrementTick() != pdFALSE )
        {
            taskYIELD();
        }
    }
    __set_interrupt_level( configKERNEL_INTERRUPT_PRIORITY );
}
/*-----------------------------------------------------------*/
