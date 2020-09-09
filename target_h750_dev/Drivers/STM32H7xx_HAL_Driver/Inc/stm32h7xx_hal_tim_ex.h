/**
  ******************************************************************************
  * @file    stm32h7xx_hal_tim_ex.h
  * @author  MCD Application Team
  * @brief   Header file of TIM HAL Extended module.
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
#ifndef STM32H7xx_HAL_TIM_EX_H
#define STM32H7xx_HAL_TIM_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup TIMEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Types TIM Extended Exported Types
  * @{
  */

/**
  * @brief  TIM Hall sensor Configuration Structure definition
  */

typedef struct
{
  uint32_t IC1Polarity;         /*!< Specifies the active edge of the input signal.
                                     This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint32_t IC1Prescaler;        /*!< Specifies the Input Capture Prescaler.
                                     This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint32_t IC1Filter;           /*!< Specifies the input capture filter.
                                     This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */

  uint32_t Commutation_Delay;   /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                                     This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */
} TIM_HallSensor_InitTypeDef;
#if defined(TIM_BREAK_INPUT_SUPPORT)

/**
  * @brief  TIM Break/Break2 input configuration
  */
typedef struct
{
  uint32_t Source;         /*!< Specifies the source of the timer break input.
                                This parameter can be a value of @ref TIMEx_Break_Input_Source */
  uint32_t Enable;         /*!< Specifies whether or not the break input source is enabled.
                                This parameter can be a value of @ref TIMEx_Break_Input_Source_Enable */
  uint32_t Polarity;       /*!< Specifies the break input source polarity.
                                This parameter can be a value of @ref TIMEx_Break_Input_Source_Polarity
                                Not relevant when analog watchdog output of the DFSDM1 used as break input source */
}
TIMEx_BreakInputConfigTypeDef;

#endif /* TIM_BREAK_INPUT_SUPPORT */
/**
  * @}
  */
/* End of exported types -----------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Constants TIM Extended Exported Constants
  * @{
  */

/** @defgroup TIMEx_Remap TIM Extended Remapping
  * @{
  */
#define TIM_TIM1_ETR_GPIO        0x00000000U                                                 /* !< TIM1_ETR is connected to GPIO */
#define TIM_TIM1_ETR_COMP1       TIM1_AF1_ETRSEL_0                                           /* !< TIM1_ETR is connected to COMP1 OUT */
#define TIM_TIM1_ETR_COMP2       TIM1_AF1_ETRSEL_1                                           /* !< TIM1_ETR is connected to COMP2 OUT */
#define TIM_TIM1_ETR_ADC1_AWD1   (TIM1_AF1_ETRSEL_1 | TIM1_AF1_ETRSEL_0)                     /* !< TIM1_ETR is connected to ADC1 AWD1 */
#define TIM_TIM1_ETR_ADC1_AWD2   (TIM1_AF1_ETRSEL_2)                                         /* !< TIM1_ETR is connected to ADC1 AWD2 */
#define TIM_TIM1_ETR_ADC1_AWD3   (TIM1_AF1_ETRSEL_2 | TIM1_AF1_ETRSEL_0)                     /* !< TIM1_ETR is connected to ADC1 AWD3 */
#define TIM_TIM1_ETR_ADC3_AWD1   (TIM1_AF1_ETRSEL_2 | TIM1_AF1_ETRSEL_1)                     /* !< TIM1_ETR is connected to ADC3 AWD1 */
#define TIM_TIM1_ETR_ADC3_AWD2   (TIM1_AF1_ETRSEL_2 | TIM1_AF1_ETRSEL_1 | TIM1_AF1_ETRSEL_0) /* !< TIM1_ETR is connected to ADC3 AWD2 */
#define TIM_TIM1_ETR_ADC3_AWD3   TIM1_AF1_ETRSEL_3                                           /* !< TIM1_ETR is connected to ADC3 AWD3 */

