/**
  ******************************************************************************
  * @file    stm32h7xx_hal_comp.h
  * @author  MCD Application Team
  * @brief   Header file of COMP HAL module.
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
#ifndef STM32H7xx_HAL_COMP_H
#define STM32H7xx_HAL_COMP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup COMP
  * @{
  */

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup COMP_Exported_Types COMP Exported Types
  * @{
  */

/** 
  * @brief  COMP Init structure definition  
  */
typedef struct
{

  uint32_t WindowMode;         /*!< Set window mode of a pair of comparators instances
                                    (2 consecutive instances odd and even COMP<x> and COMP<x+1>).
                                    Note: HAL COMP driver allows to set window mode from any COMP instance of the pair of COMP instances composing window mode.
                                    This parameter can be a value of @ref COMP_WindowMode */

  uint32_t Mode;               /*!< Set comparator operating mode to adjust power and speed.
                                    Note: For the characteritics of comparator power modes
                                          (propagation delay and power consumption), refer to device datasheet.
                                    This parameter can be a value of @ref COMP_PowerMode */

  uint32_t NonInvertingInput;  /*!< Set comparator input plus (non-inverting input).
                                    This parameter can be a value of @ref COMP_InputPlus */

  uint32_t InvertingInput;     /*!< Set comparator input minus (inverting input).
                                    This parameter can be a value of @ref COMP_InputMinus */

  uint32_t Hysteresis;         /*!< Set comparator hysteresis mode of the input minus.
                                    This parameter can be a value of @ref COMP_Hysteresis */

  uint32_t OutputPol;          /*!< Set comparator output polarity.
                                    This parameter can be a value of @ref COMP_OutputPolarity */

  uint32_t BlankingSrce;       /*!< Set comparator blanking source.
                                    This parameter can be a value of @ref COMP_BlankingSrce */

  uint32_t TriggerMode;        /*!< Set the comparator output triggering External Interrupt Line (EXTI).
                                    This parameter can be a value of @ref COMP_EXTI_TriggerMode */

}COMP_InitTypeDef;

/**
  * @brief  HAL COMP state machine: HAL COMP states definition
  */
#define COMP_STATE_BITFIELD_LOCK  ((uint32_t)0x10)
typedef enum
{
  HAL_COMP_STATE_RESET             = 0x00,                                              /*!< COMP not yet initialized                             */
  HAL_COMP_STATE_RESET_LOCKED      = (HAL_COMP_STATE_RESET | COMP_STATE_BITFIELD_LOCK), /*!< COMP not yet initialized and configuration is locked */
  HAL_COMP_STATE_READY             = 0x01,                                              /*!< COMP initialized and ready for use                   */
  HAL_COMP_STATE_READY_LOCKED      = (HAL_COMP_STATE_READY | COMP_STATE_BITFIELD_LOCK), /*!< COMP initialized but configuration is locked         */
  HAL_COMP_STATE_BUSY              = 0x02,                                              /*!< COMP is running                                      */
  HAL_COMP_STATE_BUSY_LOCKED       = (HAL_COMP_STATE_BUSY | COMP_STATE_BITFIELD_LOCK)   /*!< COMP is running and configuration is locked          */
}HAL_COMP_StateTypeDef;

/** 
  * @brief  COMP Handle Structure definition
  */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
typedef struct __COMP_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
{
  COMP_TypeDef       *Instance;       /*!< Register base address    */
  COMP_InitTypeDef   Init;            /*!< COMP required parameters */
  HAL_LockTypeDef    Lock;            /*!< Locking object           */
  __IO HAL_COMP_StateTypeDef  State;  /*!< COMP communication state */
  __IO uint32_t      ErrorCode;       /*!< COMP error code */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
  void (* TriggerCallback)(struct __COMP_HandleTypeDef *hcomp);   /*!< COMP trigger callback */
  void (* MspInitCallback)(struct __COMP_HandleTypeDef *hcomp);   /*!< COMP Msp Init callback */
  void (* MspDeInitCallback)(struct __COMP_HandleTypeDef *hcomp); /*!< COMP Msp DeInit callback */
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */

} COMP_HandleTypeDef;

#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL COMP Callback ID enumeration definition
  */
typedef enum
{
  HAL_COMP_TRIGGER_CB_ID                = 0x00U,  /*!< COMP trigger callback ID */
  HAL_COMP_MSPINIT_CB_ID                = 0x01U,  /*!< COMP Msp Init callback ID */
  HAL_COMP_MSPDEINIT_CB_ID              = 0x02U   /*!< COMP Msp DeInit callback ID */
} HAL_COMP_CallbackIDTypeDef;

/**
  * @brief  HAL COMP Callback pointer definition
  */
typedef  void (*pCOMP_CallbackTypeDef)(COMP_HandleTypeDef *hcomp); /*!< pointer to a COMP callback function */

#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup COMP_Exported_Constants COMP Exported Constants
  * @{
  */
  
/** @defgroup COMP_Error_Code COMP Error Code
  * @{
  */
#define HAL_COMP_ERROR_NONE             (0x00U)   /*!< No error */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
#define HAL_COMP_ERROR_INVALID_CALLBACK (0x01U)   /*!< Invalid Callback error */
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
/**
  * @}
  */
  
/** @defgroup COMP_WindowMode COMP Window Mode
  * @{
  */
#define COMP_WINDOWMODE_DISABLE                 ((uint32_t)0x00000000)   /*!< Window mode disable: Comparators instances pair COMP1 and COMP2 are independent */
#define COMP_WINDOWMODE_COMP1_INPUT_PLUS_COMMON (COMP_CFGRx_WINMODE)     /*!< Window mode enable: Comparators instances pair COMP1 and COMP2 have their input plus connected together. The common input is COMP1 input plus (COMP2 input plus is no more accessible). */

/**
  * @}
  */

/** @defgroup COMP_PowerMode COMP power mode
  * @{
  */
/* Note: For the characteritics of comparator power modes                     */
/*       (propagation delay and power consumption),                           */
/*       refer to device datasheet.                                           */
#define COMP_POWERMODE_HIGHSPEED       ((uint32_t)0x00000000)   /*!< High Speed */
#define COMP_POWERMODE_MEDIUMSPEED     (COMP_CFGRx_PWRMODE_0)   /*!< Medium Speed */
#define COMP_POWERMODE_ULTRALOWPOWER   (COMP_CFGRx_PWRMODE)     /*!< Ultra-low power mode */
/**
  * @}
  */

/** @defgroup COMP_InputPlus COMP input plus (non-inverting input)
  * @{
  */
#define COMP_INPUT_PLUS_IO1            ((uint32_t)0x00000000) /*!< Comparator input plus connected to IO1 (pin PB0 for COMP1, pin PE9 for COMP2) */
#define COMP_INPUT_PLUS_IO2            (COMP_CFGRx_INPSEL)    /*!< Comparator input plus connected to IO2 (pin PB2 for COMP1, pin PE11 for COMP2) */
#if defined (COMP_CFGRx_INP2SEL)
#define COMP_INPUT_PLUS_DAC2_CH1       (COMP_CFGRx_INP2SEL)   /*!< Comparator input plus 2 connected to (DAC2_CH1 for COMP1) */
#endif
/**
  * @}
  */

/** @defgroup COMP_InputMinus COMP input minus (inverting input)
  * @{
  */
#define COMP_INPUT_MINUS_1_4VREFINT     (                                                                                        COMP_CFGRx_SCALEN | COMP_CFGRx_BRGEN)    /*!< Comparator input minus connected to 1/4 VrefInt */
#define COMP_INPUT_MINUS_1_2VREFINT     (                                                                  COMP_CFGRx_INMSEL_0 | COMP_CFGRx_SCALEN | COMP_CFGRx_BRGEN)    /*!< Comparator input minus connected to 1/2 VrefInt */
#define COMP_INPUT_MINUS_3_4VREFINT     (                                            COMP_CFGRx_INMSEL_1                       | COMP_CFGRx_SCALEN | COMP_CFGRx_BRGEN)    /*!< Comparator input minus connected to 3/4 VrefInt */
#define COMP_INPUT_MINUS_VREFINT        (                                            COMP_CFGRx_INMSEL_1 | COMP_CFGRx_INMSEL_0 | COMP_CFGRx_SCALEN                   )    /*!< Comparator input minus connected to VrefInt */
#define COMP_INPUT_MINUS_DAC1_CH1       (                      COMP_CFGRx_INMSEL_2                                                                                   )    /*!< Comparator input minus connected to DAC1 channel 1 (DAC_OUT1) */
#define COMP_INPUT_MINUS_DAC1_CH2       (                      COMP_CFGRx_INMSEL_2                       | COMP_CFGRx_INMSEL_0                                       )    /*!< Comparator input minus connected to DAC1 channel 2 (DAC_OUT2) */
#define COMP_INPUT_MINUS_IO1            (                      COMP_CFGRx_INMSEL_2 | COMP_CFGRx_INMSEL_1                                                             )    /*!< Comparator input minus connected to IO1 (pin PB1 for COMP1, pin PE10 for COMP2) */
#define COMP_INPUT_MINUS_IO2            (                      COMP_CFGRx_INMSEL_2 | COMP_CFGRx_INMSEL_1 | COMP_CFGRx_INMSEL_0                                       )    /*!< Comparator input minus connected to IO2 (pin PC4 for COMP1, pin PE7 for COMP2) */
#if defined (COMP_CFGRx_INMSEL_3)
#define COMP_INPUT_MINUS_TPSENS_DAC2CH1 (COMP_CFGRx_INMSEL_3                                                                                                         )    /*!< Comparator input minus connected to (temp sensor which is exist in ADC for COMP1, DAC2_CH1 for COMP2) */
#define COMP_INPUT_MINUS_VBAT_VDDAP     (COMP_CFGRx_INMSEL_3                                             | COMP_CFGRx_INMSEL_0                                       )    /*!< Comparator input minus connected to (VBAT/4 for COMP1, VDDAP for COMP2) */
#endif
/**
  * @}
  */

/** @defgroup COMP_Hysteresis COMP hysteresis
  * @{
  */
#define COMP_HYSTERESIS_NONE           ((uint32_t)0x00000000)   /*!< No hysteresis */
#define COMP_HYSTERESIS_LOW            (COMP_CFGRx_HYST_0)      /*!< Hysteresis level low */
#define COMP_HYSTERESIS_MEDIUM         (COMP_CFGRx_HYST_1)      /*!< Hysteresis level medium */
#define COMP_HYSTERESIS_HIGH           (COMP_CFGRx_HYST)        /*!< Hysteresis level high */
/**
  * @}
  */

/** @defgroup COMP_OutputPolarity COMP Output Polarity
  * @{
  */
#define COMP_OUTPUTPOL_NONINVERTED     ((uint32_t)0x00000000)  /*!< COMP output level is not inverted (comparator output is high when the input plus is at a higher voltage than the input minus) */
#define COMP_OUTPUTPOL_INVERTED        (COMP_CFGRx_POLARITY)   /*!< COMP output level is inverted     (comparator output is low  when the input plus is at a higher voltage than the input minus) */
/**
  * @}
  */


/** @defgroup COMP_BlankingSrce  COMP Blanking Source
  * @{
  */
/* Any blanking source can be selected for all comparators */
#define COMP_BLANKINGSRC_NONE             ((uint32_t)0x00000000)                           /*!< No blanking source */
#define COMP_BLANKINGSRC_TIM1_OC5         (COMP_CFGRx_BLANKING_0)                          /*!< TIM1 OC5 selected as blanking source for comparator */
#define COMP_BLANKINGSRC_TIM2_OC3         (COMP_CFGRx_BLANKING_1)                          /*!< TIM2 OC3 selected as blanking source for comparator */
#define COMP_BLANKINGSRC_TIM3_OC3         (COMP_CFGRx_BLANKING_0 |COMP_CFGRx_BLANKING_1)   /*!< TIM3 OC3 selected as blanking source for compartor */
#define COMP_BLANKINGSRC_TIM3_OC4         (COMP_CFGRx_BLANKING_2)                          /*!< TIM3 OC4 selected as blanking source for comparator */
#define COMP_BLANKINGSRC_TIM8_OC5         (COMP_CFGRx_BLANKING_2|COMP_CFGRx_BLANKING_0)    /*!< TIM8 OC5 selected as blanking source for comparator */
#define COMP_BLANKINGSRC_TIM15_OC1        (COMP_CFGRx_BLANKING_2|COMP_CFGRx_BLANKING_1)    /*!< TIM15 OC1 selected as blanking source for comparator */
/**
  * @}
  */




/** @defgroup COMP_OutputLevel COMP Output Level
  * @{
  */ 

/* Note: Comparator output level values are fixed to "0" and "1",             */
/* corresponding COMP register bit is managed by HAL function to match        */
/* with these values (independently of bit position in register).             */

/* When output polarity is not inverted, comparator output is low when
   the input plus is at a lower voltage than the input minus */
#define COMP_OUTPUT_LEVEL_LOW              ((uint32_t)0x00000000)
/* When output polarity is not inverted, comparator output is high when
   the input plus is at a higher voltage than the input minus */
#define COMP_OUTPUT_LEVEL_HIGH             ((uint32_t)0x00000001)

/**
  * @}
  */

/** @defgroup COMP_EXTI_TriggerMode COMP output to EXTI
  * @{
  */
#define COMP_TRIGGERMODE_NONE                 ((uint32_t)0x00000000)                                    /*!< Comparator output triggering no External Interrupt Line */
#define COMP_TRIGGERMODE_IT_RISING            (COMP_EXTI_IT | COMP_EXTI_RISING)                         /*!< Comparator output triggering External Interrupt Line event with interruption, on rising edge */
#define COMP_TRIGGERMODE_IT_FALLING           (COMP_EXTI_IT | COMP_EXTI_FALLING)                        /*!< Comparator output triggering External Interrupt Line event with interruption, on falling edge */
#define COMP_TRIGGERMODE_IT_RISING_FALLING    (COMP_EXTI_IT | COMP_EXTI_RISING | COMP_EXTI_FALLING)     /*!< Comparator output triggering External Interrupt Line event with interruption, on both rising and falling edges */
#define COMP_TRIGGERMODE_EVENT_RISING         (COMP_EXTI_EVENT | COMP_EXTI_RISING)                      /*!< Comparator output triggering External Interrupt Line event only (without interruption), on rising edge */
#define COMP_TRIGGERMODE_EVENT_FALLING        (COMP_EXTI_EVENT | COMP_EXTI_FALLING)                     /*!< Comparator output triggering External Interrupt Line event only (without interruption), on falling edge */
#define COMP_TRIGGERMODE_EVENT_RISING_FALLING (COMP_EXTI_EVENT | COMP_EXTI_RISING | COMP_EXTI_FALLING)  /*!< Comparator output triggering External Interrupt Line event only (without interruption), on both rising and falling edges */

/**
  * @}
  */

/** @defgroup COMP_Flag COMP Flag
  * @{
  */
#define COMP_FLAG_C1I           COMP_SR_C1IF           /*!< Comparator 1 Interrupt Flag */
#define COMP_FLAG_C2I           COMP_SR_C2IF           /*!< Comparator 2 Interrupt Flag */
#define COMP_FLAG_LOCK          COMP_CFGRx_LOCK        /*!< Lock flag */
/**
  * @}
  */
/** @defgroup COMP_IT_CLEAR_Flags  COMP Interruption Clear Flags
  * @{
  */
#define COMP_CLEAR_C1IF          COMP_ICFR_C1IF     /*!< Clear Comparator 1 Interrupt Flag */
#define COMP_CLEAR_C2IF          COMP_ICFR_C2IF     /*!< Clear Comparator 2 Interrupt Flag */
/**
  * @}
  */
/** @defgroup COMP_Interrupts_Definitions COMP Interrupts Definitions
  * @{
  */
#define COMP_IT_EN               COMP_CFGRx_ITEN 

/**
  * @}
  */
  

/**
  * @}
  */
 
/* Exported macros -----------------------------------------------------------*/
/** @defgroup COMP_Exported_Macros COMP Exported Macros
  * @{
  */
/** @defgroup COMP_Handle_Management  COMP Handle Management
 * @{
  */

/** @brief  Reset COMP handle state.
  * @param  __HANDLE__  COMP handle
  * @retval None
  */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
#define __HAL_COMP_RESET_HANDLE_STATE(__HANDLE__) do{                                                 \
                                                     (__HANDLE__)->State = HAL_COMP_STATE_RESET;      \
                                                     (__HANDLE__)->MspInitCallback = NULL;            \
                                                     (__HANDLE__)->MspDeInitCallback = NULL;          \
                                                    } while(0)
#else
#define __HAL_COMP_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_COMP_STATE_RESET)
#endif

/**
  * @brief Clear COMP error code (set it to no error code "HAL_COMP_ERROR_NONE").
  * @param __HANDLE__ COMP handle
  * @retval None
  */
#define COMP_CLEAR_ERRORCODE(__HANDLE__) ((__HANDLE__)->ErrorCode = HAL_COMP_ERROR_NONE) 

/**
  * @brief  Enable the specified comparator.
  * @param  __HANDLE__ COMP handle
  * @retval None
  */
#define __HAL_COMP_ENABLE(__HANDLE__)                 SET_BIT((__HANDLE__)->Instance->CFGR, COMP_CFGRx_EN)

/**
  * @brief  Disable the specified comparator.
  * @param  __HANDLE__ COMP handle
  * @retval None
  */
#define __HAL_COMP_DISABLE(__HANDLE__)                CLEAR_BIT((__HANDLE__)->Instance->CFGR, COMP_CFGRx_EN)

/**
  * @brief  Lock the specified comparator configuration.
  * @note   Using this macro induce HAL COMP handle state machine being no
  *         more in line with COMP instance state.
  *         To keep HAL COMP handle state machine updated, it is recommended
  *         to use function "HAL_COMP_Lock')".
  * @param  __HANDLE__ COMP handle
  * @retval None
  */
#define __HAL_COMP_LOCK(__HANDLE__)                   SET_BIT((__HANDLE__)->Instance->CFGR, COMP_CFGRx_LOCK)

/**
  * @brief  Check whether the specified comparator is locked.
  * @param  __HANDLE__  COMP handle
  * @retval Value 0 if COMP instance is not locked, value 1 if COMP instance is locked
  */
#define __HAL_COMP_IS_LOCKED(__HANDLE__)           (READ_BIT((__HANDLE__)->Instance->CFGR, COMP_CFGRx_LOCK) == COMP_CFGRx_LOCK)

/**
  * @}
  */
  
/** @defgroup COMP_Exti_Management  COMP external interrupt line management
  * @{
  */

/**
  * @brief  Enable the COMP1 EXTI line rising edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_RISING_EDGE()    SET_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP1)


/**
  * @brief  Disable the COMP1 EXTI line rising edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_RISING_EDGE()    CLEAR_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 EXTI line falling edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_FALLING_EDGE()    SET_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_FALLING_EDGE()    CLEAR_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP1)


/**
  * @brief  Enable the COMP1 EXTI line rising & falling edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_RISING_FALLING_EDGE()   do {                                           \
                                                               __HAL_COMP_COMP1_EXTI_ENABLE_RISING_EDGE();  \
                                                               __HAL_COMP_COMP1_EXTI_ENABLE_FALLING_EDGE(); \
                                                             } while(0)


/**
  * @brief  Disable the COMP1 EXTI line rising & falling edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_DISABLE_RISING_FALLING_EDGE()  do {                                            \
                                                               __HAL_COMP_COMP1_EXTI_DISABLE_RISING_EDGE();  \
                                                               __HAL_COMP_COMP1_EXTI_DISABLE_FALLING_EDGE(); \
                                                             } while(0)


/**
  * @brief  Enable the COMP1 EXTI line in interrupt mode.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTI_ENABLE_IT()             SET_BIT(EXTI_D1->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_DISABLE_IT()            CLEAR_BIT(EXTI_D1->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_ENABLE_EVENT()           SET_BIT(EXTI_D1->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_DISABLE_EVENT()          CLEAR_BIT(EXTI_D1->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Check whether the COMP1 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP1_EXTI_GET_FLAG()              READ_BIT(EXTI_D1->PR1, COMP_EXTI_LINE_COMP1)
/**
  * @brief  Clear the COMP1 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_CLEAR_FLAG()            WRITE_REG(EXTI_D1->PR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Generate a software interrupt on the COMP1 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTI_GENERATE_SWIT()         SET_BIT(EXTI->SWIER1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 D3 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID3_ENABLE_EVENT()        SET_BIT(EXTI->D3PMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 D3 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID3_DISABLE_EVENT()        CLEAR_BIT(EXTI->D3PMR1, COMP_EXTI_LINE_COMP1)

#if defined(DUAL_CORE)
/**
  * @brief  Enable the COMP1 D2 EXTI line in interrupt mode.
  * @retval None
  */                                         
#define __HAL_COMP_COMP1_EXTID2_ENABLE_IT()           SET_BIT(EXTI_D2->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 D2 EXTI line in interrupt mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID2_DISABLE_IT()          CLEAR_BIT(EXTI_D2->IMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Enable the COMP1 D2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID2_ENABLE_EVENT()         SET_BIT(EXTI_D2->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Disable the COMP1 D2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID2_DISABLE_EVENT()        CLEAR_BIT(EXTI_D2->EMR1, COMP_EXTI_LINE_COMP1)

/**
  * @brief  Check whether the COMP1 D2 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP1_EXTID2_GET_FLAG()              READ_BIT(EXTI_D2->PR1, COMP_EXTI_LINE_COMP1)     

/**
  * @brief  Clear the COMP1 D2 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP1_EXTID2_CLEAR_FLAG()            WRITE_REG(EXTI_D2->PR1, COMP_EXTI_LINE_COMP1)

#endif

/**
  * @brief  Enable the COMP2 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_RISING_EDGE()    SET_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line rising edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_RISING_EDGE()   CLEAR_BIT(EXTI->RTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI line falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line falling edge trigger. 
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_RISING_FALLING_EDGE()   do {                                           \
                                                               __HAL_COMP_COMP2_EXTI_ENABLE_RISING_EDGE();  \
                                                               __HAL_COMP_COMP2_EXTI_ENABLE_FALLING_EDGE(); \
                                                             } while(0)

/**
  * @brief  Disable the COMP2 EXTI line rising & falling edge trigger.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_DISABLE_RISING_FALLING_EDGE()   do {                                           \
                                                               __HAL_COMP_COMP2_EXTI_DISABLE_RISING_EDGE();  \
                                                               __HAL_COMP_COMP2_EXTI_DISABLE_FALLING_EDGE(); \
                                                             } while(0)
/**
  * @brief  Enable the COMP2 EXTI line.
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTI_ENABLE_IT()             SET_BIT(EXTI_D1->IMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_DISABLE_IT()            CLEAR_BIT(EXTI_D1->IMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_ENABLE_EVENT()          SET_BIT(EXTI_D1->EMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_DISABLE_EVENT()          CLEAR_BIT(EXTI_D1->EMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Check whether the COMP2 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP2_EXTI_GET_FLAG()              READ_BIT(EXTI_D1->PR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Clear the the COMP2 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_CLEAR_FLAG()            WRITE_REG(EXTI_D1->PR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Enable the COMP2 D3 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID3_ENABLE_EVENT()        SET_BIT(EXTI->D3PMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Disable the COMP2 D3 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID3_DISABLE_EVENT()       CLEAR_BIT(EXTI->D3PMR1, COMP_EXTI_LINE_COMP2)

/**
  * @brief  Generate a software interrupt on the COMP2 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTI_GENERATE_SWIT()         SET_BIT(EXTI->SWIER1, COMP_EXTI_LINE_COMP2)

#if defined(DUAL_CORE)
/**
  * @brief  Enable the COMP2 D2 EXTI line
  * @retval None
  */                                         
#define __HAL_COMP_COMP2_EXTID2_ENABLE_IT()           SET_BIT(EXTI_D2->IMR1, COMP_EXTI_LINE_COMP2)


/**
  * @brief  Disable the COMP2 D2 EXTI line.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID2_DISABLE_IT()          CLEAR_BIT(EXTI_D2->IMR1, COMP_EXTI_LINE_COMP2)



/**
  * @brief  Enable the COMP2 D2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID2_ENABLE_EVENT()         SET_BIT(EXTI_D2->EMR1, COMP_EXTI_LINE_COMP2)



/**
  * @brief  Disable the COMP2 D2 EXTI Line in event mode.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID2_DISABLE_EVENT()        CLEAR_BIT(EXTI_D2->EMR1, COMP_EXTI_LINE_COMP2)


/**
  * @brief  Check whether the COMP2 D2 EXTI line flag is set or not.
  * @retval RESET or SET
  */
#define __HAL_COMP_COMP2_EXTID2_GET_FLAG()            READ_BIT(EXTI_D2->PR1, COMP_EXTI_LINE_COMP2)     

/**
  * @brief  Clear the the COMP2 D2 EXTI flag.
  * @retval None
  */
#define __HAL_COMP_COMP2_EXTID2_CLEAR_FLAG()          WRITE_REG(EXTI_D2->PR1, COMP_EXTI_LINE_COMP2)

#endif
/** @brief  Checks if the specified COMP interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the COMP Handle.
  *         This parameter can be COMP1 where x: 1 or 2 to select the COMP peripheral.
  * @param  __INTERRUPT__: specifies the COMP interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg COMP_IT_EN: Comparator interrupt enable
  *   
  * @retval The new state of __IT__ (TRUE or FALSE)
  */
#define __HAL_COMP_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CFGR & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Checks whether the specified COMP flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg COMP_FLAG_C1I:  Comparator 1 Interrupt Flag
  *            @arg COMP_FLAG_C2I:  Comparator 2 Interrupt Flag
  *            @retval The new state of __FLAG__ (TRUE or FALSE)
  */
#define __HAL_COMP_GET_FLAG(__FLAG__)     ((COMP12->SR & (__FLAG__)) == (__FLAG__))   

/** @brief  Clears the specified COMP pending flag.
  * @param  __FLAG__: specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg COMP_CLEAR_C1IF  : Clear Comparator 1 Interrupt Flag
  *            @arg COMP_CLEAR_C2IF  : Clear Comparator 2 Interrupt Flag
  * @retval None
  */
#define __HAL_COMP_CLEAR_FLAG(__FLAG__)   (COMP12->ICFR = (__FLAG__))

/** @brief  Clear the COMP C1I  flag.
  * @retval None
  */
#define __HAL_COMP_CLEAR_C1IFLAG()   __HAL_COMP_CLEAR_FLAG( COMP_CLEAR_C1IF)

/** @brief  Clear the COMP C2I  flag.
  * @retval None
  */
#define __HAL_COMP_CLEAR_C2IFLAG()   __HAL_COMP_CLEAR_FLAG( COMP_CLEAR_C2IF)

/** @brief  Enable the specified COMP interrupt.
  * @param  __HANDLE__: specifies the COMP Handle.
  * @param  __INTERRUPT__: specifies the COMP interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg COMP_CFGRx_ITEN :  Comparator  interrupt
  * @retval None
  */
#define __HAL_COMP_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ( ((__HANDLE__)->Instance->CFGR) |= (__INTERRUPT__) )

/** @brief  Disable the specified COMP interrupt.
  * @param  __HANDLE__: specifies the COMP Handle.
  * @param  __INTERRUPT__: specifies the COMP interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg COMP_CFGRx_ITEN :  Comparator  interrupt
  * @retval None
  */
#define __HAL_COMP_DISABLE_IT(__HANDLE__,__INTERRUPT__) (((__HANDLE__)->Instance->CFGR) &= ~(__INTERRUPT__))

/**
  * @}
  */
/** @brief  Enable the specified bit in the Option register.
  * @param  __AF__: specifies the Alternate Function source selection .
  *          This parameter can be one of the following values:
  *            @arg COMP_OR_AFOPA6  :  Alternate Function PA6 source selection
  *            @arg COMP_OR_AFOPA8  :  Alternate Function PA8 source selection
  *            @arg COMP_OR_AFOPB12 :  Alternate Function PB12 source selection
  *            @arg COMP_OR_AFOPE6  :  Alternate Function PE6 source selection
  *            @arg COMP_OR_AFOPE15 :  Alternate Function PE15 source selection
  *            @arg COMP_OR_AFOPG2  :  Alternate Function PG2 source selection
  *            @arg COMP_OR_AFOPG3  :  Alternate Function PG3 source selection
  *            @arg COMP_OR_AFOPG4  :  Alternate Function PG4 source selection
  *            @arg COMP_OR_AFOPI1  :  Alternate Function PI1 source selection
  *            @arg COMP_OR_AFOPI4  :  Alternate Function PI4 source selection 
  *            @arg COMP_OR_AFOPK2  :  Alternate Function PK2 source selection
  * @retval None
  */
#define __HAL_COMP_ENABLE_OR(__AF__) SET_BIT(COMP12->OR, (__AF__))

/** @brief  Disable the specified bit in the Option register.
  * @param  __AF__: specifies the Alternate Function source selection .
  *          This parameter can be one of the following values:
  *            @arg COMP_OR_AFOPA6  :  Alternate Function PA6 source selection
  *            @arg COMP_OR_AFOPA8  :  Alternate Function PA8 source selection
  *            @arg COMP_OR_AFOPB12 :  Alternate Function PB12 source selection
  *            @arg COMP_OR_AFOPE6  :  Alternate Function PE6 source selection
  *            @arg COMP_OR_AFOPE15 :  Alternate Function PE15 source selection
  *            @arg COMP_OR_AFOPG2  :  Alternate Function PG2 source selection
  *            @arg COMP_OR_AFOPG3  :  Alternate Function PG3 source selection
  *            @arg COMP_OR_AFOPG4  :  Alternate Function PG4 source selection
  *            @arg COMP_OR_AFOPI1  :  Alternate Function PI1 source selection
  *            @arg COMP_OR_AFOPI4  :  Alternate Function PI4 source selection
  *            @arg COMP_OR_AFOPK2  :  Alternate Function PK2 source selection  
  * @retval None
  */
#define __HAL_COMP_DISABLE_OR(__AF__) CLEAR_BIT(COMP12->OR, (__AF__))
/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup COMP_Private_Constants COMP Private Constants
  * @{
  */
/** @defgroup COMP_ExtiLine COMP EXTI Lines
  * @{
  */
#define COMP_EXTI_LINE_COMP1           (EXTI_IMR1_IM20)  /*!< EXTI line 20 connected to COMP1 output */
#define COMP_EXTI_LINE_COMP2           (EXTI_IMR1_IM21)  /*!< EXTI line 21 connected to COMP2 output */
/**
  * @}
  */
/** @defgroup COMP_ExtiLine COMP EXTI Lines
  * @{
  */
#define COMP_EXTI_IT                        ((uint32_t) 0x01)  /*!< EXTI line event with interruption */
#define COMP_EXTI_EVENT                     ((uint32_t) 0x02)  /*!< EXTI line event only (without interruption) */
#define COMP_EXTI_RISING                    ((uint32_t) 0x10)  /*!< EXTI line event on rising edge */
#define COMP_EXTI_FALLING                   ((uint32_t) 0x20)  /*!< EXTI line event on falling edge */
/**
  * @}
  */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup COMP_Private_Macros COMP Private Macros
  * @{
  */
/** @defgroup COMP_GET_EXTI_LINE COMP Private macros to get EXTI line associated with Comparators 
  * @{
  */
/**
  * @brief  Get the specified EXTI line for a comparator instance.
  * @param  __INSTANCE__: specifies the COMP instance.
  * @retval value of @ref COMP_ExtiLine
  */
#define COMP_GET_EXTI_LINE(__INSTANCE__)             (((__INSTANCE__) == COMP1) ? COMP_EXTI_LINE_COMP1 : \
                                                      COMP_EXTI_LINE_COMP2)
/**
  * @}
  */
/** @defgroup COMP_IS_COMP_Definitions COMP private macros to check input parameters
  * @{
  */
#define IS_COMP_WINDOWMODE(__WINDOWMODE__)  (((__WINDOWMODE__) == COMP_WINDOWMODE_DISABLE)                || \
                                             ((__WINDOWMODE__) == COMP_WINDOWMODE_COMP1_INPUT_PLUS_COMMON)  )

#define IS_COMP_POWERMODE(__POWERMODE__)    (((__POWERMODE__) == COMP_POWERMODE_HIGHSPEED)    || \
                                             ((__POWERMODE__) == COMP_POWERMODE_MEDIUMSPEED)  || \
                                             ((__POWERMODE__) == COMP_POWERMODE_ULTRALOWPOWER)  )

