/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rng_ex.c
  * @author  MCD Application Team
  * @brief   Extended RNG HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Random Number Generator (RNG) peripheral:
  *           + Lock configuration functions
  *           + Reset the RNG
  *
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

#if defined (RNG)

/** @addtogroup RNGEx
  * @brief RNG Extended HAL module driver.
  * @{
  */

#ifdef HAL_RNG_MODULE_ENABLED
#if defined (RNG_CR_CONDRST)
/* Private types -------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup RNGEx_Private_Defines
  * @{
  */
/*  Health test control register information to use in CCM algorithm */
#define RNG_HTCFG_1   0x17590ABCU /*!< magic number */
#if defined(RNG_VER_3_1) || defined(RNG_VER_3_0)
#define RNG_HTCFG     0x000CAA74U /*!< for best latency and To be compliant with NIST */
#else /*RNG_VER_3_2*/
#define RNG_HTCFG     0x00007274U /*!< for best latency and To be compliant with NIST */
#endif
/**
  * @}
  */
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RNGEx_Private_Constants RNG Private Constants
  * @{
  */
#define RNG_TIMEOUT_VALUE     2U
/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/* Private functions prototypes ----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup RNGEx_Exported_Functions
  * @{
  */

/** @addtogroup RNGEx_Exported_Functions_Group1
 *  @brief   Configuration functions
 *
@verbatim
 ===============================================================================
          ##### Configuration and lock functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure the RNG with the specified parameters in the RNG_ConfigTypeDef
      (+) Lock RNG configuration Allows user to lock a configuration until next reset.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the RNG with the specified parameters in the
  *         RNG_ConfigTypeDef.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *          the configuration information for RNG.
  * @param  pConf: pointer to a RNG_ConfigTypeDef structure that contains
  *         the configuration information for RNG module

  * @retval HAL status
*/
HAL_StatusTypeDef HAL_RNGEx_SetConfig(RNG_HandleTypeDef *hrng, RNG_ConfigTypeDef *pConf)
{
  uint32_t tickstart;
  uint32_t cr_value;
  HAL_StatusTypeDef status ;

  /* Check the RNG handle allocation */
  if ((hrng == NULL) || (pConf == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RNG_ALL_INSTANCE(hrng->Instance));
  assert_param(IS_RNG_CLOCK_DIVIDER(pConf->ClockDivider));
  assert_param(IS_RNG_NIST_COMPLIANCE(pConf->NistCompliance));
  assert_param(IS_RNG_CONFIG1(pConf->Config1));
  assert_param(IS_RNG_CONFIG2(pConf->Config2));
  assert_param(IS_RNG_CONFIG3(pConf->Config3));

  /* Check RNG peripheral state */
  if (hrng->State == HAL_RNG_STATE_READY)
  {
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_BUSY;

    /* Disable RNG */
    __HAL_RNG_DISABLE(hrng);

    /* RNG CR register configuration. Set value in CR register for :
        - NIST Compliance setting
        - Clock divider value
        - CONFIG 1, CONFIG 2 and CONFIG 3 values */

    cr_value = (uint32_t)(pConf->ClockDivider | pConf->NistCompliance
                          | (pConf->Config1 << RNG_CR_RNG_CONFIG1_Pos)
                          | (pConf->Config2 << RNG_CR_RNG_CONFIG2_Pos)
                          | (pConf->Config3 << RNG_CR_RNG_CONFIG3_Pos));

    MODIFY_REG(hrng->Instance->CR, RNG_CR_NISTC | RNG_CR_CLKDIV | RNG_CR_RNG_CONFIG1
               | RNG_CR_RNG_CONFIG2 | RNG_CR_RNG_CONFIG3,
               (uint32_t)(RNG_CR_CONDRST | cr_value));

#if defined(RNG_VER_3_2) || defined(RNG_VER_3_1) || defined(RNG_VER_3_0)
    /*!< magic number must be written immediately before to RNG_HTCRG */
    WRITE_REG(hrng->Instance->HTCR, RNG_HTCFG_1);
    /* for best latency and to be compliant with NIST */
    WRITE_REG(hrng->Instance->HTCR, RNG_HTCFG);
#endif

    /* Writing bits CONDRST=0*/
    CLEAR_BIT(hrng->Instance->CR, RNG_CR_CONDRST);
    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait for conditioning reset process to be completed */
    while (HAL_IS_BIT_SET(hrng->Instance->CR, RNG_CR_CONDRST))
    {
      if ((HAL_GetTick() - tickstart) > RNG_TIMEOUT_VALUE)
      {
        hrng->State = HAL_RNG_STATE_READY;
        hrng->ErrorCode = HAL_RNG_ERROR_TIMEOUT;
        return HAL_ERROR;
      }
    }

    /* Enable RNG */
    __HAL_RNG_ENABLE(hrng);

    /* Initialize the RNG state */
    hrng->State = HAL_RNG_STATE_READY;

    /* function status */
    status = HAL_OK;
  }
  else
  {
    hrng->ErrorCode = HAL_RNG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  /* Return the function status */
  return status;
}

/**
  * @brief  Get the RNG Configuration and fill parameters in the
  *         RNG_ConfigTypeDef.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *          the configuration information for RNG.
  * @param  pConf: pointer to a RNG_ConfigTypeDef structure that contains
  *         the configuration information for RNG module

  * @retval HAL status
*/
HAL_StatusTypeDef HAL_RNGEx_GetConfig(RNG_HandleTypeDef *hrng, RNG_ConfigTypeDef *pConf)
{

  HAL_StatusTypeDef status ;

  /* Check the RNG handle allocation */
  if ((hrng == NULL) || (pConf == NULL))
  {
    return HAL_ERROR;
  }

  /* Check RNG peripheral state */
  if (hrng->State == HAL_RNG_STATE_READY)
  {
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_BUSY;

    /* Get  RNG parameters  */
    pConf->Config1        = (uint32_t)((hrng->Instance->CR & RNG_CR_RNG_CONFIG1) >> RNG_CR_RNG_CONFIG1_Pos) ;
    pConf->Config2        = (uint32_t)((hrng->Instance->CR & RNG_CR_RNG_CONFIG2) >> RNG_CR_RNG_CONFIG2_Pos);
    pConf->Config3        = (uint32_t)((hrng->Instance->CR & RNG_CR_RNG_CONFIG3) >> RNG_CR_RNG_CONFIG3_Pos);
    pConf->ClockDivider   = (hrng->Instance->CR & RNG_CR_CLKDIV);
    pConf->NistCompliance = (hrng->Instance->CR & RNG_CR_NISTC);

    /* Initialize the RNG state */
    hrng->State = HAL_RNG_STATE_READY;

    /* function status */
    status = HAL_OK;
  }
  else
  {
    hrng->ErrorCode |= HAL_RNG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  /* Return the function status */
  return status;
}

/**
  * @brief  RNG current configuration lock.
  * @note   This function allows to lock RNG peripheral configuration.
  *         Once locked, HW RNG reset has to be perfomed prior any further
  *         configuration update.
  * @param  hrng pointer to a RNG_HandleTypeDef structure that contains
  *                the configuration information for RNG.
  * @retval HAL status
*/
HAL_StatusTypeDef HAL_RNGEx_LockConfig(RNG_HandleTypeDef *hrng)
{
  HAL_StatusTypeDef status;

  /* Check the RNG handle allocation */
  if (hrng == NULL)
  {
    return HAL_ERROR;
  }

  /* Check RNG peripheral state */
  if (hrng->State == HAL_RNG_STATE_READY)
  {
    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_BUSY;

    /* Perform RNG configuration Lock */
    MODIFY_REG(hrng->Instance->CR, RNG_CR_CONFIGLOCK, RNG_CR_CONFIGLOCK);

    /* Change RNG peripheral state */
    hrng->State = HAL_RNG_STATE_READY;

    /* function status */
    status = HAL_OK;
  }
  else
  {
    hrng->ErrorCode = HAL_RNG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  /* Return the function status */
  return status;
}


/**
  * @}
  */

/**
  * @}
  */

#endif  /* CONDRST */
#endif /* HAL_RNG_MODULE_ENABLED */
/**
  * @}
  */

#endif /* RNG */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
