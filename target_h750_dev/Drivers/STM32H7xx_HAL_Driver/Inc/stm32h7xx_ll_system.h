/**
  ******************************************************************************
  * @file    stm32h7xx_ll_system.h
  * @author  MCD Application Team
  * @brief   Header file of SYSTEM LL module.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The LL SYSTEM driver contains a set of generic APIs that can be
    used by user:
      (+) Some of the FLASH features need to be handled in the SYSTEM file.
      (+) Access to DBGCMU registers
      (+) Access to SYSCFG registers

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32H7xx_LL_SYSTEM_H
#define __STM32H7xx_LL_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (FLASH) || defined (SYSCFG) || defined (DBGMCU)

/** @defgroup SYSTEM_LL SYSTEM
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Private_Constants SYSTEM Private Constants
  * @{
  */
/** @defgroup SYSTEM_LL_EC_FLASH_BANK1_SECTORS SYSCFG Flash Bank1 sectors bits status
  * @{
  */
#define LL_SYSCFG_FLASH_B1_SECTOR0_STATUS_BIT   0x10000U
#define LL_SYSCFG_FLASH_B1_SECTOR1_STATUS_BIT   0x20000U
#define LL_SYSCFG_FLASH_B1_SECTOR2_STATUS_BIT   0x40000U
#define LL_SYSCFG_FLASH_B1_SECTOR3_STATUS_BIT   0x80000U
#define LL_SYSCFG_FLASH_B1_SECTOR4_STATUS_BIT   0x100000U
#define LL_SYSCFG_FLASH_B1_SECTOR5_STATUS_BIT   0x200000U
#define LL_SYSCFG_FLASH_B1_SECTOR6_STATUS_BIT   0x400000U
#define LL_SYSCFG_FLASH_B1_SECTOR7_STATUS_BIT   0x800000U
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_FLASH_BANK2_SECTORS SYSCFG Flash Bank2 sectors bits status
  * @{
  */
#define LL_SYSCFG_FLASH_B2_SECTOR0_STATUS_BIT   0x10000U
#define LL_SYSCFG_FLASH_B2_SECTOR1_STATUS_BIT   0x20000U
#define LL_SYSCFG_FLASH_B2_SECTOR2_STATUS_BIT   0x40000U
#define LL_SYSCFG_FLASH_B2_SECTOR3_STATUS_BIT   0x80000U
#define LL_SYSCFG_FLASH_B2_SECTOR4_STATUS_BIT   0x100000U
#define LL_SYSCFG_FLASH_B2_SECTOR5_STATUS_BIT   0x200000U
#define LL_SYSCFG_FLASH_B2_SECTOR6_STATUS_BIT   0x400000U
#define LL_SYSCFG_FLASH_B2_SECTOR7_STATUS_BIT   0x800000U
/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Constants SYSTEM Exported Constants
  * @{
  */

/** @defgroup SYSTEM_LL_EC_I2C_FASTMODEPLUS SYSCFG I2C FASTMODEPLUS
  * @{
  */
#define LL_SYSCFG_I2C_FASTMODEPLUS_I2C1        SYSCFG_PMCR_I2C1_FMP       /*!< Enable Fast Mode Plus for I2C1      */
#define LL_SYSCFG_I2C_FASTMODEPLUS_I2C2        SYSCFG_PMCR_I2C2_FMP       /*!< Enable Fast Mode Plus for I2C2      */
#define LL_SYSCFG_I2C_FASTMODEPLUS_I2C3        SYSCFG_PMCR_I2C3_FMP       /*!< Enable Fast Mode Plus for I2C3      */
#define LL_SYSCFG_I2C_FASTMODEPLUS_I2C4        SYSCFG_PMCR_I2C4_FMP       /*!< Enable Fast Mode Plus for I2C4      */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB6         SYSCFG_PMCR_I2C_PB6_FMP    /*!< Enable Fast Mode Plus on PB6        */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB7         SYSCFG_PMCR_I2C_PB7_FMP    /*!< Enable Fast Mode Plus on PB7        */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB8         SYSCFG_PMCR_I2C_PB8_FMP    /*!< Enable Fast Mode Plus on PB8        */
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB9         SYSCFG_PMCR_I2C_PB9_FMP    /*!< Enable Fast Mode Plus on PB9        */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_ANALOG_SWITCH Analog Switch control
* @{
*/
#define LL_SYSCFG_ANALOG_SWITCH_BOOSTEN           SYSCFG_PMCR_BOOSTEN               /*!< I/O analog switch voltage booster enable */
#define LL_SYSCFG_ANALOG_SWITCH_PA0               SYSCFG_PMCR_PA0SO                 /*!< PA0 Switch Open */
#define LL_SYSCFG_ANALOG_SWITCH_PA1               SYSCFG_PMCR_PA1SO                 /*!< PA1 Switch Open */
#define LL_SYSCFG_ANALOG_SWITCH_PC2               SYSCFG_PMCR_PC2SO                 /*!< PC2 Switch Open */
#define LL_SYSCFG_ANALOG_SWITCH_PC3               SYSCFG_PMCR_PC3SO                 /*!< PC3 Switch Open */
/**
  * @}
  */

#if defined(SYSCFG_PMCR_EPIS_SEL)
/** @defgroup SYSTEM_LL_EC_EPIS Ethernet PHY Interface Selection
* @{
*/
#define LL_SYSCFG_ETH_MII               0x00000000U                           /*!< ETH Media MII interface */
#define LL_SYSCFG_ETH_RMII              SYSCFG_PMCR_EPIS_SEL                   /*!< ETH Media RMII interface */
/**
  * @}
  */
#endif /* SYSCFG_PMCR_EPIS_SEL */

/** @defgroup SYSTEM_LL_EC_EXTI_PORT SYSCFG EXTI PORT
  * @{
  */
#define LL_SYSCFG_EXTI_PORTA               0U               /*!< EXTI PORT A                        */
#define LL_SYSCFG_EXTI_PORTB               1U               /*!< EXTI PORT B                        */
#define LL_SYSCFG_EXTI_PORTC               2U               /*!< EXTI PORT C                        */
#define LL_SYSCFG_EXTI_PORTD               3U               /*!< EXTI PORT D                        */
#define LL_SYSCFG_EXTI_PORTE               4U               /*!< EXTI PORT E                        */
#define LL_SYSCFG_EXTI_PORTF               5U               /*!< EXTI PORT F                        */
#define LL_SYSCFG_EXTI_PORTG               6U               /*!< EXTI PORT G                        */
#define LL_SYSCFG_EXTI_PORTH               7U               /*!< EXTI PORT H                        */
#define LL_SYSCFG_EXTI_PORTI               8U               /*!< EXTI PORT I                        */
#define LL_SYSCFG_EXTI_PORTJ               9U               /*!< EXTI PORT J                        */
#define LL_SYSCFG_EXTI_PORTK               10U              /*!< EXTI PORT k                        */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_EXTI_LINE SYSCFG EXTI LINE
  * @{
  */
#define LL_SYSCFG_EXTI_LINE0               ((0x000FUL << 16U) | 0U)  /*!< EXTI_POSITION_0  | EXTICR[0] */
#define LL_SYSCFG_EXTI_LINE1               ((0x00F0UL << 16U) | 0U)  /*!< EXTI_POSITION_4  | EXTICR[0] */
#define LL_SYSCFG_EXTI_LINE2               ((0x0F00UL << 16U) | 0U)  /*!< EXTI_POSITION_8  | EXTICR[0] */
#define LL_SYSCFG_EXTI_LINE3               ((0xF000UL << 16U) | 0U)  /*!< EXTI_POSITION_12 | EXTICR[0] */
#define LL_SYSCFG_EXTI_LINE4               ((0x000FUL << 16U) | 1U)  /*!< EXTI_POSITION_0  | EXTICR[1] */
#define LL_SYSCFG_EXTI_LINE5               ((0x00F0UL << 16U) | 1U)  /*!< EXTI_POSITION_4  | EXTICR[1] */
#define LL_SYSCFG_EXTI_LINE6               ((0x0F00UL << 16U) | 1U)  /*!< EXTI_POSITION_8  | EXTICR[1] */
#define LL_SYSCFG_EXTI_LINE7               ((0xF000UL << 16U) | 1U)  /*!< EXTI_POSITION_12 | EXTICR[1] */
#define LL_SYSCFG_EXTI_LINE8               ((0x000FUL << 16U) | 2U)  /*!< EXTI_POSITION_0  | EXTICR[2] */
#define LL_SYSCFG_EXTI_LINE9               ((0x00F0UL << 16U) | 2U)  /*!< EXTI_POSITION_4  | EXTICR[2] */
#define LL_SYSCFG_EXTI_LINE10              ((0x0F00UL << 16U) | 2U)  /*!< EXTI_POSITION_8  | EXTICR[2] */
#define LL_SYSCFG_EXTI_LINE11              ((0xF000UL << 16U) | 2U)  /*!< EXTI_POSITION_12 | EXTICR[2] */
#define LL_SYSCFG_EXTI_LINE12              ((0x000FUL << 16U) | 3U)  /*!< EXTI_POSITION_0  | EXTICR[3] */
#define LL_SYSCFG_EXTI_LINE13              ((0x00F0UL << 16U) | 3U)  /*!< EXTI_POSITION_4  | EXTICR[3] */
#define LL_SYSCFG_EXTI_LINE14              ((0x0F00UL << 16U) | 3U)  /*!< EXTI_POSITION_8  | EXTICR[3] */
#define LL_SYSCFG_EXTI_LINE15              ((0xF000UL << 16U) | 3U)  /*!< EXTI_POSITION_12 | EXTICR[3] */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIMBREAK SYSCFG TIMER BREAK
  * @{
  */
#define LL_SYSCFG_TIMBREAK_AXISRAM_DBL_ECC SYSCFG_CFGR_AXISRAML /*!< Enables and locks the AXIRAM double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_ITCM_DBL_ECC    SYSCFG_CFGR_ITCML    /*!< Enables and locks the ITCM double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_DTCM_DBL_ECC    SYSCFG_CFGR_DTCML    /*!< Enables and locks the DTCM double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_SRAM1_DBL_ECC   SYSCFG_CFGR_SRAM1L   /*!< Enables and locks the SRAM1 double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_SRAM2_DBL_ECC   SYSCFG_CFGR_SRAM2L   /*!< Enables and locks the SRAM2 double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_SRAM3_DBL_ECC   SYSCFG_CFGR_SRAM3L   /*!< Enables and locks the SRAM3 double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_SRAM4_DBL_ECC   SYSCFG_CFGR_SRAM4L   /*!< Enables and locks the SRAM4 double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_BKRAM_DBL_ECC   SYSCFG_CFGR_BKRAML   /*!< Enables and locks the BKRAM double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_CM7_LOCKUP      SYSCFG_CFGR_CM7L     /*!< Enables and locks the Cortex-M7 LOCKUP signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_FLASH_DBL_ECC   SYSCFG_CFGR_FLASHL   /*!< Enables and locks the FLASH double ECC error signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */

#define LL_SYSCFG_TIMBREAK_PVD             SYSCFG_CFGR_PVDL     /*!< Enables and locks the PVD connection
                                                                     with TIM1/8/15/16/17 and HRTIM Break Input
                                                                     and also the PVDE and PLS bits of the Power Control Interface */
#if defined(DUAL_CORE)
#define LL_SYSCFG_TIMBREAK_CM4_LOCKUP      SYSCFG_CFGR_CM4L     /*!< Enables and locks the Cortex-M4 LOCKUP signal
                                                                     with Break Input of TIM1/8/15/16/17 and HRTIM        */
#endif /* DUAL_CORE */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_CS SYSCFG I/O compensation cell Code selection
  * @{
  */
#define LL_SYSCFG_CELL_CODE               0U
#define LL_SYSCFG_REGISTER_CODE           SYSCFG_CCCSR_CS
/**
  * @}
  */

/** @defgroup SYSTEM_LL_IWDG1_CONTROL_MODES SYSCFG IWDG1 control modes
  * @{
  */
#define LL_SYSCFG_IWDG1_SW_CONTROL_MODE   0U
#define LL_SYSCFG_IWDG1_HW_CONTROL_MODE   SYSCFG_UR11_IWDG1M
/**
  * @}
  */

#if defined (DUAL_CORE)
/** @defgroup SYSTEM_LL_IWDG2_CONTROL_MODES SYSCFG IWDG2 control modes
  * @{
  */
#define LL_SYSCFG_IWDG2_SW_CONTROL_MODE   0U
#define LL_SYSCFG_IWDG2_HW_CONTROL_MODE   SYSCFG_UR12_IWDG2M
/**
  * @}
  */
#endif /* DUAL_CORE */

/** @defgroup SYSTEM_LL_DTCM_RAM_SIZE SYSCFG DTCM RAM size configuration
  * @{
  */
#define LL_SYSCFG_DTCM_RAM_SIZE_2KB     0U
#define LL_SYSCFG_DTCM_RAM_SIZE_4KB     1U
#define LL_SYSCFG_DTCM_RAM_SIZE_8KB     2U
#define LL_SYSCFG_DTCM_RAM_SIZE_16KB    3U
/**
  * @}
  */
#ifdef SYSCFG_PKGR_PKG
/** @defgroup SYSTEM_LL_PACKAGE SYSCFG device package
  * @{
  */
#define LL_SYSCFG_LQFP100_PACKAGE             0U
#define LL_SYSCFG_TQFP144_PACKAGE             2U
#define LL_SYSCFG_TQFP176_UFBGA176_PACKAGE    5U
#define LL_SYSCFG_LQFP208_TFBGA240_PACKAGE    8U
/**
  * @}
  */
#endif /* SYSCFG_PKGR_PKG */

/** @defgroup SYSTEM_LL_SYSCFG_BOR SYSCFG Brownout Reset Threshold Level
  * @{
  */
#define LL_SYSCFG_BOR_OFF_RESET_LEVEL      0x00000000U
#define LL_SYSCFG_BOR_LOW_RESET_LEVEL      SYSCFG_UR2_BORH_0
#define LL_SYSCFG_BOR_MEDIUM_RESET_LEVEL   SYSCFG_UR2_BORH_1
#define LL_SYSCFG_BOR_HIGH_RESET_LEVEL     SYSCFG_UR2_BORH

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TRACE DBGMCU TRACE Pin Assignment
  * @{
  */
#define LL_DBGMCU_TRACE_NONE               0x00000000U                                     /*!< TRACE pins not assigned (default state) */
#define LL_DBGMCU_TRACE_ASYNCH             DBGMCU_CR_TRACE_IOEN                            /*!< TRACE pin assignment for Asynchronous Mode */
#define LL_DBGMCU_TRACE_SYNCH_SIZE1        (DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE_0) /*!< TRACE pin assignment for Synchronous Mode with a TRACEDATA size of 1 */
#define LL_DBGMCU_TRACE_SYNCH_SIZE2        (DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE_1) /*!< TRACE pin assignment for Synchronous Mode with a TRACEDATA size of 2 */
#define LL_DBGMCU_TRACE_SYNCH_SIZE4        (DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE)   /*!< TRACE pin assignment for Synchronous Mode with a TRACEDATA size of 4 */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB1_GRP1_STOP_IP DBGMCU APB1 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB1_GRP1_TIM2_STOP      DBGMCU_APB1LFZ1_DBG_TIM2     /*!< TIM2 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM3_STOP      DBGMCU_APB1LFZ1_DBG_TIM3     /*!< TIM3 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM4_STOP      DBGMCU_APB1LFZ1_DBG_TIM4     /*!< TIM4 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM5_STOP      DBGMCU_APB1LFZ1_DBG_TIM5     /*!< TIM5 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM6_STOP      DBGMCU_APB1LFZ1_DBG_TIM6     /*!< TIM6 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM7_STOP      DBGMCU_APB1LFZ1_DBG_TIM7     /*!< TIM7 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM12_STOP     DBGMCU_APB1LFZ1_DBG_TIM12    /*!< TIM12 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM13_STOP     DBGMCU_APB1LFZ1_DBG_TIM13    /*!< TIM13 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_TIM14_STOP     DBGMCU_APB1LFZ1_DBG_TIM14    /*!< TIM14 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_LPTIM1_STOP    DBGMCU_APB1LFZ1_DBG_LPTIM1   /*!< LPTIM1 counter stopped when core is halted */
#define LL_DBGMCU_APB1_GRP1_I2C1_STOP      DBGMCU_APB1LFZ1_DBG_I2C1     /*!< I2C1 SMBUS timeout mode stopped when Core is halted */
#define LL_DBGMCU_APB1_GRP1_I2C2_STOP      DBGMCU_APB1LFZ1_DBG_I2C2     /*!< I2C2 SMBUS timeout mode stopped when Core is halted */
#define LL_DBGMCU_APB1_GRP1_I2C3_STOP      DBGMCU_APB1LFZ1_DBG_I2C3     /*!< I2C3 SMBUS timeout mode stopped when Core is halted */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB1_GRP2_STOP_IP DBGMCU APB1 GRP2 STOP IP
  * @{
  */
#define LL_DBGMCU_APB1_GRP2_FDCAN_STOP    DBGMCU_APB1HFZ1_DBG_FDCAN    /*!< FDCAN is frozen while the core is in debug mode */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB2_GRP1_STOP_IP DBGMCU APB2 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB2_GRP1_TIM1_STOP      DBGMCU_APB2FZ1_DBG_TIM1    /*!< TIM1 counter stopped when core is halted */
#define LL_DBGMCU_APB2_GRP1_TIM8_STOP      DBGMCU_APB2FZ1_DBG_TIM8    /*!< TIM8 counter stopped when core is halted */
#define LL_DBGMCU_APB2_GRP1_TIM15_STOP     DBGMCU_APB2FZ1_DBG_TIM15   /*!< TIM15 counter stopped when core is halted */
#define LL_DBGMCU_APB2_GRP1_TIM16_STOP     DBGMCU_APB2FZ1_DBG_TIM16   /*!< TIM16 counter stopped when core is halted */
#define LL_DBGMCU_APB2_GRP1_TIM17_STOP     DBGMCU_APB2FZ1_DBG_TIM17   /*!< TIM17 counter stopped when core is halted */
#define LL_DBGMCU_APB2_GRP1_HRTIM_STOP     DBGMCU_APB2FZ1_DBG_HRTIM   /*!< HRTIM counter stopped when core is halted */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB3_GRP1_STOP_IP DBGMCU APB3 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB3_GRP1_WWDG1_STOP      DBGMCU_APB3FZ1_DBG_WWDG1   /*!< WWDG1 is frozen while the core is in debug mode */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB4_GRP1_STOP_IP DBGMCU APB4 GRP1 STOP IP
  * @{
  */
#define LL_DBGMCU_APB4_GRP1_I2C4_STOP       DBGMCU_APB4FZ1_DBG_I2C4     /*!< I2C4 is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_LPTIM2_STOP     DBGMCU_APB4FZ1_DBG_LPTIM2   /*!< LPTIM2 is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_LPTIM3_STOP     DBGMCU_APB4FZ1_DBG_LPTIM3   /*!< LPTIM3 is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_LPTIM4_STOP     DBGMCU_APB4FZ1_DBG_LPTIM4   /*!< LPTIM4 is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_LPTIM5_STOP     DBGMCU_APB4FZ1_DBG_LPTIM5   /*!< LPTIM5 is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_RTC_STOP        DBGMCU_APB4FZ1_DBG_RTC      /*!< RTC is frozen while the core is in debug mode */
#define LL_DBGMCU_APB4_GRP1_IWDG1_STOP      DBGMCU_APB4FZ1_DBG_IWDG1    /*!< IWDG1 is frozen while the core is in debug mode */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_LATENCY FLASH LATENCY
  * @{
  */
#define LL_FLASH_LATENCY_0                 FLASH_ACR_LATENCY_0WS   /*!< FLASH Zero wait state */
#define LL_FLASH_LATENCY_1                 FLASH_ACR_LATENCY_1WS   /*!< FLASH One wait state */
#define LL_FLASH_LATENCY_2                 FLASH_ACR_LATENCY_2WS   /*!< FLASH Two wait states */
#define LL_FLASH_LATENCY_3                 FLASH_ACR_LATENCY_3WS   /*!< FLASH Three wait states */
#define LL_FLASH_LATENCY_4                 FLASH_ACR_LATENCY_4WS   /*!< FLASH Four wait states */
#define LL_FLASH_LATENCY_5                 FLASH_ACR_LATENCY_5WS   /*!< FLASH five wait state */
#define LL_FLASH_LATENCY_6                 FLASH_ACR_LATENCY_6WS   /*!< FLASH six wait state */
#define LL_FLASH_LATENCY_7                 FLASH_ACR_LATENCY_7WS   /*!< FLASH seven wait states */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Functions SYSTEM Exported Functions
  * @{
  */

/** @defgroup SYSTEM_LL_EF_SYSCFG SYSCFG
  * @{
  */

#if defined(SYSCFG_PMCR_EPIS_SEL)
/**
  * @brief  Select Ethernet PHY interface
  * @rmtoll PMCR    EPIS_SEL    LL_SYSCFG_SetPHYInterface
  * @param  Interface This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETH_MII
  *         @arg @ref LL_SYSCFG_ETH_RMII
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetPHYInterface(uint32_t Interface)
{
  MODIFY_REG(SYSCFG->PMCR, SYSCFG_PMCR_EPIS_SEL, Interface);
}

/**
  * @brief  Get Ethernet PHY interface
  * @rmtoll PMCR    EPIS_SEL    LL_SYSCFG_GetPHYInterface
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETH_MII
  *         @arg @ref LL_SYSCFG_ETH_RMII
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetPHYInterface(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->PMCR, SYSCFG_PMCR_EPIS_SEL));
}

#endif /* SYSCFG_PMCR_EPIS_SEL */
/**
  * @brief  Open an Analog Switch
  * @rmtoll PMCR    PA0SO   LL_SYSCFG_OpenAnalogSwitch
  * @rmtoll PMCR    PA1SO   LL_SYSCFG_OpenAnalogSwitch
  * @rmtoll PMCR    PC2SO   LL_SYSCFG_OpenAnalogSwitch
  * @rmtoll PMCR    PC3SO   LL_SYSCFG_OpenAnalogSwitch
  * @param  AnalogSwitch This parameter can be one of the following values:
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PA0 : PA0 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PA1:  PA1 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PC2 : PC2 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PC3:  PC3 analog switch
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_OpenAnalogSwitch(uint32_t AnalogSwitch)
{
  SET_BIT(SYSCFG->PMCR, AnalogSwitch);
}

/**
  * @brief  Close an Analog Switch
  * @rmtoll PMCR    PA0SO   LL_SYSCFG_CloseAnalogSwitch
  * @rmtoll PMCR    PA1SO   LL_SYSCFG_CloseAnalogSwitch
  * @rmtoll PMCR    PC2SO   LL_SYSCFG_CloseAnalogSwitch
  * @rmtoll PMCR    PC3SO   LL_SYSCFG_CloseAnalogSwitch
  * @param  AnalogSwitch This parameter can be one of the following values:
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PA0 : PA0 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PA1:  PA1 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PC2 : PC2 analog switch
  *         @arg LL_SYSCFG_ANALOG_SWITCH_PC3:  PC3 analog switch
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_CloseAnalogSwitch(uint32_t AnalogSwitch)
{
  CLEAR_BIT(SYSCFG->PMCR, AnalogSwitch);
}
#ifdef SYSCFG_PMCR_BOOSTEN
/**
  * @brief  Enable the Analog booster to reduce the total harmonic distortion
  *         of the analog switch when the supply voltage is lower than 2.7 V
  * @rmtoll PMCR    BOOSTEN   LL_SYSCFG_EnableAnalogBooster
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableAnalogBooster(void)
{
 SET_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}

/**
  * @brief  Disable the Analog booster
  * @rmtoll PMCR    BOOSTEN   LL_SYSCFG_DisableAnalogBooster
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableAnalogBooster(void)
{
 CLEAR_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}
#endif /*SYSCFG_PMCR_BOOSTEN*/
/**
  * @brief  Enable the I2C fast mode plus driving capability.
  * @rmtoll SYSCFG_PMCR   I2C_PBx_FMP   LL_SYSCFG_EnableFastModePlus\n
  *         SYSCFG_PMCR   I2Cx_FMP      LL_SYSCFG_EnableFastModePlus
  * @param  ConfigFastModePlus This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB6
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB7
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB8 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB9 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C1
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C2 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C3
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C4(*)
  *
  *         (*) value not defined in all devices
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableFastModePlus(uint32_t ConfigFastModePlus)
{
  SET_BIT(SYSCFG->PMCR, ConfigFastModePlus);
}

/**
  * @brief  Disable the I2C fast mode plus driving capability.
  * @rmtoll SYSCFG_PMCR    I2C_PBx_FMP   LL_SYSCFG_DisableFastModePlus\n
  *         SYSCFG_PMCR    I2Cx_FMP      LL_SYSCFG_DisableFastModePlus
  * @param  ConfigFastModePlus This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB6
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB7
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB8 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_PB9 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C1
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C2 (*)
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C3
  *         @arg @ref LL_SYSCFG_I2C_FASTMODEPLUS_I2C4
  *         (*) value not defined in all devices
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableFastModePlus(uint32_t ConfigFastModePlus)
{
  CLEAR_BIT(SYSCFG->PMCR, ConfigFastModePlus);
}

/**
  * @brief  Configure source input for the EXTI external interrupt.
  * @rmtoll SYSCFG_EXTICR1  EXTIx         LL_SYSCFG_SetEXTISource\n
  *         SYSCFG_EXTICR2 EXTIx         LL_SYSCFG_SetEXTISource\n
  *         SYSCFG_EXTICR3 EXTIx         LL_SYSCFG_SetEXTISource\n
  *         SYSCFG_EXTICR4 EXTIx         LL_SYSCFG_SetEXTISource
  * @param  Port This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_EXTI_PORTA
  *         @arg @ref LL_SYSCFG_EXTI_PORTB
  *         @arg @ref LL_SYSCFG_EXTI_PORTC
  *         @arg @ref LL_SYSCFG_EXTI_PORTD
  *         @arg @ref LL_SYSCFG_EXTI_PORTE
  *         @arg @ref LL_SYSCFG_EXTI_PORTF
  *         @arg @ref LL_SYSCFG_EXTI_PORTG
  *         @arg @ref LL_SYSCFG_EXTI_PORTH
  *         @arg @ref LL_SYSCFG_EXTI_PORTI
  *         @arg @ref LL_SYSCFG_EXTI_PORTJ
  *         @arg @ref LL_SYSCFG_EXTI_PORTK
  *
  *         (*) value not defined in all devices
  * @param  Line This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_EXTI_LINE0
  *         @arg @ref LL_SYSCFG_EXTI_LINE1
  *         @arg @ref LL_SYSCFG_EXTI_LINE2
  *         @arg @ref LL_SYSCFG_EXTI_LINE3
  *         @arg @ref LL_SYSCFG_EXTI_LINE4
  *         @arg @ref LL_SYSCFG_EXTI_LINE5
  *         @arg @ref LL_SYSCFG_EXTI_LINE6
  *         @arg @ref LL_SYSCFG_EXTI_LINE7
  *         @arg @ref LL_SYSCFG_EXTI_LINE8
  *         @arg @ref LL_SYSCFG_EXTI_LINE9
  *         @arg @ref LL_SYSCFG_EXTI_LINE10
  *         @arg @ref LL_SYSCFG_EXTI_LINE11
  *         @arg @ref LL_SYSCFG_EXTI_LINE12
  *         @arg @ref LL_SYSCFG_EXTI_LINE13
  *         @arg @ref LL_SYSCFG_EXTI_LINE14
  *         @arg @ref LL_SYSCFG_EXTI_LINE15
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetEXTISource(uint32_t Port, uint32_t Line)
{
  MODIFY_REG(SYSCFG->EXTICR[Line & 0x3U], (Line >> 16U), Port << ((POSITION_VAL(Line >> 16U)) & 31U));
}

/**
  * @brief  Get the configured defined for specific EXTI Line
  * @rmtoll SYSCFG_EXTICR1 EXTIx         LL_SYSCFG_GetEXTISource\n
  *         SYSCFG_EXTICR2 EXTIx         LL_SYSCFG_GetEXTISource\n
  *         SYSCFG_EXTICR3 EXTIx         LL_SYSCFG_GetEXTISource\n
  *         SYSCFG_EXTICR4 EXTIx         LL_SYSCFG_GetEXTISource
  * @param  Line This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_EXTI_LINE0
  *         @arg @ref LL_SYSCFG_EXTI_LINE1
  *         @arg @ref LL_SYSCFG_EXTI_LINE2
  *         @arg @ref LL_SYSCFG_EXTI_LINE3
  *         @arg @ref LL_SYSCFG_EXTI_LINE4
  *         @arg @ref LL_SYSCFG_EXTI_LINE5
  *         @arg @ref LL_SYSCFG_EXTI_LINE6
  *         @arg @ref LL_SYSCFG_EXTI_LINE7
  *         @arg @ref LL_SYSCFG_EXTI_LINE8
  *         @arg @ref LL_SYSCFG_EXTI_LINE9
  *         @arg @ref LL_SYSCFG_EXTI_LINE10
  *         @arg @ref LL_SYSCFG_EXTI_LINE11
  *         @arg @ref LL_SYSCFG_EXTI_LINE12
  *         @arg @ref LL_SYSCFG_EXTI_LINE13
  *         @arg @ref LL_SYSCFG_EXTI_LINE14
  *         @arg @ref LL_SYSCFG_EXTI_LINE15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_EXTI_PORTA
  *         @arg @ref LL_SYSCFG_EXTI_PORTB
  *         @arg @ref LL_SYSCFG_EXTI_PORTC
  *         @arg @ref LL_SYSCFG_EXTI_PORTD
  *         @arg @ref LL_SYSCFG_EXTI_PORTE
  *         @arg @ref LL_SYSCFG_EXTI_PORTF
  *         @arg @ref LL_SYSCFG_EXTI_PORTG
  *         @arg @ref LL_SYSCFG_EXTI_PORTH
  *         @arg @ref LL_SYSCFG_EXTI_PORTI
  *         @arg @ref LL_SYSCFG_EXTI_PORTJ
  *         @arg @ref LL_SYSCFG_EXTI_PORTK
  *         (*) value not defined in all devices
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetEXTISource(uint32_t Line)
{
  return (uint32_t)(READ_BIT(SYSCFG->EXTICR[Line & 0x3U], (Line >> 16U)) >> (POSITION_VAL(Line >> 16U) & 31U));
}

/**
  * @brief  Set connections to TIM1/8/15/16/17 and HRTIM Break inputs
  * @note this feature is available on STM32H7 rev.B and above
  * @rmtoll SYSCFG_CFGR AXISRAML       LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR ITCML          LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR DTCML          LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM1L         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM2L         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM3L         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM4L         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR BKRAML         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR CM7L           LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR FLASHL         LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR PVDL           LL_SYSCFG_SetTIMBreakInputs\n
  *         SYSCFG_CFGR_CM4L           LL_SYSCFG_SetTIMBreakInputs
  * @param  Break This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_TIMBREAK_AXISRAM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_ITCM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_DTCM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM1_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM2_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM3_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM4_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_BKRAM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_CM7_LOCKUP
  *         @arg @ref LL_SYSCFG_TIMBREAK_FLASH_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_PVD
  *         @arg @ref LL_SYSCFG_TIMBREAK_CM4_LOCKUP (available for dual core lines only)
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetTIMBreakInputs(uint32_t Break)
{
#if defined(DUAL_CORE)
  MODIFY_REG(SYSCFG->CFGR, SYSCFG_CFGR_AXISRAML | SYSCFG_CFGR_ITCML  | SYSCFG_CFGR_DTCML  | SYSCFG_CFGR_SRAM1L | SYSCFG_CFGR_SRAM2L | \
                           SYSCFG_CFGR_SRAM3L   | SYSCFG_CFGR_SRAM4L | SYSCFG_CFGR_BKRAML | SYSCFG_CFGR_CM7L   | SYSCFG_CFGR_FLASHL | \
                           SYSCFG_CFGR_PVDL     | SYSCFG_CFGR_CM4L, Break);
#elif defined (SYSCFG_CFGR_AXISRAML)
  MODIFY_REG(SYSCFG->CFGR, SYSCFG_CFGR_AXISRAML | SYSCFG_CFGR_ITCML  | SYSCFG_CFGR_DTCML  | SYSCFG_CFGR_SRAM1L | SYSCFG_CFGR_SRAM2L | \
                           SYSCFG_CFGR_SRAM3L   | SYSCFG_CFGR_SRAM4L | SYSCFG_CFGR_BKRAML | SYSCFG_CFGR_CM7L   | SYSCFG_CFGR_FLASHL | \
                           SYSCFG_CFGR_PVDL, Break);
#else
  MODIFY_REG(SYSCFG->CFGR, SYSCFG_CFGR_ITCML  | SYSCFG_CFGR_DTCML  |\
                           SYSCFG_CFGR_CM7L   | SYSCFG_CFGR_FLASHL | \
                           SYSCFG_CFGR_PVDL, Break);
#endif /* DUAL_CORE */
}

/**
  * @brief  Get connections to TIM1/8/15/16/17 and HRTIM Break inputs
  * @note this feature is available on STM32H7 rev.B and above
  * @rmtoll SYSCFG_CFGR AXISRAML       LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR ITCML          LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR DTCML          LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM1L         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM2L         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM3L         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR SRAM4L         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR BKRAML         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR CM7L           LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR FLASHL         LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR PVDL           LL_SYSCFG_GetTIMBreakInputs\n
  *         SYSCFG_CFGR_CM4L           LL_SYSCFG_GetTIMBreakInputs
  * @retval Returned value can be can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_TIMBREAK_AXISRAM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_ITCM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_DTCM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM1_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM2_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM3_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_SRAM4_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_BKRAM_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_CM7_LOCKUP
  *         @arg @ref LL_SYSCFG_TIMBREAK_FLASH_DBL_ECC
  *         @arg @ref LL_SYSCFG_TIMBREAK_PVD
  *         @arg @ref LL_SYSCFG_TIMBREAK_CM4_LOCKUP (available for dual core lines only)
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetTIMBreakInputs(void)
{
#if defined(DUAL_CORE)
  return (uint32_t)(READ_BIT(SYSCFG->CFGR,  SYSCFG_CFGR_AXISRAML | SYSCFG_CFGR_ITCML  | SYSCFG_CFGR_DTCML  | \
                                            SYSCFG_CFGR_SRAM1L   | SYSCFG_CFGR_SRAM2L | SYSCFG_CFGR_SRAM3L | \
                                            SYSCFG_CFGR_SRAM4L   | SYSCFG_CFGR_BKRAML | SYSCFG_CFGR_CM7L   | \
                                            SYSCFG_CFGR_FLASHL   | SYSCFG_CFGR_PVDL   | SYSCFG_CFGR_CM4L));
#elif defined (SYSCFG_CFGR_AXISRAML)
  return (uint32_t)(READ_BIT(SYSCFG->CFGR,  SYSCFG_CFGR_AXISRAML | SYSCFG_CFGR_ITCML  | SYSCFG_CFGR_DTCML  | \
                                            SYSCFG_CFGR_SRAM1L   | SYSCFG_CFGR_SRAM2L | SYSCFG_CFGR_SRAM3L | \
                                            SYSCFG_CFGR_SRAM4L   | SYSCFG_CFGR_BKRAML | SYSCFG_CFGR_CM7L   | \
                                            SYSCFG_CFGR_FLASHL   | SYSCFG_CFGR_PVDL ));
#else
  return (uint32_t)(READ_BIT(SYSCFG->CFGR,  SYSCFG_CFGR_ITCML    | SYSCFG_CFGR_DTCML  | SYSCFG_CFGR_CM7L   | \
                                            SYSCFG_CFGR_FLASHL   | SYSCFG_CFGR_PVDL ));
#endif /* DUAL_CORE */
}

/**
  * @brief  Enable the Compensation Cell
  * @rmtoll CCCSR   EN    LL_SYSCFG_EnableCompensationCell
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableCompensationCell(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN);
}

/**
  * @brief  Disable the Compensation Cell
  * @rmtoll CCCSR   EN    LL_SYSCFG_DisableCompensationCell
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableCompensationCell(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN);
}

/**
  * @brief  Check if the Compensation Cell is enabled
  * @rmtoll CCCSR   EN    LL_SYSCFG_IsEnabledCompensationCell
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledCompensationCell(void)
{
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN) == SYSCFG_CCCSR_EN) ? 1UL : 0UL);
}

/**
  * @brief  Get Compensation Cell ready Flag
  * @rmtoll CCCSR   READY   LL_SYSCFG_IsActiveFlag_CMPCR
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsActiveFlag_CMPCR(void)
{
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_READY) == (SYSCFG_CCCSR_READY)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the I/O speed optimization when the product voltage is low.
  * @rmtoll CCCSR   HSLV    LL_SYSCFG_EnableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableIOSpeedOptimization(void)
{
#if defined(SYSCFG_CCCSR_HSLV)
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV);
#else
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV0);
#endif   /* SYSCFG_CCCSR_HSLV */
}

#if defined(SYSCFG_CCCSR_HSLV1)
/**
  * @brief  Enable the I/O speed optimization when the product voltage is low.
  * @rmtoll CCCSR   HSLV1    LL_SYSCFG_EnableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableIOSpeedOptimization1(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV1);
}

/**
  * @brief  Enable the I/O speed optimization when the product voltage is low.
  * @rmtoll CCCSR   HSLV2    LL_SYSCFG_EnableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableIOSpeedOptimization2(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV2);
}

/**
  * @brief  Enable the I/O speed optimization when the product voltage is low.
  * @rmtoll CCCSR   HSLV3    LL_SYSCFG_EnableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableIOSpeedOptimization3(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV3);
}
#endif /*SYSCFG_CCCSR_HSLV1*/


/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @rmtoll CCCSR   HSLV    LL_SYSCFG_DisableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableIOSpeedOptimization(void)
{
#if defined(SYSCFG_CCCSR_HSLV)
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV);
#else
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV0);
#endif   /* SYSCFG_CCCSR_HSLV */
}

#if defined(SYSCFG_CCCSR_HSLV1)
/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @rmtoll CCCSR   HSLV1    LL_SYSCFG_DisableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableIOSpeedOptimization1(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV1);
}

/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @rmtoll CCCSR   HSLV2    LL_SYSCFG_DisableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableIOSpeedOptimization2(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV2);
}

/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @rmtoll CCCSR   HSLV3    LL_SYSCFG_DisableIOSpeedOptimize
  * @note   This bit is active only if IO_HSLV user option bit is set. It must be used only if the
  *         product supply voltage is below 2.7 V. Setting this bit when VDD is higher than 2.7 V
  *         might be destructive.
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableIOSpeedOptimization3(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV3);
}
#endif /*SYSCFG_CCCSR_HSLV1*/

/**
  * @brief  Check if the I/O speed optimization is enabled
  * @rmtoll CCCSR   HSLV    LL_SYSCFG_IsEnabledIOSpeedOptimization
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledIOSpeedOptimization(void)
{
#if defined(SYSCFG_CCCSR_HSLV)
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV) == SYSCFG_CCCSR_HSLV) ? 1UL : 0UL);
#else
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV0) == SYSCFG_CCCSR_HSLV0) ? 1UL : 0UL);
#endif /*SYSCFG_CCCSR_HSLV*/
}

#if defined(SYSCFG_CCCSR_HSLV1)
/**
  * @brief  Check if the I/O speed optimization is enabled
  * @rmtoll CCCSR   HSLV1    LL_SYSCFG_IsEnabledIOSpeedOptimization
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledIOSpeedOptimization1(void)
{
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV1) == SYSCFG_CCCSR_HSLV1) ? 1UL : 0UL);
}

/**
  * @brief  Check if the I/O speed optimization is enabled
  * @rmtoll CCCSR   HSLV2    LL_SYSCFG_IsEnabledIOSpeedOptimization
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledIOSpeedOptimization2(void)
{
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV2) == SYSCFG_CCCSR_HSLV2) ? 1UL : 0UL);
}

/**
  * @brief  Check if the I/O speed optimization is enabled
  * @rmtoll CCCSR   HSLV3    LL_SYSCFG_IsEnabledIOSpeedOptimization
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledIOSpeedOptimization3(void)
{
  return ((READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV3) == SYSCFG_CCCSR_HSLV3) ? 1UL : 0UL);
}
#endif /*SYSCFG_CCCSR_HSLV1*/

/**
  * @brief  Set the code selection for the I/O Compensation cell
  * @rmtoll CCCSR   CS    LL_SYSCFG_SetCellCompensationCode
  * @param  CompCode: Selects the code to be applied for the I/O compensation cell
  *   This parameter can be one of the following values:
  *   @arg LL_SYSCFG_CELL_CODE : Select Code from the cell (available in the SYSCFG_CCVR)
  *   @arg LL_SYSCFG_REGISTER_CODE: Select Code from the SYSCFG compensation cell code register (SYSCFG_CCCR)
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetCellCompensationCode(uint32_t CompCode)
{
  SET_BIT(SYSCFG->CCCSR, CompCode);
}

/**
  * @brief  Get the code selected for the I/O Compensation cell
  * @rmtoll CCCSR   CS    LL_SYSCFG_GetCellCompensationCode
  * @retval Returned value can be one of the following values:
  *   @arg LL_SYSCFG_CELL_CODE : Selected Code is from the cell (available in the SYSCFG_CCVR)
  *   @arg LL_SYSCFG_REGISTER_CODE: Selected Code is from the SYSCFG compensation cell code register (SYSCFG_CCCR)
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetCellCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_CS));
}

#ifdef SYSCFG_CCCSR_CS_MMC

/**
  * @brief  Get the code selected for the I/O Compensation cell on the VDDMMC power rail
  * @rmtoll CCCSR   CS    LL_SYSCFG_GetCellCompensationCode
  * @retval Returned value can be one of the following values:
  *   @arg LL_SYSCFG_CELL_CODE : Selected Code is from the cell (available in the SYSCFG_CCVR)
  *   @arg LL_SYSCFG_REGISTER_CODE: Selected Code is from the SYSCFG compensation cell code register (SYSCFG_CCCR)
  */
__STATIC_INLINE uint32_t LL_SYSCFG_MMCGetCellCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_CS_MMC));
}
#endif /*SYSCFG_CCCSR_CS_MMC*/

