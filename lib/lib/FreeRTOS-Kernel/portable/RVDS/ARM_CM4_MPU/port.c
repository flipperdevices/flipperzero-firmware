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
* Implementation of functions defined in portable.h for the ARM CM4 MPU port.
*----------------------------------------------------------*/

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef __TARGET_FPU_VFP
    #error This port can only be used when the project options are configured to enable hardware floating point support.
#endif

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#ifndef configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS
    #warning "configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS is not defined. We recommend defining it to 0 in FreeRTOSConfig.h for better security."
    #define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS    1
#endif

/* Constants required to access and manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL_REG                 ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG                 ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG        ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SHPR3_REG                        ( *( ( volatile uint32_t * ) 0xe000ed20 ) )
#define portNVIC_SHPR2_REG                        ( *( ( volatile uint32_t * ) 0xe000ed1c ) )
#define portNVIC_SYS_CTRL_STATE_REG               ( *( ( volatile uint32_t * ) 0xe000ed24 ) )
#define portNVIC_MEM_FAULT_ENABLE                 ( 1UL << 16UL )

/* Constants required to access and manipulate the MPU. */
#define portMPU_TYPE_REG                          ( *( ( volatile uint32_t * ) 0xe000ed90 ) )
#define portMPU_REGION_BASE_ADDRESS_REG           ( *( ( volatile uint32_t * ) 0xe000ed9C ) )
#define portMPU_REGION_ATTRIBUTE_REG              ( *( ( volatile uint32_t * ) 0xe000edA0 ) )
#define portMPU_CTRL_REG                          ( *( ( volatile uint32_t * ) 0xe000ed94 ) )
#define portEXPECTED_MPU_TYPE_VALUE               ( portTOTAL_NUM_REGIONS << 8UL )
#define portMPU_ENABLE                            ( 0x01UL )
#define portMPU_BACKGROUND_ENABLE                 ( 1UL << 2UL )
#define portPRIVILEGED_EXECUTION_START_ADDRESS    ( 0UL )
#define portMPU_REGION_VALID                      ( 0x10UL )
#define portMPU_REGION_ENABLE                     ( 0x01UL )
#define portPERIPHERALS_START_ADDRESS             0x40000000UL
#define portPERIPHERALS_END_ADDRESS               0x5FFFFFFFUL

/* Constants required to access and manipulate the SysTick. */
#define portNVIC_SYSTICK_CLK                      ( 0x00000004UL )
#define portNVIC_SYSTICK_INT                      ( 0x00000002UL )
#define portNVIC_SYSTICK_ENABLE                   ( 0x00000001UL )
#define portNVIC_PENDSV_PRI                       ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI                      ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )
#define portNVIC_SVC_PRI                          ( ( ( uint32_t ) configMAX_SYSCALL_INTERRUPT_PRIORITY - 1UL ) << 24UL )

/* Constants required to manipulate the VFP. */
#define portFPCCR                                 ( ( volatile uint32_t * ) 0xe000ef34UL ) /* Floating point context control register. */
#define portASPEN_AND_LSPEN_BITS                  ( 0x3UL << 30UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                          ( 0x01000000UL )
#define portINITIAL_EXC_RETURN                    ( 0xfffffffdUL )
#define portINITIAL_CONTROL_IF_UNPRIVILEGED       ( 0x03 )
#define portINITIAL_CONTROL_IF_PRIVILEGED         ( 0x02 )

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER           ( 16 )
#define portNVIC_IP_REGISTERS_OFFSET_16           ( 0xE000E3F0 )
#define portAIRCR_REG                             ( *( ( volatile uint32_t * ) 0xE000ED0C ) )
#define portMAX_8_BIT_VALUE                       ( ( uint8_t ) 0xff )
#define portTOP_BIT_OF_BYTE                       ( ( uint8_t ) 0x80 )
#define portMAX_PRIGROUP_BITS                     ( ( uint8_t ) 7 )
#define portPRIORITY_GROUP_MASK                   ( 0x07UL << 8UL )
#define portPRIGROUP_SHIFT                        ( 8UL )

/* Offsets in the stack to the parameters when inside the SVC handler. */
#define portOFFSET_TO_PC                          ( 6 )

/* For strict compliance with the Cortex-M spec the task start address should
 * have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK                    ( ( StackType_t ) 0xfffffffeUL )

/* Each task maintains its own interrupt status in the critical nesting
 * variable.  Note this is not saved as part of the task context as context
 * switches can only occur when uxCriticalNesting is zero. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Setup the timer to generate the tick interrupts.
 */
