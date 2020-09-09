/**
  ******************************************************************************
  * @file    stm32h7xx_hal_pwr_ex.h
  * @author  MCD Application Team
  * @brief   Header file of PWR HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics.
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
#ifndef STM32H7xx_HAL_PWR_EX_H
#define STM32H7xx_HAL_PWR_EX_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup PWREx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PWREx_Exported_Types PWREx Exported Types
  * @{
  */
/**
  * @brief  PWREx AVD configuration structure definition
  */
typedef struct
{
  uint32_t AVDLevel; /*!< AVDLevel : Specifies the AVD detection level. This
                          parameter can be a value of @ref
                          PWREx_AVD_detection_level
                     */

  uint32_t Mode;     /*!< Mode : Specifies the EXTI operating mode for the AVD
                                 event. This parameter can be a value of @ref
                                 PWREx_AVD_Mode.
                     */
}PWREx_AVDTypeDef;

/**
  * @brief  PWREx Wakeup pin configuration structure definition
  */
typedef struct
{
  uint32_t WakeUpPin;   /*!< WakeUpPin: Specifies the Wake-Up pin to be enabled.
                                        This parameter can be a value of @ref
                                        PWREx_WakeUp_Pins
                        */

  uint32_t PinPolarity; /*!< PinPolarity: Specifies the Wake-Up pin polarity.
                                          This parameter can be a value of @ref
                                          PWREx_PIN_Polarity
                        */

  uint32_t PinPull;     /*!< PinPull: Specifies the Wake-Up pin pull. This
                                      parameter can be a value of @ref
                                      PWREx_PIN_Pull
                        */
}PWREx_WakeupPinTypeDef;

#if defined (PWR_CSR1_MMCVDO)
/**
  * @brief  PWR VDDMMC voltage level enum definition
  */
typedef enum
{
  PWR_MMC_VOLTAGE_BELOW_1V2,      /*!< VDDMMC is below 1V2          */
  PWR_MMC_VOLTAGE_EQUAL_ABOVE_1V2 /*!< VDDMMC is above or equal 1V2 */
} PWREx_MMC_VoltageLevel;
#endif /* defined (PWR_CSR1_MMCVDO) */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PWREx_Exported_Constants PWREx Exported Constants
  * @{
  */
/** @defgroup PWREx_WakeUp_Pins PWREx Wake-Up Pins
  * @{
  */
/* High level and No pull (default configuration) */
#define PWR_WAKEUP_PIN6      PWR_WKUPEPR_WKUPEN6
#define PWR_WAKEUP_PIN5      PWR_WKUPEPR_WKUPEN5
#define PWR_WAKEUP_PIN4      PWR_WKUPEPR_WKUPEN4
#define PWR_WAKEUP_PIN3      PWR_WKUPEPR_WKUPEN3
#define PWR_WAKEUP_PIN2      PWR_WKUPEPR_WKUPEN2
#define PWR_WAKEUP_PIN1      PWR_WKUPEPR_WKUPEN1

/* High level and No pull */
#define PWR_WAKEUP_PIN6_HIGH PWR_WKUPEPR_WKUPEN6
#define PWR_WAKEUP_PIN5_HIGH PWR_WKUPEPR_WKUPEN5
#define PWR_WAKEUP_PIN4_HIGH PWR_WKUPEPR_WKUPEN4
#define PWR_WAKEUP_PIN3_HIGH PWR_WKUPEPR_WKUPEN3
#define PWR_WAKEUP_PIN2_HIGH PWR_WKUPEPR_WKUPEN2
#define PWR_WAKEUP_PIN1_HIGH PWR_WKUPEPR_WKUPEN1

/* Low level and No pull */
#define PWR_WAKEUP_PIN6_LOW  (PWR_WKUPEPR_WKUPP6 | PWR_WKUPEPR_WKUPEN6)
#define PWR_WAKEUP_PIN5_LOW  (PWR_WKUPEPR_WKUPP5 | PWR_WKUPEPR_WKUPEN5)
#define PWR_WAKEUP_PIN4_LOW  (PWR_WKUPEPR_WKUPP4 | PWR_WKUPEPR_WKUPEN4)
#define PWR_WAKEUP_PIN3_LOW  (PWR_WKUPEPR_WKUPP3 | PWR_WKUPEPR_WKUPEN3)
#define PWR_WAKEUP_PIN2_LOW  (PWR_WKUPEPR_WKUPP2 | PWR_WKUPEPR_WKUPEN2)
#define PWR_WAKEUP_PIN1_LOW  (PWR_WKUPEPR_WKUPP1 | PWR_WKUPEPR_WKUPEN1)
/**
  * @}
  */

/** @defgroup PWREx_PIN_Polarity PWREx Pin Polarity configuration
  * @{
  */
#define PWR_PIN_POLARITY_HIGH (0x00000000U)
#define PWR_PIN_POLARITY_LOW  (0x00000001U)
/**
  * @}
  */

/** @defgroup PWREx_PIN_Pull PWREx Pin Pull configuration
  * @{
  */
#define PWR_PIN_NO_PULL   (0x00000000U)
#define PWR_PIN_PULL_UP   (0x00000001U)
#define PWR_PIN_PULL_DOWN (0x00000002U)
/**
  * @}
  */

/** @defgroup PWREx_Wakeup_Pins_Flags PWREx Wakeup Pins Flags.
  * @{
  */
#define PWR_WAKEUP_FLAG1 PWR_WKUPFR_WKUPF1 /*!< Wakeup flag on PA0  */
#define PWR_WAKEUP_FLAG2 PWR_WKUPFR_WKUPF2 /*!< Wakeup flag on PA2  */
#define PWR_WAKEUP_FLAG3 PWR_WKUPFR_WKUPF3 /*!< Wakeup flag on PI8  */
#define PWR_WAKEUP_FLAG4 PWR_WKUPFR_WKUPF4 /*!< Wakeup flag on PC13 */
#define PWR_WAKEUP_FLAG5 PWR_WKUPFR_WKUPF5 /*!< Wakeup flag on PI11 */
#define PWR_WAKEUP_FLAG6 PWR_WKUPFR_WKUPF6 /*!< Wakeup flag on PC1  */
#define PWR_WAKEUP_FLAG_ALL (PWR_WKUPFR_WKUPF1 | PWR_WKUPFR_WKUPF2 |\
                             PWR_WKUPFR_WKUPF3 | PWR_WKUPFR_WKUPF4 |\
                             PWR_WKUPFR_WKUPF5 | PWR_WKUPFR_WKUPF6)
