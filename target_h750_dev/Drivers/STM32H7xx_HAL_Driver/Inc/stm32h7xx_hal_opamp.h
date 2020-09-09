/**
  ******************************************************************************
  * @file    stm32h7xx_hal_opamp.h
  * @author  MCD Application Team
  * @brief   Header file of OPAMP HAL module.
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
#ifndef STM32H7xx_HAL_OPAMP_H
#define STM32H7xx_HAL_OPAMP_H

#ifdef __cplusplus
 extern "C" {
#endif
                  
/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup OPAMP
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** @defgroup OPAMP_Exported_Types OPAMP Exported Types
  * @{
  */

/** 
  * @brief  OPAMP Init structure definition  
  */
  
typedef struct
{

  uint32_t PowerMode;                   /*!< Specifies the power mode Normal or High Speed.
                                             This parameter must be a value of @ref OPAMP_PowerMode */

  uint32_t Mode;                        /*!< Specifies the OPAMP mode
                                             This parameter must be a value of @ref OPAMP_Mode 
                                             mode is either Standalone, - Follower or PGA */

  uint32_t InvertingInput;              /*!< Specifies the inverting input in Standalone & PGA modes
                                             - In Standalone mode i.e when mode is OPAMP_STANDALONE_MODE
                                                 This parameter must be a value of @ref OPAMP_InvertingInput 
                                             - In Follower mode i.e when mode is OPAMP_FOLLOWER_MODE
                                           & In PGA mode i.e when mode is OPAMP_PGA_MODE 
                                                 This parameter is Not Applicable */

  uint32_t NonInvertingInput;           /*!< Specifies the non inverting input of the opamp:
                                             This parameter must be a value of @ref OPAMP_NonInvertingInput */

  uint32_t PgaGain;                     /*!< Specifies the gain in PGA mode 
                                             i.e. when mode is OPAMP_PGA_MODE. 
                                             This parameter must be a value of @ref OPAMP_PgaGain */

  uint32_t PgaConnect;                  /*!< Specifies the inverting pin in PGA mode 
                                             i.e. when mode is OPAMP_PGA_MODE 
                                             This parameter must be a value of @ref OPAMP_PgaConnect 
                                             Either: not connected, connected to VINM0, connected to VINM1
                                             (VINM0 or VINM1 are typically used for external filtering) */

  uint32_t UserTrimming;                /*!< Specifies the trimming mode 
                                             This parameter must be a value of @ref OPAMP_UserTrimming 
                                             UserTrimming is either factory or user trimming.*/

  uint32_t TrimmingValueP;              /*!< Specifies the offset trimming value (PMOS) in Normal Mode
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31.
                                             16 is typical default value */

  uint32_t TrimmingValueN;              /*!< Specifies the offset trimming value (NMOS) in Normal Mode
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31.
                                             16 is typical default value */

  uint32_t TrimmingValuePHighSpeed;      /*!< Specifies the offset trimming value (PMOS) in High Speed Mode
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER.
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31.
                                             16 is typical default value */

  uint32_t TrimmingValueNHighSpeed;      /*!< Specifies the offset trimming value (NMOS) in High Speed Mode
                                             i.e. when UserTrimming is OPAMP_TRIMMING_USER. 
                                             This parameter must be a number between Min_Data = 0 and Max_Data = 31.
                                             16 is typical default value */

}OPAMP_InitTypeDef;

/** 
  * @brief  HAL State structures definition  
  */ 

typedef enum
{
  HAL_OPAMP_STATE_RESET               = 0x00000000U, /*!< OPAMP is not yet Initialized          */

  HAL_OPAMP_STATE_READY               = 0x00000001U, /*!< OPAMP is initialized and ready for use */
  HAL_OPAMP_STATE_CALIBBUSY           = 0x00000002U, /*!< OPAMP is enabled in auto calibration mode */

  HAL_OPAMP_STATE_BUSY                = 0x00000004U, /*!< OPAMP is enabled and running in normal mode */
  HAL_OPAMP_STATE_BUSYLOCKED          = 0x00000005U  /*!< OPAMP is locked
                                                         only system reset allows reconfiguring the opamp. */

}HAL_OPAMP_StateTypeDef;

