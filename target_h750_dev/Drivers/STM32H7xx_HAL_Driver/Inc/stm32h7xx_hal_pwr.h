/**
  ******************************************************************************
  * @file    stm32h7xx_hal_pwr.h
  * @author  MCD Application Team
  * @brief   Header file of PWR HAL module.
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
#ifndef STM32H7xx_HAL_PWR_H
#define STM32H7xx_HAL_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PWR_Exported_Types PWR Exported Types
  * @{
  */

/**
  * @brief  PWR PVD configuration structure definition
  */
typedef struct
{
  uint32_t PVDLevel; /*!< PVDLevel: Specifies the PVD detection level. This
                                    parameter can be a value of @ref
                                    PWR_PVD_detection_level.
                     */

  uint32_t Mode;     /*!< Mode: Specifies the EXTI operating mode for the PVD
                                event. This parameter can be a value of @ref
                                PWR_PVD_Mode.
                     */
}PWR_PVDTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PWR_Exported_Constants PWR Exported Constants
  * @{
  */

/** @defgroup PWR_PVD_detection_level PWR PVD detection level
  * @{
  */
#define PWR_PVDLEVEL_0  PWR_CR1_PLS_LEV0  /*!< Programmable voltage detector
                                               level 0 selection : 1V95       */
#define PWR_PVDLEVEL_1  PWR_CR1_PLS_LEV1  /*!< Programmable voltage detector
                                               level 1 selection : 2V1        */
#define PWR_PVDLEVEL_2  PWR_CR1_PLS_LEV2  /*!< Programmable voltage detector
                                               level 2 selection : 2V25       */
#define PWR_PVDLEVEL_3  PWR_CR1_PLS_LEV3  /*!< Programmable voltage detector
                                               level 3 selection : 2V4        */
#define PWR_PVDLEVEL_4  PWR_CR1_PLS_LEV4  /*!< Programmable voltage detector
                                               level 4 selection : 2V55       */
#define PWR_PVDLEVEL_5  PWR_CR1_PLS_LEV5  /*!< Programmable voltage detector
                                               level 5 selection : 2V7        */
#define PWR_PVDLEVEL_6  PWR_CR1_PLS_LEV6  /*!< Programmable voltage detector
                                               level 6 selection : 2V85       */
#define PWR_PVDLEVEL_7  PWR_CR1_PLS_LEV7  /*!< External input analog voltage
                                               (Compare internally to VREF)   */
/**
  * @}
  */

/** @defgroup PWR_PVD_Mode PWR PVD Mode
  * @{
  */
#define PWR_PVD_MODE_NORMAL               (0x00000000U) /*!< Basic mode is used                                        */
#define PWR_PVD_MODE_IT_RISING            (0x00010001U) /*!< Interrupt Mode with Rising edge trigger detection         */
#define PWR_PVD_MODE_IT_FALLING           (0x00010002U) /*!< Interrupt Mode with Falling edge trigger detection        */
#define PWR_PVD_MODE_IT_RISING_FALLING    (0x00010003U) /*!< Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_PVD_MODE_EVENT_RISING         (0x00020001U) /*!< Event Mode with Rising edge trigger detection             */
#define PWR_PVD_MODE_EVENT_FALLING        (0x00020002U) /*!< Event Mode with Falling edge trigger detection            */
#define PWR_PVD_MODE_EVENT_RISING_FALLING (0x00020003U) /*!< Event Mode with Rising/Falling edge trigger detection     */
/**
  * @}
  */

/** @defgroup PWR_Regulator_state_in_STOP_mode PWR Regulator state in SLEEP/STOP mode
  * @{
  */
#define PWR_MAINREGULATOR_ON      (0U)
#define PWR_LOWPOWERREGULATOR_ON  PWR_CR1_LPDS
/**
  * @}
  */

/** @defgroup PWR_SLEEP_mode_entry PWR SLEEP mode entry
  * @{
  */
#define PWR_SLEEPENTRY_WFI  (0x01U)
#define PWR_SLEEPENTRY_WFE  (0x02U)
/**
  * @}
  */

/** @defgroup PWR_STOP_mode_entry PWR STOP mode entry
  * @{
  */
#define PWR_STOPENTRY_WFI  (0x01U)
#define PWR_STOPENTRY_WFE  (0x02U)
/**
  * @}
  */

/** @defgroup PWR_Regulator_Voltage_Scale PWR Regulator Voltage Scale
  * @{
  */
#if defined(PWR_SRDCR_VOS)
#define PWR_REGULATOR_VOLTAGE_SCALE0  (PWR_SRDCR_VOS_1 | PWR_SRDCR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE1  (PWR_SRDCR_VOS_1)
#define PWR_REGULATOR_VOLTAGE_SCALE2  (PWR_SRDCR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE3  (0U)
#else
#define PWR_REGULATOR_VOLTAGE_SCALE0  (0U)
#define PWR_REGULATOR_VOLTAGE_SCALE1  (PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE2  (PWR_D3CR_VOS_1)
#define PWR_REGULATOR_VOLTAGE_SCALE3  (PWR_D3CR_VOS_0)
#endif /* PWR_SRDCR_VOS */
/**
  * @}
  */

/** @defgroup PWR_Flag PWR Flag
  * @{
  */
/* PWR CPU flag */
#define PWR_FLAG_STOP       (0x01U)
#if defined (PWR_CPUCR_SBF_D2)
#define PWR_FLAG_SB_D1      (0x02U)
#define PWR_FLAG_SB_D2      (0x03U)
#endif /* defined (PWR_CPUCR_SBF_D2) */
#define PWR_FLAG_SB         (0x04U)
#if defined (DUAL_CORE)
#define PWR_FLAG_CPU_HOLD   (0x05U)
#define PWR_FLAG_CPU2_HOLD  (0x06U)
#define PWR_FLAG2_STOP      (0x07U)
#define PWR_FLAG2_SB_D1     (0x08U)
#define PWR_FLAG2_SB_D2     (0x09U)
#define PWR_FLAG2_SB        (0x0AU)
#endif /* defined (DUAL_CORE) */
#define PWR_FLAG_PVDO       (0x0BU)
#define PWR_FLAG_AVDO       (0x0CU)
#define PWR_FLAG_ACTVOSRDY  (0x0DU)
#define PWR_FLAG_ACTVOS     (0x0EU)
#define PWR_FLAG_BRR        (0x0FU)
#define PWR_FLAG_VOSRDY     (0x10U)
#if defined (SMPS)
#define PWR_FLAG_SMPSEXTRDY (0x11U)
#else
#define PWR_FLAG_SCUEN      (0x11U)
#endif /* defined (SMPS) */
#if defined (PWR_CSR1_MMCVDO)
#define PWR_FLAG_MMCVDO     (0x12U)
#endif /* defined (PWR_CSR1_MMCVDO) */
#define PWR_FLAG_USB33RDY   (0x13U)
#define PWR_FLAG_TEMPH      (0x14U)
#define PWR_FLAG_TEMPL      (0x15U)
#define PWR_FLAG_VBATH      (0x16U)
#define PWR_FLAG_VBATL      (0x17U)

/* PWR Wake up flag */
#define PWR_FLAG_WKUP1 PWR_WKUPCR_WKUPC1
#define PWR_FLAG_WKUP2 PWR_WKUPCR_WKUPC2
#define PWR_FLAG_WKUP3 PWR_WKUPCR_WKUPC3
#define PWR_FLAG_WKUP4 PWR_WKUPCR_WKUPC4
#define PWR_FLAG_WKUP5 PWR_WKUPCR_WKUPC5
#define PWR_FLAG_WKUP6 PWR_WKUPCR_WKUPC6
/**
  * @}
  */

/** @defgroup PWR_ENABLE_WUP_Mask PWR Enable WUP Mask
  * @{
  */
#define  PWR_EWUP_MASK  (0x0FFF3F3FU)
/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/** @defgroup PWR_Exported_Macro PWR Exported Macro
  * @{
  */

/** @brief  Configure the main internal regulator output voltage.
  * @param  __REGULATOR__ : Specifies the regulator output voltage to achieve a
  *                         trade-off between performance and power consumption
  *                         when the device does not operate at the maximum
  *                         frequency (refer to the datasheet for more details).
  *          This parameter can be one of the following values:
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE0 : Regulator voltage output
  *                                                Scale 0 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE1 : Regulator voltage output
  *                                                Scale 1 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE2 : Regulator voltage output
  *                                                Scale 2 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE3 : Regulator voltage output
  *                                                Scale 3 mode.
  * @note   For all H7 lines except STM32H7Axxx and STM32H7Bxxx lines,
  *         configuring Voltage Scale 0 is only possible when Vcore is supplied
  *         from LDO (Low DropOut). The SYSCFG Clock must be enabled through
  *         __HAL_RCC_SYSCFG_CLK_ENABLE() macro before configuring Voltage Scale
  *         0 using __HAL_PWR_VOLTAGESCALING_CONFIG().
  *         Transition to Voltage Scale 0 is only possible when the system is
  *         already in Voltage Scale 1.
  *         Transition from Voltage Scale 0 is only possible to Voltage Scale 1
  *         then once in Voltage Scale 1 it is possible to switch to another
  *         voltage scale.
  *         After each regulator voltage setting, wait on VOSRDY flag to be set
  *         using macro __HAL_PWR_GET_FLAG().
  *         To enter low power mode , and if current regulator voltage is
  *         Voltage Scale 0 then first switch to Voltage Scale 1 before entering
  *         low power mode.
  * @retval None.
  */
#if defined(PWR_SRDCR_VOS) /* STM32H7Axxx and STM32H7Bxxx lines */
#define __HAL_PWR_VOLTAGESCALING_CONFIG(__REGULATOR__)                         \
do {                                                                           \
      __IO uint32_t tmpreg = 0x00;                                             \
      MODIFY_REG(PWR->SRDCR, PWR_SRDCR_VOS, (__REGULATOR__));                  \
      /* Delay after setting the voltage scaling */                            \
      tmpreg = READ_BIT(PWR->SRDCR, PWR_SRDCR_VOS);                            \
      UNUSED(tmpreg);                                                          \
} while(0)
#else /* All H7 lines except STM32H7Axxx and STM32H7Bxxx lines */
#define __HAL_PWR_VOLTAGESCALING_CONFIG(__REGULATOR__)                         \
do {                                                                           \
      __IO uint32_t tmpreg = 0x00;                                             \
      /* Check the voltage scaling to be configured */                         \
      if((__REGULATOR__) == PWR_REGULATOR_VOLTAGE_SCALE0)                      \
      {                                                                        \
        /* Configure the Voltage Scaling 1 */                                  \
        MODIFY_REG(PWR->D3CR, PWR_D3CR_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);     \
        /* Delay after setting the voltage scaling */                          \
        tmpreg = READ_BIT(PWR->D3CR, PWR_D3CR_VOS);                            \
        /* Enable the PWR overdrive */                                         \
        SET_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);                             \
        /* Delay after setting the syscfg boost setting */                     \
        tmpreg = READ_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);                   \
      }                                                                        \
      else                                                                     \
      {                                                                        \
        /* Disable the PWR overdrive */                                        \
        CLEAR_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);                           \
        /* Delay after setting the syscfg boost setting */                     \
        tmpreg = READ_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);                   \
        /* Configure the Voltage Scaling x */                                  \
        MODIFY_REG(PWR->D3CR, PWR_D3CR_VOS, (__REGULATOR__));                  \
        /* Delay after setting the voltage scaling */                          \
        tmpreg = READ_BIT(PWR->D3CR, PWR_D3CR_VOS);                            \
      }                                                                        \
      UNUSED(tmpreg);                                                          \
} while(0)
#endif /* PWR_SRDCR_VOS */