/**
  * @}
  */

#if defined (DUAL_CORE)
/** @defgroup PWREx_Core_Select PWREx Core definition
  * @{
  */
#define PWR_CORE_CPU1 (0x00000000U)
#define PWR_CORE_CPU2 (0x00000001U)
/**
  * @}
  */
#endif /* defined (DUAL_CORE) */

/** @defgroup PWREx_Domains PWREx Domains definition
  * @{
  */
#define PWR_D1_DOMAIN (0x00000000U)
#if defined (PWR_CPUCR_PDDS_D2)
#define PWR_D2_DOMAIN (0x00000001U)
#endif /* defined (PWR_CPUCR_PDDS_D2) */
#define PWR_D3_DOMAIN (0x00000002U)
/**
  * @}
  */

/** @defgroup PWREx_Domain_Flags PWREx Domain Flags definition
  * @{
  */
#if defined (DUAL_CORE)
#define PWR_D1_DOMAIN_FLAGS  (0x00000000U)
#define PWR_D2_DOMAIN_FLAGS  (0x00000001U)
#define PWR_ALL_DOMAIN_FLAGS (0x00000002U)
#else
#define PWR_CPU_FLAGS       (0x00000000U)
#endif /* defined (DUAL_CORE) */
/**
  * @}
  */

/** @defgroup PWREx_D3_State PWREx D3 Domain State
  * @{
  */
#define PWR_D3_DOMAIN_STOP (0x00000000U)
#define PWR_D3_DOMAIN_RUN  (0x00000800U)

/**
  * @}
  */

/** @defgroup PWREx_Supply_configuration PWREx Supply configuration
  * @{
  */
