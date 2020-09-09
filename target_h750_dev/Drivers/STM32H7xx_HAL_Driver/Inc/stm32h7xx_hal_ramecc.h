/**
  ******************************************************************************
  * @file    stm32h7xx_hal_ramecc.h
  * @author  MCD Application Team
  * @brief   Header file of RAMECC HAL module.
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
#ifndef STM32H7xx_HAL_RAMECC_H
#define STM32H7xx_HAL_RAMECC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup RAMECC
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup RAMECC_Exported_Types RAMECC Exported Types
  * @brief    RAMECC Exported Types
  * @{
  */

/**
  * @brief  HAL RAMECC State structures definition
  */
typedef enum
{
  HAL_RAMECC_STATE_RESET             = 0x00U,  /*!< RAMECC not yet initialized or disabled */
  HAL_RAMECC_STATE_READY             = 0x01U,  /*!< RAMECC initialized and ready for use   */
  HAL_RAMECC_STATE_BUSY              = 0x02U,  /*!< RAMECC process is ongoing              */
  HAL_RAMECC_STATE_ERROR             = 0x03U,  /*!< RAMECC error state                     */
}HAL_RAMECC_StateTypeDef;


/**
  * @brief  RAMECC handle Structure definition
  */

typedef struct __RAMECC_HandleTypeDef
{
  RAMECC_MonitorTypeDef           *Instance;                                                            /*!< Register base address        */
  __IO HAL_RAMECC_StateTypeDef    State;                                                                /*!< RAMECC state                 */
  __IO uint32_t                   ErrorCode;                                                            /*!< RAMECC Error Code            */
  void                            (* DetectErrorCallback)( struct __RAMECC_HandleTypeDef *hramecc);     /*!< RAMECC error detect callback */
}RAMECC_HandleTypeDef;

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup RAMECC_Error_Codes RAMECC Error Codes
  * @{
  */
#define HAL_RAMECC_ERROR_NONE              0x00000000U  /*!< RAMECC No Error         */
#define HAL_RAMECC_ERROR_TIMEOUT           0x00000001U  /*!< RAMECC Timeout Error    */
#define HAL_RAMECC_ERROR_BUSY              0x00000002U  /*!< RAMECC Busy Error       */
#define HAL_RAMECC_ERROR_INVALID_CALLBACK  0x00000003U  /*!< Invalid Callback error  */
/**
  * @}
  */

/** @defgroup RAMECC_Interrupt RAMECC interrupts
  * @{
  */
#define RAMECC_IT_GLOBAL_ID                0x10000000UL
#define RAMECC_IT_MONITOR_ID               0x20000000UL

#define RAMECC_IT_GLOBAL_ENABLE            (RAMECC_IT_GLOBAL_ID | RAMECC_IER_GIE)
#define RAMECC_IT_GLOBAL_SINGLEERR_R       (RAMECC_IT_GLOBAL_ID | RAMECC_IER_GECCSEIE)
#define RAMECC_IT_GLOBAL_DOUBLEERR_R       (RAMECC_IT_GLOBAL_ID | RAMECC_IER_GECCDEIE)
#define RAMECC_IT_GLOBAL_DOUBLEERR_W       (RAMECC_IT_GLOBAL_ID | RAMECC_IER_GECCDEBWIE)
#define RAMECC_IT_GLOBAL_ALL               (RAMECC_IT_GLOBAL_ID | RAMECC_IER_GIE | RAMECC_IER_GECCSEIE | RAMECC_IER_GECCDEIE | RAMECC_IER_GECCDEBWIE)


#define RAMECC_IT_MONITOR_SINGLEERR_R      (RAMECC_IT_MONITOR_ID | RAMECC_CR_ECCSEIE)
#define RAMECC_IT_MONITOR_DOUBLEERR_R      (RAMECC_IT_MONITOR_ID | RAMECC_CR_ECCDEIE)
#define RAMECC_IT_MONITOR_DOUBLEERR_W      (RAMECC_IT_MONITOR_ID | RAMECC_CR_ECCDEBWIE)
#define RAMECC_IT_MONITOR_ALL              (RAMECC_IT_MONITOR_ID | RAMECC_CR_ECCDEBWIE | RAMECC_CR_ECCDEIE | RAMECC_CR_ECCSEIE)
/**
  * @}
  */

/** @defgroup RAMECC_FLAG RAMECC Monitor flags
  * @{
  */
#define RAMECC_FLAG_SINGLEERR_R            RAMECC_SR_SEDCF
#define RAMECC_FLAG_DOUBLEERR_R            RAMECC_SR_DEDF
#define RAMECC_FLAG_DOUBLEERR_W            RAMECC_SR_DEBWDF
#define RAMECC_FLAGS_ALL                   (RAMECC_SR_SEDCF | RAMECC_SR_DEDF | RAMECC_SR_DEBWDF)

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup RAMECC_Exported_Macros RAMECC Exported Macros
  * @{
  */

#define __HAL_RAMECC_ENABLE_GLOBAL_IT(__HANDLE__, __INTERRUPT__) ((((RAMECC_TypeDef *)((uint32_t)(__HANDLE__)->Instance & 0xFFFFFF00U))->IER) |= ((__INTERRUPT__) & ~RAMECC_IT_GLOBAL_ID))
#define __HAL_RAMECC_ENABLE_MONITOR_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR |= ((__INTERRUPT__) & ~RAMECC_IT_MONITOR_ID))

/**
  * @brief  Enable the specified RAMECC interrupts.
  * @param  __HANDLE__   : RAMECC handle.
  * @param  __INTERRUPT__: specifies the RAMECC interrupt sources to be enabled or disabled.
  *        This parameter can be one of the following values:
  *           @arg RAMECC_IT_GLOBAL_E      : Global interrupt enable mask.
  *           @arg RAMECC_IT_GLOBAL_SEE    : Global ECC single error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEE    : Global ECC double error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEBWE  : Global ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_ALL    : All Global ECC interrupts enable mask.
  *           @arg RAMECC_IT_MONITOR_SEE   : Monitor ECC single error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEE   : Monitor ECC double error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEBWE : Monitor ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_MONITOR_ALL   : All Monitor ECC interrupts enable mask.
  * @retval None
  */
#define __HAL_RAMECC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ( \
(IS_RAMECC_GLOBAL_INTERRUPT(__INTERRUPT__)) ? (__HAL_RAMECC_ENABLE_GLOBAL_IT((__HANDLE__), (__INTERRUPT__))) :\
(__HAL_RAMECC_ENABLE_MONITOR_IT((__HANDLE__), (__INTERRUPT__))))


#define __HAL_RAMECC_DISABLE_GLOBAL_IT(__HANDLE__, __INTERRUPT__) ((((RAMECC_TypeDef *)((uint32_t)(__HANDLE__)->Instance & 0xFFFFFF00U))->IER) &= ~((__INTERRUPT__) & ~RAMECC_IT_GLOBAL_ID))
#define __HAL_RAMECC_DISABLE_MONITOR_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR &= ~((__INTERRUPT__) & ~RAMECC_IT_MONITOR_ID))

/**
  * @brief  Disable the specified RAMECC interrupts.
  * @param  __HANDLE__   : RAMECC handle.
  * @param  __INTERRUPT__: specifies the RAMECC interrupt sources to be enabled or disabled.
  *        This parameter can be one of the following values:
  *           @arg RAMECC_IT_GLOBAL_E      : Global interrupt enable mask.
  *           @arg RAMECC_IT_GLOBAL_SEE    : Global ECC single error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEE    : Global ECC double error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEBWE  : Global ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_ALL    : All Global ECC interrupts enable mask.
  *           @arg RAMECC_IT_MONITOR_SEE   : Monitor ECC single error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEE   : Monitor ECC double error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEBWE : Monitor ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_MONITOR_ALL   : All Monitor ECC interrupts enable mask.
  * @retval None
  */
#define __HAL_RAMECC_DISABLE_IT(__HANDLE__, __INTERRUPT__) ( \
(IS_RAMECC_GLOBAL_INTERRUPT(__INTERRUPT__)) ? (__HAL_RAMECC_DISABLE_GLOBAL_IT((__HANDLE__), (__INTERRUPT__))) :\
(__HAL_RAMECC_DISABLE_MONITOR_IT((__HANDLE__), (__INTERRUPT__))))