#define TIM_TIM8_ETR_GPIO        0x00000000U                                                 /* !< TIM8_ETR is connected to GPIO */
#define TIM_TIM8_ETR_COMP1       TIM8_AF1_ETRSEL_0                                           /* !< TIM8_ETR is connected to COMP1 OUT */
#define TIM_TIM8_ETR_COMP2       TIM8_AF1_ETRSEL_1                                           /* !< TIM8_ETR is connected to COMP2 OUT */
#define TIM_TIM8_ETR_ADC2_AWD1   (TIM8_AF1_ETRSEL_1 | TIM8_AF1_ETRSEL_0)                     /* !< TIM8_ETR is connected to ADC2 AWD1 */
#define TIM_TIM8_ETR_ADC2_AWD2   (TIM8_AF1_ETRSEL_2)                                         /* !< TIM8_ETR is connected to ADC2 AWD2 */
#define TIM_TIM8_ETR_ADC2_AWD3   (TIM8_AF1_ETRSEL_2 | TIM8_AF1_ETRSEL_0)                     /* !< TIM8_ETR is connected to ADC2 AWD3 */
#define TIM_TIM8_ETR_ADC3_AWD1   (TIM8_AF1_ETRSEL_2 | TIM8_AF1_ETRSEL_1)                     /* !< TIM8_ETR is connected to ADC3 AWD1 */
#define TIM_TIM8_ETR_ADC3_AWD2   (TIM8_AF1_ETRSEL_2 | TIM8_AF1_ETRSEL_1 | TIM8_AF1_ETRSEL_0) /* !< TIM8_ETR is connected to ADC3 AWD2 */
#define TIM_TIM8_ETR_ADC3_AWD3   TIM8_AF1_ETRSEL_3                                           /* !< TIM8_ETR is connected to ADC3 AWD3 */

#define TIM_TIM2_ETR_GPIO        0x00000000U                             /* !< TIM2_ETR is connected to GPIO */
#define TIM_TIM2_ETR_COMP1       (TIM2_AF1_ETRSEL_0)                     /* !< TIM2_ETR is connected to COMP1 OUT */
#define TIM_TIM2_ETR_COMP2       (TIM2_AF1_ETRSEL_1)                     /* !< TIM2_ETR is connected to COMP2 OUT */
#define TIM_TIM2_ETR_RCC_LSE     (TIM2_AF1_ETRSEL_1 | TIM8_AF1_ETRSEL_0) /* !< TIM2_ETR is connected to RCC LSE */
#define TIM_TIM2_ETR_SAI1_FSA    TIM2_AF1_ETRSEL_2                       /* !< TIM2_ETR is connected to SAI1 FS_A */
#define TIM_TIM2_ETR_SAI1_FSB    (TIM2_AF1_ETRSEL_2 | TIM8_AF1_ETRSEL_0) /* !< TIM2_ETR is connected to SAI1 FS_B */

#define TIM_TIM3_ETR_GPIO        0x00000000U          /* !< TIM3_ETR is connected to GPIO */
#define TIM_TIM3_ETR_COMP1       TIM3_AF1_ETRSEL_0    /* !< TIM3_ETR is connected to COMP1 OUT */

#define TIM_TIM5_ETR_GPIO        0x00000000U          /* !< TIM5_ETR is connected to GPIO */
#define TIM_TIM5_ETR_SAI2_FSA    TIM5_AF1_ETRSEL_0    /* !< TIM5_ETR is connected to SAI2 FS_A */
#define TIM_TIM5_ETR_SAI2_FSB    TIM5_AF1_ETRSEL_1    /* !< TIM5_ETR is connected to SAI2 FS_B */
#define TIM_TIM5_ETR_SAI4_FSA    TIM5_AF1_ETRSEL_0    /* !< TIM5_ETR is connected to SAI4 FS_A */
#define TIM_TIM5_ETR_SAI4_FSB    TIM5_AF1_ETRSEL_1    /* !< TIM5_ETR is connected to SAI4 FS_B */

#define TIM_TIM23_ETR_GPIO       0x00000000U          /* !< TIM23_ETR is connected to GPIO */
#define TIM_TIM23_ETR_COMP1      (TIM2_AF1_ETRSEL_0)  /* !< TIM23_ETR is connected to COMP1 OUT */
#define TIM_TIM23_ETR_COMP2      (TIM2_AF1_ETRSEL_1)  /* !< TIM23_ETR is connected to COMP2 OUT */

