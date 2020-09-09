/**
  ******************************************************************************
  * @file    stm32h7xx_ll_delayblock.h
  * @author  MCD Application Team
  * @brief   Header file of Delay Block module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7xx_LL_DLYB_H
#define STM32H7xx_LL_DLYB_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup DELAYBLOCK_LL
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup DELAYBLOCK_LL_Exported_Types DELAYBLOCK_LL Exported Types
  * @{
  */
  

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup DLYB_Exported_Constants Delay Block Exported Constants
  * @{
  */


#define DLYB_MAX_UNIT   ((uint32_t)0x00000080U) /*!< Max UNIT value (128)  */
#define DLYB_MAX_SELECT ((uint32_t)0x0000000CU) /*!< Max SELECT value (12)  */

/**
  * @}
  */ 
 
/* Peripheral Control functions  ************************************************/
/** @addtogroup HAL_DELAYBLOCK_LL_Group3 Delay Block functions
  * @{
  */
HAL_StatusTypeDef DelayBlock_Enable(DLYB_TypeDef *DLYBx);
HAL_StatusTypeDef DelayBlock_Disable(DLYB_TypeDef *DLYBx);
HAL_StatusTypeDef DelayBlock_Configure(DLYB_TypeDef *DLYBx, uint32_t PhaseSel, uint32_t Units);

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

#endif /* STM32H7xx_LL_DLYB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
