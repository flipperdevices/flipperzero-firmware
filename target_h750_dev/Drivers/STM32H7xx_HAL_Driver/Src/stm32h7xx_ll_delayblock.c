/**
  ******************************************************************************
  * @file    stm32h7xx_ll_delayblock.c
  * @author  MCD Application Team
  * @brief   DelayBlock Low Layer HAL module driver.
  *    
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Delay Block peripheral:
  *           + input clock frequency range 25MHz to 208MHz
  *           + up to 12 oversampling phases
  *         
  @verbatim
  ==============================================================================
                       ##### DelayBlock peripheral features #####
  ==============================================================================        
    [..] The Delay block is used to generate an Output clock which is de-phased from the Input
          clock. The phase of the Output clock is programmed by FW. The Output clock is then used
          to clock the receive data in i.e. a SDMMC or QSPI interface.
         The delay is Voltage and Temperature dependent, which may require FW to do re-tuning
          and recenter the Output clock phase to the receive data.
    
    [..] The Delay Block features include the following:
         (+) Input clock frequency range 25MHz to 208MHz.
         (+) Up to 12 oversampling phases.
         
                           ##### How to use this driver #####
  ==============================================================================
    [..]
      This driver is a considered as a driver of service for external devices drivers
      that interfaces with the DELAY peripheral.
      The DelayBlock_Enable() function, enables the DelayBlock instance, configure the delay line length
      and configure the Output clock phase.
      The DelayBlock_Disable() function, disables the DelayBlock instance by setting DEN flag to 0.
      
  
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

/** @defgroup DELAYBLOCK_LL DELAYBLOCK_LL
  * @brief Low layer module for Delay Block
  * @{
  */

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_QSPI_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DLYB_TIMEOUT 0xFFU
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup DelayBlock_LL_Exported_Functions Delay Block Low Layer Exported Functions
  * @{
  */

/** @defgroup HAL_DELAY_LL_Group1 Initialization de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
 
@endverbatim
  * @{
  */


/**
  * @brief  Enable the Delay Block instance.
  * @param  DLYBx: Pointer to DLYB instance.
  * @retval HAL status
  */
HAL_StatusTypeDef DelayBlock_Enable(DLYB_TypeDef *DLYBx)
{
  uint32_t unit = 0U;
  uint32_t sel = 0U;
  uint32_t sel_current;
  uint32_t unit_current;
  uint32_t tuning;
  uint32_t lng_mask;
  uint32_t tickstart;

  DLYBx->CR = DLYB_CR_DEN | DLYB_CR_SEN;

  for (sel_current = 0U; sel_current < DLYB_MAX_SELECT; sel_current++)
  {
    /* lng_mask is the mask bit for the LNG field to check the output of the UNITx*/
    lng_mask = DLYB_CFGR_LNG_0 << sel_current;
    tuning = 0U;
    for (unit_current = 0U; unit_current < DLYB_MAX_UNIT; unit_current++)
    {
      /* Set the Delay of the UNIT(s)*/
      DLYBx->CFGR = DLYB_MAX_SELECT | (unit_current << DLYB_CFGR_UNIT_Pos);

      /* Waiting for a LNG valid value */
      tickstart =  HAL_GetTick();
      while ((DLYBx->CFGR & DLYB_CFGR_LNGF) == 0U)
      {
        if((HAL_GetTick() - tickstart) >=  DLYB_TIMEOUT)
        {
          return HAL_TIMEOUT;
        }
      }
      if (tuning == 0U)
      {
        if ((DLYBx->CFGR & lng_mask) != 0U)
        {
          /* 1/2 period HIGH is detected */
          tuning = 1U;
        }
      }
      else
      {
        /* 1/2 period LOW detected after the HIGH 1/2 period => FULL PERIOD passed*/
        if((DLYBx->CFGR & lng_mask ) == 0U)
        {
          /* Save the first result */
          if( unit == 0U )
          {
            unit = unit_current;
            sel  = sel_current + 1U;
          }
          break;
        }
      }
    }
  }

  /* Apply the Tuning settings */
  DLYBx->CR   = 0U;
  DLYBx->CR   = DLYB_CR_DEN | DLYB_CR_SEN;
  DLYBx->CFGR = sel | (unit << DLYB_CFGR_UNIT_Pos);
  DLYBx->CR   = DLYB_CR_DEN;

  return HAL_OK;
}

/**
  * @brief  Disable the Delay Block instance.
  * @param  DLYBx: Pointer to DLYB instance.
  * @retval HAL status
  */
HAL_StatusTypeDef DelayBlock_Disable(DLYB_TypeDef *DLYBx)
{
  /* Disable DLYB */
  DLYBx->CR = 0U;
  return HAL_OK;
}

/**
  * @brief  Configure the Delay Block instance.
  * @param  DLYBx: Pointer to DLYB instance.
  * @param  PhaseSel: Phase selection [0..11].
  * @param  Units: Delay units[0..127].
  * @retval HAL status
  */
HAL_StatusTypeDef DelayBlock_Configure(DLYB_TypeDef *DLYBx,uint32_t PhaseSel, uint32_t Units )
{
  /* Apply the delay settings */

  DLYBx->CR   = 0U;
  DLYBx->CR   = DLYB_CR_DEN | DLYB_CR_SEN;
  DLYBx->CFGR = PhaseSel | (Units << DLYB_CFGR_UNIT_Pos);
  DLYBx->CR   = DLYB_CR_DEN;

  return HAL_OK;
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* (HAL_SD_MODULE_ENABLED) & (HAL_QSPI_MODULE_ENABLED)*/
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
