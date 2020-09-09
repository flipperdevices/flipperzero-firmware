/**
  ******************************************************************************
  * @file    stm32h7xx_hal_pwr.c
  * @author  MCD Application Team
  * @brief   PWR HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Power Controller (PWR) peripheral:
  *           + Initialization and de-initialization functions.
  *           + Peripheral Control functions.
  *           + Interrupt Handling functions.
  @verbatim
  ==============================================================================
                        ##### PWR peripheral overview #####
  ==============================================================================
  [..]
   (#) The Power control (PWR) provides an overview of the supply architecture
       for the different power domains and of the supply configuration
       controller.
       In the H7 family, the number of power domains is different between
       device lines. This difference is due to characteristics of each device.

   (#) Domain architecture overview for the different H7 lines:
      (+) Dual core lines are STM32H745, STM32H747, STM32H755 and STM32H757.
          These devices have 3 power domains (D1, D2 and D3).
          The domain D1 contains a CPU (Cortex-M7), a Flash memory and some
          peripherals. The D2 domain contains peripherals and a CPU
          (Cortex-M4). The D3 domain contains the system control, I/O logic
          and low-power peripherals.
      (+) STM32H743, STM32H753, STM32H742 and STM32H750 devices have 3 power
          domains (D1, D2 and D3).
          The domain D1 contains a CPU (Cortex-M7), a Flash memory and some
          peripherals. The D2 domain contains peripherals. The D3 domains
          contains the system control, I/O logic and low-power peripherals.
      (+) STM32H7AxxQ, STM32H7BxxQ, STM32H7Axxx and STM32H7Bxxx devices have 2
          power domains (CD and SRD).
          The core domain (CD) contains a CPU (Cortex-M7), a Flash
          memory and peripherals. The SmartRun domain contains the system
          control, I/O logic and low-power peripherals.

   (#) Every entity have low power mode as decribed below :
   (#) The CPU low power modes are :
      (+) CPU CRUN.
      (+) CPU CSLEEP.
      (+) CPU CSTOP.
   (#) The domain low power modes are :
      (+) DRUN.
      (+) DSTOP.
      (+) DSTANDBY.
   (#) The SYSTEM low power modes are :
      (+) RUN* : The Run* mode is entered after a POR reset and a wakeup from
                 Standby. In Run* mode, the performance is limited and the
                 system supply configuration shall be programmed. The system
                 enters Run mode only when the ACTVOSRDY bit in PWR control
                 status register 1 (PWR_CSR1) is set to 1.
      (+) RUN.
      (+) STOP.
      (+) STANDBY.

  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) Power management peripheral is active by default at startup level in
       STM32h7xx lines.

   (#) Call HAL_PWR_EnableBkUpAccess() and HAL_PWR_DisableBkUpAccess() functions
       to enable/disable access to the backup domain (RTC registers, RTC backup
       data registers and backup SRAM).

   (#) Call HAL_PWR_ConfigPVD() after setting parameters to be configured (event
       mode and voltage threshold) in order to set up the Power Voltage Detector,
       then use HAL_PWR_EnablePVD() and  HAL_PWR_DisablePVD() functions to start
       and stop the PVD detection.
       (+) PVD level could be one of the following values :
             (++) 1V95
             (++) 2V1
             (++) 2V25
             (++) 2V4
             (++) 2V55
             (++) 2V7
             (++) 2V85
             (++) External voltage level

   (#) Call HAL_PWR_EnableWakeUpPin() and HAL_PWR_DisableWakeUpPin() functions
       with the right parameter to configure the wake up pin polarity (Low or
       High) and to enable and disable it.

   (#) Call HAL_PWR_EnterSLEEPMode() function to enter the current Core in SLEEP
       mode. Wake-up from SLEEP mode could be following to an event or an
       interrupt according to low power mode intrinsic request called (__WFI()
       or __WFE()).
       Please ensure to clear all CPU pending events by calling
       HAL_PWREx_ClearPendingEvent() function when trying to enter the Cortex-Mx
       in SLEEP mode with __WFE() entry.

   (#) Call HAL_PWR_EnterSTOPMode() function to enter the whole system to Stop 0
       mode for single core devices. For dual core devices, this API will enter
       the domain (containing Cortex-Mx that executing this function) in DSTOP
       mode. According to the used parameter, user could select the regulator to
       be kept actif in low power mode and wake-up event type.
       Please ensure to clear all CPU pending events by calling
       HAL_PWREx_ClearPendingEvent() function when trying to enter the Cortex-Mx
       in CSTOP mode with __WFE() entry.

   (#) Call HAL_PWR_EnterSTANDBYMode() function to enter the whole system in
       STANDBY mode for single core devices. For dual core devices, this API
       will enter the domain (containing Cortex-Mx that executing this function)
       in DSTANDBY mode.

   (#) Call HAL_PWR_EnableSleepOnExit() and HAL_PWR_DisableSleepOnExit() APIs to
       enable and disable the Cortex-Mx re-entring in SLEEP mode after an
       interruption handling is over.

   (#) Call HAL_PWR_EnableSEVOnPend() and HAL_PWR_DisableSEVOnPend() functions
       to configure the Cortex-Mx to wake-up after any pending event / interrupt
       even if it's disabled or has insufficient priority to cause exception
       entry.

   (#) Call HAL_PWR_PVD_IRQHandler() function to handle the PWR PVD interrupt
       request.

     *** PWR HAL driver macros list ***
     =============================================
     [..]
       Below the list of most used macros in PWR HAL driver.

      (+) __HAL_PWR_VOLTAGESCALING_CONFIG() : Configure the main internal
                                              regulator output voltage.
      (+) __HAL_PWR_GET_FLAG()              : Get the PWR pending flags.
      (+) __HAL_PWR_CLEAR_FLAG()            : Clear the PWR pending flags.

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

/** @defgroup PWR PWR
  * @brief PWR HAL module driver
  * @{
  */

