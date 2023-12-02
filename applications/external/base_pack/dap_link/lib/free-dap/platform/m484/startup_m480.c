// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include "M480.h"

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

DUMMY void irq_handler_bod(void);
DUMMY void irq_handler_irc(void);
DUMMY void irq_handler_pwrwu(void);
DUMMY void irq_handler_rampe(void);
DUMMY void irq_handler_ckfail(void);
DUMMY void irq_handler_rtc(void);
DUMMY void irq_handler_tamper(void);
DUMMY void irq_handler_wdt(void);
DUMMY void irq_handler_wwdt(void);
DUMMY void irq_handler_eint0(void);
DUMMY void irq_handler_eint1(void);
DUMMY void irq_handler_eint2(void);
DUMMY void irq_handler_eint3(void);
DUMMY void irq_handler_eint4(void);
DUMMY void irq_handler_eint5(void);
DUMMY void irq_handler_gpa(void);
DUMMY void irq_handler_gpb(void);
DUMMY void irq_handler_gpc(void);
DUMMY void irq_handler_gpd(void);
DUMMY void irq_handler_gpe(void);
DUMMY void irq_handler_gpf(void);
DUMMY void irq_handler_qspi0(void);
DUMMY void irq_handler_spi0(void);
DUMMY void irq_handler_brake0(void);
DUMMY void irq_handler_epwm0p0(void);
DUMMY void irq_handler_epwm0p1(void);
DUMMY void irq_handler_epwm0p2(void);
DUMMY void irq_handler_brake1(void);
DUMMY void irq_handler_epwm1p0(void);
DUMMY void irq_handler_epwm1p1(void);
DUMMY void irq_handler_epwm1p2(void);
DUMMY void irq_handler_tmr0(void);
DUMMY void irq_handler_tmr1(void);
DUMMY void irq_handler_tmr2(void);
DUMMY void irq_handler_tmr3(void);
DUMMY void irq_handler_uart0(void);
DUMMY void irq_handler_uart1(void);
DUMMY void irq_handler_i2c0(void);
DUMMY void irq_handler_i2c1(void);
DUMMY void irq_handler_pdma(void);
DUMMY void irq_handler_dac(void);
DUMMY void irq_handler_eadc00(void);
DUMMY void irq_handler_eadc01(void);
DUMMY void irq_handler_acmp01(void);
DUMMY void irq_handler_eadc02(void);
DUMMY void irq_handler_eadc03(void);
DUMMY void irq_handler_uart2(void);
DUMMY void irq_handler_uart3(void);
DUMMY void irq_handler_qspi1(void);
DUMMY void irq_handler_spi1(void);
DUMMY void irq_handler_spi2(void);
DUMMY void irq_handler_usbd(void);
DUMMY void irq_handler_ohci(void);
DUMMY void irq_handler_usbotg(void);
DUMMY void irq_handler_can0(void);
DUMMY void irq_handler_can1(void);
DUMMY void irq_handler_sc0(void);
DUMMY void irq_handler_sc1(void);
DUMMY void irq_handler_sc2(void);
DUMMY void irq_handler_spi3(void);
DUMMY void irq_handler_sdh0(void);
DUMMY void irq_handler_usbd20(void);
DUMMY void irq_handler_emac_tx(void);
DUMMY void irq_handler_emac_rx(void);
DUMMY void irq_handler_i2s0(void);
DUMMY void irq_handler_opa0(void);
DUMMY void irq_handler_crypto(void);
DUMMY void irq_handler_gpg(void);
DUMMY void irq_handler_eint6(void);
DUMMY void irq_handler_uart4(void);
DUMMY void irq_handler_uart5(void);
DUMMY void irq_handler_usci0(void);
DUMMY void irq_handler_usci1(void);
DUMMY void irq_handler_bpwm0(void);
DUMMY void irq_handler_bpwm1(void);
DUMMY void irq_handler_spim(void);
DUMMY void irq_handler_ccap(void);
DUMMY void irq_handler_i2c2(void);
DUMMY void irq_handler_qei0(void);
DUMMY void irq_handler_qei1(void);
DUMMY void irq_handler_ecap0(void);
DUMMY void irq_handler_ecap1(void);
DUMMY void irq_handler_gph(void);
DUMMY void irq_handler_eint7(void);
DUMMY void irq_handler_sdh1(void);
DUMMY void irq_handler_ehci(void);
DUMMY void irq_handler_usbotg20(void);
DUMMY void irq_handler_trng(void);
DUMMY void irq_handler_uart6(void);
DUMMY void irq_handler_uart7(void);
DUMMY void irq_handler_eadc10(void);
DUMMY void irq_handler_eadc11(void);
DUMMY void irq_handler_eadc12(void);
DUMMY void irq_handler_eadc13(void);
DUMMY void irq_handler_can2(void);

int main(void);

extern void _stack_top(void);
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

/*- Variables ---------------------------------------------------------------*/

//-----------------------------------------------------------------------------
__attribute__ ((used, section(".rom_vectors")))
void (* const rom_vectors[])(void) =
{
  &_stack_top,
  irq_handler_reset,
};

