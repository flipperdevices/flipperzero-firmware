/**
  ******************************************************************************
  * @file    stm32h7xx_hal_exti.h
  * @author  MCD Application Team
  * @brief   Header file of EXTI HAL module.
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
#ifndef STM32H7xx_HAL_EXTI_H
#define STM32H7xx_HAL_EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup EXTI EXTI
  * @brief EXTI HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup EXTI_Exported_Types EXTI Exported Types
  * @{
  */
typedef enum
{
  HAL_EXTI_COMMON_CB_ID          = 0x00U,
} EXTI_CallbackIDTypeDef;


/**
  * @brief  EXTI Handle structure definition
  */
typedef struct
{
  uint32_t Line;                    /*!<  Exti line number */
  void (* PendingCallback)(void);   /*!<  Exti pending callback */
} EXTI_HandleTypeDef;

/**
  * @brief  EXTI Configuration structure definition
  */
typedef struct
{
  uint32_t Line;           /*!< The Exti line to be configured. This parameter
                                can be a value of @ref EXTI_Line */
  uint32_t Mode;           /*!< The Exit Mode to be configured for a core.
                                This parameter can be a combination of @ref EXTI_Mode */
  uint32_t Trigger;        /*!< The Exti Trigger to be configured. This parameter
                                can be a value of @ref EXTI_Trigger */
  uint32_t GPIOSel;        /*!< The Exti GPIO multiplexer selection to be configured.
                                This parameter is only possible for line 0 to 15. It
                                can be a value of @ref EXTI_GPIOSel */

  uint32_t PendClearSource; /*!< Specifies the event pending clear source for D3/SRD
                                 domain. This parameter can be a value of @ref
                                 EXTI_PendClear_Source */

} EXTI_ConfigTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup EXTI_Exported_Constants EXTI Exported Constants
  * @{
  */

/** @defgroup EXTI_Line  EXTI Line
  * @{
  */
#define EXTI_LINE_0                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x00U)
#define EXTI_LINE_1                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x01U)
#define EXTI_LINE_2                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x02U)
#define EXTI_LINE_3                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x03U)
#define EXTI_LINE_4                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x04U)
#define EXTI_LINE_5                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x05U)
#define EXTI_LINE_6                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x06U)
#define EXTI_LINE_7                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x07U)
#define EXTI_LINE_8                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x08U)
#define EXTI_LINE_9                         (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x09U)
#define EXTI_LINE_10                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0AU)
#define EXTI_LINE_11                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0BU)
#define EXTI_LINE_12                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0CU)
#define EXTI_LINE_13                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0DU)
#define EXTI_LINE_14                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0EU)
#define EXTI_LINE_15                        (EXTI_GPIO     | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x0FU)
#define EXTI_LINE_16                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x10U)
#define EXTI_LINE_17                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x11U)
#define EXTI_LINE_18                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x12U)
#define EXTI_LINE_19                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x13U)
#define EXTI_LINE_20                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x14U)
#define EXTI_LINE_21                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x15U)
#define EXTI_LINE_22                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x16U)
#define EXTI_LINE_23                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x17U)
#define EXTI_LINE_24                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x18U)
#define EXTI_LINE_25                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL     | 0x19U)
#define EXTI_LINE_26                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1AU)
#define EXTI_LINE_27                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1BU)
#define EXTI_LINE_28                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1CU)
#define EXTI_LINE_29                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1DU)
#define EXTI_LINE_30                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1EU)
#define EXTI_LINE_31                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG1 | EXTI_TARGET_MSK_ALL_CPU | 0x1FU)
#define EXTI_LINE_32                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x00U)
#define EXTI_LINE_33                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x01U)
#define EXTI_LINE_34                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x02U)
#define EXTI_LINE_35                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x03U)
#define EXTI_LINE_36                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x04U)
#define EXTI_LINE_37                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x05U)
#define EXTI_LINE_38                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x06U)
#define EXTI_LINE_39                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x07U)
#define EXTI_LINE_40                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x08U)
#define EXTI_LINE_41                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x09U)
#define EXTI_LINE_42                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x0AU)
#define EXTI_LINE_43                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x0BU)
#if !defined(USB2_OTG_FS)
#define EXTI_LINE_44                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_NONE    | 0x0CU)
#else
#define EXTI_LINE_44                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x0CU)
#endif /* USB2_OTG_FS */
#define EXTI_LINE_45                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_NONE    | 0x0DU)
#if defined(DSI)
#define EXTI_LINE_46                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x0EU)
#else
#define EXTI_LINE_46                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_NONE    | 0x0EU)
#endif /* DSI */
#define EXTI_LINE_47                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x0FU)
#define EXTI_LINE_48                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x10U)
#define EXTI_LINE_49                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x11U)
#define EXTI_LINE_50                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x12U)
#define EXTI_LINE_51                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x13U)
#if !defined(UART9)
#define EXTI_LINE_52                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x14U)
#else
#define EXTI_LINE_52                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x14U)
#endif /* UART9 */
#if !defined(USART10)
#define EXTI_LINE_53                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL     | 0x15U)
#else
#define EXTI_LINE_53                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x15U)
#endif /* USART10 */
#define EXTI_LINE_54                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x16U)
#define EXTI_LINE_55                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x17U)
#define EXTI_LINE_56                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x18U)
#define EXTI_LINE_57                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x19U)
#define EXTI_LINE_58                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1AU)
#define EXTI_LINE_59                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1BU)
#define EXTI_LINE_60                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1CU)
#define EXTI_LINE_61                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1DU)
#define EXTI_LINE_62                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1EU)
#define EXTI_LINE_63                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG2 | EXTI_TARGET_MSK_ALL_CPU | 0x1FU)
#define EXTI_LINE_64                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x00U)
#define EXTI_LINE_65                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x01U)
#define EXTI_LINE_66                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x02U)
#define EXTI_LINE_67                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x03U)
#define EXTI_LINE_68                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x04U)
#define EXTI_LINE_69                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x05U)
#define EXTI_LINE_70                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x06U)
#define EXTI_LINE_71                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x07U)
#define EXTI_LINE_72                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x08U)
#define EXTI_LINE_73                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x09U)
#define EXTI_LINE_74                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x0AU)
#if defined(ADC3)
#define EXTI_LINE_75                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x0BU)
#else
#define EXTI_LINE_75                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 |  EXTI_TARGET_MSK_NONE   | 0x0BU)
#endif /* ADC3 */
#if defined(SAI4)
#define EXTI_LINE_76                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x0CU)
#else
#define EXTI_LINE_76                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 |  EXTI_TARGET_MSK_NONE   | 0x0CU)
#endif /* SAI4 */
#if defined (DUAL_CORE)
#define EXTI_LINE_77                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU1| 0x0DU)
#define EXTI_LINE_78                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU2| 0x0EU)
#define EXTI_LINE_79                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU1| 0x0FU)
#define EXTI_LINE_80                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU2| 0x10U)
#else
#define EXTI_LINE_77                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x0DU)
#define EXTI_LINE_78                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x0EU)
#define EXTI_LINE_79                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x0FU)
#define EXTI_LINE_80                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x10U)
#endif /* DUAL_CORE */

#define EXTI_LINE_81                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x11U)

#if defined (DUAL_CORE)
#define EXTI_LINE_82                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU2| 0x12U)
#else
#define EXTI_LINE_82                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x12U)
#endif /* DUAL_CORE */

#define EXTI_LINE_83                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x13U)
#if defined (DUAL_CORE)
#define EXTI_LINE_84                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_CPU1| 0x14U)
#else
#define EXTI_LINE_84                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x14U)
#endif /* DUAL_CORE */
#define EXTI_LINE_85                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x15U)
#if defined(ETH)
#define EXTI_LINE_86                        (EXTI_CONFIG   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x16U)
#else
#define EXTI_LINE_86                        (EXTI_RESERVED | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_NONE| 0x16U)
#endif /* ETH */
#define EXTI_LINE_87                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL_CPU | 0x17U)
#if defined(DTS)
#define EXTI_LINE_88                        (EXTI_DIRECT   | EXTI_EVENT | EXTI_REG3 | EXTI_TARGET_MSK_ALL | 0x18U)
#endif /* DTS */

/**
  * @}
  */

/** @defgroup EXTI_Mode  EXTI Mode
  * @{
  */
#define EXTI_MODE_NONE                      0x00000000U
#define EXTI_MODE_INTERRUPT                 0x00000001U
#define EXTI_MODE_EVENT                     0x00000002U
#if defined(DUAL_CORE)
#define EXTI_MODE_CORE1_INTERRUPT           EXTI_MODE_INTERRUPT
#define EXTI_MODE_CORE1_EVENT               EXTI_MODE_EVENT
#define EXTI_MODE_CORE2_INTERRUPT           0x00000010U
#define EXTI_MODE_CORE2_EVENT               0x00000020U
#endif /* DUAL_CORE */
/**
  * @}
  */

/** @defgroup EXTI_Trigger  EXTI Trigger
  * @{
  */
#define EXTI_TRIGGER_NONE                   0x00000000U
#define EXTI_TRIGGER_RISING                 0x00000001U
#define EXTI_TRIGGER_FALLING                0x00000002U
#define EXTI_TRIGGER_RISING_FALLING         (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)
/**
  * @}
  */

/** @defgroup EXTI_GPIOSel  EXTI GPIOSel
  * @brief
  * @{
  */
#define EXTI_GPIOA                          0x00000000U
#define EXTI_GPIOB                          0x00000001U
#define EXTI_GPIOC                          0x00000002U
#define EXTI_GPIOD                          0x00000003U
#define EXTI_GPIOE                          0x00000004U
#define EXTI_GPIOF                          0x00000005U
#define EXTI_GPIOG                          0x00000006U
#define EXTI_GPIOH                          0x00000007U
#define EXTI_GPIOI                          0x00000008U
#define EXTI_GPIOJ                          0x00000009U
#define EXTI_GPIOK                          0x0000000AU

/**
  * @}
  */

/** @defgroup EXTI_PendClear_Source  EXTI PendClear Source
  * @brief
  * @{
  */
#define EXTI_D3_PENDCLR_SRC_NONE       0x00000000U /*!< No D3 domain pendclear source , PMRx register to be set to zero  */
#define EXTI_D3_PENDCLR_SRC_DMACH6     0x00000001U /*!< DMA ch6 event selected as D3 domain pendclear source, PMRx register to be set to 1 */
#define EXTI_D3_PENDCLR_SRC_DMACH7     0x00000002U /*!< DMA ch7 event selected as D3 domain pendclear source, PMRx register to be set to 1*/
#if defined (LPTIM4)
#define EXTI_D3_PENDCLR_SRC_LPTIM4     0x00000003U /*!< LPTIM4 out selected as D3 domain pendclear source, PMRx register to be set to 1    */
#else
#define EXTI_D3_PENDCLR_SRC_LPTIM2     0x00000003U /*!< LPTIM2 out selected as D3 domain pendclear source, PMRx register to be set to 1    */
#endif
#if defined (LPTIM5)
#define EXTI_D3_PENDCLR_SRC_LPTIM5     0x00000004U /*!< LPTIM5 out selected as D3 domain pendclear source, PMRx register to be set to 1    */
#else
#define EXTI_D3_PENDCLR_SRC_LPTIM3     0x00000004U /*!< LPTIM3 out selected as D3 domain pendclear source, PMRx register to be set to 1    */
#endif
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup EXTI_Exported_Macros EXTI Exported Macros
  * @{
  */

/**
  * @}
  */

/* Private constants --------------------------------------------------------*/
/** @defgroup EXTI_Private_Constants EXTI Private Constants
  * @{
  */
/**
  * @brief  EXTI Line property definition
  */
#define EXTI_PROPERTY_SHIFT                 24U
#define EXTI_DIRECT                         (0x01UL << EXTI_PROPERTY_SHIFT)
#define EXTI_CONFIG                         (0x02UL << EXTI_PROPERTY_SHIFT)
#define EXTI_GPIO                           ((0x04UL << EXTI_PROPERTY_SHIFT) | EXTI_CONFIG)
#define EXTI_RESERVED                       (0x08UL << EXTI_PROPERTY_SHIFT)
#define EXTI_PROPERTY_MASK                  (EXTI_DIRECT | EXTI_CONFIG | EXTI_GPIO)

/**
  * @brief  EXTI Event presence definition
  */
#define EXTI_EVENT_PRESENCE_SHIFT           28U
#define EXTI_EVENT                          (0x01UL << EXTI_EVENT_PRESENCE_SHIFT)
#define EXTI_EVENT_PRESENCE_MASK            (EXTI_EVENT)

/**
  * @brief  EXTI Register and bit usage
  */
#define EXTI_REG_SHIFT                      16U
#define EXTI_REG1                           (0x00UL << EXTI_REG_SHIFT)
#define EXTI_REG2                           (0x01UL << EXTI_REG_SHIFT)
#define EXTI_REG3                           (0x02UL << EXTI_REG_SHIFT)
#define EXTI_REG_MASK                       (EXTI_REG1 | EXTI_REG2 | EXTI_REG3)
#define EXTI_PIN_MASK                       0x0000001FUL

/**
  * @brief  EXTI Target and bit usage
  */
#define EXTI_TARGET_SHIFT                   20U
#define EXTI_TARGET_MSK_NONE                (0x00UL << EXTI_TARGET_SHIFT)
#define EXTI_TARGET_MSK_D3SRD               (0x01UL << EXTI_TARGET_SHIFT)
#define EXTI_TARGET_MSK_CPU1                (0x02UL << EXTI_TARGET_SHIFT)
#if defined (DUAL_CORE)
#define EXTI_TARGET_MSK_CPU2                (0x04UL << EXTI_TARGET_SHIFT)
#define EXTI_TARGET_MASK                    (EXTI_TARGET_MSK_D3SRD | EXTI_TARGET_MSK_CPU1 | EXTI_TARGET_MSK_CPU2)
#define EXTI_TARGET_MSK_ALL_CPU             (EXTI_TARGET_MSK_CPU1 | EXTI_TARGET_MSK_CPU2)
#else
#define EXTI_TARGET_MASK                    (EXTI_TARGET_MSK_D3SRD | EXTI_TARGET_MSK_CPU1)
#define EXTI_TARGET_MSK_ALL_CPU              EXTI_TARGET_MSK_CPU1
#endif /* DUAL_CORE */
#define EXTI_TARGET_MSK_ALL                  EXTI_TARGET_MASK

/**
  * @brief  EXTI Mask for interrupt & event mode
  */
#if defined (DUAL_CORE)
#define EXTI_MODE_MASK                      (EXTI_MODE_CORE1_EVENT | EXTI_MODE_CORE1_INTERRUPT | EXTI_MODE_CORE2_INTERRUPT | EXTI_MODE_CORE2_EVENT)
#else
#define EXTI_MODE_MASK                      (EXTI_MODE_EVENT | EXTI_MODE_INTERRUPT)
#endif /* DUAL_CORE */

/**
  * @brief  EXTI Mask for trigger possibilities
  */
#define EXTI_TRIGGER_MASK                   (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)

/**
  * @brief  EXTI Line number
  */
#if defined (EXTI_LINE_88)
#define EXTI_LINE_NB                        89UL
#else
#define EXTI_LINE_NB                        88UL
#endif /* EXTI_LINE_88 */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup EXTI_Private_Macros EXTI Private Macros
  * @{
  */
#define IS_EXTI_PROPERTY(__LINE__)      ((((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_DIRECT) || \
                                         (((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_CONFIG)          || \
                                         (((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_GPIO))
#if defined (DUAL_CORE)
#define IS_EXTI_TARGET(__LINE__)        ((((__LINE__) & EXTI_TARGET_MASK)   == EXTI_TARGET_MSK_CPU1)      || \
                                         (((__LINE__) & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_CPU2)    || \
                                         (((__LINE__) & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL_CPU) || \
                                         (((__LINE__) & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL))
#else
#define IS_EXTI_TARGET(__LINE__)        ((((__LINE__) & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_CPU1)   || \
                                         (((__LINE__) & EXTI_TARGET_MASK) == EXTI_TARGET_MSK_ALL))
#endif

#define IS_EXTI_LINE(__LINE__)          ((((__LINE__) & ~(EXTI_PROPERTY_MASK | EXTI_EVENT_PRESENCE_MASK | EXTI_REG_MASK | EXTI_PIN_MASK | EXTI_TARGET_MASK)) == 0x00UL) && \
                                         IS_EXTI_PROPERTY(__LINE__) && IS_EXTI_TARGET(__LINE__) && \
                                         (((__LINE__) & (EXTI_REG_MASK | EXTI_PIN_MASK))      < \
                                         (((EXTI_LINE_NB / 32UL) << EXTI_REG_SHIFT) | (EXTI_LINE_NB % 32UL))))

#define IS_EXTI_MODE(__MODE__)          (((__MODE__) & ~EXTI_MODE_MASK) == 0x00UL)

#define IS_EXTI_TRIGGER(__LINE__)       (((__LINE__) & ~EXTI_TRIGGER_MASK) == 0x00UL)

#define IS_EXTI_PENDING_EDGE(__LINE__)  (((__LINE__) == EXTI_TRIGGER_RISING) || \
                                         ((__LINE__) == EXTI_TRIGGER_FALLING)|| \
                                         ((__LINE__) == EXTI_TRIGGER_RISING_FALLING))

#define IS_EXTI_CONFIG_LINE(__LINE__)   (((__LINE__) & EXTI_CONFIG) != 0x00UL)

#define IS_EXTI_GPIO_PORT(__PORT__)     (((__PORT__) == EXTI_GPIOA) || \
                                         ((__PORT__) == EXTI_GPIOB) || \
                                         ((__PORT__) == EXTI_GPIOC) || \
                                         ((__PORT__) == EXTI_GPIOD) || \
                                         ((__PORT__) == EXTI_GPIOE) || \
                                         ((__PORT__) == EXTI_GPIOF) || \
                                         ((__PORT__) == EXTI_GPIOG) || \
                                         ((__PORT__) == EXTI_GPIOH) || \
                                         ((__PORT__) == EXTI_GPIOI) || \
                                         ((__PORT__) == EXTI_GPIOJ) || \
                                         ((__PORT__) == EXTI_GPIOK))

#define IS_EXTI_GPIO_PIN(__PIN__)       ((__PIN__) < 16UL)
#if defined (LPTIM4) && defined (LPTIM5)
#define IS_EXTI_D3_PENDCLR_SRC(__SRC__) (((__SRC__) == EXTI_D3_PENDCLR_SRC_NONE) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_DMACH6) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_DMACH7) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_LPTIM4) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_LPTIM5))
#else
#define IS_EXTI_D3_PENDCLR_SRC(__SRC__) (((__SRC__) == EXTI_D3_PENDCLR_SRC_NONE) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_DMACH6) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_DMACH7) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_LPTIM2) || \
                                         ((__SRC__) == EXTI_D3_PENDCLR_SRC_LPTIM3))
#endif /* LPTIM4 && LPTIM5 */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup EXTI_Exported_Functions EXTI Exported Functions
  * @brief    EXTI Exported Functions
  * @{
  */

/** @defgroup EXTI_Exported_Functions_Group1 Configuration functions
  * @brief    Configuration functions
  * @{
  */
/* Configuration functions ****************************************************/
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);
/**
  * @}
  */

/** @defgroup EXTI_Exported_Functions_Group2 IO operation functions
  * @brief    IO operation functions
  * @{
  */
/* IO operation functions *****************************************************/
void              HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t          HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void              HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void              HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);

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

#endif /* STM32H7xx_HAL_EXTI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
