/**
  ******************************************************************************
  * @file    stm32h7xx_hal_rtc_ex.h
  * @author  MCD Application Team
  * @brief   Header file of RTC HAL Extension module.
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
#ifndef STM32H7xx_HAL_RTC_EX_H
#define STM32H7xx_HAL_RTC_EX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup RTCEx RTCEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup RTCEx_Exported_Types RTCEx Exported Types
  * @{
  */

/** @defgroup RTCEx_Tamper_structure_definition RTC Tamper structure definition
  * @{
  */
typedef struct
{
  uint32_t Tamper;                      /*!< Specifies the Tamper Pin.
                                             This parameter can be a value of @ref RTCEx_Tamper_Pins_Definitions */

  uint32_t Interrupt;                   /*!< Specifies the Tamper Interrupt.
                                             This parameter can be a value of @ref  RTCEx_Tamper_Interrupt_Definitions */

  uint32_t Trigger;                     /*!< Specifies the Tamper Trigger.
                                             This parameter can be a value of @ref RTCEx_Tamper_Trigger_Definitions */

  uint32_t NoErase;                     /*!< Specifies the Tamper no erase mode.
                                             This parameter can be a value of @ref RTCEx_Tamper_EraseBackUp_Definitions */

  uint32_t MaskFlag;                     /*!< Specifies the Tamper Flag masking.
                                             This parameter can be a value of @ref RTCEx_Tamper_MaskFlag_Definitions   */

  uint32_t Filter;                      /*!< Specifies the TAMP Filter Tamper.
                                             This parameter can be a value of @ref RTCEx_Tamper_Filter_Definitions */

  uint32_t SamplingFrequency;           /*!< Specifies the sampling frequency.
                                             This parameter can be a value of @ref RTCEx_Tamper_Sampling_Frequencies_Definitions */

  uint32_t PrechargeDuration;           /*!< Specifies the Precharge Duration .
                                             This parameter can be a value of @ref RTCEx_Tamper_Pin_Precharge_Duration_Definitions */

  uint32_t TamperPullUp;                /*!< Specifies the Tamper PullUp .
                                             This parameter can be a value of @ref RTCEx_Tamper_Pull_UP_Definitions */

  uint32_t TimeStampOnTamperDetection;  /*!< Specifies the TimeStampOnTamperDetection.
                                             This parameter can be a value of @ref RTCEx_Tamper_TimeStampOnTamperDetection_Definitions */
} RTC_TamperTypeDef;
/**
  * @}
  */

#if defined(TAMP_CR1_ITAMP1E)
/** @defgroup RTCEx_Internal_Tamper_structure_definition RTCEx Internal Tamper structure definition
  * @{
  */
typedef struct
{
  uint32_t IntTamper;                   /*!< Specifies the Internal Tamper Pin.
                                             This parameter can be a value of @ref RTCEx_Internal_Tamper_Pins */

  uint32_t TimeStampOnTamperDetection;  /*!< Specifies the TimeStampOnTamperDetection.
                                             This parameter can be a value of @ref RTCEx_Tamper_TimeStampOnTamperDetection */
} RTC_InternalTamperTypeDef;
/**
  * @}
  */
#endif /* TAMP_CR1_ITAMP1E */

#if defined(TAMP_ATCR1_TAMP1AM)
/** @defgroup RTCEx_Active_Seed_Size Seed size Definitions
  * @{
  */
#define RTC_ATAMP_SEED_NB_UINT32        4U
/**
  * @}
  */

/** @defgroup RTCEx_Active_Tampers_Number Active Tampers number Definitions
  * @{
  */
