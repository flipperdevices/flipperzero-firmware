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

Changes from V3.0.1
*/
#ifndef PORTMACRO_H
#define PORTMACRO_H

#if !defined(_SERIES) || _SERIES != 18
	#error "WizC supports FreeRTOS on the Microchip PIC18-series only"
#endif

#if !defined(QUICKCALL) || QUICKCALL != 1
	#error "QuickCall must be enabled (see ProjectOptions/Optimisations)"
#endif

#include <stddef.h>
#include <pic.h>

#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		portFLOAT
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	char

typedef portSTACK_TYPE StackType_t;
typedef signed char BaseType_t;
typedef unsigned char UBaseType_t;


#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t )	( 0xFFFF )
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t )	( 0xFFFFFFFF )
#endif

#define portBYTE_ALIGNMENT			1

/*-----------------------------------------------------------*/

/*
 * Constant used for context switch macro when we require the interrupt
 * enable state to be forced when the interrupted task is switched back in.
 */
#define portINTERRUPTS_FORCED				(0x01)

/*
 * Constant used for context switch macro when we require the interrupt
 * enable state to be unchanged when the interrupted task is switched back in.
 */
#define portINTERRUPTS_UNCHANGED			(0x00)

/* Initial interrupt enable state for newly created tasks.  This value is
 * used when a task switches in for the first time.
 */
#define portINTERRUPTS_INITIAL_STATE		(portINTERRUPTS_FORCED)

/*
 * Macros to modify the global interrupt enable bit in INTCON.
 */
#define portDISABLE_INTERRUPTS()	\
	do								\
	{								\
		bGIE=0;						\
	} while(bGIE)	// MicroChip recommends this check!

#define portENABLE_INTERRUPTS()		\
	do								\
	{								\
		bGIE=1;						\
	} while(0)

/*-----------------------------------------------------------*/

/*
 * Critical section macros.
 */
extern uint8_t ucCriticalNesting;

#define portNO_CRITICAL_SECTION_NESTING		( ( uint8_t ) 0 )

#define portENTER_CRITICAL()										\
	do																\
	{																\
		portDISABLE_INTERRUPTS();									\
																	\
		/*															\
		 * Now interrupts are disabled ucCriticalNesting			\
		 * can be accessed directly. Increment						\
		 * ucCriticalNesting to keep a count of how					\
		 * many times portENTER_CRITICAL() has been called. 		\
		 */															\
		ucCriticalNesting++;										\
	} while(0)

#define portEXIT_CRITICAL()											\
	do																\
	{																\
		if(ucCriticalNesting > portNO_CRITICAL_SECTION_NESTING)		\
		{															\
			/*														\
			 * Decrement the nesting count as we are leaving a		\
			 * critical section.									\
			 */														\
			ucCriticalNesting--;									\
		}															\
																	\
		/*															\
		 * If the nesting level has reached zero then				\
		 * interrupts should be re-enabled.							\
		 */															\
		if( ucCriticalNesting == portNO_CRITICAL_SECTION_NESTING )	\
		{															\
			portENABLE_INTERRUPTS();								\
		}															\
	} while(0)

/*-----------------------------------------------------------*/

/*
 * The minimal stacksize is calculated on the first reference of
 * portMINIMAL_STACK_SIZE. Some input to this calculation is
 * compiletime determined, other input is port-defined (see port.c)
 */
extern uint16_t usPortCALCULATE_MINIMAL_STACK_SIZE( void );
extern uint16_t usCalcMinStackSize;

#define portMINIMAL_STACK_SIZE					\
	((usCalcMinStackSize == 0)					\
		? usPortCALCULATE_MINIMAL_STACK_SIZE()	\
		: usCalcMinStackSize )

/*
 * WizC uses a downgrowing stack
 */
#define portSTACK_GROWTH			( -1 )

/*-----------------------------------------------------------*/

/*
 * Macro's that pushes all the registers that make up the context of a task onto
 * the stack, then saves the new top of stack into the TCB. TOSU and TBLPTRU
 * are only saved/restored on devices with more than 64kB (32k Words) ROM.
 *
 * The stackpointer is helt by WizC in FSR2 and points to the first free byte.
 * WizC uses a "downgrowing" stack. There is no framepointer.
 *
 * We keep track of the interruptstatus using ucCriticalNesting. When this
 * value equals zero, interrupts have to be enabled upon exit from the
 * portRESTORE_CONTEXT macro.
 *
 * If this is called from an ISR then the interrupt enable bits must have been
 * set for the ISR to ever get called.  Therefore we want to save
 * ucCriticalNesting with value zero. This means the interrupts will again be
 * re-enabled when the interrupted task is switched back in.
 *
 * If this is called from a manual context switch (i.e. from a call to yield),
 * then we want to keep the current value of ucCritialNesting so it is restored
 * with its current value. This allows a yield from within a critical section.
 *
 * The compiler uses some locations at the bottom of RAM for temporary
 * storage. The compiler may also have been instructed to optimize
 * function-parameters and local variables to global storage. The compiler
 * uses an area called LocOpt for this wizC feature.
 * The total overheadstorage has to be saved in it's entirety as part of
 * a task context. These macro's store/restore from data address 0x0000 to
 * (OVERHEADPAGE0-LOCOPTSIZE+MAXLOCOPTSIZE - 1).
 * OVERHEADPAGE0, LOCOPTSIZE and MAXLOCOPTSIZE are compiler-generated
 * assembler definitions.
 */

