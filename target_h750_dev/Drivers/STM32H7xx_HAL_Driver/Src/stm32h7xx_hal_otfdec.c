/**
  ******************************************************************************
  * @file    stm32h7xx_hal_otfdec.c
  * @author  MCD Application Team
  * @brief   OTFDEC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the On-The-Fly Decryption (OTFDEC) peripheral:
  *           + Initialization and de-initialization functions
  *           + Region setting/enable functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
 [..]
    The OTFDEC HAL driver can be used as follows:

    (#) Declare an OTFDEC_HandleTypeDef handle structure (eg. OTFDEC_HandleTypeDef hotfdec).

    (#) Initialize the OTFDEC low level resources by implementing the HAL_OTFDEC_MspInit() API:
        (++) Enable the OTFDEC interface clock.
        (++) NVIC configuration if interrupts are used
            (+++) Configure the OTFDEC interrupt priority.
            (+++) Enable the NVIC OTFDEC IRQ handle.

    (#) Initialize the OTFDEC peripheral by calling the HAL_OTFDEC_Init() API.

    (#) For each region,

        (++) Configure the region deciphering mode by calling the HAL_OTFDEC_RegionSetMode() API.

        (++) Write the region Key by calling the HAL_OTFDEC_RegionSetKey() API. If desired,
        read the key CRC by calling HAL_OTFDEC_RegionGetKeyCRC() API and compare the
        result with the theoretically expected CRC.

        (++) Initialize the OTFDEC region config structure with the Nonce, protected
        region start and end addresses and firmware version, and wrap-up the region
        configuration by calling HAL_OTFDEC_RegionConfig() API.

    (#) At this point, the OTFDEC region configuration is done and the deciphering
        is enabled. The region can be deciphered on the fly after having made sure
        the OctoSPI is configured in memory-mapped mode.

    [..]
    (@) Warning: the OTFDEC deciphering is based on a different endianness compared
        to the AES-CTR as implemented in the AES peripheral. E.g., if the OTFEC
        resorts to the Key (B0, B1, B2, B3) where Bi are 32-bit longwords and B0
        is the Least Significant Word, the AES has to be configured with the Key
        (B3, B2, B1, B0) to report the same result (with the same swapping applied
        to the Initialization Vector).

    [..]

    *** Callback registration ***
    =============================================
    [..]

     The compilation flag USE_HAL_OTFDEC_REGISTER_CALLBACKS, when set to 1,
     allows the user to configure dynamically the driver callbacks.
     Use Functions @ref HAL_OTFDEC_RegisterCallback()
     to register an interrupt callback.
    [..]

     Function @ref HAL_OTFDEC_RegisterCallback() allows to register following callbacks:
       (+) ErrorCallback                  : OTFDEC error callback
       (+) MspInitCallback                : OTFDEC Msp Init callback
       (+) MspDeInitCallback              : OTFDEC Msp DeInit callback
     This function takes as parameters the HAL peripheral handle, the Callback ID
     and a pointer to the user callback function.
    [..]

     Use function @ref HAL_OTFDEC_UnRegisterCallback to reset a callback to the default
     weak function.
    [..]

     @ref HAL_OTFDEC_UnRegisterCallback takes as parameters the HAL peripheral handle,
     and the Callback ID.
     This function allows to reset following callbacks:
       (+) ErrorCallback                  : OTFDEC error callback
       (+) MspInitCallback                : OTFDEC Msp Init callback
       (+) MspDeInitCallback              : OTFDEC Msp DeInit callback
     [..]

     By default, after the @ref HAL_OTFDEC_Init() and when the state is @ref HAL_OTFDEC_STATE_RESET
     all callbacks are set to the corresponding weak functions:
     example @ref HAL_OTFDEC_ErrorCallback().
     Exception done for MspInit and MspDeInit functions that are
     reset to the legacy weak functions in the @ref HAL_OTFDEC_Init()/ @ref HAL_OTFDEC_DeInit() only when
     these callbacks are null (not registered beforehand).
    [..]

     If MspInit or MspDeInit are not null, the @ref HAL_OTFDEC_Init()/ @ref HAL_OTFDEC_DeInit()
     keep and use the user MspInit/MspDeInit callbacks (registered beforehand) whatever the state.
     [..]

     Callbacks can be registered/unregistered in @ref HAL_OTFDEC_STATE_READY state only.
     Exception done MspInit/MspDeInit functions that can be registered/unregistered
     in @ref HAL_OTFDEC_STATE_READY or @ref HAL_OTFDEC_STATE_RESET state,
     thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
    [..]

     Then, the user first registers the MspInit/MspDeInit user callbacks
     using @ref HAL_OTFDEC_RegisterCallback() before calling @ref HAL_OTFDEC_DeInit()
     or @ref HAL_OTFDEC_Init() function.
     [..]

     When the compilation flag USE_HAL_OTFDEC_REGISTER_CALLBACKS is set to 0 or
     not defined, the callback registration feature is not available and all callbacks
     are set to the corresponding weak functions.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
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

/** @defgroup OTFDEC OTFDEC
  * @brief OTFDEC HAL module driver.
  * @{
  */