void vSetupTimerInterrupt( void ) PRIVILEGED_FUNCTION;

/*
 * Configure a number of standard MPU regions that are used by all tasks.
 */
static void prvSetupMPU( void ) PRIVILEGED_FUNCTION;

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvStartFirstTask( void ) PRIVILEGED_FUNCTION;

/*
 * Return the smallest MPU region size that a given number of bytes will fit
 * into.  The region size is returned as the value that should be programmed
 * into the region attribute register for that region.
 */
static uint32_t prvGetMPURegionSizeSetting( uint32_t ulActualSizeInBytes ) PRIVILEGED_FUNCTION;

/*
 * Standard FreeRTOS exception handlers.
 */
void xPortPendSVHandler( void ) PRIVILEGED_FUNCTION;
void xPortSysTickHandler( void ) PRIVILEGED_FUNCTION;
void vPortSVCHandler( void ) PRIVILEGED_FUNCTION;

/*
 * Starts the scheduler by restoring the context of the first task to run.
 */
static void prvRestoreContextOfFirstTask( void ) PRIVILEGED_FUNCTION;

/*
 * C portion of the SVC handler.  The SVC handler is split between an asm entry
 * and a C wrapper for simplicity of coding and maintenance.
 */
void prvSVCHandler( uint32_t * pulRegisters ) __attribute__( ( used ) ) PRIVILEGED_FUNCTION;

/*
 * Function to enable the VFP.
 */
static void vPortEnableVFP( void );

/*
 * Utility function.
 */
static uint32_t prvPortGetIPSR( void );

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if ( configASSERT_DEFINED == 1 )
    static uint8_t ucMaxSysCallPriority = 0;
    static uint32_t ulMaxPRIGROUPValue = 0;
    static const volatile uint8_t * const pcInterruptPriorityRegisters = ( const uint8_t * ) portNVIC_IP_REGISTERS_OFFSET_16;
#endif /* configASSERT_DEFINED */

/**
 * @brief Checks whether or not the processor is privileged.
 *
 * @return 1 if the processor is already privileged, 0 otherwise.
 */
BaseType_t xIsPrivileged( void );

/**
 * @brief Lowers the privilege level by setting the bit 0 of the CONTROL
 * register.
 *
 * Bit 0 of the CONTROL register defines the privilege level of Thread Mode.
 *  Bit[0] = 0 --> The processor is running privileged
 *  Bit[0] = 1 --> The processor is running unprivileged.
 */
void vResetPrivilege( void );

/**
 * @brief Enter critical section.
 */
#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    void vPortEnterCritical( void ) FREERTOS_SYSTEM_CALL;
#else
    void vPortEnterCritical( void ) PRIVILEGED_FUNCTION;
#endif

/**
 * @brief Exit from critical section.
 */
#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    void vPortExitCritical( void ) FREERTOS_SYSTEM_CALL;
#else
    void vPortExitCritical( void ) PRIVILEGED_FUNCTION;