#define	portSAVE_CONTEXT( ucInterruptForced )						\
	do																\
	{																\
		portDISABLE_INTERRUPTS();									\
																	\
		_Pragma("asm")												\
			;														\
			; Push the relevant SFR's onto the task's stack			\
			;														\
			movff   STATUS,POSTDEC2									\
			movff	WREG,POSTDEC2									\
			movff	BSR,POSTDEC2									\
			movff	PRODH,POSTDEC2									\
			movff	PRODL,POSTDEC2									\
			movff	FSR0H,POSTDEC2									\
			movff	FSR0L,POSTDEC2									\
			movff	FSR1H,POSTDEC2									\
			movff	FSR1L,POSTDEC2									\
			movff	TABLAT,POSTDEC2									\
			if __ROMSIZE > 0x8000									\
				movff	TBLPTRU,POSTDEC2							\
			endif													\
			movff	TBLPTRH,POSTDEC2								\
			movff	TBLPTRL,POSTDEC2								\
			if __ROMSIZE > 0x8000									\
				movff	PCLATU,POSTDEC2								\
			endif													\
			movff	PCLATH,POSTDEC2									\
			;														\
			; Store the compiler-scratch-area as described above.	\
			;														\
			movlw	OVERHEADPAGE0-LOCOPTSIZE+MAXLOCOPTSIZE			\
			clrf	FSR0L,ACCESS									\
			clrf	FSR0H,ACCESS									\
		_rtos_S1:													\
			movff	POSTINC0,POSTDEC2								\
			decfsz	WREG,W,ACCESS									\
			SMARTJUMP _rtos_S1										\
			;														\
			; Save the pic call/return-stack belonging to the		\
			; current task by copying it to the task's software-	\
			; stack. We save the hardware stack pointer (which		\
			; is the number of addresses on the stack) in the		\
			; W-register first because we need it later and it		\
			; is modified in the save-loop by executing pop's.		\
			; After the loop the W-register is stored on the		\
			; stack, too.											\
			;														\
			movf	STKPTR,W,ACCESS									\
			bz		_rtos_s3										\
		_rtos_S2:													\
			if __ROMSIZE > 0x8000									\
				movff	TOSU,POSTDEC2								\
			endif													\
			movff	TOSH,POSTDEC2									\
			movff	TOSL,POSTDEC2									\
			pop														\
			tstfsz	STKPTR,ACCESS									\
			SMARTJUMP _rtos_S2										\
		_rtos_s3:													\
			movwf	POSTDEC2,ACCESS									\
			;														\
			; Next the value for ucCriticalNesting used by the		\
			; task is stored on the stack. When						\
			; (ucInterruptForced == portINTERRUPTS_FORCED), we save	\
			; it as 0 (portNO_CRITICAL_SECTION_NESTING).			\
			;														\
			if ucInterruptForced == portINTERRUPTS_FORCED			\
				clrf POSTDEC2,ACCESS								\
			else													\
				movff	ucCriticalNesting,POSTDEC2					\
			endif													\
			;														\
			; Save the new top of the software stack in the TCB.	\
			;														\
			movff	pxCurrentTCB,FSR0L								\
			movff	pxCurrentTCB+1,FSR0H							\
			movff	FSR2L,POSTINC0									\
			movff	FSR2H,POSTINC0									\
		_Pragma("asmend")											\
	} while(0)

/************************************************************/

/*
 * This is the reverse of portSAVE_CONTEXT.
 */
