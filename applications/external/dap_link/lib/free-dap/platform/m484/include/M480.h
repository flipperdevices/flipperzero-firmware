/**************************************************************************//**
 * @file     M480.h
 * @version  V1.00
 * @brief    M480 peripheral access layer header file.
 *           This file contains all the peripheral register's definitions,
 *           bits definitions and memory mapping for NuMicro M480 MCU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/**
  \mainpage NuMicro M480 Driver Reference Guide
  *
  * <b>Introduction</b>
  *
  * This user manual describes the usage of M480 Series MCU device driver
  *
  * <b>Disclaimer</b>
  *
  * The Software is furnished "AS IS", without warranty as to performance or results, and
  * the entire risk as to performance or results is assumed by YOU. Nuvoton disclaims all
  * warranties, express, implied or otherwise, with regard to the Software, its use, or
  * operation, including without limitation any and all warranties of merchantability, fitness
  * for a particular purpose, and non-infringement of intellectual property rights.
  *
  * <b>Important Notice</b>
  *
  * Nuvoton Products are neither intended nor warranted for usage in systems or equipment,
  * any malfunction or failure of which may cause loss of human life, bodily injury or severe
  * property damage. Such applications are deemed, "Insecure Usage".
  *
  * Insecure usage includes, but is not limited to: equipment for surgical implementation,
  * atomic energy control instruments, airplane or spaceship instruments, the control or
  * operation of dynamic, brake or safety systems designed for vehicular use, traffic signal
  * instruments, all types of safety devices, and other applications intended to support or
  * sustain life.
  *
  * All Insecure Usage shall be made at customer's risk, and in the event that third parties
  * lay claims to Nuvoton as a result of customer's Insecure Usage, customer shall indemnify
  * the damages and liabilities thus incurred by Nuvoton.
  *
  * Please note that all data and specifications are subject to change without notice. All the
  * trademarks of products and companies mentioned in this datasheet belong to their respective
  * owners.
  *
  * <b>Copyright Notice</b>
  *
  * Copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
  */
#ifndef __M480_H__
#define __M480_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SET_FIELD(p, r, f, v) p->r = (p->r & ~p##_##r##_##f##_Msk) | ((v) << p##_##r##_##f##_Pos)