#ifdef HAL_PWR_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @addtogroup PWR_Private_Constants PWR Private Constants
  * @{
  */

/** @defgroup PWR_PVD_Mode_Mask PWR PVD Mode Mask
  * @{
  */
#if !defined (DUAL_CORE)
#define PVD_MODE_IT              (0x00010000U)
#define PVD_MODE_EVT             (0x00020000U)
#endif /* !defined (DUAL_CORE) */

#define PVD_RISING_EDGE          (0x00000001U)
#define PVD_FALLING_EDGE         (0x00000002U)
#define PVD_RISING_FALLING_EDGE  (0x00000003U)
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

/** @defgroup PWR_Exported_Functions PWR Exported Functions
  * @{
  */

/** @defgroup PWR_Exported_Functions_Group1 Initialization and De-Initialization Functions
  * @brief    Initialization and De-Initialization functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and De-Initialization Functions #####
 ===============================================================================
    [..]
      This section provides functions allowing to deinitialize power peripheral.

    [..]
      After system reset, the backup domain (RTC registers, RTC backup data
      registers and backup SRAM) is protected against possible unwanted write
      accesses.
      The HAL_PWR_EnableBkUpAccess() function enables the access to the backup
      domain.
      The HAL_PWR_DisableBkUpAccess() function disables the access to the backup
      domain.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitialize the HAL PWR peripheral registers to their default reset
  *         values.
  * @note   This functionality is not available in this product.
  *         The prototype is kept just to maintain compatibility with other
  *         products.
  * @retval None.
  */
void HAL_PWR_DeInit (void)
{
}

/**
  * @brief  Enable access to the backup domain (RTC registers, RTC backup data
  *         registers and backup SRAM).
  * @note   If the HSE divided by 2, 3, ..31 is used as the RTC clock, the
  *         Backup Domain Access should be kept enabled.
  * @retval None.
  */
void HAL_PWR_EnableBkUpAccess (void)
{
  /* Enable access to RTC and backup registers */
  SET_BIT (PWR->CR1, PWR_CR1_DBP);
}

/**
  * @brief  Disable access to the backup domain (RTC registers, RTC backup data
  *         registers and backup SRAM).
  * @note   If the HSE divided by 2, 3, ..31 is used as the RTC clock, the
  *         Backup Domain Access should be kept enabled.
  * @retval None.
  */
void HAL_PWR_DisableBkUpAccess (void)
{
  /* Disable access to RTC and backup registers */
  CLEAR_BIT (PWR->CR1, PWR_CR1_DBP);
}
/**
  * @}
  */

/** @defgroup PWR_Exported_Functions_Group2 Peripheral Control Functions
  *  @brief   Power Control functions
  *
@verbatim
 ===============================================================================
                 ##### Peripheral Control Functions #####
 ===============================================================================
    [..]
      This section provides functions allowing to control power peripheral.

    *** PVD configuration ***
    =========================
    [..]
      (+) The PVD is used to monitor the VDD power supply by comparing it to a
          threshold selected by the PVD Level (PLS[7:0] bits in the PWR_CR1
          register).

      (+) A PVDO flag is available to indicate if VDD is higher or lower
          than the PVD threshold. This event is internally connected to the EXTI
          line 16 to generate an interrupt if enabled.
          It is configurable through __HAL_PWR_PVD_EXTI_ENABLE_IT() macro.

      (+) The PVD is stopped in STANDBY mode.

    *** Wake-up pin configuration ***
    =================================
    [..]
      (+) Wake-up pin is used to wake up the system from STANDBY mode.
          The pin pull is configurable through the WKUPEPR register to be in
          No-pull, Pull-up and Pull-down.
          The pin polarity is configurable through the WKUPEPR register to be
          active on rising or falling edges.

      (+) There are up to six Wake-up pin in the STM32H7 devices family.

    *** Low Power modes configuration ***
    =====================================
    [..]
     The device present 3 principles low-power modes features:
      (+) SLEEP mode   : Cortex-Mx is stopped and all PWR domains are remaining
                         active (Powered and Clocked).

      (+) STOP mode    : Cortex-Mx is stopped, clocks are stopped and the
                         regulator is running. The Main regulator or the LP
                         regulator could be selected.

      (+) STANDBY mode : All PWR domains enter DSTANDBY mode and the VCORE
                         supply regulator is powered off.

   *** SLEEP mode ***
   ==================
    [..]
      (+) Entry:
        The SLEEP mode is entered by using the HAL_PWR_EnterSLEEPMode(Regulator,
        SLEEPEntry) function.

          (++) PWR_SLEEPENTRY_WFI: enter SLEEP mode with WFI instruction.
          (++) PWR_SLEEPENTRY_WFE: enter SLEEP mode with WFE instruction.

      -@@- The Regulator parameter is not used for the STM32H7 family
              and is kept as parameter just to maintain compatibility with the
              lower power families (STM32L).

      (+) Exit:
        Any peripheral interrupt acknowledged by the nested vectored interrupt
        controller (NVIC) can wake up the device from SLEEP mode.

   *** STOP mode ***
   =================
    [..]
      In system STOP mode, all clocks in the 1.2V domain are stopped, the PLL,
      the HSI, and the HSE RC oscillators are disabled. Internal SRAM and
      register contents are preserved.
      The voltage regulator can be configured either in normal or low-power mode.
      To minimize the consumption in STOP mode, FLASH can be powered off before
      entering the STOP mode using the HAL_PWREx_EnableFlashPowerDown() function.
      It can be switched on again by software after exiting the STOP mode using
      the HAL_PWREx_DisableFlashPowerDown() function.

      (+) Entry:
         The STOP mode is entered using the HAL_PWR_EnterSTOPMode(Regulator,
         STOPEntry) function with:

         (++) Regulator:
          (+++) PWR_MAINREGULATOR_ON: Main regulator ON.
          (+++) PWR_LOWPOWERREGULATOR_ON: Low Power regulator ON.

         (++) STOPEntry:
          (+++) PWR_STOPENTRY_WFI: enter STOP mode with WFI instruction.
          (+++) PWR_STOPENTRY_WFE: enter STOP mode with WFE instruction.

      (+) Exit:
         Any EXTI Line (Internal or External) configured in Interrupt/Event mode.

   *** STANDBY mode ***
   ====================
    [..]
    (+)
      The system STANDBY mode allows to achieve the lowest power consumption.
      It is based on the Cortex-Mx deep SLEEP mode, with the voltage regulator
      disabled. The system is consequently powered off. The PLL, the HSI
      oscillator and the HSE oscillator are also switched off. SRAM and register
      contents are lost except for the RTC registers, RTC backup registers,
      backup SRAM and standby circuitry.

    [..]
      The voltage regulator is OFF.

      (++) Entry:
        (+++) The STANDBY mode is entered using the HAL_PWR_EnterSTANDBYMode()
              function.

      (++) Exit:
        (+++) WKUP pin rising or falling edge, RTC alarm (Alarm A and Alarm B),
              RTC wakeup, tamper event, time stamp event, external reset in NRST
              pin, IWDG reset.

   *** Auto-wakeup (AWU) from low-power mode ***
   =============================================
    [..]
     (+) The MCU can be woken up from low-power mode by an RTC Alarm event, an
         RTC Wakeup event, a tamper event or a time-stamp event, without
         depending on an external interrupt (Auto-wakeup mode).

     (+) RTC auto-wakeup (AWU) from the STOP and STANDBY modes

       (++) To wake up from the STOP mode with an RTC alarm event, it is
            necessary to configure the RTC to generate the RTC alarm using the
            HAL_RTC_SetAlarm_IT() function.

       (++) To wake up from the STOP mode with an RTC Tamper or time stamp event,
            it is necessary to configure the RTC to detect the tamper or time
            stamp event using the HAL_RTCEx_SetTimeStamp_IT() or
            HAL_RTCEx_SetTamper_IT() functions.

       (++) To wake up from the STOP mode with an RTC WakeUp event, it is
            necessary to configure the RTC to generate the RTC WakeUp event
            using the HAL_RTCEx_SetWakeUpTimer_IT() function.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the event mode and the voltage threshold detected by the
  *         Programmable Voltage Detector(PVD).
  * @param  sConfigPVD : Pointer to an PWR_PVDTypeDef structure that contains
  *                      the configuration information for the PVD.
  * @note   Refer to the electrical characteristics of your device datasheet for
  *         more details about the voltage threshold corresponding to each
  *         detection level.
  * @note   For dual core devices, please ensure to configure the EXTI lines for
  *         the different Cortex-Mx through PWR_Exported_Macro provided by this
  *         driver. All combination are allowed: wake up only Cortex-M7, wake up
  *         only Cortex-M4 or wake up Cortex-M7 and Cortex-M4.
  * @retval None.
  */
void HAL_PWR_ConfigPVD (PWR_PVDTypeDef *sConfigPVD)
{
  /* Check the PVD configuration parameter */
  if (sConfigPVD == NULL)
  {
    return;
  }

  /* Check the parameters */
  assert_param (IS_PWR_PVD_LEVEL (sConfigPVD->PVDLevel));
  assert_param (IS_PWR_PVD_MODE (sConfigPVD->Mode));

  /* Set PLS[7:5] bits according to PVDLevel value */
  MODIFY_REG (PWR->CR1, PWR_CR1_PLS, sConfigPVD->PVDLevel);

  /* Clear previous config */
#if !defined (DUAL_CORE)
  __HAL_PWR_PVD_EXTI_DISABLE_EVENT ();
  __HAL_PWR_PVD_EXTI_DISABLE_IT ();
#endif /* !defined (DUAL_CORE) */

  __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE ();
  __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE ();

#if !defined (DUAL_CORE)
  /* Interrupt mode configuration */
  if ((sConfigPVD->Mode & PVD_MODE_IT) == PVD_MODE_IT)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_IT ();
  }

  /* Event mode configuration */
  if ((sConfigPVD->Mode & PVD_MODE_EVT) == PVD_MODE_EVT)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_EVENT ();
  }