#define TIM_TIM24_ETR_GPIO        0x00000000U                                /* !< TIM24_ETR is connected to GPIO */
#define TIM_TIM24_ETR_SAI4_FSA    TIM5_AF1_ETRSEL_0                          /* !< TIM24_ETR is connected to SAI4 FS_A */
#define TIM_TIM24_ETR_SAI4_FSB    TIM5_AF1_ETRSEL_1                          /* !< TIM24_ETR is connected to SAI4 FS_B */
#define TIM_TIM24_ETR_SAI1_FSA    (TIM2_AF1_ETRSEL_1 | TIM8_AF1_ETRSEL_0)    /* !< TIM24_ETR is connected to SAI1 FS_A */
#define TIM_TIM24_ETR_SAI1_FSB    TIM2_AF1_ETRSEL_2                          /* !< TIM24_ETR is connected to SAI1 FS_B */
/**
  * @}
  */
#if defined(TIM_BREAK_INPUT_SUPPORT)

/** @defgroup TIMEx_Break_Input TIM Extended Break input
  * @{
  */
#define TIM_BREAKINPUT_BRK     0x00000001U                                      /* !< Timer break input  */
#define TIM_BREAKINPUT_BRK2    0x00000002U                                      /* !< Timer break2 input */
/**
  * @}
  */

/** @defgroup TIMEx_Break_Input_Source TIM Extended Break input source
  * @{
  */
#define TIM_BREAKINPUTSOURCE_BKIN     0x00000001U                               /* !< An external source (GPIO) is connected to the BKIN pin  */
#define TIM_BREAKINPUTSOURCE_COMP1    0x00000002U                               /* !< The COMP1 output is connected to the break input */
#define TIM_BREAKINPUTSOURCE_COMP2    0x00000004U                               /* !< The COMP2 output is connected to the break input */
#define TIM_BREAKINPUTSOURCE_DFSDM1   0x00000008U                               /* !< The analog watchdog output of the DFSDM1 peripheral is connected to the break input */
/**
  * @}
  */

/** @defgroup TIMEx_Break_Input_Source_Enable TIM Extended Break input source enabling
  * @{
  */
#define TIM_BREAKINPUTSOURCE_DISABLE     0x00000000U                            /* !< Break input source is disabled */
#define TIM_BREAKINPUTSOURCE_ENABLE      0x00000001U                            /* !< Break input source is enabled */
/**
  * @}
  */

/** @defgroup TIMEx_Break_Input_Source_Polarity TIM Extended Break input polarity
  * @{
  */
#define TIM_BREAKINPUTSOURCE_POLARITY_LOW     0x00000001U                       /* !< Break input source is active low */
#define TIM_BREAKINPUTSOURCE_POLARITY_HIGH    0x00000000U                       /* !< Break input source is active_high */
/**
  * @}
  */
#endif /* TIM_BREAK_INPUT_SUPPORT */

/** @defgroup TIMEx_Timer_Input_Selection TIM Extended Timer input selection
  * @{
  */
#define TIM_TIM1_TI1_GPIO                          0x00000000U                               /* !< TIM1_TI1 is connected to GPIO */
#define TIM_TIM1_TI1_COMP1                         TIM_TISEL_TI1SEL_0                        /* !< TIM1_TI1 is connected to COMP1 OUT */

#define TIM_TIM8_TI1_GPIO                          0x00000000U                               /* !< TIM8_TI1 is connected to GPIO */
#define TIM_TIM8_TI1_COMP2                         TIM_TISEL_TI1SEL_0                        /* !< TIM8_TI1 is connected to COMP2 OUT */

#define TIM_TIM2_TI4_GPIO                          0x00000000U                               /* !< TIM2_TI4 is connected to GPIO */
#define TIM_TIM2_TI4_COMP1                         TIM_TISEL_TI4SEL_0                        /* !< TIM2_TI4 is connected to COMP1 OUT */
#define TIM_TIM2_TI4_COMP2                         TIM_TISEL_TI4SEL_1                        /* !< TIM2_TI4 is connected to COMP2 OUT */
#define TIM_TIM2_TI4_COMP1_COMP2                   (TIM_TISEL_TI4SEL_0 | TIM_TISEL_TI4SEL_1) /* !< TIM2_TI4 is connected to COMP2 OUT OR COMP2 OUT */

