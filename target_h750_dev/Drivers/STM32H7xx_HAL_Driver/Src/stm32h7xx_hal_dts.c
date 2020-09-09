/**
  ******************************************************************************
  * @file    stm32h7xx_hal_dts.c
  * @author  MCD Application Team
  * @brief   DTS HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the DTS peripheral:
  *           + Initialization and de-initialization functions
  *           + Start/Stop operation functions in polling mode.
  *           + Start/Stop operation functions in interrupt mode.
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
================================================================================
          ##### DTS Peripheral features #####
================================================================================

  [..]
      The STM32h7xx device family integrate one DTS sensor interface :


            ##### How to use this driver #####
================================================================================
  [..]


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

#ifdef HAL_DTS_MODULE_ENABLED

#if defined(DTS)

/** @defgroup DTS DTS
  * @brief DTS HAL module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup DTS_Private_Constants
  * @{
  */

/* @brief Delay for DTS startup time
 * @note  Delay required to get ready for DTS Block.
 * @note  Unit: ms
 */
#define DTS_DELAY_STARTUP (1UL)

/* @brief DTS measure ready flag time out value.
 * @note  Maximal measurement time is when LSE is selected as ref_clock and
 *        maximal sampling time is used, taking calibration into account this
 *        is equivalent to ~620 us. Use 5 ms as arbitrary timeout
 * @note Unit: ms
 */