//-----------------------------------------------------------------------------
__attribute__ ((used, section(".vectors")))
void (* const vectors[])(void) =
{
  &_stack_top,                   // 0 - Initial Stack Pointer Value

  // Cortex-M4 handlers
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
  irq_handler_bod,               // 0 - brown out detection
  irq_handler_irc,               // 1 - internal rc
  irq_handler_pwrwu,             // 2 - power down wake up
  irq_handler_rampe,             // 3 - ram parity error
  irq_handler_ckfail,            // 4 - clock detection fail
  0,                             // 5 - reserved
  irq_handler_rtc,               // 6 - real time clock
  irq_handler_tamper,            // 7 - tamper detection
  irq_handler_wdt,               // 8 - watchdog timer
  irq_handler_wwdt,              // 9 - window watchdog timer
  irq_handler_eint0,             // 10 - external input 0
  irq_handler_eint1,             // 11 - external input 1
  irq_handler_eint2,             // 12 - external input 2
  irq_handler_eint3,             // 13 - external input 3
  irq_handler_eint4,             // 14 - external input 4
  irq_handler_eint5,             // 15 - external input 5
  irq_handler_gpa,               // 16 - gpio port a
  irq_handler_gpb,               // 17 - gpio port b
  irq_handler_gpc,               // 18 - gpio port c
  irq_handler_gpd,               // 19 - gpio port d
  irq_handler_gpe,               // 20 - gpio port e
  irq_handler_gpf,               // 21 - gpio port f
  irq_handler_qspi0,             // 22 - qspi0
  irq_handler_spi0,              // 23 - spi0
  irq_handler_brake0,            // 24
  irq_handler_epwm0p0,           // 25
  irq_handler_epwm0p1,           // 26
  irq_handler_epwm0p2,           // 27
  irq_handler_brake1,            // 28
  irq_handler_epwm1p0,           // 29
  irq_handler_epwm1p1,           // 30
  irq_handler_epwm1p2,           // 31
  irq_handler_tmr0,              // 32 - timer 0
  irq_handler_tmr1,              // 33 - timer 1
  irq_handler_tmr2,              // 34 - timer 2
  irq_handler_tmr3,              // 35 - timer 3
  irq_handler_uart0,             // 36 - uart0
  irq_handler_uart1,             // 37 - uart1
  irq_handler_i2c0,              // 38 - i2c0
  irq_handler_i2c1,              // 39 - i2c1
  irq_handler_pdma,              // 40 - peripheral dma
  irq_handler_dac,               // 41 - dac
  irq_handler_eadc00,            // 42 - eadc0 interrupt source 0
  irq_handler_eadc01,            // 43 - eadc0 interrupt source 1
  irq_handler_acmp01,            // 44 - acmp0 and acmp1
  0,                             // 45 - reserved
  irq_handler_eadc02,            // 46 - eadc0 interrupt source 2
  irq_handler_eadc03,            // 47 - eadc0 interrupt source 3
  irq_handler_uart2,             // 48 - uart2
  irq_handler_uart3,             // 49 - uart3
  irq_handler_qspi1,             // 50 - qspi1
  irq_handler_spi1,              // 51 - spi1
  irq_handler_spi2,              // 52 - spi2
  irq_handler_usbd,              // 53 - usb device
  irq_handler_ohci,              // 54 - ohci
  irq_handler_usbotg,            // 55 - usb otg
  irq_handler_can0,              // 56 - can0
  irq_handler_can1,              // 57 - can1
  irq_handler_sc0,               // 58
  irq_handler_sc1,               // 59
  irq_handler_sc2,               // 60
  0,                             // 61 - reserved
  irq_handler_spi3,              // 62 - spi3
  0,                             // 63 - reserved
  irq_handler_sdh0,              // 64 - sdh0
  irq_handler_usbd20,            // 65 - usbd20
  irq_handler_emac_tx,           // 66 - emac_tx
  irq_handler_emac_rx,           // 67 - emax_rx
  irq_handler_i2s0,              // 68 - i2s0
  0,                             // 69 - reserved
  irq_handler_opa0,              // 70 - opa0
  irq_handler_crypto,            // 71 - crypto
  irq_handler_gpg,               // 72
  irq_handler_eint6,             // 73
  irq_handler_uart4,             // 74 - uart4
  irq_handler_uart5,             // 75 - uart5
  irq_handler_usci0,             // 76 - usci0
  irq_handler_usci1,             // 77 - usci1
  irq_handler_bpwm0,             // 78 - bpwm0
  irq_handler_bpwm1,             // 79 - bpwm1
  irq_handler_spim,              // 80 - spim
  irq_handler_ccap,              // 81 - ccap
  irq_handler_i2c2,              // 82 - i2c2
  0,                             // 83 - reserved
  irq_handler_qei0,              // 84 - qei0
  irq_handler_qei1,              // 85 - qei1
  irq_handler_ecap0,             // 86 - ecap0
  irq_handler_ecap1,             // 87 - ecap1
  irq_handler_gph,               // 88
  irq_handler_eint7,             // 89
  irq_handler_sdh1,              // 90 - sdh1
  0,                             // 91 - reserved
  irq_handler_ehci,              // 92 - ehci
  irq_handler_usbotg20,          // 93
  0,                             // 94 - reserved
  0,                             // 95 - reserved
  0,                             // 96 - reserved
  0,                             // 97 - reserved
  0,                             // 98 - reserved
  0,                             // 99 - reserved
  0,                             // 100 - reserved
  irq_handler_trng,              // 101 - trng
  irq_handler_uart6,             // 102 - uart6
  irq_handler_uart7,             // 103 - uart7
  irq_handler_eadc10,            // 104 - eadc1 interrupt source 0
  irq_handler_eadc11,            // 105 - eadc1 interrupt source 1
  irq_handler_eadc12,            // 106 - eadc1 interrupt source 2
  irq_handler_eadc13,            // 107 - eadc1 interrupt source 3
  irq_handler_can2,              // 108 - can2
};

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
__attribute__ ((used, section(".reset_handler"), optimize("-fno-tree-loop-distribute-patterns")))
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

