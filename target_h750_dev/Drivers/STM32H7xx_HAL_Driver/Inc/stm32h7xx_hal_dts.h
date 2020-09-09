/**
  ******************************************************************************
  * @file    stm32h7xx_hal_dts.h
  * @author  MCD Application Team
  * @brief   Header file of DTS HAL module.
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
#ifndef __STM32H7xx_HAL_DTS_H
#define __STM32H7xx_HAL_DTS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DTS)
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup DTS
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup DTS_Exported_Types DTS Exported Types
  * @{
  */

/**
  * @brief  DTS Init structure definition
  */
typedef struct
{
  uint32_t QuickMeasure;  /*!< Specifies the quick measure option selection of the DTS sensor.
                               This parameter can be a value of @ref DTS_Quick_Measurement */

  uint32_t RefClock;      /*!< Specifies the reference clock selection of the DTS sensor.
                               This parameter can be a value of @ref DTS_Reference_Clock_Selection */

  uint32_t TriggerInput;  /*!< Specifies the trigger input of the DTS sensor.
                               This parameter can be a value of @ref DTS_TriggerConfig */

  uint32_t SamplingTime;  /*!< Specifies the sampling time configuration.
                               This parameter can be a value of @ref DTS_Sampling_Time */

  uint32_t Divider;       /*!< Specifies the high speed clock divider ratio.
                               This parameter can be a value from 0 to 127 */

  uint32_t HighThreshold;  /*!< Specifies the high threshold of the DTS sensor */

  uint32_t LowThreshold;   /*!< Specifies the low threshold of the DTS sensor */

} DTS_InitTypeDef;

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_DTS_STATE_RESET       = 0x00UL,     /*!< DTS not yet initialized or disabled */
  HAL_DTS_STATE_READY       = 0x01UL,     /*!< DTS initialized and ready for use   */
  HAL_DTS_STATE_BUSY        = 0x02UL,     /*!< DTS is running                      */
  HAL_DTS_STATE_TIMEOUT     = 0x03UL,     /*!< Timeout state                       */
  HAL_DTS_STATE_ERROR       = 0x04UL      /*!< Internal Process error              */
} HAL_DTS_StateTypeDef;

/**
  * @brief  DTS Handle Structure definition
  */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
