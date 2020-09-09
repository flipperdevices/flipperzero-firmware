/**
  ******************************************************************************
  * @file    stm32h7xx_hal.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the HAL
  *          module driver.
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
#ifndef STM32H7xx_HAL_H
#define STM32H7xx_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_conf.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup HAL_TICK_FREQ Tick Frequency
  * @{
  */
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup REV_ID device revision ID
  * @{
  */
#define REV_ID_Y ((uint32_t)0x1003)  /*!< STM32H7 rev.Y */
#define REV_ID_B ((uint32_t)0x2000)  /*!< STM32H7 rev.B */
#define REV_ID_X ((uint32_t)0x2001)  /*!< STM32H7 rev.X */
#define REV_ID_V ((uint32_t)0x2003)  /*!< STM32H7 rev.V */

/**
  * @}
  */

/** @defgroup SYSCFG_VREFBUF_VoltageScale VREFBUF Voltage Scale
  * @{
  */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE0   VREFBUF_CSR_VRS_OUT2   /*!< Voltage reference scale 0 (VREF_OUT2) */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE1   VREFBUF_CSR_VRS_OUT1   /*!< Voltage reference scale 1 (VREF_OUT1) */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE2   VREFBUF_CSR_VRS_OUT4   /*!< Voltage reference scale 2 (VREF_OUT4) */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE3   VREFBUF_CSR_VRS_OUT3   /*!< Voltage reference scale 3 (VREF_OUT3) */


#define IS_SYSCFG_VREFBUF_VOLTAGE_SCALE(__SCALE__)  (((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE0) || \
                                                     ((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE1) || \
                                                     ((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE2) || \
                                                     ((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE3))


/**
  * @}
  */

/** @defgroup SYSCFG_VREFBUF_HighImpedance VREFBUF High Impedance
  * @{
  */
#define SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE  ((uint32_t)0x00000000) /*!< VREF_plus pin is internally connected to Voltage reference buffer output */
#define SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE   VREFBUF_CSR_HIZ        /*!< VREF_plus pin is high impedance */

#define IS_SYSCFG_VREFBUF_HIGH_IMPEDANCE(__VALUE__)  (((__VALUE__) == SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE) || \
                                                      ((__VALUE__) == SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE))

#define IS_SYSCFG_VREFBUF_TRIMMING(__VALUE__)  (((__VALUE__) > 0UL) && ((__VALUE__) <= VREFBUF_CCR_TRIM))

/**
  * @}
  */

#if !defined(SYSCFG_PMCR_BOOSTEN)
/** @defgroup SYSCFG_FastModePlus_GPIO Fast-mode Plus on GPIO
  * @{
  */

/** @brief  Fast-mode Plus driving capability on a specific GPIO
  */
#define SYSCFG_FASTMODEPLUS_PB6        SYSCFG_PMCR_I2C_PB6_FMP  /*!< Enable Fast-mode Plus on PB6 */
#define SYSCFG_FASTMODEPLUS_PB7        SYSCFG_PMCR_I2C_PB7_FMP  /*!< Enable Fast-mode Plus on PB7 */
#define SYSCFG_FASTMODEPLUS_PB8        SYSCFG_PMCR_I2C_PB8_FMP  /*!< Enable Fast-mode Plus on PB8 */
#define SYSCFG_FASTMODEPLUS_PB9        SYSCFG_PMCR_I2C_PB9_FMP  /*!< Enable Fast-mode Plus on PB9 */

#define IS_SYSCFG_FASTMODEPLUS(__PIN__) ((((__PIN__) & SYSCFG_FASTMODEPLUS_PB6) == SYSCFG_FASTMODEPLUS_PB6) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB7) == SYSCFG_FASTMODEPLUS_PB7) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB8) == SYSCFG_FASTMODEPLUS_PB8) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB9) == SYSCFG_FASTMODEPLUS_PB9))

#endif /* ! SYSCFG_PMCR_BOOSTEN */
/**
  * @}
  */

/** @defgroup SYSCFG_Ethernet_Config  Ethernet Config
  * @{
  */
#define SYSCFG_ETH_MII                      ((uint32_t)0x00000000)  /*!< Select the Media Independent Interface */
#define SYSCFG_ETH_RMII                     SYSCFG_PMCR_EPIS_SEL_2  /*!< Select the Reduced Media Independent Interface */

#define IS_SYSCFG_ETHERNET_CONFIG(CONFIG) (((CONFIG) == SYSCFG_ETH_MII)        || \
                                          ((CONFIG) == SYSCFG_ETH_RMII))

/**
  * @}
  */


/** @defgroup SYSCFG_Analog_Switch_Config  Analog Switch Config
  * @{
  */
#define SYSCFG_SWITCH_PA0                       SYSCFG_PMCR_PA0SO  /*!< Select PA0 analog switch */
#define SYSCFG_SWITCH_PA1                       SYSCFG_PMCR_PA1SO  /*!< Select PA1 analog switch */
#define SYSCFG_SWITCH_PC2                       SYSCFG_PMCR_PC2SO  /*!< Select PC2 analog switch */
#define SYSCFG_SWITCH_PC3                       SYSCFG_PMCR_PC3SO  /*!< Select PC3 analog switch */


#define IS_SYSCFG_ANALOG_SWITCH(SWITCH)    ((((SWITCH) & SYSCFG_SWITCH_PA0) == SYSCFG_SWITCH_PA0)|| \
                                           (((SWITCH) & SYSCFG_SWITCH_PA1) == SYSCFG_SWITCH_PA1) || \
                                           (((SWITCH) & SYSCFG_SWITCH_PC2) == SYSCFG_SWITCH_PC2) || \
                                           (((SWITCH) & SYSCFG_SWITCH_PC3) == SYSCFG_SWITCH_PC3))


#define SYSCFG_SWITCH_PA0_OPEN                       SYSCFG_PMCR_PA0SO       /*!< PA0 analog switch opened */
#define SYSCFG_SWITCH_PA0_CLOSE                      ((uint32_t)0x00000000)  /*!< PA0 analog switch closed */
#define SYSCFG_SWITCH_PA1_OPEN                       SYSCFG_PMCR_PA1SO       /*!< PA1 analog switch opened */
#define SYSCFG_SWITCH_PA1_CLOSE                      ((uint32_t)0x00000000)  /*!< PA1 analog switch closed*/
#define SYSCFG_SWITCH_PC2_OPEN                       SYSCFG_PMCR_PC2SO       /*!< PC2 analog switch opened */
#define SYSCFG_SWITCH_PC2_CLOSE                      ((uint32_t)0x00000000)  /*!< PC2 analog switch closed */
#define SYSCFG_SWITCH_PC3_OPEN                       SYSCFG_PMCR_PC3SO       /*!< PC3 analog switch opened */
#define SYSCFG_SWITCH_PC3_CLOSE                      ((uint32_t)0x00000000)  /*!< PC3 analog switch closed */