#ifdef HAL_OTFDEC_MODULE_ENABLED

#if defined(OTFDEC1)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



 /* Exported functions --------------------------------------------------------*/
/** @addtogroup OTFDEC_Exported_Functions
  * @{
  */

/** @defgroup OTFDEC_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions.
 *
@verbatim
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the OTFDEC peripheral and create the associated handle.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_OTFDEC_Init(OTFDEC_HandleTypeDef *hotfdec)
{
  /* Check the OTFDEC handle allocation */
  if(hotfdec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));

  if(hotfdec->State == HAL_OTFDEC_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    __HAL_UNLOCK(hotfdec);

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
    /* Init the OTFDEC Callback settings */
    hotfdec->ErrorCallback = HAL_OTFDEC_ErrorCallback; /* Legacy weak callback */

    if (hotfdec->MspInitCallback == NULL)
    {
      hotfdec->MspInitCallback = HAL_OTFDEC_MspInit; /* Legacy weak MspInit */
    }

    /* Init the low level hardware */
    hotfdec->MspInitCallback(hotfdec);
#else
    /* Init the low level hardware */
    HAL_OTFDEC_MspInit(hotfdec);
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */
  }

  /* Change the OTFDEC state */
  hotfdec->State = HAL_OTFDEC_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitialize the OTFDEC peripheral.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_OTFDEC_DeInit(OTFDEC_HandleTypeDef *hotfdec)
{
  /* Check the OTFDEC handle allocation */
  if(hotfdec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));

  /* Change the OTFDEC state */
  hotfdec->State = HAL_OTFDEC_STATE_BUSY;

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
  if (hotfdec->MspDeInitCallback == NULL)
  {
    hotfdec->MspDeInitCallback = HAL_OTFDEC_MspDeInit; /* Legacy weak MspDeInit */
  }

  /* DeInit the low level hardware: CLOCK, NVIC */
  hotfdec->MspDeInitCallback(hotfdec);
#else
  /* DeInit the low level hardware: CLOCK, NVIC */
  HAL_OTFDEC_MspDeInit(hotfdec);
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */

  /* Change the OTFDEC state */
  hotfdec->State = HAL_OTFDEC_STATE_RESET;

  /* Reset OTFDEC error status */
  hotfdec->ErrorCode = HAL_OTFDEC_ERROR_NONE;

  /* Release Lock */
  __HAL_UNLOCK(hotfdec);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initialize the OTFDEC MSP.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
__weak void HAL_OTFDEC_MspInit(OTFDEC_HandleTypeDef *hotfdec)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hotfdec);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_OTFDEC_MspInit can be implemented in the user file.
   */
}