typedef struct __DTS_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
{
  DTS_TypeDef         *Instance;    /*!< Register base address */
  DTS_InitTypeDef     Init;         /*!< DTS required parameters */
  HAL_LockTypeDef     Lock;         /*!< DTS Locking object */
  __IO HAL_DTS_StateTypeDef  State; /*!< DTS peripheral state  */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
  void (* MspInitCallback)(struct __DTS_HandleTypeDef *hdts);         /*!< DTS Base Msp Init Callback                   */
  void (* MspDeInitCallback)(struct __DTS_HandleTypeDef *hdts);       /*!< DTS Base Msp DeInit Callback                 */
  void (* DTS_EndCallback)(struct __DTS_HandleTypeDef *hdts);         /*!< End measure Callback                         */
  void (* DTS_LowCallback)(struct __DTS_HandleTypeDef *hdts);         /*!< low threshold Callback                       */
  void (* DTS_HighCallback)(struct __DTS_HandleTypeDef *hdts);        /*!< high threshold Callback                      */
  void (* DTS_AsyncEndCallback)(struct __DTS_HandleTypeDef *hdts);    /*!< Asynchronous end of measure Callback         */
  void (* DTS_AsyncLowCallback)(struct __DTS_HandleTypeDef *hdts);    /*!< Asynchronous low threshold Callback          */
  void (* DTS_AsyncHighCallback(struct __DTS_HandleTypeDef *hdts);    /*!< Asynchronous high threshold Callback         */
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
} DTS_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup DTS_Exported_Constants DTS Exported Constants
  * @{
  */

/** @defgroup DTS_TriggerConfig  DTS Trigger Configuration
  * @{
  */
/* @brief No Hardware trigger detection */
#define DTS_TRIGGER_HW_NONE   (0UL)

/* @brief External Interrupt Mode with LPTIMER1 trigger detection */
#define DTS_TRIGGER_LPTIMER1  DTS_CFGR1_TS1_INTRIG_SEL_0

/* @brief External Interrupt Mode with LPTIMER2 trigger detection */
#define DTS_TRIGGER_LPTIMER2  DTS_CFGR1_TS1_INTRIG_SEL_1

/* @brief External Interrupt Mode with LPTIMER3 trigger detection */
#define DTS_TRIGGER_LPTIMER3 (DTS_CFGR1_TS1_INTRIG_SEL_0 | DTS_CFGR1_TS1_INTRIG_SEL_1)

/* @brief External Interrupt Mode with EXTI13 trigger detection */
#define DTS_TRIGGER_EXTI13    DTS_CFGR1_TS1_INTRIG_SEL_2
/**
  * @}
  */

/** @defgroup DTS_Quick_Measurement  DTS Quick Measurement
  * @{
  */
#define DTS_QUICKMEAS_ENABLE    DTS_CFGR1_Q_MEAS_OPT      /*!< Enable the Quick Measure (Measure without calibration) */
#define DTS_QUICKMEAS_DISABLE   (0x0UL)                   /*!< Disable the Quick Measure (Measure with calibration) */
/**
  * @}
  */

/** @defgroup DTS_Reference_Clock_Selection   DTS Reference Clock Selection
  * @{
  */
#define DTS_REFCLKSEL_LSE   DTS_CFGR1_REFCLK_SEL          /*!< Low speed REF clock (LSE) */
#define DTS_REFCLKSEL_PCLK (0UL)                          /*!< High speed REF clock (PCLK) */
/**
  * @}
  */

/** @defgroup DTS_Sampling_Time   DTS Sampling Time
  * @{
  */
#define DTS_SMP_TIME_1_CYCLE     DTS_CFGR1_TS1_SMP_TIME_0                                                                                   /*!< 1 clock cycle for the sampling time  */
#define DTS_SMP_TIME_2_CYCLE     DTS_CFGR1_TS1_SMP_TIME_1                                                                                   /*!< 2 clock cycle for the sampling time  */
#define DTS_SMP_TIME_3_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_1)                                                       /*!< 3 clock cycle for the sampling time  */
#define DTS_SMP_TIME_4_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_2)                                                                                 /*!< 4 clock cycle for the sampling time  */
#define DTS_SMP_TIME_5_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_2)                                                       /*!< 5 clock cycle for the sampling time  */
#define DTS_SMP_TIME_6_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_2)                                                       /*!< 6 clock cycle for the sampling time  */
#define DTS_SMP_TIME_7_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_2)                            /*!< 7 clock cycle for the sampling time  */
#define DTS_SMP_TIME_8_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_3)                                                                                  /*!< 8 clock cycle for the sampling time  */
#define DTS_SMP_TIME_9_CYCLE    (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_3)                                                       /*!< 9 clock cycle for the sampling time  */
#define DTS_SMP_TIME_10_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_3)                                                       /*!< 10 clock cycle for the sampling time */
#define DTS_SMP_TIME_11_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_3)                            /*!< 11 clock cycle for the sampling time */
#define DTS_SMP_TIME_12_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_2 | DTS_CFGR1_TS1_SMP_TIME_3)                                                       /*!< 12 clock cycle for the sampling time */
#define DTS_SMP_TIME_13_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_2 | DTS_CFGR1_TS1_SMP_TIME_3)                            /*!< 13 clock cycle for the sampling time */
#define DTS_SMP_TIME_14_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_2 | DTS_CFGR1_TS1_SMP_TIME_3)                            /*!< 14 clock cycle for the sampling time */
#define DTS_SMP_TIME_15_CYCLE   (DTS_CFGR1_TS1_SMP_TIME_0 | DTS_CFGR1_TS1_SMP_TIME_1 | DTS_CFGR1_TS1_SMP_TIME_2 | DTS_CFGR1_TS1_SMP_TIME_3) /*!< 15 clock cycle for the sampling time */
/**
  * @}
  */
/** @defgroup DTS_Flag_Definitions DTS Flag Definitions
  * @{
  */
#define DTS_FLAG_TS1_ITE   DTS_SR_TS1_ITEF   /*!< Interrupt flag for end of measure for DTS1 */
#define DTS_FLAG_TS1_ITL   DTS_SR_TS1_ITLF   /*!< Interrupt flag for low threshold for DTS1  */
#define DTS_FLAG_TS1_ITH   DTS_SR_TS1_ITHF   /*!< Interrupt flag for high threshold for DTS1 */
#define DTS_FLAG_TS1_AITE  DTS_SR_TS1_AITEF  /*!< Asynchronous Interrupt flag for end of measure for DTS1 */
#define DTS_FLAG_TS1_AITL  DTS_SR_TS1_AITLF  /*!< Asynchronous Interrupt flag for low threshold for DTS1  */
#define DTS_FLAG_TS1_AITH  DTS_SR_TS1_AITHF  /*!< Asynchronous Interrupt flag for high threshold for DTS1 */
#define DTS_FLAG_TS1_RDY   DTS_SR_TS1_RDY    /*!< Ready flag for DTS1 */
/**
  * @}
  */

/** @defgroup DTS_Interrupts_Definitions DTS Interrupts Definitions
  * @{
  */
#define DTS_IT_TS1_ITE  DTS_ITENR_TS1_ITEEN   /*!< Enable interrupt flag for end of measure for DTS1 */
#define DTS_IT_TS1_ITL  DTS_ITENR_TS1_ITLEN   /*!< Enable interrupt flag for low threshold for DTS1  */
#define DTS_IT_TS1_ITH  DTS_ITENR_TS1_ITHEN   /*!< Enable interrupt flag for high threshold for DTS1 */
#define DTS_IT_TS1_AITE DTS_ITENR_TS1_AITEEN  /*!< Enable asynchronous interrupt flag for end of measure for DTS1 */
#define DTS_IT_TS1_AITL DTS_ITENR_TS1_AITLEN  /*!< Enable asynchronous interrupt flag for low threshold for DTS1  */
#define DTS_IT_TS1_AITH DTS_ITENR_TS1_AITHEN  /*!< Enable asynchronous interrupt flag for high threshold for DTS1 */
/**
  * @}
  */

/**
  * @}
  */
/* Exported macros -----------------------------------------------------------*/
/** @defgroup DTS_Exported_Macros DTS Exported Macros
  * @{
  */

/** @brief  Reset DTS handle state
  * @param  __HANDLE__ DTS handle.
  * @retval None
  */
#define __HAL_DTS_RESET_HANDLE_STATE(__HANDLE__)    ((__HANDLE__)->State = HAL_DTS_STATE_RESET)

/**
  * @brief  Enable the specified DTS sensor
  * @param  __HANDLE__ DTS handle.
  * @retval None
  */
#define __HAL_DTS_ENABLE(__HANDLE__)  SET_BIT((__HANDLE__)->Instance->CFGR1, DTS_CFGR1_TS1_EN)

/**
  * @brief  Disable the specified DTS sensor
  * @param  __HANDLE__ DTS handle.
  * @retval None
  */
#define __HAL_DTS_DISABLE(__HANDLE__)    CLEAR_BIT((__HANDLE__)->Instance->CFGR1, DTS_CFGR1_TS1_EN)

/**
  * @brief  Enable the DTS EXTI line in interrupt mode
  * @retval None
  */
#define __HAL_DTS_EXTI_WAKEUP_ENABLE_IT()  SET_BIT(EXTI->IMR3, DTS_EXTI_LINE_DTS1)

/**
  * @brief  Disable the DTS EXTI line in interrupt mode
  * @retval None
  */
#define __HAL_DTS_EXTI_WAKEUP_DISABLE_IT()  CLEAR_BIT(EXTI->IMR3, DTS_EXTI_LINE_DTS1)

/**
  * @brief  Enable the DTS EXTI Line in event mode
  * @retval None
  */
#define __HAL_DTS_EXTI_WAKEUP_ENABLE_EVENT() SET_BIT(EXTI->EMR3, DTS_EXTI_LINE_DTS1)

/**
  * @brief  Disable the DTS EXTI Line in event mode
  * @retval None
  */
#define __HAL_DTS_EXTI_WAKEUP_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR3, DTS_EXTI_LINE_DTS1)

/** @brief  Checks whether the specified DTS flag is set or not.
  * @param  __HANDLE__ specifies the DTS Handle.
  * @param  __FLAG__ specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg DTS_FLAG_TS1_ITE : interrupt flag for end of measure for DTS1
  *            @arg DTS_FLAG_TS1_ITL : interrupt flag for low threshold for DTS1
  *            @arg DTS_FLAG_TS1_ITH : interrupt flag for high threshold for DTS1
  *            @arg DTS_FLAG_TS1_AITE: asynchronous interrupt flag for end of measure for DTS1
  *            @arg DTS_FLAG_TS1_AITL: asynchronous interrupt flag for low threshold for DTS1
  *            @arg DTS_FLAG_TS1_AITH: asynchronous interrupt flag for high threshold for DTS1
  *            @arg DTS_FLAG_TS1_RDY : Ready flag for DTS1
  *            @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_DTS_GET_FLAG(__HANDLE__, __FLAG__)  \
        (((((__HANDLE__)->Instance->SR &(__FLAG__)) == (__FLAG__)))? SET : RESET)


/** @brief  Clears the specified DTS pending flag.
  * @param  __HANDLE__ specifies the DTS Handle.
  * @param  __FLAG__ specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg DTS_FLAG_TS1_ITE : interrupt flag for end of measure for DTS1
  *            @arg DTS_FLAG_TS1_ITL : interrupt flag for low threshold for DTS1
  *            @arg DTS_FLAG_TS1_ITH : interrupt flag for high threshold for DTS1
  *            @arg DTS_FLAG_TS1_AITE: asynchronous interrupt flag for end of measure for DTS1
  *            @arg DTS_FLAG_TS1_AITL: asynchronous interrupt flag for low threshold for DTS1
  *            @arg DTS_FLAG_TS1_AITH: asynchronous interrupt flag for high threshold for DTS1
  * @retval None
  */
#define __HAL_DTS_CLEAR_FLAG(__HANDLE__, __FLAG__)  \
        ((__HANDLE__)->Instance->ICIFR  = (__FLAG__))


/** @brief  Enable the specified DTS interrupt.
  * @param  __HANDLE__ specifies the DTS Handle.
  * @param  __INTERRUPT__ specifies the DTS interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg DTS_IT_TS1_ITE  : interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_ITL  : interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_ITH  : interrupt flag for high of measure for DTS1
  *            @arg DTS_IT_TS1_AITE : asynchronous interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_AITL : asynchronous interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_AITH : asynchronous interrupt flag for high of measure for DTS1
  * @retval None
  */
#define __HAL_DTS_ENABLE_IT(__HANDLE__, __INTERRUPT__)  \
        SET_BIT((__HANDLE__)->Instance->ITENR, __INTERRUPT__)


/** @brief  Disable the specified DTS interrupt.
  * @param  __HANDLE__ specifies the DTS Handle.
  * @param  __INTERRUPT__ specifies the DTS interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg DTS_IT_TS1_ITE  : interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_ITL  : interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_ITH  : interrupt flag for high of measure for DTS1
  *            @arg DTS_IT_TS1_AITE : asynchronous interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_AITL : asynchronous interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_AITH : asynchronous interrupt flag for high of measure for DTS1
  * @retval None
  */
#define __HAL_DTS_DISABLE_IT(__HANDLE__,__INTERRUPT__)  \
        CLEAR_BIT((__HANDLE__)->Instance->ITENR, __INTERRUPT__)


/** @brief  Check whether the specified DTS interrupt source is enabled or not.
  * @param __HANDLE__ DTS handle.
  * @param __INTERRUPT__ DTS interrupt source to check
  *          This parameter can be one of the following values:
  *            @arg DTS_IT_TS1_ITE  : interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_ITL  : interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_ITH  : interrupt flag for high of measure for DTS1
  *            @arg DTS_IT_TS1_AITE : asynchronous interrupt flag for end of measure for DTS1
  *            @arg DTS_IT_TS1_AITL : asynchronous interrupt flag for low of measure for DTS1
  *            @arg DTS_IT_TS1_AITH : asynchronous interrupt flag for high of measure for DTS1
  * @retval State of interruption (SET or RESET)
  */
#define __HAL_DTS_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  \
        (( ((__HANDLE__)->Instance->ITENR & (__INTERRUPT__)) == (__INTERRUPT__) \
        )? SET : RESET)


/** @brief  Check whether the specified DTS REFCLK is selected
  * @param __HANDLE__ DTS handle.
  * @param __REFCLK__ DTS reference clock to check
  *          This parameter can be one of the following values:
  * @arg DTS_REFCLKSEL_LSE:   Low speed REF clock
  * @arg DTS_REFCLKSEL_PCLK:  High speed REF clock
  * @retval State of the REF clock tested (SET or RESET)
  */
#define __HAL_DTS_GET_REFCLK(__HANDLE__, __REFCLK__)  ((((__HANDLE__)->Instance->CFGR1 & (__REFCLK__)) == (__REFCLK__))? SET : RESET)

/** @brief  Get Trigger
  * @param __HANDLE__ DTS handle.
  * @retval One of the following trigger
  *     DTS_TRIGGER_HW_NONE : No HW trigger (SW trigger)
  *     DTS_TRIGGER_LPTIMER1: LPTIMER1 trigger
  *     DTS_TRIGGER_LPTIMER2: LPTIMER2 trigger
  *     DTS_TRIGGER_LPTIMER3: LPTIMER3 trigger
  *     DTS_TRIGGER_EXTI13  : EXTI13 trigger
  */
#define __HAL_DTS_GET_TRIGGER(__HANDLE__)  ((__HANDLE__)->Instance->CFGR1 & (DTS_CFGR1_TS1_INTRIG_SEL))
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DTS_Exported_Functions
  * @{
  */

/** @addtogroup DTS_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_DTS_Init(DTS_HandleTypeDef *hdts);
HAL_StatusTypeDef HAL_DTS_DeInit(DTS_HandleTypeDef *hdts);
void              HAL_DTS_MspInit(DTS_HandleTypeDef *hdts);
void              HAL_DTS_MspDeInit(DTS_HandleTypeDef *hdts);
/**
  * @}
  */

/* IO operation functions  *****************************************************/
/** @addtogroup DTS_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_DTS_Start(DTS_HandleTypeDef *hdts);
HAL_StatusTypeDef HAL_DTS_Stop(DTS_HandleTypeDef *hdts);
HAL_StatusTypeDef HAL_DTS_GetTemperature(DTS_HandleTypeDef *hdts, int32_t *Temperature);
HAL_StatusTypeDef HAL_DTS_Start_IT(DTS_HandleTypeDef *hdts);
HAL_StatusTypeDef HAL_DTS_Stop_IT(DTS_HandleTypeDef *hdts);
void              HAL_DTS_IRQHandler(DTS_HandleTypeDef *hdts);
HAL_DTS_StateTypeDef HAL_DTS_GetState(DTS_HandleTypeDef *hdts);
/* Callback in Interrupt mode */
void              HAL_DTS_EndCallback(DTS_HandleTypeDef *hdts);
void              HAL_DTS_LowCallback(DTS_HandleTypeDef *hdts);
void              HAL_DTS_HighCallback(DTS_HandleTypeDef *hdts);
void              HAL_DTS_AsyncEndCallback(DTS_HandleTypeDef *hdts);
void              HAL_DTS_AsyncLowCallback(DTS_HandleTypeDef *hdts);
void              HAL_DTS_AsyncHighCallback(DTS_HandleTypeDef *hdts);
/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup DTS_Private_Constants DTS Private Constants
  * @{
  */
/** @defgroup DTS_ExtiLine DTS EXTI Lines
  * @{
  */
#define DTS_EXTI_LINE_DTS1           (EXTI_IMR3_IM88)  /*!< EXTI line 88 connected to DTS1 output */
/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DTS_Private_Macros DTS Private Macros
  * @{
  */

/** @defgroup DTS_IS_DTS_Definitions  DTS Private macros to check input parameters
  * @{
  */
#define IS_DTS_QUICKMEAS(__SEL__)   (((__SEL__) == DTS_QUICKMEAS_DISABLE) || \
                                     ((__SEL__) == DTS_QUICKMEAS_ENABLE))

#define IS_DTS_REFCLK(__SEL__)      (((__SEL__) == DTS_REFCLKSEL_LSE) || \
                                     ((__SEL__) == DTS_REFCLKSEL_PCLK))

#define IS_DTS_TRIGGERINPUT(__INPUT__)  (((__INPUT__) == DTS_TRIGGER_HW_NONE)   || \
                                         ((__INPUT__) == DTS_TRIGGER_LPTIMER1)  || \
                                         ((__INPUT__) == DTS_TRIGGER_LPTIMER2)  || \
                                         ((__INPUT__) == DTS_TRIGGER_LPTIMER3)  || \
                                         ((__INPUT__) == DTS_TRIGGER_EXTI13))

#define IS_DTS_THRESHOLD(__THRESHOLD__)  ((__THRESHOLD__) <= 0xFFFFUL)

#define IS_DTS_DIVIDER_RATIO_NUMBER(__NUMBER__) (((__NUMBER__) >= (2UL)) && ((__NUMBER__) <= (127UL)))

#define IS_DTS_SAMPLINGTIME(__CYCLE__)  (((__CYCLE__) == DTS_SMP_TIME_1_CYCLE)  || \
                                             ((__CYCLE__) == DTS_SMP_TIME_2_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_3_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_4_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_5_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_6_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_7_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_8_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_9_CYCLE)    || \
                                             ((__CYCLE__) == DTS_SMP_TIME_10_CYCLE)   || \
                                             ((__CYCLE__) == DTS_SMP_TIME_11_CYCLE)   || \
                                             ((__CYCLE__) == DTS_SMP_TIME_12_CYCLE)   || \
                                             ((__CYCLE__) == DTS_SMP_TIME_13_CYCLE)   || \
                                             ((__CYCLE__) == DTS_SMP_TIME_14_CYCLE)   || \
                                             ((__CYCLE__) == DTS_SMP_TIME_15_CYCLE))

/**
  * @}
  */

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

#endif /* DTS */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_HAL_DTS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
