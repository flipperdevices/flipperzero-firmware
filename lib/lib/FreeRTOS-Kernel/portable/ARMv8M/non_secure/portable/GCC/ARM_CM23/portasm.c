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
#include <stdint.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE ensures that PRIVILEGED_FUNCTION
 * is defined correctly and privileged functions are placed in correct sections. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Portasm includes. */
#include "portasm.h"

/* MPU_WRAPPERS_INCLUDED_FROM_API_FILE is needed to be defined only for the
 * header files. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if ( configENABLE_FPU == 1 )
    #error Cortex-M23 does not have a Floating Point Unit (FPU) and therefore configENABLE_FPU must be set to 0.
#endif

void vRestoreContextOfFirstTask( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	.syntax unified									\n"
        "													\n"
        "	ldr  r2, pxCurrentTCBConst2						\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        "	ldr  r3, [r2]									\n"/* Read pxCurrentTCB. */
        "	ldr  r0, [r3]									\n"/* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */
        "													\n"
        #if ( configENABLE_MPU == 1 )
            "	dmb											\n"/* Complete outstanding transfers before disabling MPU. */
            "	ldr r2, xMPUCTRLConst2						\n"/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
            "	ldr r4, [r2]								\n"/* Read the value of MPU_CTRL. */
            "	movs r5, #1									\n"/* r5 = 1. */
            "	bics r4, r5									\n"/* r4 = r4 & ~r5 i.e. Clear the bit 0 in r4. */
            "	str r4, [r2]								\n"/* Disable MPU. */
            "												\n"
            "	adds r3, #4									\n"/* r3 = r3 + 4. r3 now points to MAIR0 in TCB. */
            "	ldr  r4, [r3]								\n"/* r4 = *r3 i.e. r4 = MAIR0. */
            "	ldr  r2, xMAIR0Const2						\n"/* r2 = 0xe000edc0 [Location of MAIR0]. */
            "	str  r4, [r2]								\n"/* Program MAIR0. */
            "	ldr  r2, xRNRConst2							\n"/* r2 = 0xe000ed98 [Location of RNR]. */
            "	adds r3, #4									\n"/* r3 = r3 + 4. r3 now points to first RBAR in TCB. */
            "	movs r5, #4									\n"/* r5 = 4. */
            "	str  r5, [r2]								\n"/* Program RNR = 4. */
            "	ldmia r3!, {r6,r7}							\n"/* Read first set of RBAR/RLAR from TCB. */
            "	ldr  r4, xRBARConst2						\n"/* r4 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r4!, {r6,r7}							\n"/* Write first set of RBAR/RLAR registers. */
            "	movs r5, #5									\n"/* r5 = 5. */
            "	str  r5, [r2]								\n"/* Program RNR = 5. */
            "	ldmia r3!, {r6,r7}							\n"/* Read second set of RBAR/RLAR from TCB. */
            "	ldr  r4, xRBARConst2						\n"/* r4 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r4!, {r6,r7}							\n"/* Write second set of RBAR/RLAR registers. */
            "	movs r5, #6									\n"/* r5 = 6. */
            "	str  r5, [r2]								\n"/* Program RNR = 6. */
            "	ldmia r3!, {r6,r7}							\n"/* Read third set of RBAR/RLAR from TCB. */
            "	ldr  r4, xRBARConst2						\n"/* r4 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r4!, {r6,r7}							\n"/* Write third set of RBAR/RLAR registers. */
            "	movs r5, #7									\n"/* r5 = 7. */
            "	str  r5, [r2]								\n"/* Program RNR = 7. */
            "	ldmia r3!, {r6,r7}							\n"/* Read fourth set of RBAR/RLAR from TCB. */
            "	ldr  r4, xRBARConst2						\n"/* r4 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r4!, {r6,r7}							\n"/* Write fourth set of RBAR/RLAR registers. */
            "												\n"
            "	ldr r2, xMPUCTRLConst2						\n"/* r2 = 0xe000ed94 [Location of MPU_CTRL]. */
            "	ldr r4, [r2]								\n"/* Read the value of MPU_CTRL. */
            "	movs r5, #1									\n"/* r5 = 1. */
            "	orrs r4, r5									\n"/* r4 = r4 | r5 i.e. Set the bit 0 in r4. */
            "	str r4, [r2]								\n"/* Enable MPU. */
            "	dsb											\n"/* Force memory writes before continuing. */
        #endif /* configENABLE_MPU */
        "													\n"
        #if ( configENABLE_MPU == 1 )
            "	ldm  r0!, {r1-r4}							\n"/* Read from stack - r1 = xSecureContext, r2 = PSPLIM, r3 = CONTROL and r4 = EXC_RETURN. */
            "	ldr  r5, xSecureContextConst2				\n"
            "	str  r1, [r5]								\n"/* Set xSecureContext to this task's value for the same. */
            "	msr  psplim, r2								\n"/* Set this task's PSPLIM value. */
            "	msr  control, r3							\n"/* Set this task's CONTROL value. */
            "	adds r0, #32								\n"/* Discard everything up to r0. */
            "	msr  psp, r0								\n"/* This is now the new top of stack to use in the task. */
            "	isb											\n"
            "	bx   r4										\n"/* Finally, branch to EXC_RETURN. */
        #else /* configENABLE_MPU */
            "	ldm  r0!, {r1-r3}							\n"/* Read from stack - r1 = xSecureContext, r2 = PSPLIM and r3 = EXC_RETURN. */
            "	ldr  r4, xSecureContextConst2				\n"
            "	str  r1, [r4]								\n"/* Set xSecureContext to this task's value for the same. */
            "	msr  psplim, r2								\n"/* Set this task's PSPLIM value. */
            "	movs r1, #2									\n"/* r1 = 2. */
            "	msr  CONTROL, r1							\n"/* Switch to use PSP in the thread mode. */
            "	adds r0, #32								\n"/* Discard everything up to r0. */
            "	msr  psp, r0								\n"/* This is now the new top of stack to use in the task. */
            "	isb											\n"
            "	bx   r3										\n"/* Finally, branch to EXC_RETURN. */
        #endif /* configENABLE_MPU */
        "													\n"
        "	.align 4										\n"
        "pxCurrentTCBConst2: .word pxCurrentTCB				\n"
        "xSecureContextConst2: .word xSecureContext			\n"
        #if ( configENABLE_MPU == 1 )
            "xMPUCTRLConst2: .word 0xe000ed94				\n"
            "xMAIR0Const2: .word 0xe000edc0					\n"
            "xRNRConst2: .word 0xe000ed98					\n"
            "xRBARConst2: .word 0xe000ed9c					\n"
        #endif /* configENABLE_MPU */
    );
}
/*-----------------------------------------------------------*/