/** @brief  Check PWR flags are set or not.
  * @param  __FLAG__ : Specifies the flag to check.
  *           This parameter can be one of the following values:
  *            @arg PWR_FLAG_PVDO : PVD Output. This flag is valid only if PVD
  *                                 is enabled by the HAL_PWR_EnablePVD()
  *                                 function.
  *                                 The PVD is stopped by STANDBY mode. For this
  *                                 reason, this bit is equal to 0 after STANDBY
  *                                 or reset until the PVDE bit is set.
  *            @arg PWR_FLAG_AVDO : AVD Output. This flag is valid only if AVD
  *                                 is enabled by the HAL_PWREx_EnableAVD()
  *                                 function. The AVD is stopped by STANDBY mode.
  *                                 For this reason, this bit is equal to 0
  *                                 after STANDBY or reset until the AVDE bit
  *                                 is set.
  *            @arg PWR_FLAG_ACTVOSRDY : This flag indicates that the Regulator
  *                                      voltage scaling output selection is
  *                                      ready.
  *            @arg PWR_FLAG_BRR : Backup regulator ready flag. This bit is not
  *                                reset when the device wakes up from STANDBY
  *                                mode or by a system reset or power-on reset.
  *            @arg PWR_FLAG_VOSRDY : This flag indicates that the Regulator
  *                                   voltage scaling output selection is ready.
  *                                mode or by a system reset or power-on reset.
  *            @arg PWR_FLAG_USB33RDY : This flag indicates that the USB supply
  *                                     from regulator is ready.
  *            @arg PWR_FLAG_TEMPH : This flag indicates that the temperature
  *                                  equal or above high threshold level.
  *            @arg PWR_FLAG_TEMPL : This flag indicates that the temperature
  *                                  equal or below low threshold level.
  *            @arg PWR_FLAG_VBATH : This flag indicates that VBAT level equal
  *                                  or above high threshold level.
  *            @arg PWR_FLAG_VBATL : This flag indicates that VBAT level equal
  *                                  or below low threshold level.
  *            @arg PWR_FLAG_STOP : This flag indicates that the system entered
  *                                 in STOP mode.
  *            @arg PWR_FLAG_SB : This flag indicates that the system entered in
  *                               STANDBY mode.
  *            @arg PWR_FLAG_SB_D1 : This flag indicates that the D1 domain
  *                                  entered in STANDBY mode.
  *            @arg PWR_FLAG_SB_D2 : This flag indicates that the D2 domain
  *                                  entered in STANDBY mode.
  *            @arg PWR_FLAG2_STOP : This flag indicates that the system entered
  *                                 in STOP mode.
  *            @arg PWR_FLAG2_SB : This flag indicates that the system entered
  *                                in STANDBY mode.
  *            @arg PWR_FLAG2_SB_D1 : This flag indicates that the D1 domain
  *                                   entered in STANDBY mode.
  *            @arg PWR_FLAG2_SB_D2 : This flag indicates that the D2 domain
  *                                   entered in STANDBY mode.
  *            @arg PWR_FLAG_CPU_HOLD : This flag indicates that the CPU1 wakes
  *                                     up with hold.
  *            @arg PWR_FLAG_CPU2_HOLD : This flag indicates that the CPU2 wakes
  *                                      up with hold.
  *            @arg PWR_FLAG_SMPSEXTRDY : This flag indicates that the SMPS
  *                                       External supply is sready.
  *            @arg PWR_FLAG_SCUEN : This flag indicates that the supply
  *                                  configuration update is enabled.
  *            @arg PWR_FLAG_MMCVDO : This flag indicates that the VDDMMC is
  *                                   above or equal to 1.2 V.
  * @note   The PWR_FLAG_PVDO, PWR_FLAG_AVDO, PWR_FLAG_ACTVOSRDY, PWR_FLAG_BRR,
  *         PWR_FLAG_VOSRDY, PWR_FLAG_USB33RDY, PWR_FLAG_TEMPH, PWR_FLAG_TEMPL,
  *         PWR_FLAG_VBATH, PWR_FLAG_VBATL, PWR_FLAG_STOP and PWR_FLAG_SB flags
  *         are used for all H7 family lines.
  *         The PWR_FLAG2_STOP, PWR_FLAG2_SB, PWR_FLAG2_SB_D1, PWR_FLAG2_SB_D2,
  *         PWR_FLAG_CPU_HOLD and PWR_FLAG_CPU2_HOLD flags are used only for H7
  *         dual core lines.
  *         The PWR_FLAG_SB_D1 and PWR_FLAG_SB_D2 flags are used for all H7
  *         family except STM32H7Axxx and STM32H7Bxxx lines.
  *         The PWR_FLAG_MMCVDO flag is used only for STM32H7Axxx and
  *         STM32H7Bxxx lines.
  *         The PWR_FLAG_SCUEN flag is used for STM32H743, STM32H753, STM32H742,
  *         STM32H750, STM32H7Axx and STM32H7Bxx lines.
  *         The PWR_FLAG_SMPSEXTRDY flag is used for dual core and STM32H7AxxQ,
  *         STM32H7BxxQ lines.
  * @retval The (__FLAG__) state (TRUE or FALSE).
  */