#define PWR_LDO_SUPPLY                      PWR_CR3_LDOEN                                                               /*!< Core domains are suppplied from the LDO                                                                     */
#if defined (SMPS)
#define PWR_DIRECT_SMPS_SUPPLY              PWR_CR3_SMPSEN                                                              /*!< Core domains are suppplied from the SMPS only                                                               */
#define PWR_SMPS_1V8_SUPPLIES_LDO           (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEN    | PWR_CR3_LDOEN)                   /*!< The SMPS 1.8V output supplies the LDO which supplies the Core domains                                       */
#define PWR_SMPS_2V5_SUPPLIES_LDO           (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEN    | PWR_CR3_LDOEN)                   /*!< The SMPS 2.5V output supplies the LDO which supplies the Core domains                                       */
#define PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO   (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_LDOEN)  /*!< The SMPS 1.8V output supplies an external circuits and the LDO. The Core domains are suppplied from the LDO */
#define PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO   (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_LDOEN)  /*!< The SMPS 2.5V output supplies an external circuits and the LDO. The Core domains are suppplied from the LDO */
#define PWR_SMPS_1V8_SUPPLIES_EXT           (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_BYPASS) /*!< The SMPS 1.8V output supplies an external source which supplies the Core domains                            */
#define PWR_SMPS_2V5_SUPPLIES_EXT           (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_BYPASS) /*!< The SMPS 2.5V output supplies an external source which supplies the Core domains                            */
#endif /* defined (SMPS) */
#define PWR_EXTERNAL_SOURCE_SUPPLY          PWR_CR3_BYPASS                                                              /*!< The SMPS disabled and the LDO Bypass. The Core domains are supplied from an external source                 */

#if defined (SMPS)
#define PWR_SUPPLY_CONFIG_MASK (PWR_CR3_SMPSLEVEL | PWR_CR3_SMPSEXTHP | \
                                PWR_CR3_SMPSEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)
#else
#define PWR_SUPPLY_CONFIG_MASK (PWR_CR3_SCUEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)
#endif /* defined (SMPS) */
/**
  * @}
  */


/** @defgroup PWREx_AVD_detection_level PWREx AVD detection level
  * @{
  */
#define PWR_AVDLEVEL_0 PWR_CR1_ALS_LEV0 /*!< Analog voltage detector level 0
                                             selection : 1V7                 */
#define PWR_AVDLEVEL_1 PWR_CR1_ALS_LEV1 /*!< Analog voltage detector level 1
                                             selection : 2V1                 */
#define PWR_AVDLEVEL_2 PWR_CR1_ALS_LEV2 /*!< Analog voltage detector level 2
                                             selection : 2V5                 */
#define PWR_AVDLEVEL_3 PWR_CR1_ALS_LEV3 /*!< Analog voltage detector level 3
                                             selection : 2V8                 */
/**
  * @}
  */

/** @defgroup PWREx_AVD_Mode PWREx AVD Mode
  * @{
  */
#define PWR_AVD_MODE_NORMAL               (0x00000000U)   /*!< Basic mode is used                                                 */
#define PWR_AVD_MODE_IT_RISING            (0x00010001U)   /*!< External Interrupt Mode with Rising edge trigger detection         */
#define PWR_AVD_MODE_IT_FALLING           (0x00010002U)   /*!< External Interrupt Mode with Falling edge trigger detection        */
#define PWR_AVD_MODE_IT_RISING_FALLING    (0x00010003U)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_AVD_MODE_EVENT_RISING         (0x00020001U)   /*!< Event Mode with Rising edge trigger detection                      */
#define PWR_AVD_MODE_EVENT_FALLING        (0x00020002U)   /*!< Event Mode with Falling edge trigger detection                     */
#define PWR_AVD_MODE_EVENT_RISING_FALLING (0x00020003U)   /*!< Event Mode with Rising/Falling edge trigger detection              */
/**
  * @}
  */

/** @defgroup PWREx_Regulator_Voltage_Scale PWREx Regulator Voltage Scale
  * @{
  */
#define PWR_REGULATOR_SVOS_SCALE5 (PWR_CR1_SVOS_0)
#define PWR_REGULATOR_SVOS_SCALE4 (PWR_CR1_SVOS_1)
#define PWR_REGULATOR_SVOS_SCALE3 (PWR_CR1_SVOS_0 | PWR_CR1_SVOS_1)
/**
  * @}
  */