#endif /* !defined (DUAL_CORE) */

  /* Rising edge configuration */
  if ((sConfigPVD->Mode & PVD_RISING_EDGE) == PVD_RISING_EDGE)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE ();
  }

  /* Falling edge configuration */
  if ((sConfigPVD->Mode & PVD_FALLING_EDGE) == PVD_FALLING_EDGE)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE ();
  }
}

/**
  * @brief Enable the Programmable Voltage Detector (PVD).
  * @retval None.
  */
void HAL_PWR_EnablePVD (void)
{
  /* Enable the power voltage detector */
  SET_BIT (PWR->CR1, PWR_CR1_PVDEN);
}

/**
  * @brief Disable the Programmable Voltage Detector (PVD).
  * @retval None.
  */
void HAL_PWR_DisablePVD (void)
{
  /* Disable the power voltage detector */
  CLEAR_BIT (PWR->CR1, PWR_CR1_PVDEN);
}

/**
  * @brief  Enable the WakeUp PINx functionality.
  * @param  WakeUpPinPolarity : Specifies which Wake-Up pin to enable.
  *          This parameter can be one of the following legacy values, which
  *          sets the default (rising edge):
  *            @arg PWR_WAKEUP_PIN1, PWR_WAKEUP_PIN2, PWR_WAKEUP_PIN3,
  *                 PWR_WAKEUP_PIN4, PWR_WAKEUP_PIN5, PWR_WAKEUP_PIN6.
  *          or one of the following values where the user can explicitly states
  *          the enabled pin and the chosen polarity:
  *            @arg PWR_WAKEUP_PIN1_HIGH, PWR_WAKEUP_PIN1_LOW,
  *                 PWR_WAKEUP_PIN2_HIGH, PWR_WAKEUP_PIN2_LOW,
  *                 PWR_WAKEUP_PIN3_HIGH, PWR_WAKEUP_PIN3_LOW,
  *                 PWR_WAKEUP_PIN4_HIGH, PWR_WAKEUP_PIN4_LOW,
  *                 PWR_WAKEUP_PIN5_HIGH, PWR_WAKEUP_PIN5_LOW,
  *                 PWR_WAKEUP_PIN6_HIGH, PWR_WAKEUP_PIN6_LOW.
  * @note   PWR_WAKEUP_PINx and PWR_WAKEUP_PINx_HIGH are equivalent.
  * @retval None.
  */
