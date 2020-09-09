/**
  ******************************************************************************
  * @file    stm32h7xx_hal_irda_ex.h
  * @author  MCD Application Team
  * @brief   Header file of IRDA HAL Extended module.
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
#ifndef STM32H7xx_HAL_IRDA_EX_H
#define STM32H7xx_HAL_IRDA_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup IRDAEx IRDAEx
  * @brief IRDA Extended HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup IRDAEx_Extended_Exported_Constants IRDAEx Extended Exported Constants
  * @{
  */

/** @defgroup IRDAEx_Word_Length IRDAEx Word Length
  * @{
  */
#define IRDA_WORDLENGTH_7B                  USART_CR1_M1   /*!< 7-bit long frame */
#define IRDA_WORDLENGTH_8B                  0x00000000U    /*!< 8-bit long frame */
#define IRDA_WORDLENGTH_9B                  USART_CR1_M0   /*!< 9-bit long frame */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/** @defgroup IRDAEx_Private_Macros IRDAEx Private Macros
  * @{
  */

/** @brief  Report the IRDA clock source.
  * @param  __HANDLE__ specifies the IRDA Handle.
  * @param  __CLOCKSOURCE__ output variable.
  * @retval IRDA clocking source, written in __CLOCKSOURCE__.
  */
#if defined(UART9) && defined(USART10)
#define IRDA_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__)       \
  do {                                                        \
    if((__HANDLE__)->Instance == USART1)                      \
    {                                                         \
      switch(__HAL_RCC_GET_USART1_SOURCE())                   \
      {                                                       \
        case RCC_USART1CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART1CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART2)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART2_SOURCE())                   \
      {                                                       \
        case RCC_USART2CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART2CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART3)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART3_SOURCE())                   \
      {                                                       \
        case RCC_USART3CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART3CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART4)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART4_SOURCE())                    \
      {                                                       \
        case RCC_UART4CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART4CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if ((__HANDLE__)->Instance == UART5)                 \
    {                                                         \
      switch(__HAL_RCC_GET_UART5_SOURCE())                    \
      {                                                       \
        case RCC_UART5CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART5CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART6)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART6_SOURCE())                   \
      {                                                       \
        case RCC_USART6CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART6CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART7)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART7_SOURCE())                    \
      {                                                       \
        case RCC_UART7CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART7CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART8)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART8_SOURCE())                    \
      {                                                       \
        case RCC_UART8CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART8CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART9)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART9_SOURCE())                    \
      {                                                       \
        case RCC_UART9CLKSOURCE_D2PCLK2:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_UART9CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART9CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART9CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART9CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART9CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART10)                \
    {                                                         \
      switch(__HAL_RCC_GET_USART10_SOURCE())                  \
      {                                                       \
        case RCC_USART10CLKSOURCE_D2PCLK2:                    \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART10CLKSOURCE_PLL2:                       \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART10CLKSOURCE_PLL3:                       \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART10CLKSOURCE_HSI:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART10CLKSOURCE_CSI:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART10CLKSOURCE_LSE:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else                                                      \
    {                                                         \
      (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;         \
    }                                                         \
  } while(0U)
#else
#define IRDA_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__)       \
  do {                                                        \
    if((__HANDLE__)->Instance == USART1)                      \
    {                                                         \
      switch(__HAL_RCC_GET_USART1_SOURCE())                   \
      {                                                       \
        case RCC_USART1CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART1CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART2)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART2_SOURCE())                   \
      {                                                       \
        case RCC_USART2CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART2CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART3)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART3_SOURCE())                   \
      {                                                       \
        case RCC_USART3CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART3CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART4)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART4_SOURCE())                    \
      {                                                       \
        case RCC_UART4CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART4CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if ((__HANDLE__)->Instance == UART5)                 \
    {                                                         \
      switch(__HAL_RCC_GET_UART5_SOURCE())                    \
      {                                                       \
        case RCC_UART5CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART5CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART6)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART6_SOURCE())                   \
      {                                                       \
        case RCC_USART6CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_USART6CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART7)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART7_SOURCE())                    \
      {                                                       \
        case RCC_UART7CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART7CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART8)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART8_SOURCE())                    \
      {                                                       \
        case RCC_UART8CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL2Q;         \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_PLL3Q;         \
          break;                                              \
        case RCC_UART8CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else                                                      \
    {                                                         \
      (__CLOCKSOURCE__) = IRDA_CLOCKSOURCE_UNDEFINED;         \
    }                                                         \
  } while(0U)
#endif /* UART9 && USART10 */

/** @brief  Compute the mask to apply to retrieve the received data
  *         according to the word length and to the parity bits activation.
  * @param  __HANDLE__ specifies the IRDA Handle.
  * @retval None, the mask to apply to the associated UART RDR register is stored in (__HANDLE__)->Mask field.
  */
#define IRDA_MASK_COMPUTATION(__HANDLE__)                             \
  do {                                                                \
    if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_9B)          \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x01FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_8B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == IRDA_WORDLENGTH_7B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == IRDA_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x003FU ;                                \
      }                                                               \
    }                                                                 \
    else                                                              \
    {                                                                 \
      (__HANDLE__)->Mask = 0x0000U;                                   \
    }                                                                 \
  } while(0U)

/** @brief Ensure that IRDA frame length is valid.
  * @param __LENGTH__ IRDA frame length.
  * @retval SET (__LENGTH__ is valid) or RESET (__LENGTH__ is invalid)
  */
#define IS_IRDA_WORD_LENGTH(__LENGTH__) (((__LENGTH__) == IRDA_WORDLENGTH_7B) || \
                                         ((__LENGTH__) == IRDA_WORDLENGTH_8B) || \
                                         ((__LENGTH__) == IRDA_WORDLENGTH_9B))
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_IRDA_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
