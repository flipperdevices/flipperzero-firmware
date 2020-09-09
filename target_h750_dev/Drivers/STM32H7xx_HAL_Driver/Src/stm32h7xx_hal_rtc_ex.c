/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rtc_ex.c
  * @author  MCD Application Team
  * @brief   Extended RTC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Real Time Clock (RTC) Extended peripheral:
  *           + RTC Time Stamp functions
  *           + RTC Tamper functions
  *           + RTC Wake-up functions
  *           + Extended Control functions
  *           + Extended RTC features functions
  *
  @verbatim
  ==============================================================================
                  ##### How to use this driver #####
  ==============================================================================
  [..]
    (+) Enable the RTC domain access.
    (+) Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour
        format using the HAL_RTC_Init() function.

  *** RTC Wakeup configuration ***
  ================================
  [..]
    (+) To configure the RTC Wakeup Clock source and Counter use the HAL_RTCEx_SetWakeUpTimer()
        function. You can also configure the RTC Wakeup timer with interrupt mode
        using the HAL_RTCEx_SetWakeUpTimer_IT() function.
    (+) To read the RTC WakeUp Counter register, use the HAL_RTCEx_GetWakeUpTimer()
        function.

  *** Outputs configuration ***
  =============================
  [..]  The RTC has 2 different outputs:
    (+) RTC_ALARM: this output is used to manage the RTC Alarm A, Alarm B
        and WaKeUp signals.
        To output the selected RTC signal, use the HAL_RTC_Init() function.
    (+) RTC_CALIB: this output is 512Hz signal or 1Hz.
        To enable the RTC_CALIB, use the HAL_RTCEx_SetCalibrationOutPut() function.
    (+) Two pins can be used as RTC_ALARM or RTC_CALIB (PC13, PB2) managed on
        the RTC_OR register.
    (+) When the RTC_CALIB or RTC_ALARM output is selected, the RTC_OUT pin is
        automatically configured in output alternate function.

  *** Smooth digital Calibration configuration ***
  ================================================
  [..]
    (+) Configure the RTC Original Digital Calibration Value and the corresponding
        calibration cycle period (32s,16s and 8s) using the HAL_RTCEx_SetSmoothCalib()
        function.

  *** TimeStamp configuration ***
  ===============================
  [..]
    (+) Enable the RTC TimeStamp using the HAL_RTCEx_SetTimeStamp() function.
        You can also configure the RTC TimeStamp with interrupt mode using the
        HAL_RTCEx_SetTimeStamp_IT() function.
    (+) To read the RTC TimeStamp Time and Date register, use the HAL_RTCEx_GetTimeStamp()
        function.

  *** Internal TimeStamp configuration ***
  ===============================
  [..]
    (+) Enable the RTC internal TimeStamp using the HAL_RTCEx_SetInternalTimeStamp() function.
        User has to check internal timestamp occurrence using __HAL_RTC_INTERNAL_TIMESTAMP_GET_FLAG.
    (+) To read the RTC TimeStamp Time and Date register, use the HAL_RTCEx_GetTimeStamp()
        function.

  *** Tamper configuration ***
  ============================
  [..]
    (+) Enable the RTC Tamper and configure the Tamper filter count, trigger Edge
        or Level according to the Tamper filter (if equal to 0 Edge else Level)
        value, sampling frequency, NoErase, MaskFlag,  precharge or discharge and
        Pull-UP using the HAL_RTCEx_SetTamper() function. You can configure RTC Tamper
        with interrupt mode using HAL_RTCEx_SetTamper_IT() function.
    (+) The default configuration of the Tamper erases the backup registers. To avoid
        erase, enable the NoErase field on the RTC_TAMPCR register.

  *** Backup Data Registers configuration ***
  ===========================================
  [..]
    (+) To write to the RTC Backup Data registers, use the HAL_RTCEx_BKUPWrite()
        function.
    (+) To read the RTC Backup Data registers, use the HAL_RTCEx_BKUPRead()
        function.

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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup RTCEx
  * @brief RTC Extended HAL module driver
  * @{
  */

#ifdef HAL_RTC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TAMP_ALL (TAMP_CR1_TAMP1E | TAMP_CR1_TAMP2E | TAMP_CR1_TAMP3E)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup RTCEx_Exported_Functions
  * @{
  */


/** @addtogroup RTCEx_Exported_Functions_Group1
 *  @brief   RTC TimeStamp and Tamper functions
  *
@verbatim
 ===============================================================================
                 ##### RTC TimeStamp and Tamper functions #####
 ===============================================================================

 [..] This section provides functions allowing to configure TimeStamp feature

@endverbatim
  * @{
  */

/**
  * @brief  Set TimeStamp.
  * @note   This API must be called before enabling the TimeStamp feature.
  * @param  hrtc RTC handle
  * @param  TimeStampEdge Specifies the pin edge on which the TimeStamp is
  *         activated.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPEDGE_RISING: the Time stamp event occurs on the
  *                                        rising edge of the related pin.
  *             @arg RTC_TIMESTAMPEDGE_FALLING: the Time stamp event occurs on the
  *                                         falling edge of the related pin.
  * @param  RTC_TimeStampPin specifies the RTC TimeStamp Pin.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPPIN_DEFAULT: PC13 is selected as RTC TimeStamp Pin.
  *               The RTC TimeStamp Pin is per default PC13, but for reasons of
  *               compatibility, this parameter is required.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_TIMESTAMP_EDGE(TimeStampEdge));
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(RTC_TimeStampPin);

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));

  tmpreg |= TimeStampEdge;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;

  __HAL_RTC_TIMESTAMP_ENABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Set TimeStamp with Interrupt.
  * @note   This API must be called before enabling the TimeStamp feature.
  * @param  hrtc RTC handle
  * @param  TimeStampEdge Specifies the pin edge on which the TimeStamp is
  *         activated.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPEDGE_RISING: the Time stamp event occurs on the
  *                                        rising edge of the related pin.
  *             @arg RTC_TIMESTAMPEDGE_FALLING: the Time stamp event occurs on the
  *                                         falling edge of the related pin.
  * @param  RTC_TimeStampPin Specifies the RTC TimeStamp Pin.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPPIN_DEFAULT: PC13 is selected as RTC TimeStamp Pin.
  *               The RTC TimeStamp Pin is per default PC13, but for reasons of
  *               compatibility, this parameter is required.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_TIMESTAMP_EDGE(TimeStampEdge));
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));

  /* Prevent unused argument(s) compilation warning if no assert_param check */
  UNUSED(RTC_TimeStampPin);

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));

  tmpreg |= TimeStampEdge;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;

  __HAL_RTC_TIMESTAMP_ENABLE(hrtc);

  /* Enable IT timestamp */
  __HAL_RTC_TIMESTAMP_ENABLE_IT(hrtc, RTC_IT_TS);

#if !defined(DUAL_CORE)
  /* RTC timestamp Interrupt Configuration: EXTI configuration */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT();