void HAL_PWR_EnableWakeUpPin (uint32_t WakeUpPinPolarity)
{
  /* Check the parameters */
  assert_param (IS_PWR_WAKEUP_PIN (WakeUpPinPolarity));

  /*
     Enable and Specify the Wake-Up pin polarity and the pull configuration
     for the event detection (rising or falling edge).
  */
  MODIFY_REG (PWR->WKUPEPR, PWR_EWUP_MASK, WakeUpPinPolarity);
}

/**
  * @brief  Disable the WakeUp PINx functionality.
  * @param  WakeUpPinx : Specifies the Power Wake-Up pin to disable.
  *          This parameter can be one of the following values:
  *            @arg PWR_WAKEUP_PIN1, PWR_WAKEUP_PIN2, PWR_WAKEUP_PIN3,
  *                 PWR_WAKEUP_PIN4, PWR_WAKEUP_PIN5, PWR_WAKEUP_PIN6,
  *                 PWR_WAKEUP_PIN1_HIGH, PWR_WAKEUP_PIN1_LOW,
  *                 PWR_WAKEUP_PIN2_HIGH, PWR_WAKEUP_PIN2_LOW,
  *                 PWR_WAKEUP_PIN3_HIGH, PWR_WAKEUP_PIN3_LOW,
  *                 PWR_WAKEUP_PIN4_HIGH, PWR_WAKEUP_PIN4_LOW,
  *                 PWR_WAKEUP_PIN5_HIGH, PWR_WAKEUP_PIN5_LOW,
  *                 PWR_WAKEUP_PIN6_HIGH, PWR_WAKEUP_PIN6_LOW.
  * @retval None.
  */