BaseType_t xIsPrivileged( void ) /* __attribute__ (( naked )) */
{
    __asm volatile
    (
        "	mrs r0, control									\n"/* r0 = CONTROL. */
        "	movs r1, #1										\n"/* r1 = 1. */
        "	tst r0, r1										\n"/* Perform r0 & r1 (bitwise AND) and update the conditions flag. */
        "	beq running_privileged							\n"/* If the result of previous AND operation was 0, branch. */
        "	movs r0, #0										\n"/* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
        "	bx lr											\n"/* Return. */
        " running_privileged:								\n"
        "	movs r0, #1										\n"/* CONTROL[0]==0. Return true to indicate that the processor is privileged. */
        "	bx lr											\n"/* Return. */
        "													\n"
        "	.align 4										\n"
        ::: "r0", "r1", "memory"
    );
}
/*-----------------------------------------------------------*/

void vRaisePrivilege( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	mrs r0, control									\n"/* Read the CONTROL register. */
        "	movs r1, #1										\n"/* r1 = 1. */
        "	bics r0, r1										\n"/* Clear the bit 0. */
        "	msr control, r0									\n"/* Write back the new CONTROL value. */
        "	bx lr											\n"/* Return to the caller. */
        ::: "r0", "r1", "memory"
    );
}
/*-----------------------------------------------------------*/

void vResetPrivilege( void ) /* __attribute__ (( naked )) */
{
    __asm volatile
    (
        "	mrs r0, control									\n"/* r0 = CONTROL. */
        "	movs r1, #1										\n"/* r1 = 1. */
        "	orrs r0, r1										\n"/* r0 = r0 | r1. */
        "	msr control, r0									\n"/* CONTROL = r0. */
        "	bx lr											\n"/* Return to the caller. */
        ::: "r0", "r1", "memory"
    );
}
/*-----------------------------------------------------------*/

