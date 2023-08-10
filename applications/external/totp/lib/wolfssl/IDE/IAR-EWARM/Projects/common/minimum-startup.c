/* minimum-startup.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <stdint.h>
#pragma language=extended

extern void __iar_program_start(void);
static void Reset(void)
{
    __iar_program_start();
}

static void Nmi  (void){ while(1) ; }
static void Fault(void){ while(1) ; }

static unsigned long long Stack[256*4*16] @ ".noinit";

typedef union
{
    void (*Handler)(void);
    uint32_t Ptr;
} Vector;

__root const Vector  __vector_table[100] @ ".intvec" =
{
    { .Ptr = (uint32_t)Stack + sizeof(Stack) },
                                // stack top
    Reset,                   // reset
    Nmi,                        // NMI
    Fault,                      // hard fault
    Fault,                      // MPU fault
    Fault,                      // bus fault
    Fault,                      // usage fault
};

