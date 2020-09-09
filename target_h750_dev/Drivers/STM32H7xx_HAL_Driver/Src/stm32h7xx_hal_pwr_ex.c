/**
  ******************************************************************************
  * @file    stm32h7xx_hal_pwr_ex.c
  * @author  MCD Application Team
  * @brief   Extended PWR HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of PWR extension peripheral:
  *           + Peripheral Extended features functions
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) Call HAL_PWREx_ConfigSupply() function to configure the regulator supply
       with the following different setups according to hardware (support SMPS):
       (+) PWR_DIRECT_SMPS_SUPPLY
       (+) PWR_SMPS_1V8_SUPPLIES_LDO
       (+) PWR_SMPS_2V5_SUPPLIES_LDO
       (+) PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO
       (+) PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO
       (+) PWR_SMPS_1V8_SUPPLIES_EXT
       (+) PWR_SMPS_2V5_SUPPLIES_EXT
       (+) PWR_LDO_SUPPLY
       (+) PWR_EXTERNAL_SOURCE_SUPPLY

   (#) Call HAL_PWREx_GetSupplyConfig() function to get the current supply setup.

   (#) Call HAL_PWREx_ControlVoltageScaling() function to configure the main
       internal regulator output voltage. The voltage scaling could be one of
       the following scales :
       (+) PWR_REGULATOR_VOLTAGE_SCALE0
       (+) PWR_REGULATOR_VOLTAGE_SCALE1
       (+) PWR_REGULATOR_VOLTAGE_SCALE2
       (+) PWR_REGULATOR_VOLTAGE_SCALE3

   (#) Call HAL_PWREx_GetVoltageRange() function to get the current output
       voltage applied to the main regulator.

   (#) Call HAL_PWREx_ControlStopModeVoltageScaling() function to configure the
       main internal regulator output voltage in STOP mode. The voltage scaling
       in STOP mode could be one of the following scales :
       (+) PWR_REGULATOR_SVOS_SCALE3
       (+) PWR_REGULATOR_SVOS_SCALE4
       (+) PWR_REGULATOR_SVOS_SCALE5

   (#) Call HAL_PWREx_GetStopModeVoltageRange() function to get the current
       output voltage applied to the main regulator in STOP mode.

   (#) Call HAL_PWREx_EnterSTOP2Mode() function to enter the system in STOP mode
       with core domain in D2STOP mode. This API is used only for STM32H7Axxx
       and STM32H7Bxxx devices.
       Please ensure to clear all CPU pending events by calling
       HAL_PWREx_ClearPendingEvent() function when trying to enter the Cortex-Mx
       in DEEP-SLEEP mode with __WFE() entry.

   (#) Call HAL_PWREx_EnterSTOPMode() function to enter the selected domain in
       DSTOP mode. Call this API with all available power domains to enter the
       system in STOP mode.
       Please ensure to clear all CPU pending events by calling
       HAL_PWREx_ClearPendingEvent() function when trying to enter the Cortex-Mx
       in DEEP-SLEEP mode with __WFE() entry.

   (#) Call HAL_PWREx_ClearPendingEvent() function always before entring the
       Cortex-Mx in any low power mode (SLEEP/DEEP-SLEEP) using WFE entry.

   (#) Call HAL_PWREx_EnterSTANDBYMode() function to enter the selected domain
       in DSTANDBY mode. Call this API with all available power domains to enter
       the system in STANDBY mode.

   (#) Call HAL_PWREx_ConfigD3Domain() function to setup the D3/SRD domain state
       (RUN/STOP) when the system enter to low power mode.

   (#) Call HAL_PWREx_ClearDomainFlags() function to clear the CPU flags for the
       selected power domain. This API is used only for dual core devices.

   (#) Call HAL_PWREx_HoldCore() and HAL_PWREx_ReleaseCore() functions to hold
       and release the selected CPU and and their domain peripherals when
       exiting STOP mode. These APIs are used only for dual core devices.

   (#) Call HAL_PWREx_EnableFlashPowerDown() and
       HAL_PWREx_DisableFlashPowerDown() functions to enable and disable the
       Flash Power Down in STOP mode.

   (#) Call HAL_PWREx_EnableMemoryShutOff() and
       HAL_PWREx_DisableMemoryShutOff() functions to enable and disable the
       memory block shut-off in DStop or DStop2. These APIs are used only for
       STM32H7Axxx and STM32H7Bxxx lines.

   (#) Call HAL_PWREx_EnableWakeUpPin() and HAL_PWREx_DisableWakeUpPin()
       functions to enable and disable the Wake-up pin functionality for
       the selected pin.

   (#) Call HAL_PWREx_GetWakeupFlag() and HAL_PWREx_ClearWakeupFlag()
       functions to manage wake-up flag for the selected pin.

   (#) Call HAL_PWREx_WAKEUP_PIN_IRQHandler() function to handle all wake-up
       pins interrupts.

   (#) Call HAL_PWREx_EnableBkUpReg() and HAL_PWREx_DisableBkUpReg() functions
       to enable and disable the backup domain regulator.

   (#) Call HAL_PWREx_EnableUSBReg(), HAL_PWREx_DisableUSBReg(),
       HAL_PWREx_EnableUSBVoltageDetector() and
       HAL_PWREx_DisableUSBVoltageDetector() functions to manage USB power
       regulation functionnalities.

   (#) Call HAL_PWREx_EnableBatteryCharging() and
       HAL_PWREx_DisableBatteryCharging() functions to enable and disable the
       battery charging feature with the selected resistor.

   (#) Call HAL_PWREx_EnableAnalogBooster() and
       HAL_PWREx_DisableAnalogBooster() functions to enable and disable the
       AVD boost feature when the VDD supply voltage is below 2V7.

   (#) Call HAL_PWREx_EnableMonitoring() and HAL_PWREx_DisableMonitoring()
       functions to enable and disable the VBAT and Temperature monitoring.
       When VBAT and Temperature monitoring feature is enables, use
       HAL_PWREx_GetTemperatureLevel() and HAL_PWREx_GetVBATLevel() to get
       respectively the Temperature level and VBAT level.

   (#) Call HAL_PWREx_GetMMCVoltage() and HAL_PWREx_DisableMonitoring()
       function to get VDDMMC voltage level. This API is used only for
       STM32H7AxxQ, STM32H7BxxQ, STM32H7Axxx and STM32H7Bxxx lines

   (#) Call HAL_PWREx_ConfigAVD() after setting parameter to be configured
       (event mode and voltage threshold) in order to set up the Analog Voltage
       Detector then use HAL_PWREx_EnableAVD() and  HAL_PWREx_DisableAVD()
       functions to start and stop the AVD detection.
       (+) AVD level could be one of the following values :
             (++) 1V7
             (++) 2V1
             (++) 2V5
             (++) 2V8

   (#) Call HAL_PWREx_PVD_AVD_IRQHandler() function to handle the PWR PVD and
       AVD interrupt request.

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup PWREx PWREx
  * @brief PWR Extended HAL module driver
  * @{
  */

#ifdef HAL_PWR_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @addtogroup PWREx_Private_Constants
  * @{
  */

/** @defgroup PWREx_AVD_Mode_Mask PWR Extended AVD Mode Mask
  * @{
  */
#define AVD_MODE_IT              (0x00010000U)
#define AVD_MODE_EVT             (0x00020000U)
#define AVD_RISING_EDGE          (0x00000001U)
#define AVD_FALLING_EDGE         (0x00000002U)
#define AVD_RISING_FALLING_EDGE  (0x00000003U)
/**
  * @}
  */

/** @defgroup PWREx_REG_SET_TIMEOUT PWR Extended Flag Setting Time Out Value
  * @{
  */
#define PWR_FLAG_SETTING_DELAY   (1000U)
/**
  * @}
  */

/** @defgroup PWREx_WakeUp_Pins_Offsets PWREx Wake-Up Pins masks and offsets
  * @{
  */
/* Wake-Up Pins EXTI register mask */
#define PWR_EXTI_WAKEUP_PINS_MASK  (EXTI_IMR2_IM55 | EXTI_IMR2_IM56 |\
                                    EXTI_IMR2_IM57 | EXTI_IMR2_IM58 |\
                                    EXTI_IMR2_IM59 | EXTI_IMR2_IM60)