#define IS_SYSCFG_SWITCH_STATE(STATE)      ((((STATE) & SYSCFG_SWITCH_PA0_OPEN) == SYSCFG_SWITCH_PA0_OPEN)    || \
                                           (((STATE) & SYSCFG_SWITCH_PA0_CLOSE) == SYSCFG_SWITCH_PA0_CLOSE)   || \
                                           (((STATE) & SYSCFG_SWITCH_PA1_OPEN) == SYSCFG_SWITCH_PA1_OPEN)     || \
                                           (((STATE) & SYSCFG_SWITCH_PA1_CLOSE) == SYSCFG_SWITCH_PA1_CLOSE)   || \
                                           (((STATE) & SYSCFG_SWITCH_PC2_OPEN) == SYSCFG_SWITCH_PC2_OPEN)     || \
                                           (((STATE) & SYSCFG_SWITCH_PC2_CLOSE) == SYSCFG_SWITCH_PC2_CLOSE)   || \
                                           (((STATE) & SYSCFG_SWITCH_PC3_OPEN) == SYSCFG_SWITCH_PC3_OPEN)     || \
                                           (((STATE) & SYSCFG_SWITCH_PC3_CLOSE) == SYSCFG_SWITCH_PC3_CLOSE))
/**
  * @}
  */

/** @defgroup SYSCFG_Boot_Config  Boot Config
  * @{
  */
#define SYSCFG_BOOT_ADDR0                    ((uint32_t)0x00000000)  /*!< Select Boot address0 */
#define SYSCFG_BOOT_ADDR1                    ((uint32_t)0x00000001)  /*!< Select Boot address1 */

#define IS_SYSCFG_BOOT_REGISTER(REGISTER) (((REGISTER) == SYSCFG_BOOT_ADDR0)|| \
                                          ((REGISTER) == SYSCFG_BOOT_ADDR1))

#define IS_SYSCFG_BOOT_ADDRESS(ADDRESS) ((ADDRESS) < PERIPH_BASE)

/**
  * @}
  */


/** @defgroup SYSCFG_IOCompenstionCell_Config  IOCompenstionCell Config
  * @{
  */
#define SYSCFG_CELL_CODE                    ((uint32_t)0x00000000)  /*!< Select Code from the cell */
#define SYSCFG_REGISTER_CODE                 SYSCFG_CCCSR_CS        /*!< Code from the SYSCFG compensation cell code register */

#define IS_SYSCFG_CODE_SELECT(SELECT) (((SELECT) == SYSCFG_CELL_CODE)|| \
                                        ((SELECT) == SYSCFG_REGISTER_CODE))

#define IS_SYSCFG_CODE_CONFIG(CONFIG) ((CONFIG) < (0x10UL))

/**
  * @}
  */




/** @defgroup EXTI_Event_Input_Config  Event Input Config
  * @{
  */

#define EXTI_MODE_IT          ((uint32_t)0x00010000)
#define EXTI_MODE_EVT         ((uint32_t)0x00020000)
#define EXTI_RISING_EDGE      ((uint32_t)0x00100000)
#define EXTI_FALLING_EDGE     ((uint32_t)0x00200000)

#define IS_EXTI_EDGE_LINE(EDGE) (((EDGE) == EXTI_RISING_EDGE) || ((EDGE) == EXTI_FALLING_EDGE))
#define IS_EXTI_MODE_LINE(MODE) (((MODE) == EXTI_MODE_IT) || ((MODE) == EXTI_MODE_EVT))

#define EXTI_LINE0       ((uint32_t)0x00)  /*!< External interrupt LINE 0  */
#define EXTI_LINE1       ((uint32_t)0x01)  /*!< External interrupt LINE 1  */
#define EXTI_LINE2       ((uint32_t)0x02)  /*!< External interrupt LINE 2  */
#define EXTI_LINE3       ((uint32_t)0x03)  /*!< External interrupt LINE 3  */
#define EXTI_LINE4       ((uint32_t)0x04)  /*!< External interrupt LINE 4  */
#define EXTI_LINE5       ((uint32_t)0x05)  /*!< External interrupt LINE 5  */
#define EXTI_LINE6       ((uint32_t)0x06)  /*!< External interrupt LINE 6  */
#define EXTI_LINE7       ((uint32_t)0x07)  /*!< External interrupt LINE 7  */
#define EXTI_LINE8       ((uint32_t)0x08)  /*!< External interrupt LINE 8  */
#define EXTI_LINE9       ((uint32_t)0x09)  /*!< External interrupt LINE 9  */
#define EXTI_LINE10      ((uint32_t)0x0A)  /*!< External interrupt LINE 10 */
#define EXTI_LINE11      ((uint32_t)0x0B)  /*!< External interrupt LINE 11 */
#define EXTI_LINE12      ((uint32_t)0x0C)  /*!< External interrupt LINE 12 */
#define EXTI_LINE13      ((uint32_t)0x0D)  /*!< External interrupt LINE 13 */
#define EXTI_LINE14      ((uint32_t)0x0E)  /*!< External interrupt LINE 14 */
#define EXTI_LINE15      ((uint32_t)0x0F)  /*!< External interrupt LINE 15 */
#define EXTI_LINE16      ((uint32_t)0x10)
#define EXTI_LINE17      ((uint32_t)0x11)
#define EXTI_LINE18      ((uint32_t)0x12)
#define EXTI_LINE19      ((uint32_t)0x13)
#define EXTI_LINE20      ((uint32_t)0x14)
#define EXTI_LINE21      ((uint32_t)0x15)
#define EXTI_LINE22      ((uint32_t)0x16)
#define EXTI_LINE23      ((uint32_t)0x17)
#define EXTI_LINE24      ((uint32_t)0x18)
#define EXTI_LINE25      ((uint32_t)0x19)
#define EXTI_LINE26      ((uint32_t)0x1A)
#define EXTI_LINE27      ((uint32_t)0x1B)
#define EXTI_LINE28      ((uint32_t)0x1C)
#define EXTI_LINE29      ((uint32_t)0x1D)
#define EXTI_LINE30      ((uint32_t)0x1E)
#define EXTI_LINE31      ((uint32_t)0x1F)
#define EXTI_LINE32      ((uint32_t)0x20)
#define EXTI_LINE33      ((uint32_t)0x21)
#define EXTI_LINE34      ((uint32_t)0x22)
#define EXTI_LINE35      ((uint32_t)0x23)
#define EXTI_LINE36      ((uint32_t)0x24)
#define EXTI_LINE37      ((uint32_t)0x25)
#define EXTI_LINE38      ((uint32_t)0x26)
#define EXTI_LINE39      ((uint32_t)0x27)