void HAL_PWR_DisableWakeUpPin (uint32_t WakeUpPinx)
{
  /* Check the parameters */
  assert_param (IS_PWR_WAKEUP_PIN (WakeUpPinx));

  /* Disable the wake up pin selected */
  CLEAR_BIT (PWR->WKUPEPR, (PWR_WKUPEPR_WKUPEN & WakeUpPinx));
}

/**
  * @brief  Enter the current core in SLEEP mode (CSLEEP).
  * @param  Regulator : Specifies the regulator state in SLEEP mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MAINREGULATOR_ON     : SLEEP mode with regulator ON.
  *            @arg PWR_LOWPOWERREGULATOR_ON : SLEEP mode with low power
  *                                           regulator ON.
  * @note   This parameter is not used for the STM32H7 family and is kept as
  *         parameter just to maintain compatibility with the lower power
  *         families.
  * @param  SLEEPEntry : Specifies if SLEEP mode is entered with WFI or WFE
  *                      intrinsic instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_SLEEPENTRY_WFI : enter SLEEP mode with WFI instruction.
  *            @arg PWR_SLEEPENTRY_WFE : enter SLEEP mode with WFE instruction.
  * @note   Ensure to clear pending events before calling this API through
  *         HAL_PWREx_ClearPendingEvent() when the SLEEP entry is WFE.
  * @retval None.
  */