#define __HAL_RAMECC_GET_GLOBAL_IT_SOURCE(__HANDLE__, __INTERRUPT__) (((((RAMECC_TypeDef *)((uint32_t)(__HANDLE__)->Instance & 0xFFFFFF00U))->IER) & ((__INTERRUPT__) & ~RAMECC_IT_GLOBAL_ID)) ? SET : RESET)
#define __HAL_RAMECC_GET_MONITOR_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR) & ((__INTERRUPT__) & ~RAMECC_IT_GLOBAL_ID)) ? SET : RESET)

/**
  * @brief  Check whether the specified RAMECC interrupt source is enabled or not.
  * @param  __HANDLE__    : Specifies the RAMECC Handle.
  * @param  __INTERRUPT__ : Specifies the RAMECC interrupt source to check.
  *          This parameter can be one of the following values:
  *           @arg RAMECC_IT_GLOBAL_E      : Global interrupt enable mask.
  *           @arg RAMECC_IT_GLOBAL_SEE    : Global ECC single error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEE    : Global ECC double error interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_DEBWE  : Global ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_GLOBAL_ALL    : All Global ECC interrupts enable mask.
  *           @arg RAMECC_IT_MONITOR_SEE   : Monitor ECC single error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEE   : Monitor ECC double error interrupt enable.
  *           @arg RAMECC_IT_MONITOR_DEBWE : Monitor ECC double error on byte write (BW) interrupt enable.
  *           @arg RAMECC_IT_MONITOR_ALL   : All Monitor ECC interrupts enable mask.
  * @retval The new state of __INTERRUPT__ (SET or RESET).
  */