#endif

  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_EDGE();

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Deactivate TimeStamp.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc)
{
  uint32_t tmpreg;

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* In case of interrupt mode is used, the interrupt source must disabled */
  __HAL_RTC_TIMESTAMP_DISABLE_IT(hrtc, RTC_IT_TS);

  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));

  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Set Internal TimeStamp.
  * @note   This API must be called before enabling the internal TimeStamp feature.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetInternalTimeStamp(RTC_HandleTypeDef *hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Configure the internal Time Stamp Enable bits */
  __HAL_RTC_INTERNAL_TIMESTAMP_ENABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Deactivate Internal TimeStamp.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTimeStamp(RTC_HandleTypeDef *hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Configure the internal Time Stamp Enable bits */
  __HAL_RTC_INTERNAL_TIMESTAMP_DISABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Get the RTC TimeStamp value.
  * @param  hrtc RTC handle
  * @param  sTimeStamp Pointer to Time structure
  * @param  sTimeStampDate Pointer to Date structure
  * @param  Format specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *             @arg RTC_FORMAT_BIN: Binary data format
  *             @arg RTC_FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTimeStamp, RTC_DateTypeDef *sTimeStampDate, uint32_t Format)
{
  uint32_t tmptime;
  uint32_t tmpdate;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

  /* Get the TimeStamp time and date registers values */
  tmptime = (uint32_t)(hrtc->Instance->TSTR & RTC_TR_RESERVED_MASK);
  tmpdate = (uint32_t)(hrtc->Instance->TSDR & RTC_DR_RESERVED_MASK);

  /* Fill the Time structure fields with the read parameters */
  sTimeStamp->Hours      = (uint8_t)((tmptime & (RTC_TSTR_HT  | RTC_TSTR_HU))  >> RTC_TSTR_HU_Pos);
  sTimeStamp->Minutes    = (uint8_t)((tmptime & (RTC_TSTR_MNT | RTC_TSTR_MNU)) >> RTC_TSTR_MNU_Pos);
  sTimeStamp->Seconds    = (uint8_t)((tmptime & (RTC_TSTR_ST  | RTC_TSTR_SU))  >> RTC_TSTR_SU_Pos);
  sTimeStamp->TimeFormat = (uint8_t)((tmptime & (RTC_TSTR_PM))                 >> RTC_TSTR_PM_Pos);
  sTimeStamp->SubSeconds = (uint32_t) hrtc->Instance->TSSSR;

  /* Fill the Date structure fields with the read parameters */
  sTimeStampDate->Year    = 0U;
  sTimeStampDate->Month   = (uint8_t)((tmpdate & (RTC_TSDR_MT | RTC_TSDR_MU)) >> RTC_TSDR_MU_Pos);
  sTimeStampDate->Date    = (uint8_t)((tmpdate & (RTC_TSDR_DT | RTC_TSDR_DU)) >> RTC_TSDR_DU_Pos);
  sTimeStampDate->WeekDay = (uint8_t)((tmpdate & (RTC_TSDR_WDU))              >> RTC_TSDR_WDU_Pos);

  /* Check the input parameters format */
  if (Format == RTC_FORMAT_BIN)
  {
    /* Convert the TimeStamp structure parameters to Binary format */
    sTimeStamp->Hours   = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Hours);
    sTimeStamp->Minutes = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Minutes);
    sTimeStamp->Seconds = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Seconds);

    /* Convert the DateTimeStamp structure parameters to Binary format */
    sTimeStampDate->Month   = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->Month);
    sTimeStampDate->Date    = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->Date);
    sTimeStampDate->WeekDay = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->WeekDay);
  }

  /* Clear the TIMESTAMP Flags */
  __HAL_RTC_INTERNAL_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_ITSF);
  __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_TSF);

  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group5
  * @brief      Extended RTC Tamper functions
  *
@verbatim
 ==============================================================================
                        ##### Tamper functions #####
 ==============================================================================
 [..]
  (+) Before calling any tamper or internal tamper function, you have to call first
      HAL_RTC_Init() function.
  (+) In that ine you can select to output tamper event on RTC pin.
 [..]
  (+) Enable the Tamper and configure the Tamper filter count, trigger Edge
      or Level according to the Tamper filter (if equal to 0 Edge else Level)
      value, sampling frequency, NoErase, MaskFlag, precharge or discharge and
      Pull-UP, timestamp using the HAL_RTCEx_SetTamper() function.
      You can configure Tamper with interrupt mode using HAL_RTCEx_SetTamper_IT() function.
  (+) The default configuration of the Tamper erases the backup registers. To avoid
      erase, enable the NoErase field on the TAMP_TAMPCR register.
 [..]
  (+) Enable Internal Tamper and configure it with interrupt, timestamp using
      the HAL_RTCEx_SetInternalTamper() function.

@endverbatim
* @{
*/

#if defined(TAMP_CR1_TAMP1E)
/**
  * @brief  Set Tamper
  * @param  hrtc RTC handle
  * @param  sTamper Pointer to Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef * hrtc, RTC_TamperTypeDef * sTamper)
{
  uint32_t tmpreg;

  /* Point on TAMPER registers base address */
  TAMP_TypeDef *tamp = (TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET);

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER(sTamper->Tamper));
  assert_param(IS_RTC_TAMPER_TRIGGER(sTamper->Trigger));
  assert_param(IS_RTC_TAMPER_ERASE_MODE(sTamper->NoErase));
  assert_param(IS_RTC_TAMPER_MASKFLAG_STATE(sTamper->MaskFlag));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sTamper->TimeStampOnTamperDetection));
  assert_param(IS_RTC_TAMPER_FILTER(sTamper->Filter));
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(sTamper->SamplingFrequency));
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(sTamper->PrechargeDuration));
  assert_param(IS_RTC_TAMPER_PULLUP_STATE(sTamper->TamperPullUp));
  assert_param(IS_RTC_TAMPER_FILTER_CONFIG_CORRECT(sTamper->Filter, sTamper->Trigger));

  /* Configuration register 2 */
  tmpreg = tamp->CR2;
  tmpreg &= ~((sTamper->Tamper << TAMP_CR2_TAMP1TRG_Pos) | (sTamper->Tamper << TAMP_CR2_TAMP1MSK_Pos) | (sTamper->Tamper << TAMP_CR2_TAMP1NOERASE_Pos));

  /* Configure the tamper trigger bit */
  if ((sTamper->Trigger == RTC_TAMPERTRIGGER_HIGHLEVEL) || (sTamper->Trigger == RTC_TAMPERTRIGGER_FALLINGEDGE))
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1TRG_Pos);
  }

  /* Configure the tamper flags masking bit */
  if (sTamper->MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE)
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1MSK_Pos);
  }

  /* Configure the tamper backup registers erasure bit */
  if (sTamper->NoErase != RTC_TAMPER_ERASE_BACKUP_ENABLE)
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1NOERASE_Pos);
  }
  tamp->CR2 = tmpreg;

  /* Configure filtering parameters */
  tamp->FLTCR = (sTamper->Filter)            | (sTamper->SamplingFrequency) | \
                (sTamper->PrechargeDuration) | (sTamper->TamperPullUp);

  /* Configure Timestamp saving on tamper detection */
  if ((hrtc->Instance->CR & RTC_CR_TAMPTS) != (sTamper->TimeStampOnTamperDetection))
  {
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
    tmpreg = (hrtc->Instance->CR & ~RTC_CR_TAMPTS);
    hrtc->Instance->CR = (tmpreg | (sTamper->TimeStampOnTamperDetection));
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  }

  /* Enable selected tamper */
  tamp->CR1 |= (sTamper->Tamper);

  return HAL_OK;
}
#endif /* TAMP_CR1_TAMP1E */
#if defined (RTC_TAMPCR_TAMP1E)
/**
  * @brief  Set Tamper.
  * @note   By calling this API we disable the tamper interrupt for all tampers.
  * @param  hrtc RTC handle
  * @param  sTamper Pointer to Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef * hrtc, RTC_TamperTypeDef * sTamper)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER(sTamper->Tamper));
  assert_param(IS_RTC_TAMPER_TRIGGER(sTamper->Trigger));
  assert_param(IS_RTC_TAMPER_ERASE_MODE(sTamper->NoErase));
  assert_param(IS_RTC_TAMPER_MASKFLAG_STATE(sTamper->MaskFlag));
  assert_param(IS_RTC_TAMPER_FILTER(sTamper->Filter));
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(sTamper->SamplingFrequency));
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(sTamper->PrechargeDuration));
  assert_param(IS_RTC_TAMPER_PULLUP_STATE(sTamper->TamperPullUp));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sTamper->TimeStampOnTamperDetection));
  assert_param(IS_RTC_TAMPER_FILTER_CONFIG_CORRECT(sTamper->Filter, sTamper->Trigger));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Copy control register into temporary variable */
  tmpreg = hrtc->Instance->TAMPCR;

  /* Enable selected tamper */
  tmpreg |= (sTamper->Tamper);

  /* Configure the bit (located just next to the tamper enable bit) */
  if ((sTamper->Trigger == RTC_TAMPERTRIGGER_HIGHLEVEL) || (sTamper->Trigger == RTC_TAMPERTRIGGER_FALLINGEDGE))
  {
    /* Set the tamper trigger bit */
    tmpreg |= (uint32_t)(sTamper->Tamper << 1U);
  }
  else
  {
    /* Clear the tamper trigger bit */
    tmpreg &= (uint32_t)~(sTamper->Tamper << 1U);
  }

  /* Configure the tamper backup registers erasure bit */
  if (sTamper->NoErase != RTC_TAMPER_ERASE_BACKUP_ENABLE)
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP1NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP2NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP3NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }
  else
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP1NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP2NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP3NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }

  /* Configure the tamper flags masking bit */
  if (sTamper->MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE)
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP1MF);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP2MF);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP3MF);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }
  else
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP1MF);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP2MF);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP3MF);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }

  /* Clearing remaining fields before setting them */
  tmpreg &= ~(RTC_TAMPERFILTER_MASK            | RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK | \
              RTC_TAMPERPRECHARGEDURATION_MASK | RTC_TAMPER_PULLUP_MASK             | \
              RTC_TIMESTAMPONTAMPERDETECTION_MASK);

  /* Set remaining parameters of desired configuration into temporary variable */
  tmpreg |= ((uint32_t)sTamper->Filter            | \
             (uint32_t)sTamper->SamplingFrequency | \
             (uint32_t)sTamper->PrechargeDuration | \
             (uint32_t)sTamper->TamperPullUp      | \
             (uint32_t)sTamper->TimeStampOnTamperDetection);

  /* Copy desired configuration into configuration register */
  hrtc->Instance->TAMPCR = tmpreg;

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}
#endif /* RTC_TAMPCR_TAMP1E */