#if defined (DUAL_CORE) /* Dual core lines */
#define __HAL_PWR_GET_FLAG(__FLAG__)                                                              \
(((__FLAG__) == PWR_FLAG_PVDO)       ? ((PWR->CSR1 & PWR_CSR1_PVDO)       == PWR_CSR1_PVDO)      :\
 ((__FLAG__) == PWR_FLAG_AVDO)       ? ((PWR->CSR1 & PWR_CSR1_AVDO)       == PWR_CSR1_AVDO)      :\
 ((__FLAG__) == PWR_FLAG_ACTVOSRDY)  ? ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY)  == PWR_CSR1_ACTVOSRDY) :\
 ((__FLAG__) == PWR_FLAG_VOSRDY)     ? ((PWR->D3CR & PWR_D3CR_VOSRDY)     == PWR_D3CR_VOSRDY)    :\
 ((__FLAG__) == PWR_FLAG_SMPSEXTRDY) ? ((PWR->CR3 & PWR_CR3_SMPSEXTRDY)   == PWR_CR3_SMPSEXTRDY) :\
 ((__FLAG__) == PWR_FLAG_BRR)        ? ((PWR->CR2 & PWR_CR2_BRRDY)        == PWR_CR2_BRRDY)      :\
 ((__FLAG__) == PWR_FLAG_CPU_HOLD)   ? ((PWR->CPU2CR & PWR_CPU2CR_HOLD1F) == PWR_CPU2CR_HOLD1F)  :\
 ((__FLAG__) == PWR_FLAG_CPU2_HOLD)  ? ((PWR->CPUCR & PWR_CPUCR_HOLD2F)   == PWR_CPUCR_HOLD2F)   :\
 ((__FLAG__) == PWR_FLAG_SB)         ? ((PWR->CPUCR & PWR_CPUCR_SBF)      == PWR_CPUCR_SBF)      :\
 ((__FLAG__) == PWR_FLAG2_SB)        ? ((PWR->CPU2CR & PWR_CPU2CR_SBF)    == PWR_CPU2CR_SBF)     :\
 ((__FLAG__) == PWR_FLAG_STOP)       ? ((PWR->CPUCR & PWR_CPUCR_STOPF)    == PWR_CPUCR_STOPF)    :\
 ((__FLAG__) == PWR_FLAG2_STOP)      ? ((PWR->CPU2CR & PWR_CPU2CR_STOPF)  == PWR_CPU2CR_STOPF)   :\
 ((__FLAG__) == PWR_FLAG_SB_D1)      ? ((PWR->CPUCR & PWR_CPUCR_SBF_D1)   == PWR_CPUCR_SBF_D1)   :\
 ((__FLAG__) == PWR_FLAG2_SB_D1)     ? ((PWR->CPU2CR & PWR_CPU2CR_SBF_D1) == PWR_CPU2CR_SBF_D1)  :\
 ((__FLAG__) == PWR_FLAG_SB_D2)      ? ((PWR->CPUCR & PWR_CPUCR_SBF_D2)   == PWR_CPUCR_SBF_D2)   :\
 ((__FLAG__) == PWR_FLAG2_SB_D2)     ? ((PWR->CPU2CR & PWR_CPU2CR_SBF_D2) == PWR_CPU2CR_SBF_D2)  :\
 ((__FLAG__) == PWR_FLAG_USB33RDY)   ? ((PWR->CR3 & PWR_CR3_USB33RDY)     == PWR_CR3_USB33RDY)   :\
 ((__FLAG__) == PWR_FLAG_TEMPH)      ? ((PWR->CR2 & PWR_CR2_TEMPH)        == PWR_CR2_TEMPH)      :\
 ((__FLAG__) == PWR_FLAG_TEMPL)      ? ((PWR->CR2 & PWR_CR2_TEMPL)        == PWR_CR2_TEMPL)      :\
 ((__FLAG__) == PWR_FLAG_VBATH)      ? ((PWR->CR2 & PWR_CR2_VBATH)        == PWR_CR2_VBATH)      :\
 ((PWR->CR2 & PWR_CR2_VBATL) == PWR_CR2_VBATL))