/** @defgroup PWREx_VBAT_Battery_Charging_Resistor PWR battery charging resistor selection
  * @{
  */
#define PWR_BATTERY_CHARGING_RESISTOR_5    (0x00000000U) /*!< VBAT charging through a 5 kOhms resistor   */
#define PWR_BATTERY_CHARGING_RESISTOR_1_5  PWR_CR3_VBRS  /*!< VBAT charging through a 1.5 kOhms resistor */
/**
  * @}
  */

/** @defgroup PWREx_VBAT_Thresholds PWREx VBAT Thresholds
  * @{
  */
#define PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD  (0x00000000U)
#define PWR_VBAT_BELOW_LOW_THRESHOLD         PWR_CR2_VBATL
#define PWR_VBAT_ABOVE_HIGH_THRESHOLD        PWR_CR2_VBATH
/**
  * @}
  */

/** @defgroup PWREx_TEMP_Thresholds PWREx Temperature Thresholds
  * @{
  */
#define PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD  (0x00000000U)
#define PWR_TEMP_BELOW_LOW_THRESHOLD         PWR_CR2_TEMPL
#define PWR_TEMP_ABOVE_HIGH_THRESHOLD        PWR_CR2_TEMPH
/**
  * @}
  */
/** @defgroup PWREx_AVD_EXTI_Line PWREx AVD EXTI Line 16
  * @{
  */
#define PWR_EXTI_LINE_AVD EXTI_IMR1_IM16 /*!< External interrupt line 16
                                              Connected to the AVD EXTI Line */
/**
  * @}
  */

#if defined (PWR_CR1_SRDRAMSO)
/** @defgroup PWREx_Memory_Shut_Off Memory shut-off block selection
  * @{
  */
#define PWR_SRD_AHB_MEMORY_BLOCK        PWR_CR1_SRDRAMSO    /*!< SmartRun domain AHB memory shut-off in DStop/DStop2 low-power mode         */
#define PWR_USB_FDCAN_MEMORY_BLOCK      PWR_CR1_HSITFSO     /*!< High-speed interfaces USB and FDCAN memories shut-off in DStop/DStop2 mode */
#define PWR_GFXMMU_JPEG_MEMORY_BLOCK    PWR_CR1_GFXSO       /*!< GFXMMU and JPEG memories shut-off in DStop/DStop2 mode                     */
#define PWR_TCM_ECM_MEMORY_BLOCK        PWR_CR1_ITCMSO      /*!< Instruction TCM and ETM memories shut-off in DStop/DStop2 mode             */
#define PWR_RAM1_AHB_MEMORY_BLOCK       PWR_CR1_AHBRAM1SO   /*!< AHB RAM1 shut-off in DStop/DStop2 mode                                     */
#define PWR_RAM2_AHB_MEMORY_BLOCK       PWR_CR1_AHBRAM2SO   /*!< AHB RAM2 shut-off in DStop/DStop2 mode                                     */
#define PWR_RAM1_AXI_MEMORY_BLOCK       PWR_CR1_AXIRAM1SO   /*!< AXI RAM1 shut-off in DStop/DStop2 mode                                     */
#define PWR_RAM2_AXI_MEMORY_BLOCK       PWR_CR1_AXIRAM2SO   /*!< AXI RAM2 shut-off in DStop/DStop2 mode                                     */
#define PWR_RAM3_AXI_MEMORY_BLOCK       PWR_CR1_AXIRAM3SO   /*!< AXI RAM3 shut-off in DStop/DStop2 mode                                     */
#define PWR_MEMORY_BLOCK_KEEP_ON        0U                  /*!< Memory content is kept in DStop or DStop2 mode                             */
#define PWR_MEMORY_BLOCK_SHUT_OFF       1U                  /*!< Memory content is lost in DStop or DStop2 mode                             */
/**
  * @}
  */