#define TIM_TIM3_TI1_GPIO                          0x00000000U                               /* !< TIM3_TI1 is connected to GPIO */
#define TIM_TIM3_TI1_COMP1                         TIM_TISEL_TI1SEL_0                        /* !< TIM3_TI1 is connected to COMP1 OUT */
#define TIM_TIM3_TI1_COMP2                         TIM_TISEL_TI1SEL_1                        /* !< TIM3_TI1 is connected to COMP2 OUT */
#define TIM_TIM3_TI1_COMP1_COMP2                   (TIM_TISEL_TI1SEL_0 | TIM_TISEL_TI1SEL_1) /* !< TIM3_TI1 is connected to COMP1 OUT or COMP2 OUT */

#define TIM_TIM5_TI1_GPIO                          0x00000000U                               /* !< TIM5_TI1 is connected to GPIO */
#define TIM_TIM5_TI1_CAN_TMP                       TIM_TISEL_TI1SEL_0                        /* !< TIM5_TI1 is connected to CAN TMP */
#define TIM_TIM5_TI1_CAN_RTP                       TIM_TISEL_TI1SEL_1                        /* !< TIM5_TI1 is connected to CAN RTP */

#define TIM_TIM12_TI1_GPIO                         0x00000000U                               /* !< TIM12 TI1 is connected to GPIO */
#define TIM_TIM12_TI1_SPDIF_FS                     TIM_TISEL_TI1SEL_0                        /* !< TIM12 TI1 is connected to SPDIF FS */

#define TIM_TIM15_TI1_GPIO                         0x00000000U                               /* !< TIM15_TI1 is connected to GPIO */
#define TIM_TIM15_TI1_TIM2_CH1                     TIM_TISEL_TI1SEL_0                        /* !< TIM15_TI1 is connected to TIM2 CH1 */
#define TIM_TIM15_TI1_TIM3_CH1                     TIM_TISEL_TI1SEL_1                        /* !< TIM15_TI1 is connected to TIM3 CH1 */
#define TIM_TIM15_TI1_TIM4_CH1                     (TIM_TISEL_TI1SEL_0 | TIM_TISEL_TI1SEL_1) /* !< TIM15_TI1 is connected to TIM4 CH1 */
#define TIM_TIM15_TI1_RCC_LSE                      (TIM_TISEL_TI1SEL_2)                      /* !< TIM15_TI1 is connected to RCC LSE  */
#define TIM_TIM15_TI1_RCC_CSI                      (TIM_TISEL_TI1SEL_2 | TIM_TISEL_TI1SEL_0) /* !< TIM15_TI1 is connected to RCC CSI  */
#define TIM_TIM15_TI1_RCC_MCO2                     (TIM_TISEL_TI1SEL_2 | TIM_TISEL_TI1SEL_1) /* !< TIM15_TI1 is connected to RCC MCO2 */

#define TIM_TIM15_TI2_GPIO                         0x00000000U                               /* !< TIM15_TI2 is connected to GPIO */
#define TIM_TIM15_TI2_TIM2_CH2                     (TIM_TISEL_TI2SEL_0)                      /* !< TIM15_TI2 is connected to TIM2 CH2 */
#define TIM_TIM15_TI2_TIM3_CH2                     (TIM_TISEL_TI2SEL_1)                      /* !< TIM15_TI2 is connected to TIM3 CH2 */
#define TIM_TIM15_TI2_TIM4_CH2                     (TIM_TISEL_TI2SEL_0 | TIM_TISEL_TI2SEL_1) /* !< TIM15_TI2 is connected to TIM4 CH2 */