/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup CMSIS_Device Device CMSIS Definitions
  Configuration of the Cortex-M4 Processor and Core Peripherals
  @{
*/

/**
 * @details  Interrupt Number Definition.
 */
typedef enum IRQn
{
    /******  Cortex-M4 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt                        */
    MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt                   */
    BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt                           */
    UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt                         */
    SVCall_IRQn                   = -5,       /*!< 11 SV Call Interrupt                             */
    DebugMonitor_IRQn             = -4,       /*!< 12 Debug Monitor Interrupt                       */
    PendSV_IRQn                   = -2,       /*!< 14 Pend SV Interrupt                             */
    SysTick_IRQn                  = -1,       /*!< 15 System Tick Interrupt                         */

    /******  M480 Specific Interrupt Numbers ********************************************************/

    BOD_IRQn                      = 0,        /*!< Brown Out detection Interrupt                    */
    IRC_IRQn                      = 1,        /*!< Internal RC Interrupt                            */
    PWRWU_IRQn                    = 2,        /*!< Power Down Wake Up Interrupt                     */
    RAMPE_IRQn                    = 3,        /*!< SRAM parity check failed Interrupt               */
    CKFAIL_IRQn                   = 4,        /*!< Clock failed Interrupt                           */
    RTC_IRQn                      = 6,        /*!< Real Time Clock Interrupt                        */
    TAMPER_IRQn                   = 7,        /*!< Tamper detection Interrupt                       */
    WDT_IRQn                      = 8,        /*!< Watchdog timer Interrupt                         */
    WWDT_IRQn                     = 9,        /*!< Window Watchdog timer Interrupt                  */
    EINT0_IRQn                    = 10,       /*!< External Input 0 Interrupt                       */
    EINT1_IRQn                    = 11,       /*!< External Input 1 Interrupt                       */
    EINT2_IRQn                    = 12,       /*!< External Input 2 Interrupt                       */
    EINT3_IRQn                    = 13,       /*!< External Input 3 Interrupt                       */
    EINT4_IRQn                    = 14,       /*!< External Input 4 Interrupt                       */
    EINT5_IRQn                    = 15,       /*!< External Input 5 Interrupt                       */
    GPA_IRQn                      = 16,       /*!< GPIO Port A Interrupt                            */
    GPB_IRQn                      = 17,       /*!< GPIO Port B Interrupt                            */
    GPC_IRQn                      = 18,       /*!< GPIO Port C Interrupt                            */
    GPD_IRQn                      = 19,       /*!< GPIO Port D Interrupt                            */
    GPE_IRQn                      = 20,       /*!< GPIO Port E Interrupt                            */
    GPF_IRQn                      = 21,       /*!< GPIO Port F Interrupt                            */
    QSPI0_IRQn                    = 22,       /*!< QSPI0 Interrupt                                   */
    SPI0_IRQn                     = 23,       /*!< SPI0 Interrupt                                   */
    BRAKE0_IRQn                   = 24,       /*!< BRAKE0 Interrupt                                 */
    EPWM0P0_IRQn                  = 25,       /*!< EPWM0P0 Interrupt                                */
    EPWM0P1_IRQn                  = 26,       /*!< EPWM0P1 Interrupt                                */
    EPWM0P2_IRQn                  = 27,       /*!< EPWM0P2 Interrupt                                */
    BRAKE1_IRQn                   = 28,       /*!< BRAKE1 Interrupt                                 */
    EPWM1P0_IRQn                  = 29,       /*!< EPWM1P0 Interrupt                                */
    EPWM1P1_IRQn                  = 30,       /*!< EPWM1P1 Interrupt                                */
    EPWM1P2_IRQn                  = 31,       /*!< EPWM1P2 Interrupt                                */
    TMR0_IRQn                     = 32,       /*!< Timer 0 Interrupt                                */
    TMR1_IRQn                     = 33,       /*!< Timer 1 Interrupt                                */
    TMR2_IRQn                     = 34,       /*!< Timer 2 Interrupt                                */
    TMR3_IRQn                     = 35,       /*!< Timer 3 Interrupt                                */
    UART0_IRQn                    = 36,       /*!< UART 0 Interrupt                                 */
    UART1_IRQn                    = 37,       /*!< UART 1 Interrupt                                 */
    I2C0_IRQn                     = 38,       /*!< I2C 0 Interrupt                                  */
    I2C1_IRQn                     = 39,       /*!< I2C 1 Interrupt                                  */
    PDMA_IRQn                     = 40,       /*!< Peripheral DMA Interrupt                         */
    DAC_IRQn                      = 41,       /*!< DAC Interrupt                                    */
    EADC00_IRQn                   = 42,       /*!< EADC00 Interrupt                                 */
    EADC01_IRQn                   = 43,       /*!< EADC01 Interrupt                                 */
    ACMP01_IRQn                   = 44,       /*!< Analog Comparator 0 and 1 Interrupt              */
    EADC02_IRQn                   = 46,       /*!< EADC02 Interrupt                                 */
    EADC03_IRQn                   = 47,       /*!< EADC03 Interrupt                                 */
    UART2_IRQn                    = 48,       /*!< UART2 Interrupt                                  */
    UART3_IRQn                    = 49,       /*!< UART3 Interrupt                                  */
    QSPI1_IRQn                    = 50,       /*!< QSPI1 Interrupt                                   */
    SPI1_IRQn                     = 51,       /*!< SPI1 Interrupt                                   */
    SPI2_IRQn                     = 52,       /*!< SPI2 Interrupt                                   */
    USBD_IRQn                     = 53,       /*!< USB device Interrupt                             */
    USBH_IRQn                     = 54,       /*!< USB host Interrupt                               */
    USBOTG_IRQn                   = 55,       /*!< USB OTG Interrupt                                */
    CAN0_IRQn                     = 56,       /*!< CAN0 Interrupt                                   */
    CAN1_IRQn                     = 57,       /*!< CAN1 Interrupt                                   */
    SC0_IRQn                      = 58,       /*!< Smart Card 0 Interrupt                           */
    SC1_IRQn                      = 59,       /*!< Smart Card 1 Interrupt                           */
    SC2_IRQn                      = 60,       /*!< Smart Card 2 Interrupt                           */
    SPI3_IRQn                     = 62,       /*!< SPI3 Interrupt                                   */
    EMAC_TX_IRQn                  = 66,       /*!< Ethernet MAC TX Interrupt                        */
    EMAC_RX_IRQn                  = 67,       /*!< Ethernet MAC RX Interrupt                        */
    SDH0_IRQn                     = 64,       /*!< Secure Digital Host Controller 0 Interrupt       */
    USBD20_IRQn                   = 65,       /*!< High Speed USB device Interrupt                  */
    I2S0_IRQn                     = 68,       /*!< I2S0 Interrupt                                   */
    OPA_IRQn                      = 70,       /*!< OPA Interrupt                                    */
    CRPT_IRQn                     = 71,       /*!< CRPT Interrupt                                   */
    GPG_IRQn                      = 72,       /*!< GPIO Port G Interrupt                            */
    EINT6_IRQn                    = 73,       /*!< External Input 6 Interrupt                       */
    UART4_IRQn                    = 74,       /*!< UART4 Interrupt                                  */
    UART5_IRQn                    = 75,       /*!< UART5 Interrupt                                  */
    USCI0_IRQn                    = 76,       /*!< USCI0 Interrupt                                  */
    USCI1_IRQn                    = 77,       /*!< USCI1 Interrupt                                  */
    BPWM0_IRQn                    = 78,       /*!< BPWM0 Interrupt                                  */
    BPWM1_IRQn                    = 79,       /*!< BPWM1 Interrupt                                  */
    SPIM_IRQn                     = 80,       /*!< SPIM Interrupt                                   */
    CCAP_IRQn                     = 81,       /*!< CCAP Interrupt                                   */
    I2C2_IRQn                     = 82,       /*!< I2C2 Interrupt                                   */
    QEI0_IRQn                     = 84,       /*!< QEI0 Interrupt                                   */
    QEI1_IRQn                     = 85,       /*!< QEI1 Interrupt                                   */
    ECAP0_IRQn                    = 86,       /*!< ECAP0 Interrupt                                  */
    ECAP1_IRQn                    = 87,       /*!< ECAP1 Interrupt                                  */
    GPH_IRQn                      = 88,       /*!< GPIO Port H Interrupt                            */
    EINT7_IRQn                    = 89,       /*!< External Input 7 Interrupt                       */
    SDH1_IRQn                     = 90,       /*!< Secure Digital Host Controller 1 Interrupt       */
    HSUSBH_IRQn                   = 92,       /*!< High speed USB host Interrupt                    */
    USBOTG20_IRQn                 = 93,       /*!< High speed USB OTG Interrupt                     */
    TRNG_IRQn                     = 101,      /*!< TRNG Interrupt                                   */
    UART6_IRQn                    = 102,      /*!< UART6 Interrupt                                  */
    UART7_IRQn                    = 103,      /*!< UART7 Interrupt                                  */
    EADC10_IRQn                   = 104,      /*!< EADC10 Interrupt                                 */
    EADC11_IRQn                   = 105,      /*!< EADC11 Interrupt                                 */
    EADC12_IRQn                   = 106,      /*!< EADC12 Interrupt                                 */
    EADC13_IRQn                   = 107,      /*!< EADC13 Interrupt                                 */
    CAN2_IRQn                     = 108,      /*!< CAN2 Interrupt                                   */
}
IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M4 Processor and Core Peripherals */
#define __CM4_REV                 0x0201UL    /*!< Core Revision r2p1                               */
#define __NVIC_PRIO_BITS          4UL         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0UL         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             1UL         /*!< MPU present or not                               */
#ifdef __FPU_PRESENT
#undef __FPU_PRESENT
#define __FPU_PRESENT             1UL         /*!< FPU present or not                               */
#else
#define __FPU_PRESENT             1UL         /*!< FPU present or not                               */
#endif

/*@}*/ /* end of group CMSIS_Device */


#include "core_cm4.h"               /* Cortex-M4 processor and core peripherals           */
#include "system_M480.h"            /* System include file                         */
#include <stdint.h>



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/******************************************************************************/
/*                            Register definitions                            */
/******************************************************************************/

#include "sys_reg.h"
#include "clk_reg.h"
#include "fmc_reg.h"
#include "gpio_reg.h"
#include "pdma_reg.h"
#include "timer_reg.h"
#include "wdt_reg.h"
#include "wwdt_reg.h"
#include "rtc_reg.h"
#include "epwm_reg.h"
#include "bpwm_reg.h"
#include "qei_reg.h"
#include "ecap_reg.h"
#include "uart_reg.h"
#include "emac_reg.h"
#include "sc_reg.h"
#include "i2s_reg.h"
#include "spi_reg.h"
#include "qspi_reg.h"
#include "spim_reg.h"
#include "i2c_reg.h"
#include "uuart_reg.h"
#include "uspi_reg.h"
#include "ui2c_reg.h"
#include "can_reg.h"
#include "sdh_reg.h"
#include "ebi_reg.h"
#include "usbd_reg.h"
#include "hsusbd_reg.h"
#include "usbh_reg.h"
#include "hsusbh_reg.h"
#include "otg_reg.h"
#include "hsotg_reg.h"
#include "crc_reg.h"
#include "crypto_reg.h"
#include "trng_reg.h"
#include "eadc_reg.h"
#include "dac_reg.h"
#include "acmp_reg.h"
#include "opa_reg.h"
#include "ccap_reg.h"


/** @addtogroup PERIPHERAL_MEM_MAP Peripheral Memory Base
  Memory Mapped Structure for Peripherals
  @{
 */
/* Peripheral and SRAM base address */
#define FLASH_BASE           ((uint32_t)0x00000000)      /*!< Flash base address      */
#define SRAM_BASE            ((uint32_t)0x20000000)      /*!< SRAM Base Address       */
#define PERIPH_BASE          ((uint32_t)0x40000000)      /*!< Peripheral Base Address */
#define AHBPERIPH_BASE       PERIPH_BASE                 /*!< AHB Base Address */
#define APBPERIPH_BASE       (PERIPH_BASE + (uint32_t)0x00040000)  /*!< APB Base Address */

/*!< AHB peripherals */
#define SYS_BASE               (AHBPERIPH_BASE + 0x00000UL)
#define CLK_BASE               (AHBPERIPH_BASE + 0x00200UL)
#define NMI_BASE               (AHBPERIPH_BASE + 0x00300UL)
#define GPIOA_BASE             (AHBPERIPH_BASE + 0x04000UL)
#define GPIOB_BASE             (AHBPERIPH_BASE + 0x04040UL)
#define GPIOC_BASE             (AHBPERIPH_BASE + 0x04080UL)
#define GPIOD_BASE             (AHBPERIPH_BASE + 0x040C0UL)
#define GPIOE_BASE             (AHBPERIPH_BASE + 0x04100UL)
#define GPIOF_BASE             (AHBPERIPH_BASE + 0x04140UL)
#define GPIOG_BASE             (AHBPERIPH_BASE + 0x04180UL)
#define GPIOH_BASE             (AHBPERIPH_BASE + 0x041C0UL)
#define GPIOI_BASE             (AHBPERIPH_BASE + 0x04200UL)
#define GPIO_DBCTL_BASE        (AHBPERIPH_BASE + 0x04440UL)
#define GPIO_PIN_DATA_BASE     (AHBPERIPH_BASE + 0x04800UL)
#define PDMA_BASE              (AHBPERIPH_BASE + 0x08000UL)
#define USBH_BASE              (AHBPERIPH_BASE + 0x09000UL)
#define HSUSBH_BASE            (AHBPERIPH_BASE + 0x1A000UL)
#define EMAC_BASE              (AHBPERIPH_BASE + 0x0B000UL)
#define FMC_BASE               (AHBPERIPH_BASE + 0x0C000UL)
#define SDH0_BASE              (AHBPERIPH_BASE + 0x0D000UL)
#define SDH1_BASE              (AHBPERIPH_BASE + 0x0E000UL)
#define EBI_BASE               (AHBPERIPH_BASE + 0x10000UL)
#define HSUSBD_BASE            (AHBPERIPH_BASE + 0x19000UL)
#define CCAP_BASE              (AHBPERIPH_BASE + 0x30000UL)
#define CRC_BASE               (AHBPERIPH_BASE + 0x31000UL)
#define TAMPER_BASE            (AHBPERIPH_BASE + 0xE1000UL)

/*!< APB2 peripherals */
#define WDT_BASE              (APBPERIPH_BASE + 0x00000UL)
#define WWDT_BASE             (APBPERIPH_BASE + 0x00100UL)
#define OPA_BASE              (APBPERIPH_BASE + 0x06000UL)
#define I2S_BASE              (APBPERIPH_BASE + 0x08000UL)
#define EADC1_BASE            (APBPERIPH_BASE + 0x0B000UL)
#define TIMER0_BASE           (APBPERIPH_BASE + 0x10000UL)
#define TIMER1_BASE           (APBPERIPH_BASE + 0x10100UL)
#define EPWM0_BASE            (APBPERIPH_BASE + 0x18000UL)
#define BPWM0_BASE            (APBPERIPH_BASE + 0x1A000UL)
#define QSPI0_BASE            (APBPERIPH_BASE + 0x20000UL)
#define SPI1_BASE             (APBPERIPH_BASE + 0x22000UL)
#define SPI3_BASE             (APBPERIPH_BASE + 0x24000UL)
#define UART0_BASE            (APBPERIPH_BASE + 0x30000UL)
#define UART2_BASE            (APBPERIPH_BASE + 0x32000UL)
#define UART4_BASE            (APBPERIPH_BASE + 0x34000UL)
#define UART6_BASE            (APBPERIPH_BASE + 0x36000UL)
#define I2C0_BASE             (APBPERIPH_BASE + 0x40000UL)
#define I2C2_BASE             (APBPERIPH_BASE + 0x42000UL)
#define CAN0_BASE             (APBPERIPH_BASE + 0x60000UL)
#define CAN2_BASE             (APBPERIPH_BASE + 0x62000UL)
#define QEI0_BASE             (APBPERIPH_BASE + 0x70000UL)
#define ECAP0_BASE            (APBPERIPH_BASE + 0x74000UL)
#define USCI0_BASE            (APBPERIPH_BASE + 0x90000UL)


/*!< APB1 peripherals */
#define RTC_BASE              (APBPERIPH_BASE + 0x01000UL)
#define EADC_BASE             (APBPERIPH_BASE + 0x03000UL)
#define ACMP01_BASE           (APBPERIPH_BASE + 0x05000UL)
#define USBD_BASE             (APBPERIPH_BASE + 0x80000UL)
#define OTG_BASE              (APBPERIPH_BASE + 0x0D000UL)
#define HSOTG_BASE            (APBPERIPH_BASE + 0x0F000UL)
#define TIMER2_BASE           (APBPERIPH_BASE + 0x11000UL)
#define TIMER3_BASE           (APBPERIPH_BASE + 0x11100UL)
#define EPWM1_BASE            (APBPERIPH_BASE + 0x19000UL)
#define BPWM1_BASE            (APBPERIPH_BASE + 0x1B000UL)
#define SPI0_BASE             (APBPERIPH_BASE + 0x21000UL)
#define SPI2_BASE             (APBPERIPH_BASE + 0x23000UL)
#define QSPI1_BASE            (APBPERIPH_BASE + 0x29000UL)
#define UART1_BASE            (APBPERIPH_BASE + 0x31000UL)
#define UART3_BASE            (APBPERIPH_BASE + 0x33000UL)
#define UART5_BASE            (APBPERIPH_BASE + 0x35000UL)
#define UART7_BASE            (APBPERIPH_BASE + 0x37000UL)
#define I2C1_BASE             (APBPERIPH_BASE + 0x41000UL)
#define CAN1_BASE             (APBPERIPH_BASE + 0x61000UL)
#define QEI1_BASE             (APBPERIPH_BASE + 0x71000UL)
#define ECAP1_BASE            (APBPERIPH_BASE + 0x75000UL)
#define TRNG_BASE             (APBPERIPH_BASE + 0x79000UL)
#define USCI1_BASE            (APBPERIPH_BASE + 0x91000UL)
#define CRPT_BASE             (0x50080000UL)
#define SPIM_BASE             (0x40007000UL)

#define SC0_BASE             (APBPERIPH_BASE + 0x50000UL)
#define SC1_BASE             (APBPERIPH_BASE + 0x51000UL)
#define SC2_BASE             (APBPERIPH_BASE + 0x52000UL)
#define DAC0_BASE            (APBPERIPH_BASE + 0x07000UL)
#define DAC1_BASE            (APBPERIPH_BASE + 0x07040UL)
#define DACDBG_BASE          (APBPERIPH_BASE + 0x07FECUL)
#define OPA0_BASE            (APBPERIPH_BASE + 0x06000UL)

/*@}*/ /* end of group PERIPHERAL_MEM_MAP */


/** @addtogroup PERIPHERAL_DECLARATION Peripheral Pointer
  The Declaration of Peripherals
  @{
 */

#define SYS                  ((SYS_T *)   SYS_BASE)
#define CLK                  ((CLK_T *)   CLK_BASE)
#define NMI                  ((NMI_T *)   NMI_BASE)
#define PA                   ((GPIO_T *)  GPIOA_BASE)
#define PB                   ((GPIO_T *)  GPIOB_BASE)
#define PC                   ((GPIO_T *)  GPIOC_BASE)
#define PD                   ((GPIO_T *)  GPIOD_BASE)
#define PE                   ((GPIO_T *)  GPIOE_BASE)
#define PF                   ((GPIO_T *)  GPIOF_BASE)
#define PG                   ((GPIO_T *)  GPIOG_BASE)
#define PH                   ((GPIO_T *)  GPIOH_BASE)
#define GPA                  ((GPIO_T *)  GPIOA_BASE)
#define GPB                  ((GPIO_T *)  GPIOB_BASE)
#define GPC                  ((GPIO_T *)  GPIOC_BASE)
#define GPD                  ((GPIO_T *)  GPIOD_BASE)
#define GPE                  ((GPIO_T *)  GPIOE_BASE)
#define GPF                  ((GPIO_T *)  GPIOF_BASE)
#define GPG                  ((GPIO_T *)  GPIOG_BASE)
#define GPH                  ((GPIO_T *)  GPIOH_BASE)
#define GPIO                 ((GPIO_DBCTL_T *) GPIO_DBCTL_BASE)
#define PDMA                 ((PDMA_T *)  PDMA_BASE)
#define USBH                 ((USBH_T *)  USBH_BASE)
#define HSUSBH               ((HSUSBH_T *)  HSUSBH_BASE)
#define EMAC                 ((EMAC_T *)  EMAC_BASE)
#define FMC                  ((FMC_T *)   FMC_BASE)
#define SDH0                 ((SDH_T *)   SDH0_BASE)
#define SDH1                 ((SDH_T *)   SDH1_BASE)
#define EBI                  ((EBI_T *)   EBI_BASE)
#define CRC                  ((CRC_T *)   CRC_BASE)
#define TAMPER               ((TAMPER_T *) TAMPER_BASE)

#define WDT                  ((WDT_T *)   WDT_BASE)
#define WWDT                 ((WWDT_T *)  WWDT_BASE)
#define RTC                  ((RTC_T *)   RTC_BASE)
#define EADC                 ((EADC_T *)  EADC_BASE)
#define EADC0                ((EADC_T *)  EADC_BASE)
#define EADC1                ((EADC_T *)  EADC1_BASE)
#define ACMP01               ((ACMP_T *)  ACMP01_BASE)

#define I2S0                 ((I2S_T *)   I2S_BASE)
#define USBD                 ((USBD_T *)  USBD_BASE)
#define OTG                  ((OTG_T *)   OTG_BASE)
#define HSUSBD               ((HSUSBD_T *)HSUSBD_BASE)
#define HSOTG                ((HSOTG_T *) HSOTG_BASE)
#define TIMER0               ((TIMER_T *) TIMER0_BASE)
#define TIMER1               ((TIMER_T *) TIMER1_BASE)
#define TIMER2               ((TIMER_T *) TIMER2_BASE)
#define TIMER3               ((TIMER_T *) TIMER3_BASE)
#define EPWM0                ((EPWM_T *)  EPWM0_BASE)
#define EPWM1                ((EPWM_T *)  EPWM1_BASE)
#define BPWM0                ((BPWM_T *)  BPWM0_BASE)
#define BPWM1                ((BPWM_T *)  BPWM1_BASE)
#define ECAP0                ((ECAP_T *)  ECAP0_BASE)
#define ECAP1                ((ECAP_T *)  ECAP1_BASE)
#define QEI0                 ((QEI_T *)   QEI0_BASE)
#define QEI1                 ((QEI_T *)   QEI1_BASE)
#define QSPI0                ((QSPI_T *)  QSPI0_BASE)
#define QSPI1                ((QSPI_T *)  QSPI1_BASE)
#define SPI0                 ((SPI_T *)   SPI0_BASE)
#define SPI1                 ((SPI_T *)   SPI1_BASE)
#define SPI2                 ((SPI_T *)   SPI2_BASE)
#define SPI3                 ((SPI_T *)   SPI3_BASE)
#define UART0                ((UART_T *)  UART0_BASE)
#define UART1                ((UART_T *)  UART1_BASE)
#define UART2                ((UART_T *)  UART2_BASE)
#define UART3                ((UART_T *)  UART3_BASE)
#define UART4                ((UART_T *)  UART4_BASE)
#define UART5                ((UART_T *)  UART5_BASE)
#define UART6                ((UART_T *)  UART6_BASE)
#define UART7                ((UART_T *)  UART7_BASE)
#define I2C0                 ((I2C_T *)   I2C0_BASE)
#define I2C1                 ((I2C_T *)   I2C1_BASE)
#define I2C2                 ((I2C_T *)   I2C2_BASE)
#define SC0                  ((SC_T *)    SC0_BASE)
#define SC1                  ((SC_T *)    SC1_BASE)
#define SC2                  ((SC_T *)    SC2_BASE)
#define CAN0                 ((CAN_T *)   CAN0_BASE)
#define CAN1                 ((CAN_T *)   CAN1_BASE)
#define CAN2                 ((CAN_T *)   CAN2_BASE)
#define CRPT                 ((CRPT_T *)  CRPT_BASE)
#define TRNG                 ((TRNG_T *)  TRNG_BASE)
#define SPIM                 ((volatile SPIM_T *)  SPIM_BASE)
#define DAC0                 ((DAC_T *)   DAC0_BASE)
#define DAC1                 ((DAC_T *)   DAC1_BASE)
#define USPI0                ((USPI_T *) USCI0_BASE)                     /*!< USPI0 Configuration Struct                       */
#define USPI1                ((USPI_T *) USCI1_BASE)                     /*!< USPI1 Configuration Struct                       */
#define OPA                  ((OPA_T *) OPA_BASE)
#define UI2C0                ((UI2C_T *) USCI0_BASE)                     /*!< UI2C0 Configuration Struct                       */
#define UI2C1                ((UI2C_T *) USCI1_BASE)                     /*!< UI2C1 Configuration Struct                       */
#define UUART0               ((UUART_T *) USCI0_BASE)                    /*!< UUART0 Configuration Struct                      */
#define UUART1               ((UUART_T *) USCI1_BASE)                    /*!< UUART1 Configuration Struct                      */
#define CCAP                 ((CCAP_T *)  CCAP_BASE)

/*@}*/ /* end of group ERIPHERAL_DECLARATION */

/** @addtogroup IO_ROUTINE I/O Routines
  The Declaration of I/O Routines
  @{
 */

typedef volatile unsigned char  vu8;        ///< Define 8-bit unsigned volatile data type
typedef volatile unsigned short vu16;       ///< Define 16-bit unsigned volatile data type
typedef volatile unsigned long  vu32;       ///< Define 32-bit unsigned volatile data type

/**
  * @brief Get a 8-bit unsigned value from specified address
  * @param[in] addr Address to get 8-bit data from
  * @return  8-bit unsigned value stored in specified address
  */
#define M8(addr)  (*((vu8  *) (addr)))

/**
  * @brief Get a 16-bit unsigned value from specified address
  * @param[in] addr Address to get 16-bit data from
  * @return  16-bit unsigned value stored in specified address
  * @note The input address must be 16-bit aligned
  */
#define M16(addr) (*((vu16 *) (addr)))

/**
  * @brief Get a 32-bit unsigned value from specified address
  * @param[in] addr Address to get 32-bit data from
  * @return  32-bit unsigned value stored in specified address
  * @note The input address must be 32-bit aligned
  */
#define M32(addr) (*((vu32 *) (addr)))

/**
  * @brief Set a 32-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 32-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 32-bit aligned
  */
#define outpw(port,value)     *((volatile unsigned int *)(port)) = (value)

/**
  * @brief Get a 32-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 32-bit data from
  * @return  32-bit unsigned value stored in specified I/O port
  * @note The input port must be 32-bit aligned
  */
#define inpw(port)            (*((volatile unsigned int *)(port)))

/**
  * @brief Set a 16-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 16-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 16-bit aligned
  */
#define outps(port,value)     *((volatile unsigned short *)(port)) = (value)

/**
  * @brief Get a 16-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 16-bit data from
  * @return  16-bit unsigned value stored in specified I/O port
  * @note The input port must be 16-bit aligned
  */
#define inps(port)            (*((volatile unsigned short *)(port)))

/**
  * @brief Set a 8-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 8-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  */
#define outpb(port,value)     *((volatile unsigned char *)(port)) = (value)

/**
  * @brief Get a 8-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 8-bit data from
  * @return  8-bit unsigned value stored in specified I/O port
  */
#define inpb(port)            (*((volatile unsigned char *)(port)))

/**
  * @brief Set a 32-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 32-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 32-bit aligned
  */
#define outp32(port,value)    *((volatile unsigned int *)(port)) = (value)

/**
  * @brief Get a 32-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 32-bit data from
  * @return  32-bit unsigned value stored in specified I/O port
  * @note The input port must be 32-bit aligned
  */
#define inp32(port)           (*((volatile unsigned int *)(port)))

/**
  * @brief Set a 16-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 16-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 16-bit aligned
  */
#define outp16(port,value)    *((volatile unsigned short *)(port)) = (value)

/**
  * @brief Get a 16-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 16-bit data from
  * @return  16-bit unsigned value stored in specified I/O port
  * @note The input port must be 16-bit aligned
  */
#define inp16(port)           (*((volatile unsigned short *)(port)))

/**
  * @brief Set a 8-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 8-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  */
#define outp8(port,value)     *((volatile unsigned char *)(port)) = (value)

/**
  * @brief Get a 8-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 8-bit data from
  * @return  8-bit unsigned value stored in specified I/O port
  */
#define inp8(port)            (*((volatile unsigned char *)(port)))


/*@}*/ /* end of group IO_ROUTINE */

/******************************************************************************/
/*                Legacy Constants                                            */
/******************************************************************************/
/** @addtogroup Legacy_Constants Legacy Constants
  Legacy Constants
  @{
*/

#ifndef NULL
#define NULL           (0)      ///< NULL pointer
#endif

#define TRUE           (1UL)      ///< Boolean true, define to use in API parameters or return value
#define FALSE          (0UL)      ///< Boolean false, define to use in API parameters or return value

#define ENABLE         (1UL)      ///< Enable, define to use in API parameters
#define DISABLE        (0UL)      ///< Disable, define to use in API parameters

/* Define one bit mask */
#define BIT0     (0x00000001UL)       ///< Bit 0 mask of an 32 bit integer
#define BIT1     (0x00000002UL)       ///< Bit 1 mask of an 32 bit integer
#define BIT2     (0x00000004UL)       ///< Bit 2 mask of an 32 bit integer
#define BIT3     (0x00000008UL)       ///< Bit 3 mask of an 32 bit integer
#define BIT4     (0x00000010UL)       ///< Bit 4 mask of an 32 bit integer
#define BIT5     (0x00000020UL)       ///< Bit 5 mask of an 32 bit integer
#define BIT6     (0x00000040UL)       ///< Bit 6 mask of an 32 bit integer
#define BIT7     (0x00000080UL)       ///< Bit 7 mask of an 32 bit integer
#define BIT8     (0x00000100UL)       ///< Bit 8 mask of an 32 bit integer
#define BIT9     (0x00000200UL)       ///< Bit 9 mask of an 32 bit integer
#define BIT10    (0x00000400UL)       ///< Bit 10 mask of an 32 bit integer
#define BIT11    (0x00000800UL)       ///< Bit 11 mask of an 32 bit integer
#define BIT12    (0x00001000UL)       ///< Bit 12 mask of an 32 bit integer
#define BIT13    (0x00002000UL)       ///< Bit 13 mask of an 32 bit integer
#define BIT14    (0x00004000UL)       ///< Bit 14 mask of an 32 bit integer
#define BIT15    (0x00008000UL)       ///< Bit 15 mask of an 32 bit integer
#define BIT16    (0x00010000UL)       ///< Bit 16 mask of an 32 bit integer
#define BIT17    (0x00020000UL)       ///< Bit 17 mask of an 32 bit integer
#define BIT18    (0x00040000UL)       ///< Bit 18 mask of an 32 bit integer
#define BIT19    (0x00080000UL)       ///< Bit 19 mask of an 32 bit integer
#define BIT20    (0x00100000UL)       ///< Bit 20 mask of an 32 bit integer
#define BIT21    (0x00200000UL)       ///< Bit 21 mask of an 32 bit integer
#define BIT22    (0x00400000UL)       ///< Bit 22 mask of an 32 bit integer
#define BIT23    (0x00800000UL)       ///< Bit 23 mask of an 32 bit integer
#define BIT24    (0x01000000UL)       ///< Bit 24 mask of an 32 bit integer
#define BIT25    (0x02000000UL)       ///< Bit 25 mask of an 32 bit integer
#define BIT26    (0x04000000UL)       ///< Bit 26 mask of an 32 bit integer
#define BIT27    (0x08000000UL)       ///< Bit 27 mask of an 32 bit integer
#define BIT28    (0x10000000UL)       ///< Bit 28 mask of an 32 bit integer
#define BIT29    (0x20000000UL)       ///< Bit 29 mask of an 32 bit integer
#define BIT30    (0x40000000UL)       ///< Bit 30 mask of an 32 bit integer
#define BIT31    (0x80000000UL)       ///< Bit 31 mask of an 32 bit integer

/* Byte Mask Definitions */
#define BYTE0_Msk              (0x000000FFUL)         ///< Mask to get bit0~bit7 from a 32 bit integer
#define BYTE1_Msk              (0x0000FF00UL)         ///< Mask to get bit8~bit15 from a 32 bit integer
#define BYTE2_Msk              (0x00FF0000UL)         ///< Mask to get bit16~bit23 from a 32 bit integer
#define BYTE3_Msk              (0xFF000000UL)         ///< Mask to get bit24~bit31 from a 32 bit integer

#define GET_BYTE0(u32Param)    (((u32Param) & BYTE0_Msk)      )  /*!< Extract Byte 0 (Bit  0~ 7) from parameter u32Param */
#define GET_BYTE1(u32Param)    (((u32Param) & BYTE1_Msk) >>  8)  /*!< Extract Byte 1 (Bit  8~15) from parameter u32Param */
#define GET_BYTE2(u32Param)    (((u32Param) & BYTE2_Msk) >> 16)  /*!< Extract Byte 2 (Bit 16~23) from parameter u32Param */
#define GET_BYTE3(u32Param)    (((u32Param) & BYTE3_Msk) >> 24)  /*!< Extract Byte 3 (Bit 24~31) from parameter u32Param */

/*@}*/ /* end of group Legacy_Constants */


/******************************************************************************/
/*                         Peripheral header files                            */
/******************************************************************************/
/*
#include "sys.h"
#include "clk.h"

#include "acmp.h"
#include "dac.h"
#include "emac.h"
#include "uart.h"
#include "usci_spi.h"
#include "gpio.h"
#include "ccap.h"
#include "ecap.h"
#include "qei.h"
#include "timer.h"
#include "timer_pwm.h"
#include "pdma.h"
#include "crypto.h"
#include "trng.h"
#include "fmc.h"
#include "spim.h"
#include "i2c.h"
#include "i2s.h"
#include "epwm.h"
#include "eadc.h"
#include "bpwm.h"
#include "wdt.h"
#include "wwdt.h"
#include "opa.h"
#include "crc.h"
#include "ebi.h"
#include "usci_i2c.h"
#include "scuart.h"
#include "sc.h"
#include "spi.h"
#include "qspi.h"
#include "can.h"
#include "rtc.h"
#include "usci_uart.h"
#include "sdh.h"
#include "usbd.h"
#include "hsusbd.h"
#include "otg.h"
#include "hsotg.h"
*/

#ifdef __cplusplus
}
#endif

#endif  /* __M480_H__ */