#define EXTI_LINE40      ((uint32_t)0x28)
#define EXTI_LINE41      ((uint32_t)0x29)
#define EXTI_LINE42      ((uint32_t)0x2A)
#define EXTI_LINE43      ((uint32_t)0x2B)
#define EXTI_LINE44      ((uint32_t)0x2C)  /* Not available in all family lines */
/* EXTI_LINE45 Reserved */
#if defined(DUAL_CORE)
#define EXTI_LINE46      ((uint32_t)0x2E)
#else
/* EXTI_LINE46 Reserved */
#endif /* DUAL_CORE */
#define EXTI_LINE47      ((uint32_t)0x2F)
#define EXTI_LINE48      ((uint32_t)0x30)
#define EXTI_LINE49      ((uint32_t)0x31)
#define EXTI_LINE50      ((uint32_t)0x32)
#define EXTI_LINE51      ((uint32_t)0x33)
#define EXTI_LINE52      ((uint32_t)0x34)
#define EXTI_LINE53      ((uint32_t)0x35)
#define EXTI_LINE54      ((uint32_t)0x36)
#define EXTI_LINE55      ((uint32_t)0x37)
#define EXTI_LINE56      ((uint32_t)0x38)
#define EXTI_LINE57      ((uint32_t)0x39)
#define EXTI_LINE58      ((uint32_t)0x3A)
#define EXTI_LINE59      ((uint32_t)0x3B)
#define EXTI_LINE60      ((uint32_t)0x3C)
#define EXTI_LINE61      ((uint32_t)0x3D)
#define EXTI_LINE62      ((uint32_t)0x3E)
#define EXTI_LINE63      ((uint32_t)0x3F)
#define EXTI_LINE64      ((uint32_t)0x40)
#define EXTI_LINE65      ((uint32_t)0x41)
#define EXTI_LINE66      ((uint32_t)0x42)
#define EXTI_LINE67      ((uint32_t)0x43)
#define EXTI_LINE68      ((uint32_t)0x44)
#define EXTI_LINE69      ((uint32_t)0x45)
#define EXTI_LINE70      ((uint32_t)0x46)
#define EXTI_LINE71      ((uint32_t)0x47)
#define EXTI_LINE72      ((uint32_t)0x48)
#define EXTI_LINE73      ((uint32_t)0x49)
#define EXTI_LINE74      ((uint32_t)0x4A)
#define EXTI_LINE75      ((uint32_t)0x4B)  /* Not available in all family lines */
#define EXTI_LINE76      ((uint32_t)0x4C)  /* Not available in all family lines */
#if defined(DUAL_CORE)
#define EXTI_LINE77      ((uint32_t)0x4D)
#define EXTI_LINE78      ((uint32_t)0x4E)
#define EXTI_LINE79      ((uint32_t)0x4F)
#define EXTI_LINE80      ((uint32_t)0x50)
#else
/* EXTI_LINE77 Reserved */
/* EXTI_LINE78 Reserved */
/* EXTI_LINE79 Reserved */
/* EXTI_LINE80 Reserved */
#endif /* DUAL_CORE */
/* EXTI_LINE81   Reserved */
#if defined(DUAL_CORE)
#define EXTI_LINE82      ((uint32_t)0x52)
#else
/* EXTI_LINE82 Reserved */
#endif /* DUAL_CORE */
/* EXTI_LINE83   Reserved */
#if defined(DUAL_CORE)
#define EXTI_LINE84      ((uint32_t)0x54)
#else
/* EXTI_LINE84 Reserved */
#endif /* DUAL_CORE */
#define EXTI_LINE85      ((uint32_t)0x55)
#define EXTI_LINE86      ((uint32_t)0x56)  /* Not available in all family lines */
#define EXTI_LINE87      ((uint32_t)0x57)
#define EXTI_LINE88      ((uint32_t)0x58)  /* Not available in all family lines */

#if defined(DUAL_CORE)
#define IS_HAL_EXTI_CONFIG_LINE(LINE) (((LINE) == EXTI_LINE0) || ((LINE) == EXTI_LINE1) || \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11)  || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13)  || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15)  || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17)  || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19)  || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21)  || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE51)  || \
                                ((LINE) == EXTI_LINE82) || ((LINE) == EXTI_LINE84)  || \
                                ((LINE) == EXTI_LINE85) || ((LINE) == EXTI_LINE86))
#else
#define IS_HAL_EXTI_CONFIG_LINE(LINE) (((LINE) == EXTI_LINE0) || ((LINE) == EXTI_LINE1)|| \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE51) || \
                                ((LINE) == EXTI_LINE85) || ((LINE) == EXTI_LINE86))
#endif /* DUAL_CORE */

#if defined(DUAL_CORE)
#define IS_EXTI_ALL_LINE(LINE) (((LINE) == EXTI_LINE0)  || ((LINE) == EXTI_LINE1)  || \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)  || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)  || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)  || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)  || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE22) || ((LINE) == EXTI_LINE23) || \
                                ((LINE) == EXTI_LINE24) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE26) || ((LINE) == EXTI_LINE27) || \
                                ((LINE) == EXTI_LINE28) || ((LINE) == EXTI_LINE29) || \
                                ((LINE) == EXTI_LINE30) || ((LINE) == EXTI_LINE31) || \
                                ((LINE) == EXTI_LINE32) || ((LINE) == EXTI_LINE33) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE36) || ((LINE) == EXTI_LINE37) || \
                                ((LINE) == EXTI_LINE38) || ((LINE) == EXTI_LINE39) || \
                                ((LINE) == EXTI_LINE40) || ((LINE) == EXTI_LINE41) || \
                                ((LINE) == EXTI_LINE42) || ((LINE) == EXTI_LINE43) || \
                                ((LINE) == EXTI_LINE44) || ((LINE) == EXTI_LINE46) || \
                                ((LINE) == EXTI_LINE47) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53) || ((LINE) == EXTI_LINE54) || \
                                ((LINE) == EXTI_LINE55) || ((LINE) == EXTI_LINE56) || \
                                ((LINE) == EXTI_LINE57) || ((LINE) == EXTI_LINE58) || \
                                ((LINE) == EXTI_LINE59) || ((LINE) == EXTI_LINE60) || \
                                ((LINE) == EXTI_LINE61) || ((LINE) == EXTI_LINE62) || \
                                ((LINE) == EXTI_LINE63) || ((LINE) == EXTI_LINE64) || \
                                ((LINE) == EXTI_LINE65) || ((LINE) == EXTI_LINE66) || \
                                ((LINE) == EXTI_LINE67) || ((LINE) == EXTI_LINE68) || \
                                ((LINE) == EXTI_LINE69) || ((LINE) == EXTI_LINE70) || \
                                ((LINE) == EXTI_LINE71) || ((LINE) == EXTI_LINE72) || \
                                ((LINE) == EXTI_LINE73) || ((LINE) == EXTI_LINE74) || \
                                ((LINE) == EXTI_LINE75) || ((LINE) == EXTI_LINE76) || \
                                ((LINE) == EXTI_LINE77) || ((LINE) == EXTI_LINE79) || \
                                ((LINE) == EXTI_LINE84) || ((LINE) == EXTI_LINE85) || \
                                ((LINE) == EXTI_LINE86) || ((LINE) == EXTI_LINE87) || \
                                ((LINE) == EXTI_LINE78) || \
                                ((LINE) == EXTI_LINE80) || ((LINE) == EXTI_LINE82))
