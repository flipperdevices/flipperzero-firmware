/**
  ******************************************************************************
  * @file    stm32h7xx_hal_dma_ex.h
  * @author  MCD Application Team
  * @brief   Header file of DMA HAL extension module.
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
#ifndef STM32H7xx_HAL_DMA_EX_H
#define STM32H7xx_HAL_DMA_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup DMAEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup DMAEx_Exported_Types DMAEx Exported Types
  * @brief DMAEx Exported types
  * @{
  */

/**
  * @brief  HAL DMA Memory definition
  */
typedef enum
{
  MEMORY0      = 0x00U,    /*!< Memory 0     */
  MEMORY1      = 0x01U,    /*!< Memory 1     */

}HAL_DMA_MemoryTypeDef;

/**
  * @brief  HAL DMAMUX Synchronization configuration structure definition
  */
typedef struct
{
  uint32_t SyncSignalID;  /*!< Specifies the synchronization signal gating the DMA request in periodic mode.
                              This parameter can be a value of @ref DMAEx_MUX_SyncSignalID_selection */

  uint32_t SyncPolarity;  /*!< Specifies the polarity of the signal on which the DMA request is synchronized.
                              This parameter can be a value of @ref DMAEx_MUX_SyncPolarity_selection */

  FunctionalState SyncEnable;  /*!< Specifies if the synchronization shall be enabled or disabled
                                    This parameter can take the value ENABLE or DISABLE*/


  FunctionalState EventEnable;    /*!< Specifies if an event shall be generated once the RequestNumber is reached.
                                       This parameter can take the value ENABLE or DISABLE */

  uint32_t RequestNumber; /*!< Specifies the number of DMA request that will be authorized after a sync event.
                               This parameters can be in the range 1 to 32 */

}HAL_DMA_MuxSyncConfigTypeDef;


/**
  * @brief  HAL DMAMUX request generator parameters structure definition
  */