#endif
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters,
                                     BaseType_t xRunPrivileged )
{
    /* Simulate the stack frame as it would be created by a context switch
     * interrupt. */
    pxTopOfStack--;                                                      /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
    *pxTopOfStack = portINITIAL_XPSR;                                    /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK; /* PC */
    pxTopOfStack--;
    *pxTopOfStack = 0;                                                   /* LR */
    pxTopOfStack -= 5;                                                   /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;                        /* R0 */

    /* A save method is being used that requires each task to maintain its
     * own exec return value. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_EXC_RETURN;

    pxTopOfStack -= 9; /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    if( xRunPrivileged == pdTRUE )
    {
        *pxTopOfStack = portINITIAL_CONTROL_IF_PRIVILEGED;
    }
    else
    {
        *pxTopOfStack = portINITIAL_CONTROL_IF_UNPRIVILEGED;
    }

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void prvSVCHandler( uint32_t * pulParam )
{
    uint8_t ucSVCNumber;
    uint32_t ulReg, ulPC;

    #if ( configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY == 1 )
        extern uint32_t __syscalls_flash_start__;
        extern uint32_t __syscalls_flash_end__;
    #endif /* #if( configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY == 1 ) */

    /* The stack contains: r0, r1, r2, r3, r12, LR, PC and xPSR. The first
     * argument (r0) is pulParam[ 0 ]. */
    ulPC = pulParam[ portOFFSET_TO_PC ];
    ucSVCNumber = ( ( uint8_t * ) ulPC )[ -2 ];

    switch( ucSVCNumber )
    {
        case portSVC_START_SCHEDULER:
            portNVIC_SHPR2_REG |= portNVIC_SVC_PRI;
            prvRestoreContextOfFirstTask();
            break;

        case portSVC_YIELD:
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;

            /* Barriers are normally not required
             * but do ensure the code is completely
             * within the specified behaviour for the
             * architecture. */
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            break;

            #if ( configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY == 1 )
                case portSVC_RAISE_PRIVILEGE: /* Only raise the privilege, if the
                                               * svc was raised from any of the
                                               * system calls. */

                    if( ( ulPC >= ( uint32_t ) __syscalls_flash_start__ ) &&
                        ( ulPC <= ( uint32_t ) __syscalls_flash_end__ ) )
                    {
                        __asm
                        {
/* *INDENT-OFF* */
                            mrs ulReg, control /* Obtain current control value. */
                            bic ulReg, # 1     /* Set privilege bit. */
                            msr control, ulReg /* Write back new control value. */
/* *INDENT-ON* */
                        }
                    }

                    break;
            #else /* if ( configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY == 1 ) */
                case portSVC_RAISE_PRIVILEGE:
                    __asm
                    {
/* *INDENT-OFF* */
                        mrs ulReg, control /* Obtain current control value. */
                        bic ulReg, # 1     /* Set privilege bit. */
                        msr control, ulReg /* Write back new control value. */
/* *INDENT-ON* */
                    }
                    break;
                    #endif /* #if( configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY == 1 ) */

                default: /* Unknown SVC call. */
                    break;
    }
}
/*-----------------------------------------------------------*/