/**
  * @brief  DeInitialize OTFDEC MSP.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
__weak void HAL_OTFDEC_MspDeInit(OTFDEC_HandleTypeDef *hotfdec)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hotfdec);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_OTFDEC_MspDeInit can be implemented in the user file.
   */
}

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User OTFDEC Callback
  *         To be used instead of the weak predefined callback
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_OTFDEC_ERROR_CB_ID           OTFDEC error callback ID
  *          @arg @ref HAL_OTFDEC_MSPINIT_CB_ID         MspInit callback ID
  *          @arg @ref HAL_OTFDEC_MSPDEINIT_CB_ID       MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_OTFDEC_RegisterCallback(OTFDEC_HandleTypeDef *hotfdec, HAL_OTFDEC_CallbackIDTypeDef CallbackID, pOTFDEC_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (hotfdec->State == HAL_OTFDEC_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_OTFDEC_ERROR_CB_ID :
        hotfdec->ErrorCallback = pCallback;
        break;

      case HAL_OTFDEC_MSPINIT_CB_ID :
        hotfdec->MspInitCallback = pCallback;
        break;

      case HAL_OTFDEC_MSPDEINIT_CB_ID :
        hotfdec->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status = HAL_ERROR;
        break;
    }
  }
  else if (HAL_OTFDEC_STATE_RESET == hotfdec->State)
  {
    switch (CallbackID)
    {
      case HAL_OTFDEC_MSPINIT_CB_ID :
        hotfdec->MspInitCallback = pCallback;
        break;

      case HAL_OTFDEC_MSPDEINIT_CB_ID :
        hotfdec->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a OTFDEC Callback
  *         OTFDEC callback is redirected to the weak predefined callback
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_OTFDEC_ERROR_CB_ID           OTFDEC error callback ID
  *          @arg @ref HAL_OTFDEC_MSPINIT_CB_ID         MspInit callback ID
  *          @arg @ref HAL_OTFDEC_MSPDEINIT_CB_ID       MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_OTFDEC_UnRegisterCallback(OTFDEC_HandleTypeDef *hotfdec, HAL_OTFDEC_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hotfdec->State == HAL_OTFDEC_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_OTFDEC_ERROR_CB_ID :
        hotfdec->ErrorCallback = HAL_OTFDEC_ErrorCallback;
        break;

      case HAL_OTFDEC_MSPINIT_CB_ID :
        hotfdec->MspInitCallback = HAL_OTFDEC_MspInit; /* Legacy weak MspInit */
        break;

      case HAL_OTFDEC_MSPDEINIT_CB_ID :
        hotfdec->MspDeInitCallback = HAL_OTFDEC_MspDeInit; /* Legacy weak MspDeInit */
        break;

      default :
        /* Update the error code */
        hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_OTFDEC_STATE_RESET == hotfdec->State)
  {
    switch (CallbackID)
    {
      case HAL_OTFDEC_MSPINIT_CB_ID :
        hotfdec->MspInitCallback = HAL_OTFDEC_MspInit; /* Legacy weak MspInit */
        break;

      case HAL_OTFDEC_MSPDEINIT_CB_ID :
        hotfdec->MspDeInitCallback = HAL_OTFDEC_MspDeInit; /* Legacy weak MspDeInit */
        break;

      default :
        /* Update the error code */
        hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hotfdec->ErrorCode |= HAL_OTFDEC_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup OTFDEC_Exported_Functions_Group2  OTFDEC IRQ handler management
 *  @brief   OTFDEC IRQ handler.
 *
@verbatim
  ==============================================================================
                ##### OTFDEC IRQ handler management #####
  ==============================================================================
[..]  This section provides OTFDEC IRQ handler function.

@endverbatim
  * @{
  */

/**
  * @brief  Handle OTFDEC interrupt request.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
void HAL_OTFDEC_IRQHandler(OTFDEC_HandleTypeDef *hotfdec)
{
  uint32_t isr_reg;

  isr_reg = READ_REG(hotfdec->Instance->ISR);
  if ((isr_reg & OTFDEC_ISR_SEIF) == OTFDEC_ISR_SEIF)
  {
    SET_BIT( hotfdec->Instance->ICR, OTFDEC_ICR_SEIF );
    hotfdec->ErrorCode |= HAL_OTFDEC_SECURITY_ERROR;
  }
  if ((isr_reg & OTFDEC_ISR_XONEIF) == OTFDEC_ISR_XONEIF)
  {
    SET_BIT( hotfdec->Instance->ICR, OTFDEC_ICR_XONEIF );
    hotfdec->ErrorCode |= HAL_OTFDEC_EXECUTE_ERROR;
  }
  if ((isr_reg & OTFDEC_ISR_KEIF) == OTFDEC_ISR_KEIF)
  {
    SET_BIT( hotfdec->Instance->ICR, OTFDEC_ICR_KEIF );
    hotfdec->ErrorCode |= HAL_OTFDEC_KEY_ERROR;
  }

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
  hotfdec->ErrorCallback(hotfdec);
#else
  HAL_OTFDEC_ErrorCallback(hotfdec);
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */
}

/**
  * @brief OTFDEC error callback.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
__weak void HAL_OTFDEC_ErrorCallback(OTFDEC_HandleTypeDef *hotfdec)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hotfdec);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_OTFDEC_ErrorCallback can be implemented in the user file.
   */
}

/**
  * @}
  */




/** @defgroup OTFDEC_Exported_Functions_Group3 Peripheral Control functions
 *  @brief   Peripheral control functions.
 *
@verbatim
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================
    [..]
    This subsection permits to configure the OTFDEC peripheral

@endverbatim
  * @{
  */

/**
  * @brief  Lock region keys.
  * @note   Writes to this region KEYRx registers are ignored until next OTFDEC reset.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the keys of which are locked
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionKeyLock(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  /* Take Lock */
  __HAL_LOCK(hotfdec);

  address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
  region = (OTFDEC_Region_TypeDef *)address;

  SET_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_KEYLOCK );

  /* Release Lock */
  __HAL_UNLOCK(hotfdec);

  /* Status is okay */
  return HAL_OK;
}

/**
  * @brief  Set region keys.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the keys of which are set
  * @param  pKey pointer at set of keys
  * @note   The API reads the key CRC computed by the peripheral and compares it with thzt
  *         theoretically expected. An error is reported if they are different.
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionSetKey(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, uint32_t *pKey)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  if (pKey == NULL)
  {
    return HAL_ERROR;
  }
  else
  {
    /* Take Lock */
    __HAL_LOCK(hotfdec);

    address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
    region = (OTFDEC_Region_TypeDef *)address;

    /* Set Key */
    WRITE_REG( region->REG_KEYR0, pKey[0]);

    __DSB();
    __ISB();

    WRITE_REG( region->REG_KEYR1, pKey[1]);

    __DSB();
    __ISB();

    WRITE_REG( region->REG_KEYR2, pKey[2]);

    __DSB();
    __ISB();

    WRITE_REG( region->REG_KEYR3, pKey[3]);

    /* Compute theoretically expected CRC and compare it with that reported by the peripheral */
    if (HAL_OTFDEC_KeyCRCComputation(pKey) != HAL_OTFDEC_RegionGetKeyCRC(hotfdec, RegionIndex))
    {
      /* Release Lock */
      __HAL_UNLOCK(hotfdec);

      /* Status is okay */
      return HAL_ERROR;
    }

    /* Release Lock */
    __HAL_UNLOCK(hotfdec);

    /* Status is okay */
    return HAL_OK;
  }
}