/* Wake-Up Pins PWR Pin Pull shift offsets */
#define PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET (2U)
/**
  * @}
  */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup PWREx_Exported_Functions PWREx Exported Functions
  * @{
  */

/** @defgroup PWREx_Exported_Functions_Group1 Power Supply Control Functions
  * @brief    Power supply control functions
  *
@verbatim
 ===============================================================================
                  ##### Power supply control functions #####
 ===============================================================================
    [..]
   (#) When the system is powered on, the POR monitors VDD supply. Once VDD is
       above the POR threshold level, the voltage regulator is enabled in the
       default supply configuration:
      (+) The Voltage converter output level is set at 1V0 in accordance with
          the VOS3 level configured in PWR (D3/SRD) domain control register
          (PWR_D3CR/PWR_SRDCR).
      (+) The system is kept in reset mode as long as VCORE is not ok.
      (+) Once VCORE is ok, the system is taken out of reset and the HSI
          oscillator is enabled.
      (+) Once the oscillator is stable, the system is initialized: Flash memory
          and option bytes are loaded and the CPU starts in Run* mode.
      (+) The software shall then initialize the system including supply
          configuration programming using the HAL_PWREx_ConfigSupply().
      (+) Once the supply configuration has been configured, the
          HAL_PWREx_ConfigSupply() function checks the ACTVOSRDY bit in PWR
          control status register 1 (PWR_CSR1) to guarantee a valid voltage
          levels:
       (++) As long as ACTVOSRDY indicates that voltage levels are invalid, the
            system is in limited Run* mode, write accesses to the RAMs are not
            permitted and VOS shall not be changed.
       (++) Once ACTVOSRDY indicates that voltage levels are valid, the system
            is in normal Run mode, write accesses to RAMs are allowed and VOS
            can be changed.

@endverbatim
  * @{
  */

/**
  * @brief Configure the system Power Supply.
  * @param  SupplySource : Specifies the Power Supply source to set after a
  *                        system startup.
  *         This parameter can be one of the following values :
  *            @arg PWR_DIRECT_SMPS_SUPPLY : The SMPS supplies the Vcore Power
  *                                          Domains. The LDO is Bypassed.
  *            @arg PWR_SMPS_1V8_SUPPLIES_LDO : The SMPS 1.8V output supplies
  *                                             the LDO. The Vcore Power Domains
  *                                             are supplied from the LDO.
  *            @arg PWR_SMPS_2V5_SUPPLIES_LDO : The SMPS 2.5V output supplies
  *                                             the LDO. The Vcore Power Domains
  *                                             are supplied from the LDO.
  *            @arg PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO : The SMPS 1.8V output
  *                                                     supplies external
  *                                                     circuits and the LDO.
  *                                                     The Vcore Power Domains
  *                                                     are supplied from the
  *                                                     LDO.
  *            @arg PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO : The SMPS 2.5V output
  *                                                     supplies external
  *                                                     circuits and the LDO.
  *                                                     The Vcore Power Domains
  *                                                     are supplied from the
  *                                                     LDO.
  *            @arg PWR_SMPS_1V8_SUPPLIES_EXT : The SMPS 1.8V output supplies
  *                                             external circuits. The LDO is
  *                                             Bypassed. The Vcore Power
  *                                             Domains are supplied from
  *                                             external source.
  *            @arg PWR_SMPS_2V5_SUPPLIES_EXT : The SMPS 2.5V output supplies
  *                                             external circuits. The LDO is
  *                                             Bypassed. The Vcore Power
  *                                             Domains are supplied from
  *                                             external source.
  *            @arg PWR_LDO_SUPPLY : The LDO regulator supplies the Vcore Power
  *                                  Domains. The SMPS regulator is Bypassed.
  *            @arg PWR_EXTERNAL_SOURCE_SUPPLY : The SMPS and the LDO are
  *                                              Bypassed. The Vcore Power
  *                                              Domains are supplied from
  *                                              external source.
  * @note   The PWR_LDO_SUPPLY and PWR_EXTERNAL_SOURCE_SUPPLY are used by all
  *         H7 lines.
  *         The PWR_DIRECT_SMPS_SUPPLY, PWR_SMPS_1V8_SUPPLIES_LDO,
  *         PWR_SMPS_2V5_SUPPLIES_LDO, PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO,
  *         PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO, PWR_SMPS_1V8_SUPPLIES_EXT and
  *         PWR_SMPS_2V5_SUPPLIES_EXT are used only for lines that supports SMPS
  *         regulator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_ConfigSupply (uint32_t SupplySource)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param (IS_PWR_SUPPLY (SupplySource));

  /* Check if supply source was configured */
#if defined (PWR_FLAG_SCUEN)
  if (__HAL_PWR_GET_FLAG (PWR_FLAG_SCUEN) == 0U)
#else
  if ((PWR->CR3 & (PWR_CR3_SMPSEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)) != (PWR_CR3_SMPSEN | PWR_CR3_LDOEN))
#endif /* defined (PWR_FLAG_SCUEN) */
  {
    /* Check supply configuration */
    if ((PWR->CR3 & PWR_SUPPLY_CONFIG_MASK) != SupplySource)
    {
      /* Supply configuration update locked, can't apply a new supply config */
      return HAL_ERROR;
    }
    else
    {
      /* Supply configuration update locked, but new supply configuration
         matches with old supply configuration : nothing to do
      */
      return HAL_OK;
    }
  }

  /* Set the power supply configuration */
  MODIFY_REG (PWR->CR3, PWR_SUPPLY_CONFIG_MASK, SupplySource);

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till voltage level flag is set */
  while (__HAL_PWR_GET_FLAG (PWR_FLAG_ACTVOSRDY) == 0U)
  {
    if ((HAL_GetTick () - tickstart) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

#if defined (SMPS)
  /* When the SMPS supplies external circuits verify that SDEXTRDY flag is set */
  if ((SupplySource == PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO) ||
      (SupplySource == PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO) ||
      (SupplySource == PWR_SMPS_1V8_SUPPLIES_EXT)         ||
      (SupplySource == PWR_SMPS_2V5_SUPPLIES_EXT))
  {
    /* Get the current tick number */
    tickstart = HAL_GetTick ();

    /* Wait till SMPS external supply ready flag is set */
    while (__HAL_PWR_GET_FLAG (PWR_FLAG_SMPSEXTRDY) == 0U)
    {
      if ((HAL_GetTick () - tickstart) > PWR_FLAG_SETTING_DELAY)
      {
        return HAL_ERROR;
      }
    }
  }
#endif /* defined (SMPS) */

  return HAL_OK;
}

/**
  * @brief Get the power supply configuration.
  * @retval The supply configuration.
  */
uint32_t HAL_PWREx_GetSupplyConfig (void)
{
  return (PWR->CR3 & PWR_SUPPLY_CONFIG_MASK);
}

/**
  * @brief Configure the main internal regulator output voltage.
  * @note   For STM32H7x3, STM32H7x5, STM32H7x7, STM32H742 and STM32H750 lines,
  *         configuring Voltage Scale 0 is only possible when Vcore is supplied
  *         from LDO (Low DropOut). The SYSCFG Clock must be enabled through
  *         __HAL_RCC_SYSCFG_CLK_ENABLE() macro before configuring Voltage
  *         Scale 0.
  * @param  VoltageScaling : Specifies the regulator output voltage to achieve
  *                          a tradeoff between performance and power
  *                          consumption.
  *          This parameter can be one of the following values :
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE0 : Regulator voltage output
  *                                                Scale 0 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE1 : Regulator voltage output
  *                                                range 1 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE2 : Regulator voltage output
  *                                                range 2 mode.
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE3 : Regulator voltage output
  *                                                range 3 mode.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling (uint32_t VoltageScaling)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param (IS_PWR_REGULATOR_VOLTAGE (VoltageScaling));

  /* Get the voltage scaling  */
  if ((PWR->CSR1 & PWR_CSR1_ACTVOS) == VoltageScaling)
  {
    /* Old and new voltage scaling configuration match : nothing to do */
    return HAL_OK;
  }

#if defined (PWR_SRDCR_VOS)
  /* Set the voltage range */
  MODIFY_REG (PWR->SRDCR, PWR_SRDCR_VOS, VoltageScaling);
#else
  if (VoltageScaling == PWR_REGULATOR_VOLTAGE_SCALE0)
  {
    if ((PWR->CR3 & PWR_CR3_LDOEN) == PWR_CR3_LDOEN)
    {
      /* Set the voltage range */
      MODIFY_REG (PWR->D3CR, PWR_D3CR_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);

      /* Get tick */
      tickstart = HAL_GetTick ();

      /* Wait till voltage level flag is set */
      while (__HAL_PWR_GET_FLAG (PWR_FLAG_ACTVOSRDY) == 0U)
      {
        if ((HAL_GetTick () - tickstart) > PWR_FLAG_SETTING_DELAY)
        {
          return HAL_ERROR;
        }
      }

      /* Enable the PWR overdrive */
      SET_BIT (SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);
    }
    else
    {
      /* The voltage scale 0 is only possible when LDO regulator is enabled */
      return HAL_ERROR;
    }
  }
  else
  {
    if ((PWR->CSR1 & PWR_CSR1_ACTVOS) == PWR_REGULATOR_VOLTAGE_SCALE1)
    {
      if ((SYSCFG->PWRCR & SYSCFG_PWRCR_ODEN) != 0U)
      {
        /* Disable the PWR overdrive */
        CLEAR_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);

        /* Get tick */
        tickstart = HAL_GetTick ();

        /* Wait till voltage level flag is set */
        while (__HAL_PWR_GET_FLAG (PWR_FLAG_ACTVOSRDY) == 0U)
        {
          if ((HAL_GetTick () - tickstart) > PWR_FLAG_SETTING_DELAY)
          {
            return HAL_ERROR;
          }
        }
      }
    }

    /* Set the voltage range */
    MODIFY_REG (PWR->D3CR, PWR_D3CR_VOS, VoltageScaling);
  }
#endif /* defined (PWR_SRDCR_VOS) */

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till voltage level flag is set */
  while (__HAL_PWR_GET_FLAG (PWR_FLAG_ACTVOSRDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Get the main internal regulator output voltage. Reflecting the last
  *        VOS value applied to the PMU.
  * @retval The current applied VOS selection.
  */
uint32_t HAL_PWREx_GetVoltageRange (void)
{
  /* Get the active voltage scaling */
  return (PWR->CSR1 & PWR_CSR1_ACTVOS);
}

/**
  * @brief Configure the main internal regulator output voltage in STOP mode.
  * @param  VoltageScaling : Specifies the regulator output voltage when the
  *         system enters Stop mode to achieve a tradeoff between performance
  *         and power consumption.
  *          This parameter can be one of the following values:
  *            @arg PWR_REGULATOR_SVOS_SCALE3 : Regulator voltage output range
  *                                             3 mode.
  *            @arg PWR_REGULATOR_SVOS_SCALE4 : Regulator voltage output range
  *                                             4 mode.
  *            @arg PWR_REGULATOR_SVOS_SCALE5 : Regulator voltage output range
  *                                             5 mode.
  * @note   The Stop mode voltage scaling for SVOS4 and SVOS5 sets the voltage
  *         regulator in Low-power (LP) mode to further reduce power consumption.
  *         When preselecting SVOS3, the use of the voltage regulator low-power
  *         mode (LP) can be selected by LPDS register bit.
  * @note   The selected SVOS4 and SVOS5 levels add an additional startup delay
  *         when exiting from system Stop mode.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_PWREx_ControlStopModeVoltageScaling (uint32_t VoltageScaling)
{
  /* Check the parameters */
  assert_param (IS_PWR_STOP_MODE_REGULATOR_VOLTAGE (VoltageScaling));

  /* Return the stop mode voltage range */
  MODIFY_REG (PWR->CR1, PWR_CR1_SVOS, VoltageScaling);

  return HAL_OK;
}

/**
  * @brief Get the main internal regulator output voltage in STOP mode.
  * @retval The actual applied VOS selection.
  */
uint32_t HAL_PWREx_GetStopModeVoltageRange (void)
{
  /* Return the stop voltage scaling */
  return (PWR->CR1 & PWR_CR1_SVOS);
}
/**
  * @}
  */

/** @defgroup PWREx_Exported_Functions_Group2 Low Power Control Functions
  * @brief    Low power control functions
  *
@verbatim
 ===============================================================================
                     ##### Low power control functions #####
 ===============================================================================

    *** Domains Low Power modes configuration ***
    =============================================
    [..]
      This section provides the extended low power mode control APIs.
      The system presents 3 principles domains (D1, D2 and D3) that can be
      operated in low-power modes (DSTOP or DSTANDBY mode):

      (+) DSTOP mode to enters a domain to STOP mode:
       (++) D1 domain and/or D2 domain enters DSTOP mode only when the CPU
            subsystem is in CSTOP mode and has allocated peripheral in the
            domain.
            In DSTOP mode the domain bus matrix clock is stopped.
       (++) The system enters STOP mode using one of the following scenarios:
        (+++) D1 domain enters DSTANDBY mode (powered off) and D2, D3 domains
              enter DSTOP mode.
        (+++) D2 domain enters DSTANDBY mode (powered off) and D1, D3 domains
              enter DSTOP mode.
        (+++) D3 domain enters DSTANDBY mode (powered off) and D1, D2 domains
              enter DSTOP mode.
        (+++) D1 and D2 domains enter DSTANDBY mode (powered off) and D3 domain
              enters DSTOP mode.
        (+++) D1 and D3 domains enter DSTANDBY mode (powered off) and D2 domain
              enters DSTOP mode.
        (+++) D2 and D3 domains enter DSTANDBY mode (powered off) and D1 domain
              enters DSTOP mode.
        (+++) D1, D2 and D3 domains enter DSTOP mode.
       (++) When the system enters STOP mode, the clocks are stopped and the
            regulator is running in main or low power mode.
       (++) D3 domain can be kept in Run mode regardless of the CPU status when
            enter STOP mode by using HAL_PWREx_ConfigD3Domain(D3State) function.

      (+) DSTANDBY mode to enters a domain to STANDBY mode:
       (++) The DSTANDBY mode is entered when the PDDS_Dn bit in PWR CPU control
            register (PWR_CPUCR) for the Dn domain selects Standby mode.
       (++) The system enters STANDBY mode only when D1, D2 and D3 domains enter
            DSTANDBY mode. Consequently the VCORE supply regulator is powered
            off.

   *** DSTOP mode ***
   ==================
    [..]
      In DStop mode the domain bus matrix clock is stopped.
      The Flash memory can enter low-power Stop mode when it is enabled through
      FLPS in PWR_CR1 register. This allows a trade-off between domain DStop
      restart time and low power consumption.
    [..]
      In DStop mode domain peripherals using the LSI or LSE clock and
      peripherals having a kernel clock request are still able to operate.
    [..]
      Before entering DSTOP mode it is recommended to call SCB_CleanDCache
      function in order to clean the D-Cache and guarantee the data integrity
      for the SRAM memories.

      (+) Entry:
         The DSTOP mode is entered using the HAL_PWREx_EnterSTOPMode(Regulator,
         STOPEntry, Domain) function with:
         (++) Regulator:
          (+++) PWR_MAINREGULATOR_ON     : Main regulator ON.
          (+++) PWR_LOWPOWERREGULATOR_ON : Low Power regulator ON.
         (++) STOPEntry:
          (+++) PWR_STOPENTRY_WFI : enter STOP mode with WFI instruction
          (+++) PWR_STOPENTRY_WFE : enter STOP mode with WFE instruction
         (++) Domain:
          (+++) PWR_D1_DOMAIN : Enters D1 domain to DSTOP mode.
          (+++) PWR_D2_DOMAIN : Enters D2 domain to DSTOP mode.
          (+++) PWR_D3_DOMAIN : Enters D3 domain to DSTOP mode.

      (+) Exit:
        Any EXTI Line (Internal or External) configured in Interrupt/Event mode.

   *** DSTANDBY mode ***
   =====================
    [..]
      In DStandby mode:
        (+) The domain bus matrix clock is stopped.
        (+) The domain is powered down and the domain RAM and register contents
            are lost.
    [..]
      Before entering DSTANDBY mode it is recommended to call SCB_CleanDCache
      function in order to clean the D-Cache and guarantee the data integrity
      for the SRAM memories.

      (+) Entry:
         The DSTANDBY mode is entered using the HAL_PWREx_EnterSTANDBYMode
         (Domain) function with:
       (++) Domain:
        (+++) PWR_D1_DOMAIN : Enters D1 domain to DSTANDBY mode.
        (+++) PWR_D2_DOMAIN : Enters D2 domain to DSTANDBY mode.
        (+++) PWR_D3_DOMAIN : Enters D3 domain to DSTANDBY mode.

      (+) Exit:
        WKUP pin rising or falling edge, RTC alarm (Alarm A and Alarm B), RTC
        wakeup, tamper event, time stamp event, external reset in NRST pin,
        IWDG reset.

   *** Keep D3/SRD in RUN mode ***
   ===============================
    [..]
      D3/SRD domain can be kept in Run mode regardless of the CPU status when
      entering STOP mode by using HAL_PWREx_ConfigD3Domain(D3State) function
      with :
       (+) D3State:
        (++) PWR_D3_DOMAIN_STOP : D3/SDR domain follows the CPU sub-system
                                  mode.
        (++) PWR_D3_DOMAIN_RUN : D3/SRD domain remains in Run mode regardless
                                 of CPU subsystem mode.

    *** FLASH Power Down configuration ****
    =======================================
    [..]
      By setting the FLPS bit in the PWR_CR1 register using the
      HAL_PWREx_EnableFlashPowerDown() function, the Flash memory also enters
      power down mode when the device enters STOP mode. When the Flash memory is
      in power down mode, an additional startup delay is incurred when waking up
      from STOP mode.

    *** Wakeup Pins configuration ****
    ===================================
    [..]
      Wakeup pins allow the system to exit from Standby mode. The configuration
      of wakeup pins is done with the HAL_PWREx_EnableWakeUpPin(sPinParams)
      function with:
       (+) sPinParams: structure to enable and configure a wakeup pin:
        (++) WakeUpPin: Wakeup pin to be enabled.
        (++) PinPolarity: Wakeup pin polarity (rising or falling edge).
        (++) PinPull: Wakeup pin pull (no pull, pull-up or pull-down).
    [..]
      The wakeup pins are internally connected to the EXTI lines [55-60] to
      generate an interrupt if enabled. The EXTI lines configuration is done by
      the HAL_EXTI_Dx_EventInputConfig() functions defined in the stm32h7xxhal.c
      file.
    [..]
      When a wakeup pin event is received the HAL_PWREx_WAKEUP_PIN_IRQHandler is
      called and the appropriate flag is set in the PWR_WKUPFR register. Then in
      the HAL_PWREx_WAKEUP_PIN_IRQHandler function the wakeup pin flag will be
      cleared and the appropriate user callback will be called. The user can add
      his own code by customization of function pointer HAL_PWREx_WKUPx_Callback.

@endverbatim
  * @{
  */

#if defined (PWR_CPUCR_RETDS_CD)
/**
  * @brief Enter the system to STOP mode with main domain in DSTOP2.
  * @note   In STOP mode, the domain bus matrix clock is stalled.
  * @note   In STOP mode, memories and registers are maintained and peripherals
  *         in CPU domain are no longer operational.
  * @note   All clocks in the VCORE domain are stopped, the PLL, the HSI and the
  *         HSE oscillators are disabled. Only Peripherals that have wakeup
  *         capability can switch on the HSI to receive a frame, and switch off
  *         the HSI after receiving the frame if it is not a wakeup frame. In
  *         this case the HSI clock is propagated only to the peripheral
  *         requesting it.
  * @note   When exiting STOP mode by issuing an interrupt or a wakeup event,
  *         the HSI RC oscillator is selected as system clock if STOPWUCK bit in
  *         RCC_CFGR register is set.
  * @param  Regulator : Specifies the regulator state in STOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MAINREGULATOR_ON     : STOP mode with regulator ON.
  *            @arg PWR_LOWPOWERREGULATOR_ON : STOP mode with low power
  *                                            regulator ON.
  * @param  STOPEntry : Specifies if STOP mode in entered with WFI or WFE
  *                     intrinsic instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPENTRY_WFI : Enter STOP mode with WFI instruction.
  *            @arg PWR_STOPENTRY_WFE : Enter STOP mode with WFE instruction.
  * @retval None.
  */
void HAL_PWREx_EnterSTOP2Mode (uint32_t Regulator, uint8_t STOPEntry)
{
  /* Check the parameters */
  assert_param (IS_PWR_REGULATOR (Regulator));
  assert_param (IS_PWR_STOP_ENTRY (STOPEntry));

  /* Select the regulator state in Stop mode */
  MODIFY_REG (PWR->CR1, PWR_CR1_LPDS, Regulator);

  /* Go to DStop2 mode (deep retention) when CPU domain enters Deepsleep */
  SET_BIT (PWR->CPUCR, PWR_CPUCR_RETDS_CD);

  /* Keep DSTOP mode when SmartRun domain enters Deepsleep */
  CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_SRD);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Ensure that all instructions are done before entering STOP mode */
  __ISB ();
  __DSB ();

  /* Select Stop mode entry */
  if (STOPEntry == PWR_STOPENTRY_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI ();
  }
  else
  {
    /* Request Wait For Event */
    __WFE ();
  }

  /* Clear SLEEPDEEP bit of Cortex-Mx in the System Control Register */
  CLEAR_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
}
#endif /* defined (PWR_CPUCR_RETDS_CD) */

/**
  * @brief Enter a Domain to DSTOP mode.
  * @note   This API gives flexibility to manage independently each domain STOP
  *         mode. For dual core lines, this API should be executed with the
  *         corresponding Cortex-Mx to enter domain to DSTOP mode. When it is
  *         executed by all available Cortex-Mx, the system enter to STOP mode.
  *         For single core lines, calling this API with domain parameter set to
  *         PWR_D1_DOMAIN (D1/CD), the whole system will enter in STOP mode
  *         independently of PWR_CPUCR_PDDS_Dx bits values if RUN_D3 bit in the
  *         CPUCR_RUN_D3 is cleared.
  * @note   In DStop mode the domain bus matrix clock is stopped.
  * @note   The system D3/SRD domain enter Stop mode only when the CPU subsystem
  *         is in CStop mode, the EXTI wakeup sources are inactive and at least
  *         one PDDS_Dn bit in PWR CPU control register (PWR_CPUCR) for
  *         any domain request Stop.
  * @note   Before entering DSTOP mode it is recommended to call SCB_CleanDCache
  *         function in order to clean the D-Cache and guarantee the data
  *         integrity for the SRAM memories.
  * @note   In System Stop mode, the domain peripherals that use the LSI or LSE
  *         clock, and the peripherals that have a kernel clock request to
  *         select HSI or CSI as source, are still able to operate.
  * @param  Regulator : Specifies the regulator state in STOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MAINREGULATOR_ON     : STOP mode with regulator ON.
  *            @arg PWR_LOWPOWERREGULATOR_ON : STOP mode with low power
  *                                            regulator ON.
  * @param  STOPEntry : Specifies if STOP mode in entered with WFI or WFE
  *                     intrinsic instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPENTRY_WFI : Enter STOP mode with WFI instruction.
  *            @arg PWR_STOPENTRY_WFE : Enter STOP mode with WFE instruction.
  * @param  Domain : Specifies the Domain to enter in DSTOP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_D1_DOMAIN : Enter D1/CD Domain to DSTOP mode.
  *            @arg PWR_D2_DOMAIN : Enter D2 Domain to DSTOP mode.
  *            @arg PWR_D3_DOMAIN : Enter D3/SRD Domain to DSTOP mode.
  * @retval None.
  */
void HAL_PWREx_EnterSTOPMode (uint32_t Regulator, uint8_t STOPEntry, uint32_t Domain)
{
  /* Check the parameters */
  assert_param (IS_PWR_REGULATOR (Regulator));
  assert_param (IS_PWR_STOP_ENTRY (STOPEntry));
  assert_param (IS_PWR_DOMAIN (Domain));

  /* Select the regulator state in Stop mode */
  MODIFY_REG (PWR->CR1, PWR_CR1_LPDS, Regulator);

  /* Select the domain Power Down DeepSleep */
  if (Domain == PWR_D1_DOMAIN)
  {
#if defined (DUAL_CORE)
    /* Check current core */
    if (HAL_GetCurrentCPUID () != CM7_CPUID)
    {
      /*
         When the domain selected and the cortex-mx don't match, entering stop
         mode will not be performed
      */
      return;
    }
#endif /* defined (DUAL_CORE) */

    /* Keep DSTOP mode when D1/CD domain enters Deepsleep */
    CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_D1);

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

    /* Ensure that all instructions are done before entering STOP mode */
    __DSB ();
    __ISB ();

    /* Select Stop mode entry */
    if (STOPEntry == PWR_STOPENTRY_WFI)
    {
      /* Request Wait For Interrupt */
      __WFI ();
    }
    else
    {
      /* Request Wait For Event */
      __WFE ();
    }

    /* Clear SLEEPDEEP bit of Cortex-Mx in the System Control Register */
    CLEAR_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
  }
#if defined (PWR_CPUCR_PDDS_D2)
  else if (Domain == PWR_D2_DOMAIN)
  {
    /* Keep DSTOP mode when D2 domain enters Deepsleep */
    CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_D2);

#if defined (DUAL_CORE)
    /* Check current core */
    if (HAL_GetCurrentCPUID () != CM4_CPUID)
    {
      /*
         When the domain selected and the cortex-mx don't match, entering stop
         mode will not be performed
      */
      return;
    }

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

    /* Ensure that all instructions are done before entering STOP mode */
    __DSB ();
    __ISB ();

    /* Select Stop mode entry */
    if (STOPEntry == PWR_STOPENTRY_WFI)
    {
      /* Request Wait For Interrupt */
      __WFI ();
    }
    else
    {
      /* Request Wait For Event */
      __WFE ();
    }

    /* Clear SLEEPDEEP bit of Cortex-Mx in the System Control Register */
    CLEAR_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
#endif /* defined (DUAL_CORE) */
  }