__asm void vPortSVCHandler( void )
{
    extern prvSVCHandler

/* *INDENT-OFF* */
        PRESERVE8

    /* Assumes psp was in use. */
    #ifndef USE_PROCESS_STACK   /* Code should not be required if a main() is using the process stack. */
        tst lr, # 4
        ite eq
        mrseq r0, msp
        mrsne r0, psp
    #else
        mrs r0, psp
    #endif

    b prvSVCHandler
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

__asm void prvRestoreContextOfFirstTask( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    ldr r0, =0xE000ED08 /* Use the NVIC offset register to locate the stack. */
    ldr r0, [ r0 ]
    ldr r0, [ r0 ]
    msr msp, r0              /* Set the msp back to the start of the stack. */
    ldr r3, =pxCurrentTCB   /* Restore the context. */
    ldr r1, [ r3 ]
    ldr r0, [ r1 ]    /* The first item in the TCB is the task top of stack. */
    add r1, r1, #4          /* Move onto the second item in the TCB... */

    dmb               /* Complete outstanding transfers before disabling MPU. */
    ldr r2, =0xe000ed94     /* MPU_CTRL register. */
    ldr r3, [ r2 ] /* Read the value of MPU_CTRL. */
    bic r3, r3, # 1          /* r3 = r3 & ~1 i.e. Clear the bit 0 in r3. */
    str r3, [ r2 ]           /* Disable MPU. */

    ldr r2, =0xe000ed9c     /* Region Base Address register. */
    ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 4 - 7]. */
    stmia r2, { r4 - r11 }  /* Write 4 sets of MPU registers [MPU Region # 4 - 7]. */

    #if ( portTOTAL_NUM_REGIONS == 16 )
        ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 8 - 11]. */
        stmia r2, { r4 - r11 } /* Write 4 sets of MPU registers. [MPU Region # 8 - 11]. */
        ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 12 - 15]. */
        stmia r2, { r4 - r11 }  /* Write 4 sets of MPU registers. [MPU Region # 12 - 15]. */
    #endif /* portTOTAL_NUM_REGIONS == 16. */

    ldr r2, =0xe000ed94     /* MPU_CTRL register. */
    ldr r3, [ r2 ] /* Read the value of MPU_CTRL. */
    orr r3, r3, #1          /* r3 = r3 | 1 i.e. Set the bit 0 in r3. */
    str r3, [ r2 ]           /* Enable MPU. */
    dsb                      /* Force memory writes before continuing. */

    ldmia r0 !, { r3 - r11, r14 } /* Pop the registers that are not automatically saved on exception entry. */
    msr control, r3
    msr psp, r0 /* Restore the task stack pointer. */
    mov r0, #0
    msr basepri, r0
    bx r14
    nop
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
     * See https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
    configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY );

    #if ( configASSERT_DEFINED == 1 )
        {
            volatile uint32_t ulOriginalPriority;
            volatile uint8_t * const pucFirstUserPriorityRegister = ( volatile uint8_t * ) ( portNVIC_IP_REGISTERS_OFFSET_16 + portFIRST_USER_INTERRUPT_NUMBER );
            volatile uint8_t ucMaxPriorityValue;

            /* Determine the maximum priority from which ISR safe FreeRTOS API
             * functions can be called.  ISR safe functions are those that end in
             * "FromISR".  FreeRTOS maintains separate thread and ISR API functions to
             * ensure interrupt entry is as fast and simple as possible.
             *
             * Save the interrupt priority value that is about to be clobbered. */
            ulOriginalPriority = *pucFirstUserPriorityRegister;

            /* Determine the number of priority bits available.  First write to all
             * possible bits. */
            *pucFirstUserPriorityRegister = portMAX_8_BIT_VALUE;

            /* Read the value back to see how many bits stuck. */
            ucMaxPriorityValue = *pucFirstUserPriorityRegister;

            /* Use the same mask on the maximum system call priority. */
            ucMaxSysCallPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY & ucMaxPriorityValue;

            /* Calculate the maximum acceptable priority group value for the number
             * of bits read back. */
            ulMaxPRIGROUPValue = portMAX_PRIGROUP_BITS;

            while( ( ucMaxPriorityValue & portTOP_BIT_OF_BYTE ) == portTOP_BIT_OF_BYTE )
            {
                ulMaxPRIGROUPValue--;
                ucMaxPriorityValue <<= ( uint8_t ) 0x01;
            }

            #ifdef __NVIC_PRIO_BITS
                {
                    /* Check the CMSIS configuration that defines the number of
                     * priority bits matches the number of priority bits actually queried
                     * from the hardware. */
                    configASSERT( ( portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue ) == __NVIC_PRIO_BITS );
                }
            #endif

            #ifdef configPRIO_BITS
                {
                    /* Check the FreeRTOS configuration that defines the number of
                     * priority bits matches the number of priority bits actually queried
                     * from the hardware. */
                    configASSERT( ( portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue ) == configPRIO_BITS );
                }
            #endif

            /* Shift the priority group value back to its position within the AIRCR
             * register. */
            ulMaxPRIGROUPValue <<= portPRIGROUP_SHIFT;
            ulMaxPRIGROUPValue &= portPRIORITY_GROUP_MASK;

            /* Restore the clobbered interrupt priority register to its original
             * value. */
            *pucFirstUserPriorityRegister = ulOriginalPriority;
        }
    #endif /* configASSERT_DEFINED */

    /* Make PendSV and SysTick the same priority as the kernel, and the SVC
     * handler higher priority so it can be used to exit a critical section (where
     * lower priorities are masked). */
    portNVIC_SHPR3_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SHPR3_REG |= portNVIC_SYSTICK_PRI;

    /* Configure the regions in the MPU that are common to all tasks. */
    prvSetupMPU();

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
     * here already. */
    vSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Ensure the VFP is enabled - it should be anyway. */
    vPortEnableVFP();

    /* Lazy save always. */
    *( portFPCCR ) |= portASPEN_AND_LSPEN_BITS;

    /* Start the first task. */
    prvStartFirstTask();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

__asm void prvStartFirstTask( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    /* Use the NVIC offset register to locate the stack. */
    ldr r0, =0xE000ED08
    ldr r0, [ r0 ]
    ldr r0, [ r0 ]
    /* Set the msp back to the start of the stack. */
    msr msp, r0

    /* Clear the bit that indicates the FPU is in use in case the FPU was used
     * before the scheduler was started - which would otherwise result in the
     * unnecessary leaving of space in the SVC stack for lazy saving of FPU
     * registers. */
    mov r0, #0
    msr control, r0
    /* Globally enable interrupts. */
    cpsie i
    cpsie f
    dsb
    isb
    svc portSVC_START_SCHEDULER /* System call to start first task. */
    nop
    nop
/* *INDENT-ON* */
}

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    BaseType_t xRunningPrivileged;
    xPortRaisePrivilege( xRunningPrivileged );
