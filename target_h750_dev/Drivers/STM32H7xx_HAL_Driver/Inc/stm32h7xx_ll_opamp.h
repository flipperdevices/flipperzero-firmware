/**
  ******************************************************************************
  * @file    stm32h7xx_ll_opamp.h
  * @author  MCD Application Team
  * @brief   Header file of OPAMP LL module.
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
#ifndef __STM32H7xx_LL_OPAMP_H
#define __STM32H7xx_LL_OPAMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx.h"

/** @addtogroup STM32H7xx_LL_Driver
  * @{
  */

#if defined (OPAMP1) || defined (OPAMP2)

/** @defgroup OPAMP_LL OPAMP
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup OPAMP_LL_Private_Constants OPAMP Private Constants
  * @{
  */

/* Internal mask for OPAMP power mode:                                        */
/* To select into literal LL_OPAMP_POWERMODE_x the relevant bits for:         */
/* - OPAMP power mode into control register                                   */
/* - OPAMP trimming register offset                                           */

/* Internal register offset for OPAMP trimming configuration */
#define OPAMP_POWERMODE_OTR_REGOFFSET       0x00000000U
#define OPAMP_POWERMODE_HSOTR_REGOFFSET     0x00000001U
#define OPAMP_POWERMODE_OTR_REGOFFSET_MASK  (OPAMP_POWERMODE_OTR_REGOFFSET | OPAMP_POWERMODE_HSOTR_REGOFFSET)

/* Mask for OPAMP power mode into control register */
#define OPAMP_POWERMODE_CSR_BIT_MASK        (OPAMP_CSR_OPAHSM)

/* Internal mask for OPAMP trimming of transistors differential pair NMOS     */
/* or PMOS.                                                                   */
/* To select into literal LL_OPAMP_TRIMMING_x the relevant bits for:          */
/* - OPAMP trimming selection of transistors differential pair                */
/* - OPAMP trimming values of transistors differential pair                   */
#define OPAMP_TRIMMING_SELECT_MASK          0x00030000U
#define OPAMP_TRIMMING_VALUE_MASK           (OPAMP_OTR_TRIMOFFSETP | OPAMP_OTR_TRIMOFFSETN)

