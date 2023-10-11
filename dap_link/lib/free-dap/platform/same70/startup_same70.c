/*
 * Copyright (c) 2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*- Includes ----------------------------------------------------------------*/
#include "same70.h"

/*- Definitions -------------------------------------------------------------*/
#define DUMMY         __attribute__ ((weak, alias ("irq_handler_dummy")))

/*- Prototypes --------------------------------------------------------------*/
void irq_handler_reset(void);
DUMMY void irq_handler_nmi(void);
DUMMY void irq_handler_hard_fault(void);
DUMMY void irq_handler_mm_fault(void);
DUMMY void irq_handler_bus_fault(void);
DUMMY void irq_handler_usage_fault(void);
DUMMY void irq_handler_sv_call(void);
DUMMY void irq_handler_debug_mon(void);
DUMMY void irq_handler_pend_sv(void);
DUMMY void irq_handler_sys_tick(void);

DUMMY void irq_handler_supc(void);
DUMMY void irq_handler_rstc(void);
DUMMY void irq_handler_rtc(void);
DUMMY void irq_handler_rtt(void);
DUMMY void irq_handler_wdt(void);
DUMMY void irq_handler_pmc(void);
DUMMY void irq_handler_efc(void);
DUMMY void irq_handler_uart0(void);
DUMMY void irq_handler_uart1(void);
DUMMY void irq_handler_pioa(void);
DUMMY void irq_handler_piob(void);
DUMMY void irq_handler_pioc(void);
DUMMY void irq_handler_usart0(void);
DUMMY void irq_handler_usart1(void);
DUMMY void irq_handler_usart2(void);
DUMMY void irq_handler_piod(void);
DUMMY void irq_handler_pioe(void);
DUMMY void irq_handler_hsmci(void);
DUMMY void irq_handler_twihs0(void);
DUMMY void irq_handler_twihs1(void);
DUMMY void irq_handler_spi0(void);
DUMMY void irq_handler_ssc(void);
DUMMY void irq_handler_tc0(void);
DUMMY void irq_handler_tc1(void);
DUMMY void irq_handler_tc2(void);
DUMMY void irq_handler_tc3(void);
DUMMY void irq_handler_tc4(void);
DUMMY void irq_handler_tc5(void);
DUMMY void irq_handler_afec0(void);
DUMMY void irq_handler_dacc(void);
DUMMY void irq_handler_pwm0(void);
DUMMY void irq_handler_icm(void);
DUMMY void irq_handler_acc(void);
DUMMY void irq_handler_usbhs(void);
DUMMY void irq_handler_mcan0(void);
DUMMY void irq_handler_mcan1(void);
DUMMY void irq_handler_gmac(void);
DUMMY void irq_handler_afec1(void);
DUMMY void irq_handler_twihs2(void);
DUMMY void irq_handler_spi1(void);
DUMMY void irq_handler_qspi(void);
DUMMY void irq_handler_uart2(void);
DUMMY void irq_handler_uart3(void);
DUMMY void irq_handler_uart4(void);
DUMMY void irq_handler_tc6(void);
DUMMY void irq_handler_tc7(void);
DUMMY void irq_handler_tc8(void);
DUMMY void irq_handler_tc9(void);
DUMMY void irq_handler_tc10(void);
DUMMY void irq_handler_tc11(void);
DUMMY void irq_handler_aes(void);
DUMMY void irq_handler_trng(void);
DUMMY void irq_handler_xdmac(void);
DUMMY void irq_handler_isi(void);
DUMMY void irq_handler_pwm1(void);
DUMMY void irq_handler_sdramc(void);
DUMMY void irq_handler_rswdt(void);

int main(void);

extern void _stack_top(void);
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

/*- Variables ---------------------------------------------------------------*/