#endif /* defined (PWR_CPUCR_PDDS_D2) */
  else
  {
    /* Keep DSTOP mode when D3/SRD domain enters Deepsleep */
    CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_D3);
  }
}

/**
  * @brief Clear pending event.
  * @note   This API clears the pending event in order to enter a given CPU
  *         to CSLEEP or CSTOP. It should be called just before APIs performing
  *         enter low power mode using Wait For Event request.
  * @retval None.
  */
void HAL_PWREx_ClearPendingEvent (void)
{
#if defined (DUAL_CORE)
  /* Check the current Core */
  if (HAL_GetCurrentCPUID () == CM7_CPUID)
  {
    __WFE ();
  }
  else
  {
    __SEV ();
    __WFE ();
  }
#else
  __WFE ();
#endif /* defined (DUAL_CORE) */
}

/**
  * @brief Enter a Domain to DSTANDBY mode.
  * @note   This API gives flexibility to manage independently each domain
  *         STANDBY mode. For dual core lines, this API should be executed with
  *         the corresponding Cortex-Mx to enter domain to DSTANDBY mode. When
  *         it is executed by all available Cortex-Mx, the system enter STANDBY
  *         mode.
  *         For single core lines, calling this API with D1/SRD the selected
  *         domain will enter the whole system in STOP if PWR_CPUCR_PDDS_D3 = 0
  *         and enter the whole system in STANDBY if PWR_CPUCR_PDDS_D3 = 1.
  * @note   The DStandby mode is entered when all PDDS_Dn bits in PWR_CPUCR for
  *         the Dn domain select Standby mode. When the system enters Standby
  *         mode, the voltage regulator is disabled.
  * @note   When D2 or D3 domain is in DStandby mode and the CPU sets the
  *         domain PDDS_Dn bit to select Stop mode, the domain remains in
  *         DStandby mode. The domain will only exit DStandby when the CPU
  *         allocates a peripheral in the domain.
  * @note   The system D3/SRD domain enters Standby mode only when the D1 and D2
  *         domain are in DStandby.
  * @note   Before entering DSTANDBY mode it is recommended to call
  *         SCB_CleanDCache function in order to clean the D-Cache and guarantee
  *         the data integrity for the SRAM memories.
  * @param  Domain : Specifies the Domain to enter to STANDBY mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_D1_DOMAIN: Enter D1/CD Domain to DSTANDBY mode.
  *            @arg PWR_D2_DOMAIN: Enter D2 Domain to DSTANDBY mode.
  *            @arg PWR_D3_DOMAIN: Enter D3/SRD Domain to DSTANDBY mode.
  * @retval None
  */