/**
  * @brief  Get I/O compensation cell value for PMOS transistors
  * @rmtoll CCVR    PCV   LL_SYSCFG_GetPMOSCompensationValue
  * @retval Returned value is the I/O compensation cell value for PMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetPMOSCompensationValue(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCVR, SYSCFG_CCVR_PCV));
}

/**
  * @brief  Get I/O compensation cell value for NMOS transistors
  * @rmtoll CCVR    NCV   LL_SYSCFG_GetNMOSCompensationValue
  * @retval Returned value is the I/O compensation cell value for NMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetNMOSCompensationValue(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCVR, SYSCFG_CCVR_NCV));
}

/**
  * @brief  Set I/O compensation cell code for PMOS transistors
  * @rmtoll CCCR    PCC   LL_SYSCFG_SetPMOSCompensationCode
  * @param  PMOSCode PMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *         SYSCFG_CMPCR is set
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetPMOSCompensationCode(uint32_t PMOSCode)
{
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_PCC, PMOSCode);
}

/**
  * @brief  Get I/O compensation cell code for PMOS transistors
  * @rmtoll CCCR    PCC   LL_SYSCFG_GetPMOSCompensationCode
  * @retval Returned value is the I/O compensation cell code for PMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetPMOSCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCR, SYSCFG_CCCR_PCC));
}

#ifdef SYSCFG_CCCR_PCC_MMC

/**
  * @brief  Set I/O compensation cell code for PMOS transistors corresponding to the VDDMMC power rail
  * @rmtoll CCCR    PCC   LL_SYSCFG_SetPMOSCompensationCode
  * @param  PMOSCode PMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *         SYSCFG_CMPCR is set
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_MMCSetPMOSCompensationCode(uint32_t PMOSCode)
{
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_PCC_MMC, PMOSCode);
}

/**
  * @brief  Get I/O compensation cell code for PMOS transistors corresponding to the VDDMMC power rail
  * @rmtoll CCCR    PCC   LL_SYSCFG_GetPMOSCompensationCode
  * @retval Returned value is the I/O compensation cell code for PMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_MMCGetPMOSCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCR, SYSCFG_CCCR_PCC_MMC));
}
#endif /* SYSCFG_CCCR_PCC_MMC */