/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @defgroup OPAMP_LL_Private_Macros OPAMP Private Macros
  * @{
  */

/**
  * @brief  Driver macro reserved for internal use: set a pointer to
  *         a register from a register basis from which an offset
  *         is applied.
  * @param  __REG__ Register basis from which the offset is applied.
  * @param  __REG_OFFSET__ Offset to be applied (unit: number of registers).
  * @retval Register address
*/
#define __OPAMP_PTR_REG_OFFSET(__REG__, __REG_OFFSET__)                        \
 ((__IO uint32_t *)((uint32_t) ((uint32_t)(&(__REG__)) + ((__REG_OFFSET__) << 2U))))



/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup OPAMP_LL_ES_INIT OPAMP Exported Init structure
  * @{
  */

/**
  * @brief  Structure definition of some features of OPAMP instance.
  */
typedef struct
{
  uint32_t PowerMode;                   /*!< Set OPAMP power mode.
                                             This parameter can be a value of @ref OPAMP_LL_EC_POWER_MODE

                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetPowerMode(). */


  uint32_t FunctionalMode;              /*!< Set OPAMP functional mode by setting internal connections: OPAMP operation in standalone, follower, ...
                                             This parameter can be a value of @ref OPAMP_LL_EC_FUNCTIONAL_MODE
                                             @note If OPAMP is configured in mode PGA, the gain can be configured using function @ref LL_OPAMP_SetPGAGain().

                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetFunctionalMode(). */

  uint32_t InputNonInverting;           /*!< Set OPAMP input non-inverting connection.
                                             This parameter can be a value of @ref OPAMP_LL_EC_INPUT_NONINVERTING

                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetInputNonInverting(). */

  uint32_t InputInverting;              /*!< Set OPAMP inverting input connection.
                                             This parameter can be a value of @ref OPAMP_LL_EC_INPUT_INVERTING
                                             @note OPAMP inverting input is used with OPAMP in mode standalone or PGA with external capacitors for filtering circuit. Otherwise (OPAMP in mode follower), OPAMP inverting input is not used (not connected to GPIO pin), this parameter is discarded.

                                             This feature can be modified afterwards using unitary function @ref LL_OPAMP_SetInputInverting(). */

} LL_OPAMP_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Constants OPAMP Exported Constants
  * @{
  */

/** @defgroup OPAMP_LL_EC_MODE OPAMP mode calibration or functional.
  * @{
  */
#define LL_OPAMP_MODE_FUNCTIONAL        0x00000000U                               /*!< OPAMP functional mode */
#define LL_OPAMP_MODE_CALIBRATION       (OPAMP_CSR_CALON)                           /*!< OPAMP calibration mode */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_FUNCTIONAL_MODE OPAMP functional mode
  * @{
  */
#define LL_OPAMP_MODE_STANDALONE        0x00000000U                                                 /*!< OPAMP functional mode, OPAMP operation in standalone */
#define LL_OPAMP_MODE_FOLLOWER          (OPAMP_CSR_VMSEL_1 | OPAMP_CSR_VMSEL_0)                       /*!< OPAMP functional mode, OPAMP operation in follower */
#define LL_OPAMP_MODE_PGA               (OPAMP_CSR_VMSEL_1)                                           /*!< OPAMP functional mode, OPAMP operation in PGA */
#define LL_OPAMP_MODE_PGA_IO0           (OPAMP_CSR_PGGAIN_2|OPAMP_CSR_VMSEL_1)                        /*!< In PGA mode, the inverting input is connected to VINM0 for filtering */
#define LL_OPAMP_MODE_PGA_IO0_BIAS      (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_VMSEL_1)                        /*!< In PGA mode, the inverting input is connected to VINM0
                                                                                                          - Input signal on VINM0, bias on VINPx: negative gain
                                                                                                          - Bias on VINM0, input signal on VINPx: positive gain */
#define LL_OPAMP_MODE_PGA_IO0_IO1_BIAS  (OPAMP_CSR_PGGAIN_3|OPAMP_CSR_PGGAIN_2|OPAMP_CSR_VMSEL_1)    /*!< In PGA mode, the inverting input is connected to VINM0
                                                                                                          - Input signal on VINM0, bias on VINPx: negative gain
                                                                                                          - Bias on VINM0, input signal on VINPx: positive gain
                                                                                                          And VINM1 is connected too for filtering */

/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_MODE_PGA_GAIN OPAMP PGA gain (relevant when OPAMP is in functional mode PGA)
  * @note Gain sign:
  *         - is positive if the @ref OPAMP_LL_EC_FUNCTIONAL_MODE configuration is
  *           @ref LL_OPAMP_MODE_PGA or LL_OPAMP_MODE_PGA_IO0
  *         - may be positive or negative if the @ref OPAMP_LL_EC_FUNCTIONAL_MODE configuration is
  *           @ref LL_OPAMP_MODE_PGA_IO0_BIAS or LL_OPAMP_MODE_PGA_IO0_IO1_BIAS
  *       see @ref OPAMP_LL_EC_FUNCTIONAL_MODE for more details
  * @{
  */
#define LL_OPAMP_PGA_GAIN_2_OR_MINUS_1             0x00000000U                                                    /*!< OPAMP PGA gain 2  or -1  */
#define LL_OPAMP_PGA_GAIN_4_OR_MINUS_3             (                                          OPAMP_CSR_PGGAIN_0) /*!< OPAMP PGA gain 4  or -3  */
#define LL_OPAMP_PGA_GAIN_8_OR_MINUS_7             (                     OPAMP_CSR_PGGAIN_1                     ) /*!< OPAMP PGA gain 8  or -7  */
#define LL_OPAMP_PGA_GAIN_16_OR_MINUS_15           (                     OPAMP_CSR_PGGAIN_1 | OPAMP_CSR_PGGAIN_0) /*!< OPAMP PGA gain 16 or -15 */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_INPUT_NONINVERTING OPAMP input non-inverting
  * @{
  */
#define LL_OPAMP_INPUT_NONINVERT_IO0         0x00000000U           /*!< OPAMP non inverting input connected to I/O VINP0
                                                                        (PB0  for OPAMP1, PE9  for OPAMP2)
                                                                        Note: On this STM32 serie, all OPAMPx are not available on all devices. Refer to device datasheet for more details */
#define LL_OPAMP_INPUT_NONINVERT_DAC         OPAMP_CSR_VPSEL_0     /*!< OPAMP non inverting input connected internally to DAC channel
                                                                        (DAC1_CH1 for OPAMP1, DAC1_CH2  for OPAMP2)
                                                                        Note: On this STM32 serie, all OPAMPx are not available on all devices. Refer to device datasheet for more details */
#if defined(DAC2)
#define LL_OPAMP_INPUT_NONINVERT_DAC2        OPAMP_CSR_VPSEL_1     /*!< OPAMP non inverting input connected internally to DAC2 channel
                                                                        (DAC3 only for OPAMP2)*/
#endif /* DAC2 */

/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_INPUT_INVERTING OPAMP input inverting
  * @note OPAMP inverting input is used with OPAMP in mode standalone or PGA with negative gain or bias.
  *       Otherwise (OPAMP in mode follower), OPAMP inverting input is not used (not connected to GPIO pin).
  * @{
  */
#define LL_OPAMP_INPUT_INVERT_IO0         0x00000000U              /*!< OPAMP inverting input connected to I/O VINM0
                                                                        (PC5  for OPAMP1, PE8  for OPAMP2)
                                                                        Note: On this STM32 serie, all OPAMPx are not available on all devices. Refer to device datasheet for more details */
#define LL_OPAMP_INPUT_INVERT_IO1         OPAMP_CSR_VMSEL_0        /*!< OPAMP inverting input connected to I/0 VINM1
                                                                        (PA7  for OPAMP1, PG1  for OPAMP2)
                                                                        Note: On this STM32 serie, all OPAMPx are not available on all devices. Refer to device datasheet for more details */
#define LL_OPAMP_INPUT_INVERT_CONNECT_NO  OPAMP_CSR_VMSEL_1        /*!< OPAMP inverting input not externally connected (intended for OPAMP in mode follower or PGA with positive gain without bias).
                                                                        Note: On this STM32 serie, this literal include cases of value 0x11 for mode follower and value 0x10 for mode PGA. */
/**
  * @}
  */



/** @defgroup OPAMP_LL_EC_POWER_MODE OPAMP PowerMode
  * @{
  */
#define LL_OPAMP_POWERMODE_NORMAL        (OPAMP_POWERMODE_OTR_REGOFFSET)                            /*!< OPAMP output in normal mode */
#define LL_OPAMP_POWERMODE_HIGHSPEED     (OPAMP_POWERMODE_HSOTR_REGOFFSET | OPAMP_CSR_OPAHSM)       /*!< OPAMP output in highspeed mode */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_TRIMMING_MODE OPAMP trimming mode
  * @{
  */
#define LL_OPAMP_TRIMMING_FACTORY       0x00000000U             /*!< OPAMP trimming factors set to factory values */
#define LL_OPAMP_TRIMMING_USER          OPAMP_CSR_USERTRIM      /*!< OPAMP trimming factors set to user values */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_TRIMMING_TRANSISTORS_DIFF_PAIR OPAMP trimming of transistors differential pair NMOS or PMOS
  * @{
  */
#define LL_OPAMP_TRIMMING_NMOS_VREF_90PC_VDDA  (OPAMP_OTR_TRIMOFFSETN | ((OPAMP_CSR_CALSEL_1 | OPAMP_CSR_CALSEL_0) << 4)) /*!< OPAMP trimming of transistors differential pair NMOS (internal reference voltage set to 0.9*Vdda). Default parameters to be used for calibration using two trimming steps (one with each transistors differential pair NMOS and PMOS). */
#define LL_OPAMP_TRIMMING_NMOS_VREF_50PC_VDDA  (OPAMP_OTR_TRIMOFFSETN                      | (OPAMP_CSR_CALSEL_1 << 4))   /*!< OPAMP trimming of transistors differential pair NMOS (internal reference voltage set to 0.5*Vdda). */
#define LL_OPAMP_TRIMMING_PMOS_VREF_10PC_VDDA  (OPAMP_OTR_TRIMOFFSETP                      | (OPAMP_CSR_CALSEL_0 << 4))   /*!< OPAMP trimming of transistors differential pair PMOS (internal reference voltage set to 0.1*Vdda). Default parameters to be used for calibration using two trimming steps (one with each transistors differential pair NMOS and PMOS). */
#define LL_OPAMP_TRIMMING_PMOS_VREF_3_3PC_VDDA (OPAMP_OTR_TRIMOFFSETP                                          )          /*!< OPAMP trimming of transistors differential pair PMOS (internal reference voltage set to 0.33*Vdda). */
#define LL_OPAMP_TRIMMING_NMOS                 (LL_OPAMP_TRIMMING_NMOS_VREF_90PC_VDDA)                                    /*!< OPAMP trimming of transistors differential pair NMOS (internal reference voltage set to 0.9*Vdda). Default parameters to be used for calibration using two trimming steps (one with each transistors differential pair NMOS and PMOS). */
#define LL_OPAMP_TRIMMING_PMOS                 (LL_OPAMP_TRIMMING_PMOS_VREF_10PC_VDDA)                                    /*!< OPAMP trimming of transistors differential pair PMOS (internal reference voltage set to 0.1*Vdda). Default parameters to be used for calibration using two trimming steps (one with each transistors differential pair NMOS and PMOS). */
/**
  * @}
  */

/** @defgroup OPAMP_LL_EC_HW_DELAYS  Definitions of OPAMP hardware constraints delays
  * @note   Only OPAMP IP HW delays are defined in OPAMP LL driver driver,
  *         not timeout values.
  *         For details on delays values, refer to descriptions in source code
  *         above each literal definition.
  * @{
  */

/* Delay for OPAMP startup time (transition from state disable to enable).    */
/* Note: OPAMP startup time depends on board application environment:         */
/*       impedance connected to OPAMP output.                                 */
/*       The delay below is specified under conditions:                       */
/*        - OPAMP in functional mode follower                                 */
/*        - load impedance of 4kOhm (min), 50pF (max)                         */
/* Literal set to maximum value (refer to device datasheet,                   */
/* parameter "tWAKEUP").                                                      */
/* Unit: us                                                                   */
#define LL_OPAMP_DELAY_STARTUP_US         (3U)  /*!< Delay for OPAMP startup time */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Macros OPAMP Exported Macros
  * @{
  */
/** @defgroup OPAMP_LL_EM_WRITE_READ Common write and read registers macro
  * @{
  */
/**
  * @brief  Write a value in OPAMP LL_OPAMP_GetPowerModeregister
  * @param  __INSTANCE__ OPAMP Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_OPAMP_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in OPAMP register
  * @param  __INSTANCE__ OPAMP Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_OPAMP_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */
/** @defgroup OPAMP_LL_EM_HELPER_MACRO OPAMP helper macro
  * @{
  */

/**
  * @brief  Helper macro to select the OPAMP common instance
  *         to which is belonging the selected OPAMP instance.
  * @note   OPAMP common register instance can be used to
  *         set parameters common to several OPAMP instances.
  *         Refer to functions having argument "OPAMPxy_COMMON" as parameter.
  * @param  __OPAMPx__ OPAMP instance
  * @retval OPAMP common instance
  */
#if defined(OPAMP1) && defined(OPAMP2)
#define __LL_OPAMP_COMMON_INSTANCE(__OPAMPx__)                                 \
  (OPAMP12_COMMON)
#endif

/**
  * @brief  Helper macro to check if all OPAMP instances sharing the same
  *         OPAMP common instance are disabled.
  * @note   This check is required by functions with setting conditioned to
  *         OPAMP state:
  *         All OPAMP instances of the OPAMP common group must be disabled.
  *         Refer to functions having argument "OPAMPxy_COMMON" as parameter.
  * @retval 0: All OPAMP instances sharing the same OPAMP common instance
  *            are disabled.
  *         1: At least one OPAMP instance sharing the same OPAMP common instance
  *            is enabled
  */
#if defined(OPAMP1) && defined(OPAMP2)
#define __LL_OPAMP_IS_ENABLED_ALL_COMMON_INSTANCE()                            \
  (LL_OPAMP_IsEnabled(OPAMP1) |                                                \
   LL_OPAMP_IsEnabled(OPAMP2)  )
#endif

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup OPAMP_LL_Exported_Functions OPAMP Exported Functions
  * @{
  */

/** @defgroup OPAMP_LL_EF_CONFIGURATION_OPAMP_INSTANCE Configuration of OPAMP hierarchical scope: OPAMP instance
  * @{
  */

/**
  * @brief  Set OPAMP mode calibration or functional.
  * @note   OPAMP mode corresponds to functional or calibration mode:
  *          - functional mode: OPAMP operation in standalone, follower, ...
  *            Set functional mode using function
  *            @ref LL_OPAMP_SetFunctionalMode().
  *          - calibration mode: offset calibration of the selected
  *            transistors differential pair NMOS or PMOS.
  * @rmtoll CSR      CALON          LL_OPAMP_SetMode
  * @param  OPAMPx OPAMP instance
  * @param  Mode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_FUNCTIONAL
  *         @arg @ref LL_OPAMP_MODE_CALIBRATION
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetMode(OPAMP_TypeDef *OPAMPx, uint32_t Mode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_CALON, Mode);
}

/**
  * @brief  Get OPAMP mode calibration or functional.
  * @note   OPAMP mode corresponds to functional or calibration mode:
  *          - functional mode: OPAMP operation in standalone, follower, ...
  *            Set functional mode using function
  *            @ref LL_OPAMP_SetFunctionalMode().
  *          - calibration mode: offset calibration of the selected
  *            transistors differential pair NMOS or PMOS.
  * @rmtoll CSR      CALON          LL_OPAMP_GetMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_FUNCTIONAL
  *         @arg @ref LL_OPAMP_MODE_CALIBRATION
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALON));
}

/**
  * @brief  Set OPAMP functional mode by setting internal connections.
  *         OPAMP operation in standalone, follower, ...
  * @note   This function reset bit of calibration mode to ensure
  *         to be in functional mode, in order to have OPAMP parameters
  *         (inputs selection, ...) set with the corresponding OPAMP mode
  *         to be effective.
  * @rmtoll CSR      VMSEL          LL_OPAMP_SetFunctionalMode
  * @param  OPAMPx OPAMP instance
  * @param  FunctionalMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_STANDALONE
  *         @arg @ref LL_OPAMP_MODE_FOLLOWER
  *         @arg @ref LL_OPAMP_MODE_PGA
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0_BIAS
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0_IO1_BIAS
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetFunctionalMode(OPAMP_TypeDef *OPAMPx, uint32_t FunctionalMode)
{
  /* Note: Bit OPAMP_CSR_CALON reset to ensure to be in functional mode */
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_PGGAIN_3 | OPAMP_CSR_PGGAIN_2 | OPAMP_CSR_VMSEL | OPAMP_CSR_CALON, FunctionalMode);
}

/**
  * @brief  Get OPAMP functional mode from setting of internal connections.
  *         OPAMP operation in standalone, follower, ...
  * @rmtoll CSR      VMSEL          LL_OPAMP_GetFunctionalMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_MODE_STANDALONE
  *         @arg @ref LL_OPAMP_MODE_FOLLOWER
  *         @arg @ref LL_OPAMP_MODE_PGA
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0_BIAS
  *         @arg @ref LL_OPAMP_MODE_PGA_IO0_IO1_BIAS
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetFunctionalMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_PGGAIN_3 | OPAMP_CSR_PGGAIN_2 | OPAMP_CSR_VMSEL));
}

/**
  * @brief  Set OPAMP PGA gain.
  * @note   Preliminarily, OPAMP must be set in mode PGA
  *         using function @ref LL_OPAMP_SetFunctionalMode().
  * @rmtoll CSR      PGGAIN         LL_OPAMP_SetPGAGain
  * @param  OPAMPx OPAMP instance
  * @param  PGAGain This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_PGA_GAIN_2_OR_MINUS_1
  *         @arg @ref LL_OPAMP_PGA_GAIN_4_OR_MINUS_3
  *         @arg @ref LL_OPAMP_PGA_GAIN_8_OR_MINUS_7
  *         @arg @ref LL_OPAMP_PGA_GAIN_16_OR_MINUS_15
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetPGAGain(OPAMP_TypeDef *OPAMPx, uint32_t PGAGain)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_PGGAIN_1 | OPAMP_CSR_PGGAIN_0, PGAGain);
}

/**
  * @brief  Get OPAMP PGA gain.
  * @note   Preliminarily, OPAMP must be set in mode PGA
  *         using function @ref LL_OPAMP_SetFunctionalMode().
  * @rmtoll CSR      PGGAIN         LL_OPAMP_GetPGAGain
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_PGA_GAIN_2_OR_MINUS_1
  *         @arg @ref LL_OPAMP_PGA_GAIN_4_OR_MINUS_3
  *         @arg @ref LL_OPAMP_PGA_GAIN_8_OR_MINUS_7
  *         @arg @ref LL_OPAMP_PGA_GAIN_16_OR_MINUS_15
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetPGAGain(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_PGGAIN_1 | OPAMP_CSR_PGGAIN_0));
}

/**
  * @brief  Set OPAMP power mode normal or highspeed.
  * @note   OPAMP highspeed mode allows output stage to have a better slew rate.
  * @rmtoll CSR      OPAHSM     LL_OPAMP_SetPowerMode
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_HIGHSPEED
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetPowerMode(OPAMP_TypeDef *OPAMPx, uint32_t PowerMode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_OPAHSM, (PowerMode & OPAMP_POWERMODE_CSR_BIT_MASK));
}

/**
  * @brief  Get OPAMP power mode normal or highspeed.
  * @note   OPAMP highspeed mode allows output stage to have a better slew rate.
  * @rmtoll CSR      OPAHSM     LL_OPAMP_GetPowerMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_HIGHSPEED
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetPowerMode(OPAMP_TypeDef *OPAMPx)
{
  register uint32_t power_mode = (READ_BIT(OPAMPx->CSR, OPAMP_CSR_OPAHSM));

  return (uint32_t)(power_mode | (power_mode >> (OPAMP_CSR_OPAHSM_Pos)));
}
/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_CONFIGURATION_INPUTS Configuration of OPAMP inputs
  * @{
  */

/**
  * @brief  Set OPAMP non-inverting input connection.
  * @rmtoll CSR      VPSEL          LL_OPAMP_SetInputNonInverting
  * @param  OPAMPx OPAMP instance
  * @param  InputNonInverting This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC2 (Only for OPAMP2)
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetInputNonInverting(OPAMP_TypeDef *OPAMPx, uint32_t InputNonInverting)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_VPSEL, InputNonInverting);
}

/**
  * @brief  Get OPAMP non-inverting input connection.
  * @rmtoll CSR      VPSEL          LL_OPAMP_GetInputNonInverting
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC
  *         @arg @ref LL_OPAMP_INPUT_NONINVERT_DAC2 (Only for OPAMP2)
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetInputNonInverting(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_VPSEL));
}

/**
  * @brief  Set OPAMP inverting input connection.
  * @note   OPAMP inverting input is used with OPAMP in mode standalone
  *         or PGA with external capacitors for filtering circuit.
  *         Otherwise (OPAMP in mode follower), OPAMP inverting input
  *         is not used (not connected to GPIO pin).
  * @rmtoll CSR      VMSEL          LL_OPAMP_SetInputInverting
  * @param  OPAMPx OPAMP instance
  * @param  InputInverting This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO1
  *         @arg @ref LL_OPAMP_INPUT_INVERT_CONNECT_NO
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetInputInverting(OPAMP_TypeDef *OPAMPx, uint32_t InputInverting)
{
  /* Manage cases of OPAMP inverting input not connected (0x10 and 0x11)      */
  /* to not modify OPAMP mode follower or PGA.                                */
  /* Bit OPAMP_CSR_VMSEL_1 is set by OPAMP mode (follower, PGA). */
  MODIFY_REG(OPAMPx->CSR, (~(InputInverting >> 1)) & OPAMP_CSR_VMSEL_0, InputInverting);
}

/**
  * @brief  Get OPAMP inverting input connection.
  * @rmtoll CSR      VMSEL          LL_OPAMP_GetInputInverting
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO0
  *         @arg @ref LL_OPAMP_INPUT_INVERT_IO1
  *         @arg @ref LL_OPAMP_INPUT_INVERT_CONNECT_NO
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetInputInverting(OPAMP_TypeDef *OPAMPx)
{
  register uint32_t input_inverting = READ_BIT(OPAMPx->CSR, OPAMP_CSR_VMSEL);

  /* Manage cases 0x10 and 0x11 to return the same value: OPAMP inverting     */
  /* input not connected.                                                     */
  return (input_inverting & ~((input_inverting >> 1) & OPAMP_CSR_VMSEL_0));
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_OPAMP_TRIMMING Configuration and operation of OPAMP trimming
  * @{
  */

/**
  * @brief  Set OPAMP trimming mode.
  * @rmtoll CSR      USERTRIM       LL_OPAMP_SetTrimmingMode
  * @param  OPAMPx OPAMP instance
  * @param  TrimmingMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_FACTORY
  *         @arg @ref LL_OPAMP_TRIMMING_USER
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetTrimmingMode(OPAMP_TypeDef *OPAMPx, uint32_t TrimmingMode)
{
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_USERTRIM, TrimmingMode);
}

/**
  * @brief  Get OPAMP trimming mode.
  * @rmtoll CSR      USERTRIM       LL_OPAMP_GetTrimmingMode
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_FACTORY
  *         @arg @ref LL_OPAMP_TRIMMING_USER
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetTrimmingMode(OPAMP_TypeDef *OPAMPx)
{
  return (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_USERTRIM));
}

/**
  * @brief  Set OPAMP offset to calibrate the selected transistors
  *         differential pair NMOS or PMOS.
  * @note   Preliminarily, OPAMP must be set in mode calibration
  *         using function @ref LL_OPAMP_SetMode().
  * @rmtoll CSR      CALSEL         LL_OPAMP_SetCalibrationSelection
  * @param  OPAMPx OPAMP instance
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS            (1)
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS            (1)
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS_VREF_50PC_VDDA
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS_VREF_3_3PC_VDDA
  *
  *         (1) Default parameters to be used for calibration
  *             using two trimming steps (one with each transistors differential
  *             pair NMOS and PMOS)
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetCalibrationSelection(OPAMP_TypeDef *OPAMPx, uint32_t TransistorsDiffPair)
{
  /* Parameter used with mask "OPAMP_TRIMMING_SELECT_MASK" because            */
  /* containing other bits reserved for other purpose.                        */
  MODIFY_REG(OPAMPx->CSR, OPAMP_CSR_CALSEL, ((TransistorsDiffPair & OPAMP_TRIMMING_SELECT_MASK) >> 4));
}

/**
  * @brief  Get OPAMP offset to calibrate the selected transistors
  *         differential pair NMOS or PMOS.
  * @note   Preliminarily, OPAMP must be set in mode calibration
  *         using function @ref LL_OPAMP_SetMode().
  * @rmtoll CSR      CALSEL         LL_OPAMP_GetCalibrationSelection
  * @param  OPAMPx OPAMP instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS            (1)
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS            (1)
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS_VREF_50PC_VDDA
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS_VREF_3_3PC_VDDA
  *
  *         (1) Default parameters to be used for calibration
  *             using two trimming steps (one with each transistors differential
  *             pair NMOS and PMOS)
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetCalibrationSelection(OPAMP_TypeDef *OPAMPx)
{
  register uint32_t CalibrationSelection = (uint32_t)(READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALSEL));

  return (uint32_t)((CalibrationSelection << 4)|
          (((CalibrationSelection & OPAMP_CSR_CALSEL_1) == 0UL) ? OPAMP_OTR_TRIMOFFSETN : OPAMP_OTR_TRIMOFFSETP));
}

/**
  * @brief  Get OPAMP calibration result of toggling output.
  * @note   This functions returns:
  *         0 if OPAMP calibration output is reset
  *         1 if OPAMP calibration output is set
  * @rmtoll CSR      OUTCAL         LL_OPAMP_IsCalibrationOutputSet
  * @param  OPAMPx OPAMP instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPAMP_IsCalibrationOutputSet(OPAMP_TypeDef *OPAMPx)
{
  return ((READ_BIT(OPAMPx->CSR, OPAMP_CSR_CALOUT) == OPAMP_CSR_CALOUT)?1UL:0UL);
}

/**
  * @brief  Set OPAMP trimming factor for the selected transistors
  *         differential pair NMOS or PMOS, corresponding to the selected
  *         power mode.
  * @rmtoll OTR      TRIMOFFSETN    LL_OPAMP_SetTrimmingValue\n
  *         OTR      TRIMOFFSETP    LL_OPAMP_SetTrimmingValue\n
  *         HSOTR    TRIMHSOFFSETN  LL_OPAMP_SetTrimmingValue\n
  *         HSOTR    TRIMHSOFFSETP  LL_OPAMP_SetTrimmingValue
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_HIGHSPEED
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  * @param  TrimmingValue 0x00...0x1F
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_SetTrimmingValue(OPAMP_TypeDef* OPAMPx, uint32_t PowerMode, uint32_t TransistorsDiffPair, uint32_t TrimmingValue)
{
  register __IO uint32_t *preg = __OPAMP_PTR_REG_OFFSET(OPAMPx->OTR, (PowerMode & OPAMP_POWERMODE_OTR_REGOFFSET_MASK));

  /* Set bits with position in register depending on parameter                */
  /* "TransistorsDiffPair".                                                   */
  /* Parameter used with mask "OPAMP_TRIMMING_VALUE_MASK" because             */
  /* containing other bits reserved for other purpose.                        */
  MODIFY_REG(*preg,
             (TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK) << 1U,
             TrimmingValue << ((TransistorsDiffPair == LL_OPAMP_TRIMMING_NMOS) ? OPAMP_OTR_TRIMOFFSETN_Pos : OPAMP_OTR_TRIMOFFSETP_Pos));
}

/**
  * @brief  Get OPAMP trimming factor for the selected transistors
  *         differential pair NMOS or PMOS, corresponding to the selected
  *         power mode.
  * @rmtoll OTR      TRIMOFFSETN    LL_OPAMP_GetTrimmingValue\n
  *         OTR      TRIMOFFSETP    LL_OPAMP_GetTrimmingValue\n
  *         HSOTR    TRIMHSOFFSETN  LL_OPAMP_GetTrimmingValue\n
  *         HSOTR    TRIMHSOFFSETP  LL_OPAMP_GetTrimmingValue
  * @param  OPAMPx OPAMP instance
  * @param  PowerMode This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_POWERMODE_NORMAL
  *         @arg @ref LL_OPAMP_POWERMODE_HIGHSPEED
  * @param  TransistorsDiffPair This parameter can be one of the following values:
  *         @arg @ref LL_OPAMP_TRIMMING_NMOS
  *         @arg @ref LL_OPAMP_TRIMMING_PMOS
  * @retval 0x0...0x1F
  */
__STATIC_INLINE uint32_t LL_OPAMP_GetTrimmingValue(OPAMP_TypeDef* OPAMPx, uint32_t PowerMode, uint32_t TransistorsDiffPair)
{
  register const __IO uint32_t *preg = __OPAMP_PTR_REG_OFFSET(OPAMPx->OTR, (PowerMode & OPAMP_POWERMODE_OTR_REGOFFSET_MASK));

  /* Retrieve bits with position in register depending on parameter           */
  /* "TransistorsDiffPair".                                                   */
  /* Parameter used with mask "OPAMP_TRIMMING_VALUE_MASK" because             */
  /* containing other bits reserved for other purpose.                        */
  return (uint32_t)(READ_BIT(*preg, (TransistorsDiffPair & OPAMP_TRIMMING_VALUE_MASK))
                    >> ((TransistorsDiffPair == LL_OPAMP_TRIMMING_NMOS) ? OPAMP_OTR_TRIMOFFSETN_Pos : OPAMP_OTR_TRIMOFFSETP_Pos));
}

/**
  * @}
  */

/** @defgroup OPAMP_LL_EF_OPERATION Operation on OPAMP instance
  * @{
  */
/**
  * @brief  Enable OPAMP instance.
  * @note   After enable from off state, OPAMP requires a delay
  *         to fullfill wake up time specification.
  *         Refer to device datasheet, parameter "tWAKEUP".
  * @rmtoll CSR      OPAMPXEN       LL_OPAMP_Enable
  * @param  OPAMPx OPAMP instance
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_Enable(OPAMP_TypeDef *OPAMPx)
{
  SET_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN);
}

/**
  * @brief  Disable OPAMP instance.
  * @rmtoll CSR      OPAMPXEN       LL_OPAMP_Disable
  * @param  OPAMPx OPAMP instance
  * @retval None
  */
__STATIC_INLINE void LL_OPAMP_Disable(OPAMP_TypeDef *OPAMPx)
{
  CLEAR_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN);
}

/**
  * @brief  Get OPAMP instance enable state
  *         (0: OPAMP is disabled, 1: OPAMP is enabled)
  * @rmtoll CSR      OPAMPXEN       LL_OPAMP_IsEnabled
  * @param  OPAMPx OPAMP instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPAMP_IsEnabled(OPAMP_TypeDef *OPAMPx)
{
  return ((READ_BIT(OPAMPx->CSR, OPAMP_CSR_OPAMPxEN) == (OPAMP_CSR_OPAMPxEN))?1UL:0UL);
}
/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup OPAMP_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_OPAMP_DeInit(OPAMP_TypeDef *OPAMPx);
ErrorStatus LL_OPAMP_Init(OPAMP_TypeDef *OPAMPx, LL_OPAMP_InitTypeDef *OPAMP_InitStruct);
void        LL_OPAMP_StructInit(LL_OPAMP_InitTypeDef *OPAMP_InitStruct);

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

#endif /* OPAMP1 || OPAMP2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_LL_OPAMP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