#if defined (COMP_CFGRx_INP2SEL)
#define IS_COMP_INPUT_PLUS(__COMP_INSTANCE__, __INPUT_PLUS__) (((__INPUT_PLUS__) == COMP_INPUT_PLUS_IO1) || \
                                                               ((__INPUT_PLUS__) == COMP_INPUT_PLUS_IO2) || \
                                                               ((__INPUT_PLUS__) == COMP_INPUT_PLUS_DAC2_CH1))
#else
#define IS_COMP_INPUT_PLUS(__COMP_INSTANCE__, __INPUT_PLUS__) (((__INPUT_PLUS__) == COMP_INPUT_PLUS_IO1) || \
                                                               ((__INPUT_PLUS__) == COMP_INPUT_PLUS_IO2))                                                              
#endif                                                             


#if defined (COMP_CFGRx_INMSEL_3)
#define IS_COMP_INPUT_MINUS(__COMP_INSTANCE__, __INPUT_MINUS__) (((__INPUT_MINUS__) == COMP_INPUT_MINUS_1_4VREFINT)     || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_1_2VREFINT)     || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_3_4VREFINT)     || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_VREFINT)        || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_DAC1_CH1)       || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_DAC1_CH2)       || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_IO1)            || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_IO2)            || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_TPSENS_DAC2CH1) || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_VBAT_VDDAP))
#else                                          
#define IS_COMP_INPUT_MINUS(__COMP_INSTANCE__, __INPUT_MINUS__) (((__INPUT_MINUS__) == COMP_INPUT_MINUS_1_4VREFINT)  || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_1_2VREFINT)  || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_3_4VREFINT)  || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_VREFINT)     || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_DAC1_CH1)    || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_DAC1_CH2)    || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_IO1)         || \
                                                                 ((__INPUT_MINUS__) == COMP_INPUT_MINUS_IO2))                                                                  