/**
  * @brief  Set I/O compensation cell code for NMOS transistors
  * @rmtoll CCCR    NCC   LL_SYSCFG_SetNMOSCompensationCode
  * @param  NMOSCode NMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *         SYSCFG_CMPCR is set
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetNMOSCompensationCode(uint32_t NMOSCode)
{
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_NCC, NMOSCode);
}

/**
  * @brief  Get I/O compensation cell code for NMOS transistors
  * @rmtoll CCCR    NCC   LL_SYSCFG_GetNMOSCompensationCode
  * @retval Returned value is the I/O compensation cell code for NMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetNMOSCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCR, SYSCFG_CCCR_NCC));
}

#ifdef SYSCFG_CCCR_NCC_MMC

/**
  * @brief  Set I/O compensation cell code for NMOS transistors on the VDDMMC power rail.
  * @rmtoll CCCR    NCC   LL_SYSCFG_SetNMOSCompensationCode
  * @param  NMOSCode: NMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *         SYSCFG_CMPCR is set
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_VDMMCSetNMOSCompensationCode(uint32_t NMOSCode)
{
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_NCC_MMC, NMOSCode);
}

/**
  * @brief  Get I/O compensation cell code for NMOS transistors on the VDDMMC power rail.
  * @rmtoll CCCR    NCC   LL_SYSCFG_GetNMOSCompensationCode
  * @retval Returned value is the I/O compensation cell code for NMOS transistors
  */