typedef struct
{
 uint32_t SignalID;      /*!< Specifies the ID of the signal used for DMAMUX request generator
                              This parameter can be a value of @ref DMAEx_MUX_SignalGeneratorID_selection */

  uint32_t Polarity;       /*!< Specifies the polarity of the signal on which the request is generated.
                             This parameter can be a value of @ref DMAEx_MUX_RequestGeneneratorPolarity_selection */

  uint32_t RequestNumber;  /*!< Specifies the number of DMA request that will be generated after a signal event.
                                This parameters can be in the range 1 to 32 */

}HAL_DMA_MuxRequestGeneratorConfigTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMAEx_Exported_Constants DMA Exported Constants
  * @brief    DMAEx Exported constants
  * @{
  */

/** @defgroup DMAEx_MUX_SyncSignalID_selection DMAEx MUX SyncSignalID selection
  * @brief    DMAEx MUX SyncSignalID selection
  * @{
  */
#define HAL_DMAMUX1_SYNC_DMAMUX1_CH0_EVT   0U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel0 Event */
#define HAL_DMAMUX1_SYNC_DMAMUX1_CH1_EVT   1U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel1 Event */
#define HAL_DMAMUX1_SYNC_DMAMUX1_CH2_EVT   2U   /*!< DMAMUX1 synchronization Signal is DMAMUX1 Channel2 Event */
#define HAL_DMAMUX1_SYNC_LPTIM1_OUT        3U   /*!< DMAMUX1 synchronization Signal is LPTIM1 OUT             */
#define HAL_DMAMUX1_SYNC_LPTIM2_OUT        4U   /*!< DMAMUX1 synchronization Signal is LPTIM2 OUT             */
#define HAL_DMAMUX1_SYNC_LPTIM3_OUT        5U   /*!< DMAMUX1 synchronization Signal is LPTIM3 OUT             */
#define HAL_DMAMUX1_SYNC_EXTI0             6U   /*!< DMAMUX1 synchronization Signal is EXTI0 IT               */
#define HAL_DMAMUX1_SYNC_TIM12_TRGO        7U   /*!< DMAMUX1 synchronization Signal is TIM12 TRGO             */

#define HAL_DMAMUX2_SYNC_DMAMUX2_CH0_EVT   0U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel0 Event */
#define HAL_DMAMUX2_SYNC_DMAMUX2_CH1_EVT   1U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel1 Event */
#define HAL_DMAMUX2_SYNC_DMAMUX2_CH2_EVT   2U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel2 Event */
#define HAL_DMAMUX2_SYNC_DMAMUX2_CH3_EVT   3U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel3 Event */
#define HAL_DMAMUX2_SYNC_DMAMUX2_CH4_EVT   4U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel4 Event */
#define HAL_DMAMUX2_SYNC_DMAMUX2_CH5_EVT   5U   /*!< DMAMUX2 synchronization Signal is DMAMUX2 Channel5 Event */
#define HAL_DMAMUX2_SYNC_LPUART1_RX_WKUP   6U   /*!< DMAMUX2 synchronization Signal is LPUART1 RX Wakeup      */
#define HAL_DMAMUX2_SYNC_LPUART1_TX_WKUP   7U   /*!< DMAMUX2 synchronization Signal is LPUART1 TX Wakeup      */
#define HAL_DMAMUX2_SYNC_LPTIM2_OUT        8U   /*!< DMAMUX2 synchronization Signal is LPTIM2 output          */
#define HAL_DMAMUX2_SYNC_LPTIM3_OUT        9U   /*!< DMAMUX2 synchronization Signal is LPTIM3 output          */
#define HAL_DMAMUX2_SYNC_I2C4_WKUP        10U   /*!< DMAMUX2 synchronization Signal is I2C4 Wakeup            */
#define HAL_DMAMUX2_SYNC_SPI6_WKUP        11U   /*!< DMAMUX2 synchronization Signal is SPI6 Wakeup            */
#define HAL_DMAMUX2_SYNC_COMP1_OUT        12U   /*!< DMAMUX2 synchronization Signal is Comparator 1 output    */
#define HAL_DMAMUX2_SYNC_RTC_WKUP         13U   /*!< DMAMUX2 synchronization Signal is RTC Wakeup             */
#define HAL_DMAMUX2_SYNC_EXTI0            14U   /*!< DMAMUX2 synchronization Signal is EXTI0 IT               */
#define HAL_DMAMUX2_SYNC_EXTI2            15U   /*!< DMAMUX2 synchronization Signal is EXTI2 IT               */

/**
  * @}
  */

/** @defgroup DMAEx_MUX_SyncPolarity_selection DMAEx MUX SyncPolarity selection
  * @brief    DMAEx MUX SyncPolarity selection
  * @{
  */
#define HAL_DMAMUX_SYNC_NO_EVENT        0x00000000U             /*!< block synchronization events                    */
#define HAL_DMAMUX_SYNC_RISING          DMAMUX_CxCR_SPOL_0      /*!< synchronize with rising edge events             */
#define HAL_DMAMUX_SYNC_FALLING         DMAMUX_CxCR_SPOL_1      /*!< synchronize with falling edge events            */
#define HAL_DMAMUX_SYNC_RISING_FALLING  DMAMUX_CxCR_SPOL        /*!< synchronize with rising and falling edge events */

/**
  * @}
  */


/** @defgroup DMAEx_MUX_SignalGeneratorID_selection DMAEx MUX SignalGeneratorID selection
  * @brief    DMAEx MUX SignalGeneratorID selection
  * @{
  */
#define HAL_DMAMUX1_REQ_GEN_DMAMUX1_CH0_EVT   0U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel0 Event */
#define HAL_DMAMUX1_REQ_GEN_DMAMUX1_CH1_EVT   1U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel1 Event */
#define HAL_DMAMUX1_REQ_GEN_DMAMUX1_CH2_EVT   2U   /*!< DMAMUX1 Request generator Signal is DMAMUX1 Channel2 Event */
#define HAL_DMAMUX1_REQ_GEN_LPTIM1_OUT        3U   /*!< DMAMUX1 Request generator Signal is LPTIM1 OUT             */
#define HAL_DMAMUX1_REQ_GEN_LPTIM2_OUT        4U   /*!< DMAMUX1 Request generator Signal is LPTIM2 OUT             */
#define HAL_DMAMUX1_REQ_GEN_LPTIM3_OUT        5U   /*!< DMAMUX1 Request generator Signal is LPTIM3 OUT             */
#define HAL_DMAMUX1_REQ_GEN_EXTI0             6U   /*!< DMAMUX1 Request generator Signal is EXTI0 IT               */
#define HAL_DMAMUX1_REQ_GEN_TIM12_TRGO        7U   /*!< DMAMUX1 Request generator Signal is TIM12 TRGO             */

#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH0_EVT   0U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel0 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH1_EVT   1U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel1 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH2_EVT   2U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel2 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH3_EVT   3U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel3 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH4_EVT   4U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel4 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH5_EVT   5U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel5 Event */
#define HAL_DMAMUX2_REQ_GEN_DMAMUX2_CH6_EVT   6U   /*!< DMAMUX2 Request generator Signal is DMAMUX2 Channel6 Event */
#define HAL_DMAMUX2_REQ_GEN_LPUART1_RX_WKUP   7U   /*!< DMAMUX2 Request generator Signal is LPUART1 RX Wakeup      */
#define HAL_DMAMUX2_REQ_GEN_LPUART1_TX_WKUP   8U   /*!< DMAMUX2 Request generator Signal is LPUART1 TX Wakeup      */
#define HAL_DMAMUX2_REQ_GEN_LPTIM2_WKUP       9U   /*!< DMAMUX2 Request generator Signal is LPTIM2 Wakeup          */
#define HAL_DMAMUX2_REQ_GEN_LPTIM2_OUT       10U   /*!< DMAMUX2 Request generator Signal is LPTIM2 OUT             */
#define HAL_DMAMUX2_REQ_GEN_LPTIM3_WKUP      11U   /*!< DMAMUX2 Request generator Signal is LPTIM3 Wakeup          */
#define HAL_DMAMUX2_REQ_GEN_LPTIM3_OUT       12U   /*!< DMAMUX2 Request generator Signal is LPTIM3 OUT             */
#if defined(LPTIM4)
#define HAL_DMAMUX2_REQ_GEN_LPTIM4_WKUP      13U   /*!< DMAMUX2 Request generator Signal is LPTIM4 Wakeup          */
#endif /* LPTIM4 */
#if defined(LPTIM5)
#define HAL_DMAMUX2_REQ_GEN_LPTIM5_WKUP      14U   /*!< DMAMUX2 Request generator Signal is LPTIM5 Wakeup          */
#endif /* LPTIM5 */
#define HAL_DMAMUX2_REQ_GEN_I2C4_WKUP        15U   /*!< DMAMUX2 Request generator Signal is I2C4 Wakeup            */
#define HAL_DMAMUX2_REQ_GEN_SPI6_WKUP        16U   /*!< DMAMUX2 Request generator Signal is SPI6 Wakeup            */
#define HAL_DMAMUX2_REQ_GEN_COMP1_OUT        17U   /*!< DMAMUX2 Request generator Signal is Comparator 1 output    */
#define HAL_DMAMUX2_REQ_GEN_COMP2_OUT        18U   /*!< DMAMUX2 Request generator Signal is Comparator 2 output    */
#define HAL_DMAMUX2_REQ_GEN_RTC_WKUP         19U   /*!< DMAMUX2 Request generator Signal is RTC Wakeup             */
#define HAL_DMAMUX2_REQ_GEN_EXTI0            20U   /*!< DMAMUX2 Request generator Signal is EXTI0                  */
#define HAL_DMAMUX2_REQ_GEN_EXTI2            21U   /*!< DMAMUX2 Request generator Signal is EXTI2                  */
#define HAL_DMAMUX2_REQ_GEN_I2C4_IT_EVT      22U   /*!< DMAMUX2 Request generator Signal is I2C4 IT Event          */
#define HAL_DMAMUX2_REQ_GEN_SPI6_IT          23U   /*!< DMAMUX2 Request generator Signal is SPI6 IT                */
#define HAL_DMAMUX2_REQ_GEN_LPUART1_TX_IT    24U   /*!< DMAMUX2 Request generator Signal is LPUART1 Tx IT          */
#define HAL_DMAMUX2_REQ_GEN_LPUART1_RX_IT    25U   /*!< DMAMUX2 Request generator Signal is LPUART1 Rx IT          */
#if defined(ADC3)
#define HAL_DMAMUX2_REQ_GEN_ADC3_IT          26U   /*!< DMAMUX2 Request generator Signal is ADC3 IT                */
#define HAL_DMAMUX2_REQ_GEN_ADC3_AWD1_OUT    27U   /*!< DMAMUX2 Request generator Signal is ADC3 Analog Watchdog 1 output */
#endif /* ADC3 */
#define HAL_DMAMUX2_REQ_GEN_BDMA_CH0_IT      28U   /*!< DMAMUX2 Request generator Signal is BDMA Channel 0 IT      */
#define HAL_DMAMUX2_REQ_GEN_BDMA_CH1_IT      29U   /*!< DMAMUX2 Request generator Signal is BDMA Channel 1 IT      */


/**
  * @}
  */

/** @defgroup DMAEx_MUX_RequestGeneneratorPolarity_selection DMAEx MUX RequestGeneneratorPolarity selection
  * @brief    DMAEx MUX RequestGeneneratorPolarity selection
  * @{
  */
#define HAL_DMAMUX_REQ_GEN_NO_EVENT        0x00000000U           /*!< block request generator events                     */
#define HAL_DMAMUX_REQ_GEN_RISING          DMAMUX_RGxCR_GPOL_0  /*!< generate request on rising edge events             */
#define HAL_DMAMUX_REQ_GEN_FALLING         DMAMUX_RGxCR_GPOL_1  /*!< generate request on falling edge events            */
#define HAL_DMAMUX_REQ_GEN_RISING_FALLING  DMAMUX_RGxCR_GPOL    /*!< generate request on rising and falling edge events */

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup DMAEx_Exported_Functions DMAEx Exported Functions
  * @brief   DMAEx Exported functions
  * @{
  */

/** @defgroup DMAEx_Exported_Functions_Group1 Extended features functions
  * @brief   Extended features functions
  * @{
  */

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory);
HAL_StatusTypeDef HAL_DMAEx_ConfigMuxSync(DMA_HandleTypeDef *hdma, HAL_DMA_MuxSyncConfigTypeDef *pSyncConfig);
HAL_StatusTypeDef HAL_DMAEx_ConfigMuxRequestGenerator (DMA_HandleTypeDef *hdma, HAL_DMA_MuxRequestGeneratorConfigTypeDef *pRequestGeneratorConfig);
HAL_StatusTypeDef HAL_DMAEx_EnableMuxRequestGenerator (DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_DisableMuxRequestGenerator (DMA_HandleTypeDef *hdma);

void HAL_DMAEx_MUX_IRQHandler(DMA_HandleTypeDef *hdma);
/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DMAEx_Private_Macros DMA Private Macros
  * @brief    DMAEx private macros
  * @{
  */

#define IS_DMA_DMAMUX_SYNC_SIGNAL_ID(SIGNAL_ID) ((SIGNAL_ID) <= HAL_DMAMUX1_SYNC_TIM12_TRGO)
#define IS_BDMA_DMAMUX_SYNC_SIGNAL_ID(SIGNAL_ID) ((SIGNAL_ID) <= HAL_DMAMUX2_SYNC_EXTI2)

#define IS_DMAMUX_SYNC_REQUEST_NUMBER(REQUEST_NUMBER) (((REQUEST_NUMBER) > 0U) && ((REQUEST_NUMBER) <= 32U))

#define IS_DMAMUX_SYNC_POLARITY(POLARITY) (((POLARITY) == HAL_DMAMUX_SYNC_NO_EVENT)    || \
                                           ((POLARITY) == HAL_DMAMUX_SYNC_RISING)   || \
                                           ((POLARITY) == HAL_DMAMUX_SYNC_FALLING)  || \
                                           ((POLARITY) == HAL_DMAMUX_SYNC_RISING_FALLING))

#define IS_DMAMUX_SYNC_STATE(SYNC) (((SYNC) == DISABLE)   || ((SYNC) == ENABLE))

#define IS_DMAMUX_SYNC_EVENT(EVENT) (((EVENT) == DISABLE)   || \
                                     ((EVENT) == ENABLE))

#define IS_DMA_DMAMUX_REQUEST_GEN_SIGNAL_ID(SIGNAL_ID) ((SIGNAL_ID) <= HAL_DMAMUX1_REQ_GEN_TIM12_TRGO)
#define IS_BDMA_DMAMUX_REQUEST_GEN_SIGNAL_ID(SIGNAL_ID) ((SIGNAL_ID) <= HAL_DMAMUX2_REQ_GEN_BDMA_CH1_IT)

#define IS_DMAMUX_REQUEST_GEN_REQUEST_NUMBER(REQUEST_NUMBER) (((REQUEST_NUMBER) > 0U) && ((REQUEST_NUMBER) <= 32U))

#define IS_DMAMUX_REQUEST_GEN_POLARITY(POLARITY) (((POLARITY) == HAL_DMAMUX_REQ_GEN_NO_EVENT) || \
                                                  ((POLARITY) == HAL_DMAMUX_REQ_GEN_RISING)   || \
                                                  ((POLARITY) == HAL_DMAMUX_REQ_GEN_FALLING)   || \
                                                  ((POLARITY) == HAL_DMAMUX_REQ_GEN_RISING_FALLING))

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DMAEx_Private_Functions DMAEx Private Functions
  * @brief DMAEx Private functions
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

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_DMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