#if defined(TAMP_CR1_TAMP1E)
/**
  * @brief  Set Tamper with interrupt.
  * @param  hrtc RTC handle
  * @param  sTamper Pointer to Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef * hrtc, RTC_TamperTypeDef * sTamper)
{
  uint32_t tmpreg;

  /* Point on TAMPER registers base address */
  TAMP_TypeDef *tamp = (TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET);

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER(sTamper->Tamper));
  assert_param(IS_RTC_TAMPER_TRIGGER(sTamper->Trigger));
  assert_param(IS_RTC_TAMPER_ERASE_MODE(sTamper->NoErase));
  assert_param(IS_RTC_TAMPER_MASKFLAG_STATE(sTamper->MaskFlag));
  assert_param(IS_RTC_TAMPER_FILTER(sTamper->Filter));
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(sTamper->SamplingFrequency));
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(sTamper->PrechargeDuration));
  assert_param(IS_RTC_TAMPER_PULLUP_STATE(sTamper->TamperPullUp));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sTamper->TimeStampOnTamperDetection));
  assert_param(IS_RTC_TAMPER_FILTER_CONFIG_CORRECT(sTamper->Filter, sTamper->Trigger));

  /* Copy configuration register into temporary variable */
  tmpreg = tamp->CR2;

  /* Clear the bits that are going to be configured and leave the others unchanged */
  tmpreg &= ~((sTamper->Tamper << TAMP_CR2_TAMP1TRG_Pos) | (sTamper->Tamper << TAMP_CR2_TAMP1MSK_Pos) | (sTamper->Tamper << TAMP_CR2_TAMP1NOERASE_Pos));

  /* Configure the tamper trigger bit */
  if ((sTamper->Trigger == RTC_TAMPERTRIGGER_HIGHLEVEL) || (sTamper->Trigger == RTC_TAMPERTRIGGER_FALLINGEDGE))
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1TRG_Pos);
  }

  /* Configure the tamper flags masking bit */
  if (sTamper->MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE)
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1MSK_Pos);
  }

  /* Configure the tamper backup registers erasure bit */
  if (sTamper->NoErase != RTC_TAMPER_ERASE_BACKUP_ENABLE)
  {
    tmpreg |= (sTamper->Tamper << TAMP_CR2_TAMP1NOERASE_Pos);
  }
  tamp->CR2 = tmpreg;

  /* Configure filtering parameters */
  tamp->FLTCR = (sTamper->Filter)            | (sTamper->SamplingFrequency) | \
                (sTamper->PrechargeDuration) | (sTamper->TamperPullUp);

  /* Configure Timestamp saving on tamper detection */
  if ((hrtc->Instance->CR & RTC_CR_TAMPTS) != (sTamper->TimeStampOnTamperDetection))
  {
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
    tmpreg = (hrtc->Instance->CR & ~RTC_CR_TAMPTS);
    hrtc->Instance->CR = (tmpreg | (sTamper->TimeStampOnTamperDetection));
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  }

  /* Configure RTC Tamper Interrupt: EXTI configuration */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT();
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_FALLING_EDGE();

  /* Enable interrupt on selected tamper */
  tamp->IER |= sTamper->Tamper;

  /* Enable selected tamper */
  tamp->CR1 |= sTamper->Tamper;

  return HAL_OK;
}
#endif /* TAMP_CR1_TAMP1E */
#if defined(RTC_TAMPCR_TAMP1E)
/**
  * @brief  Set Tamper with interrupt.
  * @note   By calling this API we force the tamper interrupt for all tampers.
  * @param  hrtc RTC handle
  * @param  sTamper Pointer to Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef * hrtc, RTC_TamperTypeDef * sTamper)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER(sTamper->Tamper));
  assert_param(IS_RTC_TAMPER_INTERRUPT(sTamper->Interrupt));
  assert_param(IS_RTC_TAMPER_TRIGGER(sTamper->Trigger));
  assert_param(IS_RTC_TAMPER_ERASE_MODE(sTamper->NoErase));
  assert_param(IS_RTC_TAMPER_MASKFLAG_STATE(sTamper->MaskFlag));
  assert_param(IS_RTC_TAMPER_FILTER(sTamper->Filter));
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(sTamper->SamplingFrequency));
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(sTamper->PrechargeDuration));
  assert_param(IS_RTC_TAMPER_PULLUP_STATE(sTamper->TamperPullUp));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sTamper->TimeStampOnTamperDetection));
  assert_param(IS_RTC_TAMPER_FILTER_CONFIG_CORRECT(sTamper->Filter, sTamper->Trigger));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Copy control register into temporary variable */
  tmpreg = hrtc->Instance->TAMPCR;

  /* Enable selected tamper */
  tmpreg |= (sTamper->Tamper);

  /* Configure the tamper trigger bit (located just next to the tamper enable bit) */
  if ((sTamper->Trigger == RTC_TAMPERTRIGGER_HIGHLEVEL) || (sTamper->Trigger == RTC_TAMPERTRIGGER_FALLINGEDGE))
  {
    /* Set the tamper trigger bit */
    tmpreg |= (uint32_t)(sTamper->Tamper << 1U);
  }
  else
  {
    /* Clear the tamper trigger bit */
    tmpreg &= (uint32_t)~(sTamper->Tamper << 1U);
  }

  /* Configure the tamper backup registers erasure bit */
  if (sTamper->NoErase != RTC_TAMPER_ERASE_BACKUP_ENABLE)
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP1NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP2NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP3NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }
  else
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP1NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP2NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP3NOERASE);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }

  /* Configure the tamper flags masking bit */
  if (sTamper->MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE)
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP1MF);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP2MF);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg |= (uint32_t)(RTC_TAMPCR_TAMP3MF);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }
  else
  {
#if defined(RTC_TAMPCR_TAMP1E)
    if ((sTamper->Tamper & RTC_TAMPER_1) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP1MF);
    }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
    if ((sTamper->Tamper & RTC_TAMPER_2) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP2MF);
    }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
    if ((sTamper->Tamper & RTC_TAMPER_3) != 0U)
    {
      tmpreg &= (uint32_t)~(RTC_TAMPCR_TAMP3MF);
    }
#endif /* RTC_TAMPCR_TAMP3E */
  }

  /* Clearing remaining fields before setting them */
  tmpreg &= ~(RTC_TAMPERFILTER_MASK            | RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK | \
              RTC_TAMPERPRECHARGEDURATION_MASK | RTC_TAMPER_PULLUP_MASK             | \
              RTC_TIMESTAMPONTAMPERDETECTION_MASK);

  /* Set remaining parameters of desired configuration into temporary variable */
  tmpreg |= ((uint32_t)sTamper->Filter            | \
             (uint32_t)sTamper->SamplingFrequency | \
             (uint32_t)sTamper->PrechargeDuration | \
             (uint32_t)sTamper->TamperPullUp      | \
             (uint32_t)sTamper->TimeStampOnTamperDetection);

  /* Enable interrupt on selected tamper */
  tmpreg |= (uint32_t)sTamper->Interrupt;

  /* Copy desired configuration into configuration register */
  hrtc->Instance->TAMPCR = tmpreg;

#if !defined(DUAL_CORE)
  /* RTC Tamper Interrupt Configuration: EXTI configuration */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT();
#endif

  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_EDGE();

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}
#endif /* RTC_TAMPCR_TAMP1E */

#if defined(TAMP_CR1_TAMP1E)
/**
  * @brief  Deactivate Tamper.
  * @param  hrtc RTC handle
  * @param  Tamper Selected tamper pin.
  *         This parameter can be a combination of the following values:
  *         @arg RTC_TAMPER_1
  *         @arg RTC_TAMPER_2
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef * hrtc, uint32_t Tamper)
{
  /* Point on TAMPER registers base address */
  TAMP_TypeDef *tamp = (TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET);

  assert_param(IS_RTC_TAMPER(Tamper));

  /* Disable the selected Tamper pin */
  tamp->CR1 &= ~Tamper;

  /* Disable the selected Tamper interrupt */
  tamp->IER &= ~Tamper;

  /* Clear the selected tamper flags in SR register by setting corresponding bits in SCR register */
  tamp->SCR = Tamper;

  /* Clear the selected tamper configuration (trigger, mask flag, and no-erase) */
  tamp->CR2 &= ~((Tamper << TAMP_CR2_TAMP1TRG_Pos) | (Tamper << TAMP_CR2_TAMP1MSK_Pos) | (Tamper << TAMP_CR2_TAMP1NOERASE_Pos));

  return HAL_OK;
}
#endif /* TAMP_CR1_TAMP1E */
#if defined(RTC_TAMPCR_TAMP1E)
/**
  * @brief  Deactivate Tamper.
  * @param  hrtc RTC handle
  * @param  Tamper Selected tamper pin.
  *         This parameter can be any combination of the following values:
  *         @arg RTC_TAMPER_1
  *         @arg RTC_TAMPER_2
  *         @arg RTC_TAMPER_3
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef * hrtc, uint32_t Tamper)
{
  assert_param(IS_RTC_TAMPER(Tamper));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the selected Tamper pin */
  hrtc->Instance->TAMPCR &= ((uint32_t)~Tamper);

  /* Disable the selected Tamper interrupt */