#else /* Single core lines */
#if defined (PWR_CPUCR_SBF_D2) /* STM32H743, STM32H753, STM32H742 and STM32H750 lines */
#define __HAL_PWR_GET_FLAG(__FLAG__)                                                            \
(((__FLAG__) == PWR_FLAG_PVDO)      ? ((PWR->CSR1 & PWR_CSR1_PVDO)      == PWR_CSR1_PVDO)      :\
 ((__FLAG__) == PWR_FLAG_AVDO)      ? ((PWR->CSR1 & PWR_CSR1_AVDO)      == PWR_CSR1_AVDO)      :\
 ((__FLAG__) == PWR_FLAG_ACTVOSRDY) ? ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY) == PWR_CSR1_ACTVOSRDY) :\
 ((__FLAG__) == PWR_FLAG_VOSRDY)    ? ((PWR->D3CR & PWR_D3CR_VOSRDY)    == PWR_D3CR_VOSRDY)    :\
 ((__FLAG__) == PWR_FLAG_SCUEN)     ? ((PWR->CR3 & PWR_CR3_SCUEN)       == PWR_CR3_SCUEN)      :\
 ((__FLAG__) == PWR_FLAG_BRR)       ? ((PWR->CR2 & PWR_CR2_BRRDY)       == PWR_CR2_BRRDY)      :\
 ((__FLAG__) == PWR_FLAG_SB)        ? ((PWR->CPUCR & PWR_CPUCR_SBF)     == PWR_CPUCR_SBF)      :\
 ((__FLAG__) == PWR_FLAG_STOP)      ? ((PWR->CPUCR & PWR_CPUCR_STOPF)   == PWR_CPUCR_STOPF)    :\
 ((__FLAG__) == PWR_FLAG_SB_D1)     ? ((PWR->CPUCR & PWR_CPUCR_SBF_D1)  == PWR_CPUCR_SBF_D1)   :\
 ((__FLAG__) == PWR_FLAG_SB_D2)     ? ((PWR->CPUCR & PWR_CPUCR_SBF_D2)  == PWR_CPUCR_SBF_D2)   :\
 ((__FLAG__) == PWR_FLAG_USB33RDY)  ? ((PWR->CR3 & PWR_CR3_USB33RDY)    == PWR_CR3_USB33RDY)   :\
 ((__FLAG__) == PWR_FLAG_TEMPH)     ? ((PWR->CR2 & PWR_CR2_TEMPH)       == PWR_CR2_TEMPH)      :\
 ((__FLAG__) == PWR_FLAG_TEMPL)     ? ((PWR->CR2 & PWR_CR2_TEMPL)       == PWR_CR2_TEMPL)      :\
 ((__FLAG__) == PWR_FLAG_VBATH)     ? ((PWR->CR2 & PWR_CR2_VBATH)       == PWR_CR2_VBATH)      :\
 ((PWR->CR2 & PWR_CR2_VBATL) == PWR_CR2_VBATL))
