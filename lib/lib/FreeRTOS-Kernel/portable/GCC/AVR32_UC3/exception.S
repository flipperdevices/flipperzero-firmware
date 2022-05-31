/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT AND BSD-3-Clause
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

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Exception and interrupt vectors.
 *
 * This file maps all events supported by an AVR32UC.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  All AVR32UC devices with an INTC module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation (Now Microchip):
 *                                        https://www.microchip.com \n
 *                       Support and FAQ: https://www.microchip.com/support/
 *
 ******************************************************************************/

/*
 * Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <avr32/io.h>
#include "intc.h"


//! @{
//! \verbatim


  .section  .exception, "ax", @progbits


// Start of Exception Vector Table.

  // EVBA must be aligned with a power of two strictly greater than the EVBA-
  // relative offset of the last vector.
  .balign 0x200

  // Export symbol.
  .global _evba
  .type _evba, @function
_evba:

        .org  0x000
        // Unrecoverable Exception.
_handle_Unrecoverable_Exception:
        rjmp $

        .org  0x004
        // TLB Multiple Hit: UNUSED IN AVR32UC.
_handle_TLB_Multiple_Hit:
        rjmp $

        .org  0x008
        // Bus Error Data Fetch.
_handle_Bus_Error_Data_Fetch:
        rjmp $

        .org  0x00C
         // Bus Error Instruction Fetch.
_handle_Bus_Error_Instruction_Fetch:
        rjmp $

        .org  0x010
        // NMI.
_handle_NMI:
        rjmp $

        .org  0x014
        // Instruction Address.
_handle_Instruction_Address:
        rjmp $

        .org  0x018
        // ITLB Protection.
_handle_ITLB_Protection:
        rjmp $

        .org  0x01C
        // Breakpoint.
_handle_Breakpoint:
        rjmp $

        .org  0x020
        // Illegal Opcode.
_handle_Illegal_Opcode:
        rjmp $

        .org  0x024
        // Unimplemented Instruction.
_handle_Unimplemented_Instruction:
        rjmp $

        .org  0x028
        // Privilege Violation.
_handle_Privilege_Violation:
        rjmp $

        .org  0x02C
        // Floating-Point: UNUSED IN AVR32UC.
_handle_Floating_Point:
        rjmp $

        .org  0x030
        // Coprocessor Absent: UNUSED IN AVR32UC.
_handle_Coprocessor_Absent:
        rjmp $

        .org  0x034
        // Data Address (Read).
_handle_Data_Address_Read:
        rjmp $

        .org  0x038
        // Data Address (Write).
_handle_Data_Address_Write:
        rjmp $

        .org  0x03C
        // DTLB Protection (Read).
_handle_DTLB_Protection_Read:
        rjmp $

        .org  0x040
        // DTLB Protection (Write).
_handle_DTLB_Protection_Write:
        rjmp $

        .org  0x044
        // DTLB Modified: UNUSED IN AVR32UC.
_handle_DTLB_Modified:
        rjmp $

        .org  0x050
        // ITLB Miss: UNUSED IN AVR32UC.
_handle_ITLB_Miss:
        rjmp $

        .org  0x060
        // DTLB Miss (Read): UNUSED IN AVR32UC.
_handle_DTLB_Miss_Read:
        rjmp $

        .org  0x070
        // DTLB Miss (Write): UNUSED IN AVR32UC.
_handle_DTLB_Miss_Write:
        rjmp $

        .org  0x100
        // Supervisor Call.
_handle_Supervisor_Call:
        lda.w   pc, SCALLYield


// Interrupt support.
// The interrupt controller must provide the offset address relative to EVBA.
// Important note:
//   All interrupts call a C function named _get_interrupt_handler.
//   This function will read group and interrupt line number to then return in
//   R12 a pointer to a user-provided interrupt handler.

  .balign 4

_int0:
  // R8-R12, LR, PC and SR are automatically pushed onto the system stack by the
  // CPU upon interrupt entry.
#if 1 // B1832: interrupt stack changed to exception stack if exception is detected.
  mfsr    r12, AVR32_SR
  bfextu  r12, r12, AVR32_SR_M0_OFFSET, AVR32_SR_M0_SIZE + AVR32_SR_M1_SIZE + AVR32_SR_M2_SIZE
  cp.w    r12, 0b110
  brlo    _int0_normal
  lddsp   r12, sp[0 * 4]
  stdsp   sp[6 * 4], r12
  lddsp   r12, sp[1 * 4]
  stdsp   sp[7 * 4], r12
  lddsp   r12, sp[3 * 4]
  sub     sp, -6 * 4
  rete
_int0_normal:
#endif
  mov     r12, 0  // Pass the int_lev parameter to the _get_interrupt_handler function.
  call    _get_interrupt_handler
  cp.w    r12, 0  // Get the pointer to the interrupt handler returned by the function.
  movne   pc, r12 // If this was not a spurious interrupt (R12 != NULL), jump to the handler.
  rete            // If this was a spurious interrupt (R12 == NULL), return from event handler.

_int1:
  // R8-R12, LR, PC and SR are automatically pushed onto the system stack by the
  // CPU upon interrupt entry.
#if 1 // B1832: interrupt stack changed to exception stack if exception is detected.
  mfsr    r12, AVR32_SR
  bfextu  r12, r12, AVR32_SR_M0_OFFSET, AVR32_SR_M0_SIZE + AVR32_SR_M1_SIZE + AVR32_SR_M2_SIZE
  cp.w    r12, 0b110
  brlo    _int1_normal
  lddsp   r12, sp[0 * 4]
  stdsp   sp[6 * 4], r12
  lddsp   r12, sp[1 * 4]
  stdsp   sp[7 * 4], r12
  lddsp   r12, sp[3 * 4]
  sub     sp, -6 * 4
  rete
_int1_normal:
#endif
  mov     r12, 1  // Pass the int_lev parameter to the _get_interrupt_handler function.
  call    _get_interrupt_handler
  cp.w    r12, 0  // Get the pointer to the interrupt handler returned by the function.
  movne   pc, r12 // If this was not a spurious interrupt (R12 != NULL), jump to the handler.
  rete            // If this was a spurious interrupt (R12 == NULL), return from event handler.

_int2:
  // R8-R12, LR, PC and SR are automatically pushed onto the system stack by the
  // CPU upon interrupt entry.
#if 1 // B1832: interrupt stack changed to exception stack if exception is detected.
  mfsr    r12, AVR32_SR
  bfextu  r12, r12, AVR32_SR_M0_OFFSET, AVR32_SR_M0_SIZE + AVR32_SR_M1_SIZE + AVR32_SR_M2_SIZE
  cp.w    r12, 0b110
  brlo    _int2_normal
  lddsp   r12, sp[0 * 4]
  stdsp   sp[6 * 4], r12
  lddsp   r12, sp[1 * 4]
  stdsp   sp[7 * 4], r12
  lddsp   r12, sp[3 * 4]
  sub     sp, -6 * 4
  rete
_int2_normal:
#endif
  mov     r12, 2  // Pass the int_lev parameter to the _get_interrupt_handler function.
  call    _get_interrupt_handler
  cp.w    r12, 0  // Get the pointer to the interrupt handler returned by the function.
  movne   pc, r12 // If this was not a spurious interrupt (R12 != NULL), jump to the handler.
  rete            // If this was a spurious interrupt (R12 == NULL), return from event handler.

_int3:
  // R8-R12, LR, PC and SR are automatically pushed onto the system stack by the
  // CPU upon interrupt entry.
#if 1 // B1832: interrupt stack changed to exception stack if exception is detected.
  mfsr    r12, AVR32_SR
  bfextu  r12, r12, AVR32_SR_M0_OFFSET, AVR32_SR_M0_SIZE + AVR32_SR_M1_SIZE + AVR32_SR_M2_SIZE
  cp.w    r12, 0b110
  brlo    _int3_normal
  lddsp   r12, sp[0 * 4]
  stdsp   sp[6 * 4], r12
  lddsp   r12, sp[1 * 4]
  stdsp   sp[7 * 4], r12
  lddsp   r12, sp[3 * 4]
  sub     sp, -6 * 4
  rete
_int3_normal:
#endif
  mov     r12, 3  // Pass the int_lev parameter to the _get_interrupt_handler function.
  call    _get_interrupt_handler
  cp.w    r12, 0  // Get the pointer to the interrupt handler returned by the function.
  movne   pc, r12 // If this was not a spurious interrupt (R12 != NULL), jump to the handler.
  rete            // If this was a spurious interrupt (R12 == NULL), return from event handler.


// Constant data area.

  .balign 4

  // Values to store in the interrupt priority registers for the various interrupt priority levels.
  // The interrupt priority registers contain the interrupt priority level and
  // the EVBA-relative interrupt vector offset.
  .global ipr_val
  .type ipr_val, @object
ipr_val:
  .word (INT0 << AVR32_INTC_IPR0_INTLEV_OFFSET) | (_int0 - _evba),\
        (INT1 << AVR32_INTC_IPR0_INTLEV_OFFSET) | (_int1 - _evba),\
        (INT2 << AVR32_INTC_IPR0_INTLEV_OFFSET) | (_int2 - _evba),\
        (INT3 << AVR32_INTC_IPR0_INTLEV_OFFSET) | (_int3 - _evba)


//! \endverbatim
//! @}