#define TS_TIMEOUT_MS     (5UL)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
static void DTS_ResetCallback(DTS_HandleTypeDef *hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
/* Exported functions --------------------------------------------------------*/

/** @defgroup DTS_Exported_Functions DTS Exported Functions
  * @{
  */

/** @defgroup DTS_Exported_Functions_Group1 Initialization/de-initialization functions
 *  @brief    Initialization and de-initialization functions.
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions to initialize and de-initialize comparators

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DTS according to the specified
  *         parameters in the DTS_InitTypeDef and initialize the associated handle.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Init(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));
  assert_param(IS_DTS_QUICKMEAS(hdts->Init.QuickMeasure));
  assert_param(IS_DTS_REFCLK(hdts->Init.RefClock));
  assert_param(IS_DTS_TRIGGERINPUT(hdts->Init.TriggerInput));
  assert_param(IS_DTS_SAMPLINGTIME(hdts->Init.SamplingTime));
  assert_param(IS_DTS_THRESHOLD(hdts->Init.HighThreshold));
  assert_param(IS_DTS_THRESHOLD(hdts->Init.LowThreshold));

  if (hdts->State == HAL_DTS_STATE_RESET)
  {
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
    /* Reset interrupt callbacks to legacy weak callbacks */
    DTS_ResetCallback(hdts);

    if (hdts->MspInitCallback == NULL)
    {
      hdts->MspInitCallback = HAL_DTS_MspInit;
    }

    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    hdts->MspInitCallback(hdts);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_DTS_MspInit(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Change the DTS state */
  hdts->State = HAL_DTS_STATE_BUSY;

  /* Check ramp coefficient */
  if (hdts->Instance->RAMPVALR == 0UL)
  {
    return HAL_ERROR;
  }

  /* Check factory calibration temperature  */
  if (hdts->Instance->T0VALR1 == 0UL)
  {
    return HAL_ERROR;
  }

  /* Check Quick Measure option is enabled or disabled */
  if (hdts->Init.QuickMeasure == DTS_QUICKMEAS_DISABLE)
  {
    /* Check Reference clock selection */
    if (hdts->Init.RefClock == DTS_REFCLKSEL_PCLK)
    {
      assert_param(IS_DTS_DIVIDER_RATIO_NUMBER(hdts->Init.Divider));
    }
    /* Quick measurement mode disabled */
    CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_Q_MEAS_OPT);
  }
  else
  {
    /* DTS_QUICKMEAS_ENABLE shall be used only when the LSE clock is
       selected as reference clock */
    if (hdts->Init.RefClock != DTS_REFCLKSEL_LSE)
    {
      return HAL_ERROR;
    }

    /* Quick measurement mode enabled - no calibration needed */
    SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_Q_MEAS_OPT);
  }

  /* set the DTS clk source */
  if (hdts->Init.RefClock == DTS_REFCLKSEL_LSE)
  {
    SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_REFCLK_SEL);
  }
  else
  {
    CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_REFCLK_SEL);
  }

  MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_HSREF_CLK_DIV, (hdts->Init.Divider << DTS_CFGR1_HSREF_CLK_DIV_Pos));
  MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_TS1_SMP_TIME, hdts->Init.SamplingTime);
  MODIFY_REG(hdts->Instance->CFGR1, DTS_CFGR1_TS1_INTRIG_SEL, hdts->Init.TriggerInput);
  MODIFY_REG(hdts->Instance->ITR1, DTS_ITR1_TS1_HITTHD, (hdts->Init.HighThreshold << DTS_ITR1_TS1_HITTHD_Pos));
  MODIFY_REG(hdts->Instance->ITR1, DTS_ITR1_TS1_LITTHD, hdts->Init.LowThreshold);

  /* Change the DTS state */
  hdts->State = HAL_DTS_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the DTS peripheral.
  * @note   Deinitialization cannot be performed if the DTS configuration is locked.
  *         To unlock the configuration, perform a system reset.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_DeInit(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameter */
  assert_param(IS_DTS_ALL_INSTANCE(hdts->Instance));

  /* Set DTS_CFGR register to reset value */
  CLEAR_REG(hdts->Instance->CFGR1);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
  if (hdts->MspDeInitCallback == NULL)
  {
    hdts->MspDeInitCallback = HAL_DTS_MspDeInit;
  }

  /* DeInit the low level hardware: CLOCK, NVIC.*/
  hdts->MspDeInitCallback(hdts);
#else
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_DTS_MspDeInit(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

  hdts->State = HAL_DTS_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Initialize the DTS MSP.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_MspInit(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_MspInit could be implemented in the user file
  */
}

/**
  * @brief  DeInitialize the DTS MSP.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_MspDeInit(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_MspDeInit could be implemented in the user file
  */
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group2 Start-Stop operation functions
 *  @brief   Start-Stop operation functions.
 *
@verbatim
 ===============================================================================
                      ##### DTS Start Stop operation functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Start a DTS Sensor without interrupt.
      (+) Stop a DTS Sensor without interrupt.
      (+) Start a DTS Sensor with interrupt generation.
      (+) Stop a DTS Sensor with interrupt generation.

@endverbatim
  * @{
  */

/**
  * @brief  Start the DTS sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Start(DTS_HandleTypeDef *hdts)
{
  uint32_t Ref_Time;

  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  if (hdts->State == HAL_DTS_STATE_READY)
  {
    hdts->State = HAL_DTS_STATE_BUSY;

    /* Enable DTS sensor */
    __HAL_DTS_ENABLE(hdts);

    /* Get Start Tick*/
    Ref_Time = HAL_GetTick();

    /* Wait till TS1_RDY flag is set */
    while (__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_RDY) == RESET)
    {
      if ((HAL_GetTick() - Ref_Time) > DTS_DELAY_STARTUP)
      {
        return HAL_TIMEOUT;
      }
    }

    if (__HAL_DTS_GET_TRIGGER(hdts) == DTS_TRIGGER_HW_NONE)
    {
      /* Start continuous measures */
      SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

      /* Ensure start is taken into account */
      HAL_Delay(TS_TIMEOUT_MS);
    }

    hdts->State = HAL_DTS_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Stop the DTS Sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Stop(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  if (hdts->State == HAL_DTS_STATE_READY)
  {
    hdts->State = HAL_DTS_STATE_BUSY;

    if (__HAL_DTS_GET_TRIGGER(hdts) == DTS_TRIGGER_HW_NONE)
    {
      CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);
    }

    /* Disable the selected DTS sensor */
    __HAL_DTS_DISABLE(hdts);

    hdts->State = HAL_DTS_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Enable the interrupt(s) and start the DTS sensor
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Start_IT(DTS_HandleTypeDef *hdts)
{
  uint32_t Ref_Time;

  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  if (hdts->State == HAL_DTS_STATE_READY)
  {
    hdts->State = HAL_DTS_STATE_BUSY;

    /* On Asynchronous mode enable the asynchronous IT */
    if (hdts->Init.RefClock == DTS_REFCLKSEL_LSE)
    {
      __HAL_DTS_ENABLE_IT(hdts, DTS_IT_TS1_AITE | DTS_IT_TS1_AITL | DTS_IT_TS1_AITH);
    }
    else
    {
      /* Enable the IT(s) */
      __HAL_DTS_ENABLE_IT(hdts, DTS_IT_TS1_ITE | DTS_IT_TS1_ITL | DTS_IT_TS1_ITH);
    }

    /* Enable the selected DTS sensor */
    __HAL_DTS_ENABLE(hdts);

    /* Get Start Tick*/
    Ref_Time = HAL_GetTick();

    /* Wait till TS1_RDY flag is set */
    while (__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_RDY) == RESET)
    {
      if ((HAL_GetTick() - Ref_Time) > DTS_DELAY_STARTUP)
      {
        return HAL_TIMEOUT;
      }
    }

    if (__HAL_DTS_GET_TRIGGER(hdts) == DTS_TRIGGER_HW_NONE)
    {
      /* Start continuous measures */
      SET_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);

      /* Ensure start is taken into account */
      HAL_Delay(TS_TIMEOUT_MS);
    }

    hdts->State = HAL_DTS_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Disable the interrupt(s) and stop the DTS sensor.
  * @param  hdts  DTS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_Stop_IT(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_ERROR;
  }

  if (hdts->State == HAL_DTS_STATE_READY)
  {
    hdts->State = HAL_DTS_STATE_BUSY;

    /* On Asynchronous mode disable the asynchronous IT */
    if (hdts->Init.RefClock == DTS_REFCLKSEL_LSE)
    {
      __HAL_DTS_DISABLE_IT(hdts, DTS_IT_TS1_AITE | DTS_IT_TS1_AITL | DTS_IT_TS1_AITH);
    }
    else
    {
      /* Disable the IT(s) */
      __HAL_DTS_DISABLE_IT(hdts, DTS_IT_TS1_ITE | DTS_IT_TS1_ITL | DTS_IT_TS1_ITH);
    }

    if (__HAL_DTS_GET_TRIGGER(hdts) == DTS_TRIGGER_HW_NONE)
    {
      CLEAR_BIT(hdts->Instance->CFGR1, DTS_CFGR1_TS1_START);
    }

    /* Disable the selected DTS sensor */
    __HAL_DTS_DISABLE(hdts);

    hdts->State = HAL_DTS_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Get temperature from DTS
  * @param  hdts         DTS handle
  * @param  Temperature  Temperature in deg C
  * @note This function retrieves latest available measure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DTS_GetTemperature(DTS_HandleTypeDef *hdts, int32_t *Temperature)
{
  uint32_t freq_meas;
  uint32_t samples;
  uint32_t t0_temp;
  uint32_t t0_freq;
  uint32_t ramp_coeff;

  if (hdts->State == HAL_DTS_STATE_READY)
  {
    hdts->State = HAL_DTS_STATE_BUSY;

    /* Get the total number of samples */
    samples = (hdts->Instance->DR & DTS_DR_TS1_MFREQ);

    if ((hdts->Init.SamplingTime == 0UL) || (samples == 0UL))
    {
      hdts->State = HAL_DTS_STATE_READY;
      return HAL_ERROR;
    }

    if ((hdts->Init.RefClock) == DTS_REFCLKSEL_LSE)
    {
      freq_meas = (LSE_VALUE * samples) / (hdts->Init.SamplingTime >> DTS_CFGR1_TS1_SMP_TIME_Pos); /* On Hz */
    }
    else
    {
      freq_meas = (HAL_RCCEx_GetD3PCLK1Freq() * (hdts->Init.SamplingTime >> DTS_CFGR1_TS1_SMP_TIME_Pos)) / samples; /* On Hz */
    }

    /* Read factory settings */
    t0_temp = hdts->Instance->T0VALR1 >> DTS_T0VALR1_TS1_T0_Pos;

    if (t0_temp == 0UL)
    {
      t0_temp = 30UL; /* 30 deg C */
    }
    else if (t0_temp == 1UL)
    {
      t0_temp = 110UL; /* 110 deg C */
    }
    else
    {
      hdts->State = HAL_DTS_STATE_READY;
      return HAL_ERROR;
    }

    t0_freq = (hdts->Instance->T0VALR1 & DTS_T0VALR1_TS1_FMT0) * 100UL; /* Hz */

    ramp_coeff = hdts->Instance->RAMPVALR & DTS_RAMPVALR_TS1_RAMP_COEFF; /* deg C/Hz */

    if (ramp_coeff == 0UL)
    {
      hdts->State = HAL_DTS_STATE_READY;
      return HAL_ERROR;
    }

    /* Figure out the temperature deg C */
    *Temperature = (int32_t)t0_temp + (((int32_t)freq_meas - (int32_t)t0_freq) / (int32_t)ramp_coeff);

    hdts->State = HAL_DTS_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  DTS sensor IRQ Handler.
  * @param  hdts  DTS handle
  * @retval None
  */
void HAL_DTS_IRQHandler(DTS_HandleTypeDef *hdts)
{
  /* Check end of measure Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITE)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_AITE);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->AsyncEndCallback(hdts);
#else
      HAL_DTS_AsyncEndCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Check low threshold Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITL)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_AITL);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->AsyncLowCallback(hdts);
#else
      HAL_DTS_AsyncLowCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Check high threshold Asynchronous IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_AITH)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_AITH);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->AsyncHighCallback(hdts);
#else
      HAL_DTS_AsyncHighCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Check end of measure IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITE)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_ITE);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->EndCallback(hdts);
#else
      HAL_DTS_EndCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Check low threshold IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITL)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_ITL);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->LowCallback(hdts);
#else
      HAL_DTS_LowCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }

  /* Check high threshold IT */
  if ((__HAL_DTS_GET_FLAG(hdts, DTS_FLAG_TS1_ITH)) != RESET)
  {
      __HAL_DTS_CLEAR_FLAG(hdts, DTS_FLAG_TS1_ITH);

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
      hdts->HighCallback(hdts);
#else
      HAL_DTS_HighCallback(hdts);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  DTS Sensor End measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_EndCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_EndCallback should be implemented in the user file
  */
}

/**
  * @brief  DTS Sensor low threshold measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_LowCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_LowCallback should be implemented in the user file
  */
}

/**
  * @brief  DTS Sensor high threshold measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_HighCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_HighCallback should be implemented in the user file
  */
}

/**
  * @brief  DTS Sensor asynchronous end measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncEndCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_AsyncEndCallback should be implemented in the user file
  */
}

/**
  * @brief  DTS Sensor asynchronous low threshold measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncLowCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_AsyncLowCallback should be implemented in the user file
  */
}

/**
  * @brief  DTS Sensor asynchronous high threshold measure callback.
  * @param  hdts  DTS handle
  * @retval None
  */
__weak void HAL_DTS_AsyncHighCallback(DTS_HandleTypeDef *hdts)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdts);

  /* NOTE : This function should not be modified, when the callback is needed,
  the HAL_DTS_AsyncHighCallback should be implemented in the user file
  */
}

/**
  * @}
  */

/** @defgroup DTS_Exported_Functions_Group3 Peripheral State functions
 *  @brief   Peripheral State functions.
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the DTS handle state.
  * @param  hdts  DTS handle
  * @retval HAL state
  */
HAL_DTS_StateTypeDef HAL_DTS_GetState(DTS_HandleTypeDef *hdts)
{
  /* Check the DTS handle allocation */
  if (hdts == NULL)
  {
    return HAL_DTS_STATE_RESET;
  }

  /* Return DTS handle state */
  return hdts->State;
}
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @defgroup DTS_Private_Functions DTS Private Functions
  * @{
  */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
/**
  * @brief  Reset interrupt callbacks to the legacy weak callbacks.
  * @param  hdts pointer to a DTS_HandleTypeDef structure that contains
  *                the configuration information for DTS module.
  * @retval None
  */
static void DTS_ResetCallback(DTS_HandleTypeDef *hdts)
{
  /* Reset the DTS callback to the legacy weak callbacks */
  hdts->DTS_EndCallback       = HAL_DTS_EndCallback;        /* End measure Callback                 */
  hdts->DTS_LowCallback       = HAL_DTS_LowCallback;        /* low threshold Callback               */
  hdts->DTS_HighCallback      = HAL_DTS_HighCallback;       /* high threshold Callback              */
  hdts->DTS_AsyncEndCallback  = HAL_DTS_AsyncEndCallback;   /* Asynchronous end of measure Callback */
  hdts->DTS_AsyncLowCallback  = HAL_DTS_AsyncLowCallback;   /* Asynchronous low threshold Callback  */
  hdts->DTS_AsyncHighCallback = HAL_DTS_AsyncHighCallback;  /* Asynchronous high threshold Callback */
}
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
/**
  * @}
  */

/**
  * @}
  */

#endif /* DTS */

#endif /* HAL_DTS_MODULE_ENABLED */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