#endif

#define IS_COMP_HYSTERESIS(__HYSTERESIS__)  (((__HYSTERESIS__) == COMP_HYSTERESIS_NONE)   || \
                                             ((__HYSTERESIS__) == COMP_HYSTERESIS_LOW)    || \
                                             ((__HYSTERESIS__) == COMP_HYSTERESIS_MEDIUM) || \
                                             ((__HYSTERESIS__) == COMP_HYSTERESIS_HIGH))

#define IS_COMP_OUTPUTPOL(__POL__)          (((__POL__) == COMP_OUTPUTPOL_NONINVERTED) || \
                                             ((__POL__) == COMP_OUTPUTPOL_INVERTED))

#define IS_COMP_BLANKINGSRCE(__SOURCE__)    (((__SOURCE__) == COMP_BLANKINGSRC_NONE)      || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM1_OC5)  || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM2_OC3)  || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM3_OC3)  || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM3_OC4)  || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM8_OC5)  || \
                                             ((__SOURCE__) == COMP_BLANKINGSRC_TIM15_OC1))


#define IS_COMP_TRIGGERMODE(__MODE__)       (((__MODE__) == COMP_TRIGGERMODE_NONE)                 || \
                                             ((__MODE__) == COMP_TRIGGERMODE_IT_RISING)            || \
                                             ((__MODE__) == COMP_TRIGGERMODE_IT_FALLING)           || \
                                             ((__MODE__) == COMP_TRIGGERMODE_IT_RISING_FALLING)    || \
                                             ((__MODE__) == COMP_TRIGGERMODE_EVENT_RISING)         || \
                                             ((__MODE__) == COMP_TRIGGERMODE_EVENT_FALLING)        || \
                                             ((__MODE__) == COMP_TRIGGERMODE_EVENT_RISING_FALLING))