#define __HAL_RAMECC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (  \
(IS_RAMECC_GLOBAL_INTERRUPT(__INTERRUPT__)) ? (__HAL_RAMECC_GET_GLOBAL_IT_SOURCE((__HANDLE__), (__INTERRUPT__))) :\
(__HAL_RAMECC_GET_MONITOR_IT_SOURCE((__HANDLE__), (__INTERRUPT__))))


/**
  * @brief  Get the RAMECC pending flags.
  * @param  __HANDLE__   : RAMECC handle.
  * @param  __FLAG__     : specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg RAMECC_FLAG_SEDCF  : RAMECC instance ECC single error detected and corrected flag.
  *            @arg RAMECC_FLAG_DEDF   : RAMECC instance ECC double error detected flag.
  *            @arg RAMECC_FLAG_DEBWDF : RAMECC instance ECC double error on byte write (BW) detected flag.
  *            @arg RAMECC_FLAGS_ALL   : RAMECC instance all flag.
  * @retval The state of __FLAG__ (SET or RESET).
  */
#define __HAL_RAMECC_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR &= (__FLAG__))


/**
  * @brief  Clear the RAMECC pending flags.
  * @param  __HANDLE__   : RAMECC handle.
  * @param  __FLAG__     : specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg RAMECC_FLAG_SEDCF  : RAMECC instance ECC single error detected and corrected flag.
  *            @arg RAMECC_FLAG_DEDF   : RAMECC instance ECC double error detected flag.
  *            @arg RAMECC_FLAG_DEBWDF : RAMECC instance ECC double error on byte write (BW) detected flag.
  *            @arg RAMECC_FLAGS_ALL   : RAMECC instance all flag.
  * @retval None.
  */
#define __HAL_RAMECC_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR &= ~(__FLAG__))

/**
  * @brief  Reset the RAMECC handle state.
  * @param  __HANDLE__    : Specifies the RAMECC Handle.
  * @retval None.
  */