#else
#define IS_EXTI_ALL_LINE(LINE) (((LINE) == EXTI_LINE0)  || ((LINE) == EXTI_LINE1)  || \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)  || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)  || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)  || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)  || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE22) || ((LINE) == EXTI_LINE23) || \
                                ((LINE) == EXTI_LINE24) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE26) || ((LINE) == EXTI_LINE27) || \
                                ((LINE) == EXTI_LINE28) || ((LINE) == EXTI_LINE29) || \
                                ((LINE) == EXTI_LINE30) || ((LINE) == EXTI_LINE31) || \
                                ((LINE) == EXTI_LINE32) || ((LINE) == EXTI_LINE33) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE36) || ((LINE) == EXTI_LINE37) || \
                                ((LINE) == EXTI_LINE38) || ((LINE) == EXTI_LINE39) || \
                                ((LINE) == EXTI_LINE40) || ((LINE) == EXTI_LINE41) || \
                                ((LINE) == EXTI_LINE42) || ((LINE) == EXTI_LINE43) || \
                                ((LINE) == EXTI_LINE44) || \
                                ((LINE) == EXTI_LINE47) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53) || ((LINE) == EXTI_LINE54) || \
                                ((LINE) == EXTI_LINE55) || ((LINE) == EXTI_LINE56) || \
                                ((LINE) == EXTI_LINE57) || ((LINE) == EXTI_LINE58) || \
                                ((LINE) == EXTI_LINE59) || ((LINE) == EXTI_LINE60) || \
                                ((LINE) == EXTI_LINE61) || ((LINE) == EXTI_LINE62) || \
                                ((LINE) == EXTI_LINE63) || ((LINE) == EXTI_LINE64) || \
                                ((LINE) == EXTI_LINE65) || ((LINE) == EXTI_LINE66) || \
                                ((LINE) == EXTI_LINE67) || ((LINE) == EXTI_LINE68) || \
                                ((LINE) == EXTI_LINE69) || ((LINE) == EXTI_LINE70) || \
                                ((LINE) == EXTI_LINE71) || ((LINE) == EXTI_LINE72) || \
                                ((LINE) == EXTI_LINE73) || ((LINE) == EXTI_LINE74) || \
                                ((LINE) == EXTI_LINE75) || ((LINE) == EXTI_LINE76) || \
                                ((LINE) == EXTI_LINE85) || \
                                ((LINE) == EXTI_LINE86) || ((LINE) == EXTI_LINE87) || \
                                ((LINE) == EXTI_LINE88))
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
#define IS_EXTI_D1_LINE(LINE) (((LINE) == EXTI_LINE0)  || ((LINE) == EXTI_LINE1)  || \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)  || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)  || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)  || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)  || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE22) || ((LINE) == EXTI_LINE23) || \
                                ((LINE) == EXTI_LINE24) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE26) || ((LINE) == EXTI_LINE27) || \
                                ((LINE) == EXTI_LINE28) || ((LINE) == EXTI_LINE29) || \
                                ((LINE) == EXTI_LINE30) || ((LINE) == EXTI_LINE31) || \
                                ((LINE) == EXTI_LINE32) || ((LINE) == EXTI_LINE33) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE36) || ((LINE) == EXTI_LINE37) || \
                                ((LINE) == EXTI_LINE38) || ((LINE) == EXTI_LINE39) || \
                                ((LINE) == EXTI_LINE40) || ((LINE) == EXTI_LINE41) || \
                                ((LINE) == EXTI_LINE42) || ((LINE) == EXTI_LINE43) || \
                                ((LINE) == EXTI_LINE44) || ((LINE) == EXTI_LINE46) || \
                                ((LINE) == EXTI_LINE47) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53) || ((LINE) == EXTI_LINE54) || \
                                ((LINE) == EXTI_LINE55) || ((LINE) == EXTI_LINE56) || \
                                ((LINE) == EXTI_LINE57) || ((LINE) == EXTI_LINE58) || \
                                ((LINE) == EXTI_LINE59) || ((LINE) == EXTI_LINE60) || \
                                ((LINE) == EXTI_LINE61) || ((LINE) == EXTI_LINE62) || \
                                ((LINE) == EXTI_LINE63) || ((LINE) == EXTI_LINE64) || \
                                ((LINE) == EXTI_LINE65) || ((LINE) == EXTI_LINE66) || \
                                ((LINE) == EXTI_LINE67) || ((LINE) == EXTI_LINE68) || \
                                ((LINE) == EXTI_LINE69) || ((LINE) == EXTI_LINE70) || \
                                ((LINE) == EXTI_LINE71) || ((LINE) == EXTI_LINE72) || \
                                ((LINE) == EXTI_LINE73) || ((LINE) == EXTI_LINE74) || \
                                ((LINE) == EXTI_LINE75) || ((LINE) == EXTI_LINE76) || \
                                ((LINE) == EXTI_LINE77) || ((LINE) == EXTI_LINE79) || \
                                ((LINE) == EXTI_LINE84) || ((LINE) == EXTI_LINE85) || \
                                ((LINE) == EXTI_LINE86) || ((LINE) == EXTI_LINE87))
#else
#define IS_EXTI_D1_LINE(LINE) (((LINE) == EXTI_LINE0)  || ((LINE) == EXTI_LINE1)  || \
                                ((LINE) == EXTI_LINE2)  || ((LINE) == EXTI_LINE3)  || \
                                ((LINE) == EXTI_LINE4)  || ((LINE) == EXTI_LINE5)  || \
                                ((LINE) == EXTI_LINE6)  || ((LINE) == EXTI_LINE7)  || \
                                ((LINE) == EXTI_LINE8)  || ((LINE) == EXTI_LINE9)  || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE22) || ((LINE) == EXTI_LINE23) || \
                                ((LINE) == EXTI_LINE24) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE26) || ((LINE) == EXTI_LINE27) || \
                                ((LINE) == EXTI_LINE28) || ((LINE) == EXTI_LINE29) || \
                                ((LINE) == EXTI_LINE30) || ((LINE) == EXTI_LINE31) || \
                                ((LINE) == EXTI_LINE32) || ((LINE) == EXTI_LINE33) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE36) || ((LINE) == EXTI_LINE37) || \
                                ((LINE) == EXTI_LINE38) || ((LINE) == EXTI_LINE39) || \
                                ((LINE) == EXTI_LINE40) || ((LINE) == EXTI_LINE41) || \
                                ((LINE) == EXTI_LINE42) || ((LINE) == EXTI_LINE43) || \
                                ((LINE) == EXTI_LINE44) || \
                                ((LINE) == EXTI_LINE47) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53) || ((LINE) == EXTI_LINE54) || \
                                ((LINE) == EXTI_LINE55) || ((LINE) == EXTI_LINE56) || \
                                ((LINE) == EXTI_LINE57) || ((LINE) == EXTI_LINE58) || \
                                ((LINE) == EXTI_LINE59) || ((LINE) == EXTI_LINE60) || \
                                ((LINE) == EXTI_LINE61) || ((LINE) == EXTI_LINE62) || \
                                ((LINE) == EXTI_LINE63) || ((LINE) == EXTI_LINE64) || \
                                ((LINE) == EXTI_LINE65) || ((LINE) == EXTI_LINE66) || \
                                ((LINE) == EXTI_LINE67) || ((LINE) == EXTI_LINE68) || \
                                ((LINE) == EXTI_LINE69) || ((LINE) == EXTI_LINE70) || \
                                ((LINE) == EXTI_LINE71) || ((LINE) == EXTI_LINE72) || \
                                ((LINE) == EXTI_LINE73) || ((LINE) == EXTI_LINE74) || \
                                ((LINE) == EXTI_LINE75) || ((LINE) == EXTI_LINE76) || \
                                ((LINE) == EXTI_LINE85) || \
                                ((LINE) == EXTI_LINE86) || ((LINE) == EXTI_LINE87) || \
                                ((LINE) == EXTI_LINE88))
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
#define IS_EXTI_D2_LINE(LINE) (((LINE) == EXTI_LINE0)  || ((LINE) == EXTI_LINE1)    || \
                                ((LINE) == EXTI_LINE2) || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4) || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6) || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8) || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE16) || ((LINE) == EXTI_LINE17) || \
                                ((LINE) == EXTI_LINE18) || ((LINE) == EXTI_LINE19) || \
                                ((LINE) == EXTI_LINE20) || ((LINE) == EXTI_LINE21) || \
                                ((LINE) == EXTI_LINE22) || ((LINE) == EXTI_LINE23) || \
                                ((LINE) == EXTI_LINE24) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE26) || ((LINE) == EXTI_LINE27) || \
                                ((LINE) == EXTI_LINE28) || ((LINE) == EXTI_LINE29) || \
                                ((LINE) == EXTI_LINE30) || ((LINE) == EXTI_LINE31) || \
                                ((LINE) == EXTI_LINE32) || ((LINE) == EXTI_LINE33) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE36) || ((LINE) == EXTI_LINE37) || \
                                ((LINE) == EXTI_LINE38) || ((LINE) == EXTI_LINE39) || \
                                ((LINE) == EXTI_LINE40) || ((LINE) == EXTI_LINE41) || \
                                ((LINE) == EXTI_LINE42) || ((LINE) == EXTI_LINE43) || \
                                ((LINE) == EXTI_LINE44) || ((LINE) == EXTI_LINE46) || \
                                ((LINE) == EXTI_LINE47) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53) || ((LINE) == EXTI_LINE54) || \
                                ((LINE) == EXTI_LINE55) || ((LINE) == EXTI_LINE56) || \
                                ((LINE) == EXTI_LINE57) || ((LINE) == EXTI_LINE58) || \
                                ((LINE) == EXTI_LINE59) || ((LINE) == EXTI_LINE60) || \
                                ((LINE) == EXTI_LINE61) || ((LINE) == EXTI_LINE62) || \
                                ((LINE) == EXTI_LINE63) || ((LINE) == EXTI_LINE64) || \
                                ((LINE) == EXTI_LINE65) || ((LINE) == EXTI_LINE66) || \
                                ((LINE) == EXTI_LINE67) || ((LINE) == EXTI_LINE68) || \
                                ((LINE) == EXTI_LINE69) || ((LINE) == EXTI_LINE70) || \
                                ((LINE) == EXTI_LINE71) || ((LINE) == EXTI_LINE72) || \
                                ((LINE) == EXTI_LINE73) || ((LINE) == EXTI_LINE74) || \
                                ((LINE) == EXTI_LINE75) || ((LINE) == EXTI_LINE76) || \
                                ((LINE) == EXTI_LINE78) || ((LINE) == EXTI_LINE80) || \
                                ((LINE) == EXTI_LINE82) || ((LINE) == EXTI_LINE85) || \
                                ((LINE) == EXTI_LINE86) || ((LINE) == EXTI_LINE87))
