/**
  ******************************************************************************
  * @file    stm32h7xx_hal_otfdec.h
  * @author  MCD Application Team
  * @brief   Header file of OTFDEC HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32H7xx_HAL_OTFDEC_H
#define __STM32H7xx_HAL_OTFDEC_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(OTFDEC1)
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup OTFDEC
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup OTFDEC_Exported_Types OTFDEC Exported Types
  * @{
  */

/** @defgroup OTFDEC_Exported_Types_Group1 OTFDEC region configuration definitions
  * @{
  */

/**
  * @brief OTFDEC region configuration structure definition
  */
typedef struct
{
  uint32_t          Nonce[2];        /*!< OTFDEC region nonce */

  uint32_t          StartAddress;    /*!< OTFDEC region start address */

  uint32_t          EndAddress;      /*!< OTFDEC region end address */

  uint16_t          Version;         /*!< OTFDEC region firmware version */

}OTFDEC_RegionConfigTypeDef;

/**
  * @}
  */

/** @defgroup OTFDEC_Exported_Types_Group2 OTFDEC Peripheral handle definitions
  * @{
  */

/**
  * @brief OTFDEC states structure definition
  */
typedef enum
{
  HAL_OTFDEC_STATE_RESET             = 0x00U,  /*!< OTFDEC not yet initialized or disabled */
  HAL_OTFDEC_STATE_READY             = 0x01U,  /*!< OTFDEC initialized and ready for use   */
  HAL_OTFDEC_STATE_BUSY              = 0x02U,  /*!< OTFDEC internal processing is ongoing  */
}HAL_OTFDEC_StateTypeDef;

/**
  * @brief OTFDEC handle structure definition
  */
#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
typedef struct __OTFDEC_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */
{
  OTFDEC_TypeDef             *Instance;  /*!< OTFDEC registers base address */

  HAL_OTFDEC_StateTypeDef    State;      /*!< OTFDEC state */

  HAL_LockTypeDef            Lock;       /*!< OTFDEC locking object */

  __IO uint32_t              ErrorCode;  /*!< OTFDEC error code */

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
  void (* ErrorCallback)(struct __OTFDEC_HandleTypeDef *hotfdec);     /*!< OTFDEC error callback */

  void (* MspInitCallback)(struct __OTFDEC_HandleTypeDef *hotfdec);   /*!< OTFDEC Msp Init callback */

  void (* MspDeInitCallback)(struct __OTFDEC_HandleTypeDef *hotfdec); /*!< OTFDEC Msp DeInit callback */
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */

}OTFDEC_HandleTypeDef;

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL OTFDEC Callback ID enumeration definition
  */
typedef enum
{
  HAL_OTFDEC_ERROR_CB_ID       = 0x00U,  /*!< OTFDEC error callback ID      */
  HAL_OTFDEC_MSPINIT_CB_ID     = 0x01U,  /*!< OTFDEC Msp DeInit callback ID */
  HAL_OTFDEC_MSPDEINIT_CB_ID   = 0x02U   /*!< OTFDEC Msp DeInit callback ID */
} HAL_OTFDEC_CallbackIDTypeDef;

/**
  * @brief  HAL OTFDEC Callback pointer definition
  */
typedef  void (*pOTFDEC_CallbackTypeDef)(OTFDEC_HandleTypeDef *hotfdec); /*!< pointer to a OTFDEC callback function */

#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */

/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup OTFDEC_Exported_Constants OTFDEC Exported Constants
  * @{
  */

/** @defgroup OTFDEC_Interrupts    OTFDEC Interrupts
  * @{
  */
#define OTFDEC_SEC_ERROR_INT     (OTFDEC_IER_SEIE                                  )  /*!< OTFDEC security error interrupt                  */
#define OTFDEC_EXE_ERROR_INT     (               OTFDEC_IER_XONEIE                 )  /*!< OTFDEC execution error interrupt                 */
#define OTFDEC_KEY_ERROR_INT     (                                  OTFDEC_IER_KEIE)  /*!< OTFDEC key error interrupt                       */
#define OTFDEC_SEC_EXE_ERROR_INT (OTFDEC_IER_SEIE|OTFDEC_IER_XONEIE                )  /*!< OTFDEC security and execution errors interrupts  */
#define OTFDEC_SEC_KEY_ERROR_INT (OTFDEC_IER_SEIE|                  OTFDEC_IER_KEIE)  /*!< OTFDEC security and key errors interrupts        */
#define OTFDEC_EXE_KEY_ERROR_INT (                OTFDEC_IER_XONEIE|OTFDEC_IER_KEIE)  /*!< OTFDEC execution and key errors interrupts       */
#define OTFDEC_ALL_INT           (OTFDEC_IER_SEIE|OTFDEC_IER_XONEIE|OTFDEC_IER_KEIE)  /*!< OTFDEC all interrupts                            */
/**
  * @}
  */

/** @defgroup OTFDEC_Region_Enable   OTFDEC Region Enable
  * @{
  */
#define OTFDEC_REG_CONFIGR_REG_DISABLE   0x00000000U                /*!< OTFDEC region encryption or on-the-fly decryption disable */
#define OTFDEC_REG_CONFIGR_REG_ENABLE    OTFDEC_REG_CONFIGR_REG_EN  /*!< OTFDEC region encryption or on-the-fly decryption enable  */
/**
  * @}
  */

/** @defgroup OTFDEC_Region_Configuration_Lock    OTFDEC Region Configuration Lock
  * @{
  */
#define OTFDEC_REG_CONFIGR_LOCK_DISABLE   0x00000000U                     /*!< OTFDEC region configuration lock disable */
#define OTFDEC_REG_CONFIGR_LOCK_ENABLE    OTFDEC_REG_CONFIGR_CONFIGLOCK   /*!< OTFDEC region configuration lock enable  */
/**
  * @}
  */

/** @defgroup OTFDEC_Region_Operating_Mode    OTFDEC Region Operating Mode
  * @{
  */
#define OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY              0x00000000U                /*!< Only instruction accesses are decrypted                                   */
#define OTFDEC_REG_MODE_DATA_ACCESSES_ONLY                     OTFDEC_REG_CONFIGR_MODE_0  /*!< Only data accesses are decrypted                                          */
#define OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES           OTFDEC_REG_CONFIGR_MODE_1  /*!< All read accesses are decrypted                                           */
#define OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY_WITH_CIPHER  OTFDEC_REG_CONFIGR_MODE    /*!< Only instruction accesses are decrypted with proprietary cipher activated */
/**
  * @}
  */

/** @defgroup OTFDEC_Error_Definition   OTFDEC Error Definition
  * @{
  */
#define  HAL_OTFDEC_ERROR_NONE             ((uint32_t)0x00000000U)    /*!< No error                         */
#define  HAL_OTFDEC_SECURITY_ERROR         ((uint32_t)0x00000001U)    /*!< Security error                   */
#define  HAL_OTFDEC_EXECUTE_ERROR          ((uint32_t)0x00000002U)    /*!< Execute-only Execute-Never error */
#define  HAL_OTFDEC_KEY_ERROR              ((uint32_t)0x00000004U)    /*!< Key error                        */
#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
#define HAL_OTFDEC_ERROR_INVALID_CALLBACK  ((uint32_t)0x00000008U)   /*!< Invalid Callback error            */
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup OTFDEC_Regions_Index   OTFDEC Regions Index
  * @{
  */
#define  OTFDEC_REGION1            ((uint32_t)0x00000000U)    /*!< OTFDEC region 1 */
#define  OTFDEC_REGION2            ((uint32_t)0x00000001U)    /*!< OTFDEC region 2 */
#define  OTFDEC_REGION3            ((uint32_t)0x00000002U)    /*!< OTFDEC region 3 */
#define  OTFDEC_REGION4            ((uint32_t)0x00000003U)    /*!< OTFDEC region 4 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup OTFDEC_Exported_Macros OTFDEC Exported Macros
  * @{
  */

/** @brief  Reset OTFDEC handle state.
  * @param  __HANDLE__ pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @retval None
  */
#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
#define __HAL_OTFDEC_RESET_HANDLE_STATE(__HANDLE__)                            \
  do{                                                                          \
    (__HANDLE__)->State = HAL_OTFDEC_STATE_RESET;                              \
    (__HANDLE__)->MspInitCallback = NULL;                                      \
    (__HANDLE__)->MspDeInitCallback = NULL;                                    \
  } while(0)
#else
#define __HAL_OTFDEC_RESET_HANDLE_STATE(__HANDLE__)                            \
  ((__HANDLE__)->State = HAL_OTFDEC_STATE_RESET)
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */

/**
  * @brief  Enable OTFDEC peripheral interrupts combination
  * @param  __HANDLE__ pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  __INTERRUPT__ mask on enabled interrupts
  *          This parameter can be one of the following values:
  *            @arg @ref OTFDEC_SEC_ERROR_INT        OTFDEC security error interrupt
  *            @arg @ref OTFDEC_EXE_ERROR_INT        OTFDEC execution error interrupt
  *            @arg @ref OTFDEC_KEY_ERROR_INT        OTFDEC key error interrupt
  *            @arg @ref OTFDEC_SEC_EXE_ERROR_INT    OTFDEC security and execution errors interrupts
  *            @arg @ref OTFDEC_SEC_KEY_ERROR_INT    OTFDEC security and key errors interrupts
  *            @arg @ref OTFDEC_EXE_KEY_ERROR_INT    OTFDEC execution and key errors interrupts
  *            @arg @ref OTFDEC_ALL_INT              OTFDEC all interrupts
  * @retval None
  */
#define __HAL_OTFDEC_ENABLE_IT(__HANDLE__, __INTERRUPT__)  SET_BIT(((__HANDLE__)->Instance->IER), (__INTERRUPT__))

/**
  * @brief  Disable OTFDEC peripheral interrupts combination
  * @param  __HANDLE__ pointer to an OTFDEC_HandleTypeDef structure that contains
  *         the configuration information for OTFDEC module
  * @param  __INTERRUPT__ mask on disabled interrupts
  *          This parameter can be one of the following values:
  *            @arg @ref OTFDEC_SEC_ERROR_INT        OTFDEC security error interrupt
  *            @arg @ref OTFDEC_EXE_ERROR_INT        OTFDEC execution error interrupt
  *            @arg @ref OTFDEC_KEY_ERROR_INT        OTFDEC key error interrupt
  *            @arg @ref OTFDEC_SEC_EXE_ERROR_INT    OTFDEC security and execution errors interrupts
  *            @arg @ref OTFDEC_SEC_KEY_ERROR_INT    OTFDEC security and key errors interrupts
  *            @arg @ref OTFDEC_EXE_KEY_ERROR_INT    OTFDEC execution and key errors interrupts
  *            @arg @ref OTFDEC_ALL_INT              OTFDEC all interrupts
  * @retval None
  */
#define __HAL_OTFDEC_DISABLE_IT(__HANDLE__, __INTERRUPT__)  CLEAR_BIT(((__HANDLE__)->Instance->IER), (__INTERRUPT__))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup OTFDEC_Exported_Functions OTFDEC Exported Functions
  * @{
  */

/** @addtogroup OTFDEC_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_OTFDEC_Init(OTFDEC_HandleTypeDef *hotfdec);
HAL_StatusTypeDef HAL_OTFDEC_DeInit(OTFDEC_HandleTypeDef *hotfdec);
void HAL_OTFDEC_MspInit(OTFDEC_HandleTypeDef *hotfdec);
void HAL_OTFDEC_MspDeInit(OTFDEC_HandleTypeDef *hotfdec);

#if (USE_HAL_OTFDEC_REGISTER_CALLBACKS == 1)
/* Callbacks Register/UnRegister functions  ***********************************/
HAL_StatusTypeDef HAL_OTFDEC_RegisterCallback(OTFDEC_HandleTypeDef *hotfdec, HAL_OTFDEC_CallbackIDTypeDef CallbackID,
                                           pOTFDEC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_OTFDEC_UnRegisterCallback(OTFDEC_HandleTypeDef *hotfdec, HAL_OTFDEC_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_OTFDEC_REGISTER_CALLBACKS */
/**
  * @}
  */


/** @addtogroup OTFDEC_Exported_Functions_Group2 OTFDEC IRQ handler management
  * @{
  */
void HAL_OTFDEC_IRQHandler(OTFDEC_HandleTypeDef *hotfdec);
void HAL_OTFDEC_ErrorCallback(OTFDEC_HandleTypeDef *hotfdec);
/**
  * @}
  */

/** @addtogroup OTFDEC_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_OTFDEC_RegionKeyLock(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_OTFDEC_RegionSetKey(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, uint32_t *pKey);
HAL_StatusTypeDef HAL_OTFDEC_RegionSetMode(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, uint32_t mode);
HAL_StatusTypeDef HAL_OTFDEC_RegionConfig(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, OTFDEC_RegionConfigTypeDef *Config, uint32_t lock);
uint32_t HAL_OTFDEC_KeyCRCComputation(uint32_t *pKey);
HAL_StatusTypeDef HAL_OTFDEC_RegionEnable(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_OTFDEC_RegionDisable(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex);
/**
  * @}
  */

/** @addtogroup @addtogroup OTFDEC_Exported_Functions_Group4 Peripheral State and Status functions
  * @{
  */
HAL_OTFDEC_StateTypeDef HAL_OTFDEC_GetState(OTFDEC_HandleTypeDef *hotfdec);
uint32_t HAL_OTFDEC_RegionGetKeyCRC(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_OTFDEC_RegionGetConfig(OTFDEC_HandleTypeDef *hotfdec, uint32_t RegionIndex, OTFDEC_RegionConfigTypeDef *Config);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup OTFDEC_Private_Types OTFDEC Private Types
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup OTFDEC_Private_Variables OTFDEC Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup OTFDEC_Private_Constants OTFDEC Private Constants
  * @{
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup OTFDEC_Private_Macros OTFDEC Private Macros
  * @{
  */

/**
  * @brief Verify the OTFDEC peripheral interrupts parameter.
  * @param __INT__ OTFDEC peripheral set of interrupts parameter
  * @retval SET (__INT__ is valid) or RESET (__INT__ is invalid)
  */
#define IS_OTFDEC_INTERRUPTS(__INT__) (((__INT__) == OTFDEC_SEC_ERROR_INT)     || \
                                       ((__INT__) == OTFDEC_EXE_ERROR_INT)     || \
                                       ((__INT__) == OTFDEC_KEY_ERROR_INT)     || \
                                       ((__INT__) == OTFDEC_SEC_EXE_ERROR_INT) || \
                                       ((__INT__) == OTFDEC_SEC_KEY_ERROR_INT) || \
                                       ((__INT__) == OTFDEC_EXE_KEY_ERROR_INT) || \
                                       ((__INT__) == OTFDEC_ALL_INT)  )

/**
  * @brief Verify the OTFDEC region configuration lock parameter.
  * @param __LOCK__ OTFDEC region lock parameter.
  * @retval SET (__LOCK__ is valid) or RESET (__LOCK__ is invalid)
  */
#define IS_OTFDEC_REGION_CONFIG_LOCK(__LOCK__) (((__LOCK__) == OTFDEC_REG_CONFIGR_LOCK_DISABLE) || \
                                                ((__LOCK__) == OTFDEC_REG_CONFIGR_LOCK_ENABLE)  )

/**
  * @brief Verify the OTFDEC region operating mode.
  * @param __MODE__ OTFDEC region operating mode parameter.
  * @retval SET (__MODE__ is valid) or RESET (__MODE__ is invalid)
  */
#define IS_OTFDEC_REGION_OPERATING_MODE(__MODE__) (((__MODE__) == OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY)    || \
                                                   ((__MODE__) == OTFDEC_REG_MODE_DATA_ACCESSES_ONLY)           || \
                                                   ((__MODE__) == OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES) || \
                                                   ((__MODE__) == OTFDEC_REG_MODE_INSTRUCTION_ACCESSES_ONLY_WITH_CIPHER))

/**
  * @brief Verify the OTFDEC region index.
  * @param __INDEX__ OTFDEC region index
  * @retval SET (__INDEX__ is valid) or RESET (__INDEX__ is invalid)
  */
#define IS_OTFDEC_REGIONINDEX(__INDEX__) (((__INDEX__) == OTFDEC_REGION1)     || \
                                          ((__INDEX__) == OTFDEC_REGION2)     || \
                                          ((__INDEX__) == OTFDEC_REGION3)     || \
                                          ((__INDEX__) == OTFDEC_REGION4)  )

/**
  * @brief Verify the OTFDEC configuration attributes.
  * @param __ATTRIBUTE__ OTFDEC region index
  * @retval SET (__ATTRIBUTE__ is valid) or RESET (__ATTRIBUTE__ is invalid)
  */
#define IS_OTFDEC_ATTRIBUTE(__ATTRIBUTE__) (((__ATTRIBUTE__) == OTFDEC_ATTRIBUTE_PRIV)   || \
                                            ((__ATTRIBUTE__) == OTFDEC_ATTRIBUTE_NPRIV)  )

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup OTFDEC_Private_Functions OTFDEC Private Functions
  * @{
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
#endif /* OTFDEC1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_HAL_OTFDEC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