#define __HAL_RAMECC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_RAMECC_STATE_RESET)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup RAMECC_Exported_Functions RAMECC Exported Functions
  * @brief    RAMECC Exported functions
  * @{
  */

/** @defgroup RAMECC_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_RAMECC_Init   (RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_DeInit (RAMECC_HandleTypeDef *hramecc);
/**
  * @}
  */

/** @defgroup RAMECC_Exported_Functions_Group2 monitoring operation functions
  * @brief    monitoring operation functions
  * @{
  */
HAL_StatusTypeDef HAL_RAMECC_StartMonitor        (RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_StopMonitor         (RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_EnableNotification  (RAMECC_HandleTypeDef *hramecc, uint32_t Notifications);
HAL_StatusTypeDef HAL_RAMECC_DisableNotification (RAMECC_HandleTypeDef *hramecc, uint32_t Notifications);
void              HAL_RAMECC_IRQHandler          (RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_RegisterCallback    (RAMECC_HandleTypeDef *hramecc, void (* pCallback)(RAMECC_HandleTypeDef *_hramecc));
HAL_StatusTypeDef HAL_RAMECC_UnRegisterCallback  (RAMECC_HandleTypeDef *hramecc);
/**
  * @}
  */

/** @defgroup RAMECC_Exported_Functions_Group3 Error informations functions
  * @brief    Error informations functions
  * @{
  */
uint32_t HAL_RAMECC_GetFailingAddress        (RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetFailingDataLow        (RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetFailingDataHigh       (RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetHammingErrorCode      (RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_IsECCSingleErrorDetected (RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_IsECCDoubleErrorDetected (RAMECC_HandleTypeDef *hramecc);
/**
  * @}
  */

/** @defgroup RAMECC_Exported_Functions_Group3 Error informations functions
  * @brief    Error informations functions
  * @{
  */
HAL_RAMECC_StateTypeDef HAL_RAMECC_GetState (RAMECC_HandleTypeDef *hramecc);
uint32_t                HAL_RAMECC_GetError (RAMECC_HandleTypeDef *hramecc);
/**
  * @}
  */

/**
  * @}
  */
/* Private Constants -------------------------------------------------------------*/
/** @defgroup RAMECC_Private_Constants RAMECC Private Constants
  * @brief    RAMECC private defines and constants
  * @{
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RAMECC_Private_Macros RAMECC Private Macros
  * @brief    RAMECC private macros
  * @{
  */

#define IS_RAMECC_GLOBAL_INTERRUPT(INTERRUPT) (((INTERRUPT) == RAMECC_IT_GLOBAL_ENABLE)      || \
                                               ((INTERRUPT) == RAMECC_IT_GLOBAL_SINGLEERR_R) || \
                                               ((INTERRUPT) == RAMECC_IT_GLOBAL_DOUBLEERR_R) || \
                                               ((INTERRUPT) == RAMECC_IT_GLOBAL_DOUBLEERR_W) || \
                                               ((INTERRUPT) == RAMECC_IT_GLOBAL_ALL))


#define IS_RAMECC_MONITOR_INTERRUPT(INTERRUPT) (((INTERRUPT) == RAMECC_IT_MONITOR_SINGLEERR_R) || \
                                                ((INTERRUPT) == RAMECC_IT_MONITOR_DOUBLEERR_R) || \
                                                ((INTERRUPT) == RAMECC_IT_MONITOR_DOUBLEERR_W) || \
                                                ((INTERRUPT) == RAMECC_IT_MONITOR_ALL))

#define IS_RAMECC_INTERRUPT(INTERRUPT) ((IS_RAMECC_GLOBAL_INTERRUPT(INTERRUPT)) || \
                                        (IS_RAMECC_MONITOR_INTERRUPT(INTERRUPT)))

/**
  * @}
  */

/** @defgroup RAMECC_FLAG RAMECC Monitor flags
  * @{
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup RAMECC_Private_Functions RAMECC Private Functions
  * @brief    RAMECC private  functions
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

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_RAMECC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