/**
  * @brief  Set region mode.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the mode of which is set
  * @param  mode This parameter can be only:
  *           @arg @ref OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY             Only instruction accesses are decrypted
  *           @arg @ref OTFDEC_REG_MODE_DATA_ACCESSES_ONLY                    Only data accesses are decrypted
  *           @arg @ref OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES          All read accesses are decrypted (instruction or data)
  *           @arg @ref OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY_WITH_CIPHER Only instruction accesses are decrypted with proprietary cipher activated
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionSetMode(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, uint32_t mode)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));
  assert_param(IS_OTFDEC_REGION_OPERATING_MODE(mode));

  /* Take Lock */
  __HAL_LOCK(hotfdec);

  address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
  region = (OTFDEC_Region_TypeDef *)address;

  /* Set mode */
  MODIFY_REG(region->REG_CONFIGR, OTFDEC_REG_CONFIGR_MODE, mode);

  /* Release Lock */
  __HAL_UNLOCK(hotfdec);

  /* Status is okay */
  return HAL_OK;
}

/**
  * @brief  Set region configuration.
  * @note  Region deciphering is enabled at the end of this function
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region that is configured
  * @param  Config pointer on structure containing the region configuration parameters
  * @param  lock configuration lock enable or disable parameter
  *         This parameter can be one of the following values:
  *          @arg @ref OTFDEC_REG_CONFIGR_LOCK_DISABLE      OTFDEC region configuration is not locked
  *          @arg @ref OTFDEC_REG_CONFIGR_LOCK_ENABLE       OTFDEC region configuration is locked
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionConfig(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, OTFDEC_RegionConfigTypeDef *Config, uint32_t lock)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));
  assert_param(IS_OTFDEC_REGION_CONFIG_LOCK(lock));

  if (Config == NULL)
  {
    return HAL_ERROR;
  }
  else
  {

    /* Take Lock */
    __HAL_LOCK(hotfdec);

    address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
    region = (OTFDEC_Region_TypeDef *)address;

    /* Set Nonce */
    WRITE_REG( region->REG_NONCER0, Config->Nonce[0]);

    WRITE_REG( region->REG_NONCER1, Config->Nonce[1]);

    /* Write region protected area start and end addresses */
    WRITE_REG( region->REG_START_ADDR, Config->StartAddress);

    WRITE_REG( region->REG_END_ADDR, Config->EndAddress);

    /* Write Version */
    MODIFY_REG( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_VERSION, (uint32_t)(Config->Version) << OTFDEC_REG_CONFIGR_VERSION_Pos );

    /* Enable region deciphering or enciphering (depending of OTFDEC_CR ENC bit setting) */
    SET_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_REG_ENABLE);

    /* Lock the region configuration according to lock parameter value */
    if (lock == OTFDEC_REG_CONFIGR_LOCK_ENABLE)
    {
      SET_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_LOCK_ENABLE);
    }

    /* Release Lock */
    __HAL_UNLOCK(hotfdec);

    /* Status is okay */
    return HAL_OK;
  }
}