void HAL_PWREx_EnterSTANDBYMode (uint32_t Domain)
{
  /* Check the parameters */
  assert_param (IS_PWR_DOMAIN (Domain));

  /* Select the domain Power Down DeepSleep */
  if (Domain == PWR_D1_DOMAIN)
  {
#if defined (DUAL_CORE)
    /* Check current core */
    if (HAL_GetCurrentCPUID () != CM7_CPUID)
    {
      /*
         When the domain selected and the cortex-mx don't match, entering
         standby mode will not be performed
      */
      return;
    }
#endif /* defined (DUAL_CORE) */

    /* Allow DSTANDBY mode when D1/CD domain enters Deepsleep */
    SET_BIT (PWR-> CPUCR, PWR_CPUCR_PDDS_D1);

#if defined (DUAL_CORE)
    /* Allow DSTANDBY mode when D1/CD domain enters Deepsleep */
    SET_BIT (PWR-> CPU2CR, PWR_CPU2CR_PDDS_D1);
#endif /*DUAL_CORE*/

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

    /* This option is used to ensure that store operations are completed */
#if defined (__CC_ARM)
    __force_stores ();
#endif /* defined (__CC_ARM) */

    /* Request Wait For Interrupt */
    __WFI ();
  }
#if defined (PWR_CPUCR_PDDS_D2)
  else if (Domain == PWR_D2_DOMAIN)
  {
    /* Allow DSTANDBY mode when D2 domain enters Deepsleep */
    SET_BIT (PWR-> CPUCR, PWR_CPUCR_PDDS_D2);

#if defined (DUAL_CORE)
    /* Check current core */
    if (HAL_GetCurrentCPUID () != CM4_CPUID)
    {
      /*
         When the domain selected and the cortex-mx don't match, entering
         standby mode will not be performed
      */
      return;
    }

    /* Allow DSTANDBY mode when D2 domain enters Deepsleep */
    SET_BIT (PWR-> CPU2CR, PWR_CPU2CR_PDDS_D2);

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

    /* This option is used to ensure that store operations are completed */
#if defined (__CC_ARM)
    __force_stores ();
#endif /* defined (__CC_ARM) */

    /* Request Wait For Interrupt */
    __WFI ();
#endif /* defined (DUAL_CORE) */
  }
#endif /* defined (PWR_CPUCR_PDDS_D2) */
  else
  {
    /* Allow DSTANDBY mode when D3/SRD domain enters Deepsleep */
    SET_BIT (PWR-> CPUCR, PWR_CPUCR_PDDS_D3);

#if defined (DUAL_CORE)
    /* Allow DSTANDBY mode when D3/SRD domain enters Deepsleep */
    SET_BIT (PWR-> CPU2CR, PWR_CPU2CR_PDDS_D3);
#endif /* defined (DUAL_CORE) */
  }
}