#define portRESTORE_CONTEXT()										\
	do																\
	{																\
		_Pragma("asm")												\
			;														\
			; Set FSR0 to point to pxCurrentTCB->pxTopOfStack.		\
			;														\
			movff	pxCurrentTCB,FSR0L								\
			movff	pxCurrentTCB+1,FSR0H							\
			;														\
			; De-reference FSR0 to set the address it holds into	\
			; FSR2 (i.e. *( pxCurrentTCB->pxTopOfStack ) ). FSR2	\
			; is used by wizC as stackpointer.						\
			;														\
			movff	POSTINC0,FSR2L									\
			movff	POSTINC0,FSR2H									\
			;														\
			; Next, the value for ucCriticalNesting used by the		\
			; task is retrieved from the stack.						\
			;														\
			movff	PREINC2,ucCriticalNesting						\
			;														\
			; Rebuild the pic call/return-stack. The number of		\
			; return addresses is the next item on the task stack.	\
			; Save this number in PRODL. Then fetch the addresses	\
			; and store them on the hardwarestack.					\
			; The datasheets say we can't use movff here...			\
			;														\
			movff	PREINC2,PRODL	// Use PRODL as tempregister	\
			clrf	STKPTR,ACCESS									\
		_rtos_R1:													\
			push													\
			movf	PREINC2,W,ACCESS								\
			movwf	TOSL,ACCESS										\
			movf	PREINC2,W,ACCESS								\
			movwf	TOSH,ACCESS										\
			if __ROMSIZE > 0x8000									\
				movf	PREINC2,W,ACCESS							\
				movwf	TOSU,ACCESS									\
			else													\
				clrf	TOSU,ACCESS									\
			endif													\
			decfsz	PRODL,F,ACCESS									\
			SMARTJUMP _rtos_R1										\
			;														\
			; Restore the compiler's working storage area to page 0	\
			;														\
			movlw	OVERHEADPAGE0-LOCOPTSIZE+MAXLOCOPTSIZE			\
			movwf	FSR0L,ACCESS									\
			clrf	FSR0H,ACCESS									\
		_rtos_R2:													\
			decf	FSR0L,F,ACCESS									\
			movff	PREINC2,INDF0									\
			tstfsz	FSR0L,ACCESS									\
			SMARTJUMP _rtos_R2										\
			;														\
			; Restore the sfr's forming the tasks context.			\
			; We cannot yet restore bsr, w and status because		\
			; we need these	registers for a final test.				\
			;														\
			movff	PREINC2,PCLATH									\
			if __ROMSIZE > 0x8000									\
				movff	PREINC2,PCLATU								\
			else													\
				clrf	PCLATU,ACCESS								\
			endif													\
			movff	PREINC2,TBLPTRL									\
			movff	PREINC2,TBLPTRH									\
			if __ROMSIZE > 0x8000									\
				movff	PREINC2,TBLPTRU								\
			else													\
				clrf	TBLPTRU,ACCESS								\
			endif													\
			movff	PREINC2,TABLAT									\
			movff	PREINC2,FSR1L									\
			movff	PREINC2,FSR1H									\
			movff	PREINC2,FSR0L									\
			movff	PREINC2,FSR0H									\
			movff	PREINC2,PRODL									\
			movff	PREINC2,PRODH									\
			;														\
			; The return from portRESTORE_CONTEXT() depends on		\
			; the value of ucCriticalNesting. When it is zero,		\
			; interrupts need to be enabled. This is done via a		\
			; retfie instruction because we need the				\
			; interrupt-enabling and the return to the restored		\
			; task to be uninterruptable.							\
	 		; Because bsr, status and W are affected by the test	\
	 		; they are restored after the test.						\
			;														\
			movlb	ucCriticalNesting>>8							\
			tstfsz	ucCriticalNesting,BANKED						\
			SMARTJUMP _rtos_R4										\
		_rtos_R3:													\
			movff	PREINC2,BSR										\
			movff	PREINC2,WREG									\
			movff	PREINC2,STATUS									\
			retfie	0		; Return enabling interrupts			\
		_rtos_R4:													\
			movff	PREINC2,BSR										\
			movff	PREINC2,WREG									\
			movff	PREINC2,STATUS									\
			return	0		; Return without affecting interrupts	\
		_Pragma("asmend")											\
	} while(0)

/*-----------------------------------------------------------*/

#define portTICK_PERIOD_MS	( ( TickType_t ) 1000 / configTICK_RATE_HZ )

/*-----------------------------------------------------------*/

extern void vPortYield( void );
#define portYIELD()				vPortYield()

#define portNOP()	_Pragma("asm")									\
						nop											\
					_Pragma("asmend")

/*-----------------------------------------------------------*/

#define portTASK_FUNCTION( xFunction, pvParameters )	 	\
	void pointed xFunction( void *pvParameters )		\
	_Pragma(asmfunc xFunction)

#define portTASK_FUNCTION_PROTO		portTASK_FUNCTION
/*-----------------------------------------------------------*/


#define volatile
#define register

#endif /* PORTMACRO_H */

