/**
  ******************************************************************************
  * @file    stm32h7xx_hal_uart_ex.h
  * @author  MCD Application Team
  * @brief   Header file of UART HAL Extended module.
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
#ifndef STM32H7xx_HAL_UART_EX_H
#define STM32H7xx_HAL_UART_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup UARTEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup UARTEx_Exported_Types UARTEx Exported Types
  * @{
  */

/**
  * @brief  UART wake up from stop mode parameters
  */
typedef struct
{
  uint32_t WakeUpEvent;        /*!< Specifies which event will activate the Wakeup from Stop mode flag (WUF).
                                    This parameter can be a value of @ref UART_WakeUp_from_Stop_Selection.
                                    If set to UART_WAKEUP_ON_ADDRESS, the two other fields below must
                                    be filled up. */

  uint16_t AddressLength;      /*!< Specifies whether the address is 4 or 7-bit long.
                                    This parameter can be a value of @ref UARTEx_WakeUp_Address_Length.  */

  uint8_t Address;             /*!< UART/USART node address (7-bit long max). */
} UART_WakeUpTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup UARTEx_Exported_Constants UARTEx Exported Constants
  * @{
  */

/** @defgroup UARTEx_Word_Length UARTEx Word Length
  * @{
  */
#define UART_WORDLENGTH_7B          USART_CR1_M1   /*!< 7-bit long UART frame */
#define UART_WORDLENGTH_8B          0x00000000U    /*!< 8-bit long UART frame */
#define UART_WORDLENGTH_9B          USART_CR1_M0   /*!< 9-bit long UART frame */
/**
  * @}
  */

/** @defgroup UARTEx_WakeUp_Address_Length UARTEx WakeUp Address Length
  * @{
  */
#define UART_ADDRESS_DETECT_4B      0x00000000U      /*!< 4-bit long wake-up address */
#define UART_ADDRESS_DETECT_7B      USART_CR2_ADDM7  /*!< 7-bit long wake-up address */
/**
  * @}
  */

/** @defgroup UARTEx_FIFO_mode UARTEx FIFO mode
  * @brief    UART FIFO mode
  * @{
  */
#define UART_FIFOMODE_DISABLE       0x00000000U       /*!< FIFO mode disable */
#define UART_FIFOMODE_ENABLE        USART_CR1_FIFOEN  /*!< FIFO mode enable  */
/**
  * @}
  */

/** @defgroup UARTEx_TXFIFO_threshold_level UARTEx TXFIFO threshold level
  * @brief    UART TXFIFO threshold level
  * @{
  */
#define UART_TXFIFO_THRESHOLD_1_8   0x00000000U                               /*!< TXFIFO reaches 1/8 of its depth */
#define UART_TXFIFO_THRESHOLD_1_4   USART_CR3_TXFTCFG_0                       /*!< TXFIFO reaches 1/4 of its depth */
#define UART_TXFIFO_THRESHOLD_1_2   USART_CR3_TXFTCFG_1                       /*!< TXFIFO reaches 1/2 of its depth */
#define UART_TXFIFO_THRESHOLD_3_4   (USART_CR3_TXFTCFG_0|USART_CR3_TXFTCFG_1) /*!< TXFIFO reaches 3/4 of its depth */
#define UART_TXFIFO_THRESHOLD_7_8   USART_CR3_TXFTCFG_2                       /*!< TXFIFO reaches 7/8 of its depth */
#define UART_TXFIFO_THRESHOLD_8_8   (USART_CR3_TXFTCFG_2|USART_CR3_TXFTCFG_0) /*!< TXFIFO becomes empty            */
/**
  * @}
  */

/** @defgroup UARTEx_RXFIFO_threshold_level UARTEx RXFIFO threshold level
  * @brief    UART RXFIFO threshold level
  * @{
  */
#define UART_RXFIFO_THRESHOLD_1_8   0x00000000U                               /*!< RXFIFO FIFO reaches 1/8 of its depth */
#define UART_RXFIFO_THRESHOLD_1_4   USART_CR3_RXFTCFG_0                       /*!< RXFIFO FIFO reaches 1/4 of its depth */
#define UART_RXFIFO_THRESHOLD_1_2   USART_CR3_RXFTCFG_1                       /*!< RXFIFO FIFO reaches 1/2 of its depth */
#define UART_RXFIFO_THRESHOLD_3_4   (USART_CR3_RXFTCFG_0|USART_CR3_RXFTCFG_1) /*!< RXFIFO FIFO reaches 3/4 of its depth */
#define UART_RXFIFO_THRESHOLD_7_8   USART_CR3_RXFTCFG_2                       /*!< RXFIFO FIFO reaches 7/8 of its depth */
#define UART_RXFIFO_THRESHOLD_8_8   (USART_CR3_RXFTCFG_2|USART_CR3_RXFTCFG_0) /*!< RXFIFO FIFO becomes full             */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup UARTEx_Exported_Functions
  * @{
  */

/** @addtogroup UARTEx_Exported_Functions_Group1
  * @{
  */

/* Initialization and de-initialization functions  ****************************/
HAL_StatusTypeDef HAL_RS485Ex_Init(UART_HandleTypeDef *huart, uint32_t Polarity, uint32_t AssertionTime,
                                   uint32_t DeassertionTime);

/**
  * @}
  */

/** @addtogroup UARTEx_Exported_Functions_Group2
  * @{
  */

void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_TxFifoEmptyCallback(UART_HandleTypeDef *huart);

/**
  * @}
  */

/** @addtogroup UARTEx_Exported_Functions_Group3
  * @{
  */

/* Peripheral Control functions  **********************************************/
HAL_StatusTypeDef HAL_UARTEx_StopModeWakeUpSourceConfig(UART_HandleTypeDef *huart, UART_WakeUpTypeDef WakeUpSelection);
HAL_StatusTypeDef HAL_UARTEx_EnableStopMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_DisableStopMode(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_MultiProcessorEx_AddressLength_Set(UART_HandleTypeDef *huart, uint32_t AddressLength);

HAL_StatusTypeDef HAL_UARTEx_EnableFifoMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_DisableFifoMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_SetTxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold);
HAL_StatusTypeDef HAL_UARTEx_SetRxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup UARTEx_Private_Macros UARTEx Private Macros
  * @{
  */

/** @brief  Report the UART clock source.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __CLOCKSOURCE__ output variable.
  * @retval UART clocking source, written in __CLOCKSOURCE__.
  */
#if defined(UART9) && defined(USART10)
#define UART_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__)       \
  do {                                                        \
    if((__HANDLE__)->Instance == USART1)                      \
    {                                                         \
      switch(__HAL_RCC_GET_USART1_SOURCE())                   \
      {                                                       \
        case RCC_USART1CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART1CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART2)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART2_SOURCE())                   \
      {                                                       \
        case RCC_USART2CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART2CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART3)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART3_SOURCE())                   \
      {                                                       \
        case RCC_USART3CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART3CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART4)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART4_SOURCE())                    \
      {                                                       \
        case RCC_UART4CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART4CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if ((__HANDLE__)->Instance == UART5)                 \
    {                                                         \
      switch(__HAL_RCC_GET_UART5_SOURCE())                    \
      {                                                       \
        case RCC_UART5CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART5CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART6)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART6_SOURCE())                   \
      {                                                       \
        case RCC_USART6CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART6CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART7)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART7_SOURCE())                    \
      {                                                       \
        case RCC_UART7CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART7CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART8)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART8_SOURCE())                    \
      {                                                       \
        case RCC_UART8CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART8CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART9)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART9_SOURCE())                    \
      {                                                       \
        case RCC_UART9CLKSOURCE_D2PCLK2:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_UART9CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART9CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART9CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART9CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART9CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART10)                \
    {                                                         \
      switch(__HAL_RCC_GET_USART10_SOURCE())                  \
      {                                                       \
        case RCC_USART10CLKSOURCE_D2PCLK2:                    \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART10CLKSOURCE_PLL2:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART10CLKSOURCE_PLL3:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART10CLKSOURCE_HSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART10CLKSOURCE_CSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART10CLKSOURCE_LSE:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == LPUART1)                \
    {                                                         \
      switch(__HAL_RCC_GET_LPUART1_SOURCE())                  \
      {                                                       \
        case RCC_LPUART1CLKSOURCE_D3PCLK1:                    \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D3PCLK1;       \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_PLL2:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_PLL3:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_HSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_CSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_LSE:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else                                                      \
    {                                                         \
      (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;         \
    }                                                         \
  } while(0U)
#else
#define UART_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__)       \
  do {                                                        \
    if((__HANDLE__)->Instance == USART1)                      \
    {                                                         \
      switch(__HAL_RCC_GET_USART1_SOURCE())                   \
      {                                                       \
        case RCC_USART1CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART1CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART1CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART2)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART2_SOURCE())                   \
      {                                                       \
        case RCC_USART2CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART2CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART2CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART3)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART3_SOURCE())                   \
      {                                                       \
        case RCC_USART3CLKSOURCE_D2PCLK1:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART3CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART3CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART4)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART4_SOURCE())                    \
      {                                                       \
        case RCC_UART4CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART4CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART4CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if ((__HANDLE__)->Instance == UART5)                 \
    {                                                         \
      switch(__HAL_RCC_GET_UART5_SOURCE())                    \
      {                                                       \
        case RCC_UART5CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART5CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART5CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == USART6)                 \
    {                                                         \
      switch(__HAL_RCC_GET_USART6_SOURCE())                   \
      {                                                       \
        case RCC_USART6CLKSOURCE_D2PCLK2:                     \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK2;       \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL2:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_USART6CLKSOURCE_PLL3:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_USART6CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_CSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART7)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART7_SOURCE())                    \
      {                                                       \
        case RCC_UART7CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART7CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART7CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == UART8)                  \
    {                                                         \
      switch(__HAL_RCC_GET_UART8_SOURCE())                    \
      {                                                       \
        case RCC_UART8CLKSOURCE_D2PCLK1:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D2PCLK1;       \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL2:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_UART8CLKSOURCE_PLL3:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_UART8CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_CSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else if((__HANDLE__)->Instance == LPUART1)                \
    {                                                         \
      switch(__HAL_RCC_GET_LPUART1_SOURCE())                  \
      {                                                       \
        case RCC_LPUART1CLKSOURCE_D3PCLK1:                    \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_D3PCLK1;       \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_PLL2:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL2;          \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_PLL3:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PLL3;          \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_HSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_CSI:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_CSI;           \
          break;                                              \
        case RCC_LPUART1CLKSOURCE_LSE:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
      }                                                       \
    }                                                         \
    else                                                      \
    {                                                         \
      (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;         \
    }                                                         \
  } while(0U)
#endif  /* UART9 && USART10 */

/** @brief  Report the UART mask to apply to retrieve the received data
  *         according to the word length and to the parity bits activation.
  * @note   If PCE = 1, the parity bit is not included in the data extracted
  *         by the reception API().
  *         This masking operation is not carried out in the case of
  *         DMA transfers.
  * @param  __HANDLE__ specifies the UART Handle.
  * @retval None, the mask to apply to UART RDR register is stored in (__HANDLE__)->Mask field.
  */
#define UART_MASK_COMPUTATION(__HANDLE__)                             \
  do {                                                                \
    if ((__HANDLE__)->Init.WordLength == UART_WORDLENGTH_9B)          \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == UART_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x01FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == UART_WORDLENGTH_8B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == UART_PARITY_NONE)              \
      {                                                               \
        (__HANDLE__)->Mask = 0x00FFU ;                                \
      }                                                               \
      else                                                            \
      {                                                               \
        (__HANDLE__)->Mask = 0x007FU ;                                \
      }                                                               \
    }                                                                 \
    else if ((__HANDLE__)->Init.WordLength == UART_WORDLENGTH_7B)     \
    {                                                                 \
      if ((__HANDLE__)->Init.Parity == UART_PARITY_NONE)              \
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

/**
  * @brief Ensure that UART frame length is valid.
  * @param __LENGTH__ UART frame length.
  * @retval SET (__LENGTH__ is valid) or RESET (__LENGTH__ is invalid)
  */
#define IS_UART_WORD_LENGTH(__LENGTH__) (((__LENGTH__) == UART_WORDLENGTH_7B) || \
                                         ((__LENGTH__) == UART_WORDLENGTH_8B) || \
                                         ((__LENGTH__) == UART_WORDLENGTH_9B))

/**
  * @brief Ensure that UART wake-up address length is valid.
  * @param __ADDRESS__ UART wake-up address length.
  * @retval SET (__ADDRESS__ is valid) or RESET (__ADDRESS__ is invalid)
  */
#define IS_UART_ADDRESSLENGTH_DETECT(__ADDRESS__) (((__ADDRESS__) == UART_ADDRESS_DETECT_4B) || \
                                                   ((__ADDRESS__) == UART_ADDRESS_DETECT_7B))

/**
  * @brief Ensure that UART TXFIFO threshold level is valid.
  * @param __THRESHOLD__ UART TXFIFO threshold level.
  * @retval SET (__THRESHOLD__ is valid) or RESET (__THRESHOLD__ is invalid)
  */
#define IS_UART_TXFIFO_THRESHOLD(__THRESHOLD__) (((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_1_8) || \
                                                 ((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_1_4) || \
                                                 ((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_1_2) || \
                                                 ((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_3_4) || \
                                                 ((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_7_8) || \
                                                 ((__THRESHOLD__) == UART_TXFIFO_THRESHOLD_8_8))

/**
  * @brief Ensure that UART RXFIFO threshold level is valid.
  * @param __THRESHOLD__ UART RXFIFO threshold level.
  * @retval SET (__THRESHOLD__ is valid) or RESET (__THRESHOLD__ is invalid)
  */
#define IS_UART_RXFIFO_THRESHOLD(__THRESHOLD__) (((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_1_8) || \
                                                 ((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_1_4) || \
                                                 ((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_1_2) || \
                                                 ((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_3_4) || \
                                                 ((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_7_8) || \
                                                 ((__THRESHOLD__) == UART_RXFIFO_THRESHOLD_8_8))

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_UART_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