#endif /*DUAL_CORE*/

#if defined(DUAL_CORE)
#define IS_EXTI_D3_LINE(LINE) (((LINE) == EXTI_LINE0) || ((LINE) == EXTI_LINE1)    || \
                                ((LINE) == EXTI_LINE2) || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4) || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6) || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8) || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE19) || ((LINE) == EXTI_LINE20) || \
                                ((LINE) == EXTI_LINE21) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE41) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53))
#elif (POWER_DOMAINS_NUMBER == 3U)
#define IS_EXTI_D3_LINE(LINE) (((LINE) == EXTI_LINE0) || ((LINE) == EXTI_LINE1)    || \
                                ((LINE) == EXTI_LINE2) || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4) || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6) || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8) || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE19) || ((LINE) == EXTI_LINE20) || \
                                ((LINE) == EXTI_LINE21) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE41) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE52) || \
                                ((LINE) == EXTI_LINE53))
#else
#define IS_EXTI_D3_LINE(LINE) (((LINE) == EXTI_LINE0) || ((LINE) == EXTI_LINE1)    || \
                                ((LINE) == EXTI_LINE2) || ((LINE) == EXTI_LINE3)   || \
                                ((LINE) == EXTI_LINE4) || ((LINE) == EXTI_LINE5)   || \
                                ((LINE) == EXTI_LINE6) || ((LINE) == EXTI_LINE7)   || \
                                ((LINE) == EXTI_LINE8) || ((LINE) == EXTI_LINE9)   || \
                                ((LINE) == EXTI_LINE10) || ((LINE) == EXTI_LINE11) || \
                                ((LINE) == EXTI_LINE12) || ((LINE) == EXTI_LINE13) || \
                                ((LINE) == EXTI_LINE14) || ((LINE) == EXTI_LINE15) || \
                                ((LINE) == EXTI_LINE19) || ((LINE) == EXTI_LINE20) || \
                                ((LINE) == EXTI_LINE21) || ((LINE) == EXTI_LINE25) || \
                                ((LINE) == EXTI_LINE34) || ((LINE) == EXTI_LINE35) || \
                                ((LINE) == EXTI_LINE41) || ((LINE) == EXTI_LINE48) || \
                                ((LINE) == EXTI_LINE49) || ((LINE) == EXTI_LINE50) || \
                                ((LINE) == EXTI_LINE51) || ((LINE) == EXTI_LINE88))
#endif /*DUAL_CORE*/


#define  BDMA_CH6_CLEAR           ((uint32_t)0x00000000)   /*!< BDMA ch6 event selected as D3 domain pendclear source*/
#define  BDMA_CH7_CLEAR           ((uint32_t)0x00000001)   /*!< BDMA ch7 event selected as D3 domain pendclear source*/
#if defined (LPTIM4)
#define  LPTIM4_OUT_CLEAR         ((uint32_t)0x00000002)   /*!< LPTIM4 out selected as D3 domain pendclear source*/
#else
#define  LPTIM2_OUT_CLEAR         ((uint32_t)0x00000002)   /*!< LPTIM2 out selected as D3 domain pendclear source*/
#endif /* LPTIM4 */
#if defined (LPTIM5)
#define  LPTIM5_OUT_CLEAR         ((uint32_t)0x00000003)   /*!< LPTIM5 out selected as D3 domain pendclear source*/
#else
#define  LPTIM3_OUT_CLEAR         ((uint32_t)0x00000003)   /*!< LPTIM3 out selected as D3 domain pendclear source*/
#endif /* LPTIM5 */
#if defined (LPTIM4) && defined (LPTIM5)
#define IS_EXTI_D3_CLEAR(SOURCE) (((SOURCE) == BDMA_CH6_CLEAR) || ((SOURCE) == BDMA_CH7_CLEAR)    || \
                                 ((SOURCE) == LPTIM4_OUT_CLEAR) || ((SOURCE) == LPTIM5_OUT_CLEAR))
#else
#define IS_EXTI_D3_CLEAR(SOURCE) (((SOURCE) == BDMA_CH6_CLEAR) || ((SOURCE) == BDMA_CH7_CLEAR)    || \
                                 ((SOURCE) == LPTIM2_OUT_CLEAR) || ((SOURCE) == LPTIM3_OUT_CLEAR))
#endif /* LPTIM4 LPTIM5 */
/**
  * @}
  */


/** @defgroup FMC_SwapBankMapping_Config  SwapBankMapping Config
  * @{
  */
#define FMC_SWAPBMAP_DISABLE             (0x00000000U)
#define FMC_SWAPBMAP_SDRAM_SRAM          FMC_BCR1_BMAP_0
#define FMC_SWAPBMAP_SDRAMB2             FMC_BCR1_BMAP_1

#define IS_FMC_SWAPBMAP_MODE(__MODE__) (((__MODE__) == FMC_SWAPBMAP_DISABLE)    || \
                                        ((__MODE__) == FMC_SWAPBMAP_SDRAM_SRAM) || \
                                        ((__MODE__) == FMC_SWAPBMAP_SDRAMB2))
/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/
/** @defgroup ART_Exported_Macros ART Exported Macros
  * @{
  */
#if defined(DUAL_CORE)

/** @brief  ART Enable Macro.
  *         Enable the Cortex-M4 ART cache.
  */