void vStartFirstTask( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	ldr r0, xVTORConst								\n"/* Use the NVIC offset register to locate the stack. */
        "	ldr r0, [r0]									\n"/* Read the VTOR register which gives the address of vector table. */
        "	ldr r0, [r0]									\n"/* The first entry in vector table is stack pointer. */
        "	msr msp, r0										\n"/* Set the MSP back to the start of the stack. */
        "	cpsie i											\n"/* Globally enable interrupts. */
        "	dsb												\n"
        "	isb												\n"
        "	svc %0											\n"/* System call to start the first task. */
        "	nop												\n"
        "													\n"
        "   .align 4										\n"
        "xVTORConst: .word 0xe000ed08						\n"
        ::"i" ( portSVC_START_SCHEDULER ) : "memory"
    );
}
/*-----------------------------------------------------------*/

uint32_t ulSetInterruptMask( void ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	mrs r0, PRIMASK									\n"
        "	cpsid i											\n"
        "	bx lr											\n"
        ::: "memory"
    );
}
/*-----------------------------------------------------------*/

void vClearInterruptMask( __attribute__( ( unused ) ) uint32_t ulMask ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	msr PRIMASK, r0									\n"
        "	bx lr											\n"
        ::: "memory"
    );
}
/*-----------------------------------------------------------*/