void HAL_PWR_EnterSLEEPMode (uint32_t Regulator, uint8_t SLEEPEntry)
{
  /* Check the parameters */
  assert_param (IS_PWR_REGULATOR (Regulator));
  assert_param (IS_PWR_SLEEP_ENTRY (SLEEPEntry));

  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Select SLEEP mode entry */
  if (SLEEPEntry == PWR_SLEEPENTRY_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI ();
  }
  else
  {
    /* Request Wait For Event */
    __WFE ();
  }
}

/**
  * @brief  Enter STOP mode.
  * @note   For single core devices, this API will enter the system in STOP mode
  *         with all domains in DSTOP, if RUN_D3/RUN_SRD bit in CPUCR regiter is
  *         cleared.
  *         For dual core devices, this API will enter the domain (containing
  *         Cortex-Mx that executing this function) in DSTOP mode. If all
  *         Cortex-Mx domains are in DSTOP and RUN_D3 bit in CPUCR register is
  *         cleared, all the system will enter in STOP mode.
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
  * @note   In System STOP mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting System STOP mode by issuing an interrupt or a wakeup
  *         event, the HSI RC oscillator is selected as default system wakeup
  *         clock.
  * @note   In System STOP mode, when the voltage regulator operates in low
  *         power mode, an additional startup delay is incurred when the system
  *         is waking up. By keeping the internal regulator ON during STOP mode,
  *         the consumption is higher although the startup time is reduced.
  * @retval None.
  */
void HAL_PWR_EnterSTOPMode (uint32_t Regulator, uint8_t STOPEntry)
{
  /* Check the parameters */
  assert_param (IS_PWR_REGULATOR (Regulator));
  assert_param (IS_PWR_STOP_ENTRY (STOPEntry));

  /* Select the regulator state in STOP mode */
  MODIFY_REG (PWR->CR1, PWR_CR1_LPDS, Regulator);

  /* Configure the PWR mode for the different Domains */
#if defined (DUAL_CORE)
  /* Check CPU ID */
  if (HAL_GetCurrentCPUID () == CM7_CPUID)
  {
    /* Keep DSTOP mode when Cortex-M7 enters DEEP-SLEEP */
    CLEAR_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D3));
  }
  else
  {
    /* Keep DSTOP mode when Cortex-M4 enters DEEP-SLEEP */
    CLEAR_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D2 | PWR_CPUCR_PDDS_D3));
  }