#define RTC_TAMP_NB                     3u
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_structures_definition RTC Active Tamper structures definitions
  * @{
  */
typedef struct
{
  uint32_t Enable;                      /*!< Specifies the Tamper input is active.
                                             This parameter can be a value of @ref RTCEx_ActiveTamper_Enable */

  uint32_t Interrupt;                    /*!< Specifies the interrupt mode
                                             This parameter can be a value of @ref RTCEx_ActiveTamper_Interrupt */

  uint32_t Output;                      /*!< Specifies the TAMP output to be compared with.
                                             The same output can be used for several tamper inputs.
                                             This parameter can be a value of @ref RTCEx_ActiveTamper_Sel */

  uint32_t NoErase;                     /*!< Specifies the Tamper no erase mode.
                                             This parameter can be a value of @ref RTCEx_Tamper_EraseBackUp */

  uint32_t MaskFlag;                    /*!< Specifies the Tamper Flag masking.
                                             This parameter can be a value of @ref RTCEx_Tamper_MaskFlag */

} RTC_ATampInputTypeDef;


typedef struct
{
  uint32_t ActiveFilter;                /*!< Specifies the Active tamper filter enable.
                                             This parameter can be a value of @ref RTCEx_ActiveTamper_Filter */

  uint32_t ActiveAsyncPrescaler;        /*!< Specifies the Active Tamper asynchronous Prescaler clock.
                                             This parameter can be a value of @ref RTCEx_ActiveTamper_Async_prescaler */

  uint32_t TimeStampOnTamperDetection;  /*!< Specifies the timeStamp on tamper detection.
                                             This parameter can be a value of @ref RTCEx_Tamper_TimeStampOnTamperDetection */

  uint32_t ActiveOutputChangePeriod;    /*!< Specifies the Active Tamper output change period .
                                             This parameter can be a value from 0 to 7. */

  uint32_t Seed[RTC_ATAMP_SEED_NB_UINT32];
  /*!< Specifies the Active Tamper RNG Seed value .
       This parameter can be a value from 0 to (2^32-1) */

  RTC_ATampInputTypeDef TampInput[RTC_TAMP_NB];
  /*!< Specifies configuration of all active tampers.
       The index of TampInput[RTC_TAMP_NB] can be a value of RTCEx_ActiveTamper_Sel */
} RTC_ActiveTampersTypeDef;

/**
  * @}
  */
#endif /* TAMP_ATCR1_TAMP1AM */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup RTCEx_Exported_Constants RTC Exported Constants
  * @{
  */

/* ========================================================================== */
/*         ##### Extended RTC Backup registers exported constants #####       */
/* ========================================================================== */

/** @defgroup RTCEx_Backup_Registers_Number_Definitions RTC Backup Registers Number Definitions
  * @{
  */
#if defined(RTC_BKP_NUMBER)
#define BKP_REG_NUMBER                       RTC_BKP_NUMBER
#endif /* RTC_BKP_NUMBER */
#if defined(TAMP_BKP_NUMBER)
#define BKP_REG_NUMBER                       TAMP_BKP_NUMBER
#endif /* TAMP_BKP_NUMBER */
/**
  * @}
  */

/** @defgroup RTCEx_Backup_Registers_Definitions RTC Backup Registers Definitions
  * @{
  */
#define RTC_BKP_DR0                       0x00u
#define RTC_BKP_DR1                       0x01u
#define RTC_BKP_DR2                       0x02u
#define RTC_BKP_DR3                       0x03u
#define RTC_BKP_DR4                       0x04u
#define RTC_BKP_DR5                       0x05u
#define RTC_BKP_DR6                       0x06u
#define RTC_BKP_DR7                       0x07u
#define RTC_BKP_DR8                       0x08u
#define RTC_BKP_DR9                       0x09u
#define RTC_BKP_DR10                      0x0Au
#define RTC_BKP_DR11                      0x0Bu
#define RTC_BKP_DR12                      0x0Cu
#define RTC_BKP_DR13                      0x0Du
#define RTC_BKP_DR14                      0x0Eu
#define RTC_BKP_DR15                      0x0Fu
#define RTC_BKP_DR16                      0x10u
#define RTC_BKP_DR17                      0x11u
#define RTC_BKP_DR18                      0x12u
#define RTC_BKP_DR19                      0x13u
#define RTC_BKP_DR20                      0x14u
#define RTC_BKP_DR21                      0x15u
#define RTC_BKP_DR22                      0x16u
#define RTC_BKP_DR23                      0x17u
#define RTC_BKP_DR24                      0x18u
#define RTC_BKP_DR25                      0x19u
#define RTC_BKP_DR26                      0x1Au
#define RTC_BKP_DR27                      0x1Bu
#define RTC_BKP_DR28                      0x1Cu
#define RTC_BKP_DR29                      0x1Du
#define RTC_BKP_DR30                      0x1Eu
#define RTC_BKP_DR31                      0x1Fu
/**
  * @}
  */

/* ========================================================================== */
/*                 ##### RTC TimeStamp exported constants #####               */
/* ========================================================================== */

/** @defgroup RTCEx_TimeStamp_Edges_definitions RTC TimeStamp Edges Definitions
  *
  * @{
  */
#define RTC_TIMESTAMPEDGE_RISING            0x00000000u
#define RTC_TIMESTAMPEDGE_FALLING           RTC_CR_TSEDGE
/**
  * @}
  */

/** @defgroup RTCEx_TimeStamp_Pin_Selection RTC TimeStamp Pin Selection
  * @{
  */
#define RTC_TIMESTAMPPIN_DEFAULT              0x00000000u
/**
  * @}
  */


/* ========================================================================== */
/*                    ##### RTC Tamper exported constants #####               */
/* ========================================================================== */

/** @defgroup RTCEx_Tamper_Pins_Definitions RTC Tamper Pins Definitions
  * @{
  */
#if defined(TAMP_CR1_TAMP1E)
#define RTC_TAMPER_1                        TAMP_CR1_TAMP1E
#endif /* TAMP_CR1_TAMP1E */
#if defined(TAMP_CR1_TAMP2E)
#define RTC_TAMPER_2                        TAMP_CR1_TAMP2E
#endif /* TAMP_CR1_TAMP2E */
#if defined(TAMP_CR1_TAMP3E)
#define RTC_TAMPER_3                        TAMP_CR1_TAMP3E
#endif /* TAMP_CR1_TAMP3E */

#if defined(RTC_TAMPCR_TAMP1E)
#define RTC_TAMPER_1                       RTC_TAMPCR_TAMP1E
#endif /* RTC_TAMPCR_TAMP1E */
#if defined(RTC_TAMPCR_TAMP2E)
#define RTC_TAMPER_2                       RTC_TAMPCR_TAMP2E
#endif /* RTC_TAMPCR_TAMP2E */
#if defined(RTC_TAMPCR_TAMP3E)
#define RTC_TAMPER_3                       RTC_TAMPCR_TAMP3E
#endif /* RTC_TAMPCR_TAMP3E */

#define RTC_TAMPER_ALL                     (RTC_TAMPER_1 | RTC_TAMPER_2 | RTC_TAMPER_3)
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Interrupt_Definitions RTC Tamper Interrupts Definitions
  * @{
  */
#if defined(TAMP_IER_TAMP1IE)
#define RTC_IT_TAMP1                      TAMP_IER_TAMP1IE    /*!< Enable Tamper 1 Interrupt                */
#endif /* TAMP_IER_TAMP1IE */
#if defined(TAMP_IER_TAMP2IE)
#define RTC_IT_TAMP2                      TAMP_IER_TAMP2IE    /*!< Enable Tamper 2 Interrupt                */
#endif /* TAMP_IER_TAMP2IE */
#if defined(TAMP_IER_TAMP3IE)
#define RTC_IT_TAMP3                      TAMP_IER_TAMP3IE    /*!< Enable Tamper 3 Interrupt                */
#endif /* TAMP_IER_TAMP3IE */

#if defined(RTC_TAMPCR_TAMP1IE)
#define RTC_IT_TAMP1                      RTC_TAMPCR_TAMP1IE  /*!< Enable Tamper 1 Interrupt                */
#endif /* RTC_TAMPCR_TAMP1IE */
#if defined(RTC_TAMPCR_TAMP2IE)
#define RTC_IT_TAMP2                      RTC_TAMPCR_TAMP2IE  /*!< Enable Tamper 2 Interrupt                */
#endif /* RTC_TAMPCR_TAMP2IE */
#if defined(RTC_TAMPCR_TAMP3IE)
#define RTC_IT_TAMP3                      RTC_TAMPCR_TAMP3IE  /*!< Enable Tamper 3 Interrupt                */
#endif /* RTC_TAMPCR_TAMP3IE */

#if defined(RTC_TAMPCR_TAMPIE)
#define RTC_IT_TAMP                       RTC_TAMPCR_TAMPIE   /*!< Enable all Tamper Interrupts              */
#define RTC_IT_TAMPALL                    RTC_IT_TAMP
#else /* RTC_TAMPCR_TAMPIE not defined */
#define RTC_IT_TAMP                       0x00000000u         /*!< No such feature in RTC3                   */
#define RTC_IT_TAMPALL                    (RTC_IT_TAMP1 | RTC_IT_TAMP2 | RTC_IT_TAMP3)
#endif /* RTC_TAMPCR_TAMPIE */
/**
  * @}
  */

/** @defgroup RTCEx_Internal_Tamper_Pins_Definitions  RTCEx Internal Tamper Pins Definition
  * @{
  */
#if defined(TAMP_CR1_ITAMP1E)
#define RTC_INT_TAMPER_1                    TAMP_CR1_ITAMP1E
#endif /* TAMP_CR1_ITAMP1E */
#if defined(TAMP_CR1_ITAMP2E)
#define RTC_INT_TAMPER_2                    TAMP_CR1_ITAMP2E
#endif /* TAMP_CR1_ITAMP2E */
#if defined(TAMP_CR1_ITAMP3E)
#define RTC_INT_TAMPER_3                    TAMP_CR1_ITAMP3E
#endif /* TAMP_CR1_ITAMP3E */
#if defined(TAMP_CR1_ITAMP4E)
#define RTC_INT_TAMPER_4                    TAMP_CR1_ITAMP4E
#endif /* TAMP_CR1_ITAMP4E */
#if defined(TAMP_CR1_ITAMP5E)
#define RTC_INT_TAMPER_5                    TAMP_CR1_ITAMP5E
#endif /* TAMP_CR1_ITAMP5E */
#if defined(TAMP_CR1_ITAMP6E)
#define RTC_INT_TAMPER_6                    TAMP_CR1_ITAMP6E
#endif /* TAMP_CR1_ITAMP6E */
#if defined(TAMP_CR1_ITAMP8E)
#define RTC_INT_TAMPER_8                    TAMP_CR1_ITAMP8E
#define RTC_INT_TAMPER_ALL                  (RTC_INT_TAMPER_1 | RTC_INT_TAMPER_2 |\
                                             RTC_INT_TAMPER_3 | RTC_INT_TAMPER_4 |\
                                             RTC_INT_TAMPER_5 | RTC_INT_TAMPER_6 |\
                                             RTC_INT_TAMPER_8)
#endif /* TAMP_CR1_ITAMP8E */
/**
  * @}
  */

/** @defgroup RTCEx_Internal_Tamper_Interrupt_Definitions  RTC Internal Tamper Interrupt
  * @{
  */
#if defined(TAMP_IER_ITAMP1IE)
#define RTC_INTERNAL_TAMPER1_INTERRUPT     TAMP_IER_ITAMP1IE  /*!< Enable Internal Tamper 1 Interrupt   */
#endif /* TAMP_IER_ITAMP1IE */
#if defined(TAMP_IER_ITAMP2IE)
#define RTC_INTERNAL_TAMPER2_INTERRUPT     TAMP_IER_ITAMP2IE  /*!< Enable Internal Tamper 2 Interrupt   */
#endif /* TAMP_IER_ITAMP2IE */
#if defined(TAMP_IER_ITAMP3IE)
#define RTC_INTERNAL_TAMPER3_INTERRUPT     TAMP_IER_ITAMP3IE  /*!< Enable Internal Tamper 3 Interrupt   */
#endif /* TAMP_IER_ITAMP3IE */
#if defined(TAMP_IER_ITAMP4IE)
#define RTC_INTERNAL_TAMPER4_INTERRUPT     TAMP_IER_ITAMP4IE  /*!< Enable Internal Tamper 4 Interrupt   */
#endif /* TAMP_IER_ITAMP4IE */
#if defined(TAMP_IER_ITAMP5IE)
#define RTC_INTERNAL_TAMPER5_INTERRUPT     TAMP_IER_ITAMP5IE  /*!< Enable Internal Tamper 5 Interrupt   */
#endif /* TAMP_IER_ITAMP5IE */
#if defined(TAMP_IER_ITAMP6IE)
#define RTC_INTERNAL_TAMPER6_INTERRUPT     TAMP_IER_ITAMP6IE  /*!< Enable Internal Tamper 6 Interrupt   */
#endif /* TAMP_IER_ITAMP6IE */
#if defined(TAMP_IER_ITAMP8IE)
#define RTC_INTERNAL_TAMPER8_INTERRUPT     TAMP_IER_ITAMP8IE  /*!< Enable Internal Tamper 8 Interrupt   */
#endif /* TAMP_IER_ITAMP8IE */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Trigger_Definitions RTC Tamper Triggers Definitions
  * @{
  */
#define RTC_TAMPERTRIGGER_RISINGEDGE        0x01u  /*!< Warning : Filter must be RTC_TAMPERFILTER_DISABLE */
#define RTC_TAMPERTRIGGER_FALLINGEDGE       0x02u  /*!< Warning : Filter must be RTC_TAMPERFILTER_DISABLE */
#define RTC_TAMPERTRIGGER_LOWLEVEL          0x04u  /*!< Warning : Filter must not be RTC_TAMPERFILTER_DISABLE */
#define RTC_TAMPERTRIGGER_HIGHLEVEL         0x08u  /*!< Warning : Filter must not be RTC_TAMPERFILTER_DISABLE */

#if defined(TAMP_CR2_TAMP1TRG)
#define RTC_TAMPER_1_TRIGGER                TAMP_CR2_TAMP1TRG
#endif /* TAMP_CR2_TAMP1TRG */
#if defined(TAMP_CR2_TAMP2TRG)
#define RTC_TAMPER_2_TRIGGER                TAMP_CR2_TAMP2TRG
#endif /* TAMP_CR2_TAMP2TRG */
#if defined(TAMP_CR2_TAMP3TRG)
#define RTC_TAMPER_3_TRIGGER                TAMP_CR2_TAMP3TRG
#endif /* TAMP_CR2_TAMP3TRG */

#if defined(RTC_TAMPCR_TAMP1TRG)
#define RTC_TAMPER_1_TRIGGER                RTC_TAMPCR_TAMP1TRG
#endif /* RTC_TAMPCR_TAMP1TRG */
#if defined(RTC_TAMPCR_TAMP2TRG)
#define RTC_TAMPER_2_TRIGGER                RTC_TAMPCR_TAMP2TRG
#endif /* RTC_TAMPCR_TAMP2TRG */
#if defined(RTC_TAMPCR_TAMP3TRG)
#define RTC_TAMPER_3_TRIGGER                RTC_TAMPCR_TAMP3TRG
#endif /* RTC_TAMPCR_TAMP3TRG */

#define RTC_TAMPER_X_TRIGGER                (RTC_TAMPER_1_TRIGGER |\
                                             RTC_TAMPER_2_TRIGGER |\
                                             RTC_TAMPER_3_TRIGGER)
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_EraseBackUp_Definitions RTC Tamper EraseBackUp Definitions
* @{
*/
#if defined(TAMP_CR2_TAMP1NOERASE)
#define RTC_TAMPER_ERASE_BACKUP_ENABLE      0x00u
#define RTC_TAMPER_ERASE_BACKUP_DISABLE     0x01u
#endif /* TAMP_CR2_TAMP1NOERASE */
#if defined(RTC_TAMPCR_TAMP1NOERASE)
#define RTC_TAMPER_ERASE_BACKUP_ENABLE     0x00000000u
#define RTC_TAMPER_ERASE_BACKUP_DISABLE    RTC_TAMPCR_TAMP1NOERASE
#endif /* RTC_TAMPCR_TAMP1NOERASE */

#if defined(TAMP_CR2_TAMP1NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_1  TAMP_CR2_TAMP1NOERASE
#endif /* TAMP_CR2_TAMP1NOERASE */
#if defined(TAMP_CR2_TAMP2NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_2  TAMP_CR2_TAMP2NOERASE
#endif /* TAMP_CR2_TAMP2NOERASE */
#if defined(TAMP_CR2_TAMP3NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_3  TAMP_CR2_TAMP3NOERASE
#endif /* TAMP_CR2_TAMP3NOERASE */

#if defined(RTC_TAMPCR_TAMP1NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_1  RTC_TAMPCR_TAMP1NOERASE
#endif /* RTC_TAMPCR_TAMP1NOERASE */
#if defined(RTC_TAMPCR_TAMP2NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_2  RTC_TAMPCR_TAMP2NOERASE
#endif /* RTC_TAMPCR_TAMP2NOERASE */
#if defined(RTC_TAMPCR_TAMP3NOERASE)
#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_3  RTC_TAMPCR_TAMP3NOERASE
#endif /* RTC_TAMPCR_TAMP3NOERASE */

#define RTC_DISABLE_BKP_ERASE_ON_TAMPER_MASK (RTC_DISABLE_BKP_ERASE_ON_TAMPER_1 |\
                                              RTC_DISABLE_BKP_ERASE_ON_TAMPER_2 |\
                                              RTC_DISABLE_BKP_ERASE_ON_TAMPER_3)
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_MaskFlag_Definitions RTC Tamper Mask Flag Definitions
* @{
*/
#if defined(TAMP_CR2_TAMP1MSK)
#define RTC_TAMPERMASK_FLAG_DISABLE         0x00u
#define RTC_TAMPERMASK_FLAG_ENABLE          0x01u
#endif /* TAMP_CR2_TAMP1MSK */
#if defined(RTC_TAMPCR_TAMP1MF)
#define RTC_TAMPERMASK_FLAG_DISABLE        0x00000000u
#define RTC_TAMPERMASK_FLAG_ENABLE         RTC_TAMPCR_TAMP1MF
#endif /* RTC_TAMPCR_TAMP1MF */

#if defined(TAMP_CR2_TAMP1MSK)
#define RTC_TAMPER_1_MASK_FLAG             TAMP_CR2_TAMP1MSK
#endif /* TAMP_CR2_TAMP1MSK */
#if defined(TAMP_CR2_TAMP2MSK)
#define RTC_TAMPER_2_MASK_FLAG             TAMP_CR2_TAMP2MSK
#endif /* TAMP_CR2_TAMP2MSK */
#if defined(TAMP_CR2_TAMP3MSK)
#define RTC_TAMPER_3_MASK_FLAG             TAMP_CR2_TAMP3MSK
#endif /* TAMP_CR2_TAMP3MSK */

#if defined(RTC_TAMPCR_TAMP1MF)
#define RTC_TAMPER_1_MASK_FLAG             RTC_TAMPCR_TAMP1MF
#endif /* RTC_TAMPCR_TAMP1MF */
#if defined(RTC_TAMPCR_TAMP2MF)
#define RTC_TAMPER_2_MASK_FLAG             RTC_TAMPCR_TAMP2MF
#endif /* RTC_TAMPCR_TAMP2MF */
#if defined(RTC_TAMPCR_TAMP3MF)
#define RTC_TAMPER_3_MASK_FLAG             RTC_TAMPCR_TAMP3MF
#endif /* RTC_TAMPCR_TAMP3MF */

#define RTC_TAMPER_X_MASK_FLAG             (RTC_TAMPER_1_MASK_FLAG |\
                                            RTC_TAMPER_2_MASK_FLAG |\
                                            RTC_TAMPER_3_MASK_FLAG)
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Filter_Definitions RTC Tamper Filter Definitions
  * @{
  */
#if defined(TAMP_FLTCR_TAMPFLT)
#define RTC_TAMPERFILTER_DISABLE   0x00000000U             /*!< Tamper filter is disabled */

#define RTC_TAMPERFILTER_2SAMPLE   TAMP_FLTCR_TAMPFLT_0    /*!< Tamper is activated after 2
                                                                 consecutive samples at the active level */
#define RTC_TAMPERFILTER_4SAMPLE   TAMP_FLTCR_TAMPFLT_1    /*!< Tamper is activated after 4
                                                                 consecutive samples at the active level */
#define RTC_TAMPERFILTER_8SAMPLE   TAMP_FLTCR_TAMPFLT      /*!< Tamper is activated after 8
                                                                 consecutive samples at the active level */
#define RTC_TAMPERFILTER_MASK      TAMP_FLTCR_TAMPFLT      /*!< Masking all bits except those of
                                                                 field TAMPFLT[1:0]. */
#endif /* TAMP_FLTCR_TAMPFLT */
#if defined(RTC_TAMPCR_TAMPFLT)
#define RTC_TAMPERFILTER_DISABLE   0x00000000u             /*!< Tamper filter is disabled */

#define RTC_TAMPERFILTER_2SAMPLE   RTC_TAMPCR_TAMPFLT_0    /*!< Tamper is activated after 2
                                                                 consecutive samples at the active level */
#define RTC_TAMPERFILTER_4SAMPLE   RTC_TAMPCR_TAMPFLT_1    /*!< Tamper is activated after 4
                                                                 consecutive samples at the active level */
#define RTC_TAMPERFILTER_8SAMPLE   RTC_TAMPCR_TAMPFLT      /*!< Tamper is activated after 8
                                                                 consecutive samples at the active level. */
#define RTC_TAMPERFILTER_MASK      RTC_TAMPCR_TAMPFLT      /*!< Masking all bits except those of
                                                                 field TAMPFLT[1:0]. */
#endif /* RTC_TAMPCR_TAMPFLT */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Sampling_Frequencies_Definitions RTC Tamper Sampling Frequencies Definitions
  * @{
  */
#if defined(TAMP_FLTCR_TAMPFREQ)
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768  0x00000000U                                     /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 32768 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384  TAMP_FLTCR_TAMPFREQ_0                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 16384 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192   TAMP_FLTCR_TAMPFREQ_1                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 8192  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096   (TAMP_FLTCR_TAMPFREQ_0 | TAMP_FLTCR_TAMPFREQ_1) /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 4096  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048   TAMP_FLTCR_TAMPFREQ_2                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 2048  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024   (TAMP_FLTCR_TAMPFREQ_0 | TAMP_FLTCR_TAMPFREQ_2) /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 1024  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512    (TAMP_FLTCR_TAMPFREQ_1 | TAMP_FLTCR_TAMPFREQ_2) /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 512   */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256    TAMP_FLTCR_TAMPFREQ                             /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 256   */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK      TAMP_FLTCR_TAMPFREQ                             /*!< Masking all bits except those of
                                                                                                      field TAMPFREQ[2:0]*/
#endif /* TAMP_FLTCR_TAMPFREQ */
#if defined(RTC_TAMPCR_TAMPFREQ)
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768  0x00000000u                                     /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 32768 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384  RTC_TAMPCR_TAMPFREQ_0                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 16384 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192   RTC_TAMPCR_TAMPFREQ_1                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 8192  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096   (RTC_TAMPCR_TAMPFREQ_0 | RTC_TAMPCR_TAMPFREQ_1) /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 4096  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048   RTC_TAMPCR_TAMPFREQ_2                           /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 2048  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024   (RTC_TAMPCR_TAMPFREQ_0 | RTC_TAMPCR_TAMPFREQ_2)  /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 1024  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512    (RTC_TAMPCR_TAMPFREQ_1 | RTC_TAMPCR_TAMPFREQ_2)  /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 512   */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256    RTC_TAMPCR_TAMPFREQ                              /*!< Each of the tamper inputs are sampled
                                                                                                      with a frequency =  RTCCLK / 256   */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK      RTC_TAMPCR_TAMPFREQ                             /*!< Masking all bits except those of
                                                                                                      field TAMPFREQ[2:0]*/
#endif /* RTC_TAMPCR_TAMPFREQ */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Pin_Precharge_Duration_Definitions RTC Tamper Pin Precharge Duration Definitions
  * @{
  */
#if defined(TAMP_FLTCR_TAMPPRCH)
#define RTC_TAMPERPRECHARGEDURATION_1RTCCLK     0x00000000U             /*!< Tamper pins are pre-charged before
                                                                              sampling during 1 RTCCLK cycle  */
#define RTC_TAMPERPRECHARGEDURATION_2RTCCLK     TAMP_FLTCR_TAMPPRCH_0   /*!< Tamper pins are pre-charged before
                                                                              sampling during 2 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_4RTCCLK     TAMP_FLTCR_TAMPPRCH_1   /*!< Tamper pins are pre-charged before
                                                                              sampling during 4 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_8RTCCLK     TAMP_FLTCR_TAMPPRCH     /*!< Tamper pins are pre-charged before
                                                                              sampling during 8 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_MASK        TAMP_FLTCR_TAMPPRCH     /*!< Masking all bits except those of
                                                                              field TAMPPRCH[1:0] */
#endif /* TAMP_FLTCR_TAMPPRCH */
#if defined(RTC_TAMPCR_TAMPPRCH)
#define RTC_TAMPERPRECHARGEDURATION_1RTCCLK     0x00000000u             /*!< Tamper pins are pre-charged before
                                                                              sampling during 1 RTCCLK cycle  */
#define RTC_TAMPERPRECHARGEDURATION_2RTCCLK     RTC_TAMPCR_TAMPPRCH_0   /*!< Tamper pins are pre-charged before
                                                                              sampling during 2 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_4RTCCLK     RTC_TAMPCR_TAMPPRCH_1   /*!< Tamper pins are pre-charged before
                                                                              sampling during 4 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_8RTCCLK     RTC_TAMPCR_TAMPPRCH     /*!< Tamper pins are pre-charged before
                                                                              sampling during 8 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_MASK        RTC_TAMPCR_TAMPPRCH     /*!< Masking all bits except those of
                                                                              field TAMPPRCH[1:0] */
#endif /* RTC_TAMPCR_TAMPPRCH */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_TimeStampOnTamperDetection_Definitions RTC Tamper TimeStamp On Tamper Detection Definitions
  * @{
  */
#if defined(RTC_CR_TAMPTS)
#define RTC_TIMESTAMPONTAMPERDETECTION_DISABLE  0x00000000u                   /*!< TimeStamp on Tamper Detection event is not saved */
#define RTC_TIMESTAMPONTAMPERDETECTION_ENABLE   RTC_CR_TAMPTS                 /*!< TimeStamp on Tamper Detection event saved        */
#define RTC_TIMESTAMPONTAMPERDETECTION_MASK     RTC_CR_TAMPTS                 /*!< Masking all bits except bit TAMPTS               */
#endif  /* RTC_CR_TAMPTS */
#if defined(RTC_TAMPCR_TAMPTS)
#define RTC_TIMESTAMPONTAMPERDETECTION_DISABLE  0x00000000u                   /*!< TimeStamp on Tamper Detection event is not saved */
#define RTC_TIMESTAMPONTAMPERDETECTION_ENABLE   RTC_TAMPCR_TAMPTS             /*!< TimeStamp on Tamper Detection event saved        */
#define RTC_TIMESTAMPONTAMPERDETECTION_MASK     RTC_TAMPCR_TAMPTS             /*!< Masking all bits except bit TAMPTS               */
#endif /* RTC_TAMPCR_TAMPTS */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Pull_UP_Definitions RTC Tamper Pull Up Definitions
  * @{
  */
#if defined(TAMP_FLTCR_TAMPPUDIS)
#define RTC_TAMPER_PULLUP_ENABLE           0x00000000u              /*!< Tamper pins are pre-charged before sampling */
#define RTC_TAMPER_PULLUP_DISABLE          TAMP_FLTCR_TAMPPUDIS     /*!< Tamper pins pre-charge is disabled          */
#define RTC_TAMPER_PULLUP_MASK             TAMP_FLTCR_TAMPPUDIS     /*!< Maskin all bits except bit TAMPPUDIS        */
#endif /* TAMP_FLTCR_TAMPPUDIS */
#if defined(RTC_TAMPCR_TAMPPUDIS)
#define RTC_TAMPER_PULLUP_ENABLE           0x00000000u              /*!< TimeStamp on Tamper Detection event saved        */
#define RTC_TAMPER_PULLUP_DISABLE          RTC_TAMPCR_TAMPPUDIS     /*!< TimeStamp on Tamper Detection event is not saved */
#define RTC_TAMPER_PULLUP_MASK             RTC_TAMPCR_TAMPPUDIS     /*!< Maskin all bits except bit TAMPPUDIS        */
#endif /* RTC_TAMPCR_TAMPPUDIS */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_DetectionOutput_Definitions RTC Tamper Detection Ouput Definitions
  * @{
  */
#if defined(RTC_CR_TAMPOE)
#define RTC_TAMPERDETECTIONOUTPUT_DISABLE  0x00000000u                   /*!< The tamper flag is not routed on TAMPALRM */
#define RTC_TAMPERDETECTIONOUTPUT_ENABLE   RTC_CR_TAMPOE                 /*!< The tamper flag is routed on TAMPALRM combined with the signal
                                                                               provided by OSEL and with the polarity provided by POL */
#endif  /* RTC_CR_TAMPOE */
/**
  * @}
  */

/** @defgroup RTC_Tamper_Flags_Definitions RTC Tamper Flags Definitions
  * @{
  */
#if defined(TAMP_SR_TAMP1F)
#define RTC_FLAG_TAMP1F                   TAMP_SR_TAMP1F
#endif /* TAMP_SR_TAMP1F */
#if defined(TAMP_SR_TAMP2F)
#define RTC_FLAG_TAMP2F                   TAMP_SR_TAMP2F
#endif /* TAMP_SR_TAMP2F */
#if defined(TAMP_SR_TAMP3F)
#define RTC_FLAG_TAMP3F                   TAMP_SR_TAMP3F
#endif /* TAMP_SR_TAMP3F */

#if defined(RTC_ISR_TAMP1F)
#define RTC_FLAG_TAMP1F                   RTC_ISR_TAMP1F
#endif /* RTC_ISR_TAMP1F */
#if defined(RTC_ISR_TAMP2F)
#define RTC_FLAG_TAMP2F                   RTC_ISR_TAMP2F
#endif /* RTC_ISR_TAMP2F */
#if defined(RTC_ISR_TAMP3F)
#define RTC_FLAG_TAMP3F                   RTC_ISR_TAMP3F
#endif /* RTC_ISR_TAMP3F */
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_Enable RTCEx_ActiveTamper_Enable Definitions
  * @{
  */
#define RTC_ATAMP_ENABLE                   1u
#define RTC_ATAMP_DISABLE                  0u
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_Interrupt RTCEx_ActiveTamper_Interrupt Definitions
  * @{
  */
#define RTC_ATAMP_INTERRUPT_ENABLE        1u
#define RTC_ATAMP_INTERRUPT_DISABLE       0u
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_Filter RTCEx_ActiveTamper_Filter Definitions
  * @{
  */
#define RTC_ATAMP_FILTER_ENABLE           TAMP_ATCR1_FLTEN
#define RTC_ATAMP_FILTER_DISABLE          0u
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_Async_prescaler RTC Active_Tamper_Asynchronous_Prescaler clock Definitions
  * @{
  */
#define RTC_ATAMP_ASYNCPRES_RTCCLK        0u                                                                      /*!< RTCCLK */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_2      TAMP_ATCR1_ATCKSEL_0                                                    /*!< RTCCLK/2 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_4      TAMP_ATCR1_ATCKSEL_1                                                    /*!< RTCCLK/4 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_8     (TAMP_ATCR1_ATCKSEL_1 | TAMP_ATCR1_ATCKSEL_0)                            /*!< RTCCLK/8 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_16     TAMP_ATCR1_ATCKSEL_2                                                    /*!< RTCCLK/16 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_32    (TAMP_ATCR1_ATCKSEL_2 | TAMP_ATCR1_ATCKSEL_0)                            /*!< RTCCLK/32 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_64    (TAMP_ATCR1_ATCKSEL_2 | TAMP_ATCR1_ATCKSEL_1)                            /*!< RTCCLK/64 */
#define RTC_ATAMP_ASYNCPRES_RTCCLK_128   (TAMP_ATCR1_ATCKSEL_2 | TAMP_ATCR1_ATCKSEL_1 | TAMP_ATCR1_ATCKSEL_0)     /*!< RTCCLK/128 */
/**
  * @}
  */

/** @defgroup RTCEx_ActiveTamper_Sel  RTC Active Tamper selection Definition
  * @{
  */
#define RTC_ATAMP_1                       0u   /*!< Tamper 1 */
#define RTC_ATAMP_2                       1u   /*!< Tamper 2 */
#define RTC_ATAMP_3                       2u   /*!< Tamper 3 */
#define RTC_ATAMP_4                       3u   /*!< Tamper 4 */
#define RTC_ATAMP_5                       4u   /*!< Tamper 5 */
#define RTC_ATAMP_6                       5u   /*!< Tamper 6 */
#define RTC_ATAMP_7                       6u   /*!< Tamper 7 */
#define RTC_ATAMP_8                       7u   /*!< Tamper 8 */
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Registers_Offset  RTC Tamper Registers Offset
  * @{
  */
#if defined (TAMP_CR1_TAMP1E)
/* Add this offset to RTC registers base address to reach TAMP registers base address. */
#define TAMP_OFFSET (TAMP_BASE - RTC_BASE)
#endif /* TAMP_CR1_TAMP1E */
/**
  * @}
  */

/* ========================================================================== */
/*                   ##### RTC Wake-up exported constants #####               */
/* ========================================================================== */

/** @defgroup RTCEx_Wakeup_Timer_Definitions RTC Wakeup Timer Definitions
  * @{
  */
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        0x00000000u
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         RTC_CR_WUCKSEL_0
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         RTC_CR_WUCKSEL_1
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         (RTC_CR_WUCKSEL_0 | RTC_CR_WUCKSEL_1)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      RTC_CR_WUCKSEL_2
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      (RTC_CR_WUCKSEL_1 | RTC_CR_WUCKSEL_2)
/**
  * @}
  */

/* ========================================================================== */
/*        ##### Extended RTC Peripheral Control exported constants #####      */
/* ========================================================================== */

/** @defgroup RTCEx_Smooth_calib_period_Definitions RTC Smooth Calib Period Definitions
  * @{
  */
#define RTC_SMOOTHCALIB_PERIOD_32SEC   0x00000000u             /*!< If RTCCLK = 32768 Hz, Smooth calibration
                                                                    period is 32s,  else 2exp20 RTCCLK pulses */
#define RTC_SMOOTHCALIB_PERIOD_16SEC   RTC_CALR_CALW16         /*!< If RTCCLK = 32768 Hz, Smooth calibration
                                                                    period is 16s, else 2exp19 RTCCLK pulses */
#define RTC_SMOOTHCALIB_PERIOD_8SEC    RTC_CALR_CALW8          /*!< If RTCCLK = 32768 Hz, Smooth calibration
                                                                    period is 8s, else 2exp18 RTCCLK pulses */
/**
  * @}
  */

/** @defgroup RTCEx_Smooth_calib_Plus_pulses_Definitions RTC Smooth Calib Plus pulses Definitions
  * @{
  */
#define RTC_SMOOTHCALIB_PLUSPULSES_SET    RTC_CALR_CALP           /*!< The number of RTCCLK pulses added
                                                                       during a X -second window = Y - CALM[8:0]
                                                                       with Y = 512, 256, 128 when X = 32, 16, 8 */
#define RTC_SMOOTHCALIB_PLUSPULSES_RESET  0x00000000u             /*!< The number of RTCCLK pulses subbstited
                                                                       during a 32-second window = CALM[8:0] */
/**
  * @}
  */

/** @defgroup RTCEx_Calib_Output_selection_Definitions RTC Calib Output Selection Definitions
  * @{
  */
#define RTC_CALIBOUTPUT_512HZ            0x00000000u
#define RTC_CALIBOUTPUT_1HZ              RTC_CR_COSEL
/**
  * @}
  */

/** @defgroup RTCEx_Add_1_Second_Parameter_Definitions RTC Add 1 Second Parameter Definitions
  * @{
  */
#define RTC_SHIFTADD1S_RESET      0x00000000u
#define RTC_SHIFTADD1S_SET        RTC_SHIFTR_ADD1S
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup RTCEx_Exported_Macros RTC Exported Macros
  * @{
  */

/* ========================================================================== */
/*                    ##### RTC Wake-up exported macros #####                 */
/* ========================================================================== */

/**
  * @brief  Enable the RTC WakeUp Timer peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_ENABLE(__HANDLE__)                     ((__HANDLE__)->Instance->CR |= (RTC_CR_WUTE))

/**
  * @brief  Disable the RTC WakeUp Timer peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_DISABLE(__HANDLE__)                    ((__HANDLE__)->Instance->CR &= ~(RTC_CR_WUTE))

/**
  * @brief  Enable the RTC WakeUpTimer interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC WakeUpTimer interrupt sources to be enabled.
  *         This parameter can be:
  *            @arg @ref RTC_IT_WUT WakeUpTimer interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the RTC WakeUpTimer interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC WakeUpTimer interrupt sources to be disabled.
  *         This parameter can be:
  *            @arg @ref RTC_IT_WUT WakeUpTimer interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified RTC WakeUpTimer interrupt has occurred or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC WakeUpTimer interrupt sources to check.
  *         This parameter can be:
  *            @arg RTC_FLAG_WUTF WakeUpTimer interrupt flag
  * @retval None
  */
#if defined(RTC_SR_WUTF)
#define __HAL_RTC_WAKEUPTIMER_GET_IT(__HANDLE__, __INTERRUPT__)     (((((__HANDLE__)->Instance->SR)  & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* RTC_SR_WUTF */
#if defined(RTC_ISR_WUTF)
#define __HAL_RTC_WAKEUPTIMER_GET_IT(__HANDLE__, __INTERRUPT__)     (((((__HANDLE__)->Instance->ISR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_WUTF */

/**
  * @brief  Check whether the specified RTC Wake Up timer interrupt has been enabled or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Wake Up timer interrupt sources to check.
  *         This parameter can be:
  *            @arg @ref RTC_IT_WUT  WakeUpTimer interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)   (((((__HANDLE__)->Instance->CR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)

#if defined(RTC_SR_WUTF)
/**
  * @brief  Get the selected RTC WakeUpTimer's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC WakeUpTimer Flag sources to check.
  *          This parameter can be:
  *             @arg @ref RTC_FLAG_WUTF
  * @retval Flag status
  */
#define __HAL_RTC_WAKEUPTIMER_GET_FLAG(__HANDLE__, __FLAG__)   (((((__HANDLE__)->Instance->SR)  & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_SR_WUTF */
#if defined(RTC_ISR_WUTF)
/**
  * @brief  Get the selected RTC WakeUpTimer's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC WakeUpTimer Flag sources to check.
  *          This parameter can be:
  *             @arg @ref RTC_FLAG_WUTF
  *             @arg @ref RTC_FLAG_WUTWF
  * @retval Flag status
  */
#define __HAL_RTC_WAKEUPTIMER_GET_FLAG(__HANDLE__, __FLAG__)   (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_WUTF */

/**
  * @brief  Clear the RTC Wake Up timer's pending flags.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC WakeUpTimer Flag to clear.
  *         This parameter can be:
  *            @arg @ref RTC_FLAG_WUTF
  * @retval None
  */
#if defined(RTC_SCR_CWUTF)
#define __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SCR |= __FLAG__)
#endif /* RTC_SCR_CWUTF */
#if defined(RTC_ISR_WUTF)
#define __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR) = (~((__FLAG__) | RTC_ISR_INIT)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT))
#endif /* RTC_ISR_WUTF */

/* ========================================================================== */
/*                     ##### RTC Tamper exported macros #####                 */
/* ========================================================================== */

/**
  * @brief  Enable the RTC Tamper1 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP1E)
#define __HAL_RTC_TAMPER1_ENABLE(__HANDLE__)           (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->CR1 |= (TAMP_CR1_TAMP1E))
#endif /* TAMP_CR1_TAMP1E */
#if defined(RTC_TAMPCR_TAMP1E)
#define __HAL_RTC_TAMPER1_ENABLE(__HANDLE__)           ((__HANDLE__)->Instance->TAMPCR |= (RTC_TAMPCR_TAMP1E))
#endif /* RTC_TAMPCR_TAMP1E */

/**
  * @brief  Disable the RTC Tamper1 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP1E)
#define __HAL_RTC_TAMPER1_DISABLE(__HANDLE__)          (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->CR1 &= ~(RTC_TAMPCR_TAMP1E))
#endif /* TAMP_CR1_TAMP1E */
#if defined(RTC_TAMPCR_TAMP1E)
#define __HAL_RTC_TAMPER1_DISABLE(__HANDLE__)          ((__HANDLE__)->Instance->TAMPCR &= ~(RTC_TAMPCR_TAMP1E))
#endif /* RTC_TAMPCR_TAMP1E */

/**
  * @brief  Enable the RTC Tamper2 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP2E)
#define __HAL_RTC_TAMPER2_ENABLE(__HANDLE__)           (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->CR1 |= (TAMP_CR1_TAMP2E))
#endif /* TAMP_CR1_TAMP2E */
#if defined(RTC_TAMPCR_TAMP2E)
#define __HAL_RTC_TAMPER2_ENABLE(__HANDLE__)           ((__HANDLE__)->Instance->TAMPCR |= (RTC_TAMPCR_TAMP2E))
#endif /* RTC_TAMPCR_TAMP2E */

/**
  * @brief  Disable the RTC Tamper2 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP2E)
#define __HAL_RTC_TAMPER2_DISABLE(__HANDLE__)          (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + (TAMP_OFFSET))->CR1 &= ~(RTC_TAMPCR_TAMP2E))
#endif /* TAMP_CR1_TAMP2E */
#if defined(RTC_TAMPCR_TAMP2E)
#define __HAL_RTC_TAMPER2_DISABLE(__HANDLE__)          ((__HANDLE__)->Instance->TAMPCR &= ~(RTC_TAMPCR_TAMP2E))
#endif /* RTC_TAMPCR_TAMP2E */

/**
  * @brief  Enable the RTC Tamper3 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP3E)
#define __HAL_RTC_TAMPER3_ENABLE(__HANDLE__)          (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->CR1 |= (TAMP_CR1_TAMP3E))
#endif /* TAMP_CR1_TAMP3E */
#if defined(RTC_TAMPCR_TAMP3E)
#define __HAL_RTC_TAMPER3_ENABLE(__HANDLE__)          ((__HANDLE__)->Instance->TAMPCR |= (RTC_TAMPCR_TAMP3E))
#endif /* RTC_TAMPCR_TAMP3E */

/**
  * @brief  Disable the RTC Tamper3 input detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(TAMP_CR1_TAMP3E)
#define __HAL_RTC_TAMPER3_DISABLE(__HANDLE__)          (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->CR1 &= ~(RTC_TAMPCR_TAMP3E))
#endif /* TAMP_CR1_TAMP3E */
#if defined(RTC_TAMPCR_TAMP3E)
#define __HAL_RTC_TAMPER3_DISABLE(__HANDLE__)          ((__HANDLE__)->Instance->TAMPCR &= ~(RTC_TAMPCR_TAMP3E))
#endif /* RTC_TAMPCR_TAMP3E */

/**
  * @brief  Enable the RTC Tamper interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Tamper interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *             @arg  RTC_IT_TAMPALL: All tampers interrupts
  *             @arg  RTC_IT_TAMP1: Tamper1 interrupt
  *             @arg  RTC_IT_TAMP2: Tamper2 interrupt
  *             @arg  RTC_IT_TAMP3: Tamper3 interrupt
  * @retval None
  */
#if defined(TAMP_IER_TAMP1IE)
#define __HAL_RTC_TAMPER_ENABLE_IT(__HANDLE__, __INTERRUPT__)        (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->IER |= (__INTERRUPT__))
#endif /* TAMP_IER_TAMP1IE */
#if defined(RTC_TAMPCR_TAMP1IE)
#define __HAL_RTC_TAMPER_ENABLE_IT(__HANDLE__, __INTERRUPT__)        ((__HANDLE__)->Instance->TAMPCR |= (__INTERRUPT__))
#endif /* RTC_TAMPCR_TAMP1IE */

/**
  * @brief  Disable the RTC Tamper interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Tamper interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg  RTC_IT_TAMP: All tampers interrupts
  *            @arg  RTC_IT_TAMP1: Tamper1 interrupt
  *            @arg  RTC_IT_TAMP2: Tamper2 interrupt
  *            @arg  RTC_IT_TAMP3: Tamper3 interrupt
  * @retval None
  */
#if defined(TAMP_IER_TAMP1IE)
#define __HAL_RTC_TAMPER_DISABLE_IT(__HANDLE__, __INTERRUPT__)       (((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->IER &= ~(__INTERRUPT__))
#endif /* TAMP_IER_TAMP1IE */
#if defined(RTC_TAMPCR_TAMP1IE)
#define __HAL_RTC_TAMPER_DISABLE_IT(__HANDLE__, __INTERRUPT__)       ((__HANDLE__)->Instance->TAMPCR &= ~(__INTERRUPT__))
#endif /* RTC_TAMPCR_TAMP1IE */

/**
  * @brief  Check whether the specified RTC Tamper interrupt has occurred or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Tamper interrupt to check.
  *         This parameter can be:
  *            @arg  RTC_FLAG_TAMP1F: Tamper1 interrupt flag
  *            @arg  RTC_FLAG_TAMP2F: Tamper2 interrupt flag
  *            @arg  RTC_FLAG_TAMP3F: Tamper3 interrupt flag
  * @retval Flag status
  */
#if defined(TAMP_SR_TAMP1F)
#define __HAL_RTC_TAMPER_GET_IT(__HANDLE__, __INTERRUPT__)     ((((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->SR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* TAMP_SR_TAMP1F */
#if defined(RTC_ISR_TAMP1F)
#define __HAL_RTC_TAMPER_GET_IT(__HANDLE__, __INTERRUPT__)     (((((__HANDLE__)->Instance->ISR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_TAMP1F */

/**
  * @brief  Check whether the specified RTC Tamper interrupt has been enabled or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Tamper interrupt source to check.
  *         This parameter can be:
  *            @arg  RTC_IT_TAMPALL: All tampers interrupts
  *            @arg  RTC_IT_TAMP1: Tamper1 interrupt
  *            @arg  RTC_IT_TAMP2: Tamper2 interrupt
  *            @arg  RTC_IT_TAMP3: Tamper3 interrupt
  * @retval Flag status
  */
#if defined(TAMP_IER_TAMP1IE)
#define __HAL_RTC_TAMPER_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)    ((((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->IER) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* TAMP_IER_TAMP1IE */
#if defined(RTC_TAMPCR_TAMP1IE)
#define __HAL_RTC_TAMPER_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)    (((((__HANDLE__)->Instance->TAMPCR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* RTC_TAMPCR_TAMP1IE */

/**
  * @brief  Get the selected RTC Tamper's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Tamper Flag is pending or not.
  *          This parameter can be:
  *             @arg RTC_FLAG_TAMP1F: Tamper1 flag
  *             @arg RTC_FLAG_TAMP2F: Tamper2 flag
  *             @arg RTC_FLAG_TAMP3F: Tamper3 flag
  * @retval Flag status
  */
#if defined(TAMP_SR_TAMP1F)
#define __HAL_RTC_TAMPER_GET_FLAG(__HANDLE__, __FLAG__)   ((((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->SR) & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* TAMP_SR_TAMP1F */
#if defined(RTC_ISR_TAMP1F)
#define __HAL_RTC_TAMPER_GET_FLAG(__HANDLE__, __FLAG__)                               (((((__HANDLE__)->Instance->ISR)                  & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_TAMP1F */

/**
  * @brief  Clear the RTC Tamper's pending flags.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Tamper Flag to clear.
  *          This parameter can be:
  *             @arg RTC_FLAG_TAMP1F: Tamper1 flag
  *             @arg RTC_FLAG_TAMP2F: Tamper2 flag
  *             @arg RTC_FLAG_TAMP3F: Tamper3 flag
  * @retval None
  */
#if defined(TAMP_SCR_CTAMP1F)
#define __HAL_RTC_TAMPER_CLEAR_FLAG(__HANDLE__, __FLAG__)      ((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->SCR) |= (__FLAG__))
#endif /* TAMP_SCR_CTAMP1F */
#if defined(RTC_ISR_TAMP1F)
#define __HAL_RTC_TAMPER_CLEAR_FLAG(__HANDLE__, __FLAG__)      ((__HANDLE__)->Instance->ISR) = (~((__FLAG__) | RTC_ISR_INIT)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT))
#endif /* RTC_ISR_TAMP1F */

/**
  * @brief  Get the frequency at which each of the Tamper inputs are sampled.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval Sampling frequency
  *         This value can be:
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512
  *            @arg RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256
  */
#if defined(TAMP_FLTCR_TAMPFREQ)
#define __HAL_RTC_TAMPER_GET_SAMPLING_FREQ(__HANDLE__)   ((uint32_t)((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->FLTCR) & (RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK)))
#endif /* TAMP_FLTCR_TAMPFREQ */
#if defined(RTC_TAMPCR_TAMPFREQ)
#define __HAL_RTC_TAMPER_GET_SAMPLING_FREQ(__HANDLE__)   ((uint32_t)(((__HANDLE__)->Instance->TAMPCR) & (RTC_TAMPERSAMPLINGFREQ_RTCCLK_MASK)))
#endif /* RTC_TAMPCR_TAMPFREQ */

/**
  * @brief  Get the number of consecutive samples at the specified level needed
  *         to activate a Tamper event.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval Number of consecutive samples
  *         This value can be:
  *            @arg RTC_TAMPERFILTER_DISABLE
  *            @arg RTC_TAMPERFILTER_2SAMPLE
  *            @arg RTC_TAMPERFILTER_4SAMPLE
  *            @arg RTC_TAMPERFILTER_8SAMPLE
  */
#if defined(TAMP_FLTCR_TAMPFLT)
#define __HAL_RTC_TAMPER_GET_SAMPLES_COUNT(__HANDLE__)   ((uint32_t)((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->FLTCR) & (RTC_TAMPERFILTER_MASK)))
#endif /* TAMP_FLTCR_TAMPFLT */
#if defined(RTC_TAMPCR_TAMPFLT)
#define __HAL_RTC_TAMPER_GET_SAMPLES_COUNT(__HANDLE__)   ((uint32_t)(((__HANDLE__)->Instance->TAMPCR) & (RTC_TAMPERFILTER_MASK)))
#endif /* RTC_TAMPCR_TAMPFLT */

/**
  * @brief  Get the pull-up resistors precharge duration.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval Number of consecutive samples
  *         This value can be:
  *            @arg RTC_TAMPERPRECHARGEDURATION_1RTCCLK
  *            @arg RTC_TAMPERPRECHARGEDURATION_2RTCCLK
  *            @arg RTC_TAMPERPRECHARGEDURATION_4RTCCLK
  *            @arg RTC_TAMPERPRECHARGEDURATION_8RTCCLK
  */
#if defined(TAMP_FLTCR_TAMPPRCH)
#define __HAL_RTC_TAMPER_GET_PRCHRG_DURATION(__HANDLE__)   ((uint32_t)((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->FLTCR) & (RTC_TAMPERPRECHARGEDURATION_MASK)))
#endif /* TAMP_FLTCR_TAMPPRCH */
#if defined(RTC_TAMPCR_TAMPPRCH)
#define __HAL_RTC_TAMPER_GET_PRCHRG_DURATION(__HANDLE__)   ((uint32_t)(((__HANDLE__)->Instance->TAMPCR) & (RTC_TAMPERPRECHARGEDURATION_MASK)))
#endif /* RTC_TAMPCR_TAMPPRCH */

/**
  * @brief  Get the pull-up resistors status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval Pull-up resistors status
  *         This value can be:
  *            @arg RTC_TAMPER_PULLUP_ENABLE
  *            @arg RTC_TAMPER_PULLUP_DISABLE
  */
#if defined(TAMP_FLTCR_TAMPPUDIS)
#define __HAL_RTC_TAMPER_GET_PULLUP_STATUS(__HANDLE__)   ((uint32_t)((((TAMP_TypeDef *)((uint32_t)((__HANDLE__)->Instance) + TAMP_OFFSET))->FLTCR) & (RTC_TAMPER_PULLUP_MASK)))
#endif /* TAMP_FLTCR_TAMPPUDIS */
#if defined(RTC_TAMPCR_TAMPPUDIS)
#define __HAL_RTC_TAMPER_GET_PULLUP_STATUS(__HANDLE__)   ((uint32_t)(((__HANDLE__)->Instance->TAMPCR) & (RTC_TAMPER_PULLUP_MASK)))
#endif /* RTC_TAMPCR_TAMPPUDIS */

/* ========================================================================== */
/*                  ##### RTC TimeStamp exported macros #####                 */
/* ========================================================================== */

/**
  * @brief  Enable the RTC TimeStamp peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_ENABLE(__HANDLE__)                       ((__HANDLE__)->Instance->CR |= (RTC_CR_TSE))

/**
  * @brief  Disable the RTC TimeStamp peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_DISABLE(__HANDLE__)                      ((__HANDLE__)->Instance->CR &= ~(RTC_CR_TSE))

/**
  * @brief  Enable the RTC TimeStamp interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC TimeStamp interrupt source to be enabled.
  *         This parameter can be:
  *            @arg @ref RTC_IT_TS TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_ENABLE_IT(__HANDLE__, __INTERRUPT__)     ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the RTC TimeStamp interrupt.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC TimeStamp interrupt source to be disabled.
  *         This parameter can be:
  *            @arg @ref RTC_IT_TS TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_DISABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified RTC TimeStamp interrupt has occurred or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC TimeStamp interrupt source to check.
  *         This parameter can be:
  *            @arg @ref RTC_IT_TS TimeStamp interrupt
  * @retval None
  */
#if defined(RTC_SR_TSF)
#define __HAL_RTC_TIMESTAMP_GET_IT(__HANDLE__, __INTERRUPT__)        (((((__HANDLE__)->Instance->MISR) & ((__INTERRUPT__) >> 12)) != 0U) ? 1U : 0U)
#endif /* RTC_SR_TSF */
#if defined(RTC_ISR_TSF)
#define __HAL_RTC_TIMESTAMP_GET_IT(__HANDLE__, __INTERRUPT__)        (((((__HANDLE__)->Instance->ISR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_TSF */
/**
  * @brief  Check whether the specified RTC Time Stamp interrupt has been enabled or not.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __INTERRUPT__ specifies the RTC Time Stamp interrupt source to check.
  *         This parameter can be:
  *            @arg @ref RTC_IT_TS TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)     (((((__HANDLE__)->Instance->CR) & (__INTERRUPT__)) != 0U) ? 1U : 0U)

/**
  * @brief  Get the selected RTC TimeStamp's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC TimeStamp Flag is pending or not.
  *         This parameter can be:
  *            @arg RTC_FLAG_TSF
  *            @arg RTC_FLAG_TSOVF
  * @retval Flag status
  */
#if defined(RTC_SR_TSF)
#define __HAL_RTC_TIMESTAMP_GET_FLAG(__HANDLE__, __FLAG__)     (((((__HANDLE__)->Instance->SR)  & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_SR_TSF */
#if defined(RTC_ISR_TSF)
#define __HAL_RTC_TIMESTAMP_GET_FLAG(__HANDLE__, __FLAG__)     (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_TSF */

/**
  * @brief  Clear the RTC Time Stamp's pending flags.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Alarm Flag sources to clear.
  *          This parameter can be:
  *             @arg RTC_FLAG_TSF
  *             @arg RTC_FLAG_TSOVF
  * @retval None
  */
#if defined(RTC_SCR_CTSF)
#define __HAL_RTC_TIMESTAMP_CLEAR_FLAG(__HANDLE__, __FLAG__)   ((__HANDLE__)->Instance->SCR |= __FLAG__)
#endif /* RTC_SCR_CTSF */
#if defined(RTC_ISR_TSF)
#define __HAL_RTC_TIMESTAMP_CLEAR_FLAG(__HANDLE__, __FLAG__)   ((__HANDLE__)->Instance->ISR = (~((__FLAG__) | RTC_ISR_INIT)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT)))
#endif /* RTC_ISR_TSF */

/**
  * @brief  Enable the RTC internal TimeStamp peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_INTERNAL_TIMESTAMP_ENABLE(__HANDLE__)                ((__HANDLE__)->Instance->CR |= (RTC_CR_ITSE))

/**
  * @brief  Disable the RTC internal TimeStamp peripheral.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_INTERNAL_TIMESTAMP_DISABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR &= ~(RTC_CR_ITSE))

/**
  * @brief  Get the selected RTC Internal Time Stamp's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Internal Time Stamp Flag is pending or not.
  *         This parameter can be:
  *            @arg RTC_FLAG_ITSF
  * @retval Flag status
  */
#if defined(RTC_SR_ITSF)
#define __HAL_RTC_INTERNAL_TIMESTAMP_GET_FLAG(__HANDLE__, __FLAG__)    (((((__HANDLE__)->Instance->SR)  & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_SR_ITSF */
#if defined(RTC_ISR_ITSF)
#define __HAL_RTC_INTERNAL_TIMESTAMP_GET_FLAG(__HANDLE__, __FLAG__)    (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_ITSF */

/**
  * @brief  Clear the RTC Internal Time Stamp's pending flags.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC Internal Time Stamp Flag source to clear.
  *          This parameter can be:
  *             @arg RTC_FLAG_ITSF
  * @note   This flag must be cleared together with TSF flag.
  * @retval None
  */
#if defined(RTC_SCR_CITSF)
#define __HAL_RTC_INTERNAL_TIMESTAMP_CLEAR_FLAG(__HANDLE__, __FLAG__)  ((__HANDLE__)->Instance->SCR |= __FLAG__)
#endif /* RTC_SCR_CITSF */
#if defined(RTC_ISR_ITSF)
#define __HAL_RTC_INTERNAL_TIMESTAMP_CLEAR_FLAG(__HANDLE__, __FLAG__)  ((__HANDLE__)->Instance->ISR = (~((__FLAG__) | RTC_ISR_INIT)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT)))
#endif /* RTC_ISR_ITSF */

/**
  * @brief  Enable the RTC TimeStamp on Tamper detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(RTC_CR_TAMPTS)
#define __HAL_RTC_TAMPTS_ENABLE(__HANDLE__)                       ((__HANDLE__)->Instance->CR |= (RTC_TIMESTAMPONTAMPERDETECTION_MASK))
#endif /* RTC_CR_TAMPTS */
#if defined(RTC_TAMPCR_TAMPTS)
#define __HAL_RTC_TAMPTS_ENABLE(__HANDLE__)                       ((__HANDLE__)->Instance->TAMPCR |= (RTC_TIMESTAMPONTAMPERDETECTION_MASK))
#endif /* RTC_TAMPCR_TAMPTS */

/**
  * @brief  Disable the RTC TimeStamp on Tamper detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#if defined(RTC_CR_TAMPTS)
#define __HAL_RTC_TAMPTS_DISABLE(__HANDLE__)                      ((__HANDLE__)->Instance->CR &= ~(RTC_TIMESTAMPONTAMPERDETECTION_MASK))
#endif /* RTC_CR_TAMPTS */
#if defined(RTC_TAMPCR_TAMPTS)
#define __HAL_RTC_TAMPTS_DISABLE(__HANDLE__)                      ((__HANDLE__)->Instance->TAMPCR &= ~(RTC_TIMESTAMPONTAMPERDETECTION_MASK))
#endif /* RTC_TAMPCR_TAMPTS */

/**
  * @brief  Get activation status of the RTC TimeStamp on Tamper detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval Activation status of TimeStamp on Tamper detection
  *         This value can be:
  *            @arg RTC_TIMESTAMPONTAMPERDETECTION_ENABLE
  *            @arg RTC_TIMESTAMPONTAMPERDETECTION_DISABLE
  */
#if defined(RTC_CR_TAMPTS)
#define __HAL_RTC_TAMPTS_GET_STATUS(__HANDLE__)                   ((__HANDLE__)->Instance->CR &= RTC_TIMESTAMPONTAMPERDETECTION_MASK)
#endif /* RTC_CR_TAMPTS */
#if defined(RTC_TAMPCR_TAMPTS)
#define __HAL_RTC_TAMPTS_GET_STATUS(__HANDLE__)                   ((__HANDLE__)->Instance->TAMPCR &= RTC_TIMESTAMPONTAMPERDETECTION_MASK)
#endif /* RTC_TAMPCR_TAMPTS */

#if defined(RTC_CR_TAMPOE)
/**
  * @brief  Enable the RTC Tamper detection output.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TAMPOE_ENABLE(__HANDLE__)                       ((__HANDLE__)->Instance->CR |= (RTC_CR_TAMPOE))

/**
  * @brief  Disable the RTC Tamper detection output.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TAMPOE_DISABLE(__HANDLE__)                      ((__HANDLE__)->Instance->CR &= ~(RTC_CR_TAMPOE))
#endif /* RTC_CR_TAMPOE */

/* ========================================================================== */
/*         ##### Extended RTC Peripheral Control exported macros #####        */
/* ========================================================================== */

/**
  * @brief  Enable the RTC calibration output.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CALIBRATION_OUTPUT_ENABLE(__HANDLE__)                ((__HANDLE__)->Instance->CR |= (RTC_CR_COE))

/**
  * @brief  Disable the calibration output.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CALIBRATION_OUTPUT_DISABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR &= ~(RTC_CR_COE))

/**
  * @brief  Enable the clock reference detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CLOCKREF_DETECTION_ENABLE(__HANDLE__)                ((__HANDLE__)->Instance->CR |= (RTC_CR_REFCKON))

/**
  * @brief  Disable the clock reference detection.
  * @param  __HANDLE__ specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CLOCKREF_DETECTION_DISABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR &= ~(RTC_CR_REFCKON))

/**
  * @brief  Get the selected RTC shift operation's flag status.
  * @param  __HANDLE__ specifies the RTC handle.
  * @param  __FLAG__ specifies the RTC shift operation Flag is pending or not.
  *          This parameter can be:
  *             @arg RTC_FLAG_SHPF
  * @retval Flag status
  */
#if defined(RTC_ICSR_SHPF)
#define __HAL_RTC_SHIFT_GET_FLAG(__HANDLE__, __FLAG__)         (((((__HANDLE__)->Instance->ICSR) & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ICSR_SHPF */
#if defined(RTC_ISR_SHPF)
#define __HAL_RTC_SHIFT_GET_FLAG(__HANDLE__, __FLAG__)         (((((__HANDLE__)->Instance->ISR)  & (__FLAG__)) != 0U) ? 1U : 0U)
#endif /* RTC_ISR_SHPF */

/**
  * @}
  */

/* ========================================================================== */
/*               ##### RTC Wake-up Interrupt exported macros #####            */
/* ========================================================================== */

/**
  * @brief  Enable interrupt on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT()       (EXTI_D1->IMR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT()       (EXTI->IMR1    |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#endif /* EXTI_D1 */
/**
  * @brief  Disable interrupt on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT()      (EXTI_D1->IMR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT()      (EXTI->IMR1    &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#endif /* EXTI_D1 */

/**
  * @brief  Enable event on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT()    (EXTI_D1->EMR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT()    (EXTI->EMR1    |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief  Disable event on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_EVENT()   (EXTI_D1->EMR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_EVENT()   (EXTI->EMR1    &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))
#endif /* EXTI_D1 */

/**
  * @brief  Enable event on the RTC WakeUp Timer associated D3 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID3_ENABLE_EVENT()    (EXTI->D3PMR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief  Disable event on the RTC WakeUp Timer associated D3 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID3_DISABLE_EVENT()   (EXTI->D3PMR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))

#if defined(DUAL_CORE)
/**
  * @brief  Enable interrupt on the RTC WakeUp Timer associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_ENABLE_IT()       (EXTI_D2->IMR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief  Disable interrupt on the RTC WakeUp Timer associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_DISABLE_IT()      (EXTI_D2->IMR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))

/**
  * @brief  Enable event on the RTC WakeUp Timer associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_ENABLE_EVENT()    (EXTI_D2->EMR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief  Disable event on the RTC WakeUp Timer associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_DISABLE_EVENT()   (EXTI_D2->EMR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))

#endif /* DUAL_CORE */

/**
  * @brief  Enable falling edge trigger on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_FALLING_EDGE()   (EXTI->FTSR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief  Disable falling edge trigger on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_FALLING_EDGE()  (EXTI->FTSR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))

/**
  * @brief  Enable rising edge trigger on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_RISING_EDGE()    (EXTI->RTSR1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief  Disable rising edge trigger on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_RISING_EDGE()   (EXTI->RTSR1 &= ~(RTC_EXTI_LINE_WAKEUPTIMER_EVENT))

/**
  * @brief  Enable rising & falling edge trigger on the RTC WakeUp Timer associated Exti line.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_RISING_FALLING_EDGE()  do { \
                                                                   __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_RISING_EDGE();  \
                                                                   __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_FALLING_EDGE(); \
                                                                 } while(0)

/**
  * @brief  Disable rising & falling edge trigger on the RTC WakeUp Timer associated Exti line.
  * This parameter can be:
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_RISING_FALLING_EDGE()  do { \
                                                                   __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_RISING_EDGE();  \
                                                                   __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_FALLING_EDGE(); \
                                                                  } while(0)

/**
  * @brief Check whether the RTC WakeUp Timer associated Exti line interrupt flag is set or not.
  * @retval Line Status.
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_GET_FLAG()              (EXTI_D1->PR1 & RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_GET_FLAG()              (EXTI->PR1    & RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief Clear the RTC WakeUp Timer associated Exti line flag.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG()            (EXTI_D1->PR1 = RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#else  /* EXTI */
#define __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG()            (EXTI->PR1    = RTC_EXTI_LINE_WAKEUPTIMER_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief Check whether the RTC WakeUp Timer associated D3 Exti line interrupt flag is set or not.
  * @retval Line Status
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID3_GET_FLAG()            (EXTI_D3->PR1 & RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief Clear the RTC WakeUp Timer associated D3 Exti line flag.
  * @retval None.
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID3_CLEAR_FLAG()          (EXTI_D3->PR1 = RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief Generate a Software interrupt on the RTC WakeUp Timer associated Exti line.
  * @retval None.
  */
#define __HAL_RTC_WAKEUPTIMER_EXTI_GENERATE_SWIT()         (EXTI->SWIER1 |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

#if defined(DUAL_CORE)

/**
  * @brief Check whether the RTC WakeUp Timer associated D2 Exti line interrupt flag is set or not.
  * @retval Line Status.
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_GET_FLAG()            (EXTI_D2->PR1 & RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief Clear the RTC WakeUp Timer associated D2 Exti line flag.
  * @retval None.
  */
#define __HAL_RTC_WAKEUPTIMER_EXTID2_CLEAR_FLAG()          (EXTI_D2->PR1 = RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

#endif /* DUAL_CORE */

/* ========================================================================== */
/*       ##### RTC TimeStamp and Tamper Interrupt exported macros #####       */
/* ========================================================================== */

/**
  * @brief  Enable interrupt on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT()        (EXTI_D1->IMR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_IT()        (EXTI->IMR1    |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief  Disable interrupt on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_IT()       (EXTI_D1->IMR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_IT()       (EXTI->IMR1    &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))
#endif /* EXTI_D1 */

/**
  * @brief  Enable event on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_EVENT()    (EXTI_D1->EMR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_EVENT()    (EXTI->EMR1    |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief  Disable event on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_EVENT()   (EXTI_D1->EMR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_EVENT()   (EXTI->EMR1    &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))
#endif /* EXTI_D1 */

/**
  * @}
  */

#if defined(DUAL_CORE)
/**
  * @brief  Enable interrupt on the RTC Tamper and Timestamp associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_ENABLE_IT()      (EXTI_D2->IMR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)

/**
  * @brief  Disable interrupt on the RTC Tamper and Timestamp associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_DISABLE_IT()     (EXTI_D2->IMR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))


/**
  * @brief  Enable event on the RTC Tamper and Timestamp associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_ENABLE_EVENT()  (EXTI_D2->EMR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)


/**
  * @brief  Disable event on the RTC Tamper and Timestamp associated D2 Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_DISABLE_EVENT() (EXTI_D2->EMR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))

#endif /* DUAL_CORE */

/**
  * @brief  Enable falling edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_FALLING_EDGE()   (EXTI->FTSR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)

/**
  * @brief  Disable falling edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_FALLING_EDGE()  (EXTI->FTSR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))

/**
  * @brief  Enable rising edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_EDGE()    (EXTI->RTSR1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)

/**
  * @brief  Disable rising edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_RISING_EDGE()   (EXTI->RTSR1 &= ~(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT))

/**
  * @brief  Enable rising & falling edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_FALLING_EDGE()  do { \
                                                                        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_RISING_EDGE();  \
                                                                        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_ENABLE_FALLING_EDGE(); \
                                                                      } while(0)

/**
  * @brief  Disable rising & falling edge trigger on the RTC Tamper and Timestamp associated Exti line.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_RISING_FALLING_EDGE()  do { \
                                                                        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_RISING_EDGE();  \
                                                                        __HAL_RTC_TAMPER_TIMESTAMP_EXTI_DISABLE_FALLING_EDGE(); \
                                                                       } while(0)

/**
  * @brief Check whether the RTC Tamper and Timestamp associated Exti line interrupt flag is set or not.
  * @retval Line Status
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_GET_FLAG()         (EXTI_D1->PR1 & RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_GET_FLAG()         (EXTI->PR1    & RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#endif /* EXTI_D1 */

/**
  * @brief Clear the RTC Tamper and Timestamp associated Exti line flag.
  * @retval None
  */
#if defined(EXTI_D1)
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG()       (EXTI_D1->PR1 = RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#else  /* EXTI */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_CLEAR_FLAG()       (EXTI->PR1    = RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)
#endif /* EXTI_D1 */

#if defined(DUAL_CORE)
/**
  * @brief Check whether the RTC Tamper and Timestamp associated D2 Exti line interrupt flag is set or not.
  * @retval Line Status
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_GET_FLAG()       (EXTI_D2->PR1 & RTC_EXTI_LINE_WAKEUPTIMER_EVENT)

/**
  * @brief Clear the RTC Tamper and Timestamp associated D2 Exti line flag.
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTID2_CLEAR_FLAG()     (EXTI_D2->PR1 = RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)

#endif  /* DUAL_CORE */

/**
  * @brief Generate a Software interrupt on the RTC Tamper and Timestamp associated Exti line
  * @retval None
  */
#define __HAL_RTC_TAMPER_TIMESTAMP_EXTI_GENERATE_SWIT()    (EXTI->SWIER1 |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT)

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup RTCEx_Exported_Functions RTC Exported Functions
  * @{
  */

/* ========================================================================== */
/*                  ##### RTC TimeStamp exported functions #####              */
/* ========================================================================== */

/* RTC TimeStamp functions ****************************************************/

/** @defgroup RTCEx_Exported_Functions_Group1 Extended RTC TimeStamp functions
  * @{
  */
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetInternalTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTimeStamp, RTC_DateTypeDef *sTimeStampDate, uint32_t Format);
void              HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
/**
  * @}
  */

/* ========================================================================== */
/*                    ##### RTC Tamper exported functions #####               */
/* ========================================================================== */

/* RTC Tamper functions *******************************************************/

/** @defgroup RTCEx_Exported_Functions_Group5 Extended RTC Tamper functions
  * @{
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc, uint32_t Tamper);
#if defined(RTC_TAMPER_1)
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
#endif /* RTC_TAMPER_1 */
#if defined(RTC_TAMPER_2)
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
#endif /* RTC_TAMPER_2 */
#if defined(RTC_TAMPER_3)
HAL_StatusTypeDef HAL_RTCEx_PollForTamper3Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
#endif /* RTC_TAMPER_3 */
#if defined(RTC_TAMPER_1)
void              HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc);
#endif /* RTC_TAMPER_1 */
#if defined(RTC_TAMPER_2)
void              HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc);
#endif /* RTC_TAMPER_2 */
#if defined(RTC_TAMPER_3)
void              HAL_RTCEx_Tamper3EventCallback(RTC_HandleTypeDef *hrtc);
#endif /* RTC_TAMPER_3 */
#if defined(TAMP_CR1_ITAMP1E)
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper(RTC_HandleTypeDef *hrtc, RTC_InternalTamperTypeDef *sIntTamper);
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper_IT(RTC_HandleTypeDef *hrtc, RTC_InternalTamperTypeDef *sIntTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTamper(RTC_HandleTypeDef *hrtc, uint32_t IntTamper);
HAL_StatusTypeDef HAL_RTCEx_PollForInternalTamperEvent(RTC_HandleTypeDef *hrtc, uint32_t IntTamper, uint32_t Timeout);
#endif /* TAMP_CR1_ITAMP1E */
void              HAL_RTCEx_InternalTamper1EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper2EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper3EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper4EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper5EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper6EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper8EventCallback(RTC_HandleTypeDef *hrtc);
#if defined(TAMP_ATCR1_TAMP1AM)
HAL_StatusTypeDef HAL_RTCEx_SetActiveTampers(RTC_HandleTypeDef *hrtc, RTC_ActiveTampersTypeDef *sAllTamper);
#endif /* TAMP_ATCR1_TAMP1AM */
#if defined(TAMP_ATSEEDR_SEED)
HAL_StatusTypeDef HAL_RTCEx_SetActiveSeed(RTC_HandleTypeDef *hrtc, uint32_t *pSeed);
#endif /* TAMP_ATSEEDR_SEED */
#if defined(TAMP_ATCR1_TAMP1AM)
HAL_StatusTypeDef HAL_RTCEx_DeactivateActiveTampers(RTC_HandleTypeDef *hrtc);
#endif /* TAMP_ATCR1_TAMP1AM */

/* ========================================================================== */
/*                   ##### RTC Wake-up exported functions #####               */
/* ========================================================================== */

/* RTC Wake-up functions ******************************************************/

/** @defgroup RTCEx_Exported_Functions_Group2 Extended RTC Wake-up functions
  * @{
  */
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
/**
  * @}
  */

/* ========================================================================== */
/*         ##### Extended RTC Backup registers exported functions #####       */
/* ========================================================================== */

/* Extended RTC Backup registers functions ************************************/

/** @defgroup RTCEx_Exported_Functions_Group6 Extended RTC Backup register functions
  * @{
  */
void     HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data);
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);
/**
  * @}
  */

/* ========================================================================== */
/*        ##### Extended RTC Peripheral Control exported functions #####      */
/* ========================================================================== */

/* Extended RTC Peripheral Control functions **********************************/

/** @defgroup RTCEx_Exported_Functions_Group3 Extended Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmoothCalibMinusPulsesValue);
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc, uint32_t ShiftAdd1S, uint32_t ShiftSubFS);
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc, uint32_t CalibOutput);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc);
#if defined(TAMP_COUNTR)
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterIncrement(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterGet(RTC_HandleTypeDef *hrtc, uint32_t *Counter);
#endif /* TAMP_COUNTR */
/**
  * @}
  */

/* Extended RTC features functions *******************************************/

/** @defgroup RTCEx_Exported_Functions_Group4 Extended features functions
  * @{
  */

void              HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup RTCEx_Private_Constants RTC Private Constants
  * @{
  */
#define RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT  EXTI_IMR1_IM18  /*!< External interrupt line 18 Connected to the RTC Tamper and Time Stamp events */
#define RTC_EXTI_LINE_WAKEUPTIMER_EVENT       EXTI_IMR1_IM19  /*!< External interrupt line 19 Connected to the RTC Wakeup event */

/* Masks Definition */
#define RTC_TAMPER_X             ((uint32_t) (RTC_TAMPER_1 | RTC_TAMPER_2 | RTC_TAMPER_3))
#define RTC_TAMPER_X_INTERRUPT   ((uint32_t) (RTC_IT_TAMP1 | RTC_IT_TAMP2 | RTC_IT_TAMP3))

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup RTCEx_Private_Macros RTC Private Macros
  * @{
  */

/** @defgroup RTCEx_IS_RTC_Definitions Private macros to check input parameters
  * @{
  */

/* ========================================================================== */
/*          ##### Extended RTC Backup registers private macros #####          */
/* ========================================================================== */

#define IS_RTC_BKP(__BKP__)                   ((__BKP__) < BKP_REG_NUMBER)

/* ========================================================================== */
/*                  ##### RTC TimeStamp private macros #####                  */
/* ========================================================================== */

#define IS_TIMESTAMP_EDGE(EDGE) (((EDGE) == RTC_TIMESTAMPEDGE_RISING) || \
                                 ((EDGE) == RTC_TIMESTAMPEDGE_FALLING))

#define IS_RTC_TIMESTAMP_PIN(PIN) (((PIN) == RTC_TIMESTAMPPIN_DEFAULT))

/* ========================================================================== */
/*                    ##### RTC Wake-up private macros #####                  */
/* ========================================================================== */

#define IS_RTC_WAKEUP_CLOCK(CLOCK) (((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV16)       || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV8)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV4)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV2)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_CK_SPRE_16BITS) || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_CK_SPRE_17BITS))

#define IS_RTC_WAKEUP_COUNTER(COUNTER)  ((COUNTER) <= RTC_WUTR_WUT)

/* ========================================================================== */
/*               ##### RTC Smooth Calibration private macros #####            */
/* ========================================================================== */

#define IS_RTC_SMOOTH_CALIB_PERIOD(PERIOD) (((PERIOD) == RTC_SMOOTHCALIB_PERIOD_32SEC) || \
                                            ((PERIOD) == RTC_SMOOTHCALIB_PERIOD_16SEC) || \
                                            ((PERIOD) == RTC_SMOOTHCALIB_PERIOD_8SEC))

#define IS_RTC_SMOOTH_CALIB_PLUS(PLUS) (((PLUS) == RTC_SMOOTHCALIB_PLUSPULSES_SET) || \
                                        ((PLUS) == RTC_SMOOTHCALIB_PLUSPULSES_RESET))

#define  IS_RTC_SMOOTH_CALIB_MINUS(VALUE) ((VALUE) <= RTC_CALR_CALM)

/* ========================================================================== */
/*          ##### Extended RTC Peripheral Control private macros #####        */
/* ========================================================================== */

#define IS_RTC_SHIFT_ADD1S(SEL) (((SEL) == RTC_SHIFTADD1S_RESET) || \
                                 ((SEL) == RTC_SHIFTADD1S_SET))

#define IS_RTC_SHIFT_SUBFS(FS) ((FS) <= RTC_SHIFTR_SUBFS)

#define IS_RTC_CALIB_OUTPUT(OUTPUT)  (((OUTPUT) == RTC_CALIBOUTPUT_512HZ) || \
                                      ((OUTPUT) == RTC_CALIBOUTPUT_1HZ))

/* ========================================================================== */
/*                    ##### RTC Tamper private macros #####                   */
/* ========================================================================== */

#define IS_RTC_TAMPER(__TAMPER__)  ((((__TAMPER__) &  RTC_TAMPER_X) != 0x00U) &&  \
                                    (((__TAMPER__) & ~RTC_TAMPER_X) == 0x00U))

#if defined(RTC_ALL_TAMPER_INTERRUPT)
#define IS_RTC_TAMPER_INTERRUPT(__INTERRUPT__)                                                           \
              ((((__INTERRUPT__) & (  RTC_TAMPER_X_INTERRUPT | RTC_ALL_TAMPER_INTERRUPT )) != 0x00U) &&  \
               (((__INTERRUPT__) & (~(RTC_TAMPER_X_INTERRUPT | RTC_ALL_TAMPER_INTERRUPT))) == 0x00U))
#else  /* RTC_ALL_TAMPER_INTERRUPT not defined */
#define IS_RTC_TAMPER_INTERRUPT(__INTERRUPT__)                              \
              ((((__INTERRUPT__) &   RTC_TAMPER_X_INTERRUPT ) != 0x00U) &&  \
               (((__INTERRUPT__) & (~RTC_TAMPER_X_INTERRUPT)) == 0x00U))
#endif /* RTC_ALL_TAMPER_INTERRUPT */

#define IS_RTC_TAMPER_TRIGGER(__TRIGGER__) (((__TRIGGER__) == RTC_TAMPERTRIGGER_RISINGEDGE)    || \
                                            ((__TRIGGER__) == RTC_TAMPERTRIGGER_FALLINGEDGE)   || \
                                            ((__TRIGGER__) == RTC_TAMPERTRIGGER_LOWLEVEL)      || \
                                            ((__TRIGGER__) == RTC_TAMPERTRIGGER_HIGHLEVEL))

#define IS_RTC_TAMPER_ERASE_MODE(__MODE__)             (((__MODE__) == RTC_TAMPER_ERASE_BACKUP_ENABLE) || \
                                                        ((__MODE__) == RTC_TAMPER_ERASE_BACKUP_DISABLE))

#define IS_RTC_TAMPER_MASKFLAG_STATE(__STATE__)        (((__STATE__) == RTC_TAMPERMASK_FLAG_ENABLE) || \
                                                        ((__STATE__) == RTC_TAMPERMASK_FLAG_DISABLE))

#define IS_RTC_TAMPER_FILTER(__FILTER__)  (((__FILTER__) == RTC_TAMPERFILTER_DISABLE) || \
                                           ((__FILTER__) == RTC_TAMPERFILTER_2SAMPLE) || \
                                           ((__FILTER__) == RTC_TAMPERFILTER_4SAMPLE) || \
                                           ((__FILTER__) == RTC_TAMPERFILTER_8SAMPLE))

#define IS_RTC_TAMPER_SAMPLING_FREQ(__FREQ__) (((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768)|| \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384)|| \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192) || \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096) || \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048) || \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024) || \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512)  || \
                                               ((__FREQ__) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256))

#define IS_RTC_TAMPER_PRECHARGE_DURATION(__DURATION__) (((__DURATION__) == RTC_TAMPERPRECHARGEDURATION_1RTCCLK) || \
                                                        ((__DURATION__) == RTC_TAMPERPRECHARGEDURATION_2RTCCLK) || \
                                                        ((__DURATION__) == RTC_TAMPERPRECHARGEDURATION_4RTCCLK) || \
                                                        ((__DURATION__) == RTC_TAMPERPRECHARGEDURATION_8RTCCLK))

#define IS_RTC_TAMPER_PULLUP_STATE(__STATE__) (((__STATE__) == RTC_TAMPER_PULLUP_ENABLE) || \
                                               ((__STATE__) == RTC_TAMPER_PULLUP_DISABLE))

#define IS_RTC_TAMPER_TIMESTAMPONTAMPER_DETECTION(__DETECTION__) (((__DETECTION__) == RTC_TIMESTAMPONTAMPERDETECTION_ENABLE) || \
                                                                  ((__DETECTION__) == RTC_TIMESTAMPONTAMPERDETECTION_DISABLE))

#if defined(RTC_CR_TAMPOE)
#define IS_RTC_TAMPER_TAMPERDETECTIONOUTPUT(__MODE__) (((__MODE__) == RTC_TAMPERDETECTIONOUTPUT_ENABLE) || \
                                                       ((__MODE__) == RTC_TAMPERDETECTIONOUTPUT_DISABLE))
#endif /* RTC_CR_TAMPOE */

#define IS_RTC_TAMPER_FILTER_CONFIG_CORRECT(FILTER, TRIGGER)            \
                  (  (  ((FILTER) != RTC_TAMPERFILTER_DISABLE)          \
                     && (  ((TRIGGER) == RTC_TAMPERTRIGGER_LOWLEVEL)    \
                        || ((TRIGGER) == RTC_TAMPERTRIGGER_HIGHLEVEL))) \
                  || (  ((FILTER) == RTC_TAMPERFILTER_DISABLE)          \
                     && (  ((TRIGGER) == RTC_TAMPERTRIGGER_RISINGEDGE)  \
                        || ((TRIGGER) == RTC_TAMPERTRIGGER_FALLINGEDGE))))

#define IS_RTC_INTERNAL_TAMPER(__INT_TAMPER__)   ((((__INT_TAMPER__) &  RTC_INT_TAMPER_ALL) != 0x00U) && \
                                                  (((__INT_TAMPER__) & ~RTC_INT_TAMPER_ALL) == 0x00U))

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
#endif /* __cplusplus */

#endif /* STM32H7xx_HAL_RTC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
