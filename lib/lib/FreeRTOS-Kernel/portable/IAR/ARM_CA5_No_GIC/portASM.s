;/*
; * FreeRTOS Kernel <DEVELOPMENT BRANCH>
; * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
; *
; * SPDX-License-Identifier: MIT
; *
; * Permission is hereby granted, free of charge, to any person obtaining a copy of
; * this software and associated documentation files (the "Software"), to deal in
; * the Software without restriction, including without limitation the rights to
; * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
; * the Software, and to permit persons to whom the Software is furnished to do so,
; * subject to the following conditions:
; *
; * The above copyright notice and this permission notice shall be included in all
; * copies or substantial portions of the Software.
; *
; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
; * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
; * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
; * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; *
; * https://www.FreeRTOS.org
; * https://github.com/FreeRTOS
; *
; */

	INCLUDE FreeRTOSConfig.h
	INCLUDE portmacro.h

	EXTERN	vTaskSwitchContext
	EXTERN	ulPortYieldRequired
	EXTERN	ulPortInterruptNesting
	EXTERN	vApplicationIRQHandler

	PUBLIC	FreeRTOS_SWI_Handler
	PUBLIC  FreeRTOS_IRQ_Handler
	PUBLIC 	vPortRestoreTaskContext

SYS_MODE			EQU		0x1f
SVC_MODE			EQU		0x13
IRQ_MODE			EQU		0x12

	SECTION .text:CODE:ROOT(2)
	ARM

	INCLUDE portASM.h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC handler is used to yield a task.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FreeRTOS_SWI_Handler

	PRESERVE8

	; Save the context of the current task and select a new task to run.
	portSAVE_CONTEXT
	LDR R0, =vTaskSwitchContext
	BLX	R0
	portRESTORE_CONTEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; vPortRestoreTaskContext is used to start the scheduler.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
vPortRestoreTaskContext

	PRESERVE8

	; Switch to system mode
	CPS		#SYS_MODE
	portRESTORE_CONTEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; IRQ interrupt handler used when individual priorities cannot be masked
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FreeRTOS_IRQ_Handler

	PRESERVE8

	; Return to the interrupted instruction.
	SUB		lr, lr, #4

	; Push the return address and SPSR
	PUSH	{lr}
	MRS		lr, SPSR
	PUSH	{lr}

	; Change to supervisor mode to allow reentry.
	CPS		#SVC_MODE

	; Push used registers.
	PUSH	{r0-r4, r12}

	; Increment nesting count.  r3 holds the address of ulPortInterruptNesting
	; for future use.  r1 holds the original ulPortInterruptNesting value for
	; future use.
	LDR		r3, =ulPortInterruptNesting
	LDR		r1, [r3]
	ADD		r4, r1, #1
	STR		r4, [r3]

	; Ensure bit 2 of the stack pointer is clear.  r2 holds the bit 2 value for
	; future use.
	MOV		r2, sp
	AND		r2, r2, #4
	SUB		sp, sp, r2

	PUSH	{r0-r4, lr}

	; Call the port part specific handler.
	LDR		r0, =vApplicationIRQHandler
	BLX		r0
	POP		{r0-r4, lr}
	ADD		sp, sp, r2

	CPSID	i

	; Write to the EOI register.
	LDR 	r4, =configEOI_ADDRESS
	STR		r0, [r4]

	; Restore the old nesting count
	STR		r1, [r3]

	; A context switch is never performed if the nesting count is not 0.
	CMP		r1, #0
	BNE		exit_without_switch

	; Did the interrupt request a context switch?  r1 holds the address of
	; ulPortYieldRequired and r0 the value of ulPortYieldRequired for future
	; use.
	LDR		r1, =ulPortYieldRequired
	LDR		r0, [r1]
	CMP		r0, #0
	BNE		switch_before_exit

exit_without_switch
	; No context switch.  Restore used registers, LR_irq and SPSR before
	; returning.
	POP		{r0-r4, r12}
	CPS		#IRQ_MODE
	POP		{LR}
	MSR		SPSR_cxsf, LR
	POP		{LR}
	MOVS	PC, LR

switch_before_exit
	; A context switch is to be performed.  Clear the context switch pending
	; flag.
	MOV		r0, #0
	STR		r0, [r1]

	; Restore used registers, LR-irq and SPSR before saving the context
	; to the task stack.
	POP		{r0-r4, r12}
	CPS		#IRQ_MODE
	POP		{LR}
	MSR		SPSR_cxsf, LR
	POP		{LR}
	portSAVE_CONTEXT

	; Call the function that selects the new task to execute.
	; vTaskSwitchContext() if vTaskSwitchContext() uses LDRD or STRD
	; instructions, or 8 byte aligned stack allocated data.  LR does not need
	; saving as a new LR will be loaded by portRESTORE_CONTEXT anyway.
	LDR		r0, =vTaskSwitchContext
	BLX		r0

	; Restore the context of, and branch to, the task selected to execute next.
	portRESTORE_CONTEXT

	END