//-----------------------------------------------------------------------------
__attribute__ ((used, section(".vectors")))
void (* const vectors[])(void) =
{
  &_stack_top,                   // 0 - Initial Stack Pointer Value

  // Cortex-M7 handlers
  irq_handler_reset,             // 1 - Reset
  irq_handler_nmi,               // 2 - NMI
  irq_handler_hard_fault,        // 3 - Hard Fault
  irq_handler_mm_fault,          // 4 - MM Fault
  irq_handler_bus_fault,         // 5 - Bus Fault
  irq_handler_usage_fault,       // 6 - Usage Fault
  0,                             // 7 - Reserved
  0,                             // 8 - Reserved
  0,                             // 9 - Reserved
  0,                             // 10 - Reserved
  irq_handler_sv_call,           // 11 - SVCall
  irq_handler_debug_mon,         // 12 - Debug
  0,                             // 13 - Reserved
  irq_handler_pend_sv,           // 14 - PendSV
  irq_handler_sys_tick,          // 15 - SysTick

  // Peripheral handlers
  irq_handler_supc,              // 0 - Supply Controller
  irq_handler_rstc,              // 1 - Reset Controller
  irq_handler_rtc,               // 2 - Real Time Clock
  irq_handler_rtt,               // 3 - Real Time Timer
  irq_handler_wdt,               // 4 - Watchdog Timer
  irq_handler_pmc,               // 5 - Power Management Controller
  irq_handler_efc,               // 6 - Embedded Flash Controller
  irq_handler_uart0,             // 7 - UART 0
  irq_handler_uart1,             // 8 - UART 1
  0,                             // 9 - Reserved
  irq_handler_pioa,              // 10 - Parallel I/O Controller A
  irq_handler_piob,              // 11 - Parallel I/O Controller B
  irq_handler_pioc,              // 12 - Parallel I/O Controller C
  irq_handler_usart0,            // 13 - USART 0
  irq_handler_usart1,            // 14 - USART 1
  irq_handler_usart2,            // 15 - USART 2
  irq_handler_piod,              // 16 - Parallel I/O Controller D
  irq_handler_pioe,              // 17 - Parallel I/O Controller E
  irq_handler_hsmci,             // 18 - High Speed Multimedia Card Interface
  irq_handler_twihs0,            // 19 - Two-wire Interface 0
  irq_handler_twihs1,            // 20 - Two-wire Interface 1
  irq_handler_spi0,              // 21 - Serial Peripheral Interface 0
  irq_handler_ssc,               // 22 - Synchronous Serial Controller
  irq_handler_tc0,               // 23 - Timer/Counter 0
  irq_handler_tc1,               // 24 - Timer/Counter 1
  irq_handler_tc2,               // 25 - Timer/Counter 2
  irq_handler_tc3,               // 26 - Timer/Counter 3
  irq_handler_tc4,               // 27 - Timer/Counter 4
  irq_handler_tc5,               // 28 - Timer/Counter 5
  irq_handler_afec0,             // 29 - Analog Front End 0
  irq_handler_dacc,              // 30 - Digital To Analog Converter
  irq_handler_pwm0,              // 31 - Pulse Width Modulation 0
  irq_handler_icm,               // 32 - Integrity Check Monitor
  irq_handler_acc,               // 33 - Analog Comparator
  irq_handler_usbhs,             // 34 - USB Host / Device Controller
  irq_handler_mcan0,             // 35 - Controller Area Network Controller 0
  0,                             // 36 - Reserved
  irq_handler_mcan1,             // 37 - Controller Area Network Controller 1
  0,                             // 38 - Reserved
  irq_handler_gmac,              // 39 - Ethernet MAC
  irq_handler_afec1,             // 40 - Analog Front End 1
  irq_handler_twihs2,            // 41 - Two-wire Interface 2
  irq_handler_spi1,              // 42 - Serial Peripheral Interface 1
  irq_handler_qspi,              // 43 - Quad I/O Serial Peripheral Interface
  irq_handler_uart2,             // 44 - UART 2
  irq_handler_uart3,             // 45 - UART 3
  irq_handler_uart4,             // 46 - UART 4
  irq_handler_tc6,               // 47 - Timer/Counter 6
  irq_handler_tc7,               // 48 - Timer/Counter 7
  irq_handler_tc8,               // 49 - Timer/Counter 8
  irq_handler_tc9,               // 50 - Timer/Counter 9
  irq_handler_tc10,              // 51 - Timer/Counter 10
  irq_handler_tc11,              // 52 - Timer/Counter 11
  0,                             // 53 - Reserved
  0,                             // 54 - Reserved
  0,                             // 55 - Reserved
  irq_handler_aes,               // 56 - Advanced Encryption Standard (AES) Controller
  irq_handler_trng,              // 57 - True Random Number Generator
  irq_handler_xdmac,             // 58 - DMA Controller
  irq_handler_isi,               // 59 - Image Sensor Interface Controller
  irq_handler_pwm1,              // 60 - Pulse Width Modulation 1
  0,                             // 61 - Reserved
  irq_handler_sdramc,            // 62 - SDRAM Controller
  irq_handler_rswdt,             // 63 - Reinforced Secure Watchdog Timer
};

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void irq_handler_reset(void)
{
  unsigned int *src, *dst;

  src = &_etext;
  dst = &_data;
  while (dst < &_edata)
    *dst++ = *src++;

  dst = &_bss;
  while (dst < &_ebss)
    *dst++ = 0;

  // Enable FPU
  SCB->CPACR |=  (0xf << 20);
  __DSB();
  __ISB();

  SCB->VTOR = (uint32_t)vectors;

  main();

  while (1);
}

//-----------------------------------------------------------------------------
void irq_handler_dummy(void)
{
  while (1);
}