/**
  * @brief Configure the D3/SRD Domain state when the System in low power mode.
  * @param  D3State : Specifies the D3/SRD state.
  *          This parameter can be one of the following values :
  *            @arg PWR_D3_DOMAIN_STOP : D3/SRD domain will follow the most deep
  *                                      CPU sub-system low power mode.
  *            @arg PWR_D3_DOMAIN_RUN : D3/SRD domain will stay in RUN mode
  *                                     regardless of the CPU sub-system low
  *                                     power mode.
  * @retval None
  */
void HAL_PWREx_ConfigD3Domain (uint32_t D3State)
{
  /* Check the parameter */
  assert_param (IS_D3_STATE (D3State));

  /* Keep D3/SRD in run mode */
  MODIFY_REG (PWR->CPUCR, PWR_CPUCR_RUN_D3, D3State);
}

#if defined (DUAL_CORE)
/**
  * @brief Clear HOLD2F, HOLD1F, STOPF, SBF, SBF_D1, and SBF_D2 flags for a
  *        given domain.
  * @param  DomainFlags : Specifies the Domain flags to be cleared.
  *          This parameter can be one of the following values:
  *            @arg PWR_D1_DOMAIN_FLAGS : Clear D1 Domain flags.
  *            @arg PWR_D2_DOMAIN_FLAGS : Clear D2 Domain flags.
  *            @arg PWR_ALL_DOMAIN_FLAGS : Clear D1 and D2 Domain flags.
  * @retval None.
  */
void HAL_PWREx_ClearDomainFlags (uint32_t DomainFlags)
{
  /* Check the parameter */
  assert_param (IS_PWR_DOMAIN_FLAG (DomainFlags));

  /* D1 CPU flags */
  if (DomainFlags == PWR_D1_DOMAIN_FLAGS)
  {
    /* Clear D1 domain flags (HOLD2F, STOPF, SBF, SBF_D1, and SBF_D2) */
    SET_BIT (PWR->CPUCR, PWR_CPUCR_CSSF);
  }
  /* D2 CPU flags */
  else if (DomainFlags == PWR_D2_DOMAIN_FLAGS)
  {
    /* Clear D2 domain flags (HOLD1F, STOPF, SBF, SBF_D1, and SBF_D2) */
    SET_BIT (PWR->CPU2CR, PWR_CPU2CR_CSSF);
  }
  else
  {
    /* Clear D1 domain flags (HOLD2F, STOPF, SBF, SBF_D1, and SBF_D2) */
    SET_BIT (PWR->CPUCR, PWR_CPUCR_CSSF);
    /* Clear D2 domain flags (HOLD1F, STOPF, SBF, SBF_D1, and SBF_D2) */
    SET_BIT (PWR->CPU2CR, PWR_CPU2CR_CSSF);
  }
}