#else /* STM32H7Axxx and STM32H7Bxxx lines */
#if defined (SMPS) /* STM32H7AxxQ and STM32H7BxxQ SMPS lines */
#define __HAL_PWR_GET_FLAG(__FLAG__)                                                            \
(((__FLAG__) == PWR_FLAG_PVDO)       ? ((PWR->CSR1 & PWR_CSR1_PVDO)      == PWR_CSR1_PVDO)      :\
 ((__FLAG__) == PWR_FLAG_AVDO)       ? ((PWR->CSR1 & PWR_CSR1_AVDO)      == PWR_CSR1_AVDO)      :\
 ((__FLAG__) == PWR_FLAG_ACTVOSRDY)  ? ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY) == PWR_CSR1_ACTVOSRDY) :\
 ((__FLAG__) == PWR_FLAG_BRR)        ? ((PWR->CR2 & PWR_CR2_BRRDY)       == PWR_CR2_BRRDY)      :\
 ((__FLAG__) == PWR_FLAG_VOSRDY)     ? ((PWR->SRDCR & PWR_SRDCR_VOSRDY)  == PWR_SRDCR_VOSRDY)   :\
 ((__FLAG__) == PWR_FLAG_STOP)       ? ((PWR->CPUCR & PWR_CPUCR_STOPF)   == PWR_CPUCR_STOPF)    :\
 ((__FLAG__) == PWR_FLAG_SB)         ? ((PWR->CPUCR & PWR_CPUCR_SBF)     == PWR_CPUCR_SBF)      :\
 ((__FLAG__) == PWR_FLAG_MMCVDO)     ? ((PWR->CSR1 & PWR_CSR1_MMCVDO)    == PWR_CSR1_MMCVDO)    :\
 ((__FLAG__) == PWR_FLAG_SMPSEXTRDY) ? ((PWR->CR3 & PWR_CR3_SMPSEXTRDY)  == PWR_CR3_SMPSEXTRDY) :\
 ((__FLAG__) == PWR_FLAG_USB33RDY)   ? ((PWR->CR3 & PWR_CR3_USB33RDY)    == PWR_CR3_USB33RDY)   :\
 ((__FLAG__) == PWR_FLAG_TEMPH)      ? ((PWR->CR2 & PWR_CR2_TEMPH)       == PWR_CR2_TEMPH)      :\
 ((__FLAG__) == PWR_FLAG_TEMPL)      ? ((PWR->CR2 & PWR_CR2_TEMPL)       == PWR_CR2_TEMPL)      :\
 ((__FLAG__) == PWR_FLAG_VBATH)      ? ((PWR->CR2 & PWR_CR2_VBATH)       == PWR_CR2_VBATH)      :\
 ((PWR->CR2 & PWR_CR2_VBATL) == PWR_CR2_VBATL))