#endif /* defined (PWR_CR1_SRDRAMSO) */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup PWREx_Exported_Macro PWREx Exported Macro
  *  @{
  */

/**
  * @brief Enable the AVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR1, PWR_EXTI_LINE_AVD)

#if defined (DUAL_CORE)
/**
  * @brief Enable the AVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTID2_ENABLE_IT() SET_BIT(EXTI_D2->IMR1, PWR_EXTI_LINE_AVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Disable the AVD EXTI Line 16
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR1, PWR_EXTI_LINE_AVD)

#if defined (DUAL_CORE)
/**
  * @brief Disable the AVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTID2_DISABLE_IT() CLEAR_BIT(EXTI_D2->IMR1, PWR_EXTI_LINE_AVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Enable event on AVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR1, PWR_EXTI_LINE_AVD)

#if defined (DUAL_CORE)
/**
  * @brief Enable event on AVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTID2_ENABLE_EVENT() SET_BIT(EXTI_D2->EMR1, PWR_EXTI_LINE_AVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Disable event on AVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR1, PWR_EXTI_LINE_AVD)

#if defined (DUAL_CORE)
/**
  * @brief Disable event on AVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTID2_DISABLE_EVENT() CLEAR_BIT(EXTI_D2->EMR1, PWR_EXTI_LINE_AVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Enable the AVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR1, PWR_EXTI_LINE_AVD)

/**
  * @brief Disable the AVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR1, PWR_EXTI_LINE_AVD)

/**
  * @brief Enable the AVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR1, PWR_EXTI_LINE_AVD)

/**
  * @brief Disable the AVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR1, PWR_EXTI_LINE_AVD)

/**
  * @brief Enable the AVD Extended Interrupt Rising and Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_ENABLE_RISING_FALLING_EDGE() \
do {                                                    \
     __HAL_PWR_AVD_EXTI_ENABLE_RISING_EDGE();           \
     __HAL_PWR_AVD_EXTI_ENABLE_FALLING_EDGE();          \
} while(0);

/**
  * @brief Disable the AVD Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_DISABLE_RISING_FALLING_EDGE() \
do {                                                     \
     __HAL_PWR_AVD_EXTI_DISABLE_RISING_EDGE();           \
     __HAL_PWR_AVD_EXTI_DISABLE_FALLING_EDGE();          \
} while(0);

/**
  * @brief Check whether the specified AVD EXTI interrupt flag is set or not.
  * @retval EXTI AVD Line Status.
  */
#define __HAL_PWR_AVD_EXTI_GET_FLAG() ((READ_BIT(EXTI->PR1, PWR_EXTI_LINE_AVD) == PWR_EXTI_LINE_AVD) ? 1UL : 0UL)

#if defined (DUAL_CORE)
/**
  * @brief Check whether the specified AVD EXTI D2 interrupt flag is set or not.
  * @retval EXTI D2 AVD Line Status.
  */
#define __HAL_PWR_AVD_EXTID2_GET_FLAG() ((READ_BIT(EXTI_D2->PR1, PWR_EXTI_LINE_AVD) == PWR_EXTI_LINE_AVD) ? 1UL : 0UL)
#endif /* defined (DUAL_CORE) */

/**
  * @brief  Clear the AVD EXTI flag.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_CLEAR_FLAG() SET_BIT(EXTI->PR1, PWR_EXTI_LINE_AVD)

#if defined (DUAL_CORE)
/**
  * @brief  Clear the AVD EXTI D2 flag.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTID2_CLEAR_FLAG() SET_BIT(EXTI_D2->PR1, PWR_EXTI_LINE_AVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief  Generates a Software interrupt on AVD EXTI line.
  * @retval None.
  */
#define __HAL_PWR_AVD_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER1, PWR_EXTI_LINE_AVD)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup PWREx_Exported_Functions PWREx Exported Functions
  * @{
  */

/** @addtogroup PWREx_Exported_Functions_Group1 Power Supply Control Functions
  * @{
  */
HAL_StatusTypeDef HAL_PWREx_ConfigSupply(uint32_t SupplySource);
uint32_t HAL_PWREx_GetSupplyConfig(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlStopModeVoltageScaling(uint32_t VoltageScaling);
uint32_t HAL_PWREx_GetStopModeVoltageRange(void);
/**
  * @}
  */

/** @addtogroup PWREx_Exported_Functions_Group2 Low Power Control Functions
  * @{
  */
/* System low power control functions */
#if defined (PWR_CPUCR_RETDS_CD)
void HAL_PWREx_EnterSTOP2Mode(uint32_t Regulator, uint8_t STOPEntry);
#endif /* defined (PWR_CPUCR_RETDS_CD) */
void HAL_PWREx_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry, uint32_t Domain);
void HAL_PWREx_EnterSTANDBYMode(uint32_t Domain);
void HAL_PWREx_ConfigD3Domain(uint32_t D3State);
/* Clear Cortex-Mx pending flag */
void HAL_PWREx_ClearPendingEvent(void);
#if defined (DUAL_CORE)
/* Clear domain flags */
void HAL_PWREx_ClearDomainFlags(uint32_t DomainFlags);
/* Core Hold/Release functions */
HAL_StatusTypeDef HAL_PWREx_HoldCore(uint32_t CPU);
void HAL_PWREx_ReleaseCore(uint32_t CPU);
#endif /* defined (DUAL_CORE) */
/* Flash low power control functions */
void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void);
#if defined (PWR_CR1_SRDRAMSO)
/* Memory shut-off functions */
void HAL_PWREx_EnableMemoryShutOff(uint32_t MemoryBlock);
void HAL_PWREx_DisableMemoryShutOff(uint32_t MemoryBlock);
#endif /* defined(PWR_CR1_SRDRAMSO) */
/* Wakeup Pins control functions */
void HAL_PWREx_EnableWakeUpPin(PWREx_WakeupPinTypeDef *sPinParams);
void HAL_PWREx_DisableWakeUpPin(uint32_t WakeUpPin);
uint32_t HAL_PWREx_GetWakeupFlag(uint32_t WakeUpFlag);
HAL_StatusTypeDef HAL_PWREx_ClearWakeupFlag(uint32_t WakeUpFlag);
/* Power Wakeup PIN IRQ Handler */
void HAL_PWREx_WAKEUP_PIN_IRQHandler(void);
void HAL_PWREx_WKUP1_Callback(void);
void HAL_PWREx_WKUP2_Callback(void);
void HAL_PWREx_WKUP3_Callback(void);
void HAL_PWREx_WKUP4_Callback(void);
void HAL_PWREx_WKUP5_Callback(void);
void HAL_PWREx_WKUP6_Callback(void);
/**
  * @}
  */

/** @addtogroup PWREx_Exported_Functions_Group3 Peripherals Control Functions
  * @{
  */
/* Backup regulator control functions */
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void);
/* USB regulator control functions */
HAL_StatusTypeDef HAL_PWREx_EnableUSBReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableUSBReg(void);
void HAL_PWREx_EnableUSBVoltageDetector(void);
void HAL_PWREx_DisableUSBVoltageDetector(void);
/* Battery control functions */
void HAL_PWREx_EnableBatteryCharging(uint32_t ResistorValue);
void HAL_PWREx_DisableBatteryCharging(void);
#if defined(PWR_CR1_BOOSTE)
/* Analog Booster functions */
void HAL_PWREx_EnableAnalogBooster(void);
void HAL_PWREx_DisableAnalogBooster(void);
#endif /* PWR_CR1_BOOSTE */
/**
  * @}
  */