/**
  * @brief Hold the CPU and their domain peripherals when exiting STOP mode.
  * @param  CPU : Specifies the core to be held.
  *         This parameter can be one of the following values:
  *             @arg PWR_CORE_CPU1: Hold CPU1 and set CPU2 as master.
  *             @arg PWR_CORE_CPU2: Hold CPU2 and set CPU1 as master.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PWREx_HoldCore (uint32_t CPU)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param (IS_PWR_CORE (CPU));

  /* Check CPU index */
  if (CPU == PWR_CORE_CPU2)
  {
    /* If CPU1 is not held */
    if ((PWR->CPU2CR & PWR_CPU2CR_HOLD1) != PWR_CPU2CR_HOLD1)
    {
      /* Set HOLD2 bit */
      SET_BIT (PWR->CPUCR, PWR_CPUCR_HOLD2);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    /* If CPU2 is not held */
    if ((PWR->CPUCR & PWR_CPUCR_HOLD2) != PWR_CPUCR_HOLD2)
    {
      /* Set HOLD1 bit */
      SET_BIT (PWR->CPU2CR, PWR_CPU2CR_HOLD1);
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/**
  * @brief Release the CPU and their domain peripherals after a wake-up from
  *        STOP mode.
  * @param  CPU: Specifies the core to be released.
  *         This parameter can be one of the following values:
  *             @arg  PWR_CORE_CPU1: Release the CPU1 and their domain
  *                   peripherals from holding.
  *             @arg  PWR_CORE_CPU2: Release the CPU2 and their domain
  *                   peripherals from holding.
  * @retval None
  */
void HAL_PWREx_ReleaseCore (uint32_t CPU)
{
  /* Check the parameters */
  assert_param (IS_PWR_CORE (CPU));

  /* Check CPU index */
  if (CPU == PWR_CORE_CPU2)
  {
    /* Reset HOLD2 bit */
    CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_HOLD2);
  }
  else
  {
    /* Reset HOLD1 bit */
    CLEAR_BIT (PWR->CPU2CR, PWR_CPU2CR_HOLD1);
  }
}
#endif /* defined (DUAL_CORE) */


/**
  * @brief Enable the Flash Power Down in Stop mode.
  * @note   When Flash Power Down is enabled  the Flash memory enters low-power
  *         mode when D1/SRD domain is in DStop mode. This feature allows to
  *         obtain the best trade-off between low-power consumption and restart
  *         time when exiting from DStop mode.
  * @retval None.
  */
void HAL_PWREx_EnableFlashPowerDown (void)
{
  /* Enable the Flash Power Down */
  SET_BIT (PWR->CR1, PWR_CR1_FLPS);
}

/**
  * @brief Disable the Flash Power Down in Stop mode.
  * @note   When Flash Power Down is disabled  the Flash memory is kept on
  *         normal mode when D1/SRD domain is in DStop mode. This feature allows
  *         to obtain the best trade-off between low-power consumption and
  *         restart time when exiting from DStop mode.
  * @retval None.
  */
void HAL_PWREx_DisableFlashPowerDown (void)
{
  /* Disable the Flash Power Down */
  CLEAR_BIT (PWR->CR1, PWR_CR1_FLPS);
}

#if defined (PWR_CR1_SRDRAMSO)
/**
  * @brief Enable memory block shut-off in DStop or DStop2 modes
  * @note   In DStop or DStop2 mode, the content of the memory blocks is
  *         maintained. Further power optimization can be obtained by switching
  *         off some memory blocks. This optimization implies loss of the memory
  *         content. The user can select which memory is discarded during STOP
  *         mode by means of xxSO bits.
  * @param  MemoryBlock : Specifies the memory block to shut-off during DStop or
  *                       DStop2 mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_SRD_AHB_MEMORY_BLOCK : SmartRun domain AHB memory.
  *            @arg PWR_USB_FDCAN_MEMORY_BLOCK : High-speed interfaces USB and
  *                                              FDCAN memories.
  *            @arg PWR_GFXMMU_JPEG_MEMORY_BLOCK : GFXMMU and JPEG memories.
  *            @arg PWR_TCM_ECM_MEMORY_BLOCK : Instruction TCM and ETM memories.
  *            @arg PWR_RAM1_AHB_MEMORY_BLOCK : AHB RAM1 memory.
  *            @arg PWR_RAM2_AHB_MEMORY_BLOCK : AHB RAM2 memory.
  *            @arg PWR_RAM1_AXI_MEMORY_BLOCK : AXI RAM1 memory.
  *            @arg PWR_RAM2_AXI_MEMORY_BLOCK : AXI RAM2 memory.
  *            @arg PWR_RAM3_AXI_MEMORY_BLOCK : AXI RAM3 memory.
  * @retval None.
  */
void HAL_PWREx_EnableMemoryShutOff (uint32_t MemoryBlock)
{
  /* Check the parameter */
  assert_param (IS_PWR_MEMORY_BLOCK (MemoryBlock));

  /* Enable memory block shut-off */
  SET_BIT (PWR->CR1, MemoryBlock);
}

/**
  * @brief Disable memory block shut-off in DStop or DStop2 modes
  * @param  MemoryBlock : Specifies the memory block to keep content during
  *                       DStop or DStop2 mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_SRD_AHB_MEMORY_BLOCK : SmartRun domain AHB memory.
  *            @arg PWR_USB_FDCAN_MEMORY_BLOCK : High-speed interfaces USB and
  *                                              FDCAN memories.
  *            @arg PWR_GFXMMU_JPEG_MEMORY_BLOCK : GFXMMU and JPEG memories.
  *            @arg PWR_TCM_ECM_MEMORY_BLOCK : Instruction TCM and ETM memories.
  *            @arg PWR_RAM1_AHB_MEMORY_BLOCK : AHB RAM1 memory.
  *            @arg PWR_RAM2_AHB_MEMORY_BLOCK : AHB RAM2 memory.
  *            @arg PWR_RAM1_AXI_MEMORY_BLOCK : AXI RAM1 memory.
  *            @arg PWR_RAM2_AXI_MEMORY_BLOCK : AXI RAM2 memory.
  *            @arg PWR_RAM3_AXI_MEMORY_BLOCK : AXI RAM3 memory.
  * @retval None.
  */
void HAL_PWREx_DisableMemoryShutOff (uint32_t MemoryBlock)
{
  /* Check the parameter */
  assert_param (IS_PWR_MEMORY_BLOCK (MemoryBlock));

  /* Disable memory block shut-off */
  CLEAR_BIT (PWR->CR1, MemoryBlock);
}
#endif /* defined (PWR_CR1_SRDRAMSO) */

/**
  * @brief Enable the Wake-up PINx functionality.
  * @param  sPinParams : Pointer to a PWREx_WakeupPinTypeDef structure that
  *                      contains the configuration information for the wake-up
  *                      Pin.
  * @note   For dual core devices, please ensure to configure the EXTI lines for
  *         the different Cortex-Mx. All combination are allowed: wake up only
  *         Cortex-M7, wake up only Cortex-M4 and wake up Cortex-M7 and
  *         Cortex-M4.
  * @retval None.
  */
void HAL_PWREx_EnableWakeUpPin (PWREx_WakeupPinTypeDef *sPinParams)
{
  uint32_t pinConfig;
  uint32_t regMask;
  const uint32_t pullMask = PWR_WKUPEPR_WKUPPUPD1;

  /* Check the parameters */
  assert_param (IS_PWR_WAKEUP_PIN (sPinParams->WakeUpPin));
  assert_param (IS_PWR_WAKEUP_PIN_POLARITY (sPinParams->PinPolarity));
  assert_param (IS_PWR_WAKEUP_PIN_PULL (sPinParams->PinPull));

  pinConfig = sPinParams->WakeUpPin | \
              (sPinParams->PinPolarity << ((POSITION_VAL(sPinParams->WakeUpPin) + PWR_WKUPEPR_WKUPP1_Pos) & 0x1FU)) | \
              (sPinParams->PinPull << (((POSITION_VAL(sPinParams->WakeUpPin) * PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET) + PWR_WKUPEPR_WKUPPUPD1_Pos) & 0x1FU));

  regMask   = sPinParams->WakeUpPin | \
              (PWR_WKUPEPR_WKUPP1 << (POSITION_VAL(sPinParams->WakeUpPin) & 0x1FU)) | \
              (pullMask << ((POSITION_VAL(sPinParams->WakeUpPin) * PWR_WAKEUP_PINS_PULL_SHIFT_OFFSET) & 0x1FU));

  /* Enable and Specify the Wake-Up pin polarity and the pull configuration
     for the event detection (rising or falling edge) */
  MODIFY_REG (PWR->WKUPEPR, regMask, pinConfig);
#ifndef DUAL_CORE
  /* Configure the Wakeup Pin EXTI Line */
  MODIFY_REG (EXTI->IMR2, PWR_EXTI_WAKEUP_PINS_MASK, (sPinParams->WakeUpPin << EXTI_IMR2_IM55_Pos));
#endif /* !DUAL_CORE */
}

/**
  * @brief Disable the Wake-up PINx functionality.
  * @param  WakeUpPin : Specifies the Wake-Up pin to be disabled.
  *          This parameter can be one of the following values:
  *           @arg PWR_WAKEUP_PIN1 : Disable PA0  wake-up PIN.
  *           @arg PWR_WAKEUP_PIN2 : Disable PA2  wake-up PIN.
  *           @arg PWR_WAKEUP_PIN3 : Disable PI8  wake-up PIN.
  *           @arg PWR_WAKEUP_PIN4 : Disable PC13 wake-up PIN.
  *           @arg PWR_WAKEUP_PIN5 : Disable PI11 wake-up PIN.
  *           @arg PWR_WAKEUP_PIN6 : Disable PC1  wake-up PIN.
  * @retval None
  */
void HAL_PWREx_DisableWakeUpPin (uint32_t WakeUpPin)
{
  /* Check the parameter */
  assert_param (IS_PWR_WAKEUP_PIN (WakeUpPin));

  /* Disable the WakeUpPin */
  CLEAR_BIT (PWR->WKUPEPR, WakeUpPin);
}

/**
  * @brief Get the Wake-Up Pin pending flags.
  * @param  WakeUpFlag : Specifies the Wake-Up PIN flag to be checked.
  *          This parameter can be one of the following values:
  *            @arg PWR_WAKEUP_FLAG1    : Get wakeup event received from PA0.
  *            @arg PWR_WAKEUP_FLAG2    : Get wakeup event received from PA2.
  *            @arg PWR_WAKEUP_FLAG3    : Get wakeup event received from PI8.
  *            @arg PWR_WAKEUP_FLAG4    : Get wakeup event received from PC13.
  *            @arg PWR_WAKEUP_FLAG5    : Get wakeup event received from PI11.
  *            @arg PWR_WAKEUP_FLAG6    : Get wakeup event received from PC1.
  *            @arg PWR_WAKEUP_FLAG_ALL : Get Wakeup event received from all
  *                                      wake up pins.
  * @retval The Wake-Up pin flag.
  */
uint32_t HAL_PWREx_GetWakeupFlag (uint32_t WakeUpFlag)
{
  /* Check the parameters */
  assert_param (IS_PWR_WAKEUP_FLAG (WakeUpFlag));

  /* Return the wake up pin flag */
  return (PWR->WKUPFR & WakeUpFlag);
}

/**
  * @brief Clear the Wake-Up pin pending flag.
  * @param  WakeUpFlag: Specifies the Wake-Up PIN flag to clear.
  *          This parameter can be one of the following values:
  *            @arg PWR_WAKEUP_FLAG1 : Clear the wakeup event received from PA0.
  *            @arg PWR_WAKEUP_FLAG2 : Clear the wakeup event received from PA2.
  *            @arg PWR_WAKEUP_FLAG3 : Clear the wakeup event received from PI8.
  *            @arg PWR_WAKEUP_FLAG4 : Clear the wakeup event received from PC13.
  *            @arg PWR_WAKEUP_FLAG5 : Clear the wakeup event received from PI11.
  *            @arg PWR_WAKEUP_FLAG6 : Clear the wakeup event received from PC1.
  *            @arg PWR_WAKEUP_FLAG_ALL : Clear the wakeup events received from
  *                                      all wake up pins.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_ClearWakeupFlag (uint32_t WakeUpFlag)
{
  /* Check the parameter */
  assert_param (IS_PWR_WAKEUP_FLAG (WakeUpFlag));

  /* Clear the wake up event received from wake up pin x */
  SET_BIT (PWR->WKUPCR, WakeUpFlag);

  /* Check if the wake up event is well cleared */
  if ((PWR->WKUPFR & WakeUpFlag) != 0U)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief This function handles the PWR WAKEUP PIN interrupt request.
  * @note   This API should be called under the WAKEUP_PIN_IRQHandler().
  * @retval None.
  */
void HAL_PWREx_WAKEUP_PIN_IRQHandler (void)
{
  /* Wakeup pin EXTI line interrupt detected */
  if (READ_BIT(PWR->WKUPFR, PWR_WKUPFR_WKUPF1) != 0U)
  {
    /* Clear PWR WKUPF1 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP1);

    /* PWR WKUP1 interrupt user callback */
    HAL_PWREx_WKUP1_Callback ();
  }
  else if (READ_BIT (PWR->WKUPFR, PWR_WKUPFR_WKUPF2) != 0U)
  {
    /* Clear PWR WKUPF2 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP2);

    /* PWR WKUP2 interrupt user callback */
    HAL_PWREx_WKUP2_Callback ();
  }
  else if (READ_BIT (PWR->WKUPFR, PWR_WKUPFR_WKUPF3) != 0U)
  {
    /* Clear PWR WKUPF3 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP3);

    /* PWR WKUP3 interrupt user callback */
    HAL_PWREx_WKUP3_Callback ();
  }
  else if (READ_BIT (PWR->WKUPFR, PWR_WKUPFR_WKUPF4) != 0U)
  {
    /* Clear PWR WKUPF4 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP4);

    /* PWR WKUP4 interrupt user callback */
    HAL_PWREx_WKUP4_Callback ();
  }
  else if (READ_BIT (PWR->WKUPFR, PWR_WKUPFR_WKUPF5) != 0U)
  {
    /* Clear PWR WKUPF5 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP5);

    /* PWR WKUP5 interrupt user callback */
    HAL_PWREx_WKUP5_Callback ();
  }
  else
  {
    /* Clear PWR WKUPF6 flag */
    __HAL_PWR_CLEAR_WAKEUPFLAG (PWR_FLAG_WKUP6);

    /* PWR WKUP6 interrupt user callback */
    HAL_PWREx_WKUP6_Callback ();
  }
}

/**
  * @brief PWR WKUP1 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP1_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP1Callback can be implemented in the user file
  */
}

/**
  * @brief PWR WKUP2 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP2_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP2Callback can be implemented in the user file
  */
}

/**
  * @brief PWR WKUP3 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP3_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP3Callback can be implemented in the user file
  */
}

/**
  * @brief PWR WKUP4 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP4_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP4Callback can be implemented in the user file
  */
}

/**
  * @brief PWR WKUP5 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP5_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP5Callback can be implemented in the user file
  */
}

/**
  * @brief PWR WKUP6 interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_WKUP6_Callback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWREx_WKUP6Callback can be implemented in the user file
  */
}
/**
  * @}
  */