/**
  * @brief  Compute Key CRC
  * @param  pKey pointer at set of keys
  * @retval CRC value
  */
uint32_t HAL_OTFDEC_KeyCRCComputation(uint32_t *pKey)
{
  uint8_t crc7_poly = 0x7;
  uint32_t key_strobe[4] = {0xAA55AA55U, 0x3U, 0x18U, 0xC0U};
  uint8_t  i;
  uint8_t crc = 0;
  uint32_t  j, keyval, k;
  uint32_t * temp = pKey;

  for (j = 0U; j < 4U; j++)
  {
    keyval = *temp;
    temp++;
    if (j == 0U)
    {
      keyval ^= key_strobe[0];
    }
    else
    {
      keyval ^= (key_strobe[j] << 24) | ((uint32_t)crc << 16) | (key_strobe[j] << 8) | crc;
    }

    crc = 0;
    for (i = 0; i < (uint8_t)32; i++)
    {
      k = ((((uint32_t)crc >> 7) ^ ((keyval >> ((uint8_t)31-i))&((uint8_t)0xF)))) & 1U;
      crc <<= 1;
      if (k != 0U)
      {
        crc ^= crc7_poly;
       }
    }

    crc^=(uint8_t)0x55;
  }

  return (uint32_t) crc;
}