#define IS_COMP_OUTPUT_LEVEL(__OUTPUT_LEVEL__) (((__OUTPUT_LEVEL__) == COMP_OUTPUT_LEVEL_LOW)     || \
                                                ((__OUTPUT_LEVEL__) == COMP_OUTPUT_LEVEL_HIGH))

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup COMP_Exported_Functions
  * @{
  */

/** @addtogroup COMP_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_COMP_Init(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_DeInit (COMP_HandleTypeDef *hcomp);
void              HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp);
void              HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp);
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
/* Callbacks Register/UnRegister functions  ***********************************/
HAL_StatusTypeDef HAL_COMP_RegisterCallback(COMP_HandleTypeDef *hcomp, HAL_COMP_CallbackIDTypeDef CallbackID, pCOMP_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_COMP_UnRegisterCallback(COMP_HandleTypeDef *hcomp, HAL_COMP_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
/**
  * @}
  */

/* IO operation functions  *****************************************************/
/** @addtogroup COMP_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_COMP_Start(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Stop(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Start_IT(COMP_HandleTypeDef *hcomp);
HAL_StatusTypeDef HAL_COMP_Stop_IT(COMP_HandleTypeDef *hcomp);
void              HAL_COMP_IRQHandler(COMP_HandleTypeDef *hcomp);

/**
  * @}
  */

/* Peripheral Control functions  ************************************************/
/** @addtogroup COMP_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_COMP_Lock(COMP_HandleTypeDef *hcomp);
uint32_t          HAL_COMP_GetOutputLevel(COMP_HandleTypeDef *hcomp);
/* Callback in Interrupt mode */
void              HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp);
/**
  * @}
  */

/* Peripheral State functions  **************************************************/
/** @addtogroup COMP_Exported_Functions_Group4
  * @{
  */
HAL_COMP_StateTypeDef HAL_COMP_GetState(COMP_HandleTypeDef *hcomp);
uint32_t              HAL_COMP_GetError(COMP_HandleTypeDef *hcomp);
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

#endif /* STM32H7xx_HAL_COMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