/** @defgroup PWREx_Exported_Functions_Group3 Peripherals control functions
  * @brief    Peripherals control functions
  *
@verbatim
 ===============================================================================
                 ##### Peripherals control functions #####
 ===============================================================================

    *** Main and Backup Regulators configuration ***
    ================================================
    [..]
      (+) The backup domain includes 4 Kbytes of backup SRAM accessible only
          from the CPU, and addressed in 32-bit, 16-bit or 8-bit mode. Its
          content is retained even in Standby or VBAT mode when the low power
          backup regulator is enabled. It can be considered as an internal
          EEPROM when VBAT is always present. You can use the
          HAL_PWREx_EnableBkUpReg() function to enable the low power backup
          regulator.
      (+) When the backup domain is supplied by VDD (analog switch connected to
          VDD) the backup SRAM is powered from VDD which replaces the VBAT power
          supply to save battery life.
      (+) The backup SRAM is not mass erased by a tamper event. It is read
          protected to prevent confidential data, such as cryptographic private
          key, from being accessed. The backup SRAM can be erased only through
          the Flash interface when a protection level change from level 1 to
          level 0 is requested.
      -@- Refer to the description of Read protection (RDP) in the Flash
          programming manual.
      (+) The main internal regulator can be configured to have a tradeoff
          between performance and power consumption when the device does not
          operate at the maximum frequency. This is done through
          HAL_PWREx_ControlVoltageScaling(VOS) function which configure the VOS
          bit in PWR_D3CR register.
      (+) The main internal regulator can be configured to operate in Low Power
          mode when the system enters STOP mode to further reduce power
          consumption.
          This is done through HAL_PWREx_ControlStopModeVoltageScaling(SVOS)
          function which configure the SVOS bit in PWR_CR1 register.
          The selected SVOS4 and SVOS5 levels add an additional startup delay
          when exiting from system Stop mode.
    -@- Refer to the product datasheets for more details.

    *** USB Regulator configuration ***
    ===================================
    [..]
      (+) The USB transceivers are supplied from a dedicated VDD33USB supply
          that can be provided either by the integrated USB regulator, or by an
          external USB supply.
      (+) The USB regulator is enabled by HAL_PWREx_EnableUSBReg() function, the
          VDD33USB is then provided from the USB regulator.
      (+) When the USB regulator is enabled, the VDD33USB supply level detector
          shall be enabled through  HAL_PWREx_EnableUSBVoltageDetector()
          function.
      (+) The USB regulator is disabled through HAL_PWREx_DisableUSBReg()
          function and VDD33USB can be provided from an external supply. In this
          case VDD33USB and VDD50USB shall be connected together.

    *** VBAT battery charging ***
    =============================
    [..]
      (+) When VDD is present, the external battery connected to VBAT can be
          charged through an internal resistance. VBAT charging can be performed
          either through a 5 KOhm resistor or through a 1.5 KOhm resistor.
      (+) VBAT charging is enabled by HAL_PWREx_EnableBatteryCharging
          (ResistorValue) function with:
       (++) ResistorValue:
        (+++) PWR_BATTERY_CHARGING_RESISTOR_5: 5 KOhm resistor.
        (+++) PWR_BATTERY_CHARGING_RESISTOR_1_5: 1.5 KOhm resistor.
      (+) VBAT charging is disabled by HAL_PWREx_DisableBatteryCharging()
          function.

@endverbatim
  * @{
  */