#define __HAL_ART_ENABLE()   SET_BIT(ART->CTR, ART_CTR_EN)

/** @brief  ART Disable Macro.
  *         Disable the Cortex-M4 ART cache.
  */
#define __HAL_ART_DISABLE()   CLEAR_BIT(ART->CTR, ART_CTR_EN)

/** @brief  ART Cache BaseAddress Config.
  *         Configure the Cortex-M4 ART cache Base Address.
  */
#define __HAL_ART_CONFIG_BASE_ADDRESS(__BASE_ADDRESS__)   MODIFY_REG(ART->CTR, ART_CTR_PCACHEADDR, (((__BASE_ADDRESS__) >> 12U) & 0x000FFF00UL))

#endif /* DUAL_CORE */
/**
  * @}
  */

/** @defgroup SYSCFG_Exported_Macros SYSCFG Exported Macros
  * @{
  */

/** @brief  SYSCFG Break AXIRAM double ECC lock.
  *         Enable and lock the connection of AXIRAM double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_AXISRAM_DBL_ECC_LOCK()     SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_AXISRAML)

/** @brief  SYSCFG Break ITCM double ECC lock.
  *         Enable and lock the connection of ITCM double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_ITCM_DBL_ECC_LOCK()        SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_ITCML)

/** @brief  SYSCFG Break DTCM double ECC lock.
  *         Enable and lock the connection of DTCM double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_DTCM_DBL_ECC_LOCK()        SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_DTCML)

/** @brief  SYSCFG Break SRAM1 double ECC lock.
  *         Enable and lock the connection of SRAM1 double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_SRAM1_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_SRAM1L)

/** @brief  SYSCFG Break SRAM2 double ECC lock.
  *         Enable and lock the connection of SRAM2 double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_SRAM2_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_SRAM2L)

/** @brief  SYSCFG Break SRAM3 double ECC lock.
  *         Enable and lock the connection of SRAM3 double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_SRAM3_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_SRAM3L)

/** @brief  SYSCFG Break SRAM4 double ECC lock.
  *         Enable and lock the connection of SRAM4 double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_SRAM4_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_SRAM4L)

/** @brief  SYSCFG Break Backup SRAM double ECC lock.
  *         Enable and lock the connection of Backup SRAM double ECC error to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_BKRAM_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_BKRAML)

/** @brief  SYSCFG Break Cortex-M7 Lockup lock.
  *         Enable and lock the connection of Cortex-M7 LOCKUP output to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_CM7_LOCKUP_LOCK()          SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_CM7L)

/** @brief  SYSCFG Break FLASH double ECC lock.
  *         Enable and lock the connection of Flash double ECC error connection to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_FLASH_DBL_ECC_LOCK()       SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_FLASHL)

/** @brief  SYSCFG Break PVD lock.
  *         Enable and lock the PVD connection to Timer1/8/15/16/17 and HRTIMER Break input, as well as the PVDE and PLS[2:0] in the PWR_CR1 register.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_PVD_LOCK()                 SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_PVDL)

#if defined(DUAL_CORE)
/** @brief  SYSCFG Break Cortex-M4 Lockup lock.
  *         Enable and lock the connection of Cortex-M4 LOCKUP output to TIM1/8/15/16/17 and HRTIMER Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
            This feature is available on STM32H7 rev.B and above.
  */
#define __HAL_SYSCFG_BREAK_CM4_LOCKUP_LOCK()          SET_BIT(SYSCFG->CFGR, SYSCFG_CFGR_CM4L)
#endif /* DUAL_CORE */

#if !defined(SYSCFG_PMCR_BOOSTEN)
/** @brief  Fast-mode Plus driving capability enable/disable macros
  * @param __FASTMODEPLUS__  This parameter can be a value of :
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB6 Fast-mode Plus driving capability activation on PB6
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB7 Fast-mode Plus driving capability activation on PB7
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB8 Fast-mode Plus driving capability activation on PB8
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB9 Fast-mode Plus driving capability activation on PB9
  */
#define __HAL_SYSCFG_FASTMODEPLUS_ENABLE(__FASTMODEPLUS__)  do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                SET_BIT(SYSCFG->PMCR, (__FASTMODEPLUS__));\
                                                               }while(0)

#define __HAL_SYSCFG_FASTMODEPLUS_DISABLE(__FASTMODEPLUS__) do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                CLEAR_BIT(SYSCFG->PMCR, (__FASTMODEPLUS__));\
                                                               }while(0)

#endif /* !SYSCFG_PMCR_BOOSTEN */
/**
  * @}
  */

/** @brief  Freeze/Unfreeze Peripherals in Debug mode
  */
#define __HAL_DBGMCU_FREEZE_WWDG1()          (DBGMCU->APB3FZ1 |= (DBGMCU_APB3FZ1_DBG_WWDG1))

#define __HAL_DBGMCU_FREEZE_TIM2()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM2))
#define __HAL_DBGMCU_FREEZE_TIM3()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM3))
#define __HAL_DBGMCU_FREEZE_TIM4()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM4))
#define __HAL_DBGMCU_FREEZE_TIM5()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM5))
#define __HAL_DBGMCU_FREEZE_TIM6()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM6))
#define __HAL_DBGMCU_FREEZE_TIM7()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM7))
#define __HAL_DBGMCU_FREEZE_TIM12()          (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM12))
#define __HAL_DBGMCU_FREEZE_TIM13()          (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM13))
#define __HAL_DBGMCU_FREEZE_TIM14()          (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_TIM14))
#define __HAL_DBGMCU_FREEZE_LPTIM1()         (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_LPTIM1))
#define __HAL_DBGMCU_FREEZE_I2C1()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_I2C1))
#define __HAL_DBGMCU_FREEZE_I2C2()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_I2C2))
#define __HAL_DBGMCU_FREEZE_I2C3()           (DBGMCU->APB1LFZ1 |= (DBGMCU_APB1LFZ1_DBG_I2C3))
#define __HAL_DBGMCU_FREEZE_FDCAN()          (DBGMCU->APB1HFZ1 |= (DBGMCU_APB1HFZ1_DBG_FDCAN))


#define __HAL_DBGMCU_FREEZE_TIM1()           (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_TIM1))
#define __HAL_DBGMCU_FREEZE_TIM8()           (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_TIM8))
#define __HAL_DBGMCU_FREEZE_TIM15()          (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_TIM15))
#define __HAL_DBGMCU_FREEZE_TIM16()          (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_TIM16))
#define __HAL_DBGMCU_FREEZE_TIM17()          (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_TIM17))
#define __HAL_DBGMCU_FREEZE_HRTIM()          (DBGMCU->APB2FZ1 |= (DBGMCU_APB2FZ1_DBG_HRTIM))

#define __HAL_DBGMCU_FREEZE_I2C4()           (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_I2C4))
#define __HAL_DBGMCU_FREEZE_LPTIM2()         (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_LPTIM2))
#define __HAL_DBGMCU_FREEZE_LPTIM3()         (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_LPTIM3))
#define __HAL_DBGMCU_FREEZE_LPTIM4()         (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_LPTIM4))
#define __HAL_DBGMCU_FREEZE_LPTIM5()         (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_LPTIM5))
#define __HAL_DBGMCU_FREEZE_RTC()            (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_RTC))
#define __HAL_DBGMCU_FREEZE_IWDG1()          (DBGMCU->APB4FZ1 |= (DBGMCU_APB4FZ1_DBG_IWDG1))


