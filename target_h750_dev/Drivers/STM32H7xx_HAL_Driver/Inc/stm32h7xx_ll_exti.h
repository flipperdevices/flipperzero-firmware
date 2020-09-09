/**
  ******************************************************************************
  * @file    stm32h7xx_ll_exti.h
  * @author  MCD Application Team
  * @brief   Header file of EXTI LL module.
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
#ifndef __STM32H7xx_LL_EXTI_H
#define __STM32H7xx_LL_EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (EXTI)

/** @defgroup EXTI_LL EXTI
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private Macros ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup EXTI_LL_Private_Macros EXTI Private Macros
  * @{
  */
/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/
/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup EXTI_LL_ES_INIT EXTI Exported Init structure
  * @{
  */
typedef struct
{

  uint32_t Line_0_31;           /*!< Specifies the EXTI lines to be enabled or disabled for Lines in range 0 to 31
                                     This parameter can be any combination of @ref EXTI_LL_EC_LINE */

  uint32_t Line_32_63;          /*!< Specifies the EXTI lines to be enabled or disabled for Lines in range 32 to 63
                                     This parameter can be any combination of @ref EXTI_LL_EC_LINE */

  uint32_t Line_64_95;          /*!< Specifies the EXTI lines to be enabled or disabled for Lines in range 64 to 95
                                     This parameter can be any combination of @ref EXTI_LL_EC_LINE */

  FunctionalState LineCommand;  /*!< Specifies the new state of the selected EXTI lines.
                                     This parameter can be set either to ENABLE or DISABLE */

  uint8_t Mode;                 /*!< Specifies the mode for the EXTI lines.
                                     This parameter can be a value of @ref EXTI_LL_EC_MODE. */

  uint8_t Trigger;              /*!< Specifies the trigger signal active edge for the EXTI lines.
                                     This parameter can be a value of @ref EXTI_LL_EC_TRIGGER. */
} LL_EXTI_InitTypeDef;

/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup EXTI_LL_Exported_Constants EXTI Exported Constants
  * @{
  */

/** @defgroup EXTI_LL_EC_LINE LINE
  * @{
  */
#define LL_EXTI_LINE_0                 EXTI_IMR1_IM0           /*!< Extended line 0 */
#define LL_EXTI_LINE_1                 EXTI_IMR1_IM1           /*!< Extended line 1 */
#define LL_EXTI_LINE_2                 EXTI_IMR1_IM2           /*!< Extended line 2 */
#define LL_EXTI_LINE_3                 EXTI_IMR1_IM3           /*!< Extended line 3 */
#define LL_EXTI_LINE_4                 EXTI_IMR1_IM4           /*!< Extended line 4 */
#define LL_EXTI_LINE_5                 EXTI_IMR1_IM5           /*!< Extended line 5 */
#define LL_EXTI_LINE_6                 EXTI_IMR1_IM6           /*!< Extended line 6 */
#define LL_EXTI_LINE_7                 EXTI_IMR1_IM7           /*!< Extended line 7 */
#define LL_EXTI_LINE_8                 EXTI_IMR1_IM8           /*!< Extended line 8 */
#define LL_EXTI_LINE_9                 EXTI_IMR1_IM9           /*!< Extended line 9 */
#define LL_EXTI_LINE_10                EXTI_IMR1_IM10          /*!< Extended line 10 */
#define LL_EXTI_LINE_11                EXTI_IMR1_IM11          /*!< Extended line 11 */
#define LL_EXTI_LINE_12                EXTI_IMR1_IM12          /*!< Extended line 12 */
#define LL_EXTI_LINE_13                EXTI_IMR1_IM13          /*!< Extended line 13 */
#define LL_EXTI_LINE_14                EXTI_IMR1_IM14          /*!< Extended line 14 */
#define LL_EXTI_LINE_15                EXTI_IMR1_IM15          /*!< Extended line 15 */
#define LL_EXTI_LINE_16                EXTI_IMR1_IM16          /*!< Extended line 16 */
#define LL_EXTI_LINE_17                EXTI_IMR1_IM17          /*!< Extended line 17 */
#define LL_EXTI_LINE_18                EXTI_IMR1_IM18          /*!< Extended line 18 */
#define LL_EXTI_LINE_19                EXTI_IMR1_IM19          /*!< Extended line 19 */
#define LL_EXTI_LINE_20                EXTI_IMR1_IM20          /*!< Extended line 20 */
#define LL_EXTI_LINE_21                EXTI_IMR1_IM21          /*!< Extended line 21 */
#define LL_EXTI_LINE_22                EXTI_IMR1_IM22          /*!< Extended line 22 */
#define LL_EXTI_LINE_23                EXTI_IMR1_IM23          /*!< Extended line 23 */
#define LL_EXTI_LINE_24                EXTI_IMR1_IM24          /*!< Extended line 24 */
#define LL_EXTI_LINE_25                EXTI_IMR1_IM25          /*!< Extended line 25 */
#define LL_EXTI_LINE_26                EXTI_IMR1_IM26          /*!< Extended line 26 */
#define LL_EXTI_LINE_27                EXTI_IMR1_IM27          /*!< Extended line 27 */
#define LL_EXTI_LINE_28                EXTI_IMR1_IM28          /*!< Extended line 28 */
#define LL_EXTI_LINE_29                EXTI_IMR1_IM29          /*!< Extended line 29 */
#define LL_EXTI_LINE_30                EXTI_IMR1_IM30          /*!< Extended line 30 */
#define LL_EXTI_LINE_31                EXTI_IMR1_IM31          /*!< Extended line 31 */
#define LL_EXTI_LINE_ALL_0_31          EXTI_IMR1_IM            /*!< All Extended line not reserved*/

#define LL_EXTI_LINE_32                EXTI_IMR2_IM32          /*!< Extended line 32 */
#define LL_EXTI_LINE_33                EXTI_IMR2_IM33          /*!< Extended line 33 */
#define LL_EXTI_LINE_34                EXTI_IMR2_IM34          /*!< Extended line 34 */
#define LL_EXTI_LINE_35                EXTI_IMR2_IM35          /*!< Extended line 35 */
#define LL_EXTI_LINE_36                EXTI_IMR2_IM36          /*!< Extended line 36 */
#define LL_EXTI_LINE_37                EXTI_IMR2_IM37          /*!< Extended line 37 */
#define LL_EXTI_LINE_38                EXTI_IMR2_IM38          /*!< Extended line 38 */
#define LL_EXTI_LINE_39                EXTI_IMR2_IM39          /*!< Extended line 39 */
#define LL_EXTI_LINE_40                EXTI_IMR2_IM40          /*!< Extended line 40 */
#define LL_EXTI_LINE_41                EXTI_IMR2_IM41          /*!< Extended line 41 */
#define LL_EXTI_LINE_42                EXTI_IMR2_IM42          /*!< Extended line 42 */
#define LL_EXTI_LINE_43                EXTI_IMR2_IM43          /*!< Extended line 43 */
#if defined(USB2_OTG_FS)
#define LL_EXTI_LINE_44                EXTI_IMR2_IM44          /*!< Extended line 44 */
#endif /* USB2_OTG_FS */
#if defined(DSI)
#define LL_EXTI_LINE_46                EXTI_IMR2_IM46          /*!< Extended line 46 */
#endif /* DSI */
#define LL_EXTI_LINE_47                EXTI_IMR2_IM47          /*!< Extended line 47 */
#define LL_EXTI_LINE_48                EXTI_IMR2_IM48          /*!< Extended line 48 */
#define LL_EXTI_LINE_49                EXTI_IMR2_IM49          /*!< Extended line 49 */
#define LL_EXTI_LINE_50                EXTI_IMR2_IM50          /*!< Extended line 50 */
#define LL_EXTI_LINE_51                EXTI_IMR2_IM51          /*!< Extended line 51 */
#define LL_EXTI_LINE_52                EXTI_IMR2_IM52          /*!< Extended line 52 */
#define LL_EXTI_LINE_53                EXTI_IMR2_IM53          /*!< Extended line 53 */
#define LL_EXTI_LINE_54                EXTI_IMR2_IM54          /*!< Extended line 54 */
#define LL_EXTI_LINE_55                EXTI_IMR2_IM55          /*!< Extended line 55 */
#define LL_EXTI_LINE_56                EXTI_IMR2_IM56          /*!< Extended line 56 */
#define LL_EXTI_LINE_57                EXTI_IMR2_IM57          /*!< Extended line 57 */
#define LL_EXTI_LINE_58                EXTI_IMR2_IM58          /*!< Extended line 58 */
#define LL_EXTI_LINE_59                EXTI_IMR2_IM59          /*!< Extended line 59 */
#define LL_EXTI_LINE_60                EXTI_IMR2_IM60          /*!< Extended line 60 */
#define LL_EXTI_LINE_61                EXTI_IMR2_IM61          /*!< Extended line 61 */
#define LL_EXTI_LINE_62                EXTI_IMR2_IM62          /*!< Extended line 62 */
#define LL_EXTI_LINE_63                EXTI_IMR2_IM63          /*!< Extended line 63 */
#define LL_EXTI_LINE_ALL_32_63         EXTI_IMR2_IM            /*!< All Extended line not reserved*/

#define LL_EXTI_LINE_64                EXTI_IMR3_IM64          /*!< Extended line 64 */
#define LL_EXTI_LINE_65                EXTI_IMR3_IM65          /*!< Extended line 65 */
#define LL_EXTI_LINE_66                EXTI_IMR3_IM66          /*!< Extended line 66 */
#define LL_EXTI_LINE_67                EXTI_IMR3_IM67          /*!< Extended line 67 */
#define LL_EXTI_LINE_68                EXTI_IMR3_IM68          /*!< Extended line 68 */
#define LL_EXTI_LINE_69                EXTI_IMR3_IM69          /*!< Extended line 69 */
#define LL_EXTI_LINE_70                EXTI_IMR3_IM70          /*!< Extended line 70 */
#define LL_EXTI_LINE_71                EXTI_IMR3_IM71          /*!< Extended line 71 */
#define LL_EXTI_LINE_72                EXTI_IMR3_IM72          /*!< Extended line 72 */
#define LL_EXTI_LINE_73                EXTI_IMR3_IM73          /*!< Extended line 73 */
#define LL_EXTI_LINE_74                EXTI_IMR3_IM74          /*!< Extended line 74 */
#if defined(ADC3)
#define LL_EXTI_LINE_75                EXTI_IMR3_IM75          /*!< Extended line 75 */
#endif /* ADC3 */
#if defined(SAI4)
#define LL_EXTI_LINE_76                EXTI_IMR3_IM76          /*!< Extended line 76 */
#endif /* SAI4 */
#if defined(DUAL_CORE)
#define LL_EXTI_LINE_77                EXTI_IMR3_IM77          /*!< Extended line 77 */
#define LL_EXTI_LINE_78                EXTI_IMR3_IM78          /*!< Extended line 78 */
#define LL_EXTI_LINE_79                EXTI_IMR3_IM79          /*!< Extended line 79 */
#define LL_EXTI_LINE_80                EXTI_IMR3_IM80          /*!< Extended line 80 */
#define LL_EXTI_LINE_82                EXTI_IMR3_IM82          /*!< Extended line 82 */
#define LL_EXTI_LINE_84                EXTI_IMR3_IM84          /*!< Extended line 84 */
#endif /* DUAL_CORE */
#define LL_EXTI_LINE_85                EXTI_IMR3_IM85          /*!< Extended line 85 */
#if defined(ETH)
#define LL_EXTI_LINE_86                EXTI_IMR3_IM86          /*!< Extended line 86 */
#endif /* ETH */
#define LL_EXTI_LINE_87                EXTI_IMR3_IM87          /*!< Extended line 87 */
#if defined(DTS)
#define LL_EXTI_LINE_88                EXTI_IMR3_IM88          /*!< Extended line 87 */
#endif /* DTS */
#define LL_EXTI_LINE_ALL_64_95         EXTI_IMR3_IM            /*!< All Extended line not reserved*/


#define LL_EXTI_LINE_ALL               (0xFFFFFFFFU)  /*!< All Extended line */

#if defined(USE_FULL_LL_DRIVER)
#define LL_EXTI_LINE_NONE              (0x00000000U)  /*!< None Extended line */
#endif /*USE_FULL_LL_DRIVER*/

/**
  * @}
  */
#if defined(USE_FULL_LL_DRIVER)

/** @defgroup EXTI_LL_EC_MODE Mode
  * @{
  */
#define LL_EXTI_MODE_IT               ((uint8_t)0x01U)      /*!< Cortex-M7 Interrupt Mode  */
#define LL_EXTI_MODE_EVENT            ((uint8_t)0x02U)      /*!< Cortex-M7 Event Mode */
#define LL_EXTI_MODE_IT_EVENT         ((uint8_t)0x03U)      /*!< Cortex-M7 Interrupt & Event Mode */

#if defined(DUAL_CORE)
#define LL_EXTI_MODE_C1_IT            LL_EXTI_MODE_IT       /*!< Cortex-M7 Interrupt Mode */
#define LL_EXTI_MODE_C1_EVENT         LL_EXTI_MODE_EVENT    /*!< Cortex-M7 Event Mode */
#define LL_EXTI_MODE_C1_IT_EVENT      LL_EXTI_MODE_IT_EVENT /*!< Cortex-M7 Interrupt & Event Mode */

#define LL_EXTI_MODE_C2_IT            ((uint8_t)0x10U)      /*!< Cortex-M4 Interrupt Mode  */
#define LL_EXTI_MODE_C2_EVENT         ((uint8_t)0x20U)      /*!< Cortex-M4 Event Mode  */
#define LL_EXTI_MODE_C2_IT_EVENT      ((uint8_t)0x30U)      /*!< Cortex-M4 Interrupt & Event Mode */
#endif /* DUAL_CORE */

/**
  * @}
  */

/** @defgroup EXTI_LL_EC_TRIGGER Edge Trigger
  * @{
  */
#define LL_EXTI_TRIGGER_NONE            ((uint8_t)0x00U) /*!< No Trigger Mode */
#define LL_EXTI_TRIGGER_RISING          ((uint8_t)0x01U) /*!< Trigger Rising Mode */
#define LL_EXTI_TRIGGER_FALLING         ((uint8_t)0x02U) /*!< Trigger Falling Mode */
#define LL_EXTI_TRIGGER_RISING_FALLING  ((uint8_t)0x03U) /*!< Trigger Rising & Falling Mode */

/**
  * @}
  */

/** @defgroup EXTI_LL_D3_PEND_CLR D3 Pend Clear Source
  * @{
  */
#define LL_EXTI_D3_PEND_CLR_DMACH6      ((uint8_t)0x00U) /*!< DMA ch6 event selected as D3 domain pendclear source */
#define LL_EXTI_D3_PEND_CLR_DMACH7      ((uint8_t)0x01U) /*!< DMA ch7 event selected as D3 domain pendclear source */
#if defined (LPTIM4)
#define LL_EXTI_D3_PEND_CLR_LPTIM4      ((uint8_t)0x02U) /*!< LPTIM4 out selected as D3 domain pendclear source */
#else
#define LL_EXTI_D3_PEND_CLR_LPTIM2      ((uint8_t)0x02U) /*!< LPTIM2 out selected as D3 domain pendclear source */
#endif /*LPTIM4*/
#if defined (LPTIM5)
#define LL_EXTI_D3_PEND_CLR_LPTIM5      ((uint8_t)0x03U) /*!< LPTIM5 out selected as D3 domain pendclear source */
#else
#define LL_EXTI_D3_PEND_CLR_LPTIM3      ((uint8_t)0x02U) /*!< LPTIM3 out selected as D3 domain pendclear source */
#endif /*LPTIM5*/
/**
  * @}
  */


#endif /*USE_FULL_LL_DRIVER*/


/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup EXTI_LL_Exported_Macros EXTI Exported Macros
  * @{
  */

/** @defgroup EXTI_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in EXTI register
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_EXTI_WriteReg(__REG__, __VALUE__) WRITE_REG(EXTI->__REG__, (__VALUE__))

/**
  * @brief  Read a value in EXTI register
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_EXTI_ReadReg(__REG__) READ_REG(EXTI->__REG__)

/**
  * @}
  */


/**
  * @}
  */



/* Exported functions --------------------------------------------------------*/
/** @defgroup EXTI_LL_Exported_Functions EXTI Exported Functions
 * @{
 */
/** @defgroup EXTI_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 0 to 31
  * @rmtoll IMR1         IMx           LL_EXTI_EnableIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableIT_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->IMR1, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 32 to 63
  * @rmtoll IMR2         IMx           LL_EXTI_EnableIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableIT_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->IMR2, ExtiLine);
}


/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 64 to 95
  * @rmtoll IMR3         IMx           LL_EXTI_EnableIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableIT_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->IMR3, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 0 to 31
  * @rmtoll IMR1         IMx           LL_EXTI_DisableIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableIT_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->IMR1, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 32 to 63
  * @rmtoll IMR2         IMx           LL_EXTI_DisableIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableIT_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->IMR2, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 64 to 95
  * @rmtoll IMR3         IMx           LL_EXTI_DisableIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableIT_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->IMR3, ExtiLine);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 0 to 31
  * @rmtoll IMR1         IMx           LL_EXTI_IsEnabledIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledIT_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->IMR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 32 to 63
  * @rmtoll IMR2         IMx           LL_EXTI_IsEnabledIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledIT_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->IMR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 64 to 95
  * @rmtoll IMR3         IMx           LL_EXTI_IsEnabledIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledIT_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->IMR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

#if defined(DUAL_CORE)
/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 0 to 31 for cpu2
  * @rmtoll C2IMR1         IMx           LL_C2_EXTI_EnableIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableIT_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2IMR1, ExtiLine);
}


/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 32 to 63 for cpu2
  * @rmtoll C2IMR2         IMx           LL_C2_EXTI_EnableIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableIT_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2IMR2, ExtiLine);
}


/**
  * @brief  Enable ExtiLine Interrupt request for Lines in range 64 to 95
  * @rmtoll C2IMR3         IMx           LL_C2_EXTI_EnableIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableIT_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2IMR3, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 0 to 31 for cpu2
  * @rmtoll C2IMR1         IMx           LL_C2_EXTI_DisableIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableIT_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2IMR1, ExtiLine);
}



/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 32 to 63 for cpu2
  * @rmtoll C2IMR2         IMx           LL_C2_EXTI_DisableIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableIT_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2IMR2, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Interrupt request for Lines in range 64 to 95 for cpu2
  * @rmtoll C2IMR3         IMx           LL_C2_EXTI_DisableIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableIT_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2IMR3, ExtiLine);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 0 to 31 for cpu2
  * @rmtoll C2IMR1         IMx           LL_C2_EXTI_IsEnabledIT_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledIT_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2IMR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 32 to 63 for cpu2
  * @rmtoll C2IMR2         IMx           LL_C2_EXTI_IsEnabledIT_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledIT_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2IMR2, ExtiLine) == (ExtiLine))? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Interrupt request is enabled for Lines in range 64 to 95
  * @rmtoll C2IMR3         IMx           LL_C2_EXTI_IsEnabledIT_64_95
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledIT_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2IMR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

#endif /* DUAL_CORE */


/**
  * @}
  */

/** @defgroup EXTI_LL_EF_Event_Management Event_Management
  * @{
  */

/**
  * @brief  Enable ExtiLine Event request for Lines in range 0 to 31
  * @rmtoll EMR1         EMx           LL_EXTI_EnableEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableEvent_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->EMR1, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Event request for Lines in range 32 to 63
  * @rmtoll EMR2         EMx           LL_EXTI_EnableEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableEvent_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->EMR2, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Event request for Lines in range 64 to 95
  * @rmtoll EMR3         EMx           LL_EXTI_EnableEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableEvent_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->EMR3, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Event request for Lines in range 0 to 31
  * @rmtoll EMR1         EMx           LL_EXTI_DisableEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableEvent_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->EMR1, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Event request for Lines in range 32 to 63
  * @rmtoll EMR2         EMx           LL_EXTI_DisableEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableEvent_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->EMR2, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Event request for Lines in range 64 to 95
  * @rmtoll EMR3         EMx           LL_EXTI_DisableEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableEvent_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->EMR3, ExtiLine);
}

/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 0 to 31
  * @rmtoll EMR1         EMx           LL_EXTI_IsEnabledEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledEvent_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->EMR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 32 to 63
  * @rmtoll EMR2         EMx           LL_EXTI_IsEnabledEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledEvent_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->EMR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 64 to 95
  * @rmtoll EMR3         EMx           LL_EXTI_IsEnabledEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledEvent_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->EMR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

#if defined(DUAL_CORE)

/**
  * @brief  Enable ExtiLine Event request for Lines in range 0 to 31 for cpu2
  * @rmtoll C2EMR1         EMx           LL_C2_EXTI_EnableEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableEvent_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2EMR1, ExtiLine);
}


/**
  * @brief  Enable ExtiLine Event request for Lines in range 32 to 63 for cpu2
  * @rmtoll C2EMR2         EMx           LL_C2_EXTI_EnableEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableEvent_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2EMR2, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Event request for Lines in range 64 to 95 for cpu2
  * @rmtoll C2EMR3         EMx           LL_C2_EXTI_EnableEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableEvent_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2EMR3, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Event request for Lines in range 0 to 31 for cpu2
  * @rmtoll C2EMR1         EMx           LL_C2_EXTI_DisableEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableEvent_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2EMR1, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Event request for Lines in range 32 to 63 for cpu2
  * @rmtoll C2EMR2         EMx           LL_C2_EXTI_DisableEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableEvent_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2EMR2, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Event request for Lines in range 64 to 95 for cpu2
  * @rmtoll C2EMR3         EMx           LL_C2_EXTI_DisableEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_DisableEvent_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->C2EMR3, ExtiLine);
}


/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 0 to 31 for cpu2
  * @rmtoll C2EMR1         EMx           LL_C2_EXTI_IsEnabledEvent_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_22
  *         @arg @ref LL_EXTI_LINE_23
  *         @arg @ref LL_EXTI_LINE_24
  *         @arg @ref LL_EXTI_LINE_25
  *         @arg @ref LL_EXTI_LINE_26
  *         @arg @ref LL_EXTI_LINE_27
  *         @arg @ref LL_EXTI_LINE_28
  *         @arg @ref LL_EXTI_LINE_29
  *         @arg @ref LL_EXTI_LINE_30
  *         @arg @ref LL_EXTI_LINE_31
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledEvent_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2EMR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 32 to 63 for cpu2
  * @rmtoll C2EMR2         EMx           LL_C2_EXTI_IsEnabledEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_32
  *         @arg @ref LL_EXTI_LINE_33
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_36
  *         @arg @ref LL_EXTI_LINE_37
  *         @arg @ref LL_EXTI_LINE_38
  *         @arg @ref LL_EXTI_LINE_39
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_42
  *         @arg @ref LL_EXTI_LINE_43
  *         @arg @ref LL_EXTI_LINE_44
  *         @arg @ref LL_EXTI_LINE_46
  *         @arg @ref LL_EXTI_LINE_47
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  *         @arg @ref LL_EXTI_LINE_54
  *         @arg @ref LL_EXTI_LINE_55
  *         @arg @ref LL_EXTI_LINE_56
  *         @arg @ref LL_EXTI_LINE_57
  *         @arg @ref LL_EXTI_LINE_58
  *         @arg @ref LL_EXTI_LINE_59
  *         @arg @ref LL_EXTI_LINE_60
  *         @arg @ref LL_EXTI_LINE_61
  *         @arg @ref LL_EXTI_LINE_62
  *         @arg @ref LL_EXTI_LINE_63
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledEvent_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2EMR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Indicate if ExtiLine Event request is enabled for Lines in range 64 to 95 for cpu2
  * @rmtoll C2EMR3         EMx           LL_C2_EXTI_IsEnabledEvent_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_64
  *         @arg @ref LL_EXTI_LINE_65
  *         @arg @ref LL_EXTI_LINE_66
  *         @arg @ref LL_EXTI_LINE_67
  *         @arg @ref LL_EXTI_LINE_68
  *         @arg @ref LL_EXTI_LINE_69
  *         @arg @ref LL_EXTI_LINE_70
  *         @arg @ref LL_EXTI_LINE_71
  *         @arg @ref LL_EXTI_LINE_72
  *         @arg @ref LL_EXTI_LINE_73
  *         @arg @ref LL_EXTI_LINE_74
  *         @arg @ref LL_EXTI_LINE_75
  *         @arg @ref LL_EXTI_LINE_76
  *         @arg @ref LL_EXTI_LINE_77
  *         @arg @ref LL_EXTI_LINE_78
  *         @arg @ref LL_EXTI_LINE_79
  *         @arg @ref LL_EXTI_LINE_80
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_87
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsEnabledEvent_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2EMR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


#endif /* DUAL_CORE */

/**
  * @}
  */

/** @defgroup EXTI_LL_EF_Rising_Trigger_Management Rising_Trigger_Management
  * @{
  */

/**
  * @brief  Enable ExtiLine Rising Edge Trigger for Lines in range 0 to 31
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR1        RTx           LL_EXTI_EnableRisingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableRisingTrig_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->RTSR1, ExtiLine);

}

/**
  * @brief  Enable ExtiLine Rising Edge Trigger for Lines in range 32 to 63
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR2        RTx           LL_EXTI_EnableRisingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableRisingTrig_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->RTSR2, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Rising Edge Trigger for Lines in range 64 to 95
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR3        RTx           LL_EXTI_EnableRisingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableRisingTrig_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->RTSR3, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Rising Edge Trigger for Lines in range 0 to 31
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR1        RTx           LL_EXTI_DisableRisingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableRisingTrig_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->RTSR1, ExtiLine);

}

/**
  * @brief  Disable ExtiLine Rising Edge Trigger for Lines in range 32 to 63
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR2        RTx           LL_EXTI_DisableRisingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableRisingTrig_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->RTSR2, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Rising Edge Trigger for Lines in range 64 to 95
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR3        RTx           LL_EXTI_DisableRisingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableRisingTrig_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->RTSR3, ExtiLine);
}

/**
  * @brief  Check if rising edge trigger is enabled for Lines in range 0 to 31
  * @rmtoll RTSR1        RTx           LL_EXTI_IsEnabledRisingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledRisingTrig_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->RTSR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Check if rising edge trigger is enabled for Lines in range 32 to 63
  * @rmtoll RTSR2        RTx           LL_EXTI_IsEnabledRisingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledRisingTrig_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->RTSR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if rising edge trigger is enabled for Lines in range 64 to 95
  * @rmtoll RTSR3        RTx           LL_EXTI_IsEnabledRisingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledRisingTrig_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->RTSR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @}
  */

/** @defgroup EXTI_LL_EF_Falling_Trigger_Management Falling_Trigger_Management
  * @{
  */

/**
  * @brief  Enable ExtiLine Falling Edge Trigger for Lines in range 0 to 31
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll FTSR1        FTx           LL_EXTI_EnableFallingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableFallingTrig_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->FTSR1, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Falling Edge Trigger for Lines in range 32 to 63
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a Falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll FTSR2        FTx           LL_EXTI_EnableFallingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableFallingTrig_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->FTSR2, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Falling Edge Trigger for Lines in range 64 to 95
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a Falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll FTSR3        FTx           LL_EXTI_EnableFallingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableFallingTrig_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->FTSR3, ExtiLine);
}


/**
  * @brief  Disable ExtiLine Falling Edge Trigger for Lines in range 0 to 31
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a Falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for the same interrupt line.
  *       In this case, both generate a trigger condition.
  * @rmtoll FTSR1        FTx           LL_EXTI_DisableFallingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableFallingTrig_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->FTSR1, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Falling Edge Trigger for Lines in range 32 to 63
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a Falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for the same interrupt line.
  *       In this case, both generate a trigger condition.
  * @rmtoll FTSR2        FTx           LL_EXTI_DisableFallingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableFallingTrig_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->FTSR2, ExtiLine);
}

/**
  * @brief  Disable ExtiLine Falling Edge Trigger for Lines in range 64 to 95
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a Falling edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_FTSR register, the
  *       pending bit is not set.
  *       Rising and falling edge triggers can be set for the same interrupt line.
  *       In this case, both generate a trigger condition.
  * @rmtoll FTSR3        FTx           LL_EXTI_DisableFallingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_DisableFallingTrig_64_95(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->FTSR3, ExtiLine);
}


/**
  * @brief  Check if falling edge trigger is enabled for Lines in range 0 to 31
  * @rmtoll FTSR1        FTx           LL_EXTI_IsEnabledFallingTrig_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledFallingTrig_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->FTSR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if falling edge trigger is enabled for Lines in range 32 to 63
  * @rmtoll FTSR2        FTx           LL_EXTI_IsEnabledFallingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledFallingTrig_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->FTSR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if falling edge trigger is enabled for Lines in range 64 to 95
  * @rmtoll FTSR3        FTx           LL_EXTI_IsEnabledFallingTrig_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsEnabledFallingTrig_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->FTSR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @}
  */

/** @defgroup EXTI_LL_EF_Software_Interrupt_Management Software_Interrupt_Management
  * @{
  */

/**
  * @brief  Generate a software Interrupt Event for Lines in range 0 to 31
  * @note If the interrupt is enabled on this line in the EXTI_C1IMR1, writing a 1 to
  *       this bit when it is at '0' sets the corresponding pending bit in EXTI_PR1
  *       resulting in an interrupt request generation.
  *       This bit is cleared by clearing the corresponding bit in the EXTI_PR1
  *       register (by writing a 1 into the bit)
  * @rmtoll SWIER1       SWIx          LL_EXTI_GenerateSWI_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @note   Please check each device line mapping for EXTI Line availability
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_GenerateSWI_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->SWIER1, ExtiLine);
}

/**
  * @brief  Generate a software Interrupt Event for Lines in range 32 to 63
  * @note If the interrupt is enabled on this line in the EXTI_IMR2, writing a 1 to
  *       this bit when it is at '0' sets the corresponding pending bit in EXTI_PR2
  *       resulting in an interrupt request generation.
  *       This bit is cleared by clearing the corresponding bit in the EXTI_PR2
  *       register (by writing a 1 into the bit)
  * @rmtoll SWIER2       SWIx          LL_EXTI_GenerateSWI_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_GenerateSWI_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->SWIER2, ExtiLine);
}

/**
  * @brief  Generate a software Interrupt Event for Lines in range 64 to 95
  * @note If the interrupt is enabled on this line in the EXTI_IMR2, writing a 1 to
  *       this bit when it is at '0' sets the corresponding pending bit in EXTI_PR2
  *       resulting in an interrupt request generation.
  *       This bit is cleared by clearing the corresponding bit in the EXTI_PR3
  *       register (by writing a 1 into the bit)
  * @rmtoll SWIER3       SWIx          LL_EXTI_GenerateSWI_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_GenerateSWI_64_95(uint32_t ExtiLine)
{
  SET_BIT(EXTI->SWIER3, ExtiLine);
}


/**
  * @}
  */

/** @defgroup EXTI_LL_EF_Flag_Management Flag_Management
  * @{
  */

/**
  * @brief  Check if the ExtLine Flag is set or not for Lines in range 0 to 31
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR1          PIFx           LL_EXTI_IsActiveFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsActiveFlag_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->PR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if the ExtLine Flag is set or not for  Lines in range 32 to 63
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR2          PIFx           LL_EXTI_IsActiveFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsActiveFlag_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->PR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if the ExtLine Flag is set or not for  Lines in range 64 to 95
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR3          PIFx           LL_EXTI_IsActiveFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_EXTI_IsActiveFlag_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->PR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}


/**
  * @brief  Read ExtLine Combination Flag for Lines in range 0 to 31
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR1          PIFx           LL_EXTI_ReadFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_EXTI_ReadFlag_0_31(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->PR1, ExtiLine));
}


/**
  * @brief  Read ExtLine Combination Flag for  Lines in range 32 to 63
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR2          PIFx           LL_EXTI_ReadFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_EXTI_ReadFlag_32_63(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->PR2, ExtiLine));
}


/**
  * @brief  Read ExtLine Combination Flag for  Lines in range 64 to 95
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR3          PIFx           LL_EXTI_ReadFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_EXTI_ReadFlag_64_95(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->PR3, ExtiLine));
}

/**
  * @brief  Clear ExtLine Flags for Lines in range 0 to 31
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR1          PIFx           LL_EXTI_ClearFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_ClearFlag_0_31(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->PR1, ExtiLine);
}

/**
  * @brief  Clear ExtLine Flags for Lines in range 32 to 63
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR2          PIFx           LL_EXTI_ClearFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_ClearFlag_32_63(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->PR2, ExtiLine);
}

/**
  * @brief  Clear ExtLine Flags for Lines in range 64 to 95
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll PR3          PIFx           LL_EXTI_ClearFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_ClearFlag_64_95(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->PR3, ExtiLine);
}

#if defined(DUAL_CORE)

/**
  * @brief  Check if the ExtLine Flag is set or not for Lines in range 0 to 31 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR1          PIFx           LL_C2_EXTI_IsActiveFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_ALL_0_31
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsActiveFlag_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2PR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if the ExtLine Flag is set or not for  Lines in range 32 to 63 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR2          PIFx           LL_C2_EXTI_IsActiveFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_ALL_32_63
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsActiveFlag_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2PR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Check if the ExtLine Flag is set or not for  Lines in range 64 to 95 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR3          PIFx           LL_C2_EXTI_IsActiveFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  *         @arg @ref LL_EXTI_LINE_ALL_64_95
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_IsActiveFlag_64_95(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->C2PR3, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Read ExtLine Combination Flag for Lines in range 0 to 31 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR1          PIFx           LL_C2_EXTI_ReadFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_ReadFlag_0_31(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->C2PR1, ExtiLine));
}

/**
  * @brief  Read ExtLine Combination Flag for  Lines in range 32 to 63 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR2          PIFx           LL_C2_EXTI_ReadFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_ReadFlag_32_63(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->C2PR2, ExtiLine));
}


/**
  * @brief  Read ExtLine Combination Flag for  Lines in range 64 to 95 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR3          PIFx           LL_C2_EXTI_ReadFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval @note This bit is set when the selected edge event arrives on the interrupt
  */
__STATIC_INLINE uint32_t LL_C2_EXTI_ReadFlag_64_95(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->C2PR3, ExtiLine));
}
/**
  * @brief  Clear ExtLine Flags for Lines in range 0 to 31 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR1          PIFx           LL_C2_EXTI_ClearFlag_0_31
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_16
  *         @arg @ref LL_EXTI_LINE_17
  *         @arg @ref LL_EXTI_LINE_18
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_ClearFlag_0_31(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->C2PR1, ExtiLine);
}

/**
  * @brief  Clear ExtLine Flags for Lines in range 32 to 63 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR2          PIFx           LL_C2_EXTI_ClearFlag_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_51
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_ClearFlag_32_63(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->C2PR2, ExtiLine);
}

/**
  * @brief  Clear ExtLine Flags for Lines in range 64 to 95 for cpu2
  * @note This bit is set when the selected edge event arrives on the interrupt
  *       line. This bit is cleared by writing a 1 to the bit.
  * @rmtoll C2PR3          PIFx           LL_C2_EXTI_ClearFlag_64_95
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_82
  *         @arg @ref LL_EXTI_LINE_84
  *         @arg @ref LL_EXTI_LINE_85
  *         @arg @ref LL_EXTI_LINE_86
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_ClearFlag_64_95(uint32_t ExtiLine)
{
  WRITE_REG(EXTI->C2PR3, ExtiLine);
}

#endif /* DUAL_CORE */

/**
  * @brief  Enable ExtiLine D3 Pending Mask for Lines in range 0 to 31
  * @rmtoll D3PMR1         MRx           LL_D3_EXTI_EnablePendMask_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_25
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_EnablePendMask_0_31(uint32_t ExtiLine)
{
  SET_BIT(EXTI->D3PMR1, ExtiLine);
}

/**
  * @brief  Enable ExtiLine D3 Pending Mask for Lines in range 32 to 63
  * @rmtoll D3PMR2         MRx           LL_D3_EXTI_EnablePendMask_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_EnablePendMask_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->D3PMR2, ExtiLine);
}

/**
  * @brief  Disable ExtiLine D3 Pending Mask for Lines in range 0 to 31
  * @rmtoll D3PMR1         MRx           LL_D3_EXTI_DisablePendMask_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_25
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_DisablePendMask_0_31(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->D3PMR1, ExtiLine);
}

/**
  * @brief  Disable ExtiLine D3 Pending Mask for Lines in range 32 to 63
  * @rmtoll D3PMR2         MRx           LL_D3_EXTI_DisablePendMask_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_DisablePendMask_32_63(uint32_t ExtiLine)
{
  CLEAR_BIT(EXTI->D3PMR2, ExtiLine);
}

/**
  * @brief  Indicate if ExtiLine D3 Pending Mask is enabled for Lines in range 0 to 31
  * @rmtoll D3PMR1         MRx           LL_D3_EXTI_IsEnabledPendMask_0_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_25
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_IsEnabledPendMask_0_31(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->D3PMR1, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Indicate if ExtiLine D3 Pending Mask is enabled for Lines in range 32 to 63
  * @rmtoll D3PMR2         MRx           LL_D3_EXTI_IsEnabledPendMask_32_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_41
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_IsEnabledPendMask_32_63(uint32_t ExtiLine)
{
  return ((READ_BIT(EXTI->D3PMR2, ExtiLine) == (ExtiLine)) ? 1U : 0U);
}

/**
  * @brief  Set ExtLine D3 Domain Pend Clear Source selection for Lines in range 0 to 15
  * @rmtoll D3PCR1L          PCSx           LL_D3_EXTI_SetPendClearSel_0_15
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  * @param  ClrSrc This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_SetPendClearSel_0_15(uint32_t ExtiLine, uint32_t ClrSrc)
{
  MODIFY_REG(EXTI->D3PCR1L, ((ExtiLine * ExtiLine) * 3UL), ((ExtiLine * ExtiLine) * ClrSrc));
}

/**
  * @brief  Set ExtLine D3 Domain Pend Clear Source selection for Lines in range 16 to 31
  * @rmtoll D3PCR1H          PCSx           LL_D3_EXTI_SetPendClearSel_16_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_25
  * @param  ClrSrc This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_SetPendClearSel_16_31(uint32_t ExtiLine, uint32_t ClrSrc)
{
  MODIFY_REG(EXTI->D3PCR1H, (((ExtiLine >> EXTI_IMR1_IM16_Pos) * (ExtiLine >> EXTI_IMR1_IM16_Pos)) * 3UL), (((ExtiLine >> EXTI_IMR1_IM16_Pos) * (ExtiLine >> EXTI_IMR1_IM16_Pos)) * ClrSrc));
}


/**
  * @brief  Set ExtLine D3 Domain Pend Clear Source selection for Lines in range 32 to 47
  * @rmtoll D3PCR2L          PCSx           LL_D3_EXTI_SetPendClearSel_32_47
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_41
  * @param  ClrSrc This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_SetPendClearSel_32_47(uint32_t ExtiLine, uint32_t ClrSrc)
{
  MODIFY_REG(EXTI->D3PCR2L, ((ExtiLine * ExtiLine) * 3UL), ((ExtiLine * ExtiLine) * ClrSrc));
}

/**
  * @brief  Set ExtLine D3 Domain Pend Clear Source selection for Lines in range 48 to 63
  * @rmtoll D3PCR2H          PCSx           LL_D3_EXTI_SetPendClearSel_48_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  * @param  ClrSrc This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  * @retval None
  */
__STATIC_INLINE void LL_D3_EXTI_SetPendClearSel_48_63(uint32_t ExtiLine, uint32_t ClrSrc)
{
  MODIFY_REG(EXTI->D3PCR2H, (((ExtiLine >> EXTI_IMR2_IM48_Pos) * (ExtiLine >> EXTI_IMR2_IM48_Pos)) * 3UL), (((ExtiLine >> EXTI_IMR2_IM48_Pos) * (ExtiLine >> EXTI_IMR2_IM48_Pos)) * ClrSrc));
}

/**
  * @brief  Get ExtLine D3 Domain Pend Clear Source selection for Lines in range 0 to 15
  * @rmtoll D3PCR1L          PCSx           LL_D3_EXTI_GetPendClearSel_0_15
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_0
  *         @arg @ref LL_EXTI_LINE_1
  *         @arg @ref LL_EXTI_LINE_2
  *         @arg @ref LL_EXTI_LINE_3
  *         @arg @ref LL_EXTI_LINE_4
  *         @arg @ref LL_EXTI_LINE_5
  *         @arg @ref LL_EXTI_LINE_6
  *         @arg @ref LL_EXTI_LINE_7
  *         @arg @ref LL_EXTI_LINE_8
  *         @arg @ref LL_EXTI_LINE_9
  *         @arg @ref LL_EXTI_LINE_10
  *         @arg @ref LL_EXTI_LINE_11
  *         @arg @ref LL_EXTI_LINE_12
  *         @arg @ref LL_EXTI_LINE_13
  *         @arg @ref LL_EXTI_LINE_14
  *         @arg @ref LL_EXTI_LINE_15
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_GetPendClearSel_0_15(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->D3PCR1L, ((ExtiLine * ExtiLine) * 3UL)) / (ExtiLine * ExtiLine));
}

/**
  * @brief  Get ExtLine D3 Domain Pend Clear Source selection for Lines in range 16 to 31
  * @rmtoll D3PCR1H          PCSx           LL_D3_EXTI_GetPendClearSel_16_31
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_19
  *         @arg @ref LL_EXTI_LINE_20
  *         @arg @ref LL_EXTI_LINE_21
  *         @arg @ref LL_EXTI_LINE_25
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_GetPendClearSel_16_31(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->D3PCR1H, (((ExtiLine >> EXTI_IMR1_IM16_Pos) * (ExtiLine >> EXTI_IMR1_IM16_Pos)) * 3UL)) / ((ExtiLine >> EXTI_IMR1_IM16_Pos) * (ExtiLine >> EXTI_IMR1_IM16_Pos)));
}

/**
  * @brief  Get ExtLine D3 Domain Pend Clear Source selection for Lines in range 32 to 47
  * @rmtoll D3PCR2L          PCSx           LL_D3_EXTI_GetPendClearSel_32_47
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_34
  *         @arg @ref LL_EXTI_LINE_35
  *         @arg @ref LL_EXTI_LINE_41
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_GetPendClearSel_32_47(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->D3PCR2L, ((ExtiLine * ExtiLine) * 3UL)) / (ExtiLine * ExtiLine));
}

/**
  * @brief  Get ExtLine D3 Domain Pend Clear Source selection for Lines in range 48 to 63
  * @rmtoll D3PCR2H          PCSx           LL_D3_EXTI_GetPendClearSel_48_63
  * @param  ExtiLine This parameter can be one of the following values:
  *         @arg @ref LL_EXTI_LINE_48
  *         @arg @ref LL_EXTI_LINE_49
  *         @arg @ref LL_EXTI_LINE_50
  *         @arg @ref LL_EXTI_LINE_51
  *         @arg @ref LL_EXTI_LINE_52
  *         @arg @ref LL_EXTI_LINE_53
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH6
  *         @arg @ref LL_EXTI_D3_PEND_CLR_DMACH7
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM4
  *         @arg @ref LL_EXTI_D3_PEND_CLR_LPTIM5
  */
__STATIC_INLINE uint32_t LL_D3_EXTI_GetPendClearSel_48_63(uint32_t ExtiLine)
{
  return (uint32_t)(READ_BIT(EXTI->D3PCR2H, (((ExtiLine >> EXTI_IMR2_IM48_Pos) * (ExtiLine >> EXTI_IMR2_IM48_Pos)) * 3UL)) / ((ExtiLine >> EXTI_IMR2_IM48_Pos) * (ExtiLine >> EXTI_IMR2_IM48_Pos)));
}



/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup EXTI_LL_EF_Init Initialization and de-initialization functions
  * @{,
  */

ErrorStatus LL_EXTI_Init(LL_EXTI_InitTypeDef *EXTI_InitStruct);
ErrorStatus LL_EXTI_DeInit(void);
void LL_EXTI_StructInit(LL_EXTI_InitTypeDef *EXTI_InitStruct);


/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* EXTI */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_LL_EXTI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