/** 
  * @brief OPAMP Handle Structure definition
  */ 
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
typedef struct __OPAMP_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_OPAMP_REGISTER_CALLBACKS */
{
  OPAMP_TypeDef                 *Instance;                    /*!< OPAMP instance's registers base address */
  OPAMP_InitTypeDef              Init;                         /*!< OPAMP required parameters */
  HAL_StatusTypeDef              Status;                       /*!< OPAMP peripheral status   */
  HAL_LockTypeDef                Lock;                         /*!< Locking object          */
  __IO HAL_OPAMP_StateTypeDef    State;                        /*!< OPAMP communication state */

#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
void (* MspInitCallback)                (struct __OPAMP_HandleTypeDef *hopamp);
void (* MspDeInitCallback)              (struct __OPAMP_HandleTypeDef *hopamp); 
#endif /* USE_HAL_OPAMP_REGISTER_CALLBACKS */ 
} OPAMP_HandleTypeDef;

/** 
  * @brief HAl_OPAMP_TrimmingValueTypeDef definition 
  */ 

typedef  uint32_t HAL_OPAMP_TrimmingValueTypeDef;

/**
  * @}
  */

#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL OPAMP Callback ID enumeration definition
  */
typedef enum
{
  HAL_OPAMP_MSPINIT_CB_ID                     = 0x01U,  /*!< OPAMP MspInit Callback ID           */
  HAL_OPAMP_MSPDEINIT_CB_ID                   = 0x02U,  /*!< OPAMP MspDeInit Callback ID         */
  HAL_OPAMP_ALL_CB_ID                         = 0x03U   /*!< OPAMP All ID                        */
}HAL_OPAMP_CallbackIDTypeDef;

/**
  * @brief  HAL OPAMP Callback pointer definition
  */
typedef void (*pOPAMP_CallbackTypeDef)(OPAMP_HandleTypeDef *hopamp);
#endif /* USE_HAL_OPAMP_REGISTER_CALLBACKS */
    
    
/* Exported constants --------------------------------------------------------*/
/** @defgroup OPAMP_Exported_Constants OPAMP Exported Constants
  * @{
  */

/** @defgroup OPAMP_Mode OPAMP Mode
  * @{
  */
#define OPAMP_STANDALONE_MODE             0x00000000U                                  /*!< standalone mode */
#define OPAMP_PGA_MODE                    OPAMP_CSR_VMSEL_1                            /*!< PGA mode */
#define OPAMP_FOLLOWER_MODE               (OPAMP_CSR_VMSEL_1 | OPAMP_CSR_VMSEL_0)      /*!< follower mode */
    
/**
  * @}
  */

/** @defgroup OPAMP_NonInvertingInput OPAMP Non Inverting Input
  * @{
  */

#define OPAMP_NONINVERTINGINPUT_IO0         0x00000000U                /*!< OPAMP non-inverting input connected to dedicated IO pin */
#define OPAMP_NONINVERTINGINPUT_DAC_CH      OPAMP_CSR_VPSEL_0          /*!< OPAMP non-inverting input connected internally to DAC channel */
#if defined(DAC2)
#define OPAMP_NONINVERTINGINPUT_DAC2_CH     OPAMP_CSR_VPSEL_1         /*!< Only OPAMP2 non-inverting input connected internally to DAC2 channel */
#endif /* DAC2 */

/**
  * @}
  */

/** @defgroup OPAMP_InvertingInput OPAMP Inverting Input
  * @{
  */

#define OPAMP_INVERTINGINPUT_IO0               0x00000000U                            /*!< OPAMP inverting input connected to dedicated IO pin */
#define OPAMP_INVERTINGINPUT_IO1               OPAMP_CSR_VMSEL_0                      /*!< OPAMP inverting input connected to dedicated IO pin */

/**
  * @}
  */

/** @defgroup OPAMP_PgaConnect OPAMP Pga Connect
  * @{
  */

#define OPAMP_PGA_CONNECT_INVERTINGINPUT_NO                0x00000000U                                /*!< In PGA mode, the inverting input is not connected */
#define OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0               OPAMP_CSR_PGGAIN_2                         /*!< In PGA mode, the inverting input is connected to VINM0 */
#define OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS          OPAMP_CSR_PGGAIN_3                         /*!< In PGA mode, the inverting input is connected to VINM0 or bias */
#define OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_IO1_BIAS      (OPAMP_CSR_PGGAIN_2 | OPAMP_CSR_PGGAIN_3)  /*!< In PGA mode, the inverting input is connected to VINM0 or bias , VINM1 connected for filtering */


/**
  * @}
  */

/** @defgroup OPAMP_PgaGain OPAMP Pga Gain
  * @{
  */