#define TIM_TIM16_TI1_GPIO                         0x00000000U                               /* !< TIM16 TI1 is connected to GPIO */
#define TIM_TIM16_TI1_RCC_LSI                      TIM_TISEL_TI1SEL_0                        /* !< TIM16 TI1 is connected to RCC LSI */
#define TIM_TIM16_TI1_RCC_LSE                      TIM_TISEL_TI1SEL_1                        /* !< TIM16 TI1 is connected to RCC LSE */
#define TIM_TIM16_TI1_WKUP_IT                      (TIM_TISEL_TI1SEL_0 | TIM_TISEL_TI1SEL_1) /* !< TIM16 TI1 is connected to WKUP_IT */

#define TIM_TIM17_TI1_GPIO                         0x00000000U                               /* !< TIM17 TI1 is connected to GPIO */
#define TIM_TIM17_TI1_SPDIF_FS                     TIM_TISEL_TI1SEL_0                        /* !< TIM17 TI1 is connected to SPDIF FS */
#define TIM_TIM17_TI1_RCC_HSE1MHZ                  TIM_TISEL_TI1SEL_1                        /* !< TIM17 TI1 is connected to RCC HSE 1Mhz */
#define TIM_TIM17_TI1_RCC_MCO1                     (TIM_TISEL_TI1SEL_0 | TIM_TISEL_TI1SEL_1) /* !< TIM17 TI1 is connected to RCC MCO1 */

#define TIM_TIM23_TI4_GPIO                         0x00000000U                               /* !< TIM23_TI4 is connected to GPIO */
#define TIM_TIM23_TI4_COMP1                        TIM_TISEL_TI4SEL_0                        /* !< TIM23_TI4 is connected to COMP1 OUT */
#define TIM_TIM23_TI4_COMP2                        TIM_TISEL_TI4SEL_1                        /* !< TIM23_TI4 is connected to COMP2 OUT */
#define TIM_TIM23_TI4_COMP1_COMP2                  (TIM_TISEL_TI4SEL_0 | TIM_TISEL_TI4SEL_1) /* !< TIM23_TI4 is connected to COMP1 OUT or COMP2 OUT */

#define TIM_TIM24_TI1_GPIO                         0x00000000U                               /* !< TIM24_TI1 is connected to GPIO */
#define TIM_TIM24_TI1_CAN_TMP                      TIM_TISEL_TI1SEL_0                        /* !< TIM24_TI1 is connected to CAN TMP  */
#define TIM_TIM24_TI1_CAN_RTP                      TIM_TISEL_TI1SEL_1                        /* !< TIM24_TI1 is connected to CAN RTP  */
#define TIM_TIM24_TI1_CAN_SOC                      (TIM_TISEL_TI4SEL_0 | TIM_TISEL_TI4SEL_1) /* !< TIM24_TI1 is connected to CAN SOC */
/**
  * @}
  */

/**
  * @}
  */
/* End of exported constants -------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/** @defgroup TIMEx_Exported_Macros TIM Extended Exported Macros
  * @{
  */

/**
  * @}
  */
/* End of exported macro -----------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/** @defgroup TIMEx_Private_Macros TIM Extended Private Macros
  * @{
  */
#define IS_TIM_BREAKINPUT(__BREAKINPUT__)  (((__BREAKINPUT__) == TIM_BREAKINPUT_BRK)  || \
                                            ((__BREAKINPUT__) == TIM_BREAKINPUT_BRK2))

#define IS_TIM_BREAKINPUTSOURCE(__SOURCE__)  (((__SOURCE__) == TIM_BREAKINPUTSOURCE_BKIN)  || \
                                              ((__SOURCE__) == TIM_BREAKINPUTSOURCE_COMP1) || \
                                              ((__SOURCE__) == TIM_BREAKINPUTSOURCE_COMP2) || \
                                              ((__SOURCE__) == TIM_BREAKINPUTSOURCE_DFSDM1))

#define IS_TIM_BREAKINPUTSOURCE_STATE(__STATE__)  (((__STATE__) == TIM_BREAKINPUTSOURCE_DISABLE)  || \
                                                   ((__STATE__) == TIM_BREAKINPUTSOURCE_ENABLE))