#else /* STM32H7Axx and STM32H7Bxx LDO lines */
#define __HAL_PWR_GET_FLAG(__FLAG__)                                    \
(((__FLAG__) == PWR_FLAG_PVDO)      ? ((PWR->CSR1 & PWR_CSR1_PVDO)      == PWR_CSR1_PVDO)      :\
 ((__FLAG__) == PWR_FLAG_AVDO)      ? ((PWR->CSR1 & PWR_CSR1_AVDO)      == PWR_CSR1_AVDO)      :\
 ((__FLAG__) == PWR_FLAG_ACTVOSRDY) ? ((PWR->CSR1 & PWR_CSR1_ACTVOSRDY) == PWR_CSR1_ACTVOSRDY) :\
 ((__FLAG__) == PWR_FLAG_BRR)       ? ((PWR->CR2 & PWR_CR2_BRRDY)       == PWR_CR2_BRRDY)      :\
 ((__FLAG__) == PWR_FLAG_VOSRDY)    ? ((PWR->SRDCR & PWR_SRDCR_VOSRDY)  == PWR_SRDCR_VOSRDY)   :\
 ((__FLAG__) == PWR_FLAG_SCUEN)     ? ((PWR->CR3 & PWR_CR3_SCUEN)       == PWR_CR3_SCUEN)      :\
 ((__FLAG__) == PWR_FLAG_STOP)      ? ((PWR->CPUCR & PWR_CPUCR_STOPF)   == PWR_CPUCR_STOPF)    :\
 ((__FLAG__) == PWR_FLAG_SB)        ? ((PWR->CPUCR & PWR_CPUCR_SBF)     == PWR_CPUCR_SBF)      :\
 ((__FLAG__) == PWR_FLAG_MMCVDO)    ? ((PWR->CSR1 & PWR_CSR1_MMCVDO)    == PWR_CSR1_MMCVDO)    :\
 ((__FLAG__) == PWR_FLAG_USB33RDY)  ? ((PWR->CR3 & PWR_CR3_USB33RDY)    == PWR_CR3_USB33RDY)   :\
 ((__FLAG__) == PWR_FLAG_TEMPH)     ? ((PWR->CR2 & PWR_CR2_TEMPH)       == PWR_CR2_TEMPH)      :\
 ((__FLAG__) == PWR_FLAG_TEMPL)     ? ((PWR->CR2 & PWR_CR2_TEMPL)       == PWR_CR2_TEMPL)      :\
 ((__FLAG__) == PWR_FLAG_VBATH)     ? ((PWR->CR2 & PWR_CR2_VBATH)       == PWR_CR2_VBATH)      :\
 ((PWR->CR2 & PWR_CR2_VBATL) == PWR_CR2_VBATL))
#endif /* SMPS */
#endif /* PWR_CPUCR_SBF_D2 */
#endif /* DUAL_CORE */

