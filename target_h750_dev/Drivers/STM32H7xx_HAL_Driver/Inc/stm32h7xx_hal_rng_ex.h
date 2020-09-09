/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rng_ex.h
  * @author  MCD Application Team
  * @brief   Header file of RNG HAL Extension module.
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
#ifndef STM32H7xx_HAL_RNG_EX_H
#define STM32H7xx_HAL_RNG_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

#if defined (RNG)
#if defined (RNG_CR_CONDRST)

/** @defgroup RNGEx RNGEx
  * @brief RNG Extension HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup RNGEx_Exported_Types RNGEx Exported Types
  * @brief RNGEx Exported types
  * @{
  */

/**
  * @brief RNGEX Configuration Structure definition
  */

typedef struct
{
  uint32_t        Config1;           /*!< Config1 must be a value between 0 and 0x3F */
  uint32_t        Config2;           /*!< Config2 must be a value between 0 and 0x7 */
  uint32_t        Config3;           /*!< Config3 must be a value between 0 and 0xF */
  uint32_t        ClockDivider;      /*!< Clock Divider factor.This parameter can
                                          be a value of @ref RNGEX_Clock_Divider_Factor   */
  uint32_t        NistCompliance;    /*!< NIST compliance.This parameter can be a
                                          value of @ref RNGEX_NIST_Compliance   */
} RNG_ConfigTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RNGEX_Exported_Constants RNGEX Exported Constants
  * @{
  */

/** @defgroup RNGEX_Clock_Divider_Factor  Value used to configure an internal
 *            programmable divider acting on the incoming RNG clock
  * @{
  */
#define RNG_CLKDIV_BY_1       (0x00000000UL)      /*!< No clock division  */
#define RNG_CLKDIV_BY_2       (RNG_CR_CLKDIV_0)
/*!< 2 RNG clock cycles per internal RNG clock    */
#define RNG_CLKDIV_BY_4       (RNG_CR_CLKDIV_1)
/*!< 4 RNG clock cycles per internal RNG clock    */
#define RNG_CLKDIV_BY_8       (RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)
/*!< 8 RNG clock cycles per internal RNG clock    */
#define RNG_CLKDIV_BY_16      (RNG_CR_CLKDIV_2)
/*!< 16 RNG clock cycles per internal RNG clock   */
#define RNG_CLKDIV_BY_32      (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_0)
/*!< 32 RNG clock cycles per internal RNG clock   */
#define RNG_CLKDIV_BY_64      (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1)
/*!< 64 RNG clock cycles per internal RNG clock   */
#define RNG_CLKDIV_BY_128     (RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)
/*!< 128 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_256     (RNG_CR_CLKDIV_3)
/*!< 256 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_512     (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_0)
/*!< 512 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_1024    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_1)
/*!< 1024 RNG clock cycles per internal RNG clock */
#define RNG_CLKDIV_BY_2048    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)
/*!< 2048 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_4096    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2)
/*!< 4096 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_8192    (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_0)
/*!< 8192 RNG clock cycles per internal RNG clock  */
#define RNG_CLKDIV_BY_16384   (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1)
/*!< 16384 RNG clock cycles per internal RNG clock */
#define RNG_CLKDIV_BY_32768   (RNG_CR_CLKDIV_3 | RNG_CR_CLKDIV_2 | RNG_CR_CLKDIV_1 | RNG_CR_CLKDIV_0)
/*!< 32768 RNG clock cycles per internal RNG clock */
/**
  * @}
  */

/** @defgroup RNGEX_NIST_Compliance  NIST Compliance configuration
  * @{
  */
#define RNG_NIST_COMPLIANT     (0x00000000UL) /*!< NIST compliant configuration*/
#define RNG_CUSTOM_NIST        (RNG_CR_NISTC) /*!< Custom NIST configuration */

/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup RNGEx_Private_Types RNGEx Private Types
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup RNGEx_Private_Variables RNGEx Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup RNGEx_Private_Constants RNGEx Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RNGEx_Private_Macros RNGEx Private Macros
  * @{
  */

#define IS_RNG_CLOCK_DIVIDER(__CLOCK_DIV__) (((__CLOCK_DIV__) == RNG_CLKDIV_BY_1)     || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_2)     || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_4)     || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_8)     || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_16)    || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_32)    || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_64)    || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_128)   || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_256)   || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_512)   || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_1024)  || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_2048)  || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_4096)  || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_8192)  || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_16384) || \
                                             ((__CLOCK_DIV__) == RNG_CLKDIV_BY_32768))


#define IS_RNG_NIST_COMPLIANCE(__NIST_COMPLIANCE__) (((__NIST_COMPLIANCE__) == RNG_NIST_COMPLIANT) || \
                                                     ((__NIST_COMPLIANCE__) == RNG_CUSTOM_NIST))

#define IS_RNG_CONFIG1(__CONFIG1__) ((__CONFIG1__) <= 0x3FUL)

#define IS_RNG_CONFIG2(__CONFIG2__) ((__CONFIG2__) <= 0x07UL)

#define IS_RNG_CONFIG3(__CONFIG3__) ((__CONFIG3__) <= 0xFUL)


/**
 * @}
 */

/* Private functions ---------------------------------------------------------*/
/** @defgroup RNGEx_Private_Functions RNGEx Private Functions
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RNGEx_Exported_Functions RNGEx Exported Functions
  * @{
  */

/** @addtogroup RNGEx_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_RNGEx_SetConfig(RNG_HandleTypeDef *hrng, RNG_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_RNGEx_GetConfig(RNG_HandleTypeDef *hrng, RNG_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_RNGEx_LockConfig(RNG_HandleTypeDef *hrng);

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

#endif  /* CONDRST */
#endif /* RNG */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* STM32H7xx_HAL_RNGEX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