/** @addtogroup PWREx_Exported_Functions_Group4 Power Monitoring functions
  * @{
  */
/* Power VBAT/Temperature monitoring functions */
void HAL_PWREx_EnableMonitoring(void);
void HAL_PWREx_DisableMonitoring(void);
uint32_t HAL_PWREx_GetTemperatureLevel(void);
uint32_t HAL_PWREx_GetVBATLevel(void);
#if defined(PWR_CSR1_MMCVDO)
PWREx_MMC_VoltageLevel HAL_PWREx_GetMMCVoltage(void);
#endif /* PWR_CSR1_MMCVDO */
/* Power AVD configuration functions */
void HAL_PWREx_ConfigAVD(PWREx_AVDTypeDef *sConfigAVD);
void HAL_PWREx_EnableAVD(void);
void HAL_PWREx_DisableAVD(void);
/* Power PVD/AVD IRQ Handler */
void HAL_PWREx_PVD_AVD_IRQHandler(void);
void HAL_PWREx_AVDCallback(void);
/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup PWREx_Private_Macros PWREx Private Macros
  * @{
  */

/** @defgroup PWREx_IS_PWR_Definitions PWREx Private macros to check input parameters
  * @{
  */
/* Check PWR regulator configuration parameter */
#if defined (SMPS)
#define IS_PWR_SUPPLY(PWR_SOURCE) (((PWR_SOURCE) == PWR_LDO_SUPPLY)                    ||\
                                   ((PWR_SOURCE) == PWR_DIRECT_SMPS_SUPPLY)            ||\
                                   ((PWR_SOURCE) == PWR_SMPS_1V8_SUPPLIES_LDO)         ||\
                                   ((PWR_SOURCE) == PWR_SMPS_2V5_SUPPLIES_LDO)         ||\
                                   ((PWR_SOURCE) == PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO) ||\
                                   ((PWR_SOURCE) == PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO) ||\
                                   ((PWR_SOURCE) == PWR_SMPS_1V8_SUPPLIES_EXT)         ||\
                                   ((PWR_SOURCE) == PWR_SMPS_2V5_SUPPLIES_EXT)         ||\
                                   ((PWR_SOURCE) == PWR_EXTERNAL_SOURCE_SUPPLY))