/** @brief  Check PWR wake up flags are set or not.
  * @param  __FLAG__: specifies the wake up flag to check.
  *           This parameter can be one of the following values:
  *            @arg PWR_FLAG_WKUP1 : This parameter clear Wake up line 1 flag.
  *            @arg PWR_FLAG_WKUP2 : This parameter clear Wake up line 2 flag.
  *            @arg PWR_FLAG_WKUP3 : This parameter clear Wake up line 3 flag.
  *            @arg PWR_FLAG_WKUP4 : This parameter clear Wake up line 4 flag.
  *            @arg PWR_FLAG_WKUP5 : This parameter clear Wake up line 5 flag.
  *            @arg PWR_FLAG_WKUP6 : This parameter clear Wake up line 6 flag.
  * @retval The (__FLAG__) state (TRUE or FALSE).
  */
#define __HAL_PWR_GET_WAKEUPFLAG(__FLAG__) ((PWR->WKUPFR & (__FLAG__)) ? 0 : 1)

#if defined (DUAL_CORE)
/** @brief  Clear CPU PWR flags.
  * @param  __FLAG__ : Specifies the flag to clear.
  * @note   This parameter is not used for the STM32H7 family and is kept as
  *         parameter just to maintain compatibility with other families.
  * @note   This macro clear all CPU flags STOPF, SBF, SBF_D1, and SBF_D2.
  *           This parameter can be one of the following values :
  *            @arg PWR_CPU_FLAGS : Clear HOLD2F, STOPF, SBF, SBF_D1, and SBF_D2
  *                                 CPU flags.
  * @retval None.
  */
#define __HAL_PWR_CLEAR_FLAG(__FLAG__)      \
do {                                        \
     SET_BIT(PWR->CPUCR, PWR_CPUCR_CSSF);   \
     SET_BIT(PWR->CPU2CR, PWR_CPU2CR_CSSF); \
} while(0)
#else
/** @brief  Clear CPU PWR flags.
  * @param  __FLAG__ : Specifies the flag to clear.
  * @note   This parameter is not used for the STM32H7 family and is kept as
  *         parameter just to maintain compatibility with other families.
  * @note   This macro clear all CPU flags.
  *         For STM32H7Axxx and STM32H7Bxxx lines CPU flags are STOPF and SBF.
  *         For dual core lines flags are HOLDxF, STOPF, SBF and SBF_Dx.
  * @retval None.
  */
#define __HAL_PWR_CLEAR_FLAG(__FLAG__) SET_BIT(PWR->CPUCR, PWR_CPUCR_CSSF)
#endif /* defined (DUAL_CORE) */

/** @brief  Clear PWR wake up flags.
  * @param  __FLAG__ : Specifies the wake up flag to be cleared.
  *           This parameter can be one of the following values :
  *            @arg PWR_FLAG_WKUP1 : This parameter clear Wake up line 1 flag.
  *            @arg PWR_FLAG_WKUP2 : This parameter clear Wake up line 2 flag.
  *            @arg PWR_FLAG_WKUP3 : This parameter clear Wake up line 3 flag.
  *            @arg PWR_FLAG_WKUP4 : This parameter clear Wake up line 4 flag.
  *            @arg PWR_FLAG_WKUP5 : This parameter clear Wake up line 5 flag.
  *            @arg PWR_FLAG_WKUP6 : This parameter clear Wake up line 6 flag.
  * @retval None.
  */
#define __HAL_PWR_CLEAR_WAKEUPFLAG(__FLAG__) SET_BIT(PWR->WKUPCR, (__FLAG__))

/**
  * @brief Enable the PVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR1, PWR_EXTI_LINE_PVD)

#if defined (DUAL_CORE)
/**
  * @brief Enable the PVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTID2_ENABLE_IT() SET_BIT(EXTI_D2->IMR1, PWR_EXTI_LINE_PVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Disable the PVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR1, PWR_EXTI_LINE_PVD)

#if defined (DUAL_CORE)
/**
  * @brief Disable the PVD EXTI D2 Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTID2_DISABLE_IT() CLEAR_BIT(EXTI_D2->IMR1, PWR_EXTI_LINE_PVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief   Enable event on PVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR1, PWR_EXTI_LINE_PVD)

#if defined (DUAL_CORE)
/**
  * @brief Enable event on PVD EXTI D2 Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTID2_ENABLE_EVENT() SET_BIT(EXTI_D2->EMR1, PWR_EXTI_LINE_PVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief   Disable event on PVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR1, PWR_EXTI_LINE_PVD)

#if defined (DUAL_CORE)
/**
  * @brief Disable event on PVD EXTI D2 Line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTID2_DISABLE_EVENT() CLEAR_BIT(EXTI_D2->EMR1, PWR_EXTI_LINE_PVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Enable the PVD Rising Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR1, PWR_EXTI_LINE_PVD)

/**
  * @brief Disable the PVD Rising Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR1, PWR_EXTI_LINE_PVD)

/**
  * @brief Enable the PVD Falling Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR1, PWR_EXTI_LINE_PVD)

/**
  * @brief Disable the PVD Falling Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR1, PWR_EXTI_LINE_PVD)

/**
  * @brief Enable the PVD Rising & Falling Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_FALLING_EDGE() \
do {                                                    \
      __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE();          \
      __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE();         \
} while(0);

/**
  * @brief Disable the PVD Rising & Falling Interrupt Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_FALLING_EDGE() \
do {                                                     \
      __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE();          \
      __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE();         \
} while(0);

/**
  * @brief Check whether the specified PVD EXTI interrupt flag is set or not.
  * @retval EXTI PVD Line Status.
  */