__STATIC_INLINE uint32_t LL_SYSCFG_VDMMCGetNMOSCompensationCode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CCCR, SYSCFG_CCCR_NCC_MMC));
}
#endif /*SYSCFG_CCCR_NCC_MMC*/

#ifdef SYSCFG_PKGR_PKG
/**
  * @brief  Get the device package
  * @rmtoll PKGR    PKG   LL_SYSCFG_GetPackage
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_LQFP100_PACKAGE
  *         @arg @ref LL_SYSCFG_TQFP144_PACKAGE
  *         @arg @ref LL_SYSCFG_TQFP176_UFBGA176_PACKAGE
  *         @arg @ref LL_SYSCFG_LQFP208_TFBGA240_PACKAGE
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetPackage(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->PKGR, SYSCFG_PKGR_PKG));
}
#endif /*SYSCFG_PKGR_PKG*/

#ifdef SYSCFG_UR0_RDP
/**
  * @brief  Get the Flash memory protection level
  * @rmtoll UR0   RDP   LL_SYSCFG_GetFLashProtectionLevel
  * @retval Returned value can be one of the following values:
  *         0xAA : RDP level 0
  *         0xCC : RDP level 2
  *         Any other value : RDP level 1
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFLashProtectionLevel(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR0, SYSCFG_UR0_RDP));
}
/**
  * @brief  Indicate if the Flash memory bank addresses are inverted or not
  * @rmtoll UR0   BKS   LL_SYSCFG_IsFLashBankAddressesSwaped
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFLashBankAddressesSwaped(void)
{
  return ((READ_BIT(SYSCFG->UR0, SYSCFG_UR0_BKS) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Get the BOR Threshold Reset Level
  * @rmtoll UR2   BORH    LL_SYSCFG_GetBrownoutResetLevel
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_BOR_HIGH_RESET_LEVEL
  *         @arg @ref LL_SYSCFG_BOR_MEDIUM_RESET_LEVEL
  *         @arg @ref LL_SYSCFG_BOR_LOW_RESET_LEVEL
  *         @arg @ref LL_SYSCFG_BOR_OFF_RESET_LEVEL
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetBrownoutResetLevel(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR2, SYSCFG_UR2_BORH));
}
/**
  * @brief  BootCM7 address 0 configuration
  * @rmtoll UR2   BOOT_ADD0   LL_SYSCFG_SetCM7BootAddress0
  * @param  BootAddress :Specifies the CM7 Boot Address to be loaded in Address0
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetCM7BootAddress0(uint16_t BootAddress)
{
  /* Configure CM7 BOOT ADD0 */