#endif

    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;

#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    vPortResetPrivilege( xRunningPrivileged );
#endif
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    BaseType_t xRunningPrivileged;
    xPortRaisePrivilege( xRunningPrivileged );
#endif

    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;

    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }

#if( configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS == 1 )
    vPortResetPrivilege( xRunningPrivileged );
#endif
}
/*-----------------------------------------------------------*/

__asm void xPortPendSVHandler( void )
{
    extern uxCriticalNesting;
    extern pxCurrentTCB;
    extern vTaskSwitchContext;

/* *INDENT-OFF* */
    PRESERVE8

    mrs r0, psp

    ldr r3, =pxCurrentTCB /* Get the location of the current TCB. */
    ldr r2, [ r3 ]

    tst r14, #0x10 /* Is the task using the FPU context?  If so, push high vfp registers. */
    it eq
    vstmdbeq r0 !, { s16 - s31 }

    mrs r1, control
    stmdb r0 !, { r1, r4 - r11, r14 }   /* Save the remaining registers. */
    str r0, [ r2 ] /* Save the new top of stack into the first member of the TCB. */

    stmdb sp !, { r0, r3 }
    mov r0, # configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r0
    dsb
    isb
    bl vTaskSwitchContext
    mov r0, #0
    msr basepri, r0
    ldmia sp !, { r0, r3 }
    /* Restore the context. */
    ldr r1, [ r3 ]
    ldr r0, [ r1 ]           /* The first item in the TCB is the task top of stack. */
    add r1, r1, #4          /* Move onto the second item in the TCB... */

    dmb                      /* Complete outstanding transfers before disabling MPU. */
    ldr r2, =0xe000ed94     /* MPU_CTRL register. */
    ldr r3, [ r2 ] /* Read the value of MPU_CTRL. */
    bic r3, r3, #1          /* r3 = r3 & ~1 i.e. Clear the bit 0 in r3. */
    str r3, [ r2 ]           /* Disable MPU. */

    ldr r2, =0xe000ed9c     /* Region Base Address register. */
    ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 4 - 7]. */
    stmia r2, { r4 - r11 }   /* Write 4 sets of MPU registers [MPU Region # 4 - 7]. */

    #if ( portTOTAL_NUM_REGIONS == 16 )
        ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 8 - 11]. */
        stmia r2, { r4 - r11 }  /* Write 4 sets of MPU registers. [MPU Region # 8 - 11]. */
        ldmia r1 !, { r4 - r11 } /* Read 4 sets of MPU registers [MPU Region # 12 - 15]. */
        stmia r2, { r4 - r11 } /* Write 4 sets of MPU registers. [MPU Region # 12 - 15]. */
    #endif /* portTOTAL_NUM_REGIONS == 16. */

    ldr r2, =0xe000ed94     /* MPU_CTRL register. */
    ldr r3, [ r2 ] /* Read the value of MPU_CTRL. */
    orr r3, r3, #1          /* r3 = r3 | 1 i.e. Set the bit 0 in r3. */
    str r3, [ r2 ]           /* Enable MPU. */
    dsb                      /* Force memory writes before continuing. */

    ldmia r0 !, { r3 - r11, r14 }                               /* Pop the registers that are not automatically saved on exception entry. */
    msr control, r3

    tst r14, #0x10 /* Is the task using the FPU context?  If so, pop the high vfp registers too. */
    it eq
    vldmiaeq r0 !, { s16 - s31 }

    msr psp, r0
    bx r14
    nop
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    uint32_t ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* Pend a context switch. */
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__weak void vSetupTimerInterrupt( void )
{
    /* Reset the SysTick. */
    portNVIC_SYSTICK_CTRL_REG = 0UL;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

    /* Configure SysTick to interrupt at the requested rate. */
    portNVIC_SYSTICK_LOAD_REG = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
/*-----------------------------------------------------------*/

__asm void vPortSwitchToUserMode( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    mrs r0, control
    orr r0, #1
    msr control, r0
    bx r14
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

__asm void vPortEnableVFP( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    ldr.w r0, =0xE000ED88 /* The FPU enable bits are in the CPACR. */
    ldr r1, [ r0 ]

    orr r1, r1, #( 0xf << 20 ) /* Enable CP10 and CP11 coprocessors, then save back. */
    str r1, [ r0 ]
    bx r14
    nop
    nop
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

static void prvSetupMPU( void )
{
    extern uint32_t __privileged_functions_start__;
    extern uint32_t __privileged_functions_end__;
    extern uint32_t __FLASH_segment_start__;
    extern uint32_t __FLASH_segment_end__;
    extern uint32_t __privileged_data_start__;
    extern uint32_t __privileged_data_end__;

    /* The only permitted number of regions are 8 or 16. */
    configASSERT( ( portTOTAL_NUM_REGIONS == 8 ) || ( portTOTAL_NUM_REGIONS == 16 ) );

    /* Ensure that the configTOTAL_MPU_REGIONS is configured correctly. */
    configASSERT( portMPU_TYPE_REG == portEXPECTED_MPU_TYPE_VALUE );

    /* Check the expected MPU is present. */
    if( portMPU_TYPE_REG == portEXPECTED_MPU_TYPE_VALUE )
    {
        /* First setup the unprivileged flash for unprivileged read only access. */
        portMPU_REGION_BASE_ADDRESS_REG = ( ( uint32_t ) __FLASH_segment_start__ ) | /* Base address. */
                                          ( portMPU_REGION_VALID ) |
                                          ( portUNPRIVILEGED_FLASH_REGION );

        portMPU_REGION_ATTRIBUTE_REG = ( portMPU_REGION_READ_ONLY ) |
                                       ( ( configTEX_S_C_B_FLASH & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
                                       ( prvGetMPURegionSizeSetting( ( uint32_t ) __FLASH_segment_end__ - ( uint32_t ) __FLASH_segment_start__ ) ) |
                                       ( portMPU_REGION_ENABLE );

        /* Setup the privileged flash for privileged only access.  This is where
         * the kernel code is placed. */
        portMPU_REGION_BASE_ADDRESS_REG = ( ( uint32_t ) __privileged_functions_start__ ) | /* Base address. */
                                          ( portMPU_REGION_VALID ) |
                                          ( portPRIVILEGED_FLASH_REGION );

        portMPU_REGION_ATTRIBUTE_REG = ( portMPU_REGION_PRIVILEGED_READ_ONLY ) |
                                       ( ( configTEX_S_C_B_FLASH & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
                                       ( prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_functions_end__ - ( uint32_t ) __privileged_functions_start__ ) ) |
                                       ( portMPU_REGION_ENABLE );

        /* Setup the privileged data RAM region.  This is where the kernel data
         * is placed. */
        portMPU_REGION_BASE_ADDRESS_REG = ( ( uint32_t ) __privileged_data_start__ ) | /* Base address. */
                                          ( portMPU_REGION_VALID ) |
                                          ( portPRIVILEGED_RAM_REGION );

        portMPU_REGION_ATTRIBUTE_REG = ( portMPU_REGION_PRIVILEGED_READ_WRITE ) |
                                       ( ( configTEX_S_C_B_SRAM & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
                                       prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_data_end__ - ( uint32_t ) __privileged_data_start__ ) |
                                       ( portMPU_REGION_ENABLE );

        /* By default allow everything to access the general peripherals.  The
         * system peripherals and registers are protected. */
        portMPU_REGION_BASE_ADDRESS_REG = ( portPERIPHERALS_START_ADDRESS ) |
                                          ( portMPU_REGION_VALID ) |
                                          ( portGENERAL_PERIPHERALS_REGION );

        portMPU_REGION_ATTRIBUTE_REG = ( portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER ) |
                                       ( prvGetMPURegionSizeSetting( portPERIPHERALS_END_ADDRESS - portPERIPHERALS_START_ADDRESS ) ) |
                                       ( portMPU_REGION_ENABLE );

        /* Enable the memory fault exception. */
        portNVIC_SYS_CTRL_STATE_REG |= portNVIC_MEM_FAULT_ENABLE;

        /* Enable the MPU with the background region configured. */
        portMPU_CTRL_REG |= ( portMPU_ENABLE | portMPU_BACKGROUND_ENABLE );
    }
}
/*-----------------------------------------------------------*/

static uint32_t prvGetMPURegionSizeSetting( uint32_t ulActualSizeInBytes )
{
    uint32_t ulRegionSize, ulReturnValue = 4;

    /* 32 is the smallest region size, 31 is the largest valid value for
     * ulReturnValue. */
    for( ulRegionSize = 32UL; ulReturnValue < 31UL; ( ulRegionSize <<= 1UL ) )
    {
        if( ulActualSizeInBytes <= ulRegionSize )
        {
            break;
        }
        else
        {
            ulReturnValue++;
        }
    }

    /* Shift the code by one before returning so it can be written directly
     * into the the correct bit position of the attribute register. */
    return( ulReturnValue << 1UL );
}
/*-----------------------------------------------------------*/

__asm BaseType_t xIsPrivileged( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    mrs r0, control /* r0 = CONTROL. */
    tst r0, #1     /* Perform r0 & 1 (bitwise AND) and update the conditions flag. */
    ite ne
    movne r0, #0   /* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
    moveq r0, #1   /* CONTROL[0]==0. Return true to indicate that the processor is privileged. */
    bx lr           /* Return. */
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

__asm void vResetPrivilege( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    mrs r0, control /* r0 = CONTROL. */
    orrs r0, #1    /* r0 = r0 | 1. */
    msr control, r0 /* CONTROL = r0. */
    bx lr           /* Return. */
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

void vPortStoreTaskMPUSettings( xMPU_SETTINGS * xMPUSettings,
                                const struct xMEMORY_REGION * const xRegions,
                                StackType_t * pxBottomOfStack,
                                uint32_t ulStackDepth )
{
    extern uint32_t __SRAM_segment_start__;
    extern uint32_t __SRAM_segment_end__;
    extern uint32_t __privileged_data_start__;
    extern uint32_t __privileged_data_end__;


    int32_t lIndex;
    uint32_t ul;

    if( xRegions == NULL )
    {
        /* No MPU regions are specified so allow access to all RAM. */
        xMPUSettings->xRegion[ 0 ].ulRegionBaseAddress =
            ( ( uint32_t ) __SRAM_segment_start__ ) | /* Base address. */
            ( portMPU_REGION_VALID ) |
            ( portSTACK_REGION );

        xMPUSettings->xRegion[ 0 ].ulRegionAttribute =
            ( portMPU_REGION_READ_WRITE ) |
            ( ( configTEX_S_C_B_SRAM & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
            ( prvGetMPURegionSizeSetting( ( uint32_t ) __SRAM_segment_end__ - ( uint32_t ) __SRAM_segment_start__ ) ) |
            ( portMPU_REGION_ENABLE );

        /* Re-instate the privileged only RAM region as xRegion[ 0 ] will have
         * just removed the privileged only parameters. */
        xMPUSettings->xRegion[ 1 ].ulRegionBaseAddress =
            ( ( uint32_t ) __privileged_data_start__ ) | /* Base address. */
            ( portMPU_REGION_VALID ) |
            ( portSTACK_REGION + 1 );

        xMPUSettings->xRegion[ 1 ].ulRegionAttribute =
            ( portMPU_REGION_PRIVILEGED_READ_WRITE ) |
            ( ( configTEX_S_C_B_SRAM & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
            prvGetMPURegionSizeSetting( ( uint32_t ) __privileged_data_end__ - ( uint32_t ) __privileged_data_start__ ) |
            ( portMPU_REGION_ENABLE );

        /* Invalidate all other regions. */
        for( ul = 2; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
        {
            xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = ( portSTACK_REGION + ul ) | portMPU_REGION_VALID;
            xMPUSettings->xRegion[ ul ].ulRegionAttribute = 0UL;
        }
    }
    else
    {
        /* This function is called automatically when the task is created - in
         * which case the stack region parameters will be valid.  At all other
         * times the stack parameters will not be valid and it is assumed that the
         * stack region has already been configured. */
        if( ulStackDepth > 0 )
        {
            /* Define the region that allows access to the stack. */
            xMPUSettings->xRegion[ 0 ].ulRegionBaseAddress =
                ( ( uint32_t ) pxBottomOfStack ) |
                ( portMPU_REGION_VALID ) |
                ( portSTACK_REGION ); /* Region number. */

            xMPUSettings->xRegion[ 0 ].ulRegionAttribute =
                ( portMPU_REGION_READ_WRITE ) | /* Read and write. */
                ( prvGetMPURegionSizeSetting( ulStackDepth * ( uint32_t ) sizeof( StackType_t ) ) ) |
                ( ( configTEX_S_C_B_SRAM & portMPU_RASR_TEX_S_C_B_MASK ) << portMPU_RASR_TEX_S_C_B_LOCATION ) |
                ( portMPU_REGION_ENABLE );
        }

        lIndex = 0;

        for( ul = 1; ul <= portNUM_CONFIGURABLE_REGIONS; ul++ )
        {
            if( ( xRegions[ lIndex ] ).ulLengthInBytes > 0UL )
            {
                /* Translate the generic region definition contained in
                 * xRegions into the CM4 specific MPU settings that are then
                 * stored in xMPUSettings. */
                xMPUSettings->xRegion[ ul ].ulRegionBaseAddress =
                    ( ( uint32_t ) xRegions[ lIndex ].pvBaseAddress ) |
                    ( portMPU_REGION_VALID ) |
                    ( portSTACK_REGION + ul ); /* Region number. */

                xMPUSettings->xRegion[ ul ].ulRegionAttribute =
                    ( prvGetMPURegionSizeSetting( xRegions[ lIndex ].ulLengthInBytes ) ) |
                    ( xRegions[ lIndex ].ulParameters ) |
                    ( portMPU_REGION_ENABLE );
            }
            else
            {
                /* Invalidate the region. */
                xMPUSettings->xRegion[ ul ].ulRegionBaseAddress = ( portSTACK_REGION + ul ) | portMPU_REGION_VALID;
                xMPUSettings->xRegion[ ul ].ulRegionAttribute = 0UL;
            }

            lIndex++;
        }
    }
}
/*-----------------------------------------------------------*/

__asm uint32_t prvPortGetIPSR( void )
{
/* *INDENT-OFF* */
    PRESERVE8

    mrs r0, ipsr
    bx r14
/* *INDENT-ON* */
}
/*-----------------------------------------------------------*/

#if ( configASSERT_DEFINED == 1 )

    void vPortValidateInterruptPriority( void )
    {
        uint32_t ulCurrentInterrupt;
        uint8_t ucCurrentPriority;

        /* Obtain the number of the currently executing interrupt. */
        ulCurrentInterrupt = prvPortGetIPSR();

        /* Is the interrupt number a user defined interrupt? */
        if( ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER )
        {
            /* Look up the interrupt's priority. */
            ucCurrentPriority = pcInterruptPriorityRegisters[ ulCurrentInterrupt ];

            /* The following assertion will fail if a service routine (ISR) for
             * an interrupt that has been assigned a priority above
             * configMAX_SYSCALL_INTERRUPT_PRIORITY calls an ISR safe FreeRTOS API
             * function.  ISR safe FreeRTOS API functions must *only* be called
             * from interrupts that have been assigned a priority at or below
             * configMAX_SYSCALL_INTERRUPT_PRIORITY.
             *
             * Numerically low interrupt priority numbers represent logically high
             * interrupt priorities, therefore the priority of the interrupt must
             * be set to a value equal to or numerically *higher* than
             * configMAX_SYSCALL_INTERRUPT_PRIORITY.
             *
             * Interrupts that use the FreeRTOS API must not be left at their
             * default priority of zero as that is the highest possible priority,
             * which is guaranteed to be above configMAX_SYSCALL_INTERRUPT_PRIORITY,
             * and therefore also guaranteed to be invalid.
             *
             * FreeRTOS maintains separate thread and ISR API functions to ensure
             * interrupt entry is as fast and simple as possible.
             *
             * The following links provide detailed information:
             * https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html
             * https://www.FreeRTOS.org/FAQHelp.html */
            configASSERT( ucCurrentPriority >= ucMaxSysCallPriority );
        }

        /* Priority grouping:  The interrupt controller (NVIC) allows the bits
         * that define each interrupt's priority to be split between bits that
         * define the interrupt's pre-emption priority bits and bits that define
         * the interrupt's sub-priority.  For simplicity all bits must be defined
         * to be pre-emption priority bits.  The following assertion will fail if
         * this is not the case (if some bits represent a sub-priority).
         *
         * If the application only uses CMSIS libraries for interrupt
         * configuration then the correct setting can be achieved on all Cortex-M
         * devices by calling NVIC_SetPriorityGrouping( 0 ); before starting the
         * scheduler.  Note however that some vendor specific peripheral libraries
         * assume a non-zero priority group setting, in which cases using a value
         * of zero will result in unpredictable behaviour. */
        configASSERT( ( portAIRCR_REG & portPRIORITY_GROUP_MASK ) <= ulMaxPRIGROUPValue );
    }

#endif /* configASSERT_DEFINED */