#else /* Single core devices */
  /* Keep DSTOP mode when Cortex-M7 enter in DEEP-SLEEP */
  CLEAR_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D3));

#if defined (PWR_CPUCR_PDDS_D2)
  /* Keep DSTOP mode when Cortex-M7 enter in DEEP-SLEEP */
  CLEAR_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_D2);
#endif /* PWR_CPUCR_PDDS_D2 */
#endif /* defined (DUAL_CORE) */

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Ensure that all instructions are done before entering STOP mode */
  __DSB ();
  __ISB ();

  /* Select STOP mode entry */
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

/**
  * @brief  Enter STANDBY mode.
  * @note   For single core devices, this API will enter the system in STANDBY
  *         mode with all domains in DSTANDBY, if RUN_D3/RUN_SRD bit in CPUCR
  *         regiter is cleared.
  *         For dual core devices, this API will enter the domain (containing
  *         Cortex-Mx that executing this function) in DSTANDBY mode. If all
  *         Cortex-Mx domains are in DSTANDBY and RUN_D3 bit in CPUCR register
  *         is cleared, all the system will enter in STANDBY mode.
  * @note   The system enters Standby mode only when all domains are in DSTANDBY.
  * @note   When the System exit STANDBY mode by issuing an interrupt or a
  *         wakeup event, the HSI RC oscillator is selected as system clock.
  * @retval None.
  */
void HAL_PWR_EnterSTANDBYMode (void)
{
  /* Configure the PWR mode for the different Domains */
#if defined (DUAL_CORE)
  /* Check CPU ID */
  if (HAL_GetCurrentCPUID () == CM7_CPUID)
  {
    /* Enter DSTANDBY mode when Cortex-M7 enters DEEP-SLEEP */
    SET_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D3));
    SET_BIT (PWR->CPU2CR, (PWR_CPU2CR_PDDS_D1 | PWR_CPU2CR_PDDS_D3));
  }
  else
  {
    /* Enter DSTANDBY mode when Cortex-M4 enters DEEP-SLEEP */
    SET_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D2 | PWR_CPUCR_PDDS_D3));
    SET_BIT (PWR->CPU2CR, (PWR_CPU2CR_PDDS_D2 | PWR_CPU2CR_PDDS_D3));
  }
#else /* Single core devices */
  /* Enter DSTANDBY mode when Cortex-M7 enters DEEP-SLEEP */
  SET_BIT (PWR->CPUCR, (PWR_CPUCR_PDDS_D1 | PWR_CPUCR_PDDS_D3));

#if defined (PWR_CPUCR_PDDS_D2)
  /* Enter DSTANDBY mode when Cortex-M7 enters DEEP-SLEEP */
  SET_BIT (PWR->CPUCR, PWR_CPUCR_PDDS_D2);
#endif /* PWR_CPUCR_PDDS_D2 */
#endif /* defined (DUAL_CORE) */

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

  /* Ensure that all instructions are done before entering STOP mode */
  __DSB ();
  __ISB ();

  /* This option is used to ensure that store operations are completed */
#if defined (__CC_ARM)
  __force_stores();
#endif /* defined (__CC_ARM) */

  /* Request Wait For Interrupt */
  __WFI ();
}

/**
  * @brief  Indicate Sleep-On-Exit feature when returning from Handler mode to
  *         Thread mode.
  * @note   Set SLEEPONEXIT bit of SCR register. When this bit is set, the
  *         processor re-enters SLEEP mode when an interruption handling is over.
  *         Setting this bit is useful when the processor is expected to run
  *         only on interruptions handling.
  * @retval None.
  */