void PendSV_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	.syntax unified									\n"
        "	.extern SecureContext_SaveContext				\n"
        "	.extern SecureContext_LoadContext				\n"
        "													\n"
        "	ldr r3, xSecureContextConst						\n"/* Read the location of xSecureContext i.e. &( xSecureContext ). */
        "	ldr r0, [r3]									\n"/* Read xSecureContext - Value of xSecureContext must be in r0 as it is used as a parameter later. */
        "	ldr r3, pxCurrentTCBConst						\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        "	ldr r1, [r3]									\n"/* Read pxCurrentTCB - Value of pxCurrentTCB must be in r1 as it is used as a parameter later.*/
        "	mrs r2, psp										\n"/* Read PSP in r2. */
        "													\n"
        "	cbz r0, save_ns_context							\n"/* No secure context to save. */
        "	push {r0-r2, r14}								\n"
        "	bl SecureContext_SaveContext					\n"/* Params are in r0 and r1. r0 = xSecureContext and r1 = pxCurrentTCB. */
        "	pop {r0-r3}										\n"/* LR is now in r3. */
        "	mov lr, r3										\n"/* LR = r3. */
        "	lsls r1, r3, #25								\n"/* r1 = r3 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
        "	bpl save_ns_context								\n"/* bpl - branch if positive or zero. If r1 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
        "	ldr r3, pxCurrentTCBConst						\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        "	ldr r1, [r3]									\n"/* Read pxCurrentTCB. */
        #if ( configENABLE_MPU == 1 )
            "	subs r2, r2, #16							\n"/* Make space for xSecureContext, PSPLIM, CONTROL and LR on the stack. */
            "	str r2, [r1]								\n"/* Save the new top of stack in TCB. */
            "	mrs r1, psplim								\n"/* r1 = PSPLIM. */
            "	mrs r3, control								\n"/* r3 = CONTROL. */
            "	mov r4, lr									\n"/* r4 = LR/EXC_RETURN. */
            "	stmia r2!, {r0, r1, r3, r4}					\n"/* Store xSecureContext, PSPLIM, CONTROL and LR on the stack. */
        #else /* configENABLE_MPU */
            "	subs r2, r2, #12							\n"/* Make space for xSecureContext, PSPLIM and LR on the stack. */
            "	str r2, [r1]								\n"/* Save the new top of stack in TCB. */
            "	mrs r1, psplim								\n"/* r1 = PSPLIM. */
            "	mov r3, lr									\n"/* r3 = LR/EXC_RETURN. */
            "	stmia r2!, {r0, r1, r3}						\n"/* Store xSecureContext, PSPLIM and LR on the stack. */
        #endif /* configENABLE_MPU */
        "	b select_next_task								\n"
        "													\n"
        " save_ns_context:									\n"
        "	ldr r3, pxCurrentTCBConst						\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        "	ldr r1, [r3]									\n"/* Read pxCurrentTCB. */
        #if ( configENABLE_MPU == 1 )
            "	subs r2, r2, #48							\n"/* Make space for xSecureContext, PSPLIM, CONTROL, LR and the remaining registers on the stack. */
            "	str r2, [r1]								\n"/* Save the new top of stack in TCB. */
            "	adds r2, r2, #16							\n"/* r2 = r2 + 16. */
            "	stmia r2!, {r4-r7}							\n"/* Store the low registers that are not saved automatically. */
            "	mov r4, r8									\n"/* r4 = r8. */
            "	mov r5, r9									\n"/* r5 = r9. */
            "	mov r6, r10									\n"/* r6 = r10. */
            "	mov r7, r11									\n"/* r7 = r11. */
            "	stmia r2!, {r4-r7}							\n"/* Store the high registers that are not saved automatically. */
            "	mrs r1, psplim								\n"/* r1 = PSPLIM. */
            "	mrs r3, control								\n"/* r3 = CONTROL. */
            "	mov r4, lr									\n"/* r4 = LR/EXC_RETURN. */
            "	subs r2, r2, #48							\n"/* r2 = r2 - 48. */
            "	stmia r2!, {r0, r1, r3, r4}					\n"/* Store xSecureContext, PSPLIM, CONTROL and LR on the stack. */
        #else /* configENABLE_MPU */
            "	subs r2, r2, #44							\n"/* Make space for xSecureContext, PSPLIM, LR and the remaining registers on the stack. */
            "	str r2, [r1]								\n"/* Save the new top of stack in TCB. */
            "	mrs r1, psplim								\n"/* r1 = PSPLIM. */
            "	mov r3, lr									\n"/* r3 = LR/EXC_RETURN. */
            "	stmia r2!, {r0, r1, r3-r7}					\n"/* Store xSecureContext, PSPLIM, LR and the low registers that are not saved automatically. */
            "	mov r4, r8									\n"/* r4 = r8. */
            "	mov r5, r9									\n"/* r5 = r9. */
            "	mov r6, r10									\n"/* r6 = r10. */
            "	mov r7, r11									\n"/* r7 = r11. */
            "	stmia r2!, {r4-r7}							\n"/* Store the high registers that are not saved automatically. */
        #endif /* configENABLE_MPU */
        "													\n"
        " select_next_task:									\n"
        "	cpsid i											\n"
        "	bl vTaskSwitchContext							\n"
        "	cpsie i											\n"
        "													\n"
        "	ldr r3, pxCurrentTCBConst						\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        "	ldr r1, [r3]									\n"/* Read pxCurrentTCB. */
        "	ldr r2, [r1]									\n"/* The first item in pxCurrentTCB is the task top of stack. r2 now points to the top of stack. */
        "													\n"
        #if ( configENABLE_MPU == 1 )
            "	dmb											\n"/* Complete outstanding transfers before disabling MPU. */
            "	ldr r3, xMPUCTRLConst						\n"/* r3 = 0xe000ed94 [Location of MPU_CTRL]. */
            "	ldr r4, [r3]								\n"/* Read the value of MPU_CTRL. */
            "	movs r5, #1									\n"/* r5 = 1. */
            "	bics r4, r5									\n"/* r4 = r4 & ~r5 i.e. Clear the bit 0 in r4. */
            "	str r4, [r3]								\n"/* Disable MPU. */
            "												\n"
            "	adds r1, #4									\n"/* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
            "	ldr r4, [r1]								\n"/* r4 = *r1 i.e. r4 = MAIR0. */
            "	ldr r3, xMAIR0Const							\n"/* r3 = 0xe000edc0 [Location of MAIR0]. */
            "	str r4, [r3]								\n"/* Program MAIR0. */
            "	ldr r4, xRNRConst							\n"/* r4 = 0xe000ed98 [Location of RNR]. */
            "	adds r1, #4									\n"/* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
            "	movs r5, #4									\n"/* r5 = 4. */
            "	str  r5, [r4]								\n"/* Program RNR = 4. */
            "	ldmia r1!, {r6,r7}							\n"/* Read first set of RBAR/RLAR from TCB. */
            "	ldr  r3, xRBARConst							\n"/* r3 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r3!, {r6,r7}							\n"/* Write first set of RBAR/RLAR registers. */
            "	movs r5, #5									\n"/* r5 = 5. */
            "	str  r5, [r4]								\n"/* Program RNR = 5. */
            "	ldmia r1!, {r6,r7}							\n"/* Read second set of RBAR/RLAR from TCB. */
            "	ldr  r3, xRBARConst							\n"/* r3 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r3!, {r6,r7}							\n"/* Write second set of RBAR/RLAR registers. */
            "	movs r5, #6									\n"/* r5 = 6. */
            "	str  r5, [r4]								\n"/* Program RNR = 6. */
            "	ldmia r1!, {r6,r7}							\n"/* Read third set of RBAR/RLAR from TCB. */
            "	ldr  r3, xRBARConst							\n"/* r3 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r3!, {r6,r7}							\n"/* Write third set of RBAR/RLAR registers. */
            "	movs r5, #7									\n"/* r5 = 7. */
            "	str  r5, [r4]								\n"/* Program RNR = 7. */
            "	ldmia r1!, {r6,r7}							\n"/* Read fourth set of RBAR/RLAR from TCB. */
            "	ldr  r3, xRBARConst							\n"/* r3 = 0xe000ed9c [Location of RBAR]. */
            "	stmia r3!, {r6,r7}							\n"/* Write fourth set of RBAR/RLAR registers. */
            "												\n"
            "	ldr r3, xMPUCTRLConst						\n"/* r3 = 0xe000ed94 [Location of MPU_CTRL]. */
            "	ldr r4, [r3]								\n"/* Read the value of MPU_CTRL. */
            "	movs r5, #1									\n"/* r5 = 1. */
            "	orrs r4, r5									\n"/* r4 = r4 | r5 i.e. Set the bit 0 in r4. */
            "	str r4, [r3]								\n"/* Enable MPU. */
            "	dsb											\n"/* Force memory writes before continuing. */
        #endif /* configENABLE_MPU */
        "													\n"
        #if ( configENABLE_MPU == 1 )
            "	ldmia r2!, {r0, r1, r3, r4}					\n"/* Read from stack - r0 = xSecureContext, r1 = PSPLIM, r3 = CONTROL and r4 = LR. */
            "	msr psplim, r1								\n"/* Restore the PSPLIM register value for the task. */
            "	msr control, r3								\n"/* Restore the CONTROL register value for the task. */
            "	mov lr, r4									\n"/* LR = r4. */
            "	ldr r3, xSecureContextConst					\n"/* Read the location of xSecureContext i.e. &( xSecureContext ). */
            "	str r0, [r3]								\n"/* Restore the task's xSecureContext. */
            "	cbz r0, restore_ns_context					\n"/* If there is no secure context for the task, restore the non-secure context. */
             "	ldr r3, pxCurrentTCBConst					\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
            "	ldr r1, [r3]								\n"/* Read pxCurrentTCB. */
            "	push {r2, r4}								\n"
            "	bl SecureContext_LoadContext				\n"/* Restore the secure context. Params are in r0 and r1. r0 = xSecureContext and r1 = pxCurrentTCB. */
            "	pop {r2, r4}								\n"
            "	mov lr, r4									\n"/* LR = r4. */
            "	lsls r1, r4, #25							\n"/* r1 = r4 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
            "	bpl restore_ns_context						\n"/* bpl - branch if positive or zero. If r1 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
            "	msr psp, r2									\n"/* Remember the new top of stack for the task. */
            "	bx lr										\n"
        #else /* configENABLE_MPU */
            "	ldmia r2!, {r0, r1, r4}						\n"/* Read from stack - r0 = xSecureContext, r1 = PSPLIM and r4 = LR. */
            "	msr psplim, r1								\n"/* Restore the PSPLIM register value for the task. */
            "	mov lr, r4									\n"/* LR = r4. */
            "	ldr r3, xSecureContextConst					\n"/* Read the location of xSecureContext i.e. &( xSecureContext ). */
            "	str r0, [r3]								\n"/* Restore the task's xSecureContext. */
            "	cbz r0, restore_ns_context					\n"/* If there is no secure context for the task, restore the non-secure context. */
            "	ldr r3, pxCurrentTCBConst					\n"/* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
            "	ldr r1, [r3]								\n"/* Read pxCurrentTCB. */
            "	push {r2, r4}								\n"
            "	bl SecureContext_LoadContext				\n"/* Restore the secure context. Params are in r0 and r1. r0 = xSecureContext and r1 = pxCurrentTCB. */
            "	pop {r2, r4}								\n"
            "	mov lr, r4									\n"/* LR = r4. */
            "	lsls r1, r4, #25							\n"/* r1 = r4 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
            "	bpl restore_ns_context						\n"/* bpl - branch if positive or zero. If r1 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
            "	msr psp, r2									\n"/* Remember the new top of stack for the task. */
            "	bx lr										\n"
        #endif /* configENABLE_MPU */
        "													\n"
        " restore_ns_context:								\n"
        "	adds r2, r2, #16								\n"/* Move to the high registers. */
        "	ldmia r2!, {r4-r7}								\n"/* Restore the high registers that are not automatically restored. */
        "	mov r8, r4										\n"/* r8 = r4. */
        "	mov r9, r5										\n"/* r9 = r5. */
        "	mov r10, r6										\n"/* r10 = r6. */
        "	mov r11, r7										\n"/* r11 = r7. */
        "	msr psp, r2										\n"/* Remember the new top of stack for the task. */
        "	subs r2, r2, #32								\n"/* Go back to the low registers. */
        "	ldmia r2!, {r4-r7}								\n"/* Restore the low registers that are not automatically restored. */
        "	bx lr											\n"
        "													\n"
        "	.align 4										\n"
        "pxCurrentTCBConst: .word pxCurrentTCB				\n"
        "xSecureContextConst: .word xSecureContext			\n"
        #if ( configENABLE_MPU == 1 )
            "xMPUCTRLConst: .word 0xe000ed94				\n"
            "xMAIR0Const: .word 0xe000edc0					\n"
            "xRNRConst: .word 0xe000ed98					\n"
            "xRBARConst: .word 0xe000ed9c					\n"
        #endif /* configENABLE_MPU */
    );
}
/*-----------------------------------------------------------*/

