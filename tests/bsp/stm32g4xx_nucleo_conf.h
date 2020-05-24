/**
  ******************************************************************************
  * @file    stm32g4xx_nucleo_conf_template.h
  * @author  MCD Application Team
  * @brief   STM32G4xx_Nucleo board configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef STM32G4XX_NUCLEO_CONF_H
#define STM32G4XX_NUCLEO_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G4XX_NUCLEO
  * @{
  */

/** @defgroup STM32G4XX_NUCLEO_CONFIG Config
  * @{
  */

/** @defgroup STM32G4XX_NUCLEO_CONFIG_Exported_Constants Exported Constants
  * @{
  */
/* Uncomment one of the board define below */
/* #define USE_NUCLEO_32 */
#define USE_NUCLEO_64

/* COM usage define */
#define USE_BSP_COM_FEATURE                 0U

/* COM log define */
#define USE_COM_LOG                         0U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY         15U
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

#endif /* STM32G4XX_NUCLEO_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