#if defined(RTC_TAMPCR_TAMP1E)
  if ((Tamper & RTC_TAMPER_1) != 0U)
  {
    hrtc->Instance->TAMPCR &= ((uint32_t)~(RTC_IT_TAMP | RTC_IT_TAMP1));
  }
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
  if ((Tamper & RTC_TAMPER_2) != 0U)
  {
    hrtc->Instance->TAMPCR &= ((uint32_t)~(RTC_IT_TAMP | RTC_IT_TAMP2));
  }
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
  if ((Tamper & RTC_TAMPER_3) != 0U)
  {
    hrtc->Instance->TAMPCR &= ((uint32_t)~(RTC_IT_TAMP | RTC_IT_TAMP3));
  }
#endif /* RTC_TAMPCR_TAMP3E */

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}
#endif /* RTC_TAMPCR_TAMP1E */

#if defined(TAMP_CR1_ITAMP1E)
/**
  * @brief  Set Internal Tamper
  * @param  hrtc RTC handle
  * @param  sIntTamper Pointer to Internal Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper(RTC_HandleTypeDef *hrtc, RTC_InternalTamperTypeDef *sIntTamper)
{
  /* Check the parameters */
  assert_param(IS_RTC_INTERNAL_TAMPER(sIntTamper->IntTamper));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sIntTamper->TimeStampOnTamperDetection));

  /* Time-Stamp on internal tamper */
  if (READ_BIT(RTC->CR, RTC_CR_TAMPTS) != sIntTamper->TimeStampOnTamperDetection)
  {
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
    MODIFY_REG(RTC->CR, RTC_CR_TAMPTS, sIntTamper->TimeStampOnTamperDetection);
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  }

  /* Control register 1 */
  SET_BIT(TAMP->CR1, sIntTamper->IntTamper);

  return HAL_OK;
}

/**
  * @brief  Set Internal Tamper in interrupt mode
  * @param  hrtc RTC handle
  * @param  sIntTamper Pointer to Internal Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper_IT(RTC_HandleTypeDef *hrtc, RTC_InternalTamperTypeDef *sIntTamper)
{
  /* Check the parameters */
  assert_param(IS_RTC_INTERNAL_TAMPER(sIntTamper->IntTamper));
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sIntTamper->TimeStampOnTamperDetection));

  /* Time-stamp on internal tamper */
  if (READ_BIT(RTC->CR, RTC_CR_TAMPTS) != sIntTamper->TimeStampOnTamperDetection)
  {
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
    MODIFY_REG(RTC->CR, RTC_CR_TAMPTS, sIntTamper->TimeStampOnTamperDetection);
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  }

  /* RTC Tamper Interrupt Configuration: EXTI configuration */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT();
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_FALLING_EDGE();
  /* Interrupt enable register */
  SET_BIT(TAMP->IER, sIntTamper->IntTamper);

  /* Control register 1 */
  SET_BIT(TAMP->CR1, sIntTamper->IntTamper);

  return HAL_OK;
}

/**
  * @brief  Deactivate Internal Tamper.
  * @param  hrtc RTC handle
  * @param  IntTamper Selected internal tamper event.
  *          This parameter can be any combination of existing internal tampers.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTamper(RTC_HandleTypeDef *hrtc, uint32_t IntTamper)
{
  UNUSED(hrtc);
  assert_param(IS_RTC_INTERNAL_TAMPER(IntTamper));

  /* Disable the selected Tamper pin */
  CLEAR_BIT(TAMP->CR1, IntTamper);

  /* Clear internal tamper interrupt mode configuration */
  CLEAR_BIT(TAMP->IER, IntTamper);

  /* Clear internal tamper interrupt */
  WRITE_REG(TAMP->SCR, IntTamper);

  return HAL_OK;
}
#endif /* TAMP_CR1_ITAMP1E */

#if defined(TAMP_ATCR1_TAMP1AM)
/**
  * @brief  Set all active Tampers at the same time.
  * @param  hrtc RTC handle
  * @param  sAllTamper Pointer to active Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetActiveTampers(RTC_HandleTypeDef *hrtc, RTC_ActiveTampersTypeDef *sAllTamper)
{
  uint32_t IER, CR1, CR2, ATCR1, CR, i, tickstart;

#ifdef  USE_FULL_ASSERT
  for (i = 0; i < RTC_TAMP_NB; i++)
  {
    assert_param(IS_RTC_TAMPER_ERASE_MODE(sAllTamper->TampInput[i].NoErase));
    assert_param(IS_RTC_TAMPER_MASKFLAG_STATE(sAllTamper->TampInput[i].MaskFlag));
    /* Mask flag only supported by TAMPER 1, 2 and 3 */
    assert_param(!((sAllTamper->TampInput[i].MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE) && (i > RTC_TAMPER_3)));
  }
  assert_param(IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(sAllTamper->TimeStampOnTamperDetection));
#endif /* #ifdef  USE_FULL_ASSERT */

  /* Active Tampers must not be already enabled */
  if (READ_BIT(TAMP->ATOR, TAMP_ATOR_INITS) != 0U)
  {
    /* Disable all actives tampers with HAL_RTCEx_DeactivateActiveTampers and try again */
    return HAL_ERROR;
  }

  /* Set TimeStamp on tamper detection */
  CR = READ_REG(RTC->CR);
  if ((CR & RTC_CR_TAMPTS) != (sAllTamper->TimeStampOnTamperDetection))
  {
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
    MODIFY_REG(RTC->CR, RTC_CR_TAMPTS, sAllTamper->TimeStampOnTamperDetection);
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  }

  CR1 = READ_REG(TAMP->CR1);
  CR2 = READ_REG(TAMP->CR2);
  IER = READ_REG(TAMP->IER);

  /* Set common parameters */
  ATCR1 = (sAllTamper->ActiveFilter | (sAllTamper->ActiveOutputChangePeriod << TAMP_ATCR1_ATPER_Pos) | sAllTamper->ActiveAsyncPrescaler);

  /* Set specific parameters for each active tamper inputs if enable */
  for (i = 0; i < RTC_TAMP_NB; i++)
  {
    if (sAllTamper->TampInput[i].Enable != RTC_ATAMP_DISABLE)
    {
      CR1 |= (TAMP_CR1_TAMP1E << i);
      ATCR1 |= (TAMP_ATCR1_TAMP1AM << i);

      if (sAllTamper->TampInput[i].Interrupt != RTC_ATAMP_INTERRUPT_DISABLE)
      {
        /* RTC Tamper Interrupt Configuration: EXTI configuration */
        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT();
        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_EDGE();

        /* Interrupt enable register */
        IER |= (TAMP_IER_TAMP1IE << i);
      }

      if (sAllTamper->TampInput[i].MaskFlag != RTC_TAMPERMASK_FLAG_DISABLE)
      {
        CR2 |= (TAMP_CR2_TAMP1MSK << i);
      }

      if (sAllTamper->TampInput[i].NoErase != RTC_TAMPER_ERASE_BACKUP_ENABLE)
      {
        CR2 |= (TAMP_CR2_TAMP1NOERASE << i);
      }

      /* Set ATOSHARE and configure ATOSELx[] in case of output sharing */
      if (sAllTamper->TampInput[i].Output != i)
      {
        ATCR1 |= TAMP_ATCR1_ATOSHARE;
        ATCR1 |= sAllTamper->TampInput[i].Output << ((2u * i) + TAMP_ATCR1_ATOSEL1_Pos);
      }
    }
  }

  WRITE_REG(TAMP->IER, IER);
  WRITE_REG(TAMP->IER, IER);
  WRITE_REG(TAMP->ATCR1, ATCR1);
#if defined(TAMP_ATCR2_ATOSEL1)
  WRITE_REG(TAMP->ATCR2, ATCR2);