#else
#define IS_PWR_SUPPLY(PWR_SOURCE) (((PWR_SOURCE) == PWR_LDO_SUPPLY) ||\
                                   ((PWR_SOURCE) == PWR_EXTERNAL_SOURCE_SUPPLY))
#endif /* defined (SMPS) */

/* Check PWR regulator configuration in STOP mode parameter */
#define IS_PWR_STOP_MODE_REGULATOR_VOLTAGE(VOLTAGE) (((VOLTAGE) == PWR_REGULATOR_SVOS_SCALE3)  ||\
                                                     ((VOLTAGE) == PWR_REGULATOR_SVOS_SCALE4)  ||\
                                                     ((VOLTAGE) == PWR_REGULATOR_SVOS_SCALE5))

/* Check PWR domain parameter */
#if defined (PWR_CPUCR_PDDS_D2)
#define IS_PWR_DOMAIN(DOMAIN) (((DOMAIN) == PWR_D1_DOMAIN) ||\
                               ((DOMAIN) == PWR_D2_DOMAIN) ||\
                               ((DOMAIN) == PWR_D3_DOMAIN))
#else
#define IS_PWR_DOMAIN(DOMAIN) (((DOMAIN) == PWR_D1_DOMAIN) ||\
                               ((DOMAIN) == PWR_D3_DOMAIN))
#endif /* defined (PWR_CPUCR_PDDS_D2) */

/* Check D3/SRD domain state parameter */
#define IS_D3_STATE(STATE) (((STATE) == PWR_D3_DOMAIN_STOP) ||\
                            ((STATE) == PWR_D3_DOMAIN_RUN))

/* Check wake up pin parameter */
#define IS_PWR_WAKEUP_PIN(PIN) (((PIN) == PWR_WAKEUP_PIN1)      ||\
                                ((PIN) == PWR_WAKEUP_PIN2)      ||\
                                ((PIN) == PWR_WAKEUP_PIN3)      ||\
                                ((PIN) == PWR_WAKEUP_PIN4)      ||\
                                ((PIN) == PWR_WAKEUP_PIN5)      ||\
                                ((PIN) == PWR_WAKEUP_PIN6)      ||\
                                ((PIN) == PWR_WAKEUP_PIN1_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN2_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN3_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN4_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN5_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN6_HIGH) ||\
                                ((PIN) == PWR_WAKEUP_PIN1_LOW)  ||\
                                ((PIN) == PWR_WAKEUP_PIN2_LOW)  ||\
                                ((PIN) == PWR_WAKEUP_PIN3_LOW)  ||\
                                ((PIN) == PWR_WAKEUP_PIN4_LOW)  ||\
                                ((PIN) == PWR_WAKEUP_PIN5_LOW)  ||\
                                ((PIN) == PWR_WAKEUP_PIN6_LOW))

/* Check wake up pin polarity parameter */
#define IS_PWR_WAKEUP_PIN_POLARITY(POLARITY) (((POLARITY) == PWR_PIN_POLARITY_HIGH) ||\
                                              ((POLARITY) == PWR_PIN_POLARITY_LOW))