#if defined(DUAL_CORE)
  MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BCM7_ADD0, ((uint32_t)BootAddress << SYSCFG_UR2_BCM7_ADD0_Pos));
#else
  MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BOOT_ADD0, ((uint32_t)BootAddress << SYSCFG_UR2_BOOT_ADD0_Pos));
#endif /*DUAL_CORE*/

}

/**
  * @brief  Get BootCM7 address 0
  * @rmtoll UR2   BOOT_ADD0   LL_SYSCFG_GetCM7BootAddress0
  * @retval Returned the CM7 Boot Address0
  */
__STATIC_INLINE uint16_t LL_SYSCFG_GetCM7BootAddress0(void)
{
  /* Get CM7 BOOT ADD0 */
#if defined(DUAL_CORE)
  return (uint16_t)((uint32_t)READ_BIT(SYSCFG->UR2, SYSCFG_UR2_BCM7_ADD0) >> SYSCFG_UR2_BCM7_ADD0_Pos);
#else
  return (uint16_t)((uint32_t)READ_BIT(SYSCFG->UR2, SYSCFG_UR2_BOOT_ADD0) >> SYSCFG_UR2_BOOT_ADD0_Pos);
#endif /*DUAL_CORE*/
}

/**
  * @brief  BootCM7 address 1 configuration
  * @rmtoll UR3   BOOT_ADD1   LL_SYSCFG_SetCM7BootAddress1
  * @param  BootAddress :Specifies the CM7 Boot Address to be loaded in Address1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetCM7BootAddress1(uint16_t BootAddress)
{
  /* Configure CM7 BOOT ADD1 */