#define OPAMP_PGA_GAIN_2_OR_MINUS_1               0x00000000U                                 /*!< PGA gain could be 2 or -1   */
#define OPAMP_PGA_GAIN_4_OR_MINUS_3               OPAMP_CSR_PGGAIN_0                          /*!< PGA gain could be 4 or -3   */
#define OPAMP_PGA_GAIN_8_OR_MINUS_7               OPAMP_CSR_PGGAIN_1                          /*!< PGA gain could be 8 or -7   */
#define OPAMP_PGA_GAIN_16_OR_MINUS_15             (OPAMP_CSR_PGGAIN_0 | OPAMP_CSR_PGGAIN_1)   /*!< PGA gain could be 16 or -15 */

/**
  * @}
  */

/** @defgroup OPAMP_PowerMode OPAMP PowerMode
  * @{
  */
#define OPAMP_POWERMODE_NORMAL         0x00000000U
#define OPAMP_POWERMODE_HIGHSPEED      OPAMP_CSR_OPAHSM

/**
  * @}
  */


/** @defgroup OPAMP_VREF OPAMP VREF
  * @{
  */

#define OPAMP_VREF_3VDDA                     0x00000000U                 /*!< OPAMP Vref = 3.3% VDDA */
#define OPAMP_VREF_10VDDA                    OPAMP_CSR_CALSEL_0          /*!< OPAMP Vref = 10% VDDA  */
#define OPAMP_VREF_50VDDA                    OPAMP_CSR_CALSEL_1          /*!< OPAMP Vref = 50% VDDA  */
#define OPAMP_VREF_90VDDA                    OPAMP_CSR_CALSEL            /*!< OPAMP Vref = 90% VDDA  */

/**
  * @}
  */ 

/** @defgroup OPAMP_UserTrimming OPAMP User Trimming
  * @{
  */
#define OPAMP_TRIMMING_FACTORY         0x00000000U                             /*!< Factory trimming */
#define OPAMP_TRIMMING_USER            OPAMP_CSR_USERTRIM                      /*!< User trimming */


/**
  * @}
  */

/** @defgroup OPAMP_FactoryTrimming OPAMP Factory Trimming
  * @{
  */
#define OPAMP_FACTORYTRIMMING_DUMMY    0xFFFFFFFFU                          /*!< Dummy value if trimming value could not be retrieved */

#define OPAMP_FACTORYTRIMMING_N        0x00000000U                          /*!< Offset trimming N */
#define OPAMP_FACTORYTRIMMING_P        0x00000001U                          /*!< Offset trimming P */

/**
  * @}
  */

 /**
  * @}
  */ 

/* Private constants ---------------------------------------------------------*/
/** @defgroup OPAMP_Private_Constants OPAMP Private Constants
  * @brief   OPAMP Private constants and defines
  * @{
  */

/* NONINVERTING bit position in OTR & HSOTR */
#define OPAMP_INPUT_NONINVERTING           (8U)  /*!< Non inverting input */  

/* Offset trimming time: during calibration, minimum time needed between two  */
/* steps to have 1 mV accuracy.                                               */
/* Refer to datasheet, electrical characteristics: parameter tOFFTRIM Typ=2ms.*/
/* Unit: ms.                                                                  */
#define OPAMP_TRIMMING_DELAY               (2U)

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup OPAMP_Exported_Macros OPAMP Exported Macros
  * @{
  */

/** @brief Reset OPAMP handle state.
  * @param  __HANDLE__: OPAMP handle.
  * @retval None
  */
#define __HAL_OPAMP_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_OPAMP_STATE_RESET)

/**
  * @}
  */ 

/* Private macro -------------------------------------------------------------*/

/** @defgroup OPAMP_Private_Macros OPAMP Private Macros
  * @{
  */

#define IS_OPAMP_FUNCTIONAL_NORMALMODE(INPUT) (((INPUT) == OPAMP_STANDALONE_MODE) || \
                                               ((INPUT) == OPAMP_PGA_MODE) || \
                                               ((INPUT) == OPAMP_FOLLOWER_MODE))

#define IS_OPAMP_INVERTING_INPUT_STANDALONE(INPUT) (((INPUT) == OPAMP_INVERTINGINPUT_IO0) || \
                                                    ((INPUT) == OPAMP_INVERTINGINPUT_IO1))