/* Check wake up pin pull configuration parameter */
#define IS_PWR_WAKEUP_PIN_PULL(PULL) (((PULL) == PWR_PIN_NO_PULL) ||\
                                      ((PULL) == PWR_PIN_PULL_UP) ||\
                                      ((PULL) == PWR_PIN_PULL_DOWN))

/* Check wake up flag parameter */
#define IS_PWR_WAKEUP_FLAG(FLAG) (((FLAG) == PWR_WAKEUP_FLAG1) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG2) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG3) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG4) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG5) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG6) ||\
                                  ((FLAG) == PWR_WAKEUP_FLAG_ALL))

/* Check wake up flag parameter */
#define IS_PWR_AVD_LEVEL(LEVEL) (((LEVEL) == PWR_AVDLEVEL_0) ||\
                                 ((LEVEL) == PWR_AVDLEVEL_1) ||\
                                 ((LEVEL) == PWR_AVDLEVEL_2) ||\
                                 ((LEVEL) == PWR_AVDLEVEL_3))

/* Check AVD mode parameter */
#define IS_PWR_AVD_MODE(MODE) (((MODE) == PWR_AVD_MODE_IT_RISING)         ||\
                               ((MODE) == PWR_AVD_MODE_IT_FALLING)        ||\
                               ((MODE) == PWR_AVD_MODE_IT_RISING_FALLING) ||\
                               ((MODE) == PWR_AVD_MODE_EVENT_RISING)      ||\
                               ((MODE) == PWR_AVD_MODE_EVENT_FALLING)     ||\
                               ((MODE) == PWR_AVD_MODE_NORMAL)            ||\
                               ((MODE) == PWR_AVD_MODE_EVENT_RISING_FALLING))

/* Check resistor battery parameter */
#define IS_PWR_BATTERY_RESISTOR_SELECT(RESISTOR) (((RESISTOR) == PWR_BATTERY_CHARGING_RESISTOR_5) ||\
                                                  ((RESISTOR) == PWR_BATTERY_CHARGING_RESISTOR_1_5))
/* Check D1/CD CPU ID parameter */
#define IS_PWR_D1_CPU(CPU) ((CPU) == CM7_CPUID)

#if defined (DUAL_CORE)
/* Check CPU parameter */
#define IS_PWR_CORE(CPU)  (((CPU) == PWR_CORE_CPU1) || ((CPU) == PWR_CORE_CPU2))

/* Check D2 CPU ID parameter */
#define IS_PWR_D2_CPU(CPU) ((CPU) == CM4_CPUID)

/* Check PWR domain flag parameter */
#define IS_PWR_DOMAIN_FLAG(FLAG)  (((FLAG) == PWR_D1_DOMAIN_FLAGS) || \
                                   ((FLAG) == PWR_D2_DOMAIN_FLAGS) || \
                                   ((FLAG) == PWR_ALL_DOMAIN_FLAGS))
#endif /* defined (DUAL_CORE) */

#if defined (PWR_CR1_SRDRAMSO)
/* Check memory block parameter */
#define IS_PWR_MEMORY_BLOCK(BLOCK) (((BLOCK) == PWR_SRD_AHB_MEMORY_BLOCK)     || \
                                    ((BLOCK) == PWR_USB_FDCAN_MEMORY_BLOCK)   || \
                                    ((BLOCK) == PWR_GFXMMU_JPEG_MEMORY_BLOCK) || \
                                    ((BLOCK) == PWR_TCM_ECM_MEMORY_BLOCK)     || \
                                    ((BLOCK) == PWR_RAM1_AHB_MEMORY_BLOCK)    || \
                                    ((BLOCK) == PWR_RAM2_AHB_MEMORY_BLOCK)    || \
                                    ((BLOCK) == PWR_RAM1_AXI_MEMORY_BLOCK)    || \
                                    ((BLOCK) == PWR_RAM2_AXI_MEMORY_BLOCK)    || \
                                    ((BLOCK) == PWR_RAM3_AXI_MEMORY_BLOCK))
#endif /* defined (PWR_CR1_SRDRAMSO) */
/**
  * @}
  */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STM32H7xx_HAL_PWR_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