#define IS_TIM_BREAKINPUTSOURCE_POLARITY(__POLARITY__)  (((__POLARITY__) == TIM_BREAKINPUTSOURCE_POLARITY_LOW)  || \
                                                         ((__POLARITY__) == TIM_BREAKINPUTSOURCE_POLARITY_HIGH))

#define IS_TIM_TISEL(__TISEL__)  (((__TISEL__) == TIM_TIM1_TI1_GPIO)         ||\
                                  ((__TISEL__) == TIM_TIM1_TI1_COMP1)        ||\
                                  ((__TISEL__) == TIM_TIM8_TI1_GPIO)         ||\
                                  ((__TISEL__) == TIM_TIM8_TI1_COMP2)        ||\
                                  ((__TISEL__) == TIM_TIM2_TI4_GPIO)         ||\
                                  ((__TISEL__) == TIM_TIM2_TI4_COMP1)        ||\
                                  ((__TISEL__) == TIM_TIM2_TI4_COMP2)        ||\
                                  ((__TISEL__) == TIM_TIM2_TI4_COMP1_COMP2)  ||\
                                  ((__TISEL__) == TIM_TIM3_TI1_GPIO)         ||\
                                  ((__TISEL__) == TIM_TIM3_TI1_COMP1)        ||\
                                  ((__TISEL__) == TIM_TIM3_TI1_COMP2)        ||\
                                  ((__TISEL__) == TIM_TIM3_TI1_COMP1_COMP2)  ||\
                                  ((__TISEL__) == TIM_TIM5_TI1_GPIO)         ||\
                                  ((__TISEL__) == TIM_TIM5_TI1_CAN_TMP)      ||\
                                  ((__TISEL__) == TIM_TIM5_TI1_CAN_RTP)      ||\
                                  ((__TISEL__) == TIM_TIM12_TI1_SPDIF_FS)    ||\
                                  ((__TISEL__) == TIM_TIM12_TI1_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_TIM2_CH1)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_TIM3_CH1)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_TIM4_CH1)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_RCC_LSE)     ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_RCC_CSI)     ||\
                                  ((__TISEL__) == TIM_TIM15_TI1_RCC_MCO2)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI2_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM15_TI2_TIM2_CH2)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI2_TIM3_CH2)    ||\
                                  ((__TISEL__) == TIM_TIM15_TI2_TIM4_CH2)    ||\
                                  ((__TISEL__) == TIM_TIM16_TI1_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM16_TI1_RCC_LSI)     ||\
                                  ((__TISEL__) == TIM_TIM16_TI1_RCC_LSE)     ||\
                                  ((__TISEL__) == TIM_TIM16_TI1_WKUP_IT)     ||\
                                  ((__TISEL__) == TIM_TIM17_TI1_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM17_TI1_SPDIF_FS)    ||\
                                  ((__TISEL__) == TIM_TIM17_TI1_RCC_HSE1MHZ) ||\
                                  ((__TISEL__) == TIM_TIM17_TI1_RCC_MCO1)    ||\
                                  ((__TISEL__) == TIM_TIM23_TI4_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM23_TI4_COMP1)       ||\
                                  ((__TISEL__) == TIM_TIM23_TI4_COMP2)       ||\
                                  ((__TISEL__) == TIM_TIM23_TI4_COMP1_COMP2) ||\
                                  ((__TISEL__) == TIM_TIM24_TI1_GPIO)        ||\
                                  ((__TISEL__) == TIM_TIM24_TI1_CAN_TMP)     ||\
                                  ((__TISEL__) == TIM_TIM24_TI1_CAN_RTP)     ||\
                                  ((__TISEL__) == TIM_TIM24_TI1_CAN_SOC))

