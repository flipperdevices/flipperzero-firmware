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

	IMPORT  ulCriticalNesting		;
	IMPORT	pxCurrentTCB			;


	MACRO
	portRESTORE_CONTEXT


	LDR		R0, =pxCurrentTCB		; Set the LR to the task stack.  The location was...
	LDR		R0, [R0]				; ... stored in pxCurrentTCB
	LDR		LR, [R0]

	LDR		R0, =ulCriticalNesting	; The critical nesting depth is the first item on...
	LDMFD	LR!, {R1}				; ...the stack.  Load it into the ulCriticalNesting var.
	STR		R1, [R0]				;

	LDMFD	LR!, {R0}				; Get the SPSR from the stack.
	MSR		SPSR_cxsf, R0			;

	LDMFD	LR, {R0-R14}^			; Restore all system mode registers for the task.
	NOP								;

	LDR		LR, [LR, #+60]			; Restore the return address

									; And return - correcting the offset in the LR to obtain ...
	SUBS	PC, LR, #4				; ...the correct address.

	MEND

; /**********************************************************************/

	MACRO
	portSAVE_CONTEXT


	STMDB 	SP!, {R0}				; Store R0 first as we need to use it.

	STMDB	SP,{SP}^				; Set R0 to point to the task stack pointer.
	NOP								;
	SUB		SP, SP, #4				;
	LDMIA	SP!,{R0}				;

	STMDB	R0!, {LR}				; Push the return address onto the stack.
	MOV		LR, R0					; Now we have saved LR we can use it instead of R0.
	LDMIA	SP!, {R0}				; Pop R0 so we can save it onto the system mode stack.

	STMDB	LR,{R0-LR}^				; Push all the system mode registers onto the task stack.
	NOP								;
	SUB		LR, LR, #60				;

	MRS		R0, SPSR				; Push the SPSR onto the task stack.
	STMDB	LR!, {R0}				;

	LDR		R0, =ulCriticalNesting	;
	LDR		R0, [R0]				;
	STMDB	LR!, {R0}				;

	LDR		R0, =pxCurrentTCB		; Store the new top of stack for the task.
	LDR		R1, [R0]				;
	STR		LR, [R1]				;

	MEND

	END