void HAL_PWR_EnableSleepOnExit (void)
{
  /* Set SLEEPONEXIT bit of Cortex-Mx System Control Register */
  SET_BIT (SCB->SCR, SCB_SCR_SLEEPONEXIT_Msk);
}

/**
  * @brief  Disable Sleep-On-Exit feature when returning from Handler mode to
  *         Thread mode.
  * @note   Clears SLEEPONEXIT bit of SCR register. When this bit is set, the
  *         processor re-enters SLEEP mode when an interruption handling is over.
  * @retval None
  */
void HAL_PWR_DisableSleepOnExit (void)
{
  /* Clear SLEEPONEXIT bit of Cortex-Mx System Control Register */
  CLEAR_BIT (SCB->SCR, SCB_SCR_SLEEPONEXIT_Msk);
}

/**
  * @brief  Enable CORTEX SEVONPEND feature.
  * @note   Sets SEVONPEND bit of SCR register. When this bit is set, any
  *         pending event / interrupt even if it's disabled or has insufficient
  *         priority to cause exception entry wakes up the Cortex-Mx.
  * @retval None.
  */
void HAL_PWR_EnableSEVOnPend (void)
{
  /* Set SEVONPEND bit of Cortex-Mx System Control Register */
  SET_BIT (SCB->SCR, SCB_SCR_SEVONPEND_Msk);
}

/**
  * @brief  Disable CORTEX SEVONPEND feature.
  * @note   Resets SEVONPEND bit of SCR register. When this bit is reset, only
  *         enabled pending causes exception entry wakes up the Cortex-Mx.
  * @retval None.
  */
void HAL_PWR_DisableSEVOnPend (void)
{
  /* Clear SEVONPEND bit of Cortex System Control Register */
  CLEAR_BIT (SCB->SCR, SCB_SCR_SEVONPEND_Msk);
}
/**
  * @}
  */

/** @defgroup PWR_Exported_Functions_Group3 Interrupt Handling Functions
  *  @brief   Interrupt Handling functions
  *
@verbatim
 ===============================================================================
                    ##### Interrupt Handling Functions #####
 ===============================================================================
    [..]
    This section provides functions allowing to handle the PVD pending
    interrupts.

@endverbatim
  * @{
  */

/**
  * @brief  This function handles the PWR PVD interrupt request.
  * @note   This API should be called under the PVD_AVD_IRQHandler().
  * @retval None.
  */
void HAL_PWR_PVD_IRQHandler (void)
{
#if defined (DUAL_CORE)
  /* Check Cortex-Mx ID */
  if (HAL_GetCurrentCPUID () == CM7_CPUID)
  {
    /* Check PWR EXTI D1 flag */
    if(__HAL_PWR_PVD_EXTI_GET_FLAG () != 0U)
    {
      /* Clear PWR EXTI D1 pending bit */
      __HAL_PWR_PVD_EXTI_CLEAR_FLAG ();

      /* PWR PVD interrupt user callback */
      HAL_PWR_PVDCallback ();
    }
  }
  else
  {
    /* Check PWR EXTI D2 flag */
    if (__HAL_PWR_PVD_EXTID2_GET_FLAG () != 0U)
    {
      /* Clear PWR EXTI D2 pending bit */
      __HAL_PWR_PVD_EXTID2_CLEAR_FLAG ();

      /* PWR PVD interrupt user callback */
      HAL_PWR_PVDCallback ();
    }
  }
#else /* Single core devices */
  /* PVD EXTI line interrupt detected */
  if (__HAL_PWR_PVD_EXTI_GET_FLAG () != 0U)
  {
    /* Clear PWR EXTI pending bit */
    __HAL_PWR_PVD_EXTI_CLEAR_FLAG ();

    /* PWR PVD interrupt user callback */
    HAL_PWR_PVDCallback ();
  }
#endif /* defined (DUAL_CORE) */
}

/**
  * @brief  PWR PVD interrupt callback.
  * @retval None.
  */
__weak void HAL_PWR_PVDCallback (void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PWR_PVDCallback can be implemented in the user file
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