#if defined(DAC2)
#define IS_OPAMP_NONINVERTING_INPUT(INPUT) (((INPUT) == OPAMP_NONINVERTINGINPUT_IO0) || \
                                            ((INPUT) == OPAMP_NONINVERTINGINPUT_DAC_CH) || \
                                            ((INPUT) == OPAMP_NONINVERTINGINPUT_DAC2_CH))
#else 
#define IS_OPAMP_NONINVERTING_INPUT(INPUT) (((INPUT) == OPAMP_NONINVERTINGINPUT_IO0) || \
                                            ((INPUT) == OPAMP_NONINVERTINGINPUT_DAC_CH))
#endif /* DAC2 */

#define IS_OPAMP_PGACONNECT(CONNECT) (((CONNECT) == OPAMP_PGA_CONNECT_INVERTINGINPUT_NO)  || \
                                      ((CONNECT) == OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0) || \
                                      ((CONNECT) == OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS) || \
                                      ((CONNECT) == OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_IO1_BIAS))

#define IS_OPAMP_PGA_GAIN(GAIN) (((GAIN) == OPAMP_PGA_GAIN_2_OR_MINUS_1) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_4_OR_MINUS_3) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_8_OR_MINUS_7) || \
                                 ((GAIN) == OPAMP_PGA_GAIN_16_OR_MINUS_15))


#define IS_OPAMP_VREF(VREF) (((VREF) == OPAMP_VREF_3VDDA)  || \
                             ((VREF) == OPAMP_VREF_10VDDA) || \
                             ((VREF) == OPAMP_VREF_50VDDA) || \
                             ((VREF) == OPAMP_VREF_90VDDA))

#define IS_OPAMP_POWERMODE(TRIMMING) (((TRIMMING) == OPAMP_POWERMODE_NORMAL) || \
                                      ((TRIMMING) == OPAMP_POWERMODE_HIGHSPEED) )


#define IS_OPAMP_TRIMMING(TRIMMING) (((TRIMMING) == OPAMP_TRIMMING_FACTORY) || \
                                     ((TRIMMING) == OPAMP_TRIMMING_USER))


#define IS_OPAMP_TRIMMINGVALUE(TRIMMINGVALUE) ((TRIMMINGVALUE) <= 0x1FU)

#define IS_OPAMP_FACTORYTRIMMING(TRIMMING) (((TRIMMING) == OPAMP_FACTORYTRIMMING_N) || \
                                             ((TRIMMING) == OPAMP_FACTORYTRIMMING_P))

/**
  * @}
  */ 

/* Include OPAMP HAL Extended module */
#include "stm32h7xx_hal_opamp_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup OPAMP_Exported_Functions
  * @{
  */

/** @addtogroup OPAMP_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_OPAMP_Init(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_DeInit (OPAMP_HandleTypeDef *hopamp);
void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef *hopamp);
void HAL_OPAMP_MspDeInit(OPAMP_HandleTypeDef *hopamp);
/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group2
  * @{
  */

/* I/O operation functions  *****************************************************/
HAL_StatusTypeDef HAL_OPAMP_Start(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_Stop(OPAMP_HandleTypeDef *hopamp);
HAL_StatusTypeDef HAL_OPAMP_SelfCalibrate(OPAMP_HandleTypeDef *hopamp); 

/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group3
  * @{
  */

/* Peripheral Control functions  ************************************************/
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
/* OPAMP callback registering/unregistering */
HAL_StatusTypeDef HAL_OPAMP_RegisterCallback (OPAMP_HandleTypeDef *hopamp, HAL_OPAMP_CallbackIDTypeDef CallbackId, pOPAMP_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_OPAMP_UnRegisterCallback (OPAMP_HandleTypeDef *hopamp, HAL_OPAMP_CallbackIDTypeDef CallbackId);
#endif /* USE_HAL_OPAMP_REGISTER_CALLBACKS */
HAL_StatusTypeDef HAL_OPAMP_Lock(OPAMP_HandleTypeDef *hopamp);
HAL_OPAMP_TrimmingValueTypeDef HAL_OPAMP_GetTrimOffset (OPAMP_HandleTypeDef *hopamp, uint32_t trimmingoffset);

/**
  * @}
  */

/** @addtogroup OPAMP_Exported_Functions_Group4
  * @{
  */

/* Peripheral State functions  **************************************************/
HAL_OPAMP_StateTypeDef HAL_OPAMP_GetState(OPAMP_HandleTypeDef *hopamp);

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

#endif /* STM32H7xx_HAL_OPAMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