#if defined(DUAL_CORE)
  MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BCM7_ADD1, BootAddress);
#else
  MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BOOT_ADD1, BootAddress);
#endif /*DUAL_CORE*/
}

/**
  * @brief  Get BootCM7 address 1
  * @rmtoll UR3   BOOT_ADD1   LL_SYSCFG_GetCM7BootAddress1
  * @retval Returned the CM7 Boot Address0
  */
__STATIC_INLINE uint16_t LL_SYSCFG_GetCM7BootAddress1(void)
{
  /* Get CM7 BOOT ADD0 */
#if defined(DUAL_CORE)
  return (uint16_t)(READ_BIT(SYSCFG->UR3, SYSCFG_UR3_BCM7_ADD1));
#else
  return (uint16_t)(READ_BIT(SYSCFG->UR3, SYSCFG_UR3_BOOT_ADD1));
#endif /* DUAL_CORE */
}

#if defined(DUAL_CORE)
/**
  * @brief  BootCM4 address 0 configuration
  * @rmtoll UR3   BCM4_ADD0   LL_SYSCFG_SetCM4BootAddress0
  * @param  BootAddress :Specifies the CM4 Boot Address to be loaded in Address0
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetCM4BootAddress0(uint16_t BootAddress)
{
  /* Configure CM4 BOOT ADD0 */
  MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BCM4_ADD0, ((uint32_t)BootAddress << SYSCFG_UR3_BCM4_ADD0_Pos));
}

/**
  * @brief  Get BootCM4 address 0
  * @rmtoll UR3   BCM4_ADD0   LL_SYSCFG_GetCM4BootAddress0
  * @retval Returned the CM4 Boot Address0
  */
__STATIC_INLINE uint16_t LL_SYSCFG_GetCM4BootAddress0(void)
{
  /* Get CM4 BOOT ADD0 */
  return (uint16_t)((uint32_t)READ_BIT(SYSCFG->UR3, SYSCFG_UR3_BCM4_ADD0) >> SYSCFG_UR3_BCM4_ADD0_Pos);
}

/**
  * @brief  BootCM4 address 1 configuration
  * @rmtoll UR4   BCM4_ADD1   LL_SYSCFG_SetCM4BootAddress1
  * @param  BootAddress :Specifies the CM4 Boot Address to be loaded in Address1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetCM4BootAddress1(uint16_t BootAddress)
{
  /* Configure CM4 BOOT ADD1 */
  MODIFY_REG(SYSCFG->UR4, SYSCFG_UR4_BCM4_ADD1, BootAddress);
}

/**
  * @brief  Get BootCM4 address 1
  * @rmtoll UR4   BCM4_ADD1   LL_SYSCFG_GetCM4BootAddress1
  * @retval Returned the CM4 Boot Address0
  */
__STATIC_INLINE uint16_t LL_SYSCFG_GetCM4BootAddress1(void)
{
  /* Get CM4 BOOT ADD0 */
  return (uint16_t)(READ_BIT(SYSCFG->UR4, SYSCFG_UR4_BCM4_ADD1));
}
#endif /*DUAL_CORE*/

/**
  * @brief  Indicates if the flash protected area (Bank 1) is erased by a mass erase
  * @rmtoll UR4   MEPAD_BANK1   LL_SYSCFG_IsFlashB1ProtectedAreaErasable
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1ProtectedAreaErasable(void)
{
  return ((READ_BIT(SYSCFG->UR4, SYSCFG_UR4_MEPAD_BANK1) == SYSCFG_UR4_MEPAD_BANK1) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the flash secured area (Bank 1) is erased by a mass erase
  * @rmtoll UR5   MESAD_BANK1   LL_SYSCFG_IsFlashB1SecuredAreaErasable
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1SecuredAreaErasable(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_MESAD_BANK1) == SYSCFG_UR5_MESAD_BANK1) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 0 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector0WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector0WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR0_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 1 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector1WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector1WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR1_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 2 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector2WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector2WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR2_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 3 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector3WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector3WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR3_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 4 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector4WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector4WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR4_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 5 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector5WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector5WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR5_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 6 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector6WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector6WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR6_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 7 of the Flash memory bank 1 is write protected
  * @rmtoll UR5   WRPN_BANK1    LL_SYSCFG_IsFlashB1Sector7WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB1Sector7WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR5, SYSCFG_UR5_WRPN_BANK1) == (SYSCFG_UR5_WRPN_BANK1 & LL_SYSCFG_FLASH_B1_SECTOR7_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Get the protected area start address for Flash bank 1
  * @rmtoll UR6   PABEG_BANK1    LL_SYSCFG_GetFlashB1ProtectedAreaStartAddress
  * @retval Returned the protected area start address for Flash bank 1
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB1ProtectedAreaStartAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR6, SYSCFG_UR6_PABEG_BANK1));
}

/**
  * @brief  Get the protected area end address for Flash bank 1
  * @rmtoll UR6   PAEND_BANK1   LL_SYSCFG_GetFlashB1ProtectedAreaEndAddress
  * @retval Returned the protected area end address for Flash bank 1
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB1ProtectedAreaEndAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR6, SYSCFG_UR6_PAEND_BANK1));
}

/**
  * @brief  Get the secured area start address for Flash bank 1
  * @rmtoll UR7   SABEG_BANK1   LL_SYSCFG_GetFlashB1SecuredAreaStartAddress
  * @retval Returned the secured area start address for Flash bank 1
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB1SecuredAreaStartAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR7, SYSCFG_UR7_SABEG_BANK1));
}

/**
  * @brief  Get the secured area end address for Flash bank 1
  * @rmtoll UR7   SAEND_BANK1   LL_SYSCFG_GetFlashB1SecuredAreaEndAddress
  * @retval Returned the secured area end address for Flash bank 1
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB1SecuredAreaEndAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR7, SYSCFG_UR7_SAEND_BANK1));
}

/**
  * @brief  Indicates if the flash protected area (Bank 2) is erased by a mass erase
  * @rmtoll UR8   MEPAD_BANK2   LL_SYSCFG_IsFlashB2ProtectedAreaErasable
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2ProtectedAreaErasable(void)
{
  return ((READ_BIT(SYSCFG->UR8, SYSCFG_UR8_MEPAD_BANK2) == SYSCFG_UR8_MEPAD_BANK2) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the flash secured area (Bank 2) is erased by a mass erase
  * @rmtoll UR8   MESAD_BANK2   LL_SYSCFG_IsFlashB2SecuredAreaErasable
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2SecuredAreaErasable(void)
{
  return ((READ_BIT(SYSCFG->UR8, SYSCFG_UR8_MESAD_BANK2) == SYSCFG_UR8_MESAD_BANK2) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 0 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector0WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector0WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR0_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 1 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector1WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector1WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR1_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 2 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector2WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector2WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR2_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 3 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector3WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector3WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR3_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 4 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector4WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector4WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR4_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 5 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector5WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector5WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR5_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 6 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector6WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector6WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR6_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the sector 7 of the Flash memory bank 2 is write protected
  * @rmtoll UR9   WRPN_BANK2    LL_SYSCFG_IsFlashB2Sector7WriteProtected
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsFlashB2Sector7WriteProtected(void)
{
  return ((READ_BIT(SYSCFG->UR9, SYSCFG_UR9_WRPN_BANK2) == (SYSCFG_UR9_WRPN_BANK2 & LL_SYSCFG_FLASH_B2_SECTOR7_STATUS_BIT)) ? 1UL : 0UL);
}

/**
  * @brief  Get the protected area start address for Flash bank 2
  * @rmtoll UR9   PABEG_BANK2   LL_SYSCFG_GetFlashB2ProtectedAreaStartAddress
  * @retval Returned the protected area start address for Flash bank 2
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB2ProtectedAreaStartAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR9, SYSCFG_UR9_PABEG_BANK2));
}

/**
  * @brief  Get the protected area end address for Flash bank 2
  * @rmtoll UR10    PAEND_BANK2   LL_SYSCFG_GetFlashB2ProtectedAreaEndAddress
  * @retval Returned the protected area end address for Flash bank 2
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB2ProtectedAreaEndAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR10, SYSCFG_UR10_PAEND_BANK2));
}

/**
  * @brief  Get the secured area start address for Flash bank 2
  * @rmtoll UR10    SABEG_BANK2   LL_SYSCFG_GetFlashB2SecuredAreaStartAddress
  * @retval Returned the secured area start address for Flash bank 2
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB2SecuredAreaStartAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR10, SYSCFG_UR10_SABEG_BANK2));
}

/**
  * @brief  Get the secured area end address for Flash bank 2
  * @rmtoll UR11    SAEND_BANK2   LL_SYSCFG_GetFlashB2SecuredAreaEndAddress
  * @retval Returned the secured area end address for Flash bank 2
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetFlashB2SecuredAreaEndAddress(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR11, SYSCFG_UR11_SAEND_BANK2));
}

/**
  * @brief  Get the Independent Watchdog 1 control mode (Software or Hardware)
  * @rmtoll UR11    IWDG1M    LL_SYSCFG_GetIWDG1ControlMode
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_IWDG1_SW_CONTROL_MODE
  *         @arg @ref LL_SYSCFG_IWDG1_HW_CONTROL_MODE
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetIWDG1ControlMode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR11, SYSCFG_UR11_IWDG1M));
}

#if defined (DUAL_CORE)
/**
  * @brief  Get the Independent Watchdog 2 control mode (Software or Hardware)
  * @rmtoll UR12    IWDG2M    LL_SYSCFG_GetIWDG2ControlMode
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_IWDG2_SW_CONTROL_MODE
  *         @arg @ref LL_SYSCFG_IWDG2_HW_CONTROL_MODE
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetIWDG2ControlMode(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR12, SYSCFG_UR12_IWDG2M));
}
#endif /* DUAL_CORE */