#define __HAL_DBGMCU_UnFreeze_WWDG1()          (DBGMCU->APB3FZ1  &= ~ (DBGMCU_APB3FZ1_DBG_WWDG1))

#define __HAL_DBGMCU_UnFreeze_TIM2()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM2))
#define __HAL_DBGMCU_UnFreeze_TIM3()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM3))
#define __HAL_DBGMCU_UnFreeze_TIM4()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM4))
#define __HAL_DBGMCU_UnFreeze_TIM5()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM5))
#define __HAL_DBGMCU_UnFreeze_TIM6()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM6))
#define __HAL_DBGMCU_UnFreeze_TIM7()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM7))
#define __HAL_DBGMCU_UnFreeze_TIM12()          (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM12))
#define __HAL_DBGMCU_UnFreeze_TIM13()          (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM13))
#define __HAL_DBGMCU_UnFreeze_TIM14()          (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_TIM14))
#define __HAL_DBGMCU_UnFreeze_LPTIM1()         (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_LPTIM1))
#define __HAL_DBGMCU_UnFreeze_I2C1()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_I2C1))
#define __HAL_DBGMCU_UnFreeze_I2C2()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_I2C2))
#define __HAL_DBGMCU_UnFreeze_I2C3()           (DBGMCU->APB1LFZ1  &= ~ (DBGMCU_APB1LFZ1_DBG_I2C3))
#define __HAL_DBGMCU_UnFreeze_FDCAN()          (DBGMCU->APB1HFZ1  &= ~ (DBGMCU_APB1HFZ1_DBG_FDCAN))


#define __HAL_DBGMCU_UnFreeze_TIM1()           (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_TIM1))
#define __HAL_DBGMCU_UnFreeze_TIM8()           (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_TIM8))
#define __HAL_DBGMCU_UnFreeze_TIM15()          (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_TIM15))
#define __HAL_DBGMCU_UnFreeze_TIM16()          (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_TIM16))
#define __HAL_DBGMCU_UnFreeze_TIM17()          (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_TIM17))
#define __HAL_DBGMCU_UnFreeze_HRTIM()          (DBGMCU->APB2FZ1  &= ~ (DBGMCU_APB2FZ1_DBG_HRTIM))

#define __HAL_DBGMCU_UnFreeze_I2C4()           (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_I2C4))
#define __HAL_DBGMCU_UnFreeze_LPTIM2()         (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_LPTIM2))
#define __HAL_DBGMCU_UnFreeze_LPTIM3()         (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_LPTIM3))
#define __HAL_DBGMCU_UnFreeze_LPTIM4()         (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_LPTIM4))
#define __HAL_DBGMCU_UnFreeze_LPTIM5()         (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_LPTIM5))
#define __HAL_DBGMCU_UnFreeze_RTC()            (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_RTC))
#define __HAL_DBGMCU_UnFreeze_IWDG1()          (DBGMCU->APB4FZ1  &= ~ (DBGMCU_APB4FZ1_DBG_IWDG1))


#if defined(DUAL_CORE)
#define __HAL_DBGMCU_FREEZE2_IWDG2()          (DBGMCU->APB4FZ2  |= (DBGMCU_APB4FZ2_DBG_IWDG2))
#define __HAL_DBGMCU_FREEZE2_WWDG2()          (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_WWDG2))

#define __HAL_DBGMCU_UnFreeze2_IWDG2()        (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_IWDG2))
#define __HAL_DBGMCU_UnFreeze2_WWDG2()        (DBGMCU->APB1LFZ2 &= ~ (DBGMCU_APB1LFZ2_DBG_WWDG2))


#define __HAL_DBGMCU_FREEZE2_WWDG1()          (DBGMCU->APB3FZ2 |= (DBGMCU_APB3FZ2_DBG_WWDG1))

#define __HAL_DBGMCU_FREEZE2_TIM2()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM2))
#define __HAL_DBGMCU_FREEZE2_TIM3()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM3))
#define __HAL_DBGMCU_FREEZE2_TIM4()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM4))
#define __HAL_DBGMCU_FREEZE2_TIM5()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM5))
#define __HAL_DBGMCU_FREEZE2_TIM6()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM6))
#define __HAL_DBGMCU_FREEZE2_TIM7()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM7))
#define __HAL_DBGMCU_FREEZE2_TIM12()          (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM12))
#define __HAL_DBGMCU_FREEZE2_TIM13()          (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM13))
#define __HAL_DBGMCU_FREEZE2_TIM14()          (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_TIM14))
#define __HAL_DBGMCU_FREEZE2_LPTIM1()         (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_LPTIM1))
#define __HAL_DBGMCU_FREEZE2_I2C1()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_I2C1))
#define __HAL_DBGMCU_FREEZE2_I2C2()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_I2C2))
#define __HAL_DBGMCU_FREEZE2_I2C3()           (DBGMCU->APB1LFZ2 |= (DBGMCU_APB1LFZ2_DBG_I2C3))
#define __HAL_DBGMCU_FREEZE2_FDCAN()          (DBGMCU->APB1HFZ2 |= (DBGMCU_APB1HFZ2_DBG_FDCAN))


#define __HAL_DBGMCU_FREEZE2_TIM1()           (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_TIM1))
#define __HAL_DBGMCU_FREEZE2_TIM8()           (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_TIM8))
#define __HAL_DBGMCU_FREEZE2_TIM15()          (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_TIM15))
#define __HAL_DBGMCU_FREEZE2_TIM16()          (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_TIM16))
#define __HAL_DBGMCU_FREEZE2_TIM17()          (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_TIM17))
#define __HAL_DBGMCU_FREEZE2_HRTIM()          (DBGMCU->APB2FZ2 |= (DBGMCU_APB2FZ2_DBG_HRTIM))

#define __HAL_DBGMCU_FREEZE2_I2C4()           (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_I2C4))
#define __HAL_DBGMCU_FREEZE2_LPTIM2()         (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_LPTIM2))
#define __HAL_DBGMCU_FREEZE2_LPTIM3()         (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_LPTIM3))
#define __HAL_DBGMCU_FREEZE2_LPTIM4()         (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_LPTIM4))
#define __HAL_DBGMCU_FREEZE2_LPTIM5()         (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_LPTIM5))
#define __HAL_DBGMCU_FREEZE2_RTC()            (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_RTC))
#define __HAL_DBGMCU_FREEZE2_IWDG1()          (DBGMCU->APB4FZ2 |= (DBGMCU_APB4FZ2_DBG_IWDG1))

#define __HAL_DBGMCU_UnFreeze2_WWDG1()          (DBGMCU->APB3FZ2  &= ~ (DBGMCU_APB3FZ2_DBG_WWDG1))

#define __HAL_DBGMCU_UnFreeze2_TIM2()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM2))
#define __HAL_DBGMCU_UnFreeze2_TIM3()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM3))
#define __HAL_DBGMCU_UnFreeze2_TIM4()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM4))
#define __HAL_DBGMCU_UnFreeze2_TIM5()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM5))
#define __HAL_DBGMCU_UnFreeze2_TIM6()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM6))
#define __HAL_DBGMCU_UnFreeze2_TIM7()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM7))
#define __HAL_DBGMCU_UnFreeze2_TIM12()          (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM12))
#define __HAL_DBGMCU_UnFreeze2_TIM13()          (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM13))
#define __HAL_DBGMCU_UnFreeze2_TIM14()          (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_TIM14))
#define __HAL_DBGMCU_UnFreeze2_LPTIM1()         (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_LPTIM1))
#define __HAL_DBGMCU_UnFreeze2_I2C1()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_I2C1))
#define __HAL_DBGMCU_UnFreeze2_I2C2()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_I2C2))
#define __HAL_DBGMCU_UnFreeze2_I2C3()           (DBGMCU->APB1LFZ2  &= ~ (DBGMCU_APB1LFZ2_DBG_I2C3))
#define __HAL_DBGMCU_UnFreeze2_FDCAN()          (DBGMCU->APB1HFZ2  &= ~ (DBGMCU_APB1HFZ2_DBG_FDCAN))