void SVC_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	movs r0, #4										\n"
        "	mov r1, lr										\n"
        "	tst r0, r1										\n"
        "	beq stacking_used_msp							\n"
        "	mrs r0, psp										\n"
        "	ldr r2, svchandler_address_const				\n"
        "	bx r2											\n"
        " stacking_used_msp:								\n"
        "	mrs r0, msp										\n"
        "	ldr r2, svchandler_address_const				\n"
        "	bx r2											\n"
        "													\n"
        "	.align 4										\n"
        "svchandler_address_const: .word vPortSVCHandler_C	\n"
    );
}
/*-----------------------------------------------------------*/

void vPortAllocateSecureContext( uint32_t ulSecureStackSize ) /* __attribute__ (( naked )) */
{
    __asm volatile
    (
        "	svc %0											\n"/* Secure context is allocated in the supervisor call. */
        "	bx lr											\n"/* Return. */
        ::"i" ( portSVC_ALLOCATE_SECURE_CONTEXT ) : "memory"
    );
}
/*-----------------------------------------------------------*/

void vPortFreeSecureContext( uint32_t * pulTCB ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
    __asm volatile
    (
        "	ldr r2, [r0]									\n"/* The first item in the TCB is the top of the stack. */
        "	ldr r1, [r2]									\n"/* The first item on the stack is the task's xSecureContext. */
        "	cmp r1, #0										\n"/* Raise svc if task's xSecureContext is not NULL. */
        "	bne free_secure_context							\n"/* Branch if r1 != 0. */
        "	bx lr											\n"/* There is no secure context (xSecureContext is NULL). */
        " free_secure_context:								\n"
        "	svc %0											\n"/* Secure context is freed in the supervisor call. */
        "	bx lr											\n"/* Return. */
        ::"i" ( portSVC_FREE_SECURE_CONTEXT ) : "memory"
    );
}
/*-----------------------------------------------------------*/