#endif /* TAMP_ATCR2_ATOSEL1 */
  WRITE_REG(TAMP->CR2, CR2);
  WRITE_REG(TAMP->CR1, CR1);

  /* Write seed */
  for (i = 0; i < RTC_ATAMP_SEED_NB_UINT32; i++)
  {
    WRITE_REG(TAMP->ATSEEDR, sAllTamper->Seed[i]);
  }

  /* Wait till RTC SEEDF flag is set and if Time out is reached exit */
  tickstart = HAL_GetTick();
  while (READ_BIT(TAMP->ATOR,  TAMP_ATOR_SEEDF) != 0u)
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      hrtc->State = HAL_RTC_STATE_TIMEOUT;
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}
#endif /* TAMP_ATCR1_TAMP1AM */

#if defined(TAMP_ATSEEDR_SEED)
/**
  * @brief  Write a new seed. Active tamper must be enabled.
  * @param  hrtc RTC handle
  * @param  pSeed Pointer to active tamper seed values.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetActiveSeed(RTC_HandleTypeDef *hrtc, uint32_t *pSeed)
{
  uint32_t i, tickstart;

  /* Active Tampers must be enabled */
  if (READ_BIT(TAMP->ATOR,  TAMP_ATOR_INITS) == 0U)
  {
    return HAL_ERROR;
  }

  for (i = 0; i < RTC_ATAMP_SEED_NB_UINT32; i++)
  {
    WRITE_REG(TAMP->ATSEEDR, pSeed[i]);
  }

  /* Wait till RTC SEEDF flag is set and if Time out is reached exit */
  tickstart = HAL_GetTick();
  while (READ_BIT(TAMP->ATOR,  TAMP_ATOR_SEEDF) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      hrtc->State = HAL_RTC_STATE_TIMEOUT;
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}
#endif /* TAMP_ATSEEDR_SEED */

#if defined(TAMP_ATCR1_TAMP1AM)
/**
  * @brief  Deactivate all Active Tampers at the same time.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateActiveTampers(RTC_HandleTypeDef *hrtc)
{
  /* Get Active tampers */
  uint32_t ATamp_mask = READ_BIT(TAMP->ATCR1, TAMP_ALL);

  UNUSED(hrtc);
  /* Disable all actives tampers but not passives tampers */
  CLEAR_BIT(TAMP->CR1, ATamp_mask);
  /* Disable no erase and mask */
  CLEAR_BIT(TAMP->CR2, (ATamp_mask | ((ATamp_mask & (TAMP_ATCR1_TAMP1AM | TAMP_ATCR1_TAMP2AM | TAMP_ATCR1_TAMP3AM)) << TAMP_CR2_TAMP1MSK_Pos)));

  /* Clear tamper interrupt and event flags (WO register) of all actives tampers but not passives tampers */
  WRITE_REG(TAMP->SCR, ATamp_mask);

  /* Clear all active tampers interrupt mode configuration but not passives tampers */
  CLEAR_BIT(TAMP->IER, ATamp_mask);

  CLEAR_BIT(TAMP->ATCR1, TAMP_ALL | TAMP_ATCR1_ATCKSEL | TAMP_ATCR1_ATPER | \
            TAMP_ATCR1_ATOSHARE | TAMP_ATCR1_FLTEN);

#if defined(TAMP_ATCR2_ATOSEL1)
  CLEAR_BIT(TAMP->ATCR2, TAMP_ATCR2_ATOSEL1 | TAMP_ATCR2_ATOSEL2 | TAMP_ATCR2_ATOSEL3 | TAMP_ATCR2_ATOSEL4 |
            TAMP_ATCR2_ATOSEL5 | TAMP_ATCR2_ATOSEL6 | TAMP_ATCR2_ATOSEL7 | TAMP_ATCR2_ATOSEL8);
#endif /* TAMP_ATCR2_ATOSEL1 */

  return HAL_OK;
}
#endif /* TAMP_ATCR1_TAMP1AM */

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group1
 *  @brief   RTC TimeStamp and Tamper functions
 *
* @{
*/

/**
  * @brief  Handle TimeStamp interrupt request.
  * @param  hrtc RTC handle
  * @retval None
  */
#if defined(RTC_MISR_TSMF)
void HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc)
{

  /* Point on TAMPER registers base address */
  TAMP_TypeDef *tamp = (TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET);

  /* Clear the EXTI's Flag for RTC TimeStamp and Tamper */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG();

  if ((hrtc->Instance->MISR & RTC_MISR_TSMF) != 0u)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call TimeStampEvent registered Callback */
    hrtc->TimeStampEventCallback(hrtc);
#else
    HAL_RTCEx_TimeStampEventCallback(hrtc);
#endif
    /* Not immediately clear flags because the content of RTC_TSTR and RTC_TSDR are cleared when TSF bit is reset.*/
    hrtc->Instance->SCR = RTC_SCR_CTSF;
  }

  /* Get interrupt status */
  uint32_t tmp = tamp->MISR;

  /* Immediately clear flags */
  tamp->SCR = tmp;

#if defined(TAMP_CR1_TAMP1E)
  /* Check Tamper1 status */
  if ((tmp & RTC_TAMPER_1) == RTC_TAMPER_1)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Tamper 1 Event registered Callback */
    hrtc->Tamper1EventCallback(hrtc);
#else
    /* Tamper1 callback */
    HAL_RTCEx_Tamper1EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_TAMP1E */

#if defined(TAMP_CR1_TAMP2E)
  /* Check Tamper2 status */
  if ((tmp & RTC_TAMPER_2) == RTC_TAMPER_2)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Tamper 2 Event registered Callback */
    hrtc->Tamper2EventCallback(hrtc);
#else
    /* Tamper2 callback */
    HAL_RTCEx_Tamper2EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_TAMP2E */

#if defined(TAMP_CR1_TAMP3E)
  /* Check Tamper3 status */
  if ((tmp & RTC_TAMPER_3) == RTC_TAMPER_3)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Tamper 3 Event registered Callback */
    hrtc->Tamper3EventCallback(hrtc);
#else
    /* Tamper3 callback */
    HAL_RTCEx_Tamper3EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_TAMP3E */

#if defined(TAMP_CR1_ITAMP1E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_1) == RTC_INT_TAMPER_1)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper1EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper1EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP1E */

#if defined(TAMP_CR1_ITAMP2E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_2) == RTC_INT_TAMPER_2)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper2EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper2EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP2E */

#if defined(TAMP_CR1_ITAMP3E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_3) == RTC_INT_TAMPER_3)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper3EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper3EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP3E */

#if defined(TAMP_CR1_ITAMP4E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_4) == RTC_INT_TAMPER_4)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper4EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper4EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP4E */

#if defined(TAMP_CR1_ITAMP5E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_5) == RTC_INT_TAMPER_5)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper5EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper5EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP5E */

#if defined(TAMP_CR1_ITAMP6E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_6) == RTC_INT_TAMPER_6)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper6EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper6EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP6E */

#if defined(TAMP_CR1_ITAMP8E)
  /* Check Internal Tamper status */
  if ((tmp & RTC_INT_TAMPER_8) == RTC_INT_TAMPER_8)
  {
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call Internal Tamper Event registered callback */
    hrtc->InternalTamper8EventCallback(hrtc);
#else
    /* Call Internal Tamper Event by-default callback */
    HAL_RTCEx_InternalTamper8EventCallback(hrtc);
#endif
  }
#endif /* TAMP_CR1_ITAMP8E */

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
}
#endif /* RTC_MISR_TSMF */
#if defined(RTC_ISR_TSF)
void HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc)
{
  /* Clear the EXTI's Flag for RTC TimeStamp and Tamper */
#if defined(DUAL_CORE)
  if (HAL_GetCurrentCPUID() == CM7_CPUID)
  {
    __HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG();
  }
  else
  {
    __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_CLEAR_FLAG();
  }
#else  /* SINGLE_CORE */
  __HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG();
#endif /* DUAL_CORE */

  /* Get the TimeStamp interrupt source enable status */
  if (__HAL_RTC_TIMESTAMP_GET_IT_SOURCE(hrtc, RTC_IT_TS) != 0U)
  {
    /* Get the pending status of the TIMESTAMP Interrupt */
    if (__HAL_RTC_TIMESTAMP_GET_FLAG(hrtc, RTC_FLAG_TSF) != 0U)
    {
      /* TIMESTAMP callback */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
      hrtc->TimeStampEventCallback(hrtc);
#else  /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
      HAL_RTCEx_TimeStampEventCallback(hrtc);
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */

      /* Clear the TIMESTAMP interrupt pending bit (this will clear timestamp time and date registers) */
      __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_TSF);
    }
  }

#if defined(RTC_TAMPCR_TAMP1E)
  /* Get the Tamper1 interrupt source enable status */
  if (__HAL_RTC_TAMPER_GET_IT_SOURCE(hrtc, RTC_IT_TAMP | RTC_IT_TAMP1) != 0U)
  {
    /* Get the pending status of the Tamper1 Interrupt */
    if (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP1F) != 0U)
    {
      /* Clear the Tamper1 interrupt pending bit */
      __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP1F);

      /* Tamper1 callback */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
      hrtc->Tamper1EventCallback(hrtc);