/**
  * @brief  Indicates the Secure mode status
  * @rmtoll UR12    SECURE    LL_SYSCFG_IsSecureModeEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsSecureModeEnabled(void)
{
  return ((READ_BIT(SYSCFG->UR12, SYSCFG_UR12_SECURE) == SYSCFG_UR12_SECURE) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if a reset is generated when D1 domain enters DStandby mode
  * @rmtoll UR13    D1SBRST   LL_SYSCFG_IsD1StandbyGenerateReset
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsD1StandbyGenerateReset(void)
{
  return ((READ_BIT(SYSCFG->UR13, SYSCFG_UR13_D1SBRST) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Get the secured DTCM RAM size
  * @rmtoll UR13    SDRS    LL_SYSCFG_GetSecuredDTCMSize
  * @retval Returned value can be one of the following values:
  *           @arg @ref LL_SYSCFG_DTCM_RAM_SIZE_2KB
  *           @arg @ref LL_SYSCFG_DTCM_RAM_SIZE_4KB
  *           @arg @ref LL_SYSCFG_DTCM_RAM_SIZE_8KB
  *           @arg @ref LL_SYSCFG_DTCM_RAM_SIZE_16KB
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetSecuredDTCMSize(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->UR13, SYSCFG_UR13_SDRS));
}

/**
  * @brief  Indicates if a reset is generated when D1 domain enters DStop mode
  * @rmtoll UR14    D1STPRST    LL_SYSCFG_IsD1StopGenerateReset
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsD1StopGenerateReset(void)
{
  return ((READ_BIT(SYSCFG->UR14, SYSCFG_UR14_D1STPRST) == 0U) ? 1UL : 0UL);
}

#if defined (DUAL_CORE)
/**
  * @brief  Indicates if a reset is generated when D2 domain enters DStandby mode
  * @rmtoll UR14    D2SBRST   LL_SYSCFG_IsD2StandbyGenerateReset
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsD2StandbyGenerateReset(void)
{
  return ((READ_BIT(SYSCFG->UR14, SYSCFG_UR14_D2SBRST) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if a reset is generated when D2 domain enters DStop mode
  * @rmtoll UR15    D2STPRST    LL_SYSCFG_IsD2StopGenerateReset
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsD2StopGenerateReset(void)
{
  return ((READ_BIT(SYSCFG->UR15, SYSCFG_UR15_D2STPRST) == 0U) ? 1UL : 0UL);
}
#endif /* DUAL_CORE */