/**
  * @brief  Enable region deciphering.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the deciphering is enabled
  * @note   An error is reported when the configuration is locked.
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionEnable(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  /* Take Lock */
  __HAL_LOCK(hotfdec);

  address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
  region = (OTFDEC_Region_TypeDef *)address;

  if (READ_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_LOCK_ENABLE) == OTFDEC_REG_CONFIGR_LOCK_ENABLE)
  {
    /* Configuration is locked, REG_EN bit can't be modified */
    __HAL_UNLOCK(hotfdec);

     return HAL_ERROR;
  }

  /* Enable region processing */
  SET_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_REG_ENABLE);

  /* Release Lock */
  __HAL_UNLOCK(hotfdec);

  /* Status is okay */
  return HAL_OK;
}

/**
  * @brief  Disable region deciphering.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the deciphering is disabled
  * @note   An error is reported when the configuration is locked.
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionDisable(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  /* Take Lock */
  __HAL_LOCK(hotfdec);

  address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
  region = (OTFDEC_Region_TypeDef *)address;

  if (READ_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_LOCK_ENABLE) == OTFDEC_REG_CONFIGR_LOCK_ENABLE)
  {
    /* Configuration is locked, REG_EN bit can't be modified */
    __HAL_UNLOCK(hotfdec);

     return HAL_ERROR;
  }

  /* Disable region processing */
  CLEAR_BIT( region->REG_CONFIGR, OTFDEC_REG_CONFIGR_REG_ENABLE);

  /* Release Lock */
  __HAL_UNLOCK(hotfdec);

  /* Status is okay */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup OTFDEC_Exported_Functions_Group4 Peripheral State and Status functions
 *  @brief   Peripheral State functions.
 *
@verbatim
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the OTFDEC state.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval HAL state
  */
HAL_OTFDEC_StateTypeDef HAL_OTFDEC_GetState(OTFDEC_HandleTypeDef *hotfdec)
{
  return hotfdec->State;
}


/**
  * @brief  Return region keys CRC.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the keys CRC of which is read
  * @retval Key CRC
  */
uint32_t HAL_OTFDEC_RegionGetKeyCRC(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;
  uint32_t keycrc;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
  region = (OTFDEC_Region_TypeDef *)address;

  keycrc = (READ_REG( region->REG_CONFIGR )) & OTFDEC_REG_CONFIGR_KEYCRC;

  keycrc >>= OTFDEC_REG_CONFIGR_KEYCRC_Pos;

  return keycrc;
}

/**
  * @brief  Return region configuration parameters.
  * @param  hotfdec pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  RegionIndex index of region the configuration of which is read
  * @param  Config pointer on structure that will be filled up with the region configuration parameters
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionGetConfig(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, OTFDEC_RegionConfigTypeDef *Config)
{
  OTFDEC_Region_TypeDef * region;
  uint32_t address;

  /* Check the parameters */
  assert_param(IS_OTFDEC_ALL_INSTANCE(hotfdec->Instance));
  assert_param(IS_OTFDEC_REGIONINDEX(RegionIndex));

  if (Config == NULL)
  {
    return HAL_ERROR;
  }
  else
  {
    /* Take Lock */
    __HAL_LOCK(hotfdec);

    address =   (uint32_t)(hotfdec->Instance) + 0x20U + (0x30U * RegionIndex);
    region = (OTFDEC_Region_TypeDef *)address;

    /* Read Nonce */
    Config->Nonce[0] = READ_REG(region->REG_NONCER0);
    Config->Nonce[1] = READ_REG(region->REG_NONCER1);

    /* Read Addresses */
    Config->StartAddress = READ_REG(region->REG_START_ADDR);
    Config->EndAddress = READ_REG(region->REG_END_ADDR);

    /* Read Version */
    Config->Version = (uint16_t)(READ_REG(region->REG_CONFIGR) & OTFDEC_REG_CONFIGR_VERSION) >> OTFDEC_REG_CONFIGR_VERSION_Pos;

    /* Release Lock */
    __HAL_UNLOCK(hotfdec);

    /* Status is okay */
    return HAL_OK;
  }
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* OTFDEC1 */

#endif /* HAL_OTFDEC_MODULE_ENABLED */


/**
  * @}
  */

/**
  * @}
  */