#else  /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
      HAL_RTCEx_Tamper1EventCallback(hrtc);
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
    }
  }
#endif /* RTC_TAMPCR_TAMP1E */

#if defined(RTC_TAMPCR_TAMP2E)
  /* Get the Tamper2 interrupt source enable status */
  if (__HAL_RTC_TAMPER_GET_IT_SOURCE(hrtc, RTC_IT_TAMP | RTC_IT_TAMP2) != 0U)
  {
    /* Get the pending status of the Tamper2 Interrupt */
    if (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP2F) != 0U)
    {
      /* Clear the Tamper2 interrupt pending bit */
      __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP2F);

      /* Tamper2 callback */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
      hrtc->Tamper2EventCallback(hrtc);
#else  /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
      HAL_RTCEx_Tamper2EventCallback(hrtc);
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
    }
  }
#endif /* RTC_TAMPCR_TAMP2E */

#if defined(RTC_TAMPCR_TAMP3E)
  /* Get the Tamper3 interrupts source enable status */
  if (__HAL_RTC_TAMPER_GET_IT_SOURCE(hrtc, RTC_IT_TAMP | RTC_IT_TAMP3) != 0U)
  {
    /* Get the pending status of the Tamper3 Interrupt */
    if (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP3F) != 0U)
    {
      /* Clear the Tamper3 interrupt pending bit */
      __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP3F);

      /* Tamper3 callback */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
      hrtc->Tamper3EventCallback(hrtc);
#else  /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
      HAL_RTCEx_Tamper3EventCallback(hrtc);
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS == 1) */
    }
  }
#endif /* RTC_TAMPCR_TAMP3E */

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
}
#endif /* RTC_ISR_TSF */

/**
  * @brief  TimeStamp callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_TimeStampEventCallback could be implemented in the user file
  */
}

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group5
  * @brief      Extended RTC Tamper functions
  *
* @{
*/

#if defined(RTC_TAMPER_1)
/**
  * @brief  Tamper 1 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef * hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_Tamper1EventCallback could be implemented in the user file
   */
}
#endif /* RTC_TAMPER_1 */

#if defined(RTC_TAMPER_2)
/**
  * @brief  Tamper 2 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef * hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_Tamper2EventCallback could be implemented in the user file
   */
}
#endif /* RTC_TAMPER_2 */

#if defined(RTC_TAMPER_3)
/**
  * @brief  Tamper 3 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_Tamper3EventCallback(RTC_HandleTypeDef * hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_Tamper3EventCallback could be implemented in the user file
   */
}
#endif /* RTC_TAMPER_3 */

/**
  * @brief  Internal Tamper 1 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper1EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper1EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 2 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper2EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper2EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 3 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper3EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper3EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 4 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper4EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper4EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 5 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper5EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper5EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 6 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper6EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper6EventCallback could be implemented in the user file
   */
}

/**
  * @brief  Internal Tamper 8 callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_InternalTamper8EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_InternalTamper8EventCallback could be implemented in the user file
   */
}
/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group1
 *  @brief   RTC TimeStamp and Tamper functions
 *
* @{
*/

/**
  * @brief  Handle TimeStamp polling request.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while (__HAL_RTC_TIMESTAMP_GET_FLAG(hrtc, RTC_FLAG_TSF) == 0U)
  {
    if (__HAL_RTC_TIMESTAMP_GET_FLAG(hrtc, RTC_FLAG_TSOVF) != 0U)
    {
      /* Clear the TIMESTAMP OverRun Flag */
      __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_TSOVF);

      /* Change TIMESTAMP state */
      hrtc->State = HAL_RTC_STATE_ERROR;

      return HAL_ERROR;
    }

    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group5
  * @brief      Extended RTC Tamper functions
  *
* @{
*/

#if defined(RTC_TAMPER_1)
/**
  * @brief  Handle Tamper1 Polling.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef * hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  /* Get the status of the Interrupt */
  while (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP1F) == 0U)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the Tamper Flag */
  __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP1F);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}
#endif /* RTC_TAMPER_1 */

#if defined(RTC_TAMPER_2)
/**
  * @brief  Handle Tamper2 Polling.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef * hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  /* Get the status of the Interrupt */
  while (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP2F) == 0U)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the Tamper Flag */
  __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP2F);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}
#endif /* RTC_TAMPER_2 */

#if defined(RTC_TAMPER_3)
/**
  * @brief  Handle Tamper3 Polling.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTamper3Event(RTC_HandleTypeDef * hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  /* Get the status of the Interrupt */
  while (__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP3F) == 0U)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the Tamper Flag */
  __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc, RTC_FLAG_TAMP3F);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}
#endif /* RTC_TAMPER_3 */

#if defined(TAMP_CR1_ITAMP1E)
/**
  * @brief  Internal Tamper event polling.
  * @param  hrtc RTC handle
  * @param  IntTamper selected tamper.
  *          This parameter can be any combination of existing internal tampers.
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForInternalTamperEvent(RTC_HandleTypeDef *hrtc, uint32_t IntTamper, uint32_t Timeout)
{
  UNUSED(hrtc);
  assert_param(IS_RTC_INTERNAL_TAMPER(IntTamper));

  uint32_t tickstart = HAL_GetTick();

  /* Get the status of the Interrupt */
  while (READ_BIT(TAMP->SR, IntTamper) != IntTamper)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the Tamper Flag */
  WRITE_REG(TAMP->SCR, IntTamper);

  return HAL_OK;
}
#endif /* TAMP_CR1_ITAMP1E */

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group2
  * @brief    RTC Wake-up functions
  *
@verbatim
 ===============================================================================
                        ##### RTC Wake-up functions #####
 ===============================================================================

 [..] This section provides functions allowing to configure Wake-up feature

@endverbatim
  * @{
  */

/**
  * @brief  Set wake up timer.
  * @param  hrtc RTC handle
  * @param  WakeUpCounter Wake up counter
  * @param  WakeUpClock Wake up clock
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_RTC_WAKEUP_CLOCK(WakeUpClock));
  assert_param(IS_RTC_WAKEUP_COUNTER(WakeUpCounter));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Check RTC WUTWF flag is reset only when wake up timer enabled */
  if ((hrtc->Instance->CR & RTC_CR_WUTE) != 0U)
  {
    tickstart = HAL_GetTick();

    /* Wait till RTC WUTWF flag is reset and if Time out is reached exit */
  #if defined(RTC_ICSR_WUTWF)
    while (READ_BIT(hrtc->Instance->ICSR, RTC_FLAG_WUTWF) != 0U)
  #endif /* RTC_ICSR_WUTWF */
  #if defined(RTC_ISR_WUTWF)
    while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) != 0U)
  #endif /* RTC_ISR_WUTWF */
    {
      if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        hrtc->State = HAL_RTC_STATE_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hrtc);

        return HAL_TIMEOUT;
      }
    }
  }

  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);

  tickstart = HAL_GetTick();

  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  #if defined(RTC_ICSR_WUTWF)
    while (READ_BIT(hrtc->Instance->ICSR, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ICSR_WUTWF */
  #if defined(RTC_ISR_WUTWF)
    while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ISR_WUTWF */
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

      hrtc->State = HAL_RTC_STATE_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_TIMEOUT;
    }
  }

  /* Clear the Wakeup Timer clock source bits and configure the clock source in CR register */
  {
    uint32_t CR_tmp = hrtc->Instance->CR;
    CR_tmp &= (uint32_t)~RTC_CR_WUCKSEL;
    CR_tmp |= (uint32_t)WakeUpClock;
    hrtc->Instance->CR = CR_tmp;
  }

  /* Configure the Wakeup Timer counter */
  hrtc->Instance->WUTR = (uint32_t)WakeUpCounter;

  /* Enable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_ENABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Set wake up timer with interrupt.
  * @param  hrtc RTC handle
  * @param  WakeUpCounter Wake up counter
  * @param  WakeUpClock Wake up clock
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_RTC_WAKEUP_CLOCK(WakeUpClock));
  assert_param(IS_RTC_WAKEUP_COUNTER(WakeUpCounter));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Check RTC WUTWF flag is reset only when wake up timer enabled */
  if ((hrtc->Instance->CR & RTC_CR_WUTE) != 0U)
  {
    tickstart = HAL_GetTick();

    /* Wait till RTC WUTWF flag is reset and if Time out is reached exit */
  #if defined(RTC_ICSR_WUTWF)
    while (READ_BIT(hrtc->Instance->ICSR, RTC_FLAG_WUTWF) != 0U)
  #endif /* RTC_ICSR_WUTWF */
  #if defined(RTC_ISR_WUTWF)
    while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) != 0U)
  #endif /* RTC_ISR_WUTWF */
    {
      if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        hrtc->State = HAL_RTC_STATE_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hrtc);

        return HAL_TIMEOUT;
      }
    }
  }
  /* Disable the Wake-Up timer */
  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);

  /* Clear flag Wake-Up */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);

  tickstart = HAL_GetTick();

  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  #if defined(RTC_ICSR_WUTWF)
    while (READ_BIT(hrtc->Instance->ICSR, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ICSR_WUTWF */
  #if defined(RTC_ISR_WUTWF)
    while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ISR_WUTWF */
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

      hrtc->State = HAL_RTC_STATE_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_TIMEOUT;
    }
  }

  /* Configure the Wakeup Timer counter */
  hrtc->Instance->WUTR = (uint32_t)WakeUpCounter;

  /* Clear the Wakeup Timer clock source bits and configure the clock source in CR register */
  {
    uint32_t CR_tmp = hrtc->Instance->CR;
    CR_tmp &= (uint32_t)~RTC_CR_WUCKSEL;
    CR_tmp |= (uint32_t)WakeUpClock;
    hrtc->Instance->CR = CR_tmp;
  }