/**
  * @brief  Indicates if the independent watchdog is frozen in Standby mode
  * @rmtoll UR15    FZIWDGSTB   LL_SYSCFG_IsIWDGFrozenInStandbyMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsIWDGFrozenInStandbyMode(void)
{
  return ((READ_BIT(SYSCFG->UR15, SYSCFG_UR15_FZIWDGSTB) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the independent watchdog is frozen in Stop mode
  * @rmtoll UR16    FZIWDGSTP   LL_SYSCFG_IsIWDGFrozenInStopMode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsIWDGFrozenInStopMode(void)
{
  return ((READ_BIT(SYSCFG->UR16, SYSCFG_UR16_FZIWDGSTP) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the device private key is programmed
  * @rmtoll UR16    PKP   LL_SYSCFG_IsPrivateKeyProgrammed
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsPrivateKeyProgrammed(void)
{
  return ((READ_BIT(SYSCFG->UR16, SYSCFG_UR16_PKP) == SYSCFG_UR16_PKP) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if the Product is working on the full voltage range or not
  * @rmtoll UR17    IOHSLV    LL_SYSCFG_IsActiveFlag_IOHSLV
  * @note   When the IOHSLV option bit is set the Product is working below 2.7 V.
  *         When the IOHSLV option bit is reset the Product is working on the
  *         full voltage range.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsActiveFlag_IOHSLV(void)
{
  return ((READ_BIT(SYSCFG->UR17, SYSCFG_UR17_IOHSLV) == SYSCFG_UR17_IOHSLV) ? 1UL : 0UL);
}
#endif /*SYSCFG_UR0_RDP*/

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_DBGMCU DBGMCU
  * @{
  */

/**
  * @brief  Return the device identifier
  * @rmtoll DBGMCU_IDCODE DEV_ID        LL_DBGMCU_GetDeviceID
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFF
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetDeviceID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID));
}

/**
  * @brief  Return the device revision identifier
  * @note This field indicates the revision of the device.
          For example, it is read as RevA -> 0x1000, Cat 2 revZ -> 0x1001
  * @rmtoll DBGMCU_IDCODE REV_ID        LL_DBGMCU_GetRevisionID
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetRevisionID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID) >> DBGMCU_IDCODE_REV_ID_Pos);
}

/**
  * @brief  Enable D1 Domain/CDomain debug during SLEEP mode
  * @rmtoll DBGMCU_CR    DBGSLEEP_D1/DBGSLEEP_CD     LL_DBGMCU_EnableD1DebugInSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD1DebugInSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}

/**
  * @brief  Disable D1 Domain/CDomain debug during SLEEP mode
  * @rmtoll DBGMCU_CR    DBGSLEEP_D1/DBGSLEEP_CD     LL_DBGMCU_DisableD1DebugInSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD1DebugInSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}

/**
  * @brief  Enable D1 Domain/CDomain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D1/DBGSLEEP_CD     LL_DBGMCU_EnableD1DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD1DebugInStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Disable D1 Domain/CDomain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D1/DBGSLEEP_CD     LL_DBGMCU_DisableD1DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD1DebugInStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Enable D1 Domain/CDomain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D1/DBGSLEEP_CD     LL_DBGMCU_EnableD1DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD1DebugInStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}

/**
  * @brief  Disable D1 Domain/CDomain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D1/DBGSLEEP_CD     LL_DBGMCU_DisableD1DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD1DebugInStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}

#if defined (DUAL_CORE)
/**
  * @brief  Enable D2 Domain debug during SLEEP mode
  * @rmtoll DBGMCU_CR    DBGSLEEP_D2     LL_DBGMCU_EnableD2DebugInSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD2DebugInSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD2);
}

/**
  * @brief  Disable D2 Domain debug during SLEEP mode
  * @rmtoll DBGMCU_CR    DBGSLEEP_D2     LL_DBGMCU_DisableD2DebugInSleepMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD2DebugInSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD2);
}

/**
  * @brief  Enable D2 Domain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D2     LL_DBGMCU_EnableD2DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD2DebugInStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD2);
}

/**
  * @brief  Disable D2 Domain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D2     LL_DBGMCU_DisableD2DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD2DebugInStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD2);
}

/**
  * @brief  Enable D2 Domain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D2     LL_DBGMCU_EnableD2DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD2DebugInStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD2);
}

/**
  * @brief  Disable D2 Domain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D2     LL_DBGMCU_DisableD2DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD2DebugInStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD2);
}
#endif /* DUAL_CORE */


/**
  * @brief  Enable D3 Domain/SRDomain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D3/DBGSTOP_SRD    LL_DBGMCU_EnableD3DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD3DebugInStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}

/**
  * @brief  Disable D3 Domain/SRDomain debug during STOP mode
  * @rmtoll DBGMCU_CR    DBGSTOP_D3/DBGSTOP_SRD     LL_DBGMCU_DisableD3DebugInStopMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD3DebugInStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}

/**
  * @brief  Enable D3 Domain/SRDomain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D3/DBGSTBY_SRD     LL_DBGMCU_EnableD3DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD3DebugInStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Disable D3 Domain/SRDomain debug during STANDBY mode
  * @rmtoll DBGMCU_CR    DBGSTBY_D3/DBGSTBY_SRD     LL_DBGMCU_DisableD3DebugInStandbyMode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD3DebugInStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Enable the trace port clock
  * @rmtoll DBGMCU_CR    TRACECKEN     LL_DBGMCU_EnableTracePortClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableTracePortClock(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TRACECKEN);
}

/**
  * @brief  Disable the trace port clock
  * @rmtoll DBGMCU_CR    TRACECKEN     LL_DBGMCU_DisableTracePortClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableTracePortClock(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TRACECKEN);
}

/**
  * @brief  Enable the Domain1/CDomain debug clock enable
  * @rmtoll DBGMCU_CR    CKD1EN/CKCDEN     LL_DBGMCU_EnableD1DebugClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD1DebugClock(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_CKD1EN);
}

/**
  * @brief  Disable the Domain1/CDomain debug clock enable
  * @rmtoll DBGMCU_CR    CKD1EN/CKCDEN     LL_DBGMCU_DisableD1DebugClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD1DebugClock(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_CKD1EN);
}

/**
  * @brief  Enable the Domain3/SRDomain debug clock enable
  * @rmtoll DBGMCU_CR    CKD3EN/CKSRDEN     LL_DBGMCU_EnableD3DebugClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableD3DebugClock(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_CKD3EN);
}

/**
  * @brief  Disable the Domain3/SRDomain debug clock enable
  * @rmtoll DBGMCU_CR    CKD3EN/CKSRDEN    LL_DBGMCU_DisableD3DebugClock
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableD3DebugClock(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_CKD3EN);
}

#define LL_DBGMCU_TRGIO_INPUT_DIRECTION   0U
#define LL_DBGMCU_TRGIO_OUTPUT_DIRECTION  DBGMCU_CR_DBG_TRGOEN
/**
  * @brief  Set the direction of the bi-directional trigger pin TRGIO
  * @rmtoll DBGMCU_CR    TRGOEN    LL_DBGMCU_SetExternalTriggerPinDirection\n
  * @param  PinDirection This parameter can be one of the following values:
  *         @arg @ref LL_DBGMCU_TRGIO_INPUT_DIRECTION
  *         @arg @ref LL_DBGMCU_TRGIO_OUTPUT_DIRECTION
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_SetExternalTriggerPinDirection(uint32_t PinDirection)
{
  MODIFY_REG(DBGMCU->CR, DBGMCU_CR_DBG_TRGOEN, PinDirection);
}

/**
  * @brief  Get the direction of the bi-directional trigger pin TRGIO
  * @rmtoll DBGMCU_CR    TRGOEN    LL_DBGMCU_GetExternalTriggerPinDirection\n
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_DBGMCU_TRGIO_INPUT_DIRECTION
  *         @arg @ref LL_DBGMCU_TRGIO_OUTPUT_DIRECTION
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetExternalTriggerPinDirection(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TRGOEN));
}

/**
  * @brief  Freeze APB1 group1 peripherals
  * @rmtoll DBGMCU_APB1LFZ1   TIM2      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM3      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM4      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM5      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM6      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM7      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM12     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM13     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM14     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   LPTIM1    LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C1      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C2      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C3      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM4_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM5_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM6_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM7_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM12_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM13_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C3_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB1LFZ1, Periphs);
}

/**
  * @brief  Unfreeze APB1 peripherals (group1 peripherals)
  * @rmtoll DBGMCU_APB1LFZ1   TIM2      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM3      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM4      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM5      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM6      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM7      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM12     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM13     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   TIM14     LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   LPTIM1    LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C1      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C2      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  *         DBGMCU_APB1LFZ1   I2C3      LL_DBGMCU_APB1_GRP1_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM3_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM4_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM5_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM6_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM7_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM12_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM13_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_I2C3_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB1LFZ1, Periphs);
}

/**
  * @brief  Freeze APB1 group2 peripherals
  * @rmtoll DBGMCU_APB1HFZ1   FDCAN   LL_DBGMCU_APB1_GRP2_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_FDCAN_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB1HFZ1, Periphs);
}

/**
  * @brief  Unfreeze APB1 group2 peripherals
  * @rmtoll DBGMCU_APB1HFZ1   FDCAN   LL_DBGMCU_APB1_GRP2_UnFreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_FDCAN_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB1HFZ1, Periphs);
}

/**
  * @brief  Freeze APB2 peripherals
  * @rmtoll DBGMCU_APB2FZ1    TIM1     LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM8     LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM15    LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM16    LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM17    LL_DBGMCU_APB2_GRP1_FreezePeriph
  *         DBGMCU_APB2FZ1    HRTIM    LL_DBGMCU_APB2_GRP1_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM8_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM15_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM17_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_HRTIM_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB2_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB2FZ1, Periphs);
}

/**
  * @brief  Unfreeze APB2 peripherals
  * @rmtoll DBGMCU_APB2FZ1    TIM1     LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM8     LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM15    LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM16    LL_DBGMCU_APB2_GRP1_FreezePeriph\n
  *         DBGMCU_APB2FZ1    TIM17    LL_DBGMCU_APB2_GRP1_FreezePeriph
  *         DBGMCU_APB2FZ1    HRTIM    LL_DBGMCU_APB2_GRP1_FreezePeriph
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM8_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM15_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_TIM17_STOP
  *         @arg @ref LL_DBGMCU_APB2_GRP1_HRTIM_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB2_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB2FZ1, Periphs);
}

/**
  * @brief  Freeze APB3 peripherals
  * @rmtoll DBGMCU_APB3FZ1    WWDG1     LL_DBGMCU_APB3_GRP1_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB3_GRP1_WWDG1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB3_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB3FZ1, Periphs);
}

/**
  * @brief  Unfreeze APB3 peripherals
  * @rmtoll DBGMCU_APB3FZ1    WWDG1     LL_DBGMCU_APB3_GRP1_UnFreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB3_GRP1_WWDG1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB3_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB3FZ1, Periphs);
}

/**
  * @brief  Freeze APB4 peripherals
  * @rmtoll DBGMCU_APB4FZ1    I2C4      LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM2    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM3    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM4    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM5    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    RTC       LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    WDGLSD1   LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB4_GRP1_I2C4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM2_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM3_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM5_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_IWDG1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB4_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APB4FZ1, Periphs);
}

/**
  * @brief  Unfreeze APB4 peripherals
  * @rmtoll DBGMCU_APB4FZ1    I2C4      LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM2    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM3    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM4    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    LPTIM5    LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    RTC       LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @rmtoll DBGMCU_APB4FZ1    WDGLSD1   LL_DBGMCU_APB4_GRP1_FreezePeriph\n
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB4_GRP1_I2C4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM2_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM3_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM4_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_LPTIM5_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB4_GRP1_IWDG1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB4_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APB4FZ1, Periphs);
}
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_FLASH FLASH
  * @{
  */

/**
  * @brief  Set FLASH Latency
  * @rmtoll FLASH_ACR    LATENCY       LL_FLASH_SetLatency
  * @param  Latency This parameter can be one of the following values:
  *         @arg @ref LL_FLASH_LATENCY_0
  *         @arg @ref LL_FLASH_LATENCY_1
  *         @arg @ref LL_FLASH_LATENCY_2
  *         @arg @ref LL_FLASH_LATENCY_3
  *         @arg @ref LL_FLASH_LATENCY_4
  *         @arg @ref LL_FLASH_LATENCY_5
  *         @arg @ref LL_FLASH_LATENCY_6
  *         @arg @ref LL_FLASH_LATENCY_7
  * @retval None
  */
__STATIC_INLINE void LL_FLASH_SetLatency(uint32_t Latency)
{
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, Latency);
}

/**
  * @brief  Get FLASH Latency
  * @rmtoll FLASH_ACR    LATENCY       LL_FLASH_GetLatency
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_FLASH_LATENCY_0
  *         @arg @ref LL_FLASH_LATENCY_1
  *         @arg @ref LL_FLASH_LATENCY_2
  *         @arg @ref LL_FLASH_LATENCY_3
  *         @arg @ref LL_FLASH_LATENCY_4
  *         @arg @ref LL_FLASH_LATENCY_5
  *         @arg @ref LL_FLASH_LATENCY_6
  *         @arg @ref LL_FLASH_LATENCY_7
  */
__STATIC_INLINE uint32_t LL_FLASH_GetLatency(void)
{
  return (uint32_t)(READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY));
}

/**
  * @}
  */

#if defined(DUAL_CORE)
/** @defgroup SYSTEM_LL_EF_ART ART
  * @{
  */

/**
  * @brief  Enable the Cortex-M4 ART cache.
  * @rmtoll ART_CTR    EN   LL_ART_Enable
  * @retval None
  */
__STATIC_INLINE void LL_ART_Enable(void)
{
 SET_BIT(ART->CTR, ART_CTR_EN);
}

/**
  * @brief  Disable the Cortex-M4 ART cache.
  * @rmtoll ART_CTR    EN   LL_ART_Disable
  * @retval None
  */
__STATIC_INLINE void LL_ART_Disable(void)
{
 CLEAR_BIT(ART->CTR, ART_CTR_EN);
}

/**
  * @brief  Check if the Cortex-M4 ART cache is enabled
  * @rmtoll ART_CTR   EN    LL_ART_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ART_IsEnabled(void)
{
  return ((READ_BIT(ART->CTR, ART_CTR_EN) == ART_CTR_EN) ? 1UL : 0UL);
}

/**
  * @brief  Set the Cortex-M4 ART cache Base Address.
  * @rmtoll ART_CTR    PCACHEADDR   LL_ART_SetBaseAddress
  * @param  BaseAddress Specifies the Base address of 1 Mbyte address page (cacheable page)
            from which the ART accelerator loads code to the cache.
  * @retval None
  */
__STATIC_INLINE void LL_ART_SetBaseAddress(uint32_t BaseAddress)
{
 MODIFY_REG(ART->CTR, ART_CTR_PCACHEADDR, (((BaseAddress) >> 12U) & 0x000FFF00UL));
}

/**
  * @brief  Get the Cortex-M4 ART cache Base Address.
  * @rmtoll ART_CTR    PCACHEADDR   LL_ART_GetBaseAddress
  * @retval the Base address of 1 Mbyte address page (cacheable page)
            from which the ART accelerator loads code to the cache
  */
__STATIC_INLINE uint32_t LL_ART_GetBaseAddress(void)
{
  return (uint32_t)(READ_BIT(ART->CTR, ART_CTR_PCACHEADDR) << 12U);
}
#endif /* DUAL_CORE */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined (FLASH) || defined (SYSCFG) || defined (DBGMCU) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_LL_SYSTEM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