#define __HAL_PWR_PVD_EXTI_GET_FLAG() ((READ_BIT(EXTI->PR1, PWR_EXTI_LINE_PVD) == PWR_EXTI_LINE_PVD) ? 1UL : 0UL)

#if defined (DUAL_CORE)
/**
  * @brief Checks whether the specified PVD EXTI interrupt flag is set or not.
  * @retval EXTI D2 PVD Line Status.
  */
#define __HAL_PWR_PVD_EXTID2_GET_FLAG() ((READ_BIT(EXTI_D2->PR1, PWR_EXTI_LINE_PVD) == PWR_EXTI_LINE_PVD) ? 1UL : 0UL)
#endif /* defined (DUAL_CORE) */

/**
  * @brief Clear the PVD EXTI flag.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_CLEAR_FLAG() SET_BIT(EXTI->PR1, PWR_EXTI_LINE_PVD)

#if defined (DUAL_CORE)
/**
  * @brief Clear the PVD EXTI D2 flag.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTID2_CLEAR_FLAG() SET_BIT(EXTI_D2->PR1, PWR_EXTI_LINE_PVD)
#endif /* defined (DUAL_CORE) */

/**
  * @brief  Generates a Software interrupt on PVD EXTI line.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER1, PWR_EXTI_LINE_PVD)
/**
  * @}
  */

/* Include PWR HAL Extension module */
#include "stm32h7xx_hal_pwr_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup PWR_Exported_Functions PWR Exported Functions
  * @{
  */

/** @addtogroup PWR_Exported_Functions_Group1 Initialization and De-Initialization functions
  * @{
  */
/* Initialization and de-initialization functions *****************************/
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);
/**
  * @}
  */

/** @addtogroup PWR_Exported_Functions_Group2 Peripheral Control functions
  * @{
  */
/* Peripheral Control functions  **********************************************/
/* PVD configuration */
void HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

/* WakeUp pins configuration */
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

/* Low Power modes entry */
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

/* Power PVD IRQ Handler */
void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);

/* Cortex System Control functions  *******************************************/
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup PWR_Private_Constants PWR Private Constants
  * @{
  */

/** @defgroup PWR_PVD_EXTI_Line PWR PVD EXTI Line
  * @{
  */
#define PWR_EXTI_LINE_PVD  EXTI_IMR1_IM16 /*!< External interrupt line 16
                                               Connected to the PVD EXTI Line */
/**
  * @}
  */

/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/** @defgroup PWR_Private_Macros PWR Private Macros
  * @{
  */

/** @defgroup PWR_IS_PWR_Definitions PWR Private macros to check input parameters
  * @{
  */
/* Check PVD level parameter */
#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLEVEL_0) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_1) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_2) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_3) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_4) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_5) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_6) ||\
                                 ((LEVEL) == PWR_PVDLEVEL_7))

/* Check PVD mode parameter */
#define IS_PWR_PVD_MODE(MODE) (((MODE) == PWR_PVD_MODE_IT_RISING)            ||\
                               ((MODE) == PWR_PVD_MODE_IT_FALLING)           ||\
                               ((MODE) == PWR_PVD_MODE_IT_RISING_FALLING)    ||\
                               ((MODE) == PWR_PVD_MODE_EVENT_RISING)         ||\
                               ((MODE) == PWR_PVD_MODE_EVENT_FALLING)        ||\
                               ((MODE) == PWR_PVD_MODE_EVENT_RISING_FALLING) ||\
                               ((MODE) == PWR_PVD_MODE_NORMAL))

/* Check low power regulator parameter */
#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_MAINREGULATOR_ON)   ||\
                                     ((REGULATOR) == PWR_LOWPOWERREGULATOR_ON))

/* Check low power mode entry parameter */
#define IS_PWR_SLEEP_ENTRY(ENTRY) (((ENTRY) == PWR_SLEEPENTRY_WFI) ||\
                                   ((ENTRY) == PWR_SLEEPENTRY_WFE))

/* Check low power mode entry parameter */
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPENTRY_WFI) ||\
                                  ((ENTRY) == PWR_STOPENTRY_WFE))

/* Check voltage scale level parameter */
#define IS_PWR_REGULATOR_VOLTAGE(VOLTAGE) (((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE0) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE1) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE2) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE3))
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

#endif /* STM32H7xx_HAL_PWR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