#if !defined(DUAL_CORE)
  /* RTC WakeUpTimer Interrupt Configuration: EXTI configuration */
  __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();
#endif

  __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_RISING_EDGE();

  /* Configure the Interrupt in the RTC_CR register */
  __HAL_RTC_WAKEUPTIMER_ENABLE_IT(hrtc, RTC_IT_WUT);

  /* Enable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_ENABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Deactivate wake up timer counter.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart;

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Disable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);

  /* In case of interrupt mode is used, the interrupt source must disabled */
  __HAL_RTC_WAKEUPTIMER_DISABLE_IT(hrtc, RTC_IT_WUT);

  tickstart = HAL_GetTick();
  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  #if defined(RTC_ICSR_WUTWF)
    while (READ_BIT(hrtc->Instance->ICSR, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ICSR_WUTWF */
  #if defined(RTC_ISR_WUTWF)
    while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == 0U)
  #endif /* RTC_ISR_WUTWF */
  {
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

      hrtc->State = HAL_RTC_STATE_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_TIMEOUT;
    }
  }

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Get wake up timer counter.
  * @param  hrtc RTC handle
  * @retval Counter value
  */
uint32_t HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc)
{
  /* Get the counter value */
  return ((uint32_t)(hrtc->Instance->WUTR & RTC_WUTR_WUT));
}

/**
  * @brief  Handle Wake Up Timer interrupt request.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc)
{
  /* Clear the EXTI's line Flag for RTC WakeUpTimer */
#if defined(DUAL_CORE)
  if (HAL_GetCurrentCPUID() == CM7_CPUID)
  {
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
  }
  else
  {
    __HAL_RTC_WAKEUPTIMER_EXTID2_CLEAR_FLAG();
  }
#else  /* SINGLE_CORE */
  __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
#endif /* DUAL_CORE */

#if defined(RTC_MISR_WUTMF)
  /* Get the pending status of the WAKEUPTIMER Interrupt */
  if ((hrtc->Instance->MISR & RTC_MISR_WUTMF) != 0u)
  {
    /* Immediately clear flags */
    hrtc->Instance->SCR = RTC_SCR_CWUTF;

    /* WAKEUPTIMER callback */
  #if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call WakeUpTimerEvent registered Callback */
    hrtc->WakeUpTimerEventCallback(hrtc);
  #else
    HAL_RTCEx_WakeUpTimerEventCallback(hrtc);
  #endif /* USE_HAL_RTC_REGISTER_CALLBACKS */
  }
#endif /* RTC_MISR_WUTMF */
#if defined(RTC_ISR_WUTF)
  /* Get the pending status of the WAKEUPTIMER Interrupt */
  if (__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTF) != 0U)
  {
    /* Clear the WAKEUPTIMER interrupt pending bit */
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);

    /* WAKEUPTIMER callback */
  #if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
    /* Call WakeUpTimerEvent registered Callback */
    hrtc->WakeUpTimerEventCallback(hrtc);
  #else
    HAL_RTCEx_WakeUpTimerEventCallback(hrtc);
  #endif /* USE_HAL_RTC_REGISTER_CALLBACKS */
  }
#endif /* RTC_ISR_WUTF */

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
}

/**
  * @brief  Wake Up Timer callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef * hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_WakeUpTimerEventCallback could be implemented in the user file
   */
}


/**
  * @brief  Handle Wake Up Timer Polling.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef * hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTF) == 0U)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the WAKEUPTIMER Flag */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}

/**
  * @}
  */


/** @addtogroup RTCEx_Exported_Functions_Group6
  * @brief      Extended RTC Backup register functions
  *
@verbatim
 ===============================================================================
             ##### Extended RTC Backup register functions #####
 ===============================================================================
  [..]
   (+) Before calling any tamper or internal tamper function, you have to call first
       HAL_RTC_Init() function.
   (+) In that ine you can select to output tamper event on RTC pin.
  [..]
   This subsection provides functions allowing to
   (+) Write a data in a specified RTC Backup data register
   (+) Read a data in a specified RTC Backup data register
@endverbatim
  * @{
  */


/**
  * @brief  Write a data in a specified RTC Backup data register.
  * @param  hrtc RTC handle
  * @param  BackupRegister RTC Backup data Register number.
  *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 31 to
  *                                 specify the register.
  * @param  Data Data to be written in the specified Backup data register.
  * @retval None
  */
void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef * hrtc, uint32_t BackupRegister, uint32_t Data)
{
  uint32_t tmp;

  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));

  /* Point on address of first backup register */
#if defined(TAMP_BKP0R)
  tmp = (uint32_t) & (((TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET))->BKP0R);
#endif /* TAMP_BKP0R */
#if defined(RTC_BKP0R)
  tmp = (uint32_t) & (hrtc->Instance->BKP0R);
#endif /* RTC_BKP0R */

  tmp += (BackupRegister * 4U);

  /* Write the specified register */
  *(__IO uint32_t *)tmp = (uint32_t)Data;
}


/**
  * @brief  Read data from the specified RTC Backup data Register.
  * @param  hrtc RTC handle
  * @param  BackupRegister RTC Backup data Register number.
  *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 31 to
  *                                 specify the register.
  * @retval Read value
  */
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef * hrtc, uint32_t BackupRegister)
{
  uint32_t tmp;

  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));

  /* Point on address of first backup register */
#if defined(TAMP_BKP0R)
  tmp = (uint32_t) & (((TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET))->BKP0R);
#endif /* TAMP_BKP0R */
#if defined(RTC_BKP0R)
  tmp = (uint32_t) & (hrtc->Instance->BKP0R);
#endif /* RTC_BKP0R */

  tmp += (BackupRegister * 4U);

  /* Read the specified register */
  return (*(__IO uint32_t *)tmp);
}


/**
  * @}
  */


/** @addtogroup RTCEx_Exported_Functions_Group3
  * @brief    Extended Peripheral Control functions
  *
@verbatim
 ===============================================================================
              ##### Extended Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Write a data in a specified RTC Backup data register
      (+) Read a data in a specified RTC Backup data register
      (+) Set the Smooth calibration parameters.
      (+) Set Low Power calibration parameter (if feature supported).
      (+) Configure the Synchronization Shift Control Settings.
      (+) Configure the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
      (+) Deactivate the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
      (+) Enable the RTC reference clock detection.
      (+) Disable the RTC reference clock detection.
      (+) Enable the Bypass Shadow feature.
      (+) Disable the Bypass Shadow feature.

@endverbatim
  * @{
  */


/**
  * @brief  Set the Smooth calibration parameters.
  * @param  hrtc RTC handle
  * @param  SmoothCalibPeriod Select the Smooth Calibration Period.
  *          This parameter can be can be one of the following values :
  *             @arg RTC_SMOOTHCALIB_PERIOD_32SEC: The smooth calibration period is 32s.
  *             @arg RTC_SMOOTHCALIB_PERIOD_16SEC: The smooth calibration period is 16s.
  *             @arg RTC_SMOOTHCALIB_PERIOD_8SEC: The smooth calibration period is 8s.
  * @param  SmoothCalibPlusPulses Select to Set or reset the CALP bit.
  *          This parameter can be one of the following values:
  *             @arg RTC_SMOOTHCALIB_PLUSPULSES_SET: Add one RTCCLK pulse every 2*11 pulses.
  *             @arg RTC_SMOOTHCALIB_PLUSPULSES_RESET: No RTCCLK pulses are added.
  * @param  SmoothCalibMinusPulsesValue Select the value of CALM[8:0] bits.
  *          This parameter can be one any value from 0 to 0x000001FF.
  * @note   To deactivate the smooth calibration, the field SmoothCalibPlusPulses
  *         must be equal to SMOOTHCALIB_PLUSPULSES_RESET and the field
  *         SmoothCalibMinusPulsesValue must be equal to 0.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef * hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmoothCalibMinusPulsesValue)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_RTC_SMOOTH_CALIB_PERIOD(SmoothCalibPeriod));
  assert_param(IS_RTC_SMOOTH_CALIB_PLUS(SmoothCalibPlusPulses));
  assert_param(IS_RTC_SMOOTH_CALIB_MINUS(SmoothCalibMinusPulsesValue));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* check if a calibration operation is pending */