/**
  * @brief Enable the Backup Regulator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg (void)
{
  uint32_t tickstart;

  /* Enable the Backup regulator */
  SET_BIT (PWR->CR2, PWR_CR2_BREN);

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till Backup regulator ready flag is set */
  while (__HAL_PWR_GET_FLAG (PWR_FLAG_BRR) == 0U)
  {
    if ((HAL_GetTick() - tickstart ) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Disable the Backup Regulator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg (void)
{
  uint32_t tickstart;

  /* Disable the Backup regulator */
  CLEAR_BIT (PWR->CR2, PWR_CR2_BREN);

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till Backup regulator ready flag is reset */
  while (__HAL_PWR_GET_FLAG (PWR_FLAG_BRR) != 0U)
  {
    if ((HAL_GetTick() - tickstart ) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Enable the USB Regulator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_EnableUSBReg (void)
{
  uint32_t tickstart;

  /* Enable the USB regulator */
  SET_BIT (PWR->CR3, PWR_CR3_USBREGEN);

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till the USB regulator ready flag is set */
  while (__HAL_PWR_GET_FLAG (PWR_FLAG_USB33RDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart ) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Disable the USB Regulator.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_PWREx_DisableUSBReg (void)
{
  uint32_t tickstart;

  /* Disable the USB regulator */
  CLEAR_BIT (PWR->CR3, PWR_CR3_USBREGEN);

  /* Get tick */
  tickstart = HAL_GetTick ();

  /* Wait till the USB regulator ready flag is reset */
  while(__HAL_PWR_GET_FLAG (PWR_FLAG_USB33RDY) != 0U)
  {
    if ((HAL_GetTick() - tickstart ) > PWR_FLAG_SETTING_DELAY)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief Enable the USB voltage level detector.
  * @retval None.
  */
void HAL_PWREx_EnableUSBVoltageDetector (void)
{
  /* Enable the USB voltage detector */
  SET_BIT (PWR->CR3, PWR_CR3_USB33DEN);
}

/**
  * @brief Disable the USB voltage level detector.
  * @retval None.
  */
void HAL_PWREx_DisableUSBVoltageDetector (void)
{
  /* Disable the USB voltage detector */
  CLEAR_BIT (PWR->CR3, PWR_CR3_USB33DEN);
}

/**
  * @brief Enable the Battery charging.
  * @note   When VDD is present, charge the external battery through an internal
  *         resistor.
  * @param  ResistorValue : Specifies the charging resistor.
  *          This parameter can be one of the following values :
  *            @arg PWR_BATTERY_CHARGING_RESISTOR_5 : 5 KOhm resistor.
  *            @arg PWR_BATTERY_CHARGING_RESISTOR_1_5 : 1.5 KOhm resistor.
  * @retval None.
  */
void HAL_PWREx_EnableBatteryCharging (uint32_t ResistorValue)
{
  /* Check the parameter */
  assert_param (IS_PWR_BATTERY_RESISTOR_SELECT (ResistorValue));

  /* Specify the charging resistor */
  MODIFY_REG (PWR->CR3, PWR_CR3_VBRS, ResistorValue);

  /* Enable the Battery charging */
  SET_BIT (PWR->CR3, PWR_CR3_VBE);
}

/**
  * @brief Disable the Battery charging.
  * @retval None.
  */
void HAL_PWREx_DisableBatteryCharging (void)
{
  /* Disable the Battery charging */
  CLEAR_BIT (PWR->CR3, PWR_CR3_VBE);
}

#if defined (PWR_CR1_BOOSTE)
/**
  * @brief Enable the booster to guarantee the analog switch AC performance when
  *        the VDD supply voltage is below 2V7.
  * @note   The VDD supply voltage can be monitored through the PVD and the PLS
  *         field bits.
  * @retval None.
  */
void HAL_PWREx_EnableAnalogBooster (void)
{
  /* Enable the Analog voltage */
  SET_BIT (PWR->CR1, PWR_CR1_AVD_READY);

  /* Enable VDDA booster */
  SET_BIT (PWR->CR1, PWR_CR1_BOOSTE);
}

/**
  * @brief Disable the analog booster.
  * @retval None.
  */
void HAL_PWREx_DisableAnalogBooster (void)
{
  /* Disable VDDA booster */
  CLEAR_BIT (PWR->CR1, PWR_CR1_BOOSTE);

  /* Disable the Analog voltage */
  CLEAR_BIT (PWR->CR1, PWR_CR1_AVD_READY);
}
#endif /* defined (PWR_CR1_BOOSTE) */
/**
  * @}
  */

/** @defgroup PWREx_Exported_Functions_Group4 Power Monitoring functions
  * @brief    Power Monitoring functions
  *
@verbatim
 ===============================================================================
                 ##### Power Monitoring functions #####
 ===============================================================================

    *** VBAT and Temperature supervision ***
    ========================================
    [..]
      (+) The VBAT battery voltage supply can be monitored by comparing it with
          two threshold levels: VBAThigh and VBATlow. VBATH flag and VBATL flags
          in the PWR control register 2 (PWR_CR2), indicate if VBAT is higher or
          lower than the threshold.
      (+) The temperature can be monitored by comparing it with two threshold
          levels, TEMPhigh and TEMPlow. TEMPH and TEMPL flags, in the PWR
          control register 2 (PWR_CR2), indicate whether the device temperature
          is higher or lower than the threshold.
      (+) The VBAT and the temperature monitoring is enabled by
          HAL_PWREx_EnableMonitoring() function and disabled by
          HAL_PWREx_DisableMonitoring() function.
      (+) The HAL_PWREx_GetVBATLevel() function returns the VBAT level which can
          be : PWR_VBAT_BELOW_LOW_THRESHOLD or PWR_VBAT_ABOVE_HIGH_THRESHOLD or
          PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD.
      (+) The HAL_PWREx_GetTemperatureLevel() function returns the Temperature
          level which can be :
          PWR_TEMP_BELOW_LOW_THRESHOLD or PWR_TEMP_ABOVE_HIGH_THRESHOLD or
          PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD.

    *** AVD configuration ***
    =========================
    [..]
      (+) The AVD is used to monitor the VDDA power supply by comparing it to a
          threshold selected by the AVD Level (ALS[3:0] bits in the PWR_CR1
          register).
      (+) A AVDO flag is available to indicate if VDDA is higher or lower
          than the AVD threshold. This event is internally connected to the EXTI
          line 16 to generate an interrupt if enabled.
          It is configurable through __HAL_PWR_AVD_EXTI_ENABLE_IT() macro.
      (+) The AVD is stopped in System Standby mode.

@endverbatim
  * @{
  */

/**
  * @brief Enable the VBAT and temperature monitoring.
  * @retval HAL status.
  */
void HAL_PWREx_EnableMonitoring (void)
{
  /* Enable the VBAT and Temperature monitoring */
  SET_BIT (PWR->CR2, PWR_CR2_MONEN);
}

/**
  * @brief Disable the VBAT and temperature monitoring.
  * @retval HAL status.
  */
void HAL_PWREx_DisableMonitoring (void)
{
  /* Disable the VBAT and Temperature monitoring */
  CLEAR_BIT (PWR->CR2, PWR_CR2_MONEN);
}

/**
  * @brief Indicate whether the junction temperature is between, above or below
  *        the thresholds.
  * @retval Temperature level.
  */
uint32_t HAL_PWREx_GetTemperatureLevel (void)
{
  uint32_t tempLevel, regValue;

  /* Read the temperature flags */
  regValue = READ_BIT (PWR->CR2, (PWR_CR2_TEMPH | PWR_CR2_TEMPL));

  /* Check if the temperature is below the threshold */
  if (regValue == PWR_CR2_TEMPL)
  {
    tempLevel = PWR_TEMP_BELOW_LOW_THRESHOLD;
  }
  /* Check if the temperature is above the threshold */
  else if (regValue == PWR_CR2_TEMPH)
  {
    tempLevel = PWR_TEMP_ABOVE_HIGH_THRESHOLD;
  }
  /* The temperature is between the thresholds */
  else
  {
    tempLevel = PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD;
  }

  return tempLevel;
}

/**
  * @brief Indicate whether the Battery voltage level is between, above or below
  *        the thresholds.
  * @retval VBAT level.
  */
uint32_t HAL_PWREx_GetVBATLevel (void)
{
  uint32_t VBATLevel, regValue;

  /* Read the VBAT flags */
  regValue = READ_BIT (PWR->CR2, (PWR_CR2_VBATH | PWR_CR2_VBATL));

  /* Check if the VBAT is below the threshold */
  if (regValue == PWR_CR2_VBATL)
  {
    VBATLevel = PWR_VBAT_BELOW_LOW_THRESHOLD;
  }
  /* Check if the VBAT is above the threshold */
  else if (regValue == PWR_CR2_VBATH)
  {
    VBATLevel = PWR_VBAT_ABOVE_HIGH_THRESHOLD;
  }
  /* The VBAT is between the thresholds */
  else
  {
    VBATLevel = PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD;
  }

  return VBATLevel;
}

#if defined (PWR_CSR1_MMCVDO)
/**
  * @brief Get the VDDMMC voltage level.
  * @retval The VDDMMC voltage level.
  */
PWREx_MMC_VoltageLevel HAL_PWREx_GetMMCVoltage (void)
{
  PWREx_MMC_VoltageLevel mmc_voltage;

  /* Check voltage detector output on VDDMMC value */
  if ((PWR->CSR1 & PWR_CSR1_MMCVDO_Msk) == 0U)
  {
    mmc_voltage = PWR_MMC_VOLTAGE_BELOW_1V2;
  }
  else
  {
    mmc_voltage = PWR_MMC_VOLTAGE_EQUAL_ABOVE_1V2;
  }

  return mmc_voltage;
}
#endif /* defined (PWR_CSR1_MMCVDO) */

/**
  * @brief  Configure the event mode and the voltage threshold detected by the
  *         Analog Voltage Detector (AVD).
  * @param  sConfigAVD : Pointer to an PWREx_AVDTypeDef structure that contains
  *                      the configuration information for the AVD.
  * @note   Refer to the electrical characteristics of your device datasheet for
  *         more details about the voltage threshold corresponding to each
  *         detection level.
  * @note   For dual core devices, please ensure to configure the EXTI lines for
  *         the different Cortex-Mx through PWR_Exported_Macro provided by this
  *         driver. All combination are allowed: wake up only Cortex-M7, wake up
  *         only Cortex-M4 and wake up Cortex-M7 and Cortex-M4.
  * @retval None.
  */
void HAL_PWREx_ConfigAVD (PWREx_AVDTypeDef *sConfigAVD)
{
  /* Check the parameters */
  assert_param (IS_PWR_AVD_LEVEL (sConfigAVD->AVDLevel));
  assert_param (IS_PWR_AVD_MODE (sConfigAVD->Mode));

  /* Set the ALS[18:17] bits according to AVDLevel value */
  MODIFY_REG (PWR->CR1, PWR_CR1_ALS, sConfigAVD->AVDLevel);

  /* Clear any previous config */
#if !defined (DUAL_CORE)
  __HAL_PWR_AVD_EXTI_DISABLE_EVENT ();
  __HAL_PWR_AVD_EXTI_DISABLE_IT ();
#endif /* !defined (DUAL_CORE) */

  __HAL_PWR_AVD_EXTI_DISABLE_RISING_EDGE ();
  __HAL_PWR_AVD_EXTI_DISABLE_FALLING_EDGE ();

#if !defined (DUAL_CORE)
  /* Configure the interrupt mode */
  if ((sConfigAVD->Mode & AVD_MODE_IT) == AVD_MODE_IT)
  {
    __HAL_PWR_AVD_EXTI_ENABLE_IT ();
  }

  /* Configure the event mode */
  if ((sConfigAVD->Mode & AVD_MODE_EVT) == AVD_MODE_EVT)
  {
    __HAL_PWR_AVD_EXTI_ENABLE_EVENT ();
  }
#endif /* !defined (DUAL_CORE) */

  /* Rising edge configuration */
  if ((sConfigAVD->Mode & AVD_RISING_EDGE) == AVD_RISING_EDGE)
  {
    __HAL_PWR_AVD_EXTI_ENABLE_RISING_EDGE ();
  }

  /* Falling edge configuration */
  if ((sConfigAVD->Mode & AVD_FALLING_EDGE) == AVD_FALLING_EDGE)
  {
    __HAL_PWR_AVD_EXTI_ENABLE_FALLING_EDGE ();
  }
}

/**
  * @brief Enable the Analog Voltage Detector (AVD).
  * @retval None.
  */
void HAL_PWREx_EnableAVD (void)
{
  /* Enable the Analog Voltage Detector */
  SET_BIT (PWR->CR1, PWR_CR1_AVDEN);
}

/**
  * @brief Disable the Analog Voltage Detector(AVD).
  * @retval None.
  */
void HAL_PWREx_DisableAVD (void)
{
  /* Disable the Analog Voltage Detector */
  CLEAR_BIT (PWR->CR1, PWR_CR1_AVDEN);
}

/**
  * @brief  This function handles the PWR PVD/AVD interrupt request.
  * @note   This API should be called under the PVD_AVD_IRQHandler().
  * @retval None
  */
void HAL_PWREx_PVD_AVD_IRQHandler (void)
{
  /* Check if the Programmable Voltage Detector is enabled (PVD) */
  if(READ_BIT (PWR->CR1, PWR_CR1_PVDEN) != 0U)
  {
#if defined (DUAL_CORE)
    if (HAL_GetCurrentCPUID () == CM7_CPUID)
#endif /* defined (DUAL_CORE) */
    {
      /* Check PWR D1/CD EXTI flag */
      if(__HAL_PWR_PVD_EXTI_GET_FLAG () != 0U)
      {
        /* PWR PVD interrupt user callback */
        HAL_PWR_PVDCallback ();

        /* Clear PWR EXTI D1/CD pending bit */
        __HAL_PWR_PVD_EXTI_CLEAR_FLAG ();
      }
    }
#if defined (DUAL_CORE)
    else
    {
      /* Check PWR EXTI D2 flag */
      if (__HAL_PWR_PVD_EXTID2_GET_FLAG () != 0U)
      {
        /* PWR PVD interrupt user callback */
        HAL_PWR_PVDCallback ();

        /* Clear PWR EXTI D2 pending bit */
        __HAL_PWR_PVD_EXTID2_CLEAR_FLAG();
      }
    }
#endif /* defined (DUAL_CORE) */
  }

  /* Check if the Analog Voltage Detector is enabled (AVD) */
  if (READ_BIT (PWR->CR1, PWR_CR1_AVDEN) != 0U)
  {
#if defined (DUAL_CORE)
    if (HAL_GetCurrentCPUID () == CM7_CPUID)
#endif /* defined (DUAL_CORE) */
    {
      /* Check PWR EXTI D1/CD flag */
      if (__HAL_PWR_AVD_EXTI_GET_FLAG () != 0U)
      {
        /* PWR AVD interrupt user callback */
        HAL_PWREx_AVDCallback ();

        /* Clear PWR EXTI D1/CD pending bit */
        __HAL_PWR_AVD_EXTI_CLEAR_FLAG ();
      }
    }
#if defined (DUAL_CORE)
    else
    {
      /* Check PWR EXTI D2 flag */
      if (__HAL_PWR_AVD_EXTID2_GET_FLAG () != 0U)
      {
        /* PWR AVD interrupt user callback */
        HAL_PWREx_AVDCallback ();

        /* Clear PWR EXTI D2 pending bit */
        __HAL_PWR_AVD_EXTID2_CLEAR_FLAG ();
      }
    }
#endif /* defined (DUAL_CORE) */
  }
}

/**
  * @brief PWR AVD interrupt callback.
  * @retval None.
  */
__weak void HAL_PWREx_AVDCallback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWR_AVDCallback can be implemented in the user file
  */
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_PWR_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