#define __HAL_DBGMCU_UnFreeze2_TIM1()           (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_TIM1))
#define __HAL_DBGMCU_UnFreeze2_TIM8()           (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_TIM8))
#define __HAL_DBGMCU_UnFreeze2_TIM15()          (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_TIM15))
#define __HAL_DBGMCU_UnFreeze2_TIM16()          (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_TIM16))
#define __HAL_DBGMCU_UnFreeze2_TIM17()          (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_TIM17))
#define __HAL_DBGMCU_UnFreeze2_HRTIM()          (DBGMCU->APB2FZ2  &= ~ (DBGMCU_APB2FZ2_DBG_HRTIM))

#define __HAL_DBGMCU_UnFreeze2_I2C4()           (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_I2C4))
#define __HAL_DBGMCU_UnFreeze2_LPTIM2()         (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_LPTIM2))
#define __HAL_DBGMCU_UnFreeze2_LPTIM3()         (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_LPTIM3))
#define __HAL_DBGMCU_UnFreeze2_LPTIM4()         (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_LPTIM4))
#define __HAL_DBGMCU_UnFreeze2_LPTIM5()         (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_LPTIM5))
#define __HAL_DBGMCU_UnFreeze2_RTC()            (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_RTC))
#define __HAL_DBGMCU_UnFreeze2_IWDG1()          (DBGMCU->APB4FZ2  &= ~ (DBGMCU_APB4FZ2_DBG_IWDG1))

#endif /*DUAL_CORE*/

/** @defgroup HAL_Private_Macros HAL Private Macros
  * @{
  */
#define IS_TICKFREQ(FREQ) (((FREQ) == HAL_TICK_FREQ_10HZ)  || \
                           ((FREQ) == HAL_TICK_FREQ_100HZ) || \
                           ((FREQ) == HAL_TICK_FREQ_1KHZ))
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/

/** @addtogroup HAL_Exported_Variables
  * @{
  */
extern __IO uint32_t uwTick;
extern uint32_t uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);

/* Peripheral Control functions  ************************************************/
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);
#if defined(SYSCFG_PMCR_EPIS_SEL)
void HAL_SYSCFG_ETHInterfaceSelect(uint32_t SYSCFG_ETHInterface);
#endif /* SYSCFG_PMCR_EPIS_SEL */
void HAL_SYSCFG_AnalogSwitchConfig(uint32_t SYSCFG_AnalogSwitch , uint32_t SYSCFG_SwitchState );
#if defined(SYSCFG_PMCR_BOOSTEN)
void HAL_SYSCFG_EnableBOOST(void);
void HAL_SYSCFG_DisableBOOST(void);
#endif /* SYSCFG_PMCR_BOOSTEN */

#if defined (SYSCFG_UR2_BOOT_ADD0) ||  defined (SYSCFG_UR2_BCM7_ADD0)
void HAL_SYSCFG_CM7BootAddConfig(uint32_t BootRegister, uint32_t BootAddress);
#endif /* SYSCFG_UR2_BOOT_ADD0 || SYSCFG_UR2_BCM7_ADD0*/

#if defined(DUAL_CORE)
void HAL_SYSCFG_CM4BootAddConfig(uint32_t BootRegister, uint32_t BootAddress);
void HAL_SYSCFG_EnableCM7BOOT(void);
void HAL_SYSCFG_DisableCM7BOOT(void);
void HAL_SYSCFG_EnableCM4BOOT(void);
void HAL_SYSCFG_DisableCM4BOOT(void);
#endif /*DUAL_CORE*/
void HAL_EnableCompensationCell(void);
void HAL_DisableCompensationCell(void);
void HAL_SYSCFG_EnableIOSpeedOptimize(void);
void HAL_SYSCFG_DisableIOSpeedOptimize(void);
void HAL_SYSCFG_CompensationCodeSelect(uint32_t SYSCFG_CompCode);
void HAL_SYSCFG_CompensationCodeConfig(uint32_t SYSCFG_PMOSCode, uint32_t SYSCFG_NMOSCode);
#if defined(SYSCFG_CCCR_NCC_MMC)
void HAL_SYSCFG_VDDMMC_CompensationCodeConfig(uint32_t SYSCFG_PMOSCode, uint32_t SYSCFG_NMOSCode);
#endif /* SYSCFG_CCCR_NCC_MMC */
void HAL_EnableDBGSleepMode(void);
void HAL_DisableDBGSleepMode(void);
void HAL_EnableDBGStopMode(void);
void HAL_DisableDBGStopMode(void);
void HAL_EnableDBGStandbyMode(void);
void HAL_DisableDBGStandbyMode(void);
#if defined(DUAL_CORE)
void HAL_EnableDomain2DBGSleepMode(void);
void HAL_DisableDomain2DBGSleepMode(void);
void HAL_EnableDomain2DBGStopMode(void);
void HAL_DisableDomain2DBGStopMode(void);
void HAL_EnableDomain2DBGStandbyMode(void);
void HAL_DisableDomain2DBGStandbyMode(void);
#endif /*DUAL_CORE*/
void HAL_EnableDomain3DBGStopMode(void);
void HAL_DisableDomain3DBGStopMode(void);
void HAL_EnableDomain3DBGStandbyMode(void);
void HAL_DisableDomain3DBGStandbyMode(void);
void HAL_EXTI_EdgeConfig(uint32_t EXTI_Line , uint32_t EXTI_Edge );
void HAL_EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
#if defined(DUAL_CORE)
void HAL_EXTI_D2_ClearFlag(uint32_t EXTI_Line);
#endif /*DUAL_CORE*/
void HAL_EXTI_D1_ClearFlag(uint32_t EXTI_Line);
void HAL_EXTI_D1_EventInputConfig(uint32_t EXTI_Line , uint32_t EXTI_Mode,  uint32_t EXTI_LineCmd);
#if defined(DUAL_CORE)
void HAL_EXTI_D2_EventInputConfig(uint32_t EXTI_Line , uint32_t EXTI_Mode,  uint32_t EXTI_LineCmd);
#endif /*DUAL_CORE*/
void HAL_EXTI_D3_EventInputConfig(uint32_t EXTI_Line, uint32_t EXTI_LineCmd , uint32_t EXTI_ClearSrc);
void HAL_SetFMCMemorySwappingConfig(uint32_t BankMapConfig);
uint32_t HAL_GetFMCMemorySwappingConfig(void);
void HAL_SYSCFG_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling);
void HAL_SYSCFG_VREFBUF_HighImpedanceConfig(uint32_t Mode);
void HAL_SYSCFG_VREFBUF_TrimmingConfig(uint32_t TrimmingValue);
HAL_StatusTypeDef HAL_SYSCFG_EnableVREFBUF(void);
void HAL_SYSCFG_DisableVREFBUF(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