#define IS_TIM_REMAP(__RREMAP__)     (((__RREMAP__) == TIM_TIM1_ETR_GPIO)      ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC1_AWD1) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC1_AWD2) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC1_AWD3) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC3_AWD1) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC3_AWD2) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_ADC3_AWD3) ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_COMP1)     ||\
                                      ((__RREMAP__) == TIM_TIM1_ETR_COMP2)     ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_GPIO)      ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC2_AWD1) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC2_AWD2) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC2_AWD3) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC3_AWD1) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC3_AWD2) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_ADC3_AWD3) ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_COMP1)     ||\
                                      ((__RREMAP__) == TIM_TIM8_ETR_COMP2)     ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_GPIO)      ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_COMP1)     ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_COMP2)     ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_RCC_LSE)   ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_SAI1_FSA)  ||\
                                      ((__RREMAP__) == TIM_TIM2_ETR_SAI1_FSB)  ||\
                                      ((__RREMAP__) == TIM_TIM3_ETR_GPIO)      ||\
                                      ((__RREMAP__) == TIM_TIM3_ETR_COMP1)     ||\
                                      ((__RREMAP__) == TIM_TIM5_ETR_GPIO)      ||\
                                      ((__RREMAP__) == TIM_TIM5_ETR_SAI2_FSA)  ||\
                                      ((__RREMAP__) == TIM_TIM5_ETR_SAI2_FSB)  ||\
                                      ((__RREMAP__) == TIM_TIM23_ETR_GPIO)     ||\
                                      ((__RREMAP__) == TIM_TIM23_ETR_COMP1)    ||\
                                      ((__RREMAP__) == TIM_TIM23_ETR_COMP2)    ||\
                                      ((__RREMAP__) == TIM_TIM24_ETR_GPIO)     ||\
                                      ((__RREMAP__) == TIM_TIM24_ETR_SAI4_FSA) ||\
                                      ((__RREMAP__) == TIM_TIM24_ETR_SAI4_FSB) ||\
                                      ((__RREMAP__) == TIM_TIM24_ETR_SAI1_FSA) ||\
                                      ((__RREMAP__) == TIM_TIM24_ETR_SAI1_FSB))

/**
  * @}
  */
/* End of private macro ------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup TIMEx_Exported_Functions TIM Extended Exported Functions
  * @{
  */

/** @addtogroup TIMEx_Exported_Functions_Group1 Extended Timer Hall Sensor functions
  *  @brief    Timer Hall Sensor functions
  * @{
  */
/*  Timer Hall Sensor functions  **********************************************/
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);

/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group2 Extended Timer Complementary Output Compare functions
  *  @brief   Timer Complementary Output Compare functions
  * @{
  */
/*  Timer Complementary Output Compare functions  *****************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group3 Extended Timer Complementary PWM functions
  *  @brief    Timer Complementary PWM functions
  * @{
  */
/*  Timer Complementary PWM functions  ****************************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group4 Extended Timer Complementary One Pulse functions
  *  @brief    Timer Complementary One Pulse functions
  * @{
  */
/*  Timer Complementary One Pulse functions  **********************************/
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group5 Extended Peripheral Control functions
  *  @brief    Peripheral Control functions
  * @{
  */
/* Extended Control functions  ************************************************/
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                              uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                 uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                  uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                        TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
#if defined(TIM_BREAK_INPUT_SUPPORT)
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput,
                                             TIMEx_BreakInputConfigTypeDef *sBreakInputConfig);
#endif /* TIM_BREAK_INPUT_SUPPORT */
HAL_StatusTypeDef HAL_TIMEx_GroupChannel5(TIM_HandleTypeDef *htim, uint32_t Channels);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);
HAL_StatusTypeDef  HAL_TIMEx_TISelection(TIM_HandleTypeDef *htim, uint32_t TISelection, uint32_t Channel);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group6 Extended Callbacks functions
  * @brief    Extended Callbacks functions
  * @{
  */
/* Extended Callback **********************************************************/
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_Break2Callback(TIM_HandleTypeDef *htim);
/**
  * @}
  */

/** @addtogroup TIMEx_Exported_Functions_Group7 Extended Peripheral State functions
  * @brief    Extended Peripheral State functions
  * @{
  */
/* Extended Peripheral State functions  ***************************************/
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim);
/**
  * @}
  */

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

/* Private functions----------------------------------------------------------*/
/** @addtogroup TIMEx_Private_Functions TIMEx Private Functions
  * @{
  */
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);
/**
  * @}
  */
/* End of private functions --------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* STM32H7xx_HAL_TIM_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