#if defined(RTC_ICSR_RECALPF)
  if ((hrtc->Instance->ICSR & RTC_ICSR_RECALPF) != 0U)
#endif /* RTC_ICSR_RECALPF */
#if defined(RTC_ISR_RECALPF)
    if ((hrtc->Instance->ISR  & RTC_ISR_RECALPF)  != 0U)
#endif /* RTC_ISR_RECALPF */
    {
      tickstart = HAL_GetTick();

      /* Wait for pending calibration operation to finish */
#if defined(RTC_ICSR_RECALPF)
      while ((hrtc->Instance->ICSR & RTC_ICSR_RECALPF) != 0U)
#endif /* RTC_ICSR_RECALPF */
#if defined(RTC_ISR_RECALPF)
        while ((hrtc->Instance->ISR  & RTC_ISR_RECALPF)  != 0U)
#endif /* RTC_ISR_RECALPF */
        {
          if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
          {
            /* Enable the write protection for RTC registers */
            __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

            /* Change RTC state */
            hrtc->State = HAL_RTC_STATE_TIMEOUT;

            /* Process Unlocked */
            __HAL_UNLOCK(hrtc);

            return HAL_TIMEOUT;
          }
        }
    }

  /* Configure the Smooth calibration settings */
  MODIFY_REG(hrtc->Instance->CALR, (RTC_CALR_CALP | RTC_CALR_CALW8 | RTC_CALR_CALW16 | RTC_CALR_CALM), (uint32_t)(SmoothCalibPeriod | SmoothCalibPlusPulses | SmoothCalibMinusPulsesValue));

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Configure the Synchronization Shift Control Settings.
  * @note   When REFCKON is set, firmware must not write to Shift control register.
  * @param  hrtc RTC handle
  * @param  ShiftAdd1S Select to add or not 1 second to the time calendar.
  *          This parameter can be one of the following values:
  *             @arg RTC_SHIFTADD1S_SET: Add one second to the clock calendar.
  *             @arg RTC_SHIFTADD1S_RESET: No effect.
  * @param  ShiftSubFS Select the number of Second Fractions to substitute.
  *          This parameter can be one any value from 0 to 0x7FFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef * hrtc, uint32_t ShiftAdd1S, uint32_t ShiftSubFS)
{
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_RTC_SHIFT_ADD1S(ShiftAdd1S));
  assert_param(IS_RTC_SHIFT_SUBFS(ShiftSubFS));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  tickstart = HAL_GetTick();

  /* Wait until the shift is completed */
#if defined(RTC_ICSR_SHPF)
  while ((hrtc->Instance->ICSR & RTC_ICSR_SHPF) != 0U)
#endif /* RTC_ICSR_SHPF */
#if defined(RTC_ISR_SHPF)
    while ((hrtc->Instance->ISR  & RTC_ISR_SHPF)  != 0U)
#endif /* RTC_ISR_SHPF */
    {
      if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        hrtc->State = HAL_RTC_STATE_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hrtc);

        return HAL_TIMEOUT;
      }
    }

  /* Check if the reference clock detection is disabled */
  if ((hrtc->Instance->CR & RTC_CR_REFCKON) == 0U)
  {
    /* Configure the Shift settings */
    hrtc->Instance->SHIFTR = (uint32_t)(uint32_t)(ShiftSubFS) | (uint32_t)(ShiftAdd1S);

    /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if ((hrtc->Instance->CR & RTC_CR_BYPSHAD) == 0U)
    {
      if (HAL_RTC_WaitForSynchro(hrtc) != HAL_OK)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        hrtc->State = HAL_RTC_STATE_ERROR;

        /* Process Unlocked */
        __HAL_UNLOCK(hrtc);

        return HAL_ERROR;
      }
    }
  }
  else
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

    /* Change RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

    /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Configure the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
  * @param  hrtc RTC handle
  * @param  CalibOutput Select the Calibration output Selection.
  *          This parameter can be one of the following values:
  *             @arg RTC_CALIBOUTPUT_512HZ: A signal has a regular waveform at 512Hz.
  *             @arg RTC_CALIBOUTPUT_1HZ: A signal has a regular waveform at 1Hz.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef * hrtc, uint32_t CalibOutput)
{
  /* Check the parameters */
  assert_param(IS_RTC_CALIB_OUTPUT(CalibOutput));

  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Clear flags before config */
  hrtc->Instance->CR &= (uint32_t)~RTC_CR_COSEL;

  /* Configure the RTC_CR register */
  hrtc->Instance->CR |= (uint32_t)CalibOutput;

  __HAL_RTC_CALIBRATION_OUTPUT_ENABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Deactivate the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef * hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  __HAL_RTC_CALIBRATION_OUTPUT_DISABLE(hrtc);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Enable the RTC reference clock detection.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef * hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;

    /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
    __HAL_RTC_CLOCKREF_DETECTION_ENABLE(hrtc);

    /* Exit Initialization mode */
#if defined(RTC_ICSR_INIT)
    hrtc->Instance->ICSR &= (uint32_t)~RTC_ICSR_INIT;
#endif /* RTC_ICSR_INIT */
#if defined(RTC_ISR_INIT)
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT;
#endif /* RTC_ISR_INIT */
  }

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Disable the RTC reference clock detection.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef * hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;

    /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
    __HAL_RTC_CLOCKREF_DETECTION_DISABLE(hrtc);

    /* Exit Initialization mode */
#if defined(RTC_ICSR_INIT)
    hrtc->Instance->ICSR &= (uint32_t)~RTC_ICSR_INIT;
#endif /* RTC_ICSR_INIT */
#if defined(RTC_ISR_INIT)
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT;
#endif /* RTC_ISR_INIT */
  }

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Enable the Bypass Shadow feature.
  * @note   When the Bypass Shadow is enabled the calendar value are taken
  *         directly from the Calendar counter.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef * hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set the BYPSHAD bit */
  hrtc->Instance->CR |= (uint8_t)RTC_CR_BYPSHAD;

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Disable the Bypass Shadow feature.
  * @note   When the Bypass Shadow is enabled the calendar value are taken
  *         directly from the Calendar counter.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef * hrtc)
{
  /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Reset the BYPSHAD bit */
  hrtc->Instance->CR &= ((uint8_t)~RTC_CR_BYPSHAD);

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

#if defined(TAMP_COUNTR)
/**
  * @brief  Increment Monotonic counter.
  * @param  hrtc RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterIncrement(RTC_HandleTypeDef *hrtc)
{
  UNUSED(hrtc);
  /* This register is read-only only and is incremented by one when a write access is done to this
     register. This register cannot roll-over and is frozen when reaching the maximum value. */
  CLEAR_REG(TAMP->COUNTR);

  return HAL_OK;
}

/**
  * @brief  Monotonic counter incrementation.
  * @param  hrtc RTC handle
  * @param  Counter monotonic counter value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterGet(RTC_HandleTypeDef *hrtc, uint32_t *Counter)
{
  UNUSED(hrtc);
  /* This register is read-only only and is incremented by one when a write access is done to this
     register. This register cannot roll-over and is frozen when reaching the maximum value. */
  *Counter = READ_REG(TAMP->COUNTR);

  return HAL_OK;
}
#endif /* TAMP_COUNTR */

/**
  * @}
  */

/** @addtogroup RTCEx_Exported_Functions_Group4
  * @brief    Extended features functions
  *
@verbatim
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) RTC Alarm B callback
      (+) RTC Poll for Alarm B request

@endverbatim
  * @{
  */

/**
  * @brief  Alarm B callback.
  * @param  hrtc RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef * hrtc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RTCEx_AlarmBEventCallback could be implemented in the user file
   */
}

/**
  * @brief  Handle Alarm B Polling request.
  * @param  hrtc RTC handle
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef * hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while (__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRBF) == 0U)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

  /* Clear the Alarm Flag */
  __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRBF);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}

/**
  * @}
  */


/**
  * @}
  */

#endif /* HAL_RTC_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
